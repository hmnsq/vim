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

#تعریف_کرو USING_FLOAT_STUFF
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

ساکن class_T *پہلا_class = تہی;
ساکن class_T *اگلا_nonref_class = تہی;

/*
 * Call this function when a class has been created.  It will be added to the
 * list headed by "first_class".
 */
    ساکن خالی
class_created(class_T *cl)
{
    اگر (پہلا_class != تہی)
    {
	cl->class_اگلا_مستعمل = پہلا_class;
	پہلا_class->class_پچھلا_مستعمل = cl;
    }
    پہلا_class = cl;
}

/*
 * Call this function when a class has been cleared and is about to be freed.
 * It is removed from the list headed by "first_class".
 */
    ساکن خالی
class_cleared(class_T *cl)
{
    اگر (cl->class_اگلا_مستعمل != تہی)
	cl->class_اگلا_مستعمل->class_پچھلا_مستعمل = cl->class_پچھلا_مستعمل;
    اگر (cl->class_پچھلا_مستعمل != تہی)
	cl->class_پچھلا_مستعمل->class_اگلا_مستعمل = cl->class_اگلا_مستعمل;
    ورنہ اگر (پہلا_class == cl)
	پہلا_class = cl->class_اگلا_مستعمل;

    // update the next class to check if needed
    اگر (cl == اگلا_nonref_class)
	اگلا_nonref_class = cl->class_اگلا_مستعمل;
}

/*
 * Parse a member declaration, both object and class member.
 * Returns OK or FAIL.  When OK then "varname_end" is set to just after the
 * variable name and "type_ret" is set to the declared or detected type.
 * "init_expr" is set to the initialisation expression (allocated), if there is
 * one.  For an interface "init_expr" is NULL.
 */
    ساکن صحیح
parse_member(
    exarg_T	*eap,
    char_u	*سطر,
    char_u	*varname,
    صحیح		has_عوامی,	    // TRUE if "public" seen before "varname"
    char_u	**varname_انتہا,
    صحیح		*has_type,
    garray_T	*type_list,
    type_T	**type_ret,
    char_u	**init_اظہاریہ)
{
    *varname_انتہا = مقصد_name_انتہا(varname, FALSE);
    اگر (*varname == '_' && has_عوامی)
    {
	semsg(_(e_عوامی_variable_name_cannot_آغاز_with_underscore_str), سطر);
	واپس FAIL;
    }

    char_u *colon = skipwhite(*varname_انتہا);
    char_u *type_arg = colon;
    type_T *type = تہی;
    *has_type = FALSE;
    اگر (*colon == ':')
    {
	اگر (VIM_ISWHITE(**varname_انتہا))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), varname);
	    واپس FAIL;
	}
	اگر (!VIM_ISWHITE(colon[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", varname);
	    واپس FAIL;
	}
	type_arg = skipwhite(colon + 1);
	type = parse_type(&type_arg, type_list, تہی, تہی, TRUE);
	اگر (type == تہی || !valid_declaration_type(type))
	    واپس FAIL;
	*has_type = TRUE;
    }

    char_u *init_arg = skipwhite(type_arg);
    اگر (type == تہی && *init_arg != '=')
    {
	emsg(_(e_type_or_initialization_required));
	واپس FAIL;
    }

    اگر (init_اظہاریہ == تہی && *init_arg == '=')
    {
	emsg(_(e_cannot_initialize_variable_اندر_interface));
	واپس FAIL;
    }

    اگر (*init_arg == '=')
    {
	evalarg_T evalarg;
	char_u *اظہاریہ_آغاز, *اظہاریہ_انتہا;

	اگر (!VIM_ISWHITE(init_arg[-1]) || !VIM_ISWHITE(init_arg[1]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
							"=", type_arg);
	    واپس FAIL;
	}
	init_arg = skipwhite(init_arg + 1);

	fill_evalarg_from_eap(&evalarg, eap, FALSE);
	(خالی)skip_اظہاریہ_concatenate(&init_arg, &اظہاریہ_آغاز, &اظہاریہ_انتہا, &evalarg);

	init_arg = skipwhite(init_arg);
	اگر (*init_arg != NUL && !vim9_comment_آغاز(init_arg))
	{
	    semsg(_(e_trailing_characters_str), init_arg);
	    clear_evalarg(&evalarg, تہی);
	    واپس FAIL;
	}

	// No type specified for the member.  Set it to "any" and the correct
	// type will be set when the object is instantiated.
	اگر (type == تہی)
	    type = &t_any;

	*init_اظہاریہ = vim_strnsave(اظہاریہ_آغاز, اظہاریہ_انتہا - اظہاریہ_آغاز);
	// Free the memory pointed by expr_start.
	clear_evalarg(&evalarg, تہی);
    }
    ورنہ اگر (!valid_declaration_type(type))
	واپس FAIL;

    *type_ret = type;
    واپس OK;
}

/*
 * Add a member to an object or a class.
 * Returns OK when successful, "init_expr" will be consumed then.
 * Returns OK on success and FAIL on memory allocation failure (caller might
 * need to free "init_expr").
 */
    ساکن صحیح
جوڑو_member(
    garray_T	*gap,
    char_u	*varname,
    char_u	*varname_انتہا,
    صحیح		has_عوامی,
    صحیح		has_final,
    صحیح		has_const,
    صحیح		has_type,
    type_T	*type,
    char_u	*init_اظہاریہ)
{
    اگر (ga_grow(gap, 1) == FAIL)
	واپس FAIL;
    ocmember_T *m = ((ocmember_T *)gap->ga_ڈیٹا) + gap->ga_len;
    m->ocm_name.لمبائی = (size_t)(varname_انتہا - varname);
    m->ocm_name.string = vim_strnsave(varname, m->ocm_name.لمبائی);
    اگر (m->ocm_name.string == تہی)
    {
	m->ocm_name.لمبائی = 0;
	واپس FAIL;
    }
    m->ocm_access = has_عوامی ? VIM_ACCESS_ALL
		      : *varname == '_' ? VIM_ACCESS_نجی : VIM_ACCESS_READ;
    اگر (has_final)
	m->ocm_flags |= OCMFLAG_FINAL;
    اگر (has_const)
	m->ocm_flags |= OCMFLAG_CONST;
    اگر (has_type)
	m->ocm_flags |= OCMFLAG_HAS_TYPE;
    m->ocm_type = type;
    اگر (init_اظہاریہ != تہی)
    {
	m->ocm_init = init_اظہاریہ;
	// Save the script context, we need it when evaluating or compiling the
	// initializer expression.
	m->ocm_init_sctx = current_sctx;
	m->ocm_init_sctx.sc_lnum += SOURCING_LNUM;
    }
    ++gap->ga_len;
    واپس OK;
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
    ساکن صحیح
جوڑو_members_مقصد_class(
    garray_T	*gap,
    ocmember_T	*والد_members,
    صحیح		والد_گنتی,
    ocmember_T	**members,
    صحیح		*member_گنتی)
{
    *member_گنتی = والد_گنتی + gap->ga_len;
    *members = *member_گنتی == 0 ? تہی
				       : ALLOC_MULT(ocmember_T, *member_گنتی);
    اگر (*member_گنتی > 0 && *members == تہی)
	واپس FAIL;
    برائے (صحیح i = 0; i < والد_گنتی; ++i)
    {
	// parent members need to be copied
	ocmember_T	*m = *members + i;
	*m = والد_members[i];
	m->ocm_name.string = vim_strnsave(m->ocm_name.string, m->ocm_name.لمبائی);
	اگر (m->ocm_name.string == تہی)
	{
	    m->ocm_name.لمبائی = 0;
	    واپس FAIL;
	}
	اگر (m->ocm_init != تہی)
	    m->ocm_init = vim_strsave(m->ocm_init);
    }
    اگر (gap->ga_len > 0)
	// new members are moved
	mch_memmove(*members + والد_گنتی,
			       gap->ga_ڈیٹا, حجم(ocmember_T) * gap->ga_len);
    VIM_CLEAR(gap->ga_ڈیٹا);
    واپس OK;
}

/*
 * Convert a member index "idx" of interface "itf" to the member index of class
 * "cl" implementing that interface.
 */
    صحیح
object_اشاریہ_from_itf_اشاریہ(class_T *itf, صحیح is_طریقہ, صحیح idx, class_T *cl)
{
    اگر (idx >= (is_طریقہ ? itf->class_obj_طریقہ_گنتی
				   : itf->class_obj_member_گنتی))
    {
	siemsg("index %d out of range for interface %s", idx, itf->class_name.string);
	واپس 0;
    }

    // If "cl" is the interface or the class that is extended, then the method
    // index can be used directly and there is no need to search for the method
    // index in one of the child classes.
    اگر (cl == itf)
	واپس idx;

    itf2class_T		*i2c = تہی;
    صحیح			searching = TRUE;
    صحیح			طریقہ_انحراف = 0;

    برائے (class_T *super = cl; super != تہی && searching;
						super = super->class_extends)
    {
	برائے (i2c = itf->class_itf2class; i2c != تہی; i2c = i2c->i2c_اگلا)
	{
	    اگر (i2c->i2c_class == super && i2c->i2c_is_طریقہ == is_طریقہ)
	    {
		searching = FALSE;
		توڑو;
	    }
	}
	اگر (searching && is_طریقہ)
	    // The parent class methods are stored after the current class
	    // methods.
	    طریقہ_انحراف += super->class_obj_طریقہ_گنتی_بچہ;
    }
    اگر (i2c == تہی)
    {
	siemsg("class %s not found on interface %s",
					      cl->class_name.string, itf->class_name.string);
	واپس 0;
    }

    // A table follows the i2c for the class
    صحیح *جدول = (صحیح *)(i2c + 1);
    // "method_offset" is 0, if method is in the current class.  If method
    // is in a parent class, then it is non-zero.
    واپس جدول[idx] + طریقہ_انحراف;
}

/*
 * Check whether a class named "extends_name" is present.  If the class is
 * valid, then "extends_clp" is set with the class pointer.
 * Returns TRUE if the class name "extends_names" is a valid class.
 */
    ساکن صحیح
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
	واپس success;
    }

    tv.v_type = VAR_UNKNOWN;
    اگر (eval_variable_import(extends_name, &tv) == FAIL)
    {
	semsg(_(e_class_name_not_ملا_str), extends_name);
	واپس success;
    }

    اگر (tv.v_type != VAR_CLASS || tv.vval.v_class == تہی
	    || (is_class && IS_INTERFACE(tv.vval.v_class))
	    || (!is_class && !IS_INTERFACE(tv.vval.v_class))
	    || (is_class && IS_ENUM(tv.vval.v_class)))
    {
	// a class cannot extend an interface
	// an interface cannot extend a class
	// a class cannot extend an enum.
	semsg(_(e_cannot_extend_str), extends_name);
    }
    ورنہ
    {
	class_T *extends_cl = tv.vval.v_class;
	++extends_cl->class_refcount;
	*extends_clp = extends_cl;
	success = TRUE;
    }
    clear_tv(&tv);

    واپس success;
}

/*
 * Check a generic method is extended by a generic method and the number of
 * type variables match.
 */
    ساکن صحیح
validate_extends_generic_طریقہ(
    class_T *super_cl,
    ufunc_T *super_fp,
    ufunc_T *cl_fp)
{
    حرف	*پیغام = تہی;

    اگر (cl_fp->uf_generic_argcount == super_fp->uf_generic_argcount)
	واپس TRUE;

    اگر (super_fp->uf_generic_argcount == 0)
	پیغام = e_concrete_طریقہ_str_override_with_generic_طریقہ_اندر_class_str;
    ورنہ اگر (cl_fp->uf_generic_argcount == 0)
	پیغام = e_generic_طریقہ_str_override_with_concrete_طریقہ_اندر_class_str;
    ورنہ
	پیغام = e_generic_طریقہ_str_type_arguments_mismatch_اندر_class_str;

    semsg(_(پیغام), cl_fp->uf_name, super_cl->class_name.string);
    واپس FALSE;
}

/*
 * Check method names in the parent class lineage to make sure the access is
 * the same for overridden methods.
 */
    ساکن صحیح
validate_extends_methods(
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    class_T	*super = extends_cl;
    صحیح		طریقہ_گنتی = objmethods_gap->ga_len;
    ufunc_T	**cl_fp = (ufunc_T **)(objmethods_gap->ga_ڈیٹا);

    جب (super != تہی)
    {
	صحیح extends_طریقہ_گنتی = super->class_obj_طریقہ_گنتی_بچہ;
	اگر (extends_طریقہ_گنتی == 0)
	{
	    super = super->class_extends;
	    جاری;
	}

	ufunc_T **extends_methods = super->class_obj_methods;

	برائے (صحیح i = 0; i < extends_طریقہ_گنتی; i++)
	{
	    char_u  *pstr = extends_methods[i]->uf_name;
	    صحیح	    extends_نجی = (*pstr == '_');
	    اگر (extends_نجی)
		pstr++;

	    // When comparing the method names, ignore the access type (public
	    // and private methods are considered the same).
	    برائے (صحیح j = 0; j < طریقہ_گنتی; j++)
	    {
		char_u  *qstr = cl_fp[j]->uf_name;
		صحیح	priv_طریقہ = (*qstr == '_');
		اگر (priv_طریقہ)
		    qstr++;
		اگر (STRCMP(pstr, qstr) == 0)
		{
		    اگر (priv_طریقہ != extends_نجی)
		    {
			// Method access is different between the super class
			// and the subclass
			semsg(_(e_طریقہ_str_کا_class_str_has_different_access),
				cl_fp[j]->uf_name, super->class_name.string);
			واپس FALSE;
		    }

		    اگر (!validate_extends_generic_طریقہ(super,
						extends_methods[i], cl_fp[j]))
			    واپس FALSE;
		}
	    }
	}
	super = super->class_extends;
    }

    واپس TRUE;
}

/*
 * Check whether a object member variable in "objmembers_gap" is a duplicate of
 * a member in any of the extended parent class lineage.  Returns TRUE if there
 * are no duplicates.
 */
    ساکن صحیح
extends_check_dup_members(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحیح member_گنتی = objmembers_gap->ga_len;
    اگر (member_گنتی == 0)
	واپس TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_ڈیٹا);

    // Validate each member variable
    برائے (صحیح c_i = 0; c_i < member_گنتی; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	bool	    c_protected = (*c_m->ocm_name.string == '_');
	char_u	    *pstr = c_m->ocm_name.string + c_protected;

	// Check in all the parent classes in the lineage
	جب (p_cl != تہی)
	{
	    صحیح p_member_گنتی = p_cl->class_obj_member_گنتی;

	    اگر (p_member_گنتی == 0)
	    {
		p_cl = p_cl->class_extends;
		جاری;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    برائے (صحیح p_i = 0; p_i < p_member_گنتی; p_i++)
	    {
		ocmember_T  *p_m = p_members + p_i;
		bool	    p_protected = (*p_m->ocm_name.string == '_');
		char_u	    *qstr = p_m->ocm_name.string + p_protected;
		اگر (STRCMP(pstr, qstr) == 0)
		{
		    اگر (c_protected != p_protected)
			semsg(_(e_عوامی_and_protected_member_have_same_name_str_str),
				pstr, pstr);
		    ورنہ
			semsg(_(e_duplicate_variable_str),
				c_m->ocm_name.string);
		    واپس FALSE;
		}
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    واپس TRUE;
}

/*
 * Compare the variable type of interface variables in "objmembers_gap" against
 * the variable in any of the extended super interface lineage.  Used to
 * compare the variable types when extending interfaces.  Returns TRUE if the
 * variable types are the same.
 */
    ساکن صحیح
extends_check_intf_var_type(
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحیح member_گنتی = objmembers_gap->ga_len;
    اگر (member_گنتی == 0)
	واپس TRUE;

    ocmember_T *members = (ocmember_T *)(objmembers_gap->ga_ڈیٹا);

    // Validate each member variable
    برائے (صحیح c_i = 0; c_i < member_گنتی; c_i++)
    {
	class_T	    *p_cl = extends_cl;
	ocmember_T  *c_m = members + c_i;
	صحیح	    var_ملا = FALSE;

	// Check in all the parent classes in the lineage
	جب (p_cl != تہی && !var_ملا)
	{
	    صحیح p_member_گنتی = p_cl->class_obj_member_گنتی;
	    اگر (p_member_گنتی == 0)
	    {
		p_cl = p_cl->class_extends;
		جاری;
	    }
	    ocmember_T *p_members = p_cl->class_obj_members;

	    // Compare against all the members in the parent class
	    برائے (صحیح p_i = 0; p_i < p_member_گنتی; p_i++)
	    {
		where_T		where = WHERE_INIT;
		ocmember_T	*p_m = p_members + p_i;

		اگر (STRCMP(p_m->ocm_name.string, c_m->ocm_name.string) != 0)
		    جاری;

		// Ensure the type is matching.
		where.wt_func_name = (حرف *)c_m->ocm_name.string;
		where.wt_قسم = WT_MEMBER;

		اگر (check_type(p_m->ocm_type, c_m->ocm_type, TRUE,
								where) == FAIL)
		    واپس FALSE;

		var_ملا = TRUE;
	    }

	    p_cl = p_cl->class_extends;
	}
    }

    واپس TRUE;
}

/*
 * When extending an abstract class, check whether all the abstract methods in
 * the parent class are implemented.  Returns TRUE if all the methods are
 * implemented.
 */
    ساکن صحیح
validate_abstract_class_methods(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    برائے (صحیح loop = 1; loop <= 2; ++loop)
    {
	// loop == 1: check class methods
	// loop == 2: check object methods
	صحیح extends_طریقہ_گنتی = loop == 1
				? extends_cl->class_class_function_گنتی
				: extends_cl->class_obj_طریقہ_گنتی;
	اگر (extends_طریقہ_گنتی == 0)
	    جاری;

	ufunc_T **extends_methods = loop == 1
				? extends_cl->class_class_functions
				: extends_cl->class_obj_methods;

	صحیح طریقہ_گنتی = loop == 1 ? classmethods_gap->ga_len
						: objmethods_gap->ga_len;
	ufunc_T **cl_fp = (ufunc_T **)(loop == 1
						? classmethods_gap->ga_ڈیٹا
						: objmethods_gap->ga_ڈیٹا);

	برائے (صحیح i = 0; i < extends_طریقہ_گنتی; i++)
	{
	    ufunc_T *uf = extends_methods[i];
	    اگر (!IS_ABSTRACT_طریقہ(uf))
		جاری;

	    صحیح	concrete_طریقہ_ملا = FALSE;
	    صحیح	j = 0;

	    // Check if the abstract method is already implemented in one of
	    // the parent classes.
	    برائے (j = 0; !concrete_طریقہ_ملا && j < i; j++)
	    {
		ufunc_T *uf2 = extends_methods[j];
		اگر (!IS_ABSTRACT_طریقہ(uf2) &&
			STRCMP(uf->uf_name, uf2->uf_name) == 0)
		    concrete_طریقہ_ملا = TRUE;
	    }

	    اگر (concrete_طریقہ_ملا)
		جاری;

	    برائے (j = 0; j < طریقہ_گنتی; j++)
	    {
		اگر (STRCMP(uf->uf_name, cl_fp[j]->uf_name) == 0)
		{
		    concrete_طریقہ_ملا = TRUE;
		    توڑو;
		}
	    }

	    اگر (!concrete_طریقہ_ملا)
	    {
		semsg(_(e_abstract_طریقہ_str_not_implemented), uf->uf_name);
		واپس FALSE;
	    }
	}
    }

    واپس TRUE;
}

/*
 * Returns TRUE if the interface variable "if_var" is present in the list of
 * variables in "cl_mt" or in the parent lineage of one of the extended classes
 * in "extends_cl".  For a class variable, 'is_class_var' is TRUE.
 */
    ساکن صحیح
intf_variable_present(
    char_u	*intf_class_name,
    ocmember_T *if_var,
    ocmember_T *cl_mt,
    صحیح		cl_member_گنتی,
    class_T	*extends_cl)
{
    صحیح		variable_present  = FALSE;

    برائے (صحیح cl_i = 0; cl_i < cl_member_گنتی; ++cl_i)
    {
	ocmember_T	*m = &cl_mt[cl_i];
	where_T		where = WHERE_INIT;

	اگر (STRCMP(if_var->ocm_name.string, m->ocm_name.string) != 0)
	    جاری;

	// Ensure the access type is same
	اگر (if_var->ocm_access != m->ocm_access)
	{
	    semsg(_(e_variable_str_کا_interface_str_has_different_access),
		    if_var->ocm_name.string, intf_class_name);
	    واپس FALSE;
	}

	// Ensure the type is matching.
	اگر (m->ocm_type == &t_any)
	{
	    // variable type is not specified.  Use the variable type in the
	    // interface.
	    m->ocm_type = if_var->ocm_type;
	}
	ورنہ
	{
	    where.wt_func_name = (حرف *)m->ocm_name.string;
	    where.wt_قسم = WT_MEMBER;
	    اگر (check_type(if_var->ocm_type, m->ocm_type, TRUE,
							    where) == FAIL)
		واپس FALSE;
	}

	variable_present = TRUE;
	توڑو;
    }

    اگر (!variable_present && extends_cl != تہی)
    {
	صحیح ext_cl_گنتی = extends_cl->class_obj_member_گنتی;
	ocmember_T *ext_cl_mt = extends_cl->class_obj_members;
	واپس intf_variable_present(intf_class_name, if_var,
					ext_cl_mt, ext_cl_گنتی,
					extends_cl->class_extends);
    }

    واپس variable_present;
}

/*
 * Check the variables of the interface class "ifcl" match object variables
 * ("objmembers_gap") of a class.
 * Returns TRUE if the object variables names are valid.
 */
    ساکن صحیح
validate_interface_variables(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحیح if_گنتی = ifcl->class_obj_member_گنتی;
    اگر (if_گنتی == 0)
	واپس TRUE;

    ocmember_T *if_ms = ifcl->class_obj_members;
    ocmember_T *cl_ms = (ocmember_T *)(objmembers_gap->ga_ڈیٹا);
    صحیح cl_گنتی = objmembers_gap->ga_len;
    برائے (صحیح if_i = 0; if_i < if_گنتی; ++if_i)
    {
	اگر (!intf_variable_present(intf_class_name, &if_ms[if_i], cl_ms,
							cl_گنتی, extends_cl))
	{
	    semsg(_(e_variable_str_کا_interface_str_not_implemented),
		    if_ms[if_i].ocm_name.string, intf_class_name);
	    واپس FALSE;
	}
    }

    واپس TRUE;
}

/*
 * Returns TRUE if the method signature of "if_method" of interface
 * "intf_name" and "cl_method" matches.
 */
    ساکن صحیح
intf_طریقہ_type_matches(
    ufunc_T	*if_طریقہ,
    ufunc_T	*cl_طریقہ,
    char_u	*intf_name)
{
    where_T where = WHERE_INIT;

    // A generic method must have the same number of type variables, like
    // when overriding a method, see validate_extends_generic_method().
    اگر (cl_طریقہ->uf_generic_argcount != if_طریقہ->uf_generic_argcount)
    {
	semsg(_(e_type_var_گنتی_کا_طریقہ_str_differs_from_interface_str),
						cl_طریقہ->uf_name, intf_name);
	واپس FALSE;
    }

    // Ensure the type is matching.
    where.wt_func_name = (حرف *)if_طریقہ->uf_name;
    where.wt_قسم = WT_طریقہ;
    اگر (check_type(if_طریقہ->uf_func_type, cl_طریقہ->uf_func_type, TRUE,
								where) == FAIL)
	واپس FALSE;

    واپس TRUE;
}

/*
 * Returns TRUE if the method "if_ufunc" of interface "intf_name" is present in
 * the list of methods in "cl_fp" or in the parent lineage of one of the
 * extended classes in "extends_cl".
 */
    ساکن صحیح
intf_طریقہ_present(
    ufunc_T *if_ufunc,
    ufunc_T **cl_fp,
    صحیح	    cl_گنتی,
    class_T *extends_cl,
    char_u  *intf_name)
{
    صحیح		طریقہ_present  = FALSE;

    برائے (صحیح cl_i = 0; cl_i < cl_گنتی; ++cl_i)
    {
	اگر (STRCMP(if_ufunc->uf_name, cl_fp[cl_i]->uf_name) == 0)
	{
	    // Ensure the type is matching.
	    اگر (!intf_طریقہ_type_matches(if_ufunc, cl_fp[cl_i], intf_name))
		واپس FALSE;
	    طریقہ_present = TRUE;
	    توڑو;
	}
    }

    اگر (!طریقہ_present && extends_cl != تہی)
    {
	ufunc_T **ext_cl_fp = (ufunc_T **)(extends_cl->class_obj_methods);
	صحیح	ext_cl_گنتی = extends_cl->class_obj_طریقہ_گنتی;
	واپس intf_طریقہ_present(if_ufunc, ext_cl_fp, ext_cl_گنتی,
					extends_cl->class_extends, intf_name);
    }

    واپس طریقہ_present;
}

/*
 * Validate that a new class implements all the class/instance methods in the
 * interface "ifcl".  The new class methods are in "classfunctions_gap" and the
 * new object methods are in "objmemthods_gap".  Also validates the method
 * types.
 * Returns TRUE if all the interface class/object methods are implemented in
 * the new class.
 */
    ساکن صحیح
validate_interface_methods(
    char_u	*intf_class_name,
    class_T	*ifcl,
    garray_T	*objmethods_gap,
    class_T	*extends_cl)
{
    صحیح if_گنتی = ifcl->class_obj_طریقہ_گنتی;
    اگر (if_گنتی == 0)
	واپس TRUE;

    ufunc_T **if_fp = ifcl->class_obj_methods;
    ufunc_T **cl_fp = (ufunc_T **)(objmethods_gap->ga_ڈیٹا);
    صحیح cl_گنتی = objmethods_gap->ga_len;
    برائے (صحیح if_i = 0; if_i < if_گنتی; ++if_i)
    {
	char_u	*if_name = if_fp[if_i]->uf_name;

	اگر (!intf_طریقہ_present(if_fp[if_i], cl_fp, cl_گنتی, extends_cl,
							    intf_class_name))
	{
	    semsg(_(e_طریقہ_str_کا_interface_str_not_implemented),
		    if_name, intf_class_name);
	    واپس FALSE;
	}
    }

    واپس TRUE;
}

/*
 * Validate all the "implements" classes when creating the new class "cl".
 * The classes are returned in "intf_classes".  The class functions, class
 * members, object methods and object members in the new class are in
 * "classfunctions_gap", "classmembers_gap", "objmethods_gap", and
 * "objmembers_gap" respectively.
 */
    ساکن صحیح
validate_implements_classes(
    class_T	*cl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap,
    garray_T	*objmethods_gap,
    garray_T	*objmembers_gap,
    class_T	*extends_cl)
{
    صحیح		success = TRUE;

    برائے (صحیح i = 0; i < impl_gap->ga_len && success; ++i)
    {
	char_u *impl = ((char_u **)impl_gap->ga_ڈیٹا)[i];
	typval_T tv;
	tv.v_type = VAR_UNKNOWN;
	اگر (eval_variable_import(impl, &tv) == FAIL)
	{
	    semsg(_(e_interface_name_not_ملا_str), impl);
	    success = FALSE;
	    توڑو;
	}

	اگر (tv.v_type != VAR_CLASS
		|| tv.vval.v_class == تہی
		|| !IS_INTERFACE(tv.vval.v_class))
	{
	    semsg(_(e_not_valid_interface_str), impl);
	    success = FALSE;
	    clear_tv(&tv);
	    توڑو;
	}

	class_T *ifcl = tv.vval.v_class;
	اگر (ga_grow(intf_classes_gap, 1) == FAIL)
	{
	    success = FALSE;
	    clear_tv(&tv);
	    توڑو;
	}
	((class_T **)intf_classes_gap->ga_ڈیٹا)[intf_classes_gap->ga_len]
								= ifcl;
	intf_classes_gap->ga_len++;
	++ifcl->class_refcount;

	// check the variables of the interface match the members of the class
	success = validate_interface_variables(impl, ifcl, objmembers_gap,
								extends_cl);
	clear_tv(&tv);
    }

    اگر (!success)
	واپس FALSE;

    // check if the functions/methods of the interface match the
    // functions/methods of the class.  A method may return the new class
    // where an interface or the parent class is expected, temporarily set
    // them in "cl" to make the type check work.
    cl->class_interfaces_cl = (class_T **)intf_classes_gap->ga_ڈیٹا;
    cl->class_interface_گنتی = intf_classes_gap->ga_len;
    cl->class_extends = extends_cl;
    برائے (صحیح i = 0; i < impl_gap->ga_len && success; ++i)
	success = validate_interface_methods(
		((char_u **)impl_gap->ga_ڈیٹا)[i],
		((class_T **)intf_classes_gap->ga_ڈیٹا)[i], objmethods_gap,
		extends_cl);
    cl->class_interfaces_cl = تہی;
    cl->class_interface_گنتی = 0;
    cl->class_extends = تہی;

    واپس success;
}

/*
 * Returns TRUE if the interface class "ifcl" is already present in the
 * "intf_classes_gap" grow array.
 */
    ساکن صحیح
is_interface_class_present(garray_T *intf_classes_gap, class_T *ifcl)
{
    برائے (صحیح j = 0; j < intf_classes_gap->ga_len; j++)
    {
	اگر (((class_T **)intf_classes_gap->ga_ڈیٹا)[j] == ifcl)
	    واپس TRUE;
    }

    واپس FALSE;
}

/*
 * Add interface "ifcl" from a super class to "intf_classes_gap" and the class
 * name to "impl_gap".
 */
    ساکن صحیح
جوڑو_interface_from_super_class(
    class_T	*ifcl,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    char_u	*intf_name;

    // Add the interface name to "impl_gap"
    intf_name = vim_strnsave(ifcl->class_name.string, ifcl->class_name.لمبائی);
    اگر (intf_name == تہی)
	واپس FALSE;

    اگر (ga_grow(impl_gap, 1) == FAIL)
	جاؤ fail;

    char_u **intf_نام = (char_u **)impl_gap->ga_ڈیٹا;
    intf_نام[impl_gap->ga_len] = intf_name;
    impl_gap->ga_len++;

    // Add the interface class to "intf_classes_gap"
    اگر (ga_grow(intf_classes_gap, 1) == FAIL)
    {
	--impl_gap->ga_len;
	جاؤ fail;
    }

    class_T **intf_classes = (class_T **)intf_classes_gap->ga_ڈیٹا;
    intf_classes[intf_classes_gap->ga_len] = ifcl;
    intf_classes_gap->ga_len++;
    ++ifcl->class_refcount;

    واپس TRUE;
fail:
    vim_free(intf_name);
    واپس FALSE;
}

/*
 * Add "super" class interfaces to "intf_classes_gap" (if not present already)
 * Add the interface class names to "impl_gap".
 */
    ساکن صحیح
جوڑو_super_class_interfaces(
    class_T	*super,
    garray_T	*impl_gap,
    garray_T	*intf_classes_gap)
{
    // Iterate through all the interfaces implemented by "super"
    برائے (صحیح i = 0; i < super->class_interface_گنتی; i++)
    {
	class_T	*ifcl = super->class_interfaces_cl[i];

	اگر (!is_interface_class_present(intf_classes_gap, ifcl))
	    جوڑو_interface_from_super_class(ifcl, impl_gap, intf_classes_gap);
    }

    واپس TRUE;
}

/*
 * Check no function argument name is used as a class member.
 * (Object members are always accessed with "this." prefix, so no need
 * to check them.)
 */
    ساکن صحیح
check_func_arg_نام(
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap,
    garray_T	*classmembers_gap)
{
    // loop 1: class functions, loop 2: object methods
    برائے (صحیح loop = 1; loop <= 2; ++loop)
    {
	garray_T *gap = loop == 1 ? classfunctions_gap : objmethods_gap;

	برائے (صحیح fi = 0; fi < gap->ga_len; ++fi)
	{
	    ufunc_T *uf = ((ufunc_T **)gap->ga_ڈیٹا)[fi];

	    برائے (صحیح i = 0; i < uf->uf_دلائل.ga_len; ++i)
	    {
		char_u *aname = ((char_u **)uf->uf_دلائل.ga_ڈیٹا)[i];
		garray_T *mgap = classmembers_gap;

		// Check all the class member names
		برائے (صحیح mi = 0; mi < mgap->ga_len; ++mi)
		{
		    char_u *mname =
				((ocmember_T *)mgap->ga_ڈیٹا + mi)->ocm_name.string;
		    اگر (STRCMP(aname, mname) == 0)
		    {
			اگر (uf->uf_script_ctx.sc_sid > 0)
			    SOURCING_LNUM = uf->uf_script_ctx.sc_lnum;

			semsg(_(e_دلیل_already_declared_اندر_class_str),
				aname);

			واپس FALSE;
		    }
		}
	    }
	}
    }

    واپس TRUE;
}

/*
 * Returns TRUE if 'varname' is a reserved keyword name
 */
    ساکن صحیح
is_reserved_varname(char_u *varname, char_u *varname_انتہا)
{
    صحیح reserved = FALSE;
    char_u save_varname_انتہا = *varname_انتہا;
    *varname_انتہا = NUL;

    reserved = check_reserved_name(varname, FALSE) == FAIL;

    *varname_انتہا = save_varname_انتہا;

    واپس reserved;
}

/*
 * Returns TRUE if the variable "varname" is already defined either as a class
 * variable or as an object variable.
 */
    ساکن صحیح
is_duplicate_variable(
    garray_T	*class_members,
    garray_T	*obj_members,
    char_u	*varname,
    char_u	*varname_انتہا)
{
    // Note: the .string field may point to a string longer than the .length
    // field.  So we need to use STRNCMP() to compare it.
    string_T	pstr = {varname, (size_t)(varname_انتہا - varname)};
    صحیح		dup = FALSE;
    bool	p_protected = false;

    // Step over a leading '_'.
    اگر (*pstr.string == '_')
    {
	pstr.string++;
	pstr.لمبائی--;
	p_protected = true;
    }

    // loop == 1: class variables, loop == 2: object variables
    برائے (صحیح loop = 1; loop <= 2; loop++)
    {
	garray_T    *vgap = (loop == 1) ? class_members : obj_members;
	برائے (صحیح i = 0; i < vgap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)vgap->ga_ڈیٹا) + i;
	    string_T	qstr = {m->ocm_name.string, m->ocm_name.لمبائی};
	    bool	q_protected = false;

	    // Step over a leading '_'.
	    اگر (*qstr.string == '_')
	    {
		qstr.string++;
		qstr.لمبائی--;
		q_protected = true;
	    }

	    اگر (pstr.لمبائی == qstr.لمبائی
		&& STRNCMP(pstr.string, qstr.string, pstr.لمبائی) == 0)
	    {
		char_u	save_c = *varname_انتہا;

		*varname_انتہا = NUL;
		اگر (p_protected != q_protected)
		    semsg(_(e_عوامی_and_protected_member_have_same_name_str_str),
					       pstr.string, pstr.string);
		ورنہ
		    semsg(_(e_duplicate_variable_str), varname);
		*varname_انتہا = save_c;
		dup = TRUE;
		توڑو;
	    }
	}
    }

    واپس dup;
}

/*
 * Returns TRUE if the method "name" is already defined.
 */
    ساکن صحیح
is_duplicate_طریقہ(
    garray_T	*classmethods_gap,
    garray_T	*objmethods_gap,
    char_u	*name)
{
    char_u *pstr = (*name == '_') ? name + 1 : name;

    // loop 1: class methods, loop 2: object methods
    برائے (صحیح loop = 1; loop <= 2; loop++)
    {
	garray_T *fgap = (loop == 1) ? classmethods_gap : objmethods_gap;
	برائے (صحیح i = 0; i < fgap->ga_len; ++i)
	{
	    char_u	*n = ((ufunc_T **)fgap->ga_ڈیٹا)[i]->uf_name;
	    char_u	*qstr = *n == '_' ? n + 1 : n;
	    اگر (STRCMP(pstr, qstr) == 0)
	    {
		semsg(_(e_duplicate_function_str), name);
		واپس TRUE;
	    }
	}
    }

    واپس FALSE;
}

/*
 * Returns TRUE if the constructor is valid.
 */
    ساکن صحیح
is_valid_constructor(ufunc_T *uf, صحیح is_abstract, صحیح has_static)
{
    // Constructors are not allowed in abstract classes.
    اگر (is_abstract)
    {
	emsg(_(e_cannot_define_new_طریقہ_اندر_abstract_class));
	واپس FALSE;
    }
    // A constructor is always static, no need to define it so.
    اگر (has_static)
    {
	emsg(_(e_cannot_define_new_طریقہ_as_static));
	واپس FALSE;
    }
    // A return type should not be specified for the new()
    // constructor method.
    اگر (uf->uf_ret_type->tt_type != VAR_VOID)
    {
	emsg(_(e_cannot_use_a_return_type_with_new_طریقہ));
	واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Returns TRUE if 'uf' is a supported builtin method and has the correct
 * method signature.
 */
    ساکن صحیح
object_check_builtin_طریقہ_sig(ufunc_T *uf)
{
    char_u  *name = uf->uf_name;
    صحیح	    valid = FALSE;
    type_T  طریقہ_sig;
    type_T  طریقہ_rt;
    where_T where = WHERE_INIT;

    // validate the method signature
    CLEAR_میدان(طریقہ_sig);
    CLEAR_میدان(طریقہ_rt);
    طریقہ_sig.tt_type = VAR_FUNC;

    اگر (STRCMP(name, "len") == 0)
    {
	// def __len(): number
	طریقہ_rt.tt_type = VAR_نمبر;
	طریقہ_sig.tt_member = &طریقہ_rt;
	valid = TRUE;
    }
    ورنہ اگر (STRCMP(name, "empty") == 0)
    {
	// def __empty(): bool
	طریقہ_rt.tt_type = VAR_BOOL;
	طریقہ_sig.tt_member = &طریقہ_rt;
	valid = TRUE;
    }
    ورنہ اگر (STRCMP(name, "string") == 0)
    {
	// def __string(): string
	طریقہ_rt.tt_type = VAR_STRING;
	طریقہ_sig.tt_member = &طریقہ_rt;
	valid = TRUE;
    }
    ورنہ
	semsg(_(e_builtin_object_طریقہ_str_not_supported), uf->uf_name);

    where.wt_func_name = (حرف *)uf->uf_name;
    where.wt_قسم = WT_طریقہ;
    اگر (valid && !check_type(&طریقہ_sig, uf->uf_func_type, TRUE, where))
	valid = FALSE;

    واپس valid;
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
	    واپس STRNCMP(funcname, "empty", 5) == 0;

	حالت 'l':
	    واپس STRNCMP(funcname, "len", 3) == 0;

	حالت 'n':
	    واپس STRNCMP(funcname, "new", 3) == 0;

	حالت 's':
	    واپس STRNCMP(funcname, "string", 6) == 0;
    }

    واپس FALSE;
}


/*
 * Returns the builtin method "name" in object "obj".  Returns NULL if the
 * method is not found.
 */
    ufunc_T *
class_get_builtin_طریقہ(
    class_T		*cl,
    class_builtin_T	builtin_طریقہ,
    صحیح			*طریقہ_idx)
{
    *طریقہ_idx = -1;

    اگر (cl == تہی)
	واپس تہی;

    *طریقہ_idx = cl->class_builtin_methods[builtin_طریقہ];
    واپس *طریقہ_idx != -1 ? cl->class_obj_methods[*طریقہ_idx] : تہی;
}

/*
 * Update the interface class lookup table for the member index on the
 * interface to the member index in the class implementing the interface.
 * And a lookup table for the object method index on the interface
 * to the object method index in the class implementing the interface.
 * This is also used for updating the lookup table for the extended class
 * hierarchy.
 */
    ساکن صحیح
update_member_طریقہ_lookup_جدول(
    class_T	*ifcl,
    class_T	*cl,
    garray_T	*objmethods,
    صحیح		pobj_طریقہ_انحراف)
{
    اگر (ifcl == تہی)
	واپس OK;

    // Table for members.
    itf2class_T *if2cl = alloc_clear(حجم(itf2class_T)
				+ ifcl->class_obj_member_گنتی * حجم(صحیح));
    اگر (if2cl == تہی)
	واپس FAIL;
    if2cl->i2c_اگلا = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_طریقہ = FALSE;

    برائے (صحیح if_i = 0; if_i < ifcl->class_obj_member_گنتی; ++if_i)
    {
	char_u  *ocm_name = ifcl->class_obj_members[if_i].ocm_name.string;

	برائے (صحیح cl_i = 0; cl_i < cl->class_obj_member_گنتی; ++cl_i)
	{
	    اگر (STRCMP(ocm_name, cl->class_obj_members[cl_i].ocm_name.string) == 0)
	    {
		صحیح *جدول = (صحیح *)(if2cl + 1);
		جدول[if_i] = cl_i;
		توڑو;
	    }
	}
    }

    // Table for methods.
    if2cl = alloc_clear(حجم(itf2class_T)
				+ ifcl->class_obj_طریقہ_گنتی * حجم(صحیح));
    اگر (if2cl == تہی)
	واپس FAIL;
    if2cl->i2c_اگلا = ifcl->class_itf2class;
    ifcl->class_itf2class = if2cl;
    if2cl->i2c_class = cl;
    if2cl->i2c_is_طریقہ = TRUE;

    برائے (صحیح if_i = 0; if_i < ifcl->class_obj_طریقہ_گنتی; ++if_i)
    {
	صحیح	مکمل = FALSE;
	char_u	*if_name = ifcl->class_obj_methods[if_i]->uf_name;

	برائے (صحیح cl_i = 0; cl_i < objmethods->ga_len; ++cl_i)
	{
	    اگر (STRCMP(if_name,
			((ufunc_T **)objmethods->ga_ڈیٹا)[cl_i]->uf_name) == 0)
	    {
		صحیح *جدول = (صحیح *)(if2cl + 1);
		جدول[if_i] = cl_i;
		مکمل = TRUE;
		توڑو;
	    }
	}

	// extended class object method is not overridden by the child class.
	// Keep the method declared in one of the parent classes in the
	// lineage.
	اگر (!مکمل)
	{
	    // If "ifcl" is not the immediate parent of "cl", then search in
	    // the intermediate parent classes.
	    اگر (cl->class_extends != ifcl)
	    {
		class_T		*والد = cl->class_extends;
		صحیح		طریقہ_انحراف = objmethods->ga_len;

		جب (!مکمل && والد != تہی && والد != ifcl)
		{
		    برائے (صحیح cl_i = 0;
			    cl_i < والد->class_obj_طریقہ_گنتی_بچہ; ++cl_i)
		    {
			اگر (STRCMP(if_name,
				    والد->class_obj_methods[cl_i]->uf_name)
				== 0)
			{
			    صحیح *جدول = (صحیح *)(if2cl + 1);
			    جدول[if_i] = طریقہ_انحراف + cl_i;
			    مکمل = TRUE;
			    توڑو;
			}
		    }
		    طریقہ_انحراف += والد->class_obj_طریقہ_گنتی_بچہ;
		    والد = والد->class_extends;
		}
	    }

	    اگر (!مکمل)
	    {
		صحیح *جدول = (صحیح *)(if2cl + 1);
		جدول[if_i] = pobj_طریقہ_انحراف + if_i;
	    }
	}
    }

    واپس OK;
}

/*
 * Update the member and object method lookup tables for a new class in the
 * interface class.
 * For each interface add a lookup table for the member index on the interface
 * to the member index in the new class.  And a lookup table for the object
 * method index on the interface to the object method index in the new class.
 */
    ساکن صحیح
جوڑو_lookup_tables(class_T *cl, class_T *extends_cl, garray_T *objmethods_gap)
{
    // update the lookup table for all the implemented interfaces
    برائے (صحیح i = 0; i < cl->class_interface_گنتی; ++i)
    {
	class_T *ifcl = cl->class_interfaces_cl[i];

	// update the lookup table for this interface and all its super
	// interfaces.
	جب (ifcl != تہی)
	{
	    اگر (update_member_طریقہ_lookup_جدول(ifcl, cl, objmethods_gap,
								0) == FAIL)
		واپس FAIL;
	    ifcl = ifcl->class_extends;
	}
    }

    // Update the lookup table for the extended class, if any
    اگر (extends_cl != تہی)
    {
	class_T		*pclass = extends_cl;
	صحیح		pobj_طریقہ_انحراف = objmethods_gap->ga_len;

	// Update the entire lineage of extended classes.
	جب (pclass != تہی)
	{
	    اگر (update_member_طریقہ_lookup_جدول(pclass, cl,
			objmethods_gap, pobj_طریقہ_انحراف) == FAIL)
		واپس FAIL;

	    pobj_طریقہ_انحراف += pclass->class_obj_طریقہ_گنتی_بچہ;
	    pclass = pclass->class_extends;
	}
    }

    واپس OK;
}

/*
 * Add class members to a new class.  Allocate a typval for each class member
 * and initialize it.
 */
    ساکن صحیح
جوڑو_class_members(class_T *cl, exarg_T *eap, garray_T *type_list_gap)
{
    // Allocate a typval for each class member and initialize it.
    cl->class_members_tv = ALLOC_CLEAR_MULT(typval_T,
					    cl->class_class_member_گنتی);
    اگر (cl->class_members_tv == تہی)
	واپس FAIL;

    برائے (صحیح i = 0; i < cl->class_class_member_گنتی; ++i)
    {
	ocmember_T	*m = &cl->class_class_members[i];
	typval_T	*tv = &cl->class_members_tv[i];
	اگر (m->ocm_init != تہی)
	{
	    sctx_T	save_current_sctx = current_sctx;

	    current_sctx = m->ocm_init_sctx;
	    typval_T *etv = eval_اظہاریہ(m->ocm_init, eap);
	    current_sctx = save_current_sctx;
	    اگر (etv == تہی)
		واپس FAIL;

	    اگر (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && etv->v_type != VAR_SPECIAL)
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = typval2type(etv, get_copyID(),
			type_list_gap,
			TVTT_DO_MEMBER|TVTT_مزید_SPECIFIC);
	    *tv = *etv;
	    vim_free(etv);
	}
	ورنہ
	{
	    // TODO: proper default value
	    tv->v_type = m->ocm_type->tt_type;
	    tv->vval.v_string = تہی;
	}
	سیٹ_tv_type(tv, m->ocm_type);
	اگر (m->ocm_flags & OCMFLAG_CONST)
	    شے_تالا(tv, DICT_MAXNEST, TRUE, TRUE);
    }

    واپس OK;
}

/*
 * Add a default constructor method (new()) to the class "cl".
 */
    ساکن خالی
جوڑو_default_constructor(
    class_T	*cl,
    garray_T	*classfunctions_gap,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    صحیح		is_enum = IS_ENUM(cl);

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "new(");
    برائے (صحیح i = 0; i < cl->class_obj_member_گنتی; ++i)
    {
	اگر (i < 2 && is_enum)
	    // The first two object variables in an enum are the enum value
	    // name and ordinal.  Don't initialize these object variables in
	    // the default constructor as they are already initialized right
	    // after creating the object.
	    جاری;

	اگر (i > (is_enum ? 2 : 0))
	    GA_CONCAT_LITERAL(&fga, ", ");
	GA_CONCAT_LITERAL(&fga, "this.");
	ocmember_T *m = cl->class_obj_members + i;
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.لمبائی);
	GA_CONCAT_LITERAL(&fga, " = v:none");
    }
    GA_CONCAT_LITERAL(&fga, ")\nenddef\n");
    ga_append(&fga, NUL);

    exarg_T fea;
    CLEAR_میدان(fea);
    fea.cmdidx = CMD_def;
    fea.cmd = fea.arg = fga.ga_ڈیٹا;

    garray_T سطریں_مقصد_free;
    ga_init2(&سطریں_مقصد_free, حجم(char_u *), 50);

    ufunc_T *nf = define_function(&fea, تہی, &سطریں_مقصد_free, CF_CLASS,
			    cl->class_obj_members, cl->class_obj_member_گنتی,
			    تہی);

    ga_clear_لڑیاں(&سطریں_مقصد_free);
    vim_free(fga.ga_ڈیٹا);

    اگر (nf != تہی && ga_grow(classfunctions_gap, 1) == OK)
    {
	((ufunc_T **)classfunctions_gap->ga_ڈیٹا)[classfunctions_gap->ga_len]
									= nf;
	++classfunctions_gap->ga_len;

	nf->uf_flags |= FC_NEW;
	nf->uf_ret_type = get_type_ptr(type_list_gap);
	اگر (nf->uf_ret_type != تہی)
	{
	    nf->uf_ret_type->tt_type = VAR_OBJECT;
	    nf->uf_ret_type->tt_class = cl;
	    nf->uf_ret_type->tt_argcount = 0;
	    nf->uf_ret_type->tt_دلائل = تہی;
	}
    }
}

/*
 * Add the class methods and object methods to the new class "cl".
 * When extending a class "extends_cl", add the instance methods from the
 * parent class also.
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ساکن صحیح
جوڑو_classfuncs_objmethods(
    class_T	*cl,
    class_T	*extends_cl,
    garray_T	*classfunctions_gap,
    garray_T	*objmethods_gap)
{
    // loop 1: class functions, loop 2: object methods
    برائے (صحیح loop = 1; loop <= 2; ++loop)
    {
	garray_T	*gap = loop == 1 ? classfunctions_gap : objmethods_gap;
	صحیح		*fcount = loop == 1 ? &cl->class_class_function_گنتی
						: &cl->class_obj_طریقہ_گنتی;
	ufunc_T		***fup = loop == 1 ? &cl->class_class_functions
						: &cl->class_obj_methods;

	صحیح والد_گنتی = 0;
	اگر (extends_cl != تہی)
	    // Include object methods from the parent.
	    // Don't include the parent class methods.
	    والد_گنتی = loop == 1
				? 0
				: extends_cl->class_obj_طریقہ_گنتی;

	*fcount = والد_گنتی + gap->ga_len;
	اگر (*fcount == 0)
	{
	    *fup = تہی;
	    جاری;
	}
	*fup = ALLOC_MULT(ufunc_T *, *fcount);
	اگر (*fup == تہی)
	    واپس FAIL;

	اگر (gap->ga_len != 0)
	    mch_memmove(*fup, gap->ga_ڈیٹا, حجم(ufunc_T *) * gap->ga_len);
	VIM_CLEAR(gap->ga_ڈیٹا);
	اگر (loop == 1)
	    cl->class_class_function_گنتی_بچہ = gap->ga_len;
	ورنہ
	    cl->class_obj_طریقہ_گنتی_بچہ = gap->ga_len;

	اگر (loop == 2)
	{
	    // Copy instance methods from the parent.

	    برائے (صحیح i = 0; i < والد_گنتی; ++i)
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
		اگر (nf != تہی && IS_GENERIC_FUNC(nf)
					      && nf->uf_generic_دلائل != تہی)
		    generic_func_copy_types(pf, nf);

		// If the child class overrides a function from the parent
		// the signature must be equal.
		char_u *pname = pf->uf_name;
		برائے (صحیح ci = 0; ci < gap->ga_len; ++ci)
		{
		    ufunc_T *cf = (*fup)[ci];
		    char_u *cname = cf->uf_name;
		    اگر (STRCMP(pname, cname) == 0)
		    {
			where_T where = WHERE_INIT;
			where.wt_func_name = (حرف *)pname;
			where.wt_قسم = WT_طریقہ;
			(خالی)check_type(pf->uf_func_type, cf->uf_func_type,
								TRUE, where);
		    }
		}
	    }
	}

	// Set the class pointer on all the functions and object methods.
	برائے (صحیح i = 0; i < *fcount; ++i)
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

    واپس OK;
}

/*
 * Update the index of object methods called by builtin functions.
 */
    ساکن خالی
update_builtin_طریقہ_اشاریہ(class_T *cl)
{
    صحیح	i;

    برائے (i = 0; i < CLASS_BUILTIN_MAX; i++)
	cl->class_builtin_methods[i] = -1;

    برائے (i = 0; i < cl->class_obj_طریقہ_گنتی; i++)
    {
	ufunc_T *uf = cl->class_obj_methods[i];

	اگر (cl->class_builtin_methods[CLASS_BUILTIN_STRING] == -1
		&& STRCMP(uf->uf_name, "string") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_STRING] = i;
	ورنہ اگر (cl->class_builtin_methods[CLASS_BUILTIN_خالی_خانے] == -1 &&
		STRCMP(uf->uf_name, "empty") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_خالی_خانے] = i;
	ورنہ اگر (cl->class_builtin_methods[CLASS_BUILTIN_LEN] == -1 &&
		STRCMP(uf->uf_name, "len") == 0)
	    cl->class_builtin_methods[CLASS_BUILTIN_LEN] = i;
    }
}

/*
 * Return the end of the class name starting at "arg".  Valid characters in a
 * class name are alphanumeric characters and "_".  Also handles imported class
 * names.
 */
    ساکن char_u *
ڈھونڈو_class_name_انتہا(char_u *arg)
{
    char_u *انتہا = arg;

    جب (ASCII_ISALNUM(*انتہا) || *انتہا == '_'
	    || (*انتہا == '.' && (ASCII_ISALNUM(انتہا[1]) || انتہا[1] == '_')))
	++انتہا;

    واپس انتہا;
}

/*
 * Returns TRUE if the enum value "varname" is already defined.
 */
    ساکن صحیح
is_duplicate_enum(
    garray_T	*enum_gap,
    char_u	*varname,
    char_u	*varname_انتہا)
{
    string_T	name = {varname, (size_t)(varname_انتہا - varname)};  // Note: the .string field may
								    // point to a string longer
								    // than the .length field.
								    // So we need to use STRNCMP()
								    // to compare it.
    صحیح		dup = FALSE;

    برائے (صحیح i = 0; i < enum_gap->ga_len; ++i)
    {
	ocmember_T *m = ((ocmember_T *)enum_gap->ga_ڈیٹا) + i;

	اگر (name.لمبائی == m->ocm_name.لمبائی
	    && STRNCMP(name.string, m->ocm_name.string, name.لمبائی) == 0)
	{
	    char_u  save_c = *varname_انتہا;

	    *varname_انتہا = NUL;
	    semsg(_(e_duplicate_enum_str), varname);
	    *varname_انتہا = save_c;
	    dup = TRUE;
	    توڑو;
	}
    }

    واپس dup;
}

/*
 * Parse the enum values in "line" separated by comma and add them to "gap".
 * If the last enum value is found, then "enum_end" is set to TRUE.
 */
    ساکن صحیح
enum_parse_اقدار(
    exarg_T	*eap,
    class_T	*en,
    char_u	*سطر,
    garray_T	*gap,
    صحیح		*عدد_enum_اقدار,
    صحیح		*enum_انتہا)
{
    evalarg_T	evalarg;
    char_u	*p = سطر;
    حرف	initexpr_buf[1024];
    size_t	initexpr_buflen;
    char_u	آخری_char = NUL;
    صحیح		rc = OK;

    fill_evalarg_from_eap(&evalarg, eap, FALSE);

    صحیح		did_emsg_before = did_emsg;
    جب (*p != NUL)
    {
	// ignore comment
	اگر (*p == '#')
	    توڑو;

	اگر (!eval_isnamec1(*p))
	{
	    semsg(_(e_invalid_enum_قدر_declaration_str), p);
	    توڑو;
	}

	char_u *eni_name_آغاز = p;
	char_u *eni_name_انتہا = مقصد_name_انتہا(p, FALSE);

	اگر (is_duplicate_enum(gap, eni_name_آغاز, eni_name_انتہا))
	    توڑو;

	p = skipwhite(eni_name_انتہا);

	char_u	*init_اظہاریہ = تہی;
	اگر (*p == '(' || *p == '<')
	{
	    اگر (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), "(", سطر);
		توڑو;
	    }

	    char_u *اظہاریہ_آغاز, *اظہاریہ_انتہا;

	    p = eni_name_آغاز;
	    (خالی)skip_اظہاریہ_concatenate(&p, &اظہاریہ_آغاز, &اظہاریہ_انتہا, &evalarg);

	    جب (*اظہاریہ_آغاز && *اظہاریہ_آغاز != '(' && *اظہاریہ_آغاز != '<')
		اظہاریہ_آغاز++;

	    اگر (اظہاریہ_انتہا > اظہاریہ_آغاز)
		init_اظہاریہ = vim_strnsave(اظہاریہ_آغاز, اظہاریہ_انتہا - اظہاریہ_آغاز);
	}

	اگر (init_اظہاریہ == تہی)
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, حجم(initexpr_buf), "%s.new()", en->class_name.string);
	ورنہ
	{
	    initexpr_buflen = vim_snprintf_safelen(
		initexpr_buf, حجم(initexpr_buf), "%s.new%s", en->class_name.string,
		init_اظہاریہ);
	    vim_free(init_اظہاریہ);
	}
	اگر (جوڑو_member(gap, eni_name_آغاز, eni_name_انتہا, FALSE,
				TRUE, TRUE, TRUE, &en->class_object_type,
				vim_strnsave((char_u *)initexpr_buf, initexpr_buflen)) == FAIL)
	    توڑو;

	++*عدد_enum_اقدار;

	اگر (*p != '#')
	    آخری_char = *p;

	اگر (*p != NUL && *p != ',')
	    توڑو;

	اگر (*p == ',')
	{
	    اگر (!IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", سطر);
		توڑو;
	    }
	    اگر (VIM_ISWHITE(p[-1]))
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", سطر);
		توڑو;
	    }
	    p = skipwhite(p + 1);
	}
    }

    p = skipwhite(p);

    اگر (*p != NUL && *p != '#')
    {
	اگر (did_emsg == did_emsg_before)
	    semsg(_(e_missing_comma_before_دلیل_str), p);
	rc = FAIL;
    }

    اگر (آخری_char != ',')
	// last enum value should not be terminated by ","
	*enum_انتہا = TRUE;

    // Free the memory pointed by expr_start.
    clear_evalarg(&evalarg, تہی);

    واپس rc;
}

/*
 * Add the "values" class variable (List of enum value objects) to the enum
 * class "en"
 */
    ساکن صحیح
enum_جوڑو_اقدار_member(
    class_T	*en,
    garray_T	*gap,
    صحیح		عدد_enum_اقدار,
    garray_T	*type_list_gap)
{
    garray_T	fga;
    صحیح		rc = FAIL;

    ga_init2(&fga, 1, 1000);
    GA_CONCAT_LITERAL(&fga, "[");
    برائے (صحیح i = 0; i < عدد_enum_اقدار; ++i)
    {
	ocmember_T *m = ((ocmember_T *)gap->ga_ڈیٹا) + i;

	اگر (i > 0)
	    GA_CONCAT_LITERAL(&fga, ", ");
	ga_concat_len(&fga, en->class_name.string, en->class_name.لمبائی);
	GA_CONCAT_LITERAL(&fga, ".");
	ga_concat_len(&fga, (char_u *)m->ocm_name.string, m->ocm_name.لمبائی);
    }
    GA_CONCAT_LITERAL(&fga, "]");
    ga_append(&fga, NUL);

    char_u *varname = (char_u *)"values";

    type_T *type = get_type_ptr(type_list_gap);
    اگر (type == تہی)
	جاؤ مکمل;

    type->tt_type = VAR_LIST;
    type->tt_member = get_type_ptr(type_list_gap);
    اگر (type->tt_member != تہی)
    {
	type->tt_member->tt_type = VAR_OBJECT;
	type->tt_member->tt_class = en;
    }

    rc = جوڑو_member(gap, varname, varname + 6, FALSE, FALSE, TRUE, TRUE, type,
					vim_strnsave((char_u *)fga.ga_ڈیٹا, fga.ga_len));

مکمل:
    vim_free(fga.ga_ڈیٹا);

    واپس rc;
}

/*
 * Clear the constructor method names in a enum class, so that an enum class
 * cannot be instantiated.
 */
    ساکن خالی
enum_clear_constructors(class_T *en)
{
    برائے (صحیح i = 0; i < en->class_class_function_گنتی; ++i)
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
    خالی
enum_سیٹ_اندرونی_obj_vars(class_T *en, object_T *enval)
{
    صحیح	i;

    برائے (i = 0; i < en->class_class_member_گنتی; ++i)
    {
	typval_T *en_tv = en->class_members_tv + i;
	اگر (en_tv != تہی && en_tv->v_type == VAR_UNKNOWN)
	    توڑو;
    }

    اگر (i >= en->class_class_member_گنتی)
	// When adding enum values to an enum, the index value should be less
	// than the member count.  It will be greater only when appending a
	// new item to a list of enums.  In this case, skip setting the enum
	// name and ordinal (as this is a dummy enum object)
	واپس;

    // First object variable is the name
    ocmember_T *قدر_ocm = en->class_class_members + i;
    typval_T *name_tv = (typval_T *)(enval + 1);
    name_tv->v_type = VAR_STRING;
    name_tv->vval.v_string =
	vim_strnsave(قدر_ocm->ocm_name.string, قدر_ocm->ocm_name.لمبائی);

    // Second object variable is the ordinal
    typval_T *ord_tv = (typval_T *)(name_tv + 1);
    ord_tv->v_type = VAR_نمبر;
    ord_tv->vval.v_نمبر = i;
}

/*
 * Read the body up to ":endclass", ":endenum" or ":endinterface" and throw it
 * away.  Used while skipping.
 */
    ساکن خالی
skip_class_body(exarg_T *eap)
{
    برائے (;;)
    {
	char_u	*theline = eap->ea_getline(':', eap->cookie, 0,
							  GETLINE_CONCAT_ALL);
	اگر (theline == تہی)
	    توڑو;

	char_u	*p = theline;
	صحیح	ملا;

	// skip ':' and blanks
	برائے (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	ملا = checkforcmd(&p, "endclass", 4)
			|| checkforcmd(&p, "endenum", 4)
			|| checkforcmd(&p, "endinterface", 5);
	vim_free(theline);
	اگر (ملا)
	    توڑو;
    }
}

/*
 * Handle ":class" and ":abstract class" up to ":endclass".
 * Handle ":enum" up to ":endenum".
 * Handle ":interface" up to ":endinterface".
 */
    خالی
ex_class(exarg_T *eap)
{
    صحیح		is_class = eap->cmdidx == CMD_class;
    صحیح		is_abstract = eap->cmdidx == CMD_abstract;
    صحیح		is_enum = eap->cmdidx == CMD_enum;
    صحیح		added_enum_اقدار = FALSE;
    صحیح		is_interface;
    طویل	آغاز_lnum = SOURCING_LNUM;
    char_u	*arg = eap->arg;

    اگر (eap->skip)
    {
	skip_class_body(eap);
	واپس;
    }

    اگر (is_abstract)
    {
	اگر (STRNCMP(arg, "class", 5) != 0 || !VIM_ISWHITE(arg[5]))
	{
	    semsg(_(e_invalid_دلیل_str), arg);
	    واپس;
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
	    emsg(_(e_class_can_only_be_defined_اندر_vim9_script));
	ورنہ اگر (is_enum)
	    emsg(_(e_enum_can_only_be_defined_اندر_vim9_script));
	ورنہ
	    emsg(_(e_interface_can_only_be_defined_اندر_vim9_script));
	واپس;
    }

    اگر (!ASCII_ISUPPER(*arg))
    {
	اگر (is_class)
	    semsg(_(e_class_name_must_آغاز_with_uppercase_letter_str), arg);
	ورنہ اگر (is_enum)
	    semsg(_(e_enum_name_must_آغاز_with_uppercase_letter_str), arg);
	ورنہ
	    semsg(_(e_interface_name_must_آغاز_with_uppercase_letter_str),
									  arg);
	واپس;
    }
    char_u *name_انتہا = ڈھونڈو_name_انتہا(arg, تہی, تہی, FNE_CHECK_آغاز);
    اگر (!IS_WHITE_OR_NUL(*name_انتہا))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	واپس;
    }
    char_u *name_آغاز = arg;

    // TODO:
    //    generics: <Tkey, Tentry>

    // Name for "extends BaseClass"
    char_u *extends = تہی;

    // Names for "implements SomeInterface"
    garray_T	ga_impl;
    ga_init2(&ga_impl, حجم(char_u *), 5);

    arg = skipwhite(name_انتہا);
    جب (*arg != NUL && *arg != '#' && *arg != '\n')
    {
	// TODO:
	//    specifies SomeInterface
	اگر (STRNCMP(arg, "extends", 7) == 0 && IS_WHITE_OR_NUL(arg[7]))
	{
	    اگر (is_enum)
	    {
		emsg(_(e_enum_cannot_extend_class));
		جاؤ early_ret;
	    }
	    اگر (extends != تہی)
	    {
		emsg(_(e_duplicate_extends));
		جاؤ early_ret;
	    }
	    arg = skipwhite(arg + 7);

	    char_u *انتہا = ڈھونڈو_class_name_انتہا(arg);
	    اگر (!IS_WHITE_OR_NUL(*انتہا))
	    {
		semsg(_(e_white_space_required_after_name_str), arg);
		جاؤ early_ret;
	    }
	    extends = vim_strnsave(arg, انتہا - arg);
	    اگر (extends == تہی)
		جاؤ early_ret;

	    arg = skipwhite(انتہا + 1);
	}
	ورنہ اگر (STRNCMP(arg, "implements", 10) == 0
						   && IS_WHITE_OR_NUL(arg[10]))
	{
	    اگر (is_interface)
	    {
		emsg(_(e_interface_cannot_use_implements));
		جاؤ early_ret;
	    }

	    اگر (ga_impl.ga_len > 0)
	    {
		emsg(_(e_duplicate_implements));
		جاؤ early_ret;
	    }
	    arg = skipwhite(arg + 10);

	    برائے (;;)
	    {
		char_u *impl_انتہا = ڈھونڈو_class_name_انتہا(arg);
		اگر ((!IS_WHITE_OR_NUL(*impl_انتہا) && *impl_انتہا != ',')
			|| (*impl_انتہا == ','
			    && !IS_WHITE_OR_NUL(*(impl_انتہا + 1))))
		{
		    semsg(_(e_white_space_required_after_name_str), arg);
		    جاؤ early_ret;
		}
		اگر (impl_انتہا - arg == 0)
		{
		    emsg(_(e_missing_name_after_implements));
		    جاؤ early_ret;
		}

		char_u *iname = vim_strnsave(arg, impl_انتہا - arg);
		اگر (iname == تہی)
		    جاؤ early_ret;
		برائے (صحیح i = 0; i < ga_impl.ga_len; ++i)
		    اگر (STRCMP(((char_u **)ga_impl.ga_ڈیٹا)[i], iname) == 0)
		    {
			semsg(_(e_duplicate_interface_after_implements_str),
									iname);
			vim_free(iname);
			جاؤ early_ret;
		    }
		اگر (ga_جوڑو_string(&ga_impl, iname) == FAIL)
		{
		    vim_free(iname);
		    جاؤ early_ret;
		}
		اگر (*impl_انتہا != ',')
		{
		    arg = skipwhite(impl_انتہا);
		    توڑو;
		}
		arg = skipwhite(impl_انتہا + 1);
	    }
	}
	ورنہ
	{
	    semsg(_(e_trailing_characters_str), arg);
early_ret:
	    vim_free(extends);
	    ga_clear_لڑیاں(&ga_impl);
	    واپس;
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

    class_T *cl = تہی;
    class_T *extends_cl = تہی;  // class from "extends" argument
    class_T **intf_classes = تہی;
    صحیح	    intf_گنتی = 0;	// number of items in "intf_classes"
    صحیح	    عدد_enum_اقدار = 0;

    cl = ALLOC_CLEAR_ONE(class_T);
    اگر (cl == تہی)
	جاؤ cleanup;

    اگر (is_enum)
	cl->class_flags = CLASS_ENUM;
    ورنہ اگر (is_interface)
	cl->class_flags = CLASS_INTERFACE;
    ورنہ اگر (is_abstract)
	cl->class_flags = CLASS_ABSTRACT;

    cl->class_refcount = 1;
    cl->class_name.لمبائی = (size_t)(name_انتہا - name_آغاز);
    cl->class_name.string = vim_strnsave(name_آغاز, cl->class_name.لمبائی);
    اگر (cl->class_name.string == تہی)
	جاؤ cleanup;

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
    SOURCING_LNUM = آغاز_lnum;
    صحیح rc = سیٹ_var_const(cl->class_name.string, 0, تہی, &tv, FALSE, 0, 0);
    اگر (rc == FAIL)
	جاؤ cleanup;

    اگر (is_enum)
    {
	// All the enum classes have the name and ordinal object variables.
	char_u *varname = (char_u *)"name";
	اگر (جوڑو_member(&objmembers, varname, varname + 4, FALSE, FALSE, TRUE,
		    TRUE, &t_string, تہی) == FAIL)
	    جاؤ cleanup;

	varname = (char_u *)"ordinal";
	اگر (جوڑو_member(&objmembers, varname, varname + 7, FALSE, FALSE, TRUE,
		    TRUE, &t_نمبر, تہی) == FAIL)
	    جاؤ cleanup;
    }

    // "export class" gets used when creating the class, don't use "is_export"
    // for the items inside the class.
    is_export = FALSE;

    // When parsing an enum definition, this denotes whether all the enumerated
    // values are parsed or not.
    صحیح		enum_انتہا = FALSE;

    /*
     * Go over the body of the class/interface until "endclass" or
     * "endinterface" or "endenum" is found.
     */
    char_u *theline = تہی;
    صحیح success = FALSE;
    برائے (;;)
    {
	vim_free(theline);
	theline = eap->ea_getline(':', eap->cookie, 0, GETLINE_CONCAT_ALL);
	اگر (theline == تہی)
	    توڑو;
	char_u *سطر = skipwhite(theline);

	// Skip empty and comment lines.
	اگر (*سطر == NUL)
	    جاری;
	اگر (*سطر == '#')
	{
	    اگر (vim9_bad_comment(سطر))
		توڑو;
	    جاری;
	}

	char_u *p = سطر;

	حرف	*انتہا_name;
	صحیح	shortlen;
	صحیح	fullen;
	اگر (is_class)
	{
	    انتہا_name = "endclass";
	    shortlen = 4;
	    fullen = 8;
	}
	ورنہ اگر (is_enum)
	{
	    انتہا_name = "endenum";
	    shortlen = 4;
	    fullen = 7;
	}
	ورنہ
	{
	    انتہا_name = "endinterface";
	    shortlen = 5;
	    fullen = 12;
	}

	// skip ':' and blanks
	برائے (; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    ;
	char_u *cmd_آغاز = p;
	اگر (checkforcmd(&p, انتہا_name, shortlen))
	{
	    اگر (STRNCMP(cmd_آغاز, انتہا_name, fullen) != 0)
		semsg(_(e_command_cannot_be_shortened_str), cmd_آغاز);
	    ورنہ اگر (*p == '|' || !سرے_excmd2(سطر, p))
		semsg(_(e_trailing_characters_str), p);
	    ورنہ
		success = TRUE;
	    توڑو;
	}

	صحیح	wrong_endname = FALSE;
	اگر (is_class)
	    wrong_endname = checkforcmd(&p, "endinterface", 5)
					|| checkforcmd(&p, "endenum", 4);
	ورنہ اگر (is_enum)
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endinterface", 5);
	ورنہ
	    wrong_endname = checkforcmd(&p, "endclass", 4)
					|| checkforcmd(&p, "endenum", 4);
	اگر (wrong_endname)
	{
	    semsg(_(e_invalid_command_str_expected_str), سطر, انتہا_name);
	    توڑو;
	}

	اگر (is_enum && !enum_انتہا)
	{
	    // In an enum, all the enumerated values are at the beginning
	    // separated by comma.  The class and object variables/methods
	    // follow the values.
	    اگر (enum_parse_اقدار(eap, cl, سطر, &classmembers,
					&عدد_enum_اقدار, &enum_انتہا) == FAIL)
		توڑو;

	    اگر (enum_انتہا)
	    {
		// Add the enum "values" class variable.
		enum_جوڑو_اقدار_member(cl, &classmembers, عدد_enum_اقدار,
							&type_list);
		added_enum_اقدار = TRUE;
	    }
	    جاری;
	}

	صحیح has_عوامی = FALSE;
	اگر (checkforcmd(&p, "public", 3))
	{
	    اگر (STRNCMP(سطر, "public", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), سطر);
		توڑو;
	    }
	    اگر (is_interface)
	    {
		emsg(_(e_عوامی_variable_not_supported_اندر_interface));
		توڑو;
	    }
	    has_عوامی = TRUE;
	    p = skipwhite(سطر + 6);

	    اگر (STRNCMP(p, "def", 3) == 0)
	    {
		emsg(_(e_عوامی_keyword_not_supported_for_طریقہ));
		توڑو;
	    }

	    اگر (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "static", 6) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_عوامی_must_be_followed_by_var_static_final_or_const));
		توڑو;
	    }
	}

	صحیح abstract_طریقہ = FALSE;
	char_u *pa = p;
	اگر (checkforcmd(&p, "abstract", 3))
	{
	    اگر (STRNCMP(pa, "abstract", 8) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), pa);
		توڑو;
	    }

	    اگر (is_enum)
	    {
		// "abstract" not supported in an enum
		emsg(_(e_abstract_cannot_be_مستعمل_اندر_enum));
		توڑو;
	    }

	    اگر (is_interface)
	    {
		// "abstract" not supported in an interface
		emsg(_(e_abstract_cannot_be_مستعمل_اندر_interface));
		توڑو;
	    }

	    اگر (!is_abstract)
	    {
		semsg(_(e_abstract_طریقہ_اندر_concrete_class), pa);
		توڑو;
	    }

	    p = skipwhite(pa + 8);
	    اگر (STRNCMP(p, "def", 3) != 0)
	    {
		emsg(_(e_abstract_must_be_followed_by_def));
		توڑو;
	    }

	    abstract_طریقہ = TRUE;
	}

	صحیح has_static = FALSE;
	char_u *ps = p;
	اگر (checkforcmd(&p, "static", 4))
	{
	    اگر (STRNCMP(ps, "static", 6) != 0)
	    {
		semsg(_(e_command_cannot_be_shortened_str), ps);
		توڑو;
	    }

	    اگر (is_interface)
	    {
		emsg(_(e_static_member_not_supported_اندر_interface));
		توڑو;
	    }
	    has_static = TRUE;
	    p = skipwhite(ps + 6);

	    اگر (STRNCMP(p, "var", 3) != 0 && STRNCMP(p, "def", 3) != 0
		&& STRNCMP(p, "final", 5) != 0 && STRNCMP(p, "const", 5) != 0)
	    {
		emsg(_(e_static_must_be_followed_by_var_def_final_or_const));
		توڑو;
	    }
	}

	صحیح has_final = FALSE;
	صحیح has_var = FALSE;
	صحیح has_const = FALSE;
	اگر (checkforcmd(&p, "var", 3))
	    has_var = TRUE;
	ورنہ اگر (checkforcmd(&p, "final", 5))
	{
	    اگر (is_interface)
	    {
		emsg(_(e_final_variable_not_supported_اندر_interface));
		توڑو;
	    }
	    has_final = TRUE;
	}
	ورنہ اگر (checkforcmd(&p, "const", 5))
	{
	    اگر (is_interface)
	    {
		emsg(_(e_const_variable_not_supported_اندر_interface));
		توڑو;
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
	    char_u *varname_انتہا = تہی;
	    type_T *type = تہی;
	    char_u *init_اظہاریہ = تہی;
	    صحیح	    has_type = FALSE;

	    اگر (!eval_isnamec1(*p))
	    {
		اگر (has_static)
		    semsg(_(e_invalid_class_variable_declaration_str), سطر);
		ورنہ
		    semsg(_(e_invalid_object_variable_declaration_str), سطر);
		توڑو;
	    }

	    اگر (is_interface && *varname == '_')
	    {
		// private variables are not supported in an interface
		semsg(_(e_protected_variable_not_supported_اندر_interface),
			varname);
		توڑو;
	    }

	    اگر (parse_member(eap, سطر, varname, has_عوامی,
			  &varname_انتہا, &has_type, &type_list, &type,
			  !is_interface ? &init_اظہاریہ: تہی) == FAIL)
		توڑو;

	    اگر (is_reserved_varname(varname, varname_انتہا)
		    || is_duplicate_variable(&classmembers, &objmembers,
							varname, varname_انتہا))
	    {
		vim_free(init_اظہاریہ);
		توڑو;
	    }
	    اگر (جوڑو_member(has_static ? &classmembers : &objmembers, varname,
				varname_انتہا, has_عوامی, has_final, has_const,
				has_type, type, init_اظہاریہ) == FAIL)
	    {
		vim_free(init_اظہاریہ);
		توڑو;
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
	ورنہ اگر (checkforcmd(&p, "def", 3))
	{
	    exarg_T	ea;
	    garray_T	سطریں_مقصد_free;
	    صحیح		is_new = STRNCMP(p, "new", 3) == 0
						|| STRNCMP(p, "_new", 4) == 0;

	    اگر (has_عوامی)
	    {
		// "public" keyword is not supported when defining an object or
		// class method
		emsg(_(e_عوامی_keyword_not_supported_for_طریقہ));
		توڑو;
	    }

	    اگر (*p == NUL)
	    {
		// No method name following def
		semsg(_(e_not_valid_command_اندر_class_str), سطر);
		توڑو;
	    }

	    اگر (is_interface && *p == '_')
	    {
		// private methods are not supported in an interface
		semsg(_(e_protected_طریقہ_not_supported_اندر_interface), p);
		توڑو;
	    }

	    اگر (has_static && !is_new && محفوظ_islower(*p) &&
					is_valid_builtin_obj_methodname(p))
	    {
		semsg(_(e_builtin_class_طریقہ_not_supported), p);
		توڑو;
	    }

	    CLEAR_میدان(ea);
	    ea.cmd = سطر;
	    ea.arg = p;
	    ea.cmdidx = CMD_def;
	    ea.ea_getline = eap->ea_getline;
	    ea.cookie = eap->cookie;

	    ga_init2(&سطریں_مقصد_free, حجم(char_u *), 50);
	    صحیح class_flags;
	    اگر (is_interface)
		class_flags = CF_INTERFACE;
	    ورنہ
		class_flags = abstract_طریقہ ? CF_ABSTRACT_طریقہ : CF_CLASS;
	    ufunc_T *uf = define_function(&ea, تہی, &سطریں_مقصد_free,
			class_flags, objmembers.ga_ڈیٹا, objmembers.ga_len,
			تہی);
	    ga_clear_لڑیاں(&سطریں_مقصد_free);

	    اگر (uf != تہی)
	    {
		اگر (is_new && !is_valid_constructor(uf, is_abstract,
								has_static))
		{
		    // private variables are not supported in an interface
		    semsg(_(e_protected_طریقہ_not_supported_اندر_interface),
			    uf->uf_name);
		    func_clear_free(uf, FALSE);
		    توڑو;
		}

		// check for builtin method
		اگر (!is_new && محفوظ_islower(*uf->uf_name) &&
					!object_check_builtin_طریقہ_sig(uf))
		{
		    func_clear_free(uf, FALSE);
		    توڑو;
		}

		// Check the uf_name isn't used already.
		اگر (is_duplicate_طریقہ(&classfunctions, &objmethods, uf->uf_name))
		{
		    success = FALSE;
		    func_clear_free(uf, FALSE);
		    توڑو;
		}

		garray_T *fgap = has_static || is_new
					       ? &classfunctions : &objmethods;
		اگر (ga_grow(fgap, 1) == OK)
		{
		    اگر (is_new)
			uf->uf_flags |= FC_NEW;

		    اگر (abstract_طریقہ)
			uf->uf_flags |= FC_ABSTRACT;

		    ((ufunc_T **)fgap->ga_ڈیٹا)[fgap->ga_len] = uf;
		    ++fgap->ga_len;
		}
	    }
	}

	ورنہ
	{
	    اگر (is_class)
		semsg(_(e_not_valid_command_اندر_class_str), سطر);
	    ورنہ اگر (is_enum)
		semsg(_(e_not_valid_command_اندر_enum_str), سطر);
	    ورنہ
		semsg(_(e_not_valid_command_اندر_interface_str), سطر);
	    توڑو;
	}
    }

    اگر (theline == تہی && !success && is_enum)
	emsg(_(e_missing_endenum));

    vim_free(theline);

    اگر (success && is_enum && (عدد_enum_اقدار == 0 || !added_enum_اقدار))
	// Empty enum statement. Add an empty "values" class variable
	success = enum_جوڑو_اقدار_member(cl, &classmembers, عدد_enum_اقدار,
								&type_list);

    /*
     * Check a few things
     */

    // Check the "extends" class is valid.
    اگر (success && extends != تہی)
	success = validate_extends_class(cl, extends, &extends_cl, is_class);
    VIM_CLEAR(extends);

    // Check the new object methods to make sure their access (public or
    // private) is the same as that in the extended class lineage.
    اگر (success && extends_cl != تہی)
	success = validate_extends_methods(&objmethods, extends_cl);

    // Check the new class and object variables are not duplicates of the
    // variables in the extended class lineage.  If an interface is extending
    // another interface, then it can duplicate the member variables.
    اگر (success && extends_cl != تہی)
    {
	اگر (is_class)
	    success = extends_check_dup_members(&objmembers, extends_cl);
	ورنہ
	    success = extends_check_intf_var_type(&objmembers, extends_cl);
    }

    // When extending an abstract class, make sure all the abstract methods in
    // the parent class are implemented.  If the current class is an abstract
    // class, then there is no need for this check.
    اگر (success && !is_abstract && extends_cl != تہی
				&& (extends_cl->class_flags & CLASS_ABSTRACT))
	success = validate_abstract_class_methods(&classfunctions,
						&objmethods, extends_cl);

    // Process the "implements" entries
    // Check all "implements" entries are valid.
    garray_T  intf_classes_ga;

    ga_init2(&intf_classes_ga, حجم(class_T *), 5);

    اگر (success && ga_impl.ga_len > 0)
	success = validate_implements_classes(cl, &ga_impl, &intf_classes_ga,
					&objmethods, &objmembers, extends_cl);

    // inherit the super class interfaces
    اگر (success && extends_cl != تہی)
	success = جوڑو_super_class_interfaces(extends_cl, &ga_impl,
							&intf_classes_ga);

    // When validating failed "intf_classes" may have fewer items than
    // "ga_impl".
    intf_classes = intf_classes_ga.ga_ڈیٹا;
    intf_گنتی = intf_classes_ga.ga_len;
    intf_classes_ga.ga_len = 0;

    // Check no function argument name is used as a class member.
    اگر (success)
	success = check_func_arg_نام(&classfunctions, &objmethods,
							&classmembers);

    اگر (success)
    {
	// "endclass" or "endinterface" or "endenum" encountered without any
	// failures

	اگر (extends_cl != تہی)
	{
	    cl->class_extends = extends_cl;
	    extends_cl->class_flags |= CLASS_EXTENDED;
	}

	// Add class and object variables to "cl".
	اگر (جوڑو_members_مقصد_class(&classmembers,
				 تہی,
				 0,
				 &cl->class_class_members,
				 &cl->class_class_member_گنتی) == FAIL
		|| جوڑو_members_مقصد_class(&objmembers,
				 extends_cl == تہی ? تہی
					       : extends_cl->class_obj_members,
				 extends_cl == تہی ? 0
					  : extends_cl->class_obj_member_گنتی,
				 &cl->class_obj_members,
				 &cl->class_obj_member_گنتی) == FAIL)
	    جاؤ cleanup;

	اگر (ga_impl.ga_len > 0)
	{
	    // Move the "implements" names into the class.
	    cl->class_interface_گنتی = ga_impl.ga_len;
	    cl->class_interfaces = ALLOC_MULT(char_u *, ga_impl.ga_len);
	    اگر (cl->class_interfaces == تہی)
		جاؤ cleanup;
	    برائے (صحیح i = 0; i < ga_impl.ga_len; ++i)
		cl->class_interfaces[i] = ((char_u **)ga_impl.ga_ڈیٹا)[i];
	    VIM_CLEAR(ga_impl.ga_ڈیٹا);
	    ga_impl.ga_len = 0;

	    cl->class_interfaces_cl = intf_classes;
	    intf_classes = تہی;
	}

	اگر (cl->class_interface_گنتی > 0 || extends_cl != تہی)
	{
	    // Add a method and member lookup table to each of the interface
	    // classes.
	    اگر (جوڑو_lookup_tables(cl, extends_cl, &objmethods) == FAIL)
		جاؤ cleanup;
	}

	صحیح	have_new = FALSE;
	ufunc_T	*class_func = تہی;
	برائے (صحیح i = 0; i < classfunctions.ga_len; ++i)
	{
	    class_func = ((ufunc_T **)classfunctions.ga_ڈیٹا)[i];
	    اگر (STRCMP(class_func->uf_name, "new") == 0
				|| STRCMP(class_func->uf_name, "_new") == 0)
	    {
		have_new = TRUE;
		توڑو;
	    }
	}

	اگر (have_new)
	    // The return type of new() is an object of class "cl"
	    class_func->uf_ret_type->tt_class = cl;
	ورنہ اگر ((is_class || is_enum) && !is_abstract && !have_new)
	    // No new() method was defined, add the default constructor.
	    جوڑو_default_constructor(cl, &classfunctions, &type_list);

	// Move all the functions into the created class.
	اگر (جوڑو_classfuncs_objmethods(cl, extends_cl, &classfunctions,
							&objmethods) == FAIL)
	    جاؤ cleanup;

	update_builtin_طریقہ_اشاریہ(cl);

	class_created(cl);

	// Allocate a typval for each class member and initialize it.
	اگر ((is_class || is_enum) && cl->class_class_member_گنتی > 0)
	    اگر (جوڑو_class_members(cl, eap, &type_list) == FAIL)
	    {
		cl->class_type_list = type_list;
		واپس;
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

	واپس;
    }

cleanup:
    vim_free(extends);
    class_unref(extends_cl);

    اگر (intf_classes != تہی)
    {
	برائے (صحیح i = 0; i < intf_گنتی; ++i)
	    class_unref(intf_classes[i]);
	vim_free(intf_classes);
    }
    ga_clear_لڑیاں(&ga_impl);

    برائے (صحیح باری = 1; باری <= 2; ++باری)
    {
	garray_T *gap = باری == 1 ? &classmembers : &objmembers;
	اگر (gap->ga_len == 0 || gap->ga_ڈیٹا == تہی)
	    جاری;

	برائے (صحیح i = 0; i < gap->ga_len; ++i)
	{
	    ocmember_T *m = ((ocmember_T *)gap->ga_ڈیٹا) + i;
	    VIM_CLEAR_STRING(m->ocm_name);
	    vim_free(m->ocm_init);
	}
	ga_clear(gap);
    }

    برائے (صحیح i = 0; i < objmethods.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)objmethods.ga_ڈیٹا)[i];
	func_clear_free(uf, FALSE);
    }
    ga_clear(&objmethods);

    برائے (صحیح i = 0; i < classfunctions.ga_len; ++i)
    {
	ufunc_T *uf = ((ufunc_T **)classfunctions.ga_ڈیٹا)[i];
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
    char_u	*name_انتہا,
    صحیح		*member_idx)
{
    size_t	len = name_انتہا - name;
    ocmember_T	*m;

    *member_idx = -1;  // not found (yet)

    m = member_lookup(cl, is_object ? VAR_OBJECT : VAR_CLASS, name, len,
								member_idx);
    اگر (m == تہی)
    {
	member_not_ملا_پیغام(cl, is_object ? VAR_OBJECT : VAR_CLASS, name,
									len);
	واپس &t_any;
    }

    واپس m->ocm_type;
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
    صحیح		member_گنتی;

    اگر (is_object)
    {
	m = cl->class_obj_members;
	member_گنتی = cl->class_obj_member_گنتی;
    }
    ورنہ
    {
	m = cl->class_class_members;
	member_گنتی = cl->class_class_member_گنتی;
    }

    اگر (member_idx >= member_گنتی)
	واپس تہی;

    واپس m[member_idx].ocm_type;
}

/*
 * Type aliases (:type)
 */

    ساکن خالی
typealias_free(typealias_T *ta)
{
    // ta->ta_type is freed in clear_type_list()
    vim_free(ta->ta_name);
    vim_free(ta);
}

    خالی
typealias_unref(typealias_T *ta)
{
    اگر (ta != تہی && --ta->ta_refcount <= 0)
	typealias_free(ta);
}

/*
 * Handle ":type".  Create an alias for a type specification.
 */
    خالی
ex_type(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    اگر (!current_script_is_vim9()
		|| (cmdmod.cmod_flags & CMOD_LEGACY)
		|| !getline_equal(eap->ea_getline, eap->cookie, getsourceline))
    {
	emsg(_(e_type_can_only_be_defined_اندر_vim9_script));
	واپس;
    }

    اگر (*arg == NUL)
    {
	emsg(_(e_missing_typealias_name));
	واپس;
    }

    اگر (!ASCII_ISUPPER(*arg))
    {
	semsg(_(e_type_name_must_آغاز_with_uppercase_letter_str), arg);
	واپس;
    }

    char_u *name_انتہا = ڈھونڈو_name_انتہا(arg, تہی, تہی, FNE_CHECK_آغاز);
    اگر (!IS_WHITE_OR_NUL(*name_انتہا))
    {
	semsg(_(e_white_space_required_after_name_str), arg);
	واپس;
    }
    char_u *name_آغاز = arg;

    arg = skipwhite(name_انتہا);
    اگر (*arg != '=')
    {
	semsg(_(e_missing_equal_str), arg);
	واپس;
    }
    اگر (!IS_WHITE_OR_NUL(*(arg + 1)))
    {
	semsg(_(e_white_space_required_after_str_str), "=", arg);
	واپس;
    }
    arg++;
    arg = skipwhite(arg);

    اگر (*arg == NUL)
    {
	emsg(_(e_missing_typealias_type));
	واپس;
    }

    scriptitem_T    *si = SCRIPT_شے(current_sctx.sc_sid);
    type_T *type = parse_type(&arg, &si->sn_type_list, تہی, تہی, TRUE);
    اگر (type == تہی)
	واپس;

    اگر (*arg != NUL)
    {
	// some text after the type
	semsg(_(e_trailing_characters_str), arg);
	واپس;
    }

    صحیح cc = *name_انتہا;
    *name_انتہا = NUL;

    typval_T tv;
    tv.v_type = VAR_UNKNOWN;
    اگر (eval_variable_import(name_آغاز, &tv) == OK)
    {
	اگر (tv.v_type == VAR_TYPEALIAS)
	    semsg(_(e_typealias_already_exists_for_str), name_آغاز);
	ورنہ
	    semsg(_(e_redefining_script_شے_str), name_آغاز);
	clear_tv(&tv);
	جاؤ مکمل;
    }

    // Create a script-local variable for the type alias.
    اگر (type->tt_type != VAR_OBJECT)
    {
	tv.v_type = VAR_TYPEALIAS;
	tv.v_تالا = 0;
	tv.vval.v_typealias = ALLOC_CLEAR_ONE(typealias_T);
	++tv.vval.v_typealias->ta_refcount;
	tv.vval.v_typealias->ta_name = vim_strsave(name_آغاز);
	tv.vval.v_typealias->ta_type = type;
	tv.vval.v_typealias->ta_گہرائی = parse_type_گہرائی();
    }
    ورنہ
    {
	// When creating a type alias for a class, use the class type itself to
	// create the type alias variable.  This is needed to use the type
	// alias to invoke class methods (e.g. new()) and use class variables.
	tv.v_type = VAR_CLASS;
	tv.v_تالا = 0;
	tv.vval.v_class = type->tt_class;
	++tv.vval.v_class->class_refcount;
    }
    سیٹ_var_const(name_آغاز, current_sctx.sc_sid, تہی, &tv, FALSE,
						تفویض_CONST | تفویض_FINAL, 0);

مکمل:
    *name_انتہا = cc;
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
    اگر (m == تہی)
	واپس FAIL;

    اگر (*name == '_' && (current_class == تہی ||
				!class_instance_کا(current_class, cl)))
    {
	emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
	واپس FAIL;
    }

    اگر (is_object)
    {
	// The object only contains a pointer to the class, the member values
	// array follows right after that.
	object_T *obj = rettv->vval.v_object;
	typval_T *tv = (typval_T *)(obj + 1) + m_idx;
	copy_tv(tv, rettv);
	سیٹ_tv_type(rettv, m->ocm_type);
	object_unref(obj);
    }
    ورنہ
    {
	copy_tv(&cl->class_members_tv[m_idx], rettv);
	سیٹ_tv_type(rettv, m->ocm_type);
	class_unref(cl);
    }

    واپس OK;
}

/*
 * Call an object or class method "name" in class "cl".  The method return
 * value is returned in "rettv".
 */
    ساکن صحیح
call_oc_طریقہ(
    class_T	*cl,
    char_u	*name,
    size_t	len,
    char_u	*name_انتہا,
    evalarg_T	*evalarg,
    char_u	**arg,
    gfargs_tab_T	*gfatab,	// generic types
    typval_T	*rettv)
{
    ufunc_T	*fp;
    typval_T	argvars[MAX_FUNC_دلائل + 1];
    صحیح		argcount = 0;
    ocmember_T	*ocm = تہی;
    صحیح		m_idx;

    fp = طریقہ_lookup(cl, rettv->v_type, name, len, تہی);
    اگر (fp == تہی)
    {
	// could be an object or class funcref variable
	ocm = member_lookup(cl, rettv->v_type, name, len, &m_idx);
	اگر (ocm == تہی || ocm->ocm_type->tt_type != VAR_FUNC)
	{
	    طریقہ_not_ملا_پیغام(cl, rettv->v_type, name, len);
	    واپس FAIL;
	}

	اگر (*name == '_')
	{
	    // Protected object or class funcref variable
	    semsg(_(e_cannot_access_protected_variable_str), ocm->ocm_name.string,
		    cl->class_name.string);
	    واپس FAIL;
	}

	اگر (rettv->v_type == VAR_OBJECT)
	{
	    // funcref object variable
	    object_T	*obj = rettv->vval.v_object;
	    typval_T	*tv = (typval_T *)(obj + 1) + m_idx;
	    copy_tv(tv, rettv);
	}
	ورنہ
	    // funcref class variable
	    copy_tv(&cl->class_members_tv[m_idx], rettv);
	*arg = name_انتہا;
	واپس OK;
    }

    اگر (ocm == تہی && *fp->uf_name == '_')
    {
	// Cannot access a protected method outside of a class
	semsg(_(e_cannot_access_protected_طریقہ_str), fp->uf_name);
	واپس FAIL;
    }

    // process generic function call
    اگر (fp != تہی)
    {
	fp = generic_func_get(fp, gfatab);
	اگر (fp == تہی)
	    واپس FAIL;
    }

    char_u *argp = name_انتہا;
    صحیح ret = get_func_arguments(&argp, evalarg, 0, argvars, &argcount, FALSE);
    اگر (ret == FAIL)
    {
	جب (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	واپس FAIL;
    }

    funcexe_T funcexe;
    CLEAR_میدان(funcexe);
    funcexe.fe_evaluate = TRUE;
    اگر (rettv->v_type == VAR_OBJECT)
    {
	funcexe.fe_object = rettv->vval.v_object;
	++funcexe.fe_object->obj_refcount;
    }
    اگر (evalarg != تہی)
	funcexe.fe_cctx = evalarg->eval_cctx;

    // Clear the class or object after calling the function, in
    // case the refcount is one.
    typval_T tv_tofree = *rettv;
    rettv->v_type = VAR_UNKNOWN;

    // Call the user function.  Result goes into rettv;
    صحیح error = call_user_func_check(fp, argcount, argvars, rettv, &funcexe,
								تہی);

    // Clear the previous rettv and the arguments.
    clear_tv(&tv_tofree);
    برائے (صحیح idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    اگر (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(fp), funcexe.fe_ملا_var);
	واپس FAIL;
    }
    *arg = argp;

    واپس OK;
}

/*
 * Create a partial typval for "obj.obj_method" and store it in "rettv".
 * Returns OK on success and FAIL on memory allocation failure.
 */
    صحیح
obj_طریقہ_مقصد_partial_tv(object_T *obj, ufunc_T *obj_طریقہ, typval_T *rettv)
{
    partial_T *pt = ALLOC_CLEAR_ONE(partial_T);
    اگر (pt == تہی)
	واپس FAIL;

    pt->pt_refcount = 1;
    اگر (obj != تہی)
    {
	pt->pt_obj = obj;
	++pt->pt_obj->obj_refcount;
    }
    pt->pt_auto = TRUE;
    pt->pt_func = obj_طریقہ;
    func_ptr_ref(pt->pt_func);

    rettv->v_type = VAR_PARTIAL;
    rettv->vval.v_partial = pt;

    واپس OK;
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
class_object_اشاریہ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		verbose غیر_مستعمل)	// give error messages
{
    صحیح	ret = FAIL;

    اگر (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	واپس FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_انتہا = ڈھونڈو_name_انتہا(name, تہی, تہی, FNE_CHECK_آغاز);
    اگر (name_انتہا == name)
	واپس FAIL;
    size_t len = name_انتہا - name;

    صحیح did_emsg_save = did_emsg;
    class_T *cl;
    اگر (rettv->v_type == VAR_CLASS)
	cl = rettv->vval.v_class;
    ورنہ // VAR_OBJECT
    {
	اگر (rettv->vval.v_object == تہی)
	{
	    emsg(_(e_using_null_object));
	    واپس FAIL;
	}
	cl = rettv->vval.v_object->obj_class;
    }

    اگر (cl == تہی)
    {
	emsg(_(e_incomplete_type));
	واپس FAIL;
    }

    gfargs_tab_T    gfatab;

    generic_func_دلائل_جدول_init(&gfatab);

    اگر (*name_انتہا == '<')
    {
	cctx_T	    *cctx = تہی;

	اگر (evalarg != تہی)
	    cctx = evalarg->eval_cctx;

	// calling a generic method
	name_انتہا = parse_generic_func_type_دلائل(name, len, name + len,
						&gfatab, cctx);
	اگر (name_انتہا == تہی)
	    جاؤ مکمل;
    }

    اگر (*name_انتہا == '(')
	// Invoke the class or object method
	ret = call_oc_طریقہ(cl, name, len, name_انتہا, evalarg, arg,
							&gfatab, rettv);
    ورنہ اگر (rettv->v_type == VAR_OBJECT || rettv->v_type == VAR_CLASS)
    {
	// Search in the object member variable table and the class member
	// variable table.
	صحیح is_object = rettv->v_type == VAR_OBJECT;
	اگر (get_member_tv(cl, is_object, name, len, تہی, rettv) == OK)
	{
	    *arg = name_انتہا;
	    ret = OK;
	    جاؤ مکمل;
	}

	// could be a class method or an object method
	صحیح	fidx;
	ufunc_T	*fp = طریقہ_lookup(cl, rettv->v_type, name, len, &fidx);
	اگر (fp != تہی)
	{
	    // Protected methods are not accessible outside the class
	    اگر (fp->uf_defclass != evalarg->eval_class
		    && *name == '_')
	    {
		semsg(_(e_cannot_access_protected_طریقہ_str), fp->uf_name);
		جاؤ مکمل;
	    }

	    اگر (obj_طریقہ_مقصد_partial_tv(is_object ? rettv->vval.v_object :
						تہی, fp, rettv) == FAIL)
		جاؤ مکمل;

	    *arg = name_انتہا;
	    ret = OK;
	    جاؤ مکمل;
	}

	اگر (did_emsg == did_emsg_save)
	    member_not_ملا_پیغام(cl, rettv->v_type, name, len);
    }

مکمل:
    generic_func_دلائل_جدول_clear(&gfatab);

    واپس ret;
}

/*
 * If "arg" points to a class or object method, return it.
 * Otherwise return NULL.
 */
    ufunc_T *
ڈھونڈو_class_func(char_u **arg)
{
    char_u *name = *arg;
    char_u *name_انتہا = ڈھونڈو_name_انتہا(name, تہی, تہی, FNE_CHECK_آغاز);
    اگر (name_انتہا == name || *name_انتہا != '.')
	واپس تہی;

    ufunc_T	*fp = تہی;
    size_t	len = name_انتہا - name;
    typval_T	tv;
    tv.v_type = VAR_UNKNOWN;
    اگر (eval_variable(name, (صحیح)len,
				    0, &tv, تہی, EVAL_VAR_NOAUTOLOAD) == FAIL)
	واپس تہی;
    اگر (tv.v_type != VAR_CLASS && tv.v_type != VAR_OBJECT)
	جاؤ fail_after_eval;

    class_T *cl = tv.v_type == VAR_CLASS ? tv.vval.v_class
						 : tv.vval.v_object->obj_class;
    اگر (cl == تہی)
	جاؤ fail_after_eval;
    char_u *fname = name_انتہا + 1;
    char_u *fname_انتہا = ڈھونڈو_name_انتہا(fname, تہی, تہی, FNE_CHECK_آغاز);
    اگر (fname_انتہا == fname)
	جاؤ fail_after_eval;
    len = fname_انتہا - fname;

    fp = طریقہ_lookup(cl, tv.v_type, fname, len, تہی);

    اگر (fp != تہی)
    {
	fp = eval_generic_func(fp, fname, &fname_انتہا);
	اگر (fp != تہی)
	    *arg = fname_انتہا;
    }

fail_after_eval:
    clear_tv(&tv);
    واپس fp;
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
    واپس idx;
}

/*
 * Returns a pointer to the class member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The member variable index is set in "idx".
 */
    ocmember_T *
class_member_lookup(class_T *cl, char_u *name, size_t namelen, صحیح *idx)
{
    ocmember_T	*ret_m = تہی;
    صحیح		ret_idx = -1;
    برائے (صحیح i = 0; i < cl->class_class_member_گنتی; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	اگر (namelen)
	{
	    اگر (namelen == m->ocm_name.لمبائی
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		توڑو;
	    }
	}
	ورنہ اگر (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    توڑو;
	}
    }
    اگر (idx != تہی)
	*idx = ret_idx;
    واپس ret_m;
}

/*
 * Returns a pointer to the class method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The method index is set in "idx".
 */
    ساکن ufunc_T *
class_طریقہ_lookup(class_T *cl, char_u *name, size_t namelen, صحیح *idx)
{
    ufunc_T	*ret_fp = تہی;
    صحیح		ret_idx = -1;
    برائے (صحیح i = 0; i < cl->class_class_function_گنتی; ++i)
    {
	ufunc_T *fp = cl->class_class_functions[i];
	char_u *ufname = (char_u *)fp->uf_name;
	اگر (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    توڑو;
	}
    }
    اگر (idx != تہی)
	*idx = ret_idx;
    واپس ret_fp;
}

/*
 * Returns the index of class method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    صحیح
class_طریقہ_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحیح idx;
    class_طریقہ_lookup(cl, name, namelen, &idx);
    واپس idx;
}

/*
 * Returns the index of object member variable "name" in the class "cl".
 * Returns -1, if the variable is not found.
 * If "namelen" is zero, then it is assumed that "name" is NUL terminated.
 */
    ساکن صحیح
object_member_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحیح idx;
    object_member_lookup(cl, name, namelen, &idx);
    واپس idx;
}

/*
 * Returns a pointer to the object member variable "name" in the class "cl".
 * Returns NULL if the variable is not found.
 * The object member variable index is set in "idx".
 */
    ocmember_T *
object_member_lookup(class_T *cl, char_u *name, size_t namelen, صحیح *idx)
{
    ocmember_T	*ret_m = تہی;
    صحیح		ret_idx = -1;
    برائے (صحیح i = 0; i < cl->class_obj_member_گنتی; ++i)
    {
	ocmember_T  *m = &cl->class_obj_members[i];
	اگر (namelen)
	{
	    اگر (namelen == m->ocm_name.لمبائی
		&& STRNCMP(name, m->ocm_name.string, namelen) == 0)
	    {
		ret_m = m;
		ret_idx = i;
		توڑو;
	    }
	}
	ورنہ اگر (STRCMP(name, m->ocm_name.string) == 0)
	{
	    ret_m = m;
	    ret_idx = i;
	    توڑو;
	}
    }
    اگر (idx != تہی)
	*idx = ret_idx;
    واپس ret_m;
}

/*
 * Returns a pointer to the object method "name" in class "cl".
 * Returns NULL if the method is not found.
 * The object method index is set in "idx".
 */
    ساکن ufunc_T *
object_طریقہ_lookup(class_T *cl, char_u *name, size_t namelen, صحیح *idx)
{
    ufunc_T	*ret_fp = تہی;
    صحیح		ret_idx = -1;
    برائے (صحیح i = 0; i < cl->class_obj_طریقہ_گنتی; ++i)
    {
	ufunc_T *fp = cl->class_obj_methods[i];
	// Use a separate pointer to avoid that ASAN complains about
	// uf_name[] only being 4 characters.
	char_u *ufname = (char_u *)fp->uf_name;
	اگر (STRNCMP(name, ufname, namelen) == 0 && ufname[namelen] == NUL)
	{
	    ret_fp = fp;
	    ret_idx = i;
	    توڑو;
	}
    }
    اگر (idx != تہی)
	*idx = ret_idx;
    واپس ret_fp;
}

/*
 * Returns the index of object method "name" in the class "cl".
 * Returns -1, if the method is not found.
 */
    صحیح
object_طریقہ_idx(class_T *cl, char_u *name, size_t namelen)
{
    صحیح idx;
    object_طریقہ_lookup(cl, name, namelen, &idx);
    واپس idx;
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
	واپس class_member_lookup(cl, name, namelen, idx);

    واپس object_member_lookup(cl, name, namelen, idx);
}

/*
 * Find the class that defines the named member. Look up the hierarchy
 * starting at "cl".
 *
 * Return the class that defines the member "name", else NULL.
 * Fill in "p_m", if specified, for ocmember_T in found class.
 */
// NOTE: if useful for something could also indirectly return vartype and idx.
    ساکن class_T *
class_defining_member(class_T *cl, char_u *name, size_t len, ocmember_T **p_m)
{
    class_T	*cl_ملا = تہی;
    vartype_T	vartype = VAR_UNKNOWN;
    ocmember_T	*m_ملا = تہی;

    len = len != 0 ? len : STRLEN(name);

    // Loop assumes if member is not defined in "cl", then it is not
    // defined in any super class; the last class where it's found is the
    // class where it is defined. Once the vartype is found, the other
    // type is no longer checked.
    برائے (class_T *super = cl; super != تہی; super = super->class_extends)
    {
	class_T		*cl_tmp = تہی;
	ocmember_T	*m = تہی;
	اگر (vartype == VAR_UNKNOWN || vartype == VAR_OBJECT)
	{
	    اگر ((m = object_member_lookup(super, name, len, تہی)) != تہی)
	    {
		cl_tmp = super;
		vartype = VAR_OBJECT;
	    }
	}
	اگر (vartype == VAR_UNKNOWN || vartype == VAR_CLASS)
	{
	    اگر (( m = class_member_lookup(super, name, len, تہی)) != تہی)
	    {
		cl_tmp = super;
		vartype = VAR_CLASS;
	    }
	}
	اگر (cl_tmp == تہی)
	    توڑو;  // member is not in this or any super class.
	cl_ملا = cl_tmp;
	m_ملا = m;
    }
    اگر (p_m != تہی)
	*p_m = m_ملا;
    واپس cl_ملا;
}

/*
 * Lookup a class or object method by name.  If v_type is VAR_CLASS, then
 * lookup a class method and if it is VAR_OBJECT, then lookup a object method.
 *
 * Returns a pointer to the method structure if variable is found.
 * Otherwise returns NULL.  The method variable index is set in "*idx".
 */
    ufunc_T *
طریقہ_lookup(
    class_T	*cl,
    vartype_T	v_type,
    char_u	*name,
    size_t	namelen,
    صحیح		*idx)
{
    اگر (v_type == VAR_CLASS)
	واپس class_طریقہ_lookup(cl, name, namelen, idx);
    ورنہ
	واپس object_طریقہ_lookup(cl, name, namelen, idx);
}

/*
 * Return TRUE if current context "cctx_arg" is inside class "cl".
 * Return FALSE if not.
 */
    صحیح
inside_class(cctx_T *cctx_arg, class_T *cl)
{
    برائے (cctx_T *cctx = cctx_arg; cctx != تہی; cctx = cctx->ctx_outer)
	اگر (cctx->ctx_ufunc != تہی
			&& class_instance_کا(cctx->ctx_ufunc->uf_class, cl))
	    واپس TRUE;
    واپس FALSE;
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
	semsg(_(e_cannot_change_readonly_variable_str_اندر_class_str),
		m->ocm_name.string, cl->class_name.string);
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Lock all the constant object variables.  Called after creating and
 * initializing a new object.
 */
    خالی
obj_تالا_const_vars(object_T *obj)
{
    برائے (صحیح i = 0; i < obj->obj_class->class_obj_member_گنتی; i++)
    {
	ocmember_T *ocm = &obj->obj_class->class_obj_members[i];
	اگر (ocm->ocm_flags & OCMFLAG_CONST)
	{
	    typval_T *mtv = ((typval_T *)(obj + 1)) + i;
	    شے_تالا(mtv, DICT_MAXNEST, TRUE, TRUE);
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

    اگر (cl == تہی)
	واپس تہی;

    sz = حجم(object_T) + cl->class_obj_member_گنتی * حجم(typval_T);
    obj = alloc_clear(sz);
    اگر (obj == تہی)
	واپس تہی;

    obj->obj_class = cl;
    ++cl->class_refcount;
    obj->obj_refcount = 1;
    object_created(obj);

    // When creating an enum value object, initialize the name and ordinal
    // object variables.
    اگر (IS_ENUM(cl))
	enum_سیٹ_اندرونی_obj_vars(cl, obj);

    واپس obj;
}

/*
 * Make a copy of an object.
 */
    خالی
copy_object(typval_T *from, typval_T *مقصد)
{
    اگر (from->vval.v_object == تہی)
	مقصد->vval.v_object = تہی;
    ورنہ
    {
	مقصد->vval.v_object = from->vval.v_object;
	++مقصد->vval.v_object->obj_refcount;
    }
}

/*
 * Make a copy of a class.
 */
    خالی
copy_class(typval_T *from, typval_T *مقصد)
{
    اگر (from->vval.v_class == تہی)
	مقصد->vval.v_class = تہی;
    ورنہ
    {
	مقصد->vval.v_class = from->vval.v_class;
	++مقصد->vval.v_class->class_refcount;
    }
}

/*
 * Free the class "cl" and its contents.
 */
    ساکن خالی
class_free(class_T *cl)
{
    // Freeing what the class contains may recursively come back here.
    // Clear "class_name" first, if it is NULL the class does not need to
    // be freed.
    VIM_CLEAR_STRING(cl->class_name);

    class_unref(cl->class_extends);

    برائے (صحیح i = 0; i < cl->class_interface_گنتی; ++i)
    {
	vim_free(((char_u **)cl->class_interfaces)[i]);
	اگر (cl->class_interfaces_cl[i] != تہی)
	    class_unref(cl->class_interfaces_cl[i]);
    }
    vim_free(cl->class_interfaces);
    vim_free(cl->class_interfaces_cl);

    itf2class_T *اگلا;
    برائے (itf2class_T *i2c = cl->class_itf2class; i2c != تہی; i2c = اگلا)
    {
	اگلا = i2c->i2c_اگلا;
	vim_free(i2c);
    }

    برائے (صحیح i = 0; i < cl->class_class_member_گنتی; ++i)
    {
	ocmember_T *m = &cl->class_class_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
	اگر (cl->class_members_tv != تہی)
	    clear_tv(&cl->class_members_tv[i]);
    }
    vim_free(cl->class_class_members);
    vim_free(cl->class_members_tv);

    برائے (صحیح i = 0; i < cl->class_obj_member_گنتی; ++i)
    {
	ocmember_T *m = &cl->class_obj_members[i];
	VIM_CLEAR_STRING(m->ocm_name);
	vim_free(m->ocm_init);
    }
    vim_free(cl->class_obj_members);

    برائے (صحیح i = 0; i < cl->class_class_function_گنتی; ++i)
    {
	ufunc_T *uf = cl->class_class_functions[i];
	// For an enum class, the constructor function names are cleared.  Set
	// the name back, so that clearing the function will work properly.
	اگر (IS_ENUM(cl) && IS_CONSTRUCTOR_طریقہ(uf) && *uf->uf_name == NUL)
	    STRCPY(uf->uf_name, "new");
	func_clear_free(uf, FALSE);
    }
    vim_free(cl->class_class_functions);

    برائے (صحیح i = 0; i < cl->class_obj_طریقہ_گنتی; ++i)
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
    ساکن صحیح
class_get_selfrefs(class_T *cl)
{
    صحیح		self_refs = 0;
    typval_T	*tv;

    برائے (صحیح i = 0; i < cl->class_class_member_گنتی; ++i)
    {
	tv = &cl->class_members_tv[i];
	اگر (tv->v_type == VAR_OBJECT && tv->vval.v_object != تہی
		&& tv->vval.v_object->obj_class == cl
		&& (tv->vval.v_object->obj_refcount == 1
		    || (IS_ENUM(cl) && tv->vval.v_object->obj_refcount == 2)))
	    self_refs++;
    }

    واپس self_refs;
}

/*
 * Returns TRUE if enum "cl" can be freed.  An enum can be freed, if the enum
 * values are no longer referenced and the "values" class member is also not
 * referenced.
 */
    ساکن صحیح
can_free_enum(class_T *cl)
{
    برائے (صحیح i = 0; i < cl->class_class_member_گنتی; ++i)
    {
	typval_T	*tv = &cl->class_members_tv[i];
	ocmember_T	*ocm = &cl->class_class_members[i];

	اگر (tv->v_type != VAR_OBJECT && tv->v_type != VAR_LIST)
	    // In an enum, the first set of class members are the enum values.
	    // Followed by the "values" member which is a List of enum values.
	    // If all of those members are no longer referenced, then the enum
	    // may be freed.
	    واپس TRUE;

	اگر (tv->v_type == VAR_LIST
		&& tv->vval.v_list->lv_type->tt_member->tt_type == VAR_OBJECT
		&& tv->vval.v_list->lv_type->tt_member->tt_class == cl
		&& STRCMP(ocm->ocm_name.string, "values") == 0)
	{
	    // "values" class member is referenced outside
	    اگر (tv->vval.v_list->lv_refcount > 1)
		واپس FALSE;
	    توڑو;
	}

	اگر (tv->vval.v_object->obj_refcount > 2)
	    // enum value is referenced outside
	    واپس FALSE;
    }

    واپس TRUE;
}

/*
 * Returns TRUE if class "cl" has no references outside of the references from
 * it's own class members and can be freed.  If it is an enum, then checks
 * whether the enum values and the "values" members are referenced elsewhere.
 */
    ساکن صحیح
can_free_class(class_T *cl)
{
    صحیح		can_free = FALSE;
    صحیح		self_refs = 0;

    اگر (IS_ENUM(cl) && !can_free_enum(cl))
	واپس FALSE;

    اگر (cl->class_refcount > 0)
	self_refs = class_get_selfrefs(cl);

    // If the class is no longer referenced or only referenced from it's own
    // class member, then it can be freed.
    اگر (cl->class_refcount <= 0 || cl->class_refcount == self_refs)
	can_free = TRUE;

    واپس can_free;
}

/*
 * Unreference a class.  Free it when the reference count goes down to zero.
 */
    خالی
class_unref(class_T *cl)
{
    اگر (cl == تہی)
	واپس;

    --cl->class_refcount;

    اگر (cl->class_name.string == تہی)
	واپس;

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

    برائے (class_T *cl = پہلا_class; cl != تہی; cl = اگلا_nonref_class)
    {
	اگلا_nonref_class = cl->class_اگلا_مستعمل;
	اگر ((cl->class_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب))
	{
	    // Free the class and items it contains.
	    class_free(cl);
	    did_free = TRUE;
	}
    }

    اگلا_nonref_class = تہی;
    واپس did_free;
}

    صحیح
سیٹ_ref_اندر_classes(صحیح copyID)
{
    برائے (class_T *cl = پہلا_class; cl != تہی; cl = cl->class_اگلا_مستعمل)
	سیٹ_ref_اندر_شے_class(cl, copyID, تہی, تہی, تہی);

    واپس FALSE;
}

ساکن object_T *پہلا_object = تہی;

/*
 * Call this function when an object has been created.  It will be added to the
 * list headed by "first_object".
 */
    خالی
object_created(object_T *obj)
{
    اگر (پہلا_object != تہی)
    {
	obj->obj_اگلا_مستعمل = پہلا_object;
	پہلا_object->obj_پچھلا_مستعمل = obj;
    }
    پہلا_object = obj;
}

/*
 * Call this function when an object has been cleared and is about to be freed.
 * It is removed from the list headed by "first_object".
 */
    ساکن خالی
object_cleared(object_T *obj)
{
    اگر (obj->obj_اگلا_مستعمل != تہی)
	obj->obj_اگلا_مستعمل->obj_پچھلا_مستعمل = obj->obj_پچھلا_مستعمل;
    اگر (obj->obj_پچھلا_مستعمل != تہی)
	obj->obj_پچھلا_مستعمل->obj_اگلا_مستعمل = obj->obj_اگلا_مستعمل;
    ورنہ اگر (پہلا_object == obj)
	پہلا_object = obj->obj_اگلا_مستعمل;
}

/*
 * Free the contents of an object ignoring the reference count.
 */
    ساکن خالی
object_free_contents(object_T *obj)
{
    class_T *cl = obj->obj_class;

    اگر (!cl)
	واپس;

    // Avoid a recursive call, it can happen if "obj" has a circular reference.
    obj->obj_refcount = زیادہ_سے_زیادہ_عدد;

    // the member values are just after the object structure
    typval_T *tv = (typval_T *)(obj + 1);
    برائے (صحیح i = 0; i < cl->class_obj_member_گنتی; ++i)
	clear_tv(tv + i);
}

    ساکن خالی
object_free_object(object_T *obj)
{
    class_T *cl = obj->obj_class;

    اگر (!cl)
	واپس;

    // Remove from the list headed by "first_object".
    object_cleared(obj);

    vim_free(obj);
    class_unref(cl);
}

    ساکن خالی
object_free(object_T *obj)
{
    اگر (اندر_free_unref_اشیاء)
	واپس;

    object_free_contents(obj);
    object_free_object(obj);
}

/*
 * Unreference an object.
 */
    خالی
object_unref(object_T *obj)
{
    اگر (obj != تہی && --obj->obj_refcount <= 0)
	object_free(obj);
}

/*
 * Go through the list of all objects and free items without "copyID".
 */
    صحیح
object_free_nonref(صحیح copyID)
{
    صحیح		did_free = FALSE;

    برائے (object_T *obj = پہلا_object; obj != تہی; obj = obj->obj_اگلا_مستعمل)
    {
	اگر ((obj->obj_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب))
	{
	    // Free the object contents.  Object itself will be freed later.
	    object_free_contents(obj);
	    did_free = TRUE;
	}
    }

    واپس did_free;
}

    خالی
object_free_اشیاء(صحیح copyID)
{
    object_T	*obj_اگلا;

    برائے (object_T *obj = پہلا_object; obj != تہی; obj = obj_اگلا)
    {
	obj_اگلا = obj->obj_اگلا_مستعمل;
	اگر ((obj->obj_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب))
	    object_free_object(obj);
    }
}

/*
 * Output message which takes a variable name and the class that defines it.
 * "cl" is that class where the name was found. Search "cl"'s hierarchy to
 * find the defining class.
 */
    خالی
emsg_var_cl_define(حرف *پیغام, char_u *name, size_t len, class_T *cl)
{
    ocmember_T	*m;
    class_T	*cl_def = class_defining_member(cl, name, len, &m);
    اگر (cl_def != تہی)
	semsg(_(پیغام), m->ocm_name.string, cl_def->class_name.string);
    ورنہ
	emsg(_(e_اندرونی_error_please_report_a_bug));
}

/*
 * Echo a class or object method not found message.
 */
    خالی
طریقہ_not_ملا_پیغام(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *طریقہ_name = vim_strnsave(name, len);
    اگر ((v_type == VAR_OBJECT)
	    && (class_طریقہ_idx(cl, name, len) >= 0))
    {
	// If this is a class method, then give a different error
	اگر (*name == '_')
	    semsg(_(e_cannot_access_protected_طریقہ_str), طریقہ_name);
	ورنہ
	    semsg(_(e_class_طریقہ_str_accessible_only_using_class_str),
		    طریقہ_name, cl->class_name.string);
    }
    ورنہ اگر ((v_type == VAR_CLASS)
	    && (object_طریقہ_idx(cl, name, len) >= 0))
    {
	// If this is an object method, then give a different error
	اگر (*name == '_')
	    semsg(_(e_cannot_access_protected_طریقہ_str), طریقہ_name);
	ورنہ
	    semsg(_(e_object_طریقہ_str_accessible_only_using_object_str),
		    طریقہ_name, cl->class_name.string);
    }
    ورنہ
	semsg(_(e_طریقہ_not_ملا_on_class_str_str), طریقہ_name,
		cl->class_name.string);
    vim_free(طریقہ_name);
}

/*
 * Echo a class or object member not found message.
 */
    خالی
member_not_ملا_پیغام(class_T *cl, vartype_T v_type, char_u *name, size_t len)
{
    char_u *varname = len ? vim_strnsave(name, len) : vim_strsave(name);

    اگر (v_type == VAR_OBJECT)
    {
	اگر (class_member_idx(cl, name, len) >= 0)
	    semsg(_(e_class_variable_str_accessible_only_using_class_str),
		    varname, cl->class_name.string);
	ورنہ
	    semsg(_(e_variable_not_ملا_on_object_str_str), varname,
		    cl->class_name.string);
    }
    ورنہ
    {
	اگر (object_member_idx(cl, name, len) >= 0)
	    semsg(_(e_object_variable_str_accessible_only_using_object_str),
		    varname, cl->class_name.string);
	ورنہ
	{
	    اگر (IS_ENUM(cl))
		semsg(_(e_enum_قدر_str_not_ملا_اندر_enum_str),
			varname, cl->class_name.string);
	    ورنہ
		semsg(_(e_class_variable_str_not_ملا_اندر_class_str),
			varname, cl->class_name.string);
	}
    }
    vim_free(varname);
}

/*
 * Compile all the class and object methods in "cl".
 */
    خالی
defcompile_class(class_T *cl)
{
    برائے (صحیح loop = 1; loop <= 2; ++loop)
    {
	صحیح func_گنتی = loop == 1 ? cl->class_class_function_گنتی
						: cl->class_obj_طریقہ_گنتی;
	برائے (صحیح i = 0; i < func_گنتی; i++)
	{
	    ufunc_T *ufunc = loop == 1 ? cl->class_class_functions[i]
						: cl->class_obj_methods[i];
	    // Don't compile abstract methods
	    اگر (!IS_ABSTRACT_طریقہ(ufunc))
		defcompile_function(ufunc, cl);
	}
    }
}

/*
 * Compile all the classes defined in the current script
 */
    خالی
defcompile_classes_اندر_script(خالی)
{
    برائے (class_T *cl = پہلا_class; cl != تہی; cl = cl->class_اگلا_مستعمل)
    {
	اگر (eval_variable(cl->class_name.string, 0, 0, تہی, تہی,
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

    اگر (eval_variable(name, 0, 0, rettv, تہی, EVAL_VAR_NOAUTOLOAD |
						EVAL_VAR_NO_FUNC) != FAIL)
	واپس rettv->v_type == VAR_CLASS;
    واپس FALSE;
}

/*
 * Calls the object builtin method "name" with arguments "argv".  The value
 * returned by the builtin method is in "rettv".  Returns OK or FAIL.
 */
    ساکن صحیح
object_call_builtin_طریقہ(
    object_T		*obj,
    class_builtin_T	builtin_طریقہ,
    صحیح			argc,
    typval_T		*argv,
    typval_T		*rettv)
{
    ufunc_T *uf;
    صحیح	    midx;

    اگر (obj == تہی)
	واپس FAIL;

    uf = class_get_builtin_طریقہ(obj->obj_class, builtin_طریقہ, &midx);
    اگر (uf == تہی)
	واپس FAIL;

    funccall_T  *fc = create_funccal(uf, rettv);
    صحیح		r;

    اگر (fc == تہی)
	واپس FAIL;

    ++obj->obj_refcount;

    r = call_def_function(uf, argc, argv, 0, تہی, obj, fc, rettv);

    remove_funccal();

    واپس r;
}

/*
 * Calls the object "empty()" method and returns the method return value.  In
 * case of an error, returns TRUE.
 */
    صحیح
object_خالی_خانے(object_T *obj)
{
    typval_T	rettv;

    اگر (object_call_builtin_طریقہ(obj, CLASS_BUILTIN_خالی_خانے, 0, تہی, &rettv)
								== FAIL)
	واپس TRUE;

    واپس tv_get_bool(&rettv);
}

/*
 * Use the object "len()" method to get an object length.  Returns 0 if the
 * method is not found or there is an error.
 */
    صحیح
object_len(object_T *obj)
{
    typval_T	rettv;

    اگر (object_call_builtin_طریقہ(obj, CLASS_BUILTIN_LEN, 0, تہی, &rettv)
								== FAIL)
	واپس 0;

    واپس tv_مقصد_نمبر(&rettv);
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
	واپس TRUE;
    اگر (o1 == تہی || o2 == تہی)
	واپس FALSE;

    cl1 = o1->obj_class;
    cl2 = o2->obj_class;

    اگر (cl1 != cl2 || cl1 == تہی || cl2 == تہی)
	واپس FALSE;

    برائے (صحیح i = 0; i < cl1->class_obj_member_گنتی; ++i)
	اگر (!tv_equal((typval_T *)(o1 + 1) + i, (typval_T *)(o2 + 1) + i, ic))
	    واپس FALSE;

    واپس TRUE;
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
    صحیح		مرکب_val)
{
    typval_T	rettv;

    اگر (object_call_builtin_طریقہ(obj, CLASS_BUILTIN_STRING, 0, تہی, &rettv)
								== OK
					&& rettv.vval.v_string != تہی)
	واپس rettv.vval.v_string;

    صحیح		ok = OK;
    class_T	*cl = obj->obj_class;
    garray_T	ga;
    ga_init2(&ga, 1, 50);

    اگر (cl != تہی && IS_ENUM(cl))
    {
	GA_CONCAT_LITERAL(&ga, "enum ");
	ga_concat_len(&ga, cl->class_name.string, cl->class_name.لمبائی);
	char_u *enum_name = ((typval_T *)(obj + 1))->vval.v_string;
	GA_CONCAT_LITERAL(&ga, ".");
	ga_concat(&ga, enum_name);
    }
    ورنہ
    {
	GA_CONCAT_LITERAL(&ga, "object of ");
	اگر (cl == تہی)
	    GA_CONCAT_LITERAL(&ga, "[unknown]");
	ورنہ
	    ga_concat_len(&ga, cl->class_name.string, cl->class_name.لمبائی);
    }
    اگر (cl != تہی)
    {
	GA_CONCAT_LITERAL(&ga, " {");
	برائے (صحیح i = 0; i < cl->class_obj_member_گنتی; ++i)
	{
	    اگر (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ocmember_T *m = &cl->class_obj_members[i];
	    ga_concat_len(&ga, m->ocm_name.string, m->ocm_name.لمبائی);
	    GA_CONCAT_LITERAL(&ga, ": ");
	    char_u *tf = تہی;
	    char_u *s = echo_string_core((typval_T *)(obj + 1) + i,
					 &tf, numbuf, copyID, echo_style,
					 restore_copyID, مرکب_val);
	    اگر (s != تہی)
		ga_concat(&ga, s);
	    vim_free(tf);
	    اگر (s == تہی || did_echo_string_emsg)
	    {
		ok = FAIL;
		توڑو;
	    }
	    سطر_breakcheck();
	}
	GA_CONCAT_LITERAL(&ga, "}");
    }
    اگر (ok == FAIL)
    {
	vim_free(ga.ga_ڈیٹا);
	واپس تہی;
    }
    ga_append(&ga, NUL);
    واپس (char_u *)ga.ga_ڈیٹا;
}

/*
 * Return TRUE when the class "cl", its base class or one of the implemented
 * interfaces matches the class "other_cl".
 */
    صحیح
class_instance_کا(class_T *cl, class_T *other_cl)
{
    اگر (cl == other_cl)
	واپس TRUE;

    // Recursively check the base classes.
    برائے (; cl != تہی; cl = cl->class_extends)
    {
	اگر (cl == other_cl)
	    واپس TRUE;
	// Check the implemented interfaces and the super interfaces
	برائے (صحیح i = cl->class_interface_گنتی - 1; i >= 0; --i)
	{
	    class_T	*intf = cl->class_interfaces_cl[i];
	    جب (intf != تہی)
	    {
		اگر (intf == other_cl)
		    واپس TRUE;
		// check the super interfaces
		intf = intf->class_extends;
	    }
	}
    }

    واپس FALSE;
}

/*
 * "instanceof(object, classinfo, ...)" function
 */
    خالی
f_instanceof(typval_T *argvars, typval_T *rettv)
{
    typval_T	*object_tv = &argvars[0];
    typval_T	*classinfo_tv = &argvars[1];
    class_T	*c;

    rettv->vval.v_نمبر = VVAL_FALSE;

    اگر (check_for_object_arg(argvars, 0) == FAIL
	    || check_for_class_or_typealias_دلائل(argvars, 1) == FAIL)
	واپس;

    اگر (object_tv->vval.v_object == تہی)
    {
	اگر (classinfo_tv->vval.v_class == تہی)
	    // consider null_object as an instance of null_class
	    rettv->vval.v_نمبر = VVAL_TRUE;
	واپس;
    }

    برائے (; classinfo_tv->v_type != VAR_UNKNOWN; ++classinfo_tv)
    {
	اگر (classinfo_tv->v_type == VAR_TYPEALIAS)
	    c = classinfo_tv->vval.v_typealias->ta_type->tt_class;
	ورنہ
	    c = classinfo_tv->vval.v_class;

	اگر (class_instance_کا(object_tv->vval.v_object->obj_class, c))
	{
	    rettv->vval.v_نمبر = VVAL_TRUE;
	    واپس;
	}
    }
}

#اگر_ختم // FEAT_EVAL
