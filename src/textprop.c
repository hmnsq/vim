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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_PROP_POPUP)

ਸਥਿਰ ਖਾਲੀ um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    ਸਥਿਰ ਖਾਲੀ
um_free_vtext(textprop_T *props, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; ++i)
	ਜੇ (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_ਪਾਠ);
}

/*
 * Free memory used by an unpacked memline.
 */
    ਸਥਿਰ ਖਾਲੀ
um_free(unpacked_memline_T *um)
{
    ਜੇ (um->detached)
    {
	VIM_CLEAR(um->ਪਾਠ);
	um_free_vtext(um->props, um->prop_ਗਿਣਤੀ);
    }
    VIM_CLEAR(um->props);
    um->prop_ਮਾਪ = 0;
    um->prop_ਗਿਣਤੀ = 0;
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

    CLEAR_ਖੇਤਰ(um);
    um.buf = buf;
    ਵਾਪਸ um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_ਸਤਰ(unpacked_memline_T *um, linenr_T lnum, ਪੂਰਨ extra_props)
{
    char_u	*ਸਤਰ;
    size_t	textlen;
    size_t	propdata_len;
    ਪੂਰਨ		proplen;

    ਜੇ (um->buf == ਨਿਰਾ)
	ਵਾਪਸ false;

    // Store changes to the current line if any.
    ਜੇ (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    ਜੇ (lnum == 0)
	ਵਾਪਸ true;	// just unload

    ਸਤਰ = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_ਸਤਰ_len - textlen;

    um->lnum = lnum;
    um->ਪਾਠ_ਮਾਪ_2 = (colnr_T)textlen;
    um->ਪਾਠ = ਸਤਰ;

    ਜੇ (propdata_len == 0)
	ਵਾਪਸ true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    ਜੇ (propdata_len < PROP_ਗਿਣਤੀ_ਮਾਪ + ਆਕਾਰ(textprop_T))
    {
	emsg(e_ਪਾਠ_property_info_corrupted);
	um->buf = ਨਿਰਾ;
	ਵਾਪਸ false;
    }

    uint16_t    prop_ਗਿਣਤੀ;
    char_u	    *ਗਿਣਤੀ_ptr = ਸਤਰ + textlen;
    char_u	    *props_ਸ਼ੁਰੂ;

    mch_memmove(&prop_ਗਿਣਤੀ, ਗਿਣਤੀ_ptr, PROP_ਗਿਣਤੀ_ਮਾਪ);
    ਜੇ (!ਪਾਠ_prop_ਗਿਣਤੀ_valid(prop_ਗਿਣਤੀ, propdata_len))
    {
	emsg(e_ਪਾਠ_property_info_corrupted);
	um->buf = ਨਿਰਾ;
	ਵਾਪਸ false;
    }
    proplen = (ਪੂਰਨ)prop_ਗਿਣਤੀ;
    props_ਸ਼ੁਰੂ = ਗਿਣਤੀ_ptr + PROP_ਗਿਣਤੀ_ਮਾਪ;

    ਜੇ (!ਪਾਠ_prop_vtext_valid(props_ਸ਼ੁਰੂ, proplen, propdata_len))
    {
	emsg(e_ਪਾਠ_property_info_corrupted);
	um->buf = ਨਿਰਾ;
	ਵਾਪਸ false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    ਜੇ (um->props == ਨਿਰਾ)
    {
	um->buf = ਨਿਰਾ;
	ਵਾਪਸ false;
    }
    um->prop_ਮਾਪ = proplen + extra_props;
    um->prop_ਗਿਣਤੀ = proplen;
    mch_memmove(um->props, props_ਸ਼ੁਰੂ, proplen * ਆਕਾਰ(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    ਲਈ (ਪੂਰਨ i = 0; i < proplen; ++i)
    {
	ਜੇ (um->props[i].tp_id < 0 && um->props[i].u.tp_ਪਾਠ_ਵਿੱਥ > 0)
	{
	    um->props[i].u.tp_ਪਾਠ = ਗਿਣਤੀ_ptr + um->props[i].u.tp_ਪਾਠ_ਵਿੱਥ;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ਨਹੀਂਤਾਂ
	    um->props[i].u.tp_ਪਾਠ = ਨਿਰਾ;
    }

    ਵਾਪਸ true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, ਪੂਰਨ extra_props)
{
    unpacked_memline_T um = um_open(buf);

    ਜੇ (!um_goto_ਸਤਰ(&um, lnum, extra_props))
	um.buf = ਨਿਰਾ;
    ਵਾਪਸ um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    ਸਥਿਰ bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    ਜੇ (um->detached || um->buf == ਨਿਰਾ)
	ਵਾਪਸ um->detached;

    newtext = vim_strnsave(um->ਪਾਠ, um->ਪਾਠ_ਮਾਪ_2 - 1);
    ਜੇ (newtext == ਨਿਰਾ)
    {
	um->buf = ਨਿਰਾ;
	ਵਾਪਸ false;
    }
    um->ਪਾਠ = newtext;

    ਲਈ (ਪੂਰਨ i = 0; i < um->prop_ਗਿਣਤੀ; ++i)
    {
	ਜੇ (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_ਪਾਠ);

	    ਜੇ (copy == ਨਿਰਾ)
	    {
		ਲਈ (ਪੂਰਨ j = 0; j < i; ++j)
		    ਜੇ (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_ਪਾਠ);
		VIM_CLEAR(um->ਪਾਠ);
		um->buf = ਨਿਰਾ;
		ਵਾਪਸ false;
	    }
	    um->props[i].u.tp_ਪਾਠ = copy;
	}
    }

    um->detached = true;
    ਵਾਪਸ true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_ਸੈੱਟ_ਪਾਠ(unpacked_memline_T *um, char_u *ਪਾਠ)
{
    ਜੇ (um->buf == ਨਿਰਾ)
	ਵਾਪਸ false;
    ਜੇ (um->detached)
	vim_free(um->ਪਾਠ);
    um->ਪਾਠ = ਪਾਠ;
    um->ਪਾਠ_ਮਾਪ_2 = (colnr_T)STRLEN(ਪਾਠ) + 1;
    um->ਪਾਠ_ਬਦਲਿਆ = true;
    um->detached = true;
    ਵਾਪਸ true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    ਸਥਿਰ bool
um_grow_props(unpacked_memline_T *um, ਪੂਰਨ needed)
{
    ਜੇ (um->prop_ਗਿਣਤੀ + needed <= um->prop_ਮਾਪ)
	ਵਾਪਸ true;

    ਪੂਰਨ	    new_ਮਾਪ = um->prop_ਗਿਣਤੀ + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_ਮਾਪ * ਆਕਾਰ(textprop_T));
    ਜੇ (newprops == ਨਿਰਾ)
	ਵਾਪਸ false;
    um->props = newprops;
    um->prop_ਮਾਪ = new_ਮਾਪ;

    ਵਾਪਸ true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    ਸਥਿਰ ਖਾਲੀ
um_ਜੋੜੋ_prop(unpacked_memline_T *um, textprop_T *prop)
{
    ਜੇ (um->buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (!um->detached && !um_detach(um))
	ਵਾਪਸ;
    ਜੇ (!um_grow_props(um, 1))
    {
	um->buf = ਨਿਰਾ;
	ਵਾਪਸ;
    }
    um->props[um->prop_ਗਿਣਤੀ++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    ਖਾਲੀ
um_ਉਲਟ_props(unpacked_memline_T *um)
{
    ਲਈ (ਪੂਰਨ i = 0, j = um->prop_ਗਿਣਤੀ - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, ਪੂਰਨ prop_ਗਿਣਤੀ)
{
    unpacked_memline_T um;

    CLEAR_ਖੇਤਰ(um);
    um.buf = buf;

    ਜੇ (lnum < 1 || lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	um.buf = ਨਿਰਾ;
	ਵਾਪਸ um;
    }

    char_u *ਸਤਰ = ml_get_buf(buf, lnum, FALSE);
    char_u *ਪਾਠ_copy = vim_strsave(ਸਤਰ);

    ਜੇ (ਪਾਠ_copy == ਨਿਰਾ)
    {
	um.buf = ਨਿਰਾ;
	ਵਾਪਸ um;
    }
    um.lnum = lnum;
    um.ਪਾਠ = ਪਾਠ_copy;
    um.ਪਾਠ_ਮਾਪ_2 = (colnr_T)STRLEN(ਪਾਠ_copy) + 1;
    um.detached = true;

    ਜੇ (prop_ਗਿਣਤੀ > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_ਗਿਣਤੀ);
	ਜੇ (um.props == ਨਿਰਾ)
	{
	    vim_free(um.ਪਾਠ);
	    um.buf = ਨਿਰਾ;
	    ਵਾਪਸ um;
	}
	um.prop_ਮਾਪ = prop_ਗਿਣਤੀ;
    }

    ਵਾਪਸ um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    ਸਥਿਰ ਖਾਲੀ
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					ਪੂਰਨ tp_id, ਪੂਰਨ tp_type, ਪੂਰਨ flag)
{
    unpacked_memline_T	neighbor;

    ਜੇ (lnum < 1 || lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ;

    neighbor = um_open_at(buf, lnum, 0);
    ਜੇ (neighbor.buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ i = 0; i < neighbor.prop_ਗਿਣਤੀ; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	ਜੇ (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    ਜੇ (!neighbor.detached && !um_detach(&neighbor))
		ਰੋਕੋ;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    ਰੋਕੋ;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    ਖਾਲੀ
um_delete_prop(unpacked_memline_T *um, ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
    textprop_T *prop;

    ਜੇ (um->buf == ਨਿਰਾ || ਸੂਚਕਾਂਕ < 0 || ਸੂਚਕਾਂਕ >= um->prop_ਗਿਣਤੀ)
	ਵਾਪਸ;
    ਜੇ (!um->detached && !um_detach(um))
	ਵਾਪਸ;

    prop = &um->props[ਸੂਚਕਾਂਕ];

    // Adjust continuation flags on neighboring lines before deleting.
    ਜੇ (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_ਅਗਲਾ);
    ਜੇ (prop->tp_flags & TP_FLAG_CONT_ਅਗਲਾ)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    ਜੇ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	VIM_CLEAR(prop->u.tp_ਪਾਠ);
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
    ਸਥਿਰ char_u *
um_pack(unpacked_memline_T *um, ਪੂਰਨ *packed_len)
{
    uint16_t	ਜੀਵਤ_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		vtext_ਮਾਪ = 0;
    ਪੂਰਨ		ਕੁੱਲ_len;
    char_u	*buf;
    char_u	*ਗਿਣਤੀ_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    ਲਈ (ਪੂਰਨ i = 0; i < um->prop_ਗਿਣਤੀ; ++i)
    {
	textprop_T *prop = &um->props[i];

	ਜੇ (prop->tp_flags & TP_FLAG_DELETED)
	    ਜਾਰੀ;
	++ਜੀਵਤ_ਗਿਣਤੀ;
	ਜੇ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_ਮਾਪ += prop->tp_len + 1;
    }

    ਜੇ (ਜੀਵਤ_ਗਿਣਤੀ == 0)
    {
	// No properties: just text, no prop_count header.
	ਕੁੱਲ_len = um->ਪਾਠ_ਮਾਪ_2;
	buf = alloc(ਕੁੱਲ_len);
	ਜੇ (buf == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	*packed_len = ਕੁੱਲ_len;
	mch_memmove(buf, um->ਪਾਠ, um->ਪਾਠ_ਮਾਪ_2);
	ਵਾਪਸ buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    ਕੁੱਲ_len = um->ਪਾਠ_ਮਾਪ_2 + (ਪੂਰਨ)PROP_ਗਿਣਤੀ_ਮਾਪ
		+ ਜੀਵਤ_ਗਿਣਤੀ * (ਪੂਰਨ)ਆਕਾਰ(textprop_T) + vtext_ਮਾਪ;
    buf = alloc(ਕੁੱਲ_len);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    *packed_len = ਕੁੱਲ_len;

    mch_memmove(buf, um->ਪਾਠ, um->ਪਾਠ_ਮਾਪ_2);

    ਗਿਣਤੀ_dest = buf + um->ਪਾਠ_ਮਾਪ_2;
    mch_memmove(ਗਿਣਤੀ_dest, &ਜੀਵਤ_ਗਿਣਤੀ, PROP_ਗਿਣਤੀ_ਮਾਪ);

    prop_dest = ਗਿਣਤੀ_dest + PROP_ਗਿਣਤੀ_ਮਾਪ;
    vtext_dest = prop_dest + ਜੀਵਤ_ਗਿਣਤੀ * ਆਕਾਰ(textprop_T);

    ਲਈ (ਪੂਰਨ i = 0; i < um->prop_ਗਿਣਤੀ; ++i)
    {
	textprop_T prop = um->props[i];

	ਜੇ (prop.tp_flags & TP_FLAG_DELETED)
	    ਜਾਰੀ;

	ਜੇ (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    ਪੂਰਨ copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_ਪਾਠ, copysize);
	    prop.u.tp_ਪਾਠ_ਵਿੱਥ = (colnr_T)(vtext_dest - ਗਿਣਤੀ_dest);
	    vtext_dest += copysize;
	}
	ਨਹੀਂਤਾਂ
	    prop.u.tp_ਪਾਠ_ਵਿੱਥ = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, ਆਕਾਰ(textprop_T));
	prop_dest += ਆਕਾਰ(textprop_T);
    }

    ਵਾਪਸ buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    ਸਥਿਰ ਖਾਲੀ
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    ਪੂਰਨ	    packed_len;

    ਜੇ (!um->detached || um->buf == ਨਿਰਾ || um->lnum == 0)
	ਵਾਪਸ;

    packed = um_pack(um, &packed_len);
    ਜੇ (packed == ਨਿਰਾ)
    {
	um->buf = ਨਿਰਾ;
	ਵਾਪਸ;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    ਜੇ (um->buf->b_ml.ml_ਸਤਰ_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    ਨਹੀਂਤਾਂ ਜੇ (!(um->buf->b_ml.ml_flags & ML_ਸਤਰ_DIRTY))
	(ਖਾਲੀ)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->ਪਾਠ);
    um_free_vtext(um->props, um->prop_ਗਿਣਤੀ);

    ਜੇ (um->buf->b_ml.ml_flags & (ML_ਸਤਰ_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_ਸਤਰ_ptr);
    um->buf->b_ml.ml_ਸਤਰ_ptr = packed;
    um->buf->b_ml.ml_ਸਤਰ_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_ਸਤਰ_DIRTY;

    um->detached = false;
    um->ਪਾਠ = packed;
    um->ਪਾਠ_ਮਾਪ_2 = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_ਸਤਰ_textlen = um->ਪਾਠ_ਮਾਪ_2;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    ਖਾਲੀ
um_close(unpacked_memline_T *um)
{
    ਜੇ (um->buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = ਨਿਰਾ;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    ਖਾਲੀ
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = ਨਿਰਾ;
}

/*
 * In a hashtable item "hi_key" points to "pt_name" in a proptype_T.
 * This avoids adding a pointer to the hashtable item.
 * PT2HIKEY() converts a proptype pointer to a hashitem key pointer.
 * HIKEY2PT() converts a hashitem key pointer to a proptype pointer.
 * HI2PT() converts a hashitem pointer to a proptype pointer.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PT2HIKEY(p)  ((p)->pt_name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIKEY2PT(p)   ((proptype_T *)((p) - offsetof(proptype_T, pt_name)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HI2PT(hi)      HIKEY2PT((hi)->hi_ਕੁੰਜੀ)

// The global text property types.
ਸਥਿਰ hashtab_T *global_proptypes = ਨਿਰਾ;
ਸਥਿਰ proptype_T **global_proparray = ਨਿਰਾ;

// The last used text property type ID.
ਸਥਿਰ ਪੂਰਨ proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    ਸਥਿਰ hashitem_T *
ਲੱਭੋ_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    ਜੇ (*name == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (buf == ਨਿਰਾ)
	ht = global_proptypes;
    ਨਹੀਂਤਾਂ
	ht = buf->b_proptypes;

    ਜੇ (ht == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    hi = hash_ਲੱਭੋ(ht, name);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    ਸਥਿਰ proptype_T *
ਲੱਭੋ_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = ਲੱਭੋ_prop_type_hi(name, buf);

    ਜੇ (hi == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    ਪੂਰਨ
ਲੱਭੋ_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = ਲੱਭੋ_prop_type(name, buf);

    ਜੇ (pt == ਨਿਰਾ)
	ਵਾਪਸ 0;
    ਵਾਪਸ pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    ਸਥਿਰ proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = ਲੱਭੋ_prop_type(name, buf);

    ਜੇ (type == ਨਿਰਾ)
	type = ਲੱਭੋ_prop_type(name, ਨਿਰਾ);
    ਜੇ (type == ਨਿਰਾ)
	semsg(_(e_property_type_str_does_not_exist), name);
    ਵਾਪਸ type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    ਜੇ (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (arg->vval.v_dict == ਨਿਰਾ)
	ਵਾਪਸ OK;  // NULL dict is like an empty dict
    di = dict_ਲੱਭੋ(arg->vval.v_dict, (char_u *)"bufnr", -1);
    ਜੇ (di != ਨਿਰਾ && (di->di_tv.v_type != VAR_ਸੰਖਿਆ
					      || di->di_tv.vval.v_ਸੰਖਿਆ != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	ਜੇ (*buf == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    ਖਾਲੀ
f_prop_ਜੋੜੋ(typval_T *argvars, typval_T *rettv)
{
    linenr_T	ਸ਼ੁਰੂ_lnum;
    colnr_T	ਸ਼ੁਰੂ_ਖਾਨਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    ਸ਼ੁਰੂ_lnum = tv_get_ਸੰਖਿਆ(&argvars[0]);
    ਸ਼ੁਰੂ_ਖਾਨਾ = tv_get_ਸੰਖਿਆ(&argvars[1]);
    ਜੇ (check_for_dict_arg(argvars, 2) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = prop_ਜੋੜੋ_common(ਸ਼ੁਰੂ_lnum, ਸ਼ੁਰੂ_ਖਾਨਾ,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    ਸਥਿਰ ਪੂਰਨ
prop_ਜੋੜੋ_one(
	buf_T		*buf,
	char_u		*type_name,
	ਪੂਰਨ		id,
	char_u		*ਪਾਠ_arg,
	ਪੂਰਨ		ਪਾਠ_padding_ਖੱਬਾ,
	ਪੂਰਨ		ਪਾਠ_flags,
	linenr_T	ਸ਼ੁਰੂ_lnum,
	linenr_T	ਸਿਰਾ_lnum,
	colnr_T		ਸ਼ੁਰੂ_ਖਾਨਾ,
	colnr_T		ਸਿਰਾ_ਖਾਨਾ)
{
    proptype_T	*type;
    linenr_T	lnum;
    ਪੂਰਨ		proplen;
    char_u	*props = ਨਿਰਾ;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    ਪੂਰਨ		i;
    textprop_T	tmp_prop;
    char_u	*ਪਾਠ = ਪਾਠ_arg;
    ਪੂਰਨ		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    ਜੇ (type == ਨਿਰਾ)
	ਜਾਓ theend;

    ਜੇ (ਸ਼ੁਰੂ_lnum < 1 || ਸ਼ੁਰੂ_lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	semsg(_(e_invalid_ਸਤਰ_ਸੰਖਿਆ_nr), (ਲੰਮਾ)ਸ਼ੁਰੂ_lnum);
	ਜਾਓ theend;
    }
    ਜੇ (ਸਿਰਾ_lnum < ਸ਼ੁਰੂ_lnum || ਸਿਰਾ_lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	semsg(_(e_invalid_ਸਤਰ_ਸੰਖਿਆ_nr), (ਲੰਮਾ)ਸਿਰਾ_lnum);
	ਜਾਓ theend;
    }

    ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
    {
	emsg(_(e_cannot_ਜੋੜੋ_ਪਾਠ_property_ਮੰਜ਼ਿਲ_unloaded_ਬਫਰ));
	ਜਾਓ theend;
    }

    ਜੇ (ਪਾਠ != ਨਿਰਾ)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	ਲਈ (p = ਪਾਠ; *p != NUL; MB_PTR_ADV(p))
	    ਜੇ (*p < ' ')
		*p = ' ';
    }

    ਲਈ (lnum = ਸ਼ੁਰੂ_lnum; lnum <= ਸਿਰਾ_lnum; ++lnum)
    {
	colnr_T ਖਾਨਾ;	    // start column use in tp_col
	colnr_T ਛਾਂਟੋ_ਖਾਨਾ;   // column where it appears
	ਲੰਮਾ	ਲੰਮਾਈ;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_ਪਾਠ_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	ਜੇ (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_ਪਾਠ_properties_on_a_single_ਸਤਰ));
	    ਜਾਓ theend;
	}

	ਜੇ (lnum == ਸ਼ੁਰੂ_lnum)
	    ਖਾਨਾ = ਸ਼ੁਰੂ_ਖਾਨਾ;
	ਨਹੀਂਤਾਂ
	    ਖਾਨਾ = 1;
	ਜੇ (ਖਾਨਾ - 1 > (colnr_T)textlen && !(ਖਾਨਾ == 0 && ਪਾਠ_arg != ਨਿਰਾ))
	{
	    semsg(_(e_invalid_ਕਾਲਮ_ਸੰਖਿਆ_nr), (ਲੰਮਾ)ਸ਼ੁਰੂ_ਖਾਨਾ);
	    ਜਾਓ theend;
	}
	ਛਾਂਟੋ_ਖਾਨਾ = ਖਾਨਾ;

	ਜੇ (lnum == ਸਿਰਾ_lnum)
	    ਲੰਮਾਈ = ਸਿਰਾ_ਖਾਨਾ - ਖਾਨਾ;
	ਨਹੀਂਤਾਂ
	    ਲੰਮਾਈ = (ਪੂਰਨ)textlen - ਖਾਨਾ + 1;
	ਜੇ (ਲੰਮਾਈ > (ਲੰਮਾ)textlen)
	    ਲੰਮਾਈ = (ਪੂਰਨ)textlen;	// can include the end-of-line
	ਜੇ (ਲੰਮਾਈ < 0)
	    ਲੰਮਾਈ = 0;		// zero-width property

	ਜੇ (ਪਾਠ_arg != ਨਿਰਾ)
	{
	    ਲੰਮਾਈ = (ਲੰਮਾ)STRLEN(ਪਾਠ_arg);
	    ਜੇ (ਖਾਨਾ == 0)
	    {
		ਖਾਨਾ = MAXCOL;	// before or after the line
		ਜੇ ((ਪਾਠ_flags & TP_FLAG_ALIGN_ABOVE) == 0)
		    ਛਾਂਟੋ_ਖਾਨਾ = MAXCOL;
	    }
	}

	// Build the new property.
	CLEAR_ਖੇਤਰ(tmp_prop);
	tmp_prop.tp_ਖਾਨਾ = ਖਾਨਾ;
	tmp_prop.tp_len = ਲੰਮਾਈ;
	tmp_prop.tp_id = id;
	tmp_prop.tp_type = type->pt_id;
	tmp_prop.tp_flags = ਪਾਠ_flags
			    | (lnum > ਸ਼ੁਰੂ_lnum ? TP_FLAG_CONT_PREV : 0)
			    | (lnum < ਸਿਰਾ_lnum ? TP_FLAG_CONT_ਅਗਲਾ : 0)
			    | ((type->pt_flags & PT_FLAG_INS_ਸ਼ੁਰੂ_INCL)
						     ? TP_FLAG_ਸ਼ੁਰੂ_INCL : 0);
	tmp_prop.tp_padleft = ਪਾਠ_padding_ਖੱਬਾ;
	ਜੇ (ਪਾਠ_arg != ਨਿਰਾ)
	    tmp_prop.u.tp_ਪਾਠ = ਪਾਠ_arg;

	// Find the index where to insert the new property.
	ਲਈ (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_ਖਾਨਾ;

	    mch_memmove(&existing, props + i * ਆਕਾਰ(textprop_T),
							   ਆਕਾਰ(textprop_T));
	    prop_ਖਾਨਾ = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_ਖਾਨਾ;
	    ਜੇ (prop_ਖਾਨਾ >= ਛਾਂਟੋ_ਖਾਨਾ)
		ਰੋਕੋ;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	ਪੂਰਨ		vtext_ਕੁੱਲ = 0;
	ਪੂਰਨ		new_ਸਤਰ_len;
	char_u	*ਗਿਣਤੀ_dest;
	char_u	*vtext_dest;
	ਪੂਰਨ		j;

	// Compute total vtext size from existing props.
	ਲਈ (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * ਆਕਾਰ(textprop_T),
							ਆਕਾਰ(textprop_T));
	    ਜੇ (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_ਕੁੱਲ += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	ਜੇ (ਪਾਠ_arg != ਨਿਰਾ)
	    vtext_ਕੁੱਲ += ਲੰਮਾਈ + 1;

	new_ਸਤਰ_len = (ਪੂਰਨ)textlen + (ਪੂਰਨ)PROP_ਗਿਣਤੀ_ਮਾਪ
			+ new_propcount * (ਪੂਰਨ)ਆਕਾਰ(textprop_T)
			+ vtext_ਕੁੱਲ;
	newtext = alloc(new_ਸਤਰ_len);
	ਜੇ (newtext == ਨਿਰਾ)
	    ਜਾਓ theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_ਸਤਰ_ptr, textlen);

	// Write prop_count.
	ਗਿਣਤੀ_dest = newtext + textlen;
	mch_memmove(ਗਿਣਤੀ_dest, &new_propcount, PROP_ਗਿਣਤੀ_ਮਾਪ);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = ਗਿਣਤੀ_dest + PROP_ਗਿਣਤੀ_ਮਾਪ;
	ਜੇ (i > 0)
	    mch_memmove(newprops, props, ਆਕਾਰ(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	ਜੇ (i < proplen)
	    mch_memmove(newprops + (i + 1) * ਆਕਾਰ(textprop_T),
					props + i * ਆਕਾਰ(textprop_T),
					ਆਕਾਰ(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * ਆਕਾਰ(textprop_T);
	ਲਈ (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    ਜੇ (j == i)
		ਜਾਰੀ;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * ਆਕਾਰ(textprop_T),
		    ਆਕਾਰ(textprop_T));
	    ਜੇ (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_ਗਿਣਤੀ_2 = (char_u *)props - PROP_ਗਿਣਤੀ_ਮਾਪ;
		char_u *old_vtext = old_ਗਿਣਤੀ_2 + ep.u.tp_ਪਾਠ_ਵਿੱਥ;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_ਪਾਠ_ਵਿੱਥ = (colnr_T)(vtext_dest - ਗਿਣਤੀ_dest);
		mch_memmove(newprops + j * ਆਕਾਰ(textprop_T), &ep,
			ਆਕਾਰ(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	ਜੇ (ਪਾਠ_arg != ਨਿਰਾ)
	{
	    ਪੂਰਨ copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, ਪਾਠ_arg, copysize);
	    tmp_prop.u.tp_ਪਾਠ_ਵਿੱਥ =
				    (colnr_T)(vtext_dest - ਗਿਣਤੀ_dest);
	    vtext_dest += copysize;
	}
	ਨਹੀਂਤਾਂ
	    tmp_prop.u.tp_ਪਾਠ_ਵਿੱਥ = 0;

	mch_memmove(newprops + i * ਆਕਾਰ(textprop_T), &tmp_prop,
							ਆਕਾਰ(textprop_T));

	ਜੇ (buf->b_ml.ml_flags & (ML_ਸਤਰ_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_ਸਤਰ_ptr);
	buf->b_ml.ml_ਸਤਰ_ptr = newtext;
	buf->b_ml.ml_ਸਤਰ_len = new_ਸਤਰ_len;
	buf->b_ml.ml_flags |= ML_ਸਤਰ_DIRTY;
    }

    ਬਦਲਿਆ_ਸਤਰ_display_buf(buf);
    ਬਦਲਿਆ_ਸਤਰਾਂ_buf(buf, ਸ਼ੁਰੂ_lnum, ਸਿਰਾ_lnum + 1, 0);
    res = OK;

theend:
    vim_free(ਪਾਠ);
    ਵਾਪਸ res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    ਖਾਲੀ
f_prop_ਜੋੜੋ_list(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    ਪੂਰਨ		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	ਸ਼ੁਰੂ_lnum;
    colnr_T	ਸ਼ੁਰੂ_ਖਾਨਾ;
    linenr_T	ਸਿਰਾ_lnum;
    colnr_T	ਸਿਰਾ_ਖਾਨਾ;
    ਪੂਰਨ		error = FALSE;
    ਪੂਰਨ		prev_did_emsg = did_emsg;

    ਜੇ (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    ਜੇ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    dict = argvars[0].vval.v_dict;
    ਜੇ (dict == ਨਿਰਾ || !dict_has_ਕੁੰਜੀ(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ਵਾਪਸ;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_ਸੰਖਿਆ(dict, "id");
	ਜੇ (x > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ਵਾਪਸ;
	}
	id = (ਪੂਰਨ)x;
    }

    ਜੇ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ਵਾਪਸ;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(argvars[1].vval.v_list, li)
    {
	ਜੇ (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == ਨਿਰਾ)
	{
	    emsg(_(e_list_required));
	    ਵਾਪਸ;
	}

	pos_list = li->li_tv.vval.v_list;
	ਸ਼ੁਰੂ_lnum = list_ਲੱਭੋ_nr(pos_list, 0L, &error);
	ਜੇ (!error)
	    ਸ਼ੁਰੂ_ਖਾਨਾ = list_ਲੱਭੋ_nr(pos_list, 1L, &error);
	ਜੇ (!error)
	    ਸਿਰਾ_lnum = list_ਲੱਭੋ_nr(pos_list, 2L, &error);
	ਜੇ (!error)
	    ਸਿਰਾ_ਖਾਨਾ = list_ਲੱਭੋ_nr(pos_list, 3L, &error);
	ਪੂਰਨ this_id = id;
	ਜੇ (!error && pos_list->lv_len > 4)
	    this_id = list_ਲੱਭੋ_nr(pos_list, 4L, &error);
	ਜੇ (error || ਸ਼ੁਰੂ_lnum <= 0 || ਸ਼ੁਰੂ_ਖਾਨਾ <= 0
		  || ਸਿਰਾ_lnum <= 0 || ਸਿਰਾ_ਖਾਨਾ <= 0)
	{
	    ਜੇ (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_ਦਲੀਲ));
	    ਵਾਪਸ;
	}
	ਜੇ (prop_ਜੋੜੋ_one(buf, type_name, this_id, ਨਿਰਾ, 0, 0,
			     ਸ਼ੁਰੂ_lnum, ਸਿਰਾ_lnum, ਸ਼ੁਰੂ_ਖਾਨਾ, ਸਿਰਾ_ਖਾਨਾ) == FAIL)
	    ਵਾਪਸ;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
ਸਥਿਰ ਪੂਰਨ vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    ਸਥਿਰ ਪੂਰਨ
get_textprop_id(ਖਾਲੀ)
{
    ਜੇ (vt_id_counter > 0)
	vt_id_counter = 0;
    ਵਾਪਸ --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    ਪੂਰਨ
prop_ਜੋੜੋ_common(
	linenr_T    ਸ਼ੁਰੂ_lnum,
	colnr_T	    ਸ਼ੁਰੂ_ਖਾਨਾ,
	dict_T	    *dict,
	buf_T	    *default_buf,
	typval_T    *dict_arg)
{
    linenr_T	ਸਿਰਾ_lnum;
    colnr_T	ਸਿਰਾ_ਖਾਨਾ;
    char_u	*type_name;
    buf_T	*buf = default_buf;
    ਪੂਰਨ		id = 0;
    char_u	*ਪਾਠ = ਨਿਰਾ;
    ਪੂਰਨ		ਪਾਠ_padding_ਖੱਬਾ = 0;
    ਪੂਰਨ		flags = 0;

    ਜੇ (dict == ਨਿਰਾ || !dict_has_ਕੁੰਜੀ(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ਜਾਓ theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "end_lnum"))
    {
	ਸਿਰਾ_lnum = dict_get_ਸੰਖਿਆ(dict, "end_lnum");
	ਜੇ (ਸਿਰਾ_lnum < ਸ਼ੁਰੂ_lnum)
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "end_lnum");
	    ਜਾਓ theend;
	}
    }
    ਨਹੀਂਤਾਂ
	ਸਿਰਾ_lnum = ਸ਼ੁਰੂ_lnum;

    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "length"))
    {
	ਲੰਮਾ ਲੰਮਾਈ = dict_get_ਸੰਖਿਆ(dict, "length");

	ਜੇ (ਲੰਮਾਈ < 0 || ਸਿਰਾ_lnum > ਸ਼ੁਰੂ_lnum)
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "length");
	    ਜਾਓ theend;
	}
	ਸਿਰਾ_ਖਾਨਾ = ਸ਼ੁਰੂ_ਖਾਨਾ + ਲੰਮਾਈ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (dict_has_ਕੁੰਜੀ(dict, "end_col"))
    {
	ਸਿਰਾ_ਖਾਨਾ = dict_get_ਸੰਖਿਆ(dict, "end_col");
	ਜੇ (ਸਿਰਾ_ਖਾਨਾ <= 0)
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "end_col");
	    ਜਾਓ theend;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ੁਰੂ_lnum == ਸਿਰਾ_lnum)
	ਸਿਰਾ_ਖਾਨਾ = ਸ਼ੁਰੂ_ਖਾਨਾ;
    ਨਹੀਂਤਾਂ
	ਸਿਰਾ_ਖਾਨਾ = 1;

    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_ਸੰਖਿਆ(dict, "id");
	ਜੇ (x > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ਜਾਓ theend;
	}
	id = (ਪੂਰਨ)x;
    }

    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "text"))
    {
	ਜੇ (dict_has_ਕੁੰਜੀ(dict, "length")
		|| dict_has_ਕੁੰਜੀ(dict, "end_col")
		|| dict_has_ਕੁੰਜੀ(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_ਲੰਮਾਈ_endcol_and_endlnum_with_ਪਾਠ));
	    ਜਾਓ theend;
	}

	ਪਾਠ = dict_get_string(dict, "text", TRUE);
	ਜੇ (ਪਾਠ == ਨਿਰਾ)
	    ਜਾਓ theend;
	// use a default length of 1 to make multiple props show up
	ਸਿਰਾ_ਖਾਨਾ = ਸ਼ੁਰੂ_ਖਾਨਾ + 1;

	ਜੇ (dict_has_ਕੁੰਜੀ(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    ਜੇ (p == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਜੇ (ਸ਼ੁਰੂ_ਖਾਨਾ != 0)
	    {
		emsg(_(e_can_only_use_ਪਾਠ_align_when_ਕਾਲਮ_is_zero));
		ਜਾਓ theend;
	    }
	    ਜੇ (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_ਸੱਜਾ;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_ਹੇਠਾਂ;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "text_align", p);
		ਜਾਓ theend;
	    }
	}

	ਜੇ (dict_has_ਕੁੰਜੀ(dict, "text_padding_left"))
	{
	    ਪਾਠ_padding_ਖੱਬਾ = dict_get_ਸੰਖਿਆ(dict, "text_padding_left");
	    ਜੇ (ਪਾਠ_padding_ਖੱਬਾ < 0)
	    {
		semsg(_(e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ_str), "text_padding_left");
		ਜਾਓ theend;
	    }
	}

	ਜੇ (dict_has_ਕੁੰਜੀ(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    ਜੇ (p == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਜੇ (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "text_wrap", p);
		ਜਾਓ theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    ਜੇ (ਸ਼ੁਰੂ_ਖਾਨਾ < (ਪਾਠ == ਨਿਰਾ ? 1 : 0))
    {
	semsg(_(e_invalid_ਕਾਲਮ_ਸੰਖਿਆ_nr), (ਲੰਮਾ)ਸ਼ੁਰੂ_ਖਾਨਾ);
	ਜਾਓ theend;
    }
    ਜੇ (ਸ਼ੁਰੂ_ਖਾਨਾ > 0 && ਪਾਠ_padding_ਖੱਬਾ > 0)
    {
	emsg(_(e_can_only_use_ਖੱਬਾ_padding_when_ਕਾਲਮ_is_zero));
	ਜਾਓ theend;
    }

    ਜੇ (dict_arg != ਨਿਰਾ && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	ਜਾਓ theend;

    ਜੇ (ਪਾਠ != ਨਿਰਾ)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    ਨਹੀਂਤਾਂ ਜੇ (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	ਜਾਓ theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_ਜੋੜੋ_one(buf, type_name, id, ਪਾਠ, ਪਾਠ_padding_ਖੱਬਾ, flags,
				    ਸ਼ੁਰੂ_lnum, ਸਿਰਾ_lnum, ਸ਼ੁਰੂ_ਖਾਨਾ, ਸਿਰਾ_ਖਾਨਾ);
    ਪਾਠ = ਨਿਰਾ;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(ਪਾਠ);
    ਵਾਪਸ id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    ਪੂਰਨ
get_ਪਾਠ_props(buf_T *buf, linenr_T lnum, char_u **props, ਪੂਰਨ will_change)
{
    char_u	*ਪਾਠ;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_ਗਿਣਤੀ;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    ਜੇ ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ 0;

    // Fetch the line to get the ml_line_len field updated.
    ਪਾਠ = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_ਸਤਰ_len - textlen;
    ਜੇ (propdata_len == 0)
	ਵਾਪਸ 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    ਜੇ (propdata_len < PROP_ਗਿਣਤੀ_ਮਾਪ + ਆਕਾਰ(textprop_T))
    {
	emsg(e_ਪਾਠ_property_info_corrupted);
	ਵਾਪਸ 0;
    }
    mch_memmove(&prop_ਗਿਣਤੀ, ਪਾਠ + textlen, PROP_ਗਿਣਤੀ_ਮਾਪ);
    ਜੇ (!ਪਾਠ_prop_ਗਿਣਤੀ_valid(prop_ਗਿਣਤੀ, propdata_len))
    {
	emsg(e_ਪਾਠ_property_info_corrupted);
	ਵਾਪਸ 0;
    }
    ਜੇ (!ਪਾਠ_prop_vtext_valid(ਪਾਠ + textlen + PROP_ਗਿਣਤੀ_ਮਾਪ,
					   (ਪੂਰਨ)prop_ਗਿਣਤੀ, propdata_len))
    {
	emsg(e_ਪਾਠ_property_info_corrupted);
	ਵਾਪਸ 0;
    }
    *props = ਪਾਠ + textlen + PROP_ਗਿਣਤੀ_ਮਾਪ;
    ਵਾਪਸ (ਪੂਰਨ)prop_ਗਿਣਤੀ;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    ਪੂਰਨ
prop_ਗਿਣਤੀ_above_ਹੇਠਾਂ(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = get_ਪਾਠ_props(buf, lnum, &props, FALSE);
    ਪੂਰਨ		ਨਤੀਜਾ = 0;
    textprop_T	prop;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਅਗਲਾ_ਸੱਜਾ_goes_ਹੇਠਾਂ = FALSE;

    ਜੇ (ਗਿਣਤੀ_2 == 0)
	ਵਾਪਸ 0;
    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	mch_memmove(&prop, props + i * ਆਕਾਰ(prop), ਆਕਾਰ(prop));
	ਜੇ (prop.tp_ਖਾਨਾ == MAXCOL && ਪਾਠ_prop_type_valid(buf, &prop))
	{
	    ਜੇ ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_ਹੇਠਾਂ))
		    || (ਅਗਲਾ_ਸੱਜਾ_goes_ਹੇਠਾਂ
				     && (prop.tp_flags & TP_FLAG_ALIGN_ਸੱਜਾ)))
	    {
		++ਨਤੀਜਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (prop.tp_flags & TP_FLAG_ALIGN_ਸੱਜਾ)
		ਅਗਲਾ_ਸੱਜਾ_goes_ਹੇਠਾਂ = TRUE;
	}
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    ਪੂਰਨ
ਗਿਣਤੀ_props(linenr_T lnum, ਪੂਰਨ only_starting, ਪੂਰਨ ਆਖਰੀ_ਸਤਰ)
{
    char_u	*props;
    ਪੂਰਨ		proplen = get_ਪਾਠ_props(curbuf, lnum, &props, 0);
    ਪੂਰਨ		ਨਤੀਜਾ = proplen;
    textprop_T	prop;

    ਲਈ (ਪੂਰਨ i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * ਆਕਾਰ(prop), ਆਕਾਰ(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	ਜੇ ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!ਆਖਰੀ_ਸਤਰ && prop.tp_ਖਾਨਾ == MAXCOL))
	    --ਨਤੀਜਾ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ textprop_T	*ਪਾਠ_prop_compare_props;
ਸਥਿਰ buf_T		*ਪਾਠ_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਾਠ_prop_ਕ੍ਰਮ(ਪੂਰਨ flags)
{
    ਜੇ (flags & TP_FLAG_ALIGN_ABOVE)
	ਵਾਪਸ 0;
    ਜੇ (flags & TP_FLAG_ALIGN_ਸੱਜਾ)
	ਵਾਪਸ 2;
    ਜੇ (flags & TP_FLAG_ALIGN_ਹੇਠਾਂ)
	ਵਾਪਸ 3;
    ਵਾਪਸ 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਾਠ_prop_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ਪੂਰਨ  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(ਪੂਰਨ *)s1;
    idx2 = *(ਪੂਰਨ *)s2;
    tp1 = &ਪਾਠ_prop_compare_props[idx1];
    tp2 = &ਪਾਠ_prop_compare_props[idx2];
    col1 = tp1->tp_ਖਾਨਾ;
    col2 = tp2->tp_ਖਾਨਾ;

    // property that inserts text has priority over one that doesn't
    ਜੇ ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	ਵਾਪਸ tp1->tp_id < 0 ? 1 : -1;

    ਜੇ (col1 == MAXCOL || col2 == MAXCOL)
    {
	ਪੂਰਨ order1 = ਪਾਠ_prop_ਕ੍ਰਮ(tp1->tp_flags);
	ਪੂਰਨ order2 = ਪਾਠ_prop_ਕ੍ਰਮ(tp2->tp_flags);

	// sort on order where it is added
	ਜੇ (order1 != order2)
	    ਵਾਪਸ order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = ਪਾਠ_prop_type_by_id(ਪਾਠ_prop_compare_buf, tp1->tp_type);
    pt2 = ਪਾਠ_prop_type_by_id(ਪਾਠ_prop_compare_buf, tp2->tp_type);
    ਜੇ (pt1 != pt2)
    {
	ਜੇ (pt1 == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	ਜੇ (pt2 == ਨਿਰਾ)
	    ਵਾਪਸ 1;
	ਜੇ (pt1->pt_ਤਰਜੀਹ != pt2->pt_ਤਰਜੀਹ)
	    ਵਾਪਸ pt1->pt_ਤਰਜੀਹ > pt2->pt_ਤਰਜੀਹ ? 1 : -1;
    }

    // same priority, one that starts first wins
    ਜੇ (col1 != col2)
	ਵਾਪਸ col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    ਜੇ (tp1->tp_id < 0)
	ਵਾਪਸ tp1->tp_id > tp2->tp_id ? 1 : -1;

    ਵਾਪਸ 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    ਖਾਲੀ
ਛਾਂਟੋ_ਪਾਠ_props(
	buf_T	    *buf,
	textprop_T  *props,
	ਪੂਰਨ	    *idxs,
	ਪੂਰਨ	    ਗਿਣਤੀ_2)
{
    ਪਾਠ_prop_compare_buf = buf;
    ਪਾਠ_prop_compare_props = props;
    qsort((ਖਾਲੀ *)idxs, (size_t)ਗਿਣਤੀ_2, ਆਕਾਰ(ਪੂਰਨ), ਪਾਠ_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
ਲੱਭੋ_prop_ਵਿੱਚ_ਸਤਰਾਂ(
	win_T	    *wp,
	ਪੂਰਨ	    type_id,
	ਪੂਰਨ	    id,
	textprop_T  *prop,
	linenr_T    *ਮਿਲਿਆ_lnum,
	linenr_T    ਪਹਿਲਾ_lnum,
	linenr_T    ਆਖਰੀ_lnum)
{
    ਜੇ (ਪਾਠ_prop_type_by_id(wp->w_ਬਫਰ, type_id) == ਨਿਰਾ)
	ਵਾਪਸ false;

    ਜੇ (ਪਹਿਲਾ_lnum < 1)
	ਪਹਿਲਾ_lnum = 1;
    ਜੇ (ਆਖਰੀ_lnum > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਆਖਰੀ_lnum = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਲਈ (linenr_T lnum = ਪਹਿਲਾ_lnum; lnum <= ਆਖਰੀ_lnum; ++lnum)
    {
	char_u	*props;
	ਪੂਰਨ	ਗਿਣਤੀ_2 = get_ਪਾਠ_props(wp->w_ਬਫਰ, lnum, &props, FALSE);
	ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; ++i)
	{
	    mch_memmove(prop, props + i * ਆਕਾਰ(textprop_T),
							   ਆਕਾਰ(textprop_T));
	    ਜੇ (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*ਮਿਲਿਆ_lnum = lnum;
		ਵਾਪਸ true;
	    }
	}
    }
    ਵਾਪਸ false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
ਲੱਭੋ_visible_prop(
	win_T	    *wp,
	ਪੂਰਨ	    type_id,
	ਪੂਰਨ	    id,
	textprop_T  *prop,
	linenr_T    *ਮਿਲਿਆ_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    ਵਾਪਸ ਲੱਭੋ_prop_ਵਿੱਚ_ਸਤਰਾਂ(wp, type_id, id, prop, ਮਿਲਿਆ_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_ਪਾਠ_props(linenr_T lnum, textprop_T *tps, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    char_u  *ਪਾਠ;
    char_u  *newtext;
    ਪੂਰਨ	    textlen;

    ਪਾਠ = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    ਜੇ (ਗਿਣਤੀ_2 == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	ਜੇ (newtext == ਨਿਰਾ)
	    ਵਾਪਸ;
	mch_memmove(newtext, ਪਾਠ, textlen);
	ਜੇ (curbuf->b_ml.ml_flags & (ML_ਸਤਰ_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ਸਤਰ_ptr);
	curbuf->b_ml.ml_ਸਤਰ_ptr = newtext;
	curbuf->b_ml.ml_ਸਤਰ_len = textlen;
	curbuf->b_ml.ml_flags |= ML_ਸਤਰ_DIRTY;
    }
    ਨਹੀਂਤਾਂ
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_ਗਿਣਤੀ = (uint16_t)ਗਿਣਤੀ_2;
	ਪੂਰਨ	    vtext_ਮਾਪ = 0;
	ਪੂਰਨ	    ਕੁੱਲ_len;
	char_u	    *ਗਿਣਤੀ_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	ਪੂਰਨ	    i;

	ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
	    ਜੇ (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_ਮਾਪ += tps[i].tp_len + 1;

	ਕੁੱਲ_len = textlen + (ਪੂਰਨ)PROP_ਗਿਣਤੀ_ਮਾਪ
		    + ਗਿਣਤੀ_2 * (ਪੂਰਨ)ਆਕਾਰ(textprop_T) + vtext_ਮਾਪ;
	newtext = alloc(ਕੁੱਲ_len);
	ਜੇ (newtext == ਨਿਰਾ)
	    ਵਾਪਸ;
	mch_memmove(newtext, ਪਾਠ, textlen);

	ਗਿਣਤੀ_dest = newtext + textlen;
	mch_memmove(ਗਿਣਤੀ_dest, &prop_ਗਿਣਤੀ, PROP_ਗਿਣਤੀ_ਮਾਪ);

	prop_dest = ਗਿਣਤੀ_dest + PROP_ਗਿਣਤੀ_ਮਾਪ;
	vtext_dest = prop_dest + ਗਿਣਤੀ_2 * ਆਕਾਰ(textprop_T);

	ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
	{
	    textprop_T prop = tps[i];

	    ਜੇ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		ਪੂਰਨ copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_ਪਾਠ, copysize);
		vim_free(tps[i].u.tp_ਪਾਠ);
		tps[i].u.tp_ਪਾਠ = ਨਿਰਾ;
		prop.u.tp_ਪਾਠ_ਵਿੱਥ = (colnr_T)(vtext_dest - ਗਿਣਤੀ_dest);
		vtext_dest += copysize;
	    }
	    ਨਹੀਂਤਾਂ
		prop.u.tp_ਪਾਠ_ਵਿੱਥ = 0;
	    mch_memmove(prop_dest, &prop, ਆਕਾਰ(textprop_T));
	    prop_dest += ਆਕਾਰ(textprop_T);
	}

	ਜੇ (curbuf->b_ml.ml_flags & (ML_ਸਤਰ_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ਸਤਰ_ptr);
	curbuf->b_ml.ml_ਸਤਰ_ptr = newtext;
	curbuf->b_ml.ml_ਸਤਰ_len = ਕੁੱਲ_len;
	curbuf->b_ml.ml_flags |= ML_ਸਤਰ_DIRTY;
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
props_ਜੋੜੋ_ਗਿਣਤੀ_ਸਿਰਲੇਖ(char_u *ਸਤਰ, ਪੂਰਨ ਸਤਰ_len, ਪੂਰਨ textlen, ਪੂਰਨ *new_len)
{
    ਪੂਰਨ		prop_ਬਾਈਟਾਂ = ਸਤਰ_len - textlen;
    uint16_t	prop_ਗਿਣਤੀ;
    ਪੂਰਨ		vtext_ਮਾਪ = 0;
    ਪੂਰਨ		ਕੁੱਲ;
    char_u	*newline;
    char_u	*ਗਿਣਤੀ_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    ਪੂਰਨ		i;

    ਜੇ (prop_ਬਾਈਟਾਂ <= 0 || prop_ਬਾਈਟਾਂ % ਆਕਾਰ(textprop_T) != 0)
    {
	*new_len = ਸਤਰ_len;
	ਵਾਪਸ ਨਿਰਾ;
    }
    prop_ਗਿਣਤੀ = (uint16_t)(prop_ਬਾਈਟਾਂ / ਆਕਾਰ(textprop_T));

    // Calculate total vtext size.
    ਲਈ (i = 0; i < (ਪੂਰਨ)prop_ਗਿਣਤੀ; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ਸਤਰ + textlen + i * ਆਕਾਰ(textprop_T),
							ਆਕਾਰ(textprop_T));
	ਜੇ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_ਮਾਪ += prop.tp_len + 1;
    }

    ਕੁੱਲ = textlen + (ਪੂਰਨ)PROP_ਗਿਣਤੀ_ਮਾਪ + prop_ਬਾਈਟਾਂ + vtext_ਮਾਪ;
    newline = alloc(ਕੁੱਲ);
    ਜੇ (newline == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    *new_len = ਕੁੱਲ;

    mch_memmove(newline, ਸਤਰ, textlen);
    ਗਿਣਤੀ_dest = newline + textlen;
    mch_memmove(ਗਿਣਤੀ_dest, &prop_ਗਿਣਤੀ, PROP_ਗਿਣਤੀ_ਮਾਪ);
    prop_dest = ਗਿਣਤੀ_dest + PROP_ਗਿਣਤੀ_ਮਾਪ;
    vtext_dest = prop_dest + prop_ਬਾਈਟਾਂ;

    ਲਈ (i = 0; i < (ਪੂਰਨ)prop_ਗਿਣਤੀ; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ਸਤਰ + textlen + i * ਆਕਾਰ(textprop_T),
							ਆਕਾਰ(textprop_T));
	ਜੇ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    ਪੂਰਨ copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_ਪਾਠ, copysize);
	    vim_free(prop.u.tp_ਪਾਠ);
	    prop.u.tp_ਪਾਠ_ਵਿੱਥ = (colnr_T)(vtext_dest - ਗਿਣਤੀ_dest);
	    vtext_dest += copysize;
	}
	ਨਹੀਂਤਾਂ
	    prop.u.tp_ਪਾਠ_ਵਿੱਥ = 0;
	mch_memmove(prop_dest + i * ਆਕਾਰ(textprop_T), &prop,
							ਆਕਾਰ(textprop_T));
    }
    ਵਾਪਸ newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    ਖਾਲੀ
ਜੋੜੋ_ਪਾਠ_props(linenr_T lnum, textprop_T *ਪਾਠ_props, ਪੂਰਨ ਪਾਠ_prop_ਗਿਣਤੀ)
{
    unpacked_memline_T	um;
    ਪੂਰਨ			i;

    um = um_open_at(curbuf, lnum, ਪਾਠ_prop_ਗਿਣਤੀ);
    ਜੇ (um.buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (!um_detach(&um))
    {
	um_abort(&um);
	ਵਾਪਸ;
    }

    // Grow props array if needed.
    ਜੇ (um.prop_ਗਿਣਤੀ + ਪਾਠ_prop_ਗਿਣਤੀ > um.prop_ਮਾਪ)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_ਗਿਣਤੀ + ਪਾਠ_prop_ਗਿਣਤੀ) * ਆਕਾਰ(textprop_T));
	ਜੇ (newprops == ਨਿਰਾ)
	{
	    um_abort(&um);
	    ਵਾਪਸ;
	}
	um.props = newprops;
	um.prop_ਮਾਪ = um.prop_ਗਿਣਤੀ + ਪਾਠ_prop_ਗਿਣਤੀ;
    }

    ਲਈ (i = 0; i < ਪਾਠ_prop_ਗਿਣਤੀ; ++i)
	um.props[um.prop_ਗਿਣਤੀ++] = ਪਾਠ_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    ਸਥਿਰ ਪੂਰਨ
compare_pt(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    ਵਾਪਸ tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    ਸਥਿਰ proptype_T *
ਲੱਭੋ_type_by_id(hashtab_T *ht, proptype_T ***array, ਪੂਰਨ id)
{
    ਪੂਰਨ ਨੀਵਾਂ = 0;
    ਪੂਰਨ ਉੱਚਾ;

    ਜੇ (ht == ਨਿਰਾ || ht->ht_ਵਰਤਿਆ == 0)
	ਵਾਪਸ ਨਿਰਾ;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    ਜੇ (*array == ਨਿਰਾ)
    {
	ਲੰਮਾ	    todo;
	hashitem_T  *hi;
	ਪੂਰਨ	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_ਵਰਤਿਆ);
	ਜੇ (*array == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	todo = (ਲੰਮਾ)ht->ht_ਵਰਤਿਆ;
	FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
	{
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((ਖਾਲੀ *)*array, ht->ht_ਵਰਤਿਆ, ਆਕਾਰ(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    ਉੱਚਾ = ht->ht_ਵਰਤਿਆ;
    ਜਦੋਂ (ਉੱਚਾ > ਨੀਵਾਂ)
    {
	ਪੂਰਨ m = (ਉੱਚਾ + ਨੀਵਾਂ) / 2;

	ਜੇ ((*array)[m]->pt_id == id)
	    ਵਾਪਸ (*array)[m];
	ਜੇ ((*array)[m]->pt_id > id)
	    ਉੱਚਾ = m;
	ਨਹੀਂਤਾਂ
	    ਨੀਵਾਂ = m + 1;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    ਸਥਿਰ ਖਾਲੀ
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    ਪੂਰਨ buflocal = TRUE;
    ਪੂਰਨ virtualtext_prop = prop->tp_id < 0;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "col", (prop->tp_ਖਾਨਾ == MAXCOL) ? 0 : prop->tp_ਖਾਨਾ);
    ਜੇ (!virtualtext_prop)
    {
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "length", prop->tp_len);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "id", prop->tp_id);
    }
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_ਅਗਲਾ));

    pt = ਲੱਭੋ_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    ਜੇ (pt == ਨਿਰਾ)
    {
	pt = ਲੱਭੋ_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    ਜੇ (pt != ਨਿਰਾ)
	dict_ਜੋੜੋ_string(dict, "type", pt->pt_name);

    ਜੇ (buflocal)
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "type_bufnr", buf->b_fnum);
    ਨਹੀਂਤਾਂ
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "type_bufnr", 0);
    ਜੇ (virtualtext_prop)
    {
	string_T    ਪਾਠ_align = {ਨਿਰਾ, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_ਜੋੜੋ_string(dict, "text", prop->u.tp_ਪਾਠ);

	// text_align
	ਜੇ (prop->tp_flags & TP_FLAG_ALIGN_ਸੱਜਾ)
	    STR_LITERAL_ਸੈੱਟ(ਪਾਠ_align, "right");
	ਨਹੀਂਤਾਂ ਜੇ (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_ਸੈੱਟ(ਪਾਠ_align, "above");
	ਨਹੀਂਤਾਂ ਜੇ (prop->tp_flags & TP_FLAG_ALIGN_ਹੇਠਾਂ)
	    STR_LITERAL_ਸੈੱਟ(ਪਾਠ_align, "below");
	ਜੇ (ਪਾਠ_align.string != ਨਿਰਾ)
	    dict_ਜੋੜੋ_string_len(dict, "text_align",
		ਪਾਠ_align.string, (ਪੂਰਨ)ਪਾਠ_align.ਲੰਮਾਈ);

	// text_wrap
	ਜੇ (prop->tp_flags & TP_FLAG_WRAP)
	    dict_ਜੋੜੋ_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	ਜੇ (prop->tp_padleft != 0)
	    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
ਪਾਠ_prop_type_by_id(buf_T *buf, ਪੂਰਨ id)
{
    proptype_T *type;

    type = ਲੱਭੋ_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    ਜੇ (type == ਨਿਰਾ)
	type = ਲੱਭੋ_type_by_id(global_proptypes, &global_proparray, id);
    ਵਾਪਸ type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
ਪਾਠ_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    ਵਾਪਸ ਪਾਠ_prop_type_by_id(buf, prop->tp_type) != ਨਿਰਾ;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    ਖਾਲੀ
f_prop_clear(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    linenr_T ਸ਼ੁਰੂ;
    linenr_T ਸਿਰਾ;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    ਪੂਰਨ	    did_clear = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    ਸ਼ੁਰੂ = tv_get_ਸੰਖਿਆ(&argvars[0]);
    ਸਿਰਾ = ਸ਼ੁਰੂ;
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ਸਿਰਾ = tv_get_ਸੰਖਿਆ(&argvars[1]);
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ਜੇ (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		ਵਾਪਸ;
	}
    }
    ਜੇ (ਸ਼ੁਰੂ < 1 || ਸਿਰਾ < 1)
    {
	emsg(_(e_invalid_range));
	ਵਾਪਸ;
    }

    {
	unpacked_memline_T um = um_open(buf);

	ਲਈ (lnum = ਸ਼ੁਰੂ; lnum <= ਸਿਰਾ; ++lnum)
	{
	    ਪੂਰਨ idx;

	    ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਰੋਕੋ;
	    ਜੇ (!um_goto_ਸਤਰ(&um, lnum, 0))
		ਰੋਕੋ;
	    ਜੇ (um.prop_ਗਿਣਤੀ > 0)
	    {
		did_clear = TRUE;
		ਲਈ (idx = um.prop_ਗਿਣਤੀ - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    ਜੇ (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    ਖਾਲੀ
f_prop_ਲੱਭੋ(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    ਪੂਰਨ		lnum_ਸ਼ੁਰੂ;
    ਪੂਰਨ		ਸ਼ੁਰੂ_pos_has_prop = 0;
    ਪੂਰਨ		ਵੇਖਿਆ_ਸਿਰਾ = FALSE;
    ਪੂਰਨ		id = 0;
    ਪੂਰਨ		id_ਮਿਲਿਆ = FALSE;
    ਪੂਰਨ		type_id = -1;
    ਪੂਰਨ		skipstart = FALSE;
    ਪੂਰਨ		lnum = -1;
    ਪੂਰਨ		ਖਾਨਾ = -1;
    ਪੂਰਨ		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    ਪੂਰਨ		both;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;
    dict = argvars[0].vval.v_dict;

    ਜੇ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ਵਾਪਸ;
    ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	ਜੇ (*dir_s == 'b')
	    dir = BACKWARD;
	ਨਹੀਂਤਾਂ ਜੇ (*dir_s != 'f')
	{
	    emsg(_(e_invalid_ਦਲੀਲ));
	    ਵਾਪਸ;
	}
    }

    di = dict_ਲੱਭੋ(dict, (char_u *)"lnum", -1);
    ਜੇ (di != ਨਿਰਾ)
	lnum = tv_get_ਸੰਖਿਆ(&di->di_tv);

    di = dict_ਲੱਭੋ(dict, (char_u *)"col", -1);
    ਜੇ (di != ਨਿਰਾ)
	ਖਾਨਾ = tv_get_ਸੰਖਿਆ(&di->di_tv);

    ਜੇ (lnum == -1)
    {
	lnum = cursor->lnum;
	ਖਾਨਾ = cursor->ਖਾਨਾ + 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਖਾਨਾ == -1)
	ਖਾਨਾ = 1;

    ਜੇ (lnum < 1 || lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	emsg(_(e_invalid_range));
	ਵਾਪਸ;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "id"))
    {
	id = dict_get_ਸੰਖਿਆ(dict, "id");
	id_ਮਿਲਿਆ = TRUE;
    }
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ਜੇ (type == ਨਿਰਾ)
	    ਵਾਪਸ;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    ਜੇ (!id_ਮਿਲਿਆ && type_id == -1)
    {
	emsg(_(e_need_at_ਘੱਟ_ਤੋਂ_ਘੱਟ_one_ਦਾ_id_or_type));
	ਵਾਪਸ;
    }
    ਜੇ (both && (!id_ਮਿਲਿਆ || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ਵਾਪਸ;
    }

    lnum_ਸ਼ੁਰੂ = lnum;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜਦੋਂ (1)
    {
	char_u	*prop_ਸ਼ੁਰੂ_ptr;
	ਪੂਰਨ	ਗਿਣਤੀ_2 = get_ਪਾਠ_props(buf, lnum, &prop_ਸ਼ੁਰੂ_ptr, FALSE);
	ਪੂਰਨ	    i;
	textprop_T  prop;
	ਪੂਰਨ	    prop_ਸ਼ੁਰੂ;
	ਪੂਰਨ	    prop_ਸਿਰਾ;

	ਲਈ (i = dir == BACKWARD ? ਗਿਣਤੀ_2 - 1 : 0; i >= 0 && i < ਗਿਣਤੀ_2; i += dir)
	{
	    mch_memmove(&prop, prop_ਸ਼ੁਰੂ_ptr + i * ਆਕਾਰ(textprop_T),
							   ਆਕਾਰ(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    ਜੇ (prop.tp_id < 0 && prop.u.tp_ਪਾਠ_ਵਿੱਥ > 0)
	    {
		prop.u.tp_ਪਾਠ = (prop_ਸ਼ੁਰੂ_ptr - PROP_ਗਿਣਤੀ_ਮਾਪ)
						    + prop.u.tp_ਪਾਠ_ਵਿੱਥ;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    ਨਹੀਂਤਾਂ
		prop.u.tp_ਪਾਠ = ਨਿਰਾ;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    ਜੇ (lnum == lnum_ਸ਼ੁਰੂ)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_ਖਾਨਾ == MAXCOL;
		ਜੇ (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    ਜੇ (!is_floating_vtext && prop.tp_ਖਾਨਾ > ਖਾਨਾ)
			ਜਾਰੀ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (!is_floating_vtext
		    && prop.tp_ਖਾਨਾ + prop.tp_len - (prop.tp_len != 0) < ਖਾਨਾ)
		    ਜਾਰੀ;
	    }
	    ਜੇ (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_ਮਿਲਿਆ && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		ਜੇ (lnum_ਸ਼ੁਰੂ == lnum
			&& ਖਾਨਾ >= prop.tp_ਖਾਨਾ
			&& (ਖਾਨਾ <= prop.tp_ਖਾਨਾ + prop.tp_len
							 - (prop.tp_len != 0)))
		    ਸ਼ੁਰੂ_pos_has_prop = 1;

		// The property was not continued from last line, it starts on
		// this line.
		prop_ਸ਼ੁਰੂ = !(prop.tp_flags & TP_FLAG_CONT_PREV);
		// The property does not continue on the next line, it ends on
		// this line.
		prop_ਸਿਰਾ = !(prop.tp_flags & TP_FLAG_CONT_ਅਗਲਾ);
		ਜੇ (!prop_ਸ਼ੁਰੂ && prop_ਸਿਰਾ && dir == FORWARD)
		    ਵੇਖਿਆ_ਸਿਰਾ = 1;

		// Skip lines without the start flag.
		ਜੇ (!prop_ਸ਼ੁਰੂ)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    ਜੇ (ਸ਼ੁਰੂ_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    ਜਾਰੀ;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		ਜੇ (ਸ਼ੁਰੂ_pos_has_prop && skipstart && !ਵੇਖਿਆ_ਸਿਰਾ)
		{
		    ਸ਼ੁਰੂ_pos_has_prop = 0;
		    ਜਾਰੀ;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_ਜੋੜੋ_ਸੰਖਿਆ(rettv->vval.v_dict, "lnum", lnum);

		ਵਾਪਸ;
	    }
	}

	ਜੇ (dir > 0)
	{
	    ਜੇ (lnum >= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਰੋਕੋ;
	    lnum++;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (lnum <= 1)
		ਰੋਕੋ;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    ਸਥਿਰ ਪੂਰਨ
prop_type_or_id_ਵਿੱਚ_list(ਪੂਰਨ *types_or_ids, ਪੂਰਨ len, ਪੂਰਨ type_or_id)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < len; i++)
	ਜੇ (types_or_ids[i] == type_or_id)
	    ਵਾਪਸ TRUE;

    ਵਾਪਸ FALSE;
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
    ਸਥਿਰ ਖਾਲੀ
get_props_ਵਿੱਚ_ਸਤਰ(
	buf_T		*buf,
	linenr_T	lnum,
	ਪੂਰਨ		*prop_types,
	ਪੂਰਨ		prop_types_len,
	ਪੂਰਨ		*prop_ids,
	ਪੂਰਨ		prop_ids_len,
	list_T		*retlist,
	ਪੂਰਨ		ਜੋੜੋ_lnum)
{
    char_u	*props;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    ਪੂਰਨ		i;
    textprop_T	prop;

    ਗਿਣਤੀ_2 = get_ਪਾਠ_props(buf, lnum, &props, FALSE);
    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	mch_memmove(&prop, props + i * ਆਕਾਰ(textprop_T),
		ਆਕਾਰ(textprop_T));
	// Convert offset to pointer for virtual text props.
	ਜੇ (prop.tp_id < 0 && prop.u.tp_ਪਾਠ_ਵਿੱਥ > 0)
	{
	    prop.u.tp_ਪਾਠ = (props - PROP_ਗਿਣਤੀ_ਮਾਪ)
						+ prop.u.tp_ਪਾਠ_ਵਿੱਥ;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ਨਹੀਂਤਾਂ
	    prop.u.tp_ਪਾਠ = ਨਿਰਾ;
	ਜੇ ((prop_types == ਨਿਰਾ
		    || prop_type_or_id_ਵਿੱਚ_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == ਨਿਰਾ
		    || prop_type_or_id_ਵਿੱਚ_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    ਜੇ (d == ਨਿਰਾ)
		ਰੋਕੋ;
	    prop_fill_dict(d, &prop, buf);
	    ਜੇ (ਜੋੜੋ_lnum)
		dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "lnum", lnum);
	    ਜੇ (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    ਸਥਿਰ ਪੂਰਨ *
get_prop_types_from_ਨਾਂ(list_T *l, buf_T *buf, ਪੂਰਨ *ਅੰਕ_types)
{
    ਪੂਰਨ		*prop_types;
    listitem_T	*li;
    ਪੂਰਨ		i;
    char_u	*name;
    proptype_T	*type;

    *ਅੰਕ_types = 0;

    prop_types = ALLOC_MULT(ਪੂਰਨ, list_len(l));
    ਜੇ (prop_types == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    i = 0;
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	ਜੇ (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    ਜਾਓ errret;
	}
	name = li->li_tv.vval.v_string;
	ਜੇ (name == ਨਿਰਾ)
	    ਜਾਓ errret;

	type = lookup_prop_type(name, buf);
	ਜੇ (type == ਨਿਰਾ)
	    ਜਾਓ errret;
	prop_types[i++] = type->pt_id;
    }

    *ਅੰਕ_types = i;
    ਵਾਪਸ prop_types;

errret:
    VIM_CLEAR(prop_types);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    ਸਥਿਰ ਪੂਰਨ *
get_prop_ids_from_list(list_T *l, ਪੂਰਨ *ਅੰਕ_ids)
{
    ਪੂਰਨ		*prop_ids;
    listitem_T	*li;
    ਪੂਰਨ		i = 0;
    ਪੂਰਨ		id;
    ਪੂਰਨ		error;

    *ਅੰਕ_ids = 0;

    prop_ids = ALLOC_MULT(ਪੂਰਨ, list_len(l));
    ਜੇ (prop_ids == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	error = FALSE;
	id = tv_get_ਸੰਖਿਆ_chk(&li->li_tv, &error);
	ਜੇ (error)
	    ਜਾਓ errret;

	prop_ids[i++] = id;
    }

    *ਅੰਕ_ids = i;
    ਵਾਪਸ prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    ਖਾਲੀ
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	ਸ਼ੁਰੂ_lnum;
    linenr_T	ਸਿਰਾ_lnum;
    buf_T	*buf = curbuf;
    ਪੂਰਨ		ਜੋੜੋ_lnum = FALSE;
    ਪੂਰਨ		*prop_types = ਨਿਰਾ;
    ਪੂਰਨ		prop_types_len = 0;
    ਪੂਰਨ		*prop_ids = ਨਿਰਾ;
    ਪੂਰਨ		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    // default: get text properties on current line
    ਸ਼ੁਰੂ_lnum = tv_get_ਸੰਖਿਆ(&argvars[0]);
    ਸਿਰਾ_lnum = ਸ਼ੁਰੂ_lnum;
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ਜੇ (check_for_dict_arg(argvars, 1) == FAIL)
	    ਵਾਪਸ;
	d = argvars[1].vval.v_dict;

	ਜੇ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ਵਾਪਸ;

	ਜੇ (d != ਨਿਰਾ && (di = dict_ਲੱਭੋ(d, (char_u *)"end_lnum", -1)) != ਨਿਰਾ)
	{
	    ਜੇ (di->di_tv.v_type != VAR_ਸੰਖਿਆ)
	    {
		emsg(_(e_ਸੰਖਿਆ_required));
		ਵਾਪਸ;
	    }
	    ਸਿਰਾ_lnum = tv_get_ਸੰਖਿਆ(&di->di_tv);
	    ਜੇ (ਸਿਰਾ_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//ਸਤਰ
		ਸਿਰਾ_lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + ਸਿਰਾ_lnum + 1;
	    ਨਹੀਂਤਾਂ ਜੇ (ਸਿਰਾ_lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਸਿਰਾ_lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਜੋੜੋ_lnum = TRUE;
	}
	ਜੇ (d != ਨਿਰਾ && (di = dict_ਲੱਭੋ(d, (char_u *)"types", -1)) != ਨਿਰਾ)
	{
	    ਜੇ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ਵਾਪਸ;
	    }

	    l = di->di_tv.vval.v_list;
	    ਜੇ (l != ਨਿਰਾ && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_ਨਾਂ(l, buf, &prop_types_len);
		ਜੇ (prop_types == ਨਿਰਾ)
		    ਵਾਪਸ;
	    }
	}
	ਜੇ (d != ਨਿਰਾ && (di = dict_ਲੱਭੋ(d, (char_u *)"ids", -1)) != ਨਿਰਾ)
	{
	    ਜੇ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ਜਾਓ errret;
	    }

	    l = di->di_tv.vval.v_list;
	    ਜੇ (l != ਨਿਰਾ && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		ਜੇ (prop_ids == ਨਿਰਾ)
		    ਜਾਓ errret;
	    }
	}
    }
    ਜੇ (ਸ਼ੁਰੂ_lnum < 1 || ਸ਼ੁਰੂ_lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
		|| ਸਿਰਾ_lnum < 1 || ਸਿਰਾ_lnum < ਸ਼ੁਰੂ_lnum)
	emsg(_(e_invalid_range));
    ਨਹੀਂਤਾਂ
	ਲਈ (lnum = ਸ਼ੁਰੂ_lnum; lnum <= ਸਿਰਾ_lnum; lnum++)
	    get_props_ਵਿੱਚ_ਸਤਰ(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, ਜੋੜੋ_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    ਖਾਲੀ
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	ਸ਼ੁਰੂ = 1;
    linenr_T	ਸਿਰਾ = 0;
    linenr_T	lnum;
    linenr_T	ਪਹਿਲਾ_ਬਦਲਿਆ = 0;
    linenr_T	ਆਖਰੀ_ਬਦਲਿਆ = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    ਪੂਰਨ		do_all;
    ਪੂਰਨ		id = -MAXCOL;
    ਪੂਰਨ		type_id = -1;	    // for a single "type"
    ਪੂਰਨ		*type_ids = ਨਿਰਾ;   // array, for a list of "types", allocated
    ਪੂਰਨ		ਅੰਕ_type_ids = 0;   // number of elements in "type_ids"
    ਪੂਰਨ		both;

    rettv->vval.v_ਸੰਖਿਆ = 0;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ਸ਼ੁਰੂ = tv_get_ਸੰਖਿਆ(&argvars[1]);
	ਸਿਰਾ = ਸ਼ੁਰੂ;
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	    ਸਿਰਾ = tv_get_ਸੰਖਿਆ(&argvars[2]);
	ਜੇ (ਸ਼ੁਰੂ < 1 || ਸਿਰਾ < 1)
	{
	    emsg(_(e_invalid_range));
	    ਵਾਪਸ;
	}
    }

    dict = argvars[0].vval.v_dict;
    ਜੇ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ਵਾਪਸ;
    ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ;

    do_all = dict_get_bool(dict, "all", FALSE);

    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "id"))
	id = dict_get_ਸੰਖਿਆ(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ਜੇ (type == ਨਿਰਾ)
	    ਵਾਪਸ;
	type_id = type->pt_id;
    }
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "types"))
    {
	typval_T types;
	listitem_T *li = ਨਿਰਾ;

	dict_get_tv(dict, "types", &types);
	ਜੇ (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( ਆਕਾਰ(ਪੂਰਨ) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		ਜੇ (li->li_tv.v_type != VAR_STRING)
		    ਜਾਰੀ;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		ਜੇ (!prop_type)
		    ਜਾਓ cleanup_prop_remove;

		type_ids[ਅੰਕ_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    ਜੇ (id == -MAXCOL && (type_id == -1 && ਅੰਕ_type_ids == 0))
    {
	emsg(_(e_need_at_ਘੱਟ_ਤੋਂ_ਘੱਟ_one_ਦਾ_id_or_type));
	ਜਾਓ cleanup_prop_remove;
    }
    ਜੇ (both && (id == -MAXCOL || (type_id == -1 && ਅੰਕ_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ਜਾਓ cleanup_prop_remove;
    }
    ਜੇ (type_id != -1 && ਅੰਕ_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	ਜਾਓ cleanup_prop_remove;
    }

    ਜੇ (ਸਿਰਾ == 0)
	ਸਿਰਾ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    {
	unpacked_memline_T um = um_open(buf);

	ਲਈ (lnum = ਸ਼ੁਰੂ; lnum <= ਸਿਰਾ; ++lnum)
	{
	    ਪੂਰਨ idx;

	    ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਰੋਕੋ;
	    ਜੇ (!um_goto_ਸਤਰ(&um, lnum, 0))
		ਰੋਕੋ;

	    ਲਈ (idx = 0; idx < um.prop_ਗਿਣਤੀ; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		ਪੂਰਨ	    matches_id = 0;
		ਪੂਰਨ	    matches_type = 0;

		ਜੇ (prop->tp_flags & TP_FLAG_DELETED)
		    ਜਾਰੀ;

		matches_id = prop->tp_id == id;
		ਜੇ (ਅੰਕ_type_ids > 0)
		{
		    ਪੂਰਨ idx2;

		    ਲਈ (idx2 = 0; !matches_type && idx2 < ਅੰਕ_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		ਨਹੀਂਤਾਂ
		    matches_type = prop->tp_type == type_id;

		ਜੇ (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    ਜੇ (ਪਹਿਲਾ_ਬਦਲਿਆ == 0)
			ਪਹਿਲਾ_ਬਦਲਿਆ = lnum;
		    ਆਖਰੀ_ਬਦਲਿਆ = lnum;
		    ++rettv->vval.v_ਸੰਖਿਆ;
		    ਜੇ (!do_all)
			ਰੋਕੋ;
		}
	    }
	}
	um_close(&um);
    }

    ਜੇ (ਪਹਿਲਾ_ਬਦਲਿਆ > 0)
    {
	ਬਦਲਿਆ_ਸਤਰ_display_buf(buf);
	ਬਦਲਿਆ_ਸਤਰਾਂ_buf(buf, ਪਹਿਲਾ_ਬਦਲਿਆ, ਆਖਰੀ_ਬਦਲਿਆ + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    ਸਥਿਰ ਖਾਲੀ
prop_type_ਸੈੱਟ(typval_T *argvars, ਪੂਰਨ ਜੋੜੋ)
{
    char_u	*name;
    buf_T	*buf = ਨਿਰਾ;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);
    ਜੇ (*name == NUL)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), "\"\"");
	ਵਾਪਸ;
    }

    ਜੇ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	ਵਾਪਸ;
    dict = argvars[1].vval.v_dict;

    prop = ਲੱਭੋ_prop_type(name, buf);
    ਜੇ (ਜੋੜੋ)
    {
	hashtab_T **htp;

	ਜੇ (prop != ਨਿਰਾ)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    ਵਾਪਸ;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	ਜੇ (prop == ਨਿਰਾ)
	    ਵਾਪਸ;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	ਜੇ (buf == ਨਿਰਾ)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	ਨਹੀਂਤਾਂ
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	ਜੇ (*htp == ਨਿਰਾ)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    ਜੇ (*htp == ਨਿਰਾ)
	    {
		vim_free(prop);
		ਵਾਪਸ;
	    }
	    hash_init(*htp);
	}
	hash_ਜੋੜੋ(*htp, PT2HIKEY(prop), "prop type");
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (prop == ਨਿਰਾ)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    ਵਾਪਸ;
	}
    }

    ਜੇ (dict != ਨਿਰਾ)
    {
	di = dict_ਲੱਭੋ(dict, (char_u *)"highlight", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    char_u	*highlight;
	    ਪੂਰਨ		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    ਜੇ (highlight != ਨਿਰਾ && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    ਜੇ (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_ਸਮੂਹ_name_str),
			highlight == ਨਿਰਾ ? (char_u *)"" : highlight);
		ਵਾਪਸ;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_ਲੱਭੋ(dict, (char_u *)"combine", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    ਜੇ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    ਨਹੀਂਤਾਂ
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_ਲੱਭੋ(dict, (char_u *)"override", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    ਜੇ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    ਨਹੀਂਤਾਂ
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_ਲੱਭੋ(dict, (char_u *)"priority", -1);
	ਜੇ (di != ਨਿਰਾ)
	    prop->pt_ਤਰਜੀਹ = tv_get_ਸੰਖਿਆ(&di->di_tv);

	di = dict_ਲੱਭੋ(dict, (char_u *)"start_incl", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    ਜੇ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_ਸ਼ੁਰੂ_INCL;
	    ਨਹੀਂਤਾਂ
		prop->pt_flags &= ~PT_FLAG_INS_ਸ਼ੁਰੂ_INCL;
	}

	di = dict_ਲੱਭੋ(dict, (char_u *)"end_incl", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    ਜੇ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_ਸਿਰਾ_INCL;
	    ਨਹੀਂਤਾਂ
		prop->pt_flags &= ~PT_FLAG_INS_ਸਿਰਾ_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    ਖਾਲੀ
f_prop_type_ਜੋੜੋ(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    prop_type_ਸੈੱਟ(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    ਖਾਲੀ
f_prop_type_change(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    prop_type_ਸੈੱਟ(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    ਖਾਲੀ
f_prop_type_delete(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    char_u	*name;
    buf_T	*buf = ਨਿਰਾ;
    hashitem_T	*hi;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);
    ਜੇ (*name == NUL)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), "\"\"");
	ਵਾਪਸ;
    }

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ਜੇ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ਵਾਪਸ;
    }

    hi = ਲੱਭੋ_prop_type_hi(name, buf);
    ਜੇ (hi == ਨਿਰਾ)
	ਵਾਪਸ;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    ਜੇ (buf == ਨਿਰਾ)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    ਨਹੀਂਤਾਂ
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    ਬਦਲਿਆ_ਖਿੜਕੀ_setting_buf(buf == ਨਿਰਾ ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    ਖਾਲੀ
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);
    ਜੇ (*name == NUL)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), "\"\"");
	ਵਾਪਸ;
    }

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    proptype_T  *prop = ਨਿਰਾ;
    buf_T	    *buf = ਨਿਰਾ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ਜੇ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ਵਾਪਸ;
    }

    prop = ਲੱਭੋ_prop_type(name, buf);
    ਜੇ (prop == ਨਿਰਾ)
	ਵਾਪਸ;

    dict_T *d = rettv->vval.v_dict;

    ਜੇ (prop->pt_hl_id > 0)
	dict_ਜੋੜੋ_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "priority", prop->pt_ਤਰਜੀਹ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_ਸ਼ੁਰੂ_INCL) ? 1 : 0);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_ਸਿਰਾ_INCL) ? 1 : 0);
    ਜੇ (buf != ਨਿਰਾ)
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "bufnr", buf->b_fnum);
}

    ਸਥਿਰ ਖਾਲੀ
list_types(hashtab_T *ht, list_T *l)
{
    ਲੰਮਾ	todo;
    hashitem_T	*hi;

    todo = (ਲੰਮਾ)ht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
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
    ਖਾਲੀ
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ਨਿਰਾ;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ਜੇ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    ਵਾਪਸ;
    }
    ਜੇ (buf == ਨਿਰਾ)
    {
	ਜੇ (global_proptypes != ਨਿਰਾ)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    ਨਹੀਂਤਾਂ ਜੇ (buf->b_proptypes != ਨਿਰਾ)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    ਸਥਿਰ ਖਾਲੀ
clear_ht_prop_types(hashtab_T *ht)
{
    ਲੰਮਾ	todo;
    hashitem_T	*hi;

    ਜੇ (ht == ਨਿਰਾ)
	ਵਾਪਸ;

    todo = (ਲੰਮਾ)ht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#ਜੇ defined(EXITFREE)
/*
 * Free all global property types.
 */
    ਖਾਲੀ
clear_global_prop_types(ਖਾਲੀ)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = ਨਿਰਾ;
    VIM_CLEAR(global_proparray);
}
#ਜੇ_ਅੰਤ

/*
 * Free all property types for "buf".
 */
    ਖਾਲੀ
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = ਨਿਰਾ;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ dirty;	    // if the property was changed
    ਪੂਰਨ can_drop;   // whether after this change, the prop may be removed
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
    ਸਥਿਰ adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    ਖਾਨਾ,
	ਪੂਰਨ	    added,
	ਪੂਰਨ	    flags)
{
    proptype_T	*pt;
    ਪੂਰਨ		ਸ਼ੁਰੂ_incl;
    ਪੂਰਨ		ਸਿਰਾ_incl;
    ਪੂਰਨ		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    ਜੇ (prop->tp_ਖਾਨਾ == MAXCOL)
    {
	res.dirty = FALSE;
	ਵਾਪਸ res;
    }

    pt = ਪਾਠ_prop_type_by_id(curbuf, prop->tp_type);
    ਸ਼ੁਰੂ_incl = (pt != ਨਿਰਾ && (pt->pt_flags & PT_FLAG_INS_ਸ਼ੁਰੂ_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    ਜੇ (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	ਸ਼ੁਰੂ_incl = FALSE;
    ਸਿਰਾ_incl = (pt != ਨਿਰਾ && (pt->pt_flags & PT_FLAG_INS_ਸਿਰਾ_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_ਅਗਲਾ);
    // do not drop zero-width props if they later can increase in size
    droppable = !(ਸ਼ੁਰੂ_incl || ਸਿਰਾ_incl);

    ਜੇ (added > 0)
    {
	ਜੇ (ਖਾਨਾ + 1 <= prop->tp_ਖਾਨਾ
		- (ਸ਼ੁਰੂ_incl || (prop->tp_len == 0 && ਸਿਰਾ_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_ਖਾਨਾ += added;
	ਨਹੀਂਤਾਂ ਜੇ (ਖਾਨਾ + 1 < prop->tp_ਖਾਨਾ + prop->tp_len + ਸਿਰਾ_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    ਨਹੀਂਤਾਂ ਜੇ (prop->tp_ਖਾਨਾ > ਖਾਨਾ + 1)
    {
	ਜੇ (prop->tp_ਖਾਨਾ + added < ਖਾਨਾ + 1)
	{
	    ਜੇ (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		prop->tp_len += (prop->tp_ਖਾਨਾ - 1 - ਖਾਨਾ) + added;
		prop->tp_ਖਾਨਾ = ਖਾਨਾ + 1;
		ਜੇ (prop->tp_len <= 0)
		{
		    prop->tp_len = 0;
		    // Multiline properties with no text left should
		    // also be dropped.
		    res.can_drop = droppable
			|| (prop->tp_flags
				& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_ਅਗਲਾ));
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    prop->tp_ਖਾਨਾ += added;
    }
    ਨਹੀਂਤਾਂ ਜੇ (prop->tp_len > 0 && prop->tp_ਖਾਨਾ + prop->tp_len > ਖਾਨਾ
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	ਜੇ (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = ਖਾਨਾ - (prop->tp_ਖਾਨਾ - 1);
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ after = ਖਾਨਾ - added - (prop->tp_ਖਾਨਾ - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	ਜੇ (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_ਅਗਲਾ)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_ਅਗਲਾ));
    }
    ਨਹੀਂਤਾਂ
	res.dirty = FALSE;

    ਵਾਪਸ res;
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
    ਪੂਰਨ
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    ਖਾਨਾ,
	ਪੂਰਨ	    ਬਾਈਟਾਂ_added,
	ਪੂਰਨ	    flags)
{
    unpacked_memline_T um;
    ਪੂਰਨ		dirty = FALSE;
    ਪੂਰਨ		ri;

    ਜੇ (ਪਾਠ_prop_frozen > 0)
	ਵਾਪਸ FALSE;

    um = um_open_at(curbuf, lnum, 0);
    ਜੇ (um.buf == ਨਿਰਾ || um.prop_ਗਿਣਤੀ == 0)
    {
	um_abort(&um);
	ਵਾਪਸ FALSE;
    }

    ਲਈ (ri = 0; ri < um.prop_ਗਿਣਤੀ; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, ਖਾਨਾ, ਬਾਈਟਾਂ_added, flags);
	ਜੇ (res.dirty)
	{
	    ਜੇ (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		ਜੇ (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    ਵਾਪਸ FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		ਜੇ ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    ਵਾਪਸ FALSE;
		}
	    }
	    dirty = TRUE;
	}
	ਜੇ (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    ਜਾਰੀ;
	}
    }
    ਜੇ (dirty)
	um_close(&um);
    ਨਹੀਂਤਾਂ
	um_abort(&um);
    ਵਾਪਸ dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    ਖਾਲੀ
adjust_props_for_ਵੰਡੋ(
	linenr_T    lnum_props,
	linenr_T    lnum_ਸਿਖਰ,
	ਪੂਰਨ	    kept,
	ਪੂਰਨ	    deleted,
	ਪੂਰਨ	    at_eol)
{
    char_u	*props;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    garray_T    prevprop;
    garray_T    nextprop;
    ਪੂਰਨ		skipped = kept + deleted;

    ਜੇ (!curbuf->b_has_textprop)
	ਵਾਪਸ;

    // Get the text properties from "lnum_props".
    ਗਿਣਤੀ_2 = get_ਪਾਠ_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, ਆਕਾਰ(textprop_T), 10);
    ga_init2(&nextprop, ਆਕਾਰ(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *ਗਿਣਤੀ_ptr = props - PROP_ਗਿਣਤੀ_ਮਾਪ;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	ਪੂਰਨ	    ਸ਼ੁਰੂ_incl, ਸਿਰਾ_incl;
	ਪੂਰਨ	    cont_prev, cont_ਅਗਲਾ;
	ਪੂਰਨ	    prop_ਖਾਨਾ;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * ਆਕਾਰ(textprop_T), ਆਕਾਰ(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	ਜੇ (prop.tp_id < 0 && prop.u.tp_ਪਾਠ_ਵਿੱਥ > 0)
	{
	    prop.u.tp_ਪਾਠ = vim_strsave(ਗਿਣਤੀ_ptr + prop.u.tp_ਪਾਠ_ਵਿੱਥ);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ਨਹੀਂਤਾਂ
	    prop.u.tp_ਪਾਠ = ਨਿਰਾ;

	pt = ਪਾਠ_prop_type_by_id(curbuf, prop.tp_type);
	ਸ਼ੁਰੂ_incl = (pt != ਨਿਰਾ && (pt->pt_flags & PT_FLAG_INS_ਸ਼ੁਰੂ_INCL));
	ਸਿਰਾ_incl = (pt != ਨਿਰਾ && (pt->pt_flags & PT_FLAG_INS_ਸਿਰਾ_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_ਖਾਨਾ = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_ਖਾਨਾ;

	ਜੇ (prop_ਖਾਨਾ == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_ਅਗਲਾ = !at_eol;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    ਪੂਰਨ is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_ਖਾਨਾ == MAXCOL);

	    cont_prev = prop_ਖਾਨਾ + !ਸ਼ੁਰੂ_incl <= kept;
	    ਜੇ (is_floating_vtext)
		cont_ਅਗਲਾ = FALSE;
	    ਨਹੀਂਤਾਂ
		cont_ਅਗਲਾ = skipped <= prop_ਖਾਨਾ + prop.tp_len - !ਸਿਰਾ_incl;
	}
	// when a prop has text it is never copied
	ਜੇ (prop.tp_id < 0 && cont_ਅਗਲਾ)
	    cont_prev = FALSE;

	ਜੇ (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_ਅੰਕੜੇ) + prevprop.ga_len;
	    ਪੂਰਨ is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    ਜੇ (!is_vtext && p->tp_ਖਾਨਾ + p->tp_len >= kept)
		p->tp_len = kept - p->tp_ਖਾਨਾ;
	    ਜੇ (cont_ਅਗਲਾ)
		p->tp_flags |= TP_FLAG_CONT_ਅਗਲਾ;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	ਜੇ (cont_ਅਗਲਾ && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_ਅੰਕੜੇ) + nextprop.ga_len;
	    ਪੂਰਨ is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    ਜੇ (p->tp_ਖਾਨਾ != MAXCOL)
	    {
		ਜੇ (p->tp_ਖਾਨਾ > skipped)
		    p->tp_ਖਾਨਾ -= skipped - 1;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (!is_vtext)
			p->tp_len -= skipped - p->tp_ਖਾਨਾ;
		    p->tp_ਖਾਨਾ = 1;
		}
	    }
	    ਜੇ (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    ਸੈੱਟ_ਪਾਠ_props(lnum_ਸਿਖਰ, (textprop_T *)prevprop.ga_ਅੰਕੜੇ,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    ਸੈੱਟ_ਪਾਠ_props(lnum_ਸਿਖਰ + 1, (textprop_T *)nextprop.ga_ਅੰਕੜੇ,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    ਖਾਲੀ
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	ਪੂਰਨ		    ਆਖਰੀ_ਸਤਰ,
	ਲੰਮਾ		    ਖਾਨਾ,
	ਪੂਰਨ		    removed)
{
    unpacked_memline_T	r_um;

    ਜੇ (um->buf == ਨਿਰਾ)
	ਵਾਪਸ;

    r_um = um_open_at(um->buf, lnum, 0);
    ਜੇ (r_um.buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (!um_detach(&r_um))
    {
	um_abort(&r_um);
	ਵਾਪਸ;
    }

    ਲਈ (ਪੂਰਨ i = r_um.prop_ਗਿਣਤੀ - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	ਪੂਰਨ	    ਸਿਰਾ;

	ਜੇ (prop->tp_ਖਾਨਾ == MAXCOL && !ਆਖਰੀ_ਸਤਰ)
	    ਜਾਰੀ;  // drop floating text for non-last lines
	ਸਿਰਾ = !(prop->tp_flags & TP_FLAG_CONT_ਅਗਲਾ);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, ਖਾਨਾ, 0);

	ਜੇ (ਆਖਰੀ_ਸਤਰ || ਸਿਰਾ)
	{
	    um_ਜੋੜੋ_prop(um, prop);
	    prop->u.tp_ਪਾਠ = ਨਿਰਾ;  // ownership transferred
	}
	ਨਹੀਂਤਾਂ
	{
	    // Search for continuing prop in um.
	    bool    ਮਿਲਿਆ = false;

	    ਲਈ (ਪੂਰਨ j = 0; j < um->prop_ਗਿਣਤੀ; ++j)
	    {
		textprop_T *op = &um->props[j];

		ਜੇ ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    ਮਿਲਿਆ = true;
		    op->tp_len += op->tp_ਖਾਨਾ - prop->tp_ਖਾਨਾ;
		    op->tp_ਖਾਨਾ = prop->tp_ਖਾਨਾ;
		    op->tp_flags = prop->tp_flags;
		    ਰੋਕੋ;
		}
	    }
	    ਜੇ (!ਮਿਲਿਆ)
		ਅੰਦਰੂਨੀ_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    ਸਥਿਰ ਖਾਲੀ
remember_cleared_prop(garray_T *gap, ਪੂਰਨ which, textprop_T *prop, ਪੂਰਨ flag)
{
    undoprop_T	*ਉੱਪਰ;

    ਜੇ (which == UNDOPROP_NONE)
	ਵਾਪਸ;
    ਜੇ (gap->ga_itemsize == 0)
	ga_init2(gap, ਆਕਾਰ(undoprop_T), 4);
    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ;
    ਉੱਪਰ = (undoprop_T *)gap->ga_ਅੰਕੜੇ + gap->ga_len++;
    ਉੱਪਰ->ਉੱਪਰ_which = which;
    ਉੱਪਰ->ਉੱਪਰ_type = prop->tp_type;
    ਉੱਪਰ->ਉੱਪਰ_id = prop->tp_id;
    ਉੱਪਰ->ਉੱਪਰ_ਖਾਨਾ = prop->tp_ਖਾਨਾ;
    ਉੱਪਰ->ਉੱਪਰ_flag = flag;
}

/*
 * Clear the continuation flags between line "lnum" and the line below it that
 * have no partner in the other line.  A property is split there then.  What
 * is cleared on the line above is remembered in "gap" as "above_which", on
 * the line below as "below_which".
 */
    ਖਾਲੀ
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	ਪੂਰਨ	    above_which,
	ਪੂਰਨ	    ਹੇਠਾਂ_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	ਹੇਠਾਂ;
    bool		*paired = ਨਿਰਾ;
    bool		above_ਬਦਲਿਆ = false;
    bool		ਹੇਠਾਂ_ਬਦਲਿਆ = false;
    bool		cont_ਅਗਲਾ = false;

    ਜੇ (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ;

    above = um_open_at(buf, lnum, 0);
    ਜੇ (above.buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਲਈ (ਪੂਰਨ i = 0; i < above.prop_ਗਿਣਤੀ; ++i)
	ਜੇ (above.props[i].tp_flags & TP_FLAG_CONT_ਅਗਲਾ)
	    cont_ਅਗਲਾ = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    ਜੇ (cont_ਅਗਲਾ && !um_detach(&above))
    {
	um_abort(&above);
	ਵਾਪਸ;
    }

    ਹੇਠਾਂ = um_open_at(buf, lnum + 1, 0);
    ਜੇ (ਹੇਠਾਂ.buf == ਨਿਰਾ)
    {
	um_abort(&above);
	ਵਾਪਸ;
    }
    ਜੇ (ਹੇਠਾਂ.prop_ਗਿਣਤੀ > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, ਹੇਠਾਂ.prop_ਗਿਣਤੀ);
	ਜੇ (paired == ਨਿਰਾ)
	    ਜਾਓ theend;
    }

    ਲਈ (ਪੂਰਨ i = 0; i < above.prop_ਗਿਣਤੀ; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    ਮਿਲਿਆ = false;

	ਜੇ (!(prop->tp_flags & TP_FLAG_CONT_ਅਗਲਾ))
	    ਜਾਰੀ;
	ਲਈ (ਪੂਰਨ j = 0; j < ਹੇਠਾਂ.prop_ਗਿਣਤੀ; ++j)
	{
	    textprop_T *ਅਗਲਾ = &ਹੇਠਾਂ.props[j];

	    ਜੇ (!paired[j] && (ਅਗਲਾ->tp_flags & TP_FLAG_CONT_PREV)
		    && ਅਗਲਾ->tp_id == prop->tp_id
		    && ਅਗਲਾ->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		ਮਿਲਿਆ = true;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (!ਮਿਲਿਆ)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_ਅਗਲਾ;
	    above_ਬਦਲਿਆ = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_ਅਗਲਾ);
	}
    }

    ਲਈ (ਪੂਰਨ j = 0; j < ਹੇਠਾਂ.prop_ਗਿਣਤੀ; ++j)
    {
	ਜੇ (!(ਹੇਠਾਂ.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    ਜਾਰੀ;
	ਜੇ (!ਹੇਠਾਂ.detached && !um_detach(&ਹੇਠਾਂ))
	    ਰੋਕੋ;
	ਹੇਠਾਂ.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	ਹੇਠਾਂ_ਬਦਲਿਆ = true;
	remember_cleared_prop(gap, ਹੇਠਾਂ_which, &ਹੇਠਾਂ.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    ਜੇ (ਹੇਠਾਂ_ਬਦਲਿਆ)
	um_close(&ਹੇਠਾਂ);
    ਨਹੀਂਤਾਂ
	um_abort(&ਹੇਠਾਂ);
    ਜੇ (above_ਬਦਲਿਆ)
	um_close(&above);
    ਨਹੀਂਤਾਂ
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    ਖਾਲੀ
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	ਪੂਰਨ	    which)
{
    unpacked_memline_T	um;
    bool		ਬਦਲਿਆ = false;

    ਜੇ (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ;
    um = um_open_at(buf, lnum, 0);
    ਜੇ (um.buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਲਈ (ਪੂਰਨ r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *ਉੱਪਰ = (undoprop_T *)gap->ga_ਅੰਕੜੇ + r;

	ਜੇ (ਉੱਪਰ->ਉੱਪਰ_which != which)
	    ਜਾਰੀ;
	ਲਈ (ਪੂਰਨ i = 0; i < um.prop_ਗਿਣਤੀ; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    ਜੇ (prop->tp_type != ਉੱਪਰ->ਉੱਪਰ_type || prop->tp_id != ਉੱਪਰ->ਉੱਪਰ_id
		    || prop->tp_ਖਾਨਾ != ਉੱਪਰ->ਉੱਪਰ_ਖਾਨਾ
		    || (prop->tp_flags & ਉੱਪਰ->ਉੱਪਰ_flag))
		ਜਾਰੀ;
	    ਜੇ (!um.detached && !um_detach(&um))
		ਜਾਓ theend;
	    um.props[i].tp_flags |= ਉੱਪਰ->ਉੱਪਰ_flag;
	    ਬਦਲਿਆ = true;
	    ਰੋਕੋ;
	}
    }

theend:
    ਜੇ (ਬਦਲਿਆ)
	um_close(&um);
    ਨਹੀਂਤਾਂ
	um_abort(&um);
}

    bool
ਪਾਠ_prop_ਗਿਣਤੀ_valid(ਪੂਰਨ prop_ਗਿਣਤੀ, size_t propdata_len)
{
    ਜੇ (propdata_len < PROP_ਗਿਣਤੀ_ਮਾਪ)
	ਵਾਪਸ false;
    ਵਾਪਸ (size_t)prop_ਗਿਣਤੀ * ਆਕਾਰ(textprop_T)
		    <= propdata_len - PROP_ਗਿਣਤੀ_ਮਾਪ;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
ਪਾਠ_prop_vtext_valid(char_u *props, ਪੂਰਨ prop_ਗਿਣਤੀ, size_t propdata_len)
{
    ਲਈ (ਪੂਰਨ i = 0; i < prop_ਗਿਣਤੀ; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * ਆਕਾਰ(textprop_T),
							  ਆਕਾਰ(textprop_T));
	ਜੇ (prop.tp_id >= 0 || prop.u.tp_ਪਾਠ_ਵਿੱਥ <= 0)
	    ਜਾਰੀ;
	ਜੇ (prop.tp_len < 0 || (size_t)prop.u.tp_ਪਾਠ_ਵਿੱਥ
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    ਵਾਪਸ false;
    }
    ਵਾਪਸ true;
}

#ਜੇ_ਅੰਤ // FEAT_PROP_POPUP
