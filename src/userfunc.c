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

#ಆದರೆ defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
ಸ್ಥಿರ hashtab_T	func_hashtab;

// Used by get_func_tv()
ಸ್ಥಿರ garray_T funcargs = GA_ಖಾಲಿಗಳು;

// pointer to funccal for currently active function
ಸ್ಥಿರ funccall_T *current_funccal = ಖಾಲಿ;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
ಸ್ಥಿರ funccall_T *ಹಿಂದಿನ_funccal = ಖಾಲಿ;

ಸ್ಥಿರ ಶೂನ್ಯ funccal_unref(funccall_T *fc, ufunc_T *fp, ಪೂರ್ಣಾಂಕ force);
ಸ್ಥಿರ ಶೂನ್ಯ func_clear(ufunc_T *fp, ಪೂರ್ಣಾಂಕ force);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ func_free(ufunc_T *fp, ಪೂರ್ಣಾಂಕ force);
ಸ್ಥಿರ char_u *untrans_function_name(char_u *name);
ಸ್ಥಿರ ಶೂನ್ಯ handle_defer_one(funccall_T *funccal);
ಸ್ಥಿರ char_u *trans_function_name_ext(char_u **pp, ಪೂರ್ಣಾಂಕ *is_global, ಪೂರ್ಣಾಂಕ skip, ಪೂರ್ಣಾಂಕ flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

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
    ಹಿಂತಿರುಗಿ &func_hashtab;
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
	ಪೂರ್ಣಾಂಕ	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	ಪೂರ್ಣಾಂಕ	    is_vararg,
	ಪೂರ್ಣಾಂಕ	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		is_underscore = FALSE;

    ಆಗ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    ಆದರೆ (arg == p || ಸುರಕ್ಷಿತ_isdigit(*arg)
	    || (argtypes == ಖಾಲಿ
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	ಆದರೆ (!skip)
	    semsg(_(e_illegal_ವಾದ_str), arg);
	ಹಿಂತಿರುಗಿ arg;
    }

    // Extra checks in Vim9 script.
    ಆದರೆ (!skip && argtypes != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ c = *p;
	*p = NUL;
	ಪೂರ್ಣಾಂಕ r = check_reserved_name(arg, FALSE);
	*p = c;
	ಆದರೆ (r == FAIL)
	    ಹಿಂತಿರುಗಿ arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	ಆದರೆ (check_defined(arg, p - arg,
				   evalarg == ಖಾಲಿ ? ಖಾಲಿ : evalarg->eval_cctx,
			       eap == ಖಾಲಿ ? ಖಾಲಿ : eap->cstack, TRUE) == FAIL)
	    ಹಿಂತಿರುಗಿ arg;
    }

    ಆದರೆ (newargs != ಖಾಲಿ && ga_grow(newargs, 1) == FAIL)
	ಹಿಂತಿರುಗಿ arg;
    ಆದರೆ (newargs != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	c;
	ಪೂರ್ಣಾಂಕ	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	ಆದರೆ (arg_copy == ಖಾಲಿ)
	{
	    *p = c;
	    ಹಿಂತಿರುಗಿ arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	ಆದರೆ (argtypes == ಖಾಲಿ || !is_underscore)
	    // Check for duplicate argument name.
	    ಗಾಗಿ (i = 0; i < newargs->ga_len; ++i)
		ಆದರೆ (STRCMP(((char_u **)(newargs->ga_ದತ್ತಾಂಶ))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_ವಾದ_name_str), arg_copy);
		    vim_free(arg_copy);
		    ಹಿಂತಿರುಗಿ arg;
		}
	((char_u **)(newargs->ga_ದತ್ತಾಂಶ))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    ಆದರೆ (argtypes != ಖಾಲಿ && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != ಖಾಲಿ && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = ಖಾಲಿ;

	ಆದರೆ (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == ಖಾಲಿ ? arg : arg_copy);
	    p = skipwhite(p);
	}
	ಆದರೆ (*p == ':')
	{
	    ++p;
	    ಆದರೆ (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		ಹಿಂತಿರುಗಿ arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    ಆದರೆ (!skip)
		type = vim_strnsave(type, p - type);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_ವಾದ_type_for_str),
					    arg_copy == ಖಾಲಿ ? arg : arg_copy);
	    ಹಿಂತಿರುಗಿ arg;
	}
	ಆದರೆ (!skip)
	{
	    ಆದರೆ (type == ಖಾಲಿ && types_optional)
	    {
		// lambda arguments default to "any" type
		ಆದರೆ (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		ಇಲ್ಲದಿದ್ದರೆ
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_ದತ್ತಾಂಶ)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_ದತ್ತಾಂಶ)[arg_objm->ga_len++] = FALSE;
	}
    }

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    ಸ್ಥಿರ char_u *
get_function_ಸಾಲು(
	exarg_T		*eap,
	garray_T	*ಸಾಲುಗಳು_ಗಮ್ಯ_free,
	ಪೂರ್ಣಾಂಕ		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    ಆದರೆ (eap->ea_getline == ಖಾಲಿ)
	theline = getcmdline(':', 0L, indent, 0);
    ಇಲ್ಲದಿದ್ದರೆ
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    ಆದರೆ (theline != ಖಾಲಿ)
    {
	ಆದರೆ (ಸಾಲುಗಳು_ಗಮ್ಯ_free->ga_len > 0
		&& eap->cmdlinep != ಖಾಲಿ
		&& *eap->cmdlinep == ((char_u **)ಸಾಲುಗಳು_ಗಮ್ಯ_free->ga_ದತ್ತಾಂಶ)
						   [ಸಾಲುಗಳು_ಗಮ್ಯ_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(ಶೂನ್ಯ)ga_ಕೂಡಿಸು_string(ಸಾಲುಗಳು_ಗಮ್ಯ_free, theline);
    }

    ಹಿಂತಿರುಗಿ theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_function_ವಾದಗಳು(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    ಪೂರ್ಣಾಂಕ		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    ಪೂರ್ಣಾಂಕ		*varargs,
    garray_T	*default_ವಾದಗಳು,
    ಪೂರ್ಣಾಂಕ		skip,
    exarg_T	*eap,		// can be NULL
    ಪೂರ್ಣಾಂಕ		ಒಳ_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*ಸಾಲುಗಳು_ಗಮ್ಯ_free)
{
    ಪೂರ್ಣಾಂಕ		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		any_default = FALSE;
    char_u	*whitep = *argp;
    ಪೂರ್ಣಾಂಕ		need_ಅಭಿವ್ಯಕ್ತಿ = FALSE;

    ಆದರೆ (newargs != ಖಾಲಿ)
	ga_init2(newargs, ಗಾತ್ರ(char_u *), 3);
    ಆದರೆ (argtypes != ಖಾಲಿ)
	ga_init2(argtypes, ಗಾತ್ರ(char_u *), 3);
    ಆದರೆ (arg_objm != ಖಾಲಿ)
	ga_init2(arg_objm, ಗಾತ್ರ(int8_T), 3);
    ಆದರೆ (!skip && default_ವಾದಗಳು != ಖಾಲಿ)
	ga_init2(default_ವಾದಗಳು, ಗಾತ್ರ(char_u *), 3);

    ಆದರೆ (varargs != ಖಾಲಿ)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    ಆಗ (*p != endchar)
    {
	ಆಗ (eap != ಖಾಲಿ && eap->ea_getline != ಖಾಲಿ
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_ಸಾಲು(eap, ಸಾಲುಗಳು_ಗಮ್ಯ_free, 0,
							  GETLINE_CONCAT_CONT);

	    ಆದರೆ (theline == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	ಆದರೆ (mustend && *p != endchar)
	{
	    ಆದರೆ (!skip)
		semsg(_(e_invalid_ವಾದ_str), *argp);
	    ಹೋಗು err_ret;
	}
	ಆದರೆ (*p == endchar && !need_ಅಭಿವ್ಯಕ್ತಿ)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    ಆದರೆ (varargs != ಖಾಲಿ)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    ಆದರೆ (argtypes != ಖಾಲಿ)
	    {
		// ...name: list<type>
		ಆದರೆ (!eval_isnamec1(*p))
		{
		    ಆದರೆ (!skip)
			emsg(_(e_missing_name_after_dots));
		    ಹೋಗು err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		ಆದರೆ (p == arg)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    ಆಗ (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    ಆದರೆ (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_ವಿಧಾನ_str), arg);
		*argend = c;
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		ಆದರೆ (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_ಮೌಲ್ಯ_must_be_vnone_str), p);
		    ಹೋಗು err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		ಆದರೆ (ga_grow(default_ವಾದಗಳು, 1) == FAIL)
		    ಹೋಗು err_ret;

		char_u *ಅಭಿವ್ಯಕ್ತಿ = vim_strnsave((char_u *)"v:none", 6);
		ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ == ಖಾಲಿ)
		    ಹೋಗು err_ret;
		((char_u **)(default_ವಾದಗಳು->ga_ದತ್ತಾಂಶ))
						 [default_ವಾದಗಳು->ga_len] = ಅಭಿವ್ಯಕ್ತಿ;
		default_ವಾದಗಳು->ga_len++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (any_default)
	    {
		emsg(_(e_non_default_ವಾದ_follows_default_ವಾದ));
		ಹೋಗು err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    ಆದರೆ (newargs != ಖಾಲಿ && ga_grow(newargs, 1) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (newargs != ಖಾಲಿ)
	    {
		((char_u **)(newargs->ga_ದತ್ತಾಂಶ))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		ಆದರೆ (argtypes != ಖಾಲಿ && ga_grow(argtypes, 1) == OK
			    && arg_objm != ಖಾಲಿ && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_ದತ್ತಾಂಶ)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_ದತ್ತಾಂಶ)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    ಆದರೆ (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			ಪೂರ್ಣಾಂಕ len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			ಆದರೆ (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			ಆದರೆ (assignment != ಖಾಲಿ)
			{
			    c = *argend;
			    *argend = NUL;
			    ಆದರೆ (any_default)
				vim_snprintf((ಅಕ್ಷರ *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_ವಾದಗಳು->ga_len - 1, arg, arg);
			    ಇಲ್ಲದಿದ್ದರೆ
				vim_snprintf((ಅಕ್ಷರ *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_ದತ್ತಾಂಶ))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    ಆದರೆ (*p == ',')
		++p;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    ಆದರೆ (p == arg)
		ನಿಲ್ಲಿಸು;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    ಆದರೆ (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_ವಾದಗಳು != ಖಾಲಿ)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *ಅಭಿವ್ಯಕ್ತಿ = p;
		ಆದರೆ (eval1(&p, &rettv, ಖಾಲಿ) != FAIL)
		{
		    ಆದರೆ (!skip)
		    {
			ಆದರೆ (ga_grow(default_ವಾದಗಳು, 1) == FAIL)
			    ಹೋಗು err_ret;

			ಆದರೆ (need_ಅಭಿವ್ಯಕ್ತಿ)
			    need_ಅಭಿವ್ಯಕ್ತಿ = FALSE;
			// trim trailing whitespace
			ಆಗ (p > ಅಭಿವ್ಯಕ್ತಿ && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			ಅಭಿವ್ಯಕ್ತಿ = vim_strsave(ಅಭಿವ್ಯಕ್ತಿ);
			ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ == ಖಾಲಿ)
			{
			    *p = c;
			    ಹೋಗು err_ret;
			}
			((char_u **)(default_ವಾದಗಳು->ga_ದತ್ತಾಂಶ))
						 [default_ವಾದಗಳು->ga_len] = ಅಭಿವ್ಯಕ್ತಿ;
			default_ವಾದಗಳು->ga_len++;
			*p = c;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    mustend = TRUE;
		    ಆದರೆ (*skipwhite(p) == NUL)
			need_ಅಭಿವ್ಯಕ್ತಿ = TRUE;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (any_default)
	    {
		emsg(_(e_non_default_ವಾದ_follows_default_ವಾದ));
		ಹೋಗು err_ret;
	    }

	    ಆದರೆ (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		ಆದರೆ (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    ಹೋಗು err_ret;
		}
		p = skipwhite(p);
	    }
	    ಆದರೆ (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		ಆದರೆ (!skip && argtypes != ಖಾಲಿ
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    ಹೋಗು err_ret;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    ಆದರೆ (*p != endchar)
	ಹೋಗು err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    ಹಿಂತಿರುಗಿ OK;

err_ret:
    ಆದರೆ (newargs != ಖಾಲಿ)
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(newargs);
    ಆದರೆ (!skip && default_ವಾದಗಳು != ಖಾಲಿ)
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(default_ವಾದಗಳು);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_ವಾದ_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    ಪೂರ್ಣಾಂಕ		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    ಪೂರ್ಣಾಂಕ		obj_member_ಎಣಿಕೆ,
    cctx_T	*cctx)
{
    ಪೂರ್ಣಾಂಕ len = 0;

    ga_init2(&fp->uf_type_list, ಗಾತ್ರ(type_T *), 10);
    ಆದರೆ (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	ಆದರೆ (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	ಆದರೆ (fp->uf_arg_types != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ	i;
	    type_T	*type;

	    ಗಾಗಿ (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_ದತ್ತಾಂಶ)[i];

		ಆದರೆ (p == ಖಾಲಿ)
		    // will get the type from the default value
		    type = &t_unknown;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (arg_objm != ಖಾಲಿ && ((int8_T *)arg_objm->ga_ದತ್ತಾಂಶ)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_ವಾದಗಳು.ga_ದತ್ತಾಂಶ)[i];

			type = &t_any;
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ om = 0; om < obj_member_ಎಣಿಕೆ; ++om)
			{
			    ಆದರೆ (obj_members != ಖಾಲಿ
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				ನಿಲ್ಲಿಸು;
			    }
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		ಆದರೆ (type == ಖಾಲಿ || !valid_declaration_type(type))
		    ಹಿಂತಿರುಗಿ FAIL;
		fp->uf_arg_types[i] = type;
		ಆದರೆ (i < fp->uf_ವಾದಗಳು.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_ವಾದಗಳು.ga_ದತ್ತಾಂಶ)[i];
		    ಆದರೆ (obj_members != ಖಾಲಿ && *name == '_')
			// protected object method
			name++;

		    ಆದರೆ (var_wrong_func_name(name, TRUE))
			ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	}
    }

    ಆದರೆ (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_ವಾದಗಳು.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_ವಾದಗಳು.ga_ದತ್ತಾಂಶ)[fp->uf_ವಾದಗಳು.ga_len];
	((char_u **)fp->uf_ವಾದಗಳು.ga_ದತ್ತಾಂಶ)[fp->uf_ವಾದಗಳು.ga_len] = ಖಾಲಿ;
	p = ((char_u **)argtypes->ga_ದತ್ತಾಂಶ)[len];
	ಆದರೆ (p == ಖಾಲಿ)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    ಆದರೆ (fp->uf_va_type != ಖಾಲಿ && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_ದತ್ತಾಂಶ)[len]);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಆದರೆ (fp->uf_va_type == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    ಆದರೆ (ret_type == ಖಾಲಿ)
	fp->uf_ret_type = &t_void;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	ಆದರೆ (fp->uf_ret_type == ಖಾಲಿ)
	{
	    fp->uf_ret_type = &t_void;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ರಿಜಿಸ್ಟರ್_ಆವರಣ(ufunc_T *fp)
{
    ಆದರೆ (fp->uf_scoped == current_funccal)
	// no change
	ಹಿಂತಿರುಗಿ OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    ಆದರೆ (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_ದತ್ತಾಂಶ)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    ಹಿಂತಿರುಗಿ OK;
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

    ಆದರೆ (name[0] != K_SPECIAL)
	ಹಿಂತಿರುಗಿ name;
    len = STRLEN(name);
    ಆದರೆ (len + 3 > bufsize)
	ಹಿಂತಿರುಗಿ name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    ಹಿಂತಿರುಗಿ buf;
}

ಸ್ಥಿರ char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	lambda_no = 0;
    ಪೂರ್ಣಾಂಕ		n;
    string_T	ret;

    n = vim_snprintf((ಅಕ್ಷರ *)lambda_name, ಗಾತ್ರ(lambda_name), "<lambda>%d", ++lambda_no);
    ಆದರೆ (n < 1)
	ret.ಉದ್ದಳತೆ = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n >= (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(lambda_name))
	ret.ಉದ್ದಳತೆ = ಗಾತ್ರ(lambda_name) - 1;
    ಇಲ್ಲದಿದ್ದರೆ
	ret.ಉದ್ದಳತೆ = (size_t)n;

    ret.string = lambda_name;
    ಹಿಂತಿರುಗಿ ret;
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
    fp = alloc_clear(len < ಗಾತ್ರ(ufunc_T) ? ಗಾತ್ರ(ufunc_T) : len);
    ಆದರೆ (fp != ಖಾಲಿ)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((ಶೂನ್ಯ *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	ಆದರೆ (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    ಆದರೆ (fp->uf_name_exp != ಖಾಲಿ)
		vim_snprintf((ಅಕ್ಷರ *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    ಹಿಂತಿರುಗಿ fp;
}

#ಆದರೆ defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
ರಿಜಿಸ್ಟರ್_cfunc(cfunc_T cb, cfunc_free_T cb_free, ಶೂನ್ಯ *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.ಉದ್ದಳತೆ);
    ಆದರೆ (fp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    fp->uf_def_ಸ್ಥಿತಿ = UF_NOT_COMPILED;
    fp->uf_refcount = 1;
    fp->uf_varargs = TRUE;
    fp->uf_flags = FC_CFUNC | FC_LAMBDA;
    fp->uf_calls = 0;
    fp->uf_script_ctx = current_sctx;
    fp->uf_cb = cb;
    fp->uf_cb_free = cb_free;
    fp->uf_cb_state = state;

    hash_ಕೂಡಿಸು(&func_hashtab, UF2HIKEY(fp), "add C function");

    ಹಿಂತಿರುಗಿ name.string;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    ಸ್ಥಿರ char_u *
skip_arrow(
	char_u	*ಆರಂಭ,
	ಪೂರ್ಣಾಂಕ	equal_arrow,
	char_u	**ret_type,
	ಪೂರ್ಣಾಂಕ	*white_error)
{
    char_u  *s = ಆರಂಭ;
    char_u  *bef = ಆರಂಭ - 2; // "start" points to > of ->

    ಆದರೆ (equal_arrow)
    {
	ಆದರೆ (*s == ':')
	{
	    ಆದರೆ (white_error != ಖಾಲಿ && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    ಆದರೆ (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
	bef = s;
	s = skipwhite(s);
	ಆದರೆ (*s != '=')
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	++s;
    }
    ಆದರೆ (*s != '>')
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (white_error != ಖಾಲಿ && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    ಆದರೆ (checkforcmd(&p, "function", 2))
    {
	ಆದರೆ (*p == '(')
	    ಹಿಂತಿರುಗಿ FALSE;
	*cmd = p;
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
function_using_ಖಂಡ_scopes(ufunc_T *fp, cstack_T *cstack)
{
    ಆದರೆ (cstack == ಖಾಲಿ || cstack->cs_idx < 0)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2 = cstack->cs_idx + 1;
    ಪೂರ್ಣಾಂಕ	    i;

    fp->uf_ಖಂಡ_ids = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, ಎಣಿಕೆ_2);
    ಆದರೆ (fp->uf_ಖಂಡ_ids != ಖಾಲಿ)
    {
	mch_memmove(fp->uf_ಖಂಡ_ids, cstack->cs_ಖಂಡ_id,
		ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * ಎಣಿಕೆ_2);
	fp->uf_ಖಂಡ_ಆಳ = ಎಣಿಕೆ_2;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    ಗಾಗಿ (i = 0; i <= cstack->cs_idx; ++i)
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
    ಆಗ (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	ಆದರೆ (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    MB_PTR_ADV(p);
    }

    ಹಿಂತಿರುಗಿ p;
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
    ಆಗ (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	ಆದರೆ (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '"')
	{
	    p++;    // skip the ending double quote
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    MB_PTR_ADV(p);
    }

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_ಆರಂಭ_ದ_vim9_comment(char_u *ಸಾಲು)
{
    char_u	*p = ಸಾಲು;

    ಆಗ (*p != NUL)
    {
	ಆದರೆ (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*p == '#')
		// Found the start of a Vim9 comment
		ನಿಲ್ಲಿಸು;
	    MB_PTR_ADV(p);
	}
    }

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *ಸಾಲು_arg_ಒಳ,
	garray_T    *ಸಾಲುಗಳು_ಗಮ್ಯ_free)
{
    linenr_T	sourcing_lnum_ತುದಿ = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    ಪೂರ್ಣಾಂಕ		saved_wait_return = need_wait_return;
    char_u	*ಸಾಲು_arg = ಸಾಲು_arg_ಒಳ;
    ಪೂರ್ಣಾಂಕ		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_ಖಂಡ;
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_FUNC_NESTING 50
    ಅಕ್ಷರ	nesting_def[MAX_FUNC_NESTING];
    ಅಕ್ಷರ	nesting_inline[MAX_FUNC_NESTING];
    ಪೂರ್ಣಾಂಕ		nesting = 0;
    getline_opt_T getline_options;
    ಪೂರ್ಣಾಂಕ		indent = 2;
    char_u	*skip_until = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    ಪೂರ್ಣಾಂಕ		is_heredoc = FALSE;
    ಪೂರ್ಣಾಂಕ		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = ಖಾಲಿ;
    size_t	heredoc_trimmedlen = 0;
    ಪೂರ್ಣಾಂಕ		did_emsg_ಆರಂಭ = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    ಆದರೆ (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	ಆದರೆ (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    ಹೋಗು theend;
	ಆಗ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_ದತ್ತಾಂಶ))[newlines->ga_len++] = ಖಾಲಿ;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_ಖಂಡ;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    ಗಾಗಿ (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	ಆದರೆ (ಕೀಲಿ_Typed)
	{
	    ಸಂದೇಶ_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	ಆದರೆ (ಸಾಲು_arg != ಖಾಲಿ)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = ಸಾಲು_arg;
	    p = vim_strchr(theline, '\n');
	    ಆದರೆ (p == ಖಾಲಿ)
		ಸಾಲು_arg += STRLEN(ಸಾಲು_arg);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		*p = NUL;
		ಸಾಲು_arg = p + 1;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    theline = get_function_ಸಾಲು(eap, ಸಾಲುಗಳು_ಗಮ್ಯ_free, indent,
							      getline_options);
	}
	ಆದರೆ (ಕೀಲಿ_Typed)
	    ಸಾಲುಗಳು_ಎಡ = ಅಡ್ಡಸಾಲುಗಳು - 1;
	ಆದರೆ (theline == ಖಾಲಿ)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_ತುದಿ;
	    ಆದರೆ (skip_until != ಖಾಲಿ)
		semsg(_(e_missing_heredoc_ಕೊನೆ_marker_str), skip_until);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nesting_inline[nesting])
		emsg(_(e_missing_ಕೊನೆ_ಖಂಡ));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    ಇಲ್ಲದಿದ್ದರೆ
		emsg(_(e_missing_endfunction));
	    ಹೋಗು theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	ಆದರೆ (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	ಇಲ್ಲದಿದ್ದರೆ
	    sourcing_lnum_off = 0;

	ಆದರೆ (skip_until != ಖಾಲಿ)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    ಆದರೆ (heredoc_trimmed == ಖಾಲಿ
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		ಆದರೆ (heredoc_trimmed == ಖಾಲಿ)
		    p = theline;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		ಇಲ್ಲದಿದ್ದರೆ
		    p = theline + heredoc_trimmedlen;
		ಆದರೆ (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    ಆದರೆ (heredoc_concat_len > 0)
		    {
			// Replace the starting line with all the concatenated
			// lines.
			ga_concat(&heredoc_ga, theline);
			vim_free(((char_u **)(newlines->ga_ದತ್ತಾಂಶ))[
						      heredoc_concat_len - 1]);
			((char_u **)(newlines->ga_ದತ್ತಾಂಶ))[
				  heredoc_concat_len - 1] = heredoc_ga.ga_ದತ್ತಾಂಶ;
			ga_init(&heredoc_ga);
			heredoc_concat_len = 0;
			theline += STRLEN(theline);  // skip the "EOF"
		    }
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ	    c;
	    char_u  *ಕೊನೆ;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    ಗಾಗಿ (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		ಆದರೆ (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    ಆದರೆ (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		ಆದರೆ (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		ಆದರೆ (nesting-- == 0)
		{
		    char_u *nextcmd = ಖಾಲಿ;

		    ಆದರೆ (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು_arg != ಖಾಲಿ && *skipwhite(ಸಾಲು_arg) != NUL)
			nextcmd = ಸಾಲು_arg;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_ತುದಿ
							+ newlines->ga_len + 1;
			ಆದರೆ (eap->cmdidx == CMD_def)
			    semsg(_(e_ಪಠ್ಯ_ಸಿಕ್ಕಿತು_after_str_str), "enddef", p);
			ಇಲ್ಲದಿದ್ದರೆ
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    ಆದರೆ (nextcmd != ಖಾಲಿ && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			ಆದರೆ (ಸಾಲುಗಳು_ಗಮ್ಯ_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)ಸಾಲುಗಳು_ಗಮ್ಯ_free->ga_ದತ್ತಾಂಶ)
						   [ಸಾಲುಗಳು_ಗಮ್ಯ_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)ಸಾಲುಗಳು_ಗಮ್ಯ_free->ga_ದತ್ತಾಂಶ)
						   [ಸಾಲುಗಳು_ಗಮ್ಯ_free->ga_len - 1];
			    --ಸಾಲುಗಳು_ಗಮ್ಯ_free->ga_len;
			}
		    }
		    ನಿಲ್ಲಿಸು;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nesting_def[nesting])
	    {
		ಆದರೆ (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    ಆದರೆ (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    ಆದರೆ (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		ಆದರೆ (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, ಖಾಲಿ, TRUE, 0));
		ಆದರೆ (vim9_function && *p == '<')
		{
		    // skip generic function
		    ಆದರೆ (skip_generic_func_type_ವಾದಗಳು(&p) == FAIL)
			ಹೋಗು theend;
		}
		ಆದರೆ (*skipwhite(p) == '(')
		{
		    ಆದರೆ (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    ಆದರೆ (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		ಆದರೆ (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಆರಂಭ_ದ_vim9_comment(p) - 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಕೊನೆ = p + STRLEN(p) - 1;

		ಆಗ (ಕೊನೆ > p && VIM_ISWHITE(*ಕೊನೆ))
		    --ಕೊನೆ;
		ಆದರೆ (ಕೊನೆ > p + 1 && *ಕೊನೆ == '{' && VIM_ISWHITE(ಕೊನೆ[-1]))
		{
		    ಪೂರ್ಣಾಂಕ	    is_ಖಂಡ;

		    // check for trailing "=> {": start of an inline function
		    --ಕೊನೆ;
		    ಆಗ (ಕೊನೆ > p && VIM_ISWHITE(*ಕೊನೆ))
			--ಕೊನೆ;
		    is_ಖಂಡ = ಕೊನೆ > p + 2 && ಕೊನೆ[-1] == '=' && ಕೊನೆ[0] == '>';
		    ಆದರೆ (!is_ಖಂಡ)
			is_ಖಂಡ = ಕಂಡುಹಿಡಿ_cmd_ಖಂಡ_ಆರಂಭ(p) != ಖಾಲಿ;

		    ಆದರೆ (is_ಖಂಡ)
		    {
			ಆದರೆ (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			ಇಲ್ಲದಿದ್ದರೆ
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
	    ಆದರೆ (!vim9_function || colon)
		p = skip_range(p, FALSE, ಖಾಲಿ);
	    char_u *tp = p;
	    ಆದರೆ (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    ಇಲ್ಲದಿದ್ದರೆ
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    ಆದರೆ (arg[0] == '<' && arg[1] =='<'
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
		ಆದರೆ (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    ಆದರೆ (heredoc_trimmed == ಖಾಲಿ)
			heredoc_trimmedlen = 0;
		}
		ಆದರೆ (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		ಇಲ್ಲದಿದ್ದರೆ
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		ಆದರೆ (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    ಆದರೆ (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		ಆದರೆ (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    ಪೂರ್ಣಾಂಕ		save_sc_ಆವೃತ್ತಿ = current_sctx.sc_ಆವೃತ್ತಿ;
		    ಪೂರ್ಣಾಂಕ		var_ಎಣಿಕೆ = 0;
		    ಪೂರ್ಣಾಂಕ		semicolon = 0;

		    current_sctx.sc_ಆವೃತ್ತಿ
				     = vim9_function ? SCRIPT_ಆವೃತ್ತಿ_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_ಎಣಿಕೆ, &semicolon,
									 TRUE);
		    ಆದರೆ (arg != ಖಾಲಿ)
			arg = skipwhite(arg);
		    current_sctx.sc_ಆವೃತ್ತಿ = save_sc_ಆವೃತ್ತಿ;
		    ಆದರೆ (arg != ಖಾಲಿ && STRNCMP(arg, "=<<", 3) == 0)
		    {
			ಪೂರ್ಣಾಂಕ has_trim = FALSE;

			p = skipwhite(arg + 3);
			ಆಗ (TRUE)
			{
			    ಆದರೆ (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				ಮುಂದುವರಿ;
			    }
			    ಆದರೆ (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				ಮುಂದುವರಿ;
			    }
			    ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    ಆದರೆ (heredoc_trimmed == ಖಾಲಿ)
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
	ಆದರೆ (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    ಹೋಗು theend;

	ಆದರೆ (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹೋಗು theend;
	((char_u **)(newlines->ga_ದತ್ತಾಂಶ))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	ಆಗ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_ದತ್ತಾಂಶ))[newlines->ga_len++] = ಖಾಲಿ;

	// Check for end of eap->arg.
	ಆದರೆ (ಸಾಲು_arg != ಖಾಲಿ && *ಸಾಲು_arg == NUL)
	    ಸಾಲು_arg = ಖಾಲಿ;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    ಆದರೆ (did_emsg == did_emsg_ಆರಂಭ)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_ದತ್ತಾಂಶ);
    need_wait_return |= saved_wait_return;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	ಪೂರ್ಣಾಂಕ	    varargs,
	garray_T    *default_ವಾದಗಳು,
	char_u	    *ret_type)
{
    char_u	*ಆರಂಭ = *arg;
    ಪೂರ್ಣಾಂಕ		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = ಖಾಲಿ;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    ಪೂರ್ಣಾಂಕ		lnum_save = -1;
    linenr_T	sourcing_lnum_ತುದಿ = SOURCING_LNUM;
    char_u	*ಸಾಲು_arg = ಖಾಲಿ;

    *arg = skipwhite(*arg + 1);
    ಆದರೆ (**arg == '|' || !ತುದಿಗಳು_excmd2(ಆರಂಭ, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    ಆದರೆ (**arg == '\n')
	ಸಾಲು_arg = *arg + 1;

    CLEAR_ಕ್ಷೇತ್ರ(eap);
    eap.cmdidx = CMD_ಖಂಡ;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    ಆದರೆ (evalarg->eval_cctx != ಖಾಲಿ)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, ಗಾತ್ರ(char_u *), 10);
    ಆದರೆ (get_function_body(&eap, &newlines, ಸಾಲು_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	ಹೋಗು erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_ಎಣಿಕೆ += newlines.ga_len;
    ಆದರೆ (gap->ga_itemsize > 0)
    {
	ಪೂರ್ಣಾಂಕ	idx;
	char_u	*ಕೊನೆಯ;
	size_t  plen;
	char_u  *pnl;

	ಗಾಗಿ (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_ದತ್ತಾಂಶ)[idx];
	    ಆದರೆ (p == ಖಾಲಿ)
		// comment line in the lambda body
		ಮುಂದುವರಿ;

	    p = skipwhite(p);

	    ಆದರೆ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		ಹೋಗು erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    ಆದರೆ (*p == NUL || vim9_comment_ಆರಂಭ(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    ಆದರೆ (pnl != ಖಾಲಿ)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_ದತ್ತಾಂಶ)[freegap->ga_len++] = pnl;
	}
	ಆದರೆ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    ಹೋಗು erret;
	ಆದರೆ (eap.nextcmd != ಖಾಲಿ)
	{
	    // more is following after the "}", which was skipped
	    ಕೊನೆಯ = cmdline;
	    plen = STRLEN(ಕೊನೆಯ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // nothing is following the "}"
	    ಕೊನೆಯ = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	ಆದರೆ (pnl != ಖಾಲಿ)
	    mch_memmove(pnl + 1, ಕೊನೆಯ, plen + 1);
	((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_ದತ್ತಾಂಶ)[freegap->ga_len++] = pnl;
    }

    ಆದರೆ (eap.nextcmd != ಖಾಲಿ)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	ಆದರೆ (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_ದತ್ತಾಂಶ))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	*arg = (char_u *)"";

    ಆದರೆ (!evaluate)
    {
	ret = OK;
	ಹೋಗು erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.ಉದ್ದಳತೆ);
    ಆದರೆ (ufunc == ಖಾಲಿ)
	ಹೋಗು erret;
    ಆದರೆ (hash_ಕೂಡಿಸು(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	ಹೋಗು erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_ವಾದಗಳು = *newargs;
    newargs->ga_ದತ್ತಾಂಶ = ಖಾಲಿ;
    ufunc->uf_def_ವಾದಗಳು = *default_ವಾದಗಳು;
    default_ವಾದಗಳು->ga_ದತ್ತಾಂಶ = ಖಾಲಿ;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_ತುದಿ;

    // parse argument types, the type variables of the function being
    // compiled may be used
    ಆದರೆ (parse_ವಾದ_types(ufunc, argtypes, varargs, ಖಾಲಿ, ಖಾಲಿ, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	ಹೋಗು erret;
    }

    // parse the return type, if any
    ಆದರೆ (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	ಹೋಗು erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    ಆದರೆ (pt == ಖಾಲಿ)
	ಹೋಗು erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_ಸಾಲುಗಳು = newlines;
    newlines.ga_ದತ್ತಾಂಶ = ಖಾಲಿ;
    ಆದರೆ (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    ಆದರೆ (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_ಆವೃತ್ತಿ = current_sctx.sc_ಆವೃತ್ತಿ;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_ತುದಿ;
    ಗಣ_function_type(ufunc);

    function_using_ಖಂಡ_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = ಖಾಲಿ;
    ret = OK;

erret:
    ಆದರೆ (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&newlines);
    ಆದರೆ (newargs != ಖಾಲಿ)
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(newargs);
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(default_ವಾದಗಳು);
    ಆದರೆ (ufunc != ಖಾಲಿ)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    ಪೂರ್ಣಾಂಕ
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	ಪೂರ್ಣಾಂಕ	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg != ಖಾಲಿ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_ವಾದಗಳು;
    garray_T	arg_objm;
    ufunc_T	*fp = ಖಾಲಿ;
    partial_T   *pt = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		varargs;
    char_u	*ret_type = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ret;
    char_u	*s;
    char_u	*ಆರಂಭ, *ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		*old_eval_lavars = eval_lavars_ಬಳಸಿದ;
    ಪೂರ್ಣಾಂಕ		eval_lavars = FALSE;
    char_u	*tofree2 = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		equal_arrow = **arg == '(';
    ಪೂರ್ಣಾಂಕ		white_error = FALSE;
    ಪೂರ್ಣಾಂಕ		called_emsg_ಆರಂಭ = called_emsg;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    ದೀರ್ಘ	ಆರಂಭ_lnum = SOURCING_LNUM;

    ಆದರೆ (equal_arrow && !vim9script)
	ಹಿಂತಿರುಗಿ NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_ವಾದಗಳು(&s, equal_arrow ? ')' : '-', ಖಾಲಿ,
	    types_optional ? &argtypes : ಖಾಲಿ, types_optional,
			types_optional ? &arg_objm : ಖಾಲಿ, evalarg,
			ಖಾಲಿ, &default_ವಾದಗಳು, TRUE, ಖಾಲಿ, FALSE, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, ಖಾಲಿ) == ಖಾಲಿ)
    {
	ಆದರೆ (types_optional)
	{
	    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&argtypes);
	    ga_clear(&arg_objm);
	}
	ಹಿಂತಿರುಗಿ called_emsg == called_emsg_ಆರಂಭ ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    ಆದರೆ (evaluate)
	pnewargs = &newargs;
    ಇಲ್ಲದಿದ್ದರೆ
	pnewargs = ಖಾಲಿ;
    *arg += 1;
    ret = get_function_ವಾದಗಳು(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : ಖಾಲಿ, types_optional,
			types_optional ? &arg_objm : ಖಾಲಿ, evalarg,
					    &varargs, &default_ವಾದಗಳು,
					    FALSE, ಖಾಲಿ, FALSE, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : ಖಾಲಿ)) == ಖಾಲಿ)
    {
	ಆದರೆ (types_optional)
	{
	    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&newargs);
	ಹಿಂತಿರುಗಿ white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    ಆದರೆ (ret_type != ಖಾಲಿ)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    ಆದರೆ (evaluate)
	eval_lavars_ಬಳಸಿದ = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    ಆದರೆ (equal_arrow && **arg == '{')
    {
	ಆದರೆ (evalarg == ಖಾಲಿ)
	    // cannot happen?
	    ಹೋಗು theend;
	SOURCING_LNUM = ಆರಂಭ_lnum;  // used for where lambda is defined
	ಆದರೆ (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : ಖಾಲಿ, varargs,
			   &default_ವಾದಗಳು, ret_type) == FAIL)
	    ಹೋಗು errret;
	ಹೋಗು theend;
    }
    ಆದರೆ (default_ವಾದಗಳು.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_ಮೌಲ್ಯಗಳು_ಒಳ_lambda));
	ಹೋಗು errret;
    }

    // Get the start and the end of the expression.
    ಆರಂಭ = *arg;
    ret = skip_ಅಭಿವ್ಯಕ್ತಿ_concatenate(arg, &ಆರಂಭ, &ಕೊನೆ, evalarg);
    ಆದರೆ (ret == FAIL)
	ಹೋಗು errret;

    ಆದರೆ (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಆದರೆ (**arg != '}')
	{
	    semsg(_(e_expected_ಬಲ_curly_str), *arg);
	    ಹೋಗು errret;
	}
	++*arg;
    }

    ಆದರೆ (evaluate)
    {
	ಪೂರ್ಣಾಂಕ	    len;
	ಪೂರ್ಣಾಂಕ	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *ಸಾಲು_ಕೊನೆ;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.ಉದ್ದಳತೆ);
	ಆದರೆ (fp == ಖಾಲಿ)
	    ಹೋಗು errret;
	fp->uf_def_ಸ್ಥಿತಿ = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	ಆದರೆ (pt == ಖಾಲಿ)
	    ಹೋಗು errret;

	ga_init2(&newlines, ಗಾತ್ರ(char_u *), 1);
	ಆದರೆ (ga_grow(&newlines, 1) == FAIL)
	    ಹೋಗು errret;

	// If there are line breaks, we need to split up the string.
	ಸಾಲು_ಕೊನೆ = vim_strchr(ಆರಂಭ, '\n');
	ಆದರೆ (ಸಾಲು_ಕೊನೆ == ಖಾಲಿ || ಸಾಲು_ಕೊನೆ > ಕೊನೆ)
	    ಸಾಲು_ಕೊನೆ = ಕೊನೆ;

	// Add "return " before the expression (or the first line).
	len = 7 + (ಪೂರ್ಣಾಂಕ)(ಸಾಲು_ಕೊನೆ - ಆರಂಭ) + 1;
	p = alloc(len);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹೋಗು errret;
	((char_u **)(newlines.ga_ದತ್ತಾಂಶ))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, ಆರಂಭ, ಸಾಲು_ಕೊನೆ - ಆರಂಭ);

	ಆದರೆ (ಸಾಲು_ಕೊನೆ != ಕೊನೆ)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    ಆಗ (ಸಾಲು_ಕೊನೆ < ಕೊನೆ && *ಸಾಲು_ಕೊನೆ == '\n')
	    {
		ಆದರೆ (ga_grow(&newlines, 1) == FAIL)
		    ಹೋಗು errret;
		ಆರಂಭ = ಸಾಲು_ಕೊನೆ + 1;
		ಸಾಲು_ಕೊನೆ = vim_strchr(ಆರಂಭ, '\n');
		ಆದರೆ (ಸಾಲು_ಕೊನೆ == ಖಾಲಿ || ಸಾಲು_ಕೊನೆ > ಕೊನೆ)
		    ಸಾಲು_ಕೊನೆ = ಕೊನೆ;
		((char_u **)(newlines.ga_ದತ್ತಾಂಶ))[newlines.ga_len++] =
					 vim_strnsave(ಆರಂಭ, ಸಾಲು_ಕೊನೆ - ಆರಂಭ);
	    }
	}

	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)p + 7, "a:") == ಖಾಲಿ)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_ವಾದಗಳು = newargs;
	ga_init(&fp->uf_def_ವಾದಗಳು);
	ಆದರೆ (types_optional)
	{
	    ಆದರೆ (parse_ವಾದ_types(fp, &argtypes,
				vim9script && varargs, ಖಾಲಿ, ಖಾಲಿ, 0,
				cctx) == FAIL)
		ಹೋಗು errret;
	    ಆದರೆ (ret_type != ಖಾಲಿ)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     ಖಾಲಿ, cctx, TRUE);
		ಆದರೆ (fp->uf_ret_type == ಖಾಲಿ)
		    ಹೋಗು errret;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_ಸಾಲುಗಳು = newlines;
	ಆದರೆ (current_funccal != ಖಾಲಿ && eval_lavars)
	{
	    flags |= FC_ಆವರಣ;
	    ಆದರೆ (ರಿಜಿಸ್ಟರ್_ಆವರಣ(fp) == FAIL)
		ಹೋಗು errret;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (prof_def_func())
	    func_do_profile(fp);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (sandbox)
	    flags |= FC_SANDBOX;
	// In legacy script a lambda can be called with more args than
	// uf_args.ga_len.  In Vim9 script "...name" has to be used.
	fp->uf_varargs = !vim9script || varargs;
	fp->uf_flags = flags;
	fp->uf_calls = 0;
	fp->uf_script_ctx = current_sctx;
	// Use the line number of the arguments.
	fp->uf_script_ctx.sc_lnum += ಆರಂಭ_lnum;

	function_using_ಖಂಡ_scopes(fp, evalarg->eval_cstack);

	pt->pt_func = fp;
	pt->pt_refcount = 1;
	rettv->vval.v_partial = pt;
	rettv->v_type = VAR_PARTIAL;

	hash_ಕೂಡಿಸು(&func_hashtab, UF2HIKEY(fp), "add lambda");
    }

theend:
    eval_lavars_ಬಳಸಿದ = old_eval_lavars;
    vim_free(tofree2);
    ಆದರೆ (types_optional)
    {
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&argtypes);
	ga_clear(&arg_objm);
    }

    ಹಿಂತಿರುಗಿ OK;

errret:
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&newargs);
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&newlines);
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&default_ವಾದಗಳು);
    ಆದರೆ (types_optional)
    {
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&argtypes);
	ga_clear(&arg_objm);
    }
    ಆದರೆ (fp != ಖಾಲಿ)
    {
	vim_free(fp->uf_arg_types);
	vim_free(fp->uf_name_exp);
	vim_free(fp->uf_va_name);
	clear_type_list(&fp->uf_type_list);
	vim_free(fp);
    }
    vim_free(pt);
    vim_free(tofree2);
    eval_lavars_ಬಳಸಿದ = old_eval_lavars;
    ಹಿಂತಿರುಗಿ FAIL;
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
	ಪೂರ್ಣಾಂಕ	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	ಪೂರ್ಣಾಂಕ	    no_autoload,
	ಪೂರ್ಣಾಂಕ	    new_function,
	ಪೂರ್ಣಾಂಕ	    *ಸಿಕ್ಕಿತು_var)
{
    dictitem_T	*v;
    typval_T	*tv = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		cc;
    char_u	*s = ಖಾಲಿ;
    hashtab_T	*ht;
    ಪೂರ್ಣಾಂಕ		did_type = FALSE;

    ಆದರೆ (partialp != ಖಾಲಿ)
	*partialp = ಖಾಲಿ;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = ಕಂಡುಹಿಡಿ_var_also_ಒಳ_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    ಆದರೆ (v != ಖಾಲಿ)
    {
	tv = &v->di_tv;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	ಪೂರ್ಣಾಂಕ	    len = *lenp;

	ಆದರೆ (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = ಕಂಡುಹಿಡಿ_imported(p, len, FALSE);

	// imported function from another script
	ಆದರೆ (import != ಖಾಲಿ)
	{
	    name[len] = NUL;
	    ಆದರೆ (new_function)
		semsg(_(e_redefining_imported_ಅಂಶ_str), name);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    ಹಿಂತಿರುಗಿ (char_u *)"";	// just in case
	}
    }

    ಆದರೆ (tv != ಖಾಲಿ)
    {
	ಆದರೆ (ಸಿಕ್ಕಿತು_var != ಖಾಲಿ)
	    *ಸಿಕ್ಕಿತು_var = TRUE;
	ಆದರೆ (tv->v_type == VAR_FUNC)
	{
	    ಆದರೆ (tv->vval.v_string == ಖಾಲಿ)
	    {
		*lenp = 0;
		ಹಿಂತಿರುಗಿ (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_ಕಂಡುಹಿಡಿ_open_bracket(s);
	    ಆದರೆ (p != ಖಾಲಿ && generic_func_ಕಂಡುಹಿಡಿ_open_bracket(name) != ಖಾಲಿ)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		ಹಿಂತಿರುಗಿ (char_u *)"";
	    }
	    ಆದರೆ (p != ಖಾಲಿ)
		*lenp = p - s;
	    ಇಲ್ಲದಿದ್ದರೆ
		*lenp = (ಪೂರ್ಣಾಂಕ)STRLEN(s);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    ಆದರೆ (pt == ಖಾಲಿ)
	    {
		*lenp = 0;
		ಹಿಂತಿರುಗಿ (char_u *)"";	// just in case
	    }
	    ಆದರೆ (partialp != ಖಾಲಿ)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (ಪೂರ್ಣಾಂಕ)STRLEN(s);
	}

	ಆದರೆ (s != ಖಾಲಿ)
	{
	    ಆದರೆ (!did_type && type != ಖಾಲಿ && ht == get_script_local_ht())
	    {
		svar_T  *sv = ಕಂಡುಹಿಡಿ_typval_ಒಳ_script(tv, 0, TRUE);

		ಆದರೆ (sv != ಖಾಲಿ)
		    *type = sv->sv_type;
	    }
	    ಹಿಂತಿರುಗಿ s;
	}
    }

    ಹಿಂತಿರುಗಿ name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    ಶೂನ್ಯ
emsg_funcname(ಅಕ್ಷರ *ermsg, char_u *name)
{
    char_u	*p = name;

    ಆದರೆ (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    ಆದರೆ (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    ಪೂರ್ಣಾಂಕ
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	ಪೂರ್ಣಾಂಕ	    partial_argc,
	typval_T    *argvars,
	ಪೂರ್ಣಾಂಕ	    *argcount,
	ಪೂರ್ಣಾಂಕ	    is_builtin)
{
    char_u	*argp = *arg;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg == ಖಾಲಿ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ಆಗ (*argcount < MAX_FUNC_ವಾದಗಳು - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	ಆದರೆ (*argp == ')' || *argp == ',' || *argp == NUL)
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ಣಾಂಕ arg_idx = *argcount;
	ಆದರೆ (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    ನಿಲ್ಲಿಸು;
	}
	++*argcount;
	ಆದರೆ (!is_builtin && check_typval_is_ಮೌಲ್ಯ(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    ನಿಲ್ಲಿಸು;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	ಆದರೆ (vim9script)
	{
	    ಆದರೆ (*argp != ',' && *skipwhite(argp) == ',')
	    {
		ಆದರೆ (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    argp = skipwhite(argp);
	ಆದರೆ (*argp != ',')
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    ಆದರೆ (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    ನಿಲ್ಲಿಸು;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    ಆದರೆ (*argp == ')')
	++argp;
    ಇಲ್ಲದಿದ್ದರೆ
	ret = FAIL;
    *arg = argp;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
get_func_tv(
    char_u	*name,		// name of the function
    ಪೂರ್ಣಾಂಕ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    ಪೂರ್ಣಾಂಕ		ret;
    typval_T	argvars[MAX_FUNC_ವಾದಗಳು + 1];	// vars for arguments
    ಪೂರ್ಣಾಂಕ		argcount = 0;			// number of arguments found
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg == ಖಾಲಿ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == ಖಾಲಿ ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    ಆದರೆ (ret == OK)
    {
	ಪೂರ್ಣಾಂಕ	i = 0;
	ಪೂರ್ಣಾಂಕ	did_emsg_before = did_emsg;

	ಆದರೆ (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    ಆದರೆ (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, ಗಾತ್ರ(typval_T *), 50);
	    ಗಾಗಿ (i = 0; i < argcount; ++i)
		ಆದರೆ (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_ದತ್ತಾಂಶ)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	ಆದರೆ (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!aborting() && evaluate)
    {
	ಆದರೆ (argcount == MAX_FUNC_ವಾದಗಳು)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    ಆಗ (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    ಆದರೆ (vim9script)
	*arg = argp;
    ಇಲ್ಲದಿದ್ದರೆ
	*arg = skipwhite(argp);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_fname_sid(char_u *p)
{
    ಹಿಂತಿರುಗಿ (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    ಆದರೆ (script_name == name)
	ಹಿಂತಿರುಗಿ name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (ಪೂರ್ಣಾಂಕ)KE_SNR;
    fname_buflen = 3;
    ಆದರೆ (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    fname_buflen += vim_snprintf((ಅಕ್ಷರ *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(ದೀರ್ಘ)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    ಆದರೆ (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fname = alloc(fnamelen + 1);
	ಆದರೆ (fname == ಖಾಲಿ)
	    *error = FCERR_OTHER;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    *tofree = fname;
	    vim_snprintf((ಅಕ್ಷರ *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    ಹಿಂತಿರುಗಿ fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    ಶೂನ್ಯ
func_name_with_sid(char_u *name, ಪೂರ್ಣಾಂಕ sid, char_u *ಬಫರ್)
{
    // A script-local function is stored as "<SNR>99_name".
    ಬಫರ್[0] = K_SPECIAL;
    ಬಫರ್[1] = KS_EXTRA;
    ಬಫರ್[2] = (ಪೂರ್ಣಾಂಕ)KE_SNR;
    vim_snprintf((ಅಕ್ಷರ *)ಬಫರ್ + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (ದೀರ್ಘ)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    ಸ್ಥಿರ ufunc_T *
ಕಂಡುಹಿಡಿ_func_with_sid(char_u *name, ಪೂರ್ಣಾಂಕ sid)
{
    hashitem_T	    *hi;
    char_u	    ಬಫರ್[MAX_FUNC_NAME_LEN];

    ಆದರೆ (!SCRIPT_ID_VALID(sid))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;	// not in a script

    func_name_with_sid(name, sid, ಬಫರ್);
    hi = hash_ಕಂಡುಹಿಡಿ(&func_hashtab, ಬಫರ್);
    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	ಹಿಂತಿರುಗಿ HI2UF(hi);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    ಸ್ಥಿರ ufunc_T *
ಕಂಡುಹಿಡಿ_func_with_ಪೂರ್ವಪ್ರತ್ಯಯ(char_u *name, ಪೂರ್ಣಾಂಕ sid)
{
    hashitem_T	    *hi;
    char_u	    ಬಫರ್[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    ಆದರೆ (vim_strchr(name, AUTOLOAD_CHAR) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;	// already has the prefix
    ಆದರೆ (!SCRIPT_ID_VALID(sid))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;	// not in a script
    si = SCRIPT_ಅಂಶ(sid);
    ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
    {
	size_t	len = STRLEN(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	ಆದರೆ (namep == ಖಾಲಿ)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	ಆದರೆ (len < ಗಾತ್ರ(ಬಫರ್))
	    auto_name = ಬಫರ್;
	ಇಲ್ಲದಿದ್ದರೆ
	    auto_name = alloc(len);
	ಆದರೆ (auto_name != ಖಾಲಿ)
	{
	    vim_snprintf((ಅಕ್ಷರ *)auto_name, len, "%s%s",
						si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ, namep);
	    hi = hash_ಕಂಡುಹಿಡಿ(&func_hashtab, auto_name);
	    ಆದರೆ (auto_name != ಬಫರ್)
		vim_free(auto_name);
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		ಹಿಂತಿರುಗಿ HI2UF(hi);
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
ಕಂಡುಹಿಡಿ_func_even_dead(char_u *name, ಪೂರ್ಣಾಂಕ flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    ಆದರೆ ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	ಆದರೆ (ಒಳ_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = ಕಂಡುಹಿಡಿ_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    ಆದರೆ (func != ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ func;
	}
	ಆದರೆ (ಒಳ_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    ದೀರ್ಘ    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    ಆದರೆ (*p == '_')
	    {
		func = ಕಂಡುಹಿಡಿ_func_with_sid(p + 1, (ಪೂರ್ಣಾಂಕ)sid);
		ಆದರೆ (func != ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ func;
	    }
	}
    }

    ಆದರೆ ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_ಕಂಡುಹಿಡಿ(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    ಹಿಂತಿರುಗಿ HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_func_with_ಪೂರ್ವಪ್ರತ್ಯಯ(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
ಕಂಡುಹಿಡಿ_func(char_u *name, ಪೂರ್ಣಾಂಕ is_global)
{
    ufunc_T	*fp = ಕಂಡುಹಿಡಿ_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    ಆದರೆ (fp != ಖಾಲಿ && (fp->uf_flags & FC_DEAD) == 0)
	ಹಿಂತಿರುಗಿ fp;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    ಪೂರ್ಣಾಂಕ
func_is_global(ufunc_T *ufunc)
{
    ಹಿಂತಿರುಗಿ ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    ಪೂರ್ಣಾಂಕ
func_requires_g_ಪೂರ್ವಪ್ರತ್ಯಯ(ufunc_T *ufunc)
{
    ಹಿಂತಿರುಗಿ func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ಖಾಲಿ
	    && !ಸುರಕ್ಷಿತ_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    ಪೂರ್ಣಾಂಕ	len;

    ಆದರೆ (!func_is_global(fp))
	len = vim_snprintf((ಅಕ್ಷರ *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    ಇಲ್ಲದಿದ್ದರೆ
	len = vim_snprintf((ಅಕ್ಷರ *)buf, bufsize, "%s", fp->uf_name);

    ಹಿಂತಿರುಗಿ (len >= (ಪೂರ್ಣಾಂಕ)bufsize) ? (ಪೂರ್ಣಾಂಕ)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    ಅಕ್ಷರ	*name,
    varnumber_T nr)
{
    STRCPY(v->di_ಕೀಲಿ, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_ಕೂಡಿಸು(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_ಸಂಖ್ಯೆ;
    v->di_tv.v_ಬೀಗ = VAR_FIXED;
    v->di_tv.vval.v_ಸಂಖ್ಯೆ = nr;
}

/*
 * Free "fc".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_funccal(funccall_T *fc)
{
    ಪೂರ್ಣಾಂಕ	i;

    ಗಾಗಿ (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_ದತ್ತಾಂಶ))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	ಆದರೆ (fp != ಖಾಲಿ && fp->uf_scoped == fc)
	    fp->uf_scoped = ಖಾಲಿ;
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
    ಪೂರ್ಣಾಂಕ	may_free_fc = fc->fc_refcount <= 0;
    ಪೂರ್ಣಾಂಕ	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    ಆದರೆ (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    ಇಲ್ಲದಿದ್ದರೆ
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    ಆದರೆ (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (ಪೂರ್ಣಾಂಕ)fc->fc_l_avars.dv_hashtab.ht_ಬಳಸಿದ;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    ಆದರೆ (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_ಮೊದಲ = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_ವಸ್ತುಗಳು(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    ಆದರೆ (free_fc)
	free_funccal(fc);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = ಹಿಂದಿನ_funccal;
	ಹಿಂದಿನ_funccal = fc;

	ಆದರೆ (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (++made_copy >= (ಪೂರ್ಣಾಂಕ)((4096 * 1024) / ಗಾತ್ರ(*fc)))
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
numbered_function(char_u *name)
{
    ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_isdigit(*name)
	    || (name[0] == 'g' && name[1] == ':' && ಸುರಕ್ಷಿತ_isdigit(name[2]));
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
    ಪೂರ್ಣಾಂಕ
func_name_refcount(char_u *name)
{
    ಹಿಂತಿರುಗಿ numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
funccal_unref(funccall_T *fc, ufunc_T *fp, ಪೂರ್ಣಾಂಕ force)
{
    funccall_T	**pfc;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (fc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	ಗಾಗಿ (pfc = &ಹಿಂದಿನ_funccal; *pfc != ಖಾಲಿ; pfc = &(*pfc)->fc_caller)
	{
	    ಆದರೆ (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		ಹಿಂತಿರುಗಿ;
	    }
	}
    ಗಾಗಿ (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	ಆದರೆ (((ufunc_T **)(fc->fc_ufuncs.ga_ದತ್ತಾಂಶ))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_ದತ್ತಾಂಶ))[i] = ಖಾಲಿ;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    ಆದರೆ (fp->uf_flags & FC_DEAD)
	ಹಿಂತಿರುಗಿ FALSE;

    hi = hash_ಕಂಡುಹಿಡಿ(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi) || HI2UF(hi) != fp)
	ಹಿಂತಿರುಗಿ FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    ಆದರೆ (fp->uf_def_ಸ್ಥಿತಿ == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	ಹಿಂತಿರುಗಿ FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
func_clear_ವಸ್ತುಗಳು(ufunc_T *fp)
{
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&(fp->uf_ವಾದಗಳು));
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&(fp->uf_def_ವಾದಗಳು));
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&(fp->uf_ಸಾಲುಗಳು));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_ಖಂಡ_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    ಆದರೆ (IS_GENERIC_FUNC(fp))
	generic_func_clear_ವಸ್ತುಗಳು(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = ಖಾಲಿ;
    fp->uf_refcount -= 3;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LUA
    ಆದರೆ (fp->uf_cb_free != ಖಾಲಿ)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = ಖಾಲಿ;
    }

    fp->uf_cb_state = ಖಾಲಿ;
    fp->uf_cb = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_ಎಣಿಕೆ);
    VIM_CLEAR(fp->uf_tml_ಒಟ್ಟು);
    VIM_CLEAR(fp->uf_tml_self);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
func_clear(ufunc_T *fp, ಪೂರ್ಣಾಂಕ force)
{
    ಆದರೆ (fp->uf_cleared)
	ಹಿಂತಿರುಗಿ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
func_free(ufunc_T *fp, ಪೂರ್ಣಾಂಕ force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    ಆದರೆ ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    ಆದರೆ ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	ಆದರೆ (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	ಹಿಂತಿರುಗಿ OK;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    ಶೂನ್ಯ
func_clear_free(ufunc_T *fp, ಪೂರ್ಣಾಂಕ force)
{
    func_clear(fp, force);
    ಆದರೆ (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    ಇಲ್ಲದಿದ್ದರೆ
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    ಪೂರ್ಣಾಂಕ
copy_lambda_ಗಮ್ಯ_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = ಕಂಡುಹಿಡಿ_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = ಖಾಲಿ;

    ಆದರೆ (ufunc == ಖಾಲಿ)
    {
	semsg(_(e_lambda_function_not_ಸಿಕ್ಕಿತು_str), lambda);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    fp = ಕಂಡುಹಿಡಿ_func(global, TRUE);
    ಆದರೆ (fp != ಖಾಲಿ)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_ಕೂಡಿಸು_bang_ಗಮ್ಯ_replace), global);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    ಆದರೆ (fp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_ಸ್ಥಿತಿ = ufunc->uf_def_ಸ್ಥಿತಿ;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    ಆದರೆ (ga_copy_ಅಕ್ಷರಸರಣಿಗಳು(&ufunc->uf_ವಾದಗಳು, &fp->uf_ವಾದಗಳು) == FAIL
	    || ga_copy_ಅಕ್ಷರಸರಣಿಗಳು(&ufunc->uf_def_ವಾದಗಳು, &fp->uf_def_ವಾದಗಳು)
								    == FAIL
	    || ga_copy_ಅಕ್ಷರಸರಣಿಗಳು(&ufunc->uf_ಸಾಲುಗಳು, &fp->uf_ಸಾಲುಗಳು) == FAIL)
	ಹೋಗು failed;

    ಆದರೆ (ufunc->uf_arg_types != ಖಾಲಿ)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_ವಾದಗಳು.ga_len);
	ಆದರೆ (fp->uf_arg_types == ಖಾಲಿ)
	    ಹೋಗು failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    ಗಾತ್ರ(type_T *) * fp->uf_ವಾದಗಳು.ga_len);
    }
    ಆದರೆ (ufunc->uf_va_name != ಖಾಲಿ)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	ಆದರೆ (fp->uf_va_name == ಖಾಲಿ)
	    ಹೋಗು failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_ಕೂಡಿಸು(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    ಆದರೆ ((ufunc->uf_flags & FC_ಆವರಣ) && ufunc->uf_partial == ಖಾಲಿ)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	ಆದರೆ (pt == ಖಾಲಿ)
	    ಹೋಗು failed;
	ಆದರೆ (fill_partial_and_ಆವರಣ(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    ಹೋಗು failed;
	}
	ufunc->uf_partial = pt;
    }

    ಹಿಂತಿರುಗಿ OK;

failed:
    func_clear_free(fp, TRUE);
    ಹಿಂತಿರುಗಿ FAIL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    ಪೂರ್ಣಾಂಕ
funcdepth_increment(ಶೂನ್ಯ)
{
    ಆದರೆ (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_ಆಳ_is_higher_than_maxfuncdepth));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ++funcdepth;
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
funcdepth_decrement(ಶೂನ್ಯ)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    ಪೂರ್ಣಾಂಕ
funcdepth_get(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    ಶೂನ್ಯ
funcdepth_restore(ಪೂರ್ಣಾಂಕ ಆಳ)
{
    funcdepth = ಆಳ;
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

    ಆದರೆ (fc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    ಹಿಂತಿರುಗಿ fc;
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
    ಪೂರ್ಣಾಂಕ		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    ಪೂರ್ಣಾಂಕ		using_sandbox = FALSE;
    ಪೂರ್ಣಾಂಕ		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    ಪೂರ್ಣಾಂಕ		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    ಪೂರ್ಣಾಂಕ		default_arg_err = FALSE;
    dictitem_T	*v;
    ಪೂರ್ಣಾಂಕ		fixvar_idx = 0;	// index in fc_fixvar[]
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ai;
    ಪೂರ್ಣಾಂಕ		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_ಗಮ್ಯ_free[MAX_FUNC_ವಾದಗಳು];
    ಪೂರ್ಣಾಂಕ		tv_ಗಮ್ಯ_free_len = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    profinfo_T	profile_info;
#ಆದರೆ_ಅಂತ್ಯ
    ESTACK_CHECK_DECLARATION;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    CLEAR_ಕ್ಷೇತ್ರ(profile_info);
#ಆದರೆ_ಅಂತ್ಯ

    // If depth of calling is getting too high, don't execute the function.
    ಆದರೆ (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
	ಹಿಂತಿರುಗಿ FCERR_FAILED;
    }

    ಸಾಲು_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    ಆದರೆ (fc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, ಗಾತ್ರ(ufunc_T *), 1);

    ಆದರೆ (fp->uf_def_ಸ್ಥಿತಿ != UF_NOT_COMPILED)
    {
	ಆದರೆ (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == ಖಾಲಿ ? ಖಾಲಿ : fc->fc_caller->fc_func;
#ಆದರೆ_ಅಂತ್ಯ
	// Execute the function, possibly compiling it first.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (do_profiling == PROF_YES)
	    profile_may_ಆರಂಭ_func(&profile_info, fp, caller);
#ಆದರೆ_ಅಂತ್ಯ
	sticky_cmdmod_flags = 0;
	ಆದರೆ (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	ಆದರೆ (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != ಖಾಲಿ && caller->uf_profiling)))
	    profile_may_ಕೊನೆ_func(&profile_info, fp, caller);
#ಆದರೆ_ಅಂತ್ಯ
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	ಹಿಂತಿರುಗಿ retval;
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
    ಆದರೆ (selfdict != ಖಾಲಿ)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_ಕೀಲಿ;
	STRCPY(name, "self");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_ಕೂಡಿಸು(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "set self dictionary");
	v->di_tv.v_type = VAR_DICT;
	v->di_tv.v_ಬೀಗ = 0;
	v->di_tv.vval.v_dict = selfdict;
	++selfdict->dv_refcount;
    }

    /*
     * Init a: variables, unless none found (in lambda).
     * Set a:0 to "argcount" less number of named arguments, if >= 0.
     * Set a:000 to a list with room for the "..." arguments.
     */
    init_var_dict(&fc->fc_l_avars, &fc->fc_l_avars_var, VAR_SCOPE);
    ಆದರೆ ((fp->uf_flags & FC_NOARGS) == 0)
	ಕೂಡಿಸು_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_ವಾದಗಳು.ga_len
				    ? argcount - fp->uf_ವಾದಗಳು.ga_len : 0));
    fc->fc_l_avars.dv_ಬೀಗ = VAR_FIXED;
    ಆದರೆ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_ಕೀಲಿ;
	STRCPY(name, "000");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_ಕೂಡಿಸು(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "function argument");
	v->di_tv.v_type = VAR_LIST;
	v->di_tv.v_ಬೀಗ = VAR_FIXED;
	v->di_tv.vval.v_list = &fc->fc_l_varlist;
    }
    CLEAR_ಕ್ಷೇತ್ರ(fc->fc_l_varlist);
    fc->fc_l_varlist.lv_refcount = DO_NOT_FREE_CNT;
    fc->fc_l_varlist.lv_ಬೀಗ = VAR_FIXED;

    /*
     * Set a:firstline to "firstline" and a:lastline to "lastline".
     * Set a:name to named arguments.
     * Set a:N to the "..." arguments.
     * Skipped when no a: variables used (in lambda).
     */
    ಆದರೆ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	ಕೂಡಿಸು_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	ಕೂಡಿಸು_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    ಗಾಗಿ (i = 0; i < argcount || i < fp->uf_ವಾದಗಳು.ga_len; ++i)
    {
	ಪೂರ್ಣಾಂಕ	    addlocal = FALSE;
	typval_T    def_rettv;
	ಪೂರ್ಣಾಂಕ	    isdefault = FALSE;

	ai = i - fp->uf_ವಾದಗಳು.ga_len;
	ಆದರೆ (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    ಆದರೆ (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_ವಾದಗಳು.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_ಸಂಖ್ಯೆ == VVAL_NONE));
	    ಆದರೆ (isdefault)
	    {
		char_u	    *default_ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;

		def_rettv.v_type = VAR_ಸಂಖ್ಯೆ;
		def_rettv.vval.v_ಸಂಖ್ಯೆ = -1;

		default_ಅಭಿವ್ಯಕ್ತಿ = ((char_u **)(fp->uf_def_ವಾದಗಳು.ga_ದತ್ತಾಂಶ))
						 [ai + fp->uf_def_ವಾದಗಳು.ga_len];
		ಆದರೆ (eval1(&default_ಅಭಿವ್ಯಕ್ತಿ, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    ನಿಲ್ಲಿಸು;
		}
	    }

	    namelen = STRLEN(name);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		ನಿಲ್ಲಿಸು;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((ಅಕ್ಷರ *)numbuf, ಗಾತ್ರ(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	ಆದರೆ (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_ಕೀಲಿ, name);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    v = dictitem_alloc(name);
	    ಆದರೆ (v == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_ಬೀಗ = VAR_FIXED;

	ಆದರೆ (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_ಗಮ್ಯ_free[tv_ಗಮ್ಯ_free_len++] = &v->di_tv;

	ಆದರೆ (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_ಕೂಡಿಸು(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    hash_ಕೂಡಿಸು(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	ಆದರೆ (ai >= 0 && ai < MAX_FUNC_ವಾದಗಳು)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_ಬೀಗ = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    ಆದರೆ (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_ತಳ್ಳು_ufunc(fp, 1);
    ESTACK_CHECK_ಸಿದ್ಧತೆ;
    ಆದರೆ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	ಆದರೆ (p_verbose >= 14)
	{
	    char_u	buf[ಸಂದೇಶ_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    ಸಂದೇಶ_puts("(");
	    ಗಾಗಿ (i = 0; i < argcount; ++i)
	    {
		ಆದರೆ (i > 0)
		    ಸಂದೇಶ_puts(", ");
		ಆದರೆ (argvars[i].v_type == VAR_ಸಂಖ್ಯೆ)
		    ಸಂದೇಶ_outnum((ದೀರ್ಘ)argvars[i].vval.v_ಸಂಖ್ಯೆ);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    ಆದರೆ (s != ಖಾಲಿ)
		    {
			ಆದರೆ (vim_strsize(s) > ಸಂದೇಶ_BUF_CLEN)
			{
			    trunc_string(s, buf, ಸಂದೇಶ_BUF_CLEN, ಸಂದೇಶ_BUF_LEN);
			    s = buf;
			}
			ಸಂದೇಶ_puts((ಅಕ್ಷರ *)s);
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
	profile_may_ಆರಂಭ_func(&profile_info, fp,
			fc->fc_caller == ಖಾಲಿ ? ಖಾಲಿ : fc->fc_caller->fc_func);
#ಆದರೆ_ಅಂತ್ಯ

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

    ಆದರೆ (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (islambda)
    {
	char_u *p = *(char_u **)fp->uf_ಸಾಲುಗಳು.ga_ದತ್ತಾಂಶ + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(ಶೂನ್ಯ)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// call do_cmdline() to execute the lines
	do_cmdline(ಖಾಲಿ, get_func_ಸಾಲು, (ಶೂನ್ಯ *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    ಆದರೆ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    ಆದರೆ ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	ಆದರೆ (ಒಳ_vim9script())
	    retval = FCERR_FAILED;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == ಖಾಲಿ ? ಖಾಲಿ : fc->fc_caller->fc_func;

	ಆದರೆ (fp->uf_profiling || (caller != ಖಾಲಿ && caller->uf_profiling))
	    profile_may_ಕೊನೆ_func(&profile_info, fp, caller);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // when being verbose, mention the return value
    ಆದರೆ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	ಆದರೆ (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fc->fc_rettv->v_type == VAR_ಸಂಖ್ಯೆ)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (ದೀರ್ಘ)fc->fc_rettv->vval.v_ಸಂಖ್ಯೆ);
	ಇಲ್ಲದಿದ್ದರೆ
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
	    ಆದರೆ (s != ಖಾಲಿ)
	    {
		ಆದರೆ (vim_strsize(s) > ಸಂದೇಶ_BUF_CLEN)
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_ಆರಂಭ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    ಆದರೆ (p_verbose >= 12 && SOURCING_NAME != ಖಾಲಿ)
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
    ಗಾಗಿ (i = 0; i < tv_ಗಮ್ಯ_free_len; ++i)
	clear_tv(tv_ಗಮ್ಯ_free[i]);
    cleanup_function_call(fc);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, ಪೂರ್ಣಾಂಕ argcount)
{
    ಪೂರ್ಣಾಂಕ regular_ವಾದಗಳು = fp->uf_ವಾದಗಳು.ga_len;

    ಆದರೆ (argcount < regular_ವಾದಗಳು - fp->uf_def_ವಾದಗಳು.ga_len)
	ಹಿಂತಿರುಗಿ FCERR_TOOFEW;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!has_varargs(fp) && argcount > regular_ವಾದಗಳು)
	ಹಿಂತಿರುಗಿ FCERR_TOOMANY;
    ಹಿಂತಿರುಗಿ FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	ಪೂರ್ಣಾಂಕ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LUA
    ಆದರೆ (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	ಹಿಂತಿರುಗಿ (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != ಖಾಲಿ)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    ಆದರೆ (error != FCERR_UNKNOWN)
	ಹಿಂತಿರುಗಿ error;

    ಆದರೆ ((fp->uf_flags & FC_DICT) && selfdict == ಖಾಲಿ)
    {
	error = FCERR_DICT;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_ಹುಡುಕು_patterns();
	ಆದರೆ (!ins_compl_ಸಕ್ರಿಯ())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : ಖಾಲಿ);
	ಆದರೆ (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	ಆದರೆ (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_ಹುಡುಕು_patterns();
    }

    ಹಿಂತಿರುಗಿ error;
}

ಸ್ಥಿರ funccal_ನಮೂದು_T *funccal_ರಾಶಿ = ಖಾಲಿ;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    ಶೂನ್ಯ
save_funccal(funccal_ನಮೂದು_T *ನಮೂದು)
{
    ನಮೂದು->ತುದಿ_funccal = current_funccal;
    ನಮೂದು->ಮುಂದಿನ = funccal_ರಾಶಿ;
    funccal_ರಾಶಿ = ನಮೂದು;
    current_funccal = ಖಾಲಿ;
}

    ಶೂನ್ಯ
restore_funccal(ಶೂನ್ಯ)
{
    ಆದರೆ (funccal_ರಾಶಿ == ಖಾಲಿ)
	ಆಂತರಿಕ_error("restore_funccal()");
    ಇಲ್ಲದಿದ್ದರೆ
    {
	current_funccal = funccal_ರಾಶಿ->ತುದಿ_funccal;
	funccal_ರಾಶಿ = funccal_ರಾಶಿ->ಮುಂದಿನ;
    }
}

    funccall_T *
get_current_funccal(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    ಪೂರ್ಣಾಂಕ
at_script_level(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ current_funccal == ಖಾಲಿ && autocmd_ಹೊಂದಿಕೆ == ಖಾಲಿ;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    ಶೂನ್ಯ
delete_script_functions(ಪೂರ್ಣಾಂಕ sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (ಪೂರ್ಣಾಂಕ)KE_SNR;
    len = 3 + vim_snprintf((ಅಕ್ಷರ *)buf + 3, ಗಾತ್ರ(buf) - 3, "%d_", sid);

    ಆಗ (todo > 0)
    {
	todo = func_hashtab.ht_ಬಳಸಿದ;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&func_hashtab, hi, todo)
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    {
		fp = HI2UF(hi);
		ಆದರೆ (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    ಪೂರ್ಣಾಂಕ ಬದಲಾಯಿತು = func_hashtab.ht_ಬದಲಾಯಿತು;

		    fp->uf_flags |= FC_DEAD;

		    ಆದರೆ (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			ಆದರೆ (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			ಆದರೆ (ಬದಲಾಯಿತು != func_hashtab.ht_ಬದಲಾಯಿತು)
			    ನಿಲ್ಲಿಸು;
		    }
		}
		--todo;
	    }
    }
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_all_functions(ಶೂನ್ಯ)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    ಪೂರ್ಣಾಂಕ		ಬದಲಾಯಿತು;

    // Clean up the current_funccal chain and the funccal stack.
    ಆಗ (current_funccal != ಖಾಲಿ)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	ಆದರೆ (current_funccal == ಖಾಲಿ && funccal_ರಾಶಿ != ಖಾಲಿ)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    ಆಗ (todo > 0)
    {
	todo = func_hashtab.ht_ಬಳಸಿದ;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&func_hashtab, hi, todo)
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_ಸ್ಥಿತಿ = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		ಆದರೆ (func_name_refcount(fp->uf_name))
		    ++skipped;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಬದಲಾಯಿತು = func_hashtab.ht_ಬದಲಾಯಿತು;
		    func_clear(fp, TRUE);
		    ಆದರೆ (ಬದಲಾಯಿತು != func_hashtab.ht_ಬದಲಾಯಿತು)
		    {
			skipped = 0;
			ನಿಲ್ಲಿಸು;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    ಆಗ (func_hashtab.ht_ಬಳಸಿದ > skipped)
    {
	todo = func_hashtab.ht_ಬಳಸಿದ;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&func_hashtab, hi, todo)
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		ಆದರೆ (func_name_refcount(fp->uf_name))
		    ++skipped;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			ನಿಲ್ಲಿಸು;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    ಆದರೆ (skipped == 0)
	hash_clear(&func_hashtab);

    free_def_functions();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "name" looks like a builtin function name: starts with a
 * lower case letter, doesn't contain AUTOLOAD_CHAR or ':', no "." after the
 * name.
 * "len" is the length of "name", or -1 for NUL terminated.
 */
    ಪೂರ್ಣಾಂಕ
builtin_function(char_u *name, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	ಆದರೆ (name[i] == AUTOLOAD_CHAR)
	    ಹಿಂತಿರುಗಿ FALSE;
	ಆದರೆ (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    ಆದರೆ (name[i] == '.')
		ಹಿಂತಿರುಗಿ FALSE;
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಪೂರ್ಣಾಂಕ
func_call(
    char_u	*name,
    typval_T	*ವಾದಗಳು,
    partial_T	*partial,
    dict_T	*selfdict,
    typval_T	*rettv)
{
    list_T	*l = ವಾದಗಳು->vval.v_list;
    listitem_T	*ಅಂಶ;
    typval_T	argv[MAX_FUNC_ವಾದಗಳು + 1];
    ಪೂರ್ಣಾಂಕ		argc = 0;
    ಪೂರ್ಣಾಂಕ		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, ಅಂಶ)
    {
	ಆದರೆ (argc == MAX_FUNC_ವಾದಗಳು - (partial == ಖಾಲಿ ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    ನಿಲ್ಲಿಸು;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&ಅಂಶ->li_tv, &argv[argc++]);
    }

    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
    {
	funcexe_T	funcexe;
	ಪೂರ್ಣಾಂಕ		namelen = -1;

	ಆದರೆ (ಒಳ_vim9script())
	{
	    char_u *p = generic_func_ಕಂಡುಹಿಡಿ_open_bracket(name);

	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		ಆದರೆ (skip_generic_func_type_ವಾದಗಳು(&p) == FAIL)
		    ಹೋಗು ಮುಗಿದ;
		namelen = p - name + 1;
	    }
	}

	CLEAR_ಕ್ಷೇತ್ರ(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ಆದರೆ (partial != ಖಾಲಿ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ಆದರೆ (funcexe.fe_object != ಖಾಲಿ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    ಆಗ (argc > 0)
	clear_tv(&argv[--argc]);

ಮುಗಿದ:
    ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ callback_ಆಳ = 0;

    ಪೂರ್ಣಾಂಕ
get_callback_ಆಳ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ callback_ಆಳ;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    ಪೂರ್ಣಾಂಕ
call_callback(
    callback_T	*callback,
    ಪೂರ್ಣಾಂಕ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    ಪೂರ್ಣಾಂಕ		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    ಪೂರ್ಣಾಂಕ		ret;

    ಆದರೆ (callback->cb_name == ಖಾಲಿ || *callback->cb_name == NUL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (callback_ಆಳ > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    CLEAR_ಕ್ಷೇತ್ರ(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    ಆದರೆ (callback->cb_partial != ಖಾಲಿ)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	ಆದರೆ (funcexe.fe_object != ಖಾಲಿ)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_ಆಳ;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_ಆಳ;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    ಆದರೆ (need_rethrow && current_exception != ಖಾಲಿ && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    ಪೂರ್ಣಾಂಕ		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    ಆದರೆ (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	ಹಿಂತಿರುಗಿ -2;

    retval = tv_get_ಸಂಖ್ಯೆ_chk(&rettv, ಖಾಲಿ);
    clear_tv(&rettv);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    ಶೂನ್ಯ
user_func_error(funcerror_T error, char_u *name, ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು_var)
{
    ಆಯ್ಕೆ (error)
    {
	ಸ್ಥಿತಿ FCERR_UNKNOWN:
		ಆದರೆ (ಸಿಕ್ಕಿತು_var)
		    emsg_funcname(e_not_callable_type_str, name);
		ಇಲ್ಲದಿದ್ದರೆ
		    emsg_funcname(e_unknown_function_str, name);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_ವಿಧಾನ_str, name);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_ಒಳ_script_context_str, name);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FCERR_OTHER:
	ಸ್ಥಿತಿ FCERR_FAILED:
		// assume the error message was already given
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FCERR_NONE:
		ನಿಲ್ಲಿಸು;
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
may_check_ವಾದ_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	ಪೂರ್ಣಾಂಕ	    argcount,
	ಪೂರ್ಣಾಂಕ	    ತಳ_included,
	char_u	    *name)
{
    ಆದರೆ (funcexe->fe_check_type != ಖಾಲಿ && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	ಆದರೆ (check_ವಾದ_types(funcexe->fe_check_type,
			  argvars, argcount,
			  ತಳ_included ? ಖಾಲಿ : funcexe->fe_basetv,
			  name) == FAIL)
	    ಹಿಂತಿರುಗಿ FCERR_OTHER;
    }
    ಹಿಂತಿರುಗಿ FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    ಪೂರ್ಣಾಂಕ
call_func(
    char_u	*funcname,	// name of the function
    ಪೂರ್ಣಾಂಕ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    ಪೂರ್ಣಾಂಕ		argcount_ಒಳ,	// number of "argvars"
    typval_T	*argvars_ಒಳ,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    ಪೂರ್ಣಾಂಕ		i;
    ufunc_T	*fp = ಖಾಲಿ;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ಖಾಲಿ;
    char_u	*fname = ಖಾಲಿ;
    char_u	*name = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		argcount = argcount_ಒಳ;
    typval_T	*argvars = argvars_ಒಳ;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ವಾದಗಳು + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    ಪೂರ್ಣಾಂಕ		argv_clear = 0;
    ಪೂರ್ಣಾಂಕ		argv_ತಳ = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_ವಾದಗಳು[MAX_FUNC_ವಾದಗಳು];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_init(&gfatab);

    ಆದರೆ (partial != ಖಾಲಿ)
	fp = partial->pt_func;
    ಆದರೆ (fp == ಖಾಲಿ)
	fp = funcexe->fe_ufunc;

    ಆದರೆ (fp == ಖಾಲಿ)
    {
	ಆದರೆ (ಒಳ_vim9script())
	{
	    char_u *p = generic_func_ಕಂಡುಹಿಡಿ_open_bracket(funcname);

	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		len = p - funcname;
		ಆದರೆ (parse_generic_func_type_ವಾದಗಳು(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == ಖಾಲಿ)
		    ಹೋಗು theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	ಆದರೆ (name == ಖಾಲಿ)
	    ಹೋಗು theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    ಆದರೆ (funcexe->fe_doesrange != ಖಾಲಿ)
	*funcexe->fe_doesrange = FALSE;

    ಆದರೆ (partial != ಖಾಲಿ)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	ಆದರೆ (partial->pt_dict != ಖಾಲಿ && (selfdict == ಖಾಲಿ || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	ಆದರೆ (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    ಗಾಗಿ (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		ಆದರೆ (argv_clear + argcount_ಒಳ >= MAX_FUNC_ವಾದಗಳು)
		{
		    error = FCERR_TOOMANY;
		    ಹೋಗು theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    ಗಾಗಿ (i = 0; i < argcount_ಒಳ; ++i)
		argv[i + argv_clear] = argvars_ಒಳ[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_ಒಳ;

	    ಆದರೆ (funcexe->fe_check_type != ಖಾಲಿ
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_ವಾದಗಳು = check_type_ವಾದಗಳು;
		CLEAR_ಕ್ಷೇತ್ರ(check_type_ವಾದಗಳು);
		ಗಾಗಿ (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_ವಾದಗಳು[i + partial->pt_argc] =
							 check_type.tt_ವಾದಗಳು[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    ಆದರೆ (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_ವಾದ_types(funcexe, argvars, argcount, FALSE,
					     (name != ಖಾಲಿ) ? name : funcname);

    ಆದರೆ (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	ಪೂರ್ಣಾಂಕ	is_global = FALSE;

	// Skip "g:" before a function name.
	ಆದರೆ (fp == ಖಾಲಿ && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_ಸಂಖ್ಯೆ;	// default rettv is number zero
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
	error = FCERR_UNKNOWN;

	ಆದರೆ (fp != ಖಾಲಿ || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    ಆದರೆ (fp == ಖಾಲಿ)
	    {
		fp = ಕಂಡುಹಿಡಿ_func(rfname, is_global);
		ಆದರೆ (fp != ಖಾಲಿ && !is_global && ಒಳ_vim9script()
						 && func_requires_g_ಪೂರ್ವಪ್ರತ್ಯಯ(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = ಖಾಲಿ;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    ಆದರೆ (fp == ಖಾಲಿ
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, ಖಾಲಿ)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = ಕಂಡುಹಿಡಿ_func(rfname, is_global);
	    }
	    // Try loading a package.
	    ಆದರೆ (fp == ಖಾಲಿ && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = ಕಂಡುಹಿಡಿ_func(rfname, is_global);
	    }
	    ಆದರೆ (fp == ಖಾಲಿ)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		ಆದರೆ (p != ಖಾಲಿ && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = ಕಂಡುಹಿಡಿ_func(p, is_global);
	    }

	    ಆದರೆ (partial == ಖಾಲಿ && fp != ಖಾಲಿ && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		ಆದರೆ (fp == ಖಾಲಿ)
		{
		    error = FCERR_FAILED;
		    ಹೋಗು theend;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_ಗಾತ್ರ(&gfatab) > 0)
	    {
		emsg_funcname(fp != ಖಾಲಿ ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		ಹೋಗು theend;
	    }

	    ಆದರೆ (fp != ಖಾಲಿ && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ need_arg_check = FALSE;
		ಆದರೆ (funcexe->fe_check_type == ಖಾಲಿ)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		ಆದರೆ (funcexe->fe_argv_func != ಖಾಲಿ)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		ಆದರೆ (funcexe->fe_basetv != ಖಾಲಿ)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, ಗಾತ್ರ(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_ತಳ = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		ಆದರೆ (need_arg_check)
		    error = may_check_ವಾದ_types(funcexe, argvars, argcount,
				       TRUE, (name != ಖಾಲಿ) ? name : funcname);

		ಆದರೆ (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (funcexe->fe_basetv != ಖಾಲಿ)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_ಆಂತರಿಕ_ವಿಧಾನ(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Find the function name in the table, call its implementation.
	     */
	    error = call_ಆಂತರಿಕ_func(fname, argcount, argvars, rettv);
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
    ಆದರೆ (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    ಆದರೆ (!aborting())
	user_func_error(error, (name != ಖಾಲಿ) ? name : funcname,
							funcexe->fe_ಸಿಕ್ಕಿತು_var);

    // clear the copies made from the partial
    ಆಗ (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_ತಳ]);

    vim_free(tofree);
    vim_free(name);
    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_clear(&gfatab);

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    ಪೂರ್ಣಾಂಕ
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ಖಾಲಿ;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    ಪೂರ್ಣಾಂಕ		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;	// default rettv is number zero
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    ಆದರೆ (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rfname = fname;
    fp = ಕಂಡುಹಿಡಿ_func(rfname, is_global);
    ಆದರೆ (fp != ಖಾಲಿ && !is_global && ಒಳ_vim9script()
						 && func_requires_g_ಪೂರ್ವಪ್ರತ್ಯಯ(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = ಖಾಲಿ;
    ಆದರೆ (fp == ಖಾಲಿ)
	ret = NOTDONE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp != ಖಾಲಿ && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp != ಖಾಲಿ)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_ಕ್ಷೇತ್ರ(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, ಖಾಲಿ);
	ಆದರೆ (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    ಹಿಂತಿರುಗಿ ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    ಹಿಂತಿರುಗಿ fp->uf_name_exp != ಖಾಲಿ ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
function_list_modified(ಪೂರ್ಣಾಂಕ prev_ht_ಬದಲಾಯಿತು)
{
    ಆದರೆ (prev_ht_ಬದಲಾಯಿತು != func_hashtab.ht_ಬದಲಾಯಿತು)
    {
	emsg(_(e_function_list_was_modified));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
list_func_ತಲೆ(ufunc_T *fp, ಪೂರ್ಣಾಂಕ indent)
{
    ಪೂರ್ಣಾಂಕ		prev_ht_ಬದಲಾಯಿತು = func_hashtab.ht_ಬದಲಾಯಿತು;
    ಪೂರ್ಣಾಂಕ		j;

    ಸಂದೇಶ_ಆರಂಭ();

    // a timer at the more prompt may have deleted the function
    ಆದರೆ (function_list_modified(prev_ht_ಬದಲಾಯಿತು))
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (indent)
	ಸಂದೇಶ_puts("   ");
    ಆದರೆ (fp->uf_def_ಸ್ಥಿತಿ != UF_NOT_COMPILED)
	ಸಂದೇಶ_puts("def ");
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_puts("function ");
    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)printable_func_name(fp));
    ಸಂದೇಶ_putchar('(');
    ಗಾಗಿ (j = 0; j < fp->uf_ವಾದಗಳು.ga_len; ++j)
    {
	ಆದರೆ (j)
	    ಸಂದೇಶ_puts(", ");
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)FUNCARG(fp, j));
	ಆದರೆ (fp->uf_arg_types != ಖಾಲಿ)
	{
	    ಅಕ್ಷರ *tofree;

	    ಸಂದೇಶ_puts(": ");
	    ಸಂದೇಶ_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ಆದರೆ (j >= fp->uf_ವಾದಗಳು.ga_len - fp->uf_def_ವಾದಗಳು.ga_len)
	{
	    ಸಂದೇಶ_puts(" = ");
	    ಸಂದೇಶ_puts(((ಅಕ್ಷರ **)(fp->uf_def_ವಾದಗಳು.ga_ದತ್ತಾಂಶ))
		       [j - fp->uf_ವಾದಗಳು.ga_len + fp->uf_def_ವಾದಗಳು.ga_len]);
	}
    }
    ಆದರೆ (fp->uf_varargs)
    {
	ಆದರೆ (j)
	    ಸಂದೇಶ_puts(", ");
	ಸಂದೇಶ_puts("...");
    }
    ಆದರೆ (fp->uf_va_name != ಖಾಲಿ)
    {
	ಆದರೆ (!fp->uf_varargs)
	{
	    ಆದರೆ (j)
		ಸಂದೇಶ_puts(", ");
	    ಸಂದೇಶ_puts("...");
	}
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)fp->uf_va_name);
	ಆದರೆ (fp->uf_va_type != ಖಾಲಿ)
	{
	    ಅಕ್ಷರ *tofree;

	    ಸಂದೇಶ_puts(": ");
	    ಸಂದೇಶ_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    ಸಂದೇಶ_putchar(')');

    ಆದರೆ (fp->uf_def_ಸ್ಥಿತಿ != UF_NOT_COMPILED)
    {
	ಆದರೆ (fp->uf_ret_type != &t_void)
	{
	    ಅಕ್ಷರ *tofree;

	    ಸಂದೇಶ_puts(": ");
	    ಸಂದೇಶ_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp->uf_flags & FC_ABORT)
	ಸಂದೇಶ_puts(" abort");
    ಆದರೆ (fp->uf_flags & FC_RANGE)
	ಸಂದೇಶ_puts(" range");
    ಆದರೆ (fp->uf_flags & FC_DICT)
	ಸಂದೇಶ_puts(" dict");
    ಆದರೆ (fp->uf_flags & FC_ಆವರಣ)
	ಸಂದೇಶ_puts(" closure");
    ಸಂದೇಶ_clr_eos();
    ಆದರೆ (p_verbose > 0)
	ಕೊನೆಯ_ಗಣ_ಸಂದೇಶ(fp->uf_script_ctx);

    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ		*is_global,
    ಪೂರ್ಣಾಂಕ		skip,		// only find the end, don't evaluate
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಹಿಂತಿರುಗಿ trans_function_name_ext(pp, is_global, skip, flags,
	    ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    ಸ್ಥಿರ char_u *
trans_function_name_ext(
    char_u	**pp,
    ಪೂರ್ಣಾಂಕ		*is_global,
    ಪೂರ್ಣಾಂಕ		skip,		// only find the end, don't evaluate
    ಪೂರ್ಣಾಂಕ		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = ಖಾಲಿ;
    char_u	*ಆರಂಭ;
    char_u	*ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		lead;
    char_u	sid_buf[20];
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		extra = 0;
    ಪೂರ್ಣಾಂಕ		ಪೂರ್ವಪ್ರತ್ಯಯ_g = FALSE;
    lval_T	lv;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    ಪೂರ್ಣಾಂಕ		vim9_local;

    ಆದರೆ (fdp != ಖಾಲಿ)
	CLEAR_ಸೂಚಕ(fdp);
    ಆರಂಭ = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    ಆದರೆ ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (ಪೂರ್ಣಾಂಕ)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	ಹಿಂತಿರುಗಿ vim_strnsave(ಆರಂಭ, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(ಆರಂಭ);
    ಆದರೆ (lead > 2)
	ಆರಂಭ += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    ಕೊನೆ = get_lval(ಆರಂಭ, ಖಾಲಿ, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_ಆರಂಭ);
    ಆದರೆ (ಕೊನೆ == ಆರಂಭ || (vim9script && ಕೊನೆ != ಖಾಲಿ
				   && ಕೊನೆ[-1] == AUTOLOAD_CHAR && *ಕೊನೆ == '('))
    {
	ಆದರೆ (!skip)
	    emsg(_(e_function_name_required));
	ಹೋಗು theend;
    }
    ಆದರೆ (ಕೊನೆ == ಖಾಲಿ || (lv.ll_tv != ಖಾಲಿ && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	ಆದರೆ (!aborting())
	{
	    ಆದರೆ (ಕೊನೆ != ಖಾಲಿ)
		semsg(_(e_invalid_ವಾದ_str), ಆರಂಭ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *pp = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(ಆರಂಭ, ಖಾಲಿ, ಖಾಲಿ, FNE_INCL_BR);
	ಹೋಗು theend;
    }

    ಆದರೆ (lv.ll_ufunc != ಖಾಲಿ)
    {
	ಆದರೆ (ufunc != ಖಾಲಿ)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = ಕೊನೆ;
	ಹೋಗು theend;
    }

    ಆದರೆ (lv.ll_tv != ಖಾಲಿ)
    {
	ಆದರೆ (fdp != ಖಾಲಿ)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = ಖಾಲಿ;
	    fdp->fd_di = lv.ll_di;
	}
	ಆದರೆ (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != ಖಾಲಿ)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = ಕೊನೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != ಖಾಲಿ)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.ಉದ್ದಳತೆ);
	    *pp = ಕೊನೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != ಖಾಲಿ)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = ಕೊನೆ;
	    ಆದರೆ (partial != ಖಾಲಿ)
		*partial = lv.ll_tv->vval.v_partial;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (!skip && !(flags & TFN_QUIET) && (fdp == ಖಾಲಿ
			     || lv.ll_dict == ಖಾಲಿ || fdp->fd_newkey == ಖಾಲಿ))
		emsg(_(e_funcref_required));
	    ಇಲ್ಲದಿದ್ದರೆ
		*pp = ಕೊನೆ;
	    name = ಖಾಲಿ;
	}
	ಹೋಗು theend;
    }

    ಆದರೆ (lv.ll_name == ಖಾಲಿ)
    {
	// Error found, but continue after the function name.
	*pp = ಕೊನೆ;
	ಹೋಗು theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    ಆದರೆ (lv.ll_exp_name != ಖಾಲಿ)
    {
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ಖಾಲಿ);
	ಆದರೆ (name == lv.ll_exp_name)
	    name = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ಅಂಶ(lv.ll_sid);
	ಪೂರ್ಣಾಂಕ		cc = *lv.ll_name_ಕೊನೆ;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_ಕೊನೆ = NUL;
	ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
	{
	    name = concat_str(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ, lv.ll_name);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (ಪೂರ್ಣಾಂಕ)KE_SNR;
	    vim_snprintf((ಅಕ್ಷರ *)sid_buf + 3, ಗಾತ್ರ(sid_buf) - 3,
						      "%ld_", (ದೀರ್ಘ)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_ಕೊನೆ = cc;
	*pp = ಕೊನೆ;
	ಹೋಗು theend;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(flags & TFN_NO_DEREF))
    {
	len = (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ಖಾಲಿ);
	ಆದರೆ (name == *pp)
	    name = ಖಾಲಿ;
    }
    ಆದರೆ (name != ಖಾಲಿ)
    {
	name = vim_strsave(name);
	*pp = ಕೊನೆ;
	ಆದರೆ (name != ಖಾಲಿ && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (ಪೂರ್ಣಾಂಕ)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	ಹೋಗು theend;
    }

    ಆದರೆ (lv.ll_exp_name != ಖಾಲಿ)
    {
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(lv.ll_exp_name);
	ಆದರೆ (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// skip over "s:" and "g:"
	ಆದರೆ (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    ಆದರೆ (lv.ll_name[0] == 'g')
	    {
		ಆದರೆ (is_global != ಖಾಲಿ)
		{
		    *is_global = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    ಪೂರ್ವಪ್ರತ್ಯಯ_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - lv.ll_name);
    }
    ಆದರೆ (len <= 0)
    {
	ಆದರೆ (!skip)
	    emsg(_(e_function_name_required));
	ಹೋಗು theend;
    }

    // In Vim9 script a user function is script-local by default, unless it
    // starts with a lower case character: dict.func().  Or when in a class.
    vim9_local = ASCII_ISUPPER(*ಆರಂಭ) && vim9script
						&& (flags & TFN_ಒಳ_CLASS) == 0;

    /*
     * Copy the function name to allocated memory.
     * Accept <SID>name() inside a script, translate into <SNR>123_name().
     * Accept <SNR>123_name() outside a script.
     */
    ಆದರೆ (skip)
	lead = 0;	// do nothing
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lead > 0 || vim9_local)
    {
	ಆದರೆ (!vim9_local)
	{
	    ಆದರೆ (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_ಆರಂಭ_with_capital_str), ಆರಂಭ);
		ಹೋಗು theend;
	    }
	    lead = 3;
	}
	ಆದರೆ (vim9_local || (lv.ll_exp_name != ಖಾಲಿ
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    ಆದರೆ (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_ಒಳ_script_context));
		ಹೋಗು theend;
	    }
	    sid_buflen = vim_snprintf((ಅಕ್ಷರ *)sid_buf, ಗಾತ್ರ(sid_buf), "%ld_", (ದೀರ್ಘ)current_sctx.sc_sid);
	    ಆದರೆ (vim9_local)
		extra = 3 + (ಪೂರ್ಣಾಂಕ)sid_buflen;
	    ಇಲ್ಲದಿದ್ದರೆ
		lead += (ಪೂರ್ಣಾಂಕ)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_ಒಳ_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_ಆರಂಭ_with_capital_str
			   : e_function_name_must_ಆರಂಭ_with_capital_or_s_str),
									ಆರಂಭ);
	ಹೋಗು theend;
    }
    ಆದರೆ (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	ಆದರೆ (cp != ಖಾಲಿ && cp < ಕೊನೆ)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), ಆರಂಭ);
	    ಹೋಗು theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    ಆದರೆ (name != ಖಾಲಿ)
    {
	ಆದರೆ (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (ಪೂರ್ಣಾಂಕ)KE_SNR;
	    ಆದರೆ (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ_g)
	{
	    name[0] = 'g';
	    name[1] = ':';
	}
	mch_memmove(name + lead + extra, lv.ll_name, (size_t)len);
	name[lead + extra + len] = NUL;
    }
    *pp = ಕೊನೆ;

theend:
    clear_lval(&lv);
    ಹಿಂತಿರುಗಿ name;
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

    ಆದರೆ (*name == K_SPECIAL && ಒಳ_vim9script())
    {
	p = vim_strchr(name, '_');
	ಆದರೆ (p != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ p + 1;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
    ಅಕ್ಷರ	sid_buf[25];
    size_t	sid_buflen;
    ಪೂರ್ಣಾಂಕ		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    ಆದರೆ (funcname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	ಆದರೆ (!ಒಳ_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ufunc = ಕಂಡುಹಿಡಿ_func(funcname, FALSE);
	ಆದರೆ (ufunc == ಖಾಲಿ || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	++p;
	off = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	off = *funcname == 's' ? 2 : 5;

    ಆದರೆ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_ಒಳ_script_context));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, ಗಾತ್ರ(sid_buf), "<SNR>%ld_",
	    (ದೀರ್ಘ)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    ಆದರೆ (newname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    vim_snprintf((ಅಕ್ಷರ *)newname, newnamesize, "%s%s", sid_buf, p + off);

    ಹಿಂತಿರುಗಿ newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    ಆದರೆ (n != ಖಾಲಿ)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	ಪೂರ್ಣಾಂಕ	    *is_global,
	ಪೂರ್ಣಾಂಕ	    skip,
	ಪೂರ್ಣಾಂಕ	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    ಆದರೆ (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(ಶೂನ್ಯ)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	ಆದರೆ (fudi != ಖಾಲಿ)
	    CLEAR_ಸೂಚಕ(fudi);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    *name = p;
    ಹಿಂತಿರುಗಿ saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    ಶೂನ್ಯ
list_functions(regmatch_T *regmatch)
{
    ಪೂರ್ಣಾಂಕ		prev_ht_ಬದಲಾಯಿತು = func_hashtab.ht_ಬದಲಾಯಿತು;
    long_u	todo = func_hashtab.ht_ಬಳಸಿದ;
    hashitem_T	*hi;

    ಗಾಗಿ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    ಆದರೆ ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == ಖಾಲಿ
			? !ಸಂದೇಶ_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !ಸುರಕ್ಷಿತ_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		ಆದರೆ (list_func_ತಲೆ(fp, FALSE) == FAIL)
		    ಹಿಂತಿರುಗಿ;
		ಆದರೆ (function_list_modified(prev_ht_ಬದಲಾಯಿತು))
		    ಹಿಂತಿರುಗಿ;
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
    ಆದರೆ (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    ಆದರೆ (*p == '/')
	++p;

    ಹಿಂತಿರುಗಿ p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    ಸ್ಥಿರ ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, ಪೂರ್ಣಾಂಕ is_global)
{
    ufunc_T	*fp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		j;

    ಆದರೆ (!ತುದಿಗಳು_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಗಣ_nextcmd(eap, p);

    ಆದರೆ (eap->nextcmd != ಖಾಲಿ)
	*p = NUL;

    ಆದರೆ (eap->skip || got_int)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    fp = ಕಂಡುಹಿಡಿ_func(name, is_global);
    ಆದರೆ (fp == ಖಾಲಿ && ASCII_ISUPPER(*eap->arg))
    {
	char_u *ಮೇಲೆ = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	ಆದರೆ (ಮೇಲೆ != ಖಾಲಿ)
	    fp = ಕಂಡುಹಿಡಿ_func(ಮೇಲೆ, FALSE);
    }

    ಆದರೆ (fp == ಖಾಲಿ)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    ಪೂರ್ಣಾಂಕ prev_ht_ಬದಲಾಯಿತು = func_hashtab.ht_ಬದಲಾಯಿತು;

    ಆದರೆ (list_func_ತಲೆ(fp, TRUE) != OK)
	ಹಿಂತಿರುಗಿ fp;

    ಗಾಗಿ (j = 0; j < fp->uf_ಸಾಲುಗಳು.ga_len && !got_int; ++j)
    {
	ಆದರೆ (FUNCLINE(fp, j) == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಸಂದೇಶ_putchar('\n');
	ಸಂದೇಶ_outnum((ದೀರ್ಘ)(j + 1));
	ಆದರೆ (j < 9)
	    ಸಂದೇಶ_putchar(' ');
	ಆದರೆ (j < 99)
	    ಸಂದೇಶ_putchar(' ');
	ಆದರೆ (function_list_modified(prev_ht_ಬದಲಾಯಿತು))
	    ನಿಲ್ಲಿಸು;
	ಸಂದೇಶ_prt_ಸಾಲು(FUNCLINE(fp, j), FALSE);
	ಹೊರಹೋಗು_flush();	// show a line at a time
	ui_breakcheck();
    }

    ಆದರೆ (!got_int)
    {
	ಸಂದೇಶ_putchar('\n');
	ಆದರೆ (!function_list_modified(prev_ht_ಬದಲಾಯಿತು))
	{
	    ಆದರೆ (fp->uf_def_ಸ್ಥಿತಿ != UF_NOT_COMPILED)
		ಸಂದೇಶ_puts("   enddef");
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ_puts("   endfunction");
	}
    }

    ಹಿಂತಿರುಗಿ fp;
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
	garray_T    *ಸಾಲುಗಳು_ಗಮ್ಯ_free,
	ಪೂರ್ಣಾಂಕ	    class_flags,
	ocmember_T  *obj_members,
	ಪೂರ್ಣಾಂಕ         obj_member_ಎಣಿಕೆ,
	cctx_T	    *cctx)
{
    ಪೂರ್ಣಾಂಕ		j;
    ಪೂರ್ಣಾಂಕ		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    ಪೂರ್ಣಾಂಕ		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*ಸಾಲು_arg = ಖಾಲಿ;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_ವಾದಗಳು;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    ಪೂರ್ಣಾಂಕ		varargs = FALSE;
    ಪೂರ್ಣಾಂಕ		flags = 0;
    char_u	*ret_type = ಖಾಲಿ;
    ufunc_T	*fp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		fp_allocated = FALSE;
    ಪೂರ್ಣಾಂಕ		free_fp = FALSE;
    ಪೂರ್ಣಾಂಕ		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	func_nr = 0;	    // number for nameless function
    ಪೂರ್ಣಾಂಕ		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_ತುದಿ;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    imported_T	*import = ಖಾಲಿ;

    // ":function" without argument: list functions.
    ಆದರೆ (ತುದಿಗಳು_excmd2(eap->cmd, eap->arg))
    {
	ಆದರೆ (!eap->skip)
	    list_functions(ಖಾಲಿ);
	ಗಣ_nextcmd(eap, eap->arg);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    ಆದರೆ (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	ಗಣ_nextcmd(eap, p);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ga_init(&newargs);
    ga_init(&argtypes);
    ga_init(&arg_objm);
    ga_init(&default_ವಾದಗಳು);
    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_init(&gfatab);

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
    ಆದರೆ (name_arg != ಖಾಲಿ)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_ಕ್ಷೇತ್ರ(fudi);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (vim9script)
	{
	    ಆದರೆ (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_ಒಳ_vim9_script_str), p);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    p = ಗಮ್ಯ_name_ಕೊನೆ(p, TRUE);
	    ಆದರೆ (*skipwhite(p) == '.' && vim_strchr(p, '(') != ಖಾಲಿ)
	    {
		semsg(_(e_cannot_define_dict_func_ಒಳ_vim9_script_str),
								     eap->arg);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }

	    p = eap->arg;
	}

	ಪೂರ್ಣಾಂಕ tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_ಒಳ_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != ಖಾಲಿ);
	ಆದರೆ (name == ಖಾಲಿ && (fudi.fd_dict == ಖಾಲಿ || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    ಆದರೆ (!aborting())
	    {
		ಆದರೆ (!eap->skip && fudi.fd_newkey != ಖಾಲಿ)
		    semsg(_(e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	ಆದರೆ (is_export && name != ಖಾಲಿ)
	{
	    char_u *prefixed = may_ಪೂರ್ವಪ್ರತ್ಯಯ_autoload(name);

	    ಆದರೆ (prefixed != ಖಾಲಿ && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (paren && vim9script && name != ಖಾಲಿ
				    && vim_strchr(name, AUTOLOAD_CHAR) != ಖಾಲಿ)
	{
	    emsg(_(e_cannot_use_name_with_hash_ಒಳ_vim9_script_use_export_instead));
	    ಹೋಗು ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    ಆದರೆ (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	ಹೋಗು ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    ಆದರೆ (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹೋಗು ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    ಆದರೆ (*p != '(')
    {
	ಆದರೆ (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    ಹೋಗು ret_free;
	}
	// attempt to continue by skipping some text
	ಆದರೆ (vim_strchr(p, '(') != ಖಾಲಿ)
	    p = vim_strchr(p, '(');
    }

    ಆದರೆ ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	ಹೋಗು ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    ಆದರೆ (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	ಹೋಗು ret_free;
    }

    ga_init2(&newlines, ಗಾತ್ರ(char_u *), 10);

    ಆದರೆ (!eap->skip && name_arg == ಖಾಲಿ)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	ಆದರೆ (name != ಖಾಲಿ)
	    arg = name;
	ಇಲ್ಲದಿದ್ದರೆ
	    arg = fudi.fd_newkey;
	ಆದರೆ (arg != ಖಾಲಿ && (fudi.fd_di == ಖಾಲಿ
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_ತಳ = arg;
	    ಪೂರ್ಣಾಂಕ	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    ಆದರೆ (arg != fudi.fd_newkey)
	    {
		ಆದರೆ (*arg == K_SPECIAL)
		{
		    name_ತಳ = vim_strchr(arg, '_');
		    ಆದರೆ (name_ತಳ == ಖಾಲಿ)
			name_ತಳ = arg + 3;
		    ಇಲ್ಲದಿದ್ದರೆ
			++name_ತಳ;
		}
		ಗಾಗಿ (i = 0; name_ತಳ[i] != NUL && (i == 0
					    ? eval_isnamec1(name_ತಳ[i])
					    : eval_isnamec(name_ತಳ[i])); ++i)
		    ;
		ಆದರೆ (name_ತಳ[i] != NUL)
		{
		    emsg_funcname(e_invalid_ವಾದ_str, arg);
		    ಹೋಗು ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		ಆದರೆ (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_ತಳ, i, 0, ಖಾಲಿ,
			ಖಾಲಿ, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_ಅಂಶ_str), name_ತಳ);
		    ಹೋಗು ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	ಆದರೆ (fudi.fd_dict != ಖಾಲಿ && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_ಇಲ್ಲಿ));
	    ಹೋಗು ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    ಆದರೆ (get_function_ವಾದಗಳು(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : ಖಾಲಿ, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : ಖಾಲಿ,
			 ಖಾಲಿ, &varargs, &default_ವಾದಗಳು, eap->skip,
			 eap, class_flags, &newlines, ಸಾಲುಗಳು_ಗಮ್ಯ_free) == FAIL)
	ಹೋಗು errret_2;
    whitep = p;

    ಆದರೆ (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	ಆದರೆ (*skipwhite(p) == ':')
	{
	    ಆದರೆ (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    ಆದರೆ (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = ಖಾಲಿ;
	    }
	}
	p = skipwhite(p);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// find extra arguments "range", "dict", "abort" and "closure"
	ಗಾಗಿ (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    ಆದರೆ (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_ಆವರಣ;
		p += 7;
		ಆದರೆ (current_funccal == ಖಾಲಿ)
		{
		    emsg_funcname(e_ಆವರಣ_function_should_not_be_at_ತುದಿ_level_str,
			    name == ಖಾಲಿ ? (char_u *)"" : name);
		    ಹೋಗು erret;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    ಆದರೆ (*p == '\n')
	ಸಾಲು_arg = p + 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p != NUL
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
    ಆದರೆ (ಕೀಲಿ_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	ಆದರೆ (!eap->skip && !eap->forceit)
	{
	    ಆದರೆ (fudi.fd_dict != ಖಾಲಿ && fudi.fd_newkey == ಖಾಲಿ)
		emsg(_(e_dictionary_ನಮೂದು_already_exists));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name != ಖಾಲಿ && ಕಂಡುಹಿಡಿ_func(name, is_global) != ಖಾಲಿ)
		emsg_funcname(e_function_str_already_exists_ಕೂಡಿಸು_bang_ಗಮ್ಯ_replace, name);
	}

	ಆದರೆ (!eap->skip && did_emsg)
	    ಹೋಗು erret;

	ಸಂದೇಶ_putchar('\n');	    // don't overwrite the function name
	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
    }

    // Save the starting line number.
    sourcing_lnum_ತುದಿ = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    ಆದರೆ (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_ವಿಧಾನ) == 0
		&& (get_function_body(eap, &newlines, ಸಾಲು_arg, ಸಾಲುಗಳು_ಗಮ್ಯ_free)
							== FAIL || did_emsg))
	    || eap->skip)
	ಹೋಗು erret;

    /*
     * If there are no errors, add the function
     */
    ಆದರೆ (fudi.fd_dict != ಖಾಲಿ)
    {
	ಅಕ್ಷರ	numbuf[NUMBUFLEN];

	fp = ಖಾಲಿ;
	ಆದರೆ (fudi.fd_newkey == ಖಾಲಿ && !eap->forceit)
	{
	    emsg(_(e_dictionary_ನಮೂದು_already_exists));
	    ಹೋಗು erret;
	}
	ಆದರೆ (fudi.fd_di == ಖಾಲಿ)
	{
	    // Can't add a function to a locked dictionary
	    ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(fudi.fd_dict->dv_ಬೀಗ, eap->arg, FALSE))
		ಹೋಗು erret;
	}
	    // Can't change an existing function if it is locked
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(fudi.fd_di->di_tv.v_ಬೀಗ, eap->arg, FALSE))
	    ಹೋಗು erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, ಗಾತ್ರ(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	ಆದರೆ (name == ಖಾಲಿ)
	    ಹೋಗು erret;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*ಕಂಡುಹಿಡಿ_name = name;
	ಪೂರ್ಣಾಂಕ		var_conflict = FALSE;
	ಪೂರ್ಣಾಂಕ		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = ಕಂಡುಹಿಡಿ_var(name, &ht, TRUE);
	ಆದರೆ (v != ಖಾಲಿ && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	ಆದರೆ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);

	    ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
	    {
		ಆದರೆ (is_export)
		{
		    ಕಂಡುಹಿಡಿ_name = name + STRLEN(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ);
		    v = ಕಂಡುಹಿಡಿ_var(ಕಂಡುಹಿಡಿ_name, &ht, TRUE);
		    ಆದರೆ (v != ಖಾಲಿ)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    char_u *prefixed = may_ಪೂರ್ವಪ್ರತ್ಯಯ_autoload(name);

		    ಆದರೆ (prefixed != ಖಾಲಿ && prefixed != name)
		    {
			v = ಕಂಡುಹಿಡಿ_var(prefixed, &ht, TRUE);
			ಆದರೆ (v != ಖಾಲಿ)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	ಆದರೆ (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    ಹೋಗು erret;
	}

	fp = ಕಂಡುಹಿಡಿ_func_even_dead(ಕಂಡುಹಿಡಿ_name, ffed_flags);
	ಆದರೆ (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = ಕಂಡುಹಿಡಿ_imported(uname == ಖಾಲಿ ? name : uname, 0, FALSE);
	}

	ಆದರೆ (fp != ಖಾಲಿ || import != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ dead = fp != ಖಾಲಿ && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    ಆದರೆ (import != ಖಾಲಿ
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_ತುದಿ;
		ಆದರೆ (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		ಇಲ್ಲದಿದ್ದರೆ
		    emsg_funcname(e_function_str_already_exists_ಕೂಡಿಸು_bang_ಗಮ್ಯ_replace, name);
		ಹೋಗು errret_keep;
	    }
	    ಆದರೆ (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_ಒಳ_use, name);
		ಹೋಗು errret_keep;
	    }
	    ಆದರೆ (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = ಖಾಲಿ;
		overwrite = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = ಖಾಲಿ;
		func_clear_ವಸ್ತುಗಳು(fp);
		fp->uf_name_exp = exp_name;
		fp->uf_flags &= ~FC_DEAD;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
		fp->uf_profiling = FALSE;
		fp->uf_prof_initialized = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
		fp->uf_def_ಸ್ಥಿತಿ = UF_NOT_COMPILED;
	    }
	}
    }

    ಆದರೆ (fp == ಖಾಲಿ)
    {
	ಆದರೆ (fudi.fd_dict == ಖಾಲಿ && vim_strchr(name, AUTOLOAD_CHAR) != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    ಆದರೆ (SOURCING_NAME != ಖಾಲಿ)
	    {
		scriptname = autoload_name(name);
		ಆದರೆ (scriptname != ಖಾಲಿ)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
		    slen = (ಪೂರ್ಣಾಂಕ)STRLEN(SOURCING_NAME);
		    ಆದರೆ (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    ಆದರೆ (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_ತುದಿ;
		semsg(_(e_function_name_does_not_ಹೊಂದಿಕೆ_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		ಹೋಗು erret;
	    }
	}

	ಆದರೆ (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	ಆದರೆ (fp == ಖಾಲಿ)
	    ಹೋಗು erret;
	fp_allocated = TRUE;

	ಆದರೆ (fudi.fd_dict != ಖಾಲಿ)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    ಆದರೆ (func_name == ಖಾಲಿ)
	    {
		VIM_CLEAR(fp);
		ಹೋಗು erret;
	    }
	    ಆದರೆ (fudi.fd_di == ಖಾಲಿ)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		ಆದರೆ (fudi.fd_di == ಖಾಲಿ)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    ಹೋಗು erret;
		}
		ಆದರೆ (dict_ಕೂಡಿಸು(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    ಹೋಗು erret;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// overwrite existing dict entry
		clear_tv(&fudi.fd_di->di_tv);
	    fudi.fd_di->di_tv.v_type = VAR_FUNC;
	    fudi.fd_di->di_tv.vval.v_string = func_name;

	    // behave like "dict" was used
	    flags |= FC_DICT;
	}
    }
    fp->uf_ವಾದಗಳು = newargs;
    fp->uf_def_ವಾದಗಳು = default_ವಾದಗಳು;
    fp->uf_ret_type = &t_any;
    fp->uf_func_type = &t_func_any;

    ಆದರೆ (eap->cmdidx == CMD_def)
    {
	ಪೂರ್ಣಾಂಕ	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_ಸ್ಥಿತಿ = UF_ಗಮ್ಯ_BE_COMPILED;

	ಆದರೆ (generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_ಗಾತ್ರ(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_ತುದಿ;

	// The function may use script variables from the context.
	function_using_ಖಂಡ_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	ಪೂರ್ಣಾಂಕ save_is_export = is_export;
	is_export = FALSE;

	ಆದರೆ (parse_ವಾದ_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_ಎಣಿಕೆ, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    ಹೋಗು erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	ಆದರೆ (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    ಹೋಗು erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	fp->uf_def_ಸ್ಥಿತಿ = UF_NOT_COMPILED;

    ಆದರೆ (fp_allocated)
    {
	// insert the new function in the function list
	ಆದರೆ (overwrite)
	{
	    hi = hash_ಕಂಡುಹಿಡಿ(&func_hashtab, name);
	    hi->hi_ಕೀಲಿ = UF2HIKEY(fp);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (class_flags == 0 && hash_ಕೂಡಿಸು(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    ಹೋಗು erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_ಸಾಲುಗಳು = newlines;
    newlines.ga_ದತ್ತಾಂಶ = ಖಾಲಿ;
    ಆದರೆ ((flags & FC_ಆವರಣ) != 0)
    {
	ಆದರೆ (ರಿಜಿಸ್ಟರ್_ಆವರಣ(fp) == FAIL)
	    ಹೋಗು erret;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	fp->uf_scoped = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (prof_def_func())
	func_do_profile(fp);
#ಆದರೆ_ಅಂತ್ಯ
    fp->uf_varargs = varargs;
    ಆದರೆ (sandbox)
	flags |= FC_SANDBOX;
    ಆದರೆ (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_ಆವೃತ್ತಿ = current_sctx.sc_ಆವೃತ್ತಿ;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_ತುದಿ;
    ಆದರೆ (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    ಆದರೆ (eap->cmdidx == CMD_def)
	ಗಣ_function_type(fp);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fp->uf_script_ctx.sc_ಆವೃತ್ತಿ == SCRIPT_ಆವೃತ್ತಿ_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    ಆದರೆ (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, ಖಾಲಿ);

    ಹೋಗು ret_free;

erret:
    ಆದರೆ (fp != ಖಾಲಿ)
    {
	// these were set to "newargs" and "default_args", which are cleared
	//ಕೆಳಗೆ
	ga_init(&fp->uf_ವಾದಗಳು);
	ga_init(&fp->uf_def_ವಾದಗಳು);
    }
errret_2:
    ಆದರೆ (fp != ಖಾಲಿ)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	ಆದರೆ (IS_GENERIC_FUNC(fp))
	    generic_func_clear_ವಸ್ತುಗಳು(fp);
    }
    ಆದರೆ (free_fp)
	VIM_CLEAR(fp);
errret_keep:
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&newargs);
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&default_ವಾದಗಳು);
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&newlines);
ret_free:
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&argtypes);
    ga_clear(&arg_objm);
    // The generic types are still in use and should not be freed.  Instead
    // clear the grow array.
    ga_clear(&gfatab.gfat_param_types);
    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_clear(&gfatab);
    vim_free(fudi.fd_newkey);
    ಆದರೆ (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    ಹಿಂತಿರುಗಿ fp;
}

/*
 * ":function"
 */
    ಶೂನ್ಯ
ex_function(exarg_T *eap)
{
    garray_T ಸಾಲುಗಳು_ಗಮ್ಯ_free;

    ga_init2(&ಸಾಲುಗಳು_ಗಮ್ಯ_free, ಗಾತ್ರ(char_u *), 50);
    (ಶೂನ್ಯ)define_function(eap, ಖಾಲಿ, &ಸಾಲುಗಳು_ಗಮ್ಯ_free, 0, ಖಾಲಿ, 0, ಖಾಲಿ);
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಸಾಲುಗಳು_ಗಮ್ಯ_free);
}

    ಪೂರ್ಣಾಂಕ
get_func_arity(char_u *name, ಪೂರ್ಣಾಂಕ *required, ಪೂರ್ಣಾಂಕ *optional, ಪೂರ್ಣಾಂಕ *varargs)
{
    ufunc_T	*ufunc = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		argcount = 0;
    ಪೂರ್ಣಾಂಕ		min_argcount = 0;
    ಪೂರ್ಣಾಂಕ		idx;

    idx = ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func(name);
    ಆದರೆ (idx >= 0)
    {
	ಆಂತರಿಕ_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = ಖಾಲಿ;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ಆದರೆ (error == FCERR_NONE)
	    ufunc = ಕಂಡುಹಿಡಿ_func(fname, FALSE);
	vim_free(tofree);

	ಆದರೆ (ufunc == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	argcount = ufunc->uf_ವಾದಗಳು.ga_len;
	min_argcount = ufunc->uf_ವಾದಗಳು.ga_len - ufunc->uf_def_ವಾದಗಳು.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Find a function by name, including "<lambda>123".
 * Check for "profile" and "debug" arguments and set"compile_type".
 * Caller should initialize "compile_type" to CT_NONE.
 * Return NULL if not found.
 */
    ufunc_T *
ಕಂಡುಹಿಡಿ_func_by_name(char_u *name, compiletype_T *compile_type)
{
    char_u	*arg = name;
    char_u	*fname;
    ufunc_T	*ufunc;
    ಪೂರ್ಣಾಂಕ		is_global = FALSE;
    char_u	*bracket_ಆರಂಭ = ಖಾಲಿ;
    char_u	*generic_func_name = name;

    ಆದರೆ (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    ಆದರೆ (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(ಶೂನ್ಯ)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = ಕಂಡುಹಿಡಿ_class_func(&arg);
	ಆದರೆ (ufunc != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, &ufunc);
	ಆದರೆ (ufunc != ಖಾಲಿ)
	{
	    vim_free(fname);
	    ಹಿಂತಿರುಗಿ ufunc;
	}
	ಆದರೆ (fname != ಖಾಲಿ && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_ಆರಂಭ = arg;
	    ಆದರೆ (skip_generic_func_type_ವಾದಗಳು(&arg) == FAIL)
	    {
		vim_free(fname);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
    }
    ಆದರೆ (fname == ಖಾಲಿ)
    {
	semsg(_(e_invalid_ವಾದ_str), name);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (!ತುದಿಗಳು_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ufunc = ಕಂಡುಹಿಡಿ_func(fname, is_global);
    ಆದರೆ (ufunc == ಖಾಲಿ)
    {
	char_u *p = untrans_function_name(fname);

	ಆದರೆ (p != ಖಾಲಿ)
	    // Try again without making it script-local.
	    ufunc = ಕಂಡುಹಿಡಿ_func(p, FALSE);

	ಆದರೆ (ufunc == ಖಾಲಿ)
	{
	    bracket_ಆರಂಭ = generic_func_ಕಂಡುಹಿಡಿ_open_bracket(fname);
	    ಆದರೆ (bracket_ಆರಂಭ != ಖಾಲಿ)
	    {
		generic_func_name = fname;
		*bracket_ಆರಂಭ = NUL;
		ufunc = ಕಂಡುಹಿಡಿ_func(fname, FALSE);
		*bracket_ಆರಂಭ = '<';
	    }
	}
    }

    ಆದರೆ (ufunc != ಖಾಲಿ)
    {
	// handle generic functions
	ಆದರೆ (bracket_ಆರಂಭ == ಖಾಲಿ)
	    bracket_ಆರಂಭ = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_ಆರಂಭ);
    }

    vim_free(fname);

    ಆದರೆ (ufunc == ಖಾಲಿ)
	semsg(_(e_cannot_ಕಂಡುಹಿಡಿ_function_str), name);
    ಹಿಂತಿರುಗಿ ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    ಶೂನ್ಯ
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    ಆದರೆ (func_needs_compiling(ufunc, compile_type))
	(ಶೂನ್ಯ)compile_def_function(ufunc, FALSE, compile_type, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	smsg(_("Function %s%s%s does not need compiling"),
				cl != ಖಾಲಿ ? cl->class_name.string : (char_u *)"",
				cl != ಖಾಲಿ ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
defcompile_funcs_ಒಳ_script(ಶೂನ್ಯ)
{
    ದೀರ್ಘ	todo = (ದೀರ್ಘ)func_hashtab.ht_ಬಳಸಿದ;
    ಪೂರ್ಣಾಂಕ		ಬದಲಾಯಿತು = func_hashtab.ht_ಬದಲಾಯಿತು;
    hashitem_T	*hi;

    ಗಾಗಿ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    ಆದರೆ (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_ಸ್ಥಿತಿ == UF_ಗಮ್ಯ_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(ಶೂನ್ಯ)compile_def_function(ufunc, FALSE, CT_NONE, ಖಾಲಿ);

		ಆದರೆ (func_hashtab.ht_ಬದಲಾಯಿತು != ಬದಲಾಯಿತು)
		{
		    // a function has been added or removed, need to start
		    // over
		    todo = (ದೀರ್ಘ)func_hashtab.ht_ಬಳಸಿದ;
		    ಬದಲಾಯಿತು = func_hashtab.ht_ಬದಲಾಯಿತು;
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
    ಆದರೆ (*eap->arg != NUL)
    {
	typval_T tv;

	ಆದರೆ (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    ಆದರೆ (cl != ಖಾಲಿ)
		defcompile_class(cl);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = ಕಂಡುಹಿಡಿ_func_by_name(eap->arg, &compile_type);
	    ಆದರೆ (ufunc != ಖಾಲಿ)
		defcompile_function(ufunc, ಖಾಲಿ);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	defcompile_funcs_ಒಳ_script();

	// compile all the class defined in the current script
	defcompile_classes_ಒಳ_script();
    }
}

/*
 * Return 5 if "p" starts with "<SID>" or "<SNR>" (ignoring case).
 * Return 2 if "p" starts with "s:".
 * Return 0 otherwise.
 */
    ಪೂರ್ಣಾಂಕ
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    ಆದರೆ (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	ಹಿಂತಿರುಗಿ 5;
    ಆದರೆ (p[0] == 's' && p[1] == ':')
	ಹಿಂತಿರುಗಿ 2;
    ಹಿಂತಿರುಗಿ 0;
}

    ಪೂರ್ಣಾಂಕ
translated_function_exists(char_u *name, ಪೂರ್ಣಾಂಕ is_global)
{
    ಆದರೆ (builtin_function(name, -1))
	ಹಿಂತಿರುಗಿ has_ಆಂತರಿಕ_func(name);
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_func(name, is_global) != ಖಾಲಿ;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    ಪೂರ್ಣಾಂಕ
has_varargs(ufunc_T *ufunc)
{
    ಹಿಂತಿರುಗಿ ufunc->uf_varargs || ufunc->uf_va_name != ಖಾಲಿ;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    ಪೂರ್ಣಾಂಕ
function_exists(char_u *name, ಪೂರ್ಣಾಂಕ no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    n = FALSE;
    ಪೂರ್ಣಾಂಕ	    flag;
    ಪೂರ್ಣಾಂಕ	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    ಆದರೆ (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    ಆದರೆ (p != ಖಾಲಿ && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    ಪೂರ್ಣಾಂಕ
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		is_global = FALSE;
    ufunc_T	*fp = ಖಾಲಿ;
    list_T	*ವಾದಗಳು;
    ಪೂರ್ಣಾಂಕ		is_def;
    ಅಕ್ಷರ	*tofree;

    ಆದರೆ (*name == NUL)
	ಹಿಂತಿರುಗಿ FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    ಆದರೆ (p != ಖಾಲಿ && *skipwhite(nm) == NUL)
	fp = ಕಂಡುಹಿಡಿ_func(p, is_global);
    vim_free(p);
    ಆದರೆ (fp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    is_def = fp->uf_def_ಸ್ಥಿತಿ != UF_NOT_COMPILED;
    dict_ಕೂಡಿಸು_string(d, "name", printable_func_name(fp));
    dict_ಕೂಡಿಸು_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_ಕೂಡಿಸು_bool(d, "available", TRUE);
    ವಾದಗಳು = list_alloc();
    ಆದರೆ (ವಾದಗಳು == ಖಾಲಿ || dict_ಕೂಡಿಸು_list(d, "args", ವಾದಗಳು) == FAIL)
    {
	list_unref(ವಾದಗಳು);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < fp->uf_ವಾದಗಳು.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	ಆದರೆ (arg == ಖಾಲಿ || list_append_dict(ವಾದಗಳು, arg) == FAIL)
	{
	    dict_unref(arg);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	dict_ಕೂಡಿಸು_string(arg, "name", FUNCARG(fp, j));
	ಆದರೆ (fp->uf_arg_types == ಖಾಲಿ)
	    dict_ಕೂಡಿಸು_string(arg, "type", (char_u *)"any");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dict_ಕೂಡಿಸು_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ಆದರೆ (j >= fp->uf_ವಾದಗಳು.ga_len - fp->uf_def_ವಾದಗಳು.ga_len)
	    dict_ಕೂಡಿಸು_string(arg, "default", ((char_u **)fp->uf_def_ವಾದಗಳು.ga_ದತ್ತಾಂಶ)
			     [j - fp->uf_ವಾದಗಳು.ga_len + fp->uf_def_ವಾದಗಳು.ga_len]);
    }
    ಆದರೆ (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	ಆದರೆ (va == ಖಾಲಿ || dict_ಕೂಡಿಸು_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	dict_ಕೂಡಿಸು_string(va, "name",
			 fp->uf_va_name == ಖಾಲಿ ? (char_u *)"" : fp->uf_va_name);
	ಆದರೆ (fp->uf_va_type == ಖಾಲಿ)
	    dict_ಕೂಡಿಸು_string(va, "type", (char_u *)"list<any>");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dict_ಕೂಡಿಸು_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    ಆದರೆ (is_def)
    {
	dict_ಕೂಡಿಸು_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	dict_ಕೂಡಿಸು_string(d, "returns", (char_u *)"any");
    dict_ಕೂಡಿಸು_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_ಕೂಡಿಸು_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_ಕೂಡಿಸು_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_ಕೂಡಿಸು_bool(d, "closure", (fp->uf_flags & FC_ಆವರಣ) != 0);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "lnum", fp->uf_script_ctx.sc_lnum);
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, ಪೂರ್ಣಾಂಕ check)
{
    char_u	*nm = name;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    ಆದರೆ (p != ಖಾಲಿ && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	ಹಿಂತಿರುಗಿ p;

    vim_free(p);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, ಪೂರ್ಣಾಂಕ idx)
{
    ಸ್ಥಿರ long_u	ಮುಗಿದ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		ಬದಲಾಯಿತು;
    ಸ್ಥಿರ hashitem_T	*hi;
    ufunc_T		*fp;

    ಆದರೆ (idx == 0)
    {
	ಮುಗಿದ = 0;
	hi = func_hashtab.ht_array;
	ಬದಲಾಯಿತು = func_hashtab.ht_ಬದಲಾಯಿತು;
    }
    ಆದರೆ (ಬದಲಾಯಿತು == func_hashtab.ht_ಬದಲಾಯಿತು && ಮುಗಿದ < func_hashtab.ht_ಬಳಸಿದ)
    {
	ಪೂರ್ಣಾಂಕ len;

	ಆದರೆ (ಮುಗಿದ++ > 0)
	    ++hi;
	ಆಗ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	ಆದರೆ ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    ಹಿಂತಿರುಗಿ (char_u *)"";

	ಆದರೆ (fp->uf_namelen + 4 >= IOSIZE)
	    ಹಿಂತಿರುಗಿ fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	ಆದರೆ (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    ಆದರೆ (!has_varargs(fp) && fp->uf_ವಾದಗಳು.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	ಹಿಂತಿರುಗಿ IObuff;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, ಪೂರ್ಣಾಂಕ extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    ಆದರೆ (ufunc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_ಸ್ಥಿತಿ = UF_ಗಮ್ಯ_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_ಎಣಿಕೆ = ಖಾಲಿ;
    ufunc->uf_tml_ಒಟ್ಟು = ಖಾಲಿ;
    ufunc->uf_tml_self = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ga_copy_ಅಕ್ಷರಸರಣಿಗಳು(&fp->uf_ವಾದಗಳು, &ufunc->uf_ವಾದಗಳು);
    ga_copy_ಅಕ್ಷರಸರಣಿಗಳು(&fp->uf_def_ವಾದಗಳು, &ufunc->uf_def_ವಾದಗಳು);

    ಆದರೆ (ufunc->uf_arg_types != ಖಾಲಿ)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_ವಾದಗಳು.ga_len);
	ಆದರೆ (at != ಖಾಲಿ)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     ಗಾತ್ರ(type_T *) * ufunc->uf_ವಾದಗಳು.ga_len);
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

    ಆದರೆ (ufunc->uf_va_name != ಖಾಲಿ)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_ಖಂಡ_ಆಳ = 0;
    ufunc->uf_ಖಂಡ_ids = ಖಾಲಿ;

    ga_copy_ಅಕ್ಷರಸರಣಿಗಳು(&fp->uf_ಸಾಲುಗಳು, &ufunc->uf_ಸಾಲುಗಳು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    ಆದರೆ (fp->uf_profiling)
	func_do_profile(ufunc);
#ಆದರೆ_ಅಂತ್ಯ

    ufunc->uf_refcount = 1;

    ಹಿಂತಿರುಗಿ ufunc;
}

/*
 * ":delfunction {name}"
 */
    ಶೂನ್ಯ
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = ಖಾಲಿ;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    ಪೂರ್ಣಾಂಕ		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    vim_free(fudi.fd_newkey);
    ಆದರೆ (name == ಖಾಲಿ)
    {
	ಆದರೆ (fudi.fd_dict != ಖಾಲಿ && !eap->skip)
	    emsg(_(e_funcref_required));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!ತುದಿಗಳು_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	ಹಿಂತಿರುಗಿ;
    }
    ಗಣ_nextcmd(eap, p);
    ಆದರೆ (eap->nextcmd != ಖಾಲಿ)
	*p = NUL;

    ಆದರೆ (numbered_function(name) && fudi.fd_dict == ಖಾಲಿ)
    {
	ಆದರೆ (!eap->skip)
	    semsg(_(e_invalid_ವಾದ_str), eap->arg);
	vim_free(name);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!eap->skip)
	fp = ಕಂಡುಹಿಡಿ_func(name, is_global);
    vim_free(name);

    ಆದರೆ (!eap->skip)
    {
	ಆದರೆ (fp == ಖಾಲಿ)
	{
	    ಆದರೆ (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_ಒಳ_use), eap->arg);
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (fudi.fd_dict != ಖಾಲಿ)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    ಆದರೆ (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		ಆದರೆ (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
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
    ufunc_T *fp = ಖಾಲಿ;

    ಆದರೆ (name == ಖಾಲಿ || !func_name_refcount(name))
	ಹಿಂತಿರುಗಿ;
    fp = ಕಂಡುಹಿಡಿ_func(name, FALSE);
    ಆದರೆ (fp == ಖಾಲಿ && numbered_function(name))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ EXITFREE
	ಆದರೆ (!entered_free_all_mem)
#ಆದರೆ_ಅಂತ್ಯ
	    ಆಂತರಿಕ_error("func_unref()");
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
    ಆದರೆ (fp != ಖಾಲಿ && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != ಖಾಲಿ
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	ಆದರೆ (fp->uf_calls == 0)
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

    ಆದರೆ (name == ಖಾಲಿ || !func_name_refcount(name))
	ಹಿಂತಿರುಗಿ;
    fp = ಕಂಡುಹಿಡಿ_func(name, FALSE);
    ಆದರೆ (fp != ಖಾಲಿ)
	++fp->uf_refcount;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	ಆಂತರಿಕ_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    ಶೂನ್ಯ
func_ptr_ref(ufunc_T *fp)
{
    ಆದರೆ (fp != ಖಾಲಿ)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
can_free_funccal(funccall_T *fc, ಪೂರ್ಣಾಂಕ copyID)
{
    ಹಿಂತಿರುಗಿ (fc->fc_l_varlist.lv_copyID != copyID
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
    ಪೂರ್ಣಾಂಕ		returning = FALSE;
    evalarg_T	evalarg;

    ಆದರೆ (current_funccal == ಖಾಲಿ)
    {
	emsg(_(e_return_not_inside_function));
	ಹಿಂತಿರುಗಿ;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    ಆದರೆ (eap->skip)
	++emsg_skip;

    eap->nextcmd = ಖಾಲಿ;
    ಆದರೆ ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	ಆದರೆ (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	ಇಲ್ಲದಿದ್ದರೆ
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	ಆದರೆ (!aborting())
	    returning = do_return(eap, FALSE, TRUE, ಖಾಲಿ);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    ಆದರೆ (returning)
	eap->nextcmd = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->nextcmd == ಖಾಲಿ)	    // no argument
	ಗಣ_nextcmd(eap, arg);

    ಆದರೆ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		doesrange;
    typval_T	rettv;
    ಪೂರ್ಣಾಂಕ		failed = FALSE;

    lnum = eap->line1;
    ಗಾಗಿ ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
	{
	    ಆದರೆ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		ನಿಲ್ಲಿಸು;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	ಆದರೆ (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	ಆದರೆ (handle_subscript(arg, ಖಾಲಿ, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

	clear_tv(&rettv);
	ಆದರೆ (doesrange)
	    ನಿಲ್ಲಿಸು;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	ಆದರೆ (aborting())
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ವಾದಗಳು + 1];	// vars for arguments
    ಪೂರ್ಣಾಂಕ		partial_argc = 0;		// number of partial arguments
    ಪೂರ್ಣಾಂಕ		argcount = 0;			// number of arguments found
    ಪೂರ್ಣಾಂಕ		r;

    ಆದರೆ (current_funccal == ಖಾಲಿ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (partial != ಖಾಲಿ)
    {
	ಆದರೆ (partial->pt_dict != ಖಾಲಿ)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (partial->pt_argc > 0)
	{
	    ಪೂರ್ಣಾಂಕ i;

	    partial_argc = partial->pt_argc;
	    ಗಾಗಿ (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    ಪೂರ್ಣಾಂಕ is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    ಆದರೆ (r == OK)
    {
	ಆದರೆ (type != ಖಾಲಿ)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_ವಾದ_types(type, argvars, argcount, ಖಾಲಿ, name);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_builtin)
	{
	    ಪೂರ್ಣಾಂಕ idx = ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func(name);

	    ಆದರೆ (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_ಆಂತರಿಕ_func(idx, argcount) == -1)
		r = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ufunc_T *ufunc = ಕಂಡುಹಿಡಿ_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    ಆದರೆ (ufunc != ಖಾಲಿ)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		ಆದರೆ (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    ಆದರೆ (r == FAIL)
    {
	ಆಗ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    ಪೂರ್ಣಾಂಕ
can_ಕೂಡಿಸು_defer(ಶೂನ್ಯ)
{
    ಆದರೆ (!ಒಳ_def_function() && get_current_funccal() == ಖಾಲಿ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_defer(char_u *name, ಪೂರ್ಣಾಂಕ argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    ಪೂರ್ಣಾಂಕ		argcount = argcount_arg;
    defer_T	*dr;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;

    ಆದರೆ (saved_name == ಖಾಲಿ)
	ಹೋಗು theend;
    ಆದರೆ (ಒಳ_def_function())
    {
	ಆದರೆ (ಕೂಡಿಸು_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, ಗಾತ್ರ(defer_T), 10);
	ಆದರೆ (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    ಹೋಗು theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_ದತ್ತಾಂಶ)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	ಆಗ (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    ಆಗ (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_defer_one(funccall_T *funccal)
{
    ಪೂರ್ಣಾಂಕ	    idx;

    ಗಾಗಿ (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_ದತ್ತಾಂಶ) + idx;

	ಆದರೆ (dr->dr_name == ಖಾಲಿ)
	    // already being called, can happen if function does ":qa"
	    ಮುಂದುವರಿ;

	funcexe_T   funcexe;
	CLEAR_ಕ್ಷೇತ್ರ(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = ಖಾಲಿ;

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
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_funccall_defer(funccall_T *fc)
{
    ಆದರೆ (fc->fc_ectx != ಖಾಲಿ)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಗಾಗಿ (funccall_T *fc = current_funccal; fc != ಖಾಲಿ; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    ಗಾಗಿ (funccal_ನಮೂದು_T *fce = funccal_ರಾಶಿ; fce != ಖಾಲಿ; fce = fce->ಮುಂದಿನ)
	ಗಾಗಿ (funccall_T *fc = fce->ತುದಿ_funccal; fc != ಖಾಲಿ; fc = fc->fc_caller)
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
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = ಖಾಲಿ;
    partial_T	*partial = ಖಾಲಿ;
    evalarg_T	evalarg;
    type_T	*type = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_var = FALSE;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    ಆದರೆ (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	ಆದರೆ (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	ಹಿಂತಿರುಗಿ;
    }

    tofree = trans_function_name_ext(&arg, ಖಾಲಿ, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : ಖಾಲಿ, &ufunc);
    ಆದರೆ (fudi.fd_newkey != ಖಾಲಿ)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    ಆದರೆ (tofree == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    ಆದರೆ (fudi.fd_dict != ಖಾಲಿ)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != ಖಾಲಿ ? ಖಾಲಿ : &partial,
				vim9script && type == ಖಾಲಿ ? &type : ಖಾಲಿ,
						     FALSE, FALSE, &ಸಿಕ್ಕಿತು_var);

    ಆದರೆ (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_ವಾದಗಳು(name, STRLEN(name), &arg);
	ಆದರೆ (name == ಖಾಲಿ)
	    ಹೋಗು ಕೊನೆ;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    ಆದರೆ (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	ಹೋಗು ಕೊನೆ;
    }
    ಆದರೆ (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	ಹೋಗು ಕೊನೆ;
    }

    ಆದರೆ (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	funcexe_T funcexe;

	CLEAR_ಕ್ಷೇತ್ರ(funcexe);
	funcexe.fe_check_type = type;
	funcexe.fe_ufunc = ufunc;
	funcexe.fe_partial = partial;
	funcexe.fe_selfdict = fudi.fd_dict;
	funcexe.fe_firstline = eap->line1;
	funcexe.fe_lastline = eap->line2;
	funcexe.fe_ಸಿಕ್ಕಿತು_var = ಸಿಕ್ಕಿತು_var;
	funcexe.fe_evaluate = TRUE;
	failed = ex_call_inner(eap, name, &arg, startarg, &funcexe, &evalarg);
    }

    // When inside :try we need to check for following "| catch" or "| endtry".
    // Not when there was an error, but do check if an exception was thrown.
    ಆದರೆ ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	ಆದರೆ (!ತುದಿಗಳು_excmd2(eap->arg, arg))
	{
	    ಆದರೆ (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಣ_nextcmd(eap, arg);
    }
    // Must be after using "arg", it may point into memory cleared here.
    clear_evalarg(&evalarg, eap);

ಕೊನೆ:
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
    ಪೂರ್ಣಾಂಕ
do_return(
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		reanimate,
    ಪೂರ್ಣಾಂಕ		is_cmd,
    ಶೂನ್ಯ	*rettv)
{
    ಪೂರ್ಣಾಂಕ		idx;
    cstack_T	*cstack = eap->cstack;

    ಆದರೆ (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ಆದರೆ (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	ಆದರೆ (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    ಆದರೆ (reanimate)
		rettv = current_funccal->fc_rettv;

	    ಆದರೆ (rettv != ಖಾಲಿ)
	    {
		// Store the value of the pending return.
		ಆದರೆ ((cstack->cs_rettv[idx] = alloc_tv()) != ಖಾಲಿ)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		ಇಲ್ಲದಿದ್ದರೆ
		    emsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		cstack->cs_rettv[idx] = ಖಾಲಿ;

	    ಆದರೆ (reanimate)
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
    ಇಲ್ಲದಿದ್ದರೆ
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	ಆದರೆ (!reanimate && rettv != ಖಾಲಿ)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    ಆದರೆ (!is_cmd)
		vim_free(rettv);
	}
    }

    ಹಿಂತಿರುಗಿ idx < 0;
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
    char_u	*s = ಖಾಲಿ;
    char_u	*tofree = ಖಾಲಿ;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    ಆದರೆ (rettv != ಖಾಲಿ)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    ಆದರೆ (s == ಖಾಲಿ)
	s = (char_u *)"";
    ಇಲ್ಲದಿದ್ದರೆ
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    ಆದರೆ (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    ಹಿಂತಿರುಗಿ vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_ಸಾಲು(
    ಪೂರ್ಣಾಂಕ	    c ಬಳಸದ,
    ಶೂನ್ಯ    *cookie,
    ಪೂರ್ಣಾಂಕ	    indent ಬಳಸದ,
    getline_opt_T options ಬಳಸದ)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    ಆದರೆ (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
	func_ಸಾಲು_ಕೊನೆ(cookie);
#ಆದರೆ_ಅಂತ್ಯ

    gap = &fp->uf_ಸಾಲುಗಳು;
    ಆದರೆ (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_ಒಳ_try())
	    || fcp->fc_returned)
	retval = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Skip NULL lines (continuation lines).
	ಆಗ (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_ದತ್ತಾಂಶ))[fcp->fc_linenr] == ಖಾಲಿ)
	    ++fcp->fc_linenr;
	ಆದರೆ (fcp->fc_linenr >= gap->ga_len)
	    retval = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    retval = vim_strsave(((char_u **)(gap->ga_ದತ್ತಾಂಶ))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	    ಆದರೆ (do_profiling == PROF_YES)
		func_ಸಾಲು_ಆರಂಭ(cookie, SOURCING_LNUM);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

    // Did we encounter a breakpoint?
    ಆದರೆ (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    ಪೂರ್ಣಾಂಕ
func_has_ended(ಶೂನ್ಯ *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    ಹಿಂತಿರುಗಿ (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_ಒಳ_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    ಪೂರ್ಣಾಂಕ
func_has_abort(
    ಶೂನ್ಯ    *cookie)
{
    ಹಿಂತಿರುಗಿ ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
}


/*
 * Turn "dict.Func" into a partial for "Func" bound to "dict".
 * Don't do this when "Func" is already a partial that was bound
 * explicitly (pt_auto is FALSE).
 * Changes "rettv" in-place.
 * Returns the updated "selfdict_in".
 */
    dict_T *
make_partial(dict_T *selfdict_ಒಳ, typval_T *rettv)
{
    char_u	*fname;
    ufunc_T	*fp = ಖಾಲಿ;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_ಒಳ;

    ಆದರೆ (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != ಖಾಲಿ
				     && rettv->vval.v_partial->pt_func != ಖಾಲಿ)
	fp = rettv->vval.v_partial->pt_func;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == ಖಾಲಿ ? ಖಾಲಿ
					      : rettv->vval.v_partial->pt_name;
	ಆದರೆ (fname == ಖಾಲಿ)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u	*tofree = ಖಾಲಿ;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = ಕಂಡುಹಿಡಿ_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    ಆದರೆ (fp != ಖಾಲಿ && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	ಆದರೆ (pt != ಖಾಲಿ)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = ಖಾಲಿ;
	    ಆದರೆ (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		ಪೂರ್ಣಾಂಕ		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		ಆದರೆ (ret_pt->pt_name != ಖಾಲಿ)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		ಆದರೆ (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    ಆದರೆ (pt->pt_argv == ಖಾಲಿ)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			pt->pt_argc = ret_pt->pt_argc;
			ಗಾಗಿ (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    ಹಿಂತಿರುಗಿ selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(ಶೂನ್ಯ *cookie)
{
    ಹಿಂತಿರುಗಿ ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(ಶೂನ್ಯ *cookie)
{
    ಹಿಂತಿರುಗಿ &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    ಪೂರ್ಣಾಂಕ *
func_dbg_tick(ಶೂನ್ಯ *cookie)
{
    ಹಿಂತಿರುಗಿ &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    ಪೂರ್ಣಾಂಕ
func_level(ಶೂನ್ಯ *cookie)
{
    ಹಿಂತಿರುಗಿ ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    ಪೂರ್ಣಾಂಕ
current_func_returned(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ current_funccal->fc_returned;
}

    ಪೂರ್ಣಾಂಕ
free_unref_funccal(ಪೂರ್ಣಾಂಕ copyID, ಪೂರ್ಣಾಂಕ testing)
{
    ಪೂರ್ಣಾಂಕ		did_free = FALSE;
    ಪೂರ್ಣಾಂಕ		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    ಗಾಗಿ (pfc = &ಹಿಂದಿನ_funccal; *pfc != ಖಾಲಿ; )
    {
	ಆದರೆ (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    pfc = &(*pfc)->fc_caller;
    }
    ಆದರೆ (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(ಶೂನ್ಯ)garbage_collect(testing);

    ಹಿಂತಿರುಗಿ did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    ಸ್ಥಿರ funccall_T *
get_funccal(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    funccall_T	*funccal;
    funccall_T	*ತಾತ್ಕಾಲಿಕ_funccal;

    funccal = current_funccal;
    ಆದರೆ (debug_backtrace_level > 0)
    {
	ಗಾಗಿ (i = 0; i < debug_backtrace_level; i++)
	{
	    ತಾತ್ಕಾಲಿಕ_funccal = funccal->fc_caller;
	    ಆದರೆ (ತಾತ್ಕಾಲಿಕ_funccal)
		funccal = ತಾತ್ಕಾಲಿಕ_funccal;
	    ಇಲ್ಲದಿದ್ದರೆ
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    ಹಿಂತಿರುಗಿ funccal;
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
    funccall_T	*funccal = ಖಾಲಿ;

    ಆದರೆ (current_funccal == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    funccal = get_funccal();
    ಆದರೆ (funccal == ಖಾಲಿ || funccal->fc_l_vars.dv_refcount == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(ಶೂನ್ಯ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ಹಿಂತಿರುಗಿ funccal == ಖಾಲಿ ? ಖಾಲಿ : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(ಶೂನ್ಯ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ಹಿಂತಿರುಗಿ funccal == ಖಾಲಿ ? ಖಾಲಿ : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_ವಾದಗಳು_ht(ಶೂನ್ಯ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ಹಿಂತಿರುಗಿ funccal == ಖಾಲಿ ? ಖಾಲಿ : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_ವಾದಗಳು_var(ಶೂನ್ಯ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ಹಿಂತಿರುಗಿ funccal == ಖಾಲಿ ? ಖಾಲಿ : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    ಶೂನ್ಯ
list_func_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ)
{
    ಆದರೆ (current_funccal != ಖಾಲಿ && current_funccal->fc_l_vars.dv_refcount > 0)
	list_hashtable_vars(&current_funccal->fc_l_vars.dv_hashtab,
							   "l:", FALSE, ಮೊದಲ);
}

/*
 * If "ht" is the hashtable for local variables in the current funccal, return
 * the dict that contains it.
 * Otherwise return NULL.
 */
    dict_T *
get_current_funccal_dict(hashtab_T *ht)
{
    ಆದರೆ (current_funccal != ಖಾಲಿ
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	ಹಿಂತಿರುಗಿ &current_funccal->fc_l_vars;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
ಕಂಡುಹಿಡಿ_hi_ಒಳ_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = ಖಾಲಿ;
    char_u	*varname;

    ಆದರೆ (current_funccal == ಖಾಲಿ || current_funccal->fc_func->uf_scoped == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    ಆಗ (current_funccal != ಖಾಲಿ)
    {
	ht = ಕಂಡುಹಿಡಿ_var_ht(name, &varname);
	ಆದರೆ (ht != ಖಾಲಿ && *varname != NUL)
	{
	    hi = hash_ಕಂಡುಹಿಡಿ(ht, varname);
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    {
		*pht = ht;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (current_funccal == current_funccal->fc_func->uf_scoped)
	    ನಿಲ್ಲಿಸು;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ಹಿಂತಿರುಗಿ hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
ಕಂಡುಹಿಡಿ_var_ಒಳ_scoped_ht(char_u *name, ಪೂರ್ಣಾಂಕ no_autoload)
{
    dictitem_T	*v = ಖಾಲಿ;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    ಆದರೆ (current_funccal == ಖಾಲಿ || current_funccal->fc_func->uf_scoped == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    ಆಗ (current_funccal)
    {
	ht = ಕಂಡುಹಿಡಿ_var_ht(name, &varname);
	ಆದರೆ (ht != ಖಾಲಿ && *varname != NUL)
	{
	    v = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, *name, varname, no_autoload);
	    ಆದರೆ (v != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (current_funccal == current_funccal->fc_func->uf_scoped)
	    ನಿಲ್ಲಿಸು;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ಹಿಂತಿರುಗಿ v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಹಿಂದಿನ_funccal(ಪೂರ್ಣಾಂಕ copyID)
{
    funccall_T	*fc;

    ಗಾಗಿ (fc = ಹಿಂದಿನ_funccal; fc != ಖಾಲಿ; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	ಆದರೆ (ಗಣ_ref_ಒಳ_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, ಖಾಲಿ, ಖಾಲಿ)
		|| ಗಣ_ref_ಒಳ_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, ಖಾಲಿ, ಖಾಲಿ)
		|| ಗಣ_ref_ಒಳ_list_ವಸ್ತುಗಳು(&fc->fc_l_varlist, copyID + 1, ಖಾಲಿ, ಖಾಲಿ))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_funccal(funccall_T *fc, ಪೂರ್ಣಾಂಕ copyID)
{
    ಆದರೆ (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	ಆದರೆ (ಗಣ_ref_ಒಳ_ht(&fc->fc_l_vars.dv_hashtab, copyID, ಖಾಲಿ, ಖಾಲಿ)
		|| ಗಣ_ref_ಒಳ_ht(&fc->fc_l_avars.dv_hashtab, copyID, ಖಾಲಿ, ಖಾಲಿ)
		|| ಗಣ_ref_ಒಳ_list_ವಸ್ತುಗಳು(&fc->fc_l_varlist, copyID, ಖಾಲಿ, ಖಾಲಿ)
		|| ಗಣ_ref_ಒಳ_func(ಖಾಲಿ, fc->fc_func, copyID))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_call_ರಾಶಿ(ಪೂರ್ಣಾಂಕ copyID)
{
    funccall_T		*fc;
    funccal_ನಮೂದು_T	*ನಮೂದು;

    ಗಾಗಿ (fc = current_funccal; fc != ಖಾಲಿ; fc = fc->fc_caller)
	ಆದರೆ (ಗಣ_ref_ಒಳ_funccal(fc, copyID))
	    ಹಿಂತಿರುಗಿ TRUE;

    // Also go through the funccal_stack.
    ಗಾಗಿ (ನಮೂದು = funccal_ರಾಶಿ; ನಮೂದು != ಖಾಲಿ; ನಮೂದು = ನಮೂದು->ಮುಂದಿನ)
	ಗಾಗಿ (fc = ನಮೂದು->ತುದಿ_funccal; fc != ಖಾಲಿ; fc = fc->fc_caller)
	    ಆದರೆ (ಗಣ_ref_ಒಳ_funccal(fc, copyID))
		ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_functions(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		todo;
    hashitem_T	*hi = ಖಾಲಿ;
    ufunc_T	*fp;

    todo = (ಪೂರ್ಣಾಂಕ)func_hashtab.ht_ಬಳಸಿದ;
    ಗಾಗಿ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ಆದರೆ (!func_name_refcount(fp->uf_name)
					  && ಗಣ_ref_ಒಳ_func(ಖಾಲಿ, fp, copyID))
		ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_func_ವಾದಗಳು(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < funcargs.ga_len; ++i)
	ಆದರೆ (ಗಣ_ref_ಒಳ_ಅಂಶ(((typval_T **)funcargs.ga_ದತ್ತಾಂಶ)[i],
						copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ))
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_func(char_u *name, ufunc_T *fp_ಒಳ, ಪೂರ್ಣಾಂಕ copyID)
{
    ufunc_T	*fp = fp_ಒಳ;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ಖಾಲಿ;
    char_u	*fname;
    ಪೂರ್ಣಾಂಕ		abort = FALSE;

    ಆದರೆ (name == ಖಾಲಿ && fp_ಒಳ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (fp_ಒಳ == ಖಾಲಿ)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = ಕಂಡುಹಿಡಿ_func(fname, FALSE);
    }
    ಆದರೆ (fp != ಖಾಲಿ)
    {
	ಗಾಗಿ (fc = fp->uf_scoped; fc != ಖಾಲಿ; fc = fc->fc_func->uf_scoped)
	    abort = abort || ಗಣ_ref_ಒಳ_funccal(fc, copyID);
    }

    vim_free(tofree);
    ಹಿಂತಿರುಗಿ abort;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
