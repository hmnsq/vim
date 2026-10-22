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

#include "vim.h"

#якщо defined(FEAT_PROP_POPUP)

static void um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    static void
um_free_vtext(textprop_T *props, ціле count)
{
    для (ціле i = 0; i < count; ++i)
	якщо (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    static void
um_free(unpacked_memline_T *um)
{
    якщо (um->detached)
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
    повернути um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_line(unpacked_memline_T *um, linenr_T lnum, ціле extra_props)
{
    char_u	*line;
    size_t	textlen;
    size_t	propdata_len;
    ціле		proplen;

    якщо (um->buf == NULL)
	повернути false;

    // Store changes to the current line if any.
    якщо (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    якщо (lnum == 0)
	повернути true;	// just unload

    line = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_line_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = line;

    якщо (propdata_len == 0)
	повернути true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    якщо (propdata_len < PROP_COUNT_SIZE + sizeof(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	повернути false;
    }

    uint16_t    prop_count;
    char_u	    *count_ptr = line + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_count, count_ptr, PROP_COUNT_SIZE);
    якщо (!text_prop_count_valid(prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	повернути false;
    }
    proplen = (ціле)prop_count;
    props_start = count_ptr + PROP_COUNT_SIZE;

    якщо (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	повернути false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    якщо (um->props == NULL)
    {
	um->buf = NULL;
	повернути false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_count = proplen;
    mch_memmove(um->props, props_start, proplen * sizeof(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    для (ціле i = 0; i < proplen; ++i)
    {
	якщо (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = count_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	інакше
	    um->props[i].u.tp_text = NULL;
    }

    повернути true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, ціле extra_props)
{
    unpacked_memline_T um = um_open(buf);

    якщо (!um_goto_line(&um, lnum, extra_props))
	um.buf = NULL;
    повернути um;
}

/*
 * Make the unpacked memline writable by copying text and virtual text
 * strings to allocated memory (LOADED -> DETACHED).
 * Returns true on success.
 */
    static bool
um_detach(unpacked_memline_T *um)
{
    char_u  *newtext;

    якщо (um->detached || um->buf == NULL)
	повернути um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    якщо (newtext == NULL)
    {
	um->buf = NULL;
	повернути false;
    }
    um->text = newtext;

    для (ціле i = 0; i < um->prop_count; ++i)
    {
	якщо (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    якщо (copy == NULL)
	    {
		для (ціле j = 0; j < i; ++j)
		    якщо (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = NULL;
		повернути false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    повернути true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    якщо (um->buf == NULL)
	повернути false;
    якщо (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    повернути true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    static bool
um_grow_props(unpacked_memline_T *um, ціле needed)
{
    якщо (um->prop_count + needed <= um->prop_size)
	повернути true;

    ціле	    new_size = um->prop_count + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * sizeof(textprop_T));
    якщо (newprops == NULL)
	повернути false;
    um->props = newprops;
    um->prop_size = new_size;

    повернути true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    static void
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    якщо (um->buf == NULL)
	повернути;
    якщо (!um->detached && !um_detach(um))
	повернути;
    якщо (!um_grow_props(um, 1))
    {
	um->buf = NULL;
	повернути;
    }
    um->props[um->prop_count++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    void
um_reverse_props(unpacked_memline_T *um)
{
    для (ціле i = 0, j = um->prop_count - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, ціле prop_count)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    якщо (lnum < 1 || lnum > buf->b_ml.ml_line_count)
    {
	um.buf = NULL;
	повернути um;
    }

    char_u *line = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(line);

    якщо (text_copy == NULL)
    {
	um.buf = NULL;
	повернути um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    якщо (prop_count > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_count);
	якщо (um.props == NULL)
	{
	    vim_free(um.text);
	    um.buf = NULL;
	    повернути um;
	}
	um.prop_size = prop_count;
    }

    повернути um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    static void
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					ціле tp_id, ціле tp_type, ціле flag)
{
    unpacked_memline_T	neighbor;

    якщо (lnum < 1 || lnum > buf->b_ml.ml_line_count)
	повернути;

    neighbor = um_open_at(buf, lnum, 0);
    якщо (neighbor.buf == NULL)
	повернути;

    для (ціле i = 0; i < neighbor.prop_count; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	якщо (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    якщо (!neighbor.detached && !um_detach(&neighbor))
		break;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    break;
	}
    }
    um_close(&neighbor);
}

/*
 * Mark a property for deletion and free its virtual text if any.
 * Automatically detaches if needed.
 * Also adjusts continuation flags on neighboring lines.
 */
    void
um_delete_prop(unpacked_memline_T *um, ціле index)
{
    textprop_T *prop;

    якщо (um->buf == NULL || index < 0 || index >= um->prop_count)
	повернути;
    якщо (!um->detached && !um_detach(um))
	повернути;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    якщо (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    якщо (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    якщо (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
    static char_u *
um_pack(unpacked_memline_T *um, ціле *packed_len)
{
    uint16_t	live_count = 0;
    ціле		vtext_size = 0;
    ціле		total_len;
    char_u	*buf;
    char_u	*count_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    для (ціле i = 0; i < um->prop_count; ++i)
    {
	textprop_T *prop = &um->props[i];

	якщо (prop->tp_flags & TP_FLAG_DELETED)
	    continue;
	++live_count;
	якщо (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    якщо (live_count == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	якщо (buf == NULL)
	    повернути NULL;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	повернути buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (ціле)PROP_COUNT_SIZE
		+ live_count * (ціле)sizeof(textprop_T) + vtext_size;
    buf = alloc(total_len);
    якщо (buf == NULL)
	повернути NULL;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    count_dest = buf + um->text_size;
    mch_memmove(count_dest, &live_count, PROP_COUNT_SIZE);

    prop_dest = count_dest + PROP_COUNT_SIZE;
    vtext_dest = prop_dest + live_count * sizeof(textprop_T);

    для (ціле i = 0; i < um->prop_count; ++i)
    {
	textprop_T prop = um->props[i];

	якщо (prop.tp_flags & TP_FLAG_DELETED)
	    continue;

	якщо (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    ціле copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	інакше
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, sizeof(textprop_T));
	prop_dest += sizeof(textprop_T);
    }

    повернути buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    static void
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    ціле	    packed_len;

    якщо (!um->detached || um->buf == NULL || um->lnum == 0)
	повернути;

    packed = um_pack(um, &packed_len);
    якщо (packed == NULL)
    {
	um->buf = NULL;
	повернути;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    якщо (um->buf->b_ml.ml_line_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    інакше якщо (!(um->buf->b_ml.ml_flags & ML_LINE_DIRTY))
	(void)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_count);

    якщо (um->buf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
    void
um_close(unpacked_memline_T *um)
{
    якщо (um->buf == NULL)
	повернути;
    якщо (um->detached && um->lnum > 0)
	um_store_changes(um);
    um_free(um);
    um->buf = NULL;
}

/*
 * Abort an unpacked memline, discarding any changes.
 */
    void
um_abort(unpacked_memline_T *um)
{
    um_free(um);
    um->buf = NULL;
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
static hashtab_T *global_proptypes = NULL;
static proptype_T **global_proparray = NULL;

// The last used text property type ID.
static ціле proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    static hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    якщо (*name == NUL)
	повернути NULL;
    якщо (buf == NULL)
	ht = global_proptypes;
    інакше
	ht = buf->b_proptypes;

    якщо (ht == NULL)
	повернути NULL;
    hi = hash_find(ht, name);
    якщо (HASHITEM_EMPTY(hi))
	повернути NULL;
    повернути hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    static proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    якщо (hi == NULL)
	повернути NULL;
    повернути HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    ціле
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    якщо (pt == NULL)
	повернути 0;
    повернути pt->pt_id;
}

/*
 * Lookup a property type by name.  First in "buf" and when not found in the
 * global types.
 * When not found gives an error message and returns NULL.
 */
    static proptype_T *
lookup_prop_type(char_u *name, buf_T *buf)
{
    proptype_T *type = find_prop_type(name, buf);

    якщо (type == NULL)
	type = find_prop_type(name, NULL);
    якщо (type == NULL)
	semsg(_(e_property_type_str_does_not_exist), name);
    повернути type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    static ціле
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    якщо (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	повернути FAIL;
    }
    якщо (arg->vval.v_dict == NULL)
	повернути OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    якщо (di != NULL && (di->di_tv.v_type != VAR_NUMBER
					      || di->di_tv.vval.v_number != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	якщо (*buf == NULL)
	    повернути FAIL;
    }
    повернути OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    void
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    якщо (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	повернути;

    start_lnum = tv_get_number(&argvars[0]);
    start_col = tv_get_number(&argvars[1]);
    якщо (check_for_dict_arg(argvars, 2) == FAIL)
	повернути;

    rettv->vval.v_number = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    static ціле
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	ціле		id,
	char_u		*text_arg,
	ціле		text_padding_left,
	ціле		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    ціле		proplen;
    char_u	*props = NULL;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    ціле		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    ціле		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    якщо (type == NULL)
	goto theend;

    якщо (start_lnum < 1 || start_lnum > buf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), (long)start_lnum);
	goto theend;
    }
    якщо (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), (long)end_lnum);
	goto theend;
    }

    якщо (buf->b_ml.ml_mfp == NULL)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	goto theend;
    }

    якщо (text != NULL)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	для (p = text; *p != NUL; MB_PTR_ADV(p))
	    якщо (*p < ' ')
		*p = ' ';
    }

    для (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	якщо (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_line));
	    goto theend;
	}

	якщо (lnum == start_lnum)
	    col = start_col;
	інакше
	    col = 1;
	якщо (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != NULL))
	{
	    semsg(_(e_invalid_column_number_nr), (long)start_col);
	    goto theend;
	}
	sort_col = col;

	якщо (lnum == end_lnum)
	    length = end_col - col;
	інакше
	    length = (ціле)textlen - col + 1;
	якщо (length > (long)textlen)
	    length = (ціле)textlen;	// can include the end-of-line
	якщо (length < 0)
	    length = 0;		// zero-width property

	якщо (text_arg != NULL)
	{
	    length = (long)STRLEN(text_arg);
	    якщо (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		якщо ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	якщо (text_arg != NULL)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	для (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * sizeof(textprop_T),
							   sizeof(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    якщо (prop_col >= sort_col)
		break;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	ціле		vtext_total = 0;
	ціле		new_line_len;
	char_u	*count_dest;
	char_u	*vtext_dest;
	ціле		j;

	// Compute total vtext size from existing props.
	для (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * sizeof(textprop_T),
							sizeof(textprop_T));
	    якщо (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	якщо (text_arg != NULL)
	    vtext_total += length + 1;

	new_line_len = (ціле)textlen + (ціле)PROP_COUNT_SIZE
			+ new_propcount * (ціле)sizeof(textprop_T)
			+ vtext_total;
	newtext = alloc(new_line_len);
	якщо (newtext == NULL)
	    goto theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_line_ptr, textlen);

	// Write prop_count.
	count_dest = newtext + textlen;
	mch_memmove(count_dest, &new_propcount, PROP_COUNT_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = count_dest + PROP_COUNT_SIZE;
	якщо (i > 0)
	    mch_memmove(newprops, props, sizeof(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	якщо (i < proplen)
	    mch_memmove(newprops + (i + 1) * sizeof(textprop_T),
					props + i * sizeof(textprop_T),
					sizeof(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * sizeof(textprop_T);
	для (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    якщо (j == i)
		continue;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * sizeof(textprop_T),
		    sizeof(textprop_T));
	    якщо (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_count = (char_u *)props - PROP_COUNT_SIZE;
		char_u *old_vtext = old_count + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
		mch_memmove(newprops + j * sizeof(textprop_T), &ep,
			sizeof(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	якщо (text_arg != NULL)
	{
	    ціле copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	інакше
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * sizeof(textprop_T), &tmp_prop,
							sizeof(textprop_T));

	якщо (buf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
    повернути res;
}

/*
 * prop_add_list()
 * First argument specifies the text property:
 *   {'type': <str>, 'id': <num>, 'bufnr': <num>}
 * Second argument is a List where each item is a List with the following
 * entries: [lnum, start_col, end_col]
 */
    void
f_prop_add_list(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    char_u	*type_name;
    buf_T	*buf = curbuf;
    ціле		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    ціле		error = FALSE;
    ціле		prev_did_emsg = did_emsg;

    якщо (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	повернути;

    якщо (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	повернути;

    dict = argvars[0].vval.v_dict;
    якщо (dict == NULL || !dict_has_key(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	повернути;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    якщо (dict_has_key(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_number(dict, "id");
	якщо (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    повернути;
	}
	id = (ціле)x;
    }

    якщо (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	повернути;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_ITEMS(argvars[1].vval.v_list, li)
    {
	якщо (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == NULL)
	{
	    emsg(_(e_list_required));
	    повернути;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	якщо (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	якщо (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	якщо (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	ціле this_id = id;
	якщо (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	якщо (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    якщо (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    повернути;
	}
	якщо (prop_add_one(buf, type_name, this_id, NULL, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    повернути;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
static ціле vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    static ціле
get_textprop_id(void)
{
    якщо (vt_id_counter > 0)
	vt_id_counter = 0;
    повернути --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    ціле
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
    ціле		id = 0;
    char_u	*text = NULL;
    ціле		text_padding_left = 0;
    ціле		flags = 0;

    якщо (dict == NULL || !dict_has_key(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	goto theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    якщо (dict_has_key(dict, "end_lnum"))
    {
	end_lnum = dict_get_number(dict, "end_lnum");
	якщо (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_value_for_argument_str), "end_lnum");
	    goto theend;
	}
    }
    інакше
	end_lnum = start_lnum;

    якщо (dict_has_key(dict, "length"))
    {
	long length = dict_get_number(dict, "length");

	якщо (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_value_for_argument_str), "length");
	    goto theend;
	}
	end_col = start_col + length;
    }
    інакше якщо (dict_has_key(dict, "end_col"))
    {
	end_col = dict_get_number(dict, "end_col");
	якщо (end_col <= 0)
	{
	    semsg(_(e_invalid_value_for_argument_str), "end_col");
	    goto theend;
	}
    }
    інакше якщо (start_lnum == end_lnum)
	end_col = start_col;
    інакше
	end_col = 1;

    якщо (dict_has_key(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_number(dict, "id");
	якщо (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    goto theend;
	}
	id = (ціле)x;
    }

    якщо (dict_has_key(dict, "text"))
    {
	якщо (dict_has_key(dict, "length")
		|| dict_has_key(dict, "end_col")
		|| dict_has_key(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    goto theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	якщо (text == NULL)
	    goto theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	якщо (dict_has_key(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    якщо (p == NULL)
		goto theend;
	    якщо (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		goto theend;
	    }
	    якщо (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_RIGHT;
	    інакше якщо (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    інакше якщо (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    інакше якщо (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "text_align", p);
		goto theend;
	    }
	}

	якщо (dict_has_key(dict, "text_padding_left"))
	{
	    text_padding_left = dict_get_number(dict, "text_padding_left");
	    якщо (text_padding_left < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		goto theend;
	    }
	}

	якщо (dict_has_key(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    якщо (p == NULL)
		goto theend;
	    якщо (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    інакше якщо (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "text_wrap", p);
		goto theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    якщо (start_col < (text == NULL ? 1 : 0))
    {
	semsg(_(e_invalid_column_number_nr), (long)start_col);
	goto theend;
    }
    якщо (start_col > 0 && text_padding_left > 0)
    {
	emsg(_(e_can_only_use_left_padding_when_column_is_zero));
	goto theend;
    }

    якщо (dict_arg != NULL && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	goto theend;

    якщо (text != NULL)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    інакше якщо (id < 0)
    {
	emsg(_(e_cannot_use_negative_id));
	goto theend;
    }

    // This must be done _before_ we add the property because property changes
    // trigger buffer (memline) reorganisation, which needs this flag to be
    // correctly set.
    buf->b_has_textprop = true;  // this is never reset

    prop_add_one(buf, type_name, id, text, text_padding_left, flags,
				    start_lnum, end_lnum, start_col, end_col);
    text = NULL;

    redraw_buf_later(buf, UPD_VALID);

theend:
    vim_free(text);
    повернути id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    ціле
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, ціле will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_count;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    якщо ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == NULL)
	повернути 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_line_len - textlen;
    якщо (propdata_len == 0)
	повернути 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    якщо (propdata_len < PROP_COUNT_SIZE + sizeof(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	повернути 0;
    }
    mch_memmove(&prop_count, text + textlen, PROP_COUNT_SIZE);
    якщо (!text_prop_count_valid(prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	повернути 0;
    }
    якщо (!text_prop_vtext_valid(text + textlen + PROP_COUNT_SIZE,
					   (ціле)prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	повернути 0;
    }
    *props = text + textlen + PROP_COUNT_SIZE;
    повернути (ціле)prop_count;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    ціле
prop_count_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    ціле		count = get_text_props(buf, lnum, &props, FALSE);
    ціле		result = 0;
    textprop_T	prop;
    ціле		i;
    ціле		next_right_goes_below = FALSE;

    якщо (count == 0)
	повернути 0;
    для (i = 0; i < count; ++i)
    {
	mch_memmove(&prop, props + i * sizeof(prop), sizeof(prop));
	якщо (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    якщо ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_right_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_RIGHT)))
	    {
		++result;
	    }
	    інакше якщо (prop.tp_flags & TP_FLAG_ALIGN_RIGHT)
		next_right_goes_below = TRUE;
	}
    }
    повернути result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    ціле
count_props(linenr_T lnum, ціле only_starting, ціле last_line)
{
    char_u	*props;
    ціле		proplen = get_text_props(curbuf, lnum, &props, 0);
    ціле		result = proplen;
    textprop_T	prop;

    для (ціле i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * sizeof(prop), sizeof(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	якщо ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_line && prop.tp_col == MAXCOL))
	    --result;
    }
    повернути result;
}

static textprop_T	*text_prop_compare_props;
static buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    static ціле
text_prop_order(ціле flags)
{
    якщо (flags & TP_FLAG_ALIGN_ABOVE)
	повернути 0;
    якщо (flags & TP_FLAG_ALIGN_RIGHT)
	повернути 2;
    якщо (flags & TP_FLAG_ALIGN_BELOW)
	повернути 3;
    повернути 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    static ціле
text_prop_compare(const void *s1, const void *s2)
{
    ціле  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(ціле *)s1;
    idx2 = *(ціле *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    якщо ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	повернути tp1->tp_id < 0 ? 1 : -1;

    якщо (col1 == MAXCOL || col2 == MAXCOL)
    {
	ціле order1 = text_prop_order(tp1->tp_flags);
	ціле order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	якщо (order1 != order2)
	    повернути order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    якщо (pt1 != pt2)
    {
	якщо (pt1 == NULL)
	    повернути -1;
	якщо (pt2 == NULL)
	    повернути 1;
	якщо (pt1->pt_priority != pt2->pt_priority)
	    повернути pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    якщо (col1 != col2)
	повернути col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    якщо (tp1->tp_id < 0)
	повернути tp1->tp_id > tp2->tp_id ? 1 : -1;

    повернути 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    void
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	ціле	    *idxs,
	ціле	    count)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((void *)idxs, (size_t)count, sizeof(ціле), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_lines(
	win_T	    *wp,
	ціле	    type_id,
	ціле	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    якщо (text_prop_type_by_id(wp->w_buffer, type_id) == NULL)
	повернути false;

    якщо (first_lnum < 1)
	first_lnum = 1;
    якщо (last_lnum > wp->w_buffer->b_ml.ml_line_count)
	last_lnum = wp->w_buffer->b_ml.ml_line_count;

    для (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	ціле	count = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	для (ціле i = 0; i < count; ++i)
	{
	    mch_memmove(prop, props + i * sizeof(textprop_T),
							   sizeof(textprop_T));
	    якщо (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		повернути true;
	    }
	}
    }
    повернути false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	ціле	    type_id,
	ціле	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    повернути find_prop_in_lines(wp, type_id, id, prop, found_lnum,
					  wp->w_topline, wp->w_botline - 1);
}

/*
 * Set the text properties for line "lnum" to "tps" array with "count" entries.
 * If "count" is zero text properties are removed.
 * Any existing text properties are dropped.
 * For virtual text props, u.tp_text must point to the vtext string.
 * Only works for the current buffer.
 */
    static void
set_text_props(linenr_T lnum, textprop_T *tps, ціле count)
{
    char_u  *text;
    char_u  *newtext;
    ціле	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    якщо (count == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	якщо (newtext == NULL)
	    повернути;
	mch_memmove(newtext, text, textlen);
	якщо (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_line_ptr);
	curbuf->b_ml.ml_line_ptr = newtext;
	curbuf->b_ml.ml_line_len = textlen;
	curbuf->b_ml.ml_flags |= ML_LINE_DIRTY;
    }
    інакше
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_count = (uint16_t)count;
	ціле	    vtext_size = 0;
	ціле	    total_len;
	char_u	    *count_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	ціле	    i;

	для (i = 0; i < count; ++i)
	    якщо (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (ціле)PROP_COUNT_SIZE
		    + count * (ціле)sizeof(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	якщо (newtext == NULL)
	    повернути;
	mch_memmove(newtext, text, textlen);

	count_dest = newtext + textlen;
	mch_memmove(count_dest, &prop_count, PROP_COUNT_SIZE);

	prop_dest = count_dest + PROP_COUNT_SIZE;
	vtext_dest = prop_dest + count * sizeof(textprop_T);

	для (i = 0; i < count; ++i)
	{
	    textprop_T prop = tps[i];

	    якщо (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		ціле copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = NULL;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
		vtext_dest += copysize;
	    }
	    інакше
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, sizeof(textprop_T));
	    prop_dest += sizeof(textprop_T);
	}

	якщо (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
props_add_count_header(char_u *line, ціле line_len, ціле textlen, ціле *new_len)
{
    ціле		prop_bytes = line_len - textlen;
    uint16_t	prop_count;
    ціле		vtext_size = 0;
    ціле		total;
    char_u	*newline;
    char_u	*count_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    ціле		i;

    якщо (prop_bytes <= 0 || prop_bytes % sizeof(textprop_T) != 0)
    {
	*new_len = line_len;
	повернути NULL;
    }
    prop_count = (uint16_t)(prop_bytes / sizeof(textprop_T));

    // Calculate total vtext size.
    для (i = 0; i < (ціле)prop_count; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, line + textlen + i * sizeof(textprop_T),
							sizeof(textprop_T));
	якщо (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (ціле)PROP_COUNT_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    якщо (newline == NULL)
	повернути NULL;
    *new_len = total;

    mch_memmove(newline, line, textlen);
    count_dest = newline + textlen;
    mch_memmove(count_dest, &prop_count, PROP_COUNT_SIZE);
    prop_dest = count_dest + PROP_COUNT_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    для (i = 0; i < (ціле)prop_count; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, line + textlen + i * sizeof(textprop_T),
							sizeof(textprop_T));
	якщо (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    ціле copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	інакше
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * sizeof(textprop_T), &prop,
							sizeof(textprop_T));
    }
    повернути newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    void
add_text_props(linenr_T lnum, textprop_T *text_props, ціле text_prop_count)
{
    unpacked_memline_T	um;
    ціле			i;

    um = um_open_at(curbuf, lnum, text_prop_count);
    якщо (um.buf == NULL)
	повернути;
    якщо (!um_detach(&um))
    {
	um_abort(&um);
	повернути;
    }

    // Grow props array if needed.
    якщо (um.prop_count + text_prop_count > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_count + text_prop_count) * sizeof(textprop_T));
	якщо (newprops == NULL)
	{
	    um_abort(&um);
	    повернути;
	}
	um.props = newprops;
	um.prop_size = um.prop_count + text_prop_count;
    }

    для (i = 0; i < text_prop_count; ++i)
	um.props[um.prop_count++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    static ціле
compare_pt(const void *s1, const void *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    повернути tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    static proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, ціле id)
{
    ціле low = 0;
    ціле high;

    якщо (ht == NULL || ht->ht_used == 0)
	повернути NULL;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    якщо (*array == NULL)
    {
	long	    todo;
	hashitem_T  *hi;
	ціле	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	якщо (*array == NULL)
	    повернути NULL;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
	{
	    якщо (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((void *)*array, ht->ht_used, sizeof(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    high = ht->ht_used;
    поки (high > low)
    {
	ціле m = (high + low) / 2;

	якщо ((*array)[m]->pt_id == id)
	    повернути (*array)[m];
	якщо ((*array)[m]->pt_id > id)
	    high = m;
	інакше
	    low = m + 1;
    }
    повернути NULL;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    static void
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    ціле buflocal = TRUE;
    ціле virtualtext_prop = prop->tp_id < 0;

    dict_add_number(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    якщо (!virtualtext_prop)
    {
	dict_add_number(dict, "length", prop->tp_len);
	dict_add_number(dict, "id", prop->tp_id);
    }
    dict_add_number(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_number(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    якщо (pt == NULL)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    якщо (pt != NULL)
	dict_add_string(dict, "type", pt->pt_name);

    якщо (buflocal)
	dict_add_number(dict, "type_bufnr", buf->b_fnum);
    інакше
	dict_add_number(dict, "type_bufnr", 0);
    якщо (virtualtext_prop)
    {
	string_T    text_align = {NULL, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	якщо (prop->tp_flags & TP_FLAG_ALIGN_RIGHT)
	    STR_LITERAL_SET(text_align, "right");
	інакше якщо (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	інакше якщо (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	якщо (text_align.string != NULL)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (ціле)text_align.length);

	// text_wrap
	якщо (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	якщо (prop->tp_padleft != 0)
	    dict_add_number(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, ціле id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    якщо (type == NULL)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    повернути type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    повернути text_prop_type_by_id(buf, prop->tp_type) != NULL;
}

/*
 * prop_clear({lnum} [, {lnum_end} [, {bufnr}]])
 */
    void
f_prop_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    linenr_T start;
    linenr_T end;
    linenr_T lnum;
    buf_T    *buf = curbuf;
    ціле	    did_clear = FALSE;

    якщо (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	повернути;

    start = tv_get_number(&argvars[0]);
    end = start;
    якщо (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_number(&argvars[1]);
	якщо (argvars[2].v_type != VAR_UNKNOWN)
	{
	    якщо (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		повернути;
	}
    }
    якщо (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	повернути;
    }

    {
	unpacked_memline_T um = um_open(buf);

	для (lnum = start; lnum <= end; ++lnum)
	{
	    ціле idx;

	    якщо (lnum > buf->b_ml.ml_line_count)
		break;
	    якщо (!um_goto_line(&um, lnum, 0))
		break;
	    якщо (um.prop_count > 0)
	    {
		did_clear = TRUE;
		для (idx = um.prop_count - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    якщо (did_clear)
	redraw_buf_later(buf, UPD_NOT_VALID);
}

/*
 * prop_find({props} [, {direction}])
 */
    void
f_prop_find(typval_T *argvars, typval_T *rettv)
{
    pos_T       *cursor = &curwin->w_cursor;
    dict_T      *dict;
    buf_T       *buf = curbuf;
    dictitem_T  *di;
    ціле		lnum_start;
    ціле		start_pos_has_prop = 0;
    ціле		seen_end = FALSE;
    ціле		id = 0;
    ціле		id_found = FALSE;
    ціле		type_id = -1;
    ціле		skipstart = FALSE;
    ціле		lnum = -1;
    ціле		col = -1;
    ціле		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    ціле		both;

    якщо (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	повернути;

    якщо (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	повернути;
    dict = argvars[0].vval.v_dict;

    якщо (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	повернути;
    якщо (buf->b_ml.ml_mfp == NULL)
	повернути;

    якщо (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	якщо (*dir_s == 'b')
	    dir = BACKWARD;
	інакше якщо (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    повернути;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    якщо (di != NULL)
	lnum = tv_get_number(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    якщо (di != NULL)
	col = tv_get_number(&di->di_tv);

    якщо (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    інакше якщо (col == -1)
	col = 1;

    якщо (lnum < 1 || lnum > buf->b_ml.ml_line_count)
    {
	emsg(_(e_invalid_range));
	повернути;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    якщо (dict_has_key(dict, "id"))
    {
	id = dict_get_number(dict, "id");
	id_found = TRUE;
    }
    якщо (dict_has_key(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	якщо (type == NULL)
	    повернути;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    якщо (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	повернути;
    }
    якщо (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	повернути;
    }

    lnum_start = lnum;

    якщо (rettv_dict_alloc(rettv) == FAIL)
	повернути;

    поки (1)
    {
	char_u	*prop_start_ptr;
	ціле	count = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	ціле	    i;
	textprop_T  prop;
	ціле	    prop_start;
	ціле	    prop_end;

	для (i = dir == BACKWARD ? count - 1 : 0; i >= 0 && i < count; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * sizeof(textprop_T),
							   sizeof(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    якщо (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_COUNT_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    інакше
		prop.u.tp_text = NULL;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    якщо (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		якщо (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    якщо (!is_floating_vtext && prop.tp_col > col)
			continue;
		}
		інакше якщо (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    continue;
	    }
	    якщо (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		якщо (lnum_start == lnum
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
		якщо (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		якщо (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    якщо (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    continue;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		якщо (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    continue;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_number(rettv->vval.v_dict, "lnum", lnum);

		повернути;
	    }
	}

	якщо (dir > 0)
	{
	    якщо (lnum >= buf->b_ml.ml_line_count)
		break;
	    lnum++;
	}
	інакше
	{
	    якщо (lnum <= 1)
		break;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    static ціле
prop_type_or_id_in_list(ціле *types_or_ids, ціле len, ціле type_or_id)
{
    ціле i;

    для (i = 0; i < len; i++)
	якщо (types_or_ids[i] == type_or_id)
	    повернути TRUE;

    повернути FALSE;
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
    static void
get_props_in_line(
	buf_T		*buf,
	linenr_T	lnum,
	ціле		*prop_types,
	ціле		prop_types_len,
	ціле		*prop_ids,
	ціле		prop_ids_len,
	list_T		*retlist,
	ціле		add_lnum)
{
    char_u	*props;
    ціле		count;
    ціле		i;
    textprop_T	prop;

    count = get_text_props(buf, lnum, &props, FALSE);
    для (i = 0; i < count; ++i)
    {
	mch_memmove(&prop, props + i * sizeof(textprop_T),
		sizeof(textprop_T));
	// Convert offset to pointer for virtual text props.
	якщо (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_COUNT_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	інакше
	    prop.u.tp_text = NULL;
	якщо ((prop_types == NULL
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == NULL
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    якщо (d == NULL)
		break;
	    prop_fill_dict(d, &prop, buf);
	    якщо (add_lnum)
		dict_add_number(d, "lnum", lnum);
	    якщо (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    static ціле *
get_prop_types_from_names(list_T *l, buf_T *buf, ціле *num_types)
{
    ціле		*prop_types;
    listitem_T	*li;
    ціле		i;
    char_u	*name;
    proptype_T	*type;

    *num_types = 0;

    prop_types = ALLOC_MULT(ціле, list_len(l));
    якщо (prop_types == NULL)
	повернути NULL;

    i = 0;
    FOR_ALL_LIST_ITEMS(l, li)
    {
	якщо (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    goto errret;
	}
	name = li->li_tv.vval.v_string;
	якщо (name == NULL)
	    goto errret;

	type = lookup_prop_type(name, buf);
	якщо (type == NULL)
	    goto errret;
	prop_types[i++] = type->pt_id;
    }

    *num_types = i;
    повернути prop_types;

errret:
    VIM_CLEAR(prop_types);
    повернути NULL;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    static ціле *
get_prop_ids_from_list(list_T *l, ціле *num_ids)
{
    ціле		*prop_ids;
    listitem_T	*li;
    ціле		i = 0;
    ціле		id;
    ціле		error;

    *num_ids = 0;

    prop_ids = ALLOC_MULT(ціле, list_len(l));
    якщо (prop_ids == NULL)
	повернути NULL;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	error = FALSE;
	id = tv_get_number_chk(&li->li_tv, &error);
	якщо (error)
	    goto errret;

	prop_ids[i++] = id;
    }

    *num_ids = i;
    повернути prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    повернути NULL;
}

/*
 * prop_list({lnum} [, {bufnr}])
 */
    void
f_prop_list(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    buf_T	*buf = curbuf;
    ціле		add_lnum = FALSE;
    ціле		*prop_types = NULL;
    ціле		prop_types_len = 0;
    ціле		*prop_ids = NULL;
    ціле		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    якщо (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	повернути;

    якщо (rettv_list_alloc(rettv) == FAIL)
	повернути;

    // default: get text properties on current line
    start_lnum = tv_get_number(&argvars[0]);
    end_lnum = start_lnum;
    якщо (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	якщо (check_for_dict_arg(argvars, 1) == FAIL)
	    повернути;
	d = argvars[1].vval.v_dict;

	якщо (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    повернути;

	якщо (d != NULL && (di = dict_find(d, (char_u *)"end_lnum", -1)) != NULL)
	{
	    якщо (di->di_tv.v_type != VAR_NUMBER)
	    {
		emsg(_(e_number_required));
		повернути;
	    }
	    end_lnum = tv_get_number(&di->di_tv);
	    якщо (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		// line
		end_lnum = buf->b_ml.ml_line_count + end_lnum + 1;
	    інакше якщо (end_lnum > buf->b_ml.ml_line_count)
		end_lnum = buf->b_ml.ml_line_count;
	    add_lnum = TRUE;
	}
	якщо (d != NULL && (di = dict_find(d, (char_u *)"types", -1)) != NULL)
	{
	    якщо (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		повернути;
	    }

	    l = di->di_tv.vval.v_list;
	    якщо (l != NULL && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		якщо (prop_types == NULL)
		    повернути;
	    }
	}
	якщо (d != NULL && (di = dict_find(d, (char_u *)"ids", -1)) != NULL)
	{
	    якщо (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		goto errret;
	    }

	    l = di->di_tv.vval.v_list;
	    якщо (l != NULL && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		якщо (prop_ids == NULL)
		    goto errret;
	    }
	}
    }
    якщо (start_lnum < 1 || start_lnum > buf->b_ml.ml_line_count
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    інакше
	для (lnum = start_lnum; lnum <= end_lnum; lnum++)
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
    void
f_prop_remove(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start = 1;
    linenr_T	end = 0;
    linenr_T	lnum;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    dict_T	*dict;
    buf_T	*buf = curbuf;
    ціле		do_all;
    ціле		id = -MAXCOL;
    ціле		type_id = -1;	    // for a single "type"
    ціле		*type_ids = NULL;   // array, for a list of "types", allocated
    ціле		num_type_ids = 0;   // number of elements in "type_ids"
    ціле		both;

    rettv->vval.v_number = 0;

    якщо (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	повернути;

    якщо (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	повернути;

    якщо (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_number(&argvars[1]);
	end = start;
	якщо (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_number(&argvars[2]);
	якщо (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    повернути;
	}
    }

    dict = argvars[0].vval.v_dict;
    якщо (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	повернути;
    якщо (buf->b_ml.ml_mfp == NULL)
	повернути;

    do_all = dict_get_bool(dict, "all", FALSE);

    якщо (dict_has_key(dict, "id"))
	id = dict_get_number(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    якщо (dict_has_key(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	якщо (type == NULL)
	    повернути;
	type_id = type->pt_id;
    }
    якщо (dict_has_key(dict, "types"))
    {
	typval_T types;
	listitem_T *li = NULL;

	dict_get_tv(dict, "types", &types);
	якщо (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( sizeof(ціле) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_ITEMS(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		якщо (li->li_tv.v_type != VAR_STRING)
		    continue;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		якщо (!prop_type)
		    goto cleanup_prop_remove;

		type_ids[num_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    якщо (id == -MAXCOL && (type_id == -1 && num_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	goto cleanup_prop_remove;
    }
    якщо (both && (id == -MAXCOL || (type_id == -1 && num_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	goto cleanup_prop_remove;
    }
    якщо (type_id != -1 && num_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	goto cleanup_prop_remove;
    }

    якщо (end == 0)
	end = buf->b_ml.ml_line_count;

    {
	unpacked_memline_T um = um_open(buf);

	для (lnum = start; lnum <= end; ++lnum)
	{
	    ціле idx;

	    якщо (lnum > buf->b_ml.ml_line_count)
		break;
	    якщо (!um_goto_line(&um, lnum, 0))
		break;

	    для (idx = 0; idx < um.prop_count; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		ціле	    matches_id = 0;
		ціле	    matches_type = 0;

		якщо (prop->tp_flags & TP_FLAG_DELETED)
		    continue;

		matches_id = prop->tp_id == id;
		якщо (num_type_ids > 0)
		{
		    ціле idx2;

		    для (idx2 = 0; !matches_type && idx2 < num_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		інакше
		    matches_type = prop->tp_type == type_id;

		якщо (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    якщо (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_number;
		    якщо (!do_all)
			break;
		}
	    }
	}
	um_close(&um);
    }

    якщо (first_changed > 0)
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
    static void
prop_type_set(typval_T *argvars, ціле add)
{
    char_u	*name;
    buf_T	*buf = NULL;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    якщо (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	повернути;

    name = tv_get_string(&argvars[0]);
    якщо (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	повернути;
    }

    якщо (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	повернути;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    якщо (add)
    {
	hashtab_T **htp;

	якщо (prop != NULL)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    повернути;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	якщо (prop == NULL)
	    повернути;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	якщо (buf == NULL)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	інакше
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	якщо (*htp == NULL)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    якщо (*htp == NULL)
	    {
		vim_free(prop);
		повернути;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    інакше
    {
	якщо (prop == NULL)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    повернути;
	}
    }

    якщо (dict != NULL)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	якщо (di != NULL)
	{
	    char_u	*highlight;
	    ціле		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    якщо (highlight != NULL && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    якщо (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == NULL ? (char_u *)"" : highlight);
		повернути;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	якщо (di != NULL)
	{
	    якщо (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    інакше
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	якщо (di != NULL)
	{
	    якщо (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    інакше
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	якщо (di != NULL)
	    prop->pt_priority = tv_get_number(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	якщо (di != NULL)
	{
	    якщо (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    інакше
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	якщо (di != NULL)
	{
	    якщо (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    інакше
		prop->pt_flags &= ~PT_FLAG_INS_END_INCL;
	}
    }
}

/*
 * prop_type_add({name}, {props})
 */
    void
f_prop_type_add(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, TRUE);
}

/*
 * prop_type_change({name}, {props})
 */
    void
f_prop_type_change(typval_T *argvars, typval_T *rettv UNUSED)
{
    prop_type_set(argvars, FALSE);
}

/*
 * prop_type_delete({name} [, {bufnr}])
 */
    void
f_prop_type_delete(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*name;
    buf_T	*buf = NULL;
    hashitem_T	*hi;

    якщо (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	повернути;

    name = tv_get_string(&argvars[0]);
    якщо (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	повернути;
    }

    якщо (argvars[1].v_type != VAR_UNKNOWN)
    {
	якщо (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    повернути;
    }

    hi = find_prop_type_hi(name, buf);
    якщо (hi == NULL)
	повернути;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    якщо (buf == NULL)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    інакше
    {
	ht = buf->b_proptypes;
	VIM_CLEAR(buf->b_proparray);
    }
    hash_remove(ht, hi, "prop type delete");
    vim_free(prop);

    // currently visible text properties will disappear
    redraw_all_later(UPD_CLEAR);
    changed_window_setting_buf(buf == NULL ? curbuf : buf);
}

/*
 * prop_type_get({name} [, {props}])
 */
    void
f_prop_type_get(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    якщо (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	повернути;

    name = tv_get_string(&argvars[0]);
    якщо (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	повернути;
    }

    якщо (rettv_dict_alloc(rettv) == FAIL)
	повернути;

    proptype_T  *prop = NULL;
    buf_T	    *buf = NULL;

    якщо (argvars[1].v_type != VAR_UNKNOWN)
    {
	якщо (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    повернути;
    }

    prop = find_prop_type(name, buf);
    якщо (prop == NULL)
	повернути;

    dict_T *d = rettv->vval.v_dict;

    якщо (prop->pt_hl_id > 0)
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
    якщо (buf != NULL)
	dict_add_number(d, "bufnr", buf->b_fnum);
}

    static void
list_types(hashtab_T *ht, list_T *l)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	якщо (!HASHITEM_EMPTY(hi))
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
    void
f_prop_type_list(typval_T *argvars, typval_T *rettv)
{
    buf_T *buf = NULL;

    якщо (rettv_list_alloc(rettv) == FAIL)
	повернути;

    якщо (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	повернути;

    якщо (argvars[0].v_type != VAR_UNKNOWN)
    {
	якщо (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    повернути;
    }
    якщо (buf == NULL)
    {
	якщо (global_proptypes != NULL)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    інакше якщо (buf->b_proptypes != NULL)
	list_types(buf->b_proptypes, rettv->vval.v_list);
}

/*
 * Free all property types in "ht".
 */
    static void
clear_ht_prop_types(hashtab_T *ht)
{
    long	todo;
    hashitem_T	*hi;

    якщо (ht == NULL)
	повернути;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	якщо (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#якщо defined(EXITFREE)
/*
 * Free all global property types.
 */
    void
clear_global_prop_types(void)
{
    clear_ht_prop_types(global_proptypes);
    global_proptypes = NULL;
    VIM_CLEAR(global_proparray);
}
#endif

/*
 * Free all property types for "buf".
 */
    void
clear_buf_prop_types(buf_T *buf)
{
    clear_ht_prop_types(buf->b_proptypes);
    buf->b_proptypes = NULL;
    VIM_CLEAR(buf->b_proparray);
}

// Struct used to return two values from adjust_prop().
typedef struct
{
    ціле dirty;	    // if the property was changed
    ціле can_drop;   // whether after this change, the prop may be removed
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
    static adjustres_T
adjust_prop(
	textprop_T  *prop,
	colnr_T	    col,
	ціле	    added,
	ціле	    flags)
{
    proptype_T	*pt;
    ціле		start_incl;
    ціле		end_incl;
    ціле		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    якщо (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	повернути res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    якщо (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    якщо (added > 0)
    {
	якщо (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	інакше якщо (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    інакше якщо (prop->tp_col > col + 1)
    {
	якщо (prop->tp_col + added < col + 1)
	{
	    якщо (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    інакше
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		якщо (prop->tp_len <= 0)
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
	інакше
	    prop->tp_col += added;
    }
    інакше якщо (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	якщо (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	інакше
	{
	    ціле after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	якщо (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    інакше
	res.dirty = FALSE;

    повернути res;
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
    ціле
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	ціле	    bytes_added,
	ціле	    flags)
{
    unpacked_memline_T um;
    ціле		dirty = FALSE;
    ціле		ri;

    якщо (text_prop_frozen > 0)
	повернути FALSE;

    um = um_open_at(curbuf, lnum, 0);
    якщо (um.buf == NULL || um.prop_count == 0)
    {
	um_abort(&um);
	повернути FALSE;
    }

    для (ri = 0; ri < um.prop_count; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	якщо (res.dirty)
	{
	    якщо (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		якщо (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    повернути FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		якщо ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    повернути FALSE;
		}
	    }
	    dirty = TRUE;
	}
	якщо (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    continue;
	}
    }
    якщо (dirty)
	um_close(&um);
    інакше
	um_abort(&um);
    повернути dirty;
}

/*
 * Adjust text properties for a line that was split in two.
 * "lnum_props" is the line that has the properties from before the split.
 * "lnum_top" is the top line.
 * "kept" is the number of bytes kept in the first line, while
 * "deleted" is the number of bytes deleted.
 * "at_eol" is true if the split is after the end of the line.
 */
    void
adjust_props_for_split(
	linenr_T    lnum_props,
	linenr_T    lnum_top,
	ціле	    kept,
	ціле	    deleted,
	ціле	    at_eol)
{
    char_u	*props;
    ціле		count;
    garray_T    prevprop;
    garray_T    nextprop;
    ціле		skipped = kept + deleted;

    якщо (!curbuf->b_has_textprop)
	повернути;

    // Get the text properties from "lnum_props".
    count = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, sizeof(textprop_T), 10);
    ga_init2(&nextprop, sizeof(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *count_ptr = props - PROP_COUNT_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    для (ціле i = 0; i < count; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	ціле	    start_incl, end_incl;
	ціле	    cont_prev, cont_next;
	ціле	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * sizeof(textprop_T), sizeof(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	якщо (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(count_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	інакше
	    prop.u.tp_text = NULL;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	якщо (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	інакше
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    ціле is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    якщо (is_floating_vtext)
		cont_next = FALSE;
	    інакше
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	якщо (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	якщо (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    ціле is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    якщо (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    якщо (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	якщо (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    ціле is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    якщо (p->tp_col != MAXCOL)
	    {
		якщо (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		інакше
		{
		    якщо (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    якщо (cont_prev)
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
    void
prepend_joined_props(
	unpacked_memline_T  *um,
	linenr_T	    lnum,
	ціле		    last_line,
	long		    col,
	ціле		    removed)
{
    unpacked_memline_T	r_um;

    якщо (um->buf == NULL)
	повернути;

    r_um = um_open_at(um->buf, lnum, 0);
    якщо (r_um.buf == NULL)
	повернути;
    якщо (!um_detach(&r_um))
    {
	um_abort(&r_um);
	повернути;
    }

    для (ціле i = r_um.prop_count - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	ціле	    end;

	якщо (prop->tp_col == MAXCOL && !last_line)
	    continue;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	якщо (last_line || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = NULL;  // ownership transferred
	}
	інакше
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    для (ціле j = 0; j < um->prop_count; ++j)
	    {
		textprop_T *op = &um->props[j];

		якщо ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    break;
		}
	    }
	    якщо (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    static void
remember_cleared_prop(garray_T *gap, ціле which, textprop_T *prop, ціле flag)
{
    undoprop_T	*up;

    якщо (which == UNDOPROP_NONE)
	повернути;
    якщо (gap->ga_itemsize == 0)
	ga_init2(gap, sizeof(undoprop_T), 4);
    якщо (ga_grow(gap, 1) == FAIL)
	повернути;
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
    void
adjust_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	ціле	    above_which,
	ціле	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = NULL;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    якщо (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_line_count)
	повернути;

    above = um_open_at(buf, lnum, 0);
    якщо (above.buf == NULL)
	повернути;
    для (ціле i = 0; i < above.prop_count; ++i)
	якщо (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    якщо (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	повернути;
    }

    below = um_open_at(buf, lnum + 1, 0);
    якщо (below.buf == NULL)
    {
	um_abort(&above);
	повернути;
    }
    якщо (below.prop_count > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_count);
	якщо (paired == NULL)
	    goto theend;
    }

    для (ціле i = 0; i < above.prop_count; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	якщо (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    continue;
	для (ціле j = 0; j < below.prop_count; ++j)
	{
	    textprop_T *next = &below.props[j];

	    якщо (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		break;
	    }
	}
	якщо (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    для (ціле j = 0; j < below.prop_count; ++j)
    {
	якщо (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    continue;
	якщо (!below.detached && !um_detach(&below))
	    break;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    якщо (below_changed)
	um_close(&below);
    інакше
	um_abort(&below);
    якщо (above_changed)
	um_close(&above);
    інакше
	um_abort(&above);
    vim_free(paired);
}

/*
 * Give back to line "lnum" the continuation flags "gap" has for "which",
 * those cleared when the undo block was applied the other way.  A property
 * that is no longer there is skipped.
 */
    void
restore_props_for_undo(
	buf_T	    *buf,
	linenr_T    lnum,
	garray_T    *gap,
	ціле	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    якщо (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_line_count)
	повернути;
    um = um_open_at(buf, lnum, 0);
    якщо (um.buf == NULL)
	повернути;
    для (ціле r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	якщо (up->up_which != which)
	    continue;
	для (ціле i = 0; i < um.prop_count; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    якщо (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		continue;
	    якщо (!um.detached && !um_detach(&um))
		goto theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    break;
	}
    }

theend:
    якщо (changed)
	um_close(&um);
    інакше
	um_abort(&um);
}

    bool
text_prop_count_valid(ціле prop_count, size_t propdata_len)
{
    якщо (propdata_len < PROP_COUNT_SIZE)
	повернути false;
    повернути (size_t)prop_count * sizeof(textprop_T)
		    <= propdata_len - PROP_COUNT_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, ціле prop_count, size_t propdata_len)
{
    для (ціле i = 0; i < prop_count; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * sizeof(textprop_T),
							  sizeof(textprop_T));
	якщо (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    continue;
	якщо (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    повернути false;
    }
    повернути true;
}

#endif // FEAT_PROP_POPUP
