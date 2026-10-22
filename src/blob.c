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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(ਖਾਲੀ)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    ਜੇ (blob != ਨਿਰਾ)
	ga_init2(&blob->bv_ga, 1, 100);
    ਵਾਪਸ blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    ਜੇ (b == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    rettv_blob_ਸੈੱਟ(rettv, b);
    ਵਾਪਸ OK;
}

/*
 * Set a blob as the return value.
 */
    ਖਾਲੀ
rettv_blob_ਸੈੱਟ(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    ਜੇ (b != ਨਿਰਾ)
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
    ਪੂਰਨ		len;
    blob_T	*copy;

    ਜੇ (orig == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    copy = blob_alloc();
    ਜੇ (copy == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    ਜੇ (len > 0)
    {
	copy->bv_ga.ga_ਅੰਕੜੇ =
	    vim_memsave(orig->bv_ga.ga_ਅੰਕੜੇ, len);
	ਜੇ (copy->bv_ga.ga_ਅੰਕੜੇ == ਨਿਰਾ)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    ਵਾਪਸ copy;
}

    ਖਾਲੀ
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    ਖਾਲੀ
blob_unref(blob_T *b)
{
    ਜੇ (b != ਨਿਰਾ && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    ਲੰਮਾ
blob_len(blob_T *b)
{
    ਜੇ (b == ਨਿਰਾ)
	ਵਾਪਸ 0L;
    ਵਾਪਸ b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    ਪੂਰਨ
blob_get(blob_T *b, ਪੂਰਨ idx)
{
    ਵਾਪਸ ((char_u*)b->bv_ga.ga_ਅੰਕੜੇ)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    ਖਾਲੀ
blob_ਸੈੱਟ(blob_T *blob, ਪੂਰਨ idx, ਪੂਰਨ byte)
{
    ((char_u*)blob->bv_ga.ga_ਅੰਕੜੇ)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    ਖਾਲੀ
blob_ਸੈੱਟ_append(blob_T *blob, ਪੂਰਨ idx, ਪੂਰਨ byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    ਜੇ (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_ਸੈੱਟ(blob, idx, byte);
	ਜੇ (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    ਪੂਰਨ
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    ਪੂਰਨ	    i;
    ਲੰਮਾ    len1 = blob_len(b1);
    ਲੰਮਾ    len2 = blob_len(b2);

    // empty and NULL are considered the same
    ਜੇ (len1 == 0 && len2 == 0)
	ਵਾਪਸ TRUE;
    ਜੇ (b1 == b2)
	ਵਾਪਸ TRUE;
    ਜੇ (len1 != len2)
	ਵਾਪਸ FALSE;

    ਲਈ (i = 0; i < b1->bv_ga.ga_len; i++)
	ਜੇ (blob_get(b1, i) != blob_get(b2, i)) ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    ਪੂਰਨ
read_blob(FILE *fd, typval_T *rettv, off_T ਵਿੱਥ, off_T ਮਾਪ_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    ਬਣਤਰ stat	st;
    ਪੂਰਨ		whence;
    off_T	ਮਾਪ = ਮਾਪ_arg;

    ਜੇ (fstat(fileno(fd), &st) < 0)
	ਵਾਪਸ FAIL;  // can't read the file, error

    ਜੇ (ਵਿੱਥ >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	ਜੇ (ਮਾਪ == -1 || (ਮਾਪ > st.st_size - ਵਿੱਥ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ S_ISCHR
		    && !S_ISCHR(st.st_ਬਹੁਲਕ)
#ਜੇ_ਅੰਤ
		    ))
	    // size may become negative, checked below
	    ਮਾਪ = st.st_size - ਵਿੱਥ;
	whence = SEEK_SET;
    }
    ਨਹੀਂਤਾਂ
    {
	// limit the offset to not go before the start of the file
	ਜੇ (-ਵਿੱਥ > st.st_size
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ S_ISCHR
		    && !S_ISCHR(st.st_ਬਹੁਲਕ)
#ਜੇ_ਅੰਤ
		    )
	    ਵਿੱਥ = -st.st_size;
	// Size defaults to reading until the end of the file.
	ਜੇ (ਮਾਪ == -1 || ਮਾਪ > -ਵਿੱਥ)
	    ਮਾਪ = -ਵਿੱਥ;
	whence = SEEK_END;
    }
    ਜੇ (ਮਾਪ <= 0)
	ਵਾਪਸ OK;
    ਜੇ (ਵਿੱਥ != 0 && vim_fseek(fd, ਵਿੱਥ, whence) != 0)
	ਵਾਪਸ OK;

    ਜੇ (ga_grow(&blob->bv_ga, (ਪੂਰਨ)ਮਾਪ) == FAIL)
	ਵਾਪਸ FAIL;
    blob->bv_ga.ga_len = (ਪੂਰਨ)ਮਾਪ;
    ਜੇ (fread(blob->bv_ga.ga_ਅੰਕੜੇ, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = ਨਿਰਾ;
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    ਪੂਰਨ
write_blob(FILE *fd, blob_T *blob)
{
    ਜੇ (fwrite(blob->bv_ga.ga_ਅੰਕੜੇ, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf ਅਣਵਰਤਿਆ)
{
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ hex_chars[] = "0123456789ABCDEF";
    ਪੂਰਨ		blen;
    size_t	ਕੁੱਲ;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    ਜੇ (blob == ਨਿਰਾ || (blen = blob_len(blob)) == 0)
    {
	*tofree = ਨਿਰਾ;
	ਵਾਪਸ (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    ਕੁੱਲ = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(ਕੁੱਲ);
    ਜੇ (buf == ਨਿਰਾ)
    {
	*tofree = ਨਿਰਾ;
	ਵਾਪਸ (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_ਅੰਕੜੇ;
    ਲਈ (ਪੂਰਨ i = 0; i < blen; i++)
    {
	ਬਿਨਾਨਿਸ਼ਾਨੀ b;

	ਜੇ (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    ਵਾਪਸ buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    ਖਾਲੀ
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	ਜੇ (l2 == ਨਿਰਾ)
	    ਵਾਪਸ;

	ਜੇ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ਵਾਪਸ;
	}

	ਜੇ (list_append_ਸੰਖਿਆ(l2, i) == FAIL
		|| list_append_ਸੰਖਿਆ(l2, blob_get(blob, i)) == FAIL)
	    ਵਾਪਸ;
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

    ਜੇ (blob == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	ਜਾਓ failed;
    s += 2;
    ਜਦੋਂ (vim_isxdigit(*s))
    {
	ਜੇ (!vim_isxdigit(s[1]))
	    ਜਾਓ failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	ਜੇ (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    ਜੇ (*skipwhite(s) != NUL)
	ਜਾਓ failed;  // text after final digit

    ++blob->bv_refcount;
    ਵਾਪਸ blob;

failed:
    blob_free(blob);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    ਸਥਿਰ ਪੂਰਨ
blob_ਟੁਕੜਾ(
	blob_T		*blob,
	ਲੰਮਾ		len,
	varnumber_T	n1,
	varnumber_T	n2,
	ਪੂਰਨ		exclusive,
	typval_T	*rettv)
{
    ਜੇ (n1 < 0)
    {
	n1 = len + n1;
	ਜੇ (n1 < 0)
	    n1 = 0;
    }
    ਜੇ (n2 < 0)
	n2 = len + n2;
    ਨਹੀਂਤਾਂ ਜੇ (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    ਜੇ (exclusive)
	--n2;
    ਜੇ (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	blob_T  *new_blob = blob_alloc();
	ਲੰਮਾ    i;

	ਜੇ (new_blob != ਨਿਰਾ)
	{
	    ਜੇ (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		ਵਾਪਸ FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    ਲਈ (i = n1; i <= n2; i++)
		blob_ਸੈੱਟ(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_ਸੈੱਟ(rettv, new_blob);
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    ਸਥਿਰ ਪੂਰਨ
blob_ਸੂਚਕਾਂਕ(
	blob_T		*blob,
	ਪੂਰਨ		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    ਜੇ (idx < 0)
	idx = len + idx;
    ਜੇ (idx < len && idx >= 0)
    {
	ਪੂਰਨ v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_ਸੰਖਿਆ;
	rettv->vval.v_ਸੰਖਿਆ = v;
    }
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_blob_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), idx);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

    ਪੂਰਨ
blob_ਟੁਕੜਾ_or_ਸੂਚਕਾਂਕ(
	blob_T		*blob,
	ਪੂਰਨ		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	ਪੂਰਨ		exclusive,
	typval_T	*rettv)
{
    ਲੰਮਾ	len = blob_len(blob);

    ਜੇ (is_range)
	ਵਾਪਸ blob_ਟੁਕੜਾ(blob, len, n1, n2, exclusive, rettv);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ blob_ਸੂਚਕਾਂਕ(blob, len, n1, rettv);
    ਵਾਪਸ OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    ਪੂਰਨ
check_blob_ਸੂਚਕਾਂਕ(ਲੰਮਾ bloblen, varnumber_T n1, ਪੂਰਨ quiet)
{
    ਜੇ (n1 < 0 || n1 > bloblen)
    {
	ਜੇ (!quiet)
	    semsg(_(e_blob_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), n1);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    ਪੂਰਨ
check_blob_range(ਲੰਮਾ bloblen, varnumber_T n1, varnumber_T n2, ਪੂਰਨ quiet)
{
    ਜੇ (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	ਜੇ (!quiet)
	    semsg(_(e_blob_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), n2);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    ਪੂਰਨ
blob_ਸੈੱਟ_range(blob_T *dest, ਲੰਮਾ n1, ਲੰਮਾ n2, typval_T *src)
{
    ਪੂਰਨ	il, ir;

    ਜੇ (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_ਮੁੱਲ_does_not_have_ਸੱਜਾ_ਸੰਖਿਆ_ਦਾ_ਬਾਈਟਾਂ));
	ਵਾਪਸ FAIL;
    }

    ir = 0;
    ਲਈ (il = n1; il <= n2; il++)
	blob_ਸੈੱਟ(dest, il, blob_get(src->vval.v_blob, ir++));
    ਵਾਪਸ OK;
}

/*
 * "add(blob, item)" function
 */
    ਖਾਲੀ
blob_ਜੋੜੋ(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    ਪੂਰਨ		error = FALSE;
    varnumber_T n;

    ਜੇ (b == ਨਿਰਾ)
    {
	ਜੇ (ਵਿੱਚ_vim9script())
	    emsg(_(e_cannot_ਜੋੜੋ_ਮੰਜ਼ਿਲ_null_blob));
	ਵਾਪਸ;
    }

    ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(b->bv_ਤਾਲਾ, (char_u *)N_("add() argument"), TRUE))
	ਵਾਪਸ;

    n = tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);
    ਜੇ (error)
	ਵਾਪਸ;

    ga_append(&b->bv_ga, (ਪੂਰਨ)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    ਖਾਲੀ
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    ਪੂਰਨ		error = FALSE;
    ਲੰਮਾ	idx;
    ਲੰਮਾ	ਸਿਰਾ;
    ਪੂਰਨ		len;
    char_u	*p;

    ਜੇ (b != ਨਿਰਾ && ਮੁੱਲ_check_ਤਾਲਾ(b->bv_ਤਾਲਾ, arg_errmsg, TRUE))
	ਵਾਪਸ;

    idx = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);
    ਜੇ (error)
	ਵਾਪਸ;

    len = blob_len(b);

    ਜੇ (idx < 0)
	// count from the end
	idx = len + idx;
    ਜੇ (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), idx);
	ਵਾਪਸ;
    }
    ਜੇ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_ਅੰਕੜੇ;
	rettv->vval.v_ਸੰਖਿਆ = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	ਵਾਪਸ;
    }

    // Remove range of items, return blob with values.
    ਸਿਰਾ = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
    ਜੇ (error)
	ਵਾਪਸ;
    ਜੇ (ਸਿਰਾ < 0)
	// count from the end
	ਸਿਰਾ = len + ਸਿਰਾ;
    ਜੇ (ਸਿਰਾ >= len || idx > ਸਿਰਾ)
    {
	semsg(_(e_blob_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), ਸਿਰਾ);
	ਵਾਪਸ;
    }
    newblob = blob_alloc();
    ਜੇ (newblob == ਨਿਰਾ)
	ਵਾਪਸ;
    newblob->bv_ga.ga_len = ਸਿਰਾ - idx + 1;
    ਜੇ (ga_grow(&newblob->bv_ga, ਸਿਰਾ - idx + 1) == FAIL)
    {
	vim_free(newblob);
	ਵਾਪਸ;
    }
    p = (char_u *)b->bv_ga.ga_ਅੰਕੜੇ;
    mch_memmove((char_u *)newblob->bv_ga.ga_ਅੰਕੜੇ, p + idx,
	    (size_t)(ਸਿਰਾ - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    ਜੇ (len - ਸਿਰਾ - 1 > 0)
	mch_memmove(p + idx, p + ਸਿਰਾ + 1, (size_t)(len - ਸਿਰਾ - 1));
    b->bv_ga.ga_len -= ਸਿਰਾ - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    ਖਾਲੀ
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*ਵਿਅੰਜਕ,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    ਪੂਰਨ		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    ਪੂਰਨ		idx = 0;
    ਪੂਰਨ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ਜੇ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ਨਿਰਾ;
    }
    ਜੇ (b == ਨਿਰਾ || (filtermap == FILTERMAP_FILTER
			    && ਮੁੱਲ_check_ਤਾਲਾ(b->bv_ਤਾਲਾ, arg_errmsg, TRUE)))
	ਵਾਪਸ;

    b_ret = b;
    ਜੇ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_ਤਾਲਾ = 0;
	rettv->vval.v_blob = blob_copy(b);
	ਜੇ (rettv->vval.v_blob == ਨਿਰਾ)
	    ਵਾਪਸ;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    ਸੈੱਟ_vim_var_type(VV_ਕੁੰਜੀ, VAR_ਸੰਖਿਆ);

    ਪੂਰਨ prev_ਤਾਲਾ = b->bv_ਤਾਲਾ;
    ਜੇ (b->bv_ਤਾਲਾ == 0)
	b->bv_ਤਾਲਾ = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ਵਿਅੰਜਕ_get_funccal(ਵਿਅੰਜਕ, &newtv);

    ਲਈ (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_ਸੰਖਿਆ;
	val = blob_get(b, i);
	tv.vval.v_ਸੰਖਿਆ = val;
	ਸੈੱਟ_vim_var_nr(VV_ਕੁੰਜੀ, idx);
	ਜੇ (filter_map_one(&tv, ਵਿਅੰਜਕ, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    ਰੋਕੋ;
	ਜੇ (filtermap != FILTERMAP_FOREACH)
	{
	    ਜੇ (newtv.v_type != VAR_ਸੰਖਿਆ && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		ਰੋਕੋ;
	    }
	    ਜੇ (filtermap != FILTERMAP_FILTER)
	    {
		ਜੇ (newtv.vval.v_ਸੰਖਿਆ != val)
		    blob_ਸੈੱਟ(b_ret, i, newtv.vval.v_ਸੰਖਿਆ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (rem)
	    {
		char_u *p = (char_u *)blob_arg->bv_ga.ga_ਅੰਕੜੇ;

		mch_memmove(p + i, p + i + 1,
			    (size_t)b->bv_ga.ga_len - i - 1);
		--b->bv_ga.ga_len;
		--i;
	    }
	}
	++idx;
    }

    b->bv_ਤਾਲਾ = prev_ਤਾਲਾ;
    ਜੇ (fc != ਨਿਰਾ)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    ਖਾਲੀ
blob_ਪਾਓ_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    ਲੰਮਾ	before = 0;
    ਪੂਰਨ		error = FALSE;
    ਪੂਰਨ		val, len;
    char_u	*p;

    ਜੇ (b == ਨਿਰਾ)
    {
	ਜੇ (ਵਿੱਚ_vim9script())
	    emsg(_(e_cannot_ਜੋੜੋ_ਮੰਜ਼ਿਲ_null_blob));
	ਵਾਪਸ;
    }

    ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(b->bv_ਤਾਲਾ, (char_u *)N_("insert() argument"), TRUE))
	ਵਾਪਸ;

    len = blob_len(b);
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
	ਜੇ (error)
	    ਵਾਪਸ;		// type error; errmsg already given
	ਜੇ (before < 0 || before > len)
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[2]));
	    ਵਾਪਸ;
	}
    }
    val = tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);
    ਜੇ (error)
	ਵਾਪਸ;
    ਜੇ (val < 0 || val > 255)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[1]));
	ਵਾਪਸ;
    }

    ਜੇ (ga_grow(&b->bv_ga, 1) == FAIL)
	ਵਾਪਸ;
    p = (char_u *)b->bv_ga.ga_ਅੰਕੜੇ;
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
    ਸਥਿਰ ਪੂਰਨ
blob_extend(blob_T *b1, blob_T *b2, ਲੰਮਾ bef)
{
    ਪੂਰਨ		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    ਜੇ (b2 == ਨਿਰਾ || b2->bv_ga.ga_len == 0)
	ਵਾਪਸ OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    ਜੇ (ga_grow(&b1->bv_ga, len2) == FAIL)
	ਵਾਪਸ FAIL;

    p1 = (char_u *)b1->bv_ga.ga_ਅੰਕੜੇ;

    ਜੇ (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	ਜੇ (bef < len1)
	    ਸਮ੍ਰਿਤੀ_ਨਕਲ(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(p1 + bef, b2->bv_ga.ga_ਅੰਕੜੇ, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    ਵਾਪਸ OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ਖਾਲੀ
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	ਪੂਰਨ		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    ਲੰਮਾ	before;
    ਪੂਰਨ		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    ਜੇ (b1 == ਨਿਰਾ)
    {
	emsg(_(e_cannot_extend_null_blob));
	ਵਾਪਸ;
    }
    ਜੇ (is_new || !ਮੁੱਲ_check_ਤਾਲਾ(b1->bv_ਤਾਲਾ, arg_errmsg, TRUE))
    {
	ਜੇ (is_new)
	{
	    b1 = blob_copy(b1);
	    ਜੇ (b1 == ਨਿਰਾ)
		ਵਾਪਸ;
	}

	b2 = argvars[1].vval.v_blob;
	ਜੇ (b2 == ਨਿਰਾ)
	    ਜਾਓ theend;

	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
	    ਜੇ (error)
		ਜਾਓ cleanup;		// type error; errmsg already given
	    ਜੇ (before < 0)
		before = b1->bv_ga.ga_len + before;
	    ਜੇ (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), before);
		ਜਾਓ cleanup;
	    }
	}
	ਨਹੀਂਤਾਂ
	    before = b1->bv_ga.ga_len;
	ਜੇ (blob_extend(b1, b2, before) == FAIL)
	    ਜਾਓ cleanup;

theend:
	ਜੇ (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_ਤਾਲਾ = 0;
	    rettv->vval.v_blob = b1;
	}
	ਨਹੀਂਤਾਂ
	    copy_tv(&argvars[0], rettv);
	ਵਾਪਸ;

cleanup:
	ਜੇ (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    ਖਾਲੀ
blob_reduce(
	typval_T	*argvars,
	typval_T	*ਵਿਅੰਜਕ,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    ਪੂਰਨ		called_emsg_ਸ਼ੁਰੂ = called_emsg;
    ਪੂਰਨ		r;
    typval_T	initial;
    typval_T	argv[3];
    ਪੂਰਨ	i;

    ਜੇ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ਜੇ (b == ਨਿਰਾ || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_ਦਾ_an_ਖਾਲੀ_ਥਾਵਾਂ_str_with_no_initial_ਮੁੱਲ), "Blob");
	    ਵਾਪਸ;
	}
	initial.v_type = VAR_ਸੰਖਿਆ;
	initial.vval.v_ਸੰਖਿਆ = blob_get(b, 0);
	i = 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (check_for_ਸੰਖਿਆ_arg(argvars, 2) == FAIL)
	ਵਾਪਸ;
    ਨਹੀਂਤਾਂ
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    ਜੇ (b == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_ਸੰਖਿਆ;
	argv[1].vval.v_ਸੰਖਿਆ = blob_get(b, i);

	r = eval_ਵਿਅੰਜਕ_typval(ਵਿਅੰਜਕ, TRUE, argv, 2, ਨਿਰਾ, rettv);

	clear_tv(&argv[0]);
	ਜੇ (r == FAIL || called_emsg != called_emsg_ਸ਼ੁਰੂ)
	    ਵਾਪਸ;
    }
}

/*
 * "reverse({blob})" function
 */
    ਖਾਲੀ
blob_ਉਲਟ(blob_T *b, typval_T *rettv)
{
    ਲੰਮਾ    i, len = blob_len(b);

    ਲਈ (i = 0; i < len / 2; i++)
    {
	ਪੂਰਨ tmp = blob_get(b, (ਪੂਰਨ)i);

	blob_ਸੈੱਟ(b, (ਪੂਰਨ)i, blob_get(b, (ਪੂਰਨ)(len - i - 1)));
	blob_ਸੈੱਟ(b, (ਪੂਰਨ)(len - i - 1), tmp);
    }
    rettv_blob_ਸੈੱਟ(rettv, b);
}

/*
 * blob2list() function
 */
    ਖਾਲੀ
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    ਪੂਰਨ		i, len;
    typval_T	tv;

    ਜੇ (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(ਖਾਲੀ)rettv_list_alloc(rettv);
	ਵਾਪਸ;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_ਚੀਜ਼ਾਂ(len);
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;
    rettv_list_ਸੈੱਟ(rettv, l);

    tv.v_type = VAR_ਸੰਖਿਆ;
    tv.v_ਤਾਲਾ = 0;
    ਲਈ (i = 0; i < len; i++)
    {
	tv.vval.v_ਸੰਖਿਆ = blob_get(blob, i);
	list_ਸੈੱਟ_ਵਸਤੂ(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    ਖਾਲੀ
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    ਜੇ (rettv_blob_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    blob = rettv->vval.v_blob;

    ਜੇ (check_for_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    l = argvars->vval.v_list;
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;

    // The result has one byte per list item; reserve all space at once.
    ਜੇ (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	ਵਾਪਸ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	ਪੂਰਨ		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_ਸੰਖਿਆ_chk(&li->li_tv, &error);
	ਜੇ (error == TRUE || n < 0 || n > 255)
	{
	    ਜੇ (!error)
		semsg(_(e_invalid_ਮੁੱਲ_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    ਵਾਪਸ;
	}
	((char_u *)blob->bv_ga.ga_ਅੰਕੜੇ)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#ਜੇ_ਅੰਤ // defined(FEAT_EVAL)
