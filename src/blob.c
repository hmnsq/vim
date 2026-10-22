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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

/*
 * Allocate an empty blob.
 * Caller should take care of the reference count.
 */
    blob_T *
blob_alloc(خالی)
{
    blob_T *blob = ALLOC_CLEAR_ONE_ID(blob_T, aid_blob_alloc);

    اگر (blob != تہی)
	ga_init2(&blob->bv_ga, 1, 100);
    واپس blob;
}

/*
 * Allocate an empty blob for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    صحیح
rettv_blob_alloc(typval_T *rettv)
{
    blob_T	*b = blob_alloc();

    اگر (b == تہی)
	واپس FAIL;

    rettv_blob_سیٹ(rettv, b);
    واپس OK;
}

/*
 * Set a blob as the return value.
 */
    خالی
rettv_blob_سیٹ(typval_T *rettv, blob_T *b)
{
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = b;
    اگر (b != تہی)
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
    صحیح		len;
    blob_T	*copy;

    اگر (orig == تہی)
	واپس تہی;

    copy = blob_alloc();
    اگر (copy == تہی)
	واپس تہی;
    ++copy->bv_refcount;

    len = orig->bv_ga.ga_len;
    اگر (len > 0)
    {
	copy->bv_ga.ga_ڈیٹا =
	    vim_memsave(orig->bv_ga.ga_ڈیٹا, len);
	اگر (copy->bv_ga.ga_ڈیٹا == تہی)
	    len = 0;
    }
    copy->bv_ga.ga_len = len;
    copy->bv_ga.ga_maxlen = len;

    واپس copy;
}

    خالی
blob_free(blob_T *b)
{
    ga_clear(&b->bv_ga);
    vim_free(b);
}

/*
 * Unreference a blob: decrement the reference count and free it when it
 * becomes zero.
 */
    خالی
blob_unref(blob_T *b)
{
    اگر (b != تہی && --b->bv_refcount <= 0)
	blob_free(b);
}

/*
 * Get the length of data.
 */
    طویل
blob_len(blob_T *b)
{
    اگر (b == تہی)
	واپس 0L;
    واپس b->bv_ga.ga_len;
}

/*
 * Get byte "idx" in blob "b".
 * Caller must check that "idx" is valid.
 */
    صحیح
blob_get(blob_T *b, صحیح idx)
{
    واپس ((char_u*)b->bv_ga.ga_ڈیٹا)[idx];
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Caller must make sure that "idx" is valid.
 */
    خالی
blob_سیٹ(blob_T *blob, صحیح idx, صحیح byte)
{
    ((char_u*)blob->bv_ga.ga_ڈیٹا)[idx] = byte;
}

/*
 * Store one byte "byte" in blob "blob" at "idx".
 * Append one byte if needed.
 */
    خالی
blob_سیٹ_append(blob_T *blob, صحیح idx, صحیح byte)
{
    garray_T *gap = &blob->bv_ga;

    // Allow for appending a byte.  Setting a byte beyond
    // the end is an error otherwise.
    اگر (idx < gap->ga_len
	    || (idx == gap->ga_len && ga_grow(gap, 1) == OK))
    {
	blob_سیٹ(blob, idx, byte);
	اگر (idx == gap->ga_len)
	    ++gap->ga_len;
    }
}

/*
 * Return TRUE when two blobs have exactly the same values.
 */
    صحیح
blob_equal(
    blob_T	*b1,
    blob_T	*b2)
{
    صحیح	    i;
    طویل    len1 = blob_len(b1);
    طویل    len2 = blob_len(b2);

    // empty and NULL are considered the same
    اگر (len1 == 0 && len2 == 0)
	واپس TRUE;
    اگر (b1 == b2)
	واپس TRUE;
    اگر (len1 != len2)
	واپس FALSE;

    برائے (i = 0; i < b1->bv_ga.ga_len; i++)
	اگر (blob_get(b1, i) != blob_get(b2, i)) واپس FALSE;
    واپس TRUE;
}

/*
 * Read blob from file "fd".
 * Caller has allocated a blob in "rettv".
 * Return OK or FAIL.
 */
    صحیح
read_blob(FILE *fd, typval_T *rettv, off_T انحراف, off_T سائز_arg)
{
    blob_T	*blob = rettv->vval.v_blob;
    ساخت stat	st;
    صحیح		whence;
    off_T	سائز = سائز_arg;

    اگر (fstat(fileno(fd), &st) < 0)
	واپس FAIL;  // can't read the file, error

    اگر (انحراف >= 0)
    {
	// The size defaults to the whole file.  If a size is given it is
	// limited to not go past the end of the file.
	اگر (سائز == -1 || (سائز > st.st_size - انحراف
#اگر_تعریف_شدہ S_ISCHR
		    && !S_ISCHR(st.st_منوال)
#اگر_ختم
		    ))
	    // size may become negative, checked below
	    سائز = st.st_size - انحراف;
	whence = SEEK_SET;
    }
    ورنہ
    {
	// limit the offset to not go before the start of the file
	اگر (-انحراف > st.st_size
#اگر_تعریف_شدہ S_ISCHR
		    && !S_ISCHR(st.st_منوال)
#اگر_ختم
		    )
	    انحراف = -st.st_size;
	// Size defaults to reading until the end of the file.
	اگر (سائز == -1 || سائز > -انحراف)
	    سائز = -انحراف;
	whence = SEEK_END;
    }
    اگر (سائز <= 0)
	واپس OK;
    اگر (انحراف != 0 && vim_fseek(fd, انحراف, whence) != 0)
	واپس OK;

    اگر (ga_grow(&blob->bv_ga, (صحیح)سائز) == FAIL)
	واپس FAIL;
    blob->bv_ga.ga_len = (صحیح)سائز;
    اگر (fread(blob->bv_ga.ga_ڈیٹا, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	// An empty blob is returned on error.
	blob_free(rettv->vval.v_blob);
	rettv->vval.v_blob = تہی;
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Write "blob" to file "fd".
 * Return OK or FAIL.
 */
    صحیح
write_blob(FILE *fd, blob_T *blob)
{
    اگر (fwrite(blob->bv_ga.ga_ڈیٹا, 1, blob->bv_ga.ga_len, fd)
						  < (size_t)blob->bv_ga.ga_len)
    {
	emsg(_(e_error_while_writing));
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Convert a blob to a readable form: "0z00112233.44556677.8899"
 */
    char_u *
blob2string(blob_T *blob, char_u **tofree, char_u *numbuf غیر_مستعمل)
{
    ساکن ثابت حرف hex_chars[] = "0123456789ABCDEF";
    صحیح		blen;
    size_t	کل;
    char_u	*buf;
    char_u	*p;
    char_u	*src;

    اگر (blob == تہی || (blen = blob_len(blob)) == 0)
    {
	*tofree = تہی;
	واپس (char_u *)"0z";
    }

    // 2 ("0z") + 2 hex per byte + one '.' every 4 bytes + NUL terminator.
    کل = 2 + (size_t)blen * 2 + (size_t)((blen - 1) / 4) + 1;
    buf = alloc(کل);
    اگر (buf == تہی)
    {
	*tofree = تہی;
	واپس (char_u *)"0z";
    }

    p = buf;
    *p++ = '0';
    *p++ = 'z';
    src = (char_u *)blob->bv_ga.ga_ڈیٹا;
    برائے (صحیح i = 0; i < blen; i++)
    {
	غیرعلامتی b;

	اگر (i > 0 && (i & 3) == 0)
	    *p++ = '.';
	b = src[i];
	*p++ = hex_chars[b >> 4];
	*p++ = hex_chars[b & 0xf];
    }
    *p = NUL;
    *tofree = buf;
    واپس buf;
}

/*
 * "items(blob)" function
 * Converts a Blob into a List of [index, byte] pairs.
 * Caller must have already checked that argvars[0] is a Blob.
 * A null blob behaves like an empty blob.
 */
    خالی
blob2items(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob = argvars[0].vval.v_blob;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    برائے (صحیح i = 0; i < blob_len(blob); i++)
    {
	list_T	*l2 = list_alloc();
	اگر (l2 == تہی)
	    واپس;

	اگر (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    واپس;
	}

	اگر (list_append_نمبر(l2, i) == FAIL
		|| list_append_نمبر(l2, blob_get(blob, i)) == FAIL)
	    واپس;
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

    اگر (blob == تہی)
	واپس تہی;
    اگر (s[0] != '0' || (s[1] != 'z' && s[1] != 'Z'))
	جاؤ failed;
    s += 2;
    جب (vim_isxdigit(*s))
    {
	اگر (!vim_isxdigit(s[1]))
	    جاؤ failed;
	ga_append(&blob->bv_ga, (hex2nr(s[0]) << 4) + hex2nr(s[1]));
	s += 2;
	اگر (*s == '.' && vim_isxdigit(s[1]))
	    ++s;
    }
    اگر (*skipwhite(s) != NUL)
	جاؤ failed;  // text after final digit

    ++blob->bv_refcount;
    واپس blob;

failed:
    blob_free(blob);
    واپس تہی;
}

/*
 * Returns a slice of 'blob' from index 'n1' to 'n2' in 'rettv'.  The length of
 * the blob is 'len'.  Returns an empty blob if the indexes are out of range.
 */
    ساکن صحیح
blob_قاش(
	blob_T		*blob,
	طویل		len,
	varnumber_T	n1,
	varnumber_T	n2,
	صحیح		exclusive,
	typval_T	*rettv)
{
    اگر (n1 < 0)
    {
	n1 = len + n1;
	اگر (n1 < 0)
	    n1 = 0;
    }
    اگر (n2 < 0)
	n2 = len + n2;
    ورنہ اگر (n2 >= len)
	n2 = len - (exclusive ? 0 : 1);
    اگر (exclusive)
	--n2;
    اگر (n1 >= len || n2 < 0 || n1 > n2)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = تہی;
    }
    ورنہ
    {
	blob_T  *new_blob = blob_alloc();
	طویل    i;

	اگر (new_blob != تہی)
	{
	    اگر (ga_grow(&new_blob->bv_ga, n2 - n1 + 1) == FAIL)
	    {
		blob_free(new_blob);
		واپس FAIL;
	    }
	    new_blob->bv_ga.ga_len = n2 - n1 + 1;
	    برائے (i = n1; i <= n2; i++)
		blob_سیٹ(new_blob, i - n1, blob_get(blob, i));

	    clear_tv(rettv);
	    rettv_blob_سیٹ(rettv, new_blob);
	}
    }

    واپس OK;
}

/*
 * Return the byte value in 'blob' at index 'idx' in 'rettv'.  If the index is
 * too big or negative that is an error.  The length of the blob is 'len'.
 */
    ساکن صحیح
blob_اشاریہ(
	blob_T		*blob,
	صحیح		len,
	varnumber_T	idx,
	typval_T	*rettv)
{
    // The resulting variable is a byte value.
    // If the index is too big or negative that is an error.
    اگر (idx < 0)
	idx = len + idx;
    اگر (idx < len && idx >= 0)
    {
	صحیح v = blob_get(blob, idx);

	clear_tv(rettv);
	rettv->v_type = VAR_نمبر;
	rettv->vval.v_نمبر = v;
    }
    ورنہ
    {
	semsg(_(e_blob_اشاریہ_برآمد_کا_range_nr), idx);
	واپس FAIL;
    }

    واپس OK;
}

    صحیح
blob_قاش_or_اشاریہ(
	blob_T		*blob,
	صحیح		is_range,
	varnumber_T	n1,
	varnumber_T	n2,
	صحیح		exclusive,
	typval_T	*rettv)
{
    طویل	len = blob_len(blob);

    اگر (is_range)
	واپس blob_قاش(blob, len, n1, n2, exclusive, rettv);
    ورنہ
	واپس blob_اشاریہ(blob, len, n1, rettv);
    واپس OK;
}

/*
 * Check if "n1"- is a valid index for a blobl with length "bloblen".
 */
    صحیح
check_blob_اشاریہ(طویل bloblen, varnumber_T n1, صحیح quiet)
{
    اگر (n1 < 0 || n1 > bloblen)
    {
	اگر (!quiet)
	    semsg(_(e_blob_اشاریہ_برآمد_کا_range_nr), n1);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check if "n1"-"n2" is a valid range for a blob with length "bloblen".
 */
    صحیح
check_blob_range(طویل bloblen, varnumber_T n1, varnumber_T n2, صحیح quiet)
{
    اگر (n2 < 0 || n2 >= bloblen || n2 < n1)
    {
	اگر (!quiet)
	    semsg(_(e_blob_اشاریہ_برآمد_کا_range_nr), n2);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Set bytes "n1" to "n2" (inclusive) in "dest" to the value of "src".
 * Caller must make sure "src" is a blob.
 * Returns FAIL if the number of bytes does not match.
 */
    صحیح
blob_سیٹ_range(blob_T *dest, طویل n1, طویل n2, typval_T *src)
{
    صحیح	il, ir;

    اگر (n2 - n1 + 1 != blob_len(src->vval.v_blob))
    {
	emsg(_(e_blob_قدر_does_not_have_دایاں_نمبر_کا_بائٹیں));
	واپس FAIL;
    }

    ir = 0;
    برائے (il = n1; il <= n2; il++)
	blob_سیٹ(dest, il, blob_get(src->vval.v_blob, ir++));
    واپس OK;
}

/*
 * "add(blob, item)" function
 */
    خالی
blob_جوڑو(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    صحیح		error = FALSE;
    varnumber_T n;

    اگر (b == تہی)
    {
	اگر (اندر_vim9script())
	    emsg(_(e_cannot_جوڑو_مقصد_null_blob));
	واپس;
    }

    اگر (قدر_check_تالا(b->bv_تالا, (char_u *)N_("add() argument"), TRUE))
	واپس;

    n = tv_get_نمبر_chk(&argvars[1], &error);
    اگر (error)
	واپس;

    ga_append(&b->bv_ga, (صحیح)n);
    copy_tv(&argvars[0], rettv);
}

/*
 * "remove({blob}, {idx} [, {end}])" function
 */
    خالی
blob_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    blob_T	*b = argvars[0].vval.v_blob;
    blob_T	*newblob;
    صحیح		error = FALSE;
    طویل	idx;
    طویل	انتہا;
    صحیح		len;
    char_u	*p;

    اگر (b != تہی && قدر_check_تالا(b->bv_تالا, arg_errmsg, TRUE))
	واپس;

    idx = (طویل)tv_get_نمبر_chk(&argvars[1], &error);
    اگر (error)
	واپس;

    len = blob_len(b);

    اگر (idx < 0)
	// count from the end
	idx = len + idx;
    اگر (idx < 0 || idx >= len)
    {
	semsg(_(e_blob_اشاریہ_برآمد_کا_range_nr), idx);
	واپس;
    }
    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	p = (char_u *)b->bv_ga.ga_ڈیٹا;
	rettv->vval.v_نمبر = (varnumber_T) *(p + idx);
	mch_memmove(p + idx, p + idx + 1, (size_t)len - idx - 1);
	--b->bv_ga.ga_len;
	واپس;
    }

    // Remove range of items, return blob with values.
    انتہا = (طویل)tv_get_نمبر_chk(&argvars[2], &error);
    اگر (error)
	واپس;
    اگر (انتہا < 0)
	// count from the end
	انتہا = len + انتہا;
    اگر (انتہا >= len || idx > انتہا)
    {
	semsg(_(e_blob_اشاریہ_برآمد_کا_range_nr), انتہا);
	واپس;
    }
    newblob = blob_alloc();
    اگر (newblob == تہی)
	واپس;
    newblob->bv_ga.ga_len = انتہا - idx + 1;
    اگر (ga_grow(&newblob->bv_ga, انتہا - idx + 1) == FAIL)
    {
	vim_free(newblob);
	واپس;
    }
    p = (char_u *)b->bv_ga.ga_ڈیٹا;
    mch_memmove((char_u *)newblob->bv_ga.ga_ڈیٹا, p + idx,
	    (size_t)(انتہا - idx + 1));
    ++newblob->bv_refcount;
    rettv->v_type = VAR_BLOB;
    rettv->vval.v_blob = newblob;

    اگر (len - انتہا - 1 > 0)
	mch_memmove(p + idx, p + انتہا + 1, (size_t)(len - انتہا - 1));
    b->bv_ga.ga_len -= انتہا - idx + 1;
}

/*
 * Implementation of map() and filter() for a Blob.  Apply "expr" to every
 * number in Blob "blob_arg" and return the result in "rettv".
 */
    خالی
blob_filter_map(
	blob_T		*blob_arg,
	filtermap_T	filtermap,
	typval_T	*اظہاریہ,
	char_u		*arg_errmsg,
	typval_T	*rettv)
{
    blob_T	*b = blob_arg;
    صحیح		i;
    typval_T	tv;
    varnumber_T	val;
    blob_T	*b_ret;
    صحیح		idx = 0;
    صحیح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_BLOB;
	rettv->vval.v_blob = تہی;
    }
    اگر (b == تہی || (filtermap == FILTERMAP_FILTER
			    && قدر_check_تالا(b->bv_تالا, arg_errmsg, TRUE)))
	واپس;

    b_ret = b;
    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_تالا = 0;
	rettv->vval.v_blob = blob_copy(b);
	اگر (rettv->vval.v_blob == تہی)
	    واپس;
	b_ret = rettv->vval.v_blob;
    }

    // set_vim_var_nr() doesn't set the type
    سیٹ_vim_var_type(VV_کلید, VAR_نمبر);

    صحیح پچھلا_تالا = b->bv_تالا;
    اگر (b->bv_تالا == 0)
	b->bv_تالا = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اظہاریہ_get_funccal(اظہاریہ, &newtv);

    برائے (i = 0; i < b->bv_ga.ga_len; i++)
    {
	tv.v_type = VAR_نمبر;
	val = blob_get(b, i);
	tv.vval.v_نمبر = val;
	سیٹ_vim_var_nr(VV_کلید, idx);
	اگر (filter_map_one(&tv, اظہاریہ, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	    توڑو;
	اگر (filtermap != FILTERMAP_FOREACH)
	{
	    اگر (newtv.v_type != VAR_نمبر && newtv.v_type != VAR_BOOL)
	    {
		clear_tv(&newtv);
		emsg(_(e_invalid_operation_for_blob));
		توڑو;
	    }
	    اگر (filtermap != FILTERMAP_FILTER)
	    {
		اگر (newtv.vval.v_نمبر != val)
		    blob_سیٹ(b_ret, i, newtv.vval.v_نمبر);
	    }
	    ورنہ اگر (rem)
	    {
		char_u *p = (char_u *)blob_arg->bv_ga.ga_ڈیٹا;

		mch_memmove(p + i, p + i + 1,
			    (size_t)b->bv_ga.ga_len - i - 1);
		--b->bv_ga.ga_len;
		--i;
	    }
	}
	++idx;
    }

    b->bv_تالا = پچھلا_تالا;
    اگر (fc != تہی)
	remove_funccal();
}

/*
 * "insert(blob, {item} [, {idx}])" function
 */
    خالی
blob_داخل_کرو_func(typval_T *argvars, typval_T *rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    طویل	before = 0;
    صحیح		error = FALSE;
    صحیح		val, len;
    char_u	*p;

    اگر (b == تہی)
    {
	اگر (اندر_vim9script())
	    emsg(_(e_cannot_جوڑو_مقصد_null_blob));
	واپس;
    }

    اگر (قدر_check_تالا(b->bv_تالا, (char_u *)N_("insert() argument"), TRUE))
	واپس;

    len = blob_len(b);
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	before = (طویل)tv_get_نمبر_chk(&argvars[2], &error);
	اگر (error)
	    واپس;		// type error; errmsg already given
	اگر (before < 0 || before > len)
	{
	    semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[2]));
	    واپس;
	}
    }
    val = tv_get_نمبر_chk(&argvars[1], &error);
    اگر (error)
	واپس;
    اگر (val < 0 || val > 255)
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[1]));
	واپس;
    }

    اگر (ga_grow(&b->bv_ga, 1) == FAIL)
	واپس;
    p = (char_u *)b->bv_ga.ga_ڈیٹا;
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
    ساکن صحیح
blob_extend(blob_T *b1, blob_T *b2, طویل bef)
{
    صحیح		len1, len2;
    char_u	*p1;

    // NULL blob is equivalent to an empty blob: nothing to do.
    اگر (b2 == تہی || b2->bv_ga.ga_len == 0)
	واپس OK;

    len1 = b1->bv_ga.ga_len;
    len2 = b2->bv_ga.ga_len;

    اگر (ga_grow(&b1->bv_ga, len2) == FAIL)
	واپس FAIL;

    p1 = (char_u *)b1->bv_ga.ga_ڈیٹا;

    اگر (b1 == b2)
    {
	// Inserting a blob into itself
	mch_memmove(p1 + bef, p1, (size_t)len1);
	اگر (bef < len1)
	    حافظہ_نقل(p1 + bef + len1, p1 + bef * 2, (size_t)(len1 - bef));
    }
    ورنہ
    {
	اگر (bef < len1)
	    mch_memmove(p1 + bef + len2, p1 + bef, (size_t)(len1 - bef));

	حافظہ_نقل(p1 + bef, b2->bv_ga.ga_ڈیٹا, (size_t)len2);
    }

    b1->bv_ga.ga_len += len2;

    واپس OK;
}

/*
 * extend() a Blob. Append Blob argvars[1] to Blob argvars[0] before index
 * argvars[3] and return the resulting blob in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    خالی
blob_extend_func(
	typval_T	*argvars,
	char_u		*arg_errmsg,
	صحیح		is_new,
	typval_T	*rettv)
{
    blob_T	*b1, *b2;
    طویل	before;
    صحیح		error = FALSE;

    b1 = argvars[0].vval.v_blob;
    اگر (b1 == تہی)
    {
	emsg(_(e_cannot_extend_null_blob));
	واپس;
    }
    اگر (is_new || !قدر_check_تالا(b1->bv_تالا, arg_errmsg, TRUE))
    {
	اگر (is_new)
	{
	    b1 = blob_copy(b1);
	    اگر (b1 == تہی)
		واپس;
	}

	b2 = argvars[1].vval.v_blob;
	اگر (b2 == تہی)
	    جاؤ theend;

	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (طویل)tv_get_نمبر_chk(&argvars[2], &error);
	    اگر (error)
		جاؤ cleanup;		// type error; errmsg already given
	    اگر (before < 0)
		before = b1->bv_ga.ga_len + before;
	    اگر (before < 0 || before > b1->bv_ga.ga_len)
	    {
		semsg(_(e_blob_اشاریہ_برآمد_کا_range_nr), before);
		جاؤ cleanup;
	    }
	}
	ورنہ
	    before = b1->bv_ga.ga_len;
	اگر (blob_extend(b1, b2, before) == FAIL)
	    جاؤ cleanup;

theend:
	اگر (is_new)
	{
	    rettv->v_type = VAR_BLOB;
	    rettv->v_تالا = 0;
	    rettv->vval.v_blob = b1;
	}
	ورنہ
	    copy_tv(&argvars[0], rettv);
	واپس;

cleanup:
	اگر (is_new)
	    blob_unref(b1);
    }
}

/*
 * Implementation of reduce() for Blob "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    خالی
blob_reduce(
	typval_T	*argvars,
	typval_T	*اظہاریہ,
	typval_T	*rettv)
{
    blob_T	*b = argvars[0].vval.v_blob;
    صحیح		called_emsg_آغاز = called_emsg;
    صحیح		r;
    typval_T	initial;
    typval_T	argv[3];
    صحیح	i;

    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	اگر (b == تہی || b->bv_ga.ga_len == 0)
	{
	    semsg(_(e_reduce_کا_an_خالی_خانے_str_with_no_initial_قدر), "Blob");
	    واپس;
	}
	initial.v_type = VAR_نمبر;
	initial.vval.v_نمبر = blob_get(b, 0);
	i = 1;
    }
    ورنہ اگر (check_for_نمبر_arg(argvars, 2) == FAIL)
	واپس;
    ورنہ
    {
	initial = argvars[2];
	i = 0;
    }

    copy_tv(&initial, rettv);
    اگر (b == تہی)
	واپس;

    برائے ( ; i < b->bv_ga.ga_len; i++)
    {
	argv[0] = *rettv;
	argv[1].v_type = VAR_نمبر;
	argv[1].vval.v_نمبر = blob_get(b, i);

	r = eval_اظہاریہ_typval(اظہاریہ, TRUE, argv, 2, تہی, rettv);

	clear_tv(&argv[0]);
	اگر (r == FAIL || called_emsg != called_emsg_آغاز)
	    واپس;
    }
}

/*
 * "reverse({blob})" function
 */
    خالی
blob_الٹ(blob_T *b, typval_T *rettv)
{
    طویل    i, len = blob_len(b);

    برائے (i = 0; i < len / 2; i++)
    {
	صحیح tmp = blob_get(b, (صحیح)i);

	blob_سیٹ(b, (صحیح)i, blob_get(b, (صحیح)(len - i - 1)));
	blob_سیٹ(b, (صحیح)(len - i - 1), tmp);
    }
    rettv_blob_سیٹ(rettv, b);
}

/*
 * blob2list() function
 */
    خالی
f_blob2list(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*l;
    صحیح		i, len;
    typval_T	tv;

    اگر (check_for_blob_arg(argvars, 0) == FAIL)
    {
	(خالی)rettv_list_alloc(rettv);
	واپس;
    }

    blob = argvars->vval.v_blob;
    len = blob_len(blob);
    l = list_alloc_with_اشیاء(len);
    اگر (l == تہی)
	واپس;
    rettv_list_سیٹ(rettv, l);

    tv.v_type = VAR_نمبر;
    tv.v_تالا = 0;
    برائے (i = 0; i < len; i++)
    {
	tv.vval.v_نمبر = blob_get(blob, i);
	list_سیٹ_شے(l, i, &tv);
    }
}

/*
 * list2blob() function
 */
    خالی
f_list2blob(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    blob_T	*blob;

    اگر (rettv_blob_alloc(rettv) == FAIL)
	واپس;
    blob = rettv->vval.v_blob;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	واپس;

    l = argvars->vval.v_list;
    اگر (l == تہی)
	واپس;

    // The result has one byte per list item; reserve all space at once.
    اگر (ga_grow(&blob->bv_ga, list_len(l)) == FAIL)
	واپس;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اشیاء(l, li)
    {
	صحیح		error;
	varnumber_T	n;

	error = FALSE;
	n = tv_get_نمبر_chk(&li->li_tv, &error);
	اگر (error == TRUE || n < 0 || n > 255)
	{
	    اگر (!error)
		semsg(_(e_invalid_قدر_for_blob_nr), n);
	    ga_clear(&blob->bv_ga);
	    واپس;
	}
	((char_u *)blob->bv_ga.ga_ڈیٹا)[blob->bv_ga.ga_len++] = (char_u)n;
    }
}

#اگر_ختم // defined(FEAT_EVAL)
