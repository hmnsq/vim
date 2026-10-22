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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(শূন্য)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    যদি (blob != ফাঁকা)
	ga_init2(&blob->bv_ga, 1, 100);
    ফেরত blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    যদি (b == ফাঁকা)
	ফেরত FAIL;

    rettv_blob_set(rettv, b);
    ফেরত OK;
}

/*
 * Set a blob as the return value.
 */
    শূন্য
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    যদি (b != ফাঁকা)
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
    পূর্ণসংখ্যা		len;
    blob_T	*copy;

    যদি (orig == ফাঁকা)
	ফেরত ফাঁকা;

    copy = blob_alloc();
    যদি (copy == ফাঁকা)
	ফেরত ফাঁকা;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    যদি (len > 0)
    {
	copy->bv_ga.ga_তথ্য =
	    vim_memsave(orig->bv_ga.ga_তথ্য, len);
	যদি (copy->bv_ga.ga_তথ্য == ফাঁকা)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    ফেরত copy;
}

    শূন্য
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    শূন্য
blob_unref(blob_T *b)
{
    যদি (b != ফাঁকা && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    যদি (b == ফাঁকা)
	ফেরত 0L;
    ফেরত b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    পূর্ণসংখ্যা
blob_get(blob_T *b, পূর্ণসংখ্যা idx)
{
    ফেরত ((char_u*)b->bv_ga.ga_তথ্য)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    শূন্য
blob_set(blob_T *blob, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা byte)
{
    ((char_u*)blob->bv_ga.ga_তথ্য)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    শূন্য
blob_set_append(blob_T *blob, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    যদি (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	যদি (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    পূর্ণসংখ্যা
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    পূর্ণসংখ্যা	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    যদি (len1 == 0 && len2 == 0)
	ফেরত TRUE;
    যদি (b1 == b2)
	ফেরত TRUE;
    যদি (len1 != len2)
	ফেরত FALSE;

    জন্য (i = 0; i < b1->bv_ga.ga_len; i++)
	যদি (blob_get(b1, i) != blob_get(b2, i)) ফেরত FALSE;
    ফেরত TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    পূর্ণসংখ্যা
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T আকার_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    কাঠামো stat	st;
    পূর্ণসংখ্যা		whence;
    off_T	আকার_2 = আকার_arg;

    যদি (fstat(fileno(fd), &st) < 0)
	ফেরত FAIL;  // can't read the file, error

    যদি (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	যদি (আকার_2 == -1 || (আকার_2 > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    আকার_2 = st.st_size - offset;
	whence = SEEK_SET;
    }
    অন্যথা
    {
	// limit the offset to not go before the start of the file
	যদি (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	যদি (আকার_2 == -1 || আকার_2 > -offset)
	    আকার_2 = -offset;
	whence = SEEK_END;
    }
    যদি (আকার_2 <= 0)
	ফেরত OK;
    যদি (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	ফেরত OK;

    যদি (ga_grow(&blob->bv_ga, (পূর্ণসংখ্যা)আকার_2) == FAIL)
	ফেরত FAIL;
    blob->bv_ga.ga_len = (পূর্ণসংখ্যা)আকার_2;
    যদি (fread(blob->bv_ga.ga_তথ্য, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = ফাঁকা;
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    পূর্ণসংখ্যা
write_blob(FILE *fd, blob_T *blob)
{
    যদি (fwrite(blob->bv_ga.ga_তথ্য, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    স্থির ধ্রুবক অক্ষর hex_chars[] = "0123456789ABCDEF";
    পূর্ণসংখ্যা		blen;
    size_t	যোগ;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    যদি (blob == ফাঁকা || (blen = blob_len(blob)) == 0)
    {
	*tofree = ফাঁকা;
	ফেরত (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    যোগ = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(যোগ);
    যদি (buf == ফাঁকা)
    {
	*tofree = ফাঁকা;
	ফেরত (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_তথ্য;
    জন্য (পূর্ণসংখ্যা i = 0; i < blen; i++)
    {
	unsigned b;

	যদি (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    ফেরত buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    শূন্য
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    জন্য (পূর্ণসংখ্যা i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	যদি (l2 == ফাঁকা)
	    ফেরত;

	যদি (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ফেরত;
	}

	যদি (list_append_সংখ্যা(l2, i) == FAIL
		|| list_append_সংখ্যা(l2, blob_get(blob, i)) == FAIL)
	    ফেরত;
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

    যদি (blob == ফাঁকা)
	ফেরত ফাঁকা;
    যদি (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	যাও failed;
    s += 2;
    যখন (vim_isxdigit(*s))
    {
	যদি (!vim_isxdigit(s[1]))
	    যাও failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	যদি (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    যদি (*skipwhite(s) != NUL)
	যাও failed;  // text after final digit

    ++blob->bv_refcount;
    ফেরত blob;

failed:
    blob_free(blob);
    ফেরত ফাঁকা;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    স্থির পূর্ণসংখ্যা
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	পূর্ণসংখ্যা		exclusive,
	typval_T	*rettv)
{
    যদি (n1 < 0)
    {
	n1 = len + n1;
	যদি (n1 < 0)
	    n1 = 0;
    }
    যদি (n2 < 0)
	n2 = len + n2;
    অন্যথা যদি (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    যদি (exclusive)
	--n2;
    যদি (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ফাঁকা;
    }
    অন্যথা
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	যদি (new_blob != ফাঁকা)
	{
	    যদি (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		ফেরত FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    জন্য (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    ফেরত OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    স্থির পূর্ণসংখ্যা
blob_সূচক(
	blob_T		*blob,
	পূর্ণসংখ্যা		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    যদি (idx < 0)
	idx = len + idx;
    যদি (idx < len && idx >= 0)
    {
	পূর্ণসংখ্যা v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_সংখ্যা;
	rettv->vval.v_সংখ্যা = v;
    }
    অন্যথা
    {
	semsg(_(e_blob_সূচক_out_of_range_nr), idx);
	ফেরত FAIL;
    }

    ফেরত OK;
}

    পূর্ণসংখ্যা
blob_slice_or_সূচক(
	blob_T		*blob,
	পূর্ণসংখ্যা		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	পূর্ণসংখ্যা		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    যদি (is_range)
	ফেরত blob_slice(blob, len, n1, n2, exclusive, rettv);
    অন্যথা
	ফেরত blob_সূচক(blob, len, n1, rettv);
    ফেরত OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    পূর্ণসংখ্যা
check_blob_সূচক(long bloblen, varnumber_T n1, পূর্ণসংখ্যা quiet)
{
    যদি (n1 < 0 || n1 > bloblen)
    {
	যদি (!quiet)
	    semsg(_(e_blob_সূচক_out_of_range_nr), n1);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    পূর্ণসংখ্যা
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, পূর্ণসংখ্যা quiet)
{
    যদি (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	যদি (!quiet)
	    semsg(_(e_blob_সূচক_out_of_range_nr), n2);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    পূর্ণসংখ্যা
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    পূর্ণসংখ্যা	il, ir;

    যদি (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_মান_does_not_have_ডান_সংখ্যা_of_bytes));
	ফেরত FAIL;
    }

    ir = 0;
    জন্য (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    ফেরত OK;
}

/*
 * "add(blob, item)" function
 */
    শূন্য
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    পূর্ণসংখ্যা		error = FALSE;
    varnumber_T n;

    যদি (b == ফাঁকা)
    {
	যদি (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	ফেরত;
    }

    যদি (মান_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	ফেরত;

    n = tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (error)
	ফেরত;

    ga_append(&b->bv_ga, (পূর্ণসংখ্যা)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    শূন্য
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    পূর্ণসংখ্যা		error = FALSE;
    long	idx;
    long	end;
    পূর্ণসংখ্যা		len;
    char_u	*p;

    যদি (b != ফাঁকা && মান_check_lock(b->bv_lock, arg_errmsg, TRUE))
	ফেরত;

    idx = (long)tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (error)
	ফেরত;

    len = blob_len(b);

    যদি (idx < 0)
	// count from the end
	idx = len + idx;
    যদি (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_সূচক_out_of_range_nr), idx);
	ফেরত;
    }
    যদি (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_তথ্য;
	rettv->vval.v_সংখ্যা = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	ফেরত;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_সংখ্যা_chk(&argvars[2], &error);
    যদি (error)
	ফেরত;
    যদি (end < 0)
	// count from the end
	end = len + end;
    যদি (end >= len || idx > end)
    {
	semsg(_(e_blob_সূচক_out_of_range_nr), end);
	ফেরত;
    }
    newblob = blob_alloc();
    যদি (newblob == ফাঁকা)
	ফেরত;
    newblob->bv_ga.ga_len = end - idx + 1;
    যদি (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	ফেরত;
    }
    p = (char_u *)b->bv_ga.ga_তথ্য;
    mch_memmove((char_u *)newblob->bv_ga.ga_তথ্য, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    যদি (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (size_t)(len - end - 1));
    b->bv_ga.ga_len -= end - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    শূন্য
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*রাশিমালা,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    পূর্ণসংখ্যা		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    পূর্ণসংখ্যা		idx = 0;
    পূর্ণসংখ্যা		rem;
    typval_T	newtv;
    funccall_T	*fc;

    যদি (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ফাঁকা;
    }
    যদি (b == ফাঁকা || (filtermap == FILTERMAP_FILTER
			    && মান_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	ফেরত;

    b_ret = b;
    যদি (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	যদি (rettv->vval.v_blob == ফাঁকা)
	    ফেরত;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_চাবি, VAR_সংখ্যা);

    পূর্ণসংখ্যা পূর্ব_lock = b->bv_lock;
    যদি (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_রাশিমালা_get_funccal(রাশিমালা, &newtv);

    জন্য (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_সংখ্যা;
	val = blob_get(b, i);
	tv.vval.v_সংখ্যা = val;
	set_vim_var_nr(VV_চাবি, idx);
	যদি (filter_map_one(&tv, রাশিমালা, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    বিরতি;
	যদি (filtermap != FILTERMAP_FOREACH)
	{
	    যদি (newtv.v_type != VAR_সংখ্যা && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		বিরতি;
	    }
	    যদি (filtermap != FILTERMAP_FILTER)
	    {
		যদি (newtv.vval.v_সংখ্যা != val)
		    blob_set(b_ret, i, newtv.vval.v_সংখ্যা);
	    }
	    অন্যথা যদি (rem)
	    {
		char_u *p = (char_u *)blob_arg->bv_ga.ga_তথ্য;

		mch_memmove(p + i, p + i + 1,
			    (size_t)b->bv_ga.ga_len - i - 1);
		--b->bv_ga.ga_len;
		--i;
	    }
	}
	++idx;
    }

    b->bv_lock = পূর্ব_lock;
    যদি (fc != ফাঁকা)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    শূন্য
blob_ঢোকাও_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    পূর্ণসংখ্যা		error = FALSE;
    পূর্ণসংখ্যা		val, len;
    char_u	*p;

    যদি (b == ফাঁকা)
    {
	যদি (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	ফেরত;
    }

    যদি (মান_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	ফেরত;

    len = blob_len(b);
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_সংখ্যা_chk(&argvars[2], &error);
	যদি (error)
	    ফেরত;		// type error; errmsg already given
	যদি (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    ফেরত;
	}
    }
    val = tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (error)
	ফেরত;
    যদি (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ফেরত;
    }

    যদি (ga_grow(&b->bv_ga, 1) == FAIL)
	ফেরত;
    p = (char_u *)b->bv_ga.ga_তথ্য;
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
    স্থির পূর্ণসংখ্যা
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    পূর্ণসংখ্যা		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    যদি (b2 == ফাঁকা || b2->bv_ga.ga_len == 0)
	ফেরত OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    যদি (ga_grow(&b1->bv_ga, len2) == FAIL)
	ফেরত FAIL;

    p1 = (char_u *)b1->bv_ga.ga_তথ্য;

    যদি (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	যদি (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    অন্যথা
    {
	যদি (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_তথ্য, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    ফেরত OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    শূন্য
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	পূর্ণসংখ্যা		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    পূর্ণসংখ্যা		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    যদি (b1 == ফাঁকা)
    {
	emsg(_(e_cannot_extend_null_blob));
	ফেরত;
    }
    যদি (is_new || !মান_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	যদি (is_new)
	{
	    b1 = blob_copy(b1);
	    যদি (b1 == ফাঁকা)
		ফেরত;
	}

	b2 = argvars[1].vval.v_blob;
	যদি (b2 == ফাঁকা)
	    যাও theend;

	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_সংখ্যা_chk(&argvars[2], &error);
	    যদি (error)
		যাও cleanup;		// type error; errmsg already given
	    যদি (before < 0)
		before = b1->bv_ga.ga_len + before;
	    যদি (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_সূচক_out_of_range_nr), before);
		যাও cleanup;
	    }
	}
	অন্যথা
	    before = b1->bv_ga.ga_len;
	যদি (blob_extend(b1, b2, before) == FAIL)
	    যাও cleanup;

theend:
	যদি (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	অন্যথা
	    copy_tv(&argvars[0], rettv);
	ফেরত;

cleanup:
	যদি (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    শূন্য
blob_reduce(
	typval_T	*argvars,
	typval_T	*রাশিমালা,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    পূর্ণসংখ্যা		called_emsg_start = called_emsg;
    পূর্ণসংখ্যা		r;
    typval_T	initial;
    typval_T	argv[3];
    পূর্ণসংখ্যা	i;

    যদি (argvars[2].v_type == VAR_UNKNOWN)
    {
	যদি (b == ফাঁকা || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_মান), "Blob");
	    ফেরত;
	}
	initial.v_type = VAR_সংখ্যা;
	initial.vval.v_সংখ্যা = blob_get(b, 0);
	i = 1;
    }
    অন্যথা যদি (check_for_সংখ্যা_arg(argvars, 2) == FAIL)
	ফেরত;
    অন্যথা
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    যদি (b == ফাঁকা)
	ফেরত;

    জন্য ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_সংখ্যা;
	argv[1].vval.v_সংখ্যা = blob_get(b, i);

	r = eval_রাশিমালা_typval(রাশিমালা, TRUE, argv, 2, ফাঁকা, rettv);

	clear_tv(&argv[0]);
	যদি (r == FAIL || called_emsg != called_emsg_start)
	    ফেরত;
    }
}

/*
 * "reverse({blob})" function
 */
    শূন্য
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    জন্য (i = 0; i < len / 2; i++)
    {
	পূর্ণসংখ্যা tmp = blob_get(b, (পূর্ণসংখ্যা)i);

	blob_set(b, (পূর্ণসংখ্যা)i, blob_get(b, (পূর্ণসংখ্যা)(len - i - 1)));
	blob_set(b, (পূর্ণসংখ্যা)(len - i - 1), tmp);
    }
    rettv_blob_set(rettv, b);
}

/*
 * blob2list() function
 */
    শূন্য
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    পূর্ণসংখ্যা		i, len;
    typval_T	tv;

    যদি (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(শূন্য)rettv_list_alloc(rettv);
	ফেরত;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_বস্তুসমূহ(len);
    যদি (l == ফাঁকা)
	ফেরত;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_সংখ্যা;
    tv.v_lock = 0;
    জন্য (i = 0; i < len; i++)
    {
	tv.vval.v_সংখ্যা = blob_get(blob, i);
	list_set_item(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    শূন্য
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    যদি (rettv_blob_alloc(rettv) == FAIL)
	ফেরত;
    blob = rettv->vval.v_blob;

    যদি (check_for_list_arg(argvars, 0) == FAIL)
	ফেরত;

    l = argvars->vval.v_list;
    যদি (l == ফাঁকা)
	ফেরত;

    // The result has one byte per list item; reserve all space at once.
    যদি (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	ফেরত;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	পূর্ণসংখ্যা		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_সংখ্যা_chk(&li->li_tv, &error);
	যদি (error == TRUE || n < 0 || n > 255)
	{
	    যদি (!error)
		semsg(_(e_invalid_মান_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    ফেরত;
	}
	((char_u *)blob->bv_ga.ga_তথ্য)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
