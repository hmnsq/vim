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
#अन्तर्भाव "vim.h"

#जइ defined(FEAT_EVAL)

स्थिर class_T *first_class = रिक्त;
स्थिर class_T *next_nonref_class = रिक्त;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    स्थिर शून्य
class_created(class_T *cl)
{
    जइ (first_class != रिक्त)
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
    स्थिर शून्य
class_cleared(class_T *cl)
{
    जइ (cl->class_next_used != रिक्त)
	cl->class_next_used->class_prev_used = cl->class_prev_used;
    जइ (cl->class_prev_used != रिक्त)
	cl->class_prev_used->class_next_used = cl->class_next_used;
    अण्णहा जइ (first_class == cl)
	first_class = cl->class_next_used;

    // update the next class to check if needed
    जइ (cl == next_nonref_class)
	next_nonref_class = cl->class_next_used;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    स्थिर पूर्णाङ्क
parse_member(
    exarg_T	*eap,
    char_u	*पङ्क्तिः,
    char_u	*varname,
    पूर्णाङ्क		has_public,	    // TRUE if "public" seen before "varname"
    char_u	**varname_end,
    पूर्णाङ्क		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_व्यञ्जकम्)
{
    *varname_end = to_name_end(varname, FALSE);
    जइ (*varname == '_' && has_public)
    {
	semsg(_(e_public_variable_name_cannot_start_with_underscore_str), पङ्क्तिः);
	प्रत्यागम FAIL;
    }

    char_u *colon = skipwhite(*varname_end);
    char_u *type_arg = colon;
    type_T *type = रिक्त;
    *has_type = FALSE;
    जइ (*colon == ':')
    {
	जइ (VIM_ISWHITE(**varname_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    प्रत्यागम FAIL;
	}
	जइ (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    प्रत्यागम FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, रिक्त, रिक्त, TRUE);
	जइ (type == रिक्त || !valid_declaration_type(type))
	    प्रत्यागम FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    जइ (type == रिक्त && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	प्रत्यागम FAIL;
    }

    जइ (init_व्यञ्जकम् == रिक्त && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_in_interface));
	प्रत्यागम FAIL;
    }

    जइ (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *व्यञ्जकम्_start, *व्यञ्जकम्_end;

	जइ (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    प्रत्यागम FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(शून्य)skip_व्यञ्जकम्_concatenate(&init_arg, &व्यञ्जकम्_start, &व्यञ्जकम्_end, &evalarg);

	init_arg = skipwhite(init_arg);
	जइ (*init_arg != NUL && !vim9_comment_start(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, रिक्त);
	    प्रत्यागम FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	जइ (type == रिक्त)
	    type = &t_any;

	*init_व्यञ्जकम् = vim_strnsave(व्यञ्जकम्_start, व्यञ्जकम्_end - व्यञ्जकम्_start);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, रिक्त);
    }
    अण्णहा जइ (!valid_declaration_type(type))
	प्रत्यागम FAIL;

    *type_ret = type;
    प्रत्यागम OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    स्थिर पूर्णाङ्क
add_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_end,
    पूर्णाङ्क		has_public,
    पूर्णाङ्क		has_final,
    पूर्णाङ्क		has_const,
    पूर्णाङ्क		has_type,
    type_T	*type,
    char_u	*init_व्यञ्जकम्)
{
    जइ (ga_grow(gap, 1) == FAIL)
	प्रत्यागम FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_data) + gap->ga_len;
    m->ocm_name.length = (size_t)(varname_end - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.length);
    जइ (m->ocm_name.string == रिक्त)
    {
	m->ocm_name.length = 0;
	प्रत्यागम FAIL;
    }
    m->ocm_access = has_public ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_PRIVATE : VIM_ACCESS_READ;
    जइ (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    जइ (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    जइ (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    जइ (init_व्यञ्जकम् != रिक्त)
    {
	m->ocm_init = init_व्यञ्जकम्;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
add_members_to_class(
    garray_T	*gap,
    ocmember_T	*parent_members,
    पूर्णाङ्क		parent_गणनम्,
    ocmember_T	**members,
    पूर्णाङ्क		*member_गणनम्)
{
    *member_गणनम् = parent_गणनम् + gap->ga_len;
    *members = *member_गणनम् == 0 ? रिक्त
				       : ALLOC_MULT(ocmember_T, *member_गणनम्);
    जइ (*member_गणनम् > 0 && *members == रिक्त)
	प्रत्यागम FAIL;
    प्रति (पूर्णाङ्क i = 0; i < parent_गणनम्; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = parent_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.length);
	जइ (m->ocm_name.string == रिक्त)
	{
	    m->ocm_name.length = 0;
	    प्रत्यागम FAIL;
	}
	जइ (m->ocm_init != रिक्त)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    जइ (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + parent_गणनम्,
			       gap->ga_data, परिमाण(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_data);
    प्रत्यागम OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    पूर्णाङ्क
object_index_from_itf_index(class_T *itf, पूर्णाङ्क is_method, पूर्णाङ्क idx, class_T *cl)
{
    जइ (idx >= (is_method ? itf->class_obj_method_गणनम्
				   : itf->class_obj_member_गणनम्))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	प्रत्यागम 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    जइ (cl == itf)
	प्रत्यागम idx;

    itf2class_T		*i2c = रिक्त;
    पूर्णाङ्क			searching = TRUE;
    पूर्णाङ्क			method_offset = 0;

    प्रति (class_T *super = cl; super != रिक्त && searching;
						super = super->class_extends)
    {
	प्रति (i2c = itf->class_itf2class; i2c != रिक्त; i2c = i2c->i2c_next)
	{
	    जइ (i2c->i2c_class == super && i2c->i2c_is_method == is_method)
	    {
		searching = FALSE;
		भङ्ग;
	    }
	}
	जइ (searching && is_method)
	    // The parent class methods are stored after the current class
	    // methods.
	    method_offset += super->class_obj_method_गणनम्_child;
    }
    जइ (i2c == रिक्त)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	प्रत्यागम 0;
    }

    // A table follows the i2c for the class
    पूर्णाङ्क *table = (पूर्णाङ्क *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    प्रत्यागम table[idx] + method_offset;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    स्थिर पूर्णाङ्क
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    पूर्णाङ्क	    is_class)
{
    typval_T	tv;
    पूर्णाङ्क		success = FALSE;

    जइ (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	प्रत्यागम success;
    }

    tv.v_type = VAR_UNKNOWN;
    जइ (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_found_str), extends_name);
	प्रत्यागम success;
    }

    जइ (tv.v_type != VAR_CLASS || tv.vval.v_class == रिक्त
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    अण्णहा
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    प्रत्यागम success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    स्थिर पूर्णाङ्क
validate_extends_generic_method(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    वर्ण	*सन्देशः = रिक्त;

    जइ (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	प्रत्यागम TRUE;

    जइ (super_fp->uf_generic_argcount == 0)
	सन्देशः = e_concrete_method_str_override_with_generic_method_in_class_str;
    अण्णहा जइ (cl_fp->uf_generic_argcount == 0)
	सन्देशः = e_generic_method_str_override_with_concrete_method_in_class_str;
    अण्णहा
	सन्देशः = e_generic_method_str_type_arguments_mismatch_in_class_str;

    semsg(_(सन्देशः), cl_fp->uf_name, super_cl->class_name.string);
    प्रत्यागम FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    स्थिर पूर्णाङ्क
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    पूर्णाङ्क		method_गणनम् = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_data);

    जाव (super != रिक्त)
    {
	पूर्णाङ्क extends_method_गणनम् = super->class_obj_method_गणनम्_child;
	जइ (extends_method_गणनम् == 0)
	{
	    super = super->class_extends;
	    अनुवर्त;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	प्रति (पूर्णाङ्क i = 0; i < extends_method_गणनम्; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    पूर्णाङ्क	    extends_private = (*pstr == '_');
	    जइ (extends_private)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    प्रति (पूर्णाङ्क j = 0; j < method_गणनम्; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		पूर्णाङ्क	priv_method = (*qstr == '_');
		जइ (priv_method)
		    qstr++;
		जइ (STRCMP(pstr, qstr) == 0)
		{
		    जइ (priv_method != extends_private)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_method_str_of_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			प्रत्यागम FALSE;
		    }

		    जइ (!validate_extends_generic_method(super,
						extends_methods[i], cl_fp[j]))
			    प्रत्यागम FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    प्रत्यागम TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    स्थिर पूर्णाङ्क
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    पूर्णाङ्क member_गणनम् = objmembers_gap->ga_len;
    जइ (member_गणनम् == 0)
	प्रत्यागम TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    प्रति (पूर्णाङ्क c_i = 0; c_i < member_गणनम्; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	जाव (p_cl != रिक्त)
	{
	    पूर्णाङ्क p_member_गणनम् = p_cl->class_obj_member_गणनम्;

	    जइ (p_member_गणनम् == 0)
	    {
		p_cl = p_cl->class_extends;
		अनुवर्त;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    प्रति (पूर्णाङ्क p_i = 0; p_i < p_member_गणनम्; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		जइ (STRCMP(pstr, qstr) == 0)
		{
		    जइ (c_protected != p_protected)
			semsg(_(e_public_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    अण्णहा
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    प्रत्यागम FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    प्रत्यागम TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    स्थिर पूर्णाङ्क
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    पूर्णाङ्क member_गणनम् = objmembers_gap->ga_len;
    जइ (member_गणनम् == 0)
	प्रत्यागम TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    प्रति (पूर्णाङ्क c_i = 0; c_i < member_गणनम्; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	पूर्णाङ्क	    var_found = FALSE;

	// Check in all the parent classes in the lineage
	जाव (p_cl != रिक्त && !var_found)
	{
	    पूर्णाङ्क p_member_गणनम् = p_cl->class_obj_member_गणनम्;
	    जइ (p_member_गणनम् == 0)
	    {
		p_cl = p_cl->class_extends;
		अनुवर्त;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    प्रति (पूर्णाङ्क p_i = 0; p_i < p_member_गणनम्; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		जइ (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    अनुवर्त;

		// Ensure the type is matching.
		where.wt_func_name = (वर्ण *)c_m->ocm_name.string;
		where.wt_kind = WT_MEMBER;

		जइ (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    प्रत्यागम FALSE;

		var_found = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    प्रत्यागम TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    स्थिर पूर्णाङ्क
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    प्रति (पूर्णाङ्क loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	पूर्णाङ्क extends_method_गणनम् = loop == 1
				? extends_cl->class_class_function_गणनम्
				: extends_cl->class_obj_method_गणनम्;
	जइ (extends_method_गणनम् == 0)
	    अनुवर्त;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	पूर्णाङ्क method_गणनम् = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_data
						: objmethods_gap->ga_data);

	प्रति (पूर्णाङ्क i = 0; i < extends_method_गणनम्; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    जइ (!IS_ABSTRACT_METHOD(uf))
		अनुवर्त;

	    पूर्णाङ्क	concrete_method_found = FALSE;
	    पूर्णाङ्क	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    प्रति (j = 0; !concrete_method_found && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		जइ (!IS_ABSTRACT_METHOD(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_method_found = TRUE;
	    }

	    जइ (concrete_method_found)
		अनुवर्त;

	    प्रति (j = 0; j < method_गणनम्; j++)
	    {
		जइ (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_method_found = TRUE;
		    भङ्ग;
		}
	    }

	    जइ (!concrete_method_found)
	    {
		semsg(_(e_abstract_method_str_not_implemented), uf->uf_name);
		प्रत्यागम FALSE;
	    }
	}
    }

    प्रत्यागम TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    स्थिर पूर्णाङ्क
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    पूर्णाङ्क		cl_member_गणनम्,
    class_T	*extends_cl)
{
    पूर्णाङ्क		variable_present  = FALSE;

    प्रति (पूर्णाङ्क cl_i = 0; cl_i < cl_member_गणनम्; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	जइ (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    अनुवर्त;

	// Ensure the access type is same
	जइ (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_of_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    प्रत्यागम FALSE;
	}

	// Ensure the type is matching.
	जइ (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	अण्णहा
	{
	    where.wt_func_name = (वर्ण *)m->ocm_name.string;
	    where.wt_kind = WT_MEMBER;
	    जइ (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		प्रत्यागम FALSE;
	}

	variable_present = TRUE;
	भङ्ग;
    }

    जइ (!variable_present && extends_cl != रिक्त)
    {
	पूर्णाङ्क ext_cl_गणनम् = extends_cl->class_obj_member_गणनम्;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	प्रत्यागम intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_गणनम्,
					extends_cl->class_extends);
    }

    प्रत्यागम variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    स्थिर पूर्णाङ्क
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    पूर्णाङ्क if_गणनम् = ifcl->class_obj_member_गणनम्;
    जइ (if_गणनम् == 0)
	प्रत्यागम TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_data);
    पूर्णाङ्क cl_गणनम् = objmembers_gap->ga_len;
    प्रति (पूर्णाङ्क if_i = 0; if_i < if_गणनम्; ++if_i)
    {
	जइ (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_गणनम्, extends_cl))
	{
	    semsg(_(e_variable_str_of_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    प्रत्यागम FALSE;
	}
    }

    प्रत्यागम TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    स्थिर पूर्णाङ्क
intf_method_type_matches(
    ufunc_T	*if_method,
    ufunc_T	*cl_method,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    जइ (cl_method->uf_generic_argcount != if_method->uf_generic_argcount)
    {
	semsg(_(e_type_var_गणनम्_of_method_str_differs_from_interface_str),
						cl_method->uf_name, intf_name);
	प्रत्यागम FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (वर्ण *)if_method->uf_name;
    where.wt_kind = WT_METHOD;
    जइ (check_type(if_method->uf_func_type, cl_method->uf_func_type, TRUE,
								where) == FAIL)
	प्रत्यागम FALSE;

    प्रत्यागम TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    स्थिर पूर्णाङ्क
intf_method_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    पूर्णाङ्क	    cl_गणनम्,
    class_T *extends_cl,
    char_u  *intf_name)
{
    पूर्णाङ्क		method_present  = FALSE;

    प्रति (पूर्णाङ्क cl_i = 0; cl_i < cl_गणनम्; ++cl_i)
    {
	जइ (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    जइ (!intf_method_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		प्रत्यागम FALSE;
	    method_present = TRUE;
	    भङ्ग;
	}
    }

    जइ (!method_present && extends_cl != रिक्त)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	पूर्णाङ्क	ext_cl_गणनम् = extends_cl->class_obj_method_गणनम्;
	प्रत्यागम intf_method_present(if_ufunc, ext_cl_fp, ext_cl_गणनम्,
					extends_cl->class_extends, intf_name);
    }

    प्रत्यागम method_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    स्थिर पूर्णाङ्क
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    पूर्णाङ्क if_गणनम् = ifcl->class_obj_method_गणनम्;
    जइ (if_गणनम् == 0)
	प्रत्यागम TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_data);
    पूर्णाङ्क cl_गणनम् = objmethods_gap->ga_len;
    प्रति (पूर्णाङ्क if_i = 0; if_i < if_गणनम्; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	जइ (!intf_method_present(if_fp[if_i], cl_fp, cl_गणनम्, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_method_str_of_interface_str_not_implemented),
		    if_name, intf_class_name);
	    प्रत्यागम FALSE;
	}
    }

    प्रत्यागम TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    स्थिर पूर्णाङ्क
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    पूर्णाङ्क		success = TRUE;

    प्रति (पूर्णाङ्क i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_data)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	जइ (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_found_str), impl);
	    success = FALSE;
	    भङ्ग;
	}

	जइ (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == रिक्त
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    भङ्ग;
	}

	class_T *ifcl = tv.vval.v_class;
	जइ (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    भङ्ग;
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

    जइ (!success)
	प्रत्यागम FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_data;
    cl->class_interface_गणनम् = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    प्रति (पूर्णाङ्क i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_data)[i],
		((class_T **)intf_classes_gap->ga_data)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = रिक्त;
    cl->class_interface_गणनम् = 0;
    cl->class_extends = रिक्त;

    प्रत्यागम success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    स्थिर पूर्णाङ्क
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    प्रति (पूर्णाङ्क j = 0; j < intf_classes_gap->ga_len; j++)
    {
	जइ (((class_T **)intf_classes_gap->ga_data)[j] == ifcl)
	    प्रत्यागम TRUE;
    }

    प्रत्यागम FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    स्थिर पूर्णाङ्क
add_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.length);
    जइ (intf_name == रिक्त)
	प्रत्यागम FALSE;

    जइ (ga_grow(impl_gap, 1) == FAIL)
	गच्छ fail;

    char_u **intf_names = (char_u **)impl_gap->ga_data;
    intf_names[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    जइ (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	गच्छ fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_data;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    प्रत्यागम TRUE;
fail:
    vim_free(intf_name);
    प्रत्यागम FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    स्थिर पूर्णाङ्क
add_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    प्रति (पूर्णाङ्क i = 0; i < super->class_interface_गणनम्; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	जइ (!is_interface_class_present(intf_classes_gap, ifcl))
	    add_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    प्रत्यागम TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    स्थिर पूर्णाङ्क
check_func_arg_names(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    प्रति (पूर्णाङ्क loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	प्रति (पूर्णाङ्क fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_data)[fi];

	    प्रति (पूर्णाङ्क i = 0; i < uf->uf_args.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_args.ga_data)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		प्रति (पूर्णाङ्क mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_data + mi)->ocm_name.string;
		    जइ (STRCMP(aname, mname) == 0)
		    {
			जइ (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_argument_already_declared_in_class_str),
				aname);

			प्रत्यागम FALSE;
		    }
		}
	    }
	}
    }

    प्रत्यागम TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    स्थिर पूर्णाङ्क
is_reserved_varname(char_u *varname, char_u *varname_end)
{
    पूर्णाङ्क reserved = FALSE;
    char_u save_varname_end = *varname_end;
    *varname_end = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_end = save_varname_end;

    प्रत्यागम reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    स्थिर पूर्णाङ्क
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_end)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_end - varname)};
    पूर्णाङ्क		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    जइ (*pstr.string == '_')
    {
	pstr.string++;
	pstr.length--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    प्रति (पूर्णाङ्क loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	प्रति (पूर्णाङ्क i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_data) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.length};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    जइ (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.length--;
		q_protected = true;
	    }

	    जइ (pstr.length == qstr.length
		&& STRNCMP(pstr.string, qstr.string, pstr.length) == 0)
	    {
		char_u	save_c = *varname_end;

		*varname_end = NUL;
		जइ (p_protected != q_protected)
		    semsg(_(e_public_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		अण्णहा
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_end = save_c;
		dup = TRUE;
		भङ्ग;
	    }
	}
    }

    प्रत्यागम dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    स्थिर पूर्णाङ्क
is_duplicate_method(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    प्रति (पूर्णाङ्क loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	प्रति (पूर्णाङ्क i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_data)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    जइ (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		प्रत्यागम TRUE;
	    }
	}
    }

    प्रत्यागम FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    स्थिर पूर्णाङ्क
is_valid_constructor(ufunc_T *uf, पूर्णाङ्क is_abstract, पूर्णाङ्क has_static)
{
    // Constructors are not allowed in abstract classes.
    जइ (is_abstract)
    {
	emsg(_(e_cannot_define_new_method_in_abstract_class));
	प्रत्यागम FALSE;
    }
    // A constructor is always static, no need to define it so.
    जइ (has_static)
    {
	emsg(_(e_cannot_define_new_method_as_static));
	प्रत्यागम FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    जइ (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_method));
	प्रत्यागम FALSE;
    }
    प्रत्यागम TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    स्थिर पूर्णाङ्क
object_check_builtin_method_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    पूर्णाङ्क	    valid = FALSE;
    type_T  method_sig;
    type_T  method_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_FIELD(method_sig);
    CLEAR_FIELD(method_rt);
    method_sig.tt_type = VAR_FUNC;

    जइ (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	method_rt.tt_type = VAR_सङ्ख्या;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    अण्णहा जइ (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	method_rt.tt_type = VAR_BOOL;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    अण्णहा जइ (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	method_rt.tt_type = VAR_STRING;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    अण्णहा
	semsg(_(e_builtin_object_method_str_not_supported), uf->uf_name);

    where.wt_func_name = (वर्ण *)uf->uf_name;
    where.wt_kind = WT_METHOD;
    जइ (valid && !check_type(&method_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    प्रत्यागम valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    पूर्णाङ्क
is_valid_builtin_obj_methodname(char_u *funcname)
{
    विकल्प (funcname[0])
    {
	पक्ष 'e':
	    प्रत्यागम STRNCMP(funcname, "empty", 5) == 0;

	पक्ष 'l':
	    प्रत्यागम STRNCMP(funcname, "len", 3) == 0;

	पक्ष 'n':
	    प्रत्यागम STRNCMP(funcname, "new", 3) == 0;

	पक्ष 's':
	    प्रत्यागम STRNCMP(funcname, "string", 6) == 0;
    }

    प्रत्यागम FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_method(
    class_T		*cl,
    class_builtin_T	builtin_method,
    पूर्णाङ्क			*method_idx)
{
    *method_idx = -1;

    जइ (cl == रिक्त)
	प्रत्यागम रिक्त;

    *method_idx = cl->class_builtin_methods[builtin_method];
    प्रत्यागम *method_idx != -1 ? cl->class_obj_methods[*method_idx] : रिक्त;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    स्थिर पूर्णाङ्क
update_member_method_lookup_table(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    पूर्णाङ्क		pobj_method_offset)
{
    जइ (ifcl == रिक्त)
	प्रत्यागम OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(परिमाण(itf2class_T)
				+ ifcl->class_obj_member_गणनम् * परिमाण(पूर्णाङ्क));
    जइ (if2cl == रिक्त)
	प्रत्यागम FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = FALSE;

    प्रति (पूर्णाङ्क if_i = 0; if_i < ifcl->class_obj_member_गणनम्; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	प्रति (पूर्णाङ्क cl_i = 0; cl_i < cl->class_obj_member_गणनम्; ++cl_i)
	{
	    जइ (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		पूर्णाङ्क *table = (पूर्णाङ्क *)(if2cl + 1);
		table[if_i] = cl_i;
		भङ्ग;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(परिमाण(itf2class_T)
				+ ifcl->class_obj_method_गणनम् * परिमाण(पूर्णाङ्क));
    जइ (if2cl == रिक्त)
	प्रत्यागम FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = TRUE;

    प्रति (पूर्णाङ्क if_i = 0; if_i < ifcl->class_obj_method_गणनम्; ++if_i)
    {
	पूर्णाङ्क	done = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	प्रति (पूर्णाङ्क cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    जइ (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_data)[cl_i]->uf_name) == 0)
	    {
		पूर्णाङ्क *table = (पूर्णाङ्क *)(if2cl + 1);
		table[if_i] = cl_i;
		done = TRUE;
		भङ्ग;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	जइ (!done)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    जइ (cl->class_extends != ifcl)
	    {
		class_T		*parent = cl->class_extends;
		पूर्णाङ्क		method_offset = objmethods->ga_len;

		जाव (!done && parent != रिक्त && parent != ifcl)
		{
		    प्रति (पूर्णाङ्क cl_i = 0;
			    cl_i < parent->class_obj_method_गणनम्_child; ++cl_i)
		    {
			जइ (STRCMP(if_name,
				    parent->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    पूर्णाङ्क *table = (पूर्णाङ्क *)(if2cl + 1);
			    table[if_i] = method_offset + cl_i;
			    done = TRUE;
			    भङ्ग;
			}
		    }
		    method_offset += parent->class_obj_method_गणनम्_child;
		    parent = parent->class_extends;
		}
	    }

	    जइ (!done)
	    {
		पूर्णाङ्क *table = (पूर्णाङ्क *)(if2cl + 1);
		table[if_i] = pobj_method_offset + if_i;
	    }
	}
    }

    प्रत्यागम OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    स्थिर पूर्णाङ्क
add_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    प्रति (पूर्णाङ्क i = 0; i < cl->class_interface_गणनम्; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	जाव (ifcl != रिक्त)
	{
	    जइ (update_member_method_lookup_table(ifcl, cl, objmethods_gap,
								0) == FAIL)
		प्रत्यागम FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    जइ (extends_cl != रिक्त)
    {
	class_T		*pclass = extends_cl;
	पूर्णाङ्क		pobj_method_offset = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	जाव (pclass != रिक्त)
	{
	    जइ (update_member_method_lookup_table(pclass, cl,
			objmethods_gap, pobj_method_offset) == FAIL)
		प्रत्यागम FAIL;

	    pobj_method_offset += pclass->class_obj_method_गणनम्_child;
	    pclass = pclass->class_extends;
	}
    }

    प्रत्यागम OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    स्थिर पूर्णाङ्क
add_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_गणनम्);
    जइ (cl->class_members_tv == रिक्त)
	प्रत्यागम FAIL;

    प्रति (पूर्णाङ्क i = 0; i < cl->class_class_member_गणनम्; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	जइ (m->ocm_init != रिक्त)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_व्यञ्जकम्(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    जइ (etv == रिक्त)
		प्रत्यागम FAIL;

	    जइ (m->ocm_type->tt_type == VAR_ANY
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
	अण्णहा
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = रिक्त;
	}
	set_tv_type(tv, m->ocm_type);
	जइ (m->ocm_flags & OCMFLAG_CONST)
	    item_lock(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    प्रत्यागम OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    स्थिर शून्य
add_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    पूर्णाङ्क		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    प्रति (पूर्णाङ्क i = 0; i < cl->class_obj_member_गणनम्; ++i)
    {
	जइ (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    अनुवर्त;

	जइ (i > (is_enum ? 2 : 0))
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

    garray_T पङ्क्तयः_to_free;
    ga_init2(&पङ्क्तयः_to_free, परिमाण(char_u *), 50);

    ufunc_T *nf = define_function(&fea, रिक्त, &पङ्क्तयः_to_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_गणनम्,
			    रिक्त);

    ga_clear_strings(&पङ्क्तयः_to_free);
    vim_free(fga.ga_data);

    जइ (nf != रिक्त && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_data)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	जइ (nf->uf_ret_type != रिक्त)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_args = रिक्त;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    स्थिर पूर्णाङ्क
add_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    प्रति (पूर्णाङ्क loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	पूर्णाङ्क		*fcount = loop == 1 ? &cl->class_class_function_गणनम्
						: &cl->class_obj_method_गणनम्;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	पूर्णाङ्क parent_गणनम् = 0;
	जइ (extends_cl != रिक्त)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    parent_गणनम् = loop == 1
				? 0
				: extends_cl->class_obj_method_गणनम्;

	*fcount = parent_गणनम् + gap->ga_len;
	जइ (*fcount == 0)
	{
	    *fup = रिक्त;
	    अनुवर्त;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	जइ (*fup == रिक्त)
	    प्रत्यागम FAIL;

	जइ (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_data, परिमाण(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_data);
	जइ (loop == 1)
	    cl->class_class_function_गणनम्_child = gap->ga_len;
	अण्णहा
	    cl->class_obj_method_गणनम्_child = gap->ga_len;

	जइ (loop == 2)
	{
	    // Copy instance methods from the parent.

	    प्रति (पूर्णाङ्क i = 0; i < parent_गणनम्; ++i)
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
		जइ (nf != रिक्त && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_args != रिक्त)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		प्रति (पूर्णाङ्क ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    जइ (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (वर्ण *)pname;
			where.wt_kind = WT_METHOD;
			(शून्य)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	प्रति (पूर्णाङ्क i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    जइ (i < gap->ga_len)
		fp->uf_defclass = cl;
	    जइ (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    प्रत्यागम OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    स्थिर शून्य
update_builtin_method_index(class_T *cl)
{
    पूर्णाङ्क	i;

    प्रति (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    प्रति (i = 0; i < cl->class_obj_method_गणनम्; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	जइ (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	अण्णहा जइ (cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] = i;
	अण्णहा जइ (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    स्थिर char_u *
find_class_name_end(char_u *arg)
{
    char_u *end = arg;

    जाव (ASCII_ISALNUM(*end) || *end == '_'
	    || (*end == '.' && (ASCII_ISALNUM(end[1]) || end[1] == '_')))
	++end;

    प्रत्यागम end;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    स्थिर पूर्णाङ्क
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
    पूर्णाङ्क		dup = FALSE;

    प्रति (पूर्णाङ्क i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_data) + i;

	जइ (name.length == m->ocm_name.length
	    && STRNCMP(name.string, m->ocm_name.string, name.length) == 0)
	{
	    char_u  save_c = *varname_end;

	    *varname_end = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_end = save_c;
	    dup = TRUE;
	    भङ्ग;
	}
    }

    प्रत्यागम dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    स्थिर पूर्णाङ्क
enum_parse_मानानि(
    exarg_T	*eap,
    class_T	*en,
    char_u	*पङ्क्तिः,
    garray_T	*gap,
    पूर्णाङ्क		*अङ्कः_enum_मानानि,
    पूर्णाङ्क		*enum_end)
{
    evalarg_T	evalarg;
    char_u	*p = पङ्क्तिः;
    वर्ण	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	last_char = NUL;
    पूर्णाङ्क		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    पूर्णाङ्क		did_emsg_before = did_emsg;
    जाव (*p != NUL)
    {
	// ignore comment
	जइ (*p == '#')
	    भङ्ग;

	जइ (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_मानम्_declaration_str), p);
	    भङ्ग;
	}

	char_u *eni_name_start = p;
	char_u *eni_name_end = to_name_end(p, FALSE);

	जइ (is_duplicate_enum(gap, eni_name_start, eni_name_end))
	    भङ्ग;

	p = skipwhite(eni_name_end);

	char_u	*init_व्यञ्जकम् = रिक्त;
	जइ (*p == '(' || *p == '<')
	{
	    जइ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", पङ्क्तिः);
		भङ्ग;
	    }

	    char_u *व्यञ्जकम्_start, *व्यञ्जकम्_end;

	    p = eni_name_start;
	    (शून्य)skip_व्यञ्जकम्_concatenate(&p, &व्यञ्जकम्_start, &व्यञ्जकम्_end, &evalarg);

	    जाव (*व्यञ्जकम्_start && *व्यञ्जकम्_start != '(' && *व्यञ्जकम्_start != '<')
		व्यञ्जकम्_start++;

	    जइ (व्यञ्जकम्_end > व्यञ्जकम्_start)
		init_व्यञ्जकम् = vim_strnsave(व्यञ्जकम्_start, व्यञ्जकम्_end - व्यञ्जकम्_start);
	}

	जइ (init_व्यञ्जकम् == रिक्त)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, परिमाण(initexpr_buf), "%s.new()", en->class_name.string);
	अण्णहा
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, परिमाण(initexpr_buf), "%s.new%s", en->class_name.string,
		init_व्यञ्जकम्);
	    vim_free(init_व्यञ्जकम्);
	}
	जइ (add_member(gap, eni_name_start, eni_name_end, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    भङ्ग;

	++*अङ्कः_enum_मानानि;

	जइ (*p != '#')
	    last_char = *p;

	जइ (*p != NUL && *p != ',')
	    भङ्ग;

	जइ (*p == ',')
	{
	    जइ (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", पङ्क्तिः);
		भङ्ग;
	    }
	    जइ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", पङ्क्तिः);
		भङ्ग;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    जइ (*p != NUL && *p != '#')
    {
	जइ (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_argument_str), p);
	rc = FAIL;
    }

    जइ (last_char != ',')
	// last enum value should not be terminated by ","
	*enum_end = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, रिक्त);

    प्रत्यागम rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    स्थिर पूर्णाङ्क
enum_add_मानानि_member(
    class_T	*en,
    garray_T	*gap,
    पूर्णाङ्क		अङ्कः_enum_मानानि,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    पूर्णाङ्क		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    प्रति (पूर्णाङ्क i = 0; i < अङ्कः_enum_मानानि; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;

	जइ (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.length);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    जइ (type == रिक्त)
	गच्छ done;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    जइ (type->tt_member != रिक्त)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = add_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_data, fga.ga_len));

done:
    vim_free(fga.ga_data);

    प्रत्यागम rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    स्थिर शून्य
enum_clear_constructors(class_T *en)
{
    प्रति (पूर्णाङ्क i = 0; i < en->class_class_function_गणनम्; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	जइ (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    शून्य
enum_set_internal_obj_vars(class_T *en, object_T *enval)
{
    पूर्णाङ्क	i;

    प्रति (i = 0; i < en->class_class_member_गणनम्; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	जइ (en_tv != रिक्त && en_tv->v_type == VAR_UNKNOWN)
	    भङ्ग;
    }

    जइ (i >= en->class_class_member_गणनम्)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	प्रत्यागम;

    // First object variable is the name
    ocmember_T *मानम्_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(मानम्_ocm->ocm_name.string, मानम्_ocm->ocm_name.length);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_सङ्ख्या;
    ord_tv->vval.v_सङ्ख्या = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    स्थिर शून्य
skip_class_body(exarg_T *eap)
{
    प्रति (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	जइ (theline == रिक्त)
	    भङ्ग;

	char_u	*p = theline;
	पूर्णाङ्क	found;

	// skip ':' and blanks
	प्रति (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	found = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	जइ (found)
	    भङ्ग;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    शून्य
ex_class(exarg_T *eap)
{
    पूर्णाङ्क		is_class = eap->cmdidx == CMD_class;
    पूर्णाङ्क		is_abstract = eap->cmdidx == CMD_abstract;
    पूर्णाङ्क		is_enum = eap->cmdidx == CMD_enum;
    पूर्णाङ्क		added_enum_मानानि = FALSE;
    पूर्णाङ्क		is_interface;
    long	start_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    जइ (eap->skip)
    {
	skip_class_body(eap);
	प्रत्यागम;
    }

    जइ (is_abstract)
    {
	जइ (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_argument_str), arg);
	    प्रत्यागम;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    जइ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	जइ (is_class)
	    emsg(_(e_class_can_only_be_defined_in_vim9_script));
	अण्णहा जइ (is_enum)
	    emsg(_(e_enum_can_only_be_defined_in_vim9_script));
	अण्णहा
	    emsg(_(e_interface_can_only_be_defined_in_vim9_script));
	प्रत्यागम;
    }

    जइ (!ASCII_ISUPPER(*arg))
    {
	जइ (is_class)
	    semsg(_(e_class_name_must_start_with_uppercase_letter_str), arg);
	अण्णहा जइ (is_enum)
	    semsg(_(e_enum_name_must_start_with_uppercase_letter_str), arg);
	अण्णहा
	    semsg(_(e_interface_name_must_start_with_uppercase_letter_str),
									  arg);
	प्रत्यागम;
    }
    char_u *name_end = find_name_end(arg, रिक्त, रिक्त, FNE_CHECK_START);
    जइ (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	प्रत्यागम;
    }
    char_u *name_start = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = रिक्त;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, परिमाण(char_u *), 5);

    arg = skipwhite(name_end);
    जाव (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	जइ (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    जइ (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		गच्छ early_ret;
	    }
	    जइ (extends != रिक्त)
	    {
		emsg(_(e_duplicate_extends));
		गच्छ early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *end = find_class_name_end(arg);
	    जइ (!IS_WHITE_OR_NUL(*end))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		गच्छ early_ret;
	    }
	    extends = vim_strnsave(arg, end - arg);
	    जइ (extends == रिक्त)
		गच्छ early_ret;

	    arg = skipwhite(end + 1);
	}
	अण्णहा जइ (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    जइ (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		गच्छ early_ret;
	    }

	    जइ (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		गच्छ early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    प्रति (;;)
	    {
		char_u *impl_end = find_class_name_end(arg);
		जइ ((!IS_WHITE_OR_NUL(*impl_end) && *impl_end != ',')
			|| (*impl_end == ','
			    && !IS_WHITE_OR_NUL(*(impl_end + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    गच्छ early_ret;
		}
		जइ (impl_end - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    गच्छ early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_end - arg);
		जइ (iname == रिक्त)
		    गच्छ early_ret;
		प्रति (पूर्णाङ्क i = 0; i < ga_impl.ga_len; ++i)
		    जइ (STRCMP(((char_u **)ga_impl.ga_data)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			गच्छ early_ret;
		    }
		जइ (ga_add_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    गच्छ early_ret;
		}
		जइ (*impl_end != ',')
		{
		    arg = skipwhite(impl_end);
		    भङ्ग;
		}
		arg = skipwhite(impl_end + 1);
	    }
	}
	अण्णहा
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_strings(&ga_impl);
	    प्रत्यागम;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, परिमाण(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, परिमाण(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, परिमाण(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, परिमाण(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, परिमाण(ufunc_T *), 10);

    class_T *cl = रिक्त;
    class_T *extends_cl = रिक्त;  // class from "extends" argument
    class_T **intf_classes = रिक्त;
    पूर्णाङ्क	    intf_गणनम् = 0;	// number of items in "intf_classes"
    पूर्णाङ्क	    अङ्कः_enum_मानानि = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    जइ (cl == रिक्त)
	गच्छ cleanup;

    जइ (is_enum)
	cl->class_flags = CLASS_ENUM;
    अण्णहा जइ (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    अण्णहा जइ (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.length = (size_t)(name_end - name_start);
    cl->class_name.string = vim_strnsave(name_start, cl->class_name.length);
    जइ (cl->class_name.string == रिक्त)
	गच्छ cleanup;

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
    पूर्णाङ्क rc = set_var_const(cl->class_name.string, 0, रिक्त, &tv, FALSE, 0, 0);
    जइ (rc == FAIL)
	गच्छ cleanup;

    जइ (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	जइ (add_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, रिक्त) == FAIL)
	    गच्छ cleanup;

	varname = (char_u *)"ordinal";
	जइ (add_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_सङ्ख्या, रिक्त) == FAIL)
	    गच्छ cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    पूर्णाङ्क		enum_end = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = रिक्त;
    पूर्णाङ्क success = FALSE;
    प्रति (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	जइ (theline == रिक्त)
	    भङ्ग;
	char_u *पङ्क्तिः = skipwhite(theline);

	// Skip empty and comment lines.
	जइ (*पङ्क्तिः == NUL)
	    अनुवर्त;
	जइ (*पङ्क्तिः == '#')
	{
	    जइ (vim9_bad_comment(पङ्क्तिः))
		भङ्ग;
	    अनुवर्त;
	}

	char_u *p = पङ्क्तिः;

	वर्ण	*end_name;
	पूर्णाङ्क	shortlen;
	पूर्णाङ्क	fullen;
	जइ (is_class)
	{
	    end_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	अण्णहा जइ (is_enum)
	{
	    end_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	अण्णहा
	{
	    end_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	प्रति (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_start = p;
	जइ (checkforcmd(&p, end_name, shortlen))
	{
	    जइ (STRNCMP(cmd_start, end_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_start);
	    अण्णहा जइ (*p == '|' || !ends_excmd2(पङ्क्तिः, p))
		semsg(_(e_trailing_characters_str), p);
	    अण्णहा
		success = TRUE;
	    भङ्ग;
	}

	पूर्णाङ्क	wrong_endname = FALSE;
	जइ (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	अण्णहा जइ (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	अण्णहा
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	जइ (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), पङ्क्तिः, end_name);
	    भङ्ग;
	}

	जइ (is_enum && !enum_end)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    जइ (enum_parse_मानानि(eap, cl, पङ्क्तिः, &classmembers,
					&अङ्कः_enum_मानानि, &enum_end) == FAIL)
		भङ्ग;

	    जइ (enum_end)
	    {
		// Add the enum "values" class variable.
		enum_add_मानानि_member(cl, &classmembers, अङ्कः_enum_मानानि,
							&type_list);
		added_enum_मानानि = TRUE;
	    }
	    अनुवर्त;
	}

	पूर्णाङ्क has_public = FALSE;
	जइ (checkforcmd(&p, "public", 3))
	{
	    जइ (STRNCMP(पङ्क्तिः, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), पङ्क्तिः);
		भङ्ग;
	    }
	    जइ (is_interface)
	    {
		emsg(_(e_public_variable_not_supported_in_interface));
		भङ्ग;
	    }
	    has_public = TRUE;
	    p = skipwhite(पङ्क्तिः + 6);

	    जइ (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_public_keyword_not_supported_for_method));
		भङ्ग;
	    }

	    जइ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_public_must_be_followed_by_var_static_final_or_const));
		भङ्ग;
	    }
	}

	पूर्णाङ्क abstract_method = FALSE;
	char_u *pa = p;
	जइ (checkforcmd(&p, "abstract", 3))
	{
	    जइ (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		भङ्ग;
	    }

	    जइ (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_used_in_enum));
		भङ्ग;
	    }

	    जइ (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_used_in_interface));
		भङ्ग;
	    }

	    जइ (!is_abstract)
	    {
		semsg(_(e_abstract_method_in_concrete_class), pa);
		भङ्ग;
	    }

	    p = skipwhite(pa + 8);
	    जइ (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		भङ्ग;
	    }

	    abstract_method = TRUE;
	}

	पूर्णाङ्क has_static = FALSE;
	char_u *ps = p;
	जइ (checkforcmd(&p, "static", 4))
	{
	    जइ (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		भङ्ग;
	    }

	    जइ (is_interface)
	    {
		emsg(_(e_static_member_not_supported_in_interface));
		भङ्ग;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    जइ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		भङ्ग;
	    }
	}

	पूर्णाङ्क has_final = FALSE;
	पूर्णाङ्क has_var = FALSE;
	पूर्णाङ्क has_const = FALSE;
	जइ (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	अण्णहा जइ (checkforcmd(&p, "final", 5))
	{
	    जइ (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_in_interface));
		भङ्ग;
	    }
	    has_final = TRUE;
	}
	अण्णहा जइ (checkforcmd(&p, "const", 5))
	{
	    जइ (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_in_interface));
		भङ्ग;
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
	जइ (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_end = रिक्त;
	    type_T *type = रिक्त;
	    char_u *init_व्यञ्जकम् = रिक्त;
	    पूर्णाङ्क	    has_type = FALSE;

	    जइ (!eval_isnamec1(*p))
	    {
		जइ (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), पङ्क्तिः);
		अण्णहा
		    semsg(_(e_invalid_object_variable_declaration_str), पङ्क्तिः);
		भङ्ग;
	    }

	    जइ (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_in_interface),
			varname);
		भङ्ग;
	    }

	    जइ (parse_member(eap, पङ्क्तिः, varname, has_public,
			  &varname_end, &has_type, &type_list, &type,
			  !is_interface ? &init_व्यञ्जकम्: रिक्त) == FAIL)
		भङ्ग;

	    जइ (is_reserved_varname(varname, varname_end)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_end))
	    {
		vim_free(init_व्यञ्जकम्);
		भङ्ग;
	    }
	    जइ (add_member(has_static ? &classmembers : &objmembers, varname,
				varname_end, has_public, has_final, has_const,
				has_type, type, init_व्यञ्जकम्) == FAIL)
	    {
		vim_free(init_व्यञ्जकम्);
		भङ्ग;
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
	अण्णहा जइ (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	पङ्क्तयः_to_free;
	    पूर्णाङ्क		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    जइ (has_public)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_public_keyword_not_supported_for_method));
		भङ्ग;
	    }

	    जइ (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_in_class_str), पङ्क्तिः);
		भङ्ग;
	    }

	    जइ (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_method_not_supported_in_interface), p);
		भङ्ग;
	    }

	    जइ (has_static && !is_new && SAFE_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_method_not_supported), p);
		भङ्ग;
	    }

	    CLEAR_FIELD(ea);
	    ea.cmd = पङ्क्तिः;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&पङ्क्तयः_to_free, परिमाण(char_u *), 50);
	    पूर्णाङ्क class_flags;
	    जइ (is_interface)
		class_flags = CF_INTERFACE;
	    अण्णहा
		class_flags = abstract_method ? CF_ABSTRACT_METHOD : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, रिक्त, &पङ्क्तयः_to_free,
			class_flags, objmembers.ga_data, objmembers.ga_len,
			रिक्त);
	    ga_clear_strings(&पङ्क्तयः_to_free);

	    जइ (uf != रिक्त)
	    {
		जइ (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_method_not_supported_in_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    भङ्ग;
		}

		// check for builtin method
		जइ (!is_new && SAFE_islower(*uf->uf_name) &&
					!object_check_builtin_method_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    भङ्ग;
		}

		// Check the uf_name isn't used already.
		जइ (is_duplicate_method(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    भङ्ग;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		जइ (ga_grow(fgap, 1) == OK)
		{
		    जइ (is_new)
			uf->uf_flags |= FC_NEW;

		    जइ (abstract_method)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_data)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	अण्णहा
	{
	    जइ (is_class)
		semsg(_(e_not_valid_command_in_class_str), पङ्क्तिः);
	    अण्णहा जइ (is_enum)
		semsg(_(e_not_valid_command_in_enum_str), पङ्क्तिः);
	    अण्णहा
		semsg(_(e_not_valid_command_in_interface_str), पङ्क्तिः);
	    भङ्ग;
	}
    }

    जइ (theline == रिक्त && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    जइ (success && is_enum && (अङ्कः_enum_मानानि == 0 || !added_enum_मानानि))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_add_मानानि_member(cl, &classmembers, अङ्कः_enum_मानानि,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    जइ (success && extends != रिक्त)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    जइ (success && extends_cl != रिक्त)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    जइ (success && extends_cl != रिक्त)
    {
	जइ (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	अण्णहा
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    जइ (success && !is_abstract && extends_cl != रिक्त
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, परिमाण(class_T *), 5);

    जइ (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    जइ (success && extends_cl != रिक्त)
	success = add_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_data;
    intf_गणनम् = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    जइ (success)
	success = check_func_arg_names(&classfunctions, &objmethods,
							&classmembers);

    जइ (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	जइ (extends_cl != रिक्त)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	जइ (add_members_to_class(&classmembers,
				 रिक्त,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_गणनम्) == FAIL
		|| add_members_to_class(&objmembers,
				 extends_cl == रिक्त ? रिक्त
					       : extends_cl->class_obj_members,
				 extends_cl == रिक्त ? 0
					  : extends_cl->class_obj_member_गणनम्,
				 &cl->class_obj_members,
				 &cl->class_obj_member_गणनम्) == FAIL)
	    गच्छ cleanup;

	जइ (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_गणनम् = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    जइ (cl->class_interfaces == रिक्त)
		गच्छ cleanup;
	    प्रति (पूर्णाङ्क i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_data)[i];
	    VIM_CLEAR(ga_impl.ga_data);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = रिक्त;
	}

	जइ (cl->class_interface_गणनम् > 0 || extends_cl != रिक्त)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    जइ (add_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		गच्छ cleanup;
	}

	पूर्णाङ्क	have_new = FALSE;
	ufunc_T	*class_func = रिक्त;
	प्रति (पूर्णाङ्क i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_data)[i];
	    जइ (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		भङ्ग;
	    }
	}

	जइ (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	अण्णहा जइ ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    add_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	जइ (add_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    गच्छ cleanup;

	update_builtin_method_index(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	जइ ((is_class || is_enum) && cl->class_class_member_गणनम् > 0)
	    जइ (add_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		प्रत्यागम;
	    }

	cl->class_type_list = type_list;

	जइ (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	प्रत्यागम;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    जइ (intf_classes != रिक्त)
    {
	प्रति (पूर्णाङ्क i = 0; i < intf_गणनम्; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_strings(&ga_impl);

    प्रति (पूर्णाङ्क round = 1; round <= 2; ++round)
    {
	garray_T *gap = round == 1 ? &classmembers : &objmembers;
	जइ (gap->ga_len == 0 || gap->ga_data == रिक्त)
	    अनुवर्त;

	प्रति (पूर्णाङ्क i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    प्रति (पूर्णाङ्क i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    प्रति (पूर्णाङ्क i = 0; i < classfunctions.ga_len; ++i)
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
    पूर्णाङ्क		is_object,
    char_u	*name,
    char_u	*name_end,
    पूर्णाङ्क		*member_idx)
{
    size_t	len = name_end - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    जइ (m == रिक्त)
    {
	member_not_found_सन्देशः(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	प्रत्यागम &t_any;
    }

    प्रत्यागम m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    पूर्णाङ्क		is_object,
    पूर्णाङ्क		member_idx)
{
    ocmember_T	*m;
    पूर्णाङ्क		member_गणनम्;

    जइ (is_object)
    {
	m = cl->class_obj_members;
	member_गणनम् = cl->class_obj_member_गणनम्;
    }
    अण्णहा
    {
	m = cl->class_class_members;
	member_गणनम् = cl->class_class_member_गणनम्;
    }

    जइ (member_idx >= member_गणनम्)
	प्रत्यागम रिक्त;

    प्रत्यागम m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    स्थिर शून्य
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    शून्य
typealias_unref(typealias_T *ta)
{
    जइ (ta != रिक्त && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    शून्य
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    जइ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_in_vim9_script));
	प्रत्यागम;
    }

    जइ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	प्रत्यागम;
    }

    जइ (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_start_with_uppercase_letter_str), arg);
	प्रत्यागम;
    }

    char_u *name_end = find_name_end(arg, रिक्त, रिक्त, FNE_CHECK_START);
    जइ (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	प्रत्यागम;
    }
    char_u *name_start = arg;

    arg = skipwhite(name_end);
    जइ (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	प्रत्यागम;
    }
    जइ (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	प्रत्यागम;
    }
    arg++;
    arg = skipwhite(arg);

    जइ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	प्रत्यागम;
    }

    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, रिक्त, रिक्त, TRUE);
    जइ (type == रिक्त)
	प्रत्यागम;

    जइ (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	प्रत्यागम;
    }

    पूर्णाङ्क cc = *name_end;
    *name_end = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    जइ (eval_variable_import(name_start, &tv) == OK)
    {
	जइ (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_start);
	अण्णहा
	    semsg(_(e_redefining_script_item_str), name_start);
	clear_tv(&tv);
	गच्छ done;
    }

    // Create a script-local variable for the type alias.
    जइ (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_lock = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_start);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_depth = parse_type_depth();
    }
    अण्णहा
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_lock = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    set_var_const(name_start, current_sctx.sc_sid, रिक्त, &tv, FALSE,
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
    पूर्णाङ्क
get_member_tv(
    class_T	*cl,
    पूर्णाङ्क		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    पूर्णाङ्क		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    जइ (m == रिक्त)
	प्रत्यागम FAIL;

    जइ (*name == '_' && (current_class == रिक्त ||
				!class_instance_of(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	प्रत्यागम FAIL;
    }

    जइ (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	set_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    अण्णहा
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	set_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    प्रत्यागम OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    स्थिर पूर्णाङ्क
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
    पूर्णाङ्क		argcount = 0;
    ocmember_T	*ocm = रिक्त;
    पूर्णाङ्क		m_idx;

    fp = method_lookup(cl, rettv->v_type, name, len, रिक्त);
    जइ (fp == रिक्त)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	जइ (ocm == रिक्त || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    method_not_found_सन्देशः(cl, rettv->v_type, name, len);
	    प्रत्यागम FAIL;
	}

	जइ (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    प्रत्यागम FAIL;
	}

	जइ (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	अण्णहा
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_end;
	प्रत्यागम OK;
    }

    जइ (ocm == रिक्त && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
	प्रत्यागम FAIL;
    }

    // process generic function call
    जइ (fp != रिक्त)
    {
	fp = generic_func_get(fp, gfatab);
	जइ (fp == रिक्त)
	    प्रत्यागम FAIL;
    }

    char_u *argp = name_end;
    पूर्णाङ्क ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    जइ (ret == FAIL)
    {
	जाव (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	प्रत्यागम FAIL;
    }

    funcexe_T funcexe;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    जइ (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    जइ (evalarg != रिक्त)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    पूर्णाङ्क error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								रिक्त);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    प्रति (पूर्णाङ्क idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    जइ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_found_var);
	प्रत्यागम FAIL;
    }
    *arg = argp;

    प्रत्यागम OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    पूर्णाङ्क
obj_method_to_partial_tv(object_T *obj, ufunc_T *obj_method, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    जइ (pt == रिक्त)
	प्रत्यागम FAIL;

    pt->pt_refcount = 1;
    जइ (obj != रिक्त)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_method;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    प्रत्यागम OK;
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
    पूर्णाङ्क
class_object_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    पूर्णाङ्क		verbose UNUSED)	// give error messages
{
    पूर्णाङ्क	ret = FAIL;

    जइ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	प्रत्यागम FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, रिक्त, रिक्त, FNE_CHECK_START);
    जइ (name_end == name)
	प्रत्यागम FAIL;
    size_t len = name_end - name;

    पूर्णाङ्क did_emsg_save = did_emsg;
    class_T *cl;
    जइ (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    अण्णहा // VAR_OBJECT
    {
	जइ (rettv->vval.v_object == रिक्त)
	{
	    emsg(_(e_using_null_object));
	    प्रत्यागम FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    जइ (cl == रिक्त)
    {
	emsg(_(e_incomplete_type));
	प्रत्यागम FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    जइ (*name_end == '<')
    {
	cctx_T	    *cctx = रिक्त;

	जइ (evalarg != रिक्त)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_end = parse_generic_func_type_args(name, len, name + len,
						&gfatab, cctx);
	जइ (name_end == रिक्त)
	    गच्छ done;
    }

    जइ (*name_end == '(')
	// Invoke the class or object method
	ret = call_oc_method(cl, name, len, name_end, evalarg, arg,
							&gfatab, rettv);
    अण्णहा जइ (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	पूर्णाङ्क is_object = rettv->v_type == VAR_OBJECT;
	जइ (get_member_tv(cl, is_object, name, len, रिक्त, rettv) == OK)
	{
	    *arg = name_end;
	    ret = OK;
	    गच्छ done;
	}

	// could be a class method or an object method
	पूर्णाङ्क	fidx;
	ufunc_T	*fp = method_lookup(cl, rettv->v_type, name, len, &fidx);
	जइ (fp != रिक्त)
	{
	    // Protected methods are not accessible outside the class
	    जइ (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		गच्छ done;
	    }

	    जइ (obj_method_to_partial_tv(is_object ? rettv->vval.v_object :
						रिक्त, fp, rettv) == FAIL)
		गच्छ done;

	    *arg = name_end;
	    ret = OK;
	    गच्छ done;
	}

	जइ (did_emsg == did_emsg_save)
	    member_not_found_सन्देशः(cl, rettv->v_type, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);

    प्रत्यागम ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
find_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, रिक्त, रिक्त, FNE_CHECK_START);
    जइ (name_end == name || *name_end != '.')
	प्रत्यागम रिक्त;

    ufunc_T	*fp = रिक्त;
    size_t	len = name_end - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    जइ (eval_variable(name, (पूर्णाङ्क)len,
				    0, &tv, रिक्त, EVAL_VAR_NOAUTOLOAD) == FAIL)
	प्रत्यागम रिक्त;
    जइ (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	गच्छ fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    जइ (cl == रिक्त)
	गच्छ fail_after_eval;
    char_u *fname = name_end + 1;
    char_u *fname_end = find_name_end(fname, रिक्त, रिक्त, FNE_CHECK_START);
    जइ (fname_end == fname)
	गच्छ fail_after_eval;
    len = fname_end - fname;

    fp = method_lookup(cl, tv.v_type, fname, len, रिक्त);

    जइ (fp != रिक्त)
    {
	fp = eval_generic_func(fp, fname, &fname_end);
	जइ (fp != रिक्त)
	    *arg = fname_end;
    }

fail_after_eval:
    clear_tv(&tv);
    प्रत्यागम fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    पूर्णाङ्क
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    पूर्णाङ्क idx;
    class_member_lookup(cl, name, namelen, &idx);
    प्रत्यागम idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, पूर्णाङ्क *idx)
{
    ocmember_T	*ret_m = रिक्त;
    पूर्णाङ्क		ret_idx = -1;
    प्रति (पूर्णाङ्क i = 0; i < cl->class_class_member_गणनम्; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	जइ (namelen)
	{
	    जइ (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		भङ्ग;
	    }
	}
	अण्णहा जइ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    भङ्ग;
	}
    }
    जइ (idx != रिक्त)
	*idx = ret_idx;
    प्रत्यागम ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    स्थिर ufunc_T *
class_method_lookup(class_T *cl, char_u *name, size_t namelen, पूर्णाङ्क *idx)
{
    ufunc_T	*ret_fp = रिक्त;
    पूर्णाङ्क		ret_idx = -1;
    प्रति (पूर्णाङ्क i = 0; i < cl->class_class_function_गणनम्; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	जइ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    भङ्ग;
	}
    }
    जइ (idx != रिक्त)
	*idx = ret_idx;
    प्रत्यागम ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    पूर्णाङ्क
class_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    पूर्णाङ्क idx;
    class_method_lookup(cl, name, namelen, &idx);
    प्रत्यागम idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    स्थिर पूर्णाङ्क
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    पूर्णाङ्क idx;
    object_member_lookup(cl, name, namelen, &idx);
    प्रत्यागम idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, पूर्णाङ्क *idx)
{
    ocmember_T	*ret_m = रिक्त;
    पूर्णाङ्क		ret_idx = -1;
    प्रति (पूर्णाङ्क i = 0; i < cl->class_obj_member_गणनम्; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	जइ (namelen)
	{
	    जइ (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		भङ्ग;
	    }
	}
	अण्णहा जइ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    भङ्ग;
	}
    }
    जइ (idx != रिक्त)
	*idx = ret_idx;
    प्रत्यागम ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    स्थिर ufunc_T *
object_method_lookup(class_T *cl, char_u *name, size_t namelen, पूर्णाङ्क *idx)
{
    ufunc_T	*ret_fp = रिक्त;
    पूर्णाङ्क		ret_idx = -1;
    प्रति (पूर्णाङ्क i = 0; i < cl->class_obj_method_गणनम्; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	जइ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    भङ्ग;
	}
    }
    जइ (idx != रिक्त)
	*idx = ret_idx;
    प्रत्यागम ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    पूर्णाङ्क
object_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    पूर्णाङ्क idx;
    object_method_lookup(cl, name, namelen, &idx);
    प्रत्यागम idx;
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
    पूर्णाङ्क		*idx)
{
    जइ (v_type == VAR_CLASS)
	प्रत्यागम class_member_lookup(cl, name, namelen, idx);

    प्रत्यागम object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    स्थिर class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_found = रिक्त;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_found = रिक्त;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    प्रति (class_T *super = cl; super != रिक्त; super = super->class_extends)
    {
	class_T		*cl_tmp = रिक्त;
	ocmember_T	*m = रिक्त;
	जइ (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    जइ ((m = object_member_lookup(super, name, len, रिक्त)) != रिक्त)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	जइ (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    जइ (( m = class_member_lookup(super, name, len, रिक्त)) != रिक्त)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	जइ (cl_tmp == रिक्त)
	    भङ्ग;  // member is not in this or any super class.
	cl_found = cl_tmp;
	m_found = m;
    }
    जइ (p_m != रिक्त)
	*p_m = m_found;
    प्रत्यागम cl_found;
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
    पूर्णाङ्क		*idx)
{
    जइ (v_type == VAR_CLASS)
	प्रत्यागम class_method_lookup(cl, name, namelen, idx);
    अण्णहा
	प्रत्यागम object_method_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    पूर्णाङ्क
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    प्रति (cctx_T *cctx = cctx_arg; cctx != रिक्त; cctx = cctx->ctx_outer)
	जइ (cctx->ctx_ufunc != रिक्त
			&& class_instance_of(cctx->ctx_ufunc->uf_class, cl))
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    पूर्णाङ्क
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    जइ (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_in_class_str),
		m->ocm_name.string, cl->class_name.string);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    शून्य
obj_lock_const_vars(object_T *obj)
{
    प्रति (पूर्णाङ्क i = 0; i < obj->obj_class->class_obj_member_गणनम्; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	जइ (ocm->ocm_flags & OCMFLAG_CONST)
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
    पूर्णाङ्क		sz;

    जइ (cl == रिक्त)
	प्रत्यागम रिक्त;

    sz = परिमाण(object_T) + cl->class_obj_member_गणनम् * परिमाण(typval_T);
    obj = alloc_clear(sz);
    जइ (obj == रिक्त)
	प्रत्यागम रिक्त;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    जइ (IS_ENUM(cl))
	enum_set_internal_obj_vars(cl, obj);

    प्रत्यागम obj;
}

/*
 * Make a copy of an object.
 */
    शून्य
copy_object(typval_T *from, typval_T *to)
{
    जइ (from->vval.v_object == रिक्त)
	to->vval.v_object = रिक्त;
    अण्णहा
    {
	to->vval.v_object = from->vval.v_object;
	++to->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    शून्य
copy_class(typval_T *from, typval_T *to)
{
    जइ (from->vval.v_class == रिक्त)
	to->vval.v_class = रिक्त;
    अण्णहा
    {
	to->vval.v_class = from->vval.v_class;
	++to->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    स्थिर शून्य
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    प्रति (पूर्णाङ्क i = 0; i < cl->class_interface_गणनम्; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	जइ (cl->class_interfaces_cl[i] != रिक्त)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *next;
    प्रति (itf2class_T *i2c = cl->class_itf2class; i2c != रिक्त; i2c = next)
    {
	next = i2c->i2c_next;
	vim_free(i2c);
    }

    प्रति (पूर्णाङ्क i = 0; i < cl->class_class_member_गणनम्; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	जइ (cl->class_members_tv != रिक्त)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    प्रति (पूर्णाङ्क i = 0; i < cl->class_obj_member_गणनम्; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    प्रति (पूर्णाङ्क i = 0; i < cl->class_class_function_गणनम्; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	जइ (IS_ENUM(cl) && IS_CONSTRUCTOR_METHOD(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    प्रति (पूर्णाङ्क i = 0; i < cl->class_obj_method_गणनम्; ++i)
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
    स्थिर पूर्णाङ्क
class_get_selfrefs(class_T *cl)
{
    पूर्णाङ्क		self_refs = 0;
    typval_T	*tv;

    प्रति (पूर्णाङ्क i = 0; i < cl->class_class_member_गणनम्; ++i)
    {
	tv = &cl->class_members_tv[i];
	जइ (tv->v_type == VAR_OBJECT && tv->vval.v_object != रिक्त
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    प्रत्यागम self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    स्थिर पूर्णाङ्क
can_free_enum(class_T *cl)
{
    प्रति (पूर्णाङ्क i = 0; i < cl->class_class_member_गणनम्; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	जइ (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    प्रत्यागम TRUE;

	जइ (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    जइ (tv->vval.v_list->lv_refcount > 1)
		प्रत्यागम FALSE;
	    भङ्ग;
	}

	जइ (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    प्रत्यागम FALSE;
    }

    प्रत्यागम TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    स्थिर पूर्णाङ्क
can_free_class(class_T *cl)
{
    पूर्णाङ्क		can_free = FALSE;
    पूर्णाङ्क		self_refs = 0;

    जइ (IS_ENUM(cl) && !can_free_enum(cl))
	प्रत्यागम FALSE;

    जइ (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    जइ (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    प्रत्यागम can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    शून्य
class_unref(class_T *cl)
{
    जइ (cl == रिक्त)
	प्रत्यागम;

    --cl->class_refcount;

    जइ (cl->class_name.string == रिक्त)
	प्रत्यागम;

    जइ (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    पूर्णाङ्क
class_free_nonref(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		did_free = FALSE;

    प्रति (class_T *cl = first_class; cl != रिक्त; cl = next_nonref_class)
    {
	next_nonref_class = cl->class_next_used;
	जइ ((cl->class_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    next_nonref_class = रिक्त;
    प्रत्यागम did_free;
}

    पूर्णाङ्क
set_ref_in_classes(पूर्णाङ्क copyID)
{
    प्रति (class_T *cl = first_class; cl != रिक्त; cl = cl->class_next_used)
	set_ref_in_item_class(cl, copyID, रिक्त, रिक्त, रिक्त);

    प्रत्यागम FALSE;
}

स्थिर object_T *first_object = रिक्त;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    शून्य
object_created(object_T *obj)
{
    जइ (first_object != रिक्त)
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
    स्थिर शून्य
object_cleared(object_T *obj)
{
    जइ (obj->obj_next_used != रिक्त)
	obj->obj_next_used->obj_prev_used = obj->obj_prev_used;
    जइ (obj->obj_prev_used != रिक्त)
	obj->obj_prev_used->obj_next_used = obj->obj_next_used;
    अण्णहा जइ (first_object == obj)
	first_object = obj->obj_next_used;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    स्थिर शून्य
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    जइ (!cl)
	प्रत्यागम;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = INT_MAX;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    प्रति (पूर्णाङ्क i = 0; i < cl->class_obj_member_गणनम्; ++i)
	clear_tv(tv + i);
}

    स्थिर शून्य
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    जइ (!cl)
	प्रत्यागम;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    स्थिर शून्य
object_free(object_T *obj)
{
    जइ (in_free_unref_वस्तूनि)
	प्रत्यागम;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    शून्य
object_unref(object_T *obj)
{
    जइ (obj != रिक्त && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    पूर्णाङ्क
object_free_nonref(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		did_free = FALSE;

    प्रति (object_T *obj = first_object; obj != रिक्त; obj = obj->obj_next_used)
    {
	जइ ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    प्रत्यागम did_free;
}

    शून्य
object_free_वस्तूनि(पूर्णाङ्क copyID)
{
    object_T	*obj_next;

    प्रति (object_T *obj = first_object; obj != रिक्त; obj = obj_next)
    {
	obj_next = obj->obj_next_used;
	जइ ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    शून्य
emsg_var_cl_define(वर्ण *सन्देशः, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    जइ (cl_def != रिक्त)
	semsg(_(सन्देशः), m->ocm_name.string, cl_def->class_name.string);
    अण्णहा
	emsg(_(e_internal_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    शून्य
method_not_found_सन्देशः(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *method_name = vim_strnsave(name, len);
    जइ ((v_type == VAR_OBJECT)
	    && (class_method_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	जइ (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	अण्णहा
	    semsg(_(e_class_method_str_accessible_only_using_class_str),
		    method_name, cl->class_name.string);
    }
    अण्णहा जइ ((v_type == VAR_CLASS)
	    && (object_method_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	जइ (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	अण्णहा
	    semsg(_(e_object_method_str_accessible_only_using_object_str),
		    method_name, cl->class_name.string);
    }
    अण्णहा
	semsg(_(e_method_not_found_on_class_str_str), method_name,
		cl->class_name.string);
    vim_free(method_name);
}

/*
 * Echo a class or object member not found message.
 */
    शून्य
member_not_found_सन्देशः(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    जइ (v_type == VAR_OBJECT)
    {
	जइ (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	अण्णहा
	    semsg(_(e_variable_not_found_on_object_str_str), varname,
		    cl->class_name.string);
    }
    अण्णहा
    {
	जइ (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	अण्णहा
	{
	    जइ (IS_ENUM(cl))
		semsg(_(e_enum_मानम्_str_not_found_in_enum_str),
			varname, cl->class_name.string);
	    अण्णहा
		semsg(_(e_class_variable_str_not_found_in_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    शून्य
defcompile_class(class_T *cl)
{
    प्रति (पूर्णाङ्क loop = 1; loop <= 2; ++loop)
    {
	पूर्णाङ्क func_गणनम् = loop == 1 ? cl->class_class_function_गणनम्
						: cl->class_obj_method_गणनम्;
	प्रति (पूर्णाङ्क i = 0; i < func_गणनम्; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    जइ (!IS_ABSTRACT_METHOD(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    शून्य
defcompile_classes_in_script(शून्य)
{
    प्रति (class_T *cl = first_class; cl != रिक्त; cl = cl->class_next_used)
    {
	जइ (eval_variable(cl->class_name.string, 0, 0, रिक्त, रिक्त,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    पूर्णाङ्क
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    जइ (eval_variable(name, 0, 0, rettv, रिक्त, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	प्रत्यागम rettv->v_type == VAR_CLASS;
    प्रत्यागम FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
object_call_builtin_method(
    object_T		*obj,
    class_builtin_T	builtin_method,
    पूर्णाङ्क			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    पूर्णाङ्क	    midx;

    जइ (obj == रिक्त)
	प्रत्यागम FAIL;

    uf = class_get_builtin_method(obj->obj_class, builtin_method, &midx);
    जइ (uf == रिक्त)
	प्रत्यागम FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    पूर्णाङ्क		r;

    जइ (fc == रिक्त)
	प्रत्यागम FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, रिक्त, obj, fc, rettv);

    remove_funccal();

    प्रत्यागम r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    पूर्णाङ्क
object_empty(object_T *obj)
{
    typval_T	rettv;

    जइ (object_call_builtin_method(obj, CLASS_BUILTIN_EMPTY, 0, रिक्त, &rettv)
								== FAIL)
	प्रत्यागम TRUE;

    प्रत्यागम tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    पूर्णाङ्क
object_len(object_T *obj)
{
    typval_T	rettv;

    जइ (object_call_builtin_method(obj, CLASS_BUILTIN_LEN, 0, रिक्त, &rettv)
								== FAIL)
	प्रत्यागम 0;

    प्रत्यागम tv_to_सङ्ख्या(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    पूर्णाङ्क
object_equal(
	object_T *o1,
	object_T *o2,
	पूर्णाङ्क	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    जइ (o1 == o2)
	प्रत्यागम TRUE;
    जइ (o1 == रिक्त || o2 == रिक्त)
	प्रत्यागम FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    जइ (cl1 != cl2 || cl1 == रिक्त || cl2 == रिक्त)
	प्रत्यागम FALSE;

    प्रति (पूर्णाङ्क i = 0; i < cl1->class_obj_member_गणनम्; ++i)
	जइ (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    प्रत्यागम FALSE;

    प्रत्यागम TRUE;
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
    पूर्णाङ्क		copyID,
    पूर्णाङ्क		echo_style,
    पूर्णाङ्क		restore_copyID,
    पूर्णाङ्क		composite_val)
{
    typval_T	rettv;

    जइ (object_call_builtin_method(obj, CLASS_BUILTIN_STRING, 0, रिक्त, &rettv)
								== OK
					&& rettv.vval.v_string != रिक्त)
	प्रत्यागम rettv.vval.v_string;

    पूर्णाङ्क		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    जइ (cl != रिक्त && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    अण्णहा
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	जइ (cl == रिक्त)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	अण्णहा
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
    }
    जइ (cl != रिक्त)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	प्रति (पूर्णाङ्क i = 0; i < cl->class_obj_member_गणनम्; ++i)
	{
	    जइ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.length);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = रिक्त;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, composite_val);
	    जइ (s != रिक्त)
		ga_concat(&ga, s);
	    vim_free(tf);
	    जइ (s == रिक्त || did_echo_string_emsg)
	    {
		ok = FAIL;
		भङ्ग;
	    }
	    पङ्क्तिः_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    जइ (ok == FAIL)
    {
	vim_free(ga.ga_data);
	प्रत्यागम रिक्त;
    }
    ga_append(&ga, NUL);
    प्रत्यागम (char_u *)ga.ga_data;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    पूर्णाङ्क
class_instance_of(class_T *cl, class_T *other_cl)
{
    जइ (cl == other_cl)
	प्रत्यागम TRUE;

    // Recursively check the base classes.
    प्रति (; cl != रिक्त; cl = cl->class_extends)
    {
	जइ (cl == other_cl)
	    प्रत्यागम TRUE;
	// Check the implemented interfaces and the super interfaces
	प्रति (पूर्णाङ्क i = cl->class_interface_गणनम् - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    जाव (intf != रिक्त)
	    {
		जइ (intf == other_cl)
		    प्रत्यागम TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    प्रत्यागम FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    शून्य
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_सङ्ख्या = VVAL_FALSE;

    जइ (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_args(argvars, 1) == FAIL)
	प्रत्यागम;

    जइ (object_tv->vval.v_object == रिक्त)
    {
	जइ (classinfo_tv->vval.v_class == रिक्त)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_सङ्ख्या = VVAL_TRUE;
	प्रत्यागम;
    }

    प्रति (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	जइ (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	अण्णहा
	    c = classinfo_tv->vval.v_class;

	जइ (class_instance_of(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_सङ्ख्या = VVAL_TRUE;
	    प्रत्यागम;
	}
    }
}

#endif // FEAT_EVAL
