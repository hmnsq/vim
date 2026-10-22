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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_PROP_POPUP)

நிலையான வெறுமை um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    நிலையான வெறுமை
um_free_vtext(textprop_T *props, முழு மொத்தம்)
{
    க்கு (முழு i = 0; i < மொத்தம்; ++i)
	எனில் (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    நிலையான வெறுமை
um_free(unpacked_memline_T *um)
{
    எனில் (um->detached)
    {
	VIM_CLEAR(um->text);
	um_free_vtext(um->props, um->prop_மொத்தம்);
    }
    VIM_CLEAR(um->props);
    um->prop_size = 0;
    um->prop_மொத்தம் = 0;
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
    திரும்பு um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_வரி(unpacked_memline_T *um, linenr_T lnum, முழு extra_props)
{
    char_u	*வரி;
    size_t	textlen;
    size_t	propdata_len;
    முழு		proplen;

    எனில் (um->buf == வெற்று)
	திரும்பு false;

    // Store changes to the current line if any.
    எனில் (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    எனில் (lnum == 0)
	திரும்பு true;	// just unload

    வரி = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_வரி_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = வரி;

    எனில் (propdata_len == 0)
	திரும்பு true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    எனில் (propdata_len < PROP_மொத்தம்_SIZE + அளவு(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = வெற்று;
	திரும்பு false;
    }

    uint16_t    prop_மொத்தம்;
    char_u	    *மொத்தம்_ptr = வரி + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_மொத்தம், மொத்தம்_ptr, PROP_மொத்தம்_SIZE);
    எனில் (!text_prop_மொத்தம்_valid(prop_மொத்தம், propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = வெற்று;
	திரும்பு false;
    }
    proplen = (முழு)prop_மொத்தம்;
    props_start = மொத்தம்_ptr + PROP_மொத்தம்_SIZE;

    எனில் (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = வெற்று;
	திரும்பு false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    எனில் (um->props == வெற்று)
    {
	um->buf = வெற்று;
	திரும்பு false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_மொத்தம் = proplen;
    mch_memmove(um->props, props_start, proplen * அளவு(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    க்கு (முழு i = 0; i < proplen; ++i)
    {
	எனில் (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = மொத்தம்_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	இல்லையெனில்
	    um->props[i].u.tp_text = வெற்று;
    }

    திரும்பு true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, முழு extra_props)
{
    unpacked_memline_T um = um_open(buf);

    எனில் (!um_goto_வரி(&um, lnum, extra_props))
	um.buf = வெற்று;
    திரும்பு um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    நிலையான bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    எனில் (um->detached || um->buf == வெற்று)
	திரும்பு um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    எனில் (newtext == வெற்று)
    {
	um->buf = வெற்று;
	திரும்பு false;
    }
    um->text = newtext;

    க்கு (முழு i = 0; i < um->prop_மொத்தம்; ++i)
    {
	எனில் (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    எனில் (copy == வெற்று)
	    {
		க்கு (முழு j = 0; j < i; ++j)
		    எனில் (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = வெற்று;
		திரும்பு false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    திரும்பு true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    எனில் (um->buf == வெற்று)
	திரும்பு false;
    எனில் (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    திரும்பு true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    நிலையான bool
um_grow_props(unpacked_memline_T *um, முழு needed)
{
    எனில் (um->prop_மொத்தம் + needed <= um->prop_size)
	திரும்பு true;

    முழு	    new_size = um->prop_மொத்தம் + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * அளவு(textprop_T));
    எனில் (newprops == வெற்று)
	திரும்பு false;
    um->props = newprops;
    um->prop_size = new_size;

    திரும்பு true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    நிலையான வெறுமை
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    எனில் (um->buf == வெற்று)
	திரும்பு;
    எனில் (!um->detached && !um_detach(um))
	திரும்பு;
    எனில் (!um_grow_props(um, 1))
    {
	um->buf = வெற்று;
	திரும்பு;
    }
    um->props[um->prop_மொத்தம்++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    வெறுமை
um_reverse_props(unpacked_memline_T *um)
{
    க்கு (முழு i = 0, j = um->prop_மொத்தம் - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, முழு prop_மொத்தம்)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    எனில் (lnum < 1 || lnum > buf->b_ml.ml_வரி_மொத்தம்)
    {
	um.buf = வெற்று;
	திரும்பு um;
    }

    char_u *வரி = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(வரி);

    எனில் (text_copy == வெற்று)
    {
	um.buf = வெற்று;
	திரும்பு um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    எனில் (prop_மொத்தம் > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_மொத்தம்);
	எனில் (um.props == வெற்று)
	{
	    vim_free(um.text);
	    um.buf = வெற்று;
	    திரும்பு um;
	}
	um.prop_size = prop_மொத்தம்;
    }

    திரும்பு um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    நிலையான வெறுமை
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					முழு tp_id, முழு tp_type, முழு flag)
{
    unpacked_memline_T	neighbor;

    எனில் (lnum < 1 || lnum > buf->b_ml.ml_வரி_மொத்தம்)
	திரும்பு;

    neighbor = um_open_at(buf, lnum, 0);
    எனில் (neighbor.buf == வெற்று)
	திரும்பு;

    க்கு (முழு i = 0; i < neighbor.prop_மொத்தம்; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	எனில் (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    எனில் (!neighbor.detached && !um_detach(&neighbor))
		நிறுத்து;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    நிறுத்து;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    வெறுமை
um_delete_prop(unpacked_memline_T *um, முழு index)
{
    textprop_T *prop;

    எனில் (um->buf == வெற்று || index < 0 || index >= um->prop_மொத்தம்)
	திரும்பு;
    எனில் (!um->detached && !um_detach(um))
	திரும்பு;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    எனில் (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    எனில் (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    எனில் (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
    நிலையான char_u *
um_pack(unpacked_memline_T *um, முழு *packed_len)
{
    uint16_t	live_மொத்தம் = 0;
    முழு		vtext_size = 0;
    முழு		total_len;
    char_u	*buf;
    char_u	*மொத்தம்_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    க்கு (முழு i = 0; i < um->prop_மொத்தம்; ++i)
    {
	textprop_T *prop = &um->props[i];

	எனில் (prop->tp_flags & TP_FLAG_DELETED)
	    தொடர்;
	++live_மொத்தம்;
	எனில் (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    எனில் (live_மொத்தம் == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	எனில் (buf == வெற்று)
	    திரும்பு வெற்று;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	திரும்பு buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (முழு)PROP_மொத்தம்_SIZE
		+ live_மொத்தம் * (முழு)அளவு(textprop_T) + vtext_size;
    buf = alloc(total_len);
    எனில் (buf == வெற்று)
	திரும்பு வெற்று;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    மொத்தம்_dest = buf + um->text_size;
    mch_memmove(மொத்தம்_dest, &live_மொத்தம், PROP_மொத்தம்_SIZE);

    prop_dest = மொத்தம்_dest + PROP_மொத்தம்_SIZE;
    vtext_dest = prop_dest + live_மொத்தம் * அளவு(textprop_T);

    க்கு (முழு i = 0; i < um->prop_மொத்தம்; ++i)
    {
	textprop_T prop = um->props[i];

	எனில் (prop.tp_flags & TP_FLAG_DELETED)
	    தொடர்;

	எனில் (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    முழு copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - மொத்தம்_dest);
	    vtext_dest += copysize;
	}
	இல்லையெனில்
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, அளவு(textprop_T));
	prop_dest += அளவு(textprop_T);
    }

    திரும்பு buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    நிலையான வெறுமை
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    முழு	    packed_len;

    எனில் (!um->detached || um->buf == வெற்று || um->lnum == 0)
	திரும்பு;

    packed = um_pack(um, &packed_len);
    எனில் (packed == வெற்று)
    {
	um->buf = வெற்று;
	திரும்பு;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    எனில் (um->buf->b_ml.ml_வரி_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    இல்லையெனில் எனில் (!(um->buf->b_ml.ml_flags & ML_வரி_DIRTY))
	(வெறுமை)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_மொத்தம்);

    எனில் (um->buf->b_ml.ml_flags & (ML_வரி_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_வரி_ptr);
    um->buf->b_ml.ml_வரி_ptr = packed;
    um->buf->b_ml.ml_வரி_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_வரி_DIRTY;

    um->detached = false;
    um->text = packed;
    um->text_size = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_வரி_textlen = um->text_size;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    வெறுமை
um_close(unpacked_memline_T *um)
{
    எனில் (um->buf == வெற்று)
	திரும்பு;
    எனில் (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = வெற்று;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    வெறுமை
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = வெற்று;
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
#define HI2PT(hi)      HIKEY2PT((hi)->hi_சாவி)

// The global text property types.
நிலையான hashtab_T *global_proptypes = வெற்று;
நிலையான proptype_T **global_proparray = வெற்று;

// The last used text property type ID.
நிலையான முழு proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    நிலையான hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    எனில் (*name == NUL)
	திரும்பு வெற்று;
    எனில் (buf == வெற்று)
	ht = global_proptypes;
    இல்லையெனில்
	ht = buf->b_proptypes;

    எனில் (ht == வெற்று)
	திரும்பு வெற்று;
    hi = hash_find(ht, name);
    எனில் (HASHITEM_EMPTY(hi))
	திரும்பு வெற்று;
    திரும்பு hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    நிலையான proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    எனில் (hi == வெற்று)
	திரும்பு வெற்று;
    திரும்பு HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    முழு
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    எனில் (pt == வெற்று)
	திரும்பு 0;
    திரும்பு pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    நிலையான proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = find_prop_type(name, buf);

    எனில் (type == வெற்று)
	type = find_prop_type(name, வெற்று);
    எனில் (type == வெற்று)
	semsg(_(e_property_type_str_does_not_exist), name);
    திரும்பு type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    நிலையான முழு
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    எனில் (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	திரும்பு FAIL;
    }
    எனில் (arg->vval.v_dict == வெற்று)
	திரும்பு OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    எனில் (di != வெற்று && (di->di_tv.v_type != VAR_எண்
					      || di->di_tv.vval.v_எண் != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	எனில் (*buf == வெற்று)
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    வெறுமை
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	திரும்பு;

    start_lnum = tv_get_எண்(&argvars[0]);
    start_col = tv_get_எண்(&argvars[1]);
    எனில் (check_for_dict_arg(argvars, 2) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    நிலையான முழு
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	முழு		id,
	char_u		*text_arg,
	முழு		text_padding_இடது,
	முழு		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    முழு		proplen;
    char_u	*props = வெற்று;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    முழு		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    முழு		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    எனில் (type == வெற்று)
	செல் theend;

    எனில் (start_lnum < 1 || start_lnum > buf->b_ml.ml_வரி_மொத்தம்)
    {
	semsg(_(e_invalid_வரி_எண்_nr), (long)start_lnum);
	செல் theend;
    }
    எனில் (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_வரி_மொத்தம்)
    {
	semsg(_(e_invalid_வரி_எண்_nr), (long)end_lnum);
	செல் theend;
    }

    எனில் (buf->b_ml.ml_mfp == வெற்று)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	செல் theend;
    }

    எனில் (text != வெற்று)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	க்கு (p = text; *p != NUL; MB_PTR_ADV(p))
	    எனில் (*p < ' ')
		*p = ' ';
    }

    க்கு (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	எனில் (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_வரி));
	    செல் theend;
	}

	எனில் (lnum == start_lnum)
	    col = start_col;
	இல்லையெனில்
	    col = 1;
	எனில் (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != வெற்று))
	{
	    semsg(_(e_invalid_column_எண்_nr), (long)start_col);
	    செல் theend;
	}
	sort_col = col;

	எனில் (lnum == end_lnum)
	    length = end_col - col;
	இல்லையெனில்
	    length = (முழு)textlen - col + 1;
	எனில் (length > (long)textlen)
	    length = (முழு)textlen;	// can include the end-of-line
	எனில் (length < 0)
	    length = 0;		// zero-width property

	எனில் (text_arg != வெற்று)
	{
	    length = (long)STRLEN(text_arg);
	    எனில் (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		எனில் ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	tmp_prop.tp_padleft = text_padding_இடது;
	எனில் (text_arg != வெற்று)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	க்கு (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * அளவு(textprop_T),
							   அளவு(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    எனில் (prop_col >= sort_col)
		நிறுத்து;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	முழு		vtext_total = 0;
	முழு		new_வரி_len;
	char_u	*மொத்தம்_dest;
	char_u	*vtext_dest;
	முழு		j;

	// Compute total vtext size from existing props.
	க்கு (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * அளவு(textprop_T),
							அளவு(textprop_T));
	    எனில் (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	எனில் (text_arg != வெற்று)
	    vtext_total += length + 1;

	new_வரி_len = (முழு)textlen + (முழு)PROP_மொத்தம்_SIZE
			+ new_propcount * (முழு)அளவு(textprop_T)
			+ vtext_total;
	newtext = alloc(new_வரி_len);
	எனில் (newtext == வெற்று)
	    செல் theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_வரி_ptr, textlen);

	// Write prop_count.
	மொத்தம்_dest = newtext + textlen;
	mch_memmove(மொத்தம்_dest, &new_propcount, PROP_மொத்தம்_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = மொத்தம்_dest + PROP_மொத்தம்_SIZE;
	எனில் (i > 0)
	    mch_memmove(newprops, props, அளவு(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	எனில் (i < proplen)
	    mch_memmove(newprops + (i + 1) * அளவு(textprop_T),
					props + i * அளவு(textprop_T),
					அளவு(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * அளவு(textprop_T);
	க்கு (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    எனில் (j == i)
		தொடர்;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * அளவு(textprop_T),
		    அளவு(textprop_T));
	    எனில் (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_மொத்தம்_2 = (char_u *)props - PROP_மொத்தம்_SIZE;
		char_u *old_vtext = old_மொத்தம்_2 + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - மொத்தம்_dest);
		mch_memmove(newprops + j * அளவு(textprop_T), &ep,
			அளவு(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	எனில் (text_arg != வெற்று)
	{
	    முழு copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - மொத்தம்_dest);
	    vtext_dest += copysize;
	}
	இல்லையெனில்
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * அளவு(textprop_T), &tmp_prop,
							அளவு(textprop_T));

	எனில் (buf->b_ml.ml_flags & (ML_வரி_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_வரி_ptr);
	buf->b_ml.ml_வரி_ptr = newtext;
	buf->b_ml.ml_வரி_len = new_வரி_len;
	buf->b_ml.ml_flags |= ML_வரி_DIRTY;
    }

    changed_வரி_display_buf(buf);
    changed_வரிகள்_buf(buf, start_lnum, end_lnum + 1, 0);
    res = OK;

theend:
    vim_free(text);
    திரும்பு res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    வெறுமை
f_prop_add_list(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    முழு		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    முழு		error = FALSE;
    முழு		prev_did_emsg = did_emsg;

    எனில் (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	திரும்பு;

    எனில் (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	திரும்பு;

    dict = argvars[0].vval.v_dict;
    எனில் (dict == வெற்று || !dict_has_சாவி(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	திரும்பு;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    எனில் (dict_has_சாவி(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_எண்(dict, "id");
	எனில் (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    திரும்பு;
	}
	id = (முழு)x;
    }

    எனில் (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	திரும்பு;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_பொருட்கள்(argvars[1].vval.v_list, li)
    {
	எனில் (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == வெற்று)
	{
	    emsg(_(e_list_required));
	    திரும்பு;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	எனில் (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	எனில் (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	எனில் (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	முழு this_id = id;
	எனில் (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	எனில் (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    எனில் (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    திரும்பு;
	}
	எனில் (prop_add_one(buf, type_name, this_id, வெற்று, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    திரும்பு;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
நிலையான முழு vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    நிலையான முழு
get_textprop_id(வெறுமை)
{
    எனில் (vt_id_counter > 0)
	vt_id_counter = 0;
    திரும்பு --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    முழு
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
    முழு		id = 0;
    char_u	*text = வெற்று;
    முழு		text_padding_இடது = 0;
    முழு		flags = 0;

    எனில் (dict == வெற்று || !dict_has_சாவி(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	செல் theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    எனில் (dict_has_சாவி(dict, "end_lnum"))
    {
	end_lnum = dict_get_எண்(dict, "end_lnum");
	எனில் (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str), "end_lnum");
	    செல் theend;
	}
    }
    இல்லையெனில்
	end_lnum = start_lnum;

    எனில் (dict_has_சாவி(dict, "length"))
    {
	long length = dict_get_எண்(dict, "length");

	எனில் (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str), "length");
	    செல் theend;
	}
	end_col = start_col + length;
    }
    இல்லையெனில் எனில் (dict_has_சாவி(dict, "end_col"))
    {
	end_col = dict_get_எண்(dict, "end_col");
	எனில் (end_col <= 0)
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str), "end_col");
	    செல் theend;
	}
    }
    இல்லையெனில் எனில் (start_lnum == end_lnum)
	end_col = start_col;
    இல்லையெனில்
	end_col = 1;

    எனில் (dict_has_சாவி(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_எண்(dict, "id");
	எனில் (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    செல் theend;
	}
	id = (முழு)x;
    }

    எனில் (dict_has_சாவி(dict, "text"))
    {
	எனில் (dict_has_சாவி(dict, "length")
		|| dict_has_சாவி(dict, "end_col")
		|| dict_has_சாவி(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    செல் theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	எனில் (text == வெற்று)
	    செல் theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	எனில் (dict_has_சாவி(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    எனில் (p == வெற்று)
		செல் theend;
	    எனில் (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		செல் theend;
	    }
	    எனில் (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_வலது;
	    இல்லையெனில் எனில் (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    இல்லையெனில் எனில் (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    இல்லையெனில் எனில் (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "text_align", p);
		செல் theend;
	    }
	}

	எனில் (dict_has_சாவி(dict, "text_padding_left"))
	{
	    text_padding_இடது = dict_get_எண்(dict, "text_padding_left");
	    எனில் (text_padding_இடது < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		செல் theend;
	    }
	}

	எனில் (dict_has_சாவி(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    எனில் (p == வெற்று)
		செல் theend;
	    எனில் (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    இல்லையெனில் எனில் (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "text_wrap", p);
		செல் theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    எனில் (start_col < (text == வெற்று ? 1 : 0))
    {
	semsg(_(e_invalid_column_எண்_nr), (long)start_col);
	செல் theend;
    }
    எனில் (start_col > 0 && text_padding_இடது > 0)
    {
	emsg(_(e_can_only_use_இடது_padding_when_column_is_zero));
	செல் theend;
    }

    எனில் (dict_arg != வெற்று && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	செல் theend;

    எனில் (text != வெற்று)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    இல்லையெனில் எனில் (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	செல் theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_இடது, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = வெற்று;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    திரும்பு id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    முழு
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, முழு will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_மொத்தம்;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    எனில் ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == வெற்று)
	திரும்பு 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_வரி_len - textlen;
    எனில் (propdata_len == 0)
	திரும்பு 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    எனில் (propdata_len < PROP_மொத்தம்_SIZE + அளவு(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	திரும்பு 0;
    }
    mch_memmove(&prop_மொத்தம், text + textlen, PROP_மொத்தம்_SIZE);
    எனில் (!text_prop_மொத்தம்_valid(prop_மொத்தம், propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	திரும்பு 0;
    }
    எனில் (!text_prop_vtext_valid(text + textlen + PROP_மொத்தம்_SIZE,
					   (முழு)prop_மொத்தம், propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	திரும்பு 0;
    }
    *props = text + textlen + PROP_மொத்தம்_SIZE;
    திரும்பு (முழு)prop_மொத்தம்;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    முழு
prop_மொத்தம்_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    முழு		மொத்தம் = get_text_props(buf, lnum, &props, FALSE);
    முழு		result = 0;
    textprop_T	prop;
    முழு		i;
    முழு		next_வலது_goes_below = FALSE;

    எனில் (மொத்தம் == 0)
	திரும்பு 0;
    க்கு (i = 0; i < மொத்தம்; ++i)
    {
	mch_memmove(&prop, props + i * அளவு(prop), அளவு(prop));
	எனில் (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    எனில் ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_வலது_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_வலது)))
	    {
		++result;
	    }
	    இல்லையெனில் எனில் (prop.tp_flags & TP_FLAG_ALIGN_வலது)
		next_வலது_goes_below = TRUE;
	}
    }
    திரும்பு result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    முழு
மொத்தம்_props(linenr_T lnum, முழு only_starting, முழு last_வரி)
{
    char_u	*props;
    முழு		proplen = get_text_props(curbuf, lnum, &props, 0);
    முழு		result = proplen;
    textprop_T	prop;

    க்கு (முழு i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * அளவு(prop), அளவு(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	எனில் ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_வரி && prop.tp_col == MAXCOL))
	    --result;
    }
    திரும்பு result;
}

நிலையான textprop_T	*text_prop_compare_props;
நிலையான buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    நிலையான முழு
text_prop_order(முழு flags)
{
    எனில் (flags & TP_FLAG_ALIGN_ABOVE)
	திரும்பு 0;
    எனில் (flags & TP_FLAG_ALIGN_வலது)
	திரும்பு 2;
    எனில் (flags & TP_FLAG_ALIGN_BELOW)
	திரும்பு 3;
    திரும்பு 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    நிலையான முழு
text_prop_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    முழு  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(முழு *)s1;
    idx2 = *(முழு *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    எனில் ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	திரும்பு tp1->tp_id < 0 ? 1 : -1;

    எனில் (col1 == MAXCOL || col2 == MAXCOL)
    {
	முழு order1 = text_prop_order(tp1->tp_flags);
	முழு order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	எனில் (order1 != order2)
	    திரும்பு order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    எனில் (pt1 != pt2)
    {
	எனில் (pt1 == வெற்று)
	    திரும்பு -1;
	எனில் (pt2 == வெற்று)
	    திரும்பு 1;
	எனில் (pt1->pt_priority != pt2->pt_priority)
	    திரும்பு pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    எனில் (col1 != col2)
	திரும்பு col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    எனில் (tp1->tp_id < 0)
	திரும்பு tp1->tp_id > tp2->tp_id ? 1 : -1;

    திரும்பு 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    வெறுமை
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	முழு	    *idxs,
	முழு	    மொத்தம்)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((வெறுமை *)idxs, (size_t)மொத்தம், அளவு(முழு), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_வரிகள்(
	win_T	    *wp,
	முழு	    type_id,
	முழு	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    எனில் (text_prop_type_by_id(wp->w_buffer, type_id) == வெற்று)
	திரும்பு false;

    எனில் (first_lnum < 1)
	first_lnum = 1;
    எனில் (last_lnum > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	last_lnum = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;

    க்கு (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	முழு	மொத்தம் = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	க்கு (முழு i = 0; i < மொத்தம்; ++i)
	{
	    mch_memmove(prop, props + i * அளவு(textprop_T),
							   அளவு(textprop_T));
	    எனில் (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		திரும்பு true;
	    }
	}
    }
    திரும்பு false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	முழு	    type_id,
	முழு	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    திரும்பு find_prop_in_வரிகள்(wp, type_id, id, prop, found_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    நிலையான வெறுமை
set_text_props(linenr_T lnum, textprop_T *tps, முழு மொத்தம்)
{
    char_u  *text;
    char_u  *newtext;
    முழு	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    எனில் (மொத்தம் == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	எனில் (newtext == வெற்று)
	    திரும்பு;
	mch_memmove(newtext, text, textlen);
	எனில் (curbuf->b_ml.ml_flags & (ML_வரி_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_வரி_ptr);
	curbuf->b_ml.ml_வரி_ptr = newtext;
	curbuf->b_ml.ml_வரி_len = textlen;
	curbuf->b_ml.ml_flags |= ML_வரி_DIRTY;
    }
    இல்லையெனில்
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_மொத்தம் = (uint16_t)மொத்தம்;
	முழு	    vtext_size = 0;
	முழு	    total_len;
	char_u	    *மொத்தம்_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	முழு	    i;

	க்கு (i = 0; i < மொத்தம்; ++i)
	    எனில் (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (முழு)PROP_மொத்தம்_SIZE
		    + மொத்தம் * (முழு)அளவு(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	எனில் (newtext == வெற்று)
	    திரும்பு;
	mch_memmove(newtext, text, textlen);

	மொத்தம்_dest = newtext + textlen;
	mch_memmove(மொத்தம்_dest, &prop_மொத்தம், PROP_மொத்தம்_SIZE);

	prop_dest = மொத்தம்_dest + PROP_மொத்தம்_SIZE;
	vtext_dest = prop_dest + மொத்தம் * அளவு(textprop_T);

	க்கு (i = 0; i < மொத்தம்; ++i)
	{
	    textprop_T prop = tps[i];

	    எனில் (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		முழு copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = வெற்று;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - மொத்தம்_dest);
		vtext_dest += copysize;
	    }
	    இல்லையெனில்
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, அளவு(textprop_T));
	    prop_dest += அளவு(textprop_T);
	}

	எனில் (curbuf->b_ml.ml_flags & (ML_வரி_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_வரி_ptr);
	curbuf->b_ml.ml_வரி_ptr = newtext;
	curbuf->b_ml.ml_வரி_len = total_len;
	curbuf->b_ml.ml_flags |= ML_வரி_DIRTY;
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
props_add_மொத்தம்_header(char_u *வரி, முழு வரி_len, முழு textlen, முழு *new_len)
{
    முழு		prop_bytes = வரி_len - textlen;
    uint16_t	prop_மொத்தம்;
    முழு		vtext_size = 0;
    முழு		total;
    char_u	*newline;
    char_u	*மொத்தம்_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    முழு		i;

    எனில் (prop_bytes <= 0 || prop_bytes % அளவு(textprop_T) != 0)
    {
	*new_len = வரி_len;
	திரும்பு வெற்று;
    }
    prop_மொத்தம் = (uint16_t)(prop_bytes / அளவு(textprop_T));

    // Calculate total vtext size.
    க்கு (i = 0; i < (முழு)prop_மொத்தம்; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, வரி + textlen + i * அளவு(textprop_T),
							அளவு(textprop_T));
	எனில் (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (முழு)PROP_மொத்தம்_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    எனில் (newline == வெற்று)
	திரும்பு வெற்று;
    *new_len = total;

    mch_memmove(newline, வரி, textlen);
    மொத்தம்_dest = newline + textlen;
    mch_memmove(மொத்தம்_dest, &prop_மொத்தம், PROP_மொத்தம்_SIZE);
    prop_dest = மொத்தம்_dest + PROP_மொத்தம்_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    க்கு (i = 0; i < (முழு)prop_மொத்தம்; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, வரி + textlen + i * அளவு(textprop_T),
							அளவு(textprop_T));
	எனில் (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    முழு copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - மொத்தம்_dest);
	    vtext_dest += copysize;
	}
	இல்லையெனில்
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * அளவு(textprop_T), &prop,
							அளவு(textprop_T));
    }
    திரும்பு newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    வெறுமை
add_text_props(linenr_T lnum, textprop_T *text_props, முழு text_prop_மொத்தம்)
{
    unpacked_memline_T	um;
    முழு			i;

    um = um_open_at(curbuf, lnum, text_prop_மொத்தம்);
    எனில் (um.buf == வெற்று)
	திரும்பு;
    எனில் (!um_detach(&um))
    {
	um_abort(&um);
	திரும்பு;
    }

    // Grow props array if needed.
    எனில் (um.prop_மொத்தம் + text_prop_மொத்தம் > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_மொத்தம் + text_prop_மொத்தம்) * அளவு(textprop_T));
	எனில் (newprops == வெற்று)
	{
	    um_abort(&um);
	    திரும்பு;
	}
	um.props = newprops;
	um.prop_size = um.prop_மொத்தம் + text_prop_மொத்தம்;
    }

    க்கு (i = 0; i < text_prop_மொத்தம்; ++i)
	um.props[um.prop_மொத்தம்++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    நிலையான முழு
compare_pt(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    திரும்பு tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    நிலையான proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, முழு id)
{
    முழு கீழ் = 0;
    முழு உயர்வு;

    எனில் (ht == வெற்று || ht->ht_used == 0)
	திரும்பு வெற்று;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    எனில் (*array == வெற்று)
    {
	long	    todo;
	hashitem_T  *hi;
	முழு	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	எனில் (*array == வெற்று)
	    திரும்பு வெற்று;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
	{
	    எனில் (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((வெறுமை *)*array, ht->ht_used, அளவு(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    உயர்வு = ht->ht_used;
    போது (உயர்வு > கீழ்)
    {
	முழு m = (உயர்வு + கீழ்) / 2;

	எனில் ((*array)[m]->pt_id == id)
	    திரும்பு (*array)[m];
	எனில் ((*array)[m]->pt_id > id)
	    உயர்வு = m;
	இல்லையெனில்
	    கீழ் = m + 1;
    }
    திரும்பு வெற்று;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    நிலையான வெறுமை
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    முழு buflocal = TRUE;
    முழு virtualtext_prop = prop->tp_id < 0;

    dict_add_எண்(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    எனில் (!virtualtext_prop)
    {
	dict_add_எண்(dict, "length", prop->tp_len);
	dict_add_எண்(dict, "id", prop->tp_id);
    }
    dict_add_எண்(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_எண்(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    எனில் (pt == வெற்று)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    எனில் (pt != வெற்று)
	dict_add_string(dict, "type", pt->pt_name);

    எனில் (buflocal)
	dict_add_எண்(dict, "type_bufnr", buf->b_fnum);
    இல்லையெனில்
	dict_add_எண்(dict, "type_bufnr", 0);
    எனில் (virtualtext_prop)
    {
	string_T    text_align = {வெற்று, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	எனில் (prop->tp_flags & TP_FLAG_ALIGN_வலது)
	    STR_LITERAL_SET(text_align, "right");
	இல்லையெனில் எனில் (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	இல்லையெனில் எனில் (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	எனில் (text_align.string != வெற்று)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (முழு)text_align.length);

	// text_wrap
	எனில் (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	எனில் (prop->tp_padleft != 0)
	    dict_add_எண்(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, முழு id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    எனில் (type == வெற்று)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    திரும்பு type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    திரும்பு text_prop_type_by_id(buf, prop->tp_type) != வெற்று;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    வெறுமை
f_prop_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    linenr_T start;
    linenr_T end;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    முழு	    did_clear = FALSE;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	திரும்பு;

    start = tv_get_எண்(&argvars[0]);
    end = start;
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_எண்(&argvars[1]);
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    எனில் (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		திரும்பு;
	}
    }
    எனில் (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	திரும்பு;
    }

    {
	unpacked_memline_T um = um_open(buf);

	க்கு (lnum = start; lnum <= end; ++lnum)
	{
	    முழு idx;

	    எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம்)
		நிறுத்து;
	    எனில் (!um_goto_வரி(&um, lnum, 0))
		நிறுத்து;
	    எனில் (um.prop_மொத்தம் > 0)
	    {
		did_clear = TRUE;
		க்கு (idx = um.prop_மொத்தம் - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    எனில் (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    வெறுமை
f_prop_find(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    முழு		lnum_start;
    முழு		start_pos_has_prop = 0;
    முழு		seen_end = FALSE;
    முழு		id = 0;
    முழு		id_found = FALSE;
    முழு		type_id = -1;
    முழு		skipstart = FALSE;
    முழு		lnum = -1;
    முழு		col = -1;
    முழு		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    முழு		both;

    எனில் (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	திரும்பு;
    dict = argvars[0].vval.v_dict;

    எனில் (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	திரும்பு;
    எனில் (buf->b_ml.ml_mfp == வெற்று)
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	எனில் (*dir_s == 'b')
	    dir = BACKWARD;
	இல்லையெனில் எனில் (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    திரும்பு;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    எனில் (di != வெற்று)
	lnum = tv_get_எண்(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    எனில் (di != வெற்று)
	col = tv_get_எண்(&di->di_tv);

    எனில் (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    இல்லையெனில் எனில் (col == -1)
	col = 1;

    எனில் (lnum < 1 || lnum > buf->b_ml.ml_வரி_மொத்தம்)
    {
	emsg(_(e_invalid_range));
	திரும்பு;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    எனில் (dict_has_சாவி(dict, "id"))
    {
	id = dict_get_எண்(dict, "id");
	id_found = TRUE;
    }
    எனில் (dict_has_சாவி(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	எனில் (type == வெற்று)
	    திரும்பு;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    எனில் (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	திரும்பு;
    }
    எனில் (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	திரும்பு;
    }

    lnum_start = lnum;

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;

    போது (1)
    {
	char_u	*prop_start_ptr;
	முழு	மொத்தம் = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	முழு	    i;
	textprop_T  prop;
	முழு	    prop_start;
	முழு	    prop_end;

	க்கு (i = dir == BACKWARD ? மொத்தம் - 1 : 0; i >= 0 && i < மொத்தம்; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * அளவு(textprop_T),
							   அளவு(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    எனில் (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_மொத்தம்_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    இல்லையெனில்
		prop.u.tp_text = வெற்று;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    எனில் (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		எனில் (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    எனில் (!is_floating_vtext && prop.tp_col > col)
			தொடர்;
		}
		இல்லையெனில் எனில் (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    தொடர்;
	    }
	    எனில் (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		எனில் (lnum_start == lnum
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
		எனில் (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		எனில் (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    எனில் (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    தொடர்;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		எனில் (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    தொடர்;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_எண்(rettv->vval.v_dict, "lnum", lnum);

		திரும்பு;
	    }
	}

	எனில் (dir > 0)
	{
	    எனில் (lnum >= buf->b_ml.ml_வரி_மொத்தம்)
		நிறுத்து;
	    lnum++;
	}
	இல்லையெனில்
	{
	    எனில் (lnum <= 1)
		நிறுத்து;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    நிலையான முழு
prop_type_or_id_in_list(முழு *types_or_ids, முழு len, முழு type_or_id)
{
    முழு i;

    க்கு (i = 0; i < len; i++)
	எனில் (types_or_ids[i] == type_or_id)
	    திரும்பு TRUE;

    திரும்பு FALSE;
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
    நிலையான வெறுமை
get_props_in_வரி(
	buf_T		*buf,
	linenr_T	lnum,
	முழு		*prop_types,
	முழு		prop_types_len,
	முழு		*prop_ids,
	முழு		prop_ids_len,
	list_T		*retlist,
	முழு		add_lnum)
{
    char_u	*props;
    முழு		மொத்தம்;
    முழு		i;
    textprop_T	prop;

    மொத்தம் = get_text_props(buf, lnum, &props, FALSE);
    க்கு (i = 0; i < மொத்தம்; ++i)
    {
	mch_memmove(&prop, props + i * அளவு(textprop_T),
		அளவு(textprop_T));
	// Convert offset to pointer for virtual text props.
	எனில் (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_மொத்தம்_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	இல்லையெனில்
	    prop.u.tp_text = வெற்று;
	எனில் ((prop_types == வெற்று
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == வெற்று
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    எனில் (d == வெற்று)
		நிறுத்து;
	    prop_fill_dict(d, &prop, buf);
	    எனில் (add_lnum)
		dict_add_எண்(d, "lnum", lnum);
	    எனில் (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    நிலையான முழு *
get_prop_types_from_names(list_T *l, buf_T *buf, முழு *இலக்கம்_types)
{
    முழு		*prop_types;
    listitem_T	*li;
    முழு		i;
    char_u	*name;
    proptype_T	*type;

    *இலக்கம்_types = 0;

    prop_types = ALLOC_MULT(முழு, list_len(l));
    எனில் (prop_types == வெற்று)
	திரும்பு வெற்று;

    i = 0;
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	எனில் (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    செல் errret;
	}
	name = li->li_tv.vval.v_string;
	எனில் (name == வெற்று)
	    செல் errret;

	type = lookup_prop_type(name, buf);
	எனில் (type == வெற்று)
	    செல் errret;
	prop_types[i++] = type->pt_id;
    }

    *இலக்கம்_types = i;
    திரும்பு prop_types;

errret:
    VIM_CLEAR(prop_types);
    திரும்பு வெற்று;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    நிலையான முழு *
get_prop_ids_from_list(list_T *l, முழு *இலக்கம்_ids)
{
    முழு		*prop_ids;
    listitem_T	*li;
    முழு		i = 0;
    முழு		id;
    முழு		error;

    *இலக்கம்_ids = 0;

    prop_ids = ALLOC_MULT(முழு, list_len(l));
    எனில் (prop_ids == வெற்று)
	திரும்பு வெற்று;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	error = FALSE;
	id = tv_get_எண்_chk(&li->li_tv, &error);
	எனில் (error)
	    செல் errret;

	prop_ids[i++] = id;
    }

    *இலக்கம்_ids = i;
    திரும்பு prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    திரும்பு வெற்று;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    வெறுமை
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    buf_T	*buf = curbuf;
    முழு		add_lnum = FALSE;
    முழு		*prop_types = வெற்று;
    முழு		prop_types_len = 0;
    முழு		*prop_ids = வெற்று;
    முழு		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    // default: get text properties on current line
    start_lnum = tv_get_எண்(&argvars[0]);
    end_lnum = start_lnum;
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	எனில் (check_for_dict_arg(argvars, 1) == FAIL)
	    திரும்பு;
	d = argvars[1].vval.v_dict;

	எனில் (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    திரும்பு;

	எனில் (d != வெற்று && (di = dict_find(d, (char_u *)"end_lnum", -1)) != வெற்று)
	{
	    எனில் (di->di_tv.v_type != VAR_எண்)
	    {
		emsg(_(e_எண்_required));
		திரும்பு;
	    }
	    end_lnum = tv_get_எண்(&di->di_tv);
	    எனில் (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//வரி
		end_lnum = buf->b_ml.ml_வரி_மொத்தம் + end_lnum + 1;
	    இல்லையெனில் எனில் (end_lnum > buf->b_ml.ml_வரி_மொத்தம்)
		end_lnum = buf->b_ml.ml_வரி_மொத்தம்;
	    add_lnum = TRUE;
	}
	எனில் (d != வெற்று && (di = dict_find(d, (char_u *)"types", -1)) != வெற்று)
	{
	    எனில் (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		திரும்பு;
	    }

	    l = di->di_tv.vval.v_list;
	    எனில் (l != வெற்று && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		எனில் (prop_types == வெற்று)
		    திரும்பு;
	    }
	}
	எனில் (d != வெற்று && (di = dict_find(d, (char_u *)"ids", -1)) != வெற்று)
	{
	    எனில் (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		செல் errret;
	    }

	    l = di->di_tv.vval.v_list;
	    எனில் (l != வெற்று && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		எனில் (prop_ids == வெற்று)
		    செல் errret;
	    }
	}
    }
    எனில் (start_lnum < 1 || start_lnum > buf->b_ml.ml_வரி_மொத்தம்
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    இல்லையெனில்
	க்கு (lnum = start_lnum; lnum <= end_lnum; lnum++)
	    get_props_in_வரி(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, add_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    வெறுமை
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start = 1;
    linenr_T	end = 0;
    linenr_T	lnum;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    முழு		do_all;
    முழு		id = -MAXCOL;
    முழு		type_id = -1;	    // for a single "type"
    முழு		*type_ids = வெற்று;   // array, for a list of "types", allocated
    முழு		இலக்கம்_type_ids = 0;   // number of elements in "type_ids"
    முழு		both;

    rettv->vval.v_எண் = 0;

    எனில் (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 2) == FAIL)))
	திரும்பு;

    எனில் (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_எண்(&argvars[1]);
	end = start;
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_எண்(&argvars[2]);
	எனில் (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    திரும்பு;
	}
    }

    dict = argvars[0].vval.v_dict;
    எனில் (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	திரும்பு;
    எனில் (buf->b_ml.ml_mfp == வெற்று)
	திரும்பு;

    do_all = dict_get_bool(dict, "all", FALSE);

    எனில் (dict_has_சாவி(dict, "id"))
	id = dict_get_எண்(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    எனில் (dict_has_சாவி(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	எனில் (type == வெற்று)
	    திரும்பு;
	type_id = type->pt_id;
    }
    எனில் (dict_has_சாவி(dict, "types"))
    {
	typval_T types;
	listitem_T *li = வெற்று;

	dict_get_tv(dict, "types", &types);
	எனில் (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( அளவு(முழு) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_பொருட்கள்(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		எனில் (li->li_tv.v_type != VAR_STRING)
		    தொடர்;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		எனில் (!prop_type)
		    செல் cleanup_prop_remove;

		type_ids[இலக்கம்_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    எனில் (id == -MAXCOL && (type_id == -1 && இலக்கம்_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	செல் cleanup_prop_remove;
    }
    எனில் (both && (id == -MAXCOL || (type_id == -1 && இலக்கம்_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	செல் cleanup_prop_remove;
    }
    எனில் (type_id != -1 && இலக்கம்_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	செல் cleanup_prop_remove;
    }

    எனில் (end == 0)
	end = buf->b_ml.ml_வரி_மொத்தம்;

    {
	unpacked_memline_T um = um_open(buf);

	க்கு (lnum = start; lnum <= end; ++lnum)
	{
	    முழு idx;

	    எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம்)
		நிறுத்து;
	    எனில் (!um_goto_வரி(&um, lnum, 0))
		நிறுத்து;

	    க்கு (idx = 0; idx < um.prop_மொத்தம்; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		முழு	    matches_id = 0;
		முழு	    matches_type = 0;

		எனில் (prop->tp_flags & TP_FLAG_DELETED)
		    தொடர்;

		matches_id = prop->tp_id == id;
		எனில் (இலக்கம்_type_ids > 0)
		{
		    முழு idx2;

		    க்கு (idx2 = 0; !matches_type && idx2 < இலக்கம்_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		இல்லையெனில்
		    matches_type = prop->tp_type == type_id;

		எனில் (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    எனில் (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_எண்;
		    எனில் (!do_all)
			நிறுத்து;
		}
	    }
	}
	um_close(&um);
    }

    எனில் (first_changed > 0)
    {
	changed_வரி_display_buf(buf);
	changed_வரிகள்_buf(buf, first_changed, last_changed + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    நிலையான வெறுமை
prop_type_set(typval_T *argvars, முழு add)
{
    char_u	*name;
    buf_T	*buf = வெற்று;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    name = tv_get_string(&argvars[0]);
    எனில் (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	திரும்பு;
    }

    எனில் (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	திரும்பு;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    எனில் (add)
    {
	hashtab_T **htp;

	எனில் (prop != வெற்று)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    திரும்பு;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	எனில் (prop == வெற்று)
	    திரும்பு;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	எனில் (buf == வெற்று)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	இல்லையெனில்
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	எனில் (*htp == வெற்று)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    எனில் (*htp == வெற்று)
	    {
		vim_free(prop);
		திரும்பு;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    இல்லையெனில்
    {
	எனில் (prop == வெற்று)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    திரும்பு;
	}
    }

    எனில் (dict != வெற்று)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	எனில் (di != வெற்று)
	{
	    char_u	*highlight;
	    முழு		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    எனில் (highlight != வெற்று && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    எனில் (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == வெற்று ? (char_u *)"" : highlight);
		திரும்பு;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	எனில் (di != வெற்று)
	{
	    எனில் (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    இல்லையெனில்
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	எனில் (di != வெற்று)
	{
	    எனில் (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    இல்லையெனில்
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	எனில் (di != வெற்று)
	    prop->pt_priority = tv_get_எண்(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	எனில் (di != வெற்று)
	{
	    எனில் (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    இல்லையெனில்
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	எனில் (di != வெற்று)
	{
	    எனில் (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    இல்லையெனில்
		prop->pt_flags &= ~PT_FLAG_INS_END_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    வெறுமை
f_prop_type_add(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    வெறுமை
f_prop_type_change(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    வெறுமை
f_prop_type_delete(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*name;
    buf_T	*buf = வெற்று;
    hashitem_T	*hi;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    name = tv_get_string(&argvars[0]);
    எனில் (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	திரும்பு;
    }

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	எனில் (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    திரும்பு;
    }

    hi = find_prop_type_hi(name, buf);
    எனில் (hi == வெற்று)
	திரும்பு;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    எனில் (buf == வெற்று)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    இல்லையெனில்
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    changed_window_setting_buf(buf == வெற்று ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    வெறுமை
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    name = tv_get_string(&argvars[0]);
    எனில் (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	திரும்பு;
    }

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;

    proptype_T  *prop = வெற்று;
    buf_T	    *buf = வெற்று;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	எனில் (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    திரும்பு;
    }

    prop = find_prop_type(name, buf);
    எனில் (prop == வெற்று)
	திரும்பு;

    dict_T *d = rettv->vval.v_dict;

    எனில் (prop->pt_hl_id > 0)
	dict_add_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_add_எண்(d, "priority", prop->pt_priority);
    dict_add_எண்(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_add_எண்(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_add_எண்(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_START_INCL) ? 1 : 0);
    dict_add_எண்(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_END_INCL) ? 1 : 0);
    எனில் (buf != வெற்று)
	dict_add_எண்(d, "bufnr", buf->b_fnum);
}

    நிலையான வெறுமை
list_types(hashtab_T *ht, list_T *l)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
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
    வெறுமை
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = வெற்று;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	எனில் (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    திரும்பு;
    }
    எனில் (buf == வெற்று)
    {
	எனில் (global_proptypes != வெற்று)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    இல்லையெனில் எனில் (buf->b_proptypes != வெற்று)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    நிலையான வெறுமை
clear_ht_prop_types(hashtab_T *ht)
{
    long	todo;
    hashitem_T	*hi;

    எனில் (ht == வெற்று)
	திரும்பு;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#எனில் defined(EXITFREE)
/*
 * Free all global property types.
 */
    வெறுமை
clear_global_prop_types(வெறுமை)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = வெற்று;
    VIM_CLEAR(global_proparray);
}
#endif

/*
 * Free all property types for "buf".
 */
    வெறுமை
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = வெற்று;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
வகைவரை கட்டமைப்பு
{
    முழு dirty;	    // if the property was changed
    முழு can_drop;   // whether after this change, the prop may be removed
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
    நிலையான adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    col,
	முழு	    added,
	முழு	    flags)
{
    proptype_T	*pt;
    முழு		start_incl;
    முழு		end_incl;
    முழு		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    எனில் (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	திரும்பு res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != வெற்று && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    எனில் (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != வெற்று && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    எனில் (added > 0)
    {
	எனில் (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	இல்லையெனில் எனில் (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    இல்லையெனில் எனில் (prop->tp_col > col + 1)
    {
	எனில் (prop->tp_col + added < col + 1)
	{
	    எனில் (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    இல்லையெனில்
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		எனில் (prop->tp_len <= 0)
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
	இல்லையெனில்
	    prop->tp_col += added;
    }
    இல்லையெனில் எனில் (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	எனில் (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	இல்லையெனில்
	{
	    முழு after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	எனில் (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    இல்லையெனில்
	res.dirty = FALSE;

    திரும்பு res;
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
    முழு
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	முழு	    bytes_added,
	முழு	    flags)
{
    unpacked_memline_T um;
    முழு		dirty = FALSE;
    முழு		ri;

    எனில் (text_prop_frozen > 0)
	திரும்பு FALSE;

    um = um_open_at(curbuf, lnum, 0);
    எனில் (um.buf == வெற்று || um.prop_மொத்தம் == 0)
    {
	um_abort(&um);
	திரும்பு FALSE;
    }

    க்கு (ri = 0; ri < um.prop_மொத்தம்; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	எனில் (res.dirty)
	{
	    எனில் (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		எனில் (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    திரும்பு FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		எனில் ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    திரும்பு FALSE;
		}
	    }
	    dirty = TRUE;
	}
	எனில் (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    தொடர்;
	}
    }
    எனில் (dirty)
	um_close(&um);
    இல்லையெனில்
	um_abort(&um);
    திரும்பு dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    வெறுமை
adjust_props_for_split(
	linenr_T    lnum_props,
	linenr_T    lnum_உச்சி,
	முழு	    kept,
	முழு	    deleted,
	முழு	    at_eol)
{
    char_u	*props;
    முழு		மொத்தம்;
    garray_T    prevprop;
    garray_T    nextprop;
    முழு		skipped = kept + deleted;

    எனில் (!curbuf->b_has_textprop)
	திரும்பு;

    // Get the text properties from "lnum_props".
    மொத்தம் = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, அளவு(textprop_T), 10);
    ga_init2(&nextprop, அளவு(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *மொத்தம்_ptr = props - PROP_மொத்தம்_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    க்கு (முழு i = 0; i < மொத்தம்; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	முழு	    start_incl, end_incl;
	முழு	    cont_prev, cont_next;
	முழு	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * அளவு(textprop_T), அளவு(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	எனில் (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(மொத்தம்_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	இல்லையெனில்
	    prop.u.tp_text = வெற்று;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != வெற்று && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != வெற்று && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	எனில் (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	இல்லையெனில்
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    முழு is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    எனில் (is_floating_vtext)
		cont_next = FALSE;
	    இல்லையெனில்
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	எனில் (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	எனில் (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    முழு is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    எனில் (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    எனில் (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	எனில் (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    முழு is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    எனில் (p->tp_col != MAXCOL)
	    {
		எனில் (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		இல்லையெனில்
		{
		    எனில் (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    எனில் (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    set_text_props(lnum_உச்சி, (textprop_T *)prevprop.ga_data,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    set_text_props(lnum_உச்சி + 1, (textprop_T *)nextprop.ga_data,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    வெறுமை
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	முழு		    last_வரி,
	long		    col,
	முழு		    removed)
{
    unpacked_memline_T	r_um;

    எனில் (um->buf == வெற்று)
	திரும்பு;

    r_um = um_open_at(um->buf, lnum, 0);
    எனில் (r_um.buf == வெற்று)
	திரும்பு;
    எனில் (!um_detach(&r_um))
    {
	um_abort(&r_um);
	திரும்பு;
    }

    க்கு (முழு i = r_um.prop_மொத்தம் - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	முழு	    end;

	எனில் (prop->tp_col == MAXCOL && !last_வரி)
	    தொடர்;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	எனில் (last_வரி || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = வெற்று;  // ownership transferred
	}
	இல்லையெனில்
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    க்கு (முழு j = 0; j < um->prop_மொத்தம்; ++j)
	    {
		textprop_T *op = &um->props[j];

		எனில் ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    நிறுத்து;
		}
	    }
	    எனில் (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    நிலையான வெறுமை
remember_cleared_prop(garray_T *gap, முழு which, textprop_T *prop, முழு flag)
{
    undoprop_T	*up;

    எனில் (which == UNDOPROP_NONE)
	திரும்பு;
    எனில் (gap->ga_itemsize == 0)
	ga_init2(gap, அளவு(undoprop_T), 4);
    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு;
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
    வெறுமை
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	முழு	    above_which,
	முழு	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = வெற்று;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    எனில் (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_வரி_மொத்தம்)
	திரும்பு;

    above = um_open_at(buf, lnum, 0);
    எனில் (above.buf == வெற்று)
	திரும்பு;
    க்கு (முழு i = 0; i < above.prop_மொத்தம்; ++i)
	எனில் (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    எனில் (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	திரும்பு;
    }

    below = um_open_at(buf, lnum + 1, 0);
    எனில் (below.buf == வெற்று)
    {
	um_abort(&above);
	திரும்பு;
    }
    எனில் (below.prop_மொத்தம் > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_மொத்தம்);
	எனில் (paired == வெற்று)
	    செல் theend;
    }

    க்கு (முழு i = 0; i < above.prop_மொத்தம்; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	எனில் (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    தொடர்;
	க்கு (முழு j = 0; j < below.prop_மொத்தம்; ++j)
	{
	    textprop_T *next = &below.props[j];

	    எனில் (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		நிறுத்து;
	    }
	}
	எனில் (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    க்கு (முழு j = 0; j < below.prop_மொத்தம்; ++j)
    {
	எனில் (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    தொடர்;
	எனில் (!below.detached && !um_detach(&below))
	    நிறுத்து;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    எனில் (below_changed)
	um_close(&below);
    இல்லையெனில்
	um_abort(&below);
    எனில் (above_changed)
	um_close(&above);
    இல்லையெனில்
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    வெறுமை
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	முழு	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    எனில் (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_வரி_மொத்தம்)
	திரும்பு;
    um = um_open_at(buf, lnum, 0);
    எனில் (um.buf == வெற்று)
	திரும்பு;
    க்கு (முழு r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	எனில் (up->up_which != which)
	    தொடர்;
	க்கு (முழு i = 0; i < um.prop_மொத்தம்; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    எனில் (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		தொடர்;
	    எனில் (!um.detached && !um_detach(&um))
		செல் theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    நிறுத்து;
	}
    }

theend:
    எனில் (changed)
	um_close(&um);
    இல்லையெனில்
	um_abort(&um);
}

    bool
text_prop_மொத்தம்_valid(முழு prop_மொத்தம், size_t propdata_len)
{
    எனில் (propdata_len < PROP_மொத்தம்_SIZE)
	திரும்பு false;
    திரும்பு (size_t)prop_மொத்தம் * அளவு(textprop_T)
		    <= propdata_len - PROP_மொத்தம்_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, முழு prop_மொத்தம், size_t propdata_len)
{
    க்கு (முழு i = 0; i < prop_மொத்தம்; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * அளவு(textprop_T),
							  அளவு(textprop_T));
	எனில் (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    தொடர்;
	எனில் (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    திரும்பு false;
    }
    திரும்பு true;
}

#endif // FEAT_PROP_POPUP
