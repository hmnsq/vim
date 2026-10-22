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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_PROP_POPUP)

স্থির শূন্য um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    স্থির শূন্য
um_free_vtext(textprop_T *props, পূর্ণসংখ্যা গুনতি)
{
    জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি; ++i)
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
    um->prop_আকার = 0;
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
    ফেরত um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_সারি(unpacked_memline_T *um, linenr_T lnum, পূর্ণসংখ্যা extra_props)
{
    char_u	*সারি;
    size_t	textlen;
    size_t	propdata_len;
    পূর্ণসংখ্যা		proplen;

    যদি (um->buf == ফাঁকা)
	ফেরত false;

    // Store changes to the current line if any.
    যদি (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    যদি (lnum == 0)
	ফেরত true;	// just unload

    সারি = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_সারি_len - textlen;

    um->lnum = lnum;
    um->text_আকার_2 = (colnr_T)textlen;
    um->text = সারি;

    যদি (propdata_len == 0)
	ফেরত true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    যদি (propdata_len < PROP_গুনতি_আকার + আকার(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ফাঁকা;
	ফেরত false;
    }

    uint16_t    prop_গুনতি;
    char_u	    *গুনতি_ptr = সারি + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_গুনতি, গুনতি_ptr, PROP_গুনতি_আকার);
    যদি (!text_prop_গুনতি_valid(prop_গুনতি, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ফাঁকা;
	ফেরত false;
    }
    proplen = (পূর্ণসংখ্যা)prop_গুনতি;
    props_start = গুনতি_ptr + PROP_গুনতি_আকার;

    যদি (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ফাঁকা;
	ফেরত false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    যদি (um->props == ফাঁকা)
    {
	um->buf = ফাঁকা;
	ফেরত false;
    }
    um->prop_আকার = proplen + extra_props;
    um->prop_গুনতি = proplen;
    mch_memmove(um->props, props_start, proplen * আকার(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    জন্য (পূর্ণসংখ্যা i = 0; i < proplen; ++i)
    {
	যদি (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = গুনতি_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	অন্যথা
	    um->props[i].u.tp_text = ফাঁকা;
    }

    ফেরত true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, পূর্ণসংখ্যা extra_props)
{
    unpacked_memline_T um = um_open(buf);

    যদি (!um_goto_সারি(&um, lnum, extra_props))
	um.buf = ফাঁকা;
    ফেরত um;
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

    যদি (um->detached || um->buf == ফাঁকা)
	ফেরত um->detached;

    newtext = vim_strnsave(um->text, um->text_আকার_2 - 1);
    যদি (newtext == ফাঁকা)
    {
	um->buf = ফাঁকা;
	ফেরত false;
    }
    um->text = newtext;

    জন্য (পূর্ণসংখ্যা i = 0; i < um->prop_গুনতি; ++i)
    {
	যদি (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    যদি (copy == ফাঁকা)
	    {
		জন্য (পূর্ণসংখ্যা j = 0; j < i; ++j)
		    যদি (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = ফাঁকা;
		ফেরত false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    ফেরত true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    যদি (um->buf == ফাঁকা)
	ফেরত false;
    যদি (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_আকার_2 = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    ফেরত true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    স্থির bool
um_grow_props(unpacked_memline_T *um, পূর্ণসংখ্যা needed)
{
    যদি (um->prop_গুনতি + needed <= um->prop_আকার)
	ফেরত true;

    পূর্ণসংখ্যা	    new_আকার = um->prop_গুনতি + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_আকার * আকার(textprop_T));
    যদি (newprops == ফাঁকা)
	ফেরত false;
    um->props = newprops;
    um->prop_আকার = new_আকার;

    ফেরত true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    স্থির শূন্য
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    যদি (um->buf == ফাঁকা)
	ফেরত;
    যদি (!um->detached && !um_detach(um))
	ফেরত;
    যদি (!um_grow_props(um, 1))
    {
	um->buf = ফাঁকা;
	ফেরত;
    }
    um->props[um->prop_গুনতি++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    শূন্য
um_reverse_props(unpacked_memline_T *um)
{
    জন্য (পূর্ণসংখ্যা i = 0, j = um->prop_গুনতি - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, পূর্ণসংখ্যা prop_গুনতি)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    যদি (lnum < 1 || lnum > buf->b_ml.ml_সারি_গুনতি)
    {
	um.buf = ফাঁকা;
	ফেরত um;
    }

    char_u *সারি = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(সারি);

    যদি (text_copy == ফাঁকা)
    {
	um.buf = ফাঁকা;
	ফেরত um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_আকার_2 = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    যদি (prop_গুনতি > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_গুনতি);
	যদি (um.props == ফাঁকা)
	{
	    vim_free(um.text);
	    um.buf = ফাঁকা;
	    ফেরত um;
	}
	um.prop_আকার = prop_গুনতি;
    }

    ফেরত um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    স্থির শূন্য
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					পূর্ণসংখ্যা tp_id, পূর্ণসংখ্যা tp_type, পূর্ণসংখ্যা flag)
{
    unpacked_memline_T	neighbor;

    যদি (lnum < 1 || lnum > buf->b_ml.ml_সারি_গুনতি)
	ফেরত;

    neighbor = um_open_at(buf, lnum, 0);
    যদি (neighbor.buf == ফাঁকা)
	ফেরত;

    জন্য (পূর্ণসংখ্যা i = 0; i < neighbor.prop_গুনতি; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	যদি (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    যদি (!neighbor.detached && !um_detach(&neighbor))
		বিরতি;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    বিরতি;
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
um_delete_prop(unpacked_memline_T *um, পূর্ণসংখ্যা সূচক)
{
    textprop_T *prop;

    যদি (um->buf == ফাঁকা || সূচক < 0 || সূচক >= um->prop_গুনতি)
	ফেরত;
    যদি (!um->detached && !um_detach(um))
	ফেরত;

    prop = &um->props[সূচক];

    // Adjust continuation flags on neighboring lines before deleting.
    যদি (prop->tp_flags & TP_FLAG_CONT_পূর্ব)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_পরবর্তী);
    যদি (prop->tp_flags & TP_FLAG_CONT_পরবর্তী)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_পূর্ব);

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
um_pack(unpacked_memline_T *um, পূর্ণসংখ্যা *packed_len)
{
    uint16_t	live_গুনতি = 0;
    পূর্ণসংখ্যা		vtext_আকার = 0;
    পূর্ণসংখ্যা		যোগ_len;
    char_u	*buf;
    char_u	*গুনতি_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    জন্য (পূর্ণসংখ্যা i = 0; i < um->prop_গুনতি; ++i)
    {
	textprop_T *prop = &um->props[i];

	যদি (prop->tp_flags & TP_FLAG_DELETED)
	    চালু;
	++live_গুনতি;
	যদি (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_আকার += prop->tp_len + 1;
    }

    যদি (live_গুনতি == 0)
    {
	// No properties: just text, no prop_count header.
	যোগ_len = um->text_আকার_2;
	buf = alloc(যোগ_len);
	যদি (buf == ফাঁকা)
	    ফেরত ফাঁকা;
	*packed_len = যোগ_len;
	mch_memmove(buf, um->text, um->text_আকার_2);
	ফেরত buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    যোগ_len = um->text_আকার_2 + (পূর্ণসংখ্যা)PROP_গুনতি_আকার
		+ live_গুনতি * (পূর্ণসংখ্যা)আকার(textprop_T) + vtext_আকার;
    buf = alloc(যোগ_len);
    যদি (buf == ফাঁকা)
	ফেরত ফাঁকা;
    *packed_len = যোগ_len;

    mch_memmove(buf, um->text, um->text_আকার_2);

    গুনতি_dest = buf + um->text_আকার_2;
    mch_memmove(গুনতি_dest, &live_গুনতি, PROP_গুনতি_আকার);

    prop_dest = গুনতি_dest + PROP_গুনতি_আকার;
    vtext_dest = prop_dest + live_গুনতি * আকার(textprop_T);

    জন্য (পূর্ণসংখ্যা i = 0; i < um->prop_গুনতি; ++i)
    {
	textprop_T prop = um->props[i];

	যদি (prop.tp_flags & TP_FLAG_DELETED)
	    চালু;

	যদি (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    পূর্ণসংখ্যা copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - গুনতি_dest);
	    vtext_dest += copysize;
	}
	অন্যথা
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, আকার(textprop_T));
	prop_dest += আকার(textprop_T);
    }

    ফেরত buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    স্থির শূন্য
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    পূর্ণসংখ্যা	    packed_len;

    যদি (!um->detached || um->buf == ফাঁকা || um->lnum == 0)
	ফেরত;

    packed = um_pack(um, &packed_len);
    যদি (packed == ফাঁকা)
    {
	um->buf = ফাঁকা;
	ফেরত;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    যদি (um->buf->b_ml.ml_সারি_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    অন্যথা যদি (!(um->buf->b_ml.ml_flags & ML_সারি_DIRTY))
	(শূন্য)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_গুনতি);

    যদি (um->buf->b_ml.ml_flags & (ML_সারি_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_সারি_ptr);
    um->buf->b_ml.ml_সারি_ptr = packed;
    um->buf->b_ml.ml_সারি_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_সারি_DIRTY;

    um->detached = false;
    um->text = packed;
    um->text_আকার_2 = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_সারি_textlen = um->text_আকার_2;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    শূন্য
um_close(unpacked_memline_T *um)
{
    যদি (um->buf == ফাঁকা)
	ফেরত;
    যদি (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = ফাঁকা;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    শূন্য
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = ফাঁকা;
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
স্থির hashtab_T *global_proptypes = ফাঁকা;
স্থির proptype_T **global_proparray = ফাঁকা;

// The last used text property type ID.
স্থির পূর্ণসংখ্যা proptype_id = 0;

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
	ফেরত ফাঁকা;
    যদি (buf == ফাঁকা)
	ht = global_proptypes;
    অন্যথা
	ht = buf->b_proptypes;

    যদি (ht == ফাঁকা)
	ফেরত ফাঁকা;
    hi = hash_find(ht, name);
    যদি (HASHITEM_EMPTY(hi))
	ফেরত ফাঁকা;
    ফেরত hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    স্থির proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    যদি (hi == ফাঁকা)
	ফেরত ফাঁকা;
    ফেরত HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    পূর্ণসংখ্যা
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    যদি (pt == ফাঁকা)
	ফেরত 0;
    ফেরত pt->pt_id;
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

    যদি (type == ফাঁকা)
	type = find_prop_type(name, ফাঁকা);
    যদি (type == ফাঁকা)
	semsg(_(e_property_type_str_does_not_exist), name);
    ফেরত type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    স্থির পূর্ণসংখ্যা
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    যদি (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ফেরত FAIL;
    }
    যদি (arg->vval.v_dict == ফাঁকা)
	ফেরত OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    যদি (di != ফাঁকা && (di->di_tv.v_type != VAR_সংখ্যা
					      || di->di_tv.vval.v_সংখ্যা != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	যদি (*buf == ফাঁকা)
	    ফেরত FAIL;
    }
    ফেরত OK;
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
	ফেরত;

    start_lnum = tv_get_সংখ্যা(&argvars[0]);
    start_col = tv_get_সংখ্যা(&argvars[1]);
    যদি (check_for_dict_arg(argvars, 2) == FAIL)
	ফেরত;

    rettv->vval.v_সংখ্যা = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    স্থির পূর্ণসংখ্যা
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	পূর্ণসংখ্যা		id,
	char_u		*text_arg,
	পূর্ণসংখ্যা		text_padding_বাম,
	পূর্ণসংখ্যা		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    পূর্ণসংখ্যা		proplen;
    char_u	*props = ফাঁকা;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    পূর্ণসংখ্যা		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    পূর্ণসংখ্যা		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    যদি (type == ফাঁকা)
	যাও theend;

    যদি (start_lnum < 1 || start_lnum > buf->b_ml.ml_সারি_গুনতি)
    {
	semsg(_(e_invalid_সারি_সংখ্যা_nr), (long)start_lnum);
	যাও theend;
    }
    যদি (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_সারি_গুনতি)
    {
	semsg(_(e_invalid_সারি_সংখ্যা_nr), (long)end_lnum);
	যাও theend;
    }

    যদি (buf->b_ml.ml_mfp == ফাঁকা)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_বাফার));
	যাও theend;
    }

    যদি (text != ফাঁকা)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	জন্য (p = text; *p != NUL; MB_PTR_ADV(p))
	    যদি (*p < ' ')
		*p = ' ';
    }

    জন্য (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	দৈর্ঘ্য;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	যদি (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_সারি));
	    যাও theend;
	}

	যদি (lnum == start_lnum)
	    col = start_col;
	অন্যথা
	    col = 1;
	যদি (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != ফাঁকা))
	{
	    semsg(_(e_invalid_column_সংখ্যা_nr), (long)start_col);
	    যাও theend;
	}
	sort_col = col;

	যদি (lnum == end_lnum)
	    দৈর্ঘ্য = end_col - col;
	অন্যথা
	    দৈর্ঘ্য = (পূর্ণসংখ্যা)textlen - col + 1;
	যদি (দৈর্ঘ্য > (long)textlen)
	    দৈর্ঘ্য = (পূর্ণসংখ্যা)textlen;	// can include the end-of-line
	যদি (দৈর্ঘ্য < 0)
	    দৈর্ঘ্য = 0;		// zero-width property

	যদি (text_arg != ফাঁকা)
	{
	    দৈর্ঘ্য = (long)STRLEN(text_arg);
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
	tmp_prop.tp_len = দৈর্ঘ্য;
	tmp_prop.tp_id = id;
	tmp_prop.tp_type = type->pt_id;
	tmp_prop.tp_flags = text_flags
			    | (lnum > start_lnum ? TP_FLAG_CONT_পূর্ব : 0)
			    | (lnum < end_lnum ? TP_FLAG_CONT_পরবর্তী : 0)
			    | ((type->pt_flags & PT_FLAG_INS_START_INCL)
						     ? TP_FLAG_START_INCL : 0);
	tmp_prop.tp_padleft = text_padding_বাম;
	যদি (text_arg != ফাঁকা)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	জন্য (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * আকার(textprop_T),
							   আকার(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    যদি (prop_col >= sort_col)
		বিরতি;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	পূর্ণসংখ্যা		vtext_যোগ = 0;
	পূর্ণসংখ্যা		new_সারি_len;
	char_u	*গুনতি_dest;
	char_u	*vtext_dest;
	পূর্ণসংখ্যা		j;

	// Compute total vtext size from existing props.
	জন্য (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * আকার(textprop_T),
							আকার(textprop_T));
	    যদি (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_যোগ += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	যদি (text_arg != ফাঁকা)
	    vtext_যোগ += দৈর্ঘ্য + 1;

	new_সারি_len = (পূর্ণসংখ্যা)textlen + (পূর্ণসংখ্যা)PROP_গুনতি_আকার
			+ new_propcount * (পূর্ণসংখ্যা)আকার(textprop_T)
			+ vtext_যোগ;
	newtext = alloc(new_সারি_len);
	যদি (newtext == ফাঁকা)
	    যাও theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_সারি_ptr, textlen);

	// Write prop_count.
	গুনতি_dest = newtext + textlen;
	mch_memmove(গুনতি_dest, &new_propcount, PROP_গুনতি_আকার);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = গুনতি_dest + PROP_গুনতি_আকার;
	যদি (i > 0)
	    mch_memmove(newprops, props, আকার(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	যদি (i < proplen)
	    mch_memmove(newprops + (i + 1) * আকার(textprop_T),
					props + i * আকার(textprop_T),
					আকার(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * আকার(textprop_T);
	জন্য (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    যদি (j == i)
		চালু;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * আকার(textprop_T),
		    আকার(textprop_T));
	    যদি (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_গুনতি_2 = (char_u *)props - PROP_গুনতি_আকার;
		char_u *old_vtext = old_গুনতি_2 + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - গুনতি_dest);
		mch_memmove(newprops + j * আকার(textprop_T), &ep,
			আকার(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	যদি (text_arg != ফাঁকা)
	{
	    পূর্ণসংখ্যা copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - গুনতি_dest);
	    vtext_dest += copysize;
	}
	অন্যথা
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * আকার(textprop_T), &tmp_prop,
							আকার(textprop_T));

	যদি (buf->b_ml.ml_flags & (ML_সারি_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_সারি_ptr);
	buf->b_ml.ml_সারি_ptr = newtext;
	buf->b_ml.ml_সারি_len = new_সারি_len;
	buf->b_ml.ml_flags |= ML_সারি_DIRTY;
    }

    changed_সারি_display_buf(buf);
    changed_সারিগুলি_buf(buf, start_lnum, end_lnum + 1, 0);
    res = OK;

theend:
    vim_free(text);
    ফেরত res;
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
    পূর্ণসংখ্যা		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    পূর্ণসংখ্যা		error = FALSE;
    পূর্ণসংখ্যা		পূর্ব_did_emsg = did_emsg;

    যদি (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	ফেরত;

    যদি (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ফেরত;

    dict = argvars[0].vval.v_dict;
    যদি (dict == ফাঁকা || !dict_has_চাবি(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ফেরত;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    যদি (dict_has_চাবি(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_সংখ্যা(dict, "id");
	যদি (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ফেরত;
	}
	id = (পূর্ণসংখ্যা)x;
    }

    যদি (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ফেরত;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_বস্তুসমূহ(argvars[1].vval.v_list, li)
    {
	যদি (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == ফাঁকা)
	{
	    emsg(_(e_list_required));
	    ফেরত;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	যদি (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	যদি (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	যদি (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	পূর্ণসংখ্যা this_id = id;
	যদি (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	যদি (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    যদি (পূর্ব_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    ফেরত;
	}
	যদি (prop_add_one(buf, type_name, this_id, ফাঁকা, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    ফেরত;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
স্থির পূর্ণসংখ্যা vt_id_গণক = 0;

/*
 * Get the next ID to use for a textprop.
 */
    স্থির পূর্ণসংখ্যা
get_textprop_id(শূন্য)
{
    যদি (vt_id_গণক > 0)
	vt_id_গণক = 0;
    ফেরত --vt_id_গণক;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    পূর্ণসংখ্যা
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
    পূর্ণসংখ্যা		id = 0;
    char_u	*text = ফাঁকা;
    পূর্ণসংখ্যা		text_padding_বাম = 0;
    পূর্ণসংখ্যা		flags = 0;

    যদি (dict == ফাঁকা || !dict_has_চাবি(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	যাও theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    যদি (dict_has_চাবি(dict, "end_lnum"))
    {
	end_lnum = dict_get_সংখ্যা(dict, "end_lnum");
	যদি (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_মান_for_argument_str), "end_lnum");
	    যাও theend;
	}
    }
    অন্যথা
	end_lnum = start_lnum;

    যদি (dict_has_চাবি(dict, "length"))
    {
	long দৈর্ঘ্য = dict_get_সংখ্যা(dict, "length");

	যদি (দৈর্ঘ্য < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_মান_for_argument_str), "length");
	    যাও theend;
	}
	end_col = start_col + দৈর্ঘ্য;
    }
    অন্যথা যদি (dict_has_চাবি(dict, "end_col"))
    {
	end_col = dict_get_সংখ্যা(dict, "end_col");
	যদি (end_col <= 0)
	{
	    semsg(_(e_invalid_মান_for_argument_str), "end_col");
	    যাও theend;
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
	    যাও theend;
	}
	id = (পূর্ণসংখ্যা)x;
    }

    যদি (dict_has_চাবি(dict, "text"))
    {
	যদি (dict_has_চাবি(dict, "length")
		|| dict_has_চাবি(dict, "end_col")
		|| dict_has_চাবি(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_দৈর্ঘ্য_endcol_and_endlnum_with_text));
	    যাও theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	যদি (text == ফাঁকা)
	    যাও theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	যদি (dict_has_চাবি(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    যদি (p == ফাঁকা)
		যাও theend;
	    যদি (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		যাও theend;
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
		যাও theend;
	    }
	}

	যদি (dict_has_চাবি(dict, "text_padding_left"))
	{
	    text_padding_বাম = dict_get_সংখ্যা(dict, "text_padding_left");
	    যদি (text_padding_বাম < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		যাও theend;
	    }
	}

	যদি (dict_has_চাবি(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    যদি (p == ফাঁকা)
		যাও theend;
	    যদি (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    অন্যথা যদি (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_মান_for_argument_str_str), "text_wrap", p);
		যাও theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    যদি (start_col < (text == ফাঁকা ? 1 : 0))
    {
	semsg(_(e_invalid_column_সংখ্যা_nr), (long)start_col);
	যাও theend;
    }
    যদি (start_col > 0 && text_padding_বাম > 0)
    {
	emsg(_(e_can_only_use_বাম_padding_when_column_is_zero));
	যাও theend;
    }

    যদি (dict_arg != ফাঁকা && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	যাও theend;

    যদি (text != ফাঁকা)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    অন্যথা যদি (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	যাও theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_বাম, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = ফাঁকা;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    ফেরত id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    পূর্ণসংখ্যা
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, পূর্ণসংখ্যা will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_গুনতি;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    যদি ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == ফাঁকা)
	ফেরত 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_সারি_len - textlen;
    যদি (propdata_len == 0)
	ফেরত 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    যদি (propdata_len < PROP_গুনতি_আকার + আকার(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	ফেরত 0;
    }
    mch_memmove(&prop_গুনতি, text + textlen, PROP_গুনতি_আকার);
    যদি (!text_prop_গুনতি_valid(prop_গুনতি, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	ফেরত 0;
    }
    যদি (!text_prop_vtext_valid(text + textlen + PROP_গুনতি_আকার,
					   (পূর্ণসংখ্যা)prop_গুনতি, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	ফেরত 0;
    }
    *props = text + textlen + PROP_গুনতি_আকার;
    ফেরত (পূর্ণসংখ্যা)prop_গুনতি;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    পূর্ণসংখ্যা
prop_গুনতি_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    পূর্ণসংখ্যা		গুনতি = get_text_props(buf, lnum, &props, FALSE);
    পূর্ণসংখ্যা		ফলাফল = 0;
    textprop_T	prop;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		পরবর্তী_ডান_goes_below = FALSE;

    যদি (গুনতি == 0)
	ফেরত 0;
    জন্য (i = 0; i < গুনতি; ++i)
    {
	mch_memmove(&prop, props + i * আকার(prop), আকার(prop));
	যদি (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    যদি ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (পরবর্তী_ডান_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_ডান)))
	    {
		++ফলাফল;
	    }
	    অন্যথা যদি (prop.tp_flags & TP_FLAG_ALIGN_ডান)
		পরবর্তী_ডান_goes_below = TRUE;
	}
    }
    ফেরত ফলাফল;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    পূর্ণসংখ্যা
গুনতি_props(linenr_T lnum, পূর্ণসংখ্যা only_starting, পূর্ণসংখ্যা last_সারি)
{
    char_u	*props;
    পূর্ণসংখ্যা		proplen = get_text_props(curbuf, lnum, &props, 0);
    পূর্ণসংখ্যা		ফলাফল = proplen;
    textprop_T	prop;

    জন্য (পূর্ণসংখ্যা i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * আকার(prop), আকার(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	যদি ((only_starting && (prop.tp_flags & TP_FLAG_CONT_পূর্ব))
		|| (!last_সারি && prop.tp_col == MAXCOL))
	    --ফলাফল;
    }
    ফেরত ফলাফল;
}

স্থির textprop_T	*text_prop_compare_props;
স্থির buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    স্থির পূর্ণসংখ্যা
text_prop_order(পূর্ণসংখ্যা flags)
{
    যদি (flags & TP_FLAG_ALIGN_ABOVE)
	ফেরত 0;
    যদি (flags & TP_FLAG_ALIGN_ডান)
	ফেরত 2;
    যদি (flags & TP_FLAG_ALIGN_BELOW)
	ফেরত 3;
    ফেরত 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    স্থির পূর্ণসংখ্যা
text_prop_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    পূর্ণসংখ্যা  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(পূর্ণসংখ্যা *)s1;
    idx2 = *(পূর্ণসংখ্যা *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    যদি ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	ফেরত tp1->tp_id < 0 ? 1 : -1;

    যদি (col1 == MAXCOL || col2 == MAXCOL)
    {
	পূর্ণসংখ্যা order1 = text_prop_order(tp1->tp_flags);
	পূর্ণসংখ্যা order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	যদি (order1 != order2)
	    ফেরত order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    যদি (pt1 != pt2)
    {
	যদি (pt1 == ফাঁকা)
	    ফেরত -1;
	যদি (pt2 == ফাঁকা)
	    ফেরত 1;
	যদি (pt1->pt_priority != pt2->pt_priority)
	    ফেরত pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    যদি (col1 != col2)
	ফেরত col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    যদি (tp1->tp_id < 0)
	ফেরত tp1->tp_id > tp2->tp_id ? 1 : -1;

    ফেরত 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    শূন্য
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	পূর্ণসংখ্যা	    *idxs,
	পূর্ণসংখ্যা	    গুনতি)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((শূন্য *)idxs, (size_t)গুনতি, আকার(পূর্ণসংখ্যা), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_সারিগুলি(
	win_T	    *wp,
	পূর্ণসংখ্যা	    type_id,
	পূর্ণসংখ্যা	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    যদি (text_prop_type_by_id(wp->w_বাফার, type_id) == ফাঁকা)
	ফেরত false;

    যদি (first_lnum < 1)
	first_lnum = 1;
    যদি (last_lnum > wp->w_বাফার->b_ml.ml_সারি_গুনতি)
	last_lnum = wp->w_বাফার->b_ml.ml_সারি_গুনতি;

    জন্য (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	পূর্ণসংখ্যা	গুনতি = get_text_props(wp->w_বাফার, lnum, &props, FALSE);
	জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি; ++i)
	{
	    mch_memmove(prop, props + i * আকার(textprop_T),
							   আকার(textprop_T));
	    যদি (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		ফেরত true;
	    }
	}
    }
    ফেরত false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	পূর্ণসংখ্যা	    type_id,
	পূর্ণসংখ্যা	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    ফেরত find_prop_in_সারিগুলি(wp, type_id, id, prop, found_lnum,
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
set_text_props(linenr_T lnum, textprop_T *tps, পূর্ণসংখ্যা গুনতি)
{
    char_u  *text;
    char_u  *newtext;
    পূর্ণসংখ্যা	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    যদি (গুনতি == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	যদি (newtext == ফাঁকা)
	    ফেরত;
	mch_memmove(newtext, text, textlen);
	যদি (curbuf->b_ml.ml_flags & (ML_সারি_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_সারি_ptr);
	curbuf->b_ml.ml_সারি_ptr = newtext;
	curbuf->b_ml.ml_সারি_len = textlen;
	curbuf->b_ml.ml_flags |= ML_সারি_DIRTY;
    }
    অন্যথা
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_গুনতি = (uint16_t)গুনতি;
	পূর্ণসংখ্যা	    vtext_আকার = 0;
	পূর্ণসংখ্যা	    যোগ_len;
	char_u	    *গুনতি_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	পূর্ণসংখ্যা	    i;

	জন্য (i = 0; i < গুনতি; ++i)
	    যদি (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_আকার += tps[i].tp_len + 1;

	যোগ_len = textlen + (পূর্ণসংখ্যা)PROP_গুনতি_আকার
		    + গুনতি * (পূর্ণসংখ্যা)আকার(textprop_T) + vtext_আকার;
	newtext = alloc(যোগ_len);
	যদি (newtext == ফাঁকা)
	    ফেরত;
	mch_memmove(newtext, text, textlen);

	গুনতি_dest = newtext + textlen;
	mch_memmove(গুনতি_dest, &prop_গুনতি, PROP_গুনতি_আকার);

	prop_dest = গুনতি_dest + PROP_গুনতি_আকার;
	vtext_dest = prop_dest + গুনতি * আকার(textprop_T);

	জন্য (i = 0; i < গুনতি; ++i)
	{
	    textprop_T prop = tps[i];

	    যদি (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		পূর্ণসংখ্যা copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = ফাঁকা;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - গুনতি_dest);
		vtext_dest += copysize;
	    }
	    অন্যথা
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, আকার(textprop_T));
	    prop_dest += আকার(textprop_T);
	}

	যদি (curbuf->b_ml.ml_flags & (ML_সারি_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_সারি_ptr);
	curbuf->b_ml.ml_সারি_ptr = newtext;
	curbuf->b_ml.ml_সারি_len = যোগ_len;
	curbuf->b_ml.ml_flags |= ML_সারি_DIRTY;
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
props_add_গুনতি_header(char_u *সারি, পূর্ণসংখ্যা সারি_len, পূর্ণসংখ্যা textlen, পূর্ণসংখ্যা *new_len)
{
    পূর্ণসংখ্যা		prop_bytes = সারি_len - textlen;
    uint16_t	prop_গুনতি;
    পূর্ণসংখ্যা		vtext_আকার = 0;
    পূর্ণসংখ্যা		যোগ;
    char_u	*newline;
    char_u	*গুনতি_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    পূর্ণসংখ্যা		i;

    যদি (prop_bytes <= 0 || prop_bytes % আকার(textprop_T) != 0)
    {
	*new_len = সারি_len;
	ফেরত ফাঁকা;
    }
    prop_গুনতি = (uint16_t)(prop_bytes / আকার(textprop_T));

    // Calculate total vtext size.
    জন্য (i = 0; i < (পূর্ণসংখ্যা)prop_গুনতি; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, সারি + textlen + i * আকার(textprop_T),
							আকার(textprop_T));
	যদি (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_আকার += prop.tp_len + 1;
    }

    যোগ = textlen + (পূর্ণসংখ্যা)PROP_গুনতি_আকার + prop_bytes + vtext_আকার;
    newline = alloc(যোগ);
    যদি (newline == ফাঁকা)
	ফেরত ফাঁকা;
    *new_len = যোগ;

    mch_memmove(newline, সারি, textlen);
    গুনতি_dest = newline + textlen;
    mch_memmove(গুনতি_dest, &prop_গুনতি, PROP_গুনতি_আকার);
    prop_dest = গুনতি_dest + PROP_গুনতি_আকার;
    vtext_dest = prop_dest + prop_bytes;

    জন্য (i = 0; i < (পূর্ণসংখ্যা)prop_গুনতি; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, সারি + textlen + i * আকার(textprop_T),
							আকার(textprop_T));
	যদি (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    পূর্ণসংখ্যা copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - গুনতি_dest);
	    vtext_dest += copysize;
	}
	অন্যথা
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * আকার(textprop_T), &prop,
							আকার(textprop_T));
    }
    ফেরত newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    শূন্য
add_text_props(linenr_T lnum, textprop_T *text_props, পূর্ণসংখ্যা text_prop_গুনতি)
{
    unpacked_memline_T	um;
    পূর্ণসংখ্যা			i;

    um = um_open_at(curbuf, lnum, text_prop_গুনতি);
    যদি (um.buf == ফাঁকা)
	ফেরত;
    যদি (!um_detach(&um))
    {
	um_abort(&um);
	ফেরত;
    }

    // Grow props array if needed.
    যদি (um.prop_গুনতি + text_prop_গুনতি > um.prop_আকার)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_গুনতি + text_prop_গুনতি) * আকার(textprop_T));
	যদি (newprops == ফাঁকা)
	{
	    um_abort(&um);
	    ফেরত;
	}
	um.props = newprops;
	um.prop_আকার = um.prop_গুনতি + text_prop_গুনতি;
    }

    জন্য (i = 0; i < text_prop_গুনতি; ++i)
	um.props[um.prop_গুনতি++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    স্থির পূর্ণসংখ্যা
compare_pt(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    ফেরত tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    স্থির proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, পূর্ণসংখ্যা id)
{
    পূর্ণসংখ্যা নিচের = 0;
    পূর্ণসংখ্যা উপরের;

    যদি (ht == ফাঁকা || ht->ht_used == 0)
	ফেরত ফাঁকা;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    যদি (*array == ফাঁকা)
    {
	long	    todo;
	hashitem_T  *hi;
	পূর্ণসংখ্যা	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	যদি (*array == ফাঁকা)
	    ফেরত ফাঁকা;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
	{
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((শূন্য *)*array, ht->ht_used, আকার(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    উপরের = ht->ht_used;
    যখন (উপরের > নিচের)
    {
	পূর্ণসংখ্যা m = (উপরের + নিচের) / 2;

	যদি ((*array)[m]->pt_id == id)
	    ফেরত (*array)[m];
	যদি ((*array)[m]->pt_id > id)
	    উপরের = m;
	অন্যথা
	    নিচের = m + 1;
    }
    ফেরত ফাঁকা;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    স্থির শূন্য
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    পূর্ণসংখ্যা buflocal = TRUE;
    পূর্ণসংখ্যা virtualtext_prop = prop->tp_id < 0;

    dict_add_সংখ্যা(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    যদি (!virtualtext_prop)
    {
	dict_add_সংখ্যা(dict, "length", prop->tp_len);
	dict_add_সংখ্যা(dict, "id", prop->tp_id);
    }
    dict_add_সংখ্যা(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_পূর্ব));
    dict_add_সংখ্যা(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_পরবর্তী));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    যদি (pt == ফাঁকা)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    যদি (pt != ফাঁকা)
	dict_add_string(dict, "type", pt->pt_name);

    যদি (buflocal)
	dict_add_সংখ্যা(dict, "type_bufnr", buf->b_fnum);
    অন্যথা
	dict_add_সংখ্যা(dict, "type_bufnr", 0);
    যদি (virtualtext_prop)
    {
	string_T    text_align = {ফাঁকা, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	যদি (prop->tp_flags & TP_FLAG_ALIGN_ডান)
	    STR_LITERAL_SET(text_align, "right");
	অন্যথা যদি (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	অন্যথা যদি (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	যদি (text_align.string != ফাঁকা)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (পূর্ণসংখ্যা)text_align.দৈর্ঘ্য);

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
text_prop_type_by_id(buf_T *buf, পূর্ণসংখ্যা id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    যদি (type == ফাঁকা)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    ফেরত type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    ফেরত text_prop_type_by_id(buf, prop->tp_type) != ফাঁকা;
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
    পূর্ণসংখ্যা	    did_clear = FALSE;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ফেরত;

    start = tv_get_সংখ্যা(&argvars[0]);
    end = start;
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_সংখ্যা(&argvars[1]);
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    যদি (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		ফেরত;
	}
    }
    যদি (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	ফেরত;
    }

    {
	unpacked_memline_T um = um_open(buf);

	জন্য (lnum = start; lnum <= end; ++lnum)
	{
	    পূর্ণসংখ্যা idx;

	    যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
		বিরতি;
	    যদি (!um_goto_সারি(&um, lnum, 0))
		বিরতি;
	    যদি (um.prop_গুনতি > 0)
	    {
		did_clear = TRUE;
		জন্য (idx = um.prop_গুনতি - 1; idx >= 0; --idx)
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
    পূর্ণসংখ্যা		lnum_start;
    পূর্ণসংখ্যা		start_pos_has_prop = 0;
    পূর্ণসংখ্যা		seen_end = FALSE;
    পূর্ণসংখ্যা		id = 0;
    পূর্ণসংখ্যা		id_found = FALSE;
    পূর্ণসংখ্যা		type_id = -1;
    পূর্ণসংখ্যা		skipstart = FALSE;
    পূর্ণসংখ্যা		lnum = -1;
    পূর্ণসংখ্যা		col = -1;
    পূর্ণসংখ্যা		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    পূর্ণসংখ্যা		both;

    যদি (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ফেরত;
    dict = argvars[0].vval.v_dict;

    যদি (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ফেরত;
    যদি (buf->b_ml.ml_mfp == ফাঁকা)
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	যদি (*dir_s == 'b')
	    dir = BACKWARD;
	অন্যথা যদি (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    ফেরত;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    যদি (di != ফাঁকা)
	lnum = tv_get_সংখ্যা(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    যদি (di != ফাঁকা)
	col = tv_get_সংখ্যা(&di->di_tv);

    যদি (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    অন্যথা যদি (col == -1)
	col = 1;

    যদি (lnum < 1 || lnum > buf->b_ml.ml_সারি_গুনতি)
    {
	emsg(_(e_invalid_range));
	ফেরত;
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

	যদি (type == ফাঁকা)
	    ফেরত;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    যদি (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	ফেরত;
    }
    যদি (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ফেরত;
    }

    lnum_start = lnum;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;

    যখন (1)
    {
	char_u	*prop_start_ptr;
	পূর্ণসংখ্যা	গুনতি = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	পূর্ণসংখ্যা	    i;
	textprop_T  prop;
	পূর্ণসংখ্যা	    prop_start;
	পূর্ণসংখ্যা	    prop_end;

	জন্য (i = dir == BACKWARD ? গুনতি - 1 : 0; i >= 0 && i < গুনতি; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * আকার(textprop_T),
							   আকার(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    যদি (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_গুনতি_আকার)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    অন্যথা
		prop.u.tp_text = ফাঁকা;

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
			চালু;
		}
		অন্যথা যদি (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    চালু;
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
		prop_start = !(prop.tp_flags & TP_FLAG_CONT_পূর্ব);
		// The property does not continue on the next line, it ends on
		// this line.
		prop_end = !(prop.tp_flags & TP_FLAG_CONT_পরবর্তী);
		যদি (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		যদি (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    যদি (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    চালু;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		যদি (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    চালু;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_সংখ্যা(rettv->vval.v_dict, "lnum", lnum);

		ফেরত;
	    }
	}

	যদি (dir > 0)
	{
	    যদি (lnum >= buf->b_ml.ml_সারি_গুনতি)
		বিরতি;
	    lnum++;
	}
	অন্যথা
	{
	    যদি (lnum <= 1)
		বিরতি;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    স্থির পূর্ণসংখ্যা
prop_type_or_id_in_list(পূর্ণসংখ্যা *types_or_ids, পূর্ণসংখ্যা len, পূর্ণসংখ্যা type_or_id)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < len; i++)
	যদি (types_or_ids[i] == type_or_id)
	    ফেরত TRUE;

    ফেরত FALSE;
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
get_props_in_সারি(
	buf_T		*buf,
	linenr_T	lnum,
	পূর্ণসংখ্যা		*prop_types,
	পূর্ণসংখ্যা		prop_types_len,
	পূর্ণসংখ্যা		*prop_ids,
	পূর্ণসংখ্যা		prop_ids_len,
	list_T		*retlist,
	পূর্ণসংখ্যা		add_lnum)
{
    char_u	*props;
    পূর্ণসংখ্যা		গুনতি;
    পূর্ণসংখ্যা		i;
    textprop_T	prop;

    গুনতি = get_text_props(buf, lnum, &props, FALSE);
    জন্য (i = 0; i < গুনতি; ++i)
    {
	mch_memmove(&prop, props + i * আকার(textprop_T),
		আকার(textprop_T));
	// Convert offset to pointer for virtual text props.
	যদি (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_গুনতি_আকার)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	অন্যথা
	    prop.u.tp_text = ফাঁকা;
	যদি ((prop_types == ফাঁকা
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == ফাঁকা
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    যদি (d == ফাঁকা)
		বিরতি;
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
    স্থির পূর্ণসংখ্যা *
get_prop_types_from_names(list_T *l, buf_T *buf, পূর্ণসংখ্যা *অঙ্ক_types)
{
    পূর্ণসংখ্যা		*prop_types;
    listitem_T	*li;
    পূর্ণসংখ্যা		i;
    char_u	*name;
    proptype_T	*type;

    *অঙ্ক_types = 0;

    prop_types = ALLOC_MULT(পূর্ণসংখ্যা, list_len(l));
    যদি (prop_types == ফাঁকা)
	ফেরত ফাঁকা;

    i = 0;
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	যদি (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    যাও errret;
	}
	name = li->li_tv.vval.v_string;
	যদি (name == ফাঁকা)
	    যাও errret;

	type = lookup_prop_type(name, buf);
	যদি (type == ফাঁকা)
	    যাও errret;
	prop_types[i++] = type->pt_id;
    }

    *অঙ্ক_types = i;
    ফেরত prop_types;

errret:
    VIM_CLEAR(prop_types);
    ফেরত ফাঁকা;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    স্থির পূর্ণসংখ্যা *
get_prop_ids_from_list(list_T *l, পূর্ণসংখ্যা *অঙ্ক_ids)
{
    পূর্ণসংখ্যা		*prop_ids;
    listitem_T	*li;
    পূর্ণসংখ্যা		i = 0;
    পূর্ণসংখ্যা		id;
    পূর্ণসংখ্যা		error;

    *অঙ্ক_ids = 0;

    prop_ids = ALLOC_MULT(পূর্ণসংখ্যা, list_len(l));
    যদি (prop_ids == ফাঁকা)
	ফেরত ফাঁকা;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	error = FALSE;
	id = tv_get_সংখ্যা_chk(&li->li_tv, &error);
	যদি (error)
	    যাও errret;

	prop_ids[i++] = id;
    }

    *অঙ্ক_ids = i;
    ফেরত prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    ফেরত ফাঁকা;
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
    পূর্ণসংখ্যা		add_lnum = FALSE;
    পূর্ণসংখ্যা		*prop_types = ফাঁকা;
    পূর্ণসংখ্যা		prop_types_len = 0;
    পূর্ণসংখ্যা		*prop_ids = ফাঁকা;
    পূর্ণসংখ্যা		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    // default: get text properties on current line
    start_lnum = tv_get_সংখ্যা(&argvars[0]);
    end_lnum = start_lnum;
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	যদি (check_for_dict_arg(argvars, 1) == FAIL)
	    ফেরত;
	d = argvars[1].vval.v_dict;

	যদি (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ফেরত;

	যদি (d != ফাঁকা && (di = dict_find(d, (char_u *)"end_lnum", -1)) != ফাঁকা)
	{
	    যদি (di->di_tv.v_type != VAR_সংখ্যা)
	    {
		emsg(_(e_সংখ্যা_required));
		ফেরত;
	    }
	    end_lnum = tv_get_সংখ্যা(&di->di_tv);
	    যদি (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//সারি
		end_lnum = buf->b_ml.ml_সারি_গুনতি + end_lnum + 1;
	    অন্যথা যদি (end_lnum > buf->b_ml.ml_সারি_গুনতি)
		end_lnum = buf->b_ml.ml_সারি_গুনতি;
	    add_lnum = TRUE;
	}
	যদি (d != ফাঁকা && (di = dict_find(d, (char_u *)"types", -1)) != ফাঁকা)
	{
	    যদি (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ফেরত;
	    }

	    l = di->di_tv.vval.v_list;
	    যদি (l != ফাঁকা && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		যদি (prop_types == ফাঁকা)
		    ফেরত;
	    }
	}
	যদি (d != ফাঁকা && (di = dict_find(d, (char_u *)"ids", -1)) != ফাঁকা)
	{
	    যদি (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		যাও errret;
	    }

	    l = di->di_tv.vval.v_list;
	    যদি (l != ফাঁকা && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		যদি (prop_ids == ফাঁকা)
		    যাও errret;
	    }
	}
    }
    যদি (start_lnum < 1 || start_lnum > buf->b_ml.ml_সারি_গুনতি
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    অন্যথা
	জন্য (lnum = start_lnum; lnum <= end_lnum; lnum++)
	    get_props_in_সারি(buf, lnum, prop_types, prop_types_len,
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
    পূর্ণসংখ্যা		do_all;
    পূর্ণসংখ্যা		id = -MAXCOL;
    পূর্ণসংখ্যা		type_id = -1;	    // for a single "type"
    পূর্ণসংখ্যা		*type_ids = ফাঁকা;   // array, for a list of "types", allocated
    পূর্ণসংখ্যা		অঙ্ক_type_ids = 0;   // number of elements in "type_ids"
    পূর্ণসংখ্যা		both;

    rettv->vval.v_সংখ্যা = 0;

    যদি (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL)))
	ফেরত;

    যদি (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_সংখ্যা(&argvars[1]);
	end = start;
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_সংখ্যা(&argvars[2]);
	যদি (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    ফেরত;
	}
    }

    dict = argvars[0].vval.v_dict;
    যদি (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ফেরত;
    যদি (buf->b_ml.ml_mfp == ফাঁকা)
	ফেরত;

    do_all = dict_get_bool(dict, "all", FALSE);

    যদি (dict_has_চাবি(dict, "id"))
	id = dict_get_সংখ্যা(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    যদি (dict_has_চাবি(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	যদি (type == ফাঁকা)
	    ফেরত;
	type_id = type->pt_id;
    }
    যদি (dict_has_চাবি(dict, "types"))
    {
	typval_T types;
	listitem_T *li = ফাঁকা;

	dict_get_tv(dict, "types", &types);
	যদি (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( আকার(পূর্ণসংখ্যা) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_বস্তুসমূহ(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		যদি (li->li_tv.v_type != VAR_STRING)
		    চালু;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		যদি (!prop_type)
		    যাও cleanup_prop_remove;

		type_ids[অঙ্ক_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    যদি (id == -MAXCOL && (type_id == -1 && অঙ্ক_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	যাও cleanup_prop_remove;
    }
    যদি (both && (id == -MAXCOL || (type_id == -1 && অঙ্ক_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	যাও cleanup_prop_remove;
    }
    যদি (type_id != -1 && অঙ্ক_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	যাও cleanup_prop_remove;
    }

    যদি (end == 0)
	end = buf->b_ml.ml_সারি_গুনতি;

    {
	unpacked_memline_T um = um_open(buf);

	জন্য (lnum = start; lnum <= end; ++lnum)
	{
	    পূর্ণসংখ্যা idx;

	    যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
		বিরতি;
	    যদি (!um_goto_সারি(&um, lnum, 0))
		বিরতি;

	    জন্য (idx = 0; idx < um.prop_গুনতি; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		পূর্ণসংখ্যা	    matches_id = 0;
		পূর্ণসংখ্যা	    matches_type = 0;

		যদি (prop->tp_flags & TP_FLAG_DELETED)
		    চালু;

		matches_id = prop->tp_id == id;
		যদি (অঙ্ক_type_ids > 0)
		{
		    পূর্ণসংখ্যা idx2;

		    জন্য (idx2 = 0; !matches_type && idx2 < অঙ্ক_type_ids;
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
			বিরতি;
		}
	    }
	}
	um_close(&um);
    }

    যদি (first_changed > 0)
    {
	changed_সারি_display_buf(buf);
	changed_সারিগুলি_buf(buf, first_changed, last_changed + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    স্থির শূন্য
prop_type_set(typval_T *argvars, পূর্ণসংখ্যা add)
{
    char_u	*name;
    buf_T	*buf = ফাঁকা;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    name = tv_get_string(&argvars[0]);
    যদি (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ফেরত;
    }

    যদি (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	ফেরত;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    যদি (add)
    {
	hashtab_T **htp;

	যদি (prop != ফাঁকা)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    ফেরত;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	যদি (prop == ফাঁকা)
	    ফেরত;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	যদি (buf == ফাঁকা)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	অন্যথা
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	যদি (*htp == ফাঁকা)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    যদি (*htp == ফাঁকা)
	    {
		vim_free(prop);
		ফেরত;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    অন্যথা
    {
	যদি (prop == ফাঁকা)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    ফেরত;
	}
    }

    যদি (dict != ফাঁকা)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	যদি (di != ফাঁকা)
	{
	    char_u	*highlight;
	    পূর্ণসংখ্যা		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    যদি (highlight != ফাঁকা && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    যদি (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == ফাঁকা ? (char_u *)"" : highlight);
		ফেরত;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	যদি (di != ফাঁকা)
	{
	    যদি (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    অন্যথা
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	যদি (di != ফাঁকা)
	{
	    যদি (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    অন্যথা
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	যদি (di != ফাঁকা)
	    prop->pt_priority = tv_get_সংখ্যা(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	যদি (di != ফাঁকা)
	{
	    যদি (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    অন্যথা
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	যদি (di != ফাঁকা)
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
    buf_T	*buf = ফাঁকা;
    hashitem_T	*hi;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    name = tv_get_string(&argvars[0]);
    যদি (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ফেরত;
    }

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	যদি (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ফেরত;
    }

    hi = find_prop_type_hi(name, buf);
    যদি (hi == ফাঁকা)
	ফেরত;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    যদি (buf == ফাঁকা)
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
    changed_window_setting_buf(buf == ফাঁকা ? curbuf : buf);
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
	ফেরত;

    name = tv_get_string(&argvars[0]);
    যদি (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ফেরত;
    }

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;

    proptype_T  *prop = ফাঁকা;
    buf_T	    *buf = ফাঁকা;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	যদি (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ফেরত;
    }

    prop = find_prop_type(name, buf);
    যদি (prop == ফাঁকা)
	ফেরত;

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
    যদি (buf != ফাঁকা)
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
    buf_T *buf = ফাঁকা;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	যদি (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    ফেরত;
    }
    যদি (buf == ফাঁকা)
    {
	যদি (global_proptypes != ফাঁকা)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    অন্যথা যদি (buf->b_proptypes != ফাঁকা)
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

    যদি (ht == ফাঁকা)
	ফেরত;

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
    global_proptypes = ফাঁকা;
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
    buf->b_proptypes = ফাঁকা;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
প্রকারলেখ কাঠামো
{
    পূর্ণসংখ্যা dirty;	    // if the property was changed
    পূর্ণসংখ্যা can_drop;   // whether after this change, the prop may be removed
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
	পূর্ণসংখ্যা	    added,
	পূর্ণসংখ্যা	    flags)
{
    proptype_T	*pt;
    পূর্ণসংখ্যা		start_incl;
    পূর্ণসংখ্যা		end_incl;
    পূর্ণসংখ্যা		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    যদি (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	ফেরত res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != ফাঁকা && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_পূর্ব);
    যদি (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != ফাঁকা && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_পরবর্তী);
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
				& (TP_FLAG_CONT_পূর্ব | TP_FLAG_CONT_পরবর্তী));
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
	    পূর্ণসংখ্যা after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	যদি (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_পরবর্তী)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_পূর্ব | TP_FLAG_CONT_পরবর্তী));
    }
    অন্যথা
	res.dirty = FALSE;

    ফেরত res;
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
    পূর্ণসংখ্যা
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	পূর্ণসংখ্যা	    bytes_added,
	পূর্ণসংখ্যা	    flags)
{
    unpacked_memline_T um;
    পূর্ণসংখ্যা		dirty = FALSE;
    পূর্ণসংখ্যা		ri;

    যদি (text_prop_frozen > 0)
	ফেরত FALSE;

    um = um_open_at(curbuf, lnum, 0);
    যদি (um.buf == ফাঁকা || um.prop_গুনতি == 0)
    {
	um_abort(&um);
	ফেরত FALSE;
    }

    জন্য (ri = 0; ri < um.prop_গুনতি; ++ri)
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
		    ফেরত FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		যদি ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    ফেরত FALSE;
		}
	    }
	    dirty = TRUE;
	}
	যদি (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    চালু;
	}
    }
    যদি (dirty)
	um_close(&um);
    অন্যথা
	um_abort(&um);
    ফেরত dirty;
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
	linenr_T    lnum_শীর্ষ,
	পূর্ণসংখ্যা	    kept,
	পূর্ণসংখ্যা	    deleted,
	পূর্ণসংখ্যা	    at_eol)
{
    char_u	*props;
    পূর্ণসংখ্যা		গুনতি;
    garray_T    prevprop;
    garray_T    nextprop;
    পূর্ণসংখ্যা		skipped = kept + deleted;

    যদি (!curbuf->b_has_textprop)
	ফেরত;

    // Get the text properties from "lnum_props".
    গুনতি = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, আকার(textprop_T), 10);
    ga_init2(&nextprop, আকার(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *গুনতি_ptr = props - PROP_গুনতি_আকার;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	পূর্ণসংখ্যা	    start_incl, end_incl;
	পূর্ণসংখ্যা	    cont_পূর্ব, cont_পরবর্তী;
	পূর্ণসংখ্যা	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * আকার(textprop_T), আকার(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	যদি (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(গুনতি_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	অন্যথা
	    prop.u.tp_text = ফাঁকা;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != ফাঁকা && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != ফাঁকা && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	যদি (prop_col == MAXCOL)
	{
	    cont_পূর্ব = at_eol;
	    cont_পরবর্তী = !at_eol;
	}
	অন্যথা
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    পূর্ণসংখ্যা is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_পূর্ব = prop_col + !start_incl <= kept;
	    যদি (is_floating_vtext)
		cont_পরবর্তী = FALSE;
	    অন্যথা
		cont_পরবর্তী = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	যদি (prop.tp_id < 0 && cont_পরবর্তী)
	    cont_পূর্ব = FALSE;

	যদি (cont_পূর্ব && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_তথ্য) + prevprop.ga_len;
	    পূর্ণসংখ্যা is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    যদি (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    যদি (cont_পরবর্তী)
		p->tp_flags |= TP_FLAG_CONT_পরবর্তী;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	যদি (cont_পরবর্তী && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_তথ্য) + nextprop.ga_len;
	    পূর্ণসংখ্যা is_vtext = (prop.tp_id < 0);

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
	    যদি (cont_পূর্ব)
		p->tp_flags |= TP_FLAG_CONT_পূর্ব;
	}
    }

    set_text_props(lnum_শীর্ষ, (textprop_T *)prevprop.ga_তথ্য,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    set_text_props(lnum_শীর্ষ + 1, (textprop_T *)nextprop.ga_তথ্য,
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
	পূর্ণসংখ্যা		    last_সারি,
	long		    col,
	পূর্ণসংখ্যা		    removed)
{
    unpacked_memline_T	r_um;

    যদি (um->buf == ফাঁকা)
	ফেরত;

    r_um = um_open_at(um->buf, lnum, 0);
    যদি (r_um.buf == ফাঁকা)
	ফেরত;
    যদি (!um_detach(&r_um))
    {
	um_abort(&r_um);
	ফেরত;
    }

    জন্য (পূর্ণসংখ্যা i = r_um.prop_গুনতি - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	পূর্ণসংখ্যা	    end;

	যদি (prop->tp_col == MAXCOL && !last_সারি)
	    চালু;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_পরবর্তী);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	যদি (last_সারি || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = ফাঁকা;  // ownership transferred
	}
	অন্যথা
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    জন্য (পূর্ণসংখ্যা j = 0; j < um->prop_গুনতি; ++j)
	    {
		textprop_T *op = &um->props[j];

		যদি ((op->tp_flags & TP_FLAG_CONT_পূর্ব)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    বিরতি;
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
remember_cleared_prop(garray_T *gap, পূর্ণসংখ্যা which, textprop_T *prop, পূর্ণসংখ্যা flag)
{
    undoprop_T	*up;

    যদি (which == UNDOPROP_NONE)
	ফেরত;
    যদি (gap->ga_itemsize == 0)
	ga_init2(gap, আকার(undoprop_T), 4);
    যদি (ga_grow(gap, 1) == FAIL)
	ফেরত;
    up = (undoprop_T *)gap->ga_তথ্য + gap->ga_len++;
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
	পূর্ণসংখ্যা	    above_which,
	পূর্ণসংখ্যা	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = ফাঁকা;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_পরবর্তী = false;

    যদি (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_সারি_গুনতি)
	ফেরত;

    above = um_open_at(buf, lnum, 0);
    যদি (above.buf == ফাঁকা)
	ফেরত;
    জন্য (পূর্ণসংখ্যা i = 0; i < above.prop_গুনতি; ++i)
	যদি (above.props[i].tp_flags & TP_FLAG_CONT_পরবর্তী)
	    cont_পরবর্তী = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    যদি (cont_পরবর্তী && !um_detach(&above))
    {
	um_abort(&above);
	ফেরত;
    }

    below = um_open_at(buf, lnum + 1, 0);
    যদি (below.buf == ফাঁকা)
    {
	um_abort(&above);
	ফেরত;
    }
    যদি (below.prop_গুনতি > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_গুনতি);
	যদি (paired == ফাঁকা)
	    যাও theend;
    }

    জন্য (পূর্ণসংখ্যা i = 0; i < above.prop_গুনতি; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	যদি (!(prop->tp_flags & TP_FLAG_CONT_পরবর্তী))
	    চালু;
	জন্য (পূর্ণসংখ্যা j = 0; j < below.prop_গুনতি; ++j)
	{
	    textprop_T *পরবর্তী = &below.props[j];

	    যদি (!paired[j] && (পরবর্তী->tp_flags & TP_FLAG_CONT_পূর্ব)
		    && পরবর্তী->tp_id == prop->tp_id
		    && পরবর্তী->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		বিরতি;
	    }
	}
	যদি (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_পরবর্তী;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_পরবর্তী);
	}
    }

    জন্য (পূর্ণসংখ্যা j = 0; j < below.prop_গুনতি; ++j)
    {
	যদি (!(below.props[j].tp_flags & TP_FLAG_CONT_পূর্ব) || paired[j])
	    চালু;
	যদি (!below.detached && !um_detach(&below))
	    বিরতি;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_পূর্ব;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_পূর্ব);
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
	পূর্ণসংখ্যা	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    যদি (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_সারি_গুনতি)
	ফেরত;
    um = um_open_at(buf, lnum, 0);
    যদি (um.buf == ফাঁকা)
	ফেরত;
    জন্য (পূর্ণসংখ্যা r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_তথ্য + r;

	যদি (up->up_which != which)
	    চালু;
	জন্য (পূর্ণসংখ্যা i = 0; i < um.prop_গুনতি; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    যদি (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		চালু;
	    যদি (!um.detached && !um_detach(&um))
		যাও theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    বিরতি;
	}
    }

theend:
    যদি (changed)
	um_close(&um);
    অন্যথা
	um_abort(&um);
}

    bool
text_prop_গুনতি_valid(পূর্ণসংখ্যা prop_গুনতি, size_t propdata_len)
{
    যদি (propdata_len < PROP_গুনতি_আকার)
	ফেরত false;
    ফেরত (size_t)prop_গুনতি * আকার(textprop_T)
		    <= propdata_len - PROP_গুনতি_আকার;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, পূর্ণসংখ্যা prop_গুনতি, size_t propdata_len)
{
    জন্য (পূর্ণসংখ্যা i = 0; i < prop_গুনতি; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * আকার(textprop_T),
							  আকার(textprop_T));
	যদি (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    চালু;
	যদি (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    ফেরত false;
    }
    ফেরত true;
}

#endif // FEAT_PROP_POPUP
