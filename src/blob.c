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

#만약 defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(void)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    만약 (blob != NULL)
	ga_init2(&blob->bv_ga, 1, 100);
    반환 blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    정수
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    만약 (b == NULL)
	반환 FAIL;

    rettv_blob_set(rettv, b);
    반환 OK;
}

/*
 * Set a blob as the return value.
 */
    void
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    만약 (b != NULL)
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
    정수		len;
    blob_T	*copy;

    만약 (orig == NULL)
	반환 NULL;

    copy = blob_alloc();
    만약 (copy == NULL)
	반환 NULL;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    만약 (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	만약 (copy->bv_ga.ga_data == NULL)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    반환 copy;
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
    만약 (b != NULL && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    만약 (b == NULL)
	반환 0L;
    반환 b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    정수
blob_get(blob_T *b, 정수 idx)
{
    반환 ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    void
blob_set(blob_T *blob, 정수 idx, 정수 byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    void
blob_set_append(blob_T *blob, 정수 idx, 정수 byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    만약 (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	만약 (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    정수
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    정수	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    만약 (len1 == 0 && len2 == 0)
	반환 TRUE;
    만약 (b1 == b2)
	반환 TRUE;
    만약 (len1 != len2)
	반환 FALSE;

    위해 (i = 0; i < b1->bv_ga.ga_len; i++)
	만약 (blob_get(b1, i) != blob_get(b2, i)) 반환 FALSE;
    반환 TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    정수
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    struct stat	st;
    정수		whence;
    off_T	size = size_arg;

    만약 (fstat(fileno(fd), &st) < 0)
	반환 FAIL;  // can't read the file, error

    만약 (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	만약 (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    아니면
    {
	// limit the offset to not go before the start of the file
	만약 (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	만약 (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    만약 (size <= 0)
	반환 OK;
    만약 (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	반환 OK;

    만약 (ga_grow(&blob->bv_ga, (정수)size) == FAIL)
	반환 FAIL;
    blob->bv_ga.ga_len = (정수)size;
    만약 (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = NULL;
	반환 FAIL;
    }
    반환 OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    정수
write_blob(FILE *fd, blob_T *blob)
{
    만약 (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	반환 FAIL;
    }
    반환 OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    static const char hex_chars[] = "0123456789ABCDEF";
    정수		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    만약 (blob == NULL || (blen = blob_len(blob)) == 0)
    {
	*tofree = NULL;
	반환 (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    만약 (buf == NULL)
    {
	*tofree = NULL;
	반환 (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    위해 (정수 i = 0; i < blen; i++)
    {
	unsigned b;

	만약 (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    반환 buf;
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

    만약 (rettv_list_alloc(rettv) == FAIL)
	반환;

    위해 (정수 i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	만약 (l2 == NULL)
	    반환;

	만약 (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    반환;
	}

	만약 (list_append_number(l2, i) == FAIL
		|| list_append_number(l2, blob_get(blob, i)) == FAIL)
	    반환;
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

    만약 (blob == NULL)
	반환 NULL;
    만약 (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	goto failed;
    s += 2;
    동안 (vim_isxdigit(*s))
    {
	만약 (!vim_isxdigit(s[1]))
	    goto failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	만약 (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    만약 (*skipwhite(s) != NUL)
	goto failed;  // text after final digit

    ++blob->bv_refcount;
    반환 blob;

failed:
    blob_free(blob);
    반환 NULL;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    static 정수
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	정수		exclusive,
	typval_T	*rettv)
{
    만약 (n1 < 0)
    {
	n1 = len + n1;
	만약 (n1 < 0)
	    n1 = 0;
    }
    만약 (n2 < 0)
	n2 = len + n2;
    아니면 만약 (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    만약 (exclusive)
	--n2;
    만약 (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = NULL;
    }
    아니면
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	만약 (new_blob != NULL)
	{
	    만약 (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		반환 FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    위해 (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    반환 OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    static 정수
blob_index(
	blob_T		*blob,
	정수		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    만약 (idx < 0)
	idx = len + idx;
    만약 (idx < len && idx >= 0)
    {
	정수 v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = v;
    }
    아니면
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	반환 FAIL;
    }

    반환 OK;
}

    정수
blob_slice_or_index(
	blob_T		*blob,
	정수		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	정수		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    만약 (is_range)
	반환 blob_slice(blob, len, n1, n2, exclusive, rettv);
    아니면
	반환 blob_index(blob, len, n1, rettv);
    반환 OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    정수
check_blob_index(long bloblen, varnumber_T n1, 정수 quiet)
{
    만약 (n1 < 0 || n1 > bloblen)
    {
	만약 (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	반환 FAIL;
    }
    반환 OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    정수
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, 정수 quiet)
{
    만약 (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	만약 (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	반환 FAIL;
    }
    반환 OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    정수
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    정수	il, ir;

    만약 (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_value_does_not_have_right_number_of_bytes));
	반환 FAIL;
    }

    ir = 0;
    위해 (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    반환 OK;
}

/*
 * "add(blob, item)" function
 */
    void
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    정수		error = FALSE;
    varnumber_T n;

    만약 (b == NULL)
    {
	만약 (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	반환;
    }

    만약 (value_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	반환;

    n = tv_get_number_chk(&argvars[1], &error);
    만약 (error)
	반환;

    ga_append(&b->bv_ga, (정수)n);
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
    정수		error = FALSE;
    long	idx;
    long	end;
    정수		len;
    char_u	*p;

    만약 (b != NULL && value_check_lock(b->bv_lock, arg_errmsg, TRUE))
	반환;

    idx = (long)tv_get_number_chk(&argvars[1], &error);
    만약 (error)
	반환;

    len = blob_len(b);

    만약 (idx < 0)
	// count from the end
	idx = len + idx;
    만약 (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	반환;
    }
    만약 (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_number = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	반환;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_number_chk(&argvars[2], &error);
    만약 (error)
	반환;
    만약 (end < 0)
	// count from the end
	end = len + end;
    만약 (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	반환;
    }
    newblob = blob_alloc();
    만약 (newblob == NULL)
	반환;
    newblob->bv_ga.ga_len = end - idx + 1;
    만약 (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	반환;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    만약 (len - end - 1 > 0)
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
    정수		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    정수		idx = 0;
    정수		rem;
    typval_T	newtv;
    funccall_T	*fc;

    만약 (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = NULL;
    }
    만약 (b == NULL || (filtermap == FILTERMAP_FILTER
			    && value_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	반환;

    b_ret = b;
    만약 (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	만약 (rettv->vval.v_blob == NULL)
	    반환;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    정수 prev_lock = b->bv_lock;
    만약 (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    위해 (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_NUMBER;
	val = blob_get(b, i);
	tv.vval.v_number = val;
	set_vim_var_nr(VV_KEY, idx);
	만약 (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    break;
	만약 (filtermap != FILTERMAP_FOREACH)
	{
	    만약 (newtv.v_type != VAR_NUMBER && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		break;
	    }
	    만약 (filtermap != FILTERMAP_FILTER)
	    {
		만약 (newtv.vval.v_number != val)
		    blob_set(b_ret, i, newtv.vval.v_number);
	    }
	    아니면 만약 (rem)
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
    만약 (fc != NULL)
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
    정수		error = FALSE;
    정수		val, len;
    char_u	*p;

    만약 (b == NULL)
    {
	만약 (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	반환;
    }

    만약 (value_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	반환;

    len = blob_len(b);
    만약 (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_number_chk(&argvars[2], &error);
	만약 (error)
	    반환;		// type error; errmsg already given
	만약 (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    반환;
	}
    }
    val = tv_get_number_chk(&argvars[1], &error);
    만약 (error)
	반환;
    만약 (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	반환;
    }

    만약 (ga_grow(&b->bv_ga, 1) == FAIL)
	반환;
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
    static 정수
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    정수		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    만약 (b2 == NULL || b2->bv_ga.ga_len == 0)
	반환 OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    만약 (ga_grow(&b1->bv_ga, len2) == FAIL)
	반환 FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    만약 (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	만약 (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    아니면
    {
	만약 (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    반환 OK;
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
	정수		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    정수		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    만약 (b1 == NULL)
    {
	emsg(_(e_cannot_extend_null_blob));
	반환;
    }
    만약 (is_new || !value_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	만약 (is_new)
	{
	    b1 = blob_copy(b1);
	    만약 (b1 == NULL)
		반환;
	}

	b2 = argvars[1].vval.v_blob;
	만약 (b2 == NULL)
	    goto theend;

	만약 (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_number_chk(&argvars[2], &error);
	    만약 (error)
		goto cleanup;		// type error; errmsg already given
	    만약 (before < 0)
		before = b1->bv_ga.ga_len + before;
	    만약 (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		goto cleanup;
	    }
	}
	아니면
	    before = b1->bv_ga.ga_len;
	만약 (blob_extend(b1, b2, before) == FAIL)
	    goto cleanup;

theend:
	만약 (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	아니면
	    copy_tv(&argvars[0], rettv);
	반환;

cleanup:
	만약 (is_new)
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
    정수		called_emsg_start = called_emsg;
    정수		r;
    typval_T	initial;
    typval_T	argv[3];
    정수	i;

    만약 (argvars[2].v_type == VAR_UNKNOWN)
    {
	만약 (b == NULL || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Blob");
	    반환;
	}
	initial.v_type = VAR_NUMBER;
	initial.vval.v_number = blob_get(b, 0);
	i = 1;
    }
    아니면 만약 (check_for_number_arg(argvars, 2) == FAIL)
	반환;
    아니면
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    만약 (b == NULL)
	반환;

    위해 ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_NUMBER;
	argv[1].vval.v_number = blob_get(b, i);

	r = eval_expr_typval(expr, TRUE, argv, 2, NULL, rettv);

	clear_tv(&argv[0]);
	만약 (r == FAIL || called_emsg != called_emsg_start)
	    반환;
    }
}

/*
 * "reverse({blob})" function
 */
    void
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    위해 (i = 0; i < len / 2; i++)
    {
	정수 tmp = blob_get(b, (정수)i);

	blob_set(b, (정수)i, blob_get(b, (정수)(len - i - 1)));
	blob_set(b, (정수)(len - i - 1), tmp);
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
    정수		i, len;
    typval_T	tv;

    만약 (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(void)rettv_list_alloc(rettv);
	반환;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_items(len);
    만약 (l == NULL)
	반환;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_NUMBER;
    tv.v_lock = 0;
    위해 (i = 0; i < len; i++)
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

    만약 (rettv_blob_alloc(rettv) == FAIL)
	반환;
    blob = rettv->vval.v_blob;

    만약 (check_for_list_arg(argvars, 0) == FAIL)
	반환;

    l = argvars->vval.v_list;
    만약 (l == NULL)
	반환;

    // The result has one byte per list item; reserve all space at once.
    만약 (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	반환;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	정수		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_number_chk(&li->li_tv, &error);
	만약 (error == TRUE || n < 0 || n > 255)
	{
	    만약 (!error)
		semsg(_(e_invalid_value_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    반환;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
