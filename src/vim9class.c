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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

ਸਥਿਰ class_T *ਪਹਿਲਾ_class = ਨਿਰਾ;
ਸਥਿਰ class_T *ਅਗਲਾ_nonref_class = ਨਿਰਾ;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    ਸਥਿਰ ਖਾਲੀ
class_created(class_T *cl)
{
    ਜੇ (ਪਹਿਲਾ_class != ਨਿਰਾ)
    {
	cl->class_ਅਗਲਾ_ਵਰਤਿਆ = ਪਹਿਲਾ_class;
	ਪਹਿਲਾ_class->class_prev_ਵਰਤਿਆ = cl;
    }
    ਪਹਿਲਾ_class = cl;
}

/*
 * Call this function when a class has been cleared and is about to be freed.
 * It is removed from the list headed by "first_class".
 */
    ਸਥਿਰ ਖਾਲੀ
class_cleared(class_T *cl)
{
    ਜੇ (cl->class_ਅਗਲਾ_ਵਰਤਿਆ != ਨਿਰਾ)
	cl->class_ਅਗਲਾ_ਵਰਤਿਆ->class_prev_ਵਰਤਿਆ = cl->class_prev_ਵਰਤਿਆ;
    ਜੇ (cl->class_prev_ਵਰਤਿਆ != ਨਿਰਾ)
	cl->class_prev_ਵਰਤਿਆ->class_ਅਗਲਾ_ਵਰਤਿਆ = cl->class_ਅਗਲਾ_ਵਰਤਿਆ;
    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ_class == cl)
	ਪਹਿਲਾ_class = cl->class_ਅਗਲਾ_ਵਰਤਿਆ;

    // update the next class to check if needed
    ਜੇ (cl == ਅਗਲਾ_nonref_class)
	ਅਗਲਾ_nonref_class = cl->class_ਅਗਲਾ_ਵਰਤਿਆ;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_member(
    exarg_T	*eap,
    char_u	*ਸਤਰ,
    char_u	*varname,
    ਪੂਰਨ		has_ਜਨਤਕ,	    // TRUE if "public" seen before "varname"
    char_u	**varname_ਸਿਰਾ,
    ਪੂਰਨ		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_ਵਿਅੰਜਕ)
{
    *varname_ਸਿਰਾ = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(varname, FALSE);
    ਜੇ (*varname == '_' && has_ਜਨਤਕ)
    {
	semsg(_(e_ਜਨਤਕ_variable_name_cannot_ਸ਼ੁਰੂ_with_underscore_str), ਸਤਰ);
	ਵਾਪਸ FAIL;
    }

    char_u *colon = skipwhite(*varname_ਸਿਰਾ);
    char_u *type_arg = colon;
    type_T *type = ਨਿਰਾ;
    *has_type = FALSE;
    ਜੇ (*colon == ':')
    {
	ਜੇ (VIM_ISWHITE(**varname_ਸਿਰਾ))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    ਵਾਪਸ FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, ਨਿਰਾ, ਨਿਰਾ, TRUE);
	ਜੇ (type == ਨਿਰਾ || !valid_declaration_type(type))
	    ਵਾਪਸ FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    ਜੇ (type == ਨਿਰਾ && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (init_ਵਿਅੰਜਕ == ਨਿਰਾ && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_ਵਿੱਚ_interface));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, *ਵਿਅੰਜਕ_ਸਿਰਾ;

	ਜੇ (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    ਵਾਪਸ FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(ਖਾਲੀ)skip_ਵਿਅੰਜਕ_concatenate(&init_arg, &ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, &ਵਿਅੰਜਕ_ਸਿਰਾ, &evalarg);

	init_arg = skipwhite(init_arg);
	ਜੇ (*init_arg != NUL && !vim9_comment_ਸ਼ੁਰੂ(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, ਨਿਰਾ);
	    ਵਾਪਸ FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	ਜੇ (type == ਨਿਰਾ)
	    type = &t_any;

	*init_ਵਿਅੰਜਕ = vim_strnsave(ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, ਵਿਅੰਜਕ_ਸਿਰਾ - ਵਿਅੰਜਕ_ਸ਼ੁਰੂ);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (!valid_declaration_type(type))
	ਵਾਪਸ FAIL;

    *type_ret = type;
    ਵਾਪਸ OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_ਸਿਰਾ,
    ਪੂਰਨ		has_ਜਨਤਕ,
    ਪੂਰਨ		has_final,
    ਪੂਰਨ		has_const,
    ਪੂਰਨ		has_type,
    type_T	*type,
    char_u	*init_ਵਿਅੰਜਕ)
{
    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_ਅੰਕੜੇ) + gap->ga_len;
    m->ocm_name.ਲੰਮਾਈ = (size_t)(varname_ਸਿਰਾ - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.ਲੰਮਾਈ);
    ਜੇ (m->ocm_name.string == ਨਿਰਾ)
    {
	m->ocm_name.ਲੰਮਾਈ = 0;
	ਵਾਪਸ FAIL;
    }
    m->ocm_access = has_ਜਨਤਕ ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_ਨਿੱਜੀ : VIM_ACCESS_READ;
    ਜੇ (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    ਜੇ (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    ਜੇ (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    ਜੇ (init_ਵਿਅੰਜਕ != ਨਿਰਾ)
    {
	m->ocm_init = init_ਵਿਅੰਜਕ;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_members_ਮੰਜ਼ਿਲ_class(
    garray_T	*gap,
    ocmember_T	*ਮਾਪਾ_members,
    ਪੂਰਨ		ਮਾਪਾ_ਗਿਣਤੀ,
    ocmember_T	**members,
    ਪੂਰਨ		*member_ਗਿਣਤੀ)
{
    *member_ਗਿਣਤੀ = ਮਾਪਾ_ਗਿਣਤੀ + gap->ga_len;
    *members = *member_ਗਿਣਤੀ == 0 ? ਨਿਰਾ
				       : ALLOC_MULT(ocmember_T, *member_ਗਿਣਤੀ);
    ਜੇ (*member_ਗਿਣਤੀ > 0 && *members == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਲਈ (ਪੂਰਨ i = 0; i < ਮਾਪਾ_ਗਿਣਤੀ; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = ਮਾਪਾ_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.ਲੰਮਾਈ);
	ਜੇ (m->ocm_name.string == ਨਿਰਾ)
	{
	    m->ocm_name.ਲੰਮਾਈ = 0;
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (m->ocm_init != ਨਿਰਾ)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    ਜੇ (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + ਮਾਪਾ_ਗਿਣਤੀ,
			       gap->ga_ਅੰਕੜੇ, ਆਕਾਰ(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_ਅੰਕੜੇ);
    ਵਾਪਸ OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    ਪੂਰਨ
object_ਸੂਚਕਾਂਕ_from_itf_ਸੂਚਕਾਂਕ(class_T *itf, ਪੂਰਨ is_ਵਿਧੀ, ਪੂਰਨ idx, class_T *cl)
{
    ਜੇ (idx >= (is_ਵਿਧੀ ? itf->class_obj_ਵਿਧੀ_ਗਿਣਤੀ
				   : itf->class_obj_member_ਗਿਣਤੀ))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	ਵਾਪਸ 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    ਜੇ (cl == itf)
	ਵਾਪਸ idx;

    itf2class_T		*i2c = ਨਿਰਾ;
    ਪੂਰਨ			searching = TRUE;
    ਪੂਰਨ			ਵਿਧੀ_ਵਿੱਥ = 0;

    ਲਈ (class_T *super = cl; super != ਨਿਰਾ && searching;
						super = super->class_extends)
    {
	ਲਈ (i2c = itf->class_itf2class; i2c != ਨਿਰਾ; i2c = i2c->i2c_ਅਗਲਾ)
	{
	    ਜੇ (i2c->i2c_class == super && i2c->i2c_is_ਵਿਧੀ == is_ਵਿਧੀ)
	    {
		searching = FALSE;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (searching && is_ਵਿਧੀ)
	    // The parent class methods are stored after the current class
	    // methods.
	    ਵਿਧੀ_ਵਿੱਥ += super->class_obj_ਵਿਧੀ_ਗਿਣਤੀ_ਬੱਚਾ;
    }
    ਜੇ (i2c == ਨਿਰਾ)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	ਵਾਪਸ 0;
    }

    // A table follows the i2c for the class
    ਪੂਰਨ *ਸਾਰਣੀ = (ਪੂਰਨ *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    ਵਾਪਸ ਸਾਰਣੀ[idx] + ਵਿਧੀ_ਵਿੱਥ;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    ਸਥਿਰ ਪੂਰਨ
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    ਪੂਰਨ	    is_class)
{
    typval_T	tv;
    ਪੂਰਨ		success = FALSE;

    ਜੇ (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	ਵਾਪਸ success;
    }

    tv.v_type = VAR_UNKNOWN;
    ਜੇ (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_ਮਿਲਿਆ_str), extends_name);
	ਵਾਪਸ success;
    }

    ਜੇ (tv.v_type != VAR_CLASS || tv.vval.v_class == ਨਿਰਾ
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    ਨਹੀਂਤਾਂ
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    ਵਾਪਸ success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    ਸਥਿਰ ਪੂਰਨ
validate_extends_generic_ਵਿਧੀ(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    ਅੱਖਰ	*ਸੁਨੇਹਾ = ਨਿਰਾ;

    ਜੇ (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	ਵਾਪਸ TRUE;

    ਜੇ (super_fp->uf_generic_argcount == 0)
	ਸੁਨੇਹਾ = e_concrete_ਵਿਧੀ_str_override_with_generic_ਵਿਧੀ_ਵਿੱਚ_class_str;
    ਨਹੀਂਤਾਂ ਜੇ (cl_fp->uf_generic_argcount == 0)
	ਸੁਨੇਹਾ = e_generic_ਵਿਧੀ_str_override_with_concrete_ਵਿਧੀ_ਵਿੱਚ_class_str;
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ = e_generic_ਵਿਧੀ_str_type_arguments_mismatch_ਵਿੱਚ_class_str;

    semsg(_(ਸੁਨੇਹਾ), cl_fp->uf_name, super_cl->class_name.string);
    ਵਾਪਸ FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    ਸਥਿਰ ਪੂਰਨ
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    ਪੂਰਨ		ਵਿਧੀ_ਗਿਣਤੀ = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_ਅੰਕੜੇ);

    ਜਦੋਂ (super != ਨਿਰਾ)
    {
	ਪੂਰਨ extends_ਵਿਧੀ_ਗਿਣਤੀ = super->class_obj_ਵਿਧੀ_ਗਿਣਤੀ_ਬੱਚਾ;
	ਜੇ (extends_ਵਿਧੀ_ਗਿਣਤੀ == 0)
	{
	    super = super->class_extends;
	    ਜਾਰੀ;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	ਲਈ (ਪੂਰਨ i = 0; i < extends_ਵਿਧੀ_ਗਿਣਤੀ; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    ਪੂਰਨ	    extends_ਨਿੱਜੀ = (*pstr == '_');
	    ਜੇ (extends_ਨਿੱਜੀ)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    ਲਈ (ਪੂਰਨ j = 0; j < ਵਿਧੀ_ਗਿਣਤੀ; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		ਪੂਰਨ	priv_ਵਿਧੀ = (*qstr == '_');
		ਜੇ (priv_ਵਿਧੀ)
		    qstr++;
		ਜੇ (STRCMP(pstr, qstr) == 0)
		{
		    ਜੇ (priv_ਵਿਧੀ != extends_ਨਿੱਜੀ)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_ਵਿਧੀ_str_ਦਾ_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			ਵਾਪਸ FALSE;
		    }

		    ਜੇ (!validate_extends_generic_ਵਿਧੀ(super,
						extends_methods[i], cl_fp[j]))
			    ਵਾਪਸ FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    ਵਾਪਸ TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    ਸਥਿਰ ਪੂਰਨ
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ਪੂਰਨ member_ਗਿਣਤੀ = objmembers_gap->ga_len;
    ਜੇ (member_ਗਿਣਤੀ == 0)
	ਵਾਪਸ TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_ਅੰਕੜੇ);

    // Validate each member variable
    ਲਈ (ਪੂਰਨ c_i = 0; c_i < member_ਗਿਣਤੀ; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	ਜਦੋਂ (p_cl != ਨਿਰਾ)
	{
	    ਪੂਰਨ p_member_ਗਿਣਤੀ = p_cl->class_obj_member_ਗਿਣਤੀ;

	    ਜੇ (p_member_ਗਿਣਤੀ == 0)
	    {
		p_cl = p_cl->class_extends;
		ਜਾਰੀ;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    ਲਈ (ਪੂਰਨ p_i = 0; p_i < p_member_ਗਿਣਤੀ; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		ਜੇ (STRCMP(pstr, qstr) == 0)
		{
		    ਜੇ (c_protected != p_protected)
			semsg(_(e_ਜਨਤਕ_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    ਨਹੀਂਤਾਂ
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    ਵਾਪਸ FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    ਵਾਪਸ TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    ਸਥਿਰ ਪੂਰਨ
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ਪੂਰਨ member_ਗਿਣਤੀ = objmembers_gap->ga_len;
    ਜੇ (member_ਗਿਣਤੀ == 0)
	ਵਾਪਸ TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_ਅੰਕੜੇ);

    // Validate each member variable
    ਲਈ (ਪੂਰਨ c_i = 0; c_i < member_ਗਿਣਤੀ; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	ਪੂਰਨ	    var_ਮਿਲਿਆ = FALSE;

	// Check in all the parent classes in the lineage
	ਜਦੋਂ (p_cl != ਨਿਰਾ && !var_ਮਿਲਿਆ)
	{
	    ਪੂਰਨ p_member_ਗਿਣਤੀ = p_cl->class_obj_member_ਗਿਣਤੀ;
	    ਜੇ (p_member_ਗਿਣਤੀ == 0)
	    {
		p_cl = p_cl->class_extends;
		ਜਾਰੀ;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    ਲਈ (ਪੂਰਨ p_i = 0; p_i < p_member_ਗਿਣਤੀ; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		ਜੇ (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    ਜਾਰੀ;

		// Ensure the type is matching.
		where.wt_func_name = (ਅੱਖਰ *)c_m->ocm_name.string;
		where.wt_ਕਿਸਮ = WT_MEMBER;

		ਜੇ (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    ਵਾਪਸ FALSE;

		var_ਮਿਲਿਆ = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    ਵਾਪਸ TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    ਸਥਿਰ ਪੂਰਨ
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    ਲਈ (ਪੂਰਨ loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	ਪੂਰਨ extends_ਵਿਧੀ_ਗਿਣਤੀ = loop == 1
				? extends_cl->class_class_function_ਗਿਣਤੀ
				: extends_cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ;
	ਜੇ (extends_ਵਿਧੀ_ਗਿਣਤੀ == 0)
	    ਜਾਰੀ;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	ਪੂਰਨ ਵਿਧੀ_ਗਿਣਤੀ = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_ਅੰਕੜੇ
						: objmethods_gap->ga_ਅੰਕੜੇ);

	ਲਈ (ਪੂਰਨ i = 0; i < extends_ਵਿਧੀ_ਗਿਣਤੀ; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    ਜੇ (!IS_ABSTRACT_ਵਿਧੀ(uf))
		ਜਾਰੀ;

	    ਪੂਰਨ	concrete_ਵਿਧੀ_ਮਿਲਿਆ = FALSE;
	    ਪੂਰਨ	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    ਲਈ (j = 0; !concrete_ਵਿਧੀ_ਮਿਲਿਆ && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		ਜੇ (!IS_ABSTRACT_ਵਿਧੀ(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_ਵਿਧੀ_ਮਿਲਿਆ = TRUE;
	    }

	    ਜੇ (concrete_ਵਿਧੀ_ਮਿਲਿਆ)
		ਜਾਰੀ;

	    ਲਈ (j = 0; j < ਵਿਧੀ_ਗਿਣਤੀ; j++)
	    {
		ਜੇ (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_ਵਿਧੀ_ਮਿਲਿਆ = TRUE;
		    ਰੋਕੋ;
		}
	    }

	    ਜੇ (!concrete_ਵਿਧੀ_ਮਿਲਿਆ)
	    {
		semsg(_(e_abstract_ਵਿਧੀ_str_not_implemented), uf->uf_name);
		ਵਾਪਸ FALSE;
	    }
	}
    }

    ਵਾਪਸ TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    ਸਥਿਰ ਪੂਰਨ
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    ਪੂਰਨ		cl_member_ਗਿਣਤੀ,
    class_T	*extends_cl)
{
    ਪੂਰਨ		variable_present  = FALSE;

    ਲਈ (ਪੂਰਨ cl_i = 0; cl_i < cl_member_ਗਿਣਤੀ; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	ਜੇ (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    ਜਾਰੀ;

	// Ensure the access type is same
	ਜੇ (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_ਦਾ_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    ਵਾਪਸ FALSE;
	}

	// Ensure the type is matching.
	ਜੇ (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	ਨਹੀਂਤਾਂ
	{
	    where.wt_func_name = (ਅੱਖਰ *)m->ocm_name.string;
	    where.wt_ਕਿਸਮ = WT_MEMBER;
	    ਜੇ (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		ਵਾਪਸ FALSE;
	}

	variable_present = TRUE;
	ਰੋਕੋ;
    }

    ਜੇ (!variable_present && extends_cl != ਨਿਰਾ)
    {
	ਪੂਰਨ ext_cl_ਗਿਣਤੀ = extends_cl->class_obj_member_ਗਿਣਤੀ;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	ਵਾਪਸ intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_ਗਿਣਤੀ,
					extends_cl->class_extends);
    }

    ਵਾਪਸ variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    ਸਥਿਰ ਪੂਰਨ
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ਪੂਰਨ if_ਗਿਣਤੀ = ifcl->class_obj_member_ਗਿਣਤੀ;
    ਜੇ (if_ਗਿਣਤੀ == 0)
	ਵਾਪਸ TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_ਅੰਕੜੇ);
    ਪੂਰਨ cl_ਗਿਣਤੀ = objmembers_gap->ga_len;
    ਲਈ (ਪੂਰਨ if_i = 0; if_i < if_ਗਿਣਤੀ; ++if_i)
    {
	ਜੇ (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_ਗਿਣਤੀ, extends_cl))
	{
	    semsg(_(e_variable_str_ਦਾ_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    ਵਾਪਸ FALSE;
	}
    }

    ਵਾਪਸ TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    ਸਥਿਰ ਪੂਰਨ
intf_ਵਿਧੀ_type_matches(
    ufunc_T	*if_ਵਿਧੀ,
    ufunc_T	*cl_ਵਿਧੀ,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    ਜੇ (cl_ਵਿਧੀ->uf_generic_argcount != if_ਵਿਧੀ->uf_generic_argcount)
    {
	semsg(_(e_type_var_ਗਿਣਤੀ_ਦਾ_ਵਿਧੀ_str_differs_from_interface_str),
						cl_ਵਿਧੀ->uf_name, intf_name);
	ਵਾਪਸ FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (ਅੱਖਰ *)if_ਵਿਧੀ->uf_name;
    where.wt_ਕਿਸਮ = WT_ਵਿਧੀ;
    ਜੇ (check_type(if_ਵਿਧੀ->uf_func_type, cl_ਵਿਧੀ->uf_func_type, TRUE,
								where) == FAIL)
	ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    ਸਥਿਰ ਪੂਰਨ
intf_ਵਿਧੀ_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    ਪੂਰਨ	    cl_ਗਿਣਤੀ,
    class_T *extends_cl,
    char_u  *intf_name)
{
    ਪੂਰਨ		ਵਿਧੀ_present  = FALSE;

    ਲਈ (ਪੂਰਨ cl_i = 0; cl_i < cl_ਗਿਣਤੀ; ++cl_i)
    {
	ਜੇ (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    ਜੇ (!intf_ਵਿਧੀ_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		ਵਾਪਸ FALSE;
	    ਵਿਧੀ_present = TRUE;
	    ਰੋਕੋ;
	}
    }

    ਜੇ (!ਵਿਧੀ_present && extends_cl != ਨਿਰਾ)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	ਪੂਰਨ	ext_cl_ਗਿਣਤੀ = extends_cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ;
	ਵਾਪਸ intf_ਵਿਧੀ_present(if_ufunc, ext_cl_fp, ext_cl_ਗਿਣਤੀ,
					extends_cl->class_extends, intf_name);
    }

    ਵਾਪਸ ਵਿਧੀ_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    ਸਥਿਰ ਪੂਰਨ
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    ਪੂਰਨ if_ਗਿਣਤੀ = ifcl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ;
    ਜੇ (if_ਗਿਣਤੀ == 0)
	ਵਾਪਸ TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_ਅੰਕੜੇ);
    ਪੂਰਨ cl_ਗਿਣਤੀ = objmethods_gap->ga_len;
    ਲਈ (ਪੂਰਨ if_i = 0; if_i < if_ਗਿਣਤੀ; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	ਜੇ (!intf_ਵਿਧੀ_present(if_fp[if_i], cl_fp, cl_ਗਿਣਤੀ, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_ਵਿਧੀ_str_ਦਾ_interface_str_not_implemented),
		    if_name, intf_class_name);
	    ਵਾਪਸ FALSE;
	}
    }

    ਵਾਪਸ TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    ਸਥਿਰ ਪੂਰਨ
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ਪੂਰਨ		success = TRUE;

    ਲਈ (ਪੂਰਨ i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_ਅੰਕੜੇ)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	ਜੇ (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_ਮਿਲਿਆ_str), impl);
	    success = FALSE;
	    ਰੋਕੋ;
	}

	ਜੇ (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == ਨਿਰਾ
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    ਰੋਕੋ;
	}

	class_T *ifcl = tv.vval.v_class;
	ਜੇ (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    ਰੋਕੋ;
	}
	((class_T **)intf_classes_gap->ga_ਅੰਕੜੇ)[intf_classes_gap->ga_len]
								= ifcl;
	intf_classes_gap->ga_len++;
	++ifcl->class_refcount;

	// check the variables of the interface match the members of the class
	success = validate_interface_variables(impl, ifcl, objmembers_gap,
								extends_cl);
	clear_tv(&tv);
    }

    ਜੇ (!success)
	ਵਾਪਸ FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_ਅੰਕੜੇ;
    cl->class_interface_ਗਿਣਤੀ = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    ਲਈ (ਪੂਰਨ i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_ਅੰਕੜੇ)[i],
		((class_T **)intf_classes_gap->ga_ਅੰਕੜੇ)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = ਨਿਰਾ;
    cl->class_interface_ਗਿਣਤੀ = 0;
    cl->class_extends = ਨਿਰਾ;

    ਵਾਪਸ success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    ਸਥਿਰ ਪੂਰਨ
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    ਲਈ (ਪੂਰਨ j = 0; j < intf_classes_gap->ga_len; j++)
    {
	ਜੇ (((class_T **)intf_classes_gap->ga_ਅੰਕੜੇ)[j] == ifcl)
	    ਵਾਪਸ TRUE;
    }

    ਵਾਪਸ FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.ਲੰਮਾਈ);
    ਜੇ (intf_name == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਜੇ (ga_grow(impl_gap, 1) == FAIL)
	ਜਾਓ fail;

    char_u **intf_ਨਾਂ = (char_u **)impl_gap->ga_ਅੰਕੜੇ;
    intf_ਨਾਂ[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    ਜੇ (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	ਜਾਓ fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_ਅੰਕੜੇ;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    ਵਾਪਸ TRUE;
fail:
    vim_free(intf_name);
    ਵਾਪਸ FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    ਲਈ (ਪੂਰਨ i = 0; i < super->class_interface_ਗਿਣਤੀ; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	ਜੇ (!is_interface_class_present(intf_classes_gap, ifcl))
	    ਜੋੜੋ_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    ਵਾਪਸ TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    ਸਥਿਰ ਪੂਰਨ
check_func_arg_ਨਾਂ(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    ਲਈ (ਪੂਰਨ loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	ਲਈ (ਪੂਰਨ fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_ਅੰਕੜੇ)[fi];

	    ਲਈ (ਪੂਰਨ i = 0; i < uf->uf_ਦਲੀਲਾਂ.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		ਲਈ (ਪੂਰਨ mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_ਅੰਕੜੇ + mi)->ocm_name.string;
		    ਜੇ (STRCMP(aname, mname) == 0)
		    {
			ਜੇ (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_ਦਲੀਲ_already_declared_ਵਿੱਚ_class_str),
				aname);

			ਵਾਪਸ FALSE;
		    }
		}
	    }
	}
    }

    ਵਾਪਸ TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    ਸਥਿਰ ਪੂਰਨ
is_reserved_varname(char_u *varname, char_u *varname_ਸਿਰਾ)
{
    ਪੂਰਨ reserved = FALSE;
    char_u save_varname_ਸਿਰਾ = *varname_ਸਿਰਾ;
    *varname_ਸਿਰਾ = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_ਸਿਰਾ = save_varname_ਸਿਰਾ;

    ਵਾਪਸ reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    ਸਥਿਰ ਪੂਰਨ
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_ਸਿਰਾ)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_ਸਿਰਾ - varname)};
    ਪੂਰਨ		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    ਜੇ (*pstr.string == '_')
    {
	pstr.string++;
	pstr.ਲੰਮਾਈ--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    ਲਈ (ਪੂਰਨ loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	ਲਈ (ਪੂਰਨ i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_ਅੰਕੜੇ) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.ਲੰਮਾਈ};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    ਜੇ (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.ਲੰਮਾਈ--;
		q_protected = true;
	    }

	    ਜੇ (pstr.ਲੰਮਾਈ == qstr.ਲੰਮਾਈ
		&& STRNCMP(pstr.string, qstr.string, pstr.ਲੰਮਾਈ) == 0)
	    {
		char_u	save_c = *varname_ਸਿਰਾ;

		*varname_ਸਿਰਾ = NUL;
		ਜੇ (p_protected != q_protected)
		    semsg(_(e_ਜਨਤਕ_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		ਨਹੀਂਤਾਂ
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_ਸਿਰਾ = save_c;
		dup = TRUE;
		ਰੋਕੋ;
	    }
	}
    }

    ਵਾਪਸ dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    ਸਥਿਰ ਪੂਰਨ
is_duplicate_ਵਿਧੀ(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    ਲਈ (ਪੂਰਨ loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	ਲਈ (ਪੂਰਨ i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_ਅੰਕੜੇ)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    ਜੇ (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		ਵਾਪਸ TRUE;
	    }
	}
    }

    ਵਾਪਸ FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    ਸਥਿਰ ਪੂਰਨ
is_valid_constructor(ufunc_T *uf, ਪੂਰਨ is_abstract, ਪੂਰਨ has_static)
{
    // Constructors are not allowed in abstract classes.
    ਜੇ (is_abstract)
    {
	emsg(_(e_cannot_define_new_ਵਿਧੀ_ਵਿੱਚ_abstract_class));
	ਵਾਪਸ FALSE;
    }
    // A constructor is always static, no need to define it so.
    ਜੇ (has_static)
    {
	emsg(_(e_cannot_define_new_ਵਿਧੀ_as_static));
	ਵਾਪਸ FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    ਜੇ (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_ਵਿਧੀ));
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    ਸਥਿਰ ਪੂਰਨ
object_check_builtin_ਵਿਧੀ_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    ਪੂਰਨ	    valid = FALSE;
    type_T  ਵਿਧੀ_sig;
    type_T  ਵਿਧੀ_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_ਖੇਤਰ(ਵਿਧੀ_sig);
    CLEAR_ਖੇਤਰ(ਵਿਧੀ_rt);
    ਵਿਧੀ_sig.tt_type = VAR_FUNC;

    ਜੇ (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	ਵਿਧੀ_rt.tt_type = VAR_ਸੰਖਿਆ;
	ਵਿਧੀ_sig.tt_member = &ਵਿਧੀ_rt;
	valid = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	ਵਿਧੀ_rt.tt_type = VAR_BOOL;
	ਵਿਧੀ_sig.tt_member = &ਵਿਧੀ_rt;
	valid = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	ਵਿਧੀ_rt.tt_type = VAR_STRING;
	ਵਿਧੀ_sig.tt_member = &ਵਿਧੀ_rt;
	valid = TRUE;
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_builtin_object_ਵਿਧੀ_str_not_supported), uf->uf_name);

    where.wt_func_name = (ਅੱਖਰ *)uf->uf_name;
    where.wt_ਕਿਸਮ = WT_ਵਿਧੀ;
    ਜੇ (valid && !check_type(&ਵਿਧੀ_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    ਵਾਪਸ valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    ਪੂਰਨ
is_valid_builtin_obj_methodname(char_u *funcname)
{
    ਚੋਣ (funcname[0])
    {
	ਹਾਲਤ 'e':
	    ਵਾਪਸ STRNCMP(funcname, "empty", 5) == 0;

	ਹਾਲਤ 'l':
	    ਵਾਪਸ STRNCMP(funcname, "len", 3) == 0;

	ਹਾਲਤ 'n':
	    ਵਾਪਸ STRNCMP(funcname, "new", 3) == 0;

	ਹਾਲਤ 's':
	    ਵਾਪਸ STRNCMP(funcname, "string", 6) == 0;
    }

    ਵਾਪਸ FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_ਵਿਧੀ(
    class_T		*cl,
    class_builtin_T	builtin_ਵਿਧੀ,
    ਪੂਰਨ			*ਵਿਧੀ_idx)
{
    *ਵਿਧੀ_idx = -1;

    ਜੇ (cl == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    *ਵਿਧੀ_idx = cl->class_builtin_methods[builtin_ਵਿਧੀ];
    ਵਾਪਸ *ਵਿਧੀ_idx != -1 ? cl->class_obj_methods[*ਵਿਧੀ_idx] : ਨਿਰਾ;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    ਸਥਿਰ ਪੂਰਨ
update_member_ਵਿਧੀ_lookup_ਸਾਰਣੀ(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    ਪੂਰਨ		pobj_ਵਿਧੀ_ਵਿੱਥ)
{
    ਜੇ (ifcl == ਨਿਰਾ)
	ਵਾਪਸ OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(ਆਕਾਰ(itf2class_T)
				+ ifcl->class_obj_member_ਗਿਣਤੀ * ਆਕਾਰ(ਪੂਰਨ));
    ਜੇ (if2cl == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    if2cl->i2c_ਅਗਲਾ = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_ਵਿਧੀ = FALSE;

    ਲਈ (ਪੂਰਨ if_i = 0; if_i < ifcl->class_obj_member_ਗਿਣਤੀ; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	ਲਈ (ਪੂਰਨ cl_i = 0; cl_i < cl->class_obj_member_ਗਿਣਤੀ; ++cl_i)
	{
	    ਜੇ (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		ਪੂਰਨ *ਸਾਰਣੀ = (ਪੂਰਨ *)(if2cl + 1);
		ਸਾਰਣੀ[if_i] = cl_i;
		ਰੋਕੋ;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(ਆਕਾਰ(itf2class_T)
				+ ifcl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ * ਆਕਾਰ(ਪੂਰਨ));
    ਜੇ (if2cl == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    if2cl->i2c_ਅਗਲਾ = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_ਵਿਧੀ = TRUE;

    ਲਈ (ਪੂਰਨ if_i = 0; if_i < ifcl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ; ++if_i)
    {
	ਪੂਰਨ	ਪੂਰਾ = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	ਲਈ (ਪੂਰਨ cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    ਜੇ (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_ਅੰਕੜੇ)[cl_i]->uf_name) == 0)
	    {
		ਪੂਰਨ *ਸਾਰਣੀ = (ਪੂਰਨ *)(if2cl + 1);
		ਸਾਰਣੀ[if_i] = cl_i;
		ਪੂਰਾ = TRUE;
		ਰੋਕੋ;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	ਜੇ (!ਪੂਰਾ)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    ਜੇ (cl->class_extends != ifcl)
	    {
		class_T		*ਮਾਪਾ = cl->class_extends;
		ਪੂਰਨ		ਵਿਧੀ_ਵਿੱਥ = objmethods->ga_len;

		ਜਦੋਂ (!ਪੂਰਾ && ਮਾਪਾ != ਨਿਰਾ && ਮਾਪਾ != ifcl)
		{
		    ਲਈ (ਪੂਰਨ cl_i = 0;
			    cl_i < ਮਾਪਾ->class_obj_ਵਿਧੀ_ਗਿਣਤੀ_ਬੱਚਾ; ++cl_i)
		    {
			ਜੇ (STRCMP(if_name,
				    ਮਾਪਾ->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    ਪੂਰਨ *ਸਾਰਣੀ = (ਪੂਰਨ *)(if2cl + 1);
			    ਸਾਰਣੀ[if_i] = ਵਿਧੀ_ਵਿੱਥ + cl_i;
			    ਪੂਰਾ = TRUE;
			    ਰੋਕੋ;
			}
		    }
		    ਵਿਧੀ_ਵਿੱਥ += ਮਾਪਾ->class_obj_ਵਿਧੀ_ਗਿਣਤੀ_ਬੱਚਾ;
		    ਮਾਪਾ = ਮਾਪਾ->class_extends;
		}
	    }

	    ਜੇ (!ਪੂਰਾ)
	    {
		ਪੂਰਨ *ਸਾਰਣੀ = (ਪੂਰਨ *)(if2cl + 1);
		ਸਾਰਣੀ[if_i] = pobj_ਵਿਧੀ_ਵਿੱਥ + if_i;
	    }
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_interface_ਗਿਣਤੀ; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	ਜਦੋਂ (ifcl != ਨਿਰਾ)
	{
	    ਜੇ (update_member_ਵਿਧੀ_lookup_ਸਾਰਣੀ(ifcl, cl, objmethods_gap,
								0) == FAIL)
		ਵਾਪਸ FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    ਜੇ (extends_cl != ਨਿਰਾ)
    {
	class_T		*pclass = extends_cl;
	ਪੂਰਨ		pobj_ਵਿਧੀ_ਵਿੱਥ = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	ਜਦੋਂ (pclass != ਨਿਰਾ)
	{
	    ਜੇ (update_member_ਵਿਧੀ_lookup_ਸਾਰਣੀ(pclass, cl,
			objmethods_gap, pobj_ਵਿਧੀ_ਵਿੱਥ) == FAIL)
		ਵਾਪਸ FAIL;

	    pobj_ਵਿਧੀ_ਵਿੱਥ += pclass->class_obj_ਵਿਧੀ_ਗਿਣਤੀ_ਬੱਚਾ;
	    pclass = pclass->class_extends;
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_ਗਿਣਤੀ);
    ਜੇ (cl->class_members_tv == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_class_member_ਗਿਣਤੀ; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	ਜੇ (m->ocm_init != ਨਿਰਾ)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_ਵਿਅੰਜਕ(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    ਜੇ (etv == ਨਿਰਾ)
		ਵਾਪਸ FAIL;

	    ਜੇ (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && etv->v_type != VAR_SPECIAL)
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = typval2type(etv, get_copyID(),
			type_list_gap,
			TVTT_DO_MEMBER|TVTT_ਹੋਰ_SPECIFIC);
	    *tv = *etv;
	    vim_free(etv);
	}
	ਨਹੀਂਤਾਂ
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = ਨਿਰਾ;
	}
	ਸੈੱਟ_tv_type(tv, m->ocm_type);
	ਜੇ (m->ocm_flags & OCMFLAG_CONST)
	    ਵਸਤੂ_ਤਾਲਾ(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    ਵਾਪਸ OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    ਪੂਰਨ		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_obj_member_ਗਿਣਤੀ; ++i)
    {
	ਜੇ (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    ਜਾਰੀ;

	ਜੇ (i > (is_enum ? 2 : 0))
	    GA_CONCAT_LITERAL(&fga, ", ");
	GA_CONCAT_LITERAL(&fga, "this.");
	ocmember_T *m = cl->class_obj_members + i;
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.ਲੰਮਾਈ);
	GA_CONCAT_LITERAL(&fga, " = v:none");
    }
    GA_CONCAT_LITERAL(&fga, ")\nenddef\n");
    ga_append(&fga, NUL);

    exarg_T fea;
    CLEAR_ਖੇਤਰ(fea);
    fea.cmdidx = CMD_def;
    fea.cmd = fea.arg = fga.ga_ਅੰਕੜੇ;

    garray_T ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free;
    ga_init2(&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, ਆਕਾਰ(char_u *), 50);

    ufunc_T *nf = define_function(&fea, ਨਿਰਾ, &ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_ਗਿਣਤੀ,
			    ਨਿਰਾ);

    ga_clear_ਲੜੀਆਂ(&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free);
    vim_free(fga.ga_ਅੰਕੜੇ);

    ਜੇ (nf != ਨਿਰਾ && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_ਅੰਕੜੇ)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	ਜੇ (nf->uf_ret_type != ਨਿਰਾ)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_ਦਲੀਲਾਂ = ਨਿਰਾ;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    ਲਈ (ਪੂਰਨ loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	ਪੂਰਨ		*fcount = loop == 1 ? &cl->class_class_function_ਗਿਣਤੀ
						: &cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	ਪੂਰਨ ਮਾਪਾ_ਗਿਣਤੀ = 0;
	ਜੇ (extends_cl != ਨਿਰਾ)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    ਮਾਪਾ_ਗਿਣਤੀ = loop == 1
				? 0
				: extends_cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ;

	*fcount = ਮਾਪਾ_ਗਿਣਤੀ + gap->ga_len;
	ਜੇ (*fcount == 0)
	{
	    *fup = ਨਿਰਾ;
	    ਜਾਰੀ;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	ਜੇ (*fup == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਜੇ (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_ਅੰਕੜੇ, ਆਕਾਰ(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_ਅੰਕੜੇ);
	ਜੇ (loop == 1)
	    cl->class_class_function_ਗਿਣਤੀ_ਬੱਚਾ = gap->ga_len;
	ਨਹੀਂਤਾਂ
	    cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ_ਬੱਚਾ = gap->ga_len;

	ਜੇ (loop == 2)
	{
	    // Copy instance methods from the parent.

	    ਲਈ (ਪੂਰਨ i = 0; i < ਮਾਪਾ_ਗਿਣਤੀ; ++i)
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
		ਜੇ (nf != ਨਿਰਾ && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_ਦਲੀਲਾਂ != ਨਿਰਾ)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		ਲਈ (ਪੂਰਨ ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    ਜੇ (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (ਅੱਖਰ *)pname;
			where.wt_ਕਿਸਮ = WT_ਵਿਧੀ;
			(ਖਾਲੀ)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	ਲਈ (ਪੂਰਨ i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    ਜੇ (i < gap->ga_len)
		fp->uf_defclass = cl;
	    ਜੇ (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    ਵਾਪਸ OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    ਸਥਿਰ ਖਾਲੀ
update_builtin_ਵਿਧੀ_ਸੂਚਕਾਂਕ(class_T *cl)
{
    ਪੂਰਨ	i;

    ਲਈ (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    ਲਈ (i = 0; i < cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	ਜੇ (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	ਨਹੀਂਤਾਂ ਜੇ (cl->class_builtin_methods[CLASS_BUILTIN_ਖਾਲੀ_ਥਾਵਾਂ] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_ਖਾਲੀ_ਥਾਵਾਂ] = i;
	ਨਹੀਂਤਾਂ ਜੇ (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_class_name_ਸਿਰਾ(char_u *arg)
{
    char_u *ਸਿਰਾ = arg;

    ਜਦੋਂ (ASCII_ISALNUM(*ਸਿਰਾ) || *ਸਿਰਾ == '_'
	    || (*ਸਿਰਾ == '.' && (ASCII_ISALNUM(ਸਿਰਾ[1]) || ਸਿਰਾ[1] == '_')))
	++ਸਿਰਾ;

    ਵਾਪਸ ਸਿਰਾ;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    ਸਥਿਰ ਪੂਰਨ
is_duplicate_enum(
    garray_T	*enum_gap,
    char_u	*varname,
    char_u	*varname_ਸਿਰਾ)
{
    string_T	name = {varname, (size_t)(varname_ਸਿਰਾ - varname)};  // Note: the .string field may
								    // point to a string longer
								    // than the .length field.
								    // So we need to use STRNCMP()
								    // to compare it.
    ਪੂਰਨ		dup = FALSE;

    ਲਈ (ਪੂਰਨ i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_ਅੰਕੜੇ) + i;

	ਜੇ (name.ਲੰਮਾਈ == m->ocm_name.ਲੰਮਾਈ
	    && STRNCMP(name.string, m->ocm_name.string, name.ਲੰਮਾਈ) == 0)
	{
	    char_u  save_c = *varname_ਸਿਰਾ;

	    *varname_ਸਿਰਾ = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_ਸਿਰਾ = save_c;
	    dup = TRUE;
	    ਰੋਕੋ;
	}
    }

    ਵਾਪਸ dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    ਸਥਿਰ ਪੂਰਨ
enum_parse_ਮੁੱਲਾਂ(
    exarg_T	*eap,
    class_T	*en,
    char_u	*ਸਤਰ,
    garray_T	*gap,
    ਪੂਰਨ		*ਅੰਕ_enum_ਮੁੱਲਾਂ,
    ਪੂਰਨ		*enum_ਸਿਰਾ)
{
    evalarg_T	evalarg;
    char_u	*p = ਸਤਰ;
    ਅੱਖਰ	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	ਆਖਰੀ_char = NUL;
    ਪੂਰਨ		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    ਪੂਰਨ		did_emsg_before = did_emsg;
    ਜਦੋਂ (*p != NUL)
    {
	// ignore comment
	ਜੇ (*p == '#')
	    ਰੋਕੋ;

	ਜੇ (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_ਮੁੱਲ_declaration_str), p);
	    ਰੋਕੋ;
	}

	char_u *eni_name_ਸ਼ੁਰੂ = p;
	char_u *eni_name_ਸਿਰਾ = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(p, FALSE);

	ਜੇ (is_duplicate_enum(gap, eni_name_ਸ਼ੁਰੂ, eni_name_ਸਿਰਾ))
	    ਰੋਕੋ;

	p = skipwhite(eni_name_ਸਿਰਾ);

	char_u	*init_ਵਿਅੰਜਕ = ਨਿਰਾ;
	ਜੇ (*p == '(' || *p == '<')
	{
	    ਜੇ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", ਸਤਰ);
		ਰੋਕੋ;
	    }

	    char_u *ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, *ਵਿਅੰਜਕ_ਸਿਰਾ;

	    p = eni_name_ਸ਼ੁਰੂ;
	    (ਖਾਲੀ)skip_ਵਿਅੰਜਕ_concatenate(&p, &ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, &ਵਿਅੰਜਕ_ਸਿਰਾ, &evalarg);

	    ਜਦੋਂ (*ਵਿਅੰਜਕ_ਸ਼ੁਰੂ && *ਵਿਅੰਜਕ_ਸ਼ੁਰੂ != '(' && *ਵਿਅੰਜਕ_ਸ਼ੁਰੂ != '<')
		ਵਿਅੰਜਕ_ਸ਼ੁਰੂ++;

	    ਜੇ (ਵਿਅੰਜਕ_ਸਿਰਾ > ਵਿਅੰਜਕ_ਸ਼ੁਰੂ)
		init_ਵਿਅੰਜਕ = vim_strnsave(ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, ਵਿਅੰਜਕ_ਸਿਰਾ - ਵਿਅੰਜਕ_ਸ਼ੁਰੂ);
	}

	ਜੇ (init_ਵਿਅੰਜਕ == ਨਿਰਾ)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, ਆਕਾਰ(initexpr_buf), "%s.new()", en->class_name.string);
	ਨਹੀਂਤਾਂ
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, ਆਕਾਰ(initexpr_buf), "%s.new%s", en->class_name.string,
		init_ਵਿਅੰਜਕ);
	    vim_free(init_ਵਿਅੰਜਕ);
	}
	ਜੇ (ਜੋੜੋ_member(gap, eni_name_ਸ਼ੁਰੂ, eni_name_ਸਿਰਾ, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    ਰੋਕੋ;

	++*ਅੰਕ_enum_ਮੁੱਲਾਂ;

	ਜੇ (*p != '#')
	    ਆਖਰੀ_char = *p;

	ਜੇ (*p != NUL && *p != ',')
	    ਰੋਕੋ;

	ਜੇ (*p == ',')
	{
	    ਜੇ (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", ਸਤਰ);
		ਰੋਕੋ;
	    }
	    ਜੇ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", ਸਤਰ);
		ਰੋਕੋ;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    ਜੇ (*p != NUL && *p != '#')
    {
	ਜੇ (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_ਦਲੀਲ_str), p);
	rc = FAIL;
    }

    ਜੇ (ਆਖਰੀ_char != ',')
	// last enum value should not be terminated by ","
	*enum_ਸਿਰਾ = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, ਨਿਰਾ);

    ਵਾਪਸ rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    ਸਥਿਰ ਪੂਰਨ
enum_ਜੋੜੋ_ਮੁੱਲਾਂ_member(
    class_T	*en,
    garray_T	*gap,
    ਪੂਰਨ		ਅੰਕ_enum_ਮੁੱਲਾਂ,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    ਪੂਰਨ		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_enum_ਮੁੱਲਾਂ; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_ਅੰਕੜੇ) + i;

	ਜੇ (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.ਲੰਮਾਈ);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.ਲੰਮਾਈ);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    ਜੇ (type == ਨਿਰਾ)
	ਜਾਓ ਪੂਰਾ;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    ਜੇ (type->tt_member != ਨਿਰਾ)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = ਜੋੜੋ_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_ਅੰਕੜੇ, fga.ga_len));

ਪੂਰਾ:
    vim_free(fga.ga_ਅੰਕੜੇ);

    ਵਾਪਸ rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    ਸਥਿਰ ਖਾਲੀ
enum_clear_constructors(class_T *en)
{
    ਲਈ (ਪੂਰਨ i = 0; i < en->class_class_function_ਗਿਣਤੀ; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	ਜੇ (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    ਖਾਲੀ
enum_ਸੈੱਟ_ਅੰਦਰੂਨੀ_obj_vars(class_T *en, object_T *enval)
{
    ਪੂਰਨ	i;

    ਲਈ (i = 0; i < en->class_class_member_ਗਿਣਤੀ; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	ਜੇ (en_tv != ਨਿਰਾ && en_tv->v_type == VAR_UNKNOWN)
	    ਰੋਕੋ;
    }

    ਜੇ (i >= en->class_class_member_ਗਿਣਤੀ)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	ਵਾਪਸ;

    // First object variable is the name
    ocmember_T *ਮੁੱਲ_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(ਮੁੱਲ_ocm->ocm_name.string, ਮੁੱਲ_ocm->ocm_name.ਲੰਮਾਈ);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_ਸੰਖਿਆ;
    ord_tv->vval.v_ਸੰਖਿਆ = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    ਸਥਿਰ ਖਾਲੀ
skip_class_body(exarg_T *eap)
{
    ਲਈ (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	ਜੇ (theline == ਨਿਰਾ)
	    ਰੋਕੋ;

	char_u	*p = theline;
	ਪੂਰਨ	ਮਿਲਿਆ;

	// skip ':' and blanks
	ਲਈ (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	ਮਿਲਿਆ = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	ਜੇ (ਮਿਲਿਆ)
	    ਰੋਕੋ;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    ਖਾਲੀ
ex_class(exarg_T *eap)
{
    ਪੂਰਨ		is_class = eap->cmdidx == CMD_class;
    ਪੂਰਨ		is_abstract = eap->cmdidx == CMD_abstract;
    ਪੂਰਨ		is_enum = eap->cmdidx == CMD_enum;
    ਪੂਰਨ		added_enum_ਮੁੱਲਾਂ = FALSE;
    ਪੂਰਨ		is_interface;
    ਲੰਮਾ	ਸ਼ੁਰੂ_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    ਜੇ (eap->skip)
    {
	skip_class_body(eap);
	ਵਾਪਸ;
    }

    ਜੇ (is_abstract)
    {
	ਜੇ (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), arg);
	    ਵਾਪਸ;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    ਜੇ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	ਜੇ (is_class)
	    emsg(_(e_class_can_only_be_defined_ਵਿੱਚ_vim9_script));
	ਨਹੀਂਤਾਂ ਜੇ (is_enum)
	    emsg(_(e_enum_can_only_be_defined_ਵਿੱਚ_vim9_script));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_interface_can_only_be_defined_ਵਿੱਚ_vim9_script));
	ਵਾਪਸ;
    }

    ਜੇ (!ASCII_ISUPPER(*arg))
    {
	ਜੇ (is_class)
	    semsg(_(e_class_name_must_ਸ਼ੁਰੂ_with_uppercase_letter_str), arg);
	ਨਹੀਂਤਾਂ ਜੇ (is_enum)
	    semsg(_(e_enum_name_must_ਸ਼ੁਰੂ_with_uppercase_letter_str), arg);
	ਨਹੀਂਤਾਂ
	    semsg(_(e_interface_name_must_ਸ਼ੁਰੂ_with_uppercase_letter_str),
									  arg);
	ਵਾਪਸ;
    }
    char_u *name_ਸਿਰਾ = ਲੱਭੋ_name_ਸਿਰਾ(arg, ਨਿਰਾ, ਨਿਰਾ, FNE_CHECK_ਸ਼ੁਰੂ);
    ਜੇ (!IS_WHITE_OR_NUL(*name_ਸਿਰਾ))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	ਵਾਪਸ;
    }
    char_u *name_ਸ਼ੁਰੂ = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = ਨਿਰਾ;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, ਆਕਾਰ(char_u *), 5);

    arg = skipwhite(name_ਸਿਰਾ);
    ਜਦੋਂ (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	ਜੇ (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    ਜੇ (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		ਜਾਓ early_ret;
	    }
	    ਜੇ (extends != ਨਿਰਾ)
	    {
		emsg(_(e_duplicate_extends));
		ਜਾਓ early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *ਸਿਰਾ = ਲੱਭੋ_class_name_ਸਿਰਾ(arg);
	    ਜੇ (!IS_WHITE_OR_NUL(*ਸਿਰਾ))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		ਜਾਓ early_ret;
	    }
	    extends = vim_strnsave(arg, ਸਿਰਾ - arg);
	    ਜੇ (extends == ਨਿਰਾ)
		ਜਾਓ early_ret;

	    arg = skipwhite(ਸਿਰਾ + 1);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    ਜੇ (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		ਜਾਓ early_ret;
	    }

	    ਜੇ (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		ਜਾਓ early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    ਲਈ (;;)
	    {
		char_u *impl_ਸਿਰਾ = ਲੱਭੋ_class_name_ਸਿਰਾ(arg);
		ਜੇ ((!IS_WHITE_OR_NUL(*impl_ਸਿਰਾ) && *impl_ਸਿਰਾ != ',')
			|| (*impl_ਸਿਰਾ == ','
			    && !IS_WHITE_OR_NUL(*(impl_ਸਿਰਾ + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    ਜਾਓ early_ret;
		}
		ਜੇ (impl_ਸਿਰਾ - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    ਜਾਓ early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_ਸਿਰਾ - arg);
		ਜੇ (iname == ਨਿਰਾ)
		    ਜਾਓ early_ret;
		ਲਈ (ਪੂਰਨ i = 0; i < ga_impl.ga_len; ++i)
		    ਜੇ (STRCMP(((char_u **)ga_impl.ga_ਅੰਕੜੇ)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			ਜਾਓ early_ret;
		    }
		ਜੇ (ga_ਜੋੜੋ_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    ਜਾਓ early_ret;
		}
		ਜੇ (*impl_ਸਿਰਾ != ',')
		{
		    arg = skipwhite(impl_ਸਿਰਾ);
		    ਰੋਕੋ;
		}
		arg = skipwhite(impl_ਸਿਰਾ + 1);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_ਲੜੀਆਂ(&ga_impl);
	    ਵਾਪਸ;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, ਆਕਾਰ(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, ਆਕਾਰ(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, ਆਕਾਰ(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, ਆਕਾਰ(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, ਆਕਾਰ(ufunc_T *), 10);

    class_T *cl = ਨਿਰਾ;
    class_T *extends_cl = ਨਿਰਾ;  // class from "extends" argument
    class_T **intf_classes = ਨਿਰਾ;
    ਪੂਰਨ	    intf_ਗਿਣਤੀ = 0;	// number of items in "intf_classes"
    ਪੂਰਨ	    ਅੰਕ_enum_ਮੁੱਲਾਂ = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    ਜੇ (cl == ਨਿਰਾ)
	ਜਾਓ cleanup;

    ਜੇ (is_enum)
	cl->class_flags = CLASS_ENUM;
    ਨਹੀਂਤਾਂ ਜੇ (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    ਨਹੀਂਤਾਂ ਜੇ (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.ਲੰਮਾਈ = (size_t)(name_ਸਿਰਾ - name_ਸ਼ੁਰੂ);
    cl->class_name.string = vim_strnsave(name_ਸ਼ੁਰੂ, cl->class_name.ਲੰਮਾਈ);
    ਜੇ (cl->class_name.string == ਨਿਰਾ)
	ਜਾਓ cleanup;

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
    SOURCING_LNUM = ਸ਼ੁਰੂ_lnum;
    ਪੂਰਨ rc = ਸੈੱਟ_var_const(cl->class_name.string, 0, ਨਿਰਾ, &tv, FALSE, 0, 0);
    ਜੇ (rc == FAIL)
	ਜਾਓ cleanup;

    ਜੇ (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	ਜੇ (ਜੋੜੋ_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, ਨਿਰਾ) == FAIL)
	    ਜਾਓ cleanup;

	varname = (char_u *)"ordinal";
	ਜੇ (ਜੋੜੋ_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_ਸੰਖਿਆ, ਨਿਰਾ) == FAIL)
	    ਜਾਓ cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    ਪੂਰਨ		enum_ਸਿਰਾ = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = ਨਿਰਾ;
    ਪੂਰਨ success = FALSE;
    ਲਈ (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	ਜੇ (theline == ਨਿਰਾ)
	    ਰੋਕੋ;
	char_u *ਸਤਰ = skipwhite(theline);

	// Skip empty and comment lines.
	ਜੇ (*ਸਤਰ == NUL)
	    ਜਾਰੀ;
	ਜੇ (*ਸਤਰ == '#')
	{
	    ਜੇ (vim9_bad_comment(ਸਤਰ))
		ਰੋਕੋ;
	    ਜਾਰੀ;
	}

	char_u *p = ਸਤਰ;

	ਅੱਖਰ	*ਸਿਰਾ_name;
	ਪੂਰਨ	shortlen;
	ਪੂਰਨ	fullen;
	ਜੇ (is_class)
	{
	    ਸਿਰਾ_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	ਨਹੀਂਤਾਂ ਜੇ (is_enum)
	{
	    ਸਿਰਾ_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਸਿਰਾ_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	ਲਈ (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_ਸ਼ੁਰੂ = p;
	ਜੇ (checkforcmd(&p, ਸਿਰਾ_name, shortlen))
	{
	    ਜੇ (STRNCMP(cmd_ਸ਼ੁਰੂ, ਸਿਰਾ_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_ਸ਼ੁਰੂ);
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '|' || !ਸਿਰੇ_excmd2(ਸਤਰ, p))
		semsg(_(e_trailing_characters_str), p);
	    ਨਹੀਂਤਾਂ
		success = TRUE;
	    ਰੋਕੋ;
	}

	ਪੂਰਨ	wrong_endname = FALSE;
	ਜੇ (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	ਨਹੀਂਤਾਂ ਜੇ (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	ਨਹੀਂਤਾਂ
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	ਜੇ (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), ਸਤਰ, ਸਿਰਾ_name);
	    ਰੋਕੋ;
	}

	ਜੇ (is_enum && !enum_ਸਿਰਾ)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    ਜੇ (enum_parse_ਮੁੱਲਾਂ(eap, cl, ਸਤਰ, &classmembers,
					&ਅੰਕ_enum_ਮੁੱਲਾਂ, &enum_ਸਿਰਾ) == FAIL)
		ਰੋਕੋ;

	    ਜੇ (enum_ਸਿਰਾ)
	    {
		// Add the enum "values" class variable.
		enum_ਜੋੜੋ_ਮੁੱਲਾਂ_member(cl, &classmembers, ਅੰਕ_enum_ਮੁੱਲਾਂ,
							&type_list);
		added_enum_ਮੁੱਲਾਂ = TRUE;
	    }
	    ਜਾਰੀ;
	}

	ਪੂਰਨ has_ਜਨਤਕ = FALSE;
	ਜੇ (checkforcmd(&p, "public", 3))
	{
	    ਜੇ (STRNCMP(ਸਤਰ, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ਸਤਰ);
		ਰੋਕੋ;
	    }
	    ਜੇ (is_interface)
	    {
		emsg(_(e_ਜਨਤਕ_variable_not_supported_ਵਿੱਚ_interface));
		ਰੋਕੋ;
	    }
	    has_ਜਨਤਕ = TRUE;
	    p = skipwhite(ਸਤਰ + 6);

	    ਜੇ (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_ਜਨਤਕ_keyword_not_supported_for_ਵਿਧੀ));
		ਰੋਕੋ;
	    }

	    ਜੇ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_ਜਨਤਕ_must_be_followed_by_var_static_final_or_const));
		ਰੋਕੋ;
	    }
	}

	ਪੂਰਨ abstract_ਵਿਧੀ = FALSE;
	char_u *pa = p;
	ਜੇ (checkforcmd(&p, "abstract", 3))
	{
	    ਜੇ (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		ਰੋਕੋ;
	    }

	    ਜੇ (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_ਵਰਤਿਆ_ਵਿੱਚ_enum));
		ਰੋਕੋ;
	    }

	    ਜੇ (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_ਵਰਤਿਆ_ਵਿੱਚ_interface));
		ਰੋਕੋ;
	    }

	    ਜੇ (!is_abstract)
	    {
		semsg(_(e_abstract_ਵਿਧੀ_ਵਿੱਚ_concrete_class), pa);
		ਰੋਕੋ;
	    }

	    p = skipwhite(pa + 8);
	    ਜੇ (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		ਰੋਕੋ;
	    }

	    abstract_ਵਿਧੀ = TRUE;
	}

	ਪੂਰਨ has_static = FALSE;
	char_u *ps = p;
	ਜੇ (checkforcmd(&p, "static", 4))
	{
	    ਜੇ (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		ਰੋਕੋ;
	    }

	    ਜੇ (is_interface)
	    {
		emsg(_(e_static_member_not_supported_ਵਿੱਚ_interface));
		ਰੋਕੋ;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    ਜੇ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		ਰੋਕੋ;
	    }
	}

	ਪੂਰਨ has_final = FALSE;
	ਪੂਰਨ has_var = FALSE;
	ਪੂਰਨ has_const = FALSE;
	ਜੇ (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (checkforcmd(&p, "final", 5))
	{
	    ਜੇ (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_ਵਿੱਚ_interface));
		ਰੋਕੋ;
	    }
	    has_final = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (checkforcmd(&p, "const", 5))
	{
	    ਜੇ (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_ਵਿੱਚ_interface));
		ਰੋਕੋ;
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
	ਜੇ (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_ਸਿਰਾ = ਨਿਰਾ;
	    type_T *type = ਨਿਰਾ;
	    char_u *init_ਵਿਅੰਜਕ = ਨਿਰਾ;
	    ਪੂਰਨ	    has_type = FALSE;

	    ਜੇ (!eval_isnamec1(*p))
	    {
		ਜੇ (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), ਸਤਰ);
		ਨਹੀਂਤਾਂ
		    semsg(_(e_invalid_object_variable_declaration_str), ਸਤਰ);
		ਰੋਕੋ;
	    }

	    ਜੇ (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_ਵਿੱਚ_interface),
			varname);
		ਰੋਕੋ;
	    }

	    ਜੇ (parse_member(eap, ਸਤਰ, varname, has_ਜਨਤਕ,
			  &varname_ਸਿਰਾ, &has_type, &type_list, &type,
			  !is_interface ? &init_ਵਿਅੰਜਕ: ਨਿਰਾ) == FAIL)
		ਰੋਕੋ;

	    ਜੇ (is_reserved_varname(varname, varname_ਸਿਰਾ)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_ਸਿਰਾ))
	    {
		vim_free(init_ਵਿਅੰਜਕ);
		ਰੋਕੋ;
	    }
	    ਜੇ (ਜੋੜੋ_member(has_static ? &classmembers : &objmembers, varname,
				varname_ਸਿਰਾ, has_ਜਨਤਕ, has_final, has_const,
				has_type, type, init_ਵਿਅੰਜਕ) == FAIL)
	    {
		vim_free(init_ਵਿਅੰਜਕ);
		ਰੋਕੋ;
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
	ਨਹੀਂਤਾਂ ਜੇ (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free;
	    ਪੂਰਨ		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    ਜੇ (has_ਜਨਤਕ)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_ਜਨਤਕ_keyword_not_supported_for_ਵਿਧੀ));
		ਰੋਕੋ;
	    }

	    ਜੇ (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_ਵਿੱਚ_class_str), ਸਤਰ);
		ਰੋਕੋ;
	    }

	    ਜੇ (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_ਵਿਧੀ_not_supported_ਵਿੱਚ_interface), p);
		ਰੋਕੋ;
	    }

	    ਜੇ (has_static && !is_new && ਸੁਰੱਖਿਅਤ_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_ਵਿਧੀ_not_supported), p);
		ਰੋਕੋ;
	    }

	    CLEAR_ਖੇਤਰ(ea);
	    ea.cmd = ਸਤਰ;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, ਆਕਾਰ(char_u *), 50);
	    ਪੂਰਨ class_flags;
	    ਜੇ (is_interface)
		class_flags = CF_INTERFACE;
	    ਨਹੀਂਤਾਂ
		class_flags = abstract_ਵਿਧੀ ? CF_ABSTRACT_ਵਿਧੀ : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, ਨਿਰਾ, &ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free,
			class_flags, objmembers.ga_ਅੰਕੜੇ, objmembers.ga_len,
			ਨਿਰਾ);
	    ga_clear_ਲੜੀਆਂ(&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free);

	    ਜੇ (uf != ਨਿਰਾ)
	    {
		ਜੇ (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_ਵਿਧੀ_not_supported_ਵਿੱਚ_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    ਰੋਕੋ;
		}

		// check for builtin method
		ਜੇ (!is_new && ਸੁਰੱਖਿਅਤ_islower(*uf->uf_name) &&
					!object_check_builtin_ਵਿਧੀ_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    ਰੋਕੋ;
		}

		// Check the uf_name isn't used already.
		ਜੇ (is_duplicate_ਵਿਧੀ(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    ਰੋਕੋ;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		ਜੇ (ga_grow(fgap, 1) == OK)
		{
		    ਜੇ (is_new)
			uf->uf_flags |= FC_NEW;

		    ਜੇ (abstract_ਵਿਧੀ)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_ਅੰਕੜੇ)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (is_class)
		semsg(_(e_not_valid_command_ਵਿੱਚ_class_str), ਸਤਰ);
	    ਨਹੀਂਤਾਂ ਜੇ (is_enum)
		semsg(_(e_not_valid_command_ਵਿੱਚ_enum_str), ਸਤਰ);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_not_valid_command_ਵਿੱਚ_interface_str), ਸਤਰ);
	    ਰੋਕੋ;
	}
    }

    ਜੇ (theline == ਨਿਰਾ && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    ਜੇ (success && is_enum && (ਅੰਕ_enum_ਮੁੱਲਾਂ == 0 || !added_enum_ਮੁੱਲਾਂ))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_ਜੋੜੋ_ਮੁੱਲਾਂ_member(cl, &classmembers, ਅੰਕ_enum_ਮੁੱਲਾਂ,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    ਜੇ (success && extends != ਨਿਰਾ)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    ਜੇ (success && extends_cl != ਨਿਰਾ)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    ਜੇ (success && extends_cl != ਨਿਰਾ)
    {
	ਜੇ (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	ਨਹੀਂਤਾਂ
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    ਜੇ (success && !is_abstract && extends_cl != ਨਿਰਾ
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, ਆਕਾਰ(class_T *), 5);

    ਜੇ (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    ਜੇ (success && extends_cl != ਨਿਰਾ)
	success = ਜੋੜੋ_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_ਅੰਕੜੇ;
    intf_ਗਿਣਤੀ = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    ਜੇ (success)
	success = check_func_arg_ਨਾਂ(&classfunctions, &objmethods,
							&classmembers);

    ਜੇ (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	ਜੇ (extends_cl != ਨਿਰਾ)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	ਜੇ (ਜੋੜੋ_members_ਮੰਜ਼ਿਲ_class(&classmembers,
				 ਨਿਰਾ,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_ਗਿਣਤੀ) == FAIL
		|| ਜੋੜੋ_members_ਮੰਜ਼ਿਲ_class(&objmembers,
				 extends_cl == ਨਿਰਾ ? ਨਿਰਾ
					       : extends_cl->class_obj_members,
				 extends_cl == ਨਿਰਾ ? 0
					  : extends_cl->class_obj_member_ਗਿਣਤੀ,
				 &cl->class_obj_members,
				 &cl->class_obj_member_ਗਿਣਤੀ) == FAIL)
	    ਜਾਓ cleanup;

	ਜੇ (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_ਗਿਣਤੀ = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    ਜੇ (cl->class_interfaces == ਨਿਰਾ)
		ਜਾਓ cleanup;
	    ਲਈ (ਪੂਰਨ i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_ਅੰਕੜੇ)[i];
	    VIM_CLEAR(ga_impl.ga_ਅੰਕੜੇ);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = ਨਿਰਾ;
	}

	ਜੇ (cl->class_interface_ਗਿਣਤੀ > 0 || extends_cl != ਨਿਰਾ)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    ਜੇ (ਜੋੜੋ_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		ਜਾਓ cleanup;
	}

	ਪੂਰਨ	have_new = FALSE;
	ufunc_T	*class_func = ਨਿਰਾ;
	ਲਈ (ਪੂਰਨ i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_ਅੰਕੜੇ)[i];
	    ਜੇ (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		ਰੋਕੋ;
	    }
	}

	ਜੇ (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	ਨਹੀਂਤਾਂ ਜੇ ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    ਜੋੜੋ_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	ਜੇ (ਜੋੜੋ_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    ਜਾਓ cleanup;

	update_builtin_ਵਿਧੀ_ਸੂਚਕਾਂਕ(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	ਜੇ ((is_class || is_enum) && cl->class_class_member_ਗਿਣਤੀ > 0)
	    ਜੇ (ਜੋੜੋ_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		ਵਾਪਸ;
	    }

	cl->class_type_list = type_list;

	ਜੇ (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	ਵਾਪਸ;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    ਜੇ (intf_classes != ਨਿਰਾ)
    {
	ਲਈ (ਪੂਰਨ i = 0; i < intf_ਗਿਣਤੀ; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_ਲੜੀਆਂ(&ga_impl);

    ਲਈ (ਪੂਰਨ ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	garray_T *gap = ਗੇੜਾ == 1 ? &classmembers : &objmembers;
	ਜੇ (gap->ga_len == 0 || gap->ga_ਅੰਕੜੇ == ਨਿਰਾ)
	    ਜਾਰੀ;

	ਲਈ (ਪੂਰਨ i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_ਅੰਕੜੇ) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    ਲਈ (ਪੂਰਨ i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_ਅੰਕੜੇ)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    ਲਈ (ਪੂਰਨ i = 0; i < classfunctions.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)classfunctions.ga_ਅੰਕੜੇ)[i];
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
    ਪੂਰਨ		is_object,
    char_u	*name,
    char_u	*name_ਸਿਰਾ,
    ਪੂਰਨ		*member_idx)
{
    size_t	len = name_ਸਿਰਾ - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    ਜੇ (m == ਨਿਰਾ)
    {
	member_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	ਵਾਪਸ &t_any;
    }

    ਵਾਪਸ m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    ਪੂਰਨ		is_object,
    ਪੂਰਨ		member_idx)
{
    ocmember_T	*m;
    ਪੂਰਨ		member_ਗਿਣਤੀ;

    ਜੇ (is_object)
    {
	m = cl->class_obj_members;
	member_ਗਿਣਤੀ = cl->class_obj_member_ਗਿਣਤੀ;
    }
    ਨਹੀਂਤਾਂ
    {
	m = cl->class_class_members;
	member_ਗਿਣਤੀ = cl->class_class_member_ਗਿਣਤੀ;
    }

    ਜੇ (member_idx >= member_ਗਿਣਤੀ)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    ਸਥਿਰ ਖਾਲੀ
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    ਖਾਲੀ
typealias_unref(typealias_T *ta)
{
    ਜੇ (ta != ਨਿਰਾ && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    ਖਾਲੀ
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    ਜੇ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_ਵਿੱਚ_vim9_script));
	ਵਾਪਸ;
    }

    ਜੇ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	ਵਾਪਸ;
    }

    ਜੇ (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_ਸ਼ੁਰੂ_with_uppercase_letter_str), arg);
	ਵਾਪਸ;
    }

    char_u *name_ਸਿਰਾ = ਲੱਭੋ_name_ਸਿਰਾ(arg, ਨਿਰਾ, ਨਿਰਾ, FNE_CHECK_ਸ਼ੁਰੂ);
    ਜੇ (!IS_WHITE_OR_NUL(*name_ਸਿਰਾ))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	ਵਾਪਸ;
    }
    char_u *name_ਸ਼ੁਰੂ = arg;

    arg = skipwhite(name_ਸਿਰਾ);
    ਜੇ (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	ਵਾਪਸ;
    }
    ਜੇ (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	ਵਾਪਸ;
    }
    arg++;
    arg = skipwhite(arg);

    ਜੇ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	ਵਾਪਸ;
    }

    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, ਨਿਰਾ, ਨਿਰਾ, TRUE);
    ਜੇ (type == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	ਵਾਪਸ;
    }

    ਪੂਰਨ cc = *name_ਸਿਰਾ;
    *name_ਸਿਰਾ = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    ਜੇ (eval_variable_import(name_ਸ਼ੁਰੂ, &tv) == OK)
    {
	ਜੇ (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_ਸ਼ੁਰੂ);
	ਨਹੀਂਤਾਂ
	    semsg(_(e_redefining_script_ਵਸਤੂ_str), name_ਸ਼ੁਰੂ);
	clear_tv(&tv);
	ਜਾਓ ਪੂਰਾ;
    }

    // Create a script-local variable for the type alias.
    ਜੇ (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_ਤਾਲਾ = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_ਸ਼ੁਰੂ);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_ਡੂੰਘਾਈ = parse_type_ਡੂੰਘਾਈ();
    }
    ਨਹੀਂਤਾਂ
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_ਤਾਲਾ = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    ਸੈੱਟ_var_const(name_ਸ਼ੁਰੂ, current_sctx.sc_sid, ਨਿਰਾ, &tv, FALSE,
						ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL, 0);

ਪੂਰਾ:
    *name_ਸਿਰਾ = cc;
}

/*
 * Returns OK if a member variable named "name" is present in the class "cl".
 * Otherwise returns FAIL.  If found, the member variable typval is set in
 * "rettv".  If "is_object" is TRUE, then the object member variable table is
 * searched.  Otherwise the class member variable table is searched.
 */
    ਪੂਰਨ
get_member_tv(
    class_T	*cl,
    ਪੂਰਨ		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    ਪੂਰਨ		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    ਜੇ (m == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (*name == '_' && (current_class == ਨਿਰਾ ||
				!class_instance_ਦਾ(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	ਸੈੱਟ_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    ਨਹੀਂਤਾਂ
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	ਸੈੱਟ_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    ਵਾਪਸ OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    ਸਥਿਰ ਪੂਰਨ
call_oc_ਵਿਧੀ(
    class_T	*cl,
    char_u	*name,
    size_t	len,
    char_u	*name_ਸਿਰਾ,
    evalarg_T	*evalarg,
    char_u	**arg,
    gfargs_tab_T	*gfatab,	// generic types
    typval_T	*rettv)
{
    ufunc_T	*fp;
    typval_T	argvars[MAX_FUNC_ਦਲੀਲਾਂ + 1];
    ਪੂਰਨ		argcount = 0;
    ocmember_T	*ocm = ਨਿਰਾ;
    ਪੂਰਨ		m_idx;

    fp = ਵਿਧੀ_lookup(cl, rettv->v_type, name, len, ਨਿਰਾ);
    ਜੇ (fp == ਨਿਰਾ)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	ਜੇ (ocm == ਨਿਰਾ || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    ਵਿਧੀ_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(cl, rettv->v_type, name, len);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	ਨਹੀਂਤਾਂ
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_ਸਿਰਾ;
	ਵਾਪਸ OK;
    }

    ਜੇ (ocm == ਨਿਰਾ && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_ਵਿਧੀ_str), fp->uf_name);
	ਵਾਪਸ FAIL;
    }

    // process generic function call
    ਜੇ (fp != ਨਿਰਾ)
    {
	fp = generic_func_get(fp, gfatab);
	ਜੇ (fp == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }

    char_u *argp = name_ਸਿਰਾ;
    ਪੂਰਨ ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    ਜੇ (ret == FAIL)
    {
	ਜਦੋਂ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ਵਾਪਸ FAIL;
    }

    funcexe_T funcexe;
    CLEAR_ਖੇਤਰ(funcexe);
    funcexe.fe_evaluate = TRUE;
    ਜੇ (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    ਜੇ (evalarg != ਨਿਰਾ)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    ਪੂਰਨ error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								ਨਿਰਾ);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    ਲਈ (ਪੂਰਨ idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ਜੇ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_ਮਿਲਿਆ_var);
	ਵਾਪਸ FAIL;
    }
    *arg = argp;

    ਵਾਪਸ OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ਪੂਰਨ
obj_ਵਿਧੀ_ਮੰਜ਼ਿਲ_partial_tv(object_T *obj, ufunc_T *obj_ਵਿਧੀ, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    ਜੇ (pt == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    pt->pt_refcount = 1;
    ਜੇ (obj != ਨਿਰਾ)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_ਵਿਧੀ;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    ਵਾਪਸ OK;
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
    ਪੂਰਨ
class_object_ਸੂਚਕਾਂਕ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ਪੂਰਨ		verbose ਅਣਵਰਤਿਆ)	// give error messages
{
    ਪੂਰਨ	ret = FAIL;

    ਜੇ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ਵਾਪਸ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_ਸਿਰਾ = ਲੱਭੋ_name_ਸਿਰਾ(name, ਨਿਰਾ, ਨਿਰਾ, FNE_CHECK_ਸ਼ੁਰੂ);
    ਜੇ (name_ਸਿਰਾ == name)
	ਵਾਪਸ FAIL;
    size_t len = name_ਸਿਰਾ - name;

    ਪੂਰਨ did_emsg_save = did_emsg;
    class_T *cl;
    ਜੇ (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    ਨਹੀਂਤਾਂ // VAR_OBJECT
    {
	ਜੇ (rettv->vval.v_object == ਨਿਰਾ)
	{
	    emsg(_(e_using_null_object));
	    ਵਾਪਸ FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    ਜੇ (cl == ਨਿਰਾ)
    {
	emsg(_(e_incomplete_type));
	ਵਾਪਸ FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_init(&gfatab);

    ਜੇ (*name_ਸਿਰਾ == '<')
    {
	cctx_T	    *cctx = ਨਿਰਾ;

	ਜੇ (evalarg != ਨਿਰਾ)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_ਸਿਰਾ = parse_generic_func_type_ਦਲੀਲਾਂ(name, len, name + len,
						&gfatab, cctx);
	ਜੇ (name_ਸਿਰਾ == ਨਿਰਾ)
	    ਜਾਓ ਪੂਰਾ;
    }

    ਜੇ (*name_ਸਿਰਾ == '(')
	// Invoke the class or object method
	ret = call_oc_ਵਿਧੀ(cl, name, len, name_ਸਿਰਾ, evalarg, arg,
							&gfatab, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	ਪੂਰਨ is_object = rettv->v_type == VAR_OBJECT;
	ਜੇ (get_member_tv(cl, is_object, name, len, ਨਿਰਾ, rettv) == OK)
	{
	    *arg = name_ਸਿਰਾ;
	    ret = OK;
	    ਜਾਓ ਪੂਰਾ;
	}

	// could be a class method or an object method
	ਪੂਰਨ	fidx;
	ufunc_T	*fp = ਵਿਧੀ_lookup(cl, rettv->v_type, name, len, &fidx);
	ਜੇ (fp != ਨਿਰਾ)
	{
	    // Protected methods are not accessible outside the class
	    ਜੇ (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_ਵਿਧੀ_str), fp->uf_name);
		ਜਾਓ ਪੂਰਾ;
	    }

	    ਜੇ (obj_ਵਿਧੀ_ਮੰਜ਼ਿਲ_partial_tv(is_object ? rettv->vval.v_object :
						ਨਿਰਾ, fp, rettv) == FAIL)
		ਜਾਓ ਪੂਰਾ;

	    *arg = name_ਸਿਰਾ;
	    ret = OK;
	    ਜਾਓ ਪੂਰਾ;
	}

	ਜੇ (did_emsg == did_emsg_save)
	    member_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(cl, rettv->v_type, name, len);
    }

ਪੂਰਾ:
    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_clear(&gfatab);

    ਵਾਪਸ ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
ਲੱਭੋ_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_ਸਿਰਾ = ਲੱਭੋ_name_ਸਿਰਾ(name, ਨਿਰਾ, ਨਿਰਾ, FNE_CHECK_ਸ਼ੁਰੂ);
    ਜੇ (name_ਸਿਰਾ == name || *name_ਸਿਰਾ != '.')
	ਵਾਪਸ ਨਿਰਾ;

    ufunc_T	*fp = ਨਿਰਾ;
    size_t	len = name_ਸਿਰਾ - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    ਜੇ (eval_variable(name, (ਪੂਰਨ)len,
				    0, &tv, ਨਿਰਾ, EVAL_VAR_NOAUTOLOAD) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	ਜਾਓ fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    ਜੇ (cl == ਨਿਰਾ)
	ਜਾਓ fail_after_eval;
    char_u *fname = name_ਸਿਰਾ + 1;
    char_u *fname_ਸਿਰਾ = ਲੱਭੋ_name_ਸਿਰਾ(fname, ਨਿਰਾ, ਨਿਰਾ, FNE_CHECK_ਸ਼ੁਰੂ);
    ਜੇ (fname_ਸਿਰਾ == fname)
	ਜਾਓ fail_after_eval;
    len = fname_ਸਿਰਾ - fname;

    fp = ਵਿਧੀ_lookup(cl, tv.v_type, fname, len, ਨਿਰਾ);

    ਜੇ (fp != ਨਿਰਾ)
    {
	fp = eval_generic_func(fp, fname, &fname_ਸਿਰਾ);
	ਜੇ (fp != ਨਿਰਾ)
	    *arg = fname_ਸਿਰਾ;
    }

fail_after_eval:
    clear_tv(&tv);
    ਵਾਪਸ fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ਪੂਰਨ
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    ਪੂਰਨ idx;
    class_member_lookup(cl, name, namelen, &idx);
    ਵਾਪਸ idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, ਪੂਰਨ *idx)
{
    ocmember_T	*ret_m = ਨਿਰਾ;
    ਪੂਰਨ		ret_idx = -1;
    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_class_member_ਗਿਣਤੀ; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	ਜੇ (namelen)
	{
	    ਜੇ (namelen == m->ocm_name.ਲੰਮਾਈ
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ਰੋਕੋ;
	}
    }
    ਜੇ (idx != ਨਿਰਾ)
	*idx = ret_idx;
    ਵਾਪਸ ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    ਸਥਿਰ ufunc_T *
class_ਵਿਧੀ_lookup(class_T *cl, char_u *name, size_t namelen, ਪੂਰਨ *idx)
{
    ufunc_T	*ret_fp = ਨਿਰਾ;
    ਪੂਰਨ		ret_idx = -1;
    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_class_function_ਗਿਣਤੀ; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	ਜੇ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ਰੋਕੋ;
	}
    }
    ਜੇ (idx != ਨਿਰਾ)
	*idx = ret_idx;
    ਵਾਪਸ ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    ਪੂਰਨ
class_ਵਿਧੀ_idx(class_T *cl, char_u *name, size_t namelen)
{
    ਪੂਰਨ idx;
    class_ਵਿਧੀ_lookup(cl, name, namelen, &idx);
    ਵਾਪਸ idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ਸਥਿਰ ਪੂਰਨ
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    ਪੂਰਨ idx;
    object_member_lookup(cl, name, namelen, &idx);
    ਵਾਪਸ idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, ਪੂਰਨ *idx)
{
    ocmember_T	*ret_m = ਨਿਰਾ;
    ਪੂਰਨ		ret_idx = -1;
    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_obj_member_ਗਿਣਤੀ; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	ਜੇ (namelen)
	{
	    ਜੇ (namelen == m->ocm_name.ਲੰਮਾਈ
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ਰੋਕੋ;
	}
    }
    ਜੇ (idx != ਨਿਰਾ)
	*idx = ret_idx;
    ਵਾਪਸ ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    ਸਥਿਰ ufunc_T *
object_ਵਿਧੀ_lookup(class_T *cl, char_u *name, size_t namelen, ਪੂਰਨ *idx)
{
    ufunc_T	*ret_fp = ਨਿਰਾ;
    ਪੂਰਨ		ret_idx = -1;
    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	ਜੇ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ਰੋਕੋ;
	}
    }
    ਜੇ (idx != ਨਿਰਾ)
	*idx = ret_idx;
    ਵਾਪਸ ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    ਪੂਰਨ
object_ਵਿਧੀ_idx(class_T *cl, char_u *name, size_t namelen)
{
    ਪੂਰਨ idx;
    object_ਵਿਧੀ_lookup(cl, name, namelen, &idx);
    ਵਾਪਸ idx;
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
    ਪੂਰਨ		*idx)
{
    ਜੇ (v_type == VAR_CLASS)
	ਵਾਪਸ class_member_lookup(cl, name, namelen, idx);

    ਵਾਪਸ object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    ਸਥਿਰ class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_ਮਿਲਿਆ = ਨਿਰਾ;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_ਮਿਲਿਆ = ਨਿਰਾ;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    ਲਈ (class_T *super = cl; super != ਨਿਰਾ; super = super->class_extends)
    {
	class_T		*cl_tmp = ਨਿਰਾ;
	ocmember_T	*m = ਨਿਰਾ;
	ਜੇ (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    ਜੇ ((m = object_member_lookup(super, name, len, ਨਿਰਾ)) != ਨਿਰਾ)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	ਜੇ (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    ਜੇ (( m = class_member_lookup(super, name, len, ਨਿਰਾ)) != ਨਿਰਾ)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	ਜੇ (cl_tmp == ਨਿਰਾ)
	    ਰੋਕੋ;  // member is not in this or any super class.
	cl_ਮਿਲਿਆ = cl_tmp;
	m_ਮਿਲਿਆ = m;
    }
    ਜੇ (p_m != ਨਿਰਾ)
	*p_m = m_ਮਿਲਿਆ;
    ਵਾਪਸ cl_ਮਿਲਿਆ;
}

/*
 * Lookup a class or object method by name.  If v_type is VAR_CLASS, then
 * lookup a class method and if it is VAR_OBJECT, then lookup a object method.
 *
 * Returns a pointer to the method structure if variable is found.
 * Otherwise returns NULL.  The method variable index is set in "*idx".
 */
    ufunc_T *
ਵਿਧੀ_lookup(
    class_T	*cl,
    vartype_T	v_type,
    char_u	*name,
    size_t	namelen,
    ਪੂਰਨ		*idx)
{
    ਜੇ (v_type == VAR_CLASS)
	ਵਾਪਸ class_ਵਿਧੀ_lookup(cl, name, namelen, idx);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ object_ਵਿਧੀ_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    ਪੂਰਨ
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    ਲਈ (cctx_T *cctx = cctx_arg; cctx != ਨਿਰਾ; cctx = cctx->ctx_outer)
	ਜੇ (cctx->ctx_ufunc != ਨਿਰਾ
			&& class_instance_ਦਾ(cctx->ctx_ufunc->uf_class, cl))
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    ਪੂਰਨ
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    ਜੇ (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_ਵਿੱਚ_class_str),
		m->ocm_name.string, cl->class_name.string);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    ਖਾਲੀ
obj_ਤਾਲਾ_const_vars(object_T *obj)
{
    ਲਈ (ਪੂਰਨ i = 0; i < obj->obj_class->class_obj_member_ਗਿਣਤੀ; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	ਜੇ (ocm->ocm_flags & OCMFLAG_CONST)
	{
	    typval_T *mtv = ((typval_T *)(obj + 1)) + i;
	    ਵਸਤੂ_ਤਾਲਾ(mtv, DICT_MAXNEST, TRUE, TRUE);
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
    ਪੂਰਨ		sz;

    ਜੇ (cl == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    sz = ਆਕਾਰ(object_T) + cl->class_obj_member_ਗਿਣਤੀ * ਆਕਾਰ(typval_T);
    obj = alloc_clear(sz);
    ਜੇ (obj == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    ਜੇ (IS_ENUM(cl))
	enum_ਸੈੱਟ_ਅੰਦਰੂਨੀ_obj_vars(cl, obj);

    ਵਾਪਸ obj;
}

/*
 * Make a copy of an object.
 */
    ਖਾਲੀ
copy_object(typval_T *from, typval_T *ਮੰਜ਼ਿਲ)
{
    ਜੇ (from->vval.v_object == ਨਿਰਾ)
	ਮੰਜ਼ਿਲ->vval.v_object = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	ਮੰਜ਼ਿਲ->vval.v_object = from->vval.v_object;
	++ਮੰਜ਼ਿਲ->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    ਖਾਲੀ
copy_class(typval_T *from, typval_T *ਮੰਜ਼ਿਲ)
{
    ਜੇ (from->vval.v_class == ਨਿਰਾ)
	ਮੰਜ਼ਿਲ->vval.v_class = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	ਮੰਜ਼ਿਲ->vval.v_class = from->vval.v_class;
	++ਮੰਜ਼ਿਲ->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    ਸਥਿਰ ਖਾਲੀ
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_interface_ਗਿਣਤੀ; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	ਜੇ (cl->class_interfaces_cl[i] != ਨਿਰਾ)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *ਅਗਲਾ;
    ਲਈ (itf2class_T *i2c = cl->class_itf2class; i2c != ਨਿਰਾ; i2c = ਅਗਲਾ)
    {
	ਅਗਲਾ = i2c->i2c_ਅਗਲਾ;
	vim_free(i2c);
    }

    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_class_member_ਗਿਣਤੀ; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	ਜੇ (cl->class_members_tv != ਨਿਰਾ)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_obj_member_ਗਿਣਤੀ; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_class_function_ਗਿਣਤੀ; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	ਜੇ (IS_ENUM(cl) && IS_CONSTRUCTOR_ਵਿਧੀ(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ; ++i)
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
    ਸਥਿਰ ਪੂਰਨ
class_get_selfrefs(class_T *cl)
{
    ਪੂਰਨ		self_refs = 0;
    typval_T	*tv;

    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_class_member_ਗਿਣਤੀ; ++i)
    {
	tv = &cl->class_members_tv[i];
	ਜੇ (tv->v_type == VAR_OBJECT && tv->vval.v_object != ਨਿਰਾ
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    ਵਾਪਸ self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    ਸਥਿਰ ਪੂਰਨ
can_free_enum(class_T *cl)
{
    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_class_member_ਗਿਣਤੀ; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	ਜੇ (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    ਵਾਪਸ TRUE;

	ਜੇ (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    ਜੇ (tv->vval.v_list->lv_refcount > 1)
		ਵਾਪਸ FALSE;
	    ਰੋਕੋ;
	}

	ਜੇ (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    ਵਾਪਸ FALSE;
    }

    ਵਾਪਸ TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    ਸਥਿਰ ਪੂਰਨ
can_free_class(class_T *cl)
{
    ਪੂਰਨ		can_free = FALSE;
    ਪੂਰਨ		self_refs = 0;

    ਜੇ (IS_ENUM(cl) && !can_free_enum(cl))
	ਵਾਪਸ FALSE;

    ਜੇ (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    ਜੇ (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    ਵਾਪਸ can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    ਖਾਲੀ
class_unref(class_T *cl)
{
    ਜੇ (cl == ਨਿਰਾ)
	ਵਾਪਸ;

    --cl->class_refcount;

    ਜੇ (cl->class_name.string == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    ਪੂਰਨ
class_free_nonref(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		did_free = FALSE;

    ਲਈ (class_T *cl = ਪਹਿਲਾ_class; cl != ਨਿਰਾ; cl = ਅਗਲਾ_nonref_class)
    {
	ਅਗਲਾ_nonref_class = cl->class_ਅਗਲਾ_ਵਰਤਿਆ;
	ਜੇ ((cl->class_copyID & COPYID_ਨਕਾਬ) != (copyID & COPYID_ਨਕਾਬ))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    ਅਗਲਾ_nonref_class = ਨਿਰਾ;
    ਵਾਪਸ did_free;
}

    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_classes(ਪੂਰਨ copyID)
{
    ਲਈ (class_T *cl = ਪਹਿਲਾ_class; cl != ਨਿਰਾ; cl = cl->class_ਅਗਲਾ_ਵਰਤਿਆ)
	ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ_class(cl, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);

    ਵਾਪਸ FALSE;
}

ਸਥਿਰ object_T *ਪਹਿਲਾ_object = ਨਿਰਾ;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    ਖਾਲੀ
object_created(object_T *obj)
{
    ਜੇ (ਪਹਿਲਾ_object != ਨਿਰਾ)
    {
	obj->obj_ਅਗਲਾ_ਵਰਤਿਆ = ਪਹਿਲਾ_object;
	ਪਹਿਲਾ_object->obj_prev_ਵਰਤਿਆ = obj;
    }
    ਪਹਿਲਾ_object = obj;
}

/*
 * Call this function when an object has been cleared and is about to be freed.
 * It is removed from the list headed by "first_object".
 */
    ਸਥਿਰ ਖਾਲੀ
object_cleared(object_T *obj)
{
    ਜੇ (obj->obj_ਅਗਲਾ_ਵਰਤਿਆ != ਨਿਰਾ)
	obj->obj_ਅਗਲਾ_ਵਰਤਿਆ->obj_prev_ਵਰਤਿਆ = obj->obj_prev_ਵਰਤਿਆ;
    ਜੇ (obj->obj_prev_ਵਰਤਿਆ != ਨਿਰਾ)
	obj->obj_prev_ਵਰਤਿਆ->obj_ਅਗਲਾ_ਵਰਤਿਆ = obj->obj_ਅਗਲਾ_ਵਰਤਿਆ;
    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ_object == obj)
	ਪਹਿਲਾ_object = obj->obj_ਅਗਲਾ_ਵਰਤਿਆ;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    ਸਥਿਰ ਖਾਲੀ
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    ਜੇ (!cl)
	ਵਾਪਸ;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    ਲਈ (ਪੂਰਨ i = 0; i < cl->class_obj_member_ਗਿਣਤੀ; ++i)
	clear_tv(tv + i);
}

    ਸਥਿਰ ਖਾਲੀ
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    ਜੇ (!cl)
	ਵਾਪਸ;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    ਸਥਿਰ ਖਾਲੀ
object_free(object_T *obj)
{
    ਜੇ (ਵਿੱਚ_free_unref_ਚੀਜ਼ਾਂ)
	ਵਾਪਸ;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    ਖਾਲੀ
object_unref(object_T *obj)
{
    ਜੇ (obj != ਨਿਰਾ && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    ਪੂਰਨ
object_free_nonref(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		did_free = FALSE;

    ਲਈ (object_T *obj = ਪਹਿਲਾ_object; obj != ਨਿਰਾ; obj = obj->obj_ਅਗਲਾ_ਵਰਤਿਆ)
    {
	ਜੇ ((obj->obj_copyID & COPYID_ਨਕਾਬ) != (copyID & COPYID_ਨਕਾਬ))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    ਵਾਪਸ did_free;
}

    ਖਾਲੀ
object_free_ਚੀਜ਼ਾਂ(ਪੂਰਨ copyID)
{
    object_T	*obj_ਅਗਲਾ;

    ਲਈ (object_T *obj = ਪਹਿਲਾ_object; obj != ਨਿਰਾ; obj = obj_ਅਗਲਾ)
    {
	obj_ਅਗਲਾ = obj->obj_ਅਗਲਾ_ਵਰਤਿਆ;
	ਜੇ ((obj->obj_copyID & COPYID_ਨਕਾਬ) != (copyID & COPYID_ਨਕਾਬ))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    ਖਾਲੀ
emsg_var_cl_define(ਅੱਖਰ *ਸੁਨੇਹਾ, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    ਜੇ (cl_def != ਨਿਰਾ)
	semsg(_(ਸੁਨੇਹਾ), m->ocm_name.string, cl_def->class_name.string);
    ਨਹੀਂਤਾਂ
	emsg(_(e_ਅੰਦਰੂਨੀ_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    ਖਾਲੀ
ਵਿਧੀ_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *ਵਿਧੀ_name = vim_strnsave(name, len);
    ਜੇ ((v_type == VAR_OBJECT)
	    && (class_ਵਿਧੀ_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	ਜੇ (*name == '_')
	    semsg(_(e_cannot_access_protected_ਵਿਧੀ_str), ਵਿਧੀ_name);
	ਨਹੀਂਤਾਂ
	    semsg(_(e_class_ਵਿਧੀ_str_accessible_only_using_class_str),
		    ਵਿਧੀ_name, cl->class_name.string);
    }
    ਨਹੀਂਤਾਂ ਜੇ ((v_type == VAR_CLASS)
	    && (object_ਵਿਧੀ_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	ਜੇ (*name == '_')
	    semsg(_(e_cannot_access_protected_ਵਿਧੀ_str), ਵਿਧੀ_name);
	ਨਹੀਂਤਾਂ
	    semsg(_(e_object_ਵਿਧੀ_str_accessible_only_using_object_str),
		    ਵਿਧੀ_name, cl->class_name.string);
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_ਵਿਧੀ_not_ਮਿਲਿਆ_on_class_str_str), ਵਿਧੀ_name,
		cl->class_name.string);
    vim_free(ਵਿਧੀ_name);
}

/*
 * Echo a class or object member not found message.
 */
    ਖਾਲੀ
member_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    ਜੇ (v_type == VAR_OBJECT)
    {
	ਜੇ (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	ਨਹੀਂਤਾਂ
	    semsg(_(e_variable_not_ਮਿਲਿਆ_on_object_str_str), varname,
		    cl->class_name.string);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (IS_ENUM(cl))
		semsg(_(e_enum_ਮੁੱਲ_str_not_ਮਿਲਿਆ_ਵਿੱਚ_enum_str),
			varname, cl->class_name.string);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_class_variable_str_not_ਮਿਲਿਆ_ਵਿੱਚ_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    ਖਾਲੀ
defcompile_class(class_T *cl)
{
    ਲਈ (ਪੂਰਨ loop = 1; loop <= 2; ++loop)
    {
	ਪੂਰਨ func_ਗਿਣਤੀ = loop == 1 ? cl->class_class_function_ਗਿਣਤੀ
						: cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ;
	ਲਈ (ਪੂਰਨ i = 0; i < func_ਗਿਣਤੀ; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    ਜੇ (!IS_ABSTRACT_ਵਿਧੀ(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    ਖਾਲੀ
defcompile_classes_ਵਿੱਚ_script(ਖਾਲੀ)
{
    ਲਈ (class_T *cl = ਪਹਿਲਾ_class; cl != ਨਿਰਾ; cl = cl->class_ਅਗਲਾ_ਵਰਤਿਆ)
    {
	ਜੇ (eval_variable(cl->class_name.string, 0, 0, ਨਿਰਾ, ਨਿਰਾ,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    ਪੂਰਨ
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    ਜੇ (eval_variable(name, 0, 0, rettv, ਨਿਰਾ, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	ਵਾਪਸ rettv->v_type == VAR_CLASS;
    ਵਾਪਸ FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
object_call_builtin_ਵਿਧੀ(
    object_T		*obj,
    class_builtin_T	builtin_ਵਿਧੀ,
    ਪੂਰਨ			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    ਪੂਰਨ	    midx;

    ਜੇ (obj == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    uf = class_get_builtin_ਵਿਧੀ(obj->obj_class, builtin_ਵਿਧੀ, &midx);
    ਜੇ (uf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    ਪੂਰਨ		r;

    ਜੇ (fc == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, ਨਿਰਾ, obj, fc, rettv);

    remove_funccal();

    ਵਾਪਸ r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    ਪੂਰਨ
object_ਖਾਲੀ_ਥਾਵਾਂ(object_T *obj)
{
    typval_T	rettv;

    ਜੇ (object_call_builtin_ਵਿਧੀ(obj, CLASS_BUILTIN_ਖਾਲੀ_ਥਾਵਾਂ, 0, ਨਿਰਾ, &rettv)
								== FAIL)
	ਵਾਪਸ TRUE;

    ਵਾਪਸ tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    ਪੂਰਨ
object_len(object_T *obj)
{
    typval_T	rettv;

    ਜੇ (object_call_builtin_ਵਿਧੀ(obj, CLASS_BUILTIN_LEN, 0, ਨਿਰਾ, &rettv)
								== FAIL)
	ਵਾਪਸ 0;

    ਵਾਪਸ tv_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    ਪੂਰਨ
object_equal(
	object_T *o1,
	object_T *o2,
	ਪੂਰਨ	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    ਜੇ (o1 == o2)
	ਵਾਪਸ TRUE;
    ਜੇ (o1 == ਨਿਰਾ || o2 == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    ਜੇ (cl1 != cl2 || cl1 == ਨਿਰਾ || cl2 == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਲਈ (ਪੂਰਨ i = 0; i < cl1->class_obj_member_ਗਿਣਤੀ; ++i)
	ਜੇ (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
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
    ਪੂਰਨ		copyID,
    ਪੂਰਨ		echo_style,
    ਪੂਰਨ		restore_copyID,
    ਪੂਰਨ		ਸੰਯੁਕਤ_val)
{
    typval_T	rettv;

    ਜੇ (object_call_builtin_ਵਿਧੀ(obj, CLASS_BUILTIN_STRING, 0, ਨਿਰਾ, &rettv)
								== OK
					&& rettv.vval.v_string != ਨਿਰਾ)
	ਵਾਪਸ rettv.vval.v_string;

    ਪੂਰਨ		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    ਜੇ (cl != ਨਿਰਾ && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.ਲੰਮਾਈ);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    ਨਹੀਂਤਾਂ
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	ਜੇ (cl == ਨਿਰਾ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ਨਹੀਂਤਾਂ
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.ਲੰਮਾਈ);
    }
    ਜੇ (cl != ਨਿਰਾ)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	ਲਈ (ਪੂਰਨ i = 0; i < cl->class_obj_member_ਗਿਣਤੀ; ++i)
	{
	    ਜੇ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.ਲੰਮਾਈ);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = ਨਿਰਾ;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, ਸੰਯੁਕਤ_val);
	    ਜੇ (s != ਨਿਰਾ)
		ga_concat(&ga, s);
	    vim_free(tf);
	    ਜੇ (s == ਨਿਰਾ || did_echo_string_emsg)
	    {
		ok = FAIL;
		ਰੋਕੋ;
	    }
	    ਸਤਰ_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    ਜੇ (ok == FAIL)
    {
	vim_free(ga.ga_ਅੰਕੜੇ);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ga_append(&ga, NUL);
    ਵਾਪਸ (char_u *)ga.ga_ਅੰਕੜੇ;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    ਪੂਰਨ
class_instance_ਦਾ(class_T *cl, class_T *other_cl)
{
    ਜੇ (cl == other_cl)
	ਵਾਪਸ TRUE;

    // Recursively check the base classes.
    ਲਈ (; cl != ਨਿਰਾ; cl = cl->class_extends)
    {
	ਜੇ (cl == other_cl)
	    ਵਾਪਸ TRUE;
	// Check the implemented interfaces and the super interfaces
	ਲਈ (ਪੂਰਨ i = cl->class_interface_ਗਿਣਤੀ - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    ਜਦੋਂ (intf != ਨਿਰਾ)
	    {
		ਜੇ (intf == other_cl)
		    ਵਾਪਸ TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    ਵਾਪਸ FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    ਖਾਲੀ
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_ਸੰਖਿਆ = VVAL_FALSE;

    ਜੇ (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_ਦਲੀਲਾਂ(argvars, 1) == FAIL)
	ਵਾਪਸ;

    ਜੇ (object_tv->vval.v_object == ਨਿਰਾ)
    {
	ਜੇ (classinfo_tv->vval.v_class == ਨਿਰਾ)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_ਸੰਖਿਆ = VVAL_TRUE;
	ਵਾਪਸ;
    }

    ਲਈ (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	ਜੇ (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	ਨਹੀਂਤਾਂ
	    c = classinfo_tv->vval.v_class;

	ਜੇ (class_instance_ਦਾ(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_ਸੰਖਿਆ = VVAL_TRUE;
	    ਵਾਪਸ;
	}
    }
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
