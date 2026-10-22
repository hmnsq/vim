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

#నిర్వచించు USING_FLOAT_STUFF
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

స్థిర class_T *మొదటి_class = ఖాళీ;
స్థిర class_T *తదుపరి_nonref_class = ఖాళీ;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    స్థిర శూన్య
class_created(class_T *cl)
{
    ఒకవేళ (మొదటి_class != ఖాళీ)
    {
	cl->class_తదుపరి_వాడిన = మొదటి_class;
	మొదటి_class->class_prev_వాడిన = cl;
    }
    మొదటి_class = cl;
}

/*
 * Call this function when a class has been cleared and is about to be freed.
 * It is removed from the list headed by "first_class".
 */
    స్థిర శూన్య
class_cleared(class_T *cl)
{
    ఒకవేళ (cl->class_తదుపరి_వాడిన != ఖాళీ)
	cl->class_తదుపరి_వాడిన->class_prev_వాడిన = cl->class_prev_వాడిన;
    ఒకవేళ (cl->class_prev_వాడిన != ఖాళీ)
	cl->class_prev_వాడిన->class_తదుపరి_వాడిన = cl->class_తదుపరి_వాడిన;
    లేకపోతే ఒకవేళ (మొదటి_class == cl)
	మొదటి_class = cl->class_తదుపరి_వాడిన;

    // update the next class to check if needed
    ఒకవేళ (cl == తదుపరి_nonref_class)
	తదుపరి_nonref_class = cl->class_తదుపరి_వాడిన;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    స్థిర పూర్ణాంకం
parse_member(
    exarg_T	*eap,
    char_u	*పంక్తి,
    char_u	*varname,
    పూర్ణాంకం		has_ప్రజా,	    // TRUE if "public" seen before "varname"
    char_u	**varname_చివర,
    పూర్ణాంకం		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_సమాసం)
{
    *varname_చివర = గమ్యం_name_చివర(varname, FALSE);
    ఒకవేళ (*varname == '_' && has_ప్రజా)
    {
	semsg(_(e_ప్రజా_variable_name_cannot_ప్రారంభం_with_underscore_str), పంక్తి);
	తిరిగి FAIL;
    }

    char_u *colon = skipwhite(*varname_చివర);
    char_u *type_arg = colon;
    type_T *type = ఖాళీ;
    *has_type = FALSE;
    ఒకవేళ (*colon == ':')
    {
	ఒకవేళ (VIM_ISWHITE(**varname_చివర))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    తిరిగి FAIL;
	}
	ఒకవేళ (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    తిరిగి FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, ఖాళీ, ఖాళీ, TRUE);
	ఒకవేళ (type == ఖాళీ || !valid_declaration_type(type))
	    తిరిగి FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    ఒకవేళ (type == ఖాళీ && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	తిరిగి FAIL;
    }

    ఒకవేళ (init_సమాసం == ఖాళీ && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_లోపల_interface));
	తిరిగి FAIL;
    }

    ఒకవేళ (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *సమాసం_ప్రారంభం, *సమాసం_చివర;

	ఒకవేళ (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    తిరిగి FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(శూన్య)skip_సమాసం_concatenate(&init_arg, &సమాసం_ప్రారంభం, &సమాసం_చివర, &evalarg);

	init_arg = skipwhite(init_arg);
	ఒకవేళ (*init_arg != NUL && !vim9_comment_ప్రారంభం(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, ఖాళీ);
	    తిరిగి FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	ఒకవేళ (type == ఖాళీ)
	    type = &t_any;

	*init_సమాసం = vim_strnsave(సమాసం_ప్రారంభం, సమాసం_చివర - సమాసం_ప్రారంభం);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, ఖాళీ);
    }
    లేకపోతే ఒకవేళ (!valid_declaration_type(type))
	తిరిగి FAIL;

    *type_ret = type;
    తిరిగి OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    స్థిర పూర్ణాంకం
కూడు_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_చివర,
    పూర్ణాంకం		has_ప్రజా,
    పూర్ణాంకం		has_final,
    పూర్ణాంకం		has_const,
    పూర్ణాంకం		has_type,
    type_T	*type,
    char_u	*init_సమాసం)
{
    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_దత్తాంశం) + gap->ga_len;
    m->ocm_name.నిడివి = (size_t)(varname_చివర - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.నిడివి);
    ఒకవేళ (m->ocm_name.string == ఖాళీ)
    {
	m->ocm_name.నిడివి = 0;
	తిరిగి FAIL;
    }
    m->ocm_access = has_ప్రజా ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_వ్యక్తిగత : VIM_ACCESS_READ;
    ఒకవేళ (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    ఒకవేళ (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    ఒకవేళ (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    ఒకవేళ (init_సమాసం != ఖాళీ)
    {
	m->ocm_init = init_సమాసం;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
కూడు_members_గమ్యం_class(
    garray_T	*gap,
    ocmember_T	*మాతృ_members,
    పూర్ణాంకం		మాతృ_లెక్క,
    ocmember_T	**members,
    పూర్ణాంకం		*member_లెక్క)
{
    *member_లెక్క = మాతృ_లెక్క + gap->ga_len;
    *members = *member_లెక్క == 0 ? ఖాళీ
				       : ALLOC_MULT(ocmember_T, *member_లెక్క);
    ఒకవేళ (*member_లెక్క > 0 && *members == ఖాళీ)
	తిరిగి FAIL;
    కొరకు (పూర్ణాంకం i = 0; i < మాతృ_లెక్క; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = మాతృ_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.నిడివి);
	ఒకవేళ (m->ocm_name.string == ఖాళీ)
	{
	    m->ocm_name.నిడివి = 0;
	    తిరిగి FAIL;
	}
	ఒకవేళ (m->ocm_init != ఖాళీ)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    ఒకవేళ (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + మాతృ_లెక్క,
			       gap->ga_దత్తాంశం, పరిమాణం(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_దత్తాంశం);
    తిరిగి OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    పూర్ణాంకం
object_సూచీ_from_itf_సూచీ(class_T *itf, పూర్ణాంకం is_పద్ధతి, పూర్ణాంకం idx, class_T *cl)
{
    ఒకవేళ (idx >= (is_పద్ధతి ? itf->class_obj_పద్ధతి_లెక్క
				   : itf->class_obj_member_లెక్క))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	తిరిగి 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    ఒకవేళ (cl == itf)
	తిరిగి idx;

    itf2class_T		*i2c = ఖాళీ;
    పూర్ణాంకం			searching = TRUE;
    పూర్ణాంకం			పద్ధతి_స్థానభ్రంశం = 0;

    కొరకు (class_T *super = cl; super != ఖాళీ && searching;
						super = super->class_extends)
    {
	కొరకు (i2c = itf->class_itf2class; i2c != ఖాళీ; i2c = i2c->i2c_తదుపరి)
	{
	    ఒకవేళ (i2c->i2c_class == super && i2c->i2c_is_పద్ధతి == is_పద్ధతి)
	    {
		searching = FALSE;
		ఆపు;
	    }
	}
	ఒకవేళ (searching && is_పద్ధతి)
	    // The parent class methods are stored after the current class
	    // methods.
	    పద్ధతి_స్థానభ్రంశం += super->class_obj_పద్ధతి_లెక్క_బిడ్డ;
    }
    ఒకవేళ (i2c == ఖాళీ)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	తిరిగి 0;
    }

    // A table follows the i2c for the class
    పూర్ణాంకం *పట్టిక = (పూర్ణాంకం *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    తిరిగి పట్టిక[idx] + పద్ధతి_స్థానభ్రంశం;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    స్థిర పూర్ణాంకం
validate_extends_class(
    class_T *cl,
    char_u  *extends_name,
    class_T **extends_clp,
    పూర్ణాంకం	    is_class)
{
    typval_T	tv;
    పూర్ణాంకం		success = FALSE;

    ఒకవేళ (STRCMP(cl->class_name.string, extends_name) == 0)
    {
	semsg(_(e_cannot_extend_str), extends_name);
	తిరిగి success;
    }

    tv.v_type = VAR_UNKNOWN;
    ఒకవేళ (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_దొరికింది_str), extends_name);
	తిరిగి success;
    }

    ఒకవేళ (tv.v_type != VAR_CLASS || tv.vval.v_class == ఖాళీ
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    లేకపోతే
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    తిరిగి success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    స్థిర పూర్ణాంకం
validate_extends_generic_పద్ధతి(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    అక్షరం	*సందేశం = ఖాళీ;

    ఒకవేళ (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	తిరిగి TRUE;

    ఒకవేళ (super_fp->uf_generic_argcount == 0)
	సందేశం = e_concrete_పద్ధతి_str_override_with_generic_పద్ధతి_లోపల_class_str;
    లేకపోతే ఒకవేళ (cl_fp->uf_generic_argcount == 0)
	సందేశం = e_generic_పద్ధతి_str_override_with_concrete_పద్ధతి_లోపల_class_str;
    లేకపోతే
	సందేశం = e_generic_పద్ధతి_str_type_arguments_mismatch_లోపల_class_str;

    semsg(_(సందేశం), cl_fp->uf_name, super_cl->class_name.string);
    తిరిగి FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    స్థిర పూర్ణాంకం
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    పూర్ణాంకం		పద్ధతి_లెక్క = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_దత్తాంశం);

    అయితే (super != ఖాళీ)
    {
	పూర్ణాంకం extends_పద్ధతి_లెక్క = super->class_obj_పద్ధతి_లెక్క_బిడ్డ;
	ఒకవేళ (extends_పద్ధతి_లెక్క == 0)
	{
	    super = super->class_extends;
	    కొనసాగు;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	కొరకు (పూర్ణాంకం i = 0; i < extends_పద్ధతి_లెక్క; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    పూర్ణాంకం	    extends_వ్యక్తిగత = (*pstr == '_');
	    ఒకవేళ (extends_వ్యక్తిగత)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    కొరకు (పూర్ణాంకం j = 0; j < పద్ధతి_లెక్క; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		పూర్ణాంకం	priv_పద్ధతి = (*qstr == '_');
		ఒకవేళ (priv_పద్ధతి)
		    qstr++;
		ఒకవేళ (STRCMP(pstr, qstr) == 0)
		{
		    ఒకవేళ (priv_పద్ధతి != extends_వ్యక్తిగత)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_పద్ధతి_str_యొక్క_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			తిరిగి FALSE;
		    }

		    ఒకవేళ (!validate_extends_generic_పద్ధతి(super,
						extends_methods[i], cl_fp[j]))
			    తిరిగి FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    తిరిగి TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    స్థిర పూర్ణాంకం
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    పూర్ణాంకం member_లెక్క = objmembers_gap->ga_len;
    ఒకవేళ (member_లెక్క == 0)
	తిరిగి TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_దత్తాంశం);

    // Validate each member variable
    కొరకు (పూర్ణాంకం c_i = 0; c_i < member_లెక్క; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	అయితే (p_cl != ఖాళీ)
	{
	    పూర్ణాంకం p_member_లెక్క = p_cl->class_obj_member_లెక్క;

	    ఒకవేళ (p_member_లెక్క == 0)
	    {
		p_cl = p_cl->class_extends;
		కొనసాగు;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    కొరకు (పూర్ణాంకం p_i = 0; p_i < p_member_లెక్క; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		ఒకవేళ (STRCMP(pstr, qstr) == 0)
		{
		    ఒకవేళ (c_protected != p_protected)
			semsg(_(e_ప్రజా_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    లేకపోతే
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    తిరిగి FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    తిరిగి TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    స్థిర పూర్ణాంకం
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    పూర్ణాంకం member_లెక్క = objmembers_gap->ga_len;
    ఒకవేళ (member_లెక్క == 0)
	తిరిగి TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_దత్తాంశం);

    // Validate each member variable
    కొరకు (పూర్ణాంకం c_i = 0; c_i < member_లెక్క; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	పూర్ణాంకం	    var_దొరికింది = FALSE;

	// Check in all the parent classes in the lineage
	అయితే (p_cl != ఖాళీ && !var_దొరికింది)
	{
	    పూర్ణాంకం p_member_లెక్క = p_cl->class_obj_member_లెక్క;
	    ఒకవేళ (p_member_లెక్క == 0)
	    {
		p_cl = p_cl->class_extends;
		కొనసాగు;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    కొరకు (పూర్ణాంకం p_i = 0; p_i < p_member_లెక్క; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		ఒకవేళ (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    కొనసాగు;

		// Ensure the type is matching.
		where.wt_func_name = (అక్షరం *)c_m->ocm_name.string;
		where.wt_రకం = WT_MEMBER;

		ఒకవేళ (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    తిరిగి FALSE;

		var_దొరికింది = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    తిరిగి TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    స్థిర పూర్ణాంకం
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    కొరకు (పూర్ణాంకం loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	పూర్ణాంకం extends_పద్ధతి_లెక్క = loop == 1
				? extends_cl->class_class_function_లెక్క
				: extends_cl->class_obj_పద్ధతి_లెక్క;
	ఒకవేళ (extends_పద్ధతి_లెక్క == 0)
	    కొనసాగు;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	పూర్ణాంకం పద్ధతి_లెక్క = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_దత్తాంశం
						: objmethods_gap->ga_దత్తాంశం);

	కొరకు (పూర్ణాంకం i = 0; i < extends_పద్ధతి_లెక్క; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    ఒకవేళ (!IS_ABSTRACT_పద్ధతి(uf))
		కొనసాగు;

	    పూర్ణాంకం	concrete_పద్ధతి_దొరికింది = FALSE;
	    పూర్ణాంకం	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    కొరకు (j = 0; !concrete_పద్ధతి_దొరికింది && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		ఒకవేళ (!IS_ABSTRACT_పద్ధతి(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_పద్ధతి_దొరికింది = TRUE;
	    }

	    ఒకవేళ (concrete_పద్ధతి_దొరికింది)
		కొనసాగు;

	    కొరకు (j = 0; j < పద్ధతి_లెక్క; j++)
	    {
		ఒకవేళ (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_పద్ధతి_దొరికింది = TRUE;
		    ఆపు;
		}
	    }

	    ఒకవేళ (!concrete_పద్ధతి_దొరికింది)
	    {
		semsg(_(e_abstract_పద్ధతి_str_not_implemented), uf->uf_name);
		తిరిగి FALSE;
	    }
	}
    }

    తిరిగి TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    స్థిర పూర్ణాంకం
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    పూర్ణాంకం		cl_member_లెక్క,
    class_T	*extends_cl)
{
    పూర్ణాంకం		variable_present  = FALSE;

    కొరకు (పూర్ణాంకం cl_i = 0; cl_i < cl_member_లెక్క; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	ఒకవేళ (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    కొనసాగు;

	// Ensure the access type is same
	ఒకవేళ (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_యొక్క_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    తిరిగి FALSE;
	}

	// Ensure the type is matching.
	ఒకవేళ (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	లేకపోతే
	{
	    where.wt_func_name = (అక్షరం *)m->ocm_name.string;
	    where.wt_రకం = WT_MEMBER;
	    ఒకవేళ (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		తిరిగి FALSE;
	}

	variable_present = TRUE;
	ఆపు;
    }

    ఒకవేళ (!variable_present && extends_cl != ఖాళీ)
    {
	పూర్ణాంకం ext_cl_లెక్క = extends_cl->class_obj_member_లెక్క;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	తిరిగి intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_లెక్క,
					extends_cl->class_extends);
    }

    తిరిగి variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    స్థిర పూర్ణాంకం
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    పూర్ణాంకం if_లెక్క = ifcl->class_obj_member_లెక్క;
    ఒకవేళ (if_లెక్క == 0)
	తిరిగి TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_దత్తాంశం);
    పూర్ణాంకం cl_లెక్క = objmembers_gap->ga_len;
    కొరకు (పూర్ణాంకం if_i = 0; if_i < if_లెక్క; ++if_i)
    {
	ఒకవేళ (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_లెక్క, extends_cl))
	{
	    semsg(_(e_variable_str_యొక్క_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    తిరిగి FALSE;
	}
    }

    తిరిగి TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    స్థిర పూర్ణాంకం
intf_పద్ధతి_type_matches(
    ufunc_T	*if_పద్ధతి,
    ufunc_T	*cl_పద్ధతి,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    ఒకవేళ (cl_పద్ధతి->uf_generic_argcount != if_పద్ధతి->uf_generic_argcount)
    {
	semsg(_(e_type_var_లెక్క_యొక్క_పద్ధతి_str_differs_from_interface_str),
						cl_పద్ధతి->uf_name, intf_name);
	తిరిగి FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (అక్షరం *)if_పద్ధతి->uf_name;
    where.wt_రకం = WT_పద్ధతి;
    ఒకవేళ (check_type(if_పద్ధతి->uf_func_type, cl_పద్ధతి->uf_func_type, TRUE,
								where) == FAIL)
	తిరిగి FALSE;

    తిరిగి TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    స్థిర పూర్ణాంకం
intf_పద్ధతి_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    పూర్ణాంకం	    cl_లెక్క,
    class_T *extends_cl,
    char_u  *intf_name)
{
    పూర్ణాంకం		పద్ధతి_present  = FALSE;

    కొరకు (పూర్ణాంకం cl_i = 0; cl_i < cl_లెక్క; ++cl_i)
    {
	ఒకవేళ (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    ఒకవేళ (!intf_పద్ధతి_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		తిరిగి FALSE;
	    పద్ధతి_present = TRUE;
	    ఆపు;
	}
    }

    ఒకవేళ (!పద్ధతి_present && extends_cl != ఖాళీ)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	పూర్ణాంకం	ext_cl_లెక్క = extends_cl->class_obj_పద్ధతి_లెక్క;
	తిరిగి intf_పద్ధతి_present(if_ufunc, ext_cl_fp, ext_cl_లెక్క,
					extends_cl->class_extends, intf_name);
    }

    తిరిగి పద్ధతి_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    స్థిర పూర్ణాంకం
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    పూర్ణాంకం if_లెక్క = ifcl->class_obj_పద్ధతి_లెక్క;
    ఒకవేళ (if_లెక్క == 0)
	తిరిగి TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_దత్తాంశం);
    పూర్ణాంకం cl_లెక్క = objmethods_gap->ga_len;
    కొరకు (పూర్ణాంకం if_i = 0; if_i < if_లెక్క; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	ఒకవేళ (!intf_పద్ధతి_present(if_fp[if_i], cl_fp, cl_లెక్క, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_పద్ధతి_str_యొక్క_interface_str_not_implemented),
		    if_name, intf_class_name);
	    తిరిగి FALSE;
	}
    }

    తిరిగి TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    స్థిర పూర్ణాంకం
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    పూర్ణాంకం		success = TRUE;

    కొరకు (పూర్ణాంకం i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_దత్తాంశం)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	ఒకవేళ (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_దొరికింది_str), impl);
	    success = FALSE;
	    ఆపు;
	}

	ఒకవేళ (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == ఖాళీ
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    ఆపు;
	}

	class_T *ifcl = tv.vval.v_class;
	ఒకవేళ (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    ఆపు;
	}
	((class_T **)intf_classes_gap->ga_దత్తాంశం)[intf_classes_gap->ga_len]
								= ifcl;
	intf_classes_gap->ga_len++;
	++ifcl->class_refcount;

	// check the variables of the interface match the members of the class
	success = validate_interface_variables(impl, ifcl, objmembers_gap,
								extends_cl);
	clear_tv(&tv);
    }

    ఒకవేళ (!success)
	తిరిగి FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_దత్తాంశం;
    cl->class_interface_లెక్క = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    కొరకు (పూర్ణాంకం i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_దత్తాంశం)[i],
		((class_T **)intf_classes_gap->ga_దత్తాంశం)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = ఖాళీ;
    cl->class_interface_లెక్క = 0;
    cl->class_extends = ఖాళీ;

    తిరిగి success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    స్థిర పూర్ణాంకం
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    కొరకు (పూర్ణాంకం j = 0; j < intf_classes_gap->ga_len; j++)
    {
	ఒకవేళ (((class_T **)intf_classes_gap->ga_దత్తాంశం)[j] == ifcl)
	    తిరిగి TRUE;
    }

    తిరిగి FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    స్థిర పూర్ణాంకం
కూడు_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.నిడివి);
    ఒకవేళ (intf_name == ఖాళీ)
	తిరిగి FALSE;

    ఒకవేళ (ga_grow(impl_gap, 1) == FAIL)
	వెళ్ళు fail;

    char_u **intf_పేర్లు = (char_u **)impl_gap->ga_దత్తాంశం;
    intf_పేర్లు[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    ఒకవేళ (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	వెళ్ళు fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_దత్తాంశం;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    తిరిగి TRUE;
fail:
    vim_free(intf_name);
    తిరిగి FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    స్థిర పూర్ణాంకం
కూడు_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    కొరకు (పూర్ణాంకం i = 0; i < super->class_interface_లెక్క; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	ఒకవేళ (!is_interface_class_present(intf_classes_gap, ifcl))
	    కూడు_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    తిరిగి TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    స్థిర పూర్ణాంకం
check_func_arg_పేర్లు(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    కొరకు (పూర్ణాంకం loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	కొరకు (పూర్ణాంకం fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_దత్తాంశం)[fi];

	    కొరకు (పూర్ణాంకం i = 0; i < uf->uf_వాదనలు.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_వాదనలు.ga_దత్తాంశం)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		కొరకు (పూర్ణాంకం mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_దత్తాంశం + mi)->ocm_name.string;
		    ఒకవేళ (STRCMP(aname, mname) == 0)
		    {
			ఒకవేళ (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_వాదం_already_declared_లోపల_class_str),
				aname);

			తిరిగి FALSE;
		    }
		}
	    }
	}
    }

    తిరిగి TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    స్థిర పూర్ణాంకం
is_reserved_varname(char_u *varname, char_u *varname_చివర)
{
    పూర్ణాంకం reserved = FALSE;
    char_u save_varname_చివర = *varname_చివర;
    *varname_చివర = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_చివర = save_varname_చివర;

    తిరిగి reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    స్థిర పూర్ణాంకం
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_చివర)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_చివర - varname)};
    పూర్ణాంకం		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    ఒకవేళ (*pstr.string == '_')
    {
	pstr.string++;
	pstr.నిడివి--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    కొరకు (పూర్ణాంకం loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	కొరకు (పూర్ణాంకం i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_దత్తాంశం) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.నిడివి};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    ఒకవేళ (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.నిడివి--;
		q_protected = true;
	    }

	    ఒకవేళ (pstr.నిడివి == qstr.నిడివి
		&& STRNCMP(pstr.string, qstr.string, pstr.నిడివి) == 0)
	    {
		char_u	save_c = *varname_చివర;

		*varname_చివర = NUL;
		ఒకవేళ (p_protected != q_protected)
		    semsg(_(e_ప్రజా_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		లేకపోతే
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_చివర = save_c;
		dup = TRUE;
		ఆపు;
	    }
	}
    }

    తిరిగి dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    స్థిర పూర్ణాంకం
is_duplicate_పద్ధతి(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    కొరకు (పూర్ణాంకం loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	కొరకు (పూర్ణాంకం i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_దత్తాంశం)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    ఒకవేళ (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		తిరిగి TRUE;
	    }
	}
    }

    తిరిగి FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    స్థిర పూర్ణాంకం
is_valid_constructor(ufunc_T *uf, పూర్ణాంకం is_abstract, పూర్ణాంకం has_static)
{
    // Constructors are not allowed in abstract classes.
    ఒకవేళ (is_abstract)
    {
	emsg(_(e_cannot_define_new_పద్ధతి_లోపల_abstract_class));
	తిరిగి FALSE;
    }
    // A constructor is always static, no need to define it so.
    ఒకవేళ (has_static)
    {
	emsg(_(e_cannot_define_new_పద్ధతి_as_static));
	తిరిగి FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    ఒకవేళ (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_పద్ధతి));
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    స్థిర పూర్ణాంకం
object_check_builtin_పద్ధతి_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    పూర్ణాంకం	    valid = FALSE;
    type_T  పద్ధతి_sig;
    type_T  పద్ధతి_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_క్షేత్రం(పద్ధతి_sig);
    CLEAR_క్షేత్రం(పద్ధతి_rt);
    పద్ధతి_sig.tt_type = VAR_FUNC;

    ఒకవేళ (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	పద్ధతి_rt.tt_type = VAR_సంఖ్య;
	పద్ధతి_sig.tt_member = &పద్ధతి_rt;
	valid = TRUE;
    }
    లేకపోతే ఒకవేళ (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	పద్ధతి_rt.tt_type = VAR_BOOL;
	పద్ధతి_sig.tt_member = &పద్ధతి_rt;
	valid = TRUE;
    }
    లేకపోతే ఒకవేళ (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	పద్ధతి_rt.tt_type = VAR_STRING;
	పద్ధతి_sig.tt_member = &పద్ధతి_rt;
	valid = TRUE;
    }
    లేకపోతే
	semsg(_(e_builtin_object_పద్ధతి_str_not_supported), uf->uf_name);

    where.wt_func_name = (అక్షరం *)uf->uf_name;
    where.wt_రకం = WT_పద్ధతి;
    ఒకవేళ (valid && !check_type(&పద్ధతి_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    తిరిగి valid;
}

/*
 * Returns TRUE if "funcname" is a supported builtin object method name
 */
    పూర్ణాంకం
is_valid_builtin_obj_methodname(char_u *funcname)
{
    ఎంపిక (funcname[0])
    {
	స్థితి 'e':
	    తిరిగి STRNCMP(funcname, "empty", 5) == 0;

	స్థితి 'l':
	    తిరిగి STRNCMP(funcname, "len", 3) == 0;

	స్థితి 'n':
	    తిరిగి STRNCMP(funcname, "new", 3) == 0;

	స్థితి 's':
	    తిరిగి STRNCMP(funcname, "string", 6) == 0;
    }

    తిరిగి FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_పద్ధతి(
    class_T		*cl,
    class_builtin_T	builtin_పద్ధతి,
    పూర్ణాంకం			*పద్ధతి_idx)
{
    *పద్ధతి_idx = -1;

    ఒకవేళ (cl == ఖాళీ)
	తిరిగి ఖాళీ;

    *పద్ధతి_idx = cl->class_builtin_methods[builtin_పద్ధతి];
    తిరిగి *పద్ధతి_idx != -1 ? cl->class_obj_methods[*పద్ధతి_idx] : ఖాళీ;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    స్థిర పూర్ణాంకం
update_member_పద్ధతి_lookup_పట్టిక(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    పూర్ణాంకం		pobj_పద్ధతి_స్థానభ్రంశం)
{
    ఒకవేళ (ifcl == ఖాళీ)
	తిరిగి OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(పరిమాణం(itf2class_T)
				+ ifcl->class_obj_member_లెక్క * పరిమాణం(పూర్ణాంకం));
    ఒకవేళ (if2cl == ఖాళీ)
	తిరిగి FAIL;
    if2cl->i2c_తదుపరి = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_పద్ధతి = FALSE;

    కొరకు (పూర్ణాంకం if_i = 0; if_i < ifcl->class_obj_member_లెక్క; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	కొరకు (పూర్ణాంకం cl_i = 0; cl_i < cl->class_obj_member_లెక్క; ++cl_i)
	{
	    ఒకవేళ (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		పూర్ణాంకం *పట్టిక = (పూర్ణాంకం *)(if2cl + 1);
		పట్టిక[if_i] = cl_i;
		ఆపు;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(పరిమాణం(itf2class_T)
				+ ifcl->class_obj_పద్ధతి_లెక్క * పరిమాణం(పూర్ణాంకం));
    ఒకవేళ (if2cl == ఖాళీ)
	తిరిగి FAIL;
    if2cl->i2c_తదుపరి = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_పద్ధతి = TRUE;

    కొరకు (పూర్ణాంకం if_i = 0; if_i < ifcl->class_obj_పద్ధతి_లెక్క; ++if_i)
    {
	పూర్ణాంకం	పూర్తి = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	కొరకు (పూర్ణాంకం cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    ఒకవేళ (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_దత్తాంశం)[cl_i]->uf_name) == 0)
	    {
		పూర్ణాంకం *పట్టిక = (పూర్ణాంకం *)(if2cl + 1);
		పట్టిక[if_i] = cl_i;
		పూర్తి = TRUE;
		ఆపు;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	ఒకవేళ (!పూర్తి)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    ఒకవేళ (cl->class_extends != ifcl)
	    {
		class_T		*మాతృ = cl->class_extends;
		పూర్ణాంకం		పద్ధతి_స్థానభ్రంశం = objmethods->ga_len;

		అయితే (!పూర్తి && మాతృ != ఖాళీ && మాతృ != ifcl)
		{
		    కొరకు (పూర్ణాంకం cl_i = 0;
			    cl_i < మాతృ->class_obj_పద్ధతి_లెక్క_బిడ్డ; ++cl_i)
		    {
			ఒకవేళ (STRCMP(if_name,
				    మాతృ->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    పూర్ణాంకం *పట్టిక = (పూర్ణాంకం *)(if2cl + 1);
			    పట్టిక[if_i] = పద్ధతి_స్థానభ్రంశం + cl_i;
			    పూర్తి = TRUE;
			    ఆపు;
			}
		    }
		    పద్ధతి_స్థానభ్రంశం += మాతృ->class_obj_పద్ధతి_లెక్క_బిడ్డ;
		    మాతృ = మాతృ->class_extends;
		}
	    }

	    ఒకవేళ (!పూర్తి)
	    {
		పూర్ణాంకం *పట్టిక = (పూర్ణాంకం *)(if2cl + 1);
		పట్టిక[if_i] = pobj_పద్ధతి_స్థానభ్రంశం + if_i;
	    }
	}
    }

    తిరిగి OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    స్థిర పూర్ణాంకం
కూడు_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    కొరకు (పూర్ణాంకం i = 0; i < cl->class_interface_లెక్క; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	అయితే (ifcl != ఖాళీ)
	{
	    ఒకవేళ (update_member_పద్ధతి_lookup_పట్టిక(ifcl, cl, objmethods_gap,
								0) == FAIL)
		తిరిగి FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    ఒకవేళ (extends_cl != ఖాళీ)
    {
	class_T		*pclass = extends_cl;
	పూర్ణాంకం		pobj_పద్ధతి_స్థానభ్రంశం = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	అయితే (pclass != ఖాళీ)
	{
	    ఒకవేళ (update_member_పద్ధతి_lookup_పట్టిక(pclass, cl,
			objmethods_gap, pobj_పద్ధతి_స్థానభ్రంశం) == FAIL)
		తిరిగి FAIL;

	    pobj_పద్ధతి_స్థానభ్రంశం += pclass->class_obj_పద్ధతి_లెక్క_బిడ్డ;
	    pclass = pclass->class_extends;
	}
    }

    తిరిగి OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    స్థిర పూర్ణాంకం
కూడు_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_లెక్క);
    ఒకవేళ (cl->class_members_tv == ఖాళీ)
	తిరిగి FAIL;

    కొరకు (పూర్ణాంకం i = 0; i < cl->class_class_member_లెక్క; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	ఒకవేళ (m->ocm_init != ఖాళీ)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_సమాసం(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    ఒకవేళ (etv == ఖాళీ)
		తిరిగి FAIL;

	    ఒకవేళ (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && etv->v_type != VAR_SPECIAL)
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = typval2type(etv, get_copyID(),
			type_list_gap,
			TVTT_DO_MEMBER|TVTT_మరిన్ని_SPECIFIC);
	    *tv = *etv;
	    vim_free(etv);
	}
	లేకపోతే
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = ఖాళీ;
	}
	సమితి_tv_type(tv, m->ocm_type);
	ఒకవేళ (m->ocm_flags & OCMFLAG_CONST)
	    అంశం_తాళం(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    తిరిగి OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    స్థిర శూన్య
కూడు_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    పూర్ణాంకం		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    కొరకు (పూర్ణాంకం i = 0; i < cl->class_obj_member_లెక్క; ++i)
    {
	ఒకవేళ (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    కొనసాగు;

	ఒకవేళ (i > (is_enum ? 2 : 0))
	    GA_CONCAT_LITERAL(&fga, ", ");
	GA_CONCAT_LITERAL(&fga, "this.");
	ocmember_T *m = cl->class_obj_members + i;
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.నిడివి);
	GA_CONCAT_LITERAL(&fga, " = v:none");
    }
    GA_CONCAT_LITERAL(&fga, ")\nenddef\n");
    ga_append(&fga, NUL);

    exarg_T fea;
    CLEAR_క్షేత్రం(fea);
    fea.cmdidx = CMD_def;
    fea.cmd = fea.arg = fga.ga_దత్తాంశం;

    garray_T పంక్తులు_గమ్యం_free;
    ga_init2(&పంక్తులు_గమ్యం_free, పరిమాణం(char_u *), 50);

    ufunc_T *nf = define_function(&fea, ఖాళీ, &పంక్తులు_గమ్యం_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_లెక్క,
			    ఖాళీ);

    ga_clear_అక్షరశ్రేణులు(&పంక్తులు_గమ్యం_free);
    vim_free(fga.ga_దత్తాంశం);

    ఒకవేళ (nf != ఖాళీ && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_దత్తాంశం)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	ఒకవేళ (nf->uf_ret_type != ఖాళీ)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_వాదనలు = ఖాళీ;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    స్థిర పూర్ణాంకం
కూడు_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    కొరకు (పూర్ణాంకం loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	పూర్ణాంకం		*fcount = loop == 1 ? &cl->class_class_function_లెక్క
						: &cl->class_obj_పద్ధతి_లెక్క;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	పూర్ణాంకం మాతృ_లెక్క = 0;
	ఒకవేళ (extends_cl != ఖాళీ)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    మాతృ_లెక్క = loop == 1
				? 0
				: extends_cl->class_obj_పద్ధతి_లెక్క;

	*fcount = మాతృ_లెక్క + gap->ga_len;
	ఒకవేళ (*fcount == 0)
	{
	    *fup = ఖాళీ;
	    కొనసాగు;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	ఒకవేళ (*fup == ఖాళీ)
	    తిరిగి FAIL;

	ఒకవేళ (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_దత్తాంశం, పరిమాణం(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_దత్తాంశం);
	ఒకవేళ (loop == 1)
	    cl->class_class_function_లెక్క_బిడ్డ = gap->ga_len;
	లేకపోతే
	    cl->class_obj_పద్ధతి_లెక్క_బిడ్డ = gap->ga_len;

	ఒకవేళ (loop == 2)
	{
	    // Copy instance methods from the parent.

	    కొరకు (పూర్ణాంకం i = 0; i < మాతృ_లెక్క; ++i)
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
		ఒకవేళ (nf != ఖాళీ && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_వాదనలు != ఖాళీ)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		కొరకు (పూర్ణాంకం ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    ఒకవేళ (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (అక్షరం *)pname;
			where.wt_రకం = WT_పద్ధతి;
			(శూన్య)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	కొరకు (పూర్ణాంకం i = 0; i < *fcount; ++i)
	{
	    ufunc_T *fp = (*fup)[i];
	    fp->uf_class = cl;
	    ఒకవేళ (i < gap->ga_len)
		fp->uf_defclass = cl;
	    ఒకవేళ (loop == 2)
		fp->uf_flags |= FC_OBJECT;
	}

	ga_clear(gap);
    }

    తిరిగి OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    స్థిర శూన్య
update_builtin_పద్ధతి_సూచీ(class_T *cl)
{
    పూర్ణాంకం	i;

    కొరకు (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    కొరకు (i = 0; i < cl->class_obj_పద్ధతి_లెక్క; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	ఒకవేళ (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	లేకపోతే ఒకవేళ (cl->class_builtin_methods[CLASS_BUILTIN_ఖాళీలు] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_ఖాళీలు] = i;
	లేకపోతే ఒకవేళ (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    స్థిర char_u *
కనుగొను_class_name_చివర(char_u *arg)
{
    char_u *చివర = arg;

    అయితే (ASCII_ISALNUM(*చివర) || *చివర == '_'
	    || (*చివర == '.' && (ASCII_ISALNUM(చివర[1]) || చివర[1] == '_')))
	++చివర;

    తిరిగి చివర;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    స్థిర పూర్ణాంకం
is_duplicate_enum(
    garray_T	*enum_gap,
    char_u	*varname,
    char_u	*varname_చివర)
{
    string_T	name = {varname, (size_t)(varname_చివర - varname)};  // Note: the .string field may
								    // point to a string longer
								    // than the .length field.
								    // So we need to use STRNCMP()
								    // to compare it.
    పూర్ణాంకం		dup = FALSE;

    కొరకు (పూర్ణాంకం i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_దత్తాంశం) + i;

	ఒకవేళ (name.నిడివి == m->ocm_name.నిడివి
	    && STRNCMP(name.string, m->ocm_name.string, name.నిడివి) == 0)
	{
	    char_u  save_c = *varname_చివర;

	    *varname_చివర = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_చివర = save_c;
	    dup = TRUE;
	    ఆపు;
	}
    }

    తిరిగి dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    స్థిర పూర్ణాంకం
enum_parse_విలువలు(
    exarg_T	*eap,
    class_T	*en,
    char_u	*పంక్తి,
    garray_T	*gap,
    పూర్ణాంకం		*అంకె_enum_విలువలు,
    పూర్ణాంకం		*enum_చివర)
{
    evalarg_T	evalarg;
    char_u	*p = పంక్తి;
    అక్షరం	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	చివరి_char = NUL;
    పూర్ణాంకం		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    పూర్ణాంకం		did_emsg_before = did_emsg;
    అయితే (*p != NUL)
    {
	// ignore comment
	ఒకవేళ (*p == '#')
	    ఆపు;

	ఒకవేళ (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_విలువ_declaration_str), p);
	    ఆపు;
	}

	char_u *eni_name_ప్రారంభం = p;
	char_u *eni_name_చివర = గమ్యం_name_చివర(p, FALSE);

	ఒకవేళ (is_duplicate_enum(gap, eni_name_ప్రారంభం, eni_name_చివర))
	    ఆపు;

	p = skipwhite(eni_name_చివర);

	char_u	*init_సమాసం = ఖాళీ;
	ఒకవేళ (*p == '(' || *p == '<')
	{
	    ఒకవేళ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", పంక్తి);
		ఆపు;
	    }

	    char_u *సమాసం_ప్రారంభం, *సమాసం_చివర;

	    p = eni_name_ప్రారంభం;
	    (శూన్య)skip_సమాసం_concatenate(&p, &సమాసం_ప్రారంభం, &సమాసం_చివర, &evalarg);

	    అయితే (*సమాసం_ప్రారంభం && *సమాసం_ప్రారంభం != '(' && *సమాసం_ప్రారంభం != '<')
		సమాసం_ప్రారంభం++;

	    ఒకవేళ (సమాసం_చివర > సమాసం_ప్రారంభం)
		init_సమాసం = vim_strnsave(సమాసం_ప్రారంభం, సమాసం_చివర - సమాసం_ప్రారంభం);
	}

	ఒకవేళ (init_సమాసం == ఖాళీ)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, పరిమాణం(initexpr_buf), "%s.new()", en->class_name.string);
	లేకపోతే
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, పరిమాణం(initexpr_buf), "%s.new%s", en->class_name.string,
		init_సమాసం);
	    vim_free(init_సమాసం);
	}
	ఒకవేళ (కూడు_member(gap, eni_name_ప్రారంభం, eni_name_చివర, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    ఆపు;

	++*అంకె_enum_విలువలు;

	ఒకవేళ (*p != '#')
	    చివరి_char = *p;

	ఒకవేళ (*p != NUL && *p != ',')
	    ఆపు;

	ఒకవేళ (*p == ',')
	{
	    ఒకవేళ (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", పంక్తి);
		ఆపు;
	    }
	    ఒకవేళ (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", పంక్తి);
		ఆపు;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    ఒకవేళ (*p != NUL && *p != '#')
    {
	ఒకవేళ (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_వాదం_str), p);
	rc = FAIL;
    }

    ఒకవేళ (చివరి_char != ',')
	// last enum value should not be terminated by ","
	*enum_చివర = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, ఖాళీ);

    తిరిగి rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    స్థిర పూర్ణాంకం
enum_కూడు_విలువలు_member(
    class_T	*en,
    garray_T	*gap,
    పూర్ణాంకం		అంకె_enum_విలువలు,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    పూర్ణాంకం		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    కొరకు (పూర్ణాంకం i = 0; i < అంకె_enum_విలువలు; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_దత్తాంశం) + i;

	ఒకవేళ (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.నిడివి);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.నిడివి);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    ఒకవేళ (type == ఖాళీ)
	వెళ్ళు పూర్తి;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    ఒకవేళ (type->tt_member != ఖాళీ)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = కూడు_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_దత్తాంశం, fga.ga_len));

పూర్తి:
    vim_free(fga.ga_దత్తాంశం);

    తిరిగి rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    స్థిర శూన్య
enum_clear_constructors(class_T *en)
{
    కొరకు (పూర్ణాంకం i = 0; i < en->class_class_function_లెక్క; ++i)
    {
	ufunc_T *fp = en->class_class_functions[i];

	ఒకవేళ (fp->uf_flags & FC_NEW)
	    *fp->uf_name = NUL;
    }
}

/*
 * Initialize the name and ordinal object variable in the enum value "enval" in
 * the enum "en".  These values are set during the enum value object creation.
 */
    శూన్య
enum_సమితి_అంతర్గత_obj_vars(class_T *en, object_T *enval)
{
    పూర్ణాంకం	i;

    కొరకు (i = 0; i < en->class_class_member_లెక్క; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	ఒకవేళ (en_tv != ఖాళీ && en_tv->v_type == VAR_UNKNOWN)
	    ఆపు;
    }

    ఒకవేళ (i >= en->class_class_member_లెక్క)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	తిరిగి;

    // First object variable is the name
    ocmember_T *విలువ_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(విలువ_ocm->ocm_name.string, విలువ_ocm->ocm_name.నిడివి);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_సంఖ్య;
    ord_tv->vval.v_సంఖ్య = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    స్థిర శూన్య
skip_class_body(exarg_T *eap)
{
    కొరకు (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	ఒకవేళ (theline == ఖాళీ)
	    ఆపు;

	char_u	*p = theline;
	పూర్ణాంకం	దొరికింది;

	// skip ':' and blanks
	కొరకు (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	దొరికింది = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	ఒకవేళ (దొరికింది)
	    ఆపు;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    శూన్య
ex_class(exarg_T *eap)
{
    పూర్ణాంకం		is_class = eap->cmdidx == CMD_class;
    పూర్ణాంకం		is_abstract = eap->cmdidx == CMD_abstract;
    పూర్ణాంకం		is_enum = eap->cmdidx == CMD_enum;
    పూర్ణాంకం		added_enum_విలువలు = FALSE;
    పూర్ణాంకం		is_interface;
    దీర్ఘ	ప్రారంభం_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    ఒకవేళ (eap->skip)
    {
	skip_class_body(eap);
	తిరిగి;
    }

    ఒకవేళ (is_abstract)
    {
	ఒకవేళ (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_వాదం_str), arg);
	    తిరిగి;
	}
	arg = skipwhite(arg + 5);
	is_class = TRUE;
    }

    is_interface = !is_class && !is_enum;

    ఒకవేళ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	ఒకవేళ (is_class)
	    emsg(_(e_class_can_only_be_defined_లోపల_vim9_script));
	లేకపోతే ఒకవేళ (is_enum)
	    emsg(_(e_enum_can_only_be_defined_లోపల_vim9_script));
	లేకపోతే
	    emsg(_(e_interface_can_only_be_defined_లోపల_vim9_script));
	తిరిగి;
    }

    ఒకవేళ (!ASCII_ISUPPER(*arg))
    {
	ఒకవేళ (is_class)
	    semsg(_(e_class_name_must_ప్రారంభం_with_uppercase_letter_str), arg);
	లేకపోతే ఒకవేళ (is_enum)
	    semsg(_(e_enum_name_must_ప్రారంభం_with_uppercase_letter_str), arg);
	లేకపోతే
	    semsg(_(e_interface_name_must_ప్రారంభం_with_uppercase_letter_str),
									  arg);
	తిరిగి;
    }
    char_u *name_చివర = కనుగొను_name_చివర(arg, ఖాళీ, ఖాళీ, FNE_CHECK_ప్రారంభం);
    ఒకవేళ (!IS_WHITE_OR_NUL(*name_చివర))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	తిరిగి;
    }
    char_u *name_ప్రారంభం = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = ఖాళీ;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, పరిమాణం(char_u *), 5);

    arg = skipwhite(name_చివర);
    అయితే (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	ఒకవేళ (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    ఒకవేళ (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		వెళ్ళు early_ret;
	    }
	    ఒకవేళ (extends != ఖాళీ)
	    {
		emsg(_(e_duplicate_extends));
		వెళ్ళు early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *చివర = కనుగొను_class_name_చివర(arg);
	    ఒకవేళ (!IS_WHITE_OR_NUL(*చివర))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		వెళ్ళు early_ret;
	    }
	    extends = vim_strnsave(arg, చివర - arg);
	    ఒకవేళ (extends == ఖాళీ)
		వెళ్ళు early_ret;

	    arg = skipwhite(చివర + 1);
	}
	లేకపోతే ఒకవేళ (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    ఒకవేళ (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		వెళ్ళు early_ret;
	    }

	    ఒకవేళ (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		వెళ్ళు early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    కొరకు (;;)
	    {
		char_u *impl_చివర = కనుగొను_class_name_చివర(arg);
		ఒకవేళ ((!IS_WHITE_OR_NUL(*impl_చివర) && *impl_చివర != ',')
			|| (*impl_చివర == ','
			    && !IS_WHITE_OR_NUL(*(impl_చివర + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    వెళ్ళు early_ret;
		}
		ఒకవేళ (impl_చివర - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    వెళ్ళు early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_చివర - arg);
		ఒకవేళ (iname == ఖాళీ)
		    వెళ్ళు early_ret;
		కొరకు (పూర్ణాంకం i = 0; i < ga_impl.ga_len; ++i)
		    ఒకవేళ (STRCMP(((char_u **)ga_impl.ga_దత్తాంశం)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			వెళ్ళు early_ret;
		    }
		ఒకవేళ (ga_కూడు_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    వెళ్ళు early_ret;
		}
		ఒకవేళ (*impl_చివర != ',')
		{
		    arg = skipwhite(impl_చివర);
		    ఆపు;
		}
		arg = skipwhite(impl_చివర + 1);
	    }
	}
	లేకపోతే
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_అక్షరశ్రేణులు(&ga_impl);
	    తిరిగి;
	}
    }

    garray_T	type_list;	    // list of pointers to allocated types
    ga_init2(&type_list, పరిమాణం(type_T *), 10);

    // Growarray with class members declared in the class.
    garray_T classmembers;
    ga_init2(&classmembers, పరిమాణం(ocmember_T), 10);

    // Growarray with functions declared in the class.
    garray_T classfunctions;
    ga_init2(&classfunctions, పరిమాణం(ufunc_T *), 10);

    // Growarray with object members declared in the class.
    garray_T objmembers;
    ga_init2(&objmembers, పరిమాణం(ocmember_T), 10);

    // Growarray with object methods declared in the class.
    garray_T objmethods;
    ga_init2(&objmethods, పరిమాణం(ufunc_T *), 10);

    class_T *cl = ఖాళీ;
    class_T *extends_cl = ఖాళీ;  // class from "extends" argument
    class_T **intf_classes = ఖాళీ;
    పూర్ణాంకం	    intf_లెక్క = 0;	// number of items in "intf_classes"
    పూర్ణాంకం	    అంకె_enum_విలువలు = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    ఒకవేళ (cl == ఖాళీ)
	వెళ్ళు cleanup;

    ఒకవేళ (is_enum)
	cl->class_flags = CLASS_ENUM;
    లేకపోతే ఒకవేళ (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    లేకపోతే ఒకవేళ (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.నిడివి = (size_t)(name_చివర - name_ప్రారంభం);
    cl->class_name.string = vim_strnsave(name_ప్రారంభం, cl->class_name.నిడివి);
    ఒకవేళ (cl->class_name.string == ఖాళీ)
	వెళ్ళు cleanup;

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
    SOURCING_LNUM = ప్రారంభం_lnum;
    పూర్ణాంకం rc = సమితి_var_const(cl->class_name.string, 0, ఖాళీ, &tv, FALSE, 0, 0);
    ఒకవేళ (rc == FAIL)
	వెళ్ళు cleanup;

    ఒకవేళ (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	ఒకవేళ (కూడు_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, ఖాళీ) == FAIL)
	    వెళ్ళు cleanup;

	varname = (char_u *)"ordinal";
	ఒకవేళ (కూడు_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_సంఖ్య, ఖాళీ) == FAIL)
	    వెళ్ళు cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    పూర్ణాంకం		enum_చివర = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = ఖాళీ;
    పూర్ణాంకం success = FALSE;
    కొరకు (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	ఒకవేళ (theline == ఖాళీ)
	    ఆపు;
	char_u *పంక్తి = skipwhite(theline);

	// Skip empty and comment lines.
	ఒకవేళ (*పంక్తి == NUL)
	    కొనసాగు;
	ఒకవేళ (*పంక్తి == '#')
	{
	    ఒకవేళ (vim9_bad_comment(పంక్తి))
		ఆపు;
	    కొనసాగు;
	}

	char_u *p = పంక్తి;

	అక్షరం	*చివర_name;
	పూర్ణాంకం	shortlen;
	పూర్ణాంకం	fullen;
	ఒకవేళ (is_class)
	{
	    చివర_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	లేకపోతే ఒకవేళ (is_enum)
	{
	    చివర_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	లేకపోతే
	{
	    చివర_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	కొరకు (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_ప్రారంభం = p;
	ఒకవేళ (checkforcmd(&p, చివర_name, shortlen))
	{
	    ఒకవేళ (STRNCMP(cmd_ప్రారంభం, చివర_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_ప్రారంభం);
	    లేకపోతే ఒకవేళ (*p == '|' || !చివర్లు_excmd2(పంక్తి, p))
		semsg(_(e_trailing_characters_str), p);
	    లేకపోతే
		success = TRUE;
	    ఆపు;
	}

	పూర్ణాంకం	wrong_endname = FALSE;
	ఒకవేళ (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	లేకపోతే ఒకవేళ (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	లేకపోతే
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	ఒకవేళ (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), పంక్తి, చివర_name);
	    ఆపు;
	}

	ఒకవేళ (is_enum && !enum_చివర)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    ఒకవేళ (enum_parse_విలువలు(eap, cl, పంక్తి, &classmembers,
					&అంకె_enum_విలువలు, &enum_చివర) == FAIL)
		ఆపు;

	    ఒకవేళ (enum_చివర)
	    {
		// Add the enum "values" class variable.
		enum_కూడు_విలువలు_member(cl, &classmembers, అంకె_enum_విలువలు,
							&type_list);
		added_enum_విలువలు = TRUE;
	    }
	    కొనసాగు;
	}

	పూర్ణాంకం has_ప్రజా = FALSE;
	ఒకవేళ (checkforcmd(&p, "public", 3))
	{
	    ఒకవేళ (STRNCMP(పంక్తి, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), పంక్తి);
		ఆపు;
	    }
	    ఒకవేళ (is_interface)
	    {
		emsg(_(e_ప్రజా_variable_not_supported_లోపల_interface));
		ఆపు;
	    }
	    has_ప్రజా = TRUE;
	    p = skipwhite(పంక్తి + 6);

	    ఒకవేళ (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_ప్రజా_keyword_not_supported_for_పద్ధతి));
		ఆపు;
	    }

	    ఒకవేళ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_ప్రజా_must_be_followed_by_var_static_final_or_const));
		ఆపు;
	    }
	}

	పూర్ణాంకం abstract_పద్ధతి = FALSE;
	char_u *pa = p;
	ఒకవేళ (checkforcmd(&p, "abstract", 3))
	{
	    ఒకవేళ (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		ఆపు;
	    }

	    ఒకవేళ (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_వాడిన_లోపల_enum));
		ఆపు;
	    }

	    ఒకవేళ (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_వాడిన_లోపల_interface));
		ఆపు;
	    }

	    ఒకవేళ (!is_abstract)
	    {
		semsg(_(e_abstract_పద్ధతి_లోపల_concrete_class), pa);
		ఆపు;
	    }

	    p = skipwhite(pa + 8);
	    ఒకవేళ (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		ఆపు;
	    }

	    abstract_పద్ధతి = TRUE;
	}

	పూర్ణాంకం has_static = FALSE;
	char_u *ps = p;
	ఒకవేళ (checkforcmd(&p, "static", 4))
	{
	    ఒకవేళ (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		ఆపు;
	    }

	    ఒకవేళ (is_interface)
	    {
		emsg(_(e_static_member_not_supported_లోపల_interface));
		ఆపు;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    ఒకవేళ (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		ఆపు;
	    }
	}

	పూర్ణాంకం has_final = FALSE;
	పూర్ణాంకం has_var = FALSE;
	పూర్ణాంకం has_const = FALSE;
	ఒకవేళ (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	లేకపోతే ఒకవేళ (checkforcmd(&p, "final", 5))
	{
	    ఒకవేళ (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_లోపల_interface));
		ఆపు;
	    }
	    has_final = TRUE;
	}
	లేకపోతే ఒకవేళ (checkforcmd(&p, "const", 5))
	{
	    ఒకవేళ (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_లోపల_interface));
		ఆపు;
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
	ఒకవేళ (has_var || has_final || has_const)
	{
	    char_u *varname = p;
	    char_u *varname_చివర = ఖాళీ;
	    type_T *type = ఖాళీ;
	    char_u *init_సమాసం = ఖాళీ;
	    పూర్ణాంకం	    has_type = FALSE;

	    ఒకవేళ (!eval_isnamec1(*p))
	    {
		ఒకవేళ (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), పంక్తి);
		లేకపోతే
		    semsg(_(e_invalid_object_variable_declaration_str), పంక్తి);
		ఆపు;
	    }

	    ఒకవేళ (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_లోపల_interface),
			varname);
		ఆపు;
	    }

	    ఒకవేళ (parse_member(eap, పంక్తి, varname, has_ప్రజా,
			  &varname_చివర, &has_type, &type_list, &type,
			  !is_interface ? &init_సమాసం: ఖాళీ) == FAIL)
		ఆపు;

	    ఒకవేళ (is_reserved_varname(varname, varname_చివర)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_చివర))
	    {
		vim_free(init_సమాసం);
		ఆపు;
	    }
	    ఒకవేళ (కూడు_member(has_static ? &classmembers : &objmembers, varname,
				varname_చివర, has_ప్రజా, has_final, has_const,
				has_type, type, init_సమాసం) == FAIL)
	    {
		vim_free(init_సమాసం);
		ఆపు;
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
	లేకపోతే ఒకవేళ (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	పంక్తులు_గమ్యం_free;
	    పూర్ణాంకం		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    ఒకవేళ (has_ప్రజా)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_ప్రజా_keyword_not_supported_for_పద్ధతి));
		ఆపు;
	    }

	    ఒకవేళ (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_లోపల_class_str), పంక్తి);
		ఆపు;
	    }

	    ఒకవేళ (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_పద్ధతి_not_supported_లోపల_interface), p);
		ఆపు;
	    }

	    ఒకవేళ (has_static && !is_new && సురక్షిత_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_పద్ధతి_not_supported), p);
		ఆపు;
	    }

	    CLEAR_క్షేత్రం(ea);
	    ea.cmd = పంక్తి;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&పంక్తులు_గమ్యం_free, పరిమాణం(char_u *), 50);
	    పూర్ణాంకం class_flags;
	    ఒకవేళ (is_interface)
		class_flags = CF_INTERFACE;
	    లేకపోతే
		class_flags = abstract_పద్ధతి ? CF_ABSTRACT_పద్ధతి : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, ఖాళీ, &పంక్తులు_గమ్యం_free,
			class_flags, objmembers.ga_దత్తాంశం, objmembers.ga_len,
			ఖాళీ);
	    ga_clear_అక్షరశ్రేణులు(&పంక్తులు_గమ్యం_free);

	    ఒకవేళ (uf != ఖాళీ)
	    {
		ఒకవేళ (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_పద్ధతి_not_supported_లోపల_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    ఆపు;
		}

		// check for builtin method
		ఒకవేళ (!is_new && సురక్షిత_islower(*uf->uf_name) &&
					!object_check_builtin_పద్ధతి_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    ఆపు;
		}

		// Check the uf_name isn't used already.
		ఒకవేళ (is_duplicate_పద్ధతి(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    ఆపు;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		ఒకవేళ (ga_grow(fgap, 1) == OK)
		{
		    ఒకవేళ (is_new)
			uf->uf_flags |= FC_NEW;

		    ఒకవేళ (abstract_పద్ధతి)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_దత్తాంశం)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	లేకపోతే
	{
	    ఒకవేళ (is_class)
		semsg(_(e_not_valid_command_లోపల_class_str), పంక్తి);
	    లేకపోతే ఒకవేళ (is_enum)
		semsg(_(e_not_valid_command_లోపల_enum_str), పంక్తి);
	    లేకపోతే
		semsg(_(e_not_valid_command_లోపల_interface_str), పంక్తి);
	    ఆపు;
	}
    }

    ఒకవేళ (theline == ఖాళీ && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    ఒకవేళ (success && is_enum && (అంకె_enum_విలువలు == 0 || !added_enum_విలువలు))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_కూడు_విలువలు_member(cl, &classmembers, అంకె_enum_విలువలు,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    ఒకవేళ (success && extends != ఖాళీ)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    ఒకవేళ (success && extends_cl != ఖాళీ)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    ఒకవేళ (success && extends_cl != ఖాళీ)
    {
	ఒకవేళ (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	లేకపోతే
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    ఒకవేళ (success && !is_abstract && extends_cl != ఖాళీ
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, పరిమాణం(class_T *), 5);

    ఒకవేళ (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    ఒకవేళ (success && extends_cl != ఖాళీ)
	success = కూడు_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_దత్తాంశం;
    intf_లెక్క = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    ఒకవేళ (success)
	success = check_func_arg_పేర్లు(&classfunctions, &objmethods,
							&classmembers);

    ఒకవేళ (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	ఒకవేళ (extends_cl != ఖాళీ)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	ఒకవేళ (కూడు_members_గమ్యం_class(&classmembers,
				 ఖాళీ,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_లెక్క) == FAIL
		|| కూడు_members_గమ్యం_class(&objmembers,
				 extends_cl == ఖాళీ ? ఖాళీ
					       : extends_cl->class_obj_members,
				 extends_cl == ఖాళీ ? 0
					  : extends_cl->class_obj_member_లెక్క,
				 &cl->class_obj_members,
				 &cl->class_obj_member_లెక్క) == FAIL)
	    వెళ్ళు cleanup;

	ఒకవేళ (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_లెక్క = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    ఒకవేళ (cl->class_interfaces == ఖాళీ)
		వెళ్ళు cleanup;
	    కొరకు (పూర్ణాంకం i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_దత్తాంశం)[i];
	    VIM_CLEAR(ga_impl.ga_దత్తాంశం);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = ఖాళీ;
	}

	ఒకవేళ (cl->class_interface_లెక్క > 0 || extends_cl != ఖాళీ)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    ఒకవేళ (కూడు_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		వెళ్ళు cleanup;
	}

	పూర్ణాంకం	have_new = FALSE;
	ufunc_T	*class_func = ఖాళీ;
	కొరకు (పూర్ణాంకం i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_దత్తాంశం)[i];
	    ఒకవేళ (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		ఆపు;
	    }
	}

	ఒకవేళ (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	లేకపోతే ఒకవేళ ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    కూడు_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	ఒకవేళ (కూడు_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    వెళ్ళు cleanup;

	update_builtin_పద్ధతి_సూచీ(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	ఒకవేళ ((is_class || is_enum) && cl->class_class_member_లెక్క > 0)
	    ఒకవేళ (కూడు_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		తిరిగి;
	    }

	cl->class_type_list = type_list;

	ఒకవేళ (is_enum)
	{
	    // clear the constructor method names, so that an enum class cannot
	    // be instantiated
	    enum_clear_constructors(cl);
	}

	// TODO:
	// - Fill hashtab with object members and methods ?

	తిరిగి;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    ఒకవేళ (intf_classes != ఖాళీ)
    {
	కొరకు (పూర్ణాంకం i = 0; i < intf_లెక్క; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_అక్షరశ్రేణులు(&ga_impl);

    కొరకు (పూర్ణాంకం విడత = 1; విడత <= 2; ++విడత)
    {
	garray_T *gap = విడత == 1 ? &classmembers : &objmembers;
	ఒకవేళ (gap->ga_len == 0 || gap->ga_దత్తాంశం == ఖాళీ)
	    కొనసాగు;

	కొరకు (పూర్ణాంకం i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_దత్తాంశం) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    కొరకు (పూర్ణాంకం i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_దత్తాంశం)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    కొరకు (పూర్ణాంకం i = 0; i < classfunctions.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)classfunctions.ga_దత్తాంశం)[i];
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
    పూర్ణాంకం		is_object,
    char_u	*name,
    char_u	*name_చివర,
    పూర్ణాంకం		*member_idx)
{
    size_t	len = name_చివర - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    ఒకవేళ (m == ఖాళీ)
    {
	member_not_దొరికింది_సందేశం(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	తిరిగి &t_any;
    }

    తిరిగి m->ocm_type;
}

/*
 * Given a class or object variable index, return the variable type
 */
    type_T *
oc_member_type_by_idx(
    class_T	*cl,
    పూర్ణాంకం		is_object,
    పూర్ణాంకం		member_idx)
{
    ocmember_T	*m;
    పూర్ణాంకం		member_లెక్క;

    ఒకవేళ (is_object)
    {
	m = cl->class_obj_members;
	member_లెక్క = cl->class_obj_member_లెక్క;
    }
    లేకపోతే
    {
	m = cl->class_class_members;
	member_లెక్క = cl->class_class_member_లెక్క;
    }

    ఒకవేళ (member_idx >= member_లెక్క)
	తిరిగి ఖాళీ;

    తిరిగి m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    స్థిర శూన్య
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    శూన్య
typealias_unref(typealias_T *ta)
{
    ఒకవేళ (ta != ఖాళీ && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    శూన్య
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    ఒకవేళ (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_లోపల_vim9_script));
	తిరిగి;
    }

    ఒకవేళ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	తిరిగి;
    }

    ఒకవేళ (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_ప్రారంభం_with_uppercase_letter_str), arg);
	తిరిగి;
    }

    char_u *name_చివర = కనుగొను_name_చివర(arg, ఖాళీ, ఖాళీ, FNE_CHECK_ప్రారంభం);
    ఒకవేళ (!IS_WHITE_OR_NUL(*name_చివర))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	తిరిగి;
    }
    char_u *name_ప్రారంభం = arg;

    arg = skipwhite(name_చివర);
    ఒకవేళ (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	తిరిగి;
    }
    ఒకవేళ (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	తిరిగి;
    }
    arg++;
    arg = skipwhite(arg);

    ఒకవేళ (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	తిరిగి;
    }

    scriptitem_T    *si = SCRIPT_అంశం(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, ఖాళీ, ఖాళీ, TRUE);
    ఒకవేళ (type == ఖాళీ)
	తిరిగి;

    ఒకవేళ (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	తిరిగి;
    }

    పూర్ణాంకం cc = *name_చివర;
    *name_చివర = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    ఒకవేళ (eval_variable_import(name_ప్రారంభం, &tv) == OK)
    {
	ఒకవేళ (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_ప్రారంభం);
	లేకపోతే
	    semsg(_(e_redefining_script_అంశం_str), name_ప్రారంభం);
	clear_tv(&tv);
	వెళ్ళు పూర్తి;
    }

    // Create a script-local variable for the type alias.
    ఒకవేళ (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_తాళం = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_ప్రారంభం);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_లోతు = parse_type_లోతు();
    }
    లేకపోతే
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_తాళం = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    సమితి_var_const(name_ప్రారంభం, current_sctx.sc_sid, ఖాళీ, &tv, FALSE,
						కేటాయించు_CONST | కేటాయించు_FINAL, 0);

పూర్తి:
    *name_చివర = cc;
}

/*
 * Returns OK if a member variable named "name" is present in the class "cl".
 * Otherwise returns FAIL.  If found, the member variable typval is set in
 * "rettv".  If "is_object" is TRUE, then the object member variable table is
 * searched.  Otherwise the class member variable table is searched.
 */
    పూర్ణాంకం
get_member_tv(
    class_T	*cl,
    పూర్ణాంకం		is_object,
    char_u	*name,
    size_t	namelen,
    class_T	*current_class,
    typval_T	*rettv)
{
    ocmember_T *m;
    పూర్ణాంకం		m_idx;

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, namelen,
								&m_idx);
    ఒకవేళ (m == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (*name == '_' && (current_class == ఖాళీ ||
				!class_instance_యొక్క(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	తిరిగి FAIL;
    }

    ఒకవేళ (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	సమితి_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    లేకపోతే
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	సమితి_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    తిరిగి OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    స్థిర పూర్ణాంకం
call_oc_పద్ధతి(
    class_T	*cl,
    char_u	*name,
    size_t	len,
    char_u	*name_చివర,
    evalarg_T	*evalarg,
    char_u	**arg,
    gfargs_tab_T	*gfatab,	// generic types
    typval_T	*rettv)
{
    ufunc_T	*fp;
    typval_T	argvars[MAX_FUNC_వాదనలు + 1];
    పూర్ణాంకం		argcount = 0;
    ocmember_T	*ocm = ఖాళీ;
    పూర్ణాంకం		m_idx;

    fp = పద్ధతి_lookup(cl, rettv->v_type, name, len, ఖాళీ);
    ఒకవేళ (fp == ఖాళీ)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	ఒకవేళ (ocm == ఖాళీ || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    పద్ధతి_not_దొరికింది_సందేశం(cl, rettv->v_type, name, len);
	    తిరిగి FAIL;
	}

	ఒకవేళ (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    తిరిగి FAIL;
	}

	ఒకవేళ (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	లేకపోతే
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_చివర;
	తిరిగి OK;
    }

    ఒకవేళ (ocm == ఖాళీ && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_పద్ధతి_str), fp->uf_name);
	తిరిగి FAIL;
    }

    // process generic function call
    ఒకవేళ (fp != ఖాళీ)
    {
	fp = generic_func_get(fp, gfatab);
	ఒకవేళ (fp == ఖాళీ)
	    తిరిగి FAIL;
    }

    char_u *argp = name_చివర;
    పూర్ణాంకం ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    ఒకవేళ (ret == FAIL)
    {
	అయితే (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	తిరిగి FAIL;
    }

    funcexe_T funcexe;
    CLEAR_క్షేత్రం(funcexe);
    funcexe.fe_evaluate = TRUE;
    ఒకవేళ (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    ఒకవేళ (evalarg != ఖాళీ)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    పూర్ణాంకం error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								ఖాళీ);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    కొరకు (పూర్ణాంకం idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ఒకవేళ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_దొరికింది_var);
	తిరిగి FAIL;
    }
    *arg = argp;

    తిరిగి OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    పూర్ణాంకం
obj_పద్ధతి_గమ్యం_partial_tv(object_T *obj, ufunc_T *obj_పద్ధతి, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    ఒకవేళ (pt == ఖాళీ)
	తిరిగి FAIL;

    pt->pt_refcount = 1;
    ఒకవేళ (obj != ఖాళీ)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_పద్ధతి;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    తిరిగి OK;
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
    పూర్ణాంకం
class_object_సూచీ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    పూర్ణాంకం		verbose వాడని)	// give error messages
{
    పూర్ణాంకం	ret = FAIL;

    ఒకవేళ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	తిరిగి FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_చివర = కనుగొను_name_చివర(name, ఖాళీ, ఖాళీ, FNE_CHECK_ప్రారంభం);
    ఒకవేళ (name_చివర == name)
	తిరిగి FAIL;
    size_t len = name_చివర - name;

    పూర్ణాంకం did_emsg_save = did_emsg;
    class_T *cl;
    ఒకవేళ (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    లేకపోతే // VAR_OBJECT
    {
	ఒకవేళ (rettv->vval.v_object == ఖాళీ)
	{
	    emsg(_(e_using_null_object));
	    తిరిగి FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    ఒకవేళ (cl == ఖాళీ)
    {
	emsg(_(e_incomplete_type));
	తిరిగి FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_వాదనలు_పట్టిక_init(&gfatab);

    ఒకవేళ (*name_చివర == '<')
    {
	cctx_T	    *cctx = ఖాళీ;

	ఒకవేళ (evalarg != ఖాళీ)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_చివర = parse_generic_func_type_వాదనలు(name, len, name + len,
						&gfatab, cctx);
	ఒకవేళ (name_చివర == ఖాళీ)
	    వెళ్ళు పూర్తి;
    }

    ఒకవేళ (*name_చివర == '(')
	// Invoke the class or object method
	ret = call_oc_పద్ధతి(cl, name, len, name_చివర, evalarg, arg,
							&gfatab, rettv);
    లేకపోతే ఒకవేళ (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	పూర్ణాంకం is_object = rettv->v_type == VAR_OBJECT;
	ఒకవేళ (get_member_tv(cl, is_object, name, len, ఖాళీ, rettv) == OK)
	{
	    *arg = name_చివర;
	    ret = OK;
	    వెళ్ళు పూర్తి;
	}

	// could be a class method or an object method
	పూర్ణాంకం	fidx;
	ufunc_T	*fp = పద్ధతి_lookup(cl, rettv->v_type, name, len, &fidx);
	ఒకవేళ (fp != ఖాళీ)
	{
	    // Protected methods are not accessible outside the class
	    ఒకవేళ (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_పద్ధతి_str), fp->uf_name);
		వెళ్ళు పూర్తి;
	    }

	    ఒకవేళ (obj_పద్ధతి_గమ్యం_partial_tv(is_object ? rettv->vval.v_object :
						ఖాళీ, fp, rettv) == FAIL)
		వెళ్ళు పూర్తి;

	    *arg = name_చివర;
	    ret = OK;
	    వెళ్ళు పూర్తి;
	}

	ఒకవేళ (did_emsg == did_emsg_save)
	    member_not_దొరికింది_సందేశం(cl, rettv->v_type, name, len);
    }

పూర్తి:
    generic_func_వాదనలు_పట్టిక_clear(&gfatab);

    తిరిగి ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
కనుగొను_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_చివర = కనుగొను_name_చివర(name, ఖాళీ, ఖాళీ, FNE_CHECK_ప్రారంభం);
    ఒకవేళ (name_చివర == name || *name_చివర != '.')
	తిరిగి ఖాళీ;

    ufunc_T	*fp = ఖాళీ;
    size_t	len = name_చివర - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    ఒకవేళ (eval_variable(name, (పూర్ణాంకం)len,
				    0, &tv, ఖాళీ, EVAL_VAR_NOAUTOLOAD) == FAIL)
	తిరిగి ఖాళీ;
    ఒకవేళ (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	వెళ్ళు fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    ఒకవేళ (cl == ఖాళీ)
	వెళ్ళు fail_after_eval;
    char_u *fname = name_చివర + 1;
    char_u *fname_చివర = కనుగొను_name_చివర(fname, ఖాళీ, ఖాళీ, FNE_CHECK_ప్రారంభం);
    ఒకవేళ (fname_చివర == fname)
	వెళ్ళు fail_after_eval;
    len = fname_చివర - fname;

    fp = పద్ధతి_lookup(cl, tv.v_type, fname, len, ఖాళీ);

    ఒకవేళ (fp != ఖాళీ)
    {
	fp = eval_generic_func(fp, fname, &fname_చివర);
	ఒకవేళ (fp != ఖాళీ)
	    *arg = fname_చివర;
    }

fail_after_eval:
    clear_tv(&tv);
    తిరిగి fp;
}

/*
 * Returns the index of class variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    పూర్ణాంకం
class_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    పూర్ణాంకం idx;
    class_member_lookup(cl, name, namelen, &idx);
    తిరిగి idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, పూర్ణాంకం *idx)
{
    ocmember_T	*ret_m = ఖాళీ;
    పూర్ణాంకం		ret_idx = -1;
    కొరకు (పూర్ణాంకం i = 0; i < cl->class_class_member_లెక్క; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	ఒకవేళ (namelen)
	{
	    ఒకవేళ (namelen == m->ocm_name.నిడివి
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ఆపు;
	    }
	}
	లేకపోతే ఒకవేళ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ఆపు;
	}
    }
    ఒకవేళ (idx != ఖాళీ)
	*idx = ret_idx;
    తిరిగి ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    స్థిర ufunc_T *
class_పద్ధతి_lookup(class_T *cl, char_u *name, size_t namelen, పూర్ణాంకం *idx)
{
    ufunc_T	*ret_fp = ఖాళీ;
    పూర్ణాంకం		ret_idx = -1;
    కొరకు (పూర్ణాంకం i = 0; i < cl->class_class_function_లెక్క; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	ఒకవేళ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ఆపు;
	}
    }
    ఒకవేళ (idx != ఖాళీ)
	*idx = ret_idx;
    తిరిగి ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    పూర్ణాంకం
class_పద్ధతి_idx(class_T *cl, char_u *name, size_t namelen)
{
    పూర్ణాంకం idx;
    class_పద్ధతి_lookup(cl, name, namelen, &idx);
    తిరిగి idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    స్థిర పూర్ణాంకం
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    పూర్ణాంకం idx;
    object_member_lookup(cl, name, namelen, &idx);
    తిరిగి idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, పూర్ణాంకం *idx)
{
    ocmember_T	*ret_m = ఖాళీ;
    పూర్ణాంకం		ret_idx = -1;
    కొరకు (పూర్ణాంకం i = 0; i < cl->class_obj_member_లెక్క; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	ఒకవేళ (namelen)
	{
	    ఒకవేళ (namelen == m->ocm_name.నిడివి
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		ఆపు;
	    }
	}
	లేకపోతే ఒకవేళ (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    ఆపు;
	}
    }
    ఒకవేళ (idx != ఖాళీ)
	*idx = ret_idx;
    తిరిగి ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    స్థిర ufunc_T *
object_పద్ధతి_lookup(class_T *cl, char_u *name, size_t namelen, పూర్ణాంకం *idx)
{
    ufunc_T	*ret_fp = ఖాళీ;
    పూర్ణాంకం		ret_idx = -1;
    కొరకు (పూర్ణాంకం i = 0; i < cl->class_obj_పద్ధతి_లెక్క; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	ఒకవేళ (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    ఆపు;
	}
    }
    ఒకవేళ (idx != ఖాళీ)
	*idx = ret_idx;
    తిరిగి ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    పూర్ణాంకం
object_పద్ధతి_idx(class_T *cl, char_u *name, size_t namelen)
{
    పూర్ణాంకం idx;
    object_పద్ధతి_lookup(cl, name, namelen, &idx);
    తిరిగి idx;
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
    పూర్ణాంకం		*idx)
{
    ఒకవేళ (v_type == VAR_CLASS)
	తిరిగి class_member_lookup(cl, name, namelen, idx);

    తిరిగి object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    స్థిర class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_దొరికింది = ఖాళీ;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_దొరికింది = ఖాళీ;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    కొరకు (class_T *super = cl; super != ఖాళీ; super = super->class_extends)
    {
	class_T		*cl_tmp = ఖాళీ;
	ocmember_T	*m = ఖాళీ;
	ఒకవేళ (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    ఒకవేళ ((m = object_member_lookup(super, name, len, ఖాళీ)) != ఖాళీ)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	ఒకవేళ (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    ఒకవేళ (( m = class_member_lookup(super, name, len, ఖాళీ)) != ఖాళీ)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	ఒకవేళ (cl_tmp == ఖాళీ)
	    ఆపు;  // member is not in this or any super class.
	cl_దొరికింది = cl_tmp;
	m_దొరికింది = m;
    }
    ఒకవేళ (p_m != ఖాళీ)
	*p_m = m_దొరికింది;
    తిరిగి cl_దొరికింది;
}

/*
 * Lookup a class or object method by name.  If v_type is VAR_CLASS, then
 * lookup a class method and if it is VAR_OBJECT, then lookup a object method.
 *
 * Returns a pointer to the method structure if variable is found.
 * Otherwise returns NULL.  The method variable index is set in "*idx".
 */
    ufunc_T *
పద్ధతి_lookup(
    class_T	*cl,
    vartype_T	v_type,
    char_u	*name,
    size_t	namelen,
    పూర్ణాంకం		*idx)
{
    ఒకవేళ (v_type == VAR_CLASS)
	తిరిగి class_పద్ధతి_lookup(cl, name, namelen, idx);
    లేకపోతే
	తిరిగి object_పద్ధతి_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    పూర్ణాంకం
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    కొరకు (cctx_T *cctx = cctx_arg; cctx != ఖాళీ; cctx = cctx->ctx_outer)
	ఒకవేళ (cctx->ctx_ufunc != ఖాళీ
			&& class_instance_యొక్క(cctx->ctx_ufunc->uf_class, cl))
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Return TRUE if object/class variable "m" is read-only.
 * Also give an error message.
 */
    పూర్ణాంకం
oc_var_check_ro(class_T *cl, ocmember_T *m)
{
    ఒకవేళ (m->ocm_flags & (OCMFLAG_FINAL | OCMFLAG_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str_లోపల_class_str),
		m->ocm_name.string, cl->class_name.string);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    శూన్య
obj_తాళం_const_vars(object_T *obj)
{
    కొరకు (పూర్ణాంకం i = 0; i < obj->obj_class->class_obj_member_లెక్క; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	ఒకవేళ (ocm->ocm_flags & OCMFLAG_CONST)
	{
	    typval_T *mtv = ((typval_T *)(obj + 1)) + i;
	    అంశం_తాళం(mtv, DICT_MAXNEST, TRUE, TRUE);
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
    పూర్ణాంకం		sz;

    ఒకవేళ (cl == ఖాళీ)
	తిరిగి ఖాళీ;

    sz = పరిమాణం(object_T) + cl->class_obj_member_లెక్క * పరిమాణం(typval_T);
    obj = alloc_clear(sz);
    ఒకవేళ (obj == ఖాళీ)
	తిరిగి ఖాళీ;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    ఒకవేళ (IS_ENUM(cl))
	enum_సమితి_అంతర్గత_obj_vars(cl, obj);

    తిరిగి obj;
}

/*
 * Make a copy of an object.
 */
    శూన్య
copy_object(typval_T *from, typval_T *గమ్యం)
{
    ఒకవేళ (from->vval.v_object == ఖాళీ)
	గమ్యం->vval.v_object = ఖాళీ;
    లేకపోతే
    {
	గమ్యం->vval.v_object = from->vval.v_object;
	++గమ్యం->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    శూన్య
copy_class(typval_T *from, typval_T *గమ్యం)
{
    ఒకవేళ (from->vval.v_class == ఖాళీ)
	గమ్యం->vval.v_class = ఖాళీ;
    లేకపోతే
    {
	గమ్యం->vval.v_class = from->vval.v_class;
	++గమ్యం->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    స్థిర శూన్య
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    కొరకు (పూర్ణాంకం i = 0; i < cl->class_interface_లెక్క; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	ఒకవేళ (cl->class_interfaces_cl[i] != ఖాళీ)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *తదుపరి;
    కొరకు (itf2class_T *i2c = cl->class_itf2class; i2c != ఖాళీ; i2c = తదుపరి)
    {
	తదుపరి = i2c->i2c_తదుపరి;
	vim_free(i2c);
    }

    కొరకు (పూర్ణాంకం i = 0; i < cl->class_class_member_లెక్క; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	ఒకవేళ (cl->class_members_tv != ఖాళీ)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    కొరకు (పూర్ణాంకం i = 0; i < cl->class_obj_member_లెక్క; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    కొరకు (పూర్ణాంకం i = 0; i < cl->class_class_function_లెక్క; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	ఒకవేళ (IS_ENUM(cl) && IS_CONSTRUCTOR_పద్ధతి(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    కొరకు (పూర్ణాంకం i = 0; i < cl->class_obj_పద్ధతి_లెక్క; ++i)
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
    స్థిర పూర్ణాంకం
class_get_selfrefs(class_T *cl)
{
    పూర్ణాంకం		self_refs = 0;
    typval_T	*tv;

    కొరకు (పూర్ణాంకం i = 0; i < cl->class_class_member_లెక్క; ++i)
    {
	tv = &cl->class_members_tv[i];
	ఒకవేళ (tv->v_type == VAR_OBJECT && tv->vval.v_object != ఖాళీ
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    తిరిగి self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    స్థిర పూర్ణాంకం
can_free_enum(class_T *cl)
{
    కొరకు (పూర్ణాంకం i = 0; i < cl->class_class_member_లెక్క; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	ఒకవేళ (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    తిరిగి TRUE;

	ఒకవేళ (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    ఒకవేళ (tv->vval.v_list->lv_refcount > 1)
		తిరిగి FALSE;
	    ఆపు;
	}

	ఒకవేళ (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    తిరిగి FALSE;
    }

    తిరిగి TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    స్థిర పూర్ణాంకం
can_free_class(class_T *cl)
{
    పూర్ణాంకం		can_free = FALSE;
    పూర్ణాంకం		self_refs = 0;

    ఒకవేళ (IS_ENUM(cl) && !can_free_enum(cl))
	తిరిగి FALSE;

    ఒకవేళ (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    ఒకవేళ (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    తిరిగి can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    శూన్య
class_unref(class_T *cl)
{
    ఒకవేళ (cl == ఖాళీ)
	తిరిగి;

    --cl->class_refcount;

    ఒకవేళ (cl->class_name.string == ఖాళీ)
	తిరిగి;

    ఒకవేళ (can_free_class(cl))
	class_free(cl);
}

/*
 * Go through the list of all classes and free items without "copyID".
 */
    పూర్ణాంకం
class_free_nonref(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		did_free = FALSE;

    కొరకు (class_T *cl = మొదటి_class; cl != ఖాళీ; cl = తదుపరి_nonref_class)
    {
	తదుపరి_nonref_class = cl->class_తదుపరి_వాడిన;
	ఒకవేళ ((cl->class_copyID & COPYID_ముసుగు) != (copyID & COPYID_ముసుగు))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    తదుపరి_nonref_class = ఖాళీ;
    తిరిగి did_free;
}

    పూర్ణాంకం
సమితి_ref_లోపల_classes(పూర్ణాంకం copyID)
{
    కొరకు (class_T *cl = మొదటి_class; cl != ఖాళీ; cl = cl->class_తదుపరి_వాడిన)
	సమితి_ref_లోపల_అంశం_class(cl, copyID, ఖాళీ, ఖాళీ, ఖాళీ);

    తిరిగి FALSE;
}

స్థిర object_T *మొదటి_object = ఖాళీ;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    శూన్య
object_created(object_T *obj)
{
    ఒకవేళ (మొదటి_object != ఖాళీ)
    {
	obj->obj_తదుపరి_వాడిన = మొదటి_object;
	మొదటి_object->obj_prev_వాడిన = obj;
    }
    మొదటి_object = obj;
}

/*
 * Call this function when an object has been cleared and is about to be freed.
 * It is removed from the list headed by "first_object".
 */
    స్థిర శూన్య
object_cleared(object_T *obj)
{
    ఒకవేళ (obj->obj_తదుపరి_వాడిన != ఖాళీ)
	obj->obj_తదుపరి_వాడిన->obj_prev_వాడిన = obj->obj_prev_వాడిన;
    ఒకవేళ (obj->obj_prev_వాడిన != ఖాళీ)
	obj->obj_prev_వాడిన->obj_తదుపరి_వాడిన = obj->obj_తదుపరి_వాడిన;
    లేకపోతే ఒకవేళ (మొదటి_object == obj)
	మొదటి_object = obj->obj_తదుపరి_వాడిన;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    స్థిర శూన్య
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    ఒకవేళ (!cl)
	తిరిగి;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = గరిష్ఠ_పూర్ణాంకం;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    కొరకు (పూర్ణాంకం i = 0; i < cl->class_obj_member_లెక్క; ++i)
	clear_tv(tv + i);
}

    స్థిర శూన్య
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    ఒకవేళ (!cl)
	తిరిగి;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    స్థిర శూన్య
object_free(object_T *obj)
{
    ఒకవేళ (లోపల_free_unref_వస్తువులు)
	తిరిగి;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    శూన్య
object_unref(object_T *obj)
{
    ఒకవేళ (obj != ఖాళీ && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    పూర్ణాంకం
object_free_nonref(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		did_free = FALSE;

    కొరకు (object_T *obj = మొదటి_object; obj != ఖాళీ; obj = obj->obj_తదుపరి_వాడిన)
    {
	ఒకవేళ ((obj->obj_copyID & COPYID_ముసుగు) != (copyID & COPYID_ముసుగు))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    తిరిగి did_free;
}

    శూన్య
object_free_వస్తువులు(పూర్ణాంకం copyID)
{
    object_T	*obj_తదుపరి;

    కొరకు (object_T *obj = మొదటి_object; obj != ఖాళీ; obj = obj_తదుపరి)
    {
	obj_తదుపరి = obj->obj_తదుపరి_వాడిన;
	ఒకవేళ ((obj->obj_copyID & COPYID_ముసుగు) != (copyID & COPYID_ముసుగు))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    శూన్య
emsg_var_cl_define(అక్షరం *సందేశం, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    ఒకవేళ (cl_def != ఖాళీ)
	semsg(_(సందేశం), m->ocm_name.string, cl_def->class_name.string);
    లేకపోతే
	emsg(_(e_అంతర్గత_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    శూన్య
పద్ధతి_not_దొరికింది_సందేశం(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *పద్ధతి_name = vim_strnsave(name, len);
    ఒకవేళ ((v_type == VAR_OBJECT)
	    && (class_పద్ధతి_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	ఒకవేళ (*name == '_')
	    semsg(_(e_cannot_access_protected_పద్ధతి_str), పద్ధతి_name);
	లేకపోతే
	    semsg(_(e_class_పద్ధతి_str_accessible_only_using_class_str),
		    పద్ధతి_name, cl->class_name.string);
    }
    లేకపోతే ఒకవేళ ((v_type == VAR_CLASS)
	    && (object_పద్ధతి_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	ఒకవేళ (*name == '_')
	    semsg(_(e_cannot_access_protected_పద్ధతి_str), పద్ధతి_name);
	లేకపోతే
	    semsg(_(e_object_పద్ధతి_str_accessible_only_using_object_str),
		    పద్ధతి_name, cl->class_name.string);
    }
    లేకపోతే
	semsg(_(e_పద్ధతి_not_దొరికింది_on_class_str_str), పద్ధతి_name,
		cl->class_name.string);
    vim_free(పద్ధతి_name);
}

/*
 * Echo a class or object member not found message.
 */
    శూన్య
member_not_దొరికింది_సందేశం(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    ఒకవేళ (v_type == VAR_OBJECT)
    {
	ఒకవేళ (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	లేకపోతే
	    semsg(_(e_variable_not_దొరికింది_on_object_str_str), varname,
		    cl->class_name.string);
    }
    లేకపోతే
    {
	ఒకవేళ (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	లేకపోతే
	{
	    ఒకవేళ (IS_ENUM(cl))
		semsg(_(e_enum_విలువ_str_not_దొరికింది_లోపల_enum_str),
			varname, cl->class_name.string);
	    లేకపోతే
		semsg(_(e_class_variable_str_not_దొరికింది_లోపల_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    శూన్య
defcompile_class(class_T *cl)
{
    కొరకు (పూర్ణాంకం loop = 1; loop <= 2; ++loop)
    {
	పూర్ణాంకం func_లెక్క = loop == 1 ? cl->class_class_function_లెక్క
						: cl->class_obj_పద్ధతి_లెక్క;
	కొరకు (పూర్ణాంకం i = 0; i < func_లెక్క; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    ఒకవేళ (!IS_ABSTRACT_పద్ధతి(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    శూన్య
defcompile_classes_లోపల_script(శూన్య)
{
    కొరకు (class_T *cl = మొదటి_class; cl != ఖాళీ; cl = cl->class_తదుపరి_వాడిన)
    {
	ఒకవేళ (eval_variable(cl->class_name.string, 0, 0, ఖాళీ, ఖాళీ,
			EVAL_VAR_NOAUTOLOAD | EVAL_VAR_NO_FUNC) != FAIL)
	    defcompile_class(cl);
    }
}

/*
 * Returns TRUE if "name" is the name of a class.  The typval for the class is
 * returned in "rettv".
 */
    పూర్ణాంకం
is_class_name(char_u *name, typval_T *rettv)
{
    rettv->v_type = VAR_UNKNOWN;

    ఒకవేళ (eval_variable(name, 0, 0, rettv, ఖాళీ, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	తిరిగి rettv->v_type == VAR_CLASS;
    తిరిగి FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
object_call_builtin_పద్ధతి(
    object_T		*obj,
    class_builtin_T	builtin_పద్ధతి,
    పూర్ణాంకం			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    పూర్ణాంకం	    midx;

    ఒకవేళ (obj == ఖాళీ)
	తిరిగి FAIL;

    uf = class_get_builtin_పద్ధతి(obj->obj_class, builtin_పద్ధతి, &midx);
    ఒకవేళ (uf == ఖాళీ)
	తిరిగి FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    పూర్ణాంకం		r;

    ఒకవేళ (fc == ఖాళీ)
	తిరిగి FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, ఖాళీ, obj, fc, rettv);

    remove_funccal();

    తిరిగి r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    పూర్ణాంకం
object_ఖాళీలు(object_T *obj)
{
    typval_T	rettv;

    ఒకవేళ (object_call_builtin_పద్ధతి(obj, CLASS_BUILTIN_ఖాళీలు, 0, ఖాళీ, &rettv)
								== FAIL)
	తిరిగి TRUE;

    తిరిగి tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    పూర్ణాంకం
object_len(object_T *obj)
{
    typval_T	rettv;

    ఒకవేళ (object_call_builtin_పద్ధతి(obj, CLASS_BUILTIN_LEN, 0, ఖాళీ, &rettv)
								== FAIL)
	తిరిగి 0;

    తిరిగి tv_గమ్యం_సంఖ్య(&rettv);
}

/*
 * Return TRUE when two objects have exactly the same values.
 */
    పూర్ణాంకం
object_equal(
	object_T *o1,
	object_T *o2,
	పూర్ణాంకం	ic)	// ignore case for strings
{
    class_T *cl1, *cl2;

    ఒకవేళ (o1 == o2)
	తిరిగి TRUE;
    ఒకవేళ (o1 == ఖాళీ || o2 == ఖాళీ)
	తిరిగి FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    ఒకవేళ (cl1 != cl2 || cl1 == ఖాళీ || cl2 == ఖాళీ)
	తిరిగి FALSE;

    కొరకు (పూర్ణాంకం i = 0; i < cl1->class_obj_member_లెక్క; ++i)
	ఒకవేళ (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    తిరిగి FALSE;

    తిరిగి TRUE;
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
    పూర్ణాంకం		copyID,
    పూర్ణాంకం		echo_style,
    పూర్ణాంకం		restore_copyID,
    పూర్ణాంకం		సంయుక్త_val)
{
    typval_T	rettv;

    ఒకవేళ (object_call_builtin_పద్ధతి(obj, CLASS_BUILTIN_STRING, 0, ఖాళీ, &rettv)
								== OK
					&& rettv.vval.v_string != ఖాళీ)
	తిరిగి rettv.vval.v_string;

    పూర్ణాంకం		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    ఒకవేళ (cl != ఖాళీ && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.నిడివి);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    లేకపోతే
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	ఒకవేళ (cl == ఖాళీ)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	లేకపోతే
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.నిడివి);
    }
    ఒకవేళ (cl != ఖాళీ)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	కొరకు (పూర్ణాంకం i = 0; i < cl->class_obj_member_లెక్క; ++i)
	{
	    ఒకవేళ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.నిడివి);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = ఖాళీ;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, సంయుక్త_val);
	    ఒకవేళ (s != ఖాళీ)
		ga_concat(&ga, s);
	    vim_free(tf);
	    ఒకవేళ (s == ఖాళీ || did_echo_string_emsg)
	    {
		ok = FAIL;
		ఆపు;
	    }
	    పంక్తి_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    ఒకవేళ (ok == FAIL)
    {
	vim_free(ga.ga_దత్తాంశం);
	తిరిగి ఖాళీ;
    }
    ga_append(&ga, NUL);
    తిరిగి (char_u *)ga.ga_దత్తాంశం;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    పూర్ణాంకం
class_instance_యొక్క(class_T *cl, class_T *other_cl)
{
    ఒకవేళ (cl == other_cl)
	తిరిగి TRUE;

    // Recursively check the base classes.
    కొరకు (; cl != ఖాళీ; cl = cl->class_extends)
    {
	ఒకవేళ (cl == other_cl)
	    తిరిగి TRUE;
	// Check the implemented interfaces and the super interfaces
	కొరకు (పూర్ణాంకం i = cl->class_interface_లెక్క - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    అయితే (intf != ఖాళీ)
	    {
		ఒకవేళ (intf == other_cl)
		    తిరిగి TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    తిరిగి FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    శూన్య
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_సంఖ్య = VVAL_FALSE;

    ఒకవేళ (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_వాదనలు(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (object_tv->vval.v_object == ఖాళీ)
    {
	ఒకవేళ (classinfo_tv->vval.v_class == ఖాళీ)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_సంఖ్య = VVAL_TRUE;
	తిరిగి;
    }

    కొరకు (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	ఒకవేళ (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	లేకపోతే
	    c = classinfo_tv->vval.v_class;

	ఒకవేళ (class_instance_యొక్క(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_సంఖ్య = VVAL_TRUE;
	    తిరిగి;
	}
    }
}

#అయితే_ముగింపు // FEAT_EVAL
