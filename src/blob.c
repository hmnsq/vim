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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(शून्य)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    जइ (blob != रिक्त)
	ga_init2(&blob->bv_ga, 1, 100);
    प्रत्यागम blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    जइ (b == रिक्त)
	प्रत्यागम FAIL;

    rettv_blob_set(rettv, b);
    प्रत्यागम OK;
}

/*
 * Set a blob as the return value.
 */
    शून्य
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    जइ (b != रिक्त)
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
    पूर्णाङ्क		len;
    blob_T	*copy;

    जइ (orig == रिक्त)
	प्रत्यागम रिक्त;

    copy = blob_alloc();
    जइ (copy == रिक्त)
	प्रत्यागम रिक्त;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    जइ (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	जइ (copy->bv_ga.ga_data == रिक्त)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    प्रत्यागम copy;
}

    शून्य
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    शून्य
blob_unref(blob_T *b)
{
    जइ (b != रिक्त && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    जइ (b == रिक्त)
	प्रत्यागम 0L;
    प्रत्यागम b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    पूर्णाङ्क
blob_get(blob_T *b, पूर्णाङ्क idx)
{
    प्रत्यागम ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    शून्य
blob_set(blob_T *blob, पूर्णाङ्क idx, पूर्णाङ्क byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    शून्य
blob_set_append(blob_T *blob, पूर्णाङ्क idx, पूर्णाङ्क byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    जइ (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	जइ (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    पूर्णाङ्क
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    पूर्णाङ्क	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    जइ (len1 == 0 && len2 == 0)
	प्रत्यागम TRUE;
    जइ (b1 == b2)
	प्रत्यागम TRUE;
    जइ (len1 != len2)
	प्रत्यागम FALSE;

    प्रति (i = 0; i < b1->bv_ga.ga_len; i++)
	जइ (blob_get(b1, i) != blob_get(b2, i)) प्रत्यागम FALSE;
    प्रत्यागम TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    पूर्णाङ्क
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    संरचना stat	st;
    पूर्णाङ्क		whence;
    off_T	size = size_arg;

    जइ (fstat(fileno(fd), &st) < 0)
	प्रत्यागम FAIL;  // can't read the file, error

    जइ (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	जइ (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    अण्णहा
    {
	// limit the offset to not go before the start of the file
	जइ (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	जइ (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    जइ (size <= 0)
	प्रत्यागम OK;
    जइ (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	प्रत्यागम OK;

    जइ (ga_grow(&blob->bv_ga, (पूर्णाङ्क)size) == FAIL)
	प्रत्यागम FAIL;
    blob->bv_ga.ga_len = (पूर्णाङ्क)size;
    जइ (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = रिक्त;
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    पूर्णाङ्क
write_blob(FILE *fd, blob_T *blob)
{
    जइ (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    स्थिर ध्रुव वर्ण hex_chars[] = "0123456789ABCDEF";
    पूर्णाङ्क		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    जइ (blob == रिक्त || (blen = blob_len(blob)) == 0)
    {
	*tofree = रिक्त;
	प्रत्यागम (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    जइ (buf == रिक्त)
    {
	*tofree = रिक्त;
	प्रत्यागम (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    प्रति (पूर्णाङ्क i = 0; i < blen; i++)
    {
	unsigned b;

	जइ (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    प्रत्यागम buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    शून्य
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    जइ (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    प्रति (पूर्णाङ्क i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	जइ (l2 == रिक्त)
	    प्रत्यागम;

	जइ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    प्रत्यागम;
	}

	जइ (list_append_सङ्ख्या(l2, i) == FAIL
		|| list_append_सङ्ख्या(l2, blob_get(blob, i)) == FAIL)
	    प्रत्यागम;
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

    जइ (blob == रिक्त)
	प्रत्यागम रिक्त;
    जइ (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	गच्छ failed;
    s += 2;
    जाव (vim_isxdigit(*s))
    {
	जइ (!vim_isxdigit(s[1]))
	    गच्छ failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	जइ (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    जइ (*skipwhite(s) != NUL)
	गच्छ failed;  // text after final digit

    ++blob->bv_refcount;
    प्रत्यागम blob;

failed:
    blob_free(blob);
    प्रत्यागम रिक्त;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    स्थिर पूर्णाङ्क
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	पूर्णाङ्क		exclusive,
	typval_T	*rettv)
{
    जइ (n1 < 0)
    {
	n1 = len + n1;
	जइ (n1 < 0)
	    n1 = 0;
    }
    जइ (n2 < 0)
	n2 = len + n2;
    अण्णहा जइ (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    जइ (exclusive)
	--n2;
    जइ (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = रिक्त;
    }
    अण्णहा
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	जइ (new_blob != रिक्त)
	{
	    जइ (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		प्रत्यागम FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    प्रति (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    प्रत्यागम OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    स्थिर पूर्णाङ्क
blob_index(
	blob_T		*blob,
	पूर्णाङ्क		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    जइ (idx < 0)
	idx = len + idx;
    जइ (idx < len && idx >= 0)
    {
	पूर्णाङ्क v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_सङ्ख्या;
	rettv->vval.v_सङ्ख्या = v;
    }
    अण्णहा
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

    पूर्णाङ्क
blob_slice_or_index(
	blob_T		*blob,
	पूर्णाङ्क		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	पूर्णाङ्क		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    जइ (is_range)
	प्रत्यागम blob_slice(blob, len, n1, n2, exclusive, rettv);
    अण्णहा
	प्रत्यागम blob_index(blob, len, n1, rettv);
    प्रत्यागम OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    पूर्णाङ्क
check_blob_index(long bloblen, varnumber_T n1, पूर्णाङ्क quiet)
{
    जइ (n1 < 0 || n1 > bloblen)
    {
	जइ (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    पूर्णाङ्क
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, पूर्णाङ्क quiet)
{
    जइ (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	जइ (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    पूर्णाङ्क
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    पूर्णाङ्क	il, ir;

    जइ (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_मानम्_does_not_have_दक्षिणः_सङ्ख्या_of_bytes));
	प्रत्यागम FAIL;
    }

    ir = 0;
    प्रति (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    प्रत्यागम OK;
}

/*
 * "add(blob, item)" function
 */
    शून्य
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    पूर्णाङ्क		error = FALSE;
    varnumber_T n;

    जइ (b == रिक्त)
    {
	जइ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	प्रत्यागम;
    }

    जइ (मानम्_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	प्रत्यागम;

    n = tv_get_सङ्ख्या_chk(&argvars[1], &error);
    जइ (error)
	प्रत्यागम;

    ga_append(&b->bv_ga, (पूर्णाङ्क)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    शून्य
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    पूर्णाङ्क		error = FALSE;
    long	idx;
    long	end;
    पूर्णाङ्क		len;
    char_u	*p;

    जइ (b != रिक्त && मानम्_check_lock(b->bv_lock, arg_errmsg, TRUE))
	प्रत्यागम;

    idx = (long)tv_get_सङ्ख्या_chk(&argvars[1], &error);
    जइ (error)
	प्रत्यागम;

    len = blob_len(b);

    जइ (idx < 0)
	// count from the end
	idx = len + idx;
    जइ (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	प्रत्यागम;
    }
    जइ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_सङ्ख्या = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	प्रत्यागम;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_सङ्ख्या_chk(&argvars[2], &error);
    जइ (error)
	प्रत्यागम;
    जइ (end < 0)
	// count from the end
	end = len + end;
    जइ (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	प्रत्यागम;
    }
    newblob = blob_alloc();
    जइ (newblob == रिक्त)
	प्रत्यागम;
    newblob->bv_ga.ga_len = end - idx + 1;
    जइ (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	प्रत्यागम;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    जइ (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (size_t)(len - end - 1));
    b->bv_ga.ga_len -= end - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    शून्य
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*व्यञ्जकम्,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    पूर्णाङ्क		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    पूर्णाङ्क		idx = 0;
    पूर्णाङ्क		rem;
    typval_T	newtv;
    funccall_T	*fc;

    जइ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = रिक्त;
    }
    जइ (b == रिक्त || (filtermap == FILTERMAP_FILTER
			    && मानम्_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	प्रत्यागम;

    b_ret = b;
    जइ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	जइ (rettv->vval.v_blob == रिक्त)
	    प्रत्यागम;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_कुञ्चिका, VAR_सङ्ख्या);

    पूर्णाङ्क prev_lock = b->bv_lock;
    जइ (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_व्यञ्जकम्_get_funccal(व्यञ्जकम्, &newtv);

    प्रति (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_सङ्ख्या;
	val = blob_get(b, i);
	tv.vval.v_सङ्ख्या = val;
	set_vim_var_nr(VV_कुञ्चिका, idx);
	जइ (filter_map_one(&tv, व्यञ्जकम्, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    भङ्ग;
	जइ (filtermap != FILTERMAP_FOREACH)
	{
	    जइ (newtv.v_type != VAR_सङ्ख्या && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		भङ्ग;
	    }
	    जइ (filtermap != FILTERMAP_FILTER)
	    {
		जइ (newtv.vval.v_सङ्ख्या != val)
		    blob_set(b_ret, i, newtv.vval.v_सङ्ख्या);
	    }
	    अण्णहा जइ (rem)
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
    जइ (fc != रिक्त)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    शून्य
blob_निवेशय_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    पूर्णाङ्क		error = FALSE;
    पूर्णाङ्क		val, len;
    char_u	*p;

    जइ (b == रिक्त)
    {
	जइ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	प्रत्यागम;
    }

    जइ (मानम्_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	प्रत्यागम;

    len = blob_len(b);
    जइ (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_सङ्ख्या_chk(&argvars[2], &error);
	जइ (error)
	    प्रत्यागम;		// type error; errmsg already given
	जइ (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    प्रत्यागम;
	}
    }
    val = tv_get_सङ्ख्या_chk(&argvars[1], &error);
    जइ (error)
	प्रत्यागम;
    जइ (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	प्रत्यागम;
    }

    जइ (ga_grow(&b->bv_ga, 1) == FAIL)
	प्रत्यागम;
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
    स्थिर पूर्णाङ्क
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    पूर्णाङ्क		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    जइ (b2 == रिक्त || b2->bv_ga.ga_len == 0)
	प्रत्यागम OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    जइ (ga_grow(&b1->bv_ga, len2) == FAIL)
	प्रत्यागम FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    जइ (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	जइ (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    अण्णहा
    {
	जइ (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    प्रत्यागम OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    शून्य
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	पूर्णाङ्क		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    पूर्णाङ्क		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    जइ (b1 == रिक्त)
    {
	emsg(_(e_cannot_extend_null_blob));
	प्रत्यागम;
    }
    जइ (is_new || !मानम्_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	जइ (is_new)
	{
	    b1 = blob_copy(b1);
	    जइ (b1 == रिक्त)
		प्रत्यागम;
	}

	b2 = argvars[1].vval.v_blob;
	जइ (b2 == रिक्त)
	    गच्छ theend;

	जइ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_सङ्ख्या_chk(&argvars[2], &error);
	    जइ (error)
		गच्छ cleanup;		// type error; errmsg already given
	    जइ (before < 0)
		before = b1->bv_ga.ga_len + before;
	    जइ (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		गच्छ cleanup;
	    }
	}
	अण्णहा
	    before = b1->bv_ga.ga_len;
	जइ (blob_extend(b1, b2, before) == FAIL)
	    गच्छ cleanup;

theend:
	जइ (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	अण्णहा
	    copy_tv(&argvars[0], rettv);
	प्रत्यागम;

cleanup:
	जइ (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    शून्य
blob_reduce(
	typval_T	*argvars,
	typval_T	*व्यञ्जकम्,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    पूर्णाङ्क		called_emsg_start = called_emsg;
    पूर्णाङ्क		r;
    typval_T	initial;
    typval_T	argv[3];
    पूर्णाङ्क	i;

    जइ (argvars[2].v_type == VAR_UNKNOWN)
    {
	जइ (b == रिक्त || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_मानम्), "Blob");
	    प्रत्यागम;
	}
	initial.v_type = VAR_सङ्ख्या;
	initial.vval.v_सङ्ख्या = blob_get(b, 0);
	i = 1;
    }
    अण्णहा जइ (check_for_सङ्ख्या_arg(argvars, 2) == FAIL)
	प्रत्यागम;
    अण्णहा
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    जइ (b == रिक्त)
	प्रत्यागम;

    प्रति ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_सङ्ख्या;
	argv[1].vval.v_सङ्ख्या = blob_get(b, i);

	r = eval_व्यञ्जकम्_typval(व्यञ्जकम्, TRUE, argv, 2, रिक्त, rettv);

	clear_tv(&argv[0]);
	जइ (r == FAIL || called_emsg != called_emsg_start)
	    प्रत्यागम;
    }
}

/*
 * "reverse({blob})" function
 */
    शून्य
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    प्रति (i = 0; i < len / 2; i++)
    {
	पूर्णाङ्क tmp = blob_get(b, (पूर्णाङ्क)i);

	blob_set(b, (पूर्णाङ्क)i, blob_get(b, (पूर्णाङ्क)(len - i - 1)));
	blob_set(b, (पूर्णाङ्क)(len - i - 1), tmp);
    }
    rettv_blob_set(rettv, b);
}

/*
 * blob2list() function
 */
    शून्य
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    पूर्णाङ्क		i, len;
    typval_T	tv;

    जइ (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(शून्य)rettv_list_alloc(rettv);
	प्रत्यागम;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_वस्तूनि(len);
    जइ (l == रिक्त)
	प्रत्यागम;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_सङ्ख्या;
    tv.v_lock = 0;
    प्रति (i = 0; i < len; i++)
    {
	tv.vval.v_सङ्ख्या = blob_get(blob, i);
	list_set_item(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    शून्य
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    जइ (rettv_blob_alloc(rettv) == FAIL)
	प्रत्यागम;
    blob = rettv->vval.v_blob;

    जइ (check_for_list_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    l = argvars->vval.v_list;
    जइ (l == रिक्त)
	प्रत्यागम;

    // The result has one byte per list item; reserve all space at once.
    जइ (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	प्रत्यागम;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_वस्तूनि(l, li)
    {
	पूर्णाङ्क		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_सङ्ख्या_chk(&li->li_tv, &error);
	जइ (error == TRUE || n < 0 || n > 255)
	{
	    जइ (!error)
		semsg(_(e_invalid_मानम्_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    प्रत्यागम;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
