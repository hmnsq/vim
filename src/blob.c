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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(ଶୂନ୍ୟ)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    ଯଦି (blob != ଖାଲି)
	ga_init2(&blob->bv_ga, 1, 100);
    ଫେରନ୍ତୁ blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    ଯଦି (b == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    rettv_blob_set(rettv, b);
    ଫେରନ୍ତୁ OK;
}

/*
 * Set a blob as the return value.
 */
    ଶୂନ୍ୟ
rettv_blob_set(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    ଯଦି (b != ଖାଲି)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    blob_T	*copy;

    ଯଦି (orig == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    copy = blob_alloc();
    ଯଦି (copy == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    ଯଦି (len > 0)
    {
	copy->bv_ga.ga_data =
	    vim_memsave(orig->bv_ga.ga_data, len);
	ଯଦି (copy->bv_ga.ga_data == ଖାଲି)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    ଫେରନ୍ତୁ copy;
}

    ଶୂନ୍ୟ
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    ଶୂନ୍ୟ
blob_unref(blob_T *b)
{
    ଯଦି (b != ଖାଲି && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    long
blob_len(blob_T *b)
{
    ଯଦି (b == ଖାଲି)
	ଫେରନ୍ତୁ 0L;
    ଫେରନ୍ତୁ b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
blob_get(blob_T *b, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ ((char_u*)b->bv_ga.ga_data)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    ଶୂନ୍ୟ
blob_set(blob_T *blob, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା byte)
{
    ((char_u*)blob->bv_ga.ga_data)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    ଶୂନ୍ୟ
blob_set_append(blob_T *blob, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    ଯଦି (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_set(blob, idx, byte);
	ଯଦି (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    long    len1 = blob_len(b1);
    long    len2 = blob_len(b2);

    // empty and NULL are considered the same
    ଯଦି (len1 == 0 && len2 == 0)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (b1 == b2)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (len1 != len2)
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (i = 0; i < b1->bv_ga.ga_len; i++)
	ଯଦି (blob_get(b1, i) != blob_get(b2, i)) ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_blob(FILE *fd, typval_T *rettv, off_T offset, off_T size_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    ସଂରଚନା stat	st;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		whence;
    off_T	size = size_arg;

    ଯଦି (fstat(fileno(fd), &st) < 0)
	ଫେରନ୍ତୁ FAIL;  // can't read the file, error

    ଯଦି (offset >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	ଯଦି (size == -1 || (size > st.st_size - offset
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    ))
	    // size may become negative, checked below
	    size = st.st_size - offset;
	whence = SEEK_SET;
    }
    ଅନ୍ୟଥା
    {
	// limit the offset to not go before the start of the file
	ଯଦି (-offset > st.st_size
#ifdef S_ISCHR
		    && !S_ISCHR(st.st_mode)
#endif
		    )
	    offset = -st.st_size;
	// Size defaults to reading until the end of the file.
	ଯଦି (size == -1 || size > -offset)
	    size = -offset;
	whence = SEEK_END;
    }
    ଯଦି (size <= 0)
	ଫେରନ୍ତୁ OK;
    ଯଦି (offset != 0 && vim_fseek(fd, offset, whence) != 0)
	ଫେରନ୍ତୁ OK;

    ଯଦି (ga_grow(&blob->bv_ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)size) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    blob->bv_ga.ga_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)size;
    ଯଦି (fread(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = ଖାଲି;
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_blob(FILE *fd, blob_T *blob)
{
    ଯଦି (fwrite(blob->bv_ga.ga_data, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf UNUSED)
{
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର hex_chars[] = "0123456789ABCDEF";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		blen;
    size_t	total;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    ଯଦି (blob == ଖାଲି || (blen = blob_len(blob)) == 0)
    {
	*tofree = ଖାଲି;
	ଫେରନ୍ତୁ (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    total = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(total);
    ଯଦି (buf == ଖାଲି)
    {
	*tofree = ଖାଲି;
	ଫେରନ୍ତୁ (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_data;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < blen; i++)
    {
	unsigned b;

	ଯଦି (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    ଫେରନ୍ତୁ buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    ଶୂନ୍ୟ
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	ଯଦି (l2 == ଖାଲି)
	    ଫେରନ୍ତୁ;

	ଯଦି (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ଫେରନ୍ତୁ;
	}

	ଯଦି (list_append_ସଂଖ୍ୟା(l2, i) == FAIL
		|| list_append_ସଂଖ୍ୟା(l2, blob_get(blob, i)) == FAIL)
	    ଫେରନ୍ତୁ;
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

    ଯଦି (blob == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	ଯାଅ failed;
    s += 2;
    ଯେତେବେଳେ (vim_isxdigit(*s))
    {
	ଯଦି (!vim_isxdigit(s[1]))
	    ଯାଅ failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	ଯଦି (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    ଯଦି (*skipwhite(s) != NUL)
	ଯାଅ failed;  // text after final digit

    ++blob->bv_refcount;
    ଫେରନ୍ତୁ blob;

failed:
    blob_free(blob);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
blob_slice(
	blob_T		*blob,
	long		len,
	varnumber_T	n1,
	varnumber_T	n2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		exclusive,
	typval_T	*rettv)
{
    ଯଦି (n1 < 0)
    {
	n1 = len + n1;
	ଯଦି (n1 < 0)
	    n1 = 0;
    }
    ଯଦି (n2 < 0)
	n2 = len + n2;
    ଅନ୍ୟଥା ଯଦି (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    ଯଦି (exclusive)
	--n2;
    ଯଦି (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	blob_T  *new_blob = blob_alloc();
	long    i;

	ଯଦି (new_blob != ଖାଲି)
	{
	    ଯଦି (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		ଫେରନ୍ତୁ FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    ପାଇଁ (i = n1; i <= n2; i++)
		blob_set(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_set(rettv, new_blob);
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
blob_index(
	blob_T		*blob,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    ଯଦି (idx < 0)
	idx = len + idx;
    ଯଦି (idx < len && idx >= 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_ସଂଖ୍ୟା;
	rettv->vval.v_ସଂଖ୍ୟା = v;
    }
    ଅନ୍ୟଥା
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
blob_slice_or_index(
	blob_T		*blob,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		exclusive,
	typval_T	*rettv)
{
    long	len = blob_len(blob);

    ଯଦି (is_range)
	ଫେରନ୍ତୁ blob_slice(blob, len, n1, n2, exclusive, rettv);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ blob_index(blob, len, n1, rettv);
    ଫେରନ୍ତୁ OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_blob_index(long bloblen, varnumber_T n1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quiet)
{
    ଯଦି (n1 < 0 || n1 > bloblen)
    {
	ଯଦି (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_blob_range(long bloblen, varnumber_T n1, varnumber_T n2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quiet)
{
    ଯଦି (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	ଯଦି (!quiet)
	    semsg(_(e_blob_index_out_of_range_nr), n2);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
blob_set_range(blob_T *dest, long n1, long n2, typval_T *src)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	il, ir;

    ଯଦି (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_ମୂଲ୍ୟ_does_not_have_ଡାହାଣ_ସଂଖ୍ୟା_of_bytes));
	ଫେରନ୍ତୁ FAIL;
    }

    ir = 0;
    ପାଇଁ (il = n1; il <= n2; il++)
	blob_set(dest, il, blob_get(src->vval.v_blob, ir++));
    ଫେରନ୍ତୁ OK;
}

/*
 * "add(blob, item)" function
 */
    ଶୂନ୍ୟ
blob_add(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    varnumber_T n;

    ଯଦି (b == ଖାଲି)
    {
	ଯଦି (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (ମୂଲ୍ୟ_check_lock(b->bv_lock, (char_u *)N_("add() argument"), TRUE))
	ଫେରନ୍ତୁ;

    n = tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ;

    ga_append(&b->bv_ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    ଶୂନ୍ୟ
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    long	idx;
    long	end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*p;

    ଯଦି (b != ଖାଲି && ମୂଲ୍ୟ_check_lock(b->bv_lock, arg_errmsg, TRUE))
	ଫେରନ୍ତୁ;

    idx = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ;

    len = blob_len(b);

    ଯଦି (idx < 0)
	// count from the end
	idx = len + idx;
    ଯଦି (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_index_out_of_range_nr), idx);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_data;
	rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	ଫେରନ୍ତୁ;
    }

    // Remove range of items, return blob with values.
    end = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ;
    ଯଦି (end < 0)
	// count from the end
	end = len + end;
    ଯଦି (end >= len || idx > end)
    {
	semsg(_(e_blob_index_out_of_range_nr), end);
	ଫେରନ୍ତୁ;
    }
    newblob = blob_alloc();
    ଯଦି (newblob == ଖାଲି)
	ଫେରନ୍ତୁ;
    newblob->bv_ga.ga_len = end - idx + 1;
    ଯଦି (ga_grow(&newblob->bv_ga, end - idx + 1) == FAIL)
    {
	vim_free(newblob);
	ଫେରନ୍ତୁ;
    }
    p = (char_u *)b->bv_ga.ga_data;
    mch_memmove((char_u *)newblob->bv_ga.ga_data, p + idx,
	    (size_t)(end - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    ଯଦି (len - end - 1 > 0)
	mch_memmove(p + idx, p + end + 1, (size_t)(len - end - 1));
    b->bv_ga.ga_len -= end - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    ଶୂନ୍ୟ
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*ପଦସମଷ୍ଟି,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ଯଦି (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ଖାଲି;
    }
    ଯଦି (b == ଖାଲି || (filtermap == FILTERMAP_FILTER
			    && ମୂଲ୍ୟ_check_lock(b->bv_lock, arg_errmsg, TRUE)))
	ଫେରନ୍ତୁ;

    b_ret = b;
    ଯଦି (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_lock = 0;
	rettv->vval.v_blob = blob_copy(b);
	ଯଦି (rettv->vval.v_blob == ଖାଲି)
	    ଫେରନ୍ତୁ;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ଚାବି, VAR_ସଂଖ୍ୟା);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_lock = b->bv_lock;
    ଯଦି (b->bv_lock == 0)
	b->bv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ପଦସମଷ୍ଟି_get_funccal(ପଦସମଷ୍ଟି, &newtv);

    ପାଇଁ (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_ସଂଖ୍ୟା;
	val = blob_get(b, i);
	tv.vval.v_ସଂଖ୍ୟା = val;
	set_vim_var_nr(VV_ଚାବି, idx);
	ଯଦି (filter_map_one(&tv, ପଦସମଷ୍ଟି, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    ଭାଙ୍ଗ;
	ଯଦି (filtermap != FILTERMAP_FOREACH)
	{
	    ଯଦି (newtv.v_type != VAR_ସଂଖ୍ୟା && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (filtermap != FILTERMAP_FILTER)
	    {
		ଯଦି (newtv.vval.v_ସଂଖ୍ୟା != val)
		    blob_set(b_ret, i, newtv.vval.v_ସଂଖ୍ୟା);
	    }
	    ଅନ୍ୟଥା ଯଦି (rem)
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
    ଯଦି (fc != ଖାଲି)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    ଶୂନ୍ୟ
blob_ଭର୍ତ୍ତି_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    long	before = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		val, len;
    char_u	*p;

    ଯଦି (b == ଖାଲି)
    {
	ଯଦି (in_vim9script())
	    emsg(_(e_cannot_add_to_null_blob));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (ମୂଲ୍ୟ_check_lock(b->bv_lock, (char_u *)N_("insert() argument"), TRUE))
	ଫେରନ୍ତୁ;

    len = blob_len(b);
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	ଯଦି (error)
	    ଫେରନ୍ତୁ;		// type error; errmsg already given
	ଯଦି (before < 0 || before > len)
	{
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
	    ଫେରନ୍ତୁ;
	}
    }
    val = tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ;
    ଯଦି (val < 0 || val > 255)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (ga_grow(&b->bv_ga, 1) == FAIL)
	ଫେରନ୍ତୁ;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
blob_extend(blob_T *b1, blob_T *b2, long bef)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    ଯଦି (b2 == ଖାଲି || b2->bv_ga.ga_len == 0)
	ଫେରନ୍ତୁ OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    ଯଦି (ga_grow(&b1->bv_ga, len2) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    p1 = (char_u *)b1->bv_ga.ga_data;

    ଯଦି (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	ଯଦି (bef < len1)
	    memcpy(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	memcpy(p1 + bef, b2->bv_ga.ga_data, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    ଫେରନ୍ତୁ OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ଶୂନ୍ୟ
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    long	before;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    ଯଦି (b1 == ଖାଲି)
    {
	emsg(_(e_cannot_extend_null_blob));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (is_new || !ମୂଲ୍ୟ_check_lock(b1->bv_lock, arg_errmsg, TRUE))
    {
	ଯଦି (is_new)
	{
	    b1 = blob_copy(b1);
	    ଯଦି (b1 == ଖାଲି)
		ଫେରନ୍ତୁ;
	}

	b2 = argvars[1].vval.v_blob;
	ଯଦି (b2 == ଖାଲି)
	    ଯାଅ theend;

	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	    ଯଦି (error)
		ଯାଅ cleanup;		// type error; errmsg already given
	    ଯଦି (before < 0)
		before = b1->bv_ga.ga_len + before;
	    ଯଦି (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), before);
		ଯାଅ cleanup;
	    }
	}
	ଅନ୍ୟଥା
	    before = b1->bv_ga.ga_len;
	ଯଦି (blob_extend(b1, b2, before) == FAIL)
	    ଯାଅ cleanup;

theend:
	ଯଦି (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_lock = 0;
	    rettv->vval.v_blob = b1;
	}
	ଅନ୍ୟଥା
	    copy_tv(&argvars[0], rettv);
	ଫେରନ୍ତୁ;

cleanup:
	ଯଦି (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    ଶୂନ୍ୟ
blob_reduce(
	typval_T	*argvars,
	typval_T	*ପଦସମଷ୍ଟି,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_start = called_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    typval_T	initial;
    typval_T	argv[3];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

    ଯଦି (argvars[2].v_type == VAR_UNKNOWN)
    {
	ଯଦି (b == ଖାଲି || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_ମୂଲ୍ୟ), "Blob");
	    ଫେରନ୍ତୁ;
	}
	initial.v_type = VAR_ସଂଖ୍ୟା;
	initial.vval.v_ସଂଖ୍ୟା = blob_get(b, 0);
	i = 1;
    }
    ଅନ୍ୟଥା ଯଦି (check_for_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ;
    ଅନ୍ୟଥା
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    ଯଦି (b == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_ସଂଖ୍ୟା;
	argv[1].vval.v_ସଂଖ୍ୟା = blob_get(b, i);

	r = eval_ପଦସମଷ୍ଟି_typval(ପଦସମଷ୍ଟି, TRUE, argv, 2, ଖାଲି, rettv);

	clear_tv(&argv[0]);
	ଯଦି (r == FAIL || called_emsg != called_emsg_start)
	    ଫେରନ୍ତୁ;
    }
}

/*
 * "reverse({blob})" function
 */
    ଶୂନ୍ୟ
blob_reverse(blob_T *b, typval_T *rettv)
{
    long    i, len = blob_len(b);

    ପାଇଁ (i = 0; i < len / 2; i++)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp = blob_get(b, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)i);

	blob_set(b, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)i, blob_get(b, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(len - i - 1)));
	blob_set(b, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(len - i - 1), tmp);
    }
    rettv_blob_set(rettv, b);
}

/*
 * blob2list() function
 */
    ଶୂନ୍ୟ
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, len;
    typval_T	tv;

    ଯଦି (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(ଶୂନ୍ୟ)rettv_list_alloc(rettv);
	ଫେରନ୍ତୁ;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(len);
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;
    rettv_list_set(rettv, l);

    tv.v_type = VAR_ସଂଖ୍ୟା;
    tv.v_lock = 0;
    ପାଇଁ (i = 0; i < len; i++)
    {
	tv.vval.v_ସଂଖ୍ୟା = blob_get(blob, i);
	list_set_item(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    ଶୂନ୍ୟ
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    ଯଦି (rettv_blob_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    blob = rettv->vval.v_blob;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    l = argvars->vval.v_list;
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;

    // The result has one byte per list item; reserve all space at once.
    ଯଦି (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	ଫେରନ୍ତୁ;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_ସଂଖ୍ୟା_chk(&li->li_tv, &error);
	ଯଦି (error == TRUE || n < 0 || n > 255)
	{
	    ଯଦି (!error)
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    ଫେରନ୍ତୁ;
	}
	((char_u *)blob->bv_ga.ga_data)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#endif // defined(FEAT_EVAL)
