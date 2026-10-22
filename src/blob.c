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

#ಆದರೆ defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(ಶೂನ್ಯ)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    ಆದರೆ (blob != ಖಾಲಿ)
	ga_init2(&blob->bv_ga, 1, 100);
    ಹಿಂತಿರುಗಿ blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    ಆದರೆ (b == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    rettv_blob_ಗಣ(rettv, b);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set a blob as the return value.
 */
    ಶೂನ್ಯ
rettv_blob_ಗಣ(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    ಆದರೆ (b != ಖಾಲಿ)
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
    ಪೂರ್ಣಾಂಕ		len;
    blob_T	*copy;

    ಆದರೆ (orig == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    copy = blob_alloc();
    ಆದರೆ (copy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    ಆದರೆ (len > 0)
    {
	copy->bv_ga.ga_ದತ್ತಾಂಶ =
	    vim_memsave(orig->bv_ga.ga_ದತ್ತಾಂಶ, len);
	ಆದರೆ (copy->bv_ga.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    ಹಿಂತಿರುಗಿ copy;
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
    ಆದರೆ (b != ಖಾಲಿ && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    ದೀರ್ಘ
blob_len(blob_T *b)
{
    ಆದರೆ (b == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0L;
    ಹಿಂತಿರುಗಿ b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    ಪೂರ್ಣಾಂಕ
blob_get(blob_T *b, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ ((char_u*)b->bv_ga.ga_ದತ್ತಾಂಶ)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    ಶೂನ್ಯ
blob_ಗಣ(blob_T *blob, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ byte)
{
    ((char_u*)blob->bv_ga.ga_ದತ್ತಾಂಶ)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    ಶೂನ್ಯ
blob_ಗಣ_append(blob_T *blob, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    ಆದರೆ (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_ಗಣ(blob, idx, byte);
	ಆದರೆ (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    ಪೂರ್ಣಾಂಕ
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ದೀರ್ಘ    len1 = blob_len(b1);
    ದೀರ್ಘ    len2 = blob_len(b2);

    // empty and NULL are considered the same
    ಆದರೆ (len1 == 0 && len2 == 0)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (b1 == b2)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (len1 != len2)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (i = 0; i < b1->bv_ga.ga_len; i++)
	ಆದರೆ (blob_get(b1, i) != blob_get(b2, i)) ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
read_blob(FILE *fd, typval_T *rettv, off_T ಸ್ಥಾನಾಂತರ, off_T ಗಾತ್ರ_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    ರಚನೆ stat	st;
    ಪೂರ್ಣಾಂಕ		whence;
    off_T	ಗಾತ್ರ_2 = ಗಾತ್ರ_arg;

    ಆದರೆ (fstat(fileno(fd), &st) < 0)
	ಹಿಂತಿರುಗಿ FAIL;  // can't read the file, error

    ಆದರೆ (ಸ್ಥಾನಾಂತರ >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	ಆದರೆ (ಗಾತ್ರ_2 == -1 || (ಗಾತ್ರ_2 > st.st_size - ಸ್ಥಾನಾಂತರ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ S_ISCHR
		    && !S_ISCHR(st.st_ಬಹುಲಕ)
#ಆದರೆ_ಅಂತ್ಯ
		    ))
	    // size may become negative, checked below
	    ಗಾತ್ರ_2 = st.st_size - ಸ್ಥಾನಾಂತರ;
	whence = SEEK_SET;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// limit the offset to not go before the start of the file
	ಆದರೆ (-ಸ್ಥಾನಾಂತರ > st.st_size
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ S_ISCHR
		    && !S_ISCHR(st.st_ಬಹುಲಕ)
#ಆದರೆ_ಅಂತ್ಯ
		    )
	    ಸ್ಥಾನಾಂತರ = -st.st_size;
	// Size defaults to reading until the end of the file.
	ಆದರೆ (ಗಾತ್ರ_2 == -1 || ಗಾತ್ರ_2 > -ಸ್ಥಾನಾಂತರ)
	    ಗಾತ್ರ_2 = -ಸ್ಥಾನಾಂತರ;
	whence = SEEK_END;
    }
    ಆದರೆ (ಗಾತ್ರ_2 <= 0)
	ಹಿಂತಿರುಗಿ OK;
    ಆದರೆ (ಸ್ಥಾನಾಂತರ != 0 && vim_fseek(fd, ಸ್ಥಾನಾಂತರ, whence) != 0)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (ga_grow(&blob->bv_ga, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ_2) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    blob->bv_ga.ga_len = (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ_2;
    ಆದರೆ (fread(blob->bv_ga.ga_ದತ್ತಾಂಶ, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
write_blob(FILE *fd, blob_T *blob)
{
    ಆದರೆ (fwrite(blob->bv_ga.ga_ದತ್ತಾಂಶ, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf ಬಳಸದ)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hex_chars[] = "0123456789ABCDEF";
    ಪೂರ್ಣಾಂಕ		blen;
    size_t	ಒಟ್ಟು;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    ಆದರೆ (blob == ಖಾಲಿ || (blen = blob_len(blob)) == 0)
    {
	*tofree = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    ಒಟ್ಟು = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(ಒಟ್ಟು);
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	*tofree = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < blen; i++)
    {
	ಚಿಹ್ನೆರಹಿತ b;

	ಆದರೆ (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    ಹಿಂತಿರುಗಿ buf;
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

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	ಆದರೆ (l2 == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (list_append_ಸಂಖ್ಯೆ(l2, i) == FAIL
		|| list_append_ಸಂಖ್ಯೆ(l2, blob_get(blob, i)) == FAIL)
	    ಹಿಂತಿರುಗಿ;
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

    ಆದರೆ (blob == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	ಹೋಗು failed;
    s += 2;
    ಆಗ (vim_isxdigit(*s))
    {
	ಆದರೆ (!vim_isxdigit(s[1]))
	    ಹೋಗು failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	ಆದರೆ (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    ಆದರೆ (*skipwhite(s) != NUL)
	ಹೋಗು failed;  // text after final digit

    ++blob->bv_refcount;
    ಹಿಂತಿರುಗಿ blob;

failed:
    blob_free(blob);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
blob_ತುಂಡು(
	blob_T		*blob,
	ದೀರ್ಘ		len,
	varnumber_T	n1,
	varnumber_T	n2,
	ಪೂರ್ಣಾಂಕ		exclusive,
	typval_T	*rettv)
{
    ಆದರೆ (n1 < 0)
    {
	n1 = len + n1;
	ಆದರೆ (n1 < 0)
	    n1 = 0;
    }
    ಆದರೆ (n2 < 0)
	n2 = len + n2;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    ಆದರೆ (exclusive)
	--n2;
    ಆದರೆ (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	blob_T  *new_blob = blob_alloc();
	ದೀರ್ಘ    i;

	ಆದರೆ (new_blob != ಖಾಲಿ)
	{
	    ಆದರೆ (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    ಗಾಗಿ (i = n1; i <= n2; i++)
		blob_ಗಣ(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_ಗಣ(rettv, new_blob);
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
blob_ಸೂಚ್ಯಂಕ(
	blob_T		*blob,
	ಪೂರ್ಣಾಂಕ		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    ಆದರೆ (idx < 0)
	idx = len + idx;
    ಆದರೆ (idx < len && idx >= 0)
    {
	ಪೂರ್ಣಾಂಕ v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = v;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_blob_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), idx);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
blob_ತುಂಡು_or_ಸೂಚ್ಯಂಕ(
	blob_T		*blob,
	ಪೂರ್ಣಾಂಕ		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	ಪೂರ್ಣಾಂಕ		exclusive,
	typval_T	*rettv)
{
    ದೀರ್ಘ	len = blob_len(blob);

    ಆದರೆ (is_range)
	ಹಿಂತಿರುಗಿ blob_ತುಂಡು(blob, len, n1, n2, exclusive, rettv);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ blob_ಸೂಚ್ಯಂಕ(blob, len, n1, rettv);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    ಪೂರ್ಣಾಂಕ
check_blob_ಸೂಚ್ಯಂಕ(ದೀರ್ಘ bloblen, varnumber_T n1, ಪೂರ್ಣಾಂಕ quiet)
{
    ಆದರೆ (n1 < 0 || n1 > bloblen)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_blob_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), n1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    ಪೂರ್ಣಾಂಕ
check_blob_range(ದೀರ್ಘ bloblen, varnumber_T n1, varnumber_T n2, ಪೂರ್ಣಾಂಕ quiet)
{
    ಆದರೆ (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_blob_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), n2);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    ಪೂರ್ಣಾಂಕ
blob_ಗಣ_range(blob_T *dest, ದೀರ್ಘ n1, ದೀರ್ಘ n2, typval_T *src)
{
    ಪೂರ್ಣಾಂಕ	il, ir;

    ಆದರೆ (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_ಮೌಲ್ಯ_does_not_have_ಬಲ_ಸಂಖ್ಯೆ_ದ_ಬೈಟುಗಳು));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ir = 0;
    ಗಾಗಿ (il = n1; il <= n2; il++)
	blob_ಗಣ(dest, il, blob_get(src->vval.v_blob, ir++));
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * "add(blob, item)" function
 */
    ಶೂನ್ಯ
blob_ಕೂಡಿಸು(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    varnumber_T n;

    ಆದರೆ (b == ಖಾಲಿ)
    {
	ಆದರೆ (ಒಳ_vim9script())
	    emsg(_(e_cannot_ಕೂಡಿಸು_ಗಮ್ಯ_null_blob));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(b->bv_ಬೀಗ, (char_u *)N_("add() argument"), TRUE))
	ಹಿಂತಿರುಗಿ;

    n = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;

    ga_append(&b->bv_ga, (ಪೂರ್ಣಾಂಕ)n);
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
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    ದೀರ್ಘ	idx;
    ದೀರ್ಘ	ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*p;

    ಆದರೆ (b != ಖಾಲಿ && ಮೌಲ್ಯ_check_ಬೀಗ(b->bv_ಬೀಗ, arg_errmsg, TRUE))
	ಹಿಂತಿರುಗಿ;

    idx = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;

    len = blob_len(b);

    ಆದರೆ (idx < 0)
	// count from the end
	idx = len + idx;
    ಆದರೆ (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), idx);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_ದತ್ತಾಂಶ;
	rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	ಹಿಂತಿರುಗಿ;
    }

    // Remove range of items, return blob with values.
    ಕೊನೆ = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ಕೊನೆ < 0)
	// count from the end
	ಕೊನೆ = len + ಕೊನೆ;
    ಆದರೆ (ಕೊನೆ >= len || idx > ಕೊನೆ)
    {
	semsg(_(e_blob_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), ಕೊನೆ);
	ಹಿಂತಿರುಗಿ;
    }
    newblob = blob_alloc();
    ಆದರೆ (newblob == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    newblob->bv_ga.ga_len = ಕೊನೆ - idx + 1;
    ಆದರೆ (ga_grow(&newblob->bv_ga, ಕೊನೆ - idx + 1) == FAIL)
    {
	vim_free(newblob);
	ಹಿಂತಿರುಗಿ;
    }
    p = (char_u *)b->bv_ga.ga_ದತ್ತಾಂಶ;
    mch_memmove((char_u *)newblob->bv_ga.ga_ದತ್ತಾಂಶ, p + idx,
	    (size_t)(ಕೊನೆ - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    ಆದರೆ (len - ಕೊನೆ - 1 > 0)
	mch_memmove(p + idx, p + ಕೊನೆ + 1, (size_t)(len - ಕೊನೆ - 1));
    b->bv_ga.ga_len -= ಕೊನೆ - idx + 1;
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
    ಪೂರ್ಣಾಂಕ		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    ಪೂರ್ಣಾಂಕ		idx = 0;
    ಪೂರ್ಣಾಂಕ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ಆದರೆ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ಖಾಲಿ;
    }
    ಆದರೆ (b == ಖಾಲಿ || (filtermap == FILTERMAP_FILTER
			    && ಮೌಲ್ಯ_check_ಬೀಗ(b->bv_ಬೀಗ, arg_errmsg, TRUE)))
	ಹಿಂತಿರುಗಿ;

    b_ret = b;
    ಆದರೆ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_ಬೀಗ = 0;
	rettv->vval.v_blob = blob_copy(b);
	ಆದರೆ (rettv->vval.v_blob == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    ಗಣ_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    ಪೂರ್ಣಾಂಕ prev_ಬೀಗ = b->bv_ಬೀಗ;
    ಆದರೆ (b->bv_ಬೀಗ == 0)
	b->bv_ಬೀಗ = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, &newtv);

    ಗಾಗಿ (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_ಸಂಖ್ಯೆ;
	val = blob_get(b, i);
	tv.vval.v_ಸಂಖ್ಯೆ = val;
	ಗಣ_vim_var_nr(VV_ಕೀಲಿ, idx);
	ಆದರೆ (filter_map_one(&tv, ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (filtermap != FILTERMAP_FOREACH)
	{
	    ಆದರೆ (newtv.v_type != VAR_ಸಂಖ್ಯೆ && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (filtermap != FILTERMAP_FILTER)
	    {
		ಆದರೆ (newtv.vval.v_ಸಂಖ್ಯೆ != val)
		    blob_ಗಣ(b_ret, i, newtv.vval.v_ಸಂಖ್ಯೆ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rem)
	    {
		char_u *p = (char_u *)blob_arg->bv_ga.ga_ದತ್ತಾಂಶ;

		mch_memmove(p + i, p + i + 1,
			    (size_t)b->bv_ga.ga_len - i - 1);
		--b->bv_ga.ga_len;
		--i;
	    }
	}
	++idx;
    }

    b->bv_ಬೀಗ = prev_ಬೀಗ;
    ಆದರೆ (fc != ಖಾಲಿ)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    ಶೂನ್ಯ
blob_ಸೇರಿಸು_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    ದೀರ್ಘ	before = 0;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    ಪೂರ್ಣಾಂಕ		val, len;
    char_u	*p;

    ಆದರೆ (b == ಖಾಲಿ)
    {
	ಆದರೆ (ಒಳ_vim9script())
	    emsg(_(e_cannot_ಕೂಡಿಸು_ಗಮ್ಯ_null_blob));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(b->bv_ಬೀಗ, (char_u *)N_("insert() argument"), TRUE))
	ಹಿಂತಿರುಗಿ;

    len = blob_len(b);
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ;		// type error; errmsg already given
	ಆದರೆ (before < 0 || before > len)
	{
	    semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[2]));
	    ಹಿಂತಿರುಗಿ;
	}
    }
    val = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (val < 0 || val > 255)
    {
	semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[1]));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ga_grow(&b->bv_ga, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;
    p = (char_u *)b->bv_ga.ga_ದತ್ತಾಂಶ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
blob_extend(blob_T *b1, blob_T *b2, ದೀರ್ಘ bef)
{
    ಪೂರ್ಣಾಂಕ		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    ಆದರೆ (b2 == ಖಾಲಿ || b2->bv_ga.ga_len == 0)
	ಹಿಂತಿರುಗಿ OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    ಆದರೆ (ga_grow(&b1->bv_ga, len2) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    p1 = (char_u *)b1->bv_ga.ga_ದತ್ತಾಂಶ;

    ಆದರೆ (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	ಆದರೆ (bef < len1)
	    ಸ್ಮರಣೆ_ನಕಲು(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	ಸ್ಮರಣೆ_ನಕಲು(p1 + bef, b2->bv_ga.ga_ದತ್ತಾಂಶ, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    ಹಿಂತಿರುಗಿ OK;
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
	ಪೂರ್ಣಾಂಕ		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    ದೀರ್ಘ	before;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    ಆದರೆ (b1 == ಖಾಲಿ)
    {
	emsg(_(e_cannot_extend_null_blob));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (is_new || !ಮೌಲ್ಯ_check_ಬೀಗ(b1->bv_ಬೀಗ, arg_errmsg, TRUE))
    {
	ಆದರೆ (is_new)
	{
	    b1 = blob_copy(b1);
	    ಆದರೆ (b1 == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	}

	b2 = argvars[1].vval.v_blob;
	ಆದರೆ (b2 == ಖಾಲಿ)
	    ಹೋಗು theend;

	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	    ಆದರೆ (error)
		ಹೋಗು cleanup;		// type error; errmsg already given
	    ಆದರೆ (before < 0)
		before = b1->bv_ga.ga_len + before;
	    ಆದರೆ (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), before);
		ಹೋಗು cleanup;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    before = b1->bv_ga.ga_len;
	ಆದರೆ (blob_extend(b1, b2, before) == FAIL)
	    ಹೋಗು cleanup;

theend:
	ಆದರೆ (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_ಬೀಗ = 0;
	    rettv->vval.v_blob = b1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    copy_tv(&argvars[0], rettv);
	ಹಿಂತಿರುಗಿ;

cleanup:
	ಆದರೆ (is_new)
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
    ಪೂರ್ಣಾಂಕ		called_emsg_ಆರಂಭ = called_emsg;
    ಪೂರ್ಣಾಂಕ		r;
    typval_T	initial;
    typval_T	argv[3];
    ಪೂರ್ಣಾಂಕ	i;

    ಆದರೆ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ಆದರೆ (b == ಖಾಲಿ || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_ದ_an_ಖಾಲಿಗಳು_str_with_no_initial_ಮೌಲ್ಯ), "Blob");
	    ಹಿಂತಿರುಗಿ;
	}
	initial.v_type = VAR_ಸಂಖ್ಯೆ;
	initial.vval.v_ಸಂಖ್ಯೆ = blob_get(b, 0);
	i = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    ಆದರೆ (b == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
	argv[1].vval.v_ಸಂಖ್ಯೆ = blob_get(b, i);

	r = eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, TRUE, argv, 2, ಖಾಲಿ, rettv);

	clear_tv(&argv[0]);
	ಆದರೆ (r == FAIL || called_emsg != called_emsg_ಆರಂಭ)
	    ಹಿಂತಿರುಗಿ;
    }
}

/*
 * "reverse({blob})" function
 */
    ಶೂನ್ಯ
blob_ಹಿಮ್ಮುಖ(blob_T *b, typval_T *rettv)
{
    ದೀರ್ಘ    i, len = blob_len(b);

    ಗಾಗಿ (i = 0; i < len / 2; i++)
    {
	ಪೂರ್ಣಾಂಕ tmp = blob_get(b, (ಪೂರ್ಣಾಂಕ)i);

	blob_ಗಣ(b, (ಪೂರ್ಣಾಂಕ)i, blob_get(b, (ಪೂರ್ಣಾಂಕ)(len - i - 1)));
	blob_ಗಣ(b, (ಪೂರ್ಣಾಂಕ)(len - i - 1), tmp);
    }
    rettv_blob_ಗಣ(rettv, b);
}

/*
 * blob2list() function
 */
    ಶೂನ್ಯ
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    ಪೂರ್ಣಾಂಕ		i, len;
    typval_T	tv;

    ಆದರೆ (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(ಶೂನ್ಯ)rettv_list_alloc(rettv);
	ಹಿಂತಿರುಗಿ;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_ವಸ್ತುಗಳು(len);
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    rettv_list_ಗಣ(rettv, l);

    tv.v_type = VAR_ಸಂಖ್ಯೆ;
    tv.v_ಬೀಗ = 0;
    ಗಾಗಿ (i = 0; i < len; i++)
    {
	tv.vval.v_ಸಂಖ್ಯೆ = blob_get(blob, i);
	list_ಗಣ_ಅಂಶ(l, i, &tv);
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

    ಆದರೆ (rettv_blob_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    blob = rettv->vval.v_blob;

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    l = argvars->vval.v_list;
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // The result has one byte per list item; reserve all space at once.
    ಆದರೆ (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	ಹಿಂತಿರುಗಿ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಪೂರ್ಣಾಂಕ		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, &error);
	ಆದರೆ (error == TRUE || n < 0 || n > 255)
	{
	    ಆದರೆ (!error)
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    ಹಿಂತಿರುಗಿ;
	}
	((char_u *)blob->bv_ga.ga_ದತ್ತಾಂಶ)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_EVAL)
