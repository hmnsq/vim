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

#شامل "vim.h"

#اگر defined(FEAT_PROP_POPUP)

ساکن خالی um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    ساکن خالی
um_free_vtext(textprop_T *props, صحیح گنتی)
{
    برائے (صحیح i = 0; i < گنتی; ++i)
	اگر (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_متن);
}

/*
 * Free memory used by an unpacked memline.
 */
    ساکن خالی
um_free(unpacked_memline_T *um)
{
    اگر (um->detached)
    {
	VIM_CLEAR(um->متن);
	um_free_vtext(um->props, um->prop_گنتی);
    }
    VIM_CLEAR(um->props);
    um->prop_سائز = 0;
    um->prop_گنتی = 0;
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

    CLEAR_میدان(um);
    um.buf = buf;
    واپس um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_سطر(unpacked_memline_T *um, linenr_T lnum, صحیح extra_props)
{
    char_u	*سطر;
    size_t	textlen;
    size_t	propdata_len;
    صحیح		proplen;

    اگر (um->buf == تہی)
	واپس false;

    // Store changes to the current line if any.
    اگر (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    اگر (lnum == 0)
	واپس true;	// just unload

    سطر = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_سطر_len - textlen;

    um->lnum = lnum;
    um->متن_سائز_2 = (colnr_T)textlen;
    um->متن = سطر;

    اگر (propdata_len == 0)
	واپس true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    اگر (propdata_len < PROP_گنتی_سائز + حجم(textprop_T))
    {
	emsg(e_متن_property_info_corrupted);
	um->buf = تہی;
	واپس false;
    }

    uint16_t    prop_گنتی;
    char_u	    *گنتی_ptr = سطر + textlen;
    char_u	    *props_آغاز;

    mch_memmove(&prop_گنتی, گنتی_ptr, PROP_گنتی_سائز);
    اگر (!متن_prop_گنتی_valid(prop_گنتی, propdata_len))
    {
	emsg(e_متن_property_info_corrupted);
	um->buf = تہی;
	واپس false;
    }
    proplen = (صحیح)prop_گنتی;
    props_آغاز = گنتی_ptr + PROP_گنتی_سائز;

    اگر (!متن_prop_vtext_valid(props_آغاز, proplen, propdata_len))
    {
	emsg(e_متن_property_info_corrupted);
	um->buf = تہی;
	واپس false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    اگر (um->props == تہی)
    {
	um->buf = تہی;
	واپس false;
    }
    um->prop_سائز = proplen + extra_props;
    um->prop_گنتی = proplen;
    mch_memmove(um->props, props_آغاز, proplen * حجم(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    برائے (صحیح i = 0; i < proplen; ++i)
    {
	اگر (um->props[i].tp_id < 0 && um->props[i].u.tp_متن_انحراف > 0)
	{
	    um->props[i].u.tp_متن = گنتی_ptr + um->props[i].u.tp_متن_انحراف;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ورنہ
	    um->props[i].u.tp_متن = تہی;
    }

    واپس true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, صحیح extra_props)
{
    unpacked_memline_T um = um_open(buf);

    اگر (!um_goto_سطر(&um, lnum, extra_props))
	um.buf = تہی;
    واپس um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    ساکن bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    اگر (um->detached || um->buf == تہی)
	واپس um->detached;

    newtext = vim_strnsave(um->متن, um->متن_سائز_2 - 1);
    اگر (newtext == تہی)
    {
	um->buf = تہی;
	واپس false;
    }
    um->متن = newtext;

    برائے (صحیح i = 0; i < um->prop_گنتی; ++i)
    {
	اگر (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_متن);

	    اگر (copy == تہی)
	    {
		برائے (صحیح j = 0; j < i; ++j)
		    اگر (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_متن);
		VIM_CLEAR(um->متن);
		um->buf = تہی;
		واپس false;
	    }
	    um->props[i].u.tp_متن = copy;
	}
    }

    um->detached = true;
    واپس true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_سیٹ_متن(unpacked_memline_T *um, char_u *متن)
{
    اگر (um->buf == تہی)
	واپس false;
    اگر (um->detached)
	vim_free(um->متن);
    um->متن = متن;
    um->متن_سائز_2 = (colnr_T)STRLEN(متن) + 1;
    um->متن_بدلا = true;
    um->detached = true;
    واپس true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    ساکن bool
um_grow_props(unpacked_memline_T *um, صحیح needed)
{
    اگر (um->prop_گنتی + needed <= um->prop_سائز)
	واپس true;

    صحیح	    new_سائز = um->prop_گنتی + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_سائز * حجم(textprop_T));
    اگر (newprops == تہی)
	واپس false;
    um->props = newprops;
    um->prop_سائز = new_سائز;

    واپس true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    ساکن خالی
um_جوڑو_prop(unpacked_memline_T *um, textprop_T *prop)
{
    اگر (um->buf == تہی)
	واپس;
    اگر (!um->detached && !um_detach(um))
	واپس;
    اگر (!um_grow_props(um, 1))
    {
	um->buf = تہی;
	واپس;
    }
    um->props[um->prop_گنتی++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    خالی
um_الٹ_props(unpacked_memline_T *um)
{
    برائے (صحیح i = 0, j = um->prop_گنتی - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, صحیح prop_گنتی)
{
    unpacked_memline_T um;

    CLEAR_میدان(um);
    um.buf = buf;

    اگر (lnum < 1 || lnum > buf->b_ml.ml_سطر_گنتی)
    {
	um.buf = تہی;
	واپس um;
    }

    char_u *سطر = ml_get_buf(buf, lnum, FALSE);
    char_u *متن_copy = vim_strsave(سطر);

    اگر (متن_copy == تہی)
    {
	um.buf = تہی;
	واپس um;
    }
    um.lnum = lnum;
    um.متن = متن_copy;
    um.متن_سائز_2 = (colnr_T)STRLEN(متن_copy) + 1;
    um.detached = true;

    اگر (prop_گنتی > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_گنتی);
	اگر (um.props == تہی)
	{
	    vim_free(um.متن);
	    um.buf = تہی;
	    واپس um;
	}
	um.prop_سائز = prop_گنتی;
    }

    واپس um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    ساکن خالی
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					صحیح tp_id, صحیح tp_type, صحیح flag)
{
    unpacked_memline_T	neighbor;

    اگر (lnum < 1 || lnum > buf->b_ml.ml_سطر_گنتی)
	واپس;

    neighbor = um_open_at(buf, lnum, 0);
    اگر (neighbor.buf == تہی)
	واپس;

    برائے (صحیح i = 0; i < neighbor.prop_گنتی; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	اگر (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    اگر (!neighbor.detached && !um_detach(&neighbor))
		توڑو;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    توڑو;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    خالی
um_delete_prop(unpacked_memline_T *um, صحیح اشاریہ)
{
    textprop_T *prop;

    اگر (um->buf == تہی || اشاریہ < 0 || اشاریہ >= um->prop_گنتی)
	واپس;
    اگر (!um->detached && !um_detach(um))
	واپس;

    prop = &um->props[اشاریہ];

    // Adjust continuation flags on neighboring lines before deleting.
    اگر (prop->tp_flags & TP_FLAG_CONT_پچھلا)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_اگلا);
    اگر (prop->tp_flags & TP_FLAG_CONT_اگلا)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_پچھلا);

    اگر (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
    ساکن char_u *
um_pack(unpacked_memline_T *um, صحیح *packed_len)
{
    uint16_t	زندہ_گنتی = 0;
    صحیح		vtext_سائز = 0;
    صحیح		کل_len;
    char_u	*buf;
    char_u	*گنتی_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    برائے (صحیح i = 0; i < um->prop_گنتی; ++i)
    {
	textprop_T *prop = &um->props[i];

	اگر (prop->tp_flags & TP_FLAG_DELETED)
	    جاری;
	++زندہ_گنتی;
	اگر (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_سائز += prop->tp_len + 1;
    }

    اگر (زندہ_گنتی == 0)
    {
	// No properties: just text, no prop_count header.
	کل_len = um->متن_سائز_2;
	buf = alloc(کل_len);
	اگر (buf == تہی)
	    واپس تہی;
	*packed_len = کل_len;
	mch_memmove(buf, um->متن, um->متن_سائز_2);
	واپس buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    کل_len = um->متن_سائز_2 + (صحیح)PROP_گنتی_سائز
		+ زندہ_گنتی * (صحیح)حجم(textprop_T) + vtext_سائز;
    buf = alloc(کل_len);
    اگر (buf == تہی)
	واپس تہی;
    *packed_len = کل_len;

    mch_memmove(buf, um->متن, um->متن_سائز_2);

    گنتی_dest = buf + um->متن_سائز_2;
    mch_memmove(گنتی_dest, &زندہ_گنتی, PROP_گنتی_سائز);

    prop_dest = گنتی_dest + PROP_گنتی_سائز;
    vtext_dest = prop_dest + زندہ_گنتی * حجم(textprop_T);

    برائے (صحیح i = 0; i < um->prop_گنتی; ++i)
    {
	textprop_T prop = um->props[i];

	اگر (prop.tp_flags & TP_FLAG_DELETED)
	    جاری;

	اگر (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    صحیح copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_متن, copysize);
	    prop.u.tp_متن_انحراف = (colnr_T)(vtext_dest - گنتی_dest);
	    vtext_dest += copysize;
	}
	ورنہ
	    prop.u.tp_متن_انحراف = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, حجم(textprop_T));
	prop_dest += حجم(textprop_T);
    }

    واپس buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    ساکن خالی
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    صحیح	    packed_len;

    اگر (!um->detached || um->buf == تہی || um->lnum == 0)
	واپس;

    packed = um_pack(um, &packed_len);
    اگر (packed == تہی)
    {
	um->buf = تہی;
	واپس;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    اگر (um->buf->b_ml.ml_سطر_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    ورنہ اگر (!(um->buf->b_ml.ml_flags & ML_سطر_DIRTY))
	(خالی)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->متن);
    um_free_vtext(um->props, um->prop_گنتی);

    اگر (um->buf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_سطر_ptr);
    um->buf->b_ml.ml_سطر_ptr = packed;
    um->buf->b_ml.ml_سطر_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_سطر_DIRTY;

    um->detached = false;
    um->متن = packed;
    um->متن_سائز_2 = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_سطر_textlen = um->متن_سائز_2;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    خالی
um_close(unpacked_memline_T *um)
{
    اگر (um->buf == تہی)
	واپس;
    اگر (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = تہی;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    خالی
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = تہی;
}

/*
 * In a hashtable item "hi_key" points to "pt_name" in a proptype_T.
 * This avoids adding a pointer to the hashtable item.
 * PT2HIKEY() converts a proptype pointer to a hashitem key pointer.
 * HIKEY2PT() converts a hashitem key pointer to a proptype pointer.
 * HI2PT() converts a hashitem pointer to a proptype pointer.
 */
#تعریف_کرو PT2HIKEY(p)  ((p)->pt_name)
#تعریف_کرو HIKEY2PT(p)   ((proptype_T *)((p) - offsetof(proptype_T, pt_name)))
#تعریف_کرو HI2PT(hi)      HIKEY2PT((hi)->hi_کلید)

// The global text property types.
ساکن hashtab_T *global_proptypes = تہی;
ساکن proptype_T **global_proparray = تہی;

// The last used text property type ID.
ساکن صحیح proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    ساکن hashitem_T *
ڈھونڈو_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    اگر (*name == NUL)
	واپس تہی;
    اگر (buf == تہی)
	ht = global_proptypes;
    ورنہ
	ht = buf->b_proptypes;

    اگر (ht == تہی)
	واپس تہی;
    hi = hash_ڈھونڈو(ht, name);
    اگر (HASHITEM_خالی_خانے(hi))
	واپس تہی;
    واپس hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    ساکن proptype_T *
ڈھونڈو_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = ڈھونڈو_prop_type_hi(name, buf);

    اگر (hi == تہی)
	واپس تہی;
    واپس HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    صحیح
ڈھونڈو_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = ڈھونڈو_prop_type(name, buf);

    اگر (pt == تہی)
	واپس 0;
    واپس pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    ساکن proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = ڈھونڈو_prop_type(name, buf);

    اگر (type == تہی)
	type = ڈھونڈو_prop_type(name, تہی);
    اگر (type == تہی)
	semsg(_(e_property_type_str_does_not_exist), name);
    واپس type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    ساکن صحیح
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    اگر (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	واپس FAIL;
    }
    اگر (arg->vval.v_dict == تہی)
	واپس OK;  // NULL dict is like an empty dict
    di = dict_ڈھونڈو(arg->vval.v_dict, (char_u *)"bufnr", -1);
    اگر (di != تہی && (di->di_tv.v_type != VAR_نمبر
					      || di->di_tv.vval.v_نمبر != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	اگر (*buf == تہی)
	    واپس FAIL;
    }
    واپس OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    خالی
f_prop_جوڑو(typval_T *argvars, typval_T *rettv)
{
    linenr_T	آغاز_lnum;
    colnr_T	آغاز_ستون;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	واپس;

    آغاز_lnum = tv_get_نمبر(&argvars[0]);
    آغاز_ستون = tv_get_نمبر(&argvars[1]);
    اگر (check_for_dict_arg(argvars, 2) == FAIL)
	واپس;

    rettv->vval.v_نمبر = prop_جوڑو_common(آغاز_lnum, آغاز_ستون,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    ساکن صحیح
prop_جوڑو_one(
	buf_T		*buf,
	char_u		*type_name,
	صحیح		id,
	char_u		*متن_arg,
	صحیح		متن_padding_بایاں,
	صحیح		متن_flags,
	linenr_T	آغاز_lnum,
	linenr_T	انتہا_lnum,
	colnr_T		آغاز_ستون,
	colnr_T		انتہا_ستون)
{
    proptype_T	*type;
    linenr_T	lnum;
    صحیح		proplen;
    char_u	*props = تہی;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    صحیح		i;
    textprop_T	tmp_prop;
    char_u	*متن = متن_arg;
    صحیح		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    اگر (type == تہی)
	جاؤ theend;

    اگر (آغاز_lnum < 1 || آغاز_lnum > buf->b_ml.ml_سطر_گنتی)
    {
	semsg(_(e_invalid_سطر_نمبر_nr), (طویل)آغاز_lnum);
	جاؤ theend;
    }
    اگر (انتہا_lnum < آغاز_lnum || انتہا_lnum > buf->b_ml.ml_سطر_گنتی)
    {
	semsg(_(e_invalid_سطر_نمبر_nr), (طویل)انتہا_lnum);
	جاؤ theend;
    }

    اگر (buf->b_ml.ml_mfp == تہی)
    {
	emsg(_(e_cannot_جوڑو_متن_property_مقصد_unloaded_بفر));
	جاؤ theend;
    }

    اگر (متن != تہی)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	برائے (p = متن; *p != NUL; MB_PTR_ADV(p))
	    اگر (*p < ' ')
		*p = ' ';
    }

    برائے (lnum = آغاز_lnum; lnum <= انتہا_lnum; ++lnum)
    {
	colnr_T ستون;	    // start column use in tp_col
	colnr_T ترتیب_دو_ستون;   // column where it appears
	طویل	لمبائی;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_متن_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	اگر (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_متن_properties_on_a_single_سطر));
	    جاؤ theend;
	}

	اگر (lnum == آغاز_lnum)
	    ستون = آغاز_ستون;
	ورنہ
	    ستون = 1;
	اگر (ستون - 1 > (colnr_T)textlen && !(ستون == 0 && متن_arg != تہی))
	{
	    semsg(_(e_invalid_عمود_نمبر_nr), (طویل)آغاز_ستون);
	    جاؤ theend;
	}
	ترتیب_دو_ستون = ستون;

	اگر (lnum == انتہا_lnum)
	    لمبائی = انتہا_ستون - ستون;
	ورنہ
	    لمبائی = (صحیح)textlen - ستون + 1;
	اگر (لمبائی > (طویل)textlen)
	    لمبائی = (صحیح)textlen;	// can include the end-of-line
	اگر (لمبائی < 0)
	    لمبائی = 0;		// zero-width property

	اگر (متن_arg != تہی)
	{
	    لمبائی = (طویل)STRLEN(متن_arg);
	    اگر (ستون == 0)
	    {
		ستون = MAXCOL;	// before or after the line
		اگر ((متن_flags & TP_FLAG_ALIGN_ABOVE) == 0)
		    ترتیب_دو_ستون = MAXCOL;
	    }
	}

	// Build the new property.
	CLEAR_میدان(tmp_prop);
	tmp_prop.tp_ستون = ستون;
	tmp_prop.tp_len = لمبائی;
	tmp_prop.tp_id = id;
	tmp_prop.tp_type = type->pt_id;
	tmp_prop.tp_flags = متن_flags
			    | (lnum > آغاز_lnum ? TP_FLAG_CONT_پچھلا : 0)
			    | (lnum < انتہا_lnum ? TP_FLAG_CONT_اگلا : 0)
			    | ((type->pt_flags & PT_FLAG_INS_آغاز_INCL)
						     ? TP_FLAG_آغاز_INCL : 0);
	tmp_prop.tp_padleft = متن_padding_بایاں;
	اگر (متن_arg != تہی)
	    tmp_prop.u.tp_متن = متن_arg;

	// Find the index where to insert the new property.
	برائے (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_ستون;

	    mch_memmove(&existing, props + i * حجم(textprop_T),
							   حجم(textprop_T));
	    prop_ستون = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_ستون;
	    اگر (prop_ستون >= ترتیب_دو_ستون)
		توڑو;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	صحیح		vtext_کل = 0;
	صحیح		new_سطر_len;
	char_u	*گنتی_dest;
	char_u	*vtext_dest;
	صحیح		j;

	// Compute total vtext size from existing props.
	برائے (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * حجم(textprop_T),
							حجم(textprop_T));
	    اگر (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_کل += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	اگر (متن_arg != تہی)
	    vtext_کل += لمبائی + 1;

	new_سطر_len = (صحیح)textlen + (صحیح)PROP_گنتی_سائز
			+ new_propcount * (صحیح)حجم(textprop_T)
			+ vtext_کل;
	newtext = alloc(new_سطر_len);
	اگر (newtext == تہی)
	    جاؤ theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_سطر_ptr, textlen);

	// Write prop_count.
	گنتی_dest = newtext + textlen;
	mch_memmove(گنتی_dest, &new_propcount, PROP_گنتی_سائز);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = گنتی_dest + PROP_گنتی_سائز;
	اگر (i > 0)
	    mch_memmove(newprops, props, حجم(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	اگر (i < proplen)
	    mch_memmove(newprops + (i + 1) * حجم(textprop_T),
					props + i * حجم(textprop_T),
					حجم(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * حجم(textprop_T);
	برائے (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    اگر (j == i)
		جاری;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * حجم(textprop_T),
		    حجم(textprop_T));
	    اگر (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_گنتی_2 = (char_u *)props - PROP_گنتی_سائز;
		char_u *old_vtext = old_گنتی_2 + ep.u.tp_متن_انحراف;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_متن_انحراف = (colnr_T)(vtext_dest - گنتی_dest);
		mch_memmove(newprops + j * حجم(textprop_T), &ep,
			حجم(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	اگر (متن_arg != تہی)
	{
	    صحیح copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, متن_arg, copysize);
	    tmp_prop.u.tp_متن_انحراف =
				    (colnr_T)(vtext_dest - گنتی_dest);
	    vtext_dest += copysize;
	}
	ورنہ
	    tmp_prop.u.tp_متن_انحراف = 0;

	mch_memmove(newprops + i * حجم(textprop_T), &tmp_prop,
							حجم(textprop_T));

	اگر (buf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_سطر_ptr);
	buf->b_ml.ml_سطر_ptr = newtext;
	buf->b_ml.ml_سطر_len = new_سطر_len;
	buf->b_ml.ml_flags |= ML_سطر_DIRTY;
    }

    بدلا_سطر_display_buf(buf);
    بدلا_سطریں_buf(buf, آغاز_lnum, انتہا_lnum + 1, 0);
    res = OK;

theend:
    vim_free(متن);
    واپس res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    خالی
f_prop_جوڑو_list(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    صحیح		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	آغاز_lnum;
    colnr_T	آغاز_ستون;
    linenr_T	انتہا_lnum;
    colnr_T	انتہا_ستون;
    صحیح		error = FALSE;
    صحیح		پچھلا_did_emsg = did_emsg;

    اگر (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	واپس;

    اگر (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	واپس;

    dict = argvars[0].vval.v_dict;
    اگر (dict == تہی || !dict_has_کلید(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	واپس;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    اگر (dict_has_کلید(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_نمبر(dict, "id");
	اگر (x > زیادہ_سے_زیادہ_عدد || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    واپس;
	}
	id = (صحیح)x;
    }

    اگر (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	واپس;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_اشیاء(argvars[1].vval.v_list, li)
    {
	اگر (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == تہی)
	{
	    emsg(_(e_list_required));
	    واپس;
	}

	pos_list = li->li_tv.vval.v_list;
	آغاز_lnum = list_ڈھونڈو_nr(pos_list, 0L, &error);
	اگر (!error)
	    آغاز_ستون = list_ڈھونڈو_nr(pos_list, 1L, &error);
	اگر (!error)
	    انتہا_lnum = list_ڈھونڈو_nr(pos_list, 2L, &error);
	اگر (!error)
	    انتہا_ستون = list_ڈھونڈو_nr(pos_list, 3L, &error);
	صحیح this_id = id;
	اگر (!error && pos_list->lv_len > 4)
	    this_id = list_ڈھونڈو_nr(pos_list, 4L, &error);
	اگر (error || آغاز_lnum <= 0 || آغاز_ستون <= 0
		  || انتہا_lnum <= 0 || انتہا_ستون <= 0)
	{
	    اگر (پچھلا_did_emsg == did_emsg)
		emsg(_(e_invalid_دلیل));
	    واپس;
	}
	اگر (prop_جوڑو_one(buf, type_name, this_id, تہی, 0, 0,
			     آغاز_lnum, انتہا_lnum, آغاز_ستون, انتہا_ستون) == FAIL)
	    واپس;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
ساکن صحیح vt_id_شمار = 0;

/*
 * Get the next ID to use for a textprop.
 */
    ساکن صحیح
get_textprop_id(خالی)
{
    اگر (vt_id_شمار > 0)
	vt_id_شمار = 0;
    واپس --vt_id_شمار;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    صحیح
prop_جوڑو_common(
	linenr_T    آغاز_lnum,
	colnr_T	    آغاز_ستون,
	dict_T	    *dict,
	buf_T	    *default_buf,
	typval_T    *dict_arg)
{
    linenr_T	انتہا_lnum;
    colnr_T	انتہا_ستون;
    char_u	*type_name;
    buf_T	*buf = default_buf;
    صحیح		id = 0;
    char_u	*متن = تہی;
    صحیح		متن_padding_بایاں = 0;
    صحیح		flags = 0;

    اگر (dict == تہی || !dict_has_کلید(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	جاؤ theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    اگر (dict_has_کلید(dict, "end_lnum"))
    {
	انتہا_lnum = dict_get_نمبر(dict, "end_lnum");
	اگر (انتہا_lnum < آغاز_lnum)
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str), "end_lnum");
	    جاؤ theend;
	}
    }
    ورنہ
	انتہا_lnum = آغاز_lnum;

    اگر (dict_has_کلید(dict, "length"))
    {
	طویل لمبائی = dict_get_نمبر(dict, "length");

	اگر (لمبائی < 0 || انتہا_lnum > آغاز_lnum)
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str), "length");
	    جاؤ theend;
	}
	انتہا_ستون = آغاز_ستون + لمبائی;
    }
    ورنہ اگر (dict_has_کلید(dict, "end_col"))
    {
	انتہا_ستون = dict_get_نمبر(dict, "end_col");
	اگر (انتہا_ستون <= 0)
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str), "end_col");
	    جاؤ theend;
	}
    }
    ورنہ اگر (آغاز_lnum == انتہا_lnum)
	انتہا_ستون = آغاز_ستون;
    ورنہ
	انتہا_ستون = 1;

    اگر (dict_has_کلید(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_نمبر(dict, "id");
	اگر (x > زیادہ_سے_زیادہ_عدد || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    جاؤ theend;
	}
	id = (صحیح)x;
    }

    اگر (dict_has_کلید(dict, "text"))
    {
	اگر (dict_has_کلید(dict, "length")
		|| dict_has_کلید(dict, "end_col")
		|| dict_has_کلید(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_لمبائی_endcol_and_endlnum_with_متن));
	    جاؤ theend;
	}

	متن = dict_get_string(dict, "text", TRUE);
	اگر (متن == تہی)
	    جاؤ theend;
	// use a default length of 1 to make multiple props show up
	انتہا_ستون = آغاز_ستون + 1;

	اگر (dict_has_کلید(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    اگر (p == تہی)
		جاؤ theend;
	    اگر (آغاز_ستون != 0)
	    {
		emsg(_(e_can_only_use_متن_align_when_عمود_is_zero));
		جاؤ theend;
	    }
	    اگر (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_دایاں;
	    ورنہ اگر (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    ورنہ اگر (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_نیچے;
	    ورنہ اگر (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str_str), "text_align", p);
		جاؤ theend;
	    }
	}

	اگر (dict_has_کلید(dict, "text_padding_left"))
	{
	    متن_padding_بایاں = dict_get_نمبر(dict, "text_padding_left");
	    اگر (متن_padding_بایاں < 0)
	    {
		semsg(_(e_دلیل_must_be_مثبت_str), "text_padding_left");
		جاؤ theend;
	    }
	}

	اگر (dict_has_کلید(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    اگر (p == تہی)
		جاؤ theend;
	    اگر (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    ورنہ اگر (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str_str), "text_wrap", p);
		جاؤ theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    اگر (آغاز_ستون < (متن == تہی ? 1 : 0))
    {
	semsg(_(e_invalid_عمود_نمبر_nr), (طویل)آغاز_ستون);
	جاؤ theend;
    }
    اگر (آغاز_ستون > 0 && متن_padding_بایاں > 0)
    {
	emsg(_(e_can_only_use_بایاں_padding_when_عمود_is_zero));
	جاؤ theend;
    }

    اگر (dict_arg != تہی && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	جاؤ theend;

    اگر (متن != تہی)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    ورنہ اگر (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	جاؤ theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_جوڑو_one(buf, type_name, id, متن, متن_padding_بایاں, flags,
				    آغاز_lnum, انتہا_lnum, آغاز_ستون, انتہا_ستون);
    متن = تہی;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(متن);
    واپس id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    صحیح
get_متن_props(buf_T *buf, linenr_T lnum, char_u **props, صحیح will_change)
{
    char_u	*متن;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_گنتی;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    اگر ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == تہی)
	واپس 0;

    // Fetch the line to get the ml_line_len field updated.
    متن = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_سطر_len - textlen;
    اگر (propdata_len == 0)
	واپس 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    اگر (propdata_len < PROP_گنتی_سائز + حجم(textprop_T))
    {
	emsg(e_متن_property_info_corrupted);
	واپس 0;
    }
    mch_memmove(&prop_گنتی, متن + textlen, PROP_گنتی_سائز);
    اگر (!متن_prop_گنتی_valid(prop_گنتی, propdata_len))
    {
	emsg(e_متن_property_info_corrupted);
	واپس 0;
    }
    اگر (!متن_prop_vtext_valid(متن + textlen + PROP_گنتی_سائز,
					   (صحیح)prop_گنتی, propdata_len))
    {
	emsg(e_متن_property_info_corrupted);
	واپس 0;
    }
    *props = متن + textlen + PROP_گنتی_سائز;
    واپس (صحیح)prop_گنتی;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    صحیح
prop_گنتی_above_نیچے(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    صحیح		گنتی = get_متن_props(buf, lnum, &props, FALSE);
    صحیح		نتیجہ = 0;
    textprop_T	prop;
    صحیح		i;
    صحیح		اگلا_دایاں_goes_نیچے = FALSE;

    اگر (گنتی == 0)
	واپس 0;
    برائے (i = 0; i < گنتی; ++i)
    {
	mch_memmove(&prop, props + i * حجم(prop), حجم(prop));
	اگر (prop.tp_ستون == MAXCOL && متن_prop_type_valid(buf, &prop))
	{
	    اگر ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_نیچے))
		    || (اگلا_دایاں_goes_نیچے
				     && (prop.tp_flags & TP_FLAG_ALIGN_دایاں)))
	    {
		++نتیجہ;
	    }
	    ورنہ اگر (prop.tp_flags & TP_FLAG_ALIGN_دایاں)
		اگلا_دایاں_goes_نیچے = TRUE;
	}
    }
    واپس نتیجہ;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    صحیح
گنتی_props(linenr_T lnum, صحیح only_starting, صحیح آخری_سطر)
{
    char_u	*props;
    صحیح		proplen = get_متن_props(curbuf, lnum, &props, 0);
    صحیح		نتیجہ = proplen;
    textprop_T	prop;

    برائے (صحیح i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * حجم(prop), حجم(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	اگر ((only_starting && (prop.tp_flags & TP_FLAG_CONT_پچھلا))
		|| (!آخری_سطر && prop.tp_ستون == MAXCOL))
	    --نتیجہ;
    }
    واپس نتیجہ;
}

ساکن textprop_T	*متن_prop_compare_props;
ساکن buf_T		*متن_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    ساکن صحیح
متن_prop_ترتیب(صحیح flags)
{
    اگر (flags & TP_FLAG_ALIGN_ABOVE)
	واپس 0;
    اگر (flags & TP_FLAG_ALIGN_دایاں)
	واپس 2;
    اگر (flags & TP_FLAG_ALIGN_نیچے)
	واپس 3;
    واپس 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    ساکن صحیح
متن_prop_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    صحیح  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(صحیح *)s1;
    idx2 = *(صحیح *)s2;
    tp1 = &متن_prop_compare_props[idx1];
    tp2 = &متن_prop_compare_props[idx2];
    col1 = tp1->tp_ستون;
    col2 = tp2->tp_ستون;

    // property that inserts text has priority over one that doesn't
    اگر ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	واپس tp1->tp_id < 0 ? 1 : -1;

    اگر (col1 == MAXCOL || col2 == MAXCOL)
    {
	صحیح order1 = متن_prop_ترتیب(tp1->tp_flags);
	صحیح order2 = متن_prop_ترتیب(tp2->tp_flags);

	// sort on order where it is added
	اگر (order1 != order2)
	    واپس order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = متن_prop_type_by_id(متن_prop_compare_buf, tp1->tp_type);
    pt2 = متن_prop_type_by_id(متن_prop_compare_buf, tp2->tp_type);
    اگر (pt1 != pt2)
    {
	اگر (pt1 == تہی)
	    واپس -1;
	اگر (pt2 == تہی)
	    واپس 1;
	اگر (pt1->pt_ترجیح != pt2->pt_ترجیح)
	    واپس pt1->pt_ترجیح > pt2->pt_ترجیح ? 1 : -1;
    }

    // same priority, one that starts first wins
    اگر (col1 != col2)
	واپس col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    اگر (tp1->tp_id < 0)
	واپس tp1->tp_id > tp2->tp_id ? 1 : -1;

    واپس 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    خالی
ترتیب_دو_متن_props(
	buf_T	    *buf,
	textprop_T  *props,
	صحیح	    *idxs,
	صحیح	    گنتی)
{
    متن_prop_compare_buf = buf;
    متن_prop_compare_props = props;
    qsort((خالی *)idxs, (size_t)گنتی, حجم(صحیح), متن_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
ڈھونڈو_prop_اندر_سطریں(
	win_T	    *wp,
	صحیح	    type_id,
	صحیح	    id,
	textprop_T  *prop,
	linenr_T    *ملا_lnum,
	linenr_T    پہلا_lnum,
	linenr_T    آخری_lnum)
{
    اگر (متن_prop_type_by_id(wp->w_بفر, type_id) == تہی)
	واپس false;

    اگر (پہلا_lnum < 1)
	پہلا_lnum = 1;
    اگر (آخری_lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
	آخری_lnum = wp->w_بفر->b_ml.ml_سطر_گنتی;

    برائے (linenr_T lnum = پہلا_lnum; lnum <= آخری_lnum; ++lnum)
    {
	char_u	*props;
	صحیح	گنتی = get_متن_props(wp->w_بفر, lnum, &props, FALSE);
	برائے (صحیح i = 0; i < گنتی; ++i)
	{
	    mch_memmove(prop, props + i * حجم(textprop_T),
							   حجم(textprop_T));
	    اگر (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*ملا_lnum = lnum;
		واپس true;
	    }
	}
    }
    واپس false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
ڈھونڈو_visible_prop(
	win_T	    *wp,
	صحیح	    type_id,
	صحیح	    id,
	textprop_T  *prop,
	linenr_T    *ملا_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    واپس ڈھونڈو_prop_اندر_سطریں(wp, type_id, id, prop, ملا_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    ساکن خالی
سیٹ_متن_props(linenr_T lnum, textprop_T *tps, صحیح گنتی)
{
    char_u  *متن;
    char_u  *newtext;
    صحیح	    textlen;

    متن = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    اگر (گنتی == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	اگر (newtext == تہی)
	    واپس;
	mch_memmove(newtext, متن, textlen);
	اگر (curbuf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_سطر_ptr);
	curbuf->b_ml.ml_سطر_ptr = newtext;
	curbuf->b_ml.ml_سطر_len = textlen;
	curbuf->b_ml.ml_flags |= ML_سطر_DIRTY;
    }
    ورنہ
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_گنتی = (uint16_t)گنتی;
	صحیح	    vtext_سائز = 0;
	صحیح	    کل_len;
	char_u	    *گنتی_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	صحیح	    i;

	برائے (i = 0; i < گنتی; ++i)
	    اگر (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_سائز += tps[i].tp_len + 1;

	کل_len = textlen + (صحیح)PROP_گنتی_سائز
		    + گنتی * (صحیح)حجم(textprop_T) + vtext_سائز;
	newtext = alloc(کل_len);
	اگر (newtext == تہی)
	    واپس;
	mch_memmove(newtext, متن, textlen);

	گنتی_dest = newtext + textlen;
	mch_memmove(گنتی_dest, &prop_گنتی, PROP_گنتی_سائز);

	prop_dest = گنتی_dest + PROP_گنتی_سائز;
	vtext_dest = prop_dest + گنتی * حجم(textprop_T);

	برائے (i = 0; i < گنتی; ++i)
	{
	    textprop_T prop = tps[i];

	    اگر (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		صحیح copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_متن, copysize);
		vim_free(tps[i].u.tp_متن);
		tps[i].u.tp_متن = تہی;
		prop.u.tp_متن_انحراف = (colnr_T)(vtext_dest - گنتی_dest);
		vtext_dest += copysize;
	    }
	    ورنہ
		prop.u.tp_متن_انحراف = 0;
	    mch_memmove(prop_dest, &prop, حجم(textprop_T));
	    prop_dest += حجم(textprop_T);
	}

	اگر (curbuf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_سطر_ptr);
	curbuf->b_ml.ml_سطر_ptr = newtext;
	curbuf->b_ml.ml_سطر_len = کل_len;
	curbuf->b_ml.ml_flags |= ML_سطر_DIRTY;
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
props_جوڑو_گنتی_سرخی(char_u *سطر, صحیح سطر_len, صحیح textlen, صحیح *new_len)
{
    صحیح		prop_بائٹیں = سطر_len - textlen;
    uint16_t	prop_گنتی;
    صحیح		vtext_سائز = 0;
    صحیح		کل;
    char_u	*newline;
    char_u	*گنتی_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    صحیح		i;

    اگر (prop_بائٹیں <= 0 || prop_بائٹیں % حجم(textprop_T) != 0)
    {
	*new_len = سطر_len;
	واپس تہی;
    }
    prop_گنتی = (uint16_t)(prop_بائٹیں / حجم(textprop_T));

    // Calculate total vtext size.
    برائے (i = 0; i < (صحیح)prop_گنتی; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, سطر + textlen + i * حجم(textprop_T),
							حجم(textprop_T));
	اگر (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_سائز += prop.tp_len + 1;
    }

    کل = textlen + (صحیح)PROP_گنتی_سائز + prop_بائٹیں + vtext_سائز;
    newline = alloc(کل);
    اگر (newline == تہی)
	واپس تہی;
    *new_len = کل;

    mch_memmove(newline, سطر, textlen);
    گنتی_dest = newline + textlen;
    mch_memmove(گنتی_dest, &prop_گنتی, PROP_گنتی_سائز);
    prop_dest = گنتی_dest + PROP_گنتی_سائز;
    vtext_dest = prop_dest + prop_بائٹیں;

    برائے (i = 0; i < (صحیح)prop_گنتی; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, سطر + textlen + i * حجم(textprop_T),
							حجم(textprop_T));
	اگر (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    صحیح copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_متن, copysize);
	    vim_free(prop.u.tp_متن);
	    prop.u.tp_متن_انحراف = (colnr_T)(vtext_dest - گنتی_dest);
	    vtext_dest += copysize;
	}
	ورنہ
	    prop.u.tp_متن_انحراف = 0;
	mch_memmove(prop_dest + i * حجم(textprop_T), &prop,
							حجم(textprop_T));
    }
    واپس newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    خالی
جوڑو_متن_props(linenr_T lnum, textprop_T *متن_props, صحیح متن_prop_گنتی)
{
    unpacked_memline_T	um;
    صحیح			i;

    um = um_open_at(curbuf, lnum, متن_prop_گنتی);
    اگر (um.buf == تہی)
	واپس;
    اگر (!um_detach(&um))
    {
	um_abort(&um);
	واپس;
    }

    // Grow props array if needed.
    اگر (um.prop_گنتی + متن_prop_گنتی > um.prop_سائز)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_گنتی + متن_prop_گنتی) * حجم(textprop_T));
	اگر (newprops == تہی)
	{
	    um_abort(&um);
	    واپس;
	}
	um.props = newprops;
	um.prop_سائز = um.prop_گنتی + متن_prop_گنتی;
    }

    برائے (i = 0; i < متن_prop_گنتی; ++i)
	um.props[um.prop_گنتی++] = متن_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    ساکن صحیح
compare_pt(ثابت خالی *s1, ثابت خالی *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    واپس tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    ساکن proptype_T *
ڈھونڈو_type_by_id(hashtab_T *ht, proptype_T ***array, صحیح id)
{
    صحیح کم = 0;
    صحیح بلند;

    اگر (ht == تہی || ht->ht_مستعمل == 0)
	واپس تہی;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    اگر (*array == تہی)
    {
	طویل	    todo;
	hashitem_T  *hi;
	صحیح	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_مستعمل);
	اگر (*array == تہی)
	    واپس تہی;
	todo = (طویل)ht->ht_مستعمل;
	FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
	{
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((خالی *)*array, ht->ht_مستعمل, حجم(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    بلند = ht->ht_مستعمل;
    جب (بلند > کم)
    {
	صحیح m = (بلند + کم) / 2;

	اگر ((*array)[m]->pt_id == id)
	    واپس (*array)[m];
	اگر ((*array)[m]->pt_id > id)
	    بلند = m;
	ورنہ
	    کم = m + 1;
    }
    واپس تہی;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    ساکن خالی
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    صحیح buflocal = TRUE;
    صحیح virtualtext_prop = prop->tp_id < 0;

    dict_جوڑو_نمبر(dict, "col", (prop->tp_ستون == MAXCOL) ? 0 : prop->tp_ستون);
    اگر (!virtualtext_prop)
    {
	dict_جوڑو_نمبر(dict, "length", prop->tp_len);
	dict_جوڑو_نمبر(dict, "id", prop->tp_id);
    }
    dict_جوڑو_نمبر(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_پچھلا));
    dict_جوڑو_نمبر(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_اگلا));

    pt = ڈھونڈو_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    اگر (pt == تہی)
    {
	pt = ڈھونڈو_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    اگر (pt != تہی)
	dict_جوڑو_string(dict, "type", pt->pt_name);

    اگر (buflocal)
	dict_جوڑو_نمبر(dict, "type_bufnr", buf->b_fnum);
    ورنہ
	dict_جوڑو_نمبر(dict, "type_bufnr", 0);
    اگر (virtualtext_prop)
    {
	string_T    متن_align = {تہی, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_جوڑو_string(dict, "text", prop->u.tp_متن);

	// text_align
	اگر (prop->tp_flags & TP_FLAG_ALIGN_دایاں)
	    STR_LITERAL_سیٹ(متن_align, "right");
	ورنہ اگر (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_سیٹ(متن_align, "above");
	ورنہ اگر (prop->tp_flags & TP_FLAG_ALIGN_نیچے)
	    STR_LITERAL_سیٹ(متن_align, "below");
	اگر (متن_align.string != تہی)
	    dict_جوڑو_string_len(dict, "text_align",
		متن_align.string, (صحیح)متن_align.لمبائی);

	// text_wrap
	اگر (prop->tp_flags & TP_FLAG_WRAP)
	    dict_جوڑو_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	اگر (prop->tp_padleft != 0)
	    dict_جوڑو_نمبر(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
متن_prop_type_by_id(buf_T *buf, صحیح id)
{
    proptype_T *type;

    type = ڈھونڈو_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    اگر (type == تہی)
	type = ڈھونڈو_type_by_id(global_proptypes, &global_proparray, id);
    واپس type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
متن_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    واپس متن_prop_type_by_id(buf, prop->tp_type) != تہی;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    خالی
f_prop_clear(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    linenr_T آغاز;
    linenr_T انتہا;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    صحیح	    did_clear = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	واپس;

    آغاز = tv_get_نمبر(&argvars[0]);
    انتہا = آغاز;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	انتہا = tv_get_نمبر(&argvars[1]);
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    اگر (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		واپس;
	}
    }
    اگر (آغاز < 1 || انتہا < 1)
    {
	emsg(_(e_invalid_range));
	واپس;
    }

    {
	unpacked_memline_T um = um_open(buf);

	برائے (lnum = آغاز; lnum <= انتہا; ++lnum)
	{
	    صحیح idx;

	    اگر (lnum > buf->b_ml.ml_سطر_گنتی)
		توڑو;
	    اگر (!um_goto_سطر(&um, lnum, 0))
		توڑو;
	    اگر (um.prop_گنتی > 0)
	    {
		did_clear = TRUE;
		برائے (idx = um.prop_گنتی - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    اگر (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    خالی
f_prop_ڈھونڈو(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    صحیح		lnum_آغاز;
    صحیح		آغاز_pos_has_prop = 0;
    صحیح		نظر_آیا_انتہا = FALSE;
    صحیح		id = 0;
    صحیح		id_ملا = FALSE;
    صحیح		type_id = -1;
    صحیح		skipstart = FALSE;
    صحیح		lnum = -1;
    صحیح		ستون = -1;
    صحیح		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    صحیح		both;

    اگر (اندر_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    اگر (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	واپس;
    dict = argvars[0].vval.v_dict;

    اگر (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	واپس;
    اگر (buf->b_ml.ml_mfp == تہی)
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	اگر (*dir_s == 'b')
	    dir = BACKWARD;
	ورنہ اگر (*dir_s != 'f')
	{
	    emsg(_(e_invalid_دلیل));
	    واپس;
	}
    }

    di = dict_ڈھونڈو(dict, (char_u *)"lnum", -1);
    اگر (di != تہی)
	lnum = tv_get_نمبر(&di->di_tv);

    di = dict_ڈھونڈو(dict, (char_u *)"col", -1);
    اگر (di != تہی)
	ستون = tv_get_نمبر(&di->di_tv);

    اگر (lnum == -1)
    {
	lnum = cursor->lnum;
	ستون = cursor->ستون + 1;
    }
    ورنہ اگر (ستون == -1)
	ستون = 1;

    اگر (lnum < 1 || lnum > buf->b_ml.ml_سطر_گنتی)
    {
	emsg(_(e_invalid_range));
	واپس;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    اگر (dict_has_کلید(dict, "id"))
    {
	id = dict_get_نمبر(dict, "id");
	id_ملا = TRUE;
    }
    اگر (dict_has_کلید(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	اگر (type == تہی)
	    واپس;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    اگر (!id_ملا && type_id == -1)
    {
	emsg(_(e_need_at_کم_سے_کم_one_کا_id_or_type));
	واپس;
    }
    اگر (both && (!id_ملا || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	واپس;
    }

    lnum_آغاز = lnum;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;

    جب (1)
    {
	char_u	*prop_آغاز_ptr;
	صحیح	گنتی = get_متن_props(buf, lnum, &prop_آغاز_ptr, FALSE);
	صحیح	    i;
	textprop_T  prop;
	صحیح	    prop_آغاز;
	صحیح	    prop_انتہا;

	برائے (i = dir == BACKWARD ? گنتی - 1 : 0; i >= 0 && i < گنتی; i += dir)
	{
	    mch_memmove(&prop, prop_آغاز_ptr + i * حجم(textprop_T),
							   حجم(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    اگر (prop.tp_id < 0 && prop.u.tp_متن_انحراف > 0)
	    {
		prop.u.tp_متن = (prop_آغاز_ptr - PROP_گنتی_سائز)
						    + prop.u.tp_متن_انحراف;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    ورنہ
		prop.u.tp_متن = تہی;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    اگر (lnum == lnum_آغاز)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_ستون == MAXCOL;
		اگر (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    اگر (!is_floating_vtext && prop.tp_ستون > ستون)
			جاری;
		}
		ورنہ اگر (!is_floating_vtext
		    && prop.tp_ستون + prop.tp_len - (prop.tp_len != 0) < ستون)
		    جاری;
	    }
	    اگر (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_ملا && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		اگر (lnum_آغاز == lnum
			&& ستون >= prop.tp_ستون
			&& (ستون <= prop.tp_ستون + prop.tp_len
							 - (prop.tp_len != 0)))
		    آغاز_pos_has_prop = 1;

		// The property was not continued from last line, it starts on
		// this line.
		prop_آغاز = !(prop.tp_flags & TP_FLAG_CONT_پچھلا);
		// The property does not continue on the next line, it ends on
		// this line.
		prop_انتہا = !(prop.tp_flags & TP_FLAG_CONT_اگلا);
		اگر (!prop_آغاز && prop_انتہا && dir == FORWARD)
		    نظر_آیا_انتہا = 1;

		// Skip lines without the start flag.
		اگر (!prop_آغاز)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    اگر (آغاز_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    جاری;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		اگر (آغاز_pos_has_prop && skipstart && !نظر_آیا_انتہا)
		{
		    آغاز_pos_has_prop = 0;
		    جاری;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_جوڑو_نمبر(rettv->vval.v_dict, "lnum", lnum);

		واپس;
	    }
	}

	اگر (dir > 0)
	{
	    اگر (lnum >= buf->b_ml.ml_سطر_گنتی)
		توڑو;
	    lnum++;
	}
	ورنہ
	{
	    اگر (lnum <= 1)
		توڑو;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    ساکن صحیح
prop_type_or_id_اندر_list(صحیح *types_or_ids, صحیح len, صحیح type_or_id)
{
    صحیح i;

    برائے (i = 0; i < len; i++)
	اگر (types_or_ids[i] == type_or_id)
	    واپس TRUE;

    واپس FALSE;
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
    ساکن خالی
get_props_اندر_سطر(
	buf_T		*buf,
	linenr_T	lnum,
	صحیح		*prop_types,
	صحیح		prop_types_len,
	صحیح		*prop_ids,
	صحیح		prop_ids_len,
	list_T		*retlist,
	صحیح		جوڑو_lnum)
{
    char_u	*props;
    صحیح		گنتی;
    صحیح		i;
    textprop_T	prop;

    گنتی = get_متن_props(buf, lnum, &props, FALSE);
    برائے (i = 0; i < گنتی; ++i)
    {
	mch_memmove(&prop, props + i * حجم(textprop_T),
		حجم(textprop_T));
	// Convert offset to pointer for virtual text props.
	اگر (prop.tp_id < 0 && prop.u.tp_متن_انحراف > 0)
	{
	    prop.u.tp_متن = (props - PROP_گنتی_سائز)
						+ prop.u.tp_متن_انحراف;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ورنہ
	    prop.u.tp_متن = تہی;
	اگر ((prop_types == تہی
		    || prop_type_or_id_اندر_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == تہی
		    || prop_type_or_id_اندر_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    اگر (d == تہی)
		توڑو;
	    prop_fill_dict(d, &prop, buf);
	    اگر (جوڑو_lnum)
		dict_جوڑو_نمبر(d, "lnum", lnum);
	    اگر (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    ساکن صحیح *
get_prop_types_from_نام(list_T *l, buf_T *buf, صحیح *عدد_types)
{
    صحیح		*prop_types;
    listitem_T	*li;
    صحیح		i;
    char_u	*name;
    proptype_T	*type;

    *عدد_types = 0;

    prop_types = ALLOC_MULT(صحیح, list_len(l));
    اگر (prop_types == تہی)
	واپس تہی;

    i = 0;
    FOR_ALL_LIST_اشیاء(l, li)
    {
	اگر (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    جاؤ errret;
	}
	name = li->li_tv.vval.v_string;
	اگر (name == تہی)
	    جاؤ errret;

	type = lookup_prop_type(name, buf);
	اگر (type == تہی)
	    جاؤ errret;
	prop_types[i++] = type->pt_id;
    }

    *عدد_types = i;
    واپس prop_types;

errret:
    VIM_CLEAR(prop_types);
    واپس تہی;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    ساکن صحیح *
get_prop_ids_from_list(list_T *l, صحیح *عدد_ids)
{
    صحیح		*prop_ids;
    listitem_T	*li;
    صحیح		i = 0;
    صحیح		id;
    صحیح		error;

    *عدد_ids = 0;

    prop_ids = ALLOC_MULT(صحیح, list_len(l));
    اگر (prop_ids == تہی)
	واپس تہی;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اشیاء(l, li)
    {
	error = FALSE;
	id = tv_get_نمبر_chk(&li->li_tv, &error);
	اگر (error)
	    جاؤ errret;

	prop_ids[i++] = id;
    }

    *عدد_ids = i;
    واپس prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    واپس تہی;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    خالی
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	آغاز_lnum;
    linenr_T	انتہا_lnum;
    buf_T	*buf = curbuf;
    صحیح		جوڑو_lnum = FALSE;
    صحیح		*prop_types = تہی;
    صحیح		prop_types_len = 0;
    صحیح		*prop_ids = تہی;
    صحیح		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    // default: get text properties on current line
    آغاز_lnum = tv_get_نمبر(&argvars[0]);
    انتہا_lnum = آغاز_lnum;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	اگر (check_for_dict_arg(argvars, 1) == FAIL)
	    واپس;
	d = argvars[1].vval.v_dict;

	اگر (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    واپس;

	اگر (d != تہی && (di = dict_ڈھونڈو(d, (char_u *)"end_lnum", -1)) != تہی)
	{
	    اگر (di->di_tv.v_type != VAR_نمبر)
	    {
		emsg(_(e_نمبر_required));
		واپس;
	    }
	    انتہا_lnum = tv_get_نمبر(&di->di_tv);
	    اگر (انتہا_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//سطر
		انتہا_lnum = buf->b_ml.ml_سطر_گنتی + انتہا_lnum + 1;
	    ورنہ اگر (انتہا_lnum > buf->b_ml.ml_سطر_گنتی)
		انتہا_lnum = buf->b_ml.ml_سطر_گنتی;
	    جوڑو_lnum = TRUE;
	}
	اگر (d != تہی && (di = dict_ڈھونڈو(d, (char_u *)"types", -1)) != تہی)
	{
	    اگر (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		واپس;
	    }

	    l = di->di_tv.vval.v_list;
	    اگر (l != تہی && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_نام(l, buf, &prop_types_len);
		اگر (prop_types == تہی)
		    واپس;
	    }
	}
	اگر (d != تہی && (di = dict_ڈھونڈو(d, (char_u *)"ids", -1)) != تہی)
	{
	    اگر (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		جاؤ errret;
	    }

	    l = di->di_tv.vval.v_list;
	    اگر (l != تہی && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		اگر (prop_ids == تہی)
		    جاؤ errret;
	    }
	}
    }
    اگر (آغاز_lnum < 1 || آغاز_lnum > buf->b_ml.ml_سطر_گنتی
		|| انتہا_lnum < 1 || انتہا_lnum < آغاز_lnum)
	emsg(_(e_invalid_range));
    ورنہ
	برائے (lnum = آغاز_lnum; lnum <= انتہا_lnum; lnum++)
	    get_props_اندر_سطر(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, جوڑو_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    خالی
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	آغاز = 1;
    linenr_T	انتہا = 0;
    linenr_T	lnum;
    linenr_T	پہلا_بدلا = 0;
    linenr_T	آخری_بدلا = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    صحیح		do_all;
    صحیح		id = -MAXCOL;
    صحیح		type_id = -1;	    // for a single "type"
    صحیح		*type_ids = تہی;   // array, for a list of "types", allocated
    صحیح		عدد_type_ids = 0;   // number of elements in "type_ids"
    صحیح		both;

    rettv->vval.v_نمبر = 0;

    اگر (اندر_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 2) == FAIL)))
	واپس;

    اگر (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	آغاز = tv_get_نمبر(&argvars[1]);
	انتہا = آغاز;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    انتہا = tv_get_نمبر(&argvars[2]);
	اگر (آغاز < 1 || انتہا < 1)
	{
	    emsg(_(e_invalid_range));
	    واپس;
	}
    }

    dict = argvars[0].vval.v_dict;
    اگر (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	واپس;
    اگر (buf->b_ml.ml_mfp == تہی)
	واپس;

    do_all = dict_get_bool(dict, "all", FALSE);

    اگر (dict_has_کلید(dict, "id"))
	id = dict_get_نمبر(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    اگر (dict_has_کلید(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	اگر (type == تہی)
	    واپس;
	type_id = type->pt_id;
    }
    اگر (dict_has_کلید(dict, "types"))
    {
	typval_T types;
	listitem_T *li = تہی;

	dict_get_tv(dict, "types", &types);
	اگر (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( حجم(صحیح) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_اشیاء(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		اگر (li->li_tv.v_type != VAR_STRING)
		    جاری;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		اگر (!prop_type)
		    جاؤ cleanup_prop_remove;

		type_ids[عدد_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    اگر (id == -MAXCOL && (type_id == -1 && عدد_type_ids == 0))
    {
	emsg(_(e_need_at_کم_سے_کم_one_کا_id_or_type));
	جاؤ cleanup_prop_remove;
    }
    اگر (both && (id == -MAXCOL || (type_id == -1 && عدد_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	جاؤ cleanup_prop_remove;
    }
    اگر (type_id != -1 && عدد_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	جاؤ cleanup_prop_remove;
    }

    اگر (انتہا == 0)
	انتہا = buf->b_ml.ml_سطر_گنتی;

    {
	unpacked_memline_T um = um_open(buf);

	برائے (lnum = آغاز; lnum <= انتہا; ++lnum)
	{
	    صحیح idx;

	    اگر (lnum > buf->b_ml.ml_سطر_گنتی)
		توڑو;
	    اگر (!um_goto_سطر(&um, lnum, 0))
		توڑو;

	    برائے (idx = 0; idx < um.prop_گنتی; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		صحیح	    matches_id = 0;
		صحیح	    matches_type = 0;

		اگر (prop->tp_flags & TP_FLAG_DELETED)
		    جاری;

		matches_id = prop->tp_id == id;
		اگر (عدد_type_ids > 0)
		{
		    صحیح idx2;

		    برائے (idx2 = 0; !matches_type && idx2 < عدد_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		ورنہ
		    matches_type = prop->tp_type == type_id;

		اگر (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    اگر (پہلا_بدلا == 0)
			پہلا_بدلا = lnum;
		    آخری_بدلا = lnum;
		    ++rettv->vval.v_نمبر;
		    اگر (!do_all)
			توڑو;
		}
	    }
	}
	um_close(&um);
    }

    اگر (پہلا_بدلا > 0)
    {
	بدلا_سطر_display_buf(buf);
	بدلا_سطریں_buf(buf, پہلا_بدلا, آخری_بدلا + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    ساکن خالی
prop_type_سیٹ(typval_T *argvars, صحیح جوڑو)
{
    char_u	*name;
    buf_T	*buf = تہی;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	واپس;

    name = tv_get_string(&argvars[0]);
    اگر (*name == NUL)
    {
	semsg(_(e_invalid_دلیل_str), "\"\"");
	واپس;
    }

    اگر (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	واپس;
    dict = argvars[1].vval.v_dict;

    prop = ڈھونڈو_prop_type(name, buf);
    اگر (جوڑو)
    {
	hashtab_T **htp;

	اگر (prop != تہی)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    واپس;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	اگر (prop == تہی)
	    واپس;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	اگر (buf == تہی)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	ورنہ
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	اگر (*htp == تہی)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    اگر (*htp == تہی)
	    {
		vim_free(prop);
		واپس;
	    }
	    hash_init(*htp);
	}
	hash_جوڑو(*htp, PT2HIKEY(prop), "prop type");
    }
    ورنہ
    {
	اگر (prop == تہی)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    واپس;
	}
    }

    اگر (dict != تہی)
    {
	di = dict_ڈھونڈو(dict, (char_u *)"highlight", -1);
	اگر (di != تہی)
	{
	    char_u	*highlight;
	    صحیح		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    اگر (highlight != تہی && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    اگر (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_گروہ_name_str),
			highlight == تہی ? (char_u *)"" : highlight);
		واپس;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_ڈھونڈو(dict, (char_u *)"combine", -1);
	اگر (di != تہی)
	{
	    اگر (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    ورنہ
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_ڈھونڈو(dict, (char_u *)"override", -1);
	اگر (di != تہی)
	{
	    اگر (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    ورنہ
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_ڈھونڈو(dict, (char_u *)"priority", -1);
	اگر (di != تہی)
	    prop->pt_ترجیح = tv_get_نمبر(&di->di_tv);

	di = dict_ڈھونڈو(dict, (char_u *)"start_incl", -1);
	اگر (di != تہی)
	{
	    اگر (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_آغاز_INCL;
	    ورنہ
		prop->pt_flags &= ~PT_FLAG_INS_آغاز_INCL;
	}

	di = dict_ڈھونڈو(dict, (char_u *)"end_incl", -1);
	اگر (di != تہی)
	{
	    اگر (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_انتہا_INCL;
	    ورنہ
		prop->pt_flags &= ~PT_FLAG_INS_انتہا_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    خالی
f_prop_type_جوڑو(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    prop_type_سیٹ(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    خالی
f_prop_type_change(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    prop_type_سیٹ(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    خالی
f_prop_type_delete(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    char_u	*name;
    buf_T	*buf = تہی;
    hashitem_T	*hi;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    name = tv_get_string(&argvars[0]);
    اگر (*name == NUL)
    {
	semsg(_(e_invalid_دلیل_str), "\"\"");
	واپس;
    }

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	اگر (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    واپس;
    }

    hi = ڈھونڈو_prop_type_hi(name, buf);
    اگر (hi == تہی)
	واپس;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    اگر (buf == تہی)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    ورنہ
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    بدلا_کھڑکی_setting_buf(buf == تہی ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    خالی
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    name = tv_get_string(&argvars[0]);
    اگر (*name == NUL)
    {
	semsg(_(e_invalid_دلیل_str), "\"\"");
	واپس;
    }

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;

    proptype_T  *prop = تہی;
    buf_T	    *buf = تہی;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	اگر (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    واپس;
    }

    prop = ڈھونڈو_prop_type(name, buf);
    اگر (prop == تہی)
	واپس;

    dict_T *d = rettv->vval.v_dict;

    اگر (prop->pt_hl_id > 0)
	dict_جوڑو_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_جوڑو_نمبر(d, "priority", prop->pt_ترجیح);
    dict_جوڑو_نمبر(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_جوڑو_نمبر(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_جوڑو_نمبر(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_آغاز_INCL) ? 1 : 0);
    dict_جوڑو_نمبر(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_انتہا_INCL) ? 1 : 0);
    اگر (buf != تہی)
	dict_جوڑو_نمبر(d, "bufnr", buf->b_fnum);
}

    ساکن خالی
list_types(hashtab_T *ht, list_T *l)
{
    طویل	todo;
    hashitem_T	*hi;

    todo = (طویل)ht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
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
    خالی
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = تہی;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	اگر (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    واپس;
    }
    اگر (buf == تہی)
    {
	اگر (global_proptypes != تہی)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    ورنہ اگر (buf->b_proptypes != تہی)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    ساکن خالی
clear_ht_prop_types(hashtab_T *ht)
{
    طویل	todo;
    hashitem_T	*hi;

    اگر (ht == تہی)
	واپس;

    todo = (طویل)ht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#اگر defined(EXITFREE)
/*
 * Free all global property types.
 */
    خالی
clear_global_prop_types(خالی)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = تہی;
    VIM_CLEAR(global_proparray);
}
#اگر_ختم

/*
 * Free all property types for "buf".
 */
    خالی
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = تہی;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
قسمتعریف ساخت
{
    صحیح dirty;	    // if the property was changed
    صحیح can_drop;   // whether after this change, the prop may be removed
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
    ساکن adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    ستون,
	صحیح	    added,
	صحیح	    flags)
{
    proptype_T	*pt;
    صحیح		آغاز_incl;
    صحیح		انتہا_incl;
    صحیح		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    اگر (prop->tp_ستون == MAXCOL)
    {
	res.dirty = FALSE;
	واپس res;
    }

    pt = متن_prop_type_by_id(curbuf, prop->tp_type);
    آغاز_incl = (pt != تہی && (pt->pt_flags & PT_FLAG_INS_آغاز_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_پچھلا);
    اگر (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	آغاز_incl = FALSE;
    انتہا_incl = (pt != تہی && (pt->pt_flags & PT_FLAG_INS_انتہا_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_اگلا);
    // do not drop zero-width props if they later can increase in size
    droppable = !(آغاز_incl || انتہا_incl);

    اگر (added > 0)
    {
	اگر (ستون + 1 <= prop->tp_ستون
		- (آغاز_incl || (prop->tp_len == 0 && انتہا_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_ستون += added;
	ورنہ اگر (ستون + 1 < prop->tp_ستون + prop->tp_len + انتہا_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    ورنہ اگر (prop->tp_ستون > ستون + 1)
    {
	اگر (prop->tp_ستون + added < ستون + 1)
	{
	    اگر (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    ورنہ
	    {
		prop->tp_len += (prop->tp_ستون - 1 - ستون) + added;
		prop->tp_ستون = ستون + 1;
		اگر (prop->tp_len <= 0)
		{
		    prop->tp_len = 0;
		    // Multiline properties with no text left should
		    // also be dropped.
		    res.can_drop = droppable
			|| (prop->tp_flags
				& (TP_FLAG_CONT_پچھلا | TP_FLAG_CONT_اگلا));
		}
	    }
	}
	ورنہ
	    prop->tp_ستون += added;
    }
    ورنہ اگر (prop->tp_len > 0 && prop->tp_ستون + prop->tp_len > ستون
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	اگر (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = ستون - (prop->tp_ستون - 1);
	ورنہ
	{
	    صحیح after = ستون - added - (prop->tp_ستون - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	اگر (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_اگلا)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_پچھلا | TP_FLAG_CONT_اگلا));
    }
    ورنہ
	res.dirty = FALSE;

    واپس res;
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
    صحیح
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    ستون,
	صحیح	    بائٹیں_added,
	صحیح	    flags)
{
    unpacked_memline_T um;
    صحیح		dirty = FALSE;
    صحیح		ri;

    اگر (متن_prop_frozen > 0)
	واپس FALSE;

    um = um_open_at(curbuf, lnum, 0);
    اگر (um.buf == تہی || um.prop_گنتی == 0)
    {
	um_abort(&um);
	واپس FALSE;
    }

    برائے (ri = 0; ri < um.prop_گنتی; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, ستون, بائٹیں_added, flags);
	اگر (res.dirty)
	{
	    اگر (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		اگر (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    واپس FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		اگر ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    واپس FALSE;
		}
	    }
	    dirty = TRUE;
	}
	اگر (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    جاری;
	}
    }
    اگر (dirty)
	um_close(&um);
    ورنہ
	um_abort(&um);
    واپس dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    خالی
adjust_props_for_بانٹو(
	linenr_T    lnum_props,
	linenr_T    lnum_چوٹی,
	صحیح	    kept,
	صحیح	    deleted,
	صحیح	    at_eol)
{
    char_u	*props;
    صحیح		گنتی;
    garray_T    prevprop;
    garray_T    nextprop;
    صحیح		skipped = kept + deleted;

    اگر (!curbuf->b_has_textprop)
	واپس;

    // Get the text properties from "lnum_props".
    گنتی = get_متن_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, حجم(textprop_T), 10);
    ga_init2(&nextprop, حجم(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *گنتی_ptr = props - PROP_گنتی_سائز;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    برائے (صحیح i = 0; i < گنتی; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	صحیح	    آغاز_incl, انتہا_incl;
	صحیح	    cont_پچھلا, cont_اگلا;
	صحیح	    prop_ستون;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * حجم(textprop_T), حجم(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	اگر (prop.tp_id < 0 && prop.u.tp_متن_انحراف > 0)
	{
	    prop.u.tp_متن = vim_strsave(گنتی_ptr + prop.u.tp_متن_انحراف);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ورنہ
	    prop.u.tp_متن = تہی;

	pt = متن_prop_type_by_id(curbuf, prop.tp_type);
	آغاز_incl = (pt != تہی && (pt->pt_flags & PT_FLAG_INS_آغاز_INCL));
	انتہا_incl = (pt != تہی && (pt->pt_flags & PT_FLAG_INS_انتہا_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_ستون = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_ستون;

	اگر (prop_ستون == MAXCOL)
	{
	    cont_پچھلا = at_eol;
	    cont_اگلا = !at_eol;
	}
	ورنہ
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    صحیح is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_ستون == MAXCOL);

	    cont_پچھلا = prop_ستون + !آغاز_incl <= kept;
	    اگر (is_floating_vtext)
		cont_اگلا = FALSE;
	    ورنہ
		cont_اگلا = skipped <= prop_ستون + prop.tp_len - !انتہا_incl;
	}
	// when a prop has text it is never copied
	اگر (prop.tp_id < 0 && cont_اگلا)
	    cont_پچھلا = FALSE;

	اگر (cont_پچھلا && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_ڈیٹا) + prevprop.ga_len;
	    صحیح is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    اگر (!is_vtext && p->tp_ستون + p->tp_len >= kept)
		p->tp_len = kept - p->tp_ستون;
	    اگر (cont_اگلا)
		p->tp_flags |= TP_FLAG_CONT_اگلا;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	اگر (cont_اگلا && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_ڈیٹا) + nextprop.ga_len;
	    صحیح is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    اگر (p->tp_ستون != MAXCOL)
	    {
		اگر (p->tp_ستون > skipped)
		    p->tp_ستون -= skipped - 1;
		ورنہ
		{
		    اگر (!is_vtext)
			p->tp_len -= skipped - p->tp_ستون;
		    p->tp_ستون = 1;
		}
	    }
	    اگر (cont_پچھلا)
		p->tp_flags |= TP_FLAG_CONT_پچھلا;
	}
    }

    سیٹ_متن_props(lnum_چوٹی, (textprop_T *)prevprop.ga_ڈیٹا,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    سیٹ_متن_props(lnum_چوٹی + 1, (textprop_T *)nextprop.ga_ڈیٹا,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    خالی
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	صحیح		    آخری_سطر,
	طویل		    ستون,
	صحیح		    removed)
{
    unpacked_memline_T	r_um;

    اگر (um->buf == تہی)
	واپس;

    r_um = um_open_at(um->buf, lnum, 0);
    اگر (r_um.buf == تہی)
	واپس;
    اگر (!um_detach(&r_um))
    {
	um_abort(&r_um);
	واپس;
    }

    برائے (صحیح i = r_um.prop_گنتی - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	صحیح	    انتہا;

	اگر (prop->tp_ستون == MAXCOL && !آخری_سطر)
	    جاری;  // drop floating text for non-last lines
	انتہا = !(prop->tp_flags & TP_FLAG_CONT_اگلا);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, ستون, 0);

	اگر (آخری_سطر || انتہا)
	{
	    um_جوڑو_prop(um, prop);
	    prop->u.tp_متن = تہی;  // ownership transferred
	}
	ورنہ
	{
	    // Search for continuing prop in um.
	    bool    ملا = false;

	    برائے (صحیح j = 0; j < um->prop_گنتی; ++j)
	    {
		textprop_T *op = &um->props[j];

		اگر ((op->tp_flags & TP_FLAG_CONT_پچھلا)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    ملا = true;
		    op->tp_len += op->tp_ستون - prop->tp_ستون;
		    op->tp_ستون = prop->tp_ستون;
		    op->tp_flags = prop->tp_flags;
		    توڑو;
		}
	    }
	    اگر (!ملا)
		اندرونی_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    ساکن خالی
remember_cleared_prop(garray_T *gap, صحیح which, textprop_T *prop, صحیح flag)
{
    undoprop_T	*اوپر;

    اگر (which == UNDOPROP_NONE)
	واپس;
    اگر (gap->ga_itemsize == 0)
	ga_init2(gap, حجم(undoprop_T), 4);
    اگر (ga_grow(gap, 1) == FAIL)
	واپس;
    اوپر = (undoprop_T *)gap->ga_ڈیٹا + gap->ga_len++;
    اوپر->اوپر_which = which;
    اوپر->اوپر_type = prop->tp_type;
    اوپر->اوپر_id = prop->tp_id;
    اوپر->اوپر_ستون = prop->tp_ستون;
    اوپر->اوپر_flag = flag;
}

/*
 * Clear the continuation flags between line "lnum" and the line below it that
 * have no partner in the other line.  A property is split there then.  What
 * is cleared on the line above is remembered in "gap" as "above_which", on
 * the line below as "below_which".
 */
    خالی
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	صحیح	    above_which,
	صحیح	    نیچے_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	نیچے;
    bool		*paired = تہی;
    bool		above_بدلا = false;
    bool		نیچے_بدلا = false;
    bool		cont_اگلا = false;

    اگر (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_سطر_گنتی)
	واپس;

    above = um_open_at(buf, lnum, 0);
    اگر (above.buf == تہی)
	واپس;
    برائے (صحیح i = 0; i < above.prop_گنتی; ++i)
	اگر (above.props[i].tp_flags & TP_FLAG_CONT_اگلا)
	    cont_اگلا = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    اگر (cont_اگلا && !um_detach(&above))
    {
	um_abort(&above);
	واپس;
    }

    نیچے = um_open_at(buf, lnum + 1, 0);
    اگر (نیچے.buf == تہی)
    {
	um_abort(&above);
	واپس;
    }
    اگر (نیچے.prop_گنتی > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, نیچے.prop_گنتی);
	اگر (paired == تہی)
	    جاؤ theend;
    }

    برائے (صحیح i = 0; i < above.prop_گنتی; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    ملا = false;

	اگر (!(prop->tp_flags & TP_FLAG_CONT_اگلا))
	    جاری;
	برائے (صحیح j = 0; j < نیچے.prop_گنتی; ++j)
	{
	    textprop_T *اگلا = &نیچے.props[j];

	    اگر (!paired[j] && (اگلا->tp_flags & TP_FLAG_CONT_پچھلا)
		    && اگلا->tp_id == prop->tp_id
		    && اگلا->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		ملا = true;
		توڑو;
	    }
	}
	اگر (!ملا)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_اگلا;
	    above_بدلا = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_اگلا);
	}
    }

    برائے (صحیح j = 0; j < نیچے.prop_گنتی; ++j)
    {
	اگر (!(نیچے.props[j].tp_flags & TP_FLAG_CONT_پچھلا) || paired[j])
	    جاری;
	اگر (!نیچے.detached && !um_detach(&نیچے))
	    توڑو;
	نیچے.props[j].tp_flags &= ~TP_FLAG_CONT_پچھلا;
	نیچے_بدلا = true;
	remember_cleared_prop(gap, نیچے_which, &نیچے.props[j],
							   TP_FLAG_CONT_پچھلا);
    }

theend:
    اگر (نیچے_بدلا)
	um_close(&نیچے);
    ورنہ
	um_abort(&نیچے);
    اگر (above_بدلا)
	um_close(&above);
    ورنہ
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    خالی
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	صحیح	    which)
{
    unpacked_memline_T	um;
    bool		بدلا = false;

    اگر (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_سطر_گنتی)
	واپس;
    um = um_open_at(buf, lnum, 0);
    اگر (um.buf == تہی)
	واپس;
    برائے (صحیح r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *اوپر = (undoprop_T *)gap->ga_ڈیٹا + r;

	اگر (اوپر->اوپر_which != which)
	    جاری;
	برائے (صحیح i = 0; i < um.prop_گنتی; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    اگر (prop->tp_type != اوپر->اوپر_type || prop->tp_id != اوپر->اوپر_id
		    || prop->tp_ستون != اوپر->اوپر_ستون
		    || (prop->tp_flags & اوپر->اوپر_flag))
		جاری;
	    اگر (!um.detached && !um_detach(&um))
		جاؤ theend;
	    um.props[i].tp_flags |= اوپر->اوپر_flag;
	    بدلا = true;
	    توڑو;
	}
    }

theend:
    اگر (بدلا)
	um_close(&um);
    ورنہ
	um_abort(&um);
}

    bool
متن_prop_گنتی_valid(صحیح prop_گنتی, size_t propdata_len)
{
    اگر (propdata_len < PROP_گنتی_سائز)
	واپس false;
    واپس (size_t)prop_گنتی * حجم(textprop_T)
		    <= propdata_len - PROP_گنتی_سائز;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
متن_prop_vtext_valid(char_u *props, صحیح prop_گنتی, size_t propdata_len)
{
    برائے (صحیح i = 0; i < prop_گنتی; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * حجم(textprop_T),
							  حجم(textprop_T));
	اگر (prop.tp_id >= 0 || prop.u.tp_متن_انحراف <= 0)
	    جاری;
	اگر (prop.tp_len < 0 || (size_t)prop.u.tp_متن_انحراف
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    واپس false;
    }
    واپس true;
}

#اگر_ختم // FEAT_PROP_POPUP
