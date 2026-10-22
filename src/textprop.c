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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_PROP_POPUP)

स्थिर शून्य um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    स्थिर शून्य
um_free_vtext(textprop_T *props, पूर्णाङ्क मोजणी)
{
    प्रति (पूर्णाङ्क i = 0; i < मोजणी; ++i)
	यदि (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    स्थिर शून्य
um_free(unpacked_memline_T *um)
{
    यदि (um->detached)
    {
	VIM_CLEAR(um->text);
	um_free_vtext(um->props, um->prop_मोजणी);
    }
    VIM_CLEAR(um->props);
    um->prop_size = 0;
    um->prop_मोजणी = 0;
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
    प्रत्यागम um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_ओळ(unpacked_memline_T *um, linenr_T lnum, पूर्णाङ्क extra_props)
{
    char_u	*ओळ;
    size_t	textlen;
    size_t	propdata_len;
    पूर्णाङ्क		proplen;

    यदि (um->buf == रिक्त)
	प्रत्यागम false;

    // Store changes to the current line if any.
    यदि (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    यदि (lnum == 0)
	प्रत्यागम true;	// just unload

    ओळ = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_ओळ_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = ओळ;

    यदि (propdata_len == 0)
	प्रत्यागम true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    यदि (propdata_len < PROP_मोजणी_SIZE + परिमाण(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = रिक्त;
	प्रत्यागम false;
    }

    uint16_t    prop_मोजणी;
    char_u	    *मोजणी_ptr = ओळ + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_मोजणी, मोजणी_ptr, PROP_मोजणी_SIZE);
    यदि (!text_prop_मोजणी_valid(prop_मोजणी, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = रिक्त;
	प्रत्यागम false;
    }
    proplen = (पूर्णाङ्क)prop_मोजणी;
    props_start = मोजणी_ptr + PROP_मोजणी_SIZE;

    यदि (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = रिक्त;
	प्रत्यागम false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    यदि (um->props == रिक्त)
    {
	um->buf = रिक्त;
	प्रत्यागम false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_मोजणी = proplen;
    mch_memmove(um->props, props_start, proplen * परिमाण(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    प्रति (पूर्णाङ्क i = 0; i < proplen; ++i)
    {
	यदि (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = मोजणी_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	अन्यथा
	    um->props[i].u.tp_text = रिक्त;
    }

    प्रत्यागम true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, पूर्णाङ्क extra_props)
{
    unpacked_memline_T um = um_open(buf);

    यदि (!um_goto_ओळ(&um, lnum, extra_props))
	um.buf = रिक्त;
    प्रत्यागम um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    स्थिर bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    यदि (um->detached || um->buf == रिक्त)
	प्रत्यागम um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    यदि (newtext == रिक्त)
    {
	um->buf = रिक्त;
	प्रत्यागम false;
    }
    um->text = newtext;

    प्रति (पूर्णाङ्क i = 0; i < um->prop_मोजणी; ++i)
    {
	यदि (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    यदि (copy == रिक्त)
	    {
		प्रति (पूर्णाङ्क j = 0; j < i; ++j)
		    यदि (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = रिक्त;
		प्रत्यागम false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    प्रत्यागम true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    यदि (um->buf == रिक्त)
	प्रत्यागम false;
    यदि (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    प्रत्यागम true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    स्थिर bool
um_grow_props(unpacked_memline_T *um, पूर्णाङ्क needed)
{
    यदि (um->prop_मोजणी + needed <= um->prop_size)
	प्रत्यागम true;

    पूर्णाङ्क	    new_size = um->prop_मोजणी + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * परिमाण(textprop_T));
    यदि (newprops == रिक्त)
	प्रत्यागम false;
    um->props = newprops;
    um->prop_size = new_size;

    प्रत्यागम true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    स्थिर शून्य
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    यदि (um->buf == रिक्त)
	प्रत्यागम;
    यदि (!um->detached && !um_detach(um))
	प्रत्यागम;
    यदि (!um_grow_props(um, 1))
    {
	um->buf = रिक्त;
	प्रत्यागम;
    }
    um->props[um->prop_मोजणी++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    शून्य
um_reverse_props(unpacked_memline_T *um)
{
    प्रति (पूर्णाङ्क i = 0, j = um->prop_मोजणी - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, पूर्णाङ्क prop_मोजणी)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    यदि (lnum < 1 || lnum > buf->b_ml.ml_ओळ_मोजणी)
    {
	um.buf = रिक्त;
	प्रत्यागम um;
    }

    char_u *ओळ = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(ओळ);

    यदि (text_copy == रिक्त)
    {
	um.buf = रिक्त;
	प्रत्यागम um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    यदि (prop_मोजणी > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_मोजणी);
	यदि (um.props == रिक्त)
	{
	    vim_free(um.text);
	    um.buf = रिक्त;
	    प्रत्यागम um;
	}
	um.prop_size = prop_मोजणी;
    }

    प्रत्यागम um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    स्थिर शून्य
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					पूर्णाङ्क tp_id, पूर्णाङ्क tp_type, पूर्णाङ्क flag)
{
    unpacked_memline_T	neighbor;

    यदि (lnum < 1 || lnum > buf->b_ml.ml_ओळ_मोजणी)
	प्रत्यागम;

    neighbor = um_open_at(buf, lnum, 0);
    यदि (neighbor.buf == रिक्त)
	प्रत्यागम;

    प्रति (पूर्णाङ्क i = 0; i < neighbor.prop_मोजणी; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	यदि (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    यदि (!neighbor.detached && !um_detach(&neighbor))
		भङ्ग;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    भङ्ग;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    शून्य
um_delete_prop(unpacked_memline_T *um, पूर्णाङ्क index)
{
    textprop_T *prop;

    यदि (um->buf == रिक्त || index < 0 || index >= um->prop_मोजणी)
	प्रत्यागम;
    यदि (!um->detached && !um_detach(um))
	प्रत्यागम;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    यदि (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    यदि (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    यदि (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
    स्थिर char_u *
um_pack(unpacked_memline_T *um, पूर्णाङ्क *packed_len)
{
    uint16_t	live_मोजणी = 0;
    पूर्णाङ्क		vtext_size = 0;
    पूर्णाङ्क		total_len;
    char_u	*buf;
    char_u	*मोजणी_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    प्रति (पूर्णाङ्क i = 0; i < um->prop_मोजणी; ++i)
    {
	textprop_T *prop = &um->props[i];

	यदि (prop->tp_flags & TP_FLAG_DELETED)
	    अनुवर्त;
	++live_मोजणी;
	यदि (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    यदि (live_मोजणी == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	यदि (buf == रिक्त)
	    प्रत्यागम रिक्त;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	प्रत्यागम buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (पूर्णाङ्क)PROP_मोजणी_SIZE
		+ live_मोजणी * (पूर्णाङ्क)परिमाण(textprop_T) + vtext_size;
    buf = alloc(total_len);
    यदि (buf == रिक्त)
	प्रत्यागम रिक्त;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    मोजणी_dest = buf + um->text_size;
    mch_memmove(मोजणी_dest, &live_मोजणी, PROP_मोजणी_SIZE);

    prop_dest = मोजणी_dest + PROP_मोजणी_SIZE;
    vtext_dest = prop_dest + live_मोजणी * परिमाण(textprop_T);

    प्रति (पूर्णाङ्क i = 0; i < um->prop_मोजणी; ++i)
    {
	textprop_T prop = um->props[i];

	यदि (prop.tp_flags & TP_FLAG_DELETED)
	    अनुवर्त;

	यदि (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    पूर्णाङ्क copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - मोजणी_dest);
	    vtext_dest += copysize;
	}
	अन्यथा
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, परिमाण(textprop_T));
	prop_dest += परिमाण(textprop_T);
    }

    प्रत्यागम buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    स्थिर शून्य
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    पूर्णाङ्क	    packed_len;

    यदि (!um->detached || um->buf == रिक्त || um->lnum == 0)
	प्रत्यागम;

    packed = um_pack(um, &packed_len);
    यदि (packed == रिक्त)
    {
	um->buf = रिक्त;
	प्रत्यागम;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    यदि (um->buf->b_ml.ml_ओळ_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    अन्यथा यदि (!(um->buf->b_ml.ml_flags & ML_ओळ_DIRTY))
	(शून्य)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_मोजणी);

    यदि (um->buf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_ओळ_ptr);
    um->buf->b_ml.ml_ओळ_ptr = packed;
    um->buf->b_ml.ml_ओळ_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_ओळ_DIRTY;

    um->detached = false;
    um->text = packed;
    um->text_size = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_ओळ_textlen = um->text_size;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    शून्य
um_close(unpacked_memline_T *um)
{
    यदि (um->buf == रिक्त)
	प्रत्यागम;
    यदि (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = रिक्त;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    शून्य
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = रिक्त;
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
#define HI2PT(hi)      HIKEY2PT((hi)->hi_किल्ली)

// The global text property types.
स्थिर hashtab_T *global_proptypes = रिक्त;
स्थिर proptype_T **global_proparray = रिक्त;

// The last used text property type ID.
स्थिर पूर्णाङ्क proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    स्थिर hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    यदि (*name == NUL)
	प्रत्यागम रिक्त;
    यदि (buf == रिक्त)
	ht = global_proptypes;
    अन्यथा
	ht = buf->b_proptypes;

    यदि (ht == रिक्त)
	प्रत्यागम रिक्त;
    hi = hash_find(ht, name);
    यदि (HASHITEM_EMPTY(hi))
	प्रत्यागम रिक्त;
    प्रत्यागम hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    स्थिर proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    यदि (hi == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    पूर्णाङ्क
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    यदि (pt == रिक्त)
	प्रत्यागम 0;
    प्रत्यागम pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    स्थिर proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = find_prop_type(name, buf);

    यदि (type == रिक्त)
	type = find_prop_type(name, रिक्त);
    यदि (type == रिक्त)
	semsg(_(e_property_type_str_does_not_exist), name);
    प्रत्यागम type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    स्थिर पूर्णाङ्क
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    यदि (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	प्रत्यागम FAIL;
    }
    यदि (arg->vval.v_dict == रिक्त)
	प्रत्यागम OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    यदि (di != रिक्त && (di->di_tv.v_type != VAR_संख्या
					      || di->di_tv.vval.v_संख्या != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	यदि (*buf == रिक्त)
	    प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    शून्य
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    start_lnum = tv_get_संख्या(&argvars[0]);
    start_col = tv_get_संख्या(&argvars[1]);
    यदि (check_for_dict_arg(argvars, 2) == FAIL)
	प्रत्यागम;

    rettv->vval.v_संख्या = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    स्थिर पूर्णाङ्क
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	पूर्णाङ्क		id,
	char_u		*text_arg,
	पूर्णाङ्क		text_padding_डावा,
	पूर्णाङ्क		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    पूर्णाङ्क		proplen;
    char_u	*props = रिक्त;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    पूर्णाङ्क		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    पूर्णाङ्क		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    यदि (type == रिक्त)
	गच्छ theend;

    यदि (start_lnum < 1 || start_lnum > buf->b_ml.ml_ओळ_मोजणी)
    {
	semsg(_(e_invalid_ओळ_संख्या_nr), (long)start_lnum);
	गच्छ theend;
    }
    यदि (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_ओळ_मोजणी)
    {
	semsg(_(e_invalid_ओळ_संख्या_nr), (long)end_lnum);
	गच्छ theend;
    }

    यदि (buf->b_ml.ml_mfp == रिक्त)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	गच्छ theend;
    }

    यदि (text != रिक्त)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	प्रति (p = text; *p != NUL; MB_PTR_ADV(p))
	    यदि (*p < ' ')
		*p = ' ';
    }

    प्रति (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	यदि (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_ओळ));
	    गच्छ theend;
	}

	यदि (lnum == start_lnum)
	    col = start_col;
	अन्यथा
	    col = 1;
	यदि (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != रिक्त))
	{
	    semsg(_(e_invalid_column_संख्या_nr), (long)start_col);
	    गच्छ theend;
	}
	sort_col = col;

	यदि (lnum == end_lnum)
	    length = end_col - col;
	अन्यथा
	    length = (पूर्णाङ्क)textlen - col + 1;
	यदि (length > (long)textlen)
	    length = (पूर्णाङ्क)textlen;	// can include the end-of-line
	यदि (length < 0)
	    length = 0;		// zero-width property

	यदि (text_arg != रिक्त)
	{
	    length = (long)STRLEN(text_arg);
	    यदि (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		यदि ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	tmp_prop.tp_padleft = text_padding_डावा;
	यदि (text_arg != रिक्त)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	प्रति (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * परिमाण(textprop_T),
							   परिमाण(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    यदि (prop_col >= sort_col)
		भङ्ग;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	पूर्णाङ्क		vtext_total = 0;
	पूर्णाङ्क		new_ओळ_len;
	char_u	*मोजणी_dest;
	char_u	*vtext_dest;
	पूर्णाङ्क		j;

	// Compute total vtext size from existing props.
	प्रति (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * परिमाण(textprop_T),
							परिमाण(textprop_T));
	    यदि (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	यदि (text_arg != रिक्त)
	    vtext_total += length + 1;

	new_ओळ_len = (पूर्णाङ्क)textlen + (पूर्णाङ्क)PROP_मोजणी_SIZE
			+ new_propcount * (पूर्णाङ्क)परिमाण(textprop_T)
			+ vtext_total;
	newtext = alloc(new_ओळ_len);
	यदि (newtext == रिक्त)
	    गच्छ theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_ओळ_ptr, textlen);

	// Write prop_count.
	मोजणी_dest = newtext + textlen;
	mch_memmove(मोजणी_dest, &new_propcount, PROP_मोजणी_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = मोजणी_dest + PROP_मोजणी_SIZE;
	यदि (i > 0)
	    mch_memmove(newprops, props, परिमाण(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	यदि (i < proplen)
	    mch_memmove(newprops + (i + 1) * परिमाण(textprop_T),
					props + i * परिमाण(textprop_T),
					परिमाण(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * परिमाण(textprop_T);
	प्रति (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    यदि (j == i)
		अनुवर्त;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * परिमाण(textprop_T),
		    परिमाण(textprop_T));
	    यदि (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_मोजणी_2 = (char_u *)props - PROP_मोजणी_SIZE;
		char_u *old_vtext = old_मोजणी_2 + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - मोजणी_dest);
		mch_memmove(newprops + j * परिमाण(textprop_T), &ep,
			परिमाण(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	यदि (text_arg != रिक्त)
	{
	    पूर्णाङ्क copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - मोजणी_dest);
	    vtext_dest += copysize;
	}
	अन्यथा
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * परिमाण(textprop_T), &tmp_prop,
							परिमाण(textprop_T));

	यदि (buf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_ओळ_ptr);
	buf->b_ml.ml_ओळ_ptr = newtext;
	buf->b_ml.ml_ओळ_len = new_ओळ_len;
	buf->b_ml.ml_flags |= ML_ओळ_DIRTY;
    }

    changed_ओळ_display_buf(buf);
    changed_ओळी_buf(buf, start_lnum, end_lnum + 1, 0);
    res = OK;

theend:
    vim_free(text);
    प्रत्यागम res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    शून्य
f_prop_add_list(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    पूर्णाङ्क		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    पूर्णाङ्क		error = FALSE;
    पूर्णाङ्क		prev_did_emsg = did_emsg;

    यदि (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    यदि (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    dict = argvars[0].vval.v_dict;
    यदि (dict == रिक्त || !dict_has_किल्ली(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	प्रत्यागम;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    यदि (dict_has_किल्ली(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_संख्या(dict, "id");
	यदि (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    प्रत्यागम;
	}
	id = (पूर्णाङ्क)x;
    }

    यदि (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	प्रत्यागम;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_वस्तुसंच(argvars[1].vval.v_list, li)
    {
	यदि (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == रिक्त)
	{
	    emsg(_(e_list_required));
	    प्रत्यागम;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	यदि (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	यदि (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	यदि (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	पूर्णाङ्क this_id = id;
	यदि (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	यदि (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    यदि (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    प्रत्यागम;
	}
	यदि (prop_add_one(buf, type_name, this_id, रिक्त, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    प्रत्यागम;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
स्थिर पूर्णाङ्क vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    स्थिर पूर्णाङ्क
get_textprop_id(शून्य)
{
    यदि (vt_id_counter > 0)
	vt_id_counter = 0;
    प्रत्यागम --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    पूर्णाङ्क
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
    पूर्णाङ्क		id = 0;
    char_u	*text = रिक्त;
    पूर्णाङ्क		text_padding_डावा = 0;
    पूर्णाङ्क		flags = 0;

    यदि (dict == रिक्त || !dict_has_किल्ली(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	गच्छ theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    यदि (dict_has_किल्ली(dict, "end_lnum"))
    {
	end_lnum = dict_get_संख्या(dict, "end_lnum");
	यदि (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "end_lnum");
	    गच्छ theend;
	}
    }
    अन्यथा
	end_lnum = start_lnum;

    यदि (dict_has_किल्ली(dict, "length"))
    {
	long length = dict_get_संख्या(dict, "length");

	यदि (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "length");
	    गच्छ theend;
	}
	end_col = start_col + length;
    }
    अन्यथा यदि (dict_has_किल्ली(dict, "end_col"))
    {
	end_col = dict_get_संख्या(dict, "end_col");
	यदि (end_col <= 0)
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "end_col");
	    गच्छ theend;
	}
    }
    अन्यथा यदि (start_lnum == end_lnum)
	end_col = start_col;
    अन्यथा
	end_col = 1;

    यदि (dict_has_किल्ली(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_संख्या(dict, "id");
	यदि (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    गच्छ theend;
	}
	id = (पूर्णाङ्क)x;
    }

    यदि (dict_has_किल्ली(dict, "text"))
    {
	यदि (dict_has_किल्ली(dict, "length")
		|| dict_has_किल्ली(dict, "end_col")
		|| dict_has_किल्ली(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    गच्छ theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	यदि (text == रिक्त)
	    गच्छ theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	यदि (dict_has_किल्ली(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    यदि (p == रिक्त)
		गच्छ theend;
	    यदि (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		गच्छ theend;
	    }
	    यदि (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_उजवा;
	    अन्यथा यदि (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    अन्यथा यदि (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    अन्यथा यदि (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str_str), "text_align", p);
		गच्छ theend;
	    }
	}

	यदि (dict_has_किल्ली(dict, "text_padding_left"))
	{
	    text_padding_डावा = dict_get_संख्या(dict, "text_padding_left");
	    यदि (text_padding_डावा < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		गच्छ theend;
	    }
	}

	यदि (dict_has_किल्ली(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    यदि (p == रिक्त)
		गच्छ theend;
	    यदि (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    अन्यथा यदि (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str_str), "text_wrap", p);
		गच्छ theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    यदि (start_col < (text == रिक्त ? 1 : 0))
    {
	semsg(_(e_invalid_column_संख्या_nr), (long)start_col);
	गच्छ theend;
    }
    यदि (start_col > 0 && text_padding_डावा > 0)
    {
	emsg(_(e_can_only_use_डावा_padding_when_column_is_zero));
	गच्छ theend;
    }

    यदि (dict_arg != रिक्त && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	गच्छ theend;

    यदि (text != रिक्त)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    अन्यथा यदि (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	गच्छ theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_डावा, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = रिक्त;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    प्रत्यागम id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    पूर्णाङ्क
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, पूर्णाङ्क will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_मोजणी;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    यदि ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == रिक्त)
	प्रत्यागम 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_ओळ_len - textlen;
    यदि (propdata_len == 0)
	प्रत्यागम 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    यदि (propdata_len < PROP_मोजणी_SIZE + परिमाण(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	प्रत्यागम 0;
    }
    mch_memmove(&prop_मोजणी, text + textlen, PROP_मोजणी_SIZE);
    यदि (!text_prop_मोजणी_valid(prop_मोजणी, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	प्रत्यागम 0;
    }
    यदि (!text_prop_vtext_valid(text + textlen + PROP_मोजणी_SIZE,
					   (पूर्णाङ्क)prop_मोजणी, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	प्रत्यागम 0;
    }
    *props = text + textlen + PROP_मोजणी_SIZE;
    प्रत्यागम (पूर्णाङ्क)prop_मोजणी;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    पूर्णाङ्क
prop_मोजणी_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    पूर्णाङ्क		मोजणी = get_text_props(buf, lnum, &props, FALSE);
    पूर्णाङ्क		result = 0;
    textprop_T	prop;
    पूर्णाङ्क		i;
    पूर्णाङ्क		next_उजवा_goes_below = FALSE;

    यदि (मोजणी == 0)
	प्रत्यागम 0;
    प्रति (i = 0; i < मोजणी; ++i)
    {
	mch_memmove(&prop, props + i * परिमाण(prop), परिमाण(prop));
	यदि (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    यदि ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_उजवा_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_उजवा)))
	    {
		++result;
	    }
	    अन्यथा यदि (prop.tp_flags & TP_FLAG_ALIGN_उजवा)
		next_उजवा_goes_below = TRUE;
	}
    }
    प्रत्यागम result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    पूर्णाङ्क
मोजणी_props(linenr_T lnum, पूर्णाङ्क only_starting, पूर्णाङ्क last_ओळ)
{
    char_u	*props;
    पूर्णाङ्क		proplen = get_text_props(curbuf, lnum, &props, 0);
    पूर्णाङ्क		result = proplen;
    textprop_T	prop;

    प्रति (पूर्णाङ्क i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * परिमाण(prop), परिमाण(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	यदि ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_ओळ && prop.tp_col == MAXCOL))
	    --result;
    }
    प्रत्यागम result;
}

स्थिर textprop_T	*text_prop_compare_props;
स्थिर buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    स्थिर पूर्णाङ्क
text_prop_order(पूर्णाङ्क flags)
{
    यदि (flags & TP_FLAG_ALIGN_ABOVE)
	प्रत्यागम 0;
    यदि (flags & TP_FLAG_ALIGN_उजवा)
	प्रत्यागम 2;
    यदि (flags & TP_FLAG_ALIGN_BELOW)
	प्रत्यागम 3;
    प्रत्यागम 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    स्थिर पूर्णाङ्क
text_prop_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    पूर्णाङ्क  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(पूर्णाङ्क *)s1;
    idx2 = *(पूर्णाङ्क *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    यदि ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	प्रत्यागम tp1->tp_id < 0 ? 1 : -1;

    यदि (col1 == MAXCOL || col2 == MAXCOL)
    {
	पूर्णाङ्क order1 = text_prop_order(tp1->tp_flags);
	पूर्णाङ्क order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	यदि (order1 != order2)
	    प्रत्यागम order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    यदि (pt1 != pt2)
    {
	यदि (pt1 == रिक्त)
	    प्रत्यागम -1;
	यदि (pt2 == रिक्त)
	    प्रत्यागम 1;
	यदि (pt1->pt_priority != pt2->pt_priority)
	    प्रत्यागम pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    यदि (col1 != col2)
	प्रत्यागम col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    यदि (tp1->tp_id < 0)
	प्रत्यागम tp1->tp_id > tp2->tp_id ? 1 : -1;

    प्रत्यागम 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    शून्य
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	पूर्णाङ्क	    *idxs,
	पूर्णाङ्क	    मोजणी)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((शून्य *)idxs, (size_t)मोजणी, परिमाण(पूर्णाङ्क), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_ओळी(
	win_T	    *wp,
	पूर्णाङ्क	    type_id,
	पूर्णाङ्क	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    यदि (text_prop_type_by_id(wp->w_buffer, type_id) == रिक्त)
	प्रत्यागम false;

    यदि (first_lnum < 1)
	first_lnum = 1;
    यदि (last_lnum > wp->w_buffer->b_ml.ml_ओळ_मोजणी)
	last_lnum = wp->w_buffer->b_ml.ml_ओळ_मोजणी;

    प्रति (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	पूर्णाङ्क	मोजणी = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	प्रति (पूर्णाङ्क i = 0; i < मोजणी; ++i)
	{
	    mch_memmove(prop, props + i * परिमाण(textprop_T),
							   परिमाण(textprop_T));
	    यदि (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		प्रत्यागम true;
	    }
	}
    }
    प्रत्यागम false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	पूर्णाङ्क	    type_id,
	पूर्णाङ्क	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    प्रत्यागम find_prop_in_ओळी(wp, type_id, id, prop, found_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    स्थिर शून्य
set_text_props(linenr_T lnum, textprop_T *tps, पूर्णाङ्क मोजणी)
{
    char_u  *text;
    char_u  *newtext;
    पूर्णाङ्क	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    यदि (मोजणी == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	यदि (newtext == रिक्त)
	    प्रत्यागम;
	mch_memmove(newtext, text, textlen);
	यदि (curbuf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ओळ_ptr);
	curbuf->b_ml.ml_ओळ_ptr = newtext;
	curbuf->b_ml.ml_ओळ_len = textlen;
	curbuf->b_ml.ml_flags |= ML_ओळ_DIRTY;
    }
    अन्यथा
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_मोजणी = (uint16_t)मोजणी;
	पूर्णाङ्क	    vtext_size = 0;
	पूर्णाङ्क	    total_len;
	char_u	    *मोजणी_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	पूर्णाङ्क	    i;

	प्रति (i = 0; i < मोजणी; ++i)
	    यदि (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (पूर्णाङ्क)PROP_मोजणी_SIZE
		    + मोजणी * (पूर्णाङ्क)परिमाण(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	यदि (newtext == रिक्त)
	    प्रत्यागम;
	mch_memmove(newtext, text, textlen);

	मोजणी_dest = newtext + textlen;
	mch_memmove(मोजणी_dest, &prop_मोजणी, PROP_मोजणी_SIZE);

	prop_dest = मोजणी_dest + PROP_मोजणी_SIZE;
	vtext_dest = prop_dest + मोजणी * परिमाण(textprop_T);

	प्रति (i = 0; i < मोजणी; ++i)
	{
	    textprop_T prop = tps[i];

	    यदि (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		पूर्णाङ्क copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = रिक्त;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - मोजणी_dest);
		vtext_dest += copysize;
	    }
	    अन्यथा
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, परिमाण(textprop_T));
	    prop_dest += परिमाण(textprop_T);
	}

	यदि (curbuf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ओळ_ptr);
	curbuf->b_ml.ml_ओळ_ptr = newtext;
	curbuf->b_ml.ml_ओळ_len = total_len;
	curbuf->b_ml.ml_flags |= ML_ओळ_DIRTY;
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
props_add_मोजणी_header(char_u *ओळ, पूर्णाङ्क ओळ_len, पूर्णाङ्क textlen, पूर्णाङ्क *new_len)
{
    पूर्णाङ्क		prop_bytes = ओळ_len - textlen;
    uint16_t	prop_मोजणी;
    पूर्णाङ्क		vtext_size = 0;
    पूर्णाङ्क		total;
    char_u	*newline;
    char_u	*मोजणी_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    पूर्णाङ्क		i;

    यदि (prop_bytes <= 0 || prop_bytes % परिमाण(textprop_T) != 0)
    {
	*new_len = ओळ_len;
	प्रत्यागम रिक्त;
    }
    prop_मोजणी = (uint16_t)(prop_bytes / परिमाण(textprop_T));

    // Calculate total vtext size.
    प्रति (i = 0; i < (पूर्णाङ्क)prop_मोजणी; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ओळ + textlen + i * परिमाण(textprop_T),
							परिमाण(textprop_T));
	यदि (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (पूर्णाङ्क)PROP_मोजणी_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    यदि (newline == रिक्त)
	प्रत्यागम रिक्त;
    *new_len = total;

    mch_memmove(newline, ओळ, textlen);
    मोजणी_dest = newline + textlen;
    mch_memmove(मोजणी_dest, &prop_मोजणी, PROP_मोजणी_SIZE);
    prop_dest = मोजणी_dest + PROP_मोजणी_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    प्रति (i = 0; i < (पूर्णाङ्क)prop_मोजणी; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ओळ + textlen + i * परिमाण(textprop_T),
							परिमाण(textprop_T));
	यदि (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    पूर्णाङ्क copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - मोजणी_dest);
	    vtext_dest += copysize;
	}
	अन्यथा
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * परिमाण(textprop_T), &prop,
							परिमाण(textprop_T));
    }
    प्रत्यागम newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    शून्य
add_text_props(linenr_T lnum, textprop_T *text_props, पूर्णाङ्क text_prop_मोजणी)
{
    unpacked_memline_T	um;
    पूर्णाङ्क			i;

    um = um_open_at(curbuf, lnum, text_prop_मोजणी);
    यदि (um.buf == रिक्त)
	प्रत्यागम;
    यदि (!um_detach(&um))
    {
	um_abort(&um);
	प्रत्यागम;
    }

    // Grow props array if needed.
    यदि (um.prop_मोजणी + text_prop_मोजणी > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_मोजणी + text_prop_मोजणी) * परिमाण(textprop_T));
	यदि (newprops == रिक्त)
	{
	    um_abort(&um);
	    प्रत्यागम;
	}
	um.props = newprops;
	um.prop_size = um.prop_मोजणी + text_prop_मोजणी;
    }

    प्रति (i = 0; i < text_prop_मोजणी; ++i)
	um.props[um.prop_मोजणी++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    स्थिर पूर्णाङ्क
compare_pt(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    प्रत्यागम tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    स्थिर proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, पूर्णाङ्क id)
{
    पूर्णाङ्क खालचा = 0;
    पूर्णाङ्क वरचा;

    यदि (ht == रिक्त || ht->ht_used == 0)
	प्रत्यागम रिक्त;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    यदि (*array == रिक्त)
    {
	long	    todo;
	hashitem_T  *hi;
	पूर्णाङ्क	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	यदि (*array == रिक्त)
	    प्रत्यागम रिक्त;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_वस्तुसंच(ht, hi, todo)
	{
	    यदि (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((शून्य *)*array, ht->ht_used, परिमाण(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    वरचा = ht->ht_used;
    यावत् (वरचा > खालचा)
    {
	पूर्णाङ्क m = (वरचा + खालचा) / 2;

	यदि ((*array)[m]->pt_id == id)
	    प्रत्यागम (*array)[m];
	यदि ((*array)[m]->pt_id > id)
	    वरचा = m;
	अन्यथा
	    खालचा = m + 1;
    }
    प्रत्यागम रिक्त;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    स्थिर शून्य
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    पूर्णाङ्क buflocal = TRUE;
    पूर्णाङ्क virtualtext_prop = prop->tp_id < 0;

    dict_add_संख्या(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    यदि (!virtualtext_prop)
    {
	dict_add_संख्या(dict, "length", prop->tp_len);
	dict_add_संख्या(dict, "id", prop->tp_id);
    }
    dict_add_संख्या(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_संख्या(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    यदि (pt == रिक्त)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    यदि (pt != रिक्त)
	dict_add_string(dict, "type", pt->pt_name);

    यदि (buflocal)
	dict_add_संख्या(dict, "type_bufnr", buf->b_fnum);
    अन्यथा
	dict_add_संख्या(dict, "type_bufnr", 0);
    यदि (virtualtext_prop)
    {
	string_T    text_align = {रिक्त, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	यदि (prop->tp_flags & TP_FLAG_ALIGN_उजवा)
	    STR_LITERAL_SET(text_align, "right");
	अन्यथा यदि (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	अन्यथा यदि (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	यदि (text_align.string != रिक्त)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (पूर्णाङ्क)text_align.length);

	// text_wrap
	यदि (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	यदि (prop->tp_padleft != 0)
	    dict_add_संख्या(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, पूर्णाङ्क id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    यदि (type == रिक्त)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    प्रत्यागम type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    प्रत्यागम text_prop_type_by_id(buf, prop->tp_type) != रिक्त;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    शून्य
f_prop_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    linenr_T start;
    linenr_T end;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    पूर्णाङ्क	    did_clear = FALSE;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    start = tv_get_संख्या(&argvars[0]);
    end = start;
    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_संख्या(&argvars[1]);
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	{
	    यदि (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		प्रत्यागम;
	}
    }
    यदि (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	प्रत्यागम;
    }

    {
	unpacked_memline_T um = um_open(buf);

	प्रति (lnum = start; lnum <= end; ++lnum)
	{
	    पूर्णाङ्क idx;

	    यदि (lnum > buf->b_ml.ml_ओळ_मोजणी)
		भङ्ग;
	    यदि (!um_goto_ओळ(&um, lnum, 0))
		भङ्ग;
	    यदि (um.prop_मोजणी > 0)
	    {
		did_clear = TRUE;
		प्रति (idx = um.prop_मोजणी - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    यदि (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    शून्य
f_prop_find(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    पूर्णाङ्क		lnum_start;
    पूर्णाङ्क		start_pos_has_prop = 0;
    पूर्णाङ्क		seen_end = FALSE;
    पूर्णाङ्क		id = 0;
    पूर्णाङ्क		id_found = FALSE;
    पूर्णाङ्क		type_id = -1;
    पूर्णाङ्क		skipstart = FALSE;
    पूर्णाङ्क		lnum = -1;
    पूर्णाङ्क		col = -1;
    पूर्णाङ्क		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    पूर्णाङ्क		both;

    यदि (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    यदि (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	प्रत्यागम;
    dict = argvars[0].vval.v_dict;

    यदि (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	प्रत्यागम;
    यदि (buf->b_ml.ml_mfp == रिक्त)
	प्रत्यागम;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	यदि (*dir_s == 'b')
	    dir = BACKWARD;
	अन्यथा यदि (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    प्रत्यागम;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    यदि (di != रिक्त)
	lnum = tv_get_संख्या(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    यदि (di != रिक्त)
	col = tv_get_संख्या(&di->di_tv);

    यदि (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    अन्यथा यदि (col == -1)
	col = 1;

    यदि (lnum < 1 || lnum > buf->b_ml.ml_ओळ_मोजणी)
    {
	emsg(_(e_invalid_range));
	प्रत्यागम;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    यदि (dict_has_किल्ली(dict, "id"))
    {
	id = dict_get_संख्या(dict, "id");
	id_found = TRUE;
    }
    यदि (dict_has_किल्ली(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	यदि (type == रिक्त)
	    प्रत्यागम;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    यदि (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	प्रत्यागम;
    }
    यदि (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	प्रत्यागम;
    }

    lnum_start = lnum;

    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;

    यावत् (1)
    {
	char_u	*prop_start_ptr;
	पूर्णाङ्क	मोजणी = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	पूर्णाङ्क	    i;
	textprop_T  prop;
	पूर्णाङ्क	    prop_start;
	पूर्णाङ्क	    prop_end;

	प्रति (i = dir == BACKWARD ? मोजणी - 1 : 0; i >= 0 && i < मोजणी; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * परिमाण(textprop_T),
							   परिमाण(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    यदि (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_मोजणी_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    अन्यथा
		prop.u.tp_text = रिक्त;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    यदि (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		यदि (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    यदि (!is_floating_vtext && prop.tp_col > col)
			अनुवर्त;
		}
		अन्यथा यदि (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    अनुवर्त;
	    }
	    यदि (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		यदि (lnum_start == lnum
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
		यदि (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		यदि (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    यदि (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    अनुवर्त;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		यदि (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    अनुवर्त;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_संख्या(rettv->vval.v_dict, "lnum", lnum);

		प्रत्यागम;
	    }
	}

	यदि (dir > 0)
	{
	    यदि (lnum >= buf->b_ml.ml_ओळ_मोजणी)
		भङ्ग;
	    lnum++;
	}
	अन्यथा
	{
	    यदि (lnum <= 1)
		भङ्ग;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    स्थिर पूर्णाङ्क
prop_type_or_id_in_list(पूर्णाङ्क *types_or_ids, पूर्णाङ्क len, पूर्णाङ्क type_or_id)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < len; i++)
	यदि (types_or_ids[i] == type_or_id)
	    प्रत्यागम TRUE;

    प्रत्यागम FALSE;
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
    स्थिर शून्य
get_props_in_ओळ(
	buf_T		*buf,
	linenr_T	lnum,
	पूर्णाङ्क		*prop_types,
	पूर्णाङ्क		prop_types_len,
	पूर्णाङ्क		*prop_ids,
	पूर्णाङ्क		prop_ids_len,
	list_T		*retlist,
	पूर्णाङ्क		add_lnum)
{
    char_u	*props;
    पूर्णाङ्क		मोजणी;
    पूर्णाङ्क		i;
    textprop_T	prop;

    मोजणी = get_text_props(buf, lnum, &props, FALSE);
    प्रति (i = 0; i < मोजणी; ++i)
    {
	mch_memmove(&prop, props + i * परिमाण(textprop_T),
		परिमाण(textprop_T));
	// Convert offset to pointer for virtual text props.
	यदि (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_मोजणी_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	अन्यथा
	    prop.u.tp_text = रिक्त;
	यदि ((prop_types == रिक्त
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == रिक्त
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    यदि (d == रिक्त)
		भङ्ग;
	    prop_fill_dict(d, &prop, buf);
	    यदि (add_lnum)
		dict_add_संख्या(d, "lnum", lnum);
	    यदि (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    स्थिर पूर्णाङ्क *
get_prop_types_from_names(list_T *l, buf_T *buf, पूर्णाङ्क *अंक_types)
{
    पूर्णाङ्क		*prop_types;
    listitem_T	*li;
    पूर्णाङ्क		i;
    char_u	*name;
    proptype_T	*type;

    *अंक_types = 0;

    prop_types = ALLOC_MULT(पूर्णाङ्क, list_len(l));
    यदि (prop_types == रिक्त)
	प्रत्यागम रिक्त;

    i = 0;
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	यदि (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    गच्छ errret;
	}
	name = li->li_tv.vval.v_string;
	यदि (name == रिक्त)
	    गच्छ errret;

	type = lookup_prop_type(name, buf);
	यदि (type == रिक्त)
	    गच्छ errret;
	prop_types[i++] = type->pt_id;
    }

    *अंक_types = i;
    प्रत्यागम prop_types;

errret:
    VIM_CLEAR(prop_types);
    प्रत्यागम रिक्त;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    स्थिर पूर्णाङ्क *
get_prop_ids_from_list(list_T *l, पूर्णाङ्क *अंक_ids)
{
    पूर्णाङ्क		*prop_ids;
    listitem_T	*li;
    पूर्णाङ्क		i = 0;
    पूर्णाङ्क		id;
    पूर्णाङ्क		error;

    *अंक_ids = 0;

    prop_ids = ALLOC_MULT(पूर्णाङ्क, list_len(l));
    यदि (prop_ids == रिक्त)
	प्रत्यागम रिक्त;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	error = FALSE;
	id = tv_get_संख्या_chk(&li->li_tv, &error);
	यदि (error)
	    गच्छ errret;

	prop_ids[i++] = id;
    }

    *अंक_ids = i;
    प्रत्यागम prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    प्रत्यागम रिक्त;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    शून्य
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    buf_T	*buf = curbuf;
    पूर्णाङ्क		add_lnum = FALSE;
    पूर्णाङ्क		*prop_types = रिक्त;
    पूर्णाङ्क		prop_types_len = 0;
    पूर्णाङ्क		*prop_ids = रिक्त;
    पूर्णाङ्क		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    // default: get text properties on current line
    start_lnum = tv_get_संख्या(&argvars[0]);
    end_lnum = start_lnum;
    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	यदि (check_for_dict_arg(argvars, 1) == FAIL)
	    प्रत्यागम;
	d = argvars[1].vval.v_dict;

	यदि (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    प्रत्यागम;

	यदि (d != रिक्त && (di = dict_find(d, (char_u *)"end_lnum", -1)) != रिक्त)
	{
	    यदि (di->di_tv.v_type != VAR_संख्या)
	    {
		emsg(_(e_संख्या_required));
		प्रत्यागम;
	    }
	    end_lnum = tv_get_संख्या(&di->di_tv);
	    यदि (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//ओळ
		end_lnum = buf->b_ml.ml_ओळ_मोजणी + end_lnum + 1;
	    अन्यथा यदि (end_lnum > buf->b_ml.ml_ओळ_मोजणी)
		end_lnum = buf->b_ml.ml_ओळ_मोजणी;
	    add_lnum = TRUE;
	}
	यदि (d != रिक्त && (di = dict_find(d, (char_u *)"types", -1)) != रिक्त)
	{
	    यदि (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		प्रत्यागम;
	    }

	    l = di->di_tv.vval.v_list;
	    यदि (l != रिक्त && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		यदि (prop_types == रिक्त)
		    प्रत्यागम;
	    }
	}
	यदि (d != रिक्त && (di = dict_find(d, (char_u *)"ids", -1)) != रिक्त)
	{
	    यदि (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		गच्छ errret;
	    }

	    l = di->di_tv.vval.v_list;
	    यदि (l != रिक्त && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		यदि (prop_ids == रिक्त)
		    गच्छ errret;
	    }
	}
    }
    यदि (start_lnum < 1 || start_lnum > buf->b_ml.ml_ओळ_मोजणी
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    अन्यथा
	प्रति (lnum = start_lnum; lnum <= end_lnum; lnum++)
	    get_props_in_ओळ(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, add_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    शून्य
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start = 1;
    linenr_T	end = 0;
    linenr_T	lnum;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    पूर्णाङ्क		do_all;
    पूर्णाङ्क		id = -MAXCOL;
    पूर्णाङ्क		type_id = -1;	    // for a single "type"
    पूर्णाङ्क		*type_ids = रिक्त;   // array, for a list of "types", allocated
    पूर्णाङ्क		अंक_type_ids = 0;   // number of elements in "type_ids"
    पूर्णाङ्क		both;

    rettv->vval.v_संख्या = 0;

    यदि (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 2) == FAIL)))
	प्रत्यागम;

    यदि (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_संख्या(&argvars[1]);
	end = start;
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_संख्या(&argvars[2]);
	यदि (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    प्रत्यागम;
	}
    }

    dict = argvars[0].vval.v_dict;
    यदि (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	प्रत्यागम;
    यदि (buf->b_ml.ml_mfp == रिक्त)
	प्रत्यागम;

    do_all = dict_get_bool(dict, "all", FALSE);

    यदि (dict_has_किल्ली(dict, "id"))
	id = dict_get_संख्या(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    यदि (dict_has_किल्ली(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	यदि (type == रिक्त)
	    प्रत्यागम;
	type_id = type->pt_id;
    }
    यदि (dict_has_किल्ली(dict, "types"))
    {
	typval_T types;
	listitem_T *li = रिक्त;

	dict_get_tv(dict, "types", &types);
	यदि (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( परिमाण(पूर्णाङ्क) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_वस्तुसंच(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		यदि (li->li_tv.v_type != VAR_STRING)
		    अनुवर्त;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		यदि (!prop_type)
		    गच्छ cleanup_prop_remove;

		type_ids[अंक_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    यदि (id == -MAXCOL && (type_id == -1 && अंक_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	गच्छ cleanup_prop_remove;
    }
    यदि (both && (id == -MAXCOL || (type_id == -1 && अंक_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	गच्छ cleanup_prop_remove;
    }
    यदि (type_id != -1 && अंक_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	गच्छ cleanup_prop_remove;
    }

    यदि (end == 0)
	end = buf->b_ml.ml_ओळ_मोजणी;

    {
	unpacked_memline_T um = um_open(buf);

	प्रति (lnum = start; lnum <= end; ++lnum)
	{
	    पूर्णाङ्क idx;

	    यदि (lnum > buf->b_ml.ml_ओळ_मोजणी)
		भङ्ग;
	    यदि (!um_goto_ओळ(&um, lnum, 0))
		भङ्ग;

	    प्रति (idx = 0; idx < um.prop_मोजणी; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		पूर्णाङ्क	    matches_id = 0;
		पूर्णाङ्क	    matches_type = 0;

		यदि (prop->tp_flags & TP_FLAG_DELETED)
		    अनुवर्त;

		matches_id = prop->tp_id == id;
		यदि (अंक_type_ids > 0)
		{
		    पूर्णाङ्क idx2;

		    प्रति (idx2 = 0; !matches_type && idx2 < अंक_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		अन्यथा
		    matches_type = prop->tp_type == type_id;

		यदि (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    यदि (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_संख्या;
		    यदि (!do_all)
			भङ्ग;
		}
	    }
	}
	um_close(&um);
    }

    यदि (first_changed > 0)
    {
	changed_ओळ_display_buf(buf);
	changed_ओळी_buf(buf, first_changed, last_changed + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    स्थिर शून्य
prop_type_set(typval_T *argvars, पूर्णाङ्क add)
{
    char_u	*name;
    buf_T	*buf = रिक्त;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    name = tv_get_string(&argvars[0]);
    यदि (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	प्रत्यागम;
    }

    यदि (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	प्रत्यागम;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    यदि (add)
    {
	hashtab_T **htp;

	यदि (prop != रिक्त)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    प्रत्यागम;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	यदि (prop == रिक्त)
	    प्रत्यागम;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	यदि (buf == रिक्त)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	अन्यथा
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	यदि (*htp == रिक्त)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    यदि (*htp == रिक्त)
	    {
		vim_free(prop);
		प्रत्यागम;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    अन्यथा
    {
	यदि (prop == रिक्त)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    प्रत्यागम;
	}
    }

    यदि (dict != रिक्त)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	यदि (di != रिक्त)
	{
	    char_u	*highlight;
	    पूर्णाङ्क		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    यदि (highlight != रिक्त && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    यदि (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == रिक्त ? (char_u *)"" : highlight);
		प्रत्यागम;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	यदि (di != रिक्त)
	{
	    यदि (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    अन्यथा
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	यदि (di != रिक्त)
	{
	    यदि (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    अन्यथा
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	यदि (di != रिक्त)
	    prop->pt_priority = tv_get_संख्या(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	यदि (di != रिक्त)
	{
	    यदि (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    अन्यथा
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	यदि (di != रिक्त)
	{
	    यदि (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    अन्यथा
		prop->pt_flags &= ~PT_FLAG_INS_END_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    शून्य
f_prop_type_add(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    शून्य
f_prop_type_change(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    शून्य
f_prop_type_delete(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*name;
    buf_T	*buf = रिक्त;
    hashitem_T	*hi;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    name = tv_get_string(&argvars[0]);
    यदि (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	प्रत्यागम;
    }

    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	यदि (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    प्रत्यागम;
    }

    hi = find_prop_type_hi(name, buf);
    यदि (hi == रिक्त)
	प्रत्यागम;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    यदि (buf == रिक्त)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    अन्यथा
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    changed_window_setting_buf(buf == रिक्त ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    शून्य
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    name = tv_get_string(&argvars[0]);
    यदि (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	प्रत्यागम;
    }

    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;

    proptype_T  *prop = रिक्त;
    buf_T	    *buf = रिक्त;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	यदि (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    प्रत्यागम;
    }

    prop = find_prop_type(name, buf);
    यदि (prop == रिक्त)
	प्रत्यागम;

    dict_T *d = rettv->vval.v_dict;

    यदि (prop->pt_hl_id > 0)
	dict_add_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_add_संख्या(d, "priority", prop->pt_priority);
    dict_add_संख्या(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_add_संख्या(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_add_संख्या(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_START_INCL) ? 1 : 0);
    dict_add_संख्या(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_END_INCL) ? 1 : 0);
    यदि (buf != रिक्त)
	dict_add_संख्या(d, "bufnr", buf->b_fnum);
}

    स्थिर शून्य
list_types(hashtab_T *ht, list_T *l)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तुसंच(ht, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
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
    शून्य
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = रिक्त;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type != VAR_UNKNOWN)
    {
	यदि (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    प्रत्यागम;
    }
    यदि (buf == रिक्त)
    {
	यदि (global_proptypes != रिक्त)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    अन्यथा यदि (buf->b_proptypes != रिक्त)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    स्थिर शून्य
clear_ht_prop_types(hashtab_T *ht)
{
    long	todo;
    hashitem_T	*hi;

    यदि (ht == रिक्त)
	प्रत्यागम;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तुसंच(ht, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#यदि defined(EXITFREE)
/*
 * Free all global property types.
 */
    शून्य
clear_global_prop_types(शून्य)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = रिक्त;
    VIM_CLEAR(global_proparray);
}
#endif

/*
 * Free all property types for "buf".
 */
    शून्य
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = रिक्त;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
प्रकारनिर्देश संरचना
{
    पूर्णाङ्क dirty;	    // if the property was changed
    पूर्णाङ्क can_drop;   // whether after this change, the prop may be removed
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
    स्थिर adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    col,
	पूर्णाङ्क	    added,
	पूर्णाङ्क	    flags)
{
    proptype_T	*pt;
    पूर्णाङ्क		start_incl;
    पूर्णाङ्क		end_incl;
    पूर्णाङ्क		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    यदि (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	प्रत्यागम res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != रिक्त && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    यदि (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != रिक्त && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    यदि (added > 0)
    {
	यदि (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	अन्यथा यदि (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    अन्यथा यदि (prop->tp_col > col + 1)
    {
	यदि (prop->tp_col + added < col + 1)
	{
	    यदि (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    अन्यथा
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		यदि (prop->tp_len <= 0)
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
	अन्यथा
	    prop->tp_col += added;
    }
    अन्यथा यदि (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	यदि (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	अन्यथा
	{
	    पूर्णाङ्क after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	यदि (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    अन्यथा
	res.dirty = FALSE;

    प्रत्यागम res;
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
    पूर्णाङ्क
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	पूर्णाङ्क	    bytes_added,
	पूर्णाङ्क	    flags)
{
    unpacked_memline_T um;
    पूर्णाङ्क		dirty = FALSE;
    पूर्णाङ्क		ri;

    यदि (text_prop_frozen > 0)
	प्रत्यागम FALSE;

    um = um_open_at(curbuf, lnum, 0);
    यदि (um.buf == रिक्त || um.prop_मोजणी == 0)
    {
	um_abort(&um);
	प्रत्यागम FALSE;
    }

    प्रति (ri = 0; ri < um.prop_मोजणी; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	यदि (res.dirty)
	{
	    यदि (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		यदि (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    प्रत्यागम FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		यदि ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    प्रत्यागम FALSE;
		}
	    }
	    dirty = TRUE;
	}
	यदि (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    अनुवर्त;
	}
    }
    यदि (dirty)
	um_close(&um);
    अन्यथा
	um_abort(&um);
    प्रत्यागम dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    शून्य
adjust_props_for_split(
	linenr_T    lnum_props,
	linenr_T    lnum_शीर्ष,
	पूर्णाङ्क	    kept,
	पूर्णाङ्क	    deleted,
	पूर्णाङ्क	    at_eol)
{
    char_u	*props;
    पूर्णाङ्क		मोजणी;
    garray_T    prevprop;
    garray_T    nextprop;
    पूर्णाङ्क		skipped = kept + deleted;

    यदि (!curbuf->b_has_textprop)
	प्रत्यागम;

    // Get the text properties from "lnum_props".
    मोजणी = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, परिमाण(textprop_T), 10);
    ga_init2(&nextprop, परिमाण(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *मोजणी_ptr = props - PROP_मोजणी_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    प्रति (पूर्णाङ्क i = 0; i < मोजणी; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	पूर्णाङ्क	    start_incl, end_incl;
	पूर्णाङ्क	    cont_prev, cont_next;
	पूर्णाङ्क	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * परिमाण(textprop_T), परिमाण(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	यदि (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(मोजणी_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	अन्यथा
	    prop.u.tp_text = रिक्त;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != रिक्त && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != रिक्त && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	यदि (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	अन्यथा
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    पूर्णाङ्क is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    यदि (is_floating_vtext)
		cont_next = FALSE;
	    अन्यथा
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	यदि (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	यदि (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    पूर्णाङ्क is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    यदि (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    यदि (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	यदि (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    पूर्णाङ्क is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    यदि (p->tp_col != MAXCOL)
	    {
		यदि (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		अन्यथा
		{
		    यदि (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    यदि (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    set_text_props(lnum_शीर्ष, (textprop_T *)prevprop.ga_data,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    set_text_props(lnum_शीर्ष + 1, (textprop_T *)nextprop.ga_data,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    शून्य
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	पूर्णाङ्क		    last_ओळ,
	long		    col,
	पूर्णाङ्क		    removed)
{
    unpacked_memline_T	r_um;

    यदि (um->buf == रिक्त)
	प्रत्यागम;

    r_um = um_open_at(um->buf, lnum, 0);
    यदि (r_um.buf == रिक्त)
	प्रत्यागम;
    यदि (!um_detach(&r_um))
    {
	um_abort(&r_um);
	प्रत्यागम;
    }

    प्रति (पूर्णाङ्क i = r_um.prop_मोजणी - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	पूर्णाङ्क	    end;

	यदि (prop->tp_col == MAXCOL && !last_ओळ)
	    अनुवर्त;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	यदि (last_ओळ || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = रिक्त;  // ownership transferred
	}
	अन्यथा
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    प्रति (पूर्णाङ्क j = 0; j < um->prop_मोजणी; ++j)
	    {
		textprop_T *op = &um->props[j];

		यदि ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    भङ्ग;
		}
	    }
	    यदि (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    स्थिर शून्य
remember_cleared_prop(garray_T *gap, पूर्णाङ्क which, textprop_T *prop, पूर्णाङ्क flag)
{
    undoprop_T	*up;

    यदि (which == UNDOPROP_NONE)
	प्रत्यागम;
    यदि (gap->ga_itemsize == 0)
	ga_init2(gap, परिमाण(undoprop_T), 4);
    यदि (ga_grow(gap, 1) == FAIL)
	प्रत्यागम;
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
    शून्य
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	पूर्णाङ्क	    above_which,
	पूर्णाङ्क	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = रिक्त;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    यदि (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_ओळ_मोजणी)
	प्रत्यागम;

    above = um_open_at(buf, lnum, 0);
    यदि (above.buf == रिक्त)
	प्रत्यागम;
    प्रति (पूर्णाङ्क i = 0; i < above.prop_मोजणी; ++i)
	यदि (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    यदि (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	प्रत्यागम;
    }

    below = um_open_at(buf, lnum + 1, 0);
    यदि (below.buf == रिक्त)
    {
	um_abort(&above);
	प्रत्यागम;
    }
    यदि (below.prop_मोजणी > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_मोजणी);
	यदि (paired == रिक्त)
	    गच्छ theend;
    }

    प्रति (पूर्णाङ्क i = 0; i < above.prop_मोजणी; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	यदि (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    अनुवर्त;
	प्रति (पूर्णाङ्क j = 0; j < below.prop_मोजणी; ++j)
	{
	    textprop_T *next = &below.props[j];

	    यदि (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		भङ्ग;
	    }
	}
	यदि (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    प्रति (पूर्णाङ्क j = 0; j < below.prop_मोजणी; ++j)
    {
	यदि (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    अनुवर्त;
	यदि (!below.detached && !um_detach(&below))
	    भङ्ग;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    यदि (below_changed)
	um_close(&below);
    अन्यथा
	um_abort(&below);
    यदि (above_changed)
	um_close(&above);
    अन्यथा
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    शून्य
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	पूर्णाङ्क	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    यदि (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_ओळ_मोजणी)
	प्रत्यागम;
    um = um_open_at(buf, lnum, 0);
    यदि (um.buf == रिक्त)
	प्रत्यागम;
    प्रति (पूर्णाङ्क r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	यदि (up->up_which != which)
	    अनुवर्त;
	प्रति (पूर्णाङ्क i = 0; i < um.prop_मोजणी; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    यदि (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		अनुवर्त;
	    यदि (!um.detached && !um_detach(&um))
		गच्छ theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    भङ्ग;
	}
    }

theend:
    यदि (changed)
	um_close(&um);
    अन्यथा
	um_abort(&um);
}

    bool
text_prop_मोजणी_valid(पूर्णाङ्क prop_मोजणी, size_t propdata_len)
{
    यदि (propdata_len < PROP_मोजणी_SIZE)
	प्रत्यागम false;
    प्रत्यागम (size_t)prop_मोजणी * परिमाण(textprop_T)
		    <= propdata_len - PROP_मोजणी_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, पूर्णाङ्क prop_मोजणी, size_t propdata_len)
{
    प्रति (पूर्णाङ्क i = 0; i < prop_मोजणी; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * परिमाण(textprop_T),
							  परिमाण(textprop_T));
	यदि (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    अनुवर्त;
	यदि (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    प्रत्यागम false;
    }
    प्रत्यागम true;
}

#endif // FEAT_PROP_POPUP
