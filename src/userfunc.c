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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
స్థిర hashtab_T	func_hashtab;

// Used by get_func_tv()
స్థిర garray_T funcargs = GA_ఖాళీలు;

// pointer to funccal for currently active function
స్థిర funccall_T *current_funccal = ఖాళీ;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
స్థిర funccall_T *మునుపటి_funccal = ఖాళీ;

స్థిర శూన్య funccal_unref(funccall_T *fc, ufunc_T *fp, పూర్ణాంకం force);
స్థిర శూన్య func_clear(ufunc_T *fp, పూర్ణాంకం force);
స్థిర పూర్ణాంకం func_free(ufunc_T *fp, పూర్ణాంకం force);
స్థిర char_u *untrans_function_name(char_u *name);
స్థిర శూన్య handle_defer_one(funccall_T *funccal);
స్థిర char_u *trans_function_name_ext(char_u **pp, పూర్ణాంకం *is_global, పూర్ణాంకం skip, పూర్ణాంకం flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    శూన్య
func_init(శూన్య)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(శూన్య)
{
    తిరిగి &func_hashtab;
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
    స్థిర char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	పూర్ణాంకం	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	పూర్ణాంకం	    is_vararg,
	పూర్ణాంకం	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = ఖాళీ;
    పూర్ణాంకం		is_underscore = FALSE;

    అయితే (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    ఒకవేళ (arg == p || సురక్షిత_isdigit(*arg)
	    || (argtypes == ఖాళీ
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	ఒకవేళ (!skip)
	    semsg(_(e_illegal_వాదం_str), arg);
	తిరిగి arg;
    }

    // Extra checks in Vim9 script.
    ఒకవేళ (!skip && argtypes != ఖాళీ)
    {
	పూర్ణాంకం c = *p;
	*p = NUL;
	పూర్ణాంకం r = check_reserved_name(arg, FALSE);
	*p = c;
	ఒకవేళ (r == FAIL)
	    తిరిగి arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	ఒకవేళ (check_defined(arg, p - arg,
				   evalarg == ఖాళీ ? ఖాళీ : evalarg->eval_cctx,
			       eap == ఖాళీ ? ఖాళీ : eap->cstack, TRUE) == FAIL)
	    తిరిగి arg;
    }

    ఒకవేళ (newargs != ఖాళీ && ga_grow(newargs, 1) == FAIL)
	తిరిగి arg;
    ఒకవేళ (newargs != ఖాళీ)
    {
	పూర్ణాంకం	c;
	పూర్ణాంకం	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	ఒకవేళ (arg_copy == ఖాళీ)
	{
	    *p = c;
	    తిరిగి arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	ఒకవేళ (argtypes == ఖాళీ || !is_underscore)
	    // Check for duplicate argument name.
	    కొరకు (i = 0; i < newargs->ga_len; ++i)
		ఒకవేళ (STRCMP(((char_u **)(newargs->ga_దత్తాంశం))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_వాదం_name_str), arg_copy);
		    vim_free(arg_copy);
		    తిరిగి arg;
		}
	((char_u **)(newargs->ga_దత్తాంశం))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    ఒకవేళ (argtypes != ఖాళీ && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != ఖాళీ && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = ఖాళీ;

	ఒకవేళ (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == ఖాళీ ? arg : arg_copy);
	    p = skipwhite(p);
	}
	ఒకవేళ (*p == ':')
	{
	    ++p;
	    ఒకవేళ (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		తిరిగి arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    ఒకవేళ (!skip)
		type = vim_strnsave(type, p - type);
	}
	లేకపోతే ఒకవేళ (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_వాదం_type_for_str),
					    arg_copy == ఖాళీ ? arg : arg_copy);
	    తిరిగి arg;
	}
	ఒకవేళ (!skip)
	{
	    ఒకవేళ (type == ఖాళీ && types_optional)
	    {
		// lambda arguments default to "any" type
		ఒకవేళ (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		లేకపోతే
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_దత్తాంశం)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_దత్తాంశం)[arg_objm->ga_len++] = FALSE;
	}
    }

    తిరిగి p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    స్థిర char_u *
get_function_పంక్తి(
	exarg_T		*eap,
	garray_T	*పంక్తులు_గమ్యం_free,
	పూర్ణాంకం		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    ఒకవేళ (eap->ea_getline == ఖాళీ)
	theline = getcmdline(':', 0L, indent, 0);
    లేకపోతే
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    ఒకవేళ (theline != ఖాళీ)
    {
	ఒకవేళ (పంక్తులు_గమ్యం_free->ga_len > 0
		&& eap->cmdlinep != ఖాళీ
		&& *eap->cmdlinep == ((char_u **)పంక్తులు_గమ్యం_free->ga_దత్తాంశం)
						   [పంక్తులు_గమ్యం_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(శూన్య)ga_కూడు_string(పంక్తులు_గమ్యం_free, theline);
    }

    తిరిగి theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    స్థిర పూర్ణాంకం
get_function_వాదనలు(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    పూర్ణాంకం		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    పూర్ణాంకం		*varargs,
    garray_T	*default_వాదనలు,
    పూర్ణాంకం		skip,
    exarg_T	*eap,		// can be NULL
    పూర్ణాంకం		లోపల_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*పంక్తులు_గమ్యం_free)
{
    పూర్ణాంకం		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    పూర్ణాంకం		c;
    పూర్ణాంకం		any_default = FALSE;
    char_u	*whitep = *argp;
    పూర్ణాంకం		need_సమాసం = FALSE;

    ఒకవేళ (newargs != ఖాళీ)
	ga_init2(newargs, పరిమాణం(char_u *), 3);
    ఒకవేళ (argtypes != ఖాళీ)
	ga_init2(argtypes, పరిమాణం(char_u *), 3);
    ఒకవేళ (arg_objm != ఖాళీ)
	ga_init2(arg_objm, పరిమాణం(int8_T), 3);
    ఒకవేళ (!skip && default_వాదనలు != ఖాళీ)
	ga_init2(default_వాదనలు, పరిమాణం(char_u *), 3);

    ఒకవేళ (varargs != ఖాళీ)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    అయితే (*p != endchar)
    {
	అయితే (eap != ఖాళీ && eap->ea_getline != ఖాళీ
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_పంక్తి(eap, పంక్తులు_గమ్యం_free, 0,
							  GETLINE_CONCAT_CONT);

	    ఒకవేళ (theline == ఖాళీ)
		ఆపు;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	ఒకవేళ (mustend && *p != endchar)
	{
	    ఒకవేళ (!skip)
		semsg(_(e_invalid_వాదం_str), *argp);
	    వెళ్ళు err_ret;
	}
	ఒకవేళ (*p == endchar && !need_సమాసం)
	    ఆపు;

	ఒకవేళ (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    ఒకవేళ (varargs != ఖాళీ)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    ఒకవేళ (argtypes != ఖాళీ)
	    {
		// ...name: list<type>
		ఒకవేళ (!eval_isnamec1(*p))
		{
		    ఒకవేళ (!skip)
			emsg(_(e_missing_name_after_dots));
		    వెళ్ళు err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		ఒకవేళ (p == arg)
		    ఆపు;
		ఒకవేళ (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    ఆపు;
		}
	    }
	}
	లేకపోతే ఒకవేళ (లోపల_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    అయితే (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    ఒకవేళ (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_పద్ధతి_str), arg);
		*argend = c;
		ఆపు;
	    }

	    ఒకవేళ (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		ఒకవేళ (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_విలువ_must_be_vnone_str), p);
		    వెళ్ళు err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		ఒకవేళ (ga_grow(default_వాదనలు, 1) == FAIL)
		    వెళ్ళు err_ret;

		char_u *సమాసం = vim_strnsave((char_u *)"v:none", 6);
		ఒకవేళ (సమాసం == ఖాళీ)
		    వెళ్ళు err_ret;
		((char_u **)(default_వాదనలు->ga_దత్తాంశం))
						 [default_వాదనలు->ga_len] = సమాసం;
		default_వాదనలు->ga_len++;
	    }
	    లేకపోతే ఒకవేళ (any_default)
	    {
		emsg(_(e_non_default_వాదం_follows_default_వాదం));
		వెళ్ళు err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    ఒకవేళ (newargs != ఖాళీ && ga_grow(newargs, 1) == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (newargs != ఖాళీ)
	    {
		((char_u **)(newargs->ga_దత్తాంశం))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		ఒకవేళ (argtypes != ఖాళీ && ga_grow(argtypes, 1) == OK
			    && arg_objm != ఖాళీ && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_దత్తాంశం)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_దత్తాంశం)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    ఒకవేళ (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			పూర్ణాంకం len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			ఒకవేళ (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			ఒకవేళ (assignment != ఖాళీ)
			{
			    c = *argend;
			    *argend = NUL;
			    ఒకవేళ (any_default)
				vim_snprintf((అక్షరం *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_వాదనలు->ga_len - 1, arg, arg);
			    లేకపోతే
				vim_snprintf((అక్షరం *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_దత్తాంశం))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    ఒకవేళ (*p == ',')
		++p;
	}
	లేకపోతే
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    ఒకవేళ (p == arg)
		ఆపు;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    ఒకవేళ (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_వాదనలు != ఖాళీ)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *సమాసం = p;
		ఒకవేళ (eval1(&p, &rettv, ఖాళీ) != FAIL)
		{
		    ఒకవేళ (!skip)
		    {
			ఒకవేళ (ga_grow(default_వాదనలు, 1) == FAIL)
			    వెళ్ళు err_ret;

			ఒకవేళ (need_సమాసం)
			    need_సమాసం = FALSE;
			// trim trailing whitespace
			అయితే (p > సమాసం && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			సమాసం = vim_strsave(సమాసం);
			ఒకవేళ (సమాసం == ఖాళీ)
			{
			    *p = c;
			    వెళ్ళు err_ret;
			}
			((char_u **)(default_వాదనలు->ga_దత్తాంశం))
						 [default_వాదనలు->ga_len] = సమాసం;
			default_వాదనలు->ga_len++;
			*p = c;
		    }
		}
		లేకపోతే
		{
		    mustend = TRUE;
		    ఒకవేళ (*skipwhite(p) == NUL)
			need_సమాసం = TRUE;
		}
	    }
	    లేకపోతే ఒకవేళ (any_default)
	    {
		emsg(_(e_non_default_వాదం_follows_default_వాదం));
		వెళ్ళు err_ret;
	    }

	    ఒకవేళ (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		ఒకవేళ (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    వెళ్ళు err_ret;
		}
		p = skipwhite(p);
	    }
	    ఒకవేళ (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		ఒకవేళ (!skip && argtypes != ఖాళీ
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    వెళ్ళు err_ret;
		}
	    }
	    లేకపోతే
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    ఒకవేళ (*p != endchar)
	వెళ్ళు err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    తిరిగి OK;

err_ret:
    ఒకవేళ (newargs != ఖాళీ)
	ga_clear_అక్షరశ్రేణులు(newargs);
    ఒకవేళ (!skip && default_వాదనలు != ఖాళీ)
	ga_clear_అక్షరశ్రేణులు(default_వాదనలు);
    తిరిగి FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
parse_వాదం_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    పూర్ణాంకం		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    పూర్ణాంకం		obj_member_లెక్క,
    cctx_T	*cctx)
{
    పూర్ణాంకం len = 0;

    ga_init2(&fp->uf_type_list, పరిమాణం(type_T *), 10);
    ఒకవేళ (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	ఒకవేళ (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	ఒకవేళ (fp->uf_arg_types != ఖాళీ)
	{
	    పూర్ణాంకం	i;
	    type_T	*type;

	    కొరకు (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_దత్తాంశం)[i];

		ఒకవేళ (p == ఖాళీ)
		    // will get the type from the default value
		    type = &t_unknown;
		లేకపోతే
		{
		    ఒకవేళ (arg_objm != ఖాళీ && ((int8_T *)arg_objm->ga_దత్తాంశం)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_వాదనలు.ga_దత్తాంశం)[i];

			type = &t_any;
			కొరకు (పూర్ణాంకం om = 0; om < obj_member_లెక్క; ++om)
			{
			    ఒకవేళ (obj_members != ఖాళీ
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				ఆపు;
			    }
			}
		    }
		    లేకపోతే
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		ఒకవేళ (type == ఖాళీ || !valid_declaration_type(type))
		    తిరిగి FAIL;
		fp->uf_arg_types[i] = type;
		ఒకవేళ (i < fp->uf_వాదనలు.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_వాదనలు.ga_దత్తాంశం)[i];
		    ఒకవేళ (obj_members != ఖాళీ && *name == '_')
			// protected object method
			name++;

		    ఒకవేళ (var_wrong_func_name(name, TRUE))
			తిరిగి FAIL;
		}
	    }
	}
    }

    ఒకవేళ (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_వాదనలు.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_వాదనలు.ga_దత్తాంశం)[fp->uf_వాదనలు.ga_len];
	((char_u **)fp->uf_వాదనలు.ga_దత్తాంశం)[fp->uf_వాదనలు.ga_len] = ఖాళీ;
	p = ((char_u **)argtypes->ga_దత్తాంశం)[len];
	ఒకవేళ (p == ఖాళీ)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	లేకపోతే
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    ఒకవేళ (fp->uf_va_type != ఖాళీ && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_దత్తాంశం)[len]);
		తిరిగి FAIL;
	    }
	}
	ఒకవేళ (fp->uf_va_type == ఖాళీ)
	    తిరిగి FAIL;
    }

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    ఒకవేళ (ret_type == ఖాళీ)
	fp->uf_ret_type = &t_void;
    లేకపోతే
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	ఒకవేళ (fp->uf_ret_type == ఖాళీ)
	{
	    fp->uf_ret_type = &t_void;
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    స్థిర పూర్ణాంకం
రిజిస్టరు_సంవృతం(ufunc_T *fp)
{
    ఒకవేళ (fp->uf_scoped == current_funccal)
	// no change
	తిరిగి OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    ఒకవేళ (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	తిరిగి FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_దత్తాంశం)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    తిరిగి OK;
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

    ఒకవేళ (name[0] != K_SPECIAL)
	తిరిగి name;
    len = STRLEN(name);
    ఒకవేళ (len + 3 > bufsize)
	తిరిగి name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    తిరిగి buf;
}

స్థిర char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(శూన్య)
{
    స్థిర పూర్ణాంకం	lambda_no = 0;
    పూర్ణాంకం		n;
    string_T	ret;

    n = vim_snprintf((అక్షరం *)lambda_name, పరిమాణం(lambda_name), "<lambda>%d", ++lambda_no);
    ఒకవేళ (n < 1)
	ret.నిడివి = 0;
    లేకపోతే ఒకవేళ (n >= (పూర్ణాంకం)పరిమాణం(lambda_name))
	ret.నిడివి = పరిమాణం(lambda_name) - 1;
    లేకపోతే
	ret.నిడివి = (size_t)n;

    ret.string = lambda_name;
    తిరిగి ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    స్థిర ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < పరిమాణం(ufunc_T) ? పరిమాణం(ufunc_T) : len);
    ఒకవేళ (fp != ఖాళీ)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((శూన్య *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	ఒకవేళ (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    ఒకవేళ (fp->uf_name_exp != ఖాళీ)
		vim_snprintf((అక్షరం *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    తిరిగి fp;
}

#ఒకవేళ defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
రిజిస్టరు_cfunc(cfunc_T cb, cfunc_free_T cb_free, శూన్య *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.నిడివి);
    ఒకవేళ (fp == ఖాళీ)
	తిరిగి ఖాళీ;

    fp->uf_def_స్థితి = UF_NOT_COMPILED;
    fp->uf_refcount = 1;
    fp->uf_varargs = TRUE;
    fp->uf_flags = FC_CFUNC | FC_LAMBDA;
    fp->uf_calls = 0;
    fp->uf_script_ctx = current_sctx;
    fp->uf_cb = cb;
    fp->uf_cb_free = cb_free;
    fp->uf_cb_state = state;

    hash_కూడు(&func_hashtab, UF2HIKEY(fp), "add C function");

    తిరిగి name.string;
}
#అయితే_ముగింపు

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    స్థిర char_u *
skip_arrow(
	char_u	*ప్రారంభం,
	పూర్ణాంకం	equal_arrow,
	char_u	**ret_type,
	పూర్ణాంకం	*white_error)
{
    char_u  *s = ప్రారంభం;
    char_u  *bef = ప్రారంభం - 2; // "start" points to > of ->

    ఒకవేళ (equal_arrow)
    {
	ఒకవేళ (*s == ':')
	{
	    ఒకవేళ (white_error != ఖాళీ && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		తిరిగి ఖాళీ;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    ఒకవేళ (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		తిరిగి ఖాళీ;
	    }
	}
	bef = s;
	s = skipwhite(s);
	ఒకవేళ (*s != '=')
	    తిరిగి ఖాళీ;
	++s;
    }
    ఒకవేళ (*s != '>')
	తిరిగి ఖాళీ;
    ఒకవేళ (white_error != ఖాళీ && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	తిరిగి ఖాళీ;
    }
    తిరిగి skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    స్థిర పూర్ణాంకం
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    ఒకవేళ (checkforcmd(&p, "function", 2))
    {
	ఒకవేళ (*p == '(')
	    తిరిగి FALSE;
	*cmd = p;
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    స్థిర శూన్య
function_using_ఖండం_scopes(ufunc_T *fp, cstack_T *cstack)
{
    ఒకవేళ (cstack == ఖాళీ || cstack->cs_idx < 0)
	తిరిగి;

    పూర్ణాంకం	    లెక్క = cstack->cs_idx + 1;
    పూర్ణాంకం	    i;

    fp->uf_ఖండం_ids = ALLOC_MULT(పూర్ణాంకం, లెక్క);
    ఒకవేళ (fp->uf_ఖండం_ids != ఖాళీ)
    {
	mch_memmove(fp->uf_ఖండం_ids, cstack->cs_ఖండం_id,
		పరిమాణం(పూర్ణాంకం) * లెక్క);
	fp->uf_ఖండం_లోతు = లెక్క;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    కొరకు (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    స్థిర char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    అయితే (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	ఒకవేళ (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	లేకపోతే ఒకవేళ (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    ఆపు;
	}
	లేకపోతే
	    MB_PTR_ADV(p);
    }

    తిరిగి p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    స్థిర char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    అయితే (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	ఒకవేళ (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	లేకపోతే ఒకవేళ (*p == '"')
	{
	    p++;    // skip the ending double quote
	    ఆపు;
	}
	లేకపోతే
	    MB_PTR_ADV(p);
    }

    తిరిగి p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    స్థిర char_u *
కనుగొను_ప్రారంభం_యొక్క_vim9_comment(char_u *పంక్తి)
{
    char_u	*p = పంక్తి;

    అయితే (*p != NUL)
    {
	ఒకవేళ (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	లేకపోతే ఒకవేళ (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	లేకపోతే
	{
	    ఒకవేళ (*p == '#')
		// Found the start of a Vim9 comment
		ఆపు;
	    MB_PTR_ADV(p);
	}
    }

    తిరిగి p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    స్థిర పూర్ణాంకం
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *పంక్తి_arg_లోపల,
	garray_T    *పంక్తులు_గమ్యం_free)
{
    linenr_T	sourcing_lnum_శిఖరం = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    పూర్ణాంకం		saved_wait_return = need_wait_return;
    char_u	*పంక్తి_arg = పంక్తి_arg_లోపల;
    పూర్ణాంకం		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_ఖండం;
#నిర్వచించు MAX_FUNC_NESTING 50
    అక్షరం	nesting_def[MAX_FUNC_NESTING];
    అక్షరం	nesting_inline[MAX_FUNC_NESTING];
    పూర్ణాంకం		nesting = 0;
    getline_opt_T getline_options;
    పూర్ణాంకం		indent = 2;
    char_u	*skip_until = ఖాళీ;
    పూర్ణాంకం		ret = FAIL;
    పూర్ణాంకం		is_heredoc = FALSE;
    పూర్ణాంకం		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = ఖాళీ;
    size_t	heredoc_trimmedlen = 0;
    పూర్ణాంకం		did_emsg_ప్రారంభం = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    ఒకవేళ (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	ఒకవేళ (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    వెళ్ళు theend;
	అయితే (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_దత్తాంశం))[newlines->ga_len++] = ఖాళీ;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_ఖండం;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    కొరకు (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	ఒకవేళ (కీలకం_Typed)
	{
	    సందేశం_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	ఒకవేళ (పంక్తి_arg != ఖాళీ)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = పంక్తి_arg;
	    p = vim_strchr(theline, '\n');
	    ఒకవేళ (p == ఖాళీ)
		పంక్తి_arg += STRLEN(పంక్తి_arg);
	    లేకపోతే
	    {
		*p = NUL;
		పంక్తి_arg = p + 1;
	    }
	}
	లేకపోతే
	{
	    theline = get_function_పంక్తి(eap, పంక్తులు_గమ్యం_free, indent,
							      getline_options);
	}
	ఒకవేళ (కీలకం_Typed)
	    పంక్తులు_ఎడమ = అడ్డువరుసలు - 1;
	ఒకవేళ (theline == ఖాళీ)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_శిఖరం;
	    ఒకవేళ (skip_until != ఖాళీ)
		semsg(_(e_missing_heredoc_చివర_marker_str), skip_until);
	    లేకపోతే ఒకవేళ (nesting_inline[nesting])
		emsg(_(e_missing_చివర_ఖండం));
	    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    లేకపోతే
		emsg(_(e_missing_endfunction));
	    వెళ్ళు theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	ఒకవేళ (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	లేకపోతే
	    sourcing_lnum_off = 0;

	ఒకవేళ (skip_until != ఖాళీ)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    ఒకవేళ (heredoc_trimmed == ఖాళీ
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		ఒకవేళ (heredoc_trimmed == ఖాళీ)
		    p = theline;
		లేకపోతే ఒకవేళ (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		లేకపోతే
		    p = theline + heredoc_trimmedlen;
		ఒకవేళ (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    ఒకవేళ (heredoc_concat_len > 0)
		    {
			// Replace the starting line with all the concatenated
			// lines.
			ga_concat(&heredoc_ga, theline);
			vim_free(((char_u **)(newlines->ga_దత్తాంశం))[
						      heredoc_concat_len - 1]);
			((char_u **)(newlines->ga_దత్తాంశం))[
				  heredoc_concat_len - 1] = heredoc_ga.ga_దత్తాంశం;
			ga_init(&heredoc_ga);
			heredoc_concat_len = 0;
			theline += STRLEN(theline);  // skip the "EOF"
		    }
		}
	    }
	}
	లేకపోతే
	{
	    పూర్ణాంకం	    c;
	    char_u  *చివర;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    కొరకు (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		ఒకవేళ (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    ఒకవేళ (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		ఒకవేళ (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		ఒకవేళ (nesting-- == 0)
		{
		    char_u *nextcmd = ఖాళీ;

		    ఒకవేళ (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    లేకపోతే ఒకవేళ (పంక్తి_arg != ఖాళీ && *skipwhite(పంక్తి_arg) != NUL)
			nextcmd = పంక్తి_arg;
		    లేకపోతే ఒకవేళ (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_శిఖరం
							+ newlines->ga_len + 1;
			ఒకవేళ (eap->cmdidx == CMD_def)
			    semsg(_(e_పాఠ్యం_దొరికింది_after_str_str), "enddef", p);
			లేకపోతే
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    ఒకవేళ (nextcmd != ఖాళీ && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			ఒకవేళ (పంక్తులు_గమ్యం_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)పంక్తులు_గమ్యం_free->ga_దత్తాంశం)
						   [పంక్తులు_గమ్యం_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)పంక్తులు_గమ్యం_free->ga_దత్తాంశం)
						   [పంక్తులు_గమ్యం_free->ga_len - 1];
			    --పంక్తులు_గమ్యం_free->ga_len;
			}
		    }
		    ఆపు;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    లేకపోతే ఒకవేళ (nesting_def[nesting])
	    {
		ఒకవేళ (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    ఒకవేళ (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    లేకపోతే ఒకవేళ (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    ఒకవేళ (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		ఒకవేళ (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, ఖాళీ, TRUE, 0));
		ఒకవేళ (vim9_function && *p == '<')
		{
		    // skip generic function
		    ఒకవేళ (skip_generic_func_type_వాదనలు(&p) == FAIL)
			వెళ్ళు theend;
		}
		ఒకవేళ (*skipwhite(p) == '(')
		{
		    ఒకవేళ (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    లేకపోతే
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    ఒకవేళ (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		ఒకవేళ (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    చివర = కనుగొను_ప్రారంభం_యొక్క_vim9_comment(p) - 1;
		}
		లేకపోతే
		    చివర = p + STRLEN(p) - 1;

		అయితే (చివర > p && VIM_ISWHITE(*చివర))
		    --చివర;
		ఒకవేళ (చివర > p + 1 && *చివర == '{' && VIM_ISWHITE(చివర[-1]))
		{
		    పూర్ణాంకం	    is_ఖండం;

		    // check for trailing "=> {": start of an inline function
		    --చివర;
		    అయితే (చివర > p && VIM_ISWHITE(*చివర))
			--చివర;
		    is_ఖండం = చివర > p + 2 && చివర[-1] == '=' && చివర[0] == '>';
		    ఒకవేళ (!is_ఖండం)
			is_ఖండం = కనుగొను_cmd_ఖండం_ప్రారంభం(p) != ఖాళీ;

		    ఒకవేళ (is_ఖండం)
		    {
			ఒకవేళ (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			లేకపోతే
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
	    ఒకవేళ (!vim9_function || colon)
		p = skip_range(p, FALSE, ఖాళీ);
	    char_u *tp = p;
	    ఒకవేళ (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    లేకపోతే
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    ఒకవేళ (arg[0] == '<' && arg[1] =='<'
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
		ఒకవేళ (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    ఒకవేళ (heredoc_trimmed == ఖాళీ)
			heredoc_trimmedlen = 0;
		}
		ఒకవేళ (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		లేకపోతే
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		ఒకవేళ (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    ఒకవేళ (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		ఒకవేళ (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    పూర్ణాంకం		save_sc_సంస్కరణ = current_sctx.sc_సంస్కరణ;
		    పూర్ణాంకం		var_లెక్క = 0;
		    పూర్ణాంకం		semicolon = 0;

		    current_sctx.sc_సంస్కరణ
				     = vim9_function ? SCRIPT_సంస్కరణ_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_లెక్క, &semicolon,
									 TRUE);
		    ఒకవేళ (arg != ఖాళీ)
			arg = skipwhite(arg);
		    current_sctx.sc_సంస్కరణ = save_sc_సంస్కరణ;
		    ఒకవేళ (arg != ఖాళీ && STRNCMP(arg, "=<<", 3) == 0)
		    {
			పూర్ణాంకం has_trim = FALSE;

			p = skipwhite(arg + 3);
			అయితే (TRUE)
			{
			    ఒకవేళ (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				కొనసాగు;
			    }
			    ఒకవేళ (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				కొనసాగు;
			    }
			    ఆపు;
			}
			ఒకవేళ (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    ఒకవేళ (heredoc_trimmed == ఖాళీ)
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
	ఒకవేళ (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    వెళ్ళు theend;

	ఒకవేళ (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	లేకపోతే
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	ఒకవేళ (p == ఖాళీ)
	    వెళ్ళు theend;
	((char_u **)(newlines->ga_దత్తాంశం))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	అయితే (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_దత్తాంశం))[newlines->ga_len++] = ఖాళీ;

	// Check for end of eap->arg.
	ఒకవేళ (పంక్తి_arg != ఖాళీ && *పంక్తి_arg == NUL)
	    పంక్తి_arg = ఖాళీ;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    ఒకవేళ (did_emsg == did_emsg_ప్రారంభం)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_దత్తాంశం);
    need_wait_return |= saved_wait_return;
    తిరిగి ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    స్థిర పూర్ణాంకం
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	పూర్ణాంకం	    varargs,
	garray_T    *default_వాదనలు,
	char_u	    *ret_type)
{
    char_u	*ప్రారంభం = *arg;
    పూర్ణాంకం		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = ఖాళీ;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = ఖాళీ;
    పూర్ణాంకం		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    పూర్ణాంకం		lnum_save = -1;
    linenr_T	sourcing_lnum_శిఖరం = SOURCING_LNUM;
    char_u	*పంక్తి_arg = ఖాళీ;

    *arg = skipwhite(*arg + 1);
    ఒకవేళ (**arg == '|' || !చివర్లు_excmd2(ప్రారంభం, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	తిరిగి FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    ఒకవేళ (**arg == '\n')
	పంక్తి_arg = *arg + 1;

    CLEAR_క్షేత్రం(eap);
    eap.cmdidx = CMD_ఖండం;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    ఒకవేళ (evalarg->eval_cctx != ఖాళీ)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    లేకపోతే
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, పరిమాణం(char_u *), 10);
    ఒకవేళ (get_function_body(&eap, &newlines, పంక్తి_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	వెళ్ళు erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_లెక్క += newlines.ga_len;
    ఒకవేళ (gap->ga_itemsize > 0)
    {
	పూర్ణాంకం	idx;
	char_u	*చివరి;
	size_t  plen;
	char_u  *pnl;

	కొరకు (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_దత్తాంశం)[idx];
	    ఒకవేళ (p == ఖాళీ)
		// comment line in the lambda body
		కొనసాగు;

	    p = skipwhite(p);

	    ఒకవేళ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		వెళ్ళు erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    ఒకవేళ (*p == NUL || vim9_comment_ప్రారంభం(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    లేకపోతే
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    ఒకవేళ (pnl != ఖాళీ)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_దత్తాంశం)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_దత్తాంశం)[freegap->ga_len++] = pnl;
	}
	ఒకవేళ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    వెళ్ళు erret;
	ఒకవేళ (eap.nextcmd != ఖాళీ)
	{
	    // more is following after the "}", which was skipped
	    చివరి = cmdline;
	    plen = STRLEN(చివరి);
	}
	లేకపోతే
	{
	    // nothing is following the "}"
	    చివరి = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	ఒకవేళ (pnl != ఖాళీ)
	    mch_memmove(pnl + 1, చివరి, plen + 1);
	((char_u **)gap->ga_దత్తాంశం)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_దత్తాంశం)[freegap->ga_len++] = pnl;
    }

    ఒకవేళ (eap.nextcmd != ఖాళీ)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	ఒకవేళ (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_దత్తాంశం))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    లేకపోతే
	*arg = (char_u *)"";

    ఒకవేళ (!evaluate)
    {
	ret = OK;
	వెళ్ళు erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.నిడివి);
    ఒకవేళ (ufunc == ఖాళీ)
	వెళ్ళు erret;
    ఒకవేళ (hash_కూడు(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	వెళ్ళు erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_వాదనలు = *newargs;
    newargs->ga_దత్తాంశం = ఖాళీ;
    ufunc->uf_def_వాదనలు = *default_వాదనలు;
    default_వాదనలు->ga_దత్తాంశం = ఖాళీ;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_శిఖరం;

    // parse argument types, the type variables of the function being
    // compiled may be used
    ఒకవేళ (parse_వాదం_types(ufunc, argtypes, varargs, ఖాళీ, ఖాళీ, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	వెళ్ళు erret;
    }

    // parse the return type, if any
    ఒకవేళ (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	వెళ్ళు erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    ఒకవేళ (pt == ఖాళీ)
	వెళ్ళు erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_పంక్తులు = newlines;
    newlines.ga_దత్తాంశం = ఖాళీ;
    ఒకవేళ (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    ఒకవేళ (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_సంస్కరణ = current_sctx.sc_సంస్కరణ;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_శిఖరం;
    సమితి_function_type(ufunc);

    function_using_ఖండం_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = ఖాళీ;
    ret = OK;

erret:
    ఒకవేళ (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_అక్షరశ్రేణులు(&newlines);
    ఒకవేళ (newargs != ఖాళీ)
	ga_clear_అక్షరశ్రేణులు(newargs);
    ga_clear_అక్షరశ్రేణులు(default_వాదనలు);
    ఒకవేళ (ufunc != ఖాళీ)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    తిరిగి ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    పూర్ణాంకం
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	పూర్ణాంకం	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    పూర్ణాంకం		evaluate = evalarg != ఖాళీ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_వాదనలు;
    garray_T	arg_objm;
    ufunc_T	*fp = ఖాళీ;
    partial_T   *pt = ఖాళీ;
    పూర్ణాంకం		varargs;
    char_u	*ret_type = ఖాళీ;
    పూర్ణాంకం		ret;
    char_u	*s;
    char_u	*ప్రారంభం, *చివర;
    పూర్ణాంకం		*old_eval_lavars = eval_lavars_వాడిన;
    పూర్ణాంకం		eval_lavars = FALSE;
    char_u	*tofree2 = ఖాళీ;
    పూర్ణాంకం		equal_arrow = **arg == '(';
    పూర్ణాంకం		white_error = FALSE;
    పూర్ణాంకం		called_emsg_ప్రారంభం = called_emsg;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    దీర్ఘ	ప్రారంభం_lnum = SOURCING_LNUM;

    ఒకవేళ (equal_arrow && !vim9script)
	తిరిగి NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_వాదనలు(&s, equal_arrow ? ')' : '-', ఖాళీ,
	    types_optional ? &argtypes : ఖాళీ, types_optional,
			types_optional ? &arg_objm : ఖాళీ, evalarg,
			ఖాళీ, &default_వాదనలు, TRUE, ఖాళీ, FALSE, ఖాళీ, ఖాళీ);
    ఒకవేళ (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, ఖాళీ) == ఖాళీ)
    {
	ఒకవేళ (types_optional)
	{
	    ga_clear_అక్షరశ్రేణులు(&argtypes);
	    ga_clear(&arg_objm);
	}
	తిరిగి called_emsg == called_emsg_ప్రారంభం ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    ఒకవేళ (evaluate)
	pnewargs = &newargs;
    లేకపోతే
	pnewargs = ఖాళీ;
    *arg += 1;
    ret = get_function_వాదనలు(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : ఖాళీ, types_optional,
			types_optional ? &arg_objm : ఖాళీ, evalarg,
					    &varargs, &default_వాదనలు,
					    FALSE, ఖాళీ, FALSE, ఖాళీ, ఖాళీ);
    ఒకవేళ (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : ఖాళీ)) == ఖాళీ)
    {
	ఒకవేళ (types_optional)
	{
	    ga_clear_అక్షరశ్రేణులు(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_అక్షరశ్రేణులు(&newargs);
	తిరిగి white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    ఒకవేళ (ret_type != ఖాళీ)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    ఒకవేళ (evaluate)
	eval_lavars_వాడిన = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    ఒకవేళ (equal_arrow && **arg == '{')
    {
	ఒకవేళ (evalarg == ఖాళీ)
	    // cannot happen?
	    వెళ్ళు theend;
	SOURCING_LNUM = ప్రారంభం_lnum;  // used for where lambda is defined
	ఒకవేళ (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : ఖాళీ, varargs,
			   &default_వాదనలు, ret_type) == FAIL)
	    వెళ్ళు errret;
	వెళ్ళు theend;
    }
    ఒకవేళ (default_వాదనలు.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_విలువలు_లోపల_lambda));
	వెళ్ళు errret;
    }

    // Get the start and the end of the expression.
    ప్రారంభం = *arg;
    ret = skip_సమాసం_concatenate(arg, &ప్రారంభం, &చివర, evalarg);
    ఒకవేళ (ret == FAIL)
	వెళ్ళు errret;

    ఒకవేళ (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ఒకవేళ (**arg != '}')
	{
	    semsg(_(e_expected_కుడి_curly_str), *arg);
	    వెళ్ళు errret;
	}
	++*arg;
    }

    ఒకవేళ (evaluate)
    {
	పూర్ణాంకం	    len;
	పూర్ణాంకం	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *పంక్తి_చివర;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.నిడివి);
	ఒకవేళ (fp == ఖాళీ)
	    వెళ్ళు errret;
	fp->uf_def_స్థితి = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	ఒకవేళ (pt == ఖాళీ)
	    వెళ్ళు errret;

	ga_init2(&newlines, పరిమాణం(char_u *), 1);
	ఒకవేళ (ga_grow(&newlines, 1) == FAIL)
	    వెళ్ళు errret;

	// If there are line breaks, we need to split up the string.
	పంక్తి_చివర = vim_strchr(ప్రారంభం, '\n');
	ఒకవేళ (పంక్తి_చివర == ఖాళీ || పంక్తి_చివర > చివర)
	    పంక్తి_చివర = చివర;

	// Add "return " before the expression (or the first line).
	len = 7 + (పూర్ణాంకం)(పంక్తి_చివర - ప్రారంభం) + 1;
	p = alloc(len);
	ఒకవేళ (p == ఖాళీ)
	    వెళ్ళు errret;
	((char_u **)(newlines.ga_దత్తాంశం))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, ప్రారంభం, పంక్తి_చివర - ప్రారంభం);

	ఒకవేళ (పంక్తి_చివర != చివర)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    అయితే (పంక్తి_చివర < చివర && *పంక్తి_చివర == '\n')
	    {
		ఒకవేళ (ga_grow(&newlines, 1) == FAIL)
		    వెళ్ళు errret;
		ప్రారంభం = పంక్తి_చివర + 1;
		పంక్తి_చివర = vim_strchr(ప్రారంభం, '\n');
		ఒకవేళ (పంక్తి_చివర == ఖాళీ || పంక్తి_చివర > చివర)
		    పంక్తి_చివర = చివర;
		((char_u **)(newlines.ga_దత్తాంశం))[newlines.ga_len++] =
					 vim_strnsave(ప్రారంభం, పంక్తి_చివర - ప్రారంభం);
	    }
	}

	ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)p + 7, "a:") == ఖాళీ)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_వాదనలు = newargs;
	ga_init(&fp->uf_def_వాదనలు);
	ఒకవేళ (types_optional)
	{
	    ఒకవేళ (parse_వాదం_types(fp, &argtypes,
				vim9script && varargs, ఖాళీ, ఖాళీ, 0,
				cctx) == FAIL)
		వెళ్ళు errret;
	    ఒకవేళ (ret_type != ఖాళీ)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     ఖాళీ, cctx, TRUE);
		ఒకవేళ (fp->uf_ret_type == ఖాళీ)
		    వెళ్ళు errret;
	    }
	    లేకపోతే
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_పంక్తులు = newlines;
	ఒకవేళ (current_funccal != ఖాళీ && eval_lavars)
	{
	    flags |= FC_సంవృతం;
	    ఒకవేళ (రిజిస్టరు_సంవృతం(fp) == FAIL)
		వెళ్ళు errret;
	}

#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (prof_def_func())
	    func_do_profile(fp);
#అయితే_ముగింపు
	ఒకవేళ (sandbox)
	    flags |= FC_SANDBOX;
	// In legacy script a lambda can be called with more args than
	// uf_args.ga_len.  In Vim9 script "...name" has to be used.
	fp->uf_varargs = !vim9script || varargs;
	fp->uf_flags = flags;
	fp->uf_calls = 0;
	fp->uf_script_ctx = current_sctx;
	// Use the line number of the arguments.
	fp->uf_script_ctx.sc_lnum += ప్రారంభం_lnum;

	function_using_ఖండం_scopes(fp, evalarg->eval_cstack);

	pt->pt_func = fp;
	pt->pt_refcount = 1;
	rettv->vval.v_partial = pt;
	rettv->v_type = VAR_PARTIAL;

	hash_కూడు(&func_hashtab, UF2HIKEY(fp), "add lambda");
    }

theend:
    eval_lavars_వాడిన = old_eval_lavars;
    vim_free(tofree2);
    ఒకవేళ (types_optional)
    {
	ga_clear_అక్షరశ్రేణులు(&argtypes);
	ga_clear(&arg_objm);
    }

    తిరిగి OK;

errret:
    ga_clear_అక్షరశ్రేణులు(&newargs);
    ga_clear_అక్షరశ్రేణులు(&newlines);
    ga_clear_అక్షరశ్రేణులు(&default_వాదనలు);
    ఒకవేళ (types_optional)
    {
	ga_clear_అక్షరశ్రేణులు(&argtypes);
	ga_clear(&arg_objm);
    }
    ఒకవేళ (fp != ఖాళీ)
    {
	vim_free(fp->uf_arg_types);
	vim_free(fp->uf_name_exp);
	vim_free(fp->uf_va_name);
	clear_type_list(&fp->uf_type_list);
	vim_free(fp);
    }
    vim_free(pt);
    vim_free(tofree2);
    eval_lavars_వాడిన = old_eval_lavars;
    తిరిగి FAIL;
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
	పూర్ణాంకం	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	పూర్ణాంకం	    no_autoload,
	పూర్ణాంకం	    new_function,
	పూర్ణాంకం	    *దొరికింది_var)
{
    dictitem_T	*v;
    typval_T	*tv = ఖాళీ;
    పూర్ణాంకం		cc;
    char_u	*s = ఖాళీ;
    hashtab_T	*ht;
    పూర్ణాంకం		did_type = FALSE;

    ఒకవేళ (partialp != ఖాళీ)
	*partialp = ఖాళీ;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = కనుగొను_var_also_లోపల_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    ఒకవేళ (v != ఖాళీ)
    {
	tv = &v->di_tv;
    }
    లేకపోతే ఒకవేళ (లోపల_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	పూర్ణాంకం	    len = *lenp;

	ఒకవేళ (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = కనుగొను_imported(p, len, FALSE);

	// imported function from another script
	ఒకవేళ (import != ఖాళీ)
	{
	    name[len] = NUL;
	    ఒకవేళ (new_function)
		semsg(_(e_redefining_imported_అంశం_str), name);
	    లేకపోతే
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    తిరిగి (char_u *)"";	// just in case
	}
    }

    ఒకవేళ (tv != ఖాళీ)
    {
	ఒకవేళ (దొరికింది_var != ఖాళీ)
	    *దొరికింది_var = TRUE;
	ఒకవేళ (tv->v_type == VAR_FUNC)
	{
	    ఒకవేళ (tv->vval.v_string == ఖాళీ)
	    {
		*lenp = 0;
		తిరిగి (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_కనుగొను_open_bracket(s);
	    ఒకవేళ (p != ఖాళీ && generic_func_కనుగొను_open_bracket(name) != ఖాళీ)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		తిరిగి (char_u *)"";
	    }
	    ఒకవేళ (p != ఖాళీ)
		*lenp = p - s;
	    లేకపోతే
		*lenp = (పూర్ణాంకం)STRLEN(s);
	}
	లేకపోతే ఒకవేళ (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    ఒకవేళ (pt == ఖాళీ)
	    {
		*lenp = 0;
		తిరిగి (char_u *)"";	// just in case
	    }
	    ఒకవేళ (partialp != ఖాళీ)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (పూర్ణాంకం)STRLEN(s);
	}

	ఒకవేళ (s != ఖాళీ)
	{
	    ఒకవేళ (!did_type && type != ఖాళీ && ht == get_script_local_ht())
	    {
		svar_T  *sv = కనుగొను_typval_లోపల_script(tv, 0, TRUE);

		ఒకవేళ (sv != ఖాళీ)
		    *type = sv->sv_type;
	    }
	    తిరిగి s;
	}
    }

    తిరిగి name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    శూన్య
emsg_funcname(అక్షరం *ermsg, char_u *name)
{
    char_u	*p = name;

    ఒకవేళ (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    ఒకవేళ (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    పూర్ణాంకం
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	పూర్ణాంకం	    partial_argc,
	typval_T    *argvars,
	పూర్ణాంకం	    *argcount,
	పూర్ణాంకం	    is_builtin)
{
    char_u	*argp = *arg;
    పూర్ణాంకం		ret = OK;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    పూర్ణాంకం		evaluate = evalarg == ఖాళీ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    అయితే (*argcount < MAX_FUNC_వాదనలు - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	ఒకవేళ (*argp == ')' || *argp == ',' || *argp == NUL)
	    ఆపు;

	పూర్ణాంకం arg_idx = *argcount;
	ఒకవేళ (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    ఆపు;
	}
	++*argcount;
	ఒకవేళ (!is_builtin && check_typval_is_విలువ(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    ఆపు;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	ఒకవేళ (vim9script)
	{
	    ఒకవేళ (*argp != ',' && *skipwhite(argp) == ',')
	    {
		ఒకవేళ (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		ఆపు;
	    }
	}
	లేకపోతే
	    argp = skipwhite(argp);
	ఒకవేళ (*argp != ',')
	    ఆపు;
	ఒకవేళ (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    ఒకవేళ (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    ఆపు;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    ఒకవేళ (*argp == ')')
	++argp;
    లేకపోతే
	ret = FAIL;
    *arg = argp;
    తిరిగి ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    పూర్ణాంకం
get_func_tv(
    char_u	*name,		// name of the function
    పూర్ణాంకం		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    పూర్ణాంకం		ret;
    typval_T	argvars[MAX_FUNC_వాదనలు + 1];	// vars for arguments
    పూర్ణాంకం		argcount = 0;			// number of arguments found
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    పూర్ణాంకం		evaluate = evalarg == ఖాళీ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == ఖాళీ ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    ఒకవేళ (ret == OK)
    {
	పూర్ణాంకం	i = 0;
	పూర్ణాంకం	did_emsg_before = did_emsg;

	ఒకవేళ (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    ఒకవేళ (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, పరిమాణం(typval_T *), 50);
	    కొరకు (i = 0; i < argcount; ++i)
		ఒకవేళ (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_దత్తాంశం)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	ఒకవేళ (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    లేకపోతే ఒకవేళ (!aborting() && evaluate)
    {
	ఒకవేళ (argcount == MAX_FUNC_వాదనలు)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	లేకపోతే
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    అయితే (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    ఒకవేళ (vim9script)
	*arg = argp;
    లేకపోతే
	*arg = skipwhite(argp);
    తిరిగి ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    స్థిర పూర్ణాంకం
eval_fname_sid(char_u *p)
{
    తిరిగి (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    ఒకవేళ (script_name == name)
	తిరిగి name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (పూర్ణాంకం)KE_SNR;
    fname_buflen = 3;
    ఒకవేళ (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    లేకపోతే
    {
	ఒకవేళ (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	లేకపోతే
	{
	    fname_buflen += vim_snprintf((అక్షరం *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(దీర్ఘ)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    ఒకవేళ (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    లేకపోతే
    {
	fname = alloc(fnamelen + 1);
	ఒకవేళ (fname == ఖాళీ)
	    *error = FCERR_OTHER;
	లేకపోతే
	{
	    *tofree = fname;
	    vim_snprintf((అక్షరం *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    తిరిగి fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    శూన్య
func_name_with_sid(char_u *name, పూర్ణాంకం sid, char_u *బఫరు)
{
    // A script-local function is stored as "<SNR>99_name".
    బఫరు[0] = K_SPECIAL;
    బఫరు[1] = KS_EXTRA;
    బఫరు[2] = (పూర్ణాంకం)KE_SNR;
    vim_snprintf((అక్షరం *)బఫరు + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (దీర్ఘ)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    స్థిర ufunc_T *
కనుగొను_func_with_sid(char_u *name, పూర్ణాంకం sid)
{
    hashitem_T	    *hi;
    char_u	    బఫరు[MAX_FUNC_NAME_LEN];

    ఒకవేళ (!SCRIPT_ID_VALID(sid))
	తిరిగి ఖాళీ;	// not in a script

    func_name_with_sid(name, sid, బఫరు);
    hi = hash_కనుగొను(&func_hashtab, బఫరు);
    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	తిరిగి HI2UF(hi);
    తిరిగి ఖాళీ;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    స్థిర ufunc_T *
కనుగొను_func_with_ఉపసర్గ(char_u *name, పూర్ణాంకం sid)
{
    hashitem_T	    *hi;
    char_u	    బఫరు[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    ఒకవేళ (vim_strchr(name, AUTOLOAD_CHAR) != ఖాళీ)
	తిరిగి ఖాళీ;	// already has the prefix
    ఒకవేళ (!SCRIPT_ID_VALID(sid))
	తిరిగి ఖాళీ;	// not in a script
    si = SCRIPT_అంశం(sid);
    ఒకవేళ (si->sn_autoload_ఉపసర్గ != ఖాళీ)
    {
	size_t	len = STRLEN(si->sn_autoload_ఉపసర్గ) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	ఒకవేళ (namep == ఖాళీ)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	ఒకవేళ (len < పరిమాణం(బఫరు))
	    auto_name = బఫరు;
	లేకపోతే
	    auto_name = alloc(len);
	ఒకవేళ (auto_name != ఖాళీ)
	{
	    vim_snprintf((అక్షరం *)auto_name, len, "%s%s",
						si->sn_autoload_ఉపసర్గ, namep);
	    hi = hash_కనుగొను(&func_hashtab, auto_name);
	    ఒకవేళ (auto_name != బఫరు)
		vim_free(auto_name);
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		తిరిగి HI2UF(hi);
	}
    }

    తిరిగి ఖాళీ;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
కనుగొను_func_even_dead(char_u *name, పూర్ణాంకం flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    ఒకవేళ ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	ఒకవేళ (లోపల_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = కనుగొను_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    ఒకవేళ (func != ఖాళీ)
		తిరిగి func;
	}
	ఒకవేళ (లోపల_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    దీర్ఘ    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    ఒకవేళ (*p == '_')
	    {
		func = కనుగొను_func_with_sid(p + 1, (పూర్ణాంకం)sid);
		ఒకవేళ (func != ఖాళీ)
		    తిరిగి func;
	    }
	}
    }

    ఒకవేళ ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_కనుగొను(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    తిరిగి HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    తిరిగి కనుగొను_func_with_ఉపసర్గ(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
కనుగొను_func(char_u *name, పూర్ణాంకం is_global)
{
    ufunc_T	*fp = కనుగొను_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    ఒకవేళ (fp != ఖాళీ && (fp->uf_flags & FC_DEAD) == 0)
	తిరిగి fp;
    తిరిగి ఖాళీ;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    పూర్ణాంకం
func_is_global(ufunc_T *ufunc)
{
    తిరిగి ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    పూర్ణాంకం
func_requires_g_ఉపసర్గ(ufunc_T *ufunc)
{
    తిరిగి func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ఖాళీ
	    && !సురక్షిత_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    స్థిర పూర్ణాంకం
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    పూర్ణాంకం	len;

    ఒకవేళ (!func_is_global(fp))
	len = vim_snprintf((అక్షరం *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    లేకపోతే
	len = vim_snprintf((అక్షరం *)buf, bufsize, "%s", fp->uf_name);

    తిరిగి (len >= (పూర్ణాంకం)bufsize) ? (పూర్ణాంకం)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    స్థిర శూన్య
కూడు_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    అక్షరం	*name,
    varnumber_T nr)
{
    STRCPY(v->di_కీలకం, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_కూడు(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_సంఖ్య;
    v->di_tv.v_తాళం = VAR_FIXED;
    v->di_tv.vval.v_సంఖ్య = nr;
}

/*
 * Free "fc".
 */
    స్థిర శూన్య
free_funccal(funccall_T *fc)
{
    పూర్ణాంకం	i;

    కొరకు (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_దత్తాంశం))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	ఒకవేళ (fp != ఖాళీ && fp->uf_scoped == fc)
	    fp->uf_scoped = ఖాళీ;
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
   స్థిర శూన్య
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_వస్తువులు(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    స్థిర శూన్య
cleanup_function_call(funccall_T *fc)
{
    పూర్ణాంకం	may_free_fc = fc->fc_refcount <= 0;
    పూర్ణాంకం	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    ఒకవేళ (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    లేకపోతే
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    ఒకవేళ (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    లేకపోతే
    {
	పూర్ణాంకం	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (పూర్ణాంకం)fc->fc_l_avars.dv_hashtab.ht_వాడిన;
	FOR_ALL_HASHTAB_వస్తువులు(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    ఒకవేళ (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_మొదటి = ఖాళీ;
    లేకపోతే
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_వస్తువులు(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    ఒకవేళ (free_fc)
	free_funccal(fc);
    లేకపోతే
    {
	స్థిర పూర్ణాంకం made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = మునుపటి_funccal;
	మునుపటి_funccal = fc;

	ఒకవేళ (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	లేకపోతే ఒకవేళ (++made_copy >= (పూర్ణాంకం)((4096 * 1024) / పరిమాణం(*fc)))
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
    స్థిర పూర్ణాంకం
numbered_function(char_u *name)
{
    తిరిగి సురక్షిత_isdigit(*name)
	    || (name[0] == 'g' && name[1] == ':' && సురక్షిత_isdigit(name[2]));
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
    పూర్ణాంకం
func_name_refcount(char_u *name)
{
    తిరిగి numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    స్థిర శూన్య
funccal_unref(funccall_T *fc, ufunc_T *fp, పూర్ణాంకం force)
{
    funccall_T	**pfc;
    పూర్ణాంకం		i;

    ఒకవేళ (fc == ఖాళీ)
	తిరిగి;

    ఒకవేళ (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	కొరకు (pfc = &మునుపటి_funccal; *pfc != ఖాళీ; pfc = &(*pfc)->fc_caller)
	{
	    ఒకవేళ (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		తిరిగి;
	    }
	}
    కొరకు (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	ఒకవేళ (((ufunc_T **)(fc->fc_ufuncs.ga_దత్తాంశం))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_దత్తాంశం))[i] = ఖాళీ;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    స్థిర పూర్ణాంకం
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    ఒకవేళ (fp->uf_flags & FC_DEAD)
	తిరిగి FALSE;

    hi = hash_కనుగొను(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    ఒకవేళ (HASHITEM_ఖాళీలు(hi) || HI2UF(hi) != fp)
	తిరిగి FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    ఒకవేళ (fp->uf_def_స్థితి == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	తిరిగి FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    తిరిగి TRUE;
}

    స్థిర శూన్య
func_clear_వస్తువులు(ufunc_T *fp)
{
    ga_clear_అక్షరశ్రేణులు(&(fp->uf_వాదనలు));
    ga_clear_అక్షరశ్రేణులు(&(fp->uf_def_వాదనలు));
    ga_clear_అక్షరశ్రేణులు(&(fp->uf_పంక్తులు));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_ఖండం_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    ఒకవేళ (IS_GENERIC_FUNC(fp))
	generic_func_clear_వస్తువులు(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = ఖాళీ;
    fp->uf_refcount -= 3;

#నిర్వచితమైతే FEAT_LUA
    ఒకవేళ (fp->uf_cb_free != ఖాళీ)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = ఖాళీ;
    }

    fp->uf_cb_state = ఖాళీ;
    fp->uf_cb = ఖాళీ;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_లెక్క);
    VIM_CLEAR(fp->uf_tml_మొత్తం);
    VIM_CLEAR(fp->uf_tml_self);
#అయితే_ముగింపు
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    స్థిర శూన్య
func_clear(ufunc_T *fp, పూర్ణాంకం force)
{
    ఒకవేళ (fp->uf_cleared)
	తిరిగి;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_వస్తువులు(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    స్థిర పూర్ణాంకం
func_free(ufunc_T *fp, పూర్ణాంకం force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    ఒకవేళ ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    ఒకవేళ ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	ఒకవేళ (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	తిరిగి OK;
    }
    తిరిగి FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    శూన్య
func_clear_free(ufunc_T *fp, పూర్ణాంకం force)
{
    func_clear(fp, force);
    ఒకవేళ (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    లేకపోతే
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    పూర్ణాంకం
copy_lambda_గమ్యం_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = కనుగొను_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = ఖాళీ;

    ఒకవేళ (ufunc == ఖాళీ)
    {
	semsg(_(e_lambda_function_not_దొరికింది_str), lambda);
	తిరిగి FAIL;
    }

    fp = కనుగొను_func(global, TRUE);
    ఒకవేళ (fp != ఖాళీ)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_కూడు_bang_గమ్యం_replace), global);
	తిరిగి FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    ఒకవేళ (fp == ఖాళీ)
	తిరిగి FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_స్థితి = ufunc->uf_def_స్థితి;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    ఒకవేళ (ga_copy_అక్షరశ్రేణులు(&ufunc->uf_వాదనలు, &fp->uf_వాదనలు) == FAIL
	    || ga_copy_అక్షరశ్రేణులు(&ufunc->uf_def_వాదనలు, &fp->uf_def_వాదనలు)
								    == FAIL
	    || ga_copy_అక్షరశ్రేణులు(&ufunc->uf_పంక్తులు, &fp->uf_పంక్తులు) == FAIL)
	వెళ్ళు failed;

    ఒకవేళ (ufunc->uf_arg_types != ఖాళీ)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_వాదనలు.ga_len);
	ఒకవేళ (fp->uf_arg_types == ఖాళీ)
	    వెళ్ళు failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    పరిమాణం(type_T *) * fp->uf_వాదనలు.ga_len);
    }
    ఒకవేళ (ufunc->uf_va_name != ఖాళీ)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	ఒకవేళ (fp->uf_va_name == ఖాళీ)
	    వెళ్ళు failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_కూడు(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    ఒకవేళ ((ufunc->uf_flags & FC_సంవృతం) && ufunc->uf_partial == ఖాళీ)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	ఒకవేళ (pt == ఖాళీ)
	    వెళ్ళు failed;
	ఒకవేళ (fill_partial_and_సంవృతం(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    వెళ్ళు failed;
	}
	ufunc->uf_partial = pt;
    }

    తిరిగి OK;

failed:
    func_clear_free(fp, TRUE);
    తిరిగి FAIL;
}

స్థిర పూర్ణాంకం	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    పూర్ణాంకం
funcdepth_increment(శూన్య)
{
    ఒకవేళ (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_లోతు_is_higher_than_maxfuncdepth));
	తిరిగి FAIL;
    }
    ++funcdepth;
    తిరిగి OK;
}

    శూన్య
funcdepth_decrement(శూన్య)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    పూర్ణాంకం
funcdepth_get(శూన్య)
{
    తిరిగి funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    శూన్య
funcdepth_restore(పూర్ణాంకం లోతు)
{
    funcdepth = లోతు;
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

    ఒకవేళ (fc == ఖాళీ)
	తిరిగి ఖాళీ;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    తిరిగి fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    శూన్య
remove_funccal(శూన్య)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    స్థిర funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    పూర్ణాంకం		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    పూర్ణాంకం		using_sandbox = FALSE;
    పూర్ణాంకం		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    పూర్ణాంకం		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    పూర్ణాంకం		default_arg_err = FALSE;
    dictitem_T	*v;
    పూర్ణాంకం		fixvar_idx = 0;	// index in fc_fixvar[]
    పూర్ణాంకం		i;
    పూర్ణాంకం		ai;
    పూర్ణాంకం		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_గమ్యం_free[MAX_FUNC_వాదనలు];
    పూర్ణాంకం		tv_గమ్యం_free_len = 0;
#నిర్వచితమైతే FEAT_PROFILE
    profinfo_T	profile_info;
#అయితే_ముగింపు
    ESTACK_CHECK_DECLARATION;

#నిర్వచితమైతే FEAT_PROFILE
    CLEAR_క్షేత్రం(profile_info);
#అయితే_ముగింపు

    // If depth of calling is getting too high, don't execute the function.
    ఒకవేళ (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_సంఖ్య;
	rettv->vval.v_సంఖ్య = -1;
	తిరిగి FCERR_FAILED;
    }

    పంక్తి_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    ఒకవేళ (fc == ఖాళీ)
	తిరిగి FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_కనుగొను_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, పరిమాణం(ufunc_T *), 1);

    ఒకవేళ (fp->uf_def_స్థితి != UF_NOT_COMPILED)
    {
	ఒకవేళ (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#నిర్వచితమైతే FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == ఖాళీ ? ఖాళీ : fc->fc_caller->fc_func;
#అయితే_ముగింపు
	// Execute the function, possibly compiling it first.
#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (do_profiling == PROF_YES)
	    profile_may_ప్రారంభం_func(&profile_info, fp, caller);
#అయితే_ముగింపు
	sticky_cmdmod_flags = 0;
	ఒకవేళ (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	ఒకవేళ (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != ఖాళీ && caller->uf_profiling)))
	    profile_may_చివర_func(&profile_info, fp, caller);
#అయితే_ముగింపు
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	తిరిగి retval;
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
    ఒకవేళ (selfdict != ఖాళీ)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_కీలకం;
	STRCPY(name, "self");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_కూడు(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "set self dictionary");
	v->di_tv.v_type = VAR_DICT;
	v->di_tv.v_తాళం = 0;
	v->di_tv.vval.v_dict = selfdict;
	++selfdict->dv_refcount;
    }

    /*
     * Init a: variables, unless none found (in lambda).
     * Set a:0 to "argcount" less number of named arguments, if >= 0.
     * Set a:000 to a list with room for the "..." arguments.
     */
    init_var_dict(&fc->fc_l_avars, &fc->fc_l_avars_var, VAR_SCOPE);
    ఒకవేళ ((fp->uf_flags & FC_NOARGS) == 0)
	కూడు_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_వాదనలు.ga_len
				    ? argcount - fp->uf_వాదనలు.ga_len : 0));
    fc->fc_l_avars.dv_తాళం = VAR_FIXED;
    ఒకవేళ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_కీలకం;
	STRCPY(name, "000");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_కూడు(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "function argument");
	v->di_tv.v_type = VAR_LIST;
	v->di_tv.v_తాళం = VAR_FIXED;
	v->di_tv.vval.v_list = &fc->fc_l_varlist;
    }
    CLEAR_క్షేత్రం(fc->fc_l_varlist);
    fc->fc_l_varlist.lv_refcount = DO_NOT_FREE_CNT;
    fc->fc_l_varlist.lv_తాళం = VAR_FIXED;

    /*
     * Set a:firstline to "firstline" and a:lastline to "lastline".
     * Set a:name to named arguments.
     * Set a:N to the "..." arguments.
     * Skipped when no a: variables used (in lambda).
     */
    ఒకవేళ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	కూడు_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	కూడు_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    కొరకు (i = 0; i < argcount || i < fp->uf_వాదనలు.ga_len; ++i)
    {
	పూర్ణాంకం	    addlocal = FALSE;
	typval_T    def_rettv;
	పూర్ణాంకం	    isdefault = FALSE;

	ai = i - fp->uf_వాదనలు.ga_len;
	ఒకవేళ (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    ఒకవేళ (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_వాదనలు.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_సంఖ్య == VVAL_NONE));
	    ఒకవేళ (isdefault)
	    {
		char_u	    *default_సమాసం = ఖాళీ;

		def_rettv.v_type = VAR_సంఖ్య;
		def_rettv.vval.v_సంఖ్య = -1;

		default_సమాసం = ((char_u **)(fp->uf_def_వాదనలు.ga_దత్తాంశం))
						 [ai + fp->uf_def_వాదనలు.ga_len];
		ఒకవేళ (eval1(&default_సమాసం, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    ఆపు;
		}
	    }

	    namelen = STRLEN(name);
	}
	లేకపోతే
	{
	    ఒకవేళ ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		ఆపు;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((అక్షరం *)numbuf, పరిమాణం(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	ఒకవేళ (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_కీలకం, name);
	}
	లేకపోతే
	{
	    v = dictitem_alloc(name);
	    ఒకవేళ (v == ఖాళీ)
		ఆపు;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_తాళం = VAR_FIXED;

	ఒకవేళ (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_గమ్యం_free[tv_గమ్యం_free_len++] = &v->di_tv;

	ఒకవేళ (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_కూడు(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	లేకపోతే
	    hash_కూడు(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	ఒకవేళ (ai >= 0 && ai < MAX_FUNC_వాదనలు)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_తాళం = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    ఒకవేళ (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_నెట్టు_ufunc(fp, 1);
    ESTACK_CHECK_అమరిక;
    ఒకవేళ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	ఒకవేళ (p_verbose >= 14)
	{
	    char_u	buf[సందేశం_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    సందేశం_puts("(");
	    కొరకు (i = 0; i < argcount; ++i)
	    {
		ఒకవేళ (i > 0)
		    సందేశం_puts(", ");
		ఒకవేళ (argvars[i].v_type == VAR_సంఖ్య)
		    సందేశం_outnum((దీర్ఘ)argvars[i].vval.v_సంఖ్య);
		లేకపోతే
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    ఒకవేళ (s != ఖాళీ)
		    {
			ఒకవేళ (vim_strsize(s) > సందేశం_BUF_CLEN)
			{
			    trunc_string(s, buf, సందేశం_BUF_CLEN, సందేశం_BUF_LEN);
			    s = buf;
			}
			సందేశం_puts((అక్షరం *)s);
			vim_free(tofree);
		    }
		}
	    }
	    సందేశం_puts(")");
	}
	సందేశం_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
	profile_may_ప్రారంభం_func(&profile_info, fp,
			fc->fc_caller == ఖాళీ ? ఖాళీ : fc->fc_caller->fc_func);
#అయితే_ముగింపు

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

    ఒకవేళ (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    లేకపోతే ఒకవేళ (islambda)
    {
	char_u *p = *(char_u **)fp->uf_పంక్తులు.ga_దత్తాంశం + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(శూన్య)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    లేకపోతే
	// call do_cmdline() to execute the lines
	do_cmdline(ఖాళీ, get_func_పంక్తి, (శూన్య *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    ఒకవేళ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    ఒకవేళ ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_సంఖ్య;
	rettv->vval.v_సంఖ్య = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	ఒకవేళ (లోపల_vim9script())
	    retval = FCERR_FAILED;
    }

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == ఖాళీ ? ఖాళీ : fc->fc_caller->fc_func;

	ఒకవేళ (fp->uf_profiling || (caller != ఖాళీ && caller->uf_profiling))
	    profile_may_చివర_func(&profile_info, fp, caller);
    }
#అయితే_ముగింపు

    // when being verbose, mention the return value
    ఒకవేళ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	ఒకవేళ (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	లేకపోతే ఒకవేళ (fc->fc_rettv->v_type == VAR_సంఖ్య)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (దీర్ఘ)fc->fc_rettv->vval.v_సంఖ్య);
	లేకపోతే
	{
	    char_u	buf[సందేశం_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    ఒకవేళ (s != ఖాళీ)
	    {
		ఒకవేళ (vim_strsize(s) > సందేశం_BUF_CLEN)
		{
		    trunc_string(s, buf, సందేశం_BUF_CLEN, సందేశం_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	సందేశం_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_తీయి();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_ప్రారంభం);
#అయితే_ముగింపు
    ఒకవేళ (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    ఒకవేళ (p_verbose >= 12 && SOURCING_NAME != ఖాళీ)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	సందేశం_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    కొరకు (i = 0; i < tv_గమ్యం_free_len; ++i)
	clear_tv(tv_గమ్యం_free[i]);
    cleanup_function_call(fc);

    తిరిగి retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, పూర్ణాంకం argcount)
{
    పూర్ణాంకం regular_వాదనలు = fp->uf_వాదనలు.ga_len;

    ఒకవేళ (argcount < regular_వాదనలు - fp->uf_def_వాదనలు.ga_len)
	తిరిగి FCERR_TOOFEW;
    లేకపోతే ఒకవేళ (!has_varargs(fp) && argcount > regular_వాదనలు)
	తిరిగి FCERR_TOOMANY;
    తిరిగి FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	పూర్ణాంకం	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#నిర్వచితమైతే FEAT_LUA
    ఒకవేళ (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	తిరిగి (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#అయితే_ముగింపు

    ఒకవేళ (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != ఖాళీ)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    ఒకవేళ (error != FCERR_UNKNOWN)
	తిరిగి error;

    ఒకవేళ ((fp->uf_flags & FC_DICT) && selfdict == ఖాళీ)
    {
	error = FCERR_DICT;
    }
    లేకపోతే
    {
	పూర్ణాంకం		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_శోధన_patterns();
	ఒకవేళ (!ins_compl_చురుకైన())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : ఖాళీ);
	ఒకవేళ (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	ఒకవేళ (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_శోధన_patterns();
    }

    తిరిగి error;
}

స్థిర funccal_నమోదు_T *funccal_దొంతర = ఖాళీ;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    శూన్య
save_funccal(funccal_నమోదు_T *నమోదు_2)
{
    నమోదు_2->శిఖరం_funccal = current_funccal;
    నమోదు_2->తదుపరి = funccal_దొంతర;
    funccal_దొంతర = నమోదు_2;
    current_funccal = ఖాళీ;
}

    శూన్య
restore_funccal(శూన్య)
{
    ఒకవేళ (funccal_దొంతర == ఖాళీ)
	అంతర్గత_error("restore_funccal()");
    లేకపోతే
    {
	current_funccal = funccal_దొంతర->శిఖరం_funccal;
	funccal_దొంతర = funccal_దొంతర->తదుపరి;
    }
}

    funccall_T *
get_current_funccal(శూన్య)
{
    తిరిగి current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    పూర్ణాంకం
at_script_level(శూన్య)
{
    తిరిగి current_funccal == ఖాళీ && autocmd_సరిపోలిక == ఖాళీ;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    శూన్య
delete_script_functions(పూర్ణాంకం sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (పూర్ణాంకం)KE_SNR;
    len = 3 + vim_snprintf((అక్షరం *)buf + 3, పరిమాణం(buf) - 3, "%d_", sid);

    అయితే (todo > 0)
    {
	todo = func_hashtab.ht_వాడిన;
	FOR_ALL_HASHTAB_వస్తువులు(&func_hashtab, hi, todo)
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		fp = HI2UF(hi);
		ఒకవేళ (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    పూర్ణాంకం మారింది = func_hashtab.ht_మారింది;

		    fp->uf_flags |= FC_DEAD;

		    ఒకవేళ (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			ఒకవేళ (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    లేకపోతే
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			ఒకవేళ (మారింది != func_hashtab.ht_మారింది)
			    ఆపు;
		    }
		}
		--todo;
	    }
    }
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_all_functions(శూన్య)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    పూర్ణాంకం		మారింది;

    // Clean up the current_funccal chain and the funccal stack.
    అయితే (current_funccal != ఖాళీ)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	ఒకవేళ (current_funccal == ఖాళీ && funccal_దొంతర != ఖాళీ)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    అయితే (todo > 0)
    {
	todo = func_hashtab.ht_వాడిన;
	FOR_ALL_HASHTAB_వస్తువులు(&func_hashtab, hi, todo)
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_స్థితి = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		ఒకవేళ (func_name_refcount(fp->uf_name))
		    ++skipped;
		లేకపోతే
		{
		    మారింది = func_hashtab.ht_మారింది;
		    func_clear(fp, TRUE);
		    ఒకవేళ (మారింది != func_hashtab.ht_మారింది)
		    {
			skipped = 0;
			ఆపు;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    అయితే (func_hashtab.ht_వాడిన > skipped)
    {
	todo = func_hashtab.ht_వాడిన;
	FOR_ALL_HASHTAB_వస్తువులు(&func_hashtab, hi, todo)
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		ఒకవేళ (func_name_refcount(fp->uf_name))
		    ++skipped;
		లేకపోతే
		{
		    ఒకవేళ (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			ఆపు;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    ఒకవేళ (skipped == 0)
	hash_clear(&func_hashtab);

    free_def_functions();
}
#అయితే_ముగింపు

/*
 * Return TRUE if "name" looks like a builtin function name: starts with a
 * lower case letter, doesn't contain AUTOLOAD_CHAR or ':', no "." after the
 * name.
 * "len" is the length of "name", or -1 for NUL terminated.
 */
    పూర్ణాంకం
builtin_function(char_u *name, పూర్ణాంకం len)
{
    పూర్ణాంకం i;

    ఒకవేళ (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	తిరిగి FALSE;
    కొరకు (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	ఒకవేళ (name[i] == AUTOLOAD_CHAR)
	    తిరిగి FALSE;
	ఒకవేళ (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    ఒకవేళ (name[i] == '.')
		తిరిగి FALSE;
	    ఆపు;
	}
    }
    తిరిగి TRUE;
}

    పూర్ణాంకం
func_call(
    char_u	*name,
    typval_T	*వాదనలు,
    partial_T	*partial,
    dict_T	*selfdict,
    typval_T	*rettv)
{
    list_T	*l = వాదనలు->vval.v_list;
    listitem_T	*అంశం;
    typval_T	argv[MAX_FUNC_వాదనలు + 1];
    పూర్ణాంకం		argc = 0;
    పూర్ణాంకం		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_వస్తువులు(l, అంశం)
    {
	ఒకవేళ (argc == MAX_FUNC_వాదనలు - (partial == ఖాళీ ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    ఆపు;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&అంశం->li_tv, &argv[argc++]);
    }

    ఒకవేళ (అంశం == ఖాళీ)
    {
	funcexe_T	funcexe;
	పూర్ణాంకం		namelen = -1;

	ఒకవేళ (లోపల_vim9script())
	{
	    char_u *p = generic_func_కనుగొను_open_bracket(name);

	    ఒకవేళ (p != ఖాళీ)
	    {
		ఒకవేళ (skip_generic_func_type_వాదనలు(&p) == FAIL)
		    వెళ్ళు పూర్తి;
		namelen = p - name + 1;
	    }
	}

	CLEAR_క్షేత్రం(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ఒకవేళ (partial != ఖాళీ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ఒకవేళ (funcexe.fe_object != ఖాళీ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    అయితే (argc > 0)
	clear_tv(&argv[--argc]);

పూర్తి:
    తిరిగి r;
}

స్థిర పూర్ణాంకం callback_లోతు = 0;

    పూర్ణాంకం
get_callback_లోతు(శూన్య)
{
    తిరిగి callback_లోతు;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    పూర్ణాంకం
call_callback(
    callback_T	*callback,
    పూర్ణాంకం		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    పూర్ణాంకం		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    పూర్ణాంకం		ret;

    ఒకవేళ (callback->cb_name == ఖాళీ || *callback->cb_name == NUL)
	తిరిగి FAIL;

    ఒకవేళ (callback_లోతు > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	తిరిగి FAIL;
    }

    CLEAR_క్షేత్రం(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    ఒకవేళ (callback->cb_partial != ఖాళీ)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	ఒకవేళ (funcexe.fe_object != ఖాళీ)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_లోతు;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_లోతు;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    ఒకవేళ (need_rethrow && current_exception != ఖాళీ && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    తిరిగి ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    పూర్ణాంకం		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    ఒకవేళ (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	తిరిగి -2;

    retval = tv_get_సంఖ్య_chk(&rettv, ఖాళీ);
    clear_tv(&rettv);
    తిరిగి retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    శూన్య
user_func_error(funcerror_T error, char_u *name, పూర్ణాంకం దొరికింది_var)
{
    ఎంపిక (error)
    {
	స్థితి FCERR_UNKNOWN:
		ఒకవేళ (దొరికింది_var)
		    emsg_funcname(e_not_callable_type_str, name);
		లేకపోతే
		    emsg_funcname(e_unknown_function_str, name);
		ఆపు;
	స్థితి FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_పద్ధతి_str, name);
		ఆపు;
	స్థితి FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		ఆపు;
	స్థితి FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		ఆపు;
	స్థితి FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		ఆపు;
	స్థితి FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_లోపల_script_context_str, name);
		ఆపు;
	స్థితి FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		ఆపు;
	స్థితి FCERR_OTHER:
	స్థితి FCERR_FAILED:
		// assume the error message was already given
		ఆపు;
	స్థితి FCERR_NONE:
		ఆపు;
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
    స్థిర funcerror_T
may_check_వాదం_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	పూర్ణాంకం	    argcount,
	పూర్ణాంకం	    పునాది_included,
	char_u	    *name)
{
    ఒకవేళ (funcexe->fe_check_type != ఖాళీ && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	ఒకవేళ (check_వాదం_types(funcexe->fe_check_type,
			  argvars, argcount,
			  పునాది_included ? ఖాళీ : funcexe->fe_basetv,
			  name) == FAIL)
	    తిరిగి FCERR_OTHER;
    }
    తిరిగి FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    పూర్ణాంకం
call_func(
    char_u	*funcname,	// name of the function
    పూర్ణాంకం		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    పూర్ణాంకం		argcount_లోపల,	// number of "argvars"
    typval_T	*argvars_లోపల,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    పూర్ణాంకం		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    పూర్ణాంకం		i;
    ufunc_T	*fp = ఖాళీ;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ఖాళీ;
    char_u	*fname = ఖాళీ;
    char_u	*name = ఖాళీ;
    పూర్ణాంకం		argcount = argcount_లోపల;
    typval_T	*argvars = argvars_లోపల;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_వాదనలు + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    పూర్ణాంకం		argv_clear = 0;
    పూర్ణాంకం		argv_పునాది = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_వాదనలు[MAX_FUNC_వాదనలు];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_వాదనలు_పట్టిక_init(&gfatab);

    ఒకవేళ (partial != ఖాళీ)
	fp = partial->pt_func;
    ఒకవేళ (fp == ఖాళీ)
	fp = funcexe->fe_ufunc;

    ఒకవేళ (fp == ఖాళీ)
    {
	ఒకవేళ (లోపల_vim9script())
	{
	    char_u *p = generic_func_కనుగొను_open_bracket(funcname);

	    ఒకవేళ (p != ఖాళీ)
	    {
		len = p - funcname;
		ఒకవేళ (parse_generic_func_type_వాదనలు(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == ఖాళీ)
		    వెళ్ళు theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	ఒకవేళ (name == ఖాళీ)
	    వెళ్ళు theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    ఒకవేళ (funcexe->fe_doesrange != ఖాళీ)
	*funcexe->fe_doesrange = FALSE;

    ఒకవేళ (partial != ఖాళీ)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	ఒకవేళ (partial->pt_dict != ఖాళీ && (selfdict == ఖాళీ || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	ఒకవేళ (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    కొరకు (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		ఒకవేళ (argv_clear + argcount_లోపల >= MAX_FUNC_వాదనలు)
		{
		    error = FCERR_TOOMANY;
		    వెళ్ళు theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    కొరకు (i = 0; i < argcount_లోపల; ++i)
		argv[i + argv_clear] = argvars_లోపల[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_లోపల;

	    ఒకవేళ (funcexe->fe_check_type != ఖాళీ
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_వాదనలు = check_type_వాదనలు;
		CLEAR_క్షేత్రం(check_type_వాదనలు);
		కొరకు (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_వాదనలు[i + partial->pt_argc] =
							 check_type.tt_వాదనలు[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    ఒకవేళ (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_వాదం_types(funcexe, argvars, argcount, FALSE,
					     (name != ఖాళీ) ? name : funcname);

    ఒకవేళ (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	పూర్ణాంకం	is_global = FALSE;

	// Skip "g:" before a function name.
	ఒకవేళ (fp == ఖాళీ && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_సంఖ్య;	// default rettv is number zero
	rettv->vval.v_సంఖ్య = 0;
	error = FCERR_UNKNOWN;

	ఒకవేళ (fp != ఖాళీ || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    ఒకవేళ (fp == ఖాళీ)
	    {
		fp = కనుగొను_func(rfname, is_global);
		ఒకవేళ (fp != ఖాళీ && !is_global && లోపల_vim9script()
						 && func_requires_g_ఉపసర్గ(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = ఖాళీ;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    ఒకవేళ (fp == ఖాళీ
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, ఖాళీ)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = కనుగొను_func(rfname, is_global);
	    }
	    // Try loading a package.
	    ఒకవేళ (fp == ఖాళీ && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = కనుగొను_func(rfname, is_global);
	    }
	    ఒకవేళ (fp == ఖాళీ)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		ఒకవేళ (p != ఖాళీ && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = కనుగొను_func(p, is_global);
	    }

	    ఒకవేళ (partial == ఖాళీ && fp != ఖాళీ && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		ఒకవేళ (fp == ఖాళీ)
		{
		    error = FCERR_FAILED;
		    వెళ్ళు theend;
		}
	    }
	    లేకపోతే ఒకవేళ (generic_func_వాదనలు_పట్టిక_కొలత(&gfatab) > 0)
	    {
		emsg_funcname(fp != ఖాళీ ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		వెళ్ళు theend;
	    }

	    ఒకవేళ (fp != ఖాళీ && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    లేకపోతే ఒకవేళ (fp != ఖాళీ)
	    {
		పూర్ణాంకం need_arg_check = FALSE;
		ఒకవేళ (funcexe->fe_check_type == ఖాళీ)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		ఒకవేళ (funcexe->fe_argv_func != ఖాళీ)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		ఒకవేళ (funcexe->fe_basetv != ఖాళీ)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, పరిమాణం(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_పునాది = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		ఒకవేళ (need_arg_check)
		    error = may_check_వాదం_types(funcexe, argvars, argcount,
				       TRUE, (name != ఖాళీ) ? name : funcname);

		ఒకవేళ (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	లేకపోతే ఒకవేళ (funcexe->fe_basetv != ఖాళీ)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_అంతర్గత_పద్ధతి(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	లేకపోతే
	{
	    /*
	     * Find the function name in the table, call its implementation.
	     */
	    error = call_అంతర్గత_func(fname, argcount, argvars, rettv);
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
    ఒకవేళ (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    ఒకవేళ (!aborting())
	user_func_error(error, (name != ఖాళీ) ? name : funcname,
							funcexe->fe_దొరికింది_var);

    // clear the copies made from the partial
    అయితే (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_పునాది]);

    vim_free(tofree);
    vim_free(name);
    generic_func_వాదనలు_పట్టిక_clear(&gfatab);

    తిరిగి ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    పూర్ణాంకం
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    పూర్ణాంకం		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ఖాళీ;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    పూర్ణాంకం		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_సంఖ్య;	// default rettv is number zero
    rettv->vval.v_సంఖ్య = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    ఒకవేళ (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    లేకపోతే
	rfname = fname;
    fp = కనుగొను_func(rfname, is_global);
    ఒకవేళ (fp != ఖాళీ && !is_global && లోపల_vim9script()
						 && func_requires_g_ఉపసర్గ(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = ఖాళీ;
    ఒకవేళ (fp == ఖాళీ)
	ret = NOTDONE;
    లేకపోతే ఒకవేళ (fp != ఖాళీ && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    లేకపోతే ఒకవేళ (fp != ఖాళీ)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_క్షేత్రం(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, ఖాళీ);
	ఒకవేళ (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    తిరిగి ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    తిరిగి fp->uf_name_exp != ఖాళీ ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    స్థిర పూర్ణాంకం
function_list_modified(పూర్ణాంకం prev_ht_మారింది)
{
    ఒకవేళ (prev_ht_మారింది != func_hashtab.ht_మారింది)
    {
	emsg(_(e_function_list_was_modified));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    స్థిర పూర్ణాంకం
list_func_తల(ufunc_T *fp, పూర్ణాంకం indent)
{
    పూర్ణాంకం		prev_ht_మారింది = func_hashtab.ht_మారింది;
    పూర్ణాంకం		j;

    సందేశం_ప్రారంభం();

    // a timer at the more prompt may have deleted the function
    ఒకవేళ (function_list_modified(prev_ht_మారింది))
	తిరిగి FAIL;

    ఒకవేళ (indent)
	సందేశం_puts("   ");
    ఒకవేళ (fp->uf_def_స్థితి != UF_NOT_COMPILED)
	సందేశం_puts("def ");
    లేకపోతే
	సందేశం_puts("function ");
    సందేశం_puts((అక్షరం *)printable_func_name(fp));
    సందేశం_putchar('(');
    కొరకు (j = 0; j < fp->uf_వాదనలు.ga_len; ++j)
    {
	ఒకవేళ (j)
	    సందేశం_puts(", ");
	సందేశం_puts((అక్షరం *)FUNCARG(fp, j));
	ఒకవేళ (fp->uf_arg_types != ఖాళీ)
	{
	    అక్షరం *tofree;

	    సందేశం_puts(": ");
	    సందేశం_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ఒకవేళ (j >= fp->uf_వాదనలు.ga_len - fp->uf_def_వాదనలు.ga_len)
	{
	    సందేశం_puts(" = ");
	    సందేశం_puts(((అక్షరం **)(fp->uf_def_వాదనలు.ga_దత్తాంశం))
		       [j - fp->uf_వాదనలు.ga_len + fp->uf_def_వాదనలు.ga_len]);
	}
    }
    ఒకవేళ (fp->uf_varargs)
    {
	ఒకవేళ (j)
	    సందేశం_puts(", ");
	సందేశం_puts("...");
    }
    ఒకవేళ (fp->uf_va_name != ఖాళీ)
    {
	ఒకవేళ (!fp->uf_varargs)
	{
	    ఒకవేళ (j)
		సందేశం_puts(", ");
	    సందేశం_puts("...");
	}
	సందేశం_puts((అక్షరం *)fp->uf_va_name);
	ఒకవేళ (fp->uf_va_type != ఖాళీ)
	{
	    అక్షరం *tofree;

	    సందేశం_puts(": ");
	    సందేశం_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    సందేశం_putchar(')');

    ఒకవేళ (fp->uf_def_స్థితి != UF_NOT_COMPILED)
    {
	ఒకవేళ (fp->uf_ret_type != &t_void)
	{
	    అక్షరం *tofree;

	    సందేశం_puts(": ");
	    సందేశం_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    లేకపోతే ఒకవేళ (fp->uf_flags & FC_ABORT)
	సందేశం_puts(" abort");
    ఒకవేళ (fp->uf_flags & FC_RANGE)
	సందేశం_puts(" range");
    ఒకవేళ (fp->uf_flags & FC_DICT)
	సందేశం_puts(" dict");
    ఒకవేళ (fp->uf_flags & FC_సంవృతం)
	సందేశం_puts(" closure");
    సందేశం_clr_eos();
    ఒకవేళ (p_verbose > 0)
	చివరి_సమితి_సందేశం(fp->uf_script_ctx);

    తిరిగి OK;
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
    పూర్ణాంకం		*is_global,
    పూర్ణాంకం		skip,		// only find the end, don't evaluate
    పూర్ణాంకం		flags)
{
    తిరిగి trans_function_name_ext(pp, is_global, skip, flags,
	    ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    స్థిర char_u *
trans_function_name_ext(
    char_u	**pp,
    పూర్ణాంకం		*is_global,
    పూర్ణాంకం		skip,		// only find the end, don't evaluate
    పూర్ణాంకం		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = ఖాళీ;
    char_u	*ప్రారంభం;
    char_u	*చివర;
    పూర్ణాంకం		lead;
    char_u	sid_buf[20];
    పూర్ణాంకం		len;
    పూర్ణాంకం		extra = 0;
    పూర్ణాంకం		ఉపసర్గ_g = FALSE;
    lval_T	lv;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    పూర్ణాంకం		vim9_local;

    ఒకవేళ (fdp != ఖాళీ)
	CLEAR_సూచిక(fdp);
    ప్రారంభం = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    ఒకవేళ ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (పూర్ణాంకం)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	తిరిగి vim_strnsave(ప్రారంభం, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(ప్రారంభం);
    ఒకవేళ (lead > 2)
	ప్రారంభం += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    చివర = get_lval(ప్రారంభం, ఖాళీ, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_ప్రారంభం);
    ఒకవేళ (చివర == ప్రారంభం || (vim9script && చివర != ఖాళీ
				   && చివర[-1] == AUTOLOAD_CHAR && *చివర == '('))
    {
	ఒకవేళ (!skip)
	    emsg(_(e_function_name_required));
	వెళ్ళు theend;
    }
    ఒకవేళ (చివర == ఖాళీ || (lv.ll_tv != ఖాళీ && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	ఒకవేళ (!aborting())
	{
	    ఒకవేళ (చివర != ఖాళీ)
		semsg(_(e_invalid_వాదం_str), ప్రారంభం);
	}
	లేకపోతే
	    *pp = కనుగొను_name_చివర(ప్రారంభం, ఖాళీ, ఖాళీ, FNE_INCL_BR);
	వెళ్ళు theend;
    }

    ఒకవేళ (lv.ll_ufunc != ఖాళీ)
    {
	ఒకవేళ (ufunc != ఖాళీ)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = చివర;
	వెళ్ళు theend;
    }

    ఒకవేళ (lv.ll_tv != ఖాళీ)
    {
	ఒకవేళ (fdp != ఖాళీ)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = ఖాళీ;
	    fdp->fd_di = lv.ll_di;
	}
	ఒకవేళ (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != ఖాళీ)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = చివర;
	}
	లేకపోతే ఒకవేళ (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != ఖాళీ)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.నిడివి);
	    *pp = చివర;
	}
	లేకపోతే ఒకవేళ (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != ఖాళీ)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = చివర;
	    ఒకవేళ (partial != ఖాళీ)
		*partial = lv.ll_tv->vval.v_partial;
	}
	లేకపోతే
	{
	    ఒకవేళ (!skip && !(flags & TFN_QUIET) && (fdp == ఖాళీ
			     || lv.ll_dict == ఖాళీ || fdp->fd_newkey == ఖాళీ))
		emsg(_(e_funcref_required));
	    లేకపోతే
		*pp = చివర;
	    name = ఖాళీ;
	}
	వెళ్ళు theend;
    }

    ఒకవేళ (lv.ll_name == ఖాళీ)
    {
	// Error found, but continue after the function name.
	*pp = చివర;
	వెళ్ళు theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    ఒకవేళ (lv.ll_exp_name != ఖాళీ)
    {
	len = (పూర్ణాంకం)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ఖాళీ);
	ఒకవేళ (name == lv.ll_exp_name)
	    name = ఖాళీ;
    }
    లేకపోతే ఒకవేళ (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_అంశం(lv.ll_sid);
	పూర్ణాంకం		cc = *lv.ll_name_చివర;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_చివర = NUL;
	ఒకవేళ (si->sn_autoload_ఉపసర్గ != ఖాళీ)
	{
	    name = concat_str(si->sn_autoload_ఉపసర్గ, lv.ll_name);
	}
	లేకపోతే
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (పూర్ణాంకం)KE_SNR;
	    vim_snprintf((అక్షరం *)sid_buf + 3, పరిమాణం(sid_buf) - 3,
						      "%ld_", (దీర్ఘ)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_చివర = cc;
	*pp = చివర;
	వెళ్ళు theend;
    }
    లేకపోతే ఒకవేళ (!(flags & TFN_NO_DEREF))
    {
	len = (పూర్ణాంకం)(చివర - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ఖాళీ);
	ఒకవేళ (name == *pp)
	    name = ఖాళీ;
    }
    ఒకవేళ (name != ఖాళీ)
    {
	name = vim_strsave(name);
	*pp = చివర;
	ఒకవేళ (name != ఖాళీ && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (పూర్ణాంకం)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	వెళ్ళు theend;
    }

    ఒకవేళ (lv.ll_exp_name != ఖాళీ)
    {
	len = (పూర్ణాంకం)STRLEN(lv.ll_exp_name);
	ఒకవేళ (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    లేకపోతే
    {
	// skip over "s:" and "g:"
	ఒకవేళ (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    ఒకవేళ (lv.ll_name[0] == 'g')
	    {
		ఒకవేళ (is_global != ఖాళీ)
		{
		    *is_global = TRUE;
		}
		లేకపోతే
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    ఉపసర్గ_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (పూర్ణాంకం)(చివర - lv.ll_name);
    }
    ఒకవేళ (len <= 0)
    {
	ఒకవేళ (!skip)
	    emsg(_(e_function_name_required));
	వెళ్ళు theend;
    }

    // In Vim9 script a user function is script-local by default, unless it
    // starts with a lower case character: dict.func().  Or when in a class.
    vim9_local = ASCII_ISUPPER(*ప్రారంభం) && vim9script
						&& (flags & TFN_లోపల_CLASS) == 0;

    /*
     * Copy the function name to allocated memory.
     * Accept <SID>name() inside a script, translate into <SNR>123_name().
     * Accept <SNR>123_name() outside a script.
     */
    ఒకవేళ (skip)
	lead = 0;	// do nothing
    లేకపోతే ఒకవేళ (lead > 0 || vim9_local)
    {
	ఒకవేళ (!vim9_local)
	{
	    ఒకవేళ (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_ప్రారంభం_with_capital_str), ప్రారంభం);
		వెళ్ళు theend;
	    }
	    lead = 3;
	}
	ఒకవేళ (vim9_local || (lv.ll_exp_name != ఖాళీ
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    ఒకవేళ (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_లోపల_script_context));
		వెళ్ళు theend;
	    }
	    sid_buflen = vim_snprintf((అక్షరం *)sid_buf, పరిమాణం(sid_buf), "%ld_", (దీర్ఘ)current_sctx.sc_sid);
	    ఒకవేళ (vim9_local)
		extra = 3 + (పూర్ణాంకం)sid_buflen;
	    లేకపోతే
		lead += (పూర్ణాంకం)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    లేకపోతే ఒకవేళ (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_లోపల_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_ప్రారంభం_with_capital_str
			   : e_function_name_must_ప్రారంభం_with_capital_or_s_str),
									ప్రారంభం);
	వెళ్ళు theend;
    }
    ఒకవేళ (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	ఒకవేళ (cp != ఖాళీ && cp < చివర)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), ప్రారంభం);
	    వెళ్ళు theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    ఒకవేళ (name != ఖాళీ)
    {
	ఒకవేళ (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (పూర్ణాంకం)KE_SNR;
	    ఒకవేళ (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	లేకపోతే ఒకవేళ (ఉపసర్గ_g)
	{
	    name[0] = 'g';
	    name[1] = ':';
	}
	mch_memmove(name + lead + extra, lv.ll_name, (size_t)len);
	name[lead + extra + len] = NUL;
    }
    *pp = చివర;

theend:
    clear_lval(&lv);
    తిరిగి name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    స్థిర char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    ఒకవేళ (*name == K_SPECIAL && లోపల_vim9script())
    {
	p = vim_strchr(name, '_');
	ఒకవేళ (p != ఖాళీ)
	    తిరిగి p + 1;
    }
    తిరిగి ఖాళీ;
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
    అక్షరం	sid_buf[25];
    size_t	sid_buflen;
    పూర్ణాంకం		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    ఒకవేళ (funcname == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	ఒకవేళ (!లోపల_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    తిరిగి ఖాళీ;
	ufunc = కనుగొను_func(funcname, FALSE);
	ఒకవేళ (ufunc == ఖాళీ || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == ఖాళీ)
	    తిరిగి ఖాళీ;
	++p;
	off = 0;
    }
    లేకపోతే
	off = *funcname == 's' ? 2 : 5;

    ఒకవేళ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_లోపల_script_context));
	తిరిగి ఖాళీ;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, పరిమాణం(sid_buf), "<SNR>%ld_",
	    (దీర్ఘ)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    ఒకవేళ (newname == ఖాళీ)
	తిరిగి ఖాళీ;
    vim_snprintf((అక్షరం *)newname, newnamesize, "%s%s", sid_buf, p + off);

    తిరిగి newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    ఒకవేళ (n != ఖాళీ)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    తిరిగి n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	పూర్ణాంకం	    *is_global,
	పూర్ణాంకం	    skip,
	పూర్ణాంకం	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    ఒకవేళ (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(శూన్య)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	ఒకవేళ (fudi != ఖాళీ)
	    CLEAR_సూచిక(fudi);
    }
    లేకపోతే
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, ఖాళీ, ఖాళీ, ఖాళీ);
    *name = p;
    తిరిగి saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    శూన్య
list_functions(regmatch_T *regmatch)
{
    పూర్ణాంకం		prev_ht_మారింది = func_hashtab.ht_మారింది;
    long_u	todo = func_hashtab.ht_వాడిన;
    hashitem_T	*hi;

    కొరకు (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    ఒకవేళ ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == ఖాళీ
			? !సందేశం_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !సురక్షిత_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		ఒకవేళ (list_func_తల(fp, FALSE) == FAIL)
		    తిరిగి;
		ఒకవేళ (function_list_modified(prev_ht_మారింది))
		    తిరిగి;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    స్థిర char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    ఒకవేళ (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	ఒకవేళ (regmatch.regprog != ఖాళీ)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    ఒకవేళ (*p == '/')
	++p;

    తిరిగి p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    స్థిర ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, పూర్ణాంకం is_global)
{
    ufunc_T	*fp = ఖాళీ;
    పూర్ణాంకం		j;

    ఒకవేళ (!చివర్లు_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	తిరిగి ఖాళీ;
    }

    సమితి_nextcmd(eap, p);

    ఒకవేళ (eap->nextcmd != ఖాళీ)
	*p = NUL;

    ఒకవేళ (eap->skip || got_int)
	తిరిగి ఖాళీ;

    fp = కనుగొను_func(name, is_global);
    ఒకవేళ (fp == ఖాళీ && ASCII_ISUPPER(*eap->arg))
    {
	char_u *పైకి = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	ఒకవేళ (పైకి != ఖాళీ)
	    fp = కనుగొను_func(పైకి, FALSE);
    }

    ఒకవేళ (fp == ఖాళీ)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	తిరిగి ఖాళీ;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    పూర్ణాంకం prev_ht_మారింది = func_hashtab.ht_మారింది;

    ఒకవేళ (list_func_తల(fp, TRUE) != OK)
	తిరిగి fp;

    కొరకు (j = 0; j < fp->uf_పంక్తులు.ga_len && !got_int; ++j)
    {
	ఒకవేళ (FUNCLINE(fp, j) == ఖాళీ)
	    కొనసాగు;
	సందేశం_putchar('\n');
	సందేశం_outnum((దీర్ఘ)(j + 1));
	ఒకవేళ (j < 9)
	    సందేశం_putchar(' ');
	ఒకవేళ (j < 99)
	    సందేశం_putchar(' ');
	ఒకవేళ (function_list_modified(prev_ht_మారింది))
	    ఆపు;
	సందేశం_prt_పంక్తి(FUNCLINE(fp, j), FALSE);
	బహిర్గతం_flush();	// show a line at a time
	ui_breakcheck();
    }

    ఒకవేళ (!got_int)
    {
	సందేశం_putchar('\n');
	ఒకవేళ (!function_list_modified(prev_ht_మారింది))
	{
	    ఒకవేళ (fp->uf_def_స్థితి != UF_NOT_COMPILED)
		సందేశం_puts("   enddef");
	    లేకపోతే
		సందేశం_puts("   endfunction");
	}
    }

    తిరిగి fp;
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
	garray_T    *పంక్తులు_గమ్యం_free,
	పూర్ణాంకం	    class_flags,
	ocmember_T  *obj_members,
	పూర్ణాంకం         obj_member_లెక్క,
	cctx_T	    *cctx)
{
    పూర్ణాంకం		j;
    పూర్ణాంకం		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    పూర్ణాంకం		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*పంక్తి_arg = ఖాళీ;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_వాదనలు;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    పూర్ణాంకం		varargs = FALSE;
    పూర్ణాంకం		flags = 0;
    char_u	*ret_type = ఖాళీ;
    ufunc_T	*fp = ఖాళీ;
    పూర్ణాంకం		fp_allocated = FALSE;
    పూర్ణాంకం		free_fp = FALSE;
    పూర్ణాంకం		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    స్థిర పూర్ణాంకం	func_nr = 0;	    // number for nameless function
    పూర్ణాంకం		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_శిఖరం;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    imported_T	*import = ఖాళీ;

    // ":function" without argument: list functions.
    ఒకవేళ (చివర్లు_excmd2(eap->cmd, eap->arg))
    {
	ఒకవేళ (!eap->skip)
	    list_functions(ఖాళీ);
	సమితి_nextcmd(eap, eap->arg);
	తిరిగి ఖాళీ;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    ఒకవేళ (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	సమితి_nextcmd(eap, p);
	తిరిగి ఖాళీ;
    }

    ga_init(&newargs);
    ga_init(&argtypes);
    ga_init(&arg_objm);
    ga_init(&default_వాదనలు);
    generic_func_వాదనలు_పట్టిక_init(&gfatab);

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
    ఒకవేళ (name_arg != ఖాళీ)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_క్షేత్రం(fudi);
    }
    లేకపోతే
    {
	ఒకవేళ (vim9script)
	{
	    ఒకవేళ (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_లోపల_vim9_script_str), p);
		తిరిగి ఖాళీ;
	    }
	    p = గమ్యం_name_చివర(p, TRUE);
	    ఒకవేళ (*skipwhite(p) == '.' && vim_strchr(p, '(') != ఖాళీ)
	    {
		semsg(_(e_cannot_define_dict_func_లోపల_vim9_script_str),
								     eap->arg);
		తిరిగి ఖాళీ;
	    }

	    p = eap->arg;
	}

	పూర్ణాంకం tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_లోపల_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != ఖాళీ);
	ఒకవేళ (name == ఖాళీ && (fudi.fd_dict == ఖాళీ || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    ఒకవేళ (!aborting())
	    {
		ఒకవేళ (!eap->skip && fudi.fd_newkey != ఖాళీ)
		    semsg(_(e_కీలకం_not_present_లోపల_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		తిరిగి ఖాళీ;
	    }
	    లేకపోతే
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	ఒకవేళ (is_export && name != ఖాళీ)
	{
	    char_u *prefixed = may_ఉపసర్గ_autoload(name);

	    ఒకవేళ (prefixed != ఖాళీ && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	లేకపోతే ఒకవేళ (paren && vim9script && name != ఖాళీ
				    && vim_strchr(name, AUTOLOAD_CHAR) != ఖాళీ)
	{
	    emsg(_(e_cannot_use_name_with_hash_లోపల_vim9_script_use_export_instead));
	    వెళ్ళు ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    ఒకవేళ (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	వెళ్ళు ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    ఒకవేళ (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	ఒకవేళ (p == ఖాళీ)
	    వెళ్ళు ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    ఒకవేళ (*p != '(')
    {
	ఒకవేళ (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    వెళ్ళు ret_free;
	}
	// attempt to continue by skipping some text
	ఒకవేళ (vim_strchr(p, '(') != ఖాళీ)
	    p = vim_strchr(p, '(');
    }

    ఒకవేళ ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	వెళ్ళు ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    ఒకవేళ (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	వెళ్ళు ret_free;
    }

    ga_init2(&newlines, పరిమాణం(char_u *), 10);

    ఒకవేళ (!eap->skip && name_arg == ఖాళీ)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	ఒకవేళ (name != ఖాళీ)
	    arg = name;
	లేకపోతే
	    arg = fudi.fd_newkey;
	ఒకవేళ (arg != ఖాళీ && (fudi.fd_di == ఖాళీ
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_పునాది = arg;
	    పూర్ణాంకం	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    ఒకవేళ (arg != fudi.fd_newkey)
	    {
		ఒకవేళ (*arg == K_SPECIAL)
		{
		    name_పునాది = vim_strchr(arg, '_');
		    ఒకవేళ (name_పునాది == ఖాళీ)
			name_పునాది = arg + 3;
		    లేకపోతే
			++name_పునాది;
		}
		కొరకు (i = 0; name_పునాది[i] != NUL && (i == 0
					    ? eval_isnamec1(name_పునాది[i])
					    : eval_isnamec(name_పునాది[i])); ++i)
		    ;
		ఒకవేళ (name_పునాది[i] != NUL)
		{
		    emsg_funcname(e_invalid_వాదం_str, arg);
		    వెళ్ళు ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		ఒకవేళ (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_పునాది, i, 0, ఖాళీ,
			ఖాళీ, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_అంశం_str), name_పునాది);
		    వెళ్ళు ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	ఒకవేళ (fudi.fd_dict != ఖాళీ && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_ఇక్కడ));
	    వెళ్ళు ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    ఒకవేళ (get_function_వాదనలు(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : ఖాళీ, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : ఖాళీ,
			 ఖాళీ, &varargs, &default_వాదనలు, eap->skip,
			 eap, class_flags, &newlines, పంక్తులు_గమ్యం_free) == FAIL)
	వెళ్ళు errret_2;
    whitep = p;

    ఒకవేళ (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	ఒకవేళ (*skipwhite(p) == ':')
	{
	    ఒకవేళ (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    లేకపోతే ఒకవేళ (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    ఒకవేళ (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    లేకపోతే
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = ఖాళీ;
	    }
	}
	p = skipwhite(p);
    }
    లేకపోతే
	// find extra arguments "range", "dict", "abort" and "closure"
	కొరకు (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    ఒకవేళ (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_సంవృతం;
		p += 7;
		ఒకవేళ (current_funccal == ఖాళీ)
		{
		    emsg_funcname(e_సంవృతం_function_should_not_be_at_శిఖరం_level_str,
			    name == ఖాళీ ? (char_u *)"" : name);
		    వెళ్ళు erret;
		}
	    }
	    లేకపోతే
		ఆపు;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    ఒకవేళ (*p == '\n')
	పంక్తి_arg = p + 1;
    లేకపోతే ఒకవేళ (*p != NUL
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
    ఒకవేళ (కీలకం_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	ఒకవేళ (!eap->skip && !eap->forceit)
	{
	    ఒకవేళ (fudi.fd_dict != ఖాళీ && fudi.fd_newkey == ఖాళీ)
		emsg(_(e_dictionary_నమోదు_already_exists));
	    లేకపోతే ఒకవేళ (name != ఖాళీ && కనుగొను_func(name, is_global) != ఖాళీ)
		emsg_funcname(e_function_str_already_exists_కూడు_bang_గమ్యం_replace, name);
	}

	ఒకవేళ (!eap->skip && did_emsg)
	    వెళ్ళు erret;

	సందేశం_putchar('\n');	    // don't overwrite the function name
	cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
    }

    // Save the starting line number.
    sourcing_lnum_శిఖరం = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    ఒకవేళ (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_పద్ధతి) == 0
		&& (get_function_body(eap, &newlines, పంక్తి_arg, పంక్తులు_గమ్యం_free)
							== FAIL || did_emsg))
	    || eap->skip)
	వెళ్ళు erret;

    /*
     * If there are no errors, add the function
     */
    ఒకవేళ (fudi.fd_dict != ఖాళీ)
    {
	అక్షరం	numbuf[NUMBUFLEN];

	fp = ఖాళీ;
	ఒకవేళ (fudi.fd_newkey == ఖాళీ && !eap->forceit)
	{
	    emsg(_(e_dictionary_నమోదు_already_exists));
	    వెళ్ళు erret;
	}
	ఒకవేళ (fudi.fd_di == ఖాళీ)
	{
	    // Can't add a function to a locked dictionary
	    ఒకవేళ (విలువ_check_తాళం(fudi.fd_dict->dv_తాళం, eap->arg, FALSE))
		వెళ్ళు erret;
	}
	    // Can't change an existing function if it is locked
	లేకపోతే ఒకవేళ (విలువ_check_తాళం(fudi.fd_di->di_tv.v_తాళం, eap->arg, FALSE))
	    వెళ్ళు erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, పరిమాణం(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	ఒకవేళ (name == ఖాళీ)
	    వెళ్ళు erret;
    }
    లేకపోతే ఒకవేళ (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*కనుగొను_name = name;
	పూర్ణాంకం		var_conflict = FALSE;
	పూర్ణాంకం		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = కనుగొను_var(name, &ht, TRUE);
	ఒకవేళ (v != ఖాళీ && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	ఒకవేళ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_అంశం(current_sctx.sc_sid);

	    ఒకవేళ (si->sn_autoload_ఉపసర్గ != ఖాళీ)
	    {
		ఒకవేళ (is_export)
		{
		    కనుగొను_name = name + STRLEN(si->sn_autoload_ఉపసర్గ);
		    v = కనుగొను_var(కనుగొను_name, &ht, TRUE);
		    ఒకవేళ (v != ఖాళీ)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		లేకపోతే
		{
		    char_u *prefixed = may_ఉపసర్గ_autoload(name);

		    ఒకవేళ (prefixed != ఖాళీ && prefixed != name)
		    {
			v = కనుగొను_var(prefixed, &ht, TRUE);
			ఒకవేళ (v != ఖాళీ)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	ఒకవేళ (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    వెళ్ళు erret;
	}

	fp = కనుగొను_func_even_dead(కనుగొను_name, ffed_flags);
	ఒకవేళ (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = కనుగొను_imported(uname == ఖాళీ ? name : uname, 0, FALSE);
	}

	ఒకవేళ (fp != ఖాళీ || import != ఖాళీ)
	{
	    పూర్ణాంకం dead = fp != ఖాళీ && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    ఒకవేళ (import != ఖాళీ
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_శిఖరం;
		ఒకవేళ (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		లేకపోతే
		    emsg_funcname(e_function_str_already_exists_కూడు_bang_గమ్యం_replace, name);
		వెళ్ళు errret_keep;
	    }
	    ఒకవేళ (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_లోపల_use, name);
		వెళ్ళు errret_keep;
	    }
	    ఒకవేళ (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = ఖాళీ;
		overwrite = TRUE;
	    }
	    లేకపోతే
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = ఖాళీ;
		func_clear_వస్తువులు(fp);
		fp->uf_name_exp = exp_name;
		fp->uf_flags &= ~FC_DEAD;
#నిర్వచితమైతే FEAT_PROFILE
		fp->uf_profiling = FALSE;
		fp->uf_prof_initialized = FALSE;
#అయితే_ముగింపు
		fp->uf_def_స్థితి = UF_NOT_COMPILED;
	    }
	}
    }

    ఒకవేళ (fp == ఖాళీ)
    {
	ఒకవేళ (fudi.fd_dict == ఖాళీ && vim_strchr(name, AUTOLOAD_CHAR) != ఖాళీ)
	{
	    పూర్ణాంకం	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    ఒకవేళ (SOURCING_NAME != ఖాళీ)
	    {
		scriptname = autoload_name(name);
		ఒకవేళ (scriptname != ఖాళీ)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (పూర్ణాంకం)STRLEN(p);
		    slen = (పూర్ణాంకం)STRLEN(SOURCING_NAME);
		    ఒకవేళ (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    ఒకవేళ (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_శిఖరం;
		semsg(_(e_function_name_does_not_సరిపోలిక_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		వెళ్ళు erret;
	    }
	}

	ఒకవేళ (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	ఒకవేళ (fp == ఖాళీ)
	    వెళ్ళు erret;
	fp_allocated = TRUE;

	ఒకవేళ (fudi.fd_dict != ఖాళీ)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    ఒకవేళ (func_name == ఖాళీ)
	    {
		VIM_CLEAR(fp);
		వెళ్ళు erret;
	    }
	    ఒకవేళ (fudi.fd_di == ఖాళీ)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		ఒకవేళ (fudi.fd_di == ఖాళీ)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    వెళ్ళు erret;
		}
		ఒకవేళ (dict_కూడు(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    వెళ్ళు erret;
		}
	    }
	    లేకపోతే
		// overwrite existing dict entry
		clear_tv(&fudi.fd_di->di_tv);
	    fudi.fd_di->di_tv.v_type = VAR_FUNC;
	    fudi.fd_di->di_tv.vval.v_string = func_name;

	    // behave like "dict" was used
	    flags |= FC_DICT;
	}
    }
    fp->uf_వాదనలు = newargs;
    fp->uf_def_వాదనలు = default_వాదనలు;
    fp->uf_ret_type = &t_any;
    fp->uf_func_type = &t_func_any;

    ఒకవేళ (eap->cmdidx == CMD_def)
    {
	పూర్ణాంకం	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_స్థితి = UF_గమ్యం_BE_COMPILED;

	ఒకవేళ (generic_func_వాదనలు_పట్టిక_కొలత(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_శిఖరం;

	// The function may use script variables from the context.
	function_using_ఖండం_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	పూర్ణాంకం save_is_export = is_export;
	is_export = FALSE;

	ఒకవేళ (parse_వాదం_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_లెక్క, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    వెళ్ళు erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	ఒకవేళ (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    వెళ్ళు erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    లేకపోతే
	fp->uf_def_స్థితి = UF_NOT_COMPILED;

    ఒకవేళ (fp_allocated)
    {
	// insert the new function in the function list
	ఒకవేళ (overwrite)
	{
	    hi = hash_కనుగొను(&func_hashtab, name);
	    hi->hi_కీలకం = UF2HIKEY(fp);
	}
	లేకపోతే ఒకవేళ (class_flags == 0 && hash_కూడు(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    వెళ్ళు erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_పంక్తులు = newlines;
    newlines.ga_దత్తాంశం = ఖాళీ;
    ఒకవేళ ((flags & FC_సంవృతం) != 0)
    {
	ఒకవేళ (రిజిస్టరు_సంవృతం(fp) == FAIL)
	    వెళ్ళు erret;
    }
    లేకపోతే
	fp->uf_scoped = ఖాళీ;

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (prof_def_func())
	func_do_profile(fp);
#అయితే_ముగింపు
    fp->uf_varargs = varargs;
    ఒకవేళ (sandbox)
	flags |= FC_SANDBOX;
    ఒకవేళ (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_సంస్కరణ = current_sctx.sc_సంస్కరణ;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_శిఖరం;
    ఒకవేళ (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    ఒకవేళ (eap->cmdidx == CMD_def)
	సమితి_function_type(fp);
    లేకపోతే ఒకవేళ (fp->uf_script_ctx.sc_సంస్కరణ == SCRIPT_సంస్కరణ_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    ఒకవేళ (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, ఖాళీ);

    వెళ్ళు ret_free;

erret:
    ఒకవేళ (fp != ఖాళీ)
    {
	// these were set to "newargs" and "default_args", which are cleared
	//క్రింద
	ga_init(&fp->uf_వాదనలు);
	ga_init(&fp->uf_def_వాదనలు);
    }
errret_2:
    ఒకవేళ (fp != ఖాళీ)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	ఒకవేళ (IS_GENERIC_FUNC(fp))
	    generic_func_clear_వస్తువులు(fp);
    }
    ఒకవేళ (free_fp)
	VIM_CLEAR(fp);
errret_keep:
    ga_clear_అక్షరశ్రేణులు(&newargs);
    ga_clear_అక్షరశ్రేణులు(&default_వాదనలు);
    ga_clear_అక్షరశ్రేణులు(&newlines);
ret_free:
    ga_clear_అక్షరశ్రేణులు(&argtypes);
    ga_clear(&arg_objm);
    // The generic types are still in use and should not be freed.  Instead
    // clear the grow array.
    ga_clear(&gfatab.gfat_param_types);
    generic_func_వాదనలు_పట్టిక_clear(&gfatab);
    vim_free(fudi.fd_newkey);
    ఒకవేళ (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    తిరిగి fp;
}

/*
 * ":function"
 */
    శూన్య
ex_function(exarg_T *eap)
{
    garray_T పంక్తులు_గమ్యం_free;

    ga_init2(&పంక్తులు_గమ్యం_free, పరిమాణం(char_u *), 50);
    (శూన్య)define_function(eap, ఖాళీ, &పంక్తులు_గమ్యం_free, 0, ఖాళీ, 0, ఖాళీ);
    ga_clear_అక్షరశ్రేణులు(&పంక్తులు_గమ్యం_free);
}

    పూర్ణాంకం
get_func_arity(char_u *name, పూర్ణాంకం *required, పూర్ణాంకం *optional, పూర్ణాంకం *varargs)
{
    ufunc_T	*ufunc = ఖాళీ;
    పూర్ణాంకం		argcount = 0;
    పూర్ణాంకం		min_argcount = 0;
    పూర్ణాంకం		idx;

    idx = కనుగొను_అంతర్గత_func(name);
    ఒకవేళ (idx >= 0)
    {
	అంతర్గత_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    లేకపోతే
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = ఖాళీ;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ఒకవేళ (error == FCERR_NONE)
	    ufunc = కనుగొను_func(fname, FALSE);
	vim_free(tofree);

	ఒకవేళ (ufunc == ఖాళీ)
	    తిరిగి FAIL;

	argcount = ufunc->uf_వాదనలు.ga_len;
	min_argcount = ufunc->uf_వాదనలు.ga_len - ufunc->uf_def_వాదనలు.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    తిరిగి OK;
}

/*
 * Find a function by name, including "<lambda>123".
 * Check for "profile" and "debug" arguments and set"compile_type".
 * Caller should initialize "compile_type" to CT_NONE.
 * Return NULL if not found.
 */
    ufunc_T *
కనుగొను_func_by_name(char_u *name, compiletype_T *compile_type)
{
    char_u	*arg = name;
    char_u	*fname;
    ufunc_T	*ufunc;
    పూర్ణాంకం		is_global = FALSE;
    char_u	*bracket_ప్రారంభం = ఖాళీ;
    char_u	*generic_func_name = name;

    ఒకవేళ (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    లేకపోతే ఒకవేళ (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    ఒకవేళ (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(శూన్య)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    లేకపోతే
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = కనుగొను_class_func(&arg);
	ఒకవేళ (ufunc != ఖాళీ)
	    తిరిగి ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      ఖాళీ, ఖాళీ, ఖాళీ, &ufunc);
	ఒకవేళ (ufunc != ఖాళీ)
	{
	    vim_free(fname);
	    తిరిగి ufunc;
	}
	ఒకవేళ (fname != ఖాళీ && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_ప్రారంభం = arg;
	    ఒకవేళ (skip_generic_func_type_వాదనలు(&arg) == FAIL)
	    {
		vim_free(fname);
		తిరిగి ఖాళీ;
	    }
	}
    }
    ఒకవేళ (fname == ఖాళీ)
    {
	semsg(_(e_invalid_వాదం_str), name);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (!చివర్లు_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	తిరిగి ఖాళీ;
    }

    ufunc = కనుగొను_func(fname, is_global);
    ఒకవేళ (ufunc == ఖాళీ)
    {
	char_u *p = untrans_function_name(fname);

	ఒకవేళ (p != ఖాళీ)
	    // Try again without making it script-local.
	    ufunc = కనుగొను_func(p, FALSE);

	ఒకవేళ (ufunc == ఖాళీ)
	{
	    bracket_ప్రారంభం = generic_func_కనుగొను_open_bracket(fname);
	    ఒకవేళ (bracket_ప్రారంభం != ఖాళీ)
	    {
		generic_func_name = fname;
		*bracket_ప్రారంభం = NUL;
		ufunc = కనుగొను_func(fname, FALSE);
		*bracket_ప్రారంభం = '<';
	    }
	}
    }

    ఒకవేళ (ufunc != ఖాళీ)
    {
	// handle generic functions
	ఒకవేళ (bracket_ప్రారంభం == ఖాళీ)
	    bracket_ప్రారంభం = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_ప్రారంభం);
    }

    vim_free(fname);

    ఒకవేళ (ufunc == ఖాళీ)
	semsg(_(e_cannot_కనుగొను_function_str), name);
    తిరిగి ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    శూన్య
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    ఒకవేళ (func_needs_compiling(ufunc, compile_type))
	(శూన్య)compile_def_function(ufunc, FALSE, compile_type, ఖాళీ);
    లేకపోతే
	smsg(_("Function %s%s%s does not need compiling"),
				cl != ఖాళీ ? cl->class_name.string : (char_u *)"",
				cl != ఖాళీ ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    స్థిర శూన్య
defcompile_funcs_లోపల_script(శూన్య)
{
    దీర్ఘ	todo = (దీర్ఘ)func_hashtab.ht_వాడిన;
    పూర్ణాంకం		మారింది = func_hashtab.ht_మారింది;
    hashitem_T	*hi;

    కొరకు (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    ఒకవేళ (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_స్థితి == UF_గమ్యం_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(శూన్య)compile_def_function(ufunc, FALSE, CT_NONE, ఖాళీ);

		ఒకవేళ (func_hashtab.ht_మారింది != మారింది)
		{
		    // a function has been added or removed, need to start
		    // over
		    todo = (దీర్ఘ)func_hashtab.ht_వాడిన;
		    మారింది = func_hashtab.ht_మారింది;
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
    శూన్య
ex_defcompile(exarg_T *eap)
{
    ఒకవేళ (*eap->arg != NUL)
    {
	typval_T tv;

	ఒకవేళ (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    ఒకవేళ (cl != ఖాళీ)
		defcompile_class(cl);
	}
	లేకపోతే
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = కనుగొను_func_by_name(eap->arg, &compile_type);
	    ఒకవేళ (ufunc != ఖాళీ)
		defcompile_function(ufunc, ఖాళీ);
	}
    }
    లేకపోతే
    {
	defcompile_funcs_లోపల_script();

	// compile all the class defined in the current script
	defcompile_classes_లోపల_script();
    }
}

/*
 * Return 5 if "p" starts with "<SID>" or "<SNR>" (ignoring case).
 * Return 2 if "p" starts with "s:".
 * Return 0 otherwise.
 */
    పూర్ణాంకం
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    ఒకవేళ (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	తిరిగి 5;
    ఒకవేళ (p[0] == 's' && p[1] == ':')
	తిరిగి 2;
    తిరిగి 0;
}

    పూర్ణాంకం
translated_function_exists(char_u *name, పూర్ణాంకం is_global)
{
    ఒకవేళ (builtin_function(name, -1))
	తిరిగి has_అంతర్గత_func(name);
    తిరిగి కనుగొను_func(name, is_global) != ఖాళీ;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    పూర్ణాంకం
has_varargs(ufunc_T *ufunc)
{
    తిరిగి ufunc->uf_varargs || ufunc->uf_va_name != ఖాళీ;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    పూర్ణాంకం
function_exists(char_u *name, పూర్ణాంకం no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    పూర్ణాంకం	    n = FALSE;
    పూర్ణాంకం	    flag;
    పూర్ణాంకం	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    ఒకవేళ (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    ఒకవేళ (p != ఖాళీ && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    తిరిగి n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    పూర్ణాంకం
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    పూర్ణాంకం		is_global = FALSE;
    ufunc_T	*fp = ఖాళీ;
    list_T	*వాదనలు;
    పూర్ణాంకం		is_def;
    అక్షరం	*tofree;

    ఒకవేళ (*name == NUL)
	తిరిగి FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    ఒకవేళ (p != ఖాళీ && *skipwhite(nm) == NUL)
	fp = కనుగొను_func(p, is_global);
    vim_free(p);
    ఒకవేళ (fp == ఖాళీ)
	తిరిగి FAIL;

    is_def = fp->uf_def_స్థితి != UF_NOT_COMPILED;
    dict_కూడు_string(d, "name", printable_func_name(fp));
    dict_కూడు_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_కూడు_bool(d, "available", TRUE);
    వాదనలు = list_alloc();
    ఒకవేళ (వాదనలు == ఖాళీ || dict_కూడు_list(d, "args", వాదనలు) == FAIL)
    {
	list_unref(వాదనలు);
	తిరిగి FAIL;
    }
    కొరకు (పూర్ణాంకం j = 0; j < fp->uf_వాదనలు.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	ఒకవేళ (arg == ఖాళీ || list_append_dict(వాదనలు, arg) == FAIL)
	{
	    dict_unref(arg);
	    తిరిగి FAIL;
	}
	dict_కూడు_string(arg, "name", FUNCARG(fp, j));
	ఒకవేళ (fp->uf_arg_types == ఖాళీ)
	    dict_కూడు_string(arg, "type", (char_u *)"any");
	లేకపోతే
	{
	    dict_కూడు_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ఒకవేళ (j >= fp->uf_వాదనలు.ga_len - fp->uf_def_వాదనలు.ga_len)
	    dict_కూడు_string(arg, "default", ((char_u **)fp->uf_def_వాదనలు.ga_దత్తాంశం)
			     [j - fp->uf_వాదనలు.ga_len + fp->uf_def_వాదనలు.ga_len]);
    }
    ఒకవేళ (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	ఒకవేళ (va == ఖాళీ || dict_కూడు_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    తిరిగి FAIL;
	}
	dict_కూడు_string(va, "name",
			 fp->uf_va_name == ఖాళీ ? (char_u *)"" : fp->uf_va_name);
	ఒకవేళ (fp->uf_va_type == ఖాళీ)
	    dict_కూడు_string(va, "type", (char_u *)"list<any>");
	లేకపోతే
	{
	    dict_కూడు_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    ఒకవేళ (is_def)
    {
	dict_కూడు_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    లేకపోతే
	dict_కూడు_string(d, "returns", (char_u *)"any");
    dict_కూడు_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_కూడు_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_కూడు_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_కూడు_bool(d, "closure", (fp->uf_flags & FC_సంవృతం) != 0);
    dict_కూడు_సంఖ్య(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_కూడు_సంఖ్య(d, "lnum", fp->uf_script_ctx.sc_lnum);
    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, పూర్ణాంకం check)
{
    char_u	*nm = name;
    char_u	*p;
    పూర్ణాంకం		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    ఒకవేళ (p != ఖాళీ && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	తిరిగి p;

    vim_free(p);
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, పూర్ణాంకం idx)
{
    స్థిర long_u	పూర్తి;
    స్థిర పూర్ణాంకం		మారింది;
    స్థిర hashitem_T	*hi;
    ufunc_T		*fp;

    ఒకవేళ (idx == 0)
    {
	పూర్తి = 0;
	hi = func_hashtab.ht_array;
	మారింది = func_hashtab.ht_మారింది;
    }
    ఒకవేళ (మారింది == func_hashtab.ht_మారింది && పూర్తి < func_hashtab.ht_వాడిన)
    {
	పూర్ణాంకం len;

	ఒకవేళ (పూర్తి++ > 0)
	    ++hi;
	అయితే (HASHITEM_ఖాళీలు(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	ఒకవేళ ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    తిరిగి (char_u *)"";

	ఒకవేళ (fp->uf_namelen + 4 >= IOSIZE)
	    తిరిగి fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	ఒకవేళ (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    ఒకవేళ (!has_varargs(fp) && fp->uf_వాదనలు.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	తిరిగి IObuff;
    }
    తిరిగి ఖాళీ;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, పూర్ణాంకం extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    ఒకవేళ (ufunc == ఖాళీ)
	తిరిగి ఖాళీ;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_స్థితి = UF_గమ్యం_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = ఖాళీ;
#నిర్వచితమైతే FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_లెక్క = ఖాళీ;
    ufunc->uf_tml_మొత్తం = ఖాళీ;
    ufunc->uf_tml_self = ఖాళీ;
#అయితే_ముగింపు

    ga_copy_అక్షరశ్రేణులు(&fp->uf_వాదనలు, &ufunc->uf_వాదనలు);
    ga_copy_అక్షరశ్రేణులు(&fp->uf_def_వాదనలు, &ufunc->uf_def_వాదనలు);

    ఒకవేళ (ufunc->uf_arg_types != ఖాళీ)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_వాదనలు.ga_len);
	ఒకవేళ (at != ఖాళీ)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     పరిమాణం(type_T *) * ufunc->uf_వాదనలు.ga_len);
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

    ఒకవేళ (ufunc->uf_va_name != ఖాళీ)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_ఖండం_లోతు = 0;
    ufunc->uf_ఖండం_ids = ఖాళీ;

    ga_copy_అక్షరశ్రేణులు(&fp->uf_పంక్తులు, &ufunc->uf_పంక్తులు);

#నిర్వచితమైతే FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    ఒకవేళ (fp->uf_profiling)
	func_do_profile(ufunc);
#అయితే_ముగింపు

    ufunc->uf_refcount = 1;

    తిరిగి ufunc;
}

/*
 * ":delfunction {name}"
 */
    శూన్య
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = ఖాళీ;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    పూర్ణాంకం		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     ఖాళీ, ఖాళీ, ఖాళీ);
    vim_free(fudi.fd_newkey);
    ఒకవేళ (name == ఖాళీ)
    {
	ఒకవేళ (fudi.fd_dict != ఖాళీ && !eap->skip)
	    emsg(_(e_funcref_required));
	తిరిగి;
    }
    ఒకవేళ (!చివర్లు_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	తిరిగి;
    }
    సమితి_nextcmd(eap, p);
    ఒకవేళ (eap->nextcmd != ఖాళీ)
	*p = NUL;

    ఒకవేళ (numbered_function(name) && fudi.fd_dict == ఖాళీ)
    {
	ఒకవేళ (!eap->skip)
	    semsg(_(e_invalid_వాదం_str), eap->arg);
	vim_free(name);
	తిరిగి;
    }
    ఒకవేళ (!eap->skip)
	fp = కనుగొను_func(name, is_global);
    vim_free(name);

    ఒకవేళ (!eap->skip)
    {
	ఒకవేళ (fp == ఖాళీ)
	{
	    ఒకవేళ (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    తిరిగి;
	}
	ఒకవేళ (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_లోపల_use), eap->arg);
	    తిరిగి;
	}
	ఒకవేళ (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    తిరిగి;
	}

	ఒకవేళ (fudi.fd_dict != ఖాళీ)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	లేకపోతే
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    ఒకవేళ (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		ఒకవేళ (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    లేకపోతే
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    శూన్య
func_unref(char_u *name)
{
    ufunc_T *fp = ఖాళీ;

    ఒకవేళ (name == ఖాళీ || !func_name_refcount(name))
	తిరిగి;
    fp = కనుగొను_func(name, FALSE);
    ఒకవేళ (fp == ఖాళీ && numbered_function(name))
    {
#నిర్వచితమైతే EXITFREE
	ఒకవేళ (!entered_free_all_mem)
#అయితే_ముగింపు
	    అంతర్గత_error("func_unref()");
    }
    func_ptr_unref(fp);
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 * Also when it becomes one and uf_partial points to the function.
 */
    శూన్య
func_ptr_unref(ufunc_T *fp)
{
    ఒకవేళ (fp != ఖాళీ && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != ఖాళీ
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	ఒకవేళ (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    శూన్య
func_ref(char_u *name)
{
    ufunc_T *fp;

    ఒకవేళ (name == ఖాళీ || !func_name_refcount(name))
	తిరిగి;
    fp = కనుగొను_func(name, FALSE);
    ఒకవేళ (fp != ఖాళీ)
	++fp->uf_refcount;
    లేకపోతే ఒకవేళ (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	అంతర్గత_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    శూన్య
func_ptr_ref(ufunc_T *fp)
{
    ఒకవేళ (fp != ఖాళీ)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    స్థిర పూర్ణాంకం
can_free_funccal(funccall_T *fc, పూర్ణాంకం copyID)
{
    తిరిగి (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    శూన్య
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    పూర్ణాంకం		returning = FALSE;
    evalarg_T	evalarg;

    ఒకవేళ (current_funccal == ఖాళీ)
    {
	emsg(_(e_return_not_inside_function));
	తిరిగి;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    ఒకవేళ (eap->skip)
	++emsg_skip;

    eap->nextcmd = ఖాళీ;
    ఒకవేళ ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	ఒకవేళ (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	లేకపోతే
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    లేకపోతే ఒకవేళ (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	ఒకవేళ (!aborting())
	    returning = do_return(eap, FALSE, TRUE, ఖాళీ);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    ఒకవేళ (returning)
	eap->nextcmd = ఖాళీ;
    లేకపోతే ఒకవేళ (eap->nextcmd == ఖాళీ)	    // no argument
	సమితి_nextcmd(eap, arg);

    ఒకవేళ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    స్థిర పూర్ణాంకం
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    పూర్ణాంకం		doesrange;
    typval_T	rettv;
    పూర్ణాంకం		failed = FALSE;

    lnum = eap->line1;
    కొరకు ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	ఒకవేళ (eap->addr_లెక్క > 0)
	{
	    ఒకవేళ (lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		ఆపు;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.నిలువు = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	ఒకవేళ (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    ఆపు;
	}
	ఒకవేళ (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	ఒకవేళ (handle_subscript(arg, ఖాళీ, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    ఆపు;
	}

	clear_tv(&rettv);
	ఒకవేళ (doesrange)
	    ఆపు;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	ఒకవేళ (aborting())
	    ఆపు;
    }
    తిరిగి failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    స్థిర పూర్ణాంకం
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_వాదనలు + 1];	// vars for arguments
    పూర్ణాంకం		partial_argc = 0;		// number of partial arguments
    పూర్ణాంకం		argcount = 0;			// number of arguments found
    పూర్ణాంకం		r;

    ఒకవేళ (current_funccal == ఖాళీ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	తిరిగి FAIL;
    }
    ఒకవేళ (partial != ఖాళీ)
    {
	ఒకవేళ (partial->pt_dict != ఖాళీ)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    తిరిగి FAIL;
	}
	ఒకవేళ (partial->pt_argc > 0)
	{
	    పూర్ణాంకం i;

	    partial_argc = partial->pt_argc;
	    కొరకు (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    పూర్ణాంకం is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    ఒకవేళ (r == OK)
    {
	ఒకవేళ (type != ఖాళీ)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_వాదం_types(type, argvars, argcount, ఖాళీ, name);
	}
	లేకపోతే ఒకవేళ (is_builtin)
	{
	    పూర్ణాంకం idx = కనుగొను_అంతర్గత_func(name);

	    ఒకవేళ (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    లేకపోతే ఒకవేళ (check_అంతర్గత_func(idx, argcount) == -1)
		r = FAIL;
	}
	లేకపోతే
	{
	    ufunc_T *ufunc = కనుగొను_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    ఒకవేళ (ufunc != ఖాళీ)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		ఒకవేళ (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    ఒకవేళ (r == FAIL)
    {
	అయితే (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	తిరిగి FAIL;
    }
    తిరిగి కూడు_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    పూర్ణాంకం
can_కూడు_defer(శూన్య)
{
    ఒకవేళ (!లోపల_def_function() && get_current_funccal() == ఖాళీ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
కూడు_defer(char_u *name, పూర్ణాంకం argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    పూర్ణాంకం		argcount = argcount_arg;
    defer_T	*dr;
    పూర్ణాంకం		ret = FAIL;

    ఒకవేళ (saved_name == ఖాళీ)
	వెళ్ళు theend;
    ఒకవేళ (లోపల_def_function())
    {
	ఒకవేళ (కూడు_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    లేకపోతే
    {
	ఒకవేళ (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, పరిమాణం(defer_T), 10);
	ఒకవేళ (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    వెళ్ళు theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_దత్తాంశం)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	అయితే (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    అయితే (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    తిరిగి ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    స్థిర శూన్య
handle_defer_one(funccall_T *funccal)
{
    పూర్ణాంకం	    idx;

    కొరకు (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_దత్తాంశం) + idx;

	ఒకవేళ (dr->dr_name == ఖాళీ)
	    // already being called, can happen if function does ":qa"
	    కొనసాగు;

	funcexe_T   funcexe;
	CLEAR_క్షేత్రం(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = ఖాళీ;

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
	కొరకు (పూర్ణాంకం i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    స్థిర శూన్య
invoke_funccall_defer(funccall_T *fc)
{
    ఒకవేళ (fc->fc_ectx != ఖాళీ)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    లేకపోతే
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    శూన్య
invoke_all_defer(శూన్య)
{
    కొరకు (funccall_T *fc = current_funccal; fc != ఖాళీ; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    కొరకు (funccal_నమోదు_T *fce = funccal_దొంతర; fce != ఖాళీ; fce = fce->తదుపరి)
	కొరకు (funccall_T *fc = fce->శిఖరం_funccal; fc != ఖాళీ; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    శూన్య
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    పూర్ణాంకం		len;
    పూర్ణాంకం		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = ఖాళీ;
    partial_T	*partial = ఖాళీ;
    evalarg_T	evalarg;
    type_T	*type = ఖాళీ;
    పూర్ణాంకం		దొరికింది_var = FALSE;
    పూర్ణాంకం		vim9script = లోపల_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    ఒకవేళ (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	ఒకవేళ (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	తిరిగి;
    }

    tofree = trans_function_name_ext(&arg, ఖాళీ, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : ఖాళీ, &ufunc);
    ఒకవేళ (fudi.fd_newkey != ఖాళీ)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_కీలకం_not_present_లోపల_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    ఒకవేళ (tofree == ఖాళీ)
	తిరిగి;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    ఒకవేళ (fudi.fd_dict != ఖాళీ)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (పూర్ణాంకం)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != ఖాళీ ? ఖాళీ : &partial,
				vim9script && type == ఖాళీ ? &type : ఖాళీ,
						     FALSE, FALSE, &దొరికింది_var);

    ఒకవేళ (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_వాదనలు(name, STRLEN(name), &arg);
	ఒకవేళ (name == ఖాళీ)
	    వెళ్ళు చివర;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    ఒకవేళ (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	వెళ్ళు చివర;
    }
    ఒకవేళ (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	వెళ్ళు చివర;
    }

    ఒకవేళ (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    లేకపోతే
    {
	funcexe_T funcexe;

	CLEAR_క్షేత్రం(funcexe);
	funcexe.fe_check_type = type;
	funcexe.fe_ufunc = ufunc;
	funcexe.fe_partial = partial;
	funcexe.fe_selfdict = fudi.fd_dict;
	funcexe.fe_firstline = eap->line1;
	funcexe.fe_lastline = eap->line2;
	funcexe.fe_దొరికింది_var = దొరికింది_var;
	funcexe.fe_evaluate = TRUE;
	failed = ex_call_inner(eap, name, &arg, startarg, &funcexe, &evalarg);
    }

    // When inside :try we need to check for following "| catch" or "| endtry".
    // Not when there was an error, but do check if an exception was thrown.
    ఒకవేళ ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	ఒకవేళ (!చివర్లు_excmd2(eap->arg, arg))
	{
	    ఒకవేళ (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	లేకపోతే
	    సమితి_nextcmd(eap, arg);
    }
    // Must be after using "arg", it may point into memory cleared here.
    clear_evalarg(&evalarg, eap);

చివర:
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
    పూర్ణాంకం
do_return(
    exarg_T	*eap,
    పూర్ణాంకం		reanimate,
    పూర్ణాంకం		is_cmd,
    శూన్య	*rettv)
{
    పూర్ణాంకం		idx;
    cstack_T	*cstack = eap->cstack;

    ఒకవేళ (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ఒకవేళ (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	ఒకవేళ (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	లేకపోతే
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    ఒకవేళ (reanimate)
		rettv = current_funccal->fc_rettv;

	    ఒకవేళ (rettv != ఖాళీ)
	    {
		// Store the value of the pending return.
		ఒకవేళ ((cstack->cs_rettv[idx] = alloc_tv()) != ఖాళీ)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		లేకపోతే
		    emsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం));
	    }
	    లేకపోతే
		cstack->cs_rettv[idx] = ఖాళీ;

	    ఒకవేళ (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_సంఖ్య;
		current_funccal->fc_rettv->vval.v_సంఖ్య = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    లేకపోతే
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	ఒకవేళ (!reanimate && rettv != ఖాళీ)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    ఒకవేళ (!is_cmd)
		vim_free(rettv);
	}
    }

    తిరిగి idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    శూన్య
discard_pending_return(శూన్య *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(శూన్య *rettv)
{
    char_u	*s = ఖాళీ;
    char_u	*tofree = ఖాళీ;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    ఒకవేళ (rettv != ఖాళీ)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    ఒకవేళ (s == ఖాళీ)
	s = (char_u *)"";
    లేకపోతే
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    ఒకవేళ (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    తిరిగి vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_పంక్తి(
    పూర్ణాంకం	    c వాడని,
    శూన్య    *cookie,
    పూర్ణాంకం	    indent వాడని,
    getline_opt_T options వాడని)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    ఒకవేళ (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_కనుగొను_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
	func_పంక్తి_చివర(cookie);
#అయితే_ముగింపు

    gap = &fp->uf_పంక్తులు;
    ఒకవేళ (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_లోపల_try())
	    || fcp->fc_returned)
	retval = ఖాళీ;
    లేకపోతే
    {
	// Skip NULL lines (continuation lines).
	అయితే (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_దత్తాంశం))[fcp->fc_linenr] == ఖాళీ)
	    ++fcp->fc_linenr;
	ఒకవేళ (fcp->fc_linenr >= gap->ga_len)
	    retval = ఖాళీ;
	లేకపోతే
	{
	    retval = vim_strsave(((char_u **)(gap->ga_దత్తాంశం))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#నిర్వచితమైతే FEAT_PROFILE
	    ఒకవేళ (do_profiling == PROF_YES)
		func_పంక్తి_ప్రారంభం(cookie, SOURCING_LNUM);
#అయితే_ముగింపు
	}
    }

    // Did we encounter a breakpoint?
    ఒకవేళ (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_కనుగొను_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    తిరిగి retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    పూర్ణాంకం
func_has_ended(శూన్య *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    తిరిగి (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_లోపల_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    పూర్ణాంకం
func_has_abort(
    శూన్య    *cookie)
{
    తిరిగి ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
}


/*
 * Turn "dict.Func" into a partial for "Func" bound to "dict".
 * Don't do this when "Func" is already a partial that was bound
 * explicitly (pt_auto is FALSE).
 * Changes "rettv" in-place.
 * Returns the updated "selfdict_in".
 */
    dict_T *
make_partial(dict_T *selfdict_లోపల, typval_T *rettv)
{
    char_u	*fname;
    ufunc_T	*fp = ఖాళీ;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_లోపల;

    ఒకవేళ (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != ఖాళీ
				     && rettv->vval.v_partial->pt_func != ఖాళీ)
	fp = rettv->vval.v_partial->pt_func;
    లేకపోతే
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == ఖాళీ ? ఖాళీ
					      : rettv->vval.v_partial->pt_name;
	ఒకవేళ (fname == ఖాళీ)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = ఖాళీ;
	}
	లేకపోతే
	{
	    char_u	*tofree = ఖాళీ;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = కనుగొను_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    ఒకవేళ (fp != ఖాళీ && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	ఒకవేళ (pt != ఖాళీ)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = ఖాళీ;
	    ఒకవేళ (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    లేకపోతే
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		పూర్ణాంకం		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		ఒకవేళ (ret_pt->pt_name != ఖాళీ)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		లేకపోతే
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		ఒకవేళ (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    ఒకవేళ (pt->pt_argv == ఖాళీ)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    లేకపోతే
		    {
			pt->pt_argc = ret_pt->pt_argc;
			కొరకు (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    తిరిగి selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(శూన్య *cookie)
{
    తిరిగి ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(శూన్య *cookie)
{
    తిరిగి &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    పూర్ణాంకం *
func_dbg_tick(శూన్య *cookie)
{
    తిరిగి &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    పూర్ణాంకం
func_level(శూన్య *cookie)
{
    తిరిగి ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    పూర్ణాంకం
current_func_returned(శూన్య)
{
    తిరిగి current_funccal->fc_returned;
}

    పూర్ణాంకం
free_unref_funccal(పూర్ణాంకం copyID, పూర్ణాంకం testing)
{
    పూర్ణాంకం		did_free = FALSE;
    పూర్ణాంకం		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    కొరకు (pfc = &మునుపటి_funccal; *pfc != ఖాళీ; )
    {
	ఒకవేళ (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	లేకపోతే
	    pfc = &(*pfc)->fc_caller;
    }
    ఒకవేళ (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(శూన్య)garbage_collect(testing);

    తిరిగి did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    స్థిర funccall_T *
get_funccal(శూన్య)
{
    పూర్ణాంకం		i;
    funccall_T	*funccal;
    funccall_T	*తాత్కాలిక_funccal;

    funccal = current_funccal;
    ఒకవేళ (debug_backtrace_level > 0)
    {
	కొరకు (i = 0; i < debug_backtrace_level; i++)
	{
	    తాత్కాలిక_funccal = funccal->fc_caller;
	    ఒకవేళ (తాత్కాలిక_funccal)
		funccal = తాత్కాలిక_funccal;
	    లేకపోతే
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    తిరిగి funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    స్థిర funccall_T *
get_funccal_for_vars(శూన్య)
{
    funccall_T	*funccal = ఖాళీ;

    ఒకవేళ (current_funccal == ఖాళీ)
	తిరిగి ఖాళీ;
    funccal = get_funccal();
    ఒకవేళ (funccal == ఖాళీ || funccal->fc_l_vars.dv_refcount == 0)
	తిరిగి ఖాళీ;
    తిరిగి funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(శూన్య)
{
    funccall_T	*funccal = get_funccal_for_vars();

    తిరిగి funccal == ఖాళీ ? ఖాళీ : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(శూన్య)
{
    funccall_T	*funccal = get_funccal_for_vars();

    తిరిగి funccal == ఖాళీ ? ఖాళీ : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_వాదనలు_ht(శూన్య)
{
    funccall_T	*funccal = get_funccal_for_vars();

    తిరిగి funccal == ఖాళీ ? ఖాళీ : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_వాదనలు_var(శూన్య)
{
    funccall_T	*funccal = get_funccal_for_vars();

    తిరిగి funccal == ఖాళీ ? ఖాళీ : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    శూన్య
list_func_vars(పూర్ణాంకం *మొదటి)
{
    ఒకవేళ (current_funccal != ఖాళీ && current_funccal->fc_l_vars.dv_refcount > 0)
	list_hashtable_vars(&current_funccal->fc_l_vars.dv_hashtab,
							   "l:", FALSE, మొదటి);
}

/*
 * If "ht" is the hashtable for local variables in the current funccal, return
 * the dict that contains it.
 * Otherwise return NULL.
 */
    dict_T *
get_current_funccal_dict(hashtab_T *ht)
{
    ఒకవేళ (current_funccal != ఖాళీ
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	తిరిగి &current_funccal->fc_l_vars;
    తిరిగి ఖాళీ;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
కనుగొను_hi_లోపల_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = ఖాళీ;
    char_u	*varname;

    ఒకవేళ (current_funccal == ఖాళీ || current_funccal->fc_func->uf_scoped == ఖాళీ)
	తిరిగి ఖాళీ;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    అయితే (current_funccal != ఖాళీ)
    {
	ht = కనుగొను_var_ht(name, &varname);
	ఒకవేళ (ht != ఖాళీ && *varname != NUL)
	{
	    hi = hash_కనుగొను(ht, varname);
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		*pht = ht;
		ఆపు;
	    }
	}
	ఒకవేళ (current_funccal == current_funccal->fc_func->uf_scoped)
	    ఆపు;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    తిరిగి hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
కనుగొను_var_లోపల_scoped_ht(char_u *name, పూర్ణాంకం no_autoload)
{
    dictitem_T	*v = ఖాళీ;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    ఒకవేళ (current_funccal == ఖాళీ || current_funccal->fc_func->uf_scoped == ఖాళీ)
	తిరిగి ఖాళీ;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    అయితే (current_funccal)
    {
	ht = కనుగొను_var_ht(name, &varname);
	ఒకవేళ (ht != ఖాళీ && *varname != NUL)
	{
	    v = కనుగొను_var_లోపల_ht(ht, *name, varname, no_autoload);
	    ఒకవేళ (v != ఖాళీ)
		ఆపు;
	}
	ఒకవేళ (current_funccal == current_funccal->fc_func->uf_scoped)
	    ఆపు;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    తిరిగి v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_మునుపటి_funccal(పూర్ణాంకం copyID)
{
    funccall_T	*fc;

    కొరకు (fc = మునుపటి_funccal; fc != ఖాళీ; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	ఒకవేళ (సమితి_ref_లోపల_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, ఖాళీ, ఖాళీ)
		|| సమితి_ref_లోపల_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, ఖాళీ, ఖాళీ)
		|| సమితి_ref_లోపల_list_వస్తువులు(&fc->fc_l_varlist, copyID + 1, ఖాళీ, ఖాళీ))
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

    స్థిర పూర్ణాంకం
సమితి_ref_లోపల_funccal(funccall_T *fc, పూర్ణాంకం copyID)
{
    ఒకవేళ (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	ఒకవేళ (సమితి_ref_లోపల_ht(&fc->fc_l_vars.dv_hashtab, copyID, ఖాళీ, ఖాళీ)
		|| సమితి_ref_లోపల_ht(&fc->fc_l_avars.dv_hashtab, copyID, ఖాళీ, ఖాళీ)
		|| సమితి_ref_లోపల_list_వస్తువులు(&fc->fc_l_varlist, copyID, ఖాళీ, ఖాళీ)
		|| సమితి_ref_లోపల_func(ఖాళీ, fc->fc_func, copyID))
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_call_దొంతర(పూర్ణాంకం copyID)
{
    funccall_T		*fc;
    funccal_నమోదు_T	*నమోదు_2;

    కొరకు (fc = current_funccal; fc != ఖాళీ; fc = fc->fc_caller)
	ఒకవేళ (సమితి_ref_లోపల_funccal(fc, copyID))
	    తిరిగి TRUE;

    // Also go through the funccal_stack.
    కొరకు (నమోదు_2 = funccal_దొంతర; నమోదు_2 != ఖాళీ; నమోదు_2 = నమోదు_2->తదుపరి)
	కొరకు (fc = నమోదు_2->శిఖరం_funccal; fc != ఖాళీ; fc = fc->fc_caller)
	    ఒకవేళ (సమితి_ref_లోపల_funccal(fc, copyID))
		తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_functions(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		todo;
    hashitem_T	*hi = ఖాళీ;
    ufunc_T	*fp;

    todo = (పూర్ణాంకం)func_hashtab.ht_వాడిన;
    కొరకు (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ఒకవేళ (!func_name_refcount(fp->uf_name)
					  && సమితి_ref_లోపల_func(ఖాళీ, fp, copyID))
		తిరిగి TRUE;
	}
    }
    తిరిగి FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_func_వాదనలు(పూర్ణాంకం copyID)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < funcargs.ga_len; ++i)
	ఒకవేళ (సమితి_ref_లోపల_అంశం(((typval_T **)funcargs.ga_దత్తాంశం)[i],
						copyID, ఖాళీ, ఖాళీ, ఖాళీ))
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_func(char_u *name, ufunc_T *fp_లోపల, పూర్ణాంకం copyID)
{
    ufunc_T	*fp = fp_లోపల;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ఖాళీ;
    char_u	*fname;
    పూర్ణాంకం		abort = FALSE;

    ఒకవేళ (name == ఖాళీ && fp_లోపల == ఖాళీ)
	తిరిగి FALSE;

    ఒకవేళ (fp_లోపల == ఖాళీ)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = కనుగొను_func(fname, FALSE);
    }
    ఒకవేళ (fp != ఖాళీ)
    {
	కొరకు (fc = fp->uf_scoped; fc != ఖాళీ; fc = fc->fc_func->uf_scoped)
	    abort = abort || సమితి_ref_లోపల_funccal(fc, copyID);
    }

    vim_free(tofree);
    తిరిగి abort;
}

#అయితే_ముగింపు // FEAT_EVAL
