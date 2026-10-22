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

#ضمن "vim.h"

#אם defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(فراغ)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    אם (blob != عدم)
	ga_init2(&blob->bv_ga, 1, 100);
    החזר blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    صحيح
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    אם (b == عدم)
	החזר FAIL;

    rettv_blob_set(rettv, b);
    החזר OK;
}

/*
 * Set a blob as the return value.
 */
    فراغ
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    אם (b != عدم)
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
    صحيح		len;
    blob_T	*copy;

    אם (orig == عدم)
	החזר عدم;

    copy = blob_alloc();
    אם (copy == عدم)
	החזר عدم;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    אם (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	אם (copy->bv_ga.ga_data == عدم)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    החזר copy;
}

    فراغ
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    فراغ
blob_unref(blob_T *b)
{
    אם (b != عدم && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    אם (b == عدم)
	החזר 0L;
    החזר b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    صحيح
blob_get(blob_T *b, صحيح idx)
{
    החזר ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    فراغ
blob_set(blob_T *blob, صحيح idx, صحيح byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    فراغ
blob_set_append(blob_T *blob, صحيح idx, صحيح byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    אם (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	אם (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    صحيح
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    صحيح	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    אם (len1 == 0 && len2 == 0)
	החזר TRUE;
    אם (b1 == b2)
	החזר TRUE;
    אם (len1 != len2)
	החזר FALSE;

    עבור (i = 0; i < b1->bv_ga.ga_len; i++)
	אם (blob_get(b1, i) != blob_get(b2, i)) החזר FALSE;
    החזר TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    صحيح
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    بنية stat	st;
    صحيح		whence;
    off_T	size = size_arg;

    אם (fstat(fileno(fd), &st) < 0)
	החזר FAIL;  // can't read the file, error

    אם (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	אם (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    אחרת
    {
	// limit the offset to not go before the start of the file
	אם (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	אם (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    אם (size <= 0)
	החזר OK;
    אם (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	החזר OK;

    אם (ga_grow(&blob->bv_ga, (صحيح)size) == FAIL)
	החזר FAIL;
    blob->bv_ga.ga_len = (صحيح)size;
    אם (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = عدم;
	החזר FAIL;
    }
    החזר OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    صحيح
write_blob(FILE *fd, blob_T *blob)
{
    אם (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	החזר FAIL;
    }
    החזר OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    ساكن ثابت حرف hex_chars[] = "0123456789ABCDEF";
    صحيح		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    אם (blob == عدم || (blen = blob_len(blob)) == 0)
    {
	*tofree = عدم;
	החזר (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    אם (buf == عدم)
    {
	*tofree = عدم;
	החזר (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    עבור (صحيح i = 0; i < blen; i++)
    {
	unsigned b;

	אם (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    החזר buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    فراغ
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    אם (rettv_list_alloc(rettv) == FAIL)
	החזר;

    עבור (صحيح i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	אם (l2 == عدم)
	    החזר;

	אם (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    החזר;
	}

	אם (list_append_number(l2, i) == FAIL
		|| list_append_number(l2, blob_get(blob, i)) == FAIL)
	    החזר;
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

    אם (blob == عدم)
	החזר عدم;
    אם (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	اذهب failed;
    s += 2;
    כלעוד (vim_isxdigit(*s))
    {
	אם (!vim_isxdigit(s[1]))
	    اذهب failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	אם (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    אם (*skipwhite(s) != NUL)
	اذهب failed;  // text after final digit

    ++blob->bv_refcount;
    החזר blob;

failed:
    blob_free(blob);
    החזר عدم;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    ساكن صحيح
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	صحيح		exclusive,
	typval_T	*rettv)
{
    אם (n1 < 0)
    {
	n1 = len + n1;
	אם (n1 < 0)
	    n1 = 0;
    }
    אם (n2 < 0)
	n2 = len + n2;
    אחרת אם (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    אם (exclusive)
	--n2;
    אם (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = عدم;
    }
    אחרת
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	אם (new_blob != عدم)
	{
	    אם (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		החזר FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    עבור (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    החזר OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    ساكن صحيح
blob_index(
	blob_T		*blob,
	صحيح		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    אם (idx < 0)
	idx = len + idx;
    אם (idx < len && idx >= 0)
    {
	صحيح v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = v;
    }
    אחרת
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	החזר FAIL;
    }

    החזר OK;
}

    صحيح
blob_slice_or_index(
	blob_T		*blob,
	صحيح		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	صحيح		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    אם (is_range)
	החזר blob_slice(blob, len, n1, n2, exclusive, rettv);
    אחרת
	החזר blob_index(blob, len, n1, rettv);
    החזר OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    صحيح
check_blob_index(long bloblen, varnumber_T n1, صحيح quiet)
{
    אם (n1 < 0 || n1 > bloblen)
    {
	אם (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	החזר FAIL;
    }
    החזר OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    صحيح
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, صحيح quiet)
{
    אם (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	אם (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	החזר FAIL;
    }
    החזר OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    صحيح
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    صحيح	il, ir;

    אם (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_value_does_not_have_right_number_of_bytes));
	החזר FAIL;
    }

    ir = 0;
    עבור (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    החזר OK;
}

/*
 * "add(blob, item)" function
 */
    فراغ
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    صحيح		error = FALSE;
    varnumber_T n;

    אם (b == عدم)
    {
	אם (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	החזר;
    }

    אם (value_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	החזר;

    n = tv_get_number_chk(&argvars[1], &error);
    אם (error)
	החזר;

    ga_append(&b->bv_ga, (صحيح)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    فراغ
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    صحيح		error = FALSE;
    long	idx;
    long	end;
    صحيح		len;
    char_u	*p;

    אם (b != عدم && value_check_lock(b->bv_lock, arg_errmsg, TRUE))
	החזר;

    idx = (long)tv_get_number_chk(&argvars[1], &error);
    אם (error)
	החזר;

    len = blob_len(b);

    אם (idx < 0)
	// count from the end
	idx = len + idx;
    אם (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	החזר;
    }
    אם (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_number = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	החזר;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_number_chk(&argvars[2], &error);
    אם (error)
	החזר;
    אם (end < 0)
	// count from the end
	end = len + end;
    אם (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	החזר;
    }
    newblob = blob_alloc();
    אם (newblob == عدم)
	החזר;
    newblob->bv_ga.ga_len = end - idx + 1;
    אם (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	החזר;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    אם (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (size_t)(len - end - 1));
    b->bv_ga.ga_len -= end - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    فراغ
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*expr,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    صحيح		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    صحيح		idx = 0;
    صحيح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    אם (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = عدم;
    }
    אם (b == عدم || (filtermap == FILTERMAP_FILTER
			    && value_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	החזר;

    b_ret = b;
    אם (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	אם (rettv->vval.v_blob == عدم)
	    החזר;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    صحيح prev_lock = b->bv_lock;
    אם (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    עבור (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_NUMBER;
	val = blob_get(b, i);
	tv.vval.v_number = val;
	set_vim_var_nr(VV_KEY, idx);
	אם (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    اكسر;
	אם (filtermap != FILTERMAP_FOREACH)
	{
	    אם (newtv.v_type != VAR_NUMBER && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		اكسر;
	    }
	    אם (filtermap != FILTERMAP_FILTER)
	    {
		אם (newtv.vval.v_number != val)
		    blob_set(b_ret, i, newtv.vval.v_number);
	    }
	    אחרת אם (rem)
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
    אם (fc != عدم)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    فراغ
blob_insert_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    صحيح		error = FALSE;
    صحيح		val, len;
    char_u	*p;

    אם (b == عدم)
    {
	אם (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	החזר;
    }

    אם (value_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	החזר;

    len = blob_len(b);
    אם (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_number_chk(&argvars[2], &error);
	אם (error)
	    החזר;		// type error; errmsg already given
	אם (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    החזר;
	}
    }
    val = tv_get_number_chk(&argvars[1], &error);
    אם (error)
	החזר;
    אם (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	החזר;
    }

    אם (ga_grow(&b->bv_ga, 1) == FAIL)
	החזר;
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
    ساكن صحيح
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    صحيح		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    אם (b2 == عدم || b2->bv_ga.ga_len == 0)
	החזר OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    אם (ga_grow(&b1->bv_ga, len2) == FAIL)
	החזר FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    אם (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	אם (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    אחרת
    {
	אם (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    החזר OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    فراغ
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	صحيح		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    صحيح		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    אם (b1 == عدم)
    {
	emsg(_(e_cannot_extend_null_blob));
	החזר;
    }
    אם (is_new || !value_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	אם (is_new)
	{
	    b1 = blob_copy(b1);
	    אם (b1 == عدم)
		החזר;
	}

	b2 = argvars[1].vval.v_blob;
	אם (b2 == عدم)
	    اذهب theend;

	אם (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_number_chk(&argvars[2], &error);
	    אם (error)
		اذهب cleanup;		// type error; errmsg already given
	    אם (before < 0)
		before = b1->bv_ga.ga_len + before;
	    אם (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		اذهب cleanup;
	    }
	}
	אחרת
	    before = b1->bv_ga.ga_len;
	אם (blob_extend(b1, b2, before) == FAIL)
	    اذهب cleanup;

theend:
	אם (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	אחרת
	    copy_tv(&argvars[0], rettv);
	החזר;

cleanup:
	אם (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    فراغ
blob_reduce(
	typval_T	*argvars,
	typval_T	*expr,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    صحيح		called_emsg_start = called_emsg;
    صحيح		r;
    typval_T	initial;
    typval_T	argv[3];
    صحيح	i;

    אם (argvars[2].v_type == VAR_UNKNOWN)
    {
	אם (b == عدم || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Blob");
	    החזר;
	}
	initial.v_type = VAR_NUMBER;
	initial.vval.v_number = blob_get(b, 0);
	i = 1;
    }
    אחרת אם (check_for_number_arg(argvars, 2) == FAIL)
	החזר;
    אחרת
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    אם (b == عدم)
	החזר;

    עבור ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_NUMBER;
	argv[1].vval.v_number = blob_get(b, i);

	r = eval_expr_typval(expr, TRUE, argv, 2, عدم, rettv);

	clear_tv(&argv[0]);
	אם (r == FAIL || called_emsg != called_emsg_start)
	    החזר;
    }
}

/*
 * "reverse({blob})" function
 */
    فراغ
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    עבור (i = 0; i < len / 2; i++)
    {
	صحيح tmp = blob_get(b, (صحيح)i);

	blob_set(b, (صحيح)i, blob_get(b, (صحيح)(len - i - 1)));
	blob_set(b, (صحيح)(len - i - 1), tmp);
    }
    rettv_blob_set(rettv, b);
}

/*
 * blob2list() function
 */
    فراغ
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    صحيح		i, len;
    typval_T	tv;

    אם (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(فراغ)rettv_list_alloc(rettv);
	החזר;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_items(len);
    אם (l == عدم)
	החזר;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_NUMBER;
    tv.v_lock = 0;
    עבור (i = 0; i < len; i++)
    {
	tv.vval.v_number = blob_get(blob, i);
	list_set_item(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    فراغ
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    אם (rettv_blob_alloc(rettv) == FAIL)
	החזר;
    blob = rettv->vval.v_blob;

    אם (check_for_list_arg(argvars, 0) == FAIL)
	החזר;

    l = argvars->vval.v_list;
    אם (l == عدم)
	החזר;

    // The result has one byte per list item; reserve all space at once.
    אם (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	החזר;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	صحيح		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_number_chk(&li->li_tv, &error);
	אם (error == TRUE || n < 0 || n > 255)
	{
	    אם (!error)
		semsg(_(e_invalid_value_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    החזר;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
