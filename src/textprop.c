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

#जर defined(FEAT_PROP_POPUP)

स्थिर शून्य um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    स्थिर शून्य
um_free_vtext(textprop_T *props, पूर्णांक मोजणी)
{
    साठी (पूर्णांक i = 0; i < मोजणी; ++i)
	जर (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    स्थिर शून्य
um_free(unpacked_memline_T *um)
{
    जर (um->detached)
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
    परत um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_ओळ(unpacked_memline_T *um, linenr_T lnum, पूर्णांक extra_props)
{
    char_u	*ओळ;
    माप_प्रकार	textlen;
    माप_प्रकार	propdata_len;
    पूर्णांक		proplen;

    जर (um->buf == रिक्त)
	परत false;

    // Store changes to the current line if any.
    जर (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    जर (lnum == 0)
	परत true;	// just unload

    ओळ = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_ओळ_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = ओळ;

    जर (propdata_len == 0)
	परत true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    जर (propdata_len < PROP_मोजणी_SIZE + आकार(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = रिक्त;
	परत false;
    }

    uint16_t    prop_मोजणी;
    char_u	    *मोजणी_ptr = ओळ + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_मोजणी, मोजणी_ptr, PROP_मोजणी_SIZE);
    जर (!text_prop_मोजणी_valid(prop_मोजणी, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = रिक्त;
	परत false;
    }
    proplen = (पूर्णांक)prop_मोजणी;
    props_start = मोजणी_ptr + PROP_मोजणी_SIZE;

    जर (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = रिक्त;
	परत false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    जर (um->props == रिक्त)
    {
	um->buf = रिक्त;
	परत false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_मोजणी = proplen;
    mch_memmove(um->props, props_start, proplen * आकार(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    साठी (पूर्णांक i = 0; i < proplen; ++i)
    {
	जर (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = मोजणी_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	अन्यथा
	    um->props[i].u.tp_text = रिक्त;
    }

    परत true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, पूर्णांक extra_props)
{
    unpacked_memline_T um = um_open(buf);

    जर (!um_goto_ओळ(&um, lnum, extra_props))
	um.buf = रिक्त;
    परत um;
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

    जर (um->detached || um->buf == रिक्त)
	परत um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    जर (newtext == रिक्त)
    {
	um->buf = रिक्त;
	परत false;
    }
    um->text = newtext;

    साठी (पूर्णांक i = 0; i < um->prop_मोजणी; ++i)
    {
	जर (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    जर (copy == रिक्त)
	    {
		साठी (पूर्णांक j = 0; j < i; ++j)
		    जर (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = रिक्त;
		परत false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    परत true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    जर (um->buf == रिक्त)
	परत false;
    जर (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    परत true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    स्थिर bool
um_grow_props(unpacked_memline_T *um, पूर्णांक needed)
{
    जर (um->prop_मोजणी + needed <= um->prop_size)
	परत true;

    पूर्णांक	    new_size = um->prop_मोजणी + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * आकार(textprop_T));
    जर (newprops == रिक्त)
	परत false;
    um->props = newprops;
    um->prop_size = new_size;

    परत true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    स्थिर शून्य
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    जर (um->buf == रिक्त)
	परत;
    जर (!um->detached && !um_detach(um))
	परत;
    जर (!um_grow_props(um, 1))
    {
	um->buf = रिक्त;
	परत;
    }
    um->props[um->prop_मोजणी++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    शून्य
um_reverse_props(unpacked_memline_T *um)
{
    साठी (पूर्णांक i = 0, j = um->prop_मोजणी - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, पूर्णांक prop_मोजणी)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    जर (lnum < 1 || lnum > buf->b_ml.ml_ओळ_मोजणी)
    {
	um.buf = रिक्त;
	परत um;
    }

    char_u *ओळ = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(ओळ);

    जर (text_copy == रिक्त)
    {
	um.buf = रिक्त;
	परत um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    जर (prop_मोजणी > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_मोजणी);
	जर (um.props == रिक्त)
	{
	    vim_free(um.text);
	    um.buf = रिक्त;
	    परत um;
	}
	um.prop_size = prop_मोजणी;
    }

    परत um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    स्थिर शून्य
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					पूर्णांक tp_id, पूर्णांक tp_type, पूर्णांक flag)
{
    unpacked_memline_T	neighbor;

    जर (lnum < 1 || lnum > buf->b_ml.ml_ओळ_मोजणी)
	परत;

    neighbor = um_open_at(buf, lnum, 0);
    जर (neighbor.buf == रिक्त)
	परत;

    साठी (पूर्णांक i = 0; i < neighbor.prop_मोजणी; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	जर (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    जर (!neighbor.detached && !um_detach(&neighbor))
		थांबा;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    थांबा;
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
um_delete_prop(unpacked_memline_T *um, पूर्णांक index)
{
    textprop_T *prop;

    जर (um->buf == रिक्त || index < 0 || index >= um->prop_मोजणी)
	परत;
    जर (!um->detached && !um_detach(um))
	परत;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    जर (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    जर (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    जर (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
um_pack(unpacked_memline_T *um, पूर्णांक *packed_len)
{
    uint16_t	live_मोजणी = 0;
    पूर्णांक		vtext_size = 0;
    पूर्णांक		total_len;
    char_u	*buf;
    char_u	*मोजणी_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    साठी (पूर्णांक i = 0; i < um->prop_मोजणी; ++i)
    {
	textprop_T *prop = &um->props[i];

	जर (prop->tp_flags & TP_FLAG_DELETED)
	    सुरूठेवा;
	++live_मोजणी;
	जर (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    जर (live_मोजणी == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	जर (buf == रिक्त)
	    परत रिक्त;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	परत buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (पूर्णांक)PROP_मोजणी_SIZE
		+ live_मोजणी * (पूर्णांक)आकार(textprop_T) + vtext_size;
    buf = alloc(total_len);
    जर (buf == रिक्त)
	परत रिक्त;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    मोजणी_dest = buf + um->text_size;
    mch_memmove(मोजणी_dest, &live_मोजणी, PROP_मोजणी_SIZE);

    prop_dest = मोजणी_dest + PROP_मोजणी_SIZE;
    vtext_dest = prop_dest + live_मोजणी * आकार(textprop_T);

    साठी (पूर्णांक i = 0; i < um->prop_मोजणी; ++i)
    {
	textprop_T prop = um->props[i];

	जर (prop.tp_flags & TP_FLAG_DELETED)
	    सुरूठेवा;

	जर (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    पूर्णांक copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - मोजणी_dest);
	    vtext_dest += copysize;
	}
	अन्यथा
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, आकार(textprop_T));
	prop_dest += आकार(textprop_T);
    }

    परत buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    स्थिर शून्य
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    पूर्णांक	    packed_len;

    जर (!um->detached || um->buf == रिक्त || um->lnum == 0)
	परत;

    packed = um_pack(um, &packed_len);
    जर (packed == रिक्त)
    {
	um->buf = रिक्त;
	परत;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    जर (um->buf->b_ml.ml_ओळ_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    अन्यथा जर (!(um->buf->b_ml.ml_flags & ML_ओळ_DIRTY))
	(शून्य)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_मोजणी);

    जर (um->buf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED))
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
    जर (um->buf == रिक्त)
	परत;
    जर (um->detached && um->lnum > 0)
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
#define HIKEY2PT(p)   ((proptype_T *)((p) - दुरत्व(proptype_T, pt_name)))
#define HI2PT(hi)      HIKEY2PT((hi)->hi_किल्ली)

// The global text property types.
स्थिर hashtab_T *global_proptypes = रिक्त;
स्थिर proptype_T **global_proparray = रिक्त;

// The last used text property type ID.
स्थिर पूर्णांक proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    स्थिर hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    जर (*name == NUL)
	परत रिक्त;
    जर (buf == रिक्त)
	ht = global_proptypes;
    अन्यथा
	ht = buf->b_proptypes;

    जर (ht == रिक्त)
	परत रिक्त;
    hi = hash_find(ht, name);
    जर (HASHITEM_EMPTY(hi))
	परत रिक्त;
    परत hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    स्थिर proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    जर (hi == रिक्त)
	परत रिक्त;
    परत HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    पूर्णांक
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    जर (pt == रिक्त)
	परत 0;
    परत pt->pt_id;
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

    जर (type == रिक्त)
	type = find_prop_type(name, रिक्त);
    जर (type == रिक्त)
	semsg(_(e_property_type_str_does_not_exist), name);
    परत type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    स्थिर पूर्णांक
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    जर (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	परत FAIL;
    }
    जर (arg->vval.v_dict == रिक्त)
	परत OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    जर (di != रिक्त && (di->di_tv.v_type != VAR_संख्या
					      || di->di_tv.vval.v_संख्या != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	जर (*buf == रिक्त)
	    परत FAIL;
    }
    परत OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    शून्य
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    जर (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	परत;

    start_lnum = tv_get_संख्या(&argvars[0]);
    start_col = tv_get_संख्या(&argvars[1]);
    जर (check_for_dict_arg(argvars, 2) == FAIL)
	परत;

    rettv->vval.v_संख्या = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    स्थिर पूर्णांक
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	पूर्णांक		id,
	char_u		*text_arg,
	पूर्णांक		text_padding_डावा,
	पूर्णांक		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    पूर्णांक		proplen;
    char_u	*props = रिक्त;
    char_u	*newprops;
    माप_प्रकार	textlen;
    char_u	*newtext;
    पूर्णांक		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    पूर्णांक		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    जर (type == रिक्त)
	जा theend;

    जर (start_lnum < 1 || start_lnum > buf->b_ml.ml_ओळ_मोजणी)
    {
	semsg(_(e_invalid_ओळ_संख्या_nr), (दीर्घ)start_lnum);
	जा theend;
    }
    जर (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_ओळ_मोजणी)
    {
	semsg(_(e_invalid_ओळ_संख्या_nr), (दीर्घ)end_lnum);
	जा theend;
    }

    जर (buf->b_ml.ml_mfp == रिक्त)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	जा theend;
    }

    जर (text != रिक्त)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	साठी (p = text; *p != NUL; MB_PTR_ADV(p))
	    जर (*p < ' ')
		*p = ' ';
    }

    साठी (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	दीर्घ	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	जर (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_ओळ));
	    जा theend;
	}

	जर (lnum == start_lnum)
	    col = start_col;
	अन्यथा
	    col = 1;
	जर (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != रिक्त))
	{
	    semsg(_(e_invalid_column_संख्या_nr), (दीर्घ)start_col);
	    जा theend;
	}
	sort_col = col;

	जर (lnum == end_lnum)
	    length = end_col - col;
	अन्यथा
	    length = (पूर्णांक)textlen - col + 1;
	जर (length > (दीर्घ)textlen)
	    length = (पूर्णांक)textlen;	// can include the end-of-line
	जर (length < 0)
	    length = 0;		// zero-width property

	जर (text_arg != रिक्त)
	{
	    length = (दीर्घ)STRLEN(text_arg);
	    जर (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		जर ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	जर (text_arg != रिक्त)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	साठी (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * आकार(textprop_T),
							   आकार(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    जर (prop_col >= sort_col)
		थांबा;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	पूर्णांक		vtext_total = 0;
	पूर्णांक		new_ओळ_len;
	char_u	*मोजणी_dest;
	char_u	*vtext_dest;
	पूर्णांक		j;

	// Compute total vtext size from existing props.
	साठी (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * आकार(textprop_T),
							आकार(textprop_T));
	    जर (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	जर (text_arg != रिक्त)
	    vtext_total += length + 1;

	new_ओळ_len = (पूर्णांक)textlen + (पूर्णांक)PROP_मोजणी_SIZE
			+ new_propcount * (पूर्णांक)आकार(textprop_T)
			+ vtext_total;
	newtext = alloc(new_ओळ_len);
	जर (newtext == रिक्त)
	    जा theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_ओळ_ptr, textlen);

	// Write prop_count.
	मोजणी_dest = newtext + textlen;
	mch_memmove(मोजणी_dest, &new_propcount, PROP_मोजणी_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = मोजणी_dest + PROP_मोजणी_SIZE;
	जर (i > 0)
	    mch_memmove(newprops, props, आकार(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	जर (i < proplen)
	    mch_memmove(newprops + (i + 1) * आकार(textprop_T),
					props + i * आकार(textprop_T),
					आकार(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * आकार(textprop_T);
	साठी (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    जर (j == i)
		सुरूठेवा;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * आकार(textprop_T),
		    आकार(textprop_T));
	    जर (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_मोजणी_2 = (char_u *)props - PROP_मोजणी_SIZE;
		char_u *old_vtext = old_मोजणी_2 + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - मोजणी_dest);
		mch_memmove(newprops + j * आकार(textprop_T), &ep,
			आकार(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	जर (text_arg != रिक्त)
	{
	    पूर्णांक copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - मोजणी_dest);
	    vtext_dest += copysize;
	}
	अन्यथा
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * आकार(textprop_T), &tmp_prop,
							आकार(textprop_T));

	जर (buf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED))
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
    परत res;
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
    पूर्णांक		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    पूर्णांक		error = FALSE;
    पूर्णांक		prev_did_emsg = did_emsg;

    जर (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	परत;

    जर (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	परत;

    dict = argvars[0].vval.v_dict;
    जर (dict == रिक्त || !dict_has_किल्ली(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	परत;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    जर (dict_has_किल्ली(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_संख्या(dict, "id");
	जर (x > पूर्णांक_उच्च || x  <= पूर्णांक_न्यून)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    परत;
	}
	id = (पूर्णांक)x;
    }

    जर (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	परत;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_वस्तुसंच(argvars[1].vval.v_list, li)
    {
	जर (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == रिक्त)
	{
	    emsg(_(e_list_required));
	    परत;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	जर (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	जर (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	जर (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	पूर्णांक this_id = id;
	जर (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	जर (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    जर (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    परत;
	}
	जर (prop_add_one(buf, type_name, this_id, रिक्त, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    परत;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
स्थिर पूर्णांक vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    स्थिर पूर्णांक
get_textprop_id(शून्य)
{
    जर (vt_id_counter > 0)
	vt_id_counter = 0;
    परत --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    पूर्णांक
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
    पूर्णांक		id = 0;
    char_u	*text = रिक्त;
    पूर्णांक		text_padding_डावा = 0;
    पूर्णांक		flags = 0;

    जर (dict == रिक्त || !dict_has_किल्ली(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	जा theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    जर (dict_has_किल्ली(dict, "end_lnum"))
    {
	end_lnum = dict_get_संख्या(dict, "end_lnum");
	जर (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "end_lnum");
	    जा theend;
	}
    }
    अन्यथा
	end_lnum = start_lnum;

    जर (dict_has_किल्ली(dict, "length"))
    {
	दीर्घ length = dict_get_संख्या(dict, "length");

	जर (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "length");
	    जा theend;
	}
	end_col = start_col + length;
    }
    अन्यथा जर (dict_has_किल्ली(dict, "end_col"))
    {
	end_col = dict_get_संख्या(dict, "end_col");
	जर (end_col <= 0)
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "end_col");
	    जा theend;
	}
    }
    अन्यथा जर (start_lnum == end_lnum)
	end_col = start_col;
    अन्यथा
	end_col = 1;

    जर (dict_has_किल्ली(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_संख्या(dict, "id");
	जर (x > पूर्णांक_उच्च || x  <= पूर्णांक_न्यून)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    जा theend;
	}
	id = (पूर्णांक)x;
    }

    जर (dict_has_किल्ली(dict, "text"))
    {
	जर (dict_has_किल्ली(dict, "length")
		|| dict_has_किल्ली(dict, "end_col")
		|| dict_has_किल्ली(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    जा theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	जर (text == रिक्त)
	    जा theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	जर (dict_has_किल्ली(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    जर (p == रिक्त)
		जा theend;
	    जर (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		जा theend;
	    }
	    जर (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_उजवा;
	    अन्यथा जर (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    अन्यथा जर (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    अन्यथा जर (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str_str), "text_align", p);
		जा theend;
	    }
	}

	जर (dict_has_किल्ली(dict, "text_padding_left"))
	{
	    text_padding_डावा = dict_get_संख्या(dict, "text_padding_left");
	    जर (text_padding_डावा < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		जा theend;
	    }
	}

	जर (dict_has_किल्ली(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    जर (p == रिक्त)
		जा theend;
	    जर (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    अन्यथा जर (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str_str), "text_wrap", p);
		जा theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    जर (start_col < (text == रिक्त ? 1 : 0))
    {
	semsg(_(e_invalid_column_संख्या_nr), (दीर्घ)start_col);
	जा theend;
    }
    जर (start_col > 0 && text_padding_डावा > 0)
    {
	emsg(_(e_can_only_use_डावा_padding_when_column_is_zero));
	जा theend;
    }

    जर (dict_arg != रिक्त && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	जा theend;

    जर (text != रिक्त)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    अन्यथा जर (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	जा theend;
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
    परत id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    पूर्णांक
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, पूर्णांक will_change)
{
    char_u	*text;
    माप_प्रकार	textlen;
    माप_प्रकार	propdata_len;
    uint16_t	prop_मोजणी;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    जर ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == रिक्त)
	परत 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_ओळ_len - textlen;
    जर (propdata_len == 0)
	परत 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    जर (propdata_len < PROP_मोजणी_SIZE + आकार(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	परत 0;
    }
    mch_memmove(&prop_मोजणी, text + textlen, PROP_मोजणी_SIZE);
    जर (!text_prop_मोजणी_valid(prop_मोजणी, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	परत 0;
    }
    जर (!text_prop_vtext_valid(text + textlen + PROP_मोजणी_SIZE,
					   (पूर्णांक)prop_मोजणी, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	परत 0;
    }
    *props = text + textlen + PROP_मोजणी_SIZE;
    परत (पूर्णांक)prop_मोजणी;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    पूर्णांक
prop_मोजणी_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    पूर्णांक		मोजणी = get_text_props(buf, lnum, &props, FALSE);
    पूर्णांक		result = 0;
    textprop_T	prop;
    पूर्णांक		i;
    पूर्णांक		next_उजवा_goes_below = FALSE;

    जर (मोजणी == 0)
	परत 0;
    साठी (i = 0; i < मोजणी; ++i)
    {
	mch_memmove(&prop, props + i * आकार(prop), आकार(prop));
	जर (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    जर ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_उजवा_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_उजवा)))
	    {
		++result;
	    }
	    अन्यथा जर (prop.tp_flags & TP_FLAG_ALIGN_उजवा)
		next_उजवा_goes_below = TRUE;
	}
    }
    परत result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    पूर्णांक
मोजणी_props(linenr_T lnum, पूर्णांक only_starting, पूर्णांक last_ओळ)
{
    char_u	*props;
    पूर्णांक		proplen = get_text_props(curbuf, lnum, &props, 0);
    पूर्णांक		result = proplen;
    textprop_T	prop;

    साठी (पूर्णांक i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * आकार(prop), आकार(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	जर ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_ओळ && prop.tp_col == MAXCOL))
	    --result;
    }
    परत result;
}

स्थिर textprop_T	*text_prop_compare_props;
स्थिर buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    स्थिर पूर्णांक
text_prop_order(पूर्णांक flags)
{
    जर (flags & TP_FLAG_ALIGN_ABOVE)
	परत 0;
    जर (flags & TP_FLAG_ALIGN_उजवा)
	परत 2;
    जर (flags & TP_FLAG_ALIGN_BELOW)
	परत 3;
    परत 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    स्थिर पूर्णांक
text_prop_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2)
{
    पूर्णांक  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(पूर्णांक *)s1;
    idx2 = *(पूर्णांक *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    जर ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	परत tp1->tp_id < 0 ? 1 : -1;

    जर (col1 == MAXCOL || col2 == MAXCOL)
    {
	पूर्णांक order1 = text_prop_order(tp1->tp_flags);
	पूर्णांक order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	जर (order1 != order2)
	    परत order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    जर (pt1 != pt2)
    {
	जर (pt1 == रिक्त)
	    परत -1;
	जर (pt2 == रिक्त)
	    परत 1;
	जर (pt1->pt_priority != pt2->pt_priority)
	    परत pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    जर (col1 != col2)
	परत col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    जर (tp1->tp_id < 0)
	परत tp1->tp_id > tp2->tp_id ? 1 : -1;

    परत 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    शून्य
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	पूर्णांक	    *idxs,
	पूर्णांक	    मोजणी)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    क्विक((शून्य *)idxs, (माप_प्रकार)मोजणी, आकार(पूर्णांक), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_ओळी(
	win_T	    *wp,
	पूर्णांक	    type_id,
	पूर्णांक	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    जर (text_prop_type_by_id(wp->w_buffer, type_id) == रिक्त)
	परत false;

    जर (first_lnum < 1)
	first_lnum = 1;
    जर (last_lnum > wp->w_buffer->b_ml.ml_ओळ_मोजणी)
	last_lnum = wp->w_buffer->b_ml.ml_ओळ_मोजणी;

    साठी (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	पूर्णांक	मोजणी = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	साठी (पूर्णांक i = 0; i < मोजणी; ++i)
	{
	    mch_memmove(prop, props + i * आकार(textprop_T),
							   आकार(textprop_T));
	    जर (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		परत true;
	    }
	}
    }
    परत false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	पूर्णांक	    type_id,
	पूर्णांक	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    परत find_prop_in_ओळी(wp, type_id, id, prop, found_lnum,
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
set_text_props(linenr_T lnum, textprop_T *tps, पूर्णांक मोजणी)
{
    char_u  *text;
    char_u  *newtext;
    पूर्णांक	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    जर (मोजणी == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	जर (newtext == रिक्त)
	    परत;
	mch_memmove(newtext, text, textlen);
	जर (curbuf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ओळ_ptr);
	curbuf->b_ml.ml_ओळ_ptr = newtext;
	curbuf->b_ml.ml_ओळ_len = textlen;
	curbuf->b_ml.ml_flags |= ML_ओळ_DIRTY;
    }
    अन्यथा
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_मोजणी = (uint16_t)मोजणी;
	पूर्णांक	    vtext_size = 0;
	पूर्णांक	    total_len;
	char_u	    *मोजणी_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	पूर्णांक	    i;

	साठी (i = 0; i < मोजणी; ++i)
	    जर (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (पूर्णांक)PROP_मोजणी_SIZE
		    + मोजणी * (पूर्णांक)आकार(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	जर (newtext == रिक्त)
	    परत;
	mch_memmove(newtext, text, textlen);

	मोजणी_dest = newtext + textlen;
	mch_memmove(मोजणी_dest, &prop_मोजणी, PROP_मोजणी_SIZE);

	prop_dest = मोजणी_dest + PROP_मोजणी_SIZE;
	vtext_dest = prop_dest + मोजणी * आकार(textprop_T);

	साठी (i = 0; i < मोजणी; ++i)
	{
	    textprop_T prop = tps[i];

	    जर (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		पूर्णांक copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = रिक्त;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - मोजणी_dest);
		vtext_dest += copysize;
	    }
	    अन्यथा
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, आकार(textprop_T));
	    prop_dest += आकार(textprop_T);
	}

	जर (curbuf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED))
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
props_add_मोजणी_header(char_u *ओळ, पूर्णांक ओळ_len, पूर्णांक textlen, पूर्णांक *new_len)
{
    पूर्णांक		prop_bytes = ओळ_len - textlen;
    uint16_t	prop_मोजणी;
    पूर्णांक		vtext_size = 0;
    पूर्णांक		total;
    char_u	*newline;
    char_u	*मोजणी_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    पूर्णांक		i;

    जर (prop_bytes <= 0 || prop_bytes % आकार(textprop_T) != 0)
    {
	*new_len = ओळ_len;
	परत रिक्त;
    }
    prop_मोजणी = (uint16_t)(prop_bytes / आकार(textprop_T));

    // Calculate total vtext size.
    साठी (i = 0; i < (पूर्णांक)prop_मोजणी; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ओळ + textlen + i * आकार(textprop_T),
							आकार(textprop_T));
	जर (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (पूर्णांक)PROP_मोजणी_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    जर (newline == रिक्त)
	परत रिक्त;
    *new_len = total;

    mch_memmove(newline, ओळ, textlen);
    मोजणी_dest = newline + textlen;
    mch_memmove(मोजणी_dest, &prop_मोजणी, PROP_मोजणी_SIZE);
    prop_dest = मोजणी_dest + PROP_मोजणी_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    साठी (i = 0; i < (पूर्णांक)prop_मोजणी; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ओळ + textlen + i * आकार(textprop_T),
							आकार(textprop_T));
	जर (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    पूर्णांक copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - मोजणी_dest);
	    vtext_dest += copysize;
	}
	अन्यथा
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * आकार(textprop_T), &prop,
							आकार(textprop_T));
    }
    परत newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    शून्य
add_text_props(linenr_T lnum, textprop_T *text_props, पूर्णांक text_prop_मोजणी)
{
    unpacked_memline_T	um;
    पूर्णांक			i;

    um = um_open_at(curbuf, lnum, text_prop_मोजणी);
    जर (um.buf == रिक्त)
	परत;
    जर (!um_detach(&um))
    {
	um_abort(&um);
	परत;
    }

    // Grow props array if needed.
    जर (um.prop_मोजणी + text_prop_मोजणी > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_मोजणी + text_prop_मोजणी) * आकार(textprop_T));
	जर (newprops == रिक्त)
	{
	    um_abort(&um);
	    परत;
	}
	um.props = newprops;
	um.prop_size = um.prop_मोजणी + text_prop_मोजणी;
    }

    साठी (i = 0; i < text_prop_मोजणी; ++i)
	um.props[um.prop_मोजणी++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    स्थिर पूर्णांक
compare_pt(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    परत tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    स्थिर proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, पूर्णांक id)
{
    पूर्णांक खालचा = 0;
    पूर्णांक वरचा;

    जर (ht == रिक्त || ht->ht_used == 0)
	परत रिक्त;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    जर (*array == रिक्त)
    {
	दीर्घ	    todo;
	hashitem_T  *hi;
	पूर्णांक	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	जर (*array == रिक्त)
	    परत रिक्त;
	todo = (दीर्घ)ht->ht_used;
	FOR_ALL_HASHTAB_वस्तुसंच(ht, hi, todo)
	{
	    जर (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	क्विक((शून्य *)*array, ht->ht_used, आकार(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    वरचा = ht->ht_used;
    पर्यंत (वरचा > खालचा)
    {
	पूर्णांक m = (वरचा + खालचा) / 2;

	जर ((*array)[m]->pt_id == id)
	    परत (*array)[m];
	जर ((*array)[m]->pt_id > id)
	    वरचा = m;
	अन्यथा
	    खालचा = m + 1;
    }
    परत रिक्त;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    स्थिर शून्य
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    पूर्णांक buflocal = TRUE;
    पूर्णांक virtualtext_prop = prop->tp_id < 0;

    dict_add_संख्या(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    जर (!virtualtext_prop)
    {
	dict_add_संख्या(dict, "length", prop->tp_len);
	dict_add_संख्या(dict, "id", prop->tp_id);
    }
    dict_add_संख्या(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_संख्या(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    जर (pt == रिक्त)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    जर (pt != रिक्त)
	dict_add_string(dict, "type", pt->pt_name);

    जर (buflocal)
	dict_add_संख्या(dict, "type_bufnr", buf->b_fnum);
    अन्यथा
	dict_add_संख्या(dict, "type_bufnr", 0);
    जर (virtualtext_prop)
    {
	string_T    text_align = {रिक्त, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	जर (prop->tp_flags & TP_FLAG_ALIGN_उजवा)
	    STR_LITERAL_SET(text_align, "right");
	अन्यथा जर (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	अन्यथा जर (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	जर (text_align.string != रिक्त)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (पूर्णांक)text_align.length);

	// text_wrap
	जर (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	जर (prop->tp_padleft != 0)
	    dict_add_संख्या(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, पूर्णांक id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    जर (type == रिक्त)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    परत type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    परत text_prop_type_by_id(buf, prop->tp_type) != रिक्त;
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
    पूर्णांक	    did_clear = FALSE;

    जर (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	परत;

    start = tv_get_संख्या(&argvars[0]);
    end = start;
    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_संख्या(&argvars[1]);
	जर (argvars[2].v_type != VAR_UNKNOWN)
	{
	    जर (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		परत;
	}
    }
    जर (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	परत;
    }

    {
	unpacked_memline_T um = um_open(buf);

	साठी (lnum = start; lnum <= end; ++lnum)
	{
	    पूर्णांक idx;

	    जर (lnum > buf->b_ml.ml_ओळ_मोजणी)
		थांबा;
	    जर (!um_goto_ओळ(&um, lnum, 0))
		थांबा;
	    जर (um.prop_मोजणी > 0)
	    {
		did_clear = TRUE;
		साठी (idx = um.prop_मोजणी - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    जर (did_clear)
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
    पूर्णांक		lnum_start;
    पूर्णांक		start_pos_has_prop = 0;
    पूर्णांक		seen_end = FALSE;
    पूर्णांक		id = 0;
    पूर्णांक		id_found = FALSE;
    पूर्णांक		type_id = -1;
    पूर्णांक		skipstart = FALSE;
    पूर्णांक		lnum = -1;
    पूर्णांक		col = -1;
    पूर्णांक		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    पूर्णांक		both;

    जर (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	परत;

    जर (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	परत;
    dict = argvars[0].vval.v_dict;

    जर (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	परत;
    जर (buf->b_ml.ml_mfp == रिक्त)
	परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	जर (*dir_s == 'b')
	    dir = BACKWARD;
	अन्यथा जर (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    परत;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    जर (di != रिक्त)
	lnum = tv_get_संख्या(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    जर (di != रिक्त)
	col = tv_get_संख्या(&di->di_tv);

    जर (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    अन्यथा जर (col == -1)
	col = 1;

    जर (lnum < 1 || lnum > buf->b_ml.ml_ओळ_मोजणी)
    {
	emsg(_(e_invalid_range));
	परत;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    जर (dict_has_किल्ली(dict, "id"))
    {
	id = dict_get_संख्या(dict, "id");
	id_found = TRUE;
    }
    जर (dict_has_किल्ली(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	जर (type == रिक्त)
	    परत;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    जर (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	परत;
    }
    जर (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	परत;
    }

    lnum_start = lnum;

    जर (rettv_dict_alloc(rettv) == FAIL)
	परत;

    पर्यंत (1)
    {
	char_u	*prop_start_ptr;
	पूर्णांक	मोजणी = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	पूर्णांक	    i;
	textprop_T  prop;
	पूर्णांक	    prop_start;
	पूर्णांक	    prop_end;

	साठी (i = dir == BACKWARD ? मोजणी - 1 : 0; i >= 0 && i < मोजणी; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * आकार(textprop_T),
							   आकार(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    जर (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_मोजणी_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    अन्यथा
		prop.u.tp_text = रिक्त;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    जर (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		जर (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    जर (!is_floating_vtext && prop.tp_col > col)
			सुरूठेवा;
		}
		अन्यथा जर (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    सुरूठेवा;
	    }
	    जर (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		जर (lnum_start == lnum
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
		जर (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		जर (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    जर (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    सुरूठेवा;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		जर (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    सुरूठेवा;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_संख्या(rettv->vval.v_dict, "lnum", lnum);

		परत;
	    }
	}

	जर (dir > 0)
	{
	    जर (lnum >= buf->b_ml.ml_ओळ_मोजणी)
		थांबा;
	    lnum++;
	}
	अन्यथा
	{
	    जर (lnum <= 1)
		थांबा;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    स्थिर पूर्णांक
prop_type_or_id_in_list(पूर्णांक *types_or_ids, पूर्णांक len, पूर्णांक type_or_id)
{
    पूर्णांक i;

    साठी (i = 0; i < len; i++)
	जर (types_or_ids[i] == type_or_id)
	    परत TRUE;

    परत FALSE;
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
	पूर्णांक		*prop_types,
	पूर्णांक		prop_types_len,
	पूर्णांक		*prop_ids,
	पूर्णांक		prop_ids_len,
	list_T		*retlist,
	पूर्णांक		add_lnum)
{
    char_u	*props;
    पूर्णांक		मोजणी;
    पूर्णांक		i;
    textprop_T	prop;

    मोजणी = get_text_props(buf, lnum, &props, FALSE);
    साठी (i = 0; i < मोजणी; ++i)
    {
	mch_memmove(&prop, props + i * आकार(textprop_T),
		आकार(textprop_T));
	// Convert offset to pointer for virtual text props.
	जर (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_मोजणी_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	अन्यथा
	    prop.u.tp_text = रिक्त;
	जर ((prop_types == रिक्त
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == रिक्त
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    जर (d == रिक्त)
		थांबा;
	    prop_fill_dict(d, &prop, buf);
	    जर (add_lnum)
		dict_add_संख्या(d, "lnum", lnum);
	    जर (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    स्थिर पूर्णांक *
get_prop_types_from_names(list_T *l, buf_T *buf, पूर्णांक *अंक_types)
{
    पूर्णांक		*prop_types;
    listitem_T	*li;
    पूर्णांक		i;
    char_u	*name;
    proptype_T	*type;

    *अंक_types = 0;

    prop_types = ALLOC_MULT(पूर्णांक, list_len(l));
    जर (prop_types == रिक्त)
	परत रिक्त;

    i = 0;
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	जर (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    जा errret;
	}
	name = li->li_tv.vval.v_string;
	जर (name == रिक्त)
	    जा errret;

	type = lookup_prop_type(name, buf);
	जर (type == रिक्त)
	    जा errret;
	prop_types[i++] = type->pt_id;
    }

    *अंक_types = i;
    परत prop_types;

errret:
    VIM_CLEAR(prop_types);
    परत रिक्त;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    स्थिर पूर्णांक *
get_prop_ids_from_list(list_T *l, पूर्णांक *अंक_ids)
{
    पूर्णांक		*prop_ids;
    listitem_T	*li;
    पूर्णांक		i = 0;
    पूर्णांक		id;
    पूर्णांक		error;

    *अंक_ids = 0;

    prop_ids = ALLOC_MULT(पूर्णांक, list_len(l));
    जर (prop_ids == रिक्त)
	परत रिक्त;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	error = FALSE;
	id = tv_get_संख्या_chk(&li->li_tv, &error);
	जर (error)
	    जा errret;

	prop_ids[i++] = id;
    }

    *अंक_ids = i;
    परत prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    परत रिक्त;
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
    पूर्णांक		add_lnum = FALSE;
    पूर्णांक		*prop_types = रिक्त;
    पूर्णांक		prop_types_len = 0;
    पूर्णांक		*prop_ids = रिक्त;
    पूर्णांक		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    जर (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	परत;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    // default: get text properties on current line
    start_lnum = tv_get_संख्या(&argvars[0]);
    end_lnum = start_lnum;
    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	जर (check_for_dict_arg(argvars, 1) == FAIL)
	    परत;
	d = argvars[1].vval.v_dict;

	जर (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    परत;

	जर (d != रिक्त && (di = dict_find(d, (char_u *)"end_lnum", -1)) != रिक्त)
	{
	    जर (di->di_tv.v_type != VAR_संख्या)
	    {
		emsg(_(e_संख्या_required));
		परत;
	    }
	    end_lnum = tv_get_संख्या(&di->di_tv);
	    जर (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//ओळ
		end_lnum = buf->b_ml.ml_ओळ_मोजणी + end_lnum + 1;
	    अन्यथा जर (end_lnum > buf->b_ml.ml_ओळ_मोजणी)
		end_lnum = buf->b_ml.ml_ओळ_मोजणी;
	    add_lnum = TRUE;
	}
	जर (d != रिक्त && (di = dict_find(d, (char_u *)"types", -1)) != रिक्त)
	{
	    जर (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		परत;
	    }

	    l = di->di_tv.vval.v_list;
	    जर (l != रिक्त && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		जर (prop_types == रिक्त)
		    परत;
	    }
	}
	जर (d != रिक्त && (di = dict_find(d, (char_u *)"ids", -1)) != रिक्त)
	{
	    जर (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		जा errret;
	    }

	    l = di->di_tv.vval.v_list;
	    जर (l != रिक्त && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		जर (prop_ids == रिक्त)
		    जा errret;
	    }
	}
    }
    जर (start_lnum < 1 || start_lnum > buf->b_ml.ml_ओळ_मोजणी
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    अन्यथा
	साठी (lnum = start_lnum; lnum <= end_lnum; lnum++)
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
    पूर्णांक		do_all;
    पूर्णांक		id = -MAXCOL;
    पूर्णांक		type_id = -1;	    // for a single "type"
    पूर्णांक		*type_ids = रिक्त;   // array, for a list of "types", allocated
    पूर्णांक		अंक_type_ids = 0;   // number of elements in "type_ids"
    पूर्णांक		both;

    rettv->vval.v_संख्या = 0;

    जर (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 2) == FAIL)))
	परत;

    जर (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_संख्या(&argvars[1]);
	end = start;
	जर (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_संख्या(&argvars[2]);
	जर (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    परत;
	}
    }

    dict = argvars[0].vval.v_dict;
    जर (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	परत;
    जर (buf->b_ml.ml_mfp == रिक्त)
	परत;

    do_all = dict_get_bool(dict, "all", FALSE);

    जर (dict_has_किल्ली(dict, "id"))
	id = dict_get_संख्या(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    जर (dict_has_किल्ली(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	जर (type == रिक्त)
	    परत;
	type_id = type->pt_id;
    }
    जर (dict_has_किल्ली(dict, "types"))
    {
	typval_T types;
	listitem_T *li = रिक्त;

	dict_get_tv(dict, "types", &types);
	जर (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( आकार(पूर्णांक) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_वस्तुसंच(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		जर (li->li_tv.v_type != VAR_STRING)
		    सुरूठेवा;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		जर (!prop_type)
		    जा cleanup_prop_remove;

		type_ids[अंक_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    जर (id == -MAXCOL && (type_id == -1 && अंक_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	जा cleanup_prop_remove;
    }
    जर (both && (id == -MAXCOL || (type_id == -1 && अंक_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	जा cleanup_prop_remove;
    }
    जर (type_id != -1 && अंक_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	जा cleanup_prop_remove;
    }

    जर (end == 0)
	end = buf->b_ml.ml_ओळ_मोजणी;

    {
	unpacked_memline_T um = um_open(buf);

	साठी (lnum = start; lnum <= end; ++lnum)
	{
	    पूर्णांक idx;

	    जर (lnum > buf->b_ml.ml_ओळ_मोजणी)
		थांबा;
	    जर (!um_goto_ओळ(&um, lnum, 0))
		थांबा;

	    साठी (idx = 0; idx < um.prop_मोजणी; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		पूर्णांक	    matches_id = 0;
		पूर्णांक	    matches_type = 0;

		जर (prop->tp_flags & TP_FLAG_DELETED)
		    सुरूठेवा;

		matches_id = prop->tp_id == id;
		जर (अंक_type_ids > 0)
		{
		    पूर्णांक idx2;

		    साठी (idx2 = 0; !matches_type && idx2 < अंक_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		अन्यथा
		    matches_type = prop->tp_type == type_id;

		जर (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    जर (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_संख्या;
		    जर (!do_all)
			थांबा;
		}
	    }
	}
	um_close(&um);
    }

    जर (first_changed > 0)
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
prop_type_set(typval_T *argvars, पूर्णांक add)
{
    char_u	*name;
    buf_T	*buf = रिक्त;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	परत;

    name = tv_get_string(&argvars[0]);
    जर (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	परत;
    }

    जर (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	परत;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    जर (add)
    {
	hashtab_T **htp;

	जर (prop != रिक्त)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    परत;
	}
	prop = alloc_clear(दुरत्व(proptype_T, pt_name) + STRLEN(name) + 1);
	जर (prop == रिक्त)
	    परत;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	जर (buf == रिक्त)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	अन्यथा
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	जर (*htp == रिक्त)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    जर (*htp == रिक्त)
	    {
		vim_free(prop);
		परत;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    अन्यथा
    {
	जर (prop == रिक्त)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    परत;
	}
    }

    जर (dict != रिक्त)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	जर (di != रिक्त)
	{
	    char_u	*highlight;
	    पूर्णांक		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    जर (highlight != रिक्त && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    जर (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == रिक्त ? (char_u *)"" : highlight);
		परत;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	जर (di != रिक्त)
	{
	    जर (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    अन्यथा
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	जर (di != रिक्त)
	{
	    जर (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    अन्यथा
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	जर (di != रिक्त)
	    prop->pt_priority = tv_get_संख्या(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	जर (di != रिक्त)
	{
	    जर (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    अन्यथा
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	जर (di != रिक्त)
	{
	    जर (tv_get_bool(&di->di_tv))
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

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	परत;

    name = tv_get_string(&argvars[0]);
    जर (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	परत;
    }

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	जर (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    परत;
    }

    hi = find_prop_type_hi(name, buf);
    जर (hi == रिक्त)
	परत;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    जर (buf == रिक्त)
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

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	परत;

    name = tv_get_string(&argvars[0]);
    जर (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	परत;
    }

    जर (rettv_dict_alloc(rettv) == FAIL)
	परत;

    proptype_T  *prop = रिक्त;
    buf_T	    *buf = रिक्त;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	जर (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    परत;
    }

    prop = find_prop_type(name, buf);
    जर (prop == रिक्त)
	परत;

    dict_T *d = rettv->vval.v_dict;

    जर (prop->pt_hl_id > 0)
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
    जर (buf != रिक्त)
	dict_add_संख्या(d, "bufnr", buf->b_fnum);
}

    स्थिर शून्य
list_types(hashtab_T *ht, list_T *l)
{
    दीर्घ	todo;
    hashitem_T	*hi;

    todo = (दीर्घ)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तुसंच(ht, hi, todo)
    {
	जर (!HASHITEM_EMPTY(hi))
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

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	परत;

    जर (argvars[0].v_type != VAR_UNKNOWN)
    {
	जर (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    परत;
    }
    जर (buf == रिक्त)
    {
	जर (global_proptypes != रिक्त)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    अन्यथा जर (buf->b_proptypes != रिक्त)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    स्थिर शून्य
clear_ht_prop_types(hashtab_T *ht)
{
    दीर्घ	todo;
    hashitem_T	*hi;

    जर (ht == रिक्त)
	परत;

    todo = (दीर्घ)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तुसंच(ht, hi, todo)
    {
	जर (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#जर defined(EXITFREE)
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
प्रकारव्याख्या संरचना
{
    पूर्णांक dirty;	    // if the property was changed
    पूर्णांक can_drop;   // whether after this change, the prop may be removed
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
	पूर्णांक	    added,
	पूर्णांक	    flags)
{
    proptype_T	*pt;
    पूर्णांक		start_incl;
    पूर्णांक		end_incl;
    पूर्णांक		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    जर (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	परत res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != रिक्त && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    जर (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != रिक्त && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    जर (added > 0)
    {
	जर (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	अन्यथा जर (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    अन्यथा जर (prop->tp_col > col + 1)
    {
	जर (prop->tp_col + added < col + 1)
	{
	    जर (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    अन्यथा
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		जर (prop->tp_len <= 0)
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
    अन्यथा जर (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	जर (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	अन्यथा
	{
	    पूर्णांक after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	जर (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    अन्यथा
	res.dirty = FALSE;

    परत res;
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
    पूर्णांक
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	पूर्णांक	    bytes_added,
	पूर्णांक	    flags)
{
    unpacked_memline_T um;
    पूर्णांक		dirty = FALSE;
    पूर्णांक		ri;

    जर (text_prop_frozen > 0)
	परत FALSE;

    um = um_open_at(curbuf, lnum, 0);
    जर (um.buf == रिक्त || um.prop_मोजणी == 0)
    {
	um_abort(&um);
	परत FALSE;
    }

    साठी (ri = 0; ri < um.prop_मोजणी; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	जर (res.dirty)
	{
	    जर (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		जर (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    परत FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		जर ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    परत FALSE;
		}
	    }
	    dirty = TRUE;
	}
	जर (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    सुरूठेवा;
	}
    }
    जर (dirty)
	um_close(&um);
    अन्यथा
	um_abort(&um);
    परत dirty;
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
	पूर्णांक	    kept,
	पूर्णांक	    deleted,
	पूर्णांक	    at_eol)
{
    char_u	*props;
    पूर्णांक		मोजणी;
    garray_T    prevprop;
    garray_T    nextprop;
    पूर्णांक		skipped = kept + deleted;

    जर (!curbuf->b_has_textprop)
	परत;

    // Get the text properties from "lnum_props".
    मोजणी = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, आकार(textprop_T), 10);
    ga_init2(&nextprop, आकार(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *मोजणी_ptr = props - PROP_मोजणी_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    साठी (पूर्णांक i = 0; i < मोजणी; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	पूर्णांक	    start_incl, end_incl;
	पूर्णांक	    cont_prev, cont_next;
	पूर्णांक	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * आकार(textprop_T), आकार(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	जर (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
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

	जर (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	अन्यथा
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    पूर्णांक is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    जर (is_floating_vtext)
		cont_next = FALSE;
	    अन्यथा
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	जर (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	जर (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    पूर्णांक is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    जर (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    जर (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	जर (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    पूर्णांक is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    जर (p->tp_col != MAXCOL)
	    {
		जर (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		अन्यथा
		{
		    जर (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    जर (cont_prev)
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
	पूर्णांक		    last_ओळ,
	दीर्घ		    col,
	पूर्णांक		    removed)
{
    unpacked_memline_T	r_um;

    जर (um->buf == रिक्त)
	परत;

    r_um = um_open_at(um->buf, lnum, 0);
    जर (r_um.buf == रिक्त)
	परत;
    जर (!um_detach(&r_um))
    {
	um_abort(&r_um);
	परत;
    }

    साठी (पूर्णांक i = r_um.prop_मोजणी - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	पूर्णांक	    end;

	जर (prop->tp_col == MAXCOL && !last_ओळ)
	    सुरूठेवा;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	जर (last_ओळ || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = रिक्त;  // ownership transferred
	}
	अन्यथा
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    साठी (पूर्णांक j = 0; j < um->prop_मोजणी; ++j)
	    {
		textprop_T *op = &um->props[j];

		जर ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    थांबा;
		}
	    }
	    जर (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    स्थिर शून्य
remember_cleared_prop(garray_T *gap, पूर्णांक which, textprop_T *prop, पूर्णांक flag)
{
    undoprop_T	*up;

    जर (which == UNDOPROP_NONE)
	परत;
    जर (gap->ga_itemsize == 0)
	ga_init2(gap, आकार(undoprop_T), 4);
    जर (ga_grow(gap, 1) == FAIL)
	परत;
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
	पूर्णांक	    above_which,
	पूर्णांक	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = रिक्त;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    जर (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_ओळ_मोजणी)
	परत;

    above = um_open_at(buf, lnum, 0);
    जर (above.buf == रिक्त)
	परत;
    साठी (पूर्णांक i = 0; i < above.prop_मोजणी; ++i)
	जर (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    जर (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	परत;
    }

    below = um_open_at(buf, lnum + 1, 0);
    जर (below.buf == रिक्त)
    {
	um_abort(&above);
	परत;
    }
    जर (below.prop_मोजणी > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_मोजणी);
	जर (paired == रिक्त)
	    जा theend;
    }

    साठी (पूर्णांक i = 0; i < above.prop_मोजणी; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	जर (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    सुरूठेवा;
	साठी (पूर्णांक j = 0; j < below.prop_मोजणी; ++j)
	{
	    textprop_T *next = &below.props[j];

	    जर (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		थांबा;
	    }
	}
	जर (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    साठी (पूर्णांक j = 0; j < below.prop_मोजणी; ++j)
    {
	जर (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    सुरूठेवा;
	जर (!below.detached && !um_detach(&below))
	    थांबा;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    जर (below_changed)
	um_close(&below);
    अन्यथा
	um_abort(&below);
    जर (above_changed)
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
	पूर्णांक	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    जर (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_ओळ_मोजणी)
	परत;
    um = um_open_at(buf, lnum, 0);
    जर (um.buf == रिक्त)
	परत;
    साठी (पूर्णांक r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	जर (up->up_which != which)
	    सुरूठेवा;
	साठी (पूर्णांक i = 0; i < um.prop_मोजणी; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    जर (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		सुरूठेवा;
	    जर (!um.detached && !um_detach(&um))
		जा theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    थांबा;
	}
    }

theend:
    जर (changed)
	um_close(&um);
    अन्यथा
	um_abort(&um);
}

    bool
text_prop_मोजणी_valid(पूर्णांक prop_मोजणी, माप_प्रकार propdata_len)
{
    जर (propdata_len < PROP_मोजणी_SIZE)
	परत false;
    परत (माप_प्रकार)prop_मोजणी * आकार(textprop_T)
		    <= propdata_len - PROP_मोजणी_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, पूर्णांक prop_मोजणी, माप_प्रकार propdata_len)
{
    साठी (पूर्णांक i = 0; i < prop_मोजणी; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (माप_प्रकार)i * आकार(textprop_T),
							  आकार(textprop_T));
	जर (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    सुरूठेवा;
	जर (prop.tp_len < 0 || (माप_प्रकार)prop.u.tp_text_offset
				+ (माप_प्रकार)prop.tp_len + 1 > propdata_len)
	    परत false;
    }
    परत true;
}

#endif // FEAT_PROP_POPUP
