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

#ប្រសិនបើ defined(FEAT_PROP_POPUP)

static void um_store_changes(unpacked_memline_T *um);

/*
 * Free virtual text strings in a detached unpacked memline's props.
 */
    static void
um_free_vtext(textprop_T *props, ចំនួនគត់ count)
{
    សម្រាប់ (ចំនួនគត់ i = 0; i < count; ++i)
	ប្រសិនបើ (props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	    VIM_CLEAR(props[i].u.tp_text);
}

/*
 * Free memory used by an unpacked memline.
 */
    static void
um_free(unpacked_memline_T *um)
{
    ប្រសិនបើ (um->detached)
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
    ត្រឡប់ um;
}

/*
 * Load line "lnum" into an unpacked memline.  If a previous line was
 * loaded and modified, it is stored first.
 * "extra_props" is the number of extra prop slots to pre-allocate.
 * Returns true on success, false on error (um becomes closed).
 */
    bool
um_goto_line(unpacked_memline_T *um, linenr_T lnum, ចំនួនគត់ extra_props)
{
    char_u	*line;
    size_t	textlen;
    size_t	propdata_len;
    ចំនួនគត់		proplen;

    ប្រសិនបើ (um->buf == NULL)
	ត្រឡប់ false;

    // Store changes to the current line if any.
    ប្រសិនបើ (um->lnum > 0 && um->detached)
	um_store_changes(um);
    um_free(um);

    ប្រសិនបើ (lnum == 0)
	ត្រឡប់ true;	// just unload

    line = ml_get_buf(um->buf, lnum, FALSE);
    textlen = ml_get_buf_len(um->buf, lnum) + 1;
    propdata_len = um->buf->b_ml.ml_line_len - textlen;

    um->lnum = lnum;
    um->text_size = (colnr_T)textlen;
    um->text = line;

    ប្រសិនបើ (propdata_len == 0)
	ត្រឡប់ true;

    // New format: [prop_count (uint16)][textprop_T...][vtext...]
    ប្រសិនបើ (propdata_len < PROP_COUNT_SIZE + parimaa_na(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	ត្រឡប់ false;
    }

    uint16_t    prop_count;
    char_u	    *count_ptr = line + textlen;
    char_u	    *props_start;

    mch_memmove(&prop_count, count_ptr, PROP_COUNT_SIZE);
    ប្រសិនបើ (!text_prop_count_valid(prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	ត្រឡប់ false;
    }
    proplen = (ចំនួនគត់)prop_count;
    props_start = count_ptr + PROP_COUNT_SIZE;

    ប្រសិនបើ (!text_prop_vtext_valid(props_start, proplen, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	um->buf = NULL;
	ត្រឡប់ false;
    }

    um->props = ALLOC_MULT(textprop_T, proplen + extra_props);
    ប្រសិនបើ (um->props == NULL)
    {
	um->buf = NULL;
	ត្រឡប់ false;
    }
    um->prop_size = proplen + extra_props;
    um->prop_count = proplen;
    mch_memmove(um->props, props_start, proplen * parimaa_na(textprop_T));

    // Convert tp_text_offset to tp_text pointer for virtual text
    // props.
    សម្រាប់ (ចំនួនគត់ i = 0; i < proplen; ++i)
    {
	ប្រសិនបើ (um->props[i].tp_id < 0 && um->props[i].u.tp_text_offset > 0)
	{
	    um->props[i].u.tp_text = count_ptr + um->props[i].u.tp_text_offset;
	    um->props[i].tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	បើមិនដូច្នេះ
	    um->props[i].u.tp_text = NULL;
    }

    ត្រឡប់ true;
}

/*
 * Open an unpacked memline at a specific line.
 * Check um.buf != NULL for success.
 */
    unpacked_memline_T
um_open_at(buf_T *buf, linenr_T lnum, ចំនួនគត់ extra_props)
{
    unpacked_memline_T um = um_open(buf);

    ប្រសិនបើ (!um_goto_line(&um, lnum, extra_props))
	um.buf = NULL;
    ត្រឡប់ um;
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

    ប្រសិនបើ (um->detached || um->buf == NULL)
	ត្រឡប់ um->detached;

    newtext = vim_strnsave(um->text, um->text_size - 1);
    ប្រសិនបើ (newtext == NULL)
    {
	um->buf = NULL;
	ត្រឡប់ false;
    }
    um->text = newtext;

    សម្រាប់ (ចំនួនគត់ i = 0; i < um->prop_count; ++i)
    {
	ប្រសិនបើ (um->props[i].tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    char_u *copy = vim_strsave(um->props[i].u.tp_text);

	    ប្រសិនបើ (copy == NULL)
	    {
		សម្រាប់ (ចំនួនគត់ j = 0; j < i; ++j)
		    ប្រសិនបើ (um->props[j].tp_id < 0)
			VIM_CLEAR(um->props[j].u.tp_text);
		VIM_CLEAR(um->text);
		um->buf = NULL;
		ត្រឡប់ false;
	    }
	    um->props[i].u.tp_text = copy;
	}
    }

    um->detached = true;
    ត្រឡប់ true;
}

/*
 * Set the text of an unpacked memline.  "text" must be allocated memory;
 * ownership is transferred to the unpacked memline.
 */
    bool
um_set_text(unpacked_memline_T *um, char_u *text)
{
    ប្រសិនបើ (um->buf == NULL)
	ត្រឡប់ false;
    ប្រសិនបើ (um->detached)
	vim_free(um->text);
    um->text = text;
    um->text_size = (colnr_T)STRLEN(text) + 1;
    um->text_changed = true;
    um->detached = true;
    ត្រឡប់ true;
}

/*
 * Ensure there is space for at least "needed" more properties.
 * Returns true on success.
 */
    static bool
um_grow_props(unpacked_memline_T *um, ចំនួនគត់ needed)
{
    ប្រសិនបើ (um->prop_count + needed <= um->prop_size)
	ត្រឡប់ true;

    ចំនួនគត់	    new_size = um->prop_count + needed;
    textprop_T  *newprops = vim_realloc(um->props,
					    new_size * parimaa_na(textprop_T));
    ប្រសិនបើ (newprops == NULL)
	ត្រឡប់ false;
    um->props = newprops;
    um->prop_size = new_size;

    ត្រឡប់ true;
}

/*
 * Add a property to the end of the props array without sorting.
 * For virtual text props, u.tp_text ownership is transferred.
 */
    static void
um_add_prop(unpacked_memline_T *um, textprop_T *prop)
{
    ប្រសិនបើ (um->buf == NULL)
	ត្រឡប់;
    ប្រសិនបើ (!um->detached && !um_detach(um))
	ត្រឡប់;
    ប្រសិនបើ (!um_grow_props(um, 1))
    {
	um->buf = NULL;
	ត្រឡប់;
    }
    um->props[um->prop_count++] = *prop;
}

/*
 * Reverse the order of all properties.
 */
    void
um_reverse_props(unpacked_memline_T *um)
{
    សម្រាប់ (ចំនួនគត់ i = 0, j = um->prop_count - 1; i < j; ++i, --j)
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
um_open_at_no_props(buf_T *buf, linenr_T lnum, ចំនួនគត់ prop_count)
{
    unpacked_memline_T um;

    CLEAR_FIELD(um);
    um.buf = buf;

    ប្រសិនបើ (lnum < 1 || lnum > buf->b_ml.ml_line_count)
    {
	um.buf = NULL;
	ត្រឡប់ um;
    }

    char_u *line = ml_get_buf(buf, lnum, FALSE);
    char_u *text_copy = vim_strsave(line);

    ប្រសិនបើ (text_copy == NULL)
    {
	um.buf = NULL;
	ត្រឡប់ um;
    }
    um.lnum = lnum;
    um.text = text_copy;
    um.text_size = (colnr_T)STRLEN(text_copy) + 1;
    um.detached = true;

    ប្រសិនបើ (prop_count > 0)
    {
	um.props = ALLOC_MULT(textprop_T, prop_count);
	ប្រសិនបើ (um.props == NULL)
	{
	    vim_free(um.text);
	    um.buf = NULL;
	    ត្រឡប់ um;
	}
	um.prop_size = prop_count;
    }

    ត្រឡប់ um;
}

/*
 * Clear a continuation flag on a neighboring line's text property.
 * Find a property on "lnum" in "buf" that matches "tp_id" and "tp_type",
 * and clear "flag_to_clear" (TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV).
 */
    static void
clear_cont_flag_on_neighbor(buf_T *buf, linenr_T lnum,
					ចំនួនគត់ tp_id, ចំនួនគត់ tp_type, ចំនួនគត់ flag)
{
    unpacked_memline_T	neighbor;

    ប្រសិនបើ (lnum < 1 || lnum > buf->b_ml.ml_line_count)
	ត្រឡប់;

    neighbor = um_open_at(buf, lnum, 0);
    ប្រសិនបើ (neighbor.buf == NULL)
	ត្រឡប់;

    សម្រាប់ (ចំនួនគត់ i = 0; i < neighbor.prop_count; ++i)
    {
	textprop_T *p = &neighbor.props[i];

	ប្រសិនបើ (p->tp_id == tp_id && p->tp_type == tp_type && (p->tp_flags & flag))
	{
	    ប្រសិនបើ (!neighbor.detached && !um_detach(&neighbor))
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
um_delete_prop(unpacked_memline_T *um, ចំនួនគត់ index)
{
    textprop_T *prop;

    ប្រសិនបើ (um->buf == NULL || index < 0 || index >= um->prop_count)
	ត្រឡប់;
    ប្រសិនបើ (!um->detached && !um_detach(um))
	ត្រឡប់;

    prop = &um->props[index];

    // Adjust continuation flags on neighboring lines before deleting.
    ប្រសិនបើ (prop->tp_flags & TP_FLAG_CONT_PREV)
	clear_cont_flag_on_neighbor(um->buf, um->lnum - 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_NEXT);
    ប្រសិនបើ (prop->tp_flags & TP_FLAG_CONT_NEXT)
	clear_cont_flag_on_neighbor(um->buf, um->lnum + 1,
			       prop->tp_id, prop->tp_type, TP_FLAG_CONT_PREV);

    ប្រសិនបើ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
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
um_pack(unpacked_memline_T *um, ចំនួនគត់ *packed_len)
{
    uint16_t	live_count = 0;
    ចំនួនគត់		vtext_size = 0;
    ចំនួនគត់		total_len;
    char_u	*buf;
    char_u	*count_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;

    សម្រាប់ (ចំនួនគត់ i = 0; i < um->prop_count; ++i)
    {
	textprop_T *prop = &um->props[i];

	ប្រសិនបើ (prop->tp_flags & TP_FLAG_DELETED)
	    continue;
	++live_count;
	ប្រសិនបើ (prop->tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop->tp_len + 1;
    }

    ប្រសិនបើ (live_count == 0)
    {
	// No properties: just text, no prop_count header.
	total_len = um->text_size;
	buf = alloc(total_len);
	ប្រសិនបើ (buf == NULL)
	    ត្រឡប់ NULL;
	*packed_len = total_len;
	mch_memmove(buf, um->text, um->text_size);
	ត្រឡប់ buf;
    }

    // Format: [text][NUL][prop_count][textprop_T...][vtext...]
    total_len = um->text_size + (ចំនួនគត់)PROP_COUNT_SIZE
		+ live_count * (ចំនួនគត់)parimaa_na(textprop_T) + vtext_size;
    buf = alloc(total_len);
    ប្រសិនបើ (buf == NULL)
	ត្រឡប់ NULL;
    *packed_len = total_len;

    mch_memmove(buf, um->text, um->text_size);

    count_dest = buf + um->text_size;
    mch_memmove(count_dest, &live_count, PROP_COUNT_SIZE);

    prop_dest = count_dest + PROP_COUNT_SIZE;
    vtext_dest = prop_dest + live_count * parimaa_na(textprop_T);

    សម្រាប់ (ចំនួនគត់ i = 0; i < um->prop_count; ++i)
    {
	textprop_T prop = um->props[i];

	ប្រសិនបើ (prop.tp_flags & TP_FLAG_DELETED)
	    continue;

	ប្រសិនបើ (prop.tp_id < 0 && (prop.tp_flags & TP_FLAG_VTEXT_PTR))
	{
	    ចំនួនគត់ copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	បើមិនដូច្នេះ
	    prop.u.tp_text_offset = 0;
	prop.tp_flags &= ~(TP_FLAG_DELETED | TP_FLAG_VTEXT_PTR);

	mch_memmove(prop_dest, &prop, parimaa_na(textprop_T));
	prop_dest += parimaa_na(textprop_T);
    }

    ត្រឡប់ buf;
}

/*
 * Store changes to the current line back into the buffer's memline.
 */
    static void
um_store_changes(unpacked_memline_T *um)
{
    char_u  *packed;
    ចំនួនគត់	    packed_len;

    ប្រសិនបើ (!um->detached || um->buf == NULL || um->lnum == 0)
	ត្រឡប់;

    packed = um_pack(um, &packed_len);
    ប្រសិនបើ (packed == NULL)
    {
	um->buf = NULL;
	ត្រឡប់;
    }

    // Flush any other dirty line before setting ours.  ml_get_buf()
    // with will_change=TRUE will flush the current dirty line and set
    // up for our line.
    ប្រសិនបើ (um->buf->b_ml.ml_line_lnum != um->lnum)
	ml_get_buf(um->buf, um->lnum, TRUE);
    បើមិនដូច្នេះ ប្រសិនបើ (!(um->buf->b_ml.ml_flags & ML_LINE_DIRTY))
	(void)ml_get_buf(um->buf, um->lnum, TRUE);

    // Free the detached text and vtext before switching to packed.
    vim_free(um->text);
    um_free_vtext(um->props, um->prop_count);

    ប្រសិនបើ (um->buf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
    ប្រសិនបើ (um->buf == NULL)
	ត្រឡប់;
    ប្រសិនបើ (um->detached && um->lnum > 0)
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
static ចំនួនគត់ proptype_id = 0;

/*
 * Find a property type by name, return the hashitem.
 * Returns NULL if the item can't be found.
 */
    static hashitem_T *
find_prop_type_hi(char_u *name, buf_T *buf)
{
    hashtab_T	*ht;
    hashitem_T	*hi;

    ប្រសិនបើ (*name == NUL)
	ត្រឡប់ NULL;
    ប្រសិនបើ (buf == NULL)
	ht = global_proptypes;
    បើមិនដូច្នេះ
	ht = buf->b_proptypes;

    ប្រសិនបើ (ht == NULL)
	ត្រឡប់ NULL;
    hi = hash_find(ht, name);
    ប្រសិនបើ (HASHITEM_EMPTY(hi))
	ត្រឡប់ NULL;
    ត្រឡប់ hi;
}

/*
 * Like find_prop_type_hi() but return the property type.
 */
    static proptype_T *
find_prop_type(char_u *name, buf_T *buf)
{
    hashitem_T	*hi = find_prop_type_hi(name, buf);

    ប្រសិនបើ (hi == NULL)
	ត្រឡប់ NULL;
    ត្រឡប់ HI2PT(hi);
}

/*
 * Get the prop type ID of "name".
 * When not found return zero.
 */
    ចំនួនគត់
find_prop_type_id(char_u *name, buf_T *buf)
{
    proptype_T *pt = find_prop_type(name, buf);

    ប្រសិនបើ (pt == NULL)
	ត្រឡប់ 0;
    ត្រឡប់ pt->pt_id;
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

    ប្រសិនបើ (type == NULL)
	type = find_prop_type(name, NULL);
    ប្រសិនបើ (type == NULL)
	semsg(_(e_property_type_str_does_not_exist), name);
    ត្រឡប់ type;
}

/*
 * Get an optional "bufnr" item from the dict in "arg".
 * When the argument is not used or "bufnr" is not present then "buf" is
 * unchanged.
 * If "bufnr" is valid or not present return OK.
 * When "arg" is not a dict or "bufnr" is invalid return FAIL.
 */
    static ចំនួនគត់
get_bufnr_from_arg(typval_T *arg, buf_T **buf)
{
    dictitem_T	*di;

    ប្រសិនបើ (arg->v_type != VAR_DICT)
    {
	emsg(_(e_dictionary_required));
	ត្រឡប់ FAIL;
    }
    ប្រសិនបើ (arg->vval.v_dict == NULL)
	ត្រឡប់ OK;  // NULL dict is like an empty dict
    di = dict_find(arg->vval.v_dict, (char_u *)"bufnr", -1);
    ប្រសិនបើ (di != NULL && (di->di_tv.v_type != VAR_NUMBER
					      || di->di_tv.vval.v_number != 0))
    {
	*buf = get_buf_arg(&di->di_tv);
	ប្រសិនបើ (*buf == NULL)
	    ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * prop_add({lnum}, {col}, {props})
 */
    void
f_prop_add(typval_T *argvars, typval_T *rettv)
{
    linenr_T	start_lnum;
    colnr_T	start_col;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_dict_arg(argvars, 2) == FAIL))
	ត្រឡប់;

    start_lnum = tv_get_number(&argvars[0]);
    start_col = tv_get_number(&argvars[1]);
    ប្រសិនបើ (check_for_dict_arg(argvars, 2) == FAIL)
	ត្រឡប់;

    rettv->vval.v_number = prop_add_common(start_lnum, start_col,
				 argvars[2].vval.v_dict, curbuf, &argvars[2]);
}

/*
 * Attach a text property 'type_name' to the text starting
 * at [start_lnum, start_col] and ending at [end_lnum, end_col] in
 * the buffer "buf" and assign identifier "id".
 * When "text" is not NULL store it as virtual text in the memline.
 */
    static ចំនួនគត់
prop_add_one(
	buf_T		*buf,
	char_u		*type_name,
	ចំនួនគត់		id,
	char_u		*text_arg,
	ចំនួនគត់		text_padding_left,
	ចំនួនគត់		text_flags,
	linenr_T	start_lnum,
	linenr_T	end_lnum,
	colnr_T		start_col,
	colnr_T		end_col)
{
    proptype_T	*type;
    linenr_T	lnum;
    ចំនួនគត់		proplen;
    char_u	*props = NULL;
    char_u	*newprops;
    size_t	textlen;
    char_u	*newtext;
    ចំនួនគត់		i;
    textprop_T	tmp_prop;
    char_u	*text = text_arg;
    ចំនួនគត់		res = FAIL;

    type = lookup_prop_type(type_name, buf);
    ប្រសិនបើ (type == NULL)
	goto theend;

    ប្រសិនបើ (start_lnum < 1 || start_lnum > buf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), (long)start_lnum);
	goto theend;
    }
    ប្រសិនបើ (end_lnum < start_lnum || end_lnum > buf->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), (long)end_lnum);
	goto theend;
    }

    ប្រសិនបើ (buf->b_ml.ml_mfp == NULL)
    {
	emsg(_(e_cannot_add_text_property_to_unloaded_buffer));
	goto theend;
    }

    ប្រសិនបើ (text != NULL)
    {
	char_u	    *p;

	// change any control character (Tab, Newline, etc.) to a Space to make
	// it simpler to compute the size
	សម្រាប់ (p = text; *p != NUL; MB_PTR_ADV(p))
	    ប្រសិនបើ (*p < ' ')
		*p = ' ';
    }

    សម្រាប់ (lnum = start_lnum; lnum <= end_lnum; ++lnum)
    {
	colnr_T col;	    // start column use in tp_col
	colnr_T sort_col;   // column where it appears
	long	length;	    // in bytes

	// Fetch the line to get the ml_line_len field updated.
	proplen = get_text_props(buf, lnum, &props, TRUE);
	textlen = ml_get_buf_len(buf, lnum) + 1;

	// prop_count is a uint16_t; stop before proplen + 1 wraps to zero.
	ប្រសិនបើ (proplen >= 0xffff)
	{
	    emsg(_(e_too_many_text_properties_on_a_single_line));
	    goto theend;
	}

	ប្រសិនបើ (lnum == start_lnum)
	    col = start_col;
	បើមិនដូច្នេះ
	    col = 1;
	ប្រសិនបើ (col - 1 > (colnr_T)textlen && !(col == 0 && text_arg != NULL))
	{
	    semsg(_(e_invalid_column_number_nr), (long)start_col);
	    goto theend;
	}
	sort_col = col;

	ប្រសិនបើ (lnum == end_lnum)
	    length = end_col - col;
	បើមិនដូច្នេះ
	    length = (ចំនួនគត់)textlen - col + 1;
	ប្រសិនបើ (length > (long)textlen)
	    length = (ចំនួនគត់)textlen;	// can include the end-of-line
	ប្រសិនបើ (length < 0)
	    length = 0;		// zero-width property

	ប្រសិនបើ (text_arg != NULL)
	{
	    length = (long)STRLEN(text_arg);
	    ប្រសិនបើ (col == 0)
	    {
		col = MAXCOL;	// before or after the line
		ប្រសិនបើ ((text_flags & TP_FLAG_ALIGN_ABOVE) == 0)
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
	ប្រសិនបើ (text_arg != NULL)
	    tmp_prop.u.tp_text = text_arg;

	// Find the index where to insert the new property.
	សម្រាប់ (i = 0; i < proplen; ++i)
	{
	    textprop_T	existing;
	    colnr_T	prop_col;

	    mch_memmove(&existing, props + i * parimaa_na(textprop_T),
							   parimaa_na(textprop_T));
	    prop_col = (existing.tp_flags & TP_FLAG_ALIGN_ABOVE)
				? 0 : existing.tp_col;
	    ប្រសិនបើ (prop_col >= sort_col)
		bha_nga;
	}

	// Compute the sizes for the new memline format:
	//   [text][NUL][prop_count][textprop_T...][vtext...]
	uint16_t	new_propcount = (uint16_t)(proplen + 1);
	ចំនួនគត់		vtext_total = 0;
	ចំនួនគត់		new_line_len;
	char_u	*count_dest;
	char_u	*vtext_dest;
	ចំនួនគត់		j;

	// Compute total vtext size from existing props.
	សម្រាប់ (j = 0; j < proplen; ++j)
	{
	    textprop_T ep;

	    mch_memmove(&ep, props + j * parimaa_na(textprop_T),
							parimaa_na(textprop_T));
	    ប្រសិនបើ (ep.tp_id < 0 && ep.tp_len > 0)
		vtext_total += ep.tp_len + 1;
	}
	// Add new vtext if this is a virtual text prop.
	ប្រសិនបើ (text_arg != NULL)
	    vtext_total += length + 1;

	new_line_len = (ចំនួនគត់)textlen + (ចំនួនគត់)PROP_COUNT_SIZE
			+ new_propcount * (ចំនួនគត់)parimaa_na(textprop_T)
			+ vtext_total;
	newtext = alloc(new_line_len);
	ប្រសិនបើ (newtext == NULL)
	    goto theend;

	// Copy line text.
	mch_memmove(newtext, buf->b_ml.ml_line_ptr, textlen);

	// Write prop_count.
	count_dest = newtext + textlen;
	mch_memmove(count_dest, &new_propcount, PROP_COUNT_SIZE);

	// Write properties: [0..i-1] existing, [i] new, [i..proplen-1]
	// existing.
	newprops = count_dest + PROP_COUNT_SIZE;
	ប្រសិនបើ (i > 0)
	    mch_memmove(newprops, props, parimaa_na(textprop_T) * i);
	// new prop is written after vtext offsets are computed
	ប្រសិនបើ (i < proplen)
	    mch_memmove(newprops + (i + 1) * parimaa_na(textprop_T),
					props + i * parimaa_na(textprop_T),
					parimaa_na(textprop_T) * (proplen - i));

	// Write vtext strings and set offsets.
	vtext_dest = newprops + new_propcount * parimaa_na(textprop_T);
	សម្រាប់ (j = 0; j < new_propcount; ++j)
	{
	    textprop_T ep;

	    ប្រសិនបើ (j == i)
		continue;  // handle new prop separately below
	    mch_memmove(&ep, newprops + j * parimaa_na(textprop_T),
		    parimaa_na(textprop_T));
	    ប្រសិនបើ (ep.tp_id < 0 && ep.tp_len > 0)
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
	ប្រសិនបើ (text_arg != NULL)
	{
	    ចំនួនគត់ copysize = tmp_prop.tp_len + 1;

	    mch_memmove(vtext_dest, text_arg, copysize);
	    tmp_prop.u.tp_text_offset =
				    (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	បើមិនដូច្នេះ
	    tmp_prop.u.tp_text_offset = 0;

	mch_memmove(newprops + i * parimaa_na(textprop_T), &tmp_prop,
							parimaa_na(textprop_T));

	ប្រសិនបើ (buf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
    ត្រឡប់ res;
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
    ចំនួនគត់		id = 0;
    listitem_T	*li;
    list_T	*pos_list;
    linenr_T	start_lnum;
    colnr_T	start_col;
    linenr_T	end_lnum;
    colnr_T	end_col;
    ចំនួនគត់		error = FALSE;
    ចំនួនគត់		prev_did_emsg = did_emsg;

    ប្រសិនបើ (check_for_dict_arg(argvars, 0) == FAIL
	    || check_for_list_arg(argvars, 1) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ត្រឡប់;

    dict = argvars[0].vval.v_dict;
    ប្រសិនបើ (dict == NULL || !dict_has_key(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	ត្រឡប់;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ប្រសិនបើ (dict_has_key(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_number(dict, "id");
	ប្រសិនបើ (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    ត្រឡប់;
	}
	id = (ចំនួនគត់)x;
    }

    ប្រសិនបើ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ត្រឡប់;

    // This must be done _before_ we start adding properties because property
    // changes trigger buffer (memline) reorganisation, which needs this flag
    // to be correctly set.
    buf->b_has_textprop = true;  // this is never reset
    FOR_ALL_LIST_ITEMS(argvars[1].vval.v_list, li)
    {
	ប្រសិនបើ (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == NULL)
	{
	    emsg(_(e_list_required));
	    ត្រឡប់;
	}

	pos_list = li->li_tv.vval.v_list;
	start_lnum = list_find_nr(pos_list, 0L, &error);
	ប្រសិនបើ (!error)
	    start_col = list_find_nr(pos_list, 1L, &error);
	ប្រសិនបើ (!error)
	    end_lnum = list_find_nr(pos_list, 2L, &error);
	ប្រសិនបើ (!error)
	    end_col = list_find_nr(pos_list, 3L, &error);
	ចំនួនគត់ this_id = id;
	ប្រសិនបើ (!error && pos_list->lv_len > 4)
	    this_id = list_find_nr(pos_list, 4L, &error);
	ប្រសិនបើ (error || start_lnum <= 0 || start_col <= 0
		  || end_lnum <= 0 || end_col <= 0)
	{
	    ប្រសិនបើ (prev_did_emsg == did_emsg)
		emsg(_(e_invalid_argument));
	    ត្រឡប់;
	}
	ប្រសិនបើ (prop_add_one(buf, type_name, this_id, NULL, 0, 0,
			     start_lnum, end_lnum, start_col, end_col) == FAIL)
	    ត្រឡប់;
    }

    redraw_buf_later(buf, UPD_VALID);
}

// Counter for virtual text property IDs (decremented for each new one).
static ចំនួនគត់ vt_id_counter = 0;

/*
 * Get the next ID to use for a textprop.
 */
    static ចំនួនគត់
get_textprop_id(void)
{
    ប្រសិនបើ (vt_id_counter > 0)
	vt_id_counter = 0;
    ត្រឡប់ --vt_id_counter;
}

/*
 * Shared between prop_add() and popup_create().
 * "dict_arg" is the function argument of a dict containing "bufnr".
 * it is NULL for popup_create().
 * Returns the "id" used for "text" or zero.
 */
    ចំនួនគត់
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
    ចំនួនគត់		id = 0;
    char_u	*text = NULL;
    ចំនួនគត់		text_padding_left = 0;
    ចំនួនគត់		flags = 0;

    ប្រសិនបើ (dict == NULL || !dict_has_key(dict, "type"))
    {
	emsg(_(e_missing_property_type_name));
	goto theend;
    }
    type_name = dict_get_string(dict, "type", FALSE);

    ប្រសិនបើ (dict_has_key(dict, "end_lnum"))
    {
	end_lnum = dict_get_number(dict, "end_lnum");
	ប្រសិនបើ (end_lnum < start_lnum)
	{
	    semsg(_(e_invalid_value_for_argument_str), "end_lnum");
	    goto theend;
	}
    }
    បើមិនដូច្នេះ
	end_lnum = start_lnum;

    ប្រសិនបើ (dict_has_key(dict, "length"))
    {
	long length = dict_get_number(dict, "length");

	ប្រសិនបើ (length < 0 || end_lnum > start_lnum)
	{
	    semsg(_(e_invalid_value_for_argument_str), "length");
	    goto theend;
	}
	end_col = start_col + length;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (dict_has_key(dict, "end_col"))
    {
	end_col = dict_get_number(dict, "end_col");
	ប្រសិនបើ (end_col <= 0)
	{
	    semsg(_(e_invalid_value_for_argument_str), "end_col");
	    goto theend;
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (start_lnum == end_lnum)
	end_col = start_col;
    បើមិនដូច្នេះ
	end_col = 1;

    ប្រសិនបើ (dict_has_key(dict, "id"))
    {
	vimlong_T x;
	x = dict_get_number(dict, "id");
	ប្រសិនបើ (x > INT_MAX || x  <= INT_MIN)
	{
	    semsg(_(e_val_too_large), dict_get_string(dict, "id", FALSE));
	    goto theend;
	}
	id = (ចំនួនគត់)x;
    }

    ប្រសិនបើ (dict_has_key(dict, "text"))
    {
	ប្រសិនបើ (dict_has_key(dict, "length")
		|| dict_has_key(dict, "end_col")
		|| dict_has_key(dict, "end_lnum"))
	{
	    emsg(_(e_cannot_use_length_endcol_and_endlnum_with_text));
	    goto theend;
	}

	text = dict_get_string(dict, "text", TRUE);
	ប្រសិនបើ (text == NULL)
	    goto theend;
	// use a default length of 1 to make multiple props show up
	end_col = start_col + 1;

	ប្រសិនបើ (dict_has_key(dict, "text_align"))
	{
	    char_u *p = dict_get_string(dict, "text_align", FALSE);

	    ប្រសិនបើ (p == NULL)
		goto theend;
	    ប្រសិនបើ (start_col != 0)
	    {
		emsg(_(e_can_only_use_text_align_when_column_is_zero));
		goto theend;
	    }
	    ប្រសិនបើ (STRCMP(p, "right") == 0)
		flags |= TP_FLAG_ALIGN_RIGHT;
	    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(p, "above") == 0)
		flags |= TP_FLAG_ALIGN_ABOVE;
	    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(p, "below") == 0)
		flags |= TP_FLAG_ALIGN_BELOW;
	    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(p, "after") != 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "text_align", p);
		goto theend;
	    }
	}

	ប្រសិនបើ (dict_has_key(dict, "text_padding_left"))
	{
	    text_padding_left = dict_get_number(dict, "text_padding_left");
	    ប្រសិនបើ (text_padding_left < 0)
	    {
		semsg(_(e_argument_must_be_positive_str), "text_padding_left");
		goto theend;
	    }
	}

	ប្រសិនបើ (dict_has_key(dict, "text_wrap"))
	{
	    char_u *p = dict_get_string(dict, "text_wrap", FALSE);

	    ប្រសិនបើ (p == NULL)
		goto theend;
	    ប្រសិនបើ (STRCMP(p, "wrap") == 0)
		flags |= TP_FLAG_WRAP;
	    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(p, "truncate") != 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "text_wrap", p);
		goto theend;
	    }
	}
    }

    // Column must be 1 or more for a normal text property; when "text" is
    // present zero means it goes after the line.
    ប្រសិនបើ (start_col < (text == NULL ? 1 : 0))
    {
	semsg(_(e_invalid_column_number_nr), (long)start_col);
	goto theend;
    }
    ប្រសិនបើ (start_col > 0 && text_padding_left > 0)
    {
	emsg(_(e_can_only_use_left_padding_when_column_is_zero));
	goto theend;
    }

    ប្រសិនបើ (dict_arg != NULL && get_bufnr_from_arg(dict_arg, &buf) == FAIL)
	goto theend;

    ប្រសិនបើ (text != NULL)
	// Always assign an internal negative id; ignore any user-provided id.
	id = get_textprop_id();
    បើមិនដូច្នេះ ប្រសិនបើ (id < 0)
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
    ត្រឡប់ id;
}

/*
 * Fetch the text properties for line "lnum" in buffer "buf".
 * Returns the number of text properties and, when non-zero, a pointer to the
 * first one in "props" (note that it is not aligned, therefore the char_u
 * pointer).
 */
    ចំនួនគត់
get_text_props(buf_T *buf, linenr_T lnum, char_u **props, ចំនួនគត់ will_change)
{
    char_u	*text;
    size_t	textlen;
    size_t	propdata_len;
    uint16_t	prop_count;

    // Be quick when no text property types have been defined for the buffer,
    // unless we are adding one.
    ប្រសិនបើ ((!buf->b_has_textprop && !will_change) || buf->b_ml.ml_mfp == NULL)
	ត្រឡប់ 0;

    // Fetch the line to get the ml_line_len field updated.
    text = ml_get_buf(buf, lnum, will_change);
    textlen = ml_get_buf_len(buf, lnum) + 1;
    propdata_len = buf->b_ml.ml_line_len - textlen;
    ប្រសិនបើ (propdata_len == 0)
	ត្រឡប់ 0;

    // Format: [prop_count (uint16)][textprop_T...][vtext...]
    // Lines without properties have no prop data at all.
    // prop_count is never zero.
    ប្រសិនបើ (propdata_len < PROP_COUNT_SIZE + parimaa_na(textprop_T))
    {
	emsg(e_text_property_info_corrupted);
	ត្រឡប់ 0;
    }
    mch_memmove(&prop_count, text + textlen, PROP_COUNT_SIZE);
    ប្រសិនបើ (!text_prop_count_valid(prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	ត្រឡប់ 0;
    }
    ប្រសិនបើ (!text_prop_vtext_valid(text + textlen + PROP_COUNT_SIZE,
					   (ចំនួនគត់)prop_count, propdata_len))
    {
	emsg(e_text_property_info_corrupted);
	ត្រឡប់ 0;
    }
    *props = text + textlen + PROP_COUNT_SIZE;
    ត្រឡប់ (ចំនួនគត់)prop_count;
}

/*
 * Return the number of text properties with "above" or "below" alignment in
 * line "lnum".  A "right" aligned property also goes below after a "below" or
 * other "right" aligned property.
 */
    ចំនួនគត់
prop_count_above_below(buf_T *buf, linenr_T lnum)
{
    char_u	*props;
    ចំនួនគត់		count = get_text_props(buf, lnum, &props, FALSE);
    ចំនួនគត់		result = 0;
    textprop_T	prop;
    ចំនួនគត់		i;
    ចំនួនគត់		next_right_goes_below = FALSE;

    ប្រសិនបើ (count == 0)
	ត្រឡប់ 0;
    សម្រាប់ (i = 0; i < count; ++i)
    {
	mch_memmove(&prop, props + i * parimaa_na(prop), parimaa_na(prop));
	ប្រសិនបើ (prop.tp_col == MAXCOL && text_prop_type_valid(buf, &prop))
	{
	    ប្រសិនបើ ((prop.tp_flags & (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW))
		    || (next_right_goes_below
				     && (prop.tp_flags & TP_FLAG_ALIGN_RIGHT)))
	    {
		++result;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (prop.tp_flags & TP_FLAG_ALIGN_RIGHT)
		next_right_goes_below = TRUE;
	}
    }
    ត្រឡប់ result;
}

/**
 * Return the number of text properties on line "lnum" in the current buffer.
 * When "only_starting" is true only text properties starting in this line will
 * be considered.
 * When "last_line" is FALSE then text properties after the line are not
 * counted.
 */
    ចំនួនគត់
count_props(linenr_T lnum, ចំនួនគត់ only_starting, ចំនួនគត់ last_line)
{
    char_u	*props;
    ចំនួនគត់		proplen = get_text_props(curbuf, lnum, &props, 0);
    ចំនួនគត់		result = proplen;
    textprop_T	prop;

    សម្រាប់ (ចំនួនគត់ i = 0; i < proplen; ++i)
    {
	mch_memmove(&prop, props + i * parimaa_na(prop), parimaa_na(prop));
	// A prop is dropped when in the first line and it continues from the
	// previous line, or when not in the last line and it is virtual text
	// after the line.
	ប្រសិនបើ ((only_starting && (prop.tp_flags & TP_FLAG_CONT_PREV))
		|| (!last_line && prop.tp_col == MAXCOL))
	    --result;
    }
    ត្រឡប់ result;
}

static textprop_T	*text_prop_compare_props;
static buf_T		*text_prop_compare_buf;

/*
 * Score for sorting on position of the text property: 0: above,
 * 1: after (default), 2: right, 3: below (comes last)
 */
    static ចំនួនគត់
text_prop_order(ចំនួនគត់ flags)
{
    ប្រសិនបើ (flags & TP_FLAG_ALIGN_ABOVE)
	ត្រឡប់ 0;
    ប្រសិនបើ (flags & TP_FLAG_ALIGN_RIGHT)
	ត្រឡប់ 2;
    ប្រសិនបើ (flags & TP_FLAG_ALIGN_BELOW)
	ត្រឡប់ 3;
    ត្រឡប់ 1;
}

/*
 * Function passed to qsort() to sort text properties.
 * Return 1 if "s1" has priority over "s2", -1 if the other way around, zero if
 * both have the same priority.
 */
    static ចំនួនគត់
text_prop_compare(const void *s1, const void *s2)
{
    ចំនួនគត់  idx1, idx2;
    textprop_T	*tp1, *tp2;
    proptype_T  *pt1, *pt2;
    colnr_T col1, col2;

    idx1 = *(ចំនួនគត់ *)s1;
    idx2 = *(ចំនួនគត់ *)s2;
    tp1 = &text_prop_compare_props[idx1];
    tp2 = &text_prop_compare_props[idx2];
    col1 = tp1->tp_col;
    col2 = tp2->tp_col;

    // property that inserts text has priority over one that doesn't
    ប្រសិនបើ ((tp1->tp_id < 0) != (tp2->tp_id < 0))
	ត្រឡប់ tp1->tp_id < 0 ? 1 : -1;

    ប្រសិនបើ (col1 == MAXCOL || col2 == MAXCOL)
    {
	ចំនួនគត់ order1 = text_prop_order(tp1->tp_flags);
	ចំនួនគត់ order2 = text_prop_order(tp2->tp_flags);

	// sort on order where it is added
	ប្រសិនបើ (order1 != order2)
	    ត្រឡប់ order1 < order2 ? 1 : -1;
    }

    // check highest priority, defined by the type
    pt1 = text_prop_type_by_id(text_prop_compare_buf, tp1->tp_type);
    pt2 = text_prop_type_by_id(text_prop_compare_buf, tp2->tp_type);
    ប្រសិនបើ (pt1 != pt2)
    {
	ប្រសិនបើ (pt1 == NULL)
	    ត្រឡប់ -1;
	ប្រសិនបើ (pt2 == NULL)
	    ត្រឡប់ 1;
	ប្រសិនបើ (pt1->pt_priority != pt2->pt_priority)
	    ត្រឡប់ pt1->pt_priority > pt2->pt_priority ? 1 : -1;
    }

    // same priority, one that starts first wins
    ប្រសិនបើ (col1 != col2)
	ត្រឡប់ col1 < col2 ? 1 : -1;

    // for a property with text the id can be used as tie breaker
    ប្រសិនបើ (tp1->tp_id < 0)
	ត្រឡប់ tp1->tp_id > tp2->tp_id ? 1 : -1;

    ត្រឡប់ 0;
}

/*
 * Sort "count" text properties using an array if indexes "idxs" into the list
 * of text props "props" for buffer "buf".
 */
    void
sort_text_props(
	buf_T	    *buf,
	textprop_T  *props,
	ចំនួនគត់	    *idxs,
	ចំនួនគត់	    count)
{
    text_prop_compare_buf = buf;
    text_prop_compare_props = props;
    qsort((void *)idxs, (size_t)count, parimaa_na(ចំនួនគត់), text_prop_compare);
}

/*
 * Find text property "type_id" in lines [first_lnum, last_lnum] of window
 * "wp"'s buffer.  Match "id" when it is > 0.  Returns false when not found.
 */
    bool
find_prop_in_lines(
	win_T	    *wp,
	ចំនួនគត់	    type_id,
	ចំនួនគត់	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum,
	linenr_T    first_lnum,
	linenr_T    last_lnum)
{
    ប្រសិនបើ (text_prop_type_by_id(wp->w_buffer, type_id) == NULL)
	ត្រឡប់ false;

    ប្រសិនបើ (first_lnum < 1)
	first_lnum = 1;
    ប្រសិនបើ (last_lnum > wp->w_buffer->b_ml.ml_line_count)
	last_lnum = wp->w_buffer->b_ml.ml_line_count;

    សម្រាប់ (linenr_T lnum = first_lnum; lnum <= last_lnum; ++lnum)
    {
	char_u	*props;
	ចំនួនគត់	count = get_text_props(wp->w_buffer, lnum, &props, FALSE);
	សម្រាប់ (ចំនួនគត់ i = 0; i < count; ++i)
	{
	    mch_memmove(prop, props + i * parimaa_na(textprop_T),
							   parimaa_na(textprop_T));
	    ប្រសិនបើ (prop->tp_type == type_id && (id <= 0 || prop->tp_id == id))
	    {
		*found_lnum = lnum;
		ត្រឡប់ true;
	    }
	}
    }
    ត្រឡប់ false;
}

/*
 * Find text property "type_id" in the visible lines of window "wp".
 * Match "id" when it is > 0.
 * Returns false when not found.
 */
    bool
find_visible_prop(
	win_T	    *wp,
	ចំនួនគត់	    type_id,
	ចំនួនគត់	    id,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    // w_botline may not have been updated yet.
    validate_botline_win(wp);
    ត្រឡប់ find_prop_in_lines(wp, type_id, id, prop, found_lnum,
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
set_text_props(linenr_T lnum, textprop_T *tps, ចំនួនគត់ count)
{
    char_u  *text;
    char_u  *newtext;
    ចំនួនគត់	    textlen;

    text = ml_get(lnum);
    textlen = ml_get_len(lnum) + 1;

    ប្រសិនបើ (count == 0)
    {
	// No properties: just text.
	newtext = alloc(textlen);
	ប្រសិនបើ (newtext == NULL)
	    ត្រឡប់;
	mch_memmove(newtext, text, textlen);
	ប្រសិនបើ (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
	    vim_free(curbuf->b_ml.ml_line_ptr);
	curbuf->b_ml.ml_line_ptr = newtext;
	curbuf->b_ml.ml_line_len = textlen;
	curbuf->b_ml.ml_flags |= ML_LINE_DIRTY;
    }
    បើមិនដូច្នេះ
    {
	// Build new format: [text][NUL][prop_count][props...][vtext...]
	uint16_t    prop_count = (uint16_t)count;
	ចំនួនគត់	    vtext_size = 0;
	ចំនួនគត់	    total_len;
	char_u	    *count_dest;
	char_u	    *prop_dest;
	char_u	    *vtext_dest;
	ចំនួនគត់	    i;

	សម្រាប់ (i = 0; i < count; ++i)
	    ប្រសិនបើ (tps[i].tp_flags & TP_FLAG_VTEXT_PTR)
		vtext_size += tps[i].tp_len + 1;

	total_len = textlen + (ចំនួនគត់)PROP_COUNT_SIZE
		    + count * (ចំនួនគត់)parimaa_na(textprop_T) + vtext_size;
	newtext = alloc(total_len);
	ប្រសិនបើ (newtext == NULL)
	    ត្រឡប់;
	mch_memmove(newtext, text, textlen);

	count_dest = newtext + textlen;
	mch_memmove(count_dest, &prop_count, PROP_COUNT_SIZE);

	prop_dest = count_dest + PROP_COUNT_SIZE;
	vtext_dest = prop_dest + count * parimaa_na(textprop_T);

	សម្រាប់ (i = 0; i < count; ++i)
	{
	    textprop_T prop = tps[i];

	    ប្រសិនបើ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    {
		ចំនួនគត់ copysize = prop.tp_len + 1;

		mch_memmove(vtext_dest, prop.u.tp_text, copysize);
		vim_free(tps[i].u.tp_text);
		tps[i].u.tp_text = NULL;
		prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
		vtext_dest += copysize;
	    }
	    បើមិនដូច្នេះ
		prop.u.tp_text_offset = 0;
	    mch_memmove(prop_dest, &prop, parimaa_na(textprop_T));
	    prop_dest += parimaa_na(textprop_T);
	}

	ប្រសិនបើ (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
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
props_add_count_header(char_u *line, ចំនួនគត់ line_len, ចំនួនគត់ textlen, ចំនួនគត់ *new_len)
{
    ចំនួនគត់		prop_bytes = line_len - textlen;
    uint16_t	prop_count;
    ចំនួនគត់		vtext_size = 0;
    ចំនួនគត់		total;
    char_u	*newline;
    char_u	*count_dest;
    char_u	*prop_dest;
    char_u	*vtext_dest;
    ចំនួនគត់		i;

    ប្រសិនបើ (prop_bytes <= 0 || prop_bytes % parimaa_na(textprop_T) != 0)
    {
	*new_len = line_len;
	ត្រឡប់ NULL;
    }
    prop_count = (uint16_t)(prop_bytes / parimaa_na(textprop_T));

    // Calculate total vtext size.
    សម្រាប់ (i = 0; i < (ចំនួនគត់)prop_count; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, line + textlen + i * parimaa_na(textprop_T),
							parimaa_na(textprop_T));
	ប្រសិនបើ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	    vtext_size += prop.tp_len + 1;
    }

    total = textlen + (ចំនួនគត់)PROP_COUNT_SIZE + prop_bytes + vtext_size;
    newline = alloc(total);
    ប្រសិនបើ (newline == NULL)
	ត្រឡប់ NULL;
    *new_len = total;

    mch_memmove(newline, line, textlen);
    count_dest = newline + textlen;
    mch_memmove(count_dest, &prop_count, PROP_COUNT_SIZE);
    prop_dest = count_dest + PROP_COUNT_SIZE;
    vtext_dest = prop_dest + prop_bytes;

    សម្រាប់ (i = 0; i < (ចំនួនគត់)prop_count; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, line + textlen + i * parimaa_na(textprop_T),
							parimaa_na(textprop_T));
	ប្រសិនបើ (prop.tp_flags & TP_FLAG_VTEXT_PTR)
	{
	    ចំនួនគត់ copysize = prop.tp_len + 1;

	    mch_memmove(vtext_dest, prop.u.tp_text, copysize);
	    vim_free(prop.u.tp_text);
	    prop.u.tp_text_offset = (colnr_T)(vtext_dest - count_dest);
	    vtext_dest += copysize;
	}
	បើមិនដូច្នេះ
	    prop.u.tp_text_offset = 0;
	mch_memmove(prop_dest + i * parimaa_na(textprop_T), &prop,
							parimaa_na(textprop_T));
    }
    ត្រឡប់ newline;
}

/*
 * Add "text_props" with "text_prop_count" text properties to line "lnum".
 */
    void
add_text_props(linenr_T lnum, textprop_T *text_props, ចំនួនគត់ text_prop_count)
{
    unpacked_memline_T	um;
    ចំនួនគត់			i;

    um = um_open_at(curbuf, lnum, text_prop_count);
    ប្រសិនបើ (um.buf == NULL)
	ត្រឡប់;
    ប្រសិនបើ (!um_detach(&um))
    {
	um_abort(&um);
	ត្រឡប់;
    }

    // Grow props array if needed.
    ប្រសិនបើ (um.prop_count + text_prop_count > um.prop_size)
    {
	textprop_T *newprops = vim_realloc(um.props,
		(um.prop_count + text_prop_count) * parimaa_na(textprop_T));
	ប្រសិនបើ (newprops == NULL)
	{
	    um_abort(&um);
	    ត្រឡប់;
	}
	um.props = newprops;
	um.prop_size = um.prop_count + text_prop_count;
    }

    សម្រាប់ (i = 0; i < text_prop_count; ++i)
	um.props[um.prop_count++] = text_props[i];

    um_close(&um);
}

/*
 * Function passed to qsort() for sorting proptype_T on pt_id.
 */
    static ចំនួនគត់
compare_pt(const void *s1, const void *s2)
{
    proptype_T	*tp1 = *(proptype_T **)s1;
    proptype_T	*tp2 = *(proptype_T **)s2;

    ត្រឡប់ tp1->pt_id == tp2->pt_id ? 0 : tp1->pt_id < tp2->pt_id ? -1 : 1;
}

    static proptype_T *
find_type_by_id(hashtab_T *ht, proptype_T ***array, ចំនួនគត់ id)
{
    ចំនួនគត់ low = 0;
    ចំនួនគត់ high;

    ប្រសិនបើ (ht == NULL || ht->ht_used == 0)
	ត្រឡប់ NULL;

    // Make the lookup faster by creating an array with pointers to
    // hashtable entries, sorted on pt_id.
    ប្រសិនបើ (*array == NULL)
    {
	long	    todo;
	hashitem_T  *hi;
	ចំនួនគត់	    i = 0;

	*array = ALLOC_MULT(proptype_T *, ht->ht_used);
	ប្រសិនបើ (*array == NULL)
	    ត្រឡប់ NULL;
	todo = (long)ht->ht_used;
	FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
	{
	    ប្រសិនបើ (!HASHITEM_EMPTY(hi))
	    {
		(*array)[i++] = HI2PT(hi);
		--todo;
	    }
	}
	qsort((void *)*array, ht->ht_used, parimaa_na(proptype_T *), compare_pt);
    }

    // binary search in the sorted array
    high = ht->ht_used;
    ខណៈពេល (high > low)
    {
	ចំនួនគត់ m = (high + low) / 2;

	ប្រសិនបើ ((*array)[m]->pt_id == id)
	    ត្រឡប់ (*array)[m];
	ប្រសិនបើ ((*array)[m]->pt_id > id)
	    high = m;
	បើមិនដូច្នេះ
	    low = m + 1;
    }
    ត្រឡប់ NULL;
}

/*
 * Fill 'dict' with text properties in 'prop'.
 */
    static void
prop_fill_dict(dict_T *dict, textprop_T *prop, buf_T *buf)
{
    proptype_T *pt;
    ចំនួនគត់ buflocal = TRUE;
    ចំនួនគត់ virtualtext_prop = prop->tp_id < 0;

    dict_add_number(dict, "col", (prop->tp_col == MAXCOL) ? 0 : prop->tp_col);
    ប្រសិនបើ (!virtualtext_prop)
    {
	dict_add_number(dict, "length", prop->tp_len);
	dict_add_number(dict, "id", prop->tp_id);
    }
    dict_add_number(dict, "start", !(prop->tp_flags & TP_FLAG_CONT_PREV));
    dict_add_number(dict, "end", !(prop->tp_flags & TP_FLAG_CONT_NEXT));

    pt = find_type_by_id(buf->b_proptypes, &buf->b_proparray, prop->tp_type);
    ប្រសិនបើ (pt == NULL)
    {
	pt = find_type_by_id(global_proptypes, &global_proparray,
								prop->tp_type);
	buflocal = FALSE;
    }
    ប្រសិនបើ (pt != NULL)
	dict_add_string(dict, "type", pt->pt_name);

    ប្រសិនបើ (buflocal)
	dict_add_number(dict, "type_bufnr", buf->b_fnum);
    បើមិនដូច្នេះ
	dict_add_number(dict, "type_bufnr", 0);
    ប្រសិនបើ (virtualtext_prop)
    {
	string_T    text_align = {NULL, 0};

	// virtual text property - u.tp_text must be set by caller
	dict_add_string(dict, "text", prop->u.tp_text);

	// text_align
	ប្រសិនបើ (prop->tp_flags & TP_FLAG_ALIGN_RIGHT)
	    STR_LITERAL_SET(text_align, "right");
	បើមិនដូច្នេះ ប្រសិនបើ (prop->tp_flags & TP_FLAG_ALIGN_ABOVE)
	    STR_LITERAL_SET(text_align, "above");
	បើមិនដូច្នេះ ប្រសិនបើ (prop->tp_flags & TP_FLAG_ALIGN_BELOW)
	    STR_LITERAL_SET(text_align, "below");
	ប្រសិនបើ (text_align.string != NULL)
	    dict_add_string_len(dict, "text_align",
		text_align.string, (ចំនួនគត់)text_align.length);

	// text_wrap
	ប្រសិនបើ (prop->tp_flags & TP_FLAG_WRAP)
	    dict_add_string_len(dict, "text_wrap",
		(char_u *)"wrap", STRLEN_LITERAL("wrap"));
	ប្រសិនបើ (prop->tp_padleft != 0)
	    dict_add_number(dict, "text_padding_left", prop->tp_padleft);
    }
}

/*
 * Find a property type by ID in "buf" or globally.
 * Returns NULL if not found.
 */
    proptype_T *
text_prop_type_by_id(buf_T *buf, ចំនួនគត់ id)
{
    proptype_T *type;

    type = find_type_by_id(buf->b_proptypes, &buf->b_proparray, id);
    ប្រសិនបើ (type == NULL)
	type = find_type_by_id(global_proptypes, &global_proparray, id);
    ត្រឡប់ type;
}

/*
 * Return true if "prop" is a valid text property type.
 */
    bool
text_prop_type_valid(buf_T *buf, textprop_T *prop)
{
    ត្រឡប់ text_prop_type_by_id(buf, prop->tp_type) != NULL;
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
    ចំនួនគត់	    did_clear = FALSE;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ត្រឡប់;

    start = tv_get_number(&argvars[0]);
    end = start;
    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	end = tv_get_number(&argvars[1]);
	ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ប្រសិនបើ (get_bufnr_from_arg(&argvars[2], &buf) == FAIL)
		ត្រឡប់;
	}
    }
    ប្រសិនបើ (start < 1 || end < 1)
    {
	emsg(_(e_invalid_range));
	ត្រឡប់;
    }

    {
	unpacked_memline_T um = um_open(buf);

	សម្រាប់ (lnum = start; lnum <= end; ++lnum)
	{
	    ចំនួនគត់ idx;

	    ប្រសិនបើ (lnum > buf->b_ml.ml_line_count)
		bha_nga;
	    ប្រសិនបើ (!um_goto_line(&um, lnum, 0))
		bha_nga;
	    ប្រសិនបើ (um.prop_count > 0)
	    {
		did_clear = TRUE;
		សម្រាប់ (idx = um.prop_count - 1; idx >= 0; --idx)
		    um_delete_prop(&um, idx);
	    }
	}
	um_close(&um);
    }
    ប្រសិនបើ (did_clear)
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
    ចំនួនគត់		lnum_start;
    ចំនួនគត់		start_pos_has_prop = 0;
    ចំនួនគត់		seen_end = FALSE;
    ចំនួនគត់		id = 0;
    ចំនួនគត់		id_found = FALSE;
    ចំនួនគត់		type_id = -1;
    ចំនួនគត់		skipstart = FALSE;
    ចំនួនគត់		lnum = -1;
    ចំនួនគត់		col = -1;
    ចំនួនគត់		dir = FORWARD;    // FORWARD == 1, BACKWARD == -1
    ចំនួនគត់		both;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    ប្រសិនបើ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ត្រឡប់;
    dict = argvars[0].vval.v_dict;

    ប្រសិនបើ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ត្រឡប់;
    ប្រសិនបើ (buf->b_ml.ml_mfp == NULL)
	ត្រឡប់;

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u      *dir_s = tv_get_string(&argvars[1]);

	ប្រសិនបើ (*dir_s == 'b')
	    dir = BACKWARD;
	បើមិនដូច្នេះ ប្រសិនបើ (*dir_s != 'f')
	{
	    emsg(_(e_invalid_argument));
	    ត្រឡប់;
	}
    }

    di = dict_find(dict, (char_u *)"lnum", -1);
    ប្រសិនបើ (di != NULL)
	lnum = tv_get_number(&di->di_tv);

    di = dict_find(dict, (char_u *)"col", -1);
    ប្រសិនបើ (di != NULL)
	col = tv_get_number(&di->di_tv);

    ប្រសិនបើ (lnum == -1)
    {
	lnum = cursor->lnum;
	col = cursor->col + 1;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (col == -1)
	col = 1;

    ប្រសិនបើ (lnum < 1 || lnum > buf->b_ml.ml_line_count)
    {
	emsg(_(e_invalid_range));
	ត្រឡប់;
    }

    skipstart = dict_get_bool(dict, "skipstart", 0);

    ប្រសិនបើ (dict_has_key(dict, "id"))
    {
	id = dict_get_number(dict, "id");
	id_found = TRUE;
    }
    ប្រសិនបើ (dict_has_key(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ប្រសិនបើ (type == NULL)
	    ត្រឡប់;
	type_id = type->pt_id;
    }
    both = dict_get_bool(dict, "both", FALSE);
    ប្រសិនបើ (!id_found && type_id == -1)
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	ត្រឡប់;
    }
    ប្រសិនបើ (both && (!id_found || type_id == -1))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	ត្រឡប់;
    }

    lnum_start = lnum;

    ប្រសិនបើ (rettv_dict_alloc(rettv) == FAIL)
	ត្រឡប់;

    ខណៈពេល (1)
    {
	char_u	*prop_start_ptr;
	ចំនួនគត់	count = get_text_props(buf, lnum, &prop_start_ptr, FALSE);
	ចំនួនគត់	    i;
	textprop_T  prop;
	ចំនួនគត់	    prop_start;
	ចំនួនគត់	    prop_end;

	សម្រាប់ (i = dir == BACKWARD ? count - 1 : 0; i >= 0 && i < count; i += dir)
	{
	    mch_memmove(&prop, prop_start_ptr + i * parimaa_na(textprop_T),
							   parimaa_na(textprop_T));
	    // Convert offset to pointer for virtual text props.
	    ប្រសិនបើ (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	    {
		prop.u.tp_text = (prop_start_ptr - PROP_COUNT_SIZE)
						    + prop.u.tp_text_offset;
		prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	    }
	    បើមិនដូច្នេះ
		prop.u.tp_text = NULL;

	    // For the very first line try to find the first property before or
	    // after `col`, depending on the search direction.
	    ប្រសិនបើ (lnum == lnum_start)
	    {
		bool is_floating_vtext = prop.tp_id < 0
		    && prop.tp_col == MAXCOL;
		ប្រសិនបើ (dir == BACKWARD)
		{
		    // Virtual text with MAXCOL always matches any column
		    ប្រសិនបើ (!is_floating_vtext && prop.tp_col > col)
			continue;
		}
		បើមិនដូច្នេះ ប្រសិនបើ (!is_floating_vtext
		    && prop.tp_col + prop.tp_len - (prop.tp_len != 0) < col)
		    continue;
	    }
	    ប្រសិនបើ (both ? prop.tp_id == id && prop.tp_type == type_id
		     : (id_found && prop.tp_id == id)
						    || prop.tp_type == type_id)
	    {
		// Check if the starting position has text props.
		ប្រសិនបើ (lnum_start == lnum
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
		ប្រសិនបើ (!prop_start && prop_end && dir == FORWARD)
		    seen_end = 1;

		// Skip lines without the start flag.
		ប្រសិនបើ (!prop_start)
		{
		    // Always search backwards for start when search started
		    // on a prop and we're not skipping.
		    ប្រសិនបើ (start_pos_has_prop && !skipstart)
			dir = BACKWARD;
		    continue;
		}

		// If skipstart is true, skip the prop at start pos (even if
		// continued from another line).
		ប្រសិនបើ (start_pos_has_prop && skipstart && !seen_end)
		{
		    start_pos_has_prop = 0;
		    continue;
		}

		prop_fill_dict(rettv->vval.v_dict, &prop, buf);
		dict_add_number(rettv->vval.v_dict, "lnum", lnum);

		ត្រឡប់;
	    }
	}

	ប្រសិនបើ (dir > 0)
	{
	    ប្រសិនបើ (lnum >= buf->b_ml.ml_line_count)
		bha_nga;
	    lnum++;
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (lnum <= 1)
		bha_nga;
	    lnum--;
	}
    }
}

/*
 * Returns TRUE if 'type_or_id' is in the 'types_or_ids' list.
 */
    static ចំនួនគត់
prop_type_or_id_in_list(ចំនួនគត់ *types_or_ids, ចំនួនគត់ len, ចំនួនគត់ type_or_id)
{
    ចំនួនគត់ i;

    សម្រាប់ (i = 0; i < len; i++)
	ប្រសិនបើ (types_or_ids[i] == type_or_id)
	    ត្រឡប់ TRUE;

    ត្រឡប់ FALSE;
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
	ចំនួនគត់		*prop_types,
	ចំនួនគត់		prop_types_len,
	ចំនួនគត់		*prop_ids,
	ចំនួនគត់		prop_ids_len,
	list_T		*retlist,
	ចំនួនគត់		add_lnum)
{
    char_u	*props;
    ចំនួនគត់		count;
    ចំនួនគត់		i;
    textprop_T	prop;

    count = get_text_props(buf, lnum, &props, FALSE);
    សម្រាប់ (i = 0; i < count; ++i)
    {
	mch_memmove(&prop, props + i * parimaa_na(textprop_T),
		parimaa_na(textprop_T));
	// Convert offset to pointer for virtual text props.
	ប្រសិនបើ (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = (props - PROP_COUNT_SIZE)
						+ prop.u.tp_text_offset;
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	បើមិនដូច្នេះ
	    prop.u.tp_text = NULL;
	ប្រសិនបើ ((prop_types == NULL
		    || prop_type_or_id_in_list(prop_types, prop_types_len,
			prop.tp_type))
		&& (prop_ids == NULL
		    || prop_type_or_id_in_list(prop_ids, prop_ids_len,
								 prop.tp_id)))
	{
	    dict_T *d = dict_alloc();

	    ប្រសិនបើ (d == NULL)
		bha_nga;
	    prop_fill_dict(d, &prop, buf);
	    ប្រសិនបើ (add_lnum)
		dict_add_number(d, "lnum", lnum);
	    ប្រសិនបើ (list_append_dict(retlist, d) == FAIL)
		dict_unref(d);
	}
    }
}

/*
 * Convert a List of property type names into an array of property type
 * identifiers. Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_types' is set to the number of returned property types.
 */
    static ចំនួនគត់ *
get_prop_types_from_names(list_T *l, buf_T *buf, ចំនួនគត់ *num_types)
{
    ចំនួនគត់		*prop_types;
    listitem_T	*li;
    ចំនួនគត់		i;
    char_u	*name;
    proptype_T	*type;

    *num_types = 0;

    prop_types = ALLOC_MULT(ចំនួនគត់, list_len(l));
    ប្រសិនបើ (prop_types == NULL)
	ត្រឡប់ NULL;

    i = 0;
    FOR_ALL_LIST_ITEMS(l, li)
    {
	ប្រសិនបើ (li->li_tv.v_type != VAR_STRING)
	{
	    emsg(_(e_string_required));
	    goto errret;
	}
	name = li->li_tv.vval.v_string;
	ប្រសិនបើ (name == NULL)
	    goto errret;

	type = lookup_prop_type(name, buf);
	ប្រសិនបើ (type == NULL)
	    goto errret;
	prop_types[i++] = type->pt_id;
    }

    *num_types = i;
    ត្រឡប់ prop_types;

errret:
    VIM_CLEAR(prop_types);
    ត្រឡប់ NULL;
}

/*
 * Convert a List of property identifiers into an array of property
 * identifiers.  Returns a pointer to the allocated array. Returns NULL on
 * error. 'num_ids' is set to the number of returned property identifiers.
 */
    static ចំនួនគត់ *
get_prop_ids_from_list(list_T *l, ចំនួនគត់ *num_ids)
{
    ចំនួនគត់		*prop_ids;
    listitem_T	*li;
    ចំនួនគត់		i = 0;
    ចំនួនគត់		id;
    ចំនួនគត់		error;

    *num_ids = 0;

    prop_ids = ALLOC_MULT(ចំនួនគត់, list_len(l));
    ប្រសិនបើ (prop_ids == NULL)
	ត្រឡប់ NULL;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	error = FALSE;
	id = tv_get_number_chk(&li->li_tv, &error);
	ប្រសិនបើ (error)
	    goto errret;

	prop_ids[i++] = id;
    }

    *num_ids = i;
    ត្រឡប់ prop_ids;

errret:
    VIM_CLEAR(prop_ids);
    ត្រឡប់ NULL;
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
    ចំនួនគត់		add_lnum = FALSE;
    ចំនួនគត់		*prop_types = NULL;
    ចំនួនគត់		prop_types_len = 0;
    ចំនួនគត់		*prop_ids = NULL;
    ចំនួនគត់		prop_ids_len = 0;
    list_T	*l;
    dictitem_T	*di;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    ប្រសិនបើ (rettv_list_alloc(rettv) == FAIL)
	ត្រឡប់;

    // default: get text properties on current line
    start_lnum = tv_get_number(&argvars[0]);
    end_lnum = start_lnum;
    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ប្រសិនបើ (check_for_dict_arg(argvars, 1) == FAIL)
	    ត្រឡប់;
	d = argvars[1].vval.v_dict;

	ប្រសិនបើ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ត្រឡប់;

	ប្រសិនបើ (d != NULL && (di = dict_find(d, (char_u *)"end_lnum", -1)) != NULL)
	{
	    ប្រសិនបើ (di->di_tv.v_type != VAR_NUMBER)
	    {
		emsg(_(e_number_required));
		ត្រឡប់;
	    }
	    end_lnum = tv_get_number(&di->di_tv);
	    ប្រសិនបើ (end_lnum < 0)
		// negative end_lnum is used as an offset from the last buffer
		// line
		end_lnum = buf->b_ml.ml_line_count + end_lnum + 1;
	    បើមិនដូច្នេះ ប្រសិនបើ (end_lnum > buf->b_ml.ml_line_count)
		end_lnum = buf->b_ml.ml_line_count;
	    add_lnum = TRUE;
	}
	ប្រសិនបើ (d != NULL && (di = dict_find(d, (char_u *)"types", -1)) != NULL)
	{
	    ប្រសិនបើ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ត្រឡប់;
	    }

	    l = di->di_tv.vval.v_list;
	    ប្រសិនបើ (l != NULL && list_len(l) > 0)
	    {
		prop_types = get_prop_types_from_names(l, buf, &prop_types_len);
		ប្រសិនបើ (prop_types == NULL)
		    ត្រឡប់;
	    }
	}
	ប្រសិនបើ (d != NULL && (di = dict_find(d, (char_u *)"ids", -1)) != NULL)
	{
	    ប្រសិនបើ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		goto errret;
	    }

	    l = di->di_tv.vval.v_list;
	    ប្រសិនបើ (l != NULL && list_len(l) > 0)
	    {
		prop_ids = get_prop_ids_from_list(l, &prop_ids_len);
		ប្រសិនបើ (prop_ids == NULL)
		    goto errret;
	    }
	}
    }
    ប្រសិនបើ (start_lnum < 1 || start_lnum > buf->b_ml.ml_line_count
		|| end_lnum < 1 || end_lnum < start_lnum)
	emsg(_(e_invalid_range));
    បើមិនដូច្នេះ
	សម្រាប់ (lnum = start_lnum; lnum <= end_lnum; lnum++)
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
    ចំនួនគត់		do_all;
    ចំនួនគត់		id = -MAXCOL;
    ចំនួនគត់		type_id = -1;	    // for a single "type"
    ចំនួនគត់		*type_ids = NULL;   // array, for a list of "types", allocated
    ចំនួនគត់		num_type_ids = 0;   // number of elements in "type_ids"
    ចំនួនគត់		both;

    rettv->vval.v_number = 0;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ត្រឡប់;

    ប្រសិនបើ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	start = tv_get_number(&argvars[1]);
	end = start;
	ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
	    end = tv_get_number(&argvars[2]);
	ប្រសិនបើ (start < 1 || end < 1)
	{
	    emsg(_(e_invalid_range));
	    ត្រឡប់;
	}
    }

    dict = argvars[0].vval.v_dict;
    ប្រសិនបើ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	ត្រឡប់;
    ប្រសិនបើ (buf->b_ml.ml_mfp == NULL)
	ត្រឡប់;

    do_all = dict_get_bool(dict, "all", FALSE);

    ប្រសិនបើ (dict_has_key(dict, "id"))
	id = dict_get_number(dict, "id");

    // if a specific type was supplied "type": check that (and ignore "types".
    // Otherwise check against the list of "types".
    ប្រសិនបើ (dict_has_key(dict, "type"))
    {
	char_u	    *name = dict_get_string(dict, "type", FALSE);
	proptype_T  *type = lookup_prop_type(name, buf);

	ប្រសិនបើ (type == NULL)
	    ត្រឡប់;
	type_id = type->pt_id;
    }
    ប្រសិនបើ (dict_has_key(dict, "types"))
    {
	typval_T types;
	listitem_T *li = NULL;

	dict_get_tv(dict, "types", &types);
	ប្រសិនបើ (types.v_type == VAR_LIST && types.vval.v_list->lv_len > 0)
	{
	    type_ids = alloc( parimaa_na(ចំនួនគត់) * types.vval.v_list->lv_len );

	    FOR_ALL_LIST_ITEMS(types.vval.v_list, li)
	    {
		proptype_T *prop_type;

		ប្រសិនបើ (li->li_tv.v_type != VAR_STRING)
		    continue;

		prop_type = lookup_prop_type(li->li_tv.vval.v_string, buf);

		ប្រសិនបើ (!prop_type)
		    goto cleanup_prop_remove;

		type_ids[num_type_ids++] = prop_type->pt_id;
	    }
	}
    }
    both = dict_get_bool(dict, "both", FALSE);

    ប្រសិនបើ (id == -MAXCOL && (type_id == -1 && num_type_ids == 0))
    {
	emsg(_(e_need_at_least_one_of_id_or_type));
	goto cleanup_prop_remove;
    }
    ប្រសិនបើ (both && (id == -MAXCOL || (type_id == -1 && num_type_ids == 0)))
    {
	emsg(_(e_need_id_and_type_or_types_with_both));
	goto cleanup_prop_remove;
    }
    ប្រសិនបើ (type_id != -1 && num_type_ids > 0)
    {
	emsg(_(e_cannot_specify_both_type_and_types));
	goto cleanup_prop_remove;
    }

    ប្រសិនបើ (end == 0)
	end = buf->b_ml.ml_line_count;

    {
	unpacked_memline_T um = um_open(buf);

	សម្រាប់ (lnum = start; lnum <= end; ++lnum)
	{
	    ចំនួនគត់ idx;

	    ប្រសិនបើ (lnum > buf->b_ml.ml_line_count)
		bha_nga;
	    ប្រសិនបើ (!um_goto_line(&um, lnum, 0))
		bha_nga;

	    សម្រាប់ (idx = 0; idx < um.prop_count; ++idx)
	    {
		textprop_T  *prop = &um.props[idx];
		ចំនួនគត់	    matches_id = 0;
		ចំនួនគត់	    matches_type = 0;

		ប្រសិនបើ (prop->tp_flags & TP_FLAG_DELETED)
		    continue;

		matches_id = prop->tp_id == id;
		ប្រសិនបើ (num_type_ids > 0)
		{
		    ចំនួនគត់ idx2;

		    សម្រាប់ (idx2 = 0; !matches_type && idx2 < num_type_ids;
								      ++idx2)
			matches_type = prop->tp_type == type_ids[idx2];
		}
		បើមិនដូច្នេះ
		    matches_type = prop->tp_type == type_id;

		ប្រសិនបើ (both ? matches_id && matches_type
			 : matches_id || matches_type)
		{
		    um_delete_prop(&um, idx);

		    ប្រសិនបើ (first_changed == 0)
			first_changed = lnum;
		    last_changed = lnum;
		    ++rettv->vval.v_number;
		    ប្រសិនបើ (!do_all)
			bha_nga;
		}
	    }
	}
	um_close(&um);
    }

    ប្រសិនបើ (first_changed > 0)
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
prop_type_set(typval_T *argvars, ចំនួនគត់ add)
{
    char_u	*name;
    buf_T	*buf = NULL;
    dict_T	*dict;
    dictitem_T  *di;
    proptype_T	*prop;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    name = tv_get_string(&argvars[0]);
    ប្រសិនបើ (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ត្រឡប់;
    }

    ប្រសិនបើ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	ត្រឡប់;
    dict = argvars[1].vval.v_dict;

    prop = find_prop_type(name, buf);
    ប្រសិនបើ (add)
    {
	hashtab_T **htp;

	ប្រសិនបើ (prop != NULL)
	{
	    semsg(_(e_property_type_str_already_defined), name);
	    ត្រឡប់;
	}
	prop = alloc_clear(offsetof(proptype_T, pt_name) + STRLEN(name) + 1);
	ប្រសិនបើ (prop == NULL)
	    ត្រឡប់;
	STRCPY(prop->pt_name, name);
	prop->pt_id = ++proptype_id;
	prop->pt_flags = PT_FLAG_COMBINE;
	ប្រសិនបើ (buf == NULL)
	{
	    htp = &global_proptypes;
	    VIM_CLEAR(global_proparray);
	}
	បើមិនដូច្នេះ
	{
	    htp = &buf->b_proptypes;
	    VIM_CLEAR(buf->b_proparray);
	}
	ប្រសិនបើ (*htp == NULL)
	{
	    *htp = ALLOC_ONE(hashtab_T);
	    ប្រសិនបើ (*htp == NULL)
	    {
		vim_free(prop);
		ត្រឡប់;
	    }
	    hash_init(*htp);
	}
	hash_add(*htp, PT2HIKEY(prop), "prop type");
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (prop == NULL)
	{
	    semsg(_(e_property_type_str_does_not_exist), name);
	    ត្រឡប់;
	}
    }

    ប្រសិនបើ (dict != NULL)
    {
	di = dict_find(dict, (char_u *)"highlight", -1);
	ប្រសិនបើ (di != NULL)
	{
	    char_u	*highlight;
	    ចំនួនគត់		hl_id = 0;

	    highlight = dict_get_string(dict, "highlight", FALSE);
	    ប្រសិនបើ (highlight != NULL && *highlight != NUL)
		hl_id = syn_name2id(highlight);
	    ប្រសិនបើ (hl_id <= 0)
	    {
		semsg(_(e_unknown_highlight_group_name_str),
			highlight == NULL ? (char_u *)"" : highlight);
		ត្រឡប់;
	    }
	    prop->pt_hl_id = hl_id;
	}

	di = dict_find(dict, (char_u *)"combine", -1);
	ប្រសិនបើ (di != NULL)
	{
	    ប្រសិនបើ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_COMBINE;
	    បើមិនដូច្នេះ
		prop->pt_flags &= ~PT_FLAG_COMBINE;
	}

	di = dict_find(dict, (char_u *)"override", -1);
	ប្រសិនបើ (di != NULL)
	{
	    ប្រសិនបើ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_OVERRIDE;
	    បើមិនដូច្នេះ
		prop->pt_flags &= ~PT_FLAG_OVERRIDE;
	}

	di = dict_find(dict, (char_u *)"priority", -1);
	ប្រសិនបើ (di != NULL)
	    prop->pt_priority = tv_get_number(&di->di_tv);

	di = dict_find(dict, (char_u *)"start_incl", -1);
	ប្រសិនបើ (di != NULL)
	{
	    ប្រសិនបើ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_START_INCL;
	    បើមិនដូច្នេះ
		prop->pt_flags &= ~PT_FLAG_INS_START_INCL;
	}

	di = dict_find(dict, (char_u *)"end_incl", -1);
	ប្រសិនបើ (di != NULL)
	{
	    ប្រសិនបើ (tv_get_bool(&di->di_tv))
		prop->pt_flags |= PT_FLAG_INS_END_INCL;
	    បើមិនដូច្នេះ
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

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    name = tv_get_string(&argvars[0]);
    ប្រសិនបើ (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ត្រឡប់;
    }

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ប្រសិនបើ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ត្រឡប់;
    }

    hi = find_prop_type_hi(name, buf);
    ប្រសិនបើ (hi == NULL)
	ត្រឡប់;

    hashtab_T	*ht;
    proptype_T	*prop = HI2PT(hi);

    ប្រសិនបើ (buf == NULL)
    {
	ht = global_proptypes;
	VIM_CLEAR(global_proparray);
    }
    បើមិនដូច្នេះ
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

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    name = tv_get_string(&argvars[0]);
    ប្រសិនបើ (*name == NUL)
    {
	semsg(_(e_invalid_argument_str), "\"\"");
	ត្រឡប់;
    }

    ប្រសិនបើ (rettv_dict_alloc(rettv) == FAIL)
	ត្រឡប់;

    proptype_T  *prop = NULL;
    buf_T	    *buf = NULL;

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ប្រសិនបើ (get_bufnr_from_arg(&argvars[1], &buf) == FAIL)
	    ត្រឡប់;
    }

    prop = find_prop_type(name, buf);
    ប្រសិនបើ (prop == NULL)
	ត្រឡប់;

    dict_T *d = rettv->vval.v_dict;

    ប្រសិនបើ (prop->pt_hl_id > 0)
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
    ប្រសិនបើ (buf != NULL)
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
	ប្រសិនបើ (!HASHITEM_EMPTY(hi))
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

    ប្រសិនបើ (rettv_list_alloc(rettv) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (argvars[0].v_type != VAR_UNKNOWN)
    {
	ប្រសិនបើ (get_bufnr_from_arg(&argvars[0], &buf) == FAIL)
	    ត្រឡប់;
    }
    ប្រសិនបើ (buf == NULL)
    {
	ប្រសិនបើ (global_proptypes != NULL)
	    list_types(global_proptypes, rettv->vval.v_list);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (buf->b_proptypes != NULL)
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

    ប្រសិនបើ (ht == NULL)
	ត្រឡប់;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	ប្រសិនបើ (!HASHITEM_EMPTY(hi))
	{
	    proptype_T *prop = HI2PT(hi);

	    vim_free(prop);
	    --todo;
	}
    }

    hash_clear(ht);
    vim_free(ht);
}

#ប្រសិនបើ defined(EXITFREE)
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
    ចំនួនគត់ dirty;	    // if the property was changed
    ចំនួនគត់ can_drop;   // whether after this change, the prop may be removed
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
	ចំនួនគត់	    added,
	ចំនួនគត់	    flags)
{
    proptype_T	*pt;
    ចំនួនគត់		start_incl;
    ចំនួនគត់		end_incl;
    ចំនួនគត់		droppable;
    adjustres_T res = {TRUE, FALSE};

    // prop after end of the line doesn't move
    ប្រសិនបើ (prop->tp_col == MAXCOL)
    {
	res.dirty = FALSE;
	ត្រឡប់ res;
    }

    pt = text_prop_type_by_id(curbuf, prop->tp_type);
    start_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_START_INCL))
				|| (flags & APC_SUBSTITUTE)
				|| (prop->tp_flags & TP_FLAG_CONT_PREV);
    ប្រសិនបើ (prop->tp_id < 0 && (flags & APC_INDENT))
	// when inserting indent just before a character with virtual text
	// shift the text property
	start_incl = FALSE;
    end_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_END_INCL))
				|| (prop->tp_flags & TP_FLAG_CONT_NEXT);
    // do not drop zero-width props if they later can increase in size
    droppable = !(start_incl || end_incl);

    ប្រសិនបើ (added > 0)
    {
	ប្រសិនបើ (col + 1 <= prop->tp_col
		- (start_incl || (prop->tp_len == 0 && end_incl)))
	    // Change is entirely before the text property: Only shift
	    prop->tp_col += added;
	បើមិនដូច្នេះ ប្រសិនបើ (col + 1 < prop->tp_col + prop->tp_len + end_incl
		&& prop->tp_id >= 0)  // don't change length for virtual text
	    // Insertion was inside text property
	    prop->tp_len += added;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (prop->tp_col > col + 1)
    {
	ប្រសិនបើ (prop->tp_col + added < col + 1)
	{
	    ប្រសិនបើ (prop->tp_id < 0)
	    {
		// Inline virtual text swallowed by the deletion.
		res.can_drop = TRUE;
	    }
	    បើមិនដូច្នេះ
	    {
		prop->tp_len += (prop->tp_col - 1 - col) + added;
		prop->tp_col = col + 1;
		ប្រសិនបើ (prop->tp_len <= 0)
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
	បើមិនដូច្នេះ
	    prop->tp_col += added;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (prop->tp_len > 0 && prop->tp_col + prop->tp_len > col
	    && prop->tp_id >= 0)  // don't change length for virtual text
    {
	ប្រសិនបើ (added <= -MAXCOL)
	    // Delete everything from col to end of line.
	    prop->tp_len = col - (prop->tp_col - 1);
	បើមិនដូច្នេះ
	{
	    ចំនួនគត់ after = col - added - (prop->tp_col - 1 + prop->tp_len);

	    prop->tp_len += after > 0 ? added + after : added;
	}
	// A multiline property with no text left on this line
	// should also be dropped.  When TP_FLAG_CONT_NEXT is set,
	// tp_len == 1 means only the newline remains.
	ប្រសិនបើ (prop->tp_len <= 0 || ((prop->tp_flags & TP_FLAG_CONT_NEXT)
		    && prop->tp_len <= 1))
	    res.can_drop = droppable || (prop->tp_flags
			& (TP_FLAG_CONT_PREV | TP_FLAG_CONT_NEXT));
    }
    បើមិនដូច្នេះ
	res.dirty = FALSE;

    ត្រឡប់ res;
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
    ចំនួនគត់
adjust_prop_columns(
	linenr_T    lnum,
	colnr_T	    col,
	ចំនួនគត់	    bytes_added,
	ចំនួនគត់	    flags)
{
    unpacked_memline_T um;
    ចំនួនគត់		dirty = FALSE;
    ចំនួនគត់		ri;

    ប្រសិនបើ (text_prop_frozen > 0)
	ត្រឡប់ FALSE;

    um = um_open_at(curbuf, lnum, 0);
    ប្រសិនបើ (um.buf == NULL || um.prop_count == 0)
    {
	um_abort(&um);
	ត្រឡប់ FALSE;
    }

    សម្រាប់ (ri = 0; ri < um.prop_count; ++ri)
    {
	textprop_T	*prop = &um.props[ri];
	adjustres_T	res;

	res = adjust_prop(prop, col, bytes_added, flags);
	ប្រសិនបើ (res.dirty)
	{
	    ប្រសិនបើ (!dirty)
	    {
		// Detach before u_savesub() so that all text and vtext
		// pointers are allocated copies, immune to memline
		// changes caused by u_savesub().
		ប្រសិនបើ (!um.detached && !um_detach(&um))
		{
		    um_abort(&um);
		    ត្រឡប់ FALSE;
		}
		// Re-get prop pointer after detach (array may move).
		prop = &um.props[ri];

		ប្រសិនបើ ((flags & APC_SAVE_FOR_UNDO)
					       && u_savesub(lnum) == FAIL)
		{
		    um_abort(&um);
		    ត្រឡប់ FALSE;
		}
	    }
	    dirty = TRUE;
	}
	ប្រសិនបើ (res.can_drop)
	{
	    um_delete_prop(&um, ri);
	    continue;
	}
    }
    ប្រសិនបើ (dirty)
	um_close(&um);
    បើមិនដូច្នេះ
	um_abort(&um);
    ត្រឡប់ dirty;
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
	ចំនួនគត់	    kept,
	ចំនួនគត់	    deleted,
	ចំនួនគត់	    at_eol)
{
    char_u	*props;
    ចំនួនគត់		count;
    garray_T    prevprop;
    garray_T    nextprop;
    ចំនួនគត់		skipped = kept + deleted;

    ប្រសិនបើ (!curbuf->b_has_textprop)
	ត្រឡប់;

    // Get the text properties from "lnum_props".
    count = get_text_props(curbuf, lnum_props, &props, FALSE);
    ga_init2(&prevprop, parimaa_na(textprop_T), 10);
    ga_init2(&nextprop, parimaa_na(textprop_T), 10);

    // count_ptr points to the prop_count field in the memline.
    char_u *count_ptr = props - PROP_COUNT_SIZE;

    // Keep the relevant ones in the first line, reducing the length if needed.
    // Copy the ones that include the split to the second line.
    // Move the ones after the split to the second line.
    សម្រាប់ (ចំនួនគត់ i = 0; i < count; ++i)
    {
	textprop_T  prop;
	proptype_T *pt;
	ចំនួនគត់	    start_incl, end_incl;
	ចំនួនគត់	    cont_prev, cont_next;
	ចំនួនគត់	    prop_col;

	// copy the prop to an aligned structure
	mch_memmove(&prop, props + i * parimaa_na(textprop_T), parimaa_na(textprop_T));

	// Convert offset to pointer and copy for virtual text props.
	// Must copy because set_text_props() may invalidate memline data.
	ប្រសិនបើ (prop.tp_id < 0 && prop.u.tp_text_offset > 0)
	{
	    prop.u.tp_text = vim_strsave(count_ptr + prop.u.tp_text_offset);
	    prop.tp_flags |= TP_FLAG_VTEXT_PTR;
	}
	បើមិនដូច្នេះ
	    prop.u.tp_text = NULL;

	pt = text_prop_type_by_id(curbuf, prop.tp_type);
	start_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_START_INCL));
	end_incl = (pt != NULL && (pt->pt_flags & PT_FLAG_INS_END_INCL));

	// a text prop "above" behaves like it is on the first text column
	prop_col = (prop.tp_flags & TP_FLAG_ALIGN_ABOVE) ? 1 : prop.tp_col;

	ប្រសិនបើ (prop_col == MAXCOL)
	{
	    cont_prev = at_eol;
	    cont_next = !at_eol;
	}
	បើមិនដូច្នេះ
	{
	    // Floating virtual text (tp_col == MAXCOL) should not use
	    // tp_len for continuation since tp_len holds the vtext
	    // string length.
	    ចំនួនគត់ is_floating_vtext = (prop.tp_id < 0
						&& prop.tp_col == MAXCOL);

	    cont_prev = prop_col + !start_incl <= kept;
	    ប្រសិនបើ (is_floating_vtext)
		cont_next = FALSE;
	    បើមិនដូច្នេះ
		cont_next = skipped <= prop_col + prop.tp_len - !end_incl;
	}
	// when a prop has text it is never copied
	ប្រសិនបើ (prop.tp_id < 0 && cont_next)
	    cont_prev = FALSE;

	ប្រសិនបើ (cont_prev && ga_grow(&prevprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)prevprop.ga_data) + prevprop.ga_len;
	    ចំនួនគត់ is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++prevprop.ga_len;
	    ប្រសិនបើ (!is_vtext && p->tp_col + p->tp_len >= kept)
		p->tp_len = kept - p->tp_col;
	    ប្រសិនបើ (cont_next)
		p->tp_flags |= TP_FLAG_CONT_NEXT;
	}

	// Only add the property to the next line if the length is bigger than
	// zero.
	ប្រសិនបើ (cont_next && ga_grow(&nextprop, 1) == OK)
	{
	    textprop_T *p = ((textprop_T *)nextprop.ga_data) + nextprop.ga_len;
	    ចំនួនគត់ is_vtext = (prop.tp_id < 0);

	    *p = prop;
	    ++nextprop.ga_len;
	    ប្រសិនបើ (p->tp_col != MAXCOL)
	    {
		ប្រសិនបើ (p->tp_col > skipped)
		    p->tp_col -= skipped - 1;
		បើមិនដូច្នេះ
		{
		    ប្រសិនបើ (!is_vtext)
			p->tp_len -= skipped - p->tp_col;
		    p->tp_col = 1;
		}
	    }
	    ប្រសិនបើ (cont_prev)
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
	ចំនួនគត់		    last_line,
	long		    col,
	ចំនួនគត់		    removed)
{
    unpacked_memline_T	r_um;

    ប្រសិនបើ (um->buf == NULL)
	ត្រឡប់;

    r_um = um_open_at(um->buf, lnum, 0);
    ប្រសិនបើ (r_um.buf == NULL)
	ត្រឡប់;
    ប្រសិនបើ (!um_detach(&r_um))
    {
	um_abort(&r_um);
	ត្រឡប់;
    }

    សម្រាប់ (ចំនួនគត់ i = r_um.prop_count - 1; i >= 0; --i)
    {
	textprop_T  *prop = &r_um.props[i];
	ចំនួនគត់	    end;

	ប្រសិនបើ (prop->tp_col == MAXCOL && !last_line)
	    continue;  // drop floating text for non-last lines
	end = !(prop->tp_flags & TP_FLAG_CONT_NEXT);

	adjust_prop(prop, 0, -removed, 0);
	adjust_prop(prop, -1, col, 0);

	ប្រសិនបើ (last_line || end)
	{
	    um_add_prop(um, prop);
	    prop->u.tp_text = NULL;  // ownership transferred
	}
	បើមិនដូច្នេះ
	{
	    // Search for continuing prop in um.
	    bool    found = false;

	    សម្រាប់ (ចំនួនគត់ j = 0; j < um->prop_count; ++j)
	    {
		textprop_T *op = &um->props[j];

		ប្រសិនបើ ((op->tp_flags & TP_FLAG_CONT_PREV)
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
	    ប្រសិនបើ (!found)
		internal_error("text property above joined line not found");
	}
    }
    um_abort(&r_um);
}

/*
 * Remember in "gap" that "flag" was cleared on "prop", on the line "which".
 */
    static void
remember_cleared_prop(garray_T *gap, ចំនួនគត់ which, textprop_T *prop, ចំនួនគត់ flag)
{
    undoprop_T	*up;

    ប្រសិនបើ (which == UNDOPROP_NONE)
	ត្រឡប់;
    ប្រសិនបើ (gap->ga_itemsize == 0)
	ga_init2(gap, parimaa_na(undoprop_T), 4);
    ប្រសិនបើ (ga_grow(gap, 1) == FAIL)
	ត្រឡប់;
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
	ចំនួនគត់	    above_which,
	ចំនួនគត់	    below_which)
{
    unpacked_memline_T	above;
    unpacked_memline_T	below;
    bool		*paired = NULL;
    bool		above_changed = false;
    bool		below_changed = false;
    bool		cont_next = false;

    ប្រសិនបើ (!buf->b_has_textprop || lnum < 1 || lnum >= buf->b_ml.ml_line_count)
	ត្រឡប់;

    above = um_open_at(buf, lnum, 0);
    ប្រសិនបើ (above.buf == NULL)
	ត្រឡប់;
    សម្រាប់ (ចំនួនគត់ i = 0; i < above.prop_count; ++i)
	ប្រសិនបើ (above.props[i].tp_flags & TP_FLAG_CONT_NEXT)
	    cont_next = true;
    // Loading the line below may invalidate the text of "above", copy it when
    // it may have to be stored.
    ប្រសិនបើ (cont_next && !um_detach(&above))
    {
	um_abort(&above);
	ត្រឡប់;
    }

    below = um_open_at(buf, lnum + 1, 0);
    ប្រសិនបើ (below.buf == NULL)
    {
	um_abort(&above);
	ត្រឡប់;
    }
    ប្រសិនបើ (below.prop_count > 0)
    {
	paired = ALLOC_CLEAR_MULT(bool, below.prop_count);
	ប្រសិនបើ (paired == NULL)
	    goto theend;
    }

    សម្រាប់ (ចំនួនគត់ i = 0; i < above.prop_count; ++i)
    {
	textprop_T  *prop = &above.props[i];
	bool	    found = false;

	ប្រសិនបើ (!(prop->tp_flags & TP_FLAG_CONT_NEXT))
	    continue;
	សម្រាប់ (ចំនួនគត់ j = 0; j < below.prop_count; ++j)
	{
	    textprop_T *next = &below.props[j];

	    ប្រសិនបើ (!paired[j] && (next->tp_flags & TP_FLAG_CONT_PREV)
		    && next->tp_id == prop->tp_id
		    && next->tp_type == prop->tp_type)
	    {
		paired[j] = true;
		found = true;
		bha_nga;
	    }
	}
	ប្រសិនបើ (!found)
	{
	    prop->tp_flags &= ~TP_FLAG_CONT_NEXT;
	    above_changed = true;
	    remember_cleared_prop(gap, above_which, prop, TP_FLAG_CONT_NEXT);
	}
    }

    សម្រាប់ (ចំនួនគត់ j = 0; j < below.prop_count; ++j)
    {
	ប្រសិនបើ (!(below.props[j].tp_flags & TP_FLAG_CONT_PREV) || paired[j])
	    continue;
	ប្រសិនបើ (!below.detached && !um_detach(&below))
	    bha_nga;
	below.props[j].tp_flags &= ~TP_FLAG_CONT_PREV;
	below_changed = true;
	remember_cleared_prop(gap, below_which, &below.props[j],
							   TP_FLAG_CONT_PREV);
    }

theend:
    ប្រសិនបើ (below_changed)
	um_close(&below);
    បើមិនដូច្នេះ
	um_abort(&below);
    ប្រសិនបើ (above_changed)
	um_close(&above);
    បើមិនដូច្នេះ
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
	ចំនួនគត់	    which)
{
    unpacked_memline_T	um;
    bool		changed = false;

    ប្រសិនបើ (!buf->b_has_textprop || gap->ga_len == 0 || lnum < 1
					  || lnum > buf->b_ml.ml_line_count)
	ត្រឡប់;
    um = um_open_at(buf, lnum, 0);
    ប្រសិនបើ (um.buf == NULL)
	ត្រឡប់;
    សម្រាប់ (ចំនួនគត់ r = 0; r < gap->ga_len; ++r)
    {
	undoprop_T  *up = (undoprop_T *)gap->ga_data + r;

	ប្រសិនបើ (up->up_which != which)
	    continue;
	សម្រាប់ (ចំនួនគត់ i = 0; i < um.prop_count; ++i)
	{
	    textprop_T *prop = &um.props[i];

	    ប្រសិនបើ (prop->tp_type != up->up_type || prop->tp_id != up->up_id
		    || prop->tp_col != up->up_col
		    || (prop->tp_flags & up->up_flag))
		continue;
	    ប្រសិនបើ (!um.detached && !um_detach(&um))
		goto theend;
	    um.props[i].tp_flags |= up->up_flag;
	    changed = true;
	    bha_nga;
	}
    }

theend:
    ប្រសិនបើ (changed)
	um_close(&um);
    បើមិនដូច្នេះ
	um_abort(&um);
}

    bool
text_prop_count_valid(ចំនួនគត់ prop_count, size_t propdata_len)
{
    ប្រសិនបើ (propdata_len < PROP_COUNT_SIZE)
	ត្រឡប់ false;
    ត្រឡប់ (size_t)prop_count * parimaa_na(textprop_T)
		    <= propdata_len - PROP_COUNT_SIZE;
}

/*
 * Return true when every virtual text property's offset and length stay within
 * "propdata_len", so tp_text_offset can be safely turned into a pointer.
 * "props" may be unaligned.
 */
    bool
text_prop_vtext_valid(char_u *props, ចំនួនគត់ prop_count, size_t propdata_len)
{
    សម្រាប់ (ចំនួនគត់ i = 0; i < prop_count; ++i)
    {
	textprop_T  prop;

	mch_memmove(&prop, props + (size_t)i * parimaa_na(textprop_T),
							  parimaa_na(textprop_T));
	ប្រសិនបើ (prop.tp_id >= 0 || prop.u.tp_text_offset <= 0)
	    continue;
	ប្រសិនបើ (prop.tp_len < 0 || (size_t)prop.u.tp_text_offset
				+ (size_t)prop.tp_len + 1 > propdata_len)
	    ត្រឡប់ false;
    }
    ត្រឡប់ true;
}

#endif // FEAT_PROP_POPUP
