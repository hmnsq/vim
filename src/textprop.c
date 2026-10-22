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

#ถ้า defined(FEAT_PROP_POPUP)

static void um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    static void
um_free_vtext(textprop_T *props, จำนวนเต็ม count)
{
    สำหรับ (จำนวนเต็ม i = 0; i < count; ++i)
	ถ้า (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    static void
um_free(unpacked_memline_T *um)
{
    ถ้า (um->detached)
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
    คืน um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_line(unpacked_memline_T *um, linenr_T lnum, จำนวนเต็ม extra_props)
{
    char_u	*line;
    size_t	textlen;
    size_t	propdata_len;
    จำนวนเต็ม		proplen;

    ถ้า (um->buf == NULL)
	คืน false;

    // Store changes to the current line if any.
    ถ้า (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    ถ้า (lnum == 0)
	คืน true;	// just unload

    line = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_line_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = line;

    ถ้า (propdata_len == 0)
	คืน true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    ถ้า (propdata_len < PROP_COUNT_SIZE + parimaa_na(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	คืน false;
    }

    uint16_t    prop_count;
    char_u	    *count_ptr = line + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_count, count_ptr, PROP_COUNT_SIZE);
    ถ้า (!text_prop_count_valid(prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	คืน false;
    }
    proplen = (จำนวนเต็ม)prop_count;
    props_start = count_ptr + PROP_COUNT_SIZE;

    ถ้า (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	คืน false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    ถ้า (um->props == NULL)
    {
	um->buf = NULL;
	คืน false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_count = proplen;
    mch_memmove(um->props, props_start, proplen * parimaa_na(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    สำหรับ (จำนวนเต็ม i = 0; i < proplen; ++i)
    {
	ถ้า (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = count_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	มิฉะนั้น
	    um->props[i].u.tp_text = NULL;
    }

    คืน true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, จำนวนเต็ม extra_props)
{
    unpacked_memline_T um = um_open(buf);

    ถ้า (!um_goto_line(&um, lnum, extra_props))
	um.buf = NULL;
    คืน um;
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

    ถ้า (um->detached || um->buf == NULL)
	คืน um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    ถ้า (newtext == NULL)
    {
	um->buf = NULL;
	คืน false;
    }
    um->text = newtext;

    สำหรับ (จำนวนเต็ม i = 0; i < um->prop_count; ++i)
    {
	ถ้า (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    ถ้า (copy == NULL)
	    {
		สำหรับ (จำนวนเต็ม j = 0; j < i; ++j)
		    ถ้า (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = NULL;
		คืน false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    คืน true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    ถ้า (um->buf == NULL)
	คืน false;
    ถ้า (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    คืน true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    static bool
um_grow_props(unpacked_memline_T *um, จำนวนเต็ม needed)
{
    ถ้า (um->prop_count + needed <= um->prop_size)
	คืน true;

    จำนวนเต็ม	    new_size = um->prop_count + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * parimaa_na(textprop_T));
    ถ้า (newprops == NULL)
	คืน false;
    um->props = newprops;
    um->prop_size = new_size;

    คืน true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    static void
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    ถ้า (um->buf == NULL)
	คืน;
    ถ้า (!um->detached && !um_detach(um))
	คืน;
    ถ้า (!um_grow_props(um, 1))
    {
	um->buf = NULL;
	คืน;
    }
    um->props[um->prop_count++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    void
um_reverse_props(unpacked_memline_T *um)
{
    สำหรับ (จำนวนเต็ม i = 0, j = um->prop_count - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, จำนวนเต็ม prop_count)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    ถ้า (lnum < 1 || lnum > buf->b_ml.ml_line_count)
    {
	um.buf = NULL;
	คืน um;
    }

    char_u *line = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(line);

    ถ้า (text_copy == NULL)
    {
	um.buf = NULL;
	คืน um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    ถ้า (prop_count > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_count);
	ถ้า (um.props == NULL)
	{
	    vim_free(um.text);
	    um.buf = NULL;
	    คืน um;
	}
	um.prop_size = prop_count;
    }

    คืน um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    static void
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					จำนวนเต็ม tp_id, จำนวนเต็ม tp_type, จำนวนเต็ม flag)
{
    unpacked_memline_T	neighbor;

    ถ้า (lnum < 1 || lnum > buf->b_ml.ml_line_count)
	คืน;

    neighbor = um_open_at(buf, lnum, 0);
    ถ้า (neighbor.buf == NULL)
	คืน;

    สำหรับ (จำนวนเต็ม i = 0; i < neighbor.prop_count; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	ถ้า (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    ถ้า (!neighbor.detached && !um_detach(&neighbor))
		bha_nga;
	    // Re-get pointer after detach.
	    p = &neighbor.props[i];
	    p->tp_flags &= ~flag;
	    bha_nga;
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
um_delete_prop(unpacked_memline_T *um, จำนวนเต็ม index)
{
    textprop_T *prop;

    ถ้า (um->buf == NULL || index < 0 || index >= um->prop_count)
	คืน;
    ถ้า (!um->detached && !um_detach(um))
	คืน;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    ถ้า (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    ถ้า (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    ถ้า (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
um_pack(unpacked_memline_T *um, จำนวนเต็ม *packed_len)
{
    uint16_t	live_count = 0;
    จำนวนเต็ม		vtext_size = 0;
    จำนวนเต็ม		total_len;
    char_u	*buf;
    char_u	*count_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    สำหรับ (จำนวนเต็ม i = 0; i < um->prop_count; ++i)
    {
	textprop_T *prop = &um->props[i];

	ถ้า (prop->tp_flags & TP_FLAG_DELETED)
	    continue;
	++live_count;
	ถ้า (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    ถ้า (live_count == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	ถ้า (buf == NULL)
	    คืน NULL;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	คืน buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (จำนวนเต็ม)PROP_COUNT_SIZE
		+ live_count * (จำนวนเต็ม)parimaa_na(textprop_T) + vtext_size;
    buf = alloc(total_len);
    ถ้า (buf == NULL)
	คืน NULL;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    count_dest = buf + um->text_size;
    mch_memmove(count_dest, &live_count, PROP_COUNT_SIZE);

    prop_dest = count_dest + PROP_COUNT_SIZE;
    vtext_dest = prop_dest + live_count * parimaa_na(textprop_T);

    สำหรับ (จำนวนเต็ม i = 0; i < um->prop_count; ++i)
    {
	textprop_T prop = um->props[i];

	ถ้า (prop.tp_flags & TP_FLAG_DELETED)
	    continue;

	ถ้า (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    จำนวนเต็ม copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	มิฉะนั้น
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, parimaa_na(textprop_T));
	prop_dest += parimaa_na(textprop_T);
    }

    คืน buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    static void
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    จำนวนเต็ม	    packed_len;

    ถ้า (!um->detached || um->buf == NULL || um->lnum == 0)
	คืน;

    packed = um_pack(um, &packed_len);
    ถ้า (packed == NULL)
    {
	um->buf = NULL;
	คืน;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    ถ้า (um->buf->b_ml.ml_line_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    มิฉะนั้น ถ้า (!(um->buf->b_ml.ml_flags & ML_LINE_DIRTY))
	(void)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_count);

    ถ้า (um->buf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
    ถ้า (um->buf == NULL)
	คืน;
    ถ้า (um->detached && um->lnum > 0)
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
static จำนวนเต็ม proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    static hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    ถ้า (*name == NUL)
	คืน NULL;
    ถ้า (buf == NULL)
	ht = global_proptypes;
    มิฉะนั้น
	ht = buf->b_proptypes;

    ถ้า (ht == NULL)
	คืน NULL;
    hi = hash_find(ht, name);
    ถ้า (HASHITEM_EMPTY(hi))
	คืน NULL;
    คืน hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    static proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    ถ้า (hi == NULL)
	คืน NULL;
    คืน HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    จำนวนเต็ม
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    ถ้า (pt == NULL)
	คืน 0;
    คืน pt->pt_id;
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

    ถ้า (type == NULL)
	type = find_prop_type(name, NULL);
    ถ้า (type == NULL)
	semsg(_(e_property_type_str_does_not_exist), name);
    คืน type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    static จำนวนเต็ม
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    ถ้า (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	คืน FAIL;
    }
    ถ้า (arg->vval.v_dict == NULL)
	คืน OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    ถ้า (di != NULL && (di->di_tv.v_type != VAR_NUMBER
					      || di->di_tv.vval.v_number != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	ถ้า (*buf == NULL)
	    คืน FAIL;
    }
    คืน OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    void
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	คืน;

    start_lnum = tv_get_number(&argvars[0]);
    start_col = tv_get_number(&argvars[1]);
    ถ้า (check_for_dict_arg(argvars, 2) == FAIL)
	คืน;

    rettv->vval.v_number = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    static จำนวนเต็ม
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	จำนวนเต็ม		id,
	char_u		*text_arg,
	จำนวนเต็ม		text_padding_left,
	จำนวนเต็ม		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    จำนวนเต็ม		proplen;
    char_u	*props = NULL;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    จำนวนเต็ม		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    จำนวนเต็ม		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    ถ้า (type == NULL)
	goto theend;

    ถ้า (start_lnum < 1 || start_lnum > buf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), (long)start_lnum);
	goto theend;
    }
    ถ้า (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), (long)end_lnum);
	goto theend;
    }

    ถ้า (buf->b_ml.ml_mfp == NULL)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	goto theend;
    }

    ถ้า (text != NULL)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	สำหรับ (p = text; *p != NUL; MB_PTR_ADV(p))
	    ถ้า (*p < ' ')
		*p = ' ';
    }

    สำหรับ (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	ถ้า (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_line));
	    goto theend;
	}

	ถ้า (lnum == start_lnum)
	    col = start_col;
	มิฉะนั้น
	    col = 1;
	ถ้า (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != NULL))
	{
	    semsg(_(e_invalid_column_number_nr), (long)start_col);
	    goto theend;
	}
	sort_col = col;

	ถ้า (lnum == end_lnum)
	    length = end_col - col;
	มิฉะนั้น
	    length = (จำนวนเต็ม)textlen - col + 1;
	ถ้า (length > (long)textlen)
	    length = (จำนวนเต็ม)textlen;	// can include the end-of-line
	ถ้า (length < 0)
	    length = 0;		// zero-width property

	ถ้า (text_arg != NULL)
	{
	    length = (long)STRLEN(text_arg);
	    ถ้า (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		ถ้า ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	ถ้า (text_arg != NULL)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	สำหรับ (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * parimaa_na(textprop_T),
							   parimaa_na(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    ถ้า (prop_col >= sort_col)
		bha_nga;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	จำนวนเต็ม		vtext_total = 0;
	จำนวนเต็ม		new_line_len;
	char_u	*count_dest;
	char_u	*vtext_dest;
	จำนวนเต็ม		j;

	// Compute total vtext size from existing props.
	สำหรับ (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * parimaa_na(textprop_T),
							parimaa_na(textprop_T));
	    ถ้า (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	ถ้า (text_arg != NULL)
	    vtext_total += length + 1;

	new_line_len = (จำนวนเต็ม)textlen + (จำนวนเต็ม)PROP_COUNT_SIZE
			+ new_propcount * (จำนวนเต็ม)parimaa_na(textprop_T)
			+ vtext_total;
	newtext = alloc(new_line_len);
	ถ้า (newtext == NULL)
	    goto theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_line_ptr, textlen);

	// Write prop_count.
	count_dest = newtext + textlen;
	mch_memmove(count_dest, &new_propcount, PROP_COUNT_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = count_dest + PROP_COUNT_SIZE;
	ถ้า (i > 0)
	    mch_memmove(newprops, props, parimaa_na(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	ถ้า (i < proplen)
	    mch_memmove(newprops + (i + 1) * parimaa_na(textprop_T),
					props + i * parimaa_na(textprop_T),
					parimaa_na(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * parimaa_na(textprop_T);
	สำหรับ (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    ถ้า (j == i)
		continue;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * parimaa_na(textprop_T),
		    parimaa_na(textprop_T));
	    ถ้า (ep.tp_id < 0 && ep.tp_len > 0)
	    {
		// Copy existing vtext from old memline data.
		char_u *old_count = (char_u *)props - PROP_COUNT_SIZE;
		char_u *old_vtext = old_count + ep.u.tp_text_offset;

		mch_memmove(vtext_dest, old_vtext, ep.tp_len + 1);
		ep.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
		mch_memmove(newprops + j * parimaa_na(textprop_T), &ep,
			parimaa_na(textprop_T));
		vtext_dest += ep.tp_len + 1;
	    }
	}

	// Write new prop with vtext.
	ถ้า (text_arg != NULL)
	{
	    จำนวนเต็ม copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	มิฉะนั้น
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * parimaa_na(textprop_T), &tmp_prop,
							parimaa_na(textprop_T));

	ถ้า (buf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
    คืน res;
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
    จำนวนเต็ม		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    จำนวนเต็ม		error = FALSE;
    จำนวนเต็ม		prev_did_emsg = did_emsg;

    ถ้า (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	คืน;

    ถ้า (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	คืน;

    dict = argvars[0].vval.v_dict;
    ถ้า (dict == NULL || !dict_has_key(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	คืน;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ถ้า (dict_has_key(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_number(dict, "id");
	ถ้า (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    คืน;
	}
	id = (จำนวนเต็ม)x;
    }

    ถ้า (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	คืน;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_ITEMS(argvars[1].vval.v_list, li)
    {
	ถ้า (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == NULL)
	{
	    emsg(_(e_list_required));
	    คืน;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	ถ้า (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	ถ้า (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	ถ้า (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	จำนวนเต็ม this_id = id;
	ถ้า (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	ถ้า (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    ถ้า (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    คืน;
	}
	ถ้า (prop_add_one(buf, type_name, this_id, NULL, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    คืน;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
static จำนวนเต็ม vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    static จำนวนเต็ม
get_textprop_id(void)
{
    ถ้า (vt_id_counter > 0)
	vt_id_counter = 0;
    คืน --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    จำนวนเต็ม
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
    จำนวนเต็ม		id = 0;
    char_u	*text = NULL;
    จำนวนเต็ม		text_padding_left = 0;
    จำนวนเต็ม		flags = 0;

    ถ้า (dict == NULL || !dict_has_key(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	goto theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ถ้า (dict_has_key(dict, "end_lnum"))
    {
	end_lnum = dict_get_number(dict, "end_lnum");
	ถ้า (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_value_for_argument_str), "end_lnum");
	    goto theend;
	}
    }
    มิฉะนั้น
	end_lnum = start_lnum;

    ถ้า (dict_has_key(dict, "length"))
    {
	long length = dict_get_number(dict, "length");

	ถ้า (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_value_for_argument_str), "length");
	    goto theend;
	}
	end_col = start_col + length;
    }
    มิฉะนั้น ถ้า (dict_has_key(dict, "end_col"))
    {
	end_col = dict_get_number(dict, "end_col");
	ถ้า (end_col <= 0)
	{
	    semsg(_(e_invalid_value_for_argument_str), "end_col");
	    goto theend;
	}
    }
    มิฉะนั้น ถ้า (start_lnum == end_lnum)
	end_col = start_col;
    มิฉะนั้น
	end_col = 1;

    ถ้า (dict_has_key(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_number(dict, "id");
	ถ้า (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    goto theend;
	}
	id = (จำนวนเต็ม)x;
    }

    ถ้า (dict_has_key(dict, "text"))
    {
	ถ้า (dict_has_key(dict, "length")
		|| dict_has_key(dict, "end_col")
		|| dict_has_key(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    goto theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	ถ้า (text == NULL)
	    goto theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	ถ้า (dict_has_key(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    ถ้า (p == NULL)
		goto theend;
	    ถ้า (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		goto theend;
	    }
	    ถ้า (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_RIGHT;
	    มิฉะนั้น ถ้า (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    มิฉะนั้น ถ้า (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    มิฉะนั้น ถ้า (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "text_align", p);
		goto theend;
	    }
	}

	ถ้า (dict_has_key(dict, "text_padding_left"))
	{
	    text_padding_left = dict_get_number(dict, "text_padding_left");
	    ถ้า (text_padding_left < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		goto theend;
	    }
	}

	ถ้า (dict_has_key(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    ถ้า (p == NULL)
		goto theend;
	    ถ้า (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    มิฉะนั้น ถ้า (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "text_wrap", p);
		goto theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    ถ้า (start_col < (text == NULL ? 1 : 0))
    {
	semsg(_(e_invalid_column_number_nr), (long)start_col);
	goto theend;
    }
    ถ้า (start_col > 0 && text_padding_left > 0)
    {
	emsg(_(e_can_only_use_left_padding_when_column_is_zero));
	goto theend;
    }

    ถ้า (dict_arg != NULL && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	goto theend;

    ถ้า (text != NULL)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    มิฉะนั้น ถ้า (id < 0)
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
    คืน id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    จำนวนเต็ม
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, จำนวนเต็ม will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_count;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    ถ้า ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == NULL)
	คืน 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_line_len - textlen;
    ถ้า (propdata_len == 0)
	คืน 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    ถ้า (propdata_len < PROP_COUNT_SIZE + parimaa_na(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	คืน 0;
    }
    mch_memmove(&prop_count, text + textlen, PROP_COUNT_SIZE);
    ถ้า (!text_prop_count_valid(prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	คืน 0;
    }
    ถ้า (!text_prop_vtext_valid(text + textlen + PROP_COUNT_SIZE,
					   (จำนวนเต็ม)prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	คืน 0;
    }
    *props = text + textlen + PROP_COUNT_SIZE;
    คืน (จำนวนเต็ม)prop_count;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    จำนวนเต็ม
prop_count_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    จำนวนเต็ม		count = get_text_props(buf, lnum, &props, FALSE);
    จำนวนเต็ม		result = 0;
    textprop_T	prop;
    จำนวนเต็ม		i;
    จำนวนเต็ม		next_right_goes_below = FALSE;

    ถ้า (count == 0)
	คืน 0;
    สำหรับ (i = 0; i < count; ++i)
    {
	mch_memmove(&prop, props + i * parimaa_na(prop), parimaa_na(prop));
	ถ้า (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    ถ้า ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_right_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_RIGHT)))
	    {
		++result;
	    }
	    มิฉะนั้น ถ้า (prop.tp_flags & TP_FLAG_ALIGN_RIGHT)
		next_right_goes_below = TRUE;
	}
    }
    คืน result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    จำนวนเต็ม
count_props(linenr_T lnum, จำนวนเต็ม only_starting, จำนวนเต็ม last_line)
{
    char_u	*props;
    จำนวนเต็ม		proplen = get_text_props(curbuf, lnum, &props, 0);
    จำนวนเต็ม		result = proplen;
    textprop_T	prop;

    สำหรับ (จำนวนเต็ม i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * parimaa_na(prop), parimaa_na(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	ถ้า ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_line && prop.tp_col == MAXCOL))
	    --result;
    }
    คืน result;
}

static textprop_T	*text_prop_compare_props;
static buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    static จำนวนเต็ม
text_prop_order(จำนวนเต็ม flags)
{
    ถ้า (flags & TP_FLAG_ALIGN_ABOVE)
	คืน 0;
    ถ้า (flags & TP_FLAG_ALIGN_RIGHT)
	คืน 2;
    ถ้า (flags & TP_FLAG_ALIGN_BELOW)
	คืน 3;
    คืน 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    static จำนวนเต็ม
text_prop_compare(const void *s1, const void *s2)
{
    จำนวนเต็ม  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(จำนวนเต็ม *)s1;
    idx2 = *(จำนวนเต็ม *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    ถ้า ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	คืน tp1->tp_id < 0 ? 1 : -1;

    ถ้า (col1 == MAXCOL || col2 == MAXCOL)
    {
	จำนวนเต็ม order1 = text_prop_order(tp1->tp_flags);
	จำนวนเต็ม order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	ถ้า (order1 != order2)
	    คืน order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    ถ้า (pt1 != pt2)
    {
	ถ้า (pt1 == NULL)
	    คืน -1;
	ถ้า (pt2 == NULL)
	    คืน 1;
	ถ้า (pt1->pt_priority != pt2->pt_priority)
	    คืน pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    ถ้า (col1 != col2)
	คืน col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    ถ้า (tp1->tp_id < 0)
	คืน tp1->tp_id > tp2->tp_id ? 1 : -1;

    คืน 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    void
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	จำนวนเต็ม	    *idxs,
	จำนวนเต็ม	    count)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((void *)idxs, (size_t)count, parimaa_na(จำนวนเต็ม), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_lines(
	win_T	    *wp,
	จำนวนเต็ม	    type_id,
	จำนวนเต็ม	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    ถ้า (text_prop_type_by_id(wp->w_buffer, type_id) == NULL)
	คืน false;

    ถ้า (first_lnum < 1)
	first_lnum = 1;
    ถ้า (last_lnum > wp->w_buffer->b_ml.ml_line_count)
	last_lnum = wp->w_buffer->b_ml.ml_line_count;

    สำหรับ (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	จำนวนเต็ม	count = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	สำหรับ (จำนวนเต็ม i = 0; i < count; ++i)
	{
	    mch_memmove(prop, props + i * parimaa_na(textprop_T),
							   parimaa_na(textprop_T));
	    ถ้า (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		คืน true;
	    }
	}
    }
    คืน false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	จำนวนเต็ม	    type_id,
	จำนวนเต็ม	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    คืน find_prop_in_lines(wp, type_id, id, prop, found_lnum,
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
set_text_props(linenr_T lnum, textprop_T *tps, จำนวนเต็ม count)
{
    char_u  *text;
    char_u  *newtext;
    จำนวนเต็ม	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    ถ้า (count == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	ถ้า (newtext == NULL)
	    คืน;
	mch_memmove(newtext, text, textlen);
	ถ้า (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_line_ptr);
	curbuf->b_ml.ml_line_ptr = newtext;
	curbuf->b_ml.ml_line_len = textlen;
	curbuf->b_ml.ml_flags |= ML_LINE_DIRTY;
    }
    มิฉะนั้น
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_count = (uint16_t)count;
	จำนวนเต็ม	    vtext_size = 0;
	จำนวนเต็ม	    total_len;
	char_u	    *count_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	จำนวนเต็ม	    i;

	สำหรับ (i = 0; i < count; ++i)
	    ถ้า (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (จำนวนเต็ม)PROP_COUNT_SIZE
		    + count * (จำนวนเต็ม)parimaa_na(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	ถ้า (newtext == NULL)
	    คืน;
	mch_memmove(newtext, text, textlen);

	count_dest = newtext + textlen;
	mch_memmove(count_dest, &prop_count, PROP_COUNT_SIZE);

	prop_dest = count_dest + PROP_COUNT_SIZE;
	vtext_dest = prop_dest + count * parimaa_na(textprop_T);

	สำหรับ (i = 0; i < count; ++i)
	{
	    textprop_T prop = tps[i];

	    ถ้า (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		จำนวนเต็ม copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = NULL;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
		vtext_dest += copysize;
	    }
	    มิฉะนั้น
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, parimaa_na(textprop_T));
	    prop_dest += parimaa_na(textprop_T);
	}

	ถ้า (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
props_add_count_header(char_u *line, จำนวนเต็ม line_len, จำนวนเต็ม textlen, จำนวนเต็ม *new_len)
{
    จำนวนเต็ม		prop_bytes = line_len - textlen;
    uint16_t	prop_count;
    จำนวนเต็ม		vtext_size = 0;
    จำนวนเต็ม		total;
    char_u	*newline;
    char_u	*count_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    จำนวนเต็ม		i;

    ถ้า (prop_bytes <= 0 || prop_bytes % parimaa_na(textprop_T) != 0)
    {
	*new_len = line_len;
	คืน NULL;
    }
    prop_count = (uint16_t)(prop_bytes / parimaa_na(textprop_T));

    // Calculate total vtext size.
    สำหรับ (i = 0; i < (จำนวนเต็ม)prop_count; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, line + textlen + i * parimaa_na(textprop_T),
							parimaa_na(textprop_T));
	ถ้า (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (จำนวนเต็ม)PROP_COUNT_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    ถ้า (newline == NULL)
	คืน NULL;
    *new_len = total;

    mch_memmove(newline, line, textlen);
    count_dest = newline + textlen;
    mch_memmove(count_dest, &prop_count, PROP_COUNT_SIZE);
    prop_dest = count_dest + PROP_COUNT_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    สำหรับ (i = 0; i < (จำนวนเต็ม)prop_count; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, line + textlen + i * parimaa_na(textprop_T),
							parimaa_na(textprop_T));
	ถ้า (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    จำนวนเต็ม copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	มิฉะนั้น
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * parimaa_na(textprop_T), &prop,
							parimaa_na(textprop_T));
    }
    คืน newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    void
add_text_props(linenr_T lnum, textprop_T *text_props, จำนวนเต็ม text_prop_count)
{
    unpacked_memline_T	um;
    จำนวนเต็ม			i;

    um = um_open_at(curbuf, lnum, text_prop_count);
    ถ้า (um.buf == NULL)
	คืน;
    ถ้า (!um_detach(&um))
    {
	um_abort(&um);
	คืน;
    }

    // Grow props array if needed.
    ถ้า (um.prop_count + text_prop_count > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_count + text_prop_count) * parimaa_na(textprop_T));
	ถ้า (newprops == NULL)
	{
	    um_abort(&um);
	    คืน;
	}
	um.props = newprops;
	um.prop_size = um.prop_count + text_prop_count;
    }

    สำหรับ (i = 0; i < text_prop_count; ++i)
	um.props[um.prop_count++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    static จำนวนเต็ม
compare_pt(const void *s1, const void *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    คืน tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    static proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, จำนวนเต็ม id)
{
    จำนวนเต็ม low = 0;
    จำนวนเต็ม high;

    ถ้า (ht == NULL || ht->ht_used == 0)
	คืน NULL;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    ถ้า (*array == NULL)
    {
	long	    todo;
	hashitem_T  *hi;
	จำนวนเต็ม	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	ถ้า (*array == NULL)
	    คืน NULL;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
	{
	    ถ้า (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((void *)*array, ht->ht_used, parimaa_na(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    high = ht->ht_used;
    ขณะ (high > low)
    {
	จำนวนเต็ม m = (high + low) / 2;

	ถ้า ((*array)[m]->pt_id == id)
	    คืน (*array)[m];
	ถ้า ((*array)[m]->pt_id > id)
	    high = m;
	มิฉะนั้น
	    low = m + 1;
    }
    คืน NULL;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    static void
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    จำนวนเต็ม buflocal = TRUE;
    จำนวนเต็ม virtualtext_prop = prop->tp_id < 0;

    dict_add_number(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    ถ้า (!virtualtext_prop)
    {
	dict_add_number(dict, "length", prop->tp_len);
	dict_add_number(dict, "id", prop->tp_id);
    }
    dict_add_number(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_number(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    ถ้า (pt == NULL)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    ถ้า (pt != NULL)
	dict_add_string(dict, "type", pt->pt_name);

    ถ้า (buflocal)
	dict_add_number(dict, "type_bufnr", buf->b_fnum);
    มิฉะนั้น
	dict_add_number(dict, "type_bufnr", 0);
    ถ้า (virtualtext_prop)
    {
	string_T    text_align = {NULL, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	ถ้า (prop->tp_flags & TP_FLAG_ALIGN_RIGHT)
	    STR_LITERAL_SET(text_align, "right");
	มิฉะนั้น ถ้า (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	มิฉะนั้น ถ้า (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	ถ้า (text_align.string != NULL)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (จำนวนเต็ม)text_align.length);

	// text_wrap
	ถ้า (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	ถ้า (prop->tp_padleft != 0)
	    dict_add_number(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, จำนวนเต็ม id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    ถ้า (type == NULL)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    คืน type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    คืน text_prop_type_by_id(buf, prop->tp_type) != NULL;
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
    จำนวนเต็ม	    did_clear = FALSE;

    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	คืน;

    start = tv_get_number(&argvars[0]);
    end = start;
    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_number(&argvars[1]);
	ถ้า (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ถ้า (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		คืน;
	}
    }
    ถ้า (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	คืน;
    }

    {
	unpacked_memline_T um = um_open(buf);

	สำหรับ (lnum = start; lnum <= end; ++lnum)
	{
	    จำนวนเต็ม idx;

	    ถ้า (lnum > buf->b_ml.ml_line_count)
		bha_nga;
	    ถ้า (!um_goto_line(&um, lnum, 0))
		bha_nga;
	    ถ้า (um.prop_count > 0)
	    {
		did_clear = TRUE;
		สำหรับ (idx = um.prop_count - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    ถ้า (did_clear)
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
    จำนวนเต็ม		lnum_start;
    จำนวนเต็ม		start_pos_has_prop = 0;
    จำนวนเต็ม		seen_end = FALSE;
    จำนวนเต็ม		id = 0;
    จำนวนเต็ม		id_found = FALSE;
    จำนวนเต็ม		type_id = -1;
    จำนวนเต็ม		skipstart = FALSE;
    จำนวนเต็ม		lnum = -1;
    จำนวนเต็ม		col = -1;
    จำนวนเต็ม		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    จำนวนเต็ม		both;

    ถ้า (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	คืน;

    ถ้า (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	คืน;
    dict = argvars[0].vval.v_dict;

    ถ้า (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	คืน;
    ถ้า (buf->b_ml.ml_mfp == NULL)
	คืน;

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	ถ้า (*dir_s == 'b')
	    dir = BACKWARD;
	มิฉะนั้น ถ้า (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    คืน;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    ถ้า (di != NULL)
	lnum = tv_get_number(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    ถ้า (di != NULL)
	col = tv_get_number(&di->di_tv);

    ถ้า (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    มิฉะนั้น ถ้า (col == -1)
	col = 1;

    ถ้า (lnum < 1 || lnum > buf->b_ml.ml_line_count)
    {
	emsg(_(e_invalid_range));
	คืน;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    ถ้า (dict_has_key(dict, "id"))
    {
	id = dict_get_number(dict, "id");
	id_found = TRUE;
    }
    ถ้า (dict_has_key(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ถ้า (type == NULL)
	    คืน;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    ถ้า (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	คืน;
    }
    ถ้า (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	คืน;
    }

    lnum_start = lnum;

    ถ้า (rettv_dict_alloc(rettv) == FAIL)
	คืน;

    ขณะ (1)
    {
	char_u	*prop_start_ptr;
	จำนวนเต็ม	count = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	จำนวนเต็ม	    i;
	textprop_T  prop;
	จำนวนเต็ม	    prop_start;
	จำนวนเต็ม	    prop_end;

	สำหรับ (i = dir == BACKWARD ? count - 1 : 0; i >= 0 && i < count; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * parimaa_na(textprop_T),
							   parimaa_na(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    ถ้า (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_COUNT_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    มิฉะนั้น
		prop.u.tp_text = NULL;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    ถ้า (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		ถ้า (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    ถ้า (!is_floating_vtext && prop.tp_col > col)
			continue;
		}
		มิฉะนั้น ถ้า (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    continue;
	    }
	    ถ้า (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		ถ้า (lnum_start == lnum
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
		ถ้า (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		ถ้า (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    ถ้า (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    continue;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		ถ้า (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    continue;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_number(rettv->vval.v_dict, "lnum", lnum);

		คืน;
	    }
	}

	ถ้า (dir > 0)
	{
	    ถ้า (lnum >= buf->b_ml.ml_line_count)
		bha_nga;
	    lnum++;
	}
	มิฉะนั้น
	{
	    ถ้า (lnum <= 1)
		bha_nga;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    static จำนวนเต็ม
prop_type_or_id_in_list(จำนวนเต็ม *types_or_ids, จำนวนเต็ม len, จำนวนเต็ม type_or_id)
{
    จำนวนเต็ม i;

    สำหรับ (i = 0; i < len; i++)
	ถ้า (types_or_ids[i] == type_or_id)
	    คืน TRUE;

    คืน FALSE;
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
	จำนวนเต็ม		*prop_types,
	จำนวนเต็ม		prop_types_len,
	จำนวนเต็ม		*prop_ids,
	จำนวนเต็ม		prop_ids_len,
	list_T		*retlist,
	จำนวนเต็ม		add_lnum)
{
    char_u	*props;
    จำนวนเต็ม		count;
    จำนวนเต็ม		i;
    textprop_T	prop;

    count = get_text_props(buf, lnum, &props, FALSE);
    สำหรับ (i = 0; i < count; ++i)
    {
	mch_memmove(&prop, props + i * parimaa_na(textprop_T),
		parimaa_na(textprop_T));
	// Convert offset to pointer for virtual text props.
	ถ้า (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_COUNT_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	มิฉะนั้น
	    prop.u.tp_text = NULL;
	ถ้า ((prop_types == NULL
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == NULL
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    ถ้า (d == NULL)
		bha_nga;
	    prop_fill_dict(d, &prop, buf);
	    ถ้า (add_lnum)
		dict_add_number(d, "lnum", lnum);
	    ถ้า (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    static จำนวนเต็ม *
get_prop_types_from_names(list_T *l, buf_T *buf, จำนวนเต็ม *num_types)
{
    จำนวนเต็ม		*prop_types;
    listitem_T	*li;
    จำนวนเต็ม		i;
    char_u	*name;
    proptype_T	*type;

    *num_types = 0;

    prop_types = ALLOC_MULT(จำนวนเต็ม, list_len(l));
    ถ้า (prop_types == NULL)
	คืน NULL;

    i = 0;
    FOR_ALL_LIST_ITEMS(l, li)
    {
	ถ้า (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    goto errret;
	}
	name = li->li_tv.vval.v_string;
	ถ้า (name == NULL)
	    goto errret;

	type = lookup_prop_type(name, buf);
	ถ้า (type == NULL)
	    goto errret;
	prop_types[i++] = type->pt_id;
    }

    *num_types = i;
    คืน prop_types;

errret:
    VIM_CLEAR(prop_types);
    คืน NULL;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    static จำนวนเต็ม *
get_prop_ids_from_list(list_T *l, จำนวนเต็ม *num_ids)
{
    จำนวนเต็ม		*prop_ids;
    listitem_T	*li;
    จำนวนเต็ม		i = 0;
    จำนวนเต็ม		id;
    จำนวนเต็ม		error;

    *num_ids = 0;

    prop_ids = ALLOC_MULT(จำนวนเต็ม, list_len(l));
    ถ้า (prop_ids == NULL)
	คืน NULL;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	error = FALSE;
	id = tv_get_number_chk(&li->li_tv, &error);
	ถ้า (error)
	    goto errret;

	prop_ids[i++] = id;
    }

    *num_ids = i;
    คืน prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    คืน NULL;
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
    จำนวนเต็ม		add_lnum = FALSE;
    จำนวนเต็ม		*prop_types = NULL;
    จำนวนเต็ม		prop_types_len = 0;
    จำนวนเต็ม		*prop_ids = NULL;
    จำนวนเต็ม		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	คืน;

    ถ้า (rettv_list_alloc(rettv) == FAIL)
	คืน;

    // default: get text properties on current line
    start_lnum = tv_get_number(&argvars[0]);
    end_lnum = start_lnum;
    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ถ้า (check_for_dict_arg(argvars, 1) == FAIL)
	    คืน;
	d = argvars[1].vval.v_dict;

	ถ้า (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    คืน;

	ถ้า (d != NULL && (di = dict_find(d, (char_u *)"end_lnum", -1)) != NULL)
	{
	    ถ้า (di->di_tv.v_type != VAR_NUMBER)
	    {
		emsg(_(e_number_required));
		คืน;
	    }
	    end_lnum = tv_get_number(&di->di_tv);
	    ถ้า (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		// line
		end_lnum = buf->b_ml.ml_line_count + end_lnum + 1;
	    มิฉะนั้น ถ้า (end_lnum > buf->b_ml.ml_line_count)
		end_lnum = buf->b_ml.ml_line_count;
	    add_lnum = TRUE;
	}
	ถ้า (d != NULL && (di = dict_find(d, (char_u *)"types", -1)) != NULL)
	{
	    ถ้า (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		คืน;
	    }

	    l = di->di_tv.vval.v_list;
	    ถ้า (l != NULL && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		ถ้า (prop_types == NULL)
		    คืน;
	    }
	}
	ถ้า (d != NULL && (di = dict_find(d, (char_u *)"ids", -1)) != NULL)
	{
	    ถ้า (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		goto errret;
	    }

	    l = di->di_tv.vval.v_list;
	    ถ้า (l != NULL && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		ถ้า (prop_ids == NULL)
		    goto errret;
	    }
	}
    }
    ถ้า (start_lnum < 1 || start_lnum > buf->b_ml.ml_line_count
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    มิฉะนั้น
	สำหรับ (lnum = start_lnum; lnum <= end_lnum; lnum++)
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
    จำนวนเต็ม		do_all;
    จำนวนเต็ม		id = -MAXCOL;
    จำนวนเต็ม		type_id = -1;	    // for a single "type"
    จำนวนเต็ม		*type_ids = NULL;   // array, for a list of "types", allocated
    จำนวนเต็ม		num_type_ids = 0;   // number of elements in "type_ids"
    จำนวนเต็ม		both;

    rettv->vval.v_number = 0;

    ถ้า (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	คืน;

    ถ้า (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	คืน;

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_number(&argvars[1]);
	end = start;
	ถ้า (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_number(&argvars[2]);
	ถ้า (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    คืน;
	}
    }

    dict = argvars[0].vval.v_dict;
    ถ้า (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	คืน;
    ถ้า (buf->b_ml.ml_mfp == NULL)
	คืน;

    do_all = dict_get_bool(dict, "all", FALSE);

    ถ้า (dict_has_key(dict, "id"))
	id = dict_get_number(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    ถ้า (dict_has_key(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ถ้า (type == NULL)
	    คืน;
	type_id = type->pt_id;
    }
    ถ้า (dict_has_key(dict, "types"))
    {
	typval_T types;
	listitem_T *li = NULL;

	dict_get_tv(dict, "types", &types);
	ถ้า (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( parimaa_na(จำนวนเต็ม) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_ITEMS(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		ถ้า (li->li_tv.v_type != VAR_STRING)
		    continue;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		ถ้า (!prop_type)
		    goto cleanup_prop_remove;

		type_ids[num_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    ถ้า (id == -MAXCOL && (type_id == -1 && num_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	goto cleanup_prop_remove;
    }
    ถ้า (both && (id == -MAXCOL || (type_id == -1 && num_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	goto cleanup_prop_remove;
    }
    ถ้า (type_id != -1 && num_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	goto cleanup_prop_remove;
    }

    ถ้า (end == 0)
	end = buf->b_ml.ml_line_count;

    {
	unpacked_memline_T um = um_open(buf);

	สำหรับ (lnum = start; lnum <= end; ++lnum)
	{
	    จำนวนเต็ม idx;

	    ถ้า (lnum > buf->b_ml.ml_line_count)
		bha_nga;
	    ถ้า (!um_goto_line(&um, lnum, 0))
		bha_nga;

	    สำหรับ (idx = 0; idx < um.prop_count; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		จำนวนเต็ม	    matches_id = 0;
		จำนวนเต็ม	    matches_type = 0;

		ถ้า (prop->tp_flags & TP_FLAG_DELETED)
		    continue;

		matches_id = prop->tp_id == id;
		ถ้า (num_type_ids > 0)
		{
		    จำนวนเต็ม idx2;

		    สำหรับ (idx2 = 0; !matches_type && idx2 < num_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		มิฉะนั้น
		    matches_type = prop->tp_type == type_id;

		ถ้า (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    ถ้า (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_number;
		    ถ้า (!do_all)
			bha_nga;
		}
	    }
	}
	um_close(&um);
    }

    ถ้า (first_changed > 0)
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
prop_type_set(typval_T *argvars, จำนวนเต็ม add)
{
    char_u	*name;
    buf_T	*buf = NULL;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	คืน;

    name = tv_get_string(&argvars[0]);
    ถ้า (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	คืน;
    }

    ถ้า (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	คืน;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    ถ้า (add)
    {
	hashtab_T **htp;

	ถ้า (prop != NULL)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    คืน;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	ถ้า (prop == NULL)
	    คืน;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	ถ้า (buf == NULL)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	มิฉะนั้น
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	ถ้า (*htp == NULL)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    ถ้า (*htp == NULL)
	    {
		vim_free(prop);
		คืน;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    มิฉะนั้น
    {
	ถ้า (prop == NULL)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    คืน;
	}
    }

    ถ้า (dict != NULL)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	ถ้า (di != NULL)
	{
	    char_u	*highlight;
	    จำนวนเต็ม		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    ถ้า (highlight != NULL && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    ถ้า (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == NULL ? (char_u *)"" : highlight);
		คืน;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	ถ้า (di != NULL)
	{
	    ถ้า (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    มิฉะนั้น
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	ถ้า (di != NULL)
	{
	    ถ้า (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    มิฉะนั้น
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	ถ้า (di != NULL)
	    prop->pt_priority = tv_get_number(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	ถ้า (di != NULL)
	{
	    ถ้า (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    มิฉะนั้น
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	ถ้า (di != NULL)
	{
	    ถ้า (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    มิฉะนั้น
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

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	คืน;

    name = tv_get_string(&argvars[0]);
    ถ้า (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	คืน;
    }

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	ถ้า (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    คืน;
    }

    hi = find_prop_type_hi(name, buf);
    ถ้า (hi == NULL)
	คืน;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    ถ้า (buf == NULL)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    มิฉะนั้น
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

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	คืน;

    name = tv_get_string(&argvars[0]);
    ถ้า (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	คืน;
    }

    ถ้า (rettv_dict_alloc(rettv) == FAIL)
	คืน;

    proptype_T  *prop = NULL;
    buf_T	    *buf = NULL;

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	ถ้า (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    คืน;
    }

    prop = find_prop_type(name, buf);
    ถ้า (prop == NULL)
	คืน;

    dict_T *d = rettv->vval.v_dict;

    ถ้า (prop->pt_hl_id > 0)
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
    ถ้า (buf != NULL)
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
	ถ้า (!HASHITEM_EMPTY(hi))
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

    ถ้า (rettv_list_alloc(rettv) == FAIL)
	คืน;

    ถ้า (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	คืน;

    ถ้า (argvars[0].v_type != VAR_UNKNOWN)
    {
	ถ้า (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    คืน;
    }
    ถ้า (buf == NULL)
    {
	ถ้า (global_proptypes != NULL)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    มิฉะนั้น ถ้า (buf->b_proptypes != NULL)
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

    ถ้า (ht == NULL)
	คืน;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	ถ้า (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#ถ้า defined(EXITFREE)
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
typedef sa_nracanaa
{
    จำนวนเต็ม dirty;	    // if the property was changed
    จำนวนเต็ม can_drop;   // whether after this change, the prop may be removed
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
	จำนวนเต็ม	    added,
	จำนวนเต็ม	    flags)
{
    proptype_T	*pt;
    จำนวนเต็ม		start_incl;
    จำนวนเต็ม		end_incl;
    จำนวนเต็ม		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    ถ้า (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	คืน res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    ถ้า (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    ถ้า (added > 0)
    {
	ถ้า (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	มิฉะนั้น ถ้า (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    มิฉะนั้น ถ้า (prop->tp_col > col + 1)
    {
	ถ้า (prop->tp_col + added < col + 1)
	{
	    ถ้า (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    มิฉะนั้น
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		ถ้า (prop->tp_len <= 0)
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
	มิฉะนั้น
	    prop->tp_col += added;
    }
    มิฉะนั้น ถ้า (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	ถ้า (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	มิฉะนั้น
	{
	    จำนวนเต็ม after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	ถ้า (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    มิฉะนั้น
	res.dirty = FALSE;

    คืน res;
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
    จำนวนเต็ม
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	จำนวนเต็ม	    bytes_added,
	จำนวนเต็ม	    flags)
{
    unpacked_memline_T um;
    จำนวนเต็ม		dirty = FALSE;
    จำนวนเต็ม		ri;

    ถ้า (text_prop_frozen > 0)
	คืน FALSE;

    um = um_open_at(curbuf, lnum, 0);
    ถ้า (um.buf == NULL || um.prop_count == 0)
    {
	um_abort(&um);
	คืน FALSE;
    }

    สำหรับ (ri = 0; ri < um.prop_count; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	ถ้า (res.dirty)
	{
	    ถ้า (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		ถ้า (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    คืน FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		ถ้า ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    คืน FALSE;
		}
	    }
	    dirty = TRUE;
	}
	ถ้า (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    continue;
	}
    }
    ถ้า (dirty)
	um_close(&um);
    มิฉะนั้น
	um_abort(&um);
    คืน dirty;
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
	จำนวนเต็ม	    kept,
	จำนวนเต็ม	    deleted,
	จำนวนเต็ม	    at_eol)
{
    char_u	*props;
    จำนวนเต็ม		count;
    garray_T    prevprop;
    garray_T    nextprop;
    จำนวนเต็ม		skipped = kept + deleted;

    ถ้า (!curbuf->b_has_textprop)
	คืน;

    // Get the text properties from "lnum_props".
    count = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, parimaa_na(textprop_T), 10);
    ga_init2(&nextprop, parimaa_na(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *count_ptr = props - PROP_COUNT_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    สำหรับ (จำนวนเต็ม i = 0; i < count; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	จำนวนเต็ม	    start_incl, end_incl;
	จำนวนเต็ม	    cont_prev, cont_next;
	จำนวนเต็ม	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * parimaa_na(textprop_T), parimaa_na(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	ถ้า (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(count_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	มิฉะนั้น
	    prop.u.tp_text = NULL;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	ถ้า (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	มิฉะนั้น
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    จำนวนเต็ม is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    ถ้า (is_floating_vtext)
		cont_next = FALSE;
	    มิฉะนั้น
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	ถ้า (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	ถ้า (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    จำนวนเต็ม is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    ถ้า (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    ถ้า (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	ถ้า (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    จำนวนเต็ม is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    ถ้า (p->tp_col != MAXCOL)
	    {
		ถ้า (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		มิฉะนั้น
		{
		    ถ้า (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    ถ้า (cont_prev)
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
	จำนวนเต็ม		    last_line,
	long		    col,
	จำนวนเต็ม		    removed)
{
    unpacked_memline_T	r_um;

    ถ้า (um->buf == NULL)
	คืน;

    r_um = um_open_at(um->buf, lnum, 0);
    ถ้า (r_um.buf == NULL)
	คืน;
    ถ้า (!um_detach(&r_um))
    {
	um_abort(&r_um);
	คืน;
    }

    สำหรับ (จำนวนเต็ม i = r_um.prop_count - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	จำนวนเต็ม	    end;

	ถ้า (prop->tp_col == MAXCOL && !last_line)
	    continue;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	ถ้า (last_line || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = NULL;  // ownership transferred
	}
	มิฉะนั้น
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    สำหรับ (จำนวนเต็ม j = 0; j < um->prop_count; ++j)
	    {
		textprop_T *op = &um->props[j];

		ถ้า ((op->tp_flags & TP_FLAG_CONT_PREV)
			&& op->tp_id == prop->tp_id
			&& op->tp_type == prop->tp_type)
		{
		    found = true;
		    op->tp_len += op->tp_col - prop->tp_col;
		    op->tp_col = prop->tp_col;
		    op->tp_flags = prop->tp_flags;
		    bha_nga;
		}
	    }
	    ถ้า (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    static void
remember_cleared_prop(garray_T *gap, จำนวนเต็ม which, textprop_T *prop, จำนวนเต็ม flag)
{
    undoprop_T	*up;

    ถ้า (which == UNDOPROP_NONE)
	คืน;
    ถ้า (gap->ga_itemsize == 0)
	ga_init2(gap, parimaa_na(undoprop_T), 4);
    ถ้า (ga_grow(gap, 1) == FAIL)
	คืน;
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
	จำนวนเต็ม	    above_which,
	จำนวนเต็ม	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = NULL;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    ถ้า (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_line_count)
	คืน;

    above = um_open_at(buf, lnum, 0);
    ถ้า (above.buf == NULL)
	คืน;
    สำหรับ (จำนวนเต็ม i = 0; i < above.prop_count; ++i)
	ถ้า (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    ถ้า (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	คืน;
    }

    below = um_open_at(buf, lnum + 1, 0);
    ถ้า (below.buf == NULL)
    {
	um_abort(&above);
	คืน;
    }
    ถ้า (below.prop_count > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_count);
	ถ้า (paired == NULL)
	    goto theend;
    }

    สำหรับ (จำนวนเต็ม i = 0; i < above.prop_count; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	ถ้า (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    continue;
	สำหรับ (จำนวนเต็ม j = 0; j < below.prop_count; ++j)
	{
	    textprop_T *next = &below.props[j];

	    ถ้า (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		bha_nga;
	    }
	}
	ถ้า (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    สำหรับ (จำนวนเต็ม j = 0; j < below.prop_count; ++j)
    {
	ถ้า (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    continue;
	ถ้า (!below.detached && !um_detach(&below))
	    bha_nga;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    ถ้า (below_changed)
	um_close(&below);
    มิฉะนั้น
	um_abort(&below);
    ถ้า (above_changed)
	um_close(&above);
    มิฉะนั้น
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
	จำนวนเต็ม	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    ถ้า (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_line_count)
	คืน;
    um = um_open_at(buf, lnum, 0);
    ถ้า (um.buf == NULL)
	คืน;
    สำหรับ (จำนวนเต็ม r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	ถ้า (up->up_which != which)
	    continue;
	สำหรับ (จำนวนเต็ม i = 0; i < um.prop_count; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    ถ้า (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		continue;
	    ถ้า (!um.detached && !um_detach(&um))
		goto theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    bha_nga;
	}
    }

theend:
    ถ้า (changed)
	um_close(&um);
    มิฉะนั้น
	um_abort(&um);
}

    bool
text_prop_count_valid(จำนวนเต็ม prop_count, size_t propdata_len)
{
    ถ้า (propdata_len < PROP_COUNT_SIZE)
	คืน false;
    คืน (size_t)prop_count * parimaa_na(textprop_T)
		    <= propdata_len - PROP_COUNT_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, จำนวนเต็ม prop_count, size_t propdata_len)
{
    สำหรับ (จำนวนเต็ม i = 0; i < prop_count; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * parimaa_na(textprop_T),
							  parimaa_na(textprop_T));
	ถ้า (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    continue;
	ถ้า (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    คืน false;
    }
    คืน true;
}

#endif // FEAT_PROP_POPUP
