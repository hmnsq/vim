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
#include "vim.h"

#གལ་སྲིད defined(FEAT_EVAL)

static class_T *first_class = NULL;
static class_T *next_nonref_class = NULL;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    static void
class_created(class_T *cl)
{
    གལ་སྲིད (first_class != NULL)
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
    static void
class_cleared(class_T *cl)
{
    གལ་སྲིད (cl->class_next_used != NULL)
	cl->class_next_used->class_prev_used = cl->class_prev_used;
    གལ་སྲིད (cl->class_prev_used != NULL)
	cl->class_prev_used->class_next_used = cl->class_next_used;
    དེ་མིན གལ་སྲིད (first_class == cl)
	first_class = cl->class_next_used;

    // update the next class to check if needed
    གལ་སྲིད (cl == next_nonref_class)
	next_nonref_class = cl->class_next_used;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    static ཧྲིལ
parse_member(
    exarg_T	*eap,
    char_u	*line,
    char_u	*varname,
    ཧྲིལ		has_public,	    // TRUE if "public" seen before "varname"
    char_u	**varname_end,
    ཧྲིལ		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_expr)
{
    *varname_end = to_name_end(varname, FALSE);
    གལ་སྲིད (*varname == '_' && has_public)
    {
	semsg(_(e_public_variable_name_cannot_start_with_underscore_str), line);
	ལོག FAIL;
    }

    char_u *colon = skipwhite(*varname_end);
    char_u *type_arg = colon;
    type_T *type = NULL;
    *has_type = FALSE;
    གལ་སྲིད (*colon == ':')
    {
	གལ་སྲིད (VIM_ISWHITE(**varname_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    ལོག FAIL;
	}
	གལ་སྲིད (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    ལོག FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, NULL, NULL, TRUE);
	གལ་སྲིད (type == NULL || !valid_declaration_type(type))
	    ལོག FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    གལ་སྲིད (type == NULL && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	ལོག FAIL;
    }

    གལ་སྲིད (init_expr == NULL && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_in_interface));
	ལོག FAIL;
    }

    གལ་སྲིད (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *expr_start, *expr_end;

	གལ་སྲིད (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    ལོག FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(void)skip_expr_concatenate(&init_arg, &expr_start, &expr_end, &evalarg);

	init_arg = skipwhite(init_arg);
	གལ་སྲིད (*init_arg != NUL && !vim9_comment_start(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, NULL);
	    ལོག FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	གལ་སྲིད (type == NULL)
	    type = &t_any;

	*init_expr = vim_strnsave(expr_start, expr_end - expr_start);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, NULL);
    }
    དེ་མིན གལ་སྲིད (!valid_declaration_type(type))
	ལོག FAIL;

    *type_ret = type;
    ལོག OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    static ཧྲིལ
add_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_end,
    ཧྲིལ		has_public,
    ཧྲིལ		has_final,
    ཧྲིལ		has_const,
    ཧྲིལ		has_type,
    type_T	*type,
    char_u	*init_expr)
{
    གལ་སྲིད (ga_grow(gap, 1) == FAIL)
	ལོག FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_data) + gap->ga_len;
    m->ocm_name.length = (size_t)(varname_end - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.length);
    གལ་སྲིད (m->ocm_name.string == NULL)
    {
	m->ocm_name.length = 0;
	ལོག FAIL;
    }
    m->ocm_access = has_public ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_PRIVATE : VIM_ACCESS_READ;
    གལ་སྲིད (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    གལ་སྲིད (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    གལ་སྲིད (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    གལ་སྲིད (init_expr != NULL)
    {
	m->ocm_init = init_expr;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    ལོག OK;
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
    static ཧྲིལ
add_members_to_class(
    garray_T	*gap,
    ocmember_T	*parent_members,
    ཧྲིལ		parent_count,
    ocmember_T	**members,
    ཧྲིལ		*member_count)
{
    *member_count = parent_count + gap->ga_len;
    *members = *member_count == 0 ? NULL
				       : ALLOC_MULT(ocmember_T, *member_count);
    གལ་སྲིད (*member_count > 0 && *members == NULL)
	ལོག FAIL;
    དོན་ལུ (ཧྲིལ i = 0; i < parent_count; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = parent_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.length);
	གལ་སྲིད (m->ocm_name.string == NULL)
	{
	    m->ocm_name.length = 0;
	    ལོག FAIL;
	}
	གལ་སྲིད (m->ocm_init != NULL)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    གལ་སྲིད (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + parent_count,
			       gap->ga_data, parimaa_na(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_data);
    ལོག OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    ཧྲིལ
object_index_from_itf_index(class_T *itf, ཧྲིལ is_method, ཧྲིལ idx, class_T *cl)
{
    གལ་སྲིད (idx >= (is_method ? itf->class_obj_method_count
				   : itf->class_obj_member_count))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	ལོག 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    གལ་སྲིད (cl == itf)
	ལོག idx;

    itf2class_T		*i2c = NULL;
    ཧྲིལ			searching = TRUE;
    ཧྲིལ			method_offset = 0;

    དོན་ལུ (class_T *super = cl; super != NULL && searching;
						super = super->class_extends)
    {
	དོན་ལུ (i2c = itf->class_itf2class; i2c != NULL; i2c = i2c->i2c_next)
	{
	    གལ་སྲིད (i2c->i2c_class == super && i2c->i2c_is_method == is_method)
	    {
		searching = FALSE;
		bha_nga;
	    }
	}
	གལ་སྲིད (searching && is_method)
	    // The parent class methods are stored after the current class
	    // methods.
	    method_offset += super->class_obj_method_count_child;
    }
    གལ་སྲིད (i2c == NULL)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	ལོག 0;
    }

    // A table follows the i2c for the class
    ཧྲིལ *table = (ཧྲིལ *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    ལོག table[idx] + method_offset;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    static ཧྲིལ
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    ཧྲིལ	    is_class)
{
    typval_T	tv;
    ཧྲིལ		success = FALSE;

    གལ་སྲིད (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	ལོག success;
    }

    tv.v_type = VAR_UNKNOWN;
    གལ་སྲིད (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_found_str), extends_name);
	ལོག success;
    }

    གལ་སྲིད (tv.v_type != VAR_CLASS || tv.vval.v_class == NULL
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    དེ་མིན
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    ལོག success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    static ཧྲིལ
validate_extends_generic_method(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    var_na	*msg = NULL;

    གལ་སྲིད (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	ལོག TRUE;

    གལ་སྲིད (super_fp->uf_generic_argcount == 0)
	msg = e_concrete_method_str_override_with_generic_method_in_class_str;
    དེ་མིན གལ་སྲིད (cl_fp->uf_generic_argcount == 0)
	msg = e_generic_method_str_override_with_concrete_method_in_class_str;
    དེ་མིན
	msg = e_generic_method_str_type_arguments_mismatch_in_class_str;

    semsg(_(msg), cl_fp->uf_name, super_cl->class_name.string);
    ལོག FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    static ཧྲིལ
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    ཧྲིལ		method_count = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_data);

    སྐབས (super != NULL)
    {
	ཧྲིལ extends_method_count = super->class_obj_method_count_child;
	གལ་སྲིད (extends_method_count == 0)
	{
	    super = super->class_extends;
	    continue;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	དོན་ལུ (ཧྲིལ i = 0; i < extends_method_count; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    ཧྲིལ	    extends_private = (*pstr == '_');
	    གལ་སྲིད (extends_private)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    དོན་ལུ (ཧྲིལ j = 0; j < method_count; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		ཧྲིལ	priv_method = (*qstr == '_');
		གལ་སྲིད (priv_method)
		    qstr++;
		གལ་སྲིད (STRCMP(pstr, qstr) == 0)
		{
		    གལ་སྲིད (priv_method != extends_private)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_method_str_of_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			ལོག FALSE;
		    }

		    གལ་སྲིད (!validate_extends_generic_method(super,
						extends_methods[i], cl_fp[j]))
			    ལོག FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    ལོག TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    static ཧྲིལ
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ཧྲིལ member_count = objmembers_gap->ga_len;
    གལ་སྲིད (member_count == 0)
	ལོག TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    དོན་ལུ (ཧྲིལ c_i = 0; c_i < member_count; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	སྐབས (p_cl != NULL)
	{
	    ཧྲིལ p_member_count = p_cl->class_obj_member_count;

	    གལ་སྲིད (p_member_count == 0)
	    {
		p_cl = p_cl->class_extends;
		continue;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    དོན་ལུ (ཧྲིལ p_i = 0; p_i < p_member_count; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		གལ་སྲིད (STRCMP(pstr, qstr) == 0)
		{
		    གལ་སྲིད (c_protected != p_protected)
			semsg(_(e_public_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    དེ་མིན
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    ལོག FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    ལོག TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    static ཧྲིལ
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ཧྲིལ member_count = objmembers_gap->ga_len;
    གལ་སྲིད (member_count == 0)
	ལོག TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    དོན་ལུ (ཧྲིལ c_i = 0; c_i < member_count; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	ཧྲིལ	    var_found = FALSE;

	// Check in all the parent classes in the lineage
	སྐབས (p_cl != NULL && !var_found)
	{
	    ཧྲིལ p_member_count = p_cl->class_obj_member_count;
	    གལ་སྲིད (p_member_count == 0)
	    {
		p_cl = p_cl->class_extends;
		continue;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    དོན་ལུ (ཧྲིལ p_i = 0; p_i < p_member_count; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		གལ་སྲིད (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    continue;

		// Ensure the type is matching.
		where.wt_func_name = (var_na *)c_m->ocm_name.string;
		where.wt_kind = WT_MEMBER;

		གལ་སྲིད (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    ལོག FALSE;

		var_found = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    ལོག TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    static ཧྲིལ
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    དོན་ལུ (ཧྲིལ loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	ཧྲིལ extends_method_count = loop == 1
				? extends_cl->class_class_function_count
				: extends_cl->class_obj_method_count;
	གལ་སྲིད (extends_method_count == 0)
	    continue;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	ཧྲིལ method_count = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_data
						: objmethods_gap->ga_data);

	དོན་ལུ (ཧྲིལ i = 0; i < extends_method_count; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    གལ་སྲིད (!IS_ABSTRACT_METHOD(uf))
		continue;

	    ཧྲིལ	concrete_method_found = FALSE;
	    ཧྲིལ	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    དོན་ལུ (j = 0; !concrete_method_found && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		གལ་སྲིད (!IS_ABSTRACT_METHOD(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_method_found = TRUE;
	    }

	    གལ་སྲིད (concrete_method_found)
		continue;

	    དོན་ལུ (j = 0; j < method_count; j++)
	    {
		གལ་སྲིད (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_method_found = TRUE;
		    bha_nga;
		}
	    }

	    གལ་སྲིད (!concrete_method_found)
	    {
		semsg(_(e_abstract_method_str_not_implemented), uf->uf_name);
		ལོག FALSE;
	    }
	}
    }

    ལོག TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    static ཧྲིལ
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    ཧྲིལ		cl_member_count,
    class_T	*extends_cl)
{
    ཧྲིལ		variable_present  = FALSE;

    དོན་ལུ (ཧྲིལ cl_i = 0; cl_i < cl_member_count; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	གལ་སྲིད (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    continue;

	// Ensure the access type is same
	གལ་སྲིད (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_of_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    ལོག FALSE;
	}

	// Ensure the type is matching.
	གལ་སྲིད (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	དེ་མིན
	{
	    where.wt_func_name = (var_na *)m->ocm_name.string;
	    where.wt_kind = WT_MEMBER;
	    གལ་སྲིད (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		ལོག FALSE;
	}

	variable_present = TRUE;
	bha_nga;
    }

    གལ་སྲིད (!variable_present && extends_cl != NULL)
    {
	ཧྲིལ ext_cl_count = extends_cl->class_obj_member_count;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	ལོག intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_count,
					extends_cl->class_extends);
    }

    ལོག variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    static ཧྲིལ
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ཧྲིལ if_count = ifcl->class_obj_member_count;
    གལ་སྲིད (if_count == 0)
	ལོག TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_data);
    ཧྲིལ cl_count = objmembers_gap->ga_len;
    དོན་ལུ (ཧྲིལ if_i = 0; if_i < if_count; ++if_i)
    {
	གལ་སྲིད (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_count, extends_cl))
	{
	    semsg(_(e_variable_str_of_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    ལོག FALSE;
	}
    }

    ལོག TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    static ཧྲིལ
intf_method_type_matches(
    ufunc_T	*if_method,
    ufunc_T	*cl_method,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    གལ་སྲིད (cl_method->uf_generic_argcount != if_method->uf_generic_argcount)
    {
	semsg(_(e_type_var_count_of_method_str_differs_from_interface_str),
						cl_method->uf_name, intf_name);
	ལོག FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (var_na *)if_method->uf_name;
    where.wt_kind = WT_METHOD;
    གལ་སྲིད (check_type(if_method->uf_func_type, cl_method->uf_func_type, TRUE,
								where) == FAIL)
	ལོག FALSE;

    ལོག TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    static ཧྲིལ
intf_method_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    ཧྲིལ	    cl_count,
    class_T *extends_cl,
    char_u  *intf_name)
{
    ཧྲིལ		method_present  = FALSE;

    དོན་ལུ (ཧྲིལ cl_i = 0; cl_i < cl_count; ++cl_i)
    {
	གལ་སྲིད (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    གལ་སྲིད (!intf_method_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		ལོག FALSE;
	    method_present = TRUE;
	    bha_nga;
	}
    }

    གལ་སྲིད (!method_present && extends_cl != NULL)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	ཧྲིལ	ext_cl_count = extends_cl->class_obj_method_count;
	ལོག intf_method_present(if_ufunc, ext_cl_fp, ext_cl_count,
					extends_cl->class_extends, intf_name);
    }

    ལོག method_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    static ཧྲིལ
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    ཧྲིལ if_count = ifcl->class_obj_method_count;
    གལ་སྲིད (if_count == 0)
	ལོག TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_data);
    ཧྲིལ cl_count = objmethods_gap->ga_len;
    དོན་ལུ (ཧྲིལ if_i = 0; if_i < if_count; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	གལ་སྲིད (!intf_method_present(if_fp[if_i], cl_fp, cl_count, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_method_str_of_interface_str_not_implemented),
		    if_name, intf_class_name);
	    ལོག FALSE;
	}
    }

    ལོག TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    static ཧྲིལ
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ཧྲིལ		success = TRUE;

    དོན་ལུ (ཧྲིལ i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_data)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	གལ་སྲིད (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_found_str), impl);
	    success = FALSE;
	    bha_nga;
	}

	གལ་སྲིད (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == NULL
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    bha_nga;
	}

	class_T *ifcl = tv.vval.v_class;
	གལ་སྲིད (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    bha_nga;
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

    གལ་སྲིད (!success)
	ལོག FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_data;
    cl->class_interface_count = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    དོན་ལུ (ཧྲིལ i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_data)[i],
		((class_T **)intf_classes_gap->ga_data)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = NULL;
    cl->class_interface_count = 0;
    cl->class_extends = NULL;

    ལོག success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    static ཧྲིལ
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    དོན་ལུ (ཧྲིལ j = 0; j < intf_classes_gap->ga_len; j++)
    {
	གལ་སྲིད (((class_T **)intf_classes_gap->ga_data)[j] == ifcl)
	    ལོག TRUE;
    }

    ལོག FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    static ཧྲིལ
add_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.length);
    གལ་སྲིད (intf_name == NULL)
	ལོག FALSE;

    གལ་སྲིད (ga_grow(impl_gap, 1) == FAIL)
	goto fail;

    char_u **intf_names = (char_u **)impl_gap->ga_data;
    intf_names[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    གལ་སྲིད (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	goto fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_data;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    ལོག TRUE;
fail:
    vim_free(intf_name);
    ལོག FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    static ཧྲིལ
add_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    དོན་ལུ (ཧྲིལ i = 0; i < super->class_interface_count; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	གལ་སྲིད (!is_interface_class_present(intf_classes_gap, ifcl))
	    add_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    ལོག TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    static ཧྲིལ
check_func_arg_names(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    དོན་ལུ (ཧྲིལ loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	དོན་ལུ (ཧྲིལ fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_data)[fi];

	    དོན་ལུ (ཧྲིལ i = 0; i < uf->uf_args.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_args.ga_data)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		དོན་ལུ (ཧྲིལ mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_data + mi)->ocm_name.string;
		    གལ་སྲིད (STRCMP(aname, mname) == 0)
		    {
			གལ་སྲིད (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_argument_already_declared_in_class_str),
				aname);

			ལོག FALSE;
		    }
		}
	    }
	}
    }

    ལོག TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    static ཧྲིལ
is_reserved_varname(char_u *varname, char_u *varname_end)
{
    ཧྲིལ reserved = FALSE;
    char_u save_varname_end = *varname_end;
    *varname_end = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_end = save_varname_end;

    ལོག reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    static ཧྲིལ
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_end)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_end - varname)};
    ཧྲིལ		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    གལ་སྲིད (*pstr.string == '_')
    {
	pstr.string++;
	pstr.length--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    དོན་ལུ (ཧྲིལ loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	དོན་ལུ (ཧྲིལ i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_data) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.length};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    གལ་སྲིད (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.length--;
		q_protected = true;
	    }

	    གལ་སྲིད (pstr.length == qstr.length
		&& STRNCMP(pstr.string, qstr.string, pstr.length) == 0)
	    {
		char_u	save_c = *varname_end;

		*varname_end = NUL;
		གལ་སྲིད (p_protected != q_protected)
		    semsg(_(e_public_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		དེ་མིན
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_end = save_c;
		dup = TRUE;
		bha_nga;
	    }
	}
    }

    ལོག dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    static ཧྲིལ
is_duplicate_method(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    དོན་ལུ (ཧྲིལ loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	དོན་ལུ (ཧྲིལ i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_data)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    གལ་སྲིད (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		ལོག TRUE;
	    }
	}
    }

    ལོག FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    static ཧྲིལ
is_valid_constructor(ufunc_T *uf, ཧྲིལ is_abstract, ཧྲིལ has_static)
{
    // Constructors are not allowed in abstract classes.
    གལ་སྲིད (is_abstract)
    {
	emsg(_(e_cannot_define_new_method_in_abstract_class));
	ལོག FALSE;
    }
    // A constructor is always static, no need to define it so.
    གལ་སྲིད (has_static)
    {
	emsg(_(e_cannot_define_new_method_as_static));
	ལོག FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    གལ་སྲིད (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_method));
	ལོག FALSE;
    }
    ལོག TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    static ཧྲིལ
object_check_builtin_method_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    ཧྲིལ	    valid = FALSE;
    type_T  method_sig;
    type_T  method_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_FIELD(method_sig);
    CLEAR_FIELD(method_rt);
    method_sig.tt_type = VAR_FUNC;

    གལ་སྲིད (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	method_rt.tt_type = VAR_NUMBER;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    དེ་མིན གལ་སྲིད (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	method_rt.tt_type = VAR_BOOL;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    དེ་མིན གལ་སྲིད (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	method_rt.tt_type = VAR_STRING;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    དེ་མིན
	semsg(_(e_builtin_object_method_str_not_supported), uf->uf_name);

    where.wt_func_name = (var_na *)uf->uf_name;
    where.wt_kind = WT_METHOD;
    གལ་སྲིད (valid && !check_type(&method_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    ལོག valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    ཧྲིལ
is_valid_builtin_obj_methodname(char_u *funcname)
{
    switch (funcname[0])
    {
	pak_sa 'e':
	    ལོག STRNCMP(funcname, "empty", 5) == 0;

	pak_sa 'l':
	    ལོག STRNCMP(funcname, "len", 3) == 0;

	pak_sa 'n':
	    ལོག STRNCMP(funcname, "new", 3) == 0;

	pak_sa 's':
	    ལོག STRNCMP(funcname, "string", 6) == 0;
    }

    ལོག FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_method(
    class_T		*cl,
    class_builtin_T	builtin_method,
    ཧྲིལ			*method_idx)
{
    *method_idx = -1;

    གལ་སྲིད (cl == NULL)
	ལོག NULL;

    *method_idx = cl->class_builtin_methods[builtin_method];
    ལོག *method_idx != -1 ? cl->class_obj_methods[*method_idx] : NULL;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    static ཧྲིལ
update_member_method_lookup_table(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    ཧྲིལ		pobj_method_offset)
{
    གལ་སྲིད (ifcl == NULL)
	ལོག OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(parimaa_na(itf2class_T)
				+ ifcl->class_obj_member_count * parimaa_na(ཧྲིལ));
    གལ་སྲིད (if2cl == NULL)
	ལོག FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = FALSE;

    དོན་ལུ (ཧྲིལ if_i = 0; if_i < ifcl->class_obj_member_count; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	དོན་ལུ (ཧྲིལ cl_i = 0; cl_i < cl->class_obj_member_count; ++cl_i)
	{
	    གལ་སྲིད (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		ཧྲིལ *table = (ཧྲིལ *)(if2cl + 1);
		table[if_i] = cl_i;
		bha_nga;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(parimaa_na(itf2class_T)
				+ ifcl->class_obj_method_count * parimaa_na(ཧྲིལ));
    གལ་སྲིད (if2cl == NULL)
	ལོག FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = TRUE;

    དོན་ལུ (ཧྲིལ if_i = 0; if_i < ifcl->class_obj_method_count; ++if_i)
    {
	ཧྲིལ	done = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	དོན་ལུ (ཧྲིལ cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    གལ་སྲིད (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_data)[cl_i]->uf_name) == 0)
	    {
		ཧྲིལ *table = (ཧྲིལ *)(if2cl + 1);
		table[if_i] = cl_i;
		done = TRUE;
		bha_nga;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	གལ་སྲིད (!done)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    གལ་སྲིད (cl->class_extends != ifcl)
	    {
		class_T		*parent = cl->class_extends;
		ཧྲིལ		method_offset = objmethods->ga_len;

		སྐབས (!done && parent != NULL && parent != ifcl)
		{
		    དོན་ལུ (ཧྲིལ cl_i = 0;
			    cl_i < parent->class_obj_method_count_child; ++cl_i)
		    {
			གལ་སྲིད (STRCMP(if_name,
				    parent->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    ཧྲིལ *table = (ཧྲིལ *)(if2cl + 1);
			    table[if_i] = method_offset + cl_i;
			    done = TRUE;
			    bha_nga;
			}
		    }
		    method_offset += parent->class_obj_method_count_child;
		    parent = parent->class_extends;
		}
	    }

	    གལ་སྲིད (!done)
	    {
		ཧྲིལ *table = (ཧྲིལ *)(if2cl + 1);
		table[if_i] = pobj_method_offset + if_i;
	    }
	}
    }

    ལོག OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    static ཧྲིལ
add_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_interface_count; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	སྐབས (ifcl != NULL)
	{
	    གལ་སྲིད (update_member_method_lookup_table(ifcl, cl, objmethods_gap,
								0) == FAIL)
		ལོག FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    གལ་སྲིད (extends_cl != NULL)
    {
	class_T		*pclass = extends_cl;
	ཧྲིལ		pobj_method_offset = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	སྐབས (pclass != NULL)
	{
	    གལ་སྲིད (update_member_method_lookup_table(pclass, cl,
			objmethods_gap, pobj_method_offset) == FAIL)
		ལོག FAIL;

	    pobj_method_offset += pclass->class_obj_method_count_child;
	    pclass = pclass->class_extends;
	}
    }

    ལོག OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    static ཧྲིལ
add_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_count);
    གལ་སྲིད (cl->class_members_tv == NULL)
	ལོག FAIL;

    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_class_member_count; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	གལ་སྲིད (m->ocm_init != NULL)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_expr(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    གལ་སྲིད (etv == NULL)
		ལོག FAIL;

	    གལ་སྲིད (m->ocm_type->tt_type == VAR_ANY
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
	དེ་མིན
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = NULL;
	}
	set_tv_type(tv, m->ocm_type);
	གལ་སྲིད (m->ocm_flags & OCMFLAG_CONST)
	    item_lock(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    ལོག OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    static void
add_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    ཧྲིལ		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_obj_member_count; ++i)
    {
	གལ་སྲིད (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    continue;

	གལ་སྲིད (i > (is_enum ? 2 : 0))
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

    garray_T lines_to_free;
    ga_init2(&lines_to_free, parimaa_na(char_u *), 50);

    ufunc_T *nf = define_function(&fea, NULL, &lines_to_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_count,
			    NULL);

    ga_clear_strings(&lines_to_free);
    vim_free(fga.ga_data);

    གལ་སྲིད (nf != NULL && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_data)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	གལ་སྲིད (nf->uf_ret_type != NULL)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_args = NULL;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    static ཧྲིལ
add_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    དོན་ལུ (ཧྲིལ loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	ཧྲིལ		*fcount = loop == 1 ? &cl->class_class_function_count
						: &cl->class_obj_method_count;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	ཧྲིལ parent_count = 0;
	གལ་སྲིད (extends_cl != NULL)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    parent_count = loop == 1
				? 0
				: extends_cl->class_obj_method_count;

	*fcount = parent_count + gap->ga_len;
	གལ་སྲིད (*fcount == 0)
	{
	    *fup = NULL;
	    continue;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	གལ་སྲིད (*fup == NULL)
	    ལོག FAIL;

	གལ་སྲིད (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_data, parimaa_na(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_data);
	གལ་སྲིད (loop == 1)
	    cl->class_class_function_count_child = gap->ga_len;
	དེ་མིན
	    cl->class_obj_method_count_child = gap->ga_len;

	གལ་སྲིད (loop == 2)
	{
	    // Copy instance methods from the parent.

	    དོན་ལུ (ཧྲིལ i = 0; i < parent_count; ++i)
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
		གལ་སྲིད (nf != NULL && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_args != NULL)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		དོན་ལུ (ཧྲིལ ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    གལ་སྲིད (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (var_na *)pname;
			where.wt_kind = WT_METHOD;
			(void)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	དོན་ལུ (ཧྲིལ i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    གལ་སྲིད (i < gap->ga_len)
		fp->uf_defclass = cl;
	    གལ་སྲིད (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    ལོག OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    static void
update_builtin_method_index(class_T *cl)
{
    ཧྲིལ	i;

    དོན་ལུ (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    དོན་ལུ (i = 0; i < cl->class_obj_method_count; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	གལ་སྲིད (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	དེ་མིན གལ་སྲིད (cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] = i;
	དེ་མིན གལ་སྲིད (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    static char_u *
find_class_name_end(char_u *arg)
{
    char_u *end = arg;

    སྐབས (ASCII_ISALNUM(*end) || *end == '_'
	    || (*end == '.' && (ASCII_ISALNUM(end[1]) || end[1] == '_')))
	++end;

    ལོག end;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    static ཧྲིལ
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
    ཧྲིལ		dup = FALSE;

    དོན་ལུ (ཧྲིལ i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_data) + i;

	གལ་སྲིད (name.length == m->ocm_name.length
	    && STRNCMP(name.string, m->ocm_name.string, name.length) == 0)
	{
	    char_u  save_c = *varname_end;

	    *varname_end = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_end = save_c;
	    dup = TRUE;
	    bha_nga;
	}
    }

    ལོག dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    static ཧྲིལ
enum_parse_values(
    exarg_T	*eap,
    class_T	*en,
    char_u	*line,
    garray_T	*gap,
    ཧྲིལ		*num_enum_values,
    ཧྲིལ		*enum_end)
{
    evalarg_T	evalarg;
    char_u	*p = line;
    var_na	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	last_char = NUL;
    ཧྲིལ		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    ཧྲིལ		did_emsg_before = did_emsg;
    སྐབས (*p != NUL)
    {
	// ignore comment
	གལ་སྲིད (*p == '#')
	    bha_nga;

	གལ་སྲིད (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_value_declaration_str), p);
	    bha_nga;
	}

	char_u *eni_name_start = p;
	char_u *eni_name_end = to_name_end(p, FALSE);

	གལ་སྲིད (is_duplicate_enum(gap, eni_name_start, eni_name_end))
	    bha_nga;

	p = skipwhite(eni_name_end);

	char_u	*init_expr = NULL;
	གལ་སྲིད (*p == '(' || *p == '<')
	{
	    གལ་སྲིད (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", line);
		bha_nga;
	    }

	    char_u *expr_start, *expr_end;

	    p = eni_name_start;
	    (void)skip_expr_concatenate(&p, &expr_start, &expr_end, &evalarg);

	    སྐབས (*expr_start && *expr_start != '(' && *expr_start != '<')
		expr_start++;

	    གལ་སྲིད (expr_end > expr_start)
		init_expr = vim_strnsave(expr_start, expr_end - expr_start);
	}

	གལ་སྲིད (init_expr == NULL)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, parimaa_na(initexpr_buf), "%s.new()", en->class_name.string);
	དེ་མིན
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, parimaa_na(initexpr_buf), "%s.new%s", en->class_name.string,
		init_expr);
	    vim_free(init_expr);
	}
	གལ་སྲིད (add_member(gap, eni_name_start, eni_name_end, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    bha_nga;

	++*num_enum_values;

	གལ་སྲིད (*p != '#')
	    last_char = *p;

	གལ་སྲིད (*p != NUL && *p != ',')
	    bha_nga;

	གལ་སྲིད (*p == ',')
	{
	    གལ་སྲིད (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", line);
		bha_nga;
	    }
	    གལ་སྲིད (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", line);
		bha_nga;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    གལ་སྲིད (*p != NUL && *p != '#')
    {
	གལ་སྲིད (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_argument_str), p);
	rc = FAIL;
    }

    གལ་སྲིད (last_char != ',')
	// last enum value should not be terminated by ","
	*enum_end = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, NULL);

    ལོག rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    static ཧྲིལ
enum_add_values_member(
    class_T	*en,
    garray_T	*gap,
    ཧྲིལ		num_enum_values,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    ཧྲིལ		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    དོན་ལུ (ཧྲིལ i = 0; i < num_enum_values; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;

	གལ་སྲིད (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.length);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    གལ་སྲིད (type == NULL)
	goto done;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    གལ་སྲིད (type->tt_member != NULL)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = add_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_data, fga.ga_len));

done:
    vim_free(fga.ga_data);

    ལོག rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    static void
enum_clear_constructors(class_T *en)
{
    དོན་ལུ (ཧྲིལ i = 0; i < en->class_class_function_count; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	གལ་སྲིད (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    void
enum_set_internal_obj_vars(class_T *en, object_T *enval)
{
    ཧྲིལ	i;

    དོན་ལུ (i = 0; i < en->class_class_member_count; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	གལ་སྲིད (en_tv != NULL && en_tv->v_type == VAR_UNKNOWN)
	    bha_nga;
    }

    གལ་སྲིད (i >= en->class_class_member_count)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	ལོག;

    // First object variable is the name
    ocmember_T *value_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(value_ocm->ocm_name.string, value_ocm->ocm_name.length);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_NUMBER;
    ord_tv->vval.v_number = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    static void
skip_class_body(exarg_T *eap)
{
    དོན་ལུ (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	གལ་སྲིད (theline == NULL)
	    bha_nga;

	char_u	*p = theline;
	ཧྲིལ	found;

	// skip ':' and blanks
	དོན་ལུ (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	found = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	གལ་སྲིད (found)
	    bha_nga;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    void
ex_class(exarg_T *eap)
{
    ཧྲིལ		is_class = eap->cmdidx == CMD_class;
    ཧྲིལ		is_abstract = eap->cmdidx == CMD_abstract;
    ཧྲིལ		is_enum = eap->cmdidx == CMD_enum;
    ཧྲིལ		added_enum_values = FALSE;
    ཧྲིལ		is_interface;
    long	start_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    གལ་སྲིད (eap->skip)
    {
	skip_class_body(eap);
	ལོག;
    }

    གལ་སྲིད (is_abstract)
    {
	གལ་སྲིད (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_argument_str), arg);
	    ལོག;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    གལ་སྲིད (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	གལ་སྲིད (is_class)
	    emsg(_(e_class_can_only_be_defined_in_vim9_script));
	དེ་མིན གལ་སྲིད (is_enum)
	    emsg(_(e_enum_can_only_be_defined_in_vim9_script));
	དེ་མིན
	    emsg(_(e_interface_can_only_be_defined_in_vim9_script));
	ལོག;
    }

    གལ་སྲིད (!ASCII_ISUPPER(*arg))
    {
	གལ་སྲིད (is_class)
	    semsg(_(e_class_name_must_start_with_uppercase_letter_str), arg);
	དེ་མིན གལ་སྲིད (is_enum)
	    semsg(_(e_enum_name_must_start_with_uppercase_letter_str), arg);
	དེ་མིན
	    semsg(_(e_interface_name_must_start_with_uppercase_letter_str),
									  arg);
	ལོག;
    }
    char_u *name_end = find_name_end(arg, NULL, NULL, FNE_CHECK_START);
    གལ་སྲིད (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	ལོག;
    }
    char_u *name_start = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = NULL;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, parimaa_na(char_u *), 5);

    arg = skipwhite(name_end);
    སྐབས (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	གལ་སྲིད (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    གལ་སྲིད (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		goto early_ret;
	    }
	    གལ་སྲིད (extends != NULL)
	    {
		emsg(_(e_duplicate_extends));
		goto early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *end = find_class_name_end(arg);
	    གལ་སྲིད (!IS_WHITE_OR_NUL(*end))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		goto early_ret;
	    }
	    extends = vim_strnsave(arg, end - arg);
	    གལ་སྲིད (extends == NULL)
		goto early_ret;

	    arg = skipwhite(end + 1);
	}
	དེ་མིན གལ་སྲིད (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    གལ་སྲིད (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		goto early_ret;
	    }

	    གལ་སྲིད (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		goto early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    དོན་ལུ (;;)
	    {
		char_u *impl_end = find_class_name_end(arg);
		གལ་སྲིད ((!IS_WHITE_OR_NUL(*impl_end) && *impl_end != ',')
			|| (*impl_end == ','
			    && !IS_WHITE_OR_NUL(*(impl_end + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    goto early_ret;
		}
		གལ་སྲིད (impl_end - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    goto early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_end - arg);
		གལ་སྲིད (iname == NULL)
		    goto early_ret;
		དོན་ལུ (ཧྲིལ i = 0; i < ga_impl.ga_len; ++i)
		    གལ་སྲིད (STRCMP(((char_u **)ga_impl.ga_data)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			goto early_ret;
		    }
		གལ་སྲིད (ga_add_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    goto early_ret;
		}
		གལ་སྲིད (*impl_end != ',')
		{
		    arg = skipwhite(impl_end);
		    bha_nga;
		}
		arg = skipwhite(impl_end + 1);
	    }
	}
	དེ་མིན
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_strings(&ga_impl);
	    ལོག;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, parimaa_na(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, parimaa_na(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, parimaa_na(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, parimaa_na(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, parimaa_na(ufunc_T *), 10);

    class_T *cl = NULL;
    class_T *extends_cl = NULL;  // class from "extends" argument
    class_T **intf_classes = NULL;
    ཧྲིལ	    intf_count = 0;	// number of items in "intf_classes"
    ཧྲིལ	    num_enum_values = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    གལ་སྲིད (cl == NULL)
	goto cleanup;

    གལ་སྲིད (is_enum)
	cl->class_flags = CLASS_ENUM;
    དེ་མིན གལ་སྲིད (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    དེ་མིན གལ་སྲིད (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.length = (size_t)(name_end - name_start);
    cl->class_name.string = vim_strnsave(name_start, cl->class_name.length);
    གལ་སྲིད (cl->class_name.string == NULL)
	goto cleanup;

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
    ཧྲིལ rc = set_var_const(cl->class_name.string, 0, NULL, &tv, FALSE, 0, 0);
    གལ་སྲིད (rc == FAIL)
	goto cleanup;

    གལ་སྲིད (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	གལ་སྲིད (add_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, NULL) == FAIL)
	    goto cleanup;

	varname = (char_u *)"ordinal";
	གལ་སྲིད (add_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_number, NULL) == FAIL)
	    goto cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    ཧྲིལ		enum_end = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = NULL;
    ཧྲིལ success = FALSE;
    དོན་ལུ (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	གལ་སྲིད (theline == NULL)
	    bha_nga;
	char_u *line = skipwhite(theline);

	// Skip empty and comment lines.
	གལ་སྲིད (*line == NUL)
	    continue;
	གལ་སྲིད (*line == '#')
	{
	    གལ་སྲིད (vim9_bad_comment(line))
		bha_nga;
	    continue;
	}

	char_u *p = line;

	var_na	*end_name;
	ཧྲིལ	shortlen;
	ཧྲིལ	fullen;
	གལ་སྲིད (is_class)
	{
	    end_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	དེ་མིན གལ་སྲིད (is_enum)
	{
	    end_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	དེ་མིན
	{
	    end_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	དོན་ལུ (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_start = p;
	གལ་སྲིད (checkforcmd(&p, end_name, shortlen))
	{
	    གལ་སྲིད (STRNCMP(cmd_start, end_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_start);
	    དེ་མིན གལ་སྲིད (*p == '|' || !ends_excmd2(line, p))
		semsg(_(e_trailing_characters_str), p);
	    དེ་མིན
		success = TRUE;
	    bha_nga;
	}

	ཧྲིལ	wrong_endname = FALSE;
	གལ་སྲིད (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	དེ་མིན གལ་སྲིད (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	དེ་མིན
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	གལ་སྲིད (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), line, end_name);
	    bha_nga;
	}

	གལ་སྲིད (is_enum && !enum_end)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    གལ་སྲིད (enum_parse_values(eap, cl, line, &classmembers,
					&num_enum_values, &enum_end) == FAIL)
		bha_nga;

	    གལ་སྲིད (enum_end)
	    {
		// Add the enum "values" class variable.
		enum_add_values_member(cl, &classmembers, num_enum_values,
							&type_list);
		added_enum_values = TRUE;
	    }
	    continue;
	}

	ཧྲིལ has_public = FALSE;
	གལ་སྲིད (checkforcmd(&p, "public", 3))
	{
	    གལ་སྲིད (STRNCMP(line, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), line);
		bha_nga;
	    }
	    གལ་སྲིད (is_interface)
	    {
		emsg(_(e_public_variable_not_supported_in_interface));
		bha_nga;
	    }
	    has_public = TRUE;
	    p = skipwhite(line + 6);

	    གལ་སྲིད (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_public_keyword_not_supported_for_method));
		bha_nga;
	    }

	    གལ་སྲིད (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_public_must_be_followed_by_var_static_final_or_const));
		bha_nga;
	    }
	}

	ཧྲིལ abstract_method = FALSE;
	char_u *pa = p;
	གལ་སྲིད (checkforcmd(&p, "abstract", 3))
	{
	    གལ་སྲིད (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		bha_nga;
	    }

	    གལ་སྲིད (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_used_in_enum));
		bha_nga;
	    }

	    གལ་སྲིད (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_used_in_interface));
		bha_nga;
	    }

	    གལ་སྲིད (!is_abstract)
	    {
		semsg(_(e_abstract_method_in_concrete_class), pa);
		bha_nga;
	    }

	    p = skipwhite(pa + 8);
	    གལ་སྲིད (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		bha_nga;
	    }

	    abstract_method = TRUE;
	}

	ཧྲིལ has_static = FALSE;
	char_u *ps = p;
	གལ་སྲིད (checkforcmd(&p, "static", 4))
	{
	    གལ་སྲིད (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		bha_nga;
	    }

	    གལ་སྲིད (is_interface)
	    {
		emsg(_(e_static_member_not_supported_in_interface));
		bha_nga;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    གལ་སྲིད (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		bha_nga;
	    }
	}

	ཧྲིལ has_final = FALSE;
	ཧྲིལ has_var = FALSE;
	ཧྲིལ has_const = FALSE;
	གལ་སྲིད (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	དེ་མིན གལ་སྲིད (checkforcmd(&p, "final", 5))
	{
	    གལ་སྲིད (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_in_interface));
		bha_nga;
	    }
	    has_final = TRUE;
	}
	དེ་མིན གལ་སྲིད (checkforcmd(&p, "const", 5))
	{
	    གལ་སྲིད (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_in_interface));
		bha_nga;
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
	གལ་སྲིད (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_end = NULL;
	    type_T *type = NULL;
	    char_u *init_expr = NULL;
	    ཧྲིལ	    has_type = FALSE;

	    གལ་སྲིད (!eval_isnamec1(*p))
	    {
		གལ་སྲིད (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), line);
		དེ་མིན
		    semsg(_(e_invalid_object_variable_declaration_str), line);
		bha_nga;
	    }

	    གལ་སྲིད (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_in_interface),
			varname);
		bha_nga;
	    }

	    གལ་སྲིད (parse_member(eap, line, varname, has_public,
			  &varname_end, &has_type, &type_list, &type,
			  !is_interface ? &init_expr: NULL) == FAIL)
		bha_nga;

	    གལ་སྲིད (is_reserved_varname(varname, varname_end)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_end))
	    {
		vim_free(init_expr);
		bha_nga;
	    }
	    གལ་སྲིད (add_member(has_static ? &classmembers : &objmembers, varname,
				varname_end, has_public, has_final, has_const,
				has_type, type, init_expr) == FAIL)
	    {
		vim_free(init_expr);
		bha_nga;
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
	དེ་མིན གལ་སྲིད (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	lines_to_free;
	    ཧྲིལ		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    གལ་སྲིད (has_public)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_public_keyword_not_supported_for_method));
		bha_nga;
	    }

	    གལ་སྲིད (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_in_class_str), line);
		bha_nga;
	    }

	    གལ་སྲིད (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_method_not_supported_in_interface), p);
		bha_nga;
	    }

	    གལ་སྲིད (has_static && !is_new && SAFE_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_method_not_supported), p);
		bha_nga;
	    }

	    CLEAR_FIELD(ea);
	    ea.cmd = line;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&lines_to_free, parimaa_na(char_u *), 50);
	    ཧྲིལ class_flags;
	    གལ་སྲིད (is_interface)
		class_flags = CF_INTERFACE;
	    དེ་མིན
		class_flags = abstract_method ? CF_ABSTRACT_METHOD : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, NULL, &lines_to_free,
			class_flags, objmembers.ga_data, objmembers.ga_len,
			NULL);
	    ga_clear_strings(&lines_to_free);

	    གལ་སྲིད (uf != NULL)
	    {
		གལ་སྲིད (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_method_not_supported_in_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    bha_nga;
		}

		// check for builtin method
		གལ་སྲིད (!is_new && SAFE_islower(*uf->uf_name) &&
					!object_check_builtin_method_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    bha_nga;
		}

		// Check the uf_name isn't used already.
		གལ་སྲིད (is_duplicate_method(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    bha_nga;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		གལ་སྲིད (ga_grow(fgap, 1) == OK)
		{
		    གལ་སྲིད (is_new)
			uf->uf_flags |= FC_NEW;

		    གལ་སྲིད (abstract_method)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_data)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	དེ་མིན
	{
	    གལ་སྲིད (is_class)
		semsg(_(e_not_valid_command_in_class_str), line);
	    དེ་མིན གལ་སྲིད (is_enum)
		semsg(_(e_not_valid_command_in_enum_str), line);
	    དེ་མིན
		semsg(_(e_not_valid_command_in_interface_str), line);
	    bha_nga;
	}
    }

    གལ་སྲིད (theline == NULL && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    གལ་སྲིད (success && is_enum && (num_enum_values == 0 || !added_enum_values))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_add_values_member(cl, &classmembers, num_enum_values,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    གལ་སྲིད (success && extends != NULL)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    གལ་སྲིད (success && extends_cl != NULL)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    གལ་སྲིད (success && extends_cl != NULL)
    {
	གལ་སྲིད (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	དེ་མིན
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    གལ་སྲིད (success && !is_abstract && extends_cl != NULL
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, parimaa_na(class_T *), 5);

    གལ་སྲིད (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    གལ་སྲིད (success && extends_cl != NULL)
	success = add_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_data;
    intf_count = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    གལ་སྲིད (success)
	success = check_func_arg_names(&classfunctions, &objmethods,
							&classmembers);

    གལ་སྲིད (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	གལ་སྲིད (extends_cl != NULL)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	གལ་སྲིད (add_members_to_class(&classmembers,
				 NULL,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_count) == FAIL
		|| add_members_to_class(&objmembers,
				 extends_cl == NULL ? NULL
					       : extends_cl->class_obj_members,
				 extends_cl == NULL ? 0
					  : extends_cl->class_obj_member_count,
				 &cl->class_obj_members,
				 &cl->class_obj_member_count) == FAIL)
	    goto cleanup;

	གལ་སྲིད (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_count = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    གལ་སྲིད (cl->class_interfaces == NULL)
		goto cleanup;
	    དོན་ལུ (ཧྲིལ i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_data)[i];
	    VIM_CLEAR(ga_impl.ga_data);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = NULL;
	}

	གལ་སྲིད (cl->class_interface_count > 0 || extends_cl != NULL)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    གལ་སྲིད (add_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		goto cleanup;
	}

	ཧྲིལ	have_new = FALSE;
	ufunc_T	*class_func = NULL;
	དོན་ལུ (ཧྲིལ i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_data)[i];
	    གལ་སྲིད (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		bha_nga;
	    }
	}

	གལ་སྲིད (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	དེ་མིན གལ་སྲིད ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    add_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	གལ་སྲིད (add_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    goto cleanup;

	update_builtin_method_index(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	གལ་སྲིད ((is_class || is_enum) && cl->class_class_member_count > 0)
	    གལ་སྲིད (add_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		ལོག;
	    }

	cl->class_type_list = type_list;

	གལ་སྲིད (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	ལོག;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    གལ་སྲིད (intf_classes != NULL)
    {
	དོན་ལུ (ཧྲིལ i = 0; i < intf_count; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_strings(&ga_impl);

    དོན་ལུ (ཧྲིལ round = 1; round <= 2; ++round)
    {
	garray_T *gap = round == 1 ? &classmembers : &objmembers;
	གལ་སྲིད (gap->ga_len == 0 || gap->ga_data == NULL)
	    continue;

	དོན་ལུ (ཧྲིལ i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    དོན་ལུ (ཧྲིལ i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    དོན་ལུ (ཧྲིལ i = 0; i < classfunctions.ga_len; ++i)
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
    ཧྲིལ		is_object,
    char_u	*name,
    char_u	*name_end,
    ཧྲིལ		*member_idx)
{
    size_t	len = name_end - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    གལ་སྲིད (m == NULL)
    {
	member_not_found_msg(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	ལོག &t_any;
    }

    ལོག m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    ཧྲིལ		is_object,
    ཧྲིལ		member_idx)
{
    ocmember_T	*m;
    ཧྲིལ		member_count;

    གལ་སྲིད (is_object)
    {
	m = cl->class_obj_members;
	member_count = cl->class_obj_member_count;
    }
    དེ་མིན
    {
	m = cl->class_class_members;
	member_count = cl->class_class_member_count;
    }

    གལ་སྲིད (member_idx >= member_count)
	ལོག NULL;

    ལོག m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    static void
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    void
typealias_unref(typealias_T *ta)
{
    གལ་སྲིད (ta != NULL && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    void
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    གལ་སྲིད (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_in_vim9_script));
	ལོག;
    }

    གལ་སྲིད (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	ལོག;
    }

    གལ་སྲིད (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_start_with_uppercase_letter_str), arg);
	ལོག;
    }

    char_u *name_end = find_name_end(arg, NULL, NULL, FNE_CHECK_START);
    གལ་སྲིད (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	ལོག;
    }
    char_u *name_start = arg;

    arg = skipwhite(name_end);
    གལ་སྲིད (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	ལོག;
    }
    གལ་སྲིད (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	ལོག;
    }
    arg++;
    arg = skipwhite(arg);

    གལ་སྲིད (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	ལོག;
    }

    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, NULL, NULL, TRUE);
    གལ་སྲིད (type == NULL)
	ལོག;

    གལ་སྲིད (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	ལོག;
    }

    ཧྲིལ cc = *name_end;
    *name_end = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    གལ་སྲིད (eval_variable_import(name_start, &tv) == OK)
    {
	གལ་སྲིད (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_start);
	དེ་མིན
	    semsg(_(e_redefining_script_item_str), name_start);
	clear_tv(&tv);
	goto done;
    }

    // Create a script-local variable for the type alias.
    གལ་སྲིད (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_lock = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_start);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_depth = parse_type_depth();
    }
    དེ་མིན
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_lock = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    set_var_const(name_start, current_sctx.sc_sid, NULL, &tv, FALSE,
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
    ཧྲིལ
get_member_tv(
    class_T	*cl,
    ཧྲིལ		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    ཧྲིལ		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    གལ་སྲིད (m == NULL)
	ལོག FAIL;

    གལ་སྲིད (*name == '_' && (current_class == NULL ||
				!class_instance_of(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	ལོག FAIL;
    }

    གལ་སྲིད (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	set_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    དེ་མིན
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	set_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    ལོག OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    static ཧྲིལ
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
    ཧྲིལ		argcount = 0;
    ocmember_T	*ocm = NULL;
    ཧྲིལ		m_idx;

    fp = method_lookup(cl, rettv->v_type, name, len, NULL);
    གལ་སྲིད (fp == NULL)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	གལ་སྲིད (ocm == NULL || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    method_not_found_msg(cl, rettv->v_type, name, len);
	    ལོག FAIL;
	}

	གལ་སྲིད (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    ལོག FAIL;
	}

	གལ་སྲིད (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	དེ་མིན
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_end;
	ལོག OK;
    }

    གལ་སྲིད (ocm == NULL && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
	ལོག FAIL;
    }

    // process generic function call
    གལ་སྲིད (fp != NULL)
    {
	fp = generic_func_get(fp, gfatab);
	གལ་སྲིད (fp == NULL)
	    ལོག FAIL;
    }

    char_u *argp = name_end;
    ཧྲིལ ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    གལ་སྲིད (ret == FAIL)
    {
	སྐབས (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ལོག FAIL;
    }

    funcexe_T funcexe;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    གལ་སྲིད (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    གལ་སྲིད (evalarg != NULL)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    ཧྲིལ error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								NULL);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    དོན་ལུ (ཧྲིལ idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    གལ་སྲིད (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_found_var);
	ལོག FAIL;
    }
    *arg = argp;

    ལོག OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ཧྲིལ
obj_method_to_partial_tv(object_T *obj, ufunc_T *obj_method, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    གལ་སྲིད (pt == NULL)
	ལོག FAIL;

    pt->pt_refcount = 1;
    གལ་སྲིད (obj != NULL)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_method;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    ལོག OK;
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
    ཧྲིལ
class_object_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ཧྲིལ		verbose UNUSED)	// give error messages
{
    ཧྲིལ	ret = FAIL;

    གལ་སྲིད (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ལོག FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, NULL, NULL, FNE_CHECK_START);
    གལ་སྲིད (name_end == name)
	ལོག FAIL;
    size_t len = name_end - name;

    ཧྲིལ did_emsg_save = did_emsg;
    class_T *cl;
    གལ་སྲིད (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    དེ་མིན // VAR_OBJECT
    {
	གལ་སྲིད (rettv->vval.v_object == NULL)
	{
	    emsg(_(e_using_null_object));
	    ལོག FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    གལ་སྲིད (cl == NULL)
    {
	emsg(_(e_incomplete_type));
	ལོག FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    གལ་སྲིད (*name_end == '<')
    {
	cctx_T	    *cctx = NULL;

	གལ་སྲིད (evalarg != NULL)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_end = parse_generic_func_type_args(name, len, name + len,
						&gfatab, cctx);
	གལ་སྲིད (name_end == NULL)
	    goto done;
    }

    གལ་སྲིད (*name_end == '(')
	// Invoke the class or object method
	ret = call_oc_method(cl, name, len, name_end, evalarg, arg,
							&gfatab, rettv);
    དེ་མིན གལ་སྲིད (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	ཧྲིལ is_object = rettv->v_type == VAR_OBJECT;
	གལ་སྲིད (get_member_tv(cl, is_object, name, len, NULL, rettv) == OK)
	{
	    *arg = name_end;
	    ret = OK;
	    goto done;
	}

	// could be a class method or an object method
	ཧྲིལ	fidx;
	ufunc_T	*fp = method_lookup(cl, rettv->v_type, name, len, &fidx);
	གལ་སྲིད (fp != NULL)
	{
	    // Protected methods are not accessible outside the class
	    གལ་སྲིད (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		goto done;
	    }

	    གལ་སྲིད (obj_method_to_partial_tv(is_object ? rettv->vval.v_object :
						NULL, fp, rettv) == FAIL)
		goto done;

	    *arg = name_end;
	    ret = OK;
	    goto done;
	}

	གལ་སྲིད (did_emsg == did_emsg_save)
	    member_not_found_msg(cl, rettv->v_type, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);

    ལོག ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
find_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, NULL, NULL, FNE_CHECK_START);
    གལ་སྲིད (name_end == name || *name_end != '.')
	ལོག NULL;

    ufunc_T	*fp = NULL;
    size_t	len = name_end - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    གལ་སྲིད (eval_variable(name, (ཧྲིལ)len,
				    0, &tv, NULL, EVAL_VAR_NOAUTOLOAD) == FAIL)
	ལོག NULL;
    གལ་སྲིད (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	goto fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    གལ་སྲིད (cl == NULL)
	goto fail_after_eval;
    char_u *fname = name_end + 1;
    char_u *fname_end = find_name_end(fname, NULL, NULL, FNE_CHECK_START);
    གལ་སྲིད (fname_end == fname)
	goto fail_after_eval;
    len = fname_end - fname;

    fp = method_lookup(cl, tv.v_type, fname, len, NULL);

    གལ་སྲིད (fp != NULL)
    {
	fp = eval_generic_func(fp, fname, &fname_end);
	གལ་སྲིད (fp != NULL)
	    *arg = fname_end;
    }

fail_after_eval:
    clear_tv(&tv);
    ལོག fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ཧྲིལ
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    ཧྲིལ idx;
    class_member_lookup(cl, name, namelen, &idx);
    ལོག idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, ཧྲིལ *idx)
{
    ocmember_T	*ret_m = NULL;
    ཧྲིལ		ret_idx = -1;
    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_class_member_count; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	གལ་སྲིད (namelen)
	{
	    གལ་སྲིད (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		bha_nga;
	    }
	}
	དེ་མིན གལ་སྲིད (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    bha_nga;
	}
    }
    གལ་སྲིད (idx != NULL)
	*idx = ret_idx;
    ལོག ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    static ufunc_T *
class_method_lookup(class_T *cl, char_u *name, size_t namelen, ཧྲིལ *idx)
{
    ufunc_T	*ret_fp = NULL;
    ཧྲིལ		ret_idx = -1;
    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_class_function_count; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	གལ་སྲིད (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    bha_nga;
	}
    }
    གལ་སྲིད (idx != NULL)
	*idx = ret_idx;
    ལོག ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    ཧྲིལ
class_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    ཧྲིལ idx;
    class_method_lookup(cl, name, namelen, &idx);
    ལོག idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    static ཧྲིལ
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    ཧྲིལ idx;
    object_member_lookup(cl, name, namelen, &idx);
    ལོག idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, ཧྲིལ *idx)
{
    ocmember_T	*ret_m = NULL;
    ཧྲིལ		ret_idx = -1;
    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_obj_member_count; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	གལ་སྲིད (namelen)
	{
	    གལ་སྲིད (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		bha_nga;
	    }
	}
	དེ་མིན གལ་སྲིད (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    bha_nga;
	}
    }
    གལ་སྲིད (idx != NULL)
	*idx = ret_idx;
    ལོག ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    static ufunc_T *
object_method_lookup(class_T *cl, char_u *name, size_t namelen, ཧྲིལ *idx)
{
    ufunc_T	*ret_fp = NULL;
    ཧྲིལ		ret_idx = -1;
    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_obj_method_count; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	གལ་སྲིད (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    bha_nga;
	}
    }
    གལ་སྲིད (idx != NULL)
	*idx = ret_idx;
    ལོག ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    ཧྲིལ
object_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    ཧྲིལ idx;
    object_method_lookup(cl, name, namelen, &idx);
    ལོག idx;
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
    ཧྲིལ		*idx)
{
    གལ་སྲིད (v_type == VAR_CLASS)
	ལོག class_member_lookup(cl, name, namelen, idx);

    ལོག object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    static class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_found = NULL;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_found = NULL;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    དོན་ལུ (class_T *super = cl; super != NULL; super = super->class_extends)
    {
	class_T		*cl_tmp = NULL;
	ocmember_T	*m = NULL;
	གལ་སྲིད (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    གལ་སྲིད ((m = object_member_lookup(super, name, len, NULL)) != NULL)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	གལ་སྲིད (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    གལ་སྲིད (( m = class_member_lookup(super, name, len, NULL)) != NULL)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	གལ་སྲིད (cl_tmp == NULL)
	    bha_nga;  // member is not in this or any super class.
	cl_found = cl_tmp;
	m_found = m;
    }
    གལ་སྲིད (p_m != NULL)
	*p_m = m_found;
    ལོག cl_found;
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
    ཧྲིལ		*idx)
{
    གལ་སྲིད (v_type == VAR_CLASS)
	ལོག class_method_lookup(cl, name, namelen, idx);
    དེ་མིན
	ལོག object_method_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    ཧྲིལ
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    དོན་ལུ (cctx_T *cctx = cctx_arg; cctx != NULL; cctx = cctx->ctx_outer)
	གལ་སྲིད (cctx->ctx_ufunc != NULL
			&& class_instance_of(cctx->ctx_ufunc->uf_class, cl))
	    ལོག TRUE;
    ལོག FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    ཧྲིལ
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    གལ་སྲིད (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_in_class_str),
		m->ocm_name.string, cl->class_name.string);
	ལོག TRUE;
    }
    ལོག FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    void
obj_lock_const_vars(object_T *obj)
{
    དོན་ལུ (ཧྲིལ i = 0; i < obj->obj_class->class_obj_member_count; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	གལ་སྲིད (ocm->ocm_flags & OCMFLAG_CONST)
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
    ཧྲིལ		sz;

    གལ་སྲིད (cl == NULL)
	ལོག NULL;

    sz = parimaa_na(object_T) + cl->class_obj_member_count * parimaa_na(typval_T);
    obj = alloc_clear(sz);
    གལ་སྲིད (obj == NULL)
	ལོག NULL;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    གལ་སྲིད (IS_ENUM(cl))
	enum_set_internal_obj_vars(cl, obj);

    ལོག obj;
}

/*
 * Make a copy of an object.
 */
    void
copy_object(typval_T *from, typval_T *to)
{
    གལ་སྲིད (from->vval.v_object == NULL)
	to->vval.v_object = NULL;
    དེ་མིན
    {
	to->vval.v_object = from->vval.v_object;
	++to->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    void
copy_class(typval_T *from, typval_T *to)
{
    གལ་སྲིད (from->vval.v_class == NULL)
	to->vval.v_class = NULL;
    དེ་མིན
    {
	to->vval.v_class = from->vval.v_class;
	++to->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    static void
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_interface_count; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	གལ་སྲིད (cl->class_interfaces_cl[i] != NULL)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *next;
    དོན་ལུ (itf2class_T *i2c = cl->class_itf2class; i2c != NULL; i2c = next)
    {
	next = i2c->i2c_next;
	vim_free(i2c);
    }

    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_class_member_count; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	གལ་སྲིད (cl->class_members_tv != NULL)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_obj_member_count; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_class_function_count; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	གལ་སྲིད (IS_ENUM(cl) && IS_CONSTRUCTOR_METHOD(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_obj_method_count; ++i)
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
    static ཧྲིལ
class_get_selfrefs(class_T *cl)
{
    ཧྲིལ		self_refs = 0;
    typval_T	*tv;

    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_class_member_count; ++i)
    {
	tv = &cl->class_members_tv[i];
	གལ་སྲིད (tv->v_type == VAR_OBJECT && tv->vval.v_object != NULL
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    ལོག self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    static ཧྲིལ
can_free_enum(class_T *cl)
{
    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_class_member_count; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	གལ་སྲིད (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    ལོག TRUE;

	གལ་སྲིད (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    གལ་སྲིད (tv->vval.v_list->lv_refcount > 1)
		ལོག FALSE;
	    bha_nga;
	}

	གལ་སྲིད (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    ལོག FALSE;
    }

    ལོག TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    static ཧྲིལ
can_free_class(class_T *cl)
{
    ཧྲིལ		can_free = FALSE;
    ཧྲིལ		self_refs = 0;

    གལ་སྲིད (IS_ENUM(cl) && !can_free_enum(cl))
	ལོག FALSE;

    གལ་སྲིད (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    གལ་སྲིད (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    ལོག can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    void
class_unref(class_T *cl)
{
    གལ་སྲིད (cl == NULL)
	ལོག;

    --cl->class_refcount;

    གལ་སྲིད (cl->class_name.string == NULL)
	ལོག;

    གལ་སྲིད (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    ཧྲིལ
class_free_nonref(ཧྲིལ copyID)
{
    ཧྲིལ		did_free = FALSE;

    དོན་ལུ (class_T *cl = first_class; cl != NULL; cl = next_nonref_class)
    {
	next_nonref_class = cl->class_next_used;
	གལ་སྲིད ((cl->class_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    next_nonref_class = NULL;
    ལོག did_free;
}

    ཧྲིལ
set_ref_in_classes(ཧྲིལ copyID)
{
    དོན་ལུ (class_T *cl = first_class; cl != NULL; cl = cl->class_next_used)
	set_ref_in_item_class(cl, copyID, NULL, NULL, NULL);

    ལོག FALSE;
}

static object_T *first_object = NULL;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    void
object_created(object_T *obj)
{
    གལ་སྲིད (first_object != NULL)
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
    static void
object_cleared(object_T *obj)
{
    གལ་སྲིད (obj->obj_next_used != NULL)
	obj->obj_next_used->obj_prev_used = obj->obj_prev_used;
    གལ་སྲིད (obj->obj_prev_used != NULL)
	obj->obj_prev_used->obj_next_used = obj->obj_next_used;
    དེ་མིན གལ་སྲིད (first_object == obj)
	first_object = obj->obj_next_used;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    static void
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    གལ་སྲིད (!cl)
	ལོག;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = INT_MAX;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    དོན་ལུ (ཧྲིལ i = 0; i < cl->class_obj_member_count; ++i)
	clear_tv(tv + i);
}

    static void
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    གལ་སྲིད (!cl)
	ལོག;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    static void
object_free(object_T *obj)
{
    གལ་སྲིད (in_free_unref_items)
	ལོག;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    void
object_unref(object_T *obj)
{
    གལ་སྲིད (obj != NULL && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    ཧྲིལ
object_free_nonref(ཧྲིལ copyID)
{
    ཧྲིལ		did_free = FALSE;

    དོན་ལུ (object_T *obj = first_object; obj != NULL; obj = obj->obj_next_used)
    {
	གལ་སྲིད ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    ལོག did_free;
}

    void
object_free_items(ཧྲིལ copyID)
{
    object_T	*obj_next;

    དོན་ལུ (object_T *obj = first_object; obj != NULL; obj = obj_next)
    {
	obj_next = obj->obj_next_used;
	གལ་སྲིད ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    void
emsg_var_cl_define(var_na *msg, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    གལ་སྲིད (cl_def != NULL)
	semsg(_(msg), m->ocm_name.string, cl_def->class_name.string);
    དེ་མིན
	emsg(_(e_internal_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    void
method_not_found_msg(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *method_name = vim_strnsave(name, len);
    གལ་སྲིད ((v_type == VAR_OBJECT)
	    && (class_method_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	གལ་སྲིད (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	དེ་མིན
	    semsg(_(e_class_method_str_accessible_only_using_class_str),
		    method_name, cl->class_name.string);
    }
    དེ་མིན གལ་སྲིད ((v_type == VAR_CLASS)
	    && (object_method_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	གལ་སྲིད (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	དེ་མིན
	    semsg(_(e_object_method_str_accessible_only_using_object_str),
		    method_name, cl->class_name.string);
    }
    དེ་མིན
	semsg(_(e_method_not_found_on_class_str_str), method_name,
		cl->class_name.string);
    vim_free(method_name);
}

/*
 * Echo a class or object member not found message.
 */
    void
member_not_found_msg(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    གལ་སྲིད (v_type == VAR_OBJECT)
    {
	གལ་སྲིད (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	དེ་མིན
	    semsg(_(e_variable_not_found_on_object_str_str), varname,
		    cl->class_name.string);
    }
    དེ་མིན
    {
	གལ་སྲིད (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	དེ་མིན
	{
	    གལ་སྲིད (IS_ENUM(cl))
		semsg(_(e_enum_value_str_not_found_in_enum_str),
			varname, cl->class_name.string);
	    དེ་མིན
		semsg(_(e_class_variable_str_not_found_in_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    void
defcompile_class(class_T *cl)
{
    དོན་ལུ (ཧྲིལ loop = 1; loop <= 2; ++loop)
    {
	ཧྲིལ func_count = loop == 1 ? cl->class_class_function_count
						: cl->class_obj_method_count;
	དོན་ལུ (ཧྲིལ i = 0; i < func_count; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    གལ་སྲིད (!IS_ABSTRACT_METHOD(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    void
defcompile_classes_in_script(void)
{
    དོན་ལུ (class_T *cl = first_class; cl != NULL; cl = cl->class_next_used)
    {
	གལ་སྲིད (eval_variable(cl->class_name.string, 0, 0, NULL, NULL,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    ཧྲིལ
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    གལ་སྲིད (eval_variable(name, 0, 0, rettv, NULL, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	ལོག rettv->v_type == VAR_CLASS;
    ལོག FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    static ཧྲིལ
object_call_builtin_method(
    object_T		*obj,
    class_builtin_T	builtin_method,
    ཧྲིལ			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    ཧྲིལ	    midx;

    གལ་སྲིད (obj == NULL)
	ལོག FAIL;

    uf = class_get_builtin_method(obj->obj_class, builtin_method, &midx);
    གལ་སྲིད (uf == NULL)
	ལོག FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    ཧྲིལ		r;

    གལ་སྲིད (fc == NULL)
	ལོག FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, NULL, obj, fc, rettv);

    remove_funccal();

    ལོག r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    ཧྲིལ
object_empty(object_T *obj)
{
    typval_T	rettv;

    གལ་སྲིད (object_call_builtin_method(obj, CLASS_BUILTIN_EMPTY, 0, NULL, &rettv)
								== FAIL)
	ལོག TRUE;

    ལོག tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    ཧྲིལ
object_len(object_T *obj)
{
    typval_T	rettv;

    གལ་སྲིད (object_call_builtin_method(obj, CLASS_BUILTIN_LEN, 0, NULL, &rettv)
								== FAIL)
	ལོག 0;

    ལོག tv_to_number(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    ཧྲིལ
object_equal(
	object_T *o1,
	object_T *o2,
	ཧྲིལ	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    གལ་སྲིད (o1 == o2)
	ལོག TRUE;
    གལ་སྲིད (o1 == NULL || o2 == NULL)
	ལོག FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    གལ་སྲིད (cl1 != cl2 || cl1 == NULL || cl2 == NULL)
	ལོག FALSE;

    དོན་ལུ (ཧྲིལ i = 0; i < cl1->class_obj_member_count; ++i)
	གལ་སྲིད (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    ལོག FALSE;

    ལོག TRUE;
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
    ཧྲིལ		copyID,
    ཧྲིལ		echo_style,
    ཧྲིལ		restore_copyID,
    ཧྲིལ		composite_val)
{
    typval_T	rettv;

    གལ་སྲིད (object_call_builtin_method(obj, CLASS_BUILTIN_STRING, 0, NULL, &rettv)
								== OK
					&& rettv.vval.v_string != NULL)
	ལོག rettv.vval.v_string;

    ཧྲིལ		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    གལ་སྲིད (cl != NULL && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    དེ་མིན
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	གལ་སྲིད (cl == NULL)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	དེ་མིན
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
    }
    གལ་སྲིད (cl != NULL)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	དོན་ལུ (ཧྲིལ i = 0; i < cl->class_obj_member_count; ++i)
	{
	    གལ་སྲིད (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.length);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = NULL;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, composite_val);
	    གལ་སྲིད (s != NULL)
		ga_concat(&ga, s);
	    vim_free(tf);
	    གལ་སྲིད (s == NULL || did_echo_string_emsg)
	    {
		ok = FAIL;
		bha_nga;
	    }
	    line_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    གལ་སྲིད (ok == FAIL)
    {
	vim_free(ga.ga_data);
	ལོག NULL;
    }
    ga_append(&ga, NUL);
    ལོག (char_u *)ga.ga_data;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    ཧྲིལ
class_instance_of(class_T *cl, class_T *other_cl)
{
    གལ་སྲིད (cl == other_cl)
	ལོག TRUE;

    // Recursively check the base classes.
    དོན་ལུ (; cl != NULL; cl = cl->class_extends)
    {
	གལ་སྲིད (cl == other_cl)
	    ལོག TRUE;
	// Check the implemented interfaces and the super interfaces
	དོན་ལུ (ཧྲིལ i = cl->class_interface_count - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    སྐབས (intf != NULL)
	    {
		གལ་སྲིད (intf == other_cl)
		    ལོག TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    ལོག FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    void
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_number = VVAL_FALSE;

    གལ་སྲིད (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_args(argvars, 1) == FAIL)
	ལོག;

    གལ་སྲིད (object_tv->vval.v_object == NULL)
    {
	གལ་སྲིད (classinfo_tv->vval.v_class == NULL)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_number = VVAL_TRUE;
	ལོག;
    }

    དོན་ལུ (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	གལ་སྲིད (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	དེ་མིན
	    c = classinfo_tv->vval.v_class;

	གལ་སྲིད (class_instance_of(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_number = VVAL_TRUE;
	    ལོག;
	}
    }
}

#endif // FEAT_EVAL
