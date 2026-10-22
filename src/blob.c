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

#ᰀᰧ defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(void)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    ᰀᰧ (blob != NULL)
	ga_init2(&blob->bv_ga, 1, 100);
    ᰀᰧᰌ blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    puur_naa_nka
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    ᰀᰧ (b == NULL)
	ᰀᰧᰌ FAIL;

    rettv_blob_set(rettv, b);
    ᰀᰧᰌ OK;
}

/*
 * Set a blob as the return value.
 */
    void
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    ᰀᰧ (b != NULL)
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
    puur_naa_nka		len;
    blob_T	*copy;

    ᰀᰧ (orig == NULL)
	ᰀᰧᰌ NULL;

    copy = blob_alloc();
    ᰀᰧ (copy == NULL)
	ᰀᰧᰌ NULL;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    ᰀᰧ (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	ᰀᰧ (copy->bv_ga.ga_data == NULL)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    ᰀᰧᰌ copy;
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
    ᰀᰧ (b != NULL && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    ᰀᰧ (b == NULL)
	ᰀᰧᰌ 0L;
    ᰀᰧᰌ b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    puur_naa_nka
blob_get(blob_T *b, puur_naa_nka idx)
{
    ᰀᰧᰌ ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    void
blob_set(blob_T *blob, puur_naa_nka idx, puur_naa_nka byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    void
blob_set_append(blob_T *blob, puur_naa_nka idx, puur_naa_nka byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    ᰀᰧ (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	ᰀᰧ (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    puur_naa_nka
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    puur_naa_nka	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    ᰀᰧ (len1 == 0 && len2 == 0)
	ᰀᰧᰌ TRUE;
    ᰀᰧ (b1 == b2)
	ᰀᰧᰌ TRUE;
    ᰀᰧ (len1 != len2)
	ᰀᰧᰌ FALSE;

    for (i = 0; i < b1->bv_ga.ga_len; i++)
	ᰀᰧ (blob_get(b1, i) != blob_get(b2, i)) ᰀᰧᰌ FALSE;
    ᰀᰧᰌ TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    puur_naa_nka
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    sa_nracanaa stat	st;
    puur_naa_nka		whence;
    off_T	size = size_arg;

    ᰀᰧ (fstat(fileno(fd), &st) < 0)
	ᰀᰧᰌ FAIL;  // can't read the file, error

    ᰀᰧ (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	ᰀᰧ (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    ᰀᰧᰀ
    {
	// limit the offset to not go before the start of the file
	ᰀᰧ (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	ᰀᰧ (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    ᰀᰧ (size <= 0)
	ᰀᰧᰌ OK;
    ᰀᰧ (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	ᰀᰧᰌ OK;

    ᰀᰧ (ga_grow(&blob->bv_ga, (puur_naa_nka)size) == FAIL)
	ᰀᰧᰌ FAIL;
    blob->bv_ga.ga_len = (puur_naa_nka)size;
    ᰀᰧ (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = NULL;
	ᰀᰧᰌ FAIL;
    }
    ᰀᰧᰌ OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    puur_naa_nka
write_blob(FILE *fd, blob_T *blob)
{
    ᰀᰧ (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	ᰀᰧᰌ FAIL;
    }
    ᰀᰧᰌ OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    static const var_na hex_chars[] = "0123456789ABCDEF";
    puur_naa_nka		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    ᰀᰧ (blob == NULL || (blen = blob_len(blob)) == 0)
    {
	*tofree = NULL;
	ᰀᰧᰌ (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    ᰀᰧ (buf == NULL)
    {
	*tofree = NULL;
	ᰀᰧᰌ (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    for (puur_naa_nka i = 0; i < blen; i++)
    {
	unsigned b;

	ᰀᰧ (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    ᰀᰧᰌ buf;
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

    ᰀᰧ (rettv_list_alloc(rettv) == FAIL)
	ᰀᰧᰌ;

    for (puur_naa_nka i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	ᰀᰧ (l2 == NULL)
	    ᰀᰧᰌ;

	ᰀᰧ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ᰀᰧᰌ;
	}

	ᰀᰧ (list_append_number(l2, i) == FAIL
		|| list_append_number(l2, blob_get(blob, i)) == FAIL)
	    ᰀᰧᰌ;
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

    ᰀᰧ (blob == NULL)
	ᰀᰧᰌ NULL;
    ᰀᰧ (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	goto failed;
    s += 2;
    ᰀᰬ (vim_isxdigit(*s))
    {
	ᰀᰧ (!vim_isxdigit(s[1]))
	    goto failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	ᰀᰧ (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    ᰀᰧ (*skipwhite(s) != NUL)
	goto failed;  // text after final digit

    ++blob->bv_refcount;
    ᰀᰧᰌ blob;

failed:
    blob_free(blob);
    ᰀᰧᰌ NULL;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    static puur_naa_nka
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	puur_naa_nka		exclusive,
	typval_T	*rettv)
{
    ᰀᰧ (n1 < 0)
    {
	n1 = len + n1;
	ᰀᰧ (n1 < 0)
	    n1 = 0;
    }
    ᰀᰧ (n2 < 0)
	n2 = len + n2;
    ᰀᰧᰀ ᰀᰧ (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    ᰀᰧ (exclusive)
	--n2;
    ᰀᰧ (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = NULL;
    }
    ᰀᰧᰀ
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	ᰀᰧ (new_blob != NULL)
	{
	    ᰀᰧ (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		ᰀᰧᰌ FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    for (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    ᰀᰧᰌ OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    static puur_naa_nka
blob_index(
	blob_T		*blob,
	puur_naa_nka		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    ᰀᰧ (idx < 0)
	idx = len + idx;
    ᰀᰧ (idx < len && idx >= 0)
    {
	puur_naa_nka v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = v;
    }
    ᰀᰧᰀ
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	ᰀᰧᰌ FAIL;
    }

    ᰀᰧᰌ OK;
}

    puur_naa_nka
blob_slice_or_index(
	blob_T		*blob,
	puur_naa_nka		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	puur_naa_nka		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    ᰀᰧ (is_range)
	ᰀᰧᰌ blob_slice(blob, len, n1, n2, exclusive, rettv);
    ᰀᰧᰀ
	ᰀᰧᰌ blob_index(blob, len, n1, rettv);
    ᰀᰧᰌ OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    puur_naa_nka
check_blob_index(long bloblen, varnumber_T n1, puur_naa_nka quiet)
{
    ᰀᰧ (n1 < 0 || n1 > bloblen)
    {
	ᰀᰧ (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	ᰀᰧᰌ FAIL;
    }
    ᰀᰧᰌ OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    puur_naa_nka
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, puur_naa_nka quiet)
{
    ᰀᰧ (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	ᰀᰧ (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	ᰀᰧᰌ FAIL;
    }
    ᰀᰧᰌ OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    puur_naa_nka
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    puur_naa_nka	il, ir;

    ᰀᰧ (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_value_does_not_have_right_number_of_bytes));
	ᰀᰧᰌ FAIL;
    }

    ir = 0;
    for (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    ᰀᰧᰌ OK;
}

/*
 * "add(blob, item)" function
 */
    void
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    puur_naa_nka		error = FALSE;
    varnumber_T n;

    ᰀᰧ (b == NULL)
    {
	ᰀᰧ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	ᰀᰧᰌ;
    }

    ᰀᰧ (value_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	ᰀᰧᰌ;

    n = tv_get_number_chk(&argvars[1], &error);
    ᰀᰧ (error)
	ᰀᰧᰌ;

    ga_append(&b->bv_ga, (puur_naa_nka)n);
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
    puur_naa_nka		error = FALSE;
    long	idx;
    long	end;
    puur_naa_nka		len;
    char_u	*p;

    ᰀᰧ (b != NULL && value_check_lock(b->bv_lock, arg_errmsg, TRUE))
	ᰀᰧᰌ;

    idx = (long)tv_get_number_chk(&argvars[1], &error);
    ᰀᰧ (error)
	ᰀᰧᰌ;

    len = blob_len(b);

    ᰀᰧ (idx < 0)
	// count from the end
	idx = len + idx;
    ᰀᰧ (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	ᰀᰧᰌ;
    }
    ᰀᰧ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_number = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	ᰀᰧᰌ;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_number_chk(&argvars[2], &error);
    ᰀᰧ (error)
	ᰀᰧᰌ;
    ᰀᰧ (end < 0)
	// count from the end
	end = len + end;
    ᰀᰧ (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	ᰀᰧᰌ;
    }
    newblob = blob_alloc();
    ᰀᰧ (newblob == NULL)
	ᰀᰧᰌ;
    newblob->bv_ga.ga_len = end - idx + 1;
    ᰀᰧ (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	ᰀᰧᰌ;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    ᰀᰧ (len - end - 1 > 0)
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
    puur_naa_nka		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    puur_naa_nka		idx = 0;
    puur_naa_nka		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ᰀᰧ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = NULL;
    }
    ᰀᰧ (b == NULL || (filtermap == FILTERMAP_FILTER
			    && value_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	ᰀᰧᰌ;

    b_ret = b;
    ᰀᰧ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	ᰀᰧ (rettv->vval.v_blob == NULL)
	    ᰀᰧᰌ;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    puur_naa_nka prev_lock = b->bv_lock;
    ᰀᰧ (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    for (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_NUMBER;
	val = blob_get(b, i);
	tv.vval.v_number = val;
	set_vim_var_nr(VV_KEY, idx);
	ᰀᰧ (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    bha_nga;
	ᰀᰧ (filtermap != FILTERMAP_FOREACH)
	{
	    ᰀᰧ (newtv.v_type != VAR_NUMBER && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		bha_nga;
	    }
	    ᰀᰧ (filtermap != FILTERMAP_FILTER)
	    {
		ᰀᰧ (newtv.vval.v_number != val)
		    blob_set(b_ret, i, newtv.vval.v_number);
	    }
	    ᰀᰧᰀ ᰀᰧ (rem)
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
    ᰀᰧ (fc != NULL)
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
    puur_naa_nka		error = FALSE;
    puur_naa_nka		val, len;
    char_u	*p;

    ᰀᰧ (b == NULL)
    {
	ᰀᰧ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	ᰀᰧᰌ;
    }

    ᰀᰧ (value_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	ᰀᰧᰌ;

    len = blob_len(b);
    ᰀᰧ (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_number_chk(&argvars[2], &error);
	ᰀᰧ (error)
	    ᰀᰧᰌ;		// type error; errmsg already given
	ᰀᰧ (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    ᰀᰧᰌ;
	}
    }
    val = tv_get_number_chk(&argvars[1], &error);
    ᰀᰧ (error)
	ᰀᰧᰌ;
    ᰀᰧ (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ᰀᰧᰌ;
    }

    ᰀᰧ (ga_grow(&b->bv_ga, 1) == FAIL)
	ᰀᰧᰌ;
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
    static puur_naa_nka
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    puur_naa_nka		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    ᰀᰧ (b2 == NULL || b2->bv_ga.ga_len == 0)
	ᰀᰧᰌ OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    ᰀᰧ (ga_grow(&b1->bv_ga, len2) == FAIL)
	ᰀᰧᰌ FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    ᰀᰧ (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	ᰀᰧ (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    ᰀᰧᰀ
    {
	ᰀᰧ (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    ᰀᰧᰌ OK;
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
	puur_naa_nka		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    puur_naa_nka		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    ᰀᰧ (b1 == NULL)
    {
	emsg(_(e_cannot_extend_null_blob));
	ᰀᰧᰌ;
    }
    ᰀᰧ (is_new || !value_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	ᰀᰧ (is_new)
	{
	    b1 = blob_copy(b1);
	    ᰀᰧ (b1 == NULL)
		ᰀᰧᰌ;
	}

	b2 = argvars[1].vval.v_blob;
	ᰀᰧ (b2 == NULL)
	    goto theend;

	ᰀᰧ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_number_chk(&argvars[2], &error);
	    ᰀᰧ (error)
		goto cleanup;		// type error; errmsg already given
	    ᰀᰧ (before < 0)
		before = b1->bv_ga.ga_len + before;
	    ᰀᰧ (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		goto cleanup;
	    }
	}
	ᰀᰧᰀ
	    before = b1->bv_ga.ga_len;
	ᰀᰧ (blob_extend(b1, b2, before) == FAIL)
	    goto cleanup;

theend:
	ᰀᰧ (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	ᰀᰧᰀ
	    copy_tv(&argvars[0], rettv);
	ᰀᰧᰌ;

cleanup:
	ᰀᰧ (is_new)
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
    puur_naa_nka		called_emsg_start = called_emsg;
    puur_naa_nka		r;
    typval_T	initial;
    typval_T	argv[3];
    puur_naa_nka	i;

    ᰀᰧ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ᰀᰧ (b == NULL || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Blob");
	    ᰀᰧᰌ;
	}
	initial.v_type = VAR_NUMBER;
	initial.vval.v_number = blob_get(b, 0);
	i = 1;
    }
    ᰀᰧᰀ ᰀᰧ (check_for_number_arg(argvars, 2) == FAIL)
	ᰀᰧᰌ;
    ᰀᰧᰀ
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    ᰀᰧ (b == NULL)
	ᰀᰧᰌ;

    for ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_NUMBER;
	argv[1].vval.v_number = blob_get(b, i);

	r = eval_expr_typval(expr, TRUE, argv, 2, NULL, rettv);

	clear_tv(&argv[0]);
	ᰀᰧ (r == FAIL || called_emsg != called_emsg_start)
	    ᰀᰧᰌ;
    }
}

/*
 * "reverse({blob})" function
 */
    void
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    for (i = 0; i < len / 2; i++)
    {
	puur_naa_nka tmp = blob_get(b, (puur_naa_nka)i);

	blob_set(b, (puur_naa_nka)i, blob_get(b, (puur_naa_nka)(len - i - 1)));
	blob_set(b, (puur_naa_nka)(len - i - 1), tmp);
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
    puur_naa_nka		i, len;
    typval_T	tv;

    ᰀᰧ (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(void)rettv_list_alloc(rettv);
	ᰀᰧᰌ;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_items(len);
    ᰀᰧ (l == NULL)
	ᰀᰧᰌ;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_NUMBER;
    tv.v_lock = 0;
    for (i = 0; i < len; i++)
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

    ᰀᰧ (rettv_blob_alloc(rettv) == FAIL)
	ᰀᰧᰌ;
    blob = rettv->vval.v_blob;

    ᰀᰧ (check_for_list_arg(argvars, 0) == FAIL)
	ᰀᰧᰌ;

    l = argvars->vval.v_list;
    ᰀᰧ (l == NULL)
	ᰀᰧᰌ;

    // The result has one byte per list item; reserve all space at once.
    ᰀᰧ (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	ᰀᰧᰌ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	puur_naa_nka		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_number_chk(&li->li_tv, &error);
	ᰀᰧ (error == TRUE || n < 0 || n > 255)
	{
	    ᰀᰧ (!error)
		semsg(_(e_invalid_value_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    ᰀᰧᰌ;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
