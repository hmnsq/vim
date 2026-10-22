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

#include "vim.h"

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
static hashtab_T	func_hashtab;

// Used by get_func_tv()
static garray_T funcargs = GA_EMPTY;

// pointer to funccal for currently active function
static funccall_T *current_funccal = NULL;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
static funccall_T *previous_funccal = NULL;

static void funccal_unref(funccall_T *fc, ufunc_T *fp, puur_naa_nka force);
static void func_clear(ufunc_T *fp, puur_naa_nka force);
static puur_naa_nka func_free(ufunc_T *fp, puur_naa_nka force);
static char_u *untrans_function_name(char_u *name);
static void handle_defer_one(funccall_T *funccal);
static char_u *trans_function_name_ext(char_u **pp, puur_naa_nka *is_global, puur_naa_nka skip, puur_naa_nka flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    void
func_init(void)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(void)
{
    ꦮꦭꦶ &func_hashtab;
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
    static char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	puur_naa_nka	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	puur_naa_nka	    is_vararg,
	puur_naa_nka	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = NULL;
    puur_naa_nka		is_underscore = FALSE;

    ꦱꦼꦢꦼꦭ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    ꦪꦼꦤ꧀ (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == NULL
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	ꦪꦼꦤ꧀ (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	ꦮꦭꦶ arg;
    }

    // Extra checks in Vim9 script.
    ꦪꦼꦤ꧀ (!skip && argtypes != NULL)
    {
	puur_naa_nka c = *p;
	*p = NUL;
	puur_naa_nka r = check_reserved_name(arg, FALSE);
	*p = c;
	ꦪꦼꦤ꧀ (r == FAIL)
	    ꦮꦭꦶ arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	ꦪꦼꦤ꧀ (check_defined(arg, p - arg,
				   evalarg == NULL ? NULL : evalarg->eval_cctx,
			       eap == NULL ? NULL : eap->cstack, TRUE) == FAIL)
	    ꦮꦭꦶ arg;
    }

    ꦪꦼꦤ꧀ (newargs != NULL && ga_grow(newargs, 1) == FAIL)
	ꦮꦭꦶ arg;
    ꦪꦼꦤ꧀ (newargs != NULL)
    {
	puur_naa_nka	c;
	puur_naa_nka	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	ꦪꦼꦤ꧀ (arg_copy == NULL)
	{
	    *p = c;
	    ꦮꦭꦶ arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	ꦪꦼꦤ꧀ (argtypes == NULL || !is_underscore)
	    // Check for duplicate argument name.
	    ꦏꦁꦒꦺꦴ (i = 0; i < newargs->ga_len; ++i)
		ꦪꦼꦤ꧀ (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    ꦮꦭꦶ arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    ꦪꦼꦤ꧀ (argtypes != NULL && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != NULL && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = NULL;

	ꦪꦼꦤ꧀ (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == NULL ? arg : arg_copy);
	    p = skipwhite(p);
	}
	ꦪꦼꦤ꧀ (*p == ':')
	{
	    ++p;
	    ꦪꦼꦤ꧀ (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		ꦮꦭꦶ arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    ꦪꦼꦤ꧀ (!skip)
		type = vim_strnsave(type, p - type);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == NULL ? arg : arg_copy);
	    ꦮꦭꦶ arg;
	}
	ꦪꦼꦤ꧀ (!skip)
	{
	    ꦪꦼꦤ꧀ (type == NULL && types_optional)
	    {
		// lambda arguments default to "any" type
		ꦪꦼꦤ꧀ (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		ꦭꦶꦪ
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    ꦮꦭꦶ p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    static char_u *
get_function_line(
	exarg_T		*eap,
	garray_T	*lines_to_free,
	puur_naa_nka		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    ꦪꦼꦤ꧀ (eap->ea_getline == NULL)
	theline = getcmdline(':', 0L, indent, 0);
    ꦭꦶꦪ
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    ꦪꦼꦤ꧀ (theline != NULL)
    {
	ꦪꦼꦤ꧀ (lines_to_free->ga_len > 0
		&& eap->cmdlinep != NULL
		&& *eap->cmdlinep == ((char_u **)lines_to_free->ga_data)
						   [lines_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(void)ga_add_string(lines_to_free, theline);
    }

    ꦮꦭꦶ theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    static puur_naa_nka
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    puur_naa_nka		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    puur_naa_nka		*varargs,
    garray_T	*default_args,
    puur_naa_nka		skip,
    exarg_T	*eap,		// can be NULL
    puur_naa_nka		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*lines_to_free)
{
    puur_naa_nka		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    puur_naa_nka		c;
    puur_naa_nka		any_default = FALSE;
    char_u	*whitep = *argp;
    puur_naa_nka		need_expr = FALSE;

    ꦪꦼꦤ꧀ (newargs != NULL)
	ga_init2(newargs, parimaa_na(char_u *), 3);
    ꦪꦼꦤ꧀ (argtypes != NULL)
	ga_init2(argtypes, parimaa_na(char_u *), 3);
    ꦪꦼꦤ꧀ (arg_objm != NULL)
	ga_init2(arg_objm, parimaa_na(int8_T), 3);
    ꦪꦼꦤ꧀ (!skip && default_args != NULL)
	ga_init2(default_args, parimaa_na(char_u *), 3);

    ꦪꦼꦤ꧀ (varargs != NULL)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    ꦱꦼꦢꦼꦭ (*p != endchar)
    {
	ꦱꦼꦢꦼꦭ (eap != NULL && eap->ea_getline != NULL
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_line(eap, lines_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    ꦪꦼꦤ꧀ (theline == NULL)
		bha_nga;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	ꦪꦼꦤ꧀ (mustend && *p != endchar)
	{
	    ꦪꦼꦤ꧀ (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    goto err_ret;
	}
	ꦪꦼꦤ꧀ (*p == endchar && !need_expr)
	    bha_nga;

	ꦪꦼꦤ꧀ (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    ꦪꦼꦤ꧀ (varargs != NULL)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    ꦪꦼꦤ꧀ (argtypes != NULL)
	    {
		// ...name: list<type>
		ꦪꦼꦤ꧀ (!eval_isnamec1(*p))
		{
		    ꦪꦼꦤ꧀ (!skip)
			emsg(_(e_missing_name_after_dots));
		    goto err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		ꦪꦼꦤ꧀ (p == arg)
		    bha_nga;
		ꦪꦼꦤ꧀ (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    bha_nga;
		}
	    }
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    ꦱꦼꦢꦼꦭ (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    ꦪꦼꦤ꧀ (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		bha_nga;
	    }

	    ꦪꦼꦤ꧀ (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		ꦪꦼꦤ꧀ (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_value_must_be_vnone_str), p);
		    goto err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		ꦪꦼꦤ꧀ (ga_grow(default_args, 1) == FAIL)
		    goto err_ret;

		char_u *expr = vim_strnsave((char_u *)"v:none", 6);
		ꦪꦼꦤ꧀ (expr == NULL)
		    goto err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = expr;
		default_args->ga_len++;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		goto err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    ꦪꦼꦤ꧀ (newargs != NULL && ga_grow(newargs, 1) == FAIL)
		ꦮꦭꦶ FAIL;
	    ꦪꦼꦤ꧀ (newargs != NULL)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		ꦪꦼꦤ꧀ (argtypes != NULL && ga_grow(argtypes, 1) == OK
			    && arg_objm != NULL && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    ꦪꦼꦤ꧀ (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			puur_naa_nka len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			ꦪꦼꦤ꧀ (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			ꦪꦼꦤ꧀ (assignment != NULL)
			{
			    c = *argend;
			    *argend = NUL;
			    ꦪꦼꦤ꧀ (any_default)
				vim_snprintf((var_na *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    ꦭꦶꦪ
				vim_snprintf((var_na *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    ꦪꦼꦤ꧀ (*p == ',')
		++p;
	}
	ꦭꦶꦪ
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    ꦪꦼꦤ꧀ (p == arg)
		bha_nga;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    ꦪꦼꦤ꧀ (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != NULL)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *expr = p;
		ꦪꦼꦤ꧀ (eval1(&p, &rettv, NULL) != FAIL)
		{
		    ꦪꦼꦤ꧀ (!skip)
		    {
			ꦪꦼꦤ꧀ (ga_grow(default_args, 1) == FAIL)
			    goto err_ret;

			ꦪꦼꦤ꧀ (need_expr)
			    need_expr = FALSE;
			// trim trailing whitespace
			ꦱꦼꦢꦼꦭ (p > expr && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			expr = vim_strsave(expr);
			ꦪꦼꦤ꧀ (expr == NULL)
			{
			    *p = c;
			    goto err_ret;
			}
			((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = expr;
			default_args->ga_len++;
			*p = c;
		    }
		}
		ꦭꦶꦪ
		{
		    mustend = TRUE;
		    ꦪꦼꦤ꧀ (*skipwhite(p) == NUL)
			need_expr = TRUE;
		}
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		goto err_ret;
	    }

	    ꦪꦼꦤ꧀ (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		ꦪꦼꦤ꧀ (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    goto err_ret;
		}
		p = skipwhite(p);
	    }
	    ꦪꦼꦤ꧀ (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		ꦪꦼꦤ꧀ (!skip && argtypes != NULL
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    goto err_ret;
		}
	    }
	    ꦭꦶꦪ
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    ꦪꦼꦤ꧀ (*p != endchar)
	goto err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    ꦮꦭꦶ OK;

err_ret:
    ꦪꦼꦤ꧀ (newargs != NULL)
	ga_clear_strings(newargs);
    ꦪꦼꦤ꧀ (!skip && default_args != NULL)
	ga_clear_strings(default_args);
    ꦮꦭꦶ FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    static puur_naa_nka
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    puur_naa_nka		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    puur_naa_nka		obj_member_count,
    cctx_T	*cctx)
{
    puur_naa_nka len = 0;

    ga_init2(&fp->uf_type_list, parimaa_na(type_T *), 10);
    ꦪꦼꦤ꧀ (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	ꦪꦼꦤ꧀ (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	ꦪꦼꦤ꧀ (fp->uf_arg_types != NULL)
	{
	    puur_naa_nka	i;
	    type_T	*type;

	    ꦏꦁꦒꦺꦴ (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		ꦪꦼꦤ꧀ (p == NULL)
		    // will get the type from the default value
		    type = &t_unknown;
		ꦭꦶꦪ
		{
		    ꦪꦼꦤ꧀ (arg_objm != NULL && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			ꦏꦁꦒꦺꦴ (puur_naa_nka om = 0; om < obj_member_count; ++om)
			{
			    ꦪꦼꦤ꧀ (obj_members != NULL
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				bha_nga;
			    }
			}
		    }
		    ꦭꦶꦪ
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		ꦪꦼꦤ꧀ (type == NULL || !valid_declaration_type(type))
		    ꦮꦭꦶ FAIL;
		fp->uf_arg_types[i] = type;
		ꦪꦼꦤ꧀ (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    ꦪꦼꦤ꧀ (obj_members != NULL && *name == '_')
			// protected object method
			name++;

		    ꦪꦼꦤ꧀ (var_wrong_func_name(name, TRUE))
			ꦮꦭꦶ FAIL;
		}
	    }
	}
    }

    ꦪꦼꦤ꧀ (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = NULL;
	p = ((char_u **)argtypes->ga_data)[len];
	ꦪꦼꦤ꧀ (p == NULL)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	ꦭꦶꦪ
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    ꦪꦼꦤ꧀ (fp->uf_va_type != NULL && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		ꦮꦭꦶ FAIL;
	    }
	}
	ꦪꦼꦤ꧀ (fp->uf_va_type == NULL)
	    ꦮꦭꦶ FAIL;
    }

    ꦮꦭꦶ OK;
}

    static puur_naa_nka
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    ꦪꦼꦤ꧀ (ret_type == NULL)
	fp->uf_ret_type = &t_void;
    ꦭꦶꦪ
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	ꦪꦼꦤ꧀ (fp->uf_ret_type == NULL)
	{
	    fp->uf_ret_type = &t_void;
	    ꦮꦭꦶ FAIL;
	}
    }
    ꦮꦭꦶ OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    static puur_naa_nka
register_closure(ufunc_T *fp)
{
    ꦪꦼꦤ꧀ (fp->uf_scoped == current_funccal)
	// no change
	ꦮꦭꦶ OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    ꦪꦼꦤ꧀ (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	ꦮꦭꦶ FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    ꦮꦭꦶ OK;
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

    ꦪꦼꦤ꧀ (name[0] != K_SPECIAL)
	ꦮꦭꦶ name;
    len = STRLEN(name);
    ꦪꦼꦤ꧀ (len + 3 > bufsize)
	ꦮꦭꦶ name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    ꦮꦭꦶ buf;
}

static char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(void)
{
    static puur_naa_nka	lambda_no = 0;
    puur_naa_nka		n;
    string_T	ret;

    n = vim_snprintf((var_na *)lambda_name, parimaa_na(lambda_name), "<lambda>%d", ++lambda_no);
    ꦪꦼꦤ꧀ (n < 1)
	ret.length = 0;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (n >= (puur_naa_nka)parimaa_na(lambda_name))
	ret.length = parimaa_na(lambda_name) - 1;
    ꦭꦶꦪ
	ret.length = (size_t)n;

    ret.string = lambda_name;
    ꦮꦭꦶ ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    static ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < parimaa_na(ufunc_T) ? parimaa_na(ufunc_T) : len);
    ꦪꦼꦤ꧀ (fp != NULL)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((void *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	ꦪꦼꦤ꧀ (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    ꦪꦼꦤ꧀ (fp->uf_name_exp != NULL)
		vim_snprintf((var_na *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    ꦮꦭꦶ fp;
}

#ꦪꦼꦤ꧀ defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
register_cfunc(cfunc_T cb, cfunc_free_T cb_free, void *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.length);
    ꦪꦼꦤ꧀ (fp == NULL)
	ꦮꦭꦶ NULL;

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

    ꦮꦭꦶ name.string;
}
#endif

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    static char_u *
skip_arrow(
	char_u	*start,
	puur_naa_nka	equal_arrow,
	char_u	**ret_type,
	puur_naa_nka	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    ꦪꦼꦤ꧀ (equal_arrow)
    {
	ꦪꦼꦤ꧀ (*s == ':')
	{
	    ꦪꦼꦤ꧀ (white_error != NULL && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		ꦮꦭꦶ NULL;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    ꦪꦼꦤ꧀ (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		ꦮꦭꦶ NULL;
	    }
	}
	bef = s;
	s = skipwhite(s);
	ꦪꦼꦤ꧀ (*s != '=')
	    ꦮꦭꦶ NULL;
	++s;
    }
    ꦪꦼꦤ꧀ (*s != '>')
	ꦮꦭꦶ NULL;
    ꦪꦼꦤ꧀ (white_error != NULL && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	ꦮꦭꦶ NULL;
    }
    ꦮꦭꦶ skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    static puur_naa_nka
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    ꦪꦼꦤ꧀ (checkforcmd(&p, "function", 2))
    {
	ꦪꦼꦤ꧀ (*p == '(')
	    ꦮꦭꦶ FALSE;
	*cmd = p;
	ꦮꦭꦶ TRUE;
    }
    ꦮꦭꦶ FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    static void
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    ꦪꦼꦤ꧀ (cstack == NULL || cstack->cs_idx < 0)
	ꦮꦭꦶ;

    puur_naa_nka	    count = cstack->cs_idx + 1;
    puur_naa_nka	    i;

    fp->uf_block_ids = ALLOC_MULT(puur_naa_nka, count);
    ꦪꦼꦤ꧀ (fp->uf_block_ids != NULL)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		parimaa_na(puur_naa_nka) * count);
	fp->uf_block_depth = count;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    ꦏꦁꦒꦺꦴ (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    static char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    ꦱꦼꦢꦼꦭ (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	ꦪꦼꦤ꧀ (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    bha_nga;
	}
	ꦭꦶꦪ
	    MB_PTR_ADV(p);
    }

    ꦮꦭꦶ p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    static char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    ꦱꦼꦢꦼꦭ (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	ꦪꦼꦤ꧀ (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*p == '"')
	{
	    p++;    // skip the ending double quote
	    bha_nga;
	}
	ꦭꦶꦪ
	    MB_PTR_ADV(p);
    }

    ꦮꦭꦶ p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    static char_u *
find_start_of_vim9_comment(char_u *line)
{
    char_u	*p = line;

    ꦱꦼꦢꦼꦭ (*p != NUL)
    {
	ꦪꦼꦤ꧀ (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (*p == '#')
		// Found the start of a Vim9 comment
		bha_nga;
	    MB_PTR_ADV(p);
	}
    }

    ꦮꦭꦶ p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    static puur_naa_nka
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *line_arg_in,
	garray_T    *lines_to_free)
{
    linenr_T	sourcing_lnum_top = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    puur_naa_nka		saved_wait_return = need_wait_return;
    char_u	*line_arg = line_arg_in;
    puur_naa_nka		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    var_na	nesting_def[MAX_FUNC_NESTING];
    var_na	nesting_inline[MAX_FUNC_NESTING];
    puur_naa_nka		nesting = 0;
    getline_opt_T getline_options;
    puur_naa_nka		indent = 2;
    char_u	*skip_until = NULL;
    puur_naa_nka		ret = FAIL;
    puur_naa_nka		is_heredoc = FALSE;
    puur_naa_nka		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = NULL;
    size_t	heredoc_trimmedlen = 0;
    puur_naa_nka		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    ꦪꦼꦤ꧀ (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	ꦪꦼꦤ꧀ (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    goto theend;
	ꦱꦼꦢꦼꦭ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = NULL;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    ꦏꦁꦒꦺꦴ (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	ꦪꦼꦤ꧀ (KeyTyped)
	{
	    msg_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	ꦪꦼꦤ꧀ (line_arg != NULL)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = line_arg;
	    p = vim_strchr(theline, '\n');
	    ꦪꦼꦤ꧀ (p == NULL)
		line_arg += STRLEN(line_arg);
	    ꦭꦶꦪ
	    {
		*p = NUL;
		line_arg = p + 1;
	    }
	}
	ꦭꦶꦪ
	{
	    theline = get_function_line(eap, lines_to_free, indent,
							      getline_options);
	}
	ꦪꦼꦤ꧀ (KeyTyped)
	    lines_left = Rows - 1;
	ꦪꦼꦤ꧀ (theline == NULL)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_top;
	    ꦪꦼꦤ꧀ (skip_until != NULL)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    ꦭꦶꦪ
		emsg(_(e_missing_endfunction));
	    goto theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	ꦪꦼꦤ꧀ (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	ꦭꦶꦪ
	    sourcing_lnum_off = 0;

	ꦪꦼꦤ꧀ (skip_until != NULL)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    ꦪꦼꦤ꧀ (heredoc_trimmed == NULL
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		ꦪꦼꦤ꧀ (heredoc_trimmed == NULL)
		    p = theline;
		ꦭꦶꦪ ꦪꦼꦤ꧀ (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		ꦭꦶꦪ
		    p = theline + heredoc_trimmedlen;
		ꦪꦼꦤ꧀ (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    ꦪꦼꦤ꧀ (heredoc_concat_len > 0)
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
	ꦭꦶꦪ
	{
	    puur_naa_nka	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    ꦏꦁꦒꦺꦴ (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		ꦪꦼꦤ꧀ (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    ꦪꦼꦤ꧀ (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		ꦪꦼꦤ꧀ (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		ꦪꦼꦤ꧀ (nesting-- == 0)
		{
		    char_u *nextcmd = NULL;

		    ꦪꦼꦤ꧀ (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (line_arg != NULL && *skipwhite(line_arg) != NUL)
			nextcmd = line_arg;
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_top
							+ newlines->ga_len + 1;
			ꦪꦼꦤ꧀ (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			ꦭꦶꦪ
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    ꦪꦼꦤ꧀ (nextcmd != NULL && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			ꦪꦼꦤ꧀ (lines_to_free->ga_len > 0
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
		    bha_nga;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (nesting_def[nesting])
	    {
		ꦪꦼꦤ꧀ (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    ꦪꦼꦤ꧀ (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    ꦪꦼꦤ꧀ (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		ꦪꦼꦤ꧀ (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, NULL, TRUE, 0));
		ꦪꦼꦤ꧀ (vim9_function && *p == '<')
		{
		    // skip generic function
		    ꦪꦼꦤ꧀ (skip_generic_func_type_args(&p) == FAIL)
			goto theend;
		}
		ꦪꦼꦤ꧀ (*skipwhite(p) == '(')
		{
		    ꦪꦼꦤ꧀ (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    ꦭꦶꦪ
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    ꦪꦼꦤ꧀ (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		ꦪꦼꦤ꧀ (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		ꦭꦶꦪ
		    end = p + STRLEN(p) - 1;

		ꦱꦼꦢꦼꦭ (end > p && VIM_ISWHITE(*end))
		    --end;
		ꦪꦼꦤ꧀ (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    puur_naa_nka	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    ꦱꦼꦢꦼꦭ (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    ꦪꦼꦤ꧀ (!is_block)
			is_block = find_cmd_block_start(p) != NULL;

		    ꦪꦼꦤ꧀ (is_block)
		    {
			ꦪꦼꦤ꧀ (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			ꦭꦶꦪ
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
	    ꦪꦼꦤ꧀ (!vim9_function || colon)
		p = skip_range(p, FALSE, NULL);
	    char_u *tp = p;
	    ꦪꦼꦤ꧀ (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    ꦭꦶꦪ
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    ꦪꦼꦤ꧀ (arg[0] == '<' && arg[1] =='<'
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
		ꦪꦼꦤ꧀ (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    ꦪꦼꦤ꧀ (heredoc_trimmed == NULL)
			heredoc_trimmedlen = 0;
		}
		ꦪꦼꦤ꧀ (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		ꦭꦶꦪ
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		ꦪꦼꦤ꧀ (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    ꦪꦼꦤ꧀ (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		ꦪꦼꦤ꧀ (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    puur_naa_nka		save_sc_version = current_sctx.sc_version;
		    puur_naa_nka		var_count = 0;
		    puur_naa_nka		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_count, &semicolon,
									 TRUE);
		    ꦪꦼꦤ꧀ (arg != NULL)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    ꦪꦼꦤ꧀ (arg != NULL && STRNCMP(arg, "=<<", 3) == 0)
		    {
			puur_naa_nka has_trim = FALSE;

			p = skipwhite(arg + 3);
			ꦱꦼꦢꦼꦭ (TRUE)
			{
			    ꦪꦼꦤ꧀ (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				continue;
			    }
			    ꦪꦼꦤ꧀ (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				continue;
			    }
			    bha_nga;
			}
			ꦪꦼꦤ꧀ (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    ꦪꦼꦤ꧀ (heredoc_trimmed == NULL)
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
	ꦪꦼꦤ꧀ (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    goto theend;

	ꦪꦼꦤ꧀ (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	ꦭꦶꦪ
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	ꦪꦼꦤ꧀ (p == NULL)
	    goto theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	ꦱꦼꦢꦼꦭ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = NULL;

	// Check for end of eap->arg.
	ꦪꦼꦤ꧀ (line_arg != NULL && *line_arg == NUL)
	    line_arg = NULL;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    ꦪꦼꦤ꧀ (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    ꦮꦭꦶ ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    static puur_naa_nka
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	puur_naa_nka	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    puur_naa_nka		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = NULL;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = NULL;
    puur_naa_nka		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    puur_naa_nka		lnum_save = -1;
    linenr_T	sourcing_lnum_top = SOURCING_LNUM;
    char_u	*line_arg = NULL;

    *arg = skipwhite(*arg + 1);
    ꦪꦼꦤ꧀ (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	ꦮꦭꦶ FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    ꦪꦼꦤ꧀ (**arg == '\n')
	line_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    ꦪꦼꦤ꧀ (evalarg->eval_cctx != NULL)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    ꦭꦶꦪ
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, parimaa_na(char_u *), 10);
    ꦪꦼꦤ꧀ (get_function_body(&eap, &newlines, line_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	goto erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_count += newlines.ga_len;
    ꦪꦼꦤ꧀ (gap->ga_itemsize > 0)
    {
	puur_naa_nka	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	ꦏꦁꦒꦺꦴ (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    ꦪꦼꦤ꧀ (p == NULL)
		// comment line in the lambda body
		continue;

	    p = skipwhite(p);

	    ꦪꦼꦤ꧀ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		goto erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    ꦪꦼꦤ꧀ (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    ꦭꦶꦪ
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    ꦪꦼꦤ꧀ (pnl != NULL)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	ꦪꦼꦤ꧀ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    goto erret;
	ꦪꦼꦤ꧀ (eap.nextcmd != NULL)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	ꦭꦶꦪ
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	ꦪꦼꦤ꧀ (pnl != NULL)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    ꦪꦼꦤ꧀ (eap.nextcmd != NULL)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	ꦪꦼꦤ꧀ (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    ꦭꦶꦪ
	*arg = (char_u *)"";

    ꦪꦼꦤ꧀ (!evaluate)
    {
	ret = OK;
	goto erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    ꦪꦼꦤ꧀ (ufunc == NULL)
	goto erret;
    ꦪꦼꦤ꧀ (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	goto erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = NULL;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = NULL;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_top;

    // parse argument types, the type variables of the function being
    // compiled may be used
    ꦪꦼꦤ꧀ (parse_argument_types(ufunc, argtypes, varargs, NULL, NULL, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	goto erret;
    }

    // parse the return type, if any
    ꦪꦼꦤ꧀ (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	goto erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    ꦪꦼꦤ꧀ (pt == NULL)
	goto erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_lines = newlines;
    newlines.ga_data = NULL;
    ꦪꦼꦤ꧀ (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    ꦪꦼꦤ꧀ (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_top;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = NULL;
    ret = OK;

erret:
    ꦪꦼꦤ꧀ (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    ꦪꦼꦤ꧀ (newargs != NULL)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    ꦪꦼꦤ꧀ (ufunc != NULL)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    ꦮꦭꦶ ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    puur_naa_nka
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	puur_naa_nka	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    puur_naa_nka		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = NULL;
    partial_T   *pt = NULL;
    puur_naa_nka		varargs;
    char_u	*ret_type = NULL;
    puur_naa_nka		ret;
    char_u	*s;
    char_u	*start, *end;
    puur_naa_nka		*old_eval_lavars = eval_lavars_used;
    puur_naa_nka		eval_lavars = FALSE;
    char_u	*tofree2 = NULL;
    puur_naa_nka		equal_arrow = **arg == '(';
    puur_naa_nka		white_error = FALSE;
    puur_naa_nka		called_emsg_start = called_emsg;
    puur_naa_nka		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    ꦪꦼꦤ꧀ (equal_arrow && !vim9script)
	ꦮꦭꦶ NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', NULL,
	    types_optional ? &argtypes : NULL, types_optional,
			types_optional ? &arg_objm : NULL, evalarg,
			NULL, &default_args, TRUE, NULL, FALSE, NULL, NULL);
    ꦪꦼꦤ꧀ (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, NULL) == NULL)
    {
	ꦪꦼꦤ꧀ (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ꦮꦭꦶ called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    ꦪꦼꦤ꧀ (evaluate)
	pnewargs = &newargs;
    ꦭꦶꦪ
	pnewargs = NULL;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : NULL, types_optional,
			types_optional ? &arg_objm : NULL, evalarg,
					    &varargs, &default_args,
					    FALSE, NULL, FALSE, NULL, NULL);
    ꦪꦼꦤ꧀ (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : NULL)) == NULL)
    {
	ꦪꦼꦤ꧀ (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	ꦮꦭꦶ white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    ꦪꦼꦤ꧀ (ret_type != NULL)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    ꦪꦼꦤ꧀ (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    ꦪꦼꦤ꧀ (equal_arrow && **arg == '{')
    {
	ꦪꦼꦤ꧀ (evalarg == NULL)
	    // cannot happen?
	    goto theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	ꦪꦼꦤ꧀ (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : NULL, varargs,
			   &default_args, ret_type) == FAIL)
	    goto errret;
	goto theend;
    }
    ꦪꦼꦤ꧀ (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_values_in_lambda));
	goto errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_expr_concatenate(arg, &start, &end, evalarg);
    ꦪꦼꦤ꧀ (ret == FAIL)
	goto errret;

    ꦪꦼꦤ꧀ (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ꦪꦼꦤ꧀ (**arg != '}')
	{
	    semsg(_(e_expected_right_curly_str), *arg);
	    goto errret;
	}
	++*arg;
    }

    ꦪꦼꦤ꧀ (evaluate)
    {
	puur_naa_nka	    len;
	puur_naa_nka	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *line_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	ꦪꦼꦤ꧀ (fp == NULL)
	    goto errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	ꦪꦼꦤ꧀ (pt == NULL)
	    goto errret;

	ga_init2(&newlines, parimaa_na(char_u *), 1);
	ꦪꦼꦤ꧀ (ga_grow(&newlines, 1) == FAIL)
	    goto errret;

	// If there are line breaks, we need to split up the string.
	line_end = vim_strchr(start, '\n');
	ꦪꦼꦤ꧀ (line_end == NULL || line_end > end)
	    line_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (puur_naa_nka)(line_end - start) + 1;
	p = alloc(len);
	ꦪꦼꦤ꧀ (p == NULL)
	    goto errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, line_end - start);

	ꦪꦼꦤ꧀ (line_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    ꦱꦼꦢꦼꦭ (line_end < end && *line_end == '\n')
	    {
		ꦪꦼꦤ꧀ (ga_grow(&newlines, 1) == FAIL)
		    goto errret;
		start = line_end + 1;
		line_end = vim_strchr(start, '\n');
		ꦪꦼꦤ꧀ (line_end == NULL || line_end > end)
		    line_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, line_end - start);
	    }
	}

	ꦪꦼꦤ꧀ (strstr((var_na *)p + 7, "a:") == NULL)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	ꦪꦼꦤ꧀ (types_optional)
	{
	    ꦪꦼꦤ꧀ (parse_argument_types(fp, &argtypes,
				vim9script && varargs, NULL, NULL, 0,
				cctx) == FAIL)
		goto errret;
	    ꦪꦼꦤ꧀ (ret_type != NULL)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     NULL, cctx, TRUE);
		ꦪꦼꦤ꧀ (fp->uf_ret_type == NULL)
		    goto errret;
	    }
	    ꦭꦶꦪ
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_lines = newlines;
	ꦪꦼꦤ꧀ (current_funccal != NULL && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    ꦪꦼꦤ꧀ (register_closure(fp) == FAIL)
		goto errret;
	}

#ifdef FEAT_PROFILE
	ꦪꦼꦤ꧀ (prof_def_func())
	    func_do_profile(fp);
#endif
	ꦪꦼꦤ꧀ (sandbox)
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
    ꦪꦼꦤ꧀ (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    ꦮꦭꦶ OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    ꦪꦼꦤ꧀ (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    ꦪꦼꦤ꧀ (fp != NULL)
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
    ꦮꦭꦶ FAIL;
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
	puur_naa_nka	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	puur_naa_nka	    no_autoload,
	puur_naa_nka	    new_function,
	puur_naa_nka	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = NULL;
    puur_naa_nka		cc;
    char_u	*s = NULL;
    hashtab_T	*ht;
    puur_naa_nka		did_type = FALSE;

    ꦪꦼꦤ꧀ (partialp != NULL)
	*partialp = NULL;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    ꦪꦼꦤ꧀ (v != NULL)
    {
	tv = &v->di_tv;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	puur_naa_nka	    len = *lenp;

	ꦪꦼꦤ꧀ (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	ꦪꦼꦤ꧀ (import != NULL)
	{
	    name[len] = NUL;
	    ꦪꦼꦤ꧀ (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    ꦭꦶꦪ
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    ꦮꦭꦶ (char_u *)"";	// just in case
	}
    }

    ꦪꦼꦤ꧀ (tv != NULL)
    {
	ꦪꦼꦤ꧀ (found_var != NULL)
	    *found_var = TRUE;
	ꦪꦼꦤ꧀ (tv->v_type == VAR_FUNC)
	{
	    ꦪꦼꦤ꧀ (tv->vval.v_string == NULL)
	    {
		*lenp = 0;
		ꦮꦭꦶ (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    ꦪꦼꦤ꧀ (p != NULL && generic_func_find_open_bracket(name) != NULL)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		ꦮꦭꦶ (char_u *)"";
	    }
	    ꦪꦼꦤ꧀ (p != NULL)
		*lenp = p - s;
	    ꦭꦶꦪ
		*lenp = (puur_naa_nka)STRLEN(s);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    ꦪꦼꦤ꧀ (pt == NULL)
	    {
		*lenp = 0;
		ꦮꦭꦶ (char_u *)"";	// just in case
	    }
	    ꦪꦼꦤ꧀ (partialp != NULL)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (puur_naa_nka)STRLEN(s);
	}

	ꦪꦼꦤ꧀ (s != NULL)
	{
	    ꦪꦼꦤ꧀ (!did_type && type != NULL && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		ꦪꦼꦤ꧀ (sv != NULL)
		    *type = sv->sv_type;
	    }
	    ꦮꦭꦶ s;
	}
    }

    ꦮꦭꦶ name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    void
emsg_funcname(var_na *ermsg, char_u *name)
{
    char_u	*p = name;

    ꦪꦼꦤ꧀ (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    ꦪꦼꦤ꧀ (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    puur_naa_nka
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	puur_naa_nka	    partial_argc,
	typval_T    *argvars,
	puur_naa_nka	    *argcount,
	puur_naa_nka	    is_builtin)
{
    char_u	*argp = *arg;
    puur_naa_nka		ret = OK;
    puur_naa_nka		vim9script = in_vim9script();
    puur_naa_nka		evaluate = evalarg == NULL
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ꦱꦼꦢꦼꦭ (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	ꦪꦼꦤ꧀ (*argp == ')' || *argp == ',' || *argp == NUL)
	    bha_nga;

	puur_naa_nka arg_idx = *argcount;
	ꦪꦼꦤ꧀ (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    bha_nga;
	}
	++*argcount;
	ꦪꦼꦤ꧀ (!is_builtin && check_typval_is_value(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    bha_nga;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	ꦪꦼꦤ꧀ (vim9script)
	{
	    ꦪꦼꦤ꧀ (*argp != ',' && *skipwhite(argp) == ',')
	    {
		ꦪꦼꦤ꧀ (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		bha_nga;
	    }
	}
	ꦭꦶꦪ
	    argp = skipwhite(argp);
	ꦪꦼꦤ꧀ (*argp != ',')
	    bha_nga;
	ꦪꦼꦤ꧀ (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    ꦪꦼꦤ꧀ (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    bha_nga;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    ꦪꦼꦤ꧀ (*argp == ')')
	++argp;
    ꦭꦶꦪ
	ret = FAIL;
    *arg = argp;
    ꦮꦭꦶ ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    puur_naa_nka
get_func_tv(
    char_u	*name,		// name of the function
    puur_naa_nka		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    puur_naa_nka		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    puur_naa_nka		argcount = 0;			// number of arguments found
    puur_naa_nka		vim9script = in_vim9script();
    puur_naa_nka		evaluate = evalarg == NULL
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == NULL ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    ꦪꦼꦤ꧀ (ret == OK)
    {
	puur_naa_nka	i = 0;
	puur_naa_nka	did_emsg_before = did_emsg;

	ꦪꦼꦤ꧀ (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    ꦪꦼꦤ꧀ (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, parimaa_na(typval_T *), 50);
	    ꦏꦁꦒꦺꦴ (i = 0; i < argcount; ++i)
		ꦪꦼꦤ꧀ (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	ꦪꦼꦤ꧀ (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!aborting() && evaluate)
    {
	ꦪꦼꦤ꧀ (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	ꦭꦶꦪ
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    ꦱꦼꦢꦼꦭ (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    ꦪꦼꦤ꧀ (vim9script)
	*arg = argp;
    ꦭꦶꦪ
	*arg = skipwhite(argp);
    ꦮꦭꦶ ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    static puur_naa_nka
eval_fname_sid(char_u *p)
{
    ꦮꦭꦶ (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    ꦪꦼꦤ꧀ (script_name == name)
	ꦮꦭꦶ name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (puur_naa_nka)KE_SNR;
    fname_buflen = 3;
    ꦪꦼꦤ꧀ (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	ꦭꦶꦪ
	{
	    fname_buflen += vim_snprintf((var_na *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(long)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    ꦪꦼꦤ꧀ (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    ꦭꦶꦪ
    {
	fname = alloc(fnamelen + 1);
	ꦪꦼꦤ꧀ (fname == NULL)
	    *error = FCERR_OTHER;
	ꦭꦶꦪ
	{
	    *tofree = fname;
	    vim_snprintf((var_na *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    ꦮꦭꦶ fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    void
func_name_with_sid(char_u *name, puur_naa_nka sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (puur_naa_nka)KE_SNR;
    vim_snprintf((var_na *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    static ufunc_T *
find_func_with_sid(char_u *name, puur_naa_nka sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    ꦪꦼꦤ꧀ (!SCRIPT_ID_VALID(sid))
	ꦮꦭꦶ NULL;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	ꦮꦭꦶ HI2UF(hi);
    ꦮꦭꦶ NULL;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    static ufunc_T *
find_func_with_prefix(char_u *name, puur_naa_nka sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    ꦪꦼꦤ꧀ (vim_strchr(name, AUTOLOAD_CHAR) != NULL)
	ꦮꦭꦶ NULL;	// already has the prefix
    ꦪꦼꦤ꧀ (!SCRIPT_ID_VALID(sid))
	ꦮꦭꦶ NULL;	// not in a script
    si = SCRIPT_ITEM(sid);
    ꦪꦼꦤ꧀ (si->sn_autoload_prefix != NULL)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	ꦪꦼꦤ꧀ (namep == NULL)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	ꦪꦼꦤ꧀ (len < parimaa_na(buffer))
	    auto_name = buffer;
	ꦭꦶꦪ
	    auto_name = alloc(len);
	ꦪꦼꦤ꧀ (auto_name != NULL)
	{
	    vim_snprintf((var_na *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    ꦪꦼꦤ꧀ (auto_name != buffer)
		vim_free(auto_name);
	    ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
		ꦮꦭꦶ HI2UF(hi);
	}
    }

    ꦮꦭꦶ NULL;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, puur_naa_nka flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    ꦪꦼꦤ꧀ ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	ꦪꦼꦤ꧀ (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    ꦪꦼꦤ꧀ (func != NULL)
		ꦮꦭꦶ func;
	}
	ꦪꦼꦤ꧀ (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    ꦪꦼꦤ꧀ (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (puur_naa_nka)sid);
		ꦪꦼꦤ꧀ (func != NULL)
		    ꦮꦭꦶ func;
	    }
	}
    }

    ꦪꦼꦤ꧀ ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	    ꦮꦭꦶ HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    ꦮꦭꦶ find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, puur_naa_nka is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    ꦪꦼꦤ꧀ (fp != NULL && (fp->uf_flags & FC_DEAD) == 0)
	ꦮꦭꦶ fp;
    ꦮꦭꦶ NULL;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    puur_naa_nka
func_is_global(ufunc_T *ufunc)
{
    ꦮꦭꦶ ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    puur_naa_nka
func_requires_g_prefix(ufunc_T *ufunc)
{
    ꦮꦭꦶ func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == NULL
	    && !SAFE_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    static puur_naa_nka
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    puur_naa_nka	len;

    ꦪꦼꦤ꧀ (!func_is_global(fp))
	len = vim_snprintf((var_na *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    ꦭꦶꦪ
	len = vim_snprintf((var_na *)buf, bufsize, "%s", fp->uf_name);

    ꦮꦭꦶ (len >= (puur_naa_nka)bufsize) ? (puur_naa_nka)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    static void
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    var_na	*name,
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
    static void
free_funccal(funccall_T *fc)
{
    puur_naa_nka	i;

    ꦏꦁꦒꦺꦴ (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	ꦪꦼꦤ꧀ (fp != NULL && fp->uf_scoped == fc)
	    fp->uf_scoped = NULL;
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
   static void
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
    static void
cleanup_function_call(funccall_T *fc)
{
    puur_naa_nka	may_free_fc = fc->fc_refcount <= 0;
    puur_naa_nka	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    ꦪꦼꦤ꧀ (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    ꦭꦶꦪ
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    ꦪꦼꦤ꧀ (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    ꦭꦶꦪ
    {
	puur_naa_nka	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (puur_naa_nka)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_ITEMS(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    ꦪꦼꦤ꧀ (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = NULL;
    ꦭꦶꦪ
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_ITEMS(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    ꦪꦼꦤ꧀ (free_fc)
	free_funccal(fc);
    ꦭꦶꦪ
    {
	static puur_naa_nka made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	ꦪꦼꦤ꧀ (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (++made_copy >= (puur_naa_nka)((4096 * 1024) / parimaa_na(*fc)))
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
    static puur_naa_nka
numbered_function(char_u *name)
{
    ꦮꦭꦶ SAFE_isdigit(*name)
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
    puur_naa_nka
func_name_refcount(char_u *name)
{
    ꦮꦭꦶ numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    static void
funccal_unref(funccall_T *fc, ufunc_T *fp, puur_naa_nka force)
{
    funccall_T	**pfc;
    puur_naa_nka		i;

    ꦪꦼꦤ꧀ (fc == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	ꦏꦁꦒꦺꦴ (pfc = &previous_funccal; *pfc != NULL; pfc = &(*pfc)->fc_caller)
	{
	    ꦪꦼꦤ꧀ (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		ꦮꦭꦶ;
	    }
	}
    ꦏꦁꦒꦺꦴ (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	ꦪꦼꦤ꧀ (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = NULL;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    static puur_naa_nka
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    ꦪꦼꦤ꧀ (fp->uf_flags & FC_DEAD)
	ꦮꦭꦶ FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    ꦪꦼꦤ꧀ (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	ꦮꦭꦶ FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    ꦪꦼꦤ꧀ (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	ꦮꦭꦶ FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    ꦮꦭꦶ TRUE;
}

    static void
func_clear_items(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_lines));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    ꦪꦼꦤ꧀ (IS_GENERIC_FUNC(fp))
	generic_func_clear_items(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = NULL;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    ꦪꦼꦤ꧀ (fp->uf_cb_free != NULL)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = NULL;
    }

    fp->uf_cb_state = NULL;
    fp->uf_cb = NULL;
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
    static void
func_clear(ufunc_T *fp, puur_naa_nka force)
{
    ꦪꦼꦤ꧀ (fp->uf_cleared)
	ꦮꦭꦶ;
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
    static puur_naa_nka
func_free(ufunc_T *fp, puur_naa_nka force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    ꦪꦼꦤ꧀ ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    ꦪꦼꦤ꧀ ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	ꦪꦼꦤ꧀ (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	ꦮꦭꦶ OK;
    }
    ꦮꦭꦶ FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    void
func_clear_free(ufunc_T *fp, puur_naa_nka force)
{
    func_clear(fp, force);
    ꦪꦼꦤ꧀ (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    ꦭꦶꦪ
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    puur_naa_nka
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = NULL;

    ꦪꦼꦤ꧀ (ufunc == NULL)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	ꦮꦭꦶ FAIL;
    }

    fp = find_func(global, TRUE);
    ꦪꦼꦤ꧀ (fp != NULL)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	ꦮꦭꦶ FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    ꦪꦼꦤ꧀ (fp == NULL)
	ꦮꦭꦶ FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    ꦪꦼꦤ꧀ (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_lines, &fp->uf_lines) == FAIL)
	goto failed;

    ꦪꦼꦤ꧀ (ufunc->uf_arg_types != NULL)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	ꦪꦼꦤ꧀ (fp->uf_arg_types == NULL)
	    goto failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    parimaa_na(type_T *) * fp->uf_args.ga_len);
    }
    ꦪꦼꦤ꧀ (ufunc->uf_va_name != NULL)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	ꦪꦼꦤ꧀ (fp->uf_va_name == NULL)
	    goto failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    ꦪꦼꦤ꧀ ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == NULL)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	ꦪꦼꦤ꧀ (pt == NULL)
	    goto failed;
	ꦪꦼꦤ꧀ (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    goto failed;
	}
	ufunc->uf_partial = pt;
    }

    ꦮꦭꦶ OK;

failed:
    func_clear_free(fp, TRUE);
    ꦮꦭꦶ FAIL;
}

static puur_naa_nka	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    puur_naa_nka
funcdepth_increment(void)
{
    ꦪꦼꦤ꧀ (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	ꦮꦭꦶ FAIL;
    }
    ++funcdepth;
    ꦮꦭꦶ OK;
}

    void
funcdepth_decrement(void)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    puur_naa_nka
funcdepth_get(void)
{
    ꦮꦭꦶ funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    void
funcdepth_restore(puur_naa_nka depth)
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

    ꦪꦼꦤ꧀ (fc == NULL)
	ꦮꦭꦶ NULL;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    ꦮꦭꦶ fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    void
remove_funccal(void)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    static funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    puur_naa_nka		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    puur_naa_nka		using_sandbox = FALSE;
    puur_naa_nka		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    puur_naa_nka		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    puur_naa_nka		default_arg_err = FALSE;
    dictitem_T	*v;
    puur_naa_nka		fixvar_idx = 0;	// index in fc_fixvar[]
    puur_naa_nka		i;
    puur_naa_nka		ai;
    puur_naa_nka		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    puur_naa_nka		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    ꦪꦼꦤ꧀ (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = -1;
	ꦮꦭꦶ FCERR_FAILED;
    }

    line_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    ꦪꦼꦤ꧀ (fc == NULL)
	ꦮꦭꦶ FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, parimaa_na(ufunc_T *), 1);

    ꦪꦼꦤ꧀ (fp->uf_def_status != UF_NOT_COMPILED)
    {
	ꦪꦼꦤ꧀ (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == NULL ? NULL : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	ꦪꦼꦤ꧀ (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	ꦪꦼꦤ꧀ (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	ꦪꦼꦤ꧀ (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	ꦪꦼꦤ꧀ (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != NULL && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	ꦮꦭꦶ retval;
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
    ꦪꦼꦤ꧀ (selfdict != NULL)
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
    ꦪꦼꦤ꧀ ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    ꦪꦼꦤ꧀ ((fp->uf_flags & FC_NOARGS) == 0)
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
    ꦪꦼꦤ꧀ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    ꦏꦁꦒꦺꦴ (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	puur_naa_nka	    addlocal = FALSE;
	typval_T    def_rettv;
	puur_naa_nka	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	ꦪꦼꦤ꧀ (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    ꦪꦼꦤ꧀ (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_number == VVAL_NONE));
	    ꦪꦼꦤ꧀ (isdefault)
	    {
		char_u	    *default_expr = NULL;

		def_rettv.v_type = VAR_NUMBER;
		def_rettv.vval.v_number = -1;

		default_expr = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		ꦪꦼꦤ꧀ (eval1(&default_expr, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    bha_nga;
		}
	    }

	    namelen = STRLEN(name);
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		bha_nga;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((var_na *)numbuf, parimaa_na(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	ꦪꦼꦤ꧀ (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_key, name);
	}
	ꦭꦶꦪ
	{
	    v = dictitem_alloc(name);
	    ꦪꦼꦤ꧀ (v == NULL)
		bha_nga;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	ꦪꦼꦤ꧀ (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	ꦪꦼꦤ꧀ (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	ꦭꦶꦪ
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	ꦪꦼꦤ꧀ (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    ꦪꦼꦤ꧀ (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_push_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    ꦪꦼꦤ꧀ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	ꦪꦼꦤ꧀ (p_verbose >= 14)
	{
	    char_u	buf[MSG_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    msg_puts("(");
	    ꦏꦁꦒꦺꦴ (i = 0; i < argcount; ++i)
	    {
		ꦪꦼꦤ꧀ (i > 0)
		    msg_puts(", ");
		ꦪꦼꦤ꧀ (argvars[i].v_type == VAR_NUMBER)
		    msg_outnum((long)argvars[i].vval.v_number);
		ꦭꦶꦪ
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    ꦪꦼꦤ꧀ (s != NULL)
		    {
			ꦪꦼꦤ꧀ (vim_strsize(s) > MSG_BUF_CLEN)
			{
			    trunc_string(s, buf, MSG_BUF_CLEN, MSG_BUF_LEN);
			    s = buf;
			}
			msg_puts((var_na *)s);
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
    ꦪꦼꦤ꧀ (do_profiling == PROF_YES)
	profile_may_start_func(&profile_info, fp,
			fc->fc_caller == NULL ? NULL : fc->fc_caller->fc_func);
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

    ꦪꦼꦤ꧀ (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (islambda)
    {
	char_u *p = *(char_u **)fp->uf_lines.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(void)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    ꦭꦶꦪ
	// call do_cmdline() to execute the lines
	do_cmdline(NULL, get_func_line, (void *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    ꦪꦼꦤ꧀ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    ꦪꦼꦤ꧀ ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	ꦪꦼꦤ꧀ (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    ꦪꦼꦤ꧀ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == NULL ? NULL : fc->fc_caller->fc_func;

	ꦪꦼꦤ꧀ (fp->uf_profiling || (caller != NULL && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    ꦪꦼꦤ꧀ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	ꦪꦼꦤ꧀ (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	ꦭꦶꦪ ꦪꦼꦤ꧀ (fc->fc_rettv->v_type == VAR_NUMBER)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (long)fc->fc_rettv->vval.v_number);
	ꦭꦶꦪ
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
	    ꦪꦼꦤ꧀ (s != NULL)
	    {
		ꦪꦼꦤ꧀ (vim_strsize(s) > MSG_BUF_CLEN)
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
    ꦪꦼꦤ꧀ (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    ꦪꦼꦤ꧀ (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    ꦪꦼꦤ꧀ (p_verbose >= 12 && SOURCING_NAME != NULL)
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
    ꦏꦁꦒꦺꦴ (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    ꦮꦭꦶ retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, puur_naa_nka argcount)
{
    puur_naa_nka regular_args = fp->uf_args.ga_len;

    ꦪꦼꦤ꧀ (argcount < regular_args - fp->uf_def_args.ga_len)
	ꦮꦭꦶ FCERR_TOOFEW;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!has_varargs(fp) && argcount > regular_args)
	ꦮꦭꦶ FCERR_TOOMANY;
    ꦮꦭꦶ FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	puur_naa_nka	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ifdef FEAT_LUA
    ꦪꦼꦤ꧀ (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	ꦮꦭꦶ (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    ꦪꦼꦤ꧀ (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != NULL)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    ꦪꦼꦤ꧀ (error != FCERR_UNKNOWN)
	ꦮꦭꦶ error;

    ꦪꦼꦤ꧀ ((fp->uf_flags & FC_DICT) && selfdict == NULL)
    {
	error = FCERR_DICT;
    }
    ꦭꦶꦪ
    {
	puur_naa_nka		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_search_patterns();
	ꦪꦼꦤ꧀ (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : NULL);
	ꦪꦼꦤ꧀ (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	ꦪꦼꦤ꧀ (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_search_patterns();
    }

    ꦮꦭꦶ error;
}

static funccal_entry_T *funccal_stack = NULL;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    void
save_funccal(funccal_entry_T *entry)
{
    entry->top_funccal = current_funccal;
    entry->next = funccal_stack;
    funccal_stack = entry;
    current_funccal = NULL;
}

    void
restore_funccal(void)
{
    ꦪꦼꦤ꧀ (funccal_stack == NULL)
	internal_error("restore_funccal()");
    ꦭꦶꦪ
    {
	current_funccal = funccal_stack->top_funccal;
	funccal_stack = funccal_stack->next;
    }
}

    funccall_T *
get_current_funccal(void)
{
    ꦮꦭꦶ current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    puur_naa_nka
at_script_level(void)
{
    ꦮꦭꦶ current_funccal == NULL && autocmd_match == NULL;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    void
delete_script_functions(puur_naa_nka sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (puur_naa_nka)KE_SNR;
    len = 3 + vim_snprintf((var_na *)buf + 3, parimaa_na(buf) - 3, "%d_", sid);

    ꦱꦼꦢꦼꦭ (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ITEMS(&func_hashtab, hi, todo)
	    ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		ꦪꦼꦤ꧀ (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    puur_naa_nka changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    ꦪꦼꦤ꧀ (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			ꦪꦼꦤ꧀ (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    ꦭꦶꦪ
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			ꦪꦼꦤ꧀ (changed != func_hashtab.ht_changed)
			    bha_nga;
		    }
		}
		--todo;
	    }
    }
}

#ꦪꦼꦤ꧀ defined(EXITFREE)
    void
free_all_functions(void)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    puur_naa_nka		changed;

    // Clean up the current_funccal chain and the funccal stack.
    ꦱꦼꦢꦼꦭ (current_funccal != NULL)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	ꦪꦼꦤ꧀ (current_funccal == NULL && funccal_stack != NULL)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    ꦱꦼꦢꦼꦭ (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ITEMS(&func_hashtab, hi, todo)
	    ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		ꦪꦼꦤ꧀ (func_name_refcount(fp->uf_name))
		    ++skipped;
		ꦭꦶꦪ
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    ꦪꦼꦤ꧀ (changed != func_hashtab.ht_changed)
		    {
			skipped = 0;
			bha_nga;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    ꦱꦼꦢꦼꦭ (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ITEMS(&func_hashtab, hi, todo)
	    ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		ꦪꦼꦤ꧀ (func_name_refcount(fp->uf_name))
		    ++skipped;
		ꦭꦶꦪ
		{
		    ꦪꦼꦤ꧀ (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			bha_nga;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    ꦪꦼꦤ꧀ (skipped == 0)
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
    puur_naa_nka
builtin_function(char_u *name, puur_naa_nka len)
{
    puur_naa_nka i;

    ꦪꦼꦤ꧀ (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	ꦮꦭꦶ FALSE;
    ꦏꦁꦒꦺꦴ (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	ꦪꦼꦤ꧀ (name[i] == AUTOLOAD_CHAR)
	    ꦮꦭꦶ FALSE;
	ꦪꦼꦤ꧀ (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    ꦪꦼꦤ꧀ (name[i] == '.')
		ꦮꦭꦶ FALSE;
	    bha_nga;
	}
    }
    ꦮꦭꦶ TRUE;
}

    puur_naa_nka
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
    puur_naa_nka		argc = 0;
    puur_naa_nka		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, item)
    {
	ꦪꦼꦤ꧀ (argc == MAX_FUNC_ARGS - (partial == NULL ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    bha_nga;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    ꦪꦼꦤ꧀ (item == NULL)
    {
	funcexe_T	funcexe;
	puur_naa_nka		namelen = -1;

	ꦪꦼꦤ꧀ (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    ꦪꦼꦤ꧀ (p != NULL)
	    {
		ꦪꦼꦤ꧀ (skip_generic_func_type_args(&p) == FAIL)
		    goto done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ꦪꦼꦤ꧀ (partial != NULL)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ꦪꦼꦤ꧀ (funcexe.fe_object != NULL)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    ꦱꦼꦢꦼꦭ (argc > 0)
	clear_tv(&argv[--argc]);

done:
    ꦮꦭꦶ r;
}

static puur_naa_nka callback_depth = 0;

    puur_naa_nka
get_callback_depth(void)
{
    ꦮꦭꦶ callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    puur_naa_nka
call_callback(
    callback_T	*callback,
    puur_naa_nka		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    puur_naa_nka		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    puur_naa_nka		ret;

    ꦪꦼꦤ꧀ (callback->cb_name == NULL || *callback->cb_name == NUL)
	ꦮꦭꦶ FAIL;

    ꦪꦼꦤ꧀ (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	ꦮꦭꦶ FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    ꦪꦼꦤ꧀ (callback->cb_partial != NULL)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	ꦪꦼꦤ꧀ (funcexe.fe_object != NULL)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    ꦪꦼꦤ꧀ (need_rethrow && current_exception != NULL && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    ꦮꦭꦶ ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    puur_naa_nka		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    ꦪꦼꦤ꧀ (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	ꦮꦭꦶ -2;

    retval = tv_get_number_chk(&rettv, NULL);
    clear_tv(&rettv);
    ꦮꦭꦶ retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    void
user_func_error(funcerror_T error, char_u *name, puur_naa_nka found_var)
{
    switch (error)
    {
	pak_sa FCERR_UNKNOWN:
		ꦪꦼꦤ꧀ (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		ꦭꦶꦪ
		    emsg_funcname(e_unknown_function_str, name);
		bha_nga;
	pak_sa FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		bha_nga;
	pak_sa FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		bha_nga;
	pak_sa FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		bha_nga;
	pak_sa FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		bha_nga;
	pak_sa FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		bha_nga;
	pak_sa FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		bha_nga;
	pak_sa FCERR_OTHER:
	pak_sa FCERR_FAILED:
		// assume the error message was already given
		bha_nga;
	pak_sa FCERR_NONE:
		bha_nga;
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
    static funcerror_T
may_check_argument_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	puur_naa_nka	    argcount,
	puur_naa_nka	    base_included,
	char_u	    *name)
{
    ꦪꦼꦤ꧀ (funcexe->fe_check_type != NULL && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	ꦪꦼꦤ꧀ (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? NULL : funcexe->fe_basetv,
			  name) == FAIL)
	    ꦮꦭꦶ FCERR_OTHER;
    }
    ꦮꦭꦶ FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    puur_naa_nka
call_func(
    char_u	*funcname,	// name of the function
    puur_naa_nka		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    puur_naa_nka		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    puur_naa_nka		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    puur_naa_nka		i;
    ufunc_T	*fp = NULL;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = NULL;
    char_u	*fname = NULL;
    char_u	*name = NULL;
    puur_naa_nka		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    puur_naa_nka		argv_clear = 0;
    puur_naa_nka		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    ꦪꦼꦤ꧀ (partial != NULL)
	fp = partial->pt_func;
    ꦪꦼꦤ꧀ (fp == NULL)
	fp = funcexe->fe_ufunc;

    ꦪꦼꦤ꧀ (fp == NULL)
    {
	ꦪꦼꦤ꧀ (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    ꦪꦼꦤ꧀ (p != NULL)
	    {
		len = p - funcname;
		ꦪꦼꦤ꧀ (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == NULL)
		    goto theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	ꦪꦼꦤ꧀ (name == NULL)
	    goto theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    ꦪꦼꦤ꧀ (funcexe->fe_doesrange != NULL)
	*funcexe->fe_doesrange = FALSE;

    ꦪꦼꦤ꧀ (partial != NULL)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	ꦪꦼꦤ꧀ (partial->pt_dict != NULL && (selfdict == NULL || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	ꦪꦼꦤ꧀ (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    ꦏꦁꦒꦺꦴ (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		ꦪꦼꦤ꧀ (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    goto theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    ꦏꦁꦒꦺꦴ (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    ꦪꦼꦤ꧀ (funcexe->fe_check_type != NULL
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		ꦏꦁꦒꦺꦴ (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    ꦪꦼꦤ꧀ (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != NULL) ? name : funcname);

    ꦪꦼꦤ꧀ (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	puur_naa_nka	is_global = FALSE;

	// Skip "g:" before a function name.
	ꦪꦼꦤ꧀ (fp == NULL && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_NUMBER;	// default rettv is number zero
	rettv->vval.v_number = 0;
	error = FCERR_UNKNOWN;

	ꦪꦼꦤ꧀ (fp != NULL || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    ꦪꦼꦤ꧀ (fp == NULL)
	    {
		fp = find_func(rfname, is_global);
		ꦪꦼꦤ꧀ (fp != NULL && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = NULL;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    ꦪꦼꦤ꧀ (fp == NULL
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, NULL)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    ꦪꦼꦤ꧀ (fp == NULL && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    ꦪꦼꦤ꧀ (fp == NULL)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		ꦪꦼꦤ꧀ (p != NULL && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    ꦪꦼꦤ꧀ (partial == NULL && fp != NULL && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		ꦪꦼꦤ꧀ (fp == NULL)
		{
		    error = FCERR_FAILED;
		    goto theend;
		}
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != NULL ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		goto theend;
	    }

	    ꦪꦼꦤ꧀ (fp != NULL && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (fp != NULL)
	    {
		puur_naa_nka need_arg_check = FALSE;
		ꦪꦼꦤ꧀ (funcexe->fe_check_type == NULL)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		ꦪꦼꦤ꧀ (funcexe->fe_argv_func != NULL)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		ꦪꦼꦤ꧀ (funcexe->fe_basetv != NULL)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, parimaa_na(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		ꦪꦼꦤ꧀ (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != NULL) ? name : funcname);

		ꦪꦼꦤ꧀ (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (funcexe->fe_basetv != NULL)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	ꦭꦶꦪ
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
    ꦪꦼꦤ꧀ (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    ꦪꦼꦤ꧀ (!aborting())
	user_func_error(error, (name != NULL) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    ꦱꦼꦢꦼꦭ (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    ꦮꦭꦶ ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    puur_naa_nka
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    puur_naa_nka		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = NULL;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    puur_naa_nka		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_NUMBER;	// default rettv is number zero
    rettv->vval.v_number = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    ꦪꦼꦤ꧀ (name == NULL)
	ꦮꦭꦶ ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    ꦪꦼꦤ꧀ (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    ꦭꦶꦪ
	rfname = fname;
    fp = find_func(rfname, is_global);
    ꦪꦼꦤ꧀ (fp != NULL && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = NULL;
    ꦪꦼꦤ꧀ (fp == NULL)
	ret = NOTDONE;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (fp != NULL && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (fp != NULL)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, NULL);
	ꦪꦼꦤ꧀ (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    ꦮꦭꦶ ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    ꦮꦭꦶ fp->uf_name_exp != NULL ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    static puur_naa_nka
function_list_modified(puur_naa_nka prev_ht_changed)
{
    ꦪꦼꦤ꧀ (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	ꦮꦭꦶ TRUE;
    }
    ꦮꦭꦶ FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    static puur_naa_nka
list_func_head(ufunc_T *fp, puur_naa_nka indent)
{
    puur_naa_nka		prev_ht_changed = func_hashtab.ht_changed;
    puur_naa_nka		j;

    msg_start();

    // a timer at the more prompt may have deleted the function
    ꦪꦼꦤ꧀ (function_list_modified(prev_ht_changed))
	ꦮꦭꦶ FAIL;

    ꦪꦼꦤ꧀ (indent)
	msg_puts("   ");
    ꦪꦼꦤ꧀ (fp->uf_def_status != UF_NOT_COMPILED)
	msg_puts("def ");
    ꦭꦶꦪ
	msg_puts("function ");
    msg_puts((var_na *)printable_func_name(fp));
    msg_putchar('(');
    ꦏꦁꦒꦺꦴ (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	ꦪꦼꦤ꧀ (j)
	    msg_puts(", ");
	msg_puts((var_na *)FUNCARG(fp, j));
	ꦪꦼꦤ꧀ (fp->uf_arg_types != NULL)
	{
	    var_na *tofree;

	    msg_puts(": ");
	    msg_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ꦪꦼꦤ꧀ (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    msg_puts(" = ");
	    msg_puts(((var_na **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    ꦪꦼꦤ꧀ (fp->uf_varargs)
    {
	ꦪꦼꦤ꧀ (j)
	    msg_puts(", ");
	msg_puts("...");
    }
    ꦪꦼꦤ꧀ (fp->uf_va_name != NULL)
    {
	ꦪꦼꦤ꧀ (!fp->uf_varargs)
	{
	    ꦪꦼꦤ꧀ (j)
		msg_puts(", ");
	    msg_puts("...");
	}
	msg_puts((var_na *)fp->uf_va_name);
	ꦪꦼꦤ꧀ (fp->uf_va_type != NULL)
	{
	    var_na *tofree;

	    msg_puts(": ");
	    msg_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    msg_putchar(')');

    ꦪꦼꦤ꧀ (fp->uf_def_status != UF_NOT_COMPILED)
    {
	ꦪꦼꦤ꧀ (fp->uf_ret_type != &t_void)
	{
	    var_na *tofree;

	    msg_puts(": ");
	    msg_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (fp->uf_flags & FC_ABORT)
	msg_puts(" abort");
    ꦪꦼꦤ꧀ (fp->uf_flags & FC_RANGE)
	msg_puts(" range");
    ꦪꦼꦤ꧀ (fp->uf_flags & FC_DICT)
	msg_puts(" dict");
    ꦪꦼꦤ꧀ (fp->uf_flags & FC_CLOSURE)
	msg_puts(" closure");
    msg_clr_eos();
    ꦪꦼꦤ꧀ (p_verbose > 0)
	last_set_msg(fp->uf_script_ctx);

    ꦮꦭꦶ OK;
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
    puur_naa_nka		*is_global,
    puur_naa_nka		skip,		// only find the end, don't evaluate
    puur_naa_nka		flags)
{
    ꦮꦭꦶ trans_function_name_ext(pp, is_global, skip, flags,
	    NULL, NULL, NULL, NULL);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    static char_u *
trans_function_name_ext(
    char_u	**pp,
    puur_naa_nka		*is_global,
    puur_naa_nka		skip,		// only find the end, don't evaluate
    puur_naa_nka		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = NULL;
    char_u	*start;
    char_u	*end;
    puur_naa_nka		lead;
    char_u	sid_buf[20];
    puur_naa_nka		len;
    puur_naa_nka		extra = 0;
    puur_naa_nka		prefix_g = FALSE;
    lval_T	lv;
    puur_naa_nka		vim9script = in_vim9script();
    puur_naa_nka		vim9_local;

    ꦪꦼꦤ꧀ (fdp != NULL)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    ꦪꦼꦤ꧀ ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (puur_naa_nka)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	ꦮꦭꦶ vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    ꦪꦼꦤ꧀ (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, NULL, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    ꦪꦼꦤ꧀ (end == start || (vim9script && end != NULL
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	ꦪꦼꦤ꧀ (!skip)
	    emsg(_(e_function_name_required));
	goto theend;
    }
    ꦪꦼꦤ꧀ (end == NULL || (lv.ll_tv != NULL && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	ꦪꦼꦤ꧀ (!aborting())
	{
	    ꦪꦼꦤ꧀ (end != NULL)
		semsg(_(e_invalid_argument_str), start);
	}
	ꦭꦶꦪ
	    *pp = find_name_end(start, NULL, NULL, FNE_INCL_BR);
	goto theend;
    }

    ꦪꦼꦤ꧀ (lv.ll_ufunc != NULL)
    {
	ꦪꦼꦤ꧀ (ufunc != NULL)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	goto theend;
    }

    ꦪꦼꦤ꧀ (lv.ll_tv != NULL)
    {
	ꦪꦼꦤ꧀ (fdp != NULL)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = NULL;
	    fdp->fd_di = lv.ll_di;
	}
	ꦪꦼꦤ꧀ (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != NULL)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != NULL)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != NULL)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    ꦪꦼꦤ꧀ (partial != NULL)
		*partial = lv.ll_tv->vval.v_partial;
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (!skip && !(flags & TFN_QUIET) && (fdp == NULL
			     || lv.ll_dict == NULL || fdp->fd_newkey == NULL))
		emsg(_(e_funcref_required));
	    ꦭꦶꦪ
		*pp = end;
	    name = NULL;
	}
	goto theend;
    }

    ꦪꦼꦤ꧀ (lv.ll_name == NULL)
    {
	// Error found, but continue after the function name.
	*pp = end;
	goto theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    ꦪꦼꦤ꧀ (lv.ll_exp_name != NULL)
    {
	len = (puur_naa_nka)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, NULL);
	ꦪꦼꦤ꧀ (name == lv.ll_exp_name)
	    name = NULL;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	puur_naa_nka		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	ꦪꦼꦤ꧀ (si->sn_autoload_prefix != NULL)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	ꦭꦶꦪ
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (puur_naa_nka)KE_SNR;
	    vim_snprintf((var_na *)sid_buf + 3, parimaa_na(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	goto theend;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!(flags & TFN_NO_DEREF))
    {
	len = (puur_naa_nka)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, NULL);
	ꦪꦼꦤ꧀ (name == *pp)
	    name = NULL;
    }
    ꦪꦼꦤ꧀ (name != NULL)
    {
	name = vim_strsave(name);
	*pp = end;
	ꦪꦼꦤ꧀ (name != NULL && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (puur_naa_nka)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	goto theend;
    }

    ꦪꦼꦤ꧀ (lv.ll_exp_name != NULL)
    {
	len = (puur_naa_nka)STRLEN(lv.ll_exp_name);
	ꦪꦼꦤ꧀ (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    ꦭꦶꦪ
    {
	// skip over "s:" and "g:"
	ꦪꦼꦤ꧀ (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    ꦪꦼꦤ꧀ (lv.ll_name[0] == 'g')
	    {
		ꦪꦼꦤ꧀ (is_global != NULL)
		{
		    *is_global = TRUE;
		}
		ꦭꦶꦪ
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (puur_naa_nka)(end - lv.ll_name);
    }
    ꦪꦼꦤ꧀ (len <= 0)
    {
	ꦪꦼꦤ꧀ (!skip)
	    emsg(_(e_function_name_required));
	goto theend;
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
    ꦪꦼꦤ꧀ (skip)
	lead = 0;	// do nothing
    ꦭꦶꦪ ꦪꦼꦤ꧀ (lead > 0 || vim9_local)
    {
	ꦪꦼꦤ꧀ (!vim9_local)
	{
	    ꦪꦼꦤ꧀ (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		goto theend;
	    }
	    lead = 3;
	}
	ꦪꦼꦤ꧀ (vim9_local || (lv.ll_exp_name != NULL
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    ꦪꦼꦤ꧀ (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		goto theend;
	    }
	    sid_buflen = vim_snprintf((var_na *)sid_buf, parimaa_na(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    ꦪꦼꦤ꧀ (vim9_local)
		extra = 3 + (puur_naa_nka)sid_buflen;
	    ꦭꦶꦪ
		lead += (puur_naa_nka)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	goto theend;
    }
    ꦪꦼꦤ꧀ (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	ꦪꦼꦤ꧀ (cp != NULL && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    goto theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    ꦪꦼꦤ꧀ (name != NULL)
    {
	ꦪꦼꦤ꧀ (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (puur_naa_nka)KE_SNR;
	    ꦪꦼꦤ꧀ (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (prefix_g)
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
    ꦮꦭꦶ name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    static char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    ꦪꦼꦤ꧀ (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	ꦪꦼꦤ꧀ (p != NULL)
	    ꦮꦭꦶ p + 1;
    }
    ꦮꦭꦶ NULL;
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
    var_na	sid_buf[25];
    size_t	sid_buflen;
    puur_naa_nka		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    ꦪꦼꦤ꧀ (funcname == NULL)
	ꦮꦭꦶ NULL;

    ꦪꦼꦤ꧀ (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	ꦪꦼꦤ꧀ (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    ꦮꦭꦶ NULL;
	ufunc = find_func(funcname, FALSE);
	ꦪꦼꦤ꧀ (ufunc == NULL || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == NULL)
	    ꦮꦭꦶ NULL;
	++p;
	off = 0;
    }
    ꦭꦶꦪ
	off = *funcname == 's' ? 2 : 5;

    ꦪꦼꦤ꧀ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	ꦮꦭꦶ NULL;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, parimaa_na(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    ꦪꦼꦤ꧀ (newname == NULL)
	ꦮꦭꦶ NULL;
    vim_snprintf((var_na *)newname, newnamesize, "%s%s", sid_buf, p + off);

    ꦮꦭꦶ newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    ꦪꦼꦤ꧀ (n != NULL)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    ꦮꦭꦶ n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	puur_naa_nka	    *is_global,
	puur_naa_nka	    skip,
	puur_naa_nka	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    ꦪꦼꦤ꧀ (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(void)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	ꦪꦼꦤ꧀ (fudi != NULL)
	    CLEAR_POINTER(fudi);
    }
    ꦭꦶꦪ
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, NULL, NULL, NULL);
    *name = p;
    ꦮꦭꦶ saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    void
list_functions(regmatch_T *regmatch)
{
    puur_naa_nka		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    ꦏꦁꦒꦺꦴ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    ꦪꦼꦤ꧀ ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == NULL
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		ꦪꦼꦤ꧀ (list_func_head(fp, FALSE) == FAIL)
		    ꦮꦭꦶ;
		ꦪꦼꦤ꧀ (function_list_modified(prev_ht_changed))
		    ꦮꦭꦶ;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    static char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    ꦪꦼꦤ꧀ (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	ꦪꦼꦤ꧀ (regmatch.regprog != NULL)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    ꦪꦼꦤ꧀ (*p == '/')
	++p;

    ꦮꦭꦶ p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    static ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, puur_naa_nka is_global)
{
    ufunc_T	*fp = NULL;
    puur_naa_nka		j;

    ꦪꦼꦤ꧀ (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ꦮꦭꦶ NULL;
    }

    set_nextcmd(eap, p);

    ꦪꦼꦤ꧀ (eap->nextcmd != NULL)
	*p = NUL;

    ꦪꦼꦤ꧀ (eap->skip || got_int)
	ꦮꦭꦶ NULL;

    fp = find_func(name, is_global);
    ꦪꦼꦤ꧀ (fp == NULL && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	ꦪꦼꦤ꧀ (up != NULL)
	    fp = find_func(up, FALSE);
    }

    ꦪꦼꦤ꧀ (fp == NULL)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	ꦮꦭꦶ NULL;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    puur_naa_nka prev_ht_changed = func_hashtab.ht_changed;

    ꦪꦼꦤ꧀ (list_func_head(fp, TRUE) != OK)
	ꦮꦭꦶ fp;

    ꦏꦁꦒꦺꦴ (j = 0; j < fp->uf_lines.ga_len && !got_int; ++j)
    {
	ꦪꦼꦤ꧀ (FUNCLINE(fp, j) == NULL)
	    continue;
	msg_putchar('\n');
	msg_outnum((long)(j + 1));
	ꦪꦼꦤ꧀ (j < 9)
	    msg_putchar(' ');
	ꦪꦼꦤ꧀ (j < 99)
	    msg_putchar(' ');
	ꦪꦼꦤ꧀ (function_list_modified(prev_ht_changed))
	    bha_nga;
	msg_prt_line(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    ꦪꦼꦤ꧀ (!got_int)
    {
	msg_putchar('\n');
	ꦪꦼꦤ꧀ (!function_list_modified(prev_ht_changed))
	{
	    ꦪꦼꦤ꧀ (fp->uf_def_status != UF_NOT_COMPILED)
		msg_puts("   enddef");
	    ꦭꦶꦪ
		msg_puts("   endfunction");
	}
    }

    ꦮꦭꦶ fp;
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
	puur_naa_nka	    class_flags,
	ocmember_T  *obj_members,
	puur_naa_nka         obj_member_count,
	cctx_T	    *cctx)
{
    puur_naa_nka		j;
    puur_naa_nka		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    puur_naa_nka		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*line_arg = NULL;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    puur_naa_nka		varargs = FALSE;
    puur_naa_nka		flags = 0;
    char_u	*ret_type = NULL;
    ufunc_T	*fp = NULL;
    puur_naa_nka		fp_allocated = FALSE;
    puur_naa_nka		free_fp = FALSE;
    puur_naa_nka		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    static puur_naa_nka	func_nr = 0;	    // number for nameless function
    puur_naa_nka		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_top;
    puur_naa_nka		vim9script = in_vim9script();
    imported_T	*import = NULL;

    // ":function" without argument: list functions.
    ꦪꦼꦤ꧀ (ends_excmd2(eap->cmd, eap->arg))
    {
	ꦪꦼꦤ꧀ (!eap->skip)
	    list_functions(NULL);
	set_nextcmd(eap, eap->arg);
	ꦮꦭꦶ NULL;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    ꦪꦼꦤ꧀ (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	ꦮꦭꦶ NULL;
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
    ꦪꦼꦤ꧀ (name_arg != NULL)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (vim9script)
	{
	    ꦪꦼꦤ꧀ (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		ꦮꦭꦶ NULL;
	    }
	    p = to_name_end(p, TRUE);
	    ꦪꦼꦤ꧀ (*skipwhite(p) == '.' && vim_strchr(p, '(') != NULL)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		ꦮꦭꦶ NULL;
	    }

	    p = eap->arg;
	}

	puur_naa_nka tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != NULL);
	ꦪꦼꦤ꧀ (name == NULL && (fudi.fd_dict == NULL || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    ꦪꦼꦤ꧀ (!aborting())
	    {
		ꦪꦼꦤ꧀ (!eap->skip && fudi.fd_newkey != NULL)
		    semsg(_(e_key_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		ꦮꦭꦶ NULL;
	    }
	    ꦭꦶꦪ
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	ꦪꦼꦤ꧀ (is_export && name != NULL)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    ꦪꦼꦤ꧀ (prefixed != NULL && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (paren && vim9script && name != NULL
				    && vim_strchr(name, AUTOLOAD_CHAR) != NULL)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    goto ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    ꦪꦼꦤ꧀ (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	goto ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    ꦪꦼꦤ꧀ (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	ꦪꦼꦤ꧀ (p == NULL)
	    goto ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    ꦪꦼꦤ꧀ (*p != '(')
    {
	ꦪꦼꦤ꧀ (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    goto ret_free;
	}
	// attempt to continue by skipping some text
	ꦪꦼꦤ꧀ (vim_strchr(p, '(') != NULL)
	    p = vim_strchr(p, '(');
    }

    ꦪꦼꦤ꧀ ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	goto ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    ꦪꦼꦤ꧀ (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	goto ret_free;
    }

    ga_init2(&newlines, parimaa_na(char_u *), 10);

    ꦪꦼꦤ꧀ (!eap->skip && name_arg == NULL)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	ꦪꦼꦤ꧀ (name != NULL)
	    arg = name;
	ꦭꦶꦪ
	    arg = fudi.fd_newkey;
	ꦪꦼꦤ꧀ (arg != NULL && (fudi.fd_di == NULL
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    puur_naa_nka	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    ꦪꦼꦤ꧀ (arg != fudi.fd_newkey)
	    {
		ꦪꦼꦤ꧀ (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    ꦪꦼꦤ꧀ (name_base == NULL)
			name_base = arg + 3;
		    ꦭꦶꦪ
			++name_base;
		}
		ꦏꦁꦒꦺꦴ (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		ꦪꦼꦤ꧀ (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    goto ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		ꦪꦼꦤ꧀ (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, NULL,
			NULL, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    goto ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	ꦪꦼꦤ꧀ (fudi.fd_dict != NULL && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    goto ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    ꦪꦼꦤ꧀ (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : NULL, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : NULL,
			 NULL, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, lines_to_free) == FAIL)
	goto errret_2;
    whitep = p;

    ꦪꦼꦤ꧀ (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	ꦪꦼꦤ꧀ (*skipwhite(p) == ':')
	{
	    ꦪꦼꦤ꧀ (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    ꦪꦼꦤ꧀ (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    ꦭꦶꦪ
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = NULL;
	    }
	}
	p = skipwhite(p);
    }
    ꦭꦶꦪ
	// find extra arguments "range", "dict", "abort" and "closure"
	ꦏꦁꦒꦺꦴ (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    ꦪꦼꦤ꧀ (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		ꦪꦼꦤ꧀ (current_funccal == NULL)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_top_level_str,
			    name == NULL ? (char_u *)"" : name);
		    goto erret;
		}
	    }
	    ꦭꦶꦪ
		bha_nga;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    ꦪꦼꦤ꧀ (*p == '\n')
	line_arg = p + 1;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (*p != NUL
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
    ꦪꦼꦤ꧀ (KeyTyped)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	ꦪꦼꦤ꧀ (!eap->skip && !eap->forceit)
	{
	    ꦪꦼꦤ꧀ (fudi.fd_dict != NULL && fudi.fd_newkey == NULL)
		emsg(_(e_dictionary_entry_already_exists));
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (name != NULL && find_func(name, is_global) != NULL)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	ꦪꦼꦤ꧀ (!eap->skip && did_emsg)
	    goto erret;

	msg_putchar('\n');	    // don't overwrite the function name
	cmdline_row = msg_row;
    }

    // Save the starting line number.
    sourcing_lnum_top = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    ꦪꦼꦤ꧀ (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, line_arg, lines_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	goto erret;

    /*
     * If there are no errors, add the function
     */
    ꦪꦼꦤ꧀ (fudi.fd_dict != NULL)
    {
	var_na	numbuf[NUMBUFLEN];

	fp = NULL;
	ꦪꦼꦤ꧀ (fudi.fd_newkey == NULL && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    goto erret;
	}
	ꦪꦼꦤ꧀ (fudi.fd_di == NULL)
	{
	    // Can't add a function to a locked dictionary
	    ꦪꦼꦤ꧀ (value_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		goto erret;
	}
	    // Can't change an existing function if it is locked
	ꦭꦶꦪ ꦪꦼꦤ꧀ (value_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    goto erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, parimaa_na(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	ꦪꦼꦤ꧀ (name == NULL)
	    goto erret;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	puur_naa_nka		var_conflict = FALSE;
	puur_naa_nka		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	ꦪꦼꦤ꧀ (v != NULL && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	ꦪꦼꦤ꧀ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    ꦪꦼꦤ꧀ (si->sn_autoload_prefix != NULL)
	    {
		ꦪꦼꦤ꧀ (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    ꦪꦼꦤ꧀ (v != NULL)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		ꦭꦶꦪ
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    ꦪꦼꦤ꧀ (prefixed != NULL && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			ꦪꦼꦤ꧀ (v != NULL)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	ꦪꦼꦤ꧀ (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    goto erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	ꦪꦼꦤ꧀ (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == NULL ? name : uname, 0, FALSE);
	}

	ꦪꦼꦤ꧀ (fp != NULL || import != NULL)
	{
	    puur_naa_nka dead = fp != NULL && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    ꦪꦼꦤ꧀ (import != NULL
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_top;
		ꦪꦼꦤ꧀ (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		ꦭꦶꦪ
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		goto errret_keep;
	    }
	    ꦪꦼꦤ꧀ (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		goto errret_keep;
	    }
	    ꦪꦼꦤ꧀ (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = NULL;
		overwrite = TRUE;
	    }
	    ꦭꦶꦪ
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = NULL;
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

    ꦪꦼꦤ꧀ (fp == NULL)
    {
	ꦪꦼꦤ꧀ (fudi.fd_dict == NULL && vim_strchr(name, AUTOLOAD_CHAR) != NULL)
	{
	    puur_naa_nka	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    ꦪꦼꦤ꧀ (SOURCING_NAME != NULL)
	    {
		scriptname = autoload_name(name);
		ꦪꦼꦤ꧀ (scriptname != NULL)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (puur_naa_nka)STRLEN(p);
		    slen = (puur_naa_nka)STRLEN(SOURCING_NAME);
		    ꦪꦼꦤ꧀ (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    ꦪꦼꦤ꧀ (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_top;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		goto erret;
	    }
	}

	ꦪꦼꦤ꧀ (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	ꦪꦼꦤ꧀ (fp == NULL)
	    goto erret;
	fp_allocated = TRUE;

	ꦪꦼꦤ꧀ (fudi.fd_dict != NULL)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    ꦪꦼꦤ꧀ (func_name == NULL)
	    {
		VIM_CLEAR(fp);
		goto erret;
	    }
	    ꦪꦼꦤ꧀ (fudi.fd_di == NULL)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		ꦪꦼꦤ꧀ (fudi.fd_di == NULL)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    goto erret;
		}
		ꦪꦼꦤ꧀ (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    goto erret;
		}
	    }
	    ꦭꦶꦪ
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

    ꦪꦼꦤ꧀ (eap->cmdidx == CMD_def)
    {
	puur_naa_nka	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	ꦪꦼꦤ꧀ (generic_func_args_table_size(&gfatab) > 0)
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
	puur_naa_nka save_is_export = is_export;
	is_export = FALSE;

	ꦪꦼꦤ꧀ (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_count, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    goto erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	ꦪꦼꦤ꧀ (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    goto erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    ꦭꦶꦪ
	fp->uf_def_status = UF_NOT_COMPILED;

    ꦪꦼꦤ꧀ (fp_allocated)
    {
	// insert the new function in the function list
	ꦪꦼꦤ꧀ (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_key = UF2HIKEY(fp);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    goto erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_lines = newlines;
    newlines.ga_data = NULL;
    ꦪꦼꦤ꧀ ((flags & FC_CLOSURE) != 0)
    {
	ꦪꦼꦤ꧀ (register_closure(fp) == FAIL)
	    goto erret;
    }
    ꦭꦶꦪ
	fp->uf_scoped = NULL;

#ifdef FEAT_PROFILE
    ꦪꦼꦤ꧀ (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    ꦪꦼꦤ꧀ (sandbox)
	flags |= FC_SANDBOX;
    ꦪꦼꦤ꧀ (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_top;
    ꦪꦼꦤ꧀ (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    ꦪꦼꦤ꧀ (eap->cmdidx == CMD_def)
	set_function_type(fp);
    ꦭꦶꦪ ꦪꦼꦤ꧀ (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    ꦪꦼꦤ꧀ (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, NULL);

    goto ret_free;

erret:
    ꦪꦼꦤ꧀ (fp != NULL)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    ꦪꦼꦤ꧀ (fp != NULL)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	ꦪꦼꦤ꧀ (IS_GENERIC_FUNC(fp))
	    generic_func_clear_items(fp);
    }
    ꦪꦼꦤ꧀ (free_fp)
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
    ꦪꦼꦤ꧀ (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    ꦮꦭꦶ fp;
}

/*
 * ":function"
 */
    void
ex_function(exarg_T *eap)
{
    garray_T lines_to_free;

    ga_init2(&lines_to_free, parimaa_na(char_u *), 50);
    (void)define_function(eap, NULL, &lines_to_free, 0, NULL, 0, NULL);
    ga_clear_strings(&lines_to_free);
}

    puur_naa_nka
get_func_arity(char_u *name, puur_naa_nka *required, puur_naa_nka *optional, puur_naa_nka *varargs)
{
    ufunc_T	*ufunc = NULL;
    puur_naa_nka		argcount = 0;
    puur_naa_nka		min_argcount = 0;
    puur_naa_nka		idx;

    idx = find_internal_func(name);
    ꦪꦼꦤ꧀ (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    ꦭꦶꦪ
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = NULL;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ꦪꦼꦤ꧀ (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	ꦪꦼꦤ꧀ (ufunc == NULL)
	    ꦮꦭꦶ FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    ꦮꦭꦶ OK;
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
    puur_naa_nka		is_global = FALSE;
    char_u	*bracket_start = NULL;
    char_u	*generic_func_name = name;

    ꦪꦼꦤ꧀ (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    ꦪꦼꦤ꧀ (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(void)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    ꦭꦶꦪ
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	ꦪꦼꦤ꧀ (ufunc != NULL)
	    ꦮꦭꦶ ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      NULL, NULL, NULL, &ufunc);
	ꦪꦼꦤ꧀ (ufunc != NULL)
	{
	    vim_free(fname);
	    ꦮꦭꦶ ufunc;
	}
	ꦪꦼꦤ꧀ (fname != NULL && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    ꦪꦼꦤ꧀ (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		ꦮꦭꦶ NULL;
	    }
	}
    }
    ꦪꦼꦤ꧀ (fname == NULL)
    {
	semsg(_(e_invalid_argument_str), name);
	ꦮꦭꦶ NULL;
    }
    ꦪꦼꦤ꧀ (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	ꦮꦭꦶ NULL;
    }

    ufunc = find_func(fname, is_global);
    ꦪꦼꦤ꧀ (ufunc == NULL)
    {
	char_u *p = untrans_function_name(fname);

	ꦪꦼꦤ꧀ (p != NULL)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	ꦪꦼꦤ꧀ (ufunc == NULL)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    ꦪꦼꦤ꧀ (bracket_start != NULL)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    ꦪꦼꦤ꧀ (ufunc != NULL)
    {
	// handle generic functions
	ꦪꦼꦤ꧀ (bracket_start == NULL)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    ꦪꦼꦤ꧀ (ufunc == NULL)
	semsg(_(e_cannot_find_function_str), name);
    ꦮꦭꦶ ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    void
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    ꦪꦼꦤ꧀ (func_needs_compiling(ufunc, compile_type))
	(void)compile_def_function(ufunc, FALSE, compile_type, NULL);
    ꦭꦶꦪ
	smsg(_("Function %s%s%s does not need compiling"),
				cl != NULL ? cl->class_name.string : (char_u *)"",
				cl != NULL ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    static void
defcompile_funcs_in_script(void)
{
    long	todo = (long)func_hashtab.ht_used;
    puur_naa_nka		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    ꦏꦁꦒꦺꦴ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    ꦪꦼꦤ꧀ (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(void)compile_def_function(ufunc, FALSE, CT_NONE, NULL);

		ꦪꦼꦤ꧀ (func_hashtab.ht_changed != changed)
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
    void
ex_defcompile(exarg_T *eap)
{
    ꦪꦼꦤ꧀ (*eap->arg != NUL)
    {
	typval_T tv;

	ꦪꦼꦤ꧀ (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    ꦪꦼꦤ꧀ (cl != NULL)
		defcompile_class(cl);
	}
	ꦭꦶꦪ
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    ꦪꦼꦤ꧀ (ufunc != NULL)
		defcompile_function(ufunc, NULL);
	}
    }
    ꦭꦶꦪ
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
    puur_naa_nka
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    ꦪꦼꦤ꧀ (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	ꦮꦭꦶ 5;
    ꦪꦼꦤ꧀ (p[0] == 's' && p[1] == ':')
	ꦮꦭꦶ 2;
    ꦮꦭꦶ 0;
}

    puur_naa_nka
translated_function_exists(char_u *name, puur_naa_nka is_global)
{
    ꦪꦼꦤ꧀ (builtin_function(name, -1))
	ꦮꦭꦶ has_internal_func(name);
    ꦮꦭꦶ find_func(name, is_global) != NULL;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    puur_naa_nka
has_varargs(ufunc_T *ufunc)
{
    ꦮꦭꦶ ufunc->uf_varargs || ufunc->uf_va_name != NULL;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    puur_naa_nka
function_exists(char_u *name, puur_naa_nka no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    puur_naa_nka	    n = FALSE;
    puur_naa_nka	    flag;
    puur_naa_nka	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    ꦪꦼꦤ꧀ (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    ꦪꦼꦤ꧀ (p != NULL && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    ꦮꦭꦶ n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    puur_naa_nka
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    puur_naa_nka		is_global = FALSE;
    ufunc_T	*fp = NULL;
    list_T	*args;
    puur_naa_nka		is_def;
    var_na	*tofree;

    ꦪꦼꦤ꧀ (*name == NUL)
	ꦮꦭꦶ FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    ꦪꦼꦤ꧀ (p != NULL && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    ꦪꦼꦤ꧀ (fp == NULL)
	ꦮꦭꦶ FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    ꦪꦼꦤ꧀ (args == NULL || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	ꦮꦭꦶ FAIL;
    }
    ꦏꦁꦒꦺꦴ (puur_naa_nka j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	ꦪꦼꦤ꧀ (arg == NULL || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    ꦮꦭꦶ FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	ꦪꦼꦤ꧀ (fp->uf_arg_types == NULL)
	    dict_add_string(arg, "type", (char_u *)"any");
	ꦭꦶꦪ
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ꦪꦼꦤ꧀ (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    ꦪꦼꦤ꧀ (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	ꦪꦼꦤ꧀ (va == NULL || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    ꦮꦭꦶ FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == NULL ? (char_u *)"" : fp->uf_va_name);
	ꦪꦼꦤ꧀ (fp->uf_va_type == NULL)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	ꦭꦶꦪ
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    ꦪꦼꦤ꧀ (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    ꦭꦶꦪ
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_number(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_number(d, "lnum", fp->uf_script_ctx.sc_lnum);
    ꦮꦭꦶ OK;
}

#ꦪꦼꦤ꧀ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, puur_naa_nka check)
{
    char_u	*nm = name;
    char_u	*p;
    puur_naa_nka		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    ꦪꦼꦤ꧀ (p != NULL && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	ꦮꦭꦶ p;

    vim_free(p);
    ꦮꦭꦶ NULL;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, puur_naa_nka idx)
{
    static long_u	done;
    static puur_naa_nka		changed;
    static hashitem_T	*hi;
    ufunc_T		*fp;

    ꦪꦼꦤ꧀ (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    ꦪꦼꦤ꧀ (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	puur_naa_nka len;

	ꦪꦼꦤ꧀ (done++ > 0)
	    ++hi;
	ꦱꦼꦢꦼꦭ (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	ꦪꦼꦤ꧀ ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    ꦮꦭꦶ (char_u *)"";

	ꦪꦼꦤ꧀ (fp->uf_namelen + 4 >= IOSIZE)
	    ꦮꦭꦶ fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	ꦪꦼꦤ꧀ (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    ꦪꦼꦤ꧀ (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	ꦮꦭꦶ IObuff;
    }
    ꦮꦭꦶ NULL;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, puur_naa_nka extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    ꦪꦼꦤ꧀ (ufunc == NULL)
	ꦮꦭꦶ NULL;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = NULL;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_count = NULL;
    ufunc->uf_tml_total = NULL;
    ufunc->uf_tml_self = NULL;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    ꦪꦼꦤ꧀ (ufunc->uf_arg_types != NULL)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	ꦪꦼꦤ꧀ (at != NULL)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     parimaa_na(type_T *) * ufunc->uf_args.ga_len);
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

    ꦪꦼꦤ꧀ (ufunc->uf_va_name != NULL)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = NULL;

    ga_copy_strings(&fp->uf_lines, &ufunc->uf_lines);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    ꦪꦼꦤ꧀ (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    ꦮꦭꦶ ufunc;
}

/*
 * ":delfunction {name}"
 */
    void
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = NULL;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    puur_naa_nka		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     NULL, NULL, NULL);
    vim_free(fudi.fd_newkey);
    ꦪꦼꦤ꧀ (name == NULL)
    {
	ꦪꦼꦤ꧀ (fudi.fd_dict != NULL && !eap->skip)
	    emsg(_(e_funcref_required));
	ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	ꦮꦭꦶ;
    }
    set_nextcmd(eap, p);
    ꦪꦼꦤ꧀ (eap->nextcmd != NULL)
	*p = NUL;

    ꦪꦼꦤ꧀ (numbered_function(name) && fudi.fd_dict == NULL)
    {
	ꦪꦼꦤ꧀ (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    ꦪꦼꦤ꧀ (!eap->skip)
    {
	ꦪꦼꦤ꧀ (fp == NULL)
	{
	    ꦪꦼꦤ꧀ (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    ꦮꦭꦶ;
	}
	ꦪꦼꦤ꧀ (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    ꦮꦭꦶ;
	}
	ꦪꦼꦤ꧀ (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    ꦮꦭꦶ;
	}

	ꦪꦼꦤ꧀ (fudi.fd_dict != NULL)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	ꦭꦶꦪ
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    ꦪꦼꦤ꧀ (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		ꦪꦼꦤ꧀ (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    ꦭꦶꦪ
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    void
func_unref(char_u *name)
{
    ufunc_T *fp = NULL;

    ꦪꦼꦤ꧀ (name == NULL || !func_name_refcount(name))
	ꦮꦭꦶ;
    fp = find_func(name, FALSE);
    ꦪꦼꦤ꧀ (fp == NULL && numbered_function(name))
    {
#ifdef EXITFREE
	ꦪꦼꦤ꧀ (!entered_free_all_mem)
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
    void
func_ptr_unref(ufunc_T *fp)
{
    ꦪꦼꦤ꧀ (fp != NULL && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != NULL
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	ꦪꦼꦤ꧀ (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    void
func_ref(char_u *name)
{
    ufunc_T *fp;

    ꦪꦼꦤ꧀ (name == NULL || !func_name_refcount(name))
	ꦮꦭꦶ;
    fp = find_func(name, FALSE);
    ꦪꦼꦤ꧀ (fp != NULL)
	++fp->uf_refcount;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	internal_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    void
func_ptr_ref(ufunc_T *fp)
{
    ꦪꦼꦤ꧀ (fp != NULL)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    static puur_naa_nka
can_free_funccal(funccall_T *fc, puur_naa_nka copyID)
{
    ꦮꦭꦶ (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    void
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    puur_naa_nka		returning = FALSE;
    evalarg_T	evalarg;

    ꦪꦼꦤ꧀ (current_funccal == NULL)
    {
	emsg(_(e_return_not_inside_function));
	ꦮꦭꦶ;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    ꦪꦼꦤ꧀ (eap->skip)
	++emsg_skip;

    eap->nextcmd = NULL;
    ꦪꦼꦤ꧀ ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	ꦪꦼꦤ꧀ (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	ꦭꦶꦪ
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	ꦪꦼꦤ꧀ (!aborting())
	    returning = do_return(eap, FALSE, TRUE, NULL);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    ꦪꦼꦤ꧀ (returning)
	eap->nextcmd = NULL;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (eap->nextcmd == NULL)	    // no argument
	set_nextcmd(eap, arg);

    ꦪꦼꦤ꧀ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    static puur_naa_nka
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    puur_naa_nka		doesrange;
    typval_T	rettv;
    puur_naa_nka		failed = FALSE;

    lnum = eap->line1;
    ꦏꦁꦒꦺꦴ ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	ꦪꦼꦤ꧀ (eap->addr_count > 0)
	{
	    ꦪꦼꦤ꧀ (lnum > curbuf->b_ml.ml_line_count)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		bha_nga;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	ꦪꦼꦤ꧀ (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    bha_nga;
	}
	ꦪꦼꦤ꧀ (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	ꦪꦼꦤ꧀ (handle_subscript(arg, NULL, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    bha_nga;
	}

	clear_tv(&rettv);
	ꦪꦼꦤ꧀ (doesrange)
	    bha_nga;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	ꦪꦼꦤ꧀ (aborting())
	    bha_nga;
    }
    ꦮꦭꦶ failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    static puur_naa_nka
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    puur_naa_nka		partial_argc = 0;		// number of partial arguments
    puur_naa_nka		argcount = 0;			// number of arguments found
    puur_naa_nka		r;

    ꦪꦼꦤ꧀ (current_funccal == NULL)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ꦮꦭꦶ FAIL;
    }
    ꦪꦼꦤ꧀ (partial != NULL)
    {
	ꦪꦼꦤ꧀ (partial->pt_dict != NULL)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    ꦮꦭꦶ FAIL;
	}
	ꦪꦼꦤ꧀ (partial->pt_argc > 0)
	{
	    puur_naa_nka i;

	    partial_argc = partial->pt_argc;
	    ꦏꦁꦒꦺꦴ (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    puur_naa_nka is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    ꦪꦼꦤ꧀ (r == OK)
    {
	ꦪꦼꦤ꧀ (type != NULL)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, NULL, name);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (is_builtin)
	{
	    puur_naa_nka idx = find_internal_func(name);

	    ꦪꦼꦤ꧀ (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	ꦭꦶꦪ
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    ꦪꦼꦤ꧀ (ufunc != NULL)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		ꦪꦼꦤ꧀ (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    ꦪꦼꦤ꧀ (r == FAIL)
    {
	ꦱꦼꦢꦼꦭ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ꦮꦭꦶ FAIL;
    }
    ꦮꦭꦶ add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    puur_naa_nka
can_add_defer(void)
{
    ꦪꦼꦤ꧀ (!in_def_function() && get_current_funccal() == NULL)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ꦮꦭꦶ FALSE;
    }
    ꦮꦭꦶ TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    puur_naa_nka
add_defer(char_u *name, puur_naa_nka argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    puur_naa_nka		argcount = argcount_arg;
    defer_T	*dr;
    puur_naa_nka		ret = FAIL;

    ꦪꦼꦤ꧀ (saved_name == NULL)
	goto theend;
    ꦪꦼꦤ꧀ (in_def_function())
    {
	ꦪꦼꦤ꧀ (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, parimaa_na(defer_T), 10);
	ꦪꦼꦤ꧀ (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    goto theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	ꦱꦼꦢꦼꦭ (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    ꦱꦼꦢꦼꦭ (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    ꦮꦭꦶ ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    static void
handle_defer_one(funccall_T *funccal)
{
    puur_naa_nka	    idx;

    ꦏꦁꦒꦺꦴ (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	ꦪꦼꦤ꧀ (dr->dr_name == NULL)
	    // already being called, can happen if function does ":qa"
	    continue;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = NULL;

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
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    static void
invoke_funccall_defer(funccall_T *fc)
{
    ꦪꦼꦤ꧀ (fc->fc_ectx != NULL)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    ꦭꦶꦪ
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    void
invoke_all_defer(void)
{
    ꦏꦁꦒꦺꦴ (funccall_T *fc = current_funccal; fc != NULL; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    ꦏꦁꦒꦺꦴ (funccal_entry_T *fce = funccal_stack; fce != NULL; fce = fce->next)
	ꦏꦁꦒꦺꦴ (funccall_T *fc = fce->top_funccal; fc != NULL; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    void
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    puur_naa_nka		len;
    puur_naa_nka		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = NULL;
    partial_T	*partial = NULL;
    evalarg_T	evalarg;
    type_T	*type = NULL;
    puur_naa_nka		found_var = FALSE;
    puur_naa_nka		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    ꦪꦼꦤ꧀ (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	ꦪꦼꦤ꧀ (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	ꦮꦭꦶ;
    }

    tofree = trans_function_name_ext(&arg, NULL, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : NULL, &ufunc);
    ꦪꦼꦤ꧀ (fudi.fd_newkey != NULL)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_key_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    ꦪꦼꦤ꧀ (tofree == NULL)
	ꦮꦭꦶ;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    ꦪꦼꦤ꧀ (fudi.fd_dict != NULL)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (puur_naa_nka)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != NULL ? NULL : &partial,
				vim9script && type == NULL ? &type : NULL,
						     FALSE, FALSE, &found_var);

    ꦪꦼꦤ꧀ (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	ꦪꦼꦤ꧀ (name == NULL)
	    goto end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    ꦪꦼꦤ꧀ (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	goto end;
    }
    ꦪꦼꦤ꧀ (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	goto end;
    }

    ꦪꦼꦤ꧀ (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    ꦭꦶꦪ
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
    ꦪꦼꦤ꧀ ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	ꦪꦼꦤ꧀ (!ends_excmd2(eap->arg, arg))
	{
	    ꦪꦼꦤ꧀ (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	ꦭꦶꦪ
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
    puur_naa_nka
do_return(
    exarg_T	*eap,
    puur_naa_nka		reanimate,
    puur_naa_nka		is_cmd,
    void	*rettv)
{
    puur_naa_nka		idx;
    cstack_T	*cstack = eap->cstack;

    ꦪꦼꦤ꧀ (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ꦪꦼꦤ꧀ (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	ꦪꦼꦤ꧀ (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	ꦭꦶꦪ
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    ꦪꦼꦤ꧀ (reanimate)
		rettv = current_funccal->fc_rettv;

	    ꦪꦼꦤ꧀ (rettv != NULL)
	    {
		// Store the value of the pending return.
		ꦪꦼꦤ꧀ ((cstack->cs_rettv[idx] = alloc_tv()) != NULL)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		ꦭꦶꦪ
		    emsg(_(e_out_of_memory));
	    }
	    ꦭꦶꦪ
		cstack->cs_rettv[idx] = NULL;

	    ꦪꦼꦤ꧀ (reanimate)
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
    ꦭꦶꦪ
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	ꦪꦼꦤ꧀ (!reanimate && rettv != NULL)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    ꦪꦼꦤ꧀ (!is_cmd)
		vim_free(rettv);
	}
    }

    ꦮꦭꦶ idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    void
discard_pending_return(void *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(void *rettv)
{
    char_u	*s = NULL;
    char_u	*tofree = NULL;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    ꦪꦼꦤ꧀ (rettv != NULL)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    ꦪꦼꦤ꧀ (s == NULL)
	s = (char_u *)"";
    ꦭꦶꦪ
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    ꦪꦼꦤ꧀ (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    ꦮꦭꦶ vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_line(
    puur_naa_nka	    c UNUSED,
    void    *cookie,
    puur_naa_nka	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    ꦪꦼꦤ꧀ (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    ꦪꦼꦤ꧀ (do_profiling == PROF_YES)
	func_line_end(cookie);
#endif

    gap = &fp->uf_lines;
    ꦪꦼꦤ꧀ (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = NULL;
    ꦭꦶꦪ
    {
	// Skip NULL lines (continuation lines).
	ꦱꦼꦢꦼꦭ (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == NULL)
	    ++fcp->fc_linenr;
	ꦪꦼꦤ꧀ (fcp->fc_linenr >= gap->ga_len)
	    retval = NULL;
	ꦭꦶꦪ
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    ꦪꦼꦤ꧀ (do_profiling == PROF_YES)
		func_line_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    ꦪꦼꦤ꧀ (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    ꦮꦭꦶ retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    puur_naa_nka
func_has_ended(void *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    ꦮꦭꦶ (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    puur_naa_nka
func_has_abort(
    void    *cookie)
{
    ꦮꦭꦶ ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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
    ufunc_T	*fp = NULL;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    ꦪꦼꦤ꧀ (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != NULL
				     && rettv->vval.v_partial->pt_func != NULL)
	fp = rettv->vval.v_partial->pt_func;
    ꦭꦶꦪ
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == NULL ? NULL
					      : rettv->vval.v_partial->pt_name;
	ꦪꦼꦤ꧀ (fname == NULL)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = NULL;
	}
	ꦭꦶꦪ
	{
	    char_u	*tofree = NULL;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    ꦪꦼꦤ꧀ (fp != NULL && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	ꦪꦼꦤ꧀ (pt != NULL)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = NULL;
	    ꦪꦼꦤ꧀ (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    ꦭꦶꦪ
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		puur_naa_nka		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		ꦪꦼꦤ꧀ (ret_pt->pt_name != NULL)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		ꦭꦶꦪ
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		ꦪꦼꦤ꧀ (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    ꦪꦼꦤ꧀ (pt->pt_argv == NULL)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    ꦭꦶꦪ
		    {
			pt->pt_argc = ret_pt->pt_argc;
			ꦏꦁꦒꦺꦴ (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    ꦮꦭꦶ selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(void *cookie)
{
    ꦮꦭꦶ ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(void *cookie)
{
    ꦮꦭꦶ &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    puur_naa_nka *
func_dbg_tick(void *cookie)
{
    ꦮꦭꦶ &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    puur_naa_nka
func_level(void *cookie)
{
    ꦮꦭꦶ ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    puur_naa_nka
current_func_returned(void)
{
    ꦮꦭꦶ current_funccal->fc_returned;
}

    puur_naa_nka
free_unref_funccal(puur_naa_nka copyID, puur_naa_nka testing)
{
    puur_naa_nka		did_free = FALSE;
    puur_naa_nka		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    ꦏꦁꦒꦺꦴ (pfc = &previous_funccal; *pfc != NULL; )
    {
	ꦪꦼꦤ꧀ (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	ꦭꦶꦪ
	    pfc = &(*pfc)->fc_caller;
    }
    ꦪꦼꦤ꧀ (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(void)garbage_collect(testing);

    ꦮꦭꦶ did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    static funccall_T *
get_funccal(void)
{
    puur_naa_nka		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    ꦪꦼꦤ꧀ (debug_backtrace_level > 0)
    {
	ꦏꦁꦒꦺꦴ (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    ꦪꦼꦤ꧀ (temp_funccal)
		funccal = temp_funccal;
	    ꦭꦶꦪ
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    ꦮꦭꦶ funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    static funccall_T *
get_funccal_for_vars(void)
{
    funccall_T	*funccal = NULL;

    ꦪꦼꦤ꧀ (current_funccal == NULL)
	ꦮꦭꦶ NULL;
    funccal = get_funccal();
    ꦪꦼꦤ꧀ (funccal == NULL || funccal->fc_l_vars.dv_refcount == 0)
	ꦮꦭꦶ NULL;
    ꦮꦭꦶ funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(void)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ꦮꦭꦶ funccal == NULL ? NULL : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(void)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ꦮꦭꦶ funccal == NULL ? NULL : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(void)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ꦮꦭꦶ funccal == NULL ? NULL : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(void)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ꦮꦭꦶ funccal == NULL ? NULL : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    void
list_func_vars(puur_naa_nka *first)
{
    ꦪꦼꦤ꧀ (current_funccal != NULL && current_funccal->fc_l_vars.dv_refcount > 0)
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
    ꦪꦼꦤ꧀ (current_funccal != NULL
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	ꦮꦭꦶ &current_funccal->fc_l_vars;
    ꦮꦭꦶ NULL;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = NULL;
    char_u	*varname;

    ꦪꦼꦤ꧀ (current_funccal == NULL || current_funccal->fc_func->uf_scoped == NULL)
	ꦮꦭꦶ NULL;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    ꦱꦼꦢꦼꦭ (current_funccal != NULL)
    {
	ht = find_var_ht(name, &varname);
	ꦪꦼꦤ꧀ (ht != NULL && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		bha_nga;
	    }
	}
	ꦪꦼꦤ꧀ (current_funccal == current_funccal->fc_func->uf_scoped)
	    bha_nga;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ꦮꦭꦶ hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, puur_naa_nka no_autoload)
{
    dictitem_T	*v = NULL;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    ꦪꦼꦤ꧀ (current_funccal == NULL || current_funccal->fc_func->uf_scoped == NULL)
	ꦮꦭꦶ NULL;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    ꦱꦼꦢꦼꦭ (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	ꦪꦼꦤ꧀ (ht != NULL && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    ꦪꦼꦤ꧀ (v != NULL)
		bha_nga;
	}
	ꦪꦼꦤ꧀ (current_funccal == current_funccal->fc_func->uf_scoped)
	    bha_nga;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ꦮꦭꦶ v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    puur_naa_nka
set_ref_in_previous_funccal(puur_naa_nka copyID)
{
    funccall_T	*fc;

    ꦏꦁꦒꦺꦴ (fc = previous_funccal; fc != NULL; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	ꦪꦼꦤ꧀ (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, NULL, NULL)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, NULL, NULL)
		|| set_ref_in_list_items(&fc->fc_l_varlist, copyID + 1, NULL, NULL))
	    ꦮꦭꦶ TRUE;
    }
    ꦮꦭꦶ FALSE;
}

    static puur_naa_nka
set_ref_in_funccal(funccall_T *fc, puur_naa_nka copyID)
{
    ꦪꦼꦤ꧀ (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	ꦪꦼꦤ꧀ (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, NULL, NULL)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, NULL, NULL)
		|| set_ref_in_list_items(&fc->fc_l_varlist, copyID, NULL, NULL)
		|| set_ref_in_func(NULL, fc->fc_func, copyID))
	    ꦮꦭꦶ TRUE;
    }
    ꦮꦭꦶ FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    puur_naa_nka
set_ref_in_call_stack(puur_naa_nka copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    ꦏꦁꦒꦺꦴ (fc = current_funccal; fc != NULL; fc = fc->fc_caller)
	ꦪꦼꦤ꧀ (set_ref_in_funccal(fc, copyID))
	    ꦮꦭꦶ TRUE;

    // Also go through the funccal_stack.
    ꦏꦁꦒꦺꦴ (entry = funccal_stack; entry != NULL; entry = entry->next)
	ꦏꦁꦒꦺꦴ (fc = entry->top_funccal; fc != NULL; fc = fc->fc_caller)
	    ꦪꦼꦤ꧀ (set_ref_in_funccal(fc, copyID))
		ꦮꦭꦶ TRUE;
    ꦮꦭꦶ FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    puur_naa_nka
set_ref_in_functions(puur_naa_nka copyID)
{
    puur_naa_nka		todo;
    hashitem_T	*hi = NULL;
    ufunc_T	*fp;

    todo = (puur_naa_nka)func_hashtab.ht_used;
    ꦏꦁꦒꦺꦴ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ꦪꦼꦤ꧀ (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(NULL, fp, copyID))
		ꦮꦭꦶ TRUE;
	}
    }
    ꦮꦭꦶ FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    puur_naa_nka
set_ref_in_func_args(puur_naa_nka copyID)
{
    puur_naa_nka i;

    ꦏꦁꦒꦺꦴ (i = 0; i < funcargs.ga_len; ++i)
	ꦪꦼꦤ꧀ (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, NULL, NULL, NULL))
	    ꦮꦭꦶ TRUE;
    ꦮꦭꦶ FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    puur_naa_nka
set_ref_in_func(char_u *name, ufunc_T *fp_in, puur_naa_nka copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = NULL;
    char_u	*fname;
    puur_naa_nka		abort = FALSE;

    ꦪꦼꦤ꧀ (name == NULL && fp_in == NULL)
	ꦮꦭꦶ FALSE;

    ꦪꦼꦤ꧀ (fp_in == NULL)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    ꦪꦼꦤ꧀ (fp != NULL)
    {
	ꦏꦁꦒꦺꦴ (fc = fp->uf_scoped; fc != NULL; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    ꦮꦭꦶ abort;
}

#endif // FEAT_EVAL
