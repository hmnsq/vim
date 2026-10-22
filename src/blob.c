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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(تهی)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    اگر (blob != پوچ)
	ga_init2(&blob->bv_ga, 1, 100);
    بازگشت blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    صحیح
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    اگر (b == پوچ)
	بازگشت FAIL;

    rettv_blob_set(rettv, b);
    بازگشت OK;
}

/*
 * Set a blob as the return value.
 */
    تهی
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    اگر (b != پوچ)
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
    صحیح		len;
    blob_T	*copy;

    اگر (orig == پوچ)
	بازگشت پوچ;

    copy = blob_alloc();
    اگر (copy == پوچ)
	بازگشت پوچ;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    اگر (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	اگر (copy->bv_ga.ga_data == پوچ)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    بازگشت copy;
}

    تهی
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    تهی
blob_unref(blob_T *b)
{
    اگر (b != پوچ && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    اگر (b == پوچ)
	بازگشت 0L;
    بازگشت b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    صحیح
blob_get(blob_T *b, صحیح idx)
{
    بازگشت ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    تهی
blob_set(blob_T *blob, صحیح idx, صحیح byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    تهی
blob_set_append(blob_T *blob, صحیح idx, صحیح byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    اگر (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	اگر (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    صحیح
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    صحیح	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    اگر (len1 == 0 && len2 == 0)
	بازگشت TRUE;
    اگر (b1 == b2)
	بازگشت TRUE;
    اگر (len1 != len2)
	بازگشت FALSE;

    برای (i = 0; i < b1->bv_ga.ga_len; i++)
	اگر (blob_get(b1, i) != blob_get(b2, i)) بازگشت FALSE;
    بازگشت TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    صحیح
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    ساختار stat	st;
    صحیح		whence;
    off_T	size = size_arg;

    اگر (fstat(fileno(fd), &st) < 0)
	بازگشت FAIL;  // can't read the file, error

    اگر (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	اگر (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    وگرنه
    {
	// limit the offset to not go before the start of the file
	اگر (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	اگر (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    اگر (size <= 0)
	بازگشت OK;
    اگر (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	بازگشت OK;

    اگر (ga_grow(&blob->bv_ga, (صحیح)size) == FAIL)
	بازگشت FAIL;
    blob->bv_ga.ga_len = (صحیح)size;
    اگر (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = پوچ;
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    صحیح
write_blob(FILE *fd, blob_T *blob)
{
    اگر (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    ایستا ثابت نویسه hex_chars[] = "0123456789ABCDEF";
    صحیح		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    اگر (blob == پوچ || (blen = blob_len(blob)) == 0)
    {
	*tofree = پوچ;
	بازگشت (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    اگر (buf == پوچ)
    {
	*tofree = پوچ;
	بازگشت (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    برای (صحیح i = 0; i < blen; i++)
    {
	unsigned b;

	اگر (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    بازگشت buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    تهی
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    برای (صحیح i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	اگر (l2 == پوچ)
	    بازگشت;

	اگر (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    بازگشت;
	}

	اگر (list_append_عدد(l2, i) == FAIL
		|| list_append_عدد(l2, blob_get(blob, i)) == FAIL)
	    بازگشت;
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

    اگر (blob == پوچ)
	بازگشت پوچ;
    اگر (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	برو failed;
    s += 2;
    وقتی (vim_isxdigit(*s))
    {
	اگر (!vim_isxdigit(s[1]))
	    برو failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	اگر (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    اگر (*skipwhite(s) != NUL)
	برو failed;  // text after final digit

    ++blob->bv_refcount;
    بازگشت blob;

failed:
    blob_free(blob);
    بازگشت پوچ;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    ایستا صحیح
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	صحیح		exclusive,
	typval_T	*rettv)
{
    اگر (n1 < 0)
    {
	n1 = len + n1;
	اگر (n1 < 0)
	    n1 = 0;
    }
    اگر (n2 < 0)
	n2 = len + n2;
    وگرنه اگر (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    اگر (exclusive)
	--n2;
    اگر (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = پوچ;
    }
    وگرنه
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	اگر (new_blob != پوچ)
	{
	    اگر (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		بازگشت FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    برای (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    بازگشت OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    ایستا صحیح
blob_index(
	blob_T		*blob,
	صحیح		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    اگر (idx < 0)
	idx = len + idx;
    اگر (idx < len && idx >= 0)
    {
	صحیح v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_عدد;
	rettv->vval.v_عدد = v;
    }
    وگرنه
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	بازگشت FAIL;
    }

    بازگشت OK;
}

    صحیح
blob_slice_or_index(
	blob_T		*blob,
	صحیح		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	صحیح		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    اگر (is_range)
	بازگشت blob_slice(blob, len, n1, n2, exclusive, rettv);
    وگرنه
	بازگشت blob_index(blob, len, n1, rettv);
    بازگشت OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    صحیح
check_blob_index(long bloblen, varnumber_T n1, صحیح quiet)
{
    اگر (n1 < 0 || n1 > bloblen)
    {
	اگر (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    صحیح
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, صحیح quiet)
{
    اگر (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	اگر (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    صحیح
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    صحیح	il, ir;

    اگر (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_مقدار_does_not_have_راست_عدد_of_bytes));
	بازگشت FAIL;
    }

    ir = 0;
    برای (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    بازگشت OK;
}

/*
 * "add(blob, item)" function
 */
    تهی
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    صحیح		error = FALSE;
    varnumber_T n;

    اگر (b == پوچ)
    {
	اگر (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	بازگشت;
    }

    اگر (مقدار_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	بازگشت;

    n = tv_get_عدد_chk(&argvars[1], &error);
    اگر (error)
	بازگشت;

    ga_append(&b->bv_ga, (صحیح)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    تهی
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    صحیح		error = FALSE;
    long	idx;
    long	end;
    صحیح		len;
    char_u	*p;

    اگر (b != پوچ && مقدار_check_lock(b->bv_lock, arg_errmsg, TRUE))
	بازگشت;

    idx = (long)tv_get_عدد_chk(&argvars[1], &error);
    اگر (error)
	بازگشت;

    len = blob_len(b);

    اگر (idx < 0)
	// count from the end
	idx = len + idx;
    اگر (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	بازگشت;
    }
    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_عدد = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	بازگشت;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_عدد_chk(&argvars[2], &error);
    اگر (error)
	بازگشت;
    اگر (end < 0)
	// count from the end
	end = len + end;
    اگر (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	بازگشت;
    }
    newblob = blob_alloc();
    اگر (newblob == پوچ)
	بازگشت;
    newblob->bv_ga.ga_len = end - idx + 1;
    اگر (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	بازگشت;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    اگر (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (size_t)(len - end - 1));
    b->bv_ga.ga_len -= end - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    تهی
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*عبارت,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    صحیح		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    صحیح		idx = 0;
    صحیح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = پوچ;
    }
    اگر (b == پوچ || (filtermap == FILTERMAP_FILTER
			    && مقدار_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	بازگشت;

    b_ret = b;
    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	اگر (rettv->vval.v_blob == پوچ)
	    بازگشت;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_کلید, VAR_عدد);

    صحیح prev_lock = b->bv_lock;
    اگر (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_عبارت_get_funccal(عبارت, &newtv);

    برای (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_عدد;
	val = blob_get(b, i);
	tv.vval.v_عدد = val;
	set_vim_var_nr(VV_کلید, idx);
	اگر (filter_map_one(&tv, عبارت, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    بشکن;
	اگر (filtermap != FILTERMAP_FOREACH)
	{
	    اگر (newtv.v_type != VAR_عدد && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		بشکن;
	    }
	    اگر (filtermap != FILTERMAP_FILTER)
	    {
		اگر (newtv.vval.v_عدد != val)
		    blob_set(b_ret, i, newtv.vval.v_عدد);
	    }
	    وگرنه اگر (rem)
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
    اگر (fc != پوچ)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    تهی
blob_درج_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    صحیح		error = FALSE;
    صحیح		val, len;
    char_u	*p;

    اگر (b == پوچ)
    {
	اگر (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	بازگشت;
    }

    اگر (مقدار_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	بازگشت;

    len = blob_len(b);
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_عدد_chk(&argvars[2], &error);
	اگر (error)
	    بازگشت;		// type error; errmsg already given
	اگر (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    بازگشت;
	}
    }
    val = tv_get_عدد_chk(&argvars[1], &error);
    اگر (error)
	بازگشت;
    اگر (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	بازگشت;
    }

    اگر (ga_grow(&b->bv_ga, 1) == FAIL)
	بازگشت;
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
    ایستا صحیح
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    صحیح		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    اگر (b2 == پوچ || b2->bv_ga.ga_len == 0)
	بازگشت OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    اگر (ga_grow(&b1->bv_ga, len2) == FAIL)
	بازگشت FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    اگر (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	اگر (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    وگرنه
    {
	اگر (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    بازگشت OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    تهی
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	صحیح		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    صحیح		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    اگر (b1 == پوچ)
    {
	emsg(_(e_cannot_extend_null_blob));
	بازگشت;
    }
    اگر (is_new || !مقدار_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	اگر (is_new)
	{
	    b1 = blob_copy(b1);
	    اگر (b1 == پوچ)
		بازگشت;
	}

	b2 = argvars[1].vval.v_blob;
	اگر (b2 == پوچ)
	    برو theend;

	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_عدد_chk(&argvars[2], &error);
	    اگر (error)
		برو cleanup;		// type error; errmsg already given
	    اگر (before < 0)
		before = b1->bv_ga.ga_len + before;
	    اگر (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		برو cleanup;
	    }
	}
	وگرنه
	    before = b1->bv_ga.ga_len;
	اگر (blob_extend(b1, b2, before) == FAIL)
	    برو cleanup;

theend:
	اگر (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	وگرنه
	    copy_tv(&argvars[0], rettv);
	بازگشت;

cleanup:
	اگر (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    تهی
blob_reduce(
	typval_T	*argvars,
	typval_T	*عبارت,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    صحیح		called_emsg_start = called_emsg;
    صحیح		r;
    typval_T	initial;
    typval_T	argv[3];
    صحیح	i;

    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	اگر (b == پوچ || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_مقدار), "Blob");
	    بازگشت;
	}
	initial.v_type = VAR_عدد;
	initial.vval.v_عدد = blob_get(b, 0);
	i = 1;
    }
    وگرنه اگر (check_for_عدد_arg(argvars, 2) == FAIL)
	بازگشت;
    وگرنه
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    اگر (b == پوچ)
	بازگشت;

    برای ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_عدد;
	argv[1].vval.v_عدد = blob_get(b, i);

	r = eval_عبارت_typval(عبارت, TRUE, argv, 2, پوچ, rettv);

	clear_tv(&argv[0]);
	اگر (r == FAIL || called_emsg != called_emsg_start)
	    بازگشت;
    }
}

/*
 * "reverse({blob})" function
 */
    تهی
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    برای (i = 0; i < len / 2; i++)
    {
	صحیح tmp = blob_get(b, (صحیح)i);

	blob_set(b, (صحیح)i, blob_get(b, (صحیح)(len - i - 1)));
	blob_set(b, (صحیح)(len - i - 1), tmp);
    }
    rettv_blob_set(rettv, b);
}

/*
 * blob2list() function
 */
    تهی
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    صحیح		i, len;
    typval_T	tv;

    اگر (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(تهی)rettv_list_alloc(rettv);
	بازگشت;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_اقلام(len);
    اگر (l == پوچ)
	بازگشت;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_عدد;
    tv.v_lock = 0;
    برای (i = 0; i < len; i++)
    {
	tv.vval.v_عدد = blob_get(blob, i);
	list_set_item(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    تهی
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    اگر (rettv_blob_alloc(rettv) == FAIL)
	بازگشت;
    blob = rettv->vval.v_blob;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	بازگشت;

    l = argvars->vval.v_list;
    اگر (l == پوچ)
	بازگشت;

    // The result has one byte per list item; reserve all space at once.
    اگر (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	بازگشت;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اقلام(l, li)
    {
	صحیح		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_عدد_chk(&li->li_tv, &error);
	اگر (error == TRUE || n < 0 || n > 255)
	{
	    اگر (!error)
		semsg(_(e_invalid_مقدار_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    بازگشت;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
