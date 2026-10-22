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
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

ایستا class_T *first_class = پوچ;
ایستا class_T *next_nonref_class = پوچ;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    ایستا تهی
class_created(class_T *cl)
{
    اگر (first_class != پوچ)
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
    ایستا تهی
class_cleared(class_T *cl)
{
    اگر (cl->class_next_used != پوچ)
	cl->class_next_used->class_prev_used = cl->class_prev_used;
    اگر (cl->class_prev_used != پوچ)
	cl->class_prev_used->class_next_used = cl->class_next_used;
    وگرنه اگر (first_class == cl)
	first_class = cl->class_next_used;

    // update the next class to check if needed
    اگر (cl == next_nonref_class)
	next_nonref_class = cl->class_next_used;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    ایستا صحیح
parse_member(
    exarg_T	*eap,
    char_u	*سطر,
    char_u	*varname,
    صحیح		has_public,	    // TRUE if "public" seen before "varname"
    char_u	**varname_end,
    صحیح		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_عبارت)
{
    *varname_end = to_name_end(varname, FALSE);
    اگر (*varname == '_' && has_public)
    {
	semsg(_(e_public_variable_name_cannot_start_with_underscore_str), سطر);
	بازگشت FAIL;
    }

    char_u *colon = skipwhite(*varname_end);
    char_u *type_arg = colon;
    type_T *type = پوچ;
    *has_type = FALSE;
    اگر (*colon == ':')
    {
	اگر (VIM_ISWHITE(**varname_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    بازگشت FAIL;
	}
	اگر (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    بازگشت FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, پوچ, پوچ, TRUE);
	اگر (type == پوچ || !valid_declaration_type(type))
	    بازگشت FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    اگر (type == پوچ && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	بازگشت FAIL;
    }

    اگر (init_عبارت == پوچ && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_in_interface));
	بازگشت FAIL;
    }

    اگر (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *عبارت_start, *عبارت_end;

	اگر (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    بازگشت FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(تهی)skip_عبارت_concatenate(&init_arg, &عبارت_start, &عبارت_end, &evalarg);

	init_arg = skipwhite(init_arg);
	اگر (*init_arg != NUL && !vim9_comment_start(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, پوچ);
	    بازگشت FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	اگر (type == پوچ)
	    type = &t_any;

	*init_عبارت = vim_strnsave(عبارت_start, عبارت_end - عبارت_start);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, پوچ);
    }
    وگرنه اگر (!valid_declaration_type(type))
	بازگشت FAIL;

    *type_ret = type;
    بازگشت OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    ایستا صحیح
add_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_end,
    صحیح		has_public,
    صحیح		has_final,
    صحیح		has_const,
    صحیح		has_type,
    type_T	*type,
    char_u	*init_عبارت)
{
    اگر (ga_grow(gap, 1) == FAIL)
	بازگشت FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_data) + gap->ga_len;
    m->ocm_name.length = (size_t)(varname_end - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.length);
    اگر (m->ocm_name.string == پوچ)
    {
	m->ocm_name.length = 0;
	بازگشت FAIL;
    }
    m->ocm_access = has_public ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_PRIVATE : VIM_ACCESS_READ;
    اگر (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    اگر (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    اگر (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    اگر (init_عبارت != پوچ)
    {
	m->ocm_init = init_عبارت;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    بازگشت OK;
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
    ایستا صحیح
add_members_to_class(
    garray_T	*gap,
    ocmember_T	*parent_members,
    صحیح		parent_شمار,
    ocmember_T	**members,
    صحیح		*member_شمار)
{
    *member_شمار = parent_شمار + gap->ga_len;
    *members = *member_شمار == 0 ? پوچ
				       : ALLOC_MULT(ocmember_T, *member_شمار);
    اگر (*member_شمار > 0 && *members == پوچ)
	بازگشت FAIL;
    برای (صحیح i = 0; i < parent_شمار; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = parent_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.length);
	اگر (m->ocm_name.string == پوچ)
	{
	    m->ocm_name.length = 0;
	    بازگشت FAIL;
	}
	اگر (m->ocm_init != پوچ)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    اگر (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + parent_شمار,
			       gap->ga_data, اندازه(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_data);
    بازگشت OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    صحیح
object_index_from_itf_index(class_T *itf, صحیح is_method, صحیح idx, class_T *cl)
{
    اگر (idx >= (is_method ? itf->class_obj_method_شمار
				   : itf->class_obj_member_شمار))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	بازگشت 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    اگر (cl == itf)
	بازگشت idx;

    itf2class_T		*i2c = پوچ;
    صحیح			searching = TRUE;
    صحیح			method_offset = 0;

    برای (class_T *super = cl; super != پوچ && searching;
						super = super->class_extends)
    {
	برای (i2c = itf->class_itf2class; i2c != پوچ; i2c = i2c->i2c_next)
	{
	    اگر (i2c->i2c_class == super && i2c->i2c_is_method == is_method)
	    {
		searching = FALSE;
		بشکن;
	    }
	}
	اگر (searching && is_method)
	    // The parent class methods are stored after the current class
	    // methods.
	    method_offset += super->class_obj_method_شمار_child;
    }
    اگر (i2c == پوچ)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	بازگشت 0;
    }

    // A table follows the i2c for the class
    صحیح *table = (صحیح *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    بازگشت table[idx] + method_offset;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    ایستا صحیح
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    صحیح	    is_class)
{
    typval_T	tv;
    صحیح		success = FALSE;

    اگر (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	بازگشت success;
    }

    tv.v_type = VAR_UNKNOWN;
    اگر (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_found_str), extends_name);
	بازگشت success;
    }

    اگر (tv.v_type != VAR_CLASS || tv.vval.v_class == پوچ
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    وگرنه
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    بازگشت success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    ایستا صحیح
validate_extends_generic_method(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    نویسه	*پیام = پوچ;

    اگر (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	بازگشت TRUE;

    اگر (super_fp->uf_generic_argcount == 0)
	پیام = e_concrete_method_str_override_with_generic_method_in_class_str;
    وگرنه اگر (cl_fp->uf_generic_argcount == 0)
	پیام = e_generic_method_str_override_with_concrete_method_in_class_str;
    وگرنه
	پیام = e_generic_method_str_type_arguments_mismatch_in_class_str;

    semsg(_(پیام), cl_fp->uf_name, super_cl->class_name.string);
    بازگشت FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    ایستا صحیح
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    صحیح		method_شمار = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_data);

    وقتی (super != پوچ)
    {
	صحیح extends_method_شمار = super->class_obj_method_شمار_child;
	اگر (extends_method_شمار == 0)
	{
	    super = super->class_extends;
	    ادامه;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	برای (صحیح i = 0; i < extends_method_شمار; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    صحیح	    extends_private = (*pstr == '_');
	    اگر (extends_private)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    برای (صحیح j = 0; j < method_شمار; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		صحیح	priv_method = (*qstr == '_');
		اگر (priv_method)
		    qstr++;
		اگر (STRCMP(pstr, qstr) == 0)
		{
		    اگر (priv_method != extends_private)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_method_str_of_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			بازگشت FALSE;
		    }

		    اگر (!validate_extends_generic_method(super,
						extends_methods[i], cl_fp[j]))
			    بازگشت FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    بازگشت TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    ایستا صحیح
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحیح member_شمار = objmembers_gap->ga_len;
    اگر (member_شمار == 0)
	بازگشت TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    برای (صحیح c_i = 0; c_i < member_شمار; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	وقتی (p_cl != پوچ)
	{
	    صحیح p_member_شمار = p_cl->class_obj_member_شمار;

	    اگر (p_member_شمار == 0)
	    {
		p_cl = p_cl->class_extends;
		ادامه;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    برای (صحیح p_i = 0; p_i < p_member_شمار; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		اگر (STRCMP(pstr, qstr) == 0)
		{
		    اگر (c_protected != p_protected)
			semsg(_(e_public_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    وگرنه
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    بازگشت FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    بازگشت TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    ایستا صحیح
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحیح member_شمار = objmembers_gap->ga_len;
    اگر (member_شمار == 0)
	بازگشت TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    برای (صحیح c_i = 0; c_i < member_شمار; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	صحیح	    var_found = FALSE;

	// Check in all the parent classes in the lineage
	وقتی (p_cl != پوچ && !var_found)
	{
	    صحیح p_member_شمار = p_cl->class_obj_member_شمار;
	    اگر (p_member_شمار == 0)
	    {
		p_cl = p_cl->class_extends;
		ادامه;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    برای (صحیح p_i = 0; p_i < p_member_شمار; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		اگر (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    ادامه;

		// Ensure the type is matching.
		where.wt_func_name = (نویسه *)c_m->ocm_name.string;
		where.wt_kind = WT_MEMBER;

		اگر (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    بازگشت FALSE;

		var_found = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    بازگشت TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    ایستا صحیح
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    برای (صحیح loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	صحیح extends_method_شمار = loop == 1
				? extends_cl->class_class_function_شمار
				: extends_cl->class_obj_method_شمار;
	اگر (extends_method_شمار == 0)
	    ادامه;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	صحیح method_شمار = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_data
						: objmethods_gap->ga_data);

	برای (صحیح i = 0; i < extends_method_شمار; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    اگر (!IS_ABSTRACT_METHOD(uf))
		ادامه;

	    صحیح	concrete_method_found = FALSE;
	    صحیح	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    برای (j = 0; !concrete_method_found && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		اگر (!IS_ABSTRACT_METHOD(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_method_found = TRUE;
	    }

	    اگر (concrete_method_found)
		ادامه;

	    برای (j = 0; j < method_شمار; j++)
	    {
		اگر (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_method_found = TRUE;
		    بشکن;
		}
	    }

	    اگر (!concrete_method_found)
	    {
		semsg(_(e_abstract_method_str_not_implemented), uf->uf_name);
		بازگشت FALSE;
	    }
	}
    }

    بازگشت TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    ایستا صحیح
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    صحیح		cl_member_شمار,
    class_T	*extends_cl)
{
    صحیح		variable_present  = FALSE;

    برای (صحیح cl_i = 0; cl_i < cl_member_شمار; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	اگر (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    ادامه;

	// Ensure the access type is same
	اگر (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_of_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    بازگشت FALSE;
	}

	// Ensure the type is matching.
	اگر (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	وگرنه
	{
	    where.wt_func_name = (نویسه *)m->ocm_name.string;
	    where.wt_kind = WT_MEMBER;
	    اگر (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		بازگشت FALSE;
	}

	variable_present = TRUE;
	بشکن;
    }

    اگر (!variable_present && extends_cl != پوچ)
    {
	صحیح ext_cl_شمار = extends_cl->class_obj_member_شمار;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	بازگشت intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_شمار,
					extends_cl->class_extends);
    }

    بازگشت variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    ایستا صحیح
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحیح if_شمار = ifcl->class_obj_member_شمار;
    اگر (if_شمار == 0)
	بازگشت TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_data);
    صحیح cl_شمار = objmembers_gap->ga_len;
    برای (صحیح if_i = 0; if_i < if_شمار; ++if_i)
    {
	اگر (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_شمار, extends_cl))
	{
	    semsg(_(e_variable_str_of_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    بازگشت FALSE;
	}
    }

    بازگشت TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    ایستا صحیح
intf_method_type_matches(
    ufunc_T	*if_method,
    ufunc_T	*cl_method,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    اگر (cl_method->uf_generic_argcount != if_method->uf_generic_argcount)
    {
	semsg(_(e_type_var_شمار_of_method_str_differs_from_interface_str),
						cl_method->uf_name, intf_name);
	بازگشت FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (نویسه *)if_method->uf_name;
    where.wt_kind = WT_METHOD;
    اگر (check_type(if_method->uf_func_type, cl_method->uf_func_type, TRUE,
								where) == FAIL)
	بازگشت FALSE;

    بازگشت TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    ایستا صحیح
intf_method_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    صحیح	    cl_شمار,
    class_T *extends_cl,
    char_u  *intf_name)
{
    صحیح		method_present  = FALSE;

    برای (صحیح cl_i = 0; cl_i < cl_شمار; ++cl_i)
    {
	اگر (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    اگر (!intf_method_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		بازگشت FALSE;
	    method_present = TRUE;
	    بشکن;
	}
    }

    اگر (!method_present && extends_cl != پوچ)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	صحیح	ext_cl_شمار = extends_cl->class_obj_method_شمار;
	بازگشت intf_method_present(if_ufunc, ext_cl_fp, ext_cl_شمار,
					extends_cl->class_extends, intf_name);
    }

    بازگشت method_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    ایستا صحیح
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    صحیح if_شمار = ifcl->class_obj_method_شمار;
    اگر (if_شمار == 0)
	بازگشت TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_data);
    صحیح cl_شمار = objmethods_gap->ga_len;
    برای (صحیح if_i = 0; if_i < if_شمار; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	اگر (!intf_method_present(if_fp[if_i], cl_fp, cl_شمار, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_method_str_of_interface_str_not_implemented),
		    if_name, intf_class_name);
	    بازگشت FALSE;
	}
    }

    بازگشت TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    ایستا صحیح
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحیح		success = TRUE;

    برای (صحیح i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_data)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	اگر (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_found_str), impl);
	    success = FALSE;
	    بشکن;
	}

	اگر (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == پوچ
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    بشکن;
	}

	class_T *ifcl = tv.vval.v_class;
	اگر (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    بشکن;
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

    اگر (!success)
	بازگشت FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_data;
    cl->class_interface_شمار = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    برای (صحیح i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_data)[i],
		((class_T **)intf_classes_gap->ga_data)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = پوچ;
    cl->class_interface_شمار = 0;
    cl->class_extends = پوچ;

    بازگشت success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    ایستا صحیح
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    برای (صحیح j = 0; j < intf_classes_gap->ga_len; j++)
    {
	اگر (((class_T **)intf_classes_gap->ga_data)[j] == ifcl)
	    بازگشت TRUE;
    }

    بازگشت FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    ایستا صحیح
add_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.length);
    اگر (intf_name == پوچ)
	بازگشت FALSE;

    اگر (ga_grow(impl_gap, 1) == FAIL)
	برو fail;

    char_u **intf_names = (char_u **)impl_gap->ga_data;
    intf_names[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    اگر (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	برو fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_data;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    بازگشت TRUE;
fail:
    vim_free(intf_name);
    بازگشت FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    ایستا صحیح
add_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    برای (صحیح i = 0; i < super->class_interface_شمار; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	اگر (!is_interface_class_present(intf_classes_gap, ifcl))
	    add_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    بازگشت TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    ایستا صحیح
check_func_arg_names(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    برای (صحیح loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	برای (صحیح fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_data)[fi];

	    برای (صحیح i = 0; i < uf->uf_args.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_args.ga_data)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		برای (صحیح mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_data + mi)->ocm_name.string;
		    اگر (STRCMP(aname, mname) == 0)
		    {
			اگر (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_argument_already_declared_in_class_str),
				aname);

			بازگشت FALSE;
		    }
		}
	    }
	}
    }

    بازگشت TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    ایستا صحیح
is_reserved_varname(char_u *varname, char_u *varname_end)
{
    صحیح reserved = FALSE;
    char_u save_varname_end = *varname_end;
    *varname_end = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_end = save_varname_end;

    بازگشت reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    ایستا صحیح
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_end)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_end - varname)};
    صحیح		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    اگر (*pstr.string == '_')
    {
	pstr.string++;
	pstr.length--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    برای (صحیح loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	برای (صحیح i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_data) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.length};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    اگر (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.length--;
		q_protected = true;
	    }

	    اگر (pstr.length == qstr.length
		&& STRNCMP(pstr.string, qstr.string, pstr.length) == 0)
	    {
		char_u	save_c = *varname_end;

		*varname_end = NUL;
		اگر (p_protected != q_protected)
		    semsg(_(e_public_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		وگرنه
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_end = save_c;
		dup = TRUE;
		بشکن;
	    }
	}
    }

    بازگشت dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    ایستا صحیح
is_duplicate_method(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    برای (صحیح loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	برای (صحیح i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_data)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    اگر (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		بازگشت TRUE;
	    }
	}
    }

    بازگشت FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    ایستا صحیح
is_valid_constructor(ufunc_T *uf, صحیح is_abstract, صحیح has_static)
{
    // Constructors are not allowed in abstract classes.
    اگر (is_abstract)
    {
	emsg(_(e_cannot_define_new_method_in_abstract_class));
	بازگشت FALSE;
    }
    // A constructor is always static, no need to define it so.
    اگر (has_static)
    {
	emsg(_(e_cannot_define_new_method_as_static));
	بازگشت FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    اگر (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_method));
	بازگشت FALSE;
    }
    بازگشت TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    ایستا صحیح
object_check_builtin_method_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    صحیح	    valid = FALSE;
    type_T  method_sig;
    type_T  method_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_FIELD(method_sig);
    CLEAR_FIELD(method_rt);
    method_sig.tt_type = VAR_FUNC;

    اگر (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	method_rt.tt_type = VAR_عدد;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    وگرنه اگر (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	method_rt.tt_type = VAR_BOOL;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    وگرنه اگر (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	method_rt.tt_type = VAR_STRING;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    وگرنه
	semsg(_(e_builtin_object_method_str_not_supported), uf->uf_name);

    where.wt_func_name = (نویسه *)uf->uf_name;
    where.wt_kind = WT_METHOD;
    اگر (valid && !check_type(&method_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    بازگشت valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    صحیح
is_valid_builtin_obj_methodname(char_u *funcname)
{
    انتخاب (funcname[0])
    {
	حالت 'e':
	    بازگشت STRNCMP(funcname, "empty", 5) == 0;

	حالت 'l':
	    بازگشت STRNCMP(funcname, "len", 3) == 0;

	حالت 'n':
	    بازگشت STRNCMP(funcname, "new", 3) == 0;

	حالت 's':
	    بازگشت STRNCMP(funcname, "string", 6) == 0;
    }

    بازگشت FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_method(
    class_T		*cl,
    class_builtin_T	builtin_method,
    صحیح			*method_idx)
{
    *method_idx = -1;

    اگر (cl == پوچ)
	بازگشت پوچ;

    *method_idx = cl->class_builtin_methods[builtin_method];
    بازگشت *method_idx != -1 ? cl->class_obj_methods[*method_idx] : پوچ;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    ایستا صحیح
update_member_method_lookup_table(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    صحیح		pobj_method_offset)
{
    اگر (ifcl == پوچ)
	بازگشت OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(اندازه(itf2class_T)
				+ ifcl->class_obj_member_شمار * اندازه(صحیح));
    اگر (if2cl == پوچ)
	بازگشت FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = FALSE;

    برای (صحیح if_i = 0; if_i < ifcl->class_obj_member_شمار; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	برای (صحیح cl_i = 0; cl_i < cl->class_obj_member_شمار; ++cl_i)
	{
	    اگر (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		صحیح *table = (صحیح *)(if2cl + 1);
		table[if_i] = cl_i;
		بشکن;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(اندازه(itf2class_T)
				+ ifcl->class_obj_method_شمار * اندازه(صحیح));
    اگر (if2cl == پوچ)
	بازگشت FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = TRUE;

    برای (صحیح if_i = 0; if_i < ifcl->class_obj_method_شمار; ++if_i)
    {
	صحیح	done = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	برای (صحیح cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    اگر (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_data)[cl_i]->uf_name) == 0)
	    {
		صحیح *table = (صحیح *)(if2cl + 1);
		table[if_i] = cl_i;
		done = TRUE;
		بشکن;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	اگر (!done)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    اگر (cl->class_extends != ifcl)
	    {
		class_T		*parent = cl->class_extends;
		صحیح		method_offset = objmethods->ga_len;

		وقتی (!done && parent != پوچ && parent != ifcl)
		{
		    برای (صحیح cl_i = 0;
			    cl_i < parent->class_obj_method_شمار_child; ++cl_i)
		    {
			اگر (STRCMP(if_name,
				    parent->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    صحیح *table = (صحیح *)(if2cl + 1);
			    table[if_i] = method_offset + cl_i;
			    done = TRUE;
			    بشکن;
			}
		    }
		    method_offset += parent->class_obj_method_شمار_child;
		    parent = parent->class_extends;
		}
	    }

	    اگر (!done)
	    {
		صحیح *table = (صحیح *)(if2cl + 1);
		table[if_i] = pobj_method_offset + if_i;
	    }
	}
    }

    بازگشت OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    ایستا صحیح
add_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    برای (صحیح i = 0; i < cl->class_interface_شمار; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	وقتی (ifcl != پوچ)
	{
	    اگر (update_member_method_lookup_table(ifcl, cl, objmethods_gap,
								0) == FAIL)
		بازگشت FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    اگر (extends_cl != پوچ)
    {
	class_T		*pclass = extends_cl;
	صحیح		pobj_method_offset = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	وقتی (pclass != پوچ)
	{
	    اگر (update_member_method_lookup_table(pclass, cl,
			objmethods_gap, pobj_method_offset) == FAIL)
		بازگشت FAIL;

	    pobj_method_offset += pclass->class_obj_method_شمار_child;
	    pclass = pclass->class_extends;
	}
    }

    بازگشت OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    ایستا صحیح
add_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_شمار);
    اگر (cl->class_members_tv == پوچ)
	بازگشت FAIL;

    برای (صحیح i = 0; i < cl->class_class_member_شمار; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	اگر (m->ocm_init != پوچ)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_عبارت(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    اگر (etv == پوچ)
		بازگشت FAIL;

	    اگر (m->ocm_type->tt_type == VAR_ANY
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
	وگرنه
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = پوچ;
	}
	set_tv_type(tv, m->ocm_type);
	اگر (m->ocm_flags & OCMFLAG_CONST)
	    item_lock(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    بازگشت OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    ایستا تهی
add_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    صحیح		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    برای (صحیح i = 0; i < cl->class_obj_member_شمار; ++i)
    {
	اگر (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    ادامه;

	اگر (i > (is_enum ? 2 : 0))
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

    garray_T سطرها_to_free;
    ga_init2(&سطرها_to_free, اندازه(char_u *), 50);

    ufunc_T *nf = define_function(&fea, پوچ, &سطرها_to_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_شمار,
			    پوچ);

    ga_clear_strings(&سطرها_to_free);
    vim_free(fga.ga_data);

    اگر (nf != پوچ && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_data)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	اگر (nf->uf_ret_type != پوچ)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_args = پوچ;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ایستا صحیح
add_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    برای (صحیح loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	صحیح		*fcount = loop == 1 ? &cl->class_class_function_شمار
						: &cl->class_obj_method_شمار;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	صحیح parent_شمار = 0;
	اگر (extends_cl != پوچ)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    parent_شمار = loop == 1
				? 0
				: extends_cl->class_obj_method_شمار;

	*fcount = parent_شمار + gap->ga_len;
	اگر (*fcount == 0)
	{
	    *fup = پوچ;
	    ادامه;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	اگر (*fup == پوچ)
	    بازگشت FAIL;

	اگر (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_data, اندازه(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_data);
	اگر (loop == 1)
	    cl->class_class_function_شمار_child = gap->ga_len;
	وگرنه
	    cl->class_obj_method_شمار_child = gap->ga_len;

	اگر (loop == 2)
	{
	    // Copy instance methods from the parent.

	    برای (صحیح i = 0; i < parent_شمار; ++i)
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
		اگر (nf != پوچ && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_args != پوچ)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		برای (صحیح ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    اگر (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (نویسه *)pname;
			where.wt_kind = WT_METHOD;
			(تهی)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	برای (صحیح i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    اگر (i < gap->ga_len)
		fp->uf_defclass = cl;
	    اگر (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    بازگشت OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    ایستا تهی
update_builtin_method_index(class_T *cl)
{
    صحیح	i;

    برای (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    برای (i = 0; i < cl->class_obj_method_شمار; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	اگر (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	وگرنه اگر (cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] = i;
	وگرنه اگر (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    ایستا char_u *
find_class_name_end(char_u *arg)
{
    char_u *end = arg;

    وقتی (ASCII_ISALNUM(*end) || *end == '_'
	    || (*end == '.' && (ASCII_ISALNUM(end[1]) || end[1] == '_')))
	++end;

    بازگشت end;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    ایستا صحیح
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
    صحیح		dup = FALSE;

    برای (صحیح i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_data) + i;

	اگر (name.length == m->ocm_name.length
	    && STRNCMP(name.string, m->ocm_name.string, name.length) == 0)
	{
	    char_u  save_c = *varname_end;

	    *varname_end = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_end = save_c;
	    dup = TRUE;
	    بشکن;
	}
    }

    بازگشت dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    ایستا صحیح
enum_parse_مقادیر(
    exarg_T	*eap,
    class_T	*en,
    char_u	*سطر,
    garray_T	*gap,
    صحیح		*رقم_enum_مقادیر,
    صحیح		*enum_end)
{
    evalarg_T	evalarg;
    char_u	*p = سطر;
    نویسه	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	last_char = NUL;
    صحیح		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    صحیح		did_emsg_before = did_emsg;
    وقتی (*p != NUL)
    {
	// ignore comment
	اگر (*p == '#')
	    بشکن;

	اگر (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_مقدار_declaration_str), p);
	    بشکن;
	}

	char_u *eni_name_start = p;
	char_u *eni_name_end = to_name_end(p, FALSE);

	اگر (is_duplicate_enum(gap, eni_name_start, eni_name_end))
	    بشکن;

	p = skipwhite(eni_name_end);

	char_u	*init_عبارت = پوچ;
	اگر (*p == '(' || *p == '<')
	{
	    اگر (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", سطر);
		بشکن;
	    }

	    char_u *عبارت_start, *عبارت_end;

	    p = eni_name_start;
	    (تهی)skip_عبارت_concatenate(&p, &عبارت_start, &عبارت_end, &evalarg);

	    وقتی (*عبارت_start && *عبارت_start != '(' && *عبارت_start != '<')
		عبارت_start++;

	    اگر (عبارت_end > عبارت_start)
		init_عبارت = vim_strnsave(عبارت_start, عبارت_end - عبارت_start);
	}

	اگر (init_عبارت == پوچ)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, اندازه(initexpr_buf), "%s.new()", en->class_name.string);
	وگرنه
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, اندازه(initexpr_buf), "%s.new%s", en->class_name.string,
		init_عبارت);
	    vim_free(init_عبارت);
	}
	اگر (add_member(gap, eni_name_start, eni_name_end, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    بشکن;

	++*رقم_enum_مقادیر;

	اگر (*p != '#')
	    last_char = *p;

	اگر (*p != NUL && *p != ',')
	    بشکن;

	اگر (*p == ',')
	{
	    اگر (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", سطر);
		بشکن;
	    }
	    اگر (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", سطر);
		بشکن;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    اگر (*p != NUL && *p != '#')
    {
	اگر (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_argument_str), p);
	rc = FAIL;
    }

    اگر (last_char != ',')
	// last enum value should not be terminated by ","
	*enum_end = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, پوچ);

    بازگشت rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    ایستا صحیح
enum_add_مقادیر_member(
    class_T	*en,
    garray_T	*gap,
    صحیح		رقم_enum_مقادیر,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    صحیح		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    برای (صحیح i = 0; i < رقم_enum_مقادیر; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;

	اگر (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.length);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    اگر (type == پوچ)
	برو done;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    اگر (type->tt_member != پوچ)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = add_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_data, fga.ga_len));

done:
    vim_free(fga.ga_data);

    بازگشت rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    ایستا تهی
enum_clear_constructors(class_T *en)
{
    برای (صحیح i = 0; i < en->class_class_function_شمار; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	اگر (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    تهی
enum_set_internal_obj_vars(class_T *en, object_T *enval)
{
    صحیح	i;

    برای (i = 0; i < en->class_class_member_شمار; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	اگر (en_tv != پوچ && en_tv->v_type == VAR_UNKNOWN)
	    بشکن;
    }

    اگر (i >= en->class_class_member_شمار)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	بازگشت;

    // First object variable is the name
    ocmember_T *مقدار_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(مقدار_ocm->ocm_name.string, مقدار_ocm->ocm_name.length);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_عدد;
    ord_tv->vval.v_عدد = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    ایستا تهی
skip_class_body(exarg_T *eap)
{
    برای (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	اگر (theline == پوچ)
	    بشکن;

	char_u	*p = theline;
	صحیح	found;

	// skip ':' and blanks
	برای (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	found = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	اگر (found)
	    بشکن;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    تهی
ex_class(exarg_T *eap)
{
    صحیح		is_class = eap->cmdidx == CMD_class;
    صحیح		is_abstract = eap->cmdidx == CMD_abstract;
    صحیح		is_enum = eap->cmdidx == CMD_enum;
    صحیح		added_enum_مقادیر = FALSE;
    صحیح		is_interface;
    long	start_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    اگر (eap->skip)
    {
	skip_class_body(eap);
	بازگشت;
    }

    اگر (is_abstract)
    {
	اگر (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_argument_str), arg);
	    بازگشت;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    اگر (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	اگر (is_class)
	    emsg(_(e_class_can_only_be_defined_in_vim9_script));
	وگرنه اگر (is_enum)
	    emsg(_(e_enum_can_only_be_defined_in_vim9_script));
	وگرنه
	    emsg(_(e_interface_can_only_be_defined_in_vim9_script));
	بازگشت;
    }

    اگر (!ASCII_ISUPPER(*arg))
    {
	اگر (is_class)
	    semsg(_(e_class_name_must_start_with_uppercase_letter_str), arg);
	وگرنه اگر (is_enum)
	    semsg(_(e_enum_name_must_start_with_uppercase_letter_str), arg);
	وگرنه
	    semsg(_(e_interface_name_must_start_with_uppercase_letter_str),
									  arg);
	بازگشت;
    }
    char_u *name_end = find_name_end(arg, پوچ, پوچ, FNE_CHECK_START);
    اگر (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	بازگشت;
    }
    char_u *name_start = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = پوچ;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, اندازه(char_u *), 5);

    arg = skipwhite(name_end);
    وقتی (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	اگر (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    اگر (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		برو early_ret;
	    }
	    اگر (extends != پوچ)
	    {
		emsg(_(e_duplicate_extends));
		برو early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *end = find_class_name_end(arg);
	    اگر (!IS_WHITE_OR_NUL(*end))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		برو early_ret;
	    }
	    extends = vim_strnsave(arg, end - arg);
	    اگر (extends == پوچ)
		برو early_ret;

	    arg = skipwhite(end + 1);
	}
	وگرنه اگر (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    اگر (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		برو early_ret;
	    }

	    اگر (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		برو early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    برای (;;)
	    {
		char_u *impl_end = find_class_name_end(arg);
		اگر ((!IS_WHITE_OR_NUL(*impl_end) && *impl_end != ',')
			|| (*impl_end == ','
			    && !IS_WHITE_OR_NUL(*(impl_end + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    برو early_ret;
		}
		اگر (impl_end - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    برو early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_end - arg);
		اگر (iname == پوچ)
		    برو early_ret;
		برای (صحیح i = 0; i < ga_impl.ga_len; ++i)
		    اگر (STRCMP(((char_u **)ga_impl.ga_data)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			برو early_ret;
		    }
		اگر (ga_add_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    برو early_ret;
		}
		اگر (*impl_end != ',')
		{
		    arg = skipwhite(impl_end);
		    بشکن;
		}
		arg = skipwhite(impl_end + 1);
	    }
	}
	وگرنه
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_strings(&ga_impl);
	    بازگشت;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, اندازه(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, اندازه(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, اندازه(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, اندازه(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, اندازه(ufunc_T *), 10);

    class_T *cl = پوچ;
    class_T *extends_cl = پوچ;  // class from "extends" argument
    class_T **intf_classes = پوچ;
    صحیح	    intf_شمار = 0;	// number of items in "intf_classes"
    صحیح	    رقم_enum_مقادیر = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    اگر (cl == پوچ)
	برو cleanup;

    اگر (is_enum)
	cl->class_flags = CLASS_ENUM;
    وگرنه اگر (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    وگرنه اگر (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.length = (size_t)(name_end - name_start);
    cl->class_name.string = vim_strnsave(name_start, cl->class_name.length);
    اگر (cl->class_name.string == پوچ)
	برو cleanup;

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
    صحیح rc = set_var_const(cl->class_name.string, 0, پوچ, &tv, FALSE, 0, 0);
    اگر (rc == FAIL)
	برو cleanup;

    اگر (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	اگر (add_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, پوچ) == FAIL)
	    برو cleanup;

	varname = (char_u *)"ordinal";
	اگر (add_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_عدد, پوچ) == FAIL)
	    برو cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    صحیح		enum_end = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = پوچ;
    صحیح success = FALSE;
    برای (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	اگر (theline == پوچ)
	    بشکن;
	char_u *سطر = skipwhite(theline);

	// Skip empty and comment lines.
	اگر (*سطر == NUL)
	    ادامه;
	اگر (*سطر == '#')
	{
	    اگر (vim9_bad_comment(سطر))
		بشکن;
	    ادامه;
	}

	char_u *p = سطر;

	نویسه	*end_name;
	صحیح	shortlen;
	صحیح	fullen;
	اگر (is_class)
	{
	    end_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	وگرنه اگر (is_enum)
	{
	    end_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	وگرنه
	{
	    end_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	برای (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_start = p;
	اگر (checkforcmd(&p, end_name, shortlen))
	{
	    اگر (STRNCMP(cmd_start, end_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_start);
	    وگرنه اگر (*p == '|' || !ends_excmd2(سطر, p))
		semsg(_(e_trailing_characters_str), p);
	    وگرنه
		success = TRUE;
	    بشکن;
	}

	صحیح	wrong_endname = FALSE;
	اگر (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	وگرنه اگر (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	وگرنه
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	اگر (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), سطر, end_name);
	    بشکن;
	}

	اگر (is_enum && !enum_end)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    اگر (enum_parse_مقادیر(eap, cl, سطر, &classmembers,
					&رقم_enum_مقادیر, &enum_end) == FAIL)
		بشکن;

	    اگر (enum_end)
	    {
		// Add the enum "values" class variable.
		enum_add_مقادیر_member(cl, &classmembers, رقم_enum_مقادیر,
							&type_list);
		added_enum_مقادیر = TRUE;
	    }
	    ادامه;
	}

	صحیح has_public = FALSE;
	اگر (checkforcmd(&p, "public", 3))
	{
	    اگر (STRNCMP(سطر, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), سطر);
		بشکن;
	    }
	    اگر (is_interface)
	    {
		emsg(_(e_public_variable_not_supported_in_interface));
		بشکن;
	    }
	    has_public = TRUE;
	    p = skipwhite(سطر + 6);

	    اگر (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_public_keyword_not_supported_for_method));
		بشکن;
	    }

	    اگر (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_public_must_be_followed_by_var_static_final_or_const));
		بشکن;
	    }
	}

	صحیح abstract_method = FALSE;
	char_u *pa = p;
	اگر (checkforcmd(&p, "abstract", 3))
	{
	    اگر (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		بشکن;
	    }

	    اگر (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_used_in_enum));
		بشکن;
	    }

	    اگر (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_used_in_interface));
		بشکن;
	    }

	    اگر (!is_abstract)
	    {
		semsg(_(e_abstract_method_in_concrete_class), pa);
		بشکن;
	    }

	    p = skipwhite(pa + 8);
	    اگر (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		بشکن;
	    }

	    abstract_method = TRUE;
	}

	صحیح has_static = FALSE;
	char_u *ps = p;
	اگر (checkforcmd(&p, "static", 4))
	{
	    اگر (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		بشکن;
	    }

	    اگر (is_interface)
	    {
		emsg(_(e_static_member_not_supported_in_interface));
		بشکن;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    اگر (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		بشکن;
	    }
	}

	صحیح has_final = FALSE;
	صحیح has_var = FALSE;
	صحیح has_const = FALSE;
	اگر (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	وگرنه اگر (checkforcmd(&p, "final", 5))
	{
	    اگر (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_in_interface));
		بشکن;
	    }
	    has_final = TRUE;
	}
	وگرنه اگر (checkforcmd(&p, "const", 5))
	{
	    اگر (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_in_interface));
		بشکن;
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
	اگر (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_end = پوچ;
	    type_T *type = پوچ;
	    char_u *init_عبارت = پوچ;
	    صحیح	    has_type = FALSE;

	    اگر (!eval_isnamec1(*p))
	    {
		اگر (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), سطر);
		وگرنه
		    semsg(_(e_invalid_object_variable_declaration_str), سطر);
		بشکن;
	    }

	    اگر (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_in_interface),
			varname);
		بشکن;
	    }

	    اگر (parse_member(eap, سطر, varname, has_public,
			  &varname_end, &has_type, &type_list, &type,
			  !is_interface ? &init_عبارت: پوچ) == FAIL)
		بشکن;

	    اگر (is_reserved_varname(varname, varname_end)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_end))
	    {
		vim_free(init_عبارت);
		بشکن;
	    }
	    اگر (add_member(has_static ? &classmembers : &objmembers, varname,
				varname_end, has_public, has_final, has_const,
				has_type, type, init_عبارت) == FAIL)
	    {
		vim_free(init_عبارت);
		بشکن;
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
	وگرنه اگر (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	سطرها_to_free;
	    صحیح		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    اگر (has_public)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_public_keyword_not_supported_for_method));
		بشکن;
	    }

	    اگر (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_in_class_str), سطر);
		بشکن;
	    }

	    اگر (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_method_not_supported_in_interface), p);
		بشکن;
	    }

	    اگر (has_static && !is_new && SAFE_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_method_not_supported), p);
		بشکن;
	    }

	    CLEAR_FIELD(ea);
	    ea.cmd = سطر;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&سطرها_to_free, اندازه(char_u *), 50);
	    صحیح class_flags;
	    اگر (is_interface)
		class_flags = CF_INTERFACE;
	    وگرنه
		class_flags = abstract_method ? CF_ABSTRACT_METHOD : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, پوچ, &سطرها_to_free,
			class_flags, objmembers.ga_data, objmembers.ga_len,
			پوچ);
	    ga_clear_strings(&سطرها_to_free);

	    اگر (uf != پوچ)
	    {
		اگر (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_method_not_supported_in_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    بشکن;
		}

		// check for builtin method
		اگر (!is_new && SAFE_islower(*uf->uf_name) &&
					!object_check_builtin_method_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    بشکن;
		}

		// Check the uf_name isn't used already.
		اگر (is_duplicate_method(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    بشکن;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		اگر (ga_grow(fgap, 1) == OK)
		{
		    اگر (is_new)
			uf->uf_flags |= FC_NEW;

		    اگر (abstract_method)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_data)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	وگرنه
	{
	    اگر (is_class)
		semsg(_(e_not_valid_command_in_class_str), سطر);
	    وگرنه اگر (is_enum)
		semsg(_(e_not_valid_command_in_enum_str), سطر);
	    وگرنه
		semsg(_(e_not_valid_command_in_interface_str), سطر);
	    بشکن;
	}
    }

    اگر (theline == پوچ && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    اگر (success && is_enum && (رقم_enum_مقادیر == 0 || !added_enum_مقادیر))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_add_مقادیر_member(cl, &classmembers, رقم_enum_مقادیر,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    اگر (success && extends != پوچ)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    اگر (success && extends_cl != پوچ)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    اگر (success && extends_cl != پوچ)
    {
	اگر (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	وگرنه
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    اگر (success && !is_abstract && extends_cl != پوچ
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, اندازه(class_T *), 5);

    اگر (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    اگر (success && extends_cl != پوچ)
	success = add_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_data;
    intf_شمار = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    اگر (success)
	success = check_func_arg_names(&classfunctions, &objmethods,
							&classmembers);

    اگر (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	اگر (extends_cl != پوچ)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	اگر (add_members_to_class(&classmembers,
				 پوچ,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_شمار) == FAIL
		|| add_members_to_class(&objmembers,
				 extends_cl == پوچ ? پوچ
					       : extends_cl->class_obj_members,
				 extends_cl == پوچ ? 0
					  : extends_cl->class_obj_member_شمار,
				 &cl->class_obj_members,
				 &cl->class_obj_member_شمار) == FAIL)
	    برو cleanup;

	اگر (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_شمار = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    اگر (cl->class_interfaces == پوچ)
		برو cleanup;
	    برای (صحیح i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_data)[i];
	    VIM_CLEAR(ga_impl.ga_data);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = پوچ;
	}

	اگر (cl->class_interface_شمار > 0 || extends_cl != پوچ)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    اگر (add_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		برو cleanup;
	}

	صحیح	have_new = FALSE;
	ufunc_T	*class_func = پوچ;
	برای (صحیح i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_data)[i];
	    اگر (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		بشکن;
	    }
	}

	اگر (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	وگرنه اگر ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    add_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	اگر (add_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    برو cleanup;

	update_builtin_method_index(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	اگر ((is_class || is_enum) && cl->class_class_member_شمار > 0)
	    اگر (add_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		بازگشت;
	    }

	cl->class_type_list = type_list;

	اگر (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	بازگشت;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    اگر (intf_classes != پوچ)
    {
	برای (صحیح i = 0; i < intf_شمار; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_strings(&ga_impl);

    برای (صحیح round = 1; round <= 2; ++round)
    {
	garray_T *gap = round == 1 ? &classmembers : &objmembers;
	اگر (gap->ga_len == 0 || gap->ga_data == پوچ)
	    ادامه;

	برای (صحیح i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    برای (صحیح i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    برای (صحیح i = 0; i < classfunctions.ga_len; ++i)
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
    صحیح		is_object,
    char_u	*name,
    char_u	*name_end,
    صحیح		*member_idx)
{
    size_t	len = name_end - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    اگر (m == پوچ)
    {
	member_not_found_پیام(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	بازگشت &t_any;
    }

    بازگشت m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    صحیح		is_object,
    صحیح		member_idx)
{
    ocmember_T	*m;
    صحیح		member_شمار;

    اگر (is_object)
    {
	m = cl->class_obj_members;
	member_شمار = cl->class_obj_member_شمار;
    }
    وگرنه
    {
	m = cl->class_class_members;
	member_شمار = cl->class_class_member_شمار;
    }

    اگر (member_idx >= member_شمار)
	بازگشت پوچ;

    بازگشت m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    ایستا تهی
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    تهی
typealias_unref(typealias_T *ta)
{
    اگر (ta != پوچ && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    تهی
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    اگر (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_in_vim9_script));
	بازگشت;
    }

    اگر (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	بازگشت;
    }

    اگر (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_start_with_uppercase_letter_str), arg);
	بازگشت;
    }

    char_u *name_end = find_name_end(arg, پوچ, پوچ, FNE_CHECK_START);
    اگر (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	بازگشت;
    }
    char_u *name_start = arg;

    arg = skipwhite(name_end);
    اگر (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	بازگشت;
    }
    اگر (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	بازگشت;
    }
    arg++;
    arg = skipwhite(arg);

    اگر (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	بازگشت;
    }

    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, پوچ, پوچ, TRUE);
    اگر (type == پوچ)
	بازگشت;

    اگر (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	بازگشت;
    }

    صحیح cc = *name_end;
    *name_end = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    اگر (eval_variable_import(name_start, &tv) == OK)
    {
	اگر (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_start);
	وگرنه
	    semsg(_(e_redefining_script_item_str), name_start);
	clear_tv(&tv);
	برو done;
    }

    // Create a script-local variable for the type alias.
    اگر (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_lock = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_start);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_depth = parse_type_depth();
    }
    وگرنه
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_lock = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    set_var_const(name_start, current_sctx.sc_sid, پوچ, &tv, FALSE,
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
    صحیح
get_member_tv(
    class_T	*cl,
    صحیح		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    صحیح		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    اگر (m == پوچ)
	بازگشت FAIL;

    اگر (*name == '_' && (current_class == پوچ ||
				!class_instance_of(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	بازگشت FAIL;
    }

    اگر (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	set_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    وگرنه
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	set_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    بازگشت OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    ایستا صحیح
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
    صحیح		argcount = 0;
    ocmember_T	*ocm = پوچ;
    صحیح		m_idx;

    fp = method_lookup(cl, rettv->v_type, name, len, پوچ);
    اگر (fp == پوچ)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	اگر (ocm == پوچ || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    method_not_found_پیام(cl, rettv->v_type, name, len);
	    بازگشت FAIL;
	}

	اگر (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    بازگشت FAIL;
	}

	اگر (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	وگرنه
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_end;
	بازگشت OK;
    }

    اگر (ocm == پوچ && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
	بازگشت FAIL;
    }

    // process generic function call
    اگر (fp != پوچ)
    {
	fp = generic_func_get(fp, gfatab);
	اگر (fp == پوچ)
	    بازگشت FAIL;
    }

    char_u *argp = name_end;
    صحیح ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    اگر (ret == FAIL)
    {
	وقتی (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	بازگشت FAIL;
    }

    funcexe_T funcexe;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    اگر (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    اگر (evalarg != پوچ)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    صحیح error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								پوچ);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    برای (صحیح idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    اگر (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_found_var);
	بازگشت FAIL;
    }
    *arg = argp;

    بازگشت OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    صحیح
obj_method_to_partial_tv(object_T *obj, ufunc_T *obj_method, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    اگر (pt == پوچ)
	بازگشت FAIL;

    pt->pt_refcount = 1;
    اگر (obj != پوچ)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_method;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    بازگشت OK;
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
    صحیح
class_object_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		verbose UNUSED)	// give error messages
{
    صحیح	ret = FAIL;

    اگر (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	بازگشت FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, پوچ, پوچ, FNE_CHECK_START);
    اگر (name_end == name)
	بازگشت FAIL;
    size_t len = name_end - name;

    صحیح did_emsg_save = did_emsg;
    class_T *cl;
    اگر (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    وگرنه // VAR_OBJECT
    {
	اگر (rettv->vval.v_object == پوچ)
	{
	    emsg(_(e_using_null_object));
	    بازگشت FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    اگر (cl == پوچ)
    {
	emsg(_(e_incomplete_type));
	بازگشت FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    اگر (*name_end == '<')
    {
	cctx_T	    *cctx = پوچ;

	اگر (evalarg != پوچ)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_end = parse_generic_func_type_args(name, len, name + len,
						&gfatab, cctx);
	اگر (name_end == پوچ)
	    برو done;
    }

    اگر (*name_end == '(')
	// Invoke the class or object method
	ret = call_oc_method(cl, name, len, name_end, evalarg, arg,
							&gfatab, rettv);
    وگرنه اگر (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	صحیح is_object = rettv->v_type == VAR_OBJECT;
	اگر (get_member_tv(cl, is_object, name, len, پوچ, rettv) == OK)
	{
	    *arg = name_end;
	    ret = OK;
	    برو done;
	}

	// could be a class method or an object method
	صحیح	fidx;
	ufunc_T	*fp = method_lookup(cl, rettv->v_type, name, len, &fidx);
	اگر (fp != پوچ)
	{
	    // Protected methods are not accessible outside the class
	    اگر (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		برو done;
	    }

	    اگر (obj_method_to_partial_tv(is_object ? rettv->vval.v_object :
						پوچ, fp, rettv) == FAIL)
		برو done;

	    *arg = name_end;
	    ret = OK;
	    برو done;
	}

	اگر (did_emsg == did_emsg_save)
	    member_not_found_پیام(cl, rettv->v_type, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);

    بازگشت ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
find_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, پوچ, پوچ, FNE_CHECK_START);
    اگر (name_end == name || *name_end != '.')
	بازگشت پوچ;

    ufunc_T	*fp = پوچ;
    size_t	len = name_end - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    اگر (eval_variable(name, (صحیح)len,
				    0, &tv, پوچ, EVAL_VAR_NOAUTOLOAD) == FAIL)
	بازگشت پوچ;
    اگر (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	برو fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    اگر (cl == پوچ)
	برو fail_after_eval;
    char_u *fname = name_end + 1;
    char_u *fname_end = find_name_end(fname, پوچ, پوچ, FNE_CHECK_START);
    اگر (fname_end == fname)
	برو fail_after_eval;
    len = fname_end - fname;

    fp = method_lookup(cl, tv.v_type, fname, len, پوچ);

    اگر (fp != پوچ)
    {
	fp = eval_generic_func(fp, fname, &fname_end);
	اگر (fp != پوچ)
	    *arg = fname_end;
    }

fail_after_eval:
    clear_tv(&tv);
    بازگشت fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    صحیح
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحیح idx;
    class_member_lookup(cl, name, namelen, &idx);
    بازگشت idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, صحیح *idx)
{
    ocmember_T	*ret_m = پوچ;
    صحیح		ret_idx = -1;
    برای (صحیح i = 0; i < cl->class_class_member_شمار; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	اگر (namelen)
	{
	    اگر (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		بشکن;
	    }
	}
	وگرنه اگر (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    بشکن;
	}
    }
    اگر (idx != پوچ)
	*idx = ret_idx;
    بازگشت ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    ایستا ufunc_T *
class_method_lookup(class_T *cl, char_u *name, size_t namelen, صحیح *idx)
{
    ufunc_T	*ret_fp = پوچ;
    صحیح		ret_idx = -1;
    برای (صحیح i = 0; i < cl->class_class_function_شمار; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	اگر (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    بشکن;
	}
    }
    اگر (idx != پوچ)
	*idx = ret_idx;
    بازگشت ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    صحیح
class_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحیح idx;
    class_method_lookup(cl, name, namelen, &idx);
    بازگشت idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ایستا صحیح
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحیح idx;
    object_member_lookup(cl, name, namelen, &idx);
    بازگشت idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, صحیح *idx)
{
    ocmember_T	*ret_m = پوچ;
    صحیح		ret_idx = -1;
    برای (صحیح i = 0; i < cl->class_obj_member_شمار; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	اگر (namelen)
	{
	    اگر (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		بشکن;
	    }
	}
	وگرنه اگر (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    بشکن;
	}
    }
    اگر (idx != پوچ)
	*idx = ret_idx;
    بازگشت ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    ایستا ufunc_T *
object_method_lookup(class_T *cl, char_u *name, size_t namelen, صحیح *idx)
{
    ufunc_T	*ret_fp = پوچ;
    صحیح		ret_idx = -1;
    برای (صحیح i = 0; i < cl->class_obj_method_شمار; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	اگر (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    بشکن;
	}
    }
    اگر (idx != پوچ)
	*idx = ret_idx;
    بازگشت ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    صحیح
object_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحیح idx;
    object_method_lookup(cl, name, namelen, &idx);
    بازگشت idx;
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
    صحیح		*idx)
{
    اگر (v_type == VAR_CLASS)
	بازگشت class_member_lookup(cl, name, namelen, idx);

    بازگشت object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    ایستا class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_found = پوچ;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_found = پوچ;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    برای (class_T *super = cl; super != پوچ; super = super->class_extends)
    {
	class_T		*cl_tmp = پوچ;
	ocmember_T	*m = پوچ;
	اگر (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    اگر ((m = object_member_lookup(super, name, len, پوچ)) != پوچ)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	اگر (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    اگر (( m = class_member_lookup(super, name, len, پوچ)) != پوچ)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	اگر (cl_tmp == پوچ)
	    بشکن;  // member is not in this or any super class.
	cl_found = cl_tmp;
	m_found = m;
    }
    اگر (p_m != پوچ)
	*p_m = m_found;
    بازگشت cl_found;
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
    صحیح		*idx)
{
    اگر (v_type == VAR_CLASS)
	بازگشت class_method_lookup(cl, name, namelen, idx);
    وگرنه
	بازگشت object_method_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    صحیح
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    برای (cctx_T *cctx = cctx_arg; cctx != پوچ; cctx = cctx->ctx_outer)
	اگر (cctx->ctx_ufunc != پوچ
			&& class_instance_of(cctx->ctx_ufunc->uf_class, cl))
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    صحیح
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    اگر (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_in_class_str),
		m->ocm_name.string, cl->class_name.string);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    تهی
obj_lock_const_vars(object_T *obj)
{
    برای (صحیح i = 0; i < obj->obj_class->class_obj_member_شمار; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	اگر (ocm->ocm_flags & OCMFLAG_CONST)
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
    صحیح		sz;

    اگر (cl == پوچ)
	بازگشت پوچ;

    sz = اندازه(object_T) + cl->class_obj_member_شمار * اندازه(typval_T);
    obj = alloc_clear(sz);
    اگر (obj == پوچ)
	بازگشت پوچ;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    اگر (IS_ENUM(cl))
	enum_set_internal_obj_vars(cl, obj);

    بازگشت obj;
}

/*
 * Make a copy of an object.
 */
    تهی
copy_object(typval_T *from, typval_T *to)
{
    اگر (from->vval.v_object == پوچ)
	to->vval.v_object = پوچ;
    وگرنه
    {
	to->vval.v_object = from->vval.v_object;
	++to->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    تهی
copy_class(typval_T *from, typval_T *to)
{
    اگر (from->vval.v_class == پوچ)
	to->vval.v_class = پوچ;
    وگرنه
    {
	to->vval.v_class = from->vval.v_class;
	++to->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    ایستا تهی
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    برای (صحیح i = 0; i < cl->class_interface_شمار; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	اگر (cl->class_interfaces_cl[i] != پوچ)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *next;
    برای (itf2class_T *i2c = cl->class_itf2class; i2c != پوچ; i2c = next)
    {
	next = i2c->i2c_next;
	vim_free(i2c);
    }

    برای (صحیح i = 0; i < cl->class_class_member_شمار; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	اگر (cl->class_members_tv != پوچ)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    برای (صحیح i = 0; i < cl->class_obj_member_شمار; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    برای (صحیح i = 0; i < cl->class_class_function_شمار; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	اگر (IS_ENUM(cl) && IS_CONSTRUCTOR_METHOD(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    برای (صحیح i = 0; i < cl->class_obj_method_شمار; ++i)
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
    ایستا صحیح
class_get_selfrefs(class_T *cl)
{
    صحیح		self_refs = 0;
    typval_T	*tv;

    برای (صحیح i = 0; i < cl->class_class_member_شمار; ++i)
    {
	tv = &cl->class_members_tv[i];
	اگر (tv->v_type == VAR_OBJECT && tv->vval.v_object != پوچ
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    بازگشت self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    ایستا صحیح
can_free_enum(class_T *cl)
{
    برای (صحیح i = 0; i < cl->class_class_member_شمار; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	اگر (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    بازگشت TRUE;

	اگر (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    اگر (tv->vval.v_list->lv_refcount > 1)
		بازگشت FALSE;
	    بشکن;
	}

	اگر (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    بازگشت FALSE;
    }

    بازگشت TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    ایستا صحیح
can_free_class(class_T *cl)
{
    صحیح		can_free = FALSE;
    صحیح		self_refs = 0;

    اگر (IS_ENUM(cl) && !can_free_enum(cl))
	بازگشت FALSE;

    اگر (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    اگر (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    بازگشت can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    تهی
class_unref(class_T *cl)
{
    اگر (cl == پوچ)
	بازگشت;

    --cl->class_refcount;

    اگر (cl->class_name.string == پوچ)
	بازگشت;

    اگر (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    صحیح
class_free_nonref(صحیح copyID)
{
    صحیح		did_free = FALSE;

    برای (class_T *cl = first_class; cl != پوچ; cl = next_nonref_class)
    {
	next_nonref_class = cl->class_next_used;
	اگر ((cl->class_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    next_nonref_class = پوچ;
    بازگشت did_free;
}

    صحیح
set_ref_in_classes(صحیح copyID)
{
    برای (class_T *cl = first_class; cl != پوچ; cl = cl->class_next_used)
	set_ref_in_item_class(cl, copyID, پوچ, پوچ, پوچ);

    بازگشت FALSE;
}

ایستا object_T *first_object = پوچ;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    تهی
object_created(object_T *obj)
{
    اگر (first_object != پوچ)
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
    ایستا تهی
object_cleared(object_T *obj)
{
    اگر (obj->obj_next_used != پوچ)
	obj->obj_next_used->obj_prev_used = obj->obj_prev_used;
    اگر (obj->obj_prev_used != پوچ)
	obj->obj_prev_used->obj_next_used = obj->obj_next_used;
    وگرنه اگر (first_object == obj)
	first_object = obj->obj_next_used;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    ایستا تهی
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    اگر (!cl)
	بازگشت;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = INT_MAX;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    برای (صحیح i = 0; i < cl->class_obj_member_شمار; ++i)
	clear_tv(tv + i);
}

    ایستا تهی
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    اگر (!cl)
	بازگشت;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    ایستا تهی
object_free(object_T *obj)
{
    اگر (in_free_unref_اقلام)
	بازگشت;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    تهی
object_unref(object_T *obj)
{
    اگر (obj != پوچ && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    صحیح
object_free_nonref(صحیح copyID)
{
    صحیح		did_free = FALSE;

    برای (object_T *obj = first_object; obj != پوچ; obj = obj->obj_next_used)
    {
	اگر ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    بازگشت did_free;
}

    تهی
object_free_اقلام(صحیح copyID)
{
    object_T	*obj_next;

    برای (object_T *obj = first_object; obj != پوچ; obj = obj_next)
    {
	obj_next = obj->obj_next_used;
	اگر ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    تهی
emsg_var_cl_define(نویسه *پیام, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    اگر (cl_def != پوچ)
	semsg(_(پیام), m->ocm_name.string, cl_def->class_name.string);
    وگرنه
	emsg(_(e_internal_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    تهی
method_not_found_پیام(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *method_name = vim_strnsave(name, len);
    اگر ((v_type == VAR_OBJECT)
	    && (class_method_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	اگر (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	وگرنه
	    semsg(_(e_class_method_str_accessible_only_using_class_str),
		    method_name, cl->class_name.string);
    }
    وگرنه اگر ((v_type == VAR_CLASS)
	    && (object_method_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	اگر (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	وگرنه
	    semsg(_(e_object_method_str_accessible_only_using_object_str),
		    method_name, cl->class_name.string);
    }
    وگرنه
	semsg(_(e_method_not_found_on_class_str_str), method_name,
		cl->class_name.string);
    vim_free(method_name);
}

/*
 * Echo a class or object member not found message.
 */
    تهی
member_not_found_پیام(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    اگر (v_type == VAR_OBJECT)
    {
	اگر (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	وگرنه
	    semsg(_(e_variable_not_found_on_object_str_str), varname,
		    cl->class_name.string);
    }
    وگرنه
    {
	اگر (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	وگرنه
	{
	    اگر (IS_ENUM(cl))
		semsg(_(e_enum_مقدار_str_not_found_in_enum_str),
			varname, cl->class_name.string);
	    وگرنه
		semsg(_(e_class_variable_str_not_found_in_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    تهی
defcompile_class(class_T *cl)
{
    برای (صحیح loop = 1; loop <= 2; ++loop)
    {
	صحیح func_شمار = loop == 1 ? cl->class_class_function_شمار
						: cl->class_obj_method_شمار;
	برای (صحیح i = 0; i < func_شمار; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    اگر (!IS_ABSTRACT_METHOD(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    تهی
defcompile_classes_in_script(تهی)
{
    برای (class_T *cl = first_class; cl != پوچ; cl = cl->class_next_used)
    {
	اگر (eval_variable(cl->class_name.string, 0, 0, پوچ, پوچ,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    صحیح
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    اگر (eval_variable(name, 0, 0, rettv, پوچ, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	بازگشت rettv->v_type == VAR_CLASS;
    بازگشت FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    ایستا صحیح
object_call_builtin_method(
    object_T		*obj,
    class_builtin_T	builtin_method,
    صحیح			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    صحیح	    midx;

    اگر (obj == پوچ)
	بازگشت FAIL;

    uf = class_get_builtin_method(obj->obj_class, builtin_method, &midx);
    اگر (uf == پوچ)
	بازگشت FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    صحیح		r;

    اگر (fc == پوچ)
	بازگشت FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, پوچ, obj, fc, rettv);

    remove_funccal();

    بازگشت r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    صحیح
object_empty(object_T *obj)
{
    typval_T	rettv;

    اگر (object_call_builtin_method(obj, CLASS_BUILTIN_EMPTY, 0, پوچ, &rettv)
								== FAIL)
	بازگشت TRUE;

    بازگشت tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    صحیح
object_len(object_T *obj)
{
    typval_T	rettv;

    اگر (object_call_builtin_method(obj, CLASS_BUILTIN_LEN, 0, پوچ, &rettv)
								== FAIL)
	بازگشت 0;

    بازگشت tv_to_عدد(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    صحیح
object_equal(
	object_T *o1,
	object_T *o2,
	صحیح	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    اگر (o1 == o2)
	بازگشت TRUE;
    اگر (o1 == پوچ || o2 == پوچ)
	بازگشت FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    اگر (cl1 != cl2 || cl1 == پوچ || cl2 == پوچ)
	بازگشت FALSE;

    برای (صحیح i = 0; i < cl1->class_obj_member_شمار; ++i)
	اگر (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    بازگشت FALSE;

    بازگشت TRUE;
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
    صحیح		copyID,
    صحیح		echo_style,
    صحیح		restore_copyID,
    صحیح		composite_val)
{
    typval_T	rettv;

    اگر (object_call_builtin_method(obj, CLASS_BUILTIN_STRING, 0, پوچ, &rettv)
								== OK
					&& rettv.vval.v_string != پوچ)
	بازگشت rettv.vval.v_string;

    صحیح		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    اگر (cl != پوچ && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    وگرنه
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	اگر (cl == پوچ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	وگرنه
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
    }
    اگر (cl != پوچ)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	برای (صحیح i = 0; i < cl->class_obj_member_شمار; ++i)
	{
	    اگر (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.length);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = پوچ;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, composite_val);
	    اگر (s != پوچ)
		ga_concat(&ga, s);
	    vim_free(tf);
	    اگر (s == پوچ || did_echo_string_emsg)
	    {
		ok = FAIL;
		بشکن;
	    }
	    سطر_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    اگر (ok == FAIL)
    {
	vim_free(ga.ga_data);
	بازگشت پوچ;
    }
    ga_append(&ga, NUL);
    بازگشت (char_u *)ga.ga_data;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    صحیح
class_instance_of(class_T *cl, class_T *other_cl)
{
    اگر (cl == other_cl)
	بازگشت TRUE;

    // Recursively check the base classes.
    برای (; cl != پوچ; cl = cl->class_extends)
    {
	اگر (cl == other_cl)
	    بازگشت TRUE;
	// Check the implemented interfaces and the super interfaces
	برای (صحیح i = cl->class_interface_شمار - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    وقتی (intf != پوچ)
	    {
		اگر (intf == other_cl)
		    بازگشت TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    بازگشت FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    تهی
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_عدد = VVAL_FALSE;

    اگر (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_args(argvars, 1) == FAIL)
	بازگشت;

    اگر (object_tv->vval.v_object == پوچ)
    {
	اگر (classinfo_tv->vval.v_class == پوچ)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_عدد = VVAL_TRUE;
	بازگشت;
    }

    برای (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	اگر (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	وگرنه
	    c = classinfo_tv->vval.v_class;

	اگر (class_instance_of(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_عدد = VVAL_TRUE;
	    بازگشت;
	}
    }
}

#endif // FEAT_EVAL
