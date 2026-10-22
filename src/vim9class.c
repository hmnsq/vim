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
#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

સ્થિર class_T *first_class = ખાલી;
સ્થિર class_T *next_nonref_class = ખાલી;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    સ્થિર શૂન્ય
class_created(class_T *cl)
{
    જો (first_class != ખાલી)
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
    સ્થિર શૂન્ય
class_cleared(class_T *cl)
{
    જો (cl->class_next_used != ખાલી)
	cl->class_next_used->class_prev_used = cl->class_prev_used;
    જો (cl->class_prev_used != ખાલી)
	cl->class_prev_used->class_next_used = cl->class_next_used;
    નહીંતર જો (first_class == cl)
	first_class = cl->class_next_used;

    // update the next class to check if needed
    જો (cl == next_nonref_class)
	next_nonref_class = cl->class_next_used;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    સ્થિર પૂર્ણાંક
parse_member(
    exarg_T	*eap,
    char_u	*લીટી,
    char_u	*varname,
    પૂર્ણાંક		has_public,	    // TRUE if "public" seen before "varname"
    char_u	**varname_end,
    પૂર્ણાંક		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_પદાવલિ)
{
    *varname_end = to_name_end(varname, FALSE);
    જો (*varname == '_' && has_public)
    {
	semsg(_(e_public_variable_name_cannot_start_with_underscore_str), લીટી);
	પાછા FAIL;
    }

    char_u *colon = skipwhite(*varname_end);
    char_u *type_arg = colon;
    type_T *type = ખાલી;
    *has_type = FALSE;
    જો (*colon == ':')
    {
	જો (VIM_ISWHITE(**varname_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    પાછા FAIL;
	}
	જો (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    પાછા FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, ખાલી, ખાલી, TRUE);
	જો (type == ખાલી || !valid_declaration_type(type))
	    પાછા FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    જો (type == ખાલી && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	પાછા FAIL;
    }

    જો (init_પદાવલિ == ખાલી && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_in_interface));
	પાછા FAIL;
    }

    જો (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *પદાવલિ_start, *પદાવલિ_end;

	જો (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    પાછા FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(શૂન્ય)skip_પદાવલિ_concatenate(&init_arg, &પદાવલિ_start, &પદાવલિ_end, &evalarg);

	init_arg = skipwhite(init_arg);
	જો (*init_arg != NUL && !vim9_comment_start(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, ખાલી);
	    પાછા FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	જો (type == ખાલી)
	    type = &t_any;

	*init_પદાવલિ = vim_strnsave(પદાવલિ_start, પદાવલિ_end - પદાવલિ_start);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, ખાલી);
    }
    નહીંતર જો (!valid_declaration_type(type))
	પાછા FAIL;

    *type_ret = type;
    પાછા OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    સ્થિર પૂર્ણાંક
add_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_end,
    પૂર્ણાંક		has_public,
    પૂર્ણાંક		has_final,
    પૂર્ણાંક		has_const,
    પૂર્ણાંક		has_type,
    type_T	*type,
    char_u	*init_પદાવલિ)
{
    જો (ga_grow(gap, 1) == FAIL)
	પાછા FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_data) + gap->ga_len;
    m->ocm_name.length = (size_t)(varname_end - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.length);
    જો (m->ocm_name.string == ખાલી)
    {
	m->ocm_name.length = 0;
	પાછા FAIL;
    }
    m->ocm_access = has_public ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_PRIVATE : VIM_ACCESS_READ;
    જો (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    જો (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    જો (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    જો (init_પદાવલિ != ખાલી)
    {
	m->ocm_init = init_પદાવલિ;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    પાછા OK;
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
    સ્થિર પૂર્ણાંક
add_members_to_class(
    garray_T	*gap,
    ocmember_T	*parent_members,
    પૂર્ણાંક		parent_ગણતરી,
    ocmember_T	**members,
    પૂર્ણાંક		*member_ગણતરી)
{
    *member_ગણતરી = parent_ગણતરી + gap->ga_len;
    *members = *member_ગણતરી == 0 ? ખાલી
				       : ALLOC_MULT(ocmember_T, *member_ગણતરી);
    જો (*member_ગણતરી > 0 && *members == ખાલી)
	પાછા FAIL;
    માટે (પૂર્ણાંક i = 0; i < parent_ગણતરી; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = parent_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.length);
	જો (m->ocm_name.string == ખાલી)
	{
	    m->ocm_name.length = 0;
	    પાછા FAIL;
	}
	જો (m->ocm_init != ખાલી)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    જો (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + parent_ગણતરી,
			       gap->ga_data, કદ(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_data);
    પાછા OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    પૂર્ણાંક
object_index_from_itf_index(class_T *itf, પૂર્ણાંક is_method, પૂર્ણાંક idx, class_T *cl)
{
    જો (idx >= (is_method ? itf->class_obj_method_ગણતરી
				   : itf->class_obj_member_ગણતરી))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	પાછા 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    જો (cl == itf)
	પાછા idx;

    itf2class_T		*i2c = ખાલી;
    પૂર્ણાંક			searching = TRUE;
    પૂર્ણાંક			method_offset = 0;

    માટે (class_T *super = cl; super != ખાલી && searching;
						super = super->class_extends)
    {
	માટે (i2c = itf->class_itf2class; i2c != ખાલી; i2c = i2c->i2c_next)
	{
	    જો (i2c->i2c_class == super && i2c->i2c_is_method == is_method)
	    {
		searching = FALSE;
		તોડો;
	    }
	}
	જો (searching && is_method)
	    // The parent class methods are stored after the current class
	    // methods.
	    method_offset += super->class_obj_method_ગણતરી_child;
    }
    જો (i2c == ખાલી)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	પાછા 0;
    }

    // A table follows the i2c for the class
    પૂર્ણાંક *table = (પૂર્ણાંક *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    પાછા table[idx] + method_offset;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    સ્થિર પૂર્ણાંક
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    પૂર્ણાંક	    is_class)
{
    typval_T	tv;
    પૂર્ણાંક		success = FALSE;

    જો (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	પાછા success;
    }

    tv.v_type = VAR_UNKNOWN;
    જો (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_found_str), extends_name);
	પાછા success;
    }

    જો (tv.v_type != VAR_CLASS || tv.vval.v_class == ખાલી
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    નહીંતર
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    પાછા success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    સ્થિર પૂર્ણાંક
validate_extends_generic_method(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    અક્ષર	*સંદેશ = ખાલી;

    જો (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	પાછા TRUE;

    જો (super_fp->uf_generic_argcount == 0)
	સંદેશ = e_concrete_method_str_override_with_generic_method_in_class_str;
    નહીંતર જો (cl_fp->uf_generic_argcount == 0)
	સંદેશ = e_generic_method_str_override_with_concrete_method_in_class_str;
    નહીંતર
	સંદેશ = e_generic_method_str_type_arguments_mismatch_in_class_str;

    semsg(_(સંદેશ), cl_fp->uf_name, super_cl->class_name.string);
    પાછા FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    સ્થિર પૂર્ણાંક
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    પૂર્ણાંક		method_ગણતરી = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_data);

    જ્યારે (super != ખાલી)
    {
	પૂર્ણાંક extends_method_ગણતરી = super->class_obj_method_ગણતરી_child;
	જો (extends_method_ગણતરી == 0)
	{
	    super = super->class_extends;
	    ચાલુ;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	માટે (પૂર્ણાંક i = 0; i < extends_method_ગણતરી; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    પૂર્ણાંક	    extends_private = (*pstr == '_');
	    જો (extends_private)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    માટે (પૂર્ણાંક j = 0; j < method_ગણતરી; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		પૂર્ણાંક	priv_method = (*qstr == '_');
		જો (priv_method)
		    qstr++;
		જો (STRCMP(pstr, qstr) == 0)
		{
		    જો (priv_method != extends_private)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_method_str_of_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			પાછા FALSE;
		    }

		    જો (!validate_extends_generic_method(super,
						extends_methods[i], cl_fp[j]))
			    પાછા FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    પાછા TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    સ્થિર પૂર્ણાંક
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    પૂર્ણાંક member_ગણતરી = objmembers_gap->ga_len;
    જો (member_ગણતરી == 0)
	પાછા TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    માટે (પૂર્ણાંક c_i = 0; c_i < member_ગણતરી; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	જ્યારે (p_cl != ખાલી)
	{
	    પૂર્ણાંક p_member_ગણતરી = p_cl->class_obj_member_ગણતરી;

	    જો (p_member_ગણતરી == 0)
	    {
		p_cl = p_cl->class_extends;
		ચાલુ;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    માટે (પૂર્ણાંક p_i = 0; p_i < p_member_ગણતરી; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		જો (STRCMP(pstr, qstr) == 0)
		{
		    જો (c_protected != p_protected)
			semsg(_(e_public_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    નહીંતર
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    પાછા FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    પાછા TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    સ્થિર પૂર્ણાંક
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    પૂર્ણાંક member_ગણતરી = objmembers_gap->ga_len;
    જો (member_ગણતરી == 0)
	પાછા TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    માટે (પૂર્ણાંક c_i = 0; c_i < member_ગણતરી; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	પૂર્ણાંક	    var_found = FALSE;

	// Check in all the parent classes in the lineage
	જ્યારે (p_cl != ખાલી && !var_found)
	{
	    પૂર્ણાંક p_member_ગણતરી = p_cl->class_obj_member_ગણતરી;
	    જો (p_member_ગણતરી == 0)
	    {
		p_cl = p_cl->class_extends;
		ચાલુ;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    માટે (પૂર્ણાંક p_i = 0; p_i < p_member_ગણતરી; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		જો (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    ચાલુ;

		// Ensure the type is matching.
		where.wt_func_name = (અક્ષર *)c_m->ocm_name.string;
		where.wt_kind = WT_MEMBER;

		જો (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    પાછા FALSE;

		var_found = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    પાછા TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    સ્થિર પૂર્ણાંક
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    માટે (પૂર્ણાંક loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	પૂર્ણાંક extends_method_ગણતરી = loop == 1
				? extends_cl->class_class_function_ગણતરી
				: extends_cl->class_obj_method_ગણતરી;
	જો (extends_method_ગણતરી == 0)
	    ચાલુ;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	પૂર્ણાંક method_ગણતરી = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_data
						: objmethods_gap->ga_data);

	માટે (પૂર્ણાંક i = 0; i < extends_method_ગણતરી; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    જો (!IS_ABSTRACT_METHOD(uf))
		ચાલુ;

	    પૂર્ણાંક	concrete_method_found = FALSE;
	    પૂર્ણાંક	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    માટે (j = 0; !concrete_method_found && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		જો (!IS_ABSTRACT_METHOD(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_method_found = TRUE;
	    }

	    જો (concrete_method_found)
		ચાલુ;

	    માટે (j = 0; j < method_ગણતરી; j++)
	    {
		જો (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_method_found = TRUE;
		    તોડો;
		}
	    }

	    જો (!concrete_method_found)
	    {
		semsg(_(e_abstract_method_str_not_implemented), uf->uf_name);
		પાછા FALSE;
	    }
	}
    }

    પાછા TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    સ્થિર પૂર્ણાંક
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    પૂર્ણાંક		cl_member_ગણતરી,
    class_T	*extends_cl)
{
    પૂર્ણાંક		variable_present  = FALSE;

    માટે (પૂર્ણાંક cl_i = 0; cl_i < cl_member_ગણતરી; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	જો (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    ચાલુ;

	// Ensure the access type is same
	જો (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_of_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    પાછા FALSE;
	}

	// Ensure the type is matching.
	જો (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	નહીંતર
	{
	    where.wt_func_name = (અક્ષર *)m->ocm_name.string;
	    where.wt_kind = WT_MEMBER;
	    જો (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		પાછા FALSE;
	}

	variable_present = TRUE;
	તોડો;
    }

    જો (!variable_present && extends_cl != ખાલી)
    {
	પૂર્ણાંક ext_cl_ગણતરી = extends_cl->class_obj_member_ગણતરી;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	પાછા intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_ગણતરી,
					extends_cl->class_extends);
    }

    પાછા variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    સ્થિર પૂર્ણાંક
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    પૂર્ણાંક if_ગણતરી = ifcl->class_obj_member_ગણતરી;
    જો (if_ગણતરી == 0)
	પાછા TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_data);
    પૂર્ણાંક cl_ગણતરી = objmembers_gap->ga_len;
    માટે (પૂર્ણાંક if_i = 0; if_i < if_ગણતરી; ++if_i)
    {
	જો (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_ગણતરી, extends_cl))
	{
	    semsg(_(e_variable_str_of_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    પાછા FALSE;
	}
    }

    પાછા TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    સ્થિર પૂર્ણાંક
intf_method_type_matches(
    ufunc_T	*if_method,
    ufunc_T	*cl_method,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    જો (cl_method->uf_generic_argcount != if_method->uf_generic_argcount)
    {
	semsg(_(e_type_var_ગણતરી_of_method_str_differs_from_interface_str),
						cl_method->uf_name, intf_name);
	પાછા FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (અક્ષર *)if_method->uf_name;
    where.wt_kind = WT_METHOD;
    જો (check_type(if_method->uf_func_type, cl_method->uf_func_type, TRUE,
								where) == FAIL)
	પાછા FALSE;

    પાછા TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    સ્થિર પૂર્ણાંક
intf_method_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    પૂર્ણાંક	    cl_ગણતરી,
    class_T *extends_cl,
    char_u  *intf_name)
{
    પૂર્ણાંક		method_present  = FALSE;

    માટે (પૂર્ણાંક cl_i = 0; cl_i < cl_ગણતરી; ++cl_i)
    {
	જો (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    જો (!intf_method_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		પાછા FALSE;
	    method_present = TRUE;
	    તોડો;
	}
    }

    જો (!method_present && extends_cl != ખાલી)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	પૂર્ણાંક	ext_cl_ગણતરી = extends_cl->class_obj_method_ગણતરી;
	પાછા intf_method_present(if_ufunc, ext_cl_fp, ext_cl_ગણતરી,
					extends_cl->class_extends, intf_name);
    }

    પાછા method_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    સ્થિર પૂર્ણાંક
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    પૂર્ણાંક if_ગણતરી = ifcl->class_obj_method_ગણતરી;
    જો (if_ગણતરી == 0)
	પાછા TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_data);
    પૂર્ણાંક cl_ગણતરી = objmethods_gap->ga_len;
    માટે (પૂર્ણાંક if_i = 0; if_i < if_ગણતરી; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	જો (!intf_method_present(if_fp[if_i], cl_fp, cl_ગણતરી, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_method_str_of_interface_str_not_implemented),
		    if_name, intf_class_name);
	    પાછા FALSE;
	}
    }

    પાછા TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    સ્થિર પૂર્ણાંક
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    પૂર્ણાંક		success = TRUE;

    માટે (પૂર્ણાંક i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_data)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	જો (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_found_str), impl);
	    success = FALSE;
	    તોડો;
	}

	જો (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == ખાલી
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    તોડો;
	}

	class_T *ifcl = tv.vval.v_class;
	જો (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    તોડો;
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

    જો (!success)
	પાછા FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_data;
    cl->class_interface_ગણતરી = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    માટે (પૂર્ણાંક i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_data)[i],
		((class_T **)intf_classes_gap->ga_data)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = ખાલી;
    cl->class_interface_ગણતરી = 0;
    cl->class_extends = ખાલી;

    પાછા success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    સ્થિર પૂર્ણાંક
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    માટે (પૂર્ણાંક j = 0; j < intf_classes_gap->ga_len; j++)
    {
	જો (((class_T **)intf_classes_gap->ga_data)[j] == ifcl)
	    પાછા TRUE;
    }

    પાછા FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    સ્થિર પૂર્ણાંક
add_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.length);
    જો (intf_name == ખાલી)
	પાછા FALSE;

    જો (ga_grow(impl_gap, 1) == FAIL)
	જાઓ fail;

    char_u **intf_names = (char_u **)impl_gap->ga_data;
    intf_names[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    જો (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	જાઓ fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_data;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    પાછા TRUE;
fail:
    vim_free(intf_name);
    પાછા FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    સ્થિર પૂર્ણાંક
add_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    માટે (પૂર્ણાંક i = 0; i < super->class_interface_ગણતરી; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	જો (!is_interface_class_present(intf_classes_gap, ifcl))
	    add_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    પાછા TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    સ્થિર પૂર્ણાંક
check_func_arg_names(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    માટે (પૂર્ણાંક loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	માટે (પૂર્ણાંક fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_data)[fi];

	    માટે (પૂર્ણાંક i = 0; i < uf->uf_args.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_args.ga_data)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		માટે (પૂર્ણાંક mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_data + mi)->ocm_name.string;
		    જો (STRCMP(aname, mname) == 0)
		    {
			જો (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_argument_already_declared_in_class_str),
				aname);

			પાછા FALSE;
		    }
		}
	    }
	}
    }

    પાછા TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    સ્થિર પૂર્ણાંક
is_reserved_varname(char_u *varname, char_u *varname_end)
{
    પૂર્ણાંક reserved = FALSE;
    char_u save_varname_end = *varname_end;
    *varname_end = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_end = save_varname_end;

    પાછા reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    સ્થિર પૂર્ણાંક
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_end)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_end - varname)};
    પૂર્ણાંક		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    જો (*pstr.string == '_')
    {
	pstr.string++;
	pstr.length--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    માટે (પૂર્ણાંક loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	માટે (પૂર્ણાંક i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_data) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.length};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    જો (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.length--;
		q_protected = true;
	    }

	    જો (pstr.length == qstr.length
		&& STRNCMP(pstr.string, qstr.string, pstr.length) == 0)
	    {
		char_u	save_c = *varname_end;

		*varname_end = NUL;
		જો (p_protected != q_protected)
		    semsg(_(e_public_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		નહીંતર
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_end = save_c;
		dup = TRUE;
		તોડો;
	    }
	}
    }

    પાછા dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    સ્થિર પૂર્ણાંક
is_duplicate_method(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    માટે (પૂર્ણાંક loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	માટે (પૂર્ણાંક i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_data)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    જો (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		પાછા TRUE;
	    }
	}
    }

    પાછા FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    સ્થિર પૂર્ણાંક
is_valid_constructor(ufunc_T *uf, પૂર્ણાંક is_abstract, પૂર્ણાંક has_static)
{
    // Constructors are not allowed in abstract classes.
    જો (is_abstract)
    {
	emsg(_(e_cannot_define_new_method_in_abstract_class));
	પાછા FALSE;
    }
    // A constructor is always static, no need to define it so.
    જો (has_static)
    {
	emsg(_(e_cannot_define_new_method_as_static));
	પાછા FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    જો (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_method));
	પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    સ્થિર પૂર્ણાંક
object_check_builtin_method_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    પૂર્ણાંક	    valid = FALSE;
    type_T  method_sig;
    type_T  method_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_FIELD(method_sig);
    CLEAR_FIELD(method_rt);
    method_sig.tt_type = VAR_FUNC;

    જો (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	method_rt.tt_type = VAR_સંખ્યા;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    નહીંતર જો (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	method_rt.tt_type = VAR_BOOL;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    નહીંતર જો (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	method_rt.tt_type = VAR_STRING;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    નહીંતર
	semsg(_(e_builtin_object_method_str_not_supported), uf->uf_name);

    where.wt_func_name = (અક્ષર *)uf->uf_name;
    where.wt_kind = WT_METHOD;
    જો (valid && !check_type(&method_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    પાછા valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    પૂર્ણાંક
is_valid_builtin_obj_methodname(char_u *funcname)
{
    પસંદગી (funcname[0])
    {
	સ્થિતિ 'e':
	    પાછા STRNCMP(funcname, "empty", 5) == 0;

	સ્થિતિ 'l':
	    પાછા STRNCMP(funcname, "len", 3) == 0;

	સ્થિતિ 'n':
	    પાછા STRNCMP(funcname, "new", 3) == 0;

	સ્થિતિ 's':
	    પાછા STRNCMP(funcname, "string", 6) == 0;
    }

    પાછા FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_method(
    class_T		*cl,
    class_builtin_T	builtin_method,
    પૂર્ણાંક			*method_idx)
{
    *method_idx = -1;

    જો (cl == ખાલી)
	પાછા ખાલી;

    *method_idx = cl->class_builtin_methods[builtin_method];
    પાછા *method_idx != -1 ? cl->class_obj_methods[*method_idx] : ખાલી;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    સ્થિર પૂર્ણાંક
update_member_method_lookup_table(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    પૂર્ણાંક		pobj_method_offset)
{
    જો (ifcl == ખાલી)
	પાછા OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(કદ(itf2class_T)
				+ ifcl->class_obj_member_ગણતરી * કદ(પૂર્ણાંક));
    જો (if2cl == ખાલી)
	પાછા FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = FALSE;

    માટે (પૂર્ણાંક if_i = 0; if_i < ifcl->class_obj_member_ગણતરી; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	માટે (પૂર્ણાંક cl_i = 0; cl_i < cl->class_obj_member_ગણતરી; ++cl_i)
	{
	    જો (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		પૂર્ણાંક *table = (પૂર્ણાંક *)(if2cl + 1);
		table[if_i] = cl_i;
		તોડો;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(કદ(itf2class_T)
				+ ifcl->class_obj_method_ગણતરી * કદ(પૂર્ણાંક));
    જો (if2cl == ખાલી)
	પાછા FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = TRUE;

    માટે (પૂર્ણાંક if_i = 0; if_i < ifcl->class_obj_method_ગણતરી; ++if_i)
    {
	પૂર્ણાંક	done = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	માટે (પૂર્ણાંક cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    જો (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_data)[cl_i]->uf_name) == 0)
	    {
		પૂર્ણાંક *table = (પૂર્ણાંક *)(if2cl + 1);
		table[if_i] = cl_i;
		done = TRUE;
		તોડો;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	જો (!done)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    જો (cl->class_extends != ifcl)
	    {
		class_T		*parent = cl->class_extends;
		પૂર્ણાંક		method_offset = objmethods->ga_len;

		જ્યારે (!done && parent != ખાલી && parent != ifcl)
		{
		    માટે (પૂર્ણાંક cl_i = 0;
			    cl_i < parent->class_obj_method_ગણતરી_child; ++cl_i)
		    {
			જો (STRCMP(if_name,
				    parent->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    પૂર્ણાંક *table = (પૂર્ણાંક *)(if2cl + 1);
			    table[if_i] = method_offset + cl_i;
			    done = TRUE;
			    તોડો;
			}
		    }
		    method_offset += parent->class_obj_method_ગણતરી_child;
		    parent = parent->class_extends;
		}
	    }

	    જો (!done)
	    {
		પૂર્ણાંક *table = (પૂર્ણાંક *)(if2cl + 1);
		table[if_i] = pobj_method_offset + if_i;
	    }
	}
    }

    પાછા OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    સ્થિર પૂર્ણાંક
add_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    માટે (પૂર્ણાંક i = 0; i < cl->class_interface_ગણતરી; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	જ્યારે (ifcl != ખાલી)
	{
	    જો (update_member_method_lookup_table(ifcl, cl, objmethods_gap,
								0) == FAIL)
		પાછા FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    જો (extends_cl != ખાલી)
    {
	class_T		*pclass = extends_cl;
	પૂર્ણાંક		pobj_method_offset = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	જ્યારે (pclass != ખાલી)
	{
	    જો (update_member_method_lookup_table(pclass, cl,
			objmethods_gap, pobj_method_offset) == FAIL)
		પાછા FAIL;

	    pobj_method_offset += pclass->class_obj_method_ગણતરી_child;
	    pclass = pclass->class_extends;
	}
    }

    પાછા OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    સ્થિર પૂર્ણાંક
add_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_ગણતરી);
    જો (cl->class_members_tv == ખાલી)
	પાછા FAIL;

    માટે (પૂર્ણાંક i = 0; i < cl->class_class_member_ગણતરી; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	જો (m->ocm_init != ખાલી)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_પદાવલિ(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    જો (etv == ખાલી)
		પાછા FAIL;

	    જો (m->ocm_type->tt_type == VAR_ANY
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
	નહીંતર
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = ખાલી;
	}
	set_tv_type(tv, m->ocm_type);
	જો (m->ocm_flags & OCMFLAG_CONST)
	    item_lock(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    પાછા OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    સ્થિર શૂન્ય
add_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    પૂર્ણાંક		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    માટે (પૂર્ણાંક i = 0; i < cl->class_obj_member_ગણતરી; ++i)
    {
	જો (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    ચાલુ;

	જો (i > (is_enum ? 2 : 0))
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

    garray_T લીટીઓ_to_free;
    ga_init2(&લીટીઓ_to_free, કદ(char_u *), 50);

    ufunc_T *nf = define_function(&fea, ખાલી, &લીટીઓ_to_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_ગણતરી,
			    ખાલી);

    ga_clear_strings(&લીટીઓ_to_free);
    vim_free(fga.ga_data);

    જો (nf != ખાલી && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_data)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	જો (nf->uf_ret_type != ખાલી)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_args = ખાલી;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    સ્થિર પૂર્ણાંક
add_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    માટે (પૂર્ણાંક loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	પૂર્ણાંક		*fcount = loop == 1 ? &cl->class_class_function_ગણતરી
						: &cl->class_obj_method_ગણતરી;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	પૂર્ણાંક parent_ગણતરી = 0;
	જો (extends_cl != ખાલી)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    parent_ગણતરી = loop == 1
				? 0
				: extends_cl->class_obj_method_ગણતરી;

	*fcount = parent_ગણતરી + gap->ga_len;
	જો (*fcount == 0)
	{
	    *fup = ખાલી;
	    ચાલુ;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	જો (*fup == ખાલી)
	    પાછા FAIL;

	જો (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_data, કદ(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_data);
	જો (loop == 1)
	    cl->class_class_function_ગણતરી_child = gap->ga_len;
	નહીંતર
	    cl->class_obj_method_ગણતરી_child = gap->ga_len;

	જો (loop == 2)
	{
	    // Copy instance methods from the parent.

	    માટે (પૂર્ણાંક i = 0; i < parent_ગણતરી; ++i)
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
		જો (nf != ખાલી && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_args != ખાલી)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		માટે (પૂર્ણાંક ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    જો (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (અક્ષર *)pname;
			where.wt_kind = WT_METHOD;
			(શૂન્ય)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	માટે (પૂર્ણાંક i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    જો (i < gap->ga_len)
		fp->uf_defclass = cl;
	    જો (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    પાછા OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    સ્થિર શૂન્ય
update_builtin_method_index(class_T *cl)
{
    પૂર્ણાંક	i;

    માટે (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    માટે (i = 0; i < cl->class_obj_method_ગણતરી; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	જો (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	નહીંતર જો (cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] = i;
	નહીંતર જો (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    સ્થિર char_u *
find_class_name_end(char_u *arg)
{
    char_u *end = arg;

    જ્યારે (ASCII_ISALNUM(*end) || *end == '_'
	    || (*end == '.' && (ASCII_ISALNUM(end[1]) || end[1] == '_')))
	++end;

    પાછા end;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    સ્થિર પૂર્ણાંક
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
    પૂર્ણાંક		dup = FALSE;

    માટે (પૂર્ણાંક i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_data) + i;

	જો (name.length == m->ocm_name.length
	    && STRNCMP(name.string, m->ocm_name.string, name.length) == 0)
	{
	    char_u  save_c = *varname_end;

	    *varname_end = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_end = save_c;
	    dup = TRUE;
	    તોડો;
	}
    }

    પાછા dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    સ્થિર પૂર્ણાંક
enum_parse_કિંમતો(
    exarg_T	*eap,
    class_T	*en,
    char_u	*લીટી,
    garray_T	*gap,
    પૂર્ણાંક		*અંક_enum_કિંમતો,
    પૂર્ણાંક		*enum_end)
{
    evalarg_T	evalarg;
    char_u	*p = લીટી;
    અક્ષર	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	last_char = NUL;
    પૂર્ણાંક		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    પૂર્ણાંક		did_emsg_before = did_emsg;
    જ્યારે (*p != NUL)
    {
	// ignore comment
	જો (*p == '#')
	    તોડો;

	જો (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_કિંમત_declaration_str), p);
	    તોડો;
	}

	char_u *eni_name_start = p;
	char_u *eni_name_end = to_name_end(p, FALSE);

	જો (is_duplicate_enum(gap, eni_name_start, eni_name_end))
	    તોડો;

	p = skipwhite(eni_name_end);

	char_u	*init_પદાવલિ = ખાલી;
	જો (*p == '(' || *p == '<')
	{
	    જો (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", લીટી);
		તોડો;
	    }

	    char_u *પદાવલિ_start, *પદાવલિ_end;

	    p = eni_name_start;
	    (શૂન્ય)skip_પદાવલિ_concatenate(&p, &પદાવલિ_start, &પદાવલિ_end, &evalarg);

	    જ્યારે (*પદાવલિ_start && *પદાવલિ_start != '(' && *પદાવલિ_start != '<')
		પદાવલિ_start++;

	    જો (પદાવલિ_end > પદાવલિ_start)
		init_પદાવલિ = vim_strnsave(પદાવલિ_start, પદાવલિ_end - પદાવલિ_start);
	}

	જો (init_પદાવલિ == ખાલી)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, કદ(initexpr_buf), "%s.new()", en->class_name.string);
	નહીંતર
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, કદ(initexpr_buf), "%s.new%s", en->class_name.string,
		init_પદાવલિ);
	    vim_free(init_પદાવલિ);
	}
	જો (add_member(gap, eni_name_start, eni_name_end, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    તોડો;

	++*અંક_enum_કિંમતો;

	જો (*p != '#')
	    last_char = *p;

	જો (*p != NUL && *p != ',')
	    તોડો;

	જો (*p == ',')
	{
	    જો (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", લીટી);
		તોડો;
	    }
	    જો (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", લીટી);
		તોડો;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    જો (*p != NUL && *p != '#')
    {
	જો (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_argument_str), p);
	rc = FAIL;
    }

    જો (last_char != ',')
	// last enum value should not be terminated by ","
	*enum_end = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, ખાલી);

    પાછા rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    સ્થિર પૂર્ણાંક
enum_add_કિંમતો_member(
    class_T	*en,
    garray_T	*gap,
    પૂર્ણાંક		અંક_enum_કિંમતો,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    પૂર્ણાંક		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    માટે (પૂર્ણાંક i = 0; i < અંક_enum_કિંમતો; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;

	જો (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.length);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    જો (type == ખાલી)
	જાઓ done;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    જો (type->tt_member != ખાલી)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = add_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_data, fga.ga_len));

done:
    vim_free(fga.ga_data);

    પાછા rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    સ્થિર શૂન્ય
enum_clear_constructors(class_T *en)
{
    માટે (પૂર્ણાંક i = 0; i < en->class_class_function_ગણતરી; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	જો (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    શૂન્ય
enum_set_internal_obj_vars(class_T *en, object_T *enval)
{
    પૂર્ણાંક	i;

    માટે (i = 0; i < en->class_class_member_ગણતરી; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	જો (en_tv != ખાલી && en_tv->v_type == VAR_UNKNOWN)
	    તોડો;
    }

    જો (i >= en->class_class_member_ગણતરી)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	પાછા;

    // First object variable is the name
    ocmember_T *કિંમત_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(કિંમત_ocm->ocm_name.string, કિંમત_ocm->ocm_name.length);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_સંખ્યા;
    ord_tv->vval.v_સંખ્યા = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    સ્થિર શૂન્ય
skip_class_body(exarg_T *eap)
{
    માટે (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	જો (theline == ખાલી)
	    તોડો;

	char_u	*p = theline;
	પૂર્ણાંક	found;

	// skip ':' and blanks
	માટે (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	found = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	જો (found)
	    તોડો;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    શૂન્ય
ex_class(exarg_T *eap)
{
    પૂર્ણાંક		is_class = eap->cmdidx == CMD_class;
    પૂર્ણાંક		is_abstract = eap->cmdidx == CMD_abstract;
    પૂર્ણાંક		is_enum = eap->cmdidx == CMD_enum;
    પૂર્ણાંક		added_enum_કિંમતો = FALSE;
    પૂર્ણાંક		is_interface;
    long	start_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    જો (eap->skip)
    {
	skip_class_body(eap);
	પાછા;
    }

    જો (is_abstract)
    {
	જો (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_argument_str), arg);
	    પાછા;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    જો (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	જો (is_class)
	    emsg(_(e_class_can_only_be_defined_in_vim9_script));
	નહીંતર જો (is_enum)
	    emsg(_(e_enum_can_only_be_defined_in_vim9_script));
	નહીંતર
	    emsg(_(e_interface_can_only_be_defined_in_vim9_script));
	પાછા;
    }

    જો (!ASCII_ISUPPER(*arg))
    {
	જો (is_class)
	    semsg(_(e_class_name_must_start_with_uppercase_letter_str), arg);
	નહીંતર જો (is_enum)
	    semsg(_(e_enum_name_must_start_with_uppercase_letter_str), arg);
	નહીંતર
	    semsg(_(e_interface_name_must_start_with_uppercase_letter_str),
									  arg);
	પાછા;
    }
    char_u *name_end = find_name_end(arg, ખાલી, ખાલી, FNE_CHECK_START);
    જો (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	પાછા;
    }
    char_u *name_start = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = ખાલી;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, કદ(char_u *), 5);

    arg = skipwhite(name_end);
    જ્યારે (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	જો (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    જો (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		જાઓ early_ret;
	    }
	    જો (extends != ખાલી)
	    {
		emsg(_(e_duplicate_extends));
		જાઓ early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *end = find_class_name_end(arg);
	    જો (!IS_WHITE_OR_NUL(*end))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		જાઓ early_ret;
	    }
	    extends = vim_strnsave(arg, end - arg);
	    જો (extends == ખાલી)
		જાઓ early_ret;

	    arg = skipwhite(end + 1);
	}
	નહીંતર જો (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    જો (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		જાઓ early_ret;
	    }

	    જો (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		જાઓ early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    માટે (;;)
	    {
		char_u *impl_end = find_class_name_end(arg);
		જો ((!IS_WHITE_OR_NUL(*impl_end) && *impl_end != ',')
			|| (*impl_end == ','
			    && !IS_WHITE_OR_NUL(*(impl_end + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    જાઓ early_ret;
		}
		જો (impl_end - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    જાઓ early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_end - arg);
		જો (iname == ખાલી)
		    જાઓ early_ret;
		માટે (પૂર્ણાંક i = 0; i < ga_impl.ga_len; ++i)
		    જો (STRCMP(((char_u **)ga_impl.ga_data)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			જાઓ early_ret;
		    }
		જો (ga_add_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    જાઓ early_ret;
		}
		જો (*impl_end != ',')
		{
		    arg = skipwhite(impl_end);
		    તોડો;
		}
		arg = skipwhite(impl_end + 1);
	    }
	}
	નહીંતર
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_strings(&ga_impl);
	    પાછા;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, કદ(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, કદ(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, કદ(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, કદ(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, કદ(ufunc_T *), 10);

    class_T *cl = ખાલી;
    class_T *extends_cl = ખાલી;  // class from "extends" argument
    class_T **intf_classes = ખાલી;
    પૂર્ણાંક	    intf_ગણતરી = 0;	// number of items in "intf_classes"
    પૂર્ણાંક	    અંક_enum_કિંમતો = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    જો (cl == ખાલી)
	જાઓ cleanup;

    જો (is_enum)
	cl->class_flags = CLASS_ENUM;
    નહીંતર જો (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    નહીંતર જો (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.length = (size_t)(name_end - name_start);
    cl->class_name.string = vim_strnsave(name_start, cl->class_name.length);
    જો (cl->class_name.string == ખાલી)
	જાઓ cleanup;

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
    પૂર્ણાંક rc = set_var_const(cl->class_name.string, 0, ખાલી, &tv, FALSE, 0, 0);
    જો (rc == FAIL)
	જાઓ cleanup;

    જો (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	જો (add_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, ખાલી) == FAIL)
	    જાઓ cleanup;

	varname = (char_u *)"ordinal";
	જો (add_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_સંખ્યા, ખાલી) == FAIL)
	    જાઓ cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    પૂર્ણાંક		enum_end = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = ખાલી;
    પૂર્ણાંક success = FALSE;
    માટે (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	જો (theline == ખાલી)
	    તોડો;
	char_u *લીટી = skipwhite(theline);

	// Skip empty and comment lines.
	જો (*લીટી == NUL)
	    ચાલુ;
	જો (*લીટી == '#')
	{
	    જો (vim9_bad_comment(લીટી))
		તોડો;
	    ચાલુ;
	}

	char_u *p = લીટી;

	અક્ષર	*end_name;
	પૂર્ણાંક	shortlen;
	પૂર્ણાંક	fullen;
	જો (is_class)
	{
	    end_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	નહીંતર જો (is_enum)
	{
	    end_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	નહીંતર
	{
	    end_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	માટે (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_start = p;
	જો (checkforcmd(&p, end_name, shortlen))
	{
	    જો (STRNCMP(cmd_start, end_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_start);
	    નહીંતર જો (*p == '|' || !ends_excmd2(લીટી, p))
		semsg(_(e_trailing_characters_str), p);
	    નહીંતર
		success = TRUE;
	    તોડો;
	}

	પૂર્ણાંક	wrong_endname = FALSE;
	જો (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	નહીંતર જો (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	નહીંતર
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	જો (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), લીટી, end_name);
	    તોડો;
	}

	જો (is_enum && !enum_end)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    જો (enum_parse_કિંમતો(eap, cl, લીટી, &classmembers,
					&અંક_enum_કિંમતો, &enum_end) == FAIL)
		તોડો;

	    જો (enum_end)
	    {
		// Add the enum "values" class variable.
		enum_add_કિંમતો_member(cl, &classmembers, અંક_enum_કિંમતો,
							&type_list);
		added_enum_કિંમતો = TRUE;
	    }
	    ચાલુ;
	}

	પૂર્ણાંક has_public = FALSE;
	જો (checkforcmd(&p, "public", 3))
	{
	    જો (STRNCMP(લીટી, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), લીટી);
		તોડો;
	    }
	    જો (is_interface)
	    {
		emsg(_(e_public_variable_not_supported_in_interface));
		તોડો;
	    }
	    has_public = TRUE;
	    p = skipwhite(લીટી + 6);

	    જો (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_public_keyword_not_supported_for_method));
		તોડો;
	    }

	    જો (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_public_must_be_followed_by_var_static_final_or_const));
		તોડો;
	    }
	}

	પૂર્ણાંક abstract_method = FALSE;
	char_u *pa = p;
	જો (checkforcmd(&p, "abstract", 3))
	{
	    જો (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		તોડો;
	    }

	    જો (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_used_in_enum));
		તોડો;
	    }

	    જો (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_used_in_interface));
		તોડો;
	    }

	    જો (!is_abstract)
	    {
		semsg(_(e_abstract_method_in_concrete_class), pa);
		તોડો;
	    }

	    p = skipwhite(pa + 8);
	    જો (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		તોડો;
	    }

	    abstract_method = TRUE;
	}

	પૂર્ણાંક has_static = FALSE;
	char_u *ps = p;
	જો (checkforcmd(&p, "static", 4))
	{
	    જો (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		તોડો;
	    }

	    જો (is_interface)
	    {
		emsg(_(e_static_member_not_supported_in_interface));
		તોડો;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    જો (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		તોડો;
	    }
	}

	પૂર્ણાંક has_final = FALSE;
	પૂર્ણાંક has_var = FALSE;
	પૂર્ણાંક has_const = FALSE;
	જો (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	નહીંતર જો (checkforcmd(&p, "final", 5))
	{
	    જો (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_in_interface));
		તોડો;
	    }
	    has_final = TRUE;
	}
	નહીંતર જો (checkforcmd(&p, "const", 5))
	{
	    જો (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_in_interface));
		તોડો;
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
	જો (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_end = ખાલી;
	    type_T *type = ખાલી;
	    char_u *init_પદાવલિ = ખાલી;
	    પૂર્ણાંક	    has_type = FALSE;

	    જો (!eval_isnamec1(*p))
	    {
		જો (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), લીટી);
		નહીંતર
		    semsg(_(e_invalid_object_variable_declaration_str), લીટી);
		તોડો;
	    }

	    જો (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_in_interface),
			varname);
		તોડો;
	    }

	    જો (parse_member(eap, લીટી, varname, has_public,
			  &varname_end, &has_type, &type_list, &type,
			  !is_interface ? &init_પદાવલિ: ખાલી) == FAIL)
		તોડો;

	    જો (is_reserved_varname(varname, varname_end)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_end))
	    {
		vim_free(init_પદાવલિ);
		તોડો;
	    }
	    જો (add_member(has_static ? &classmembers : &objmembers, varname,
				varname_end, has_public, has_final, has_const,
				has_type, type, init_પદાવલિ) == FAIL)
	    {
		vim_free(init_પદાવલિ);
		તોડો;
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
	નહીંતર જો (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	લીટીઓ_to_free;
	    પૂર્ણાંક		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    જો (has_public)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_public_keyword_not_supported_for_method));
		તોડો;
	    }

	    જો (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_in_class_str), લીટી);
		તોડો;
	    }

	    જો (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_method_not_supported_in_interface), p);
		તોડો;
	    }

	    જો (has_static && !is_new && SAFE_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_method_not_supported), p);
		તોડો;
	    }

	    CLEAR_FIELD(ea);
	    ea.cmd = લીટી;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&લીટીઓ_to_free, કદ(char_u *), 50);
	    પૂર્ણાંક class_flags;
	    જો (is_interface)
		class_flags = CF_INTERFACE;
	    નહીંતર
		class_flags = abstract_method ? CF_ABSTRACT_METHOD : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, ખાલી, &લીટીઓ_to_free,
			class_flags, objmembers.ga_data, objmembers.ga_len,
			ખાલી);
	    ga_clear_strings(&લીટીઓ_to_free);

	    જો (uf != ખાલી)
	    {
		જો (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_method_not_supported_in_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    તોડો;
		}

		// check for builtin method
		જો (!is_new && SAFE_islower(*uf->uf_name) &&
					!object_check_builtin_method_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    તોડો;
		}

		// Check the uf_name isn't used already.
		જો (is_duplicate_method(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    તોડો;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		જો (ga_grow(fgap, 1) == OK)
		{
		    જો (is_new)
			uf->uf_flags |= FC_NEW;

		    જો (abstract_method)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_data)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	નહીંતર
	{
	    જો (is_class)
		semsg(_(e_not_valid_command_in_class_str), લીટી);
	    નહીંતર જો (is_enum)
		semsg(_(e_not_valid_command_in_enum_str), લીટી);
	    નહીંતર
		semsg(_(e_not_valid_command_in_interface_str), લીટી);
	    તોડો;
	}
    }

    જો (theline == ખાલી && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    જો (success && is_enum && (અંક_enum_કિંમતો == 0 || !added_enum_કિંમતો))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_add_કિંમતો_member(cl, &classmembers, અંક_enum_કિંમતો,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    જો (success && extends != ખાલી)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    જો (success && extends_cl != ખાલી)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    જો (success && extends_cl != ખાલી)
    {
	જો (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	નહીંતર
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    જો (success && !is_abstract && extends_cl != ખાલી
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, કદ(class_T *), 5);

    જો (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    જો (success && extends_cl != ખાલી)
	success = add_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_data;
    intf_ગણતરી = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    જો (success)
	success = check_func_arg_names(&classfunctions, &objmethods,
							&classmembers);

    જો (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	જો (extends_cl != ખાલી)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	જો (add_members_to_class(&classmembers,
				 ખાલી,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_ગણતરી) == FAIL
		|| add_members_to_class(&objmembers,
				 extends_cl == ખાલી ? ખાલી
					       : extends_cl->class_obj_members,
				 extends_cl == ખાલી ? 0
					  : extends_cl->class_obj_member_ગણતરી,
				 &cl->class_obj_members,
				 &cl->class_obj_member_ગણતરી) == FAIL)
	    જાઓ cleanup;

	જો (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_ગણતરી = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    જો (cl->class_interfaces == ખાલી)
		જાઓ cleanup;
	    માટે (પૂર્ણાંક i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_data)[i];
	    VIM_CLEAR(ga_impl.ga_data);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = ખાલી;
	}

	જો (cl->class_interface_ગણતરી > 0 || extends_cl != ખાલી)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    જો (add_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		જાઓ cleanup;
	}

	પૂર્ણાંક	have_new = FALSE;
	ufunc_T	*class_func = ખાલી;
	માટે (પૂર્ણાંક i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_data)[i];
	    જો (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		તોડો;
	    }
	}

	જો (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	નહીંતર જો ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    add_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	જો (add_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    જાઓ cleanup;

	update_builtin_method_index(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	જો ((is_class || is_enum) && cl->class_class_member_ગણતરી > 0)
	    જો (add_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		પાછા;
	    }

	cl->class_type_list = type_list;

	જો (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	પાછા;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    જો (intf_classes != ખાલી)
    {
	માટે (પૂર્ણાંક i = 0; i < intf_ગણતરી; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_strings(&ga_impl);

    માટે (પૂર્ણાંક round = 1; round <= 2; ++round)
    {
	garray_T *gap = round == 1 ? &classmembers : &objmembers;
	જો (gap->ga_len == 0 || gap->ga_data == ખાલી)
	    ચાલુ;

	માટે (પૂર્ણાંક i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    માટે (પૂર્ણાંક i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    માટે (પૂર્ણાંક i = 0; i < classfunctions.ga_len; ++i)
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
    પૂર્ણાંક		is_object,
    char_u	*name,
    char_u	*name_end,
    પૂર્ણાંક		*member_idx)
{
    size_t	len = name_end - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    જો (m == ખાલી)
    {
	member_not_found_સંદેશ(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	પાછા &t_any;
    }

    પાછા m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    પૂર્ણાંક		is_object,
    પૂર્ણાંક		member_idx)
{
    ocmember_T	*m;
    પૂર્ણાંક		member_ગણતરી;

    જો (is_object)
    {
	m = cl->class_obj_members;
	member_ગણતરી = cl->class_obj_member_ગણતરી;
    }
    નહીંતર
    {
	m = cl->class_class_members;
	member_ગણતરી = cl->class_class_member_ગણતરી;
    }

    જો (member_idx >= member_ગણતરી)
	પાછા ખાલી;

    પાછા m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    સ્થિર શૂન્ય
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    શૂન્ય
typealias_unref(typealias_T *ta)
{
    જો (ta != ખાલી && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    શૂન્ય
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    જો (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_in_vim9_script));
	પાછા;
    }

    જો (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	પાછા;
    }

    જો (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_start_with_uppercase_letter_str), arg);
	પાછા;
    }

    char_u *name_end = find_name_end(arg, ખાલી, ખાલી, FNE_CHECK_START);
    જો (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	પાછા;
    }
    char_u *name_start = arg;

    arg = skipwhite(name_end);
    જો (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	પાછા;
    }
    જો (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	પાછા;
    }
    arg++;
    arg = skipwhite(arg);

    જો (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	પાછા;
    }

    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, ખાલી, ખાલી, TRUE);
    જો (type == ખાલી)
	પાછા;

    જો (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	પાછા;
    }

    પૂર્ણાંક cc = *name_end;
    *name_end = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    જો (eval_variable_import(name_start, &tv) == OK)
    {
	જો (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_start);
	નહીંતર
	    semsg(_(e_redefining_script_item_str), name_start);
	clear_tv(&tv);
	જાઓ done;
    }

    // Create a script-local variable for the type alias.
    જો (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_lock = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_start);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_depth = parse_type_depth();
    }
    નહીંતર
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_lock = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    set_var_const(name_start, current_sctx.sc_sid, ખાલી, &tv, FALSE,
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
    પૂર્ણાંક
get_member_tv(
    class_T	*cl,
    પૂર્ણાંક		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    પૂર્ણાંક		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    જો (m == ખાલી)
	પાછા FAIL;

    જો (*name == '_' && (current_class == ખાલી ||
				!class_instance_of(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	પાછા FAIL;
    }

    જો (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	set_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    નહીંતર
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	set_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    પાછા OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    સ્થિર પૂર્ણાંક
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
    પૂર્ણાંક		argcount = 0;
    ocmember_T	*ocm = ખાલી;
    પૂર્ણાંક		m_idx;

    fp = method_lookup(cl, rettv->v_type, name, len, ખાલી);
    જો (fp == ખાલી)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	જો (ocm == ખાલી || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    method_not_found_સંદેશ(cl, rettv->v_type, name, len);
	    પાછા FAIL;
	}

	જો (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    પાછા FAIL;
	}

	જો (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	નહીંતર
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_end;
	પાછા OK;
    }

    જો (ocm == ખાલી && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
	પાછા FAIL;
    }

    // process generic function call
    જો (fp != ખાલી)
    {
	fp = generic_func_get(fp, gfatab);
	જો (fp == ખાલી)
	    પાછા FAIL;
    }

    char_u *argp = name_end;
    પૂર્ણાંક ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    જો (ret == FAIL)
    {
	જ્યારે (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	પાછા FAIL;
    }

    funcexe_T funcexe;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    જો (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    જો (evalarg != ખાલી)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    પૂર્ણાંક error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								ખાલી);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    માટે (પૂર્ણાંક idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    જો (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_found_var);
	પાછા FAIL;
    }
    *arg = argp;

    પાછા OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    પૂર્ણાંક
obj_method_to_partial_tv(object_T *obj, ufunc_T *obj_method, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    જો (pt == ખાલી)
	પાછા FAIL;

    pt->pt_refcount = 1;
    જો (obj != ખાલી)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_method;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    પાછા OK;
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
    પૂર્ણાંક
class_object_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    પૂર્ણાંક		verbose UNUSED)	// give error messages
{
    પૂર્ણાંક	ret = FAIL;

    જો (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	પાછા FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ખાલી, ખાલી, FNE_CHECK_START);
    જો (name_end == name)
	પાછા FAIL;
    size_t len = name_end - name;

    પૂર્ણાંક did_emsg_save = did_emsg;
    class_T *cl;
    જો (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    નહીંતર // VAR_OBJECT
    {
	જો (rettv->vval.v_object == ખાલી)
	{
	    emsg(_(e_using_null_object));
	    પાછા FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    જો (cl == ખાલી)
    {
	emsg(_(e_incomplete_type));
	પાછા FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    જો (*name_end == '<')
    {
	cctx_T	    *cctx = ખાલી;

	જો (evalarg != ખાલી)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_end = parse_generic_func_type_args(name, len, name + len,
						&gfatab, cctx);
	જો (name_end == ખાલી)
	    જાઓ done;
    }

    જો (*name_end == '(')
	// Invoke the class or object method
	ret = call_oc_method(cl, name, len, name_end, evalarg, arg,
							&gfatab, rettv);
    નહીંતર જો (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	પૂર્ણાંક is_object = rettv->v_type == VAR_OBJECT;
	જો (get_member_tv(cl, is_object, name, len, ખાલી, rettv) == OK)
	{
	    *arg = name_end;
	    ret = OK;
	    જાઓ done;
	}

	// could be a class method or an object method
	પૂર્ણાંક	fidx;
	ufunc_T	*fp = method_lookup(cl, rettv->v_type, name, len, &fidx);
	જો (fp != ખાલી)
	{
	    // Protected methods are not accessible outside the class
	    જો (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		જાઓ done;
	    }

	    જો (obj_method_to_partial_tv(is_object ? rettv->vval.v_object :
						ખાલી, fp, rettv) == FAIL)
		જાઓ done;

	    *arg = name_end;
	    ret = OK;
	    જાઓ done;
	}

	જો (did_emsg == did_emsg_save)
	    member_not_found_સંદેશ(cl, rettv->v_type, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);

    પાછા ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
find_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ખાલી, ખાલી, FNE_CHECK_START);
    જો (name_end == name || *name_end != '.')
	પાછા ખાલી;

    ufunc_T	*fp = ખાલી;
    size_t	len = name_end - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    જો (eval_variable(name, (પૂર્ણાંક)len,
				    0, &tv, ખાલી, EVAL_VAR_NOAUTOLOAD) == FAIL)
	પાછા ખાલી;
    જો (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	જાઓ fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    જો (cl == ખાલી)
	જાઓ fail_after_eval;
    char_u *fname = name_end + 1;
    char_u *fname_end = find_name_end(fname, ખાલી, ખાલી, FNE_CHECK_START);
    જો (fname_end == fname)
	જાઓ fail_after_eval;
    len = fname_end - fname;

    fp = method_lookup(cl, tv.v_type, fname, len, ખાલી);

    જો (fp != ખાલી)
    {
	fp = eval_generic_func(fp, fname, &fname_end);
	જો (fp != ખાલી)
	    *arg = fname_end;
    }

fail_after_eval:
    clear_tv(&tv);
    પાછા fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    પૂર્ણાંક
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    પૂર્ણાંક idx;
    class_member_lookup(cl, name, namelen, &idx);
    પાછા idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, પૂર્ણાંક *idx)
{
    ocmember_T	*ret_m = ખાલી;
    પૂર્ણાંક		ret_idx = -1;
    માટે (પૂર્ણાંક i = 0; i < cl->class_class_member_ગણતરી; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	જો (namelen)
	{
	    જો (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		તોડો;
	    }
	}
	નહીંતર જો (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    તોડો;
	}
    }
    જો (idx != ખાલી)
	*idx = ret_idx;
    પાછા ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    સ્થિર ufunc_T *
class_method_lookup(class_T *cl, char_u *name, size_t namelen, પૂર્ણાંક *idx)
{
    ufunc_T	*ret_fp = ખાલી;
    પૂર્ણાંક		ret_idx = -1;
    માટે (પૂર્ણાંક i = 0; i < cl->class_class_function_ગણતરી; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	જો (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    તોડો;
	}
    }
    જો (idx != ખાલી)
	*idx = ret_idx;
    પાછા ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    પૂર્ણાંક
class_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    પૂર્ણાંક idx;
    class_method_lookup(cl, name, namelen, &idx);
    પાછા idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    સ્થિર પૂર્ણાંક
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    પૂર્ણાંક idx;
    object_member_lookup(cl, name, namelen, &idx);
    પાછા idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, પૂર્ણાંક *idx)
{
    ocmember_T	*ret_m = ખાલી;
    પૂર્ણાંક		ret_idx = -1;
    માટે (પૂર્ણાંક i = 0; i < cl->class_obj_member_ગણતરી; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	જો (namelen)
	{
	    જો (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		તોડો;
	    }
	}
	નહીંતર જો (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    તોડો;
	}
    }
    જો (idx != ખાલી)
	*idx = ret_idx;
    પાછા ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    સ્થિર ufunc_T *
object_method_lookup(class_T *cl, char_u *name, size_t namelen, પૂર્ણાંક *idx)
{
    ufunc_T	*ret_fp = ખાલી;
    પૂર્ણાંક		ret_idx = -1;
    માટે (પૂર્ણાંક i = 0; i < cl->class_obj_method_ગણતરી; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	જો (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    તોડો;
	}
    }
    જો (idx != ખાલી)
	*idx = ret_idx;
    પાછા ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    પૂર્ણાંક
object_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    પૂર્ણાંક idx;
    object_method_lookup(cl, name, namelen, &idx);
    પાછા idx;
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
    પૂર્ણાંક		*idx)
{
    જો (v_type == VAR_CLASS)
	પાછા class_member_lookup(cl, name, namelen, idx);

    પાછા object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    સ્થિર class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_found = ખાલી;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_found = ખાલી;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    માટે (class_T *super = cl; super != ખાલી; super = super->class_extends)
    {
	class_T		*cl_tmp = ખાલી;
	ocmember_T	*m = ખાલી;
	જો (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    જો ((m = object_member_lookup(super, name, len, ખાલી)) != ખાલી)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	જો (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    જો (( m = class_member_lookup(super, name, len, ખાલી)) != ખાલી)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	જો (cl_tmp == ખાલી)
	    તોડો;  // member is not in this or any super class.
	cl_found = cl_tmp;
	m_found = m;
    }
    જો (p_m != ખાલી)
	*p_m = m_found;
    પાછા cl_found;
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
    પૂર્ણાંક		*idx)
{
    જો (v_type == VAR_CLASS)
	પાછા class_method_lookup(cl, name, namelen, idx);
    નહીંતર
	પાછા object_method_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    પૂર્ણાંક
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    માટે (cctx_T *cctx = cctx_arg; cctx != ખાલી; cctx = cctx->ctx_outer)
	જો (cctx->ctx_ufunc != ખાલી
			&& class_instance_of(cctx->ctx_ufunc->uf_class, cl))
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    પૂર્ણાંક
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    જો (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_in_class_str),
		m->ocm_name.string, cl->class_name.string);
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    શૂન્ય
obj_lock_const_vars(object_T *obj)
{
    માટે (પૂર્ણાંક i = 0; i < obj->obj_class->class_obj_member_ગણતરી; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	જો (ocm->ocm_flags & OCMFLAG_CONST)
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
    પૂર્ણાંક		sz;

    જો (cl == ખાલી)
	પાછા ખાલી;

    sz = કદ(object_T) + cl->class_obj_member_ગણતરી * કદ(typval_T);
    obj = alloc_clear(sz);
    જો (obj == ખાલી)
	પાછા ખાલી;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    જો (IS_ENUM(cl))
	enum_set_internal_obj_vars(cl, obj);

    પાછા obj;
}

/*
 * Make a copy of an object.
 */
    શૂન્ય
copy_object(typval_T *from, typval_T *to)
{
    જો (from->vval.v_object == ખાલી)
	to->vval.v_object = ખાલી;
    નહીંતર
    {
	to->vval.v_object = from->vval.v_object;
	++to->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    શૂન્ય
copy_class(typval_T *from, typval_T *to)
{
    જો (from->vval.v_class == ખાલી)
	to->vval.v_class = ખાલી;
    નહીંતર
    {
	to->vval.v_class = from->vval.v_class;
	++to->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    સ્થિર શૂન્ય
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    માટે (પૂર્ણાંક i = 0; i < cl->class_interface_ગણતરી; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	જો (cl->class_interfaces_cl[i] != ખાલી)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *next;
    માટે (itf2class_T *i2c = cl->class_itf2class; i2c != ખાલી; i2c = next)
    {
	next = i2c->i2c_next;
	vim_free(i2c);
    }

    માટે (પૂર્ણાંક i = 0; i < cl->class_class_member_ગણતરી; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	જો (cl->class_members_tv != ખાલી)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    માટે (પૂર્ણાંક i = 0; i < cl->class_obj_member_ગણતરી; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    માટે (પૂર્ણાંક i = 0; i < cl->class_class_function_ગણતરી; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	જો (IS_ENUM(cl) && IS_CONSTRUCTOR_METHOD(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    માટે (પૂર્ણાંક i = 0; i < cl->class_obj_method_ગણતરી; ++i)
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
    સ્થિર પૂર્ણાંક
class_get_selfrefs(class_T *cl)
{
    પૂર્ણાંક		self_refs = 0;
    typval_T	*tv;

    માટે (પૂર્ણાંક i = 0; i < cl->class_class_member_ગણતરી; ++i)
    {
	tv = &cl->class_members_tv[i];
	જો (tv->v_type == VAR_OBJECT && tv->vval.v_object != ખાલી
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    પાછા self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    સ્થિર પૂર્ણાંક
can_free_enum(class_T *cl)
{
    માટે (પૂર્ણાંક i = 0; i < cl->class_class_member_ગણતરી; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	જો (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    પાછા TRUE;

	જો (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    જો (tv->vval.v_list->lv_refcount > 1)
		પાછા FALSE;
	    તોડો;
	}

	જો (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    પાછા FALSE;
    }

    પાછા TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    સ્થિર પૂર્ણાંક
can_free_class(class_T *cl)
{
    પૂર્ણાંક		can_free = FALSE;
    પૂર્ણાંક		self_refs = 0;

    જો (IS_ENUM(cl) && !can_free_enum(cl))
	પાછા FALSE;

    જો (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    જો (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    પાછા can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    શૂન્ય
class_unref(class_T *cl)
{
    જો (cl == ખાલી)
	પાછા;

    --cl->class_refcount;

    જો (cl->class_name.string == ખાલી)
	પાછા;

    જો (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    પૂર્ણાંક
class_free_nonref(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		did_free = FALSE;

    માટે (class_T *cl = first_class; cl != ખાલી; cl = next_nonref_class)
    {
	next_nonref_class = cl->class_next_used;
	જો ((cl->class_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    next_nonref_class = ખાલી;
    પાછા did_free;
}

    પૂર્ણાંક
set_ref_in_classes(પૂર્ણાંક copyID)
{
    માટે (class_T *cl = first_class; cl != ખાલી; cl = cl->class_next_used)
	set_ref_in_item_class(cl, copyID, ખાલી, ખાલી, ખાલી);

    પાછા FALSE;
}

સ્થિર object_T *first_object = ખાલી;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    શૂન્ય
object_created(object_T *obj)
{
    જો (first_object != ખાલી)
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
    સ્થિર શૂન્ય
object_cleared(object_T *obj)
{
    જો (obj->obj_next_used != ખાલી)
	obj->obj_next_used->obj_prev_used = obj->obj_prev_used;
    જો (obj->obj_prev_used != ખાલી)
	obj->obj_prev_used->obj_next_used = obj->obj_next_used;
    નહીંતર જો (first_object == obj)
	first_object = obj->obj_next_used;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    સ્થિર શૂન્ય
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    જો (!cl)
	પાછા;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = INT_MAX;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    માટે (પૂર્ણાંક i = 0; i < cl->class_obj_member_ગણતરી; ++i)
	clear_tv(tv + i);
}

    સ્થિર શૂન્ય
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    જો (!cl)
	પાછા;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    સ્થિર શૂન્ય
object_free(object_T *obj)
{
    જો (in_free_unref_વસ્તુઓ)
	પાછા;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    શૂન્ય
object_unref(object_T *obj)
{
    જો (obj != ખાલી && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    પૂર્ણાંક
object_free_nonref(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		did_free = FALSE;

    માટે (object_T *obj = first_object; obj != ખાલી; obj = obj->obj_next_used)
    {
	જો ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    પાછા did_free;
}

    શૂન્ય
object_free_વસ્તુઓ(પૂર્ણાંક copyID)
{
    object_T	*obj_next;

    માટે (object_T *obj = first_object; obj != ખાલી; obj = obj_next)
    {
	obj_next = obj->obj_next_used;
	જો ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    શૂન્ય
emsg_var_cl_define(અક્ષર *સંદેશ, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    જો (cl_def != ખાલી)
	semsg(_(સંદેશ), m->ocm_name.string, cl_def->class_name.string);
    નહીંતર
	emsg(_(e_internal_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    શૂન્ય
method_not_found_સંદેશ(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *method_name = vim_strnsave(name, len);
    જો ((v_type == VAR_OBJECT)
	    && (class_method_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	જો (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	નહીંતર
	    semsg(_(e_class_method_str_accessible_only_using_class_str),
		    method_name, cl->class_name.string);
    }
    નહીંતર જો ((v_type == VAR_CLASS)
	    && (object_method_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	જો (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	નહીંતર
	    semsg(_(e_object_method_str_accessible_only_using_object_str),
		    method_name, cl->class_name.string);
    }
    નહીંતર
	semsg(_(e_method_not_found_on_class_str_str), method_name,
		cl->class_name.string);
    vim_free(method_name);
}

/*
 * Echo a class or object member not found message.
 */
    શૂન્ય
member_not_found_સંદેશ(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    જો (v_type == VAR_OBJECT)
    {
	જો (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	નહીંતર
	    semsg(_(e_variable_not_found_on_object_str_str), varname,
		    cl->class_name.string);
    }
    નહીંતર
    {
	જો (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	નહીંતર
	{
	    જો (IS_ENUM(cl))
		semsg(_(e_enum_કિંમત_str_not_found_in_enum_str),
			varname, cl->class_name.string);
	    નહીંતર
		semsg(_(e_class_variable_str_not_found_in_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    શૂન્ય
defcompile_class(class_T *cl)
{
    માટે (પૂર્ણાંક loop = 1; loop <= 2; ++loop)
    {
	પૂર્ણાંક func_ગણતરી = loop == 1 ? cl->class_class_function_ગણતરી
						: cl->class_obj_method_ગણતરી;
	માટે (પૂર્ણાંક i = 0; i < func_ગણતરી; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    જો (!IS_ABSTRACT_METHOD(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    શૂન્ય
defcompile_classes_in_script(શૂન્ય)
{
    માટે (class_T *cl = first_class; cl != ખાલી; cl = cl->class_next_used)
    {
	જો (eval_variable(cl->class_name.string, 0, 0, ખાલી, ખાલી,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    પૂર્ણાંક
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    જો (eval_variable(name, 0, 0, rettv, ખાલી, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	પાછા rettv->v_type == VAR_CLASS;
    પાછા FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
object_call_builtin_method(
    object_T		*obj,
    class_builtin_T	builtin_method,
    પૂર્ણાંક			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    પૂર્ણાંક	    midx;

    જો (obj == ખાલી)
	પાછા FAIL;

    uf = class_get_builtin_method(obj->obj_class, builtin_method, &midx);
    જો (uf == ખાલી)
	પાછા FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    પૂર્ણાંક		r;

    જો (fc == ખાલી)
	પાછા FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, ખાલી, obj, fc, rettv);

    remove_funccal();

    પાછા r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    પૂર્ણાંક
object_empty(object_T *obj)
{
    typval_T	rettv;

    જો (object_call_builtin_method(obj, CLASS_BUILTIN_EMPTY, 0, ખાલી, &rettv)
								== FAIL)
	પાછા TRUE;

    પાછા tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    પૂર્ણાંક
object_len(object_T *obj)
{
    typval_T	rettv;

    જો (object_call_builtin_method(obj, CLASS_BUILTIN_LEN, 0, ખાલી, &rettv)
								== FAIL)
	પાછા 0;

    પાછા tv_to_સંખ્યા(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    પૂર્ણાંક
object_equal(
	object_T *o1,
	object_T *o2,
	પૂર્ણાંક	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    જો (o1 == o2)
	પાછા TRUE;
    જો (o1 == ખાલી || o2 == ખાલી)
	પાછા FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    જો (cl1 != cl2 || cl1 == ખાલી || cl2 == ખાલી)
	પાછા FALSE;

    માટે (પૂર્ણાંક i = 0; i < cl1->class_obj_member_ગણતરી; ++i)
	જો (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    પાછા FALSE;

    પાછા TRUE;
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
    પૂર્ણાંક		copyID,
    પૂર્ણાંક		echo_style,
    પૂર્ણાંક		restore_copyID,
    પૂર્ણાંક		composite_val)
{
    typval_T	rettv;

    જો (object_call_builtin_method(obj, CLASS_BUILTIN_STRING, 0, ખાલી, &rettv)
								== OK
					&& rettv.vval.v_string != ખાલી)
	પાછા rettv.vval.v_string;

    પૂર્ણાંક		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    જો (cl != ખાલી && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    નહીંતર
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	જો (cl == ખાલી)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	નહીંતર
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
    }
    જો (cl != ખાલી)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	માટે (પૂર્ણાંક i = 0; i < cl->class_obj_member_ગણતરી; ++i)
	{
	    જો (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.length);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = ખાલી;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, composite_val);
	    જો (s != ખાલી)
		ga_concat(&ga, s);
	    vim_free(tf);
	    જો (s == ખાલી || did_echo_string_emsg)
	    {
		ok = FAIL;
		તોડો;
	    }
	    લીટી_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    જો (ok == FAIL)
    {
	vim_free(ga.ga_data);
	પાછા ખાલી;
    }
    ga_append(&ga, NUL);
    પાછા (char_u *)ga.ga_data;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    પૂર્ણાંક
class_instance_of(class_T *cl, class_T *other_cl)
{
    જો (cl == other_cl)
	પાછા TRUE;

    // Recursively check the base classes.
    માટે (; cl != ખાલી; cl = cl->class_extends)
    {
	જો (cl == other_cl)
	    પાછા TRUE;
	// Check the implemented interfaces and the super interfaces
	માટે (પૂર્ણાંક i = cl->class_interface_ગણતરી - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    જ્યારે (intf != ખાલી)
	    {
		જો (intf == other_cl)
		    પાછા TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    પાછા FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    શૂન્ય
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_સંખ્યા = VVAL_FALSE;

    જો (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_args(argvars, 1) == FAIL)
	પાછા;

    જો (object_tv->vval.v_object == ખાલી)
    {
	જો (classinfo_tv->vval.v_class == ખાલી)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_સંખ્યા = VVAL_TRUE;
	પાછા;
    }

    માટે (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	જો (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	નહીંતર
	    c = classinfo_tv->vval.v_class;

	જો (class_instance_of(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_સંખ્યા = VVAL_TRUE;
	    પાછા;
	}
    }
}

#endif // FEAT_EVAL
