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

#ಆದರೆ defined(FEAT_PROP_POPUP)

ಸ್ಥಿರ ಶೂನ್ಯ um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
um_free_vtext(textprop_T *props, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; ++i)
	ಆದರೆ (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_ಪಠ್ಯ);
}

/*
 * Free memory used by an unpacked memline.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
um_free(unpacked_memline_T *um)
{
    ಆದರೆ (um->detached)
    {
	VIM_CLEAR(um->ಪಠ್ಯ);
	um_free_vtext(um->props, um->prop_ಎಣಿಕೆ);
    }
    VIM_CLEAR(um->props);
    um->prop_ಗಾತ್ರ = 0;
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

    CLEAR_ಕ್ಷೇತ್ರ(um);
    um.buf = buf;
    ಹಿಂತಿರುಗಿ um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_ಸಾಲು(unpacked_memline_T *um, linenr_T lnum, ಪೂರ್ಣಾಂಕ extra_props)
{
    char_u	*ಸಾಲು;
    size_t	textlen;
    size_t	propdata_len;
    ಪೂರ್ಣಾಂಕ		proplen;

    ಆದರೆ (um->buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ false;

    // Store changes to the current line if any.
    ಆದರೆ (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    ಆದರೆ (lnum == 0)
	ಹಿಂತಿರುಗಿ true;	// just unload

    ಸಾಲು = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_ಸಾಲು_len - textlen;

    um->lnum = lnum;
    um->ಪಠ್ಯ_ಗಾತ್ರ_2 = (colnr_T)textlen;
    um->ಪಠ್ಯ = ಸಾಲು;

    ಆದರೆ (propdata_len == 0)
	ಹಿಂತಿರುಗಿ true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    ಆದರೆ (propdata_len < PROP_ಎಣಿಕೆ_ಗಾತ್ರ + ಗಾತ್ರ(textprop_T))
    {
	emsg(e_ಪಠ್ಯ_property_info_corrupted);
	um->buf = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ false;
    }

    uint16_t    prop_ಎಣಿಕೆ;
    char_u	    *ಎಣಿಕೆ_ptr = ಸಾಲು + textlen;
    char_u	    *props_ಆರಂಭ;

    mch_memmove(&prop_ಎಣಿಕೆ, ಎಣಿಕೆ_ptr, PROP_ಎಣಿಕೆ_ಗಾತ್ರ);
    ಆದರೆ (!ಪಠ್ಯ_prop_ಎಣಿಕೆ_valid(prop_ಎಣಿಕೆ, propdata_len))
    {
	emsg(e_ಪಠ್ಯ_property_info_corrupted);
	um->buf = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ false;
    }
    proplen = (ಪೂರ್ಣಾಂಕ)prop_ಎಣಿಕೆ;
    props_ಆರಂಭ = ಎಣಿಕೆ_ptr + PROP_ಎಣಿಕೆ_ಗಾತ್ರ;

    ಆದರೆ (!ಪಠ್ಯ_prop_vtext_valid(props_ಆರಂಭ, proplen, propdata_len))
    {
	emsg(e_ಪಠ್ಯ_property_info_corrupted);
	um->buf = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    ಆದರೆ (um->props == ಖಾಲಿ)
    {
	um->buf = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ false;
    }
    um->prop_ಗಾತ್ರ = proplen + extra_props;
    um->prop_ಎಣಿಕೆ = proplen;
    mch_memmove(um->props, props_ಆರಂಭ, proplen * ಗಾತ್ರ(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < proplen; ++i)
    {
	ಆದರೆ (um->props[i].tp_id < 0 && um->props[i].u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ > 0)
	{
	    um->props[i].u.tp_ಪಠ್ಯ = ಎಣಿಕೆ_ptr + um->props[i].u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    um->props[i].u.tp_ಪಠ್ಯ = ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, ಪೂರ್ಣಾಂಕ extra_props)
{
    unpacked_memline_T um = um_open(buf);

    ಆದರೆ (!um_goto_ಸಾಲು(&um, lnum, extra_props))
	um.buf = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ um;
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

    ಆದರೆ (um->detached || um->buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ um->detached;

    newtext = vim_strnsave(um->ಪಠ್ಯ, um->ಪಠ್ಯ_ಗಾತ್ರ_2 - 1);
    ಆದರೆ (newtext == ಖಾಲಿ)
    {
	um->buf = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ false;
    }
    um->ಪಠ್ಯ = newtext;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < um->prop_ಎಣಿಕೆ; ++i)
    {
	ಆದರೆ (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_ಪಠ್ಯ);

	    ಆದರೆ (copy == ಖಾಲಿ)
	    {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < i; ++j)
		    ಆದರೆ (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_ಪಠ್ಯ);
		VIM_CLEAR(um->ಪಠ್ಯ);
		um->buf = ಖಾಲಿ;
		ಹಿಂತಿರುಗಿ false;
	    }
	    um->props[i].u.tp_ಪಠ್ಯ = copy;
	}
    }

    um->detached = true;
    ಹಿಂತಿರುಗಿ true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_ಗಣ_ಪಠ್ಯ(unpacked_memline_T *um, char_u *ಪಠ್ಯ)
{
    ಆದರೆ (um->buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ false;
    ಆದರೆ (um->detached)
	vim_free(um->ಪಠ್ಯ);
    um->ಪಠ್ಯ = ಪಠ್ಯ;
    um->ಪಠ್ಯ_ಗಾತ್ರ_2 = (colnr_T)STRLEN(ಪಠ್ಯ) + 1;
    um->ಪಠ್ಯ_ಬದಲಾಯಿತು = true;
    um->detached = true;
    ಹಿಂತಿರುಗಿ true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    ಸ್ಥಿರ bool
um_grow_props(unpacked_memline_T *um, ಪೂರ್ಣಾಂಕ needed)
{
    ಆದರೆ (um->prop_ಎಣಿಕೆ + needed <= um->prop_ಗಾತ್ರ)
	ಹಿಂತಿರುಗಿ true;

    ಪೂರ್ಣಾಂಕ	    new_ಗಾತ್ರ = um->prop_ಎಣಿಕೆ + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_ಗಾತ್ರ * ಗಾತ್ರ(textprop_T));
    ಆದರೆ (newprops == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ false;
    um->props = newprops;
    um->prop_ಗಾತ್ರ = new_ಗಾತ್ರ;

    ಹಿಂತಿರುಗಿ true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
um_ಕೂಡಿಸು_prop(unpacked_memline_T *um, textprop_T *prop)
{
    ಆದರೆ (um->buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!um->detached && !um_detach(um))
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!um_grow_props(um, 1))
    {
	um->buf = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }
    um->props[um->prop_ಎಣಿಕೆ++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    ಶೂನ್ಯ
um_ಹಿಮ್ಮುಖ_props(unpacked_memline_T *um)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, j = um->prop_ಎಣಿಕೆ - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, ಪೂರ್ಣಾಂಕ prop_ಎಣಿಕೆ)
{
    unpacked_memline_T um;

    CLEAR_ಕ್ಷೇತ್ರ(um);
    um.buf = buf;

    ಆದರೆ (lnum < 1 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	um.buf = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ um;
    }

    char_u *ಸಾಲು = ml_get_buf(buf, lnum, FALSE);
    char_u *ಪಠ್ಯ_copy = vim_strsave(ಸಾಲು);

    ಆದರೆ (ಪಠ್ಯ_copy == ಖಾಲಿ)
    {
	um.buf = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ um;
    }
    um.lnum = lnum;
    um.ಪಠ್ಯ = ಪಠ್ಯ_copy;
    um.ಪಠ್ಯ_ಗಾತ್ರ_2 = (colnr_T)STRLEN(ಪಠ್ಯ_copy) + 1;
    um.detached = true;

    ಆದರೆ (prop_ಎಣಿಕೆ > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_ಎಣಿಕೆ);
	ಆದರೆ (um.props == ಖಾಲಿ)
	{
	    vim_free(um.ಪಠ್ಯ);
	    um.buf = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ um;
	}
	um.prop_ಗಾತ್ರ = prop_ಎಣಿಕೆ;
    }

    ಹಿಂತಿರುಗಿ um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					ಪೂರ್ಣಾಂಕ tp_id, ಪೂರ್ಣಾಂಕ tp_type, ಪೂರ್ಣಾಂಕ flag)
{
    unpacked_memline_T	neighbor;

    ಆದರೆ (lnum < 1 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;

    neighbor = um_open_at(buf, lnum, 0);
    ಆದರೆ (neighbor.buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < neighbor.prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	ಆದರೆ (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    ಆದರೆ (!neighbor.detached && !um_detach(&neighbor))
		ನಿಲ್ಲಿಸು;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    ನಿಲ್ಲಿಸು;
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
um_delete_prop(unpacked_memline_T *um, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
    textprop_T *prop;

    ಆದರೆ (um->buf == ಖಾಲಿ || ಸೂಚ್ಯಂಕ < 0 || ಸೂಚ್ಯಂಕ >= um->prop_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!um->detached && !um_detach(um))
	ಹಿಂತಿರುಗಿ;

    prop = &um->props[ಸೂಚ್ಯಂಕ];

    // Adjust continuation flags on neighboring lines before deleting.
    ಆದರೆ (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_ಮುಂದಿನ);
    ಆದರೆ (prop->tp_flags & TP_FLAG_CONT_ಮುಂದಿನ)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    ಆದರೆ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	VIM_CLEAR(prop->u.tp_ಪಠ್ಯ);
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
um_pack(unpacked_memline_T *um, ಪೂರ್ಣಾಂಕ *packed_len)
{
    uint16_t	ಜೀವಂತ_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		vtext_ಗಾತ್ರ = 0;
    ಪೂರ್ಣಾಂಕ		ಒಟ್ಟು_len;
    char_u	*buf;
    char_u	*ಎಣಿಕೆ_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < um->prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T *prop = &um->props[i];

	ಆದರೆ (prop->tp_flags & TP_FLAG_DELETED)
	    ಮುಂದುವರಿ;
	++ಜೀವಂತ_ಎಣಿಕೆ;
	ಆದರೆ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_ಗಾತ್ರ += prop->tp_len + 1;
    }

    ಆದರೆ (ಜೀವಂತ_ಎಣಿಕೆ == 0)
    {
	// No properties: just text, no prop_count header.
	ಒಟ್ಟು_len = um->ಪಠ್ಯ_ಗಾತ್ರ_2;
	buf = alloc(ಒಟ್ಟು_len);
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	*packed_len = ಒಟ್ಟು_len;
	mch_memmove(buf, um->ಪಠ್ಯ, um->ಪಠ್ಯ_ಗಾತ್ರ_2);
	ಹಿಂತಿರುಗಿ buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    ಒಟ್ಟು_len = um->ಪಠ್ಯ_ಗಾತ್ರ_2 + (ಪೂರ್ಣಾಂಕ)PROP_ಎಣಿಕೆ_ಗಾತ್ರ
		+ ಜೀವಂತ_ಎಣಿಕೆ * (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(textprop_T) + vtext_ಗಾತ್ರ;
    buf = alloc(ಒಟ್ಟು_len);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    *packed_len = ಒಟ್ಟು_len;

    mch_memmove(buf, um->ಪಠ್ಯ, um->ಪಠ್ಯ_ಗಾತ್ರ_2);

    ಎಣಿಕೆ_dest = buf + um->ಪಠ್ಯ_ಗಾತ್ರ_2;
    mch_memmove(ಎಣಿಕೆ_dest, &ಜೀವಂತ_ಎಣಿಕೆ, PROP_ಎಣಿಕೆ_ಗಾತ್ರ);

    prop_dest = ಎಣಿಕೆ_dest + PROP_ಎಣಿಕೆ_ಗಾತ್ರ;
    vtext_dest = prop_dest + ಜೀವಂತ_ಎಣಿಕೆ * ಗಾತ್ರ(textprop_T);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < um->prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T prop = um->props[i];

	ಆದರೆ (prop.tp_flags & TP_FLAG_DELETED)
	    ಮುಂದುವರಿ;

	ಆದರೆ (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    ಪೂರ್ಣಾಂಕ copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_ಪಠ್ಯ, copysize);
	    prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ = (colnr_T)(vtext_dest - ಎಣಿಕೆ_dest);
	    vtext_dest += copysize;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, ಗಾತ್ರ(textprop_T));
	prop_dest += ಗಾತ್ರ(textprop_T);
    }

    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    ಪೂರ್ಣಾಂಕ	    packed_len;

    ಆದರೆ (!um->detached || um->buf == ಖಾಲಿ || um->lnum == 0)
	ಹಿಂತಿರುಗಿ;

    packed = um_pack(um, &packed_len);
    ಆದರೆ (packed == ಖಾಲಿ)
    {
	um->buf = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    ಆದರೆ (um->buf->b_ml.ml_ಸಾಲು_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(um->buf->b_ml.ml_flags & ML_ಸಾಲು_DIRTY))
	(ಶೂನ್ಯ)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->ಪಠ್ಯ);
    um_free_vtext(um->props, um->prop_ಎಣಿಕೆ);

    ಆದರೆ (um->buf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_ಸಾಲು_ptr);
    um->buf->b_ml.ml_ಸಾಲು_ptr = packed;
    um->buf->b_ml.ml_ಸಾಲು_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;

    um->detached = false;
    um->ಪಠ್ಯ = packed;
    um->ಪಠ್ಯ_ಗಾತ್ರ_2 = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_ಸಾಲು_textlen = um->ಪಠ್ಯ_ಗಾತ್ರ_2;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    ಶೂನ್ಯ
um_close(unpacked_memline_T *um)
{
    ಆದರೆ (um->buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = ಖಾಲಿ;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    ಶೂನ್ಯ
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = ಖಾಲಿ;
}

/*
 * In a hashtable item "hi_key" points to "pt_name" in a proptype_T.
 * This avoids adding a pointer to the hashtable item.
 * PT2HIKEY() converts a proptype pointer to a hashitem key pointer.
 * HIKEY2PT() converts a hashitem key pointer to a proptype pointer.
 * HI2PT() converts a hashitem pointer to a proptype pointer.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು PT2HIKEY(p)  ((p)->pt_name)
#ವ್ಯಾಖ್ಯಾನಿಸು HIKEY2PT(p)   ((proptype_T *)((p) - offsetof(proptype_T, pt_name)))
#ವ್ಯಾಖ್ಯಾನಿಸು HI2PT(hi)      HIKEY2PT((hi)->hi_ಕೀಲಿ)

// The global text property types.
ಸ್ಥಿರ hashtab_T *global_proptypes = ಖಾಲಿ;
ಸ್ಥಿರ proptype_T **global_proparray = ಖಾಲಿ;

// The last used text property type ID.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    ಸ್ಥಿರ hashitem_T *
ಕಂಡುಹಿಡಿ_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    ಆದರೆ (*name == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (buf == ಖಾಲಿ)
	ht = global_proptypes;
    ಇಲ್ಲದಿದ್ದರೆ
	ht = buf->b_proptypes;

    ಆದರೆ (ht == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    hi = hash_ಕಂಡುಹಿಡಿ(ht, name);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    ಸ್ಥಿರ proptype_T *
ಕಂಡುಹಿಡಿ_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = ಕಂಡುಹಿಡಿ_prop_type_hi(name, buf);

    ಆದರೆ (hi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = ಕಂಡುಹಿಡಿ_prop_type(name, buf);

    ಆದರೆ (pt == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    ಸ್ಥಿರ proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = ಕಂಡುಹಿಡಿ_prop_type(name, buf);

    ಆದರೆ (type == ಖಾಲಿ)
	type = ಕಂಡುಹಿಡಿ_prop_type(name, ಖಾಲಿ);
    ಆದರೆ (type == ಖಾಲಿ)
	semsg(_(e_property_type_str_does_not_exist), name);
    ಹಿಂತಿರುಗಿ type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    ಆದರೆ (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (arg->vval.v_dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;  // NULL dict is like an empty dict
    di = dict_ಕಂಡುಹಿಡಿ(arg->vval.v_dict, (char_u *)"bufnr", -1);
    ಆದರೆ (di != ಖಾಲಿ && (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ
					      || di->di_tv.vval.v_ಸಂಖ್ಯೆ != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	ಆದರೆ (*buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    ಶೂನ್ಯ
f_prop_ಕೂಡಿಸು(typval_T *argvars, typval_T *rettv)
{
    linenr_T	ಆರಂಭ_lnum;
    colnr_T	ಆರಂಭ_ಲಂಬಸಾಲು;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆರಂಭ_lnum = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಆರಂಭ_ಲಂಬಸಾಲು = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    ಆದರೆ (check_for_dict_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = prop_ಕೂಡಿಸು_common(ಆರಂಭ_lnum, ಆರಂಭ_ಲಂಬಸಾಲು,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prop_ಕೂಡಿಸು_one(
	buf_T		*buf,
	char_u		*type_name,
	ಪೂರ್ಣಾಂಕ		id,
	char_u		*ಪಠ್ಯ_arg,
	ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_padding_ಎಡ,
	ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_flags,
	linenr_T	ಆರಂಭ_lnum,
	linenr_T	ಕೊನೆ_lnum,
	colnr_T		ಆರಂಭ_ಲಂಬಸಾಲು,
	colnr_T		ಕೊನೆ_ಲಂಬಸಾಲು)
{
    proptype_T	*type;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		proplen;
    char_u	*props = ಖಾಲಿ;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    ಪೂರ್ಣಾಂಕ		i;
    textprop_T	tmp_prop;
    char_u	*ಪಠ್ಯ = ಪಠ್ಯ_arg;
    ಪೂರ್ಣಾಂಕ		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    ಆದರೆ (type == ಖಾಲಿ)
	ಹೋಗು theend;

    ಆದರೆ (ಆರಂಭ_lnum < 1 || ಆರಂಭ_lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), (ದೀರ್ಘ)ಆರಂಭ_lnum);
	ಹೋಗು theend;
    }
    ಆದರೆ (ಕೊನೆ_lnum < ಆರಂಭ_lnum || ಕೊನೆ_lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), (ದೀರ್ಘ)ಕೊನೆ_lnum);
	ಹೋಗು theend;
    }

    ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
    {
	emsg(_(e_cannot_ಕೂಡಿಸು_ಪಠ್ಯ_property_ಗಮ್ಯ_unloaded_ಬಫರ್));
	ಹೋಗು theend;
    }

    ಆದರೆ (ಪಠ್ಯ != ಖಾಲಿ)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	ಗಾಗಿ (p = ಪಠ್ಯ; *p != NUL; MB_PTR_ADV(p))
	    ಆದರೆ (*p < ' ')
		*p = ' ';
    }

    ಗಾಗಿ (lnum = ಆರಂಭ_lnum; lnum <= ಕೊನೆ_lnum; ++lnum)
    {
	colnr_T ಲಂಬಸಾಲು;	    // start column use in tp_col
	colnr_T ವಿಂಗಡಿಸು_ಲಂಬಸಾಲು;   // column where it appears
	ದೀರ್ಘ	ಉದ್ದಳತೆ;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_ಪಠ್ಯ_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	ಆದರೆ (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_ಪಠ್ಯ_properties_on_a_single_ಸಾಲು));
	    ಹೋಗು theend;
	}

	ಆದರೆ (lnum == ಆರಂಭ_lnum)
	    ಲಂಬಸಾಲು = ಆರಂಭ_ಲಂಬಸಾಲು;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಲಂಬಸಾಲು = 1;
	ಆದರೆ (ಲಂಬಸಾಲು - 1 > (colnr_T)textlen && !(ಲಂಬಸಾಲು == 0 && ಪಠ್ಯ_arg != ಖಾಲಿ))
	{
	    semsg(_(e_invalid_ಸ್ತಂಭ_ಸಂಖ್ಯೆ_nr), (ದೀರ್ಘ)ಆರಂಭ_ಲಂಬಸಾಲು);
	    ಹೋಗು theend;
	}
	ವಿಂಗಡಿಸು_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;

	ಆದರೆ (lnum == ಕೊನೆ_lnum)
	    ಉದ್ದಳತೆ = ಕೊನೆ_ಲಂಬಸಾಲು - ಲಂಬಸಾಲು;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಉದ್ದಳತೆ = (ಪೂರ್ಣಾಂಕ)textlen - ಲಂಬಸಾಲು + 1;
	ಆದರೆ (ಉದ್ದಳತೆ > (ದೀರ್ಘ)textlen)
	    ಉದ್ದಳತೆ = (ಪೂರ್ಣಾಂಕ)textlen;	// can include the end-of-line
	ಆದರೆ (ಉದ್ದಳತೆ < 0)
	    ಉದ್ದಳತೆ = 0;		// zero-width property

	ಆದರೆ (ಪಠ್ಯ_arg != ಖಾಲಿ)
	{
	    ಉದ್ದಳತೆ = (ದೀರ್ಘ)STRLEN(ಪಠ್ಯ_arg);
	    ಆದರೆ (ಲಂಬಸಾಲು == 0)
	    {
		ಲಂಬಸಾಲು = MAXCOL;	// before or after the line
		ಆದರೆ ((ಪಠ್ಯ_flags & TP_FLAG_ALIGN_ABOVE) == 0)
		    ವಿಂಗಡಿಸು_ಲಂಬಸಾಲು = MAXCOL;
	    }
	}

	// Build the new property.
	CLEAR_ಕ್ಷೇತ್ರ(tmp_prop);
	tmp_prop.tp_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	tmp_prop.tp_len = ಉದ್ದಳತೆ;
	tmp_prop.tp_id = id;
	tmp_prop.tp_type = type->pt_id;
	tmp_prop.tp_flags = ಪಠ್ಯ_flags
			    | (lnum > ಆರಂಭ_lnum ? TP_FLAG_CONT_PREV : 0)
			    | (lnum < ಕೊನೆ_lnum ? TP_FLAG_CONT_ಮುಂದಿನ : 0)
			    | ((type->pt_flags & PT_FLAG_INS_ಆರಂಭ_INCL)
						     ? TP_FLAG_ಆರಂಭ_INCL : 0);
	tmp_prop.tp_padleft = ಪಠ್ಯ_padding_ಎಡ;
	ಆದರೆ (ಪಠ್ಯ_arg != ಖಾಲಿ)
	    tmp_prop.u.tp_ಪಠ್ಯ = ಪಠ್ಯ_arg;

	// Find the index where to insert the new property.
	ಗಾಗಿ (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_ಲಂಬಸಾಲು;

	    mch_memmove(&existing, props + i * ಗಾತ್ರ(textprop_T),
							   ಗಾತ್ರ(textprop_T));
	    prop_ಲಂಬಸಾಲು = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_ಲಂಬಸಾಲು;
	    ಆದರೆ (prop_ಲಂಬಸಾಲು >= ವಿಂಗಡಿಸು_ಲಂಬಸಾಲು)
		ನಿಲ್ಲಿಸು;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	ಪೂರ್ಣಾಂಕ		vtext_ಒಟ್ಟು = 0;
	ಪೂರ್ಣಾಂಕ		new_ಸಾಲು_len;
	char_u	*ಎಣಿಕೆ_dest;
	char_u	*vtext_dest;
	ಪೂರ್ಣಾಂಕ		j;

	// Compute total vtext size from existing props.
	ಗಾಗಿ (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * ಗಾತ್ರ(textprop_T),
							ಗಾತ್ರ(textprop_T));
	    ಆದರೆ (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_ಒಟ್ಟು += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	ಆದರೆ (ಪಠ್ಯ_arg != ಖಾಲಿ)
	    vtext_ಒಟ್ಟು += ಉದ್ದಳತೆ + 1;

	new_ಸಾಲು_len = (ಪೂರ್ಣಾಂಕ)textlen + (ಪೂರ್ಣಾಂಕ)PROP_ಎಣಿಕೆ_ಗಾತ್ರ
			+ new_propcount * (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(textprop_T)
			+ vtext_ಒಟ್ಟು;
	newtext = alloc(new_ಸಾಲು_len);
	ಆದರೆ (newtext == ಖಾಲಿ)
	    ಹೋಗು theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_ಸಾಲು_ptr, textlen);

	// Write prop_count.
	ಎಣಿಕೆ_dest = newtext + textlen;
	mch_memmove(ಎಣಿಕೆ_dest, &new_propcount, PROP_ಎಣಿಕೆ_ಗಾತ್ರ);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = ಎಣಿಕೆ_dest + PROP_ಎಣಿಕೆ_ಗಾತ್ರ;
	ಆದರೆ (i > 0)
	    mch_memmove(newprops, props, ಗಾತ್ರ(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	ಆದರೆ (i < proplen)
	    mch_memmove(newprops + (i + 1) * ಗಾತ್ರ(textprop_T),
					props + i * ಗಾತ್ರ(textprop_T),
					ಗಾತ್ರ(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * ಗಾತ್ರ(textprop_T);
	ಗಾಗಿ (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    ಆದರೆ (j == i)
		ಮುಂದುವರಿ;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * ಗಾತ್ರ(textprop_T),
		    ಗಾತ್ರ(textprop_T));
	    ಆದರೆ (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_ಎಣಿಕೆ_2 = (char_u *)props - PROP_ಎಣಿಕೆ_ಗಾತ್ರ;
		char_u *old_vtext = old_ಎಣಿಕೆ_2 + ep.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ = (colnr_T)(vtext_dest - ಎಣಿಕೆ_dest);
		mch_memmove(newprops + j * ಗಾತ್ರ(textprop_T), &ep,
			ಗಾತ್ರ(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	ಆದರೆ (ಪಠ್ಯ_arg != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, ಪಠ್ಯ_arg, copysize);
	    tmp_prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ =
				    (colnr_T)(vtext_dest - ಎಣಿಕೆ_dest);
	    vtext_dest += copysize;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    tmp_prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ = 0;

	mch_memmove(newprops + i * ಗಾತ್ರ(textprop_T), &tmp_prop,
							ಗಾತ್ರ(textprop_T));

	ಆದರೆ (buf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_ಸಾಲು_ptr);
	buf->b_ml.ml_ಸಾಲು_ptr = newtext;
	buf->b_ml.ml_ಸಾಲು_len = new_ಸಾಲು_len;
	buf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;
    }

    ಬದಲಾಯಿತು_ಸಾಲು_display_buf(buf);
    ಬದಲಾಯಿತು_ಸಾಲುಗಳು_buf(buf, ಆರಂಭ_lnum, ಕೊನೆ_lnum + 1, 0);
    res = OK;

theend:
    vim_free(ಪಠ್ಯ);
    ಹಿಂತಿರುಗಿ res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    ಶೂನ್ಯ
f_prop_ಕೂಡಿಸು_list(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    ಪೂರ್ಣಾಂಕ		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	ಆರಂಭ_lnum;
    colnr_T	ಆರಂಭ_ಲಂಬಸಾಲು;
    linenr_T	ಕೊನೆ_lnum;
    colnr_T	ಕೊನೆ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    ಪೂರ್ಣಾಂಕ		prev_did_emsg = did_emsg;

    ಆದರೆ (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    dict = argvars[0].vval.v_dict;
    ಆದರೆ (dict == ಖಾಲಿ || !dict_has_ಕೀಲಿ(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ಹಿಂತಿರುಗಿ;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_ಸಂಖ್ಯೆ(dict, "id");
	ಆದರೆ (x > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ಹಿಂತಿರುಗಿ;
	}
	id = (ಪೂರ್ಣಾಂಕ)x;
    }

    ಆದರೆ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_ವಸ್ತುಗಳು(argvars[1].vval.v_list, li)
    {
	ಆದರೆ (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == ಖಾಲಿ)
	{
	    emsg(_(e_list_required));
	    ಹಿಂತಿರುಗಿ;
	}

	pos_list = li->li_tv.vval.v_list;
	ಆರಂಭ_lnum = list_ಕಂಡುಹಿಡಿ_nr(pos_list, 0L, &error);
	ಆದರೆ (!error)
	    ಆರಂಭ_ಲಂಬಸಾಲು = list_ಕಂಡುಹಿಡಿ_nr(pos_list, 1L, &error);
	ಆದರೆ (!error)
	    ಕೊನೆ_lnum = list_ಕಂಡುಹಿಡಿ_nr(pos_list, 2L, &error);
	ಆದರೆ (!error)
	    ಕೊನೆ_ಲಂಬಸಾಲು = list_ಕಂಡುಹಿಡಿ_nr(pos_list, 3L, &error);
	ಪೂರ್ಣಾಂಕ this_id = id;
	ಆದರೆ (!error && pos_list->lv_len > 4)
	    this_id = list_ಕಂಡುಹಿಡಿ_nr(pos_list, 4L, &error);
	ಆದರೆ (error || ಆರಂಭ_lnum <= 0 || ಆರಂಭ_ಲಂಬಸಾಲು <= 0
		  || ಕೊನೆ_lnum <= 0 || ಕೊನೆ_ಲಂಬಸಾಲು <= 0)
	{
	    ಆದರೆ (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_ವಾದ));
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (prop_ಕೂಡಿಸು_one(buf, type_name, this_id, ಖಾಲಿ, 0, 0,
			     ಆರಂಭ_lnum, ಕೊನೆ_lnum, ಆರಂಭ_ಲಂಬಸಾಲು, ಕೊನೆ_ಲಂಬಸಾಲು) == FAIL)
	    ಹಿಂತಿರುಗಿ;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_textprop_id(ಶೂನ್ಯ)
{
    ಆದರೆ (vt_id_counter > 0)
	vt_id_counter = 0;
    ಹಿಂತಿರುಗಿ --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    ಪೂರ್ಣಾಂಕ
prop_ಕೂಡಿಸು_common(
	linenr_T    ಆರಂಭ_lnum,
	colnr_T	    ಆರಂಭ_ಲಂಬಸಾಲು,
	dict_T	    *dict,
	buf_T	    *default_buf,
	typval_T    *dict_arg)
{
    linenr_T	ಕೊನೆ_lnum;
    colnr_T	ಕೊನೆ_ಲಂಬಸಾಲು;
    char_u	*type_name;
    buf_T	*buf = default_buf;
    ಪೂರ್ಣಾಂಕ		id = 0;
    char_u	*ಪಠ್ಯ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_padding_ಎಡ = 0;
    ಪೂರ್ಣಾಂಕ		flags = 0;

    ಆದರೆ (dict == ಖಾಲಿ || !dict_has_ಕೀಲಿ(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ಹೋಗು theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "end_lnum"))
    {
	ಕೊನೆ_lnum = dict_get_ಸಂಖ್ಯೆ(dict, "end_lnum");
	ಆದರೆ (ಕೊನೆ_lnum < ಆರಂಭ_lnum)
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "end_lnum");
	    ಹೋಗು theend;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೊನೆ_lnum = ಆರಂಭ_lnum;

    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "length"))
    {
	ದೀರ್ಘ ಉದ್ದಳತೆ = dict_get_ಸಂಖ್ಯೆ(dict, "length");

	ಆದರೆ (ಉದ್ದಳತೆ < 0 || ಕೊನೆ_lnum > ಆರಂಭ_lnum)
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "length");
	    ಹೋಗು theend;
	}
	ಕೊನೆ_ಲಂಬಸಾಲು = ಆರಂಭ_ಲಂಬಸಾಲು + ಉದ್ದಳತೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dict_has_ಕೀಲಿ(dict, "end_col"))
    {
	ಕೊನೆ_ಲಂಬಸಾಲು = dict_get_ಸಂಖ್ಯೆ(dict, "end_col");
	ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು <= 0)
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "end_col");
	    ಹೋಗು theend;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆರಂಭ_lnum == ಕೊನೆ_lnum)
	ಕೊನೆ_ಲಂಬಸಾಲು = ಆರಂಭ_ಲಂಬಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೊನೆ_ಲಂಬಸಾಲು = 1;

    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_ಸಂಖ್ಯೆ(dict, "id");
	ಆದರೆ (x > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ಹೋಗು theend;
	}
	id = (ಪೂರ್ಣಾಂಕ)x;
    }

    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "text"))
    {
	ಆದರೆ (dict_has_ಕೀಲಿ(dict, "length")
		|| dict_has_ಕೀಲಿ(dict, "end_col")
		|| dict_has_ಕೀಲಿ(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_ಉದ್ದಳತೆ_endcol_and_endlnum_with_ಪಠ್ಯ));
	    ಹೋಗು theend;
	}

	ಪಠ್ಯ = dict_get_string(dict, "text", TRUE);
	ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ)
	    ಹೋಗು theend;
	// use a default length of 1 to make multiple props show up
	ಕೊನೆ_ಲಂಬಸಾಲು = ಆರಂಭ_ಲಂಬಸಾಲು + 1;

	ಆದರೆ (dict_has_ಕೀಲಿ(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    ಆದರೆ (p == ಖಾಲಿ)
		ಹೋಗು theend;
	    ಆದರೆ (ಆರಂಭ_ಲಂಬಸಾಲು != 0)
	    {
		emsg(_(e_can_only_use_ಪಠ್ಯ_align_when_ಸ್ತಂಭ_is_zero));
		ಹೋಗು theend;
	    }
	    ಆದರೆ (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_ಬಲ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_ಕೆಳಗೆ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "text_align", p);
		ಹೋಗು theend;
	    }
	}

	ಆದರೆ (dict_has_ಕೀಲಿ(dict, "text_padding_left"))
	{
	    ಪಠ್ಯ_padding_ಎಡ = dict_get_ಸಂಖ್ಯೆ(dict, "text_padding_left");
	    ಆದರೆ (ಪಠ್ಯ_padding_ಎಡ < 0)
	    {
		semsg(_(e_ವಾದ_must_be_ಧನಾತ್ಮಕ_str), "text_padding_left");
		ಹೋಗು theend;
	    }
	}

	ಆದರೆ (dict_has_ಕೀಲಿ(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    ಆದರೆ (p == ಖಾಲಿ)
		ಹೋಗು theend;
	    ಆದರೆ (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "text_wrap", p);
		ಹೋಗು theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    ಆದರೆ (ಆರಂಭ_ಲಂಬಸಾಲು < (ಪಠ್ಯ == ಖಾಲಿ ? 1 : 0))
    {
	semsg(_(e_invalid_ಸ್ತಂಭ_ಸಂಖ್ಯೆ_nr), (ದೀರ್ಘ)ಆರಂಭ_ಲಂಬಸಾಲು);
	ಹೋಗು theend;
    }
    ಆದರೆ (ಆರಂಭ_ಲಂಬಸಾಲು > 0 && ಪಠ್ಯ_padding_ಎಡ > 0)
    {
	emsg(_(e_can_only_use_ಎಡ_padding_when_ಸ್ತಂಭ_is_zero));
	ಹೋಗು theend;
    }

    ಆದರೆ (dict_arg != ಖಾಲಿ && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	ಹೋಗು theend;

    ಆದರೆ (ಪಠ್ಯ != ಖಾಲಿ)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	ಹೋಗು theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_ಕೂಡಿಸು_one(buf, type_name, id, ಪಠ್ಯ, ಪಠ್ಯ_padding_ಎಡ, flags,
				    ಆರಂಭ_lnum, ಕೊನೆ_lnum, ಆರಂಭ_ಲಂಬಸಾಲು, ಕೊನೆ_ಲಂಬಸಾಲು);
    ಪಠ್ಯ = ಖಾಲಿ;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(ಪಠ್ಯ);
    ಹಿಂತಿರುಗಿ id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    ಪೂರ್ಣಾಂಕ
get_ಪಠ್ಯ_props(buf_T *buf, linenr_T lnum, char_u **props, ಪೂರ್ಣಾಂಕ will_change)
{
    char_u	*ಪಠ್ಯ;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_ಎಣಿಕೆ;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    ಆದರೆ ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    // Fetch the line to get the ml_line_len field updated.
    ಪಠ್ಯ = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_ಸಾಲು_len - textlen;
    ಆದರೆ (propdata_len == 0)
	ಹಿಂತಿರುಗಿ 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    ಆದರೆ (propdata_len < PROP_ಎಣಿಕೆ_ಗಾತ್ರ + ಗಾತ್ರ(textprop_T))
    {
	emsg(e_ಪಠ್ಯ_property_info_corrupted);
	ಹಿಂತಿರುಗಿ 0;
    }
    mch_memmove(&prop_ಎಣಿಕೆ, ಪಠ್ಯ + textlen, PROP_ಎಣಿಕೆ_ಗಾತ್ರ);
    ಆದರೆ (!ಪಠ್ಯ_prop_ಎಣಿಕೆ_valid(prop_ಎಣಿಕೆ, propdata_len))
    {
	emsg(e_ಪಠ್ಯ_property_info_corrupted);
	ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (!ಪಠ್ಯ_prop_vtext_valid(ಪಠ್ಯ + textlen + PROP_ಎಣಿಕೆ_ಗಾತ್ರ,
					   (ಪೂರ್ಣಾಂಕ)prop_ಎಣಿಕೆ, propdata_len))
    {
	emsg(e_ಪಠ್ಯ_property_info_corrupted);
	ಹಿಂತಿರುಗಿ 0;
    }
    *props = ಪಠ್ಯ + textlen + PROP_ಎಣಿಕೆ_ಗಾತ್ರ;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)prop_ಎಣಿಕೆ;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    ಪೂರ್ಣಾಂಕ
prop_ಎಣಿಕೆ_above_ಕೆಳಗೆ(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = get_ಪಠ್ಯ_props(buf, lnum, &props, FALSE);
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ = 0;
    textprop_T	prop;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_ಬಲ_goes_ಕೆಳಗೆ = FALSE;

    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	ಹಿಂತಿರುಗಿ 0;
    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	mch_memmove(&prop, props + i * ಗಾತ್ರ(prop), ಗಾತ್ರ(prop));
	ಆದರೆ (prop.tp_ಲಂಬಸಾಲು == MAXCOL && ಪಠ್ಯ_prop_type_valid(buf, &prop))
	{
	    ಆದರೆ ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_ಕೆಳಗೆ))
		    || (ಮುಂದಿನ_ಬಲ_goes_ಕೆಳಗೆ
				     && (prop.tp_flags & TP_FLAG_ALIGN_ಬಲ)))
	    {
		++ಫಲಿತಾಂಶ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prop.tp_flags & TP_FLAG_ALIGN_ಬಲ)
		ಮುಂದಿನ_ಬಲ_goes_ಕೆಳಗೆ = TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    ಪೂರ್ಣಾಂಕ
ಎಣಿಕೆ_props(linenr_T lnum, ಪೂರ್ಣಾಂಕ only_starting, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_ಸಾಲು)
{
    char_u	*props;
    ಪೂರ್ಣಾಂಕ		proplen = get_ಪಠ್ಯ_props(curbuf, lnum, &props, 0);
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ = proplen;
    textprop_T	prop;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * ಗಾತ್ರ(prop), ಗಾತ್ರ(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	ಆದರೆ ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!ಕೊನೆಯ_ಸಾಲು && prop.tp_ಲಂಬಸಾಲು == MAXCOL))
	    --ಫಲಿತಾಂಶ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ textprop_T	*ಪಠ್ಯ_prop_compare_props;
ಸ್ಥಿರ buf_T		*ಪಠ್ಯ_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪಠ್ಯ_prop_ಕ್ರಮ(ಪೂರ್ಣಾಂಕ flags)
{
    ಆದರೆ (flags & TP_FLAG_ALIGN_ABOVE)
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (flags & TP_FLAG_ALIGN_ಬಲ)
	ಹಿಂತಿರುಗಿ 2;
    ಆದರೆ (flags & TP_FLAG_ALIGN_ಕೆಳಗೆ)
	ಹಿಂತಿರುಗಿ 3;
    ಹಿಂತಿರುಗಿ 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪಠ್ಯ_prop_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ಪೂರ್ಣಾಂಕ  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(ಪೂರ್ಣಾಂಕ *)s1;
    idx2 = *(ಪೂರ್ಣಾಂಕ *)s2;
    tp1 = &ಪಠ್ಯ_prop_compare_props[idx1];
    tp2 = &ಪಠ್ಯ_prop_compare_props[idx2];
    col1 = tp1->tp_ಲಂಬಸಾಲು;
    col2 = tp2->tp_ಲಂಬಸಾಲು;

    // property that inserts text has priority over one that doesn't
    ಆದರೆ ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	ಹಿಂತಿರುಗಿ tp1->tp_id < 0 ? 1 : -1;

    ಆದರೆ (col1 == MAXCOL || col2 == MAXCOL)
    {
	ಪೂರ್ಣಾಂಕ order1 = ಪಠ್ಯ_prop_ಕ್ರಮ(tp1->tp_flags);
	ಪೂರ್ಣಾಂಕ order2 = ಪಠ್ಯ_prop_ಕ್ರಮ(tp2->tp_flags);

	// sort on order where it is added
	ಆದರೆ (order1 != order2)
	    ಹಿಂತಿರುಗಿ order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = ಪಠ್ಯ_prop_type_by_id(ಪಠ್ಯ_prop_compare_buf, tp1->tp_type);
    pt2 = ಪಠ್ಯ_prop_type_by_id(ಪಠ್ಯ_prop_compare_buf, tp2->tp_type);
    ಆದರೆ (pt1 != pt2)
    {
	ಆದರೆ (pt1 == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (pt2 == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ 1;
	ಆದರೆ (pt1->pt_ಆದ್ಯತೆ != pt2->pt_ಆದ್ಯತೆ)
	    ಹಿಂತಿರುಗಿ pt1->pt_ಆದ್ಯತೆ > pt2->pt_ಆದ್ಯತೆ ? 1 : -1;
    }

    // same priority, one that starts first wins
    ಆದರೆ (col1 != col2)
	ಹಿಂತಿರುಗಿ col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    ಆದರೆ (tp1->tp_id < 0)
	ಹಿಂತಿರುಗಿ tp1->tp_id > tp2->tp_id ? 1 : -1;

    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    ಶೂನ್ಯ
ವಿಂಗಡಿಸು_ಪಠ್ಯ_props(
	buf_T	    *buf,
	textprop_T  *props,
	ಪೂರ್ಣಾಂಕ	    *idxs,
	ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2)
{
    ಪಠ್ಯ_prop_compare_buf = buf;
    ಪಠ್ಯ_prop_compare_props = props;
    qsort((ಶೂನ್ಯ *)idxs, (size_t)ಎಣಿಕೆ_2, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), ಪಠ್ಯ_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
ಕಂಡುಹಿಡಿ_prop_ಒಳ_ಸಾಲುಗಳು(
	win_T	    *wp,
	ಪೂರ್ಣಾಂಕ	    type_id,
	ಪೂರ್ಣಾಂಕ	    id,
	textprop_T  *prop,
	linenr_T    *ಸಿಕ್ಕಿತು_lnum,
	linenr_T    ಮೊದಲ_lnum,
	linenr_T    ಕೊನೆಯ_lnum)
{
    ಆದರೆ (ಪಠ್ಯ_prop_type_by_id(wp->w_ಬಫರ್, type_id) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ false;

    ಆದರೆ (ಮೊದಲ_lnum < 1)
	ಮೊದಲ_lnum = 1;
    ಆದರೆ (ಕೊನೆಯ_lnum > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಕೊನೆಯ_lnum = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಗಾಗಿ (linenr_T lnum = ಮೊದಲ_lnum; lnum <= ಕೊನೆಯ_lnum; ++lnum)
    {
	char_u	*props;
	ಪೂರ್ಣಾಂಕ	ಎಣಿಕೆ_2 = get_ಪಠ್ಯ_props(wp->w_ಬಫರ್, lnum, &props, FALSE);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; ++i)
	{
	    mch_memmove(prop, props + i * ಗಾತ್ರ(textprop_T),
							   ಗಾತ್ರ(textprop_T));
	    ಆದರೆ (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*ಸಿಕ್ಕಿತು_lnum = lnum;
		ಹಿಂತಿರುಗಿ true;
	    }
	}
    }
    ಹಿಂತಿರುಗಿ false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
ಕಂಡುಹಿಡಿ_visible_prop(
	win_T	    *wp,
	ಪೂರ್ಣಾಂಕ	    type_id,
	ಪೂರ್ಣಾಂಕ	    id,
	textprop_T  *prop,
	linenr_T    *ಸಿಕ್ಕಿತು_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_prop_ಒಳ_ಸಾಲುಗಳು(wp, type_id, id, prop, ಸಿಕ್ಕಿತು_lnum,
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
ಗಣ_ಪಠ್ಯ_props(linenr_T lnum, textprop_T *tps, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    char_u  *ಪಠ್ಯ;
    char_u  *newtext;
    ಪೂರ್ಣಾಂಕ	    textlen;

    ಪಠ್ಯ = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	ಆದರೆ (newtext == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	mch_memmove(newtext, ಪಠ್ಯ, textlen);
	ಆದರೆ (curbuf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ಸಾಲು_ptr);
	curbuf->b_ml.ml_ಸಾಲು_ptr = newtext;
	curbuf->b_ml.ml_ಸಾಲು_len = textlen;
	curbuf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_ಎಣಿಕೆ = (uint16_t)ಎಣಿಕೆ_2;
	ಪೂರ್ಣಾಂಕ	    vtext_ಗಾತ್ರ = 0;
	ಪೂರ್ಣಾಂಕ	    ಒಟ್ಟು_len;
	char_u	    *ಎಣಿಕೆ_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	ಪೂರ್ಣಾಂಕ	    i;

	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
	    ಆದರೆ (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_ಗಾತ್ರ += tps[i].tp_len + 1;

	ಒಟ್ಟು_len = textlen + (ಪೂರ್ಣಾಂಕ)PROP_ಎಣಿಕೆ_ಗಾತ್ರ
		    + ಎಣಿಕೆ_2 * (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(textprop_T) + vtext_ಗಾತ್ರ;
	newtext = alloc(ಒಟ್ಟು_len);
	ಆದರೆ (newtext == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	mch_memmove(newtext, ಪಠ್ಯ, textlen);

	ಎಣಿಕೆ_dest = newtext + textlen;
	mch_memmove(ಎಣಿಕೆ_dest, &prop_ಎಣಿಕೆ, PROP_ಎಣಿಕೆ_ಗಾತ್ರ);

	prop_dest = ಎಣಿಕೆ_dest + PROP_ಎಣಿಕೆ_ಗಾತ್ರ;
	vtext_dest = prop_dest + ಎಣಿಕೆ_2 * ಗಾತ್ರ(textprop_T);

	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
	{
	    textprop_T prop = tps[i];

	    ಆದರೆ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		ಪೂರ್ಣಾಂಕ copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_ಪಠ್ಯ, copysize);
		vim_free(tps[i].u.tp_ಪಠ್ಯ);
		tps[i].u.tp_ಪಠ್ಯ = ಖಾಲಿ;
		prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ = (colnr_T)(vtext_dest - ಎಣಿಕೆ_dest);
		vtext_dest += copysize;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ = 0;
	    mch_memmove(prop_dest, &prop, ಗಾತ್ರ(textprop_T));
	    prop_dest += ಗಾತ್ರ(textprop_T);
	}

	ಆದರೆ (curbuf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ಸಾಲು_ptr);
	curbuf->b_ml.ml_ಸಾಲು_ptr = newtext;
	curbuf->b_ml.ml_ಸಾಲು_len = ಒಟ್ಟು_len;
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
props_ಕೂಡಿಸು_ಎಣಿಕೆ_ಶೀರ್ಷಿಕೆ(char_u *ಸಾಲು, ಪೂರ್ಣಾಂಕ ಸಾಲು_len, ಪೂರ್ಣಾಂಕ textlen, ಪೂರ್ಣಾಂಕ *new_len)
{
    ಪೂರ್ಣಾಂಕ		prop_ಬೈಟುಗಳು = ಸಾಲು_len - textlen;
    uint16_t	prop_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		vtext_ಗಾತ್ರ = 0;
    ಪೂರ್ಣಾಂಕ		ಒಟ್ಟು;
    char_u	*newline;
    char_u	*ಎಣಿಕೆ_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (prop_ಬೈಟುಗಳು <= 0 || prop_ಬೈಟುಗಳು % ಗಾತ್ರ(textprop_T) != 0)
    {
	*new_len = ಸಾಲು_len;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    prop_ಎಣಿಕೆ = (uint16_t)(prop_ಬೈಟುಗಳು / ಗಾತ್ರ(textprop_T));

    // Calculate total vtext size.
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ಸಾಲು + textlen + i * ಗಾತ್ರ(textprop_T),
							ಗಾತ್ರ(textprop_T));
	ಆದರೆ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_ಗಾತ್ರ += prop.tp_len + 1;
    }

    ಒಟ್ಟು = textlen + (ಪೂರ್ಣಾಂಕ)PROP_ಎಣಿಕೆ_ಗಾತ್ರ + prop_ಬೈಟುಗಳು + vtext_ಗಾತ್ರ;
    newline = alloc(ಒಟ್ಟು);
    ಆದರೆ (newline == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    *new_len = ಒಟ್ಟು;

    mch_memmove(newline, ಸಾಲು, textlen);
    ಎಣಿಕೆ_dest = newline + textlen;
    mch_memmove(ಎಣಿಕೆ_dest, &prop_ಎಣಿಕೆ, PROP_ಎಣಿಕೆ_ಗಾತ್ರ);
    prop_dest = ಎಣಿಕೆ_dest + PROP_ಎಣಿಕೆ_ಗಾತ್ರ;
    vtext_dest = prop_dest + prop_ಬೈಟುಗಳು;

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ಸಾಲು + textlen + i * ಗಾತ್ರ(textprop_T),
							ಗಾತ್ರ(textprop_T));
	ಆದರೆ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    ಪೂರ್ಣಾಂಕ copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_ಪಠ್ಯ, copysize);
	    vim_free(prop.u.tp_ಪಠ್ಯ);
	    prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ = (colnr_T)(vtext_dest - ಎಣಿಕೆ_dest);
	    vtext_dest += copysize;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ = 0;
	mch_memmove(prop_dest + i * ಗಾತ್ರ(textprop_T), &prop,
							ಗಾತ್ರ(textprop_T));
    }
    ಹಿಂತಿರುಗಿ newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    ಶೂನ್ಯ
ಕೂಡಿಸು_ಪಠ್ಯ_props(linenr_T lnum, textprop_T *ಪಠ್ಯ_props, ಪೂರ್ಣಾಂಕ ಪಠ್ಯ_prop_ಎಣಿಕೆ)
{
    unpacked_memline_T	um;
    ಪೂರ್ಣಾಂಕ			i;

    um = um_open_at(curbuf, lnum, ಪಠ್ಯ_prop_ಎಣಿಕೆ);
    ಆದರೆ (um.buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!um_detach(&um))
    {
	um_abort(&um);
	ಹಿಂತಿರುಗಿ;
    }

    // Grow props array if needed.
    ಆದರೆ (um.prop_ಎಣಿಕೆ + ಪಠ್ಯ_prop_ಎಣಿಕೆ > um.prop_ಗಾತ್ರ)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_ಎಣಿಕೆ + ಪಠ್ಯ_prop_ಎಣಿಕೆ) * ಗಾತ್ರ(textprop_T));
	ಆದರೆ (newprops == ಖಾಲಿ)
	{
	    um_abort(&um);
	    ಹಿಂತಿರುಗಿ;
	}
	um.props = newprops;
	um.prop_ಗಾತ್ರ = um.prop_ಎಣಿಕೆ + ಪಠ್ಯ_prop_ಎಣಿಕೆ;
    }

    ಗಾಗಿ (i = 0; i < ಪಠ್ಯ_prop_ಎಣಿಕೆ; ++i)
	um.props[um.prop_ಎಣಿಕೆ++] = ಪಠ್ಯ_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compare_pt(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    ಹಿಂತಿರುಗಿ tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    ಸ್ಥಿರ proptype_T *
ಕಂಡುಹಿಡಿ_type_by_id(hashtab_T *ht, proptype_T ***array, ಪೂರ್ಣಾಂಕ id)
{
    ಪೂರ್ಣಾಂಕ ಕೆಳಗಿನ = 0;
    ಪೂರ್ಣಾಂಕ ಮೇಲಿನ;

    ಆದರೆ (ht == ಖಾಲಿ || ht->ht_ಬಳಸಿದ == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    ಆದರೆ (*array == ಖಾಲಿ)
    {
	ದೀರ್ಘ	    todo;
	hashitem_T  *hi;
	ಪೂರ್ಣಾಂಕ	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_ಬಳಸಿದ);
	ಆದರೆ (*array == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	todo = (ದೀರ್ಘ)ht->ht_ಬಳಸಿದ;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
	{
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((ಶೂನ್ಯ *)*array, ht->ht_ಬಳಸಿದ, ಗಾತ್ರ(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    ಮೇಲಿನ = ht->ht_ಬಳಸಿದ;
    ಆಗ (ಮೇಲಿನ > ಕೆಳಗಿನ)
    {
	ಪೂರ್ಣಾಂಕ m = (ಮೇಲಿನ + ಕೆಳಗಿನ) / 2;

	ಆದರೆ ((*array)[m]->pt_id == id)
	    ಹಿಂತಿರುಗಿ (*array)[m];
	ಆದರೆ ((*array)[m]->pt_id > id)
	    ಮೇಲಿನ = m;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೆಳಗಿನ = m + 1;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    ಪೂರ್ಣಾಂಕ buflocal = TRUE;
    ಪೂರ್ಣಾಂಕ virtualtext_prop = prop->tp_id < 0;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "col", (prop->tp_ಲಂಬಸಾಲು == MAXCOL) ? 0 : prop->tp_ಲಂಬಸಾಲು);
    ಆದರೆ (!virtualtext_prop)
    {
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "length", prop->tp_len);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "id", prop->tp_id);
    }
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_ಮುಂದಿನ));

    pt = ಕಂಡುಹಿಡಿ_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    ಆದರೆ (pt == ಖಾಲಿ)
    {
	pt = ಕಂಡುಹಿಡಿ_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    ಆದರೆ (pt != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_string(dict, "type", pt->pt_name);

    ಆದರೆ (buflocal)
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "type_bufnr", buf->b_fnum);
    ಇಲ್ಲದಿದ್ದರೆ
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "type_bufnr", 0);
    ಆದರೆ (virtualtext_prop)
    {
	string_T    ಪಠ್ಯ_align = {ಖಾಲಿ, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_ಕೂಡಿಸು_string(dict, "text", prop->u.tp_ಪಠ್ಯ);

	// text_align
	ಆದರೆ (prop->tp_flags & TP_FLAG_ALIGN_ಬಲ)
	    STR_LITERAL_ಗಣ(ಪಠ್ಯ_align, "right");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_ಗಣ(ಪಠ್ಯ_align, "above");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prop->tp_flags & TP_FLAG_ALIGN_ಕೆಳಗೆ)
	    STR_LITERAL_ಗಣ(ಪಠ್ಯ_align, "below");
	ಆದರೆ (ಪಠ್ಯ_align.string != ಖಾಲಿ)
	    dict_ಕೂಡಿಸು_string_len(dict, "text_align",
		ಪಠ್ಯ_align.string, (ಪೂರ್ಣಾಂಕ)ಪಠ್ಯ_align.ಉದ್ದಳತೆ);

	// text_wrap
	ಆದರೆ (prop->tp_flags & TP_FLAG_WRAP)
	    dict_ಕೂಡಿಸು_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	ಆದರೆ (prop->tp_padleft != 0)
	    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
ಪಠ್ಯ_prop_type_by_id(buf_T *buf, ಪೂರ್ಣಾಂಕ id)
{
    proptype_T *type;

    type = ಕಂಡುಹಿಡಿ_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    ಆದರೆ (type == ಖಾಲಿ)
	type = ಕಂಡುಹಿಡಿ_type_by_id(global_proptypes, &global_proparray, id);
    ಹಿಂತಿರುಗಿ type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
ಪಠ್ಯ_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    ಹಿಂತಿರುಗಿ ಪಠ್ಯ_prop_type_by_id(buf, prop->tp_type) != ಖಾಲಿ;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    ಶೂನ್ಯ
f_prop_clear(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    linenr_T ಆರಂಭ;
    linenr_T ಕೊನೆ;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    ಪೂರ್ಣಾಂಕ	    did_clear = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆರಂಭ = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಕೊನೆ = ಆರಂಭ;
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಕೊನೆ = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ಆದರೆ (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		ಹಿಂತಿರುಗಿ;
	}
    }
    ಆದರೆ (ಆರಂಭ < 1 || ಕೊನೆ < 1)
    {
	emsg(_(e_invalid_range));
	ಹಿಂತಿರುಗಿ;
    }

    {
	unpacked_memline_T um = um_open(buf);

	ಗಾಗಿ (lnum = ಆರಂಭ; lnum <= ಕೊನೆ; ++lnum)
	{
	    ಪೂರ್ಣಾಂಕ idx;

	    ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (!um_goto_ಸಾಲು(&um, lnum, 0))
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (um.prop_ಎಣಿಕೆ > 0)
	    {
		did_clear = TRUE;
		ಗಾಗಿ (idx = um.prop_ಎಣಿಕೆ - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    ಆದರೆ (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    ಶೂನ್ಯ
f_prop_ಕಂಡುಹಿಡಿ(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    ಪೂರ್ಣಾಂಕ		lnum_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_pos_has_prop = 0;
    ಪೂರ್ಣಾಂಕ		ನೋಡಿದ_ಕೊನೆ = FALSE;
    ಪೂರ್ಣಾಂಕ		id = 0;
    ಪೂರ್ಣಾಂಕ		id_ಸಿಕ್ಕಿತು = FALSE;
    ಪೂರ್ಣಾಂಕ		type_id = -1;
    ಪೂರ್ಣಾಂಕ		skipstart = FALSE;
    ಪೂರ್ಣಾಂಕ		lnum = -1;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = -1;
    ಪೂರ್ಣಾಂಕ		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    ಪೂರ್ಣಾಂಕ		both;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;
    dict = argvars[0].vval.v_dict;

    ಆದರೆ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	ಆದರೆ (*dir_s == 'b')
	    dir = BACKWARD;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*dir_s != 'f')
	{
	    emsg(_(e_invalid_ವಾದ));
	    ಹಿಂತಿರುಗಿ;
	}
    }

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"lnum", -1);
    ಆದರೆ (di != ಖಾಲಿ)
	lnum = tv_get_ಸಂಖ್ಯೆ(&di->di_tv);

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"col", -1);
    ಆದರೆ (di != ಖಾಲಿ)
	ಲಂಬಸಾಲು = tv_get_ಸಂಖ್ಯೆ(&di->di_tv);

    ಆದರೆ (lnum == -1)
    {
	lnum = cursor->lnum;
	ಲಂಬಸಾಲು = cursor->ಲಂಬಸಾಲು + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಲಂಬಸಾಲು == -1)
	ಲಂಬಸಾಲು = 1;

    ಆದರೆ (lnum < 1 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	emsg(_(e_invalid_range));
	ಹಿಂತಿರುಗಿ;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "id"))
    {
	id = dict_get_ಸಂಖ್ಯೆ(dict, "id");
	id_ಸಿಕ್ಕಿತು = TRUE;
    }
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ಆದರೆ (type == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    ಆದರೆ (!id_ಸಿಕ್ಕಿತು && type_id == -1)
    {
	emsg(_(e_need_at_ಕನಿಷ್ಠ_one_ದ_id_or_type));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (both && (!id_ಸಿಕ್ಕಿತು || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ಹಿಂತಿರುಗಿ;
    }

    lnum_ಆರಂಭ = lnum;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆಗ (1)
    {
	char_u	*prop_ಆರಂಭ_ptr;
	ಪೂರ್ಣಾಂಕ	ಎಣಿಕೆ_2 = get_ಪಠ್ಯ_props(buf, lnum, &prop_ಆರಂಭ_ptr, FALSE);
	ಪೂರ್ಣಾಂಕ	    i;
	textprop_T  prop;
	ಪೂರ್ಣಾಂಕ	    prop_ಆರಂಭ;
	ಪೂರ್ಣಾಂಕ	    prop_ಕೊನೆ;

	ಗಾಗಿ (i = dir == BACKWARD ? ಎಣಿಕೆ_2 - 1 : 0; i >= 0 && i < ಎಣಿಕೆ_2; i += dir)
	{
	    mch_memmove(&prop, prop_ಆರಂಭ_ptr + i * ಗಾತ್ರ(textprop_T),
							   ಗಾತ್ರ(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    ಆದರೆ (prop.tp_id < 0 && prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ > 0)
	    {
		prop.u.tp_ಪಠ್ಯ = (prop_ಆರಂಭ_ptr - PROP_ಎಣಿಕೆ_ಗಾತ್ರ)
						    + prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		prop.u.tp_ಪಠ್ಯ = ಖಾಲಿ;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    ಆದರೆ (lnum == lnum_ಆರಂಭ)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_ಲಂಬಸಾಲು == MAXCOL;
		ಆದರೆ (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    ಆದರೆ (!is_floating_vtext && prop.tp_ಲಂಬಸಾಲು > ಲಂಬಸಾಲು)
			ಮುಂದುವರಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!is_floating_vtext
		    && prop.tp_ಲಂಬಸಾಲು + prop.tp_len - (prop.tp_len != 0) < ಲಂಬಸಾಲು)
		    ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_ಸಿಕ್ಕಿತು && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		ಆದರೆ (lnum_ಆರಂಭ == lnum
			&& ಲಂಬಸಾಲು >= prop.tp_ಲಂಬಸಾಲು
			&& (ಲಂಬಸಾಲು <= prop.tp_ಲಂಬಸಾಲು + prop.tp_len
							 - (prop.tp_len != 0)))
		    ಆರಂಭ_pos_has_prop = 1;

		// The property was not continued from last line, it starts on
		// this line.
		prop_ಆರಂಭ = !(prop.tp_flags & TP_FLAG_CONT_PREV);
		// The property does not continue on the next line, it ends on
		// this line.
		prop_ಕೊನೆ = !(prop.tp_flags & TP_FLAG_CONT_ಮುಂದಿನ);
		ಆದರೆ (!prop_ಆರಂಭ && prop_ಕೊನೆ && dir == FORWARD)
		    ನೋಡಿದ_ಕೊನೆ = 1;

		// Skip lines without the start flag.
		ಆದರೆ (!prop_ಆರಂಭ)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    ಆದರೆ (ಆರಂಭ_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    ಮುಂದುವರಿ;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		ಆದರೆ (ಆರಂಭ_pos_has_prop && skipstart && !ನೋಡಿದ_ಕೊನೆ)
		{
		    ಆರಂಭ_pos_has_prop = 0;
		    ಮುಂದುವರಿ;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "lnum", lnum);

		ಹಿಂತಿರುಗಿ;
	    }
	}

	ಆದರೆ (dir > 0)
	{
	    ಆದರೆ (lnum >= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ನಿಲ್ಲಿಸು;
	    lnum++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (lnum <= 1)
		ನಿಲ್ಲಿಸು;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prop_type_or_id_ಒಳ_list(ಪೂರ್ಣಾಂಕ *types_or_ids, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ type_or_id)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < len; i++)
	ಆದರೆ (types_or_ids[i] == type_or_id)
	    ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ FALSE;
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
get_props_ಒಳ_ಸಾಲು(
	buf_T		*buf,
	linenr_T	lnum,
	ಪೂರ್ಣಾಂಕ		*prop_types,
	ಪೂರ್ಣಾಂಕ		prop_types_len,
	ಪೂರ್ಣಾಂಕ		*prop_ids,
	ಪೂರ್ಣಾಂಕ		prop_ids_len,
	list_T		*retlist,
	ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_lnum)
{
    char_u	*props;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		i;
    textprop_T	prop;

    ಎಣಿಕೆ_2 = get_ಪಠ್ಯ_props(buf, lnum, &props, FALSE);
    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	mch_memmove(&prop, props + i * ಗಾತ್ರ(textprop_T),
		ಗಾತ್ರ(textprop_T));
	// Convert offset to pointer for virtual text props.
	ಆದರೆ (prop.tp_id < 0 && prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ > 0)
	{
	    prop.u.tp_ಪಠ್ಯ = (props - PROP_ಎಣಿಕೆ_ಗಾತ್ರ)
						+ prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    prop.u.tp_ಪಠ್ಯ = ಖಾಲಿ;
	ಆದರೆ ((prop_types == ಖಾಲಿ
		    || prop_type_or_id_ಒಳ_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == ಖಾಲಿ
		    || prop_type_or_id_ಒಳ_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    ಆದರೆ (d == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    prop_fill_dict(d, &prop, buf);
	    ಆದರೆ (ಕೂಡಿಸು_lnum)
		dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "lnum", lnum);
	    ಆದರೆ (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *
get_prop_types_from_ಹೆಸರುಗಳು(list_T *l, buf_T *buf, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_types)
{
    ಪೂರ್ಣಾಂಕ		*prop_types;
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*name;
    proptype_T	*type;

    *ಅಂಕೆ_types = 0;

    prop_types = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, list_len(l));
    ಆದರೆ (prop_types == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    i = 0;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಆದರೆ (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    ಹೋಗು errret;
	}
	name = li->li_tv.vval.v_string;
	ಆದರೆ (name == ಖಾಲಿ)
	    ಹೋಗು errret;

	type = lookup_prop_type(name, buf);
	ಆದರೆ (type == ಖಾಲಿ)
	    ಹೋಗು errret;
	prop_types[i++] = type->pt_id;
    }

    *ಅಂಕೆ_types = i;
    ಹಿಂತಿರುಗಿ prop_types;

errret:
    VIM_CLEAR(prop_types);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *
get_prop_ids_from_list(list_T *l, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_ids)
{
    ಪೂರ್ಣಾಂಕ		*prop_ids;
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		i = 0;
    ಪೂರ್ಣಾಂಕ		id;
    ಪೂರ್ಣಾಂಕ		error;

    *ಅಂಕೆ_ids = 0;

    prop_ids = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, list_len(l));
    ಆದರೆ (prop_ids == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	error = FALSE;
	id = tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, &error);
	ಆದರೆ (error)
	    ಹೋಗು errret;

	prop_ids[i++] = id;
    }

    *ಅಂಕೆ_ids = i;
    ಹಿಂತಿರುಗಿ prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    ಶೂನ್ಯ
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	ಆರಂಭ_lnum;
    linenr_T	ಕೊನೆ_lnum;
    buf_T	*buf = curbuf;
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_lnum = FALSE;
    ಪೂರ್ಣಾಂಕ		*prop_types = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		prop_types_len = 0;
    ಪೂರ್ಣಾಂಕ		*prop_ids = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // default: get text properties on current line
    ಆರಂಭ_lnum = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಕೊನೆ_lnum = ಆರಂಭ_lnum;
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ಆದರೆ (check_for_dict_arg(argvars, 1) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	d = argvars[1].vval.v_dict;

	ಆದರೆ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (d != ಖಾಲಿ && (di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"end_lnum", -1)) != ಖಾಲಿ)
	{
	    ಆದರೆ (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ)
	    {
		emsg(_(e_ಸಂಖ್ಯೆ_required));
		ಹಿಂತಿರುಗಿ;
	    }
	    ಕೊನೆ_lnum = tv_get_ಸಂಖ್ಯೆ(&di->di_tv);
	    ಆದರೆ (ಕೊನೆ_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//ಸಾಲು
		ಕೊನೆ_lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + ಕೊನೆ_lnum + 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೊನೆ_lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಕೊನೆ_lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಕೂಡಿಸು_lnum = TRUE;
	}
	ಆದರೆ (d != ಖಾಲಿ && (di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"types", -1)) != ಖಾಲಿ)
	{
	    ಆದರೆ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ಹಿಂತಿರುಗಿ;
	    }

	    l = di->di_tv.vval.v_list;
	    ಆದರೆ (l != ಖಾಲಿ && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_ಹೆಸರುಗಳು(l, buf, &prop_types_len);
		ಆದರೆ (prop_types == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ;
	    }
	}
	ಆದರೆ (d != ಖಾಲಿ && (di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"ids", -1)) != ಖಾಲಿ)
	{
	    ಆದರೆ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ಹೋಗು errret;
	    }

	    l = di->di_tv.vval.v_list;
	    ಆದರೆ (l != ಖಾಲಿ && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		ಆದರೆ (prop_ids == ಖಾಲಿ)
		    ಹೋಗು errret;
	    }
	}
    }
    ಆದರೆ (ಆರಂಭ_lnum < 1 || ಆರಂಭ_lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		|| ಕೊನೆ_lnum < 1 || ಕೊನೆ_lnum < ಆರಂಭ_lnum)
	emsg(_(e_invalid_range));
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (lnum = ಆರಂಭ_lnum; lnum <= ಕೊನೆ_lnum; lnum++)
	    get_props_ಒಳ_ಸಾಲು(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, ಕೂಡಿಸು_lnum);

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
    linenr_T	ಆರಂಭ = 1;
    linenr_T	ಕೊನೆ = 0;
    linenr_T	lnum;
    linenr_T	ಮೊದಲ_ಬದಲಾಯಿತು = 0;
    linenr_T	ಕೊನೆಯ_ಬದಲಾಯಿತು = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    ಪೂರ್ಣಾಂಕ		do_all;
    ಪೂರ್ಣಾಂಕ		id = -MAXCOL;
    ಪೂರ್ಣಾಂಕ		type_id = -1;	    // for a single "type"
    ಪೂರ್ಣಾಂಕ		*type_ids = ಖಾಲಿ;   // array, for a list of "types", allocated
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_type_ids = 0;   // number of elements in "type_ids"
    ಪೂರ್ಣಾಂಕ		both;

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಆರಂಭ = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	ಕೊನೆ = ಆರಂಭ;
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	    ಕೊನೆ = tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
	ಆದರೆ (ಆರಂಭ < 1 || ಕೊನೆ < 1)
	{
	    emsg(_(e_invalid_range));
	    ಹಿಂತಿರುಗಿ;
	}
    }

    dict = argvars[0].vval.v_dict;
    ಆದರೆ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    do_all = dict_get_bool(dict, "all", FALSE);

    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "id"))
	id = dict_get_ಸಂಖ್ಯೆ(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ಆದರೆ (type == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	type_id = type->pt_id;
    }
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "types"))
    {
	typval_T types;
	listitem_T *li = ಖಾಲಿ;

	dict_get_tv(dict, "types", &types);
	ಆದರೆ (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_ವಸ್ತುಗಳು(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		ಆದರೆ (li->li_tv.v_type != VAR_STRING)
		    ಮುಂದುವರಿ;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		ಆದರೆ (!prop_type)
		    ಹೋಗು cleanup_prop_remove;

		type_ids[ಅಂಕೆ_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    ಆದರೆ (id == -MAXCOL && (type_id == -1 && ಅಂಕೆ_type_ids == 0))
    {
	emsg(_(e_need_at_ಕನಿಷ್ಠ_one_ದ_id_or_type));
	ಹೋಗು cleanup_prop_remove;
    }
    ಆದರೆ (both && (id == -MAXCOL || (type_id == -1 && ಅಂಕೆ_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ಹೋಗು cleanup_prop_remove;
    }
    ಆದರೆ (type_id != -1 && ಅಂಕೆ_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	ಹೋಗು cleanup_prop_remove;
    }

    ಆದರೆ (ಕೊನೆ == 0)
	ಕೊನೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    {
	unpacked_memline_T um = um_open(buf);

	ಗಾಗಿ (lnum = ಆರಂಭ; lnum <= ಕೊನೆ; ++lnum)
	{
	    ಪೂರ್ಣಾಂಕ idx;

	    ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (!um_goto_ಸಾಲು(&um, lnum, 0))
		ನಿಲ್ಲಿಸು;

	    ಗಾಗಿ (idx = 0; idx < um.prop_ಎಣಿಕೆ; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		ಪೂರ್ಣಾಂಕ	    matches_id = 0;
		ಪೂರ್ಣಾಂಕ	    matches_type = 0;

		ಆದರೆ (prop->tp_flags & TP_FLAG_DELETED)
		    ಮುಂದುವರಿ;

		matches_id = prop->tp_id == id;
		ಆದರೆ (ಅಂಕೆ_type_ids > 0)
		{
		    ಪೂರ್ಣಾಂಕ idx2;

		    ಗಾಗಿ (idx2 = 0; !matches_type && idx2 < ಅಂಕೆ_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    matches_type = prop->tp_type == type_id;

		ಆದರೆ (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    ಆದರೆ (ಮೊದಲ_ಬದಲಾಯಿತು == 0)
			ಮೊದಲ_ಬದಲಾಯಿತು = lnum;
		    ಕೊನೆಯ_ಬದಲಾಯಿತು = lnum;
		    ++rettv->vval.v_ಸಂಖ್ಯೆ;
		    ಆದರೆ (!do_all)
			ನಿಲ್ಲಿಸು;
		}
	    }
	}
	um_close(&um);
    }

    ಆದರೆ (ಮೊದಲ_ಬದಲಾಯಿತು > 0)
    {
	ಬದಲಾಯಿತು_ಸಾಲು_display_buf(buf);
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು_buf(buf, ಮೊದಲ_ಬದಲಾಯಿತು, ಕೊನೆಯ_ಬದಲಾಯಿತು + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prop_type_ಗಣ(typval_T *argvars, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು)
{
    char_u	*name;
    buf_T	*buf = ಖಾಲಿ;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);
    ಆದರೆ (*name == NUL)
    {
	semsg(_(e_invalid_ವಾದ_str), "\"\"");
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	ಹಿಂತಿರುಗಿ;
    dict = argvars[1].vval.v_dict;

    prop = ಕಂಡುಹಿಡಿ_prop_type(name, buf);
    ಆದರೆ (ಕೂಡಿಸು)
    {
	hashtab_T **htp;

	ಆದರೆ (prop != ಖಾಲಿ)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    ಹಿಂತಿರುಗಿ;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	ಆದರೆ (prop == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	ಆದರೆ (*htp == ಖಾಲಿ)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    ಆದರೆ (*htp == ಖಾಲಿ)
	    {
		vim_free(prop);
		ಹಿಂತಿರುಗಿ;
	    }
	    hash_init(*htp);
	}
	hash_ಕೂಡಿಸು(*htp, PT2HIKEY(prop), "prop type");
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (prop == ಖಾಲಿ)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಆದರೆ (dict != ಖಾಲಿ)
    {
	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"highlight", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    char_u	*highlight;
	    ಪೂರ್ಣಾಂಕ		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    ಆದರೆ (highlight != ಖಾಲಿ && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    ಆದರೆ (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_ಗುಂಪು_name_str),
			highlight == ಖಾಲಿ ? (char_u *)"" : highlight);
		ಹಿಂತಿರುಗಿ;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"combine", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    ಆದರೆ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    ಇಲ್ಲದಿದ್ದರೆ
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"override", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    ಆದರೆ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    ಇಲ್ಲದಿದ್ದರೆ
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"priority", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	    prop->pt_ಆದ್ಯತೆ = tv_get_ಸಂಖ್ಯೆ(&di->di_tv);

	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"start_incl", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    ಆದರೆ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_ಆರಂಭ_INCL;
	    ಇಲ್ಲದಿದ್ದರೆ
		prop->pt_flags &= ~PT_FLAG_INS_ಆರಂಭ_INCL;
	}

	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"end_incl", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    ಆದರೆ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_ಕೊನೆ_INCL;
	    ಇಲ್ಲದಿದ್ದರೆ
		prop->pt_flags &= ~PT_FLAG_INS_ಕೊನೆ_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    ಶೂನ್ಯ
f_prop_type_ಕೂಡಿಸು(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    prop_type_ಗಣ(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    ಶೂನ್ಯ
f_prop_type_change(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    prop_type_ಗಣ(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    ಶೂನ್ಯ
f_prop_type_delete(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    char_u	*name;
    buf_T	*buf = ಖಾಲಿ;
    hashitem_T	*hi;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);
    ಆದರೆ (*name == NUL)
    {
	semsg(_(e_invalid_ವಾದ_str), "\"\"");
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ಹಿಂತಿರುಗಿ;
    }

    hi = ಕಂಡುಹಿಡಿ_prop_type_hi(name, buf);
    ಆದರೆ (hi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    ಆದರೆ (buf == ಖಾಲಿ)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_buf(buf == ಖಾಲಿ ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    ಶೂನ್ಯ
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);
    ಆದರೆ (*name == NUL)
    {
	semsg(_(e_invalid_ವಾದ_str), "\"\"");
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    proptype_T  *prop = ಖಾಲಿ;
    buf_T	    *buf = ಖಾಲಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ಹಿಂತಿರುಗಿ;
    }

    prop = ಕಂಡುಹಿಡಿ_prop_type(name, buf);
    ಆದರೆ (prop == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    dict_T *d = rettv->vval.v_dict;

    ಆದರೆ (prop->pt_hl_id > 0)
	dict_ಕೂಡಿಸು_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "priority", prop->pt_ಆದ್ಯತೆ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_ಆರಂಭ_INCL) ? 1 : 0);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_ಕೊನೆ_INCL) ? 1 : 0);
    ಆದರೆ (buf != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "bufnr", buf->b_fnum);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_types(hashtab_T *ht, list_T *l)
{
    ದೀರ್ಘ	todo;
    hashitem_T	*hi;

    todo = (ದೀರ್ಘ)ht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
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
    buf_T *buf = ಖಾಲಿ;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	ಆದರೆ (global_proptypes != ಖಾಲಿ)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->b_proptypes != ಖಾಲಿ)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_ht_prop_types(hashtab_T *ht)
{
    ದೀರ್ಘ	todo;
    hashitem_T	*hi;

    ಆದರೆ (ht == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    todo = (ದೀರ್ಘ)ht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#ಆದರೆ defined(EXITFREE)
/*
 * Free all global property types.
 */
    ಶೂನ್ಯ
clear_global_prop_types(ಶೂನ್ಯ)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = ಖಾಲಿ;
    VIM_CLEAR(global_proparray);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Free all property types for "buf".
 */
    ಶೂನ್ಯ
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = ಖಾಲಿ;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ dirty;	    // if the property was changed
    ಪೂರ್ಣಾಂಕ can_drop;   // whether after this change, the prop may be removed
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
	colnr_T	    ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ	    added,
	ಪೂರ್ಣಾಂಕ	    flags)
{
    proptype_T	*pt;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_incl;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_incl;
    ಪೂರ್ಣಾಂಕ		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    ಆದರೆ (prop->tp_ಲಂಬಸಾಲು == MAXCOL)
    {
	res.dirty = FALSE;
	ಹಿಂತಿರುಗಿ res;
    }

    pt = ಪಠ್ಯ_prop_type_by_id(curbuf, prop->tp_type);
    ಆರಂಭ_incl = (pt != ಖಾಲಿ && (pt->pt_flags & PT_FLAG_INS_ಆರಂಭ_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    ಆದರೆ (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	ಆರಂಭ_incl = FALSE;
    ಕೊನೆ_incl = (pt != ಖಾಲಿ && (pt->pt_flags & PT_FLAG_INS_ಕೊನೆ_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_ಮುಂದಿನ);
    // do not drop zero-width props if they later can increase in size
    droppable = !(ಆರಂಭ_incl || ಕೊನೆ_incl);

    ಆದರೆ (added > 0)
    {
	ಆದರೆ (ಲಂಬಸಾಲು + 1 <= prop->tp_ಲಂಬಸಾಲು
		- (ಆರಂಭ_incl || (prop->tp_len == 0 && ಕೊನೆ_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_ಲಂಬಸಾಲು += added;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಲಂಬಸಾಲು + 1 < prop->tp_ಲಂಬಸಾಲು + prop->tp_len + ಕೊನೆ_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prop->tp_ಲಂಬಸಾಲು > ಲಂಬಸಾಲು + 1)
    {
	ಆದರೆ (prop->tp_ಲಂಬಸಾಲು + added < ಲಂಬಸಾಲು + 1)
	{
	    ಆದರೆ (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		prop->tp_len += (prop->tp_ಲಂಬಸಾಲು - 1 - ಲಂಬಸಾಲು) + added;
		prop->tp_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು + 1;
		ಆದರೆ (prop->tp_len <= 0)
		{
		    prop->tp_len = 0;
		    // Multiline properties with no text left should
		    // also be dropped.
		    res.can_drop = droppable
			|| (prop->tp_flags
				& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_ಮುಂದಿನ));
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    prop->tp_ಲಂಬಸಾಲು += added;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prop->tp_len > 0 && prop->tp_ಲಂಬಸಾಲು + prop->tp_len > ಲಂಬಸಾಲು
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	ಆದರೆ (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = ಲಂಬಸಾಲು - (prop->tp_ಲಂಬಸಾಲು - 1);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ after = ಲಂಬಸಾಲು - added - (prop->tp_ಲಂಬಸಾಲು - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	ಆದರೆ (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_ಮುಂದಿನ)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_ಮುಂದಿನ));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	res.dirty = FALSE;

    ಹಿಂತಿರುಗಿ res;
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
    ಪೂರ್ಣಾಂಕ
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ	    ಬೈಟುಗಳು_added,
	ಪೂರ್ಣಾಂಕ	    flags)
{
    unpacked_memline_T um;
    ಪೂರ್ಣಾಂಕ		dirty = FALSE;
    ಪೂರ್ಣಾಂಕ		ri;

    ಆದರೆ (ಪಠ್ಯ_prop_frozen > 0)
	ಹಿಂತಿರುಗಿ FALSE;

    um = um_open_at(curbuf, lnum, 0);
    ಆದರೆ (um.buf == ಖಾಲಿ || um.prop_ಎಣಿಕೆ == 0)
    {
	um_abort(&um);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಗಾಗಿ (ri = 0; ri < um.prop_ಎಣಿಕೆ; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, ಲಂಬಸಾಲು, ಬೈಟುಗಳು_added, flags);
	ಆದರೆ (res.dirty)
	{
	    ಆದರೆ (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		ಆದರೆ (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    ಹಿಂತಿರುಗಿ FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		ಆದರೆ ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    ಹಿಂತಿರುಗಿ FALSE;
		}
	    }
	    dirty = TRUE;
	}
	ಆದರೆ (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    ಮುಂದುವರಿ;
	}
    }
    ಆದರೆ (dirty)
	um_close(&um);
    ಇಲ್ಲದಿದ್ದರೆ
	um_abort(&um);
    ಹಿಂತಿರುಗಿ dirty;
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
adjust_props_for_ವಿಭಜಿಸು(
	linenr_T    lnum_props,
	linenr_T    lnum_ತುದಿ,
	ಪೂರ್ಣಾಂಕ	    kept,
	ಪೂರ್ಣಾಂಕ	    deleted,
	ಪೂರ್ಣಾಂಕ	    at_eol)
{
    char_u	*props;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    garray_T    prevprop;
    garray_T    nextprop;
    ಪೂರ್ಣಾಂಕ		skipped = kept + deleted;

    ಆದರೆ (!curbuf->b_has_textprop)
	ಹಿಂತಿರುಗಿ;

    // Get the text properties from "lnum_props".
    ಎಣಿಕೆ_2 = get_ಪಠ್ಯ_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, ಗಾತ್ರ(textprop_T), 10);
    ga_init2(&nextprop, ಗಾತ್ರ(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *ಎಣಿಕೆ_ptr = props - PROP_ಎಣಿಕೆ_ಗಾತ್ರ;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	ಪೂರ್ಣಾಂಕ	    ಆರಂಭ_incl, ಕೊನೆ_incl;
	ಪೂರ್ಣಾಂಕ	    cont_prev, cont_ಮುಂದಿನ;
	ಪೂರ್ಣಾಂಕ	    prop_ಲಂಬಸಾಲು;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * ಗಾತ್ರ(textprop_T), ಗಾತ್ರ(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	ಆದರೆ (prop.tp_id < 0 && prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ > 0)
	{
	    prop.u.tp_ಪಠ್ಯ = vim_strsave(ಎಣಿಕೆ_ptr + prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    prop.u.tp_ಪಠ್ಯ = ಖಾಲಿ;

	pt = ಪಠ್ಯ_prop_type_by_id(curbuf, prop.tp_type);
	ಆರಂಭ_incl = (pt != ಖಾಲಿ && (pt->pt_flags & PT_FLAG_INS_ಆರಂಭ_INCL));
	ಕೊನೆ_incl = (pt != ಖಾಲಿ && (pt->pt_flags & PT_FLAG_INS_ಕೊನೆ_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_ಲಂಬಸಾಲು = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_ಲಂಬಸಾಲು;

	ಆದರೆ (prop_ಲಂಬಸಾಲು == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_ಮುಂದಿನ = !at_eol;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    ಪೂರ್ಣಾಂಕ is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_ಲಂಬಸಾಲು == MAXCOL);

	    cont_prev = prop_ಲಂಬಸಾಲು + !ಆರಂಭ_incl <= kept;
	    ಆದರೆ (is_floating_vtext)
		cont_ಮುಂದಿನ = FALSE;
	    ಇಲ್ಲದಿದ್ದರೆ
		cont_ಮುಂದಿನ = skipped <= prop_ಲಂಬಸಾಲು + prop.tp_len - !ಕೊನೆ_incl;
	}
	// when a prop has text it is never copied
	ಆದರೆ (prop.tp_id < 0 && cont_ಮುಂದಿನ)
	    cont_prev = FALSE;

	ಆದರೆ (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_ದತ್ತಾಂಶ) + prevprop.ga_len;
	    ಪೂರ್ಣಾಂಕ is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    ಆದರೆ (!is_vtext && p->tp_ಲಂಬಸಾಲು + p->tp_len >= kept)
		p->tp_len = kept - p->tp_ಲಂಬಸಾಲು;
	    ಆದರೆ (cont_ಮುಂದಿನ)
		p->tp_flags |= TP_FLAG_CONT_ಮುಂದಿನ;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	ಆದರೆ (cont_ಮುಂದಿನ && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_ದತ್ತಾಂಶ) + nextprop.ga_len;
	    ಪೂರ್ಣಾಂಕ is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    ಆದರೆ (p->tp_ಲಂಬಸಾಲು != MAXCOL)
	    {
		ಆದರೆ (p->tp_ಲಂಬಸಾಲು > skipped)
		    p->tp_ಲಂಬಸಾಲು -= skipped - 1;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (!is_vtext)
			p->tp_len -= skipped - p->tp_ಲಂಬಸಾಲು;
		    p->tp_ಲಂಬಸಾಲು = 1;
		}
	    }
	    ಆದರೆ (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    ಗಣ_ಪಠ್ಯ_props(lnum_ತುದಿ, (textprop_T *)prevprop.ga_ದತ್ತಾಂಶ,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    ಗಣ_ಪಠ್ಯ_props(lnum_ತುದಿ + 1, (textprop_T *)nextprop.ga_ದತ್ತಾಂಶ,
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
	ಪೂರ್ಣಾಂಕ		    ಕೊನೆಯ_ಸಾಲು,
	ದೀರ್ಘ		    ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ		    removed)
{
    unpacked_memline_T	r_um;

    ಆದರೆ (um->buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    r_um = um_open_at(um->buf, lnum, 0);
    ಆದರೆ (r_um.buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!um_detach(&r_um))
    {
	um_abort(&r_um);
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = r_um.prop_ಎಣಿಕೆ - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	ಪೂರ್ಣಾಂಕ	    ಕೊನೆ;

	ಆದರೆ (prop->tp_ಲಂಬಸಾಲು == MAXCOL && !ಕೊನೆಯ_ಸಾಲು)
	    ಮುಂದುವರಿ;  // drop floating text for non-last lines
	ಕೊನೆ = !(prop->tp_flags & TP_FLAG_CONT_ಮುಂದಿನ);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, ಲಂಬಸಾಲು, 0);

	ಆದರೆ (ಕೊನೆಯ_ಸಾಲು || ಕೊನೆ)
	{
	    um_ಕೂಡಿಸು_prop(um, prop);
	    prop->u.tp_ಪಠ್ಯ = ಖಾಲಿ;  // ownership transferred
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Search for continuing prop in um.
	    bool    ಸಿಕ್ಕಿತು = false;

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < um->prop_ಎಣಿಕೆ; ++j)
	    {
		textprop_T *op = &um->props[j];

		ಆದರೆ ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    ಸಿಕ್ಕಿತು = true;
		    op->tp_len += op->tp_ಲಂಬಸಾಲು - prop->tp_ಲಂಬಸಾಲು;
		    op->tp_ಲಂಬಸಾಲು = prop->tp_ಲಂಬಸಾಲು;
		    op->tp_flags = prop->tp_flags;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಆದರೆ (!ಸಿಕ್ಕಿತು)
		ಆಂತರಿಕ_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remember_cleared_prop(garray_T *gap, ಪೂರ್ಣಾಂಕ which, textprop_T *prop, ಪೂರ್ಣಾಂಕ flag)
{
    undoprop_T	*ಮೇಲೆ;

    ಆದರೆ (which == UNDOPROP_NONE)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (gap->ga_itemsize == 0)
	ga_init2(gap, ಗಾತ್ರ(undoprop_T), 4);
    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಮೇಲೆ = (undoprop_T *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++;
    ಮೇಲೆ->ಮೇಲೆ_which = which;
    ಮೇಲೆ->ಮೇಲೆ_type = prop->tp_type;
    ಮೇಲೆ->ಮೇಲೆ_id = prop->tp_id;
    ಮೇಲೆ->ಮೇಲೆ_ಲಂಬಸಾಲು = prop->tp_ಲಂಬಸಾಲು;
    ಮೇಲೆ->ಮೇಲೆ_flag = flag;
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
	ಪೂರ್ಣಾಂಕ	    above_which,
	ಪೂರ್ಣಾಂಕ	    ಕೆಳಗೆ_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	ಕೆಳಗೆ;
    bool		*paired = ಖಾಲಿ;
    bool		above_ಬದಲಾಯಿತು = false;
    bool		ಕೆಳಗೆ_ಬದಲಾಯಿತು = false;
    bool		cont_ಮುಂದಿನ = false;

    ಆದರೆ (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;

    above = um_open_at(buf, lnum, 0);
    ಆದರೆ (above.buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < above.prop_ಎಣಿಕೆ; ++i)
	ಆದರೆ (above.props[i].tp_flags & TP_FLAG_CONT_ಮುಂದಿನ)
	    cont_ಮುಂದಿನ = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    ಆದರೆ (cont_ಮುಂದಿನ && !um_detach(&above))
    {
	um_abort(&above);
	ಹಿಂತಿರುಗಿ;
    }

    ಕೆಳಗೆ = um_open_at(buf, lnum + 1, 0);
    ಆದರೆ (ಕೆಳಗೆ.buf == ಖಾಲಿ)
    {
	um_abort(&above);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಕೆಳಗೆ.prop_ಎಣಿಕೆ > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, ಕೆಳಗೆ.prop_ಎಣಿಕೆ);
	ಆದರೆ (paired == ಖಾಲಿ)
	    ಹೋಗು theend;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < above.prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    ಸಿಕ್ಕಿತು = false;

	ಆದರೆ (!(prop->tp_flags & TP_FLAG_CONT_ಮುಂದಿನ))
	    ಮುಂದುವರಿ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಕೆಳಗೆ.prop_ಎಣಿಕೆ; ++j)
	{
	    textprop_T *ಮುಂದಿನ = &ಕೆಳಗೆ.props[j];

	    ಆದರೆ (!paired[j] && (ಮುಂದಿನ->tp_flags & TP_FLAG_CONT_PREV)
		    && ಮುಂದಿನ->tp_id == prop->tp_id
		    && ಮುಂದಿನ->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		ಸಿಕ್ಕಿತು = true;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (!ಸಿಕ್ಕಿತು)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_ಮುಂದಿನ;
	    above_ಬದಲಾಯಿತು = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_ಮುಂದಿನ);
	}
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಕೆಳಗೆ.prop_ಎಣಿಕೆ; ++j)
    {
	ಆದರೆ (!(ಕೆಳಗೆ.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    ಮುಂದುವರಿ;
	ಆದರೆ (!ಕೆಳಗೆ.detached && !um_detach(&ಕೆಳಗೆ))
	    ನಿಲ್ಲಿಸು;
	ಕೆಳಗೆ.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	ಕೆಳಗೆ_ಬದಲಾಯಿತು = true;
	remember_cleared_prop(gap, ಕೆಳಗೆ_which, &ಕೆಳಗೆ.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    ಆದರೆ (ಕೆಳಗೆ_ಬದಲಾಯಿತು)
	um_close(&ಕೆಳಗೆ);
    ಇಲ್ಲದಿದ್ದರೆ
	um_abort(&ಕೆಳಗೆ);
    ಆದರೆ (above_ಬದಲಾಯಿತು)
	um_close(&above);
    ಇಲ್ಲದಿದ್ದರೆ
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
	ಪೂರ್ಣಾಂಕ	    which)
{
    unpacked_memline_T	um;
    bool		ಬದಲಾಯಿತು = false;

    ಆದರೆ (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;
    um = um_open_at(buf, lnum, 0);
    ಆದರೆ (um.buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *ಮೇಲೆ = (undoprop_T *)gap->ga_ದತ್ತಾಂಶ + r;

	ಆದರೆ (ಮೇಲೆ->ಮೇಲೆ_which != which)
	    ಮುಂದುವರಿ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < um.prop_ಎಣಿಕೆ; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    ಆದರೆ (prop->tp_type != ಮೇಲೆ->ಮೇಲೆ_type || prop->tp_id != ಮೇಲೆ->ಮೇಲೆ_id
		    || prop->tp_ಲಂಬಸಾಲು != ಮೇಲೆ->ಮೇಲೆ_ಲಂಬಸಾಲು
		    || (prop->tp_flags & ಮೇಲೆ->ಮೇಲೆ_flag))
		ಮುಂದುವರಿ;
	    ಆದರೆ (!um.detached && !um_detach(&um))
		ಹೋಗು theend;
	    um.props[i].tp_flags |= ಮೇಲೆ->ಮೇಲೆ_flag;
	    ಬದಲಾಯಿತು = true;
	    ನಿಲ್ಲಿಸು;
	}
    }

theend:
    ಆದರೆ (ಬದಲಾಯಿತು)
	um_close(&um);
    ಇಲ್ಲದಿದ್ದರೆ
	um_abort(&um);
}

    bool
ಪಠ್ಯ_prop_ಎಣಿಕೆ_valid(ಪೂರ್ಣಾಂಕ prop_ಎಣಿಕೆ, size_t propdata_len)
{
    ಆದರೆ (propdata_len < PROP_ಎಣಿಕೆ_ಗಾತ್ರ)
	ಹಿಂತಿರುಗಿ false;
    ಹಿಂತಿರುಗಿ (size_t)prop_ಎಣಿಕೆ * ಗಾತ್ರ(textprop_T)
		    <= propdata_len - PROP_ಎಣಿಕೆ_ಗಾತ್ರ;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
ಪಠ್ಯ_prop_vtext_valid(char_u *props, ಪೂರ್ಣಾಂಕ prop_ಎಣಿಕೆ, size_t propdata_len)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < prop_ಎಣಿಕೆ; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * ಗಾತ್ರ(textprop_T),
							  ಗಾತ್ರ(textprop_T));
	ಆದರೆ (prop.tp_id >= 0 || prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ <= 0)
	    ಮುಂದುವರಿ;
	ಆದರೆ (prop.tp_len < 0 || (size_t)prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    ಹಿಂತಿರುಗಿ false;
    }
    ಹಿಂತಿರುಗಿ true;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_PROP_POPUP
