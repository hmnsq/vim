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
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

ಸ್ಥಿರ class_T *first_class = ರಿಕ್ತ;
ಸ್ಥಿರ class_T *next_nonref_class = ರಿಕ್ತ;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
class_created(class_T *cl)
{
    ಯದಿ (first_class != ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
class_cleared(class_T *cl)
{
    ಯದಿ (cl->class_next_used != ರಿಕ್ತ)
	cl->class_next_used->class_prev_used = cl->class_prev_used;
    ಯದಿ (cl->class_prev_used != ರಿಕ್ತ)
	cl->class_prev_used->class_next_used = cl->class_next_used;
    ಅನ್ಯಥಾ ಯದಿ (first_class == cl)
	first_class = cl->class_next_used;

    // update the next class to check if needed
    ಯದಿ (cl == next_nonref_class)
	next_nonref_class = cl->class_next_used;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_member(
    exarg_T	*eap,
    char_u	*ಸಾಲು,
    char_u	*varname,
    ಪೂರ್ಣಾಙ್ಕ		has_public,	    // TRUE if "public" seen before "varname"
    char_u	**varname_end,
    ಪೂರ್ಣಾಙ್ಕ		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_ಅಭಿವ್ಯಕ್ತಿ)
{
    *varname_end = to_name_end(varname, FALSE);
    ಯದಿ (*varname == '_' && has_public)
    {
	semsg(_(e_public_variable_name_cannot_start_with_underscore_str), ಸಾಲು);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    char_u *colon = skipwhite(*varname_end);
    char_u *type_arg = colon;
    type_T *type = ರಿಕ್ತ;
    *has_type = FALSE;
    ಯದಿ (*colon == ':')
    {
	ಯದಿ (VIM_ISWHITE(**varname_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, ರಿಕ್ತ, ರಿಕ್ತ, TRUE);
	ಯದಿ (type == ರಿಕ್ತ || !valid_declaration_type(type))
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    ಯದಿ (type == ರಿಕ್ತ && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (init_ಅಭಿವ್ಯಕ್ತಿ == ರಿಕ್ತ && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_in_interface));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *ಅಭಿವ್ಯಕ್ತಿ_start, *ಅಭಿವ್ಯಕ್ತಿ_end;

	ಯದಿ (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ_concatenate(&init_arg, &ಅಭಿವ್ಯಕ್ತಿ_start, &ಅಭಿವ್ಯಕ್ತಿ_end, &evalarg);

	init_arg = skipwhite(init_arg);
	ಯದಿ (*init_arg != NUL && !vim9_comment_start(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, ರಿಕ್ತ);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	ಯದಿ (type == ರಿಕ್ತ)
	    type = &t_any;

	*init_ಅಭಿವ್ಯಕ್ತಿ = vim_strnsave(ಅಭಿವ್ಯಕ್ತಿ_start, ಅಭಿವ್ಯಕ್ತಿ_end - ಅಭಿವ್ಯಕ್ತಿ_start);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ ಯದಿ (!valid_declaration_type(type))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    *type_ret = type;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_end,
    ಪೂರ್ಣಾಙ್ಕ		has_public,
    ಪೂರ್ಣಾಙ್ಕ		has_final,
    ಪೂರ್ಣಾಙ್ಕ		has_const,
    ಪೂರ್ಣಾಙ್ಕ		has_type,
    type_T	*type,
    char_u	*init_ಅಭಿವ್ಯಕ್ತಿ)
{
    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_data) + gap->ga_len;
    m->ocm_name.length = (size_t)(varname_end - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.length);
    ಯದಿ (m->ocm_name.string == ರಿಕ್ತ)
    {
	m->ocm_name.length = 0;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    m->ocm_access = has_public ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_PRIVATE : VIM_ACCESS_READ;
    ಯದಿ (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    ಯದಿ (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    ಯದಿ (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    ಯದಿ (init_ಅಭಿವ್ಯಕ್ತಿ != ರಿಕ್ತ)
    {
	m->ocm_init = init_ಅಭಿವ್ಯಕ್ತಿ;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_members_to_class(
    garray_T	*gap,
    ocmember_T	*parent_members,
    ಪೂರ್ಣಾಙ್ಕ		parent_ಎಣಿಕೆ,
    ocmember_T	**members,
    ಪೂರ್ಣಾಙ್ಕ		*member_ಎಣಿಕೆ)
{
    *member_ಎಣಿಕೆ = parent_ಎಣಿಕೆ + gap->ga_len;
    *members = *member_ಎಣಿಕೆ == 0 ? ರಿಕ್ತ
				       : ALLOC_MULT(ocmember_T, *member_ಎಣಿಕೆ);
    ಯದಿ (*member_ಎಣಿಕೆ > 0 && *members == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < parent_ಎಣಿಕೆ; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = parent_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.length);
	ಯದಿ (m->ocm_name.string == ರಿಕ್ತ)
	{
	    m->ocm_name.length = 0;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (m->ocm_init != ರಿಕ್ತ)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    ಯದಿ (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + parent_ಎಣಿಕೆ,
			       gap->ga_data, ಪರಿಮಾಣ(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_data);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    ಪೂರ್ಣಾಙ್ಕ
object_index_from_itf_index(class_T *itf, ಪೂರ್ಣಾಙ್ಕ is_method, ಪೂರ್ಣಾಙ್ಕ idx, class_T *cl)
{
    ಯದಿ (idx >= (is_method ? itf->class_obj_method_ಎಣಿಕೆ
				   : itf->class_obj_member_ಎಣಿಕೆ))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    ಯದಿ (cl == itf)
	ಪ್ರತ್ಯಾಗಮ idx;

    itf2class_T		*i2c = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			searching = TRUE;
    ಪೂರ್ಣಾಙ್ಕ			method_offset = 0;

    ಪ್ರತಿ (class_T *super = cl; super != ರಿಕ್ತ && searching;
						super = super->class_extends)
    {
	ಪ್ರತಿ (i2c = itf->class_itf2class; i2c != ರಿಕ್ತ; i2c = i2c->i2c_next)
	{
	    ಯದಿ (i2c->i2c_class == super && i2c->i2c_is_method == is_method)
	    {
		searching = FALSE;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (searching && is_method)
	    // The parent class methods are stored after the current class
	    // methods.
	    method_offset += super->class_obj_method_ಎಣಿಕೆ_child;
    }
    ಯದಿ (i2c == ರಿಕ್ತ)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    // A table follows the i2c for the class
    ಪೂರ್ಣಾಙ್ಕ *table = (ಪೂರ್ಣಾಙ್ಕ *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    ಪ್ರತ್ಯಾಗಮ table[idx] + method_offset;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    ಪೂರ್ಣಾಙ್ಕ	    is_class)
{
    typval_T	tv;
    ಪೂರ್ಣಾಙ್ಕ		success = FALSE;

    ಯದಿ (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	ಪ್ರತ್ಯಾಗಮ success;
    }

    tv.v_type = VAR_UNKNOWN;
    ಯದಿ (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_found_str), extends_name);
	ಪ್ರತ್ಯಾಗಮ success;
    }

    ಯದಿ (tv.v_type != VAR_CLASS || tv.vval.v_class == ರಿಕ್ತ
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    ಅನ್ಯಥಾ
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    ಪ್ರತ್ಯಾಗಮ success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
validate_extends_generic_method(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    ವರ್ಣ	*ಸಂದೇಶ = ರಿಕ್ತ;

    ಯದಿ (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಯದಿ (super_fp->uf_generic_argcount == 0)
	ಸಂದೇಶ = e_concrete_method_str_override_with_generic_method_in_class_str;
    ಅನ್ಯಥಾ ಯದಿ (cl_fp->uf_generic_argcount == 0)
	ಸಂದೇಶ = e_generic_method_str_override_with_concrete_method_in_class_str;
    ಅನ್ಯಥಾ
	ಸಂದೇಶ = e_generic_method_str_type_arguments_mismatch_in_class_str;

    semsg(_(ಸಂದೇಶ), cl_fp->uf_name, super_cl->class_name.string);
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    ಪೂರ್ಣಾಙ್ಕ		method_ಎಣಿಕೆ = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_data);

    ಯಾವತ್ (super != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ extends_method_ಎಣಿಕೆ = super->class_obj_method_ಎಣಿಕೆ_child;
	ಯದಿ (extends_method_ಎಣಿಕೆ == 0)
	{
	    super = super->class_extends;
	    ಅನುವರ್ತ;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < extends_method_ಎಣಿಕೆ; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    ಪೂರ್ಣಾಙ್ಕ	    extends_private = (*pstr == '_');
	    ಯದಿ (extends_private)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < method_ಎಣಿಕೆ; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		ಪೂರ್ಣಾಙ್ಕ	priv_method = (*qstr == '_');
		ಯದಿ (priv_method)
		    qstr++;
		ಯದಿ (STRCMP(pstr, qstr) == 0)
		{
		    ಯದಿ (priv_method != extends_private)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_method_str_of_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			ಪ್ರತ್ಯಾಗಮ FALSE;
		    }

		    ಯದಿ (!validate_extends_generic_method(super,
						extends_methods[i], cl_fp[j]))
			    ಪ್ರತ್ಯಾಗಮ FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಙ್ಕ member_ಎಣಿಕೆ = objmembers_gap->ga_len;
    ಯದಿ (member_ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ c_i = 0; c_i < member_ಎಣಿಕೆ; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	ಯಾವತ್ (p_cl != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ p_member_ಎಣಿಕೆ = p_cl->class_obj_member_ಎಣಿಕೆ;

	    ಯದಿ (p_member_ಎಣಿಕೆ == 0)
	    {
		p_cl = p_cl->class_extends;
		ಅನುವರ್ತ;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ p_i = 0; p_i < p_member_ಎಣಿಕೆ; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		ಯದಿ (STRCMP(pstr, qstr) == 0)
		{
		    ಯದಿ (c_protected != p_protected)
			semsg(_(e_public_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    ಅನ್ಯಥಾ
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಙ್ಕ member_ಎಣಿಕೆ = objmembers_gap->ga_len;
    ಯದಿ (member_ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ c_i = 0; c_i < member_ಎಣಿಕೆ; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	ಪೂರ್ಣಾಙ್ಕ	    var_found = FALSE;

	// Check in all the parent classes in the lineage
	ಯಾವತ್ (p_cl != ರಿಕ್ತ && !var_found)
	{
	    ಪೂರ್ಣಾಙ್ಕ p_member_ಎಣಿಕೆ = p_cl->class_obj_member_ಎಣಿಕೆ;
	    ಯದಿ (p_member_ಎಣಿಕೆ == 0)
	    {
		p_cl = p_cl->class_extends;
		ಅನುವರ್ತ;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ p_i = 0; p_i < p_member_ಎಣಿಕೆ; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		ಯದಿ (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    ಅನುವರ್ತ;

		// Ensure the type is matching.
		where.wt_func_name = (ವರ್ಣ *)c_m->ocm_name.string;
		where.wt_kind = WT_MEMBER;

		ಯದಿ (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FALSE;

		var_found = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	ಪೂರ್ಣಾಙ್ಕ extends_method_ಎಣಿಕೆ = loop == 1
				? extends_cl->class_class_function_ಎಣಿಕೆ
				: extends_cl->class_obj_method_ಎಣಿಕೆ;
	ಯದಿ (extends_method_ಎಣಿಕೆ == 0)
	    ಅನುವರ್ತ;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	ಪೂರ್ಣಾಙ್ಕ method_ಎಣಿಕೆ = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_data
						: objmethods_gap->ga_data);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < extends_method_ಎಣಿಕೆ; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    ಯದಿ (!IS_ABSTRACT_METHOD(uf))
		ಅನುವರ್ತ;

	    ಪೂರ್ಣಾಙ್ಕ	concrete_method_found = FALSE;
	    ಪೂರ್ಣಾಙ್ಕ	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    ಪ್ರತಿ (j = 0; !concrete_method_found && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		ಯದಿ (!IS_ABSTRACT_METHOD(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_method_found = TRUE;
	    }

	    ಯದಿ (concrete_method_found)
		ಅನುವರ್ತ;

	    ಪ್ರತಿ (j = 0; j < method_ಎಣಿಕೆ; j++)
	    {
		ಯದಿ (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_method_found = TRUE;
		    ಭಙ್ಗ;
		}
	    }

	    ಯದಿ (!concrete_method_found)
	    {
		semsg(_(e_abstract_method_str_not_implemented), uf->uf_name);
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    ಪೂರ್ಣಾಙ್ಕ		cl_member_ಎಣಿಕೆ,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಙ್ಕ		variable_present  = FALSE;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ cl_i = 0; cl_i < cl_member_ಎಣಿಕೆ; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	ಯದಿ (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    ಅನುವರ್ತ;

	// Ensure the access type is same
	ಯದಿ (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_of_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	// Ensure the type is matching.
	ಯದಿ (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	ಅನ್ಯಥಾ
	{
	    where.wt_func_name = (ವರ್ಣ *)m->ocm_name.string;
	    where.wt_kind = WT_MEMBER;
	    ಯದಿ (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	variable_present = TRUE;
	ಭಙ್ಗ;
    }

    ಯದಿ (!variable_present && extends_cl != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ ext_cl_ಎಣಿಕೆ = extends_cl->class_obj_member_ಎಣಿಕೆ;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	ಪ್ರತ್ಯಾಗಮ intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_ಎಣಿಕೆ,
					extends_cl->class_extends);
    }

    ಪ್ರತ್ಯಾಗಮ variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಙ್ಕ if_ಎಣಿಕೆ = ifcl->class_obj_member_ಎಣಿಕೆ;
    ಯದಿ (if_ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_data);
    ಪೂರ್ಣಾಙ್ಕ cl_ಎಣಿಕೆ = objmembers_gap->ga_len;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ if_i = 0; if_i < if_ಎಣಿಕೆ; ++if_i)
    {
	ಯದಿ (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_ಎಣಿಕೆ, extends_cl))
	{
	    semsg(_(e_variable_str_of_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
intf_method_type_matches(
    ufunc_T	*if_method,
    ufunc_T	*cl_method,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    ಯದಿ (cl_method->uf_generic_argcount != if_method->uf_generic_argcount)
    {
	semsg(_(e_type_var_ಎಣಿಕೆ_of_method_str_differs_from_interface_str),
						cl_method->uf_name, intf_name);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (ವರ್ಣ *)if_method->uf_name;
    where.wt_kind = WT_METHOD;
    ಯದಿ (check_type(if_method->uf_func_type, cl_method->uf_func_type, TRUE,
								where) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
intf_method_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    ಪೂರ್ಣಾಙ್ಕ	    cl_ಎಣಿಕೆ,
    class_T *extends_cl,
    char_u  *intf_name)
{
    ಪೂರ್ಣಾಙ್ಕ		method_present  = FALSE;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ cl_i = 0; cl_i < cl_ಎಣಿಕೆ; ++cl_i)
    {
	ಯದಿ (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    ಯದಿ (!intf_method_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    method_present = TRUE;
	    ಭಙ್ಗ;
	}
    }

    ಯದಿ (!method_present && extends_cl != ರಿಕ್ತ)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	ಪೂರ್ಣಾಙ್ಕ	ext_cl_ಎಣಿಕೆ = extends_cl->class_obj_method_ಎಣಿಕೆ;
	ಪ್ರತ್ಯಾಗಮ intf_method_present(if_ufunc, ext_cl_fp, ext_cl_ಎಣಿಕೆ,
					extends_cl->class_extends, intf_name);
    }

    ಪ್ರತ್ಯಾಗಮ method_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಙ್ಕ if_ಎಣಿಕೆ = ifcl->class_obj_method_ಎಣಿಕೆ;
    ಯದಿ (if_ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_data);
    ಪೂರ್ಣಾಙ್ಕ cl_ಎಣಿಕೆ = objmethods_gap->ga_len;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ if_i = 0; if_i < if_ಎಣಿಕೆ; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	ಯದಿ (!intf_method_present(if_fp[if_i], cl_fp, cl_ಎಣಿಕೆ, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_method_str_of_interface_str_not_implemented),
		    if_name, intf_class_name);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಙ್ಕ		success = TRUE;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_data)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	ಯದಿ (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_found_str), impl);
	    success = FALSE;
	    ಭಙ್ಗ;
	}

	ಯದಿ (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == ರಿಕ್ತ
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    ಭಙ್ಗ;
	}

	class_T *ifcl = tv.vval.v_class;
	ಯದಿ (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    ಭಙ್ಗ;
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

    ಯದಿ (!success)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_data;
    cl->class_interface_ಎಣಿಕೆ = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_data)[i],
		((class_T **)intf_classes_gap->ga_data)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = ರಿಕ್ತ;
    cl->class_interface_ಎಣಿಕೆ = 0;
    cl->class_extends = ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < intf_classes_gap->ga_len; j++)
    {
	ಯದಿ (((class_T **)intf_classes_gap->ga_data)[j] == ifcl)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.length);
    ಯದಿ (intf_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (ga_grow(impl_gap, 1) == FAIL)
	ಗಚ್ಛ fail;

    char_u **intf_names = (char_u **)impl_gap->ga_data;
    intf_names[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    ಯದಿ (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	ಗಚ್ಛ fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_data;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    ಪ್ರತ್ಯಾಗಮ TRUE;
fail:
    vim_free(intf_name);
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < super->class_interface_ಎಣಿಕೆ; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	ಯದಿ (!is_interface_class_present(intf_classes_gap, ifcl))
	    add_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_func_arg_names(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_data)[fi];

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < uf->uf_args.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_args.ga_data)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_data + mi)->ocm_name.string;
		    ಯದಿ (STRCMP(aname, mname) == 0)
		    {
			ಯದಿ (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_argument_already_declared_in_class_str),
				aname);

			ಪ್ರತ್ಯಾಗಮ FALSE;
		    }
		}
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_reserved_varname(char_u *varname, char_u *varname_end)
{
    ಪೂರ್ಣಾಙ್ಕ reserved = FALSE;
    char_u save_varname_end = *varname_end;
    *varname_end = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_end = save_varname_end;

    ಪ್ರತ್ಯಾಗಮ reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_end)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_end - varname)};
    ಪೂರ್ಣಾಙ್ಕ		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    ಯದಿ (*pstr.string == '_')
    {
	pstr.string++;
	pstr.length--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_data) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.length};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    ಯದಿ (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.length--;
		q_protected = true;
	    }

	    ಯದಿ (pstr.length == qstr.length
		&& STRNCMP(pstr.string, qstr.string, pstr.length) == 0)
	    {
		char_u	save_c = *varname_end;

		*varname_end = NUL;
		ಯದಿ (p_protected != q_protected)
		    semsg(_(e_public_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		ಅನ್ಯಥಾ
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_end = save_c;
		dup = TRUE;
		ಭಙ್ಗ;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_duplicate_method(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_data)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    ಯದಿ (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_valid_constructor(ufunc_T *uf, ಪೂರ್ಣಾಙ್ಕ is_abstract, ಪೂರ್ಣಾಙ್ಕ has_static)
{
    // Constructors are not allowed in abstract classes.
    ಯದಿ (is_abstract)
    {
	emsg(_(e_cannot_define_new_method_in_abstract_class));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    // A constructor is always static, no need to define it so.
    ಯದಿ (has_static)
    {
	emsg(_(e_cannot_define_new_method_as_static));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    ಯದಿ (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_method));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
object_check_builtin_method_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    ಪೂರ್ಣಾಙ್ಕ	    valid = FALSE;
    type_T  method_sig;
    type_T  method_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_FIELD(method_sig);
    CLEAR_FIELD(method_rt);
    method_sig.tt_type = VAR_FUNC;

    ಯದಿ (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	method_rt.tt_type = VAR_ಸಂಖ್ಯೆ;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	method_rt.tt_type = VAR_BOOL;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	method_rt.tt_type = VAR_STRING;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    ಅನ್ಯಥಾ
	semsg(_(e_builtin_object_method_str_not_supported), uf->uf_name);

    where.wt_func_name = (ವರ್ಣ *)uf->uf_name;
    where.wt_kind = WT_METHOD;
    ಯದಿ (valid && !check_type(&method_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    ಪ್ರತ್ಯಾಗಮ valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    ಪೂರ್ಣಾಙ್ಕ
is_valid_builtin_obj_methodname(char_u *funcname)
{
    ವಿಕಲ್ಪ (funcname[0])
    {
	ಪಕ್ಷ 'e':
	    ಪ್ರತ್ಯಾಗಮ STRNCMP(funcname, "empty", 5) == 0;

	ಪಕ್ಷ 'l':
	    ಪ್ರತ್ಯಾಗಮ STRNCMP(funcname, "len", 3) == 0;

	ಪಕ್ಷ 'n':
	    ಪ್ರತ್ಯಾಗಮ STRNCMP(funcname, "new", 3) == 0;

	ಪಕ್ಷ 's':
	    ಪ್ರತ್ಯಾಗಮ STRNCMP(funcname, "string", 6) == 0;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_method(
    class_T		*cl,
    class_builtin_T	builtin_method,
    ಪೂರ್ಣಾಙ್ಕ			*method_idx)
{
    *method_idx = -1;

    ಯದಿ (cl == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    *method_idx = cl->class_builtin_methods[builtin_method];
    ಪ್ರತ್ಯಾಗಮ *method_idx != -1 ? cl->class_obj_methods[*method_idx] : ರಿಕ್ತ;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
update_member_method_lookup_table(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    ಪೂರ್ಣಾಙ್ಕ		pobj_method_offset)
{
    ಯದಿ (ifcl == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(ಪರಿಮಾಣ(itf2class_T)
				+ ifcl->class_obj_member_ಎಣಿಕೆ * ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
    ಯದಿ (if2cl == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = FALSE;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ if_i = 0; if_i < ifcl->class_obj_member_ಎಣಿಕೆ; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ cl_i = 0; cl_i < cl->class_obj_member_ಎಣಿಕೆ; ++cl_i)
	{
	    ಯದಿ (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ *table = (ಪೂರ್ಣಾಙ್ಕ *)(if2cl + 1);
		table[if_i] = cl_i;
		ಭಙ್ಗ;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(ಪರಿಮಾಣ(itf2class_T)
				+ ifcl->class_obj_method_ಎಣಿಕೆ * ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
    ಯದಿ (if2cl == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = TRUE;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ if_i = 0; if_i < ifcl->class_obj_method_ಎಣಿಕೆ; ++if_i)
    {
	ಪೂರ್ಣಾಙ್ಕ	done = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    ಯದಿ (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_data)[cl_i]->uf_name) == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ *table = (ಪೂರ್ಣಾಙ್ಕ *)(if2cl + 1);
		table[if_i] = cl_i;
		done = TRUE;
		ಭಙ್ಗ;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	ಯದಿ (!done)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    ಯದಿ (cl->class_extends != ifcl)
	    {
		class_T		*parent = cl->class_extends;
		ಪೂರ್ಣಾಙ್ಕ		method_offset = objmethods->ga_len;

		ಯಾವತ್ (!done && parent != ರಿಕ್ತ && parent != ifcl)
		{
		    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ cl_i = 0;
			    cl_i < parent->class_obj_method_ಎಣಿಕೆ_child; ++cl_i)
		    {
			ಯದಿ (STRCMP(if_name,
				    parent->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    ಪೂರ್ಣಾಙ್ಕ *table = (ಪೂರ್ಣಾಙ್ಕ *)(if2cl + 1);
			    table[if_i] = method_offset + cl_i;
			    done = TRUE;
			    ಭಙ್ಗ;
			}
		    }
		    method_offset += parent->class_obj_method_ಎಣಿಕೆ_child;
		    parent = parent->class_extends;
		}
	    }

	    ಯದಿ (!done)
	    {
		ಪೂರ್ಣಾಙ್ಕ *table = (ಪೂರ್ಣಾಙ್ಕ *)(if2cl + 1);
		table[if_i] = pobj_method_offset + if_i;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_interface_ಎಣಿಕೆ; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	ಯಾವತ್ (ifcl != ರಿಕ್ತ)
	{
	    ಯದಿ (update_member_method_lookup_table(ifcl, cl, objmethods_gap,
								0) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    ಯದಿ (extends_cl != ರಿಕ್ತ)
    {
	class_T		*pclass = extends_cl;
	ಪೂರ್ಣಾಙ್ಕ		pobj_method_offset = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	ಯಾವತ್ (pclass != ರಿಕ್ತ)
	{
	    ಯದಿ (update_member_method_lookup_table(pclass, cl,
			objmethods_gap, pobj_method_offset) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    pobj_method_offset += pclass->class_obj_method_ಎಣಿಕೆ_child;
	    pclass = pclass->class_extends;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_ಎಣಿಕೆ);
    ಯದಿ (cl->class_members_tv == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_class_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	ಯದಿ (m->ocm_init != ರಿಕ್ತ)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_ಅಭಿವ್ಯಕ್ತಿ(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    ಯದಿ (etv == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    ಯದಿ (m->ocm_type->tt_type == VAR_ANY
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
	ಅನ್ಯಥಾ
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = ರಿಕ್ತ;
	}
	set_tv_type(tv, m->ocm_type);
	ಯದಿ (m->ocm_flags & OCMFLAG_CONST)
	    item_lock(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    ಪೂರ್ಣಾಙ್ಕ		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_obj_member_ಎಣಿಕೆ; ++i)
    {
	ಯದಿ (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    ಅನುವರ್ತ;

	ಯದಿ (i > (is_enum ? 2 : 0))
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

    garray_T ಸಾಲುಗಳು_to_free;
    ga_init2(&ಸಾಲುಗಳು_to_free, ಪರಿಮಾಣ(char_u *), 50);

    ufunc_T *nf = define_function(&fea, ರಿಕ್ತ, &ಸಾಲುಗಳು_to_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_ಎಣಿಕೆ,
			    ರಿಕ್ತ);

    ga_clear_strings(&ಸಾಲುಗಳು_to_free);
    vim_free(fga.ga_data);

    ಯದಿ (nf != ರಿಕ್ತ && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_data)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	ಯದಿ (nf->uf_ret_type != ರಿಕ್ತ)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_args = ರಿಕ್ತ;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	ಪೂರ್ಣಾಙ್ಕ		*fcount = loop == 1 ? &cl->class_class_function_ಎಣಿಕೆ
						: &cl->class_obj_method_ಎಣಿಕೆ;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	ಪೂರ್ಣಾಙ್ಕ parent_ಎಣಿಕೆ = 0;
	ಯದಿ (extends_cl != ರಿಕ್ತ)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    parent_ಎಣಿಕೆ = loop == 1
				? 0
				: extends_cl->class_obj_method_ಎಣಿಕೆ;

	*fcount = parent_ಎಣಿಕೆ + gap->ga_len;
	ಯದಿ (*fcount == 0)
	{
	    *fup = ರಿಕ್ತ;
	    ಅನುವರ್ತ;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	ಯದಿ (*fup == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_data, ಪರಿಮಾಣ(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_data);
	ಯದಿ (loop == 1)
	    cl->class_class_function_ಎಣಿಕೆ_child = gap->ga_len;
	ಅನ್ಯಥಾ
	    cl->class_obj_method_ಎಣಿಕೆ_child = gap->ga_len;

	ಯದಿ (loop == 2)
	{
	    // Copy instance methods from the parent.

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < parent_ಎಣಿಕೆ; ++i)
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
		ಯದಿ (nf != ರಿಕ್ತ && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_args != ರಿಕ್ತ)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    ಯದಿ (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (ವರ್ಣ *)pname;
			where.wt_kind = WT_METHOD;
			(ಶೂನ್ಯ)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    ಯದಿ (i < gap->ga_len)
		fp->uf_defclass = cl;
	    ಯದಿ (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_builtin_method_index(class_T *cl)
{
    ಪೂರ್ಣಾಙ್ಕ	i;

    ಪ್ರತಿ (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    ಪ್ರತಿ (i = 0; i < cl->class_obj_method_ಎಣಿಕೆ; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	ಯದಿ (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	ಅನ್ಯಥಾ ಯದಿ (cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] = i;
	ಅನ್ಯಥಾ ಯದಿ (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    ಸ್ಥಿರ char_u *
find_class_name_end(char_u *arg)
{
    char_u *end = arg;

    ಯಾವತ್ (ASCII_ISALNUM(*end) || *end == '_'
	    || (*end == '.' && (ASCII_ISALNUM(end[1]) || end[1] == '_')))
	++end;

    ಪ್ರತ್ಯಾಗಮ end;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
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
    ಪೂರ್ಣಾಙ್ಕ		dup = FALSE;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_data) + i;

	ಯದಿ (name.length == m->ocm_name.length
	    && STRNCMP(name.string, m->ocm_name.string, name.length) == 0)
	{
	    char_u  save_c = *varname_end;

	    *varname_end = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_end = save_c;
	    dup = TRUE;
	    ಭಙ್ಗ;
	}
    }

    ಪ್ರತ್ಯಾಗಮ dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
enum_parse_ಮೌಲ್ಯಗಳು(
    exarg_T	*eap,
    class_T	*en,
    char_u	*ಸಾಲು,
    garray_T	*gap,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು,
    ಪೂರ್ಣಾಙ್ಕ		*enum_end)
{
    evalarg_T	evalarg;
    char_u	*p = ಸಾಲು;
    ವರ್ಣ	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	last_char = NUL;
    ಪೂರ್ಣಾಙ್ಕ		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;
    ಯಾವತ್ (*p != NUL)
    {
	// ignore comment
	ಯದಿ (*p == '#')
	    ಭಙ್ಗ;

	ಯದಿ (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_ಮೌಲ್ಯ_declaration_str), p);
	    ಭಙ್ಗ;
	}

	char_u *eni_name_start = p;
	char_u *eni_name_end = to_name_end(p, FALSE);

	ಯದಿ (is_duplicate_enum(gap, eni_name_start, eni_name_end))
	    ಭಙ್ಗ;

	p = skipwhite(eni_name_end);

	char_u	*init_ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
	ಯದಿ (*p == '(' || *p == '<')
	{
	    ಯದಿ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", ಸಾಲು);
		ಭಙ್ಗ;
	    }

	    char_u *ಅಭಿವ್ಯಕ್ತಿ_start, *ಅಭಿವ್ಯಕ್ತಿ_end;

	    p = eni_name_start;
	    (ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ_concatenate(&p, &ಅಭಿವ್ಯಕ್ತಿ_start, &ಅಭಿವ್ಯಕ್ತಿ_end, &evalarg);

	    ಯಾವತ್ (*ಅಭಿವ್ಯಕ್ತಿ_start && *ಅಭಿವ್ಯಕ್ತಿ_start != '(' && *ಅಭಿವ್ಯಕ್ತಿ_start != '<')
		ಅಭಿವ್ಯಕ್ತಿ_start++;

	    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_end > ಅಭಿವ್ಯಕ್ತಿ_start)
		init_ಅಭಿವ್ಯಕ್ತಿ = vim_strnsave(ಅಭಿವ್ಯಕ್ತಿ_start, ಅಭಿವ್ಯಕ್ತಿ_end - ಅಭಿವ್ಯಕ್ತಿ_start);
	}

	ಯದಿ (init_ಅಭಿವ್ಯಕ್ತಿ == ರಿಕ್ತ)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, ಪರಿಮಾಣ(initexpr_buf), "%s.new()", en->class_name.string);
	ಅನ್ಯಥಾ
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, ಪರಿಮಾಣ(initexpr_buf), "%s.new%s", en->class_name.string,
		init_ಅಭಿವ್ಯಕ್ತಿ);
	    vim_free(init_ಅಭಿವ್ಯಕ್ತಿ);
	}
	ಯದಿ (add_member(gap, eni_name_start, eni_name_end, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    ಭಙ್ಗ;

	++*ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು;

	ಯದಿ (*p != '#')
	    last_char = *p;

	ಯದಿ (*p != NUL && *p != ',')
	    ಭಙ್ಗ;

	ಯದಿ (*p == ',')
	{
	    ಯದಿ (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", ಸಾಲು);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", ಸಾಲು);
		ಭಙ್ಗ;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    ಯದಿ (*p != NUL && *p != '#')
    {
	ಯದಿ (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_argument_str), p);
	rc = FAIL;
    }

    ಯದಿ (last_char != ',')
	// last enum value should not be terminated by ","
	*enum_end = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
enum_add_ಮೌಲ್ಯಗಳು_member(
    class_T	*en,
    garray_T	*gap,
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    ಪೂರ್ಣಾಙ್ಕ		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;

	ಯದಿ (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.length);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    ಯದಿ (type == ರಿಕ್ತ)
	ಗಚ್ಛ done;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    ಯದಿ (type->tt_member != ರಿಕ್ತ)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = add_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_data, fga.ga_len));

done:
    vim_free(fga.ga_data);

    ಪ್ರತ್ಯಾಗಮ rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
enum_clear_constructors(class_T *en)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < en->class_class_function_ಎಣಿಕೆ; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	ಯದಿ (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    ಶೂನ್ಯ
enum_set_internal_obj_vars(class_T *en, object_T *enval)
{
    ಪೂರ್ಣಾಙ್ಕ	i;

    ಪ್ರತಿ (i = 0; i < en->class_class_member_ಎಣಿಕೆ; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	ಯದಿ (en_tv != ರಿಕ್ತ && en_tv->v_type == VAR_UNKNOWN)
	    ಭಙ್ಗ;
    }

    ಯದಿ (i >= en->class_class_member_ಎಣಿಕೆ)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	ಪ್ರತ್ಯಾಗಮ;

    // First object variable is the name
    ocmember_T *ಮೌಲ್ಯ_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(ಮೌಲ್ಯ_ocm->ocm_name.string, ಮೌಲ್ಯ_ocm->ocm_name.length);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_ಸಂಖ್ಯೆ;
    ord_tv->vval.v_ಸಂಖ್ಯೆ = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
skip_class_body(exarg_T *eap)
{
    ಪ್ರತಿ (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	ಯದಿ (theline == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	char_u	*p = theline;
	ಪೂರ್ಣಾಙ್ಕ	found;

	// skip ':' and blanks
	ಪ್ರತಿ (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	found = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	ಯದಿ (found)
	    ಭಙ್ಗ;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    ಶೂನ್ಯ
ex_class(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		is_class = eap->cmdidx == CMD_class;
    ಪೂರ್ಣಾಙ್ಕ		is_abstract = eap->cmdidx == CMD_abstract;
    ಪೂರ್ಣಾಙ್ಕ		is_enum = eap->cmdidx == CMD_enum;
    ಪೂರ್ಣಾಙ್ಕ		added_enum_ಮೌಲ್ಯಗಳು = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		is_interface;
    long	start_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    ಯದಿ (eap->skip)
    {
	skip_class_body(eap);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (is_abstract)
    {
	ಯದಿ (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_argument_str), arg);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    ಯದಿ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	ಯದಿ (is_class)
	    emsg(_(e_class_can_only_be_defined_in_vim9_script));
	ಅನ್ಯಥಾ ಯದಿ (is_enum)
	    emsg(_(e_enum_can_only_be_defined_in_vim9_script));
	ಅನ್ಯಥಾ
	    emsg(_(e_interface_can_only_be_defined_in_vim9_script));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!ASCII_ISUPPER(*arg))
    {
	ಯದಿ (is_class)
	    semsg(_(e_class_name_must_start_with_uppercase_letter_str), arg);
	ಅನ್ಯಥಾ ಯದಿ (is_enum)
	    semsg(_(e_enum_name_must_start_with_uppercase_letter_str), arg);
	ಅನ್ಯಥಾ
	    semsg(_(e_interface_name_must_start_with_uppercase_letter_str),
									  arg);
	ಪ್ರತ್ಯಾಗಮ;
    }
    char_u *name_end = find_name_end(arg, ರಿಕ್ತ, ರಿಕ್ತ, FNE_CHECK_START);
    ಯದಿ (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }
    char_u *name_start = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = ರಿಕ್ತ;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, ಪರಿಮಾಣ(char_u *), 5);

    arg = skipwhite(name_end);
    ಯಾವತ್ (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	ಯದಿ (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    ಯದಿ (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		ಗಚ್ಛ early_ret;
	    }
	    ಯದಿ (extends != ರಿಕ್ತ)
	    {
		emsg(_(e_duplicate_extends));
		ಗಚ್ಛ early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *end = find_class_name_end(arg);
	    ಯದಿ (!IS_WHITE_OR_NUL(*end))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		ಗಚ್ಛ early_ret;
	    }
	    extends = vim_strnsave(arg, end - arg);
	    ಯದಿ (extends == ರಿಕ್ತ)
		ಗಚ್ಛ early_ret;

	    arg = skipwhite(end + 1);
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    ಯದಿ (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		ಗಚ್ಛ early_ret;
	    }

	    ಯದಿ (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		ಗಚ್ಛ early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    ಪ್ರತಿ (;;)
	    {
		char_u *impl_end = find_class_name_end(arg);
		ಯದಿ ((!IS_WHITE_OR_NUL(*impl_end) && *impl_end != ',')
			|| (*impl_end == ','
			    && !IS_WHITE_OR_NUL(*(impl_end + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    ಗಚ್ಛ early_ret;
		}
		ಯದಿ (impl_end - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    ಗಚ್ಛ early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_end - arg);
		ಯದಿ (iname == ರಿಕ್ತ)
		    ಗಚ್ಛ early_ret;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ga_impl.ga_len; ++i)
		    ಯದಿ (STRCMP(((char_u **)ga_impl.ga_data)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			ಗಚ್ಛ early_ret;
		    }
		ಯದಿ (ga_add_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    ಗಚ್ಛ early_ret;
		}
		ಯದಿ (*impl_end != ',')
		{
		    arg = skipwhite(impl_end);
		    ಭಙ್ಗ;
		}
		arg = skipwhite(impl_end + 1);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_strings(&ga_impl);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, ಪರಿಮಾಣ(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, ಪರಿಮಾಣ(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, ಪರಿಮಾಣ(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, ಪರಿಮಾಣ(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, ಪರಿಮಾಣ(ufunc_T *), 10);

    class_T *cl = ರಿಕ್ತ;
    class_T *extends_cl = ರಿಕ್ತ;  // class from "extends" argument
    class_T **intf_classes = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ	    intf_ಎಣಿಕೆ = 0;	// number of items in "intf_classes"
    ಪೂರ್ಣಾಙ್ಕ	    ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    ಯದಿ (cl == ರಿಕ್ತ)
	ಗಚ್ಛ cleanup;

    ಯದಿ (is_enum)
	cl->class_flags = CLASS_ENUM;
    ಅನ್ಯಥಾ ಯದಿ (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    ಅನ್ಯಥಾ ಯದಿ (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.length = (size_t)(name_end - name_start);
    cl->class_name.string = vim_strnsave(name_start, cl->class_name.length);
    ಯದಿ (cl->class_name.string == ರಿಕ್ತ)
	ಗಚ್ಛ cleanup;

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
    ಪೂರ್ಣಾಙ್ಕ rc = set_var_const(cl->class_name.string, 0, ರಿಕ್ತ, &tv, FALSE, 0, 0);
    ಯದಿ (rc == FAIL)
	ಗಚ್ಛ cleanup;

    ಯದಿ (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	ಯದಿ (add_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, ರಿಕ್ತ) == FAIL)
	    ಗಚ್ಛ cleanup;

	varname = (char_u *)"ordinal";
	ಯದಿ (add_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_ಸಂಖ್ಯೆ, ರಿಕ್ತ) == FAIL)
	    ಗಚ್ಛ cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    ಪೂರ್ಣಾಙ್ಕ		enum_end = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ success = FALSE;
    ಪ್ರತಿ (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	ಯದಿ (theline == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	char_u *ಸಾಲು = skipwhite(theline);

	// Skip empty and comment lines.
	ಯದಿ (*ಸಾಲು == NUL)
	    ಅನುವರ್ತ;
	ಯದಿ (*ಸಾಲು == '#')
	{
	    ಯದಿ (vim9_bad_comment(ಸಾಲು))
		ಭಙ್ಗ;
	    ಅನುವರ್ತ;
	}

	char_u *p = ಸಾಲು;

	ವರ್ಣ	*end_name;
	ಪೂರ್ಣಾಙ್ಕ	shortlen;
	ಪೂರ್ಣಾಙ್ಕ	fullen;
	ಯದಿ (is_class)
	{
	    end_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	ಅನ್ಯಥಾ ಯದಿ (is_enum)
	{
	    end_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	ಅನ್ಯಥಾ
	{
	    end_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	ಪ್ರತಿ (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_start = p;
	ಯದಿ (checkforcmd(&p, end_name, shortlen))
	{
	    ಯದಿ (STRNCMP(cmd_start, end_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_start);
	    ಅನ್ಯಥಾ ಯದಿ (*p == '|' || !ends_excmd2(ಸಾಲು, p))
		semsg(_(e_trailing_characters_str), p);
	    ಅನ್ಯಥಾ
		success = TRUE;
	    ಭಙ್ಗ;
	}

	ಪೂರ್ಣಾಙ್ಕ	wrong_endname = FALSE;
	ಯದಿ (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	ಅನ್ಯಥಾ ಯದಿ (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	ಅನ್ಯಥಾ
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	ಯದಿ (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), ಸಾಲು, end_name);
	    ಭಙ್ಗ;
	}

	ಯದಿ (is_enum && !enum_end)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    ಯದಿ (enum_parse_ಮೌಲ್ಯಗಳು(eap, cl, ಸಾಲು, &classmembers,
					&ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು, &enum_end) == FAIL)
		ಭಙ್ಗ;

	    ಯದಿ (enum_end)
	    {
		// Add the enum "values" class variable.
		enum_add_ಮೌಲ್ಯಗಳು_member(cl, &classmembers, ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು,
							&type_list);
		added_enum_ಮೌಲ್ಯಗಳು = TRUE;
	    }
	    ಅನುವರ್ತ;
	}

	ಪೂರ್ಣಾಙ್ಕ has_public = FALSE;
	ಯದಿ (checkforcmd(&p, "public", 3))
	{
	    ಯದಿ (STRNCMP(ಸಾಲು, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ಸಾಲು);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (is_interface)
	    {
		emsg(_(e_public_variable_not_supported_in_interface));
		ಭಙ್ಗ;
	    }
	    has_public = TRUE;
	    p = skipwhite(ಸಾಲು + 6);

	    ಯದಿ (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_public_keyword_not_supported_for_method));
		ಭಙ್ಗ;
	    }

	    ಯದಿ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_public_must_be_followed_by_var_static_final_or_const));
		ಭಙ್ಗ;
	    }
	}

	ಪೂರ್ಣಾಙ್ಕ abstract_method = FALSE;
	char_u *pa = p;
	ಯದಿ (checkforcmd(&p, "abstract", 3))
	{
	    ಯದಿ (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		ಭಙ್ಗ;
	    }

	    ಯದಿ (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_used_in_enum));
		ಭಙ್ಗ;
	    }

	    ಯದಿ (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_used_in_interface));
		ಭಙ್ಗ;
	    }

	    ಯದಿ (!is_abstract)
	    {
		semsg(_(e_abstract_method_in_concrete_class), pa);
		ಭಙ್ಗ;
	    }

	    p = skipwhite(pa + 8);
	    ಯದಿ (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		ಭಙ್ಗ;
	    }

	    abstract_method = TRUE;
	}

	ಪೂರ್ಣಾಙ್ಕ has_static = FALSE;
	char_u *ps = p;
	ಯದಿ (checkforcmd(&p, "static", 4))
	{
	    ಯದಿ (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		ಭಙ್ಗ;
	    }

	    ಯದಿ (is_interface)
	    {
		emsg(_(e_static_member_not_supported_in_interface));
		ಭಙ್ಗ;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    ಯದಿ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		ಭಙ್ಗ;
	    }
	}

	ಪೂರ್ಣಾಙ್ಕ has_final = FALSE;
	ಪೂರ್ಣಾಙ್ಕ has_var = FALSE;
	ಪೂರ್ಣಾಙ್ಕ has_const = FALSE;
	ಯದಿ (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (checkforcmd(&p, "final", 5))
	{
	    ಯದಿ (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_in_interface));
		ಭಙ್ಗ;
	    }
	    has_final = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (checkforcmd(&p, "const", 5))
	{
	    ಯದಿ (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_in_interface));
		ಭಙ್ಗ;
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
	ಯದಿ (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_end = ರಿಕ್ತ;
	    type_T *type = ರಿಕ್ತ;
	    char_u *init_ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
	    ಪೂರ್ಣಾಙ್ಕ	    has_type = FALSE;

	    ಯದಿ (!eval_isnamec1(*p))
	    {
		ಯದಿ (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), ಸಾಲು);
		ಅನ್ಯಥಾ
		    semsg(_(e_invalid_object_variable_declaration_str), ಸಾಲು);
		ಭಙ್ಗ;
	    }

	    ಯದಿ (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_in_interface),
			varname);
		ಭಙ್ಗ;
	    }

	    ಯದಿ (parse_member(eap, ಸಾಲು, varname, has_public,
			  &varname_end, &has_type, &type_list, &type,
			  !is_interface ? &init_ಅಭಿವ್ಯಕ್ತಿ: ರಿಕ್ತ) == FAIL)
		ಭಙ್ಗ;

	    ಯದಿ (is_reserved_varname(varname, varname_end)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_end))
	    {
		vim_free(init_ಅಭಿವ್ಯಕ್ತಿ);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (add_member(has_static ? &classmembers : &objmembers, varname,
				varname_end, has_public, has_final, has_const,
				has_type, type, init_ಅಭಿವ್ಯಕ್ತಿ) == FAIL)
	    {
		vim_free(init_ಅಭಿವ್ಯಕ್ತಿ);
		ಭಙ್ಗ;
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
	ಅನ್ಯಥಾ ಯದಿ (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	ಸಾಲುಗಳು_to_free;
	    ಪೂರ್ಣಾಙ್ಕ		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    ಯದಿ (has_public)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_public_keyword_not_supported_for_method));
		ಭಙ್ಗ;
	    }

	    ಯದಿ (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_in_class_str), ಸಾಲು);
		ಭಙ್ಗ;
	    }

	    ಯದಿ (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_method_not_supported_in_interface), p);
		ಭಙ್ಗ;
	    }

	    ಯದಿ (has_static && !is_new && SAFE_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_method_not_supported), p);
		ಭಙ್ಗ;
	    }

	    CLEAR_FIELD(ea);
	    ea.cmd = ಸಾಲು;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&ಸಾಲುಗಳು_to_free, ಪರಿಮಾಣ(char_u *), 50);
	    ಪೂರ್ಣಾಙ್ಕ class_flags;
	    ಯದಿ (is_interface)
		class_flags = CF_INTERFACE;
	    ಅನ್ಯಥಾ
		class_flags = abstract_method ? CF_ABSTRACT_METHOD : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, ರಿಕ್ತ, &ಸಾಲುಗಳು_to_free,
			class_flags, objmembers.ga_data, objmembers.ga_len,
			ರಿಕ್ತ);
	    ga_clear_strings(&ಸಾಲುಗಳು_to_free);

	    ಯದಿ (uf != ರಿಕ್ತ)
	    {
		ಯದಿ (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_method_not_supported_in_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    ಭಙ್ಗ;
		}

		// check for builtin method
		ಯದಿ (!is_new && SAFE_islower(*uf->uf_name) &&
					!object_check_builtin_method_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    ಭಙ್ಗ;
		}

		// Check the uf_name isn't used already.
		ಯದಿ (is_duplicate_method(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    ಭಙ್ಗ;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		ಯದಿ (ga_grow(fgap, 1) == OK)
		{
		    ಯದಿ (is_new)
			uf->uf_flags |= FC_NEW;

		    ಯದಿ (abstract_method)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_data)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	ಅನ್ಯಥಾ
	{
	    ಯದಿ (is_class)
		semsg(_(e_not_valid_command_in_class_str), ಸಾಲು);
	    ಅನ್ಯಥಾ ಯದಿ (is_enum)
		semsg(_(e_not_valid_command_in_enum_str), ಸಾಲು);
	    ಅನ್ಯಥಾ
		semsg(_(e_not_valid_command_in_interface_str), ಸಾಲು);
	    ಭಙ್ಗ;
	}
    }

    ಯದಿ (theline == ರಿಕ್ತ && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    ಯದಿ (success && is_enum && (ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು == 0 || !added_enum_ಮೌಲ್ಯಗಳು))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_add_ಮೌಲ್ಯಗಳು_member(cl, &classmembers, ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    ಯದಿ (success && extends != ರಿಕ್ತ)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    ಯದಿ (success && extends_cl != ರಿಕ್ತ)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    ಯದಿ (success && extends_cl != ರಿಕ್ತ)
    {
	ಯದಿ (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	ಅನ್ಯಥಾ
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    ಯದಿ (success && !is_abstract && extends_cl != ರಿಕ್ತ
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, ಪರಿಮಾಣ(class_T *), 5);

    ಯದಿ (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    ಯದಿ (success && extends_cl != ರಿಕ್ತ)
	success = add_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_data;
    intf_ಎಣಿಕೆ = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    ಯದಿ (success)
	success = check_func_arg_names(&classfunctions, &objmethods,
							&classmembers);

    ಯದಿ (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	ಯದಿ (extends_cl != ರಿಕ್ತ)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	ಯದಿ (add_members_to_class(&classmembers,
				 ರಿಕ್ತ,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_ಎಣಿಕೆ) == FAIL
		|| add_members_to_class(&objmembers,
				 extends_cl == ರಿಕ್ತ ? ರಿಕ್ತ
					       : extends_cl->class_obj_members,
				 extends_cl == ರಿಕ್ತ ? 0
					  : extends_cl->class_obj_member_ಎಣಿಕೆ,
				 &cl->class_obj_members,
				 &cl->class_obj_member_ಎಣಿಕೆ) == FAIL)
	    ಗಚ್ಛ cleanup;

	ಯದಿ (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_ಎಣಿಕೆ = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    ಯದಿ (cl->class_interfaces == ರಿಕ್ತ)
		ಗಚ್ಛ cleanup;
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_data)[i];
	    VIM_CLEAR(ga_impl.ga_data);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = ರಿಕ್ತ;
	}

	ಯದಿ (cl->class_interface_ಎಣಿಕೆ > 0 || extends_cl != ರಿಕ್ತ)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    ಯದಿ (add_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		ಗಚ್ಛ cleanup;
	}

	ಪೂರ್ಣಾಙ್ಕ	have_new = FALSE;
	ufunc_T	*class_func = ರಿಕ್ತ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_data)[i];
	    ಯದಿ (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		ಭಙ್ಗ;
	    }
	}

	ಯದಿ (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	ಅನ್ಯಥಾ ಯದಿ ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    add_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	ಯದಿ (add_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    ಗಚ್ಛ cleanup;

	update_builtin_method_index(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	ಯದಿ ((is_class || is_enum) && cl->class_class_member_ಎಣಿಕೆ > 0)
	    ಯದಿ (add_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		ಪ್ರತ್ಯಾಗಮ;
	    }

	cl->class_type_list = type_list;

	ಯದಿ (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	ಪ್ರತ್ಯಾಗಮ;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    ಯದಿ (intf_classes != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < intf_ಎಣಿಕೆ; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_strings(&ga_impl);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ round = 1; round <= 2; ++round)
    {
	garray_T *gap = round == 1 ? &classmembers : &objmembers;
	ಯದಿ (gap->ga_len == 0 || gap->ga_data == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < classfunctions.ga_len; ++i)
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
    ಪೂರ್ಣಾಙ್ಕ		is_object,
    char_u	*name,
    char_u	*name_end,
    ಪೂರ್ಣಾಙ್ಕ		*member_idx)
{
    size_t	len = name_end - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    ಯದಿ (m == ರಿಕ್ತ)
    {
	member_not_found_ಸಂದೇಶ(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	ಪ್ರತ್ಯಾಗಮ &t_any;
    }

    ಪ್ರತ್ಯಾಗಮ m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    ಪೂರ್ಣಾಙ್ಕ		is_object,
    ಪೂರ್ಣಾಙ್ಕ		member_idx)
{
    ocmember_T	*m;
    ಪೂರ್ಣಾಙ್ಕ		member_ಎಣಿಕೆ;

    ಯದಿ (is_object)
    {
	m = cl->class_obj_members;
	member_ಎಣಿಕೆ = cl->class_obj_member_ಎಣಿಕೆ;
    }
    ಅನ್ಯಥಾ
    {
	m = cl->class_class_members;
	member_ಎಣಿಕೆ = cl->class_class_member_ಎಣಿಕೆ;
    }

    ಯದಿ (member_idx >= member_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    ಶೂನ್ಯ
typealias_unref(typealias_T *ta)
{
    ಯದಿ (ta != ರಿಕ್ತ && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    ಶೂನ್ಯ
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    ಯದಿ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_in_vim9_script));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_start_with_uppercase_letter_str), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }

    char_u *name_end = find_name_end(arg, ರಿಕ್ತ, ರಿಕ್ತ, FNE_CHECK_START);
    ಯದಿ (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }
    char_u *name_start = arg;

    arg = skipwhite(name_end);
    ಯದಿ (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	ಪ್ರತ್ಯಾಗಮ;
    }
    arg++;
    arg = skipwhite(arg);

    ಯದಿ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	ಪ್ರತ್ಯಾಗಮ;
    }

    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, ರಿಕ್ತ, ರಿಕ್ತ, TRUE);
    ಯದಿ (type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪೂರ್ಣಾಙ್ಕ cc = *name_end;
    *name_end = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    ಯದಿ (eval_variable_import(name_start, &tv) == OK)
    {
	ಯದಿ (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_start);
	ಅನ್ಯಥಾ
	    semsg(_(e_redefining_script_item_str), name_start);
	clear_tv(&tv);
	ಗಚ್ಛ done;
    }

    // Create a script-local variable for the type alias.
    ಯದಿ (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_lock = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_start);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_depth = parse_type_depth();
    }
    ಅನ್ಯಥಾ
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_lock = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    set_var_const(name_start, current_sctx.sc_sid, ರಿಕ್ತ, &tv, FALSE,
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
    ಪೂರ್ಣಾಙ್ಕ
get_member_tv(
    class_T	*cl,
    ಪೂರ್ಣಾಙ್ಕ		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    ಪೂರ್ಣಾಙ್ಕ		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    ಯದಿ (m == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (*name == '_' && (current_class == ರಿಕ್ತ ||
				!class_instance_of(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	set_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    ಅನ್ಯಥಾ
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	set_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
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
    ಪೂರ್ಣಾಙ್ಕ		argcount = 0;
    ocmember_T	*ocm = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		m_idx;

    fp = method_lookup(cl, rettv->v_type, name, len, ರಿಕ್ತ);
    ಯದಿ (fp == ರಿಕ್ತ)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	ಯದಿ (ocm == ರಿಕ್ತ || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    method_not_found_ಸಂದೇಶ(cl, rettv->v_type, name, len);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	ಅನ್ಯಥಾ
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_end;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಯದಿ (ocm == ರಿಕ್ತ && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // process generic function call
    ಯದಿ (fp != ರಿಕ್ತ)
    {
	fp = generic_func_get(fp, gfatab);
	ಯದಿ (fp == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    char_u *argp = name_end;
    ಪೂರ್ಣಾಙ್ಕ ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    ಯದಿ (ret == FAIL)
    {
	ಯಾವತ್ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    funcexe_T funcexe;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    ಯದಿ (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    ಯದಿ (evalarg != ರಿಕ್ತ)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    ಪೂರ್ಣಾಙ್ಕ error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								ರಿಕ್ತ);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ಯದಿ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_found_var);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    *arg = argp;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
obj_method_to_partial_tv(object_T *obj, ufunc_T *obj_method, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    ಯದಿ (pt == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    pt->pt_refcount = 1;
    ಯದಿ (obj != ರಿಕ್ತ)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_method;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ
class_object_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಙ್ಕ		verbose UNUSED)	// give error messages
{
    ಪೂರ್ಣಾಙ್ಕ	ret = FAIL;

    ಯದಿ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ರಿಕ್ತ, ರಿಕ್ತ, FNE_CHECK_START);
    ಯದಿ (name_end == name)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    size_t len = name_end - name;

    ಪೂರ್ಣಾಙ್ಕ did_emsg_save = did_emsg;
    class_T *cl;
    ಯದಿ (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    ಅನ್ಯಥಾ // VAR_OBJECT
    {
	ಯದಿ (rettv->vval.v_object == ರಿಕ್ತ)
	{
	    emsg(_(e_using_null_object));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    ಯದಿ (cl == ರಿಕ್ತ)
    {
	emsg(_(e_incomplete_type));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    ಯದಿ (*name_end == '<')
    {
	cctx_T	    *cctx = ರಿಕ್ತ;

	ಯದಿ (evalarg != ರಿಕ್ತ)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_end = parse_generic_func_type_args(name, len, name + len,
						&gfatab, cctx);
	ಯದಿ (name_end == ರಿಕ್ತ)
	    ಗಚ್ಛ done;
    }

    ಯದಿ (*name_end == '(')
	// Invoke the class or object method
	ret = call_oc_method(cl, name, len, name_end, evalarg, arg,
							&gfatab, rettv);
    ಅನ್ಯಥಾ ಯದಿ (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	ಪೂರ್ಣಾಙ್ಕ is_object = rettv->v_type == VAR_OBJECT;
	ಯದಿ (get_member_tv(cl, is_object, name, len, ರಿಕ್ತ, rettv) == OK)
	{
	    *arg = name_end;
	    ret = OK;
	    ಗಚ್ಛ done;
	}

	// could be a class method or an object method
	ಪೂರ್ಣಾಙ್ಕ	fidx;
	ufunc_T	*fp = method_lookup(cl, rettv->v_type, name, len, &fidx);
	ಯದಿ (fp != ರಿಕ್ತ)
	{
	    // Protected methods are not accessible outside the class
	    ಯದಿ (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		ಗಚ್ಛ done;
	    }

	    ಯದಿ (obj_method_to_partial_tv(is_object ? rettv->vval.v_object :
						ರಿಕ್ತ, fp, rettv) == FAIL)
		ಗಚ್ಛ done;

	    *arg = name_end;
	    ret = OK;
	    ಗಚ್ಛ done;
	}

	ಯದಿ (did_emsg == did_emsg_save)
	    member_not_found_ಸಂದೇಶ(cl, rettv->v_type, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
find_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ರಿಕ್ತ, ರಿಕ್ತ, FNE_CHECK_START);
    ಯದಿ (name_end == name || *name_end != '.')
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ufunc_T	*fp = ರಿಕ್ತ;
    size_t	len = name_end - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    ಯದಿ (eval_variable(name, (ಪೂರ್ಣಾಙ್ಕ)len,
				    0, &tv, ರಿಕ್ತ, EVAL_VAR_NOAUTOLOAD) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	ಗಚ್ಛ fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    ಯದಿ (cl == ರಿಕ್ತ)
	ಗಚ್ಛ fail_after_eval;
    char_u *fname = name_end + 1;
    char_u *fname_end = find_name_end(fname, ರಿಕ್ತ, ರಿಕ್ತ, FNE_CHECK_START);
    ಯದಿ (fname_end == fname)
	ಗಚ್ಛ fail_after_eval;
    len = fname_end - fname;

    fp = method_lookup(cl, tv.v_type, fname, len, ರಿಕ್ತ);

    ಯದಿ (fp != ರಿಕ್ತ)
    {
	fp = eval_generic_func(fp, fname, &fname_end);
	ಯದಿ (fp != ರಿಕ್ತ)
	    *arg = fname_end;
    }

fail_after_eval:
    clear_tv(&tv);
    ಪ್ರತ್ಯಾಗಮ fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ಪೂರ್ಣಾಙ್ಕ
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    ಪೂರ್ಣಾಙ್ಕ idx;
    class_member_lookup(cl, name, namelen, &idx);
    ಪ್ರತ್ಯಾಗಮ idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, ಪೂರ್ಣಾಙ್ಕ *idx)
{
    ocmember_T	*ret_m = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ret_idx = -1;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_class_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	ಯದಿ (namelen)
	{
	    ಯದಿ (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ಭಙ್ಗ;
	}
    }
    ಯದಿ (idx != ರಿಕ್ತ)
	*idx = ret_idx;
    ಪ್ರತ್ಯಾಗಮ ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    ಸ್ಥಿರ ufunc_T *
class_method_lookup(class_T *cl, char_u *name, size_t namelen, ಪೂರ್ಣಾಙ್ಕ *idx)
{
    ufunc_T	*ret_fp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ret_idx = -1;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_class_function_ಎಣಿಕೆ; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	ಯದಿ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ಭಙ್ಗ;
	}
    }
    ಯದಿ (idx != ರಿಕ್ತ)
	*idx = ret_idx;
    ಪ್ರತ್ಯಾಗಮ ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
class_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    ಪೂರ್ಣಾಙ್ಕ idx;
    class_method_lookup(cl, name, namelen, &idx);
    ಪ್ರತ್ಯಾಗಮ idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    ಪೂರ್ಣಾಙ್ಕ idx;
    object_member_lookup(cl, name, namelen, &idx);
    ಪ್ರತ್ಯಾಗಮ idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, ಪೂರ್ಣಾಙ್ಕ *idx)
{
    ocmember_T	*ret_m = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ret_idx = -1;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_obj_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	ಯದಿ (namelen)
	{
	    ಯದಿ (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ಭಙ್ಗ;
	}
    }
    ಯದಿ (idx != ರಿಕ್ತ)
	*idx = ret_idx;
    ಪ್ರತ್ಯಾಗಮ ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    ಸ್ಥಿರ ufunc_T *
object_method_lookup(class_T *cl, char_u *name, size_t namelen, ಪೂರ್ಣಾಙ್ಕ *idx)
{
    ufunc_T	*ret_fp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ret_idx = -1;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_obj_method_ಎಣಿಕೆ; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	ಯದಿ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ಭಙ್ಗ;
	}
    }
    ಯದಿ (idx != ರಿಕ್ತ)
	*idx = ret_idx;
    ಪ್ರತ್ಯಾಗಮ ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
object_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    ಪೂರ್ಣಾಙ್ಕ idx;
    object_method_lookup(cl, name, namelen, &idx);
    ಪ್ರತ್ಯಾಗಮ idx;
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
    ಪೂರ್ಣಾಙ್ಕ		*idx)
{
    ಯದಿ (v_type == VAR_CLASS)
	ಪ್ರತ್ಯಾಗಮ class_member_lookup(cl, name, namelen, idx);

    ಪ್ರತ್ಯಾಗಮ object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    ಸ್ಥಿರ class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_found = ರಿಕ್ತ;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_found = ರಿಕ್ತ;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    ಪ್ರತಿ (class_T *super = cl; super != ರಿಕ್ತ; super = super->class_extends)
    {
	class_T		*cl_tmp = ರಿಕ್ತ;
	ocmember_T	*m = ರಿಕ್ತ;
	ಯದಿ (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    ಯದಿ ((m = object_member_lookup(super, name, len, ರಿಕ್ತ)) != ರಿಕ್ತ)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	ಯದಿ (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    ಯದಿ (( m = class_member_lookup(super, name, len, ರಿಕ್ತ)) != ರಿಕ್ತ)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	ಯದಿ (cl_tmp == ರಿಕ್ತ)
	    ಭಙ್ಗ;  // member is not in this or any super class.
	cl_found = cl_tmp;
	m_found = m;
    }
    ಯದಿ (p_m != ರಿಕ್ತ)
	*p_m = m_found;
    ಪ್ರತ್ಯಾಗಮ cl_found;
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
    ಪೂರ್ಣಾಙ್ಕ		*idx)
{
    ಯದಿ (v_type == VAR_CLASS)
	ಪ್ರತ್ಯಾಗಮ class_method_lookup(cl, name, namelen, idx);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ object_method_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    ಪೂರ್ಣಾಙ್ಕ
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    ಪ್ರತಿ (cctx_T *cctx = cctx_arg; cctx != ರಿಕ್ತ; cctx = cctx->ctx_outer)
	ಯದಿ (cctx->ctx_ufunc != ರಿಕ್ತ
			&& class_instance_of(cctx->ctx_ufunc->uf_class, cl))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    ಪೂರ್ಣಾಙ್ಕ
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    ಯದಿ (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_in_class_str),
		m->ocm_name.string, cl->class_name.string);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    ಶೂನ್ಯ
obj_lock_const_vars(object_T *obj)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < obj->obj_class->class_obj_member_ಎಣಿಕೆ; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	ಯದಿ (ocm->ocm_flags & OCMFLAG_CONST)
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
    ಪೂರ್ಣಾಙ್ಕ		sz;

    ಯದಿ (cl == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    sz = ಪರಿಮಾಣ(object_T) + cl->class_obj_member_ಎಣಿಕೆ * ಪರಿಮಾಣ(typval_T);
    obj = alloc_clear(sz);
    ಯದಿ (obj == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    ಯದಿ (IS_ENUM(cl))
	enum_set_internal_obj_vars(cl, obj);

    ಪ್ರತ್ಯಾಗಮ obj;
}

/*
 * Make a copy of an object.
 */
    ಶೂನ್ಯ
copy_object(typval_T *from, typval_T *to)
{
    ಯದಿ (from->vval.v_object == ರಿಕ್ತ)
	to->vval.v_object = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	to->vval.v_object = from->vval.v_object;
	++to->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    ಶೂನ್ಯ
copy_class(typval_T *from, typval_T *to)
{
    ಯದಿ (from->vval.v_class == ರಿಕ್ತ)
	to->vval.v_class = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	to->vval.v_class = from->vval.v_class;
	++to->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_interface_ಎಣಿಕೆ; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	ಯದಿ (cl->class_interfaces_cl[i] != ರಿಕ್ತ)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *next;
    ಪ್ರತಿ (itf2class_T *i2c = cl->class_itf2class; i2c != ರಿಕ್ತ; i2c = next)
    {
	next = i2c->i2c_next;
	vim_free(i2c);
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_class_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	ಯದಿ (cl->class_members_tv != ರಿಕ್ತ)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_obj_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_class_function_ಎಣಿಕೆ; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	ಯದಿ (IS_ENUM(cl) && IS_CONSTRUCTOR_METHOD(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_obj_method_ಎಣಿಕೆ; ++i)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
class_get_selfrefs(class_T *cl)
{
    ಪೂರ್ಣಾಙ್ಕ		self_refs = 0;
    typval_T	*tv;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_class_member_ಎಣಿಕೆ; ++i)
    {
	tv = &cl->class_members_tv[i];
	ಯದಿ (tv->v_type == VAR_OBJECT && tv->vval.v_object != ರಿಕ್ತ
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    ಪ್ರತ್ಯಾಗಮ self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
can_free_enum(class_T *cl)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_class_member_ಎಣಿಕೆ; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	ಯದಿ (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ಯದಿ (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    ಯದಿ (tv->vval.v_list->lv_refcount > 1)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಭಙ್ಗ;
	}

	ಯದಿ (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
can_free_class(class_T *cl)
{
    ಪೂರ್ಣಾಙ್ಕ		can_free = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		self_refs = 0;

    ಯದಿ (IS_ENUM(cl) && !can_free_enum(cl))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    ಯದಿ (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    ಪ್ರತ್ಯಾಗಮ can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    ಶೂನ್ಯ
class_unref(class_T *cl)
{
    ಯದಿ (cl == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    --cl->class_refcount;

    ಯದಿ (cl->class_name.string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    ಪೂರ್ಣಾಙ್ಕ
class_free_nonref(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		did_free = FALSE;

    ಪ್ರತಿ (class_T *cl = first_class; cl != ರಿಕ್ತ; cl = next_nonref_class)
    {
	next_nonref_class = cl->class_next_used;
	ಯದಿ ((cl->class_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    next_nonref_class = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ did_free;
}

    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_classes(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪ್ರತಿ (class_T *cl = first_class; cl != ರಿಕ್ತ; cl = cl->class_next_used)
	set_ref_in_item_class(cl, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ object_T *first_object = ರಿಕ್ತ;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    ಶೂನ್ಯ
object_created(object_T *obj)
{
    ಯದಿ (first_object != ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
object_cleared(object_T *obj)
{
    ಯದಿ (obj->obj_next_used != ರಿಕ್ತ)
	obj->obj_next_used->obj_prev_used = obj->obj_prev_used;
    ಯದಿ (obj->obj_prev_used != ರಿಕ್ತ)
	obj->obj_prev_used->obj_next_used = obj->obj_next_used;
    ಅನ್ಯಥಾ ಯದಿ (first_object == obj)
	first_object = obj->obj_next_used;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    ಯದಿ (!cl)
	ಪ್ರತ್ಯಾಗಮ;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = INT_MAX;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_obj_member_ಎಣಿಕೆ; ++i)
	clear_tv(tv + i);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    ಯದಿ (!cl)
	ಪ್ರತ್ಯಾಗಮ;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
object_free(object_T *obj)
{
    ಯದಿ (in_free_unref_ವಸ್ತುಗಳು)
	ಪ್ರತ್ಯಾಗಮ;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    ಶೂನ್ಯ
object_unref(object_T *obj)
{
    ಯದಿ (obj != ರಿಕ್ತ && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    ಪೂರ್ಣಾಙ್ಕ
object_free_nonref(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		did_free = FALSE;

    ಪ್ರತಿ (object_T *obj = first_object; obj != ರಿಕ್ತ; obj = obj->obj_next_used)
    {
	ಯದಿ ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    ಪ್ರತ್ಯಾಗಮ did_free;
}

    ಶೂನ್ಯ
object_free_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    object_T	*obj_next;

    ಪ್ರತಿ (object_T *obj = first_object; obj != ರಿಕ್ತ; obj = obj_next)
    {
	obj_next = obj->obj_next_used;
	ಯದಿ ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    ಶೂನ್ಯ
emsg_var_cl_define(ವರ್ಣ *ಸಂದೇಶ, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    ಯದಿ (cl_def != ರಿಕ್ತ)
	semsg(_(ಸಂದೇಶ), m->ocm_name.string, cl_def->class_name.string);
    ಅನ್ಯಥಾ
	emsg(_(e_internal_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    ಶೂನ್ಯ
method_not_found_ಸಂದೇಶ(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *method_name = vim_strnsave(name, len);
    ಯದಿ ((v_type == VAR_OBJECT)
	    && (class_method_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	ಯದಿ (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	ಅನ್ಯಥಾ
	    semsg(_(e_class_method_str_accessible_only_using_class_str),
		    method_name, cl->class_name.string);
    }
    ಅನ್ಯಥಾ ಯದಿ ((v_type == VAR_CLASS)
	    && (object_method_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	ಯದಿ (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	ಅನ್ಯಥಾ
	    semsg(_(e_object_method_str_accessible_only_using_object_str),
		    method_name, cl->class_name.string);
    }
    ಅನ್ಯಥಾ
	semsg(_(e_method_not_found_on_class_str_str), method_name,
		cl->class_name.string);
    vim_free(method_name);
}

/*
 * Echo a class or object member not found message.
 */
    ಶೂನ್ಯ
member_not_found_ಸಂದೇಶ(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    ಯದಿ (v_type == VAR_OBJECT)
    {
	ಯದಿ (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	ಅನ್ಯಥಾ
	    semsg(_(e_variable_not_found_on_object_str_str), varname,
		    cl->class_name.string);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (IS_ENUM(cl))
		semsg(_(e_enum_ಮೌಲ್ಯ_str_not_found_in_enum_str),
			varname, cl->class_name.string);
	    ಅನ್ಯಥಾ
		semsg(_(e_class_variable_str_not_found_in_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    ಶೂನ್ಯ
defcompile_class(class_T *cl)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ loop = 1; loop <= 2; ++loop)
    {
	ಪೂರ್ಣಾಙ್ಕ func_ಎಣಿಕೆ = loop == 1 ? cl->class_class_function_ಎಣಿಕೆ
						: cl->class_obj_method_ಎಣಿಕೆ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < func_ಎಣಿಕೆ; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    ಯದಿ (!IS_ABSTRACT_METHOD(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    ಶೂನ್ಯ
defcompile_classes_in_script(ಶೂನ್ಯ)
{
    ಪ್ರತಿ (class_T *cl = first_class; cl != ರಿಕ್ತ; cl = cl->class_next_used)
    {
	ಯದಿ (eval_variable(cl->class_name.string, 0, 0, ರಿಕ್ತ, ರಿಕ್ತ,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    ಪೂರ್ಣಾಙ್ಕ
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    ಯದಿ (eval_variable(name, 0, 0, rettv, ರಿಕ್ತ, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	ಪ್ರತ್ಯಾಗಮ rettv->v_type == VAR_CLASS;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
object_call_builtin_method(
    object_T		*obj,
    class_builtin_T	builtin_method,
    ಪೂರ್ಣಾಙ್ಕ			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    ಪೂರ್ಣಾಙ್ಕ	    midx;

    ಯದಿ (obj == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    uf = class_get_builtin_method(obj->obj_class, builtin_method, &midx);
    ಯದಿ (uf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    ಪೂರ್ಣಾಙ್ಕ		r;

    ಯದಿ (fc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, ರಿಕ್ತ, obj, fc, rettv);

    remove_funccal();

    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    ಪೂರ್ಣಾಙ್ಕ
object_empty(object_T *obj)
{
    typval_T	rettv;

    ಯದಿ (object_call_builtin_method(obj, CLASS_BUILTIN_EMPTY, 0, ರಿಕ್ತ, &rettv)
								== FAIL)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪ್ರತ್ಯಾಗಮ tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    ಪೂರ್ಣಾಙ್ಕ
object_len(object_T *obj)
{
    typval_T	rettv;

    ಯದಿ (object_call_builtin_method(obj, CLASS_BUILTIN_LEN, 0, ರಿಕ್ತ, &rettv)
								== FAIL)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತ್ಯಾಗಮ tv_to_ಸಂಖ್ಯೆ(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    ಪೂರ್ಣಾಙ್ಕ
object_equal(
	object_T *o1,
	object_T *o2,
	ಪೂರ್ಣಾಙ್ಕ	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    ಯದಿ (o1 == o2)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (o1 == ರಿಕ್ತ || o2 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    ಯದಿ (cl1 != cl2 || cl1 == ರಿಕ್ತ || cl2 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl1->class_obj_member_ಎಣಿಕೆ; ++i)
	ಯದಿ (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಪೂರ್ಣಾಙ್ಕ		copyID,
    ಪೂರ್ಣಾಙ್ಕ		echo_style,
    ಪೂರ್ಣಾಙ್ಕ		restore_copyID,
    ಪೂರ್ಣಾಙ್ಕ		composite_val)
{
    typval_T	rettv;

    ಯದಿ (object_call_builtin_method(obj, CLASS_BUILTIN_STRING, 0, ರಿಕ್ತ, &rettv)
								== OK
					&& rettv.vval.v_string != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ rettv.vval.v_string;

    ಪೂರ್ಣಾಙ್ಕ		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    ಯದಿ (cl != ರಿಕ್ತ && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    ಅನ್ಯಥಾ
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	ಯದಿ (cl == ರಿಕ್ತ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ಅನ್ಯಥಾ
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
    }
    ಯದಿ (cl != ರಿಕ್ತ)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < cl->class_obj_member_ಎಣಿಕೆ; ++i)
	{
	    ಯದಿ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.length);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = ರಿಕ್ತ;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, composite_val);
	    ಯದಿ (s != ರಿಕ್ತ)
		ga_concat(&ga, s);
	    vim_free(tf);
	    ಯದಿ (s == ರಿಕ್ತ || did_echo_string_emsg)
	    {
		ok = FAIL;
		ಭಙ್ಗ;
	    }
	    ಸಾಲು_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    ಯದಿ (ok == FAIL)
    {
	vim_free(ga.ga_data);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ga_append(&ga, NUL);
    ಪ್ರತ್ಯಾಗಮ (char_u *)ga.ga_data;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    ಪೂರ್ಣಾಙ್ಕ
class_instance_of(class_T *cl, class_T *other_cl)
{
    ಯದಿ (cl == other_cl)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // Recursively check the base classes.
    ಪ್ರತಿ (; cl != ರಿಕ್ತ; cl = cl->class_extends)
    {
	ಯದಿ (cl == other_cl)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	// Check the implemented interfaces and the super interfaces
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = cl->class_interface_ಎಣಿಕೆ - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    ಯಾವತ್ (intf != ರಿಕ್ತ)
	    {
		ಯದಿ (intf == other_cl)
		    ಪ್ರತ್ಯಾಗಮ TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    ಶೂನ್ಯ
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;

    ಯದಿ (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_args(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (object_tv->vval.v_object == ರಿಕ್ತ)
    {
	ಯದಿ (classinfo_tv->vval.v_class == ರಿಕ್ತ)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	ಯದಿ (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	ಅನ್ಯಥಾ
	    c = classinfo_tv->vval.v_class;

	ಯದಿ (class_instance_of(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
}

#endif // FEAT_EVAL
