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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_PROP_POPUP)

ସ୍ଥିର ଶୂନ୍ୟ um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
um_free_vtext(textprop_T *props, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_2; ++i)
	ଯଦି (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
um_free(unpacked_memline_T *um)
{
    ଯଦି (um->detached)
    {
	VIM_CLEAR(um->text);
	um_free_vtext(um->props, um->prop_ଗଣନା);
    }
    VIM_CLEAR(um->props);
    um->prop_size = 0;
    um->prop_ଗଣନା = 0;
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
    ଫେରନ୍ତୁ um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_ଧାଡ଼ି(unpacked_memline_T *um, linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_props)
{
    char_u	*ଧାଡ଼ି;
    size_t	textlen;
    size_t	propdata_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		proplen;

    ଯଦି (um->buf == ଖାଲି)
	ଫେରନ୍ତୁ false;

    // Store changes to the current line if any.
    ଯଦି (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    ଯଦି (lnum == 0)
	ଫେରନ୍ତୁ true;	// just unload

    ଧାଡ଼ି = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_ଧାଡ଼ି_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = ଧାଡ଼ି;

    ଯଦି (propdata_len == 0)
	ଫେରନ୍ତୁ true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    ଯଦି (propdata_len < PROP_ଗଣନା_SIZE + ଆକାର(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ଖାଲି;
	ଫେରନ୍ତୁ false;
    }

    uint16_t    prop_ଗଣନା;
    char_u	    *ଗଣନା_ptr = ଧାଡ଼ି + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_ଗଣନା, ଗଣନା_ptr, PROP_ଗଣନା_SIZE);
    ଯଦି (!text_prop_ଗଣନା_valid(prop_ଗଣନା, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ଖାଲି;
	ଫେରନ୍ତୁ false;
    }
    proplen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prop_ଗଣନା;
    props_start = ଗଣନା_ptr + PROP_ଗଣନା_SIZE;

    ଯଦି (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ଖାଲି;
	ଫେରନ୍ତୁ false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    ଯଦି (um->props == ଖାଲି)
    {
	um->buf = ଖାଲି;
	ଫେରନ୍ତୁ false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_ଗଣନା = proplen;
    mch_memmove(um->props, props_start, proplen * ଆକାର(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < proplen; ++i)
    {
	ଯଦି (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = ଗଣନା_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ଅନ୍ୟଥା
	    um->props[i].u.tp_text = ଖାଲି;
    }

    ଫେରନ୍ତୁ true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_props)
{
    unpacked_memline_T um = um_open(buf);

    ଯଦି (!um_goto_ଧାଡ଼ି(&um, lnum, extra_props))
	um.buf = ଖାଲି;
    ଫେରନ୍ତୁ um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    ସ୍ଥିର bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    ଯଦି (um->detached || um->buf == ଖାଲି)
	ଫେରନ୍ତୁ um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    ଯଦି (newtext == ଖାଲି)
    {
	um->buf = ଖାଲି;
	ଫେରନ୍ତୁ false;
    }
    um->text = newtext;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < um->prop_ଗଣନା; ++i)
    {
	ଯଦି (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    ଯଦି (copy == ଖାଲି)
	    {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < i; ++j)
		    ଯଦି (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = ଖାଲି;
		ଫେରନ୍ତୁ false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    ଫେରନ୍ତୁ true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    ଯଦି (um->buf == ଖାଲି)
	ଫେରନ୍ତୁ false;
    ଯଦି (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    ଫେରନ୍ତୁ true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    ସ୍ଥିର bool
um_grow_props(unpacked_memline_T *um, ପୂର୍ଣ୍ଣସଂଖ୍ୟା needed)
{
    ଯଦି (um->prop_ଗଣନା + needed <= um->prop_size)
	ଫେରନ୍ତୁ true;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    new_size = um->prop_ଗଣନା + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * ଆକାର(textprop_T));
    ଯଦି (newprops == ଖାଲି)
	ଫେରନ୍ତୁ false;
    um->props = newprops;
    um->prop_size = new_size;

    ଫେରନ୍ତୁ true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    ଯଦି (um->buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (!um->detached && !um_detach(um))
	ଫେରନ୍ତୁ;
    ଯଦି (!um_grow_props(um, 1))
    {
	um->buf = ଖାଲି;
	ଫେରନ୍ତୁ;
    }
    um->props[um->prop_ଗଣନା++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    ଶୂନ୍ୟ
um_reverse_props(unpacked_memline_T *um)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0, j = um->prop_ଗଣନା - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prop_ଗଣନା)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    ଯଦି (lnum < 1 || lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	um.buf = ଖାଲି;
	ଫେରନ୍ତୁ um;
    }

    char_u *ଧାଡ଼ି = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(ଧାଡ଼ି);

    ଯଦି (text_copy == ଖାଲି)
    {
	um.buf = ଖାଲି;
	ଫେରନ୍ତୁ um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    ଯଦି (prop_ଗଣନା > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_ଗଣନା);
	ଯଦି (um.props == ଖାଲି)
	{
	    vim_free(um.text);
	    um.buf = ଖାଲି;
	    ଫେରନ୍ତୁ um;
	}
	um.prop_size = prop_ଗଣନା;
    }

    ଫେରନ୍ତୁ um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା tp_id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tp_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    unpacked_memline_T	neighbor;

    ଯଦି (lnum < 1 || lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	ଫେରନ୍ତୁ;

    neighbor = um_open_at(buf, lnum, 0);
    ଯଦି (neighbor.buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < neighbor.prop_ଗଣନା; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	ଯଦି (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    ଯଦି (!neighbor.detached && !um_detach(&neighbor))
		ଭାଙ୍ଗ;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    ଭାଙ୍ଗ;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    ଶୂନ୍ୟ
um_delete_prop(unpacked_memline_T *um, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
{
    textprop_T *prop;

    ଯଦି (um->buf == ଖାଲି || index < 0 || index >= um->prop_ଗଣନା)
	ଫେରନ୍ତୁ;
    ଯଦି (!um->detached && !um_detach(um))
	ଫେରନ୍ତୁ;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    ଯଦି (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    ଯଦି (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    ଯଦି (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
    ସ୍ଥିର char_u *
um_pack(unpacked_memline_T *um, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *packed_len)
{
    uint16_t	live_ଗଣନା = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vtext_size = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		total_len;
    char_u	*buf;
    char_u	*ଗଣନା_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < um->prop_ଗଣନା; ++i)
    {
	textprop_T *prop = &um->props[i];

	ଯଦି (prop->tp_flags & TP_FLAG_DELETED)
	    ଜାରି;
	++live_ଗଣନା;
	ଯଦି (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    ଯଦି (live_ଗଣନା == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	ଯଦି (buf == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	ଫେରନ୍ତୁ buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PROP_ଗଣନା_SIZE
		+ live_ଗଣନା * (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(textprop_T) + vtext_size;
    buf = alloc(total_len);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    ଗଣନା_dest = buf + um->text_size;
    mch_memmove(ଗଣନା_dest, &live_ଗଣନା, PROP_ଗଣନା_SIZE);

    prop_dest = ଗଣନା_dest + PROP_ଗଣନା_SIZE;
    vtext_dest = prop_dest + live_ଗଣନା * ଆକାର(textprop_T);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < um->prop_ଗଣନା; ++i)
    {
	textprop_T prop = um->props[i];

	ଯଦି (prop.tp_flags & TP_FLAG_DELETED)
	    ଜାରି;

	ଯଦି (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - ଗଣନା_dest);
	    vtext_dest += copysize;
	}
	ଅନ୍ୟଥା
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, ଆକାର(textprop_T));
	prop_dest += ଆକାର(textprop_T);
    }

    ଫେରନ୍ତୁ buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    packed_len;

    ଯଦି (!um->detached || um->buf == ଖାଲି || um->lnum == 0)
	ଫେରନ୍ତୁ;

    packed = um_pack(um, &packed_len);
    ଯଦି (packed == ଖାଲି)
    {
	um->buf = ଖାଲି;
	ଫେରନ୍ତୁ;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    ଯଦି (um->buf->b_ml.ml_ଧାଡ଼ି_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    ଅନ୍ୟଥା ଯଦି (!(um->buf->b_ml.ml_flags & ML_ଧାଡ଼ି_DIRTY))
	(ଶୂନ୍ୟ)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_ଗଣନା);

    ଯଦି (um->buf->b_ml.ml_flags & (ML_ଧାଡ଼ି_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_ଧାଡ଼ି_ptr);
    um->buf->b_ml.ml_ଧାଡ଼ି_ptr = packed;
    um->buf->b_ml.ml_ଧାଡ଼ି_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_ଧାଡ଼ି_DIRTY;

    um->detached = false;
    um->text = packed;
    um->text_size = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_ଧାଡ଼ି_textlen = um->text_size;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    ଶୂନ୍ୟ
um_close(unpacked_memline_T *um)
{
    ଯଦି (um->buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = ଖାଲି;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    ଶୂନ୍ୟ
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = ଖାଲି;
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
#define HI2PT(hi)      HIKEY2PT((hi)->hi_ଚାବି)

// The global text property types.
ସ୍ଥିର hashtab_T *global_proptypes = ଖାଲି;
ସ୍ଥିର proptype_T **global_proparray = ଖାଲି;

// The last used text property type ID.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    ସ୍ଥିର hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    ଯଦି (*name == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (buf == ଖାଲି)
	ht = global_proptypes;
    ଅନ୍ୟଥା
	ht = buf->b_proptypes;

    ଯଦି (ht == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    hi = hash_find(ht, name);
    ଯଦି (HASHITEM_EMPTY(hi))
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    ସ୍ଥିର proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    ଯଦି (hi == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    ଯଦି (pt == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    ସ୍ଥିର proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = find_prop_type(name, buf);

    ଯଦି (type == ଖାଲି)
	type = find_prop_type(name, ଖାଲି);
    ଯଦି (type == ଖାଲି)
	semsg(_(e_property_type_str_does_not_exist), name);
    ଫେରନ୍ତୁ type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    ଯଦି (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (arg->vval.v_dict == ଖାଲି)
	ଫେରନ୍ତୁ OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    ଯଦି (di != ଖାଲି && (di->di_tv.v_type != VAR_ସଂଖ୍ୟା
					      || di->di_tv.vval.v_ସଂଖ୍ୟା != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	ଯଦି (*buf == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    ଶୂନ୍ୟ
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    start_lnum = tv_get_ସଂଖ୍ୟା(&argvars[0]);
    start_col = tv_get_ସଂଖ୍ୟା(&argvars[1]);
    ଯଦି (check_for_dict_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id,
	char_u		*text_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_padding_ବାମ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		proplen;
    char_u	*props = ଖାଲି;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    ଯଦି (type == ଖାଲି)
	ଯାଅ theend;

    ଯଦି (start_lnum < 1 || start_lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	semsg(_(e_invalid_ଧାଡ଼ି_ସଂଖ୍ୟା_nr), (long)start_lnum);
	ଯାଅ theend;
    }
    ଯଦି (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	semsg(_(e_invalid_ଧାଡ଼ି_ସଂଖ୍ୟା_nr), (long)end_lnum);
	ଯାଅ theend;
    }

    ଯଦି (buf->b_ml.ml_mfp == ଖାଲି)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	ଯାଅ theend;
    }

    ଯଦି (text != ଖାଲି)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	ପାଇଁ (p = text; *p != NUL; MB_PTR_ADV(p))
	    ଯଦି (*p < ' ')
		*p = ' ';
    }

    ପାଇଁ (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	ଯଦି (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_ଧାଡ଼ି));
	    ଯାଅ theend;
	}

	ଯଦି (lnum == start_lnum)
	    col = start_col;
	ଅନ୍ୟଥା
	    col = 1;
	ଯଦି (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != ଖାଲି))
	{
	    semsg(_(e_invalid_column_ସଂଖ୍ୟା_nr), (long)start_col);
	    ଯାଅ theend;
	}
	sort_col = col;

	ଯଦି (lnum == end_lnum)
	    length = end_col - col;
	ଅନ୍ୟଥା
	    length = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)textlen - col + 1;
	ଯଦି (length > (long)textlen)
	    length = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)textlen;	// can include the end-of-line
	ଯଦି (length < 0)
	    length = 0;		// zero-width property

	ଯଦି (text_arg != ଖାଲି)
	{
	    length = (long)STRLEN(text_arg);
	    ଯଦି (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		ଯଦି ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	tmp_prop.tp_padleft = text_padding_ବାମ;
	ଯଦି (text_arg != ଖାଲି)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	ପାଇଁ (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * ଆକାର(textprop_T),
							   ଆକାର(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    ଯଦି (prop_col >= sort_col)
		ଭାଙ୍ଗ;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vtext_total = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_ଧାଡ଼ି_len;
	char_u	*ଗଣନା_dest;
	char_u	*vtext_dest;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;

	// Compute total vtext size from existing props.
	ପାଇଁ (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * ଆକାର(textprop_T),
							ଆକାର(textprop_T));
	    ଯଦି (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	ଯଦି (text_arg != ଖାଲି)
	    vtext_total += length + 1;

	new_ଧାଡ଼ି_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)textlen + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PROP_ଗଣନା_SIZE
			+ new_propcount * (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(textprop_T)
			+ vtext_total;
	newtext = alloc(new_ଧାଡ଼ି_len);
	ଯଦି (newtext == ଖାଲି)
	    ଯାଅ theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_ଧାଡ଼ି_ptr, textlen);

	// Write prop_count.
	ଗଣନା_dest = newtext + textlen;
	mch_memmove(ଗଣନା_dest, &new_propcount, PROP_ଗଣନା_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = ଗଣନା_dest + PROP_ଗଣନା_SIZE;
	ଯଦି (i > 0)
	    mch_memmove(newprops, props, ଆକାର(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	ଯଦି (i < proplen)
	    mch_memmove(newprops + (i + 1) * ଆକାର(textprop_T),
					props + i * ଆକାର(textprop_T),
					ଆକାର(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * ଆକାର(textprop_T);
	ପାଇଁ (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    ଯଦି (j == i)
		ଜାରି;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * ଆକାର(textprop_T),
		    ଆକାର(textprop_T));
	    ଯଦି (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_ଗଣନା_2 = (char_u *)props - PROP_ଗଣନା_SIZE;
		char_u *old_vtext = old_ଗଣନା_2 + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - ଗଣନା_dest);
		mch_memmove(newprops + j * ଆକାର(textprop_T), &ep,
			ଆକାର(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	ଯଦି (text_arg != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - ଗଣନା_dest);
	    vtext_dest += copysize;
	}
	ଅନ୍ୟଥା
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * ଆକାର(textprop_T), &tmp_prop,
							ଆକାର(textprop_T));

	ଯଦି (buf->b_ml.ml_flags & (ML_ଧାଡ଼ି_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_ଧାଡ଼ି_ptr);
	buf->b_ml.ml_ଧାଡ଼ି_ptr = newtext;
	buf->b_ml.ml_ଧାଡ଼ି_len = new_ଧାଡ଼ି_len;
	buf->b_ml.ml_flags |= ML_ଧାଡ଼ି_DIRTY;
    }

    changed_ଧାଡ଼ି_display_buf(buf);
    changed_ଧାଡ଼ିଗୁଡ଼ିକ_buf(buf, start_lnum, end_lnum + 1, 0);
    res = OK;

theend:
    vim_free(text);
    ଫେରନ୍ତୁ res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    ଶୂନ୍ୟ
f_prop_add_list(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_did_emsg = did_emsg;

    ଯଦି (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    dict = argvars[0].vval.v_dict;
    ଯଦି (dict == ଖାଲି || !dict_has_ଚାବି(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ଫେରନ୍ତୁ;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ଯଦି (dict_has_ଚାବି(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_ସଂଖ୍ୟା(dict, "id");
	ଯଦି (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ଫେରନ୍ତୁ;
	}
	id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)x;
    }

    ଯଦି (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ଫେରନ୍ତୁ;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(argvars[1].vval.v_list, li)
    {
	ଯଦି (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == ଖାଲି)
	{
	    emsg(_(e_list_required));
	    ଫେରନ୍ତୁ;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	ଯଦି (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	ଯଦି (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	ଯଦି (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_id = id;
	ଯଦି (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	ଯଦି (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    ଯଦି (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (prop_add_one(buf, type_name, this_id, ଖାଲି, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    ଫେରନ୍ତୁ;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_textprop_id(ଶୂନ୍ୟ)
{
    ଯଦି (vt_id_counter > 0)
	vt_id_counter = 0;
    ଫେରନ୍ତୁ --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id = 0;
    char_u	*text = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_padding_ବାମ = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;

    ଯଦି (dict == ଖାଲି || !dict_has_ଚାବି(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ଯାଅ theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ଯଦି (dict_has_ଚାବି(dict, "end_lnum"))
    {
	end_lnum = dict_get_ସଂଖ୍ୟା(dict, "end_lnum");
	ଯଦି (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "end_lnum");
	    ଯାଅ theend;
	}
    }
    ଅନ୍ୟଥା
	end_lnum = start_lnum;

    ଯଦି (dict_has_ଚାବି(dict, "length"))
    {
	long length = dict_get_ସଂଖ୍ୟା(dict, "length");

	ଯଦି (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "length");
	    ଯାଅ theend;
	}
	end_col = start_col + length;
    }
    ଅନ୍ୟଥା ଯଦି (dict_has_ଚାବି(dict, "end_col"))
    {
	end_col = dict_get_ସଂଖ୍ୟା(dict, "end_col");
	ଯଦି (end_col <= 0)
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "end_col");
	    ଯାଅ theend;
	}
    }
    ଅନ୍ୟଥା ଯଦି (start_lnum == end_lnum)
	end_col = start_col;
    ଅନ୍ୟଥା
	end_col = 1;

    ଯଦି (dict_has_ଚାବି(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_ସଂଖ୍ୟା(dict, "id");
	ଯଦି (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ଯାଅ theend;
	}
	id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)x;
    }

    ଯଦି (dict_has_ଚାବି(dict, "text"))
    {
	ଯଦି (dict_has_ଚାବି(dict, "length")
		|| dict_has_ଚାବି(dict, "end_col")
		|| dict_has_ଚାବି(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    ଯାଅ theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	ଯଦି (text == ଖାଲି)
	    ଯାଅ theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	ଯଦି (dict_has_ଚାବି(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    ଯଦି (p == ଖାଲି)
		ଯାଅ theend;
	    ଯଦି (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		ଯାଅ theend;
	    }
	    ଯଦି (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_ଡାହାଣ;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "text_align", p);
		ଯାଅ theend;
	    }
	}

	ଯଦି (dict_has_ଚାବି(dict, "text_padding_left"))
	{
	    text_padding_ବାମ = dict_get_ସଂଖ୍ୟା(dict, "text_padding_left");
	    ଯଦି (text_padding_ବାମ < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		ଯାଅ theend;
	    }
	}

	ଯଦି (dict_has_ଚାବି(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    ଯଦି (p == ଖାଲି)
		ଯାଅ theend;
	    ଯଦି (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "text_wrap", p);
		ଯାଅ theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    ଯଦି (start_col < (text == ଖାଲି ? 1 : 0))
    {
	semsg(_(e_invalid_column_ସଂଖ୍ୟା_nr), (long)start_col);
	ଯାଅ theend;
    }
    ଯଦି (start_col > 0 && text_padding_ବାମ > 0)
    {
	emsg(_(e_can_only_use_ବାମ_padding_when_column_is_zero));
	ଯାଅ theend;
    }

    ଯଦି (dict_arg != ଖାଲି && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	ଯାଅ theend;

    ଯଦି (text != ଖାଲି)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    ଅନ୍ୟଥା ଯଦି (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	ଯାଅ theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_ବାମ, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = ଖାଲି;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    ଫେରନ୍ତୁ id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, ପୂର୍ଣ୍ଣସଂଖ୍ୟା will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_ଗଣନା;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    ଯଦି ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_ଧାଡ଼ି_len - textlen;
    ଯଦି (propdata_len == 0)
	ଫେରନ୍ତୁ 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    ଯଦି (propdata_len < PROP_ଗଣନା_SIZE + ଆକାର(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	ଫେରନ୍ତୁ 0;
    }
    mch_memmove(&prop_ଗଣନା, text + textlen, PROP_ଗଣନା_SIZE);
    ଯଦି (!text_prop_ଗଣନା_valid(prop_ଗଣନା, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	ଫେରନ୍ତୁ 0;
    }
    ଯଦି (!text_prop_vtext_valid(text + textlen + PROP_ଗଣନା_SIZE,
					   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prop_ଗଣନା, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	ଫେରନ୍ତୁ 0;
    }
    *props = text + textlen + PROP_ଗଣନା_SIZE;
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prop_ଗଣନା;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prop_ଗଣନା_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = get_text_props(buf, lnum, &props, FALSE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result = 0;
    textprop_T	prop;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_ଡାହାଣ_goes_below = FALSE;

    ଯଦି (ଗଣନା_2 == 0)
	ଫେରନ୍ତୁ 0;
    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
    {
	mch_memmove(&prop, props + i * ଆକାର(prop), ଆକାର(prop));
	ଯଦି (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    ଯଦି ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_ଡାହାଣ_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_ଡାହାଣ)))
	    {
		++result;
	    }
	    ଅନ୍ୟଥା ଯଦି (prop.tp_flags & TP_FLAG_ALIGN_ଡାହାଣ)
		next_ଡାହାଣ_goes_below = TRUE;
	}
    }
    ଫେରନ୍ତୁ result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଗଣନା_props(linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା only_starting, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_ଧାଡ଼ି)
{
    char_u	*props;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		proplen = get_text_props(curbuf, lnum, &props, 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result = proplen;
    textprop_T	prop;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * ଆକାର(prop), ଆକାର(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	ଯଦି ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_ଧାଡ଼ି && prop.tp_col == MAXCOL))
	    --result;
    }
    ଫେରନ୍ତୁ result;
}

ସ୍ଥିର textprop_T	*text_prop_compare_props;
ସ୍ଥିର buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
text_prop_order(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ଯଦି (flags & TP_FLAG_ALIGN_ABOVE)
	ଫେରନ୍ତୁ 0;
    ଯଦି (flags & TP_FLAG_ALIGN_ଡାହାଣ)
	ଫେରନ୍ତୁ 2;
    ଯଦି (flags & TP_FLAG_ALIGN_BELOW)
	ଫେରନ୍ତୁ 3;
    ଫେରନ୍ତୁ 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
text_prop_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)s1;
    idx2 = *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    ଯଦି ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	ଫେରନ୍ତୁ tp1->tp_id < 0 ? 1 : -1;

    ଯଦି (col1 == MAXCOL || col2 == MAXCOL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା order1 = text_prop_order(tp1->tp_flags);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	ଯଦି (order1 != order2)
	    ଫେରନ୍ତୁ order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    ଯଦି (pt1 != pt2)
    {
	ଯଦି (pt1 == ଖାଲି)
	    ଫେରନ୍ତୁ -1;
	ଯଦି (pt2 == ଖାଲି)
	    ଫେରନ୍ତୁ 1;
	ଯଦି (pt1->pt_priority != pt2->pt_priority)
	    ଫେରନ୍ତୁ pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    ଯଦି (col1 != col2)
	ଫେରନ୍ତୁ col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    ଯଦି (tp1->tp_id < 0)
	ଫେରନ୍ତୁ tp1->tp_id > tp2->tp_id ? 1 : -1;

    ଫେରନ୍ତୁ 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    ଶୂନ୍ୟ
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *idxs,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((ଶୂନ୍ୟ *)idxs, (size_t)ଗଣନା_2, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_ଧାଡ଼ିଗୁଡ଼ିକ(
	win_T	    *wp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    type_id,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    ଯଦି (text_prop_type_by_id(wp->w_buffer, type_id) == ଖାଲି)
	ଫେରନ୍ତୁ false;

    ଯଦି (first_lnum < 1)
	first_lnum = 1;
    ଯଦି (last_lnum > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	last_lnum = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    ପାଇଁ (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଗଣନା_2 = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_2; ++i)
	{
	    mch_memmove(prop, props + i * ଆକାର(textprop_T),
							   ଆକାର(textprop_T));
	    ଯଦି (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		ଫେରନ୍ତୁ true;
	    }
	}
    }
    ଫେରନ୍ତୁ false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    type_id,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    ଫେରନ୍ତୁ find_prop_in_ଧାଡ଼ିଗୁଡ଼ିକ(wp, type_id, id, prop, found_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_text_props(linenr_T lnum, textprop_T *tps, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    char_u  *text;
    char_u  *newtext;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    ଯଦି (ଗଣନା_2 == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	ଯଦି (newtext == ଖାଲି)
	    ଫେରନ୍ତୁ;
	mch_memmove(newtext, text, textlen);
	ଯଦି (curbuf->b_ml.ml_flags & (ML_ଧାଡ଼ି_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ଧାଡ଼ି_ptr);
	curbuf->b_ml.ml_ଧାଡ଼ି_ptr = newtext;
	curbuf->b_ml.ml_ଧାଡ଼ି_len = textlen;
	curbuf->b_ml.ml_flags |= ML_ଧାଡ଼ି_DIRTY;
    }
    ଅନ୍ୟଥା
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_ଗଣନା = (uint16_t)ଗଣନା_2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    vtext_size = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    total_len;
	char_u	    *ଗଣନା_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

	ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
	    ଯଦି (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PROP_ଗଣନା_SIZE
		    + ଗଣନା_2 * (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	ଯଦି (newtext == ଖାଲି)
	    ଫେରନ୍ତୁ;
	mch_memmove(newtext, text, textlen);

	ଗଣନା_dest = newtext + textlen;
	mch_memmove(ଗଣନା_dest, &prop_ଗଣନା, PROP_ଗଣନା_SIZE);

	prop_dest = ଗଣନା_dest + PROP_ଗଣନା_SIZE;
	vtext_dest = prop_dest + ଗଣନା_2 * ଆକାର(textprop_T);

	ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
	{
	    textprop_T prop = tps[i];

	    ଯଦି (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = ଖାଲି;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - ଗଣନା_dest);
		vtext_dest += copysize;
	    }
	    ଅନ୍ୟଥା
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, ଆକାର(textprop_T));
	    prop_dest += ଆକାର(textprop_T);
	}

	ଯଦି (curbuf->b_ml.ml_flags & (ML_ଧାଡ଼ି_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_ଧାଡ଼ି_ptr);
	curbuf->b_ml.ml_ଧାଡ଼ି_ptr = newtext;
	curbuf->b_ml.ml_ଧାଡ଼ି_len = total_len;
	curbuf->b_ml.ml_flags |= ML_ଧାଡ଼ି_DIRTY;
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
props_add_ଗଣନା_header(char_u *ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି_len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା textlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *new_len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prop_bytes = ଧାଡ଼ି_len - textlen;
    uint16_t	prop_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vtext_size = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		total;
    char_u	*newline;
    char_u	*ଗଣନା_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (prop_bytes <= 0 || prop_bytes % ଆକାର(textprop_T) != 0)
    {
	*new_len = ଧାଡ଼ି_len;
	ଫେରନ୍ତୁ ଖାଲି;
    }
    prop_ଗଣନା = (uint16_t)(prop_bytes / ଆକାର(textprop_T));

    // Calculate total vtext size.
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prop_ଗଣନା; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ଧାଡ଼ି + textlen + i * ଆକାର(textprop_T),
							ଆକାର(textprop_T));
	ଯଦି (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PROP_ଗଣନା_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    ଯଦି (newline == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    *new_len = total;

    mch_memmove(newline, ଧାଡ଼ି, textlen);
    ଗଣନା_dest = newline + textlen;
    mch_memmove(ଗଣନା_dest, &prop_ଗଣନା, PROP_ଗଣନା_SIZE);
    prop_dest = ଗଣନା_dest + PROP_ଗଣନା_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prop_ଗଣନା; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, ଧାଡ଼ି + textlen + i * ଆକାର(textprop_T),
							ଆକାର(textprop_T));
	ଯଦି (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - ଗଣନା_dest);
	    vtext_dest += copysize;
	}
	ଅନ୍ୟଥା
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * ଆକାର(textprop_T), &prop,
							ଆକାର(textprop_T));
    }
    ଫେରନ୍ତୁ newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    ଶୂନ୍ୟ
add_text_props(linenr_T lnum, textprop_T *text_props, ପୂର୍ଣ୍ଣସଂଖ୍ୟା text_prop_ଗଣନା)
{
    unpacked_memline_T	um;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;

    um = um_open_at(curbuf, lnum, text_prop_ଗଣନା);
    ଯଦି (um.buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (!um_detach(&um))
    {
	um_abort(&um);
	ଫେରନ୍ତୁ;
    }

    // Grow props array if needed.
    ଯଦି (um.prop_ଗଣନା + text_prop_ଗଣନା > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_ଗଣନା + text_prop_ଗଣନା) * ଆକାର(textprop_T));
	ଯଦି (newprops == ଖାଲି)
	{
	    um_abort(&um);
	    ଫେରନ୍ତୁ;
	}
	um.props = newprops;
	um.prop_size = um.prop_ଗଣନା + text_prop_ଗଣନା;
    }

    ପାଇଁ (i = 0; i < text_prop_ଗଣନା; ++i)
	um.props[um.prop_ଗଣନା++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compare_pt(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    ଫେରନ୍ତୁ tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    ସ୍ଥିର proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ତଳ = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଉପର;

    ଯଦି (ht == ଖାଲି || ht->ht_used == 0)
	ଫେରନ୍ତୁ ଖାଲି;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    ଯଦି (*array == ଖାଲି)
    {
	long	    todo;
	hashitem_T  *hi;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	ଯଦି (*array == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
	{
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((ଶୂନ୍ୟ *)*array, ht->ht_used, ଆକାର(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    ଉପର = ht->ht_used;
    ଯେତେବେଳେ (ଉପର > ତଳ)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା m = (ଉପର + ତଳ) / 2;

	ଯଦି ((*array)[m]->pt_id == id)
	    ଫେରନ୍ତୁ (*array)[m];
	ଯଦି ((*array)[m]->pt_id > id)
	    ଉପର = m;
	ଅନ୍ୟଥା
	    ତଳ = m + 1;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା buflocal = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା virtualtext_prop = prop->tp_id < 0;

    dict_add_ସଂଖ୍ୟା(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    ଯଦି (!virtualtext_prop)
    {
	dict_add_ସଂଖ୍ୟା(dict, "length", prop->tp_len);
	dict_add_ସଂଖ୍ୟା(dict, "id", prop->tp_id);
    }
    dict_add_ସଂଖ୍ୟା(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_ସଂଖ୍ୟା(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    ଯଦି (pt == ଖାଲି)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    ଯଦି (pt != ଖାଲି)
	dict_add_string(dict, "type", pt->pt_name);

    ଯଦି (buflocal)
	dict_add_ସଂଖ୍ୟା(dict, "type_bufnr", buf->b_fnum);
    ଅନ୍ୟଥା
	dict_add_ସଂଖ୍ୟା(dict, "type_bufnr", 0);
    ଯଦି (virtualtext_prop)
    {
	string_T    text_align = {ଖାଲି, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	ଯଦି (prop->tp_flags & TP_FLAG_ALIGN_ଡାହାଣ)
	    STR_LITERAL_SET(text_align, "right");
	ଅନ୍ୟଥା ଯଦି (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	ଅନ୍ୟଥା ଯଦି (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	ଯଦି (text_align.string != ଖାଲି)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)text_align.length);

	// text_wrap
	ଯଦି (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	ଯଦି (prop->tp_padleft != 0)
	    dict_add_ସଂଖ୍ୟା(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    ଯଦି (type == ଖାଲି)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    ଫେରନ୍ତୁ type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    ଫେରନ୍ତୁ text_prop_type_by_id(buf, prop->tp_type) != ଖାଲି;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    ଶୂନ୍ୟ
f_prop_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    linenr_T start;
    linenr_T end;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    did_clear = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    start = tv_get_ସଂଖ୍ୟା(&argvars[0]);
    end = start;
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_ସଂଖ୍ୟା(&argvars[1]);
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ଯଦି (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		ଫେରନ୍ତୁ;
	}
    }
    ଯଦି (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	ଫେରନ୍ତୁ;
    }

    {
	unpacked_memline_T um = um_open(buf);

	ପାଇଁ (lnum = start; lnum <= end; ++lnum)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

	    ଯଦି (lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		ଭାଙ୍ଗ;
	    ଯଦି (!um_goto_ଧାଡ଼ି(&um, lnum, 0))
		ଭାଙ୍ଗ;
	    ଯଦି (um.prop_ଗଣନା > 0)
	    {
		did_clear = TRUE;
		ପାଇଁ (idx = um.prop_ଗଣନା - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    ଯଦି (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    ଶୂନ୍ୟ
f_prop_find(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_pos_has_prop = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seen_end = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id_found = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type_id = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skipstart = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		both;

    ଯଦି (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;
    dict = argvars[0].vval.v_dict;

    ଯଦି (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (buf->b_ml.ml_mfp == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	ଯଦି (*dir_s == 'b')
	    dir = BACKWARD;
	ଅନ୍ୟଥା ଯଦି (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    ଫେରନ୍ତୁ;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    ଯଦି (di != ଖାଲି)
	lnum = tv_get_ସଂଖ୍ୟା(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    ଯଦି (di != ଖାଲି)
	col = tv_get_ସଂଖ୍ୟା(&di->di_tv);

    ଯଦି (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    ଅନ୍ୟଥା ଯଦି (col == -1)
	col = 1;

    ଯଦି (lnum < 1 || lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	emsg(_(e_invalid_range));
	ଫେରନ୍ତୁ;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    ଯଦି (dict_has_ଚାବି(dict, "id"))
    {
	id = dict_get_ସଂଖ୍ୟା(dict, "id");
	id_found = TRUE;
    }
    ଯଦି (dict_has_ଚାବି(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ଯଦି (type == ଖାଲି)
	    ଫେରନ୍ତୁ;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    ଯଦି (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ଫେରନ୍ତୁ;
    }

    lnum_start = lnum;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯେତେବେଳେ (1)
    {
	char_u	*prop_start_ptr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଗଣନା_2 = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
	textprop_T  prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prop_start;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prop_end;

	ପାଇଁ (i = dir == BACKWARD ? ଗଣନା_2 - 1 : 0; i >= 0 && i < ଗଣନା_2; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * ଆକାର(textprop_T),
							   ଆକାର(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    ଯଦି (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_ଗଣନା_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    ଅନ୍ୟଥା
		prop.u.tp_text = ଖାଲି;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    ଯଦି (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		ଯଦି (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    ଯଦି (!is_floating_vtext && prop.tp_col > col)
			ଜାରି;
		}
		ଅନ୍ୟଥା ଯଦି (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    ଜାରି;
	    }
	    ଯଦି (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		ଯଦି (lnum_start == lnum
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
		ଯଦି (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		ଯଦି (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    ଯଦି (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    ଜାରି;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		ଯଦି (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    ଜାରି;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_ସଂଖ୍ୟା(rettv->vval.v_dict, "lnum", lnum);

		ଫେରନ୍ତୁ;
	    }
	}

	ଯଦି (dir > 0)
	{
	    ଯଦି (lnum >= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		ଭାଙ୍ଗ;
	    lnum++;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (lnum <= 1)
		ଭାଙ୍ଗ;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prop_type_or_id_in_list(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *types_or_ids, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_or_id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < len; i++)
	ଯଦି (types_or_ids[i] == type_or_id)
	    ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ FALSE;
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
    ସ୍ଥିର ଶୂନ୍ୟ
get_props_in_ଧାଡ଼ି(
	buf_T		*buf,
	linenr_T	lnum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*prop_types,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prop_types_len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*prop_ids,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prop_ids_len,
	list_T		*retlist,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_lnum)
{
    char_u	*props;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    textprop_T	prop;

    ଗଣନା_2 = get_text_props(buf, lnum, &props, FALSE);
    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
    {
	mch_memmove(&prop, props + i * ଆକାର(textprop_T),
		ଆକାର(textprop_T));
	// Convert offset to pointer for virtual text props.
	ଯଦି (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_ଗଣନା_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ଅନ୍ୟଥା
	    prop.u.tp_text = ଖାଲି;
	ଯଦି ((prop_types == ଖାଲି
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == ଖାଲି
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    ଯଦି (d == ଖାଲି)
		ଭାଙ୍ଗ;
	    prop_fill_dict(d, &prop, buf);
	    ଯଦି (add_lnum)
		dict_add_ସଂଖ୍ୟା(d, "lnum", lnum);
	    ଯଦି (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *
get_prop_types_from_names(list_T *l, buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_types)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*prop_types;
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*name;
    proptype_T	*type;

    *ଅଙ୍କ_types = 0;

    prop_types = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, list_len(l));
    ଯଦି (prop_types == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    i = 0;
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	ଯଦି (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    ଯାଅ errret;
	}
	name = li->li_tv.vval.v_string;
	ଯଦି (name == ଖାଲି)
	    ଯାଅ errret;

	type = lookup_prop_type(name, buf);
	ଯଦି (type == ଖାଲି)
	    ଯାଅ errret;
	prop_types[i++] = type->pt_id;
    }

    *ଅଙ୍କ_types = i;
    ଫେରନ୍ତୁ prop_types;

errret:
    VIM_CLEAR(prop_types);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *
get_prop_ids_from_list(list_T *l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_ids)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*prop_ids;
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error;

    *ଅଙ୍କ_ids = 0;

    prop_ids = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, list_len(l));
    ଯଦି (prop_ids == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	error = FALSE;
	id = tv_get_ସଂଖ୍ୟା_chk(&li->li_tv, &error);
	ଯଦି (error)
	    ଯାଅ errret;

	prop_ids[i++] = id;
    }

    *ଅଙ୍କ_ids = i;
    ଫେରନ୍ତୁ prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    ଶୂନ୍ୟ
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    buf_T	*buf = curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_lnum = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*prop_types = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prop_types_len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*prop_ids = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    // default: get text properties on current line
    start_lnum = tv_get_ସଂଖ୍ୟା(&argvars[0]);
    end_lnum = start_lnum;
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ଯଦି (check_for_dict_arg(argvars, 1) == FAIL)
	    ଫେରନ୍ତୁ;
	d = argvars[1].vval.v_dict;

	ଯଦି (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ଫେରନ୍ତୁ;

	ଯଦି (d != ଖାଲି && (di = dict_find(d, (char_u *)"end_lnum", -1)) != ଖାଲି)
	{
	    ଯଦି (di->di_tv.v_type != VAR_ସଂଖ୍ୟା)
	    {
		emsg(_(e_ସଂଖ୍ୟା_required));
		ଫେରନ୍ତୁ;
	    }
	    end_lnum = tv_get_ସଂଖ୍ୟା(&di->di_tv);
	    ଯଦି (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//ଧାଡ଼ି
		end_lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + end_lnum + 1;
	    ଅନ୍ୟଥା ଯଦି (end_lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		end_lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    add_lnum = TRUE;
	}
	ଯଦି (d != ଖାଲି && (di = dict_find(d, (char_u *)"types", -1)) != ଖାଲି)
	{
	    ଯଦି (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ଫେରନ୍ତୁ;
	    }

	    l = di->di_tv.vval.v_list;
	    ଯଦି (l != ଖାଲି && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		ଯଦି (prop_types == ଖାଲି)
		    ଫେରନ୍ତୁ;
	    }
	}
	ଯଦି (d != ଖାଲି && (di = dict_find(d, (char_u *)"ids", -1)) != ଖାଲି)
	{
	    ଯଦି (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ଯାଅ errret;
	    }

	    l = di->di_tv.vval.v_list;
	    ଯଦି (l != ଖାଲି && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		ଯଦି (prop_ids == ଖାଲି)
		    ଯାଅ errret;
	    }
	}
    }
    ଯଦି (start_lnum < 1 || start_lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    ଅନ୍ୟଥା
	ପାଇଁ (lnum = start_lnum; lnum <= end_lnum; lnum++)
	    get_props_in_ଧାଡ଼ି(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, add_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    ଶୂନ୍ୟ
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start = 1;
    linenr_T	end = 0;
    linenr_T	lnum;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_all;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id = -MAXCOL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type_id = -1;	    // for a single "type"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*type_ids = ଖାଲି;   // array, for a list of "types", allocated
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_type_ids = 0;   // number of elements in "type_ids"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		both;

    rettv->vval.v_ସଂଖ୍ୟା = 0;

    ଯଦି (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_ସଂଖ୍ୟା(&argvars[1]);
	end = start;
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_ସଂଖ୍ୟା(&argvars[2]);
	ଯଦି (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    ଫେରନ୍ତୁ;
	}
    }

    dict = argvars[0].vval.v_dict;
    ଯଦି (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (buf->b_ml.ml_mfp == ଖାଲି)
	ଫେରନ୍ତୁ;

    do_all = dict_get_bool(dict, "all", FALSE);

    ଯଦି (dict_has_ଚାବି(dict, "id"))
	id = dict_get_ସଂଖ୍ୟା(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    ଯଦି (dict_has_ଚାବି(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ଯଦି (type == ଖାଲି)
	    ଫେରନ୍ତୁ;
	type_id = type->pt_id;
    }
    ଯଦି (dict_has_ଚାବି(dict, "types"))
    {
	typval_T types;
	listitem_T *li = ଖାଲି;

	dict_get_tv(dict, "types", &types);
	ଯଦି (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		ଯଦି (li->li_tv.v_type != VAR_STRING)
		    ଜାରି;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		ଯଦି (!prop_type)
		    ଯାଅ cleanup_prop_remove;

		type_ids[ଅଙ୍କ_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    ଯଦି (id == -MAXCOL && (type_id == -1 && ଅଙ୍କ_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	ଯାଅ cleanup_prop_remove;
    }
    ଯଦି (both && (id == -MAXCOL || (type_id == -1 && ଅଙ୍କ_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ଯାଅ cleanup_prop_remove;
    }
    ଯଦି (type_id != -1 && ଅଙ୍କ_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	ଯାଅ cleanup_prop_remove;
    }

    ଯଦି (end == 0)
	end = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    {
	unpacked_memline_T um = um_open(buf);

	ପାଇଁ (lnum = start; lnum <= end; ++lnum)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

	    ଯଦି (lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		ଭାଙ୍ଗ;
	    ଯଦି (!um_goto_ଧାଡ଼ି(&um, lnum, 0))
		ଭାଙ୍ଗ;

	    ପାଇଁ (idx = 0; idx < um.prop_ଗଣନା; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    matches_id = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    matches_type = 0;

		ଯଦି (prop->tp_flags & TP_FLAG_DELETED)
		    ଜାରି;

		matches_id = prop->tp_id == id;
		ଯଦି (ଅଙ୍କ_type_ids > 0)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx2;

		    ପାଇଁ (idx2 = 0; !matches_type && idx2 < ଅଙ୍କ_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		ଅନ୍ୟଥା
		    matches_type = prop->tp_type == type_id;

		ଯଦି (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    ଯଦି (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_ସଂଖ୍ୟା;
		    ଯଦି (!do_all)
			ଭାଙ୍ଗ;
		}
	    }
	}
	um_close(&um);
    }

    ଯଦି (first_changed > 0)
    {
	changed_ଧାଡ଼ି_display_buf(buf);
	changed_ଧାଡ଼ିଗୁଡ଼ିକ_buf(buf, first_changed, last_changed + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prop_type_set(typval_T *argvars, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add)
{
    char_u	*name;
    buf_T	*buf = ଖାଲି;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);
    ଯଦି (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ଫେରନ୍ତୁ;
    }

    ଯଦି (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	ଫେରନ୍ତୁ;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    ଯଦି (add)
    {
	hashtab_T **htp;

	ଯଦି (prop != ଖାଲି)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    ଫେରନ୍ତୁ;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	ଯଦି (prop == ଖାଲି)
	    ଫେରନ୍ତୁ;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	ଯଦି (buf == ଖାଲି)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	ଅନ୍ୟଥା
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	ଯଦି (*htp == ଖାଲି)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    ଯଦି (*htp == ଖାଲି)
	    {
		vim_free(prop);
		ଫେରନ୍ତୁ;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (prop == ଖାଲି)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    ଫେରନ୍ତୁ;
	}
    }

    ଯଦି (dict != ଖାଲି)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	ଯଦି (di != ଖାଲି)
	{
	    char_u	*highlight;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    ଯଦି (highlight != ଖାଲି && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    ଯଦି (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == ଖାଲି ? (char_u *)"" : highlight);
		ଫେରନ୍ତୁ;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	ଯଦି (di != ଖାଲି)
	{
	    ଯଦି (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    ଅନ୍ୟଥା
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	ଯଦି (di != ଖାଲି)
	{
	    ଯଦି (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    ଅନ୍ୟଥା
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	ଯଦି (di != ଖାଲି)
	    prop->pt_priority = tv_get_ସଂଖ୍ୟା(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	ଯଦି (di != ଖାଲି)
	{
	    ଯଦି (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    ଅନ୍ୟଥା
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	ଯଦି (di != ଖାଲି)
	{
	    ଯଦି (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    ଅନ୍ୟଥା
		prop->pt_flags &= ~PT_FLAG_INS_END_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    ଶୂନ୍ୟ
f_prop_type_add(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    ଶୂନ୍ୟ
f_prop_type_change(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    ଶୂନ୍ୟ
f_prop_type_delete(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*name;
    buf_T	*buf = ଖାଲି;
    hashitem_T	*hi;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);
    ଯଦି (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ଫେରନ୍ତୁ;
    }

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	ଯଦି (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ଫେରନ୍ତୁ;
    }

    hi = find_prop_type_hi(name, buf);
    ଯଦି (hi == ଖାଲି)
	ଫେରନ୍ତୁ;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    ଯଦି (buf == ଖାଲି)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    ଅନ୍ୟଥା
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    changed_window_setting_buf(buf == ଖାଲି ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    ଶୂନ୍ୟ
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);
    ଯଦି (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ଫେରନ୍ତୁ;
    }

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    proptype_T  *prop = ଖାଲି;
    buf_T	    *buf = ଖାଲି;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	ଯଦି (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ଫେରନ୍ତୁ;
    }

    prop = find_prop_type(name, buf);
    ଯଦି (prop == ଖାଲି)
	ଫେରନ୍ତୁ;

    dict_T *d = rettv->vval.v_dict;

    ଯଦି (prop->pt_hl_id > 0)
	dict_add_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_add_ସଂଖ୍ୟା(d, "priority", prop->pt_priority);
    dict_add_ସଂଖ୍ୟା(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_add_ସଂଖ୍ୟା(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_add_ସଂଖ୍ୟା(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_START_INCL) ? 1 : 0);
    dict_add_ସଂଖ୍ୟା(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_END_INCL) ? 1 : 0);
    ଯଦି (buf != ଖାଲି)
	dict_add_ସଂଖ୍ୟା(d, "bufnr", buf->b_fnum);
}

    ସ୍ଥିର ଶୂନ୍ୟ
list_types(hashtab_T *ht, list_T *l)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
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
    ଶୂନ୍ୟ
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ଖାଲି;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	ଯଦି (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    ଫେରନ୍ତୁ;
    }
    ଯଦି (buf == ଖାଲି)
    {
	ଯଦି (global_proptypes != ଖାଲି)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    ଅନ୍ୟଥା ଯଦି (buf->b_proptypes != ଖାଲି)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_ht_prop_types(hashtab_T *ht)
{
    long	todo;
    hashitem_T	*hi;

    ଯଦି (ht == ଖାଲି)
	ଫେରନ୍ତୁ;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#ଯଦି defined(EXITFREE)
/*
 * Free all global property types.
 */
    ଶୂନ୍ୟ
clear_global_prop_types(ଶୂନ୍ୟ)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = ଖାଲି;
    VIM_CLEAR(global_proparray);
}
#endif

/*
 * Free all property types for "buf".
 */
    ଶୂନ୍ୟ
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = ଖାଲି;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirty;	    // if the property was changed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା can_drop;   // whether after this change, the prop may be removed
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
    ସ୍ଥିର adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    col,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    added,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags)
{
    proptype_T	*pt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_incl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_incl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    ଯଦି (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	ଫେରନ୍ତୁ res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != ଖାଲି && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    ଯଦି (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != ଖାଲି && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    ଯଦି (added > 0)
    {
	ଯଦି (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	ଅନ୍ୟଥା ଯଦି (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    ଅନ୍ୟଥା ଯଦି (prop->tp_col > col + 1)
    {
	ଯଦି (prop->tp_col + added < col + 1)
	{
	    ଯଦି (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		ଯଦି (prop->tp_len <= 0)
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
	ଅନ୍ୟଥା
	    prop->tp_col += added;
    }
    ଅନ୍ୟଥା ଯଦି (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	ଯଦି (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	ଯଦି (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    ଅନ୍ୟଥା
	res.dirty = FALSE;

    ଫେରନ୍ତୁ res;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    bytes_added,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags)
{
    unpacked_memline_T um;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dirty = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ri;

    ଯଦି (text_prop_frozen > 0)
	ଫେରନ୍ତୁ FALSE;

    um = um_open_at(curbuf, lnum, 0);
    ଯଦି (um.buf == ଖାଲି || um.prop_ଗଣନା == 0)
    {
	um_abort(&um);
	ଫେରନ୍ତୁ FALSE;
    }

    ପାଇଁ (ri = 0; ri < um.prop_ଗଣନା; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	ଯଦି (res.dirty)
	{
	    ଯଦି (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		ଯଦି (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    ଫେରନ୍ତୁ FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		ଯଦି ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    ଫେରନ୍ତୁ FALSE;
		}
	    }
	    dirty = TRUE;
	}
	ଯଦି (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    ଜାରି;
	}
    }
    ଯଦି (dirty)
	um_close(&um);
    ଅନ୍ୟଥା
	um_abort(&um);
    ଫେରନ୍ତୁ dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    ଶୂନ୍ୟ
adjust_props_for_split(
	linenr_T    lnum_props,
	linenr_T    lnum_ଶୀର୍ଷ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    kept,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    deleted,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    at_eol)
{
    char_u	*props;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    garray_T    prevprop;
    garray_T    nextprop;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skipped = kept + deleted;

    ଯଦି (!curbuf->b_has_textprop)
	ଫେରନ୍ତୁ;

    // Get the text properties from "lnum_props".
    ଗଣନା_2 = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, ଆକାର(textprop_T), 10);
    ga_init2(&nextprop, ଆକାର(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *ଗଣନା_ptr = props - PROP_ଗଣନା_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_2; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    start_incl, end_incl;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cont_prev, cont_next;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * ଆକାର(textprop_T), ଆକାର(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	ଯଦି (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(ଗଣନା_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	ଅନ୍ୟଥା
	    prop.u.tp_text = ଖାଲି;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != ଖାଲି && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != ଖାଲି && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	ଯଦି (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	ଅନ୍ୟଥା
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    ଯଦି (is_floating_vtext)
		cont_next = FALSE;
	    ଅନ୍ୟଥା
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	ଯଦି (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	ଯଦି (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    ଯଦି (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    ଯଦି (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	ଯଦି (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    ଯଦି (p->tp_col != MAXCOL)
	    {
		ଯଦି (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		ଅନ୍ୟଥା
		{
		    ଯଦି (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    ଯଦି (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    set_text_props(lnum_ଶୀର୍ଷ, (textprop_T *)prevprop.ga_data,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    set_text_props(lnum_ଶୀର୍ଷ + 1, (textprop_T *)nextprop.ga_data,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    ଶୂନ୍ୟ
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    last_ଧାଡ଼ି,
	long		    col,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    removed)
{
    unpacked_memline_T	r_um;

    ଯଦି (um->buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    r_um = um_open_at(um->buf, lnum, 0);
    ଯଦି (r_um.buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (!um_detach(&r_um))
    {
	um_abort(&r_um);
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = r_um.prop_ଗଣନା - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    end;

	ଯଦି (prop->tp_col == MAXCOL && !last_ଧାଡ଼ି)
	    ଜାରି;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	ଯଦି (last_ଧାଡ଼ି || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = ଖାଲି;  // ownership transferred
	}
	ଅନ୍ୟଥା
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < um->prop_ଗଣନା; ++j)
	    {
		textprop_T *op = &um->props[j];

		ଯଦି ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    ଭାଙ୍ଗ;
		}
	    }
	    ଯଦି (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
remember_cleared_prop(garray_T *gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା which, textprop_T *prop, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    undoprop_T	*up;

    ଯଦି (which == UNDOPROP_NONE)
	ଫେରନ୍ତୁ;
    ଯଦି (gap->ga_itemsize == 0)
	ga_init2(gap, ଆକାର(undoprop_T), 4);
    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ;
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
    ଶୂନ୍ୟ
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    above_which,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = ଖାଲି;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    ଯଦି (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	ଫେରନ୍ତୁ;

    above = um_open_at(buf, lnum, 0);
    ଯଦି (above.buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < above.prop_ଗଣନା; ++i)
	ଯଦି (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    ଯଦି (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	ଫେରନ୍ତୁ;
    }

    below = um_open_at(buf, lnum + 1, 0);
    ଯଦି (below.buf == ଖାଲି)
    {
	um_abort(&above);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (below.prop_ଗଣନା > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_ଗଣନା);
	ଯଦି (paired == ଖାଲି)
	    ଯାଅ theend;
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < above.prop_ଗଣନା; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	ଯଦି (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    ଜାରି;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < below.prop_ଗଣନା; ++j)
	{
	    textprop_T *next = &below.props[j];

	    ଯଦି (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < below.prop_ଗଣନା; ++j)
    {
	ଯଦି (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    ଜାରି;
	ଯଦି (!below.detached && !um_detach(&below))
	    ଭାଙ୍ଗ;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    ଯଦି (below_changed)
	um_close(&below);
    ଅନ୍ୟଥା
	um_abort(&below);
    ଯଦି (above_changed)
	um_close(&above);
    ଅନ୍ୟଥା
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    ଶୂନ୍ୟ
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    ଯଦି (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	ଫେରନ୍ତୁ;
    um = um_open_at(buf, lnum, 0);
    ଯଦି (um.buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	ଯଦି (up->up_which != which)
	    ଜାରି;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < um.prop_ଗଣନା; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    ଯଦି (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		ଜାରି;
	    ଯଦି (!um.detached && !um_detach(&um))
		ଯାଅ theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    ଭାଙ୍ଗ;
	}
    }

theend:
    ଯଦି (changed)
	um_close(&um);
    ଅନ୍ୟଥା
	um_abort(&um);
}

    bool
text_prop_ଗଣନା_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା prop_ଗଣନା, size_t propdata_len)
{
    ଯଦି (propdata_len < PROP_ଗଣନା_SIZE)
	ଫେରନ୍ତୁ false;
    ଫେରନ୍ତୁ (size_t)prop_ଗଣନା * ଆକାର(textprop_T)
		    <= propdata_len - PROP_ଗଣନା_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prop_ଗଣନା, size_t propdata_len)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < prop_ଗଣନା; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * ଆକାର(textprop_T),
							  ଆକାର(textprop_T));
	ଯଦି (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    ଜାରି;
	ଯଦି (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    ଫେରନ୍ତୁ false;
    }
    ଫେରନ୍ତୁ true;
}

#endif // FEAT_PROP_POPUP
