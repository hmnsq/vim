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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(శూన్య)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    ఒకవేళ (blob != ఖాళీ)
	ga_init2(&blob->bv_ga, 1, 100);
    తిరిగి blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    ఒకవేళ (b == ఖాళీ)
	తిరిగి FAIL;

    rettv_blob_సమితి(rettv, b);
    తిరిగి OK;
}

/*
 * Set a blob as the return value.
 */
    శూన్య
rettv_blob_సమితి(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    ఒకవేళ (b != ఖాళీ)
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
    పూర్ణాంకం		len;
    blob_T	*copy;

    ఒకవేళ (orig == ఖాళీ)
	తిరిగి ఖాళీ;

    copy = blob_alloc();
    ఒకవేళ (copy == ఖాళీ)
	తిరిగి ఖాళీ;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    ఒకవేళ (len > 0)
    {
	copy->bv_ga.ga_దత్తాంశం =
	    vim_memsave(orig->bv_ga.ga_దత్తాంశం, len);
	ఒకవేళ (copy->bv_ga.ga_దత్తాంశం == ఖాళీ)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    తిరిగి copy;
}

    శూన్య
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    శూన్య
blob_unref(blob_T *b)
{
    ఒకవేళ (b != ఖాళీ && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    దీర్ఘ
blob_len(blob_T *b)
{
    ఒకవేళ (b == ఖాళీ)
	తిరిగి 0L;
    తిరిగి b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    పూర్ణాంకం
blob_get(blob_T *b, పూర్ణాంకం idx)
{
    తిరిగి ((char_u*)b->bv_ga.ga_దత్తాంశం)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    శూన్య
blob_సమితి(blob_T *blob, పూర్ణాంకం idx, పూర్ణాంకం byte)
{
    ((char_u*)blob->bv_ga.ga_దత్తాంశం)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    శూన్య
blob_సమితి_append(blob_T *blob, పూర్ణాంకం idx, పూర్ణాంకం byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    ఒకవేళ (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_సమితి(blob, idx, byte);
	ఒకవేళ (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    పూర్ణాంకం
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    పూర్ణాంకం	    i;
    దీర్ఘ    len1 = blob_len(b1);
    దీర్ఘ    len2 = blob_len(b2);

    // empty and NULL are considered the same
    ఒకవేళ (len1 == 0 && len2 == 0)
	తిరిగి TRUE;
    ఒకవేళ (b1 == b2)
	తిరిగి TRUE;
    ఒకవేళ (len1 != len2)
	తిరిగి FALSE;

    కొరకు (i = 0; i < b1->bv_ga.ga_len; i++)
	ఒకవేళ (blob_get(b1, i) != blob_get(b2, i)) తిరిగి FALSE;
    తిరిగి TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    పూర్ణాంకం
read_blob(FILE *fd, typval_T *rettv, off_T స్థానభ్రంశం, off_T కొలత_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    నిర్మాణం stat	st;
    పూర్ణాంకం		whence;
    off_T	కొలత = కొలత_arg;

    ఒకవేళ (fstat(fileno(fd), &st) < 0)
	తిరిగి FAIL;  // can't read the file, error

    ఒకవేళ (స్థానభ్రంశం >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	ఒకవేళ (కొలత == -1 || (కొలత > st.st_size - స్థానభ్రంశం
#నిర్వచితమైతే S_ISCHR
		    && !S_ISCHR(st.st_బహుళకం)
#అయితే_ముగింపు
		    ))
	    // size may become negative, checked below
	    కొలత = st.st_size - స్థానభ్రంశం;
	whence = SEEK_SET;
    }
    లేకపోతే
    {
	// limit the offset to not go before the start of the file
	ఒకవేళ (-స్థానభ్రంశం > st.st_size
#నిర్వచితమైతే S_ISCHR
		    && !S_ISCHR(st.st_బహుళకం)
#అయితే_ముగింపు
		    )
	    స్థానభ్రంశం = -st.st_size;
	// Size defaults to reading until the end of the file.
	ఒకవేళ (కొలత == -1 || కొలత > -స్థానభ్రంశం)
	    కొలత = -స్థానభ్రంశం;
	whence = SEEK_END;
    }
    ఒకవేళ (కొలత <= 0)
	తిరిగి OK;
    ఒకవేళ (స్థానభ్రంశం != 0 && vim_fseek(fd, స్థానభ్రంశం, whence) != 0)
	తిరిగి OK;

    ఒకవేళ (ga_grow(&blob->bv_ga, (పూర్ణాంకం)కొలత) == FAIL)
	తిరిగి FAIL;
    blob->bv_ga.ga_len = (పూర్ణాంకం)కొలత;
    ఒకవేళ (fread(blob->bv_ga.ga_దత్తాంశం, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = ఖాళీ;
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    పూర్ణాంకం
write_blob(FILE *fd, blob_T *blob)
{
    ఒకవేళ (fwrite(blob->bv_ga.ga_దత్తాంశం, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf వాడని)
{
    స్థిర స్థిరరాశి అక్షరం hex_chars[] = "0123456789ABCDEF";
    పూర్ణాంకం		blen;
    size_t	మొత్తం;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    ఒకవేళ (blob == ఖాళీ || (blen = blob_len(blob)) == 0)
    {
	*tofree = ఖాళీ;
	తిరిగి (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    మొత్తం = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(మొత్తం);
    ఒకవేళ (buf == ఖాళీ)
    {
	*tofree = ఖాళీ;
	తిరిగి (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_దత్తాంశం;
    కొరకు (పూర్ణాంకం i = 0; i < blen; i++)
    {
	గుర్తులేని b;

	ఒకవేళ (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    తిరిగి buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    శూన్య
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    కొరకు (పూర్ణాంకం i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	ఒకవేళ (l2 == ఖాళీ)
	    తిరిగి;

	ఒకవేళ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    తిరిగి;
	}

	ఒకవేళ (list_append_సంఖ్య(l2, i) == FAIL
		|| list_append_సంఖ్య(l2, blob_get(blob, i)) == FAIL)
	    తిరిగి;
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

    ఒకవేళ (blob == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	వెళ్ళు failed;
    s += 2;
    అయితే (vim_isxdigit(*s))
    {
	ఒకవేళ (!vim_isxdigit(s[1]))
	    వెళ్ళు failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	ఒకవేళ (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    ఒకవేళ (*skipwhite(s) != NUL)
	వెళ్ళు failed;  // text after final digit

    ++blob->bv_refcount;
    తిరిగి blob;

failed:
    blob_free(blob);
    తిరిగి ఖాళీ;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    స్థిర పూర్ణాంకం
blob_ముక్క(
	blob_T		*blob,
	దీర్ఘ		len,
	varnumber_T	n1,
	varnumber_T	n2,
	పూర్ణాంకం		exclusive,
	typval_T	*rettv)
{
    ఒకవేళ (n1 < 0)
    {
	n1 = len + n1;
	ఒకవేళ (n1 < 0)
	    n1 = 0;
    }
    ఒకవేళ (n2 < 0)
	n2 = len + n2;
    లేకపోతే ఒకవేళ (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    ఒకవేళ (exclusive)
	--n2;
    ఒకవేళ (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ఖాళీ;
    }
    లేకపోతే
    {
	blob_T  *new_blob = blob_alloc();
	దీర్ఘ    i;

	ఒకవేళ (new_blob != ఖాళీ)
	{
	    ఒకవేళ (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		తిరిగి FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    కొరకు (i = n1; i <= n2; i++)
		blob_సమితి(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_సమితి(rettv, new_blob);
	}
    }

    తిరిగి OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    స్థిర పూర్ణాంకం
blob_సూచీ(
	blob_T		*blob,
	పూర్ణాంకం		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    ఒకవేళ (idx < 0)
	idx = len + idx;
    ఒకవేళ (idx < len && idx >= 0)
    {
	పూర్ణాంకం v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_సంఖ్య;
	rettv->vval.v_సంఖ్య = v;
    }
    లేకపోతే
    {
	semsg(_(e_blob_సూచీ_బహిర్గతం_యొక్క_range_nr), idx);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

    పూర్ణాంకం
blob_ముక్క_or_సూచీ(
	blob_T		*blob,
	పూర్ణాంకం		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	పూర్ణాంకం		exclusive,
	typval_T	*rettv)
{
    దీర్ఘ	len = blob_len(blob);

    ఒకవేళ (is_range)
	తిరిగి blob_ముక్క(blob, len, n1, n2, exclusive, rettv);
    లేకపోతే
	తిరిగి blob_సూచీ(blob, len, n1, rettv);
    తిరిగి OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    పూర్ణాంకం
check_blob_సూచీ(దీర్ఘ bloblen, varnumber_T n1, పూర్ణాంకం quiet)
{
    ఒకవేళ (n1 < 0 || n1 > bloblen)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_blob_సూచీ_బహిర్గతం_యొక్క_range_nr), n1);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    పూర్ణాంకం
check_blob_range(దీర్ఘ bloblen, varnumber_T n1, varnumber_T n2, పూర్ణాంకం quiet)
{
    ఒకవేళ (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_blob_సూచీ_బహిర్గతం_యొక్క_range_nr), n2);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    పూర్ణాంకం
blob_సమితి_range(blob_T *dest, దీర్ఘ n1, దీర్ఘ n2, typval_T *src)
{
    పూర్ణాంకం	il, ir;

    ఒకవేళ (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_విలువ_does_not_have_కుడి_సంఖ్య_యొక్క_బైట్లు));
	తిరిగి FAIL;
    }

    ir = 0;
    కొరకు (il = n1; il <= n2; il++)
	blob_సమితి(dest, il, blob_get(src->vval.v_blob, ir++));
    తిరిగి OK;
}

/*
 * "add(blob, item)" function
 */
    శూన్య
blob_కూడు(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    పూర్ణాంకం		error = FALSE;
    varnumber_T n;

    ఒకవేళ (b == ఖాళీ)
    {
	ఒకవేళ (లోపల_vim9script())
	    emsg(_(e_cannot_కూడు_గమ్యం_null_blob));
	తిరిగి;
    }

    ఒకవేళ (విలువ_check_తాళం(b->bv_తాళం, (char_u *)N_("add() argument"), TRUE))
	తిరిగి;

    n = tv_get_సంఖ్య_chk(&argvars[1], &error);
    ఒకవేళ (error)
	తిరిగి;

    ga_append(&b->bv_ga, (పూర్ణాంకం)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    శూన్య
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    పూర్ణాంకం		error = FALSE;
    దీర్ఘ	idx;
    దీర్ఘ	చివర;
    పూర్ణాంకం		len;
    char_u	*p;

    ఒకవేళ (b != ఖాళీ && విలువ_check_తాళం(b->bv_తాళం, arg_errmsg, TRUE))
	తిరిగి;

    idx = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[1], &error);
    ఒకవేళ (error)
	తిరిగి;

    len = blob_len(b);

    ఒకవేళ (idx < 0)
	// count from the end
	idx = len + idx;
    ఒకవేళ (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_సూచీ_బహిర్గతం_యొక్క_range_nr), idx);
	తిరిగి;
    }
    ఒకవేళ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_దత్తాంశం;
	rettv->vval.v_సంఖ్య = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	తిరిగి;
    }

    // Remove range of items, return blob with values.
    చివర = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[2], &error);
    ఒకవేళ (error)
	తిరిగి;
    ఒకవేళ (చివర < 0)
	// count from the end
	చివర = len + చివర;
    ఒకవేళ (చివర >= len || idx > చివర)
    {
	semsg(_(e_blob_సూచీ_బహిర్గతం_యొక్క_range_nr), చివర);
	తిరిగి;
    }
    newblob = blob_alloc();
    ఒకవేళ (newblob == ఖాళీ)
	తిరిగి;
    newblob->bv_ga.ga_len = చివర - idx + 1;
    ఒకవేళ (ga_grow(&newblob->bv_ga, చివర - idx + 1) == FAIL)
    {
	vim_free(newblob);
	తిరిగి;
    }
    p = (char_u *)b->bv_ga.ga_దత్తాంశం;
    mch_memmove((char_u *)newblob->bv_ga.ga_దత్తాంశం, p + idx,
	    (size_t)(చివర - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    ఒకవేళ (len - చివర - 1 > 0)
	mch_memmove(p + idx, p + చివర + 1, (size_t)(len - చివర - 1));
    b->bv_ga.ga_len -= చివర - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    శూన్య
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*సమాసం,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    పూర్ణాంకం		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    పూర్ణాంకం		idx = 0;
    పూర్ణాంకం		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ఒకవేళ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = ఖాళీ;
    }
    ఒకవేళ (b == ఖాళీ || (filtermap == FILTERMAP_FILTER
			    && విలువ_check_తాళం(b->bv_తాళం, arg_errmsg, TRUE)))
	తిరిగి;

    b_ret = b;
    ఒకవేళ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_తాళం = 0;
	rettv->vval.v_blob = blob_copy(b);
	ఒకవేళ (rettv->vval.v_blob == ఖాళీ)
	    తిరిగి;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    సమితి_vim_var_type(VV_కీలకం, VAR_సంఖ్య);

    పూర్ణాంకం prev_తాళం = b->bv_తాళం;
    ఒకవేళ (b->bv_తాళం == 0)
	b->bv_తాళం = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_సమాసం_get_funccal(సమాసం, &newtv);

    కొరకు (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_సంఖ్య;
	val = blob_get(b, i);
	tv.vval.v_సంఖ్య = val;
	సమితి_vim_var_nr(VV_కీలకం, idx);
	ఒకవేళ (filter_map_one(&tv, సమాసం, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    ఆపు;
	ఒకవేళ (filtermap != FILTERMAP_FOREACH)
	{
	    ఒకవేళ (newtv.v_type != VAR_సంఖ్య && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		ఆపు;
	    }
	    ఒకవేళ (filtermap != FILTERMAP_FILTER)
	    {
		ఒకవేళ (newtv.vval.v_సంఖ్య != val)
		    blob_సమితి(b_ret, i, newtv.vval.v_సంఖ్య);
	    }
	    లేకపోతే ఒకవేళ (rem)
	    {
		char_u *p = (char_u *)blob_arg->bv_ga.ga_దత్తాంశం;

		mch_memmove(p + i, p + i + 1,
			    (size_t)b->bv_ga.ga_len - i - 1);
		--b->bv_ga.ga_len;
		--i;
	    }
	}
	++idx;
    }

    b->bv_తాళం = prev_తాళం;
    ఒకవేళ (fc != ఖాళీ)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    శూన్య
blob_చొప్పించు_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    దీర్ఘ	before = 0;
    పూర్ణాంకం		error = FALSE;
    పూర్ణాంకం		val, len;
    char_u	*p;

    ఒకవేళ (b == ఖాళీ)
    {
	ఒకవేళ (లోపల_vim9script())
	    emsg(_(e_cannot_కూడు_గమ్యం_null_blob));
	తిరిగి;
    }

    ఒకవేళ (విలువ_check_తాళం(b->bv_తాళం, (char_u *)N_("insert() argument"), TRUE))
	తిరిగి;

    len = blob_len(b);
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[2], &error);
	ఒకవేళ (error)
	    తిరిగి;		// type error; errmsg already given
	ఒకవేళ (before < 0 || before > len)
	{
	    semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[2]));
	    తిరిగి;
	}
    }
    val = tv_get_సంఖ్య_chk(&argvars[1], &error);
    ఒకవేళ (error)
	తిరిగి;
    ఒకవేళ (val < 0 || val > 255)
    {
	semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[1]));
	తిరిగి;
    }

    ఒకవేళ (ga_grow(&b->bv_ga, 1) == FAIL)
	తిరిగి;
    p = (char_u *)b->bv_ga.ga_దత్తాంశం;
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
    స్థిర పూర్ణాంకం
blob_extend(blob_T *b1, blob_T *b2, దీర్ఘ bef)
{
    పూర్ణాంకం		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    ఒకవేళ (b2 == ఖాళీ || b2->bv_ga.ga_len == 0)
	తిరిగి OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    ఒకవేళ (ga_grow(&b1->bv_ga, len2) == FAIL)
	తిరిగి FAIL;

    p1 = (char_u *)b1->bv_ga.ga_దత్తాంశం;

    ఒకవేళ (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	ఒకవేళ (bef < len1)
	    జ్ఞాపకం_నకలు(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    లేకపోతే
    {
	ఒకవేళ (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	జ్ఞాపకం_నకలు(p1 + bef, b2->bv_ga.ga_దత్తాంశం, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    తిరిగి OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    శూన్య
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	పూర్ణాంకం		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    దీర్ఘ	before;
    పూర్ణాంకం		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    ఒకవేళ (b1 == ఖాళీ)
    {
	emsg(_(e_cannot_extend_null_blob));
	తిరిగి;
    }
    ఒకవేళ (is_new || !విలువ_check_తాళం(b1->bv_తాళం, arg_errmsg, TRUE))
    {
	ఒకవేళ (is_new)
	{
	    b1 = blob_copy(b1);
	    ఒకవేళ (b1 == ఖాళీ)
		తిరిగి;
	}

	b2 = argvars[1].vval.v_blob;
	ఒకవేళ (b2 == ఖాళీ)
	    వెళ్ళు theend;

	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[2], &error);
	    ఒకవేళ (error)
		వెళ్ళు cleanup;		// type error; errmsg already given
	    ఒకవేళ (before < 0)
		before = b1->bv_ga.ga_len + before;
	    ఒకవేళ (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_సూచీ_బహిర్గతం_యొక్క_range_nr), before);
		వెళ్ళు cleanup;
	    }
	}
	లేకపోతే
	    before = b1->bv_ga.ga_len;
	ఒకవేళ (blob_extend(b1, b2, before) == FAIL)
	    వెళ్ళు cleanup;

theend:
	ఒకవేళ (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_తాళం = 0;
	    rettv->vval.v_blob = b1;
	}
	లేకపోతే
	    copy_tv(&argvars[0], rettv);
	తిరిగి;

cleanup:
	ఒకవేళ (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    శూన్య
blob_reduce(
	typval_T	*argvars,
	typval_T	*సమాసం,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    పూర్ణాంకం		called_emsg_ప్రారంభం = called_emsg;
    పూర్ణాంకం		r;
    typval_T	initial;
    typval_T	argv[3];
    పూర్ణాంకం	i;

    ఒకవేళ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ఒకవేళ (b == ఖాళీ || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_యొక్క_an_ఖాళీలు_str_with_no_initial_విలువ), "Blob");
	    తిరిగి;
	}
	initial.v_type = VAR_సంఖ్య;
	initial.vval.v_సంఖ్య = blob_get(b, 0);
	i = 1;
    }
    లేకపోతే ఒకవేళ (check_for_సంఖ్య_arg(argvars, 2) == FAIL)
	తిరిగి;
    లేకపోతే
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    ఒకవేళ (b == ఖాళీ)
	తిరిగి;

    కొరకు ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_సంఖ్య;
	argv[1].vval.v_సంఖ్య = blob_get(b, i);

	r = eval_సమాసం_typval(సమాసం, TRUE, argv, 2, ఖాళీ, rettv);

	clear_tv(&argv[0]);
	ఒకవేళ (r == FAIL || called_emsg != called_emsg_ప్రారంభం)
	    తిరిగి;
    }
}

/*
 * "reverse({blob})" function
 */
    శూన్య
blob_తిరగేయి(blob_T *b, typval_T *rettv)
{
    దీర్ఘ    i, len = blob_len(b);

    కొరకు (i = 0; i < len / 2; i++)
    {
	పూర్ణాంకం tmp = blob_get(b, (పూర్ణాంకం)i);

	blob_సమితి(b, (పూర్ణాంకం)i, blob_get(b, (పూర్ణాంకం)(len - i - 1)));
	blob_సమితి(b, (పూర్ణాంకం)(len - i - 1), tmp);
    }
    rettv_blob_సమితి(rettv, b);
}

/*
 * blob2list() function
 */
    శూన్య
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    పూర్ణాంకం		i, len;
    typval_T	tv;

    ఒకవేళ (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(శూన్య)rettv_list_alloc(rettv);
	తిరిగి;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_వస్తువులు(len);
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;
    rettv_list_సమితి(rettv, l);

    tv.v_type = VAR_సంఖ్య;
    tv.v_తాళం = 0;
    కొరకు (i = 0; i < len; i++)
    {
	tv.vval.v_సంఖ్య = blob_get(blob, i);
	list_సమితి_అంశం(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    శూన్య
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    ఒకవేళ (rettv_blob_alloc(rettv) == FAIL)
	తిరిగి;
    blob = rettv->vval.v_blob;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    l = argvars->vval.v_list;
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;

    // The result has one byte per list item; reserve all space at once.
    ఒకవేళ (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	తిరిగి;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	పూర్ణాంకం		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_సంఖ్య_chk(&li->li_tv, &error);
	ఒకవేళ (error == TRUE || n < 0 || n > 255)
	{
	    ఒకవేళ (!error)
		semsg(_(e_invalid_విలువ_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    తిరిగి;
	}
	((char_u *)blob->bv_ga.ga_దత్తాంశం)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#అయితే_ముగింపు // defined(FEAT_EVAL)
