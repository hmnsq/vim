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

#nếu defined(FEAT_PROP_POPUP)

static void um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    static void
um_free_vtext(textprop_T *props, số_nguyên count)
{
    cho (số_nguyên i = 0; i < count; ++i)
	nếu (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    static void
um_free(unpacked_memline_T *um)
{
    nếu (um->detached)
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
    trả_về um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_line(unpacked_memline_T *um, linenr_T lnum, số_nguyên extra_props)
{
    char_u	*line;
    size_t	textlen;
    size_t	propdata_len;
    số_nguyên		proplen;

    nếu (um->buf == NULL)
	trả_về false;

    // Store changes to the current line if any.
    nếu (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    nếu (lnum == 0)
	trả_về true;	// just unload

    line = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_line_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = line;

    nếu (propdata_len == 0)
	trả_về true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    nếu (propdata_len < PROP_COUNT_SIZE + sizeof(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	trả_về false;
    }

    uint16_t    prop_count;
    char_u	    *count_ptr = line + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_count, count_ptr, PROP_COUNT_SIZE);
    nếu (!text_prop_count_valid(prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	trả_về false;
    }
    proplen = (số_nguyên)prop_count;
    props_start = count_ptr + PROP_COUNT_SIZE;

    nếu (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	trả_về false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    nếu (um->props == NULL)
    {
	um->buf = NULL;
	trả_về false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_count = proplen;
    mch_memmove(um->props, props_start, proplen * sizeof(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    cho (số_nguyên i = 0; i < proplen; ++i)
    {
	nếu (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = count_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	không_thì
	    um->props[i].u.tp_text = NULL;
    }

    trả_về true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, số_nguyên extra_props)
{
    unpacked_memline_T um = um_open(buf);

    nếu (!um_goto_line(&um, lnum, extra_props))
	um.buf = NULL;
    trả_về um;
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

    nếu (um->detached || um->buf == NULL)
	trả_về um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    nếu (newtext == NULL)
    {
	um->buf = NULL;
	trả_về false;
    }
    um->text = newtext;

    cho (số_nguyên i = 0; i < um->prop_count; ++i)
    {
	nếu (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    nếu (copy == NULL)
	    {
		cho (số_nguyên j = 0; j < i; ++j)
		    nếu (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = NULL;
		trả_về false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    trả_về true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    nếu (um->buf == NULL)
	trả_về false;
    nếu (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    trả_về true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    static bool
um_grow_props(unpacked_memline_T *um, số_nguyên needed)
{
    nếu (um->prop_count + needed <= um->prop_size)
	trả_về true;

    số_nguyên	    new_size = um->prop_count + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * sizeof(textprop_T));
    nếu (newprops == NULL)
	trả_về false;
    um->props = newprops;
    um->prop_size = new_size;

    trả_về true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    static void
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    nếu (um->buf == NULL)
	trả_về;
    nếu (!um->detached && !um_detach(um))
	trả_về;
    nếu (!um_grow_props(um, 1))
    {
	um->buf = NULL;
	trả_về;
    }
    um->props[um->prop_count++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    void
um_reverse_props(unpacked_memline_T *um)
{
    cho (số_nguyên i = 0, j = um->prop_count - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, số_nguyên prop_count)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    nếu (lnum < 1 || lnum > buf->b_ml.ml_line_count)
    {
	um.buf = NULL;
	trả_về um;
    }

    char_u *line = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(line);

    nếu (text_copy == NULL)
    {
	um.buf = NULL;
	trả_về um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    nếu (prop_count > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_count);
	nếu (um.props == NULL)
	{
	    vim_free(um.text);
	    um.buf = NULL;
	    trả_về um;
	}
	um.prop_size = prop_count;
    }

    trả_về um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    static void
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					số_nguyên tp_id, số_nguyên tp_type, số_nguyên flag)
{
    unpacked_memline_T	neighbor;

    nếu (lnum < 1 || lnum > buf->b_ml.ml_line_count)
	trả_về;

    neighbor = um_open_at(buf, lnum, 0);
    nếu (neighbor.buf == NULL)
	trả_về;

    cho (số_nguyên i = 0; i < neighbor.prop_count; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	nếu (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    nếu (!neighbor.detached && !um_detach(&neighbor))
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
um_delete_prop(unpacked_memline_T *um, số_nguyên index)
{
    textprop_T *prop;

    nếu (um->buf == NULL || index < 0 || index >= um->prop_count)
	trả_về;
    nếu (!um->detached && !um_detach(um))
	trả_về;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    nếu (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    nếu (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    nếu (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
um_pack(unpacked_memline_T *um, số_nguyên *packed_len)
{
    uint16_t	live_count = 0;
    số_nguyên		vtext_size = 0;
    số_nguyên		total_len;
    char_u	*buf;
    char_u	*count_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    cho (số_nguyên i = 0; i < um->prop_count; ++i)
    {
	textprop_T *prop = &um->props[i];

	nếu (prop->tp_flags & TP_FLAG_DELETED)
	    continue;
	++live_count;
	nếu (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    nếu (live_count == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	nếu (buf == NULL)
	    trả_về NULL;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	trả_về buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (số_nguyên)PROP_COUNT_SIZE
		+ live_count * (số_nguyên)sizeof(textprop_T) + vtext_size;
    buf = alloc(total_len);
    nếu (buf == NULL)
	trả_về NULL;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    count_dest = buf + um->text_size;
    mch_memmove(count_dest, &live_count, PROP_COUNT_SIZE);

    prop_dest = count_dest + PROP_COUNT_SIZE;
    vtext_dest = prop_dest + live_count * sizeof(textprop_T);

    cho (số_nguyên i = 0; i < um->prop_count; ++i)
    {
	textprop_T prop = um->props[i];

	nếu (prop.tp_flags & TP_FLAG_DELETED)
	    continue;

	nếu (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    số_nguyên copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	không_thì
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, sizeof(textprop_T));
	prop_dest += sizeof(textprop_T);
    }

    trả_về buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    static void
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    số_nguyên	    packed_len;

    nếu (!um->detached || um->buf == NULL || um->lnum == 0)
	trả_về;

    packed = um_pack(um, &packed_len);
    nếu (packed == NULL)
    {
	um->buf = NULL;
	trả_về;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    nếu (um->buf->b_ml.ml_line_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    không_thì nếu (!(um->buf->b_ml.ml_flags & ML_LINE_DIRTY))
	(void)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_count);

    nếu (um->buf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
    nếu (um->buf == NULL)
	trả_về;
    nếu (um->detached && um->lnum > 0)
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
static số_nguyên proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    static hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    nếu (*name == NUL)
	trả_về NULL;
    nếu (buf == NULL)
	ht = global_proptypes;
    không_thì
	ht = buf->b_proptypes;

    nếu (ht == NULL)
	trả_về NULL;
    hi = hash_find(ht, name);
    nếu (HASHITEM_EMPTY(hi))
	trả_về NULL;
    trả_về hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    static proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    nếu (hi == NULL)
	trả_về NULL;
    trả_về HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    số_nguyên
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    nếu (pt == NULL)
	trả_về 0;
    trả_về pt->pt_id;
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

    nếu (type == NULL)
	type = find_prop_type(name, NULL);
    nếu (type == NULL)
	semsg(_(e_property_type_str_does_not_exist), name);
    trả_về type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    static số_nguyên
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    nếu (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	trả_về FAIL;
    }
    nếu (arg->vval.v_dict == NULL)
	trả_về OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    nếu (di != NULL && (di->di_tv.v_type != VAR_NUMBER
					      || di->di_tv.vval.v_number != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	nếu (*buf == NULL)
	    trả_về FAIL;
    }
    trả_về OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    void
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    nếu (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	trả_về;

    start_lnum = tv_get_number(&argvars[0]);
    start_col = tv_get_number(&argvars[1]);
    nếu (check_for_dict_arg(argvars, 2) == FAIL)
	trả_về;

    rettv->vval.v_number = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    static số_nguyên
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	số_nguyên		id,
	char_u		*text_arg,
	số_nguyên		text_padding_left,
	số_nguyên		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    số_nguyên		proplen;
    char_u	*props = NULL;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    số_nguyên		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    số_nguyên		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    nếu (type == NULL)
	goto theend;

    nếu (start_lnum < 1 || start_lnum > buf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), (long)start_lnum);
	goto theend;
    }
    nếu (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), (long)end_lnum);
	goto theend;
    }

    nếu (buf->b_ml.ml_mfp == NULL)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	goto theend;
    }

    nếu (text != NULL)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	cho (p = text; *p != NUL; MB_PTR_ADV(p))
	    nếu (*p < ' ')
		*p = ' ';
    }

    cho (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	nếu (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_line));
	    goto theend;
	}

	nếu (lnum == start_lnum)
	    col = start_col;
	không_thì
	    col = 1;
	nếu (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != NULL))
	{
	    semsg(_(e_invalid_column_number_nr), (long)start_col);
	    goto theend;
	}
	sort_col = col;

	nếu (lnum == end_lnum)
	    length = end_col - col;
	không_thì
	    length = (số_nguyên)textlen - col + 1;
	nếu (length > (long)textlen)
	    length = (số_nguyên)textlen;	// can include the end-of-line
	nếu (length < 0)
	    length = 0;		// zero-width property

	nếu (text_arg != NULL)
	{
	    length = (long)STRLEN(text_arg);
	    nếu (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		nếu ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	nếu (text_arg != NULL)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	cho (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * sizeof(textprop_T),
							   sizeof(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    nếu (prop_col >= sort_col)
		break;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	số_nguyên		vtext_total = 0;
	số_nguyên		new_line_len;
	char_u	*count_dest;
	char_u	*vtext_dest;
	số_nguyên		j;

	// Compute total vtext size from existing props.
	cho (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * sizeof(textprop_T),
							sizeof(textprop_T));
	    nếu (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	nếu (text_arg != NULL)
	    vtext_total += length + 1;

	new_line_len = (số_nguyên)textlen + (số_nguyên)PROP_COUNT_SIZE
			+ new_propcount * (số_nguyên)sizeof(textprop_T)
			+ vtext_total;
	newtext = alloc(new_line_len);
	nếu (newtext == NULL)
	    goto theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_line_ptr, textlen);

	// Write prop_count.
	count_dest = newtext + textlen;
	mch_memmove(count_dest, &new_propcount, PROP_COUNT_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = count_dest + PROP_COUNT_SIZE;
	nếu (i > 0)
	    mch_memmove(newprops, props, sizeof(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	nếu (i < proplen)
	    mch_memmove(newprops + (i + 1) * sizeof(textprop_T),
					props + i * sizeof(textprop_T),
					sizeof(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * sizeof(textprop_T);
	cho (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    nếu (j == i)
		continue;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * sizeof(textprop_T),
		    sizeof(textprop_T));
	    nếu (ep.tp_id < 0 && ep.tp_len > 0)
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
	nếu (text_arg != NULL)
	{
	    số_nguyên copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	không_thì
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * sizeof(textprop_T), &tmp_prop,
							sizeof(textprop_T));

	nếu (buf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
    trả_về res;
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
    số_nguyên		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    số_nguyên		error = FALSE;
    số_nguyên		prev_did_emsg = did_emsg;

    nếu (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	trả_về;

    nếu (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	trả_về;

    dict = argvars[0].vval.v_dict;
    nếu (dict == NULL || !dict_has_key(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	trả_về;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    nếu (dict_has_key(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_number(dict, "id");
	nếu (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    trả_về;
	}
	id = (số_nguyên)x;
    }

    nếu (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	trả_về;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_ITEMS(argvars[1].vval.v_list, li)
    {
	nếu (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == NULL)
	{
	    emsg(_(e_list_required));
	    trả_về;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	nếu (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	nếu (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	nếu (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	số_nguyên this_id = id;
	nếu (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	nếu (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    nếu (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    trả_về;
	}
	nếu (prop_add_one(buf, type_name, this_id, NULL, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    trả_về;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
static số_nguyên vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    static số_nguyên
get_textprop_id(void)
{
    nếu (vt_id_counter > 0)
	vt_id_counter = 0;
    trả_về --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    số_nguyên
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
    số_nguyên		id = 0;
    char_u	*text = NULL;
    số_nguyên		text_padding_left = 0;
    số_nguyên		flags = 0;

    nếu (dict == NULL || !dict_has_key(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	goto theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    nếu (dict_has_key(dict, "end_lnum"))
    {
	end_lnum = dict_get_number(dict, "end_lnum");
	nếu (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_value_for_argument_str), "end_lnum");
	    goto theend;
	}
    }
    không_thì
	end_lnum = start_lnum;

    nếu (dict_has_key(dict, "length"))
    {
	long length = dict_get_number(dict, "length");

	nếu (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_value_for_argument_str), "length");
	    goto theend;
	}
	end_col = start_col + length;
    }
    không_thì nếu (dict_has_key(dict, "end_col"))
    {
	end_col = dict_get_number(dict, "end_col");
	nếu (end_col <= 0)
	{
	    semsg(_(e_invalid_value_for_argument_str), "end_col");
	    goto theend;
	}
    }
    không_thì nếu (start_lnum == end_lnum)
	end_col = start_col;
    không_thì
	end_col = 1;

    nếu (dict_has_key(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_number(dict, "id");
	nếu (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    goto theend;
	}
	id = (số_nguyên)x;
    }

    nếu (dict_has_key(dict, "text"))
    {
	nếu (dict_has_key(dict, "length")
		|| dict_has_key(dict, "end_col")
		|| dict_has_key(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    goto theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	nếu (text == NULL)
	    goto theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	nếu (dict_has_key(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    nếu (p == NULL)
		goto theend;
	    nếu (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		goto theend;
	    }
	    nếu (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_RIGHT;
	    không_thì nếu (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    không_thì nếu (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    không_thì nếu (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "text_align", p);
		goto theend;
	    }
	}

	nếu (dict_has_key(dict, "text_padding_left"))
	{
	    text_padding_left = dict_get_number(dict, "text_padding_left");
	    nếu (text_padding_left < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		goto theend;
	    }
	}

	nếu (dict_has_key(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    nếu (p == NULL)
		goto theend;
	    nếu (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    không_thì nếu (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "text_wrap", p);
		goto theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    nếu (start_col < (text == NULL ? 1 : 0))
    {
	semsg(_(e_invalid_column_number_nr), (long)start_col);
	goto theend;
    }
    nếu (start_col > 0 && text_padding_left > 0)
    {
	emsg(_(e_can_only_use_left_padding_when_column_is_zero));
	goto theend;
    }

    nếu (dict_arg != NULL && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	goto theend;

    nếu (text != NULL)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    không_thì nếu (id < 0)
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
    trả_về id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    số_nguyên
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, số_nguyên will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_count;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    nếu ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == NULL)
	trả_về 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_line_len - textlen;
    nếu (propdata_len == 0)
	trả_về 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    nếu (propdata_len < PROP_COUNT_SIZE + sizeof(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	trả_về 0;
    }
    mch_memmove(&prop_count, text + textlen, PROP_COUNT_SIZE);
    nếu (!text_prop_count_valid(prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	trả_về 0;
    }
    nếu (!text_prop_vtext_valid(text + textlen + PROP_COUNT_SIZE,
					   (số_nguyên)prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	trả_về 0;
    }
    *props = text + textlen + PROP_COUNT_SIZE;
    trả_về (số_nguyên)prop_count;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    số_nguyên
prop_count_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    số_nguyên		count = get_text_props(buf, lnum, &props, FALSE);
    số_nguyên		result = 0;
    textprop_T	prop;
    số_nguyên		i;
    số_nguyên		next_right_goes_below = FALSE;

    nếu (count == 0)
	trả_về 0;
    cho (i = 0; i < count; ++i)
    {
	mch_memmove(&prop, props + i * sizeof(prop), sizeof(prop));
	nếu (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    nếu ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_right_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_RIGHT)))
	    {
		++result;
	    }
	    không_thì nếu (prop.tp_flags & TP_FLAG_ALIGN_RIGHT)
		next_right_goes_below = TRUE;
	}
    }
    trả_về result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    số_nguyên
count_props(linenr_T lnum, số_nguyên only_starting, số_nguyên last_line)
{
    char_u	*props;
    số_nguyên		proplen = get_text_props(curbuf, lnum, &props, 0);
    số_nguyên		result = proplen;
    textprop_T	prop;

    cho (số_nguyên i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * sizeof(prop), sizeof(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	nếu ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_line && prop.tp_col == MAXCOL))
	    --result;
    }
    trả_về result;
}

static textprop_T	*text_prop_compare_props;
static buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    static số_nguyên
text_prop_order(số_nguyên flags)
{
    nếu (flags & TP_FLAG_ALIGN_ABOVE)
	trả_về 0;
    nếu (flags & TP_FLAG_ALIGN_RIGHT)
	trả_về 2;
    nếu (flags & TP_FLAG_ALIGN_BELOW)
	trả_về 3;
    trả_về 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    static số_nguyên
text_prop_compare(const void *s1, const void *s2)
{
    số_nguyên  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(số_nguyên *)s1;
    idx2 = *(số_nguyên *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    nếu ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	trả_về tp1->tp_id < 0 ? 1 : -1;

    nếu (col1 == MAXCOL || col2 == MAXCOL)
    {
	số_nguyên order1 = text_prop_order(tp1->tp_flags);
	số_nguyên order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	nếu (order1 != order2)
	    trả_về order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    nếu (pt1 != pt2)
    {
	nếu (pt1 == NULL)
	    trả_về -1;
	nếu (pt2 == NULL)
	    trả_về 1;
	nếu (pt1->pt_priority != pt2->pt_priority)
	    trả_về pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    nếu (col1 != col2)
	trả_về col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    nếu (tp1->tp_id < 0)
	trả_về tp1->tp_id > tp2->tp_id ? 1 : -1;

    trả_về 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    void
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	số_nguyên	    *idxs,
	số_nguyên	    count)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((void *)idxs, (size_t)count, sizeof(số_nguyên), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_lines(
	win_T	    *wp,
	số_nguyên	    type_id,
	số_nguyên	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    nếu (text_prop_type_by_id(wp->w_buffer, type_id) == NULL)
	trả_về false;

    nếu (first_lnum < 1)
	first_lnum = 1;
    nếu (last_lnum > wp->w_buffer->b_ml.ml_line_count)
	last_lnum = wp->w_buffer->b_ml.ml_line_count;

    cho (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	số_nguyên	count = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	cho (số_nguyên i = 0; i < count; ++i)
	{
	    mch_memmove(prop, props + i * sizeof(textprop_T),
							   sizeof(textprop_T));
	    nếu (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		trả_về true;
	    }
	}
    }
    trả_về false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	số_nguyên	    type_id,
	số_nguyên	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    trả_về find_prop_in_lines(wp, type_id, id, prop, found_lnum,
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
set_text_props(linenr_T lnum, textprop_T *tps, số_nguyên count)
{
    char_u  *text;
    char_u  *newtext;
    số_nguyên	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    nếu (count == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	nếu (newtext == NULL)
	    trả_về;
	mch_memmove(newtext, text, textlen);
	nếu (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_line_ptr);
	curbuf->b_ml.ml_line_ptr = newtext;
	curbuf->b_ml.ml_line_len = textlen;
	curbuf->b_ml.ml_flags |= ML_LINE_DIRTY;
    }
    không_thì
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_count = (uint16_t)count;
	số_nguyên	    vtext_size = 0;
	số_nguyên	    total_len;
	char_u	    *count_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	số_nguyên	    i;

	cho (i = 0; i < count; ++i)
	    nếu (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (số_nguyên)PROP_COUNT_SIZE
		    + count * (số_nguyên)sizeof(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	nếu (newtext == NULL)
	    trả_về;
	mch_memmove(newtext, text, textlen);

	count_dest = newtext + textlen;
	mch_memmove(count_dest, &prop_count, PROP_COUNT_SIZE);

	prop_dest = count_dest + PROP_COUNT_SIZE;
	vtext_dest = prop_dest + count * sizeof(textprop_T);

	cho (i = 0; i < count; ++i)
	{
	    textprop_T prop = tps[i];

	    nếu (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		số_nguyên copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = NULL;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
		vtext_dest += copysize;
	    }
	    không_thì
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, sizeof(textprop_T));
	    prop_dest += sizeof(textprop_T);
	}

	nếu (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
props_add_count_header(char_u *line, số_nguyên line_len, số_nguyên textlen, số_nguyên *new_len)
{
    số_nguyên		prop_bytes = line_len - textlen;
    uint16_t	prop_count;
    số_nguyên		vtext_size = 0;
    số_nguyên		total;
    char_u	*newline;
    char_u	*count_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    số_nguyên		i;

    nếu (prop_bytes <= 0 || prop_bytes % sizeof(textprop_T) != 0)
    {
	*new_len = line_len;
	trả_về NULL;
    }
    prop_count = (uint16_t)(prop_bytes / sizeof(textprop_T));

    // Calculate total vtext size.
    cho (i = 0; i < (số_nguyên)prop_count; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, line + textlen + i * sizeof(textprop_T),
							sizeof(textprop_T));
	nếu (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (số_nguyên)PROP_COUNT_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    nếu (newline == NULL)
	trả_về NULL;
    *new_len = total;

    mch_memmove(newline, line, textlen);
    count_dest = newline + textlen;
    mch_memmove(count_dest, &prop_count, PROP_COUNT_SIZE);
    prop_dest = count_dest + PROP_COUNT_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    cho (i = 0; i < (số_nguyên)prop_count; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, line + textlen + i * sizeof(textprop_T),
							sizeof(textprop_T));
	nếu (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    số_nguyên copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	không_thì
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * sizeof(textprop_T), &prop,
							sizeof(textprop_T));
    }
    trả_về newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    void
add_text_props(linenr_T lnum, textprop_T *text_props, số_nguyên text_prop_count)
{
    unpacked_memline_T	um;
    số_nguyên			i;

    um = um_open_at(curbuf, lnum, text_prop_count);
    nếu (um.buf == NULL)
	trả_về;
    nếu (!um_detach(&um))
    {
	um_abort(&um);
	trả_về;
    }

    // Grow props array if needed.
    nếu (um.prop_count + text_prop_count > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_count + text_prop_count) * sizeof(textprop_T));
	nếu (newprops == NULL)
	{
	    um_abort(&um);
	    trả_về;
	}
	um.props = newprops;
	um.prop_size = um.prop_count + text_prop_count;
    }

    cho (i = 0; i < text_prop_count; ++i)
	um.props[um.prop_count++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    static số_nguyên
compare_pt(const void *s1, const void *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    trả_về tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    static proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, số_nguyên id)
{
    số_nguyên low = 0;
    số_nguyên high;

    nếu (ht == NULL || ht->ht_used == 0)
	trả_về NULL;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    nếu (*array == NULL)
    {
	long	    todo;
	hashitem_T  *hi;
	số_nguyên	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	nếu (*array == NULL)
	    trả_về NULL;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
	{
	    nếu (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((void *)*array, ht->ht_used, sizeof(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    high = ht->ht_used;
    trong_khi (high > low)
    {
	số_nguyên m = (high + low) / 2;

	nếu ((*array)[m]->pt_id == id)
	    trả_về (*array)[m];
	nếu ((*array)[m]->pt_id > id)
	    high = m;
	không_thì
	    low = m + 1;
    }
    trả_về NULL;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    static void
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    số_nguyên buflocal = TRUE;
    số_nguyên virtualtext_prop = prop->tp_id < 0;

    dict_add_number(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    nếu (!virtualtext_prop)
    {
	dict_add_number(dict, "length", prop->tp_len);
	dict_add_number(dict, "id", prop->tp_id);
    }
    dict_add_number(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_number(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    nếu (pt == NULL)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    nếu (pt != NULL)
	dict_add_string(dict, "type", pt->pt_name);

    nếu (buflocal)
	dict_add_number(dict, "type_bufnr", buf->b_fnum);
    không_thì
	dict_add_number(dict, "type_bufnr", 0);
    nếu (virtualtext_prop)
    {
	string_T    text_align = {NULL, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	nếu (prop->tp_flags & TP_FLAG_ALIGN_RIGHT)
	    STR_LITERAL_SET(text_align, "right");
	không_thì nếu (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	không_thì nếu (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	nếu (text_align.string != NULL)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (số_nguyên)text_align.length);

	// text_wrap
	nếu (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	nếu (prop->tp_padleft != 0)
	    dict_add_number(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, số_nguyên id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    nếu (type == NULL)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    trả_về type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    trả_về text_prop_type_by_id(buf, prop->tp_type) != NULL;
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
    số_nguyên	    did_clear = FALSE;

    nếu (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	trả_về;

    start = tv_get_number(&argvars[0]);
    end = start;
    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_number(&argvars[1]);
	nếu (argvars[2].v_type != VAR_UNKNOWN)
	{
	    nếu (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		trả_về;
	}
    }
    nếu (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	trả_về;
    }

    {
	unpacked_memline_T um = um_open(buf);

	cho (lnum = start; lnum <= end; ++lnum)
	{
	    số_nguyên idx;

	    nếu (lnum > buf->b_ml.ml_line_count)
		break;
	    nếu (!um_goto_line(&um, lnum, 0))
		break;
	    nếu (um.prop_count > 0)
	    {
		did_clear = TRUE;
		cho (idx = um.prop_count - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    nếu (did_clear)
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
    số_nguyên		lnum_start;
    số_nguyên		start_pos_has_prop = 0;
    số_nguyên		seen_end = FALSE;
    số_nguyên		id = 0;
    số_nguyên		id_found = FALSE;
    số_nguyên		type_id = -1;
    số_nguyên		skipstart = FALSE;
    số_nguyên		lnum = -1;
    số_nguyên		col = -1;
    số_nguyên		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    số_nguyên		both;

    nếu (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	trả_về;

    nếu (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	trả_về;
    dict = argvars[0].vval.v_dict;

    nếu (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	trả_về;
    nếu (buf->b_ml.ml_mfp == NULL)
	trả_về;

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	nếu (*dir_s == 'b')
	    dir = BACKWARD;
	không_thì nếu (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    trả_về;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    nếu (di != NULL)
	lnum = tv_get_number(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    nếu (di != NULL)
	col = tv_get_number(&di->di_tv);

    nếu (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    không_thì nếu (col == -1)
	col = 1;

    nếu (lnum < 1 || lnum > buf->b_ml.ml_line_count)
    {
	emsg(_(e_invalid_range));
	trả_về;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    nếu (dict_has_key(dict, "id"))
    {
	id = dict_get_number(dict, "id");
	id_found = TRUE;
    }
    nếu (dict_has_key(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	nếu (type == NULL)
	    trả_về;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    nếu (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	trả_về;
    }
    nếu (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	trả_về;
    }

    lnum_start = lnum;

    nếu (rettv_dict_alloc(rettv) == FAIL)
	trả_về;

    trong_khi (1)
    {
	char_u	*prop_start_ptr;
	số_nguyên	count = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	số_nguyên	    i;
	textprop_T  prop;
	số_nguyên	    prop_start;
	số_nguyên	    prop_end;

	cho (i = dir == BACKWARD ? count - 1 : 0; i >= 0 && i < count; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * sizeof(textprop_T),
							   sizeof(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    nếu (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_COUNT_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    không_thì
		prop.u.tp_text = NULL;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    nếu (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		nếu (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    nếu (!is_floating_vtext && prop.tp_col > col)
			continue;
		}
		không_thì nếu (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    continue;
	    }
	    nếu (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		nếu (lnum_start == lnum
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
		nếu (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		nếu (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    nếu (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    continue;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		nếu (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    continue;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_number(rettv->vval.v_dict, "lnum", lnum);

		trả_về;
	    }
	}

	nếu (dir > 0)
	{
	    nếu (lnum >= buf->b_ml.ml_line_count)
		break;
	    lnum++;
	}
	không_thì
	{
	    nếu (lnum <= 1)
		break;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    static số_nguyên
prop_type_or_id_in_list(số_nguyên *types_or_ids, số_nguyên len, số_nguyên type_or_id)
{
    số_nguyên i;

    cho (i = 0; i < len; i++)
	nếu (types_or_ids[i] == type_or_id)
	    trả_về TRUE;

    trả_về FALSE;
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
	số_nguyên		*prop_types,
	số_nguyên		prop_types_len,
	số_nguyên		*prop_ids,
	số_nguyên		prop_ids_len,
	list_T		*retlist,
	số_nguyên		add_lnum)
{
    char_u	*props;
    số_nguyên		count;
    số_nguyên		i;
    textprop_T	prop;

    count = get_text_props(buf, lnum, &props, FALSE);
    cho (i = 0; i < count; ++i)
    {
	mch_memmove(&prop, props + i * sizeof(textprop_T),
		sizeof(textprop_T));
	// Convert offset to pointer for virtual text props.
	nếu (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_COUNT_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	không_thì
	    prop.u.tp_text = NULL;
	nếu ((prop_types == NULL
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == NULL
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    nếu (d == NULL)
		break;
	    prop_fill_dict(d, &prop, buf);
	    nếu (add_lnum)
		dict_add_number(d, "lnum", lnum);
	    nếu (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    static số_nguyên *
get_prop_types_from_names(list_T *l, buf_T *buf, số_nguyên *num_types)
{
    số_nguyên		*prop_types;
    listitem_T	*li;
    số_nguyên		i;
    char_u	*name;
    proptype_T	*type;

    *num_types = 0;

    prop_types = ALLOC_MULT(số_nguyên, list_len(l));
    nếu (prop_types == NULL)
	trả_về NULL;

    i = 0;
    FOR_ALL_LIST_ITEMS(l, li)
    {
	nếu (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    goto errret;
	}
	name = li->li_tv.vval.v_string;
	nếu (name == NULL)
	    goto errret;

	type = lookup_prop_type(name, buf);
	nếu (type == NULL)
	    goto errret;
	prop_types[i++] = type->pt_id;
    }

    *num_types = i;
    trả_về prop_types;

errret:
    VIM_CLEAR(prop_types);
    trả_về NULL;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    static số_nguyên *
get_prop_ids_from_list(list_T *l, số_nguyên *num_ids)
{
    số_nguyên		*prop_ids;
    listitem_T	*li;
    số_nguyên		i = 0;
    số_nguyên		id;
    số_nguyên		error;

    *num_ids = 0;

    prop_ids = ALLOC_MULT(số_nguyên, list_len(l));
    nếu (prop_ids == NULL)
	trả_về NULL;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	error = FALSE;
	id = tv_get_number_chk(&li->li_tv, &error);
	nếu (error)
	    goto errret;

	prop_ids[i++] = id;
    }

    *num_ids = i;
    trả_về prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    trả_về NULL;
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
    số_nguyên		add_lnum = FALSE;
    số_nguyên		*prop_types = NULL;
    số_nguyên		prop_types_len = 0;
    số_nguyên		*prop_ids = NULL;
    số_nguyên		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    nếu (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	trả_về;

    nếu (rettv_list_alloc(rettv) == FAIL)
	trả_về;

    // default: get text properties on current line
    start_lnum = tv_get_number(&argvars[0]);
    end_lnum = start_lnum;
    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	nếu (check_for_dict_arg(argvars, 1) == FAIL)
	    trả_về;
	d = argvars[1].vval.v_dict;

	nếu (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    trả_về;

	nếu (d != NULL && (di = dict_find(d, (char_u *)"end_lnum", -1)) != NULL)
	{
	    nếu (di->di_tv.v_type != VAR_NUMBER)
	    {
		emsg(_(e_number_required));
		trả_về;
	    }
	    end_lnum = tv_get_number(&di->di_tv);
	    nếu (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		// line
		end_lnum = buf->b_ml.ml_line_count + end_lnum + 1;
	    không_thì nếu (end_lnum > buf->b_ml.ml_line_count)
		end_lnum = buf->b_ml.ml_line_count;
	    add_lnum = TRUE;
	}
	nếu (d != NULL && (di = dict_find(d, (char_u *)"types", -1)) != NULL)
	{
	    nếu (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		trả_về;
	    }

	    l = di->di_tv.vval.v_list;
	    nếu (l != NULL && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		nếu (prop_types == NULL)
		    trả_về;
	    }
	}
	nếu (d != NULL && (di = dict_find(d, (char_u *)"ids", -1)) != NULL)
	{
	    nếu (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		goto errret;
	    }

	    l = di->di_tv.vval.v_list;
	    nếu (l != NULL && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		nếu (prop_ids == NULL)
		    goto errret;
	    }
	}
    }
    nếu (start_lnum < 1 || start_lnum > buf->b_ml.ml_line_count
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    không_thì
	cho (lnum = start_lnum; lnum <= end_lnum; lnum++)
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
    số_nguyên		do_all;
    số_nguyên		id = -MAXCOL;
    số_nguyên		type_id = -1;	    // for a single "type"
    số_nguyên		*type_ids = NULL;   // array, for a list of "types", allocated
    số_nguyên		num_type_ids = 0;   // number of elements in "type_ids"
    số_nguyên		both;

    rettv->vval.v_number = 0;

    nếu (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	trả_về;

    nếu (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	trả_về;

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_number(&argvars[1]);
	end = start;
	nếu (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_number(&argvars[2]);
	nếu (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    trả_về;
	}
    }

    dict = argvars[0].vval.v_dict;
    nếu (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	trả_về;
    nếu (buf->b_ml.ml_mfp == NULL)
	trả_về;

    do_all = dict_get_bool(dict, "all", FALSE);

    nếu (dict_has_key(dict, "id"))
	id = dict_get_number(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    nếu (dict_has_key(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	nếu (type == NULL)
	    trả_về;
	type_id = type->pt_id;
    }
    nếu (dict_has_key(dict, "types"))
    {
	typval_T types;
	listitem_T *li = NULL;

	dict_get_tv(dict, "types", &types);
	nếu (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( sizeof(số_nguyên) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_ITEMS(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		nếu (li->li_tv.v_type != VAR_STRING)
		    continue;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		nếu (!prop_type)
		    goto cleanup_prop_remove;

		type_ids[num_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    nếu (id == -MAXCOL && (type_id == -1 && num_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	goto cleanup_prop_remove;
    }
    nếu (both && (id == -MAXCOL || (type_id == -1 && num_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	goto cleanup_prop_remove;
    }
    nếu (type_id != -1 && num_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	goto cleanup_prop_remove;
    }

    nếu (end == 0)
	end = buf->b_ml.ml_line_count;

    {
	unpacked_memline_T um = um_open(buf);

	cho (lnum = start; lnum <= end; ++lnum)
	{
	    số_nguyên idx;

	    nếu (lnum > buf->b_ml.ml_line_count)
		break;
	    nếu (!um_goto_line(&um, lnum, 0))
		break;

	    cho (idx = 0; idx < um.prop_count; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		số_nguyên	    matches_id = 0;
		số_nguyên	    matches_type = 0;

		nếu (prop->tp_flags & TP_FLAG_DELETED)
		    continue;

		matches_id = prop->tp_id == id;
		nếu (num_type_ids > 0)
		{
		    số_nguyên idx2;

		    cho (idx2 = 0; !matches_type && idx2 < num_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		không_thì
		    matches_type = prop->tp_type == type_id;

		nếu (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    nếu (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_number;
		    nếu (!do_all)
			break;
		}
	    }
	}
	um_close(&um);
    }

    nếu (first_changed > 0)
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
prop_type_set(typval_T *argvars, số_nguyên add)
{
    char_u	*name;
    buf_T	*buf = NULL;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	trả_về;

    name = tv_get_string(&argvars[0]);
    nếu (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	trả_về;
    }

    nếu (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	trả_về;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    nếu (add)
    {
	hashtab_T **htp;

	nếu (prop != NULL)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    trả_về;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	nếu (prop == NULL)
	    trả_về;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	nếu (buf == NULL)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	không_thì
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	nếu (*htp == NULL)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    nếu (*htp == NULL)
	    {
		vim_free(prop);
		trả_về;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    không_thì
    {
	nếu (prop == NULL)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    trả_về;
	}
    }

    nếu (dict != NULL)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	nếu (di != NULL)
	{
	    char_u	*highlight;
	    số_nguyên		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    nếu (highlight != NULL && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    nếu (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == NULL ? (char_u *)"" : highlight);
		trả_về;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	nếu (di != NULL)
	{
	    nếu (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    không_thì
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	nếu (di != NULL)
	{
	    nếu (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    không_thì
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	nếu (di != NULL)
	    prop->pt_priority = tv_get_number(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	nếu (di != NULL)
	{
	    nếu (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    không_thì
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	nếu (di != NULL)
	{
	    nếu (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    không_thì
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

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	trả_về;

    name = tv_get_string(&argvars[0]);
    nếu (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	trả_về;
    }

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	nếu (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    trả_về;
    }

    hi = find_prop_type_hi(name, buf);
    nếu (hi == NULL)
	trả_về;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    nếu (buf == NULL)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    không_thì
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

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	trả_về;

    name = tv_get_string(&argvars[0]);
    nếu (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	trả_về;
    }

    nếu (rettv_dict_alloc(rettv) == FAIL)
	trả_về;

    proptype_T  *prop = NULL;
    buf_T	    *buf = NULL;

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	nếu (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    trả_về;
    }

    prop = find_prop_type(name, buf);
    nếu (prop == NULL)
	trả_về;

    dict_T *d = rettv->vval.v_dict;

    nếu (prop->pt_hl_id > 0)
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
    nếu (buf != NULL)
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
	nếu (!HASHITEM_EMPTY(hi))
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

    nếu (rettv_list_alloc(rettv) == FAIL)
	trả_về;

    nếu (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	trả_về;

    nếu (argvars[0].v_type != VAR_UNKNOWN)
    {
	nếu (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    trả_về;
    }
    nếu (buf == NULL)
    {
	nếu (global_proptypes != NULL)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    không_thì nếu (buf->b_proptypes != NULL)
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

    nếu (ht == NULL)
	trả_về;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	nếu (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#nếu defined(EXITFREE)
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
    số_nguyên dirty;	    // if the property was changed
    số_nguyên can_drop;   // whether after this change, the prop may be removed
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
	số_nguyên	    added,
	số_nguyên	    flags)
{
    proptype_T	*pt;
    số_nguyên		start_incl;
    số_nguyên		end_incl;
    số_nguyên		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    nếu (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	trả_về res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    nếu (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    nếu (added > 0)
    {
	nếu (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	không_thì nếu (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    không_thì nếu (prop->tp_col > col + 1)
    {
	nếu (prop->tp_col + added < col + 1)
	{
	    nếu (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    không_thì
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		nếu (prop->tp_len <= 0)
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
	không_thì
	    prop->tp_col += added;
    }
    không_thì nếu (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	nếu (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	không_thì
	{
	    số_nguyên after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	nếu (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    không_thì
	res.dirty = FALSE;

    trả_về res;
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
    số_nguyên
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	số_nguyên	    bytes_added,
	số_nguyên	    flags)
{
    unpacked_memline_T um;
    số_nguyên		dirty = FALSE;
    số_nguyên		ri;

    nếu (text_prop_frozen > 0)
	trả_về FALSE;

    um = um_open_at(curbuf, lnum, 0);
    nếu (um.buf == NULL || um.prop_count == 0)
    {
	um_abort(&um);
	trả_về FALSE;
    }

    cho (ri = 0; ri < um.prop_count; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	nếu (res.dirty)
	{
	    nếu (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		nếu (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    trả_về FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		nếu ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    trả_về FALSE;
		}
	    }
	    dirty = TRUE;
	}
	nếu (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    continue;
	}
    }
    nếu (dirty)
	um_close(&um);
    không_thì
	um_abort(&um);
    trả_về dirty;
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
	số_nguyên	    kept,
	số_nguyên	    deleted,
	số_nguyên	    at_eol)
{
    char_u	*props;
    số_nguyên		count;
    garray_T    prevprop;
    garray_T    nextprop;
    số_nguyên		skipped = kept + deleted;

    nếu (!curbuf->b_has_textprop)
	trả_về;

    // Get the text properties from "lnum_props".
    count = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, sizeof(textprop_T), 10);
    ga_init2(&nextprop, sizeof(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *count_ptr = props - PROP_COUNT_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    cho (số_nguyên i = 0; i < count; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	số_nguyên	    start_incl, end_incl;
	số_nguyên	    cont_prev, cont_next;
	số_nguyên	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * sizeof(textprop_T), sizeof(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	nếu (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(count_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	không_thì
	    prop.u.tp_text = NULL;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	nếu (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	không_thì
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    số_nguyên is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    nếu (is_floating_vtext)
		cont_next = FALSE;
	    không_thì
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	nếu (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	nếu (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    số_nguyên is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    nếu (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    nếu (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	nếu (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    số_nguyên is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    nếu (p->tp_col != MAXCOL)
	    {
		nếu (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		không_thì
		{
		    nếu (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    nếu (cont_prev)
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
	số_nguyên		    last_line,
	long		    col,
	số_nguyên		    removed)
{
    unpacked_memline_T	r_um;

    nếu (um->buf == NULL)
	trả_về;

    r_um = um_open_at(um->buf, lnum, 0);
    nếu (r_um.buf == NULL)
	trả_về;
    nếu (!um_detach(&r_um))
    {
	um_abort(&r_um);
	trả_về;
    }

    cho (số_nguyên i = r_um.prop_count - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	số_nguyên	    end;

	nếu (prop->tp_col == MAXCOL && !last_line)
	    continue;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	nếu (last_line || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = NULL;  // ownership transferred
	}
	không_thì
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    cho (số_nguyên j = 0; j < um->prop_count; ++j)
	    {
		textprop_T *op = &um->props[j];

		nếu ((op->tp_flags & TP_FLAG_CONT_PREV)
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
	    nếu (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    static void
remember_cleared_prop(garray_T *gap, số_nguyên which, textprop_T *prop, số_nguyên flag)
{
    undoprop_T	*up;

    nếu (which == UNDOPROP_NONE)
	trả_về;
    nếu (gap->ga_itemsize == 0)
	ga_init2(gap, sizeof(undoprop_T), 4);
    nếu (ga_grow(gap, 1) == FAIL)
	trả_về;
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
	số_nguyên	    above_which,
	số_nguyên	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = NULL;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    nếu (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_line_count)
	trả_về;

    above = um_open_at(buf, lnum, 0);
    nếu (above.buf == NULL)
	trả_về;
    cho (số_nguyên i = 0; i < above.prop_count; ++i)
	nếu (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    nếu (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	trả_về;
    }

    below = um_open_at(buf, lnum + 1, 0);
    nếu (below.buf == NULL)
    {
	um_abort(&above);
	trả_về;
    }
    nếu (below.prop_count > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_count);
	nếu (paired == NULL)
	    goto theend;
    }

    cho (số_nguyên i = 0; i < above.prop_count; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	nếu (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    continue;
	cho (số_nguyên j = 0; j < below.prop_count; ++j)
	{
	    textprop_T *next = &below.props[j];

	    nếu (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		break;
	    }
	}
	nếu (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    cho (số_nguyên j = 0; j < below.prop_count; ++j)
    {
	nếu (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    continue;
	nếu (!below.detached && !um_detach(&below))
	    break;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    nếu (below_changed)
	um_close(&below);
    không_thì
	um_abort(&below);
    nếu (above_changed)
	um_close(&above);
    không_thì
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
	số_nguyên	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    nếu (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_line_count)
	trả_về;
    um = um_open_at(buf, lnum, 0);
    nếu (um.buf == NULL)
	trả_về;
    cho (số_nguyên r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	nếu (up->up_which != which)
	    continue;
	cho (số_nguyên i = 0; i < um.prop_count; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    nếu (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		continue;
	    nếu (!um.detached && !um_detach(&um))
		goto theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    break;
	}
    }

theend:
    nếu (changed)
	um_close(&um);
    không_thì
	um_abort(&um);
}

    bool
text_prop_count_valid(số_nguyên prop_count, size_t propdata_len)
{
    nếu (propdata_len < PROP_COUNT_SIZE)
	trả_về false;
    trả_về (size_t)prop_count * sizeof(textprop_T)
		    <= propdata_len - PROP_COUNT_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, số_nguyên prop_count, size_t propdata_len)
{
    cho (số_nguyên i = 0; i < prop_count; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * sizeof(textprop_T),
							  sizeof(textprop_T));
	nếu (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    continue;
	nếu (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    trả_về false;
    }
    trả_về true;
}

#endif // FEAT_PROP_POPUP
