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

#ضمن "vim.h"

#אם defined(FEAT_PROP_POPUP)

ساكن فراغ um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    ساكن فراغ
um_free_vtext(textprop_T *props, صحيح count)
{
    עבור (صحيح i = 0; i < count; ++i)
	אם (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    ساكن فراغ
um_free(unpacked_memline_T *um)
{
    אם (um->detached)
    {
	VIM_CLEAR(um->text);
	um_free_vtext(um->props, um->prop_count);
    }
    VIM_CLEAR(um->props);
    um->prop_size = 0;
    um->prop_count = 0;
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
    החזר um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_line(unpacked_memline_T *um, linenr_T lnum, صحيح extra_props)
{
    char_u	*line;
    size_t	textlen;
    size_t	propdata_len;
    صحيح		proplen;

    אם (um->buf == عدم)
	החזר false;

    // Store changes to the current line if any.
    אם (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    אם (lnum == 0)
	החזר true;	// just unload

    line = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_line_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = line;

    אם (propdata_len == 0)
	החזר true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    אם (propdata_len < PROP_COUNT_SIZE + حجم(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = عدم;
	החזר false;
    }

    uint16_t    prop_count;
    char_u	    *count_ptr = line + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_count, count_ptr, PROP_COUNT_SIZE);
    אם (!text_prop_count_valid(prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = عدم;
	החזר false;
    }
    proplen = (صحيح)prop_count;
    props_start = count_ptr + PROP_COUNT_SIZE;

    אם (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = عدم;
	החזר false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    אם (um->props == عدم)
    {
	um->buf = عدم;
	החזר false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_count = proplen;
    mch_memmove(um->props, props_start, proplen * حجم(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    עבור (صحيح i = 0; i < proplen; ++i)
    {
	אם (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = count_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	אחרת
	    um->props[i].u.tp_text = عدم;
    }

    החזר true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, صحيح extra_props)
{
    unpacked_memline_T um = um_open(buf);

    אם (!um_goto_line(&um, lnum, extra_props))
	um.buf = عدم;
    החזר um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    ساكن bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    אם (um->detached || um->buf == عدم)
	החזר um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    אם (newtext == عدم)
    {
	um->buf = عدم;
	החזר false;
    }
    um->text = newtext;

    עבור (صحيح i = 0; i < um->prop_count; ++i)
    {
	אם (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    אם (copy == عدم)
	    {
		עבור (صحيح j = 0; j < i; ++j)
		    אם (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = عدم;
		החזר false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    החזר true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    אם (um->buf == عدم)
	החזר false;
    אם (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    החזר true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    ساكن bool
um_grow_props(unpacked_memline_T *um, صحيح needed)
{
    אם (um->prop_count + needed <= um->prop_size)
	החזר true;

    صحيح	    new_size = um->prop_count + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * حجم(textprop_T));
    אם (newprops == عدم)
	החזר false;
    um->props = newprops;
    um->prop_size = new_size;

    החזר true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    ساكن فراغ
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    אם (um->buf == عدم)
	החזר;
    אם (!um->detached && !um_detach(um))
	החזר;
    אם (!um_grow_props(um, 1))
    {
	um->buf = عدم;
	החזר;
    }
    um->props[um->prop_count++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    فراغ
um_reverse_props(unpacked_memline_T *um)
{
    עבור (صحيح i = 0, j = um->prop_count - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, صحيح prop_count)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    אם (lnum < 1 || lnum > buf->b_ml.ml_line_count)
    {
	um.buf = عدم;
	החזר um;
    }

    char_u *line = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(line);

    אם (text_copy == عدم)
    {
	um.buf = عدم;
	החזר um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    אם (prop_count > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_count);
	אם (um.props == عدم)
	{
	    vim_free(um.text);
	    um.buf = عدم;
	    החזר um;
	}
	um.prop_size = prop_count;
    }

    החזר um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    ساكن فراغ
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					صحيح tp_id, صحيح tp_type, صحيح flag)
{
    unpacked_memline_T	neighbor;

    אם (lnum < 1 || lnum > buf->b_ml.ml_line_count)
	החזר;

    neighbor = um_open_at(buf, lnum, 0);
    אם (neighbor.buf == عدم)
	החזר;

    עבור (صحيح i = 0; i < neighbor.prop_count; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	אם (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    אם (!neighbor.detached && !um_detach(&neighbor))
		اكسر;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    اكسر;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    فراغ
um_delete_prop(unpacked_memline_T *um, صحيح index)
{
    textprop_T *prop;

    אם (um->buf == عدم || index < 0 || index >= um->prop_count)
	החזר;
    אם (!um->detached && !um_detach(um))
	החזר;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    אם (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    אם (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    אם (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
    ساكن char_u *
um_pack(unpacked_memline_T *um, صحيح *packed_len)
{
    uint16_t	live_count = 0;
    صحيح		vtext_size = 0;
    صحيح		total_len;
    char_u	*buf;
    char_u	*count_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    עבור (صحيح i = 0; i < um->prop_count; ++i)
    {
	textprop_T *prop = &um->props[i];

	אם (prop->tp_flags & TP_FLAG_DELETED)
	    استمر;
	++live_count;
	אם (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    אם (live_count == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	אם (buf == عدم)
	    החזר عدم;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	החזר buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (صحيح)PROP_COUNT_SIZE
		+ live_count * (صحيح)حجم(textprop_T) + vtext_size;
    buf = alloc(total_len);
    אם (buf == عدم)
	החזר عدم;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    count_dest = buf + um->text_size;
    mch_memmove(count_dest, &live_count, PROP_COUNT_SIZE);

    prop_dest = count_dest + PROP_COUNT_SIZE;
    vtext_dest = prop_dest + live_count * حجم(textprop_T);

    עבור (صحيح i = 0; i < um->prop_count; ++i)
    {
	textprop_T prop = um->props[i];

	אם (prop.tp_flags & TP_FLAG_DELETED)
	    استمر;

	אם (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    صحيح copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	אחרת
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, حجم(textprop_T));
	prop_dest += حجم(textprop_T);
    }

    החזר buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    ساكن فراغ
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    صحيح	    packed_len;

    אם (!um->detached || um->buf == عدم || um->lnum == 0)
	החזר;

    packed = um_pack(um, &packed_len);
    אם (packed == عدم)
    {
	um->buf = عدم;
	החזר;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    אם (um->buf->b_ml.ml_line_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    אחרת אם (!(um->buf->b_ml.ml_flags & ML_LINE_DIRTY))
	(فراغ)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_count);

    אם (um->buf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
	vim_free(um->buf->b_ml.ml_line_ptr);
    um->buf->b_ml.ml_line_ptr = packed;
    um->buf->b_ml.ml_line_len = packed_len;
    um->buf->b_ml.ml_flags |= ML_LINE_DIRTY;

    um->detached = false;
    um->text = packed;
    um->text_size = (colnr_T)STRLEN(packed) + 1;
    um->buf->b_ml.ml_line_textlen = um->text_size;
}

/*
 * Close an unpacked memline, storing any changes.
 */
    فراغ
um_close(unpacked_memline_T *um)
{
    אם (um->buf == عدم)
	החזר;
    אם (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = عدم;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    فراغ
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = عدم;
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
#define HI2PT(hi)      HIKEY2PT((hi)->hi_key)

// The global text property types.
ساكن hashtab_T *global_proptypes = عدم;
ساكن proptype_T **global_proparray = عدم;

// The last used text property type ID.
ساكن صحيح proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    ساكن hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    אם (*name == NUL)
	החזר عدم;
    אם (buf == عدم)
	ht = global_proptypes;
    אחרת
	ht = buf->b_proptypes;

    אם (ht == عدم)
	החזר عدم;
    hi = hash_find(ht, name);
    אם (HASHITEM_EMPTY(hi))
	החזר عدم;
    החזר hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    ساكن proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    אם (hi == عدم)
	החזר عدم;
    החזר HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    صحيح
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    אם (pt == عدم)
	החזר 0;
    החזר pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    ساكن proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = find_prop_type(name, buf);

    אם (type == عدم)
	type = find_prop_type(name, عدم);
    אם (type == عدم)
	semsg(_(e_property_type_str_does_not_exist), name);
    החזר type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    ساكن صحيح
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    אם (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	החזר FAIL;
    }
    אם (arg->vval.v_dict == عدم)
	החזר OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    אם (di != عدم && (di->di_tv.v_type != VAR_NUMBER
					      || di->di_tv.vval.v_number != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	אם (*buf == عدم)
	    החזר FAIL;
    }
    החזר OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    فراغ
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	החזר;

    start_lnum = tv_get_number(&argvars[0]);
    start_col = tv_get_number(&argvars[1]);
    אם (check_for_dict_arg(argvars, 2) == FAIL)
	החזר;

    rettv->vval.v_number = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    ساكن صحيح
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	صحيح		id,
	char_u		*text_arg,
	صحيح		text_padding_left,
	صحيح		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    صحيح		proplen;
    char_u	*props = عدم;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    صحيح		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    صحيح		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    אם (type == عدم)
	اذهب theend;

    אם (start_lnum < 1 || start_lnum > buf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), (long)start_lnum);
	اذهب theend;
    }
    אם (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), (long)end_lnum);
	اذهب theend;
    }

    אם (buf->b_ml.ml_mfp == عدم)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	اذهب theend;
    }

    אם (text != عدم)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	עבור (p = text; *p != NUL; MB_PTR_ADV(p))
	    אם (*p < ' ')
		*p = ' ';
    }

    עבור (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	אם (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_line));
	    اذهب theend;
	}

	אם (lnum == start_lnum)
	    col = start_col;
	אחרת
	    col = 1;
	אם (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != عدم))
	{
	    semsg(_(e_invalid_column_number_nr), (long)start_col);
	    اذهب theend;
	}
	sort_col = col;

	אם (lnum == end_lnum)
	    length = end_col - col;
	אחרת
	    length = (صحيح)textlen - col + 1;
	אם (length > (long)textlen)
	    length = (صحيح)textlen;	// can include the end-of-line
	אם (length < 0)
	    length = 0;		// zero-width property

	אם (text_arg != عدم)
	{
	    length = (long)STRLEN(text_arg);
	    אם (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		אם ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	tmp_prop.tp_padleft = text_padding_left;
	אם (text_arg != عدم)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	עבור (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * حجم(textprop_T),
							   حجم(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    אם (prop_col >= sort_col)
		اكسر;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	صحيح		vtext_total = 0;
	صحيح		new_line_len;
	char_u	*count_dest;
	char_u	*vtext_dest;
	صحيح		j;

	// Compute total vtext size from existing props.
	עבור (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * حجم(textprop_T),
							حجم(textprop_T));
	    אם (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	אם (text_arg != عدم)
	    vtext_total += length + 1;

	new_line_len = (صحيح)textlen + (صحيح)PROP_COUNT_SIZE
			+ new_propcount * (صحيح)حجم(textprop_T)
			+ vtext_total;
	newtext = alloc(new_line_len);
	אם (newtext == عدم)
	    اذهب theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_line_ptr, textlen);

	// Write prop_count.
	count_dest = newtext + textlen;
	mch_memmove(count_dest, &new_propcount, PROP_COUNT_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = count_dest + PROP_COUNT_SIZE;
	אם (i > 0)
	    mch_memmove(newprops, props, حجم(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	אם (i < proplen)
	    mch_memmove(newprops + (i + 1) * حجم(textprop_T),
					props + i * حجم(textprop_T),
					حجم(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * حجم(textprop_T);
	עבור (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    אם (j == i)
		استمر;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * حجم(textprop_T),
		    حجم(textprop_T));
	    אם (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_count = (char_u *)props - PROP_COUNT_SIZE;
		char_u *old_vtext = old_count + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
		mch_memmove(newprops + j * حجم(textprop_T), &ep,
			حجم(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	אם (text_arg != عدم)
	{
	    صحيح copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	אחרת
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * حجم(textprop_T), &tmp_prop,
							حجم(textprop_T));

	אם (buf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
	    vim_free(buf->b_ml.ml_line_ptr);
	buf->b_ml.ml_line_ptr = newtext;
	buf->b_ml.ml_line_len = new_line_len;
	buf->b_ml.ml_flags |= ML_LINE_DIRTY;
    }

    changed_line_display_buf(buf);
    changed_lines_buf(buf, start_lnum, end_lnum + 1, 0);
    res = OK;

theend:
    vim_free(text);
    החזר res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    فراغ
f_prop_add_list(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    صحيح		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    صحيح		error = FALSE;
    صحيح		prev_did_emsg = did_emsg;

    אם (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	החזר;

    אם (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	החזר;

    dict = argvars[0].vval.v_dict;
    אם (dict == عدم || !dict_has_key(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	החזר;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    אם (dict_has_key(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_number(dict, "id");
	אם (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    החזר;
	}
	id = (صحيح)x;
    }

    אם (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	החזר;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_ITEMS(argvars[1].vval.v_list, li)
    {
	אם (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == عدم)
	{
	    emsg(_(e_list_required));
	    החזר;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	אם (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	אם (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	אם (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	صحيح this_id = id;
	אם (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	אם (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    אם (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    החזר;
	}
	אם (prop_add_one(buf, type_name, this_id, عدم, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    החזר;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
ساكن صحيح vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    ساكن صحيح
get_textprop_id(فراغ)
{
    אם (vt_id_counter > 0)
	vt_id_counter = 0;
    החזר --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    صحيح
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
    صحيح		id = 0;
    char_u	*text = عدم;
    صحيح		text_padding_left = 0;
    صحيح		flags = 0;

    אם (dict == عدم || !dict_has_key(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	اذهب theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    אם (dict_has_key(dict, "end_lnum"))
    {
	end_lnum = dict_get_number(dict, "end_lnum");
	אם (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_value_for_argument_str), "end_lnum");
	    اذهب theend;
	}
    }
    אחרת
	end_lnum = start_lnum;

    אם (dict_has_key(dict, "length"))
    {
	long length = dict_get_number(dict, "length");

	אם (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_value_for_argument_str), "length");
	    اذهب theend;
	}
	end_col = start_col + length;
    }
    אחרת אם (dict_has_key(dict, "end_col"))
    {
	end_col = dict_get_number(dict, "end_col");
	אם (end_col <= 0)
	{
	    semsg(_(e_invalid_value_for_argument_str), "end_col");
	    اذهب theend;
	}
    }
    אחרת אם (start_lnum == end_lnum)
	end_col = start_col;
    אחרת
	end_col = 1;

    אם (dict_has_key(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_number(dict, "id");
	אם (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    اذهب theend;
	}
	id = (صحيح)x;
    }

    אם (dict_has_key(dict, "text"))
    {
	אם (dict_has_key(dict, "length")
		|| dict_has_key(dict, "end_col")
		|| dict_has_key(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    اذهب theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	אם (text == عدم)
	    اذهب theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	אם (dict_has_key(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    אם (p == عدم)
		اذهب theend;
	    אם (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		اذهب theend;
	    }
	    אם (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_RIGHT;
	    אחרת אם (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    אחרת אם (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    אחרת אם (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "text_align", p);
		اذهب theend;
	    }
	}

	אם (dict_has_key(dict, "text_padding_left"))
	{
	    text_padding_left = dict_get_number(dict, "text_padding_left");
	    אם (text_padding_left < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		اذهب theend;
	    }
	}

	אם (dict_has_key(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    אם (p == عدم)
		اذهب theend;
	    אם (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    אחרת אם (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "text_wrap", p);
		اذهب theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    אם (start_col < (text == عدم ? 1 : 0))
    {
	semsg(_(e_invalid_column_number_nr), (long)start_col);
	اذهب theend;
    }
    אם (start_col > 0 && text_padding_left > 0)
    {
	emsg(_(e_can_only_use_left_padding_when_column_is_zero));
	اذهب theend;
    }

    אם (dict_arg != عدم && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	اذهب theend;

    אם (text != عدم)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    אחרת אם (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	اذهب theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_left, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = عدم;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    החזר id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    صحيح
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, صحيح will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_count;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    אם ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == عدم)
	החזר 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_line_len - textlen;
    אם (propdata_len == 0)
	החזר 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    אם (propdata_len < PROP_COUNT_SIZE + حجم(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	החזר 0;
    }
    mch_memmove(&prop_count, text + textlen, PROP_COUNT_SIZE);
    אם (!text_prop_count_valid(prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	החזר 0;
    }
    אם (!text_prop_vtext_valid(text + textlen + PROP_COUNT_SIZE,
					   (صحيح)prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	החזר 0;
    }
    *props = text + textlen + PROP_COUNT_SIZE;
    החזר (صحيح)prop_count;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    صحيح
prop_count_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    صحيح		count = get_text_props(buf, lnum, &props, FALSE);
    صحيح		result = 0;
    textprop_T	prop;
    صحيح		i;
    صحيح		next_right_goes_below = FALSE;

    אם (count == 0)
	החזר 0;
    עבור (i = 0; i < count; ++i)
    {
	mch_memmove(&prop, props + i * حجم(prop), حجم(prop));
	אם (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    אם ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_right_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_RIGHT)))
	    {
		++result;
	    }
	    אחרת אם (prop.tp_flags & TP_FLAG_ALIGN_RIGHT)
		next_right_goes_below = TRUE;
	}
    }
    החזר result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    صحيح
count_props(linenr_T lnum, صحيح only_starting, صحيح last_line)
{
    char_u	*props;
    صحيح		proplen = get_text_props(curbuf, lnum, &props, 0);
    صحيح		result = proplen;
    textprop_T	prop;

    עבור (صحيح i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * حجم(prop), حجم(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	אם ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_line && prop.tp_col == MAXCOL))
	    --result;
    }
    החזר result;
}

ساكن textprop_T	*text_prop_compare_props;
ساكن buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    ساكن صحيح
text_prop_order(صحيح flags)
{
    אם (flags & TP_FLAG_ALIGN_ABOVE)
	החזר 0;
    אם (flags & TP_FLAG_ALIGN_RIGHT)
	החזר 2;
    אם (flags & TP_FLAG_ALIGN_BELOW)
	החזר 3;
    החזר 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    ساكن صحيح
text_prop_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    صحيح  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(صحيح *)s1;
    idx2 = *(صحيح *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    אם ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	החזר tp1->tp_id < 0 ? 1 : -1;

    אם (col1 == MAXCOL || col2 == MAXCOL)
    {
	صحيح order1 = text_prop_order(tp1->tp_flags);
	صحيح order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	אם (order1 != order2)
	    החזר order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    אם (pt1 != pt2)
    {
	אם (pt1 == عدم)
	    החזר -1;
	אם (pt2 == عدم)
	    החזר 1;
	אם (pt1->pt_priority != pt2->pt_priority)
	    החזר pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    אם (col1 != col2)
	החזר col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    אם (tp1->tp_id < 0)
	החזר tp1->tp_id > tp2->tp_id ? 1 : -1;

    החזר 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    فراغ
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	صحيح	    *idxs,
	صحيح	    count)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((فراغ *)idxs, (size_t)count, حجم(صحيح), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_lines(
	win_T	    *wp,
	صحيح	    type_id,
	صحيح	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    אם (text_prop_type_by_id(wp->w_buffer, type_id) == عدم)
	החזר false;

    אם (first_lnum < 1)
	first_lnum = 1;
    אם (last_lnum > wp->w_buffer->b_ml.ml_line_count)
	last_lnum = wp->w_buffer->b_ml.ml_line_count;

    עבור (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	صحيح	count = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	עבור (صحيح i = 0; i < count; ++i)
	{
	    mch_memmove(prop, props + i * حجم(textprop_T),
							   حجم(textprop_T));
	    אם (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		החזר true;
	    }
	}
    }
    החזר false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	صحيح	    type_id,
	صحيح	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    החזר find_prop_in_lines(wp, type_id, id, prop, found_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    ساكن فراغ
set_text_props(linenr_T lnum, textprop_T *tps, صحيح count)
{
    char_u  *text;
    char_u  *newtext;
    صحيح	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    אם (count == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	אם (newtext == عدم)
	    החזר;
	mch_memmove(newtext, text, textlen);
	אם (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_line_ptr);
	curbuf->b_ml.ml_line_ptr = newtext;
	curbuf->b_ml.ml_line_len = textlen;
	curbuf->b_ml.ml_flags |= ML_LINE_DIRTY;
    }
    אחרת
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_count = (uint16_t)count;
	صحيح	    vtext_size = 0;
	صحيح	    total_len;
	char_u	    *count_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	صحيح	    i;

	עבור (i = 0; i < count; ++i)
	    אם (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (صحيح)PROP_COUNT_SIZE
		    + count * (صحيح)حجم(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	אם (newtext == عدم)
	    החזר;
	mch_memmove(newtext, text, textlen);

	count_dest = newtext + textlen;
	mch_memmove(count_dest, &prop_count, PROP_COUNT_SIZE);

	prop_dest = count_dest + PROP_COUNT_SIZE;
	vtext_dest = prop_dest + count * حجم(textprop_T);

	עבור (i = 0; i < count; ++i)
	{
	    textprop_T prop = tps[i];

	    אם (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		صحيح copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = عدم;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
		vtext_dest += copysize;
	    }
	    אחרת
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, حجم(textprop_T));
	    prop_dest += حجم(textprop_T);
	}

	אם (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_line_ptr);
	curbuf->b_ml.ml_line_ptr = newtext;
	curbuf->b_ml.ml_line_len = total_len;
	curbuf->b_ml.ml_flags |= ML_LINE_DIRTY;
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
props_add_count_header(char_u *line, صحيح line_len, صحيح textlen, صحيح *new_len)
{
    صحيح		prop_bytes = line_len - textlen;
    uint16_t	prop_count;
    صحيح		vtext_size = 0;
    صحيح		total;
    char_u	*newline;
    char_u	*count_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    صحيح		i;

    אם (prop_bytes <= 0 || prop_bytes % حجم(textprop_T) != 0)
    {
	*new_len = line_len;
	החזר عدم;
    }
    prop_count = (uint16_t)(prop_bytes / حجم(textprop_T));

    // Calculate total vtext size.
    עבור (i = 0; i < (صحيح)prop_count; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, line + textlen + i * حجم(textprop_T),
							حجم(textprop_T));
	אם (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (صحيح)PROP_COUNT_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    אם (newline == عدم)
	החזר عدم;
    *new_len = total;

    mch_memmove(newline, line, textlen);
    count_dest = newline + textlen;
    mch_memmove(count_dest, &prop_count, PROP_COUNT_SIZE);
    prop_dest = count_dest + PROP_COUNT_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    עבור (i = 0; i < (صحيح)prop_count; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, line + textlen + i * حجم(textprop_T),
							حجم(textprop_T));
	אם (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    صحيح copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	אחרת
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * حجم(textprop_T), &prop,
							حجم(textprop_T));
    }
    החזר newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    فراغ
add_text_props(linenr_T lnum, textprop_T *text_props, صحيح text_prop_count)
{
    unpacked_memline_T	um;
    صحيح			i;

    um = um_open_at(curbuf, lnum, text_prop_count);
    אם (um.buf == عدم)
	החזר;
    אם (!um_detach(&um))
    {
	um_abort(&um);
	החזר;
    }

    // Grow props array if needed.
    אם (um.prop_count + text_prop_count > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_count + text_prop_count) * حجم(textprop_T));
	אם (newprops == عدم)
	{
	    um_abort(&um);
	    החזר;
	}
	um.props = newprops;
	um.prop_size = um.prop_count + text_prop_count;
    }

    עבור (i = 0; i < text_prop_count; ++i)
	um.props[um.prop_count++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    ساكن صحيح
compare_pt(ثابت فراغ *s1, ثابت فراغ *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    החזר tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    ساكن proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, صحيح id)
{
    صحيح low = 0;
    صحيح high;

    אם (ht == عدم || ht->ht_used == 0)
	החזר عدم;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    אם (*array == عدم)
    {
	long	    todo;
	hashitem_T  *hi;
	صحيح	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	אם (*array == عدم)
	    החזר عدم;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
	{
	    אם (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((فراغ *)*array, ht->ht_used, حجم(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    high = ht->ht_used;
    כלעוד (high > low)
    {
	صحيح m = (high + low) / 2;

	אם ((*array)[m]->pt_id == id)
	    החזר (*array)[m];
	אם ((*array)[m]->pt_id > id)
	    high = m;
	אחרת
	    low = m + 1;
    }
    החזר عدم;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    ساكن فراغ
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    صحيح buflocal = TRUE;
    صحيح virtualtext_prop = prop->tp_id < 0;

    dict_add_number(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    אם (!virtualtext_prop)
    {
	dict_add_number(dict, "length", prop->tp_len);
	dict_add_number(dict, "id", prop->tp_id);
    }
    dict_add_number(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_number(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    אם (pt == عدم)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    אם (pt != عدم)
	dict_add_string(dict, "type", pt->pt_name);

    אם (buflocal)
	dict_add_number(dict, "type_bufnr", buf->b_fnum);
    אחרת
	dict_add_number(dict, "type_bufnr", 0);
    אם (virtualtext_prop)
    {
	string_T    text_align = {عدم, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	אם (prop->tp_flags & TP_FLAG_ALIGN_RIGHT)
	    STR_LITERAL_SET(text_align, "right");
	אחרת אם (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	אחרת אם (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	אם (text_align.string != عدم)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (صحيح)text_align.length);

	// text_wrap
	אם (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	אם (prop->tp_padleft != 0)
	    dict_add_number(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, صحيح id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    אם (type == عدم)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    החזר type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    החזר text_prop_type_by_id(buf, prop->tp_type) != عدم;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    فراغ
f_prop_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    linenr_T start;
    linenr_T end;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    صحيح	    did_clear = FALSE;

    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	החזר;

    start = tv_get_number(&argvars[0]);
    end = start;
    אם (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_number(&argvars[1]);
	אם (argvars[2].v_type != VAR_UNKNOWN)
	{
	    אם (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		החזר;
	}
    }
    אם (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	החזר;
    }

    {
	unpacked_memline_T um = um_open(buf);

	עבור (lnum = start; lnum <= end; ++lnum)
	{
	    صحيح idx;

	    אם (lnum > buf->b_ml.ml_line_count)
		اكسر;
	    אם (!um_goto_line(&um, lnum, 0))
		اكسر;
	    אם (um.prop_count > 0)
	    {
		did_clear = TRUE;
		עבור (idx = um.prop_count - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    אם (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    فراغ
f_prop_find(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    صحيح		lnum_start;
    صحيح		start_pos_has_prop = 0;
    صحيح		seen_end = FALSE;
    صحيح		id = 0;
    صحيح		id_found = FALSE;
    صحيح		type_id = -1;
    صحيح		skipstart = FALSE;
    صحيح		lnum = -1;
    صحيح		col = -1;
    صحيح		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    صحيح		both;

    אם (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	החזר;

    אם (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	החזר;
    dict = argvars[0].vval.v_dict;

    אם (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	החזר;
    אם (buf->b_ml.ml_mfp == عدم)
	החזר;

    אם (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	אם (*dir_s == 'b')
	    dir = BACKWARD;
	אחרת אם (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    החזר;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    אם (di != عدم)
	lnum = tv_get_number(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    אם (di != عدم)
	col = tv_get_number(&di->di_tv);

    אם (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    אחרת אם (col == -1)
	col = 1;

    אם (lnum < 1 || lnum > buf->b_ml.ml_line_count)
    {
	emsg(_(e_invalid_range));
	החזר;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    אם (dict_has_key(dict, "id"))
    {
	id = dict_get_number(dict, "id");
	id_found = TRUE;
    }
    אם (dict_has_key(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	אם (type == عدم)
	    החזר;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    אם (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	החזר;
    }
    אם (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	החזר;
    }

    lnum_start = lnum;

    אם (rettv_dict_alloc(rettv) == FAIL)
	החזר;

    כלעוד (1)
    {
	char_u	*prop_start_ptr;
	صحيح	count = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	صحيح	    i;
	textprop_T  prop;
	صحيح	    prop_start;
	صحيح	    prop_end;

	עבור (i = dir == BACKWARD ? count - 1 : 0; i >= 0 && i < count; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * حجم(textprop_T),
							   حجم(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    אם (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_COUNT_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    אחרת
		prop.u.tp_text = عدم;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    אם (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		אם (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    אם (!is_floating_vtext && prop.tp_col > col)
			استمر;
		}
		אחרת אם (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    استمر;
	    }
	    אם (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		אם (lnum_start == lnum
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
		אם (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		אם (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    אם (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    استمر;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		אם (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    استمر;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_number(rettv->vval.v_dict, "lnum", lnum);

		החזר;
	    }
	}

	אם (dir > 0)
	{
	    אם (lnum >= buf->b_ml.ml_line_count)
		اكسر;
	    lnum++;
	}
	אחרת
	{
	    אם (lnum <= 1)
		اكسر;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    ساكن صحيح
prop_type_or_id_in_list(صحيح *types_or_ids, صحيح len, صحيح type_or_id)
{
    صحيح i;

    עבור (i = 0; i < len; i++)
	אם (types_or_ids[i] == type_or_id)
	    החזר TRUE;

    החזר FALSE;
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
    ساكن فراغ
get_props_in_line(
	buf_T		*buf,
	linenr_T	lnum,
	صحيح		*prop_types,
	صحيح		prop_types_len,
	صحيح		*prop_ids,
	صحيح		prop_ids_len,
	list_T		*retlist,
	صحيح		add_lnum)
{
    char_u	*props;
    صحيح		count;
    صحيح		i;
    textprop_T	prop;

    count = get_text_props(buf, lnum, &props, FALSE);
    עבור (i = 0; i < count; ++i)
    {
	mch_memmove(&prop, props + i * حجم(textprop_T),
		حجم(textprop_T));
	// Convert offset to pointer for virtual text props.
	אם (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_COUNT_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	אחרת
	    prop.u.tp_text = عدم;
	אם ((prop_types == عدم
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == عدم
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    אם (d == عدم)
		اكسر;
	    prop_fill_dict(d, &prop, buf);
	    אם (add_lnum)
		dict_add_number(d, "lnum", lnum);
	    אם (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    ساكن صحيح *
get_prop_types_from_names(list_T *l, buf_T *buf, صحيح *num_types)
{
    صحيح		*prop_types;
    listitem_T	*li;
    صحيح		i;
    char_u	*name;
    proptype_T	*type;

    *num_types = 0;

    prop_types = ALLOC_MULT(صحيح, list_len(l));
    אם (prop_types == عدم)
	החזר عدم;

    i = 0;
    FOR_ALL_LIST_ITEMS(l, li)
    {
	אם (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    اذهب errret;
	}
	name = li->li_tv.vval.v_string;
	אם (name == عدم)
	    اذهب errret;

	type = lookup_prop_type(name, buf);
	אם (type == عدم)
	    اذهب errret;
	prop_types[i++] = type->pt_id;
    }

    *num_types = i;
    החזר prop_types;

errret:
    VIM_CLEAR(prop_types);
    החזר عدم;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    ساكن صحيح *
get_prop_ids_from_list(list_T *l, صحيح *num_ids)
{
    صحيح		*prop_ids;
    listitem_T	*li;
    صحيح		i = 0;
    صحيح		id;
    صحيح		error;

    *num_ids = 0;

    prop_ids = ALLOC_MULT(صحيح, list_len(l));
    אם (prop_ids == عدم)
	החזר عدم;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	error = FALSE;
	id = tv_get_number_chk(&li->li_tv, &error);
	אם (error)
	    اذهب errret;

	prop_ids[i++] = id;
    }

    *num_ids = i;
    החזר prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    החזר عدم;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    فراغ
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    buf_T	*buf = curbuf;
    صحيح		add_lnum = FALSE;
    صحيح		*prop_types = عدم;
    صحيح		prop_types_len = 0;
    صحيح		*prop_ids = عدم;
    صحيح		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	החזר;

    אם (rettv_list_alloc(rettv) == FAIL)
	החזר;

    // default: get text properties on current line
    start_lnum = tv_get_number(&argvars[0]);
    end_lnum = start_lnum;
    אם (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	אם (check_for_dict_arg(argvars, 1) == FAIL)
	    החזר;
	d = argvars[1].vval.v_dict;

	אם (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    החזר;

	אם (d != عدم && (di = dict_find(d, (char_u *)"end_lnum", -1)) != عدم)
	{
	    אם (di->di_tv.v_type != VAR_NUMBER)
	    {
		emsg(_(e_number_required));
		החזר;
	    }
	    end_lnum = tv_get_number(&di->di_tv);
	    אם (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		// line
		end_lnum = buf->b_ml.ml_line_count + end_lnum + 1;
	    אחרת אם (end_lnum > buf->b_ml.ml_line_count)
		end_lnum = buf->b_ml.ml_line_count;
	    add_lnum = TRUE;
	}
	אם (d != عدم && (di = dict_find(d, (char_u *)"types", -1)) != عدم)
	{
	    אם (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		החזר;
	    }

	    l = di->di_tv.vval.v_list;
	    אם (l != عدم && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		אם (prop_types == عدم)
		    החזר;
	    }
	}
	אם (d != عدم && (di = dict_find(d, (char_u *)"ids", -1)) != عدم)
	{
	    אם (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		اذهب errret;
	    }

	    l = di->di_tv.vval.v_list;
	    אם (l != عدم && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		אם (prop_ids == عدم)
		    اذهب errret;
	    }
	}
    }
    אם (start_lnum < 1 || start_lnum > buf->b_ml.ml_line_count
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    אחרת
	עבור (lnum = start_lnum; lnum <= end_lnum; lnum++)
	    get_props_in_line(buf, lnum, prop_types, prop_types_len,
		    prop_ids, prop_ids_len,
		    rettv->vval.v_list, add_lnum);

errret:
    VIM_CLEAR(prop_types);
    VIM_CLEAR(prop_ids);
}

/*
 * prop_remove({props} [, {lnum} [, {lnum_end}]])
 */
    فراغ
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start = 1;
    linenr_T	end = 0;
    linenr_T	lnum;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    صحيح		do_all;
    صحيح		id = -MAXCOL;
    صحيح		type_id = -1;	    // for a single "type"
    صحيح		*type_ids = عدم;   // array, for a list of "types", allocated
    صحيح		num_type_ids = 0;   // number of elements in "type_ids"
    صحيح		both;

    rettv->vval.v_number = 0;

    אם (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	החזר;

    אם (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	החזר;

    אם (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_number(&argvars[1]);
	end = start;
	אם (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_number(&argvars[2]);
	אם (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    החזר;
	}
    }

    dict = argvars[0].vval.v_dict;
    אם (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	החזר;
    אם (buf->b_ml.ml_mfp == عدم)
	החזר;

    do_all = dict_get_bool(dict, "all", FALSE);

    אם (dict_has_key(dict, "id"))
	id = dict_get_number(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    אם (dict_has_key(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	אם (type == عدم)
	    החזר;
	type_id = type->pt_id;
    }
    אם (dict_has_key(dict, "types"))
    {
	typval_T types;
	listitem_T *li = عدم;

	dict_get_tv(dict, "types", &types);
	אם (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( حجم(صحيح) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_ITEMS(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		אם (li->li_tv.v_type != VAR_STRING)
		    استمر;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		אם (!prop_type)
		    اذهب cleanup_prop_remove;

		type_ids[num_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    אם (id == -MAXCOL && (type_id == -1 && num_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	اذهب cleanup_prop_remove;
    }
    אם (both && (id == -MAXCOL || (type_id == -1 && num_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	اذهب cleanup_prop_remove;
    }
    אם (type_id != -1 && num_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	اذهب cleanup_prop_remove;
    }

    אם (end == 0)
	end = buf->b_ml.ml_line_count;

    {
	unpacked_memline_T um = um_open(buf);

	עבור (lnum = start; lnum <= end; ++lnum)
	{
	    صحيح idx;

	    אם (lnum > buf->b_ml.ml_line_count)
		اكسر;
	    אם (!um_goto_line(&um, lnum, 0))
		اكسر;

	    עבור (idx = 0; idx < um.prop_count; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		صحيح	    matches_id = 0;
		صحيح	    matches_type = 0;

		אם (prop->tp_flags & TP_FLAG_DELETED)
		    استمر;

		matches_id = prop->tp_id == id;
		אם (num_type_ids > 0)
		{
		    صحيح idx2;

		    עבור (idx2 = 0; !matches_type && idx2 < num_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		אחרת
		    matches_type = prop->tp_type == type_id;

		אם (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    אם (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_number;
		    אם (!do_all)
			اكسر;
		}
	    }
	}
	um_close(&um);
    }

    אם (first_changed > 0)
    {
	changed_line_display_buf(buf);
	changed_lines_buf(buf, first_changed, last_changed + 1, 0);
	redraw_buf_later(buf, UPD_VALID);
    }

cleanup_prop_remove:
    vim_free(type_ids);
}

/*
 * Common for f_prop_type_add() and f_prop_type_change().
 */
    ساكن فراغ
prop_type_set(typval_T *argvars, صحيح add)
{
    char_u	*name;
    buf_T	*buf = عدم;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    אם (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	החזר;

    name = tv_get_string(&argvars[0]);
    אם (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	החזר;
    }

    אם (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	החזר;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    אם (add)
    {
	hashtab_T **htp;

	אם (prop != عدم)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    החזר;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	אם (prop == عدم)
	    החזר;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	אם (buf == عدم)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	אחרת
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	אם (*htp == عدم)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    אם (*htp == عدم)
	    {
		vim_free(prop);
		החזר;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    אחרת
    {
	אם (prop == عدم)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    החזר;
	}
    }

    אם (dict != عدم)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	אם (di != عدم)
	{
	    char_u	*highlight;
	    صحيح		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    אם (highlight != عدم && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    אם (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == عدم ? (char_u *)"" : highlight);
		החזר;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	אם (di != عدم)
	{
	    אם (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    אחרת
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	אם (di != عدم)
	{
	    אם (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    אחרת
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	אם (di != عدم)
	    prop->pt_priority = tv_get_number(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	אם (di != عدم)
	{
	    אם (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    אחרת
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	אם (di != عدم)
	{
	    אם (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    אחרת
		prop->pt_flags &= ~PT_FLAG_INS_END_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    فراغ
f_prop_type_add(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    فراغ
f_prop_type_change(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    فراغ
f_prop_type_delete(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*name;
    buf_T	*buf = عدم;
    hashitem_T	*hi;

    אם (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	החזר;

    name = tv_get_string(&argvars[0]);
    אם (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	החזר;
    }

    אם (argvars[1].v_type != VAR_UNKNOWN)
    {
	אם (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    החזר;
    }

    hi = find_prop_type_hi(name, buf);
    אם (hi == عدم)
	החזר;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    אם (buf == عدم)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    אחרת
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    changed_window_setting_buf(buf == عدم ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    فراغ
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    אם (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	החזר;

    name = tv_get_string(&argvars[0]);
    אם (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	החזר;
    }

    אם (rettv_dict_alloc(rettv) == FAIL)
	החזר;

    proptype_T  *prop = عدم;
    buf_T	    *buf = عدم;

    אם (argvars[1].v_type != VAR_UNKNOWN)
    {
	אם (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    החזר;
    }

    prop = find_prop_type(name, buf);
    אם (prop == عدم)
	החזר;

    dict_T *d = rettv->vval.v_dict;

    אם (prop->pt_hl_id > 0)
	dict_add_string(d, "highlight", syn_id2name(prop->pt_hl_id));
    dict_add_number(d, "priority", prop->pt_priority);
    dict_add_number(d, "combine",
	    (prop->pt_flags & PT_FLAG_COMBINE) ? 1 : 0);
    dict_add_number(d, "override",
	    (prop->pt_flags & PT_FLAG_OVERRIDE) ? 1 : 0);
    dict_add_number(d, "start_incl",
	    (prop->pt_flags & PT_FLAG_INS_START_INCL) ? 1 : 0);
    dict_add_number(d, "end_incl",
	    (prop->pt_flags & PT_FLAG_INS_END_INCL) ? 1 : 0);
    אם (buf != عدم)
	dict_add_number(d, "bufnr", buf->b_fnum);
}

    ساكن فراغ
list_types(hashtab_T *ht, list_T *l)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	אם (!HASHITEM_EMPTY(hi))
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
    فراغ
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = عدم;

    אם (rettv_list_alloc(rettv) == FAIL)
	החזר;

    אם (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	החזר;

    אם (argvars[0].v_type != VAR_UNKNOWN)
    {
	אם (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    החזר;
    }
    אם (buf == عدم)
    {
	אם (global_proptypes != عدم)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    אחרת אם (buf->b_proptypes != عدم)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    ساكن فراغ
clear_ht_prop_types(hashtab_T *ht)
{
    long	todo;
    hashitem_T	*hi;

    אם (ht == عدم)
	החזר;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	אם (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#אם defined(EXITFREE)
/*
 * Free all global property types.
 */
    فراغ
clear_global_prop_types(فراغ)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = عدم;
    VIM_CLEAR(global_proparray);
}
#endif

/*
 * Free all property types for "buf".
 */
    فراغ
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = عدم;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
تعريفنوع بنية
{
    صحيح dirty;	    // if the property was changed
    صحيح can_drop;   // whether after this change, the prop may be removed
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
    ساكن adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    col,
	صحيح	    added,
	صحيح	    flags)
{
    proptype_T	*pt;
    صحيح		start_incl;
    صحيح		end_incl;
    صحيح		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    אם (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	החזר res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != عدم && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    אם (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != عدم && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    אם (added > 0)
    {
	אם (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	אחרת אם (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    אחרת אם (prop->tp_col > col + 1)
    {
	אם (prop->tp_col + added < col + 1)
	{
	    אם (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    אחרת
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		אם (prop->tp_len <= 0)
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
	אחרת
	    prop->tp_col += added;
    }
    אחרת אם (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	אם (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	אחרת
	{
	    صحيح after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	אם (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    אחרת
	res.dirty = FALSE;

    החזר res;
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
    صحيح
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	صحيح	    bytes_added,
	صحيح	    flags)
{
    unpacked_memline_T um;
    صحيح		dirty = FALSE;
    صحيح		ri;

    אם (text_prop_frozen > 0)
	החזר FALSE;

    um = um_open_at(curbuf, lnum, 0);
    אם (um.buf == عدم || um.prop_count == 0)
    {
	um_abort(&um);
	החזר FALSE;
    }

    עבור (ri = 0; ri < um.prop_count; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	אם (res.dirty)
	{
	    אם (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		אם (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    החזר FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		אם ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    החזר FALSE;
		}
	    }
	    dirty = TRUE;
	}
	אם (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    استمر;
	}
    }
    אם (dirty)
	um_close(&um);
    אחרת
	um_abort(&um);
    החזר dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    فراغ
adjust_props_for_split(
	linenr_T    lnum_props,
	linenr_T    lnum_top,
	صحيح	    kept,
	صحيح	    deleted,
	صحيح	    at_eol)
{
    char_u	*props;
    صحيح		count;
    garray_T    prevprop;
    garray_T    nextprop;
    صحيح		skipped = kept + deleted;

    אם (!curbuf->b_has_textprop)
	החזר;

    // Get the text properties from "lnum_props".
    count = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, حجم(textprop_T), 10);
    ga_init2(&nextprop, حجم(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *count_ptr = props - PROP_COUNT_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    עבור (صحيح i = 0; i < count; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	صحيح	    start_incl, end_incl;
	صحيح	    cont_prev, cont_next;
	صحيح	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * حجم(textprop_T), حجم(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	אם (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(count_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	אחרת
	    prop.u.tp_text = عدم;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != عدم && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != عدم && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	אם (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	אחרת
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    صحيح is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    אם (is_floating_vtext)
		cont_next = FALSE;
	    אחרת
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	אם (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	אם (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    صحيح is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    אם (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    אם (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	אם (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    صحيح is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    אם (p->tp_col != MAXCOL)
	    {
		אם (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		אחרת
		{
		    אם (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    אם (cont_prev)
		p->tp_flags |= TP_FLAG_CONT_PREV;
	}
    }

    set_text_props(lnum_top, (textprop_T *)prevprop.ga_data,
							    prevprop.ga_len);
    ga_clear(&prevprop);
    set_text_props(lnum_top + 1, (textprop_T *)nextprop.ga_data,
							    nextprop.ga_len);
    ga_clear(&nextprop);
}

/*
 * Prepend properties of joined line "lnum" to the unpacked memline "um".
 * Properties are added in reverse order; caller should call
 * um_reverse_props() after all lines are processed.
 */
    فراغ
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	صحيح		    last_line,
	long		    col,
	صحيح		    removed)
{
    unpacked_memline_T	r_um;

    אם (um->buf == عدم)
	החזר;

    r_um = um_open_at(um->buf, lnum, 0);
    אם (r_um.buf == عدم)
	החזר;
    אם (!um_detach(&r_um))
    {
	um_abort(&r_um);
	החזר;
    }

    עבור (صحيح i = r_um.prop_count - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	صحيح	    end;

	אם (prop->tp_col == MAXCOL && !last_line)
	    استمر;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	אם (last_line || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = عدم;  // ownership transferred
	}
	אחרת
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    עבור (صحيح j = 0; j < um->prop_count; ++j)
	    {
		textprop_T *op = &um->props[j];

		אם ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    اكسر;
		}
	    }
	    אם (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    ساكن فراغ
remember_cleared_prop(garray_T *gap, صحيح which, textprop_T *prop, صحيح flag)
{
    undoprop_T	*up;

    אם (which == UNDOPROP_NONE)
	החזר;
    אם (gap->ga_itemsize == 0)
	ga_init2(gap, حجم(undoprop_T), 4);
    אם (ga_grow(gap, 1) == FAIL)
	החזר;
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
    فراغ
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	صحيح	    above_which,
	صحيح	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = عدم;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    אם (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_line_count)
	החזר;

    above = um_open_at(buf, lnum, 0);
    אם (above.buf == عدم)
	החזר;
    עבור (صحيح i = 0; i < above.prop_count; ++i)
	אם (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    אם (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	החזר;
    }

    below = um_open_at(buf, lnum + 1, 0);
    אם (below.buf == عدم)
    {
	um_abort(&above);
	החזר;
    }
    אם (below.prop_count > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_count);
	אם (paired == عدم)
	    اذهب theend;
    }

    עבור (صحيح i = 0; i < above.prop_count; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	אם (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    استمر;
	עבור (صحيح j = 0; j < below.prop_count; ++j)
	{
	    textprop_T *next = &below.props[j];

	    אם (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		اكسر;
	    }
	}
	אם (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    עבור (صحيح j = 0; j < below.prop_count; ++j)
    {
	אם (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    استمر;
	אם (!below.detached && !um_detach(&below))
	    اكسر;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    אם (below_changed)
	um_close(&below);
    אחרת
	um_abort(&below);
    אם (above_changed)
	um_close(&above);
    אחרת
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    فراغ
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	صحيح	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    אם (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_line_count)
	החזר;
    um = um_open_at(buf, lnum, 0);
    אם (um.buf == عدم)
	החזר;
    עבור (صحيح r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	אם (up->up_which != which)
	    استمر;
	עבור (صحيح i = 0; i < um.prop_count; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    אם (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		استمر;
	    אם (!um.detached && !um_detach(&um))
		اذهب theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    اكسر;
	}
    }

theend:
    אם (changed)
	um_close(&um);
    אחרת
	um_abort(&um);
}

    bool
text_prop_count_valid(صحيح prop_count, size_t propdata_len)
{
    אם (propdata_len < PROP_COUNT_SIZE)
	החזר false;
    החזר (size_t)prop_count * حجم(textprop_T)
		    <= propdata_len - PROP_COUNT_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, صحيح prop_count, size_t propdata_len)
{
    עבור (صحيح i = 0; i < prop_count; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * حجم(textprop_T),
							  حجم(textprop_T));
	אם (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    استمر;
	אם (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    החזר false;
    }
    החזר true;
}

#endif // FEAT_PROP_POPUP
