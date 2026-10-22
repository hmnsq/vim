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

#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

ಸ್ಥಿರ class_T *ಮೊದಲ_class = ಖಾಲಿ;
ಸ್ಥಿರ class_T *ಮುಂದಿನ_nonref_class = ಖಾಲಿ;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
class_created(class_T *cl)
{
    ಆದರೆ (ಮೊದಲ_class != ಖಾಲಿ)
    {
	cl->class_ಮುಂದಿನ_ಬಳಸಿದ = ಮೊದಲ_class;
	ಮೊದಲ_class->class_prev_ಬಳಸಿದ = cl;
    }
    ಮೊದಲ_class = cl;
}

/*
 * Call this function when a class has been cleared and is about to be freed.
 * It is removed from the list headed by "first_class".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
class_cleared(class_T *cl)
{
    ಆದರೆ (cl->class_ಮುಂದಿನ_ಬಳಸಿದ != ಖಾಲಿ)
	cl->class_ಮುಂದಿನ_ಬಳಸಿದ->class_prev_ಬಳಸಿದ = cl->class_prev_ಬಳಸಿದ;
    ಆದರೆ (cl->class_prev_ಬಳಸಿದ != ಖಾಲಿ)
	cl->class_prev_ಬಳಸಿದ->class_ಮುಂದಿನ_ಬಳಸಿದ = cl->class_ಮುಂದಿನ_ಬಳಸಿದ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ_class == cl)
	ಮೊದಲ_class = cl->class_ಮುಂದಿನ_ಬಳಸಿದ;

    // update the next class to check if needed
    ಆದರೆ (cl == ಮುಂದಿನ_nonref_class)
	ಮುಂದಿನ_nonref_class = cl->class_ಮುಂದಿನ_ಬಳಸಿದ;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_member(
    exarg_T	*eap,
    char_u	*ಸಾಲು,
    char_u	*varname,
    ಪೂರ್ಣಾಂಕ		has_ಬಹಿರಂಗ,	    // TRUE if "public" seen before "varname"
    char_u	**varname_ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_ಅಭಿವ್ಯಕ್ತಿ)
{
    *varname_ಕೊನೆ = ಗಮ್ಯ_name_ಕೊನೆ(varname, FALSE);
    ಆದರೆ (*varname == '_' && has_ಬಹಿರಂಗ)
    {
	semsg(_(e_ಬಹಿರಂಗ_variable_name_cannot_ಆರಂಭ_with_underscore_str), ಸಾಲು);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    char_u *colon = skipwhite(*varname_ಕೊನೆ);
    char_u *type_arg = colon;
    type_T *type = ಖಾಲಿ;
    *has_type = FALSE;
    ಆದರೆ (*colon == ':')
    {
	ಆದರೆ (VIM_ISWHITE(**varname_ಕೊನೆ))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, ಖಾಲಿ, ಖಾಲಿ, TRUE);
	ಆದರೆ (type == ಖಾಲಿ || !valid_declaration_type(type))
	    ಹಿಂತಿರುಗಿ FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    ಆದರೆ (type == ಖಾಲಿ && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (init_ಅಭಿವ್ಯಕ್ತಿ == ಖಾಲಿ && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_ಒಳ_interface));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, *ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ;

	ಆದರೆ (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ_concatenate(&init_arg, &ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, &ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ, &evalarg);

	init_arg = skipwhite(init_arg);
	ಆದರೆ (*init_arg != NUL && !vim9_comment_ಆರಂಭ(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, ಖಾಲಿ);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	ಆದರೆ (type == ಖಾಲಿ)
	    type = &t_any;

	*init_ಅಭಿವ್ಯಕ್ತಿ = vim_strnsave(ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ - ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!valid_declaration_type(type))
	ಹಿಂತಿರುಗಿ FAIL;

    *type_ret = type;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		has_ಬಹಿರಂಗ,
    ಪೂರ್ಣಾಂಕ		has_final,
    ಪೂರ್ಣಾಂಕ		has_const,
    ಪೂರ್ಣಾಂಕ		has_type,
    type_T	*type,
    char_u	*init_ಅಭಿವ್ಯಕ್ತಿ)
{
    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_ದತ್ತಾಂಶ) + gap->ga_len;
    m->ocm_name.ಉದ್ದಳತೆ = (size_t)(varname_ಕೊನೆ - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.ಉದ್ದಳತೆ);
    ಆದರೆ (m->ocm_name.string == ಖಾಲಿ)
    {
	m->ocm_name.ಉದ್ದಳತೆ = 0;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    m->ocm_access = has_ಬಹಿರಂಗ ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_ಖಾಸಗಿ : VIM_ACCESS_READ;
    ಆದರೆ (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    ಆದರೆ (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    ಆದರೆ (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    ಆದರೆ (init_ಅಭಿವ್ಯಕ್ತಿ != ಖಾಲಿ)
    {
	m->ocm_init = init_ಅಭಿವ್ಯಕ್ತಿ;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_members_ಗಮ್ಯ_class(
    garray_T	*gap,
    ocmember_T	*ಪೋಷಕ_members,
    ಪೂರ್ಣಾಂಕ		ಪೋಷಕ_ಎಣಿಕೆ,
    ocmember_T	**members,
    ಪೂರ್ಣಾಂಕ		*member_ಎಣಿಕೆ)
{
    *member_ಎಣಿಕೆ = ಪೋಷಕ_ಎಣಿಕೆ + gap->ga_len;
    *members = *member_ಎಣಿಕೆ == 0 ? ಖಾಲಿ
				       : ALLOC_MULT(ocmember_T, *member_ಎಣಿಕೆ);
    ಆದರೆ (*member_ಎಣಿಕೆ > 0 && *members == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಪೋಷಕ_ಎಣಿಕೆ; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = ಪೋಷಕ_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.ಉದ್ದಳತೆ);
	ಆದರೆ (m->ocm_name.string == ಖಾಲಿ)
	{
	    m->ocm_name.ಉದ್ದಳತೆ = 0;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (m->ocm_init != ಖಾಲಿ)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    ಆದರೆ (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + ಪೋಷಕ_ಎಣಿಕೆ,
			       gap->ga_ದತ್ತಾಂಶ, ಗಾತ್ರ(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_ದತ್ತಾಂಶ);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    ಪೂರ್ಣಾಂಕ
object_ಸೂಚ್ಯಂಕ_from_itf_ಸೂಚ್ಯಂಕ(class_T *itf, ಪೂರ್ಣಾಂಕ is_ವಿಧಾನ, ಪೂರ್ಣಾಂಕ idx, class_T *cl)
{
    ಆದರೆ (idx >= (is_ವಿಧಾನ ? itf->class_obj_ವಿಧಾನ_ಎಣಿಕೆ
				   : itf->class_obj_member_ಎಣಿಕೆ))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	ಹಿಂತಿರುಗಿ 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    ಆದರೆ (cl == itf)
	ಹಿಂತಿರುಗಿ idx;

    itf2class_T		*i2c = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			searching = TRUE;
    ಪೂರ್ಣಾಂಕ			ವಿಧಾನ_ಸ್ಥಾನಾಂತರ = 0;

    ಗಾಗಿ (class_T *super = cl; super != ಖಾಲಿ && searching;
						super = super->class_extends)
    {
	ಗಾಗಿ (i2c = itf->class_itf2class; i2c != ಖಾಲಿ; i2c = i2c->i2c_ಮುಂದಿನ)
	{
	    ಆದರೆ (i2c->i2c_class == super && i2c->i2c_is_ವಿಧಾನ == is_ವಿಧಾನ)
	    {
		searching = FALSE;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (searching && is_ವಿಧಾನ)
	    // The parent class methods are stored after the current class
	    // methods.
	    ವಿಧಾನ_ಸ್ಥಾನಾಂತರ += super->class_obj_ವಿಧಾನ_ಎಣಿಕೆ_ಮಗು;
    }
    ಆದರೆ (i2c == ಖಾಲಿ)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	ಹಿಂತಿರುಗಿ 0;
    }

    // A table follows the i2c for the class
    ಪೂರ್ಣಾಂಕ *ಕೋಷ್ಟಕ = (ಪೂರ್ಣಾಂಕ *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    ಹಿಂತಿರುಗಿ ಕೋಷ್ಟಕ[idx] + ವಿಧಾನ_ಸ್ಥಾನಾಂತರ;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    ಪೂರ್ಣಾಂಕ	    is_class)
{
    typval_T	tv;
    ಪೂರ್ಣಾಂಕ		success = FALSE;

    ಆದರೆ (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	ಹಿಂತಿರುಗಿ success;
    }

    tv.v_type = VAR_UNKNOWN;
    ಆದರೆ (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_ಸಿಕ್ಕಿತು_str), extends_name);
	ಹಿಂತಿರುಗಿ success;
    }

    ಆದರೆ (tv.v_type != VAR_CLASS || tv.vval.v_class == ಖಾಲಿ
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    ಹಿಂತಿರುಗಿ success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
validate_extends_generic_ವಿಧಾನ(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    ಅಕ್ಷರ	*ಸಂದೇಶ = ಖಾಲಿ;

    ಆದರೆ (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (super_fp->uf_generic_argcount == 0)
	ಸಂದೇಶ = e_concrete_ವಿಧಾನ_str_override_with_generic_ವಿಧಾನ_ಒಳ_class_str;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cl_fp->uf_generic_argcount == 0)
	ಸಂದೇಶ = e_generic_ವಿಧಾನ_str_override_with_concrete_ವಿಧಾನ_ಒಳ_class_str;
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ = e_generic_ವಿಧಾನ_str_type_arguments_mismatch_ಒಳ_class_str;

    semsg(_(ಸಂದೇಶ), cl_fp->uf_name, super_cl->class_name.string);
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    ಪೂರ್ಣಾಂಕ		ವಿಧಾನ_ಎಣಿಕೆ = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_ದತ್ತಾಂಶ);

    ಆಗ (super != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ extends_ವಿಧಾನ_ಎಣಿಕೆ = super->class_obj_ವಿಧಾನ_ಎಣಿಕೆ_ಮಗು;
	ಆದರೆ (extends_ವಿಧಾನ_ಎಣಿಕೆ == 0)
	{
	    super = super->class_extends;
	    ಮುಂದುವರಿ;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < extends_ವಿಧಾನ_ಎಣಿಕೆ; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    ಪೂರ್ಣಾಂಕ	    extends_ಖಾಸಗಿ = (*pstr == '_');
	    ಆದರೆ (extends_ಖಾಸಗಿ)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ವಿಧಾನ_ಎಣಿಕೆ; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		ಪೂರ್ಣಾಂಕ	priv_ವಿಧಾನ = (*qstr == '_');
		ಆದರೆ (priv_ವಿಧಾನ)
		    qstr++;
		ಆದರೆ (STRCMP(pstr, qstr) == 0)
		{
		    ಆದರೆ (priv_ವಿಧಾನ != extends_ಖಾಸಗಿ)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_ವಿಧಾನ_str_ದ_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			ಹಿಂತಿರುಗಿ FALSE;
		    }

		    ಆದರೆ (!validate_extends_generic_ವಿಧಾನ(super,
						extends_methods[i], cl_fp[j]))
			    ಹಿಂತಿರುಗಿ FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಂಕ member_ಎಣಿಕೆ = objmembers_gap->ga_len;
    ಆದರೆ (member_ಎಣಿಕೆ == 0)
	ಹಿಂತಿರುಗಿ TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_ದತ್ತಾಂಶ);

    // Validate each member variable
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ c_i = 0; c_i < member_ಎಣಿಕೆ; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	ಆಗ (p_cl != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ p_member_ಎಣಿಕೆ = p_cl->class_obj_member_ಎಣಿಕೆ;

	    ಆದರೆ (p_member_ಎಣಿಕೆ == 0)
	    {
		p_cl = p_cl->class_extends;
		ಮುಂದುವರಿ;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ p_i = 0; p_i < p_member_ಎಣಿಕೆ; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		ಆದರೆ (STRCMP(pstr, qstr) == 0)
		{
		    ಆದರೆ (c_protected != p_protected)
			semsg(_(e_ಬಹಿರಂಗ_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    ಇಲ್ಲದಿದ್ದರೆ
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    ಹಿಂತಿರುಗಿ FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಂಕ member_ಎಣಿಕೆ = objmembers_gap->ga_len;
    ಆದರೆ (member_ಎಣಿಕೆ == 0)
	ಹಿಂತಿರುಗಿ TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_ದತ್ತಾಂಶ);

    // Validate each member variable
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ c_i = 0; c_i < member_ಎಣಿಕೆ; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	ಪೂರ್ಣಾಂಕ	    var_ಸಿಕ್ಕಿತು = FALSE;

	// Check in all the parent classes in the lineage
	ಆಗ (p_cl != ಖಾಲಿ && !var_ಸಿಕ್ಕಿತು)
	{
	    ಪೂರ್ಣಾಂಕ p_member_ಎಣಿಕೆ = p_cl->class_obj_member_ಎಣಿಕೆ;
	    ಆದರೆ (p_member_ಎಣಿಕೆ == 0)
	    {
		p_cl = p_cl->class_extends;
		ಮುಂದುವರಿ;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ p_i = 0; p_i < p_member_ಎಣಿಕೆ; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		ಆದರೆ (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    ಮುಂದುವರಿ;

		// Ensure the type is matching.
		where.wt_func_name = (ಅಕ್ಷರ *)c_m->ocm_name.string;
		where.wt_ಬಗೆ = WT_MEMBER;

		ಆದರೆ (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    ಹಿಂತಿರುಗಿ FALSE;

		var_ಸಿಕ್ಕಿತು = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	ಪೂರ್ಣಾಂಕ extends_ವಿಧಾನ_ಎಣಿಕೆ = loop == 1
				? extends_cl->class_class_function_ಎಣಿಕೆ
				: extends_cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ;
	ಆದರೆ (extends_ವಿಧಾನ_ಎಣಿಕೆ == 0)
	    ಮುಂದುವರಿ;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	ಪೂರ್ಣಾಂಕ ವಿಧಾನ_ಎಣಿಕೆ = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_ದತ್ತಾಂಶ
						: objmethods_gap->ga_ದತ್ತಾಂಶ);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < extends_ವಿಧಾನ_ಎಣಿಕೆ; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    ಆದರೆ (!IS_ABSTRACT_ವಿಧಾನ(uf))
		ಮುಂದುವರಿ;

	    ಪೂರ್ಣಾಂಕ	concrete_ವಿಧಾನ_ಸಿಕ್ಕಿತು = FALSE;
	    ಪೂರ್ಣಾಂಕ	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    ಗಾಗಿ (j = 0; !concrete_ವಿಧಾನ_ಸಿಕ್ಕಿತು && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		ಆದರೆ (!IS_ABSTRACT_ವಿಧಾನ(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_ವಿಧಾನ_ಸಿಕ್ಕಿತು = TRUE;
	    }

	    ಆದರೆ (concrete_ವಿಧಾನ_ಸಿಕ್ಕಿತು)
		ಮುಂದುವರಿ;

	    ಗಾಗಿ (j = 0; j < ವಿಧಾನ_ಎಣಿಕೆ; j++)
	    {
		ಆದರೆ (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_ವಿಧಾನ_ಸಿಕ್ಕಿತು = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }

	    ಆದರೆ (!concrete_ವಿಧಾನ_ಸಿಕ್ಕಿತು)
	    {
		semsg(_(e_abstract_ವಿಧಾನ_str_not_implemented), uf->uf_name);
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    ಪೂರ್ಣಾಂಕ		cl_member_ಎಣಿಕೆ,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಂಕ		variable_present  = FALSE;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cl_i = 0; cl_i < cl_member_ಎಣಿಕೆ; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	ಆದರೆ (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    ಮುಂದುವರಿ;

	// Ensure the access type is same
	ಆದರೆ (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_ದ_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	// Ensure the type is matching.
	ಆದರೆ (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    where.wt_func_name = (ಅಕ್ಷರ *)m->ocm_name.string;
	    where.wt_ಬಗೆ = WT_MEMBER;
	    ಆದರೆ (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		ಹಿಂತಿರುಗಿ FALSE;
	}

	variable_present = TRUE;
	ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (!variable_present && extends_cl != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ ext_cl_ಎಣಿಕೆ = extends_cl->class_obj_member_ಎಣಿಕೆ;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	ಹಿಂತಿರುಗಿ intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_ಎಣಿಕೆ,
					extends_cl->class_extends);
    }

    ಹಿಂತಿರುಗಿ variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಂಕ if_ಎಣಿಕೆ = ifcl->class_obj_member_ಎಣಿಕೆ;
    ಆದರೆ (if_ಎಣಿಕೆ == 0)
	ಹಿಂತಿರುಗಿ TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_ದತ್ತಾಂಶ);
    ಪೂರ್ಣಾಂಕ cl_ಎಣಿಕೆ = objmembers_gap->ga_len;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ if_i = 0; if_i < if_ಎಣಿಕೆ; ++if_i)
    {
	ಆದರೆ (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_ಎಣಿಕೆ, extends_cl))
	{
	    semsg(_(e_variable_str_ದ_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
intf_ವಿಧಾನ_type_matches(
    ufunc_T	*if_ವಿಧಾನ,
    ufunc_T	*cl_ವಿಧಾನ,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    ಆದರೆ (cl_ವಿಧಾನ->uf_generic_argcount != if_ವಿಧಾನ->uf_generic_argcount)
    {
	semsg(_(e_type_var_ಎಣಿಕೆ_ದ_ವಿಧಾನ_str_differs_from_interface_str),
						cl_ವಿಧಾನ->uf_name, intf_name);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (ಅಕ್ಷರ *)if_ವಿಧಾನ->uf_name;
    where.wt_ಬಗೆ = WT_ವಿಧಾನ;
    ಆದರೆ (check_type(if_ವಿಧಾನ->uf_func_type, cl_ವಿಧಾನ->uf_func_type, TRUE,
								where) == FAIL)
	ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
intf_ವಿಧಾನ_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    ಪೂರ್ಣಾಂಕ	    cl_ಎಣಿಕೆ,
    class_T *extends_cl,
    char_u  *intf_name)
{
    ಪೂರ್ಣಾಂಕ		ವಿಧಾನ_present  = FALSE;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cl_i = 0; cl_i < cl_ಎಣಿಕೆ; ++cl_i)
    {
	ಆದರೆ (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    ಆದರೆ (!intf_ವಿಧಾನ_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		ಹಿಂತಿರುಗಿ FALSE;
	    ವಿಧಾನ_present = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (!ವಿಧಾನ_present && extends_cl != ಖಾಲಿ)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	ಪೂರ್ಣಾಂಕ	ext_cl_ಎಣಿಕೆ = extends_cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ;
	ಹಿಂತಿರುಗಿ intf_ವಿಧಾನ_present(if_ufunc, ext_cl_fp, ext_cl_ಎಣಿಕೆ,
					extends_cl->class_extends, intf_name);
    }

    ಹಿಂತಿರುಗಿ ವಿಧಾನ_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಂಕ if_ಎಣಿಕೆ = ifcl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ;
    ಆದರೆ (if_ಎಣಿಕೆ == 0)
	ಹಿಂತಿರುಗಿ TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_ದತ್ತಾಂಶ);
    ಪೂರ್ಣಾಂಕ cl_ಎಣಿಕೆ = objmethods_gap->ga_len;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ if_i = 0; if_i < if_ಎಣಿಕೆ; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	ಆದರೆ (!intf_ವಿಧಾನ_present(if_fp[if_i], cl_fp, cl_ಎಣಿಕೆ, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_ವಿಧಾನ_str_ದ_interface_str_not_implemented),
		    if_name, intf_class_name);
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    ಪೂರ್ಣಾಂಕ		success = TRUE;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_ದತ್ತಾಂಶ)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	ಆದರೆ (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_ಸಿಕ್ಕಿತು_str), impl);
	    success = FALSE;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == ಖಾಲಿ
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    ನಿಲ್ಲಿಸು;
	}

	class_T *ifcl = tv.vval.v_class;
	ಆದರೆ (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    ನಿಲ್ಲಿಸು;
	}
	((class_T **)intf_classes_gap->ga_ದತ್ತಾಂಶ)[intf_classes_gap->ga_len]
								= ifcl;
	intf_classes_gap->ga_len++;
	++ifcl->class_refcount;

	// check the variables of the interface match the members of the class
	success = validate_interface_variables(impl, ifcl, objmembers_gap,
								extends_cl);
	clear_tv(&tv);
    }

    ಆದರೆ (!success)
	ಹಿಂತಿರುಗಿ FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_ದತ್ತಾಂಶ;
    cl->class_interface_ಎಣಿಕೆ = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_ದತ್ತಾಂಶ)[i],
		((class_T **)intf_classes_gap->ga_ದತ್ತಾಂಶ)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = ಖಾಲಿ;
    cl->class_interface_ಎಣಿಕೆ = 0;
    cl->class_extends = ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < intf_classes_gap->ga_len; j++)
    {
	ಆದರೆ (((class_T **)intf_classes_gap->ga_ದತ್ತಾಂಶ)[j] == ifcl)
	    ಹಿಂತಿರುಗಿ TRUE;
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.ಉದ್ದಳತೆ);
    ಆದರೆ (intf_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (ga_grow(impl_gap, 1) == FAIL)
	ಹೋಗು fail;

    char_u **intf_ಹೆಸರುಗಳು = (char_u **)impl_gap->ga_ದತ್ತಾಂಶ;
    intf_ಹೆಸರುಗಳು[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    ಆದರೆ (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	ಹೋಗು fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_ದತ್ತಾಂಶ;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    ಹಿಂತಿರುಗಿ TRUE;
fail:
    vim_free(intf_name);
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < super->class_interface_ಎಣಿಕೆ; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	ಆದರೆ (!is_interface_class_present(intf_classes_gap, ifcl))
	    ಕೂಡಿಸು_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_func_arg_ಹೆಸರುಗಳು(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_ದತ್ತಾಂಶ)[fi];

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < uf->uf_ವಾದಗಳು.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_ವಾದಗಳು.ga_ದತ್ತಾಂಶ)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_ದತ್ತಾಂಶ + mi)->ocm_name.string;
		    ಆದರೆ (STRCMP(aname, mname) == 0)
		    {
			ಆದರೆ (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_ವಾದ_already_declared_ಒಳ_class_str),
				aname);

			ಹಿಂತಿರುಗಿ FALSE;
		    }
		}
	    }
	}
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_reserved_varname(char_u *varname, char_u *varname_ಕೊನೆ)
{
    ಪೂರ್ಣಾಂಕ reserved = FALSE;
    char_u save_varname_ಕೊನೆ = *varname_ಕೊನೆ;
    *varname_ಕೊನೆ = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_ಕೊನೆ = save_varname_ಕೊನೆ;

    ಹಿಂತಿರುಗಿ reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_ಕೊನೆ)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_ಕೊನೆ - varname)};
    ಪೂರ್ಣಾಂಕ		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    ಆದರೆ (*pstr.string == '_')
    {
	pstr.string++;
	pstr.ಉದ್ದಳತೆ--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_ದತ್ತಾಂಶ) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.ಉದ್ದಳತೆ};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    ಆದರೆ (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.ಉದ್ದಳತೆ--;
		q_protected = true;
	    }

	    ಆದರೆ (pstr.ಉದ್ದಳತೆ == qstr.ಉದ್ದಳತೆ
		&& STRNCMP(pstr.string, qstr.string, pstr.ಉದ್ದಳತೆ) == 0)
	    {
		char_u	save_c = *varname_ಕೊನೆ;

		*varname_ಕೊನೆ = NUL;
		ಆದರೆ (p_protected != q_protected)
		    semsg(_(e_ಬಹಿರಂಗ_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		ಇಲ್ಲದಿದ್ದರೆ
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_ಕೊನೆ = save_c;
		dup = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_duplicate_ವಿಧಾನ(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_ದತ್ತಾಂಶ)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    ಆದರೆ (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		ಹಿಂತಿರುಗಿ TRUE;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_valid_constructor(ufunc_T *uf, ಪೂರ್ಣಾಂಕ is_abstract, ಪೂರ್ಣಾಂಕ has_static)
{
    // Constructors are not allowed in abstract classes.
    ಆದರೆ (is_abstract)
    {
	emsg(_(e_cannot_define_new_ವಿಧಾನ_ಒಳ_abstract_class));
	ಹಿಂತಿರುಗಿ FALSE;
    }
    // A constructor is always static, no need to define it so.
    ಆದರೆ (has_static)
    {
	emsg(_(e_cannot_define_new_ವಿಧಾನ_as_static));
	ಹಿಂತಿರುಗಿ FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    ಆದರೆ (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_ವಿಧಾನ));
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
object_check_builtin_ವಿಧಾನ_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    ಪೂರ್ಣಾಂಕ	    valid = FALSE;
    type_T  ವಿಧಾನ_sig;
    type_T  ವಿಧಾನ_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_ಕ್ಷೇತ್ರ(ವಿಧಾನ_sig);
    CLEAR_ಕ್ಷೇತ್ರ(ವಿಧಾನ_rt);
    ವಿಧಾನ_sig.tt_type = VAR_FUNC;

    ಆದರೆ (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	ವಿಧಾನ_rt.tt_type = VAR_ಸಂಖ್ಯೆ;
	ವಿಧಾನ_sig.tt_member = &ವಿಧಾನ_rt;
	valid = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	ವಿಧಾನ_rt.tt_type = VAR_BOOL;
	ವಿಧಾನ_sig.tt_member = &ವಿಧಾನ_rt;
	valid = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	ವಿಧಾನ_rt.tt_type = VAR_STRING;
	ವಿಧಾನ_sig.tt_member = &ವಿಧಾನ_rt;
	valid = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_builtin_object_ವಿಧಾನ_str_not_supported), uf->uf_name);

    where.wt_func_name = (ಅಕ್ಷರ *)uf->uf_name;
    where.wt_ಬಗೆ = WT_ವಿಧಾನ;
    ಆದರೆ (valid && !check_type(&ವಿಧಾನ_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    ಹಿಂತಿರುಗಿ valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    ಪೂರ್ಣಾಂಕ
is_valid_builtin_obj_methodname(char_u *funcname)
{
    ಆಯ್ಕೆ (funcname[0])
    {
	ಸ್ಥಿತಿ 'e':
	    ಹಿಂತಿರುಗಿ STRNCMP(funcname, "empty", 5) == 0;

	ಸ್ಥಿತಿ 'l':
	    ಹಿಂತಿರುಗಿ STRNCMP(funcname, "len", 3) == 0;

	ಸ್ಥಿತಿ 'n':
	    ಹಿಂತಿರುಗಿ STRNCMP(funcname, "new", 3) == 0;

	ಸ್ಥಿತಿ 's':
	    ಹಿಂತಿರುಗಿ STRNCMP(funcname, "string", 6) == 0;
    }

    ಹಿಂತಿರುಗಿ FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_ವಿಧಾನ(
    class_T		*cl,
    class_builtin_T	builtin_ವಿಧಾನ,
    ಪೂರ್ಣಾಂಕ			*ವಿಧಾನ_idx)
{
    *ವಿಧಾನ_idx = -1;

    ಆದರೆ (cl == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    *ವಿಧಾನ_idx = cl->class_builtin_methods[builtin_ವಿಧಾನ];
    ಹಿಂತಿರುಗಿ *ವಿಧಾನ_idx != -1 ? cl->class_obj_methods[*ವಿಧಾನ_idx] : ಖಾಲಿ;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
update_member_ವಿಧಾನ_lookup_ಕೋಷ್ಟಕ(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    ಪೂರ್ಣಾಂಕ		pobj_ವಿಧಾನ_ಸ್ಥಾನಾಂತರ)
{
    ಆದರೆ (ifcl == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(ಗಾತ್ರ(itf2class_T)
				+ ifcl->class_obj_member_ಎಣಿಕೆ * ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
    ಆದರೆ (if2cl == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    if2cl->i2c_ಮುಂದಿನ = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_ವಿಧಾನ = FALSE;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ if_i = 0; if_i < ifcl->class_obj_member_ಎಣಿಕೆ; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cl_i = 0; cl_i < cl->class_obj_member_ಎಣಿಕೆ; ++cl_i)
	{
	    ಆದರೆ (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		ಪೂರ್ಣಾಂಕ *ಕೋಷ್ಟಕ = (ಪೂರ್ಣಾಂಕ *)(if2cl + 1);
		ಕೋಷ್ಟಕ[if_i] = cl_i;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(ಗಾತ್ರ(itf2class_T)
				+ ifcl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ * ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
    ಆದರೆ (if2cl == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    if2cl->i2c_ಮುಂದಿನ = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_ವಿಧಾನ = TRUE;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ if_i = 0; if_i < ifcl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ; ++if_i)
    {
	ಪೂರ್ಣಾಂಕ	ಮುಗಿದ = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    ಆದರೆ (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_ದತ್ತಾಂಶ)[cl_i]->uf_name) == 0)
	    {
		ಪೂರ್ಣಾಂಕ *ಕೋಷ್ಟಕ = (ಪೂರ್ಣಾಂಕ *)(if2cl + 1);
		ಕೋಷ್ಟಕ[if_i] = cl_i;
		ಮುಗಿದ = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	ಆದರೆ (!ಮುಗಿದ)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    ಆದರೆ (cl->class_extends != ifcl)
	    {
		class_T		*ಪೋಷಕ = cl->class_extends;
		ಪೂರ್ಣಾಂಕ		ವಿಧಾನ_ಸ್ಥಾನಾಂತರ = objmethods->ga_len;

		ಆಗ (!ಮುಗಿದ && ಪೋಷಕ != ಖಾಲಿ && ಪೋಷಕ != ifcl)
		{
		    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cl_i = 0;
			    cl_i < ಪೋಷಕ->class_obj_ವಿಧಾನ_ಎಣಿಕೆ_ಮಗು; ++cl_i)
		    {
			ಆದರೆ (STRCMP(if_name,
				    ಪೋಷಕ->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    ಪೂರ್ಣಾಂಕ *ಕೋಷ್ಟಕ = (ಪೂರ್ಣಾಂಕ *)(if2cl + 1);
			    ಕೋಷ್ಟಕ[if_i] = ವಿಧಾನ_ಸ್ಥಾನಾಂತರ + cl_i;
			    ಮುಗಿದ = TRUE;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    ವಿಧಾನ_ಸ್ಥಾನಾಂತರ += ಪೋಷಕ->class_obj_ವಿಧಾನ_ಎಣಿಕೆ_ಮಗು;
		    ಪೋಷಕ = ಪೋಷಕ->class_extends;
		}
	    }

	    ಆದರೆ (!ಮುಗಿದ)
	    {
		ಪೂರ್ಣಾಂಕ *ಕೋಷ್ಟಕ = (ಪೂರ್ಣಾಂಕ *)(if2cl + 1);
		ಕೋಷ್ಟಕ[if_i] = pobj_ವಿಧಾನ_ಸ್ಥಾನಾಂತರ + if_i;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_interface_ಎಣಿಕೆ; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	ಆಗ (ifcl != ಖಾಲಿ)
	{
	    ಆದರೆ (update_member_ವಿಧಾನ_lookup_ಕೋಷ್ಟಕ(ifcl, cl, objmethods_gap,
								0) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    ಆದರೆ (extends_cl != ಖಾಲಿ)
    {
	class_T		*pclass = extends_cl;
	ಪೂರ್ಣಾಂಕ		pobj_ವಿಧಾನ_ಸ್ಥಾನಾಂತರ = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	ಆಗ (pclass != ಖಾಲಿ)
	{
	    ಆದರೆ (update_member_ವಿಧಾನ_lookup_ಕೋಷ್ಟಕ(pclass, cl,
			objmethods_gap, pobj_ವಿಧಾನ_ಸ್ಥಾನಾಂತರ) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;

	    pobj_ವಿಧಾನ_ಸ್ಥಾನಾಂತರ += pclass->class_obj_ವಿಧಾನ_ಎಣಿಕೆ_ಮಗು;
	    pclass = pclass->class_extends;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_ಎಣಿಕೆ);
    ಆದರೆ (cl->class_members_tv == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_class_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	ಆದರೆ (m->ocm_init != ಖಾಲಿ)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_ಅಭಿವ್ಯಕ್ತಿ(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    ಆದರೆ (etv == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;

	    ಆದರೆ (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && etv->v_type != VAR_SPECIAL)
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = typval2type(etv, get_copyID(),
			type_list_gap,
			TVTT_DO_MEMBER|TVTT_ಇನ್ನಷ್ಟು_SPECIFIC);
	    *tv = *etv;
	    vim_free(etv);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = ಖಾಲಿ;
	}
	ಗಣ_tv_type(tv, m->ocm_type);
	ಆದರೆ (m->ocm_flags & OCMFLAG_CONST)
	    ಅಂಶ_ಬೀಗ(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    ಪೂರ್ಣಾಂಕ		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_obj_member_ಎಣಿಕೆ; ++i)
    {
	ಆದರೆ (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    ಮುಂದುವರಿ;

	ಆದರೆ (i > (is_enum ? 2 : 0))
	    GA_CONCAT_LITERAL(&fga, ", ");
	GA_CONCAT_LITERAL(&fga, "this.");
	ocmember_T *m = cl->class_obj_members + i;
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.ಉದ್ದಳತೆ);
	GA_CONCAT_LITERAL(&fga, " = v:none");
    }
    GA_CONCAT_LITERAL(&fga, ")\nenddef\n");
    ga_append(&fga, NUL);

    exarg_T fea;
    CLEAR_ಕ್ಷೇತ್ರ(fea);
    fea.cmdidx = CMD_def;
    fea.cmd = fea.arg = fga.ga_ದತ್ತಾಂಶ;

    garray_T ಸಾಲುಗಳು_ಗಮ್ಯ_free;
    ga_init2(&ಸಾಲುಗಳು_ಗಮ್ಯ_free, ಗಾತ್ರ(char_u *), 50);

    ufunc_T *nf = define_function(&fea, ಖಾಲಿ, &ಸಾಲುಗಳು_ಗಮ್ಯ_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_ಎಣಿಕೆ,
			    ಖಾಲಿ);

    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಸಾಲುಗಳು_ಗಮ್ಯ_free);
    vim_free(fga.ga_ದತ್ತಾಂಶ);

    ಆದರೆ (nf != ಖಾಲಿ && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_ದತ್ತಾಂಶ)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	ಆದರೆ (nf->uf_ret_type != ಖಾಲಿ)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_ವಾದಗಳು = ಖಾಲಿ;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	ಪೂರ್ಣಾಂಕ		*fcount = loop == 1 ? &cl->class_class_function_ಎಣಿಕೆ
						: &cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	ಪೂರ್ಣಾಂಕ ಪೋಷಕ_ಎಣಿಕೆ = 0;
	ಆದರೆ (extends_cl != ಖಾಲಿ)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    ಪೋಷಕ_ಎಣಿಕೆ = loop == 1
				? 0
				: extends_cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ;

	*fcount = ಪೋಷಕ_ಎಣಿಕೆ + gap->ga_len;
	ಆದರೆ (*fcount == 0)
	{
	    *fup = ಖಾಲಿ;
	    ಮುಂದುವರಿ;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	ಆದರೆ (*fup == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_ದತ್ತಾಂಶ, ಗಾತ್ರ(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_ದತ್ತಾಂಶ);
	ಆದರೆ (loop == 1)
	    cl->class_class_function_ಎಣಿಕೆ_ಮಗು = gap->ga_len;
	ಇಲ್ಲದಿದ್ದರೆ
	    cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ_ಮಗು = gap->ga_len;

	ಆದರೆ (loop == 2)
	{
	    // Copy instance methods from the parent.

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಪೋಷಕ_ಎಣಿಕೆ; ++i)
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
		ಆದರೆ (nf != ಖಾಲಿ && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_ವಾದಗಳು != ಖಾಲಿ)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    ಆದರೆ (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (ಅಕ್ಷರ *)pname;
			where.wt_ಬಗೆ = WT_ವಿಧಾನ;
			(ಶೂನ್ಯ)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    ಆದರೆ (i < gap->ga_len)
		fp->uf_defclass = cl;
	    ಆದರೆ (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_builtin_ವಿಧಾನ_ಸೂಚ್ಯಂಕ(class_T *cl)
{
    ಪೂರ್ಣಾಂಕ	i;

    ಗಾಗಿ (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    ಗಾಗಿ (i = 0; i < cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	ಆದರೆ (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cl->class_builtin_methods[CLASS_BUILTIN_ಖಾಲಿಗಳು] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_ಖಾಲಿಗಳು] = i;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
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
ಕಂಡುಹಿಡಿ_class_name_ಕೊನೆ(char_u *arg)
{
    char_u *ಕೊನೆ = arg;

    ಆಗ (ASCII_ISALNUM(*ಕೊನೆ) || *ಕೊನೆ == '_'
	    || (*ಕೊನೆ == '.' && (ASCII_ISALNUM(ಕೊನೆ[1]) || ಕೊನೆ[1] == '_')))
	++ಕೊನೆ;

    ಹಿಂತಿರುಗಿ ಕೊನೆ;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_duplicate_enum(
    garray_T	*enum_gap,
    char_u	*varname,
    char_u	*varname_ಕೊನೆ)
{
    string_T	name = {varname, (size_t)(varname_ಕೊನೆ - varname)};  // Note: the .string field may
								    // point to a string longer
								    // than the .length field.
								    // So we need to use STRNCMP()
								    // to compare it.
    ಪೂರ್ಣಾಂಕ		dup = FALSE;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_ದತ್ತಾಂಶ) + i;

	ಆದರೆ (name.ಉದ್ದಳತೆ == m->ocm_name.ಉದ್ದಳತೆ
	    && STRNCMP(name.string, m->ocm_name.string, name.ಉದ್ದಳತೆ) == 0)
	{
	    char_u  save_c = *varname_ಕೊನೆ;

	    *varname_ಕೊನೆ = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_ಕೊನೆ = save_c;
	    dup = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಹಿಂತಿರುಗಿ dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
enum_parse_ಮೌಲ್ಯಗಳು(
    exarg_T	*eap,
    class_T	*en,
    char_u	*ಸಾಲು,
    garray_T	*gap,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು,
    ಪೂರ್ಣಾಂಕ		*enum_ಕೊನೆ)
{
    evalarg_T	evalarg;
    char_u	*p = ಸಾಲು;
    ಅಕ್ಷರ	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	ಕೊನೆಯ_char = NUL;
    ಪೂರ್ಣಾಂಕ		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
    ಆಗ (*p != NUL)
    {
	// ignore comment
	ಆದರೆ (*p == '#')
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_ಮೌಲ್ಯ_declaration_str), p);
	    ನಿಲ್ಲಿಸು;
	}

	char_u *eni_name_ಆರಂಭ = p;
	char_u *eni_name_ಕೊನೆ = ಗಮ್ಯ_name_ಕೊನೆ(p, FALSE);

	ಆದರೆ (is_duplicate_enum(gap, eni_name_ಆರಂಭ, eni_name_ಕೊನೆ))
	    ನಿಲ್ಲಿಸು;

	p = skipwhite(eni_name_ಕೊನೆ);

	char_u	*init_ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
	ಆದರೆ (*p == '(' || *p == '<')
	{
	    ಆದರೆ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", ಸಾಲು);
		ನಿಲ್ಲಿಸು;
	    }

	    char_u *ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, *ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ;

	    p = eni_name_ಆರಂಭ;
	    (ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ_concatenate(&p, &ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, &ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ, &evalarg);

	    ಆಗ (*ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ && *ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ != '(' && *ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ != '<')
		ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ++;

	    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ > ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ)
		init_ಅಭಿವ್ಯಕ್ತಿ = vim_strnsave(ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ - ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ);
	}

	ಆದರೆ (init_ಅಭಿವ್ಯಕ್ತಿ == ಖಾಲಿ)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, ಗಾತ್ರ(initexpr_buf), "%s.new()", en->class_name.string);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, ಗಾತ್ರ(initexpr_buf), "%s.new%s", en->class_name.string,
		init_ಅಭಿವ್ಯಕ್ತಿ);
	    vim_free(init_ಅಭಿವ್ಯಕ್ತಿ);
	}
	ಆದರೆ (ಕೂಡಿಸು_member(gap, eni_name_ಆರಂಭ, eni_name_ಕೊನೆ, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    ನಿಲ್ಲಿಸು;

	++*ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು;

	ಆದರೆ (*p != '#')
	    ಕೊನೆಯ_char = *p;

	ಆದರೆ (*p != NUL && *p != ',')
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (*p == ',')
	{
	    ಆದರೆ (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", ಸಾಲು);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", ಸಾಲು);
		ನಿಲ್ಲಿಸು;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    ಆದರೆ (*p != NUL && *p != '#')
    {
	ಆದರೆ (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_ವಾದ_str), p);
	rc = FAIL;
    }

    ಆದರೆ (ಕೊನೆಯ_char != ',')
	// last enum value should not be terminated by ","
	*enum_ಕೊನೆ = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
enum_ಕೂಡಿಸು_ಮೌಲ್ಯಗಳು_member(
    class_T	*en,
    garray_T	*gap,
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    ಪೂರ್ಣಾಂಕ		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_ದತ್ತಾಂಶ) + i;

	ಆದರೆ (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.ಉದ್ದಳತೆ);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.ಉದ್ದಳತೆ);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    ಆದರೆ (type == ಖಾಲಿ)
	ಹೋಗು ಮುಗಿದ;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    ಆದರೆ (type->tt_member != ಖಾಲಿ)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = ಕೂಡಿಸು_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_ದತ್ತಾಂಶ, fga.ga_len));

ಮುಗಿದ:
    vim_free(fga.ga_ದತ್ತಾಂಶ);

    ಹಿಂತಿರುಗಿ rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
enum_clear_constructors(class_T *en)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < en->class_class_function_ಎಣಿಕೆ; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	ಆದರೆ (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    ಶೂನ್ಯ
enum_ಗಣ_ಆಂತರಿಕ_obj_vars(class_T *en, object_T *enval)
{
    ಪೂರ್ಣಾಂಕ	i;

    ಗಾಗಿ (i = 0; i < en->class_class_member_ಎಣಿಕೆ; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	ಆದರೆ (en_tv != ಖಾಲಿ && en_tv->v_type == VAR_UNKNOWN)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (i >= en->class_class_member_ಎಣಿಕೆ)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	ಹಿಂತಿರುಗಿ;

    // First object variable is the name
    ocmember_T *ಮೌಲ್ಯ_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(ಮೌಲ್ಯ_ocm->ocm_name.string, ಮೌಲ್ಯ_ocm->ocm_name.ಉದ್ದಳತೆ);

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
    ಗಾಗಿ (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	ಆದರೆ (theline == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	char_u	*p = theline;
	ಪೂರ್ಣಾಂಕ	ಸಿಕ್ಕಿತು;

	// skip ':' and blanks
	ಗಾಗಿ (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	ಸಿಕ್ಕಿತು = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	ಆದರೆ (ಸಿಕ್ಕಿತು)
	    ನಿಲ್ಲಿಸು;
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
    ಪೂರ್ಣಾಂಕ		is_class = eap->cmdidx == CMD_class;
    ಪೂರ್ಣಾಂಕ		is_abstract = eap->cmdidx == CMD_abstract;
    ಪೂರ್ಣಾಂಕ		is_enum = eap->cmdidx == CMD_enum;
    ಪೂರ್ಣಾಂಕ		added_enum_ಮೌಲ್ಯಗಳು = FALSE;
    ಪೂರ್ಣಾಂಕ		is_interface;
    ದೀರ್ಘ	ಆರಂಭ_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    ಆದರೆ (eap->skip)
    {
	skip_class_body(eap);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (is_abstract)
    {
	ಆದರೆ (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_ವಾದ_str), arg);
	    ಹಿಂತಿರುಗಿ;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    ಆದರೆ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	ಆದರೆ (is_class)
	    emsg(_(e_class_can_only_be_defined_ಒಳ_vim9_script));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_enum)
	    emsg(_(e_enum_can_only_be_defined_ಒಳ_vim9_script));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_interface_can_only_be_defined_ಒಳ_vim9_script));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!ASCII_ISUPPER(*arg))
    {
	ಆದರೆ (is_class)
	    semsg(_(e_class_name_must_ಆರಂಭ_with_uppercase_letter_str), arg);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_enum)
	    semsg(_(e_enum_name_must_ಆರಂಭ_with_uppercase_letter_str), arg);
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_interface_name_must_ಆರಂಭ_with_uppercase_letter_str),
									  arg);
	ಹಿಂತಿರುಗಿ;
    }
    char_u *name_ಕೊನೆ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(arg, ಖಾಲಿ, ಖಾಲಿ, FNE_CHECK_ಆರಂಭ);
    ಆದರೆ (!IS_WHITE_OR_NUL(*name_ಕೊನೆ))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	ಹಿಂತಿರುಗಿ;
    }
    char_u *name_ಆರಂಭ = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = ಖಾಲಿ;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, ಗಾತ್ರ(char_u *), 5);

    arg = skipwhite(name_ಕೊನೆ);
    ಆಗ (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	ಆದರೆ (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    ಆದರೆ (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		ಹೋಗು early_ret;
	    }
	    ಆದರೆ (extends != ಖಾಲಿ)
	    {
		emsg(_(e_duplicate_extends));
		ಹೋಗು early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *ಕೊನೆ = ಕಂಡುಹಿಡಿ_class_name_ಕೊನೆ(arg);
	    ಆದರೆ (!IS_WHITE_OR_NUL(*ಕೊನೆ))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		ಹೋಗು early_ret;
	    }
	    extends = vim_strnsave(arg, ಕೊನೆ - arg);
	    ಆದರೆ (extends == ಖಾಲಿ)
		ಹೋಗು early_ret;

	    arg = skipwhite(ಕೊನೆ + 1);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    ಆದರೆ (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		ಹೋಗು early_ret;
	    }

	    ಆದರೆ (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		ಹೋಗು early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    ಗಾಗಿ (;;)
	    {
		char_u *impl_ಕೊನೆ = ಕಂಡುಹಿಡಿ_class_name_ಕೊನೆ(arg);
		ಆದರೆ ((!IS_WHITE_OR_NUL(*impl_ಕೊನೆ) && *impl_ಕೊನೆ != ',')
			|| (*impl_ಕೊನೆ == ','
			    && !IS_WHITE_OR_NUL(*(impl_ಕೊನೆ + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    ಹೋಗು early_ret;
		}
		ಆದರೆ (impl_ಕೊನೆ - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    ಹೋಗು early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_ಕೊನೆ - arg);
		ಆದರೆ (iname == ಖಾಲಿ)
		    ಹೋಗು early_ret;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ga_impl.ga_len; ++i)
		    ಆದರೆ (STRCMP(((char_u **)ga_impl.ga_ದತ್ತಾಂಶ)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			ಹೋಗು early_ret;
		    }
		ಆದರೆ (ga_ಕೂಡಿಸು_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    ಹೋಗು early_ret;
		}
		ಆದರೆ (*impl_ಕೊನೆ != ',')
		{
		    arg = skipwhite(impl_ಕೊನೆ);
		    ನಿಲ್ಲಿಸು;
		}
		arg = skipwhite(impl_ಕೊನೆ + 1);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga_impl);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, ಗಾತ್ರ(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, ಗಾತ್ರ(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, ಗಾತ್ರ(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, ಗಾತ್ರ(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, ಗಾತ್ರ(ufunc_T *), 10);

    class_T *cl = ಖಾಲಿ;
    class_T *extends_cl = ಖಾಲಿ;  // class from "extends" argument
    class_T **intf_classes = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ	    intf_ಎಣಿಕೆ = 0;	// number of items in "intf_classes"
    ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    ಆದರೆ (cl == ಖಾಲಿ)
	ಹೋಗು cleanup;

    ಆದರೆ (is_enum)
	cl->class_flags = CLASS_ENUM;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.ಉದ್ದಳತೆ = (size_t)(name_ಕೊನೆ - name_ಆರಂಭ);
    cl->class_name.string = vim_strnsave(name_ಆರಂಭ, cl->class_name.ಉದ್ದಳತೆ);
    ಆದರೆ (cl->class_name.string == ಖಾಲಿ)
	ಹೋಗು cleanup;

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
    SOURCING_LNUM = ಆರಂಭ_lnum;
    ಪೂರ್ಣಾಂಕ rc = ಗಣ_var_const(cl->class_name.string, 0, ಖಾಲಿ, &tv, FALSE, 0, 0);
    ಆದರೆ (rc == FAIL)
	ಹೋಗು cleanup;

    ಆದರೆ (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	ಆದರೆ (ಕೂಡಿಸು_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, ಖಾಲಿ) == FAIL)
	    ಹೋಗು cleanup;

	varname = (char_u *)"ordinal";
	ಆದರೆ (ಕೂಡಿಸು_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_ಸಂಖ್ಯೆ, ಖಾಲಿ) == FAIL)
	    ಹೋಗು cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    ಪೂರ್ಣಾಂಕ		enum_ಕೊನೆ = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ success = FALSE;
    ಗಾಗಿ (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	ಆದರೆ (theline == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	char_u *ಸಾಲು = skipwhite(theline);

	// Skip empty and comment lines.
	ಆದರೆ (*ಸಾಲು == NUL)
	    ಮುಂದುವರಿ;
	ಆದರೆ (*ಸಾಲು == '#')
	{
	    ಆದರೆ (vim9_bad_comment(ಸಾಲು))
		ನಿಲ್ಲಿಸು;
	    ಮುಂದುವರಿ;
	}

	char_u *p = ಸಾಲು;

	ಅಕ್ಷರ	*ಕೊನೆ_name;
	ಪೂರ್ಣಾಂಕ	shortlen;
	ಪೂರ್ಣಾಂಕ	fullen;
	ಆದರೆ (is_class)
	{
	    ಕೊನೆ_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_enum)
	{
	    ಕೊನೆ_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಕೊನೆ_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	ಗಾಗಿ (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_ಆರಂಭ = p;
	ಆದರೆ (checkforcmd(&p, ಕೊನೆ_name, shortlen))
	{
	    ಆದರೆ (STRNCMP(cmd_ಆರಂಭ, ಕೊನೆ_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_ಆರಂಭ);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '|' || !ತುದಿಗಳು_excmd2(ಸಾಲು, p))
		semsg(_(e_trailing_characters_str), p);
	    ಇಲ್ಲದಿದ್ದರೆ
		success = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

	ಪೂರ್ಣಾಂಕ	wrong_endname = FALSE;
	ಆದರೆ (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	ಇಲ್ಲದಿದ್ದರೆ
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	ಆದರೆ (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), ಸಾಲು, ಕೊನೆ_name);
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (is_enum && !enum_ಕೊನೆ)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    ಆದರೆ (enum_parse_ಮೌಲ್ಯಗಳು(eap, cl, ಸಾಲು, &classmembers,
					&ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು, &enum_ಕೊನೆ) == FAIL)
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (enum_ಕೊನೆ)
	    {
		// Add the enum "values" class variable.
		enum_ಕೂಡಿಸು_ಮೌಲ್ಯಗಳು_member(cl, &classmembers, ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು,
							&type_list);
		added_enum_ಮೌಲ್ಯಗಳು = TRUE;
	    }
	    ಮುಂದುವರಿ;
	}

	ಪೂರ್ಣಾಂಕ has_ಬಹಿರಂಗ = FALSE;
	ಆದರೆ (checkforcmd(&p, "public", 3))
	{
	    ಆದರೆ (STRNCMP(ಸಾಲು, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ಸಾಲು);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (is_interface)
	    {
		emsg(_(e_ಬಹಿರಂಗ_variable_not_supported_ಒಳ_interface));
		ನಿಲ್ಲಿಸು;
	    }
	    has_ಬಹಿರಂಗ = TRUE;
	    p = skipwhite(ಸಾಲು + 6);

	    ಆದರೆ (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_ಬಹಿರಂಗ_keyword_not_supported_for_ವಿಧಾನ));
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_ಬಹಿರಂಗ_must_be_followed_by_var_static_final_or_const));
		ನಿಲ್ಲಿಸು;
	    }
	}

	ಪೂರ್ಣಾಂಕ abstract_ವಿಧಾನ = FALSE;
	char_u *pa = p;
	ಆದರೆ (checkforcmd(&p, "abstract", 3))
	{
	    ಆದರೆ (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_ಬಳಸಿದ_ಒಳ_enum));
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_ಬಳಸಿದ_ಒಳ_interface));
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (!is_abstract)
	    {
		semsg(_(e_abstract_ವಿಧಾನ_ಒಳ_concrete_class), pa);
		ನಿಲ್ಲಿಸು;
	    }

	    p = skipwhite(pa + 8);
	    ಆದರೆ (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		ನಿಲ್ಲಿಸು;
	    }

	    abstract_ವಿಧಾನ = TRUE;
	}

	ಪೂರ್ಣಾಂಕ has_static = FALSE;
	char_u *ps = p;
	ಆದರೆ (checkforcmd(&p, "static", 4))
	{
	    ಆದರೆ (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (is_interface)
	    {
		emsg(_(e_static_member_not_supported_ಒಳ_interface));
		ನಿಲ್ಲಿಸು;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    ಆದರೆ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		ನಿಲ್ಲಿಸು;
	    }
	}

	ಪೂರ್ಣಾಂಕ has_final = FALSE;
	ಪೂರ್ಣಾಂಕ has_var = FALSE;
	ಪೂರ್ಣಾಂಕ has_const = FALSE;
	ಆದರೆ (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (checkforcmd(&p, "final", 5))
	{
	    ಆದರೆ (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_ಒಳ_interface));
		ನಿಲ್ಲಿಸು;
	    }
	    has_final = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (checkforcmd(&p, "const", 5))
	{
	    ಆದರೆ (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_ಒಳ_interface));
		ನಿಲ್ಲಿಸು;
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
	ಆದರೆ (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_ಕೊನೆ = ಖಾಲಿ;
	    type_T *type = ಖಾಲಿ;
	    char_u *init_ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
	    ಪೂರ್ಣಾಂಕ	    has_type = FALSE;

	    ಆದರೆ (!eval_isnamec1(*p))
	    {
		ಆದರೆ (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), ಸಾಲು);
		ಇಲ್ಲದಿದ್ದರೆ
		    semsg(_(e_invalid_object_variable_declaration_str), ಸಾಲು);
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_ಒಳ_interface),
			varname);
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (parse_member(eap, ಸಾಲು, varname, has_ಬಹಿರಂಗ,
			  &varname_ಕೊನೆ, &has_type, &type_list, &type,
			  !is_interface ? &init_ಅಭಿವ್ಯಕ್ತಿ: ಖಾಲಿ) == FAIL)
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (is_reserved_varname(varname, varname_ಕೊನೆ)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_ಕೊನೆ))
	    {
		vim_free(init_ಅಭಿವ್ಯಕ್ತಿ);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (ಕೂಡಿಸು_member(has_static ? &classmembers : &objmembers, varname,
				varname_ಕೊನೆ, has_ಬಹಿರಂಗ, has_final, has_const,
				has_type, type, init_ಅಭಿವ್ಯಕ್ತಿ) == FAIL)
	    {
		vim_free(init_ಅಭಿವ್ಯಕ್ತಿ);
		ನಿಲ್ಲಿಸು;
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
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	ಸಾಲುಗಳು_ಗಮ್ಯ_free;
	    ಪೂರ್ಣಾಂಕ		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    ಆದರೆ (has_ಬಹಿರಂಗ)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_ಬಹಿರಂಗ_keyword_not_supported_for_ವಿಧಾನ));
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_ಒಳ_class_str), ಸಾಲು);
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_ವಿಧಾನ_not_supported_ಒಳ_interface), p);
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (has_static && !is_new && ಸುರಕ್ಷಿತ_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_ವಿಧಾನ_not_supported), p);
		ನಿಲ್ಲಿಸು;
	    }

	    CLEAR_ಕ್ಷೇತ್ರ(ea);
	    ea.cmd = ಸಾಲು;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&ಸಾಲುಗಳು_ಗಮ್ಯ_free, ಗಾತ್ರ(char_u *), 50);
	    ಪೂರ್ಣಾಂಕ class_flags;
	    ಆದರೆ (is_interface)
		class_flags = CF_INTERFACE;
	    ಇಲ್ಲದಿದ್ದರೆ
		class_flags = abstract_ವಿಧಾನ ? CF_ABSTRACT_ವಿಧಾನ : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, ಖಾಲಿ, &ಸಾಲುಗಳು_ಗಮ್ಯ_free,
			class_flags, objmembers.ga_ದತ್ತಾಂಶ, objmembers.ga_len,
			ಖಾಲಿ);
	    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಸಾಲುಗಳು_ಗಮ್ಯ_free);

	    ಆದರೆ (uf != ಖಾಲಿ)
	    {
		ಆದರೆ (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_ವಿಧಾನ_not_supported_ಒಳ_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    ನಿಲ್ಲಿಸು;
		}

		// check for builtin method
		ಆದರೆ (!is_new && ಸುರಕ್ಷಿತ_islower(*uf->uf_name) &&
					!object_check_builtin_ವಿಧಾನ_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    ನಿಲ್ಲಿಸು;
		}

		// Check the uf_name isn't used already.
		ಆದರೆ (is_duplicate_ವಿಧಾನ(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    ನಿಲ್ಲಿಸು;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		ಆದರೆ (ga_grow(fgap, 1) == OK)
		{
		    ಆದರೆ (is_new)
			uf->uf_flags |= FC_NEW;

		    ಆದರೆ (abstract_ವಿಧಾನ)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_ದತ್ತಾಂಶ)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (is_class)
		semsg(_(e_not_valid_command_ಒಳ_class_str), ಸಾಲು);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_enum)
		semsg(_(e_not_valid_command_ಒಳ_enum_str), ಸಾಲು);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_not_valid_command_ಒಳ_interface_str), ಸಾಲು);
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (theline == ಖಾಲಿ && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    ಆದರೆ (success && is_enum && (ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು == 0 || !added_enum_ಮೌಲ್ಯಗಳು))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_ಕೂಡಿಸು_ಮೌಲ್ಯಗಳು_member(cl, &classmembers, ಅಂಕೆ_enum_ಮೌಲ್ಯಗಳು,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    ಆದರೆ (success && extends != ಖಾಲಿ)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    ಆದರೆ (success && extends_cl != ಖಾಲಿ)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    ಆದರೆ (success && extends_cl != ಖಾಲಿ)
    {
	ಆದರೆ (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	ಇಲ್ಲದಿದ್ದರೆ
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    ಆದರೆ (success && !is_abstract && extends_cl != ಖಾಲಿ
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, ಗಾತ್ರ(class_T *), 5);

    ಆದರೆ (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    ಆದರೆ (success && extends_cl != ಖಾಲಿ)
	success = ಕೂಡಿಸು_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_ದತ್ತಾಂಶ;
    intf_ಎಣಿಕೆ = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    ಆದರೆ (success)
	success = check_func_arg_ಹೆಸರುಗಳು(&classfunctions, &objmethods,
							&classmembers);

    ಆದರೆ (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	ಆದರೆ (extends_cl != ಖಾಲಿ)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	ಆದರೆ (ಕೂಡಿಸು_members_ಗಮ್ಯ_class(&classmembers,
				 ಖಾಲಿ,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_ಎಣಿಕೆ) == FAIL
		|| ಕೂಡಿಸು_members_ಗಮ್ಯ_class(&objmembers,
				 extends_cl == ಖಾಲಿ ? ಖಾಲಿ
					       : extends_cl->class_obj_members,
				 extends_cl == ಖಾಲಿ ? 0
					  : extends_cl->class_obj_member_ಎಣಿಕೆ,
				 &cl->class_obj_members,
				 &cl->class_obj_member_ಎಣಿಕೆ) == FAIL)
	    ಹೋಗು cleanup;

	ಆದರೆ (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_ಎಣಿಕೆ = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    ಆದರೆ (cl->class_interfaces == ಖಾಲಿ)
		ಹೋಗು cleanup;
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_ದತ್ತಾಂಶ)[i];
	    VIM_CLEAR(ga_impl.ga_ದತ್ತಾಂಶ);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = ಖಾಲಿ;
	}

	ಆದರೆ (cl->class_interface_ಎಣಿಕೆ > 0 || extends_cl != ಖಾಲಿ)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    ಆದರೆ (ಕೂಡಿಸು_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		ಹೋಗು cleanup;
	}

	ಪೂರ್ಣಾಂಕ	have_new = FALSE;
	ufunc_T	*class_func = ಖಾಲಿ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_ದತ್ತಾಂಶ)[i];
	    ಆದರೆ (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}

	ಆದರೆ (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    ಕೂಡಿಸು_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	ಆದರೆ (ಕೂಡಿಸು_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    ಹೋಗು cleanup;

	update_builtin_ವಿಧಾನ_ಸೂಚ್ಯಂಕ(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	ಆದರೆ ((is_class || is_enum) && cl->class_class_member_ಎಣಿಕೆ > 0)
	    ಆದರೆ (ಕೂಡಿಸು_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		ಹಿಂತಿರುಗಿ;
	    }

	cl->class_type_list = type_list;

	ಆದರೆ (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	ಹಿಂತಿರುಗಿ;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    ಆದರೆ (intf_classes != ಖಾಲಿ)
    {
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < intf_ಎಣಿಕೆ; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga_impl);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	garray_T *gap = ಸುತ್ತು == 1 ? &classmembers : &objmembers;
	ಆದರೆ (gap->ga_len == 0 || gap->ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_ದತ್ತಾಂಶ) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_ದತ್ತಾಂಶ)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < classfunctions.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)classfunctions.ga_ದತ್ತಾಂಶ)[i];
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
    ಪೂರ್ಣಾಂಕ		is_object,
    char_u	*name,
    char_u	*name_ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		*member_idx)
{
    size_t	len = name_ಕೊನೆ - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    ಆದರೆ (m == ಖಾಲಿ)
    {
	member_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	ಹಿಂತಿರುಗಿ &t_any;
    }

    ಹಿಂತಿರುಗಿ m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    ಪೂರ್ಣಾಂಕ		is_object,
    ಪೂರ್ಣಾಂಕ		member_idx)
{
    ocmember_T	*m;
    ಪೂರ್ಣಾಂಕ		member_ಎಣಿಕೆ;

    ಆದರೆ (is_object)
    {
	m = cl->class_obj_members;
	member_ಎಣಿಕೆ = cl->class_obj_member_ಎಣಿಕೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	m = cl->class_class_members;
	member_ಎಣಿಕೆ = cl->class_class_member_ಎಣಿಕೆ;
    }

    ಆದರೆ (member_idx >= member_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ m[member_idx].ocm_type;
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
    ಆದರೆ (ta != ಖಾಲಿ && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    ಶೂನ್ಯ
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    ಆದರೆ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_ಒಳ_vim9_script));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_ಆರಂಭ_with_uppercase_letter_str), arg);
	ಹಿಂತಿರುಗಿ;
    }

    char_u *name_ಕೊನೆ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(arg, ಖಾಲಿ, ಖಾಲಿ, FNE_CHECK_ಆರಂಭ);
    ಆದರೆ (!IS_WHITE_OR_NUL(*name_ಕೊನೆ))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	ಹಿಂತಿರುಗಿ;
    }
    char_u *name_ಆರಂಭ = arg;

    arg = skipwhite(name_ಕೊನೆ);
    ಆದರೆ (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	ಹಿಂತಿರುಗಿ;
    }
    arg++;
    arg = skipwhite(arg);

    ಆದರೆ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	ಹಿಂತಿರುಗಿ;
    }

    scriptitem_T    *si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, ಖಾಲಿ, ಖಾಲಿ, TRUE);
    ಆದರೆ (type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	ಹಿಂತಿರುಗಿ;
    }

    ಪೂರ್ಣಾಂಕ cc = *name_ಕೊನೆ;
    *name_ಕೊನೆ = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    ಆದರೆ (eval_variable_import(name_ಆರಂಭ, &tv) == OK)
    {
	ಆದರೆ (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_ಆರಂಭ);
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_redefining_script_ಅಂಶ_str), name_ಆರಂಭ);
	clear_tv(&tv);
	ಹೋಗು ಮುಗಿದ;
    }

    // Create a script-local variable for the type alias.
    ಆದರೆ (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_ಬೀಗ = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_ಆರಂಭ);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_ಆಳ = parse_type_ಆಳ();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_ಬೀಗ = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    ಗಣ_var_const(name_ಆರಂಭ, current_sctx.sc_sid, ಖಾಲಿ, &tv, FALSE,
						ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL, 0);

ಮುಗಿದ:
    *name_ಕೊನೆ = cc;
}

/*
 * Returns OK if a member variable named "name" is present in the class "cl".
 * Otherwise returns FAIL.  If found, the member variable typval is set in
 * "rettv".  If "is_object" is TRUE, then the object member variable table is
 * searched.  Otherwise the class member variable table is searched.
 */
    ಪೂರ್ಣಾಂಕ
get_member_tv(
    class_T	*cl,
    ಪೂರ್ಣಾಂಕ		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    ಪೂರ್ಣಾಂಕ		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    ಆದರೆ (m == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (*name == '_' && (current_class == ಖಾಲಿ ||
				!class_instance_ದ(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	ಗಣ_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	ಗಣ_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
call_oc_ವಿಧಾನ(
    class_T	*cl,
    char_u	*name,
    size_t	len,
    char_u	*name_ಕೊನೆ,
    evalarg_T	*evalarg,
    char_u	**arg,
    gfargs_tab_T	*gfatab,	// generic types
    typval_T	*rettv)
{
    ufunc_T	*fp;
    typval_T	argvars[MAX_FUNC_ವಾದಗಳು + 1];
    ಪೂರ್ಣಾಂಕ		argcount = 0;
    ocmember_T	*ocm = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		m_idx;

    fp = ವಿಧಾನ_lookup(cl, rettv->v_type, name, len, ಖಾಲಿ);
    ಆದರೆ (fp == ಖಾಲಿ)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	ಆದರೆ (ocm == ಖಾಲಿ || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    ವಿಧಾನ_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(cl, rettv->v_type, name, len);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_ಕೊನೆ;
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (ocm == ಖಾಲಿ && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_ವಿಧಾನ_str), fp->uf_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // process generic function call
    ಆದರೆ (fp != ಖಾಲಿ)
    {
	fp = generic_func_get(fp, gfatab);
	ಆದರೆ (fp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    char_u *argp = name_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    ಆದರೆ (ret == FAIL)
    {
	ಆಗ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    funcexe_T funcexe;
    CLEAR_ಕ್ಷೇತ್ರ(funcexe);
    funcexe.fe_evaluate = TRUE;
    ಆದರೆ (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    ಆದರೆ (evalarg != ಖಾಲಿ)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    ಪೂರ್ಣಾಂಕ error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								ಖಾಲಿ);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ಆದರೆ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_ಸಿಕ್ಕಿತು_var);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    *arg = argp;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ಪೂರ್ಣಾಂಕ
obj_ವಿಧಾನ_ಗಮ್ಯ_partial_tv(object_T *obj, ufunc_T *obj_ವಿಧಾನ, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    ಆದರೆ (pt == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    pt->pt_refcount = 1;
    ಆದರೆ (obj != ಖಾಲಿ)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_ವಿಧಾನ;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ
class_object_ಸೂಚ್ಯಂಕ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಂಕ		verbose ಬಳಸದ)	// give error messages
{
    ಪೂರ್ಣಾಂಕ	ret = FAIL;

    ಆದರೆ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_ಕೊನೆ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(name, ಖಾಲಿ, ಖಾಲಿ, FNE_CHECK_ಆರಂಭ);
    ಆದರೆ (name_ಕೊನೆ == name)
	ಹಿಂತಿರುಗಿ FAIL;
    size_t len = name_ಕೊನೆ - name;

    ಪೂರ್ಣಾಂಕ did_emsg_save = did_emsg;
    class_T *cl;
    ಆದರೆ (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    ಇಲ್ಲದಿದ್ದರೆ // VAR_OBJECT
    {
	ಆದರೆ (rettv->vval.v_object == ಖಾಲಿ)
	{
	    emsg(_(e_using_null_object));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    ಆದರೆ (cl == ಖಾಲಿ)
    {
	emsg(_(e_incomplete_type));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_init(&gfatab);

    ಆದರೆ (*name_ಕೊನೆ == '<')
    {
	cctx_T	    *cctx = ಖಾಲಿ;

	ಆದರೆ (evalarg != ಖಾಲಿ)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_ಕೊನೆ = parse_generic_func_type_ವಾದಗಳು(name, len, name + len,
						&gfatab, cctx);
	ಆದರೆ (name_ಕೊನೆ == ಖಾಲಿ)
	    ಹೋಗು ಮುಗಿದ;
    }

    ಆದರೆ (*name_ಕೊನೆ == '(')
	// Invoke the class or object method
	ret = call_oc_ವಿಧಾನ(cl, name, len, name_ಕೊನೆ, evalarg, arg,
							&gfatab, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	ಪೂರ್ಣಾಂಕ is_object = rettv->v_type == VAR_OBJECT;
	ಆದರೆ (get_member_tv(cl, is_object, name, len, ಖಾಲಿ, rettv) == OK)
	{
	    *arg = name_ಕೊನೆ;
	    ret = OK;
	    ಹೋಗು ಮುಗಿದ;
	}

	// could be a class method or an object method
	ಪೂರ್ಣಾಂಕ	fidx;
	ufunc_T	*fp = ವಿಧಾನ_lookup(cl, rettv->v_type, name, len, &fidx);
	ಆದರೆ (fp != ಖಾಲಿ)
	{
	    // Protected methods are not accessible outside the class
	    ಆದರೆ (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_ವಿಧಾನ_str), fp->uf_name);
		ಹೋಗು ಮುಗಿದ;
	    }

	    ಆದರೆ (obj_ವಿಧಾನ_ಗಮ್ಯ_partial_tv(is_object ? rettv->vval.v_object :
						ಖಾಲಿ, fp, rettv) == FAIL)
		ಹೋಗು ಮುಗಿದ;

	    *arg = name_ಕೊನೆ;
	    ret = OK;
	    ಹೋಗು ಮುಗಿದ;
	}

	ಆದರೆ (did_emsg == did_emsg_save)
	    member_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(cl, rettv->v_type, name, len);
    }

ಮುಗಿದ:
    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_clear(&gfatab);

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
ಕಂಡುಹಿಡಿ_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_ಕೊನೆ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(name, ಖಾಲಿ, ಖಾಲಿ, FNE_CHECK_ಆರಂಭ);
    ಆದರೆ (name_ಕೊನೆ == name || *name_ಕೊನೆ != '.')
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ufunc_T	*fp = ಖಾಲಿ;
    size_t	len = name_ಕೊನೆ - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    ಆದರೆ (eval_variable(name, (ಪೂರ್ಣಾಂಕ)len,
				    0, &tv, ಖಾಲಿ, EVAL_VAR_NOAUTOLOAD) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	ಹೋಗು fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    ಆದರೆ (cl == ಖಾಲಿ)
	ಹೋಗು fail_after_eval;
    char_u *fname = name_ಕೊನೆ + 1;
    char_u *fname_ಕೊನೆ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(fname, ಖಾಲಿ, ಖಾಲಿ, FNE_CHECK_ಆರಂಭ);
    ಆದರೆ (fname_ಕೊನೆ == fname)
	ಹೋಗು fail_after_eval;
    len = fname_ಕೊನೆ - fname;

    fp = ವಿಧಾನ_lookup(cl, tv.v_type, fname, len, ಖಾಲಿ);

    ಆದರೆ (fp != ಖಾಲಿ)
    {
	fp = eval_generic_func(fp, fname, &fname_ಕೊನೆ);
	ಆದರೆ (fp != ಖಾಲಿ)
	    *arg = fname_ಕೊನೆ;
    }

fail_after_eval:
    clear_tv(&tv);
    ಹಿಂತಿರುಗಿ fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ಪೂರ್ಣಾಂಕ
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    ಪೂರ್ಣಾಂಕ idx;
    class_member_lookup(cl, name, namelen, &idx);
    ಹಿಂತಿರುಗಿ idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, ಪೂರ್ಣಾಂಕ *idx)
{
    ocmember_T	*ret_m = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ret_idx = -1;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_class_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	ಆದರೆ (namelen)
	{
	    ಆದರೆ (namelen == m->ocm_name.ಉದ್ದಳತೆ
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (idx != ಖಾಲಿ)
	*idx = ret_idx;
    ಹಿಂತಿರುಗಿ ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    ಸ್ಥಿರ ufunc_T *
class_ವಿಧಾನ_lookup(class_T *cl, char_u *name, size_t namelen, ಪೂರ್ಣಾಂಕ *idx)
{
    ufunc_T	*ret_fp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ret_idx = -1;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_class_function_ಎಣಿಕೆ; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	ಆದರೆ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (idx != ಖಾಲಿ)
	*idx = ret_idx;
    ಹಿಂತಿರುಗಿ ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    ಪೂರ್ಣಾಂಕ
class_ವಿಧಾನ_idx(class_T *cl, char_u *name, size_t namelen)
{
    ಪೂರ್ಣಾಂಕ idx;
    class_ವಿಧಾನ_lookup(cl, name, namelen, &idx);
    ಹಿಂತಿರುಗಿ idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    ಪೂರ್ಣಾಂಕ idx;
    object_member_lookup(cl, name, namelen, &idx);
    ಹಿಂತಿರುಗಿ idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, ಪೂರ್ಣಾಂಕ *idx)
{
    ocmember_T	*ret_m = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ret_idx = -1;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_obj_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	ಆದರೆ (namelen)
	{
	    ಆದರೆ (namelen == m->ocm_name.ಉದ್ದಳತೆ
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (idx != ಖಾಲಿ)
	*idx = ret_idx;
    ಹಿಂತಿರುಗಿ ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    ಸ್ಥಿರ ufunc_T *
object_ವಿಧಾನ_lookup(class_T *cl, char_u *name, size_t namelen, ಪೂರ್ಣಾಂಕ *idx)
{
    ufunc_T	*ret_fp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ret_idx = -1;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	ಆದರೆ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (idx != ಖಾಲಿ)
	*idx = ret_idx;
    ಹಿಂತಿರುಗಿ ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    ಪೂರ್ಣಾಂಕ
object_ವಿಧಾನ_idx(class_T *cl, char_u *name, size_t namelen)
{
    ಪೂರ್ಣಾಂಕ idx;
    object_ವಿಧಾನ_lookup(cl, name, namelen, &idx);
    ಹಿಂತಿರುಗಿ idx;
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
    ಪೂರ್ಣಾಂಕ		*idx)
{
    ಆದರೆ (v_type == VAR_CLASS)
	ಹಿಂತಿರುಗಿ class_member_lookup(cl, name, namelen, idx);

    ಹಿಂತಿರುಗಿ object_member_lookup(cl, name, namelen, idx);
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
    class_T	*cl_ಸಿಕ್ಕಿತು = ಖಾಲಿ;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_ಸಿಕ್ಕಿತು = ಖಾಲಿ;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    ಗಾಗಿ (class_T *super = cl; super != ಖಾಲಿ; super = super->class_extends)
    {
	class_T		*cl_tmp = ಖಾಲಿ;
	ocmember_T	*m = ಖಾಲಿ;
	ಆದರೆ (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    ಆದರೆ ((m = object_member_lookup(super, name, len, ಖಾಲಿ)) != ಖಾಲಿ)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	ಆದರೆ (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    ಆದರೆ (( m = class_member_lookup(super, name, len, ಖಾಲಿ)) != ಖಾಲಿ)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	ಆದರೆ (cl_tmp == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;  // member is not in this or any super class.
	cl_ಸಿಕ್ಕಿತು = cl_tmp;
	m_ಸಿಕ್ಕಿತು = m;
    }
    ಆದರೆ (p_m != ಖಾಲಿ)
	*p_m = m_ಸಿಕ್ಕಿತು;
    ಹಿಂತಿರುಗಿ cl_ಸಿಕ್ಕಿತು;
}

/*
 * Lookup a class or object method by name.  If v_type is VAR_CLASS, then
 * lookup a class method and if it is VAR_OBJECT, then lookup a object method.
 *
 * Returns a pointer to the method structure if variable is found.
 * Otherwise returns NULL.  The method variable index is set in "*idx".
 */
    ufunc_T *
ವಿಧಾನ_lookup(
    class_T	*cl,
    vartype_T	v_type,
    char_u	*name,
    size_t	namelen,
    ಪೂರ್ಣಾಂಕ		*idx)
{
    ಆದರೆ (v_type == VAR_CLASS)
	ಹಿಂತಿರುಗಿ class_ವಿಧಾನ_lookup(cl, name, namelen, idx);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ object_ವಿಧಾನ_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    ಪೂರ್ಣಾಂಕ
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    ಗಾಗಿ (cctx_T *cctx = cctx_arg; cctx != ಖಾಲಿ; cctx = cctx->ctx_outer)
	ಆದರೆ (cctx->ctx_ufunc != ಖಾಲಿ
			&& class_instance_ದ(cctx->ctx_ufunc->uf_class, cl))
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    ಪೂರ್ಣಾಂಕ
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    ಆದರೆ (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_ಒಳ_class_str),
		m->ocm_name.string, cl->class_name.string);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    ಶೂನ್ಯ
obj_ಬೀಗ_const_vars(object_T *obj)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < obj->obj_class->class_obj_member_ಎಣಿಕೆ; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	ಆದರೆ (ocm->ocm_flags & OCMFLAG_CONST)
	{
	    typval_T *mtv = ((typval_T *)(obj + 1)) + i;
	    ಅಂಶ_ಬೀಗ(mtv, DICT_MAXNEST, TRUE, TRUE);
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
    ಪೂರ್ಣಾಂಕ		sz;

    ಆದರೆ (cl == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    sz = ಗಾತ್ರ(object_T) + cl->class_obj_member_ಎಣಿಕೆ * ಗಾತ್ರ(typval_T);
    obj = alloc_clear(sz);
    ಆದರೆ (obj == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    ಆದರೆ (IS_ENUM(cl))
	enum_ಗಣ_ಆಂತರಿಕ_obj_vars(cl, obj);

    ಹಿಂತಿರುಗಿ obj;
}

/*
 * Make a copy of an object.
 */
    ಶೂನ್ಯ
copy_object(typval_T *from, typval_T *ಗಮ್ಯ)
{
    ಆದರೆ (from->vval.v_object == ಖಾಲಿ)
	ಗಮ್ಯ->vval.v_object = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಮ್ಯ->vval.v_object = from->vval.v_object;
	++ಗಮ್ಯ->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    ಶೂನ್ಯ
copy_class(typval_T *from, typval_T *ಗಮ್ಯ)
{
    ಆದರೆ (from->vval.v_class == ಖಾಲಿ)
	ಗಮ್ಯ->vval.v_class = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಮ್ಯ->vval.v_class = from->vval.v_class;
	++ಗಮ್ಯ->vval.v_class->class_refcount;
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

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_interface_ಎಣಿಕೆ; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	ಆದರೆ (cl->class_interfaces_cl[i] != ಖಾಲಿ)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *ಮುಂದಿನ;
    ಗಾಗಿ (itf2class_T *i2c = cl->class_itf2class; i2c != ಖಾಲಿ; i2c = ಮುಂದಿನ)
    {
	ಮುಂದಿನ = i2c->i2c_ಮುಂದಿನ;
	vim_free(i2c);
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_class_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	ಆದರೆ (cl->class_members_tv != ಖಾಲಿ)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_obj_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_class_function_ಎಣಿಕೆ; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	ಆದರೆ (IS_ENUM(cl) && IS_CONSTRUCTOR_ವಿಧಾನ(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ; ++i)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
class_get_selfrefs(class_T *cl)
{
    ಪೂರ್ಣಾಂಕ		self_refs = 0;
    typval_T	*tv;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_class_member_ಎಣಿಕೆ; ++i)
    {
	tv = &cl->class_members_tv[i];
	ಆದರೆ (tv->v_type == VAR_OBJECT && tv->vval.v_object != ಖಾಲಿ
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    ಹಿಂತಿರುಗಿ self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
can_free_enum(class_T *cl)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_class_member_ಎಣಿಕೆ; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	ಆದರೆ (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    ಹಿಂತಿರುಗಿ TRUE;

	ಆದರೆ (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    ಆದರೆ (tv->vval.v_list->lv_refcount > 1)
		ಹಿಂತಿರುಗಿ FALSE;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    ಹಿಂತಿರುಗಿ FALSE;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
can_free_class(class_T *cl)
{
    ಪೂರ್ಣಾಂಕ		can_free = FALSE;
    ಪೂರ್ಣಾಂಕ		self_refs = 0;

    ಆದರೆ (IS_ENUM(cl) && !can_free_enum(cl))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    ಆದರೆ (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    ಹಿಂತಿರುಗಿ can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    ಶೂನ್ಯ
class_unref(class_T *cl)
{
    ಆದರೆ (cl == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    --cl->class_refcount;

    ಆದರೆ (cl->class_name.string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    ಪೂರ್ಣಾಂಕ
class_free_nonref(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		did_free = FALSE;

    ಗಾಗಿ (class_T *cl = ಮೊದಲ_class; cl != ಖಾಲಿ; cl = ಮುಂದಿನ_nonref_class)
    {
	ಮುಂದಿನ_nonref_class = cl->class_ಮುಂದಿನ_ಬಳಸಿದ;
	ಆದರೆ ((cl->class_copyID & COPYID_ಮುಖವಾಡ) != (copyID & COPYID_ಮುಖವಾಡ))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    ಮುಂದಿನ_nonref_class = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ did_free;
}

    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_classes(ಪೂರ್ಣಾಂಕ copyID)
{
    ಗಾಗಿ (class_T *cl = ಮೊದಲ_class; cl != ಖಾಲಿ; cl = cl->class_ಮುಂದಿನ_ಬಳಸಿದ)
	ಗಣ_ref_ಒಳ_ಅಂಶ_class(cl, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ object_T *ಮೊದಲ_object = ಖಾಲಿ;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    ಶೂನ್ಯ
object_created(object_T *obj)
{
    ಆದರೆ (ಮೊದಲ_object != ಖಾಲಿ)
    {
	obj->obj_ಮುಂದಿನ_ಬಳಸಿದ = ಮೊದಲ_object;
	ಮೊದಲ_object->obj_prev_ಬಳಸಿದ = obj;
    }
    ಮೊದಲ_object = obj;
}

/*
 * Call this function when an object has been cleared and is about to be freed.
 * It is removed from the list headed by "first_object".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
object_cleared(object_T *obj)
{
    ಆದರೆ (obj->obj_ಮುಂದಿನ_ಬಳಸಿದ != ಖಾಲಿ)
	obj->obj_ಮುಂದಿನ_ಬಳಸಿದ->obj_prev_ಬಳಸಿದ = obj->obj_prev_ಬಳಸಿದ;
    ಆದರೆ (obj->obj_prev_ಬಳಸಿದ != ಖಾಲಿ)
	obj->obj_prev_ಬಳಸಿದ->obj_ಮುಂದಿನ_ಬಳಸಿದ = obj->obj_ಮುಂದಿನ_ಬಳಸಿದ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ_object == obj)
	ಮೊದಲ_object = obj->obj_ಮುಂದಿನ_ಬಳಸಿದ;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    ಆದರೆ (!cl)
	ಹಿಂತಿರುಗಿ;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_obj_member_ಎಣಿಕೆ; ++i)
	clear_tv(tv + i);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    ಆದರೆ (!cl)
	ಹಿಂತಿರುಗಿ;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
object_free(object_T *obj)
{
    ಆದರೆ (ಒಳ_free_unref_ವಸ್ತುಗಳು)
	ಹಿಂತಿರುಗಿ;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    ಶೂನ್ಯ
object_unref(object_T *obj)
{
    ಆದರೆ (obj != ಖಾಲಿ && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    ಪೂರ್ಣಾಂಕ
object_free_nonref(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		did_free = FALSE;

    ಗಾಗಿ (object_T *obj = ಮೊದಲ_object; obj != ಖಾಲಿ; obj = obj->obj_ಮುಂದಿನ_ಬಳಸಿದ)
    {
	ಆದರೆ ((obj->obj_copyID & COPYID_ಮುಖವಾಡ) != (copyID & COPYID_ಮುಖವಾಡ))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    ಹಿಂತಿರುಗಿ did_free;
}

    ಶೂನ್ಯ
object_free_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಂಕ copyID)
{
    object_T	*obj_ಮುಂದಿನ;

    ಗಾಗಿ (object_T *obj = ಮೊದಲ_object; obj != ಖಾಲಿ; obj = obj_ಮುಂದಿನ)
    {
	obj_ಮುಂದಿನ = obj->obj_ಮುಂದಿನ_ಬಳಸಿದ;
	ಆದರೆ ((obj->obj_copyID & COPYID_ಮುಖವಾಡ) != (copyID & COPYID_ಮುಖವಾಡ))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    ಶೂನ್ಯ
emsg_var_cl_define(ಅಕ್ಷರ *ಸಂದೇಶ, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    ಆದರೆ (cl_def != ಖಾಲಿ)
	semsg(_(ಸಂದೇಶ), m->ocm_name.string, cl_def->class_name.string);
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_ಆಂತರಿಕ_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    ಶೂನ್ಯ
ವಿಧಾನ_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *ವಿಧಾನ_name = vim_strnsave(name, len);
    ಆದರೆ ((v_type == VAR_OBJECT)
	    && (class_ವಿಧಾನ_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	ಆದರೆ (*name == '_')
	    semsg(_(e_cannot_access_protected_ವಿಧಾನ_str), ವಿಧಾನ_name);
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_class_ವಿಧಾನ_str_accessible_only_using_class_str),
		    ವಿಧಾನ_name, cl->class_name.string);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((v_type == VAR_CLASS)
	    && (object_ವಿಧಾನ_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	ಆದರೆ (*name == '_')
	    semsg(_(e_cannot_access_protected_ವಿಧಾನ_str), ವಿಧಾನ_name);
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_object_ವಿಧಾನ_str_accessible_only_using_object_str),
		    ವಿಧಾನ_name, cl->class_name.string);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_ವಿಧಾನ_not_ಸಿಕ್ಕಿತು_on_class_str_str), ವಿಧಾನ_name,
		cl->class_name.string);
    vim_free(ವಿಧಾನ_name);
}

/*
 * Echo a class or object member not found message.
 */
    ಶೂನ್ಯ
member_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    ಆದರೆ (v_type == VAR_OBJECT)
    {
	ಆದರೆ (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_variable_not_ಸಿಕ್ಕಿತು_on_object_str_str), varname,
		    cl->class_name.string);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (IS_ENUM(cl))
		semsg(_(e_enum_ಮೌಲ್ಯ_str_not_ಸಿಕ್ಕಿತು_ಒಳ_enum_str),
			varname, cl->class_name.string);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_class_variable_str_not_ಸಿಕ್ಕಿತು_ಒಳ_class_str),
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
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ loop = 1; loop <= 2; ++loop)
    {
	ಪೂರ್ಣಾಂಕ func_ಎಣಿಕೆ = loop == 1 ? cl->class_class_function_ಎಣಿಕೆ
						: cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < func_ಎಣಿಕೆ; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    ಆದರೆ (!IS_ABSTRACT_ವಿಧಾನ(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    ಶೂನ್ಯ
defcompile_classes_ಒಳ_script(ಶೂನ್ಯ)
{
    ಗಾಗಿ (class_T *cl = ಮೊದಲ_class; cl != ಖಾಲಿ; cl = cl->class_ಮುಂದಿನ_ಬಳಸಿದ)
    {
	ಆದರೆ (eval_variable(cl->class_name.string, 0, 0, ಖಾಲಿ, ಖಾಲಿ,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    ಪೂರ್ಣಾಂಕ
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    ಆದರೆ (eval_variable(name, 0, 0, rettv, ಖಾಲಿ, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	ಹಿಂತಿರುಗಿ rettv->v_type == VAR_CLASS;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
object_call_builtin_ವಿಧಾನ(
    object_T		*obj,
    class_builtin_T	builtin_ವಿಧಾನ,
    ಪೂರ್ಣಾಂಕ			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    ಪೂರ್ಣಾಂಕ	    midx;

    ಆದರೆ (obj == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    uf = class_get_builtin_ವಿಧಾನ(obj->obj_class, builtin_ವಿಧಾನ, &midx);
    ಆದರೆ (uf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    ಪೂರ್ಣಾಂಕ		r;

    ಆದರೆ (fc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, ಖಾಲಿ, obj, fc, rettv);

    remove_funccal();

    ಹಿಂತಿರುಗಿ r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    ಪೂರ್ಣಾಂಕ
object_ಖಾಲಿಗಳು(object_T *obj)
{
    typval_T	rettv;

    ಆದರೆ (object_call_builtin_ವಿಧಾನ(obj, CLASS_BUILTIN_ಖಾಲಿಗಳು, 0, ಖಾಲಿ, &rettv)
								== FAIL)
	ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    ಪೂರ್ಣಾಂಕ
object_len(object_T *obj)
{
    typval_T	rettv;

    ಆದರೆ (object_call_builtin_ವಿಧಾನ(obj, CLASS_BUILTIN_LEN, 0, ಖಾಲಿ, &rettv)
								== FAIL)
	ಹಿಂತಿರುಗಿ 0;

    ಹಿಂತಿರುಗಿ tv_ಗಮ್ಯ_ಸಂಖ್ಯೆ(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    ಪೂರ್ಣಾಂಕ
object_equal(
	object_T *o1,
	object_T *o2,
	ಪೂರ್ಣಾಂಕ	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    ಆದರೆ (o1 == o2)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (o1 == ಖಾಲಿ || o2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    ಆದರೆ (cl1 != cl2 || cl1 == ಖಾಲಿ || cl2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl1->class_obj_member_ಎಣಿಕೆ; ++i)
	ಆದರೆ (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
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
    ಪೂರ್ಣಾಂಕ		copyID,
    ಪೂರ್ಣಾಂಕ		echo_style,
    ಪೂರ್ಣಾಂಕ		restore_copyID,
    ಪೂರ್ಣಾಂಕ		ಸಂಯುಕ್ತ_val)
{
    typval_T	rettv;

    ಆದರೆ (object_call_builtin_ವಿಧಾನ(obj, CLASS_BUILTIN_STRING, 0, ಖಾಲಿ, &rettv)
								== OK
					&& rettv.vval.v_string != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ rettv.vval.v_string;

    ಪೂರ್ಣಾಂಕ		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    ಆದರೆ (cl != ಖಾಲಿ && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.ಉದ್ದಳತೆ);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	ಆದರೆ (cl == ಖಾಲಿ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ಇಲ್ಲದಿದ್ದರೆ
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.ಉದ್ದಳತೆ);
    }
    ಆದರೆ (cl != ಖಾಲಿ)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < cl->class_obj_member_ಎಣಿಕೆ; ++i)
	{
	    ಆದರೆ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.ಉದ್ದಳತೆ);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = ಖಾಲಿ;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, ಸಂಯುಕ್ತ_val);
	    ಆದರೆ (s != ಖಾಲಿ)
		ga_concat(&ga, s);
	    vim_free(tf);
	    ಆದರೆ (s == ಖಾಲಿ || did_echo_string_emsg)
	    {
		ok = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	    ಸಾಲು_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    ಆದರೆ (ok == FAIL)
    {
	vim_free(ga.ga_ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ga_append(&ga, NUL);
    ಹಿಂತಿರುಗಿ (char_u *)ga.ga_ದತ್ತಾಂಶ;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    ಪೂರ್ಣಾಂಕ
class_instance_ದ(class_T *cl, class_T *other_cl)
{
    ಆದರೆ (cl == other_cl)
	ಹಿಂತಿರುಗಿ TRUE;

    // Recursively check the base classes.
    ಗಾಗಿ (; cl != ಖಾಲಿ; cl = cl->class_extends)
    {
	ಆದರೆ (cl == other_cl)
	    ಹಿಂತಿರುಗಿ TRUE;
	// Check the implemented interfaces and the super interfaces
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = cl->class_interface_ಎಣಿಕೆ - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    ಆಗ (intf != ಖಾಲಿ)
	    {
		ಆದರೆ (intf == other_cl)
		    ಹಿಂತಿರುಗಿ TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ FALSE;
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

    ಆದರೆ (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_ವಾದಗಳು(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (object_tv->vval.v_object == ಖಾಲಿ)
    {
	ಆದರೆ (classinfo_tv->vval.v_class == ಖಾಲಿ)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	ಆದರೆ (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	ಇಲ್ಲದಿದ್ದರೆ
	    c = classinfo_tv->vval.v_class;

	ಆದರೆ (class_instance_ದ(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
	    ಹಿಂತಿರುಗಿ;
	}
    }
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
