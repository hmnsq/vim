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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(வெறுமை)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    எனில் (blob != வெற்று)
	ga_init2(&blob->bv_ga, 1, 100);
    திரும்பு blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    முழு
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    எனில் (b == வெற்று)
	திரும்பு FAIL;

    rettv_blob_set(rettv, b);
    திரும்பு OK;
}

/*
 * Set a blob as the return value.
 */
    வெறுமை
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    எனில் (b != வெற்று)
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
    முழு		len;
    blob_T	*copy;

    எனில் (orig == வெற்று)
	திரும்பு வெற்று;

    copy = blob_alloc();
    எனில் (copy == வெற்று)
	திரும்பு வெற்று;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    எனில் (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	எனில் (copy->bv_ga.ga_data == வெற்று)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    திரும்பு copy;
}

    வெறுமை
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    வெறுமை
blob_unref(blob_T *b)
{
    எனில் (b != வெற்று && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    எனில் (b == வெற்று)
	திரும்பு 0L;
    திரும்பு b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    முழு
blob_get(blob_T *b, முழு idx)
{
    திரும்பு ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    வெறுமை
blob_set(blob_T *blob, முழு idx, முழு byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    வெறுமை
blob_set_append(blob_T *blob, முழு idx, முழு byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    எனில் (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	எனில் (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    முழு
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    முழு	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    எனில் (len1 == 0 && len2 == 0)
	திரும்பு TRUE;
    எனில் (b1 == b2)
	திரும்பு TRUE;
    எனில் (len1 != len2)
	திரும்பு FALSE;

    க்கு (i = 0; i < b1->bv_ga.ga_len; i++)
	எனில் (blob_get(b1, i) != blob_get(b2, i)) திரும்பு FALSE;
    திரும்பு TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    முழு
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    கட்டமைப்பு stat	st;
    முழு		whence;
    off_T	size = size_arg;

    எனில் (fstat(fileno(fd), &st) < 0)
	திரும்பு FAIL;  // can't read the file, error

    எனில் (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	எனில் (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    இல்லையெனில்
    {
	// limit the offset to not go before the start of the file
	எனில் (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	எனில் (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    எனில் (size <= 0)
	திரும்பு OK;
    எனில் (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	திரும்பு OK;

    எனில் (ga_grow(&blob->bv_ga, (முழு)size) == FAIL)
	திரும்பு FAIL;
    blob->bv_ga.ga_len = (முழு)size;
    எனில் (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = வெற்று;
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    முழு
write_blob(FILE *fd, blob_T *blob)
{
    எனில் (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    நிலையான மாறிலி எழுத்து hex_chars[] = "0123456789ABCDEF";
    முழு		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    எனில் (blob == வெற்று || (blen = blob_len(blob)) == 0)
    {
	*tofree = வெற்று;
	திரும்பு (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    எனில் (buf == வெற்று)
    {
	*tofree = வெற்று;
	திரும்பு (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    க்கு (முழு i = 0; i < blen; i++)
    {
	unsigned b;

	எனில் (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    திரும்பு buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    வெறுமை
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    க்கு (முழு i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	எனில் (l2 == வெற்று)
	    திரும்பு;

	எனில் (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    திரும்பு;
	}

	எனில் (list_append_எண்(l2, i) == FAIL
		|| list_append_எண்(l2, blob_get(blob, i)) == FAIL)
	    திரும்பு;
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

    எனில் (blob == வெற்று)
	திரும்பு வெற்று;
    எனில் (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	செல் failed;
    s += 2;
    போது (vim_isxdigit(*s))
    {
	எனில் (!vim_isxdigit(s[1]))
	    செல் failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	எனில் (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    எனில் (*skipwhite(s) != NUL)
	செல் failed;  // text after final digit

    ++blob->bv_refcount;
    திரும்பு blob;

failed:
    blob_free(blob);
    திரும்பு வெற்று;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    நிலையான முழு
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	முழு		exclusive,
	typval_T	*rettv)
{
    எனில் (n1 < 0)
    {
	n1 = len + n1;
	எனில் (n1 < 0)
	    n1 = 0;
    }
    எனில் (n2 < 0)
	n2 = len + n2;
    இல்லையெனில் எனில் (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    எனில் (exclusive)
	--n2;
    எனில் (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = வெற்று;
    }
    இல்லையெனில்
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	எனில் (new_blob != வெற்று)
	{
	    எனில் (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		திரும்பு FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    க்கு (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    திரும்பு OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    நிலையான முழு
blob_index(
	blob_T		*blob,
	முழு		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    எனில் (idx < 0)
	idx = len + idx;
    எனில் (idx < len && idx >= 0)
    {
	முழு v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_எண்;
	rettv->vval.v_எண் = v;
    }
    இல்லையெனில்
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

    முழு
blob_slice_or_index(
	blob_T		*blob,
	முழு		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	முழு		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    எனில் (is_range)
	திரும்பு blob_slice(blob, len, n1, n2, exclusive, rettv);
    இல்லையெனில்
	திரும்பு blob_index(blob, len, n1, rettv);
    திரும்பு OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    முழு
check_blob_index(long bloblen, varnumber_T n1, முழு quiet)
{
    எனில் (n1 < 0 || n1 > bloblen)
    {
	எனில் (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    முழு
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, முழு quiet)
{
    எனில் (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	எனில் (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    முழு
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    முழு	il, ir;

    எனில் (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_மதிப்பு_does_not_have_வலது_எண்_of_bytes));
	திரும்பு FAIL;
    }

    ir = 0;
    க்கு (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    திரும்பு OK;
}

/*
 * "add(blob, item)" function
 */
    வெறுமை
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    முழு		error = FALSE;
    varnumber_T n;

    எனில் (b == வெற்று)
    {
	எனில் (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	திரும்பு;
    }

    எனில் (மதிப்பு_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	திரும்பு;

    n = tv_get_எண்_chk(&argvars[1], &error);
    எனில் (error)
	திரும்பு;

    ga_append(&b->bv_ga, (முழு)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    வெறுமை
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    முழு		error = FALSE;
    long	idx;
    long	end;
    முழு		len;
    char_u	*p;

    எனில் (b != வெற்று && மதிப்பு_check_lock(b->bv_lock, arg_errmsg, TRUE))
	திரும்பு;

    idx = (long)tv_get_எண்_chk(&argvars[1], &error);
    எனில் (error)
	திரும்பு;

    len = blob_len(b);

    எனில் (idx < 0)
	// count from the end
	idx = len + idx;
    எனில் (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	திரும்பு;
    }
    எனில் (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_எண் = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	திரும்பு;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_எண்_chk(&argvars[2], &error);
    எனில் (error)
	திரும்பு;
    எனில் (end < 0)
	// count from the end
	end = len + end;
    எனில் (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	திரும்பு;
    }
    newblob = blob_alloc();
    எனில் (newblob == வெற்று)
	திரும்பு;
    newblob->bv_ga.ga_len = end - idx + 1;
    எனில் (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	திரும்பு;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    எனில் (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (size_t)(len - end - 1));
    b->bv_ga.ga_len -= end - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    வெறுமை
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*கோவை,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    முழு		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    முழு		idx = 0;
    முழு		rem;
    typval_T	newtv;
    funccall_T	*fc;

    எனில் (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = வெற்று;
    }
    எனில் (b == வெற்று || (filtermap == FILTERMAP_FILTER
			    && மதிப்பு_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	திரும்பு;

    b_ret = b;
    எனில் (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	எனில் (rettv->vval.v_blob == வெற்று)
	    திரும்பு;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_சாவி, VAR_எண்);

    முழு prev_lock = b->bv_lock;
    எனில் (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_கோவை_get_funccal(கோவை, &newtv);

    க்கு (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_எண்;
	val = blob_get(b, i);
	tv.vval.v_எண் = val;
	set_vim_var_nr(VV_சாவி, idx);
	எனில் (filter_map_one(&tv, கோவை, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    நிறுத்து;
	எனில் (filtermap != FILTERMAP_FOREACH)
	{
	    எனில் (newtv.v_type != VAR_எண் && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		நிறுத்து;
	    }
	    எனில் (filtermap != FILTERMAP_FILTER)
	    {
		எனில் (newtv.vval.v_எண் != val)
		    blob_set(b_ret, i, newtv.vval.v_எண்);
	    }
	    இல்லையெனில் எனில் (rem)
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
    எனில் (fc != வெற்று)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    வெறுமை
blob_செருகு_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    முழு		error = FALSE;
    முழு		val, len;
    char_u	*p;

    எனில் (b == வெற்று)
    {
	எனில் (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	திரும்பு;
    }

    எனில் (மதிப்பு_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	திரும்பு;

    len = blob_len(b);
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_எண்_chk(&argvars[2], &error);
	எனில் (error)
	    திரும்பு;		// type error; errmsg already given
	எனில் (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    திரும்பு;
	}
    }
    val = tv_get_எண்_chk(&argvars[1], &error);
    எனில் (error)
	திரும்பு;
    எனில் (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	திரும்பு;
    }

    எனில் (ga_grow(&b->bv_ga, 1) == FAIL)
	திரும்பு;
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
    நிலையான முழு
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    முழு		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    எனில் (b2 == வெற்று || b2->bv_ga.ga_len == 0)
	திரும்பு OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    எனில் (ga_grow(&b1->bv_ga, len2) == FAIL)
	திரும்பு FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    எனில் (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	எனில் (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    இல்லையெனில்
    {
	எனில் (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    திரும்பு OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    வெறுமை
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	முழு		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    முழு		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    எனில் (b1 == வெற்று)
    {
	emsg(_(e_cannot_extend_null_blob));
	திரும்பு;
    }
    எனில் (is_new || !மதிப்பு_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	எனில் (is_new)
	{
	    b1 = blob_copy(b1);
	    எனில் (b1 == வெற்று)
		திரும்பு;
	}

	b2 = argvars[1].vval.v_blob;
	எனில் (b2 == வெற்று)
	    செல் theend;

	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_எண்_chk(&argvars[2], &error);
	    எனில் (error)
		செல் cleanup;		// type error; errmsg already given
	    எனில் (before < 0)
		before = b1->bv_ga.ga_len + before;
	    எனில் (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		செல் cleanup;
	    }
	}
	இல்லையெனில்
	    before = b1->bv_ga.ga_len;
	எனில் (blob_extend(b1, b2, before) == FAIL)
	    செல் cleanup;

theend:
	எனில் (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	இல்லையெனில்
	    copy_tv(&argvars[0], rettv);
	திரும்பு;

cleanup:
	எனில் (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    வெறுமை
blob_reduce(
	typval_T	*argvars,
	typval_T	*கோவை,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    முழு		called_emsg_start = called_emsg;
    முழு		r;
    typval_T	initial;
    typval_T	argv[3];
    முழு	i;

    எனில் (argvars[2].v_type == VAR_UNKNOWN)
    {
	எனில் (b == வெற்று || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_மதிப்பு), "Blob");
	    திரும்பு;
	}
	initial.v_type = VAR_எண்;
	initial.vval.v_எண் = blob_get(b, 0);
	i = 1;
    }
    இல்லையெனில் எனில் (check_for_எண்_arg(argvars, 2) == FAIL)
	திரும்பு;
    இல்லையெனில்
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    எனில் (b == வெற்று)
	திரும்பு;

    க்கு ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_எண்;
	argv[1].vval.v_எண் = blob_get(b, i);

	r = eval_கோவை_typval(கோவை, TRUE, argv, 2, வெற்று, rettv);

	clear_tv(&argv[0]);
	எனில் (r == FAIL || called_emsg != called_emsg_start)
	    திரும்பு;
    }
}

/*
 * "reverse({blob})" function
 */
    வெறுமை
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    க்கு (i = 0; i < len / 2; i++)
    {
	முழு tmp = blob_get(b, (முழு)i);

	blob_set(b, (முழு)i, blob_get(b, (முழு)(len - i - 1)));
	blob_set(b, (முழு)(len - i - 1), tmp);
    }
    rettv_blob_set(rettv, b);
}

/*
 * blob2list() function
 */
    வெறுமை
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    முழு		i, len;
    typval_T	tv;

    எனில் (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(வெறுமை)rettv_list_alloc(rettv);
	திரும்பு;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_பொருட்கள்(len);
    எனில் (l == வெற்று)
	திரும்பு;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_எண்;
    tv.v_lock = 0;
    க்கு (i = 0; i < len; i++)
    {
	tv.vval.v_எண் = blob_get(blob, i);
	list_set_item(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    வெறுமை
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    எனில் (rettv_blob_alloc(rettv) == FAIL)
	திரும்பு;
    blob = rettv->vval.v_blob;

    எனில் (check_for_list_arg(argvars, 0) == FAIL)
	திரும்பு;

    l = argvars->vval.v_list;
    எனில் (l == வெற்று)
	திரும்பு;

    // The result has one byte per list item; reserve all space at once.
    எனில் (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	திரும்பு;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	முழு		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_எண்_chk(&li->li_tv, &error);
	எனில் (error == TRUE || n < 0 || n > 255)
	{
	    எனில் (!error)
		semsg(_(e_invalid_மதிப்பு_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    திரும்பு;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
