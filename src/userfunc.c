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
ساکن hashtab_T	func_hashtab;

// Used by get_func_tv()
ساکن garray_T funcargs = GA_خالی_خانے;

// pointer to funccal for currently active function
ساکن funccall_T *current_funccal = تہی;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
ساکن funccall_T *پچھلا_funccal = تہی;

ساکن خالی funccal_unref(funccall_T *fc, ufunc_T *fp, صحیح force);
ساکن خالی func_clear(ufunc_T *fp, صحیح force);
ساکن صحیح func_free(ufunc_T *fp, صحیح force);
ساکن char_u *untrans_function_name(char_u *name);
ساکن خالی handle_defer_one(funccall_T *funccal);
ساکن char_u *trans_function_name_ext(char_u **pp, صحیح *is_global, صحیح skip, صحیح flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    خالی
func_init(خالی)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(خالی)
{
    واپس &func_hashtab;
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
    ساکن char_u *
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
    char_u	*arg_copy = تہی;
    صحیح		is_underscore = FALSE;

    جب (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    اگر (arg == p || محفوظ_isdigit(*arg)
	    || (argtypes == تہی
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	اگر (!skip)
	    semsg(_(e_illegal_دلیل_str), arg);
	واپس arg;
    }

    // Extra checks in Vim9 script.
    اگر (!skip && argtypes != تہی)
    {
	صحیح c = *p;
	*p = NUL;
	صحیح r = check_reserved_name(arg, FALSE);
	*p = c;
	اگر (r == FAIL)
	    واپس arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	اگر (check_defined(arg, p - arg,
				   evalarg == تہی ? تہی : evalarg->eval_cctx,
			       eap == تہی ? تہی : eap->cstack, TRUE) == FAIL)
	    واپس arg;
    }

    اگر (newargs != تہی && ga_grow(newargs, 1) == FAIL)
	واپس arg;
    اگر (newargs != تہی)
    {
	صحیح	c;
	صحیح	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	اگر (arg_copy == تہی)
	{
	    *p = c;
	    واپس arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	اگر (argtypes == تہی || !is_underscore)
	    // Check for duplicate argument name.
	    برائے (i = 0; i < newargs->ga_len; ++i)
		اگر (STRCMP(((char_u **)(newargs->ga_ڈیٹا))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_دلیل_name_str), arg_copy);
		    vim_free(arg_copy);
		    واپس arg;
		}
	((char_u **)(newargs->ga_ڈیٹا))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    اگر (argtypes != تہی && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != تہی && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = تہی;

	اگر (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == تہی ? arg : arg_copy);
	    p = skipwhite(p);
	}
	اگر (*p == ':')
	{
	    ++p;
	    اگر (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		واپس arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    اگر (!skip)
		type = vim_strnsave(type, p - type);
	}
	ورنہ اگر (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_دلیل_type_for_str),
					    arg_copy == تہی ? arg : arg_copy);
	    واپس arg;
	}
	اگر (!skip)
	{
	    اگر (type == تہی && types_optional)
	    {
		// lambda arguments default to "any" type
		اگر (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		ورنہ
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_ڈیٹا)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_ڈیٹا)[arg_objm->ga_len++] = FALSE;
	}
    }

    واپس p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    ساکن char_u *
get_function_سطر(
	exarg_T		*eap,
	garray_T	*سطریں_مقصد_free,
	صحیح		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    اگر (eap->ea_getline == تہی)
	theline = getcmdline(':', 0L, indent, 0);
    ورنہ
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    اگر (theline != تہی)
    {
	اگر (سطریں_مقصد_free->ga_len > 0
		&& eap->cmdlinep != تہی
		&& *eap->cmdlinep == ((char_u **)سطریں_مقصد_free->ga_ڈیٹا)
						   [سطریں_مقصد_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(خالی)ga_جوڑو_string(سطریں_مقصد_free, theline);
    }

    واپس theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    ساکن صحیح
get_function_دلائل(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    صحیح		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    صحیح		*varargs,
    garray_T	*default_دلائل,
    صحیح		skip,
    exarg_T	*eap,		// can be NULL
    صحیح		اندر_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*سطریں_مقصد_free)
{
    صحیح		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    صحیح		c;
    صحیح		any_default = FALSE;
    char_u	*whitep = *argp;
    صحیح		need_اظہاریہ = FALSE;

    اگر (newargs != تہی)
	ga_init2(newargs, حجم(char_u *), 3);
    اگر (argtypes != تہی)
	ga_init2(argtypes, حجم(char_u *), 3);
    اگر (arg_objm != تہی)
	ga_init2(arg_objm, حجم(int8_T), 3);
    اگر (!skip && default_دلائل != تہی)
	ga_init2(default_دلائل, حجم(char_u *), 3);

    اگر (varargs != تہی)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    جب (*p != endchar)
    {
	جب (eap != تہی && eap->ea_getline != تہی
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_سطر(eap, سطریں_مقصد_free, 0,
							  GETLINE_CONCAT_CONT);

	    اگر (theline == تہی)
		توڑو;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	اگر (mustend && *p != endchar)
	{
	    اگر (!skip)
		semsg(_(e_invalid_دلیل_str), *argp);
	    جاؤ err_ret;
	}
	اگر (*p == endchar && !need_اظہاریہ)
	    توڑو;

	اگر (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    اگر (varargs != تہی)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    اگر (argtypes != تہی)
	    {
		// ...name: list<type>
		اگر (!eval_isnamec1(*p))
		{
		    اگر (!skip)
			emsg(_(e_missing_name_after_dots));
		    جاؤ err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		اگر (p == arg)
		    توڑو;
		اگر (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    توڑو;
		}
	    }
	}
	ورنہ اگر (اندر_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    جب (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    اگر (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_طریقہ_str), arg);
		*argend = c;
		توڑو;
	    }

	    اگر (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		اگر (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_قدر_must_be_vnone_str), p);
		    جاؤ err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		اگر (ga_grow(default_دلائل, 1) == FAIL)
		    جاؤ err_ret;

		char_u *اظہاریہ = vim_strnsave((char_u *)"v:none", 6);
		اگر (اظہاریہ == تہی)
		    جاؤ err_ret;
		((char_u **)(default_دلائل->ga_ڈیٹا))
						 [default_دلائل->ga_len] = اظہاریہ;
		default_دلائل->ga_len++;
	    }
	    ورنہ اگر (any_default)
	    {
		emsg(_(e_non_default_دلیل_follows_default_دلیل));
		جاؤ err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    اگر (newargs != تہی && ga_grow(newargs, 1) == FAIL)
		واپس FAIL;
	    اگر (newargs != تہی)
	    {
		((char_u **)(newargs->ga_ڈیٹا))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		اگر (argtypes != تہی && ga_grow(argtypes, 1) == OK
			    && arg_objm != تہی && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_ڈیٹا)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_ڈیٹا)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    اگر (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			صحیح len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			اگر (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			اگر (assignment != تہی)
			{
			    c = *argend;
			    *argend = NUL;
			    اگر (any_default)
				vim_snprintf((حرف *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_دلائل->ga_len - 1, arg, arg);
			    ورنہ
				vim_snprintf((حرف *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_ڈیٹا))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    اگر (*p == ',')
		++p;
	}
	ورنہ
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    اگر (p == arg)
		توڑو;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    اگر (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_دلائل != تہی)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *اظہاریہ = p;
		اگر (eval1(&p, &rettv, تہی) != FAIL)
		{
		    اگر (!skip)
		    {
			اگر (ga_grow(default_دلائل, 1) == FAIL)
			    جاؤ err_ret;

			اگر (need_اظہاریہ)
			    need_اظہاریہ = FALSE;
			// trim trailing whitespace
			جب (p > اظہاریہ && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			اظہاریہ = vim_strsave(اظہاریہ);
			اگر (اظہاریہ == تہی)
			{
			    *p = c;
			    جاؤ err_ret;
			}
			((char_u **)(default_دلائل->ga_ڈیٹا))
						 [default_دلائل->ga_len] = اظہاریہ;
			default_دلائل->ga_len++;
			*p = c;
		    }
		}
		ورنہ
		{
		    mustend = TRUE;
		    اگر (*skipwhite(p) == NUL)
			need_اظہاریہ = TRUE;
		}
	    }
	    ورنہ اگر (any_default)
	    {
		emsg(_(e_non_default_دلیل_follows_default_دلیل));
		جاؤ err_ret;
	    }

	    اگر (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		اگر (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    جاؤ err_ret;
		}
		p = skipwhite(p);
	    }
	    اگر (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		اگر (!skip && argtypes != تہی
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    جاؤ err_ret;
		}
	    }
	    ورنہ
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    اگر (*p != endchar)
	جاؤ err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    واپس OK;

err_ret:
    اگر (newargs != تہی)
	ga_clear_لڑیاں(newargs);
    اگر (!skip && default_دلائل != تہی)
	ga_clear_لڑیاں(default_دلائل);
    واپس FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    ساکن صحیح
parse_دلیل_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    صحیح		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    صحیح		obj_member_گنتی,
    cctx_T	*cctx)
{
    صحیح len = 0;

    ga_init2(&fp->uf_type_list, حجم(type_T *), 10);
    اگر (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	اگر (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	اگر (fp->uf_arg_types != تہی)
	{
	    صحیح	i;
	    type_T	*type;

	    برائے (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_ڈیٹا)[i];

		اگر (p == تہی)
		    // will get the type from the default value
		    type = &t_unknown;
		ورنہ
		{
		    اگر (arg_objm != تہی && ((int8_T *)arg_objm->ga_ڈیٹا)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_دلائل.ga_ڈیٹا)[i];

			type = &t_any;
			برائے (صحیح om = 0; om < obj_member_گنتی; ++om)
			{
			    اگر (obj_members != تہی
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				توڑو;
			    }
			}
		    }
		    ورنہ
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		اگر (type == تہی || !valid_declaration_type(type))
		    واپس FAIL;
		fp->uf_arg_types[i] = type;
		اگر (i < fp->uf_دلائل.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_دلائل.ga_ڈیٹا)[i];
		    اگر (obj_members != تہی && *name == '_')
			// protected object method
			name++;

		    اگر (var_wrong_func_name(name, TRUE))
			واپس FAIL;
		}
	    }
	}
    }

    اگر (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_دلائل.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_دلائل.ga_ڈیٹا)[fp->uf_دلائل.ga_len];
	((char_u **)fp->uf_دلائل.ga_ڈیٹا)[fp->uf_دلائل.ga_len] = تہی;
	p = ((char_u **)argtypes->ga_ڈیٹا)[len];
	اگر (p == تہی)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	ورنہ
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    اگر (fp->uf_va_type != تہی && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_ڈیٹا)[len]);
		واپس FAIL;
	    }
	}
	اگر (fp->uf_va_type == تہی)
	    واپس FAIL;
    }

    واپس OK;
}

    ساکن صحیح
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    اگر (ret_type == تہی)
	fp->uf_ret_type = &t_void;
    ورنہ
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	اگر (fp->uf_ret_type == تہی)
	{
	    fp->uf_ret_type = &t_void;
	    واپس FAIL;
	}
    }
    واپس OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    ساکن صحیح
رجسٹر_احاطہ(ufunc_T *fp)
{
    اگر (fp->uf_scoped == current_funccal)
	// no change
	واپس OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    اگر (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	واپس FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_ڈیٹا)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    واپس OK;
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
	واپس name;
    len = STRLEN(name);
    اگر (len + 3 > bufsize)
	واپس name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    واپس buf;
}

ساکن char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(خالی)
{
    ساکن صحیح	lambda_no = 0;
    صحیح		n;
    string_T	ret;

    n = vim_snprintf((حرف *)lambda_name, حجم(lambda_name), "<lambda>%d", ++lambda_no);
    اگر (n < 1)
	ret.لمبائی = 0;
    ورنہ اگر (n >= (صحیح)حجم(lambda_name))
	ret.لمبائی = حجم(lambda_name) - 1;
    ورنہ
	ret.لمبائی = (size_t)n;

    ret.string = lambda_name;
    واپس ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    ساکن ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < حجم(ufunc_T) ? حجم(ufunc_T) : len);
    اگر (fp != تہی)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((خالی *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	اگر (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    اگر (fp->uf_name_exp != تہی)
		vim_snprintf((حرف *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    واپس fp;
}

#اگر defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
رجسٹر_cfunc(cfunc_T cb, cfunc_free_T cb_free, خالی *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.لمبائی);
    اگر (fp == تہی)
	واپس تہی;

    fp->uf_def_حالت = UF_NOT_COMPILED;
    fp->uf_refcount = 1;
    fp->uf_varargs = TRUE;
    fp->uf_flags = FC_CFUNC | FC_LAMBDA;
    fp->uf_calls = 0;
    fp->uf_script_ctx = current_sctx;
    fp->uf_cb = cb;
    fp->uf_cb_free = cb_free;
    fp->uf_cb_state = state;

    hash_جوڑو(&func_hashtab, UF2HIKEY(fp), "add C function");

    واپس name.string;
}
#اگر_ختم

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    ساکن char_u *
skip_arrow(
	char_u	*آغاز,
	صحیح	equal_arrow,
	char_u	**ret_type,
	صحیح	*white_error)
{
    char_u  *s = آغاز;
    char_u  *bef = آغاز - 2; // "start" points to > of ->

    اگر (equal_arrow)
    {
	اگر (*s == ':')
	{
	    اگر (white_error != تہی && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		واپس تہی;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    اگر (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		واپس تہی;
	    }
	}
	bef = s;
	s = skipwhite(s);
	اگر (*s != '=')
	    واپس تہی;
	++s;
    }
    اگر (*s != '>')
	واپس تہی;
    اگر (white_error != تہی && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	واپس تہی;
    }
    واپس skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    ساکن صحیح
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    اگر (checkforcmd(&p, "function", 2))
    {
	اگر (*p == '(')
	    واپس FALSE;
	*cmd = p;
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    ساکن خالی
function_using_بلاک_scopes(ufunc_T *fp, cstack_T *cstack)
{
    اگر (cstack == تہی || cstack->cs_idx < 0)
	واپس;

    صحیح	    گنتی = cstack->cs_idx + 1;
    صحیح	    i;

    fp->uf_بلاک_ids = ALLOC_MULT(صحیح, گنتی);
    اگر (fp->uf_بلاک_ids != تہی)
    {
	mch_memmove(fp->uf_بلاک_ids, cstack->cs_بلاک_id,
		حجم(صحیح) * گنتی);
	fp->uf_بلاک_گہرائی = گنتی;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    برائے (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    ساکن char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    جب (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	اگر (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	ورنہ اگر (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    توڑو;
	}
	ورنہ
	    MB_PTR_ADV(p);
    }

    واپس p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    ساکن char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    جب (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	اگر (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	ورنہ اگر (*p == '"')
	{
	    p++;    // skip the ending double quote
	    توڑو;
	}
	ورنہ
	    MB_PTR_ADV(p);
    }

    واپس p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    ساکن char_u *
ڈھونڈو_آغاز_کا_vim9_comment(char_u *سطر)
{
    char_u	*p = سطر;

    جب (*p != NUL)
    {
	اگر (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	ورنہ اگر (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	ورنہ
	{
	    اگر (*p == '#')
		// Found the start of a Vim9 comment
		توڑو;
	    MB_PTR_ADV(p);
	}
    }

    واپس p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    ساکن صحیح
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *سطر_arg_اندر,
	garray_T    *سطریں_مقصد_free)
{
    linenr_T	sourcing_lnum_چوٹی = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    صحیح		saved_wait_return = need_wait_return;
    char_u	*سطر_arg = سطر_arg_اندر;
    صحیح		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_بلاک;
#تعریف_کرو MAX_FUNC_NESTING 50
    حرف	nesting_def[MAX_FUNC_NESTING];
    حرف	nesting_inline[MAX_FUNC_NESTING];
    صحیح		nesting = 0;
    getline_opt_T getline_options;
    صحیح		indent = 2;
    char_u	*skip_until = تہی;
    صحیح		ret = FAIL;
    صحیح		is_heredoc = FALSE;
    صحیح		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = تہی;
    size_t	heredoc_trimmedlen = 0;
    صحیح		did_emsg_آغاز = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    اگر (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	اگر (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    جاؤ theend;
	جب (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_ڈیٹا))[newlines->ga_len++] = تہی;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_بلاک;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    برائے (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	اگر (کلید_Typed)
	{
	    پیغام_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	اگر (سطر_arg != تہی)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = سطر_arg;
	    p = vim_strchr(theline, '\n');
	    اگر (p == تہی)
		سطر_arg += STRLEN(سطر_arg);
	    ورنہ
	    {
		*p = NUL;
		سطر_arg = p + 1;
	    }
	}
	ورنہ
	{
	    theline = get_function_سطر(eap, سطریں_مقصد_free, indent,
							      getline_options);
	}
	اگر (کلید_Typed)
	    سطریں_بایاں = صفیں - 1;
	اگر (theline == تہی)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_چوٹی;
	    اگر (skip_until != تہی)
		semsg(_(e_missing_heredoc_انتہا_marker_str), skip_until);
	    ورنہ اگر (nesting_inline[nesting])
		emsg(_(e_missing_انتہا_بلاک));
	    ورنہ اگر (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    ورنہ
		emsg(_(e_missing_endfunction));
	    جاؤ theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	اگر (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	ورنہ
	    sourcing_lnum_off = 0;

	اگر (skip_until != تہی)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    اگر (heredoc_trimmed == تہی
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		اگر (heredoc_trimmed == تہی)
		    p = theline;
		ورنہ اگر (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		ورنہ
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
			vim_free(((char_u **)(newlines->ga_ڈیٹا))[
						      heredoc_concat_len - 1]);
			((char_u **)(newlines->ga_ڈیٹا))[
				  heredoc_concat_len - 1] = heredoc_ga.ga_ڈیٹا;
			ga_init(&heredoc_ga);
			heredoc_concat_len = 0;
			theline += STRLEN(theline);  // skip the "EOF"
		    }
		}
	    }
	}
	ورنہ
	{
	    صحیح	    c;
	    char_u  *انتہا;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    برائے (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
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
		    char_u *nextcmd = تہی;

		    اگر (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    ورنہ اگر (سطر_arg != تہی && *skipwhite(سطر_arg) != NUL)
			nextcmd = سطر_arg;
		    ورنہ اگر (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_چوٹی
							+ newlines->ga_len + 1;
			اگر (eap->cmdidx == CMD_def)
			    semsg(_(e_متن_ملا_after_str_str), "enddef", p);
			ورنہ
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    اگر (nextcmd != تہی && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			اگر (سطریں_مقصد_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)سطریں_مقصد_free->ga_ڈیٹا)
						   [سطریں_مقصد_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)سطریں_مقصد_free->ga_ڈیٹا)
						   [سطریں_مقصد_free->ga_len - 1];
			    --سطریں_مقصد_free->ga_len;
			}
		    }
		    توڑو;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    ورنہ اگر (nesting_def[nesting])
	    {
		اگر (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    ورنہ اگر (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    اگر (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    ورنہ اگر (STRNCMP(p, "if", 2) == 0
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
		vim_free(trans_function_name(&p, تہی, TRUE, 0));
		اگر (vim9_function && *p == '<')
		{
		    // skip generic function
		    اگر (skip_generic_func_type_دلائل(&p) == FAIL)
			جاؤ theend;
		}
		اگر (*skipwhite(p) == '(')
		{
		    اگر (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    ورنہ
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
		    انتہا = ڈھونڈو_آغاز_کا_vim9_comment(p) - 1;
		}
		ورنہ
		    انتہا = p + STRLEN(p) - 1;

		جب (انتہا > p && VIM_ISWHITE(*انتہا))
		    --انتہا;
		اگر (انتہا > p + 1 && *انتہا == '{' && VIM_ISWHITE(انتہا[-1]))
		{
		    صحیح	    is_بلاک;

		    // check for trailing "=> {": start of an inline function
		    --انتہا;
		    جب (انتہا > p && VIM_ISWHITE(*انتہا))
			--انتہا;
		    is_بلاک = انتہا > p + 2 && انتہا[-1] == '=' && انتہا[0] == '>';
		    اگر (!is_بلاک)
			is_بلاک = ڈھونڈو_cmd_بلاک_آغاز(p) != تہی;

		    اگر (is_بلاک)
		    {
			اگر (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			ورنہ
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
		p = skip_range(p, FALSE, تہی);
	    char_u *tp = p;
	    اگر (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    ورنہ
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
		    اگر (heredoc_trimmed == تہی)
			heredoc_trimmedlen = 0;
		}
		اگر (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		ورنہ
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
		    صحیح		save_sc_نسخہ = current_sctx.sc_نسخہ;
		    صحیح		var_گنتی = 0;
		    صحیح		semicolon = 0;

		    current_sctx.sc_نسخہ
				     = vim9_function ? SCRIPT_نسخہ_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_گنتی, &semicolon,
									 TRUE);
		    اگر (arg != تہی)
			arg = skipwhite(arg);
		    current_sctx.sc_نسخہ = save_sc_نسخہ;
		    اگر (arg != تہی && STRNCMP(arg, "=<<", 3) == 0)
		    {
			صحیح has_trim = FALSE;

			p = skipwhite(arg + 3);
			جب (TRUE)
			{
			    اگر (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				جاری;
			    }
			    اگر (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				جاری;
			    }
			    توڑو;
			}
			اگر (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    اگر (heredoc_trimmed == تہی)
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
	    جاؤ theend;

	اگر (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	ورنہ
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	اگر (p == تہی)
	    جاؤ theend;
	((char_u **)(newlines->ga_ڈیٹا))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	جب (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_ڈیٹا))[newlines->ga_len++] = تہی;

	// Check for end of eap->arg.
	اگر (سطر_arg != تہی && *سطر_arg == NUL)
	    سطر_arg = تہی;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    اگر (did_emsg == did_emsg_آغاز)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_ڈیٹا);
    need_wait_return |= saved_wait_return;
    واپس ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    ساکن صحیح
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	صحیح	    varargs,
	garray_T    *default_دلائل,
	char_u	    *ret_type)
{
    char_u	*آغاز = *arg;
    صحیح		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = تہی;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = تہی;
    صحیح		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    صحیح		lnum_save = -1;
    linenr_T	sourcing_lnum_چوٹی = SOURCING_LNUM;
    char_u	*سطر_arg = تہی;

    *arg = skipwhite(*arg + 1);
    اگر (**arg == '|' || !سرے_excmd2(آغاز, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	واپس FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    اگر (**arg == '\n')
	سطر_arg = *arg + 1;

    CLEAR_میدان(eap);
    eap.cmdidx = CMD_بلاک;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    اگر (evalarg->eval_cctx != تہی)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    ورنہ
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, حجم(char_u *), 10);
    اگر (get_function_body(&eap, &newlines, سطر_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	جاؤ erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_گنتی += newlines.ga_len;
    اگر (gap->ga_itemsize > 0)
    {
	صحیح	idx;
	char_u	*آخری;
	size_t  plen;
	char_u  *pnl;

	برائے (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_ڈیٹا)[idx];
	    اگر (p == تہی)
		// comment line in the lambda body
		جاری;

	    p = skipwhite(p);

	    اگر (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		جاؤ erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    اگر (*p == NUL || vim9_comment_آغاز(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    ورنہ
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    اگر (pnl != تہی)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_ڈیٹا)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_ڈیٹا)[freegap->ga_len++] = pnl;
	}
	اگر (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    جاؤ erret;
	اگر (eap.nextcmd != تہی)
	{
	    // more is following after the "}", which was skipped
	    آخری = cmdline;
	    plen = STRLEN(آخری);
	}
	ورنہ
	{
	    // nothing is following the "}"
	    آخری = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	اگر (pnl != تہی)
	    mch_memmove(pnl + 1, آخری, plen + 1);
	((char_u **)gap->ga_ڈیٹا)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_ڈیٹا)[freegap->ga_len++] = pnl;
    }

    اگر (eap.nextcmd != تہی)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	اگر (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_ڈیٹا))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    ورنہ
	*arg = (char_u *)"";

    اگر (!evaluate)
    {
	ret = OK;
	جاؤ erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.لمبائی);
    اگر (ufunc == تہی)
	جاؤ erret;
    اگر (hash_جوڑو(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	جاؤ erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_دلائل = *newargs;
    newargs->ga_ڈیٹا = تہی;
    ufunc->uf_def_دلائل = *default_دلائل;
    default_دلائل->ga_ڈیٹا = تہی;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_چوٹی;

    // parse argument types, the type variables of the function being
    // compiled may be used
    اگر (parse_دلیل_types(ufunc, argtypes, varargs, تہی, تہی, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	جاؤ erret;
    }

    // parse the return type, if any
    اگر (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	جاؤ erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    اگر (pt == تہی)
	جاؤ erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_سطریں = newlines;
    newlines.ga_ڈیٹا = تہی;
    اگر (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    اگر (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_نسخہ = current_sctx.sc_نسخہ;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_چوٹی;
    سیٹ_function_type(ufunc);

    function_using_بلاک_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = تہی;
    ret = OK;

erret:
    اگر (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_لڑیاں(&newlines);
    اگر (newargs != تہی)
	ga_clear_لڑیاں(newargs);
    ga_clear_لڑیاں(default_دلائل);
    اگر (ufunc != تہی)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    واپس ret;
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
    صحیح		evaluate = evalarg != تہی
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_دلائل;
    garray_T	arg_objm;
    ufunc_T	*fp = تہی;
    partial_T   *pt = تہی;
    صحیح		varargs;
    char_u	*ret_type = تہی;
    صحیح		ret;
    char_u	*s;
    char_u	*آغاز, *انتہا;
    صحیح		*old_eval_lavars = eval_lavars_مستعمل;
    صحیح		eval_lavars = FALSE;
    char_u	*tofree2 = تہی;
    صحیح		equal_arrow = **arg == '(';
    صحیح		white_error = FALSE;
    صحیح		called_emsg_آغاز = called_emsg;
    صحیح		vim9script = اندر_vim9script();
    طویل	آغاز_lnum = SOURCING_LNUM;

    اگر (equal_arrow && !vim9script)
	واپس NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_دلائل(&s, equal_arrow ? ')' : '-', تہی,
	    types_optional ? &argtypes : تہی, types_optional,
			types_optional ? &arg_objm : تہی, evalarg,
			تہی, &default_دلائل, TRUE, تہی, FALSE, تہی, تہی);
    اگر (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, تہی) == تہی)
    {
	اگر (types_optional)
	{
	    ga_clear_لڑیاں(&argtypes);
	    ga_clear(&arg_objm);
	}
	واپس called_emsg == called_emsg_آغاز ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    اگر (evaluate)
	pnewargs = &newargs;
    ورنہ
	pnewargs = تہی;
    *arg += 1;
    ret = get_function_دلائل(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : تہی, types_optional,
			types_optional ? &arg_objm : تہی, evalarg,
					    &varargs, &default_دلائل,
					    FALSE, تہی, FALSE, تہی, تہی);
    اگر (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : تہی)) == تہی)
    {
	اگر (types_optional)
	{
	    ga_clear_لڑیاں(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_لڑیاں(&newargs);
	واپس white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    اگر (ret_type != تہی)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    اگر (evaluate)
	eval_lavars_مستعمل = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    اگر (equal_arrow && **arg == '{')
    {
	اگر (evalarg == تہی)
	    // cannot happen?
	    جاؤ theend;
	SOURCING_LNUM = آغاز_lnum;  // used for where lambda is defined
	اگر (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : تہی, varargs,
			   &default_دلائل, ret_type) == FAIL)
	    جاؤ errret;
	جاؤ theend;
    }
    اگر (default_دلائل.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_اقدار_اندر_lambda));
	جاؤ errret;
    }

    // Get the start and the end of the expression.
    آغاز = *arg;
    ret = skip_اظہاریہ_concatenate(arg, &آغاز, &انتہا, evalarg);
    اگر (ret == FAIL)
	جاؤ errret;

    اگر (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg != '}')
	{
	    semsg(_(e_expected_دایاں_curly_str), *arg);
	    جاؤ errret;
	}
	++*arg;
    }

    اگر (evaluate)
    {
	صحیح	    len;
	صحیح	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *سطر_انتہا;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.لمبائی);
	اگر (fp == تہی)
	    جاؤ errret;
	fp->uf_def_حالت = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	اگر (pt == تہی)
	    جاؤ errret;

	ga_init2(&newlines, حجم(char_u *), 1);
	اگر (ga_grow(&newlines, 1) == FAIL)
	    جاؤ errret;

	// If there are line breaks, we need to split up the string.
	سطر_انتہا = vim_strchr(آغاز, '\n');
	اگر (سطر_انتہا == تہی || سطر_انتہا > انتہا)
	    سطر_انتہا = انتہا;

	// Add "return " before the expression (or the first line).
	len = 7 + (صحیح)(سطر_انتہا - آغاز) + 1;
	p = alloc(len);
	اگر (p == تہی)
	    جاؤ errret;
	((char_u **)(newlines.ga_ڈیٹا))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, آغاز, سطر_انتہا - آغاز);

	اگر (سطر_انتہا != انتہا)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    جب (سطر_انتہا < انتہا && *سطر_انتہا == '\n')
	    {
		اگر (ga_grow(&newlines, 1) == FAIL)
		    جاؤ errret;
		آغاز = سطر_انتہا + 1;
		سطر_انتہا = vim_strchr(آغاز, '\n');
		اگر (سطر_انتہا == تہی || سطر_انتہا > انتہا)
		    سطر_انتہا = انتہا;
		((char_u **)(newlines.ga_ڈیٹا))[newlines.ga_len++] =
					 vim_strnsave(آغاز, سطر_انتہا - آغاز);
	    }
	}

	اگر (لڑی_تلاش((حرف *)p + 7, "a:") == تہی)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_دلائل = newargs;
	ga_init(&fp->uf_def_دلائل);
	اگر (types_optional)
	{
	    اگر (parse_دلیل_types(fp, &argtypes,
				vim9script && varargs, تہی, تہی, 0,
				cctx) == FAIL)
		جاؤ errret;
	    اگر (ret_type != تہی)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     تہی, cctx, TRUE);
		اگر (fp->uf_ret_type == تہی)
		    جاؤ errret;
	    }
	    ورنہ
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_سطریں = newlines;
	اگر (current_funccal != تہی && eval_lavars)
	{
	    flags |= FC_احاطہ;
	    اگر (رجسٹر_احاطہ(fp) == FAIL)
		جاؤ errret;
	}

#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (prof_def_func())
	    func_do_profile(fp);
#اگر_ختم
	اگر (sandbox)
	    flags |= FC_SANDBOX;
	// In legacy script a lambda can be called with more args than
	// uf_args.ga_len.  In Vim9 script "...name" has to be used.
	fp->uf_varargs = !vim9script || varargs;
	fp->uf_flags = flags;
	fp->uf_calls = 0;
	fp->uf_script_ctx = current_sctx;
	// Use the line number of the arguments.
	fp->uf_script_ctx.sc_lnum += آغاز_lnum;

	function_using_بلاک_scopes(fp, evalarg->eval_cstack);

	pt->pt_func = fp;
	pt->pt_refcount = 1;
	rettv->vval.v_partial = pt;
	rettv->v_type = VAR_PARTIAL;

	hash_جوڑو(&func_hashtab, UF2HIKEY(fp), "add lambda");
    }

theend:
    eval_lavars_مستعمل = old_eval_lavars;
    vim_free(tofree2);
    اگر (types_optional)
    {
	ga_clear_لڑیاں(&argtypes);
	ga_clear(&arg_objm);
    }

    واپس OK;

errret:
    ga_clear_لڑیاں(&newargs);
    ga_clear_لڑیاں(&newlines);
    ga_clear_لڑیاں(&default_دلائل);
    اگر (types_optional)
    {
	ga_clear_لڑیاں(&argtypes);
	ga_clear(&arg_objm);
    }
    اگر (fp != تہی)
    {
	vim_free(fp->uf_arg_types);
	vim_free(fp->uf_name_exp);
	vim_free(fp->uf_va_name);
	clear_type_list(&fp->uf_type_list);
	vim_free(fp);
    }
    vim_free(pt);
    vim_free(tofree2);
    eval_lavars_مستعمل = old_eval_lavars;
    واپس FAIL;
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
	صحیح	    *ملا_var)
{
    dictitem_T	*v;
    typval_T	*tv = تہی;
    صحیح		cc;
    char_u	*s = تہی;
    hashtab_T	*ht;
    صحیح		did_type = FALSE;

    اگر (partialp != تہی)
	*partialp = تہی;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = ڈھونڈو_var_also_اندر_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    اگر (v != تہی)
    {
	tv = &v->di_tv;
    }
    ورنہ اگر (اندر_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	صحیح	    len = *lenp;

	اگر (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = ڈھونڈو_imported(p, len, FALSE);

	// imported function from another script
	اگر (import != تہی)
	{
	    name[len] = NUL;
	    اگر (new_function)
		semsg(_(e_redefining_imported_شے_str), name);
	    ورنہ
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    واپس (char_u *)"";	// just in case
	}
    }

    اگر (tv != تہی)
    {
	اگر (ملا_var != تہی)
	    *ملا_var = TRUE;
	اگر (tv->v_type == VAR_FUNC)
	{
	    اگر (tv->vval.v_string == تہی)
	    {
		*lenp = 0;
		واپس (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_ڈھونڈو_open_bracket(s);
	    اگر (p != تہی && generic_func_ڈھونڈو_open_bracket(name) != تہی)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		واپس (char_u *)"";
	    }
	    اگر (p != تہی)
		*lenp = p - s;
	    ورنہ
		*lenp = (صحیح)STRLEN(s);
	}
	ورنہ اگر (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    اگر (pt == تہی)
	    {
		*lenp = 0;
		واپس (char_u *)"";	// just in case
	    }
	    اگر (partialp != تہی)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (صحیح)STRLEN(s);
	}

	اگر (s != تہی)
	{
	    اگر (!did_type && type != تہی && ht == get_script_local_ht())
	    {
		svar_T  *sv = ڈھونڈو_typval_اندر_script(tv, 0, TRUE);

		اگر (sv != تہی)
		    *type = sv->sv_type;
	    }
	    واپس s;
	}
    }

    واپس name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    خالی
emsg_funcname(حرف *ermsg, char_u *name)
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
    صحیح		vim9script = اندر_vim9script();
    صحیح		evaluate = evalarg == تہی
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    جب (*argcount < MAX_FUNC_دلائل - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	اگر (*argp == ')' || *argp == ',' || *argp == NUL)
	    توڑو;

	صحیح arg_idx = *argcount;
	اگر (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    توڑو;
	}
	++*argcount;
	اگر (!is_builtin && check_typval_is_قدر(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    توڑو;
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
		توڑو;
	    }
	}
	ورنہ
	    argp = skipwhite(argp);
	اگر (*argp != ',')
	    توڑو;
	اگر (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    اگر (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    توڑو;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    اگر (*argp == ')')
	++argp;
    ورنہ
	ret = FAIL;
    *arg = argp;
    واپس ret;
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
    typval_T	argvars[MAX_FUNC_دلائل + 1];	// vars for arguments
    صحیح		argcount = 0;			// number of arguments found
    صحیح		vim9script = اندر_vim9script();
    صحیح		evaluate = evalarg == تہی
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == تہی ? 0 : funcexe->fe_partial->pt_argc),
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
		ga_init2(&funcargs, حجم(typval_T *), 50);
	    برائے (i = 0; i < argcount; ++i)
		اگر (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_ڈیٹا)[funcargs.ga_len++] =
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
    ورنہ اگر (!aborting() && evaluate)
    {
	اگر (argcount == MAX_FUNC_دلائل)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	ورنہ
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    جب (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    اگر (vim9script)
	*arg = argp;
    ورنہ
	*arg = skipwhite(argp);
    واپس ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    ساکن صحیح
eval_fname_sid(char_u *p)
{
    واپس (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
	واپس name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (صحیح)KE_SNR;
    fname_buflen = 3;
    اگر (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    ورنہ
    {
	اگر (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	ورنہ
	{
	    fname_buflen += vim_snprintf((حرف *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(طویل)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    اگر (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    ورنہ
    {
	fname = alloc(fnamelen + 1);
	اگر (fname == تہی)
	    *error = FCERR_OTHER;
	ورنہ
	{
	    *tofree = fname;
	    vim_snprintf((حرف *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    واپس fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    خالی
func_name_with_sid(char_u *name, صحیح sid, char_u *بفر)
{
    // A script-local function is stored as "<SNR>99_name".
    بفر[0] = K_SPECIAL;
    بفر[1] = KS_EXTRA;
    بفر[2] = (صحیح)KE_SNR;
    vim_snprintf((حرف *)بفر + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (طویل)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    ساکن ufunc_T *
ڈھونڈو_func_with_sid(char_u *name, صحیح sid)
{
    hashitem_T	    *hi;
    char_u	    بفر[MAX_FUNC_NAME_LEN];

    اگر (!SCRIPT_ID_VALID(sid))
	واپس تہی;	// not in a script

    func_name_with_sid(name, sid, بفر);
    hi = hash_ڈھونڈو(&func_hashtab, بفر);
    اگر (!HASHITEM_خالی_خانے(hi))
	واپس HI2UF(hi);
    واپس تہی;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    ساکن ufunc_T *
ڈھونڈو_func_with_سابقہ(char_u *name, صحیح sid)
{
    hashitem_T	    *hi;
    char_u	    بفر[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    اگر (vim_strchr(name, AUTOLOAD_CHAR) != تہی)
	واپس تہی;	// already has the prefix
    اگر (!SCRIPT_ID_VALID(sid))
	واپس تہی;	// not in a script
    si = SCRIPT_شے(sid);
    اگر (si->sn_autoload_سابقہ != تہی)
    {
	size_t	len = STRLEN(si->sn_autoload_سابقہ) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	اگر (namep == تہی)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	اگر (len < حجم(بفر))
	    auto_name = بفر;
	ورنہ
	    auto_name = alloc(len);
	اگر (auto_name != تہی)
	{
	    vim_snprintf((حرف *)auto_name, len, "%s%s",
						si->sn_autoload_سابقہ, namep);
	    hi = hash_ڈھونڈو(&func_hashtab, auto_name);
	    اگر (auto_name != بفر)
		vim_free(auto_name);
	    اگر (!HASHITEM_خالی_خانے(hi))
		واپس HI2UF(hi);
	}
    }

    واپس تہی;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
ڈھونڈو_func_even_dead(char_u *name, صحیح flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    اگر ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	اگر (اندر_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = ڈھونڈو_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    اگر (func != تہی)
		واپس func;
	}
	اگر (اندر_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    طویل    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    اگر (*p == '_')
	    {
		func = ڈھونڈو_func_with_sid(p + 1, (صحیح)sid);
		اگر (func != تہی)
		    واپس func;
	    }
	}
    }

    اگر ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_ڈھونڈو(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	اگر (!HASHITEM_خالی_خانے(hi))
	    واپس HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    واپس ڈھونڈو_func_with_سابقہ(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
ڈھونڈو_func(char_u *name, صحیح is_global)
{
    ufunc_T	*fp = ڈھونڈو_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    اگر (fp != تہی && (fp->uf_flags & FC_DEAD) == 0)
	واپس fp;
    واپس تہی;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    صحیح
func_is_global(ufunc_T *ufunc)
{
    واپس ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    صحیح
func_requires_g_سابقہ(ufunc_T *ufunc)
{
    واپس func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == تہی
	    && !محفوظ_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    ساکن صحیح
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    صحیح	len;

    اگر (!func_is_global(fp))
	len = vim_snprintf((حرف *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    ورنہ
	len = vim_snprintf((حرف *)buf, bufsize, "%s", fp->uf_name);

    واپس (len >= (صحیح)bufsize) ? (صحیح)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    ساکن خالی
جوڑو_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    حرف	*name,
    varnumber_T nr)
{
    STRCPY(v->di_کلید, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_جوڑو(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_نمبر;
    v->di_tv.v_تالا = VAR_FIXED;
    v->di_tv.vval.v_نمبر = nr;
}

/*
 * Free "fc".
 */
    ساکن خالی
free_funccal(funccall_T *fc)
{
    صحیح	i;

    برائے (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_ڈیٹا))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	اگر (fp != تہی && fp->uf_scoped == fc)
	    fp->uf_scoped = تہی;
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
   ساکن خالی
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_اشیاء(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    ساکن خالی
cleanup_function_call(funccall_T *fc)
{
    صحیح	may_free_fc = fc->fc_refcount <= 0;
    صحیح	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    اگر (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    ورنہ
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    اگر (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    ورنہ
    {
	صحیح	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (صحیح)fc->fc_l_avars.dv_hashtab.ht_مستعمل;
	FOR_ALL_HASHTAB_اشیاء(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    اگر (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_پہلا = تہی;
    ورنہ
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_اشیاء(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    اگر (free_fc)
	free_funccal(fc);
    ورنہ
    {
	ساکن صحیح made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = پچھلا_funccal;
	پچھلا_funccal = fc;

	اگر (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	ورنہ اگر (++made_copy >= (صحیح)((4096 * 1024) / حجم(*fc)))
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
    ساکن صحیح
numbered_function(char_u *name)
{
    واپس محفوظ_isdigit(*name)
	    || (name[0] == 'g' && name[1] == ':' && محفوظ_isdigit(name[2]));
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
    واپس numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    ساکن خالی
funccal_unref(funccall_T *fc, ufunc_T *fp, صحیح force)
{
    funccall_T	**pfc;
    صحیح		i;

    اگر (fc == تہی)
	واپس;

    اگر (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	برائے (pfc = &پچھلا_funccal; *pfc != تہی; pfc = &(*pfc)->fc_caller)
	{
	    اگر (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		واپس;
	    }
	}
    برائے (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	اگر (((ufunc_T **)(fc->fc_ufuncs.ga_ڈیٹا))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_ڈیٹا))[i] = تہی;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    ساکن صحیح
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    اگر (fp->uf_flags & FC_DEAD)
	واپس FALSE;

    hi = hash_ڈھونڈو(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    اگر (HASHITEM_خالی_خانے(hi) || HI2UF(hi) != fp)
	واپس FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    اگر (fp->uf_def_حالت == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	واپس FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    واپس TRUE;
}

    ساکن خالی
func_clear_اشیاء(ufunc_T *fp)
{
    ga_clear_لڑیاں(&(fp->uf_دلائل));
    ga_clear_لڑیاں(&(fp->uf_def_دلائل));
    ga_clear_لڑیاں(&(fp->uf_سطریں));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_بلاک_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    اگر (IS_GENERIC_FUNC(fp))
	generic_func_clear_اشیاء(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = تہی;
    fp->uf_refcount -= 3;

#اگر_تعریف_شدہ FEAT_LUA
    اگر (fp->uf_cb_free != تہی)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = تہی;
    }

    fp->uf_cb_state = تہی;
    fp->uf_cb = تہی;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_گنتی);
    VIM_CLEAR(fp->uf_tml_کل);
    VIM_CLEAR(fp->uf_tml_self);
#اگر_ختم
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    ساکن خالی
func_clear(ufunc_T *fp, صحیح force)
{
    اگر (fp->uf_cleared)
	واپس;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_اشیاء(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    ساکن صحیح
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
	واپس OK;
    }
    واپس FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    خالی
func_clear_free(ufunc_T *fp, صحیح force)
{
    func_clear(fp, force);
    اگر (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    ورنہ
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    صحیح
copy_lambda_مقصد_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = ڈھونڈو_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = تہی;

    اگر (ufunc == تہی)
    {
	semsg(_(e_lambda_function_not_ملا_str), lambda);
	واپس FAIL;
    }

    fp = ڈھونڈو_func(global, TRUE);
    اگر (fp != تہی)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_جوڑو_bang_مقصد_replace), global);
	واپس FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    اگر (fp == تہی)
	واپس FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_حالت = ufunc->uf_def_حالت;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    اگر (ga_copy_لڑیاں(&ufunc->uf_دلائل, &fp->uf_دلائل) == FAIL
	    || ga_copy_لڑیاں(&ufunc->uf_def_دلائل, &fp->uf_def_دلائل)
								    == FAIL
	    || ga_copy_لڑیاں(&ufunc->uf_سطریں, &fp->uf_سطریں) == FAIL)
	جاؤ failed;

    اگر (ufunc->uf_arg_types != تہی)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_دلائل.ga_len);
	اگر (fp->uf_arg_types == تہی)
	    جاؤ failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    حجم(type_T *) * fp->uf_دلائل.ga_len);
    }
    اگر (ufunc->uf_va_name != تہی)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	اگر (fp->uf_va_name == تہی)
	    جاؤ failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_جوڑو(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    اگر ((ufunc->uf_flags & FC_احاطہ) && ufunc->uf_partial == تہی)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	اگر (pt == تہی)
	    جاؤ failed;
	اگر (fill_partial_and_احاطہ(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    جاؤ failed;
	}
	ufunc->uf_partial = pt;
    }

    واپس OK;

failed:
    func_clear_free(fp, TRUE);
    واپس FAIL;
}

ساکن صحیح	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    صحیح
funcdepth_increment(خالی)
{
    اگر (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_گہرائی_is_higher_than_maxfuncdepth));
	واپس FAIL;
    }
    ++funcdepth;
    واپس OK;
}

    خالی
funcdepth_decrement(خالی)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    صحیح
funcdepth_get(خالی)
{
    واپس funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    خالی
funcdepth_restore(صحیح گہرائی)
{
    funcdepth = گہرائی;
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

    اگر (fc == تہی)
	واپس تہی;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    واپس fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    خالی
remove_funccal(خالی)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    ساکن funcerror_T
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
    typval_T	*tv_مقصد_free[MAX_FUNC_دلائل];
    صحیح		tv_مقصد_free_len = 0;
#اگر_تعریف_شدہ FEAT_PROFILE
    profinfo_T	profile_info;
#اگر_ختم
    ESTACK_CHECK_DECLARATION;

#اگر_تعریف_شدہ FEAT_PROFILE
    CLEAR_میدان(profile_info);
#اگر_ختم

    // If depth of calling is getting too high, don't execute the function.
    اگر (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_نمبر;
	rettv->vval.v_نمبر = -1;
	واپس FCERR_FAILED;
    }

    سطر_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    اگر (fc == تہی)
	واپس FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_ڈھونڈو_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, حجم(ufunc_T *), 1);

    اگر (fp->uf_def_حالت != UF_NOT_COMPILED)
    {
	اگر (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#اگر_تعریف_شدہ FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == تہی ? تہی : fc->fc_caller->fc_func;
#اگر_ختم
	// Execute the function, possibly compiling it first.
#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (do_profiling == PROF_YES)
	    profile_may_آغاز_func(&profile_info, fp, caller);
#اگر_ختم
	sticky_cmdmod_flags = 0;
	اگر (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	اگر (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != تہی && caller->uf_profiling)))
	    profile_may_انتہا_func(&profile_info, fp, caller);
#اگر_ختم
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	واپس retval;
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
    اگر (selfdict != تہی)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_کلید;
	STRCPY(name, "self");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_جوڑو(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "set self dictionary");
	v->di_tv.v_type = VAR_DICT;
	v->di_tv.v_تالا = 0;
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
	جوڑو_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_دلائل.ga_len
				    ? argcount - fp->uf_دلائل.ga_len : 0));
    fc->fc_l_avars.dv_تالا = VAR_FIXED;
    اگر ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_کلید;
	STRCPY(name, "000");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_جوڑو(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "function argument");
	v->di_tv.v_type = VAR_LIST;
	v->di_tv.v_تالا = VAR_FIXED;
	v->di_tv.vval.v_list = &fc->fc_l_varlist;
    }
    CLEAR_میدان(fc->fc_l_varlist);
    fc->fc_l_varlist.lv_refcount = DO_NOT_FREE_CNT;
    fc->fc_l_varlist.lv_تالا = VAR_FIXED;

    /*
     * Set a:firstline to "firstline" and a:lastline to "lastline".
     * Set a:name to named arguments.
     * Set a:N to the "..." arguments.
     * Skipped when no a: variables used (in lambda).
     */
    اگر ((fp->uf_flags & FC_NOARGS) == 0)
    {
	جوڑو_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	جوڑو_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    برائے (i = 0; i < argcount || i < fp->uf_دلائل.ga_len; ++i)
    {
	صحیح	    addlocal = FALSE;
	typval_T    def_rettv;
	صحیح	    isdefault = FALSE;

	ai = i - fp->uf_دلائل.ga_len;
	اگر (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    اگر (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_دلائل.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_نمبر == VVAL_NONE));
	    اگر (isdefault)
	    {
		char_u	    *default_اظہاریہ = تہی;

		def_rettv.v_type = VAR_نمبر;
		def_rettv.vval.v_نمبر = -1;

		default_اظہاریہ = ((char_u **)(fp->uf_def_دلائل.ga_ڈیٹا))
						 [ai + fp->uf_def_دلائل.ga_len];
		اگر (eval1(&default_اظہاریہ, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    توڑو;
		}
	    }

	    namelen = STRLEN(name);
	}
	ورنہ
	{
	    اگر ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		توڑو;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((حرف *)numbuf, حجم(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	اگر (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_کلید, name);
	}
	ورنہ
	{
	    v = dictitem_alloc(name);
	    اگر (v == تہی)
		توڑو;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_تالا = VAR_FIXED;

	اگر (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_مقصد_free[tv_مقصد_free_len++] = &v->di_tv;

	اگر (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_جوڑو(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	ورنہ
	    hash_جوڑو(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	اگر (ai >= 0 && ai < MAX_FUNC_دلائل)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_تالا = VAR_FIXED;
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

    estack_ڈالو_ufunc(fp, 1);
    ESTACK_CHECK_تیاری;
    اگر (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	اگر (p_verbose >= 14)
	{
	    char_u	buf[پیغام_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    پیغام_puts("(");
	    برائے (i = 0; i < argcount; ++i)
	    {
		اگر (i > 0)
		    پیغام_puts(", ");
		اگر (argvars[i].v_type == VAR_نمبر)
		    پیغام_outnum((طویل)argvars[i].vval.v_نمبر);
		ورنہ
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    اگر (s != تہی)
		    {
			اگر (vim_strsize(s) > پیغام_BUF_CLEN)
			{
			    trunc_string(s, buf, پیغام_BUF_CLEN, پیغام_BUF_LEN);
			    s = buf;
			}
			پیغام_puts((حرف *)s);
			vim_free(tofree);
		    }
		}
	    }
	    پیغام_puts(")");
	}
	پیغام_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	profile_may_آغاز_func(&profile_info, fp,
			fc->fc_caller == تہی ? تہی : fc->fc_caller->fc_func);
#اگر_ختم

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
    ورنہ اگر (islambda)
    {
	char_u *p = *(char_u **)fp->uf_سطریں.ga_ڈیٹا + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(خالی)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    ورنہ
	// call do_cmdline() to execute the lines
	do_cmdline(تہی, get_func_سطر, (خالی *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    اگر ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_نمبر;
	rettv->vval.v_نمبر = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	اگر (اندر_vim9script())
	    retval = FCERR_FAILED;
    }

#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == تہی ? تہی : fc->fc_caller->fc_func;

	اگر (fp->uf_profiling || (caller != تہی && caller->uf_profiling))
	    profile_may_انتہا_func(&profile_info, fp, caller);
    }
#اگر_ختم

    // when being verbose, mention the return value
    اگر (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	اگر (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	ورنہ اگر (fc->fc_rettv->v_type == VAR_نمبر)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (طویل)fc->fc_rettv->vval.v_نمبر);
	ورنہ
	{
	    char_u	buf[پیغام_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    اگر (s != تہی)
	    {
		اگر (vim_strsize(s) > پیغام_BUF_CLEN)
		{
		    trunc_string(s, buf, پیغام_BUF_CLEN, پیغام_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	پیغام_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_نکالو();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_آغاز);
#اگر_ختم
    اگر (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    اگر (p_verbose >= 12 && SOURCING_NAME != تہی)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	پیغام_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    برائے (i = 0; i < tv_مقصد_free_len; ++i)
	clear_tv(tv_مقصد_free[i]);
    cleanup_function_call(fc);

    واپس retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, صحیح argcount)
{
    صحیح regular_دلائل = fp->uf_دلائل.ga_len;

    اگر (argcount < regular_دلائل - fp->uf_def_دلائل.ga_len)
	واپس FCERR_TOOFEW;
    ورنہ اگر (!has_varargs(fp) && argcount > regular_دلائل)
	واپس FCERR_TOOMANY;
    واپس FCERR_UNKNOWN;
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

#اگر_تعریف_شدہ FEAT_LUA
    اگر (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	واپس (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#اگر_ختم

    اگر (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != تہی)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    اگر (error != FCERR_UNKNOWN)
	واپس error;

    اگر ((fp->uf_flags & FC_DICT) && selfdict == تہی)
    {
	error = FCERR_DICT;
    }
    ورنہ
    {
	صحیح		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_تلاش_patterns();
	اگر (!ins_compl_فعال())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : تہی);
	اگر (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	اگر (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_تلاش_patterns();
    }

    واپس error;
}

ساکن funccal_اندراج_T *funccal_ڈھیر = تہی;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    خالی
save_funccal(funccal_اندراج_T *اندراج_2)
{
    اندراج_2->چوٹی_funccal = current_funccal;
    اندراج_2->اگلا = funccal_ڈھیر;
    funccal_ڈھیر = اندراج_2;
    current_funccal = تہی;
}

    خالی
restore_funccal(خالی)
{
    اگر (funccal_ڈھیر == تہی)
	اندرونی_error("restore_funccal()");
    ورنہ
    {
	current_funccal = funccal_ڈھیر->چوٹی_funccal;
	funccal_ڈھیر = funccal_ڈھیر->اگلا;
    }
}

    funccall_T *
get_current_funccal(خالی)
{
    واپس current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    صحیح
at_script_level(خالی)
{
    واپس current_funccal == تہی && autocmd_مطابقت == تہی;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    خالی
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
    len = 3 + vim_snprintf((حرف *)buf + 3, حجم(buf) - 3, "%d_", sid);

    جب (todo > 0)
    {
	todo = func_hashtab.ht_مستعمل;
	FOR_ALL_HASHTAB_اشیاء(&func_hashtab, hi, todo)
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		fp = HI2UF(hi);
		اگر (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    صحیح بدلا = func_hashtab.ht_بدلا;

		    fp->uf_flags |= FC_DEAD;

		    اگر (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			اگر (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    ورنہ
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			اگر (بدلا != func_hashtab.ht_بدلا)
			    توڑو;
		    }
		}
		--todo;
	    }
    }
}

#اگر defined(EXITFREE)
    خالی
free_all_functions(خالی)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    صحیح		بدلا;

    // Clean up the current_funccal chain and the funccal stack.
    جب (current_funccal != تہی)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	اگر (current_funccal == تہی && funccal_ڈھیر != تہی)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    جب (todo > 0)
    {
	todo = func_hashtab.ht_مستعمل;
	FOR_ALL_HASHTAB_اشیاء(&func_hashtab, hi, todo)
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_حالت = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		اگر (func_name_refcount(fp->uf_name))
		    ++skipped;
		ورنہ
		{
		    بدلا = func_hashtab.ht_بدلا;
		    func_clear(fp, TRUE);
		    اگر (بدلا != func_hashtab.ht_بدلا)
		    {
			skipped = 0;
			توڑو;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    جب (func_hashtab.ht_مستعمل > skipped)
    {
	todo = func_hashtab.ht_مستعمل;
	FOR_ALL_HASHTAB_اشیاء(&func_hashtab, hi, todo)
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		اگر (func_name_refcount(fp->uf_name))
		    ++skipped;
		ورنہ
		{
		    اگر (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			توڑو;
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
#اگر_ختم

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
	واپس FALSE;
    برائے (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	اگر (name[i] == AUTOLOAD_CHAR)
	    واپس FALSE;
	اگر (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    اگر (name[i] == '.')
		واپس FALSE;
	    توڑو;
	}
    }
    واپس TRUE;
}

    صحیح
func_call(
    char_u	*name,
    typval_T	*دلائل,
    partial_T	*partial,
    dict_T	*selfdict,
    typval_T	*rettv)
{
    list_T	*l = دلائل->vval.v_list;
    listitem_T	*شے;
    typval_T	argv[MAX_FUNC_دلائل + 1];
    صحیح		argc = 0;
    صحیح		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اشیاء(l, شے)
    {
	اگر (argc == MAX_FUNC_دلائل - (partial == تہی ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    توڑو;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&شے->li_tv, &argv[argc++]);
    }

    اگر (شے == تہی)
    {
	funcexe_T	funcexe;
	صحیح		namelen = -1;

	اگر (اندر_vim9script())
	{
	    char_u *p = generic_func_ڈھونڈو_open_bracket(name);

	    اگر (p != تہی)
	    {
		اگر (skip_generic_func_type_دلائل(&p) == FAIL)
		    جاؤ مکمل;
		namelen = p - name + 1;
	    }
	}

	CLEAR_میدان(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	اگر (partial != تہی)
	{
	    funcexe.fe_object = partial->pt_obj;
	    اگر (funcexe.fe_object != تہی)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    جب (argc > 0)
	clear_tv(&argv[--argc]);

مکمل:
    واپس r;
}

ساکن صحیح callback_گہرائی = 0;

    صحیح
get_callback_گہرائی(خالی)
{
    واپس callback_گہرائی;
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

    اگر (callback->cb_name == تہی || *callback->cb_name == NUL)
	واپس FAIL;

    اگر (callback_گہرائی > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	واپس FAIL;
    }

    CLEAR_میدان(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    اگر (callback->cb_partial != تہی)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	اگر (funcexe.fe_object != تہی)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_گہرائی;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_گہرائی;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    اگر (need_rethrow && current_exception != تہی && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    واپس ret;
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
	واپس -2;

    retval = tv_get_نمبر_chk(&rettv, تہی);
    clear_tv(&rettv);
    واپس retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    خالی
user_func_error(funcerror_T error, char_u *name, صحیح ملا_var)
{
    انتخاب (error)
    {
	حالت FCERR_UNKNOWN:
		اگر (ملا_var)
		    emsg_funcname(e_not_callable_type_str, name);
		ورنہ
		    emsg_funcname(e_unknown_function_str, name);
		توڑو;
	حالت FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_طریقہ_str, name);
		توڑو;
	حالت FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		توڑو;
	حالت FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		توڑو;
	حالت FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		توڑو;
	حالت FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_اندر_script_context_str, name);
		توڑو;
	حالت FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		توڑو;
	حالت FCERR_OTHER:
	حالت FCERR_FAILED:
		// assume the error message was already given
		توڑو;
	حالت FCERR_NONE:
		توڑو;
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
    ساکن funcerror_T
may_check_دلیل_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	صحیح	    argcount,
	صحیح	    بنیاد_included,
	char_u	    *name)
{
    اگر (funcexe->fe_check_type != تہی && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	اگر (check_دلیل_types(funcexe->fe_check_type,
			  argvars, argcount,
			  بنیاد_included ? تہی : funcexe->fe_basetv,
			  name) == FAIL)
	    واپس FCERR_OTHER;
    }
    واپس FCERR_NONE;
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
    صحیح		argcount_اندر,	// number of "argvars"
    typval_T	*argvars_اندر,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    صحیح		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    صحیح		i;
    ufunc_T	*fp = تہی;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = تہی;
    char_u	*fname = تہی;
    char_u	*name = تہی;
    صحیح		argcount = argcount_اندر;
    typval_T	*argvars = argvars_اندر;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_دلائل + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    صحیح		argv_clear = 0;
    صحیح		argv_بنیاد = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_دلائل[MAX_FUNC_دلائل];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_دلائل_جدول_init(&gfatab);

    اگر (partial != تہی)
	fp = partial->pt_func;
    اگر (fp == تہی)
	fp = funcexe->fe_ufunc;

    اگر (fp == تہی)
    {
	اگر (اندر_vim9script())
	{
	    char_u *p = generic_func_ڈھونڈو_open_bracket(funcname);

	    اگر (p != تہی)
	    {
		len = p - funcname;
		اگر (parse_generic_func_type_دلائل(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == تہی)
		    جاؤ theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	اگر (name == تہی)
	    جاؤ theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    اگر (funcexe->fe_doesrange != تہی)
	*funcexe->fe_doesrange = FALSE;

    اگر (partial != تہی)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	اگر (partial->pt_dict != تہی && (selfdict == تہی || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	اگر (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    برائے (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		اگر (argv_clear + argcount_اندر >= MAX_FUNC_دلائل)
		{
		    error = FCERR_TOOMANY;
		    جاؤ theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    برائے (i = 0; i < argcount_اندر; ++i)
		argv[i + argv_clear] = argvars_اندر[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_اندر;

	    اگر (funcexe->fe_check_type != تہی
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_دلائل = check_type_دلائل;
		CLEAR_میدان(check_type_دلائل);
		برائے (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_دلائل[i + partial->pt_argc] =
							 check_type.tt_دلائل[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    اگر (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_دلیل_types(funcexe, argvars, argcount, FALSE,
					     (name != تہی) ? name : funcname);

    اگر (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	صحیح	is_global = FALSE;

	// Skip "g:" before a function name.
	اگر (fp == تہی && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_نمبر;	// default rettv is number zero
	rettv->vval.v_نمبر = 0;
	error = FCERR_UNKNOWN;

	اگر (fp != تہی || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    اگر (fp == تہی)
	    {
		fp = ڈھونڈو_func(rfname, is_global);
		اگر (fp != تہی && !is_global && اندر_vim9script()
						 && func_requires_g_سابقہ(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = تہی;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    اگر (fp == تہی
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, تہی)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = ڈھونڈو_func(rfname, is_global);
	    }
	    // Try loading a package.
	    اگر (fp == تہی && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = ڈھونڈو_func(rfname, is_global);
	    }
	    اگر (fp == تہی)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		اگر (p != تہی && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = ڈھونڈو_func(p, is_global);
	    }

	    اگر (partial == تہی && fp != تہی && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		اگر (fp == تہی)
		{
		    error = FCERR_FAILED;
		    جاؤ theend;
		}
	    }
	    ورنہ اگر (generic_func_دلائل_جدول_سائز(&gfatab) > 0)
	    {
		emsg_funcname(fp != تہی ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		جاؤ theend;
	    }

	    اگر (fp != تہی && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    ورنہ اگر (fp != تہی)
	    {
		صحیح need_arg_check = FALSE;
		اگر (funcexe->fe_check_type == تہی)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		اگر (funcexe->fe_argv_func != تہی)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		اگر (funcexe->fe_basetv != تہی)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, حجم(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_بنیاد = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		اگر (need_arg_check)
		    error = may_check_دلیل_types(funcexe, argvars, argcount,
				       TRUE, (name != تہی) ? name : funcname);

		اگر (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	ورنہ اگر (funcexe->fe_basetv != تہی)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_اندرونی_طریقہ(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	ورنہ
	{
	    /*
	     * Find the function name in the table, call its implementation.
	     */
	    error = call_اندرونی_func(fname, argcount, argvars, rettv);
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
	user_func_error(error, (name != تہی) ? name : funcname,
							funcexe->fe_ملا_var);

    // clear the copies made from the partial
    جب (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_بنیاد]);

    vim_free(tofree);
    vim_free(name);
    generic_func_دلائل_جدول_clear(&gfatab);

    واپس ret;
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
    char_u	*tofree = تہی;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    صحیح		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_نمبر;	// default rettv is number zero
    rettv->vval.v_نمبر = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    اگر (name == تہی)
	واپس ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    اگر (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    ورنہ
	rfname = fname;
    fp = ڈھونڈو_func(rfname, is_global);
    اگر (fp != تہی && !is_global && اندر_vim9script()
						 && func_requires_g_سابقہ(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = تہی;
    اگر (fp == تہی)
	ret = NOTDONE;
    ورنہ اگر (fp != تہی && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    ورنہ اگر (fp != تہی)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_میدان(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, تہی);
	اگر (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    واپس ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    واپس fp->uf_name_exp != تہی ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    ساکن صحیح
function_list_modified(صحیح پچھلا_ht_بدلا)
{
    اگر (پچھلا_ht_بدلا != func_hashtab.ht_بدلا)
    {
	emsg(_(e_function_list_was_modified));
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    ساکن صحیح
list_func_سر(ufunc_T *fp, صحیح indent)
{
    صحیح		پچھلا_ht_بدلا = func_hashtab.ht_بدلا;
    صحیح		j;

    پیغام_آغاز();

    // a timer at the more prompt may have deleted the function
    اگر (function_list_modified(پچھلا_ht_بدلا))
	واپس FAIL;

    اگر (indent)
	پیغام_puts("   ");
    اگر (fp->uf_def_حالت != UF_NOT_COMPILED)
	پیغام_puts("def ");
    ورنہ
	پیغام_puts("function ");
    پیغام_puts((حرف *)printable_func_name(fp));
    پیغام_putchar('(');
    برائے (j = 0; j < fp->uf_دلائل.ga_len; ++j)
    {
	اگر (j)
	    پیغام_puts(", ");
	پیغام_puts((حرف *)FUNCARG(fp, j));
	اگر (fp->uf_arg_types != تہی)
	{
	    حرف *tofree;

	    پیغام_puts(": ");
	    پیغام_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	اگر (j >= fp->uf_دلائل.ga_len - fp->uf_def_دلائل.ga_len)
	{
	    پیغام_puts(" = ");
	    پیغام_puts(((حرف **)(fp->uf_def_دلائل.ga_ڈیٹا))
		       [j - fp->uf_دلائل.ga_len + fp->uf_def_دلائل.ga_len]);
	}
    }
    اگر (fp->uf_varargs)
    {
	اگر (j)
	    پیغام_puts(", ");
	پیغام_puts("...");
    }
    اگر (fp->uf_va_name != تہی)
    {
	اگر (!fp->uf_varargs)
	{
	    اگر (j)
		پیغام_puts(", ");
	    پیغام_puts("...");
	}
	پیغام_puts((حرف *)fp->uf_va_name);
	اگر (fp->uf_va_type != تہی)
	{
	    حرف *tofree;

	    پیغام_puts(": ");
	    پیغام_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    پیغام_putchar(')');

    اگر (fp->uf_def_حالت != UF_NOT_COMPILED)
    {
	اگر (fp->uf_ret_type != &t_void)
	{
	    حرف *tofree;

	    پیغام_puts(": ");
	    پیغام_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    ورنہ اگر (fp->uf_flags & FC_ABORT)
	پیغام_puts(" abort");
    اگر (fp->uf_flags & FC_RANGE)
	پیغام_puts(" range");
    اگر (fp->uf_flags & FC_DICT)
	پیغام_puts(" dict");
    اگر (fp->uf_flags & FC_احاطہ)
	پیغام_puts(" closure");
    پیغام_clr_eos();
    اگر (p_verbose > 0)
	آخری_سیٹ_پیغام(fp->uf_script_ctx);

    واپس OK;
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
    واپس trans_function_name_ext(pp, is_global, skip, flags,
	    تہی, تہی, تہی, تہی);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    ساکن char_u *
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
    char_u	*name = تہی;
    char_u	*آغاز;
    char_u	*انتہا;
    صحیح		lead;
    char_u	sid_buf[20];
    صحیح		len;
    صحیح		extra = 0;
    صحیح		سابقہ_g = FALSE;
    lval_T	lv;
    صحیح		vim9script = اندر_vim9script();
    صحیح		vim9_local;

    اگر (fdp != تہی)
	CLEAR_اشارہ_گر(fdp);
    آغاز = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    اگر ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (صحیح)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	واپس vim_strnsave(آغاز, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(آغاز);
    اگر (lead > 2)
	آغاز += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    انتہا = get_lval(آغاز, تہی, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_آغاز);
    اگر (انتہا == آغاز || (vim9script && انتہا != تہی
				   && انتہا[-1] == AUTOLOAD_CHAR && *انتہا == '('))
    {
	اگر (!skip)
	    emsg(_(e_function_name_required));
	جاؤ theend;
    }
    اگر (انتہا == تہی || (lv.ll_tv != تہی && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	اگر (!aborting())
	{
	    اگر (انتہا != تہی)
		semsg(_(e_invalid_دلیل_str), آغاز);
	}
	ورنہ
	    *pp = ڈھونڈو_name_انتہا(آغاز, تہی, تہی, FNE_INCL_BR);
	جاؤ theend;
    }

    اگر (lv.ll_ufunc != تہی)
    {
	اگر (ufunc != تہی)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = انتہا;
	جاؤ theend;
    }

    اگر (lv.ll_tv != تہی)
    {
	اگر (fdp != تہی)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = تہی;
	    fdp->fd_di = lv.ll_di;
	}
	اگر (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != تہی)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = انتہا;
	}
	ورنہ اگر (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != تہی)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.لمبائی);
	    *pp = انتہا;
	}
	ورنہ اگر (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != تہی)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = انتہا;
	    اگر (partial != تہی)
		*partial = lv.ll_tv->vval.v_partial;
	}
	ورنہ
	{
	    اگر (!skip && !(flags & TFN_QUIET) && (fdp == تہی
			     || lv.ll_dict == تہی || fdp->fd_newkey == تہی))
		emsg(_(e_funcref_required));
	    ورنہ
		*pp = انتہا;
	    name = تہی;
	}
	جاؤ theend;
    }

    اگر (lv.ll_name == تہی)
    {
	// Error found, but continue after the function name.
	*pp = انتہا;
	جاؤ theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    اگر (lv.ll_exp_name != تہی)
    {
	len = (صحیح)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, تہی);
	اگر (name == lv.ll_exp_name)
	    name = تہی;
    }
    ورنہ اگر (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_شے(lv.ll_sid);
	صحیح		cc = *lv.ll_name_انتہا;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_انتہا = NUL;
	اگر (si->sn_autoload_سابقہ != تہی)
	{
	    name = concat_str(si->sn_autoload_سابقہ, lv.ll_name);
	}
	ورنہ
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (صحیح)KE_SNR;
	    vim_snprintf((حرف *)sid_buf + 3, حجم(sid_buf) - 3,
						      "%ld_", (طویل)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_انتہا = cc;
	*pp = انتہا;
	جاؤ theend;
    }
    ورنہ اگر (!(flags & TFN_NO_DEREF))
    {
	len = (صحیح)(انتہا - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, تہی);
	اگر (name == *pp)
	    name = تہی;
    }
    اگر (name != تہی)
    {
	name = vim_strsave(name);
	*pp = انتہا;
	اگر (name != تہی && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (صحیح)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	جاؤ theend;
    }

    اگر (lv.ll_exp_name != تہی)
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
    ورنہ
    {
	// skip over "s:" and "g:"
	اگر (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    اگر (lv.ll_name[0] == 'g')
	    {
		اگر (is_global != تہی)
		{
		    *is_global = TRUE;
		}
		ورنہ
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    سابقہ_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (صحیح)(انتہا - lv.ll_name);
    }
    اگر (len <= 0)
    {
	اگر (!skip)
	    emsg(_(e_function_name_required));
	جاؤ theend;
    }

    // In Vim9 script a user function is script-local by default, unless it
    // starts with a lower case character: dict.func().  Or when in a class.
    vim9_local = ASCII_ISUPPER(*آغاز) && vim9script
						&& (flags & TFN_اندر_CLASS) == 0;

    /*
     * Copy the function name to allocated memory.
     * Accept <SID>name() inside a script, translate into <SNR>123_name().
     * Accept <SNR>123_name() outside a script.
     */
    اگر (skip)
	lead = 0;	// do nothing
    ورنہ اگر (lead > 0 || vim9_local)
    {
	اگر (!vim9_local)
	{
	    اگر (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_آغاز_with_capital_str), آغاز);
		جاؤ theend;
	    }
	    lead = 3;
	}
	اگر (vim9_local || (lv.ll_exp_name != تہی
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    اگر (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_اندر_script_context));
		جاؤ theend;
	    }
	    sid_buflen = vim_snprintf((حرف *)sid_buf, حجم(sid_buf), "%ld_", (طویل)current_sctx.sc_sid);
	    اگر (vim9_local)
		extra = 3 + (صحیح)sid_buflen;
	    ورنہ
		lead += (صحیح)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    ورنہ اگر (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_اندر_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_آغاز_with_capital_str
			   : e_function_name_must_آغاز_with_capital_or_s_str),
									آغاز);
	جاؤ theend;
    }
    اگر (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	اگر (cp != تہی && cp < انتہا)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), آغاز);
	    جاؤ theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    اگر (name != تہی)
    {
	اگر (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (صحیح)KE_SNR;
	    اگر (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	ورنہ اگر (سابقہ_g)
	{
	    name[0] = 'g';
	    name[1] = ':';
	}
	mch_memmove(name + lead + extra, lv.ll_name, (size_t)len);
	name[lead + extra + len] = NUL;
    }
    *pp = انتہا;

theend:
    clear_lval(&lv);
    واپس name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    ساکن char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    اگر (*name == K_SPECIAL && اندر_vim9script())
    {
	p = vim_strchr(name, '_');
	اگر (p != تہی)
	    واپس p + 1;
    }
    واپس تہی;
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
    صحیح		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    اگر (funcname == تہی)
	واپس تہی;

    اگر (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	اگر (!اندر_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    واپس تہی;
	ufunc = ڈھونڈو_func(funcname, FALSE);
	اگر (ufunc == تہی || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == تہی)
	    واپس تہی;
	++p;
	off = 0;
    }
    ورنہ
	off = *funcname == 's' ? 2 : 5;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_اندر_script_context));
	واپس تہی;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, حجم(sid_buf), "<SNR>%ld_",
	    (طویل)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    اگر (newname == تہی)
	واپس تہی;
    vim_snprintf((حرف *)newname, newnamesize, "%s%s", sid_buf, p + off);

    واپس newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    اگر (n != تہی)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    واپس n;
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
	(خالی)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	اگر (fudi != تہی)
	    CLEAR_اشارہ_گر(fudi);
    }
    ورنہ
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, تہی, تہی, تہی);
    *name = p;
    واپس saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    خالی
list_functions(regmatch_T *regmatch)
{
    صحیح		پچھلا_ht_بدلا = func_hashtab.ht_بدلا;
    long_u	todo = func_hashtab.ht_مستعمل;
    hashitem_T	*hi;

    برائے (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    اگر ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == تہی
			? !پیغام_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !محفوظ_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		اگر (list_func_سر(fp, FALSE) == FAIL)
		    واپس;
		اگر (function_list_modified(پچھلا_ht_بدلا))
		    واپس;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    ساکن char_u *
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
	اگر (regmatch.regprog != تہی)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    اگر (*p == '/')
	++p;

    واپس p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    ساکن ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, صحیح is_global)
{
    ufunc_T	*fp = تہی;
    صحیح		j;

    اگر (!سرے_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	واپس تہی;
    }

    سیٹ_nextcmd(eap, p);

    اگر (eap->nextcmd != تہی)
	*p = NUL;

    اگر (eap->skip || got_int)
	واپس تہی;

    fp = ڈھونڈو_func(name, is_global);
    اگر (fp == تہی && ASCII_ISUPPER(*eap->arg))
    {
	char_u *اوپر = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	اگر (اوپر != تہی)
	    fp = ڈھونڈو_func(اوپر, FALSE);
    }

    اگر (fp == تہی)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	واپس تہی;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    صحیح پچھلا_ht_بدلا = func_hashtab.ht_بدلا;

    اگر (list_func_سر(fp, TRUE) != OK)
	واپس fp;

    برائے (j = 0; j < fp->uf_سطریں.ga_len && !got_int; ++j)
    {
	اگر (FUNCLINE(fp, j) == تہی)
	    جاری;
	پیغام_putchar('\n');
	پیغام_outnum((طویل)(j + 1));
	اگر (j < 9)
	    پیغام_putchar(' ');
	اگر (j < 99)
	    پیغام_putchar(' ');
	اگر (function_list_modified(پچھلا_ht_بدلا))
	    توڑو;
	پیغام_prt_سطر(FUNCLINE(fp, j), FALSE);
	برآمد_flush();	// show a line at a time
	ui_breakcheck();
    }

    اگر (!got_int)
    {
	پیغام_putchar('\n');
	اگر (!function_list_modified(پچھلا_ht_بدلا))
	{
	    اگر (fp->uf_def_حالت != UF_NOT_COMPILED)
		پیغام_puts("   enddef");
	    ورنہ
		پیغام_puts("   endfunction");
	}
    }

    واپس fp;
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
	garray_T    *سطریں_مقصد_free,
	صحیح	    class_flags,
	ocmember_T  *obj_members,
	صحیح         obj_member_گنتی,
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
    char_u	*سطر_arg = تہی;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_دلائل;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    صحیح		varargs = FALSE;
    صحیح		flags = 0;
    char_u	*ret_type = تہی;
    ufunc_T	*fp = تہی;
    صحیح		fp_allocated = FALSE;
    صحیح		free_fp = FALSE;
    صحیح		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    ساکن صحیح	func_nr = 0;	    // number for nameless function
    صحیح		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_چوٹی;
    صحیح		vim9script = اندر_vim9script();
    imported_T	*import = تہی;

    // ":function" without argument: list functions.
    اگر (سرے_excmd2(eap->cmd, eap->arg))
    {
	اگر (!eap->skip)
	    list_functions(تہی);
	سیٹ_nextcmd(eap, eap->arg);
	واپس تہی;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    اگر (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	سیٹ_nextcmd(eap, p);
	واپس تہی;
    }

    ga_init(&newargs);
    ga_init(&argtypes);
    ga_init(&arg_objm);
    ga_init(&default_دلائل);
    generic_func_دلائل_جدول_init(&gfatab);

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
    اگر (name_arg != تہی)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_میدان(fudi);
    }
    ورنہ
    {
	اگر (vim9script)
	{
	    اگر (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_اندر_vim9_script_str), p);
		واپس تہی;
	    }
	    p = مقصد_name_انتہا(p, TRUE);
	    اگر (*skipwhite(p) == '.' && vim_strchr(p, '(') != تہی)
	    {
		semsg(_(e_cannot_define_dict_func_اندر_vim9_script_str),
								     eap->arg);
		واپس تہی;
	    }

	    p = eap->arg;
	}

	صحیح tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_اندر_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != تہی);
	اگر (name == تہی && (fudi.fd_dict == تہی || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    اگر (!aborting())
	    {
		اگر (!eap->skip && fudi.fd_newkey != تہی)
		    semsg(_(e_کلید_not_present_اندر_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		واپس تہی;
	    }
	    ورنہ
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	اگر (is_export && name != تہی)
	{
	    char_u *prefixed = may_سابقہ_autoload(name);

	    اگر (prefixed != تہی && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	ورنہ اگر (paren && vim9script && name != تہی
				    && vim_strchr(name, AUTOLOAD_CHAR) != تہی)
	{
	    emsg(_(e_cannot_use_name_with_hash_اندر_vim9_script_use_export_instead));
	    جاؤ ret_free;
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
	جاؤ ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    اگر (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	اگر (p == تہی)
	    جاؤ ret_free;
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
	    جاؤ ret_free;
	}
	// attempt to continue by skipping some text
	اگر (vim_strchr(p, '(') != تہی)
	    p = vim_strchr(p, '(');
    }

    اگر ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	جاؤ ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    اگر (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	جاؤ ret_free;
    }

    ga_init2(&newlines, حجم(char_u *), 10);

    اگر (!eap->skip && name_arg == تہی)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	اگر (name != تہی)
	    arg = name;
	ورنہ
	    arg = fudi.fd_newkey;
	اگر (arg != تہی && (fudi.fd_di == تہی
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_بنیاد = arg;
	    صحیح	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    اگر (arg != fudi.fd_newkey)
	    {
		اگر (*arg == K_SPECIAL)
		{
		    name_بنیاد = vim_strchr(arg, '_');
		    اگر (name_بنیاد == تہی)
			name_بنیاد = arg + 3;
		    ورنہ
			++name_بنیاد;
		}
		برائے (i = 0; name_بنیاد[i] != NUL && (i == 0
					    ? eval_isnamec1(name_بنیاد[i])
					    : eval_isnamec(name_بنیاد[i])); ++i)
		    ;
		اگر (name_بنیاد[i] != NUL)
		{
		    emsg_funcname(e_invalid_دلیل_str, arg);
		    جاؤ ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		اگر (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_بنیاد, i, 0, تہی,
			تہی, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_شے_str), name_بنیاد);
		    جاؤ ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	اگر (fudi.fd_dict != تہی && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_یہاں));
	    جاؤ ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    اگر (get_function_دلائل(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : تہی, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : تہی,
			 تہی, &varargs, &default_دلائل, eap->skip,
			 eap, class_flags, &newlines, سطریں_مقصد_free) == FAIL)
	جاؤ errret_2;
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
	    ورنہ اگر (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    اگر (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    ورنہ
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = تہی;
	    }
	}
	p = skipwhite(p);
    }
    ورنہ
	// find extra arguments "range", "dict", "abort" and "closure"
	برائے (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    اگر (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    ورنہ اگر (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    ورنہ اگر (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    ورنہ اگر (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_احاطہ;
		p += 7;
		اگر (current_funccal == تہی)
		{
		    emsg_funcname(e_احاطہ_function_should_not_be_at_چوٹی_level_str,
			    name == تہی ? (char_u *)"" : name);
		    جاؤ erret;
		}
	    }
	    ورنہ
		توڑو;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    اگر (*p == '\n')
	سطر_arg = p + 1;
    ورنہ اگر (*p != NUL
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
	    اگر (fudi.fd_dict != تہی && fudi.fd_newkey == تہی)
		emsg(_(e_dictionary_اندراج_already_exists));
	    ورنہ اگر (name != تہی && ڈھونڈو_func(name, is_global) != تہی)
		emsg_funcname(e_function_str_already_exists_جوڑو_bang_مقصد_replace, name);
	}

	اگر (!eap->skip && did_emsg)
	    جاؤ erret;

	پیغام_putchar('\n');	    // don't overwrite the function name
	cmdline_صف = پیغام_صف;
    }

    // Save the starting line number.
    sourcing_lnum_چوٹی = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    اگر (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_طریقہ) == 0
		&& (get_function_body(eap, &newlines, سطر_arg, سطریں_مقصد_free)
							== FAIL || did_emsg))
	    || eap->skip)
	جاؤ erret;

    /*
     * If there are no errors, add the function
     */
    اگر (fudi.fd_dict != تہی)
    {
	حرف	numbuf[NUMBUFLEN];

	fp = تہی;
	اگر (fudi.fd_newkey == تہی && !eap->forceit)
	{
	    emsg(_(e_dictionary_اندراج_already_exists));
	    جاؤ erret;
	}
	اگر (fudi.fd_di == تہی)
	{
	    // Can't add a function to a locked dictionary
	    اگر (قدر_check_تالا(fudi.fd_dict->dv_تالا, eap->arg, FALSE))
		جاؤ erret;
	}
	    // Can't change an existing function if it is locked
	ورنہ اگر (قدر_check_تالا(fudi.fd_di->di_tv.v_تالا, eap->arg, FALSE))
	    جاؤ erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, حجم(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	اگر (name == تہی)
	    جاؤ erret;
    }
    ورنہ اگر (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*ڈھونڈو_name = name;
	صحیح		var_conflict = FALSE;
	صحیح		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = ڈھونڈو_var(name, &ht, TRUE);
	اگر (v != تہی && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	اگر (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_شے(current_sctx.sc_sid);

	    اگر (si->sn_autoload_سابقہ != تہی)
	    {
		اگر (is_export)
		{
		    ڈھونڈو_name = name + STRLEN(si->sn_autoload_سابقہ);
		    v = ڈھونڈو_var(ڈھونڈو_name, &ht, TRUE);
		    اگر (v != تہی)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		ورنہ
		{
		    char_u *prefixed = may_سابقہ_autoload(name);

		    اگر (prefixed != تہی && prefixed != name)
		    {
			v = ڈھونڈو_var(prefixed, &ht, TRUE);
			اگر (v != تہی)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	اگر (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    جاؤ erret;
	}

	fp = ڈھونڈو_func_even_dead(ڈھونڈو_name, ffed_flags);
	اگر (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = ڈھونڈو_imported(uname == تہی ? name : uname, 0, FALSE);
	}

	اگر (fp != تہی || import != تہی)
	{
	    صحیح dead = fp != تہی && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    اگر (import != تہی
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_چوٹی;
		اگر (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		ورنہ
		    emsg_funcname(e_function_str_already_exists_جوڑو_bang_مقصد_replace, name);
		جاؤ errret_keep;
	    }
	    اگر (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_اندر_use, name);
		جاؤ errret_keep;
	    }
	    اگر (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = تہی;
		overwrite = TRUE;
	    }
	    ورنہ
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = تہی;
		func_clear_اشیاء(fp);
		fp->uf_name_exp = exp_name;
		fp->uf_flags &= ~FC_DEAD;
#اگر_تعریف_شدہ FEAT_PROFILE
		fp->uf_profiling = FALSE;
		fp->uf_prof_initialized = FALSE;
#اگر_ختم
		fp->uf_def_حالت = UF_NOT_COMPILED;
	    }
	}
    }

    اگر (fp == تہی)
    {
	اگر (fudi.fd_dict == تہی && vim_strchr(name, AUTOLOAD_CHAR) != تہی)
	{
	    صحیح	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    اگر (SOURCING_NAME != تہی)
	    {
		scriptname = autoload_name(name);
		اگر (scriptname != تہی)
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

		SOURCING_LNUM = sourcing_lnum_چوٹی;
		semsg(_(e_function_name_does_not_مطابقت_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		جاؤ erret;
	    }
	}

	اگر (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	اگر (fp == تہی)
	    جاؤ erret;
	fp_allocated = TRUE;

	اگر (fudi.fd_dict != تہی)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    اگر (func_name == تہی)
	    {
		VIM_CLEAR(fp);
		جاؤ erret;
	    }
	    اگر (fudi.fd_di == تہی)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		اگر (fudi.fd_di == تہی)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    جاؤ erret;
		}
		اگر (dict_جوڑو(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    جاؤ erret;
		}
	    }
	    ورنہ
		// overwrite existing dict entry
		clear_tv(&fudi.fd_di->di_tv);
	    fudi.fd_di->di_tv.v_type = VAR_FUNC;
	    fudi.fd_di->di_tv.vval.v_string = func_name;

	    // behave like "dict" was used
	    flags |= FC_DICT;
	}
    }
    fp->uf_دلائل = newargs;
    fp->uf_def_دلائل = default_دلائل;
    fp->uf_ret_type = &t_any;
    fp->uf_func_type = &t_func_any;

    اگر (eap->cmdidx == CMD_def)
    {
	صحیح	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_حالت = UF_مقصد_BE_COMPILED;

	اگر (generic_func_دلائل_جدول_سائز(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_چوٹی;

	// The function may use script variables from the context.
	function_using_بلاک_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	صحیح save_is_export = is_export;
	is_export = FALSE;

	اگر (parse_دلیل_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_گنتی, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    جاؤ erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	اگر (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    جاؤ erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    ورنہ
	fp->uf_def_حالت = UF_NOT_COMPILED;

    اگر (fp_allocated)
    {
	// insert the new function in the function list
	اگر (overwrite)
	{
	    hi = hash_ڈھونڈو(&func_hashtab, name);
	    hi->hi_کلید = UF2HIKEY(fp);
	}
	ورنہ اگر (class_flags == 0 && hash_جوڑو(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    جاؤ erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_سطریں = newlines;
    newlines.ga_ڈیٹا = تہی;
    اگر ((flags & FC_احاطہ) != 0)
    {
	اگر (رجسٹر_احاطہ(fp) == FAIL)
	    جاؤ erret;
    }
    ورنہ
	fp->uf_scoped = تہی;

#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (prof_def_func())
	func_do_profile(fp);
#اگر_ختم
    fp->uf_varargs = varargs;
    اگر (sandbox)
	flags |= FC_SANDBOX;
    اگر (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_نسخہ = current_sctx.sc_نسخہ;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_چوٹی;
    اگر (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    اگر (eap->cmdidx == CMD_def)
	سیٹ_function_type(fp);
    ورنہ اگر (fp->uf_script_ctx.sc_نسخہ == SCRIPT_نسخہ_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_نسخہ = SCRIPT_نسخہ_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    اگر (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, تہی);

    جاؤ ret_free;

erret:
    اگر (fp != تہی)
    {
	// these were set to "newargs" and "default_args", which are cleared
	//نیچے
	ga_init(&fp->uf_دلائل);
	ga_init(&fp->uf_def_دلائل);
    }
errret_2:
    اگر (fp != تہی)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	اگر (IS_GENERIC_FUNC(fp))
	    generic_func_clear_اشیاء(fp);
    }
    اگر (free_fp)
	VIM_CLEAR(fp);
errret_keep:
    ga_clear_لڑیاں(&newargs);
    ga_clear_لڑیاں(&default_دلائل);
    ga_clear_لڑیاں(&newlines);
ret_free:
    ga_clear_لڑیاں(&argtypes);
    ga_clear(&arg_objm);
    // The generic types are still in use and should not be freed.  Instead
    // clear the grow array.
    ga_clear(&gfatab.gfat_param_types);
    generic_func_دلائل_جدول_clear(&gfatab);
    vim_free(fudi.fd_newkey);
    اگر (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    واپس fp;
}

/*
 * ":function"
 */
    خالی
ex_function(exarg_T *eap)
{
    garray_T سطریں_مقصد_free;

    ga_init2(&سطریں_مقصد_free, حجم(char_u *), 50);
    (خالی)define_function(eap, تہی, &سطریں_مقصد_free, 0, تہی, 0, تہی);
    ga_clear_لڑیاں(&سطریں_مقصد_free);
}

    صحیح
get_func_arity(char_u *name, صحیح *required, صحیح *optional, صحیح *varargs)
{
    ufunc_T	*ufunc = تہی;
    صحیح		argcount = 0;
    صحیح		min_argcount = 0;
    صحیح		idx;

    idx = ڈھونڈو_اندرونی_func(name);
    اگر (idx >= 0)
    {
	اندرونی_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    ورنہ
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = تہی;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	اگر (error == FCERR_NONE)
	    ufunc = ڈھونڈو_func(fname, FALSE);
	vim_free(tofree);

	اگر (ufunc == تہی)
	    واپس FAIL;

	argcount = ufunc->uf_دلائل.ga_len;
	min_argcount = ufunc->uf_دلائل.ga_len - ufunc->uf_def_دلائل.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    واپس OK;
}

/*
 * Find a function by name, including "<lambda>123".
 * Check for "profile" and "debug" arguments and set"compile_type".
 * Caller should initialize "compile_type" to CT_NONE.
 * Return NULL if not found.
 */
    ufunc_T *
ڈھونڈو_func_by_name(char_u *name, compiletype_T *compile_type)
{
    char_u	*arg = name;
    char_u	*fname;
    ufunc_T	*ufunc;
    صحیح		is_global = FALSE;
    char_u	*bracket_آغاز = تہی;
    char_u	*generic_func_name = name;

    اگر (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    ورنہ اگر (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    اگر (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(خالی)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    ورنہ
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = ڈھونڈو_class_func(&arg);
	اگر (ufunc != تہی)
	    واپس ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      تہی, تہی, تہی, &ufunc);
	اگر (ufunc != تہی)
	{
	    vim_free(fname);
	    واپس ufunc;
	}
	اگر (fname != تہی && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_آغاز = arg;
	    اگر (skip_generic_func_type_دلائل(&arg) == FAIL)
	    {
		vim_free(fname);
		واپس تہی;
	    }
	}
    }
    اگر (fname == تہی)
    {
	semsg(_(e_invalid_دلیل_str), name);
	واپس تہی;
    }
    اگر (!سرے_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	واپس تہی;
    }

    ufunc = ڈھونڈو_func(fname, is_global);
    اگر (ufunc == تہی)
    {
	char_u *p = untrans_function_name(fname);

	اگر (p != تہی)
	    // Try again without making it script-local.
	    ufunc = ڈھونڈو_func(p, FALSE);

	اگر (ufunc == تہی)
	{
	    bracket_آغاز = generic_func_ڈھونڈو_open_bracket(fname);
	    اگر (bracket_آغاز != تہی)
	    {
		generic_func_name = fname;
		*bracket_آغاز = NUL;
		ufunc = ڈھونڈو_func(fname, FALSE);
		*bracket_آغاز = '<';
	    }
	}
    }

    اگر (ufunc != تہی)
    {
	// handle generic functions
	اگر (bracket_آغاز == تہی)
	    bracket_آغاز = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_آغاز);
    }

    vim_free(fname);

    اگر (ufunc == تہی)
	semsg(_(e_cannot_ڈھونڈو_function_str), name);
    واپس ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    خالی
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    اگر (func_needs_compiling(ufunc, compile_type))
	(خالی)compile_def_function(ufunc, FALSE, compile_type, تہی);
    ورنہ
	smsg(_("Function %s%s%s does not need compiling"),
				cl != تہی ? cl->class_name.string : (char_u *)"",
				cl != تہی ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    ساکن خالی
defcompile_funcs_اندر_script(خالی)
{
    طویل	todo = (طویل)func_hashtab.ht_مستعمل;
    صحیح		بدلا = func_hashtab.ht_بدلا;
    hashitem_T	*hi;

    برائے (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    اگر (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_حالت == UF_مقصد_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(خالی)compile_def_function(ufunc, FALSE, CT_NONE, تہی);

		اگر (func_hashtab.ht_بدلا != بدلا)
		{
		    // a function has been added or removed, need to start
		    // over
		    todo = (طویل)func_hashtab.ht_مستعمل;
		    بدلا = func_hashtab.ht_بدلا;
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
    خالی
ex_defcompile(exarg_T *eap)
{
    اگر (*eap->arg != NUL)
    {
	typval_T tv;

	اگر (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    اگر (cl != تہی)
		defcompile_class(cl);
	}
	ورنہ
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = ڈھونڈو_func_by_name(eap->arg, &compile_type);
	    اگر (ufunc != تہی)
		defcompile_function(ufunc, تہی);
	}
    }
    ورنہ
    {
	defcompile_funcs_اندر_script();

	// compile all the class defined in the current script
	defcompile_classes_اندر_script();
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
	واپس 5;
    اگر (p[0] == 's' && p[1] == ':')
	واپس 2;
    واپس 0;
}

    صحیح
translated_function_exists(char_u *name, صحیح is_global)
{
    اگر (builtin_function(name, -1))
	واپس has_اندرونی_func(name);
    واپس ڈھونڈو_func(name, is_global) != تہی;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    صحیح
has_varargs(ufunc_T *ufunc)
{
    واپس ufunc->uf_varargs || ufunc->uf_va_name != تہی;
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
    اگر (p != تہی && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    واپس n;
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
    ufunc_T	*fp = تہی;
    list_T	*دلائل;
    صحیح		is_def;
    حرف	*tofree;

    اگر (*name == NUL)
	واپس FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    اگر (p != تہی && *skipwhite(nm) == NUL)
	fp = ڈھونڈو_func(p, is_global);
    vim_free(p);
    اگر (fp == تہی)
	واپس FAIL;

    is_def = fp->uf_def_حالت != UF_NOT_COMPILED;
    dict_جوڑو_string(d, "name", printable_func_name(fp));
    dict_جوڑو_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_جوڑو_bool(d, "available", TRUE);
    دلائل = list_alloc();
    اگر (دلائل == تہی || dict_جوڑو_list(d, "args", دلائل) == FAIL)
    {
	list_unref(دلائل);
	واپس FAIL;
    }
    برائے (صحیح j = 0; j < fp->uf_دلائل.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	اگر (arg == تہی || list_append_dict(دلائل, arg) == FAIL)
	{
	    dict_unref(arg);
	    واپس FAIL;
	}
	dict_جوڑو_string(arg, "name", FUNCARG(fp, j));
	اگر (fp->uf_arg_types == تہی)
	    dict_جوڑو_string(arg, "type", (char_u *)"any");
	ورنہ
	{
	    dict_جوڑو_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	اگر (j >= fp->uf_دلائل.ga_len - fp->uf_def_دلائل.ga_len)
	    dict_جوڑو_string(arg, "default", ((char_u **)fp->uf_def_دلائل.ga_ڈیٹا)
			     [j - fp->uf_دلائل.ga_len + fp->uf_def_دلائل.ga_len]);
    }
    اگر (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	اگر (va == تہی || dict_جوڑو_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    واپس FAIL;
	}
	dict_جوڑو_string(va, "name",
			 fp->uf_va_name == تہی ? (char_u *)"" : fp->uf_va_name);
	اگر (fp->uf_va_type == تہی)
	    dict_جوڑو_string(va, "type", (char_u *)"list<any>");
	ورنہ
	{
	    dict_جوڑو_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    اگر (is_def)
    {
	dict_جوڑو_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    ورنہ
	dict_جوڑو_string(d, "returns", (char_u *)"any");
    dict_جوڑو_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_جوڑو_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_جوڑو_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_جوڑو_bool(d, "closure", (fp->uf_flags & FC_احاطہ) != 0);
    dict_جوڑو_نمبر(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_جوڑو_نمبر(d, "lnum", fp->uf_script_ctx.sc_lnum);
    واپس OK;
}

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, صحیح check)
{
    char_u	*nm = name;
    char_u	*p;
    صحیح		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    اگر (p != تہی && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	واپس p;

    vim_free(p);
    واپس تہی;
}
#اگر_ختم

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, صحیح idx)
{
    ساکن long_u	مکمل;
    ساکن صحیح		بدلا;
    ساکن hashitem_T	*hi;
    ufunc_T		*fp;

    اگر (idx == 0)
    {
	مکمل = 0;
	hi = func_hashtab.ht_array;
	بدلا = func_hashtab.ht_بدلا;
    }
    اگر (بدلا == func_hashtab.ht_بدلا && مکمل < func_hashtab.ht_مستعمل)
    {
	صحیح len;

	اگر (مکمل++ > 0)
	    ++hi;
	جب (HASHITEM_خالی_خانے(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	اگر ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    واپس (char_u *)"";

	اگر (fp->uf_namelen + 4 >= IOSIZE)
	    واپس fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	اگر (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    اگر (!has_varargs(fp) && fp->uf_دلائل.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	واپس IObuff;
    }
    واپس تہی;
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
    اگر (ufunc == تہی)
	واپس تہی;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_حالت = UF_مقصد_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = تہی;
#اگر_تعریف_شدہ FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_گنتی = تہی;
    ufunc->uf_tml_کل = تہی;
    ufunc->uf_tml_self = تہی;
#اگر_ختم

    ga_copy_لڑیاں(&fp->uf_دلائل, &ufunc->uf_دلائل);
    ga_copy_لڑیاں(&fp->uf_def_دلائل, &ufunc->uf_def_دلائل);

    اگر (ufunc->uf_arg_types != تہی)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_دلائل.ga_len);
	اگر (at != تہی)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     حجم(type_T *) * ufunc->uf_دلائل.ga_len);
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

    اگر (ufunc->uf_va_name != تہی)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_بلاک_گہرائی = 0;
    ufunc->uf_بلاک_ids = تہی;

    ga_copy_لڑیاں(&fp->uf_سطریں, &ufunc->uf_سطریں);

#اگر_تعریف_شدہ FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    اگر (fp->uf_profiling)
	func_do_profile(ufunc);
#اگر_ختم

    ufunc->uf_refcount = 1;

    واپس ufunc;
}

/*
 * ":delfunction {name}"
 */
    خالی
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = تہی;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    صحیح		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     تہی, تہی, تہی);
    vim_free(fudi.fd_newkey);
    اگر (name == تہی)
    {
	اگر (fudi.fd_dict != تہی && !eap->skip)
	    emsg(_(e_funcref_required));
	واپس;
    }
    اگر (!سرے_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	واپس;
    }
    سیٹ_nextcmd(eap, p);
    اگر (eap->nextcmd != تہی)
	*p = NUL;

    اگر (numbered_function(name) && fudi.fd_dict == تہی)
    {
	اگر (!eap->skip)
	    semsg(_(e_invalid_دلیل_str), eap->arg);
	vim_free(name);
	واپس;
    }
    اگر (!eap->skip)
	fp = ڈھونڈو_func(name, is_global);
    vim_free(name);

    اگر (!eap->skip)
    {
	اگر (fp == تہی)
	{
	    اگر (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    واپس;
	}
	اگر (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_اندر_use), eap->arg);
	    واپس;
	}
	اگر (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    واپس;
	}

	اگر (fudi.fd_dict != تہی)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	ورنہ
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
	    ورنہ
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    خالی
func_unref(char_u *name)
{
    ufunc_T *fp = تہی;

    اگر (name == تہی || !func_name_refcount(name))
	واپس;
    fp = ڈھونڈو_func(name, FALSE);
    اگر (fp == تہی && numbered_function(name))
    {
#اگر_تعریف_شدہ EXITFREE
	اگر (!entered_free_all_mem)
#اگر_ختم
	    اندرونی_error("func_unref()");
    }
    func_ptr_unref(fp);
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 * Also when it becomes one and uf_partial points to the function.
 */
    خالی
func_ptr_unref(ufunc_T *fp)
{
    اگر (fp != تہی && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != تہی
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
    خالی
func_ref(char_u *name)
{
    ufunc_T *fp;

    اگر (name == تہی || !func_name_refcount(name))
	واپس;
    fp = ڈھونڈو_func(name, FALSE);
    اگر (fp != تہی)
	++fp->uf_refcount;
    ورنہ اگر (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	اندرونی_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    خالی
func_ptr_ref(ufunc_T *fp)
{
    اگر (fp != تہی)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    ساکن صحیح
can_free_funccal(funccall_T *fc, صحیح copyID)
{
    واپس (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    خالی
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    صحیح		returning = FALSE;
    evalarg_T	evalarg;

    اگر (current_funccal == تہی)
    {
	emsg(_(e_return_not_inside_function));
	واپس;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    اگر (eap->skip)
	++emsg_skip;

    eap->nextcmd = تہی;
    اگر ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	اگر (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	ورنہ
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    ورنہ اگر (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	اگر (!aborting())
	    returning = do_return(eap, FALSE, TRUE, تہی);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    اگر (returning)
	eap->nextcmd = تہی;
    ورنہ اگر (eap->nextcmd == تہی)	    // no argument
	سیٹ_nextcmd(eap, arg);

    اگر (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    ساکن صحیح
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
    برائے ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	اگر (eap->addr_گنتی > 0)
	{
	    اگر (lnum > curbuf->b_ml.ml_سطر_گنتی)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		توڑو;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.ستون = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	اگر (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    توڑو;
	}
	اگر (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	اگر (handle_subscript(arg, تہی, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    توڑو;
	}

	clear_tv(&rettv);
	اگر (doesrange)
	    توڑو;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	اگر (aborting())
	    توڑو;
    }
    واپس failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    ساکن صحیح
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_دلائل + 1];	// vars for arguments
    صحیح		partial_argc = 0;		// number of partial arguments
    صحیح		argcount = 0;			// number of arguments found
    صحیح		r;

    اگر (current_funccal == تہی)
    {
	semsg(_(e_str_not_inside_function), "defer");
	واپس FAIL;
    }
    اگر (partial != تہی)
    {
	اگر (partial->pt_dict != تہی)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    واپس FAIL;
	}
	اگر (partial->pt_argc > 0)
	{
	    صحیح i;

	    partial_argc = partial->pt_argc;
	    برائے (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    صحیح is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    اگر (r == OK)
    {
	اگر (type != تہی)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_دلیل_types(type, argvars, argcount, تہی, name);
	}
	ورنہ اگر (is_builtin)
	{
	    صحیح idx = ڈھونڈو_اندرونی_func(name);

	    اگر (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    ورنہ اگر (check_اندرونی_func(idx, argcount) == -1)
		r = FAIL;
	}
	ورنہ
	{
	    ufunc_T *ufunc = ڈھونڈو_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    اگر (ufunc != تہی)
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
	جب (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	واپس FAIL;
    }
    واپس جوڑو_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    صحیح
can_جوڑو_defer(خالی)
{
    اگر (!اندر_def_function() && get_current_funccal() == تہی)
    {
	semsg(_(e_str_not_inside_function), "defer");
	واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    صحیح
جوڑو_defer(char_u *name, صحیح argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    صحیح		argcount = argcount_arg;
    defer_T	*dr;
    صحیح		ret = FAIL;

    اگر (saved_name == تہی)
	جاؤ theend;
    اگر (اندر_def_function())
    {
	اگر (جوڑو_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    ورنہ
    {
	اگر (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, حجم(defer_T), 10);
	اگر (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    جاؤ theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_ڈیٹا)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	جب (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    جب (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    واپس ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    ساکن خالی
handle_defer_one(funccall_T *funccal)
{
    صحیح	    idx;

    برائے (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_ڈیٹا) + idx;

	اگر (dr->dr_name == تہی)
	    // already being called, can happen if function does ":qa"
	    جاری;

	funcexe_T   funcexe;
	CLEAR_میدان(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = تہی;

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
	برائے (صحیح i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    ساکن خالی
invoke_funccall_defer(funccall_T *fc)
{
    اگر (fc->fc_ectx != تہی)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    ورنہ
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    خالی
invoke_all_defer(خالی)
{
    برائے (funccall_T *fc = current_funccal; fc != تہی; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    برائے (funccal_اندراج_T *fce = funccal_ڈھیر; fce != تہی; fce = fce->اگلا)
	برائے (funccall_T *fc = fce->چوٹی_funccal; fc != تہی; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    خالی
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    صحیح		len;
    صحیح		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = تہی;
    partial_T	*partial = تہی;
    evalarg_T	evalarg;
    type_T	*type = تہی;
    صحیح		ملا_var = FALSE;
    صحیح		vim9script = اندر_vim9script();

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
	واپس;
    }

    tofree = trans_function_name_ext(&arg, تہی, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : تہی, &ufunc);
    اگر (fudi.fd_newkey != تہی)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_کلید_not_present_اندر_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    اگر (tofree == تہی)
	واپس;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    اگر (fudi.fd_dict != تہی)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (صحیح)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != تہی ? تہی : &partial,
				vim9script && type == تہی ? &type : تہی,
						     FALSE, FALSE, &ملا_var);

    اگر (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_دلائل(name, STRLEN(name), &arg);
	اگر (name == تہی)
	    جاؤ انتہا;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    اگر (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	جاؤ انتہا;
    }
    اگر (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	جاؤ انتہا;
    }

    اگر (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    ورنہ
    {
	funcexe_T funcexe;

	CLEAR_میدان(funcexe);
	funcexe.fe_check_type = type;
	funcexe.fe_ufunc = ufunc;
	funcexe.fe_partial = partial;
	funcexe.fe_selfdict = fudi.fd_dict;
	funcexe.fe_firstline = eap->line1;
	funcexe.fe_lastline = eap->line2;
	funcexe.fe_ملا_var = ملا_var;
	funcexe.fe_evaluate = TRUE;
	failed = ex_call_inner(eap, name, &arg, startarg, &funcexe, &evalarg);
    }

    // When inside :try we need to check for following "| catch" or "| endtry".
    // Not when there was an error, but do check if an exception was thrown.
    اگر ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	اگر (!سرے_excmd2(eap->arg, arg))
	{
	    اگر (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	ورنہ
	    سیٹ_nextcmd(eap, arg);
    }
    // Must be after using "arg", it may point into memory cleared here.
    clear_evalarg(&evalarg, eap);

انتہا:
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
    خالی	*rettv)
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
	ورنہ
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    اگر (reanimate)
		rettv = current_funccal->fc_rettv;

	    اگر (rettv != تہی)
	    {
		// Store the value of the pending return.
		اگر ((cstack->cs_rettv[idx] = alloc_tv()) != تہی)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		ورنہ
		    emsg(_(e_برآمد_کا_حافظہ));
	    }
	    ورنہ
		cstack->cs_rettv[idx] = تہی;

	    اگر (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_نمبر;
		current_funccal->fc_rettv->vval.v_نمبر = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    ورنہ
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	اگر (!reanimate && rettv != تہی)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    اگر (!is_cmd)
		vim_free(rettv);
	}
    }

    واپس idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    خالی
discard_pending_return(خالی *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(خالی *rettv)
{
    char_u	*s = تہی;
    char_u	*tofree = تہی;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    اگر (rettv != تہی)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    اگر (s == تہی)
	s = (char_u *)"";
    ورنہ
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
    واپس vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_سطر(
    صحیح	    c غیر_مستعمل,
    خالی    *cookie,
    صحیح	    indent غیر_مستعمل,
    getline_opt_T options غیر_مستعمل)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    اگر (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_ڈھونڈو_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	func_سطر_انتہا(cookie);
#اگر_ختم

    gap = &fp->uf_سطریں;
    اگر (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_اندر_try())
	    || fcp->fc_returned)
	retval = تہی;
    ورنہ
    {
	// Skip NULL lines (continuation lines).
	جب (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_ڈیٹا))[fcp->fc_linenr] == تہی)
	    ++fcp->fc_linenr;
	اگر (fcp->fc_linenr >= gap->ga_len)
	    retval = تہی;
	ورنہ
	{
	    retval = vim_strsave(((char_u **)(gap->ga_ڈیٹا))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#اگر_تعریف_شدہ FEAT_PROFILE
	    اگر (do_profiling == PROF_YES)
		func_سطر_آغاز(cookie, SOURCING_LNUM);
#اگر_ختم
	}
    }

    // Did we encounter a breakpoint?
    اگر (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_ڈھونڈو_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    واپس retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    صحیح
func_has_ended(خالی *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    واپس (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_اندر_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    صحیح
func_has_abort(
    خالی    *cookie)
{
    واپس ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
}


/*
 * Turn "dict.Func" into a partial for "Func" bound to "dict".
 * Don't do this when "Func" is already a partial that was bound
 * explicitly (pt_auto is FALSE).
 * Changes "rettv" in-place.
 * Returns the updated "selfdict_in".
 */
    dict_T *
make_partial(dict_T *selfdict_اندر, typval_T *rettv)
{
    char_u	*fname;
    ufunc_T	*fp = تہی;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_اندر;

    اگر (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != تہی
				     && rettv->vval.v_partial->pt_func != تہی)
	fp = rettv->vval.v_partial->pt_func;
    ورنہ
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == تہی ? تہی
					      : rettv->vval.v_partial->pt_name;
	اگر (fname == تہی)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = تہی;
	}
	ورنہ
	{
	    char_u	*tofree = تہی;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = ڈھونڈو_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    اگر (fp != تہی && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	اگر (pt != تہی)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = تہی;
	    اگر (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    ورنہ
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		صحیح		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		اگر (ret_pt->pt_name != تہی)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		ورنہ
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		اگر (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    اگر (pt->pt_argv == تہی)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    ورنہ
		    {
			pt->pt_argc = ret_pt->pt_argc;
			برائے (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    واپس selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(خالی *cookie)
{
    واپس ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(خالی *cookie)
{
    واپس &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    صحیح *
func_dbg_tick(خالی *cookie)
{
    واپس &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    صحیح
func_level(خالی *cookie)
{
    واپس ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    صحیح
current_func_returned(خالی)
{
    واپس current_funccal->fc_returned;
}

    صحیح
free_unref_funccal(صحیح copyID, صحیح testing)
{
    صحیح		did_free = FALSE;
    صحیح		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    برائے (pfc = &پچھلا_funccal; *pfc != تہی; )
    {
	اگر (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	ورنہ
	    pfc = &(*pfc)->fc_caller;
    }
    اگر (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(خالی)garbage_collect(testing);

    واپس did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    ساکن funccall_T *
get_funccal(خالی)
{
    صحیح		i;
    funccall_T	*funccal;
    funccall_T	*عارضی_funccal;

    funccal = current_funccal;
    اگر (debug_backtrace_level > 0)
    {
	برائے (i = 0; i < debug_backtrace_level; i++)
	{
	    عارضی_funccal = funccal->fc_caller;
	    اگر (عارضی_funccal)
		funccal = عارضی_funccal;
	    ورنہ
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    واپس funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    ساکن funccall_T *
get_funccal_for_vars(خالی)
{
    funccall_T	*funccal = تہی;

    اگر (current_funccal == تہی)
	واپس تہی;
    funccal = get_funccal();
    اگر (funccal == تہی || funccal->fc_l_vars.dv_refcount == 0)
	واپس تہی;
    واپس funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(خالی)
{
    funccall_T	*funccal = get_funccal_for_vars();

    واپس funccal == تہی ? تہی : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(خالی)
{
    funccall_T	*funccal = get_funccal_for_vars();

    واپس funccal == تہی ? تہی : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_دلائل_ht(خالی)
{
    funccall_T	*funccal = get_funccal_for_vars();

    واپس funccal == تہی ? تہی : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_دلائل_var(خالی)
{
    funccall_T	*funccal = get_funccal_for_vars();

    واپس funccal == تہی ? تہی : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    خالی
list_func_vars(صحیح *پہلا)
{
    اگر (current_funccal != تہی && current_funccal->fc_l_vars.dv_refcount > 0)
	list_hashtable_vars(&current_funccal->fc_l_vars.dv_hashtab,
							   "l:", FALSE, پہلا);
}

/*
 * If "ht" is the hashtable for local variables in the current funccal, return
 * the dict that contains it.
 * Otherwise return NULL.
 */
    dict_T *
get_current_funccal_dict(hashtab_T *ht)
{
    اگر (current_funccal != تہی
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	واپس &current_funccal->fc_l_vars;
    واپس تہی;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
ڈھونڈو_hi_اندر_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = تہی;
    char_u	*varname;

    اگر (current_funccal == تہی || current_funccal->fc_func->uf_scoped == تہی)
	واپس تہی;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    جب (current_funccal != تہی)
    {
	ht = ڈھونڈو_var_ht(name, &varname);
	اگر (ht != تہی && *varname != NUL)
	{
	    hi = hash_ڈھونڈو(ht, varname);
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		*pht = ht;
		توڑو;
	    }
	}
	اگر (current_funccal == current_funccal->fc_func->uf_scoped)
	    توڑو;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    واپس hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
ڈھونڈو_var_اندر_scoped_ht(char_u *name, صحیح no_autoload)
{
    dictitem_T	*v = تہی;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    اگر (current_funccal == تہی || current_funccal->fc_func->uf_scoped == تہی)
	واپس تہی;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    جب (current_funccal)
    {
	ht = ڈھونڈو_var_ht(name, &varname);
	اگر (ht != تہی && *varname != NUL)
	{
	    v = ڈھونڈو_var_اندر_ht(ht, *name, varname, no_autoload);
	    اگر (v != تہی)
		توڑو;
	}
	اگر (current_funccal == current_funccal->fc_func->uf_scoped)
	    توڑو;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    واپس v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    صحیح
سیٹ_ref_اندر_پچھلا_funccal(صحیح copyID)
{
    funccall_T	*fc;

    برائے (fc = پچھلا_funccal; fc != تہی; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	اگر (سیٹ_ref_اندر_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, تہی, تہی)
		|| سیٹ_ref_اندر_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, تہی, تہی)
		|| سیٹ_ref_اندر_list_اشیاء(&fc->fc_l_varlist, copyID + 1, تہی, تہی))
	    واپس TRUE;
    }
    واپس FALSE;
}

    ساکن صحیح
سیٹ_ref_اندر_funccal(funccall_T *fc, صحیح copyID)
{
    اگر (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	اگر (سیٹ_ref_اندر_ht(&fc->fc_l_vars.dv_hashtab, copyID, تہی, تہی)
		|| سیٹ_ref_اندر_ht(&fc->fc_l_avars.dv_hashtab, copyID, تہی, تہی)
		|| سیٹ_ref_اندر_list_اشیاء(&fc->fc_l_varlist, copyID, تہی, تہی)
		|| سیٹ_ref_اندر_func(تہی, fc->fc_func, copyID))
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    صحیح
سیٹ_ref_اندر_call_ڈھیر(صحیح copyID)
{
    funccall_T		*fc;
    funccal_اندراج_T	*اندراج_2;

    برائے (fc = current_funccal; fc != تہی; fc = fc->fc_caller)
	اگر (سیٹ_ref_اندر_funccal(fc, copyID))
	    واپس TRUE;

    // Also go through the funccal_stack.
    برائے (اندراج_2 = funccal_ڈھیر; اندراج_2 != تہی; اندراج_2 = اندراج_2->اگلا)
	برائے (fc = اندراج_2->چوٹی_funccal; fc != تہی; fc = fc->fc_caller)
	    اگر (سیٹ_ref_اندر_funccal(fc, copyID))
		واپس TRUE;
    واپس FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    صحیح
سیٹ_ref_اندر_functions(صحیح copyID)
{
    صحیح		todo;
    hashitem_T	*hi = تہی;
    ufunc_T	*fp;

    todo = (صحیح)func_hashtab.ht_مستعمل;
    برائے (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    اگر (!func_name_refcount(fp->uf_name)
					  && سیٹ_ref_اندر_func(تہی, fp, copyID))
		واپس TRUE;
	}
    }
    واپس FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    صحیح
سیٹ_ref_اندر_func_دلائل(صحیح copyID)
{
    صحیح i;

    برائے (i = 0; i < funcargs.ga_len; ++i)
	اگر (سیٹ_ref_اندر_شے(((typval_T **)funcargs.ga_ڈیٹا)[i],
						copyID, تہی, تہی, تہی))
	    واپس TRUE;
    واپس FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
سیٹ_ref_اندر_func(char_u *name, ufunc_T *fp_اندر, صحیح copyID)
{
    ufunc_T	*fp = fp_اندر;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = تہی;
    char_u	*fname;
    صحیح		abort = FALSE;

    اگر (name == تہی && fp_اندر == تہی)
	واپس FALSE;

    اگر (fp_اندر == تہی)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = ڈھونڈو_func(fname, FALSE);
    }
    اگر (fp != تہی)
    {
	برائے (fc = fp->uf_scoped; fc != تہی; fc = fc->fc_func->uf_scoped)
	    abort = abort || سیٹ_ref_اندر_funccal(fc, copyID);
    }

    vim_free(tofree);
    واپس abort;
}

#اگر_ختم // FEAT_EVAL
