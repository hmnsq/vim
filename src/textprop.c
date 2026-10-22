/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Text properties implementation.  See ":help text-properties".
 */

#ضمن "vim.h"

#إذا defined(FEAT_PROP_POPUP)

ساكن فراغ um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    ساكن فراغ
um_free_vtext(textprop_T *props, صحيح شمېر)
{
    لكل (صحيح i = 0; i < شمېر; ++i)
	إذا (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_متن);
}

/*
 * Free memory used by an unpacked memline.
 */
    ساكن فراغ
um_free(unpacked_memline_T *um)
{
    إذا (um->detached)
    {
	VIM_CLEAR(um->متن);
	um_free_vtext(um->props, um->prop_شمېر);
    }
    VIM_CLEAR(um->props);
    um->prop_اندازه = 0;
    um->prop_شمېر = 0;
    um->lnum = 0;
    um->detached = false;
}

/*
 * Initialize an unpacked memline for the given buffer.
 * No line is loaded yet; use um_goto_line() to load one.
 */
    unpacked_memline_T
um_open(buf_T *buf)
{
    unpacked_memline_T um;

    CLEAR_ډګر(um);
    um.buf = buf;
    أرجع um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_کرښه(unpacked_memline_T *um, linenr_T lnum, صحيح extra_props)
{
    char_u	*کرښه;
    size_t	textlen;
    size_t	propdata_len;
    صحيح		proplen;

    إذا (um->buf == عدم)
	أرجع false;

    // Store changes to the current line if any.
    إذا (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    إذا (lnum == 0)
	أرجع true;	// just unload

    کرښه = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_کرښه_len - textlen;

    um->lnum = lnum;
    um->متن_اندازه_2 = (colnr_T)textlen;
    um->متن = کرښه;

    إذا (propdata_len == 0)
	أرجع true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    إذا (propdata_len < PROP_شمېر_اندازه + حجم(textprop_T))
    {
	emsg(e_متن_property_info_corrupted);
	um->buf = عدم;
	أرجع false;
    }

    uint16_t    prop_شمېر;
    char_u	    *شمېر_ptr = کرښه + textlen;
    char_u	    *props_پیل;

    mch_memmove(&prop_شمېر, شمېر_ptr, PROP_شمېر_اندازه);
    إذا (!متن_prop_شمېر_valid(prop_شمېر, propdata_len))
    {
	emsg(e_متن_property_info_corrupted);
	um->buf = عدم;
	أرجع false;
    }
    proplen = (صحيح)prop_شمېر;
    props_پیل = شمېر_ptr + PROP_شمېر_اندازه;

    إذا (!متن_prop_vtext_valid(props_پیل, proplen, propdata_len))
    {
	emsg(e_متن_property_info_corrupted);
	um->buf = عدم;
	أرجع false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    إذا (um->props == عدم)
    {
	um->buf = عدم;
	أرجع false;
    }
    um->prop_اندازه = proplen + extra_props;
    um->prop_شمېر = proplen;
    mch_memmove(um->props, props_پیل, proplen * حجم(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    لكل (صحيح i = 0; i < proplen; ++i)
    {
	إذا (um->props[i].tp_id < 0 && um->props[i].u.tp_متن_دننه_واټن > 0)
	{
	    um->props[i].u.tp_متن = شمېر_ptr + um->props[i].u.tp_متن_دننه_واټن;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	وإلا
	    um->props[i].u.tp_متن = عدم;
    }

    أرجع true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, صحيح extra_props)
{
    unpacked_memline_T um = um_open(buf);

    إذا (!um_goto_کرښه(&um, lnum, extra_props))
	um.buf = عدم;
    أرجع um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    ساكن bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    إذا (um->detached || um->buf == عدم)
	أرجع um->detached;

    newtext = vim_strnsave(um->متن, um->متن_اندازه_2 - 1);
    إذا (newtext == عدم)
    {
	um->buf = عدم;
	أرجع false;
    }
    um->متن = newtext;

    لكل (صحيح i = 0; i < um->prop_شمېر; ++i)
    {
	إذا (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_متن);

	    إذا (copy == عدم)
	    {
		لكل (صحيح j = 0; j < i; ++j)
		    إذا (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_متن);
		VIM_CLEAR(um->متن);
		um->buf = عدم;
		أرجع false;
	    }
	    um->props[i].u.tp_متن = copy;
	}
    }

    um->detached = true;
    أرجع true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_سېټ_متن(unpacked_memline_T *um, char_u *متن)
{
    إذا (um->buf == عدم)
	أرجع false;
    إذا (um->detached)
	vim_free(um->متن);
    um->متن = متن;
    um->متن_اندازه_2 = (colnr_T)STRLEN(متن) + 1;
    um->متن_بدل_شوی = true;
    um->detached = true;
    أرجع true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    ساكن bool
um_grow_props(unpacked_memline_T *um, صحيح needed)
{
    إذا (um->prop_شمېر + needed <= um->prop_اندازه)
	أرجع true;

    صحيح	    new_اندازه = um->prop_شمېر + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_اندازه * حجم(textprop_T));
    إذا (newprops == عدم)
	أرجع false;
    um->props = newprops;
    um->prop_اندازه = new_اندازه;

    أرجع true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    ساكن فراغ
um_جمع_کړه_prop(unpacked_memline_T *um, textprop_T *prop)
{
    إذا (um->buf == عدم)
	أرجع;
    إذا (!um->detached && !um_detach(um))
	أرجع;
    إذا (!um_grow_props(um, 1))
    {
	um->buf = عدم;
	أرجع;
    }
    um->props[um->prop_شمېر++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    فراغ
um_سرچپه_props(unpacked_memline_T *um)
{
    لكل (صحيح i = 0, j = um->prop_شمېر - 1; i < j; ++i, --j)
    {
	textprop_T tmp = um->props[i];
	um->props[i] = um->props[j];
	um->props[j] = tmp;
    }
}

/*
 * Open an unpacked memline at "lnum" with no properties copied,
 * but with space for "prop_count" properties.  The umemline is
 * immediately in the DETACHED state.
 */
    unpacked_memline_T
um_open_at_no_props(buf_T *buf, linenr_T lnum, صحيح prop_شمېر)
{
    unpacked_memline_T um;

    CLEAR_ډګر(um);
    um.buf = buf;

    إذا (lnum < 1 || lnum > buf->b_ml.ml_کرښه_شمېر)
    {
	um.buf = عدم;
	أرجع um;
    }

    char_u *کرښه = ml_get_buf(buf, lnum, FALSE);
    char_u *متن_copy = vim_strsave(کرښه);

    إذا (متن_copy == عدم)
    {
	um.buf = عدم;
	أرجع um;
    }
    um.lnum = lnum;
    um.متن = متن_copy;
    um.متن_اندازه_2 = (colnr_T)STRLEN(متن_copy) + 1;
    um.detached = true;

    إذا (prop_شمېر > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_شمېر);
	إذا (um.props == عدم)
	{
	    vim_free(um.متن);
	    um.buf = عدم;
	    أرجع um;
	}
	um.prop_اندازه = prop_شمېر;
    }

    أرجع um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    ساكن فراغ
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					صحيح tp_id, صحيح tp_type, صحيح flag)
{
    unpacked_memline_T	neighbor;

    إذا (lnum < 1 || lnum > buf->b_ml.ml_کرښه_شمېر)
	أرجع;

    neighbor = um_open_at(buf, lnum, 0);
    إذا (neighbor.buf == عدم)
	أرجع;

    لكل (صحيح i = 0; i < neighbor.prop_شمېر; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	إذا (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    إذا (!neighbor.detached && !um_detach(&neighbor))
		اكسر;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    اكسر;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    فراغ
um_delete_prop(unpacked_memline_T *um, صحيح شاخص)
{
    textprop_T *prop;

    إذا (um->buf == عدم || شاخص < 0 || شاخص >= um->prop_شمېر)
	أرجع;
    إذا (!um->detached && !um_detach(um))
	أرجع;

    prop = &um->props[شاخص];

    // Adjust continuation flags on neighboring lines before deleting.
    إذا (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_بل);
    إذا (prop->tp_flags & TP_FLAG_CONT_بل)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    إذا (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	VIM_CLEAR(prop->u.tp_متن);
    prop->tp_flags |= TP_FLAG_DELETED;
}

/*
 * Pack an unpacked memline into a newly allocated buffer.
 * Packed format: [text][NUL][textprop_T...][vtext strings...]
 * Returns the packed buffer, or NULL on failure.
 * "packed_len" is set to the total length.
 *
 * NOTE: Currently packs in the OLD format (no inline vtext) because
 * the read side has not been converted yet.
 */
    ساكن char_u *
um_pack(unpacked_memline_T *um, صحيح *packed_len)
{
    uint16_t	ژوندی_شمېر = 0;
    صحيح		vtext_اندازه = 0;
    صحيح		ټول_len;
    char_u	*buf;
    char_u	*شمېر_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    لكل (صحيح i = 0; i < um->prop_شمېر; ++i)
    {
	textprop_T *prop = &um->props[i];

	إذا (prop->tp_flags & TP_FLAG_DELETED)
	    استمر;
	++ژوندی_شمېر;
	إذا (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_اندازه += prop->tp_len + 1;
    }

    إذا (ژوندی_شمېر == 0)
    {
	// No properties: just text, no prop_count header.
	ټول_len = um->متن_اندازه_2;
	buf = alloc(ټول_len);
	إذا (buf == عدم)
	    أرجع عدم;
	*packed_len = ټول_len;
	mch_memmove(buf, um->متن, um->متن_اندازه_2);
	أرجع buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    ټول_len = um->متن_اندازه_2 + (صحيح)PROP_شمېر_اندازه
		+ ژوندی_شمېر * (صحيح)حجم(textprop_T) + vtext_اندازه;
    buf = alloc(ټول_len);
    إذا (buf == عدم)
	أرجع عدم;
    *packed_len = ټول_len;

    mch_memmove(buf, um->متن, um->متن_اندازه_2);

    شمېر_dest = buf + um->متن_اندازه_2;
    mch_memmove(شمېر_dest, &ژوندی_شمېر, PROP_شمېر_اندازه);

    prop_dest = شمېر_dest + PROP_شمېر_اندازه;
    vtext_dest = prop_dest + ژوندی_شمېر * حجم(textprop_T);

    لكل (صحيح i = 0; i < um->prop_شمېر; ++i)
    {
	textprop_T prop = um->props[i];

	إذا (prop.tp_flags & TP_FLAG_DELETED)
	    استمر;

	إذا (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    صحيح copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_متن, copysize);
	    prop.u.tp_متن_دننه_واټن = (colnr_T)(vtext_dest - شمېر_dest);
	    vtext_dest += copysize;
	}
	وإلا
	    prop.u.tp_متن_دننه_واټن = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, حجم(textprop_T));
	prop_dest += حجم(textprop_T);
    }

    أرجع buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    ساكن فراغ
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    صحيح	    packed_len;

    إذا (!um->detached || um->buf == عدم || um->lnum == 0)
	أرجع;

    packed = um_pack(um, &packed_len);
    إذا (packed == عدم)
    {
	um->buf = عدم;
	أرجع;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    إذا (um->buf->b_ml.ml_کرښه_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    وإلا إذا (!(um->buf->b_ml.ml_flags & ML_کرښه_DIRTY))
	(فراغ)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->متن);
    um_free_vtext(um->props, um->prop_شمېر);

    إذا (um->buf->b_ml.ml_flags & (ML_کرښه_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_کرښه_ptr);
    um->buf->b_ml.ml_کرښه_ptr = packed;
    um->buf->b_ml.ml_کرښه_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_کرښه_DIRTY;

    um->detached = false;
    um->متن = packed;
    um->متن_اندازه_2 = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_کرښه_textlen = um->متن_اندازه_2;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    فراغ
um_close(unpacked_memline_T *um)
{
    إذا (um->buf == عدم)
	أرجع;
    إذا (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = عدم;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    فراغ
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = عدم;
}

/*
 * In a hashtable item "hi_key" points to "pt_name" in a proptype_T.
 * This avoids adding a pointer to the hashtable item.
 * PT2HIKEY() converts a proptype pointer to a hashitem key pointer.
 * HIKEY2PT() converts a hashitem key pointer to a proptype pointer.
 * HI2PT() converts a hashitem pointer to a proptype pointer.
 */
#تعریف_کړه PT2HIKEY(p)  ((p)->pt_name)
#تعریف_کړه HIKEY2PT(p)   ((proptype_T *)((p) - offsetof(proptype_T, pt_name)))
#تعریف_کړه HI2PT(hi)      HIKEY2PT((hi)->hi_کلی)

// The global text property types.
ساكن hashtab_T *global_proptypes = عدم;
ساكن proptype_T **global_proparray = عدم;

// The last used text property type ID.
ساكن صحيح proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    ساكن hashitem_T *
پیدا_کړه_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    إذا (*name == NUL)
	أرجع عدم;
    إذا (buf == عدم)
	ht = global_proptypes;
    وإلا
	ht = buf->b_proptypes;

    إذا (ht == عدم)
	أرجع عدم;
    hi = hash_پیدا_کړه(ht, name);
    إذا (HASHITEM_تشه(hi))
	أرجع عدم;
    أرجع hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    ساكن proptype_T *
پیدا_کړه_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = پیدا_کړه_prop_type_hi(name, buf);

    إذا (hi == عدم)
	أرجع عدم;
    أرجع HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    صحيح
پیدا_کړه_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = پیدا_کړه_prop_type(name, buf);

    إذا (pt == عدم)
	أرجع 0;
    أرجع pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    ساكن proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = پیدا_کړه_prop_type(name, buf);

    إذا (type == عدم)
	type = پیدا_کړه_prop_type(name, عدم);
    إذا (type == عدم)
	semsg(_(e_property_type_str_does_not_exist), name);
    أرجع type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    ساكن صحيح
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    إذا (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	أرجع FAIL;
    }
    إذا (arg->vval.v_dict == عدم)
	أرجع OK;  // NULL dict is like an empty dict
    di = dict_پیدا_کړه(arg->vval.v_dict, (char_u *)"bufnr", -1);
    إذا (di != عدم && (di->di_tv.v_type != VAR_شمېره
					      || di->di_tv.vval.v_شمېره != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	إذا (*buf == عدم)
	    أرجع FAIL;
    }
    أرجع OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    فراغ
f_prop_جمع_کړه(typval_T *argvars, typval_T *rettv)
{
    linenr_T	پیل_lnum;
    colnr_T	پیل_ستون;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	أرجع;

    پیل_lnum = tv_get_شمېره(&argvars[0]);
    پیل_ستون = tv_get_شمېره(&argvars[1]);
    إذا (check_for_dict_arg(argvars, 2) == FAIL)
	أرجع;

    rettv->vval.v_شمېره = prop_جمع_کړه_common(پیل_lnum, پیل_ستون,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    ساكن صحيح
prop_جمع_کړه_one(
	buf_T		*buf,
	char_u		*type_name,
	صحيح		id,
	char_u		*متن_arg,
	صحيح		متن_padding_کیڼ,
	صحيح		متن_flags,
	linenr_T	پیل_lnum,
	linenr_T	پای_ټکی_lnum,
	colnr_T		پیل_ستون,
	colnr_T		پای_ټکی_ستون)
{
    proptype_T	*type;
    linenr_T	lnum;
    صحيح		proplen;
    char_u	*props = عدم;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    صحيح		i;
    textprop_T	tmp_prop;
    char_u	*متن = متن_arg;
    صحيح		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    إذا (type == عدم)
	اذهب theend;

    إذا (پیل_lnum < 1 || پیل_lnum > buf->b_ml.ml_کرښه_شمېر)
    {
	semsg(_(e_invalid_کرښه_شمېره_nr), (اوږد)پیل_lnum);
	اذهب theend;
    }
    إذا (پای_ټکی_lnum < پیل_lnum || پای_ټکی_lnum > buf->b_ml.ml_کرښه_شمېر)
    {
	semsg(_(e_invalid_کرښه_شمېره_nr), (اوږد)پای_ټکی_lnum);
	اذهب theend;
    }

    إذا (buf->b_ml.ml_mfp == عدم)
    {
	emsg(_(e_cannot_جمع_کړه_متن_property_منزل_unloaded_بفر));
	اذهب theend;
    }

    إذا (متن != عدم)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	لكل (p = متن; *p != NUL; MB_PTR_ADV(p))
	    إذا (*p < ' ')
		*p = ' ';
    }

    لكل (lnum = پیل_lnum; lnum <= پای_ټکی_lnum; ++lnum)
    {
	colnr_T ستون;	    // start column use in tp_col
	colnr_T ترتیبول_ستون;   // column where it appears
	اوږد	اوږدتیا;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_متن_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	إذا (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_متن_properties_on_a_single_کرښه));
	    اذهب theend;
	}

	إذا (lnum == پیل_lnum)
	    ستون = پیل_ستون;
	وإلا
	    ستون = 1;
	إذا (ستون - 1 > (colnr_T)textlen && !(ستون == 0 && متن_arg != عدم))
	{
	    semsg(_(e_invalid_ستنه_شمېره_nr), (اوږد)پیل_ستون);
	    اذهب theend;
	}
	ترتیبول_ستون = ستون;

	إذا (lnum == پای_ټکی_lnum)
	    اوږدتیا = پای_ټکی_ستون - ستون;
	وإلا
	    اوږدتیا = (صحيح)textlen - ستون + 1;
	إذا (اوږدتیا > (اوږد)textlen)
	    اوږدتیا = (صحيح)textlen;	// can include the end-of-line
	إذا (اوږدتیا < 0)
	    اوږدتیا = 0;		// zero-width property

	إذا (متن_arg != عدم)
	{
	    اوږدتیا = (اوږد)STRLEN(متن_arg);
	    إذا (ستون == 0)
	    {
		ستون = MAXCOL;	// before or after the line
		إذا ((متن_flags & TP_FLAG_ALIGN_ABOVE) == 0)
		    ترتیبول_ستون = MAXCOL;
	    }
	}

	// Build the new property.
	CLEAR_ډګر(tmp_prop);
	tmp_prop.tp_ستون = ستون;
	tmp_prop.tp_len = اوږدتیا;
	tmp_prop.tp_id = id;
	tmp_prop.tp_type = type->pt_id;
	tmp_prop.tp_flags = متن_flags
			    | (lnum > پیل_lnum ? TP_FLAG_CONT_PREV : 0)
			    | (lnum < پای_ټکی_lnum ? TP_FLAG_CONT_بل : 0)
			    | ((type->pt_flags & PT_FLAG_INS_پیل_INCL)
						     ? TP_FLAG_پیل_INCL : 0);
	tmp_prop.tp_padleft = متن_padding_کیڼ;
	إذا (متن_arg != عدم)
	    tmp_prop.u.tp_متن = متن_arg;

	// Find the index where to insert the new property.
	لكل (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_ستون;

	    mch_memmove(&existing, props + i * حجم(textprop_T),
							   حجم(textprop_T));
	    prop_ستون = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_ستون;
	    إذا (prop_ستون >= ترتیبول_ستون)
		اكسر;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	صحيح		vtext_ټول = 0;
	صحيح		new_کرښه_len;
	char_u	*شمېر_dest;
	char_u	*vtext_dest;
	صحيح		j;

	// Compute total vtext size from existing props.
	لكل (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * حجم(textprop_T),
							حجم(textprop_T));
	    إذا (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_ټول += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	إذا (متن_arg != عدم)
	    vtext_ټول += اوږدتیا + 1;

	new_کرښه_len = (صحيح)textlen + (صحيح)PROP_شمېر_اندازه
			+ new_propcount * (صحيح)حجم(textprop_T)
			+ vtext_ټول;
	newtext = alloc(new_کرښه_len);
	إذا (newtext == عدم)
	    اذهب theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_کرښه_ptr, textlen);

	// Write prop_count.
	شمېر_dest = newtext + textlen;
	mch_memmove(شمېر_dest, &new_propcount, PROP_شمېر_اندازه);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = شمېر_dest + PROP_شمېر_اندازه;
	إذا (i > 0)
	    mch_memmove(newprops, props, حجم(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	إذا (i < proplen)
	    mch_memmove(newprops + (i + 1) * حجم(textprop_T),
					props + i * حجم(textprop_T),
					حجم(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * حجم(textprop_T);
	لكل (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    إذا (j == i)
		استمر;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * حجم(textprop_T),
		    حجم(textprop_T));
	    إذا (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_شمېر_2 = (char_u *)props - PROP_شمېر_اندازه;
		char_u *old_vtext = old_شمېر_2 + ep.u.tp_متن_دننه_واټن;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_متن_دننه_واټن = (colnr_T)(vtext_dest - شمېر_dest);
		mch_memmove(newprops + j * حجم(textprop_T), &ep,
			حجم(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	إذا (متن_arg != عدم)
	{
	    صحيح copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, متن_arg, copysize);
	    tmp_prop.u.tp_متن_دننه_واټن =
				    (colnr_T)(vtext_dest - شمېر_dest);
	    vtext_dest += copysize;
	}
	وإلا
	    tmp_prop.u.tp_متن_دننه_واټن = 0;

	mch_memmove(newprops + i * حجم(textprop_T), &tmp_prop,
							حجم(textprop_T));

	إذا (buf->b_ml.ml_flags & (ML_کرښه_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_کرښه_ptr);
	buf->b_ml.ml_کرښه_ptr = newtext;
	buf->b_ml.ml_کرښه_len = new_کرښه_len;
	buf->b_ml.ml_flags |= ML_کرښه_DIRTY;
    }

    بدل_شوی_کرښه_display_buf(buf);
    بدل_شوی_کرښې_buf(buf, پیل_lnum, پای_ټکی_lnum + 1, 0);
    res = OK;

theend:
    vim_free(متن);
    أرجع res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    فراغ
f_prop_جمع_کړه_list(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    صحيح		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	پیل_lnum;
    colnr_T	پیل_ستون;
    linenr_T	پای_ټکی_lnum;
    colnr_T	پای_ټکی_ستون;
    صحيح		error = FALSE;
    صحيح		prev_did_emsg = did_emsg;

    إذا (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	أرجع;

    dict = argvars[0].vval.v_dict;
    إذا (dict == عدم || !dict_has_کلی(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	أرجع;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    إذا (dict_has_کلی(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_شمېره(dict, "id");
	إذا (x > لوی_عدد || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    أرجع;
	}
	id = (صحيح)x;
    }

    إذا (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	أرجع;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_توکي(argvars[1].vval.v_list, li)
    {
	إذا (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == عدم)
	{
	    emsg(_(e_list_required));
	    أرجع;
	}

	pos_list = li->li_tv.vval.v_list;
	پیل_lnum = list_پیدا_کړه_nr(pos_list, 0L, &error);
	إذا (!error)
	    پیل_ستون = list_پیدا_کړه_nr(pos_list, 1L, &error);
	إذا (!error)
	    پای_ټکی_lnum = list_پیدا_کړه_nr(pos_list, 2L, &error);
	إذا (!error)
	    پای_ټکی_ستون = list_پیدا_کړه_nr(pos_list, 3L, &error);
	صحيح this_id = id;
	إذا (!error && pos_list->lv_len > 4)
	    this_id = list_پیدا_کړه_nr(pos_list, 4L, &error);
	إذا (error || پیل_lnum <= 0 || پیل_ستون <= 0
		  || پای_ټکی_lnum <= 0 || پای_ټکی_ستون <= 0)
	{
	    إذا (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_دلیل));
	    أرجع;
	}
	إذا (prop_جمع_کړه_one(buf, type_name, this_id, عدم, 0, 0,
			     پیل_lnum, پای_ټکی_lnum, پیل_ستون, پای_ټکی_ستون) == FAIL)
	    أرجع;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
ساكن صحيح vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    ساكن صحيح
get_textprop_id(فراغ)
{
    إذا (vt_id_counter > 0)
	vt_id_counter = 0;
    أرجع --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    صحيح
prop_جمع_کړه_common(
	linenr_T    پیل_lnum,
	colnr_T	    پیل_ستون,
	dict_T	    *dict,
	buf_T	    *default_buf,
	typval_T    *dict_arg)
{
    linenr_T	پای_ټکی_lnum;
    colnr_T	پای_ټکی_ستون;
    char_u	*type_name;
    buf_T	*buf = default_buf;
    صحيح		id = 0;
    char_u	*متن = عدم;
    صحيح		متن_padding_کیڼ = 0;
    صحيح		flags = 0;

    إذا (dict == عدم || !dict_has_کلی(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	اذهب theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    إذا (dict_has_کلی(dict, "end_lnum"))
    {
	پای_ټکی_lnum = dict_get_شمېره(dict, "end_lnum");
	إذا (پای_ټکی_lnum < پیل_lnum)
	{
	    semsg(_(e_invalid_ارزښت_for_دلیل_str), "end_lnum");
	    اذهب theend;
	}
    }
    وإلا
	پای_ټکی_lnum = پیل_lnum;

    إذا (dict_has_کلی(dict, "length"))
    {
	اوږد اوږدتیا = dict_get_شمېره(dict, "length");

	إذا (اوږدتیا < 0 || پای_ټکی_lnum > پیل_lnum)
	{
	    semsg(_(e_invalid_ارزښت_for_دلیل_str), "length");
	    اذهب theend;
	}
	پای_ټکی_ستون = پیل_ستون + اوږدتیا;
    }
    وإلا إذا (dict_has_کلی(dict, "end_col"))
    {
	پای_ټکی_ستون = dict_get_شمېره(dict, "end_col");
	إذا (پای_ټکی_ستون <= 0)
	{
	    semsg(_(e_invalid_ارزښت_for_دلیل_str), "end_col");
	    اذهب theend;
	}
    }
    وإلا إذا (پیل_lnum == پای_ټکی_lnum)
	پای_ټکی_ستون = پیل_ستون;
    وإلا
	پای_ټکی_ستون = 1;

    إذا (dict_has_کلی(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_شمېره(dict, "id");
	إذا (x > لوی_عدد || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    اذهب theend;
	}
	id = (صحيح)x;
    }

    إذا (dict_has_کلی(dict, "text"))
    {
	إذا (dict_has_کلی(dict, "length")
		|| dict_has_کلی(dict, "end_col")
		|| dict_has_کلی(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_اوږدتیا_endcol_and_endlnum_with_متن));
	    اذهب theend;
	}

	متن = dict_get_string(dict, "text", TRUE);
	إذا (متن == عدم)
	    اذهب theend;
	// use a default length of 1 to make multiple props show up
	پای_ټکی_ستون = پیل_ستون + 1;

	إذا (dict_has_کلی(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    إذا (p == عدم)
		اذهب theend;
	    إذا (پیل_ستون != 0)
	    {
		emsg(_(e_can_only_use_متن_align_when_ستنه_is_zero));
		اذهب theend;
	    }
	    إذا (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_ښی;
	    وإلا إذا (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    وإلا إذا (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_لاندې;
	    وإلا إذا (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_ارزښت_for_دلیل_str_str), "text_align", p);
		اذهب theend;
	    }
	}

	إذا (dict_has_کلی(dict, "text_padding_left"))
	{
	    متن_padding_کیڼ = dict_get_شمېره(dict, "text_padding_left");
	    إذا (متن_padding_کیڼ < 0)
	    {
		semsg(_(e_دلیل_must_be_مثبت_str), "text_padding_left");
		اذهب theend;
	    }
	}

	إذا (dict_has_کلی(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    إذا (p == عدم)
		اذهب theend;
	    إذا (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    وإلا إذا (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_ارزښت_for_دلیل_str_str), "text_wrap", p);
		اذهب theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    إذا (پیل_ستون < (متن == عدم ? 1 : 0))
    {
	semsg(_(e_invalid_ستنه_شمېره_nr), (اوږد)پیل_ستون);
	اذهب theend;
    }
    إذا (پیل_ستون > 0 && متن_padding_کیڼ > 0)
    {
	emsg(_(e_can_only_use_کیڼ_padding_when_ستنه_is_zero));
	اذهب theend;
    }

    إذا (dict_arg != عدم && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	اذهب theend;

    إذا (متن != عدم)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    وإلا إذا (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	اذهب theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_جمع_کړه_one(buf, type_name, id, متن, متن_padding_کیڼ, flags,
				    پیل_lnum, پای_ټکی_lnum, پیل_ستون, پای_ټکی_ستون);
    متن = عدم;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(متن);
    أرجع id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    صحيح
get_متن_props(buf_T *buf, linenr_T lnum, char_u **props, صحيح will_change)
{
    char_u	*متن;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_شمېر;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    إذا ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == عدم)
	أرجع 0;

    // Fetch the line to get the ml_line_len field updated.
    متن = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_کرښه_len - textlen;
    إذا (propdata_len == 0)
	أرجع 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    إذا (propdata_len < PROP_شمېر_اندازه + حجم(textprop_T))
    {
	emsg(e_متن_property_info_corrupted);
	أرجع 0;
    }
    mch_memmove(&prop_شمېر, متن + textlen, PROP_شمېر_اندازه);
    إذا (!متن_prop_شمېر_valid(prop_شمېر, propdata_len))
    {
	emsg(e_متن_property_info_corrupted);
	أرجع 0;
    }
    إذا (!متن_prop_vtext_valid(متن + textlen + PROP_شمېر_اندازه,
					   (صحيح)prop_شمېر, propdata_len))
    {
	emsg(e_متن_property_info_corrupted);
	أرجع 0;
    }
    *props = متن + textlen + PROP_شمېر_اندازه;
    أرجع (صحيح)prop_شمېر;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    صحيح
prop_شمېر_above_لاندې(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    صحيح		شمېر = get_متن_props(buf, lnum, &props, FALSE);
    صحيح		پایله = 0;
    textprop_T	prop;
    صحيح		i;
    صحيح		بل_ښی_goes_لاندې = FALSE;

    إذا (شمېر == 0)
	أرجع 0;
    لكل (i = 0; i < شمېر; ++i)
    {
	mch_memmove(&prop, props + i * حجم(prop), حجم(prop));
	إذا (prop.tp_ستون == MAXCOL && متن_prop_type_valid(buf, &prop))
	{
	    إذا ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_لاندې))
		    || (بل_ښی_goes_لاندې
				     && (prop.tp_flags & TP_FLAG_ALIGN_ښی)))
	    {
		++پایله;
	    }
	    وإلا إذا (prop.tp_flags & TP_FLAG_ALIGN_ښی)
		بل_ښی_goes_لاندې = TRUE;
	}
    }
    أرجع پایله;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    صحيح
شمېر_props(linenr_T lnum, صحيح only_starting, صحيح وروستی_کرښه)
{
    char_u	*props;
    صحيح		proplen = get_متن_props(curbuf, lnum, &props, 0);
    صحيح		پایله = proplen;
    textprop_T	prop;

    لكل (صحيح i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * حجم(prop), حجم(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	إذا ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!وروستی_کرښه && prop.tp_ستون == MAXCOL))
	    --پایله;
    }
    أرجع پایله;
}

ساكن textprop_T	*متن_prop_compare_props;
ساكن buf_T		*متن_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    ساكن صحيح
متن_prop_ترتیب(صحيح flags)
{
    إذا (flags & TP_FLAG_ALIGN_ABOVE)
	أرجع 0;
    إذا (flags & TP_FLAG_ALIGN_ښی)
	أرجع 2;
    إذا (flags & TP_FLAG_ALIGN_لاندې)
	أرجع 3;
    أرجع 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    ساكن صحيح
متن_prop_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(صحيح *)s1;
    idx2 = *(صحيح *)s2;
    tp1 = &متن_prop_compare_props[idx1];
    tp2 = &متن_prop_compare_props[idx2];
    col1 = tp1->tp_ستون;
    col2 = tp2->tp_ستون;

    // property that inserts text has priority over one that doesn't
    إذا ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	أرجع tp1->tp_id < 0 ? 1 : -1;

    إذا (col1 == MAXCOL || col2 == MAXCOL)
    {
	صحيح order1 = متن_prop_ترتیب(tp1->tp_flags);
	صحيح order2 = متن_prop_ترتیب(tp2->tp_flags);

	// sort on order where it is added
	إذا (order1 != order2)
	    أرجع order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = متن_prop_type_by_id(متن_prop_compare_buf, tp1->tp_type);
    pt2 = متن_prop_type_by_id(متن_prop_compare_buf, tp2->tp_type);
    إذا (pt1 != pt2)
    {
	إذا (pt1 == عدم)
	    أرجع -1;
	إذا (pt2 == عدم)
	    أرجع 1;
	إذا (pt1->pt_لومړیتوب != pt2->pt_لومړیتوب)
	    أرجع pt1->pt_لومړیتوب > pt2->pt_لومړیتوب ? 1 : -1;
    }

    // same priority, one that starts first wins
    إذا (col1 != col2)
	أرجع col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    إذا (tp1->tp_id < 0)
	أرجع tp1->tp_id > tp2->tp_id ? 1 : -1;

    أرجع 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    فراغ
ترتیبول_متن_props(
	buf_T	    *buf,
	textprop_T  *props,
	صحيح	    *idxs,
	صحيح	    شمېر)
{
    متن_prop_compare_buf = buf;
    متن_prop_compare_props = props;
    qsort((فراغ *)idxs, (size_t)شمېر, حجم(صحيح), متن_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
پیدا_کړه_prop_دننه_کرښې(
	win_T	    *wp,
	صحيح	    type_id,
	صحيح	    id,
	textprop_T  *prop,
	linenr_T    *وموندل_lnum,
	linenr_T    لومړی_lnum,
	linenr_T    وروستی_lnum)
{
    إذا (متن_prop_type_by_id(wp->w_بفر, type_id) == عدم)
	أرجع false;

    إذا (لومړی_lnum < 1)
	لومړی_lnum = 1;
    إذا (وروستی_lnum > wp->w_بفر->b_ml.ml_کرښه_شمېر)
	وروستی_lnum = wp->w_بفر->b_ml.ml_کرښه_شمېر;

    لكل (linenr_T lnum = لومړی_lnum; lnum <= وروستی_lnum; ++lnum)
    {
	char_u	*props;
	صحيح	شمېر = get_متن_props(wp->w_بفر, lnum, &props, FALSE);
	لكل (صحيح i = 0; i < شمېر; ++i)
	{
	    mch_memmove(prop, props + i * حجم(textprop_T),
							   حجم(textprop_T));
	    إذا (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*وموندل_lnum = lnum;
		أرجع true;
	    }
	}
    }
    أرجع false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
پیدا_کړه_visible_prop(
	win_T	    *wp,
	صحيح	    type_id,
	صحيح	    id,
	textprop_T  *prop,
	linenr_T    *وموندل_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    أرجع پیدا_کړه_prop_دننه_کرښې(wp, type_id, id, prop, وموندل_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    ساكن فراغ
سېټ_متن_props(linenr_T lnum, textprop_T *tps, صحيح شمېر)
{
    char_u  *متن;
    char_u  *newtext;
    صحيح	    textlen;

    متن = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    إذا (شمېر == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	إذا (newtext == عدم)
	    أرجع;
	mch_memmove(newtext, متن, textlen);
	إذا (curbuf->b_ml.ml_flags & (ML_کرښه_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_کرښه_ptr);
	curbuf->b_ml.ml_کرښه_ptr = newtext;
	curbuf->b_ml.ml_کرښه_len = textlen;
	curbuf->b_ml.ml_flags |= ML_کرښه_DIRTY;
    }
    وإلا
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_شمېر = (uint16_t)شمېر;
	صحيح	    vtext_اندازه = 0;
	صحيح	    ټول_len;
	char_u	    *شمېر_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	صحيح	    i;

	لكل (i = 0; i < شمېر; ++i)
	    إذا (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_اندازه += tps[i].tp_len + 1;

	ټول_len = textlen + (صحيح)PROP_شمېر_اندازه
		    + شمېر * (صحيح)حجم(textprop_T) + vtext_اندازه;
	newtext = alloc(ټول_len);
	إذا (newtext == عدم)
	    أرجع;
	mch_memmove(newtext, متن, textlen);

	شمېر_dest = newtext + textlen;
	mch_memmove(شمېر_dest, &prop_شمېر, PROP_شمېر_اندازه);

	prop_dest = شمېر_dest + PROP_شمېر_اندازه;
	vtext_dest = prop_dest + شمېر * حجم(textprop_T);

	لكل (i = 0; i < شمېر; ++i)
	{
	    textprop_T prop = tps[i];

	    إذا (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		صحيح copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_متن, copysize);
		vim_free(tps[i].u.tp_متن);
		tps[i].u.tp_متن = عدم;
		prop.u.tp_متن_دننه_واټن = (colnr_T)(vtext_dest - شمېر_dest);
		vtext_dest += copysize;
	    }
	    وإلا
		prop.u.tp_متن_دننه_واټن = 0;
	    mch_memmove(prop_dest, &prop, حجم(textprop_T));
	    prop_dest += حجم(textprop_T);
	}

	إذا (curbuf->b_ml.ml_flags & (ML_کرښه_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_کرښه_ptr);
	curbuf->b_ml.ml_کرښه_ptr = newtext;
	curbuf->b_ml.ml_کرښه_len = ټول_len;
	curbuf->b_ml.ml_flags |= ML_کرښه_DIRTY;
    }
}

/*
 * Convert a line buffer with text properties in the old format
 * [text][NUL][textprop_T...] to the new format
 * [text][NUL][prop_count][textprop_T...][vtext...].
 * For virtual text props, u.tp_text is expected to be a pointer to the
 * vtext string (allocated).  These strings are freed after copying.
 * Returns a newly allocated buffer, or NULL on failure.
 * "line_len" is the total length (text + props).  "textlen" is the length
 * of the text including NUL.  "*new_len" is set to the new total length.
 */
    char_u *
props_جمع_کړه_شمېر_سرلیک(char_u *کرښه, صحيح کرښه_len, صحيح textlen, صحيح *new_len)
{
    صحيح		prop_بایټونه = کرښه_len - textlen;
    uint16_t	prop_شمېر;
    صحيح		vtext_اندازه = 0;
    صحيح		ټول;
    char_u	*newline;
    char_u	*شمېر_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    صحيح		i;

    إذا (prop_بایټونه <= 0 || prop_بایټونه % حجم(textprop_T) != 0)
    {
	*new_len = کرښه_len;
	أرجع عدم;
    }
    prop_شمېر = (uint16_t)(prop_بایټونه / حجم(textprop_T));

    // Calculate total vtext size.
    لكل (i = 0; i < (صحيح)prop_شمېر; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, کرښه + textlen + i * حجم(textprop_T),
							حجم(textprop_T));
	إذا (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_اندازه += prop.tp_len + 1;
    }

    ټول = textlen + (صحيح)PROP_شمېر_اندازه + prop_بایټونه + vtext_اندازه;
    newline = alloc(ټول);
    إذا (newline == عدم)
	أرجع عدم;
    *new_len = ټول;

    mch_memmove(newline, کرښه, textlen);
    شمېر_dest = newline + textlen;
    mch_memmove(شمېر_dest, &prop_شمېر, PROP_شمېر_اندازه);
    prop_dest = شمېر_dest + PROP_شمېر_اندازه;
    vtext_dest = prop_dest + prop_بایټونه;

    لكل (i = 0; i < (صحيح)prop_شمېر; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, کرښه + textlen + i * حجم(textprop_T),
							حجم(textprop_T));
	إذا (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    صحيح copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_متن, copysize);
	    vim_free(prop.u.tp_متن);
	    prop.u.tp_متن_دننه_واټن = (colnr_T)(vtext_dest - شمېر_dest);
	    vtext_dest += copysize;
	}
	وإلا
	    prop.u.tp_متن_دننه_واټن = 0;
	mch_memmove(prop_dest + i * حجم(textprop_T), &prop,
							حجم(textprop_T));
    }
    أرجع newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    فراغ
جمع_کړه_متن_props(linenr_T lnum, textprop_T *متن_props, صحيح متن_prop_شمېر)
{
    unpacked_memline_T	um;
    صحيح			i;

    um = um_open_at(curbuf, lnum, متن_prop_شمېر);
    إذا (um.buf == عدم)
	أرجع;
    إذا (!um_detach(&um))
    {
	um_abort(&um);
	أرجع;
    }

    // Grow props array if needed.
    إذا (um.prop_شمېر + متن_prop_شمېر > um.prop_اندازه)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_شمېر + متن_prop_شمېر) * حجم(textprop_T));
	إذا (newprops == عدم)
	{
	    um_abort(&um);
	    أرجع;
	}
	um.props = newprops;
	um.prop_اندازه = um.prop_شمېر + متن_prop_شمېر;
    }

    لكل (i = 0; i < متن_prop_شمېر; ++i)
	um.props[um.prop_شمېر++] = متن_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    ساكن صحيح
compare_pt(ثابت فراغ *s1, ثابت فراغ *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    أرجع tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    ساكن proptype_T *
پیدا_کړه_type_by_id(hashtab_T *ht, proptype_T ***array, صحيح id)
{
    صحيح ټیټ = 0;
    صحيح لوړ;

    إذا (ht == عدم || ht->ht_کارول_شوی == 0)
	أرجع عدم;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    إذا (*array == عدم)
    {
	اوږد	    todo;
	hashitem_T  *hi;
	صحيح	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_کارول_شوی);
	إذا (*array == عدم)
	    أرجع عدم;
	todo = (اوږد)ht->ht_کارول_شوی;
	FOR_ALL_HASHTAB_توکي(ht, hi, todo)
	{
	    إذا (!HASHITEM_تشه(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((فراغ *)*array, ht->ht_کارول_شوی, حجم(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    لوړ = ht->ht_کارول_شوی;
    طالما (لوړ > ټیټ)
    {
	صحيح m = (لوړ + ټیټ) / 2;

	إذا ((*array)[m]->pt_id == id)
	    أرجع (*array)[m];
	إذا ((*array)[m]->pt_id > id)
	    لوړ = m;
	وإلا
	    ټیټ = m + 1;
    }
    أرجع عدم;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    ساكن فراغ
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    صحيح buflocal = TRUE;
    صحيح virtualtext_prop = prop->tp_id < 0;

    dict_جمع_کړه_شمېره(dict, "col", (prop->tp_ستون == MAXCOL) ? 0 : prop->tp_ستون);
    إذا (!virtualtext_prop)
    {
	dict_جمع_کړه_شمېره(dict, "length", prop->tp_len);
	dict_جمع_کړه_شمېره(dict, "id", prop->tp_id);
    }
    dict_جمع_کړه_شمېره(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_جمع_کړه_شمېره(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_بل));

    pt = پیدا_کړه_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    إذا (pt == عدم)
    {
	pt = پیدا_کړه_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    إذا (pt != عدم)
	dict_جمع_کړه_string(dict, "type", pt->pt_name);

    إذا (buflocal)
	dict_جمع_کړه_شمېره(dict, "type_bufnr", buf->b_fnum);
    وإلا
	dict_جمع_کړه_شمېره(dict, "type_bufnr", 0);
    إذا (virtualtext_prop)
    {
	string_T    متن_align = {عدم, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_جمع_کړه_string(dict, "text", prop->u.tp_متن);

	// text_align
	إذا (prop->tp_flags & TP_FLAG_ALIGN_ښی)
	    STR_LITERAL_سېټ(متن_align, "right");
	وإلا إذا (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_سېټ(متن_align, "above");
	وإلا إذا (prop->tp_flags & TP_FLAG_ALIGN_لاندې)
	    STR_LITERAL_سېټ(متن_align, "below");
	إذا (متن_align.string != عدم)
	    dict_جمع_کړه_string_len(dict, "text_align",
		متن_align.string, (صحيح)متن_align.اوږدتیا);

	// text_wrap
	إذا (prop->tp_flags & TP_FLAG_WRAP)
	    dict_جمع_کړه_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	إذا (prop->tp_padleft != 0)
	    dict_جمع_کړه_شمېره(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
متن_prop_type_by_id(buf_T *buf, صحيح id)
{
    proptype_T *type;

    type = پیدا_کړه_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    إذا (type == عدم)
	type = پیدا_کړه_type_by_id(global_proptypes, &global_proparray, id);
    أرجع type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
متن_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    أرجع متن_prop_type_by_id(buf, prop->tp_type) != عدم;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    فراغ
f_prop_clear(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    linenr_T پیل;
    linenr_T پای_ټکی;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    صحيح	    did_clear = FALSE;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_opt_شمېره_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	أرجع;

    پیل = tv_get_شمېره(&argvars[0]);
    پای_ټکی = پیل;
    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	پای_ټکی = tv_get_شمېره(&argvars[1]);
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	{
	    إذا (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		أرجع;
	}
    }
    إذا (پیل < 1 || پای_ټکی < 1)
    {
	emsg(_(e_invalid_range));
	أرجع;
    }

    {
	unpacked_memline_T um = um_open(buf);

	لكل (lnum = پیل; lnum <= پای_ټکی; ++lnum)
	{
	    صحيح idx;

	    إذا (lnum > buf->b_ml.ml_کرښه_شمېر)
		اكسر;
	    إذا (!um_goto_کرښه(&um, lnum, 0))
		اكسر;
	    إذا (um.prop_شمېر > 0)
	    {
		did_clear = TRUE;
		لكل (idx = um.prop_شمېر - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    إذا (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    فراغ
f_prop_پیدا_کړه(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    صحيح		lnum_پیل;
    صحيح		پیل_pos_has_prop = 0;
    صحيح		کتل_شوی_پای_ټکی = FALSE;
    صحيح		id = 0;
    صحيح		id_وموندل = FALSE;
    صحيح		type_id = -1;
    صحيح		skipstart = FALSE;
    صحيح		lnum = -1;
    صحيح		ستون = -1;
    صحيح		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    صحيح		both;

    إذا (دننه_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	أرجع;
    dict = argvars[0].vval.v_dict;

    إذا (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	أرجع;
    إذا (buf->b_ml.ml_mfp == عدم)
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	إذا (*dir_s == 'b')
	    dir = BACKWARD;
	وإلا إذا (*dir_s != 'f')
	{
	    emsg(_(e_invalid_دلیل));
	    أرجع;
	}
    }

    di = dict_پیدا_کړه(dict, (char_u *)"lnum", -1);
    إذا (di != عدم)
	lnum = tv_get_شمېره(&di->di_tv);

    di = dict_پیدا_کړه(dict, (char_u *)"col", -1);
    إذا (di != عدم)
	ستون = tv_get_شمېره(&di->di_tv);

    إذا (lnum == -1)
    {
	lnum = cursor->lnum;
	ستون = cursor->ستون + 1;
    }
    وإلا إذا (ستون == -1)
	ستون = 1;

    إذا (lnum < 1 || lnum > buf->b_ml.ml_کرښه_شمېر)
    {
	emsg(_(e_invalid_range));
	أرجع;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    إذا (dict_has_کلی(dict, "id"))
    {
	id = dict_get_شمېره(dict, "id");
	id_وموندل = TRUE;
    }
    إذا (dict_has_کلی(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	إذا (type == عدم)
	    أرجع;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    إذا (!id_وموندل && type_id == -1)
    {
	emsg(_(e_need_at_لږترلږه_one_د_id_or_type));
	أرجع;
    }
    إذا (both && (!id_وموندل || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	أرجع;
    }

    lnum_پیل = lnum;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

    طالما (1)
    {
	char_u	*prop_پیل_ptr;
	صحيح	شمېر = get_متن_props(buf, lnum, &prop_پیل_ptr, FALSE);
	صحيح	    i;
	textprop_T  prop;
	صحيح	    prop_پیل;
	صحيح	    prop_پای_ټکی;

	لكل (i = dir == BACKWARD ? شمېر - 1 : 0; i >= 0 && i < شمېر; i += dir)
	{
	    mch_memmove(&prop, prop_پیل_ptr + i * حجم(textprop_T),
							   حجم(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    إذا (prop.tp_id < 0 && prop.u.tp_متن_دننه_واټن > 0)
	    {
		prop.u.tp_متن = (prop_پیل_ptr - PROP_شمېر_اندازه)
						    + prop.u.tp_متن_دننه_واټن;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    وإلا
		prop.u.tp_متن = عدم;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    إذا (lnum == lnum_پیل)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_ستون == MAXCOL;
		إذا (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    إذا (!is_floating_vtext && prop.tp_ستون > ستون)
			استمر;
		}
		وإلا إذا (!is_floating_vtext
		    && prop.tp_ستون + prop.tp_len - (prop.tp_len != 0) < ستون)
		    استمر;
	    }
	    إذا (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_وموندل && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		إذا (lnum_پیل == lnum
			&& ستون >= prop.tp_ستون
			&& (ستون <= prop.tp_ستون + prop.tp_len
							 - (prop.tp_len != 0)))
		    پیل_pos_has_prop = 1;

		// The property was not continued from last line, it starts on
		// this line.
		prop_پیل = !(prop.tp_flags & TP_FLAG_CONT_PREV);
		// The property does not continue on the next line, it ends on
		// this line.
		prop_پای_ټکی = !(prop.tp_flags & TP_FLAG_CONT_بل);
		إذا (!prop_پیل && prop_پای_ټکی && dir == FORWARD)
		    کتل_شوی_پای_ټکی = 1;

		// Skip lines without the start flag.
		إذا (!prop_پیل)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    إذا (پیل_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    استمر;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		إذا (پیل_pos_has_prop && skipstart && !کتل_شوی_پای_ټکی)
		{
		    پیل_pos_has_prop = 0;
		    استمر;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_جمع_کړه_شمېره(rettv->vval.v_dict, "lnum", lnum);

		أرجع;
	    }
	}

	إذا (dir > 0)
	{
	    إذا (lnum >= buf->b_ml.ml_کرښه_شمېر)
		اكسر;
	    lnum++;
	}
	وإلا
	{
	    إذا (lnum <= 1)
		اكسر;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    ساكن صحيح
prop_type_or_id_دننه_list(صحيح *types_or_ids, صحيح len, صحيح type_or_id)
{
    صحيح i;

    لكل (i = 0; i < len; i++)
	إذا (types_or_ids[i] == type_or_id)
	    أرجع TRUE;

    أرجع FALSE;
}

/*
 * Return all the text properties in line 'lnum' in buffer 'buf' in 'retlist'.
 * If 'prop_types' is not NULL, then return only the text properties with
 * matching property type in the 'prop_types' array.
 * If 'prop_ids' is not NULL, then return only the text properties with
 * an identifier in the 'props_ids' array.
 * If 'add_lnum' is TRUE, then add the line number also to the text property
 * dictionary.
 */
    ساكن فراغ
get_props_دننه_کرښه(
	buf_T		*buf,
	linenr_T	lnum,
	صحيح		*prop_types,
	صحيح		prop_types_len,
	صحيح		*prop_ids,
	صحيح		prop_ids_len,
	list_T		*retlist,
	صحيح		جمع_کړه_lnum)
{
    char_u	*props;
    صحيح		شمېر;
    صحيح		i;
    textprop_T	prop;

    شمېر = get_متن_props(buf, lnum, &props, FALSE);
    لكل (i = 0; i < شمېر; ++i)
    {
	mch_memmove(&prop, props + i * حجم(textprop_T),
		حجم(textprop_T));
	// Convert offset to pointer for virtual text props.
	إذا (prop.tp_id < 0 && prop.u.tp_متن_دننه_واټن > 0)
	{
	    prop.u.tp_متن = (props - PROP_شمېر_اندازه)
						+ prop.u.tp_متن_دننه_واټن;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	وإلا
	    prop.u.tp_متن = عدم;
	إذا ((prop_types == عدم
		    || prop_type_or_id_دننه_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == عدم
		    || prop_type_or_id_دننه_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    إذا (d == عدم)
		اكسر;
	    prop_fill_dict(d, &prop, buf);
	    إذا (جمع_کړه_lnum)
		dict_جمع_کړه_شمېره(d, "lnum", lnum);
	    إذا (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    ساكن صحيح *
get_prop_types_from_نومونه(list_T *l, buf_T *buf, صحيح *عدد_types)
{
    صحيح		*prop_types;
    listitem_T	*li;
    صحيح		i;
    char_u	*name;
    proptype_T	*type;

    *عدد_types = 0;

    prop_types = ALLOC_MULT(صحيح, list_len(l));
    إذا (prop_types == عدم)
	أرجع عدم;

    i = 0;
    FOR_ALL_LIST_توکي(l, li)
    {
	إذا (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    اذهب errret;
	}
	name = li->li_tv.vval.v_string;
	إذا (name == عدم)
	    اذهب errret;

	type = lookup_prop_type(name, buf);
	إذا (type == عدم)
	    اذهب errret;
	prop_types[i++] = type->pt_id;
    }

    *عدد_types = i;
    أرجع prop_types;

errret:
    VIM_CLEAR(prop_types);
    أرجع عدم;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    ساكن صحيح *
get_prop_ids_from_list(list_T *l, صحيح *عدد_ids)
{
    صحيح		*prop_ids;
    listitem_T	*li;
    صحيح		i = 0;
    صحيح		id;
    صحيح		error;

    *عدد_ids = 0;

    prop_ids = ALLOC_MULT(صحيح, list_len(l));
    إذا (prop_ids == عدم)
	أرجع عدم;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_توکي(l, li)
    {
	error = FALSE;
	id = tv_get_شمېره_chk(&li->li_tv, &error);
	إذا (error)
	    اذهب errret;

	prop_ids[i++] = id;
    }

    *عدد_ids = i;
    أرجع prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    أرجع عدم;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    فراغ
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	پیل_lnum;
    linenr_T	پای_ټکی_lnum;
    buf_T	*buf = curbuf;
    صحيح		جمع_کړه_lnum = FALSE;
    صحيح		*prop_types = عدم;
    صحيح		prop_types_len = 0;
    صحيح		*prop_ids = عدم;
    صحيح		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    // default: get text properties on current line
    پیل_lnum = tv_get_شمېره(&argvars[0]);
    پای_ټکی_lnum = پیل_lnum;
    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	إذا (check_for_dict_arg(argvars, 1) == FAIL)
	    أرجع;
	d = argvars[1].vval.v_dict;

	إذا (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    أرجع;

	إذا (d != عدم && (di = dict_پیدا_کړه(d, (char_u *)"end_lnum", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_شمېره)
	    {
		emsg(_(e_شمېره_required));
		أرجع;
	    }
	    پای_ټکی_lnum = tv_get_شمېره(&di->di_tv);
	    إذا (پای_ټکی_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//کرښه
		پای_ټکی_lnum = buf->b_ml.ml_کرښه_شمېر + پای_ټکی_lnum + 1;
	    وإلا إذا (پای_ټکی_lnum > buf->b_ml.ml_کرښه_شمېر)
		پای_ټکی_lnum = buf->b_ml.ml_کرښه_شمېر;
	    جمع_کړه_lnum = TRUE;
	}
	إذا (d != عدم && (di = dict_پیدا_کړه(d, (char_u *)"types", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		أرجع;
	    }

	    l = di->di_tv.vval.v_list;
	    إذا (l != عدم && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_نومونه(l, buf, &prop_types_len);
		إذا (prop_types == عدم)
		    أرجع;
	    }
	}
	إذا (d != عدم && (di = dict_پیدا_کړه(d, (char_u *)"ids", -1)) != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		اذهب errret;
	    }

	    l = di->di_tv.vval.v_list;
	    إذا (l != عدم && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		إذا (prop_ids == عدم)
		    اذهب errret;
	    }
	}
    }
    إذا (پیل_lnum < 1 || پیل_lnum > buf->b_ml.ml_کرښه_شمېر
		|| پای_ټکی_lnum < 1 || پای_ټکی_lnum < پیل_lnum)
	emsg(_(e_invalid_range));
    وإلا
	لكل (lnum = پیل_lnum; lnum <= پای_ټکی_lnum; lnum++)
	    get_props_دننه_کرښه(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, جمع_کړه_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    فراغ
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	پیل = 1;
    linenr_T	پای_ټکی = 0;
    linenr_T	lnum;
    linenr_T	لومړی_بدل_شوی = 0;
    linenr_T	وروستی_بدل_شوی = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    صحيح		do_all;
    صحيح		id = -MAXCOL;
    صحيح		type_id = -1;	    // for a single "type"
    صحيح		*type_ids = عدم;   // array, for a list of "types", allocated
    صحيح		عدد_type_ids = 0;   // number of elements in "type_ids"
    صحيح		both;

    rettv->vval.v_شمېره = 0;

    إذا (دننه_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_شمېره_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_شمېره_arg(argvars, 2) == FAIL)))
	أرجع;

    إذا (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	پیل = tv_get_شمېره(&argvars[1]);
	پای_ټکی = پیل;
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	    پای_ټکی = tv_get_شمېره(&argvars[2]);
	إذا (پیل < 1 || پای_ټکی < 1)
	{
	    emsg(_(e_invalid_range));
	    أرجع;
	}
    }

    dict = argvars[0].vval.v_dict;
    إذا (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	أرجع;
    إذا (buf->b_ml.ml_mfp == عدم)
	أرجع;

    do_all = dict_get_bool(dict, "all", FALSE);

    إذا (dict_has_کلی(dict, "id"))
	id = dict_get_شمېره(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    إذا (dict_has_کلی(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	إذا (type == عدم)
	    أرجع;
	type_id = type->pt_id;
    }
    إذا (dict_has_کلی(dict, "types"))
    {
	typval_T types;
	listitem_T *li = عدم;

	dict_get_tv(dict, "types", &types);
	إذا (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( حجم(صحيح) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_توکي(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		إذا (li->li_tv.v_type != VAR_STRING)
		    استمر;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		إذا (!prop_type)
		    اذهب cleanup_prop_remove;

		type_ids[عدد_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    إذا (id == -MAXCOL && (type_id == -1 && عدد_type_ids == 0))
    {
	emsg(_(e_need_at_لږترلږه_one_د_id_or_type));
	اذهب cleanup_prop_remove;
    }
    إذا (both && (id == -MAXCOL || (type_id == -1 && عدد_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	اذهب cleanup_prop_remove;
    }
    إذا (type_id != -1 && عدد_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	اذهب cleanup_prop_remove;
    }

    إذا (پای_ټکی == 0)
	پای_ټکی = buf->b_ml.ml_کرښه_شمېر;

    {
	unpacked_memline_T um = um_open(buf);

	لكل (lnum = پیل; lnum <= پای_ټکی; ++lnum)
	{
	    صحيح idx;

	    إذا (lnum > buf->b_ml.ml_کرښه_شمېر)
		اكسر;
	    إذا (!um_goto_کرښه(&um, lnum, 0))
		اكسر;

	    لكل (idx = 0; idx < um.prop_شمېر; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		صحيح	    matches_id = 0;
		صحيح	    matches_type = 0;

		إذا (prop->tp_flags & TP_FLAG_DELETED)
		    استمر;

		matches_id = prop->tp_id == id;
		إذا (عدد_type_ids > 0)
		{
		    صحيح idx2;

		    لكل (idx2 = 0; !matches_type && idx2 < عدد_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		وإلا
		    matches_type = prop->tp_type == type_id;

		إذا (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    إذا (لومړی_بدل_شوی == 0)
			لومړی_بدل_شوی = lnum;
		    وروستی_بدل_شوی = lnum;
		    ++rettv->vval.v_شمېره;
		    إذا (!do_all)
			اكسر;
		}
	    }
	}
	um_close(&um);
    }

    إذا (لومړی_بدل_شوی > 0)
    {
	بدل_شوی_کرښه_display_buf(buf);
	بدل_شوی_کرښې_buf(buf, لومړی_بدل_شوی, وروستی_بدل_شوی + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    ساكن فراغ
prop_type_سېټ(typval_T *argvars, صحيح جمع_کړه)
{
    char_u	*name;
    buf_T	*buf = عدم;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    name = tv_get_string(&argvars[0]);
    إذا (*name == NUL)
    {
	semsg(_(e_invalid_دلیل_str), "\"\"");
	أرجع;
    }

    إذا (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	أرجع;
    dict = argvars[1].vval.v_dict;

    prop = پیدا_کړه_prop_type(name, buf);
    إذا (جمع_کړه)
    {
	hashtab_T **htp;

	إذا (prop != عدم)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    أرجع;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	إذا (prop == عدم)
	    أرجع;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	إذا (buf == عدم)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	وإلا
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	إذا (*htp == عدم)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    إذا (*htp == عدم)
	    {
		vim_free(prop);
		أرجع;
	    }
	    hash_init(*htp);
	}
	hash_جمع_کړه(*htp, PT2HIKEY(prop), "prop type");
    }
    وإلا
    {
	إذا (prop == عدم)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    أرجع;
	}
    }

    إذا (dict != عدم)
    {
	di = dict_پیدا_کړه(dict, (char_u *)"highlight", -1);
	إذا (di != عدم)
	{
	    char_u	*highlight;
	    صحيح		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    إذا (highlight != عدم && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    إذا (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_ډله_name_str),
			highlight == عدم ? (char_u *)"" : highlight);
		أرجع;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_پیدا_کړه(dict, (char_u *)"combine", -1);
	إذا (di != عدم)
	{
	    إذا (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    وإلا
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_پیدا_کړه(dict, (char_u *)"override", -1);
	إذا (di != عدم)
	{
	    إذا (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    وإلا
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_پیدا_کړه(dict, (char_u *)"priority", -1);
	إذا (di != عدم)
	    prop->pt_لومړیتوب = tv_get_شمېره(&di->di_tv);

	di = dict_پیدا_کړه(dict, (char_u *)"start_incl", -1);
	إذا (di != عدم)
	{
	    إذا (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_پیل_INCL;
	    وإلا
		prop->pt_flags &= ~PT_FLAG_INS_پیل_INCL;
	}

	di = dict_پیدا_کړه(dict, (char_u *)"end_incl", -1);
	إذا (di != عدم)
	{
	    إذا (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_پای_ټکی_INCL;
	    وإلا
		prop->pt_flags &= ~PT_FLAG_INS_پای_ټکی_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    فراغ
f_prop_type_جمع_کړه(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    prop_type_سېټ(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    فراغ
f_prop_type_change(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    prop_type_سېټ(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    فراغ
f_prop_type_delete(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    char_u	*name;
    buf_T	*buf = عدم;
    hashitem_T	*hi;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    name = tv_get_string(&argvars[0]);
    إذا (*name == NUL)
    {
	semsg(_(e_invalid_دلیل_str), "\"\"");
	أرجع;
    }

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	إذا (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    أرجع;
    }

    hi = پیدا_کړه_prop_type_hi(name, buf);
    إذا (hi == عدم)
	أرجع;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    إذا (buf == عدم)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    وإلا
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    بدل_شوی_کړکۍ_setting_buf(buf == عدم ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    فراغ
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    name = tv_get_string(&argvars[0]);
    إذا (*name == NUL)
    {
	semsg(_(e_invalid_دلیل_str), "\"\"");
	أرجع;
    }

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

    proptype_T  *prop = عدم;
    buf_T	    *buf = عدم;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	إذا (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    أرجع;
    }

    prop = پیدا_کړه_prop_type(name, buf);
    إذا (prop == عدم)
	أرجع;

    dict_T *d = rettv->vval.v_dict;

    إذا (prop->pt_hl_id > 0)
	dict_جمع_کړه_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_جمع_کړه_شمېره(d, "priority", prop->pt_لومړیتوب);
    dict_جمع_کړه_شمېره(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_جمع_کړه_شمېره(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_جمع_کړه_شمېره(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_پیل_INCL) ? 1 : 0);
    dict_جمع_کړه_شمېره(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_پای_ټکی_INCL) ? 1 : 0);
    إذا (buf != عدم)
	dict_جمع_کړه_شمېره(d, "bufnr", buf->b_fnum);
}

    ساكن فراغ
list_types(hashtab_T *ht, list_T *l)
{
    اوږد	todo;
    hashitem_T	*hi;

    todo = (اوږد)ht->ht_کارول_شوی;
    FOR_ALL_HASHTAB_توکي(ht, hi, todo)
    {
	إذا (!HASHITEM_تشه(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    list_append_string(l, prop->pt_name, -1);
	    --todo;
	}
    }
}

/*
 * prop_type_list([{bufnr}])
 */
    فراغ
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = عدم;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	إذا (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    أرجع;
    }
    إذا (buf == عدم)
    {
	إذا (global_proptypes != عدم)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    وإلا إذا (buf->b_proptypes != عدم)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    ساكن فراغ
clear_ht_prop_types(hashtab_T *ht)
{
    اوږد	todo;
    hashitem_T	*hi;

    إذا (ht == عدم)
	أرجع;

    todo = (اوږد)ht->ht_کارول_شوی;
    FOR_ALL_HASHTAB_توکي(ht, hi, todo)
    {
	إذا (!HASHITEM_تشه(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#إذا defined(EXITFREE)
/*
 * Free all global property types.
 */
    فراغ
clear_global_prop_types(فراغ)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = عدم;
    VIM_CLEAR(global_proparray);
}
#د_که_پای

/*
 * Free all property types for "buf".
 */
    فراغ
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = عدم;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
تعريفنوع بنية
{
    صحيح dirty;	    // if the property was changed
    صحيح can_drop;   // whether after this change, the prop may be removed
} adjustres_T;

/*
 * Adjust the property for "added" bytes (can be negative) inserted at "col".
 *
 * Note that "col" is zero-based, while tp_col is one-based.
 * Only for the current buffer.
 * "flags" can have:
 * APC_SUBSTITUTE:	Text is replaced, not inserted.
 * APC_INDENT:		Text is inserted before virtual text prop
 */
    ساكن adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    ستون,
	صحيح	    added,
	صحيح	    flags)
{
    proptype_T	*pt;
    صحيح		پیل_incl;
    صحيح		پای_ټکی_incl;
    صحيح		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    إذا (prop->tp_ستون == MAXCOL)
    {
	res.dirty = FALSE;
	أرجع res;
    }

    pt = متن_prop_type_by_id(curbuf, prop->tp_type);
    پیل_incl = (pt != عدم && (pt->pt_flags & PT_FLAG_INS_پیل_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    إذا (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	پیل_incl = FALSE;
    پای_ټکی_incl = (pt != عدم && (pt->pt_flags & PT_FLAG_INS_پای_ټکی_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_بل);
    // do not drop zero-width props if they later can increase in size
    droppable = !(پیل_incl || پای_ټکی_incl);

    إذا (added > 0)
    {
	إذا (ستون + 1 <= prop->tp_ستون
		- (پیل_incl || (prop->tp_len == 0 && پای_ټکی_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_ستون += added;
	وإلا إذا (ستون + 1 < prop->tp_ستون + prop->tp_len + پای_ټکی_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    وإلا إذا (prop->tp_ستون > ستون + 1)
    {
	إذا (prop->tp_ستون + added < ستون + 1)
	{
	    إذا (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    وإلا
	    {
		prop->tp_len += (prop->tp_ستون - 1 - ستون) + added;
		prop->tp_ستون = ستون + 1;
		إذا (prop->tp_len <= 0)
		{
		    prop->tp_len = 0;
		    // Multiline properties with no text left should
		    // also be dropped.
		    res.can_drop = droppable
			|| (prop->tp_flags
				& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_بل));
		}
	    }
	}
	وإلا
	    prop->tp_ستون += added;
    }
    وإلا إذا (prop->tp_len > 0 && prop->tp_ستون + prop->tp_len > ستون
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	إذا (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = ستون - (prop->tp_ستون - 1);
	وإلا
	{
	    صحيح after = ستون - added - (prop->tp_ستون - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	إذا (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_بل)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_بل));
    }
    وإلا
	res.dirty = FALSE;

    أرجع res;
}

/*
 * Adjust the columns of text properties in line "lnum" after position "col" to
 * shift by "bytes_added" (can be negative).
 * Note that "col" is zero-based, while tp_col is one-based.
 * Only for the current buffer.
 * "flags" can have:
 * APC_SAVE_FOR_UNDO:	Call u_savesub() before making changes to the line.
 * APC_SUBSTITUTE:	Text is replaced, not inserted.
 * APC_INDENT:		Text is inserted before virtual text prop
 * Caller is expected to check b_has_textprop and "bytes_added" being non-zero.
 * Returns TRUE when props were changed.
 */
    صحيح
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    ستون,
	صحيح	    بایټونه_added,
	صحيح	    flags)
{
    unpacked_memline_T um;
    صحيح		dirty = FALSE;
    صحيح		ri;

    إذا (متن_prop_frozen > 0)
	أرجع FALSE;

    um = um_open_at(curbuf, lnum, 0);
    إذا (um.buf == عدم || um.prop_شمېر == 0)
    {
	um_abort(&um);
	أرجع FALSE;
    }

    لكل (ri = 0; ri < um.prop_شمېر; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, ستون, بایټونه_added, flags);
	إذا (res.dirty)
	{
	    إذا (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		إذا (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    أرجع FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		إذا ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    أرجع FALSE;
		}
	    }
	    dirty = TRUE;
	}
	إذا (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    استمر;
	}
    }
    إذا (dirty)
	um_close(&um);
    وإلا
	um_abort(&um);
    أرجع dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    فراغ
adjust_props_for_وېشل(
	linenr_T    lnum_props,
	linenr_T    lnum_سر,
	صحيح	    kept,
	صحيح	    deleted,
	صحيح	    at_eol)
{
    char_u	*props;
    صحيح		شمېر;
    garray_T    prevprop;
    garray_T    nextprop;
    صحيح		skipped = kept + deleted;

    إذا (!curbuf->b_has_textprop)
	أرجع;

    // Get the text properties from "lnum_props".
    شمېر = get_متن_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, حجم(textprop_T), 10);
    ga_init2(&nextprop, حجم(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *شمېر_ptr = props - PROP_شمېر_اندازه;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    لكل (صحيح i = 0; i < شمېر; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	صحيح	    پیل_incl, پای_ټکی_incl;
	صحيح	    cont_prev, cont_بل;
	صحيح	    prop_ستون;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * حجم(textprop_T), حجم(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	إذا (prop.tp_id < 0 && prop.u.tp_متن_دننه_واټن > 0)
	{
	    prop.u.tp_متن = vim_strsave(شمېر_ptr + prop.u.tp_متن_دننه_واټن);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	وإلا
	    prop.u.tp_متن = عدم;

	pt = متن_prop_type_by_id(curbuf, prop.tp_type);
	پیل_incl = (pt != عدم && (pt->pt_flags & PT_FLAG_INS_پیل_INCL));
	پای_ټکی_incl = (pt != عدم && (pt->pt_flags & PT_FLAG_INS_پای_ټکی_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_ستون = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_ستون;

	إذا (prop_ستون == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_بل = !at_eol;
	}
	وإلا
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    صحيح is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_ستون == MAXCOL);

	    cont_prev = prop_ستون + !پیل_incl <= kept;
	    إذا (is_floating_vtext)
		cont_بل = FALSE;
	    وإلا
		cont_بل = skipped <= prop_ستون + prop.tp_len - !پای_ټکی_incl;
	}
	// when a prop has text it is never copied
	إذا (prop.tp_id < 0 && cont_بل)
	    cont_prev = FALSE;

	إذا (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_معلومات) + prevprop.ga_len;
	    صحيح is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    إذا (!is_vtext && p->tp_ستون + p->tp_len >= kept)
		p->tp_len = kept - p->tp_ستون;
	    إذا (cont_بل)
		p->tp_flags |= TP_FLAG_CONT_بل;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	إذا (cont_بل && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_معلومات) + nextprop.ga_len;
	    صحيح is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    إذا (p->tp_ستون != MAXCOL)
	    {
		إذا (p->tp_ستون > skipped)
		    p->tp_ستون -= skipped - 1;
		وإلا
		{
		    إذا (!is_vtext)
			p->tp_len -= skipped - p->tp_ستون;
		    p->tp_ستون = 1;
		}
	    }
	    إذا (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    سېټ_متن_props(lnum_سر, (textprop_T *)prevprop.ga_معلومات,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    سېټ_متن_props(lnum_سر + 1, (textprop_T *)nextprop.ga_معلومات,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    فراغ
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	صحيح		    وروستی_کرښه,
	اوږد		    ستون,
	صحيح		    removed)
{
    unpacked_memline_T	r_um;

    إذا (um->buf == عدم)
	أرجع;

    r_um = um_open_at(um->buf, lnum, 0);
    إذا (r_um.buf == عدم)
	أرجع;
    إذا (!um_detach(&r_um))
    {
	um_abort(&r_um);
	أرجع;
    }

    لكل (صحيح i = r_um.prop_شمېر - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	صحيح	    پای_ټکی;

	إذا (prop->tp_ستون == MAXCOL && !وروستی_کرښه)
	    استمر;  // drop floating text for non-last lines
	پای_ټکی = !(prop->tp_flags & TP_FLAG_CONT_بل);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, ستون, 0);

	إذا (وروستی_کرښه || پای_ټکی)
	{
	    um_جمع_کړه_prop(um, prop);
	    prop->u.tp_متن = عدم;  // ownership transferred
	}
	وإلا
	{
	    // Search for continuing prop in um.
	    bool    وموندل = false;

	    لكل (صحيح j = 0; j < um->prop_شمېر; ++j)
	    {
		textprop_T *op = &um->props[j];

		إذا ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    وموندل = true;
		    op->tp_len += op->tp_ستون - prop->tp_ستون;
		    op->tp_ستون = prop->tp_ستون;
		    op->tp_flags = prop->tp_flags;
		    اكسر;
		}
	    }
	    إذا (!وموندل)
		دننی_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    ساكن فراغ
remember_cleared_prop(garray_T *gap, صحيح which, textprop_T *prop, صحيح flag)
{
    undoprop_T	*پورته;

    إذا (which == UNDOPROP_NONE)
	أرجع;
    إذا (gap->ga_itemsize == 0)
	ga_init2(gap, حجم(undoprop_T), 4);
    إذا (ga_grow(gap, 1) == FAIL)
	أرجع;
    پورته = (undoprop_T *)gap->ga_معلومات + gap->ga_len++;
    پورته->پورته_which = which;
    پورته->پورته_type = prop->tp_type;
    پورته->پورته_id = prop->tp_id;
    پورته->پورته_ستون = prop->tp_ستون;
    پورته->پورته_flag = flag;
}

/*
 * Clear the continuation flags between line "lnum" and the line below it that
 * have no partner in the other line.  A property is split there then.  What
 * is cleared on the line above is remembered in "gap" as "above_which", on
 * the line below as "below_which".
 */
    فراغ
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	صحيح	    above_which,
	صحيح	    لاندې_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	لاندې;
    bool		*paired = عدم;
    bool		above_بدل_شوی = false;
    bool		لاندې_بدل_شوی = false;
    bool		cont_بل = false;

    إذا (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_کرښه_شمېر)
	أرجع;

    above = um_open_at(buf, lnum, 0);
    إذا (above.buf == عدم)
	أرجع;
    لكل (صحيح i = 0; i < above.prop_شمېر; ++i)
	إذا (above.props[i].tp_flags & TP_FLAG_CONT_بل)
	    cont_بل = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    إذا (cont_بل && !um_detach(&above))
    {
	um_abort(&above);
	أرجع;
    }

    لاندې = um_open_at(buf, lnum + 1, 0);
    إذا (لاندې.buf == عدم)
    {
	um_abort(&above);
	أرجع;
    }
    إذا (لاندې.prop_شمېر > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, لاندې.prop_شمېر);
	إذا (paired == عدم)
	    اذهب theend;
    }

    لكل (صحيح i = 0; i < above.prop_شمېر; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    وموندل = false;

	إذا (!(prop->tp_flags & TP_FLAG_CONT_بل))
	    استمر;
	لكل (صحيح j = 0; j < لاندې.prop_شمېر; ++j)
	{
	    textprop_T *بل = &لاندې.props[j];

	    إذا (!paired[j] && (بل->tp_flags & TP_FLAG_CONT_PREV)
		    && بل->tp_id == prop->tp_id
		    && بل->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		وموندل = true;
		اكسر;
	    }
	}
	إذا (!وموندل)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_بل;
	    above_بدل_شوی = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_بل);
	}
    }

    لكل (صحيح j = 0; j < لاندې.prop_شمېر; ++j)
    {
	إذا (!(لاندې.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    استمر;
	إذا (!لاندې.detached && !um_detach(&لاندې))
	    اكسر;
	لاندې.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	لاندې_بدل_شوی = true;
	remember_cleared_prop(gap, لاندې_which, &لاندې.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    إذا (لاندې_بدل_شوی)
	um_close(&لاندې);
    وإلا
	um_abort(&لاندې);
    إذا (above_بدل_شوی)
	um_close(&above);
    وإلا
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    فراغ
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	صحيح	    which)
{
    unpacked_memline_T	um;
    bool		بدل_شوی = false;

    إذا (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_کرښه_شمېر)
	أرجع;
    um = um_open_at(buf, lnum, 0);
    إذا (um.buf == عدم)
	أرجع;
    لكل (صحيح r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *پورته = (undoprop_T *)gap->ga_معلومات + r;

	إذا (پورته->پورته_which != which)
	    استمر;
	لكل (صحيح i = 0; i < um.prop_شمېر; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    إذا (prop->tp_type != پورته->پورته_type || prop->tp_id != پورته->پورته_id
		    || prop->tp_ستون != پورته->پورته_ستون
		    || (prop->tp_flags & پورته->پورته_flag))
		استمر;
	    إذا (!um.detached && !um_detach(&um))
		اذهب theend;
	    um.props[i].tp_flags |= پورته->پورته_flag;
	    بدل_شوی = true;
	    اكسر;
	}
    }

theend:
    إذا (بدل_شوی)
	um_close(&um);
    وإلا
	um_abort(&um);
}

    bool
متن_prop_شمېر_valid(صحيح prop_شمېر, size_t propdata_len)
{
    إذا (propdata_len < PROP_شمېر_اندازه)
	أرجع false;
    أرجع (size_t)prop_شمېر * حجم(textprop_T)
		    <= propdata_len - PROP_شمېر_اندازه;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
متن_prop_vtext_valid(char_u *props, صحيح prop_شمېر, size_t propdata_len)
{
    لكل (صحيح i = 0; i < prop_شمېر; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * حجم(textprop_T),
							  حجم(textprop_T));
	إذا (prop.tp_id >= 0 || prop.u.tp_متن_دننه_واټن <= 0)
	    استمر;
	إذا (prop.tp_len < 0 || (size_t)prop.u.tp_متن_دننه_واټن
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    أرجع false;
    }
    أرجع true;
}

#د_که_پای // FEAT_PROP_POPUP
