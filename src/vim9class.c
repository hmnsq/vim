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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

ସ୍ଥିର class_T *first_class = ଖାଲି;
ସ୍ଥିର class_T *next_nonref_class = ଖାଲି;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
class_created(class_T *cl)
{
    ଯଦି (first_class != ଖାଲି)
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
    ସ୍ଥିର ଶୂନ୍ୟ
class_cleared(class_T *cl)
{
    ଯଦି (cl->class_next_used != ଖାଲି)
	cl->class_next_used->class_prev_used = cl->class_prev_used;
    ଯଦି (cl->class_prev_used != ଖାଲି)
	cl->class_prev_used->class_next_used = cl->class_next_used;
    ଅନ୍ୟଥା ଯଦି (first_class == cl)
	first_class = cl->class_next_used;

    // update the next class to check if needed
    ଯଦି (cl == next_nonref_class)
	next_nonref_class = cl->class_next_used;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_member(
    exarg_T	*eap,
    char_u	*ଧାଡ଼ି,
    char_u	*varname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_public,	    // TRUE if "public" seen before "varname"
    char_u	**varname_end,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_ପଦସମଷ୍ଟି)
{
    *varname_end = to_name_end(varname, FALSE);
    ଯଦି (*varname == '_' && has_public)
    {
	semsg(_(e_public_variable_name_cannot_start_with_underscore_str), ଧାଡ଼ି);
	ଫେରନ୍ତୁ FAIL;
    }

    char_u *colon = skipwhite(*varname_end);
    char_u *type_arg = colon;
    type_T *type = ଖାଲି;
    *has_type = FALSE;
    ଯଦି (*colon == ':')
    {
	ଯଦି (VIM_ISWHITE(**varname_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    ଫେରନ୍ତୁ FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, ଖାଲି, ଖାଲି, TRUE);
	ଯଦି (type == ଖାଲି || !valid_declaration_type(type))
	    ଫେରନ୍ତୁ FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    ଯଦି (type == ଖାଲି && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (init_ପଦସମଷ୍ଟି == ଖାଲି && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_in_interface));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *ପଦସମଷ୍ଟି_start, *ପଦସମଷ୍ଟି_end;

	ଯଦି (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    ଫେରନ୍ତୁ FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(ଶୂନ୍ୟ)skip_ପଦସମଷ୍ଟି_concatenate(&init_arg, &ପଦସମଷ୍ଟି_start, &ପଦସମଷ୍ଟି_end, &evalarg);

	init_arg = skipwhite(init_arg);
	ଯଦି (*init_arg != NUL && !vim9_comment_start(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, ଖାଲି);
	    ଫେରନ୍ତୁ FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	ଯଦି (type == ଖାଲି)
	    type = &t_any;

	*init_ପଦସମଷ୍ଟି = vim_strnsave(ପଦସମଷ୍ଟି_start, ପଦସମଷ୍ଟି_end - ପଦସମଷ୍ଟି_start);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, ଖାଲି);
    }
    ଅନ୍ୟଥା ଯଦି (!valid_declaration_type(type))
	ଫେରନ୍ତୁ FAIL;

    *type_ret = type;
    ଫେରନ୍ତୁ OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_end,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_public,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_final,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_const,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_type,
    type_T	*type,
    char_u	*init_ପଦସମଷ୍ଟି)
{
    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_data) + gap->ga_len;
    m->ocm_name.length = (size_t)(varname_end - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.length);
    ଯଦି (m->ocm_name.string == ଖାଲି)
    {
	m->ocm_name.length = 0;
	ଫେରନ୍ତୁ FAIL;
    }
    m->ocm_access = has_public ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_PRIVATE : VIM_ACCESS_READ;
    ଯଦି (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    ଯଦି (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    ଯଦି (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    ଯଦି (init_ପଦସମଷ୍ଟି != ଖାଲି)
    {
	m->ocm_init = init_ପଦସମଷ୍ଟି;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_members_to_class(
    garray_T	*gap,
    ocmember_T	*parent_members,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		parent_ଗଣନା,
    ocmember_T	**members,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*member_ଗଣନା)
{
    *member_ଗଣନା = parent_ଗଣନା + gap->ga_len;
    *members = *member_ଗଣନା == 0 ? ଖାଲି
				       : ALLOC_MULT(ocmember_T, *member_ଗଣନା);
    ଯଦି (*member_ଗଣନା > 0 && *members == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < parent_ଗଣନା; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = parent_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.length);
	ଯଦି (m->ocm_name.string == ଖାଲି)
	{
	    m->ocm_name.length = 0;
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (m->ocm_init != ଖାଲି)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    ଯଦି (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + parent_ଗଣନା,
			       gap->ga_data, ଆକାର(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_data);
    ଫେରନ୍ତୁ OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
object_index_from_itf_index(class_T *itf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_method, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, class_T *cl)
{
    ଯଦି (idx >= (is_method ? itf->class_obj_method_ଗଣନା
				   : itf->class_obj_member_ଗଣନା))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	ଫେରନ୍ତୁ 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    ଯଦି (cl == itf)
	ଫେରନ୍ତୁ idx;

    itf2class_T		*i2c = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			searching = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			method_offset = 0;

    ପାଇଁ (class_T *super = cl; super != ଖାଲି && searching;
						super = super->class_extends)
    {
	ପାଇଁ (i2c = itf->class_itf2class; i2c != ଖାଲି; i2c = i2c->i2c_next)
	{
	    ଯଦି (i2c->i2c_class == super && i2c->i2c_is_method == is_method)
	    {
		searching = FALSE;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (searching && is_method)
	    // The parent class methods are stored after the current class
	    // methods.
	    method_offset += super->class_obj_method_ଗଣନା_child;
    }
    ଯଦି (i2c == ଖାଲି)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	ଫେରନ୍ତୁ 0;
    }

    // A table follows the i2c for the class
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *table = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    ଫେରନ୍ତୁ table[idx] + method_offset;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_class)
{
    typval_T	tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		success = FALSE;

    ଯଦି (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	ଫେରନ୍ତୁ success;
    }

    tv.v_type = VAR_UNKNOWN;
    ଯଦି (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_found_str), extends_name);
	ଫେରନ୍ତୁ success;
    }

    ଯଦି (tv.v_type != VAR_CLASS || tv.vval.v_class == ଖାଲି
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    ଅନ୍ୟଥା
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    ଫେରନ୍ତୁ success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
validate_extends_generic_method(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    ଅକ୍ଷର	*ସନ୍ଦେଶ = ଖାଲି;

    ଯଦି (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (super_fp->uf_generic_argcount == 0)
	ସନ୍ଦେଶ = e_concrete_method_str_override_with_generic_method_in_class_str;
    ଅନ୍ୟଥା ଯଦି (cl_fp->uf_generic_argcount == 0)
	ସନ୍ଦେଶ = e_generic_method_str_override_with_concrete_method_in_class_str;
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ = e_generic_method_str_type_arguments_mismatch_in_class_str;

    semsg(_(ସନ୍ଦେଶ), cl_fp->uf_name, super_cl->class_name.string);
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		method_ଗଣନା = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_data);

    ଯେତେବେଳେ (super != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା extends_method_ଗଣନା = super->class_obj_method_ଗଣନା_child;
	ଯଦି (extends_method_ଗଣନା == 0)
	{
	    super = super->class_extends;
	    ଜାରି;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < extends_method_ଗଣନା; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    extends_private = (*pstr == '_');
	    ଯଦି (extends_private)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < method_ଗଣନା; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	priv_method = (*qstr == '_');
		ଯଦି (priv_method)
		    qstr++;
		ଯଦି (STRCMP(pstr, qstr) == 0)
		{
		    ଯଦି (priv_method != extends_private)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_method_str_of_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			ଫେରନ୍ତୁ FALSE;
		    }

		    ଯଦି (!validate_extends_generic_method(super,
						extends_methods[i], cl_fp[j]))
			    ଫେରନ୍ତୁ FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା member_ଗଣନା = objmembers_gap->ga_len;
    ଯଦି (member_ଗଣନା == 0)
	ଫେରନ୍ତୁ TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା c_i = 0; c_i < member_ଗଣନା; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	ଯେତେବେଳେ (p_cl != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା p_member_ଗଣନା = p_cl->class_obj_member_ଗଣନା;

	    ଯଦି (p_member_ଗଣନା == 0)
	    {
		p_cl = p_cl->class_extends;
		ଜାରି;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା p_i = 0; p_i < p_member_ଗଣନା; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		ଯଦି (STRCMP(pstr, qstr) == 0)
		{
		    ଯଦି (c_protected != p_protected)
			semsg(_(e_public_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    ଅନ୍ୟଥା
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    ଫେରନ୍ତୁ FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା member_ଗଣନା = objmembers_gap->ga_len;
    ଯଦି (member_ଗଣନା == 0)
	ଫେରନ୍ତୁ TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_data);

    // Validate each member variable
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା c_i = 0; c_i < member_ଗଣନା; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    var_found = FALSE;

	// Check in all the parent classes in the lineage
	ଯେତେବେଳେ (p_cl != ଖାଲି && !var_found)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା p_member_ଗଣନା = p_cl->class_obj_member_ଗଣନା;
	    ଯଦି (p_member_ଗଣନା == 0)
	    {
		p_cl = p_cl->class_extends;
		ଜାରି;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା p_i = 0; p_i < p_member_ଗଣନା; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		ଯଦି (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    ଜାରି;

		// Ensure the type is matching.
		where.wt_func_name = (ଅକ୍ଷର *)c_m->ocm_name.string;
		where.wt_kind = WT_MEMBER;

		ଯଦି (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    ଫେରନ୍ତୁ FALSE;

		var_found = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା extends_method_ଗଣନା = loop == 1
				? extends_cl->class_class_function_ଗଣନା
				: extends_cl->class_obj_method_ଗଣନା;
	ଯଦି (extends_method_ଗଣନା == 0)
	    ଜାରି;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା method_ଗଣନା = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_data
						: objmethods_gap->ga_data);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < extends_method_ଗଣନା; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    ଯଦି (!IS_ABSTRACT_METHOD(uf))
		ଜାରି;

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	concrete_method_found = FALSE;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    ପାଇଁ (j = 0; !concrete_method_found && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		ଯଦି (!IS_ABSTRACT_METHOD(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_method_found = TRUE;
	    }

	    ଯଦି (concrete_method_found)
		ଜାରି;

	    ପାଇଁ (j = 0; j < method_ଗଣନା; j++)
	    {
		ଯଦି (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_method_found = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }

	    ଯଦି (!concrete_method_found)
	    {
		semsg(_(e_abstract_method_str_not_implemented), uf->uf_name);
		ଫେରନ୍ତୁ FALSE;
	    }
	}
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cl_member_ଗଣନା,
    class_T	*extends_cl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		variable_present  = FALSE;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା cl_i = 0; cl_i < cl_member_ଗଣନା; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	ଯଦି (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    ଜାରି;

	// Ensure the access type is same
	ଯଦି (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_of_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    ଫେରନ୍ତୁ FALSE;
	}

	// Ensure the type is matching.
	ଯଦି (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	ଅନ୍ୟଥା
	{
	    where.wt_func_name = (ଅକ୍ଷର *)m->ocm_name.string;
	    where.wt_kind = WT_MEMBER;
	    ଯଦି (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		ଫେରନ୍ତୁ FALSE;
	}

	variable_present = TRUE;
	ଭାଙ୍ଗ;
    }

    ଯଦି (!variable_present && extends_cl != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ext_cl_ଗଣନା = extends_cl->class_obj_member_ଗଣନା;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	ଫେରନ୍ତୁ intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_ଗଣନା,
					extends_cl->class_extends);
    }

    ଫେରନ୍ତୁ variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା if_ଗଣନା = ifcl->class_obj_member_ଗଣନା;
    ଯଦି (if_ଗଣନା == 0)
	ଫେରନ୍ତୁ TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_data);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cl_ଗଣନା = objmembers_gap->ga_len;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା if_i = 0; if_i < if_ଗଣନା; ++if_i)
    {
	ଯଦି (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_ଗଣନା, extends_cl))
	{
	    semsg(_(e_variable_str_of_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    ଫେରନ୍ତୁ FALSE;
	}
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
intf_method_type_matches(
    ufunc_T	*if_method,
    ufunc_T	*cl_method,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    ଯଦି (cl_method->uf_generic_argcount != if_method->uf_generic_argcount)
    {
	semsg(_(e_type_var_ଗଣନା_of_method_str_differs_from_interface_str),
						cl_method->uf_name, intf_name);
	ଫେରନ୍ତୁ FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (ଅକ୍ଷର *)if_method->uf_name;
    where.wt_kind = WT_METHOD;
    ଯଦି (check_type(if_method->uf_func_type, cl_method->uf_func_type, TRUE,
								where) == FAIL)
	ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
intf_method_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cl_ଗଣନା,
    class_T *extends_cl,
    char_u  *intf_name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		method_present  = FALSE;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା cl_i = 0; cl_i < cl_ଗଣନା; ++cl_i)
    {
	ଯଦି (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    ଯଦି (!intf_method_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		ଫେରନ୍ତୁ FALSE;
	    method_present = TRUE;
	    ଭାଙ୍ଗ;
	}
    }

    ଯଦି (!method_present && extends_cl != ଖାଲି)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ext_cl_ଗଣନା = extends_cl->class_obj_method_ଗଣନା;
	ଫେରନ୍ତୁ intf_method_present(if_ufunc, ext_cl_fp, ext_cl_ଗଣନା,
					extends_cl->class_extends, intf_name);
    }

    ଫେରନ୍ତୁ method_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା if_ଗଣନା = ifcl->class_obj_method_ଗଣନା;
    ଯଦି (if_ଗଣନା == 0)
	ଫେରନ୍ତୁ TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_data);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cl_ଗଣନା = objmethods_gap->ga_len;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା if_i = 0; if_i < if_ଗଣନା; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	ଯଦି (!intf_method_present(if_fp[if_i], cl_fp, cl_ଗଣନା, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_method_str_of_interface_str_not_implemented),
		    if_name, intf_class_name);
	    ଫେରନ୍ତୁ FALSE;
	}
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		success = TRUE;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_data)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	ଯଦି (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_found_str), impl);
	    success = FALSE;
	    ଭାଙ୍ଗ;
	}

	ଯଦି (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == ଖାଲି
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    ଭାଙ୍ଗ;
	}

	class_T *ifcl = tv.vval.v_class;
	ଯଦି (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    ଭାଙ୍ଗ;
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

    ଯଦି (!success)
	ଫେରନ୍ତୁ FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_data;
    cl->class_interface_ଗଣନା = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_data)[i],
		((class_T **)intf_classes_gap->ga_data)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = ଖାଲି;
    cl->class_interface_ଗଣନା = 0;
    cl->class_extends = ଖାଲି;

    ଫେରନ୍ତୁ success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < intf_classes_gap->ga_len; j++)
    {
	ଯଦି (((class_T **)intf_classes_gap->ga_data)[j] == ifcl)
	    ଫେରନ୍ତୁ TRUE;
    }

    ଫେରନ୍ତୁ FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.length);
    ଯଦି (intf_name == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (ga_grow(impl_gap, 1) == FAIL)
	ଯାଅ fail;

    char_u **intf_names = (char_u **)impl_gap->ga_data;
    intf_names[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    ଯଦି (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	ଯାଅ fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_data;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    ଫେରନ୍ତୁ TRUE;
fail:
    vim_free(intf_name);
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < super->class_interface_ଗଣନା; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	ଯଦି (!is_interface_class_present(intf_classes_gap, ifcl))
	    add_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_func_arg_names(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_data)[fi];

	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < uf->uf_args.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_args.ga_data)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_data + mi)->ocm_name.string;
		    ଯଦି (STRCMP(aname, mname) == 0)
		    {
			ଯଦି (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_argument_already_declared_in_class_str),
				aname);

			ଫେରନ୍ତୁ FALSE;
		    }
		}
	    }
	}
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_reserved_varname(char_u *varname, char_u *varname_end)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା reserved = FALSE;
    char_u save_varname_end = *varname_end;
    *varname_end = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_end = save_varname_end;

    ଫେରନ୍ତୁ reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_end)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_end - varname)};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    ଯଦି (*pstr.string == '_')
    {
	pstr.string++;
	pstr.length--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_data) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.length};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    ଯଦି (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.length--;
		q_protected = true;
	    }

	    ଯଦି (pstr.length == qstr.length
		&& STRNCMP(pstr.string, qstr.string, pstr.length) == 0)
	    {
		char_u	save_c = *varname_end;

		*varname_end = NUL;
		ଯଦି (p_protected != q_protected)
		    semsg(_(e_public_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		ଅନ୍ୟଥା
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_end = save_c;
		dup = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
    }

    ଫେରନ୍ତୁ dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_duplicate_method(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_data)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    ଯଦି (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		ଫେରନ୍ତୁ TRUE;
	    }
	}
    }

    ଫେରନ୍ତୁ FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_valid_constructor(ufunc_T *uf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_abstract, ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_static)
{
    // Constructors are not allowed in abstract classes.
    ଯଦି (is_abstract)
    {
	emsg(_(e_cannot_define_new_method_in_abstract_class));
	ଫେରନ୍ତୁ FALSE;
    }
    // A constructor is always static, no need to define it so.
    ଯଦି (has_static)
    {
	emsg(_(e_cannot_define_new_method_as_static));
	ଫେରନ୍ତୁ FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    ଯଦି (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_method));
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
object_check_builtin_method_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    valid = FALSE;
    type_T  method_sig;
    type_T  method_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_FIELD(method_sig);
    CLEAR_FIELD(method_rt);
    method_sig.tt_type = VAR_FUNC;

    ଯଦି (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	method_rt.tt_type = VAR_ସଂଖ୍ୟା;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	method_rt.tt_type = VAR_BOOL;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	method_rt.tt_type = VAR_STRING;
	method_sig.tt_member = &method_rt;
	valid = TRUE;
    }
    ଅନ୍ୟଥା
	semsg(_(e_builtin_object_method_str_not_supported), uf->uf_name);

    where.wt_func_name = (ଅକ୍ଷର *)uf->uf_name;
    where.wt_kind = WT_METHOD;
    ଯଦି (valid && !check_type(&method_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    ଫେରନ୍ତୁ valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_valid_builtin_obj_methodname(char_u *funcname)
{
    ବାଛ (funcname[0])
    {
	ସ୍ଥିତି 'e':
	    ଫେରନ୍ତୁ STRNCMP(funcname, "empty", 5) == 0;

	ସ୍ଥିତି 'l':
	    ଫେରନ୍ତୁ STRNCMP(funcname, "len", 3) == 0;

	ସ୍ଥିତି 'n':
	    ଫେରନ୍ତୁ STRNCMP(funcname, "new", 3) == 0;

	ସ୍ଥିତି 's':
	    ଫେରନ୍ତୁ STRNCMP(funcname, "string", 6) == 0;
    }

    ଫେରନ୍ତୁ FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_method(
    class_T		*cl,
    class_builtin_T	builtin_method,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*method_idx)
{
    *method_idx = -1;

    ଯଦି (cl == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    *method_idx = cl->class_builtin_methods[builtin_method];
    ଫେରନ୍ତୁ *method_idx != -1 ? cl->class_obj_methods[*method_idx] : ଖାଲି;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
update_member_method_lookup_table(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pobj_method_offset)
{
    ଯଦି (ifcl == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(ଆକାର(itf2class_T)
				+ ifcl->class_obj_member_ଗଣନା * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
    ଯଦି (if2cl == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = FALSE;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା if_i = 0; if_i < ifcl->class_obj_member_ଗଣନା; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା cl_i = 0; cl_i < cl->class_obj_member_ଗଣନା; ++cl_i)
	{
	    ଯଦି (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା *table = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(if2cl + 1);
		table[if_i] = cl_i;
		ଭାଙ୍ଗ;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(ଆକାର(itf2class_T)
				+ ifcl->class_obj_method_ଗଣନା * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
    ଯଦି (if2cl == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    if2cl->i2c_next = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_method = TRUE;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା if_i = 0; if_i < ifcl->class_obj_method_ଗଣନା; ++if_i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	done = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    ଯଦି (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_data)[cl_i]->uf_name) == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା *table = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(if2cl + 1);
		table[if_i] = cl_i;
		done = TRUE;
		ଭାଙ୍ଗ;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	ଯଦି (!done)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    ଯଦି (cl->class_extends != ifcl)
	    {
		class_T		*parent = cl->class_extends;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		method_offset = objmethods->ga_len;

		ଯେତେବେଳେ (!done && parent != ଖାଲି && parent != ifcl)
		{
		    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା cl_i = 0;
			    cl_i < parent->class_obj_method_ଗଣନା_child; ++cl_i)
		    {
			ଯଦି (STRCMP(if_name,
				    parent->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *table = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(if2cl + 1);
			    table[if_i] = method_offset + cl_i;
			    done = TRUE;
			    ଭାଙ୍ଗ;
			}
		    }
		    method_offset += parent->class_obj_method_ଗଣନା_child;
		    parent = parent->class_extends;
		}
	    }

	    ଯଦି (!done)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା *table = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(if2cl + 1);
		table[if_i] = pobj_method_offset + if_i;
	    }
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_interface_ଗଣନା; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	ଯେତେବେଳେ (ifcl != ଖାଲି)
	{
	    ଯଦି (update_member_method_lookup_table(ifcl, cl, objmethods_gap,
								0) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    ଯଦି (extends_cl != ଖାଲି)
    {
	class_T		*pclass = extends_cl;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pobj_method_offset = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	ଯେତେବେଳେ (pclass != ଖାଲି)
	{
	    ଯଦି (update_member_method_lookup_table(pclass, cl,
			objmethods_gap, pobj_method_offset) == FAIL)
		ଫେରନ୍ତୁ FAIL;

	    pobj_method_offset += pclass->class_obj_method_ଗଣନା_child;
	    pclass = pclass->class_extends;
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_ଗଣନା);
    ଯଦି (cl->class_members_tv == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_class_member_ଗଣନା; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	ଯଦି (m->ocm_init != ଖାଲି)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_ପଦସମଷ୍ଟି(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    ଯଦି (etv == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;

	    ଯଦି (m->ocm_type->tt_type == VAR_ANY
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
	ଅନ୍ୟଥା
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = ଖାଲି;
	}
	set_tv_type(tv, m->ocm_type);
	ଯଦି (m->ocm_flags & OCMFLAG_CONST)
	    item_lock(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_obj_member_ଗଣନା; ++i)
    {
	ଯଦି (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    ଜାରି;

	ଯଦି (i > (is_enum ? 2 : 0))
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

    garray_T ଧାଡ଼ିଗୁଡ଼ିକ_to_free;
    ga_init2(&ଧାଡ଼ିଗୁଡ଼ିକ_to_free, ଆକାର(char_u *), 50);

    ufunc_T *nf = define_function(&fea, ଖାଲି, &ଧାଡ଼ିଗୁଡ଼ିକ_to_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_ଗଣନା,
			    ଖାଲି);

    ga_clear_strings(&ଧାଡ଼ିଗୁଡ଼ିକ_to_free);
    vim_free(fga.ga_data);

    ଯଦି (nf != ଖାଲି && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_data)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	ଯଦି (nf->uf_ret_type != ଖାଲି)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_args = ଖାଲି;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*fcount = loop == 1 ? &cl->class_class_function_ଗଣନା
						: &cl->class_obj_method_ଗଣନା;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା parent_ଗଣନା = 0;
	ଯଦି (extends_cl != ଖାଲି)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    parent_ଗଣନା = loop == 1
				? 0
				: extends_cl->class_obj_method_ଗଣନା;

	*fcount = parent_ଗଣନା + gap->ga_len;
	ଯଦି (*fcount == 0)
	{
	    *fup = ଖାଲି;
	    ଜାରି;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	ଯଦି (*fup == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_data, ଆକାର(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_data);
	ଯଦି (loop == 1)
	    cl->class_class_function_ଗଣନା_child = gap->ga_len;
	ଅନ୍ୟଥା
	    cl->class_obj_method_ଗଣନା_child = gap->ga_len;

	ଯଦି (loop == 2)
	{
	    // Copy instance methods from the parent.

	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < parent_ଗଣନା; ++i)
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
		ଯଦି (nf != ଖାଲି && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_args != ଖାଲି)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    ଯଦି (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (ଅକ୍ଷର *)pname;
			where.wt_kind = WT_METHOD;
			(ଶୂନ୍ୟ)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    ଯଦି (i < gap->ga_len)
		fp->uf_defclass = cl;
	    ଯଦି (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
update_builtin_method_index(class_T *cl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

    ପାଇଁ (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    ପାଇଁ (i = 0; i < cl->class_obj_method_ଗଣନା; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	ଯଦି (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	ଅନ୍ୟଥା ଯଦି (cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_EMPTY] = i;
	ଅନ୍ୟଥା ଯଦି (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    ସ୍ଥିର char_u *
find_class_name_end(char_u *arg)
{
    char_u *end = arg;

    ଯେତେବେଳେ (ASCII_ISALNUM(*end) || *end == '_'
	    || (*end == '.' && (ASCII_ISALNUM(end[1]) || end[1] == '_')))
	++end;

    ଫେରନ୍ତୁ end;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dup = FALSE;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_data) + i;

	ଯଦି (name.length == m->ocm_name.length
	    && STRNCMP(name.string, m->ocm_name.string, name.length) == 0)
	{
	    char_u  save_c = *varname_end;

	    *varname_end = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_end = save_c;
	    dup = TRUE;
	    ଭାଙ୍ଗ;
	}
    }

    ଫେରନ୍ତୁ dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
enum_parse_ମୂଲ୍ୟଗୁଡ଼ିକ(
    exarg_T	*eap,
    class_T	*en,
    char_u	*ଧାଡ଼ି,
    garray_T	*gap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_enum_ମୂଲ୍ୟଗୁଡ଼ିକ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*enum_end)
{
    evalarg_T	evalarg;
    char_u	*p = ଧାଡ଼ି;
    ଅକ୍ଷର	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	last_char = NUL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
    ଯେତେବେଳେ (*p != NUL)
    {
	// ignore comment
	ଯଦି (*p == '#')
	    ଭାଙ୍ଗ;

	ଯଦି (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_ମୂଲ୍ୟ_declaration_str), p);
	    ଭାଙ୍ଗ;
	}

	char_u *eni_name_start = p;
	char_u *eni_name_end = to_name_end(p, FALSE);

	ଯଦି (is_duplicate_enum(gap, eni_name_start, eni_name_end))
	    ଭାଙ୍ଗ;

	p = skipwhite(eni_name_end);

	char_u	*init_ପଦସମଷ୍ଟି = ଖାଲି;
	ଯଦି (*p == '(' || *p == '<')
	{
	    ଯଦି (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", ଧାଡ଼ି);
		ଭାଙ୍ଗ;
	    }

	    char_u *ପଦସମଷ୍ଟି_start, *ପଦସମଷ୍ଟି_end;

	    p = eni_name_start;
	    (ଶୂନ୍ୟ)skip_ପଦସମଷ୍ଟି_concatenate(&p, &ପଦସମଷ୍ଟି_start, &ପଦସମଷ୍ଟି_end, &evalarg);

	    ଯେତେବେଳେ (*ପଦସମଷ୍ଟି_start && *ପଦସମଷ୍ଟି_start != '(' && *ପଦସମଷ୍ଟି_start != '<')
		ପଦସମଷ୍ଟି_start++;

	    ଯଦି (ପଦସମଷ୍ଟି_end > ପଦସମଷ୍ଟି_start)
		init_ପଦସମଷ୍ଟି = vim_strnsave(ପଦସମଷ୍ଟି_start, ପଦସମଷ୍ଟି_end - ପଦସମଷ୍ଟି_start);
	}

	ଯଦି (init_ପଦସମଷ୍ଟି == ଖାଲି)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, ଆକାର(initexpr_buf), "%s.new()", en->class_name.string);
	ଅନ୍ୟଥା
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, ଆକାର(initexpr_buf), "%s.new%s", en->class_name.string,
		init_ପଦସମଷ୍ଟି);
	    vim_free(init_ପଦସମଷ୍ଟି);
	}
	ଯଦି (add_member(gap, eni_name_start, eni_name_end, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    ଭାଙ୍ଗ;

	++*ଅଙ୍କ_enum_ମୂଲ୍ୟଗୁଡ଼ିକ;

	ଯଦି (*p != '#')
	    last_char = *p;

	ଯଦି (*p != NUL && *p != ',')
	    ଭାଙ୍ଗ;

	ଯଦି (*p == ',')
	{
	    ଯଦି (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", ଧାଡ଼ି);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", ଧାଡ଼ି);
		ଭାଙ୍ଗ;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    ଯଦି (*p != NUL && *p != '#')
    {
	ଯଦି (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_argument_str), p);
	rc = FAIL;
    }

    ଯଦି (last_char != ',')
	// last enum value should not be terminated by ","
	*enum_end = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, ଖାଲି);

    ଫେରନ୍ତୁ rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
enum_add_ମୂଲ୍ୟଗୁଡ଼ିକ_member(
    class_T	*en,
    garray_T	*gap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_enum_ମୂଲ୍ୟଗୁଡ଼ିକ,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_enum_ମୂଲ୍ୟଗୁଡ଼ିକ; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;

	ଯଦି (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.length);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.length);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    ଯଦି (type == ଖାଲି)
	ଯାଅ done;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    ଯଦି (type->tt_member != ଖାଲି)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = add_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_data, fga.ga_len));

done:
    vim_free(fga.ga_data);

    ଫେରନ୍ତୁ rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
enum_clear_constructors(class_T *en)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < en->class_class_function_ଗଣନା; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	ଯଦି (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    ଶୂନ୍ୟ
enum_set_internal_obj_vars(class_T *en, object_T *enval)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

    ପାଇଁ (i = 0; i < en->class_class_member_ଗଣନା; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	ଯଦି (en_tv != ଖାଲି && en_tv->v_type == VAR_UNKNOWN)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (i >= en->class_class_member_ଗଣନା)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	ଫେରନ୍ତୁ;

    // First object variable is the name
    ocmember_T *ମୂଲ୍ୟ_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(ମୂଲ୍ୟ_ocm->ocm_name.string, ମୂଲ୍ୟ_ocm->ocm_name.length);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_ସଂଖ୍ୟା;
    ord_tv->vval.v_ସଂଖ୍ୟା = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
skip_class_body(exarg_T *eap)
{
    ପାଇଁ (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	ଯଦି (theline == ଖାଲି)
	    ଭାଙ୍ଗ;

	char_u	*p = theline;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	found;

	// skip ':' and blanks
	ପାଇଁ (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	found = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	ଯଦି (found)
	    ଭାଙ୍ଗ;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    ଶୂନ୍ୟ
ex_class(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_class = eap->cmdidx == CMD_class;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_abstract = eap->cmdidx == CMD_abstract;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_enum = eap->cmdidx == CMD_enum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		added_enum_ମୂଲ୍ୟଗୁଡ଼ିକ = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_interface;
    long	start_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    ଯଦି (eap->skip)
    {
	skip_class_body(eap);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (is_abstract)
    {
	ଯଦି (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_argument_str), arg);
	    ଫେରନ୍ତୁ;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    ଯଦି (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	ଯଦି (is_class)
	    emsg(_(e_class_can_only_be_defined_in_vim9_script));
	ଅନ୍ୟଥା ଯଦି (is_enum)
	    emsg(_(e_enum_can_only_be_defined_in_vim9_script));
	ଅନ୍ୟଥା
	    emsg(_(e_interface_can_only_be_defined_in_vim9_script));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (!ASCII_ISUPPER(*arg))
    {
	ଯଦି (is_class)
	    semsg(_(e_class_name_must_start_with_uppercase_letter_str), arg);
	ଅନ୍ୟଥା ଯଦି (is_enum)
	    semsg(_(e_enum_name_must_start_with_uppercase_letter_str), arg);
	ଅନ୍ୟଥା
	    semsg(_(e_interface_name_must_start_with_uppercase_letter_str),
									  arg);
	ଫେରନ୍ତୁ;
    }
    char_u *name_end = find_name_end(arg, ଖାଲି, ଖାଲି, FNE_CHECK_START);
    ଯଦି (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	ଫେରନ୍ତୁ;
    }
    char_u *name_start = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = ଖାଲି;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, ଆକାର(char_u *), 5);

    arg = skipwhite(name_end);
    ଯେତେବେଳେ (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	ଯଦି (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    ଯଦି (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		ଯାଅ early_ret;
	    }
	    ଯଦି (extends != ଖାଲି)
	    {
		emsg(_(e_duplicate_extends));
		ଯାଅ early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *end = find_class_name_end(arg);
	    ଯଦି (!IS_WHITE_OR_NUL(*end))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		ଯାଅ early_ret;
	    }
	    extends = vim_strnsave(arg, end - arg);
	    ଯଦି (extends == ଖାଲି)
		ଯାଅ early_ret;

	    arg = skipwhite(end + 1);
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    ଯଦି (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		ଯାଅ early_ret;
	    }

	    ଯଦି (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		ଯାଅ early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    ପାଇଁ (;;)
	    {
		char_u *impl_end = find_class_name_end(arg);
		ଯଦି ((!IS_WHITE_OR_NUL(*impl_end) && *impl_end != ',')
			|| (*impl_end == ','
			    && !IS_WHITE_OR_NUL(*(impl_end + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    ଯାଅ early_ret;
		}
		ଯଦି (impl_end - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    ଯାଅ early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_end - arg);
		ଯଦି (iname == ଖାଲି)
		    ଯାଅ early_ret;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ga_impl.ga_len; ++i)
		    ଯଦି (STRCMP(((char_u **)ga_impl.ga_data)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			ଯାଅ early_ret;
		    }
		ଯଦି (ga_add_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    ଯାଅ early_ret;
		}
		ଯଦି (*impl_end != ',')
		{
		    arg = skipwhite(impl_end);
		    ଭାଙ୍ଗ;
		}
		arg = skipwhite(impl_end + 1);
	    }
	}
	ଅନ୍ୟଥା
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_strings(&ga_impl);
	    ଫେରନ୍ତୁ;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, ଆକାର(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, ଆକାର(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, ଆକାର(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, ଆକାର(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, ଆକାର(ufunc_T *), 10);

    class_T *cl = ଖାଲି;
    class_T *extends_cl = ଖାଲି;  // class from "extends" argument
    class_T **intf_classes = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    intf_ଗଣନା = 0;	// number of items in "intf_classes"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଅଙ୍କ_enum_ମୂଲ୍ୟଗୁଡ଼ିକ = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    ଯଦି (cl == ଖାଲି)
	ଯାଅ cleanup;

    ଯଦି (is_enum)
	cl->class_flags = CLASS_ENUM;
    ଅନ୍ୟଥା ଯଦି (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    ଅନ୍ୟଥା ଯଦି (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.length = (size_t)(name_end - name_start);
    cl->class_name.string = vim_strnsave(name_start, cl->class_name.length);
    ଯଦି (cl->class_name.string == ଖାଲି)
	ଯାଅ cleanup;

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = set_var_const(cl->class_name.string, 0, ଖାଲି, &tv, FALSE, 0, 0);
    ଯଦି (rc == FAIL)
	ଯାଅ cleanup;

    ଯଦି (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	ଯଦି (add_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, ଖାଲି) == FAIL)
	    ଯାଅ cleanup;

	varname = (char_u *)"ordinal";
	ଯଦି (add_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_ସଂଖ୍ୟା, ଖାଲି) == FAIL)
	    ଯାଅ cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		enum_end = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା success = FALSE;
    ପାଇଁ (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	ଯଦି (theline == ଖାଲି)
	    ଭାଙ୍ଗ;
	char_u *ଧାଡ଼ି = skipwhite(theline);

	// Skip empty and comment lines.
	ଯଦି (*ଧାଡ଼ି == NUL)
	    ଜାରି;
	ଯଦି (*ଧାଡ଼ି == '#')
	{
	    ଯଦି (vim9_bad_comment(ଧାଡ଼ି))
		ଭାଙ୍ଗ;
	    ଜାରି;
	}

	char_u *p = ଧାଡ଼ି;

	ଅକ୍ଷର	*end_name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	shortlen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fullen;
	ଯଦି (is_class)
	{
	    end_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	ଅନ୍ୟଥା ଯଦି (is_enum)
	{
	    end_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	ଅନ୍ୟଥା
	{
	    end_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	ପାଇଁ (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_start = p;
	ଯଦି (checkforcmd(&p, end_name, shortlen))
	{
	    ଯଦି (STRNCMP(cmd_start, end_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_start);
	    ଅନ୍ୟଥା ଯଦି (*p == '|' || !ends_excmd2(ଧାଡ଼ି, p))
		semsg(_(e_trailing_characters_str), p);
	    ଅନ୍ୟଥା
		success = TRUE;
	    ଭାଙ୍ଗ;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	wrong_endname = FALSE;
	ଯଦି (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	ଅନ୍ୟଥା ଯଦି (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	ଅନ୍ୟଥା
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	ଯଦି (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), ଧାଡ଼ି, end_name);
	    ଭାଙ୍ଗ;
	}

	ଯଦି (is_enum && !enum_end)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    ଯଦି (enum_parse_ମୂଲ୍ୟଗୁଡ଼ିକ(eap, cl, ଧାଡ଼ି, &classmembers,
					&ଅଙ୍କ_enum_ମୂଲ୍ୟଗୁଡ଼ିକ, &enum_end) == FAIL)
		ଭାଙ୍ଗ;

	    ଯଦି (enum_end)
	    {
		// Add the enum "values" class variable.
		enum_add_ମୂଲ୍ୟଗୁଡ଼ିକ_member(cl, &classmembers, ଅଙ୍କ_enum_ମୂଲ୍ୟଗୁଡ଼ିକ,
							&type_list);
		added_enum_ମୂଲ୍ୟଗୁଡ଼ିକ = TRUE;
	    }
	    ଜାରି;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_public = FALSE;
	ଯଦି (checkforcmd(&p, "public", 3))
	{
	    ଯଦି (STRNCMP(ଧାଡ଼ି, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ଧାଡ଼ି);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (is_interface)
	    {
		emsg(_(e_public_variable_not_supported_in_interface));
		ଭାଙ୍ଗ;
	    }
	    has_public = TRUE;
	    p = skipwhite(ଧାଡ଼ି + 6);

	    ଯଦି (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_public_keyword_not_supported_for_method));
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_public_must_be_followed_by_var_static_final_or_const));
		ଭାଙ୍ଗ;
	    }
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା abstract_method = FALSE;
	char_u *pa = p;
	ଯଦି (checkforcmd(&p, "abstract", 3))
	{
	    ଯଦି (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_used_in_enum));
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_used_in_interface));
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (!is_abstract)
	    {
		semsg(_(e_abstract_method_in_concrete_class), pa);
		ଭାଙ୍ଗ;
	    }

	    p = skipwhite(pa + 8);
	    ଯଦି (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		ଭାଙ୍ଗ;
	    }

	    abstract_method = TRUE;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_static = FALSE;
	char_u *ps = p;
	ଯଦି (checkforcmd(&p, "static", 4))
	{
	    ଯଦି (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (is_interface)
	    {
		emsg(_(e_static_member_not_supported_in_interface));
		ଭାଙ୍ଗ;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    ଯଦି (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		ଭାଙ୍ଗ;
	    }
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_final = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_var = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_const = FALSE;
	ଯଦି (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	ଅନ୍ୟଥା ଯଦି (checkforcmd(&p, "final", 5))
	{
	    ଯଦି (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_in_interface));
		ଭାଙ୍ଗ;
	    }
	    has_final = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (checkforcmd(&p, "const", 5))
	{
	    ଯଦି (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_in_interface));
		ଭାଙ୍ଗ;
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
	ଯଦି (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_end = ଖାଲି;
	    type_T *type = ଖାଲି;
	    char_u *init_ପଦସମଷ୍ଟି = ଖାଲି;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_type = FALSE;

	    ଯଦି (!eval_isnamec1(*p))
	    {
		ଯଦି (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), ଧାଡ଼ି);
		ଅନ୍ୟଥା
		    semsg(_(e_invalid_object_variable_declaration_str), ଧାଡ଼ି);
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_in_interface),
			varname);
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (parse_member(eap, ଧାଡ଼ି, varname, has_public,
			  &varname_end, &has_type, &type_list, &type,
			  !is_interface ? &init_ପଦସମଷ୍ଟି: ଖାଲି) == FAIL)
		ଭାଙ୍ଗ;

	    ଯଦି (is_reserved_varname(varname, varname_end)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_end))
	    {
		vim_free(init_ପଦସମଷ୍ଟି);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (add_member(has_static ? &classmembers : &objmembers, varname,
				varname_end, has_public, has_final, has_const,
				has_type, type, init_ପଦସମଷ୍ଟି) == FAIL)
	    {
		vim_free(init_ପଦସମଷ୍ଟି);
		ଭାଙ୍ଗ;
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
	ଅନ୍ୟଥା ଯଦି (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	ଧାଡ଼ିଗୁଡ଼ିକ_to_free;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    ଯଦି (has_public)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_public_keyword_not_supported_for_method));
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_in_class_str), ଧାଡ଼ି);
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_method_not_supported_in_interface), p);
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (has_static && !is_new && SAFE_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_method_not_supported), p);
		ଭାଙ୍ଗ;
	    }

	    CLEAR_FIELD(ea);
	    ea.cmd = ଧାଡ଼ି;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&ଧାଡ଼ିଗୁଡ଼ିକ_to_free, ଆକାର(char_u *), 50);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା class_flags;
	    ଯଦି (is_interface)
		class_flags = CF_INTERFACE;
	    ଅନ୍ୟଥା
		class_flags = abstract_method ? CF_ABSTRACT_METHOD : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, ଖାଲି, &ଧାଡ଼ିଗୁଡ଼ିକ_to_free,
			class_flags, objmembers.ga_data, objmembers.ga_len,
			ଖାଲି);
	    ga_clear_strings(&ଧାଡ଼ିଗୁଡ଼ିକ_to_free);

	    ଯଦି (uf != ଖାଲି)
	    {
		ଯଦି (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_method_not_supported_in_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    ଭାଙ୍ଗ;
		}

		// check for builtin method
		ଯଦି (!is_new && SAFE_islower(*uf->uf_name) &&
					!object_check_builtin_method_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    ଭାଙ୍ଗ;
		}

		// Check the uf_name isn't used already.
		ଯଦି (is_duplicate_method(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    ଭାଙ୍ଗ;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		ଯଦି (ga_grow(fgap, 1) == OK)
		{
		    ଯଦି (is_new)
			uf->uf_flags |= FC_NEW;

		    ଯଦି (abstract_method)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_data)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	ଅନ୍ୟଥା
	{
	    ଯଦି (is_class)
		semsg(_(e_not_valid_command_in_class_str), ଧାଡ଼ି);
	    ଅନ୍ୟଥା ଯଦି (is_enum)
		semsg(_(e_not_valid_command_in_enum_str), ଧାଡ଼ି);
	    ଅନ୍ୟଥା
		semsg(_(e_not_valid_command_in_interface_str), ଧାଡ଼ି);
	    ଭାଙ୍ଗ;
	}
    }

    ଯଦି (theline == ଖାଲି && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    ଯଦି (success && is_enum && (ଅଙ୍କ_enum_ମୂଲ୍ୟଗୁଡ଼ିକ == 0 || !added_enum_ମୂଲ୍ୟଗୁଡ଼ିକ))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_add_ମୂଲ୍ୟଗୁଡ଼ିକ_member(cl, &classmembers, ଅଙ୍କ_enum_ମୂଲ୍ୟଗୁଡ଼ିକ,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    ଯଦି (success && extends != ଖାଲି)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    ଯଦି (success && extends_cl != ଖାଲି)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    ଯଦି (success && extends_cl != ଖାଲି)
    {
	ଯଦି (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	ଅନ୍ୟଥା
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    ଯଦି (success && !is_abstract && extends_cl != ଖାଲି
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, ଆକାର(class_T *), 5);

    ଯଦି (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    ଯଦି (success && extends_cl != ଖାଲି)
	success = add_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_data;
    intf_ଗଣନା = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    ଯଦି (success)
	success = check_func_arg_names(&classfunctions, &objmethods,
							&classmembers);

    ଯଦି (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	ଯଦି (extends_cl != ଖାଲି)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	ଯଦି (add_members_to_class(&classmembers,
				 ଖାଲି,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_ଗଣନା) == FAIL
		|| add_members_to_class(&objmembers,
				 extends_cl == ଖାଲି ? ଖାଲି
					       : extends_cl->class_obj_members,
				 extends_cl == ଖାଲି ? 0
					  : extends_cl->class_obj_member_ଗଣନା,
				 &cl->class_obj_members,
				 &cl->class_obj_member_ଗଣନା) == FAIL)
	    ଯାଅ cleanup;

	ଯଦି (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_ଗଣନା = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    ଯଦି (cl->class_interfaces == ଖାଲି)
		ଯାଅ cleanup;
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_data)[i];
	    VIM_CLEAR(ga_impl.ga_data);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = ଖାଲି;
	}

	ଯଦି (cl->class_interface_ଗଣନା > 0 || extends_cl != ଖାଲି)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    ଯଦି (add_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		ଯାଅ cleanup;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	have_new = FALSE;
	ufunc_T	*class_func = ଖାଲି;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_data)[i];
	    ଯଦି (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		ଭାଙ୍ଗ;
	    }
	}

	ଯଦି (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	ଅନ୍ୟଥା ଯଦି ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    add_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	ଯଦି (add_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    ଯାଅ cleanup;

	update_builtin_method_index(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	ଯଦି ((is_class || is_enum) && cl->class_class_member_ଗଣନା > 0)
	    ଯଦି (add_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		ଫେରନ୍ତୁ;
	    }

	cl->class_type_list = type_list;

	ଯଦି (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	ଫେରନ୍ତୁ;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    ଯଦି (intf_classes != ଖାଲି)
    {
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < intf_ଗଣନା; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_strings(&ga_impl);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା round = 1; round <= 2; ++round)
    {
	garray_T *gap = round == 1 ? &classmembers : &objmembers;
	ଯଦି (gap->ga_len == 0 || gap->ga_data == ଖାଲି)
	    ଜାରି;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_data) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_data)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < classfunctions.ga_len; ++i)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_object,
    char_u	*name,
    char_u	*name_end,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*member_idx)
{
    size_t	len = name_end - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    ଯଦି (m == ଖାଲି)
    {
	member_not_found_ସନ୍ଦେଶ(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	ଫେରନ୍ତୁ &t_any;
    }

    ଫେରନ୍ତୁ m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_object,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		member_idx)
{
    ocmember_T	*m;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		member_ଗଣନା;

    ଯଦି (is_object)
    {
	m = cl->class_obj_members;
	member_ଗଣନା = cl->class_obj_member_ଗଣନା;
    }
    ଅନ୍ୟଥା
    {
	m = cl->class_class_members;
	member_ଗଣନା = cl->class_class_member_ଗଣନା;
    }

    ଯଦି (member_idx >= member_ଗଣନା)
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    ସ୍ଥିର ଶୂନ୍ୟ
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    ଶୂନ୍ୟ
typealias_unref(typealias_T *ta)
{
    ଯଦି (ta != ଖାଲି && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    ଶୂନ୍ୟ
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    ଯଦି (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_in_vim9_script));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_start_with_uppercase_letter_str), arg);
	ଫେରନ୍ତୁ;
    }

    char_u *name_end = find_name_end(arg, ଖାଲି, ଖାଲି, FNE_CHECK_START);
    ଯଦି (!IS_WHITE_OR_NUL(*name_end))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	ଫେରନ୍ତୁ;
    }
    char_u *name_start = arg;

    arg = skipwhite(name_end);
    ଯଦି (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	ଫେରନ୍ତୁ;
    }
    arg++;
    arg = skipwhite(arg);

    ଯଦି (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	ଫେରନ୍ତୁ;
    }

    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, ଖାଲି, ଖାଲି, TRUE);
    ଯଦି (type == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	ଫେରନ୍ତୁ;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cc = *name_end;
    *name_end = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    ଯଦି (eval_variable_import(name_start, &tv) == OK)
    {
	ଯଦି (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_start);
	ଅନ୍ୟଥା
	    semsg(_(e_redefining_script_item_str), name_start);
	clear_tv(&tv);
	ଯାଅ done;
    }

    // Create a script-local variable for the type alias.
    ଯଦି (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_lock = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_start);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_depth = parse_type_depth();
    }
    ଅନ୍ୟଥା
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_lock = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    set_var_const(name_start, current_sctx.sc_sid, ଖାଲି, &tv, FALSE,
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_member_tv(
    class_T	*cl,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    ଯଦି (m == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (*name == '_' && (current_class == ଖାଲି ||
				!class_instance_of(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	set_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    ଅନ୍ୟଥା
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	set_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = 0;
    ocmember_T	*ocm = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		m_idx;

    fp = method_lookup(cl, rettv->v_type, name, len, ଖାଲି);
    ଯଦି (fp == ଖାଲି)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	ଯଦି (ocm == ଖାଲି || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    method_not_found_ସନ୍ଦେଶ(cl, rettv->v_type, name, len);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	ଅନ୍ୟଥା
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_end;
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (ocm == ଖାଲି && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
	ଫେରନ୍ତୁ FAIL;
    }

    // process generic function call
    ଯଦି (fp != ଖାଲି)
    {
	fp = generic_func_get(fp, gfatab);
	ଯଦି (fp == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }

    char_u *argp = name_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    ଯଦି (ret == FAIL)
    {
	ଯେତେବେଳେ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ଫେରନ୍ତୁ FAIL;
    }

    funcexe_T funcexe;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    ଯଦି (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    ଯଦି (evalarg != ଖାଲି)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								ଖାଲି);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ଯଦି (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_found_var);
	ଫେରନ୍ତୁ FAIL;
    }
    *arg = argp;

    ଫେରନ୍ତୁ OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
obj_method_to_partial_tv(object_T *obj, ufunc_T *obj_method, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    ଯଦି (pt == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    pt->pt_refcount = 1;
    ଯଦି (obj != ଖାଲି)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_method;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
class_object_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose UNUSED)	// give error messages
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ret = FAIL;

    ଯଦି (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ଫେରନ୍ତୁ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ଖାଲି, ଖାଲି, FNE_CHECK_START);
    ଯଦି (name_end == name)
	ଫେରନ୍ତୁ FAIL;
    size_t len = name_end - name;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_emsg_save = did_emsg;
    class_T *cl;
    ଯଦି (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    ଅନ୍ୟଥା // VAR_OBJECT
    {
	ଯଦି (rettv->vval.v_object == ଖାଲି)
	{
	    emsg(_(e_using_null_object));
	    ଫେରନ୍ତୁ FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    ଯଦି (cl == ଖାଲି)
    {
	emsg(_(e_incomplete_type));
	ଫେରନ୍ତୁ FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    ଯଦି (*name_end == '<')
    {
	cctx_T	    *cctx = ଖାଲି;

	ଯଦି (evalarg != ଖାଲି)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_end = parse_generic_func_type_args(name, len, name + len,
						&gfatab, cctx);
	ଯଦି (name_end == ଖାଲି)
	    ଯାଅ done;
    }

    ଯଦି (*name_end == '(')
	// Invoke the class or object method
	ret = call_oc_method(cl, name, len, name_end, evalarg, arg,
							&gfatab, rettv);
    ଅନ୍ୟଥା ଯଦି (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_object = rettv->v_type == VAR_OBJECT;
	ଯଦି (get_member_tv(cl, is_object, name, len, ଖାଲି, rettv) == OK)
	{
	    *arg = name_end;
	    ret = OK;
	    ଯାଅ done;
	}

	// could be a class method or an object method
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fidx;
	ufunc_T	*fp = method_lookup(cl, rettv->v_type, name, len, &fidx);
	ଯଦି (fp != ଖାଲି)
	{
	    // Protected methods are not accessible outside the class
	    ଯଦି (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		ଯାଅ done;
	    }

	    ଯଦି (obj_method_to_partial_tv(is_object ? rettv->vval.v_object :
						ଖାଲି, fp, rettv) == FAIL)
		ଯାଅ done;

	    *arg = name_end;
	    ret = OK;
	    ଯାଅ done;
	}

	ଯଦି (did_emsg == did_emsg_save)
	    member_not_found_ସନ୍ଦେଶ(cl, rettv->v_type, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);

    ଫେରନ୍ତୁ ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
find_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ଖାଲି, ଖାଲି, FNE_CHECK_START);
    ଯଦି (name_end == name || *name_end != '.')
	ଫେରନ୍ତୁ ଖାଲି;

    ufunc_T	*fp = ଖାଲି;
    size_t	len = name_end - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    ଯଦି (eval_variable(name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len,
				    0, &tv, ଖାଲି, EVAL_VAR_NOAUTOLOAD) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	ଯାଅ fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    ଯଦି (cl == ଖାଲି)
	ଯାଅ fail_after_eval;
    char_u *fname = name_end + 1;
    char_u *fname_end = find_name_end(fname, ଖାଲି, ଖାଲି, FNE_CHECK_START);
    ଯଦି (fname_end == fname)
	ଯାଅ fail_after_eval;
    len = fname_end - fname;

    fp = method_lookup(cl, tv.v_type, fname, len, ଖାଲି);

    ଯଦି (fp != ଖାଲି)
    {
	fp = eval_generic_func(fp, fname, &fname_end);
	ଯଦି (fp != ଖାଲି)
	    *arg = fname_end;
    }

fail_after_eval:
    clear_tv(&tv);
    ଫେରନ୍ତୁ fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
    class_member_lookup(cl, name, namelen, &idx);
    ଫେରନ୍ତୁ idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *idx)
{
    ocmember_T	*ret_m = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret_idx = -1;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_class_member_ଗଣନା; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	ଯଦି (namelen)
	{
	    ଯଦି (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ଭାଙ୍ଗ;
	}
    }
    ଯଦି (idx != ଖାଲି)
	*idx = ret_idx;
    ଫେରନ୍ତୁ ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    ସ୍ଥିର ufunc_T *
class_method_lookup(class_T *cl, char_u *name, size_t namelen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *idx)
{
    ufunc_T	*ret_fp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret_idx = -1;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_class_function_ଗଣନା; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	ଯଦି (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ଭାଙ୍ଗ;
	}
    }
    ଯଦି (idx != ଖାଲି)
	*idx = ret_idx;
    ଫେରନ୍ତୁ ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
class_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
    class_method_lookup(cl, name, namelen, &idx);
    ଫେରନ୍ତୁ idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
    object_member_lookup(cl, name, namelen, &idx);
    ଫେରନ୍ତୁ idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *idx)
{
    ocmember_T	*ret_m = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret_idx = -1;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_obj_member_ଗଣନା; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	ଯଦି (namelen)
	{
	    ଯଦି (namelen == m->ocm_name.length
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ଭାଙ୍ଗ;
	}
    }
    ଯଦି (idx != ଖାଲି)
	*idx = ret_idx;
    ଫେରନ୍ତୁ ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    ସ୍ଥିର ufunc_T *
object_method_lookup(class_T *cl, char_u *name, size_t namelen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *idx)
{
    ufunc_T	*ret_fp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret_idx = -1;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_obj_method_ଗଣନା; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	ଯଦି (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ଭାଙ୍ଗ;
	}
    }
    ଯଦି (idx != ଖାଲି)
	*idx = ret_idx;
    ଫେରନ୍ତୁ ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
object_method_idx(class_T *cl, char_u *name, size_t namelen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
    object_method_lookup(cl, name, namelen, &idx);
    ଫେରନ୍ତୁ idx;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*idx)
{
    ଯଦି (v_type == VAR_CLASS)
	ଫେରନ୍ତୁ class_member_lookup(cl, name, namelen, idx);

    ଫେରନ୍ତୁ object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    ସ୍ଥିର class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_found = ଖାଲି;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_found = ଖାଲି;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    ପାଇଁ (class_T *super = cl; super != ଖାଲି; super = super->class_extends)
    {
	class_T		*cl_tmp = ଖାଲି;
	ocmember_T	*m = ଖାଲି;
	ଯଦି (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    ଯଦି ((m = object_member_lookup(super, name, len, ଖାଲି)) != ଖାଲି)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	ଯଦି (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    ଯଦି (( m = class_member_lookup(super, name, len, ଖାଲି)) != ଖାଲି)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	ଯଦି (cl_tmp == ଖାଲି)
	    ଭାଙ୍ଗ;  // member is not in this or any super class.
	cl_found = cl_tmp;
	m_found = m;
    }
    ଯଦି (p_m != ଖାଲି)
	*p_m = m_found;
    ଫେରନ୍ତୁ cl_found;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*idx)
{
    ଯଦି (v_type == VAR_CLASS)
	ଫେରନ୍ତୁ class_method_lookup(cl, name, namelen, idx);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ object_method_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    ପାଇଁ (cctx_T *cctx = cctx_arg; cctx != ଖାଲି; cctx = cctx->ctx_outer)
	ଯଦି (cctx->ctx_ufunc != ଖାଲି
			&& class_instance_of(cctx->ctx_ufunc->uf_class, cl))
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    ଯଦି (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_in_class_str),
		m->ocm_name.string, cl->class_name.string);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    ଶୂନ୍ୟ
obj_lock_const_vars(object_T *obj)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < obj->obj_class->class_obj_member_ଗଣନା; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	ଯଦି (ocm->ocm_flags & OCMFLAG_CONST)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sz;

    ଯଦି (cl == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    sz = ଆକାର(object_T) + cl->class_obj_member_ଗଣନା * ଆକାର(typval_T);
    obj = alloc_clear(sz);
    ଯଦି (obj == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    ଯଦି (IS_ENUM(cl))
	enum_set_internal_obj_vars(cl, obj);

    ଫେରନ୍ତୁ obj;
}

/*
 * Make a copy of an object.
 */
    ଶୂନ୍ୟ
copy_object(typval_T *from, typval_T *to)
{
    ଯଦି (from->vval.v_object == ଖାଲି)
	to->vval.v_object = ଖାଲି;
    ଅନ୍ୟଥା
    {
	to->vval.v_object = from->vval.v_object;
	++to->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    ଶୂନ୍ୟ
copy_class(typval_T *from, typval_T *to)
{
    ଯଦି (from->vval.v_class == ଖାଲି)
	to->vval.v_class = ଖାଲି;
    ଅନ୍ୟଥା
    {
	to->vval.v_class = from->vval.v_class;
	++to->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_interface_ଗଣନା; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	ଯଦି (cl->class_interfaces_cl[i] != ଖାଲି)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *next;
    ପାଇଁ (itf2class_T *i2c = cl->class_itf2class; i2c != ଖାଲି; i2c = next)
    {
	next = i2c->i2c_next;
	vim_free(i2c);
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_class_member_ଗଣନା; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	ଯଦି (cl->class_members_tv != ଖାଲି)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_obj_member_ଗଣନା; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_class_function_ଗଣନା; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	ଯଦି (IS_ENUM(cl) && IS_CONSTRUCTOR_METHOD(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_obj_method_ଗଣନା; ++i)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
class_get_selfrefs(class_T *cl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		self_refs = 0;
    typval_T	*tv;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_class_member_ଗଣନା; ++i)
    {
	tv = &cl->class_members_tv[i];
	ଯଦି (tv->v_type == VAR_OBJECT && tv->vval.v_object != ଖାଲି
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    ଫେରନ୍ତୁ self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
can_free_enum(class_T *cl)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_class_member_ଗଣନା; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	ଯଦି (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    ଫେରନ୍ତୁ TRUE;

	ଯଦି (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    ଯଦି (tv->vval.v_list->lv_refcount > 1)
		ଫେରନ୍ତୁ FALSE;
	    ଭାଙ୍ଗ;
	}

	ଯଦି (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    ଫେରନ୍ତୁ FALSE;
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
can_free_class(class_T *cl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		can_free = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		self_refs = 0;

    ଯଦି (IS_ENUM(cl) && !can_free_enum(cl))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    ଯଦି (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    ଫେରନ୍ତୁ can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    ଶୂନ୍ୟ
class_unref(class_T *cl)
{
    ଯଦି (cl == ଖାଲି)
	ଫେରନ୍ତୁ;

    --cl->class_refcount;

    ଯଦି (cl->class_name.string == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
class_free_nonref(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_free = FALSE;

    ପାଇଁ (class_T *cl = first_class; cl != ଖାଲି; cl = next_nonref_class)
    {
	next_nonref_class = cl->class_next_used;
	ଯଦି ((cl->class_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    next_nonref_class = ଖାଲି;
    ଫେରନ୍ତୁ did_free;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_classes(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପାଇଁ (class_T *cl = first_class; cl != ଖାଲି; cl = cl->class_next_used)
	set_ref_in_item_class(cl, copyID, ଖାଲି, ଖାଲି, ଖାଲି);

    ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର object_T *first_object = ଖାଲି;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    ଶୂନ୍ୟ
object_created(object_T *obj)
{
    ଯଦି (first_object != ଖାଲି)
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
    ସ୍ଥିର ଶୂନ୍ୟ
object_cleared(object_T *obj)
{
    ଯଦି (obj->obj_next_used != ଖାଲି)
	obj->obj_next_used->obj_prev_used = obj->obj_prev_used;
    ଯଦି (obj->obj_prev_used != ଖାଲି)
	obj->obj_prev_used->obj_next_used = obj->obj_next_used;
    ଅନ୍ୟଥା ଯଦି (first_object == obj)
	first_object = obj->obj_next_used;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    ଯଦି (!cl)
	ଫେରନ୍ତୁ;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = INT_MAX;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_obj_member_ଗଣନା; ++i)
	clear_tv(tv + i);
}

    ସ୍ଥିର ଶୂନ୍ୟ
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    ଯଦି (!cl)
	ଫେରନ୍ତୁ;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    ସ୍ଥିର ଶୂନ୍ୟ
object_free(object_T *obj)
{
    ଯଦି (in_free_unref_ବସ୍ତୁଗୁଡ଼ିକ)
	ଫେରନ୍ତୁ;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    ଶୂନ୍ୟ
object_unref(object_T *obj)
{
    ଯଦି (obj != ଖାଲି && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
object_free_nonref(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_free = FALSE;

    ପାଇଁ (object_T *obj = first_object; obj != ଖାଲି; obj = obj->obj_next_used)
    {
	ଯଦି ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    ଫେରନ୍ତୁ did_free;
}

    ଶୂନ୍ୟ
object_free_ବସ୍ତୁଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    object_T	*obj_next;

    ପାଇଁ (object_T *obj = first_object; obj != ଖାଲି; obj = obj_next)
    {
	obj_next = obj->obj_next_used;
	ଯଦି ((obj->obj_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    ଶୂନ୍ୟ
emsg_var_cl_define(ଅକ୍ଷର *ସନ୍ଦେଶ, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    ଯଦି (cl_def != ଖାଲି)
	semsg(_(ସନ୍ଦେଶ), m->ocm_name.string, cl_def->class_name.string);
    ଅନ୍ୟଥା
	emsg(_(e_internal_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    ଶୂନ୍ୟ
method_not_found_ସନ୍ଦେଶ(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *method_name = vim_strnsave(name, len);
    ଯଦି ((v_type == VAR_OBJECT)
	    && (class_method_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	ଯଦି (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	ଅନ୍ୟଥା
	    semsg(_(e_class_method_str_accessible_only_using_class_str),
		    method_name, cl->class_name.string);
    }
    ଅନ୍ୟଥା ଯଦି ((v_type == VAR_CLASS)
	    && (object_method_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	ଯଦି (*name == '_')
	    semsg(_(e_cannot_access_protected_method_str), method_name);
	ଅନ୍ୟଥା
	    semsg(_(e_object_method_str_accessible_only_using_object_str),
		    method_name, cl->class_name.string);
    }
    ଅନ୍ୟଥା
	semsg(_(e_method_not_found_on_class_str_str), method_name,
		cl->class_name.string);
    vim_free(method_name);
}

/*
 * Echo a class or object member not found message.
 */
    ଶୂନ୍ୟ
member_not_found_ସନ୍ଦେଶ(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    ଯଦି (v_type == VAR_OBJECT)
    {
	ଯଦି (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	ଅନ୍ୟଥା
	    semsg(_(e_variable_not_found_on_object_str_str), varname,
		    cl->class_name.string);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	ଅନ୍ୟଥା
	{
	    ଯଦି (IS_ENUM(cl))
		semsg(_(e_enum_ମୂଲ୍ୟ_str_not_found_in_enum_str),
			varname, cl->class_name.string);
	    ଅନ୍ୟଥା
		semsg(_(e_class_variable_str_not_found_in_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    ଶୂନ୍ୟ
defcompile_class(class_T *cl)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop = 1; loop <= 2; ++loop)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା func_ଗଣନା = loop == 1 ? cl->class_class_function_ଗଣନା
						: cl->class_obj_method_ଗଣନା;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < func_ଗଣନା; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    ଯଦି (!IS_ABSTRACT_METHOD(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    ଶୂନ୍ୟ
defcompile_classes_in_script(ଶୂନ୍ୟ)
{
    ପାଇଁ (class_T *cl = first_class; cl != ଖାଲି; cl = cl->class_next_used)
    {
	ଯଦି (eval_variable(cl->class_name.string, 0, 0, ଖାଲି, ଖାଲି,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    ଯଦି (eval_variable(name, 0, 0, rettv, ଖାଲି, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	ଫେରନ୍ତୁ rettv->v_type == VAR_CLASS;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
object_call_builtin_method(
    object_T		*obj,
    class_builtin_T	builtin_method,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    midx;

    ଯଦି (obj == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    uf = class_get_builtin_method(obj->obj_class, builtin_method, &midx);
    ଯଦି (uf == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

    ଯଦି (fc == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, ଖାଲି, obj, fc, rettv);

    remove_funccal();

    ଫେରନ୍ତୁ r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
object_empty(object_T *obj)
{
    typval_T	rettv;

    ଯଦି (object_call_builtin_method(obj, CLASS_BUILTIN_EMPTY, 0, ଖାଲି, &rettv)
								== FAIL)
	ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
object_len(object_T *obj)
{
    typval_T	rettv;

    ଯଦି (object_call_builtin_method(obj, CLASS_BUILTIN_LEN, 0, ଖାଲି, &rettv)
								== FAIL)
	ଫେରନ୍ତୁ 0;

    ଫେରନ୍ତୁ tv_to_ସଂଖ୍ୟା(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
object_equal(
	object_T *o1,
	object_T *o2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    ଯଦି (o1 == o2)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (o1 == ଖାଲି || o2 == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    ଯଦି (cl1 != cl2 || cl1 == ଖାଲି || cl2 == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl1->class_obj_member_ଗଣନା; ++i)
	ଯଦି (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		echo_style,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		composite_val)
{
    typval_T	rettv;

    ଯଦି (object_call_builtin_method(obj, CLASS_BUILTIN_STRING, 0, ଖାଲି, &rettv)
								== OK
					&& rettv.vval.v_string != ଖାଲି)
	ଫେରନ୍ତୁ rettv.vval.v_string;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    ଯଦି (cl != ଖାଲି && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    ଅନ୍ୟଥା
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	ଯଦି (cl == ଖାଲି)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ଅନ୍ୟଥା
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.length);
    }
    ଯଦି (cl != ଖାଲି)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < cl->class_obj_member_ଗଣନା; ++i)
	{
	    ଯଦି (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.length);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = ଖାଲି;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, composite_val);
	    ଯଦି (s != ଖାଲି)
		ga_concat(&ga, s);
	    vim_free(tf);
	    ଯଦି (s == ଖାଲି || did_echo_string_emsg)
	    {
		ok = FAIL;
		ଭାଙ୍ଗ;
	    }
	    ଧାଡ଼ି_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    ଯଦି (ok == FAIL)
    {
	vim_free(ga.ga_data);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ga_append(&ga, NUL);
    ଫେରନ୍ତୁ (char_u *)ga.ga_data;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
class_instance_of(class_T *cl, class_T *other_cl)
{
    ଯଦି (cl == other_cl)
	ଫେରନ୍ତୁ TRUE;

    // Recursively check the base classes.
    ପାଇଁ (; cl != ଖାଲି; cl = cl->class_extends)
    {
	ଯଦି (cl == other_cl)
	    ଫେରନ୍ତୁ TRUE;
	// Check the implemented interfaces and the super interfaces
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = cl->class_interface_ଗଣନା - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    ଯେତେବେଳେ (intf != ଖାଲି)
	    {
		ଯଦି (intf == other_cl)
		    ଫେରନ୍ତୁ TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    ଫେରନ୍ତୁ FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    ଶୂନ୍ୟ
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_ସଂଖ୍ୟା = VVAL_FALSE;

    ଯଦି (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_args(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (object_tv->vval.v_object == ଖାଲି)
    {
	ଯଦି (classinfo_tv->vval.v_class == ଖାଲି)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	ଯଦି (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	ଅନ୍ୟଥା
	    c = classinfo_tv->vval.v_class;

	ଯଦି (class_instance_of(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;
	    ଫେରନ୍ତୁ;
	}
    }
}

#endif // FEAT_EVAL
