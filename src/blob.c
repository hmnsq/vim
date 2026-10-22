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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(શૂન્ય)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    જો (blob != ખાલી)
	ga_init2(&blob->bv_ga, 1, 100);
    પાછા blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    જો (b == ખાલી)
	પાછા FAIL;

    rettv_blob_set(rettv, b);
    પાછા OK;
}

/*
 * Set a blob as the return value.
 */
    શૂન્ય
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    જો (b != ખાલી)
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
    પૂર્ણાંક		len;
    blob_T	*copy;

    જો (orig == ખાલી)
	પાછા ખાલી;

    copy = blob_alloc();
    જો (copy == ખાલી)
	પાછા ખાલી;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    જો (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	જો (copy->bv_ga.ga_data == ખાલી)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    પાછા copy;
}

    શૂન્ય
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    શૂન્ય
blob_unref(blob_T *b)
{
    જો (b != ખાલી && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    જો (b == ખાલી)
	પાછા 0L;
    પાછા b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    પૂર્ણાંક
blob_get(blob_T *b, પૂર્ણાંક idx)
{
    પાછા ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    શૂન્ય
blob_set(blob_T *blob, પૂર્ણાંક idx, પૂર્ણાંક byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    શૂન્ય
blob_set_append(blob_T *blob, પૂર્ણાંક idx, પૂર્ણાંક byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    જો (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	જો (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    પૂર્ણાંક
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    પૂર્ણાંક	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    જો (len1 == 0 && len2 == 0)
	પાછા TRUE;
    જો (b1 == b2)
	પાછા TRUE;
    જો (len1 != len2)
	પાછા FALSE;

    માટે (i = 0; i < b1->bv_ga.ga_len; i++)
	જો (blob_get(b1, i) != blob_get(b2, i)) પાછા FALSE;
    પાછા TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    પૂર્ણાંક
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    સંરચના stat	st;
    પૂર્ણાંક		whence;
    off_T	size = size_arg;

    જો (fstat(fileno(fd), &st) < 0)
	પાછા FAIL;  // can't read the file, error

    જો (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	જો (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    નહીંતર
    {
	// limit the offset to not go before the start of the file
	જો (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	જો (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    જો (size <= 0)
	પાછા OK;
    જો (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	પાછા OK;

    જો (ga_grow(&blob->bv_ga, (પૂર્ણાંક)size) == FAIL)
	પાછા FAIL;
    blob->bv_ga.ga_len = (પૂર્ણાંક)size;
    જો (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = ખાલી;
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    પૂર્ણાંક
write_blob(FILE *fd, blob_T *blob)
{
    જો (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    સ્થિર અચલ અક્ષર hex_chars[] = "0123456789ABCDEF";
    પૂર્ણાંક		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    જો (blob == ખાલી || (blen = blob_len(blob)) == 0)
    {
	*tofree = ખાલી;
	પાછા (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    જો (buf == ખાલી)
    {
	*tofree = ખાલી;
	પાછા (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    માટે (પૂર્ણાંક i = 0; i < blen; i++)
    {
	unsigned b;

	જો (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    પાછા buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    શૂન્ય
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    માટે (પૂર્ણાંક i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	જો (l2 == ખાલી)
	    પાછા;

	જો (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    પાછા;
	}

	જો (list_append_સંખ્યા(l2, i) == FAIL
		|| list_append_સંખ્યા(l2, blob_get(blob, i)) == FAIL)
	    પાછા;
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

    જો (blob == ખાલી)
	પાછા ખાલી;
    જો (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	જાઓ failed;
    s += 2;
    જ્યારે (vim_isxdigit(*s))
    {
	જો (!vim_isxdigit(s[1]))
	    જાઓ failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	જો (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    જો (*skipwhite(s) != NUL)
	જાઓ failed;  // text after final digit

    ++blob->bv_refcount;
    પાછા blob;

failed:
    blob_free(blob);
    પાછા ખાલી;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    સ્થિર પૂર્ણાંક
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	પૂર્ણાંક		exclusive,
	typval_T	*rettv)
{
    જો (n1 < 0)
    {
	n1 = len + n1;
	જો (n1 < 0)
	    n1 = 0;
    }
    જો (n2 < 0)
	n2 = len + n2;
    નહીંતર જો (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    જો (exclusive)
	--n2;
    જો (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ખાલી;
    }
    નહીંતર
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	જો (new_blob != ખાલી)
	{
	    જો (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		પાછા FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    માટે (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    પાછા OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    સ્થિર પૂર્ણાંક
blob_index(
	blob_T		*blob,
	પૂર્ણાંક		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    જો (idx < 0)
	idx = len + idx;
    જો (idx < len && idx >= 0)
    {
	પૂર્ણાંક v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_સંખ્યા;
	rettv->vval.v_સંખ્યા = v;
    }
    નહીંતર
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	પાછા FAIL;
    }

    પાછા OK;
}

    પૂર્ણાંક
blob_slice_or_index(
	blob_T		*blob,
	પૂર્ણાંક		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	પૂર્ણાંક		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    જો (is_range)
	પાછા blob_slice(blob, len, n1, n2, exclusive, rettv);
    નહીંતર
	પાછા blob_index(blob, len, n1, rettv);
    પાછા OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    પૂર્ણાંક
check_blob_index(long bloblen, varnumber_T n1, પૂર્ણાંક quiet)
{
    જો (n1 < 0 || n1 > bloblen)
    {
	જો (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    પૂર્ણાંક
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, પૂર્ણાંક quiet)
{
    જો (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	જો (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    પૂર્ણાંક
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    પૂર્ણાંક	il, ir;

    જો (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_કિંમત_does_not_have_જમણું_સંખ્યા_of_bytes));
	પાછા FAIL;
    }

    ir = 0;
    માટે (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    પાછા OK;
}

/*
 * "add(blob, item)" function
 */
    શૂન્ય
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    પૂર્ણાંક		error = FALSE;
    varnumber_T n;

    જો (b == ખાલી)
    {
	જો (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	પાછા;
    }

    જો (કિંમત_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	પાછા;

    n = tv_get_સંખ્યા_chk(&argvars[1], &error);
    જો (error)
	પાછા;

    ga_append(&b->bv_ga, (પૂર્ણાંક)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    શૂન્ય
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    પૂર્ણાંક		error = FALSE;
    long	idx;
    long	end;
    પૂર્ણાંક		len;
    char_u	*p;

    જો (b != ખાલી && કિંમત_check_lock(b->bv_lock, arg_errmsg, TRUE))
	પાછા;

    idx = (long)tv_get_સંખ્યા_chk(&argvars[1], &error);
    જો (error)
	પાછા;

    len = blob_len(b);

    જો (idx < 0)
	// count from the end
	idx = len + idx;
    જો (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	પાછા;
    }
    જો (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_સંખ્યા = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	પાછા;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_સંખ્યા_chk(&argvars[2], &error);
    જો (error)
	પાછા;
    જો (end < 0)
	// count from the end
	end = len + end;
    જો (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	પાછા;
    }
    newblob = blob_alloc();
    જો (newblob == ખાલી)
	પાછા;
    newblob->bv_ga.ga_len = end - idx + 1;
    જો (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	પાછા;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    જો (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (size_t)(len - end - 1));
    b->bv_ga.ga_len -= end - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    શૂન્ય
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*પદાવલિ,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    પૂર્ણાંક		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    પૂર્ણાંક		idx = 0;
    પૂર્ણાંક		rem;
    typval_T	newtv;
    funccall_T	*fc;

    જો (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ખાલી;
    }
    જો (b == ખાલી || (filtermap == FILTERMAP_FILTER
			    && કિંમત_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	પાછા;

    b_ret = b;
    જો (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	જો (rettv->vval.v_blob == ખાલી)
	    પાછા;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ચાવી, VAR_સંખ્યા);

    પૂર્ણાંક prev_lock = b->bv_lock;
    જો (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_પદાવલિ_get_funccal(પદાવલિ, &newtv);

    માટે (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_સંખ્યા;
	val = blob_get(b, i);
	tv.vval.v_સંખ્યા = val;
	set_vim_var_nr(VV_ચાવી, idx);
	જો (filter_map_one(&tv, પદાવલિ, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    તોડો;
	જો (filtermap != FILTERMAP_FOREACH)
	{
	    જો (newtv.v_type != VAR_સંખ્યા && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		તોડો;
	    }
	    જો (filtermap != FILTERMAP_FILTER)
	    {
		જો (newtv.vval.v_સંખ્યા != val)
		    blob_set(b_ret, i, newtv.vval.v_સંખ્યા);
	    }
	    નહીંતર જો (rem)
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
    જો (fc != ખાલી)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    શૂન્ય
blob_ઉમેરો_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    પૂર્ણાંક		error = FALSE;
    પૂર્ણાંક		val, len;
    char_u	*p;

    જો (b == ખાલી)
    {
	જો (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	પાછા;
    }

    જો (કિંમત_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	પાછા;

    len = blob_len(b);
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_સંખ્યા_chk(&argvars[2], &error);
	જો (error)
	    પાછા;		// type error; errmsg already given
	જો (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    પાછા;
	}
    }
    val = tv_get_સંખ્યા_chk(&argvars[1], &error);
    જો (error)
	પાછા;
    જો (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	પાછા;
    }

    જો (ga_grow(&b->bv_ga, 1) == FAIL)
	પાછા;
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
    સ્થિર પૂર્ણાંક
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    પૂર્ણાંક		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    જો (b2 == ખાલી || b2->bv_ga.ga_len == 0)
	પાછા OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    જો (ga_grow(&b1->bv_ga, len2) == FAIL)
	પાછા FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    જો (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	જો (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    નહીંતર
    {
	જો (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    પાછા OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    શૂન્ય
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	પૂર્ણાંક		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    પૂર્ણાંક		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    જો (b1 == ખાલી)
    {
	emsg(_(e_cannot_extend_null_blob));
	પાછા;
    }
    જો (is_new || !કિંમત_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	જો (is_new)
	{
	    b1 = blob_copy(b1);
	    જો (b1 == ખાલી)
		પાછા;
	}

	b2 = argvars[1].vval.v_blob;
	જો (b2 == ખાલી)
	    જાઓ theend;

	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_સંખ્યા_chk(&argvars[2], &error);
	    જો (error)
		જાઓ cleanup;		// type error; errmsg already given
	    જો (before < 0)
		before = b1->bv_ga.ga_len + before;
	    જો (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		જાઓ cleanup;
	    }
	}
	નહીંતર
	    before = b1->bv_ga.ga_len;
	જો (blob_extend(b1, b2, before) == FAIL)
	    જાઓ cleanup;

theend:
	જો (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	નહીંતર
	    copy_tv(&argvars[0], rettv);
	પાછા;

cleanup:
	જો (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    શૂન્ય
blob_reduce(
	typval_T	*argvars,
	typval_T	*પદાવલિ,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    પૂર્ણાંક		called_emsg_start = called_emsg;
    પૂર્ણાંક		r;
    typval_T	initial;
    typval_T	argv[3];
    પૂર્ણાંક	i;

    જો (argvars[2].v_type == VAR_UNKNOWN)
    {
	જો (b == ખાલી || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_કિંમત), "Blob");
	    પાછા;
	}
	initial.v_type = VAR_સંખ્યા;
	initial.vval.v_સંખ્યા = blob_get(b, 0);
	i = 1;
    }
    નહીંતર જો (check_for_સંખ્યા_arg(argvars, 2) == FAIL)
	પાછા;
    નહીંતર
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    જો (b == ખાલી)
	પાછા;

    માટે ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_સંખ્યા;
	argv[1].vval.v_સંખ્યા = blob_get(b, i);

	r = eval_પદાવલિ_typval(પદાવલિ, TRUE, argv, 2, ખાલી, rettv);

	clear_tv(&argv[0]);
	જો (r == FAIL || called_emsg != called_emsg_start)
	    પાછા;
    }
}

/*
 * "reverse({blob})" function
 */
    શૂન્ય
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    માટે (i = 0; i < len / 2; i++)
    {
	પૂર્ણાંક tmp = blob_get(b, (પૂર્ણાંક)i);

	blob_set(b, (પૂર્ણાંક)i, blob_get(b, (પૂર્ણાંક)(len - i - 1)));
	blob_set(b, (પૂર્ણાંક)(len - i - 1), tmp);
    }
    rettv_blob_set(rettv, b);
}

/*
 * blob2list() function
 */
    શૂન્ય
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    પૂર્ણાંક		i, len;
    typval_T	tv;

    જો (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(શૂન્ય)rettv_list_alloc(rettv);
	પાછા;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_વસ્તુઓ(len);
    જો (l == ખાલી)
	પાછા;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_સંખ્યા;
    tv.v_lock = 0;
    માટે (i = 0; i < len; i++)
    {
	tv.vval.v_સંખ્યા = blob_get(blob, i);
	list_set_item(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    શૂન્ય
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    જો (rettv_blob_alloc(rettv) == FAIL)
	પાછા;
    blob = rettv->vval.v_blob;

    જો (check_for_list_arg(argvars, 0) == FAIL)
	પાછા;

    l = argvars->vval.v_list;
    જો (l == ખાલી)
	પાછા;

    // The result has one byte per list item; reserve all space at once.
    જો (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	પાછા;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	પૂર્ણાંક		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_સંખ્યા_chk(&li->li_tv, &error);
	જો (error == TRUE || n < 0 || n > 255)
	{
	    જો (!error)
		semsg(_(e_invalid_કિંમત_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    પાછા;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
