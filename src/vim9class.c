/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9class.c: Vim9 script class support
 */

#define USING_FLOAT_STUFF
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

സ്ഥിതം class_T *first_class = ശൂന്യ;
സ്ഥിതം class_T *next_nonref_class = ശൂന്യ;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    സ്ഥിതം ശൂന്യം
class_created(class_T *cl)
{
    എങ്കിൽ (first_class != ശൂന്യ)
    {
	cl->class_next_used = first_class;
	first_class->class_prev_used = cl;
    }
    first_class = cl;
}

/*
 * Call this function when a class has been cleared and is about to be freed.
 * It is removed from the list headed by "first_class".
 */
    സ്ഥിതം ശൂന്യം
class_cleared(class_T *cl)
{
    എങ്കിൽ (cl->class_next_used != ശൂന്യ)
	cl->class_next_used->class_prev_used = cl->class_prev_used;
    എങ്കിൽ (cl->class_prev_used != ശൂന്യ)
	cl->class_prev_used->class_next_used = cl->class_next_used;
    അല്ലെങ്കിൽ എങ്കിൽ (first_class == cl)
	first_class = cl->class_next_used;

    // update the next class to check if needed
    എങ്കിൽ (cl == next_nonref_class)
	next_nonref_class = cl->class_next_used;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_member(
    exarg_T	*eap,
    char_u	*വരി,
    char_u	*varname,
    പൂർണ്ണസംഖ്യ		has_public,	    // TRUE if "public" seen before "varname"
    char_u	**varname_end,
    പൂർണ്ണസംഖ്യ		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_വ്യഞ്ജകം)
{
    *varname_end = to_name_end(varname, FALSE);
    എങ്കിൽ (*varname == '_' && has_public)
    {
	semsg(_(e_public_variable_name_cannot_start_with_underscore_str), വരി);
	തിരികെ FAIL;
    }

    char_u *colon = skipwhite(*varname_end);
    char_u *type_arg = colon;
    type_T *type = ശൂന്യ;
    *has_type = FALSE;
    എങ്കിൽ (*colon == ':')
    {
	എങ്കിൽ (VIM_ISWHITE(**varname_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    തിരികെ FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, ശൂന്യ, ശൂന്യ, TRUE);
	എങ്കിൽ (type == ശൂന്യ || !valid_declaration_type(type))
	    തിരികെ FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    എങ്കിൽ (type == ശൂന്യ && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	തിരികെ FAIL;
    }

    എങ്കിൽ (init_വ്യഞ്ജകം == ശൂന്യ && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_in_interface));
	തിരികെ FAIL;
    }

    എങ്കിൽ (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *വ്യഞ്ജകം_start, *വ്യഞ്ജകം_end;

	എങ്കിൽ (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    തിരികെ FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(ശൂന്യം)skip_വ്യഞ്ജകം_concatenate(&init_arg, &വ്യഞ്ജകം_start, &വ്യഞ്ജകം_end, &evalarg);

	init_arg = skipwhite(init_arg);
	എങ്കിൽ (*init_arg != NUL && !vim9_comment_start(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, ശൂന്യ);
	    തിരികെ FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	എങ്കിൽ (type == ശൂന്യ)
	    type = &t_any;

	*init_വ്യഞ്ജകം = vim_strnsave(വ്യഞ്ജകം_start, വ്യഞ്ജകം_end - വ്യഞ്ജകം_start);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, ശൂന്യ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!valid_declaration_type(type))
	തിരികെ FAIL;

    *type_ret = type;
    തിരികെ OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_end,
    പൂർണ്ണസംഖ്യ		has_public,
    പൂർണ്ണസംഖ്യ		has_final,
    പൂർണ്ണസംഖ്യ		has_const,
    പൂർണ്ണസംഖ്യ		has_type,
    type_T	*type,
    char_u	*init_വ്യഞ്ജകം)
{
    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_data) + gap->ga_len;
    m->ocm_name.length = (size_t)(varname_end - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.length);
    എങ്കിൽ (m->ocm_name.string == ശൂന്യ)
    {
	m->ocm_name.length = 0;
	തിരികെ FAIL;
    }
    m->ocm_access = has_public ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_PRIVATE : VIM_ACCESS_READ;
    എങ്കിൽ (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    എങ്കിൽ (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    എങ്കിൽ (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    എങ്കിൽ (init_വ്യഞ്ജകം != ശൂന്യ)
    {
	m->ocm_init = init_വ്യഞ്ജകം;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    തിരികെ OK;
}

/*
 * Move the class or object members found while parsing a class into the class.
 * "gap" contains the found members.
 * "parent_members" points to the members in the parent class (if any)
 * "parent_count" is the number of members in the parent class
 * "members" will be set to the newly allocated array of members and
 * "member_count" set to the number of members.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_members_to_class(
    garray_T	*gap,
    ocmember_T	*parent_members,
    പൂർണ്ണസംഖ്യ		parent_എണ്ണം,
    ocmember_T	**members,
    പൂർണ്ണസംഖ്യ		*member_എണ്ണം)
{
    *member_എണ്ണം = parent_എണ്ണം + gap->ga_len;
    *members = *member_എണ്ണം == 0 ? ശൂന്യ
				       : ALLOC_MULT(ocmember_T, *member_എണ്ണം);
    എങ്കിൽ (*member_എണ്ണം > 0 && *members == ശൂന്യ)
	തിരികെ FAIL;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < parent_എണ്ണം; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = parent_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.length);
	എങ്കിൽ (m->ocm_name.string == ശൂന്യ)
	{
	    m->ocm_name.length = 0;
	    തിരികെ FAIL;
	}
	എങ്കിൽ (m->ocm_init != ശൂന്യ)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    എങ്കിൽ (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + parent_എണ്ണം,
			       gap->ga_data, വലുപ്പം(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_data);
    തിരികെ OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    പൂർണ്ണസംഖ്യ
object_index_from_itf_index(class_T *itf, പൂർണ്ണസംഖ്യ is_method, പൂർണ്ണസംഖ്യ idx, class_T *cl)
{
    എങ്കിൽ (idx >= (is_method ? itf->class_obj_method_എണ്ണം
				   : itf->class_obj_member_എണ്ണം))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	തിരികെ 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    എങ്കിൽ (cl == itf)
	തിരികെ idx;

    itf2class_T		*i2c = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			searching = TRUE;
    പൂർണ്ണസംഖ്യ			method_offset = 0;

    വേണ്ടി (class_T *super = cl; super != ശൂന്യ && searching;
						super = super->class_extends)
    {
	വേണ്ടി (i2c = itf->class_itf2class; i2c != ശൂന്യ; i2c = i2c->i2c_next)
	{
	    എങ്കിൽ (i2c->i2c_class == super && i2c->i2c_is_method == is_method)
	    {
		searching = FALSE;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (searching && is_method)
	    // The parent class methods are stored after the current class
	    // methods.
	    method_offset += super->class_obj_method_എണ്ണം_child;
    }
    എങ്കിൽ (i2c == ശൂന്യ)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	തിരികെ 0;
    }

    // A table follows the i2c for the class
    പൂർണ്ണസംഖ്യ *table = (പൂർണ്ണസംഖ്യ *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    തിരികെ table[idx] + method_offset;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    പൂർണ്ണസംഖ്യ	    is_class)
{
    typval_T	tv;
    പൂർണ്ണസംഖ്യ		success = FALSE;

    എങ്കിൽ (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	തിരികെ success;
    }

    tv.v_type = VAR_UNKNOWN;
    എങ്കിൽ (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_found_str), extends_name);
	തിരികെ success;
    }

    എങ്കിൽ (tv.v_type != VAR_CLASS || tv.vval.v_class == ശൂന്യ
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    അല്ലെങ്കിൽ
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    തിരികെ success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
validate_extends_generic_method(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    അക്ഷരം	*സന്ദേശം = ശൂന്യ;

    എങ്കിൽ (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	തിരികെ TRUE;

    എങ്കിൽ (super_fp->uf_generic_argcount == 0)
	സന്ദേശം = e_concrete_method_str_override_with_generic_method_in_class_str;
    അല്ലെങ്കിൽ എങ്കിൽ (cl_fp->uf_generic_argcount == 0)
	സന്ദേശം = e_generic_method_str_override_with_concrete_method_in_class_str;
    അല്ലെങ്കിൽ
	സന്ദേശം = e_generic_method_str_type_arguments_mismatch_in_class_str;

    semsg(_(സന്ദേശം), cl_fp->uf_name, super_cl->class_name.string);
    തിരികെ FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    പൂർണ്ണസംഖ്യ		method_എണ്ണം = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_data);

    ആകുമ്പോൾ (super != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ extends_method_എണ്ണം = super->class_obj_method_എണ്ണം_child;
	എങ്കിൽ (extends_method_എണ്ണം == 0)
	{
	    super = super->class_extends;
	    തുടരുക;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < extends_method_എണ്ണം; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    പൂർണ്ണസംഖ്യ	    extends_private = (*pstr == '_');
	    എങ്കിൽ (extends_private)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < method_എണ്ണം; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		പൂർണ്ണസംഖ്യ	priv_method = (*qstr == '_');
		എങ്കിൽ (priv_method)
		    qstr++;
		എങ്കിൽ (STRCMP(pstr, qstr) == 0)
		{
		    എങ്കിൽ (priv_method != extends_private)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_method_str_of_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			തിരികെ FALSE;
		    }

		    എങ്കിൽ (!validate_extends_generic_method(super,
						extends_methods[i], cl_fp[j]))
			    തിരികെ FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    തിരികെ TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    പൂർണ്ണസംഖ്യ member_എണ്ണം = objmembers_gap->ga_len;
    എങ്കിൽ (member_എണ്ണം == 0)
	തിരികെ TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    വേണ്ടി (പൂർണ്ണസംഖ്യ c_i = 0; c_i < member_എണ്ണം; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	ആകുമ്പോൾ (p_cl != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ p_member_എണ്ണം = p_cl->class_obj_member_എണ്ണം;

	    എങ്കിൽ (p_member_എണ്ണം == 0)
	    {
		p_cl = p_cl->class_extends;
		തുടരുക;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    വേണ്ടി (പൂർണ്ണസംഖ്യ p_i = 0; p_i < p_member_എണ്ണം; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		എങ്കിൽ (STRCMP(pstr, qstr) == 0)
		{
		    എങ്കിൽ (c_protected != p_protected)
			semsg(_(e_public_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    അല്ലെങ്കിൽ
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    തിരികെ FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    തിരികെ TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    പൂർണ്ണസംഖ്യ member_എണ്ണം = objmembers_gap->ga_len;
    എങ്കിൽ (member_എണ്ണം == 0)
	തിരികെ TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    വേണ്ടി (പൂർണ്ണസംഖ്യ c_i = 0; c_i < member_എണ്ണം; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	പൂർണ്ണസംഖ്യ	    var_found = FALSE;

	// Check in all the parent classes in the lineage
	ആകുമ്പോൾ (p_cl != ശൂന്യ && !var_found)
	{
	    പൂർണ്ണസംഖ്യ p_member_എണ്ണം = p_cl->class_obj_member_എണ്ണം;
	    എങ്കിൽ (p_member_എണ്ണം == 0)
	    {
		p_cl = p_cl->class_extends;
		തുടരുക;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    വേണ്ടി (പൂർണ്ണസംഖ്യ p_i = 0; p_i < p_member_എണ്ണം; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		എങ്കിൽ (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    തുടരുക;

		// Ensure the type is matching.
		where.wt_func_name = (അക്ഷരം *)c_m->ocm_name.string;
		where.wt_kind = WT_MEMBER;

		എങ്കിൽ (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    തിരികെ FALSE;

		var_found = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    തിരികെ TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	പൂർണ്ണസംഖ്യ extends_method_എണ്ണം = loop == 1
				? extends_cl->class_class_function_എണ്ണം
				: extends_cl->class_obj_method_എണ്ണം;
	എങ്കിൽ (extends_method_എണ്ണം == 0)
	    തുടരുക;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	പൂർണ്ണസംഖ്യ method_എണ്ണം = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_data
						: objmethods_gap->ga_data);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < extends_method_എണ്ണം; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    എങ്കിൽ (!IS_ABSTRACT_METHOD(uf))
		തുടരുക;

	    പൂർണ്ണസംഖ്യ	concrete_method_found = FALSE;
	    പൂർണ്ണസംഖ്യ	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    വേണ്ടി (j = 0; !concrete_method_found && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		എങ്കിൽ (!IS_ABSTRACT_METHOD(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_method_found = TRUE;
	    }

	    എങ്കിൽ (concrete_method_found)
		തുടരുക;

	    വേണ്ടി (j = 0; j < method_എണ്ണം; j++)
	    {
		എങ്കിൽ (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_method_found = TRUE;
		    നിർത്തുക;
		}
	    }

	    എങ്കിൽ (!concrete_method_found)
	    {
		semsg(_(e_abstract_method_str_not_implemented), uf->uf_name);
		തിരികെ FALSE;
	    }
	}
    }

    തിരികെ TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    പൂർണ്ണസംഖ്യ		cl_member_എണ്ണം,
    class_T	*extends_cl)
{
    പൂർണ്ണസംഖ്യ		variable_present  = FALSE;

    വേണ്ടി (പൂർണ്ണസംഖ്യ cl_i = 0; cl_i < cl_member_എണ്ണം; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	എങ്കിൽ (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    തുടരുക;

	// Ensure the access type is same
	എങ്കിൽ (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_of_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    തിരികെ FALSE;
	}

	// Ensure the type is matching.
	എങ്കിൽ (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	അല്ലെങ്കിൽ
	{
	    where.wt_func_name = (അക്ഷരം *)m->ocm_name.string;
	    where.wt_kind = WT_MEMBER;
	    എങ്കിൽ (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		തിരികെ FALSE;
	}

	variable_present = TRUE;
	നിർത്തുക;
    }

    എങ്കിൽ (!variable_present && extends_cl != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ ext_cl_എണ്ണം = extends_cl->class_obj_member_എണ്ണം;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	തിരികെ intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_എണ്ണം,
					extends_cl->class_extends);
    }

    തിരികെ variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    പൂർണ്ണസംഖ്യ if_എണ്ണം = ifcl->class_obj_member_എണ്ണം;
    എങ്കിൽ (if_എണ്ണം == 0)
	തിരികെ TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_data);
    പൂർണ്ണസംഖ്യ cl_എണ്ണം = objmembers_gap->ga_len;
    വേണ്ടി (പൂർണ്ണസംഖ്യ if_i = 0; if_i < if_എണ്ണം; ++if_i)
    {
	എങ്കിൽ (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_എണ്ണം, extends_cl))
	{
	    semsg(_(e_variable_str_of_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    തിരികെ FALSE;
	}
    }

    തിരികെ TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
intf_method_type_matches(
    ufunc_T	*if_method,
    ufunc_T	*cl_method,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    എങ്കിൽ (cl_method->uf_generic_argcount != if_method->uf_generic_argcount)
    {
	semsg(_(e_type_var_എണ്ണം_of_method_str_differs_from_interface_str),
						cl_method->uf_name, intf_name);
	തിരികെ FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (അക്ഷരം *)if_method->uf_name;
    where.wt_kind = WT_METHOD;
    എങ്കിൽ (check_type(if_method->uf_func_type, cl_method->uf_func_type, TRUE,
								where) == FAIL)
	തിരികെ FALSE;

    തിരികെ TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
intf_method_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    പൂർണ്ണസംഖ്യ	    cl_എണ്ണം,
    class_T *extends_cl,
    char_u  *intf_name)
{
    പൂർണ്ണസംഖ്യ		method_present  = FALSE;

    വേണ്ടി (പൂർണ്ണസംഖ്യ cl_i = 0; cl_i < cl_എണ്ണം; ++cl_i)
    {
	എങ്കിൽ (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    എങ്കിൽ (!intf_method_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		തിരികെ FALSE;
	    method_present = TRUE;
	    നിർത്തുക;
	}
    }

    എങ്കിൽ (!method_present && extends_cl != ശൂന്യ)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	പൂർണ്ണസംഖ്യ	ext_cl_എണ്ണം = extends_cl->class_obj_method_എണ്ണം;
	തിരികെ intf_method_present(if_ufunc, ext_cl_fp, ext_cl_എണ്ണം,
					extends_cl->class_extends, intf_name);
    }

    തിരികെ method_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    പൂർണ്ണസംഖ്യ if_എണ്ണം = ifcl->class_obj_method_എണ്ണം;
    എങ്കിൽ (if_എണ്ണം == 0)
	തിരികെ TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_data);
    പൂർണ്ണസംഖ്യ cl_എണ്ണം = objmethods_gap->ga_len;
    വേണ്ടി (പൂർണ്ണസംഖ്യ if_i = 0; if_i < if_എണ്ണം; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	എങ്കിൽ (!intf_method_present(if_fp[if_i], cl_fp, cl_എണ്ണം, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_method_str_of_interface_str_not_implemented),
		    if_name, intf_class_name);
	    തിരികെ FALSE;
	}
    }

    തിരികെ TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    പൂർണ്ണസംഖ്യ		success = TRUE;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_data)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	എങ്കിൽ (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_found_str), impl);
	    success = FALSE;
	    നിർത്തുക;
	}

	എങ്കിൽ (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == ശൂന്യ
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    നിർത്തുക;
	}

	class_T *ifcl = tv.vval.v_class;
	എങ്കിൽ (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    നിർത്തുക;
	}
	((class_T **)intf_classes_gap->ga_data)[intf_classes_gap->ga_len]
								= ifcl;
	intf_classes_gap->ga_len++;
	++ifcl->class_refcount;

	// check the variables of the interface match the members of the class
	success = validate_interface_variables(impl, ifcl, objmembers_gap,
								extends_cl);
	clear_tv(&tv);
    }

    എങ്കിൽ (!success)
	തിരികെ FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_data;
    cl->class_interface_എണ്ണം = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_data)[i],
		((class_T **)intf_classes_gap->ga_data)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = ശൂന്യ;
    cl->class_interface_എണ്ണം = 0;
    cl->class_extends = ശൂന്യ;

    തിരികെ success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < intf_classes_gap->ga_len; j++)
    {
	എങ്കിൽ (((class_T **)intf_classes_gap->ga_data)[j] == ifcl)
	    തിരികെ TRUE;
    }

    തിരികെ FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.length);
    എങ്കിൽ (intf_name == ശൂന്യ)
	തിരികെ FALSE;

    എങ്കിൽ (ga_grow(impl_gap, 1) == FAIL)
	പോകുക fail;

    char_u **intf_names = (char_u **)impl_gap->ga_data;
    intf_names[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    എങ്കിൽ (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	പോകുക fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_data;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    തിരികെ TRUE;
fail:
    vim_free(intf_name);
    തിരികെ FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < super->class_interface_എണ്ണം; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	എങ്കിൽ (!is_interface_class_present(intf_classes_gap, ifcl))
	    add_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    തിരികെ TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_func_arg_names(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    വേണ്ടി (പൂർണ്ണസംഖ്യ loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	വേണ്ടി (പൂർണ്ണസംഖ്യ fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_data)[fi];

	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < uf->uf_args.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_args.ga_data)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		വേണ്ടി (പൂർണ്ണസംഖ്യ mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_data + mi)->ocm_name.string;
		    എങ്കിൽ (STRCMP(aname, mname) == 0)
		    {
			എങ്കിൽ (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_argument_already_declared_in_class_str),
				aname);

			തിരികെ FALSE;
		    }
		}
	    }
	}
    }

    തിരികെ TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_reserved_varname(char_u *varname, char_u *varname_end)
{
    പൂർണ്ണസംഖ്യ reserved = FALSE;
    char_u save_varname_end = *varname_end;
    *varname_end = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_end = save_varname_end;

    തിരികെ reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_end)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_end - varname)};
    പൂർണ്ണസംഖ്യ		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    എങ്കിൽ (*pstr.string == '_')
    {
	pstr.string++;
	pstr.length--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    വേണ്ടി (പൂർണ്ണസംഖ്യ loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_data) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.length};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    എങ്കിൽ (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.length--;
		q_protected = true;
	    }

	    എങ്കിൽ (pstr.length == qstr.length
		&& STRNCMP(pstr.string, qstr.string, pstr.length) == 0)
	    {
		char_u	save_c = *varname_end;

		*varname_end = NUL;
		എങ്കിൽ (p_protected != q_protected)
		    semsg(_(e_public_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		അല്ലെങ്കിൽ
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_end = save_c;
		dup = TRUE;
		നിർത്തുക;
	    }
	}
    }

    തിരികെ dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_duplicate_method(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    വേണ്ടി (പൂർണ്ണസംഖ്യ loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_data)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    എങ്കിൽ (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		തിരികെ TRUE;
	    }
	}
    }

    തിരികെ FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_valid_constructor(ufunc_T *uf, പൂർണ്ണസംഖ്യ is_abstract, പൂർണ്ണസംഖ്യ has_static)
{
    // Constructors are not allowed in abstract classes.
    എങ്കിൽ (is_abstract)
    {
	emsg(_(e_cannot_define_new_method_in_abstract_class));
	തിരികെ FALSE;
    }
    // A constructor is always static, no need to define it so.
    എങ്കിൽ (has_static)
    {
	emsg(_(e_cannot_define_new_method_as_static));
	തിരികെ FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    എങ്കിൽ (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_method));
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
object_check_builtin_method_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    പൂർണ്ണസംഖ്യ	    valid = FALSE;
    type_T  method_sig;
    type_T  method_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_FIELD(method_sig);
    CLEAR_FIELD(method_rt);
    method_sig.tt_type = VAR_FUNC;

    എങ്കിൽ (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	method_rt.tt_type = VAR_സംഖ്യ;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	method_rt.tt_type = VAR_BOOL;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	method_rt.tt_type = VAR_STRING;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    അല്ലെങ്കിൽ
	semsg(_(e_builtin_object_method_str_not_supported), uf->uf_name);

    where.wt_func_name = (അക്ഷരം *)uf->uf_name;
    where.wt_kind = WT_METHOD;
    എങ്കിൽ (valid && !check_type(&method_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    തിരികെ valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    പൂർണ്ണസംഖ്യ
is_valid_builtin_obj_methodname(char_u *funcname)
{
    തിരഞ്ഞെടുപ്പ് (funcname[0])
    {
	അവസ്ഥ 'e':
	    തിരികെ STRNCMP(funcname, "empty", 5) == 0;

	അവസ്ഥ 'l':
	    തിരികെ STRNCMP(funcname, "len", 3) == 0;

	അവസ്ഥ 'n':
	    തിരികെ STRNCMP(funcname, "new", 3) == 0;

	അവസ്ഥ 's':
	    തിരികെ STRNCMP(funcname, "string", 6) == 0;
    }

    തിരികെ FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_method(
    class_T		*cl,
    class_builtin_T	builtin_method,
    പൂർണ്ണസംഖ്യ			*method_idx)
{
    *method_idx = -1;

    എങ്കിൽ (cl == ശൂന്യ)
	തിരികെ ശൂന്യ;

    *method_idx = cl->class_builtin_methods[builtin_method];
    തിരികെ *method_idx != -1 ? cl->class_obj_methods[*method_idx] : ശൂന്യ;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
update_member_method_lookup_table(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    പൂർണ്ണസംഖ്യ		pobj_method_offset)
{
    എങ്കിൽ (ifcl == ശൂന്യ)
	തിരികെ OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(വലുപ്പം(itf2class_T)
				+ ifcl->class_obj_member_എണ്ണം * വലുപ്പം(പൂർണ്ണസംഖ്യ));
    എങ്കിൽ (if2cl == ശൂന്യ)
	തിരികെ FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = FALSE;

    വേണ്ടി (പൂർണ്ണസംഖ്യ if_i = 0; if_i < ifcl->class_obj_member_എണ്ണം; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	വേണ്ടി (പൂർണ്ണസംഖ്യ cl_i = 0; cl_i < cl->class_obj_member_എണ്ണം; ++cl_i)
	{
	    എങ്കിൽ (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		പൂർണ്ണസംഖ്യ *table = (പൂർണ്ണസംഖ്യ *)(if2cl + 1);
		table[if_i] = cl_i;
		നിർത്തുക;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(വലുപ്പം(itf2class_T)
				+ ifcl->class_obj_method_എണ്ണം * വലുപ്പം(പൂർണ്ണസംഖ്യ));
    എങ്കിൽ (if2cl == ശൂന്യ)
	തിരികെ FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = TRUE;

    വേണ്ടി (പൂർണ്ണസംഖ്യ if_i = 0; if_i < ifcl->class_obj_method_എണ്ണം; ++if_i)
    {
	പൂർണ്ണസംഖ്യ	done = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	വേണ്ടി (പൂർണ്ണസംഖ്യ cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    എങ്കിൽ (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_data)[cl_i]->uf_name) == 0)
	    {
		പൂർണ്ണസംഖ്യ *table = (പൂർണ്ണസംഖ്യ *)(if2cl + 1);
		table[if_i] = cl_i;
		done = TRUE;
		നിർത്തുക;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	എങ്കിൽ (!done)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    എങ്കിൽ (cl->class_extends != ifcl)
	    {
		class_T		*parent = cl->class_extends;
		പൂർണ്ണസംഖ്യ		method_offset = objmethods->ga_len;

		ആകുമ്പോൾ (!done && parent != ശൂന്യ && parent != ifcl)
		{
		    വേണ്ടി (പൂർണ്ണസംഖ്യ cl_i = 0;
			    cl_i < parent->class_obj_method_എണ്ണം_child; ++cl_i)
		    {
			എങ്കിൽ (STRCMP(if_name,
				    parent->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    പൂർണ്ണസംഖ്യ *table = (പൂർണ്ണസംഖ്യ *)(if2cl + 1);
			    table[if_i] = method_offset + cl_i;
			    done = TRUE;
			    നിർത്തുക;
			}
		    }
		    method_offset += parent->class_obj_method_എണ്ണം_child;
		    parent = parent->class_extends;
		}
	    }

	    എങ്കിൽ (!done)
	    {
		പൂർണ്ണസംഖ്യ *table = (പൂർണ്ണസംഖ്യ *)(if2cl + 1);
		table[if_i] = pobj_method_offset + if_i;
	    }
	}
    }

    തിരികെ OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_interface_എണ്ണം; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	ആകുമ്പോൾ (ifcl != ശൂന്യ)
	{
	    എങ്കിൽ (update_member_method_lookup_table(ifcl, cl, objmethods_gap,
								0) == FAIL)
		തിരികെ FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    എങ്കിൽ (extends_cl != ശൂന്യ)
    {
	class_T		*pclass = extends_cl;
	പൂർണ്ണസംഖ്യ		pobj_method_offset = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	ആകുമ്പോൾ (pclass != ശൂന്യ)
	{
	    എങ്കിൽ (update_member_method_lookup_table(pclass, cl,
			objmethods_gap, pobj_method_offset) == FAIL)
		തിരികെ FAIL;

	    pobj_method_offset += pclass->class_obj_method_എണ്ണം_child;
	    pclass = pclass->class_extends;
	}
    }

    തിരികെ OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_എണ്ണം);
    എങ്കിൽ (cl->class_members_tv == ശൂന്യ)
	തിരികെ FAIL;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_class_member_എണ്ണം; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	എങ്കിൽ (m->ocm_init != ശൂന്യ)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_വ്യഞ്ജകം(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    എങ്കിൽ (etv == ശൂന്യ)
		തിരികെ FAIL;

	    എങ്കിൽ (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && etv->v_type != VAR_SPECIAL)
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = typval2type(etv, get_copyID(),
			type_list_gap,
			TVTT_DO_MEMBER|TVTT_MORE_SPECIFIC);
	    *tv = *etv;
	    vim_free(etv);
	}
	അല്ലെങ്കിൽ
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = ശൂന്യ;
	}
	set_tv_type(tv, m->ocm_type);
	എങ്കിൽ (m->ocm_flags & OCMFLAG_CONST)
	    item_lock(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    തിരികെ OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    സ്ഥിതം ശൂന്യം
add_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    പൂർണ്ണസംഖ്യ		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_obj_member_എണ്ണം; ++i)
    {
	എങ്കിൽ (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    തുടരുക;

	എങ്കിൽ (i > (is_enum ? 2 : 0))
	    GA_CONCAT_LITERAL(&fga, ", ");
	GA_CONCAT_LITERAL(&fga, "this.");
	ocmember_T *m = cl->class_obj_members + i;
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
	GA_CONCAT_LITERAL(&fga, " = v:none");
    }
    GA_CONCAT_LITERAL(&fga, ")\nenddef\n");
    ga_append(&fga, NUL);

    exarg_T fea;
    CLEAR_FIELD(fea);
    fea.cmdidx = CMD_def;
    fea.cmd = fea.arg = fga.ga_data;

    garray_T വരികൾ_to_free;
    ga_init2(&വരികൾ_to_free, വലുപ്പം(char_u *), 50);

    ufunc_T *nf = define_function(&fea, ശൂന്യ, &വരികൾ_to_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_എണ്ണം,
			    ശൂന്യ);

    ga_clear_strings(&വരികൾ_to_free);
    vim_free(fga.ga_data);

    എങ്കിൽ (nf != ശൂന്യ && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_data)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	എങ്കിൽ (nf->uf_ret_type != ശൂന്യ)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_args = ശൂന്യ;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    വേണ്ടി (പൂർണ്ണസംഖ്യ loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	പൂർണ്ണസംഖ്യ		*fcount = loop == 1 ? &cl->class_class_function_എണ്ണം
						: &cl->class_obj_method_എണ്ണം;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	പൂർണ്ണസംഖ്യ parent_എണ്ണം = 0;
	എങ്കിൽ (extends_cl != ശൂന്യ)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    parent_എണ്ണം = loop == 1
				? 0
				: extends_cl->class_obj_method_എണ്ണം;

	*fcount = parent_എണ്ണം + gap->ga_len;
	എങ്കിൽ (*fcount == 0)
	{
	    *fup = ശൂന്യ;
	    തുടരുക;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	എങ്കിൽ (*fup == ശൂന്യ)
	    തിരികെ FAIL;

	എങ്കിൽ (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_data, വലുപ്പം(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_data);
	എങ്കിൽ (loop == 1)
	    cl->class_class_function_എണ്ണം_child = gap->ga_len;
	അല്ലെങ്കിൽ
	    cl->class_obj_method_എണ്ണം_child = gap->ga_len;

	എങ്കിൽ (loop == 2)
	{
	    // Copy instance methods from the parent.

	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < parent_എണ്ണം; ++i)
	    {
		// Can't use the same parent function, because "uf_class" is
		// different and compilation will have a different result.
		// Put them after the functions in the current class, object
		// methods may be overruled, then "super.Method()" is used to
		// find a method from the parent.
		ufunc_T *pf = (extends_cl->class_obj_methods)[i];
		ufunc_T *nf = copy_function(pf, 0);
		(*fup)[gap->ga_len + i] = nf;

		// The types of a generic method use the type variables of the
		// parent method, make them use the type variables of the copy.
		എങ്കിൽ (nf != ശൂന്യ && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_args != ശൂന്യ)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		വേണ്ടി (പൂർണ്ണസംഖ്യ ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    എങ്കിൽ (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (അക്ഷരം *)pname;
			where.wt_kind = WT_METHOD;
			(ശൂന്യം)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    എങ്കിൽ (i < gap->ga_len)
		fp->uf_defclass = cl;
	    എങ്കിൽ (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    തിരികെ OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    സ്ഥിതം ശൂന്യം
update_builtin_method_index(class_T *cl)
{
    പൂർണ്ണസംഖ്യ	i;

    വേണ്ടി (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    വേണ്ടി (i = 0; i < cl->class_obj_method_എണ്ണം; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	എങ്കിൽ (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	അല്ലെങ്കിൽ എങ്കിൽ (cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] = i;
	അല്ലെങ്കിൽ എങ്കിൽ (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    സ്ഥിതം char_u *
find_class_name_end(char_u *arg)
{
    char_u *end = arg;

    ആകുമ്പോൾ (ASCII_ISALNUM(*end) || *end == '_'
	    || (*end == '.' && (ASCII_ISALNUM(end[1]) || end[1] == '_')))
	++end;

    തിരികെ end;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_duplicate_enum(
    garray_T	*enum_gap,
    char_u	*varname,
    char_u	*varname_end)
{
    string_T	name = {varname, (size_t)(varname_end - varname)};  // Note: the .string field may
								    // point to a string longer
								    // than the .length field.
								    // So we need to use STRNCMP()
								    // to compare it.
    പൂർണ്ണസംഖ്യ		dup = FALSE;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_data) + i;

	എങ്കിൽ (name.length == m->ocm_name.length
	    && STRNCMP(name.string, m->ocm_name.string, name.length) == 0)
	{
	    char_u  save_c = *varname_end;

	    *varname_end = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_end = save_c;
	    dup = TRUE;
	    നിർത്തുക;
	}
    }

    തിരികെ dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
enum_parse_മൂല്യങ്ങൾ(
    exarg_T	*eap,
    class_T	*en,
    char_u	*വരി,
    garray_T	*gap,
    പൂർണ്ണസംഖ്യ		*അക്കം_enum_മൂല്യങ്ങൾ,
    പൂർണ്ണസംഖ്യ		*enum_end)
{
    evalarg_T	evalarg;
    char_u	*p = വരി;
    അക്ഷരം	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	last_char = NUL;
    പൂർണ്ണസംഖ്യ		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
    ആകുമ്പോൾ (*p != NUL)
    {
	// ignore comment
	എങ്കിൽ (*p == '#')
	    നിർത്തുക;

	എങ്കിൽ (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_മൂല്യം_declaration_str), p);
	    നിർത്തുക;
	}

	char_u *eni_name_start = p;
	char_u *eni_name_end = to_name_end(p, FALSE);

	എങ്കിൽ (is_duplicate_enum(gap, eni_name_start, eni_name_end))
	    നിർത്തുക;

	p = skipwhite(eni_name_end);

	char_u	*init_വ്യഞ്ജകം = ശൂന്യ;
	എങ്കിൽ (*p == '(' || *p == '<')
	{
	    എങ്കിൽ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", വരി);
		നിർത്തുക;
	    }

	    char_u *വ്യഞ്ജകം_start, *വ്യഞ്ജകം_end;

	    p = eni_name_start;
	    (ശൂന്യം)skip_വ്യഞ്ജകം_concatenate(&p, &വ്യഞ്ജകം_start, &വ്യഞ്ജകം_end, &evalarg);

	    ആകുമ്പോൾ (*വ്യഞ്ജകം_start && *വ്യഞ്ജകം_start != '(' && *വ്യഞ്ജകം_start != '<')
		വ്യഞ്ജകം_start++;

	    എങ്കിൽ (വ്യഞ്ജകം_end > വ്യഞ്ജകം_start)
		init_വ്യഞ്ജകം = vim_strnsave(വ്യഞ്ജകം_start, വ്യഞ്ജകം_end - വ്യഞ്ജകം_start);
	}

	എങ്കിൽ (init_വ്യഞ്ജകം == ശൂന്യ)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, വലുപ്പം(initexpr_buf), "%s.new()", en->class_name.string);
	അല്ലെങ്കിൽ
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, വലുപ്പം(initexpr_buf), "%s.new%s", en->class_name.string,
		init_വ്യഞ്ജകം);
	    vim_free(init_വ്യഞ്ജകം);
	}
	എങ്കിൽ (add_member(gap, eni_name_start, eni_name_end, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    നിർത്തുക;

	++*അക്കം_enum_മൂല്യങ്ങൾ;

	എങ്കിൽ (*p != '#')
	    last_char = *p;

	എങ്കിൽ (*p != NUL && *p != ',')
	    നിർത്തുക;

	എങ്കിൽ (*p == ',')
	{
	    എങ്കിൽ (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", വരി);
		നിർത്തുക;
	    }
	    എങ്കിൽ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", വരി);
		നിർത്തുക;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    എങ്കിൽ (*p != NUL && *p != '#')
    {
	എങ്കിൽ (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_argument_str), p);
	rc = FAIL;
    }

    എങ്കിൽ (last_char != ',')
	// last enum value should not be terminated by ","
	*enum_end = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, ശൂന്യ);

    തിരികെ rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
enum_add_മൂല്യങ്ങൾ_member(
    class_T	*en,
    garray_T	*gap,
    പൂർണ്ണസംഖ്യ		അക്കം_enum_മൂല്യങ്ങൾ,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    പൂർണ്ണസംഖ്യ		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_enum_മൂല്യങ്ങൾ; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;

	എങ്കിൽ (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.length);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    എങ്കിൽ (type == ശൂന്യ)
	പോകുക done;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    എങ്കിൽ (type->tt_member != ശൂന്യ)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = add_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_data, fga.ga_len));

done:
    vim_free(fga.ga_data);

    തിരികെ rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    സ്ഥിതം ശൂന്യം
enum_clear_constructors(class_T *en)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < en->class_class_function_എണ്ണം; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	എങ്കിൽ (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    ശൂന്യം
enum_set_internal_obj_vars(class_T *en, object_T *enval)
{
    പൂർണ്ണസംഖ്യ	i;

    വേണ്ടി (i = 0; i < en->class_class_member_എണ്ണം; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	എങ്കിൽ (en_tv != ശൂന്യ && en_tv->v_type == VAR_UNKNOWN)
	    നിർത്തുക;
    }

    എങ്കിൽ (i >= en->class_class_member_എണ്ണം)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	തിരികെ;

    // First object variable is the name
    ocmember_T *മൂല്യം_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(മൂല്യം_ocm->ocm_name.string, മൂല്യം_ocm->ocm_name.length);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_സംഖ്യ;
    ord_tv->vval.v_സംഖ്യ = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    സ്ഥിതം ശൂന്യം
skip_class_body(exarg_T *eap)
{
    വേണ്ടി (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	എങ്കിൽ (theline == ശൂന്യ)
	    നിർത്തുക;

	char_u	*p = theline;
	പൂർണ്ണസംഖ്യ	found;

	// skip ':' and blanks
	വേണ്ടി (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	found = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	എങ്കിൽ (found)
	    നിർത്തുക;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    ശൂന്യം
ex_class(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		is_class = eap->cmdidx == CMD_class;
    പൂർണ്ണസംഖ്യ		is_abstract = eap->cmdidx == CMD_abstract;
    പൂർണ്ണസംഖ്യ		is_enum = eap->cmdidx == CMD_enum;
    പൂർണ്ണസംഖ്യ		added_enum_മൂല്യങ്ങൾ = FALSE;
    പൂർണ്ണസംഖ്യ		is_interface;
    long	start_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    എങ്കിൽ (eap->skip)
    {
	skip_class_body(eap);
	തിരികെ;
    }

    എങ്കിൽ (is_abstract)
    {
	എങ്കിൽ (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_argument_str), arg);
	    തിരികെ;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    എങ്കിൽ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	എങ്കിൽ (is_class)
	    emsg(_(e_class_can_only_be_defined_in_vim9_script));
	അല്ലെങ്കിൽ എങ്കിൽ (is_enum)
	    emsg(_(e_enum_can_only_be_defined_in_vim9_script));
	അല്ലെങ്കിൽ
	    emsg(_(e_interface_can_only_be_defined_in_vim9_script));
	തിരികെ;
    }

    എങ്കിൽ (!ASCII_ISUPPER(*arg))
    {
	എങ്കിൽ (is_class)
	    semsg(_(e_class_name_must_start_with_uppercase_letter_str), arg);
	അല്ലെങ്കിൽ എങ്കിൽ (is_enum)
	    semsg(_(e_enum_name_must_start_with_uppercase_letter_str), arg);
	അല്ലെങ്കിൽ
	    semsg(_(e_interface_name_must_start_with_uppercase_letter_str),
									  arg);
	തിരികെ;
    }
    char_u *name_end = find_name_end(arg, ശൂന്യ, ശൂന്യ, FNE_CHECK_START);
    എങ്കിൽ (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	തിരികെ;
    }
    char_u *name_start = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = ശൂന്യ;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, വലുപ്പം(char_u *), 5);

    arg = skipwhite(name_end);
    ആകുമ്പോൾ (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	എങ്കിൽ (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    എങ്കിൽ (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		പോകുക early_ret;
	    }
	    എങ്കിൽ (extends != ശൂന്യ)
	    {
		emsg(_(e_duplicate_extends));
		പോകുക early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *end = find_class_name_end(arg);
	    എങ്കിൽ (!IS_WHITE_OR_NUL(*end))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		പോകുക early_ret;
	    }
	    extends = vim_strnsave(arg, end - arg);
	    എങ്കിൽ (extends == ശൂന്യ)
		പോകുക early_ret;

	    arg = skipwhite(end + 1);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    എങ്കിൽ (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		പോകുക early_ret;
	    }

	    എങ്കിൽ (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		പോകുക early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    വേണ്ടി (;;)
	    {
		char_u *impl_end = find_class_name_end(arg);
		എങ്കിൽ ((!IS_WHITE_OR_NUL(*impl_end) && *impl_end != ',')
			|| (*impl_end == ','
			    && !IS_WHITE_OR_NUL(*(impl_end + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    പോകുക early_ret;
		}
		എങ്കിൽ (impl_end - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    പോകുക early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_end - arg);
		എങ്കിൽ (iname == ശൂന്യ)
		    പോകുക early_ret;
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < ga_impl.ga_len; ++i)
		    എങ്കിൽ (STRCMP(((char_u **)ga_impl.ga_data)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			പോകുക early_ret;
		    }
		എങ്കിൽ (ga_add_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    പോകുക early_ret;
		}
		എങ്കിൽ (*impl_end != ',')
		{
		    arg = skipwhite(impl_end);
		    നിർത്തുക;
		}
		arg = skipwhite(impl_end + 1);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_strings(&ga_impl);
	    തിരികെ;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, വലുപ്പം(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, വലുപ്പം(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, വലുപ്പം(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, വലുപ്പം(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, വലുപ്പം(ufunc_T *), 10);

    class_T *cl = ശൂന്യ;
    class_T *extends_cl = ശൂന്യ;  // class from "extends" argument
    class_T **intf_classes = ശൂന്യ;
    പൂർണ്ണസംഖ്യ	    intf_എണ്ണം = 0;	// number of items in "intf_classes"
    പൂർണ്ണസംഖ്യ	    അക്കം_enum_മൂല്യങ്ങൾ = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    എങ്കിൽ (cl == ശൂന്യ)
	പോകുക cleanup;

    എങ്കിൽ (is_enum)
	cl->class_flags = CLASS_ENUM;
    അല്ലെങ്കിൽ എങ്കിൽ (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    അല്ലെങ്കിൽ എങ്കിൽ (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.length = (size_t)(name_end - name_start);
    cl->class_name.string = vim_strnsave(name_start, cl->class_name.length);
    എങ്കിൽ (cl->class_name.string == ശൂന്യ)
	പോകുക cleanup;

    cl->class_type.tt_type = VAR_CLASS;
    cl->class_type.tt_class = cl;
    cl->class_object_type.tt_type = VAR_OBJECT;
    cl->class_object_type.tt_class = cl;

    eap->ea_class = cl;

    // Add the class to the script-local variables.
    // TODO: handle other context, e.g. in a function
    // TODO: does uf_hash need to be cleared?
    typval_T tv;
    tv.v_type = VAR_CLASS;
    tv.vval.v_class = cl;
    SOURCING_LNUM = start_lnum;
    പൂർണ്ണസംഖ്യ rc = set_var_const(cl->class_name.string, 0, ശൂന്യ, &tv, FALSE, 0, 0);
    എങ്കിൽ (rc == FAIL)
	പോകുക cleanup;

    എങ്കിൽ (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	എങ്കിൽ (add_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, ശൂന്യ) == FAIL)
	    പോകുക cleanup;

	varname = (char_u *)"ordinal";
	എങ്കിൽ (add_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_സംഖ്യ, ശൂന്യ) == FAIL)
	    പോകുക cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    പൂർണ്ണസംഖ്യ		enum_end = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = ശൂന്യ;
    പൂർണ്ണസംഖ്യ success = FALSE;
    വേണ്ടി (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	എങ്കിൽ (theline == ശൂന്യ)
	    നിർത്തുക;
	char_u *വരി = skipwhite(theline);

	// Skip empty and comment lines.
	എങ്കിൽ (*വരി == NUL)
	    തുടരുക;
	എങ്കിൽ (*വരി == '#')
	{
	    എങ്കിൽ (vim9_bad_comment(വരി))
		നിർത്തുക;
	    തുടരുക;
	}

	char_u *p = വരി;

	അക്ഷരം	*end_name;
	പൂർണ്ണസംഖ്യ	shortlen;
	പൂർണ്ണസംഖ്യ	fullen;
	എങ്കിൽ (is_class)
	{
	    end_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (is_enum)
	{
	    end_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	അല്ലെങ്കിൽ
	{
	    end_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	വേണ്ടി (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_start = p;
	എങ്കിൽ (checkforcmd(&p, end_name, shortlen))
	{
	    എങ്കിൽ (STRNCMP(cmd_start, end_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_start);
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '|' || !ends_excmd2(വരി, p))
		semsg(_(e_trailing_characters_str), p);
	    അല്ലെങ്കിൽ
		success = TRUE;
	    നിർത്തുക;
	}

	പൂർണ്ണസംഖ്യ	wrong_endname = FALSE;
	എങ്കിൽ (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	അല്ലെങ്കിൽ എങ്കിൽ (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	അല്ലെങ്കിൽ
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	എങ്കിൽ (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), വരി, end_name);
	    നിർത്തുക;
	}

	എങ്കിൽ (is_enum && !enum_end)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    എങ്കിൽ (enum_parse_മൂല്യങ്ങൾ(eap, cl, വരി, &classmembers,
					&അക്കം_enum_മൂല്യങ്ങൾ, &enum_end) == FAIL)
		നിർത്തുക;

	    എങ്കിൽ (enum_end)
	    {
		// Add the enum "values" class variable.
		enum_add_മൂല്യങ്ങൾ_member(cl, &classmembers, അക്കം_enum_മൂല്യങ്ങൾ,
							&type_list);
		added_enum_മൂല്യങ്ങൾ = TRUE;
	    }
	    തുടരുക;
	}

	പൂർണ്ണസംഖ്യ has_public = FALSE;
	എങ്കിൽ (checkforcmd(&p, "public", 3))
	{
	    എങ്കിൽ (STRNCMP(വരി, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), വരി);
		നിർത്തുക;
	    }
	    എങ്കിൽ (is_interface)
	    {
		emsg(_(e_public_variable_not_supported_in_interface));
		നിർത്തുക;
	    }
	    has_public = TRUE;
	    p = skipwhite(വരി + 6);

	    എങ്കിൽ (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_public_keyword_not_supported_for_method));
		നിർത്തുക;
	    }

	    എങ്കിൽ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_public_must_be_followed_by_var_static_final_or_const));
		നിർത്തുക;
	    }
	}

	പൂർണ്ണസംഖ്യ abstract_method = FALSE;
	char_u *pa = p;
	എങ്കിൽ (checkforcmd(&p, "abstract", 3))
	{
	    എങ്കിൽ (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		നിർത്തുക;
	    }

	    എങ്കിൽ (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_used_in_enum));
		നിർത്തുക;
	    }

	    എങ്കിൽ (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_used_in_interface));
		നിർത്തുക;
	    }

	    എങ്കിൽ (!is_abstract)
	    {
		semsg(_(e_abstract_method_in_concrete_class), pa);
		നിർത്തുക;
	    }

	    p = skipwhite(pa + 8);
	    എങ്കിൽ (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		നിർത്തുക;
	    }

	    abstract_method = TRUE;
	}

	പൂർണ്ണസംഖ്യ has_static = FALSE;
	char_u *ps = p;
	എങ്കിൽ (checkforcmd(&p, "static", 4))
	{
	    എങ്കിൽ (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		നിർത്തുക;
	    }

	    എങ്കിൽ (is_interface)
	    {
		emsg(_(e_static_member_not_supported_in_interface));
		നിർത്തുക;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    എങ്കിൽ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		നിർത്തുക;
	    }
	}

	പൂർണ്ണസംഖ്യ has_final = FALSE;
	പൂർണ്ണസംഖ്യ has_var = FALSE;
	പൂർണ്ണസംഖ്യ has_const = FALSE;
	എങ്കിൽ (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (checkforcmd(&p, "final", 5))
	{
	    എങ്കിൽ (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_in_interface));
		നിർത്തുക;
	    }
	    has_final = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (checkforcmd(&p, "const", 5))
	{
	    എങ്കിൽ (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_in_interface));
		നിർത്തുക;
	    }
	    has_const = TRUE;
	}
	p = skipwhite(p);

	// object members (public, read access, private):
	//	"var _varname"
	//	"var varname"
	//	"public var varname"
	//	"final _varname"
	//	"final varname"
	//	"public final varname"
	//	"const _varname"
	//	"const varname"
	//	"public const varname"
	// class members (public, read access, private):
	//	"static var _varname"
	//	"static var varname"
	//	"public static var varname"
	//	"static final _varname"
	//	"static final varname"
	//	"public static final varname"
	//	"static const _varname"
	//	"static const varname"
	//	"public static const varname"
	എങ്കിൽ (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_end = ശൂന്യ;
	    type_T *type = ശൂന്യ;
	    char_u *init_വ്യഞ്ജകം = ശൂന്യ;
	    പൂർണ്ണസംഖ്യ	    has_type = FALSE;

	    എങ്കിൽ (!eval_isnamec1(*p))
	    {
		എങ്കിൽ (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), വരി);
		അല്ലെങ്കിൽ
		    semsg(_(e_invalid_object_variable_declaration_str), വരി);
		നിർത്തുക;
	    }

	    എങ്കിൽ (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_in_interface),
			varname);
		നിർത്തുക;
	    }

	    എങ്കിൽ (parse_member(eap, വരി, varname, has_public,
			  &varname_end, &has_type, &type_list, &type,
			  !is_interface ? &init_വ്യഞ്ജകം: ശൂന്യ) == FAIL)
		നിർത്തുക;

	    എങ്കിൽ (is_reserved_varname(varname, varname_end)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_end))
	    {
		vim_free(init_വ്യഞ്ജകം);
		നിർത്തുക;
	    }
	    എങ്കിൽ (add_member(has_static ? &classmembers : &objmembers, varname,
				varname_end, has_public, has_final, has_const,
				has_type, type, init_വ്യഞ്ജകം) == FAIL)
	    {
		vim_free(init_വ്യഞ്ജകം);
		നിർത്തുക;
	    }
	}

	// constructors:
	//	  def new()
	//	  enddef
	//	  def newOther()
	//	  enddef
	// object methods and class functions:
	//	  def SomeMethod()
	//	  enddef
	//	  static def ClassFunction()
	//	  enddef
	//	  def someMethod<typeA, typeB>()
	//	  enddef
	അല്ലെങ്കിൽ എങ്കിൽ (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	വരികൾ_to_free;
	    പൂർണ്ണസംഖ്യ		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    എങ്കിൽ (has_public)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_public_keyword_not_supported_for_method));
		നിർത്തുക;
	    }

	    എങ്കിൽ (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_in_class_str), വരി);
		നിർത്തുക;
	    }

	    എങ്കിൽ (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_method_not_supported_in_interface), p);
		നിർത്തുക;
	    }

	    എങ്കിൽ (has_static && !is_new && SAFE_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_method_not_supported), p);
		നിർത്തുക;
	    }

	    CLEAR_FIELD(ea);
	    ea.cmd = വരി;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&വരികൾ_to_free, വലുപ്പം(char_u *), 50);
	    പൂർണ്ണസംഖ്യ class_flags;
	    എങ്കിൽ (is_interface)
		class_flags = CF_INTERFACE;
	    അല്ലെങ്കിൽ
		class_flags = abstract_method ? CF_ABSTRACT_METHOD : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, ശൂന്യ, &വരികൾ_to_free,
			class_flags, objmembers.ga_data, objmembers.ga_len,
			ശൂന്യ);
	    ga_clear_strings(&വരികൾ_to_free);

	    എങ്കിൽ (uf != ശൂന്യ)
	    {
		എങ്കിൽ (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_method_not_supported_in_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    നിർത്തുക;
		}

		// check for builtin method
		എങ്കിൽ (!is_new && SAFE_islower(*uf->uf_name) &&
					!object_check_builtin_method_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    നിർത്തുക;
		}

		// Check the uf_name isn't used already.
		എങ്കിൽ (is_duplicate_method(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    നിർത്തുക;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		എങ്കിൽ (ga_grow(fgap, 1) == OK)
		{
		    എങ്കിൽ (is_new)
			uf->uf_flags |= FC_NEW;

		    എങ്കിൽ (abstract_method)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_data)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (is_class)
		semsg(_(e_not_valid_command_in_class_str), വരി);
	    അല്ലെങ്കിൽ എങ്കിൽ (is_enum)
		semsg(_(e_not_valid_command_in_enum_str), വരി);
	    അല്ലെങ്കിൽ
		semsg(_(e_not_valid_command_in_interface_str), വരി);
	    നിർത്തുക;
	}
    }

    എങ്കിൽ (theline == ശൂന്യ && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    എങ്കിൽ (success && is_enum && (അക്കം_enum_മൂല്യങ്ങൾ == 0 || !added_enum_മൂല്യങ്ങൾ))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_add_മൂല്യങ്ങൾ_member(cl, &classmembers, അക്കം_enum_മൂല്യങ്ങൾ,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    എങ്കിൽ (success && extends != ശൂന്യ)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    എങ്കിൽ (success && extends_cl != ശൂന്യ)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    എങ്കിൽ (success && extends_cl != ശൂന്യ)
    {
	എങ്കിൽ (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	അല്ലെങ്കിൽ
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    എങ്കിൽ (success && !is_abstract && extends_cl != ശൂന്യ
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, വലുപ്പം(class_T *), 5);

    എങ്കിൽ (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    എങ്കിൽ (success && extends_cl != ശൂന്യ)
	success = add_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_data;
    intf_എണ്ണം = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    എങ്കിൽ (success)
	success = check_func_arg_names(&classfunctions, &objmethods,
							&classmembers);

    എങ്കിൽ (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	എങ്കിൽ (extends_cl != ശൂന്യ)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	എങ്കിൽ (add_members_to_class(&classmembers,
				 ശൂന്യ,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_എണ്ണം) == FAIL
		|| add_members_to_class(&objmembers,
				 extends_cl == ശൂന്യ ? ശൂന്യ
					       : extends_cl->class_obj_members,
				 extends_cl == ശൂന്യ ? 0
					  : extends_cl->class_obj_member_എണ്ണം,
				 &cl->class_obj_members,
				 &cl->class_obj_member_എണ്ണം) == FAIL)
	    പോകുക cleanup;

	എങ്കിൽ (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_എണ്ണം = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    എങ്കിൽ (cl->class_interfaces == ശൂന്യ)
		പോകുക cleanup;
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_data)[i];
	    VIM_CLEAR(ga_impl.ga_data);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = ശൂന്യ;
	}

	എങ്കിൽ (cl->class_interface_എണ്ണം > 0 || extends_cl != ശൂന്യ)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    എങ്കിൽ (add_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		പോകുക cleanup;
	}

	പൂർണ്ണസംഖ്യ	have_new = FALSE;
	ufunc_T	*class_func = ശൂന്യ;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_data)[i];
	    എങ്കിൽ (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		നിർത്തുക;
	    }
	}

	എങ്കിൽ (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	അല്ലെങ്കിൽ എങ്കിൽ ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    add_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	എങ്കിൽ (add_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    പോകുക cleanup;

	update_builtin_method_index(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	എങ്കിൽ ((is_class || is_enum) && cl->class_class_member_എണ്ണം > 0)
	    എങ്കിൽ (add_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		തിരികെ;
	    }

	cl->class_type_list = type_list;

	എങ്കിൽ (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	തിരികെ;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    എങ്കിൽ (intf_classes != ശൂന്യ)
    {
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < intf_എണ്ണം; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_strings(&ga_impl);

    വേണ്ടി (പൂർണ്ണസംഖ്യ round = 1; round <= 2; ++round)
    {
	garray_T *gap = round == 1 ? &classmembers : &objmembers;
	എങ്കിൽ (gap->ga_len == 0 || gap->ga_data == ശൂന്യ)
	    തുടരുക;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < classfunctions.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)classfunctions.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&classfunctions);

    clear_type_list(&type_list);
}

/*
 * Find member "name" in class "cl", set "member_idx" to the member index and
 * return its type.
 * When "is_object" is TRUE, then look for object members.  Otherwise look for
 * class members.
 * When not found "member_idx" is set to -1 and t_any is returned.
 * Set *p_m ocmmember_T if not NULL
 */
    type_T *
oc_member_type(
    class_T	*cl,
    പൂർണ്ണസംഖ്യ		is_object,
    char_u	*name,
    char_u	*name_end,
    പൂർണ്ണസംഖ്യ		*member_idx)
{
    size_t	len = name_end - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    എങ്കിൽ (m == ശൂന്യ)
    {
	member_not_found_സന്ദേശം(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	തിരികെ &t_any;
    }

    തിരികെ m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    പൂർണ്ണസംഖ്യ		is_object,
    പൂർണ്ണസംഖ്യ		member_idx)
{
    ocmember_T	*m;
    പൂർണ്ണസംഖ്യ		member_എണ്ണം;

    എങ്കിൽ (is_object)
    {
	m = cl->class_obj_members;
	member_എണ്ണം = cl->class_obj_member_എണ്ണം;
    }
    അല്ലെങ്കിൽ
    {
	m = cl->class_class_members;
	member_എണ്ണം = cl->class_class_member_എണ്ണം;
    }

    എങ്കിൽ (member_idx >= member_എണ്ണം)
	തിരികെ ശൂന്യ;

    തിരികെ m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    സ്ഥിതം ശൂന്യം
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    ശൂന്യം
typealias_unref(typealias_T *ta)
{
    എങ്കിൽ (ta != ശൂന്യ && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    ശൂന്യം
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    എങ്കിൽ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_in_vim9_script));
	തിരികെ;
    }

    എങ്കിൽ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	തിരികെ;
    }

    എങ്കിൽ (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_start_with_uppercase_letter_str), arg);
	തിരികെ;
    }

    char_u *name_end = find_name_end(arg, ശൂന്യ, ശൂന്യ, FNE_CHECK_START);
    എങ്കിൽ (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	തിരികെ;
    }
    char_u *name_start = arg;

    arg = skipwhite(name_end);
    എങ്കിൽ (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	തിരികെ;
    }
    എങ്കിൽ (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	തിരികെ;
    }
    arg++;
    arg = skipwhite(arg);

    എങ്കിൽ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	തിരികെ;
    }

    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, ശൂന്യ, ശൂന്യ, TRUE);
    എങ്കിൽ (type == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	തിരികെ;
    }

    പൂർണ്ണസംഖ്യ cc = *name_end;
    *name_end = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    എങ്കിൽ (eval_variable_import(name_start, &tv) == OK)
    {
	എങ്കിൽ (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_start);
	അല്ലെങ്കിൽ
	    semsg(_(e_redefining_script_item_str), name_start);
	clear_tv(&tv);
	പോകുക done;
    }

    // Create a script-local variable for the type alias.
    എങ്കിൽ (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_lock = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_start);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_depth = parse_type_depth();
    }
    അല്ലെങ്കിൽ
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_lock = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    set_var_const(name_start, current_sctx.sc_sid, ശൂന്യ, &tv, FALSE,
						ASSIGN_CONST | ASSIGN_FINAL, 0);

done:
    *name_end = cc;
}

/*
 * Returns OK if a member variable named "name" is present in the class "cl".
 * Otherwise returns FAIL.  If found, the member variable typval is set in
 * "rettv".  If "is_object" is TRUE, then the object member variable table is
 * searched.  Otherwise the class member variable table is searched.
 */
    പൂർണ്ണസംഖ്യ
get_member_tv(
    class_T	*cl,
    പൂർണ്ണസംഖ്യ		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    പൂർണ്ണസംഖ്യ		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    എങ്കിൽ (m == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (*name == '_' && (current_class == ശൂന്യ ||
				!class_instance_of(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	തിരികെ FAIL;
    }

    എങ്കിൽ (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	set_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    അല്ലെങ്കിൽ
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	set_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    തിരികെ OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
call_oc_method(
    class_T	*cl,
    char_u	*name,
    size_t	len,
    char_u	*name_end,
    evalarg_T	*evalarg,
    char_u	**arg,
    gfargs_tab_T	*gfatab,	// generic types
    typval_T	*rettv)
{
    ufunc_T	*fp;
    typval_T	argvars[MAX_FUNC_ARGS + 1];
    പൂർണ്ണസംഖ്യ		argcount = 0;
    ocmember_T	*ocm = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		m_idx;

    fp = method_lookup(cl, rettv->v_type, name, len, ശൂന്യ);
    എങ്കിൽ (fp == ശൂന്യ)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	എങ്കിൽ (ocm == ശൂന്യ || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    method_not_found_സന്ദേശം(cl, rettv->v_type, name, len);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	അല്ലെങ്കിൽ
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_end;
	തിരികെ OK;
    }

    എങ്കിൽ (ocm == ശൂന്യ && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
	തിരികെ FAIL;
    }

    // process generic function call
    എങ്കിൽ (fp != ശൂന്യ)
    {
	fp = generic_func_get(fp, gfatab);
	എങ്കിൽ (fp == ശൂന്യ)
	    തിരികെ FAIL;
    }

    char_u *argp = name_end;
    പൂർണ്ണസംഖ്യ ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    എങ്കിൽ (ret == FAIL)
    {
	ആകുമ്പോൾ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	തിരികെ FAIL;
    }

    funcexe_T funcexe;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    എങ്കിൽ (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    എങ്കിൽ (evalarg != ശൂന്യ)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    പൂർണ്ണസംഖ്യ error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								ശൂന്യ);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    എങ്കിൽ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_found_var);
	തിരികെ FAIL;
    }
    *arg = argp;

    തിരികെ OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    പൂർണ്ണസംഖ്യ
obj_method_to_partial_tv(object_T *obj, ufunc_T *obj_method, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    എങ്കിൽ (pt == ശൂന്യ)
	തിരികെ FAIL;

    pt->pt_refcount = 1;
    എങ്കിൽ (obj != ശൂന്യ)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_method;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    തിരികെ OK;
}

/*
 * Evaluate what comes after a class:
 * - class member: SomeClass.varname
 * - class function: SomeClass.SomeMethod()
 * - class constructor: SomeClass.new()
 * - object member: someObject.varname
 * - object method: someObject.SomeMethod()
 *
 * "*arg" points to the '.'.
 * "*arg" is advanced to after the member name or method call.
 *
 * Returns FAIL or OK.
 */
    പൂർണ്ണസംഖ്യ
class_object_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    പൂർണ്ണസംഖ്യ		verbose UNUSED)	// give error messages
{
    പൂർണ്ണസംഖ്യ	ret = FAIL;

    എങ്കിൽ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	തിരികെ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ശൂന്യ, ശൂന്യ, FNE_CHECK_START);
    എങ്കിൽ (name_end == name)
	തിരികെ FAIL;
    size_t len = name_end - name;

    പൂർണ്ണസംഖ്യ did_emsg_save = did_emsg;
    class_T *cl;
    എങ്കിൽ (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    അല്ലെങ്കിൽ // VAR_OBJECT
    {
	എങ്കിൽ (rettv->vval.v_object == ശൂന്യ)
	{
	    emsg(_(e_using_null_object));
	    തിരികെ FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    എങ്കിൽ (cl == ശൂന്യ)
    {
	emsg(_(e_incomplete_type));
	തിരികെ FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    എങ്കിൽ (*name_end == '<')
    {
	cctx_T	    *cctx = ശൂന്യ;

	എങ്കിൽ (evalarg != ശൂന്യ)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_end = parse_generic_func_type_args(name, len, name + len,
						&gfatab, cctx);
	എങ്കിൽ (name_end == ശൂന്യ)
	    പോകുക done;
    }

    എങ്കിൽ (*name_end == '(')
	// Invoke the class or object method
	ret = call_oc_method(cl, name, len, name_end, evalarg, arg,
							&gfatab, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	പൂർണ്ണസംഖ്യ is_object = rettv->v_type == VAR_OBJECT;
	എങ്കിൽ (get_member_tv(cl, is_object, name, len, ശൂന്യ, rettv) == OK)
	{
	    *arg = name_end;
	    ret = OK;
	    പോകുക done;
	}

	// could be a class method or an object method
	പൂർണ്ണസംഖ്യ	fidx;
	ufunc_T	*fp = method_lookup(cl, rettv->v_type, name, len, &fidx);
	എങ്കിൽ (fp != ശൂന്യ)
	{
	    // Protected methods are not accessible outside the class
	    എങ്കിൽ (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		പോകുക done;
	    }

	    എങ്കിൽ (obj_method_to_partial_tv(is_object ? rettv->vval.v_object :
						ശൂന്യ, fp, rettv) == FAIL)
		പോകുക done;

	    *arg = name_end;
	    ret = OK;
	    പോകുക done;
	}

	എങ്കിൽ (did_emsg == did_emsg_save)
	    member_not_found_സന്ദേശം(cl, rettv->v_type, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);

    തിരികെ ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
find_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ശൂന്യ, ശൂന്യ, FNE_CHECK_START);
    എങ്കിൽ (name_end == name || *name_end != '.')
	തിരികെ ശൂന്യ;

    ufunc_T	*fp = ശൂന്യ;
    size_t	len = name_end - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    എങ്കിൽ (eval_variable(name, (പൂർണ്ണസംഖ്യ)len,
				    0, &tv, ശൂന്യ, EVAL_VAR_NOAUTOLOAD) == FAIL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	പോകുക fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    എങ്കിൽ (cl == ശൂന്യ)
	പോകുക fail_after_eval;
    char_u *fname = name_end + 1;
    char_u *fname_end = find_name_end(fname, ശൂന്യ, ശൂന്യ, FNE_CHECK_START);
    എങ്കിൽ (fname_end == fname)
	പോകുക fail_after_eval;
    len = fname_end - fname;

    fp = method_lookup(cl, tv.v_type, fname, len, ശൂന്യ);

    എങ്കിൽ (fp != ശൂന്യ)
    {
	fp = eval_generic_func(fp, fname, &fname_end);
	എങ്കിൽ (fp != ശൂന്യ)
	    *arg = fname_end;
    }

fail_after_eval:
    clear_tv(&tv);
    തിരികെ fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    പൂർണ്ണസംഖ്യ
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    പൂർണ്ണസംഖ്യ idx;
    class_member_lookup(cl, name, namelen, &idx);
    തിരികെ idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, പൂർണ്ണസംഖ്യ *idx)
{
    ocmember_T	*ret_m = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		ret_idx = -1;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_class_member_എണ്ണം; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	എങ്കിൽ (namelen)
	{
	    എങ്കിൽ (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    നിർത്തുക;
	}
    }
    എങ്കിൽ (idx != ശൂന്യ)
	*idx = ret_idx;
    തിരികെ ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    സ്ഥിതം ufunc_T *
class_method_lookup(class_T *cl, char_u *name, size_t namelen, പൂർണ്ണസംഖ്യ *idx)
{
    ufunc_T	*ret_fp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		ret_idx = -1;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_class_function_എണ്ണം; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	എങ്കിൽ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    നിർത്തുക;
	}
    }
    എങ്കിൽ (idx != ശൂന്യ)
	*idx = ret_idx;
    തിരികെ ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    പൂർണ്ണസംഖ്യ
class_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    പൂർണ്ണസംഖ്യ idx;
    class_method_lookup(cl, name, namelen, &idx);
    തിരികെ idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    പൂർണ്ണസംഖ്യ idx;
    object_member_lookup(cl, name, namelen, &idx);
    തിരികെ idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, പൂർണ്ണസംഖ്യ *idx)
{
    ocmember_T	*ret_m = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		ret_idx = -1;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_obj_member_എണ്ണം; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	എങ്കിൽ (namelen)
	{
	    എങ്കിൽ (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    നിർത്തുക;
	}
    }
    എങ്കിൽ (idx != ശൂന്യ)
	*idx = ret_idx;
    തിരികെ ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    സ്ഥിതം ufunc_T *
object_method_lookup(class_T *cl, char_u *name, size_t namelen, പൂർണ്ണസംഖ്യ *idx)
{
    ufunc_T	*ret_fp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		ret_idx = -1;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_obj_method_എണ്ണം; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	എങ്കിൽ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    നിർത്തുക;
	}
    }
    എങ്കിൽ (idx != ശൂന്യ)
	*idx = ret_idx;
    തിരികെ ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    പൂർണ്ണസംഖ്യ
object_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    പൂർണ്ണസംഖ്യ idx;
    object_method_lookup(cl, name, namelen, &idx);
    തിരികെ idx;
}

/*
 * Lookup a class or object member variable by name.  If v_type is VAR_CLASS,
 * then lookup a class member variable and if it is VAR_OBJECT, then lookup a
 * object member variable.
 *
 * Returns a pointer to the member variable structure if variable is found.
 * Otherwise returns NULL.  The member variable index is set in "*idx".
 */
    ocmember_T *
member_lookup(
    class_T	*cl,
    vartype_T	v_type,
    char_u	*name,
    size_t	namelen,
    പൂർണ്ണസംഖ്യ		*idx)
{
    എങ്കിൽ (v_type == VAR_CLASS)
	തിരികെ class_member_lookup(cl, name, namelen, idx);

    തിരികെ object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    സ്ഥിതം class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_found = ശൂന്യ;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_found = ശൂന്യ;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    വേണ്ടി (class_T *super = cl; super != ശൂന്യ; super = super->class_extends)
    {
	class_T		*cl_tmp = ശൂന്യ;
	ocmember_T	*m = ശൂന്യ;
	എങ്കിൽ (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    എങ്കിൽ ((m = object_member_lookup(super, name, len, ശൂന്യ)) != ശൂന്യ)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	എങ്കിൽ (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    എങ്കിൽ (( m = class_member_lookup(super, name, len, ശൂന്യ)) != ശൂന്യ)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	എങ്കിൽ (cl_tmp == ശൂന്യ)
	    നിർത്തുക;  // member is not in this or any super class.
	cl_found = cl_tmp;
	m_found = m;
    }
    എങ്കിൽ (p_m != ശൂന്യ)
	*p_m = m_found;
    തിരികെ cl_found;
}

/*
 * Lookup a class or object method by name.  If v_type is VAR_CLASS, then
 * lookup a class method and if it is VAR_OBJECT, then lookup a object method.
 *
 * Returns a pointer to the method structure if variable is found.
 * Otherwise returns NULL.  The method variable index is set in "*idx".
 */
    ufunc_T *
method_lookup(
    class_T	*cl,
    vartype_T	v_type,
    char_u	*name,
    size_t	namelen,
    പൂർണ്ണസംഖ്യ		*idx)
{
    എങ്കിൽ (v_type == VAR_CLASS)
	തിരികെ class_method_lookup(cl, name, namelen, idx);
    അല്ലെങ്കിൽ
	തിരികെ object_method_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    പൂർണ്ണസംഖ്യ
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    വേണ്ടി (cctx_T *cctx = cctx_arg; cctx != ശൂന്യ; cctx = cctx->ctx_outer)
	എങ്കിൽ (cctx->ctx_ufunc != ശൂന്യ
			&& class_instance_of(cctx->ctx_ufunc->uf_class, cl))
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    പൂർണ്ണസംഖ്യ
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    എങ്കിൽ (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_in_class_str),
		m->ocm_name.string, cl->class_name.string);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    ശൂന്യം
obj_lock_const_vars(object_T *obj)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < obj->obj_class->class_obj_member_എണ്ണം; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	എങ്കിൽ (ocm->ocm_flags & OCMFLAG_CONST)
	{
	    typval_T *mtv = ((typval_T *)(obj + 1)) + i;
	    item_lock(mtv, DICT_MAXNEST, TRUE, TRUE);
	}
    }
}

/*
 * Create a new instance of class "cl"
 */
    object_T *
alloc_object(class_T *cl)
{
    object_T	*obj;
    പൂർണ്ണസംഖ്യ		sz;

    എങ്കിൽ (cl == ശൂന്യ)
	തിരികെ ശൂന്യ;

    sz = വലുപ്പം(object_T) + cl->class_obj_member_എണ്ണം * വലുപ്പം(typval_T);
    obj = alloc_clear(sz);
    എങ്കിൽ (obj == ശൂന്യ)
	തിരികെ ശൂന്യ;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    എങ്കിൽ (IS_ENUM(cl))
	enum_set_internal_obj_vars(cl, obj);

    തിരികെ obj;
}

/*
 * Make a copy of an object.
 */
    ശൂന്യം
copy_object(typval_T *from, typval_T *to)
{
    എങ്കിൽ (from->vval.v_object == ശൂന്യ)
	to->vval.v_object = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	to->vval.v_object = from->vval.v_object;
	++to->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    ശൂന്യം
copy_class(typval_T *from, typval_T *to)
{
    എങ്കിൽ (from->vval.v_class == ശൂന്യ)
	to->vval.v_class = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	to->vval.v_class = from->vval.v_class;
	++to->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    സ്ഥിതം ശൂന്യം
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_interface_എണ്ണം; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	എങ്കിൽ (cl->class_interfaces_cl[i] != ശൂന്യ)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *next;
    വേണ്ടി (itf2class_T *i2c = cl->class_itf2class; i2c != ശൂന്യ; i2c = next)
    {
	next = i2c->i2c_next;
	vim_free(i2c);
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_class_member_എണ്ണം; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	എങ്കിൽ (cl->class_members_tv != ശൂന്യ)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_obj_member_എണ്ണം; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_class_function_എണ്ണം; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	എങ്കിൽ (IS_ENUM(cl) && IS_CONSTRUCTOR_METHOD(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_obj_method_എണ്ണം; ++i)
    {
	ufunc_T *uf = cl->class_obj_methods[i];
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_obj_methods);

    clear_type_list(&cl->class_type_list);

    class_cleared(cl);

    vim_free(cl);
}

/*
 * Returns the number of references from the class members of class "cl" to cl"
 * itself.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
class_get_selfrefs(class_T *cl)
{
    പൂർണ്ണസംഖ്യ		self_refs = 0;
    typval_T	*tv;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_class_member_എണ്ണം; ++i)
    {
	tv = &cl->class_members_tv[i];
	എങ്കിൽ (tv->v_type == VAR_OBJECT && tv->vval.v_object != ശൂന്യ
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    തിരികെ self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
can_free_enum(class_T *cl)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_class_member_എണ്ണം; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	എങ്കിൽ (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    തിരികെ TRUE;

	എങ്കിൽ (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    എങ്കിൽ (tv->vval.v_list->lv_refcount > 1)
		തിരികെ FALSE;
	    നിർത്തുക;
	}

	എങ്കിൽ (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    തിരികെ FALSE;
    }

    തിരികെ TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
can_free_class(class_T *cl)
{
    പൂർണ്ണസംഖ്യ		can_free = FALSE;
    പൂർണ്ണസംഖ്യ		self_refs = 0;

    എങ്കിൽ (IS_ENUM(cl) && !can_free_enum(cl))
	തിരികെ FALSE;

    എങ്കിൽ (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    എങ്കിൽ (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    തിരികെ can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    ശൂന്യം
class_unref(class_T *cl)
{
    എങ്കിൽ (cl == ശൂന്യ)
	തിരികെ;

    --cl->class_refcount;

    എങ്കിൽ (cl->class_name.string == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    പൂർണ്ണസംഖ്യ
class_free_nonref(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		did_free = FALSE;

    വേണ്ടി (class_T *cl = first_class; cl != ശൂന്യ; cl = next_nonref_class)
    {
	next_nonref_class = cl->class_next_used;
	എങ്കിൽ ((cl->class_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    next_nonref_class = ശൂന്യ;
    തിരികെ did_free;
}

    പൂർണ്ണസംഖ്യ
set_ref_in_classes(പൂർണ്ണസംഖ്യ copyID)
{
    വേണ്ടി (class_T *cl = first_class; cl != ശൂന്യ; cl = cl->class_next_used)
	set_ref_in_item_class(cl, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);

    തിരികെ FALSE;
}

സ്ഥിതം object_T *first_object = ശൂന്യ;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    ശൂന്യം
object_created(object_T *obj)
{
    എങ്കിൽ (first_object != ശൂന്യ)
    {
	obj->obj_next_used = first_object;
	first_object->obj_prev_used = obj;
    }
    first_object = obj;
}

/*
 * Call this function when an object has been cleared and is about to be freed.
 * It is removed from the list headed by "first_object".
 */
    സ്ഥിതം ശൂന്യം
object_cleared(object_T *obj)
{
    എങ്കിൽ (obj->obj_next_used != ശൂന്യ)
	obj->obj_next_used->obj_prev_used = obj->obj_prev_used;
    എങ്കിൽ (obj->obj_prev_used != ശൂന്യ)
	obj->obj_prev_used->obj_next_used = obj->obj_next_used;
    അല്ലെങ്കിൽ എങ്കിൽ (first_object == obj)
	first_object = obj->obj_next_used;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    സ്ഥിതം ശൂന്യം
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    എങ്കിൽ (!cl)
	തിരികെ;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = INT_MAX;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_obj_member_എണ്ണം; ++i)
	clear_tv(tv + i);
}

    സ്ഥിതം ശൂന്യം
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    എങ്കിൽ (!cl)
	തിരികെ;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    സ്ഥിതം ശൂന്യം
object_free(object_T *obj)
{
    എങ്കിൽ (in_free_unref_ഇനങ്ങൾ)
	തിരികെ;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    ശൂന്യം
object_unref(object_T *obj)
{
    എങ്കിൽ (obj != ശൂന്യ && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    പൂർണ്ണസംഖ്യ
object_free_nonref(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		did_free = FALSE;

    വേണ്ടി (object_T *obj = first_object; obj != ശൂന്യ; obj = obj->obj_next_used)
    {
	എങ്കിൽ ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    തിരികെ did_free;
}

    ശൂന്യം
object_free_ഇനങ്ങൾ(പൂർണ്ണസംഖ്യ copyID)
{
    object_T	*obj_next;

    വേണ്ടി (object_T *obj = first_object; obj != ശൂന്യ; obj = obj_next)
    {
	obj_next = obj->obj_next_used;
	എങ്കിൽ ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    ശൂന്യം
emsg_var_cl_define(അക്ഷരം *സന്ദേശം, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    എങ്കിൽ (cl_def != ശൂന്യ)
	semsg(_(സന്ദേശം), m->ocm_name.string, cl_def->class_name.string);
    അല്ലെങ്കിൽ
	emsg(_(e_internal_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    ശൂന്യം
method_not_found_സന്ദേശം(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *method_name = vim_strnsave(name, len);
    എങ്കിൽ ((v_type == VAR_OBJECT)
	    && (class_method_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	എങ്കിൽ (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	അല്ലെങ്കിൽ
	    semsg(_(e_class_method_str_accessible_only_using_class_str),
		    method_name, cl->class_name.string);
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((v_type == VAR_CLASS)
	    && (object_method_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	എങ്കിൽ (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	അല്ലെങ്കിൽ
	    semsg(_(e_object_method_str_accessible_only_using_object_str),
		    method_name, cl->class_name.string);
    }
    അല്ലെങ്കിൽ
	semsg(_(e_method_not_found_on_class_str_str), method_name,
		cl->class_name.string);
    vim_free(method_name);
}

/*
 * Echo a class or object member not found message.
 */
    ശൂന്യം
member_not_found_സന്ദേശം(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    എങ്കിൽ (v_type == VAR_OBJECT)
    {
	എങ്കിൽ (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	അല്ലെങ്കിൽ
	    semsg(_(e_variable_not_found_on_object_str_str), varname,
		    cl->class_name.string);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (IS_ENUM(cl))
		semsg(_(e_enum_മൂല്യം_str_not_found_in_enum_str),
			varname, cl->class_name.string);
	    അല്ലെങ്കിൽ
		semsg(_(e_class_variable_str_not_found_in_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    ശൂന്യം
defcompile_class(class_T *cl)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ loop = 1; loop <= 2; ++loop)
    {
	പൂർണ്ണസംഖ്യ func_എണ്ണം = loop == 1 ? cl->class_class_function_എണ്ണം
						: cl->class_obj_method_എണ്ണം;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < func_എണ്ണം; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    എങ്കിൽ (!IS_ABSTRACT_METHOD(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    ശൂന്യം
defcompile_classes_in_script(ശൂന്യം)
{
    വേണ്ടി (class_T *cl = first_class; cl != ശൂന്യ; cl = cl->class_next_used)
    {
	എങ്കിൽ (eval_variable(cl->class_name.string, 0, 0, ശൂന്യ, ശൂന്യ,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    പൂർണ്ണസംഖ്യ
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    എങ്കിൽ (eval_variable(name, 0, 0, rettv, ശൂന്യ, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	തിരികെ rettv->v_type == VAR_CLASS;
    തിരികെ FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
object_call_builtin_method(
    object_T		*obj,
    class_builtin_T	builtin_method,
    പൂർണ്ണസംഖ്യ			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    പൂർണ്ണസംഖ്യ	    midx;

    എങ്കിൽ (obj == ശൂന്യ)
	തിരികെ FAIL;

    uf = class_get_builtin_method(obj->obj_class, builtin_method, &midx);
    എങ്കിൽ (uf == ശൂന്യ)
	തിരികെ FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    പൂർണ്ണസംഖ്യ		r;

    എങ്കിൽ (fc == ശൂന്യ)
	തിരികെ FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, ശൂന്യ, obj, fc, rettv);

    remove_funccal();

    തിരികെ r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    പൂർണ്ണസംഖ്യ
object_empty(object_T *obj)
{
    typval_T	rettv;

    എങ്കിൽ (object_call_builtin_method(obj, CLASS_BUILTIN_EMPTY, 0, ശൂന്യ, &rettv)
								== FAIL)
	തിരികെ TRUE;

    തിരികെ tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    പൂർണ്ണസംഖ്യ
object_len(object_T *obj)
{
    typval_T	rettv;

    എങ്കിൽ (object_call_builtin_method(obj, CLASS_BUILTIN_LEN, 0, ശൂന്യ, &rettv)
								== FAIL)
	തിരികെ 0;

    തിരികെ tv_to_സംഖ്യ(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    പൂർണ്ണസംഖ്യ
object_equal(
	object_T *o1,
	object_T *o2,
	പൂർണ്ണസംഖ്യ	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    എങ്കിൽ (o1 == o2)
	തിരികെ TRUE;
    എങ്കിൽ (o1 == ശൂന്യ || o2 == ശൂന്യ)
	തിരികെ FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    എങ്കിൽ (cl1 != cl2 || cl1 == ശൂന്യ || cl2 == ശൂന്യ)
	തിരികെ FALSE;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl1->class_obj_member_എണ്ണം; ++i)
	എങ്കിൽ (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    തിരികെ FALSE;

    തിരികെ TRUE;
}

/*
 * Return a textual representation of object "obj".
 * "obj" must not be NULL.
 * May return NULL.
 */
    char_u *
object2string(
    object_T	*obj,
    char_u	*numbuf,
    പൂർണ്ണസംഖ്യ		copyID,
    പൂർണ്ണസംഖ്യ		echo_style,
    പൂർണ്ണസംഖ്യ		restore_copyID,
    പൂർണ്ണസംഖ്യ		composite_val)
{
    typval_T	rettv;

    എങ്കിൽ (object_call_builtin_method(obj, CLASS_BUILTIN_STRING, 0, ശൂന്യ, &rettv)
								== OK
					&& rettv.vval.v_string != ശൂന്യ)
	തിരികെ rettv.vval.v_string;

    പൂർണ്ണസംഖ്യ		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    എങ്കിൽ (cl != ശൂന്യ && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    അല്ലെങ്കിൽ
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	എങ്കിൽ (cl == ശൂന്യ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	അല്ലെങ്കിൽ
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
    }
    എങ്കിൽ (cl != ശൂന്യ)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < cl->class_obj_member_എണ്ണം; ++i)
	{
	    എങ്കിൽ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.length);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = ശൂന്യ;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, composite_val);
	    എങ്കിൽ (s != ശൂന്യ)
		ga_concat(&ga, s);
	    vim_free(tf);
	    എങ്കിൽ (s == ശൂന്യ || did_echo_string_emsg)
	    {
		ok = FAIL;
		നിർത്തുക;
	    }
	    വരി_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    എങ്കിൽ (ok == FAIL)
    {
	vim_free(ga.ga_data);
	തിരികെ ശൂന്യ;
    }
    ga_append(&ga, NUL);
    തിരികെ (char_u *)ga.ga_data;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    പൂർണ്ണസംഖ്യ
class_instance_of(class_T *cl, class_T *other_cl)
{
    എങ്കിൽ (cl == other_cl)
	തിരികെ TRUE;

    // Recursively check the base classes.
    വേണ്ടി (; cl != ശൂന്യ; cl = cl->class_extends)
    {
	എങ്കിൽ (cl == other_cl)
	    തിരികെ TRUE;
	// Check the implemented interfaces and the super interfaces
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = cl->class_interface_എണ്ണം - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    ആകുമ്പോൾ (intf != ശൂന്യ)
	    {
		എങ്കിൽ (intf == other_cl)
		    തിരികെ TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    തിരികെ FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    ശൂന്യം
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_സംഖ്യ = VVAL_FALSE;

    എങ്കിൽ (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_args(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (object_tv->vval.v_object == ശൂന്യ)
    {
	എങ്കിൽ (classinfo_tv->vval.v_class == ശൂന്യ)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_സംഖ്യ = VVAL_TRUE;
	തിരികെ;
    }

    വേണ്ടി (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	എങ്കിൽ (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	അല്ലെങ്കിൽ
	    c = classinfo_tv->vval.v_class;

	എങ്കിൽ (class_instance_of(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_സംഖ്യ = VVAL_TRUE;
	    തിരികെ;
	}
    }
}

#endif // FEAT_EVAL
