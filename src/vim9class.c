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
#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

স্থির class_T *first_class = রিক্ত;
স্থির class_T *next_nonref_class = রিক্ত;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    স্থির শূন্য
class_created(class_T *cl)
{
    যদি (first_class != রিক্ত)
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
    স্থির শূন্য
class_cleared(class_T *cl)
{
    যদি (cl->class_next_used != রিক্ত)
	cl->class_next_used->class_prev_used = cl->class_prev_used;
    যদি (cl->class_prev_used != রিক্ত)
	cl->class_prev_used->class_next_used = cl->class_next_used;
    অন্যথা যদি (first_class == cl)
	first_class = cl->class_next_used;

    // update the next class to check if needed
    যদি (cl == next_nonref_class)
	next_nonref_class = cl->class_next_used;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    স্থির পূর্ণাঙ্ক
parse_member(
    exarg_T	*eap,
    char_u	*সারি,
    char_u	*varname,
    পূর্ণাঙ্ক		has_public,	    // TRUE if "public" seen before "varname"
    char_u	**varname_end,
    পূর্ণাঙ্ক		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_রাশিমালা)
{
    *varname_end = to_name_end(varname, FALSE);
    যদি (*varname == '_' && has_public)
    {
	semsg(_(e_public_variable_name_cannot_start_with_underscore_str), সারি);
	প্রত্যাগম FAIL;
    }

    char_u *colon = skipwhite(*varname_end);
    char_u *type_arg = colon;
    type_T *type = রিক্ত;
    *has_type = FALSE;
    যদি (*colon == ':')
    {
	যদি (VIM_ISWHITE(**varname_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    প্রত্যাগম FAIL;
	}
	যদি (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    প্রত্যাগম FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, রিক্ত, রিক্ত, TRUE);
	যদি (type == রিক্ত || !valid_declaration_type(type))
	    প্রত্যাগম FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    যদি (type == রিক্ত && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	প্রত্যাগম FAIL;
    }

    যদি (init_রাশিমালা == রিক্ত && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_in_interface));
	প্রত্যাগম FAIL;
    }

    যদি (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *রাশিমালা_start, *রাশিমালা_end;

	যদি (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    প্রত্যাগম FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(শূন্য)skip_রাশিমালা_concatenate(&init_arg, &রাশিমালা_start, &রাশিমালা_end, &evalarg);

	init_arg = skipwhite(init_arg);
	যদি (*init_arg != NUL && !vim9_comment_start(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, রিক্ত);
	    প্রত্যাগম FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	যদি (type == রিক্ত)
	    type = &t_any;

	*init_রাশিমালা = vim_strnsave(রাশিমালা_start, রাশিমালা_end - রাশিমালা_start);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, রিক্ত);
    }
    অন্যথা যদি (!valid_declaration_type(type))
	প্রত্যাগম FAIL;

    *type_ret = type;
    প্রত্যাগম OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    স্থির পূর্ণাঙ্ক
add_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_end,
    পূর্ণাঙ্ক		has_public,
    পূর্ণাঙ্ক		has_final,
    পূর্ণাঙ্ক		has_const,
    পূর্ণাঙ্ক		has_type,
    type_T	*type,
    char_u	*init_রাশিমালা)
{
    যদি (ga_grow(gap, 1) == FAIL)
	প্রত্যাগম FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_data) + gap->ga_len;
    m->ocm_name.length = (size_t)(varname_end - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.length);
    যদি (m->ocm_name.string == রিক্ত)
    {
	m->ocm_name.length = 0;
	প্রত্যাগম FAIL;
    }
    m->ocm_access = has_public ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_PRIVATE : VIM_ACCESS_READ;
    যদি (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    যদি (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    যদি (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    যদি (init_রাশিমালা != রিক্ত)
    {
	m->ocm_init = init_রাশিমালা;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
add_members_to_class(
    garray_T	*gap,
    ocmember_T	*parent_members,
    পূর্ণাঙ্ক		parent_গুনতি,
    ocmember_T	**members,
    পূর্ণাঙ্ক		*member_গুনতি)
{
    *member_গুনতি = parent_গুনতি + gap->ga_len;
    *members = *member_গুনতি == 0 ? রিক্ত
				       : ALLOC_MULT(ocmember_T, *member_গুনতি);
    যদি (*member_গুনতি > 0 && *members == রিক্ত)
	প্রত্যাগম FAIL;
    প্রতি (পূর্ণাঙ্ক i = 0; i < parent_গুনতি; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = parent_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.length);
	যদি (m->ocm_name.string == রিক্ত)
	{
	    m->ocm_name.length = 0;
	    প্রত্যাগম FAIL;
	}
	যদি (m->ocm_init != রিক্ত)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    যদি (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + parent_গুনতি,
			       gap->ga_data, পরিমাণ(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_data);
    প্রত্যাগম OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    পূর্ণাঙ্ক
object_index_from_itf_index(class_T *itf, পূর্ণাঙ্ক is_method, পূর্ণাঙ্ক idx, class_T *cl)
{
    যদি (idx >= (is_method ? itf->class_obj_method_গুনতি
				   : itf->class_obj_member_গুনতি))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	প্রত্যাগম 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    যদি (cl == itf)
	প্রত্যাগম idx;

    itf2class_T		*i2c = রিক্ত;
    পূর্ণাঙ্ক			searching = TRUE;
    পূর্ণাঙ্ক			method_offset = 0;

    প্রতি (class_T *super = cl; super != রিক্ত && searching;
						super = super->class_extends)
    {
	প্রতি (i2c = itf->class_itf2class; i2c != রিক্ত; i2c = i2c->i2c_next)
	{
	    যদি (i2c->i2c_class == super && i2c->i2c_is_method == is_method)
	    {
		searching = FALSE;
		ভঙ্গ;
	    }
	}
	যদি (searching && is_method)
	    // The parent class methods are stored after the current class
	    // methods.
	    method_offset += super->class_obj_method_গুনতি_child;
    }
    যদি (i2c == রিক্ত)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	প্রত্যাগম 0;
    }

    // A table follows the i2c for the class
    পূর্ণাঙ্ক *table = (পূর্ণাঙ্ক *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    প্রত্যাগম table[idx] + method_offset;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    স্থির পূর্ণাঙ্ক
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    পূর্ণাঙ্ক	    is_class)
{
    typval_T	tv;
    পূর্ণাঙ্ক		success = FALSE;

    যদি (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	প্রত্যাগম success;
    }

    tv.v_type = VAR_UNKNOWN;
    যদি (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_found_str), extends_name);
	প্রত্যাগম success;
    }

    যদি (tv.v_type != VAR_CLASS || tv.vval.v_class == রিক্ত
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    অন্যথা
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    প্রত্যাগম success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    স্থির পূর্ণাঙ্ক
validate_extends_generic_method(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    বর্ণ	*বার্তা = রিক্ত;

    যদি (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	প্রত্যাগম TRUE;

    যদি (super_fp->uf_generic_argcount == 0)
	বার্তা = e_concrete_method_str_override_with_generic_method_in_class_str;
    অন্যথা যদি (cl_fp->uf_generic_argcount == 0)
	বার্তা = e_generic_method_str_override_with_concrete_method_in_class_str;
    অন্যথা
	বার্তা = e_generic_method_str_type_arguments_mismatch_in_class_str;

    semsg(_(বার্তা), cl_fp->uf_name, super_cl->class_name.string);
    প্রত্যাগম FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    স্থির পূর্ণাঙ্ক
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    পূর্ণাঙ্ক		method_গুনতি = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_data);

    যাবত্ (super != রিক্ত)
    {
	পূর্ণাঙ্ক extends_method_গুনতি = super->class_obj_method_গুনতি_child;
	যদি (extends_method_গুনতি == 0)
	{
	    super = super->class_extends;
	    অনুবর্ত;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	প্রতি (পূর্ণাঙ্ক i = 0; i < extends_method_গুনতি; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    পূর্ণাঙ্ক	    extends_private = (*pstr == '_');
	    যদি (extends_private)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    প্রতি (পূর্ণাঙ্ক j = 0; j < method_গুনতি; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		পূর্ণাঙ্ক	priv_method = (*qstr == '_');
		যদি (priv_method)
		    qstr++;
		যদি (STRCMP(pstr, qstr) == 0)
		{
		    যদি (priv_method != extends_private)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_method_str_of_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			প্রত্যাগম FALSE;
		    }

		    যদি (!validate_extends_generic_method(super,
						extends_methods[i], cl_fp[j]))
			    প্রত্যাগম FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    প্রত্যাগম TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    স্থির পূর্ণাঙ্ক
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    পূর্ণাঙ্ক member_গুনতি = objmembers_gap->ga_len;
    যদি (member_গুনতি == 0)
	প্রত্যাগম TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    প্রতি (পূর্ণাঙ্ক c_i = 0; c_i < member_গুনতি; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	যাবত্ (p_cl != রিক্ত)
	{
	    পূর্ণাঙ্ক p_member_গুনতি = p_cl->class_obj_member_গুনতি;

	    যদি (p_member_গুনতি == 0)
	    {
		p_cl = p_cl->class_extends;
		অনুবর্ত;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    প্রতি (পূর্ণাঙ্ক p_i = 0; p_i < p_member_গুনতি; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		যদি (STRCMP(pstr, qstr) == 0)
		{
		    যদি (c_protected != p_protected)
			semsg(_(e_public_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    অন্যথা
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    প্রত্যাগম FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    প্রত্যাগম TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    স্থির পূর্ণাঙ্ক
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    পূর্ণাঙ্ক member_গুনতি = objmembers_gap->ga_len;
    যদি (member_গুনতি == 0)
	প্রত্যাগম TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    প্রতি (পূর্ণাঙ্ক c_i = 0; c_i < member_গুনতি; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	পূর্ণাঙ্ক	    var_found = FALSE;

	// Check in all the parent classes in the lineage
	যাবত্ (p_cl != রিক্ত && !var_found)
	{
	    পূর্ণাঙ্ক p_member_গুনতি = p_cl->class_obj_member_গুনতি;
	    যদি (p_member_গুনতি == 0)
	    {
		p_cl = p_cl->class_extends;
		অনুবর্ত;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    প্রতি (পূর্ণাঙ্ক p_i = 0; p_i < p_member_গুনতি; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		যদি (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    অনুবর্ত;

		// Ensure the type is matching.
		where.wt_func_name = (বর্ণ *)c_m->ocm_name.string;
		where.wt_kind = WT_MEMBER;

		যদি (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    প্রত্যাগম FALSE;

		var_found = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    প্রত্যাগম TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    স্থির পূর্ণাঙ্ক
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    প্রতি (পূর্ণাঙ্ক loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	পূর্ণাঙ্ক extends_method_গুনতি = loop == 1
				? extends_cl->class_class_function_গুনতি
				: extends_cl->class_obj_method_গুনতি;
	যদি (extends_method_গুনতি == 0)
	    অনুবর্ত;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	পূর্ণাঙ্ক method_গুনতি = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_data
						: objmethods_gap->ga_data);

	প্রতি (পূর্ণাঙ্ক i = 0; i < extends_method_গুনতি; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    যদি (!IS_ABSTRACT_METHOD(uf))
		অনুবর্ত;

	    পূর্ণাঙ্ক	concrete_method_found = FALSE;
	    পূর্ণাঙ্ক	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    প্রতি (j = 0; !concrete_method_found && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		যদি (!IS_ABSTRACT_METHOD(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_method_found = TRUE;
	    }

	    যদি (concrete_method_found)
		অনুবর্ত;

	    প্রতি (j = 0; j < method_গুনতি; j++)
	    {
		যদি (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_method_found = TRUE;
		    ভঙ্গ;
		}
	    }

	    যদি (!concrete_method_found)
	    {
		semsg(_(e_abstract_method_str_not_implemented), uf->uf_name);
		প্রত্যাগম FALSE;
	    }
	}
    }

    প্রত্যাগম TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    স্থির পূর্ণাঙ্ক
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    পূর্ণাঙ্ক		cl_member_গুনতি,
    class_T	*extends_cl)
{
    পূর্ণাঙ্ক		variable_present  = FALSE;

    প্রতি (পূর্ণাঙ্ক cl_i = 0; cl_i < cl_member_গুনতি; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	যদি (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    অনুবর্ত;

	// Ensure the access type is same
	যদি (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_of_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    প্রত্যাগম FALSE;
	}

	// Ensure the type is matching.
	যদি (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	অন্যথা
	{
	    where.wt_func_name = (বর্ণ *)m->ocm_name.string;
	    where.wt_kind = WT_MEMBER;
	    যদি (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		প্রত্যাগম FALSE;
	}

	variable_present = TRUE;
	ভঙ্গ;
    }

    যদি (!variable_present && extends_cl != রিক্ত)
    {
	পূর্ণাঙ্ক ext_cl_গুনতি = extends_cl->class_obj_member_গুনতি;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	প্রত্যাগম intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_গুনতি,
					extends_cl->class_extends);
    }

    প্রত্যাগম variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    স্থির পূর্ণাঙ্ক
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    পূর্ণাঙ্ক if_গুনতি = ifcl->class_obj_member_গুনতি;
    যদি (if_গুনতি == 0)
	প্রত্যাগম TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_data);
    পূর্ণাঙ্ক cl_গুনতি = objmembers_gap->ga_len;
    প্রতি (পূর্ণাঙ্ক if_i = 0; if_i < if_গুনতি; ++if_i)
    {
	যদি (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_গুনতি, extends_cl))
	{
	    semsg(_(e_variable_str_of_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    প্রত্যাগম FALSE;
	}
    }

    প্রত্যাগম TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    স্থির পূর্ণাঙ্ক
intf_method_type_matches(
    ufunc_T	*if_method,
    ufunc_T	*cl_method,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    যদি (cl_method->uf_generic_argcount != if_method->uf_generic_argcount)
    {
	semsg(_(e_type_var_গুনতি_of_method_str_differs_from_interface_str),
						cl_method->uf_name, intf_name);
	প্রত্যাগম FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (বর্ণ *)if_method->uf_name;
    where.wt_kind = WT_METHOD;
    যদি (check_type(if_method->uf_func_type, cl_method->uf_func_type, TRUE,
								where) == FAIL)
	প্রত্যাগম FALSE;

    প্রত্যাগম TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    স্থির পূর্ণাঙ্ক
intf_method_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    পূর্ণাঙ্ক	    cl_গুনতি,
    class_T *extends_cl,
    char_u  *intf_name)
{
    পূর্ণাঙ্ক		method_present  = FALSE;

    প্রতি (পূর্ণাঙ্ক cl_i = 0; cl_i < cl_গুনতি; ++cl_i)
    {
	যদি (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    যদি (!intf_method_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		প্রত্যাগম FALSE;
	    method_present = TRUE;
	    ভঙ্গ;
	}
    }

    যদি (!method_present && extends_cl != রিক্ত)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	পূর্ণাঙ্ক	ext_cl_গুনতি = extends_cl->class_obj_method_গুনতি;
	প্রত্যাগম intf_method_present(if_ufunc, ext_cl_fp, ext_cl_গুনতি,
					extends_cl->class_extends, intf_name);
    }

    প্রত্যাগম method_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    স্থির পূর্ণাঙ্ক
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    পূর্ণাঙ্ক if_গুনতি = ifcl->class_obj_method_গুনতি;
    যদি (if_গুনতি == 0)
	প্রত্যাগম TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_data);
    পূর্ণাঙ্ক cl_গুনতি = objmethods_gap->ga_len;
    প্রতি (পূর্ণাঙ্ক if_i = 0; if_i < if_গুনতি; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	যদি (!intf_method_present(if_fp[if_i], cl_fp, cl_গুনতি, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_method_str_of_interface_str_not_implemented),
		    if_name, intf_class_name);
	    প্রত্যাগম FALSE;
	}
    }

    প্রত্যাগম TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    স্থির পূর্ণাঙ্ক
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    পূর্ণাঙ্ক		success = TRUE;

    প্রতি (পূর্ণাঙ্ক i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_data)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	যদি (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_found_str), impl);
	    success = FALSE;
	    ভঙ্গ;
	}

	যদি (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == রিক্ত
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    ভঙ্গ;
	}

	class_T *ifcl = tv.vval.v_class;
	যদি (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    ভঙ্গ;
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

    যদি (!success)
	প্রত্যাগম FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_data;
    cl->class_interface_গুনতি = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    প্রতি (পূর্ণাঙ্ক i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_data)[i],
		((class_T **)intf_classes_gap->ga_data)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = রিক্ত;
    cl->class_interface_গুনতি = 0;
    cl->class_extends = রিক্ত;

    প্রত্যাগম success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    স্থির পূর্ণাঙ্ক
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    প্রতি (পূর্ণাঙ্ক j = 0; j < intf_classes_gap->ga_len; j++)
    {
	যদি (((class_T **)intf_classes_gap->ga_data)[j] == ifcl)
	    প্রত্যাগম TRUE;
    }

    প্রত্যাগম FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    স্থির পূর্ণাঙ্ক
add_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.length);
    যদি (intf_name == রিক্ত)
	প্রত্যাগম FALSE;

    যদি (ga_grow(impl_gap, 1) == FAIL)
	গচ্ছ fail;

    char_u **intf_names = (char_u **)impl_gap->ga_data;
    intf_names[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    যদি (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	গচ্ছ fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_data;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    প্রত্যাগম TRUE;
fail:
    vim_free(intf_name);
    প্রত্যাগম FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    স্থির পূর্ণাঙ্ক
add_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    প্রতি (পূর্ণাঙ্ক i = 0; i < super->class_interface_গুনতি; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	যদি (!is_interface_class_present(intf_classes_gap, ifcl))
	    add_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    প্রত্যাগম TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    স্থির পূর্ণাঙ্ক
check_func_arg_names(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    প্রতি (পূর্ণাঙ্ক loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	প্রতি (পূর্ণাঙ্ক fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_data)[fi];

	    প্রতি (পূর্ণাঙ্ক i = 0; i < uf->uf_args.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_args.ga_data)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		প্রতি (পূর্ণাঙ্ক mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_data + mi)->ocm_name.string;
		    যদি (STRCMP(aname, mname) == 0)
		    {
			যদি (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_argument_already_declared_in_class_str),
				aname);

			প্রত্যাগম FALSE;
		    }
		}
	    }
	}
    }

    প্রত্যাগম TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    স্থির পূর্ণাঙ্ক
is_reserved_varname(char_u *varname, char_u *varname_end)
{
    পূর্ণাঙ্ক reserved = FALSE;
    char_u save_varname_end = *varname_end;
    *varname_end = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_end = save_varname_end;

    প্রত্যাগম reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    স্থির পূর্ণাঙ্ক
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_end)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_end - varname)};
    পূর্ণাঙ্ক		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    যদি (*pstr.string == '_')
    {
	pstr.string++;
	pstr.length--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    প্রতি (পূর্ণাঙ্ক loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	প্রতি (পূর্ণাঙ্ক i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_data) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.length};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    যদি (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.length--;
		q_protected = true;
	    }

	    যদি (pstr.length == qstr.length
		&& STRNCMP(pstr.string, qstr.string, pstr.length) == 0)
	    {
		char_u	save_c = *varname_end;

		*varname_end = NUL;
		যদি (p_protected != q_protected)
		    semsg(_(e_public_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		অন্যথা
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_end = save_c;
		dup = TRUE;
		ভঙ্গ;
	    }
	}
    }

    প্রত্যাগম dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    স্থির পূর্ণাঙ্ক
is_duplicate_method(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    প্রতি (পূর্ণাঙ্ক loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	প্রতি (পূর্ণাঙ্ক i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_data)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    যদি (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		প্রত্যাগম TRUE;
	    }
	}
    }

    প্রত্যাগম FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    স্থির পূর্ণাঙ্ক
is_valid_constructor(ufunc_T *uf, পূর্ণাঙ্ক is_abstract, পূর্ণাঙ্ক has_static)
{
    // Constructors are not allowed in abstract classes.
    যদি (is_abstract)
    {
	emsg(_(e_cannot_define_new_method_in_abstract_class));
	প্রত্যাগম FALSE;
    }
    // A constructor is always static, no need to define it so.
    যদি (has_static)
    {
	emsg(_(e_cannot_define_new_method_as_static));
	প্রত্যাগম FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    যদি (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_method));
	প্রত্যাগম FALSE;
    }
    প্রত্যাগম TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    স্থির পূর্ণাঙ্ক
object_check_builtin_method_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    পূর্ণাঙ্ক	    valid = FALSE;
    type_T  method_sig;
    type_T  method_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_FIELD(method_sig);
    CLEAR_FIELD(method_rt);
    method_sig.tt_type = VAR_FUNC;

    যদি (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	method_rt.tt_type = VAR_সংখ্যা;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    অন্যথা যদি (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	method_rt.tt_type = VAR_BOOL;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    অন্যথা যদি (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	method_rt.tt_type = VAR_STRING;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    অন্যথা
	semsg(_(e_builtin_object_method_str_not_supported), uf->uf_name);

    where.wt_func_name = (বর্ণ *)uf->uf_name;
    where.wt_kind = WT_METHOD;
    যদি (valid && !check_type(&method_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    প্রত্যাগম valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    পূর্ণাঙ্ক
is_valid_builtin_obj_methodname(char_u *funcname)
{
    বিকল্প (funcname[0])
    {
	পক্ষ 'e':
	    প্রত্যাগম STRNCMP(funcname, "empty", 5) == 0;

	পক্ষ 'l':
	    প্রত্যাগম STRNCMP(funcname, "len", 3) == 0;

	পক্ষ 'n':
	    প্রত্যাগম STRNCMP(funcname, "new", 3) == 0;

	পক্ষ 's':
	    প্রত্যাগম STRNCMP(funcname, "string", 6) == 0;
    }

    প্রত্যাগম FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_method(
    class_T		*cl,
    class_builtin_T	builtin_method,
    পূর্ণাঙ্ক			*method_idx)
{
    *method_idx = -1;

    যদি (cl == রিক্ত)
	প্রত্যাগম রিক্ত;

    *method_idx = cl->class_builtin_methods[builtin_method];
    প্রত্যাগম *method_idx != -1 ? cl->class_obj_methods[*method_idx] : রিক্ত;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    স্থির পূর্ণাঙ্ক
update_member_method_lookup_table(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    পূর্ণাঙ্ক		pobj_method_offset)
{
    যদি (ifcl == রিক্ত)
	প্রত্যাগম OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(পরিমাণ(itf2class_T)
				+ ifcl->class_obj_member_গুনতি * পরিমাণ(পূর্ণাঙ্ক));
    যদি (if2cl == রিক্ত)
	প্রত্যাগম FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = FALSE;

    প্রতি (পূর্ণাঙ্ক if_i = 0; if_i < ifcl->class_obj_member_গুনতি; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	প্রতি (পূর্ণাঙ্ক cl_i = 0; cl_i < cl->class_obj_member_গুনতি; ++cl_i)
	{
	    যদি (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		পূর্ণাঙ্ক *table = (পূর্ণাঙ্ক *)(if2cl + 1);
		table[if_i] = cl_i;
		ভঙ্গ;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(পরিমাণ(itf2class_T)
				+ ifcl->class_obj_method_গুনতি * পরিমাণ(পূর্ণাঙ্ক));
    যদি (if2cl == রিক্ত)
	প্রত্যাগম FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = TRUE;

    প্রতি (পূর্ণাঙ্ক if_i = 0; if_i < ifcl->class_obj_method_গুনতি; ++if_i)
    {
	পূর্ণাঙ্ক	done = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	প্রতি (পূর্ণাঙ্ক cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    যদি (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_data)[cl_i]->uf_name) == 0)
	    {
		পূর্ণাঙ্ক *table = (পূর্ণাঙ্ক *)(if2cl + 1);
		table[if_i] = cl_i;
		done = TRUE;
		ভঙ্গ;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	যদি (!done)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    যদি (cl->class_extends != ifcl)
	    {
		class_T		*parent = cl->class_extends;
		পূর্ণাঙ্ক		method_offset = objmethods->ga_len;

		যাবত্ (!done && parent != রিক্ত && parent != ifcl)
		{
		    প্রতি (পূর্ণাঙ্ক cl_i = 0;
			    cl_i < parent->class_obj_method_গুনতি_child; ++cl_i)
		    {
			যদি (STRCMP(if_name,
				    parent->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    পূর্ণাঙ্ক *table = (পূর্ণাঙ্ক *)(if2cl + 1);
			    table[if_i] = method_offset + cl_i;
			    done = TRUE;
			    ভঙ্গ;
			}
		    }
		    method_offset += parent->class_obj_method_গুনতি_child;
		    parent = parent->class_extends;
		}
	    }

	    যদি (!done)
	    {
		পূর্ণাঙ্ক *table = (পূর্ণাঙ্ক *)(if2cl + 1);
		table[if_i] = pobj_method_offset + if_i;
	    }
	}
    }

    প্রত্যাগম OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    স্থির পূর্ণাঙ্ক
add_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_interface_গুনতি; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	যাবত্ (ifcl != রিক্ত)
	{
	    যদি (update_member_method_lookup_table(ifcl, cl, objmethods_gap,
								0) == FAIL)
		প্রত্যাগম FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    যদি (extends_cl != রিক্ত)
    {
	class_T		*pclass = extends_cl;
	পূর্ণাঙ্ক		pobj_method_offset = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	যাবত্ (pclass != রিক্ত)
	{
	    যদি (update_member_method_lookup_table(pclass, cl,
			objmethods_gap, pobj_method_offset) == FAIL)
		প্রত্যাগম FAIL;

	    pobj_method_offset += pclass->class_obj_method_গুনতি_child;
	    pclass = pclass->class_extends;
	}
    }

    প্রত্যাগম OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    স্থির পূর্ণাঙ্ক
add_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_গুনতি);
    যদি (cl->class_members_tv == রিক্ত)
	প্রত্যাগম FAIL;

    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_class_member_গুনতি; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	যদি (m->ocm_init != রিক্ত)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_রাশিমালা(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    যদি (etv == রিক্ত)
		প্রত্যাগম FAIL;

	    যদি (m->ocm_type->tt_type == VAR_ANY
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
	অন্যথা
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = রিক্ত;
	}
	set_tv_type(tv, m->ocm_type);
	যদি (m->ocm_flags & OCMFLAG_CONST)
	    item_lock(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    প্রত্যাগম OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    স্থির শূন্য
add_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    পূর্ণাঙ্ক		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_obj_member_গুনতি; ++i)
    {
	যদি (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    অনুবর্ত;

	যদি (i > (is_enum ? 2 : 0))
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

    garray_T সারিগুলি_to_free;
    ga_init2(&সারিগুলি_to_free, পরিমাণ(char_u *), 50);

    ufunc_T *nf = define_function(&fea, রিক্ত, &সারিগুলি_to_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_গুনতি,
			    রিক্ত);

    ga_clear_strings(&সারিগুলি_to_free);
    vim_free(fga.ga_data);

    যদি (nf != রিক্ত && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_data)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	যদি (nf->uf_ret_type != রিক্ত)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_args = রিক্ত;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    স্থির পূর্ণাঙ্ক
add_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    প্রতি (পূর্ণাঙ্ক loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	পূর্ণাঙ্ক		*fcount = loop == 1 ? &cl->class_class_function_গুনতি
						: &cl->class_obj_method_গুনতি;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	পূর্ণাঙ্ক parent_গুনতি = 0;
	যদি (extends_cl != রিক্ত)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    parent_গুনতি = loop == 1
				? 0
				: extends_cl->class_obj_method_গুনতি;

	*fcount = parent_গুনতি + gap->ga_len;
	যদি (*fcount == 0)
	{
	    *fup = রিক্ত;
	    অনুবর্ত;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	যদি (*fup == রিক্ত)
	    প্রত্যাগম FAIL;

	যদি (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_data, পরিমাণ(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_data);
	যদি (loop == 1)
	    cl->class_class_function_গুনতি_child = gap->ga_len;
	অন্যথা
	    cl->class_obj_method_গুনতি_child = gap->ga_len;

	যদি (loop == 2)
	{
	    // Copy instance methods from the parent.

	    প্রতি (পূর্ণাঙ্ক i = 0; i < parent_গুনতি; ++i)
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
		যদি (nf != রিক্ত && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_args != রিক্ত)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		প্রতি (পূর্ণাঙ্ক ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    যদি (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (বর্ণ *)pname;
			where.wt_kind = WT_METHOD;
			(শূন্য)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	প্রতি (পূর্ণাঙ্ক i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    যদি (i < gap->ga_len)
		fp->uf_defclass = cl;
	    যদি (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    প্রত্যাগম OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    স্থির শূন্য
update_builtin_method_index(class_T *cl)
{
    পূর্ণাঙ্ক	i;

    প্রতি (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    প্রতি (i = 0; i < cl->class_obj_method_গুনতি; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	যদি (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	অন্যথা যদি (cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] = i;
	অন্যথা যদি (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    স্থির char_u *
find_class_name_end(char_u *arg)
{
    char_u *end = arg;

    যাবত্ (ASCII_ISALNUM(*end) || *end == '_'
	    || (*end == '.' && (ASCII_ISALNUM(end[1]) || end[1] == '_')))
	++end;

    প্রত্যাগম end;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    স্থির পূর্ণাঙ্ক
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
    পূর্ণাঙ্ক		dup = FALSE;

    প্রতি (পূর্ণাঙ্ক i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_data) + i;

	যদি (name.length == m->ocm_name.length
	    && STRNCMP(name.string, m->ocm_name.string, name.length) == 0)
	{
	    char_u  save_c = *varname_end;

	    *varname_end = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_end = save_c;
	    dup = TRUE;
	    ভঙ্গ;
	}
    }

    প্রত্যাগম dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    স্থির পূর্ণাঙ্ক
enum_parse_মানসমূহ(
    exarg_T	*eap,
    class_T	*en,
    char_u	*সারি,
    garray_T	*gap,
    পূর্ণাঙ্ক		*অঙ্ক_enum_মানসমূহ,
    পূর্ণাঙ্ক		*enum_end)
{
    evalarg_T	evalarg;
    char_u	*p = সারি;
    বর্ণ	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	last_char = NUL;
    পূর্ণাঙ্ক		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    পূর্ণাঙ্ক		did_emsg_before = did_emsg;
    যাবত্ (*p != NUL)
    {
	// ignore comment
	যদি (*p == '#')
	    ভঙ্গ;

	যদি (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_মান_declaration_str), p);
	    ভঙ্গ;
	}

	char_u *eni_name_start = p;
	char_u *eni_name_end = to_name_end(p, FALSE);

	যদি (is_duplicate_enum(gap, eni_name_start, eni_name_end))
	    ভঙ্গ;

	p = skipwhite(eni_name_end);

	char_u	*init_রাশিমালা = রিক্ত;
	যদি (*p == '(' || *p == '<')
	{
	    যদি (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", সারি);
		ভঙ্গ;
	    }

	    char_u *রাশিমালা_start, *রাশিমালা_end;

	    p = eni_name_start;
	    (শূন্য)skip_রাশিমালা_concatenate(&p, &রাশিমালা_start, &রাশিমালা_end, &evalarg);

	    যাবত্ (*রাশিমালা_start && *রাশিমালা_start != '(' && *রাশিমালা_start != '<')
		রাশিমালা_start++;

	    যদি (রাশিমালা_end > রাশিমালা_start)
		init_রাশিমালা = vim_strnsave(রাশিমালা_start, রাশিমালা_end - রাশিমালা_start);
	}

	যদি (init_রাশিমালা == রিক্ত)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, পরিমাণ(initexpr_buf), "%s.new()", en->class_name.string);
	অন্যথা
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, পরিমাণ(initexpr_buf), "%s.new%s", en->class_name.string,
		init_রাশিমালা);
	    vim_free(init_রাশিমালা);
	}
	যদি (add_member(gap, eni_name_start, eni_name_end, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    ভঙ্গ;

	++*অঙ্ক_enum_মানসমূহ;

	যদি (*p != '#')
	    last_char = *p;

	যদি (*p != NUL && *p != ',')
	    ভঙ্গ;

	যদি (*p == ',')
	{
	    যদি (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", সারি);
		ভঙ্গ;
	    }
	    যদি (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", সারি);
		ভঙ্গ;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    যদি (*p != NUL && *p != '#')
    {
	যদি (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_argument_str), p);
	rc = FAIL;
    }

    যদি (last_char != ',')
	// last enum value should not be terminated by ","
	*enum_end = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, রিক্ত);

    প্রত্যাগম rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    স্থির পূর্ণাঙ্ক
enum_add_মানসমূহ_member(
    class_T	*en,
    garray_T	*gap,
    পূর্ণাঙ্ক		অঙ্ক_enum_মানসমূহ,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    পূর্ণাঙ্ক		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক_enum_মানসমূহ; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;

	যদি (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.length);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    যদি (type == রিক্ত)
	গচ্ছ done;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    যদি (type->tt_member != রিক্ত)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = add_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_data, fga.ga_len));

done:
    vim_free(fga.ga_data);

    প্রত্যাগম rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    স্থির শূন্য
enum_clear_constructors(class_T *en)
{
    প্রতি (পূর্ণাঙ্ক i = 0; i < en->class_class_function_গুনতি; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	যদি (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    শূন্য
enum_set_internal_obj_vars(class_T *en, object_T *enval)
{
    পূর্ণাঙ্ক	i;

    প্রতি (i = 0; i < en->class_class_member_গুনতি; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	যদি (en_tv != রিক্ত && en_tv->v_type == VAR_UNKNOWN)
	    ভঙ্গ;
    }

    যদি (i >= en->class_class_member_গুনতি)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	প্রত্যাগম;

    // First object variable is the name
    ocmember_T *মান_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(মান_ocm->ocm_name.string, মান_ocm->ocm_name.length);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_সংখ্যা;
    ord_tv->vval.v_সংখ্যা = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    স্থির শূন্য
skip_class_body(exarg_T *eap)
{
    প্রতি (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	যদি (theline == রিক্ত)
	    ভঙ্গ;

	char_u	*p = theline;
	পূর্ণাঙ্ক	found;

	// skip ':' and blanks
	প্রতি (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	found = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	যদি (found)
	    ভঙ্গ;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    শূন্য
ex_class(exarg_T *eap)
{
    পূর্ণাঙ্ক		is_class = eap->cmdidx == CMD_class;
    পূর্ণাঙ্ক		is_abstract = eap->cmdidx == CMD_abstract;
    পূর্ণাঙ্ক		is_enum = eap->cmdidx == CMD_enum;
    পূর্ণাঙ্ক		added_enum_মানসমূহ = FALSE;
    পূর্ণাঙ্ক		is_interface;
    long	start_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    যদি (eap->skip)
    {
	skip_class_body(eap);
	প্রত্যাগম;
    }

    যদি (is_abstract)
    {
	যদি (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_argument_str), arg);
	    প্রত্যাগম;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    যদি (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	যদি (is_class)
	    emsg(_(e_class_can_only_be_defined_in_vim9_script));
	অন্যথা যদি (is_enum)
	    emsg(_(e_enum_can_only_be_defined_in_vim9_script));
	অন্যথা
	    emsg(_(e_interface_can_only_be_defined_in_vim9_script));
	প্রত্যাগম;
    }

    যদি (!ASCII_ISUPPER(*arg))
    {
	যদি (is_class)
	    semsg(_(e_class_name_must_start_with_uppercase_letter_str), arg);
	অন্যথা যদি (is_enum)
	    semsg(_(e_enum_name_must_start_with_uppercase_letter_str), arg);
	অন্যথা
	    semsg(_(e_interface_name_must_start_with_uppercase_letter_str),
									  arg);
	প্রত্যাগম;
    }
    char_u *name_end = find_name_end(arg, রিক্ত, রিক্ত, FNE_CHECK_START);
    যদি (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	প্রত্যাগম;
    }
    char_u *name_start = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = রিক্ত;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, পরিমাণ(char_u *), 5);

    arg = skipwhite(name_end);
    যাবত্ (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	যদি (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    যদি (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		গচ্ছ early_ret;
	    }
	    যদি (extends != রিক্ত)
	    {
		emsg(_(e_duplicate_extends));
		গচ্ছ early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *end = find_class_name_end(arg);
	    যদি (!IS_WHITE_OR_NUL(*end))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		গচ্ছ early_ret;
	    }
	    extends = vim_strnsave(arg, end - arg);
	    যদি (extends == রিক্ত)
		গচ্ছ early_ret;

	    arg = skipwhite(end + 1);
	}
	অন্যথা যদি (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    যদি (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		গচ্ছ early_ret;
	    }

	    যদি (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		গচ্ছ early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    প্রতি (;;)
	    {
		char_u *impl_end = find_class_name_end(arg);
		যদি ((!IS_WHITE_OR_NUL(*impl_end) && *impl_end != ',')
			|| (*impl_end == ','
			    && !IS_WHITE_OR_NUL(*(impl_end + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    গচ্ছ early_ret;
		}
		যদি (impl_end - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    গচ্ছ early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_end - arg);
		যদি (iname == রিক্ত)
		    গচ্ছ early_ret;
		প্রতি (পূর্ণাঙ্ক i = 0; i < ga_impl.ga_len; ++i)
		    যদি (STRCMP(((char_u **)ga_impl.ga_data)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			গচ্ছ early_ret;
		    }
		যদি (ga_add_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    গচ্ছ early_ret;
		}
		যদি (*impl_end != ',')
		{
		    arg = skipwhite(impl_end);
		    ভঙ্গ;
		}
		arg = skipwhite(impl_end + 1);
	    }
	}
	অন্যথা
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_strings(&ga_impl);
	    প্রত্যাগম;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, পরিমাণ(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, পরিমাণ(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, পরিমাণ(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, পরিমাণ(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, পরিমাণ(ufunc_T *), 10);

    class_T *cl = রিক্ত;
    class_T *extends_cl = রিক্ত;  // class from "extends" argument
    class_T **intf_classes = রিক্ত;
    পূর্ণাঙ্ক	    intf_গুনতি = 0;	// number of items in "intf_classes"
    পূর্ণাঙ্ক	    অঙ্ক_enum_মানসমূহ = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    যদি (cl == রিক্ত)
	গচ্ছ cleanup;

    যদি (is_enum)
	cl->class_flags = CLASS_ENUM;
    অন্যথা যদি (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    অন্যথা যদি (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.length = (size_t)(name_end - name_start);
    cl->class_name.string = vim_strnsave(name_start, cl->class_name.length);
    যদি (cl->class_name.string == রিক্ত)
	গচ্ছ cleanup;

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
    পূর্ণাঙ্ক rc = set_var_const(cl->class_name.string, 0, রিক্ত, &tv, FALSE, 0, 0);
    যদি (rc == FAIL)
	গচ্ছ cleanup;

    যদি (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	যদি (add_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, রিক্ত) == FAIL)
	    গচ্ছ cleanup;

	varname = (char_u *)"ordinal";
	যদি (add_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_সংখ্যা, রিক্ত) == FAIL)
	    গচ্ছ cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    পূর্ণাঙ্ক		enum_end = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = রিক্ত;
    পূর্ণাঙ্ক success = FALSE;
    প্রতি (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	যদি (theline == রিক্ত)
	    ভঙ্গ;
	char_u *সারি = skipwhite(theline);

	// Skip empty and comment lines.
	যদি (*সারি == NUL)
	    অনুবর্ত;
	যদি (*সারি == '#')
	{
	    যদি (vim9_bad_comment(সারি))
		ভঙ্গ;
	    অনুবর্ত;
	}

	char_u *p = সারি;

	বর্ণ	*end_name;
	পূর্ণাঙ্ক	shortlen;
	পূর্ণাঙ্ক	fullen;
	যদি (is_class)
	{
	    end_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	অন্যথা যদি (is_enum)
	{
	    end_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	অন্যথা
	{
	    end_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	প্রতি (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_start = p;
	যদি (checkforcmd(&p, end_name, shortlen))
	{
	    যদি (STRNCMP(cmd_start, end_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_start);
	    অন্যথা যদি (*p == '|' || !ends_excmd2(সারি, p))
		semsg(_(e_trailing_characters_str), p);
	    অন্যথা
		success = TRUE;
	    ভঙ্গ;
	}

	পূর্ণাঙ্ক	wrong_endname = FALSE;
	যদি (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	অন্যথা যদি (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	অন্যথা
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	যদি (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), সারি, end_name);
	    ভঙ্গ;
	}

	যদি (is_enum && !enum_end)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    যদি (enum_parse_মানসমূহ(eap, cl, সারি, &classmembers,
					&অঙ্ক_enum_মানসমূহ, &enum_end) == FAIL)
		ভঙ্গ;

	    যদি (enum_end)
	    {
		// Add the enum "values" class variable.
		enum_add_মানসমূহ_member(cl, &classmembers, অঙ্ক_enum_মানসমূহ,
							&type_list);
		added_enum_মানসমূহ = TRUE;
	    }
	    অনুবর্ত;
	}

	পূর্ণাঙ্ক has_public = FALSE;
	যদি (checkforcmd(&p, "public", 3))
	{
	    যদি (STRNCMP(সারি, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), সারি);
		ভঙ্গ;
	    }
	    যদি (is_interface)
	    {
		emsg(_(e_public_variable_not_supported_in_interface));
		ভঙ্গ;
	    }
	    has_public = TRUE;
	    p = skipwhite(সারি + 6);

	    যদি (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_public_keyword_not_supported_for_method));
		ভঙ্গ;
	    }

	    যদি (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_public_must_be_followed_by_var_static_final_or_const));
		ভঙ্গ;
	    }
	}

	পূর্ণাঙ্ক abstract_method = FALSE;
	char_u *pa = p;
	যদি (checkforcmd(&p, "abstract", 3))
	{
	    যদি (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		ভঙ্গ;
	    }

	    যদি (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_used_in_enum));
		ভঙ্গ;
	    }

	    যদি (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_used_in_interface));
		ভঙ্গ;
	    }

	    যদি (!is_abstract)
	    {
		semsg(_(e_abstract_method_in_concrete_class), pa);
		ভঙ্গ;
	    }

	    p = skipwhite(pa + 8);
	    যদি (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		ভঙ্গ;
	    }

	    abstract_method = TRUE;
	}

	পূর্ণাঙ্ক has_static = FALSE;
	char_u *ps = p;
	যদি (checkforcmd(&p, "static", 4))
	{
	    যদি (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		ভঙ্গ;
	    }

	    যদি (is_interface)
	    {
		emsg(_(e_static_member_not_supported_in_interface));
		ভঙ্গ;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    যদি (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		ভঙ্গ;
	    }
	}

	পূর্ণাঙ্ক has_final = FALSE;
	পূর্ণাঙ্ক has_var = FALSE;
	পূর্ণাঙ্ক has_const = FALSE;
	যদি (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	অন্যথা যদি (checkforcmd(&p, "final", 5))
	{
	    যদি (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_in_interface));
		ভঙ্গ;
	    }
	    has_final = TRUE;
	}
	অন্যথা যদি (checkforcmd(&p, "const", 5))
	{
	    যদি (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_in_interface));
		ভঙ্গ;
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
	যদি (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_end = রিক্ত;
	    type_T *type = রিক্ত;
	    char_u *init_রাশিমালা = রিক্ত;
	    পূর্ণাঙ্ক	    has_type = FALSE;

	    যদি (!eval_isnamec1(*p))
	    {
		যদি (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), সারি);
		অন্যথা
		    semsg(_(e_invalid_object_variable_declaration_str), সারি);
		ভঙ্গ;
	    }

	    যদি (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_in_interface),
			varname);
		ভঙ্গ;
	    }

	    যদি (parse_member(eap, সারি, varname, has_public,
			  &varname_end, &has_type, &type_list, &type,
			  !is_interface ? &init_রাশিমালা: রিক্ত) == FAIL)
		ভঙ্গ;

	    যদি (is_reserved_varname(varname, varname_end)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_end))
	    {
		vim_free(init_রাশিমালা);
		ভঙ্গ;
	    }
	    যদি (add_member(has_static ? &classmembers : &objmembers, varname,
				varname_end, has_public, has_final, has_const,
				has_type, type, init_রাশিমালা) == FAIL)
	    {
		vim_free(init_রাশিমালা);
		ভঙ্গ;
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
	অন্যথা যদি (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	সারিগুলি_to_free;
	    পূর্ণাঙ্ক		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    যদি (has_public)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_public_keyword_not_supported_for_method));
		ভঙ্গ;
	    }

	    যদি (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_in_class_str), সারি);
		ভঙ্গ;
	    }

	    যদি (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_method_not_supported_in_interface), p);
		ভঙ্গ;
	    }

	    যদি (has_static && !is_new && SAFE_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_method_not_supported), p);
		ভঙ্গ;
	    }

	    CLEAR_FIELD(ea);
	    ea.cmd = সারি;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&সারিগুলি_to_free, পরিমাণ(char_u *), 50);
	    পূর্ণাঙ্ক class_flags;
	    যদি (is_interface)
		class_flags = CF_INTERFACE;
	    অন্যথা
		class_flags = abstract_method ? CF_ABSTRACT_METHOD : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, রিক্ত, &সারিগুলি_to_free,
			class_flags, objmembers.ga_data, objmembers.ga_len,
			রিক্ত);
	    ga_clear_strings(&সারিগুলি_to_free);

	    যদি (uf != রিক্ত)
	    {
		যদি (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_method_not_supported_in_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    ভঙ্গ;
		}

		// check for builtin method
		যদি (!is_new && SAFE_islower(*uf->uf_name) &&
					!object_check_builtin_method_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    ভঙ্গ;
		}

		// Check the uf_name isn't used already.
		যদি (is_duplicate_method(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    ভঙ্গ;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		যদি (ga_grow(fgap, 1) == OK)
		{
		    যদি (is_new)
			uf->uf_flags |= FC_NEW;

		    যদি (abstract_method)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_data)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	অন্যথা
	{
	    যদি (is_class)
		semsg(_(e_not_valid_command_in_class_str), সারি);
	    অন্যথা যদি (is_enum)
		semsg(_(e_not_valid_command_in_enum_str), সারি);
	    অন্যথা
		semsg(_(e_not_valid_command_in_interface_str), সারি);
	    ভঙ্গ;
	}
    }

    যদি (theline == রিক্ত && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    যদি (success && is_enum && (অঙ্ক_enum_মানসমূহ == 0 || !added_enum_মানসমূহ))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_add_মানসমূহ_member(cl, &classmembers, অঙ্ক_enum_মানসমূহ,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    যদি (success && extends != রিক্ত)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    যদি (success && extends_cl != রিক্ত)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    যদি (success && extends_cl != রিক্ত)
    {
	যদি (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	অন্যথা
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    যদি (success && !is_abstract && extends_cl != রিক্ত
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, পরিমাণ(class_T *), 5);

    যদি (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    যদি (success && extends_cl != রিক্ত)
	success = add_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_data;
    intf_গুনতি = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    যদি (success)
	success = check_func_arg_names(&classfunctions, &objmethods,
							&classmembers);

    যদি (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	যদি (extends_cl != রিক্ত)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	যদি (add_members_to_class(&classmembers,
				 রিক্ত,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_গুনতি) == FAIL
		|| add_members_to_class(&objmembers,
				 extends_cl == রিক্ত ? রিক্ত
					       : extends_cl->class_obj_members,
				 extends_cl == রিক্ত ? 0
					  : extends_cl->class_obj_member_গুনতি,
				 &cl->class_obj_members,
				 &cl->class_obj_member_গুনতি) == FAIL)
	    গচ্ছ cleanup;

	যদি (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_গুনতি = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    যদি (cl->class_interfaces == রিক্ত)
		গচ্ছ cleanup;
	    প্রতি (পূর্ণাঙ্ক i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_data)[i];
	    VIM_CLEAR(ga_impl.ga_data);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = রিক্ত;
	}

	যদি (cl->class_interface_গুনতি > 0 || extends_cl != রিক্ত)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    যদি (add_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		গচ্ছ cleanup;
	}

	পূর্ণাঙ্ক	have_new = FALSE;
	ufunc_T	*class_func = রিক্ত;
	প্রতি (পূর্ণাঙ্ক i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_data)[i];
	    যদি (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		ভঙ্গ;
	    }
	}

	যদি (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	অন্যথা যদি ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    add_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	যদি (add_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    গচ্ছ cleanup;

	update_builtin_method_index(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	যদি ((is_class || is_enum) && cl->class_class_member_গুনতি > 0)
	    যদি (add_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		প্রত্যাগম;
	    }

	cl->class_type_list = type_list;

	যদি (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	প্রত্যাগম;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    যদি (intf_classes != রিক্ত)
    {
	প্রতি (পূর্ণাঙ্ক i = 0; i < intf_গুনতি; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_strings(&ga_impl);

    প্রতি (পূর্ণাঙ্ক round = 1; round <= 2; ++round)
    {
	garray_T *gap = round == 1 ? &classmembers : &objmembers;
	যদি (gap->ga_len == 0 || gap->ga_data == রিক্ত)
	    অনুবর্ত;

	প্রতি (পূর্ণাঙ্ক i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    প্রতি (পূর্ণাঙ্ক i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    প্রতি (পূর্ণাঙ্ক i = 0; i < classfunctions.ga_len; ++i)
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
    পূর্ণাঙ্ক		is_object,
    char_u	*name,
    char_u	*name_end,
    পূর্ণাঙ্ক		*member_idx)
{
    size_t	len = name_end - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    যদি (m == রিক্ত)
    {
	member_not_found_বার্তা(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	প্রত্যাগম &t_any;
    }

    প্রত্যাগম m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    পূর্ণাঙ্ক		is_object,
    পূর্ণাঙ্ক		member_idx)
{
    ocmember_T	*m;
    পূর্ণাঙ্ক		member_গুনতি;

    যদি (is_object)
    {
	m = cl->class_obj_members;
	member_গুনতি = cl->class_obj_member_গুনতি;
    }
    অন্যথা
    {
	m = cl->class_class_members;
	member_গুনতি = cl->class_class_member_গুনতি;
    }

    যদি (member_idx >= member_গুনতি)
	প্রত্যাগম রিক্ত;

    প্রত্যাগম m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    স্থির শূন্য
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    শূন্য
typealias_unref(typealias_T *ta)
{
    যদি (ta != রিক্ত && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    শূন্য
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    যদি (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_in_vim9_script));
	প্রত্যাগম;
    }

    যদি (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	প্রত্যাগম;
    }

    যদি (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_start_with_uppercase_letter_str), arg);
	প্রত্যাগম;
    }

    char_u *name_end = find_name_end(arg, রিক্ত, রিক্ত, FNE_CHECK_START);
    যদি (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	প্রত্যাগম;
    }
    char_u *name_start = arg;

    arg = skipwhite(name_end);
    যদি (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	প্রত্যাগম;
    }
    যদি (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	প্রত্যাগম;
    }
    arg++;
    arg = skipwhite(arg);

    যদি (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	প্রত্যাগম;
    }

    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, রিক্ত, রিক্ত, TRUE);
    যদি (type == রিক্ত)
	প্রত্যাগম;

    যদি (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	প্রত্যাগম;
    }

    পূর্ণাঙ্ক cc = *name_end;
    *name_end = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    যদি (eval_variable_import(name_start, &tv) == OK)
    {
	যদি (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_start);
	অন্যথা
	    semsg(_(e_redefining_script_item_str), name_start);
	clear_tv(&tv);
	গচ্ছ done;
    }

    // Create a script-local variable for the type alias.
    যদি (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_lock = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_start);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_depth = parse_type_depth();
    }
    অন্যথা
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_lock = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    set_var_const(name_start, current_sctx.sc_sid, রিক্ত, &tv, FALSE,
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
    পূর্ণাঙ্ক
get_member_tv(
    class_T	*cl,
    পূর্ণাঙ্ক		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    পূর্ণাঙ্ক		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    যদি (m == রিক্ত)
	প্রত্যাগম FAIL;

    যদি (*name == '_' && (current_class == রিক্ত ||
				!class_instance_of(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	প্রত্যাগম FAIL;
    }

    যদি (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	set_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    অন্যথা
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	set_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    প্রত্যাগম OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    স্থির পূর্ণাঙ্ক
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
    পূর্ণাঙ্ক		argcount = 0;
    ocmember_T	*ocm = রিক্ত;
    পূর্ণাঙ্ক		m_idx;

    fp = method_lookup(cl, rettv->v_type, name, len, রিক্ত);
    যদি (fp == রিক্ত)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	যদি (ocm == রিক্ত || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    method_not_found_বার্তা(cl, rettv->v_type, name, len);
	    প্রত্যাগম FAIL;
	}

	যদি (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    প্রত্যাগম FAIL;
	}

	যদি (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	অন্যথা
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_end;
	প্রত্যাগম OK;
    }

    যদি (ocm == রিক্ত && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
	প্রত্যাগম FAIL;
    }

    // process generic function call
    যদি (fp != রিক্ত)
    {
	fp = generic_func_get(fp, gfatab);
	যদি (fp == রিক্ত)
	    প্রত্যাগম FAIL;
    }

    char_u *argp = name_end;
    পূর্ণাঙ্ক ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    যদি (ret == FAIL)
    {
	যাবত্ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	প্রত্যাগম FAIL;
    }

    funcexe_T funcexe;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    যদি (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    যদি (evalarg != রিক্ত)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    পূর্ণাঙ্ক error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								রিক্ত);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    প্রতি (পূর্ণাঙ্ক idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    যদি (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_found_var);
	প্রত্যাগম FAIL;
    }
    *arg = argp;

    প্রত্যাগম OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    পূর্ণাঙ্ক
obj_method_to_partial_tv(object_T *obj, ufunc_T *obj_method, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    যদি (pt == রিক্ত)
	প্রত্যাগম FAIL;

    pt->pt_refcount = 1;
    যদি (obj != রিক্ত)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_method;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    প্রত্যাগম OK;
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
    পূর্ণাঙ্ক
class_object_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    পূর্ণাঙ্ক		verbose UNUSED)	// give error messages
{
    পূর্ণাঙ্ক	ret = FAIL;

    যদি (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	প্রত্যাগম FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, রিক্ত, রিক্ত, FNE_CHECK_START);
    যদি (name_end == name)
	প্রত্যাগম FAIL;
    size_t len = name_end - name;

    পূর্ণাঙ্ক did_emsg_save = did_emsg;
    class_T *cl;
    যদি (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    অন্যথা // VAR_OBJECT
    {
	যদি (rettv->vval.v_object == রিক্ত)
	{
	    emsg(_(e_using_null_object));
	    প্রত্যাগম FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    যদি (cl == রিক্ত)
    {
	emsg(_(e_incomplete_type));
	প্রত্যাগম FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    যদি (*name_end == '<')
    {
	cctx_T	    *cctx = রিক্ত;

	যদি (evalarg != রিক্ত)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_end = parse_generic_func_type_args(name, len, name + len,
						&gfatab, cctx);
	যদি (name_end == রিক্ত)
	    গচ্ছ done;
    }

    যদি (*name_end == '(')
	// Invoke the class or object method
	ret = call_oc_method(cl, name, len, name_end, evalarg, arg,
							&gfatab, rettv);
    অন্যথা যদি (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	পূর্ণাঙ্ক is_object = rettv->v_type == VAR_OBJECT;
	যদি (get_member_tv(cl, is_object, name, len, রিক্ত, rettv) == OK)
	{
	    *arg = name_end;
	    ret = OK;
	    গচ্ছ done;
	}

	// could be a class method or an object method
	পূর্ণাঙ্ক	fidx;
	ufunc_T	*fp = method_lookup(cl, rettv->v_type, name, len, &fidx);
	যদি (fp != রিক্ত)
	{
	    // Protected methods are not accessible outside the class
	    যদি (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		গচ্ছ done;
	    }

	    যদি (obj_method_to_partial_tv(is_object ? rettv->vval.v_object :
						রিক্ত, fp, rettv) == FAIL)
		গচ্ছ done;

	    *arg = name_end;
	    ret = OK;
	    গচ্ছ done;
	}

	যদি (did_emsg == did_emsg_save)
	    member_not_found_বার্তা(cl, rettv->v_type, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);

    প্রত্যাগম ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
find_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, রিক্ত, রিক্ত, FNE_CHECK_START);
    যদি (name_end == name || *name_end != '.')
	প্রত্যাগম রিক্ত;

    ufunc_T	*fp = রিক্ত;
    size_t	len = name_end - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    যদি (eval_variable(name, (পূর্ণাঙ্ক)len,
				    0, &tv, রিক্ত, EVAL_VAR_NOAUTOLOAD) == FAIL)
	প্রত্যাগম রিক্ত;
    যদি (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	গচ্ছ fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    যদি (cl == রিক্ত)
	গচ্ছ fail_after_eval;
    char_u *fname = name_end + 1;
    char_u *fname_end = find_name_end(fname, রিক্ত, রিক্ত, FNE_CHECK_START);
    যদি (fname_end == fname)
	গচ্ছ fail_after_eval;
    len = fname_end - fname;

    fp = method_lookup(cl, tv.v_type, fname, len, রিক্ত);

    যদি (fp != রিক্ত)
    {
	fp = eval_generic_func(fp, fname, &fname_end);
	যদি (fp != রিক্ত)
	    *arg = fname_end;
    }

fail_after_eval:
    clear_tv(&tv);
    প্রত্যাগম fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    পূর্ণাঙ্ক
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    পূর্ণাঙ্ক idx;
    class_member_lookup(cl, name, namelen, &idx);
    প্রত্যাগম idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, পূর্ণাঙ্ক *idx)
{
    ocmember_T	*ret_m = রিক্ত;
    পূর্ণাঙ্ক		ret_idx = -1;
    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_class_member_গুনতি; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	যদি (namelen)
	{
	    যদি (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ভঙ্গ;
	    }
	}
	অন্যথা যদি (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ভঙ্গ;
	}
    }
    যদি (idx != রিক্ত)
	*idx = ret_idx;
    প্রত্যাগম ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    স্থির ufunc_T *
class_method_lookup(class_T *cl, char_u *name, size_t namelen, পূর্ণাঙ্ক *idx)
{
    ufunc_T	*ret_fp = রিক্ত;
    পূর্ণাঙ্ক		ret_idx = -1;
    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_class_function_গুনতি; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	যদি (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ভঙ্গ;
	}
    }
    যদি (idx != রিক্ত)
	*idx = ret_idx;
    প্রত্যাগম ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    পূর্ণাঙ্ক
class_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    পূর্ণাঙ্ক idx;
    class_method_lookup(cl, name, namelen, &idx);
    প্রত্যাগম idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    স্থির পূর্ণাঙ্ক
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    পূর্ণাঙ্ক idx;
    object_member_lookup(cl, name, namelen, &idx);
    প্রত্যাগম idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, পূর্ণাঙ্ক *idx)
{
    ocmember_T	*ret_m = রিক্ত;
    পূর্ণাঙ্ক		ret_idx = -1;
    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_obj_member_গুনতি; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	যদি (namelen)
	{
	    যদি (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ভঙ্গ;
	    }
	}
	অন্যথা যদি (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ভঙ্গ;
	}
    }
    যদি (idx != রিক্ত)
	*idx = ret_idx;
    প্রত্যাগম ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    স্থির ufunc_T *
object_method_lookup(class_T *cl, char_u *name, size_t namelen, পূর্ণাঙ্ক *idx)
{
    ufunc_T	*ret_fp = রিক্ত;
    পূর্ণাঙ্ক		ret_idx = -1;
    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_obj_method_গুনতি; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	যদি (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ভঙ্গ;
	}
    }
    যদি (idx != রিক্ত)
	*idx = ret_idx;
    প্রত্যাগম ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    পূর্ণাঙ্ক
object_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    পূর্ণাঙ্ক idx;
    object_method_lookup(cl, name, namelen, &idx);
    প্রত্যাগম idx;
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
    পূর্ণাঙ্ক		*idx)
{
    যদি (v_type == VAR_CLASS)
	প্রত্যাগম class_member_lookup(cl, name, namelen, idx);

    প্রত্যাগম object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    স্থির class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_found = রিক্ত;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_found = রিক্ত;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    প্রতি (class_T *super = cl; super != রিক্ত; super = super->class_extends)
    {
	class_T		*cl_tmp = রিক্ত;
	ocmember_T	*m = রিক্ত;
	যদি (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    যদি ((m = object_member_lookup(super, name, len, রিক্ত)) != রিক্ত)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	যদি (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    যদি (( m = class_member_lookup(super, name, len, রিক্ত)) != রিক্ত)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	যদি (cl_tmp == রিক্ত)
	    ভঙ্গ;  // member is not in this or any super class.
	cl_found = cl_tmp;
	m_found = m;
    }
    যদি (p_m != রিক্ত)
	*p_m = m_found;
    প্রত্যাগম cl_found;
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
    পূর্ণাঙ্ক		*idx)
{
    যদি (v_type == VAR_CLASS)
	প্রত্যাগম class_method_lookup(cl, name, namelen, idx);
    অন্যথা
	প্রত্যাগম object_method_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    পূর্ণাঙ্ক
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    প্রতি (cctx_T *cctx = cctx_arg; cctx != রিক্ত; cctx = cctx->ctx_outer)
	যদি (cctx->ctx_ufunc != রিক্ত
			&& class_instance_of(cctx->ctx_ufunc->uf_class, cl))
	    প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    পূর্ণাঙ্ক
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    যদি (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_in_class_str),
		m->ocm_name.string, cl->class_name.string);
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    শূন্য
obj_lock_const_vars(object_T *obj)
{
    প্রতি (পূর্ণাঙ্ক i = 0; i < obj->obj_class->class_obj_member_গুনতি; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	যদি (ocm->ocm_flags & OCMFLAG_CONST)
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
    পূর্ণাঙ্ক		sz;

    যদি (cl == রিক্ত)
	প্রত্যাগম রিক্ত;

    sz = পরিমাণ(object_T) + cl->class_obj_member_গুনতি * পরিমাণ(typval_T);
    obj = alloc_clear(sz);
    যদি (obj == রিক্ত)
	প্রত্যাগম রিক্ত;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    যদি (IS_ENUM(cl))
	enum_set_internal_obj_vars(cl, obj);

    প্রত্যাগম obj;
}

/*
 * Make a copy of an object.
 */
    শূন্য
copy_object(typval_T *from, typval_T *to)
{
    যদি (from->vval.v_object == রিক্ত)
	to->vval.v_object = রিক্ত;
    অন্যথা
    {
	to->vval.v_object = from->vval.v_object;
	++to->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    শূন্য
copy_class(typval_T *from, typval_T *to)
{
    যদি (from->vval.v_class == রিক্ত)
	to->vval.v_class = রিক্ত;
    অন্যথা
    {
	to->vval.v_class = from->vval.v_class;
	++to->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    স্থির শূন্য
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_interface_গুনতি; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	যদি (cl->class_interfaces_cl[i] != রিক্ত)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *next;
    প্রতি (itf2class_T *i2c = cl->class_itf2class; i2c != রিক্ত; i2c = next)
    {
	next = i2c->i2c_next;
	vim_free(i2c);
    }

    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_class_member_গুনতি; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	যদি (cl->class_members_tv != রিক্ত)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_obj_member_গুনতি; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_class_function_গুনতি; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	যদি (IS_ENUM(cl) && IS_CONSTRUCTOR_METHOD(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_obj_method_গুনতি; ++i)
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
    স্থির পূর্ণাঙ্ক
class_get_selfrefs(class_T *cl)
{
    পূর্ণাঙ্ক		self_refs = 0;
    typval_T	*tv;

    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_class_member_গুনতি; ++i)
    {
	tv = &cl->class_members_tv[i];
	যদি (tv->v_type == VAR_OBJECT && tv->vval.v_object != রিক্ত
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    প্রত্যাগম self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    স্থির পূর্ণাঙ্ক
can_free_enum(class_T *cl)
{
    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_class_member_গুনতি; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	যদি (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    প্রত্যাগম TRUE;

	যদি (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    যদি (tv->vval.v_list->lv_refcount > 1)
		প্রত্যাগম FALSE;
	    ভঙ্গ;
	}

	যদি (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    প্রত্যাগম FALSE;
    }

    প্রত্যাগম TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    স্থির পূর্ণাঙ্ক
can_free_class(class_T *cl)
{
    পূর্ণাঙ্ক		can_free = FALSE;
    পূর্ণাঙ্ক		self_refs = 0;

    যদি (IS_ENUM(cl) && !can_free_enum(cl))
	প্রত্যাগম FALSE;

    যদি (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    যদি (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    প্রত্যাগম can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    শূন্য
class_unref(class_T *cl)
{
    যদি (cl == রিক্ত)
	প্রত্যাগম;

    --cl->class_refcount;

    যদি (cl->class_name.string == রিক্ত)
	প্রত্যাগম;

    যদি (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    পূর্ণাঙ্ক
class_free_nonref(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		did_free = FALSE;

    প্রতি (class_T *cl = first_class; cl != রিক্ত; cl = next_nonref_class)
    {
	next_nonref_class = cl->class_next_used;
	যদি ((cl->class_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    next_nonref_class = রিক্ত;
    প্রত্যাগম did_free;
}

    পূর্ণাঙ্ক
set_ref_in_classes(পূর্ণাঙ্ক copyID)
{
    প্রতি (class_T *cl = first_class; cl != রিক্ত; cl = cl->class_next_used)
	set_ref_in_item_class(cl, copyID, রিক্ত, রিক্ত, রিক্ত);

    প্রত্যাগম FALSE;
}

স্থির object_T *first_object = রিক্ত;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    শূন্য
object_created(object_T *obj)
{
    যদি (first_object != রিক্ত)
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
    স্থির শূন্য
object_cleared(object_T *obj)
{
    যদি (obj->obj_next_used != রিক্ত)
	obj->obj_next_used->obj_prev_used = obj->obj_prev_used;
    যদি (obj->obj_prev_used != রিক্ত)
	obj->obj_prev_used->obj_next_used = obj->obj_next_used;
    অন্যথা যদি (first_object == obj)
	first_object = obj->obj_next_used;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    স্থির শূন্য
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    যদি (!cl)
	প্রত্যাগম;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = INT_MAX;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_obj_member_গুনতি; ++i)
	clear_tv(tv + i);
}

    স্থির শূন্য
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    যদি (!cl)
	প্রত্যাগম;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    স্থির শূন্য
object_free(object_T *obj)
{
    যদি (in_free_unref_বস্তুসমূহ)
	প্রত্যাগম;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    শূন্য
object_unref(object_T *obj)
{
    যদি (obj != রিক্ত && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    পূর্ণাঙ্ক
object_free_nonref(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		did_free = FALSE;

    প্রতি (object_T *obj = first_object; obj != রিক্ত; obj = obj->obj_next_used)
    {
	যদি ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    প্রত্যাগম did_free;
}

    শূন্য
object_free_বস্তুসমূহ(পূর্ণাঙ্ক copyID)
{
    object_T	*obj_next;

    প্রতি (object_T *obj = first_object; obj != রিক্ত; obj = obj_next)
    {
	obj_next = obj->obj_next_used;
	যদি ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    শূন্য
emsg_var_cl_define(বর্ণ *বার্তা, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    যদি (cl_def != রিক্ত)
	semsg(_(বার্তা), m->ocm_name.string, cl_def->class_name.string);
    অন্যথা
	emsg(_(e_internal_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    শূন্য
method_not_found_বার্তা(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *method_name = vim_strnsave(name, len);
    যদি ((v_type == VAR_OBJECT)
	    && (class_method_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	যদি (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	অন্যথা
	    semsg(_(e_class_method_str_accessible_only_using_class_str),
		    method_name, cl->class_name.string);
    }
    অন্যথা যদি ((v_type == VAR_CLASS)
	    && (object_method_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	যদি (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	অন্যথা
	    semsg(_(e_object_method_str_accessible_only_using_object_str),
		    method_name, cl->class_name.string);
    }
    অন্যথা
	semsg(_(e_method_not_found_on_class_str_str), method_name,
		cl->class_name.string);
    vim_free(method_name);
}

/*
 * Echo a class or object member not found message.
 */
    শূন্য
member_not_found_বার্তা(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    যদি (v_type == VAR_OBJECT)
    {
	যদি (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	অন্যথা
	    semsg(_(e_variable_not_found_on_object_str_str), varname,
		    cl->class_name.string);
    }
    অন্যথা
    {
	যদি (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	অন্যথা
	{
	    যদি (IS_ENUM(cl))
		semsg(_(e_enum_মান_str_not_found_in_enum_str),
			varname, cl->class_name.string);
	    অন্যথা
		semsg(_(e_class_variable_str_not_found_in_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    শূন্য
defcompile_class(class_T *cl)
{
    প্রতি (পূর্ণাঙ্ক loop = 1; loop <= 2; ++loop)
    {
	পূর্ণাঙ্ক func_গুনতি = loop == 1 ? cl->class_class_function_গুনতি
						: cl->class_obj_method_গুনতি;
	প্রতি (পূর্ণাঙ্ক i = 0; i < func_গুনতি; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    যদি (!IS_ABSTRACT_METHOD(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    শূন্য
defcompile_classes_in_script(শূন্য)
{
    প্রতি (class_T *cl = first_class; cl != রিক্ত; cl = cl->class_next_used)
    {
	যদি (eval_variable(cl->class_name.string, 0, 0, রিক্ত, রিক্ত,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    পূর্ণাঙ্ক
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    যদি (eval_variable(name, 0, 0, rettv, রিক্ত, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	প্রত্যাগম rettv->v_type == VAR_CLASS;
    প্রত্যাগম FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
object_call_builtin_method(
    object_T		*obj,
    class_builtin_T	builtin_method,
    পূর্ণাঙ্ক			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    পূর্ণাঙ্ক	    midx;

    যদি (obj == রিক্ত)
	প্রত্যাগম FAIL;

    uf = class_get_builtin_method(obj->obj_class, builtin_method, &midx);
    যদি (uf == রিক্ত)
	প্রত্যাগম FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    পূর্ণাঙ্ক		r;

    যদি (fc == রিক্ত)
	প্রত্যাগম FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, রিক্ত, obj, fc, rettv);

    remove_funccal();

    প্রত্যাগম r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    পূর্ণাঙ্ক
object_empty(object_T *obj)
{
    typval_T	rettv;

    যদি (object_call_builtin_method(obj, CLASS_BUILTIN_EMPTY, 0, রিক্ত, &rettv)
								== FAIL)
	প্রত্যাগম TRUE;

    প্রত্যাগম tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    পূর্ণাঙ্ক
object_len(object_T *obj)
{
    typval_T	rettv;

    যদি (object_call_builtin_method(obj, CLASS_BUILTIN_LEN, 0, রিক্ত, &rettv)
								== FAIL)
	প্রত্যাগম 0;

    প্রত্যাগম tv_to_সংখ্যা(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    পূর্ণাঙ্ক
object_equal(
	object_T *o1,
	object_T *o2,
	পূর্ণাঙ্ক	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    যদি (o1 == o2)
	প্রত্যাগম TRUE;
    যদি (o1 == রিক্ত || o2 == রিক্ত)
	প্রত্যাগম FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    যদি (cl1 != cl2 || cl1 == রিক্ত || cl2 == রিক্ত)
	প্রত্যাগম FALSE;

    প্রতি (পূর্ণাঙ্ক i = 0; i < cl1->class_obj_member_গুনতি; ++i)
	যদি (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    প্রত্যাগম FALSE;

    প্রত্যাগম TRUE;
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
    পূর্ণাঙ্ক		copyID,
    পূর্ণাঙ্ক		echo_style,
    পূর্ণাঙ্ক		restore_copyID,
    পূর্ণাঙ্ক		composite_val)
{
    typval_T	rettv;

    যদি (object_call_builtin_method(obj, CLASS_BUILTIN_STRING, 0, রিক্ত, &rettv)
								== OK
					&& rettv.vval.v_string != রিক্ত)
	প্রত্যাগম rettv.vval.v_string;

    পূর্ণাঙ্ক		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    যদি (cl != রিক্ত && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    অন্যথা
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	যদি (cl == রিক্ত)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	অন্যথা
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
    }
    যদি (cl != রিক্ত)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	প্রতি (পূর্ণাঙ্ক i = 0; i < cl->class_obj_member_গুনতি; ++i)
	{
	    যদি (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.length);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = রিক্ত;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, composite_val);
	    যদি (s != রিক্ত)
		ga_concat(&ga, s);
	    vim_free(tf);
	    যদি (s == রিক্ত || did_echo_string_emsg)
	    {
		ok = FAIL;
		ভঙ্গ;
	    }
	    সারি_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    যদি (ok == FAIL)
    {
	vim_free(ga.ga_data);
	প্রত্যাগম রিক্ত;
    }
    ga_append(&ga, NUL);
    প্রত্যাগম (char_u *)ga.ga_data;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    পূর্ণাঙ্ক
class_instance_of(class_T *cl, class_T *other_cl)
{
    যদি (cl == other_cl)
	প্রত্যাগম TRUE;

    // Recursively check the base classes.
    প্রতি (; cl != রিক্ত; cl = cl->class_extends)
    {
	যদি (cl == other_cl)
	    প্রত্যাগম TRUE;
	// Check the implemented interfaces and the super interfaces
	প্রতি (পূর্ণাঙ্ক i = cl->class_interface_গুনতি - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    যাবত্ (intf != রিক্ত)
	    {
		যদি (intf == other_cl)
		    প্রত্যাগম TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    প্রত্যাগম FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    শূন্য
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_সংখ্যা = VVAL_FALSE;

    যদি (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_args(argvars, 1) == FAIL)
	প্রত্যাগম;

    যদি (object_tv->vval.v_object == রিক্ত)
    {
	যদি (classinfo_tv->vval.v_class == রিক্ত)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_সংখ্যা = VVAL_TRUE;
	প্রত্যাগম;
    }

    প্রতি (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	যদি (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	অন্যথা
	    c = classinfo_tv->vval.v_class;

	যদি (class_instance_of(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_সংখ্যা = VVAL_TRUE;
	    প্রত্যাগম;
	}
    }
}

#endif // FEAT_EVAL
