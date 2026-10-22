/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * blob.c: Blob support by Yasuhiro Matsumoto
 */

#include "vim.h"

#ถ้า defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(void)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    ถ้า (blob != NULL)
	ga_init2(&blob->bv_ga, 1, 100);
    คืน blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    จำนวนเต็ม
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    ถ้า (b == NULL)
	คืน FAIL;

    rettv_blob_set(rettv, b);
    คืน OK;
}

/*
 * Set a blob as the return value.
 */
    void
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    ถ้า (b != NULL)
	++b->bv_refcount;
}

/*
 * Create a copy of a blob.
 * The refcount of the new blob is set to 1.
 * Returns NULL when "orig" is NULL or out of memory.
 */
    blob_T *
blob_copy(blob_T *orig)
{
    จำนวนเต็ม		len;
    blob_T	*copy;

    ถ้า (orig == NULL)
	คืน NULL;

    copy = blob_alloc();
    ถ้า (copy == NULL)
	คืน NULL;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    ถ้า (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	ถ้า (copy->bv_ga.ga_data == NULL)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    คืน copy;
}

    void
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    void
blob_unref(blob_T *b)
{
    ถ้า (b != NULL && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    ถ้า (b == NULL)
	คืน 0L;
    คืน b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    จำนวนเต็ม
blob_get(blob_T *b, จำนวนเต็ม idx)
{
    คืน ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    void
blob_set(blob_T *blob, จำนวนเต็ม idx, จำนวนเต็ม byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    void
blob_set_append(blob_T *blob, จำนวนเต็ม idx, จำนวนเต็ม byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    ถ้า (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	ถ้า (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    จำนวนเต็ม
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    จำนวนเต็ม	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    ถ้า (len1 == 0 && len2 == 0)
	คืน TRUE;
    ถ้า (b1 == b2)
	คืน TRUE;
    ถ้า (len1 != len2)
	คืน FALSE;

    สำหรับ (i = 0; i < b1->bv_ga.ga_len; i++)
	ถ้า (blob_get(b1, i) != blob_get(b2, i)) คืน FALSE;
    คืน TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    จำนวนเต็ม
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    sa_nracanaa stat	st;
    จำนวนเต็ม		whence;
    off_T	size = size_arg;

    ถ้า (fstat(fileno(fd), &st) < 0)
	คืน FAIL;  // can't read the file, error

    ถ้า (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	ถ้า (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    มิฉะนั้น
    {
	// limit the offset to not go before the start of the file
	ถ้า (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	ถ้า (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    ถ้า (size <= 0)
	คืน OK;
    ถ้า (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	คืน OK;

    ถ้า (ga_grow(&blob->bv_ga, (จำนวนเต็ม)size) == FAIL)
	คืน FAIL;
    blob->bv_ga.ga_len = (จำนวนเต็ม)size;
    ถ้า (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = NULL;
	คืน FAIL;
    }
    คืน OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    จำนวนเต็ม
write_blob(FILE *fd, blob_T *blob)
{
    ถ้า (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	คืน FAIL;
    }
    คืน OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    static const var_na hex_chars[] = "0123456789ABCDEF";
    จำนวนเต็ม		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    ถ้า (blob == NULL || (blen = blob_len(blob)) == 0)
    {
	*tofree = NULL;
	คืน (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    ถ้า (buf == NULL)
    {
	*tofree = NULL;
	คืน (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    สำหรับ (จำนวนเต็ม i = 0; i < blen; i++)
    {
	unsigned b;

	ถ้า (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    คืน buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    void
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    ถ้า (rettv_list_alloc(rettv) == FAIL)
	คืน;

    สำหรับ (จำนวนเต็ม i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	ถ้า (l2 == NULL)
	    คืน;

	ถ้า (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    คืน;
	}

	ถ้า (list_append_number(l2, i) == FAIL
		|| list_append_number(l2, blob_get(blob, i)) == FAIL)
	    คืน;
    }
}

/*
 * Convert a string variable, in the format of blob2string(), to a blob.
 * Return NULL when conversion failed.
 */
    blob_T *
string2blob(char_u *str)
{
    blob_T  *blob = blob_alloc();
    char_u  *s = str;

    ถ้า (blob == NULL)
	คืน NULL;
    ถ้า (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	goto failed;
    s += 2;
    ขณะ (vim_isxdigit(*s))
    {
	ถ้า (!vim_isxdigit(s[1]))
	    goto failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	ถ้า (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    ถ้า (*skipwhite(s) != NUL)
	goto failed;  // text after final digit

    ++blob->bv_refcount;
    คืน blob;

failed:
    blob_free(blob);
    คืน NULL;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    static จำนวนเต็ม
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	จำนวนเต็ม		exclusive,
	typval_T	*rettv)
{
    ถ้า (n1 < 0)
    {
	n1 = len + n1;
	ถ้า (n1 < 0)
	    n1 = 0;
    }
    ถ้า (n2 < 0)
	n2 = len + n2;
    มิฉะนั้น ถ้า (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    ถ้า (exclusive)
	--n2;
    ถ้า (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = NULL;
    }
    มิฉะนั้น
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	ถ้า (new_blob != NULL)
	{
	    ถ้า (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		คืน FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    สำหรับ (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    คืน OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    static จำนวนเต็ม
blob_index(
	blob_T		*blob,
	จำนวนเต็ม		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    ถ้า (idx < 0)
	idx = len + idx;
    ถ้า (idx < len && idx >= 0)
    {
	จำนวนเต็ม v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = v;
    }
    มิฉะนั้น
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	คืน FAIL;
    }

    คืน OK;
}

    จำนวนเต็ม
blob_slice_or_index(
	blob_T		*blob,
	จำนวนเต็ม		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	จำนวนเต็ม		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    ถ้า (is_range)
	คืน blob_slice(blob, len, n1, n2, exclusive, rettv);
    มิฉะนั้น
	คืน blob_index(blob, len, n1, rettv);
    คืน OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    จำนวนเต็ม
check_blob_index(long bloblen, varnumber_T n1, จำนวนเต็ม quiet)
{
    ถ้า (n1 < 0 || n1 > bloblen)
    {
	ถ้า (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	คืน FAIL;
    }
    คืน OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    จำนวนเต็ม
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, จำนวนเต็ม quiet)
{
    ถ้า (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	ถ้า (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	คืน FAIL;
    }
    คืน OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    จำนวนเต็ม
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    จำนวนเต็ม	il, ir;

    ถ้า (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_value_does_not_have_right_number_of_bytes));
	คืน FAIL;
    }

    ir = 0;
    สำหรับ (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    คืน OK;
}

/*
 * "add(blob, item)" function
 */
    void
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    จำนวนเต็ม		error = FALSE;
    varnumber_T n;

    ถ้า (b == NULL)
    {
	ถ้า (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	คืน;
    }

    ถ้า (value_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	คืน;

    n = tv_get_number_chk(&argvars[1], &error);
    ถ้า (error)
	คืน;

    ga_append(&b->bv_ga, (จำนวนเต็ม)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    void
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    จำนวนเต็ม		error = FALSE;
    long	idx;
    long	end;
    จำนวนเต็ม		len;
    char_u	*p;

    ถ้า (b != NULL && value_check_lock(b->bv_lock, arg_errmsg, TRUE))
	คืน;

    idx = (long)tv_get_number_chk(&argvars[1], &error);
    ถ้า (error)
	คืน;

    len = blob_len(b);

    ถ้า (idx < 0)
	// count from the end
	idx = len + idx;
    ถ้า (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	คืน;
    }
    ถ้า (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_number = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	คืน;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_number_chk(&argvars[2], &error);
    ถ้า (error)
	คืน;
    ถ้า (end < 0)
	// count from the end
	end = len + end;
    ถ้า (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	คืน;
    }
    newblob = blob_alloc();
    ถ้า (newblob == NULL)
	คืน;
    newblob->bv_ga.ga_len = end - idx + 1;
    ถ้า (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	คืน;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    ถ้า (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (size_t)(len - end - 1));
    b->bv_ga.ga_len -= end - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    void
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*expr,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    จำนวนเต็ม		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    จำนวนเต็ม		idx = 0;
    จำนวนเต็ม		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ถ้า (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = NULL;
    }
    ถ้า (b == NULL || (filtermap == FILTERMAP_FILTER
			    && value_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	คืน;

    b_ret = b;
    ถ้า (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	ถ้า (rettv->vval.v_blob == NULL)
	    คืน;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    จำนวนเต็ม prev_lock = b->bv_lock;
    ถ้า (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    สำหรับ (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_NUMBER;
	val = blob_get(b, i);
	tv.vval.v_number = val;
	set_vim_var_nr(VV_KEY, idx);
	ถ้า (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    bha_nga;
	ถ้า (filtermap != FILTERMAP_FOREACH)
	{
	    ถ้า (newtv.v_type != VAR_NUMBER && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		bha_nga;
	    }
	    ถ้า (filtermap != FILTERMAP_FILTER)
	    {
		ถ้า (newtv.vval.v_number != val)
		    blob_set(b_ret, i, newtv.vval.v_number);
	    }
	    มิฉะนั้น ถ้า (rem)
	    {
		char_u *p = (char_u *)blob_arg->bv_ga.ga_data;

		mch_memmove(p + i, p + i + 1,
			    (size_t)b->bv_ga.ga_len - i - 1);
		--b->bv_ga.ga_len;
		--i;
	    }
	}
	++idx;
    }

    b->bv_lock = prev_lock;
    ถ้า (fc != NULL)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    void
blob_insert_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    จำนวนเต็ม		error = FALSE;
    จำนวนเต็ม		val, len;
    char_u	*p;

    ถ้า (b == NULL)
    {
	ถ้า (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	คืน;
    }

    ถ้า (value_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	คืน;

    len = blob_len(b);
    ถ้า (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_number_chk(&argvars[2], &error);
	ถ้า (error)
	    คืน;		// type error; errmsg already given
	ถ้า (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    คืน;
	}
    }
    val = tv_get_number_chk(&argvars[1], &error);
    ถ้า (error)
	คืน;
    ถ้า (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	คืน;
    }

    ถ้า (ga_grow(&b->bv_ga, 1) == FAIL)
	คืน;
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove(p + before + 1, p + before, (size_t)len - before);
    *(p + before) = val;
    ++b->bv_ga.ga_len;

    copy_tv(&argvars[0], rettv);
}

/*
 * Extend "b1" with "b2".  "b1" must not be NULL.
 * If "bef" is equal to the length of b1 append at the end,
 * otherwise insert before this index.
 * Caller must check that "bef" is valid.
 * Returns FAIL when out of memory.
 */
    static จำนวนเต็ม
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    จำนวนเต็ม		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    ถ้า (b2 == NULL || b2->bv_ga.ga_len == 0)
	คืน OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    ถ้า (ga_grow(&b1->bv_ga, len2) == FAIL)
	คืน FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    ถ้า (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	ถ้า (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    มิฉะนั้น
    {
	ถ้า (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    คืน OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    void
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	จำนวนเต็ม		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    จำนวนเต็ม		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    ถ้า (b1 == NULL)
    {
	emsg(_(e_cannot_extend_null_blob));
	คืน;
    }
    ถ้า (is_new || !value_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	ถ้า (is_new)
	{
	    b1 = blob_copy(b1);
	    ถ้า (b1 == NULL)
		คืน;
	}

	b2 = argvars[1].vval.v_blob;
	ถ้า (b2 == NULL)
	    goto theend;

	ถ้า (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_number_chk(&argvars[2], &error);
	    ถ้า (error)
		goto cleanup;		// type error; errmsg already given
	    ถ้า (before < 0)
		before = b1->bv_ga.ga_len + before;
	    ถ้า (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		goto cleanup;
	    }
	}
	มิฉะนั้น
	    before = b1->bv_ga.ga_len;
	ถ้า (blob_extend(b1, b2, before) == FAIL)
	    goto cleanup;

theend:
	ถ้า (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	มิฉะนั้น
	    copy_tv(&argvars[0], rettv);
	คืน;

cleanup:
	ถ้า (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    void
blob_reduce(
	typval_T	*argvars,
	typval_T	*expr,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    จำนวนเต็ม		called_emsg_start = called_emsg;
    จำนวนเต็ม		r;
    typval_T	initial;
    typval_T	argv[3];
    จำนวนเต็ม	i;

    ถ้า (argvars[2].v_type == VAR_UNKNOWN)
    {
	ถ้า (b == NULL || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Blob");
	    คืน;
	}
	initial.v_type = VAR_NUMBER;
	initial.vval.v_number = blob_get(b, 0);
	i = 1;
    }
    มิฉะนั้น ถ้า (check_for_number_arg(argvars, 2) == FAIL)
	คืน;
    มิฉะนั้น
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    ถ้า (b == NULL)
	คืน;

    สำหรับ ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_NUMBER;
	argv[1].vval.v_number = blob_get(b, i);

	r = eval_expr_typval(expr, TRUE, argv, 2, NULL, rettv);

	clear_tv(&argv[0]);
	ถ้า (r == FAIL || called_emsg != called_emsg_start)
	    คืน;
    }
}

/*
 * "reverse({blob})" function
 */
    void
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    สำหรับ (i = 0; i < len / 2; i++)
    {
	จำนวนเต็ม tmp = blob_get(b, (จำนวนเต็ม)i);

	blob_set(b, (จำนวนเต็ม)i, blob_get(b, (จำนวนเต็ม)(len - i - 1)));
	blob_set(b, (จำนวนเต็ม)(len - i - 1), tmp);
    }
    rettv_blob_set(rettv, b);
}

/*
 * blob2list() function
 */
    void
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    จำนวนเต็ม		i, len;
    typval_T	tv;

    ถ้า (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(void)rettv_list_alloc(rettv);
	คืน;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_items(len);
    ถ้า (l == NULL)
	คืน;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_NUMBER;
    tv.v_lock = 0;
    สำหรับ (i = 0; i < len; i++)
    {
	tv.vval.v_number = blob_get(blob, i);
	list_set_item(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    void
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    ถ้า (rettv_blob_alloc(rettv) == FAIL)
	คืน;
    blob = rettv->vval.v_blob;

    ถ้า (check_for_list_arg(argvars, 0) == FAIL)
	คืน;

    l = argvars->vval.v_list;
    ถ้า (l == NULL)
	คืน;

    // The result has one byte per list item; reserve all space at once.
    ถ้า (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	คืน;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	จำนวนเต็ม		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_number_chk(&li->li_tv, &error);
	ถ้า (error == TRUE || n < 0 || n > 255)
	{
	    ถ้า (!error)
		semsg(_(e_invalid_value_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    คืน;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
