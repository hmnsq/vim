/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9generics.c: Vim9 script generics support
 */

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_EVAL)


/*
 * A hash table is used to lookup a generic function with specific types.
 * The specific type names are used as the key.
 */
প্রকারনির্দেশ সংরচনা gfitem_S gfitem_T;
সংরচনা gfitem_S
{
    ufunc_T	*gfi_ufunc;
    char_u	gfi_name[1];	// actually longer
};
#define GFITEM_চাবি_OFF	offsetof(gfitem_T, gfi_name)
#define HI2GFITEM(hi)	((gfitem_T *)((hi)->hi_চাবি - GFITEM_চাবি_OFF))

স্থির type_T *find_generic_type_in_cctx(char_u *gt_name, size_t len, cctx_T *cctx);

/*
 * Returns a pointer to the first '<' character in "name" that starts the
 * generic type argument list, skipping an initial <SNR> or <lambda> prefix if
 * present.  The prefix is only skipped if "name" starts with '<'.
 *
 * Returns NULL if no '<' is found before a '(' or the end of the string.
 * The returned pointer refers to the original string.
 *
 * Examples:
 *   "<SNR>123_Fn<number>"    -> returns pointer to '<'
 *   "<lambda>123_Fn<number>" -> returns pointer to '<'
 *   "Func<number>"           -> returns pointer to '<'
 *   "Func()"                 -> returns NULL
 */
    char_u *
generic_func_find_open_bracket(char_u *name)
{
    char_u	*p = name;

    যদি (name[0] == '<')
    {
	// Skip the <SNR> or <lambda> at the start of the name
	যদি (STRNCMP(name + 1, "SNR>", 4) == 0)
	    p += 5;
	অন্যথা যদি (STRNCMP(name + 1, "lambda>", 7) == 0)
	    p += 8;
    }

    যেতিয়ালৈ (*p && *p != '(' && *p != '<')
	p++;

    যদি (*p == '<')
	ঘূৰাই p;

    ঘূৰাই রিক্ত;
}

/*
 * Finds the matching '>' character for a generic function type parameter or
 * argument list, starting from the opening '<'.
 *
 * Enforces correct syntax for a flat, comma-separated list of types:
 * - No whitespace before or after type names or commas
 * - Each type must be non-empty and separated by a comma and whitespace
 * - At least one type must be present
 *
 * Arguments:
 *   start - pointer to the opening '<'
 *
 * Returns:
 *   Pointer to the matching '>' character if found and syntax is valid,
 *   or NULL if not found, invalid syntax, or on error.
 */
    স্থির char_u *
generic_func_find_close_bracket(char_u *start)
{
    char_u	*p = start + 1;
    পূর্ণাঙ্ক		type_গুনতি = 0;

    যেতিয়ালৈ (*p && *p != '>')
    {
	char_u	*typename = p;

	যদি (VIM_ISWHITE(*p))
	{
	    ৱর্ণ tmpstr[2];
	    tmpstr[0] = *(p - 1); tmpstr[1] = NUL;
	    semsg(_(e_no_white_space_allowed_after_str_str), tmpstr, start);
	    ঘূৰাই রিক্ত;
	}

	p = skip_type(p, FALSE);
	যদি (p == typename)
	{
	    char_u cc = *p;
	    *p = NUL;
	    semsg(_(e_missing_type_after_str), start);
	    *p = cc;
	    ঘূৰাই রিক্ত;
	}
	type_গুনতি++;

	যদি (*p == '>' || *p == NUL)
	    ভঙ্গ;

	যদি (VIM_ISWHITE(*p))
	{
	    char_u cc = *p;
	    *p = NUL;
	    semsg(_(e_no_white_space_allowed_after_str_str), typename, start);
	    *p = cc;
	    ঘূৰাই রিক্ত;
	}

	যদি (*p != ',')
	{
	    semsg(_(e_missing_comma_in_generic_function_str), start);
	    ঘূৰাই রিক্ত;
	}
	p++;

	যদি (*p == NUL)
	    ভঙ্গ;

	যদি (!VIM_ISWHITE(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", start);
	    ঘূৰাই রিক্ত;
	}
	p = skipwhite(p);
    }

    যদি (*p != '>')
    {
	semsg(_(e_missing_closing_angle_bracket_in_generic_function_str), start);
	ঘূৰাই রিক্ত;
    }

    যদি (VIM_ISWHITE(*(p + 1)) && *skipwhite(p + 1) == '(')
    {
	// white space not allowed between '>' and '('
	semsg(_(e_no_white_space_allowed_after_str_str), ">", start);
	ঘূৰাই রিক্ত;
    }


    যদি (type_গুনতি == 0)
    {
	semsg(_(e_empty_type_list_for_generic_function_str), start);
	ঘূৰাই রিক্ত;
    }

    ঘূৰাই p;
}

/*
 * Advances the argument pointer past a generic function's type argument list.
 *
 * On entry, "*argp" must point to the opening '<' of a generic type argument
 * list.  This function finds the matching closing '>' (validating the syntax
 * via generic_func_find_close_bracket), and if successful, advances "*argp" to
 * the character immediately after the closing '>'.
 *
 * Returns OK on success, or FAIL if the type argument list is invalid or no
 * matching '>' is found. On failure, "*argp" is not modified.
 */
    পূর্ণাঙ্ক
skip_generic_func_type_args(char_u **argp)
{
    char_u *p = generic_func_find_close_bracket(*argp);
    যদি (p == রিক্ত)
	ঘূৰাই FAIL;

    *argp = p + 1;	// skip '>'

    ঘূৰাই OK;
}

/*
 * Appends the generic function type arguments, starting at "*argp", to the
 * function name "funcname" (of length "namelen") and returns a newly allocated
 * string containing the result.
 *
 * On entry, "*argp" must point to the opening '<' of the generic type argument
 * list.  If the type argument list is valid, the substring from "*argp" up to
 * and including the matching '>' is appended to "funcname". On success,
 * "*argp" is updated to point to the character after the closing '>'.
 *
 * Returns:
 *   A newly allocated string with the combined function name and type
 *   arguments, or NULL if there is a syntax error in the generic type
 *   arguments or if there is an allocation failure.
 *
 * The caller is responsible for freeing the returned string.
 */
    char_u *
append_generic_func_type_args(
    char_u	*funcname,
    size_t	namelen,
    char_u	**argp)
{
    char_u	*p = generic_func_find_close_bracket(*argp);
    size_t	argslen;
    char_u	*name;

    যদি (p == রিক্ত)
	ঘূৰাই রিক্ত;

    // The type arguments can be of any length, do not use IObuff.
    argslen = (size_t)(p - *argp) + 1;
    name = alloc(namelen + argslen + 1);
    যদি (name == রিক্ত)
	ঘূৰাই রিক্ত;
    mch_memmove(name, funcname, namelen);
    mch_memmove(name + namelen, *argp, argslen);
    name[namelen + argslen] = NUL;

    *argp = p + 1;

    ঘূৰাই name;
}

/*
 * Returns a newly allocated string containing the function name from "fp" with
 * the generic type arguments from "*argp" appended.
 *
 * On entry, "*argp" must point to the opening '<' of the generic type argument
 * list.  On success, "*argp" is advanced to the character after the closing
 * '>'.
 *
 * Returns:
 *   A newly allocated string with the combined function name and type
 *   arguments, or NULL if "fp" is not a generic function, if there is a
 *   parsing error, or on memory allocation failure.
 *
 * The caller is responsible for freeing the returned string.
 */
    char_u *
get_generic_func_name(ufunc_T *fp, char_u **argp)
{
    যদি (!IS_GENERIC_FUNC(fp))
    {
	emsg_funcname(e_not_a_generic_function_str, fp->uf_name);
	ঘূৰাই রিক্ত;
    }

    ঘূৰাই append_generic_func_type_args(fp->uf_name, fp->uf_namelen, argp);
}

/*
 * Parses the concrete type arguments provided in a generic function call,
 * starting at the opening '<' character and ending at the matching '>'.
 *
 * On entry, "start" must point to the opening '<' character.
 * On success, returns a pointer to the character after the closing '>'.
 * On failure, returns NULL and reports an error message.
 *
 * Arguments:
 *   func_name - the name of the function being called (used for error
 *               messages)
 *   namelen   - length of the function name
 *   start     - pointer to the opening '<' character in the call
 *   gfatab    - args table to allocate new type objects and to store parsed
 *               type argument names and their types.
 *   cctx      - compile context for type resolution (may be NULL)
 *
 * This function enforces correct syntax for generic type argument lists,
 * including whitespace rules, comma separation, and non-empty argument lists.
 */
    char_u *
parse_generic_func_type_args(
    char_u		*func_name,
    size_t		namelen,
    char_u		*start,
    gfargs_tab_T	*gfatab,
    cctx_T		*cctx)
{
    generic_T	*generic_arg;
    type_T	*type_arg;
    char_u	*p = start;

    // White spaces not allowed after '<'
    যদি (VIM_ISWHITE(*(p + 1)))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), "<", p);
	ঘূৰাই রিক্ত;
    }

    ++p;	// skip the '<'

    // parse each type argument until '>' or end of string
    যেতিয়ালৈ (*p && *p != '>')
    {
	p = skipwhite(p);

	যদি (!ASCII_ISALNUM(*p))
	{
	    semsg(_(e_missing_type_after_str), start);
	    ঘূৰাই রিক্ত;
	}

	// parse the type
	type_arg = parse_type(&p, &gfatab->gfat_arg_types, রিক্ত, cctx, TRUE);
	যদি (type_arg == রিক্ত || !valid_declaration_type(type_arg))
	    ঘূৰাই রিক্ত;

	ৱর্ণ	*ret_free = রিক্ত;
	ৱর্ণ	*ret_name = type_name(type_arg, &ret_free);

	// create space for the name and the new type
	যদি (ga_grow(&gfatab->gfat_args, 1) == FAIL)
	{
	    vim_free(ret_free);
	    ঘূৰাই রিক্ত;
	}
	generic_arg = (generic_T *)gfatab->gfat_args.ga_data +
						gfatab->gfat_args.ga_len;
	gfatab->gfat_args.ga_len++;

	// copy the type name
	generic_arg->gt_name = alloc(STRLEN(ret_name) + 1);
	যদি (generic_arg->gt_name == রিক্ত)
	    ঘূৰাই রিক্ত;
	STRCPY(generic_arg->gt_name, ret_name);
	vim_free(ret_free);

	// add the new type
	generic_arg->gt_type = type_arg;

	p = skipwhite(p);

	যদি (*p == NUL || *p == '>')
	    ভঙ্গ;

	// after a type, expect ',' or '>'
	যদি (*p != ',')
	{
	    semsg(_(e_missing_comma_in_generic_function_str), start);
	    ঘূৰাই রিক্ত;
	}

	যদি (*(p + 1) == NUL)
	    ভঙ্গ;

	// Require whitespace after a comma and skip it
	যদি (!VIM_ISWHITE(*(p + 1)))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p);
	    ঘূৰাই রিক্ত;
	}
	p++;
    }

    // ensure the list of types ends in a closing '>'
    যদি (*p != '>')
    {
	semsg(_(e_missing_closing_angle_bracket_in_generic_function_str),
		func_name);
	ঘূৰাই রিক্ত;
    }

    // no whitespace allowed before '>'
    যদি (VIM_ISWHITE(*(p - 1)))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), ">", p);
	ঘূৰাই রিক্ত;
    }

    // at least one type argument is required
    যদি (generic_func_args_table_size(gfatab) == 0)
    {
	char_u	cc = func_name[namelen];
	func_name[namelen] = NUL;
	semsg(_(e_empty_type_list_for_generic_function_str), func_name);
	func_name[namelen] = cc;
	ঘূৰাই রিক্ত;
    }
    ++p;	// skip the '>'

    ঘূৰাই p;
}

/*
 * Checks if a generic type name already exists in the current context.
 *
 * This function verifies that the given generic type name "name" does not
 * conflict with an imported variable, an existing generic type in the provided
 * growarray "gt_gap", or a generic type in the current or outer compile
 * context "cctx". If a conflict is found, an appropriate error message is
 * reported.
 *
 * Arguments:
 *   name    - the generic type name to check
 *   gfatab  - args table to allocate new type objects and to store parsed
 *             type argument names and their types.
 *   cctx    - current compile context, used to check for outer generic types
 *   (may be NULL)
 *
 * Returns:
 *   TRUE if the name already exists or conflicts, FALSE otherwise.
 */
    স্থির পূর্ণাঙ্ক
generic_name_exists(
    char_u		*gt_name,
    size_t		name_len,
    gfargs_tab_T	*gfatab,
    cctx_T		*cctx)
{
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;

    যদি (eval_variable_import(gt_name, &tv) == OK)
    {
	semsg(_(e_redefining_script_item_str), gt_name);
	clear_tv(&tv);
	ঘূৰাই TRUE;
    }

    বাবে (পূর্ণাঙ্ক i = 0; i < gfatab->gfat_args.ga_len; i++)
    {
	generic_T *generic = &((generic_T *)gfatab->gfat_args.ga_data)[i];

	যদি (STRNCMP(gt_name, generic->gt_name, name_len) == 0
				&& generic->gt_name[name_len] == NUL)
	{
	    semsg(_(e_duplicate_type_var_name_str), gt_name);
	    ঘূৰাই TRUE;
	}
    }

    যদি (cctx != রিক্ত &&
	    find_generic_type_in_cctx(gt_name, name_len, cctx) != রিক্ত)
    {
	semsg(_(e_duplicate_type_var_name_str), gt_name);
	ঘূৰাই TRUE;
    }

    ঘূৰাই FALSE;
}

/*
 * Parses the type parameters specified when defining a new generic function,
 * starting at the opening '<' character and ending at the matching '>'.
 *
 * On entry, "p" must point to the opening '<' character.
 * On success, returns a pointer to the character after the closing '>'.
 * On failure, returns NULL and reports an error message.
 *
 * Arguments:
 *   func_name - the name of the function being defined (for error messages)
 *   p         - pointer to the opening '<' character in the definition
 *   gfatab    - args table to allocate new type objects and to store parsed
 *               type argument names and their types.
 *   cctx      - current compile context, used to check for duplicate names in
 *		 outer scopes (may be NULL)
 *
 * This function enforces correct syntax for generic type parameter lists:
 * - No whitespace before or after the opening '<'
 * - Parameters must be separated by a comma and whitespace
 * - No whitespace after a parameter name
 * - The list must not be empty
 */
    char_u *
parse_generic_func_type_params(
    char_u		*func_name,
    char_u		*p,
    gfargs_tab_T	*gfatab,
    cctx_T		*cctx)
{
    // No white space allowed before the '<'
    যদি (VIM_ISWHITE(*(p - 1)))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "<", p);
	ঘূৰাই রিক্ত;
    }

    যদি (VIM_ISWHITE(*(p + 1)))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), "<", p);
	ঘূৰাই রিক্ত;
    }

    // Find the start of the function name before the '<'.  It is always
    // preceded by "def" and white space, or "g:".
    char_u	    *fname_start = p;
    যেতিয়ালৈ (ASCII_ISALNUM(fname_start[-1]) || fname_start[-1] == '_')
	--fname_start;
    size_t	    fname_len = p - fname_start;

    char_u	    *start = ++p;

    যেতিয়ালৈ (*p && *p != '>')
    {
	p = skipwhite(p);

	যদি (*p == NUL || *p == '>')
	{
	    semsg(_(e_missing_type_after_str), p - 1);
	    ঘূৰাই রিক্ত;
	}

	যদি (!ASCII_ISUPPER(*p))
	{
	    যদি (ASCII_ISLOWER(*p))
		semsg(_(e_type_var_name_must_start_with_uppercase_letter_str), p);
	    অন্যথা
		semsg(_(e_missing_type_after_str), p - 1);
	    ঘূৰাই রিক্ত;
	}

	char_u	*name_start = p;
	char_u	*name_end = রিক্ত;
	char_u	cc;
	size_t	name_len = 0;

	p++;
	যেতিয়ালৈ (ASCII_ISALNUM(*p) || *p == '_')
	    p++;
	name_end = p;

	name_len = name_end - name_start;
	cc = *name_end;
	*name_end = NUL;

	// The type variable cannot have the name of the function being
	// defined.  Use the name as it appears before the '<', a nested
	// function internally has a "<lambda>N" name.
	যদি (fname_len == name_len
			&& STRNCMP(fname_start, name_start, name_len) == 0)
	{
	    semsg(_(e_redefining_script_item_str), name_start);
	    *name_end = cc;
	    ঘূৰাই রিক্ত;
	}

	পূর্ণাঙ্ক name_exists = generic_name_exists(name_start, name_len, gfatab,
									cctx);
	*name_end = cc;
	যদি (name_exists)
	    ঘূৰাই রিক্ত;

	যদি (ga_grow(&gfatab->gfat_args, 1) == FAIL)
	    ঘূৰাই রিক্ত;
	generic_T *generic =
	    &((generic_T *)gfatab->gfat_args.ga_data)[gfatab->gfat_args.ga_len];
	gfatab->gfat_args.ga_len++;

	generic->gt_name = alloc(name_len + 1);
	যদি (generic->gt_name == রিক্ত)
	    ঘূৰাই রিক্ত;
	vim_strncpy(generic->gt_name, name_start, name_len);
	generic->gt_type = রিক্ত;

	যদি (VIM_ISWHITE(*p))
	{
	    semsg(_(e_no_white_space_allowed_after_str_str), generic->gt_name,
		    name_start);
	    ঘূৰাই রিক্ত;
	}

	যদি (*p != ',' && *p != '>')
	{
	    semsg(_(e_missing_comma_in_generic_function_str), start);
	    ঘূৰাই রিক্ত;
	}
	যদি (*p == ',')
	{
	    যদি (!VIM_ISWHITE(*(p + 1)))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p);
		ঘূৰাই রিক্ত;
	    }
	    p++;
	}
    }
    যদি (*p != '>')
	ঘূৰাই রিক্ত;
    p++;

    পূর্ণাঙ্ক gfat_sz = generic_func_args_table_size(gfatab);

    যদি (gfat_sz == 0)
    {
	emsg_funcname(e_empty_type_list_for_generic_function_str, func_name);
	ঘূৰাই রিক্ত;
    }

    // set the generic params to VAR_ANY type
    যদি (ga_grow(&gfatab->gfat_param_types, gfat_sz) == FAIL)
	ঘূৰাই রিক্ত;

    gfatab->gfat_param_types.ga_len = gfat_sz;
    বাবে (পূর্ণাঙ্ক i = 0; i < generic_func_args_table_size(gfatab); i++)
    {
	type_T *gt = &((type_T *)gfatab->gfat_param_types.ga_data)[i];

	CLEAR_POINTER(gt);
	gt->tt_type = VAR_ANY;
	gt->tt_flags = TTFLAG_GENERIC;

	generic_T *generic = &((generic_T *)gfatab->gfat_args.ga_data)[i];
	generic->gt_type = gt;
    }

    ঘূৰাই p;
}

/*
 * Initialize a new generic function "fp" using the list of generic types and
 * generic arguments in "gfatab".
 *
 * This function:
 *   - Marks the function as generic.
 *   - Sets the generic argument count and stores the type and argument lists.
 *   - Transfers ownership of the arrays from the growarrays to the function.
 *   - Initializes the generic function's lookup table.
 */
    শূন্য
generic_func_init(ufunc_T *fp, gfargs_tab_T *gfatab)
{
    fp->uf_flags |= FC_GENERIC;
    fp->uf_generic_argcount = gfatab->gfat_args.ga_len;
    fp->uf_generic_args = (generic_T *)gfatab->gfat_args.ga_data;
    ga_init(&gfatab->gfat_args);	// remove the reference to the args
    fp->uf_generic_param_types = (type_T *)gfatab->gfat_param_types.ga_data;
    ga_init(&gfatab->gfat_param_types);	// remove the reference to the types
    ga_init(&fp->uf_generic_arg_types);
    hash_init(&fp->uf_generic_functab);
}

/*
 * Initialize the generic function args table
 */
    শূন্য
generic_func_args_table_init(gfargs_tab_T *gfatab)
{
    ga_init2(&gfatab->gfat_args, পরিমাণ(generic_T), 10);
    ga_init2(&gfatab->gfat_param_types, পরিমাণ(type_T), 10);
    ga_init2(&gfatab->gfat_arg_types, পরিমাণ(type_T), 10);
}

/*
 * Return the number of entries in the generic function args table
 */
    পূর্ণাঙ্ক
generic_func_args_table_size(gfargs_tab_T *gfatab)
{
    ঘূৰাই gfatab->gfat_args.ga_len;
}

/*
 * Free all the generic function args table items
 */
    শূন্য
generic_func_args_table_clear(gfargs_tab_T *gfatab)
{
    clear_type_list(&gfatab->gfat_param_types);
    clear_type_list(&gfatab->gfat_arg_types);
    বাবে (পূর্ণাঙ্ক i = 0; i < gfatab->gfat_args.ga_len; i++)
    {
	generic_T *generic = &((generic_T *)gfatab->gfat_args.ga_data)[i];
	VIM_CLEAR(generic->gt_name);
    }
    ga_clear(&gfatab->gfat_args);
}

/*
 * When a cloning a function "fp" to "new_fp", copy the generic function
 * related information.
 */
    শূন্য
copy_generic_function(ufunc_T *fp, ufunc_T *new_fp)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		sz;

    যদি (!IS_GENERIC_FUNC(fp))
	ঘূৰাই;

    sz = fp->uf_generic_argcount * পরিমাণ(type_T);
    new_fp->uf_generic_param_types = alloc_clear(sz);
    যদি (new_fp->uf_generic_param_types == রিক্ত)
	ঘূৰাই;

    memcpy(new_fp->uf_generic_param_types, fp->uf_generic_param_types, sz);

    sz = fp->uf_generic_argcount * পরিমাণ(generic_T);
    new_fp->uf_generic_args = alloc_clear(sz);
    যদি (new_fp->uf_generic_args == রিক্ত)
    {
	VIM_CLEAR(new_fp->uf_generic_param_types);
	ঘূৰাই;
    }
    memcpy(new_fp->uf_generic_args, fp->uf_generic_args, sz);

    বাবে (i = 0; i < fp->uf_generic_argcount; i++)
	new_fp->uf_generic_args[i].gt_name =
	    vim_strsave(fp->uf_generic_args[i].gt_name);

    বাবে (i = 0; i < fp->uf_generic_argcount; i++)
	new_fp->uf_generic_args[i].gt_type =
	    &new_fp->uf_generic_param_types[i];

    ga_init(&new_fp->uf_generic_arg_types);
    hash_init(&new_fp->uf_generic_functab);
}

/*
 * Returns the index of the generic type pointer "t" in the generic type list
 * of the function "fp".
 *
 * Arguments:
 *   fp - pointer to the generic function (ufunc_T)
 *   t  - pointer to the type_T to search for in the function's generic type
 *        list
 *
 * Returns:
 *   The zero-based index of "t" in fp->uf_generic_param_types if found,
 *   or -1 if not found.
 */
    স্থির পূর্ণাঙ্ক
get_generic_type_index(ufunc_T *fp, type_T *t)
{
    বাবে (পূর্ণাঙ্ক i = 0; i < fp->uf_generic_argcount; i++)
    {
	যদি (&fp->uf_generic_param_types[i] == t)
	    ঘূৰাই i;
    }
    ঘূৰাই -1;
}

/*
 * Evaluates the type arguments for a generic function call and looks up the
 * corresponding concrete function.
 *
 * Arguments:
 *   ufunc - the original (possibly generic) function to evaluate
 *   name  - the function name (used for error messages and lookup)
 *   argp   - pointer to a pointer to the argument string; on entry, "*argp"
 *            should point to the character after the function name (possibly
 *            '<')
 *
 * Returns:
 *   The concrete function corresponding to the given type arguments,
 *   or NULL on error (with an error message reported).
 *
 * Behavior:
 *   - If "ufunc" is a generic function and "*argp" points to '<', attempts to
 *     find or instantiate the concrete function with the specified type
 *     arguments.  On success, advances "*argp" past the type argument list.
 *   - If "ufunc" is generic but "*argp" does not point to '<', reports a
 *     missing type argument error.
 *   - If "ufunc" is not generic but "*argp" points to '<', reports an error
 *     that the function is not generic.
 *   - Otherwise, returns the original function.
 */
    ufunc_T *
eval_generic_func(
    ufunc_T	*ufunc,
    char_u	*name,
    char_u	**argp)
{
    যদি (IS_GENERIC_FUNC(ufunc))
    {
	যদি (**argp == '<')
	    ufunc = find_generic_func(ufunc, name, argp);
	অন্যথা
	{
	    emsg_funcname(e_generic_func_missing_type_args_str, name);
	    ঘূৰাই রিক্ত;
	}
    }
    অন্যথা যদি (**argp == '<')
    {
	emsg_funcname(e_not_a_generic_function_str, name);
	ঘূৰাই রিক্ত;
    }

    ঘূৰাই ufunc;
}

/*
 * Checks if the string at "*argp" represents a generic function call with type
 * arguments, i.e., if it starts with a '<', contains a valid type argument
 * list, a closing '>', and is immediately followed by '('.
 *
 * On entry, "*argp" should point to the '<' character.
 * If the pattern matches, advances "*argp" to point to the '(' and returns
 * TRUE.  If not, leaves "*argp" unchanged and returns FALSE.
 *
 * Example:
 *   "<number, string>("
 */
    পূর্ণাঙ্ক
generic_func_call(char_u **argp)
{
    char_u	*p = *argp;

    যদি (*p != '<')
	ঘূৰাই FALSE;

    যদি (skip_generic_func_type_args(&p) == FAIL)
	ঘূৰাই FALSE;

    যদি (*p != '(')
	ঘূৰাই FALSE;

    *argp = p;
    ঘূৰাই TRUE;
}

/*
 * Recursively replaces all occurrences of the generic type "generic_type" in a
 * type structure with the corresponding concrete type from "new_ufunc", based
 * on the mapping from the original generic function "ufunc".
 *
 * This is used when instantiating a new function "new_ufunc" from a generic
 * function "ufunc" with specific type arguments. The function updates all
 * relevant type pointers in place, including nested types (such as lists,
 * dictionaries, and tuples).
 *
 * Arguments:
 *   ufunc         - the original generic function
 *   new_ufunc     - the new function being created with concrete types
 *   generic_type  - the generic type to be replaced (may be a nested type)
 *   specific_type - pointer to the location where the concrete type should be
 *		     set
 *   func_type     - pointer to the function type to update (may be NULL)
 */
    স্থির শূন্য
update_generic_type(
    ufunc_T	*ufunc,
    ufunc_T	*new_ufunc,
    type_T	*generic_type,
    type_T	**specific_type,
    type_T	**func_type)
{
    পূর্ণাঙ্ক	idx;

    ৱিকল্প (generic_type->tt_type)
    {
	পক্ষ VAR_ANY:
	    idx = get_generic_type_index(ufunc, generic_type);
	    যদি (idx != -1)
	    {
		*specific_type = new_ufunc->uf_generic_args[idx].gt_type;
		যদি (func_type != রিক্ত)
		    *func_type = new_ufunc->uf_generic_args[idx].gt_type;
	    }
	    ভঙ্গ;
	পক্ষ VAR_LIST:
	পক্ষ VAR_DICT:
	    update_generic_type(ufunc, new_ufunc, generic_type->tt_member,
		    &(*specific_type)->tt_member,
		    func_type != রিক্ত ? &(*func_type)->tt_member : রিক্ত);
	    ভঙ্গ;
	পক্ষ VAR_TUPLE:
	    বাবে (পূর্ণাঙ্ক i = 0; i < generic_type->tt_argcount; i++)
		update_generic_type(ufunc, new_ufunc,
			generic_type->tt_args[i],
			&(*specific_type)->tt_args[i],
			func_type != রিক্ত ? &(*func_type)->tt_args[i] : রিক্ত);
	    ভঙ্গ;
	পক্ষ VAR_FUNC:
	    বাবে (পূর্ণাঙ্ক i = 0; i < generic_type->tt_argcount; i++)
		update_generic_type(ufunc, new_ufunc,
			generic_type->tt_args[i],
			&(*specific_type)->tt_args[i],
			func_type != রিক্ত ? &(*func_type)->tt_args[i] : রিক্ত);
	    update_generic_type(ufunc, new_ufunc,
		    generic_type->tt_member,
		    &(*specific_type)->tt_member,
		    func_type != রিক্ত ? &(*func_type)->tt_member : রিক্ত);
	    ভঙ্গ;
	স্ৱভাৱ:
	    ভঙ্গ;
    }
}

/*
 * Make a copy of the argument types, the return type, the vararg type and the
 * function type of the generic function "fp" for "new_fp", a copy of "fp".
 * The type variables of "fp" in these types are replaced with the types of
 * the type variables of "new_fp", see update_generic_type().
 */
    শূন্য
generic_func_copy_types(ufunc_T *fp, ufunc_T *new_fp)
{
    পূর্ণাঙ্ক		i;

    // create a copy of
    // - all the argument types
    // - return type
    // - vararg type
    // - function type
    // if any generic type is used, it will be replaced below).
    যদি (fp->uf_arg_types != রিক্ত)
	বাবে (i = 0; i < fp->uf_args.ga_len; i++)
	    new_fp->uf_arg_types[i] = copy_type_deep(fp->uf_arg_types[i],
						&new_fp->uf_type_list);

    যদি (fp->uf_ret_type != রিক্ত)
	new_fp->uf_ret_type = copy_type_deep(fp->uf_ret_type,
						&new_fp->uf_type_list);

    যদি (fp->uf_va_type != রিক্ত)
	new_fp->uf_va_type = copy_type_deep(fp->uf_va_type,
						&new_fp->uf_type_list);

    যদি (fp->uf_func_type != রিক্ত)
	new_fp->uf_func_type = copy_type_deep(fp->uf_func_type,
						&new_fp->uf_type_list);

    type_T	*ft = new_fp->uf_func_type;

    // Update any generic types in the function arguments
    যদি (fp->uf_arg_types != রিক্ত)
	বাবে (i = 0; i < fp->uf_args.ga_len; i++)
	    update_generic_type(fp, new_fp, fp->uf_arg_types[i],
			    &new_fp->uf_arg_types[i],
			    ft != রিক্ত && ft->tt_args != রিক্ত
					&& ft->tt_argcount > i
						    ? &ft->tt_args[i] : রিক্ত);

    // Update the vararg type if it uses generic types.  It is the last
    // argument of the function type.
    যদি (fp->uf_va_type != রিক্ত)
    {
	পূর্ণাঙ্ক	va_idx = fp->uf_args.ga_len;

	update_generic_type(fp, new_fp, fp->uf_va_type, &new_fp->uf_va_type,
			    ft != রিক্ত && ft->tt_args != রিক্ত
					&& ft->tt_argcount > va_idx
						? &ft->tt_args[va_idx] : রিক্ত);
    }

    // Update the return type if it is a generic type
    যদি (fp->uf_ret_type != রিক্ত)
	update_generic_type(fp, new_fp, fp->uf_ret_type, &new_fp->uf_ret_type,
			    ft != রিক্ত ? &ft->tt_member : রিক্ত);
}

/*
 * Adds a new concrete instance of a generic function for a specific set of
 * type arguments.
 *
 * Arguments:
 *   fp       - the original generic function to instantiate
 *   key      - a string key representing the specific type arguments (used for
 *		lookup)
 *   gfatab   - generic function args table containing the parsed type
 *              arguments and their names
 *
 * Returns:
 *   Pointer to the new ufunc_T representing the instantiated function,
 *   or NULL if the function already exists or on allocation failure.
 *
 * This function:
 *   - Checks if a function with the given type arguments already exists.
 *   - Allocates and initializes a new function instance with the specific
 *     types.
 *   - Updates the function's name and expanded name to include the type
 *     arguments.
 *   - Copies and updates all relevant type information (argument types, return
 *     type, vararg type, function type), replacing generic types with the
 *     actual types.
 *   - Sets the new function's status to UF_TO_BE_COMPILED.
 *   - Registers the new function in the generic function's lookup table.
 */
    স্থির ufunc_T *
generic_func_add(ufunc_T *fp, char_u *চাবি, gfargs_tab_T *gfatab)
{
    hashtab_T	*ht = &fp->uf_generic_functab;
    long_u	hash;
    hashitem_T	*hi;
    পূর্ণাঙ্ক		i;

    hash = hash_hash(চাবি);
    hi = hash_lookup(ht, চাবি, hash);
    যদি (!HASHITEM_EMPTY(hi))
	ঘূৰাই রিক্ত;

    size_t	keylen = STRLEN(চাবি);
    gfitem_T    *gfitem = alloc(পরিমাণ(gfitem_T) + keylen);
    যদি (gfitem == রিক্ত)
	ঘূৰাই রিক্ত;

    STRCPY(gfitem->gfi_name, চাবি);

    ufunc_T *new_fp = copy_function(fp, (পূর্ণাঙ্ক)(keylen + 2));
    যদি (new_fp == রিক্ত)
    {
	vim_free(gfitem);
	ঘূৰাই রিক্ত;
    }

    new_fp->uf_generic_arg_types = gfatab->gfat_arg_types;
    // now that the type arguments is copied, remove the reference to the type
    // arguments
    ga_init(&gfatab->gfat_arg_types);

    যদি (fp->uf_class != রিক্ত)
	new_fp->uf_class = fp->uf_class;

    // Create a new name for the function: name<type1, type2...>
    new_fp->uf_name[new_fp->uf_namelen] =  '<';
    STRCPY(new_fp->uf_name + new_fp->uf_namelen + 1, চাবি);
    new_fp->uf_name[new_fp->uf_namelen + keylen + 1] =  '>';
    new_fp->uf_namelen += keylen + 2;

    যদি (new_fp->uf_name_exp != রিক্ত)
    {
	char_u	*new_name_exp = alloc(STRLEN(new_fp->uf_name_exp) + keylen + 3);
	যদি (new_name_exp != রিক্ত)
	{
	    STRCPY(new_name_exp, new_fp->uf_name_exp);
	    STRCAT(new_name_exp, "<");
	    STRCAT(new_name_exp, চাবি);
	    STRCAT(new_name_exp, ">");
	    vim_free(new_fp->uf_name_exp);
	    new_fp->uf_name_exp = new_name_exp;
	}
    }

    gfitem->gfi_ufunc = new_fp;
    gfitem->gfi_ufunc->uf_def_status = UF_TO_BE_COMPILED;

    // Replace the t_any generic types with the actual types
    বাবে (i = 0; i < fp->uf_generic_argcount; i++)
    {
	generic_T  *generic_arg;
	generic_arg = (generic_T *)gfatab->gfat_args.ga_data + i;
	generic_T *gt = &new_fp->uf_generic_args[i];
	gt->gt_type = generic_arg->gt_type;
    }

    generic_func_copy_types(fp, new_fp);

    hash_add_item(ht, hi, gfitem->gfi_name, hash);

    ঘূৰাই new_fp;
}

/*
 * Looks up a concrete instance of a generic function "fp" using the type
 * arguments specified in "gfatab".
 *
 * The lookup key is constructed by concatenating the type argument names from
 * "gfatab", separated by ", ", and stored in the provided growarray
 * "gfkey_gap".  The contents of "gfkey_gap" will be overwritten.
 *
 * Arguments:
 *   fp        - the generic function to search in
 *   gfatab    - generic function args table containing the parsed type
 *               arguments and their names
 *   gfkey_gap - growarray used to build and store the lookup key string
 *
 * Returns:
 *   Pointer to the ufunc_T representing the concrete function if found, or
 *   NULL if no matching function exists.
 */
    স্থির ufunc_T *
generic_lookup_func(ufunc_T *fp, gfargs_tab_T *gfatab, garray_T *gfkey_gap)
{
    hashtab_T	*ht = &fp->uf_generic_functab;
    hashitem_T	*hi;

    বাবে (পূর্ণাঙ্ক i = 0; i < gfatab->gfat_args.ga_len; i++)
    {
	generic_T  *generic_arg;

	generic_arg = (generic_T *)gfatab->gfat_args.ga_data + i;
	ga_concat(gfkey_gap, generic_arg->gt_name);

	যদি (i != gfatab->gfat_args.ga_len - 1)
	{
	    ga_append(gfkey_gap, ',');
	    ga_append(gfkey_gap, ' ');
	}
    }
    ga_append(gfkey_gap, NUL);

    char_u	*চাবি = ((char_u *)gfkey_gap->ga_data);

    hi = hash_find(ht, চাবি);

    যদি (HASHITEM_EMPTY(hi))
	ঘূৰাই রিক্ত;

    gfitem_T	*gfitem = HI2GFITEM(hi);
    ঘূৰাই gfitem->gfi_ufunc;
}

/*
 * Returns a concrete instance of the generic function "fp" using the type
 * arguments specified in "gfatab". If such an instance does not exist,
 * it is created and registered.
 *
 * Arguments:
 *   fp        - the generic function to instantiate
 *   gfatab    - generic function args table containing the parsed type
 *               arguments and their names
 *
 * Returns:
 *   Pointer to the ufunc_T representing the concrete function instance,
 *   or NULL if the type arguments are invalid or on allocation failure.
 *
 * Behavior:
 *   - If "fp" is not a generic function and no type arguments are given,
 *     returns "fp" as-is.
 *   - If "fp" is not generic but type arguments are given, reports an error
 *     and returns NULL.
 *   - Validates the number of type arguments, reporting errors for missing,
 *     too few, or too many.
 *   - Looks up an existing function instance with the given types.
 *   - If not found, creates and registers a new function instance.
 */
    ufunc_T *
generic_func_get(ufunc_T *fp, gfargs_tab_T *gfatab)
{
    ৱর্ণ	*emsg = রিক্ত;

    যদি (!IS_GENERIC_FUNC(fp))
    {
	যদি (gfatab && generic_func_args_table_size(gfatab) > 0)
	{
	    emsg_funcname(e_not_a_generic_function_str, fp->uf_name);
	    ঘূৰাই রিক্ত;
	}
	ঘূৰাই fp;
    }

    যদি (gfatab == রিক্ত || gfatab->gfat_args.ga_len == 0)
	emsg = e_generic_func_missing_type_args_str;
    অন্যথা যদি (gfatab->gfat_args.ga_len < fp->uf_generic_argcount)
	emsg = e_not_enough_types_for_generic_function_str;
    অন্যথা যদি (gfatab->gfat_args.ga_len > fp->uf_generic_argcount)
	emsg = e_too_many_types_for_generic_function_str;

    যদি (emsg != রিক্ত)
    {
	emsg_funcname(emsg, printable_func_name(fp));
	ঘূৰাই রিক্ত;
    }

    // generic function call
    garray_T gfkey_ga;

    ga_init2(&gfkey_ga, 1, 80);

    // Look up the function with specific types
    ufunc_T	*generic_fp = generic_lookup_func(fp, gfatab, &gfkey_ga);
    যদি (generic_fp == রিক্ত)
	// generic function with these type arguments doesn't exist.
	// Create a new one.
	generic_fp = generic_func_add(fp, (char_u *)gfkey_ga.ga_data, gfatab);
    ga_clear(&gfkey_ga);

    ঘূৰাই generic_fp;
}

/*
 * Returns the concrete function of the generic function "fp" for the type
 * arguments of "inst", a concrete function of another generic function with
 * the same number of type variables.  E.g. "inst" is the interface method
 * "Get<string>" and "fp" the generic method "Get" of the object class.  It is
 * created when it does not exist yet.
 * Returns NULL on failure, an error was given then (except for memory
 * allocation failure).
 */
    ufunc_T *
generic_func_get_same_types(ufunc_T *fp, ufunc_T *inst)
{
    gfargs_tab_T	gfatab;
    ufunc_T		*new_fp = রিক্ত;
    পূর্ণাঙ্ক			argcount = inst->uf_generic_argcount;
    পূর্ণাঙ্ক			i;

    generic_func_args_table_init(&gfatab);

    যদি (ga_grow(&gfatab.gfat_args, argcount) == FAIL)
	গচ্ছ done;
    বাবে (i = 0; i < argcount; i++)
    {
	generic_T	*generic_arg = (generic_T *)gfatab.gfat_args.ga_data + i;
	type_T		*type = inst->uf_generic_args[i].gt_type;
	ৱর্ণ		*tofree = রিক্ত;

	generic_arg->gt_name = vim_strsave(
				    (char_u *)type_name(type, &tofree));
	vim_free(tofree);
	যদি (generic_arg->gt_name == রিক্ত)
	    গচ্ছ done;
	// The types of "inst" are used for the lookup, see below.
	generic_arg->gt_type = type;
	gfatab.gfat_args.ga_len++;
    }

    // Usually the function exists, look it up first to avoid copying the
    // types.
    garray_T	gfkey_ga;

    ga_init2(&gfkey_ga, 1, 80);
    new_fp = generic_lookup_func(fp, &gfatab, &gfkey_ga);
    ga_clear(&gfkey_ga);
    যদি (new_fp != রিক্ত)
	গচ্ছ done;

    // The new function owns the types, use a copy of the types of "inst".
    বাবে (i = 0; i < argcount; i++)
    {
	generic_T   *generic_arg = (generic_T *)gfatab.gfat_args.ga_data + i;
	type_T	    *type = copy_type_deep(generic_arg->gt_type,
							&gfatab.gfat_arg_types);

	// out of memory when the type is returned, it must not be used
	যদি (type == generic_arg->gt_type)
	    গচ্ছ done;
	generic_arg->gt_type = type;
    }
    new_fp = generic_func_get(fp, &gfatab);

done:
    generic_func_args_table_clear(&gfatab);
    ঘূৰাই new_fp;
}

/*
 * Looks up or creates a concrete instance of a generic function "ufunc" using
 * the type arguments specified after the function name in "name".
 *
 * On entry, "name" points to the function name, and "*argp" points to the
 * opening '<' of the type argument list (i.e., name + namelen).
 *
 * Arguments:
 *   ufunc - the generic function to instantiate or look up
 *   name  - the function name, followed by the type argument list
 *   argp  - pointer to a pointer to the type argument list (should point to
 *           '<'); on success, advanced to the character after the closing '>'
 *
 * Returns:
 *   Pointer to the ufunc_T representing the concrete function instance if
 *   successful, or NULL if parsing fails or the instance cannot be created.
 *
 * This function:
 *   - Parses the type arguments from the string after the function name.
 *   - Looks up an existing function instance with those type arguments.
 *   - If not found, creates and registers a new function instance.
 *   - Advances "*argp" to after the type argument list on success.
 */
    ufunc_T *
find_generic_func(ufunc_T *ufunc, char_u *name, char_u **argp)
{
    gfargs_tab_T    gfatab;
    char_u	*p;
    ufunc_T	*new_ufunc = রিক্ত;

    generic_func_args_table_init(&gfatab);

    // Get the list of types following the name
    p = parse_generic_func_type_args(name, *argp - name, *argp, &gfatab, রিক্ত);
    যদি (p != রিক্ত)
    {
	new_ufunc = generic_func_get(ufunc, &gfatab);
	*argp = p;
    }

    generic_func_args_table_clear(&gfatab);

    ঘূৰাই new_ufunc;
}

/*
 * Searches for a generic type with the given name "gt_name" in the generic
 * function "ufunc".
 *
 * Arguments:
 *   gt_name - the name of the generic type to search for
 *   ufunc   - the generic function in which to search for the type
 *
 * Returns:
 *   Pointer to the type_T representing the found generic type,
 *   or NULL if the type is not found or if "ufunc" is not a generic function.
 */
    স্থির type_T *
find_generic_type_in_ufunc(char_u *gt_name, size_t name_len, ufunc_T *ufunc)
{
    যদি (!IS_GENERIC_FUNC(ufunc))
	ঘূৰাই রিক্ত;

    বাবে (পূর্ণাঙ্ক i = 0; i < ufunc->uf_generic_argcount; i++)
    {
	generic_T *generic;

	generic = ((generic_T *)ufunc->uf_generic_args) + i;
	যদি (STRNCMP(generic->gt_name, gt_name, name_len) == 0
				&& generic->gt_name[name_len] == NUL)
	{
	    type_T *type = generic->gt_type;
	    ঘূৰাই type;
	}
    }

    ঘূৰাই রিক্ত;
}

/*
 * Searches for a generic type with the given name "gt_name" in the current
 * function context "cctx" and its outer (enclosing) contexts, if necessary.
 *
 * Arguments:
 *   gt_name - the name of the generic type to search for
 *   cctx    - the current compile context, which may be nested
 *
 * Returns:
 *   Pointer to the type_T representing the found generic type,
 *   or NULL if the type is not found in the current or any outer context.
 */
    স্থির type_T *
find_generic_type_in_cctx(char_u *gt_name, size_t name_len, cctx_T *cctx)
{
    type_T	*type;

    type = find_generic_type_in_ufunc(gt_name, name_len, cctx->ctx_ufunc);
    যদি (type != রিক্ত)
	ঘূৰাই type;

    যদি (cctx->ctx_outer != রিক্ত)
	ঘূৰাই find_generic_type_in_cctx(gt_name, name_len, cctx->ctx_outer);

    ঘূৰাই রিক্ত;
}

/*
 * Looks up a generic type with the given name "gt_name" in the generic
 * function "ufunc".  If not found, searches in the enclosing compile context
 * "cctx" (for nested functions).
 *
 * Arguments:
 *   gt_name - the name of the generic type to search for
 *   ufunc   - the generic function to search in first (may be NULL)
 *   cctx    - the compile context to search in outer functions if not found
 *             in "ufunc" (may be NULL)
 *
 * Returns:
 *   Pointer to the type_T representing the found generic type, or NULL if the
 *   type is not found in the given function or any outer context.
 */
    type_T *
find_generic_type(
    char_u	*gt_name,
    size_t	name_len,
    ufunc_T	*ufunc,
    cctx_T	*cctx)
{
    যদি (ufunc != রিক্ত)
    {
	type_T *type = find_generic_type_in_ufunc(gt_name, name_len, ufunc);
	যদি (type != রিক্ত)
	    ঘূৰাই type;
    }

    // Also when "ufunc" is the compiled function: a lambda can use the type
    // variables of the function it is defined in.
    যদি (cctx != রিক্ত)
	ঘূৰাই find_generic_type_in_cctx(gt_name, name_len, cctx);

    ঘূৰাই রিক্ত;
}

/*
 * Frees all concrete function instances stored in the generic function table
 * of "fp". This includes freeing each instantiated function and its
 * associated gfitem_T structure, and clearing the hash table.
 *
 * Arguments:
 *   fp - the generic function whose function table should be freed
 */
    স্থির শূন্য
free_generic_functab(ufunc_T *fp)
{
    hashtab_T	*ht = &fp->uf_generic_functab;
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    gfitem_T    *gfitem = HI2GFITEM(hi);

	    func_clear_free(gfitem->gfi_ufunc, FALSE);
	    vim_free(gfitem);
	    --todo;
	}
    }
    hash_clear(ht);
}

/*
 * Frees all memory and state associated with a generic function "fp".
 * This includes the generic type list, generic argument list, and all
 * concrete function instances in the generic function table.
 *
 * Arguments:
 *   fp - the generic function to clear
 */
    শূন্য
generic_func_clear_বস্তুসমূহ(ufunc_T *fp)
{
    VIM_CLEAR(fp->uf_generic_param_types);
    clear_type_list(&fp->uf_generic_arg_types);
    বাবে (পূর্ণাঙ্ক i = 0; i < fp->uf_generic_argcount; i++)
	VIM_CLEAR(fp->uf_generic_args[i].gt_name);
    VIM_CLEAR(fp->uf_generic_args);
    free_generic_functab(fp);
    fp->uf_flags &= ~FC_GENERIC;
}

#endif // FEAT_EVAL
