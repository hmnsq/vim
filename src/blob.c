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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(ശൂന്യം)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    എങ്കിൽ (blob != ശൂന്യ)
	ga_init2(&blob->bv_ga, 1, 100);
    തിരികെ blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    എങ്കിൽ (b == ശൂന്യ)
	തിരികെ FAIL;

    rettv_blob_set(rettv, b);
    തിരികെ OK;
}

/*
 * Set a blob as the return value.
 */
    ശൂന്യം
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    എങ്കിൽ (b != ശൂന്യ)
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
    പൂർണ്ണസംഖ്യ		len;
    blob_T	*copy;

    എങ്കിൽ (orig == ശൂന്യ)
	തിരികെ ശൂന്യ;

    copy = blob_alloc();
    എങ്കിൽ (copy == ശൂന്യ)
	തിരികെ ശൂന്യ;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    എങ്കിൽ (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	എങ്കിൽ (copy->bv_ga.ga_data == ശൂന്യ)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    തിരികെ copy;
}

    ശൂന്യം
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    ശൂന്യം
blob_unref(blob_T *b)
{
    എങ്കിൽ (b != ശൂന്യ && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    എങ്കിൽ (b == ശൂന്യ)
	തിരികെ 0L;
    തിരികെ b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    പൂർണ്ണസംഖ്യ
blob_get(blob_T *b, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    ശൂന്യം
blob_set(blob_T *blob, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    ശൂന്യം
blob_set_append(blob_T *blob, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    എങ്കിൽ (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	എങ്കിൽ (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    പൂർണ്ണസംഖ്യ
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    പൂർണ്ണസംഖ്യ	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    എങ്കിൽ (len1 == 0 && len2 == 0)
	തിരികെ TRUE;
    എങ്കിൽ (b1 == b2)
	തിരികെ TRUE;
    എങ്കിൽ (len1 != len2)
	തിരികെ FALSE;

    വേണ്ടി (i = 0; i < b1->bv_ga.ga_len; i++)
	എങ്കിൽ (blob_get(b1, i) != blob_get(b2, i)) തിരികെ FALSE;
    തിരികെ TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    ഘടന stat	st;
    പൂർണ്ണസംഖ്യ		whence;
    off_T	size = size_arg;

    എങ്കിൽ (fstat(fileno(fd), &st) < 0)
	തിരികെ FAIL;  // can't read the file, error

    എങ്കിൽ (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	എങ്കിൽ (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    അല്ലെങ്കിൽ
    {
	// limit the offset to not go before the start of the file
	എങ്കിൽ (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	എങ്കിൽ (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    എങ്കിൽ (size <= 0)
	തിരികെ OK;
    എങ്കിൽ (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	തിരികെ OK;

    എങ്കിൽ (ga_grow(&blob->bv_ga, (പൂർണ്ണസംഖ്യ)size) == FAIL)
	തിരികെ FAIL;
    blob->bv_ga.ga_len = (പൂർണ്ണസംഖ്യ)size;
    എങ്കിൽ (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = ശൂന്യ;
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
write_blob(FILE *fd, blob_T *blob)
{
    എങ്കിൽ (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    സ്ഥിതം സ്ഥിരം അക്ഷരം hex_chars[] = "0123456789ABCDEF";
    പൂർണ്ണസംഖ്യ		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    എങ്കിൽ (blob == ശൂന്യ || (blen = blob_len(blob)) == 0)
    {
	*tofree = ശൂന്യ;
	തിരികെ (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    എങ്കിൽ (buf == ശൂന്യ)
    {
	*tofree = ശൂന്യ;
	തിരികെ (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < blen; i++)
    {
	unsigned b;

	എങ്കിൽ (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    തിരികെ buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    ശൂന്യം
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	എങ്കിൽ (l2 == ശൂന്യ)
	    തിരികെ;

	എങ്കിൽ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    തിരികെ;
	}

	എങ്കിൽ (list_append_സംഖ്യ(l2, i) == FAIL
		|| list_append_സംഖ്യ(l2, blob_get(blob, i)) == FAIL)
	    തിരികെ;
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

    എങ്കിൽ (blob == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	പോകുക failed;
    s += 2;
    ആകുമ്പോൾ (vim_isxdigit(*s))
    {
	എങ്കിൽ (!vim_isxdigit(s[1]))
	    പോകുക failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	എങ്കിൽ (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    എങ്കിൽ (*skipwhite(s) != NUL)
	പോകുക failed;  // text after final digit

    ++blob->bv_refcount;
    തിരികെ blob;

failed:
    blob_free(blob);
    തിരികെ ശൂന്യ;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	പൂർണ്ണസംഖ്യ		exclusive,
	typval_T	*rettv)
{
    എങ്കിൽ (n1 < 0)
    {
	n1 = len + n1;
	എങ്കിൽ (n1 < 0)
	    n1 = 0;
    }
    എങ്കിൽ (n2 < 0)
	n2 = len + n2;
    അല്ലെങ്കിൽ എങ്കിൽ (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    എങ്കിൽ (exclusive)
	--n2;
    എങ്കിൽ (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	എങ്കിൽ (new_blob != ശൂന്യ)
	{
	    എങ്കിൽ (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		തിരികെ FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    വേണ്ടി (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    തിരികെ OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
blob_index(
	blob_T		*blob,
	പൂർണ്ണസംഖ്യ		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    എങ്കിൽ (idx < 0)
	idx = len + idx;
    എങ്കിൽ (idx < len && idx >= 0)
    {
	പൂർണ്ണസംഖ്യ v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_സംഖ്യ;
	rettv->vval.v_സംഖ്യ = v;
    }
    അല്ലെങ്കിൽ
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
blob_slice_or_index(
	blob_T		*blob,
	പൂർണ്ണസംഖ്യ		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	പൂർണ്ണസംഖ്യ		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    എങ്കിൽ (is_range)
	തിരികെ blob_slice(blob, len, n1, n2, exclusive, rettv);
    അല്ലെങ്കിൽ
	തിരികെ blob_index(blob, len, n1, rettv);
    തിരികെ OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    പൂർണ്ണസംഖ്യ
check_blob_index(long bloblen, varnumber_T n1, പൂർണ്ണസംഖ്യ quiet)
{
    എങ്കിൽ (n1 < 0 || n1 > bloblen)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    പൂർണ്ണസംഖ്യ
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, പൂർണ്ണസംഖ്യ quiet)
{
    എങ്കിൽ (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    പൂർണ്ണസംഖ്യ
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    പൂർണ്ണസംഖ്യ	il, ir;

    എങ്കിൽ (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_മൂല്യം_does_not_have_വലത്_സംഖ്യ_of_bytes));
	തിരികെ FAIL;
    }

    ir = 0;
    വേണ്ടി (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    തിരികെ OK;
}

/*
 * "add(blob, item)" function
 */
    ശൂന്യം
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    varnumber_T n;

    എങ്കിൽ (b == ശൂന്യ)
    {
	എങ്കിൽ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	തിരികെ;
    }

    എങ്കിൽ (മൂല്യം_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	തിരികെ;

    n = tv_get_സംഖ്യ_chk(&argvars[1], &error);
    എങ്കിൽ (error)
	തിരികെ;

    ga_append(&b->bv_ga, (പൂർണ്ണസംഖ്യ)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    ശൂന്യം
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    long	idx;
    long	end;
    പൂർണ്ണസംഖ്യ		len;
    char_u	*p;

    എങ്കിൽ (b != ശൂന്യ && മൂല്യം_check_lock(b->bv_lock, arg_errmsg, TRUE))
	തിരികെ;

    idx = (long)tv_get_സംഖ്യ_chk(&argvars[1], &error);
    എങ്കിൽ (error)
	തിരികെ;

    len = blob_len(b);

    എങ്കിൽ (idx < 0)
	// count from the end
	idx = len + idx;
    എങ്കിൽ (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	തിരികെ;
    }
    എങ്കിൽ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_സംഖ്യ = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	തിരികെ;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_സംഖ്യ_chk(&argvars[2], &error);
    എങ്കിൽ (error)
	തിരികെ;
    എങ്കിൽ (end < 0)
	// count from the end
	end = len + end;
    എങ്കിൽ (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	തിരികെ;
    }
    newblob = blob_alloc();
    എങ്കിൽ (newblob == ശൂന്യ)
	തിരികെ;
    newblob->bv_ga.ga_len = end - idx + 1;
    എങ്കിൽ (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	തിരികെ;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    എങ്കിൽ (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (size_t)(len - end - 1));
    b->bv_ga.ga_len -= end - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    ശൂന്യം
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*വ്യഞ്ജകം,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    പൂർണ്ണസംഖ്യ		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    പൂർണ്ണസംഖ്യ		idx = 0;
    പൂർണ്ണസംഖ്യ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    എങ്കിൽ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ശൂന്യ;
    }
    എങ്കിൽ (b == ശൂന്യ || (filtermap == FILTERMAP_FILTER
			    && മൂല്യം_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	തിരികെ;

    b_ret = b;
    എങ്കിൽ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	എങ്കിൽ (rettv->vval.v_blob == ശൂന്യ)
	    തിരികെ;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_താക്കോൽ, VAR_സംഖ്യ);

    പൂർണ്ണസംഖ്യ prev_lock = b->bv_lock;
    എങ്കിൽ (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_വ്യഞ്ജകം_get_funccal(വ്യഞ്ജകം, &newtv);

    വേണ്ടി (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_സംഖ്യ;
	val = blob_get(b, i);
	tv.vval.v_സംഖ്യ = val;
	set_vim_var_nr(VV_താക്കോൽ, idx);
	എങ്കിൽ (filter_map_one(&tv, വ്യഞ്ജകം, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    നിർത്തുക;
	എങ്കിൽ (filtermap != FILTERMAP_FOREACH)
	{
	    എങ്കിൽ (newtv.v_type != VAR_സംഖ്യ && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		നിർത്തുക;
	    }
	    എങ്കിൽ (filtermap != FILTERMAP_FILTER)
	    {
		എങ്കിൽ (newtv.vval.v_സംഖ്യ != val)
		    blob_set(b_ret, i, newtv.vval.v_സംഖ്യ);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (rem)
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
    എങ്കിൽ (fc != ശൂന്യ)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    ശൂന്യം
blob_തിരുകുക_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    പൂർണ്ണസംഖ്യ		val, len;
    char_u	*p;

    എങ്കിൽ (b == ശൂന്യ)
    {
	എങ്കിൽ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	തിരികെ;
    }

    എങ്കിൽ (മൂല്യം_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	തിരികെ;

    len = blob_len(b);
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_സംഖ്യ_chk(&argvars[2], &error);
	എങ്കിൽ (error)
	    തിരികെ;		// type error; errmsg already given
	എങ്കിൽ (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    തിരികെ;
	}
    }
    val = tv_get_സംഖ്യ_chk(&argvars[1], &error);
    എങ്കിൽ (error)
	തിരികെ;
    എങ്കിൽ (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	തിരികെ;
    }

    എങ്കിൽ (ga_grow(&b->bv_ga, 1) == FAIL)
	തിരികെ;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    പൂർണ്ണസംഖ്യ		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    എങ്കിൽ (b2 == ശൂന്യ || b2->bv_ga.ga_len == 0)
	തിരികെ OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    എങ്കിൽ (ga_grow(&b1->bv_ga, len2) == FAIL)
	തിരികെ FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    എങ്കിൽ (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	എങ്കിൽ (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    തിരികെ OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ശൂന്യം
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	പൂർണ്ണസംഖ്യ		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    എങ്കിൽ (b1 == ശൂന്യ)
    {
	emsg(_(e_cannot_extend_null_blob));
	തിരികെ;
    }
    എങ്കിൽ (is_new || !മൂല്യം_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	എങ്കിൽ (is_new)
	{
	    b1 = blob_copy(b1);
	    എങ്കിൽ (b1 == ശൂന്യ)
		തിരികെ;
	}

	b2 = argvars[1].vval.v_blob;
	എങ്കിൽ (b2 == ശൂന്യ)
	    പോകുക theend;

	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_സംഖ്യ_chk(&argvars[2], &error);
	    എങ്കിൽ (error)
		പോകുക cleanup;		// type error; errmsg already given
	    എങ്കിൽ (before < 0)
		before = b1->bv_ga.ga_len + before;
	    എങ്കിൽ (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		പോകുക cleanup;
	    }
	}
	അല്ലെങ്കിൽ
	    before = b1->bv_ga.ga_len;
	എങ്കിൽ (blob_extend(b1, b2, before) == FAIL)
	    പോകുക cleanup;

theend:
	എങ്കിൽ (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	അല്ലെങ്കിൽ
	    copy_tv(&argvars[0], rettv);
	തിരികെ;

cleanup:
	എങ്കിൽ (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    ശൂന്യം
blob_reduce(
	typval_T	*argvars,
	typval_T	*വ്യഞ്ജകം,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    പൂർണ്ണസംഖ്യ		called_emsg_start = called_emsg;
    പൂർണ്ണസംഖ്യ		r;
    typval_T	initial;
    typval_T	argv[3];
    പൂർണ്ണസംഖ്യ	i;

    എങ്കിൽ (argvars[2].v_type == VAR_UNKNOWN)
    {
	എങ്കിൽ (b == ശൂന്യ || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_മൂല്യം), "Blob");
	    തിരികെ;
	}
	initial.v_type = VAR_സംഖ്യ;
	initial.vval.v_സംഖ്യ = blob_get(b, 0);
	i = 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (check_for_സംഖ്യ_arg(argvars, 2) == FAIL)
	തിരികെ;
    അല്ലെങ്കിൽ
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    എങ്കിൽ (b == ശൂന്യ)
	തിരികെ;

    വേണ്ടി ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_സംഖ്യ;
	argv[1].vval.v_സംഖ്യ = blob_get(b, i);

	r = eval_വ്യഞ്ജകം_typval(വ്യഞ്ജകം, TRUE, argv, 2, ശൂന്യ, rettv);

	clear_tv(&argv[0]);
	എങ്കിൽ (r == FAIL || called_emsg != called_emsg_start)
	    തിരികെ;
    }
}

/*
 * "reverse({blob})" function
 */
    ശൂന്യം
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    വേണ്ടി (i = 0; i < len / 2; i++)
    {
	പൂർണ്ണസംഖ്യ tmp = blob_get(b, (പൂർണ്ണസംഖ്യ)i);

	blob_set(b, (പൂർണ്ണസംഖ്യ)i, blob_get(b, (പൂർണ്ണസംഖ്യ)(len - i - 1)));
	blob_set(b, (പൂർണ്ണസംഖ്യ)(len - i - 1), tmp);
    }
    rettv_blob_set(rettv, b);
}

/*
 * blob2list() function
 */
    ശൂന്യം
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    പൂർണ്ണസംഖ്യ		i, len;
    typval_T	tv;

    എങ്കിൽ (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(ശൂന്യം)rettv_list_alloc(rettv);
	തിരികെ;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_ഇനങ്ങൾ(len);
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_സംഖ്യ;
    tv.v_lock = 0;
    വേണ്ടി (i = 0; i < len; i++)
    {
	tv.vval.v_സംഖ്യ = blob_get(blob, i);
	list_set_item(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    ശൂന്യം
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    എങ്കിൽ (rettv_blob_alloc(rettv) == FAIL)
	തിരികെ;
    blob = rettv->vval.v_blob;

    എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL)
	തിരികെ;

    l = argvars->vval.v_list;
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;

    // The result has one byte per list item; reserve all space at once.
    എങ്കിൽ (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	തിരികെ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	പൂർണ്ണസംഖ്യ		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_സംഖ്യ_chk(&li->li_tv, &error);
	എങ്കിൽ (error == TRUE || n < 0 || n > 255)
	{
	    എങ്കിൽ (!error)
		semsg(_(e_invalid_മൂല്യം_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    തിരികെ;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
