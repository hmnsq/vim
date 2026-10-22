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

#जर defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(शून्य)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    जर (blob != रिक्त)
	ga_init2(&blob->bv_ga, 1, 100);
    परत blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    पूर्णांक
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    जर (b == रिक्त)
	परत FAIL;

    rettv_blob_set(rettv, b);
    परत OK;
}

/*
 * Set a blob as the return value.
 */
    शून्य
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    जर (b != रिक्त)
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
    पूर्णांक		len;
    blob_T	*copy;

    जर (orig == रिक्त)
	परत रिक्त;

    copy = blob_alloc();
    जर (copy == रिक्त)
	परत रिक्त;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    जर (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	जर (copy->bv_ga.ga_data == रिक्त)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    परत copy;
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
    जर (b != रिक्त && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    दीर्घ
blob_len(blob_T *b)
{
    जर (b == रिक्त)
	परत 0L;
    परत b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    पूर्णांक
blob_get(blob_T *b, पूर्णांक idx)
{
    परत ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    शून्य
blob_set(blob_T *blob, पूर्णांक idx, पूर्णांक byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    शून्य
blob_set_append(blob_T *blob, पूर्णांक idx, पूर्णांक byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    जर (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	जर (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    पूर्णांक
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    पूर्णांक	    i;
    दीर्घ    len1 = blob_len(b1);
    दीर्घ    len2 = blob_len(b2);

    // empty and NULL are considered the same
    जर (len1 == 0 && len2 == 0)
	परत TRUE;
    जर (b1 == b2)
	परत TRUE;
    जर (len1 != len2)
	परत FALSE;

    साठी (i = 0; i < b1->bv_ga.ga_len; i++)
	जर (blob_get(b1, i) != blob_get(b2, i)) परत FALSE;
    परत TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    पूर्णांक
read_blob(खाता *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    संरचना stat	st;
    पूर्णांक		whence;
    off_T	size = size_arg;

    जर (ख_स्थिति(fileno(fd), &st) < 0)
	परत FAIL;  // can't read the file, error

    जर (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	जर (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = शुरू_से;
    }
    अन्यथा
    {
	// limit the offset to not go before the start of the file
	जर (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	जर (size == -1 || size > -offset)
	    size = -offset;
	whence = अंत_से;
    }
    जर (size <= 0)
	परत OK;
    जर (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	परत OK;

    जर (ga_grow(&blob->bv_ga, (पूर्णांक)size) == FAIL)
	परत FAIL;
    blob->bv_ga.ga_len = (पूर्णांक)size;
    जर (ख_पढ़ो(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (माप_प्रकार)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = रिक्त;
	परत FAIL;
    }
    परत OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    पूर्णांक
write_blob(खाता *fd, blob_T *blob)
{
    जर (ख_डालो(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (माप_प्रकार)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	परत FAIL;
    }
    परत OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    स्थिर स्थिरांक अक्षर hex_chars[] = "0123456789ABCDEF";
    पूर्णांक		blen;
    माप_प्रकार	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    जर (blob == रिक्त || (blen = blob_len(blob)) == 0)
    {
	*tofree = रिक्त;
	परत (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (माप_प्रकार)blen * 2 + (माप_प्रकार)((blen - 1) / 4) + 1;
    buf = alloc(total);
    जर (buf == रिक्त)
    {
	*tofree = रिक्त;
	परत (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    साठी (पूर्णांक i = 0; i < blen; i++)
    {
	अचिन्हित b;

	जर (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    परत buf;
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

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    साठी (पूर्णांक i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	जर (l2 == रिक्त)
	    परत;

	जर (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    परत;
	}

	जर (list_append_संख्या(l2, i) == FAIL
		|| list_append_संख्या(l2, blob_get(blob, i)) == FAIL)
	    परत;
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

    जर (blob == रिक्त)
	परत रिक्त;
    जर (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	जा failed;
    s += 2;
    पर्यंत (vim_isxdigit(*s))
    {
	जर (!vim_isxdigit(s[1]))
	    जा failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	जर (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    जर (*skipwhite(s) != NUL)
	जा failed;  // text after final digit

    ++blob->bv_refcount;
    परत blob;

failed:
    blob_free(blob);
    परत रिक्त;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    स्थिर पूर्णांक
blob_slice(
	blob_T		*blob,
	दीर्घ		len,
	varnumber_T	n1,
	varnumber_T	n2,
	पूर्णांक		exclusive,
	typval_T	*rettv)
{
    जर (n1 < 0)
    {
	n1 = len + n1;
	जर (n1 < 0)
	    n1 = 0;
    }
    जर (n2 < 0)
	n2 = len + n2;
    अन्यथा जर (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    जर (exclusive)
	--n2;
    जर (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = रिक्त;
    }
    अन्यथा
    {
	blob_T  *new_blob = blob_alloc();
	दीर्घ    i;

	जर (new_blob != रिक्त)
	{
	    जर (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		परत FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    साठी (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    परत OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    स्थिर पूर्णांक
blob_index(
	blob_T		*blob,
	पूर्णांक		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    जर (idx < 0)
	idx = len + idx;
    जर (idx < len && idx >= 0)
    {
	पूर्णांक v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_संख्या;
	rettv->vval.v_संख्या = v;
    }
    अन्यथा
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	परत FAIL;
    }

    परत OK;
}

    पूर्णांक
blob_slice_or_index(
	blob_T		*blob,
	पूर्णांक		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	पूर्णांक		exclusive,
	typval_T	*rettv)
{
    दीर्घ	len = blob_len(blob);

    जर (is_range)
	परत blob_slice(blob, len, n1, n2, exclusive, rettv);
    अन्यथा
	परत blob_index(blob, len, n1, rettv);
    परत OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    पूर्णांक
check_blob_index(दीर्घ bloblen, varnumber_T n1, पूर्णांक quiet)
{
    जर (n1 < 0 || n1 > bloblen)
    {
	जर (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	परत FAIL;
    }
    परत OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    पूर्णांक
check_blob_range(दीर्घ bloblen, varnumber_T n1, varnumber_T n2, पूर्णांक quiet)
{
    जर (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	जर (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	परत FAIL;
    }
    परत OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    पूर्णांक
blob_set_range(blob_T *dest, दीर्घ n1, दीर्घ n2, typval_T *src)
{
    पूर्णांक	il, ir;

    जर (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_मूल्य_does_not_have_उजवा_संख्या_of_bytes));
	परत FAIL;
    }

    ir = 0;
    साठी (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    परत OK;
}

/*
 * "add(blob, item)" function
 */
    शून्य
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    पूर्णांक		error = FALSE;
    varnumber_T n;

    जर (b == रिक्त)
    {
	जर (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	परत;
    }

    जर (मूल्य_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	परत;

    n = tv_get_संख्या_chk(&argvars[1], &error);
    जर (error)
	परत;

    ga_append(&b->bv_ga, (पूर्णांक)n);
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
    पूर्णांक		error = FALSE;
    दीर्घ	idx;
    दीर्घ	end;
    पूर्णांक		len;
    char_u	*p;

    जर (b != रिक्त && मूल्य_check_lock(b->bv_lock, arg_errmsg, TRUE))
	परत;

    idx = (दीर्घ)tv_get_संख्या_chk(&argvars[1], &error);
    जर (error)
	परत;

    len = blob_len(b);

    जर (idx < 0)
	// count from the end
	idx = len + idx;
    जर (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	परत;
    }
    जर (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_संख्या = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (माप_प्रकार)len - idx - 1);
	--b->bv_ga.ga_len;
	परत;
    }

    // Remove range of items, return blob with values.
    end = (दीर्घ)tv_get_संख्या_chk(&argvars[2], &error);
    जर (error)
	परत;
    जर (end < 0)
	// count from the end
	end = len + end;
    जर (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	परत;
    }
    newblob = blob_alloc();
    जर (newblob == रिक्त)
	परत;
    newblob->bv_ga.ga_len = end - idx + 1;
    जर (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	परत;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (माप_प्रकार)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    जर (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (माप_प्रकार)(len - end - 1));
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
	typval_T	*अभिव्यक्ती,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    पूर्णांक		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    पूर्णांक		idx = 0;
    पूर्णांक		rem;
    typval_T	newtv;
    funccall_T	*fc;

    जर (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = रिक्त;
    }
    जर (b == रिक्त || (filtermap == FILTERMAP_FILTER
			    && मूल्य_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	परत;

    b_ret = b;
    जर (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	जर (rettv->vval.v_blob == रिक्त)
	    परत;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_किल्ली, VAR_संख्या);

    पूर्णांक prev_lock = b->bv_lock;
    जर (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_अभिव्यक्ती_get_funccal(अभिव्यक्ती, &newtv);

    साठी (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_संख्या;
	val = blob_get(b, i);
	tv.vval.v_संख्या = val;
	set_vim_var_nr(VV_किल्ली, idx);
	जर (filter_map_one(&tv, अभिव्यक्ती, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    थांबा;
	जर (filtermap != FILTERMAP_FOREACH)
	{
	    जर (newtv.v_type != VAR_संख्या && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		थांबा;
	    }
	    जर (filtermap != FILTERMAP_FILTER)
	    {
		जर (newtv.vval.v_संख्या != val)
		    blob_set(b_ret, i, newtv.vval.v_संख्या);
	    }
	    अन्यथा जर (rem)
	    {
		char_u *p = (char_u *)blob_arg->bv_ga.ga_data;

		mch_memmove(p + i, p + i + 1,
			    (माप_प्रकार)b->bv_ga.ga_len - i - 1);
		--b->bv_ga.ga_len;
		--i;
	    }
	}
	++idx;
    }

    b->bv_lock = prev_lock;
    जर (fc != रिक्त)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    शून्य
blob_घाला_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    दीर्घ	before = 0;
    पूर्णांक		error = FALSE;
    पूर्णांक		val, len;
    char_u	*p;

    जर (b == रिक्त)
    {
	जर (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	परत;
    }

    जर (मूल्य_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	परत;

    len = blob_len(b);
    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (दीर्घ)tv_get_संख्या_chk(&argvars[2], &error);
	जर (error)
	    परत;		// type error; errmsg already given
	जर (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    परत;
	}
    }
    val = tv_get_संख्या_chk(&argvars[1], &error);
    जर (error)
	परत;
    जर (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	परत;
    }

    जर (ga_grow(&b->bv_ga, 1) == FAIL)
	परत;
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove(p + before + 1, p + before, (माप_प्रकार)len - before);
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
    स्थिर पूर्णांक
blob_extend(blob_T *b1, blob_T *b2, दीर्घ bef)
{
    पूर्णांक		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    जर (b2 == रिक्त || b2->bv_ga.ga_len == 0)
	परत OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    जर (ga_grow(&b1->bv_ga, len2) == FAIL)
	परत FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    जर (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (माप_प्रकार)len1);
	जर (bef < len1)
	    स_नकल(p1 + bef + len1, p1 + bef * 2, (माप_प्रकार)(len1 - bef));
    }
    अन्यथा
    {
	जर (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (माप_प्रकार)(len1 - bef));

	स_नकल(p1 + bef, b2->bv_ga.ga_data, (माप_प्रकार)len2);
    }

    b1->bv_ga.ga_len += len2;

    परत OK;
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
	पूर्णांक		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    दीर्घ	before;
    पूर्णांक		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    जर (b1 == रिक्त)
    {
	emsg(_(e_cannot_extend_null_blob));
	परत;
    }
    जर (is_new || !मूल्य_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	जर (is_new)
	{
	    b1 = blob_copy(b1);
	    जर (b1 == रिक्त)
		परत;
	}

	b2 = argvars[1].vval.v_blob;
	जर (b2 == रिक्त)
	    जा theend;

	जर (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (दीर्घ)tv_get_संख्या_chk(&argvars[2], &error);
	    जर (error)
		जा cleanup;		// type error; errmsg already given
	    जर (before < 0)
		before = b1->bv_ga.ga_len + before;
	    जर (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		जा cleanup;
	    }
	}
	अन्यथा
	    before = b1->bv_ga.ga_len;
	जर (blob_extend(b1, b2, before) == FAIL)
	    जा cleanup;

theend:
	जर (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	अन्यथा
	    copy_tv(&argvars[0], rettv);
	परत;

cleanup:
	जर (is_new)
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
	typval_T	*अभिव्यक्ती,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    पूर्णांक		called_emsg_start = called_emsg;
    पूर्णांक		r;
    typval_T	initial;
    typval_T	argv[3];
    पूर्णांक	i;

    जर (argvars[2].v_type == VAR_UNKNOWN)
    {
	जर (b == रिक्त || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_मूल्य), "Blob");
	    परत;
	}
	initial.v_type = VAR_संख्या;
	initial.vval.v_संख्या = blob_get(b, 0);
	i = 1;
    }
    अन्यथा जर (check_for_संख्या_arg(argvars, 2) == FAIL)
	परत;
    अन्यथा
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    जर (b == रिक्त)
	परत;

    साठी ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_संख्या;
	argv[1].vval.v_संख्या = blob_get(b, i);

	r = eval_अभिव्यक्ती_typval(अभिव्यक्ती, TRUE, argv, 2, रिक्त, rettv);

	clear_tv(&argv[0]);
	जर (r == FAIL || called_emsg != called_emsg_start)
	    परत;
    }
}

/*
 * "reverse({blob})" function
 */
    शून्य
blob_reverse(blob_T *b, typval_T *rettv)
{
    दीर्घ    i, len = blob_len(b);

    साठी (i = 0; i < len / 2; i++)
    {
	पूर्णांक tmp = blob_get(b, (पूर्णांक)i);

	blob_set(b, (पूर्णांक)i, blob_get(b, (पूर्णांक)(len - i - 1)));
	blob_set(b, (पूर्णांक)(len - i - 1), tmp);
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
    पूर्णांक		i, len;
    typval_T	tv;

    जर (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(शून्य)rettv_list_alloc(rettv);
	परत;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_वस्तुसंच(len);
    जर (l == रिक्त)
	परत;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_संख्या;
    tv.v_lock = 0;
    साठी (i = 0; i < len; i++)
    {
	tv.vval.v_संख्या = blob_get(blob, i);
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

    जर (rettv_blob_alloc(rettv) == FAIL)
	परत;
    blob = rettv->vval.v_blob;

    जर (check_for_list_arg(argvars, 0) == FAIL)
	परत;

    l = argvars->vval.v_list;
    जर (l == रिक्त)
	परत;

    // The result has one byte per list item; reserve all space at once.
    जर (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	परत;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	पूर्णांक		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_संख्या_chk(&li->li_tv, &error);
	जर (error == TRUE || n < 0 || n > 255)
	{
	    जर (!error)
		semsg(_(e_invalid_मूल्य_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    परत;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
