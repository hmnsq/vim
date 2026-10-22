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
#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

ساكن class_T *first_class = عدم;
ساكن class_T *next_nonref_class = عدم;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    ساكن فراغ
class_created(class_T *cl)
{
    إذا (first_class != عدم)
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
    ساكن فراغ
class_cleared(class_T *cl)
{
    إذا (cl->class_next_used != عدم)
	cl->class_next_used->class_prev_used = cl->class_prev_used;
    إذا (cl->class_prev_used != عدم)
	cl->class_prev_used->class_next_used = cl->class_next_used;
    وإلا إذا (first_class == cl)
	first_class = cl->class_next_used;

    // update the next class to check if needed
    إذا (cl == next_nonref_class)
	next_nonref_class = cl->class_next_used;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    ساكن صحيح
parse_member(
    exarg_T	*eap,
    char_u	*سطر,
    char_u	*varname,
    صحيح		has_public,	    // TRUE if "public" seen before "varname"
    char_u	**varname_end,
    صحيح		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_تعبير)
{
    *varname_end = to_name_end(varname, FALSE);
    إذا (*varname == '_' && has_public)
    {
	semsg(_(e_public_variable_name_cannot_start_with_underscore_str), سطر);
	أرجع FAIL;
    }

    char_u *colon = skipwhite(*varname_end);
    char_u *type_arg = colon;
    type_T *type = عدم;
    *has_type = FALSE;
    إذا (*colon == ':')
    {
	إذا (VIM_ISWHITE(**varname_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    أرجع FAIL;
	}
	إذا (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    أرجع FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, عدم, عدم, TRUE);
	إذا (type == عدم || !valid_declaration_type(type))
	    أرجع FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    إذا (type == عدم && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	أرجع FAIL;
    }

    إذا (init_تعبير == عدم && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_in_interface));
	أرجع FAIL;
    }

    إذا (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *تعبير_start, *تعبير_end;

	إذا (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    أرجع FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(فراغ)skip_تعبير_concatenate(&init_arg, &تعبير_start, &تعبير_end, &evalarg);

	init_arg = skipwhite(init_arg);
	إذا (*init_arg != NUL && !vim9_comment_start(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, عدم);
	    أرجع FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	إذا (type == عدم)
	    type = &t_any;

	*init_تعبير = vim_strnsave(تعبير_start, تعبير_end - تعبير_start);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, عدم);
    }
    وإلا إذا (!valid_declaration_type(type))
	أرجع FAIL;

    *type_ret = type;
    أرجع OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    ساكن صحيح
add_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_end,
    صحيح		has_public,
    صحيح		has_final,
    صحيح		has_const,
    صحيح		has_type,
    type_T	*type,
    char_u	*init_تعبير)
{
    إذا (ga_grow(gap, 1) == FAIL)
	أرجع FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_data) + gap->ga_len;
    m->ocm_name.length = (size_t)(varname_end - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.length);
    إذا (m->ocm_name.string == عدم)
    {
	m->ocm_name.length = 0;
	أرجع FAIL;
    }
    m->ocm_access = has_public ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_PRIVATE : VIM_ACCESS_READ;
    إذا (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    إذا (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    إذا (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    إذا (init_تعبير != عدم)
    {
	m->ocm_init = init_تعبير;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    أرجع OK;
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
    ساكن صحيح
add_members_to_class(
    garray_T	*gap,
    ocmember_T	*parent_members,
    صحيح		parent_عدد,
    ocmember_T	**members,
    صحيح		*member_عدد)
{
    *member_عدد = parent_عدد + gap->ga_len;
    *members = *member_عدد == 0 ? عدم
				       : ALLOC_MULT(ocmember_T, *member_عدد);
    إذا (*member_عدد > 0 && *members == عدم)
	أرجع FAIL;
    لكل (صحيح i = 0; i < parent_عدد; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = parent_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.length);
	إذا (m->ocm_name.string == عدم)
	{
	    m->ocm_name.length = 0;
	    أرجع FAIL;
	}
	إذا (m->ocm_init != عدم)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    إذا (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + parent_عدد,
			       gap->ga_data, حجم(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_data);
    أرجع OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    صحيح
object_index_from_itf_index(class_T *itf, صحيح is_method, صحيح idx, class_T *cl)
{
    إذا (idx >= (is_method ? itf->class_obj_method_عدد
				   : itf->class_obj_member_عدد))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	أرجع 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    إذا (cl == itf)
	أرجع idx;

    itf2class_T		*i2c = عدم;
    صحيح			searching = TRUE;
    صحيح			method_offset = 0;

    لكل (class_T *super = cl; super != عدم && searching;
						super = super->class_extends)
    {
	لكل (i2c = itf->class_itf2class; i2c != عدم; i2c = i2c->i2c_next)
	{
	    إذا (i2c->i2c_class == super && i2c->i2c_is_method == is_method)
	    {
		searching = FALSE;
		اكسر;
	    }
	}
	إذا (searching && is_method)
	    // The parent class methods are stored after the current class
	    // methods.
	    method_offset += super->class_obj_method_عدد_child;
    }
    إذا (i2c == عدم)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	أرجع 0;
    }

    // A table follows the i2c for the class
    صحيح *table = (صحيح *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    أرجع table[idx] + method_offset;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    ساكن صحيح
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    صحيح	    is_class)
{
    typval_T	tv;
    صحيح		success = FALSE;

    إذا (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	أرجع success;
    }

    tv.v_type = VAR_UNKNOWN;
    إذا (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_found_str), extends_name);
	أرجع success;
    }

    إذا (tv.v_type != VAR_CLASS || tv.vval.v_class == عدم
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    وإلا
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    أرجع success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    ساكن صحيح
validate_extends_generic_method(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    حرف	*رسالة = عدم;

    إذا (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	أرجع TRUE;

    إذا (super_fp->uf_generic_argcount == 0)
	رسالة = e_concrete_method_str_override_with_generic_method_in_class_str;
    وإلا إذا (cl_fp->uf_generic_argcount == 0)
	رسالة = e_generic_method_str_override_with_concrete_method_in_class_str;
    وإلا
	رسالة = e_generic_method_str_type_arguments_mismatch_in_class_str;

    semsg(_(رسالة), cl_fp->uf_name, super_cl->class_name.string);
    أرجع FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    ساكن صحيح
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    صحيح		method_عدد = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_data);

    طالما (super != عدم)
    {
	صحيح extends_method_عدد = super->class_obj_method_عدد_child;
	إذا (extends_method_عدد == 0)
	{
	    super = super->class_extends;
	    استمر;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	لكل (صحيح i = 0; i < extends_method_عدد; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    صحيح	    extends_private = (*pstr == '_');
	    إذا (extends_private)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    لكل (صحيح j = 0; j < method_عدد; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		صحيح	priv_method = (*qstr == '_');
		إذا (priv_method)
		    qstr++;
		إذا (STRCMP(pstr, qstr) == 0)
		{
		    إذا (priv_method != extends_private)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_method_str_of_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			أرجع FALSE;
		    }

		    إذا (!validate_extends_generic_method(super,
						extends_methods[i], cl_fp[j]))
			    أرجع FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    أرجع TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    ساكن صحيح
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحيح member_عدد = objmembers_gap->ga_len;
    إذا (member_عدد == 0)
	أرجع TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    لكل (صحيح c_i = 0; c_i < member_عدد; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	طالما (p_cl != عدم)
	{
	    صحيح p_member_عدد = p_cl->class_obj_member_عدد;

	    إذا (p_member_عدد == 0)
	    {
		p_cl = p_cl->class_extends;
		استمر;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    لكل (صحيح p_i = 0; p_i < p_member_عدد; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		إذا (STRCMP(pstr, qstr) == 0)
		{
		    إذا (c_protected != p_protected)
			semsg(_(e_public_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    وإلا
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    أرجع FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    أرجع TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    ساكن صحيح
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحيح member_عدد = objmembers_gap->ga_len;
    إذا (member_عدد == 0)
	أرجع TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    لكل (صحيح c_i = 0; c_i < member_عدد; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	صحيح	    var_found = FALSE;

	// Check in all the parent classes in the lineage
	طالما (p_cl != عدم && !var_found)
	{
	    صحيح p_member_عدد = p_cl->class_obj_member_عدد;
	    إذا (p_member_عدد == 0)
	    {
		p_cl = p_cl->class_extends;
		استمر;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    لكل (صحيح p_i = 0; p_i < p_member_عدد; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		إذا (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    استمر;

		// Ensure the type is matching.
		where.wt_func_name = (حرف *)c_m->ocm_name.string;
		where.wt_kind = WT_MEMBER;

		إذا (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    أرجع FALSE;

		var_found = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    أرجع TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    ساكن صحيح
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    لكل (صحيح loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	صحيح extends_method_عدد = loop == 1
				? extends_cl->class_class_function_عدد
				: extends_cl->class_obj_method_عدد;
	إذا (extends_method_عدد == 0)
	    استمر;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	صحيح method_عدد = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_data
						: objmethods_gap->ga_data);

	لكل (صحيح i = 0; i < extends_method_عدد; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    إذا (!IS_ABSTRACT_METHOD(uf))
		استمر;

	    صحيح	concrete_method_found = FALSE;
	    صحيح	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    لكل (j = 0; !concrete_method_found && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		إذا (!IS_ABSTRACT_METHOD(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_method_found = TRUE;
	    }

	    إذا (concrete_method_found)
		استمر;

	    لكل (j = 0; j < method_عدد; j++)
	    {
		إذا (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_method_found = TRUE;
		    اكسر;
		}
	    }

	    إذا (!concrete_method_found)
	    {
		semsg(_(e_abstract_method_str_not_implemented), uf->uf_name);
		أرجع FALSE;
	    }
	}
    }

    أرجع TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    ساكن صحيح
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    صحيح		cl_member_عدد,
    class_T	*extends_cl)
{
    صحيح		variable_present  = FALSE;

    لكل (صحيح cl_i = 0; cl_i < cl_member_عدد; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	إذا (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    استمر;

	// Ensure the access type is same
	إذا (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_of_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    أرجع FALSE;
	}

	// Ensure the type is matching.
	إذا (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	وإلا
	{
	    where.wt_func_name = (حرف *)m->ocm_name.string;
	    where.wt_kind = WT_MEMBER;
	    إذا (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		أرجع FALSE;
	}

	variable_present = TRUE;
	اكسر;
    }

    إذا (!variable_present && extends_cl != عدم)
    {
	صحيح ext_cl_عدد = extends_cl->class_obj_member_عدد;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	أرجع intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_عدد,
					extends_cl->class_extends);
    }

    أرجع variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    ساكن صحيح
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحيح if_عدد = ifcl->class_obj_member_عدد;
    إذا (if_عدد == 0)
	أرجع TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_data);
    صحيح cl_عدد = objmembers_gap->ga_len;
    لكل (صحيح if_i = 0; if_i < if_عدد; ++if_i)
    {
	إذا (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_عدد, extends_cl))
	{
	    semsg(_(e_variable_str_of_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    أرجع FALSE;
	}
    }

    أرجع TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    ساكن صحيح
intf_method_type_matches(
    ufunc_T	*if_method,
    ufunc_T	*cl_method,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    إذا (cl_method->uf_generic_argcount != if_method->uf_generic_argcount)
    {
	semsg(_(e_type_var_عدد_of_method_str_differs_from_interface_str),
						cl_method->uf_name, intf_name);
	أرجع FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (حرف *)if_method->uf_name;
    where.wt_kind = WT_METHOD;
    إذا (check_type(if_method->uf_func_type, cl_method->uf_func_type, TRUE,
								where) == FAIL)
	أرجع FALSE;

    أرجع TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    ساكن صحيح
intf_method_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    صحيح	    cl_عدد,
    class_T *extends_cl,
    char_u  *intf_name)
{
    صحيح		method_present  = FALSE;

    لكل (صحيح cl_i = 0; cl_i < cl_عدد; ++cl_i)
    {
	إذا (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    إذا (!intf_method_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		أرجع FALSE;
	    method_present = TRUE;
	    اكسر;
	}
    }

    إذا (!method_present && extends_cl != عدم)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	صحيح	ext_cl_عدد = extends_cl->class_obj_method_عدد;
	أرجع intf_method_present(if_ufunc, ext_cl_fp, ext_cl_عدد,
					extends_cl->class_extends, intf_name);
    }

    أرجع method_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    ساكن صحيح
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    صحيح if_عدد = ifcl->class_obj_method_عدد;
    إذا (if_عدد == 0)
	أرجع TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_data);
    صحيح cl_عدد = objmethods_gap->ga_len;
    لكل (صحيح if_i = 0; if_i < if_عدد; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	إذا (!intf_method_present(if_fp[if_i], cl_fp, cl_عدد, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_method_str_of_interface_str_not_implemented),
		    if_name, intf_class_name);
	    أرجع FALSE;
	}
    }

    أرجع TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    ساكن صحيح
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحيح		success = TRUE;

    لكل (صحيح i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_data)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	إذا (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_found_str), impl);
	    success = FALSE;
	    اكسر;
	}

	إذا (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == عدم
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    اكسر;
	}

	class_T *ifcl = tv.vval.v_class;
	إذا (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    اكسر;
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

    إذا (!success)
	أرجع FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_data;
    cl->class_interface_عدد = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    لكل (صحيح i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_data)[i],
		((class_T **)intf_classes_gap->ga_data)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = عدم;
    cl->class_interface_عدد = 0;
    cl->class_extends = عدم;

    أرجع success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    ساكن صحيح
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    لكل (صحيح j = 0; j < intf_classes_gap->ga_len; j++)
    {
	إذا (((class_T **)intf_classes_gap->ga_data)[j] == ifcl)
	    أرجع TRUE;
    }

    أرجع FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    ساكن صحيح
add_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.length);
    إذا (intf_name == عدم)
	أرجع FALSE;

    إذا (ga_grow(impl_gap, 1) == FAIL)
	اذهب fail;

    char_u **intf_names = (char_u **)impl_gap->ga_data;
    intf_names[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    إذا (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	اذهب fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_data;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    أرجع TRUE;
fail:
    vim_free(intf_name);
    أرجع FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    ساكن صحيح
add_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    لكل (صحيح i = 0; i < super->class_interface_عدد; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	إذا (!is_interface_class_present(intf_classes_gap, ifcl))
	    add_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    أرجع TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    ساكن صحيح
check_func_arg_names(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    لكل (صحيح loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	لكل (صحيح fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_data)[fi];

	    لكل (صحيح i = 0; i < uf->uf_وسائط.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_وسائط.ga_data)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		لكل (صحيح mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_data + mi)->ocm_name.string;
		    إذا (STRCMP(aname, mname) == 0)
		    {
			إذا (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_argument_already_declared_in_class_str),
				aname);

			أرجع FALSE;
		    }
		}
	    }
	}
    }

    أرجع TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    ساكن صحيح
is_reserved_varname(char_u *varname, char_u *varname_end)
{
    صحيح reserved = FALSE;
    char_u save_varname_end = *varname_end;
    *varname_end = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_end = save_varname_end;

    أرجع reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    ساكن صحيح
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_end)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_end - varname)};
    صحيح		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    إذا (*pstr.string == '_')
    {
	pstr.string++;
	pstr.length--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    لكل (صحيح loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	لكل (صحيح i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_data) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.length};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    إذا (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.length--;
		q_protected = true;
	    }

	    إذا (pstr.length == qstr.length
		&& STRNCMP(pstr.string, qstr.string, pstr.length) == 0)
	    {
		char_u	save_c = *varname_end;

		*varname_end = NUL;
		إذا (p_protected != q_protected)
		    semsg(_(e_public_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		وإلا
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_end = save_c;
		dup = TRUE;
		اكسر;
	    }
	}
    }

    أرجع dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    ساكن صحيح
is_duplicate_method(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    لكل (صحيح loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	لكل (صحيح i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_data)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    إذا (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		أرجع TRUE;
	    }
	}
    }

    أرجع FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    ساكن صحيح
is_valid_constructor(ufunc_T *uf, صحيح is_abstract, صحيح has_static)
{
    // Constructors are not allowed in abstract classes.
    إذا (is_abstract)
    {
	emsg(_(e_cannot_define_new_method_in_abstract_class));
	أرجع FALSE;
    }
    // A constructor is always static, no need to define it so.
    إذا (has_static)
    {
	emsg(_(e_cannot_define_new_method_as_static));
	أرجع FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    إذا (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_method));
	أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    ساكن صحيح
object_check_builtin_method_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    صحيح	    valid = FALSE;
    type_T  method_sig;
    type_T  method_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_FIELD(method_sig);
    CLEAR_FIELD(method_rt);
    method_sig.tt_type = VAR_FUNC;

    إذا (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	method_rt.tt_type = VAR_رقم;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    وإلا إذا (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	method_rt.tt_type = VAR_BOOL;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    وإلا إذا (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	method_rt.tt_type = VAR_STRING;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    وإلا
	semsg(_(e_builtin_object_method_str_not_supported), uf->uf_name);

    where.wt_func_name = (حرف *)uf->uf_name;
    where.wt_kind = WT_METHOD;
    إذا (valid && !check_type(&method_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    أرجع valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    صحيح
is_valid_builtin_obj_methodname(char_u *funcname)
{
    بدل (funcname[0])
    {
	حالة 'e':
	    أرجع STRNCMP(funcname, "empty", 5) == 0;

	حالة 'l':
	    أرجع STRNCMP(funcname, "len", 3) == 0;

	حالة 'n':
	    أرجع STRNCMP(funcname, "new", 3) == 0;

	حالة 's':
	    أرجع STRNCMP(funcname, "string", 6) == 0;
    }

    أرجع FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_method(
    class_T		*cl,
    class_builtin_T	builtin_method,
    صحيح			*method_idx)
{
    *method_idx = -1;

    إذا (cl == عدم)
	أرجع عدم;

    *method_idx = cl->class_builtin_methods[builtin_method];
    أرجع *method_idx != -1 ? cl->class_obj_methods[*method_idx] : عدم;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    ساكن صحيح
update_member_method_lookup_table(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    صحيح		pobj_method_offset)
{
    إذا (ifcl == عدم)
	أرجع OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(حجم(itf2class_T)
				+ ifcl->class_obj_member_عدد * حجم(صحيح));
    إذا (if2cl == عدم)
	أرجع FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = FALSE;

    لكل (صحيح if_i = 0; if_i < ifcl->class_obj_member_عدد; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	لكل (صحيح cl_i = 0; cl_i < cl->class_obj_member_عدد; ++cl_i)
	{
	    إذا (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		صحيح *table = (صحيح *)(if2cl + 1);
		table[if_i] = cl_i;
		اكسر;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(حجم(itf2class_T)
				+ ifcl->class_obj_method_عدد * حجم(صحيح));
    إذا (if2cl == عدم)
	أرجع FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = TRUE;

    لكل (صحيح if_i = 0; if_i < ifcl->class_obj_method_عدد; ++if_i)
    {
	صحيح	done = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	لكل (صحيح cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    إذا (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_data)[cl_i]->uf_name) == 0)
	    {
		صحيح *table = (صحيح *)(if2cl + 1);
		table[if_i] = cl_i;
		done = TRUE;
		اكسر;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	إذا (!done)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    إذا (cl->class_extends != ifcl)
	    {
		class_T		*parent = cl->class_extends;
		صحيح		method_offset = objmethods->ga_len;

		طالما (!done && parent != عدم && parent != ifcl)
		{
		    لكل (صحيح cl_i = 0;
			    cl_i < parent->class_obj_method_عدد_child; ++cl_i)
		    {
			إذا (STRCMP(if_name,
				    parent->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    صحيح *table = (صحيح *)(if2cl + 1);
			    table[if_i] = method_offset + cl_i;
			    done = TRUE;
			    اكسر;
			}
		    }
		    method_offset += parent->class_obj_method_عدد_child;
		    parent = parent->class_extends;
		}
	    }

	    إذا (!done)
	    {
		صحيح *table = (صحيح *)(if2cl + 1);
		table[if_i] = pobj_method_offset + if_i;
	    }
	}
    }

    أرجع OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    ساكن صحيح
add_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    لكل (صحيح i = 0; i < cl->class_interface_عدد; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	طالما (ifcl != عدم)
	{
	    إذا (update_member_method_lookup_table(ifcl, cl, objmethods_gap,
								0) == FAIL)
		أرجع FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    إذا (extends_cl != عدم)
    {
	class_T		*pclass = extends_cl;
	صحيح		pobj_method_offset = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	طالما (pclass != عدم)
	{
	    إذا (update_member_method_lookup_table(pclass, cl,
			objmethods_gap, pobj_method_offset) == FAIL)
		أرجع FAIL;

	    pobj_method_offset += pclass->class_obj_method_عدد_child;
	    pclass = pclass->class_extends;
	}
    }

    أرجع OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    ساكن صحيح
add_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_عدد);
    إذا (cl->class_members_tv == عدم)
	أرجع FAIL;

    لكل (صحيح i = 0; i < cl->class_class_member_عدد; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	إذا (m->ocm_init != عدم)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_تعبير(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    إذا (etv == عدم)
		أرجع FAIL;

	    إذا (m->ocm_type->tt_type == VAR_ANY
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
	وإلا
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = عدم;
	}
	set_tv_type(tv, m->ocm_type);
	إذا (m->ocm_flags & OCMFLAG_CONST)
	    item_lock(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    أرجع OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    ساكن فراغ
add_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    صحيح		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    لكل (صحيح i = 0; i < cl->class_obj_member_عدد; ++i)
    {
	إذا (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    استمر;

	إذا (i > (is_enum ? 2 : 0))
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

    garray_T أسطر_to_free;
    ga_init2(&أسطر_to_free, حجم(char_u *), 50);

    ufunc_T *nf = define_function(&fea, عدم, &أسطر_to_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_عدد,
			    عدم);

    ga_clear_strings(&أسطر_to_free);
    vim_free(fga.ga_data);

    إذا (nf != عدم && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_data)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	إذا (nf->uf_ret_type != عدم)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_وسائط = عدم;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ساكن صحيح
add_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    لكل (صحيح loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	صحيح		*fcount = loop == 1 ? &cl->class_class_function_عدد
						: &cl->class_obj_method_عدد;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	صحيح parent_عدد = 0;
	إذا (extends_cl != عدم)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    parent_عدد = loop == 1
				? 0
				: extends_cl->class_obj_method_عدد;

	*fcount = parent_عدد + gap->ga_len;
	إذا (*fcount == 0)
	{
	    *fup = عدم;
	    استمر;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	إذا (*fup == عدم)
	    أرجع FAIL;

	إذا (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_data, حجم(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_data);
	إذا (loop == 1)
	    cl->class_class_function_عدد_child = gap->ga_len;
	وإلا
	    cl->class_obj_method_عدد_child = gap->ga_len;

	إذا (loop == 2)
	{
	    // Copy instance methods from the parent.

	    لكل (صحيح i = 0; i < parent_عدد; ++i)
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
		إذا (nf != عدم && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_وسائط != عدم)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		لكل (صحيح ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    إذا (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (حرف *)pname;
			where.wt_kind = WT_METHOD;
			(فراغ)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	لكل (صحيح i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    إذا (i < gap->ga_len)
		fp->uf_defclass = cl;
	    إذا (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    أرجع OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    ساكن فراغ
update_builtin_method_index(class_T *cl)
{
    صحيح	i;

    لكل (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    لكل (i = 0; i < cl->class_obj_method_عدد; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	إذا (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	وإلا إذا (cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] = i;
	وإلا إذا (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    ساكن char_u *
find_class_name_end(char_u *arg)
{
    char_u *end = arg;

    طالما (ASCII_ISALNUM(*end) || *end == '_'
	    || (*end == '.' && (ASCII_ISALNUM(end[1]) || end[1] == '_')))
	++end;

    أرجع end;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    ساكن صحيح
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
    صحيح		dup = FALSE;

    لكل (صحيح i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_data) + i;

	إذا (name.length == m->ocm_name.length
	    && STRNCMP(name.string, m->ocm_name.string, name.length) == 0)
	{
	    char_u  save_c = *varname_end;

	    *varname_end = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_end = save_c;
	    dup = TRUE;
	    اكسر;
	}
    }

    أرجع dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    ساكن صحيح
enum_parse_قيم(
    exarg_T	*eap,
    class_T	*en,
    char_u	*سطر,
    garray_T	*gap,
    صحيح		*رقم_enum_قيم,
    صحيح		*enum_end)
{
    evalarg_T	evalarg;
    char_u	*p = سطر;
    حرف	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	last_char = NUL;
    صحيح		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    صحيح		did_emsg_before = did_emsg;
    طالما (*p != NUL)
    {
	// ignore comment
	إذا (*p == '#')
	    اكسر;

	إذا (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_قيمة_declaration_str), p);
	    اكسر;
	}

	char_u *eni_name_start = p;
	char_u *eni_name_end = to_name_end(p, FALSE);

	إذا (is_duplicate_enum(gap, eni_name_start, eni_name_end))
	    اكسر;

	p = skipwhite(eni_name_end);

	char_u	*init_تعبير = عدم;
	إذا (*p == '(' || *p == '<')
	{
	    إذا (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", سطر);
		اكسر;
	    }

	    char_u *تعبير_start, *تعبير_end;

	    p = eni_name_start;
	    (فراغ)skip_تعبير_concatenate(&p, &تعبير_start, &تعبير_end, &evalarg);

	    طالما (*تعبير_start && *تعبير_start != '(' && *تعبير_start != '<')
		تعبير_start++;

	    إذا (تعبير_end > تعبير_start)
		init_تعبير = vim_strnsave(تعبير_start, تعبير_end - تعبير_start);
	}

	إذا (init_تعبير == عدم)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, حجم(initexpr_buf), "%s.new()", en->class_name.string);
	وإلا
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, حجم(initexpr_buf), "%s.new%s", en->class_name.string,
		init_تعبير);
	    vim_free(init_تعبير);
	}
	إذا (add_member(gap, eni_name_start, eni_name_end, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    اكسر;

	++*رقم_enum_قيم;

	إذا (*p != '#')
	    last_char = *p;

	إذا (*p != NUL && *p != ',')
	    اكسر;

	إذا (*p == ',')
	{
	    إذا (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", سطر);
		اكسر;
	    }
	    إذا (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", سطر);
		اكسر;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    إذا (*p != NUL && *p != '#')
    {
	إذا (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_argument_str), p);
	rc = FAIL;
    }

    إذا (last_char != ',')
	// last enum value should not be terminated by ","
	*enum_end = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, عدم);

    أرجع rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    ساكن صحيح
enum_add_قيم_member(
    class_T	*en,
    garray_T	*gap,
    صحيح		رقم_enum_قيم,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    صحيح		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    لكل (صحيح i = 0; i < رقم_enum_قيم; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;

	إذا (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.length);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    إذا (type == عدم)
	اذهب done;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    إذا (type->tt_member != عدم)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = add_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_data, fga.ga_len));

done:
    vim_free(fga.ga_data);

    أرجع rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    ساكن فراغ
enum_clear_constructors(class_T *en)
{
    لكل (صحيح i = 0; i < en->class_class_function_عدد; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	إذا (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    فراغ
enum_set_internal_obj_vars(class_T *en, object_T *enval)
{
    صحيح	i;

    لكل (i = 0; i < en->class_class_member_عدد; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	إذا (en_tv != عدم && en_tv->v_type == VAR_UNKNOWN)
	    اكسر;
    }

    إذا (i >= en->class_class_member_عدد)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	أرجع;

    // First object variable is the name
    ocmember_T *قيمة_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(قيمة_ocm->ocm_name.string, قيمة_ocm->ocm_name.length);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_رقم;
    ord_tv->vval.v_رقم = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    ساكن فراغ
skip_class_body(exarg_T *eap)
{
    لكل (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	إذا (theline == عدم)
	    اكسر;

	char_u	*p = theline;
	صحيح	found;

	// skip ':' and blanks
	لكل (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	found = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	إذا (found)
	    اكسر;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    فراغ
ex_class(exarg_T *eap)
{
    صحيح		is_class = eap->cmdidx == CMD_class;
    صحيح		is_abstract = eap->cmdidx == CMD_abstract;
    صحيح		is_enum = eap->cmdidx == CMD_enum;
    صحيح		added_enum_قيم = FALSE;
    صحيح		is_interface;
    long	start_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    إذا (eap->skip)
    {
	skip_class_body(eap);
	أرجع;
    }

    إذا (is_abstract)
    {
	إذا (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_argument_str), arg);
	    أرجع;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    إذا (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	إذا (is_class)
	    emsg(_(e_class_can_only_be_defined_in_vim9_script));
	وإلا إذا (is_enum)
	    emsg(_(e_enum_can_only_be_defined_in_vim9_script));
	وإلا
	    emsg(_(e_interface_can_only_be_defined_in_vim9_script));
	أرجع;
    }

    إذا (!ASCII_ISUPPER(*arg))
    {
	إذا (is_class)
	    semsg(_(e_class_name_must_start_with_uppercase_letter_str), arg);
	وإلا إذا (is_enum)
	    semsg(_(e_enum_name_must_start_with_uppercase_letter_str), arg);
	وإلا
	    semsg(_(e_interface_name_must_start_with_uppercase_letter_str),
									  arg);
	أرجع;
    }
    char_u *name_end = find_name_end(arg, عدم, عدم, FNE_CHECK_START);
    إذا (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	أرجع;
    }
    char_u *name_start = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = عدم;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, حجم(char_u *), 5);

    arg = skipwhite(name_end);
    طالما (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	إذا (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    إذا (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		اذهب early_ret;
	    }
	    إذا (extends != عدم)
	    {
		emsg(_(e_duplicate_extends));
		اذهب early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *end = find_class_name_end(arg);
	    إذا (!IS_WHITE_OR_NUL(*end))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		اذهب early_ret;
	    }
	    extends = vim_strnsave(arg, end - arg);
	    إذا (extends == عدم)
		اذهب early_ret;

	    arg = skipwhite(end + 1);
	}
	وإلا إذا (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    إذا (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		اذهب early_ret;
	    }

	    إذا (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		اذهب early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    لكل (;;)
	    {
		char_u *impl_end = find_class_name_end(arg);
		إذا ((!IS_WHITE_OR_NUL(*impl_end) && *impl_end != ',')
			|| (*impl_end == ','
			    && !IS_WHITE_OR_NUL(*(impl_end + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    اذهب early_ret;
		}
		إذا (impl_end - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    اذهب early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_end - arg);
		إذا (iname == عدم)
		    اذهب early_ret;
		لكل (صحيح i = 0; i < ga_impl.ga_len; ++i)
		    إذا (STRCMP(((char_u **)ga_impl.ga_data)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			اذهب early_ret;
		    }
		إذا (ga_add_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    اذهب early_ret;
		}
		إذا (*impl_end != ',')
		{
		    arg = skipwhite(impl_end);
		    اكسر;
		}
		arg = skipwhite(impl_end + 1);
	    }
	}
	وإلا
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_strings(&ga_impl);
	    أرجع;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, حجم(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, حجم(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, حجم(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, حجم(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, حجم(ufunc_T *), 10);

    class_T *cl = عدم;
    class_T *extends_cl = عدم;  // class from "extends" argument
    class_T **intf_classes = عدم;
    صحيح	    intf_عدد = 0;	// number of items in "intf_classes"
    صحيح	    رقم_enum_قيم = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    إذا (cl == عدم)
	اذهب cleanup;

    إذا (is_enum)
	cl->class_flags = CLASS_ENUM;
    وإلا إذا (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    وإلا إذا (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.length = (size_t)(name_end - name_start);
    cl->class_name.string = vim_strnsave(name_start, cl->class_name.length);
    إذا (cl->class_name.string == عدم)
	اذهب cleanup;

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
    صحيح rc = set_var_const(cl->class_name.string, 0, عدم, &tv, FALSE, 0, 0);
    إذا (rc == FAIL)
	اذهب cleanup;

    إذا (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	إذا (add_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, عدم) == FAIL)
	    اذهب cleanup;

	varname = (char_u *)"ordinal";
	إذا (add_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_رقم, عدم) == FAIL)
	    اذهب cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    صحيح		enum_end = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = عدم;
    صحيح success = FALSE;
    لكل (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	إذا (theline == عدم)
	    اكسر;
	char_u *سطر = skipwhite(theline);

	// Skip empty and comment lines.
	إذا (*سطر == NUL)
	    استمر;
	إذا (*سطر == '#')
	{
	    إذا (vim9_bad_comment(سطر))
		اكسر;
	    استمر;
	}

	char_u *p = سطر;

	حرف	*end_name;
	صحيح	shortlen;
	صحيح	fullen;
	إذا (is_class)
	{
	    end_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	وإلا إذا (is_enum)
	{
	    end_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	وإلا
	{
	    end_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	لكل (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_start = p;
	إذا (checkforcmd(&p, end_name, shortlen))
	{
	    إذا (STRNCMP(cmd_start, end_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_start);
	    وإلا إذا (*p == '|' || !ends_excmd2(سطر, p))
		semsg(_(e_trailing_characters_str), p);
	    وإلا
		success = TRUE;
	    اكسر;
	}

	صحيح	wrong_endname = FALSE;
	إذا (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	وإلا إذا (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	وإلا
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	إذا (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), سطر, end_name);
	    اكسر;
	}

	إذا (is_enum && !enum_end)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    إذا (enum_parse_قيم(eap, cl, سطر, &classmembers,
					&رقم_enum_قيم, &enum_end) == FAIL)
		اكسر;

	    إذا (enum_end)
	    {
		// Add the enum "values" class variable.
		enum_add_قيم_member(cl, &classmembers, رقم_enum_قيم,
							&type_list);
		added_enum_قيم = TRUE;
	    }
	    استمر;
	}

	صحيح has_public = FALSE;
	إذا (checkforcmd(&p, "public", 3))
	{
	    إذا (STRNCMP(سطر, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), سطر);
		اكسر;
	    }
	    إذا (is_interface)
	    {
		emsg(_(e_public_variable_not_supported_in_interface));
		اكسر;
	    }
	    has_public = TRUE;
	    p = skipwhite(سطر + 6);

	    إذا (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_public_keyword_not_supported_for_method));
		اكسر;
	    }

	    إذا (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_public_must_be_followed_by_var_static_final_or_const));
		اكسر;
	    }
	}

	صحيح abstract_method = FALSE;
	char_u *pa = p;
	إذا (checkforcmd(&p, "abstract", 3))
	{
	    إذا (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		اكسر;
	    }

	    إذا (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_used_in_enum));
		اكسر;
	    }

	    إذا (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_used_in_interface));
		اكسر;
	    }

	    إذا (!is_abstract)
	    {
		semsg(_(e_abstract_method_in_concrete_class), pa);
		اكسر;
	    }

	    p = skipwhite(pa + 8);
	    إذا (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		اكسر;
	    }

	    abstract_method = TRUE;
	}

	صحيح has_static = FALSE;
	char_u *ps = p;
	إذا (checkforcmd(&p, "static", 4))
	{
	    إذا (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		اكسر;
	    }

	    إذا (is_interface)
	    {
		emsg(_(e_static_member_not_supported_in_interface));
		اكسر;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    إذا (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		اكسر;
	    }
	}

	صحيح has_final = FALSE;
	صحيح has_var = FALSE;
	صحيح has_const = FALSE;
	إذا (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	وإلا إذا (checkforcmd(&p, "final", 5))
	{
	    إذا (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_in_interface));
		اكسر;
	    }
	    has_final = TRUE;
	}
	وإلا إذا (checkforcmd(&p, "const", 5))
	{
	    إذا (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_in_interface));
		اكسر;
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
	إذا (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_end = عدم;
	    type_T *type = عدم;
	    char_u *init_تعبير = عدم;
	    صحيح	    has_type = FALSE;

	    إذا (!eval_isnamec1(*p))
	    {
		إذا (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), سطر);
		وإلا
		    semsg(_(e_invalid_object_variable_declaration_str), سطر);
		اكسر;
	    }

	    إذا (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_in_interface),
			varname);
		اكسر;
	    }

	    إذا (parse_member(eap, سطر, varname, has_public,
			  &varname_end, &has_type, &type_list, &type,
			  !is_interface ? &init_تعبير: عدم) == FAIL)
		اكسر;

	    إذا (is_reserved_varname(varname, varname_end)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_end))
	    {
		vim_free(init_تعبير);
		اكسر;
	    }
	    إذا (add_member(has_static ? &classmembers : &objmembers, varname,
				varname_end, has_public, has_final, has_const,
				has_type, type, init_تعبير) == FAIL)
	    {
		vim_free(init_تعبير);
		اكسر;
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
	وإلا إذا (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	أسطر_to_free;
	    صحيح		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    إذا (has_public)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_public_keyword_not_supported_for_method));
		اكسر;
	    }

	    إذا (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_in_class_str), سطر);
		اكسر;
	    }

	    إذا (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_method_not_supported_in_interface), p);
		اكسر;
	    }

	    إذا (has_static && !is_new && SAFE_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_method_not_supported), p);
		اكسر;
	    }

	    CLEAR_FIELD(ea);
	    ea.cmd = سطر;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&أسطر_to_free, حجم(char_u *), 50);
	    صحيح class_flags;
	    إذا (is_interface)
		class_flags = CF_INTERFACE;
	    وإلا
		class_flags = abstract_method ? CF_ABSTRACT_METHOD : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, عدم, &أسطر_to_free,
			class_flags, objmembers.ga_data, objmembers.ga_len,
			عدم);
	    ga_clear_strings(&أسطر_to_free);

	    إذا (uf != عدم)
	    {
		إذا (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_method_not_supported_in_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    اكسر;
		}

		// check for builtin method
		إذا (!is_new && SAFE_islower(*uf->uf_name) &&
					!object_check_builtin_method_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    اكسر;
		}

		// Check the uf_name isn't used already.
		إذا (is_duplicate_method(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    اكسر;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		إذا (ga_grow(fgap, 1) == OK)
		{
		    إذا (is_new)
			uf->uf_flags |= FC_NEW;

		    إذا (abstract_method)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_data)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	وإلا
	{
	    إذا (is_class)
		semsg(_(e_not_valid_command_in_class_str), سطر);
	    وإلا إذا (is_enum)
		semsg(_(e_not_valid_command_in_enum_str), سطر);
	    وإلا
		semsg(_(e_not_valid_command_in_interface_str), سطر);
	    اكسر;
	}
    }

    إذا (theline == عدم && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    إذا (success && is_enum && (رقم_enum_قيم == 0 || !added_enum_قيم))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_add_قيم_member(cl, &classmembers, رقم_enum_قيم,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    إذا (success && extends != عدم)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    إذا (success && extends_cl != عدم)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    إذا (success && extends_cl != عدم)
    {
	إذا (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	وإلا
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    إذا (success && !is_abstract && extends_cl != عدم
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, حجم(class_T *), 5);

    إذا (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    إذا (success && extends_cl != عدم)
	success = add_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_data;
    intf_عدد = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    إذا (success)
	success = check_func_arg_names(&classfunctions, &objmethods,
							&classmembers);

    إذا (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	إذا (extends_cl != عدم)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	إذا (add_members_to_class(&classmembers,
				 عدم,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_عدد) == FAIL
		|| add_members_to_class(&objmembers,
				 extends_cl == عدم ? عدم
					       : extends_cl->class_obj_members,
				 extends_cl == عدم ? 0
					  : extends_cl->class_obj_member_عدد,
				 &cl->class_obj_members,
				 &cl->class_obj_member_عدد) == FAIL)
	    اذهب cleanup;

	إذا (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_عدد = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    إذا (cl->class_interfaces == عدم)
		اذهب cleanup;
	    لكل (صحيح i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_data)[i];
	    VIM_CLEAR(ga_impl.ga_data);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = عدم;
	}

	إذا (cl->class_interface_عدد > 0 || extends_cl != عدم)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    إذا (add_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		اذهب cleanup;
	}

	صحيح	have_new = FALSE;
	ufunc_T	*class_func = عدم;
	لكل (صحيح i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_data)[i];
	    إذا (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		اكسر;
	    }
	}

	إذا (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	وإلا إذا ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    add_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	إذا (add_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    اذهب cleanup;

	update_builtin_method_index(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	إذا ((is_class || is_enum) && cl->class_class_member_عدد > 0)
	    إذا (add_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		أرجع;
	    }

	cl->class_type_list = type_list;

	إذا (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	أرجع;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    إذا (intf_classes != عدم)
    {
	لكل (صحيح i = 0; i < intf_عدد; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_strings(&ga_impl);

    لكل (صحيح round = 1; round <= 2; ++round)
    {
	garray_T *gap = round == 1 ? &classmembers : &objmembers;
	إذا (gap->ga_len == 0 || gap->ga_data == عدم)
	    استمر;

	لكل (صحيح i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    لكل (صحيح i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    لكل (صحيح i = 0; i < classfunctions.ga_len; ++i)
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
    صحيح		is_object,
    char_u	*name,
    char_u	*name_end,
    صحيح		*member_idx)
{
    size_t	len = name_end - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    إذا (m == عدم)
    {
	member_not_found_رسالة(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	أرجع &t_any;
    }

    أرجع m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    صحيح		is_object,
    صحيح		member_idx)
{
    ocmember_T	*m;
    صحيح		member_عدد;

    إذا (is_object)
    {
	m = cl->class_obj_members;
	member_عدد = cl->class_obj_member_عدد;
    }
    وإلا
    {
	m = cl->class_class_members;
	member_عدد = cl->class_class_member_عدد;
    }

    إذا (member_idx >= member_عدد)
	أرجع عدم;

    أرجع m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    ساكن فراغ
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    فراغ
typealias_unref(typealias_T *ta)
{
    إذا (ta != عدم && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    فراغ
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    إذا (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_in_vim9_script));
	أرجع;
    }

    إذا (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	أرجع;
    }

    إذا (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_start_with_uppercase_letter_str), arg);
	أرجع;
    }

    char_u *name_end = find_name_end(arg, عدم, عدم, FNE_CHECK_START);
    إذا (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	أرجع;
    }
    char_u *name_start = arg;

    arg = skipwhite(name_end);
    إذا (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	أرجع;
    }
    إذا (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	أرجع;
    }
    arg++;
    arg = skipwhite(arg);

    إذا (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	أرجع;
    }

    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, عدم, عدم, TRUE);
    إذا (type == عدم)
	أرجع;

    إذا (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	أرجع;
    }

    صحيح cc = *name_end;
    *name_end = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    إذا (eval_variable_import(name_start, &tv) == OK)
    {
	إذا (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_start);
	وإلا
	    semsg(_(e_redefining_script_item_str), name_start);
	clear_tv(&tv);
	اذهب done;
    }

    // Create a script-local variable for the type alias.
    إذا (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_lock = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_start);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_depth = parse_type_depth();
    }
    وإلا
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_lock = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    set_var_const(name_start, current_sctx.sc_sid, عدم, &tv, FALSE,
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
    صحيح
get_member_tv(
    class_T	*cl,
    صحيح		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    صحيح		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    إذا (m == عدم)
	أرجع FAIL;

    إذا (*name == '_' && (current_class == عدم ||
				!class_instance_of(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	أرجع FAIL;
    }

    إذا (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	set_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    وإلا
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	set_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    أرجع OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    ساكن صحيح
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
    typval_T	argvars[MAX_FUNC_وسائط + 1];
    صحيح		argcount = 0;
    ocmember_T	*ocm = عدم;
    صحيح		m_idx;

    fp = method_lookup(cl, rettv->v_type, name, len, عدم);
    إذا (fp == عدم)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	إذا (ocm == عدم || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    method_not_found_رسالة(cl, rettv->v_type, name, len);
	    أرجع FAIL;
	}

	إذا (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    أرجع FAIL;
	}

	إذا (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	وإلا
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_end;
	أرجع OK;
    }

    إذا (ocm == عدم && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
	أرجع FAIL;
    }

    // process generic function call
    إذا (fp != عدم)
    {
	fp = generic_func_get(fp, gfatab);
	إذا (fp == عدم)
	    أرجع FAIL;
    }

    char_u *argp = name_end;
    صحيح ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    إذا (ret == FAIL)
    {
	طالما (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	أرجع FAIL;
    }

    funcexe_T funcexe;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    إذا (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    إذا (evalarg != عدم)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    صحيح error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								عدم);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    لكل (صحيح idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    إذا (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_found_var);
	أرجع FAIL;
    }
    *arg = argp;

    أرجع OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    صحيح
obj_method_to_partial_tv(object_T *obj, ufunc_T *obj_method, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    إذا (pt == عدم)
	أرجع FAIL;

    pt->pt_refcount = 1;
    إذا (obj != عدم)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_method;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    أرجع OK;
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
    صحيح
class_object_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحيح		verbose UNUSED)	// give error messages
{
    صحيح	ret = FAIL;

    إذا (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	أرجع FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, عدم, عدم, FNE_CHECK_START);
    إذا (name_end == name)
	أرجع FAIL;
    size_t len = name_end - name;

    صحيح did_emsg_save = did_emsg;
    class_T *cl;
    إذا (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    وإلا // VAR_OBJECT
    {
	إذا (rettv->vval.v_object == عدم)
	{
	    emsg(_(e_using_null_object));
	    أرجع FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    إذا (cl == عدم)
    {
	emsg(_(e_incomplete_type));
	أرجع FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_وسائط_table_init(&gfatab);

    إذا (*name_end == '<')
    {
	cctx_T	    *cctx = عدم;

	إذا (evalarg != عدم)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_end = parse_generic_func_type_وسائط(name, len, name + len,
						&gfatab, cctx);
	إذا (name_end == عدم)
	    اذهب done;
    }

    إذا (*name_end == '(')
	// Invoke the class or object method
	ret = call_oc_method(cl, name, len, name_end, evalarg, arg,
							&gfatab, rettv);
    وإلا إذا (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	صحيح is_object = rettv->v_type == VAR_OBJECT;
	إذا (get_member_tv(cl, is_object, name, len, عدم, rettv) == OK)
	{
	    *arg = name_end;
	    ret = OK;
	    اذهب done;
	}

	// could be a class method or an object method
	صحيح	fidx;
	ufunc_T	*fp = method_lookup(cl, rettv->v_type, name, len, &fidx);
	إذا (fp != عدم)
	{
	    // Protected methods are not accessible outside the class
	    إذا (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		اذهب done;
	    }

	    إذا (obj_method_to_partial_tv(is_object ? rettv->vval.v_object :
						عدم, fp, rettv) == FAIL)
		اذهب done;

	    *arg = name_end;
	    ret = OK;
	    اذهب done;
	}

	إذا (did_emsg == did_emsg_save)
	    member_not_found_رسالة(cl, rettv->v_type, name, len);
    }

done:
    generic_func_وسائط_table_clear(&gfatab);

    أرجع ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
find_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, عدم, عدم, FNE_CHECK_START);
    إذا (name_end == name || *name_end != '.')
	أرجع عدم;

    ufunc_T	*fp = عدم;
    size_t	len = name_end - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    إذا (eval_variable(name, (صحيح)len,
				    0, &tv, عدم, EVAL_VAR_NOAUTOLOAD) == FAIL)
	أرجع عدم;
    إذا (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	اذهب fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    إذا (cl == عدم)
	اذهب fail_after_eval;
    char_u *fname = name_end + 1;
    char_u *fname_end = find_name_end(fname, عدم, عدم, FNE_CHECK_START);
    إذا (fname_end == fname)
	اذهب fail_after_eval;
    len = fname_end - fname;

    fp = method_lookup(cl, tv.v_type, fname, len, عدم);

    إذا (fp != عدم)
    {
	fp = eval_generic_func(fp, fname, &fname_end);
	إذا (fp != عدم)
	    *arg = fname_end;
    }

fail_after_eval:
    clear_tv(&tv);
    أرجع fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    صحيح
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحيح idx;
    class_member_lookup(cl, name, namelen, &idx);
    أرجع idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, صحيح *idx)
{
    ocmember_T	*ret_m = عدم;
    صحيح		ret_idx = -1;
    لكل (صحيح i = 0; i < cl->class_class_member_عدد; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	إذا (namelen)
	{
	    إذا (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		اكسر;
	    }
	}
	وإلا إذا (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    اكسر;
	}
    }
    إذا (idx != عدم)
	*idx = ret_idx;
    أرجع ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    ساكن ufunc_T *
class_method_lookup(class_T *cl, char_u *name, size_t namelen, صحيح *idx)
{
    ufunc_T	*ret_fp = عدم;
    صحيح		ret_idx = -1;
    لكل (صحيح i = 0; i < cl->class_class_function_عدد; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	إذا (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    اكسر;
	}
    }
    إذا (idx != عدم)
	*idx = ret_idx;
    أرجع ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    صحيح
class_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحيح idx;
    class_method_lookup(cl, name, namelen, &idx);
    أرجع idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ساكن صحيح
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحيح idx;
    object_member_lookup(cl, name, namelen, &idx);
    أرجع idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, صحيح *idx)
{
    ocmember_T	*ret_m = عدم;
    صحيح		ret_idx = -1;
    لكل (صحيح i = 0; i < cl->class_obj_member_عدد; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	إذا (namelen)
	{
	    إذا (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		اكسر;
	    }
	}
	وإلا إذا (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    اكسر;
	}
    }
    إذا (idx != عدم)
	*idx = ret_idx;
    أرجع ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    ساكن ufunc_T *
object_method_lookup(class_T *cl, char_u *name, size_t namelen, صحيح *idx)
{
    ufunc_T	*ret_fp = عدم;
    صحيح		ret_idx = -1;
    لكل (صحيح i = 0; i < cl->class_obj_method_عدد; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	إذا (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    اكسر;
	}
    }
    إذا (idx != عدم)
	*idx = ret_idx;
    أرجع ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    صحيح
object_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحيح idx;
    object_method_lookup(cl, name, namelen, &idx);
    أرجع idx;
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
    صحيح		*idx)
{
    إذا (v_type == VAR_CLASS)
	أرجع class_member_lookup(cl, name, namelen, idx);

    أرجع object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    ساكن class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_found = عدم;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_found = عدم;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    لكل (class_T *super = cl; super != عدم; super = super->class_extends)
    {
	class_T		*cl_tmp = عدم;
	ocmember_T	*m = عدم;
	إذا (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    إذا ((m = object_member_lookup(super, name, len, عدم)) != عدم)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	إذا (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    إذا (( m = class_member_lookup(super, name, len, عدم)) != عدم)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	إذا (cl_tmp == عدم)
	    اكسر;  // member is not in this or any super class.
	cl_found = cl_tmp;
	m_found = m;
    }
    إذا (p_m != عدم)
	*p_m = m_found;
    أرجع cl_found;
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
    صحيح		*idx)
{
    إذا (v_type == VAR_CLASS)
	أرجع class_method_lookup(cl, name, namelen, idx);
    وإلا
	أرجع object_method_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    صحيح
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    لكل (cctx_T *cctx = cctx_arg; cctx != عدم; cctx = cctx->ctx_outer)
	إذا (cctx->ctx_ufunc != عدم
			&& class_instance_of(cctx->ctx_ufunc->uf_class, cl))
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    صحيح
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    إذا (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_in_class_str),
		m->ocm_name.string, cl->class_name.string);
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    فراغ
obj_lock_const_vars(object_T *obj)
{
    لكل (صحيح i = 0; i < obj->obj_class->class_obj_member_عدد; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	إذا (ocm->ocm_flags & OCMFLAG_CONST)
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
    صحيح		sz;

    إذا (cl == عدم)
	أرجع عدم;

    sz = حجم(object_T) + cl->class_obj_member_عدد * حجم(typval_T);
    obj = alloc_clear(sz);
    إذا (obj == عدم)
	أرجع عدم;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    إذا (IS_ENUM(cl))
	enum_set_internal_obj_vars(cl, obj);

    أرجع obj;
}

/*
 * Make a copy of an object.
 */
    فراغ
copy_object(typval_T *from, typval_T *to)
{
    إذا (from->vval.v_object == عدم)
	to->vval.v_object = عدم;
    وإلا
    {
	to->vval.v_object = from->vval.v_object;
	++to->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    فراغ
copy_class(typval_T *from, typval_T *to)
{
    إذا (from->vval.v_class == عدم)
	to->vval.v_class = عدم;
    وإلا
    {
	to->vval.v_class = from->vval.v_class;
	++to->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    ساكن فراغ
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    لكل (صحيح i = 0; i < cl->class_interface_عدد; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	إذا (cl->class_interfaces_cl[i] != عدم)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *next;
    لكل (itf2class_T *i2c = cl->class_itf2class; i2c != عدم; i2c = next)
    {
	next = i2c->i2c_next;
	vim_free(i2c);
    }

    لكل (صحيح i = 0; i < cl->class_class_member_عدد; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	إذا (cl->class_members_tv != عدم)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    لكل (صحيح i = 0; i < cl->class_obj_member_عدد; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    لكل (صحيح i = 0; i < cl->class_class_function_عدد; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	إذا (IS_ENUM(cl) && IS_CONSTRUCTOR_METHOD(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    لكل (صحيح i = 0; i < cl->class_obj_method_عدد; ++i)
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
    ساكن صحيح
class_get_selfrefs(class_T *cl)
{
    صحيح		self_refs = 0;
    typval_T	*tv;

    لكل (صحيح i = 0; i < cl->class_class_member_عدد; ++i)
    {
	tv = &cl->class_members_tv[i];
	إذا (tv->v_type == VAR_OBJECT && tv->vval.v_object != عدم
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    أرجع self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    ساكن صحيح
can_free_enum(class_T *cl)
{
    لكل (صحيح i = 0; i < cl->class_class_member_عدد; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	إذا (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    أرجع TRUE;

	إذا (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    إذا (tv->vval.v_list->lv_refcount > 1)
		أرجع FALSE;
	    اكسر;
	}

	إذا (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    أرجع FALSE;
    }

    أرجع TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    ساكن صحيح
can_free_class(class_T *cl)
{
    صحيح		can_free = FALSE;
    صحيح		self_refs = 0;

    إذا (IS_ENUM(cl) && !can_free_enum(cl))
	أرجع FALSE;

    إذا (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    إذا (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    أرجع can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    فراغ
class_unref(class_T *cl)
{
    إذا (cl == عدم)
	أرجع;

    --cl->class_refcount;

    إذا (cl->class_name.string == عدم)
	أرجع;

    إذا (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    صحيح
class_free_nonref(صحيح copyID)
{
    صحيح		did_free = FALSE;

    لكل (class_T *cl = first_class; cl != عدم; cl = next_nonref_class)
    {
	next_nonref_class = cl->class_next_used;
	إذا ((cl->class_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    next_nonref_class = عدم;
    أرجع did_free;
}

    صحيح
set_ref_in_classes(صحيح copyID)
{
    لكل (class_T *cl = first_class; cl != عدم; cl = cl->class_next_used)
	set_ref_in_item_class(cl, copyID, عدم, عدم, عدم);

    أرجع FALSE;
}

ساكن object_T *first_object = عدم;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    فراغ
object_created(object_T *obj)
{
    إذا (first_object != عدم)
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
    ساكن فراغ
object_cleared(object_T *obj)
{
    إذا (obj->obj_next_used != عدم)
	obj->obj_next_used->obj_prev_used = obj->obj_prev_used;
    إذا (obj->obj_prev_used != عدم)
	obj->obj_prev_used->obj_next_used = obj->obj_next_used;
    وإلا إذا (first_object == obj)
	first_object = obj->obj_next_used;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    ساكن فراغ
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    إذا (!cl)
	أرجع;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = INT_MAX;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    لكل (صحيح i = 0; i < cl->class_obj_member_عدد; ++i)
	clear_tv(tv + i);
}

    ساكن فراغ
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    إذا (!cl)
	أرجع;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    ساكن فراغ
object_free(object_T *obj)
{
    إذا (in_free_unref_عناصر)
	أرجع;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    فراغ
object_unref(object_T *obj)
{
    إذا (obj != عدم && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    صحيح
object_free_nonref(صحيح copyID)
{
    صحيح		did_free = FALSE;

    لكل (object_T *obj = first_object; obj != عدم; obj = obj->obj_next_used)
    {
	إذا ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    أرجع did_free;
}

    فراغ
object_free_عناصر(صحيح copyID)
{
    object_T	*obj_next;

    لكل (object_T *obj = first_object; obj != عدم; obj = obj_next)
    {
	obj_next = obj->obj_next_used;
	إذا ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    فراغ
emsg_var_cl_define(حرف *رسالة, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    إذا (cl_def != عدم)
	semsg(_(رسالة), m->ocm_name.string, cl_def->class_name.string);
    وإلا
	emsg(_(e_internal_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    فراغ
method_not_found_رسالة(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *method_name = vim_strnsave(name, len);
    إذا ((v_type == VAR_OBJECT)
	    && (class_method_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	إذا (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	وإلا
	    semsg(_(e_class_method_str_accessible_only_using_class_str),
		    method_name, cl->class_name.string);
    }
    وإلا إذا ((v_type == VAR_CLASS)
	    && (object_method_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	إذا (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	وإلا
	    semsg(_(e_object_method_str_accessible_only_using_object_str),
		    method_name, cl->class_name.string);
    }
    وإلا
	semsg(_(e_method_not_found_on_class_str_str), method_name,
		cl->class_name.string);
    vim_free(method_name);
}

/*
 * Echo a class or object member not found message.
 */
    فراغ
member_not_found_رسالة(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    إذا (v_type == VAR_OBJECT)
    {
	إذا (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	وإلا
	    semsg(_(e_variable_not_found_on_object_str_str), varname,
		    cl->class_name.string);
    }
    وإلا
    {
	إذا (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	وإلا
	{
	    إذا (IS_ENUM(cl))
		semsg(_(e_enum_قيمة_str_not_found_in_enum_str),
			varname, cl->class_name.string);
	    وإلا
		semsg(_(e_class_variable_str_not_found_in_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    فراغ
defcompile_class(class_T *cl)
{
    لكل (صحيح loop = 1; loop <= 2; ++loop)
    {
	صحيح func_عدد = loop == 1 ? cl->class_class_function_عدد
						: cl->class_obj_method_عدد;
	لكل (صحيح i = 0; i < func_عدد; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    إذا (!IS_ABSTRACT_METHOD(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    فراغ
defcompile_classes_in_script(فراغ)
{
    لكل (class_T *cl = first_class; cl != عدم; cl = cl->class_next_used)
    {
	إذا (eval_variable(cl->class_name.string, 0, 0, عدم, عدم,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    صحيح
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    إذا (eval_variable(name, 0, 0, rettv, عدم, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	أرجع rettv->v_type == VAR_CLASS;
    أرجع FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    ساكن صحيح
object_call_builtin_method(
    object_T		*obj,
    class_builtin_T	builtin_method,
    صحيح			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    صحيح	    midx;

    إذا (obj == عدم)
	أرجع FAIL;

    uf = class_get_builtin_method(obj->obj_class, builtin_method, &midx);
    إذا (uf == عدم)
	أرجع FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    صحيح		r;

    إذا (fc == عدم)
	أرجع FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, عدم, obj, fc, rettv);

    remove_funccal();

    أرجع r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    صحيح
object_empty(object_T *obj)
{
    typval_T	rettv;

    إذا (object_call_builtin_method(obj, CLASS_BUILTIN_EMPTY, 0, عدم, &rettv)
								== FAIL)
	أرجع TRUE;

    أرجع tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    صحيح
object_len(object_T *obj)
{
    typval_T	rettv;

    إذا (object_call_builtin_method(obj, CLASS_BUILTIN_LEN, 0, عدم, &rettv)
								== FAIL)
	أرجع 0;

    أرجع tv_to_رقم(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    صحيح
object_equal(
	object_T *o1,
	object_T *o2,
	صحيح	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    إذا (o1 == o2)
	أرجع TRUE;
    إذا (o1 == عدم || o2 == عدم)
	أرجع FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    إذا (cl1 != cl2 || cl1 == عدم || cl2 == عدم)
	أرجع FALSE;

    لكل (صحيح i = 0; i < cl1->class_obj_member_عدد; ++i)
	إذا (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    أرجع FALSE;

    أرجع TRUE;
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
    صحيح		copyID,
    صحيح		echo_style,
    صحيح		restore_copyID,
    صحيح		composite_val)
{
    typval_T	rettv;

    إذا (object_call_builtin_method(obj, CLASS_BUILTIN_STRING, 0, عدم, &rettv)
								== OK
					&& rettv.vval.v_string != عدم)
	أرجع rettv.vval.v_string;

    صحيح		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    إذا (cl != عدم && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    وإلا
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	إذا (cl == عدم)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	وإلا
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
    }
    إذا (cl != عدم)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	لكل (صحيح i = 0; i < cl->class_obj_member_عدد; ++i)
	{
	    إذا (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.length);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = عدم;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, composite_val);
	    إذا (s != عدم)
		ga_concat(&ga, s);
	    vim_free(tf);
	    إذا (s == عدم || did_echo_string_emsg)
	    {
		ok = FAIL;
		اكسر;
	    }
	    سطر_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    إذا (ok == FAIL)
    {
	vim_free(ga.ga_data);
	أرجع عدم;
    }
    ga_append(&ga, NUL);
    أرجع (char_u *)ga.ga_data;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    صحيح
class_instance_of(class_T *cl, class_T *other_cl)
{
    إذا (cl == other_cl)
	أرجع TRUE;

    // Recursively check the base classes.
    لكل (; cl != عدم; cl = cl->class_extends)
    {
	إذا (cl == other_cl)
	    أرجع TRUE;
	// Check the implemented interfaces and the super interfaces
	لكل (صحيح i = cl->class_interface_عدد - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    طالما (intf != عدم)
	    {
		إذا (intf == other_cl)
		    أرجع TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    أرجع FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    فراغ
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_رقم = VVAL_FALSE;

    إذا (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_وسائط(argvars, 1) == FAIL)
	أرجع;

    إذا (object_tv->vval.v_object == عدم)
    {
	إذا (classinfo_tv->vval.v_class == عدم)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_رقم = VVAL_TRUE;
	أرجع;
    }

    لكل (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	إذا (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	وإلا
	    c = classinfo_tv->vval.v_class;

	إذا (class_instance_of(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_رقم = VVAL_TRUE;
	    أرجع;
	}
    }
}

#endif // FEAT_EVAL
