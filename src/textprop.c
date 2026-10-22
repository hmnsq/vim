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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_PROP_POPUP)

স্থির শূন্য um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    স্থির শূন্য
um_free_vtext(textprop_T *props, পূর্ণাঙ্ক গুনতি)
{
    বাবে (পূর্ণাঙ্ক i = 0; i < গুনতি; ++i)
	যদি (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    স্থির শূন্য
um_free(unpacked_memline_T *um)
{
    যদি (um->detached)
    {
	VIM_CLEAR(um->text);
	um_free_vtext(um->props, um->prop_গুনতি);
    }
    VIM_CLEAR(um->props);
    um->prop_size = 0;
    um->prop_গুনতি = 0;
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
    ঘূৰাই um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_সাৰি(unpacked_memline_T *um, linenr_T lnum, পূর্ণাঙ্ক extra_props)
{
    char_u	*সাৰি;
    size_t	textlen;
    size_t	propdata_len;
    পূর্ণাঙ্ক		proplen;

    যদি (um->buf == রিক্ত)
	ঘূৰাই false;

    // Store changes to the current line if any.
    যদি (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    যদি (lnum == 0)
	ঘূৰাই true;	// just unload

    সাৰি = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_সাৰি_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = সাৰি;

    যদি (propdata_len == 0)
	ঘূৰাই true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    যদি (propdata_len < PROP_গুনতি_SIZE + পরিমাণ(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = রিক্ত;
	ঘূৰাই false;
    }

    uint16_t    prop_গুনতি;
    char_u	    *গুনতি_ptr = সাৰি + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_গুনতি, গুনতি_ptr, PROP_গুনতি_SIZE);
    যদি (!text_prop_গুনতি_valid(prop_গুনতি, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = রিক্ত;
	ঘূৰাই false;
    }
    proplen = (পূর্ণাঙ্ক)prop_গুনতি;
    props_start = গুনতি_ptr + PROP_গুনতি_SIZE;

    যদি (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = রিক্ত;
	ঘূৰাই false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    যদি (um->props == রিক্ত)
    {
	um->buf = রিক্ত;
	ঘূৰাই false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_গুনতি = proplen;
    mch_memmove(um->props, props_start, proplen * পরিমাণ(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    বাবে (পূর্ণাঙ্ক i = 0; i < proplen; ++i)
    {
	যদি (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = গুনতি_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	অন্যথা
	    um->props[i].u.tp_text = রিক্ত;
    }

    ঘূৰাই true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, পূর্ণাঙ্ক extra_props)
{
    unpacked_memline_T um = um_open(buf);

    যদি (!um_goto_সাৰি(&um, lnum, extra_props))
	um.buf = রিক্ত;
    ঘূৰাই um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    স্থির bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    যদি (um->detached || um->buf == রিক্ত)
	ঘূৰাই um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    যদি (newtext == রিক্ত)
    {
	um->buf = রিক্ত;
	ঘূৰাই false;
    }
    um->text = newtext;

    বাবে (পূর্ণাঙ্ক i = 0; i < um->prop_গুনতি; ++i)
    {
	যদি (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    যদি (copy == রিক্ত)
	    {
		বাবে (পূর্ণাঙ্ক j = 0; j < i; ++j)
		    যদি (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = রিক্ত;
		ঘূৰাই false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    ঘূৰাই true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    যদি (um->buf == রিক্ত)
	ঘূৰাই false;
    যদি (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    ঘূৰাই true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    স্থির bool
um_grow_props(unpacked_memline_T *um, পূর্ণাঙ্ক needed)
{
    যদি (um->prop_গুনতি + needed <= um->prop_size)
	ঘূৰাই true;

    পূর্ণাঙ্ক	    new_size = um->prop_গুনতি + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * পরিমাণ(textprop_T));
    যদি (newprops == রিক্ত)
	ঘূৰাই false;
    um->props = newprops;
    um->prop_size = new_size;

    ঘূৰাই true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    স্থির শূন্য
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    যদি (um->buf == রিক্ত)
	ঘূৰাই;
    যদি (!um->detached && !um_detach(um))
	ঘূৰাই;
    যদি (!um_grow_props(um, 1))
    {
	um->buf = রিক্ত;
	ঘূৰাই;
    }
    um->props[um->prop_গুনতি++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    শূন্য
um_reverse_props(unpacked_memline_T *um)
{
    বাবে (পূর্ণাঙ্ক i = 0, j = um->prop_গুনতি - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, পূর্ণাঙ্ক prop_গুনতি)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    যদি (lnum < 1 || lnum > buf->b_ml.ml_সাৰি_গুনতি)
    {
	um.buf = রিক্ত;
	ঘূৰাই um;
    }

    char_u *সাৰি = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(সাৰি);

    যদি (text_copy == রিক্ত)
    {
	um.buf = রিক্ত;
	ঘূৰাই um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    যদি (prop_গুনতি > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_গুনতি);
	যদি (um.props == রিক্ত)
	{
	    vim_free(um.text);
	    um.buf = রিক্ত;
	    ঘূৰাই um;
	}
	um.prop_size = prop_গুনতি;
    }

    ঘূৰাই um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    স্থির শূন্য
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					পূর্ণাঙ্ক tp_id, পূর্ণাঙ্ক tp_type, পূর্ণাঙ্ক flag)
{
    unpacked_memline_T	neighbor;

    যদি (lnum < 1 || lnum > buf->b_ml.ml_সাৰি_গুনতি)
	ঘূৰাই;

    neighbor = um_open_at(buf, lnum, 0);
    যদি (neighbor.buf == রিক্ত)
	ঘূৰাই;

    বাবে (পূর্ণাঙ্ক i = 0; i < neighbor.prop_গুনতি; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	যদি (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    যদি (!neighbor.detached && !um_detach(&neighbor))
		ভঙ্গ;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    ভঙ্গ;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    শূন্য
um_delete_prop(unpacked_memline_T *um, পূর্ণাঙ্ক index)
{
    textprop_T *prop;

    যদি (um->buf == রিক্ত || index < 0 || index >= um->prop_গুনতি)
	ঘূৰাই;
    যদি (!um->detached && !um_detach(um))
	ঘূৰাই;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    যদি (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    যদি (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    যদি (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
    স্থির char_u *
um_pack(unpacked_memline_T *um, পূর্ণাঙ্ক *packed_len)
{
    uint16_t	live_গুনতি = 0;
    পূর্ণাঙ্ক		vtext_size = 0;
    পূর্ণাঙ্ক		total_len;
    char_u	*buf;
    char_u	*গুনতি_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    বাবে (পূর্ণাঙ্ক i = 0; i < um->prop_গুনতি; ++i)
    {
	textprop_T *prop = &um->props[i];

	যদি (prop->tp_flags & TP_FLAG_DELETED)
	    অনুৱর্ত;
	++live_গুনতি;
	যদি (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    যদি (live_গুনতি == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	যদি (buf == রিক্ত)
	    ঘূৰাই রিক্ত;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	ঘূৰাই buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (পূর্ণাঙ্ক)PROP_গুনতি_SIZE
		+ live_গুনতি * (পূর্ণাঙ্ক)পরিমাণ(textprop_T) + vtext_size;
    buf = alloc(total_len);
    যদি (buf == রিক্ত)
	ঘূৰাই রিক্ত;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    গুনতি_dest = buf + um->text_size;
    mch_memmove(গুনতি_dest, &live_গুনতি, PROP_গুনতি_SIZE);

    prop_dest = গুনতি_dest + PROP_গুনতি_SIZE;
    vtext_dest = prop_dest + live_গুনতি * পরিমাণ(textprop_T);

    বাবে (পূর্ণাঙ্ক i = 0; i < um->prop_গুনতি; ++i)
    {
	textprop_T prop = um->props[i];

	যদি (prop.tp_flags & TP_FLAG_DELETED)
	    অনুৱর্ত;

	যদি (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    পূর্ণাঙ্ক copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - গুনতি_dest);
	    vtext_dest += copysize;
	}
	অন্যথা
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, পরিমাণ(textprop_T));
	prop_dest += পরিমাণ(textprop_T);
    }

    ঘূৰাই buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    স্থির শূন্য
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    পূর্ণাঙ্ক	    packed_len;

    যদি (!um->detached || um->buf == রিক্ত || um->lnum == 0)
	ঘূৰাই;

    packed = um_pack(um, &packed_len);
    যদি (packed == রিক্ত)
    {
	um->buf = রিক্ত;
	ঘূৰাই;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    যদি (um->buf->b_ml.ml_সাৰি_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    অন্যথা যদি (!(um->buf->b_ml.ml_flags & ML_সাৰি_DIRTY))
	(শূন্য)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_গুনতি);

    যদি (um->buf->b_ml.ml_flags & (ML_সাৰি_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_সাৰি_ptr);
    um->buf->b_ml.ml_সাৰি_ptr = packed;
    um->buf->b_ml.ml_সাৰি_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_সাৰি_DIRTY;

    um->detached = false;
    um->text = packed;
    um->text_size = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_সাৰি_textlen = um->text_size;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    শূন্য
um_close(unpacked_memline_T *um)
{
    যদি (um->buf == রিক্ত)
	ঘূৰাই;
    যদি (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = রিক্ত;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    শূন্য
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = রিক্ত;
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
#define HI2PT(hi)      HIKEY2PT((hi)->hi_চাবি)

// The global text property types.
স্থির hashtab_T *global_proptypes = রিক্ত;
স্থির proptype_T **global_proparray = রিক্ত;

// The last used text property type ID.
স্থির পূর্ণাঙ্ক proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    স্থির hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    যদি (*name == NUL)
	ঘূৰাই রিক্ত;
    যদি (buf == রিক্ত)
	ht = global_proptypes;
    অন্যথা
	ht = buf->b_proptypes;

    যদি (ht == রিক্ত)
	ঘূৰাই রিক্ত;
    hi = hash_find(ht, name);
    যদি (HASHITEM_EMPTY(hi))
	ঘূৰাই রিক্ত;
    ঘূৰাই hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    স্থির proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    যদি (hi == রিক্ত)
	ঘূৰাই রিক্ত;
    ঘূৰাই HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    পূর্ণাঙ্ক
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    যদি (pt == রিক্ত)
	ঘূৰাই 0;
    ঘূৰাই pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    স্থির proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = find_prop_type(name, buf);

    যদি (type == রিক্ত)
	type = find_prop_type(name, রিক্ত);
    যদি (type == রিক্ত)
	semsg(_(e_property_type_str_does_not_exist), name);
    ঘূৰাই type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    স্থির পূর্ণাঙ্ক
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    যদি (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ঘূৰাই FAIL;
    }
    যদি (arg->vval.v_dict == রিক্ত)
	ঘূৰাই OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    যদি (di != রিক্ত && (di->di_tv.v_type != VAR_সংখ্যা
					      || di->di_tv.vval.v_সংখ্যা != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	যদি (*buf == রিক্ত)
	    ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    শূন্য
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    start_lnum = tv_get_সংখ্যা(&argvars[0]);
    start_col = tv_get_সংখ্যা(&argvars[1]);
    যদি (check_for_dict_arg(argvars, 2) == FAIL)
	ঘূৰাই;

    rettv->vval.v_সংখ্যা = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    স্থির পূর্ণাঙ্ক
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	পূর্ণাঙ্ক		id,
	char_u		*text_arg,
	পূর্ণাঙ্ক		text_padding_বাম,
	পূর্ণাঙ্ক		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    পূর্ণাঙ্ক		proplen;
    char_u	*props = রিক্ত;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    পূর্ণাঙ্ক		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    পূর্ণাঙ্ক		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    যদি (type == রিক্ত)
	গচ্ছ theend;

    যদি (start_lnum < 1 || start_lnum > buf->b_ml.ml_সাৰি_গুনতি)
    {
	semsg(_(e_invalid_সাৰি_সংখ্যা_nr), (long)start_lnum);
	গচ্ছ theend;
    }
    যদি (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_সাৰি_গুনতি)
    {
	semsg(_(e_invalid_সাৰি_সংখ্যা_nr), (long)end_lnum);
	গচ্ছ theend;
    }

    যদি (buf->b_ml.ml_mfp == রিক্ত)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	গচ্ছ theend;
    }

    যদি (text != রিক্ত)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	বাবে (p = text; *p != NUL; MB_PTR_ADV(p))
	    যদি (*p < ' ')
		*p = ' ';
    }

    বাবে (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	যদি (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_সাৰি));
	    গচ্ছ theend;
	}

	যদি (lnum == start_lnum)
	    col = start_col;
	অন্যথা
	    col = 1;
	যদি (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != রিক্ত))
	{
	    semsg(_(e_invalid_column_সংখ্যা_nr), (long)start_col);
	    গচ্ছ theend;
	}
	sort_col = col;

	যদি (lnum == end_lnum)
	    length = end_col - col;
	অন্যথা
	    length = (পূর্ণাঙ্ক)textlen - col + 1;
	যদি (length > (long)textlen)
	    length = (পূর্ণাঙ্ক)textlen;	// can include the end-of-line
	যদি (length < 0)
	    length = 0;		// zero-width property

	যদি (text_arg != রিক্ত)
	{
	    length = (long)STRLEN(text_arg);
	    যদি (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		যদি ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	tmp_prop.tp_padleft = text_padding_বাম;
	যদি (text_arg != রিক্ত)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	বাবে (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * পরিমাণ(textprop_T),
							   পরিমাণ(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    যদি (prop_col >= sort_col)
		ভঙ্গ;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	পূর্ণাঙ্ক		vtext_total = 0;
	পূর্ণাঙ্ক		new_সাৰি_len;
	char_u	*গুনতি_dest;
	char_u	*vtext_dest;
	পূর্ণাঙ্ক		j;

	// Compute total vtext size from existing props.
	বাবে (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * পরিমাণ(textprop_T),
							পরিমাণ(textprop_T));
	    যদি (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	যদি (text_arg != রিক্ত)
	    vtext_total += length + 1;

	new_সাৰি_len = (পূর্ণাঙ্ক)textlen + (পূর্ণাঙ্ক)PROP_গুনতি_SIZE
			+ new_propcount * (পূর্ণাঙ্ক)পরিমাণ(textprop_T)
			+ vtext_total;
	newtext = alloc(new_সাৰি_len);
	যদি (newtext == রিক্ত)
	    গচ্ছ theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_সাৰি_ptr, textlen);

	// Write prop_count.
	গুনতি_dest = newtext + textlen;
	mch_memmove(গুনতি_dest, &new_propcount, PROP_গুনতি_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = গুনতি_dest + PROP_গুনতি_SIZE;
	যদি (i > 0)
	    mch_memmove(newprops, props, পরিমাণ(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	যদি (i < proplen)
	    mch_memmove(newprops + (i + 1) * পরিমাণ(textprop_T),
					props + i * পরিমাণ(textprop_T),
					পরিমাণ(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * পরিমাণ(textprop_T);
	বাবে (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    যদি (j == i)
		অনুৱর্ত;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * পরিমাণ(textprop_T),
		    পরিমাণ(textprop_T));
	    যদি (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_গুনতি_2 = (char_u *)props - PROP_গুনতি_SIZE;
		char_u *old_vtext = old_গুনতি_2 + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - গুনতি_dest);
		mch_memmove(newprops + j * পরিমাণ(textprop_T), &ep,
			পরিমাণ(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	যদি (text_arg != রিক্ত)
	{
	    পূর্ণাঙ্ক copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - গুনতি_dest);
	    vtext_dest += copysize;
	}
	অন্যথা
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * পরিমাণ(textprop_T), &tmp_prop,
							পরিমাণ(textprop_T));

	যদি (buf->b_ml.ml_flags & (ML_সাৰি_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_সাৰি_ptr);
	buf->b_ml.ml_সাৰি_ptr = newtext;
	buf->b_ml.ml_সাৰি_len = new_সাৰি_len;
	buf->b_ml.ml_flags |= ML_সাৰি_DIRTY;
    }

    changed_সাৰি_display_buf(buf);
    changed_সাৰিগুলি_buf(buf, start_lnum, end_lnum + 1, 0);
    res = OK;

theend:
    vim_free(text);
    ঘূৰাই res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    শূন্য
f_prop_add_list(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    পূর্ণাঙ্ক		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    পূর্ণাঙ্ক		error = FALSE;
    পূর্ণাঙ্ক		prev_did_emsg = did_emsg;

    যদি (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	ঘূৰাই;

    যদি (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ঘূৰাই;

    dict = argvars[0].vval.v_dict;
    যদি (dict == রিক্ত || !dict_has_চাবি(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ঘূৰাই;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    যদি (dict_has_চাবি(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_সংখ্যা(dict, "id");
	যদি (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ঘূৰাই;
	}
	id = (পূর্ণাঙ্ক)x;
    }

    যদি (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ঘূৰাই;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_বস্তুসমূহ(argvars[1].vval.v_list, li)
    {
	যদি (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == রিক্ত)
	{
	    emsg(_(e_list_required));
	    ঘূৰাই;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	যদি (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	যদি (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	যদি (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	পূর্ণাঙ্ক this_id = id;
	যদি (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	যদি (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    যদি (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    ঘূৰাই;
	}
	যদি (prop_add_one(buf, type_name, this_id, রিক্ত, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    ঘূৰাই;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
স্থির পূর্ণাঙ্ক vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    স্থির পূর্ণাঙ্ক
get_textprop_id(শূন্য)
{
    যদি (vt_id_counter > 0)
	vt_id_counter = 0;
    ঘূৰাই --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    পূর্ণাঙ্ক
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
    পূর্ণাঙ্ক		id = 0;
    char_u	*text = রিক্ত;
    পূর্ণাঙ্ক		text_padding_বাম = 0;
    পূর্ণাঙ্ক		flags = 0;

    যদি (dict == রিক্ত || !dict_has_চাবি(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	গচ্ছ theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    যদি (dict_has_চাবি(dict, "end_lnum"))
    {
	end_lnum = dict_get_সংখ্যা(dict, "end_lnum");
	যদি (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_মান_for_argument_str), "end_lnum");
	    গচ্ছ theend;
	}
    }
    অন্যথা
	end_lnum = start_lnum;

    যদি (dict_has_চাবি(dict, "length"))
    {
	long length = dict_get_সংখ্যা(dict, "length");

	যদি (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_মান_for_argument_str), "length");
	    গচ্ছ theend;
	}
	end_col = start_col + length;
    }
    অন্যথা যদি (dict_has_চাবি(dict, "end_col"))
    {
	end_col = dict_get_সংখ্যা(dict, "end_col");
	যদি (end_col <= 0)
	{
	    semsg(_(e_invalid_মান_for_argument_str), "end_col");
	    গচ্ছ theend;
	}
    }
    অন্যথা যদি (start_lnum == end_lnum)
	end_col = start_col;
    অন্যথা
	end_col = 1;

    যদি (dict_has_চাবি(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_সংখ্যা(dict, "id");
	যদি (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    গচ্ছ theend;
	}
	id = (পূর্ণাঙ্ক)x;
    }

    যদি (dict_has_চাবি(dict, "text"))
    {
	যদি (dict_has_চাবি(dict, "length")
		|| dict_has_চাবি(dict, "end_col")
		|| dict_has_চাবি(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    গচ্ছ theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	যদি (text == রিক্ত)
	    গচ্ছ theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	যদি (dict_has_চাবি(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    যদি (p == রিক্ত)
		গচ্ছ theend;
	    যদি (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		গচ্ছ theend;
	    }
	    যদি (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_ডান;
	    অন্যথা যদি (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    অন্যথা যদি (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    অন্যথা যদি (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_মান_for_argument_str_str), "text_align", p);
		গচ্ছ theend;
	    }
	}

	যদি (dict_has_চাবি(dict, "text_padding_left"))
	{
	    text_padding_বাম = dict_get_সংখ্যা(dict, "text_padding_left");
	    যদি (text_padding_বাম < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		গচ্ছ theend;
	    }
	}

	যদি (dict_has_চাবি(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    যদি (p == রিক্ত)
		গচ্ছ theend;
	    যদি (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    অন্যথা যদি (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_মান_for_argument_str_str), "text_wrap", p);
		গচ্ছ theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    যদি (start_col < (text == রিক্ত ? 1 : 0))
    {
	semsg(_(e_invalid_column_সংখ্যা_nr), (long)start_col);
	গচ্ছ theend;
    }
    যদি (start_col > 0 && text_padding_বাম > 0)
    {
	emsg(_(e_can_only_use_বাম_padding_when_column_is_zero));
	গচ্ছ theend;
    }

    যদি (dict_arg != রিক্ত && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	গচ্ছ theend;

    যদি (text != রিক্ত)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    অন্যথা যদি (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	গচ্ছ theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_বাম, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = রিক্ত;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    ঘূৰাই id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    পূর্ণাঙ্ক
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, পূর্ণাঙ্ক will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_গুনতি;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    যদি ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == রিক্ত)
	ঘূৰাই 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_সাৰি_len - textlen;
    যদি (propdata_len == 0)
	ঘূৰাই 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    যদি (propdata_len < PROP_গুনতি_SIZE + পরিমাণ(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	ঘূৰাই 0;
    }
    mch_memmove(&prop_গুনতি, text + textlen, PROP_গুনতি_SIZE);
    যদি (!text_prop_গুনতি_valid(prop_গুনতি, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	ঘূৰাই 0;
    }
    যদি (!text_prop_vtext_valid(text + textlen + PROP_গুনতি_SIZE,
					   (পূর্ণাঙ্ক)prop_গুনতি, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	ঘূৰাই 0;
    }
    *props = text + textlen + PROP_গুনতি_SIZE;
    ঘূৰাই (পূর্ণাঙ্ক)prop_গুনতি;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    পূর্ণাঙ্ক
prop_গুনতি_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    পূর্ণাঙ্ক		গুনতি = get_text_props(buf, lnum, &props, FALSE);
    পূর্ণাঙ্ক		result = 0;
    textprop_T	prop;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		next_ডান_goes_below = FALSE;

    যদি (গুনতি == 0)
	ঘূৰাই 0;
    বাবে (i = 0; i < গুনতি; ++i)
    {
	mch_memmove(&prop, props + i * পরিমাণ(prop), পরিমাণ(prop));
	যদি (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    যদি ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_ডান_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_ডান)))
	    {
		++result;
	    }
	    অন্যথা যদি (prop.tp_flags & TP_FLAG_ALIGN_ডান)
		next_ডান_goes_below = TRUE;
	}
    }
    ঘূৰাই result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    পূর্ণাঙ্ক
গুনতি_props(linenr_T lnum, পূর্ণাঙ্ক only_starting, পূর্ণাঙ্ক last_সাৰি)
{
    char_u	*props;
    পূর্ণাঙ্ক		proplen = get_text_props(curbuf, lnum, &props, 0);
    পূর্ণাঙ্ক		result = proplen;
    textprop_T	prop;

    বাবে (পূর্ণাঙ্ক i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * পরিমাণ(prop), পরিমাণ(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	যদি ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_সাৰি && prop.tp_col == MAXCOL))
	    --result;
    }
    ঘূৰাই result;
}

স্থির textprop_T	*text_prop_compare_props;
স্থির buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    স্থির পূর্ণাঙ্ক
text_prop_order(পূর্ণাঙ্ক flags)
{
    যদি (flags & TP_FLAG_ALIGN_ABOVE)
	ঘূৰাই 0;
    যদি (flags & TP_FLAG_ALIGN_ডান)
	ঘূৰাই 2;
    যদি (flags & TP_FLAG_ALIGN_BELOW)
	ঘূৰাই 3;
    ঘূৰাই 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    স্থির পূর্ণাঙ্ক
text_prop_compare(ধ্রুৱ শূন্য *s1, ধ্রুৱ শূন্য *s2)
{
    পূর্ণাঙ্ক  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(পূর্ণাঙ্ক *)s1;
    idx2 = *(পূর্ণাঙ্ক *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    যদি ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	ঘূৰাই tp1->tp_id < 0 ? 1 : -1;

    যদি (col1 == MAXCOL || col2 == MAXCOL)
    {
	পূর্ণাঙ্ক order1 = text_prop_order(tp1->tp_flags);
	পূর্ণাঙ্ক order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	যদি (order1 != order2)
	    ঘূৰাই order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    যদি (pt1 != pt2)
    {
	যদি (pt1 == রিক্ত)
	    ঘূৰাই -1;
	যদি (pt2 == রিক্ত)
	    ঘূৰাই 1;
	যদি (pt1->pt_priority != pt2->pt_priority)
	    ঘূৰাই pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    যদি (col1 != col2)
	ঘূৰাই col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    যদি (tp1->tp_id < 0)
	ঘূৰাই tp1->tp_id > tp2->tp_id ? 1 : -1;

    ঘূৰাই 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    শূন্য
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	পূর্ণাঙ্ক	    *idxs,
	পূর্ণাঙ্ক	    গুনতি)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((শূন্য *)idxs, (size_t)গুনতি, পরিমাণ(পূর্ণাঙ্ক), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_সাৰিগুলি(
	win_T	    *wp,
	পূর্ণাঙ্ক	    type_id,
	পূর্ণাঙ্ক	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    যদি (text_prop_type_by_id(wp->w_buffer, type_id) == রিক্ত)
	ঘূৰাই false;

    যদি (first_lnum < 1)
	first_lnum = 1;
    যদি (last_lnum > wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	last_lnum = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;

    বাবে (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	পূর্ণাঙ্ক	গুনতি = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	বাবে (পূর্ণাঙ্ক i = 0; i < গুনতি; ++i)
	{
	    mch_memmove(prop, props + i * পরিমাণ(textprop_T),
							   পরিমাণ(textprop_T));
	    যদি (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		ঘূৰাই true;
	    }
	}
    }
    ঘূৰাই false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	পূর্ণাঙ্ক	    type_id,
	পূর্ণাঙ্ক	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    ঘূৰাই find_prop_in_সাৰিগুলি(wp, type_id, id, prop, found_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    স্থির শূন্য
set_text_props(linenr_T lnum, textprop_T *tps, পূর্ণাঙ্ক গুনতি)
{
    char_u  *text;
    char_u  *newtext;
    পূর্ণাঙ্ক	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    যদি (গুনতি == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	যদি (newtext == রিক্ত)
	    ঘূৰাই;
	mch_memmove(newtext, text, textlen);
	যদি (curbuf->b_ml.ml_flags & (ML_সাৰি_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_সাৰি_ptr);
	curbuf->b_ml.ml_সাৰি_ptr = newtext;
	curbuf->b_ml.ml_সাৰি_len = textlen;
	curbuf->b_ml.ml_flags |= ML_সাৰি_DIRTY;
    }
    অন্যথা
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_গুনতি = (uint16_t)গুনতি;
	পূর্ণাঙ্ক	    vtext_size = 0;
	পূর্ণাঙ্ক	    total_len;
	char_u	    *গুনতি_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	পূর্ণাঙ্ক	    i;

	বাবে (i = 0; i < গুনতি; ++i)
	    যদি (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (পূর্ণাঙ্ক)PROP_গুনতি_SIZE
		    + গুনতি * (পূর্ণাঙ্ক)পরিমাণ(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	যদি (newtext == রিক্ত)
	    ঘূৰাই;
	mch_memmove(newtext, text, textlen);

	গুনতি_dest = newtext + textlen;
	mch_memmove(গুনতি_dest, &prop_গুনতি, PROP_গুনতি_SIZE);

	prop_dest = গুনতি_dest + PROP_গুনতি_SIZE;
	vtext_dest = prop_dest + গুনতি * পরিমাণ(textprop_T);

	বাবে (i = 0; i < গুনতি; ++i)
	{
	    textprop_T prop = tps[i];

	    যদি (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		পূর্ণাঙ্ক copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = রিক্ত;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - গুনতি_dest);
		vtext_dest += copysize;
	    }
	    অন্যথা
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, পরিমাণ(textprop_T));
	    prop_dest += পরিমাণ(textprop_T);
	}

	যদি (curbuf->b_ml.ml_flags & (ML_সাৰি_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_সাৰি_ptr);
	curbuf->b_ml.ml_সাৰি_ptr = newtext;
	curbuf->b_ml.ml_সাৰি_len = total_len;
	curbuf->b_ml.ml_flags |= ML_সাৰি_DIRTY;
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
props_add_গুনতি_header(char_u *সাৰি, পূর্ণাঙ্ক সাৰি_len, পূর্ণাঙ্ক textlen, পূর্ণাঙ্ক *new_len)
{
    পূর্ণাঙ্ক		prop_bytes = সাৰি_len - textlen;
    uint16_t	prop_গুনতি;
    পূর্ণাঙ্ক		vtext_size = 0;
    পূর্ণাঙ্ক		total;
    char_u	*newline;
    char_u	*গুনতি_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    পূর্ণাঙ্ক		i;

    যদি (prop_bytes <= 0 || prop_bytes % পরিমাণ(textprop_T) != 0)
    {
	*new_len = সাৰি_len;
	ঘূৰাই রিক্ত;
    }
    prop_গুনতি = (uint16_t)(prop_bytes / পরিমাণ(textprop_T));

    // Calculate total vtext size.
    বাবে (i = 0; i < (পূর্ণাঙ্ক)prop_গুনতি; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, সাৰি + textlen + i * পরিমাণ(textprop_T),
							পরিমাণ(textprop_T));
	যদি (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (পূর্ণাঙ্ক)PROP_গুনতি_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    যদি (newline == রিক্ত)
	ঘূৰাই রিক্ত;
    *new_len = total;

    mch_memmove(newline, সাৰি, textlen);
    গুনতি_dest = newline + textlen;
    mch_memmove(গুনতি_dest, &prop_গুনতি, PROP_গুনতি_SIZE);
    prop_dest = গুনতি_dest + PROP_গুনতি_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    বাবে (i = 0; i < (পূর্ণাঙ্ক)prop_গুনতি; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, সাৰি + textlen + i * পরিমাণ(textprop_T),
							পরিমাণ(textprop_T));
	যদি (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    পূর্ণাঙ্ক copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - গুনতি_dest);
	    vtext_dest += copysize;
	}
	অন্যথা
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * পরিমাণ(textprop_T), &prop,
							পরিমাণ(textprop_T));
    }
    ঘূৰাই newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    শূন্য
add_text_props(linenr_T lnum, textprop_T *text_props, পূর্ণাঙ্ক text_prop_গুনতি)
{
    unpacked_memline_T	um;
    পূর্ণাঙ্ক			i;

    um = um_open_at(curbuf, lnum, text_prop_গুনতি);
    যদি (um.buf == রিক্ত)
	ঘূৰাই;
    যদি (!um_detach(&um))
    {
	um_abort(&um);
	ঘূৰাই;
    }

    // Grow props array if needed.
    যদি (um.prop_গুনতি + text_prop_গুনতি > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_গুনতি + text_prop_গুনতি) * পরিমাণ(textprop_T));
	যদি (newprops == রিক্ত)
	{
	    um_abort(&um);
	    ঘূৰাই;
	}
	um.props = newprops;
	um.prop_size = um.prop_গুনতি + text_prop_গুনতি;
    }

    বাবে (i = 0; i < text_prop_গুনতি; ++i)
	um.props[um.prop_গুনতি++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    স্থির পূর্ণাঙ্ক
compare_pt(ধ্রুৱ শূন্য *s1, ধ্রুৱ শূন্য *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    ঘূৰাই tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    স্থির proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, পূর্ণাঙ্ক id)
{
    পূর্ণাঙ্ক নিচেৰ = 0;
    পূর্ণাঙ্ক উপৰেৰ;

    যদি (ht == রিক্ত || ht->ht_used == 0)
	ঘূৰাই রিক্ত;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    যদি (*array == রিক্ত)
    {
	long	    todo;
	hashitem_T  *hi;
	পূর্ণাঙ্ক	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	যদি (*array == রিক্ত)
	    ঘূৰাই রিক্ত;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
	{
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((শূন্য *)*array, ht->ht_used, পরিমাণ(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    উপৰেৰ = ht->ht_used;
    যেতিয়ালৈ (উপৰেৰ > নিচেৰ)
    {
	পূর্ণাঙ্ক m = (উপৰেৰ + নিচেৰ) / 2;

	যদি ((*array)[m]->pt_id == id)
	    ঘূৰাই (*array)[m];
	যদি ((*array)[m]->pt_id > id)
	    উপৰেৰ = m;
	অন্যথা
	    নিচেৰ = m + 1;
    }
    ঘূৰাই রিক্ত;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    স্থির শূন্য
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    পূর্ণাঙ্ক buflocal = TRUE;
    পূর্ণাঙ্ক virtualtext_prop = prop->tp_id < 0;

    dict_add_সংখ্যা(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    যদি (!virtualtext_prop)
    {
	dict_add_সংখ্যা(dict, "length", prop->tp_len);
	dict_add_সংখ্যা(dict, "id", prop->tp_id);
    }
    dict_add_সংখ্যা(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_সংখ্যা(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    যদি (pt == রিক্ত)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    যদি (pt != রিক্ত)
	dict_add_string(dict, "type", pt->pt_name);

    যদি (buflocal)
	dict_add_সংখ্যা(dict, "type_bufnr", buf->b_fnum);
    অন্যথা
	dict_add_সংখ্যা(dict, "type_bufnr", 0);
    যদি (virtualtext_prop)
    {
	string_T    text_align = {রিক্ত, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	যদি (prop->tp_flags & TP_FLAG_ALIGN_ডান)
	    STR_LITERAL_SET(text_align, "right");
	অন্যথা যদি (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	অন্যথা যদি (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	যদি (text_align.string != রিক্ত)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (পূর্ণাঙ্ক)text_align.length);

	// text_wrap
	যদি (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	যদি (prop->tp_padleft != 0)
	    dict_add_সংখ্যা(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, পূর্ণাঙ্ক id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    যদি (type == রিক্ত)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    ঘূৰাই type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    ঘূৰাই text_prop_type_by_id(buf, prop->tp_type) != রিক্ত;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    শূন্য
f_prop_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    linenr_T start;
    linenr_T end;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    পূর্ণাঙ্ক	    did_clear = FALSE;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ঘূৰাই;

    start = tv_get_সংখ্যা(&argvars[0]);
    end = start;
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_সংখ্যা(&argvars[1]);
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    যদি (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		ঘূৰাই;
	}
    }
    যদি (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	ঘূৰাই;
    }

    {
	unpacked_memline_T um = um_open(buf);

	বাবে (lnum = start; lnum <= end; ++lnum)
	{
	    পূর্ণাঙ্ক idx;

	    যদি (lnum > buf->b_ml.ml_সাৰি_গুনতি)
		ভঙ্গ;
	    যদি (!um_goto_সাৰি(&um, lnum, 0))
		ভঙ্গ;
	    যদি (um.prop_গুনতি > 0)
	    {
		did_clear = TRUE;
		বাবে (idx = um.prop_গুনতি - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    যদি (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    শূন্য
f_prop_find(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    পূর্ণাঙ্ক		lnum_start;
    পূর্ণাঙ্ক		start_pos_has_prop = 0;
    পূর্ণাঙ্ক		seen_end = FALSE;
    পূর্ণাঙ্ক		id = 0;
    পূর্ণাঙ্ক		id_found = FALSE;
    পূর্ণাঙ্ক		type_id = -1;
    পূর্ণাঙ্ক		skipstart = FALSE;
    পূর্ণাঙ্ক		lnum = -1;
    পূর্ণাঙ্ক		col = -1;
    পূর্ণাঙ্ক		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    পূর্ণাঙ্ক		both;

    যদি (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    যদি (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ঘূৰাই;
    dict = argvars[0].vval.v_dict;

    যদি (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ঘূৰাই;
    যদি (buf->b_ml.ml_mfp == রিক্ত)
	ঘূৰাই;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	যদি (*dir_s == 'b')
	    dir = BACKWARD;
	অন্যথা যদি (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    ঘূৰাই;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    যদি (di != রিক্ত)
	lnum = tv_get_সংখ্যা(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    যদি (di != রিক্ত)
	col = tv_get_সংখ্যা(&di->di_tv);

    যদি (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    অন্যথা যদি (col == -1)
	col = 1;

    যদি (lnum < 1 || lnum > buf->b_ml.ml_সাৰি_গুনতি)
    {
	emsg(_(e_invalid_range));
	ঘূৰাই;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    যদি (dict_has_চাবি(dict, "id"))
    {
	id = dict_get_সংখ্যা(dict, "id");
	id_found = TRUE;
    }
    যদি (dict_has_চাবি(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	যদি (type == রিক্ত)
	    ঘূৰাই;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    যদি (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	ঘূৰাই;
    }
    যদি (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ঘূৰাই;
    }

    lnum_start = lnum;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ঘূৰাই;

    যেতিয়ালৈ (1)
    {
	char_u	*prop_start_ptr;
	পূর্ণাঙ্ক	গুনতি = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	পূর্ণাঙ্ক	    i;
	textprop_T  prop;
	পূর্ণাঙ্ক	    prop_start;
	পূর্ণাঙ্ক	    prop_end;

	বাবে (i = dir == BACKWARD ? গুনতি - 1 : 0; i >= 0 && i < গুনতি; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * পরিমাণ(textprop_T),
							   পরিমাণ(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    যদি (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_গুনতি_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    অন্যথা
		prop.u.tp_text = রিক্ত;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    যদি (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		যদি (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    যদি (!is_floating_vtext && prop.tp_col > col)
			অনুৱর্ত;
		}
		অন্যথা যদি (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    অনুৱর্ত;
	    }
	    যদি (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		যদি (lnum_start == lnum
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
		যদি (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		যদি (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    যদি (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    অনুৱর্ত;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		যদি (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    অনুৱর্ত;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_সংখ্যা(rettv->vval.v_dict, "lnum", lnum);

		ঘূৰাই;
	    }
	}

	যদি (dir > 0)
	{
	    যদি (lnum >= buf->b_ml.ml_সাৰি_গুনতি)
		ভঙ্গ;
	    lnum++;
	}
	অন্যথা
	{
	    যদি (lnum <= 1)
		ভঙ্গ;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    স্থির পূর্ণাঙ্ক
prop_type_or_id_in_list(পূর্ণাঙ্ক *types_or_ids, পূর্ণাঙ্ক len, পূর্ণাঙ্ক type_or_id)
{
    পূর্ণাঙ্ক i;

    বাবে (i = 0; i < len; i++)
	যদি (types_or_ids[i] == type_or_id)
	    ঘূৰাই TRUE;

    ঘূৰাই FALSE;
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
    স্থির শূন্য
get_props_in_সাৰি(
	buf_T		*buf,
	linenr_T	lnum,
	পূর্ণাঙ্ক		*prop_types,
	পূর্ণাঙ্ক		prop_types_len,
	পূর্ণাঙ্ক		*prop_ids,
	পূর্ণাঙ্ক		prop_ids_len,
	list_T		*retlist,
	পূর্ণাঙ্ক		add_lnum)
{
    char_u	*props;
    পূর্ণাঙ্ক		গুনতি;
    পূর্ণাঙ্ক		i;
    textprop_T	prop;

    গুনতি = get_text_props(buf, lnum, &props, FALSE);
    বাবে (i = 0; i < গুনতি; ++i)
    {
	mch_memmove(&prop, props + i * পরিমাণ(textprop_T),
		পরিমাণ(textprop_T));
	// Convert offset to pointer for virtual text props.
	যদি (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_গুনতি_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	অন্যথা
	    prop.u.tp_text = রিক্ত;
	যদি ((prop_types == রিক্ত
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == রিক্ত
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    যদি (d == রিক্ত)
		ভঙ্গ;
	    prop_fill_dict(d, &prop, buf);
	    যদি (add_lnum)
		dict_add_সংখ্যা(d, "lnum", lnum);
	    যদি (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    স্থির পূর্ণাঙ্ক *
get_prop_types_from_names(list_T *l, buf_T *buf, পূর্ণাঙ্ক *অঙ্ক_types)
{
    পূর্ণাঙ্ক		*prop_types;
    listitem_T	*li;
    পূর্ণাঙ্ক		i;
    char_u	*name;
    proptype_T	*type;

    *অঙ্ক_types = 0;

    prop_types = ALLOC_MULT(পূর্ণাঙ্ক, list_len(l));
    যদি (prop_types == রিক্ত)
	ঘূৰাই রিক্ত;

    i = 0;
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	যদি (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    গচ্ছ errret;
	}
	name = li->li_tv.vval.v_string;
	যদি (name == রিক্ত)
	    গচ্ছ errret;

	type = lookup_prop_type(name, buf);
	যদি (type == রিক্ত)
	    গচ্ছ errret;
	prop_types[i++] = type->pt_id;
    }

    *অঙ্ক_types = i;
    ঘূৰাই prop_types;

errret:
    VIM_CLEAR(prop_types);
    ঘূৰাই রিক্ত;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    স্থির পূর্ণাঙ্ক *
get_prop_ids_from_list(list_T *l, পূর্ণাঙ্ক *অঙ্ক_ids)
{
    পূর্ণাঙ্ক		*prop_ids;
    listitem_T	*li;
    পূর্ণাঙ্ক		i = 0;
    পূর্ণাঙ্ক		id;
    পূর্ণাঙ্ক		error;

    *অঙ্ক_ids = 0;

    prop_ids = ALLOC_MULT(পূর্ণাঙ্ক, list_len(l));
    যদি (prop_ids == রিক্ত)
	ঘূৰাই রিক্ত;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	error = FALSE;
	id = tv_get_সংখ্যা_chk(&li->li_tv, &error);
	যদি (error)
	    গচ্ছ errret;

	prop_ids[i++] = id;
    }

    *অঙ্ক_ids = i;
    ঘূৰাই prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    ঘূৰাই রিক্ত;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    শূন্য
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    buf_T	*buf = curbuf;
    পূর্ণাঙ্ক		add_lnum = FALSE;
    পূর্ণাঙ্ক		*prop_types = রিক্ত;
    পূর্ণাঙ্ক		prop_types_len = 0;
    পূর্ণাঙ্ক		*prop_ids = রিক্ত;
    পূর্ণাঙ্ক		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ঘূৰাই;

    // default: get text properties on current line
    start_lnum = tv_get_সংখ্যা(&argvars[0]);
    end_lnum = start_lnum;
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	যদি (check_for_dict_arg(argvars, 1) == FAIL)
	    ঘূৰাই;
	d = argvars[1].vval.v_dict;

	যদি (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ঘূৰাই;

	যদি (d != রিক্ত && (di = dict_find(d, (char_u *)"end_lnum", -1)) != রিক্ত)
	{
	    যদি (di->di_tv.v_type != VAR_সংখ্যা)
	    {
		emsg(_(e_সংখ্যা_required));
		ঘূৰাই;
	    }
	    end_lnum = tv_get_সংখ্যা(&di->di_tv);
	    যদি (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//সাৰি
		end_lnum = buf->b_ml.ml_সাৰি_গুনতি + end_lnum + 1;
	    অন্যথা যদি (end_lnum > buf->b_ml.ml_সাৰি_গুনতি)
		end_lnum = buf->b_ml.ml_সাৰি_গুনতি;
	    add_lnum = TRUE;
	}
	যদি (d != রিক্ত && (di = dict_find(d, (char_u *)"types", -1)) != রিক্ত)
	{
	    যদি (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ঘূৰাই;
	    }

	    l = di->di_tv.vval.v_list;
	    যদি (l != রিক্ত && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		যদি (prop_types == রিক্ত)
		    ঘূৰাই;
	    }
	}
	যদি (d != রিক্ত && (di = dict_find(d, (char_u *)"ids", -1)) != রিক্ত)
	{
	    যদি (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		গচ্ছ errret;
	    }

	    l = di->di_tv.vval.v_list;
	    যদি (l != রিক্ত && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		যদি (prop_ids == রিক্ত)
		    গচ্ছ errret;
	    }
	}
    }
    যদি (start_lnum < 1 || start_lnum > buf->b_ml.ml_সাৰি_গুনতি
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    অন্যথা
	বাবে (lnum = start_lnum; lnum <= end_lnum; lnum++)
	    get_props_in_সাৰি(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, add_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    শূন্য
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start = 1;
    linenr_T	end = 0;
    linenr_T	lnum;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    পূর্ণাঙ্ক		do_all;
    পূর্ণাঙ্ক		id = -MAXCOL;
    পূর্ণাঙ্ক		type_id = -1;	    // for a single "type"
    পূর্ণাঙ্ক		*type_ids = রিক্ত;   // array, for a list of "types", allocated
    পূর্ণাঙ্ক		অঙ্ক_type_ids = 0;   // number of elements in "type_ids"
    পূর্ণাঙ্ক		both;

    rettv->vval.v_সংখ্যা = 0;

    যদি (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL)))
	ঘূৰাই;

    যদি (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_সংখ্যা(&argvars[1]);
	end = start;
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_সংখ্যা(&argvars[2]);
	যদি (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    ঘূৰাই;
	}
    }

    dict = argvars[0].vval.v_dict;
    যদি (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ঘূৰাই;
    যদি (buf->b_ml.ml_mfp == রিক্ত)
	ঘূৰাই;

    do_all = dict_get_bool(dict, "all", FALSE);

    যদি (dict_has_চাবি(dict, "id"))
	id = dict_get_সংখ্যা(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    যদি (dict_has_চাবি(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	যদি (type == রিক্ত)
	    ঘূৰাই;
	type_id = type->pt_id;
    }
    যদি (dict_has_চাবি(dict, "types"))
    {
	typval_T types;
	listitem_T *li = রিক্ত;

	dict_get_tv(dict, "types", &types);
	যদি (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( পরিমাণ(পূর্ণাঙ্ক) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_বস্তুসমূহ(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		যদি (li->li_tv.v_type != VAR_STRING)
		    অনুৱর্ত;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		যদি (!prop_type)
		    গচ্ছ cleanup_prop_remove;

		type_ids[অঙ্ক_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    যদি (id == -MAXCOL && (type_id == -1 && অঙ্ক_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	গচ্ছ cleanup_prop_remove;
    }
    যদি (both && (id == -MAXCOL || (type_id == -1 && অঙ্ক_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	গচ্ছ cleanup_prop_remove;
    }
    যদি (type_id != -1 && অঙ্ক_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	গচ্ছ cleanup_prop_remove;
    }

    যদি (end == 0)
	end = buf->b_ml.ml_সাৰি_গুনতি;

    {
	unpacked_memline_T um = um_open(buf);

	বাবে (lnum = start; lnum <= end; ++lnum)
	{
	    পূর্ণাঙ্ক idx;

	    যদি (lnum > buf->b_ml.ml_সাৰি_গুনতি)
		ভঙ্গ;
	    যদি (!um_goto_সাৰি(&um, lnum, 0))
		ভঙ্গ;

	    বাবে (idx = 0; idx < um.prop_গুনতি; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		পূর্ণাঙ্ক	    matches_id = 0;
		পূর্ণাঙ্ক	    matches_type = 0;

		যদি (prop->tp_flags & TP_FLAG_DELETED)
		    অনুৱর্ত;

		matches_id = prop->tp_id == id;
		যদি (অঙ্ক_type_ids > 0)
		{
		    পূর্ণাঙ্ক idx2;

		    বাবে (idx2 = 0; !matches_type && idx2 < অঙ্ক_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		অন্যথা
		    matches_type = prop->tp_type == type_id;

		যদি (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    যদি (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_সংখ্যা;
		    যদি (!do_all)
			ভঙ্গ;
		}
	    }
	}
	um_close(&um);
    }

    যদি (first_changed > 0)
    {
	changed_সাৰি_display_buf(buf);
	changed_সাৰিগুলি_buf(buf, first_changed, last_changed + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    স্থির শূন্য
prop_type_set(typval_T *argvars, পূর্ণাঙ্ক add)
{
    char_u	*name;
    buf_T	*buf = রিক্ত;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    name = tv_get_string(&argvars[0]);
    যদি (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ঘূৰাই;
    }

    যদি (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	ঘূৰাই;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    যদি (add)
    {
	hashtab_T **htp;

	যদি (prop != রিক্ত)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    ঘূৰাই;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	যদি (prop == রিক্ত)
	    ঘূৰাই;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	যদি (buf == রিক্ত)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	অন্যথা
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	যদি (*htp == রিক্ত)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    যদি (*htp == রিক্ত)
	    {
		vim_free(prop);
		ঘূৰাই;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    অন্যথা
    {
	যদি (prop == রিক্ত)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    ঘূৰাই;
	}
    }

    যদি (dict != রিক্ত)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	যদি (di != রিক্ত)
	{
	    char_u	*highlight;
	    পূর্ণাঙ্ক		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    যদি (highlight != রিক্ত && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    যদি (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == রিক্ত ? (char_u *)"" : highlight);
		ঘূৰাই;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	যদি (di != রিক্ত)
	{
	    যদি (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    অন্যথা
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	যদি (di != রিক্ত)
	{
	    যদি (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    অন্যথা
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	যদি (di != রিক্ত)
	    prop->pt_priority = tv_get_সংখ্যা(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	যদি (di != রিক্ত)
	{
	    যদি (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    অন্যথা
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	যদি (di != রিক্ত)
	{
	    যদি (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    অন্যথা
		prop->pt_flags &= ~PT_FLAG_INS_END_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    শূন্য
f_prop_type_add(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    শূন্য
f_prop_type_change(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    শূন্য
f_prop_type_delete(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*name;
    buf_T	*buf = রিক্ত;
    hashitem_T	*hi;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    name = tv_get_string(&argvars[0]);
    যদি (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ঘূৰাই;
    }

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	যদি (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ঘূৰাই;
    }

    hi = find_prop_type_hi(name, buf);
    যদি (hi == রিক্ত)
	ঘূৰাই;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    যদি (buf == রিক্ত)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    অন্যথা
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    changed_window_setting_buf(buf == রিক্ত ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    শূন্য
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    name = tv_get_string(&argvars[0]);
    যদি (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ঘূৰাই;
    }

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ঘূৰাই;

    proptype_T  *prop = রিক্ত;
    buf_T	    *buf = রিক্ত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	যদি (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ঘূৰাই;
    }

    prop = find_prop_type(name, buf);
    যদি (prop == রিক্ত)
	ঘূৰাই;

    dict_T *d = rettv->vval.v_dict;

    যদি (prop->pt_hl_id > 0)
	dict_add_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_add_সংখ্যা(d, "priority", prop->pt_priority);
    dict_add_সংখ্যা(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_add_সংখ্যা(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_add_সংখ্যা(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_START_INCL) ? 1 : 0);
    dict_add_সংখ্যা(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_END_INCL) ? 1 : 0);
    যদি (buf != রিক্ত)
	dict_add_সংখ্যা(d, "bufnr", buf->b_fnum);
}

    স্থির শূন্য
list_types(hashtab_T *ht, list_T *l)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
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
    শূন্য
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = রিক্ত;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ঘূৰাই;

    যদি (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	যদি (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    ঘূৰাই;
    }
    যদি (buf == রিক্ত)
    {
	যদি (global_proptypes != রিক্ত)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    অন্যথা যদি (buf->b_proptypes != রিক্ত)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    স্থির শূন্য
clear_ht_prop_types(hashtab_T *ht)
{
    long	todo;
    hashitem_T	*hi;

    যদি (ht == রিক্ত)
	ঘূৰাই;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#যদি defined(EXITFREE)
/*
 * Free all global property types.
 */
    শূন্য
clear_global_prop_types(শূন্য)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = রিক্ত;
    VIM_CLEAR(global_proparray);
}
#endif

/*
 * Free all property types for "buf".
 */
    শূন্য
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = রিক্ত;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক dirty;	    // if the property was changed
    পূর্ণাঙ্ক can_drop;   // whether after this change, the prop may be removed
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
    স্থির adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    col,
	পূর্ণাঙ্ক	    added,
	পূর্ণাঙ্ক	    flags)
{
    proptype_T	*pt;
    পূর্ণাঙ্ক		start_incl;
    পূর্ণাঙ্ক		end_incl;
    পূর্ণাঙ্ক		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    যদি (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	ঘূৰাই res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != রিক্ত && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    যদি (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != রিক্ত && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    যদি (added > 0)
    {
	যদি (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	অন্যথা যদি (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    অন্যথা যদি (prop->tp_col > col + 1)
    {
	যদি (prop->tp_col + added < col + 1)
	{
	    যদি (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    অন্যথা
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		যদি (prop->tp_len <= 0)
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
	অন্যথা
	    prop->tp_col += added;
    }
    অন্যথা যদি (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	যদি (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	অন্যথা
	{
	    পূর্ণাঙ্ক after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	যদি (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    অন্যথা
	res.dirty = FALSE;

    ঘূৰাই res;
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
    পূর্ণাঙ্ক
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	পূর্ণাঙ্ক	    bytes_added,
	পূর্ণাঙ্ক	    flags)
{
    unpacked_memline_T um;
    পূর্ণাঙ্ক		dirty = FALSE;
    পূর্ণাঙ্ক		ri;

    যদি (text_prop_frozen > 0)
	ঘূৰাই FALSE;

    um = um_open_at(curbuf, lnum, 0);
    যদি (um.buf == রিক্ত || um.prop_গুনতি == 0)
    {
	um_abort(&um);
	ঘূৰাই FALSE;
    }

    বাবে (ri = 0; ri < um.prop_গুনতি; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	যদি (res.dirty)
	{
	    যদি (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		যদি (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    ঘূৰাই FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		যদি ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    ঘূৰাই FALSE;
		}
	    }
	    dirty = TRUE;
	}
	যদি (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    অনুৱর্ত;
	}
    }
    যদি (dirty)
	um_close(&um);
    অন্যথা
	um_abort(&um);
    ঘূৰাই dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    শূন্য
adjust_props_for_split(
	linenr_T    lnum_props,
	linenr_T    lnum_শীৰ্ষ,
	পূর্ণাঙ্ক	    kept,
	পূর্ণাঙ্ক	    deleted,
	পূর্ণাঙ্ক	    at_eol)
{
    char_u	*props;
    পূর্ণাঙ্ক		গুনতি;
    garray_T    prevprop;
    garray_T    nextprop;
    পূর্ণাঙ্ক		skipped = kept + deleted;

    যদি (!curbuf->b_has_textprop)
	ঘূৰাই;

    // Get the text properties from "lnum_props".
    গুনতি = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, পরিমাণ(textprop_T), 10);
    ga_init2(&nextprop, পরিমাণ(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *গুনতি_ptr = props - PROP_গুনতি_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    বাবে (পূর্ণাঙ্ক i = 0; i < গুনতি; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	পূর্ণাঙ্ক	    start_incl, end_incl;
	পূর্ণাঙ্ক	    cont_prev, cont_next;
	পূর্ণাঙ্ক	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * পরিমাণ(textprop_T), পরিমাণ(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	যদি (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(গুনতি_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	অন্যথা
	    prop.u.tp_text = রিক্ত;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != রিক্ত && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != রিক্ত && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	যদি (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	অন্যথা
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    পূর্ণাঙ্ক is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    যদি (is_floating_vtext)
		cont_next = FALSE;
	    অন্যথা
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	যদি (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	যদি (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    পূর্ণাঙ্ক is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    যদি (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    যদি (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	যদি (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    পূর্ণাঙ্ক is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    যদি (p->tp_col != MAXCOL)
	    {
		যদি (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		অন্যথা
		{
		    যদি (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    যদি (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    set_text_props(lnum_শীৰ্ষ, (textprop_T *)prevprop.ga_data,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    set_text_props(lnum_শীৰ্ষ + 1, (textprop_T *)nextprop.ga_data,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    শূন্য
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	পূর্ণাঙ্ক		    last_সাৰি,
	long		    col,
	পূর্ণাঙ্ক		    removed)
{
    unpacked_memline_T	r_um;

    যদি (um->buf == রিক্ত)
	ঘূৰাই;

    r_um = um_open_at(um->buf, lnum, 0);
    যদি (r_um.buf == রিক্ত)
	ঘূৰাই;
    যদি (!um_detach(&r_um))
    {
	um_abort(&r_um);
	ঘূৰাই;
    }

    বাবে (পূর্ণাঙ্ক i = r_um.prop_গুনতি - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	পূর্ণাঙ্ক	    end;

	যদি (prop->tp_col == MAXCOL && !last_সাৰি)
	    অনুৱর্ত;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	যদি (last_সাৰি || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = রিক্ত;  // ownership transferred
	}
	অন্যথা
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    বাবে (পূর্ণাঙ্ক j = 0; j < um->prop_গুনতি; ++j)
	    {
		textprop_T *op = &um->props[j];

		যদি ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    ভঙ্গ;
		}
	    }
	    যদি (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    স্থির শূন্য
remember_cleared_prop(garray_T *gap, পূর্ণাঙ্ক which, textprop_T *prop, পূর্ণাঙ্ক flag)
{
    undoprop_T	*up;

    যদি (which == UNDOPROP_NONE)
	ঘূৰাই;
    যদি (gap->ga_itemsize == 0)
	ga_init2(gap, পরিমাণ(undoprop_T), 4);
    যদি (ga_grow(gap, 1) == FAIL)
	ঘূৰাই;
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
    শূন্য
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	পূর্ণাঙ্ক	    above_which,
	পূর্ণাঙ্ক	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = রিক্ত;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    যদি (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_সাৰি_গুনতি)
	ঘূৰাই;

    above = um_open_at(buf, lnum, 0);
    যদি (above.buf == রিক্ত)
	ঘূৰাই;
    বাবে (পূর্ণাঙ্ক i = 0; i < above.prop_গুনতি; ++i)
	যদি (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    যদি (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	ঘূৰাই;
    }

    below = um_open_at(buf, lnum + 1, 0);
    যদি (below.buf == রিক্ত)
    {
	um_abort(&above);
	ঘূৰাই;
    }
    যদি (below.prop_গুনতি > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_গুনতি);
	যদি (paired == রিক্ত)
	    গচ্ছ theend;
    }

    বাবে (পূর্ণাঙ্ক i = 0; i < above.prop_গুনতি; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	যদি (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    অনুৱর্ত;
	বাবে (পূর্ণাঙ্ক j = 0; j < below.prop_গুনতি; ++j)
	{
	    textprop_T *next = &below.props[j];

	    যদি (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		ভঙ্গ;
	    }
	}
	যদি (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    বাবে (পূর্ণাঙ্ক j = 0; j < below.prop_গুনতি; ++j)
    {
	যদি (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    অনুৱর্ত;
	যদি (!below.detached && !um_detach(&below))
	    ভঙ্গ;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    যদি (below_changed)
	um_close(&below);
    অন্যথা
	um_abort(&below);
    যদি (above_changed)
	um_close(&above);
    অন্যথা
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    শূন্য
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	পূর্ণাঙ্ক	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    যদি (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_সাৰি_গুনতি)
	ঘূৰাই;
    um = um_open_at(buf, lnum, 0);
    যদি (um.buf == রিক্ত)
	ঘূৰাই;
    বাবে (পূর্ণাঙ্ক r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	যদি (up->up_which != which)
	    অনুৱর্ত;
	বাবে (পূর্ণাঙ্ক i = 0; i < um.prop_গুনতি; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    যদি (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		অনুৱর্ত;
	    যদি (!um.detached && !um_detach(&um))
		গচ্ছ theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    ভঙ্গ;
	}
    }

theend:
    যদি (changed)
	um_close(&um);
    অন্যথা
	um_abort(&um);
}

    bool
text_prop_গুনতি_valid(পূর্ণাঙ্ক prop_গুনতি, size_t propdata_len)
{
    যদি (propdata_len < PROP_গুনতি_SIZE)
	ঘূৰাই false;
    ঘূৰাই (size_t)prop_গুনতি * পরিমাণ(textprop_T)
		    <= propdata_len - PROP_গুনতি_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, পূর্ণাঙ্ক prop_গুনতি, size_t propdata_len)
{
    বাবে (পূর্ণাঙ্ক i = 0; i < prop_গুনতি; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * পরিমাণ(textprop_T),
							  পরিমাণ(textprop_T));
	যদি (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    অনুৱর্ত;
	যদি (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    ঘূৰাই false;
    }
    ঘূৰাই true;
}

#endif // FEAT_PROP_POPUP
