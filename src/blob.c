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

#إذا defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(فراغ)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    إذا (blob != عدم)
	ga_init2(&blob->bv_ga, 1, 100);
    أرجع blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    صحيح
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    إذا (b == عدم)
	أرجع FAIL;

    rettv_blob_set(rettv, b);
    أرجع OK;
}

/*
 * Set a blob as the return value.
 */
    فراغ
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    إذا (b != عدم)
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

    إذا (orig == عدم)
	أرجع عدم;

    copy = blob_alloc();
    إذا (copy == عدم)
	أرجع عدم;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    إذا (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	إذا (copy->bv_ga.ga_data == عدم)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    أرجع copy;
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
    إذا (b != عدم && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    إذا (b == عدم)
	أرجع 0L;
    أرجع b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    صحيح
blob_get(blob_T *b, صحيح idx)
{
    أرجع ((char_u*)b->bv_ga.ga_data)[idx];
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
    إذا (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	إذا (idx == gap->ga_len)
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
    إذا (len1 == 0 && len2 == 0)
	أرجع TRUE;
    إذا (b1 == b2)
	أرجع TRUE;
    إذا (len1 != len2)
	أرجع FALSE;

    لكل (i = 0; i < b1->bv_ga.ga_len; i++)
	إذا (blob_get(b1, i) != blob_get(b2, i)) أرجع FALSE;
    أرجع TRUE;
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

    إذا (fstat(fileno(fd), &st) < 0)
	أرجع FAIL;  // can't read the file, error

    إذا (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	إذا (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    وإلا
    {
	// limit the offset to not go before the start of the file
	إذا (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	إذا (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    إذا (size <= 0)
	أرجع OK;
    إذا (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	أرجع OK;

    إذا (ga_grow(&blob->bv_ga, (صحيح)size) == FAIL)
	أرجع FAIL;
    blob->bv_ga.ga_len = (صحيح)size;
    إذا (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = عدم;
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    صحيح
write_blob(FILE *fd, blob_T *blob)
{
    إذا (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	أرجع FAIL;
    }
    أرجع OK;
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

    إذا (blob == عدم || (blen = blob_len(blob)) == 0)
    {
	*tofree = عدم;
	أرجع (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    إذا (buf == عدم)
    {
	*tofree = عدم;
	أرجع (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    لكل (صحيح i = 0; i < blen; i++)
    {
	unsigned b;

	إذا (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    أرجع buf;
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

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    لكل (صحيح i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	إذا (l2 == عدم)
	    أرجع;

	إذا (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    أرجع;
	}

	إذا (list_append_نومبور(l2, i) == FAIL
		|| list_append_نومبور(l2, blob_get(blob, i)) == FAIL)
	    أرجع;
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

    إذا (blob == عدم)
	أرجع عدم;
    إذا (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	اذهب failed;
    s += 2;
    طالما (vim_isxdigit(*s))
    {
	إذا (!vim_isxdigit(s[1]))
	    اذهب failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	إذا (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    إذا (*skipwhite(s) != NUL)
	اذهب failed;  // text after final digit

    ++blob->bv_refcount;
    أرجع blob;

failed:
    blob_free(blob);
    أرجع عدم;
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
    إذا (n1 < 0)
    {
	n1 = len + n1;
	إذا (n1 < 0)
	    n1 = 0;
    }
    إذا (n2 < 0)
	n2 = len + n2;
    وإلا إذا (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    إذا (exclusive)
	--n2;
    إذا (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = عدم;
    }
    وإلا
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	إذا (new_blob != عدم)
	{
	    إذا (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		أرجع FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    لكل (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    أرجع OK;
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
    إذا (idx < 0)
	idx = len + idx;
    إذا (idx < len && idx >= 0)
    {
	صحيح v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_نومبور;
	rettv->vval.v_نومبور = v;
    }
    وإلا
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	أرجع FAIL;
    }

    أرجع OK;
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

    إذا (is_range)
	أرجع blob_slice(blob, len, n1, n2, exclusive, rettv);
    وإلا
	أرجع blob_index(blob, len, n1, rettv);
    أرجع OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    صحيح
check_blob_index(long bloblen, varnumber_T n1, صحيح quiet)
{
    إذا (n1 < 0 || n1 > bloblen)
    {
	إذا (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    صحيح
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, صحيح quiet)
{
    إذا (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	إذا (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	أرجع FAIL;
    }
    أرجع OK;
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

    إذا (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_نيلاي_does_not_have_کانن_نومبور_of_bytes));
	أرجع FAIL;
    }

    ir = 0;
    لكل (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    أرجع OK;
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

    إذا (b == عدم)
    {
	إذا (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	أرجع;
    }

    إذا (نيلاي_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	أرجع;

    n = tv_get_نومبور_chk(&argvars[1], &error);
    إذا (error)
	أرجع;

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

    إذا (b != عدم && نيلاي_check_lock(b->bv_lock, arg_errmsg, TRUE))
	أرجع;

    idx = (long)tv_get_نومبور_chk(&argvars[1], &error);
    إذا (error)
	أرجع;

    len = blob_len(b);

    إذا (idx < 0)
	// count from the end
	idx = len + idx;
    إذا (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	أرجع;
    }
    إذا (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_نومبور = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	أرجع;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_نومبور_chk(&argvars[2], &error);
    إذا (error)
	أرجع;
    إذا (end < 0)
	// count from the end
	end = len + end;
    إذا (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	أرجع;
    }
    newblob = blob_alloc();
    إذا (newblob == عدم)
	أرجع;
    newblob->bv_ga.ga_len = end - idx + 1;
    إذا (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	أرجع;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    إذا (len - end - 1 > 0)
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
	typval_T	*اوڠکاڤن,
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

    إذا (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = عدم;
    }
    إذا (b == عدم || (filtermap == FILTERMAP_FILTER
			    && نيلاي_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	أرجع;

    b_ret = b;
    إذا (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	إذا (rettv->vval.v_blob == عدم)
	    أرجع;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_کونچي, VAR_نومبور);

    صحيح prev_lock = b->bv_lock;
    إذا (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اوڠکاڤن_get_funccal(اوڠکاڤن, &newtv);

    لكل (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_نومبور;
	val = blob_get(b, i);
	tv.vval.v_نومبور = val;
	set_vim_var_nr(VV_کونچي, idx);
	إذا (filter_map_one(&tv, اوڠکاڤن, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    اكسر;
	إذا (filtermap != FILTERMAP_FOREACH)
	{
	    إذا (newtv.v_type != VAR_نومبور && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		اكسر;
	    }
	    إذا (filtermap != FILTERMAP_FILTER)
	    {
		إذا (newtv.vval.v_نومبور != val)
		    blob_set(b_ret, i, newtv.vval.v_نومبور);
	    }
	    وإلا إذا (rem)
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
    إذا (fc != عدم)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    فراغ
blob_سيسيڤ_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    صحيح		error = FALSE;
    صحيح		val, len;
    char_u	*p;

    إذا (b == عدم)
    {
	إذا (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	أرجع;
    }

    إذا (نيلاي_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	أرجع;

    len = blob_len(b);
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_نومبور_chk(&argvars[2], &error);
	إذا (error)
	    أرجع;		// type error; errmsg already given
	إذا (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    أرجع;
	}
    }
    val = tv_get_نومبور_chk(&argvars[1], &error);
    إذا (error)
	أرجع;
    إذا (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	أرجع;
    }

    إذا (ga_grow(&b->bv_ga, 1) == FAIL)
	أرجع;
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
    إذا (b2 == عدم || b2->bv_ga.ga_len == 0)
	أرجع OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    إذا (ga_grow(&b1->bv_ga, len2) == FAIL)
	أرجع FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    إذا (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	إذا (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    وإلا
    {
	إذا (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    أرجع OK;
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
    إذا (b1 == عدم)
    {
	emsg(_(e_cannot_extend_null_blob));
	أرجع;
    }
    إذا (is_new || !نيلاي_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	إذا (is_new)
	{
	    b1 = blob_copy(b1);
	    إذا (b1 == عدم)
		أرجع;
	}

	b2 = argvars[1].vval.v_blob;
	إذا (b2 == عدم)
	    اذهب theend;

	إذا (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_نومبور_chk(&argvars[2], &error);
	    إذا (error)
		اذهب cleanup;		// type error; errmsg already given
	    إذا (before < 0)
		before = b1->bv_ga.ga_len + before;
	    إذا (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		اذهب cleanup;
	    }
	}
	وإلا
	    before = b1->bv_ga.ga_len;
	إذا (blob_extend(b1, b2, before) == FAIL)
	    اذهب cleanup;

theend:
	إذا (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	وإلا
	    copy_tv(&argvars[0], rettv);
	أرجع;

cleanup:
	إذا (is_new)
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
	typval_T	*اوڠکاڤن,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    صحيح		called_emsg_start = called_emsg;
    صحيح		r;
    typval_T	initial;
    typval_T	argv[3];
    صحيح	i;

    إذا (argvars[2].v_type == VAR_UNKNOWN)
    {
	إذا (b == عدم || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_نيلاي), "Blob");
	    أرجع;
	}
	initial.v_type = VAR_نومبور;
	initial.vval.v_نومبور = blob_get(b, 0);
	i = 1;
    }
    وإلا إذا (check_for_نومبور_arg(argvars, 2) == FAIL)
	أرجع;
    وإلا
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    إذا (b == عدم)
	أرجع;

    لكل ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_نومبور;
	argv[1].vval.v_نومبور = blob_get(b, i);

	r = eval_اوڠکاڤن_typval(اوڠکاڤن, TRUE, argv, 2, عدم, rettv);

	clear_tv(&argv[0]);
	إذا (r == FAIL || called_emsg != called_emsg_start)
	    أرجع;
    }
}

/*
 * "reverse({blob})" function
 */
    فراغ
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    لكل (i = 0; i < len / 2; i++)
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

    إذا (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(فراغ)rettv_list_alloc(rettv);
	أرجع;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_بارڠن(len);
    إذا (l == عدم)
	أرجع;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_نومبور;
    tv.v_lock = 0;
    لكل (i = 0; i < len; i++)
    {
	tv.vval.v_نومبور = blob_get(blob, i);
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

    إذا (rettv_blob_alloc(rettv) == FAIL)
	أرجع;
    blob = rettv->vval.v_blob;

    إذا (check_for_list_arg(argvars, 0) == FAIL)
	أرجع;

    l = argvars->vval.v_list;
    إذا (l == عدم)
	أرجع;

    // The result has one byte per list item; reserve all space at once.
    إذا (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	أرجع;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_بارڠن(l, li)
    {
	صحيح		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_نومبور_chk(&li->li_tv, &error);
	إذا (error == TRUE || n < 0 || n > 255)
	{
	    إذا (!error)
		semsg(_(e_invalid_نيلاي_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    أرجع;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
