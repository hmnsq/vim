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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_PROP_POPUP)

સ્થિર શૂન્ય um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    સ્થિર શૂન્ય
um_free_vtext(textprop_T *props, પૂર્ણાંક ગણતરી)
{
    માટે (પૂર્ણાંક i = 0; i < ગણતરી; ++i)
	જો (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    સ્થિર શૂન્ય
um_free(unpacked_memline_T *um)
{
    જો (um->detached)
    {
	VIM_CLEAR(um->text);
	um_free_vtext(um->props, um->prop_ગણતરી);
    }
    VIM_CLEAR(um->props);
    um->prop_size = 0;
    um->prop_ગણતરી = 0;
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
    પાછા um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_લીટી(unpacked_memline_T *um, linenr_T lnum, પૂર્ણાંક extra_props)
{
    char_u	*લીટી;
    size_t	textlen;
    size_t	propdata_len;
    પૂર્ણાંક		proplen;

    જો (um->buf == ખાલી)
	પાછા false;

    // Store changes to the current line if any.
    જો (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    જો (lnum == 0)
	પાછા true;	// just unload

    લીટી = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_લીટી_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = લીટી;

    જો (propdata_len == 0)
	પાછા true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    જો (propdata_len < PROP_ગણતરી_SIZE + કદ(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ખાલી;
	પાછા false;
    }

    uint16_t    prop_ગણતરી;
    char_u	    *ગણતરી_ptr = લીટી + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_ગણતરી, ગણતરી_ptr, PROP_ગણતરી_SIZE);
    જો (!text_prop_ગણતરી_valid(prop_ગણતરી, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ખાલી;
	પાછા false;
    }
    proplen = (પૂર્ણાંક)prop_ગણતરી;
    props_start = ગણતરી_ptr + PROP_ગણતરી_SIZE;

    જો (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = ખાલી;
	પાછા false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    જો (um->props == ખાલી)
    {
	um->buf = ખાલી;
	પાછા false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_ગણતરી = proplen;
    mch_memmove(um->props, props_start, proplen * કદ(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    માટે (પૂર્ણાંક i = 0; i < proplen; ++i)
    {
	જો (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = ગણતરી_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	નહીંતર
	    um->props[i].u.tp_text = ખાલી;
    }

    પાછા true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, પૂર્ણાંક extra_props)
{
    unpacked_memline_T um = um_open(buf);

    જો (!um_goto_લીટી(&um, lnum, extra_props))
	um.buf = ખાલી;
    પાછા um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    સ્થિર bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    જો (um->detached || um->buf == ખાલી)
	પાછા um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    જો (newtext == ખાલી)
    {
	um->buf = ખાલી;
	પાછા false;
    }
    um->text = newtext;

    માટે (પૂર્ણાંક i = 0; i < um->prop_ગણતરી; ++i)
    {
	જો (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    જો (copy == ખાલી)
	    {
		માટે (પૂર્ણાંક j = 0; j < i; ++j)
		    જો (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = ખાલી;
		પાછા false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    પાછા true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    જો (um->buf == ખાલી)
	પાછા false;
    જો (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    પાછા true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    સ્થિર bool
um_grow_props(unpacked_memline_T *um, પૂર્ણાંક needed)
{
    જો (um->prop_ગણતરી + needed <= um->prop_size)
	પાછા true;

    પૂર્ણાંક	    new_size = um->prop_ગણતરી + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * કદ(textprop_T));
    જો (newprops == ખાલી)
	પાછા false;
    um->props = newprops;
    um->prop_size = new_size;

    પાછા true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    સ્થિર શૂન્ય
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    જો (um->buf == ખાલી)
	પાછા;
    જો (!um->detached && !um_detach(um))
	પાછા;
    જો (!um_grow_props(um, 1))
    {
	um->buf = ખાલી;
	પાછા;
    }
    um->props[um->prop_ગણતરી++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    શૂન્ય
um_reverse_props(unpacked_memline_T *um)
{
    માટે (પૂર્ણાંક i = 0, j = um->prop_ગણતરી - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, પૂર્ણાંક prop_ગણતરી)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    જો (lnum < 1 || lnum > buf->b_ml.ml_લીટી_ગણતરી)
    {
	um.buf = ખાલી;
	પાછા um;
    }

    char_u *લીટી = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(લીટી);

    જો (text_copy == ખાલી)
    {
	um.buf = ખાલી;
	પાછા um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    જો (prop_ગણતરી > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_ગણતરી);
	જો (um.props == ખાલી)
	{
	    vim_free(um.text);
	    um.buf = ખાલી;
	    પાછા um;
	}
	um.prop_size = prop_ગણતરી;
    }

    પાછા um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    સ્થિર શૂન્ય
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					પૂર્ણાંક tp_id, પૂર્ણાંક tp_type, પૂર્ણાંક flag)
{
    unpacked_memline_T	neighbor;

    જો (lnum < 1 || lnum > buf->b_ml.ml_લીટી_ગણતરી)
	પાછા;

    neighbor = um_open_at(buf, lnum, 0);
    જો (neighbor.buf == ખાલી)
	પાછા;

    માટે (પૂર્ણાંક i = 0; i < neighbor.prop_ગણતરી; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	જો (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    જો (!neighbor.detached && !um_detach(&neighbor))
		તોડો;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    તોડો;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    શૂન્ય
um_delete_prop(unpacked_memline_T *um, પૂર્ણાંક index)
{
    textprop_T *prop;

    જો (um->buf == ખાલી || index < 0 || index >= um->prop_ગણતરી)
	પાછા;
    જો (!um->detached && !um_detach(um))
	પાછા;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    જો (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    જો (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    જો (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
    સ્થિર char_u *
um_pack(unpacked_memline_T *um, પૂર્ણાંક *packed_len)
{
    uint16_t	live_ગણતરી = 0;
    પૂર્ણાંક		vtext_size = 0;
    પૂર્ણાંક		total_len;
    char_u	*buf;
    char_u	*ગણતરી_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    માટે (પૂર્ણાંક i = 0; i < um->prop_ગણતરી; ++i)
    {
	textprop_T *prop = &um->props[i];

	જો (prop->tp_flags & TP_FLAG_DELETED)
	    ચાલુ;
	++live_ગણતરી;
	જો (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    જો (live_ગણતરી == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	જો (buf == ખાલી)
	    પાછા ખાલી;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	પાછા buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (પૂર્ણાંક)PROP_ગણતરી_SIZE
		+ live_ગણતરી * (પૂર્ણાંક)કદ(textprop_T) + vtext_size;
    buf = alloc(total_len);
    જો (buf == ખાલી)
	પાછા ખાલી;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    ગણતરી_dest = buf + um->text_size;
    mch_memmove(ગણતરી_dest, &live_ગણતરી, PROP_ગણતરી_SIZE);

    prop_dest = ગણતરી_dest + PROP_ગણતરી_SIZE;
    vtext_dest = prop_dest + live_ગણતરી * કદ(textprop_T);

    માટે (પૂર્ણાંક i = 0; i < um->prop_ગણતરી; ++i)
    {
	textprop_T prop = um->props[i];

	જો (prop.tp_flags & TP_FLAG_DELETED)
	    ચાલુ;

	જો (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    પૂર્ણાંક copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - ગણતરી_dest);
	    vtext_dest += copysize;
	}
	નહીંતર
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, કદ(textprop_T));
	prop_dest += કદ(textprop_T);
    }

    પાછા buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    સ્થિર શૂન્ય
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    પૂર્ણાંક	    packed_len;

    જો (!um->detached || um->buf == ખાલી || um->lnum == 0)
	પાછા;

    packed = um_pack(um, &packed_len);
    જો (packed == ખાલી)
    {
	um->buf = ખાલી;
	પાછા;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    જો (um->buf->b_ml.ml_લીટી_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    નહીંતર જો (!(um->buf->b_ml.ml_flags & ML_લીટી_DIRTY))
	(શૂન્ય)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_ગણતરી);

    જો (um->buf->b_ml.ml_flags & (ML_લીટી_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_લીટી_ptr);
    um->buf->b_ml.ml_લીટી_ptr = packed;
    um->buf->b_ml.ml_લીટી_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_લીટી_DIRTY;

    um->detached = false;
    um->text = packed;
    um->text_size = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_લીટી_textlen = um->text_size;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    શૂન્ય
um_close(unpacked_memline_T *um)
{
    જો (um->buf == ખાલી)
	પાછા;
    જો (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = ખાલી;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    શૂન્ય
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = ખાલી;
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
#define HI2PT(hi)      HIKEY2PT((hi)->hi_ચાવી)

// The global text property types.
સ્થિર hashtab_T *global_proptypes = ખાલી;
સ્થિર proptype_T **global_proparray = ખાલી;

// The last used text property type ID.
સ્થિર પૂર્ણાંક proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    સ્થિર hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    જો (*name == NUL)
	પાછા ખાલી;
    જો (buf == ખાલી)
	ht = global_proptypes;
    નહીંતર
	ht = buf->b_proptypes;

    જો (ht == ખાલી)
	પાછા ખાલી;
    hi = hash_find(ht, name);
    જો (HASHITEM_EMPTY(hi))
	પાછા ખાલી;
    પાછા hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    સ્થિર proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    જો (hi == ખાલી)
	પાછા ખાલી;
    પાછા HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    પૂર્ણાંક
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    જો (pt == ખાલી)
	પાછા 0;
    પાછા pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    સ્થિર proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = find_prop_type(name, buf);

    જો (type == ખાલી)
	type = find_prop_type(name, ખાલી);
    જો (type == ખાલી)
	semsg(_(e_property_type_str_does_not_exist), name);
    પાછા type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    સ્થિર પૂર્ણાંક
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    જો (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	પાછા FAIL;
    }
    જો (arg->vval.v_dict == ખાલી)
	પાછા OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    જો (di != ખાલી && (di->di_tv.v_type != VAR_સંખ્યા
					      || di->di_tv.vval.v_સંખ્યા != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	જો (*buf == ખાલી)
	    પાછા FAIL;
    }
    પાછા OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    શૂન્ય
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	પાછા;

    start_lnum = tv_get_સંખ્યા(&argvars[0]);
    start_col = tv_get_સંખ્યા(&argvars[1]);
    જો (check_for_dict_arg(argvars, 2) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    સ્થિર પૂર્ણાંક
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	પૂર્ણાંક		id,
	char_u		*text_arg,
	પૂર્ણાંક		text_padding_ડાબું,
	પૂર્ણાંક		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    પૂર્ણાંક		proplen;
    char_u	*props = ખાલી;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    પૂર્ણાંક		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    પૂર્ણાંક		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    જો (type == ખાલી)
	જાઓ theend;

    જો (start_lnum < 1 || start_lnum > buf->b_ml.ml_લીટી_ગણતરી)
    {
	semsg(_(e_invalid_લીટી_સંખ્યા_nr), (long)start_lnum);
	જાઓ theend;
    }
    જો (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_લીટી_ગણતરી)
    {
	semsg(_(e_invalid_લીટી_સંખ્યા_nr), (long)end_lnum);
	જાઓ theend;
    }

    જો (buf->b_ml.ml_mfp == ખાલી)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	જાઓ theend;
    }

    જો (text != ખાલી)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	માટે (p = text; *p != NUL; MB_PTR_ADV(p))
	    જો (*p < ' ')
		*p = ' ';
    }

    માટે (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	જો (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_લીટી));
	    જાઓ theend;
	}

	જો (lnum == start_lnum)
	    col = start_col;
	નહીંતર
	    col = 1;
	જો (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != ખાલી))
	{
	    semsg(_(e_invalid_column_સંખ્યા_nr), (long)start_col);
	    જાઓ theend;
	}
	sort_col = col;

	જો (lnum == end_lnum)
	    length = end_col - col;
	નહીંતર
	    length = (પૂર્ણાંક)textlen - col + 1;
	જો (length > (long)textlen)
	    length = (પૂર્ણાંક)textlen;	// can include the end-of-line
	જો (length < 0)
	    length = 0;		// zero-width property

	જો (text_arg != ખાલી)
	{
	    length = (long)STRLEN(text_arg);
	    જો (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		જો ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	tmp_prop.tp_padleft = text_padding_ડાબું;
	જો (text_arg != ખાલી)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	માટે (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * કદ(textprop_T),
							   કદ(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    જો (prop_col >= sort_col)
		તોડો;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	પૂર્ણાંક		vtext_total = 0;
	પૂર્ણાંક		new_લીટી_len;
	char_u	*ગણતરી_dest;
	char_u	*vtext_dest;
	પૂર્ણાંક		j;

	// Compute total vtext size from existing props.
	માટે (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * કદ(textprop_T),
							કદ(textprop_T));
	    જો (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	જો (text_arg != ખાલી)
	    vtext_total += length + 1;

	new_લીટી_len = (પૂર્ણાંક)textlen + (પૂર્ણાંક)PROP_ગણતરી_SIZE
			+ new_propcount * (પૂર્ણાંક)કદ(textprop_T)
			+ vtext_total;
	newtext = alloc(new_લીટી_len);
	જો (newtext == ખાલી)
	    જાઓ theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_લીટી_ptr, textlen);

	// Write prop_count.
	ગણતરી_dest = newtext + textlen;
	mch_memmove(ગણતરી_dest, &new_propcount, PROP_ગણતરી_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = ગણતરી_dest + PROP_ગણતરી_SIZE;
	જો (i > 0)
	    mch_memmove(newprops, props, કદ(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	જો (i < proplen)
	    mch_memmove(newprops + (i + 1) * કદ(textprop_T),
					props + i * કદ(textprop_T),
					કદ(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * કદ(textprop_T);
	માટે (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    જો (j == i)
		ચાલુ;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * કદ(textprop_T),
		    કદ(textprop_T));
	    જો (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_ગણતરી_2 = (char_u *)props - PROP_ગણતરી_SIZE;
		char_u *old_vtext = old_ગણતરી_2 + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - ગણતરી_dest);
		mch_memmove(newprops + j * કદ(textprop_T), &ep,
			કદ(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	જો (text_arg != ખાલી)
	{
	    પૂર્ણાંક copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - ગણતરી_dest);
	    vtext_dest += copysize;
	}
	નહીંતર
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * કદ(textprop_T), &tmp_prop,
							કદ(textprop_T));

	જો (buf->b_ml.ml_flags & (ML_લીટી_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_લીટી_ptr);
	buf->b_ml.ml_લીટી_ptr = newtext;
	buf->b_ml.ml_લીટી_len = new_લીટી_len;
	buf->b_ml.ml_flags |= ML_લીટી_DIRTY;
    }

    changed_લીટી_display_buf(buf);
    changed_લીટીઓ_buf(buf, start_lnum, end_lnum + 1, 0);
    res = OK;

theend:
    vim_free(text);
    પાછા res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    શૂન્ય
f_prop_add_list(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    પૂર્ણાંક		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    પૂર્ણાંક		error = FALSE;
    પૂર્ણાંક		prev_did_emsg = did_emsg;

    જો (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	પાછા;

    જો (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	પાછા;

    dict = argvars[0].vval.v_dict;
    જો (dict == ખાલી || !dict_has_ચાવી(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	પાછા;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    જો (dict_has_ચાવી(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_સંખ્યા(dict, "id");
	જો (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    પાછા;
	}
	id = (પૂર્ણાંક)x;
    }

    જો (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	પાછા;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_વસ્તુઓ(argvars[1].vval.v_list, li)
    {
	જો (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == ખાલી)
	{
	    emsg(_(e_list_required));
	    પાછા;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	જો (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	જો (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	જો (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	પૂર્ણાંક this_id = id;
	જો (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	જો (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    જો (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    પાછા;
	}
	જો (prop_add_one(buf, type_name, this_id, ખાલી, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    પાછા;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
સ્થિર પૂર્ણાંક vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    સ્થિર પૂર્ણાંક
get_textprop_id(શૂન્ય)
{
    જો (vt_id_counter > 0)
	vt_id_counter = 0;
    પાછા --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    પૂર્ણાંક
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
    પૂર્ણાંક		id = 0;
    char_u	*text = ખાલી;
    પૂર્ણાંક		text_padding_ડાબું = 0;
    પૂર્ણાંક		flags = 0;

    જો (dict == ખાલી || !dict_has_ચાવી(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	જાઓ theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    જો (dict_has_ચાવી(dict, "end_lnum"))
    {
	end_lnum = dict_get_સંખ્યા(dict, "end_lnum");
	જો (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str), "end_lnum");
	    જાઓ theend;
	}
    }
    નહીંતર
	end_lnum = start_lnum;

    જો (dict_has_ચાવી(dict, "length"))
    {
	long length = dict_get_સંખ્યા(dict, "length");

	જો (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str), "length");
	    જાઓ theend;
	}
	end_col = start_col + length;
    }
    નહીંતર જો (dict_has_ચાવી(dict, "end_col"))
    {
	end_col = dict_get_સંખ્યા(dict, "end_col");
	જો (end_col <= 0)
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str), "end_col");
	    જાઓ theend;
	}
    }
    નહીંતર જો (start_lnum == end_lnum)
	end_col = start_col;
    નહીંતર
	end_col = 1;

    જો (dict_has_ચાવી(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_સંખ્યા(dict, "id");
	જો (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    જાઓ theend;
	}
	id = (પૂર્ણાંક)x;
    }

    જો (dict_has_ચાવી(dict, "text"))
    {
	જો (dict_has_ચાવી(dict, "length")
		|| dict_has_ચાવી(dict, "end_col")
		|| dict_has_ચાવી(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    જાઓ theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	જો (text == ખાલી)
	    જાઓ theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	જો (dict_has_ચાવી(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    જો (p == ખાલી)
		જાઓ theend;
	    જો (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		જાઓ theend;
	    }
	    જો (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_જમણું;
	    નહીંતર જો (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    નહીંતર જો (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    નહીંતર જો (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_કિંમત_for_argument_str_str), "text_align", p);
		જાઓ theend;
	    }
	}

	જો (dict_has_ચાવી(dict, "text_padding_left"))
	{
	    text_padding_ડાબું = dict_get_સંખ્યા(dict, "text_padding_left");
	    જો (text_padding_ડાબું < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		જાઓ theend;
	    }
	}

	જો (dict_has_ચાવી(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    જો (p == ખાલી)
		જાઓ theend;
	    જો (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    નહીંતર જો (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_કિંમત_for_argument_str_str), "text_wrap", p);
		જાઓ theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    જો (start_col < (text == ખાલી ? 1 : 0))
    {
	semsg(_(e_invalid_column_સંખ્યા_nr), (long)start_col);
	જાઓ theend;
    }
    જો (start_col > 0 && text_padding_ડાબું > 0)
    {
	emsg(_(e_can_only_use_ડાબું_padding_when_column_is_zero));
	જાઓ theend;
    }

    જો (dict_arg != ખાલી && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	જાઓ theend;

    જો (text != ખાલી)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    નહીંતર જો (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	જાઓ theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_ડાબું, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = ખાલી;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    પાછા id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    પૂર્ણાંક
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, પૂર્ણાંક will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_ગણતરી;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    જો ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == ખાલી)
	પાછા 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_લીટી_len - textlen;
    જો (propdata_len == 0)
	પાછા 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    જો (propdata_len < PROP_ગણતરી_SIZE + કદ(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	પાછા 0;
    }
    mch_memmove(&prop_ગણતરી, text + textlen, PROP_ગણતરી_SIZE);
    જો (!text_prop_ગણતરી_valid(prop_ગણતરી, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	પાછા 0;
    }
    જો (!text_prop_vtext_valid(text + textlen + PROP_ગણતરી_SIZE,
					   (પૂર્ણાંક)prop_ગણતરી, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	પાછા 0;
    }
    *props = text + textlen + PROP_ગણતરી_SIZE;
    પાછા (પૂર્ણાંક)prop_ગણતરી;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    પૂર્ણાંક
prop_ગણતરી_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    પૂર્ણાંક		ગણતરી = get_text_props(buf, lnum, &props, FALSE);
    પૂર્ણાંક		result = 0;
    textprop_T	prop;
    પૂર્ણાંક		i;
    પૂર્ણાંક		next_જમણું_goes_below = FALSE;

    જો (ગણતરી == 0)
	પાછા 0;
    માટે (i = 0; i < ગણતરી; ++i)
    {
	mch_memmove(&prop, props + i * કદ(prop), કદ(prop));
	જો (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    જો ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_જમણું_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_જમણું)))
	    {
		++result;
	    }
	    નહીંતર જો (prop.tp_flags & TP_FLAG_ALIGN_જમણું)
		next_જમણું_goes_below = TRUE;
	}
    }
    પાછા result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    પૂર્ણાંક
ગણતરી_props(linenr_T lnum, પૂર્ણાંક only_starting, પૂર્ણાંક last_લીટી)
{
    char_u	*props;
    પૂર્ણાંક		proplen = get_text_props(curbuf, lnum, &props, 0);
    પૂર્ણાંક		result = proplen;
    textprop_T	prop;

    માટે (પૂર્ણાંક i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * કદ(prop), કદ(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	જો ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_લીટી && prop.tp_col == MAXCOL))
	    --result;
    }
    પાછા result;
}

સ્થિર textprop_T	*text_prop_compare_props;
સ્થિર buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    સ્થિર પૂર્ણાંક
text_prop_order(પૂર્ણાંક flags)
{
    જો (flags & TP_FLAG_ALIGN_ABOVE)
	પાછા 0;
    જો (flags & TP_FLAG_ALIGN_જમણું)
	પાછા 2;
    જો (flags & TP_FLAG_ALIGN_BELOW)
	પાછા 3;
    પાછા 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    સ્થિર પૂર્ણાંક
text_prop_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    પૂર્ણાંક  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(પૂર્ણાંક *)s1;
    idx2 = *(પૂર્ણાંક *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    જો ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	પાછા tp1->tp_id < 0 ? 1 : -1;

    જો (col1 == MAXCOL || col2 == MAXCOL)
    {
	પૂર્ણાંક order1 = text_prop_order(tp1->tp_flags);
	પૂર્ણાંક order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	જો (order1 != order2)
	    પાછા order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    જો (pt1 != pt2)
    {
	જો (pt1 == ખાલી)
	    પાછા -1;
	જો (pt2 == ખાલી)
	    પાછા 1;
	જો (pt1->pt_priority != pt2->pt_priority)
	    પાછા pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    જો (col1 != col2)
	પાછા col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    જો (tp1->tp_id < 0)
	પાછા tp1->tp_id > tp2->tp_id ? 1 : -1;

    પાછા 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    શૂન્ય
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	પૂર્ણાંક	    *idxs,
	પૂર્ણાંક	    ગણતરી)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((શૂન્ય *)idxs, (size_t)ગણતરી, કદ(પૂર્ણાંક), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_લીટીઓ(
	win_T	    *wp,
	પૂર્ણાંક	    type_id,
	પૂર્ણાંક	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    જો (text_prop_type_by_id(wp->w_buffer, type_id) == ખાલી)
	પાછા false;

    જો (first_lnum < 1)
	first_lnum = 1;
    જો (last_lnum > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	last_lnum = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;

    માટે (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	પૂર્ણાંક	ગણતરી = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	માટે (પૂર્ણાંક i = 0; i < ગણતરી; ++i)
	{
	    mch_memmove(prop, props + i * કદ(textprop_T),
							   કદ(textprop_T));
	    જો (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		પાછા true;
	    }
	}
    }
    પાછા false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	પૂર્ણાંક	    type_id,
	પૂર્ણાંક	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    પાછા find_prop_in_લીટીઓ(wp, type_id, id, prop, found_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    સ્થિર શૂન્ય
set_text_props(linenr_T lnum, textprop_T *tps, પૂર્ણાંક ગણતરી)
{
    char_u  *text;
    char_u  *newtext;
    પૂર્ણાંક	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    જો (ગણતરી == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	જો (newtext == ખાલી)
	    પાછા;
	mch_memmove(newtext, text, textlen);
	જો (curbuf->b_ml.ml_flags & (ML_લીટી_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_લીટી_ptr);
	curbuf->b_ml.ml_લીટી_ptr = newtext;
	curbuf->b_ml.ml_લીટી_len = textlen;
	curbuf->b_ml.ml_flags |= ML_લીટી_DIRTY;
    }
    નહીંતર
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_ગણતરી = (uint16_t)ગણતરી;
	પૂર્ણાંક	    vtext_size = 0;
	પૂર્ણાંક	    total_len;
	char_u	    *ગણતરી_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	પૂર્ણાંક	    i;

	માટે (i = 0; i < ગણતરી; ++i)
	    જો (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (પૂર્ણાંક)PROP_ગણતરી_SIZE
		    + ગણતરી * (પૂર્ણાંક)કદ(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	જો (newtext == ખાલી)
	    પાછા;
	mch_memmove(newtext, text, textlen);

	ગણતરી_dest = newtext + textlen;
	mch_memmove(ગણતરી_dest, &prop_ગણતરી, PROP_ગણતરી_SIZE);

	prop_dest = ગણતરી_dest + PROP_ગણતરી_SIZE;
	vtext_dest = prop_dest + ગણતરી * કદ(textprop_T);

	માટે (i = 0; i < ગણતરી; ++i)
	{
	    textprop_T prop = tps[i];

	    જો (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		પૂર્ણાંક copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = ખાલી;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - ગણતરી_dest);
		vtext_dest += copysize;
	    }
	    નહીંતર
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, કદ(textprop_T));
	    prop_dest += કદ(textprop_T);
	}

	જો (curbuf->b_ml.ml_flags & (ML_લીટી_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_લીટી_ptr);
	curbuf->b_ml.ml_લીટી_ptr = newtext;
	curbuf->b_ml.ml_લીટી_len = total_len;
	curbuf->b_ml.ml_flags |= ML_લીટી_DIRTY;
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
props_add_ગણતરી_header(char_u *લીટી, પૂર્ણાંક લીટી_len, પૂર્ણાંક textlen, પૂર્ણાંક *new_len)
{
    પૂર્ણાંક		prop_bytes = લીટી_len - textlen;
    uint16_t	prop_ગણતરી;
    પૂર્ણાંક		vtext_size = 0;
    પૂર્ણાંક		total;
    char_u	*newline;
    char_u	*ગણતરી_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    પૂર્ણાંક		i;

    જો (prop_bytes <= 0 || prop_bytes % કદ(textprop_T) != 0)
    {
	*new_len = લીટી_len;
	પાછા ખાલી;
    }
    prop_ગણતરી = (uint16_t)(prop_bytes / કદ(textprop_T));

    // Calculate total vtext size.
    માટે (i = 0; i < (પૂર્ણાંક)prop_ગણતરી; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, લીટી + textlen + i * કદ(textprop_T),
							કદ(textprop_T));
	જો (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (પૂર્ણાંક)PROP_ગણતરી_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    જો (newline == ખાલી)
	પાછા ખાલી;
    *new_len = total;

    mch_memmove(newline, લીટી, textlen);
    ગણતરી_dest = newline + textlen;
    mch_memmove(ગણતરી_dest, &prop_ગણતરી, PROP_ગણતરી_SIZE);
    prop_dest = ગણતરી_dest + PROP_ગણતરી_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    માટે (i = 0; i < (પૂર્ણાંક)prop_ગણતરી; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, લીટી + textlen + i * કદ(textprop_T),
							કદ(textprop_T));
	જો (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    પૂર્ણાંક copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - ગણતરી_dest);
	    vtext_dest += copysize;
	}
	નહીંતર
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * કદ(textprop_T), &prop,
							કદ(textprop_T));
    }
    પાછા newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    શૂન્ય
add_text_props(linenr_T lnum, textprop_T *text_props, પૂર્ણાંક text_prop_ગણતરી)
{
    unpacked_memline_T	um;
    પૂર્ણાંક			i;

    um = um_open_at(curbuf, lnum, text_prop_ગણતરી);
    જો (um.buf == ખાલી)
	પાછા;
    જો (!um_detach(&um))
    {
	um_abort(&um);
	પાછા;
    }

    // Grow props array if needed.
    જો (um.prop_ગણતરી + text_prop_ગણતરી > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_ગણતરી + text_prop_ગણતરી) * કદ(textprop_T));
	જો (newprops == ખાલી)
	{
	    um_abort(&um);
	    પાછા;
	}
	um.props = newprops;
	um.prop_size = um.prop_ગણતરી + text_prop_ગણતરી;
    }

    માટે (i = 0; i < text_prop_ગણતરી; ++i)
	um.props[um.prop_ગણતરી++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    સ્થિર પૂર્ણાંક
compare_pt(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    પાછા tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    સ્થિર proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, પૂર્ણાંક id)
{
    પૂર્ણાંક નીચલું = 0;
    પૂર્ણાંક ઉપલું;

    જો (ht == ખાલી || ht->ht_used == 0)
	પાછા ખાલી;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    જો (*array == ખાલી)
    {
	long	    todo;
	hashitem_T  *hi;
	પૂર્ણાંક	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	જો (*array == ખાલી)
	    પાછા ખાલી;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
	{
	    જો (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((શૂન્ય *)*array, ht->ht_used, કદ(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    ઉપલું = ht->ht_used;
    જ્યારે (ઉપલું > નીચલું)
    {
	પૂર્ણાંક m = (ઉપલું + નીચલું) / 2;

	જો ((*array)[m]->pt_id == id)
	    પાછા (*array)[m];
	જો ((*array)[m]->pt_id > id)
	    ઉપલું = m;
	નહીંતર
	    નીચલું = m + 1;
    }
    પાછા ખાલી;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    સ્થિર શૂન્ય
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    પૂર્ણાંક buflocal = TRUE;
    પૂર્ણાંક virtualtext_prop = prop->tp_id < 0;

    dict_add_સંખ્યા(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    જો (!virtualtext_prop)
    {
	dict_add_સંખ્યા(dict, "length", prop->tp_len);
	dict_add_સંખ્યા(dict, "id", prop->tp_id);
    }
    dict_add_સંખ્યા(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_સંખ્યા(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    જો (pt == ખાલી)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    જો (pt != ખાલી)
	dict_add_string(dict, "type", pt->pt_name);

    જો (buflocal)
	dict_add_સંખ્યા(dict, "type_bufnr", buf->b_fnum);
    નહીંતર
	dict_add_સંખ્યા(dict, "type_bufnr", 0);
    જો (virtualtext_prop)
    {
	string_T    text_align = {ખાલી, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	જો (prop->tp_flags & TP_FLAG_ALIGN_જમણું)
	    STR_LITERAL_SET(text_align, "right");
	નહીંતર જો (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	નહીંતર જો (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	જો (text_align.string != ખાલી)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (પૂર્ણાંક)text_align.length);

	// text_wrap
	જો (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	જો (prop->tp_padleft != 0)
	    dict_add_સંખ્યા(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, પૂર્ણાંક id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    જો (type == ખાલી)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    પાછા type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    પાછા text_prop_type_by_id(buf, prop->tp_type) != ખાલી;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    શૂન્ય
f_prop_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    linenr_T start;
    linenr_T end;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    પૂર્ણાંક	    did_clear = FALSE;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	પાછા;

    start = tv_get_સંખ્યા(&argvars[0]);
    end = start;
    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_સંખ્યા(&argvars[1]);
	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    જો (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		પાછા;
	}
    }
    જો (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	પાછા;
    }

    {
	unpacked_memline_T um = um_open(buf);

	માટે (lnum = start; lnum <= end; ++lnum)
	{
	    પૂર્ણાંક idx;

	    જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)
		તોડો;
	    જો (!um_goto_લીટી(&um, lnum, 0))
		તોડો;
	    જો (um.prop_ગણતરી > 0)
	    {
		did_clear = TRUE;
		માટે (idx = um.prop_ગણતરી - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    જો (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    શૂન્ય
f_prop_find(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    પૂર્ણાંક		lnum_start;
    પૂર્ણાંક		start_pos_has_prop = 0;
    પૂર્ણાંક		seen_end = FALSE;
    પૂર્ણાંક		id = 0;
    પૂર્ણાંક		id_found = FALSE;
    પૂર્ણાંક		type_id = -1;
    પૂર્ણાંક		skipstart = FALSE;
    પૂર્ણાંક		lnum = -1;
    પૂર્ણાંક		col = -1;
    પૂર્ણાંક		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    પૂર્ણાંક		both;

    જો (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    જો (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	પાછા;
    dict = argvars[0].vval.v_dict;

    જો (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	પાછા;
    જો (buf->b_ml.ml_mfp == ખાલી)
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	જો (*dir_s == 'b')
	    dir = BACKWARD;
	નહીંતર જો (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    પાછા;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    જો (di != ખાલી)
	lnum = tv_get_સંખ્યા(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    જો (di != ખાલી)
	col = tv_get_સંખ્યા(&di->di_tv);

    જો (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    નહીંતર જો (col == -1)
	col = 1;

    જો (lnum < 1 || lnum > buf->b_ml.ml_લીટી_ગણતરી)
    {
	emsg(_(e_invalid_range));
	પાછા;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    જો (dict_has_ચાવી(dict, "id"))
    {
	id = dict_get_સંખ્યા(dict, "id");
	id_found = TRUE;
    }
    જો (dict_has_ચાવી(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	જો (type == ખાલી)
	    પાછા;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    જો (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	પાછા;
    }
    જો (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	પાછા;
    }

    lnum_start = lnum;

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;

    જ્યારે (1)
    {
	char_u	*prop_start_ptr;
	પૂર્ણાંક	ગણતરી = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	પૂર્ણાંક	    i;
	textprop_T  prop;
	પૂર્ણાંક	    prop_start;
	પૂર્ણાંક	    prop_end;

	માટે (i = dir == BACKWARD ? ગણતરી - 1 : 0; i >= 0 && i < ગણતરી; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * કદ(textprop_T),
							   કદ(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    જો (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_ગણતરી_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    નહીંતર
		prop.u.tp_text = ખાલી;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    જો (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		જો (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    જો (!is_floating_vtext && prop.tp_col > col)
			ચાલુ;
		}
		નહીંતર જો (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    ચાલુ;
	    }
	    જો (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		જો (lnum_start == lnum
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
		જો (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		જો (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    જો (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    ચાલુ;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		જો (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    ચાલુ;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_સંખ્યા(rettv->vval.v_dict, "lnum", lnum);

		પાછા;
	    }
	}

	જો (dir > 0)
	{
	    જો (lnum >= buf->b_ml.ml_લીટી_ગણતરી)
		તોડો;
	    lnum++;
	}
	નહીંતર
	{
	    જો (lnum <= 1)
		તોડો;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    સ્થિર પૂર્ણાંક
prop_type_or_id_in_list(પૂર્ણાંક *types_or_ids, પૂર્ણાંક len, પૂર્ણાંક type_or_id)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < len; i++)
	જો (types_or_ids[i] == type_or_id)
	    પાછા TRUE;

    પાછા FALSE;
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
    સ્થિર શૂન્ય
get_props_in_લીટી(
	buf_T		*buf,
	linenr_T	lnum,
	પૂર્ણાંક		*prop_types,
	પૂર્ણાંક		prop_types_len,
	પૂર્ણાંક		*prop_ids,
	પૂર્ણાંક		prop_ids_len,
	list_T		*retlist,
	પૂર્ણાંક		add_lnum)
{
    char_u	*props;
    પૂર્ણાંક		ગણતરી;
    પૂર્ણાંક		i;
    textprop_T	prop;

    ગણતરી = get_text_props(buf, lnum, &props, FALSE);
    માટે (i = 0; i < ગણતરી; ++i)
    {
	mch_memmove(&prop, props + i * કદ(textprop_T),
		કદ(textprop_T));
	// Convert offset to pointer for virtual text props.
	જો (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_ગણતરી_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	નહીંતર
	    prop.u.tp_text = ખાલી;
	જો ((prop_types == ખાલી
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == ખાલી
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    જો (d == ખાલી)
		તોડો;
	    prop_fill_dict(d, &prop, buf);
	    જો (add_lnum)
		dict_add_સંખ્યા(d, "lnum", lnum);
	    જો (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    સ્થિર પૂર્ણાંક *
get_prop_types_from_names(list_T *l, buf_T *buf, પૂર્ણાંક *અંક_types)
{
    પૂર્ણાંક		*prop_types;
    listitem_T	*li;
    પૂર્ણાંક		i;
    char_u	*name;
    proptype_T	*type;

    *અંક_types = 0;

    prop_types = ALLOC_MULT(પૂર્ણાંક, list_len(l));
    જો (prop_types == ખાલી)
	પાછા ખાલી;

    i = 0;
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	જો (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    જાઓ errret;
	}
	name = li->li_tv.vval.v_string;
	જો (name == ખાલી)
	    જાઓ errret;

	type = lookup_prop_type(name, buf);
	જો (type == ખાલી)
	    જાઓ errret;
	prop_types[i++] = type->pt_id;
    }

    *અંક_types = i;
    પાછા prop_types;

errret:
    VIM_CLEAR(prop_types);
    પાછા ખાલી;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    સ્થિર પૂર્ણાંક *
get_prop_ids_from_list(list_T *l, પૂર્ણાંક *અંક_ids)
{
    પૂર્ણાંક		*prop_ids;
    listitem_T	*li;
    પૂર્ણાંક		i = 0;
    પૂર્ણાંક		id;
    પૂર્ણાંક		error;

    *અંક_ids = 0;

    prop_ids = ALLOC_MULT(પૂર્ણાંક, list_len(l));
    જો (prop_ids == ખાલી)
	પાછા ખાલી;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	error = FALSE;
	id = tv_get_સંખ્યા_chk(&li->li_tv, &error);
	જો (error)
	    જાઓ errret;

	prop_ids[i++] = id;
    }

    *અંક_ids = i;
    પાછા prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    પાછા ખાલી;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    શૂન્ય
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    buf_T	*buf = curbuf;
    પૂર્ણાંક		add_lnum = FALSE;
    પૂર્ણાંક		*prop_types = ખાલી;
    પૂર્ણાંક		prop_types_len = 0;
    પૂર્ણાંક		*prop_ids = ખાલી;
    પૂર્ણાંક		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    // default: get text properties on current line
    start_lnum = tv_get_સંખ્યા(&argvars[0]);
    end_lnum = start_lnum;
    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	જો (check_for_dict_arg(argvars, 1) == FAIL)
	    પાછા;
	d = argvars[1].vval.v_dict;

	જો (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    પાછા;

	જો (d != ખાલી && (di = dict_find(d, (char_u *)"end_lnum", -1)) != ખાલી)
	{
	    જો (di->di_tv.v_type != VAR_સંખ્યા)
	    {
		emsg(_(e_સંખ્યા_required));
		પાછા;
	    }
	    end_lnum = tv_get_સંખ્યા(&di->di_tv);
	    જો (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		//લીટી
		end_lnum = buf->b_ml.ml_લીટી_ગણતરી + end_lnum + 1;
	    નહીંતર જો (end_lnum > buf->b_ml.ml_લીટી_ગણતરી)
		end_lnum = buf->b_ml.ml_લીટી_ગણતરી;
	    add_lnum = TRUE;
	}
	જો (d != ખાલી && (di = dict_find(d, (char_u *)"types", -1)) != ખાલી)
	{
	    જો (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		પાછા;
	    }

	    l = di->di_tv.vval.v_list;
	    જો (l != ખાલી && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		જો (prop_types == ખાલી)
		    પાછા;
	    }
	}
	જો (d != ખાલી && (di = dict_find(d, (char_u *)"ids", -1)) != ખાલી)
	{
	    જો (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		જાઓ errret;
	    }

	    l = di->di_tv.vval.v_list;
	    જો (l != ખાલી && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		જો (prop_ids == ખાલી)
		    જાઓ errret;
	    }
	}
    }
    જો (start_lnum < 1 || start_lnum > buf->b_ml.ml_લીટી_ગણતરી
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    નહીંતર
	માટે (lnum = start_lnum; lnum <= end_lnum; lnum++)
	    get_props_in_લીટી(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, add_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    શૂન્ય
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start = 1;
    linenr_T	end = 0;
    linenr_T	lnum;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    પૂર્ણાંક		do_all;
    પૂર્ણાંક		id = -MAXCOL;
    પૂર્ણાંક		type_id = -1;	    // for a single "type"
    પૂર્ણાંક		*type_ids = ખાલી;   // array, for a list of "types", allocated
    પૂર્ણાંક		અંક_type_ids = 0;   // number of elements in "type_ids"
    પૂર્ણાંક		both;

    rettv->vval.v_સંખ્યા = 0;

    જો (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL)))
	પાછા;

    જો (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_સંખ્યા(&argvars[1]);
	end = start;
	જો (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_સંખ્યા(&argvars[2]);
	જો (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    પાછા;
	}
    }

    dict = argvars[0].vval.v_dict;
    જો (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	પાછા;
    જો (buf->b_ml.ml_mfp == ખાલી)
	પાછા;

    do_all = dict_get_bool(dict, "all", FALSE);

    જો (dict_has_ચાવી(dict, "id"))
	id = dict_get_સંખ્યા(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    જો (dict_has_ચાવી(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	જો (type == ખાલી)
	    પાછા;
	type_id = type->pt_id;
    }
    જો (dict_has_ચાવી(dict, "types"))
    {
	typval_T types;
	listitem_T *li = ખાલી;

	dict_get_tv(dict, "types", &types);
	જો (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( કદ(પૂર્ણાંક) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_વસ્તુઓ(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		જો (li->li_tv.v_type != VAR_STRING)
		    ચાલુ;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		જો (!prop_type)
		    જાઓ cleanup_prop_remove;

		type_ids[અંક_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    જો (id == -MAXCOL && (type_id == -1 && અંક_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	જાઓ cleanup_prop_remove;
    }
    જો (both && (id == -MAXCOL || (type_id == -1 && અંક_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	જાઓ cleanup_prop_remove;
    }
    જો (type_id != -1 && અંક_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	જાઓ cleanup_prop_remove;
    }

    જો (end == 0)
	end = buf->b_ml.ml_લીટી_ગણતરી;

    {
	unpacked_memline_T um = um_open(buf);

	માટે (lnum = start; lnum <= end; ++lnum)
	{
	    પૂર્ણાંક idx;

	    જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)
		તોડો;
	    જો (!um_goto_લીટી(&um, lnum, 0))
		તોડો;

	    માટે (idx = 0; idx < um.prop_ગણતરી; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		પૂર્ણાંક	    matches_id = 0;
		પૂર્ણાંક	    matches_type = 0;

		જો (prop->tp_flags & TP_FLAG_DELETED)
		    ચાલુ;

		matches_id = prop->tp_id == id;
		જો (અંક_type_ids > 0)
		{
		    પૂર્ણાંક idx2;

		    માટે (idx2 = 0; !matches_type && idx2 < અંક_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		નહીંતર
		    matches_type = prop->tp_type == type_id;

		જો (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    જો (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_સંખ્યા;
		    જો (!do_all)
			તોડો;
		}
	    }
	}
	um_close(&um);
    }

    જો (first_changed > 0)
    {
	changed_લીટી_display_buf(buf);
	changed_લીટીઓ_buf(buf, first_changed, last_changed + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    સ્થિર શૂન્ય
prop_type_set(typval_T *argvars, પૂર્ણાંક add)
{
    char_u	*name;
    buf_T	*buf = ખાલી;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	પાછા;

    name = tv_get_string(&argvars[0]);
    જો (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	પાછા;
    }

    જો (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	પાછા;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    જો (add)
    {
	hashtab_T **htp;

	જો (prop != ખાલી)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    પાછા;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	જો (prop == ખાલી)
	    પાછા;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	જો (buf == ખાલી)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	નહીંતર
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	જો (*htp == ખાલી)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    જો (*htp == ખાલી)
	    {
		vim_free(prop);
		પાછા;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    નહીંતર
    {
	જો (prop == ખાલી)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    પાછા;
	}
    }

    જો (dict != ખાલી)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	જો (di != ખાલી)
	{
	    char_u	*highlight;
	    પૂર્ણાંક		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    જો (highlight != ખાલી && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    જો (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == ખાલી ? (char_u *)"" : highlight);
		પાછા;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	જો (di != ખાલી)
	{
	    જો (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    નહીંતર
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	જો (di != ખાલી)
	{
	    જો (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    નહીંતર
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	જો (di != ખાલી)
	    prop->pt_priority = tv_get_સંખ્યા(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	જો (di != ખાલી)
	{
	    જો (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    નહીંતર
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	જો (di != ખાલી)
	{
	    જો (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    નહીંતર
		prop->pt_flags &= ~PT_FLAG_INS_END_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    શૂન્ય
f_prop_type_add(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    શૂન્ય
f_prop_type_change(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    શૂન્ય
f_prop_type_delete(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*name;
    buf_T	*buf = ખાલી;
    hashitem_T	*hi;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    name = tv_get_string(&argvars[0]);
    જો (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	પાછા;
    }

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	જો (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    પાછા;
    }

    hi = find_prop_type_hi(name, buf);
    જો (hi == ખાલી)
	પાછા;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    જો (buf == ખાલી)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    નહીંતર
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    changed_window_setting_buf(buf == ખાલી ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    શૂન્ય
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    name = tv_get_string(&argvars[0]);
    જો (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	પાછા;
    }

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;

    proptype_T  *prop = ખાલી;
    buf_T	    *buf = ખાલી;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	જો (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    પાછા;
    }

    prop = find_prop_type(name, buf);
    જો (prop == ખાલી)
	પાછા;

    dict_T *d = rettv->vval.v_dict;

    જો (prop->pt_hl_id > 0)
	dict_add_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_add_સંખ્યા(d, "priority", prop->pt_priority);
    dict_add_સંખ્યા(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_add_સંખ્યા(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_add_સંખ્યા(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_START_INCL) ? 1 : 0);
    dict_add_સંખ્યા(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_END_INCL) ? 1 : 0);
    જો (buf != ખાલી)
	dict_add_સંખ્યા(d, "bufnr", buf->b_fnum);
}

    સ્થિર શૂન્ય
list_types(hashtab_T *ht, list_T *l)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
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
    શૂન્ય
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = ખાલી;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	જો (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    પાછા;
    }
    જો (buf == ખાલી)
    {
	જો (global_proptypes != ખાલી)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    નહીંતર જો (buf->b_proptypes != ખાલી)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    સ્થિર શૂન્ય
clear_ht_prop_types(hashtab_T *ht)
{
    long	todo;
    hashitem_T	*hi;

    જો (ht == ખાલી)
	પાછા;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#જો defined(EXITFREE)
/*
 * Free all global property types.
 */
    શૂન્ય
clear_global_prop_types(શૂન્ય)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = ખાલી;
    VIM_CLEAR(global_proparray);
}
#endif

/*
 * Free all property types for "buf".
 */
    શૂન્ય
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = ખાલી;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક dirty;	    // if the property was changed
    પૂર્ણાંક can_drop;   // whether after this change, the prop may be removed
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
    સ્થિર adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    col,
	પૂર્ણાંક	    added,
	પૂર્ણાંક	    flags)
{
    proptype_T	*pt;
    પૂર્ણાંક		start_incl;
    પૂર્ણાંક		end_incl;
    પૂર્ણાંક		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    જો (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	પાછા res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != ખાલી && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    જો (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != ખાલી && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    જો (added > 0)
    {
	જો (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	નહીંતર જો (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    નહીંતર જો (prop->tp_col > col + 1)
    {
	જો (prop->tp_col + added < col + 1)
	{
	    જો (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    નહીંતર
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		જો (prop->tp_len <= 0)
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
	નહીંતર
	    prop->tp_col += added;
    }
    નહીંતર જો (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	જો (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	નહીંતર
	{
	    પૂર્ણાંક after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	જો (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    નહીંતર
	res.dirty = FALSE;

    પાછા res;
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
    પૂર્ણાંક
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	પૂર્ણાંક	    bytes_added,
	પૂર્ણાંક	    flags)
{
    unpacked_memline_T um;
    પૂર્ણાંક		dirty = FALSE;
    પૂર્ણાંક		ri;

    જો (text_prop_frozen > 0)
	પાછા FALSE;

    um = um_open_at(curbuf, lnum, 0);
    જો (um.buf == ખાલી || um.prop_ગણતરી == 0)
    {
	um_abort(&um);
	પાછા FALSE;
    }

    માટે (ri = 0; ri < um.prop_ગણતરી; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	જો (res.dirty)
	{
	    જો (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		જો (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    પાછા FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		જો ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    પાછા FALSE;
		}
	    }
	    dirty = TRUE;
	}
	જો (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    ચાલુ;
	}
    }
    જો (dirty)
	um_close(&um);
    નહીંતર
	um_abort(&um);
    પાછા dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    શૂન્ય
adjust_props_for_split(
	linenr_T    lnum_props,
	linenr_T    lnum_ટોચ,
	પૂર્ણાંક	    kept,
	પૂર્ણાંક	    deleted,
	પૂર્ણાંક	    at_eol)
{
    char_u	*props;
    પૂર્ણાંક		ગણતરી;
    garray_T    prevprop;
    garray_T    nextprop;
    પૂર્ણાંક		skipped = kept + deleted;

    જો (!curbuf->b_has_textprop)
	પાછા;

    // Get the text properties from "lnum_props".
    ગણતરી = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, કદ(textprop_T), 10);
    ga_init2(&nextprop, કદ(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *ગણતરી_ptr = props - PROP_ગણતરી_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    માટે (પૂર્ણાંક i = 0; i < ગણતરી; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	પૂર્ણાંક	    start_incl, end_incl;
	પૂર્ણાંક	    cont_prev, cont_next;
	પૂર્ણાંક	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * કદ(textprop_T), કદ(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	જો (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(ગણતરી_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	નહીંતર
	    prop.u.tp_text = ખાલી;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != ખાલી && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != ખાલી && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	જો (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	નહીંતર
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    પૂર્ણાંક is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    જો (is_floating_vtext)
		cont_next = FALSE;
	    નહીંતર
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	જો (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	જો (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    પૂર્ણાંક is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    જો (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    જો (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	જો (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    પૂર્ણાંક is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    જો (p->tp_col != MAXCOL)
	    {
		જો (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		નહીંતર
		{
		    જો (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    જો (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    set_text_props(lnum_ટોચ, (textprop_T *)prevprop.ga_data,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    set_text_props(lnum_ટોચ + 1, (textprop_T *)nextprop.ga_data,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    શૂન્ય
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	પૂર્ણાંક		    last_લીટી,
	long		    col,
	પૂર્ણાંક		    removed)
{
    unpacked_memline_T	r_um;

    જો (um->buf == ખાલી)
	પાછા;

    r_um = um_open_at(um->buf, lnum, 0);
    જો (r_um.buf == ખાલી)
	પાછા;
    જો (!um_detach(&r_um))
    {
	um_abort(&r_um);
	પાછા;
    }

    માટે (પૂર્ણાંક i = r_um.prop_ગણતરી - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	પૂર્ણાંક	    end;

	જો (prop->tp_col == MAXCOL && !last_લીટી)
	    ચાલુ;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	જો (last_લીટી || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = ખાલી;  // ownership transferred
	}
	નહીંતર
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    માટે (પૂર્ણાંક j = 0; j < um->prop_ગણતરી; ++j)
	    {
		textprop_T *op = &um->props[j];

		જો ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    તોડો;
		}
	    }
	    જો (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    સ્થિર શૂન્ય
remember_cleared_prop(garray_T *gap, પૂર્ણાંક which, textprop_T *prop, પૂર્ણાંક flag)
{
    undoprop_T	*up;

    જો (which == UNDOPROP_NONE)
	પાછા;
    જો (gap->ga_itemsize == 0)
	ga_init2(gap, કદ(undoprop_T), 4);
    જો (ga_grow(gap, 1) == FAIL)
	પાછા;
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
    શૂન્ય
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	પૂર્ણાંક	    above_which,
	પૂર્ણાંક	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = ખાલી;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    જો (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_લીટી_ગણતરી)
	પાછા;

    above = um_open_at(buf, lnum, 0);
    જો (above.buf == ખાલી)
	પાછા;
    માટે (પૂર્ણાંક i = 0; i < above.prop_ગણતરી; ++i)
	જો (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    જો (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	પાછા;
    }

    below = um_open_at(buf, lnum + 1, 0);
    જો (below.buf == ખાલી)
    {
	um_abort(&above);
	પાછા;
    }
    જો (below.prop_ગણતરી > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_ગણતરી);
	જો (paired == ખાલી)
	    જાઓ theend;
    }

    માટે (પૂર્ણાંક i = 0; i < above.prop_ગણતરી; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	જો (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    ચાલુ;
	માટે (પૂર્ણાંક j = 0; j < below.prop_ગણતરી; ++j)
	{
	    textprop_T *next = &below.props[j];

	    જો (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		તોડો;
	    }
	}
	જો (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    માટે (પૂર્ણાંક j = 0; j < below.prop_ગણતરી; ++j)
    {
	જો (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    ચાલુ;
	જો (!below.detached && !um_detach(&below))
	    તોડો;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    જો (below_changed)
	um_close(&below);
    નહીંતર
	um_abort(&below);
    જો (above_changed)
	um_close(&above);
    નહીંતર
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    શૂન્ય
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	પૂર્ણાંક	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    જો (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_લીટી_ગણતરી)
	પાછા;
    um = um_open_at(buf, lnum, 0);
    જો (um.buf == ખાલી)
	પાછા;
    માટે (પૂર્ણાંક r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	જો (up->up_which != which)
	    ચાલુ;
	માટે (પૂર્ણાંક i = 0; i < um.prop_ગણતરી; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    જો (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		ચાલુ;
	    જો (!um.detached && !um_detach(&um))
		જાઓ theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    તોડો;
	}
    }

theend:
    જો (changed)
	um_close(&um);
    નહીંતર
	um_abort(&um);
}

    bool
text_prop_ગણતરી_valid(પૂર્ણાંક prop_ગણતરી, size_t propdata_len)
{
    જો (propdata_len < PROP_ગણતરી_SIZE)
	પાછા false;
    પાછા (size_t)prop_ગણતરી * કદ(textprop_T)
		    <= propdata_len - PROP_ગણતરી_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, પૂર્ણાંક prop_ગણતરી, size_t propdata_len)
{
    માટે (પૂર્ણાંક i = 0; i < prop_ગણતરી; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * કદ(textprop_T),
							  કદ(textprop_T));
	જો (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    ચાલુ;
	જો (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    પાછા false;
    }
    પાછા true;
}

#endif // FEAT_PROP_POPUP
