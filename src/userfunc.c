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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
ସ୍ଥିର hashtab_T	func_hashtab;

// Used by get_func_tv()
ସ୍ଥିର garray_T funcargs = GA_EMPTY;

// pointer to funccal for currently active function
ସ୍ଥିର funccall_T *current_funccal = ଖାଲି;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
ସ୍ଥିର funccall_T *previous_funccal = ଖାଲି;

ସ୍ଥିର ଶୂନ୍ୟ funccal_unref(funccall_T *fc, ufunc_T *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force);
ସ୍ଥିର ଶୂନ୍ୟ func_clear(ufunc_T *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା func_free(ufunc_T *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force);
ସ୍ଥିର char_u *untrans_function_name(char_u *name);
ସ୍ଥିର ଶୂନ୍ୟ handle_defer_one(funccall_T *funccal);
ସ୍ଥିର char_u *trans_function_name_ext(char_u **pp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *is_global, ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    ଶୂନ୍ୟ
func_init(ଶୂନ୍ୟ)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ &func_hashtab;
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
    ସ୍ଥିର char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_vararg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_underscore = FALSE;

    ଯେତେବେଳେ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    ଯଦି (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == ଖାଲି
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	ଯଦି (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	ଫେରନ୍ତୁ arg;
    }

    // Extra checks in Vim9 script.
    ଯଦି (!skip && argtypes != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = *p;
	*p = NUL;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = check_reserved_name(arg, FALSE);
	*p = c;
	ଯଦି (r == FAIL)
	    ଫେରନ୍ତୁ arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	ଯଦି (check_defined(arg, p - arg,
				   evalarg == ଖାଲି ? ଖାଲି : evalarg->eval_cctx,
			       eap == ଖାଲି ? ଖାଲି : eap->cstack, TRUE) == FAIL)
	    ଫେରନ୍ତୁ arg;
    }

    ଯଦି (newargs != ଖାଲି && ga_grow(newargs, 1) == FAIL)
	ଫେରନ୍ତୁ arg;
    ଯଦି (newargs != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	ଯଦି (arg_copy == ଖାଲି)
	{
	    *p = c;
	    ଫେରନ୍ତୁ arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	ଯଦି (argtypes == ଖାଲି || !is_underscore)
	    // Check for duplicate argument name.
	    ପାଇଁ (i = 0; i < newargs->ga_len; ++i)
		ଯଦି (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    ଫେରନ୍ତୁ arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    ଯଦି (argtypes != ଖାଲି && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != ଖାଲି && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = ଖାଲି;

	ଯଦି (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == ଖାଲି ? arg : arg_copy);
	    p = skipwhite(p);
	}
	ଯଦି (*p == ':')
	{
	    ++p;
	    ଯଦି (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		ଫେରନ୍ତୁ arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    ଯଦି (!skip)
		type = vim_strnsave(type, p - type);
	}
	ଅନ୍ୟଥା ଯଦି (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == ଖାଲି ? arg : arg_copy);
	    ଫେରନ୍ତୁ arg;
	}
	ଯଦି (!skip)
	{
	    ଯଦି (type == ଖାଲି && types_optional)
	    {
		// lambda arguments default to "any" type
		ଯଦି (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		ଅନ୍ୟଥା
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    ଫେରନ୍ତୁ p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    ସ୍ଥିର char_u *
get_function_ଧାଡ଼ି(
	exarg_T		*eap,
	garray_T	*ଧାଡ଼ିଗୁଡ଼ିକ_to_free,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    ଯଦି (eap->ea_getline == ଖାଲି)
	theline = getcmdline(':', 0L, indent, 0);
    ଅନ୍ୟଥା
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    ଯଦି (theline != ଖାଲି)
    {
	ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ_to_free->ga_len > 0
		&& eap->cmdlinep != ଖାଲି
		&& *eap->cmdlinep == ((char_u **)ଧାଡ଼ିଗୁଡ଼ିକ_to_free->ga_data)
						   [ଧାଡ଼ିଗୁଡ଼ିକ_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(ଶୂନ୍ୟ)ga_add_string(ଧାଡ଼ିଗୁଡ଼ିକ_to_free, theline);
    }

    ଫେରନ୍ତୁ theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*varargs,
    garray_T	*default_args,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip,
    exarg_T	*eap,		// can be NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*ଧାଡ଼ିଗୁଡ଼ିକ_to_free)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		any_default = FALSE;
    char_u	*whitep = *argp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_ପଦସମଷ୍ଟି = FALSE;

    ଯଦି (newargs != ଖାଲି)
	ga_init2(newargs, ଆକାର(char_u *), 3);
    ଯଦି (argtypes != ଖାଲି)
	ga_init2(argtypes, ଆକାର(char_u *), 3);
    ଯଦି (arg_objm != ଖାଲି)
	ga_init2(arg_objm, ଆକାର(int8_T), 3);
    ଯଦି (!skip && default_args != ଖାଲି)
	ga_init2(default_args, ଆକାର(char_u *), 3);

    ଯଦି (varargs != ଖାଲି)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    ଯେତେବେଳେ (*p != endchar)
    {
	ଯେତେବେଳେ (eap != ଖାଲି && eap->ea_getline != ଖାଲି
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_ଧାଡ଼ି(eap, ଧାଡ଼ିଗୁଡ଼ିକ_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    ଯଦି (theline == ଖାଲି)
		ଭାଙ୍ଗ;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	ଯଦି (mustend && *p != endchar)
	{
	    ଯଦି (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    ଯାଅ err_ret;
	}
	ଯଦି (*p == endchar && !need_ପଦସମଷ୍ଟି)
	    ଭାଙ୍ଗ;

	ଯଦି (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    ଯଦି (varargs != ଖାଲି)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    ଯଦି (argtypes != ଖାଲି)
	    {
		// ...name: list<type>
		ଯଦି (!eval_isnamec1(*p))
		{
		    ଯଦି (!skip)
			emsg(_(e_missing_name_after_dots));
		    ଯାଅ err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		ଯଦି (p == arg)
		    ଭାଙ୍ଗ;
		ଯଦି (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    ଭାଙ୍ଗ;
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    ଯେତେବେଳେ (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    ଯଦି (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		ଯଦି (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_ମୂଲ୍ୟ_must_be_vnone_str), p);
		    ଯାଅ err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		ଯଦି (ga_grow(default_args, 1) == FAIL)
		    ଯାଅ err_ret;

		char_u *ପଦସମଷ୍ଟି = vim_strnsave((char_u *)"v:none", 6);
		ଯଦି (ପଦସମଷ୍ଟି == ଖାଲି)
		    ଯାଅ err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = ପଦସମଷ୍ଟି;
		default_args->ga_len++;
	    }
	    ଅନ୍ୟଥା ଯଦି (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		ଯାଅ err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    ଯଦି (newargs != ଖାଲି && ga_grow(newargs, 1) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (newargs != ଖାଲି)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		ଯଦି (argtypes != ଖାଲି && ga_grow(argtypes, 1) == OK
			    && arg_objm != ଖାଲି && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    ଯଦି (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			ଯଦି (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			ଯଦି (assignment != ଖାଲି)
			{
			    c = *argend;
			    *argend = NUL;
			    ଯଦି (any_default)
				vim_snprintf((ଅକ୍ଷର *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    ଅନ୍ୟଥା
				vim_snprintf((ଅକ୍ଷର *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    ଯଦି (*p == ',')
		++p;
	}
	ଅନ୍ୟଥା
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    ଯଦି (p == arg)
		ଭାଙ୍ଗ;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    ଯଦି (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != ଖାଲି)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *ପଦସମଷ୍ଟି = p;
		ଯଦି (eval1(&p, &rettv, ଖାଲି) != FAIL)
		{
		    ଯଦି (!skip)
		    {
			ଯଦି (ga_grow(default_args, 1) == FAIL)
			    ଯାଅ err_ret;

			ଯଦି (need_ପଦସମଷ୍ଟି)
			    need_ପଦସମଷ୍ଟି = FALSE;
			// trim trailing whitespace
			ଯେତେବେଳେ (p > ପଦସମଷ୍ଟି && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			ପଦସମଷ୍ଟି = vim_strsave(ପଦସମଷ୍ଟି);
			ଯଦି (ପଦସମଷ୍ଟି == ଖାଲି)
			{
			    *p = c;
			    ଯାଅ err_ret;
			}
			((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = ପଦସମଷ୍ଟି;
			default_args->ga_len++;
			*p = c;
		    }
		}
		ଅନ୍ୟଥା
		{
		    mustend = TRUE;
		    ଯଦି (*skipwhite(p) == NUL)
			need_ପଦସମଷ୍ଟି = TRUE;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		ଯାଅ err_ret;
	    }

	    ଯଦି (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		ଯଦି (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    ଯାଅ err_ret;
		}
		p = skipwhite(p);
	    }
	    ଯଦି (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		ଯଦି (!skip && argtypes != ଖାଲି
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    ଯାଅ err_ret;
		}
	    }
	    ଅନ୍ୟଥା
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    ଯଦି (*p != endchar)
	ଯାଅ err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    ଫେରନ୍ତୁ OK;

err_ret:
    ଯଦି (newargs != ଖାଲି)
	ga_clear_strings(newargs);
    ଯଦି (!skip && default_args != ଖାଲି)
	ga_clear_strings(default_args);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		obj_member_ଗଣନା,
    cctx_T	*cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = 0;

    ga_init2(&fp->uf_type_list, ଆକାର(type_T *), 10);
    ଯଦି (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	ଯଦି (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	ଯଦି (fp->uf_arg_types != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
	    type_T	*type;

	    ପାଇଁ (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		ଯଦି (p == ଖାଲି)
		    // will get the type from the default value
		    type = &t_unknown;
		ଅନ୍ୟଥା
		{
		    ଯଦି (arg_objm != ଖାଲି && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା om = 0; om < obj_member_ଗଣନା; ++om)
			{
			    ଯଦି (obj_members != ଖାଲି
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				ଭାଙ୍ଗ;
			    }
			}
		    }
		    ଅନ୍ୟଥା
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		ଯଦି (type == ଖାଲି || !valid_declaration_type(type))
		    ଫେରନ୍ତୁ FAIL;
		fp->uf_arg_types[i] = type;
		ଯଦି (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    ଯଦି (obj_members != ଖାଲି && *name == '_')
			// protected object method
			name++;

		    ଯଦି (var_wrong_func_name(name, TRUE))
			ଫେରନ୍ତୁ FAIL;
		}
	    }
	}
    }

    ଯଦି (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = ଖାଲି;
	p = ((char_u **)argtypes->ga_data)[len];
	ଯଦି (p == ଖାଲି)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	ଅନ୍ୟଥା
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    ଯଦି (fp->uf_va_type != ଖାଲି && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଯଦି (fp->uf_va_type == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    ଯଦି (ret_type == ଖାଲି)
	fp->uf_ret_type = &t_void;
    ଅନ୍ୟଥା
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	ଯଦି (fp->uf_ret_type == ଖାଲି)
	{
	    fp->uf_ret_type = &t_void;
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
register_closure(ufunc_T *fp)
{
    ଯଦି (fp->uf_scoped == current_funccal)
	// no change
	ଫେରନ୍ତୁ OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    ଯଦି (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    ଫେରନ୍ତୁ OK;
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

    ଯଦି (name[0] != K_SPECIAL)
	ଫେରନ୍ତୁ name;
    len = STRLEN(name);
    ଯଦି (len + 3 > bufsize)
	ଫେରନ୍ତୁ name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    ଫେରନ୍ତୁ buf;
}

ସ୍ଥିର char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lambda_no = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    string_T	ret;

    n = vim_snprintf((ଅକ୍ଷର *)lambda_name, ଆକାର(lambda_name), "<lambda>%d", ++lambda_no);
    ଯଦି (n < 1)
	ret.length = 0;
    ଅନ୍ୟଥା ଯଦି (n >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(lambda_name))
	ret.length = ଆକାର(lambda_name) - 1;
    ଅନ୍ୟଥା
	ret.length = (size_t)n;

    ret.string = lambda_name;
    ଫେରନ୍ତୁ ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    ସ୍ଥିର ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < ଆକାର(ufunc_T) ? ଆକାର(ufunc_T) : len);
    ଯଦି (fp != ଖାଲି)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((ଶୂନ୍ୟ *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	ଯଦି (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    ଯଦି (fp->uf_name_exp != ଖାଲି)
		vim_snprintf((ଅକ୍ଷର *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    ଫେରନ୍ତୁ fp;
}

#ଯଦି defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
register_cfunc(cfunc_T cb, cfunc_free_T cb_free, ଶୂନ୍ୟ *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.length);
    ଯଦି (fp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

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

    ଫେରନ୍ତୁ name.string;
}
#endif

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    ସ୍ଥିର char_u *
skip_arrow(
	char_u	*start,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	equal_arrow,
	char_u	**ret_type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    ଯଦି (equal_arrow)
    {
	ଯଦି (*s == ':')
	{
	    ଯଦି (white_error != ଖାଲି && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    ଯଦି (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
	bef = s;
	s = skipwhite(s);
	ଯଦି (*s != '=')
	    ଫେରନ୍ତୁ ଖାଲି;
	++s;
    }
    ଯଦି (*s != '>')
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (white_error != ଖାଲି && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    ଯଦି (checkforcmd(&p, "function", 2))
    {
	ଯଦି (*p == '(')
	    ଫେରନ୍ତୁ FALSE;
	*cmd = p;
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    ଯଦି (cstack == ଖାଲି || cstack->cs_idx < 0)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2 = cstack->cs_idx + 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    fp->uf_block_ids = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଗଣନା_2);
    ଯଦି (fp->uf_block_ids != ଖାଲି)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * ଗଣନା_2);
	fp->uf_block_depth = ଗଣନା_2;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    ପାଇଁ (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    ସ୍ଥିର char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    ଯେତେବେଳେ (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	ଯଦି (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	ଅନ୍ୟଥା ଯଦି (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	    MB_PTR_ADV(p);
    }

    ଫେରନ୍ତୁ p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    ସ୍ଥିର char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    ଯେତେବେଳେ (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	ଯଦି (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	ଅନ୍ୟଥା ଯଦି (*p == '"')
	{
	    p++;    // skip the ending double quote
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	    MB_PTR_ADV(p);
    }

    ଫେରନ୍ତୁ p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    ସ୍ଥିର char_u *
find_start_of_vim9_comment(char_u *ଧାଡ଼ି)
{
    char_u	*p = ଧାଡ଼ି;

    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	ଅନ୍ୟଥା ଯଦି (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	ଅନ୍ୟଥା
	{
	    ଯଦି (*p == '#')
		// Found the start of a Vim9 comment
		ଭାଙ୍ଗ;
	    MB_PTR_ADV(p);
	}
    }

    ଫେରନ୍ତୁ p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *ଧାଡ଼ି_arg_in,
	garray_T    *ଧାଡ଼ିଗୁଡ଼ିକ_to_free)
{
    linenr_T	sourcing_lnum_ଶୀର୍ଷ = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_wait_return = need_wait_return;
    char_u	*ଧାଡ଼ି_arg = ଧାଡ଼ି_arg_in;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    ଅକ୍ଷର	nesting_def[MAX_FUNC_NESTING];
    ଅକ୍ଷର	nesting_inline[MAX_FUNC_NESTING];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nesting = 0;
    getline_opt_T getline_options;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent = 2;
    char_u	*skip_until = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_heredoc = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = ଖାଲି;
    size_t	heredoc_trimmedlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    ଯଦି (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	ଯଦି (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    ଯାଅ theend;
	ଯେତେବେଳେ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = ଖାଲି;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    ପାଇଁ (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	ଯଦି (ଚାବି_Typed)
	{
	    ସନ୍ଦେଶ_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	ଯଦି (ଧାଡ଼ି_arg != ଖାଲି)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = ଧାଡ଼ି_arg;
	    p = vim_strchr(theline, '\n');
	    ଯଦି (p == ଖାଲି)
		ଧାଡ଼ି_arg += STRLEN(ଧାଡ଼ି_arg);
	    ଅନ୍ୟଥା
	    {
		*p = NUL;
		ଧାଡ଼ି_arg = p + 1;
	    }
	}
	ଅନ୍ୟଥା
	{
	    theline = get_function_ଧାଡ଼ି(eap, ଧାଡ଼ିଗୁଡ଼ିକ_to_free, indent,
							      getline_options);
	}
	ଯଦି (ଚାବି_Typed)
	    ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = Rows - 1;
	ଯଦି (theline == ଖାଲି)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_ଶୀର୍ଷ;
	    ଯଦି (skip_until != ଖାଲି)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    ଅନ୍ୟଥା ଯଦି (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    ଅନ୍ୟଥା
		emsg(_(e_missing_endfunction));
	    ଯାଅ theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	ଯଦି (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	ଅନ୍ୟଥା
	    sourcing_lnum_off = 0;

	ଯଦି (skip_until != ଖାଲି)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    ଯଦି (heredoc_trimmed == ଖାଲି
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		ଯଦି (heredoc_trimmed == ଖାଲି)
		    p = theline;
		ଅନ୍ୟଥା ଯଦି (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		ଅନ୍ୟଥା
		    p = theline + heredoc_trimmedlen;
		ଯଦି (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    ଯଦି (heredoc_concat_len > 0)
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
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    ପାଇଁ (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		ଯଦି (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    ଯଦି (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		ଯଦି (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		ଯଦି (nesting-- == 0)
		{
		    char_u *nextcmd = ଖାଲି;

		    ଯଦି (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି_arg != ଖାଲି && *skipwhite(ଧାଡ଼ି_arg) != NUL)
			nextcmd = ଧାଡ଼ି_arg;
		    ଅନ୍ୟଥା ଯଦି (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_ଶୀର୍ଷ
							+ newlines->ga_len + 1;
			ଯଦି (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			ଅନ୍ୟଥା
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    ଯଦି (nextcmd != ଖାଲି && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)ଧାଡ଼ିଗୁଡ଼ିକ_to_free->ga_data)
						   [ଧାଡ଼ିଗୁଡ଼ିକ_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)ଧାଡ଼ିଗୁଡ଼ିକ_to_free->ga_data)
						   [ଧାଡ଼ିଗୁଡ଼ିକ_to_free->ga_len - 1];
			    --ଧାଡ଼ିଗୁଡ଼ିକ_to_free->ga_len;
			}
		    }
		    ଭାଙ୍ଗ;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    ଅନ୍ୟଥା ଯଦି (nesting_def[nesting])
	    {
		ଯଦି (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    ଯଦି (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    ଯଦି (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		ଯଦି (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, ଖାଲି, TRUE, 0));
		ଯଦି (vim9_function && *p == '<')
		{
		    // skip generic function
		    ଯଦି (skip_generic_func_type_args(&p) == FAIL)
			ଯାଅ theend;
		}
		ଯଦି (*skipwhite(p) == '(')
		{
		    ଯଦି (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    ଅନ୍ୟଥା
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    ଯଦି (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		ଯଦି (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		ଅନ୍ୟଥା
		    end = p + STRLEN(p) - 1;

		ଯେତେବେଳେ (end > p && VIM_ISWHITE(*end))
		    --end;
		ଯଦି (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    ଯେତେବେଳେ (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    ଯଦି (!is_block)
			is_block = find_cmd_block_start(p) != ଖାଲି;

		    ଯଦି (is_block)
		    {
			ଯଦି (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			ଅନ୍ୟଥା
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
	    ଯଦି (!vim9_function || colon)
		p = skip_range(p, FALSE, ଖାଲି);
	    char_u *tp = p;
	    ଯଦି (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    ଅନ୍ୟଥା
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    ଯଦି (arg[0] == '<' && arg[1] =='<'
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
		ଯଦି (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    ଯଦି (heredoc_trimmed == ଖାଲି)
			heredoc_trimmedlen = 0;
		}
		ଯଦି (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		ଅନ୍ୟଥା
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		ଯଦି (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    ଯଦି (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		ଯଦି (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_sc_version = current_sctx.sc_version;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_ଗଣନା = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_ଗଣନା, &semicolon,
									 TRUE);
		    ଯଦି (arg != ଖାଲି)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    ଯଦି (arg != ଖାଲି && STRNCMP(arg, "=<<", 3) == 0)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_trim = FALSE;

			p = skipwhite(arg + 3);
			ଯେତେବେଳେ (TRUE)
			{
			    ଯଦି (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				ଜାରି;
			    }
			    ଯଦି (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				ଜାରି;
			    }
			    ଭାଙ୍ଗ;
			}
			ଯଦି (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    ଯଦି (heredoc_trimmed == ଖାଲି)
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
	ଯଦି (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    ଯାଅ theend;

	ଯଦି (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	ଅନ୍ୟଥା
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	ଯଦି (p == ଖାଲି)
	    ଯାଅ theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	ଯେତେବେଳେ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = ଖାଲି;

	// Check for end of eap->arg.
	ଯଦି (ଧାଡ଼ି_arg != ଖାଲି && *ଧାଡ଼ି_arg == NUL)
	    ଧାଡ଼ି_arg = ଖାଲି;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    ଯଦି (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    ଫେରନ୍ତୁ ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = ଖାଲି;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum_save = -1;
    linenr_T	sourcing_lnum_ଶୀର୍ଷ = SOURCING_LNUM;
    char_u	*ଧାଡ଼ି_arg = ଖାଲି;

    *arg = skipwhite(*arg + 1);
    ଯଦି (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	ଫେରନ୍ତୁ FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    ଯଦି (**arg == '\n')
	ଧାଡ଼ି_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    ଯଦି (evalarg->eval_cctx != ଖାଲି)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    ଅନ୍ୟଥା
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, ଆକାର(char_u *), 10);
    ଯଦି (get_function_body(&eap, &newlines, ଧାଡ଼ି_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	ଯାଅ erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_ଗଣନା += newlines.ga_len;
    ଯଦି (gap->ga_itemsize > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	ପାଇଁ (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    ଯଦି (p == ଖାଲି)
		// comment line in the lambda body
		ଜାରି;

	    p = skipwhite(p);

	    ଯଦି (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		ଯାଅ erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    ଯଦି (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    ଅନ୍ୟଥା
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    ଯଦି (pnl != ଖାଲି)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	ଯଦି (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    ଯାଅ erret;
	ଯଦି (eap.nextcmd != ଖାଲି)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	ଅନ୍ୟଥା
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	ଯଦି (pnl != ଖାଲି)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    ଯଦି (eap.nextcmd != ଖାଲି)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	ଯଦି (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    ଅନ୍ୟଥା
	*arg = (char_u *)"";

    ଯଦି (!evaluate)
    {
	ret = OK;
	ଯାଅ erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    ଯଦି (ufunc == ଖାଲି)
	ଯାଅ erret;
    ଯଦି (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	ଯାଅ erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = ଖାଲି;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = ଖାଲି;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_ଶୀର୍ଷ;

    // parse argument types, the type variables of the function being
    // compiled may be used
    ଯଦି (parse_argument_types(ufunc, argtypes, varargs, ଖାଲି, ଖାଲି, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	ଯାଅ erret;
    }

    // parse the return type, if any
    ଯଦି (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	ଯାଅ erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    ଯଦି (pt == ଖାଲି)
	ଯାଅ erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ = newlines;
    newlines.ga_data = ଖାଲି;
    ଯଦି (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    ଯଦି (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_ଶୀର୍ଷ;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = ଖାଲି;
    ret = OK;

erret:
    ଯଦି (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    ଯଦି (newargs != ଖାଲି)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    ଯଦି (ufunc != ଖାଲି)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    ଫେରନ୍ତୁ ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg != ଖାଲି
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = ଖାଲି;
    partial_T   *pt = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		varargs;
    char_u	*ret_type = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    char_u	*s;
    char_u	*start, *end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*old_eval_lavars = eval_lavars_used;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eval_lavars = FALSE;
    char_u	*tofree2 = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		equal_arrow = **arg == '(';
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		white_error = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_start = called_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    ଯଦି (equal_arrow && !vim9script)
	ଫେରନ୍ତୁ NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', ଖାଲି,
	    types_optional ? &argtypes : ଖାଲି, types_optional,
			types_optional ? &arg_objm : ଖାଲି, evalarg,
			ଖାଲି, &default_args, TRUE, ଖାଲି, FALSE, ଖାଲି, ଖାଲି);
    ଯଦି (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, ଖାଲି) == ଖାଲି)
    {
	ଯଦି (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ଫେରନ୍ତୁ called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    ଯଦି (evaluate)
	pnewargs = &newargs;
    ଅନ୍ୟଥା
	pnewargs = ଖାଲି;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : ଖାଲି, types_optional,
			types_optional ? &arg_objm : ଖାଲି, evalarg,
					    &varargs, &default_args,
					    FALSE, ଖାଲି, FALSE, ଖାଲି, ଖାଲି);
    ଯଦି (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : ଖାଲି)) == ଖାଲି)
    {
	ଯଦି (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	ଫେରନ୍ତୁ white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    ଯଦି (ret_type != ଖାଲି)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    ଯଦି (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    ଯଦି (equal_arrow && **arg == '{')
    {
	ଯଦି (evalarg == ଖାଲି)
	    // cannot happen?
	    ଯାଅ theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	ଯଦି (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : ଖାଲି, varargs,
			   &default_args, ret_type) == FAIL)
	    ଯାଅ errret;
	ଯାଅ theend;
    }
    ଯଦି (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_ମୂଲ୍ୟଗୁଡ଼ିକ_in_lambda));
	ଯାଅ errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_ପଦସମଷ୍ଟି_concatenate(arg, &start, &end, evalarg);
    ଯଦି (ret == FAIL)
	ଯାଅ errret;

    ଯଦି (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ଯଦି (**arg != '}')
	{
	    semsg(_(e_expected_ଡାହାଣ_curly_str), *arg);
	    ଯାଅ errret;
	}
	++*arg;
    }

    ଯଦି (evaluate)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *ଧାଡ଼ି_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	ଯଦି (fp == ଖାଲି)
	    ଯାଅ errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	ଯଦି (pt == ଖାଲି)
	    ଯାଅ errret;

	ga_init2(&newlines, ଆକାର(char_u *), 1);
	ଯଦି (ga_grow(&newlines, 1) == FAIL)
	    ଯାଅ errret;

	// If there are line breaks, we need to split up the string.
	ଧାଡ଼ି_end = vim_strchr(start, '\n');
	ଯଦି (ଧାଡ଼ି_end == ଖାଲି || ଧାଡ଼ି_end > end)
	    ଧାଡ଼ି_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ଧାଡ଼ି_end - start) + 1;
	p = alloc(len);
	ଯଦି (p == ଖାଲି)
	    ଯାଅ errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, ଧାଡ଼ି_end - start);

	ଯଦି (ଧାଡ଼ି_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    ଯେତେବେଳେ (ଧାଡ଼ି_end < end && *ଧାଡ଼ି_end == '\n')
	    {
		ଯଦି (ga_grow(&newlines, 1) == FAIL)
		    ଯାଅ errret;
		start = ଧାଡ଼ି_end + 1;
		ଧାଡ଼ି_end = vim_strchr(start, '\n');
		ଯଦି (ଧାଡ଼ି_end == ଖାଲି || ଧାଡ଼ି_end > end)
		    ଧାଡ଼ି_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, ଧାଡ଼ି_end - start);
	    }
	}

	ଯଦି (strstr((ଅକ୍ଷର *)p + 7, "a:") == ଖାଲି)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	ଯଦି (types_optional)
	{
	    ଯଦି (parse_argument_types(fp, &argtypes,
				vim9script && varargs, ଖାଲି, ଖାଲି, 0,
				cctx) == FAIL)
		ଯାଅ errret;
	    ଯଦି (ret_type != ଖାଲି)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     ଖାଲି, cctx, TRUE);
		ଯଦି (fp->uf_ret_type == ଖାଲି)
		    ଯାଅ errret;
	    }
	    ଅନ୍ୟଥା
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ = newlines;
	ଯଦି (current_funccal != ଖାଲି && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    ଯଦି (register_closure(fp) == FAIL)
		ଯାଅ errret;
	}

#ifdef FEAT_PROFILE
	ଯଦି (prof_def_func())
	    func_do_profile(fp);
#endif
	ଯଦି (sandbox)
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
    ଯଦି (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    ଫେରନ୍ତୁ OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    ଯଦି (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    ଯଦି (fp != ଖାଲି)
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
    ଫେରନ୍ତୁ FAIL;
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    no_autoload,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    new_function,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    char_u	*s = ଖାଲି;
    hashtab_T	*ht;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_type = FALSE;

    ଯଦି (partialp != ଖାଲି)
	*partialp = ଖାଲି;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    ଯଦି (v != ଖାଲି)
    {
	tv = &v->di_tv;
    }
    ଅନ୍ୟଥା ଯଦି (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = *lenp;

	ଯଦି (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	ଯଦି (import != ଖାଲି)
	{
	    name[len] = NUL;
	    ଯଦି (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    ଅନ୍ୟଥା
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    ଫେରନ୍ତୁ (char_u *)"";	// just in case
	}
    }

    ଯଦି (tv != ଖାଲି)
    {
	ଯଦି (found_var != ଖାଲି)
	    *found_var = TRUE;
	ଯଦି (tv->v_type == VAR_FUNC)
	{
	    ଯଦି (tv->vval.v_string == ଖାଲି)
	    {
		*lenp = 0;
		ଫେରନ୍ତୁ (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    ଯଦି (p != ଖାଲି && generic_func_find_open_bracket(name) != ଖାଲି)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		ଫେରନ୍ତୁ (char_u *)"";
	    }
	    ଯଦି (p != ଖାଲି)
		*lenp = p - s;
	    ଅନ୍ୟଥା
		*lenp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);
	}
	ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    ଯଦି (pt == ଖାଲି)
	    {
		*lenp = 0;
		ଫେରନ୍ତୁ (char_u *)"";	// just in case
	    }
	    ଯଦି (partialp != ଖାଲି)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);
	}

	ଯଦି (s != ଖାଲି)
	{
	    ଯଦି (!did_type && type != ଖାଲି && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		ଯଦି (sv != ଖାଲି)
		    *type = sv->sv_type;
	    }
	    ଫେରନ୍ତୁ s;
	}
    }

    ଫେରନ୍ତୁ name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    ଶୂନ୍ୟ
emsg_funcname(ଅକ୍ଷର *ermsg, char_u *name)
{
    char_u	*p = name;

    ଯଦି (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    ଯଦି (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    partial_argc,
	typval_T    *argvars,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *argcount,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_builtin)
{
    char_u	*argp = *arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg == ଖାଲି
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ଯେତେବେଳେ (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	ଯଦି (*argp == ')' || *argp == ',' || *argp == NUL)
	    ଭାଙ୍ଗ;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_idx = *argcount;
	ଯଦି (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    ଭାଙ୍ଗ;
	}
	++*argcount;
	ଯଦି (!is_builtin && check_typval_is_ମୂଲ୍ୟ(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    ଭାଙ୍ଗ;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	ଯଦି (vim9script)
	{
	    ଯଦି (*argp != ',' && *skipwhite(argp) == ',')
	    {
		ଯଦି (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା
	    argp = skipwhite(argp);
	ଯଦି (*argp != ',')
	    ଭାଙ୍ଗ;
	ଯଦି (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    ଯଦି (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    ଭାଙ୍ଗ;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    ଯଦି (*argp == ')')
	++argp;
    ଅନ୍ୟଥା
	ret = FAIL;
    *arg = argp;
    ଫେରନ୍ତୁ ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_func_tv(
    char_u	*name,		// name of the function
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = 0;			// number of arguments found
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg == ଖାଲି
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == ଖାଲି ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    ଯଦି (ret == OK)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_emsg_before = did_emsg;

	ଯଦି (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    ଯଦି (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, ଆକାର(typval_T *), 50);
	    ପାଇଁ (i = 0; i < argcount; ++i)
		ଯଦି (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	ଯଦି (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    ଅନ୍ୟଥା ଯଦି (!aborting() && evaluate)
    {
	ଯଦି (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	ଅନ୍ୟଥା
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    ଯେତେବେଳେ (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    ଯଦି (vim9script)
	*arg = argp;
    ଅନ୍ୟଥା
	*arg = skipwhite(argp);
    ଫେରନ୍ତୁ ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_fname_sid(char_u *p)
{
    ଫେରନ୍ତୁ (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    ଯଦି (script_name == name)
	ଫେରନ୍ତୁ name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR;
    fname_buflen = 3;
    ଯଦି (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    ଅନ୍ୟଥା
    {
	ଯଦି (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	ଅନ୍ୟଥା
	{
	    fname_buflen += vim_snprintf((ଅକ୍ଷର *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(long)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    ଯଦି (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    ଅନ୍ୟଥା
    {
	fname = alloc(fnamelen + 1);
	ଯଦି (fname == ଖାଲି)
	    *error = FCERR_OTHER;
	ଅନ୍ୟଥା
	{
	    *tofree = fname;
	    vim_snprintf((ଅକ୍ଷର *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    ଫେରନ୍ତୁ fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    ଶୂନ୍ୟ
func_name_with_sid(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR;
    vim_snprintf((ଅକ୍ଷର *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    ସ୍ଥିର ufunc_T *
find_func_with_sid(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    ଯଦି (!SCRIPT_ID_VALID(sid))
	ଫେରନ୍ତୁ ଖାଲି;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    ଯଦି (!HASHITEM_EMPTY(hi))
	ଫେରନ୍ତୁ HI2UF(hi);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    ସ୍ଥିର ufunc_T *
find_func_with_prefix(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    ଯଦି (vim_strchr(name, AUTOLOAD_CHAR) != ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;	// already has the prefix
    ଯଦି (!SCRIPT_ID_VALID(sid))
	ଫେରନ୍ତୁ ଖାଲି;	// not in a script
    si = SCRIPT_ITEM(sid);
    ଯଦି (si->sn_autoload_prefix != ଖାଲି)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	ଯଦି (namep == ଖାଲି)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	ଯଦି (len < ଆକାର(buffer))
	    auto_name = buffer;
	ଅନ୍ୟଥା
	    auto_name = alloc(len);
	ଯଦି (auto_name != ଖାଲି)
	{
	    vim_snprintf((ଅକ୍ଷର *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    ଯଦି (auto_name != buffer)
		vim_free(auto_name);
	    ଯଦି (!HASHITEM_EMPTY(hi))
		ଫେରନ୍ତୁ HI2UF(hi);
	}
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    ଯଦି ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	ଯଦି (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    ଯଦି (func != ଖାଲି)
		ଫେରନ୍ତୁ func;
	}
	ଯଦି (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    ଯଦି (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sid);
		ଯଦି (func != ଖାଲି)
		    ଫେରନ୍ତୁ func;
	    }
	}
    }

    ଯଦି ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	ଯଦି (!HASHITEM_EMPTY(hi))
	    ଫେରନ୍ତୁ HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    ଫେରନ୍ତୁ find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    ଯଦି (fp != ଖାଲି && (fp->uf_flags & FC_DEAD) == 0)
	ଫେରନ୍ତୁ fp;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_is_global(ufunc_T *ufunc)
{
    ଫେରନ୍ତୁ ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_requires_g_prefix(ufunc_T *ufunc)
{
    ଫେରନ୍ତୁ func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ଖାଲି
	    && !SAFE_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;

    ଯଦି (!func_is_global(fp))
	len = vim_snprintf((ଅକ୍ଷର *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    ଅନ୍ୟଥା
	len = vim_snprintf((ଅକ୍ଷର *)buf, bufsize, "%s", fp->uf_name);

    ଫେରନ୍ତୁ (len >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)bufsize) ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    ଅକ୍ଷର	*name,
    varnumber_T nr)
{
    STRCPY(v->di_ଚାବି, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_add(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_ସଂଖ୍ୟା;
    v->di_tv.v_lock = VAR_FIXED;
    v->di_tv.vval.v_ସଂଖ୍ୟା = nr;
}

/*
 * Free "fc".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_funccal(funccall_T *fc)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

    ପାଇଁ (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	ଯଦି (fp != ଖାଲି && fp->uf_scoped == fc)
	    fp->uf_scoped = ଖାଲି;
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
   ସ୍ଥିର ଶୂନ୍ୟ
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cleanup_function_call(funccall_T *fc)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	may_free_fc = fc->fc_refcount <= 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    ଯଦି (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    ଅନ୍ୟଥା
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    ଯଦି (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    ଯଦି (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = ଖାଲି;
    ଅନ୍ୟଥା
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    ଯଦି (free_fc)
	free_funccal(fc);
    ଅନ୍ୟଥା
    {
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	ଯଦି (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	ଅନ୍ୟଥା ଯଦି (++made_copy >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((4096 * 1024) / ଆକାର(*fc)))
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
numbered_function(char_u *name)
{
    ଫେରନ୍ତୁ SAFE_isdigit(*name)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_name_refcount(char_u *name)
{
    ଫେରନ୍ତୁ numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
funccal_unref(funccall_T *fc, ufunc_T *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    funccall_T	**pfc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (fc == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	ପାଇଁ (pfc = &previous_funccal; *pfc != ଖାଲି; pfc = &(*pfc)->fc_caller)
	{
	    ଯଦି (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		ଫେରନ୍ତୁ;
	    }
	}
    ପାଇଁ (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	ଯଦି (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = ଖାଲି;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    ଯଦି (fp->uf_flags & FC_DEAD)
	ଫେରନ୍ତୁ FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    ଯଦି (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	ଫେରନ୍ତୁ FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    ଯଦି (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	ଫେରନ୍ତୁ FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
func_clear_ବସ୍ତୁଗୁଡ଼ିକ(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    ଯଦି (IS_GENERIC_FUNC(fp))
	generic_func_clear_ବସ୍ତୁଗୁଡ଼ିକ(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = ଖାଲି;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    ଯଦି (fp->uf_cb_free != ଖାଲି)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = ଖାଲି;
    }

    fp->uf_cb_state = ଖାଲି;
    fp->uf_cb = ଖାଲି;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_ଗଣନା);
    VIM_CLEAR(fp->uf_tml_total);
    VIM_CLEAR(fp->uf_tml_self);
#endif
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
func_clear(ufunc_T *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    ଯଦି (fp->uf_cleared)
	ଫେରନ୍ତୁ;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_ବସ୍ତୁଗୁଡ଼ିକ(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_free(ufunc_T *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    ଯଦି ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    ଯଦି ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	ଯଦି (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	ଫେରନ୍ତୁ OK;
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    ଶୂନ୍ୟ
func_clear_free(ufunc_T *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    func_clear(fp, force);
    ଯଦି (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    ଅନ୍ୟଥା
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = ଖାଲି;

    ଯଦି (ufunc == ଖାଲି)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	ଫେରନ୍ତୁ FAIL;
    }

    fp = find_func(global, TRUE);
    ଯଦି (fp != ଖାଲି)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	ଫେରନ୍ତୁ FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    ଯଦି (fp == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    ଯଦି (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ, &fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ) == FAIL)
	ଯାଅ failed;

    ଯଦି (ufunc->uf_arg_types != ଖାଲି)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	ଯଦି (fp->uf_arg_types == ଖାଲି)
	    ଯାଅ failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    ଆକାର(type_T *) * fp->uf_args.ga_len);
    }
    ଯଦି (ufunc->uf_va_name != ଖାଲି)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	ଯଦି (fp->uf_va_name == ଖାଲି)
	    ଯାଅ failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    ଯଦି ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == ଖାଲି)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	ଯଦି (pt == ଖାଲି)
	    ଯାଅ failed;
	ଯଦି (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    ଯାଅ failed;
	}
	ufunc->uf_partial = pt;
    }

    ଫେରନ୍ତୁ OK;

failed:
    func_clear_free(fp, TRUE);
    ଫେରନ୍ତୁ FAIL;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
funcdepth_increment(ଶୂନ୍ୟ)
{
    ଯଦି (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	ଫେରନ୍ତୁ FAIL;
    }
    ++funcdepth;
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
funcdepth_decrement(ଶୂନ୍ୟ)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
funcdepth_get(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    ଶୂନ୍ୟ
funcdepth_restore(ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth)
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

    ଯଦି (fc == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    ଫେରନ୍ତୁ fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    ଶୂନ୍ୟ
remove_funccal(ଶୂନ୍ୟ)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    ସ୍ଥିର funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		using_sandbox = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		default_arg_err = FALSE;
    dictitem_T	*v;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fixvar_idx = 0;	// index in fc_fixvar[]
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ai;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    ଯଦି (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_ସଂଖ୍ୟା;
	rettv->vval.v_ସଂଖ୍ୟା = -1;
	ଫେରନ୍ତୁ FCERR_FAILED;
    }

    ଧାଡ଼ି_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    ଯଦି (fc == ଖାଲି)
	ଫେରନ୍ତୁ FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, ଆକାର(ufunc_T *), 1);

    ଯଦି (fp->uf_def_status != UF_NOT_COMPILED)
    {
	ଯଦି (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == ଖାଲି ? ଖାଲି : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	ଯଦି (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	ଯଦି (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	ଯଦି (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	ଯଦି (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != ଖାଲି && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	ଫେରନ୍ତୁ retval;
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
    ଯଦି (selfdict != ଖାଲି)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_ଚାବି;
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
    ଯଦି ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    ଯଦି ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_ଚାବି;
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
    ଯଦି ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    ପାଇଁ (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    addlocal = FALSE;
	typval_T    def_rettv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	ଯଦି (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    ଯଦି (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_ସଂଖ୍ୟା == VVAL_NONE));
	    ଯଦି (isdefault)
	    {
		char_u	    *default_ପଦସମଷ୍ଟି = ଖାଲି;

		def_rettv.v_type = VAR_ସଂଖ୍ୟା;
		def_rettv.vval.v_ସଂଖ୍ୟା = -1;

		default_ପଦସମଷ୍ଟି = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		ଯଦି (eval1(&default_ପଦସମଷ୍ଟି, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    ଭାଙ୍ଗ;
		}
	    }

	    namelen = STRLEN(name);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		ଭାଙ୍ଗ;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((ଅକ୍ଷର *)numbuf, ଆକାର(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	ଯଦି (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_ଚାବି, name);
	}
	ଅନ୍ୟଥା
	{
	    v = dictitem_alloc(name);
	    ଯଦି (v == ଖାଲି)
		ଭାଙ୍ଗ;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	ଯଦି (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	ଯଦି (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	ଅନ୍ୟଥା
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	ଯଦି (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    ଯଦି (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_ଠେଲ_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    ଯଦି (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	ଯଦି (p_verbose >= 14)
	{
	    char_u	buf[ସନ୍ଦେଶ_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    ସନ୍ଦେଶ_puts("(");
	    ପାଇଁ (i = 0; i < argcount; ++i)
	    {
		ଯଦି (i > 0)
		    ସନ୍ଦେଶ_puts(", ");
		ଯଦି (argvars[i].v_type == VAR_ସଂଖ୍ୟା)
		    ସନ୍ଦେଶ_outnum((long)argvars[i].vval.v_ସଂଖ୍ୟା);
		ଅନ୍ୟଥା
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    ଯଦି (s != ଖାଲି)
		    {
			ଯଦି (vim_strsize(s) > ସନ୍ଦେଶ_BUF_CLEN)
			{
			    trunc_string(s, buf, ସନ୍ଦେଶ_BUF_CLEN, ସନ୍ଦେଶ_BUF_LEN);
			    s = buf;
			}
			ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)s);
			vim_free(tofree);
		    }
		}
	    }
	    ସନ୍ଦେଶ_puts(")");
	}
	ସନ୍ଦେଶ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
	profile_may_start_func(&profile_info, fp,
			fc->fc_caller == ଖାଲି ? ଖାଲି : fc->fc_caller->fc_func);
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

    ଯଦି (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    ଅନ୍ୟଥା ଯଦି (islambda)
    {
	char_u *p = *(char_u **)fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(ଶୂନ୍ୟ)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    ଅନ୍ୟଥା
	// call do_cmdline() to execute the lines
	do_cmdline(ଖାଲି, get_func_ଧାଡ଼ି, (ଶୂନ୍ୟ *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    ଯଦି (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    ଯଦି ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_ସଂଖ୍ୟା;
	rettv->vval.v_ସଂଖ୍ୟା = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	ଯଦି (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == ଖାଲି ? ଖାଲି : fc->fc_caller->fc_func;

	ଯଦି (fp->uf_profiling || (caller != ଖାଲି && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    ଯଦି (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	ଯଦି (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	ଅନ୍ୟଥା ଯଦି (fc->fc_rettv->v_type == VAR_ସଂଖ୍ୟା)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (long)fc->fc_rettv->vval.v_ସଂଖ୍ୟା);
	ଅନ୍ୟଥା
	{
	    char_u	buf[ସନ୍ଦେଶ_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    ଯଦି (s != ଖାଲି)
	    {
		ଯଦି (vim_strsize(s) > ସନ୍ଦେଶ_BUF_CLEN)
		{
		    trunc_string(s, buf, ସନ୍ଦେଶ_BUF_CLEN, ସନ୍ଦେଶ_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	ସନ୍ଦେଶ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_କାଢ଼();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    ଯଦି (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    ଯଦି (p_verbose >= 12 && SOURCING_NAME != ଖାଲି)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	ସନ୍ଦେଶ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    ପାଇଁ (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    ଫେରନ୍ତୁ retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା regular_args = fp->uf_args.ga_len;

    ଯଦି (argcount < regular_args - fp->uf_def_args.ga_len)
	ଫେରନ୍ତୁ FCERR_TOOFEW;
    ଅନ୍ୟଥା ଯଦି (!has_varargs(fp) && argcount > regular_args)
	ଫେରନ୍ତୁ FCERR_TOOMANY;
    ଫେରନ୍ତୁ FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ifdef FEAT_LUA
    ଯଦି (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	ଫେରନ୍ତୁ (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    ଯଦି (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != ଖାଲି)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    ଯଦି (error != FCERR_UNKNOWN)
	ଫେରନ୍ତୁ error;

    ଯଦି ((fp->uf_flags & FC_DICT) && selfdict == ଖାଲି)
    {
	error = FCERR_DICT;
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_ସନ୍ଧାନ_patterns();
	ଯଦି (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : ଖାଲି);
	ଯଦି (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	ଯଦି (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_ସନ୍ଧାନ_patterns();
    }

    ଫେରନ୍ତୁ error;
}

ସ୍ଥିର funccal_entry_T *funccal_ଗଦା = ଖାଲି;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    ଶୂନ୍ୟ
save_funccal(funccal_entry_T *entry)
{
    entry->ଶୀର୍ଷ_funccal = current_funccal;
    entry->next = funccal_ଗଦା;
    funccal_ଗଦା = entry;
    current_funccal = ଖାଲି;
}

    ଶୂନ୍ୟ
restore_funccal(ଶୂନ୍ୟ)
{
    ଯଦି (funccal_ଗଦା == ଖାଲି)
	internal_error("restore_funccal()");
    ଅନ୍ୟଥା
    {
	current_funccal = funccal_ଗଦା->ଶୀର୍ଷ_funccal;
	funccal_ଗଦା = funccal_ଗଦା->next;
    }
}

    funccall_T *
get_current_funccal(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
at_script_level(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ current_funccal == ଖାଲି && autocmd_match == ଖାଲି;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    ଶୂନ୍ୟ
delete_script_functions(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR;
    len = 3 + vim_snprintf((ଅକ୍ଷର *)buf + 3, ଆକାର(buf) - 3, "%d_", sid);

    ଯେତେବେଳେ (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&func_hashtab, hi, todo)
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		ଯଦି (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    ଯଦି (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			ଯଦି (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    ଅନ୍ୟଥା
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			ଯଦି (changed != func_hashtab.ht_changed)
			    ଭାଙ୍ଗ;
		    }
		}
		--todo;
	    }
    }
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_all_functions(ଶୂନ୍ୟ)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		changed;

    // Clean up the current_funccal chain and the funccal stack.
    ଯେତେବେଳେ (current_funccal != ଖାଲି)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	ଯଦି (current_funccal == ଖାଲି && funccal_ଗଦା != ଖାଲି)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    ଯେତେବେଳେ (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&func_hashtab, hi, todo)
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		ଯଦି (func_name_refcount(fp->uf_name))
		    ++skipped;
		ଅନ୍ୟଥା
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    ଯଦି (changed != func_hashtab.ht_changed)
		    {
			skipped = 0;
			ଭାଙ୍ଗ;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    ଯେତେବେଳେ (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&func_hashtab, hi, todo)
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		ଯଦି (func_name_refcount(fp->uf_name))
		    ++skipped;
		ଅନ୍ୟଥା
		{
		    ଯଦି (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			ଭାଙ୍ଗ;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    ଯଦି (skipped == 0)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
builtin_function(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	ଯଦି (name[i] == AUTOLOAD_CHAR)
	    ଫେରନ୍ତୁ FALSE;
	ଯଦି (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    ଯଦି (name[i] == '.')
		ଫେରନ୍ତୁ FALSE;
	    ଭାଙ୍ଗ;
	}
    }
    ଫେରନ୍ତୁ TRUE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, item)
    {
	ଯଦି (argc == MAX_FUNC_ARGS - (partial == ଖାଲି ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    ଭାଙ୍ଗ;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    ଯଦି (item == ଖାଲି)
    {
	funcexe_T	funcexe;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		namelen = -1;

	ଯଦି (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    ଯଦି (p != ଖାଲି)
	    {
		ଯଦି (skip_generic_func_type_args(&p) == FAIL)
		    ଯାଅ done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ଯଦି (partial != ଖାଲି)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ଯଦି (funcexe.fe_object != ଖାଲି)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    ଯେତେବେଳେ (argc > 0)
	clear_tv(&argv[--argc]);

done:
    ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା callback_depth = 0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_callback_depth(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_callback(
    callback_T	*callback,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    ଯଦି (callback->cb_name == ଖାଲି || *callback->cb_name == NUL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	ଫେରନ୍ତୁ FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    ଯଦି (callback->cb_partial != ଖାଲି)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	ଯଦି (funcexe.fe_object != ଖାଲି)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    ଯଦି (need_rethrow && current_exception != ଖାଲି && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    ଯଦି (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	ଫେରନ୍ତୁ -2;

    retval = tv_get_ସଂଖ୍ୟା_chk(&rettv, ଖାଲି);
    clear_tv(&rettv);
    ଫେରନ୍ତୁ retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    ଶୂନ୍ୟ
user_func_error(funcerror_T error, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା found_var)
{
    ବାଛ (error)
    {
	ସ୍ଥିତି FCERR_UNKNOWN:
		ଯଦି (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		ଅନ୍ୟଥା
		    emsg_funcname(e_unknown_function_str, name);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି FCERR_OTHER:
	ସ୍ଥିତି FCERR_FAILED:
		// assume the error message was already given
		ଭାଙ୍ଗ;
	ସ୍ଥିତି FCERR_NONE:
		ଭାଙ୍ଗ;
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
    ସ୍ଥିର funcerror_T
may_check_argument_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    base_included,
	char_u	    *name)
{
    ଯଦି (funcexe->fe_check_type != ଖାଲି && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	ଯଦି (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? ଖାଲି : funcexe->fe_basetv,
			  name) == FAIL)
	    ଫେରନ୍ତୁ FCERR_OTHER;
    }
    ଫେରନ୍ତୁ FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_func(
    char_u	*funcname,	// name of the function
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ufunc_T	*fp = ଖାଲି;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ଖାଲି;
    char_u	*fname = ଖାଲି;
    char_u	*name = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argv_clear = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    ଯଦି (partial != ଖାଲି)
	fp = partial->pt_func;
    ଯଦି (fp == ଖାଲି)
	fp = funcexe->fe_ufunc;

    ଯଦି (fp == ଖାଲି)
    {
	ଯଦି (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    ଯଦି (p != ଖାଲି)
	    {
		len = p - funcname;
		ଯଦି (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == ଖାଲି)
		    ଯାଅ theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	ଯଦି (name == ଖାଲି)
	    ଯାଅ theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    ଯଦି (funcexe->fe_doesrange != ଖାଲି)
	*funcexe->fe_doesrange = FALSE;

    ଯଦି (partial != ଖାଲି)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	ଯଦି (partial->pt_dict != ଖାଲି && (selfdict == ଖାଲି || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	ଯଦି (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    ପାଇଁ (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		ଯଦି (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    ଯାଅ theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    ପାଇଁ (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    ଯଦି (funcexe->fe_check_type != ଖାଲି
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		ପାଇଁ (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    ଯଦି (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != ଖାଲି) ? name : funcname);

    ଯଦି (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_global = FALSE;

	// Skip "g:" before a function name.
	ଯଦି (fp == ଖାଲି && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_ସଂଖ୍ୟା;	// default rettv is number zero
	rettv->vval.v_ସଂଖ୍ୟା = 0;
	error = FCERR_UNKNOWN;

	ଯଦି (fp != ଖାଲି || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    ଯଦି (fp == ଖାଲି)
	    {
		fp = find_func(rfname, is_global);
		ଯଦି (fp != ଖାଲି && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = ଖାଲି;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    ଯଦି (fp == ଖାଲି
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, ଖାଲି)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    ଯଦି (fp == ଖାଲି && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    ଯଦି (fp == ଖାଲି)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		ଯଦି (p != ଖାଲି && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    ଯଦି (partial == ଖାଲି && fp != ଖାଲି && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		ଯଦି (fp == ଖାଲି)
		{
		    error = FCERR_FAILED;
		    ଯାଅ theend;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != ଖାଲି ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		ଯାଅ theend;
	    }

	    ଯଦି (fp != ଖାଲି && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    ଅନ୍ୟଥା ଯଦି (fp != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା need_arg_check = FALSE;
		ଯଦି (funcexe->fe_check_type == ଖାଲି)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		ଯଦି (funcexe->fe_argv_func != ଖାଲି)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		ଯଦି (funcexe->fe_basetv != ଖାଲି)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, ଆକାର(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		ଯଦି (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != ଖାଲି) ? name : funcname);

		ଯଦି (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (funcexe->fe_basetv != ଖାଲି)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	ଅନ୍ୟଥା
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
    ଯଦି (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    ଯଦି (!aborting())
	user_func_error(error, (name != ଖାଲି) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    ଯେତେବେଳେ (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    ଫେରନ୍ତୁ ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ଖାଲି;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_ସଂଖ୍ୟା;	// default rettv is number zero
    rettv->vval.v_ସଂଖ୍ୟା = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    ଯଦି (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    ଅନ୍ୟଥା
	rfname = fname;
    fp = find_func(rfname, is_global);
    ଯଦି (fp != ଖାଲି && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = ଖାଲି;
    ଯଦି (fp == ଖାଲି)
	ret = NOTDONE;
    ଅନ୍ୟଥା ଯଦି (fp != ଖାଲି && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    ଅନ୍ୟଥା ଯଦି (fp != ଖାଲି)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, ଖାଲି);
	ଯଦି (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    ଫେରନ୍ତୁ ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    ଫେରନ୍ତୁ fp->uf_name_exp != ଖାଲି ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
function_list_modified(ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_ht_changed)
{
    ଯଦି (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_func_head(ufunc_T *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_ht_changed = func_hashtab.ht_changed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;

    ସନ୍ଦେଶ_start();

    // a timer at the more prompt may have deleted the function
    ଯଦି (function_list_modified(prev_ht_changed))
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (indent)
	ସନ୍ଦେଶ_puts("   ");
    ଯଦି (fp->uf_def_status != UF_NOT_COMPILED)
	ସନ୍ଦେଶ_puts("def ");
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_puts("function ");
    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)printable_func_name(fp));
    ସନ୍ଦେଶ_putchar('(');
    ପାଇଁ (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	ଯଦି (j)
	    ସନ୍ଦେଶ_puts(", ");
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)FUNCARG(fp, j));
	ଯଦି (fp->uf_arg_types != ଖାଲି)
	{
	    ଅକ୍ଷର *tofree;

	    ସନ୍ଦେଶ_puts(": ");
	    ସନ୍ଦେଶ_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ଯଦି (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    ସନ୍ଦେଶ_puts(" = ");
	    ସନ୍ଦେଶ_puts(((ଅକ୍ଷର **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    ଯଦି (fp->uf_varargs)
    {
	ଯଦି (j)
	    ସନ୍ଦେଶ_puts(", ");
	ସନ୍ଦେଶ_puts("...");
    }
    ଯଦି (fp->uf_va_name != ଖାଲି)
    {
	ଯଦି (!fp->uf_varargs)
	{
	    ଯଦି (j)
		ସନ୍ଦେଶ_puts(", ");
	    ସନ୍ଦେଶ_puts("...");
	}
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)fp->uf_va_name);
	ଯଦି (fp->uf_va_type != ଖାଲି)
	{
	    ଅକ୍ଷର *tofree;

	    ସନ୍ଦେଶ_puts(": ");
	    ସନ୍ଦେଶ_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    ସନ୍ଦେଶ_putchar(')');

    ଯଦି (fp->uf_def_status != UF_NOT_COMPILED)
    {
	ଯଦି (fp->uf_ret_type != &t_void)
	{
	    ଅକ୍ଷର *tofree;

	    ସନ୍ଦେଶ_puts(": ");
	    ସନ୍ଦେଶ_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    ଅନ୍ୟଥା ଯଦି (fp->uf_flags & FC_ABORT)
	ସନ୍ଦେଶ_puts(" abort");
    ଯଦି (fp->uf_flags & FC_RANGE)
	ସନ୍ଦେଶ_puts(" range");
    ଯଦି (fp->uf_flags & FC_DICT)
	ସନ୍ଦେଶ_puts(" dict");
    ଯଦି (fp->uf_flags & FC_CLOSURE)
	ସନ୍ଦେଶ_puts(" closure");
    ସନ୍ଦେଶ_clr_eos();
    ଯଦି (p_verbose > 0)
	last_set_ସନ୍ଦେଶ(fp->uf_script_ctx);

    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*is_global,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip,		// only find the end, don't evaluate
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    ଫେରନ୍ତୁ trans_function_name_ext(pp, is_global, skip, flags,
	    ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    ସ୍ଥିର char_u *
trans_function_name_ext(
    char_u	**pp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*is_global,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip,		// only find the end, don't evaluate
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = ଖାଲି;
    char_u	*start;
    char_u	*end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lead;
    char_u	sid_buf[20];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefix_g = FALSE;
    lval_T	lv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9_local;

    ଯଦି (fdp != ଖାଲି)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    ଯଦି ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	ଫେରନ୍ତୁ vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    ଯଦି (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, ଖାଲି, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    ଯଦି (end == start || (vim9script && end != ଖାଲି
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	ଯଦି (!skip)
	    emsg(_(e_function_name_required));
	ଯାଅ theend;
    }
    ଯଦି (end == ଖାଲି || (lv.ll_tv != ଖାଲି && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	ଯଦି (!aborting())
	{
	    ଯଦି (end != ଖାଲି)
		semsg(_(e_invalid_argument_str), start);
	}
	ଅନ୍ୟଥା
	    *pp = find_name_end(start, ଖାଲି, ଖାଲି, FNE_INCL_BR);
	ଯାଅ theend;
    }

    ଯଦି (lv.ll_ufunc != ଖାଲି)
    {
	ଯଦି (ufunc != ଖାଲି)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	ଯାଅ theend;
    }

    ଯଦି (lv.ll_tv != ଖାଲି)
    {
	ଯଦି (fdp != ଖାଲି)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = ଖାଲି;
	    fdp->fd_di = lv.ll_di;
	}
	ଯଦି (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != ଖାଲି)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	ଅନ୍ୟଥା ଯଦି (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != ଖାଲି)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	ଅନ୍ୟଥା ଯଦି (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != ଖାଲି)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    ଯଦି (partial != ଖାଲି)
		*partial = lv.ll_tv->vval.v_partial;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (!skip && !(flags & TFN_QUIET) && (fdp == ଖାଲି
			     || lv.ll_dict == ଖାଲି || fdp->fd_newkey == ଖାଲି))
		emsg(_(e_funcref_required));
	    ଅନ୍ୟଥା
		*pp = end;
	    name = ଖାଲି;
	}
	ଯାଅ theend;
    }

    ଯଦି (lv.ll_name == ଖାଲି)
    {
	// Error found, but continue after the function name.
	*pp = end;
	ଯାଅ theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    ଯଦି (lv.ll_exp_name != ଖାଲି)
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ଖାଲି);
	ଯଦି (name == lv.ll_exp_name)
	    name = ଖାଲି;
    }
    ଅନ୍ୟଥା ଯଦି (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	ଯଦି (si->sn_autoload_prefix != ଖାଲି)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	ଅନ୍ୟଥା
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR;
	    vim_snprintf((ଅକ୍ଷର *)sid_buf + 3, ଆକାର(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	ଯାଅ theend;
    }
    ଅନ୍ୟଥା ଯଦି (!(flags & TFN_NO_DEREF))
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ଖାଲି);
	ଯଦି (name == *pp)
	    name = ଖାଲି;
    }
    ଯଦି (name != ଖାଲି)
    {
	name = vim_strsave(name);
	*pp = end;
	ଯଦି (name != ଖାଲି && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	ଯାଅ theend;
    }

    ଯଦି (lv.ll_exp_name != ଖାଲି)
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(lv.ll_exp_name);
	ଯଦି (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    ଅନ୍ୟଥା
    {
	// skip over "s:" and "g:"
	ଯଦି (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    ଯଦି (lv.ll_name[0] == 'g')
	    {
		ଯଦି (is_global != ଖାଲି)
		{
		    *is_global = TRUE;
		}
		ଅନ୍ୟଥା
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - lv.ll_name);
    }
    ଯଦି (len <= 0)
    {
	ଯଦି (!skip)
	    emsg(_(e_function_name_required));
	ଯାଅ theend;
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
    ଯଦି (skip)
	lead = 0;	// do nothing
    ଅନ୍ୟଥା ଯଦି (lead > 0 || vim9_local)
    {
	ଯଦି (!vim9_local)
	{
	    ଯଦି (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		ଯାଅ theend;
	    }
	    lead = 3;
	}
	ଯଦି (vim9_local || (lv.ll_exp_name != ଖାଲି
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    ଯଦି (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		ଯାଅ theend;
	    }
	    sid_buflen = vim_snprintf((ଅକ୍ଷର *)sid_buf, ଆକାର(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    ଯଦି (vim9_local)
		extra = 3 + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sid_buflen;
	    ଅନ୍ୟଥା
		lead += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    ଅନ୍ୟଥା ଯଦି (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	ଯାଅ theend;
    }
    ଯଦି (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	ଯଦି (cp != ଖାଲି && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    ଯାଅ theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    ଯଦି (name != ଖାଲି)
    {
	ଯଦି (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR;
	    ଯଦି (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	ଅନ୍ୟଥା ଯଦି (prefix_g)
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
    ଫେରନ୍ତୁ name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    ସ୍ଥିର char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    ଯଦି (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	ଯଦି (p != ଖାଲି)
	    ଫେରନ୍ତୁ p + 1;
    }
    ଫେରନ୍ତୁ ଖାଲି;
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
    ଅକ୍ଷର	sid_buf[25];
    size_t	sid_buflen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    ଯଦି (funcname == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	ଯଦି (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    ଫେରନ୍ତୁ ଖାଲି;
	ufunc = find_func(funcname, FALSE);
	ଯଦି (ufunc == ଖାଲି || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	++p;
	off = 0;
    }
    ଅନ୍ୟଥା
	off = *funcname == 's' ? 2 : 5;

    ଯଦି (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, ଆକାର(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    ଯଦି (newname == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    vim_snprintf((ଅକ୍ଷର *)newname, newnamesize, "%s%s", sid_buf, p + off);

    ଫେରନ୍ତୁ newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    ଯଦି (n != ଖାଲି)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    ଫେରନ୍ତୁ n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *is_global,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    skip,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    ଯଦି (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(ଶୂନ୍ୟ)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	ଯଦି (fudi != ଖାଲି)
	    CLEAR_POINTER(fudi);
    }
    ଅନ୍ୟଥା
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, ଖାଲି, ଖାଲି, ଖାଲି);
    *name = p;
    ଫେରନ୍ତୁ saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    ଶୂନ୍ୟ
list_functions(regmatch_T *regmatch)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    ପାଇଁ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    ଯଦି ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == ଖାଲି
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		ଯଦି (list_func_head(fp, FALSE) == FAIL)
		    ଫେରନ୍ତୁ;
		ଯଦି (function_list_modified(prev_ht_changed))
		    ଫେରନ୍ତୁ;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    ସ୍ଥିର char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    ଯଦି (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	ଯଦି (regmatch.regprog != ଖାଲି)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    ଯଦି (*p == '/')
	++p;

    ଫେରନ୍ତୁ p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    ସ୍ଥିର ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_global)
{
    ufunc_T	*fp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;

    ଯଦି (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    set_nextcmd(eap, p);

    ଯଦି (eap->nextcmd != ଖାଲି)
	*p = NUL;

    ଯଦି (eap->skip || got_int)
	ଫେରନ୍ତୁ ଖାଲି;

    fp = find_func(name, is_global);
    ଯଦି (fp == ଖାଲି && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	ଯଦି (up != ଖାଲି)
	    fp = find_func(up, FALSE);
    }

    ଯଦି (fp == ଖାଲି)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_ht_changed = func_hashtab.ht_changed;

    ଯଦି (list_func_head(fp, TRUE) != OK)
	ଫେରନ୍ତୁ fp;

    ପାଇଁ (j = 0; j < fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len && !got_int; ++j)
    {
	ଯଦି (FUNCLINE(fp, j) == ଖାଲି)
	    ଜାରି;
	ସନ୍ଦେଶ_putchar('\n');
	ସନ୍ଦେଶ_outnum((long)(j + 1));
	ଯଦି (j < 9)
	    ସନ୍ଦେଶ_putchar(' ');
	ଯଦି (j < 99)
	    ସନ୍ଦେଶ_putchar(' ');
	ଯଦି (function_list_modified(prev_ht_changed))
	    ଭାଙ୍ଗ;
	ସନ୍ଦେଶ_prt_ଧାଡ଼ି(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    ଯଦି (!got_int)
    {
	ସନ୍ଦେଶ_putchar('\n');
	ଯଦି (!function_list_modified(prev_ht_changed))
	{
	    ଯଦି (fp->uf_def_status != UF_NOT_COMPILED)
		ସନ୍ଦେଶ_puts("   enddef");
	    ଅନ୍ୟଥା
		ସନ୍ଦେଶ_puts("   endfunction");
	}
    }

    ଫେରନ୍ତୁ fp;
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
	garray_T    *ଧାଡ଼ିଗୁଡ଼ିକ_to_free,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    class_flags,
	ocmember_T  *obj_members,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା         obj_member_ଗଣନା,
	cctx_T	    *cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*ଧାଡ଼ି_arg = ଖାଲି;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		varargs = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;
    char_u	*ret_type = ଖାଲି;
    ufunc_T	*fp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fp_allocated = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		free_fp = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	func_nr = 0;	    // number for nameless function
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_ଶୀର୍ଷ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    imported_T	*import = ଖାଲି;

    // ":function" without argument: list functions.
    ଯଦି (ends_excmd2(eap->cmd, eap->arg))
    {
	ଯଦି (!eap->skip)
	    list_functions(ଖାଲି);
	set_nextcmd(eap, eap->arg);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    ଯଦି (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	ଫେରନ୍ତୁ ଖାଲି;
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
    ଯଦି (name_arg != ଖାଲି)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (vim9script)
	{
	    ଯଦି (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    p = to_name_end(p, TRUE);
	    ଯଦି (*skipwhite(p) == '.' && vim_strchr(p, '(') != ଖାଲି)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		ଫେରନ୍ତୁ ଖାଲି;
	    }

	    p = eap->arg;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != ଖାଲି);
	ଯଦି (name == ଖାଲି && (fudi.fd_dict == ଖାଲି || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    ଯଦି (!aborting())
	    {
		ଯଦି (!eap->skip && fudi.fd_newkey != ଖାଲି)
		    semsg(_(e_ଚାବି_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    ଅନ୍ୟଥା
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	ଯଦି (is_export && name != ଖାଲି)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    ଯଦି (prefixed != ଖାଲି && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (paren && vim9script && name != ଖାଲି
				    && vim_strchr(name, AUTOLOAD_CHAR) != ଖାଲି)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    ଯାଅ ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    ଯଦି (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	ଯାଅ ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    ଯଦି (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	ଯଦି (p == ଖାଲି)
	    ଯାଅ ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    ଯଦି (*p != '(')
    {
	ଯଦି (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    ଯାଅ ret_free;
	}
	// attempt to continue by skipping some text
	ଯଦି (vim_strchr(p, '(') != ଖାଲି)
	    p = vim_strchr(p, '(');
    }

    ଯଦି ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	ଯାଅ ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    ଯଦି (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	ଯାଅ ret_free;
    }

    ga_init2(&newlines, ଆକାର(char_u *), 10);

    ଯଦି (!eap->skip && name_arg == ଖାଲି)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	ଯଦି (name != ଖାଲି)
	    arg = name;
	ଅନ୍ୟଥା
	    arg = fudi.fd_newkey;
	ଯଦି (arg != ଖାଲି && (fudi.fd_di == ଖାଲି
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    ଯଦି (arg != fudi.fd_newkey)
	    {
		ଯଦି (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    ଯଦି (name_base == ଖାଲି)
			name_base = arg + 3;
		    ଅନ୍ୟଥା
			++name_base;
		}
		ପାଇଁ (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		ଯଦି (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    ଯାଅ ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		ଯଦି (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, ଖାଲି,
			ଖାଲି, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    ଯାଅ ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	ଯଦି (fudi.fd_dict != ଖାଲି && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    ଯାଅ ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    ଯଦି (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : ଖାଲି, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : ଖାଲି,
			 ଖାଲି, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, ଧାଡ଼ିଗୁଡ଼ିକ_to_free) == FAIL)
	ଯାଅ errret_2;
    whitep = p;

    ଯଦି (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	ଯଦି (*skipwhite(p) == ':')
	{
	    ଯଦି (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    ଅନ୍ୟଥା ଯଦି (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    ଯଦି (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    ଅନ୍ୟଥା
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = ଖାଲି;
	    }
	}
	p = skipwhite(p);
    }
    ଅନ୍ୟଥା
	// find extra arguments "range", "dict", "abort" and "closure"
	ପାଇଁ (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    ଯଦି (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		ଯଦି (current_funccal == ଖାଲି)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_ଶୀର୍ଷ_level_str,
			    name == ଖାଲି ? (char_u *)"" : name);
		    ଯାଅ erret;
		}
	    }
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    ଯଦି (*p == '\n')
	ଧାଡ଼ି_arg = p + 1;
    ଅନ୍ୟଥା ଯଦି (*p != NUL
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
    ଯଦି (ଚାବି_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	ଯଦି (!eap->skip && !eap->forceit)
	{
	    ଯଦି (fudi.fd_dict != ଖାଲି && fudi.fd_newkey == ଖାଲି)
		emsg(_(e_dictionary_entry_already_exists));
	    ଅନ୍ୟଥା ଯଦି (name != ଖାଲି && find_func(name, is_global) != ଖାଲି)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	ଯଦି (!eap->skip && did_emsg)
	    ଯାଅ erret;

	ସନ୍ଦେଶ_putchar('\n');	    // don't overwrite the function name
	cmdline_row = ସନ୍ଦେଶ_row;
    }

    // Save the starting line number.
    sourcing_lnum_ଶୀର୍ଷ = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    ଯଦି (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, ଧାଡ଼ି_arg, ଧାଡ଼ିଗୁଡ଼ିକ_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	ଯାଅ erret;

    /*
     * If there are no errors, add the function
     */
    ଯଦି (fudi.fd_dict != ଖାଲି)
    {
	ଅକ୍ଷର	numbuf[NUMBUFLEN];

	fp = ଖାଲି;
	ଯଦି (fudi.fd_newkey == ଖାଲି && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    ଯାଅ erret;
	}
	ଯଦି (fudi.fd_di == ଖାଲି)
	{
	    // Can't add a function to a locked dictionary
	    ଯଦି (ମୂଲ୍ୟ_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		ଯାଅ erret;
	}
	    // Can't change an existing function if it is locked
	ଅନ୍ୟଥା ଯଦି (ମୂଲ୍ୟ_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    ଯାଅ erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, ଆକାର(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	ଯଦି (name == ଖାଲି)
	    ଯାଅ erret;
    }
    ଅନ୍ୟଥା ଯଦି (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_conflict = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	ଯଦି (v != ଖାଲି && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	ଯଦି (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    ଯଦି (si->sn_autoload_prefix != ଖାଲି)
	    {
		ଯଦି (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    ଯଦି (v != ଖାଲି)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		ଅନ୍ୟଥା
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    ଯଦି (prefixed != ଖାଲି && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			ଯଦି (v != ଖାଲି)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	ଯଦି (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    ଯାଅ erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	ଯଦି (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == ଖାଲି ? name : uname, 0, FALSE);
	}

	ଯଦି (fp != ଖାଲି || import != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dead = fp != ଖାଲି && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    ଯଦି (import != ଖାଲି
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_ଶୀର୍ଷ;
		ଯଦି (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		ଅନ୍ୟଥା
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		ଯାଅ errret_keep;
	    }
	    ଯଦି (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		ଯାଅ errret_keep;
	    }
	    ଯଦି (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = ଖାଲି;
		overwrite = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = ଖାଲି;
		func_clear_ବସ୍ତୁଗୁଡ଼ିକ(fp);
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

    ଯଦି (fp == ଖାଲି)
    {
	ଯଦି (fudi.fd_dict == ଖାଲି && vim_strchr(name, AUTOLOAD_CHAR) != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    ଯଦି (SOURCING_NAME != ଖାଲି)
	    {
		scriptname = autoload_name(name);
		ଯଦି (scriptname != ଖାଲି)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
		    slen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(SOURCING_NAME);
		    ଯଦି (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    ଯଦି (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_ଶୀର୍ଷ;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		ଯାଅ erret;
	    }
	}

	ଯଦି (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	ଯଦି (fp == ଖାଲି)
	    ଯାଅ erret;
	fp_allocated = TRUE;

	ଯଦି (fudi.fd_dict != ଖାଲି)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    ଯଦି (func_name == ଖାଲି)
	    {
		VIM_CLEAR(fp);
		ଯାଅ erret;
	    }
	    ଯଦି (fudi.fd_di == ଖାଲି)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		ଯଦି (fudi.fd_di == ଖାଲି)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    ଯାଅ erret;
		}
		ଯଦି (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    ଯାଅ erret;
		}
	    }
	    ଅନ୍ୟଥା
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

    ଯଦି (eap->cmdidx == CMD_def)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	ଯଦି (generic_func_args_table_size(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_ଶୀର୍ଷ;

	// The function may use script variables from the context.
	function_using_block_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_is_export = is_export;
	is_export = FALSE;

	ଯଦି (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_ଗଣନା, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    ଯାଅ erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	ଯଦି (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    ଯାଅ erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    ଅନ୍ୟଥା
	fp->uf_def_status = UF_NOT_COMPILED;

    ଯଦି (fp_allocated)
    {
	// insert the new function in the function list
	ଯଦି (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_ଚାବି = UF2HIKEY(fp);
	}
	ଅନ୍ୟଥା ଯଦି (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    ଯାଅ erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ = newlines;
    newlines.ga_data = ଖାଲି;
    ଯଦି ((flags & FC_CLOSURE) != 0)
    {
	ଯଦି (register_closure(fp) == FAIL)
	    ଯାଅ erret;
    }
    ଅନ୍ୟଥା
	fp->uf_scoped = ଖାଲି;

#ifdef FEAT_PROFILE
    ଯଦି (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    ଯଦି (sandbox)
	flags |= FC_SANDBOX;
    ଯଦି (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_ଶୀର୍ଷ;
    ଯଦି (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    ଯଦି (eap->cmdidx == CMD_def)
	set_function_type(fp);
    ଅନ୍ୟଥା ଯଦି (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    ଯଦି (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, ଖାଲି);

    ଯାଅ ret_free;

erret:
    ଯଦି (fp != ଖାଲି)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    ଯଦି (fp != ଖାଲି)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	ଯଦି (IS_GENERIC_FUNC(fp))
	    generic_func_clear_ବସ୍ତୁଗୁଡ଼ିକ(fp);
    }
    ଯଦି (free_fp)
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
    ଯଦି (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    ଫେରନ୍ତୁ fp;
}

/*
 * ":function"
 */
    ଶୂନ୍ୟ
ex_function(exarg_T *eap)
{
    garray_T ଧାଡ଼ିଗୁଡ଼ିକ_to_free;

    ga_init2(&ଧାଡ଼ିଗୁଡ଼ିକ_to_free, ଆକାର(char_u *), 50);
    (ଶୂନ୍ୟ)define_function(eap, ଖାଲି, &ଧାଡ଼ିଗୁଡ଼ିକ_to_free, 0, ଖାଲି, 0, ଖାଲି);
    ga_clear_strings(&ଧାଡ଼ିଗୁଡ଼ିକ_to_free);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_func_arity(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *required, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *optional, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *varargs)
{
    ufunc_T	*ufunc = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		min_argcount = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    idx = find_internal_func(name);
    ଯଦି (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    ଅନ୍ୟଥା
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = ଖାଲି;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ଯଦି (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	ଯଦି (ufunc == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_global = FALSE;
    char_u	*bracket_start = ଖାଲି;
    char_u	*generic_func_name = name;

    ଯଦି (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    ଯଦି (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(ଶୂନ୍ୟ)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    ଅନ୍ୟଥା
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	ଯଦି (ufunc != ଖାଲି)
	    ଫେରନ୍ତୁ ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      ଖାଲି, ଖାଲି, ଖାଲି, &ufunc);
	ଯଦି (ufunc != ଖାଲି)
	{
	    vim_free(fname);
	    ଫେରନ୍ତୁ ufunc;
	}
	ଯଦି (fname != ଖାଲି && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    ଯଦି (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
    }
    ଯଦି (fname == ଖାଲି)
    {
	semsg(_(e_invalid_argument_str), name);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ufunc = find_func(fname, is_global);
    ଯଦି (ufunc == ଖାଲି)
    {
	char_u *p = untrans_function_name(fname);

	ଯଦି (p != ଖାଲି)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	ଯଦି (ufunc == ଖାଲି)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    ଯଦି (bracket_start != ଖାଲି)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    ଯଦି (ufunc != ଖାଲି)
    {
	// handle generic functions
	ଯଦି (bracket_start == ଖାଲି)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    ଯଦି (ufunc == ଖାଲି)
	semsg(_(e_cannot_find_function_str), name);
    ଫେରନ୍ତୁ ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    ଶୂନ୍ୟ
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    ଯଦି (func_needs_compiling(ufunc, compile_type))
	(ଶୂନ୍ୟ)compile_def_function(ufunc, FALSE, compile_type, ଖାଲି);
    ଅନ୍ୟଥା
	smsg(_("Function %s%s%s does not need compiling"),
				cl != ଖାଲି ? cl->class_name.string : (char_u *)"",
				cl != ଖାଲି ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    ସ୍ଥିର ଶୂନ୍ୟ
defcompile_funcs_in_script(ଶୂନ୍ୟ)
{
    long	todo = (long)func_hashtab.ht_used;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    ପାଇଁ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    ଯଦି (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(ଶୂନ୍ୟ)compile_def_function(ufunc, FALSE, CT_NONE, ଖାଲି);

		ଯଦି (func_hashtab.ht_changed != changed)
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
    ଶୂନ୍ୟ
ex_defcompile(exarg_T *eap)
{
    ଯଦି (*eap->arg != NUL)
    {
	typval_T tv;

	ଯଦି (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    ଯଦି (cl != ଖାଲି)
		defcompile_class(cl);
	}
	ଅନ୍ୟଥା
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    ଯଦି (ufunc != ଖାଲି)
		defcompile_function(ufunc, ଖାଲି);
	}
    }
    ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    ଯଦି (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	ଫେରନ୍ତୁ 5;
    ଯଦି (p[0] == 's' && p[1] == ':')
	ଫେରନ୍ତୁ 2;
    ଫେରନ୍ତୁ 0;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
translated_function_exists(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_global)
{
    ଯଦି (builtin_function(name, -1))
	ଫେରନ୍ତୁ has_internal_func(name);
    ଫେରନ୍ତୁ find_func(name, is_global) != ଖାଲି;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_varargs(ufunc_T *ufunc)
{
    ଫେରନ୍ତୁ ufunc->uf_varargs || ufunc->uf_va_name != ଖାଲି;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
function_exists(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flag;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    ଯଦି (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    ଯଦି (p != ଖାଲି && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    ଫେରନ୍ତୁ n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_global = FALSE;
    ufunc_T	*fp = ଖାଲି;
    list_T	*args;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_def;
    ଅକ୍ଷର	*tofree;

    ଯଦି (*name == NUL)
	ଫେରନ୍ତୁ FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    ଯଦି (p != ଖାଲି && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    ଯଦି (fp == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    ଯଦି (args == ଖାଲି || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	ଫେରନ୍ତୁ FAIL;
    }
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	ଯଦି (arg == ଖାଲି || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    ଫେରନ୍ତୁ FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	ଯଦି (fp->uf_arg_types == ଖାଲି)
	    dict_add_string(arg, "type", (char_u *)"any");
	ଅନ୍ୟଥା
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ଯଦି (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    ଯଦି (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	ଯଦି (va == ଖାଲି || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    ଫେରନ୍ତୁ FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == ଖାଲି ? (char_u *)"" : fp->uf_va_name);
	ଯଦି (fp->uf_va_type == ଖାଲି)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	ଅନ୍ୟଥା
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    ଯଦି (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    ଅନ୍ୟଥା
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_ସଂଖ୍ୟା(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_ସଂଖ୍ୟା(d, "lnum", fp->uf_script_ctx.sc_lnum);
    ଫେରନ୍ତୁ OK;
}

#ଯଦି defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା check)
{
    char_u	*nm = name;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    ଯଦି (p != ଖାଲି && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	ଫେରନ୍ତୁ p;

    vim_free(p);
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ସ୍ଥିର long_u	done;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		changed;
    ସ୍ଥିର hashitem_T	*hi;
    ufunc_T		*fp;

    ଯଦି (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    ଯଦି (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	ଯଦି (done++ > 0)
	    ++hi;
	ଯେତେବେଳେ (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	ଯଦି ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    ଫେରନ୍ତୁ (char_u *)"";

	ଯଦି (fp->uf_namelen + 4 >= IOSIZE)
	    ଫେରନ୍ତୁ fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	ଯଦି (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    ଯଦି (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	ଫେରନ୍ତୁ IObuff;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    ଯଦି (ufunc == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = ଖାଲି;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_ଗଣନା = ଖାଲି;
    ufunc->uf_tml_total = ଖାଲି;
    ufunc->uf_tml_self = ଖାଲି;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    ଯଦି (ufunc->uf_arg_types != ଖାଲି)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	ଯଦି (at != ଖାଲି)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     ଆକାର(type_T *) * ufunc->uf_args.ga_len);
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

    ଯଦି (ufunc->uf_va_name != ଖାଲି)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = ଖାଲି;

    ga_copy_strings(&fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ, &ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    ଯଦି (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    ଫେରନ୍ତୁ ufunc;
}

/*
 * ":delfunction {name}"
 */
    ଶୂନ୍ୟ
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = ଖାଲି;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     ଖାଲି, ଖାଲି, ଖାଲି);
    vim_free(fudi.fd_newkey);
    ଯଦି (name == ଖାଲି)
    {
	ଯଦି (fudi.fd_dict != ଖାଲି && !eap->skip)
	    emsg(_(e_funcref_required));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	ଫେରନ୍ତୁ;
    }
    set_nextcmd(eap, p);
    ଯଦି (eap->nextcmd != ଖାଲି)
	*p = NUL;

    ଯଦି (numbered_function(name) && fudi.fd_dict == ଖାଲି)
    {
	ଯଦି (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    ଯଦି (!eap->skip)
    {
	ଯଦି (fp == ଖାଲି)
	{
	    ଯଦି (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    ଫେରନ୍ତୁ;
	}

	ଯଦି (fudi.fd_dict != ଖାଲି)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	ଅନ୍ୟଥା
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    ଯଦି (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		ଯଦି (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    ଅନ୍ୟଥା
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    ଶୂନ୍ୟ
func_unref(char_u *name)
{
    ufunc_T *fp = ଖାଲି;

    ଯଦି (name == ଖାଲି || !func_name_refcount(name))
	ଫେରନ୍ତୁ;
    fp = find_func(name, FALSE);
    ଯଦି (fp == ଖାଲି && numbered_function(name))
    {
#ifdef EXITFREE
	ଯଦି (!entered_free_all_mem)
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
    ଶୂନ୍ୟ
func_ptr_unref(ufunc_T *fp)
{
    ଯଦି (fp != ଖାଲି && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != ଖାଲି
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	ଯଦି (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    ଶୂନ୍ୟ
func_ref(char_u *name)
{
    ufunc_T *fp;

    ଯଦି (name == ଖାଲି || !func_name_refcount(name))
	ଫେରନ୍ତୁ;
    fp = find_func(name, FALSE);
    ଯଦି (fp != ଖାଲି)
	++fp->uf_refcount;
    ଅନ୍ୟଥା ଯଦି (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	internal_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    ଶୂନ୍ୟ
func_ptr_ref(ufunc_T *fp)
{
    ଯଦି (fp != ଖାଲି)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
can_free_funccal(funccall_T *fc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ଫେରନ୍ତୁ (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    ଶୂନ୍ୟ
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		returning = FALSE;
    evalarg_T	evalarg;

    ଯଦି (current_funccal == ଖାଲି)
    {
	emsg(_(e_return_not_inside_function));
	ଫେରନ୍ତୁ;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    ଯଦି (eap->skip)
	++emsg_skip;

    eap->nextcmd = ଖାଲି;
    ଯଦି ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	ଯଦି (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	ଅନ୍ୟଥା
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    ଅନ୍ୟଥା ଯଦି (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	ଯଦି (!aborting())
	    returning = do_return(eap, FALSE, TRUE, ଖାଲି);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    ଯଦି (returning)
	eap->nextcmd = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି (eap->nextcmd == ଖାଲି)	    // no argument
	set_nextcmd(eap, arg);

    ଯଦି (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		doesrange;
    typval_T	rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		failed = FALSE;

    lnum = eap->line1;
    ପାଇଁ ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	ଯଦି (eap->addr_ଗଣନା > 0)
	{
	    ଯଦି (lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		ଭାଙ୍ଗ;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	ଯଦି (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	ଯଦି (handle_subscript(arg, ଖାଲି, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    ଭାଙ୍ଗ;
	}

	clear_tv(&rettv);
	ଯଦି (doesrange)
	    ଭାଙ୍ଗ;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	ଯଦି (aborting())
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		partial_argc = 0;		// number of partial arguments
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = 0;			// number of arguments found
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

    ଯଦି (current_funccal == ଖାଲି)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (partial != ଖାଲି)
    {
	ଯଦି (partial->pt_dict != ଖାଲି)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (partial->pt_argc > 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    partial_argc = partial->pt_argc;
	    ପାଇଁ (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    ଯଦି (r == OK)
    {
	ଯଦି (type != ଖାଲି)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, ଖାଲି, name);
	}
	ଅନ୍ୟଥା ଯଦି (is_builtin)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = find_internal_func(name);

	    ଯଦି (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	ଅନ୍ୟଥା
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    ଯଦି (ufunc != ଖାଲି)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		ଯଦି (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    ଯଦି (r == FAIL)
    {
	ଯେତେବେଳେ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
can_add_defer(ଶୂନ୍ୟ)
{
    ଯଦି (!in_def_function() && get_current_funccal() == ଖାଲି)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_defer(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = argcount_arg;
    defer_T	*dr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;

    ଯଦି (saved_name == ଖାଲି)
	ଯାଅ theend;
    ଯଦି (in_def_function())
    {
	ଯଦି (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, ଆକାର(defer_T), 10);
	ଯଦି (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    ଯାଅ theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	ଯେତେବେଳେ (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    ଯେତେବେଳେ (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    ଫେରନ୍ତୁ ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_defer_one(funccall_T *funccal)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;

    ପାଇଁ (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	ଯଦି (dr->dr_name == ଖାଲି)
	    // already being called, can happen if function does ":qa"
	    ଜାରି;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = ଖାଲି;

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
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    ସ୍ଥିର ଶୂନ୍ୟ
invoke_funccall_defer(funccall_T *fc)
{
    ଯଦି (fc->fc_ectx != ଖାଲି)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    ଅନ୍ୟଥା
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    ଶୂନ୍ୟ
invoke_all_defer(ଶୂନ୍ୟ)
{
    ପାଇଁ (funccall_T *fc = current_funccal; fc != ଖାଲି; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    ପାଇଁ (funccal_entry_T *fce = funccal_ଗଦା; fce != ଖାଲି; fce = fce->next)
	ପାଇଁ (funccall_T *fc = fce->ଶୀର୍ଷ_funccal; fc != ଖାଲି; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    ଶୂନ୍ୟ
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = ଖାଲି;
    partial_T	*partial = ଖାଲି;
    evalarg_T	evalarg;
    type_T	*type = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_var = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    ଯଦି (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	ଯଦି (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	ଫେରନ୍ତୁ;
    }

    tofree = trans_function_name_ext(&arg, ଖାଲି, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : ଖାଲି, &ufunc);
    ଯଦି (fudi.fd_newkey != ଖାଲି)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_ଚାବି_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    ଯଦି (tofree == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    ଯଦି (fudi.fd_dict != ଖାଲି)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != ଖାଲି ? ଖାଲି : &partial,
				vim9script && type == ଖାଲି ? &type : ଖାଲି,
						     FALSE, FALSE, &found_var);

    ଯଦି (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	ଯଦି (name == ଖାଲି)
	    ଯାଅ end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    ଯଦି (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	ଯାଅ end;
    }
    ଯଦି (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	ଯାଅ end;
    }

    ଯଦି (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    ଅନ୍ୟଥା
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
    ଯଦି ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	ଯଦି (!ends_excmd2(eap->arg, arg))
	{
	    ଯଦି (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_return(
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		reanimate,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_cmd,
    ଶୂନ୍ୟ	*rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    cstack_T	*cstack = eap->cstack;

    ଯଦି (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ଯଦି (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	ଯଦି (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	ଅନ୍ୟଥା
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    ଯଦି (reanimate)
		rettv = current_funccal->fc_rettv;

	    ଯଦି (rettv != ଖାଲି)
	    {
		// Store the value of the pending return.
		ଯଦି ((cstack->cs_rettv[idx] = alloc_tv()) != ଖାଲି)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		ଅନ୍ୟଥା
		    emsg(_(e_out_of_memory));
	    }
	    ଅନ୍ୟଥା
		cstack->cs_rettv[idx] = ଖାଲି;

	    ଯଦି (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_ସଂଖ୍ୟା;
		current_funccal->fc_rettv->vval.v_ସଂଖ୍ୟା = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    ଅନ୍ୟଥା
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	ଯଦି (!reanimate && rettv != ଖାଲି)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    ଯଦି (!is_cmd)
		vim_free(rettv);
	}
    }

    ଫେରନ୍ତୁ idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    ଶୂନ୍ୟ
discard_pending_return(ଶୂନ୍ୟ *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(ଶୂନ୍ୟ *rettv)
{
    char_u	*s = ଖାଲି;
    char_u	*tofree = ଖାଲି;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    ଯଦି (rettv != ଖାଲି)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    ଯଦି (s == ଖାଲି)
	s = (char_u *)"";
    ଅନ୍ୟଥା
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    ଯଦି (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    ଫେରନ୍ତୁ vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_ଧାଡ଼ି(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c UNUSED,
    ଶୂନ୍ୟ    *cookie,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    ଯଦି (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
	func_ଧାଡ଼ି_end(cookie);
#endif

    gap = &fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ;
    ଯଦି (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = ଖାଲି;
    ଅନ୍ୟଥା
    {
	// Skip NULL lines (continuation lines).
	ଯେତେବେଳେ (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == ଖାଲି)
	    ++fcp->fc_linenr;
	ଯଦି (fcp->fc_linenr >= gap->ga_len)
	    retval = ଖାଲି;
	ଅନ୍ୟଥା
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    ଯଦି (do_profiling == PROF_YES)
		func_ଧାଡ଼ି_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    ଯଦି (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_has_ended(ଶୂନ୍ୟ *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    ଫେରନ୍ତୁ (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_has_abort(
    ଶୂନ୍ୟ    *cookie)
{
    ଫେରନ୍ତୁ ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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
    ufunc_T	*fp = ଖାଲି;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    ଯଦି (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != ଖାଲି
				     && rettv->vval.v_partial->pt_func != ଖାଲି)
	fp = rettv->vval.v_partial->pt_func;
    ଅନ୍ୟଥା
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == ଖାଲି ? ଖାଲି
					      : rettv->vval.v_partial->pt_name;
	ଯଦି (fname == ଖାଲି)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    char_u	*tofree = ଖାଲି;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    ଯଦି (fp != ଖାଲି && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	ଯଦି (pt != ଖାଲି)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = ଖାଲି;
	    ଯଦି (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    ଅନ୍ୟଥା
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		ଯଦି (ret_pt->pt_name != ଖାଲି)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		ଅନ୍ୟଥା
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		ଯଦି (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    ଯଦି (pt->pt_argv == ଖାଲି)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    ଅନ୍ୟଥା
		    {
			pt->pt_argc = ret_pt->pt_argc;
			ପାଇଁ (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    ଫେରନ୍ତୁ selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(ଶୂନ୍ୟ *cookie)
{
    ଫେରନ୍ତୁ ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(ଶୂନ୍ୟ *cookie)
{
    ଫେରନ୍ତୁ &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *
func_dbg_tick(ଶୂନ୍ୟ *cookie)
{
    ଫେରନ୍ତୁ &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_level(ଶୂନ୍ୟ *cookie)
{
    ଫେରନ୍ତୁ ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_func_returned(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ current_funccal->fc_returned;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
free_unref_funccal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା testing)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_free = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    ପାଇଁ (pfc = &previous_funccal; *pfc != ଖାଲି; )
    {
	ଯଦି (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	ଅନ୍ୟଥା
	    pfc = &(*pfc)->fc_caller;
    }
    ଯଦି (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(ଶୂନ୍ୟ)garbage_collect(testing);

    ଫେରନ୍ତୁ did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    ସ୍ଥିର funccall_T *
get_funccal(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    ଯଦି (debug_backtrace_level > 0)
    {
	ପାଇଁ (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    ଯଦି (temp_funccal)
		funccal = temp_funccal;
	    ଅନ୍ୟଥା
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    ଫେରନ୍ତୁ funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    ସ୍ଥିର funccall_T *
get_funccal_for_vars(ଶୂନ୍ୟ)
{
    funccall_T	*funccal = ଖାଲି;

    ଯଦି (current_funccal == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    funccal = get_funccal();
    ଯଦି (funccal == ଖାଲି || funccal->fc_l_vars.dv_refcount == 0)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(ଶୂନ୍ୟ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ଫେରନ୍ତୁ funccal == ଖାଲି ? ଖାଲି : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(ଶୂନ୍ୟ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ଫେରନ୍ତୁ funccal == ଖାଲି ? ଖାଲି : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(ଶୂନ୍ୟ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ଫେରନ୍ତୁ funccal == ଖାଲି ? ଖାଲି : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(ଶୂନ୍ୟ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ଫେରନ୍ତୁ funccal == ଖାଲି ? ଖାଲି : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    ଶୂନ୍ୟ
list_func_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first)
{
    ଯଦି (current_funccal != ଖାଲି && current_funccal->fc_l_vars.dv_refcount > 0)
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
    ଯଦି (current_funccal != ଖାଲି
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	ଫେରନ୍ତୁ &current_funccal->fc_l_vars;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = ଖାଲି;
    char_u	*varname;

    ଯଦି (current_funccal == ଖାଲି || current_funccal->fc_func->uf_scoped == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    ଯେତେବେଳେ (current_funccal != ଖାଲି)
    {
	ht = find_var_ht(name, &varname);
	ଯଦି (ht != ଖାଲି && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (current_funccal == current_funccal->fc_func->uf_scoped)
	    ଭାଙ୍ଗ;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ଫେରନ୍ତୁ hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_autoload)
{
    dictitem_T	*v = ଖାଲି;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    ଯଦି (current_funccal == ଖାଲି || current_funccal->fc_func->uf_scoped == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    ଯେତେବେଳେ (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	ଯଦି (ht != ଖାଲି && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    ଯଦି (v != ଖାଲି)
		ଭାଙ୍ଗ;
	}
	ଯଦି (current_funccal == current_funccal->fc_func->uf_scoped)
	    ଭାଙ୍ଗ;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ଫେରନ୍ତୁ v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_previous_funccal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    funccall_T	*fc;

    ପାଇଁ (fc = previous_funccal; fc != ଖାଲି; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	ଯଦି (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, ଖାଲି, ଖାଲି)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, ଖାଲି, ଖାଲି)
		|| set_ref_in_list_ବସ୍ତୁଗୁଡ଼ିକ(&fc->fc_l_varlist, copyID + 1, ଖାଲି, ଖାଲି))
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_funccal(funccall_T *fc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ଯଦି (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	ଯଦି (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, ଖାଲି, ଖାଲି)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, ଖାଲି, ଖାଲି)
		|| set_ref_in_list_ବସ୍ତୁଗୁଡ଼ିକ(&fc->fc_l_varlist, copyID, ଖାଲି, ଖାଲି)
		|| set_ref_in_func(ଖାଲି, fc->fc_func, copyID))
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_call_ଗଦା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    ପାଇଁ (fc = current_funccal; fc != ଖାଲି; fc = fc->fc_caller)
	ଯଦି (set_ref_in_funccal(fc, copyID))
	    ଫେରନ୍ତୁ TRUE;

    // Also go through the funccal_stack.
    ପାଇଁ (entry = funccal_ଗଦା; entry != ଖାଲି; entry = entry->next)
	ପାଇଁ (fc = entry->ଶୀର୍ଷ_funccal; fc != ଖାଲି; fc = fc->fc_caller)
	    ଯଦି (set_ref_in_funccal(fc, copyID))
		ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_functions(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    hashitem_T	*hi = ଖାଲି;
    ufunc_T	*fp;

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)func_hashtab.ht_used;
    ପାଇଁ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ଯଦି (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(ଖାଲି, fp, copyID))
		ଫେରନ୍ତୁ TRUE;
	}
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_func_args(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < funcargs.ga_len; ++i)
	ଯଦି (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, ଖାଲି, ଖାଲି, ଖାଲି))
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_func(char_u *name, ufunc_T *fp_in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ଖାଲି;
    char_u	*fname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort = FALSE;

    ଯଦି (name == ଖାଲି && fp_in == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (fp_in == ଖାଲି)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    ଯଦି (fp != ଖାଲି)
    {
	ପାଇଁ (fc = fp->uf_scoped; fc != ଖାଲି; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    ଫେରନ୍ତୁ abort;
}

#endif // FEAT_EVAL
