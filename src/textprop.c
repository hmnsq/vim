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

ایستا تهی um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    ایستا تهی
um_free_vtext(textprop_T *props, صحیح شمار)
{
    برای (صحیح i = 0; i < شمار; ++i)
	اگر (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    ایستا تهی
um_free(unpacked_memline_T *um)
{
    اگر (um->detached)
    {
	VIM_CLEAR(um->text);
	um_free_vtext(um->props, um->prop_شمار);
    }
    VIM_CLEAR(um->props);
    um->prop_size = 0;
    um->prop_شمار = 0;
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

    CLEAR_FIELD(um);
    um.buf = buf;
    بازگشت um;
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

    اگر (um->buf == پوچ)
	بازگشت false;

    // Store changes to the current line if any.
    اگر (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    اگر (lnum == 0)
	بازگشت true;	// just unload

    سطر = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_سطر_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = سطر;

    اگر (propdata_len == 0)
	بازگشت true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    اگر (propdata_len < PROP_شمار_SIZE + اندازه(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = پوچ;
	بازگشت false;
    }

    uint16_t    prop_شمار;
    char_u	    *شمار_ptr = سطر + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_شمار, شمار_ptr, PROP_شمار_SIZE);
    اگر (!text_prop_شمار_valid(prop_شمار, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = پوچ;
	بازگشت false;
    }
    proplen = (صحیح)prop_شمار;
    props_start = شمار_ptr + PROP_شمار_SIZE;

    اگر (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = پوچ;
	بازگشت false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    اگر (um->props == پوچ)
    {
	um->buf = پوچ;
	بازگشت false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_شمار = proplen;
    mch_memmove(um->props, props_start, proplen * اندازه(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    برای (صحیح i = 0; i < proplen; ++i)
    {
	اگر (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = شمار_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	وگرنه
	    um->props[i].u.tp_text = پوچ;
    }

    بازگشت true;
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
	um.buf = پوچ;
    بازگشت um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    ایستا bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    اگر (um->detached || um->buf == پوچ)
	بازگشت um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    اگر (newtext == پوچ)
    {
	um->buf = پوچ;
	بازگشت false;
    }
    um->text = newtext;

    برای (صحیح i = 0; i < um->prop_شمار; ++i)
    {
	اگر (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    اگر (copy == پوچ)
	    {
		برای (صحیح j = 0; j < i; ++j)
		    اگر (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = پوچ;
		بازگشت false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    بازگشت true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    اگر (um->buf == پوچ)
	بازگشت false;
    اگر (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    بازگشت true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    ایستا bool
um_grow_props(unpacked_memline_T *um, صحیح needed)
{
    اگر (um->prop_شمار + needed <= um->prop_size)
	بازگشت true;

    صحیح	    new_size = um->prop_شمار + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * اندازه(textprop_T));
    اگر (newprops == پوچ)
	بازگشت false;
    um->props = newprops;
    um->prop_size = new_size;

    بازگشت true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    ایستا تهی
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    اگر (um->buf == پوچ)
	بازگشت;
    اگر (!um->detached && !um_detach(um))
	بازگشت;
    اگر (!um_grow_props(um, 1))
    {
	um->buf = پوچ;
	بازگشت;
    }
    um->props[um->prop_شمار++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    تهی
um_reverse_props(unpacked_memline_T *um)
{
    برای (صحیح i = 0, j = um->prop_شمار - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, صحیح prop_شمار)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    اگر (lnum < 1 || lnum > buf->b_ml.ml_سطر_شمار)
    {
	um.buf = پوچ;
	بازگشت um;
    }

    char_u *سطر = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(سطر);

    اگر (text_copy == پوچ)
    {
	um.buf = پوچ;
	بازگشت um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    اگر (prop_شمار > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_شمار);
	اگر (um.props == پوچ)
	{
	    vim_free(um.text);
	    um.buf = پوچ;
	    بازگشت um;
	}
	um.prop_size = prop_شمار;
    }

    بازگشت um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    ایستا تهی
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					صحیح tp_id, صحیح tp_type, صحیح flag)
{
    unpacked_memline_T	neighbor;

    اگر (lnum < 1 || lnum > buf->b_ml.ml_سطر_شمار)
	بازگشت;

    neighbor = um_open_at(buf, lnum, 0);
    اگر (neighbor.buf == پوچ)
	بازگشت;

    برای (صحیح i = 0; i < neighbor.prop_شمار; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	اگر (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    اگر (!neighbor.detached && !um_detach(&neighbor))
		بشکن;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    بشکن;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    تهی
um_delete_prop(unpacked_memline_T *um, صحیح index)
{
    textprop_T *prop;

    اگر (um->buf == پوچ || index < 0 || index >= um->prop_شمار)
	بازگشت;
    اگر (!um->detached && !um_detach(um))
	بازگشت;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    اگر (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    اگر (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    اگر (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	VIM_CLEAR(prop->u.tp_text);
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
    ایستا char_u *
um_pack(unpacked_memline_T *um, صحیح *packed_len)
{
    uint16_t	live_شمار = 0;
    صحیح		vtext_size = 0;
    صحیح		total_len;
    char_u	*buf;
    char_u	*شمار_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    برای (صحیح i = 0; i < um->prop_شمار; ++i)
    {
	textprop_T *prop = &um->props[i];

	اگر (prop->tp_flags & TP_FLAG_DELETED)
	    ادامه;
	++live_شمار;
	اگر (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    اگر (live_شمار == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	اگر (buf == پوچ)
	    بازگشت پوچ;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	بازگشت buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (صحیح)PROP_شمار_SIZE
		+ live_شمار * (صحیح)اندازه(textprop_T) + vtext_size;
    buf = alloc(total_len);
    اگر (buf == پوچ)
	بازگشت پوچ;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    شمار_dest = buf + um->text_size;
    mch_memmove(شمار_dest, &live_شمار, PROP_شمار_SIZE);

    prop_dest = شمار_dest + PROP_شمار_SIZE;
    vtext_dest = prop_dest + live_شمار * اندازه(textprop_T);

    برای (صحیح i = 0; i < um->prop_شمار; ++i)
    {
	textprop_T prop = um->props[i];

	اگر (prop.tp_flags & TP_FLAG_DELETED)
	    ادامه;

	اگر (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    صحیح copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - شمار_dest);
	    vtext_dest += copysize;
	}
	وگرنه
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, اندازه(textprop_T));
	prop_dest += اندازه(textprop_T);
    }

    بازگشت buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    ایستا تهی
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    صحیح	    packed_len;

    اگر (!um->detached || um->buf == پوچ || um->lnum == 0)
	بازگشت;

    packed = um_pack(um, &packed_len);
    اگر (packed == پوچ)
    {
	um->buf = پوچ;
	بازگشت;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    اگر (um->buf->b_ml.ml_سطر_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    وگرنه اگر (!(um->buf->b_ml.ml_flags & ML_سطر_DIRTY))
	(تهی)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_شمار);

    اگر (um->buf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_سطر_ptr);
    um->buf->b_ml.ml_سطر_ptr = packed;
    um->buf->b_ml.ml_سطر_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_سطر_DIRTY;

    um->detached = false;
    um->text = packed;
    um->text_size = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_سطر_textlen = um->text_size;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    تهی
um_close(unpacked_memline_T *um)
{
    اگر (um->buf == پوچ)
	بازگشت;
    اگر (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = پوچ;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    تهی
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = پوچ;
}

/*
 * In a hashtable item "hi_key" points to "pt_name" in a proptype_T.
 * This avoids adding a pointer to the hashtable item.
 * PT2HIKEY() converts a proptype pointer to a hashitem key pointer.
 * HIKEY2PT() converts a hashitem key pointer to a proptype pointer.
 * HI2PT() converts a hashitem pointer to a proptype pointer.
 */
#define PT2HIKEY(p)  ((p)->pt_name)
#define HIKEY2PT(p)   ((proptype_T *)((p) - offsetof(proptype_T, pt_name)))
#define HI2PT(hi)      HIKEY2PT((hi)->hi_کلید)

// The global text property types.
ایستا hashtab_T *global_proptypes = پوچ;
ایستا proptype_T **global_proparray = پوچ;

// The last used text property type ID.
ایستا صحیح proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    ایستا hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    اگر (*name == NUL)
	بازگشت پوچ;
    اگر (buf == پوچ)
	ht = global_proptypes;
    وگرنه
	ht = buf->b_proptypes;

    اگر (ht == پوچ)
	بازگشت پوچ;
    hi = hash_find(ht, name);
    اگر (HASHITEM_EMPTY(hi))
	بازگشت پوچ;
    بازگشت hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    ایستا proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    اگر (hi == پوچ)
	بازگشت پوچ;
    بازگشت HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    صحیح
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    اگر (pt == پوچ)
	بازگشت 0;
    بازگشت pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    ایستا proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = find_prop_type(name, buf);

    اگر (type == پوچ)
	type = find_prop_type(name, پوچ);
    اگر (type == پوچ)
	semsg(_(e_property_type_str_does_not_exist), name);
    بازگشت type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    ایستا صحیح
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    اگر (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	بازگشت FAIL;
    }
    اگر (arg->vval.v_dict == پوچ)
	بازگشت OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    اگر (di != پوچ && (di->di_tv.v_type != VAR_عدد
					      || di->di_tv.vval.v_عدد != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	اگر (*buf == پوچ)
	    بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    تهی
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	بازگشت;

    start_lnum = tv_get_عدد(&argvars[0]);
    start_col = tv_get_عدد(&argvars[1]);
    اگر (check_for_dict_arg(argvars, 2) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    ایستا صحیح
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	صحیح		id,
	char_u		*text_arg,
	صحیح		text_padding_چپ,
	صحیح		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    صحیح		proplen;
    char_u	*props = پوچ;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    صحیح		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    صحیح		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    اگر (type == پوچ)
	برو theend;

    اگر (start_lnum < 1 || start_lnum > buf->b_ml.ml_سطر_شمار)
    {
	semsg(_(e_invalid_سطر_عدد_nr), (long)start_lnum);
	برو theend;
    }
    اگر (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_سطر_شمار)
    {
	semsg(_(e_invalid_سطر_عدد_nr), (long)end_lnum);
	برو theend;
    }

    اگر (buf->b_ml.ml_mfp == پوچ)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	برو theend;
    }

    اگر (text != پوچ)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	برای (p = text; *p != NUL; MB_PTR_ADV(p))
	    اگر (*p < ' ')
		*p = ' ';
    }

    برای (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	اگر (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_سطر));
	    برو theend;
	}

	اگر (lnum == start_lnum)
	    col = start_col;
	وگرنه
	    col = 1;
	اگر (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != پوچ))
	{
	    semsg(_(e_invalid_column_عدد_nr), (long)start_col);
	    برو theend;
	}
	sort_col = col;

	اگر (lnum == end_lnum)
	    length = end_col - col;
	وگرنه
	    length = (صحیح)textlen - col + 1;
	اگر (length > (long)textlen)
	    length = (صحیح)textlen;	// can include the end-of-line
	اگر (length < 0)
	    length = 0;		// zero-width property

	اگر (text_arg != پوچ)
	{
	    length = (long)STRLEN(text_arg);
	    اگر (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		اگر ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
		    sort_col = MAXCOL;
	    }
	}

	// Build the new property.
	CLEAR_FIELD(tmp_prop);
	tmp_prop.tp_col = col;
	tmp_prop.tp_len = length;
	tmp_prop.tp_id = id;
	tmp_prop.tp_type = type->pt_id;
	tmp_prop.tp_flags = text_flags
			    | (lnum > start_lnum ? TP_FLAG_CONT_PREV : 0)
			    | (lnum < end_lnum ? TP_FLAG_CONT_NEXT : 0)
			    | ((type->pt_flags & PT_FLAG_INS_START_INCL)
						     ? TP_FLAG_START_INCL : 0);
	tmp_prop.tp_padleft = text_padding_چپ;
	اگر (text_arg != پوچ)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	برای (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * اندازه(textprop_T),
							   اندازه(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    اگر (prop_col >= sort_col)
		بشکن;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	صحیح		vtext_total = 0;
	صحیح		new_سطر_len;
	char_u	*شمار_dest;
	char_u	*vtext_dest;
	صحیح		j;

	// Compute total vtext size from existing props.
	برای (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * اندازه(textprop_T),
							اندازه(textprop_T));
	    اگر (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	اگر (text_arg != پوچ)
	    vtext_total += length + 1;

	new_سطر_len = (صحیح)textlen + (صحیح)PROP_شمار_SIZE
			+ new_propcount * (صحیح)اندازه(textprop_T)
			+ vtext_total;
	newtext = alloc(new_سطر_len);
	اگر (newtext == پوچ)
	    برو theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_سطر_ptr, textlen);

	// Write prop_count.
	شمار_dest = newtext + textlen;
	mch_memmove(شمار_dest, &new_propcount, PROP_شمار_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = شمار_dest + PROP_شمار_SIZE;
	اگر (i > 0)
	    mch_memmove(newprops, props, اندازه(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	اگر (i < proplen)
	    mch_memmove(newprops + (i + 1) * اندازه(textprop_T),
					props + i * اندازه(textprop_T),
					اندازه(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * اندازه(textprop_T);
	برای (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    اگر (j == i)
		ادامه;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * اندازه(textprop_T),
		    اندازه(textprop_T));
	    اگر (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_شمار_2 = (char_u *)props - PROP_شمار_SIZE;
		char_u *old_vtext = old_شمار_2 + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - شمار_dest);
		mch_memmove(newprops + j * اندازه(textprop_T), &ep,
			اندازه(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	اگر (text_arg != پوچ)
	{
	    صحیح copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - شمار_dest);
	    vtext_dest += copysize;
	}
	وگرنه
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * اندازه(textprop_T), &tmp_prop,
							اندازه(textprop_T));

	اگر (buf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_سطر_ptr);
	buf->b_ml.ml_سطر_ptr = newtext;
	buf->b_ml.ml_سطر_len = new_سطر_len;
	buf->b_ml.ml_flags |= ML_سطر_DIRTY;
    }

    changed_سطر_display_buf(buf);
    changed_سطرها_buf(buf, start_lnum, end_lnum + 1, 0);
    res = OK;

theend:
    vim_free(text);
    بازگشت res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    تهی
f_prop_add_list(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    صحیح		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    صحیح		error = FALSE;
    صحیح		prev_did_emsg = did_emsg;

    اگر (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	بازگشت;

    اگر (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	بازگشت;

    dict = argvars[0].vval.v_dict;
    اگر (dict == پوچ || !dict_has_کلید(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	بازگشت;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    اگر (dict_has_کلید(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_عدد(dict, "id");
	اگر (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    بازگشت;
	}
	id = (صحیح)x;
    }

    اگر (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	بازگشت;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_اقلام(argvars[1].vval.v_list, li)
    {
	اگر (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == پوچ)
	{
	    emsg(_(e_list_required));
	    بازگشت;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	اگر (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	اگر (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	اگر (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	صحیح this_id = id;
	اگر (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	اگر (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    اگر (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    بازگشت;
	}
	اگر (prop_add_one(buf, type_name, this_id, پوچ, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    بازگشت;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
ایستا صحیح vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    ایستا صحیح
get_textprop_id(تهی)
{
    اگر (vt_id_counter > 0)
	vt_id_counter = 0;
    بازگشت --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    صحیح
prop_add_common(
	linenr_T    start_lnum,
	colnr_T	    start_col,
	dict_T	    *dict,
	buf_T	    *default_buf,
	typval_T    *dict_arg)
{
    linenr_T	end_lnum;
    colnr_T	end_col;
    char_u	*type_name;
    buf_T	*buf = default_buf;
    صحیح		id = 0;
    char_u	*text = پوچ;
    صحیح		text_padding_چپ = 0;
    صحیح		flags = 0;

    اگر (dict == پوچ || !dict_has_کلید(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	برو theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    اگر (dict_has_کلید(dict, "end_lnum"))
    {
	end_lnum = dict_get_عدد(dict, "end_lnum");
	اگر (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_مقدار_for_argument_str), "end_lnum");
	    برو theend;
	}
    }
    وگرنه
	end_lnum = start_lnum;

    اگر (dict_has_کلید(dict, "length"))
    {
	long length = dict_get_عدد(dict, "length");

	اگر (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_مقدار_for_argument_str), "length");
	    برو theend;
	}
	end_col = start_col + length;
    }
    وگرنه اگر (dict_has_کلید(dict, "end_col"))
    {
	end_col = dict_get_عدد(dict, "end_col");
	اگر (end_col <= 0)
	{
	    semsg(_(e_invalid_مقدار_for_argument_str), "end_col");
	    برو theend;
	}
    }
    وگرنه اگر (start_lnum == end_lnum)
	end_col = start_col;
    وگرنه
	end_col = 1;

    اگر (dict_has_کلید(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_عدد(dict, "id");
	اگر (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    برو theend;
	}
	id = (صحیح)x;
    }

    اگر (dict_has_کلید(dict, "text"))
    {
	اگر (dict_has_کلید(dict, "length")
		|| dict_has_کلید(dict, "end_col")
		|| dict_has_کلید(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    برو theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	اگر (text == پوچ)
	    برو theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	اگر (dict_has_کلید(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    اگر (p == پوچ)
		برو theend;
	    اگر (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		برو theend;
	    }
	    اگر (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_راست;
	    وگرنه اگر (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    وگرنه اگر (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    وگرنه اگر (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_مقدار_for_argument_str_str), "text_align", p);
		برو theend;
	    }
	}

	اگر (dict_has_کلید(dict, "text_padding_left"))
	{
	    text_padding_چپ = dict_get_عدد(dict, "text_padding_left");
	    اگر (text_padding_چپ < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		برو theend;
	    }
	}

	اگر (dict_has_کلید(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    اگر (p == پوچ)
		برو theend;
	    اگر (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    وگرنه اگر (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_مقدار_for_argument_str_str), "text_wrap", p);
		برو theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    اگر (start_col < (text == پوچ ? 1 : 0))
    {
	semsg(_(e_invalid_column_عدد_nr), (long)start_col);
	برو theend;
    }
    اگر (start_col > 0 && text_padding_چپ > 0)
    {
	emsg(_(e_can_only_use_چپ_padding_when_column_is_zero));
	برو theend;
    }

    اگر (dict_arg != پوچ && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	برو theend;

    اگر (text != پوچ)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    وگرنه اگر (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	برو theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_چپ, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = پوچ;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    بازگشت id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    صحیح
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, صحیح will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_شمار;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    اگر ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == پوچ)
	بازگشت 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_سطر_len - textlen;
    اگر (propdata_len == 0)
	بازگشت 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    اگر (propdata_len < PROP_شمار_SIZE + اندازه(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	بازگشت 0;
    }
    mch_memmove(&prop_شمار, text + textlen, PROP_شمار_SIZE);
    اگر (!text_prop_شمار_valid(prop_شمار, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	بازگشت 0;
    }
    اگر (!text_prop_vtext_valid(text + textlen + PROP_شمار_SIZE,
					   (صحیح)prop_شمار, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	بازگشت 0;
    }
    *props = text + textlen + PROP_شمار_SIZE;
    بازگشت (صحیح)prop_شمار;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    صحیح
prop_شمار_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    صحیح		شمار = get_text_props(buf, lnum, &props, FALSE);
    صحیح		result = 0;
    textprop_T	prop;
    صحیح		i;
    صحیح		next_راست_goes_below = FALSE;

    اگر (شمار == 0)
	بازگشت 0;
    برای (i = 0; i < شمار; ++i)
    {
	mch_memmove(&prop, props + i * اندازه(prop), اندازه(prop));
	اگر (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    اگر ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_راست_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_راست)))
	    {
		++result;
	    }
	    وگرنه اگر (prop.tp_flags & TP_FLAG_ALIGN_راست)
		next_راست_goes_below = TRUE;
	}
    }
    بازگشت result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    صحیح
شمار_props(linenr_T lnum, صحیح only_starting, صحیح last_سطر)
{
    char_u	*props;
    صحیح		proplen = get_text_props(curbuf, lnum, &props, 0);
    صحیح		result = proplen;
    textprop_T	prop;

    برای (صحیح i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * اندازه(prop), اندازه(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	اگر ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_سطر && prop.tp_col == MAXCOL))
	    --result;
    }
    بازگشت result;
}

ایستا textprop_T	*text_prop_compare_props;
ایستا buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    ایستا صحیح
text_prop_order(صحیح flags)
{
    اگر (flags & TP_FLAG_ALIGN_ABOVE)
	بازگشت 0;
    اگر (flags & TP_FLAG_ALIGN_راست)
	بازگشت 2;
    اگر (flags & TP_FLAG_ALIGN_BELOW)
	بازگشت 3;
    بازگشت 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    ایستا صحیح
text_prop_compare(ثابت تهی *s1, ثابت تهی *s2)
{
    صحیح  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(صحیح *)s1;
    idx2 = *(صحیح *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    اگر ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	بازگشت tp1->tp_id < 0 ? 1 : -1;

    اگر (col1 == MAXCOL || col2 == MAXCOL)
    {
	صحیح order1 = text_prop_order(tp1->tp_flags);
	صحیح order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	اگر (order1 != order2)
	    بازگشت order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    اگر (pt1 != pt2)
    {
	اگر (pt1 == پوچ)
	    بازگشت -1;
	اگر (pt2 == پوچ)
	    بازگشت 1;
	اگر (pt1->pt_priority != pt2->pt_priority)
	    بازگشت pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    اگر (col1 != col2)
	بازگشت col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    اگر (tp1->tp_id < 0)
	بازگشت tp1->tp_id > tp2->tp_id ? 1 : -1;

    بازگشت 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    تهی
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	صحیح	    *idxs,
	صحیح	    شمار)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((تهی *)idxs, (size_t)شمار, اندازه(صحیح), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_سطرها(
	win_T	    *wp,
	صحیح	    type_id,
	صحیح	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    اگر (text_prop_type_by_id(wp->w_buffer, type_id) == پوچ)
	بازگشت false;

    اگر (first_lnum < 1)
	first_lnum = 1;
    اگر (last_lnum > wp->w_buffer->b_ml.ml_سطر_شمار)
	last_lnum = wp->w_buffer->b_ml.ml_سطر_شمار;

    برای (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	صحیح	شمار = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	برای (صحیح i = 0; i < شمار; ++i)
	{
	    mch_memmove(prop, props + i * اندازه(textprop_T),
							   اندازه(textprop_T));
	    اگر (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		بازگشت true;
	    }
	}
    }
    بازگشت false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	صحیح	    type_id,
	صحیح	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    بازگشت find_prop_in_سطرها(wp, type_id, id, prop, found_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    ایستا تهی
set_text_props(linenr_T lnum, textprop_T *tps, صحیح شمار)
{
    char_u  *text;
    char_u  *newtext;
    صحیح	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    اگر (شمار == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	اگر (newtext == پوچ)
	    بازگشت;
	mch_memmove(newtext, text, textlen);
	اگر (curbuf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_سطر_ptr);
	curbuf->b_ml.ml_سطر_ptr = newtext;
	curbuf->b_ml.ml_سطر_len = textlen;
	curbuf->b_ml.ml_flags |= ML_سطر_DIRTY;
    }
    وگرنه
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_شمار = (uint16_t)شمار;
	صحیح	    vtext_size = 0;
	صحیح	    total_len;
	char_u	    *شمار_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	صحیح	    i;

	برای (i = 0; i < شمار; ++i)
	    اگر (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (صحیح)PROP_شمار_SIZE
		    + شمار * (صحیح)اندازه(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	اگر (newtext == پوچ)
	    بازگشت;
	mch_memmove(newtext, text, textlen);

	شمار_dest = newtext + textlen;
	mch_memmove(شمار_dest, &prop_شمار, PROP_شمار_SIZE);

	prop_dest = شمار_dest + PROP_شمار_SIZE;
	vtext_dest = prop_dest + شمار * اندازه(textprop_T);

	برای (i = 0; i < شمار; ++i)
	{
	    textprop_T prop = tps[i];

	    اگر (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		صحیح copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = پوچ;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - شمار_dest);
		vtext_dest += copysize;
	    }
	    وگرنه
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, اندازه(textprop_T));
	    prop_dest += اندازه(textprop_T);
	}

	اگر (curbuf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_سطر_ptr);
	curbuf->b_ml.ml_سطر_ptr = newtext;
	curbuf->b_ml.ml_سطر_len = total_len;
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
props_add_شمار_header(char_u *سطر, صحیح سطر_len, صحیح textlen, صحیح *new_len)
{
    صحیح		prop_bytes = سطر_len - textlen;
    uint16_t	prop_شمار;
    صحیح		vtext_size = 0;
    صحیح		total;
    char_u	*newline;
    char_u	*شمار_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    صحیح		i;

    اگر (prop_bytes <= 0 || prop_bytes % اندازه(textprop_T) != 0)
    {
	*new_len = سطر_len;
	بازگشت پوچ;
    }
    prop_شمار = (uint16_t)(prop_bytes / اندازه(textprop_T));

    // Calculate total vtext size.
    برای (i = 0; i < (صحیح)prop_شمار; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, سطر + textlen + i * اندازه(textprop_T),
							اندازه(textprop_T));
	اگر (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (صحیح)PROP_شمار_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    اگر (newline == پوچ)
	بازگشت پوچ;
    *new_len = total;

    mch_memmove(newline, سطر, textlen);
    شمار_dest = newline + textlen;
    mch_memmove(شمار_dest, &prop_شمار, PROP_شمار_SIZE);
    prop_dest = شمار_dest + PROP_شمار_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    برای (i = 0; i < (صحیح)prop_شمار; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, سطر + textlen + i * اندازه(textprop_T),
							اندازه(textprop_T));
	اگر (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    صحیح copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - شمار_dest);
	    vtext_dest += copysize;
	}
	وگرنه
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * اندازه(textprop_T), &prop,
							اندازه(textprop_T));
    }
    بازگشت newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    تهی
add_text_props(linenr_T lnum, textprop_T *text_props, صحیح text_prop_شمار)
{
    unpacked_memline_T	um;
    صحیح			i;

    um = um_open_at(curbuf, lnum, text_prop_شمار);
    اگر (um.buf == پوچ)
	بازگشت;
    اگر (!um_detach(&um))
    {
	um_abort(&um);
	بازگشت;
    }

    // Grow props array if needed.
    اگر (um.prop_شمار + text_prop_شمار > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_شمار + text_prop_شمار) * اندازه(textprop_T));
	اگر (newprops == پوچ)
	{
	    um_abort(&um);
	    بازگشت;
	}
	um.props = newprops;
	um.prop_size = um.prop_شمار + text_prop_شمار;
    }

    برای (i = 0; i < text_prop_شمار; ++i)
	um.props[um.prop_شمار++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    ایستا صحیح
compare_pt(ثابت تهی *s1, ثابت تهی *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    بازگشت tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    ایستا proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, صحیح id)
{
    صحیح کم = 0;
    صحیح بالا;

    اگر (ht == پوچ || ht->ht_used == 0)
	بازگشت پوچ;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    اگر (*array == پوچ)
    {
	long	    todo;
	hashitem_T  *hi;
	صحیح	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	اگر (*array == پوچ)
	    بازگشت پوچ;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_اقلام(ht, hi, todo)
	{
	    اگر (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((تهی *)*array, ht->ht_used, اندازه(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    بالا = ht->ht_used;
    وقتی (بالا > کم)
    {
	صحیح m = (بالا + کم) / 2;

	اگر ((*array)[m]->pt_id == id)
	    بازگشت (*array)[m];
	اگر ((*array)[m]->pt_id > id)
	    بالا = m;
	وگرنه
	    کم = m + 1;
    }
    بازگشت پوچ;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    ایستا تهی
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    صحیح buflocal = TRUE;
    صحیح virtualtext_prop = prop->tp_id < 0;

    dict_add_عدد(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    اگر (!virtualtext_prop)
    {
	dict_add_عدد(dict, "length", prop->tp_len);
	dict_add_عدد(dict, "id", prop->tp_id);
    }
    dict_add_عدد(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_عدد(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    اگر (pt == پوچ)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    اگر (pt != پوچ)
	dict_add_string(dict, "type", pt->pt_name);

    اگر (buflocal)
	dict_add_عدد(dict, "type_bufnr", buf->b_fnum);
    وگرنه
	dict_add_عدد(dict, "type_bufnr", 0);
    اگر (virtualtext_prop)
    {
	string_T    text_align = {پوچ, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	اگر (prop->tp_flags & TP_FLAG_ALIGN_راست)
	    STR_LITERAL_SET(text_align, "right");
	وگرنه اگر (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	وگرنه اگر (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	اگر (text_align.string != پوچ)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (صحیح)text_align.length);

	// text_wrap
	اگر (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	اگر (prop->tp_padleft != 0)
	    dict_add_عدد(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, صحیح id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    اگر (type == پوچ)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    بازگشت type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    بازگشت text_prop_type_by_id(buf, prop->tp_type) != پوچ;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    تهی
f_prop_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    linenr_T start;
    linenr_T end;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    صحیح	    did_clear = FALSE;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	بازگشت;

    start = tv_get_عدد(&argvars[0]);
    end = start;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_عدد(&argvars[1]);
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    اگر (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		بازگشت;
	}
    }
    اگر (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	بازگشت;
    }

    {
	unpacked_memline_T um = um_open(buf);

	برای (lnum = start; lnum <= end; ++lnum)
	{
	    صحیح idx;

	    اگر (lnum > buf->b_ml.ml_سطر_شمار)
		بشکن;
	    اگر (!um_goto_سطر(&um, lnum, 0))
		بشکن;
	    اگر (um.prop_شمار > 0)
	    {
		did_clear = TRUE;
		برای (idx = um.prop_شمار - 1; idx >= 0; --idx)
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
    تهی
f_prop_find(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    صحیح		lnum_start;
    صحیح		start_pos_has_prop = 0;
    صحیح		seen_end = FALSE;
    صحیح		id = 0;
    صحیح		id_found = FALSE;
    صحیح		type_id = -1;
    صحیح		skipstart = FALSE;
    صحیح		lnum = -1;
    صحیح		col = -1;
    صحیح		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    صحیح		both;

    اگر (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	بازگشت;
    dict = argvars[0].vval.v_dict;

    اگر (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	بازگشت;
    اگر (buf->b_ml.ml_mfp == پوچ)
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	اگر (*dir_s == 'b')
	    dir = BACKWARD;
	وگرنه اگر (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    بازگشت;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    اگر (di != پوچ)
	lnum = tv_get_عدد(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    اگر (di != پوچ)
	col = tv_get_عدد(&di->di_tv);

    اگر (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    وگرنه اگر (col == -1)
	col = 1;

    اگر (lnum < 1 || lnum > buf->b_ml.ml_سطر_شمار)
    {
	emsg(_(e_invalid_range));
	بازگشت;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    اگر (dict_has_کلید(dict, "id"))
    {
	id = dict_get_عدد(dict, "id");
	id_found = TRUE;
    }
    اگر (dict_has_کلید(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	اگر (type == پوچ)
	    بازگشت;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    اگر (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	بازگشت;
    }
    اگر (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	بازگشت;
    }

    lnum_start = lnum;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;

    وقتی (1)
    {
	char_u	*prop_start_ptr;
	صحیح	شمار = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	صحیح	    i;
	textprop_T  prop;
	صحیح	    prop_start;
	صحیح	    prop_end;

	برای (i = dir == BACKWARD ? شمار - 1 : 0; i >= 0 && i < شمار; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * اندازه(textprop_T),
							   اندازه(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    اگر (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_شمار_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    وگرنه
		prop.u.tp_text = پوچ;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    اگر (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		اگر (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    اگر (!is_floating_vtext && prop.tp_col > col)
			ادامه;
		}
		وگرنه اگر (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    ادامه;
	    }
	    اگر (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		اگر (lnum_start == lnum
			&& col >= prop.tp_col
			&& (col <= prop.tp_col + prop.tp_len
							 - (prop.tp_len != 0)))
		    start_pos_has_prop = 1;

		// The property was not continued from last line, it starts on
		// this line.
		prop_start = !(prop.tp_flags & TP_FLAG_CONT_PREV);
		// The property does not continue on the next line, it ends on
		// this line.
		prop_end = !(prop.tp_flags & TP_FLAG_CONT_NEXT);
		اگر (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		اگر (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    اگر (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    ادامه;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		اگر (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    ادامه;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_عدد(rettv->vval.v_dict, "lnum", lnum);

		بازگشت;
	    }
	}

	اگر (dir > 0)
	{
	    اگر (lnum >= buf->b_ml.ml_سطر_شمار)
		بشکن;
	    lnum++;
	}
	وگرنه
	{
	    اگر (lnum <= 1)
		بشکن;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    ایستا صحیح
prop_type_or_id_in_list(صحیح *types_or_ids, صحیح len, صحیح type_or_id)
{
    صحیح i;

    برای (i = 0; i < len; i++)
	اگر (types_or_ids[i] == type_or_id)
	    بازگشت TRUE;

    بازگشت FALSE;
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
    ایستا تهی
get_props_in_سطر(
	buf_T		*buf,
	linenr_T	lnum,
	صحیح		*prop_types,
	صحیح		prop_types_len,
	صحیح		*prop_ids,
	صحیح		prop_ids_len,
	list_T		*retlist,
	صحیح		add_lnum)
{
    char_u	*props;
    صحیح		شمار;
    صحیح		i;
    textprop_T	prop;

    شمار = get_text_props(buf, lnum, &props, FALSE);
    برای (i = 0; i < شمار; ++i)
    {
	mch_memmove(&prop, props + i * اندازه(textprop_T),
		اندازه(textprop_T));
	// Convert offset to pointer for virtual text props.
	اگر (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_شمار_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	وگرنه
	    prop.u.tp_text = پوچ;
	اگر ((prop_types == پوچ
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == پوچ
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    اگر (d == پوچ)
		بشکن;
	    prop_fill_dict(d, &prop, buf);
	    اگر (add_lnum)
		dict_add_عدد(d, "lnum", lnum);
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
    ایستا صحیح *
get_prop_types_from_names(list_T *l, buf_T *buf, صحیح *رقم_types)
{
    صحیح		*prop_types;
    listitem_T	*li;
    صحیح		i;
    char_u	*name;
    proptype_T	*type;

    *رقم_types = 0;

    prop_types = ALLOC_MULT(صحیح, list_len(l));
    اگر (prop_types == پوچ)
	بازگشت پوچ;

    i = 0;
    FOR_ALL_LIST_اقلام(l, li)
    {
	اگر (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    برو errret;
	}
	name = li->li_tv.vval.v_string;
	اگر (name == پوچ)
	    برو errret;

	type = lookup_prop_type(name, buf);
	اگر (type == پوچ)
	    برو errret;
	prop_types[i++] = type->pt_id;
    }

    *رقم_types = i;
    بازگشت prop_types;

errret:
    VIM_CLEAR(prop_types);
    بازگشت پوچ;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    ایستا صحیح *
get_prop_ids_from_list(list_T *l, صحیح *رقم_ids)
{
    صحیح		*prop_ids;
    listitem_T	*li;
    صحیح		i = 0;
    صحیح		id;
    صحیح		error;

    *رقم_ids = 0;

    prop_ids = ALLOC_MULT(صحیح, list_len(l));
    اگر (prop_ids == پوچ)
	بازگشت پوچ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اقلام(l, li)
    {
	error = FALSE;
	id = tv_get_عدد_chk(&li->li_tv, &error);
	اگر (error)
	    برو errret;

	prop_ids[i++] = id;
    }

    *رقم_ids = i;
    بازگشت prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    بازگشت پوچ;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    تهی
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    buf_T	*buf = curbuf;
    صحیح		add_lnum = FALSE;
    صحیح		*prop_types = پوچ;
    صحیح		prop_types_len = 0;
    صحیح		*prop_ids = پوچ;
    صحیح		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    // default: get text properties on current line
    start_lnum = tv_get_عدد(&argvars[0]);
    end_lnum = start_lnum;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	اگر (check_for_dict_arg(argvars, 1) == FAIL)
	    بازگشت;
	d = argvars[1].vval.v_dict;

	اگر (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    بازگشت;

	اگر (d != پوچ && (di = dict_find(d, (char_u *)"end_lnum", -1)) != پوچ)
	{
	    اگر (di->di_tv.v_type != VAR_عدد)
	    {
		emsg(_(e_عدد_required));
		بازگشت;
	    }
	    end_lnum = tv_get_عدد(&di->di_tv);
	    اگر (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//سطر
		end_lnum = buf->b_ml.ml_سطر_شمار + end_lnum + 1;
	    وگرنه اگر (end_lnum > buf->b_ml.ml_سطر_شمار)
		end_lnum = buf->b_ml.ml_سطر_شمار;
	    add_lnum = TRUE;
	}
	اگر (d != پوچ && (di = dict_find(d, (char_u *)"types", -1)) != پوچ)
	{
	    اگر (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		بازگشت;
	    }

	    l = di->di_tv.vval.v_list;
	    اگر (l != پوچ && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		اگر (prop_types == پوچ)
		    بازگشت;
	    }
	}
	اگر (d != پوچ && (di = dict_find(d, (char_u *)"ids", -1)) != پوچ)
	{
	    اگر (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		برو errret;
	    }

	    l = di->di_tv.vval.v_list;
	    اگر (l != پوچ && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		اگر (prop_ids == پوچ)
		    برو errret;
	    }
	}
    }
    اگر (start_lnum < 1 || start_lnum > buf->b_ml.ml_سطر_شمار
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    وگرنه
	برای (lnum = start_lnum; lnum <= end_lnum; lnum++)
	    get_props_in_سطر(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, add_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    تهی
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start = 1;
    linenr_T	end = 0;
    linenr_T	lnum;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    صحیح		do_all;
    صحیح		id = -MAXCOL;
    صحیح		type_id = -1;	    // for a single "type"
    صحیح		*type_ids = پوچ;   // array, for a list of "types", allocated
    صحیح		رقم_type_ids = 0;   // number of elements in "type_ids"
    صحیح		both;

    rettv->vval.v_عدد = 0;

    اگر (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 2) == FAIL)))
	بازگشت;

    اگر (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_عدد(&argvars[1]);
	end = start;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_عدد(&argvars[2]);
	اگر (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    بازگشت;
	}
    }

    dict = argvars[0].vval.v_dict;
    اگر (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	بازگشت;
    اگر (buf->b_ml.ml_mfp == پوچ)
	بازگشت;

    do_all = dict_get_bool(dict, "all", FALSE);

    اگر (dict_has_کلید(dict, "id"))
	id = dict_get_عدد(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    اگر (dict_has_کلید(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	اگر (type == پوچ)
	    بازگشت;
	type_id = type->pt_id;
    }
    اگر (dict_has_کلید(dict, "types"))
    {
	typval_T types;
	listitem_T *li = پوچ;

	dict_get_tv(dict, "types", &types);
	اگر (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( اندازه(صحیح) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_اقلام(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		اگر (li->li_tv.v_type != VAR_STRING)
		    ادامه;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		اگر (!prop_type)
		    برو cleanup_prop_remove;

		type_ids[رقم_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    اگر (id == -MAXCOL && (type_id == -1 && رقم_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	برو cleanup_prop_remove;
    }
    اگر (both && (id == -MAXCOL || (type_id == -1 && رقم_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	برو cleanup_prop_remove;
    }
    اگر (type_id != -1 && رقم_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	برو cleanup_prop_remove;
    }

    اگر (end == 0)
	end = buf->b_ml.ml_سطر_شمار;

    {
	unpacked_memline_T um = um_open(buf);

	برای (lnum = start; lnum <= end; ++lnum)
	{
	    صحیح idx;

	    اگر (lnum > buf->b_ml.ml_سطر_شمار)
		بشکن;
	    اگر (!um_goto_سطر(&um, lnum, 0))
		بشکن;

	    برای (idx = 0; idx < um.prop_شمار; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		صحیح	    matches_id = 0;
		صحیح	    matches_type = 0;

		اگر (prop->tp_flags & TP_FLAG_DELETED)
		    ادامه;

		matches_id = prop->tp_id == id;
		اگر (رقم_type_ids > 0)
		{
		    صحیح idx2;

		    برای (idx2 = 0; !matches_type && idx2 < رقم_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		وگرنه
		    matches_type = prop->tp_type == type_id;

		اگر (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    اگر (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_عدد;
		    اگر (!do_all)
			بشکن;
		}
	    }
	}
	um_close(&um);
    }

    اگر (first_changed > 0)
    {
	changed_سطر_display_buf(buf);
	changed_سطرها_buf(buf, first_changed, last_changed + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    ایستا تهی
prop_type_set(typval_T *argvars, صحیح add)
{
    char_u	*name;
    buf_T	*buf = پوچ;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    name = tv_get_string(&argvars[0]);
    اگر (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	بازگشت;
    }

    اگر (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	بازگشت;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    اگر (add)
    {
	hashtab_T **htp;

	اگر (prop != پوچ)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    بازگشت;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	اگر (prop == پوچ)
	    بازگشت;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	اگر (buf == پوچ)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	وگرنه
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	اگر (*htp == پوچ)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    اگر (*htp == پوچ)
	    {
		vim_free(prop);
		بازگشت;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    وگرنه
    {
	اگر (prop == پوچ)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    بازگشت;
	}
    }

    اگر (dict != پوچ)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	اگر (di != پوچ)
	{
	    char_u	*highlight;
	    صحیح		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    اگر (highlight != پوچ && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    اگر (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == پوچ ? (char_u *)"" : highlight);
		بازگشت;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	اگر (di != پوچ)
	{
	    اگر (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    وگرنه
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	اگر (di != پوچ)
	{
	    اگر (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    وگرنه
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	اگر (di != پوچ)
	    prop->pt_priority = tv_get_عدد(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	اگر (di != پوچ)
	{
	    اگر (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    وگرنه
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	اگر (di != پوچ)
	{
	    اگر (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    وگرنه
		prop->pt_flags &= ~PT_FLAG_INS_END_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    تهی
f_prop_type_add(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    تهی
f_prop_type_change(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    تهی
f_prop_type_delete(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*name;
    buf_T	*buf = پوچ;
    hashitem_T	*hi;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    name = tv_get_string(&argvars[0]);
    اگر (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	بازگشت;
    }

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	اگر (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    بازگشت;
    }

    hi = find_prop_type_hi(name, buf);
    اگر (hi == پوچ)
	بازگشت;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    اگر (buf == پوچ)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    وگرنه
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    changed_window_setting_buf(buf == پوچ ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    تهی
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    name = tv_get_string(&argvars[0]);
    اگر (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	بازگشت;
    }

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;

    proptype_T  *prop = پوچ;
    buf_T	    *buf = پوچ;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	اگر (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    بازگشت;
    }

    prop = find_prop_type(name, buf);
    اگر (prop == پوچ)
	بازگشت;

    dict_T *d = rettv->vval.v_dict;

    اگر (prop->pt_hl_id > 0)
	dict_add_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_add_عدد(d, "priority", prop->pt_priority);
    dict_add_عدد(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_add_عدد(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_add_عدد(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_START_INCL) ? 1 : 0);
    dict_add_عدد(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_END_INCL) ? 1 : 0);
    اگر (buf != پوچ)
	dict_add_عدد(d, "bufnr", buf->b_fnum);
}

    ایستا تهی
list_types(hashtab_T *ht, list_T *l)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_اقلام(ht, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
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
    تهی
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = پوچ;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	اگر (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    بازگشت;
    }
    اگر (buf == پوچ)
    {
	اگر (global_proptypes != پوچ)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    وگرنه اگر (buf->b_proptypes != پوچ)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    ایستا تهی
clear_ht_prop_types(hashtab_T *ht)
{
    long	todo;
    hashitem_T	*hi;

    اگر (ht == پوچ)
	بازگشت;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_اقلام(ht, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
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
    تهی
clear_global_prop_types(تهی)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = پوچ;
    VIM_CLEAR(global_proparray);
}
#endif

/*
 * Free all property types for "buf".
 */
    تهی
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = پوچ;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
تعریفنوع ساختار
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
    ایستا adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    col,
	صحیح	    added,
	صحیح	    flags)
{
    proptype_T	*pt;
    صحیح		start_incl;
    صحیح		end_incl;
    صحیح		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    اگر (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	بازگشت res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != پوچ && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    اگر (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != پوچ && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    اگر (added > 0)
    {
	اگر (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	وگرنه اگر (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    وگرنه اگر (prop->tp_col > col + 1)
    {
	اگر (prop->tp_col + added < col + 1)
	{
	    اگر (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    وگرنه
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		اگر (prop->tp_len <= 0)
		{
		    prop->tp_len = 0;
		    // Multiline properties with no text left should
		    // also be dropped.
		    res.can_drop = droppable
			|| (prop->tp_flags
				& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
		}
	    }
	}
	وگرنه
	    prop->tp_col += added;
    }
    وگرنه اگر (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	اگر (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	وگرنه
	{
	    صحیح after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	اگر (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    وگرنه
	res.dirty = FALSE;

    بازگشت res;
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
	colnr_T	    col,
	صحیح	    bytes_added,
	صحیح	    flags)
{
    unpacked_memline_T um;
    صحیح		dirty = FALSE;
    صحیح		ri;

    اگر (text_prop_frozen > 0)
	بازگشت FALSE;

    um = um_open_at(curbuf, lnum, 0);
    اگر (um.buf == پوچ || um.prop_شمار == 0)
    {
	um_abort(&um);
	بازگشت FALSE;
    }

    برای (ri = 0; ri < um.prop_شمار; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
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
		    بازگشت FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		اگر ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    بازگشت FALSE;
		}
	    }
	    dirty = TRUE;
	}
	اگر (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    ادامه;
	}
    }
    اگر (dirty)
	um_close(&um);
    وگرنه
	um_abort(&um);
    بازگشت dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    تهی
adjust_props_for_split(
	linenr_T    lnum_props,
	linenr_T    lnum_بالاترین,
	صحیح	    kept,
	صحیح	    deleted,
	صحیح	    at_eol)
{
    char_u	*props;
    صحیح		شمار;
    garray_T    prevprop;
    garray_T    nextprop;
    صحیح		skipped = kept + deleted;

    اگر (!curbuf->b_has_textprop)
	بازگشت;

    // Get the text properties from "lnum_props".
    شمار = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, اندازه(textprop_T), 10);
    ga_init2(&nextprop, اندازه(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *شمار_ptr = props - PROP_شمار_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    برای (صحیح i = 0; i < شمار; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	صحیح	    start_incl, end_incl;
	صحیح	    cont_prev, cont_next;
	صحیح	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * اندازه(textprop_T), اندازه(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	اگر (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(شمار_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	وگرنه
	    prop.u.tp_text = پوچ;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != پوچ && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != پوچ && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	اگر (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	وگرنه
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    صحیح is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    اگر (is_floating_vtext)
		cont_next = FALSE;
	    وگرنه
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	اگر (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	اگر (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    صحیح is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    اگر (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    اگر (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	اگر (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    صحیح is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    اگر (p->tp_col != MAXCOL)
	    {
		اگر (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		وگرنه
		{
		    اگر (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    اگر (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    set_text_props(lnum_بالاترین, (textprop_T *)prevprop.ga_data,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    set_text_props(lnum_بالاترین + 1, (textprop_T *)nextprop.ga_data,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    تهی
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	صحیح		    last_سطر,
	long		    col,
	صحیح		    removed)
{
    unpacked_memline_T	r_um;

    اگر (um->buf == پوچ)
	بازگشت;

    r_um = um_open_at(um->buf, lnum, 0);
    اگر (r_um.buf == پوچ)
	بازگشت;
    اگر (!um_detach(&r_um))
    {
	um_abort(&r_um);
	بازگشت;
    }

    برای (صحیح i = r_um.prop_شمار - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	صحیح	    end;

	اگر (prop->tp_col == MAXCOL && !last_سطر)
	    ادامه;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	اگر (last_سطر || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = پوچ;  // ownership transferred
	}
	وگرنه
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    برای (صحیح j = 0; j < um->prop_شمار; ++j)
	    {
		textprop_T *op = &um->props[j];

		اگر ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    بشکن;
		}
	    }
	    اگر (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    ایستا تهی
remember_cleared_prop(garray_T *gap, صحیح which, textprop_T *prop, صحیح flag)
{
    undoprop_T	*up;

    اگر (which == UNDOPROP_NONE)
	بازگشت;
    اگر (gap->ga_itemsize == 0)
	ga_init2(gap, اندازه(undoprop_T), 4);
    اگر (ga_grow(gap, 1) == FAIL)
	بازگشت;
    up = (undoprop_T *)gap->ga_data + gap->ga_len++;
    up->up_which = which;
    up->up_type = prop->tp_type;
    up->up_id = prop->tp_id;
    up->up_col = prop->tp_col;
    up->up_flag = flag;
}

/*
 * Clear the continuation flags between line "lnum" and the line below it that
 * have no partner in the other line.  A property is split there then.  What
 * is cleared on the line above is remembered in "gap" as "above_which", on
 * the line below as "below_which".
 */
    تهی
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	صحیح	    above_which,
	صحیح	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = پوچ;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    اگر (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_سطر_شمار)
	بازگشت;

    above = um_open_at(buf, lnum, 0);
    اگر (above.buf == پوچ)
	بازگشت;
    برای (صحیح i = 0; i < above.prop_شمار; ++i)
	اگر (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    اگر (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	بازگشت;
    }

    below = um_open_at(buf, lnum + 1, 0);
    اگر (below.buf == پوچ)
    {
	um_abort(&above);
	بازگشت;
    }
    اگر (below.prop_شمار > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_شمار);
	اگر (paired == پوچ)
	    برو theend;
    }

    برای (صحیح i = 0; i < above.prop_شمار; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	اگر (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    ادامه;
	برای (صحیح j = 0; j < below.prop_شمار; ++j)
	{
	    textprop_T *next = &below.props[j];

	    اگر (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		بشکن;
	    }
	}
	اگر (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    برای (صحیح j = 0; j < below.prop_شمار; ++j)
    {
	اگر (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    ادامه;
	اگر (!below.detached && !um_detach(&below))
	    بشکن;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    اگر (below_changed)
	um_close(&below);
    وگرنه
	um_abort(&below);
    اگر (above_changed)
	um_close(&above);
    وگرنه
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    تهی
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	صحیح	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    اگر (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_سطر_شمار)
	بازگشت;
    um = um_open_at(buf, lnum, 0);
    اگر (um.buf == پوچ)
	بازگشت;
    برای (صحیح r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	اگر (up->up_which != which)
	    ادامه;
	برای (صحیح i = 0; i < um.prop_شمار; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    اگر (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		ادامه;
	    اگر (!um.detached && !um_detach(&um))
		برو theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    بشکن;
	}
    }

theend:
    اگر (changed)
	um_close(&um);
    وگرنه
	um_abort(&um);
}

    bool
text_prop_شمار_valid(صحیح prop_شمار, size_t propdata_len)
{
    اگر (propdata_len < PROP_شمار_SIZE)
	بازگشت false;
    بازگشت (size_t)prop_شمار * اندازه(textprop_T)
		    <= propdata_len - PROP_شمار_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, صحیح prop_شمار, size_t propdata_len)
{
    برای (صحیح i = 0; i < prop_شمار; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * اندازه(textprop_T),
							  اندازه(textprop_T));
	اگر (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    ادامه;
	اگر (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    بازگشت false;
    }
    بازگشت true;
}

#endif // FEAT_PROP_POPUP
