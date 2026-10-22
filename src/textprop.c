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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_PROP_POPUP)

స్థిర శూన్య um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    స్థిర శూన్య
um_free_vtext(textprop_T *props, పూర్ణాంకం లెక్క)
{
    కొరకు (పూర్ణాంకం i = 0; i < లెక్క; ++i)
	ఒకవేళ (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_పాఠ్యం);
}

/*
 * Free memory used by an unpacked memline.
 */
    స్థిర శూన్య
um_free(unpacked_memline_T *um)
{
    ఒకవేళ (um->detached)
    {
	VIM_CLEAR(um->పాఠ్యం);
	um_free_vtext(um->props, um->prop_లెక్క);
    }
    VIM_CLEAR(um->props);
    um->prop_కొలత = 0;
    um->prop_లెక్క = 0;
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

    CLEAR_క్షేత్రం(um);
    um.buf = buf;
    తిరిగి um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_పంక్తి(unpacked_memline_T *um, linenr_T lnum, పూర్ణాంకం extra_props)
{
    char_u	*పంక్తి;
    size_t	textlen;
    size_t	propdata_len;
    పూర్ణాంకం		proplen;

    ఒకవేళ (um->buf == ఖాళీ)
	తిరిగి false;

    // Store changes to the current line if any.
    ఒకవేళ (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    ఒకవేళ (lnum == 0)
	తిరిగి true;	// just unload

    పంక్తి = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_పంక్తి_len - textlen;

    um->lnum = lnum;
    um->పాఠ్యం_కొలత_2 = (colnr_T)textlen;
    um->పాఠ్యం = పంక్తి;

    ఒకవేళ (propdata_len == 0)
	తిరిగి true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    ఒకవేళ (propdata_len < PROP_లెక్క_కొలత + పరిమాణం(textprop_T))
    {
	emsg(e_పాఠ్యం_property_info_corrupted);
	um->buf = ఖాళీ;
	తిరిగి false;
    }

    uint16_t    prop_లెక్క;
    char_u	    *లెక్క_ptr = పంక్తి + textlen;
    char_u	    *props_ప్రారంభం;

    mch_memmove(&prop_లెక్క, లెక్క_ptr, PROP_లెక్క_కొలత);
    ఒకవేళ (!పాఠ్యం_prop_లెక్క_valid(prop_లెక్క, propdata_len))
    {
	emsg(e_పాఠ్యం_property_info_corrupted);
	um->buf = ఖాళీ;
	తిరిగి false;
    }
    proplen = (పూర్ణాంకం)prop_లెక్క;
    props_ప్రారంభం = లెక్క_ptr + PROP_లెక్క_కొలత;

    ఒకవేళ (!పాఠ్యం_prop_vtext_valid(props_ప్రారంభం, proplen, propdata_len))
    {
	emsg(e_పాఠ్యం_property_info_corrupted);
	um->buf = ఖాళీ;
	తిరిగి false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    ఒకవేళ (um->props == ఖాళీ)
    {
	um->buf = ఖాళీ;
	తిరిగి false;
    }
    um->prop_కొలత = proplen + extra_props;
    um->prop_లెక్క = proplen;
    mch_memmove(um->props, props_ప్రారంభం, proplen * పరిమాణం(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    కొరకు (పూర్ణాంకం i = 0; i < proplen; ++i)
    {
	ఒకవేళ (um->props[i].tp_id < 0 && um->props[i].u.tp_పాఠ్యం_స్థానభ్రంశం > 0)
	{
	    um->props[i].u.tp_పాఠ్యం = లెక్క_ptr + um->props[i].u.tp_పాఠ్యం_స్థానభ్రంశం;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	లేకపోతే
	    um->props[i].u.tp_పాఠ్యం = ఖాళీ;
    }

    తిరిగి true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, పూర్ణాంకం extra_props)
{
    unpacked_memline_T um = um_open(buf);

    ఒకవేళ (!um_goto_పంక్తి(&um, lnum, extra_props))
	um.buf = ఖాళీ;
    తిరిగి um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    స్థిర bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    ఒకవేళ (um->detached || um->buf == ఖాళీ)
	తిరిగి um->detached;

    newtext = vim_strnsave(um->పాఠ్యం, um->పాఠ్యం_కొలత_2 - 1);
    ఒకవేళ (newtext == ఖాళీ)
    {
	um->buf = ఖాళీ;
	తిరిగి false;
    }
    um->పాఠ్యం = newtext;

    కొరకు (పూర్ణాంకం i = 0; i < um->prop_లెక్క; ++i)
    {
	ఒకవేళ (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_పాఠ్యం);

	    ఒకవేళ (copy == ఖాళీ)
	    {
		కొరకు (పూర్ణాంకం j = 0; j < i; ++j)
		    ఒకవేళ (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_పాఠ్యం);
		VIM_CLEAR(um->పాఠ్యం);
		um->buf = ఖాళీ;
		తిరిగి false;
	    }
	    um->props[i].u.tp_పాఠ్యం = copy;
	}
    }

    um->detached = true;
    తిరిగి true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_సమితి_పాఠ్యం(unpacked_memline_T *um, char_u *పాఠ్యం)
{
    ఒకవేళ (um->buf == ఖాళీ)
	తిరిగి false;
    ఒకవేళ (um->detached)
	vim_free(um->పాఠ్యం);
    um->పాఠ్యం = పాఠ్యం;
    um->పాఠ్యం_కొలత_2 = (colnr_T)STRLEN(పాఠ్యం) + 1;
    um->పాఠ్యం_మారింది = true;
    um->detached = true;
    తిరిగి true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    స్థిర bool
um_grow_props(unpacked_memline_T *um, పూర్ణాంకం needed)
{
    ఒకవేళ (um->prop_లెక్క + needed <= um->prop_కొలత)
	తిరిగి true;

    పూర్ణాంకం	    new_కొలత = um->prop_లెక్క + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_కొలత * పరిమాణం(textprop_T));
    ఒకవేళ (newprops == ఖాళీ)
	తిరిగి false;
    um->props = newprops;
    um->prop_కొలత = new_కొలత;

    తిరిగి true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    స్థిర శూన్య
um_కూడు_prop(unpacked_memline_T *um, textprop_T *prop)
{
    ఒకవేళ (um->buf == ఖాళీ)
	తిరిగి;
    ఒకవేళ (!um->detached && !um_detach(um))
	తిరిగి;
    ఒకవేళ (!um_grow_props(um, 1))
    {
	um->buf = ఖాళీ;
	తిరిగి;
    }
    um->props[um->prop_లెక్క++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    శూన్య
um_తిరగేయి_props(unpacked_memline_T *um)
{
    కొరకు (పూర్ణాంకం i = 0, j = um->prop_లెక్క - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, పూర్ణాంకం prop_లెక్క)
{
    unpacked_memline_T um;

    CLEAR_క్షేత్రం(um);
    um.buf = buf;

    ఒకవేళ (lnum < 1 || lnum > buf->b_ml.ml_పంక్తి_లెక్క)
    {
	um.buf = ఖాళీ;
	తిరిగి um;
    }

    char_u *పంక్తి = ml_get_buf(buf, lnum, FALSE);
    char_u *పాఠ్యం_copy = vim_strsave(పంక్తి);

    ఒకవేళ (పాఠ్యం_copy == ఖాళీ)
    {
	um.buf = ఖాళీ;
	తిరిగి um;
    }
    um.lnum = lnum;
    um.పాఠ్యం = పాఠ్యం_copy;
    um.పాఠ్యం_కొలత_2 = (colnr_T)STRLEN(పాఠ్యం_copy) + 1;
    um.detached = true;

    ఒకవేళ (prop_లెక్క > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_లెక్క);
	ఒకవేళ (um.props == ఖాళీ)
	{
	    vim_free(um.పాఠ్యం);
	    um.buf = ఖాళీ;
	    తిరిగి um;
	}
	um.prop_కొలత = prop_లెక్క;
    }

    తిరిగి um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    స్థిర శూన్య
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					పూర్ణాంకం tp_id, పూర్ణాంకం tp_type, పూర్ణాంకం flag)
{
    unpacked_memline_T	neighbor;

    ఒకవేళ (lnum < 1 || lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి;

    neighbor = um_open_at(buf, lnum, 0);
    ఒకవేళ (neighbor.buf == ఖాళీ)
	తిరిగి;

    కొరకు (పూర్ణాంకం i = 0; i < neighbor.prop_లెక్క; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	ఒకవేళ (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    ఒకవేళ (!neighbor.detached && !um_detach(&neighbor))
		ఆపు;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    ఆపు;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    శూన్య
um_delete_prop(unpacked_memline_T *um, పూర్ణాంకం సూచీ)
{
    textprop_T *prop;

    ఒకవేళ (um->buf == ఖాళీ || సూచీ < 0 || సూచీ >= um->prop_లెక్క)
	తిరిగి;
    ఒకవేళ (!um->detached && !um_detach(um))
	తిరిగి;

    prop = &um->props[సూచీ];

    // Adjust continuation flags on neighboring lines before deleting.
    ఒకవేళ (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_తదుపరి);
    ఒకవేళ (prop->tp_flags & TP_FLAG_CONT_తదుపరి)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    ఒకవేళ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	VIM_CLEAR(prop->u.tp_పాఠ్యం);
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
    స్థిర char_u *
um_pack(unpacked_memline_T *um, పూర్ణాంకం *packed_len)
{
    uint16_t	సజీవ_లెక్క = 0;
    పూర్ణాంకం		vtext_కొలత = 0;
    పూర్ణాంకం		మొత్తం_len;
    char_u	*buf;
    char_u	*లెక్క_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    కొరకు (పూర్ణాంకం i = 0; i < um->prop_లెక్క; ++i)
    {
	textprop_T *prop = &um->props[i];

	ఒకవేళ (prop->tp_flags & TP_FLAG_DELETED)
	    కొనసాగు;
	++సజీవ_లెక్క;
	ఒకవేళ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_కొలత += prop->tp_len + 1;
    }

    ఒకవేళ (సజీవ_లెక్క == 0)
    {
	// No properties: just text, no prop_count header.
	మొత్తం_len = um->పాఠ్యం_కొలత_2;
	buf = alloc(మొత్తం_len);
	ఒకవేళ (buf == ఖాళీ)
	    తిరిగి ఖాళీ;
	*packed_len = మొత్తం_len;
	mch_memmove(buf, um->పాఠ్యం, um->పాఠ్యం_కొలత_2);
	తిరిగి buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    మొత్తం_len = um->పాఠ్యం_కొలత_2 + (పూర్ణాంకం)PROP_లెక్క_కొలత
		+ సజీవ_లెక్క * (పూర్ణాంకం)పరిమాణం(textprop_T) + vtext_కొలత;
    buf = alloc(మొత్తం_len);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి ఖాళీ;
    *packed_len = మొత్తం_len;

    mch_memmove(buf, um->పాఠ్యం, um->పాఠ్యం_కొలత_2);

    లెక్క_dest = buf + um->పాఠ్యం_కొలత_2;
    mch_memmove(లెక్క_dest, &సజీవ_లెక్క, PROP_లెక్క_కొలత);

    prop_dest = లెక్క_dest + PROP_లెక్క_కొలత;
    vtext_dest = prop_dest + సజీవ_లెక్క * పరిమాణం(textprop_T);

    కొరకు (పూర్ణాంకం i = 0; i < um->prop_లెక్క; ++i)
    {
	textprop_T prop = um->props[i];

	ఒకవేళ (prop.tp_flags & TP_FLAG_DELETED)
	    కొనసాగు;

	ఒకవేళ (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    పూర్ణాంకం copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_పాఠ్యం, copysize);
	    prop.u.tp_పాఠ్యం_స్థానభ్రంశం = (colnr_T)(vtext_dest - లెక్క_dest);
	    vtext_dest += copysize;
	}
	లేకపోతే
	    prop.u.tp_పాఠ్యం_స్థానభ్రంశం = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, పరిమాణం(textprop_T));
	prop_dest += పరిమాణం(textprop_T);
    }

    తిరిగి buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    స్థిర శూన్య
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    పూర్ణాంకం	    packed_len;

    ఒకవేళ (!um->detached || um->buf == ఖాళీ || um->lnum == 0)
	తిరిగి;

    packed = um_pack(um, &packed_len);
    ఒకవేళ (packed == ఖాళీ)
    {
	um->buf = ఖాళీ;
	తిరిగి;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    ఒకవేళ (um->buf->b_ml.ml_పంక్తి_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    లేకపోతే ఒకవేళ (!(um->buf->b_ml.ml_flags & ML_పంక్తి_DIRTY))
	(శూన్య)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->పాఠ్యం);
    um_free_vtext(um->props, um->prop_లెక్క);

    ఒకవేళ (um->buf->b_ml.ml_flags & (ML_పంక్తి_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_పంక్తి_ptr);
    um->buf->b_ml.ml_పంక్తి_ptr = packed;
    um->buf->b_ml.ml_పంక్తి_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_పంక్తి_DIRTY;

    um->detached = false;
    um->పాఠ్యం = packed;
    um->పాఠ్యం_కొలత_2 = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_పంక్తి_textlen = um->పాఠ్యం_కొలత_2;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    శూన్య
um_close(unpacked_memline_T *um)
{
    ఒకవేళ (um->buf == ఖాళీ)
	తిరిగి;
    ఒకవేళ (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = ఖాళీ;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    శూన్య
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = ఖాళీ;
}

/*
 * In a hashtable item "hi_key" points to "pt_name" in a proptype_T.
 * This avoids adding a pointer to the hashtable item.
 * PT2HIKEY() converts a proptype pointer to a hashitem key pointer.
 * HIKEY2PT() converts a hashitem key pointer to a proptype pointer.
 * HI2PT() converts a hashitem pointer to a proptype pointer.
 */
#నిర్వచించు PT2HIKEY(p)  ((p)->pt_name)
#నిర్వచించు HIKEY2PT(p)   ((proptype_T *)((p) - offsetof(proptype_T, pt_name)))
#నిర్వచించు HI2PT(hi)      HIKEY2PT((hi)->hi_కీలకం)

// The global text property types.
స్థిర hashtab_T *global_proptypes = ఖాళీ;
స్థిర proptype_T **global_proparray = ఖాళీ;

// The last used text property type ID.
స్థిర పూర్ణాంకం proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    స్థిర hashitem_T *
కనుగొను_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    ఒకవేళ (*name == NUL)
	తిరిగి ఖాళీ;
    ఒకవేళ (buf == ఖాళీ)
	ht = global_proptypes;
    లేకపోతే
	ht = buf->b_proptypes;

    ఒకవేళ (ht == ఖాళీ)
	తిరిగి ఖాళీ;
    hi = hash_కనుగొను(ht, name);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	తిరిగి ఖాళీ;
    తిరిగి hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    స్థిర proptype_T *
కనుగొను_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = కనుగొను_prop_type_hi(name, buf);

    ఒకవేళ (hi == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    పూర్ణాంకం
కనుగొను_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = కనుగొను_prop_type(name, buf);

    ఒకవేళ (pt == ఖాళీ)
	తిరిగి 0;
    తిరిగి pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    స్థిర proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = కనుగొను_prop_type(name, buf);

    ఒకవేళ (type == ఖాళీ)
	type = కనుగొను_prop_type(name, ఖాళీ);
    ఒకవేళ (type == ఖాళీ)
	semsg(_(e_property_type_str_does_not_exist), name);
    తిరిగి type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    స్థిర పూర్ణాంకం
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    ఒకవేళ (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	తిరిగి FAIL;
    }
    ఒకవేళ (arg->vval.v_dict == ఖాళీ)
	తిరిగి OK;  // NULL dict is like an empty dict
    di = dict_కనుగొను(arg->vval.v_dict, (char_u *)"bufnr", -1);
    ఒకవేళ (di != ఖాళీ && (di->di_tv.v_type != VAR_సంఖ్య
					      || di->di_tv.vval.v_సంఖ్య != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	ఒకవేళ (*buf == ఖాళీ)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    శూన్య
f_prop_కూడు(typval_T *argvars, typval_T *rettv)
{
    linenr_T	ప్రారంభం_lnum;
    colnr_T	ప్రారంభం_నిలువు;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	తిరిగి;

    ప్రారంభం_lnum = tv_get_సంఖ్య(&argvars[0]);
    ప్రారంభం_నిలువు = tv_get_సంఖ్య(&argvars[1]);
    ఒకవేళ (check_for_dict_arg(argvars, 2) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = prop_కూడు_common(ప్రారంభం_lnum, ప్రారంభం_నిలువు,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    స్థిర పూర్ణాంకం
prop_కూడు_one(
	buf_T		*buf,
	char_u		*type_name,
	పూర్ణాంకం		id,
	char_u		*పాఠ్యం_arg,
	పూర్ణాంకం		పాఠ్యం_padding_ఎడమ,
	పూర్ణాంకం		పాఠ్యం_flags,
	linenr_T	ప్రారంభం_lnum,
	linenr_T	చివర_lnum,
	colnr_T		ప్రారంభం_నిలువు,
	colnr_T		చివర_నిలువు)
{
    proptype_T	*type;
    linenr_T	lnum;
    పూర్ణాంకం		proplen;
    char_u	*props = ఖాళీ;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    పూర్ణాంకం		i;
    textprop_T	tmp_prop;
    char_u	*పాఠ్యం = పాఠ్యం_arg;
    పూర్ణాంకం		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    ఒకవేళ (type == ఖాళీ)
	వెళ్ళు theend;

    ఒకవేళ (ప్రారంభం_lnum < 1 || ప్రారంభం_lnum > buf->b_ml.ml_పంక్తి_లెక్క)
    {
	semsg(_(e_invalid_పంక్తి_సంఖ్య_nr), (దీర్ఘ)ప్రారంభం_lnum);
	వెళ్ళు theend;
    }
    ఒకవేళ (చివర_lnum < ప్రారంభం_lnum || చివర_lnum > buf->b_ml.ml_పంక్తి_లెక్క)
    {
	semsg(_(e_invalid_పంక్తి_సంఖ్య_nr), (దీర్ఘ)చివర_lnum);
	వెళ్ళు theend;
    }

    ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)
    {
	emsg(_(e_cannot_కూడు_పాఠ్యం_property_గమ్యం_unloaded_బఫరు));
	వెళ్ళు theend;
    }

    ఒకవేళ (పాఠ్యం != ఖాళీ)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	కొరకు (p = పాఠ్యం; *p != NUL; MB_PTR_ADV(p))
	    ఒకవేళ (*p < ' ')
		*p = ' ';
    }

    కొరకు (lnum = ప్రారంభం_lnum; lnum <= చివర_lnum; ++lnum)
    {
	colnr_T నిలువు;	    // start column use in tp_col
	colnr_T క్రమీకరించు_నిలువు;   // column where it appears
	దీర్ఘ	నిడివి;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_పాఠ్యం_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	ఒకవేళ (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_పాఠ్యం_properties_on_a_single_పంక్తి));
	    వెళ్ళు theend;
	}

	ఒకవేళ (lnum == ప్రారంభం_lnum)
	    నిలువు = ప్రారంభం_నిలువు;
	లేకపోతే
	    నిలువు = 1;
	ఒకవేళ (నిలువు - 1 > (colnr_T)textlen && !(నిలువు == 0 && పాఠ్యం_arg != ఖాళీ))
	{
	    semsg(_(e_invalid_స్తంభం_సంఖ్య_nr), (దీర్ఘ)ప్రారంభం_నిలువు);
	    వెళ్ళు theend;
	}
	క్రమీకరించు_నిలువు = నిలువు;

	ఒకవేళ (lnum == చివర_lnum)
	    నిడివి = చివర_నిలువు - నిలువు;
	లేకపోతే
	    నిడివి = (పూర్ణాంకం)textlen - నిలువు + 1;
	ఒకవేళ (నిడివి > (దీర్ఘ)textlen)
	    నిడివి = (పూర్ణాంకం)textlen;	// can include the end-of-line
	ఒకవేళ (నిడివి < 0)
	    నిడివి = 0;		// zero-width property

	ఒకవేళ (పాఠ్యం_arg != ఖాళీ)
	{
	    నిడివి = (దీర్ఘ)STRLEN(పాఠ్యం_arg);
	    ఒకవేళ (నిలువు == 0)
	    {
		నిలువు = MAXCOL;	// before or after the line
		ఒకవేళ ((పాఠ్యం_flags & TP_FLAG_ALIGN_ABOVE) == 0)
		    క్రమీకరించు_నిలువు = MAXCOL;
	    }
	}

	// Build the new property.
	CLEAR_క్షేత్రం(tmp_prop);
	tmp_prop.tp_నిలువు = నిలువు;
	tmp_prop.tp_len = నిడివి;
	tmp_prop.tp_id = id;
	tmp_prop.tp_type = type->pt_id;
	tmp_prop.tp_flags = పాఠ్యం_flags
			    | (lnum > ప్రారంభం_lnum ? TP_FLAG_CONT_PREV : 0)
			    | (lnum < చివర_lnum ? TP_FLAG_CONT_తదుపరి : 0)
			    | ((type->pt_flags & PT_FLAG_INS_ప్రారంభం_INCL)
						     ? TP_FLAG_ప్రారంభం_INCL : 0);
	tmp_prop.tp_padleft = పాఠ్యం_padding_ఎడమ;
	ఒకవేళ (పాఠ్యం_arg != ఖాళీ)
	    tmp_prop.u.tp_పాఠ్యం = పాఠ్యం_arg;

	// Find the index where to insert the new property.
	కొరకు (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_నిలువు;

	    mch_memmove(&existing, props + i * పరిమాణం(textprop_T),
							   పరిమాణం(textprop_T));
	    prop_నిలువు = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_నిలువు;
	    ఒకవేళ (prop_నిలువు >= క్రమీకరించు_నిలువు)
		ఆపు;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	పూర్ణాంకం		vtext_మొత్తం = 0;
	పూర్ణాంకం		new_పంక్తి_len;
	char_u	*లెక్క_dest;
	char_u	*vtext_dest;
	పూర్ణాంకం		j;

	// Compute total vtext size from existing props.
	కొరకు (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * పరిమాణం(textprop_T),
							పరిమాణం(textprop_T));
	    ఒకవేళ (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_మొత్తం += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	ఒకవేళ (పాఠ్యం_arg != ఖాళీ)
	    vtext_మొత్తం += నిడివి + 1;

	new_పంక్తి_len = (పూర్ణాంకం)textlen + (పూర్ణాంకం)PROP_లెక్క_కొలత
			+ new_propcount * (పూర్ణాంకం)పరిమాణం(textprop_T)
			+ vtext_మొత్తం;
	newtext = alloc(new_పంక్తి_len);
	ఒకవేళ (newtext == ఖాళీ)
	    వెళ్ళు theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_పంక్తి_ptr, textlen);

	// Write prop_count.
	లెక్క_dest = newtext + textlen;
	mch_memmove(లెక్క_dest, &new_propcount, PROP_లెక్క_కొలత);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = లెక్క_dest + PROP_లెక్క_కొలత;
	ఒకవేళ (i > 0)
	    mch_memmove(newprops, props, పరిమాణం(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	ఒకవేళ (i < proplen)
	    mch_memmove(newprops + (i + 1) * పరిమాణం(textprop_T),
					props + i * పరిమాణం(textprop_T),
					పరిమాణం(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * పరిమాణం(textprop_T);
	కొరకు (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    ఒకవేళ (j == i)
		కొనసాగు;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * పరిమాణం(textprop_T),
		    పరిమాణం(textprop_T));
	    ఒకవేళ (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_లెక్క_2 = (char_u *)props - PROP_లెక్క_కొలత;
		char_u *old_vtext = old_లెక్క_2 + ep.u.tp_పాఠ్యం_స్థానభ్రంశం;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_పాఠ్యం_స్థానభ్రంశం = (colnr_T)(vtext_dest - లెక్క_dest);
		mch_memmove(newprops + j * పరిమాణం(textprop_T), &ep,
			పరిమాణం(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	ఒకవేళ (పాఠ్యం_arg != ఖాళీ)
	{
	    పూర్ణాంకం copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, పాఠ్యం_arg, copysize);
	    tmp_prop.u.tp_పాఠ్యం_స్థానభ్రంశం =
				    (colnr_T)(vtext_dest - లెక్క_dest);
	    vtext_dest += copysize;
	}
	లేకపోతే
	    tmp_prop.u.tp_పాఠ్యం_స్థానభ్రంశం = 0;

	mch_memmove(newprops + i * పరిమాణం(textprop_T), &tmp_prop,
							పరిమాణం(textprop_T));

	ఒకవేళ (buf->b_ml.ml_flags & (ML_పంక్తి_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_పంక్తి_ptr);
	buf->b_ml.ml_పంక్తి_ptr = newtext;
	buf->b_ml.ml_పంక్తి_len = new_పంక్తి_len;
	buf->b_ml.ml_flags |= ML_పంక్తి_DIRTY;
    }

    మారింది_పంక్తి_display_buf(buf);
    మారింది_పంక్తులు_buf(buf, ప్రారంభం_lnum, చివర_lnum + 1, 0);
    res = OK;

theend:
    vim_free(పాఠ్యం);
    తిరిగి res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    శూన్య
f_prop_కూడు_list(typval_T *argvars, typval_T *rettv వాడని)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    పూర్ణాంకం		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	ప్రారంభం_lnum;
    colnr_T	ప్రారంభం_నిలువు;
    linenr_T	చివర_lnum;
    colnr_T	చివర_నిలువు;
    పూర్ణాంకం		error = FALSE;
    పూర్ణాంకం		prev_did_emsg = did_emsg;

    ఒకవేళ (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	తిరిగి;

    dict = argvars[0].vval.v_dict;
    ఒకవేళ (dict == ఖాళీ || !dict_has_కీలకం(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	తిరిగి;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ఒకవేళ (dict_has_కీలకం(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_సంఖ్య(dict, "id");
	ఒకవేళ (x > గరిష్ఠ_పూర్ణాంకం || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    తిరిగి;
	}
	id = (పూర్ణాంకం)x;
    }

    ఒకవేళ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	తిరిగి;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_వస్తువులు(argvars[1].vval.v_list, li)
    {
	ఒకవేళ (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == ఖాళీ)
	{
	    emsg(_(e_list_required));
	    తిరిగి;
	}

	pos_list = li->li_tv.vval.v_list;
	ప్రారంభం_lnum = list_కనుగొను_nr(pos_list, 0L, &error);
	ఒకవేళ (!error)
	    ప్రారంభం_నిలువు = list_కనుగొను_nr(pos_list, 1L, &error);
	ఒకవేళ (!error)
	    చివర_lnum = list_కనుగొను_nr(pos_list, 2L, &error);
	ఒకవేళ (!error)
	    చివర_నిలువు = list_కనుగొను_nr(pos_list, 3L, &error);
	పూర్ణాంకం this_id = id;
	ఒకవేళ (!error && pos_list->lv_len > 4)
	    this_id = list_కనుగొను_nr(pos_list, 4L, &error);
	ఒకవేళ (error || ప్రారంభం_lnum <= 0 || ప్రారంభం_నిలువు <= 0
		  || చివర_lnum <= 0 || చివర_నిలువు <= 0)
	{
	    ఒకవేళ (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_వాదం));
	    తిరిగి;
	}
	ఒకవేళ (prop_కూడు_one(buf, type_name, this_id, ఖాళీ, 0, 0,
			     ప్రారంభం_lnum, చివర_lnum, ప్రారంభం_నిలువు, చివర_నిలువు) == FAIL)
	    తిరిగి;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
స్థిర పూర్ణాంకం vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    స్థిర పూర్ణాంకం
get_textprop_id(శూన్య)
{
    ఒకవేళ (vt_id_counter > 0)
	vt_id_counter = 0;
    తిరిగి --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    పూర్ణాంకం
prop_కూడు_common(
	linenr_T    ప్రారంభం_lnum,
	colnr_T	    ప్రారంభం_నిలువు,
	dict_T	    *dict,
	buf_T	    *default_buf,
	typval_T    *dict_arg)
{
    linenr_T	చివర_lnum;
    colnr_T	చివర_నిలువు;
    char_u	*type_name;
    buf_T	*buf = default_buf;
    పూర్ణాంకం		id = 0;
    char_u	*పాఠ్యం = ఖాళీ;
    పూర్ణాంకం		పాఠ్యం_padding_ఎడమ = 0;
    పూర్ణాంకం		flags = 0;

    ఒకవేళ (dict == ఖాళీ || !dict_has_కీలకం(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	వెళ్ళు theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ఒకవేళ (dict_has_కీలకం(dict, "end_lnum"))
    {
	చివర_lnum = dict_get_సంఖ్య(dict, "end_lnum");
	ఒకవేళ (చివర_lnum < ప్రారంభం_lnum)
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str), "end_lnum");
	    వెళ్ళు theend;
	}
    }
    లేకపోతే
	చివర_lnum = ప్రారంభం_lnum;

    ఒకవేళ (dict_has_కీలకం(dict, "length"))
    {
	దీర్ఘ నిడివి = dict_get_సంఖ్య(dict, "length");

	ఒకవేళ (నిడివి < 0 || చివర_lnum > ప్రారంభం_lnum)
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str), "length");
	    వెళ్ళు theend;
	}
	చివర_నిలువు = ప్రారంభం_నిలువు + నిడివి;
    }
    లేకపోతే ఒకవేళ (dict_has_కీలకం(dict, "end_col"))
    {
	చివర_నిలువు = dict_get_సంఖ్య(dict, "end_col");
	ఒకవేళ (చివర_నిలువు <= 0)
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str), "end_col");
	    వెళ్ళు theend;
	}
    }
    లేకపోతే ఒకవేళ (ప్రారంభం_lnum == చివర_lnum)
	చివర_నిలువు = ప్రారంభం_నిలువు;
    లేకపోతే
	చివర_నిలువు = 1;

    ఒకవేళ (dict_has_కీలకం(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_సంఖ్య(dict, "id");
	ఒకవేళ (x > గరిష్ఠ_పూర్ణాంకం || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    వెళ్ళు theend;
	}
	id = (పూర్ణాంకం)x;
    }

    ఒకవేళ (dict_has_కీలకం(dict, "text"))
    {
	ఒకవేళ (dict_has_కీలకం(dict, "length")
		|| dict_has_కీలకం(dict, "end_col")
		|| dict_has_కీలకం(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_నిడివి_endcol_and_endlnum_with_పాఠ్యం));
	    వెళ్ళు theend;
	}

	పాఠ్యం = dict_get_string(dict, "text", TRUE);
	ఒకవేళ (పాఠ్యం == ఖాళీ)
	    వెళ్ళు theend;
	// use a default length of 1 to make multiple props show up
	చివర_నిలువు = ప్రారంభం_నిలువు + 1;

	ఒకవేళ (dict_has_కీలకం(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    ఒకవేళ (p == ఖాళీ)
		వెళ్ళు theend;
	    ఒకవేళ (ప్రారంభం_నిలువు != 0)
	    {
		emsg(_(e_can_only_use_పాఠ్యం_align_when_స్తంభం_is_zero));
		వెళ్ళు theend;
	    }
	    ఒకవేళ (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_కుడి;
	    లేకపోతే ఒకవేళ (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    లేకపోతే ఒకవేళ (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_క్రింద;
	    లేకపోతే ఒకవేళ (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_విలువ_for_వాదం_str_str), "text_align", p);
		వెళ్ళు theend;
	    }
	}

	ఒకవేళ (dict_has_కీలకం(dict, "text_padding_left"))
	{
	    పాఠ్యం_padding_ఎడమ = dict_get_సంఖ్య(dict, "text_padding_left");
	    ఒకవేళ (పాఠ్యం_padding_ఎడమ < 0)
	    {
		semsg(_(e_వాదం_must_be_ధనాత్మక_str), "text_padding_left");
		వెళ్ళు theend;
	    }
	}

	ఒకవేళ (dict_has_కీలకం(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    ఒకవేళ (p == ఖాళీ)
		వెళ్ళు theend;
	    ఒకవేళ (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    లేకపోతే ఒకవేళ (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_విలువ_for_వాదం_str_str), "text_wrap", p);
		వెళ్ళు theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    ఒకవేళ (ప్రారంభం_నిలువు < (పాఠ్యం == ఖాళీ ? 1 : 0))
    {
	semsg(_(e_invalid_స్తంభం_సంఖ్య_nr), (దీర్ఘ)ప్రారంభం_నిలువు);
	వెళ్ళు theend;
    }
    ఒకవేళ (ప్రారంభం_నిలువు > 0 && పాఠ్యం_padding_ఎడమ > 0)
    {
	emsg(_(e_can_only_use_ఎడమ_padding_when_స్తంభం_is_zero));
	వెళ్ళు theend;
    }

    ఒకవేళ (dict_arg != ఖాళీ && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	వెళ్ళు theend;

    ఒకవేళ (పాఠ్యం != ఖాళీ)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    లేకపోతే ఒకవేళ (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	వెళ్ళు theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_కూడు_one(buf, type_name, id, పాఠ్యం, పాఠ్యం_padding_ఎడమ, flags,
				    ప్రారంభం_lnum, చివర_lnum, ప్రారంభం_నిలువు, చివర_నిలువు);
    పాఠ్యం = ఖాళీ;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(పాఠ్యం);
    తిరిగి id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    పూర్ణాంకం
get_పాఠ్యం_props(buf_T *buf, linenr_T lnum, char_u **props, పూర్ణాంకం will_change)
{
    char_u	*పాఠ్యం;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_లెక్క;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    ఒకవేళ ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == ఖాళీ)
	తిరిగి 0;

    // Fetch the line to get the ml_line_len field updated.
    పాఠ్యం = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_పంక్తి_len - textlen;
    ఒకవేళ (propdata_len == 0)
	తిరిగి 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    ఒకవేళ (propdata_len < PROP_లెక్క_కొలత + పరిమాణం(textprop_T))
    {
	emsg(e_పాఠ్యం_property_info_corrupted);
	తిరిగి 0;
    }
    mch_memmove(&prop_లెక్క, పాఠ్యం + textlen, PROP_లెక్క_కొలత);
    ఒకవేళ (!పాఠ్యం_prop_లెక్క_valid(prop_లెక్క, propdata_len))
    {
	emsg(e_పాఠ్యం_property_info_corrupted);
	తిరిగి 0;
    }
    ఒకవేళ (!పాఠ్యం_prop_vtext_valid(పాఠ్యం + textlen + PROP_లెక్క_కొలత,
					   (పూర్ణాంకం)prop_లెక్క, propdata_len))
    {
	emsg(e_పాఠ్యం_property_info_corrupted);
	తిరిగి 0;
    }
    *props = పాఠ్యం + textlen + PROP_లెక్క_కొలత;
    తిరిగి (పూర్ణాంకం)prop_లెక్క;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    పూర్ణాంకం
prop_లెక్క_above_క్రింద(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    పూర్ణాంకం		లెక్క = get_పాఠ్యం_props(buf, lnum, &props, FALSE);
    పూర్ణాంకం		ఫలితం = 0;
    textprop_T	prop;
    పూర్ణాంకం		i;
    పూర్ణాంకం		తదుపరి_కుడి_goes_క్రింద = FALSE;

    ఒకవేళ (లెక్క == 0)
	తిరిగి 0;
    కొరకు (i = 0; i < లెక్క; ++i)
    {
	mch_memmove(&prop, props + i * పరిమాణం(prop), పరిమాణం(prop));
	ఒకవేళ (prop.tp_నిలువు == MAXCOL && పాఠ్యం_prop_type_valid(buf, &prop))
	{
	    ఒకవేళ ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_క్రింద))
		    || (తదుపరి_కుడి_goes_క్రింద
				     && (prop.tp_flags & TP_FLAG_ALIGN_కుడి)))
	    {
		++ఫలితం;
	    }
	    లేకపోతే ఒకవేళ (prop.tp_flags & TP_FLAG_ALIGN_కుడి)
		తదుపరి_కుడి_goes_క్రింద = TRUE;
	}
    }
    తిరిగి ఫలితం;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    పూర్ణాంకం
లెక్క_props(linenr_T lnum, పూర్ణాంకం only_starting, పూర్ణాంకం చివరి_పంక్తి)
{
    char_u	*props;
    పూర్ణాంకం		proplen = get_పాఠ్యం_props(curbuf, lnum, &props, 0);
    పూర్ణాంకం		ఫలితం = proplen;
    textprop_T	prop;

    కొరకు (పూర్ణాంకం i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * పరిమాణం(prop), పరిమాణం(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	ఒకవేళ ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!చివరి_పంక్తి && prop.tp_నిలువు == MAXCOL))
	    --ఫలితం;
    }
    తిరిగి ఫలితం;
}

స్థిర textprop_T	*పాఠ్యం_prop_compare_props;
స్థిర buf_T		*పాఠ్యం_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    స్థిర పూర్ణాంకం
పాఠ్యం_prop_క్రమం(పూర్ణాంకం flags)
{
    ఒకవేళ (flags & TP_FLAG_ALIGN_ABOVE)
	తిరిగి 0;
    ఒకవేళ (flags & TP_FLAG_ALIGN_కుడి)
	తిరిగి 2;
    ఒకవేళ (flags & TP_FLAG_ALIGN_క్రింద)
	తిరిగి 3;
    తిరిగి 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    స్థిర పూర్ణాంకం
పాఠ్యం_prop_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    పూర్ణాంకం  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(పూర్ణాంకం *)s1;
    idx2 = *(పూర్ణాంకం *)s2;
    tp1 = &పాఠ్యం_prop_compare_props[idx1];
    tp2 = &పాఠ్యం_prop_compare_props[idx2];
    col1 = tp1->tp_నిలువు;
    col2 = tp2->tp_నిలువు;

    // property that inserts text has priority over one that doesn't
    ఒకవేళ ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	తిరిగి tp1->tp_id < 0 ? 1 : -1;

    ఒకవేళ (col1 == MAXCOL || col2 == MAXCOL)
    {
	పూర్ణాంకం order1 = పాఠ్యం_prop_క్రమం(tp1->tp_flags);
	పూర్ణాంకం order2 = పాఠ్యం_prop_క్రమం(tp2->tp_flags);

	// sort on order where it is added
	ఒకవేళ (order1 != order2)
	    తిరిగి order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = పాఠ్యం_prop_type_by_id(పాఠ్యం_prop_compare_buf, tp1->tp_type);
    pt2 = పాఠ్యం_prop_type_by_id(పాఠ్యం_prop_compare_buf, tp2->tp_type);
    ఒకవేళ (pt1 != pt2)
    {
	ఒకవేళ (pt1 == ఖాళీ)
	    తిరిగి -1;
	ఒకవేళ (pt2 == ఖాళీ)
	    తిరిగి 1;
	ఒకవేళ (pt1->pt_ప్రాధాన్యత != pt2->pt_ప్రాధాన్యత)
	    తిరిగి pt1->pt_ప్రాధాన్యత > pt2->pt_ప్రాధాన్యత ? 1 : -1;
    }

    // same priority, one that starts first wins
    ఒకవేళ (col1 != col2)
	తిరిగి col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    ఒకవేళ (tp1->tp_id < 0)
	తిరిగి tp1->tp_id > tp2->tp_id ? 1 : -1;

    తిరిగి 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    శూన్య
క్రమీకరించు_పాఠ్యం_props(
	buf_T	    *buf,
	textprop_T  *props,
	పూర్ణాంకం	    *idxs,
	పూర్ణాంకం	    లెక్క)
{
    పాఠ్యం_prop_compare_buf = buf;
    పాఠ్యం_prop_compare_props = props;
    qsort((శూన్య *)idxs, (size_t)లెక్క, పరిమాణం(పూర్ణాంకం), పాఠ్యం_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
కనుగొను_prop_లోపల_పంక్తులు(
	win_T	    *wp,
	పూర్ణాంకం	    type_id,
	పూర్ణాంకం	    id,
	textprop_T  *prop,
	linenr_T    *దొరికింది_lnum,
	linenr_T    మొదటి_lnum,
	linenr_T    చివరి_lnum)
{
    ఒకవేళ (పాఠ్యం_prop_type_by_id(wp->w_బఫరు, type_id) == ఖాళీ)
	తిరిగి false;

    ఒకవేళ (మొదటి_lnum < 1)
	మొదటి_lnum = 1;
    ఒకవేళ (చివరి_lnum > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	చివరి_lnum = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;

    కొరకు (linenr_T lnum = మొదటి_lnum; lnum <= చివరి_lnum; ++lnum)
    {
	char_u	*props;
	పూర్ణాంకం	లెక్క = get_పాఠ్యం_props(wp->w_బఫరు, lnum, &props, FALSE);
	కొరకు (పూర్ణాంకం i = 0; i < లెక్క; ++i)
	{
	    mch_memmove(prop, props + i * పరిమాణం(textprop_T),
							   పరిమాణం(textprop_T));
	    ఒకవేళ (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*దొరికింది_lnum = lnum;
		తిరిగి true;
	    }
	}
    }
    తిరిగి false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
కనుగొను_visible_prop(
	win_T	    *wp,
	పూర్ణాంకం	    type_id,
	పూర్ణాంకం	    id,
	textprop_T  *prop,
	linenr_T    *దొరికింది_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    తిరిగి కనుగొను_prop_లోపల_పంక్తులు(wp, type_id, id, prop, దొరికింది_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    స్థిర శూన్య
సమితి_పాఠ్యం_props(linenr_T lnum, textprop_T *tps, పూర్ణాంకం లెక్క)
{
    char_u  *పాఠ్యం;
    char_u  *newtext;
    పూర్ణాంకం	    textlen;

    పాఠ్యం = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    ఒకవేళ (లెక్క == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	ఒకవేళ (newtext == ఖాళీ)
	    తిరిగి;
	mch_memmove(newtext, పాఠ్యం, textlen);
	ఒకవేళ (curbuf->b_ml.ml_flags & (ML_పంక్తి_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_పంక్తి_ptr);
	curbuf->b_ml.ml_పంక్తి_ptr = newtext;
	curbuf->b_ml.ml_పంక్తి_len = textlen;
	curbuf->b_ml.ml_flags |= ML_పంక్తి_DIRTY;
    }
    లేకపోతే
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_లెక్క = (uint16_t)లెక్క;
	పూర్ణాంకం	    vtext_కొలత = 0;
	పూర్ణాంకం	    మొత్తం_len;
	char_u	    *లెక్క_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	పూర్ణాంకం	    i;

	కొరకు (i = 0; i < లెక్క; ++i)
	    ఒకవేళ (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_కొలత += tps[i].tp_len + 1;

	మొత్తం_len = textlen + (పూర్ణాంకం)PROP_లెక్క_కొలత
		    + లెక్క * (పూర్ణాంకం)పరిమాణం(textprop_T) + vtext_కొలత;
	newtext = alloc(మొత్తం_len);
	ఒకవేళ (newtext == ఖాళీ)
	    తిరిగి;
	mch_memmove(newtext, పాఠ్యం, textlen);

	లెక్క_dest = newtext + textlen;
	mch_memmove(లెక్క_dest, &prop_లెక్క, PROP_లెక్క_కొలత);

	prop_dest = లెక్క_dest + PROP_లెక్క_కొలత;
	vtext_dest = prop_dest + లెక్క * పరిమాణం(textprop_T);

	కొరకు (i = 0; i < లెక్క; ++i)
	{
	    textprop_T prop = tps[i];

	    ఒకవేళ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		పూర్ణాంకం copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_పాఠ్యం, copysize);
		vim_free(tps[i].u.tp_పాఠ్యం);
		tps[i].u.tp_పాఠ్యం = ఖాళీ;
		prop.u.tp_పాఠ్యం_స్థానభ్రంశం = (colnr_T)(vtext_dest - లెక్క_dest);
		vtext_dest += copysize;
	    }
	    లేకపోతే
		prop.u.tp_పాఠ్యం_స్థానభ్రంశం = 0;
	    mch_memmove(prop_dest, &prop, పరిమాణం(textprop_T));
	    prop_dest += పరిమాణం(textprop_T);
	}

	ఒకవేళ (curbuf->b_ml.ml_flags & (ML_పంక్తి_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_పంక్తి_ptr);
	curbuf->b_ml.ml_పంక్తి_ptr = newtext;
	curbuf->b_ml.ml_పంక్తి_len = మొత్తం_len;
	curbuf->b_ml.ml_flags |= ML_పంక్తి_DIRTY;
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
props_కూడు_లెక్క_శీర్షిక(char_u *పంక్తి, పూర్ణాంకం పంక్తి_len, పూర్ణాంకం textlen, పూర్ణాంకం *new_len)
{
    పూర్ణాంకం		prop_బైట్లు = పంక్తి_len - textlen;
    uint16_t	prop_లెక్క;
    పూర్ణాంకం		vtext_కొలత = 0;
    పూర్ణాంకం		మొత్తం;
    char_u	*newline;
    char_u	*లెక్క_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    పూర్ణాంకం		i;

    ఒకవేళ (prop_బైట్లు <= 0 || prop_బైట్లు % పరిమాణం(textprop_T) != 0)
    {
	*new_len = పంక్తి_len;
	తిరిగి ఖాళీ;
    }
    prop_లెక్క = (uint16_t)(prop_బైట్లు / పరిమాణం(textprop_T));

    // Calculate total vtext size.
    కొరకు (i = 0; i < (పూర్ణాంకం)prop_లెక్క; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, పంక్తి + textlen + i * పరిమాణం(textprop_T),
							పరిమాణం(textprop_T));
	ఒకవేళ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_కొలత += prop.tp_len + 1;
    }

    మొత్తం = textlen + (పూర్ణాంకం)PROP_లెక్క_కొలత + prop_బైట్లు + vtext_కొలత;
    newline = alloc(మొత్తం);
    ఒకవేళ (newline == ఖాళీ)
	తిరిగి ఖాళీ;
    *new_len = మొత్తం;

    mch_memmove(newline, పంక్తి, textlen);
    లెక్క_dest = newline + textlen;
    mch_memmove(లెక్క_dest, &prop_లెక్క, PROP_లెక్క_కొలత);
    prop_dest = లెక్క_dest + PROP_లెక్క_కొలత;
    vtext_dest = prop_dest + prop_బైట్లు;

    కొరకు (i = 0; i < (పూర్ణాంకం)prop_లెక్క; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, పంక్తి + textlen + i * పరిమాణం(textprop_T),
							పరిమాణం(textprop_T));
	ఒకవేళ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    పూర్ణాంకం copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_పాఠ్యం, copysize);
	    vim_free(prop.u.tp_పాఠ్యం);
	    prop.u.tp_పాఠ్యం_స్థానభ్రంశం = (colnr_T)(vtext_dest - లెక్క_dest);
	    vtext_dest += copysize;
	}
	లేకపోతే
	    prop.u.tp_పాఠ్యం_స్థానభ్రంశం = 0;
	mch_memmove(prop_dest + i * పరిమాణం(textprop_T), &prop,
							పరిమాణం(textprop_T));
    }
    తిరిగి newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    శూన్య
కూడు_పాఠ్యం_props(linenr_T lnum, textprop_T *పాఠ్యం_props, పూర్ణాంకం పాఠ్యం_prop_లెక్క)
{
    unpacked_memline_T	um;
    పూర్ణాంకం			i;

    um = um_open_at(curbuf, lnum, పాఠ్యం_prop_లెక్క);
    ఒకవేళ (um.buf == ఖాళీ)
	తిరిగి;
    ఒకవేళ (!um_detach(&um))
    {
	um_abort(&um);
	తిరిగి;
    }

    // Grow props array if needed.
    ఒకవేళ (um.prop_లెక్క + పాఠ్యం_prop_లెక్క > um.prop_కొలత)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_లెక్క + పాఠ్యం_prop_లెక్క) * పరిమాణం(textprop_T));
	ఒకవేళ (newprops == ఖాళీ)
	{
	    um_abort(&um);
	    తిరిగి;
	}
	um.props = newprops;
	um.prop_కొలత = um.prop_లెక్క + పాఠ్యం_prop_లెక్క;
    }

    కొరకు (i = 0; i < పాఠ్యం_prop_లెక్క; ++i)
	um.props[um.prop_లెక్క++] = పాఠ్యం_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    స్థిర పూర్ణాంకం
compare_pt(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    తిరిగి tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    స్థిర proptype_T *
కనుగొను_type_by_id(hashtab_T *ht, proptype_T ***array, పూర్ణాంకం id)
{
    పూర్ణాంకం దిగువ = 0;
    పూర్ణాంకం ఎగువ;

    ఒకవేళ (ht == ఖాళీ || ht->ht_వాడిన == 0)
	తిరిగి ఖాళీ;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    ఒకవేళ (*array == ఖాళీ)
    {
	దీర్ఘ	    todo;
	hashitem_T  *hi;
	పూర్ణాంకం	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_వాడిన);
	ఒకవేళ (*array == ఖాళీ)
	    తిరిగి ఖాళీ;
	todo = (దీర్ఘ)ht->ht_వాడిన;
	FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
	{
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((శూన్య *)*array, ht->ht_వాడిన, పరిమాణం(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    ఎగువ = ht->ht_వాడిన;
    అయితే (ఎగువ > దిగువ)
    {
	పూర్ణాంకం m = (ఎగువ + దిగువ) / 2;

	ఒకవేళ ((*array)[m]->pt_id == id)
	    తిరిగి (*array)[m];
	ఒకవేళ ((*array)[m]->pt_id > id)
	    ఎగువ = m;
	లేకపోతే
	    దిగువ = m + 1;
    }
    తిరిగి ఖాళీ;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    స్థిర శూన్య
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    పూర్ణాంకం buflocal = TRUE;
    పూర్ణాంకం virtualtext_prop = prop->tp_id < 0;

    dict_కూడు_సంఖ్య(dict, "col", (prop->tp_నిలువు == MAXCOL) ? 0 : prop->tp_నిలువు);
    ఒకవేళ (!virtualtext_prop)
    {
	dict_కూడు_సంఖ్య(dict, "length", prop->tp_len);
	dict_కూడు_సంఖ్య(dict, "id", prop->tp_id);
    }
    dict_కూడు_సంఖ్య(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_కూడు_సంఖ్య(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_తదుపరి));

    pt = కనుగొను_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    ఒకవేళ (pt == ఖాళీ)
    {
	pt = కనుగొను_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    ఒకవేళ (pt != ఖాళీ)
	dict_కూడు_string(dict, "type", pt->pt_name);

    ఒకవేళ (buflocal)
	dict_కూడు_సంఖ్య(dict, "type_bufnr", buf->b_fnum);
    లేకపోతే
	dict_కూడు_సంఖ్య(dict, "type_bufnr", 0);
    ఒకవేళ (virtualtext_prop)
    {
	string_T    పాఠ్యం_align = {ఖాళీ, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_కూడు_string(dict, "text", prop->u.tp_పాఠ్యం);

	// text_align
	ఒకవేళ (prop->tp_flags & TP_FLAG_ALIGN_కుడి)
	    STR_LITERAL_సమితి(పాఠ్యం_align, "right");
	లేకపోతే ఒకవేళ (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_సమితి(పాఠ్యం_align, "above");
	లేకపోతే ఒకవేళ (prop->tp_flags & TP_FLAG_ALIGN_క్రింద)
	    STR_LITERAL_సమితి(పాఠ్యం_align, "below");
	ఒకవేళ (పాఠ్యం_align.string != ఖాళీ)
	    dict_కూడు_string_len(dict, "text_align",
		పాఠ్యం_align.string, (పూర్ణాంకం)పాఠ్యం_align.నిడివి);

	// text_wrap
	ఒకవేళ (prop->tp_flags & TP_FLAG_WRAP)
	    dict_కూడు_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	ఒకవేళ (prop->tp_padleft != 0)
	    dict_కూడు_సంఖ్య(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
పాఠ్యం_prop_type_by_id(buf_T *buf, పూర్ణాంకం id)
{
    proptype_T *type;

    type = కనుగొను_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    ఒకవేళ (type == ఖాళీ)
	type = కనుగొను_type_by_id(global_proptypes, &global_proparray, id);
    తిరిగి type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
పాఠ్యం_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    తిరిగి పాఠ్యం_prop_type_by_id(buf, prop->tp_type) != ఖాళీ;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    శూన్య
f_prop_clear(typval_T *argvars, typval_T *rettv వాడని)
{
    linenr_T ప్రారంభం;
    linenr_T చివర;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    పూర్ణాంకం	    did_clear = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	తిరిగి;

    ప్రారంభం = tv_get_సంఖ్య(&argvars[0]);
    చివర = ప్రారంభం;
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	చివర = tv_get_సంఖ్య(&argvars[1]);
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ఒకవేళ (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		తిరిగి;
	}
    }
    ఒకవేళ (ప్రారంభం < 1 || చివర < 1)
    {
	emsg(_(e_invalid_range));
	తిరిగి;
    }

    {
	unpacked_memline_T um = um_open(buf);

	కొరకు (lnum = ప్రారంభం; lnum <= చివర; ++lnum)
	{
	    పూర్ణాంకం idx;

	    ఒకవేళ (lnum > buf->b_ml.ml_పంక్తి_లెక్క)
		ఆపు;
	    ఒకవేళ (!um_goto_పంక్తి(&um, lnum, 0))
		ఆపు;
	    ఒకవేళ (um.prop_లెక్క > 0)
	    {
		did_clear = TRUE;
		కొరకు (idx = um.prop_లెక్క - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    ఒకవేళ (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    శూన్య
f_prop_కనుగొను(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    పూర్ణాంకం		lnum_ప్రారంభం;
    పూర్ణాంకం		ప్రారంభం_pos_has_prop = 0;
    పూర్ణాంకం		చూసిన_చివర = FALSE;
    పూర్ణాంకం		id = 0;
    పూర్ణాంకం		id_దొరికింది = FALSE;
    పూర్ణాంకం		type_id = -1;
    పూర్ణాంకం		skipstart = FALSE;
    పూర్ణాంకం		lnum = -1;
    పూర్ణాంకం		నిలువు = -1;
    పూర్ణాంకం		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    పూర్ణాంకం		both;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	తిరిగి;
    dict = argvars[0].vval.v_dict;

    ఒకవేళ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	తిరిగి;
    ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	ఒకవేళ (*dir_s == 'b')
	    dir = BACKWARD;
	లేకపోతే ఒకవేళ (*dir_s != 'f')
	{
	    emsg(_(e_invalid_వాదం));
	    తిరిగి;
	}
    }

    di = dict_కనుగొను(dict, (char_u *)"lnum", -1);
    ఒకవేళ (di != ఖాళీ)
	lnum = tv_get_సంఖ్య(&di->di_tv);

    di = dict_కనుగొను(dict, (char_u *)"col", -1);
    ఒకవేళ (di != ఖాళీ)
	నిలువు = tv_get_సంఖ్య(&di->di_tv);

    ఒకవేళ (lnum == -1)
    {
	lnum = cursor->lnum;
	నిలువు = cursor->నిలువు + 1;
    }
    లేకపోతే ఒకవేళ (నిలువు == -1)
	నిలువు = 1;

    ఒకవేళ (lnum < 1 || lnum > buf->b_ml.ml_పంక్తి_లెక్క)
    {
	emsg(_(e_invalid_range));
	తిరిగి;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    ఒకవేళ (dict_has_కీలకం(dict, "id"))
    {
	id = dict_get_సంఖ్య(dict, "id");
	id_దొరికింది = TRUE;
    }
    ఒకవేళ (dict_has_కీలకం(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ఒకవేళ (type == ఖాళీ)
	    తిరిగి;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    ఒకవేళ (!id_దొరికింది && type_id == -1)
    {
	emsg(_(e_need_at_కనిష్ఠ_one_యొక్క_id_or_type));
	తిరిగి;
    }
    ఒకవేళ (both && (!id_దొరికింది || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	తిరిగి;
    }

    lnum_ప్రారంభం = lnum;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;

    అయితే (1)
    {
	char_u	*prop_ప్రారంభం_ptr;
	పూర్ణాంకం	లెక్క = get_పాఠ్యం_props(buf, lnum, &prop_ప్రారంభం_ptr, FALSE);
	పూర్ణాంకం	    i;
	textprop_T  prop;
	పూర్ణాంకం	    prop_ప్రారంభం;
	పూర్ణాంకం	    prop_చివర;

	కొరకు (i = dir == BACKWARD ? లెక్క - 1 : 0; i >= 0 && i < లెక్క; i += dir)
	{
	    mch_memmove(&prop, prop_ప్రారంభం_ptr + i * పరిమాణం(textprop_T),
							   పరిమాణం(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    ఒకవేళ (prop.tp_id < 0 && prop.u.tp_పాఠ్యం_స్థానభ్రంశం > 0)
	    {
		prop.u.tp_పాఠ్యం = (prop_ప్రారంభం_ptr - PROP_లెక్క_కొలత)
						    + prop.u.tp_పాఠ్యం_స్థానభ్రంశం;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    లేకపోతే
		prop.u.tp_పాఠ్యం = ఖాళీ;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    ఒకవేళ (lnum == lnum_ప్రారంభం)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_నిలువు == MAXCOL;
		ఒకవేళ (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    ఒకవేళ (!is_floating_vtext && prop.tp_నిలువు > నిలువు)
			కొనసాగు;
		}
		లేకపోతే ఒకవేళ (!is_floating_vtext
		    && prop.tp_నిలువు + prop.tp_len - (prop.tp_len != 0) < నిలువు)
		    కొనసాగు;
	    }
	    ఒకవేళ (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_దొరికింది && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		ఒకవేళ (lnum_ప్రారంభం == lnum
			&& నిలువు >= prop.tp_నిలువు
			&& (నిలువు <= prop.tp_నిలువు + prop.tp_len
							 - (prop.tp_len != 0)))
		    ప్రారంభం_pos_has_prop = 1;

		// The property was not continued from last line, it starts on
		// this line.
		prop_ప్రారంభం = !(prop.tp_flags & TP_FLAG_CONT_PREV);
		// The property does not continue on the next line, it ends on
		// this line.
		prop_చివర = !(prop.tp_flags & TP_FLAG_CONT_తదుపరి);
		ఒకవేళ (!prop_ప్రారంభం && prop_చివర && dir == FORWARD)
		    చూసిన_చివర = 1;

		// Skip lines without the start flag.
		ఒకవేళ (!prop_ప్రారంభం)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    ఒకవేళ (ప్రారంభం_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    కొనసాగు;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		ఒకవేళ (ప్రారంభం_pos_has_prop && skipstart && !చూసిన_చివర)
		{
		    ప్రారంభం_pos_has_prop = 0;
		    కొనసాగు;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_కూడు_సంఖ్య(rettv->vval.v_dict, "lnum", lnum);

		తిరిగి;
	    }
	}

	ఒకవేళ (dir > 0)
	{
	    ఒకవేళ (lnum >= buf->b_ml.ml_పంక్తి_లెక్క)
		ఆపు;
	    lnum++;
	}
	లేకపోతే
	{
	    ఒకవేళ (lnum <= 1)
		ఆపు;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    స్థిర పూర్ణాంకం
prop_type_or_id_లోపల_list(పూర్ణాంకం *types_or_ids, పూర్ణాంకం len, పూర్ణాంకం type_or_id)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < len; i++)
	ఒకవేళ (types_or_ids[i] == type_or_id)
	    తిరిగి TRUE;

    తిరిగి FALSE;
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
    స్థిర శూన్య
get_props_లోపల_పంక్తి(
	buf_T		*buf,
	linenr_T	lnum,
	పూర్ణాంకం		*prop_types,
	పూర్ణాంకం		prop_types_len,
	పూర్ణాంకం		*prop_ids,
	పూర్ణాంకం		prop_ids_len,
	list_T		*retlist,
	పూర్ణాంకం		కూడు_lnum)
{
    char_u	*props;
    పూర్ణాంకం		లెక్క;
    పూర్ణాంకం		i;
    textprop_T	prop;

    లెక్క = get_పాఠ్యం_props(buf, lnum, &props, FALSE);
    కొరకు (i = 0; i < లెక్క; ++i)
    {
	mch_memmove(&prop, props + i * పరిమాణం(textprop_T),
		పరిమాణం(textprop_T));
	// Convert offset to pointer for virtual text props.
	ఒకవేళ (prop.tp_id < 0 && prop.u.tp_పాఠ్యం_స్థానభ్రంశం > 0)
	{
	    prop.u.tp_పాఠ్యం = (props - PROP_లెక్క_కొలత)
						+ prop.u.tp_పాఠ్యం_స్థానభ్రంశం;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	లేకపోతే
	    prop.u.tp_పాఠ్యం = ఖాళీ;
	ఒకవేళ ((prop_types == ఖాళీ
		    || prop_type_or_id_లోపల_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == ఖాళీ
		    || prop_type_or_id_లోపల_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    ఒకవేళ (d == ఖాళీ)
		ఆపు;
	    prop_fill_dict(d, &prop, buf);
	    ఒకవేళ (కూడు_lnum)
		dict_కూడు_సంఖ్య(d, "lnum", lnum);
	    ఒకవేళ (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    స్థిర పూర్ణాంకం *
get_prop_types_from_పేర్లు(list_T *l, buf_T *buf, పూర్ణాంకం *అంకె_types)
{
    పూర్ణాంకం		*prop_types;
    listitem_T	*li;
    పూర్ణాంకం		i;
    char_u	*name;
    proptype_T	*type;

    *అంకె_types = 0;

    prop_types = ALLOC_MULT(పూర్ణాంకం, list_len(l));
    ఒకవేళ (prop_types == ఖాళీ)
	తిరిగి ఖాళీ;

    i = 0;
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	ఒకవేళ (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    వెళ్ళు errret;
	}
	name = li->li_tv.vval.v_string;
	ఒకవేళ (name == ఖాళీ)
	    వెళ్ళు errret;

	type = lookup_prop_type(name, buf);
	ఒకవేళ (type == ఖాళీ)
	    వెళ్ళు errret;
	prop_types[i++] = type->pt_id;
    }

    *అంకె_types = i;
    తిరిగి prop_types;

errret:
    VIM_CLEAR(prop_types);
    తిరిగి ఖాళీ;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    స్థిర పూర్ణాంకం *
get_prop_ids_from_list(list_T *l, పూర్ణాంకం *అంకె_ids)
{
    పూర్ణాంకం		*prop_ids;
    listitem_T	*li;
    పూర్ణాంకం		i = 0;
    పూర్ణాంకం		id;
    పూర్ణాంకం		error;

    *అంకె_ids = 0;

    prop_ids = ALLOC_MULT(పూర్ణాంకం, list_len(l));
    ఒకవేళ (prop_ids == ఖాళీ)
	తిరిగి ఖాళీ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	error = FALSE;
	id = tv_get_సంఖ్య_chk(&li->li_tv, &error);
	ఒకవేళ (error)
	    వెళ్ళు errret;

	prop_ids[i++] = id;
    }

    *అంకె_ids = i;
    తిరిగి prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    తిరిగి ఖాళీ;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    శూన్య
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	ప్రారంభం_lnum;
    linenr_T	చివర_lnum;
    buf_T	*buf = curbuf;
    పూర్ణాంకం		కూడు_lnum = FALSE;
    పూర్ణాంకం		*prop_types = ఖాళీ;
    పూర్ణాంకం		prop_types_len = 0;
    పూర్ణాంకం		*prop_ids = ఖాళీ;
    పూర్ణాంకం		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    // default: get text properties on current line
    ప్రారంభం_lnum = tv_get_సంఖ్య(&argvars[0]);
    చివర_lnum = ప్రారంభం_lnum;
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ఒకవేళ (check_for_dict_arg(argvars, 1) == FAIL)
	    తిరిగి;
	d = argvars[1].vval.v_dict;

	ఒకవేళ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    తిరిగి;

	ఒకవేళ (d != ఖాళీ && (di = dict_కనుగొను(d, (char_u *)"end_lnum", -1)) != ఖాళీ)
	{
	    ఒకవేళ (di->di_tv.v_type != VAR_సంఖ్య)
	    {
		emsg(_(e_సంఖ్య_required));
		తిరిగి;
	    }
	    చివర_lnum = tv_get_సంఖ్య(&di->di_tv);
	    ఒకవేళ (చివర_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//పంక్తి
		చివర_lnum = buf->b_ml.ml_పంక్తి_లెక్క + చివర_lnum + 1;
	    లేకపోతే ఒకవేళ (చివర_lnum > buf->b_ml.ml_పంక్తి_లెక్క)
		చివర_lnum = buf->b_ml.ml_పంక్తి_లెక్క;
	    కూడు_lnum = TRUE;
	}
	ఒకవేళ (d != ఖాళీ && (di = dict_కనుగొను(d, (char_u *)"types", -1)) != ఖాళీ)
	{
	    ఒకవేళ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		తిరిగి;
	    }

	    l = di->di_tv.vval.v_list;
	    ఒకవేళ (l != ఖాళీ && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_పేర్లు(l, buf, &prop_types_len);
		ఒకవేళ (prop_types == ఖాళీ)
		    తిరిగి;
	    }
	}
	ఒకవేళ (d != ఖాళీ && (di = dict_కనుగొను(d, (char_u *)"ids", -1)) != ఖాళీ)
	{
	    ఒకవేళ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		వెళ్ళు errret;
	    }

	    l = di->di_tv.vval.v_list;
	    ఒకవేళ (l != ఖాళీ && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		ఒకవేళ (prop_ids == ఖాళీ)
		    వెళ్ళు errret;
	    }
	}
    }
    ఒకవేళ (ప్రారంభం_lnum < 1 || ప్రారంభం_lnum > buf->b_ml.ml_పంక్తి_లెక్క
		|| చివర_lnum < 1 || చివర_lnum < ప్రారంభం_lnum)
	emsg(_(e_invalid_range));
    లేకపోతే
	కొరకు (lnum = ప్రారంభం_lnum; lnum <= చివర_lnum; lnum++)
	    get_props_లోపల_పంక్తి(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, కూడు_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    శూన్య
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	ప్రారంభం = 1;
    linenr_T	చివర = 0;
    linenr_T	lnum;
    linenr_T	మొదటి_మారింది = 0;
    linenr_T	చివరి_మారింది = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    పూర్ణాంకం		do_all;
    పూర్ణాంకం		id = -MAXCOL;
    పూర్ణాంకం		type_id = -1;	    // for a single "type"
    పూర్ణాంకం		*type_ids = ఖాళీ;   // array, for a list of "types", allocated
    పూర్ణాంకం		అంకె_type_ids = 0;   // number of elements in "type_ids"
    పూర్ణాంకం		both;

    rettv->vval.v_సంఖ్య = 0;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL)))
	తిరిగి;

    ఒకవేళ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ప్రారంభం = tv_get_సంఖ్య(&argvars[1]);
	చివర = ప్రారంభం;
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	    చివర = tv_get_సంఖ్య(&argvars[2]);
	ఒకవేళ (ప్రారంభం < 1 || చివర < 1)
	{
	    emsg(_(e_invalid_range));
	    తిరిగి;
	}
    }

    dict = argvars[0].vval.v_dict;
    ఒకవేళ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	తిరిగి;
    ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)
	తిరిగి;

    do_all = dict_get_bool(dict, "all", FALSE);

    ఒకవేళ (dict_has_కీలకం(dict, "id"))
	id = dict_get_సంఖ్య(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    ఒకవేళ (dict_has_కీలకం(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ఒకవేళ (type == ఖాళీ)
	    తిరిగి;
	type_id = type->pt_id;
    }
    ఒకవేళ (dict_has_కీలకం(dict, "types"))
    {
	typval_T types;
	listitem_T *li = ఖాళీ;

	dict_get_tv(dict, "types", &types);
	ఒకవేళ (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( పరిమాణం(పూర్ణాంకం) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_వస్తువులు(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		ఒకవేళ (li->li_tv.v_type != VAR_STRING)
		    కొనసాగు;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		ఒకవేళ (!prop_type)
		    వెళ్ళు cleanup_prop_remove;

		type_ids[అంకె_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    ఒకవేళ (id == -MAXCOL && (type_id == -1 && అంకె_type_ids == 0))
    {
	emsg(_(e_need_at_కనిష్ఠ_one_యొక్క_id_or_type));
	వెళ్ళు cleanup_prop_remove;
    }
    ఒకవేళ (both && (id == -MAXCOL || (type_id == -1 && అంకె_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	వెళ్ళు cleanup_prop_remove;
    }
    ఒకవేళ (type_id != -1 && అంకె_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	వెళ్ళు cleanup_prop_remove;
    }

    ఒకవేళ (చివర == 0)
	చివర = buf->b_ml.ml_పంక్తి_లెక్క;

    {
	unpacked_memline_T um = um_open(buf);

	కొరకు (lnum = ప్రారంభం; lnum <= చివర; ++lnum)
	{
	    పూర్ణాంకం idx;

	    ఒకవేళ (lnum > buf->b_ml.ml_పంక్తి_లెక్క)
		ఆపు;
	    ఒకవేళ (!um_goto_పంక్తి(&um, lnum, 0))
		ఆపు;

	    కొరకు (idx = 0; idx < um.prop_లెక్క; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		పూర్ణాంకం	    matches_id = 0;
		పూర్ణాంకం	    matches_type = 0;

		ఒకవేళ (prop->tp_flags & TP_FLAG_DELETED)
		    కొనసాగు;

		matches_id = prop->tp_id == id;
		ఒకవేళ (అంకె_type_ids > 0)
		{
		    పూర్ణాంకం idx2;

		    కొరకు (idx2 = 0; !matches_type && idx2 < అంకె_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		లేకపోతే
		    matches_type = prop->tp_type == type_id;

		ఒకవేళ (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    ఒకవేళ (మొదటి_మారింది == 0)
			మొదటి_మారింది = lnum;
		    చివరి_మారింది = lnum;
		    ++rettv->vval.v_సంఖ్య;
		    ఒకవేళ (!do_all)
			ఆపు;
		}
	    }
	}
	um_close(&um);
    }

    ఒకవేళ (మొదటి_మారింది > 0)
    {
	మారింది_పంక్తి_display_buf(buf);
	మారింది_పంక్తులు_buf(buf, మొదటి_మారింది, చివరి_మారింది + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    స్థిర శూన్య
prop_type_సమితి(typval_T *argvars, పూర్ణాంకం కూడు)
{
    char_u	*name;
    buf_T	*buf = ఖాళీ;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    name = tv_get_string(&argvars[0]);
    ఒకవేళ (*name == NUL)
    {
	semsg(_(e_invalid_వాదం_str), "\"\"");
	తిరిగి;
    }

    ఒకవేళ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	తిరిగి;
    dict = argvars[1].vval.v_dict;

    prop = కనుగొను_prop_type(name, buf);
    ఒకవేళ (కూడు)
    {
	hashtab_T **htp;

	ఒకవేళ (prop != ఖాళీ)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    తిరిగి;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	ఒకవేళ (prop == ఖాళీ)
	    తిరిగి;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	ఒకవేళ (buf == ఖాళీ)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	లేకపోతే
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	ఒకవేళ (*htp == ఖాళీ)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    ఒకవేళ (*htp == ఖాళీ)
	    {
		vim_free(prop);
		తిరిగి;
	    }
	    hash_init(*htp);
	}
	hash_కూడు(*htp, PT2HIKEY(prop), "prop type");
    }
    లేకపోతే
    {
	ఒకవేళ (prop == ఖాళీ)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    తిరిగి;
	}
    }

    ఒకవేళ (dict != ఖాళీ)
    {
	di = dict_కనుగొను(dict, (char_u *)"highlight", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    char_u	*highlight;
	    పూర్ణాంకం		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    ఒకవేళ (highlight != ఖాళీ && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    ఒకవేళ (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_సమూహం_name_str),
			highlight == ఖాళీ ? (char_u *)"" : highlight);
		తిరిగి;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_కనుగొను(dict, (char_u *)"combine", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    ఒకవేళ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    లేకపోతే
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_కనుగొను(dict, (char_u *)"override", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    ఒకవేళ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    లేకపోతే
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_కనుగొను(dict, (char_u *)"priority", -1);
	ఒకవేళ (di != ఖాళీ)
	    prop->pt_ప్రాధాన్యత = tv_get_సంఖ్య(&di->di_tv);

	di = dict_కనుగొను(dict, (char_u *)"start_incl", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    ఒకవేళ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_ప్రారంభం_INCL;
	    లేకపోతే
		prop->pt_flags &= ~PT_FLAG_INS_ప్రారంభం_INCL;
	}

	di = dict_కనుగొను(dict, (char_u *)"end_incl", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    ఒకవేళ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_చివర_INCL;
	    లేకపోతే
		prop->pt_flags &= ~PT_FLAG_INS_చివర_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    శూన్య
f_prop_type_కూడు(typval_T *argvars, typval_T *rettv వాడని)
{
    prop_type_సమితి(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    శూన్య
f_prop_type_change(typval_T *argvars, typval_T *rettv వాడని)
{
    prop_type_సమితి(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    శూన్య
f_prop_type_delete(typval_T *argvars, typval_T *rettv వాడని)
{
    char_u	*name;
    buf_T	*buf = ఖాళీ;
    hashitem_T	*hi;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    name = tv_get_string(&argvars[0]);
    ఒకవేళ (*name == NUL)
    {
	semsg(_(e_invalid_వాదం_str), "\"\"");
	తిరిగి;
    }

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    తిరిగి;
    }

    hi = కనుగొను_prop_type_hi(name, buf);
    ఒకవేళ (hi == ఖాళీ)
	తిరిగి;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    ఒకవేళ (buf == ఖాళీ)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    లేకపోతే
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    మారింది_కిటికీ_setting_buf(buf == ఖాళీ ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    శూన్య
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    name = tv_get_string(&argvars[0]);
    ఒకవేళ (*name == NUL)
    {
	semsg(_(e_invalid_వాదం_str), "\"\"");
	తిరిగి;
    }

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;

    proptype_T  *prop = ఖాళీ;
    buf_T	    *buf = ఖాళీ;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    తిరిగి;
    }

    prop = కనుగొను_prop_type(name, buf);
    ఒకవేళ (prop == ఖాళీ)
	తిరిగి;

    dict_T *d = rettv->vval.v_dict;

    ఒకవేళ (prop->pt_hl_id > 0)
	dict_కూడు_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_కూడు_సంఖ్య(d, "priority", prop->pt_ప్రాధాన్యత);
    dict_కూడు_సంఖ్య(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_కూడు_సంఖ్య(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_కూడు_సంఖ్య(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_ప్రారంభం_INCL) ? 1 : 0);
    dict_కూడు_సంఖ్య(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_చివర_INCL) ? 1 : 0);
    ఒకవేళ (buf != ఖాళీ)
	dict_కూడు_సంఖ్య(d, "bufnr", buf->b_fnum);
}

    స్థిర శూన్య
list_types(hashtab_T *ht, list_T *l)
{
    దీర్ఘ	todo;
    hashitem_T	*hi;

    todo = (దీర్ఘ)ht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
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
    శూన్య
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ఖాళీ;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    తిరిగి;
    }
    ఒకవేళ (buf == ఖాళీ)
    {
	ఒకవేళ (global_proptypes != ఖాళీ)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    లేకపోతే ఒకవేళ (buf->b_proptypes != ఖాళీ)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    స్థిర శూన్య
clear_ht_prop_types(hashtab_T *ht)
{
    దీర్ఘ	todo;
    hashitem_T	*hi;

    ఒకవేళ (ht == ఖాళీ)
	తిరిగి;

    todo = (దీర్ఘ)ht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#ఒకవేళ defined(EXITFREE)
/*
 * Free all global property types.
 */
    శూన్య
clear_global_prop_types(శూన్య)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = ఖాళీ;
    VIM_CLEAR(global_proparray);
}
#అయితే_ముగింపు

/*
 * Free all property types for "buf".
 */
    శూన్య
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = ఖాళీ;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం dirty;	    // if the property was changed
    పూర్ణాంకం can_drop;   // whether after this change, the prop may be removed
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
    స్థిర adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    నిలువు,
	పూర్ణాంకం	    added,
	పూర్ణాంకం	    flags)
{
    proptype_T	*pt;
    పూర్ణాంకం		ప్రారంభం_incl;
    పూర్ణాంకం		చివర_incl;
    పూర్ణాంకం		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    ఒకవేళ (prop->tp_నిలువు == MAXCOL)
    {
	res.dirty = FALSE;
	తిరిగి res;
    }

    pt = పాఠ్యం_prop_type_by_id(curbuf, prop->tp_type);
    ప్రారంభం_incl = (pt != ఖాళీ && (pt->pt_flags & PT_FLAG_INS_ప్రారంభం_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    ఒకవేళ (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	ప్రారంభం_incl = FALSE;
    చివర_incl = (pt != ఖాళీ && (pt->pt_flags & PT_FLAG_INS_చివర_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_తదుపరి);
    // do not drop zero-width props if they later can increase in size
    droppable = !(ప్రారంభం_incl || చివర_incl);

    ఒకవేళ (added > 0)
    {
	ఒకవేళ (నిలువు + 1 <= prop->tp_నిలువు
		- (ప్రారంభం_incl || (prop->tp_len == 0 && చివర_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_నిలువు += added;
	లేకపోతే ఒకవేళ (నిలువు + 1 < prop->tp_నిలువు + prop->tp_len + చివర_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    లేకపోతే ఒకవేళ (prop->tp_నిలువు > నిలువు + 1)
    {
	ఒకవేళ (prop->tp_నిలువు + added < నిలువు + 1)
	{
	    ఒకవేళ (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    లేకపోతే
	    {
		prop->tp_len += (prop->tp_నిలువు - 1 - నిలువు) + added;
		prop->tp_నిలువు = నిలువు + 1;
		ఒకవేళ (prop->tp_len <= 0)
		{
		    prop->tp_len = 0;
		    // Multiline properties with no text left should
		    // also be dropped.
		    res.can_drop = droppable
			|| (prop->tp_flags
				& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_తదుపరి));
		}
	    }
	}
	లేకపోతే
	    prop->tp_నిలువు += added;
    }
    లేకపోతే ఒకవేళ (prop->tp_len > 0 && prop->tp_నిలువు + prop->tp_len > నిలువు
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	ఒకవేళ (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = నిలువు - (prop->tp_నిలువు - 1);
	లేకపోతే
	{
	    పూర్ణాంకం after = నిలువు - added - (prop->tp_నిలువు - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	ఒకవేళ (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_తదుపరి)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_తదుపరి));
    }
    లేకపోతే
	res.dirty = FALSE;

    తిరిగి res;
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
    పూర్ణాంకం
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    నిలువు,
	పూర్ణాంకం	    బైట్లు_added,
	పూర్ణాంకం	    flags)
{
    unpacked_memline_T um;
    పూర్ణాంకం		dirty = FALSE;
    పూర్ణాంకం		ri;

    ఒకవేళ (పాఠ్యం_prop_frozen > 0)
	తిరిగి FALSE;

    um = um_open_at(curbuf, lnum, 0);
    ఒకవేళ (um.buf == ఖాళీ || um.prop_లెక్క == 0)
    {
	um_abort(&um);
	తిరిగి FALSE;
    }

    కొరకు (ri = 0; ri < um.prop_లెక్క; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, నిలువు, బైట్లు_added, flags);
	ఒకవేళ (res.dirty)
	{
	    ఒకవేళ (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		ఒకవేళ (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    తిరిగి FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		ఒకవేళ ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    తిరిగి FALSE;
		}
	    }
	    dirty = TRUE;
	}
	ఒకవేళ (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    కొనసాగు;
	}
    }
    ఒకవేళ (dirty)
	um_close(&um);
    లేకపోతే
	um_abort(&um);
    తిరిగి dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    శూన్య
adjust_props_for_విభజించు(
	linenr_T    lnum_props,
	linenr_T    lnum_శిఖరం,
	పూర్ణాంకం	    kept,
	పూర్ణాంకం	    deleted,
	పూర్ణాంకం	    at_eol)
{
    char_u	*props;
    పూర్ణాంకం		లెక్క;
    garray_T    prevprop;
    garray_T    nextprop;
    పూర్ణాంకం		skipped = kept + deleted;

    ఒకవేళ (!curbuf->b_has_textprop)
	తిరిగి;

    // Get the text properties from "lnum_props".
    లెక్క = get_పాఠ్యం_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, పరిమాణం(textprop_T), 10);
    ga_init2(&nextprop, పరిమాణం(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *లెక్క_ptr = props - PROP_లెక్క_కొలత;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    కొరకు (పూర్ణాంకం i = 0; i < లెక్క; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	పూర్ణాంకం	    ప్రారంభం_incl, చివర_incl;
	పూర్ణాంకం	    cont_prev, cont_తదుపరి;
	పూర్ణాంకం	    prop_నిలువు;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * పరిమాణం(textprop_T), పరిమాణం(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	ఒకవేళ (prop.tp_id < 0 && prop.u.tp_పాఠ్యం_స్థానభ్రంశం > 0)
	{
	    prop.u.tp_పాఠ్యం = vim_strsave(లెక్క_ptr + prop.u.tp_పాఠ్యం_స్థానభ్రంశం);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	లేకపోతే
	    prop.u.tp_పాఠ్యం = ఖాళీ;

	pt = పాఠ్యం_prop_type_by_id(curbuf, prop.tp_type);
	ప్రారంభం_incl = (pt != ఖాళీ && (pt->pt_flags & PT_FLAG_INS_ప్రారంభం_INCL));
	చివర_incl = (pt != ఖాళీ && (pt->pt_flags & PT_FLAG_INS_చివర_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_నిలువు = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_నిలువు;

	ఒకవేళ (prop_నిలువు == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_తదుపరి = !at_eol;
	}
	లేకపోతే
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    పూర్ణాంకం is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_నిలువు == MAXCOL);

	    cont_prev = prop_నిలువు + !ప్రారంభం_incl <= kept;
	    ఒకవేళ (is_floating_vtext)
		cont_తదుపరి = FALSE;
	    లేకపోతే
		cont_తదుపరి = skipped <= prop_నిలువు + prop.tp_len - !చివర_incl;
	}
	// when a prop has text it is never copied
	ఒకవేళ (prop.tp_id < 0 && cont_తదుపరి)
	    cont_prev = FALSE;

	ఒకవేళ (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_దత్తాంశం) + prevprop.ga_len;
	    పూర్ణాంకం is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    ఒకవేళ (!is_vtext && p->tp_నిలువు + p->tp_len >= kept)
		p->tp_len = kept - p->tp_నిలువు;
	    ఒకవేళ (cont_తదుపరి)
		p->tp_flags |= TP_FLAG_CONT_తదుపరి;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	ఒకవేళ (cont_తదుపరి && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_దత్తాంశం) + nextprop.ga_len;
	    పూర్ణాంకం is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    ఒకవేళ (p->tp_నిలువు != MAXCOL)
	    {
		ఒకవేళ (p->tp_నిలువు > skipped)
		    p->tp_నిలువు -= skipped - 1;
		లేకపోతే
		{
		    ఒకవేళ (!is_vtext)
			p->tp_len -= skipped - p->tp_నిలువు;
		    p->tp_నిలువు = 1;
		}
	    }
	    ఒకవేళ (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    సమితి_పాఠ్యం_props(lnum_శిఖరం, (textprop_T *)prevprop.ga_దత్తాంశం,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    సమితి_పాఠ్యం_props(lnum_శిఖరం + 1, (textprop_T *)nextprop.ga_దత్తాంశం,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    శూన్య
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	పూర్ణాంకం		    చివరి_పంక్తి,
	దీర్ఘ		    నిలువు,
	పూర్ణాంకం		    removed)
{
    unpacked_memline_T	r_um;

    ఒకవేళ (um->buf == ఖాళీ)
	తిరిగి;

    r_um = um_open_at(um->buf, lnum, 0);
    ఒకవేళ (r_um.buf == ఖాళీ)
	తిరిగి;
    ఒకవేళ (!um_detach(&r_um))
    {
	um_abort(&r_um);
	తిరిగి;
    }

    కొరకు (పూర్ణాంకం i = r_um.prop_లెక్క - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	పూర్ణాంకం	    చివర;

	ఒకవేళ (prop->tp_నిలువు == MAXCOL && !చివరి_పంక్తి)
	    కొనసాగు;  // drop floating text for non-last lines
	చివర = !(prop->tp_flags & TP_FLAG_CONT_తదుపరి);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, నిలువు, 0);

	ఒకవేళ (చివరి_పంక్తి || చివర)
	{
	    um_కూడు_prop(um, prop);
	    prop->u.tp_పాఠ్యం = ఖాళీ;  // ownership transferred
	}
	లేకపోతే
	{
	    // Search for continuing prop in um.
	    bool    దొరికింది = false;

	    కొరకు (పూర్ణాంకం j = 0; j < um->prop_లెక్క; ++j)
	    {
		textprop_T *op = &um->props[j];

		ఒకవేళ ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    దొరికింది = true;
		    op->tp_len += op->tp_నిలువు - prop->tp_నిలువు;
		    op->tp_నిలువు = prop->tp_నిలువు;
		    op->tp_flags = prop->tp_flags;
		    ఆపు;
		}
	    }
	    ఒకవేళ (!దొరికింది)
		అంతర్గత_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    స్థిర శూన్య
remember_cleared_prop(garray_T *gap, పూర్ణాంకం which, textprop_T *prop, పూర్ణాంకం flag)
{
    undoprop_T	*పైకి;

    ఒకవేళ (which == UNDOPROP_NONE)
	తిరిగి;
    ఒకవేళ (gap->ga_itemsize == 0)
	ga_init2(gap, పరిమాణం(undoprop_T), 4);
    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి;
    పైకి = (undoprop_T *)gap->ga_దత్తాంశం + gap->ga_len++;
    పైకి->పైకి_which = which;
    పైకి->పైకి_type = prop->tp_type;
    పైకి->పైకి_id = prop->tp_id;
    పైకి->పైకి_నిలువు = prop->tp_నిలువు;
    పైకి->పైకి_flag = flag;
}

/*
 * Clear the continuation flags between line "lnum" and the line below it that
 * have no partner in the other line.  A property is split there then.  What
 * is cleared on the line above is remembered in "gap" as "above_which", on
 * the line below as "below_which".
 */
    శూన్య
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	పూర్ణాంకం	    above_which,
	పూర్ణాంకం	    క్రింద_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	క్రింద;
    bool		*paired = ఖాళీ;
    bool		above_మారింది = false;
    bool		క్రింద_మారింది = false;
    bool		cont_తదుపరి = false;

    ఒకవేళ (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి;

    above = um_open_at(buf, lnum, 0);
    ఒకవేళ (above.buf == ఖాళీ)
	తిరిగి;
    కొరకు (పూర్ణాంకం i = 0; i < above.prop_లెక్క; ++i)
	ఒకవేళ (above.props[i].tp_flags & TP_FLAG_CONT_తదుపరి)
	    cont_తదుపరి = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    ఒకవేళ (cont_తదుపరి && !um_detach(&above))
    {
	um_abort(&above);
	తిరిగి;
    }

    క్రింద = um_open_at(buf, lnum + 1, 0);
    ఒకవేళ (క్రింద.buf == ఖాళీ)
    {
	um_abort(&above);
	తిరిగి;
    }
    ఒకవేళ (క్రింద.prop_లెక్క > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, క్రింద.prop_లెక్క);
	ఒకవేళ (paired == ఖాళీ)
	    వెళ్ళు theend;
    }

    కొరకు (పూర్ణాంకం i = 0; i < above.prop_లెక్క; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    దొరికింది = false;

	ఒకవేళ (!(prop->tp_flags & TP_FLAG_CONT_తదుపరి))
	    కొనసాగు;
	కొరకు (పూర్ణాంకం j = 0; j < క్రింద.prop_లెక్క; ++j)
	{
	    textprop_T *తదుపరి = &క్రింద.props[j];

	    ఒకవేళ (!paired[j] && (తదుపరి->tp_flags & TP_FLAG_CONT_PREV)
		    && తదుపరి->tp_id == prop->tp_id
		    && తదుపరి->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		దొరికింది = true;
		ఆపు;
	    }
	}
	ఒకవేళ (!దొరికింది)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_తదుపరి;
	    above_మారింది = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_తదుపరి);
	}
    }

    కొరకు (పూర్ణాంకం j = 0; j < క్రింద.prop_లెక్క; ++j)
    {
	ఒకవేళ (!(క్రింద.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    కొనసాగు;
	ఒకవేళ (!క్రింద.detached && !um_detach(&క్రింద))
	    ఆపు;
	క్రింద.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	క్రింద_మారింది = true;
	remember_cleared_prop(gap, క్రింద_which, &క్రింద.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    ఒకవేళ (క్రింద_మారింది)
	um_close(&క్రింద);
    లేకపోతే
	um_abort(&క్రింద);
    ఒకవేళ (above_మారింది)
	um_close(&above);
    లేకపోతే
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    శూన్య
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	పూర్ణాంకం	    which)
{
    unpacked_memline_T	um;
    bool		మారింది = false;

    ఒకవేళ (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి;
    um = um_open_at(buf, lnum, 0);
    ఒకవేళ (um.buf == ఖాళీ)
	తిరిగి;
    కొరకు (పూర్ణాంకం r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *పైకి = (undoprop_T *)gap->ga_దత్తాంశం + r;

	ఒకవేళ (పైకి->పైకి_which != which)
	    కొనసాగు;
	కొరకు (పూర్ణాంకం i = 0; i < um.prop_లెక్క; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    ఒకవేళ (prop->tp_type != పైకి->పైకి_type || prop->tp_id != పైకి->పైకి_id
		    || prop->tp_నిలువు != పైకి->పైకి_నిలువు
		    || (prop->tp_flags & పైకి->పైకి_flag))
		కొనసాగు;
	    ఒకవేళ (!um.detached && !um_detach(&um))
		వెళ్ళు theend;
	    um.props[i].tp_flags |= పైకి->పైకి_flag;
	    మారింది = true;
	    ఆపు;
	}
    }

theend:
    ఒకవేళ (మారింది)
	um_close(&um);
    లేకపోతే
	um_abort(&um);
}

    bool
పాఠ్యం_prop_లెక్క_valid(పూర్ణాంకం prop_లెక్క, size_t propdata_len)
{
    ఒకవేళ (propdata_len < PROP_లెక్క_కొలత)
	తిరిగి false;
    తిరిగి (size_t)prop_లెక్క * పరిమాణం(textprop_T)
		    <= propdata_len - PROP_లెక్క_కొలత;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
పాఠ్యం_prop_vtext_valid(char_u *props, పూర్ణాంకం prop_లెక్క, size_t propdata_len)
{
    కొరకు (పూర్ణాంకం i = 0; i < prop_లెక్క; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * పరిమాణం(textprop_T),
							  పరిమాణం(textprop_T));
	ఒకవేళ (prop.tp_id >= 0 || prop.u.tp_పాఠ్యం_స్థానభ్రంశం <= 0)
	    కొనసాగు;
	ఒకవేళ (prop.tp_len < 0 || (size_t)prop.u.tp_పాఠ్యం_స్థానభ్రంశం
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    తిరిగి false;
    }
    తిరిగి true;
}

#అయితే_ముగింపు // FEAT_PROP_POPUP
