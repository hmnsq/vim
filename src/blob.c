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

#nếu defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(void)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    nếu (blob != NULL)
	ga_init2(&blob->bv_ga, 1, 100);
    trả_về blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    số_nguyên
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    nếu (b == NULL)
	trả_về FAIL;

    rettv_blob_set(rettv, b);
    trả_về OK;
}

/*
 * Set a blob as the return value.
 */
    void
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    nếu (b != NULL)
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
    số_nguyên		len;
    blob_T	*copy;

    nếu (orig == NULL)
	trả_về NULL;

    copy = blob_alloc();
    nếu (copy == NULL)
	trả_về NULL;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    nếu (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	nếu (copy->bv_ga.ga_data == NULL)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    trả_về copy;
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
    nếu (b != NULL && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    nếu (b == NULL)
	trả_về 0L;
    trả_về b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    số_nguyên
blob_get(blob_T *b, số_nguyên idx)
{
    trả_về ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    void
blob_set(blob_T *blob, số_nguyên idx, số_nguyên byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    void
blob_set_append(blob_T *blob, số_nguyên idx, số_nguyên byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    nếu (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	nếu (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    số_nguyên
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    số_nguyên	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    nếu (len1 == 0 && len2 == 0)
	trả_về TRUE;
    nếu (b1 == b2)
	trả_về TRUE;
    nếu (len1 != len2)
	trả_về FALSE;

    cho (i = 0; i < b1->bv_ga.ga_len; i++)
	nếu (blob_get(b1, i) != blob_get(b2, i)) trả_về FALSE;
    trả_về TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    số_nguyên
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    struct stat	st;
    số_nguyên		whence;
    off_T	size = size_arg;

    nếu (fstat(fileno(fd), &st) < 0)
	trả_về FAIL;  // can't read the file, error

    nếu (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	nếu (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    không_thì
    {
	// limit the offset to not go before the start of the file
	nếu (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	nếu (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    nếu (size <= 0)
	trả_về OK;
    nếu (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	trả_về OK;

    nếu (ga_grow(&blob->bv_ga, (số_nguyên)size) == FAIL)
	trả_về FAIL;
    blob->bv_ga.ga_len = (số_nguyên)size;
    nếu (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = NULL;
	trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    số_nguyên
write_blob(FILE *fd, blob_T *blob)
{
    nếu (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    static const char hex_chars[] = "0123456789ABCDEF";
    số_nguyên		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    nếu (blob == NULL || (blen = blob_len(blob)) == 0)
    {
	*tofree = NULL;
	trả_về (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    nếu (buf == NULL)
    {
	*tofree = NULL;
	trả_về (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    cho (số_nguyên i = 0; i < blen; i++)
    {
	unsigned b;

	nếu (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    trả_về buf;
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

    nếu (rettv_list_alloc(rettv) == FAIL)
	trả_về;

    cho (số_nguyên i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	nếu (l2 == NULL)
	    trả_về;

	nếu (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    trả_về;
	}

	nếu (list_append_number(l2, i) == FAIL
		|| list_append_number(l2, blob_get(blob, i)) == FAIL)
	    trả_về;
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

    nếu (blob == NULL)
	trả_về NULL;
    nếu (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	goto failed;
    s += 2;
    trong_khi (vim_isxdigit(*s))
    {
	nếu (!vim_isxdigit(s[1]))
	    goto failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	nếu (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    nếu (*skipwhite(s) != NUL)
	goto failed;  // text after final digit

    ++blob->bv_refcount;
    trả_về blob;

failed:
    blob_free(blob);
    trả_về NULL;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    static số_nguyên
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	số_nguyên		exclusive,
	typval_T	*rettv)
{
    nếu (n1 < 0)
    {
	n1 = len + n1;
	nếu (n1 < 0)
	    n1 = 0;
    }
    nếu (n2 < 0)
	n2 = len + n2;
    không_thì nếu (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    nếu (exclusive)
	--n2;
    nếu (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = NULL;
    }
    không_thì
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	nếu (new_blob != NULL)
	{
	    nếu (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		trả_về FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    cho (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    trả_về OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    static số_nguyên
blob_index(
	blob_T		*blob,
	số_nguyên		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    nếu (idx < 0)
	idx = len + idx;
    nếu (idx < len && idx >= 0)
    {
	số_nguyên v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = v;
    }
    không_thì
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	trả_về FAIL;
    }

    trả_về OK;
}

    số_nguyên
blob_slice_or_index(
	blob_T		*blob,
	số_nguyên		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	số_nguyên		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    nếu (is_range)
	trả_về blob_slice(blob, len, n1, n2, exclusive, rettv);
    không_thì
	trả_về blob_index(blob, len, n1, rettv);
    trả_về OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    số_nguyên
check_blob_index(long bloblen, varnumber_T n1, số_nguyên quiet)
{
    nếu (n1 < 0 || n1 > bloblen)
    {
	nếu (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    số_nguyên
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, số_nguyên quiet)
{
    nếu (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	nếu (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    số_nguyên
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    số_nguyên	il, ir;

    nếu (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_value_does_not_have_right_number_of_bytes));
	trả_về FAIL;
    }

    ir = 0;
    cho (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    trả_về OK;
}

/*
 * "add(blob, item)" function
 */
    void
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    số_nguyên		error = FALSE;
    varnumber_T n;

    nếu (b == NULL)
    {
	nếu (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	trả_về;
    }

    nếu (value_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	trả_về;

    n = tv_get_number_chk(&argvars[1], &error);
    nếu (error)
	trả_về;

    ga_append(&b->bv_ga, (số_nguyên)n);
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
    số_nguyên		error = FALSE;
    long	idx;
    long	end;
    số_nguyên		len;
    char_u	*p;

    nếu (b != NULL && value_check_lock(b->bv_lock, arg_errmsg, TRUE))
	trả_về;

    idx = (long)tv_get_number_chk(&argvars[1], &error);
    nếu (error)
	trả_về;

    len = blob_len(b);

    nếu (idx < 0)
	// count from the end
	idx = len + idx;
    nếu (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	trả_về;
    }
    nếu (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_number = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	trả_về;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_number_chk(&argvars[2], &error);
    nếu (error)
	trả_về;
    nếu (end < 0)
	// count from the end
	end = len + end;
    nếu (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	trả_về;
    }
    newblob = blob_alloc();
    nếu (newblob == NULL)
	trả_về;
    newblob->bv_ga.ga_len = end - idx + 1;
    nếu (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	trả_về;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    nếu (len - end - 1 > 0)
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
    số_nguyên		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    số_nguyên		idx = 0;
    số_nguyên		rem;
    typval_T	newtv;
    funccall_T	*fc;

    nếu (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = NULL;
    }
    nếu (b == NULL || (filtermap == FILTERMAP_FILTER
			    && value_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	trả_về;

    b_ret = b;
    nếu (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	nếu (rettv->vval.v_blob == NULL)
	    trả_về;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    số_nguyên prev_lock = b->bv_lock;
    nếu (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    cho (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_NUMBER;
	val = blob_get(b, i);
	tv.vval.v_number = val;
	set_vim_var_nr(VV_KEY, idx);
	nếu (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    break;
	nếu (filtermap != FILTERMAP_FOREACH)
	{
	    nếu (newtv.v_type != VAR_NUMBER && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		break;
	    }
	    nếu (filtermap != FILTERMAP_FILTER)
	    {
		nếu (newtv.vval.v_number != val)
		    blob_set(b_ret, i, newtv.vval.v_number);
	    }
	    không_thì nếu (rem)
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
    nếu (fc != NULL)
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
    số_nguyên		error = FALSE;
    số_nguyên		val, len;
    char_u	*p;

    nếu (b == NULL)
    {
	nếu (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	trả_về;
    }

    nếu (value_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	trả_về;

    len = blob_len(b);
    nếu (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_number_chk(&argvars[2], &error);
	nếu (error)
	    trả_về;		// type error; errmsg already given
	nếu (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    trả_về;
	}
    }
    val = tv_get_number_chk(&argvars[1], &error);
    nếu (error)
	trả_về;
    nếu (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	trả_về;
    }

    nếu (ga_grow(&b->bv_ga, 1) == FAIL)
	trả_về;
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
    static số_nguyên
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    số_nguyên		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    nếu (b2 == NULL || b2->bv_ga.ga_len == 0)
	trả_về OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    nếu (ga_grow(&b1->bv_ga, len2) == FAIL)
	trả_về FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    nếu (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	nếu (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    không_thì
    {
	nếu (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    trả_về OK;
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
	số_nguyên		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    số_nguyên		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    nếu (b1 == NULL)
    {
	emsg(_(e_cannot_extend_null_blob));
	trả_về;
    }
    nếu (is_new || !value_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	nếu (is_new)
	{
	    b1 = blob_copy(b1);
	    nếu (b1 == NULL)
		trả_về;
	}

	b2 = argvars[1].vval.v_blob;
	nếu (b2 == NULL)
	    goto theend;

	nếu (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_number_chk(&argvars[2], &error);
	    nếu (error)
		goto cleanup;		// type error; errmsg already given
	    nếu (before < 0)
		before = b1->bv_ga.ga_len + before;
	    nếu (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		goto cleanup;
	    }
	}
	không_thì
	    before = b1->bv_ga.ga_len;
	nếu (blob_extend(b1, b2, before) == FAIL)
	    goto cleanup;

theend:
	nếu (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	không_thì
	    copy_tv(&argvars[0], rettv);
	trả_về;

cleanup:
	nếu (is_new)
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
    số_nguyên		called_emsg_start = called_emsg;
    số_nguyên		r;
    typval_T	initial;
    typval_T	argv[3];
    số_nguyên	i;

    nếu (argvars[2].v_type == VAR_UNKNOWN)
    {
	nếu (b == NULL || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Blob");
	    trả_về;
	}
	initial.v_type = VAR_NUMBER;
	initial.vval.v_number = blob_get(b, 0);
	i = 1;
    }
    không_thì nếu (check_for_number_arg(argvars, 2) == FAIL)
	trả_về;
    không_thì
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    nếu (b == NULL)
	trả_về;

    cho ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_NUMBER;
	argv[1].vval.v_number = blob_get(b, i);

	r = eval_expr_typval(expr, TRUE, argv, 2, NULL, rettv);

	clear_tv(&argv[0]);
	nếu (r == FAIL || called_emsg != called_emsg_start)
	    trả_về;
    }
}

/*
 * "reverse({blob})" function
 */
    void
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    cho (i = 0; i < len / 2; i++)
    {
	số_nguyên tmp = blob_get(b, (số_nguyên)i);

	blob_set(b, (số_nguyên)i, blob_get(b, (số_nguyên)(len - i - 1)));
	blob_set(b, (số_nguyên)(len - i - 1), tmp);
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
    số_nguyên		i, len;
    typval_T	tv;

    nếu (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(void)rettv_list_alloc(rettv);
	trả_về;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_items(len);
    nếu (l == NULL)
	trả_về;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_NUMBER;
    tv.v_lock = 0;
    cho (i = 0; i < len; i++)
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

    nếu (rettv_blob_alloc(rettv) == FAIL)
	trả_về;
    blob = rettv->vval.v_blob;

    nếu (check_for_list_arg(argvars, 0) == FAIL)
	trả_về;

    l = argvars->vval.v_list;
    nếu (l == NULL)
	trả_về;

    // The result has one byte per list item; reserve all space at once.
    nếu (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	trả_về;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, li)
    {
	số_nguyên		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_number_chk(&li->li_tv, &error);
	nếu (error == TRUE || n < 0 || n > 255)
	{
	    nếu (!error)
		semsg(_(e_invalid_value_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    trả_về;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
