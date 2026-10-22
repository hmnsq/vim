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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_PROP_POPUP)

സ്ഥിതം ശൂന്യം um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    സ്ഥിതം ശൂന്യം
um_free_vtext(textprop_T *props, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; ++i)
	എങ്കിൽ (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    സ്ഥിതം ശൂന്യം
um_free(unpacked_memline_T *um)
{
    എങ്കിൽ (um->detached)
    {
	VIM_CLEAR(um->text);
	um_free_vtext(um->props, um->prop_എണ്ണം);
    }
    VIM_CLEAR(um->props);
    um->prop_size = 0;
    um->prop_എണ്ണം = 0;
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
    തിരികെ um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_വരി(unpacked_memline_T *um, linenr_T lnum, പൂർണ്ണസംഖ്യ extra_props)
{
    char_u	*വരി;
    size_t	textlen;
    size_t	propdata_len;
    പൂർണ്ണസംഖ്യ		proplen;

    എങ്കിൽ (um->buf == ശൂന്യ)
	തിരികെ false;

    // Store changes to the current line if any.
    എങ്കിൽ (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    എങ്കിൽ (lnum == 0)
	തിരികെ true;	// just unload

    വരി = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_വരി_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = വരി;

    എങ്കിൽ (propdata_len == 0)
	തിരികെ true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    എങ്കിൽ (propdata_len < PROP_എണ്ണം_SIZE + വലുപ്പം(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ശൂന്യ;
	തിരികെ false;
    }

    uint16_t    prop_എണ്ണം;
    char_u	    *എണ്ണം_ptr = വരി + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_എണ്ണം, എണ്ണം_ptr, PROP_എണ്ണം_SIZE);
    എങ്കിൽ (!text_prop_എണ്ണം_valid(prop_എണ്ണം, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ശൂന്യ;
	തിരികെ false;
    }
    proplen = (പൂർണ്ണസംഖ്യ)prop_എണ്ണം;
    props_start = എണ്ണം_ptr + PROP_എണ്ണം_SIZE;

    എങ്കിൽ (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ശൂന്യ;
	തിരികെ false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    എങ്കിൽ (um->props == ശൂന്യ)
    {
	um->buf = ശൂന്യ;
	തിരികെ false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_എണ്ണം = proplen;
    mch_memmove(um->props, props_start, proplen * വലുപ്പം(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < proplen; ++i)
    {
	എങ്കിൽ (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = എണ്ണം_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	അല്ലെങ്കിൽ
	    um->props[i].u.tp_text = ശൂന്യ;
    }

    തിരികെ true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, പൂർണ്ണസംഖ്യ extra_props)
{
    unpacked_memline_T um = um_open(buf);

    എങ്കിൽ (!um_goto_വരി(&um, lnum, extra_props))
	um.buf = ശൂന്യ;
    തിരികെ um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    സ്ഥിതം bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    എങ്കിൽ (um->detached || um->buf == ശൂന്യ)
	തിരികെ um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    എങ്കിൽ (newtext == ശൂന്യ)
    {
	um->buf = ശൂന്യ;
	തിരികെ false;
    }
    um->text = newtext;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < um->prop_എണ്ണം; ++i)
    {
	എങ്കിൽ (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    എങ്കിൽ (copy == ശൂന്യ)
	    {
		വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < i; ++j)
		    എങ്കിൽ (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = ശൂന്യ;
		തിരികെ false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    തിരികെ true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    എങ്കിൽ (um->buf == ശൂന്യ)
	തിരികെ false;
    എങ്കിൽ (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    തിരികെ true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    സ്ഥിതം bool
um_grow_props(unpacked_memline_T *um, പൂർണ്ണസംഖ്യ needed)
{
    എങ്കിൽ (um->prop_എണ്ണം + needed <= um->prop_size)
	തിരികെ true;

    പൂർണ്ണസംഖ്യ	    new_size = um->prop_എണ്ണം + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * വലുപ്പം(textprop_T));
    എങ്കിൽ (newprops == ശൂന്യ)
	തിരികെ false;
    um->props = newprops;
    um->prop_size = new_size;

    തിരികെ true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    സ്ഥിതം ശൂന്യം
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    എങ്കിൽ (um->buf == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (!um->detached && !um_detach(um))
	തിരികെ;
    എങ്കിൽ (!um_grow_props(um, 1))
    {
	um->buf = ശൂന്യ;
	തിരികെ;
    }
    um->props[um->prop_എണ്ണം++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    ശൂന്യം
um_reverse_props(unpacked_memline_T *um)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0, j = um->prop_എണ്ണം - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, പൂർണ്ണസംഖ്യ prop_എണ്ണം)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    എങ്കിൽ (lnum < 1 || lnum > buf->b_ml.ml_വരി_എണ്ണം)
    {
	um.buf = ശൂന്യ;
	തിരികെ um;
    }

    char_u *വരി = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(വരി);

    എങ്കിൽ (text_copy == ശൂന്യ)
    {
	um.buf = ശൂന്യ;
	തിരികെ um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    എങ്കിൽ (prop_എണ്ണം > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_എണ്ണം);
	എങ്കിൽ (um.props == ശൂന്യ)
	{
	    vim_free(um.text);
	    um.buf = ശൂന്യ;
	    തിരികെ um;
	}
	um.prop_size = prop_എണ്ണം;
    }

    തിരികെ um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    സ്ഥിതം ശൂന്യം
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					പൂർണ്ണസംഖ്യ tp_id, പൂർണ്ണസംഖ്യ tp_type, പൂർണ്ണസംഖ്യ flag)
{
    unpacked_memline_T	neighbor;

    എങ്കിൽ (lnum < 1 || lnum > buf->b_ml.ml_വരി_എണ്ണം)
	തിരികെ;

    neighbor = um_open_at(buf, lnum, 0);
    എങ്കിൽ (neighbor.buf == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < neighbor.prop_എണ്ണം; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	എങ്കിൽ (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    എങ്കിൽ (!neighbor.detached && !um_detach(&neighbor))
		നിർത്തുക;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    നിർത്തുക;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    ശൂന്യം
um_delete_prop(unpacked_memline_T *um, പൂർണ്ണസംഖ്യ index)
{
    textprop_T *prop;

    എങ്കിൽ (um->buf == ശൂന്യ || index < 0 || index >= um->prop_എണ്ണം)
	തിരികെ;
    എങ്കിൽ (!um->detached && !um_detach(um))
	തിരികെ;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    എങ്കിൽ (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    എങ്കിൽ (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    എങ്കിൽ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
    സ്ഥിതം char_u *
um_pack(unpacked_memline_T *um, പൂർണ്ണസംഖ്യ *packed_len)
{
    uint16_t	live_എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		vtext_size = 0;
    പൂർണ്ണസംഖ്യ		total_len;
    char_u	*buf;
    char_u	*എണ്ണം_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < um->prop_എണ്ണം; ++i)
    {
	textprop_T *prop = &um->props[i];

	എങ്കിൽ (prop->tp_flags & TP_FLAG_DELETED)
	    തുടരുക;
	++live_എണ്ണം;
	എങ്കിൽ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    എങ്കിൽ (live_എണ്ണം == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	എങ്കിൽ (buf == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	തിരികെ buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (പൂർണ്ണസംഖ്യ)PROP_എണ്ണം_SIZE
		+ live_എണ്ണം * (പൂർണ്ണസംഖ്യ)വലുപ്പം(textprop_T) + vtext_size;
    buf = alloc(total_len);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ ശൂന്യ;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    എണ്ണം_dest = buf + um->text_size;
    mch_memmove(എണ്ണം_dest, &live_എണ്ണം, PROP_എണ്ണം_SIZE);

    prop_dest = എണ്ണം_dest + PROP_എണ്ണം_SIZE;
    vtext_dest = prop_dest + live_എണ്ണം * വലുപ്പം(textprop_T);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < um->prop_എണ്ണം; ++i)
    {
	textprop_T prop = um->props[i];

	എങ്കിൽ (prop.tp_flags & TP_FLAG_DELETED)
	    തുടരുക;

	എങ്കിൽ (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    പൂർണ്ണസംഖ്യ copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - എണ്ണം_dest);
	    vtext_dest += copysize;
	}
	അല്ലെങ്കിൽ
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, വലുപ്പം(textprop_T));
	prop_dest += വലുപ്പം(textprop_T);
    }

    തിരികെ buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    സ്ഥിതം ശൂന്യം
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    പൂർണ്ണസംഖ്യ	    packed_len;

    എങ്കിൽ (!um->detached || um->buf == ശൂന്യ || um->lnum == 0)
	തിരികെ;

    packed = um_pack(um, &packed_len);
    എങ്കിൽ (packed == ശൂന്യ)
    {
	um->buf = ശൂന്യ;
	തിരികെ;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    എങ്കിൽ (um->buf->b_ml.ml_വരി_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    അല്ലെങ്കിൽ എങ്കിൽ (!(um->buf->b_ml.ml_flags & ML_വരി_DIRTY))
	(ശൂന്യം)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_എണ്ണം);

    എങ്കിൽ (um->buf->b_ml.ml_flags & (ML_വരി_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_വരി_ptr);
    um->buf->b_ml.ml_വരി_ptr = packed;
    um->buf->b_ml.ml_വരി_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_വരി_DIRTY;

    um->detached = false;
    um->text = packed;
    um->text_size = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_വരി_textlen = um->text_size;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    ശൂന്യം
um_close(unpacked_memline_T *um)
{
    എങ്കിൽ (um->buf == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = ശൂന്യ;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    ശൂന്യം
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = ശൂന്യ;
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
#define HI2PT(hi)      HIKEY2PT((hi)->hi_താക്കോൽ)

// The global text property types.
സ്ഥിതം hashtab_T *global_proptypes = ശൂന്യ;
സ്ഥിതം proptype_T **global_proparray = ശൂന്യ;

// The last used text property type ID.
സ്ഥിതം പൂർണ്ണസംഖ്യ proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    സ്ഥിതം hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    എങ്കിൽ (*name == NUL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (buf == ശൂന്യ)
	ht = global_proptypes;
    അല്ലെങ്കിൽ
	ht = buf->b_proptypes;

    എങ്കിൽ (ht == ശൂന്യ)
	തിരികെ ശൂന്യ;
    hi = hash_find(ht, name);
    എങ്കിൽ (HASHITEM_EMPTY(hi))
	തിരികെ ശൂന്യ;
    തിരികെ hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    സ്ഥിതം proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    എങ്കിൽ (hi == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    പൂർണ്ണസംഖ്യ
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    എങ്കിൽ (pt == ശൂന്യ)
	തിരികെ 0;
    തിരികെ pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    സ്ഥിതം proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = find_prop_type(name, buf);

    എങ്കിൽ (type == ശൂന്യ)
	type = find_prop_type(name, ശൂന്യ);
    എങ്കിൽ (type == ശൂന്യ)
	semsg(_(e_property_type_str_does_not_exist), name);
    തിരികെ type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    എങ്കിൽ (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	തിരികെ FAIL;
    }
    എങ്കിൽ (arg->vval.v_dict == ശൂന്യ)
	തിരികെ OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    എങ്കിൽ (di != ശൂന്യ && (di->di_tv.v_type != VAR_സംഖ്യ
					      || di->di_tv.vval.v_സംഖ്യ != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	എങ്കിൽ (*buf == ശൂന്യ)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    ശൂന്യം
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	തിരികെ;

    start_lnum = tv_get_സംഖ്യ(&argvars[0]);
    start_col = tv_get_സംഖ്യ(&argvars[1]);
    എങ്കിൽ (check_for_dict_arg(argvars, 2) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	പൂർണ്ണസംഖ്യ		id,
	char_u		*text_arg,
	പൂർണ്ണസംഖ്യ		text_padding_ഇടത്,
	പൂർണ്ണസംഖ്യ		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		proplen;
    char_u	*props = ശൂന്യ;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    പൂർണ്ണസംഖ്യ		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    പൂർണ്ണസംഖ്യ		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    എങ്കിൽ (type == ശൂന്യ)
	പോകുക theend;

    എങ്കിൽ (start_lnum < 1 || start_lnum > buf->b_ml.ml_വരി_എണ്ണം)
    {
	semsg(_(e_invalid_വരി_സംഖ്യ_nr), (long)start_lnum);
	പോകുക theend;
    }
    എങ്കിൽ (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_വരി_എണ്ണം)
    {
	semsg(_(e_invalid_വരി_സംഖ്യ_nr), (long)end_lnum);
	പോകുക theend;
    }

    എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	പോകുക theend;
    }

    എങ്കിൽ (text != ശൂന്യ)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	വേണ്ടി (p = text; *p != NUL; MB_PTR_ADV(p))
	    എങ്കിൽ (*p < ' ')
		*p = ' ';
    }

    വേണ്ടി (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	എങ്കിൽ (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_വരി));
	    പോകുക theend;
	}

	എങ്കിൽ (lnum == start_lnum)
	    col = start_col;
	അല്ലെങ്കിൽ
	    col = 1;
	എങ്കിൽ (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != ശൂന്യ))
	{
	    semsg(_(e_invalid_column_സംഖ്യ_nr), (long)start_col);
	    പോകുക theend;
	}
	sort_col = col;

	എങ്കിൽ (lnum == end_lnum)
	    length = end_col - col;
	അല്ലെങ്കിൽ
	    length = (പൂർണ്ണസംഖ്യ)textlen - col + 1;
	എങ്കിൽ (length > (long)textlen)
	    length = (പൂർണ്ണസംഖ്യ)textlen;	// can include the end-of-line
	എങ്കിൽ (length < 0)
	    length = 0;		// zero-width property

	എങ്കിൽ (text_arg != ശൂന്യ)
	{
	    length = (long)STRLEN(text_arg);
	    എങ്കിൽ (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		എങ്കിൽ ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	tmp_prop.tp_padleft = text_padding_ഇടത്;
	എങ്കിൽ (text_arg != ശൂന്യ)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	വേണ്ടി (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * വലുപ്പം(textprop_T),
							   വലുപ്പം(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    എങ്കിൽ (prop_col >= sort_col)
		നിർത്തുക;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	പൂർണ്ണസംഖ്യ		vtext_total = 0;
	പൂർണ്ണസംഖ്യ		new_വരി_len;
	char_u	*എണ്ണം_dest;
	char_u	*vtext_dest;
	പൂർണ്ണസംഖ്യ		j;

	// Compute total vtext size from existing props.
	വേണ്ടി (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * വലുപ്പം(textprop_T),
							വലുപ്പം(textprop_T));
	    എങ്കിൽ (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	എങ്കിൽ (text_arg != ശൂന്യ)
	    vtext_total += length + 1;

	new_വരി_len = (പൂർണ്ണസംഖ്യ)textlen + (പൂർണ്ണസംഖ്യ)PROP_എണ്ണം_SIZE
			+ new_propcount * (പൂർണ്ണസംഖ്യ)വലുപ്പം(textprop_T)
			+ vtext_total;
	newtext = alloc(new_വരി_len);
	എങ്കിൽ (newtext == ശൂന്യ)
	    പോകുക theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_വരി_ptr, textlen);

	// Write prop_count.
	എണ്ണം_dest = newtext + textlen;
	mch_memmove(എണ്ണം_dest, &new_propcount, PROP_എണ്ണം_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = എണ്ണം_dest + PROP_എണ്ണം_SIZE;
	എങ്കിൽ (i > 0)
	    mch_memmove(newprops, props, വലുപ്പം(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	എങ്കിൽ (i < proplen)
	    mch_memmove(newprops + (i + 1) * വലുപ്പം(textprop_T),
					props + i * വലുപ്പം(textprop_T),
					വലുപ്പം(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * വലുപ്പം(textprop_T);
	വേണ്ടി (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    എങ്കിൽ (j == i)
		തുടരുക;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * വലുപ്പം(textprop_T),
		    വലുപ്പം(textprop_T));
	    എങ്കിൽ (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_എണ്ണം_2 = (char_u *)props - PROP_എണ്ണം_SIZE;
		char_u *old_vtext = old_എണ്ണം_2 + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - എണ്ണം_dest);
		mch_memmove(newprops + j * വലുപ്പം(textprop_T), &ep,
			വലുപ്പം(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	എങ്കിൽ (text_arg != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - എണ്ണം_dest);
	    vtext_dest += copysize;
	}
	അല്ലെങ്കിൽ
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * വലുപ്പം(textprop_T), &tmp_prop,
							വലുപ്പം(textprop_T));

	എങ്കിൽ (buf->b_ml.ml_flags & (ML_വരി_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_വരി_ptr);
	buf->b_ml.ml_വരി_ptr = newtext;
	buf->b_ml.ml_വരി_len = new_വരി_len;
	buf->b_ml.ml_flags |= ML_വരി_DIRTY;
    }

    changed_വരി_display_buf(buf);
    changed_വരികൾ_buf(buf, start_lnum, end_lnum + 1, 0);
    res = OK;

theend:
    vim_free(text);
    തിരികെ res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    ശൂന്യം
f_prop_add_list(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    പൂർണ്ണസംഖ്യ		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    പൂർണ്ണസംഖ്യ		prev_did_emsg = did_emsg;

    എങ്കിൽ (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	തിരികെ;

    dict = argvars[0].vval.v_dict;
    എങ്കിൽ (dict == ശൂന്യ || !dict_has_താക്കോൽ(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	തിരികെ;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    എങ്കിൽ (dict_has_താക്കോൽ(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_സംഖ്യ(dict, "id");
	എങ്കിൽ (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    തിരികെ;
	}
	id = (പൂർണ്ണസംഖ്യ)x;
    }

    എങ്കിൽ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	തിരികെ;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_ഇനങ്ങൾ(argvars[1].vval.v_list, li)
    {
	എങ്കിൽ (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == ശൂന്യ)
	{
	    emsg(_(e_list_required));
	    തിരികെ;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	എങ്കിൽ (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	എങ്കിൽ (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	എങ്കിൽ (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	പൂർണ്ണസംഖ്യ this_id = id;
	എങ്കിൽ (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	എങ്കിൽ (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    എങ്കിൽ (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    തിരികെ;
	}
	എങ്കിൽ (prop_add_one(buf, type_name, this_id, ശൂന്യ, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    തിരികെ;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
സ്ഥിതം പൂർണ്ണസംഖ്യ vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_textprop_id(ശൂന്യം)
{
    എങ്കിൽ (vt_id_counter > 0)
	vt_id_counter = 0;
    തിരികെ --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    പൂർണ്ണസംഖ്യ
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
    പൂർണ്ണസംഖ്യ		id = 0;
    char_u	*text = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		text_padding_ഇടത് = 0;
    പൂർണ്ണസംഖ്യ		flags = 0;

    എങ്കിൽ (dict == ശൂന്യ || !dict_has_താക്കോൽ(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	പോകുക theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    എങ്കിൽ (dict_has_താക്കോൽ(dict, "end_lnum"))
    {
	end_lnum = dict_get_സംഖ്യ(dict, "end_lnum");
	എങ്കിൽ (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str), "end_lnum");
	    പോകുക theend;
	}
    }
    അല്ലെങ്കിൽ
	end_lnum = start_lnum;

    എങ്കിൽ (dict_has_താക്കോൽ(dict, "length"))
    {
	long length = dict_get_സംഖ്യ(dict, "length");

	എങ്കിൽ (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str), "length");
	    പോകുക theend;
	}
	end_col = start_col + length;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (dict_has_താക്കോൽ(dict, "end_col"))
    {
	end_col = dict_get_സംഖ്യ(dict, "end_col");
	എങ്കിൽ (end_col <= 0)
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str), "end_col");
	    പോകുക theend;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (start_lnum == end_lnum)
	end_col = start_col;
    അല്ലെങ്കിൽ
	end_col = 1;

    എങ്കിൽ (dict_has_താക്കോൽ(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_സംഖ്യ(dict, "id");
	എങ്കിൽ (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    പോകുക theend;
	}
	id = (പൂർണ്ണസംഖ്യ)x;
    }

    എങ്കിൽ (dict_has_താക്കോൽ(dict, "text"))
    {
	എങ്കിൽ (dict_has_താക്കോൽ(dict, "length")
		|| dict_has_താക്കോൽ(dict, "end_col")
		|| dict_has_താക്കോൽ(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    പോകുക theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	എങ്കിൽ (text == ശൂന്യ)
	    പോകുക theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	എങ്കിൽ (dict_has_താക്കോൽ(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    എങ്കിൽ (p == ശൂന്യ)
		പോകുക theend;
	    എങ്കിൽ (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		പോകുക theend;
	    }
	    എങ്കിൽ (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_വലത്;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "text_align", p);
		പോകുക theend;
	    }
	}

	എങ്കിൽ (dict_has_താക്കോൽ(dict, "text_padding_left"))
	{
	    text_padding_ഇടത് = dict_get_സംഖ്യ(dict, "text_padding_left");
	    എങ്കിൽ (text_padding_ഇടത് < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		പോകുക theend;
	    }
	}

	എങ്കിൽ (dict_has_താക്കോൽ(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    എങ്കിൽ (p == ശൂന്യ)
		പോകുക theend;
	    എങ്കിൽ (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "text_wrap", p);
		പോകുക theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    എങ്കിൽ (start_col < (text == ശൂന്യ ? 1 : 0))
    {
	semsg(_(e_invalid_column_സംഖ്യ_nr), (long)start_col);
	പോകുക theend;
    }
    എങ്കിൽ (start_col > 0 && text_padding_ഇടത് > 0)
    {
	emsg(_(e_can_only_use_ഇടത്_padding_when_column_is_zero));
	പോകുക theend;
    }

    എങ്കിൽ (dict_arg != ശൂന്യ && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	പോകുക theend;

    എങ്കിൽ (text != ശൂന്യ)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    അല്ലെങ്കിൽ എങ്കിൽ (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	പോകുക theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_ഇടത്, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = ശൂന്യ;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    തിരികെ id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    പൂർണ്ണസംഖ്യ
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, പൂർണ്ണസംഖ്യ will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_എണ്ണം;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    എങ്കിൽ ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == ശൂന്യ)
	തിരികെ 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_വരി_len - textlen;
    എങ്കിൽ (propdata_len == 0)
	തിരികെ 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    എങ്കിൽ (propdata_len < PROP_എണ്ണം_SIZE + വലുപ്പം(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	തിരികെ 0;
    }
    mch_memmove(&prop_എണ്ണം, text + textlen, PROP_എണ്ണം_SIZE);
    എങ്കിൽ (!text_prop_എണ്ണം_valid(prop_എണ്ണം, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	തിരികെ 0;
    }
    എങ്കിൽ (!text_prop_vtext_valid(text + textlen + PROP_എണ്ണം_SIZE,
					   (പൂർണ്ണസംഖ്യ)prop_എണ്ണം, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	തിരികെ 0;
    }
    *props = text + textlen + PROP_എണ്ണം_SIZE;
    തിരികെ (പൂർണ്ണസംഖ്യ)prop_എണ്ണം;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    പൂർണ്ണസംഖ്യ
prop_എണ്ണം_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    പൂർണ്ണസംഖ്യ		എണ്ണം = get_text_props(buf, lnum, &props, FALSE);
    പൂർണ്ണസംഖ്യ		result = 0;
    textprop_T	prop;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		next_വലത്_goes_below = FALSE;

    എങ്കിൽ (എണ്ണം == 0)
	തിരികെ 0;
    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
    {
	mch_memmove(&prop, props + i * വലുപ്പം(prop), വലുപ്പം(prop));
	എങ്കിൽ (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    എങ്കിൽ ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_വലത്_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_വലത്)))
	    {
		++result;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (prop.tp_flags & TP_FLAG_ALIGN_വലത്)
		next_വലത്_goes_below = TRUE;
	}
    }
    തിരികെ result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    പൂർണ്ണസംഖ്യ
എണ്ണം_props(linenr_T lnum, പൂർണ്ണസംഖ്യ only_starting, പൂർണ്ണസംഖ്യ last_വരി)
{
    char_u	*props;
    പൂർണ്ണസംഖ്യ		proplen = get_text_props(curbuf, lnum, &props, 0);
    പൂർണ്ണസംഖ്യ		result = proplen;
    textprop_T	prop;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * വലുപ്പം(prop), വലുപ്പം(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	എങ്കിൽ ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_വരി && prop.tp_col == MAXCOL))
	    --result;
    }
    തിരികെ result;
}

സ്ഥിതം textprop_T	*text_prop_compare_props;
സ്ഥിതം buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
text_prop_order(പൂർണ്ണസംഖ്യ flags)
{
    എങ്കിൽ (flags & TP_FLAG_ALIGN_ABOVE)
	തിരികെ 0;
    എങ്കിൽ (flags & TP_FLAG_ALIGN_വലത്)
	തിരികെ 2;
    എങ്കിൽ (flags & TP_FLAG_ALIGN_BELOW)
	തിരികെ 3;
    തിരികെ 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
text_prop_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    പൂർണ്ണസംഖ്യ  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(പൂർണ്ണസംഖ്യ *)s1;
    idx2 = *(പൂർണ്ണസംഖ്യ *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    എങ്കിൽ ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	തിരികെ tp1->tp_id < 0 ? 1 : -1;

    എങ്കിൽ (col1 == MAXCOL || col2 == MAXCOL)
    {
	പൂർണ്ണസംഖ്യ order1 = text_prop_order(tp1->tp_flags);
	പൂർണ്ണസംഖ്യ order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	എങ്കിൽ (order1 != order2)
	    തിരികെ order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    എങ്കിൽ (pt1 != pt2)
    {
	എങ്കിൽ (pt1 == ശൂന്യ)
	    തിരികെ -1;
	എങ്കിൽ (pt2 == ശൂന്യ)
	    തിരികെ 1;
	എങ്കിൽ (pt1->pt_priority != pt2->pt_priority)
	    തിരികെ pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    എങ്കിൽ (col1 != col2)
	തിരികെ col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    എങ്കിൽ (tp1->tp_id < 0)
	തിരികെ tp1->tp_id > tp2->tp_id ? 1 : -1;

    തിരികെ 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    ശൂന്യം
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	പൂർണ്ണസംഖ്യ	    *idxs,
	പൂർണ്ണസംഖ്യ	    എണ്ണം)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((ശൂന്യം *)idxs, (size_t)എണ്ണം, വലുപ്പം(പൂർണ്ണസംഖ്യ), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_വരികൾ(
	win_T	    *wp,
	പൂർണ്ണസംഖ്യ	    type_id,
	പൂർണ്ണസംഖ്യ	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    എങ്കിൽ (text_prop_type_by_id(wp->w_buffer, type_id) == ശൂന്യ)
	തിരികെ false;

    എങ്കിൽ (first_lnum < 1)
	first_lnum = 1;
    എങ്കിൽ (last_lnum > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	last_lnum = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;

    വേണ്ടി (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	പൂർണ്ണസംഖ്യ	എണ്ണം = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; ++i)
	{
	    mch_memmove(prop, props + i * വലുപ്പം(textprop_T),
							   വലുപ്പം(textprop_T));
	    എങ്കിൽ (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		തിരികെ true;
	    }
	}
    }
    തിരികെ false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	പൂർണ്ണസംഖ്യ	    type_id,
	പൂർണ്ണസംഖ്യ	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    തിരികെ find_prop_in_വരികൾ(wp, type_id, id, prop, found_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    സ്ഥിതം ശൂന്യം
set_text_props(linenr_T lnum, textprop_T *tps, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    char_u  *text;
    char_u  *newtext;
    പൂർണ്ണസംഖ്യ	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    എങ്കിൽ (എണ്ണം == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	എങ്കിൽ (newtext == ശൂന്യ)
	    തിരികെ;
	mch_memmove(newtext, text, textlen);
	എങ്കിൽ (curbuf->b_ml.ml_flags & (ML_വരി_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_വരി_ptr);
	curbuf->b_ml.ml_വരി_ptr = newtext;
	curbuf->b_ml.ml_വരി_len = textlen;
	curbuf->b_ml.ml_flags |= ML_വരി_DIRTY;
    }
    അല്ലെങ്കിൽ
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_എണ്ണം = (uint16_t)എണ്ണം;
	പൂർണ്ണസംഖ്യ	    vtext_size = 0;
	പൂർണ്ണസംഖ്യ	    total_len;
	char_u	    *എണ്ണം_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	പൂർണ്ണസംഖ്യ	    i;

	വേണ്ടി (i = 0; i < എണ്ണം; ++i)
	    എങ്കിൽ (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (പൂർണ്ണസംഖ്യ)PROP_എണ്ണം_SIZE
		    + എണ്ണം * (പൂർണ്ണസംഖ്യ)വലുപ്പം(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	എങ്കിൽ (newtext == ശൂന്യ)
	    തിരികെ;
	mch_memmove(newtext, text, textlen);

	എണ്ണം_dest = newtext + textlen;
	mch_memmove(എണ്ണം_dest, &prop_എണ്ണം, PROP_എണ്ണം_SIZE);

	prop_dest = എണ്ണം_dest + PROP_എണ്ണം_SIZE;
	vtext_dest = prop_dest + എണ്ണം * വലുപ്പം(textprop_T);

	വേണ്ടി (i = 0; i < എണ്ണം; ++i)
	{
	    textprop_T prop = tps[i];

	    എങ്കിൽ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		പൂർണ്ണസംഖ്യ copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = ശൂന്യ;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - എണ്ണം_dest);
		vtext_dest += copysize;
	    }
	    അല്ലെങ്കിൽ
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, വലുപ്പം(textprop_T));
	    prop_dest += വലുപ്പം(textprop_T);
	}

	എങ്കിൽ (curbuf->b_ml.ml_flags & (ML_വരി_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_വരി_ptr);
	curbuf->b_ml.ml_വരി_ptr = newtext;
	curbuf->b_ml.ml_വരി_len = total_len;
	curbuf->b_ml.ml_flags |= ML_വരി_DIRTY;
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
props_add_എണ്ണം_header(char_u *വരി, പൂർണ്ണസംഖ്യ വരി_len, പൂർണ്ണസംഖ്യ textlen, പൂർണ്ണസംഖ്യ *new_len)
{
    പൂർണ്ണസംഖ്യ		prop_bytes = വരി_len - textlen;
    uint16_t	prop_എണ്ണം;
    പൂർണ്ണസംഖ്യ		vtext_size = 0;
    പൂർണ്ണസംഖ്യ		total;
    char_u	*newline;
    char_u	*എണ്ണം_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (prop_bytes <= 0 || prop_bytes % വലുപ്പം(textprop_T) != 0)
    {
	*new_len = വരി_len;
	തിരികെ ശൂന്യ;
    }
    prop_എണ്ണം = (uint16_t)(prop_bytes / വലുപ്പം(textprop_T));

    // Calculate total vtext size.
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)prop_എണ്ണം; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, വരി + textlen + i * വലുപ്പം(textprop_T),
							വലുപ്പം(textprop_T));
	എങ്കിൽ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (പൂർണ്ണസംഖ്യ)PROP_എണ്ണം_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    എങ്കിൽ (newline == ശൂന്യ)
	തിരികെ ശൂന്യ;
    *new_len = total;

    mch_memmove(newline, വരി, textlen);
    എണ്ണം_dest = newline + textlen;
    mch_memmove(എണ്ണം_dest, &prop_എണ്ണം, PROP_എണ്ണം_SIZE);
    prop_dest = എണ്ണം_dest + PROP_എണ്ണം_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)prop_എണ്ണം; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, വരി + textlen + i * വലുപ്പം(textprop_T),
							വലുപ്പം(textprop_T));
	എങ്കിൽ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    പൂർണ്ണസംഖ്യ copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - എണ്ണം_dest);
	    vtext_dest += copysize;
	}
	അല്ലെങ്കിൽ
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * വലുപ്പം(textprop_T), &prop,
							വലുപ്പം(textprop_T));
    }
    തിരികെ newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    ശൂന്യം
add_text_props(linenr_T lnum, textprop_T *text_props, പൂർണ്ണസംഖ്യ text_prop_എണ്ണം)
{
    unpacked_memline_T	um;
    പൂർണ്ണസംഖ്യ			i;

    um = um_open_at(curbuf, lnum, text_prop_എണ്ണം);
    എങ്കിൽ (um.buf == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (!um_detach(&um))
    {
	um_abort(&um);
	തിരികെ;
    }

    // Grow props array if needed.
    എങ്കിൽ (um.prop_എണ്ണം + text_prop_എണ്ണം > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_എണ്ണം + text_prop_എണ്ണം) * വലുപ്പം(textprop_T));
	എങ്കിൽ (newprops == ശൂന്യ)
	{
	    um_abort(&um);
	    തിരികെ;
	}
	um.props = newprops;
	um.prop_size = um.prop_എണ്ണം + text_prop_എണ്ണം;
    }

    വേണ്ടി (i = 0; i < text_prop_എണ്ണം; ++i)
	um.props[um.prop_എണ്ണം++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compare_pt(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    തിരികെ tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    സ്ഥിതം proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, പൂർണ്ണസംഖ്യ id)
{
    പൂർണ്ണസംഖ്യ താഴെയുള്ള = 0;
    പൂർണ്ണസംഖ്യ മുകളിലെ;

    എങ്കിൽ (ht == ശൂന്യ || ht->ht_used == 0)
	തിരികെ ശൂന്യ;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    എങ്കിൽ (*array == ശൂന്യ)
    {
	long	    todo;
	hashitem_T  *hi;
	പൂർണ്ണസംഖ്യ	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	എങ്കിൽ (*array == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_ഇനങ്ങൾ(ht, hi, todo)
	{
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((ശൂന്യം *)*array, ht->ht_used, വലുപ്പം(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    മുകളിലെ = ht->ht_used;
    ആകുമ്പോൾ (മുകളിലെ > താഴെയുള്ള)
    {
	പൂർണ്ണസംഖ്യ m = (മുകളിലെ + താഴെയുള്ള) / 2;

	എങ്കിൽ ((*array)[m]->pt_id == id)
	    തിരികെ (*array)[m];
	എങ്കിൽ ((*array)[m]->pt_id > id)
	    മുകളിലെ = m;
	അല്ലെങ്കിൽ
	    താഴെയുള്ള = m + 1;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    സ്ഥിതം ശൂന്യം
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    പൂർണ്ണസംഖ്യ buflocal = TRUE;
    പൂർണ്ണസംഖ്യ virtualtext_prop = prop->tp_id < 0;

    dict_add_സംഖ്യ(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    എങ്കിൽ (!virtualtext_prop)
    {
	dict_add_സംഖ്യ(dict, "length", prop->tp_len);
	dict_add_സംഖ്യ(dict, "id", prop->tp_id);
    }
    dict_add_സംഖ്യ(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_സംഖ്യ(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    എങ്കിൽ (pt == ശൂന്യ)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    എങ്കിൽ (pt != ശൂന്യ)
	dict_add_string(dict, "type", pt->pt_name);

    എങ്കിൽ (buflocal)
	dict_add_സംഖ്യ(dict, "type_bufnr", buf->b_fnum);
    അല്ലെങ്കിൽ
	dict_add_സംഖ്യ(dict, "type_bufnr", 0);
    എങ്കിൽ (virtualtext_prop)
    {
	string_T    text_align = {ശൂന്യ, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	എങ്കിൽ (prop->tp_flags & TP_FLAG_ALIGN_വലത്)
	    STR_LITERAL_SET(text_align, "right");
	അല്ലെങ്കിൽ എങ്കിൽ (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	അല്ലെങ്കിൽ എങ്കിൽ (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	എങ്കിൽ (text_align.string != ശൂന്യ)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (പൂർണ്ണസംഖ്യ)text_align.length);

	// text_wrap
	എങ്കിൽ (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	എങ്കിൽ (prop->tp_padleft != 0)
	    dict_add_സംഖ്യ(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, പൂർണ്ണസംഖ്യ id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    എങ്കിൽ (type == ശൂന്യ)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    തിരികെ type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    തിരികെ text_prop_type_by_id(buf, prop->tp_type) != ശൂന്യ;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    ശൂന്യം
f_prop_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    linenr_T start;
    linenr_T end;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    പൂർണ്ണസംഖ്യ	    did_clear = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	തിരികെ;

    start = tv_get_സംഖ്യ(&argvars[0]);
    end = start;
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_സംഖ്യ(&argvars[1]);
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    എങ്കിൽ (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		തിരികെ;
	}
    }
    എങ്കിൽ (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	തിരികെ;
    }

    {
	unpacked_memline_T um = um_open(buf);

	വേണ്ടി (lnum = start; lnum <= end; ++lnum)
	{
	    പൂർണ്ണസംഖ്യ idx;

	    എങ്കിൽ (lnum > buf->b_ml.ml_വരി_എണ്ണം)
		നിർത്തുക;
	    എങ്കിൽ (!um_goto_വരി(&um, lnum, 0))
		നിർത്തുക;
	    എങ്കിൽ (um.prop_എണ്ണം > 0)
	    {
		did_clear = TRUE;
		വേണ്ടി (idx = um.prop_എണ്ണം - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    എങ്കിൽ (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    ശൂന്യം
f_prop_find(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    പൂർണ്ണസംഖ്യ		lnum_start;
    പൂർണ്ണസംഖ്യ		start_pos_has_prop = 0;
    പൂർണ്ണസംഖ്യ		seen_end = FALSE;
    പൂർണ്ണസംഖ്യ		id = 0;
    പൂർണ്ണസംഖ്യ		id_found = FALSE;
    പൂർണ്ണസംഖ്യ		type_id = -1;
    പൂർണ്ണസംഖ്യ		skipstart = FALSE;
    പൂർണ്ണസംഖ്യ		lnum = -1;
    പൂർണ്ണസംഖ്യ		col = -1;
    പൂർണ്ണസംഖ്യ		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    പൂർണ്ണസംഖ്യ		both;

    എങ്കിൽ (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	തിരികെ;
    dict = argvars[0].vval.v_dict;

    എങ്കിൽ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	തിരികെ;
    എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	എങ്കിൽ (*dir_s == 'b')
	    dir = BACKWARD;
	അല്ലെങ്കിൽ എങ്കിൽ (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    തിരികെ;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    എങ്കിൽ (di != ശൂന്യ)
	lnum = tv_get_സംഖ്യ(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    എങ്കിൽ (di != ശൂന്യ)
	col = tv_get_സംഖ്യ(&di->di_tv);

    എങ്കിൽ (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (col == -1)
	col = 1;

    എങ്കിൽ (lnum < 1 || lnum > buf->b_ml.ml_വരി_എണ്ണം)
    {
	emsg(_(e_invalid_range));
	തിരികെ;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    എങ്കിൽ (dict_has_താക്കോൽ(dict, "id"))
    {
	id = dict_get_സംഖ്യ(dict, "id");
	id_found = TRUE;
    }
    എങ്കിൽ (dict_has_താക്കോൽ(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	എങ്കിൽ (type == ശൂന്യ)
	    തിരികെ;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    എങ്കിൽ (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	തിരികെ;
    }
    എങ്കിൽ (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	തിരികെ;
    }

    lnum_start = lnum;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;

    ആകുമ്പോൾ (1)
    {
	char_u	*prop_start_ptr;
	പൂർണ്ണസംഖ്യ	എണ്ണം = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	പൂർണ്ണസംഖ്യ	    i;
	textprop_T  prop;
	പൂർണ്ണസംഖ്യ	    prop_start;
	പൂർണ്ണസംഖ്യ	    prop_end;

	വേണ്ടി (i = dir == BACKWARD ? എണ്ണം - 1 : 0; i >= 0 && i < എണ്ണം; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * വലുപ്പം(textprop_T),
							   വലുപ്പം(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    എങ്കിൽ (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_എണ്ണം_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    അല്ലെങ്കിൽ
		prop.u.tp_text = ശൂന്യ;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    എങ്കിൽ (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		എങ്കിൽ (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    എങ്കിൽ (!is_floating_vtext && prop.tp_col > col)
			തുടരുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    തുടരുക;
	    }
	    എങ്കിൽ (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		എങ്കിൽ (lnum_start == lnum
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
		എങ്കിൽ (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		എങ്കിൽ (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    എങ്കിൽ (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    തുടരുക;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		എങ്കിൽ (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    തുടരുക;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_സംഖ്യ(rettv->vval.v_dict, "lnum", lnum);

		തിരികെ;
	    }
	}

	എങ്കിൽ (dir > 0)
	{
	    എങ്കിൽ (lnum >= buf->b_ml.ml_വരി_എണ്ണം)
		നിർത്തുക;
	    lnum++;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (lnum <= 1)
		നിർത്തുക;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prop_type_or_id_in_list(പൂർണ്ണസംഖ്യ *types_or_ids, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ type_or_id)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < len; i++)
	എങ്കിൽ (types_or_ids[i] == type_or_id)
	    തിരികെ TRUE;

    തിരികെ FALSE;
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
    സ്ഥിതം ശൂന്യം
get_props_in_വരി(
	buf_T		*buf,
	linenr_T	lnum,
	പൂർണ്ണസംഖ്യ		*prop_types,
	പൂർണ്ണസംഖ്യ		prop_types_len,
	പൂർണ്ണസംഖ്യ		*prop_ids,
	പൂർണ്ണസംഖ്യ		prop_ids_len,
	list_T		*retlist,
	പൂർണ്ണസംഖ്യ		add_lnum)
{
    char_u	*props;
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    പൂർണ്ണസംഖ്യ		i;
    textprop_T	prop;

    എണ്ണം = get_text_props(buf, lnum, &props, FALSE);
    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
    {
	mch_memmove(&prop, props + i * വലുപ്പം(textprop_T),
		വലുപ്പം(textprop_T));
	// Convert offset to pointer for virtual text props.
	എങ്കിൽ (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_എണ്ണം_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	അല്ലെങ്കിൽ
	    prop.u.tp_text = ശൂന്യ;
	എങ്കിൽ ((prop_types == ശൂന്യ
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == ശൂന്യ
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    എങ്കിൽ (d == ശൂന്യ)
		നിർത്തുക;
	    prop_fill_dict(d, &prop, buf);
	    എങ്കിൽ (add_lnum)
		dict_add_സംഖ്യ(d, "lnum", lnum);
	    എങ്കിൽ (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ *
get_prop_types_from_names(list_T *l, buf_T *buf, പൂർണ്ണസംഖ്യ *അക്കം_types)
{
    പൂർണ്ണസംഖ്യ		*prop_types;
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*name;
    proptype_T	*type;

    *അക്കം_types = 0;

    prop_types = ALLOC_MULT(പൂർണ്ണസംഖ്യ, list_len(l));
    എങ്കിൽ (prop_types == ശൂന്യ)
	തിരികെ ശൂന്യ;

    i = 0;
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	എങ്കിൽ (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    പോകുക errret;
	}
	name = li->li_tv.vval.v_string;
	എങ്കിൽ (name == ശൂന്യ)
	    പോകുക errret;

	type = lookup_prop_type(name, buf);
	എങ്കിൽ (type == ശൂന്യ)
	    പോകുക errret;
	prop_types[i++] = type->pt_id;
    }

    *അക്കം_types = i;
    തിരികെ prop_types;

errret:
    VIM_CLEAR(prop_types);
    തിരികെ ശൂന്യ;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ *
get_prop_ids_from_list(list_T *l, പൂർണ്ണസംഖ്യ *അക്കം_ids)
{
    പൂർണ്ണസംഖ്യ		*prop_ids;
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		i = 0;
    പൂർണ്ണസംഖ്യ		id;
    പൂർണ്ണസംഖ്യ		error;

    *അക്കം_ids = 0;

    prop_ids = ALLOC_MULT(പൂർണ്ണസംഖ്യ, list_len(l));
    എങ്കിൽ (prop_ids == ശൂന്യ)
	തിരികെ ശൂന്യ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	error = FALSE;
	id = tv_get_സംഖ്യ_chk(&li->li_tv, &error);
	എങ്കിൽ (error)
	    പോകുക errret;

	prop_ids[i++] = id;
    }

    *അക്കം_ids = i;
    തിരികെ prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    തിരികെ ശൂന്യ;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    ശൂന്യം
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    buf_T	*buf = curbuf;
    പൂർണ്ണസംഖ്യ		add_lnum = FALSE;
    പൂർണ്ണസംഖ്യ		*prop_types = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		prop_types_len = 0;
    പൂർണ്ണസംഖ്യ		*prop_ids = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    // default: get text properties on current line
    start_lnum = tv_get_സംഖ്യ(&argvars[0]);
    end_lnum = start_lnum;
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	എങ്കിൽ (check_for_dict_arg(argvars, 1) == FAIL)
	    തിരികെ;
	d = argvars[1].vval.v_dict;

	എങ്കിൽ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    തിരികെ;

	എങ്കിൽ (d != ശൂന്യ && (di = dict_find(d, (char_u *)"end_lnum", -1)) != ശൂന്യ)
	{
	    എങ്കിൽ (di->di_tv.v_type != VAR_സംഖ്യ)
	    {
		emsg(_(e_സംഖ്യ_required));
		തിരികെ;
	    }
	    end_lnum = tv_get_സംഖ്യ(&di->di_tv);
	    എങ്കിൽ (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//വരി
		end_lnum = buf->b_ml.ml_വരി_എണ്ണം + end_lnum + 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (end_lnum > buf->b_ml.ml_വരി_എണ്ണം)
		end_lnum = buf->b_ml.ml_വരി_എണ്ണം;
	    add_lnum = TRUE;
	}
	എങ്കിൽ (d != ശൂന്യ && (di = dict_find(d, (char_u *)"types", -1)) != ശൂന്യ)
	{
	    എങ്കിൽ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		തിരികെ;
	    }

	    l = di->di_tv.vval.v_list;
	    എങ്കിൽ (l != ശൂന്യ && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		എങ്കിൽ (prop_types == ശൂന്യ)
		    തിരികെ;
	    }
	}
	എങ്കിൽ (d != ശൂന്യ && (di = dict_find(d, (char_u *)"ids", -1)) != ശൂന്യ)
	{
	    എങ്കിൽ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		പോകുക errret;
	    }

	    l = di->di_tv.vval.v_list;
	    എങ്കിൽ (l != ശൂന്യ && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		എങ്കിൽ (prop_ids == ശൂന്യ)
		    പോകുക errret;
	    }
	}
    }
    എങ്കിൽ (start_lnum < 1 || start_lnum > buf->b_ml.ml_വരി_എണ്ണം
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    അല്ലെങ്കിൽ
	വേണ്ടി (lnum = start_lnum; lnum <= end_lnum; lnum++)
	    get_props_in_വരി(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, add_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    ശൂന്യം
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start = 1;
    linenr_T	end = 0;
    linenr_T	lnum;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    പൂർണ്ണസംഖ്യ		do_all;
    പൂർണ്ണസംഖ്യ		id = -MAXCOL;
    പൂർണ്ണസംഖ്യ		type_id = -1;	    // for a single "type"
    പൂർണ്ണസംഖ്യ		*type_ids = ശൂന്യ;   // array, for a list of "types", allocated
    പൂർണ്ണസംഖ്യ		അക്കം_type_ids = 0;   // number of elements in "type_ids"
    പൂർണ്ണസംഖ്യ		both;

    rettv->vval.v_സംഖ്യ = 0;

    എങ്കിൽ (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL)))
	തിരികെ;

    എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_സംഖ്യ(&argvars[1]);
	end = start;
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_സംഖ്യ(&argvars[2]);
	എങ്കിൽ (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    തിരികെ;
	}
    }

    dict = argvars[0].vval.v_dict;
    എങ്കിൽ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	തിരികെ;
    എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ)
	തിരികെ;

    do_all = dict_get_bool(dict, "all", FALSE);

    എങ്കിൽ (dict_has_താക്കോൽ(dict, "id"))
	id = dict_get_സംഖ്യ(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    എങ്കിൽ (dict_has_താക്കോൽ(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	എങ്കിൽ (type == ശൂന്യ)
	    തിരികെ;
	type_id = type->pt_id;
    }
    എങ്കിൽ (dict_has_താക്കോൽ(dict, "types"))
    {
	typval_T types;
	listitem_T *li = ശൂന്യ;

	dict_get_tv(dict, "types", &types);
	എങ്കിൽ (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( വലുപ്പം(പൂർണ്ണസംഖ്യ) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_ഇനങ്ങൾ(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		എങ്കിൽ (li->li_tv.v_type != VAR_STRING)
		    തുടരുക;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		എങ്കിൽ (!prop_type)
		    പോകുക cleanup_prop_remove;

		type_ids[അക്കം_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    എങ്കിൽ (id == -MAXCOL && (type_id == -1 && അക്കം_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	പോകുക cleanup_prop_remove;
    }
    എങ്കിൽ (both && (id == -MAXCOL || (type_id == -1 && അക്കം_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	പോകുക cleanup_prop_remove;
    }
    എങ്കിൽ (type_id != -1 && അക്കം_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	പോകുക cleanup_prop_remove;
    }

    എങ്കിൽ (end == 0)
	end = buf->b_ml.ml_വരി_എണ്ണം;

    {
	unpacked_memline_T um = um_open(buf);

	വേണ്ടി (lnum = start; lnum <= end; ++lnum)
	{
	    പൂർണ്ണസംഖ്യ idx;

	    എങ്കിൽ (lnum > buf->b_ml.ml_വരി_എണ്ണം)
		നിർത്തുക;
	    എങ്കിൽ (!um_goto_വരി(&um, lnum, 0))
		നിർത്തുക;

	    വേണ്ടി (idx = 0; idx < um.prop_എണ്ണം; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		പൂർണ്ണസംഖ്യ	    matches_id = 0;
		പൂർണ്ണസംഖ്യ	    matches_type = 0;

		എങ്കിൽ (prop->tp_flags & TP_FLAG_DELETED)
		    തുടരുക;

		matches_id = prop->tp_id == id;
		എങ്കിൽ (അക്കം_type_ids > 0)
		{
		    പൂർണ്ണസംഖ്യ idx2;

		    വേണ്ടി (idx2 = 0; !matches_type && idx2 < അക്കം_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		അല്ലെങ്കിൽ
		    matches_type = prop->tp_type == type_id;

		എങ്കിൽ (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    എങ്കിൽ (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_സംഖ്യ;
		    എങ്കിൽ (!do_all)
			നിർത്തുക;
		}
	    }
	}
	um_close(&um);
    }

    എങ്കിൽ (first_changed > 0)
    {
	changed_വരി_display_buf(buf);
	changed_വരികൾ_buf(buf, first_changed, last_changed + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    സ്ഥിതം ശൂന്യം
prop_type_set(typval_T *argvars, പൂർണ്ണസംഖ്യ add)
{
    char_u	*name;
    buf_T	*buf = ശൂന്യ;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    name = tv_get_string(&argvars[0]);
    എങ്കിൽ (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	തിരികെ;
    }

    എങ്കിൽ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	തിരികെ;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    എങ്കിൽ (add)
    {
	hashtab_T **htp;

	എങ്കിൽ (prop != ശൂന്യ)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    തിരികെ;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	എങ്കിൽ (prop == ശൂന്യ)
	    തിരികെ;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	എങ്കിൽ (buf == ശൂന്യ)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	അല്ലെങ്കിൽ
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	എങ്കിൽ (*htp == ശൂന്യ)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    എങ്കിൽ (*htp == ശൂന്യ)
	    {
		vim_free(prop);
		തിരികെ;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (prop == ശൂന്യ)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    തിരികെ;
	}
    }

    എങ്കിൽ (dict != ശൂന്യ)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	എങ്കിൽ (di != ശൂന്യ)
	{
	    char_u	*highlight;
	    പൂർണ്ണസംഖ്യ		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    എങ്കിൽ (highlight != ശൂന്യ && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    എങ്കിൽ (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == ശൂന്യ ? (char_u *)"" : highlight);
		തിരികെ;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	എങ്കിൽ (di != ശൂന്യ)
	{
	    എങ്കിൽ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    അല്ലെങ്കിൽ
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	എങ്കിൽ (di != ശൂന്യ)
	{
	    എങ്കിൽ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    അല്ലെങ്കിൽ
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	എങ്കിൽ (di != ശൂന്യ)
	    prop->pt_priority = tv_get_സംഖ്യ(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	എങ്കിൽ (di != ശൂന്യ)
	{
	    എങ്കിൽ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    അല്ലെങ്കിൽ
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	എങ്കിൽ (di != ശൂന്യ)
	{
	    എങ്കിൽ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    അല്ലെങ്കിൽ
		prop->pt_flags &= ~PT_FLAG_INS_END_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    ശൂന്യം
f_prop_type_add(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    ശൂന്യം
f_prop_type_change(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    ശൂന്യം
f_prop_type_delete(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*name;
    buf_T	*buf = ശൂന്യ;
    hashitem_T	*hi;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    name = tv_get_string(&argvars[0]);
    എങ്കിൽ (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	തിരികെ;
    }

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    തിരികെ;
    }

    hi = find_prop_type_hi(name, buf);
    എങ്കിൽ (hi == ശൂന്യ)
	തിരികെ;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    എങ്കിൽ (buf == ശൂന്യ)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    അല്ലെങ്കിൽ
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    changed_window_setting_buf(buf == ശൂന്യ ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    ശൂന്യം
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    name = tv_get_string(&argvars[0]);
    എങ്കിൽ (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	തിരികെ;
    }

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;

    proptype_T  *prop = ശൂന്യ;
    buf_T	    *buf = ശൂന്യ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    തിരികെ;
    }

    prop = find_prop_type(name, buf);
    എങ്കിൽ (prop == ശൂന്യ)
	തിരികെ;

    dict_T *d = rettv->vval.v_dict;

    എങ്കിൽ (prop->pt_hl_id > 0)
	dict_add_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_add_സംഖ്യ(d, "priority", prop->pt_priority);
    dict_add_സംഖ്യ(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_add_സംഖ്യ(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_add_സംഖ്യ(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_START_INCL) ? 1 : 0);
    dict_add_സംഖ്യ(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_END_INCL) ? 1 : 0);
    എങ്കിൽ (buf != ശൂന്യ)
	dict_add_സംഖ്യ(d, "bufnr", buf->b_fnum);
}

    സ്ഥിതം ശൂന്യം
list_types(hashtab_T *ht, list_T *l)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(ht, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
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
    ശൂന്യം
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ശൂന്യ;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    തിരികെ;
    }
    എങ്കിൽ (buf == ശൂന്യ)
    {
	എങ്കിൽ (global_proptypes != ശൂന്യ)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (buf->b_proptypes != ശൂന്യ)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    സ്ഥിതം ശൂന്യം
clear_ht_prop_types(hashtab_T *ht)
{
    long	todo;
    hashitem_T	*hi;

    എങ്കിൽ (ht == ശൂന്യ)
	തിരികെ;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(ht, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#എങ്കിൽ defined(EXITFREE)
/*
 * Free all global property types.
 */
    ശൂന്യം
clear_global_prop_types(ശൂന്യം)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = ശൂന്യ;
    VIM_CLEAR(global_proparray);
}
#endif

/*
 * Free all property types for "buf".
 */
    ശൂന്യം
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = ശൂന്യ;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ dirty;	    // if the property was changed
    പൂർണ്ണസംഖ്യ can_drop;   // whether after this change, the prop may be removed
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
    സ്ഥിതം adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    col,
	പൂർണ്ണസംഖ്യ	    added,
	പൂർണ്ണസംഖ്യ	    flags)
{
    proptype_T	*pt;
    പൂർണ്ണസംഖ്യ		start_incl;
    പൂർണ്ണസംഖ്യ		end_incl;
    പൂർണ്ണസംഖ്യ		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    എങ്കിൽ (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	തിരികെ res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != ശൂന്യ && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    എങ്കിൽ (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != ശൂന്യ && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    എങ്കിൽ (added > 0)
    {
	എങ്കിൽ (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	അല്ലെങ്കിൽ എങ്കിൽ (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (prop->tp_col > col + 1)
    {
	എങ്കിൽ (prop->tp_col + added < col + 1)
	{
	    എങ്കിൽ (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		എങ്കിൽ (prop->tp_len <= 0)
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
	അല്ലെങ്കിൽ
	    prop->tp_col += added;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	എങ്കിൽ (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	എങ്കിൽ (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    അല്ലെങ്കിൽ
	res.dirty = FALSE;

    തിരികെ res;
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
    പൂർണ്ണസംഖ്യ
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	പൂർണ്ണസംഖ്യ	    bytes_added,
	പൂർണ്ണസംഖ്യ	    flags)
{
    unpacked_memline_T um;
    പൂർണ്ണസംഖ്യ		dirty = FALSE;
    പൂർണ്ണസംഖ്യ		ri;

    എങ്കിൽ (text_prop_frozen > 0)
	തിരികെ FALSE;

    um = um_open_at(curbuf, lnum, 0);
    എങ്കിൽ (um.buf == ശൂന്യ || um.prop_എണ്ണം == 0)
    {
	um_abort(&um);
	തിരികെ FALSE;
    }

    വേണ്ടി (ri = 0; ri < um.prop_എണ്ണം; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	എങ്കിൽ (res.dirty)
	{
	    എങ്കിൽ (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		എങ്കിൽ (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    തിരികെ FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		എങ്കിൽ ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    തിരികെ FALSE;
		}
	    }
	    dirty = TRUE;
	}
	എങ്കിൽ (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    തുടരുക;
	}
    }
    എങ്കിൽ (dirty)
	um_close(&um);
    അല്ലെങ്കിൽ
	um_abort(&um);
    തിരികെ dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    ശൂന്യം
adjust_props_for_split(
	linenr_T    lnum_props,
	linenr_T    lnum_മുകൾ,
	പൂർണ്ണസംഖ്യ	    kept,
	പൂർണ്ണസംഖ്യ	    deleted,
	പൂർണ്ണസംഖ്യ	    at_eol)
{
    char_u	*props;
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    garray_T    prevprop;
    garray_T    nextprop;
    പൂർണ്ണസംഖ്യ		skipped = kept + deleted;

    എങ്കിൽ (!curbuf->b_has_textprop)
	തിരികെ;

    // Get the text properties from "lnum_props".
    എണ്ണം = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, വലുപ്പം(textprop_T), 10);
    ga_init2(&nextprop, വലുപ്പം(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *എണ്ണം_ptr = props - PROP_എണ്ണം_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	പൂർണ്ണസംഖ്യ	    start_incl, end_incl;
	പൂർണ്ണസംഖ്യ	    cont_prev, cont_next;
	പൂർണ്ണസംഖ്യ	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * വലുപ്പം(textprop_T), വലുപ്പം(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	എങ്കിൽ (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(എണ്ണം_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	അല്ലെങ്കിൽ
	    prop.u.tp_text = ശൂന്യ;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != ശൂന്യ && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != ശൂന്യ && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	എങ്കിൽ (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	അല്ലെങ്കിൽ
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    പൂർണ്ണസംഖ്യ is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    എങ്കിൽ (is_floating_vtext)
		cont_next = FALSE;
	    അല്ലെങ്കിൽ
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	എങ്കിൽ (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	എങ്കിൽ (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    പൂർണ്ണസംഖ്യ is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    എങ്കിൽ (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    എങ്കിൽ (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	എങ്കിൽ (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    പൂർണ്ണസംഖ്യ is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    എങ്കിൽ (p->tp_col != MAXCOL)
	    {
		എങ്കിൽ (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    എങ്കിൽ (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    set_text_props(lnum_മുകൾ, (textprop_T *)prevprop.ga_data,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    set_text_props(lnum_മുകൾ + 1, (textprop_T *)nextprop.ga_data,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    ശൂന്യം
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	പൂർണ്ണസംഖ്യ		    last_വരി,
	long		    col,
	പൂർണ്ണസംഖ്യ		    removed)
{
    unpacked_memline_T	r_um;

    എങ്കിൽ (um->buf == ശൂന്യ)
	തിരികെ;

    r_um = um_open_at(um->buf, lnum, 0);
    എങ്കിൽ (r_um.buf == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (!um_detach(&r_um))
    {
	um_abort(&r_um);
	തിരികെ;
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = r_um.prop_എണ്ണം - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	പൂർണ്ണസംഖ്യ	    end;

	എങ്കിൽ (prop->tp_col == MAXCOL && !last_വരി)
	    തുടരുക;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	എങ്കിൽ (last_വരി || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = ശൂന്യ;  // ownership transferred
	}
	അല്ലെങ്കിൽ
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < um->prop_എണ്ണം; ++j)
	    {
		textprop_T *op = &um->props[j];

		എങ്കിൽ ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    നിർത്തുക;
		}
	    }
	    എങ്കിൽ (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    സ്ഥിതം ശൂന്യം
remember_cleared_prop(garray_T *gap, പൂർണ്ണസംഖ്യ which, textprop_T *prop, പൂർണ്ണസംഖ്യ flag)
{
    undoprop_T	*up;

    എങ്കിൽ (which == UNDOPROP_NONE)
	തിരികെ;
    എങ്കിൽ (gap->ga_itemsize == 0)
	ga_init2(gap, വലുപ്പം(undoprop_T), 4);
    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ;
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
    ശൂന്യം
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	പൂർണ്ണസംഖ്യ	    above_which,
	പൂർണ്ണസംഖ്യ	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = ശൂന്യ;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    എങ്കിൽ (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_വരി_എണ്ണം)
	തിരികെ;

    above = um_open_at(buf, lnum, 0);
    എങ്കിൽ (above.buf == ശൂന്യ)
	തിരികെ;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < above.prop_എണ്ണം; ++i)
	എങ്കിൽ (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    എങ്കിൽ (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	തിരികെ;
    }

    below = um_open_at(buf, lnum + 1, 0);
    എങ്കിൽ (below.buf == ശൂന്യ)
    {
	um_abort(&above);
	തിരികെ;
    }
    എങ്കിൽ (below.prop_എണ്ണം > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_എണ്ണം);
	എങ്കിൽ (paired == ശൂന്യ)
	    പോകുക theend;
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < above.prop_എണ്ണം; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	എങ്കിൽ (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    തുടരുക;
	വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < below.prop_എണ്ണം; ++j)
	{
	    textprop_T *next = &below.props[j];

	    എങ്കിൽ (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < below.prop_എണ്ണം; ++j)
    {
	എങ്കിൽ (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    തുടരുക;
	എങ്കിൽ (!below.detached && !um_detach(&below))
	    നിർത്തുക;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    എങ്കിൽ (below_changed)
	um_close(&below);
    അല്ലെങ്കിൽ
	um_abort(&below);
    എങ്കിൽ (above_changed)
	um_close(&above);
    അല്ലെങ്കിൽ
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    ശൂന്യം
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	പൂർണ്ണസംഖ്യ	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    എങ്കിൽ (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_വരി_എണ്ണം)
	തിരികെ;
    um = um_open_at(buf, lnum, 0);
    എങ്കിൽ (um.buf == ശൂന്യ)
	തിരികെ;
    വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	എങ്കിൽ (up->up_which != which)
	    തുടരുക;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < um.prop_എണ്ണം; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    എങ്കിൽ (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		തുടരുക;
	    എങ്കിൽ (!um.detached && !um_detach(&um))
		പോകുക theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    നിർത്തുക;
	}
    }

theend:
    എങ്കിൽ (changed)
	um_close(&um);
    അല്ലെങ്കിൽ
	um_abort(&um);
}

    bool
text_prop_എണ്ണം_valid(പൂർണ്ണസംഖ്യ prop_എണ്ണം, size_t propdata_len)
{
    എങ്കിൽ (propdata_len < PROP_എണ്ണം_SIZE)
	തിരികെ false;
    തിരികെ (size_t)prop_എണ്ണം * വലുപ്പം(textprop_T)
		    <= propdata_len - PROP_എണ്ണം_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, പൂർണ്ണസംഖ്യ prop_എണ്ണം, size_t propdata_len)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < prop_എണ്ണം; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * വലുപ്പം(textprop_T),
							  വലുപ്പം(textprop_T));
	എങ്കിൽ (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    തുടരുക;
	എങ്കിൽ (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    തിരികെ false;
    }
    തിരികെ true;
}

#endif // FEAT_PROP_POPUP
