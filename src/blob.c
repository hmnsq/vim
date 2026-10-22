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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(ಶೂನ್ಯ)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    ಯದಿ (blob != ರಿಕ್ತ)
	ga_init2(&blob->bv_ga, 1, 100);
    ಪ್ರತ್ಯಾಗಮ blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    ಯದಿ (b == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    rettv_blob_set(rettv, b);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set a blob as the return value.
 */
    ಶೂನ್ಯ
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    ಯದಿ (b != ರಿಕ್ತ)
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
    ಪೂರ್ಣಾಙ್ಕ		len;
    blob_T	*copy;

    ಯದಿ (orig == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    copy = blob_alloc();
    ಯದಿ (copy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    ಯದಿ (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	ಯದಿ (copy->bv_ga.ga_data == ರಿಕ್ತ)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    ಪ್ರತ್ಯಾಗಮ copy;
}

    ಶೂನ್ಯ
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    ಶೂನ್ಯ
blob_unref(blob_T *b)
{
    ಯದಿ (b != ರಿಕ್ತ && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    ಯದಿ (b == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0L;
    ಪ್ರತ್ಯಾಗಮ b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    ಪೂರ್ಣಾಙ್ಕ
blob_get(blob_T *b, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    ಶೂನ್ಯ
blob_set(blob_T *blob, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    ಶೂನ್ಯ
blob_set_append(blob_T *blob, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    ಯದಿ (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	ಯದಿ (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    ಪೂರ್ಣಾಙ್ಕ
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    ಯದಿ (len1 == 0 && len2 == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (b1 == b2)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (len1 != len2)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (i = 0; i < b1->bv_ga.ga_len; i++)
	ಯದಿ (blob_get(b1, i) != blob_get(b2, i)) ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    ಸಂರಚನಾ stat	st;
    ಪೂರ್ಣಾಙ್ಕ		whence;
    off_T	size = size_arg;

    ಯದಿ (fstat(fileno(fd), &st) < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;  // can't read the file, error

    ಯದಿ (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	ಯದಿ (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    ಅನ್ಯಥಾ
    {
	// limit the offset to not go before the start of the file
	ಯದಿ (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	ಯದಿ (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    ಯದಿ (size <= 0)
	ಪ್ರತ್ಯಾಗಮ OK;
    ಯದಿ (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (ga_grow(&blob->bv_ga, (ಪೂರ್ಣಾಙ್ಕ)size) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    blob->bv_ga.ga_len = (ಪೂರ್ಣಾಙ್ಕ)size;
    ಯದಿ (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
write_blob(FILE *fd, blob_T *blob)
{
    ಯದಿ (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ hex_chars[] = "0123456789ABCDEF";
    ಪೂರ್ಣಾಙ್ಕ		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    ಯದಿ (blob == ರಿಕ್ತ || (blen = blob_len(blob)) == 0)
    {
	*tofree = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	*tofree = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < blen; i++)
    {
	unsigned b;

	ಯದಿ (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    ಶೂನ್ಯ
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	ಯದಿ (l2 == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (list_append_ಸಂಖ್ಯೆ(l2, i) == FAIL
		|| list_append_ಸಂಖ್ಯೆ(l2, blob_get(blob, i)) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
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

    ಯದಿ (blob == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	ಗಚ್ಛ failed;
    s += 2;
    ಯಾವತ್ (vim_isxdigit(*s))
    {
	ಯದಿ (!vim_isxdigit(s[1]))
	    ಗಚ್ಛ failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	ಯದಿ (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    ಯದಿ (*skipwhite(s) != NUL)
	ಗಚ್ಛ failed;  // text after final digit

    ++blob->bv_refcount;
    ಪ್ರತ್ಯಾಗಮ blob;

failed:
    blob_free(blob);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	ಪೂರ್ಣಾಙ್ಕ		exclusive,
	typval_T	*rettv)
{
    ಯದಿ (n1 < 0)
    {
	n1 = len + n1;
	ಯದಿ (n1 < 0)
	    n1 = 0;
    }
    ಯದಿ (n2 < 0)
	n2 = len + n2;
    ಅನ್ಯಥಾ ಯದಿ (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    ಯದಿ (exclusive)
	--n2;
    ಯದಿ (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	ಯದಿ (new_blob != ರಿಕ್ತ)
	{
	    ಯದಿ (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    ಪ್ರತಿ (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
blob_index(
	blob_T		*blob,
	ಪೂರ್ಣಾಙ್ಕ		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    ಯದಿ (idx < 0)
	idx = len + idx;
    ಯದಿ (idx < len && idx >= 0)
    {
	ಪೂರ್ಣಾಙ್ಕ v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = v;
    }
    ಅನ್ಯಥಾ
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
blob_slice_or_index(
	blob_T		*blob,
	ಪೂರ್ಣಾಙ್ಕ		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	ಪೂರ್ಣಾಙ್ಕ		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    ಯದಿ (is_range)
	ಪ್ರತ್ಯಾಗಮ blob_slice(blob, len, n1, n2, exclusive, rettv);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ blob_index(blob, len, n1, rettv);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    ಪೂರ್ಣಾಙ್ಕ
check_blob_index(long bloblen, varnumber_T n1, ಪೂರ್ಣಾಙ್ಕ quiet)
{
    ಯದಿ (n1 < 0 || n1 > bloblen)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    ಪೂರ್ಣಾಙ್ಕ
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, ಪೂರ್ಣಾಙ್ಕ quiet)
{
    ಯದಿ (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    ಪೂರ್ಣಾಙ್ಕ
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    ಪೂರ್ಣಾಙ್ಕ	il, ir;

    ಯದಿ (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_ಮೌಲ್ಯ_does_not_have_ಬಲ_ಸಂಖ್ಯೆ_of_bytes));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ir = 0;
    ಪ್ರತಿ (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * "add(blob, item)" function
 */
    ಶೂನ್ಯ
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    varnumber_T n;

    ಯದಿ (b == ರಿಕ್ತ)
    {
	ಯದಿ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (ಮೌಲ್ಯ_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	ಪ್ರತ್ಯಾಗಮ;

    n = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;

    ga_append(&b->bv_ga, (ಪೂರ್ಣಾಙ್ಕ)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    ಶೂನ್ಯ
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    long	idx;
    long	end;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*p;

    ಯದಿ (b != ರಿಕ್ತ && ಮೌಲ್ಯ_check_lock(b->bv_lock, arg_errmsg, TRUE))
	ಪ್ರತ್ಯಾಗಮ;

    idx = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;

    len = blob_len(b);

    ಯದಿ (idx < 0)
	// count from the end
	idx = len + idx;
    ಯದಿ (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (end < 0)
	// count from the end
	end = len + end;
    ಯದಿ (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	ಪ್ರತ್ಯಾಗಮ;
    }
    newblob = blob_alloc();
    ಯದಿ (newblob == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    newblob->bv_ga.ga_len = end - idx + 1;
    ಯದಿ (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	ಪ್ರತ್ಯಾಗಮ;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    ಯದಿ (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (size_t)(len - end - 1));
    b->bv_ga.ga_len -= end - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    ಶೂನ್ಯ
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    ಪೂರ್ಣಾಙ್ಕ		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ಯದಿ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ರಿಕ್ತ;
    }
    ಯದಿ (b == ರಿಕ್ತ || (filtermap == FILTERMAP_FILTER
			    && ಮೌಲ್ಯ_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	ಪ್ರತ್ಯಾಗಮ;

    b_ret = b;
    ಯದಿ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	ಯದಿ (rettv->vval.v_blob == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    ಪೂರ್ಣಾಙ್ಕ prev_lock = b->bv_lock;
    ಯದಿ (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, &newtv);

    ಪ್ರತಿ (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_ಸಂಖ್ಯೆ;
	val = blob_get(b, i);
	tv.vval.v_ಸಂಖ್ಯೆ = val;
	set_vim_var_nr(VV_ಕೀಲಿ, idx);
	ಯದಿ (filter_map_one(&tv, ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    ಭಙ್ಗ;
	ಯದಿ (filtermap != FILTERMAP_FOREACH)
	{
	    ಯದಿ (newtv.v_type != VAR_ಸಂಖ್ಯೆ && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		ಭಙ್ಗ;
	    }
	    ಯದಿ (filtermap != FILTERMAP_FILTER)
	    {
		ಯದಿ (newtv.vval.v_ಸಂಖ್ಯೆ != val)
		    blob_set(b_ret, i, newtv.vval.v_ಸಂಖ್ಯೆ);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (rem)
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
    ಯದಿ (fc != ರಿಕ್ತ)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    ಶೂನ್ಯ
blob_ಸೇರಿಸು_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		val, len;
    char_u	*p;

    ಯದಿ (b == ರಿಕ್ತ)
    {
	ಯದಿ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (ಮೌಲ್ಯ_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	ಪ್ರತ್ಯಾಗಮ;

    len = blob_len(b);
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given
	ಯದಿ (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    val = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (ga_grow(&b->bv_ga, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    ಪೂರ್ಣಾಙ್ಕ		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    ಯದಿ (b2 == ರಿಕ್ತ || b2->bv_ga.ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    ಯದಿ (ga_grow(&b1->bv_ga, len2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    ಯದಿ (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	ಯದಿ (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ಶೂನ್ಯ
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	ಪೂರ್ಣಾಙ್ಕ		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    ಯದಿ (b1 == ರಿಕ್ತ)
    {
	emsg(_(e_cannot_extend_null_blob));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (is_new || !ಮೌಲ್ಯ_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	ಯದಿ (is_new)
	{
	    b1 = blob_copy(b1);
	    ಯದಿ (b1 == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	}

	b2 = argvars[1].vval.v_blob;
	ಯದಿ (b2 == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	    ಯದಿ (error)
		ಗಚ್ಛ cleanup;		// type error; errmsg already given
	    ಯದಿ (before < 0)
		before = b1->bv_ga.ga_len + before;
	    ಯದಿ (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		ಗಚ್ಛ cleanup;
	    }
	}
	ಅನ್ಯಥಾ
	    before = b1->bv_ga.ga_len;
	ಯದಿ (blob_extend(b1, b2, before) == FAIL)
	    ಗಚ್ಛ cleanup;

theend:
	ಯದಿ (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	ಅನ್ಯಥಾ
	    copy_tv(&argvars[0], rettv);
	ಪ್ರತ್ಯಾಗಮ;

cleanup:
	ಯದಿ (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    ಶೂನ್ಯ
blob_reduce(
	typval_T	*argvars,
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_start = called_emsg;
    ಪೂರ್ಣಾಙ್ಕ		r;
    typval_T	initial;
    typval_T	argv[3];
    ಪೂರ್ಣಾಙ್ಕ	i;

    ಯದಿ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ಯದಿ (b == ರಿಕ್ತ || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_ಮೌಲ್ಯ), "Blob");
	    ಪ್ರತ್ಯಾಗಮ;
	}
	initial.v_type = VAR_ಸಂಖ್ಯೆ;
	initial.vval.v_ಸಂಖ್ಯೆ = blob_get(b, 0);
	i = 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಅನ್ಯಥಾ
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    ಯದಿ (b == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
	argv[1].vval.v_ಸಂಖ್ಯೆ = blob_get(b, i);

	r = eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, TRUE, argv, 2, ರಿಕ್ತ, rettv);

	clear_tv(&argv[0]);
	ಯದಿ (r == FAIL || called_emsg != called_emsg_start)
	    ಪ್ರತ್ಯಾಗಮ;
    }
}

/*
 * "reverse({blob})" function
 */
    ಶೂನ್ಯ
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    ಪ್ರತಿ (i = 0; i < len / 2; i++)
    {
	ಪೂರ್ಣಾಙ್ಕ tmp = blob_get(b, (ಪೂರ್ಣಾಙ್ಕ)i);

	blob_set(b, (ಪೂರ್ಣಾಙ್ಕ)i, blob_get(b, (ಪೂರ್ಣಾಙ್ಕ)(len - i - 1)));
	blob_set(b, (ಪೂರ್ಣಾಙ್ಕ)(len - i - 1), tmp);
    }
    rettv_blob_set(rettv, b);
}

/*
 * blob2list() function
 */
    ಶೂನ್ಯ
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    ಪೂರ್ಣಾಙ್ಕ		i, len;
    typval_T	tv;

    ಯದಿ (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(ಶೂನ್ಯ)rettv_list_alloc(rettv);
	ಪ್ರತ್ಯಾಗಮ;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_ವಸ್ತುಗಳು(len);
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_ಸಂಖ್ಯೆ;
    tv.v_lock = 0;
    ಪ್ರತಿ (i = 0; i < len; i++)
    {
	tv.vval.v_ಸಂಖ್ಯೆ = blob_get(blob, i);
	list_set_item(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    ಶೂನ್ಯ
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    ಯದಿ (rettv_blob_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    blob = rettv->vval.v_blob;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    l = argvars->vval.v_list;
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // The result has one byte per list item; reserve all space at once.
    ಯದಿ (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಪೂರ್ಣಾಙ್ಕ		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, &error);
	ಯದಿ (error == TRUE || n < 0 || n > 255)
	{
	    ಯದಿ (!error)
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
