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
#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

நிலையான class_T *first_class = வெற்று;
நிலையான class_T *next_nonref_class = வெற்று;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    நிலையான வெறுமை
class_created(class_T *cl)
{
    எனில் (first_class != வெற்று)
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
    நிலையான வெறுமை
class_cleared(class_T *cl)
{
    எனில் (cl->class_next_used != வெற்று)
	cl->class_next_used->class_prev_used = cl->class_prev_used;
    எனில் (cl->class_prev_used != வெற்று)
	cl->class_prev_used->class_next_used = cl->class_next_used;
    இல்லையெனில் எனில் (first_class == cl)
	first_class = cl->class_next_used;

    // update the next class to check if needed
    எனில் (cl == next_nonref_class)
	next_nonref_class = cl->class_next_used;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    நிலையான முழு
parse_member(
    exarg_T	*eap,
    char_u	*வரி,
    char_u	*varname,
    முழு		has_public,	    // TRUE if "public" seen before "varname"
    char_u	**varname_end,
    முழு		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_கோவை)
{
    *varname_end = to_name_end(varname, FALSE);
    எனில் (*varname == '_' && has_public)
    {
	semsg(_(e_public_variable_name_cannot_start_with_underscore_str), வரி);
	திரும்பு FAIL;
    }

    char_u *colon = skipwhite(*varname_end);
    char_u *type_arg = colon;
    type_T *type = வெற்று;
    *has_type = FALSE;
    எனில் (*colon == ':')
    {
	எனில் (VIM_ISWHITE(**varname_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    திரும்பு FAIL;
	}
	எனில் (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    திரும்பு FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, வெற்று, வெற்று, TRUE);
	எனில் (type == வெற்று || !valid_declaration_type(type))
	    திரும்பு FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    எனில் (type == வெற்று && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	திரும்பு FAIL;
    }

    எனில் (init_கோவை == வெற்று && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_in_interface));
	திரும்பு FAIL;
    }

    எனில் (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *கோவை_start, *கோவை_end;

	எனில் (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    திரும்பு FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(வெறுமை)skip_கோவை_concatenate(&init_arg, &கோவை_start, &கோவை_end, &evalarg);

	init_arg = skipwhite(init_arg);
	எனில் (*init_arg != NUL && !vim9_comment_start(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, வெற்று);
	    திரும்பு FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	எனில் (type == வெற்று)
	    type = &t_any;

	*init_கோவை = vim_strnsave(கோவை_start, கோவை_end - கோவை_start);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, வெற்று);
    }
    இல்லையெனில் எனில் (!valid_declaration_type(type))
	திரும்பு FAIL;

    *type_ret = type;
    திரும்பு OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    நிலையான முழு
add_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_end,
    முழு		has_public,
    முழு		has_final,
    முழு		has_const,
    முழு		has_type,
    type_T	*type,
    char_u	*init_கோவை)
{
    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_data) + gap->ga_len;
    m->ocm_name.length = (size_t)(varname_end - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.length);
    எனில் (m->ocm_name.string == வெற்று)
    {
	m->ocm_name.length = 0;
	திரும்பு FAIL;
    }
    m->ocm_access = has_public ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_PRIVATE : VIM_ACCESS_READ;
    எனில் (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    எனில் (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    எனில் (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    எனில் (init_கோவை != வெற்று)
    {
	m->ocm_init = init_கோவை;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    திரும்பு OK;
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
    நிலையான முழு
add_members_to_class(
    garray_T	*gap,
    ocmember_T	*parent_members,
    முழு		parent_மொத்தம்,
    ocmember_T	**members,
    முழு		*member_மொத்தம்)
{
    *member_மொத்தம் = parent_மொத்தம் + gap->ga_len;
    *members = *member_மொத்தம் == 0 ? வெற்று
				       : ALLOC_MULT(ocmember_T, *member_மொத்தம்);
    எனில் (*member_மொத்தம் > 0 && *members == வெற்று)
	திரும்பு FAIL;
    க்கு (முழு i = 0; i < parent_மொத்தம்; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = parent_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.length);
	எனில் (m->ocm_name.string == வெற்று)
	{
	    m->ocm_name.length = 0;
	    திரும்பு FAIL;
	}
	எனில் (m->ocm_init != வெற்று)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    எனில் (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + parent_மொத்தம்,
			       gap->ga_data, அளவு(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_data);
    திரும்பு OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    முழு
object_index_from_itf_index(class_T *itf, முழு is_method, முழு idx, class_T *cl)
{
    எனில் (idx >= (is_method ? itf->class_obj_method_மொத்தம்
				   : itf->class_obj_member_மொத்தம்))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	திரும்பு 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    எனில் (cl == itf)
	திரும்பு idx;

    itf2class_T		*i2c = வெற்று;
    முழு			searching = TRUE;
    முழு			method_offset = 0;

    க்கு (class_T *super = cl; super != வெற்று && searching;
						super = super->class_extends)
    {
	க்கு (i2c = itf->class_itf2class; i2c != வெற்று; i2c = i2c->i2c_next)
	{
	    எனில் (i2c->i2c_class == super && i2c->i2c_is_method == is_method)
	    {
		searching = FALSE;
		நிறுத்து;
	    }
	}
	எனில் (searching && is_method)
	    // The parent class methods are stored after the current class
	    // methods.
	    method_offset += super->class_obj_method_மொத்தம்_child;
    }
    எனில் (i2c == வெற்று)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	திரும்பு 0;
    }

    // A table follows the i2c for the class
    முழு *table = (முழு *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    திரும்பு table[idx] + method_offset;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    நிலையான முழு
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    முழு	    is_class)
{
    typval_T	tv;
    முழு		success = FALSE;

    எனில் (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	திரும்பு success;
    }

    tv.v_type = VAR_UNKNOWN;
    எனில் (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_found_str), extends_name);
	திரும்பு success;
    }

    எனில் (tv.v_type != VAR_CLASS || tv.vval.v_class == வெற்று
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    இல்லையெனில்
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    திரும்பு success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    நிலையான முழு
validate_extends_generic_method(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    எழுத்து	*செய்தி = வெற்று;

    எனில் (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	திரும்பு TRUE;

    எனில் (super_fp->uf_generic_argcount == 0)
	செய்தி = e_concrete_method_str_override_with_generic_method_in_class_str;
    இல்லையெனில் எனில் (cl_fp->uf_generic_argcount == 0)
	செய்தி = e_generic_method_str_override_with_concrete_method_in_class_str;
    இல்லையெனில்
	செய்தி = e_generic_method_str_type_arguments_mismatch_in_class_str;

    semsg(_(செய்தி), cl_fp->uf_name, super_cl->class_name.string);
    திரும்பு FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    நிலையான முழு
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    முழு		method_மொத்தம் = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_data);

    போது (super != வெற்று)
    {
	முழு extends_method_மொத்தம் = super->class_obj_method_மொத்தம்_child;
	எனில் (extends_method_மொத்தம் == 0)
	{
	    super = super->class_extends;
	    தொடர்;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	க்கு (முழு i = 0; i < extends_method_மொத்தம்; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    முழு	    extends_private = (*pstr == '_');
	    எனில் (extends_private)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    க்கு (முழு j = 0; j < method_மொத்தம்; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		முழு	priv_method = (*qstr == '_');
		எனில் (priv_method)
		    qstr++;
		எனில் (STRCMP(pstr, qstr) == 0)
		{
		    எனில் (priv_method != extends_private)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_method_str_of_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			திரும்பு FALSE;
		    }

		    எனில் (!validate_extends_generic_method(super,
						extends_methods[i], cl_fp[j]))
			    திரும்பு FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    திரும்பு TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    நிலையான முழு
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    முழு member_மொத்தம் = objmembers_gap->ga_len;
    எனில் (member_மொத்தம் == 0)
	திரும்பு TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    க்கு (முழு c_i = 0; c_i < member_மொத்தம்; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	போது (p_cl != வெற்று)
	{
	    முழு p_member_மொத்தம் = p_cl->class_obj_member_மொத்தம்;

	    எனில் (p_member_மொத்தம் == 0)
	    {
		p_cl = p_cl->class_extends;
		தொடர்;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    க்கு (முழு p_i = 0; p_i < p_member_மொத்தம்; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		எனில் (STRCMP(pstr, qstr) == 0)
		{
		    எனில் (c_protected != p_protected)
			semsg(_(e_public_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    இல்லையெனில்
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    திரும்பு FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    திரும்பு TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    நிலையான முழு
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    முழு member_மொத்தம் = objmembers_gap->ga_len;
    எனில் (member_மொத்தம் == 0)
	திரும்பு TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    க்கு (முழு c_i = 0; c_i < member_மொத்தம்; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	முழு	    var_found = FALSE;

	// Check in all the parent classes in the lineage
	போது (p_cl != வெற்று && !var_found)
	{
	    முழு p_member_மொத்தம் = p_cl->class_obj_member_மொத்தம்;
	    எனில் (p_member_மொத்தம் == 0)
	    {
		p_cl = p_cl->class_extends;
		தொடர்;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    க்கு (முழு p_i = 0; p_i < p_member_மொத்தம்; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		எனில் (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    தொடர்;

		// Ensure the type is matching.
		where.wt_func_name = (எழுத்து *)c_m->ocm_name.string;
		where.wt_kind = WT_MEMBER;

		எனில் (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    திரும்பு FALSE;

		var_found = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    திரும்பு TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    நிலையான முழு
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    க்கு (முழு loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	முழு extends_method_மொத்தம் = loop == 1
				? extends_cl->class_class_function_மொத்தம்
				: extends_cl->class_obj_method_மொத்தம்;
	எனில் (extends_method_மொத்தம் == 0)
	    தொடர்;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	முழு method_மொத்தம் = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_data
						: objmethods_gap->ga_data);

	க்கு (முழு i = 0; i < extends_method_மொத்தம்; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    எனில் (!IS_ABSTRACT_METHOD(uf))
		தொடர்;

	    முழு	concrete_method_found = FALSE;
	    முழு	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    க்கு (j = 0; !concrete_method_found && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		எனில் (!IS_ABSTRACT_METHOD(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_method_found = TRUE;
	    }

	    எனில் (concrete_method_found)
		தொடர்;

	    க்கு (j = 0; j < method_மொத்தம்; j++)
	    {
		எனில் (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_method_found = TRUE;
		    நிறுத்து;
		}
	    }

	    எனில் (!concrete_method_found)
	    {
		semsg(_(e_abstract_method_str_not_implemented), uf->uf_name);
		திரும்பு FALSE;
	    }
	}
    }

    திரும்பு TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    நிலையான முழு
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    முழு		cl_member_மொத்தம்,
    class_T	*extends_cl)
{
    முழு		variable_present  = FALSE;

    க்கு (முழு cl_i = 0; cl_i < cl_member_மொத்தம்; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	எனில் (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    தொடர்;

	// Ensure the access type is same
	எனில் (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_of_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    திரும்பு FALSE;
	}

	// Ensure the type is matching.
	எனில் (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	இல்லையெனில்
	{
	    where.wt_func_name = (எழுத்து *)m->ocm_name.string;
	    where.wt_kind = WT_MEMBER;
	    எனில் (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		திரும்பு FALSE;
	}

	variable_present = TRUE;
	நிறுத்து;
    }

    எனில் (!variable_present && extends_cl != வெற்று)
    {
	முழு ext_cl_மொத்தம் = extends_cl->class_obj_member_மொத்தம்;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	திரும்பு intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_மொத்தம்,
					extends_cl->class_extends);
    }

    திரும்பு variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    நிலையான முழு
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    முழு if_மொத்தம் = ifcl->class_obj_member_மொத்தம்;
    எனில் (if_மொத்தம் == 0)
	திரும்பு TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_data);
    முழு cl_மொத்தம் = objmembers_gap->ga_len;
    க்கு (முழு if_i = 0; if_i < if_மொத்தம்; ++if_i)
    {
	எனில் (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_மொத்தம், extends_cl))
	{
	    semsg(_(e_variable_str_of_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    திரும்பு FALSE;
	}
    }

    திரும்பு TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    நிலையான முழு
intf_method_type_matches(
    ufunc_T	*if_method,
    ufunc_T	*cl_method,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    எனில் (cl_method->uf_generic_argcount != if_method->uf_generic_argcount)
    {
	semsg(_(e_type_var_மொத்தம்_of_method_str_differs_from_interface_str),
						cl_method->uf_name, intf_name);
	திரும்பு FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (எழுத்து *)if_method->uf_name;
    where.wt_kind = WT_METHOD;
    எனில் (check_type(if_method->uf_func_type, cl_method->uf_func_type, TRUE,
								where) == FAIL)
	திரும்பு FALSE;

    திரும்பு TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    நிலையான முழு
intf_method_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    முழு	    cl_மொத்தம்,
    class_T *extends_cl,
    char_u  *intf_name)
{
    முழு		method_present  = FALSE;

    க்கு (முழு cl_i = 0; cl_i < cl_மொத்தம்; ++cl_i)
    {
	எனில் (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    எனில் (!intf_method_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		திரும்பு FALSE;
	    method_present = TRUE;
	    நிறுத்து;
	}
    }

    எனில் (!method_present && extends_cl != வெற்று)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	முழு	ext_cl_மொத்தம் = extends_cl->class_obj_method_மொத்தம்;
	திரும்பு intf_method_present(if_ufunc, ext_cl_fp, ext_cl_மொத்தம்,
					extends_cl->class_extends, intf_name);
    }

    திரும்பு method_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    நிலையான முழு
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    முழு if_மொத்தம் = ifcl->class_obj_method_மொத்தம்;
    எனில் (if_மொத்தம் == 0)
	திரும்பு TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_data);
    முழு cl_மொத்தம் = objmethods_gap->ga_len;
    க்கு (முழு if_i = 0; if_i < if_மொத்தம்; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	எனில் (!intf_method_present(if_fp[if_i], cl_fp, cl_மொத்தம், extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_method_str_of_interface_str_not_implemented),
		    if_name, intf_class_name);
	    திரும்பு FALSE;
	}
    }

    திரும்பு TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    நிலையான முழு
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    முழு		success = TRUE;

    க்கு (முழு i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_data)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	எனில் (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_found_str), impl);
	    success = FALSE;
	    நிறுத்து;
	}

	எனில் (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == வெற்று
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    நிறுத்து;
	}

	class_T *ifcl = tv.vval.v_class;
	எனில் (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    நிறுத்து;
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

    எனில் (!success)
	திரும்பு FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_data;
    cl->class_interface_மொத்தம் = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    க்கு (முழு i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_data)[i],
		((class_T **)intf_classes_gap->ga_data)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = வெற்று;
    cl->class_interface_மொத்தம் = 0;
    cl->class_extends = வெற்று;

    திரும்பு success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    நிலையான முழு
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    க்கு (முழு j = 0; j < intf_classes_gap->ga_len; j++)
    {
	எனில் (((class_T **)intf_classes_gap->ga_data)[j] == ifcl)
	    திரும்பு TRUE;
    }

    திரும்பு FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    நிலையான முழு
add_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.length);
    எனில் (intf_name == வெற்று)
	திரும்பு FALSE;

    எனில் (ga_grow(impl_gap, 1) == FAIL)
	செல் fail;

    char_u **intf_names = (char_u **)impl_gap->ga_data;
    intf_names[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    எனில் (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	செல் fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_data;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    திரும்பு TRUE;
fail:
    vim_free(intf_name);
    திரும்பு FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    நிலையான முழு
add_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    க்கு (முழு i = 0; i < super->class_interface_மொத்தம்; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	எனில் (!is_interface_class_present(intf_classes_gap, ifcl))
	    add_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    திரும்பு TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    நிலையான முழு
check_func_arg_names(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    க்கு (முழு loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	க்கு (முழு fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_data)[fi];

	    க்கு (முழு i = 0; i < uf->uf_args.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_args.ga_data)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		க்கு (முழு mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_data + mi)->ocm_name.string;
		    எனில் (STRCMP(aname, mname) == 0)
		    {
			எனில் (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_argument_already_declared_in_class_str),
				aname);

			திரும்பு FALSE;
		    }
		}
	    }
	}
    }

    திரும்பு TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    நிலையான முழு
is_reserved_varname(char_u *varname, char_u *varname_end)
{
    முழு reserved = FALSE;
    char_u save_varname_end = *varname_end;
    *varname_end = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_end = save_varname_end;

    திரும்பு reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    நிலையான முழு
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_end)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_end - varname)};
    முழு		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    எனில் (*pstr.string == '_')
    {
	pstr.string++;
	pstr.length--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    க்கு (முழு loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	க்கு (முழு i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_data) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.length};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    எனில் (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.length--;
		q_protected = true;
	    }

	    எனில் (pstr.length == qstr.length
		&& STRNCMP(pstr.string, qstr.string, pstr.length) == 0)
	    {
		char_u	save_c = *varname_end;

		*varname_end = NUL;
		எனில் (p_protected != q_protected)
		    semsg(_(e_public_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		இல்லையெனில்
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_end = save_c;
		dup = TRUE;
		நிறுத்து;
	    }
	}
    }

    திரும்பு dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    நிலையான முழு
is_duplicate_method(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    க்கு (முழு loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	க்கு (முழு i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_data)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    எனில் (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		திரும்பு TRUE;
	    }
	}
    }

    திரும்பு FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    நிலையான முழு
is_valid_constructor(ufunc_T *uf, முழு is_abstract, முழு has_static)
{
    // Constructors are not allowed in abstract classes.
    எனில் (is_abstract)
    {
	emsg(_(e_cannot_define_new_method_in_abstract_class));
	திரும்பு FALSE;
    }
    // A constructor is always static, no need to define it so.
    எனில் (has_static)
    {
	emsg(_(e_cannot_define_new_method_as_static));
	திரும்பு FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    எனில் (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_method));
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    நிலையான முழு
object_check_builtin_method_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    முழு	    valid = FALSE;
    type_T  method_sig;
    type_T  method_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_FIELD(method_sig);
    CLEAR_FIELD(method_rt);
    method_sig.tt_type = VAR_FUNC;

    எனில் (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	method_rt.tt_type = VAR_எண்;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    இல்லையெனில் எனில் (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	method_rt.tt_type = VAR_BOOL;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    இல்லையெனில் எனில் (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	method_rt.tt_type = VAR_STRING;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    இல்லையெனில்
	semsg(_(e_builtin_object_method_str_not_supported), uf->uf_name);

    where.wt_func_name = (எழுத்து *)uf->uf_name;
    where.wt_kind = WT_METHOD;
    எனில் (valid && !check_type(&method_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    திரும்பு valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    முழு
is_valid_builtin_obj_methodname(char_u *funcname)
{
    தேர்வு (funcname[0])
    {
	நிலை 'e':
	    திரும்பு STRNCMP(funcname, "empty", 5) == 0;

	நிலை 'l':
	    திரும்பு STRNCMP(funcname, "len", 3) == 0;

	நிலை 'n':
	    திரும்பு STRNCMP(funcname, "new", 3) == 0;

	நிலை 's':
	    திரும்பு STRNCMP(funcname, "string", 6) == 0;
    }

    திரும்பு FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_method(
    class_T		*cl,
    class_builtin_T	builtin_method,
    முழு			*method_idx)
{
    *method_idx = -1;

    எனில் (cl == வெற்று)
	திரும்பு வெற்று;

    *method_idx = cl->class_builtin_methods[builtin_method];
    திரும்பு *method_idx != -1 ? cl->class_obj_methods[*method_idx] : வெற்று;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    நிலையான முழு
update_member_method_lookup_table(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    முழு		pobj_method_offset)
{
    எனில் (ifcl == வெற்று)
	திரும்பு OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(அளவு(itf2class_T)
				+ ifcl->class_obj_member_மொத்தம் * அளவு(முழு));
    எனில் (if2cl == வெற்று)
	திரும்பு FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = FALSE;

    க்கு (முழு if_i = 0; if_i < ifcl->class_obj_member_மொத்தம்; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	க்கு (முழு cl_i = 0; cl_i < cl->class_obj_member_மொத்தம்; ++cl_i)
	{
	    எனில் (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		முழு *table = (முழு *)(if2cl + 1);
		table[if_i] = cl_i;
		நிறுத்து;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(அளவு(itf2class_T)
				+ ifcl->class_obj_method_மொத்தம் * அளவு(முழு));
    எனில் (if2cl == வெற்று)
	திரும்பு FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = TRUE;

    க்கு (முழு if_i = 0; if_i < ifcl->class_obj_method_மொத்தம்; ++if_i)
    {
	முழு	done = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	க்கு (முழு cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    எனில் (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_data)[cl_i]->uf_name) == 0)
	    {
		முழு *table = (முழு *)(if2cl + 1);
		table[if_i] = cl_i;
		done = TRUE;
		நிறுத்து;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	எனில் (!done)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    எனில் (cl->class_extends != ifcl)
	    {
		class_T		*parent = cl->class_extends;
		முழு		method_offset = objmethods->ga_len;

		போது (!done && parent != வெற்று && parent != ifcl)
		{
		    க்கு (முழு cl_i = 0;
			    cl_i < parent->class_obj_method_மொத்தம்_child; ++cl_i)
		    {
			எனில் (STRCMP(if_name,
				    parent->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    முழு *table = (முழு *)(if2cl + 1);
			    table[if_i] = method_offset + cl_i;
			    done = TRUE;
			    நிறுத்து;
			}
		    }
		    method_offset += parent->class_obj_method_மொத்தம்_child;
		    parent = parent->class_extends;
		}
	    }

	    எனில் (!done)
	    {
		முழு *table = (முழு *)(if2cl + 1);
		table[if_i] = pobj_method_offset + if_i;
	    }
	}
    }

    திரும்பு OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    நிலையான முழு
add_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    க்கு (முழு i = 0; i < cl->class_interface_மொத்தம்; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	போது (ifcl != வெற்று)
	{
	    எனில் (update_member_method_lookup_table(ifcl, cl, objmethods_gap,
								0) == FAIL)
		திரும்பு FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    எனில் (extends_cl != வெற்று)
    {
	class_T		*pclass = extends_cl;
	முழு		pobj_method_offset = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	போது (pclass != வெற்று)
	{
	    எனில் (update_member_method_lookup_table(pclass, cl,
			objmethods_gap, pobj_method_offset) == FAIL)
		திரும்பு FAIL;

	    pobj_method_offset += pclass->class_obj_method_மொத்தம்_child;
	    pclass = pclass->class_extends;
	}
    }

    திரும்பு OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    நிலையான முழு
add_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_மொத்தம்);
    எனில் (cl->class_members_tv == வெற்று)
	திரும்பு FAIL;

    க்கு (முழு i = 0; i < cl->class_class_member_மொத்தம்; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	எனில் (m->ocm_init != வெற்று)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_கோவை(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    எனில் (etv == வெற்று)
		திரும்பு FAIL;

	    எனில் (m->ocm_type->tt_type == VAR_ANY
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
	இல்லையெனில்
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = வெற்று;
	}
	set_tv_type(tv, m->ocm_type);
	எனில் (m->ocm_flags & OCMFLAG_CONST)
	    item_lock(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    திரும்பு OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    நிலையான வெறுமை
add_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    முழு		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    க்கு (முழு i = 0; i < cl->class_obj_member_மொத்தம்; ++i)
    {
	எனில் (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    தொடர்;

	எனில் (i > (is_enum ? 2 : 0))
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

    garray_T வரிகள்_to_free;
    ga_init2(&வரிகள்_to_free, அளவு(char_u *), 50);

    ufunc_T *nf = define_function(&fea, வெற்று, &வரிகள்_to_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_மொத்தம்,
			    வெற்று);

    ga_clear_strings(&வரிகள்_to_free);
    vim_free(fga.ga_data);

    எனில் (nf != வெற்று && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_data)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	எனில் (nf->uf_ret_type != வெற்று)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_args = வெற்று;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    நிலையான முழு
add_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    க்கு (முழு loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	முழு		*fcount = loop == 1 ? &cl->class_class_function_மொத்தம்
						: &cl->class_obj_method_மொத்தம்;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	முழு parent_மொத்தம் = 0;
	எனில் (extends_cl != வெற்று)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    parent_மொத்தம் = loop == 1
				? 0
				: extends_cl->class_obj_method_மொத்தம்;

	*fcount = parent_மொத்தம் + gap->ga_len;
	எனில் (*fcount == 0)
	{
	    *fup = வெற்று;
	    தொடர்;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	எனில் (*fup == வெற்று)
	    திரும்பு FAIL;

	எனில் (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_data, அளவு(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_data);
	எனில் (loop == 1)
	    cl->class_class_function_மொத்தம்_child = gap->ga_len;
	இல்லையெனில்
	    cl->class_obj_method_மொத்தம்_child = gap->ga_len;

	எனில் (loop == 2)
	{
	    // Copy instance methods from the parent.

	    க்கு (முழு i = 0; i < parent_மொத்தம்; ++i)
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
		எனில் (nf != வெற்று && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_args != வெற்று)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		க்கு (முழு ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    எனில் (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (எழுத்து *)pname;
			where.wt_kind = WT_METHOD;
			(வெறுமை)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	க்கு (முழு i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    எனில் (i < gap->ga_len)
		fp->uf_defclass = cl;
	    எனில் (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    திரும்பு OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    நிலையான வெறுமை
update_builtin_method_index(class_T *cl)
{
    முழு	i;

    க்கு (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    க்கு (i = 0; i < cl->class_obj_method_மொத்தம்; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	எனில் (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	இல்லையெனில் எனில் (cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] = i;
	இல்லையெனில் எனில் (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    நிலையான char_u *
find_class_name_end(char_u *arg)
{
    char_u *end = arg;

    போது (ASCII_ISALNUM(*end) || *end == '_'
	    || (*end == '.' && (ASCII_ISALNUM(end[1]) || end[1] == '_')))
	++end;

    திரும்பு end;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    நிலையான முழு
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
    முழு		dup = FALSE;

    க்கு (முழு i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_data) + i;

	எனில் (name.length == m->ocm_name.length
	    && STRNCMP(name.string, m->ocm_name.string, name.length) == 0)
	{
	    char_u  save_c = *varname_end;

	    *varname_end = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_end = save_c;
	    dup = TRUE;
	    நிறுத்து;
	}
    }

    திரும்பு dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    நிலையான முழு
enum_parse_மதிப்புகள்(
    exarg_T	*eap,
    class_T	*en,
    char_u	*வரி,
    garray_T	*gap,
    முழு		*இலக்கம்_enum_மதிப்புகள்,
    முழு		*enum_end)
{
    evalarg_T	evalarg;
    char_u	*p = வரி;
    எழுத்து	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	last_char = NUL;
    முழு		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    முழு		did_emsg_before = did_emsg;
    போது (*p != NUL)
    {
	// ignore comment
	எனில் (*p == '#')
	    நிறுத்து;

	எனில் (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_மதிப்பு_declaration_str), p);
	    நிறுத்து;
	}

	char_u *eni_name_start = p;
	char_u *eni_name_end = to_name_end(p, FALSE);

	எனில் (is_duplicate_enum(gap, eni_name_start, eni_name_end))
	    நிறுத்து;

	p = skipwhite(eni_name_end);

	char_u	*init_கோவை = வெற்று;
	எனில் (*p == '(' || *p == '<')
	{
	    எனில் (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", வரி);
		நிறுத்து;
	    }

	    char_u *கோவை_start, *கோவை_end;

	    p = eni_name_start;
	    (வெறுமை)skip_கோவை_concatenate(&p, &கோவை_start, &கோவை_end, &evalarg);

	    போது (*கோவை_start && *கோவை_start != '(' && *கோவை_start != '<')
		கோவை_start++;

	    எனில் (கோவை_end > கோவை_start)
		init_கோவை = vim_strnsave(கோவை_start, கோவை_end - கோவை_start);
	}

	எனில் (init_கோவை == வெற்று)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, அளவு(initexpr_buf), "%s.new()", en->class_name.string);
	இல்லையெனில்
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, அளவு(initexpr_buf), "%s.new%s", en->class_name.string,
		init_கோவை);
	    vim_free(init_கோவை);
	}
	எனில் (add_member(gap, eni_name_start, eni_name_end, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    நிறுத்து;

	++*இலக்கம்_enum_மதிப்புகள்;

	எனில் (*p != '#')
	    last_char = *p;

	எனில் (*p != NUL && *p != ',')
	    நிறுத்து;

	எனில் (*p == ',')
	{
	    எனில் (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", வரி);
		நிறுத்து;
	    }
	    எனில் (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", வரி);
		நிறுத்து;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    எனில் (*p != NUL && *p != '#')
    {
	எனில் (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_argument_str), p);
	rc = FAIL;
    }

    எனில் (last_char != ',')
	// last enum value should not be terminated by ","
	*enum_end = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, வெற்று);

    திரும்பு rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    நிலையான முழு
enum_add_மதிப்புகள்_member(
    class_T	*en,
    garray_T	*gap,
    முழு		இலக்கம்_enum_மதிப்புகள்,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    முழு		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    க்கு (முழு i = 0; i < இலக்கம்_enum_மதிப்புகள்; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;

	எனில் (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.length);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    எனில் (type == வெற்று)
	செல் done;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    எனில் (type->tt_member != வெற்று)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = add_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_data, fga.ga_len));

done:
    vim_free(fga.ga_data);

    திரும்பு rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    நிலையான வெறுமை
enum_clear_constructors(class_T *en)
{
    க்கு (முழு i = 0; i < en->class_class_function_மொத்தம்; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	எனில் (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    வெறுமை
enum_set_internal_obj_vars(class_T *en, object_T *enval)
{
    முழு	i;

    க்கு (i = 0; i < en->class_class_member_மொத்தம்; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	எனில் (en_tv != வெற்று && en_tv->v_type == VAR_UNKNOWN)
	    நிறுத்து;
    }

    எனில் (i >= en->class_class_member_மொத்தம்)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	திரும்பு;

    // First object variable is the name
    ocmember_T *மதிப்பு_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(மதிப்பு_ocm->ocm_name.string, மதிப்பு_ocm->ocm_name.length);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_எண்;
    ord_tv->vval.v_எண் = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    நிலையான வெறுமை
skip_class_body(exarg_T *eap)
{
    க்கு (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	எனில் (theline == வெற்று)
	    நிறுத்து;

	char_u	*p = theline;
	முழு	found;

	// skip ':' and blanks
	க்கு (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	found = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	எனில் (found)
	    நிறுத்து;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    வெறுமை
ex_class(exarg_T *eap)
{
    முழு		is_class = eap->cmdidx == CMD_class;
    முழு		is_abstract = eap->cmdidx == CMD_abstract;
    முழு		is_enum = eap->cmdidx == CMD_enum;
    முழு		added_enum_மதிப்புகள் = FALSE;
    முழு		is_interface;
    long	start_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    எனில் (eap->skip)
    {
	skip_class_body(eap);
	திரும்பு;
    }

    எனில் (is_abstract)
    {
	எனில் (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_argument_str), arg);
	    திரும்பு;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    எனில் (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	எனில் (is_class)
	    emsg(_(e_class_can_only_be_defined_in_vim9_script));
	இல்லையெனில் எனில் (is_enum)
	    emsg(_(e_enum_can_only_be_defined_in_vim9_script));
	இல்லையெனில்
	    emsg(_(e_interface_can_only_be_defined_in_vim9_script));
	திரும்பு;
    }

    எனில் (!ASCII_ISUPPER(*arg))
    {
	எனில் (is_class)
	    semsg(_(e_class_name_must_start_with_uppercase_letter_str), arg);
	இல்லையெனில் எனில் (is_enum)
	    semsg(_(e_enum_name_must_start_with_uppercase_letter_str), arg);
	இல்லையெனில்
	    semsg(_(e_interface_name_must_start_with_uppercase_letter_str),
									  arg);
	திரும்பு;
    }
    char_u *name_end = find_name_end(arg, வெற்று, வெற்று, FNE_CHECK_START);
    எனில் (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	திரும்பு;
    }
    char_u *name_start = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = வெற்று;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, அளவு(char_u *), 5);

    arg = skipwhite(name_end);
    போது (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	எனில் (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    எனில் (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		செல் early_ret;
	    }
	    எனில் (extends != வெற்று)
	    {
		emsg(_(e_duplicate_extends));
		செல் early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *end = find_class_name_end(arg);
	    எனில் (!IS_WHITE_OR_NUL(*end))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		செல் early_ret;
	    }
	    extends = vim_strnsave(arg, end - arg);
	    எனில் (extends == வெற்று)
		செல் early_ret;

	    arg = skipwhite(end + 1);
	}
	இல்லையெனில் எனில் (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    எனில் (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		செல் early_ret;
	    }

	    எனில் (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		செல் early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    க்கு (;;)
	    {
		char_u *impl_end = find_class_name_end(arg);
		எனில் ((!IS_WHITE_OR_NUL(*impl_end) && *impl_end != ',')
			|| (*impl_end == ','
			    && !IS_WHITE_OR_NUL(*(impl_end + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    செல் early_ret;
		}
		எனில் (impl_end - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    செல் early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_end - arg);
		எனில் (iname == வெற்று)
		    செல் early_ret;
		க்கு (முழு i = 0; i < ga_impl.ga_len; ++i)
		    எனில் (STRCMP(((char_u **)ga_impl.ga_data)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			செல் early_ret;
		    }
		எனில் (ga_add_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    செல் early_ret;
		}
		எனில் (*impl_end != ',')
		{
		    arg = skipwhite(impl_end);
		    நிறுத்து;
		}
		arg = skipwhite(impl_end + 1);
	    }
	}
	இல்லையெனில்
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_strings(&ga_impl);
	    திரும்பு;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, அளவு(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, அளவு(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, அளவு(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, அளவு(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, அளவு(ufunc_T *), 10);

    class_T *cl = வெற்று;
    class_T *extends_cl = வெற்று;  // class from "extends" argument
    class_T **intf_classes = வெற்று;
    முழு	    intf_மொத்தம் = 0;	// number of items in "intf_classes"
    முழு	    இலக்கம்_enum_மதிப்புகள் = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    எனில் (cl == வெற்று)
	செல் cleanup;

    எனில் (is_enum)
	cl->class_flags = CLASS_ENUM;
    இல்லையெனில் எனில் (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    இல்லையெனில் எனில் (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.length = (size_t)(name_end - name_start);
    cl->class_name.string = vim_strnsave(name_start, cl->class_name.length);
    எனில் (cl->class_name.string == வெற்று)
	செல் cleanup;

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
    முழு rc = set_var_const(cl->class_name.string, 0, வெற்று, &tv, FALSE, 0, 0);
    எனில் (rc == FAIL)
	செல் cleanup;

    எனில் (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	எனில் (add_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, வெற்று) == FAIL)
	    செல் cleanup;

	varname = (char_u *)"ordinal";
	எனில் (add_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_எண், வெற்று) == FAIL)
	    செல் cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    முழு		enum_end = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = வெற்று;
    முழு success = FALSE;
    க்கு (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	எனில் (theline == வெற்று)
	    நிறுத்து;
	char_u *வரி = skipwhite(theline);

	// Skip empty and comment lines.
	எனில் (*வரி == NUL)
	    தொடர்;
	எனில் (*வரி == '#')
	{
	    எனில் (vim9_bad_comment(வரி))
		நிறுத்து;
	    தொடர்;
	}

	char_u *p = வரி;

	எழுத்து	*end_name;
	முழு	shortlen;
	முழு	fullen;
	எனில் (is_class)
	{
	    end_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	இல்லையெனில் எனில் (is_enum)
	{
	    end_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	இல்லையெனில்
	{
	    end_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	க்கு (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_start = p;
	எனில் (checkforcmd(&p, end_name, shortlen))
	{
	    எனில் (STRNCMP(cmd_start, end_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_start);
	    இல்லையெனில் எனில் (*p == '|' || !ends_excmd2(வரி, p))
		semsg(_(e_trailing_characters_str), p);
	    இல்லையெனில்
		success = TRUE;
	    நிறுத்து;
	}

	முழு	wrong_endname = FALSE;
	எனில் (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	இல்லையெனில் எனில் (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	இல்லையெனில்
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	எனில் (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), வரி, end_name);
	    நிறுத்து;
	}

	எனில் (is_enum && !enum_end)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    எனில் (enum_parse_மதிப்புகள்(eap, cl, வரி, &classmembers,
					&இலக்கம்_enum_மதிப்புகள், &enum_end) == FAIL)
		நிறுத்து;

	    எனில் (enum_end)
	    {
		// Add the enum "values" class variable.
		enum_add_மதிப்புகள்_member(cl, &classmembers, இலக்கம்_enum_மதிப்புகள்,
							&type_list);
		added_enum_மதிப்புகள் = TRUE;
	    }
	    தொடர்;
	}

	முழு has_public = FALSE;
	எனில் (checkforcmd(&p, "public", 3))
	{
	    எனில் (STRNCMP(வரி, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), வரி);
		நிறுத்து;
	    }
	    எனில் (is_interface)
	    {
		emsg(_(e_public_variable_not_supported_in_interface));
		நிறுத்து;
	    }
	    has_public = TRUE;
	    p = skipwhite(வரி + 6);

	    எனில் (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_public_keyword_not_supported_for_method));
		நிறுத்து;
	    }

	    எனில் (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_public_must_be_followed_by_var_static_final_or_const));
		நிறுத்து;
	    }
	}

	முழு abstract_method = FALSE;
	char_u *pa = p;
	எனில் (checkforcmd(&p, "abstract", 3))
	{
	    எனில் (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		நிறுத்து;
	    }

	    எனில் (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_used_in_enum));
		நிறுத்து;
	    }

	    எனில் (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_used_in_interface));
		நிறுத்து;
	    }

	    எனில் (!is_abstract)
	    {
		semsg(_(e_abstract_method_in_concrete_class), pa);
		நிறுத்து;
	    }

	    p = skipwhite(pa + 8);
	    எனில் (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		நிறுத்து;
	    }

	    abstract_method = TRUE;
	}

	முழு has_static = FALSE;
	char_u *ps = p;
	எனில் (checkforcmd(&p, "static", 4))
	{
	    எனில் (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		நிறுத்து;
	    }

	    எனில் (is_interface)
	    {
		emsg(_(e_static_member_not_supported_in_interface));
		நிறுத்து;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    எனில் (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		நிறுத்து;
	    }
	}

	முழு has_final = FALSE;
	முழு has_var = FALSE;
	முழு has_const = FALSE;
	எனில் (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	இல்லையெனில் எனில் (checkforcmd(&p, "final", 5))
	{
	    எனில் (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_in_interface));
		நிறுத்து;
	    }
	    has_final = TRUE;
	}
	இல்லையெனில் எனில் (checkforcmd(&p, "const", 5))
	{
	    எனில் (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_in_interface));
		நிறுத்து;
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
	எனில் (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_end = வெற்று;
	    type_T *type = வெற்று;
	    char_u *init_கோவை = வெற்று;
	    முழு	    has_type = FALSE;

	    எனில் (!eval_isnamec1(*p))
	    {
		எனில் (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), வரி);
		இல்லையெனில்
		    semsg(_(e_invalid_object_variable_declaration_str), வரி);
		நிறுத்து;
	    }

	    எனில் (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_in_interface),
			varname);
		நிறுத்து;
	    }

	    எனில் (parse_member(eap, வரி, varname, has_public,
			  &varname_end, &has_type, &type_list, &type,
			  !is_interface ? &init_கோவை: வெற்று) == FAIL)
		நிறுத்து;

	    எனில் (is_reserved_varname(varname, varname_end)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_end))
	    {
		vim_free(init_கோவை);
		நிறுத்து;
	    }
	    எனில் (add_member(has_static ? &classmembers : &objmembers, varname,
				varname_end, has_public, has_final, has_const,
				has_type, type, init_கோவை) == FAIL)
	    {
		vim_free(init_கோவை);
		நிறுத்து;
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
	இல்லையெனில் எனில் (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	வரிகள்_to_free;
	    முழு		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    எனில் (has_public)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_public_keyword_not_supported_for_method));
		நிறுத்து;
	    }

	    எனில் (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_in_class_str), வரி);
		நிறுத்து;
	    }

	    எனில் (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_method_not_supported_in_interface), p);
		நிறுத்து;
	    }

	    எனில் (has_static && !is_new && SAFE_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_method_not_supported), p);
		நிறுத்து;
	    }

	    CLEAR_FIELD(ea);
	    ea.cmd = வரி;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&வரிகள்_to_free, அளவு(char_u *), 50);
	    முழு class_flags;
	    எனில் (is_interface)
		class_flags = CF_INTERFACE;
	    இல்லையெனில்
		class_flags = abstract_method ? CF_ABSTRACT_METHOD : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, வெற்று, &வரிகள்_to_free,
			class_flags, objmembers.ga_data, objmembers.ga_len,
			வெற்று);
	    ga_clear_strings(&வரிகள்_to_free);

	    எனில் (uf != வெற்று)
	    {
		எனில் (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_method_not_supported_in_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    நிறுத்து;
		}

		// check for builtin method
		எனில் (!is_new && SAFE_islower(*uf->uf_name) &&
					!object_check_builtin_method_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    நிறுத்து;
		}

		// Check the uf_name isn't used already.
		எனில் (is_duplicate_method(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    நிறுத்து;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		எனில் (ga_grow(fgap, 1) == OK)
		{
		    எனில் (is_new)
			uf->uf_flags |= FC_NEW;

		    எனில் (abstract_method)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_data)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	இல்லையெனில்
	{
	    எனில் (is_class)
		semsg(_(e_not_valid_command_in_class_str), வரி);
	    இல்லையெனில் எனில் (is_enum)
		semsg(_(e_not_valid_command_in_enum_str), வரி);
	    இல்லையெனில்
		semsg(_(e_not_valid_command_in_interface_str), வரி);
	    நிறுத்து;
	}
    }

    எனில் (theline == வெற்று && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    எனில் (success && is_enum && (இலக்கம்_enum_மதிப்புகள் == 0 || !added_enum_மதிப்புகள்))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_add_மதிப்புகள்_member(cl, &classmembers, இலக்கம்_enum_மதிப்புகள்,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    எனில் (success && extends != வெற்று)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    எனில் (success && extends_cl != வெற்று)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    எனில் (success && extends_cl != வெற்று)
    {
	எனில் (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	இல்லையெனில்
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    எனில் (success && !is_abstract && extends_cl != வெற்று
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, அளவு(class_T *), 5);

    எனில் (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    எனில் (success && extends_cl != வெற்று)
	success = add_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_data;
    intf_மொத்தம் = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    எனில் (success)
	success = check_func_arg_names(&classfunctions, &objmethods,
							&classmembers);

    எனில் (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	எனில் (extends_cl != வெற்று)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	எனில் (add_members_to_class(&classmembers,
				 வெற்று,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_மொத்தம்) == FAIL
		|| add_members_to_class(&objmembers,
				 extends_cl == வெற்று ? வெற்று
					       : extends_cl->class_obj_members,
				 extends_cl == வெற்று ? 0
					  : extends_cl->class_obj_member_மொத்தம்,
				 &cl->class_obj_members,
				 &cl->class_obj_member_மொத்தம்) == FAIL)
	    செல் cleanup;

	எனில் (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_மொத்தம் = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    எனில் (cl->class_interfaces == வெற்று)
		செல் cleanup;
	    க்கு (முழு i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_data)[i];
	    VIM_CLEAR(ga_impl.ga_data);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = வெற்று;
	}

	எனில் (cl->class_interface_மொத்தம் > 0 || extends_cl != வெற்று)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    எனில் (add_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		செல் cleanup;
	}

	முழு	have_new = FALSE;
	ufunc_T	*class_func = வெற்று;
	க்கு (முழு i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_data)[i];
	    எனில் (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		நிறுத்து;
	    }
	}

	எனில் (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	இல்லையெனில் எனில் ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    add_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	எனில் (add_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    செல் cleanup;

	update_builtin_method_index(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	எனில் ((is_class || is_enum) && cl->class_class_member_மொத்தம் > 0)
	    எனில் (add_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		திரும்பு;
	    }

	cl->class_type_list = type_list;

	எனில் (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	திரும்பு;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    எனில் (intf_classes != வெற்று)
    {
	க்கு (முழு i = 0; i < intf_மொத்தம்; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_strings(&ga_impl);

    க்கு (முழு round = 1; round <= 2; ++round)
    {
	garray_T *gap = round == 1 ? &classmembers : &objmembers;
	எனில் (gap->ga_len == 0 || gap->ga_data == வெற்று)
	    தொடர்;

	க்கு (முழு i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    க்கு (முழு i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    க்கு (முழு i = 0; i < classfunctions.ga_len; ++i)
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
    முழு		is_object,
    char_u	*name,
    char_u	*name_end,
    முழு		*member_idx)
{
    size_t	len = name_end - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    எனில் (m == வெற்று)
    {
	member_not_found_செய்தி(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	திரும்பு &t_any;
    }

    திரும்பு m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    முழு		is_object,
    முழு		member_idx)
{
    ocmember_T	*m;
    முழு		member_மொத்தம்;

    எனில் (is_object)
    {
	m = cl->class_obj_members;
	member_மொத்தம் = cl->class_obj_member_மொத்தம்;
    }
    இல்லையெனில்
    {
	m = cl->class_class_members;
	member_மொத்தம் = cl->class_class_member_மொத்தம்;
    }

    எனில் (member_idx >= member_மொத்தம்)
	திரும்பு வெற்று;

    திரும்பு m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    நிலையான வெறுமை
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    வெறுமை
typealias_unref(typealias_T *ta)
{
    எனில் (ta != வெற்று && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    வெறுமை
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    எனில் (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_in_vim9_script));
	திரும்பு;
    }

    எனில் (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	திரும்பு;
    }

    எனில் (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_start_with_uppercase_letter_str), arg);
	திரும்பு;
    }

    char_u *name_end = find_name_end(arg, வெற்று, வெற்று, FNE_CHECK_START);
    எனில் (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	திரும்பு;
    }
    char_u *name_start = arg;

    arg = skipwhite(name_end);
    எனில் (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	திரும்பு;
    }
    எனில் (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	திரும்பு;
    }
    arg++;
    arg = skipwhite(arg);

    எனில் (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	திரும்பு;
    }

    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, வெற்று, வெற்று, TRUE);
    எனில் (type == வெற்று)
	திரும்பு;

    எனில் (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	திரும்பு;
    }

    முழு cc = *name_end;
    *name_end = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    எனில் (eval_variable_import(name_start, &tv) == OK)
    {
	எனில் (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_start);
	இல்லையெனில்
	    semsg(_(e_redefining_script_item_str), name_start);
	clear_tv(&tv);
	செல் done;
    }

    // Create a script-local variable for the type alias.
    எனில் (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_lock = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_start);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_depth = parse_type_depth();
    }
    இல்லையெனில்
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_lock = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    set_var_const(name_start, current_sctx.sc_sid, வெற்று, &tv, FALSE,
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
    முழு
get_member_tv(
    class_T	*cl,
    முழு		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    முழு		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    எனில் (m == வெற்று)
	திரும்பு FAIL;

    எனில் (*name == '_' && (current_class == வெற்று ||
				!class_instance_of(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	திரும்பு FAIL;
    }

    எனில் (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	set_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    இல்லையெனில்
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	set_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    திரும்பு OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    நிலையான முழு
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
    முழு		argcount = 0;
    ocmember_T	*ocm = வெற்று;
    முழு		m_idx;

    fp = method_lookup(cl, rettv->v_type, name, len, வெற்று);
    எனில் (fp == வெற்று)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	எனில் (ocm == வெற்று || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    method_not_found_செய்தி(cl, rettv->v_type, name, len);
	    திரும்பு FAIL;
	}

	எனில் (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    திரும்பு FAIL;
	}

	எனில் (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	இல்லையெனில்
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_end;
	திரும்பு OK;
    }

    எனில் (ocm == வெற்று && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
	திரும்பு FAIL;
    }

    // process generic function call
    எனில் (fp != வெற்று)
    {
	fp = generic_func_get(fp, gfatab);
	எனில் (fp == வெற்று)
	    திரும்பு FAIL;
    }

    char_u *argp = name_end;
    முழு ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    எனில் (ret == FAIL)
    {
	போது (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	திரும்பு FAIL;
    }

    funcexe_T funcexe;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    எனில் (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    எனில் (evalarg != வெற்று)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    முழு error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								வெற்று);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    க்கு (முழு idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    எனில் (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_found_var);
	திரும்பு FAIL;
    }
    *arg = argp;

    திரும்பு OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    முழு
obj_method_to_partial_tv(object_T *obj, ufunc_T *obj_method, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    எனில் (pt == வெற்று)
	திரும்பு FAIL;

    pt->pt_refcount = 1;
    எனில் (obj != வெற்று)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_method;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    திரும்பு OK;
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
    முழு
class_object_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    முழு		verbose UNUSED)	// give error messages
{
    முழு	ret = FAIL;

    எனில் (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	திரும்பு FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, வெற்று, வெற்று, FNE_CHECK_START);
    எனில் (name_end == name)
	திரும்பு FAIL;
    size_t len = name_end - name;

    முழு did_emsg_save = did_emsg;
    class_T *cl;
    எனில் (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    இல்லையெனில் // VAR_OBJECT
    {
	எனில் (rettv->vval.v_object == வெற்று)
	{
	    emsg(_(e_using_null_object));
	    திரும்பு FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    எனில் (cl == வெற்று)
    {
	emsg(_(e_incomplete_type));
	திரும்பு FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    எனில் (*name_end == '<')
    {
	cctx_T	    *cctx = வெற்று;

	எனில் (evalarg != வெற்று)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_end = parse_generic_func_type_args(name, len, name + len,
						&gfatab, cctx);
	எனில் (name_end == வெற்று)
	    செல் done;
    }

    எனில் (*name_end == '(')
	// Invoke the class or object method
	ret = call_oc_method(cl, name, len, name_end, evalarg, arg,
							&gfatab, rettv);
    இல்லையெனில் எனில் (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	முழு is_object = rettv->v_type == VAR_OBJECT;
	எனில் (get_member_tv(cl, is_object, name, len, வெற்று, rettv) == OK)
	{
	    *arg = name_end;
	    ret = OK;
	    செல் done;
	}

	// could be a class method or an object method
	முழு	fidx;
	ufunc_T	*fp = method_lookup(cl, rettv->v_type, name, len, &fidx);
	எனில் (fp != வெற்று)
	{
	    // Protected methods are not accessible outside the class
	    எனில் (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		செல் done;
	    }

	    எனில் (obj_method_to_partial_tv(is_object ? rettv->vval.v_object :
						வெற்று, fp, rettv) == FAIL)
		செல் done;

	    *arg = name_end;
	    ret = OK;
	    செல் done;
	}

	எனில் (did_emsg == did_emsg_save)
	    member_not_found_செய்தி(cl, rettv->v_type, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);

    திரும்பு ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
find_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, வெற்று, வெற்று, FNE_CHECK_START);
    எனில் (name_end == name || *name_end != '.')
	திரும்பு வெற்று;

    ufunc_T	*fp = வெற்று;
    size_t	len = name_end - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    எனில் (eval_variable(name, (முழு)len,
				    0, &tv, வெற்று, EVAL_VAR_NOAUTOLOAD) == FAIL)
	திரும்பு வெற்று;
    எனில் (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	செல் fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    எனில் (cl == வெற்று)
	செல் fail_after_eval;
    char_u *fname = name_end + 1;
    char_u *fname_end = find_name_end(fname, வெற்று, வெற்று, FNE_CHECK_START);
    எனில் (fname_end == fname)
	செல் fail_after_eval;
    len = fname_end - fname;

    fp = method_lookup(cl, tv.v_type, fname, len, வெற்று);

    எனில் (fp != வெற்று)
    {
	fp = eval_generic_func(fp, fname, &fname_end);
	எனில் (fp != வெற்று)
	    *arg = fname_end;
    }

fail_after_eval:
    clear_tv(&tv);
    திரும்பு fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    முழு
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    முழு idx;
    class_member_lookup(cl, name, namelen, &idx);
    திரும்பு idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, முழு *idx)
{
    ocmember_T	*ret_m = வெற்று;
    முழு		ret_idx = -1;
    க்கு (முழு i = 0; i < cl->class_class_member_மொத்தம்; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	எனில் (namelen)
	{
	    எனில் (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		நிறுத்து;
	    }
	}
	இல்லையெனில் எனில் (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    நிறுத்து;
	}
    }
    எனில் (idx != வெற்று)
	*idx = ret_idx;
    திரும்பு ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    நிலையான ufunc_T *
class_method_lookup(class_T *cl, char_u *name, size_t namelen, முழு *idx)
{
    ufunc_T	*ret_fp = வெற்று;
    முழு		ret_idx = -1;
    க்கு (முழு i = 0; i < cl->class_class_function_மொத்தம்; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	எனில் (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    நிறுத்து;
	}
    }
    எனில் (idx != வெற்று)
	*idx = ret_idx;
    திரும்பு ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    முழு
class_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    முழு idx;
    class_method_lookup(cl, name, namelen, &idx);
    திரும்பு idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    நிலையான முழு
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    முழு idx;
    object_member_lookup(cl, name, namelen, &idx);
    திரும்பு idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, முழு *idx)
{
    ocmember_T	*ret_m = வெற்று;
    முழு		ret_idx = -1;
    க்கு (முழு i = 0; i < cl->class_obj_member_மொத்தம்; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	எனில் (namelen)
	{
	    எனில் (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		நிறுத்து;
	    }
	}
	இல்லையெனில் எனில் (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    நிறுத்து;
	}
    }
    எனில் (idx != வெற்று)
	*idx = ret_idx;
    திரும்பு ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    நிலையான ufunc_T *
object_method_lookup(class_T *cl, char_u *name, size_t namelen, முழு *idx)
{
    ufunc_T	*ret_fp = வெற்று;
    முழு		ret_idx = -1;
    க்கு (முழு i = 0; i < cl->class_obj_method_மொத்தம்; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	எனில் (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    நிறுத்து;
	}
    }
    எனில் (idx != வெற்று)
	*idx = ret_idx;
    திரும்பு ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    முழு
object_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    முழு idx;
    object_method_lookup(cl, name, namelen, &idx);
    திரும்பு idx;
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
    முழு		*idx)
{
    எனில் (v_type == VAR_CLASS)
	திரும்பு class_member_lookup(cl, name, namelen, idx);

    திரும்பு object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    நிலையான class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_found = வெற்று;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_found = வெற்று;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    க்கு (class_T *super = cl; super != வெற்று; super = super->class_extends)
    {
	class_T		*cl_tmp = வெற்று;
	ocmember_T	*m = வெற்று;
	எனில் (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    எனில் ((m = object_member_lookup(super, name, len, வெற்று)) != வெற்று)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	எனில் (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    எனில் (( m = class_member_lookup(super, name, len, வெற்று)) != வெற்று)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	எனில் (cl_tmp == வெற்று)
	    நிறுத்து;  // member is not in this or any super class.
	cl_found = cl_tmp;
	m_found = m;
    }
    எனில் (p_m != வெற்று)
	*p_m = m_found;
    திரும்பு cl_found;
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
    முழு		*idx)
{
    எனில் (v_type == VAR_CLASS)
	திரும்பு class_method_lookup(cl, name, namelen, idx);
    இல்லையெனில்
	திரும்பு object_method_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    முழு
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    க்கு (cctx_T *cctx = cctx_arg; cctx != வெற்று; cctx = cctx->ctx_outer)
	எனில் (cctx->ctx_ufunc != வெற்று
			&& class_instance_of(cctx->ctx_ufunc->uf_class, cl))
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    முழு
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    எனில் (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_in_class_str),
		m->ocm_name.string, cl->class_name.string);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    வெறுமை
obj_lock_const_vars(object_T *obj)
{
    க்கு (முழு i = 0; i < obj->obj_class->class_obj_member_மொத்தம்; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	எனில் (ocm->ocm_flags & OCMFLAG_CONST)
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
    முழு		sz;

    எனில் (cl == வெற்று)
	திரும்பு வெற்று;

    sz = அளவு(object_T) + cl->class_obj_member_மொத்தம் * அளவு(typval_T);
    obj = alloc_clear(sz);
    எனில் (obj == வெற்று)
	திரும்பு வெற்று;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    எனில் (IS_ENUM(cl))
	enum_set_internal_obj_vars(cl, obj);

    திரும்பு obj;
}

/*
 * Make a copy of an object.
 */
    வெறுமை
copy_object(typval_T *from, typval_T *to)
{
    எனில் (from->vval.v_object == வெற்று)
	to->vval.v_object = வெற்று;
    இல்லையெனில்
    {
	to->vval.v_object = from->vval.v_object;
	++to->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    வெறுமை
copy_class(typval_T *from, typval_T *to)
{
    எனில் (from->vval.v_class == வெற்று)
	to->vval.v_class = வெற்று;
    இல்லையெனில்
    {
	to->vval.v_class = from->vval.v_class;
	++to->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    நிலையான வெறுமை
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    க்கு (முழு i = 0; i < cl->class_interface_மொத்தம்; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	எனில் (cl->class_interfaces_cl[i] != வெற்று)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *next;
    க்கு (itf2class_T *i2c = cl->class_itf2class; i2c != வெற்று; i2c = next)
    {
	next = i2c->i2c_next;
	vim_free(i2c);
    }

    க்கு (முழு i = 0; i < cl->class_class_member_மொத்தம்; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	எனில் (cl->class_members_tv != வெற்று)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    க்கு (முழு i = 0; i < cl->class_obj_member_மொத்தம்; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    க்கு (முழு i = 0; i < cl->class_class_function_மொத்தம்; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	எனில் (IS_ENUM(cl) && IS_CONSTRUCTOR_METHOD(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    க்கு (முழு i = 0; i < cl->class_obj_method_மொத்தம்; ++i)
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
    நிலையான முழு
class_get_selfrefs(class_T *cl)
{
    முழு		self_refs = 0;
    typval_T	*tv;

    க்கு (முழு i = 0; i < cl->class_class_member_மொத்தம்; ++i)
    {
	tv = &cl->class_members_tv[i];
	எனில் (tv->v_type == VAR_OBJECT && tv->vval.v_object != வெற்று
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    திரும்பு self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    நிலையான முழு
can_free_enum(class_T *cl)
{
    க்கு (முழு i = 0; i < cl->class_class_member_மொத்தம்; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	எனில் (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    திரும்பு TRUE;

	எனில் (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    எனில் (tv->vval.v_list->lv_refcount > 1)
		திரும்பு FALSE;
	    நிறுத்து;
	}

	எனில் (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    திரும்பு FALSE;
    }

    திரும்பு TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    நிலையான முழு
can_free_class(class_T *cl)
{
    முழு		can_free = FALSE;
    முழு		self_refs = 0;

    எனில் (IS_ENUM(cl) && !can_free_enum(cl))
	திரும்பு FALSE;

    எனில் (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    எனில் (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    திரும்பு can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    வெறுமை
class_unref(class_T *cl)
{
    எனில் (cl == வெற்று)
	திரும்பு;

    --cl->class_refcount;

    எனில் (cl->class_name.string == வெற்று)
	திரும்பு;

    எனில் (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    முழு
class_free_nonref(முழு copyID)
{
    முழு		did_free = FALSE;

    க்கு (class_T *cl = first_class; cl != வெற்று; cl = next_nonref_class)
    {
	next_nonref_class = cl->class_next_used;
	எனில் ((cl->class_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    next_nonref_class = வெற்று;
    திரும்பு did_free;
}

    முழு
set_ref_in_classes(முழு copyID)
{
    க்கு (class_T *cl = first_class; cl != வெற்று; cl = cl->class_next_used)
	set_ref_in_item_class(cl, copyID, வெற்று, வெற்று, வெற்று);

    திரும்பு FALSE;
}

நிலையான object_T *first_object = வெற்று;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    வெறுமை
object_created(object_T *obj)
{
    எனில் (first_object != வெற்று)
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
    நிலையான வெறுமை
object_cleared(object_T *obj)
{
    எனில் (obj->obj_next_used != வெற்று)
	obj->obj_next_used->obj_prev_used = obj->obj_prev_used;
    எனில் (obj->obj_prev_used != வெற்று)
	obj->obj_prev_used->obj_next_used = obj->obj_next_used;
    இல்லையெனில் எனில் (first_object == obj)
	first_object = obj->obj_next_used;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    நிலையான வெறுமை
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    எனில் (!cl)
	திரும்பு;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = INT_MAX;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    க்கு (முழு i = 0; i < cl->class_obj_member_மொத்தம்; ++i)
	clear_tv(tv + i);
}

    நிலையான வெறுமை
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    எனில் (!cl)
	திரும்பு;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    நிலையான வெறுமை
object_free(object_T *obj)
{
    எனில் (in_free_unref_பொருட்கள்)
	திரும்பு;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    வெறுமை
object_unref(object_T *obj)
{
    எனில் (obj != வெற்று && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    முழு
object_free_nonref(முழு copyID)
{
    முழு		did_free = FALSE;

    க்கு (object_T *obj = first_object; obj != வெற்று; obj = obj->obj_next_used)
    {
	எனில் ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    திரும்பு did_free;
}

    வெறுமை
object_free_பொருட்கள்(முழு copyID)
{
    object_T	*obj_next;

    க்கு (object_T *obj = first_object; obj != வெற்று; obj = obj_next)
    {
	obj_next = obj->obj_next_used;
	எனில் ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    வெறுமை
emsg_var_cl_define(எழுத்து *செய்தி, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    எனில் (cl_def != வெற்று)
	semsg(_(செய்தி), m->ocm_name.string, cl_def->class_name.string);
    இல்லையெனில்
	emsg(_(e_internal_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    வெறுமை
method_not_found_செய்தி(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *method_name = vim_strnsave(name, len);
    எனில் ((v_type == VAR_OBJECT)
	    && (class_method_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	எனில் (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	இல்லையெனில்
	    semsg(_(e_class_method_str_accessible_only_using_class_str),
		    method_name, cl->class_name.string);
    }
    இல்லையெனில் எனில் ((v_type == VAR_CLASS)
	    && (object_method_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	எனில் (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	இல்லையெனில்
	    semsg(_(e_object_method_str_accessible_only_using_object_str),
		    method_name, cl->class_name.string);
    }
    இல்லையெனில்
	semsg(_(e_method_not_found_on_class_str_str), method_name,
		cl->class_name.string);
    vim_free(method_name);
}

/*
 * Echo a class or object member not found message.
 */
    வெறுமை
member_not_found_செய்தி(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    எனில் (v_type == VAR_OBJECT)
    {
	எனில் (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	இல்லையெனில்
	    semsg(_(e_variable_not_found_on_object_str_str), varname,
		    cl->class_name.string);
    }
    இல்லையெனில்
    {
	எனில் (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	இல்லையெனில்
	{
	    எனில் (IS_ENUM(cl))
		semsg(_(e_enum_மதிப்பு_str_not_found_in_enum_str),
			varname, cl->class_name.string);
	    இல்லையெனில்
		semsg(_(e_class_variable_str_not_found_in_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    வெறுமை
defcompile_class(class_T *cl)
{
    க்கு (முழு loop = 1; loop <= 2; ++loop)
    {
	முழு func_மொத்தம் = loop == 1 ? cl->class_class_function_மொத்தம்
						: cl->class_obj_method_மொத்தம்;
	க்கு (முழு i = 0; i < func_மொத்தம்; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    எனில் (!IS_ABSTRACT_METHOD(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    வெறுமை
defcompile_classes_in_script(வெறுமை)
{
    க்கு (class_T *cl = first_class; cl != வெற்று; cl = cl->class_next_used)
    {
	எனில் (eval_variable(cl->class_name.string, 0, 0, வெற்று, வெற்று,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    முழு
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    எனில் (eval_variable(name, 0, 0, rettv, வெற்று, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	திரும்பு rettv->v_type == VAR_CLASS;
    திரும்பு FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    நிலையான முழு
object_call_builtin_method(
    object_T		*obj,
    class_builtin_T	builtin_method,
    முழு			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    முழு	    midx;

    எனில் (obj == வெற்று)
	திரும்பு FAIL;

    uf = class_get_builtin_method(obj->obj_class, builtin_method, &midx);
    எனில் (uf == வெற்று)
	திரும்பு FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    முழு		r;

    எனில் (fc == வெற்று)
	திரும்பு FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, வெற்று, obj, fc, rettv);

    remove_funccal();

    திரும்பு r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    முழு
object_empty(object_T *obj)
{
    typval_T	rettv;

    எனில் (object_call_builtin_method(obj, CLASS_BUILTIN_EMPTY, 0, வெற்று, &rettv)
								== FAIL)
	திரும்பு TRUE;

    திரும்பு tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    முழு
object_len(object_T *obj)
{
    typval_T	rettv;

    எனில் (object_call_builtin_method(obj, CLASS_BUILTIN_LEN, 0, வெற்று, &rettv)
								== FAIL)
	திரும்பு 0;

    திரும்பு tv_to_எண்(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    முழு
object_equal(
	object_T *o1,
	object_T *o2,
	முழு	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    எனில் (o1 == o2)
	திரும்பு TRUE;
    எனில் (o1 == வெற்று || o2 == வெற்று)
	திரும்பு FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    எனில் (cl1 != cl2 || cl1 == வெற்று || cl2 == வெற்று)
	திரும்பு FALSE;

    க்கு (முழு i = 0; i < cl1->class_obj_member_மொத்தம்; ++i)
	எனில் (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    திரும்பு FALSE;

    திரும்பு TRUE;
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
    முழு		copyID,
    முழு		echo_style,
    முழு		restore_copyID,
    முழு		composite_val)
{
    typval_T	rettv;

    எனில் (object_call_builtin_method(obj, CLASS_BUILTIN_STRING, 0, வெற்று, &rettv)
								== OK
					&& rettv.vval.v_string != வெற்று)
	திரும்பு rettv.vval.v_string;

    முழு		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    எனில் (cl != வெற்று && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    இல்லையெனில்
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	எனில் (cl == வெற்று)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	இல்லையெனில்
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
    }
    எனில் (cl != வெற்று)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	க்கு (முழு i = 0; i < cl->class_obj_member_மொத்தம்; ++i)
	{
	    எனில் (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.length);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = வெற்று;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, composite_val);
	    எனில் (s != வெற்று)
		ga_concat(&ga, s);
	    vim_free(tf);
	    எனில் (s == வெற்று || did_echo_string_emsg)
	    {
		ok = FAIL;
		நிறுத்து;
	    }
	    வரி_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    எனில் (ok == FAIL)
    {
	vim_free(ga.ga_data);
	திரும்பு வெற்று;
    }
    ga_append(&ga, NUL);
    திரும்பு (char_u *)ga.ga_data;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    முழு
class_instance_of(class_T *cl, class_T *other_cl)
{
    எனில் (cl == other_cl)
	திரும்பு TRUE;

    // Recursively check the base classes.
    க்கு (; cl != வெற்று; cl = cl->class_extends)
    {
	எனில் (cl == other_cl)
	    திரும்பு TRUE;
	// Check the implemented interfaces and the super interfaces
	க்கு (முழு i = cl->class_interface_மொத்தம் - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    போது (intf != வெற்று)
	    {
		எனில் (intf == other_cl)
		    திரும்பு TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    திரும்பு FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    வெறுமை
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_எண் = VVAL_FALSE;

    எனில் (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_args(argvars, 1) == FAIL)
	திரும்பு;

    எனில் (object_tv->vval.v_object == வெற்று)
    {
	எனில் (classinfo_tv->vval.v_class == வெற்று)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_எண் = VVAL_TRUE;
	திரும்பு;
    }

    க்கு (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	எனில் (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	இல்லையெனில்
	    c = classinfo_tv->vval.v_class;

	எனில் (class_instance_of(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_எண் = VVAL_TRUE;
	    திரும்பு;
	}
    }
}

#endif // FEAT_EVAL
