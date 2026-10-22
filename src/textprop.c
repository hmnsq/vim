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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_PROP_POPUP)

ಸ್ಥಿರ ಶೂನ್ಯ um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
um_free_vtext(textprop_T *props, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ; ++i)
	ಯದಿ (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
um_free(unpacked_memline_T *um)
{
    ಯದಿ (um->detached)
    {
	VIM_CLEAR(um->text);
	um_free_vtext(um->props, um->prop_ಎಣಿಕೆ);
    }
    VIM_CLEAR(um->props);
    um->prop_size = 0;
    um->prop_ಎಣಿಕೆ = 0;
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
    ಪ್ರತ್ಯಾಗಮ um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_ಸಾಲು(unpacked_memline_T *um, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ extra_props)
{
    char_u	*ಸಾಲು;
    size_t	textlen;
    size_t	propdata_len;
    ಪೂರ್ಣಾಙ್ಕ		proplen;

    ಯದಿ (um->buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ false;

    // Store changes to the current line if any.
    ಯದಿ (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    ಯದಿ (lnum == 0)
	ಪ್ರತ್ಯಾಗಮ true;	// just unload

    ಸಾಲು = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_ಸಾಲು_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = ಸಾಲು;

    ಯದಿ (propdata_len == 0)
	ಪ್ರತ್ಯಾಗಮ true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    ಯದಿ (propdata_len < PROP_ಎಣಿಕೆ_SIZE + ಪರಿಮಾಣ(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ false;
    }

    uint16_t    prop_ಎಣಿಕೆ;
    char_u	    *ಎಣಿಕೆ_ptr = ಸಾಲು + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_ಎಣಿಕೆ, ಎಣಿಕೆ_ptr, PROP_ಎಣಿಕೆ_SIZE);
    ಯದಿ (!text_prop_ಎಣಿಕೆ_valid(prop_ಎಣಿಕೆ, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ false;
    }
    proplen = (ಪೂರ್ಣಾಙ್ಕ)prop_ಎಣಿಕೆ;
    props_start = ಎಣಿಕೆ_ptr + PROP_ಎಣಿಕೆ_SIZE;

    ಯದಿ (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    ಯದಿ (um->props == ರಿಕ್ತ)
    {
	um->buf = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_ಎಣಿಕೆ = proplen;
    mch_memmove(um->props, props_start, proplen * ಪರಿಮಾಣ(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < proplen; ++i)
    {
	ಯದಿ (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = ಎಣಿಕೆ_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ಅನ್ಯಥಾ
	    um->props[i].u.tp_text = ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ extra_props)
{
    unpacked_memline_T um = um_open(buf);

    ಯದಿ (!um_goto_ಸಾಲು(&um, lnum, extra_props))
	um.buf = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    ಸ್ಥಿರ bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    ಯದಿ (um->detached || um->buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    ಯದಿ (newtext == ರಿಕ್ತ)
    {
	um->buf = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ false;
    }
    um->text = newtext;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < um->prop_ಎಣಿಕೆ; ++i)
    {
	ಯದಿ (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    ಯದಿ (copy == ರಿಕ್ತ)
	    {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < i; ++j)
		    ಯದಿ (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = ರಿಕ್ತ;
		ಪ್ರತ್ಯಾಗಮ false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    ಪ್ರತ್ಯಾಗಮ true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    ಯದಿ (um->buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ false;
    ಯದಿ (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    ಪ್ರತ್ಯಾಗಮ true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    ಸ್ಥಿರ bool
um_grow_props(unpacked_memline_T *um, ಪೂರ್ಣಾಙ್ಕ needed)
{
    ಯದಿ (um->prop_ಎಣಿಕೆ + needed <= um->prop_size)
	ಪ್ರತ್ಯಾಗಮ true;

    ಪೂರ್ಣಾಙ್ಕ	    new_size = um->prop_ಎಣಿಕೆ + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * ಪರಿಮಾಣ(textprop_T));
    ಯದಿ (newprops == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ false;
    um->props = newprops;
    um->prop_size = new_size;

    ಪ್ರತ್ಯಾಗಮ true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    ಯದಿ (um->buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!um->detached && !um_detach(um))
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!um_grow_props(um, 1))
    {
	um->buf = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }
    um->props[um->prop_ಎಣಿಕೆ++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    ಶೂನ್ಯ
um_reverse_props(unpacked_memline_T *um)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0, j = um->prop_ಎಣಿಕೆ - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ prop_ಎಣಿಕೆ)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    ಯದಿ (lnum < 1 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	um.buf = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ um;
    }

    char_u *ಸಾಲು = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(ಸಾಲು);

    ಯದಿ (text_copy == ರಿಕ್ತ)
    {
	um.buf = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    ಯದಿ (prop_ಎಣಿಕೆ > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_ಎಣಿಕೆ);
	ಯದಿ (um.props == ರಿಕ್ತ)
	{
	    vim_free(um.text);
	    um.buf = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ um;
	}
	um.prop_size = prop_ಎಣಿಕೆ;
    }

    ಪ್ರತ್ಯಾಗಮ um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					ಪೂರ್ಣಾಙ್ಕ tp_id, ಪೂರ್ಣಾಙ್ಕ tp_type, ಪೂರ್ಣಾಙ್ಕ flag)
{
    unpacked_memline_T	neighbor;

    ಯದಿ (lnum < 1 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;

    neighbor = um_open_at(buf, lnum, 0);
    ಯದಿ (neighbor.buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < neighbor.prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	ಯದಿ (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    ಯದಿ (!neighbor.detached && !um_detach(&neighbor))
		ಭಙ್ಗ;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    ಭಙ್ಗ;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    ಶೂನ್ಯ
um_delete_prop(unpacked_memline_T *um, ಪೂರ್ಣಾಙ್ಕ index)
{
    textprop_T *prop;

    ಯದಿ (um->buf == ರಿಕ್ತ || index < 0 || index >= um->prop_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!um->detached && !um_detach(um))
	ಪ್ರತ್ಯಾಗಮ;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    ಯದಿ (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    ಯದಿ (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    ಯದಿ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
    ಸ್ಥಿರ char_u *
um_pack(unpacked_memline_T *um, ಪೂರ್ಣಾಙ್ಕ *packed_len)
{
    uint16_t	live_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		vtext_size = 0;
    ಪೂರ್ಣಾಙ್ಕ		total_len;
    char_u	*buf;
    char_u	*ಎಣಿಕೆ_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < um->prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T *prop = &um->props[i];

	ಯದಿ (prop->tp_flags & TP_FLAG_DELETED)
	    ಅನುವರ್ತ;
	++live_ಎಣಿಕೆ;
	ಯದಿ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    ಯದಿ (live_ಎಣಿಕೆ == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	ಯದಿ (buf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	ಪ್ರತ್ಯಾಗಮ buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (ಪೂರ್ಣಾಙ್ಕ)PROP_ಎಣಿಕೆ_SIZE
		+ live_ಎಣಿಕೆ * (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(textprop_T) + vtext_size;
    buf = alloc(total_len);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    ಎಣಿಕೆ_dest = buf + um->text_size;
    mch_memmove(ಎಣಿಕೆ_dest, &live_ಎಣಿಕೆ, PROP_ಎಣಿಕೆ_SIZE);

    prop_dest = ಎಣಿಕೆ_dest + PROP_ಎಣಿಕೆ_SIZE;
    vtext_dest = prop_dest + live_ಎಣಿಕೆ * ಪರಿಮಾಣ(textprop_T);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < um->prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T prop = um->props[i];

	ಯದಿ (prop.tp_flags & TP_FLAG_DELETED)
	    ಅನುವರ್ತ;

	ಯದಿ (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    ಪೂರ್ಣಾಙ್ಕ copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - ಎಣಿಕೆ_dest);
	    vtext_dest += copysize;
	}
	ಅನ್ಯಥಾ
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, ಪರಿಮಾಣ(textprop_T));
	prop_dest += ಪರಿಮಾಣ(textprop_T);
    }

    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    ಪೂರ್ಣಾಙ್ಕ	    packed_len;

    ಯದಿ (!um->detached || um->buf == ರಿಕ್ತ || um->lnum == 0)
	ಪ್ರತ್ಯಾಗಮ;

    packed = um_pack(um, &packed_len);
    ಯದಿ (packed == ರಿಕ್ತ)
    {
	um->buf = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    ಯದಿ (um->buf->b_ml.ml_ಸಾಲು_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    ಅನ್ಯಥಾ ಯದಿ (!(um->buf->b_ml.ml_flags & ML_ಸಾಲು_DIRTY))
	(ಶೂನ್ಯ)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_ಎಣಿಕೆ);

    ಯದಿ (um->buf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_ಸಾಲು_ptr);
    um->buf->b_ml.ml_ಸಾಲು_ptr = packed;
    um->buf->b_ml.ml_ಸಾಲು_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;

    um->detached = false;
    um->text = packed;
    um->text_size = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_ಸಾಲು_textlen = um->text_size;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    ಶೂನ್ಯ
um_close(unpacked_memline_T *um)
{
    ಯದಿ (um->buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = ರಿಕ್ತ;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    ಶೂನ್ಯ
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = ರಿಕ್ತ;
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
#define HI2PT(hi)      HIKEY2PT((hi)->hi_ಕೀಲಿ)

// The global text property types.
ಸ್ಥಿರ hashtab_T *global_proptypes = ರಿಕ್ತ;
ಸ್ಥಿರ proptype_T **global_proparray = ರಿಕ್ತ;

// The last used text property type ID.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    ಸ್ಥಿರ hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    ಯದಿ (*name == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (buf == ರಿಕ್ತ)
	ht = global_proptypes;
    ಅನ್ಯಥಾ
	ht = buf->b_proptypes;

    ಯದಿ (ht == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    hi = hash_find(ht, name);
    ಯದಿ (HASHITEM_EMPTY(hi))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    ಸ್ಥಿರ proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    ಯದಿ (hi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    ಪೂರ್ಣಾಙ್ಕ
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    ಯದಿ (pt == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    ಸ್ಥಿರ proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = find_prop_type(name, buf);

    ಯದಿ (type == ರಿಕ್ತ)
	type = find_prop_type(name, ರಿಕ್ತ);
    ಯದಿ (type == ರಿಕ್ತ)
	semsg(_(e_property_type_str_does_not_exist), name);
    ಪ್ರತ್ಯಾಗಮ type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    ಯದಿ (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (arg->vval.v_dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    ಯದಿ (di != ರಿಕ್ತ && (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ
					      || di->di_tv.vval.v_ಸಂಖ್ಯೆ != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	ಯದಿ (*buf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    ಶೂನ್ಯ
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    start_lnum = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    start_col = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    ಯದಿ (check_for_dict_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	ಪೂರ್ಣಾಙ್ಕ		id,
	char_u		*text_arg,
	ಪೂರ್ಣಾಙ್ಕ		text_padding_ಎಡ,
	ಪೂರ್ಣಾಙ್ಕ		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		proplen;
    char_u	*props = ರಿಕ್ತ;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    ಪೂರ್ಣಾಙ್ಕ		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    ಪೂರ್ಣಾಙ್ಕ		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    ಯದಿ (type == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    ಯದಿ (start_lnum < 1 || start_lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), (long)start_lnum);
	ಗಚ್ಛ theend;
    }
    ಯದಿ (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), (long)end_lnum);
	ಗಚ್ಛ theend;
    }

    ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	ಗಚ್ಛ theend;
    }

    ಯದಿ (text != ರಿಕ್ತ)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	ಪ್ರತಿ (p = text; *p != NUL; MB_PTR_ADV(p))
	    ಯದಿ (*p < ' ')
		*p = ' ';
    }

    ಪ್ರತಿ (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	ಯದಿ (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_ಸಾಲು));
	    ಗಚ್ಛ theend;
	}

	ಯದಿ (lnum == start_lnum)
	    col = start_col;
	ಅನ್ಯಥಾ
	    col = 1;
	ಯದಿ (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != ರಿಕ್ತ))
	{
	    semsg(_(e_invalid_column_ಸಂಖ್ಯೆ_nr), (long)start_col);
	    ಗಚ್ಛ theend;
	}
	sort_col = col;

	ಯದಿ (lnum == end_lnum)
	    length = end_col - col;
	ಅನ್ಯಥಾ
	    length = (ಪೂರ್ಣಾಙ್ಕ)textlen - col + 1;
	ಯದಿ (length > (long)textlen)
	    length = (ಪೂರ್ಣಾಙ್ಕ)textlen;	// can include the end-of-line
	ಯದಿ (length < 0)
	    length = 0;		// zero-width property

	ಯದಿ (text_arg != ರಿಕ್ತ)
	{
	    length = (long)STRLEN(text_arg);
	    ಯದಿ (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		ಯದಿ ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	tmp_prop.tp_padleft = text_padding_ಎಡ;
	ಯದಿ (text_arg != ರಿಕ್ತ)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	ಪ್ರತಿ (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * ಪರಿಮಾಣ(textprop_T),
							   ಪರಿಮಾಣ(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    ಯದಿ (prop_col >= sort_col)
		ಭಙ್ಗ;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	ಪೂರ್ಣಾಙ್ಕ		vtext_total = 0;
	ಪೂರ್ಣಾಙ್ಕ		new_ಸಾಲು_len;
	char_u	*ಎಣಿಕೆ_dest;
	char_u	*vtext_dest;
	ಪೂರ್ಣಾಙ್ಕ		j;

	// Compute total vtext size from existing props.
	ಪ್ರತಿ (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * ಪರಿಮಾಣ(textprop_T),
							ಪರಿಮಾಣ(textprop_T));
	    ಯದಿ (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	ಯದಿ (text_arg != ರಿಕ್ತ)
	    vtext_total += length + 1;

	new_ಸಾಲು_len = (ಪೂರ್ಣಾಙ್ಕ)textlen + (ಪೂರ್ಣಾಙ್ಕ)PROP_ಎಣಿಕೆ_SIZE
			+ new_propcount * (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(textprop_T)
			+ vtext_total;
	newtext = alloc(new_ಸಾಲು_len);
	ಯದಿ (newtext == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_ಸಾಲು_ptr, textlen);

	// Write prop_count.
	ಎಣಿಕೆ_dest = newtext + textlen;
	mch_memmove(ಎಣಿಕೆ_dest, &new_propcount, PROP_ಎಣಿಕೆ_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = ಎಣಿಕೆ_dest + PROP_ಎಣಿಕೆ_SIZE;
	ಯದಿ (i > 0)
	    mch_memmove(newprops, props, ಪರಿಮಾಣ(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	ಯದಿ (i < proplen)
	    mch_memmove(newprops + (i + 1) * ಪರಿಮಾಣ(textprop_T),
					props + i * ಪರಿಮಾಣ(textprop_T),
					ಪರಿಮಾಣ(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * ಪರಿಮಾಣ(textprop_T);
	ಪ್ರತಿ (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    ಯದಿ (j == i)
		ಅನುವರ್ತ;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * ಪರಿಮಾಣ(textprop_T),
		    ಪರಿಮಾಣ(textprop_T));
	    ಯದಿ (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_ಎಣಿಕೆ_2 = (char_u *)props - PROP_ಎಣಿಕೆ_SIZE;
		char_u *old_vtext = old_ಎಣಿಕೆ_2 + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - ಎಣಿಕೆ_dest);
		mch_memmove(newprops + j * ಪರಿಮಾಣ(textprop_T), &ep,
			ಪರಿಮಾಣ(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	ಯದಿ (text_arg != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - ಎಣಿಕೆ_dest);
	    vtext_dest += copysize;
	}
	ಅನ್ಯಥಾ
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * ಪರಿಮಾಣ(textprop_T), &tmp_prop,
							ಪರಿಮಾಣ(textprop_T));

	ಯದಿ (buf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_ಸಾಲು_ptr);
	buf->b_ml.ml_ಸಾಲು_ptr = newtext;
	buf->b_ml.ml_ಸಾಲು_len = new_ಸಾಲು_len;
	buf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;
    }

    changed_ಸಾಲು_display_buf(buf);
    changed_ಸಾಲುಗಳು_buf(buf, start_lnum, end_lnum + 1, 0);
    res = OK;

theend:
    vim_free(text);
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    ಶೂನ್ಯ
f_prop_add_list(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    ಪೂರ್ಣಾಙ್ಕ		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		prev_did_emsg = did_emsg;

    ಯದಿ (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    dict = argvars[0].vval.v_dict;
    ಯದಿ (dict == ರಿಕ್ತ || !dict_has_ಕೀಲಿ(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ಪ್ರತ್ಯಾಗಮ;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ಯದಿ (dict_has_ಕೀಲಿ(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_ಸಂಖ್ಯೆ(dict, "id");
	ಯದಿ (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	id = (ಪೂರ್ಣಾಙ್ಕ)x;
    }

    ಯದಿ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_ವಸ್ತುಗಳು(argvars[1].vval.v_list, li)
    {
	ಯದಿ (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == ರಿಕ್ತ)
	{
	    emsg(_(e_list_required));
	    ಪ್ರತ್ಯಾಗಮ;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	ಯದಿ (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	ಯದಿ (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	ಯದಿ (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	ಪೂರ್ಣಾಙ್ಕ this_id = id;
	ಯದಿ (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	ಯದಿ (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    ಯದಿ (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (prop_add_one(buf, type_name, this_id, ರಿಕ್ತ, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_textprop_id(ಶೂನ್ಯ)
{
    ಯದಿ (vt_id_counter > 0)
	vt_id_counter = 0;
    ಪ್ರತ್ಯಾಗಮ --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    ಪೂರ್ಣಾಙ್ಕ
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
    ಪೂರ್ಣಾಙ್ಕ		id = 0;
    char_u	*text = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		text_padding_ಎಡ = 0;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;

    ಯದಿ (dict == ರಿಕ್ತ || !dict_has_ಕೀಲಿ(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ಗಚ್ಛ theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ಯದಿ (dict_has_ಕೀಲಿ(dict, "end_lnum"))
    {
	end_lnum = dict_get_ಸಂಖ್ಯೆ(dict, "end_lnum");
	ಯದಿ (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "end_lnum");
	    ಗಚ್ಛ theend;
	}
    }
    ಅನ್ಯಥಾ
	end_lnum = start_lnum;

    ಯದಿ (dict_has_ಕೀಲಿ(dict, "length"))
    {
	long length = dict_get_ಸಂಖ್ಯೆ(dict, "length");

	ಯದಿ (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "length");
	    ಗಚ್ಛ theend;
	}
	end_col = start_col + length;
    }
    ಅನ್ಯಥಾ ಯದಿ (dict_has_ಕೀಲಿ(dict, "end_col"))
    {
	end_col = dict_get_ಸಂಖ್ಯೆ(dict, "end_col");
	ಯದಿ (end_col <= 0)
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "end_col");
	    ಗಚ್ಛ theend;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (start_lnum == end_lnum)
	end_col = start_col;
    ಅನ್ಯಥಾ
	end_col = 1;

    ಯದಿ (dict_has_ಕೀಲಿ(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_ಸಂಖ್ಯೆ(dict, "id");
	ಯದಿ (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ಗಚ್ಛ theend;
	}
	id = (ಪೂರ್ಣಾಙ್ಕ)x;
    }

    ಯದಿ (dict_has_ಕೀಲಿ(dict, "text"))
    {
	ಯದಿ (dict_has_ಕೀಲಿ(dict, "length")
		|| dict_has_ಕೀಲಿ(dict, "end_col")
		|| dict_has_ಕೀಲಿ(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    ಗಚ್ಛ theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	ಯದಿ (text == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	ಯದಿ (dict_has_ಕೀಲಿ(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    ಯದಿ (p == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    ಯದಿ (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		ಗಚ್ಛ theend;
	    }
	    ಯದಿ (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_ಬಲ;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "text_align", p);
		ಗಚ್ಛ theend;
	    }
	}

	ಯದಿ (dict_has_ಕೀಲಿ(dict, "text_padding_left"))
	{
	    text_padding_ಎಡ = dict_get_ಸಂಖ್ಯೆ(dict, "text_padding_left");
	    ಯದಿ (text_padding_ಎಡ < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		ಗಚ್ಛ theend;
	    }
	}

	ಯದಿ (dict_has_ಕೀಲಿ(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    ಯದಿ (p == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    ಯದಿ (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "text_wrap", p);
		ಗಚ್ಛ theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    ಯದಿ (start_col < (text == ರಿಕ್ತ ? 1 : 0))
    {
	semsg(_(e_invalid_column_ಸಂಖ್ಯೆ_nr), (long)start_col);
	ಗಚ್ಛ theend;
    }
    ಯದಿ (start_col > 0 && text_padding_ಎಡ > 0)
    {
	emsg(_(e_can_only_use_ಎಡ_padding_when_column_is_zero));
	ಗಚ್ಛ theend;
    }

    ಯದಿ (dict_arg != ರಿಕ್ತ && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	ಗಚ್ಛ theend;

    ಯದಿ (text != ರಿಕ್ತ)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    ಅನ್ಯಥಾ ಯದಿ (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	ಗಚ್ಛ theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_ಎಡ, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = ರಿಕ್ತ;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    ಪ್ರತ್ಯಾಗಮ id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    ಪೂರ್ಣಾಙ್ಕ
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, ಪೂರ್ಣಾಙ್ಕ will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_ಎಣಿಕೆ;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    ಯದಿ ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_ಸಾಲು_len - textlen;
    ಯದಿ (propdata_len == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    ಯದಿ (propdata_len < PROP_ಎಣಿಕೆ_SIZE + ಪರಿಮಾಣ(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    mch_memmove(&prop_ಎಣಿಕೆ, text + textlen, PROP_ಎಣಿಕೆ_SIZE);
    ಯದಿ (!text_prop_ಎಣಿಕೆ_valid(prop_ಎಣಿಕೆ, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (!text_prop_vtext_valid(text + textlen + PROP_ಎಣಿಕೆ_SIZE,
					   (ಪೂರ್ಣಾಙ್ಕ)prop_ಎಣಿಕೆ, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    *props = text + textlen + PROP_ಎಣಿಕೆ_SIZE;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)prop_ಎಣಿಕೆ;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    ಪೂರ್ಣಾಙ್ಕ
prop_ಎಣಿಕೆ_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = get_text_props(buf, lnum, &props, FALSE);
    ಪೂರ್ಣಾಙ್ಕ		result = 0;
    textprop_T	prop;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		next_ಬಲ_goes_below = FALSE;

    ಯದಿ (ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
    {
	mch_memmove(&prop, props + i * ಪರಿಮಾಣ(prop), ಪರಿಮಾಣ(prop));
	ಯದಿ (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    ಯದಿ ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_ಬಲ_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_ಬಲ)))
	    {
		++result;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (prop.tp_flags & TP_FLAG_ALIGN_ಬಲ)
		next_ಬಲ_goes_below = TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    ಪೂರ್ಣಾಙ್ಕ
ಎಣಿಕೆ_props(linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ only_starting, ಪೂರ್ಣಾಙ್ಕ last_ಸಾಲು)
{
    char_u	*props;
    ಪೂರ್ಣಾಙ್ಕ		proplen = get_text_props(curbuf, lnum, &props, 0);
    ಪೂರ್ಣಾಙ್ಕ		result = proplen;
    textprop_T	prop;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * ಪರಿಮಾಣ(prop), ಪರಿಮಾಣ(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	ಯದಿ ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_ಸಾಲು && prop.tp_col == MAXCOL))
	    --result;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ textprop_T	*text_prop_compare_props;
ಸ್ಥಿರ buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
text_prop_order(ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಯದಿ (flags & TP_FLAG_ALIGN_ABOVE)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (flags & TP_FLAG_ALIGN_ಬಲ)
	ಪ್ರತ್ಯಾಗಮ 2;
    ಯದಿ (flags & TP_FLAG_ALIGN_BELOW)
	ಪ್ರತ್ಯಾಗಮ 3;
    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
text_prop_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    ಪೂರ್ಣಾಙ್ಕ  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(ಪೂರ್ಣಾಙ್ಕ *)s1;
    idx2 = *(ಪೂರ್ಣಾಙ್ಕ *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    ಯದಿ ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	ಪ್ರತ್ಯಾಗಮ tp1->tp_id < 0 ? 1 : -1;

    ಯದಿ (col1 == MAXCOL || col2 == MAXCOL)
    {
	ಪೂರ್ಣಾಙ್ಕ order1 = text_prop_order(tp1->tp_flags);
	ಪೂರ್ಣಾಙ್ಕ order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	ಯದಿ (order1 != order2)
	    ಪ್ರತ್ಯಾಗಮ order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    ಯದಿ (pt1 != pt2)
    {
	ಯದಿ (pt1 == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (pt2 == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ 1;
	ಯದಿ (pt1->pt_priority != pt2->pt_priority)
	    ಪ್ರತ್ಯಾಗಮ pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    ಯದಿ (col1 != col2)
	ಪ್ರತ್ಯಾಗಮ col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    ಯದಿ (tp1->tp_id < 0)
	ಪ್ರತ್ಯಾಗಮ tp1->tp_id > tp2->tp_id ? 1 : -1;

    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    ಶೂನ್ಯ
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	ಪೂರ್ಣಾಙ್ಕ	    *idxs,
	ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((ಶೂನ್ಯ *)idxs, (size_t)ಎಣಿಕೆ, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_ಸಾಲುಗಳು(
	win_T	    *wp,
	ಪೂರ್ಣಾಙ್ಕ	    type_id,
	ಪೂರ್ಣಾಙ್ಕ	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    ಯದಿ (text_prop_type_by_id(wp->w_buffer, type_id) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ false;

    ಯದಿ (first_lnum < 1)
	first_lnum = 1;
    ಯದಿ (last_lnum > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	last_lnum = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಪ್ರತಿ (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	ಪೂರ್ಣಾಙ್ಕ	ಎಣಿಕೆ = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ; ++i)
	{
	    mch_memmove(prop, props + i * ಪರಿಮಾಣ(textprop_T),
							   ಪರಿಮಾಣ(textprop_T));
	    ಯದಿ (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		ಪ್ರತ್ಯಾಗಮ true;
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	ಪೂರ್ಣಾಙ್ಕ	    type_id,
	ಪೂರ್ಣಾಙ್ಕ	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    ಪ್ರತ್ಯಾಗಮ find_prop_in_ಸಾಲುಗಳು(wp, type_id, id, prop, found_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_text_props(linenr_T lnum, textprop_T *tps, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    char_u  *text;
    char_u  *newtext;
    ಪೂರ್ಣಾಙ್ಕ	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    ಯದಿ (ಎಣಿಕೆ == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	ಯದಿ (newtext == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	mch_memmove(newtext, text, textlen);
	ಯದಿ (curbuf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ಸಾಲು_ptr);
	curbuf->b_ml.ml_ಸಾಲು_ptr = newtext;
	curbuf->b_ml.ml_ಸಾಲು_len = textlen;
	curbuf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;
    }
    ಅನ್ಯಥಾ
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_ಎಣಿಕೆ = (uint16_t)ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ	    vtext_size = 0;
	ಪೂರ್ಣಾಙ್ಕ	    total_len;
	char_u	    *ಎಣಿಕೆ_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	ಪೂರ್ಣಾಙ್ಕ	    i;

	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
	    ಯದಿ (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (ಪೂರ್ಣಾಙ್ಕ)PROP_ಎಣಿಕೆ_SIZE
		    + ಎಣಿಕೆ * (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	ಯದಿ (newtext == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	mch_memmove(newtext, text, textlen);

	ಎಣಿಕೆ_dest = newtext + textlen;
	mch_memmove(ಎಣಿಕೆ_dest, &prop_ಎಣಿಕೆ, PROP_ಎಣಿಕೆ_SIZE);

	prop_dest = ಎಣಿಕೆ_dest + PROP_ಎಣಿಕೆ_SIZE;
	vtext_dest = prop_dest + ಎಣಿಕೆ * ಪರಿಮಾಣ(textprop_T);

	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
	{
	    textprop_T prop = tps[i];

	    ಯದಿ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		ಪೂರ್ಣಾಙ್ಕ copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = ರಿಕ್ತ;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - ಎಣಿಕೆ_dest);
		vtext_dest += copysize;
	    }
	    ಅನ್ಯಥಾ
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, ಪರಿಮಾಣ(textprop_T));
	    prop_dest += ಪರಿಮಾಣ(textprop_T);
	}

	ಯದಿ (curbuf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ಸಾಲು_ptr);
	curbuf->b_ml.ml_ಸಾಲು_ptr = newtext;
	curbuf->b_ml.ml_ಸಾಲು_len = total_len;
	curbuf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;
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
props_add_ಎಣಿಕೆ_header(char_u *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_len, ಪೂರ್ಣಾಙ್ಕ textlen, ಪೂರ್ಣಾಙ್ಕ *new_len)
{
    ಪೂರ್ಣಾಙ್ಕ		prop_bytes = ಸಾಲು_len - textlen;
    uint16_t	prop_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		vtext_size = 0;
    ಪೂರ್ಣಾಙ್ಕ		total;
    char_u	*newline;
    char_u	*ಎಣಿಕೆ_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (prop_bytes <= 0 || prop_bytes % ಪರಿಮಾಣ(textprop_T) != 0)
    {
	*new_len = ಸಾಲು_len;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    prop_ಎಣಿಕೆ = (uint16_t)(prop_bytes / ಪರಿಮಾಣ(textprop_T));

    // Calculate total vtext size.
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ಸಾಲು + textlen + i * ಪರಿಮಾಣ(textprop_T),
							ಪರಿಮಾಣ(textprop_T));
	ಯದಿ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (ಪೂರ್ಣಾಙ್ಕ)PROP_ಎಣಿಕೆ_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    ಯದಿ (newline == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    *new_len = total;

    mch_memmove(newline, ಸಾಲು, textlen);
    ಎಣಿಕೆ_dest = newline + textlen;
    mch_memmove(ಎಣಿಕೆ_dest, &prop_ಎಣಿಕೆ, PROP_ಎಣಿಕೆ_SIZE);
    prop_dest = ಎಣಿಕೆ_dest + PROP_ಎಣಿಕೆ_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ಸಾಲು + textlen + i * ಪರಿಮಾಣ(textprop_T),
							ಪರಿಮಾಣ(textprop_T));
	ಯದಿ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    ಪೂರ್ಣಾಙ್ಕ copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - ಎಣಿಕೆ_dest);
	    vtext_dest += copysize;
	}
	ಅನ್ಯಥಾ
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * ಪರಿಮಾಣ(textprop_T), &prop,
							ಪರಿಮಾಣ(textprop_T));
    }
    ಪ್ರತ್ಯಾಗಮ newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    ಶೂನ್ಯ
add_text_props(linenr_T lnum, textprop_T *text_props, ಪೂರ್ಣಾಙ್ಕ text_prop_ಎಣಿಕೆ)
{
    unpacked_memline_T	um;
    ಪೂರ್ಣಾಙ್ಕ			i;

    um = um_open_at(curbuf, lnum, text_prop_ಎಣಿಕೆ);
    ಯದಿ (um.buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!um_detach(&um))
    {
	um_abort(&um);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Grow props array if needed.
    ಯದಿ (um.prop_ಎಣಿಕೆ + text_prop_ಎಣಿಕೆ > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_ಎಣಿಕೆ + text_prop_ಎಣಿಕೆ) * ಪರಿಮಾಣ(textprop_T));
	ಯದಿ (newprops == ರಿಕ್ತ)
	{
	    um_abort(&um);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	um.props = newprops;
	um.prop_size = um.prop_ಎಣಿಕೆ + text_prop_ಎಣಿಕೆ;
    }

    ಪ್ರತಿ (i = 0; i < text_prop_ಎಣಿಕೆ; ++i)
	um.props[um.prop_ಎಣಿಕೆ++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compare_pt(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    ಪ್ರತ್ಯಾಗಮ tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    ಸ್ಥಿರ proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, ಪೂರ್ಣಾಙ್ಕ id)
{
    ಪೂರ್ಣಾಙ್ಕ ಕೆಳಗಿನ = 0;
    ಪೂರ್ಣಾಙ್ಕ ಮೇಲಿನ;

    ಯದಿ (ht == ರಿಕ್ತ || ht->ht_used == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    ಯದಿ (*array == ರಿಕ್ತ)
    {
	long	    todo;
	hashitem_T  *hi;
	ಪೂರ್ಣಾಙ್ಕ	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	ಯದಿ (*array == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
	{
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((ಶೂನ್ಯ *)*array, ht->ht_used, ಪರಿಮಾಣ(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    ಮೇಲಿನ = ht->ht_used;
    ಯಾವತ್ (ಮೇಲಿನ > ಕೆಳಗಿನ)
    {
	ಪೂರ್ಣಾಙ್ಕ m = (ಮೇಲಿನ + ಕೆಳಗಿನ) / 2;

	ಯದಿ ((*array)[m]->pt_id == id)
	    ಪ್ರತ್ಯಾಗಮ (*array)[m];
	ಯದಿ ((*array)[m]->pt_id > id)
	    ಮೇಲಿನ = m;
	ಅನ್ಯಥಾ
	    ಕೆಳಗಿನ = m + 1;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    ಪೂರ್ಣಾಙ್ಕ buflocal = TRUE;
    ಪೂರ್ಣಾಙ್ಕ virtualtext_prop = prop->tp_id < 0;

    dict_add_ಸಂಖ್ಯೆ(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    ಯದಿ (!virtualtext_prop)
    {
	dict_add_ಸಂಖ್ಯೆ(dict, "length", prop->tp_len);
	dict_add_ಸಂಖ್ಯೆ(dict, "id", prop->tp_id);
    }
    dict_add_ಸಂಖ್ಯೆ(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_ಸಂಖ್ಯೆ(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    ಯದಿ (pt == ರಿಕ್ತ)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    ಯದಿ (pt != ರಿಕ್ತ)
	dict_add_string(dict, "type", pt->pt_name);

    ಯದಿ (buflocal)
	dict_add_ಸಂಖ್ಯೆ(dict, "type_bufnr", buf->b_fnum);
    ಅನ್ಯಥಾ
	dict_add_ಸಂಖ್ಯೆ(dict, "type_bufnr", 0);
    ಯದಿ (virtualtext_prop)
    {
	string_T    text_align = {ರಿಕ್ತ, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	ಯದಿ (prop->tp_flags & TP_FLAG_ALIGN_ಬಲ)
	    STR_LITERAL_SET(text_align, "right");
	ಅನ್ಯಥಾ ಯದಿ (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	ಅನ್ಯಥಾ ಯದಿ (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	ಯದಿ (text_align.string != ರಿಕ್ತ)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (ಪೂರ್ಣಾಙ್ಕ)text_align.length);

	// text_wrap
	ಯದಿ (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	ಯದಿ (prop->tp_padleft != 0)
	    dict_add_ಸಂಖ್ಯೆ(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    ಯದಿ (type == ರಿಕ್ತ)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    ಪ್ರತ್ಯಾಗಮ type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    ಪ್ರತ್ಯಾಗಮ text_prop_type_by_id(buf, prop->tp_type) != ರಿಕ್ತ;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    ಶೂನ್ಯ
f_prop_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    linenr_T start;
    linenr_T end;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    ಪೂರ್ಣಾಙ್ಕ	    did_clear = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    start = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    end = start;
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ಯದಿ (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಯದಿ (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	ಪ್ರತ್ಯಾಗಮ;
    }

    {
	unpacked_memline_T um = um_open(buf);

	ಪ್ರತಿ (lnum = start; lnum <= end; ++lnum)
	{
	    ಪೂರ್ಣಾಙ್ಕ idx;

	    ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಭಙ್ಗ;
	    ಯದಿ (!um_goto_ಸಾಲು(&um, lnum, 0))
		ಭಙ್ಗ;
	    ಯದಿ (um.prop_ಎಣಿಕೆ > 0)
	    {
		did_clear = TRUE;
		ಪ್ರತಿ (idx = um.prop_ಎಣಿಕೆ - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    ಯದಿ (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    ಶೂನ್ಯ
f_prop_find(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    ಪೂರ್ಣಾಙ್ಕ		lnum_start;
    ಪೂರ್ಣಾಙ್ಕ		start_pos_has_prop = 0;
    ಪೂರ್ಣಾಙ್ಕ		seen_end = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		id = 0;
    ಪೂರ್ಣಾಙ್ಕ		id_found = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		type_id = -1;
    ಪೂರ್ಣಾಙ್ಕ		skipstart = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		lnum = -1;
    ಪೂರ್ಣಾಙ್ಕ		col = -1;
    ಪೂರ್ಣಾಙ್ಕ		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    ಪೂರ್ಣಾಙ್ಕ		both;

    ಯದಿ (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    dict = argvars[0].vval.v_dict;

    ಯದಿ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	ಯದಿ (*dir_s == 'b')
	    dir = BACKWARD;
	ಅನ್ಯಥಾ ಯದಿ (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    ಯದಿ (di != ರಿಕ್ತ)
	lnum = tv_get_ಸಂಖ್ಯೆ(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    ಯದಿ (di != ರಿಕ್ತ)
	col = tv_get_ಸಂಖ್ಯೆ(&di->di_tv);

    ಯದಿ (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (col == -1)
	col = 1;

    ಯದಿ (lnum < 1 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	emsg(_(e_invalid_range));
	ಪ್ರತ್ಯಾಗಮ;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    ಯದಿ (dict_has_ಕೀಲಿ(dict, "id"))
    {
	id = dict_get_ಸಂಖ್ಯೆ(dict, "id");
	id_found = TRUE;
    }
    ಯದಿ (dict_has_ಕೀಲಿ(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ಯದಿ (type == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    ಯದಿ (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ಪ್ರತ್ಯಾಗಮ;
    }

    lnum_start = lnum;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯಾವತ್ (1)
    {
	char_u	*prop_start_ptr;
	ಪೂರ್ಣಾಙ್ಕ	ಎಣಿಕೆ = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	ಪೂರ್ಣಾಙ್ಕ	    i;
	textprop_T  prop;
	ಪೂರ್ಣಾಙ್ಕ	    prop_start;
	ಪೂರ್ಣಾಙ್ಕ	    prop_end;

	ಪ್ರತಿ (i = dir == BACKWARD ? ಎಣಿಕೆ - 1 : 0; i >= 0 && i < ಎಣಿಕೆ; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * ಪರಿಮಾಣ(textprop_T),
							   ಪರಿಮಾಣ(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    ಯದಿ (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_ಎಣಿಕೆ_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    ಅನ್ಯಥಾ
		prop.u.tp_text = ರಿಕ್ತ;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    ಯದಿ (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		ಯದಿ (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    ಯದಿ (!is_floating_vtext && prop.tp_col > col)
			ಅನುವರ್ತ;
		}
		ಅನ್ಯಥಾ ಯದಿ (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    ಅನುವರ್ತ;
	    }
	    ಯದಿ (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		ಯದಿ (lnum_start == lnum
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
		ಯದಿ (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		ಯದಿ (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    ಯದಿ (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    ಅನುವರ್ತ;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		ಯದಿ (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    ಅನುವರ್ತ;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "lnum", lnum);

		ಪ್ರತ್ಯಾಗಮ;
	    }
	}

	ಯದಿ (dir > 0)
	{
	    ಯದಿ (lnum >= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಭಙ್ಗ;
	    lnum++;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (lnum <= 1)
		ಭಙ್ಗ;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prop_type_or_id_in_list(ಪೂರ್ಣಾಙ್ಕ *types_or_ids, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ type_or_id)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < len; i++)
	ಯದಿ (types_or_ids[i] == type_or_id)
	    ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
get_props_in_ಸಾಲು(
	buf_T		*buf,
	linenr_T	lnum,
	ಪೂರ್ಣಾಙ್ಕ		*prop_types,
	ಪೂರ್ಣಾಙ್ಕ		prop_types_len,
	ಪೂರ್ಣಾಙ್ಕ		*prop_ids,
	ಪೂರ್ಣಾಙ್ಕ		prop_ids_len,
	list_T		*retlist,
	ಪೂರ್ಣಾಙ್ಕ		add_lnum)
{
    char_u	*props;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		i;
    textprop_T	prop;

    ಎಣಿಕೆ = get_text_props(buf, lnum, &props, FALSE);
    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
    {
	mch_memmove(&prop, props + i * ಪರಿಮಾಣ(textprop_T),
		ಪರಿಮಾಣ(textprop_T));
	// Convert offset to pointer for virtual text props.
	ಯದಿ (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_ಎಣಿಕೆ_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ಅನ್ಯಥಾ
	    prop.u.tp_text = ರಿಕ್ತ;
	ಯದಿ ((prop_types == ರಿಕ್ತ
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == ರಿಕ್ತ
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    ಯದಿ (d == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    prop_fill_dict(d, &prop, buf);
	    ಯದಿ (add_lnum)
		dict_add_ಸಂಖ್ಯೆ(d, "lnum", lnum);
	    ಯದಿ (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *
get_prop_types_from_names(list_T *l, buf_T *buf, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_types)
{
    ಪೂರ್ಣಾಙ್ಕ		*prop_types;
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*name;
    proptype_T	*type;

    *ಅಂಕೆ_types = 0;

    prop_types = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, list_len(l));
    ಯದಿ (prop_types == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    i = 0;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಯದಿ (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    ಗಚ್ಛ errret;
	}
	name = li->li_tv.vval.v_string;
	ಯದಿ (name == ರಿಕ್ತ)
	    ಗಚ್ಛ errret;

	type = lookup_prop_type(name, buf);
	ಯದಿ (type == ರಿಕ್ತ)
	    ಗಚ್ಛ errret;
	prop_types[i++] = type->pt_id;
    }

    *ಅಂಕೆ_types = i;
    ಪ್ರತ್ಯಾಗಮ prop_types;

errret:
    VIM_CLEAR(prop_types);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *
get_prop_ids_from_list(list_T *l, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_ids)
{
    ಪೂರ್ಣಾಙ್ಕ		*prop_ids;
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;
    ಪೂರ್ಣಾಙ್ಕ		id;
    ಪೂರ್ಣಾಙ್ಕ		error;

    *ಅಂಕೆ_ids = 0;

    prop_ids = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, list_len(l));
    ಯದಿ (prop_ids == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	error = FALSE;
	id = tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, &error);
	ಯದಿ (error)
	    ಗಚ್ಛ errret;

	prop_ids[i++] = id;
    }

    *ಅಂಕೆ_ids = i;
    ಪ್ರತ್ಯಾಗಮ prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    ಶೂನ್ಯ
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    buf_T	*buf = curbuf;
    ಪೂರ್ಣಾಙ್ಕ		add_lnum = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		*prop_types = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		prop_types_len = 0;
    ಪೂರ್ಣಾಙ್ಕ		*prop_ids = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // default: get text properties on current line
    start_lnum = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    end_lnum = start_lnum;
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ಯದಿ (check_for_dict_arg(argvars, 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	d = argvars[1].vval.v_dict;

	ಯದಿ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (d != ರಿಕ್ತ && (di = dict_find(d, (char_u *)"end_lnum", -1)) != ರಿಕ್ತ)
	{
	    ಯದಿ (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ)
	    {
		emsg(_(e_ಸಂಖ್ಯೆ_required));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    end_lnum = tv_get_ಸಂಖ್ಯೆ(&di->di_tv);
	    ಯದಿ (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//ಸಾಲು
		end_lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + end_lnum + 1;
	    ಅನ್ಯಥಾ ಯದಿ (end_lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		end_lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    add_lnum = TRUE;
	}
	ಯದಿ (d != ರಿಕ್ತ && (di = dict_find(d, (char_u *)"types", -1)) != ರಿಕ್ತ)
	{
	    ಯದಿ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ಪ್ರತ್ಯಾಗಮ;
	    }

	    l = di->di_tv.vval.v_list;
	    ಯದಿ (l != ರಿಕ್ತ && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		ಯದಿ (prop_types == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ;
	    }
	}
	ಯದಿ (d != ರಿಕ್ತ && (di = dict_find(d, (char_u *)"ids", -1)) != ರಿಕ್ತ)
	{
	    ಯದಿ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ಗಚ್ಛ errret;
	    }

	    l = di->di_tv.vval.v_list;
	    ಯದಿ (l != ರಿಕ್ತ && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		ಯದಿ (prop_ids == ರಿಕ್ತ)
		    ಗಚ್ಛ errret;
	    }
	}
    }
    ಯದಿ (start_lnum < 1 || start_lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    ಅನ್ಯಥಾ
	ಪ್ರತಿ (lnum = start_lnum; lnum <= end_lnum; lnum++)
	    get_props_in_ಸಾಲು(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, add_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    ಶೂನ್ಯ
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start = 1;
    linenr_T	end = 0;
    linenr_T	lnum;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    ಪೂರ್ಣಾಙ್ಕ		do_all;
    ಪೂರ್ಣಾಙ್ಕ		id = -MAXCOL;
    ಪೂರ್ಣಾಙ್ಕ		type_id = -1;	    // for a single "type"
    ಪೂರ್ಣಾಙ್ಕ		*type_ids = ರಿಕ್ತ;   // array, for a list of "types", allocated
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_type_ids = 0;   // number of elements in "type_ids"
    ಪೂರ್ಣಾಙ್ಕ		both;

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    ಯದಿ (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	end = start;
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
	ಯದಿ (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    dict = argvars[0].vval.v_dict;
    ಯದಿ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    do_all = dict_get_bool(dict, "all", FALSE);

    ಯದಿ (dict_has_ಕೀಲಿ(dict, "id"))
	id = dict_get_ಸಂಖ್ಯೆ(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    ಯದಿ (dict_has_ಕೀಲಿ(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ಯದಿ (type == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	type_id = type->pt_id;
    }
    ಯದಿ (dict_has_ಕೀಲಿ(dict, "types"))
    {
	typval_T types;
	listitem_T *li = ರಿಕ್ತ;

	dict_get_tv(dict, "types", &types);
	ಯದಿ (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_ವಸ್ತುಗಳು(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		ಯದಿ (li->li_tv.v_type != VAR_STRING)
		    ಅನುವರ್ತ;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		ಯದಿ (!prop_type)
		    ಗಚ್ಛ cleanup_prop_remove;

		type_ids[ಅಂಕೆ_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    ಯದಿ (id == -MAXCOL && (type_id == -1 && ಅಂಕೆ_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	ಗಚ್ಛ cleanup_prop_remove;
    }
    ಯದಿ (both && (id == -MAXCOL || (type_id == -1 && ಅಂಕೆ_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ಗಚ್ಛ cleanup_prop_remove;
    }
    ಯದಿ (type_id != -1 && ಅಂಕೆ_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	ಗಚ್ಛ cleanup_prop_remove;
    }

    ಯದಿ (end == 0)
	end = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    {
	unpacked_memline_T um = um_open(buf);

	ಪ್ರತಿ (lnum = start; lnum <= end; ++lnum)
	{
	    ಪೂರ್ಣಾಙ್ಕ idx;

	    ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಭಙ್ಗ;
	    ಯದಿ (!um_goto_ಸಾಲು(&um, lnum, 0))
		ಭಙ್ಗ;

	    ಪ್ರತಿ (idx = 0; idx < um.prop_ಎಣಿಕೆ; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		ಪೂರ್ಣಾಙ್ಕ	    matches_id = 0;
		ಪೂರ್ಣಾಙ್ಕ	    matches_type = 0;

		ಯದಿ (prop->tp_flags & TP_FLAG_DELETED)
		    ಅನುವರ್ತ;

		matches_id = prop->tp_id == id;
		ಯದಿ (ಅಂಕೆ_type_ids > 0)
		{
		    ಪೂರ್ಣಾಙ್ಕ idx2;

		    ಪ್ರತಿ (idx2 = 0; !matches_type && idx2 < ಅಂಕೆ_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		ಅನ್ಯಥಾ
		    matches_type = prop->tp_type == type_id;

		ಯದಿ (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    ಯದಿ (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_ಸಂಖ್ಯೆ;
		    ಯದಿ (!do_all)
			ಭಙ್ಗ;
		}
	    }
	}
	um_close(&um);
    }

    ಯದಿ (first_changed > 0)
    {
	changed_ಸಾಲು_display_buf(buf);
	changed_ಸಾಲುಗಳು_buf(buf, first_changed, last_changed + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prop_type_set(typval_T *argvars, ಪೂರ್ಣಾಙ್ಕ add)
{
    char_u	*name;
    buf_T	*buf = ರಿಕ್ತ;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);
    ಯದಿ (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    ಯದಿ (add)
    {
	hashtab_T **htp;

	ಯದಿ (prop != ರಿಕ್ತ)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	ಯದಿ (prop == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	ಯದಿ (buf == ರಿಕ್ತ)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	ಅನ್ಯಥಾ
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	ಯದಿ (*htp == ರಿಕ್ತ)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    ಯದಿ (*htp == ರಿಕ್ತ)
	    {
		vim_free(prop);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (prop == ರಿಕ್ತ)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    ಯದಿ (dict != ರಿಕ್ತ)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    char_u	*highlight;
	    ಪೂರ್ಣಾಙ್ಕ		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    ಯದಿ (highlight != ರಿಕ್ತ && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    ಯದಿ (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == ರಿಕ್ತ ? (char_u *)"" : highlight);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    ಯದಿ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    ಅನ್ಯಥಾ
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    ಯದಿ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    ಅನ್ಯಥಾ
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	    prop->pt_priority = tv_get_ಸಂಖ್ಯೆ(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    ಯದಿ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    ಅನ್ಯಥಾ
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    ಯದಿ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    ಅನ್ಯಥಾ
		prop->pt_flags &= ~PT_FLAG_INS_END_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    ಶೂನ್ಯ
f_prop_type_add(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    ಶೂನ್ಯ
f_prop_type_change(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    ಶೂನ್ಯ
f_prop_type_delete(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*name;
    buf_T	*buf = ರಿಕ್ತ;
    hashitem_T	*hi;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);
    ಯದಿ (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    hi = find_prop_type_hi(name, buf);
    ಯದಿ (hi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    ಯದಿ (buf == ರಿಕ್ತ)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    ಅನ್ಯಥಾ
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    changed_window_setting_buf(buf == ರಿಕ್ತ ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    ಶೂನ್ಯ
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);
    ಯದಿ (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    proptype_T  *prop = ರಿಕ್ತ;
    buf_T	    *buf = ರಿಕ್ತ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    prop = find_prop_type(name, buf);
    ಯದಿ (prop == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    dict_T *d = rettv->vval.v_dict;

    ಯದಿ (prop->pt_hl_id > 0)
	dict_add_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_add_ಸಂಖ್ಯೆ(d, "priority", prop->pt_priority);
    dict_add_ಸಂಖ್ಯೆ(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_add_ಸಂಖ್ಯೆ(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_add_ಸಂಖ್ಯೆ(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_START_INCL) ? 1 : 0);
    dict_add_ಸಂಖ್ಯೆ(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_END_INCL) ? 1 : 0);
    ಯದಿ (buf != ರಿಕ್ತ)
	dict_add_ಸಂಖ್ಯೆ(d, "bufnr", buf->b_fnum);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_types(hashtab_T *ht, list_T *l)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
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
    ಶೂನ್ಯ
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ರಿಕ್ತ;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	ಯದಿ (global_proptypes != ರಿಕ್ತ)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    ಅನ್ಯಥಾ ಯದಿ (buf->b_proptypes != ರಿಕ್ತ)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_ht_prop_types(hashtab_T *ht)
{
    long	todo;
    hashitem_T	*hi;

    ಯದಿ (ht == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#ಯದಿ defined(EXITFREE)
/*
 * Free all global property types.
 */
    ಶೂನ್ಯ
clear_global_prop_types(ಶೂನ್ಯ)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = ರಿಕ್ತ;
    VIM_CLEAR(global_proparray);
}
#endif

/*
 * Free all property types for "buf".
 */
    ಶೂನ್ಯ
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = ರಿಕ್ತ;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ dirty;	    // if the property was changed
    ಪೂರ್ಣಾಙ್ಕ can_drop;   // whether after this change, the prop may be removed
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
    ಸ್ಥಿರ adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    col,
	ಪೂರ್ಣಾಙ್ಕ	    added,
	ಪೂರ್ಣಾಙ್ಕ	    flags)
{
    proptype_T	*pt;
    ಪೂರ್ಣಾಙ್ಕ		start_incl;
    ಪೂರ್ಣಾಙ್ಕ		end_incl;
    ಪೂರ್ಣಾಙ್ಕ		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    ಯದಿ (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	ಪ್ರತ್ಯಾಗಮ res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != ರಿಕ್ತ && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    ಯದಿ (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != ರಿಕ್ತ && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    ಯದಿ (added > 0)
    {
	ಯದಿ (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	ಅನ್ಯಥಾ ಯದಿ (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    ಅನ್ಯಥಾ ಯದಿ (prop->tp_col > col + 1)
    {
	ಯದಿ (prop->tp_col + added < col + 1)
	{
	    ಯದಿ (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    ಅನ್ಯಥಾ
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		ಯದಿ (prop->tp_len <= 0)
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
	ಅನ್ಯಥಾ
	    prop->tp_col += added;
    }
    ಅನ್ಯಥಾ ಯದಿ (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	ಯದಿ (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	ಯದಿ (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    ಅನ್ಯಥಾ
	res.dirty = FALSE;

    ಪ್ರತ್ಯಾಗಮ res;
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
    ಪೂರ್ಣಾಙ್ಕ
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	ಪೂರ್ಣಾಙ್ಕ	    bytes_added,
	ಪೂರ್ಣಾಙ್ಕ	    flags)
{
    unpacked_memline_T um;
    ಪೂರ್ಣಾಙ್ಕ		dirty = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		ri;

    ಯದಿ (text_prop_frozen > 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    um = um_open_at(curbuf, lnum, 0);
    ಯದಿ (um.buf == ರಿಕ್ತ || um.prop_ಎಣಿಕೆ == 0)
    {
	um_abort(&um);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಪ್ರತಿ (ri = 0; ri < um.prop_ಎಣಿಕೆ; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	ಯದಿ (res.dirty)
	{
	    ಯದಿ (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		ಯದಿ (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		ಯದಿ ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		}
	    }
	    dirty = TRUE;
	}
	ಯದಿ (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    ಅನುವರ್ತ;
	}
    }
    ಯದಿ (dirty)
	um_close(&um);
    ಅನ್ಯಥಾ
	um_abort(&um);
    ಪ್ರತ್ಯಾಗಮ dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    ಶೂನ್ಯ
adjust_props_for_split(
	linenr_T    lnum_props,
	linenr_T    lnum_ತುದಿ,
	ಪೂರ್ಣಾಙ್ಕ	    kept,
	ಪೂರ್ಣಾಙ್ಕ	    deleted,
	ಪೂರ್ಣಾಙ್ಕ	    at_eol)
{
    char_u	*props;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    garray_T    prevprop;
    garray_T    nextprop;
    ಪೂರ್ಣಾಙ್ಕ		skipped = kept + deleted;

    ಯದಿ (!curbuf->b_has_textprop)
	ಪ್ರತ್ಯಾಗಮ;

    // Get the text properties from "lnum_props".
    ಎಣಿಕೆ = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, ಪರಿಮಾಣ(textprop_T), 10);
    ga_init2(&nextprop, ಪರಿಮಾಣ(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *ಎಣಿಕೆ_ptr = props - PROP_ಎಣಿಕೆ_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	ಪೂರ್ಣಾಙ್ಕ	    start_incl, end_incl;
	ಪೂರ್ಣಾಙ್ಕ	    cont_prev, cont_next;
	ಪೂರ್ಣಾಙ್ಕ	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * ಪರಿಮಾಣ(textprop_T), ಪರಿಮಾಣ(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	ಯದಿ (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(ಎಣಿಕೆ_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ಅನ್ಯಥಾ
	    prop.u.tp_text = ರಿಕ್ತ;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != ರಿಕ್ತ && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != ರಿಕ್ತ && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	ಯದಿ (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	ಅನ್ಯಥಾ
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    ಪೂರ್ಣಾಙ್ಕ is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    ಯದಿ (is_floating_vtext)
		cont_next = FALSE;
	    ಅನ್ಯಥಾ
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	ಯದಿ (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	ಯದಿ (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    ಪೂರ್ಣಾಙ್ಕ is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    ಯದಿ (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    ಯದಿ (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	ಯದಿ (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    ಪೂರ್ಣಾಙ್ಕ is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    ಯದಿ (p->tp_col != MAXCOL)
	    {
		ಯದಿ (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    ಯದಿ (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    set_text_props(lnum_ತುದಿ, (textprop_T *)prevprop.ga_data,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    set_text_props(lnum_ತುದಿ + 1, (textprop_T *)nextprop.ga_data,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    ಶೂನ್ಯ
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	ಪೂರ್ಣಾಙ್ಕ		    last_ಸಾಲು,
	long		    col,
	ಪೂರ್ಣಾಙ್ಕ		    removed)
{
    unpacked_memline_T	r_um;

    ಯದಿ (um->buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    r_um = um_open_at(um->buf, lnum, 0);
    ಯದಿ (r_um.buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!um_detach(&r_um))
    {
	um_abort(&r_um);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = r_um.prop_ಎಣಿಕೆ - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	ಪೂರ್ಣಾಙ್ಕ	    end;

	ಯದಿ (prop->tp_col == MAXCOL && !last_ಸಾಲು)
	    ಅನುವರ್ತ;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	ಯದಿ (last_ಸಾಲು || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = ರಿಕ್ತ;  // ownership transferred
	}
	ಅನ್ಯಥಾ
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < um->prop_ಎಣಿಕೆ; ++j)
	    {
		textprop_T *op = &um->props[j];

		ಯದಿ ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    ಭಙ್ಗ;
		}
	    }
	    ಯದಿ (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remember_cleared_prop(garray_T *gap, ಪೂರ್ಣಾಙ್ಕ which, textprop_T *prop, ಪೂರ್ಣಾಙ್ಕ flag)
{
    undoprop_T	*up;

    ಯದಿ (which == UNDOPROP_NONE)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (gap->ga_itemsize == 0)
	ga_init2(gap, ಪರಿಮಾಣ(undoprop_T), 4);
    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
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
    ಶೂನ್ಯ
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	ಪೂರ್ಣಾಙ್ಕ	    above_which,
	ಪೂರ್ಣಾಙ್ಕ	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = ರಿಕ್ತ;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    ಯದಿ (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;

    above = um_open_at(buf, lnum, 0);
    ಯದಿ (above.buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < above.prop_ಎಣಿಕೆ; ++i)
	ಯದಿ (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    ಯದಿ (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	ಪ್ರತ್ಯಾಗಮ;
    }

    below = um_open_at(buf, lnum + 1, 0);
    ಯದಿ (below.buf == ರಿಕ್ತ)
    {
	um_abort(&above);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (below.prop_ಎಣಿಕೆ > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_ಎಣಿಕೆ);
	ಯದಿ (paired == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < above.prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	ಯದಿ (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    ಅನುವರ್ತ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < below.prop_ಎಣಿಕೆ; ++j)
	{
	    textprop_T *next = &below.props[j];

	    ಯದಿ (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < below.prop_ಎಣಿಕೆ; ++j)
    {
	ಯದಿ (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    ಅನುವರ್ತ;
	ಯದಿ (!below.detached && !um_detach(&below))
	    ಭಙ್ಗ;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    ಯದಿ (below_changed)
	um_close(&below);
    ಅನ್ಯಥಾ
	um_abort(&below);
    ಯದಿ (above_changed)
	um_close(&above);
    ಅನ್ಯಥಾ
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    ಶೂನ್ಯ
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	ಪೂರ್ಣಾಙ್ಕ	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    ಯದಿ (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;
    um = um_open_at(buf, lnum, 0);
    ಯದಿ (um.buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	ಯದಿ (up->up_which != which)
	    ಅನುವರ್ತ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < um.prop_ಎಣಿಕೆ; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    ಯದಿ (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		ಅನುವರ್ತ;
	    ಯದಿ (!um.detached && !um_detach(&um))
		ಗಚ್ಛ theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    ಭಙ್ಗ;
	}
    }

theend:
    ಯದಿ (changed)
	um_close(&um);
    ಅನ್ಯಥಾ
	um_abort(&um);
}

    bool
text_prop_ಎಣಿಕೆ_valid(ಪೂರ್ಣಾಙ್ಕ prop_ಎಣಿಕೆ, size_t propdata_len)
{
    ಯದಿ (propdata_len < PROP_ಎಣಿಕೆ_SIZE)
	ಪ್ರತ್ಯಾಗಮ false;
    ಪ್ರತ್ಯಾಗಮ (size_t)prop_ಎಣಿಕೆ * ಪರಿಮಾಣ(textprop_T)
		    <= propdata_len - PROP_ಎಣಿಕೆ_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, ಪೂರ್ಣಾಙ್ಕ prop_ಎಣಿಕೆ, size_t propdata_len)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * ಪರಿಮಾಣ(textprop_T),
							  ಪರಿಮಾಣ(textprop_T));
	ಯದಿ (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    ಅನುವರ್ತ;
	ಯದಿ (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    ಪ್ರತ್ಯಾಗಮ false;
    }
    ಪ್ರತ್ಯಾಗಮ true;
}

#endif // FEAT_PROP_POPUP
