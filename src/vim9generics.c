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

#include "vim.h"

#ᱡᱩᱫᱤ defined(FEAT_EVAL)


/*
 * A hash table is used to lookup a generic function with specific types.
 * The specific type names are used as the key.
 */
typedef sa_nracanaa gfitem_S gfitem_T;
sa_nracanaa gfitem_S
{
    ufunc_T	*gfi_ufunc;
    char_u	gfi_name[1];	// actually longer
};
#define GFITEM_KEY_OFF	offsetof(gfitem_T, gfi_name)
#define HI2GFITEM(hi)	((gfitem_T *)((hi)->hi_key - GFITEM_KEY_OFF))

static type_T *find_generic_type_in_cctx(char_u *gt_name, size_t len, cctx_T *cctx);

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

    ᱡᱩᱫᱤ (name[0] == '<')
    {
	// Skip the <SNR> or <lambda> at the start of the name
	ᱡᱩᱫᱤ (STRNCMP(name + 1, "SNR>", 4) == 0)
	    p += 5;
	ᱵᱟᱝ ᱡᱩᱫᱤ (STRNCMP(name + 1, "lambda>", 7) == 0)
	    p += 8;
    }

    ᱡᱚᱠᱷᱚᱱ (*p && *p != '(' && *p != '<')
	p++;

    ᱡᱩᱫᱤ (*p == '<')
	ᱨᱩᱣᱟᱹᱲ p;

    ᱨᱩᱣᱟᱹᱲ NULL;
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
    static char_u *
generic_func_find_close_bracket(char_u *start)
{
    char_u	*p = start + 1;
    puur_naa_nka		type_count = 0;

    ᱡᱚᱠᱷᱚᱱ (*p && *p != '>')
    {
	char_u	*typename = p;

	ᱡᱩᱫᱤ (VIM_ISWHITE(*p))
	{
	    var_na tmpstr[2];
	    tmpstr[0] = *(p - 1); tmpstr[1] = NUL;
	    semsg(_(e_no_white_space_allowed_after_str_str), tmpstr, start);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}

	p = skip_type(p, FALSE);
	ᱡᱩᱫᱤ (p == typename)
	{
	    char_u cc = *p;
	    *p = NUL;
	    semsg(_(e_missing_type_after_str), start);
	    *p = cc;
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}
	type_count++;

	ᱡᱩᱫᱤ (*p == '>' || *p == NUL)
	    bha_nga;

	ᱡᱩᱫᱤ (VIM_ISWHITE(*p))
	{
	    char_u cc = *p;
	    *p = NUL;
	    semsg(_(e_no_white_space_allowed_after_str_str), typename, start);
	    *p = cc;
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}

	ᱡᱩᱫᱤ (*p != ',')
	{
	    semsg(_(e_missing_comma_in_generic_function_str), start);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}
	p++;

	ᱡᱩᱫᱤ (*p == NUL)
	    bha_nga;

	ᱡᱩᱫᱤ (!VIM_ISWHITE(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", start);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}
	p = skipwhite(p);
    }

    ᱡᱩᱫᱤ (*p != '>')
    {
	semsg(_(e_missing_closing_angle_bracket_in_generic_function_str), start);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    ᱡᱩᱫᱤ (VIM_ISWHITE(*(p + 1)) && *skipwhite(p + 1) == '(')
    {
	// white space not allowed between '>' and '('
	semsg(_(e_no_white_space_allowed_after_str_str), ">", start);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }


    ᱡᱩᱫᱤ (type_count == 0)
    {
	semsg(_(e_empty_type_list_for_generic_function_str), start);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    ᱨᱩᱣᱟᱹᱲ p;
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
    puur_naa_nka
skip_generic_func_type_args(char_u **argp)
{
    char_u *p = generic_func_find_close_bracket(*argp);
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ FAIL;

    *argp = p + 1;	// skip '>'

    ᱨᱩᱣᱟᱹᱲ OK;
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

    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    // The type arguments can be of any length, do not use IObuff.
    argslen = (size_t)(p - *argp) + 1;
    name = alloc(namelen + argslen + 1);
    ᱡᱩᱫᱤ (name == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;
    mch_memmove(name, funcname, namelen);
    mch_memmove(name + namelen, *argp, argslen);
    name[namelen + argslen] = NUL;

    *argp = p + 1;

    ᱨᱩᱣᱟᱹᱲ name;
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
    ᱡᱩᱫᱤ (!IS_GENERIC_FUNC(fp))
    {
	emsg_funcname(e_not_a_generic_function_str, fp->uf_name);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    ᱨᱩᱣᱟᱹᱲ append_generic_func_type_args(fp->uf_name, fp->uf_namelen, argp);
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
    ᱡᱩᱫᱤ (VIM_ISWHITE(*(p + 1)))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), "<", p);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    ++p;	// skip the '<'

    // parse each type argument until '>' or end of string
    ᱡᱚᱠᱷᱚᱱ (*p && *p != '>')
    {
	p = skipwhite(p);

	ᱡᱩᱫᱤ (!ASCII_ISALNUM(*p))
	{
	    semsg(_(e_missing_type_after_str), start);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}

	// parse the type
	type_arg = parse_type(&p, &gfatab->gfat_arg_types, NULL, cctx, TRUE);
	ᱡᱩᱫᱤ (type_arg == NULL || !valid_declaration_type(type_arg))
	    ᱨᱩᱣᱟᱹᱲ NULL;

	var_na	*ret_free = NULL;
	var_na	*ret_name = type_name(type_arg, &ret_free);

	// create space for the name and the new type
	ᱡᱩᱫᱤ (ga_grow(&gfatab->gfat_args, 1) == FAIL)
	{
	    vim_free(ret_free);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}
	generic_arg = (generic_T *)gfatab->gfat_args.ga_data +
						gfatab->gfat_args.ga_len;
	gfatab->gfat_args.ga_len++;

	// copy the type name
	generic_arg->gt_name = alloc(STRLEN(ret_name) + 1);
	ᱡᱩᱫᱤ (generic_arg->gt_name == NULL)
	    ᱨᱩᱣᱟᱹᱲ NULL;
	STRCPY(generic_arg->gt_name, ret_name);
	vim_free(ret_free);

	// add the new type
	generic_arg->gt_type = type_arg;

	p = skipwhite(p);

	ᱡᱩᱫᱤ (*p == NUL || *p == '>')
	    bha_nga;

	// after a type, expect ',' or '>'
	ᱡᱩᱫᱤ (*p != ',')
	{
	    semsg(_(e_missing_comma_in_generic_function_str), start);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}

	ᱡᱩᱫᱤ (*(p + 1) == NUL)
	    bha_nga;

	// Require whitespace after a comma and skip it
	ᱡᱩᱫᱤ (!VIM_ISWHITE(*(p + 1)))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}
	p++;
    }

    // ensure the list of types ends in a closing '>'
    ᱡᱩᱫᱤ (*p != '>')
    {
	semsg(_(e_missing_closing_angle_bracket_in_generic_function_str),
		func_name);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    // no whitespace allowed before '>'
    ᱡᱩᱫᱤ (VIM_ISWHITE(*(p - 1)))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), ">", p);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    // at least one type argument is required
    ᱡᱩᱫᱤ (generic_func_args_table_size(gfatab) == 0)
    {
	char_u	cc = func_name[namelen];
	func_name[namelen] = NUL;
	semsg(_(e_empty_type_list_for_generic_function_str), func_name);
	func_name[namelen] = cc;
	ᱨᱩᱣᱟᱹᱲ NULL;
    }
    ++p;	// skip the '>'

    ᱨᱩᱣᱟᱹᱲ p;
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
    static puur_naa_nka
generic_name_exists(
    char_u		*gt_name,
    size_t		name_len,
    gfargs_tab_T	*gfatab,
    cctx_T		*cctx)
{
    typval_T	tv;

    tv.v_type = VAR_UNKNOWN;

    ᱡᱩᱫᱤ (eval_variable_import(gt_name, &tv) == OK)
    {
	semsg(_(e_redefining_script_item_str), gt_name);
	clear_tv(&tv);
	ᱨᱩᱣᱟᱹᱲ TRUE;
    }

    for (puur_naa_nka i = 0; i < gfatab->gfat_args.ga_len; i++)
    {
	generic_T *generic = &((generic_T *)gfatab->gfat_args.ga_data)[i];

	ᱡᱩᱫᱤ (STRNCMP(gt_name, generic->gt_name, name_len) == 0
				&& generic->gt_name[name_len] == NUL)
	{
	    semsg(_(e_duplicate_type_var_name_str), gt_name);
	    ᱨᱩᱣᱟᱹᱲ TRUE;
	}
    }

    ᱡᱩᱫᱤ (cctx != NULL &&
	    find_generic_type_in_cctx(gt_name, name_len, cctx) != NULL)
    {
	semsg(_(e_duplicate_type_var_name_str), gt_name);
	ᱨᱩᱣᱟᱹᱲ TRUE;
    }

    ᱨᱩᱣᱟᱹᱲ FALSE;
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
    ᱡᱩᱫᱤ (VIM_ISWHITE(*(p - 1)))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "<", p);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    ᱡᱩᱫᱤ (VIM_ISWHITE(*(p + 1)))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), "<", p);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    // Find the start of the function name before the '<'.  It is always
    // preceded by "def" and white space, or "g:".
    char_u	    *fname_start = p;
    ᱡᱚᱠᱷᱚᱱ (ASCII_ISALNUM(fname_start[-1]) || fname_start[-1] == '_')
	--fname_start;
    size_t	    fname_len = p - fname_start;

    char_u	    *start = ++p;

    ᱡᱚᱠᱷᱚᱱ (*p && *p != '>')
    {
	p = skipwhite(p);

	ᱡᱩᱫᱤ (*p == NUL || *p == '>')
	{
	    semsg(_(e_missing_type_after_str), p - 1);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}

	ᱡᱩᱫᱤ (!ASCII_ISUPPER(*p))
	{
	    ᱡᱩᱫᱤ (ASCII_ISLOWER(*p))
		semsg(_(e_type_var_name_must_start_with_uppercase_letter_str), p);
	    ᱵᱟᱝ
		semsg(_(e_missing_type_after_str), p - 1);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}

	char_u	*name_start = p;
	char_u	*name_end = NULL;
	char_u	cc;
	size_t	name_len = 0;

	p++;
	ᱡᱚᱠᱷᱚᱱ (ASCII_ISALNUM(*p) || *p == '_')
	    p++;
	name_end = p;

	name_len = name_end - name_start;
	cc = *name_end;
	*name_end = NUL;

	// The type variable cannot have the name of the function being
	// defined.  Use the name as it appears before the '<', a nested
	// function internally has a "<lambda>N" name.
	ᱡᱩᱫᱤ (fname_len == name_len
			&& STRNCMP(fname_start, name_start, name_len) == 0)
	{
	    semsg(_(e_redefining_script_item_str), name_start);
	    *name_end = cc;
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}

	puur_naa_nka name_exists = generic_name_exists(name_start, name_len, gfatab,
									cctx);
	*name_end = cc;
	ᱡᱩᱫᱤ (name_exists)
	    ᱨᱩᱣᱟᱹᱲ NULL;

	ᱡᱩᱫᱤ (ga_grow(&gfatab->gfat_args, 1) == FAIL)
	    ᱨᱩᱣᱟᱹᱲ NULL;
	generic_T *generic =
	    &((generic_T *)gfatab->gfat_args.ga_data)[gfatab->gfat_args.ga_len];
	gfatab->gfat_args.ga_len++;

	generic->gt_name = alloc(name_len + 1);
	ᱡᱩᱫᱤ (generic->gt_name == NULL)
	    ᱨᱩᱣᱟᱹᱲ NULL;
	vim_strncpy(generic->gt_name, name_start, name_len);
	generic->gt_type = NULL;

	ᱡᱩᱫᱤ (VIM_ISWHITE(*p))
	{
	    semsg(_(e_no_white_space_allowed_after_str_str), generic->gt_name,
		    name_start);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}

	ᱡᱩᱫᱤ (*p != ',' && *p != '>')
	{
	    semsg(_(e_missing_comma_in_generic_function_str), start);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}
	ᱡᱩᱫᱤ (*p == ',')
	{
	    ᱡᱩᱫᱤ (!VIM_ISWHITE(*(p + 1)))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p);
		ᱨᱩᱣᱟᱹᱲ NULL;
	    }
	    p++;
	}
    }
    ᱡᱩᱫᱤ (*p != '>')
	ᱨᱩᱣᱟᱹᱲ NULL;
    p++;

    puur_naa_nka gfat_sz = generic_func_args_table_size(gfatab);

    ᱡᱩᱫᱤ (gfat_sz == 0)
    {
	emsg_funcname(e_empty_type_list_for_generic_function_str, func_name);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    // set the generic params to VAR_ANY type
    ᱡᱩᱫᱤ (ga_grow(&gfatab->gfat_param_types, gfat_sz) == FAIL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    gfatab->gfat_param_types.ga_len = gfat_sz;
    for (puur_naa_nka i = 0; i < generic_func_args_table_size(gfatab); i++)
    {
	type_T *gt = &((type_T *)gfatab->gfat_param_types.ga_data)[i];

	CLEAR_POINTER(gt);
	gt->tt_type = VAR_ANY;
	gt->tt_flags = TTFLAG_GENERIC;

	generic_T *generic = &((generic_T *)gfatab->gfat_args.ga_data)[i];
	generic->gt_type = gt;
    }

    ᱨᱩᱣᱟᱹᱲ p;
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
    void
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
    void
generic_func_args_table_init(gfargs_tab_T *gfatab)
{
    ga_init2(&gfatab->gfat_args, parimaa_na(generic_T), 10);
    ga_init2(&gfatab->gfat_param_types, parimaa_na(type_T), 10);
    ga_init2(&gfatab->gfat_arg_types, parimaa_na(type_T), 10);
}

/*
 * Return the number of entries in the generic function args table
 */
    puur_naa_nka
generic_func_args_table_size(gfargs_tab_T *gfatab)
{
    ᱨᱩᱣᱟᱹᱲ gfatab->gfat_args.ga_len;
}

/*
 * Free all the generic function args table items
 */
    void
generic_func_args_table_clear(gfargs_tab_T *gfatab)
{
    clear_type_list(&gfatab->gfat_param_types);
    clear_type_list(&gfatab->gfat_arg_types);
    for (puur_naa_nka i = 0; i < gfatab->gfat_args.ga_len; i++)
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
    void
copy_generic_function(ufunc_T *fp, ufunc_T *new_fp)
{
    puur_naa_nka		i;
    puur_naa_nka		sz;

    ᱡᱩᱫᱤ (!IS_GENERIC_FUNC(fp))
	ᱨᱩᱣᱟᱹᱲ;

    sz = fp->uf_generic_argcount * parimaa_na(type_T);
    new_fp->uf_generic_param_types = alloc_clear(sz);
    ᱡᱩᱫᱤ (new_fp->uf_generic_param_types == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    memcpy(new_fp->uf_generic_param_types, fp->uf_generic_param_types, sz);

    sz = fp->uf_generic_argcount * parimaa_na(generic_T);
    new_fp->uf_generic_args = alloc_clear(sz);
    ᱡᱩᱫᱤ (new_fp->uf_generic_args == NULL)
    {
	VIM_CLEAR(new_fp->uf_generic_param_types);
	ᱨᱩᱣᱟᱹᱲ;
    }
    memcpy(new_fp->uf_generic_args, fp->uf_generic_args, sz);

    for (i = 0; i < fp->uf_generic_argcount; i++)
	new_fp->uf_generic_args[i].gt_name =
	    vim_strsave(fp->uf_generic_args[i].gt_name);

    for (i = 0; i < fp->uf_generic_argcount; i++)
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
    static puur_naa_nka
get_generic_type_index(ufunc_T *fp, type_T *t)
{
    for (puur_naa_nka i = 0; i < fp->uf_generic_argcount; i++)
    {
	ᱡᱩᱫᱤ (&fp->uf_generic_param_types[i] == t)
	    ᱨᱩᱣᱟᱹᱲ i;
    }
    ᱨᱩᱣᱟᱹᱲ -1;
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
    ᱡᱩᱫᱤ (IS_GENERIC_FUNC(ufunc))
    {
	ᱡᱩᱫᱤ (**argp == '<')
	    ufunc = find_generic_func(ufunc, name, argp);
	ᱵᱟᱝ
	{
	    emsg_funcname(e_generic_func_missing_type_args_str, name);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (**argp == '<')
    {
	emsg_funcname(e_not_a_generic_function_str, name);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    ᱨᱩᱣᱟᱹᱲ ufunc;
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
    puur_naa_nka
generic_func_call(char_u **argp)
{
    char_u	*p = *argp;

    ᱡᱩᱫᱤ (*p != '<')
	ᱨᱩᱣᱟᱹᱲ FALSE;

    ᱡᱩᱫᱤ (skip_generic_func_type_args(&p) == FAIL)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    ᱡᱩᱫᱤ (*p != '(')
	ᱨᱩᱣᱟᱹᱲ FALSE;

    *argp = p;
    ᱨᱩᱣᱟᱹᱲ TRUE;
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
    static void
update_generic_type(
    ufunc_T	*ufunc,
    ufunc_T	*new_ufunc,
    type_T	*generic_type,
    type_T	**specific_type,
    type_T	**func_type)
{
    puur_naa_nka	idx;

    switch (generic_type->tt_type)
    {
	pak_sa VAR_ANY:
	    idx = get_generic_type_index(ufunc, generic_type);
	    ᱡᱩᱫᱤ (idx != -1)
	    {
		*specific_type = new_ufunc->uf_generic_args[idx].gt_type;
		ᱡᱩᱫᱤ (func_type != NULL)
		    *func_type = new_ufunc->uf_generic_args[idx].gt_type;
	    }
	    bha_nga;
	pak_sa VAR_LIST:
	pak_sa VAR_DICT:
	    update_generic_type(ufunc, new_ufunc, generic_type->tt_member,
		    &(*specific_type)->tt_member,
		    func_type != NULL ? &(*func_type)->tt_member : NULL);
	    bha_nga;
	pak_sa VAR_TUPLE:
	    for (puur_naa_nka i = 0; i < generic_type->tt_argcount; i++)
		update_generic_type(ufunc, new_ufunc,
			generic_type->tt_args[i],
			&(*specific_type)->tt_args[i],
			func_type != NULL ? &(*func_type)->tt_args[i] : NULL);
	    bha_nga;
	pak_sa VAR_FUNC:
	    for (puur_naa_nka i = 0; i < generic_type->tt_argcount; i++)
		update_generic_type(ufunc, new_ufunc,
			generic_type->tt_args[i],
			&(*specific_type)->tt_args[i],
			func_type != NULL ? &(*func_type)->tt_args[i] : NULL);
	    update_generic_type(ufunc, new_ufunc,
		    generic_type->tt_member,
		    &(*specific_type)->tt_member,
		    func_type != NULL ? &(*func_type)->tt_member : NULL);
	    bha_nga;
	default:
	    bha_nga;
    }
}

/*
 * Make a copy of the argument types, the return type, the vararg type and the
 * function type of the generic function "fp" for "new_fp", a copy of "fp".
 * The type variables of "fp" in these types are replaced with the types of
 * the type variables of "new_fp", see update_generic_type().
 */
    void
generic_func_copy_types(ufunc_T *fp, ufunc_T *new_fp)
{
    puur_naa_nka		i;

    // create a copy of
    // - all the argument types
    // - return type
    // - vararg type
    // - function type
    // if any generic type is used, it will be replaced below).
    ᱡᱩᱫᱤ (fp->uf_arg_types != NULL)
	for (i = 0; i < fp->uf_args.ga_len; i++)
	    new_fp->uf_arg_types[i] = copy_type_deep(fp->uf_arg_types[i],
						&new_fp->uf_type_list);

    ᱡᱩᱫᱤ (fp->uf_ret_type != NULL)
	new_fp->uf_ret_type = copy_type_deep(fp->uf_ret_type,
						&new_fp->uf_type_list);

    ᱡᱩᱫᱤ (fp->uf_va_type != NULL)
	new_fp->uf_va_type = copy_type_deep(fp->uf_va_type,
						&new_fp->uf_type_list);

    ᱡᱩᱫᱤ (fp->uf_func_type != NULL)
	new_fp->uf_func_type = copy_type_deep(fp->uf_func_type,
						&new_fp->uf_type_list);

    type_T	*ft = new_fp->uf_func_type;

    // Update any generic types in the function arguments
    ᱡᱩᱫᱤ (fp->uf_arg_types != NULL)
	for (i = 0; i < fp->uf_args.ga_len; i++)
	    update_generic_type(fp, new_fp, fp->uf_arg_types[i],
			    &new_fp->uf_arg_types[i],
			    ft != NULL && ft->tt_args != NULL
					&& ft->tt_argcount > i
						    ? &ft->tt_args[i] : NULL);

    // Update the vararg type if it uses generic types.  It is the last
    // argument of the function type.
    ᱡᱩᱫᱤ (fp->uf_va_type != NULL)
    {
	puur_naa_nka	va_idx = fp->uf_args.ga_len;

	update_generic_type(fp, new_fp, fp->uf_va_type, &new_fp->uf_va_type,
			    ft != NULL && ft->tt_args != NULL
					&& ft->tt_argcount > va_idx
						? &ft->tt_args[va_idx] : NULL);
    }

    // Update the return type if it is a generic type
    ᱡᱩᱫᱤ (fp->uf_ret_type != NULL)
	update_generic_type(fp, new_fp, fp->uf_ret_type, &new_fp->uf_ret_type,
			    ft != NULL ? &ft->tt_member : NULL);
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
    static ufunc_T *
generic_func_add(ufunc_T *fp, char_u *key, gfargs_tab_T *gfatab)
{
    hashtab_T	*ht = &fp->uf_generic_functab;
    long_u	hash;
    hashitem_T	*hi;
    puur_naa_nka		i;

    hash = hash_hash(key);
    hi = hash_lookup(ht, key, hash);
    ᱡᱩᱫᱤ (!HASHITEM_EMPTY(hi))
	ᱨᱩᱣᱟᱹᱲ NULL;

    size_t	keylen = STRLEN(key);
    gfitem_T    *gfitem = alloc(parimaa_na(gfitem_T) + keylen);
    ᱡᱩᱫᱤ (gfitem == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    STRCPY(gfitem->gfi_name, key);

    ufunc_T *new_fp = copy_function(fp, (puur_naa_nka)(keylen + 2));
    ᱡᱩᱫᱤ (new_fp == NULL)
    {
	vim_free(gfitem);
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    new_fp->uf_generic_arg_types = gfatab->gfat_arg_types;
    // now that the type arguments is copied, remove the reference to the type
    // arguments
    ga_init(&gfatab->gfat_arg_types);

    ᱡᱩᱫᱤ (fp->uf_class != NULL)
	new_fp->uf_class = fp->uf_class;

    // Create a new name for the function: name<type1, type2...>
    new_fp->uf_name[new_fp->uf_namelen] =  '<';
    STRCPY(new_fp->uf_name + new_fp->uf_namelen + 1, key);
    new_fp->uf_name[new_fp->uf_namelen + keylen + 1] =  '>';
    new_fp->uf_namelen += keylen + 2;

    ᱡᱩᱫᱤ (new_fp->uf_name_exp != NULL)
    {
	char_u	*new_name_exp = alloc(STRLEN(new_fp->uf_name_exp) + keylen + 3);
	ᱡᱩᱫᱤ (new_name_exp != NULL)
	{
	    STRCPY(new_name_exp, new_fp->uf_name_exp);
	    STRCAT(new_name_exp, "<");
	    STRCAT(new_name_exp, key);
	    STRCAT(new_name_exp, ">");
	    vim_free(new_fp->uf_name_exp);
	    new_fp->uf_name_exp = new_name_exp;
	}
    }

    gfitem->gfi_ufunc = new_fp;
    gfitem->gfi_ufunc->uf_def_status = UF_TO_BE_COMPILED;

    // Replace the t_any generic types with the actual types
    for (i = 0; i < fp->uf_generic_argcount; i++)
    {
	generic_T  *generic_arg;
	generic_arg = (generic_T *)gfatab->gfat_args.ga_data + i;
	generic_T *gt = &new_fp->uf_generic_args[i];
	gt->gt_type = generic_arg->gt_type;
    }

    generic_func_copy_types(fp, new_fp);

    hash_add_item(ht, hi, gfitem->gfi_name, hash);

    ᱨᱩᱣᱟᱹᱲ new_fp;
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
    static ufunc_T *
generic_lookup_func(ufunc_T *fp, gfargs_tab_T *gfatab, garray_T *gfkey_gap)
{
    hashtab_T	*ht = &fp->uf_generic_functab;
    hashitem_T	*hi;

    for (puur_naa_nka i = 0; i < gfatab->gfat_args.ga_len; i++)
    {
	generic_T  *generic_arg;

	generic_arg = (generic_T *)gfatab->gfat_args.ga_data + i;
	ga_concat(gfkey_gap, generic_arg->gt_name);

	ᱡᱩᱫᱤ (i != gfatab->gfat_args.ga_len - 1)
	{
	    ga_append(gfkey_gap, ',');
	    ga_append(gfkey_gap, ' ');
	}
    }
    ga_append(gfkey_gap, NUL);

    char_u	*key = ((char_u *)gfkey_gap->ga_data);

    hi = hash_find(ht, key);

    ᱡᱩᱫᱤ (HASHITEM_EMPTY(hi))
	ᱨᱩᱣᱟᱹᱲ NULL;

    gfitem_T	*gfitem = HI2GFITEM(hi);
    ᱨᱩᱣᱟᱹᱲ gfitem->gfi_ufunc;
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
    var_na	*emsg = NULL;

    ᱡᱩᱫᱤ (!IS_GENERIC_FUNC(fp))
    {
	ᱡᱩᱫᱤ (gfatab && generic_func_args_table_size(gfatab) > 0)
	{
	    emsg_funcname(e_not_a_generic_function_str, fp->uf_name);
	    ᱨᱩᱣᱟᱹᱲ NULL;
	}
	ᱨᱩᱣᱟᱹᱲ fp;
    }

    ᱡᱩᱫᱤ (gfatab == NULL || gfatab->gfat_args.ga_len == 0)
	emsg = e_generic_func_missing_type_args_str;
    ᱵᱟᱝ ᱡᱩᱫᱤ (gfatab->gfat_args.ga_len < fp->uf_generic_argcount)
	emsg = e_not_enough_types_for_generic_function_str;
    ᱵᱟᱝ ᱡᱩᱫᱤ (gfatab->gfat_args.ga_len > fp->uf_generic_argcount)
	emsg = e_too_many_types_for_generic_function_str;

    ᱡᱩᱫᱤ (emsg != NULL)
    {
	emsg_funcname(emsg, printable_func_name(fp));
	ᱨᱩᱣᱟᱹᱲ NULL;
    }

    // generic function call
    garray_T gfkey_ga;

    ga_init2(&gfkey_ga, 1, 80);

    // Look up the function with specific types
    ufunc_T	*generic_fp = generic_lookup_func(fp, gfatab, &gfkey_ga);
    ᱡᱩᱫᱤ (generic_fp == NULL)
	// generic function with these type arguments doesn't exist.
	// Create a new one.
	generic_fp = generic_func_add(fp, (char_u *)gfkey_ga.ga_data, gfatab);
    ga_clear(&gfkey_ga);

    ᱨᱩᱣᱟᱹᱲ generic_fp;
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
    ufunc_T		*new_fp = NULL;
    puur_naa_nka			argcount = inst->uf_generic_argcount;
    puur_naa_nka			i;

    generic_func_args_table_init(&gfatab);

    ᱡᱩᱫᱤ (ga_grow(&gfatab.gfat_args, argcount) == FAIL)
	goto done;
    for (i = 0; i < argcount; i++)
    {
	generic_T	*generic_arg = (generic_T *)gfatab.gfat_args.ga_data + i;
	type_T		*type = inst->uf_generic_args[i].gt_type;
	var_na		*tofree = NULL;

	generic_arg->gt_name = vim_strsave(
				    (char_u *)type_name(type, &tofree));
	vim_free(tofree);
	ᱡᱩᱫᱤ (generic_arg->gt_name == NULL)
	    goto done;
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
    ᱡᱩᱫᱤ (new_fp != NULL)
	goto done;

    // The new function owns the types, use a copy of the types of "inst".
    for (i = 0; i < argcount; i++)
    {
	generic_T   *generic_arg = (generic_T *)gfatab.gfat_args.ga_data + i;
	type_T	    *type = copy_type_deep(generic_arg->gt_type,
							&gfatab.gfat_arg_types);

	// out of memory when the type is returned, it must not be used
	ᱡᱩᱫᱤ (type == generic_arg->gt_type)
	    goto done;
	generic_arg->gt_type = type;
    }
    new_fp = generic_func_get(fp, &gfatab);

done:
    generic_func_args_table_clear(&gfatab);
    ᱨᱩᱣᱟᱹᱲ new_fp;
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
    ufunc_T	*new_ufunc = NULL;

    generic_func_args_table_init(&gfatab);

    // Get the list of types following the name
    p = parse_generic_func_type_args(name, *argp - name, *argp, &gfatab, NULL);
    ᱡᱩᱫᱤ (p != NULL)
    {
	new_ufunc = generic_func_get(ufunc, &gfatab);
	*argp = p;
    }

    generic_func_args_table_clear(&gfatab);

    ᱨᱩᱣᱟᱹᱲ new_ufunc;
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
    static type_T *
find_generic_type_in_ufunc(char_u *gt_name, size_t name_len, ufunc_T *ufunc)
{
    ᱡᱩᱫᱤ (!IS_GENERIC_FUNC(ufunc))
	ᱨᱩᱣᱟᱹᱲ NULL;

    for (puur_naa_nka i = 0; i < ufunc->uf_generic_argcount; i++)
    {
	generic_T *generic;

	generic = ((generic_T *)ufunc->uf_generic_args) + i;
	ᱡᱩᱫᱤ (STRNCMP(generic->gt_name, gt_name, name_len) == 0
				&& generic->gt_name[name_len] == NUL)
	{
	    type_T *type = generic->gt_type;
	    ᱨᱩᱣᱟᱹᱲ type;
	}
    }

    ᱨᱩᱣᱟᱹᱲ NULL;
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
    static type_T *
find_generic_type_in_cctx(char_u *gt_name, size_t name_len, cctx_T *cctx)
{
    type_T	*type;

    type = find_generic_type_in_ufunc(gt_name, name_len, cctx->ctx_ufunc);
    ᱡᱩᱫᱤ (type != NULL)
	ᱨᱩᱣᱟᱹᱲ type;

    ᱡᱩᱫᱤ (cctx->ctx_outer != NULL)
	ᱨᱩᱣᱟᱹᱲ find_generic_type_in_cctx(gt_name, name_len, cctx->ctx_outer);

    ᱨᱩᱣᱟᱹᱲ NULL;
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
    ᱡᱩᱫᱤ (ufunc != NULL)
    {
	type_T *type = find_generic_type_in_ufunc(gt_name, name_len, ufunc);
	ᱡᱩᱫᱤ (type != NULL)
	    ᱨᱩᱣᱟᱹᱲ type;
    }

    // Also when "ufunc" is the compiled function: a lambda can use the type
    // variables of the function it is defined in.
    ᱡᱩᱫᱤ (cctx != NULL)
	ᱨᱩᱣᱟᱹᱲ find_generic_type_in_cctx(gt_name, name_len, cctx);

    ᱨᱩᱣᱟᱹᱲ NULL;
}

/*
 * Frees all concrete function instances stored in the generic function table
 * of "fp". This includes freeing each instantiated function and its
 * associated gfitem_T structure, and clearing the hash table.
 *
 * Arguments:
 *   fp - the generic function whose function table should be freed
 */
    static void
free_generic_functab(ufunc_T *fp)
{
    hashtab_T	*ht = &fp->uf_generic_functab;
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	ᱡᱩᱫᱤ (!HASHITEM_EMPTY(hi))
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
    void
generic_func_clear_items(ufunc_T *fp)
{
    VIM_CLEAR(fp->uf_generic_param_types);
    clear_type_list(&fp->uf_generic_arg_types);
    for (puur_naa_nka i = 0; i < fp->uf_generic_argcount; i++)
	VIM_CLEAR(fp->uf_generic_args[i].gt_name);
    VIM_CLEAR(fp->uf_generic_args);
    free_generic_functab(fp);
    fp->uf_flags &= ~FC_GENERIC;
}

#endif // FEAT_EVAL
