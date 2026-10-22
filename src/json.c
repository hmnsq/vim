/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * json.c: Encoding and decoding JSON.
 *
 * Follows this standard: https://tools.ietf.org/html/rfc7159.html
 */
#تعریف_کن USING_FLOAT_STUFF

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

ساكن صحيح json_encode_قلم(garray_T *gap, typval_T *val, صحيح copyID, صحيح options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    ساكن صحيح
json_encode_gap(garray_T *gap, typval_T *val, صحيح options)
{
    إذا (json_encode_قلم(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_داده = vim_strsave((char_u *)"");
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, صحيح options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    إذا (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    أرجع ga.ga_داده;
}

#إذا defined(FEAT_کار_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_عبارت(صحيح nr, typval_T *val, صحيح options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_عدد;
    nrtv.vval.v_عدد = nr;
    إذا (rettv_list_alloc(&listtv) == FAIL)
	أرجع عدم;
    إذا (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	أرجع عدم;
    }

    ga_init2(&ga, 1, 4000);
    إذا (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    أرجع ga.ga_داده;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_پیام(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    إذا (json_encode_gap(&ga, val, 0) == FAIL)
	أرجع عدم;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_داده, ga.ga_len);
    ga_clear(&ga);
    أرجع lspga.ga_داده;
}
#پایان_اگر

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
ساكن ثابت حرف ascii_needs_escape[128] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0x0.
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0x1.
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x2.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x3.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x4.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, // 0x5.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x6.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x7.
};

/*
 * Encode the utf-8 encoded string "str" into "gap".
 */
    ساكن فراغ
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#إذا defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = عدم;
#پایان_اگر
    صحيح		c;

    إذا (res == عدم)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	أرجع;
    }

#إذا defined(USE_ICONV)
    إذا (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_راهاندازی(&conv, p_enc, (char_u*)"utf-8");
	إذا (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, عدم);
	convert_راهاندازی(&conv, عدم, عدم);
    }
#پایان_اگر
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (صحيح)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    طالما ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	إذا (c < 0x80)
	{
	    إذا (!ascii_needs_escape[c])
	    {
		res += 1;
		استمر;
	    }

	    إذا (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    بدل (c)
	    {
		حالة 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); اكسر;
		حالة 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); اكسر;
		حالة 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); اكسر;
		حالة 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); اكسر;
		حالة 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); اكسر;
		حالة 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); اكسر;
		حالة 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); اكسر;
		افتراضي:
		{
		    char_u escaped[] = "\\u0000";

		    // Only control characters below 0x20 reach this case.
		    escaped[4] = '0' + (c >> 4);
		    escaped[5] = "0123456789abcdef"[c & 0xf];
		    ga_concat_len(gap, escaped, STRLEN_LITERAL("\\u0000"));
		}
	    }

	    res += 1;
	}
	وإلا
	{
	    صحيح	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    إذا (l > 1)
	    {
		res += l;
		استمر;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    إذا (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    إذا (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#إذا defined(USE_ICONV)
    vim_free(converted);
#پایان_اگر
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    ساكن صحيح
is_simple_کلید(char_u *کلید)
{
    char_u *p;

    إذا (!ASCII_ISALPHA(*کلید))
	أرجع FALSE;
    لكل (p = کلید + 1; *p != NUL; ++p)
	إذا (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    أرجع FALSE;
    أرجع TRUE;
}

تعريفنوع تعداد {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

تعريفنوع بنية {
    json_enc_type_T je_type;
    صحيح		    je_options;	    // options when this container was entered
    اتحاد {
	بنية {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	بنية {
	    tuple_T	*tuple;
	    صحيح		idx;	    // current index
	    صحيح		len;
	} t;
	بنية {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    صحيح		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_قاب_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    ساكن صحيح
json_encode_قلم(garray_T *gap, typval_T *val, صحيح copyID, صحيح options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    صحيح			i;
    garray_T		پشته;
    typval_T		*cur_val;
    json_enc_قاب_T	*قاب;

    ga_init2(&پشته, حجم(json_enc_قاب_T), 100);
    cur_val = val;

    لكل (;;)
    {
    بدل (cur_val->v_type)
    {
	حالة VAR_BOOL:
	    بدل ((دراز)cur_val->vval.v_عدد)
	    {
		حالة VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); اكسر;
		حالة VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); اكسر;
	    }
	    اكسر;

	حالة VAR_SPECIAL:
	    بدل ((دراز)cur_val->vval.v_عدد)
	    {
		حالة VVAL_NONE: إذا ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    اكسر;
				// FALLTHROUGH
		حالة VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); اكسر;
	    }
	    اكسر;

	حالة VAR_عدد:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((حرف *)numbuf, حجم(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_عدد);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    اكسر;

	حالة VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    اكسر;

	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	حالة VAR_کار:
	حالة VAR_CHANNEL:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	حالة VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    اذهب theend;

	حالة VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    إذا (b == عدم || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    وإلا
	    {
		صحيح	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		إذا (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    اذهب theend;
		src = (char_u *)b->bv_ga.ga_داده;
		dst = (char_u *)gap->ga_داده + gap->ga_len;
		*dst++ = '[';
		لكل (i = 0; i < blen; i++)
		{
		    صحيح	    byte = src[i];

		    إذا (i > 0)
			*dst++ = ',';
		    إذا (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    وإلا إذا (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    وإلا
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (صحيح)(dst - (char_u *)gap->ga_داده);
	    }
	    اكسر;

	حالة VAR_LIST:
	    l = cur_val->vval.v_list;
	    إذا (l == عدم)
		GA_CONCAT_LITERAL(gap, "[]");
	    وإلا
	    {
		إذا (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		وإلا
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    إذا (l->lv_اولی != عدم)
		    {
			إذا (پشته.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_عمق_is_higher_than_maxfuncdepth));
			    اذهب theend;
			}
			إذا (ga_grow(&پشته, 1) == FAIL)
			    اذهب theend;
			قاب = ((json_enc_قاب_T *)پشته.ga_داده)
							    + پشته.ga_len;
			قاب->je_type = ENC_LIST;
			قاب->je_options = options;
			قاب->je_u.l.list = l;
			قاب->je_u.l.li = l->lv_اولی;
			++پشته.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_اولی->li_tv;
			استمر;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    اكسر;

	حالة VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    إذا (tuple == عدم)
		GA_CONCAT_LITERAL(gap, "[]");
	    وإلا
	    {
		إذا (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		وإلا
		{
		    صحيح		len = TUPLE_LEN(tuple);

		    إذا (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			إذا (پشته.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_عمق_is_higher_than_maxfuncdepth));
			    اذهب theend;
			}
			إذا (ga_grow(&پشته, 1) == FAIL)
			    اذهب theend;
			قاب = ((json_enc_قاب_T *)پشته.ga_داده)
							    + پشته.ga_len;
			قاب->je_type = ENC_TUPLE;
			قاب->je_options = options;
			قاب->je_u.t.tuple = tuple;
			قاب->je_u.t.idx = 0;
			قاب->je_u.t.len = len;
			++پشته.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_قلم(tuple, 0);
			استمر;
		    }
		    وإلا
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    اكسر;

	حالة VAR_DICT:
	    d = cur_val->vval.v_dict;
	    إذا (d == عدم)
		GA_CONCAT_LITERAL(gap, "{}");
	    وإلا
	    {
		إذا (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		وإلا
		{
		    صحيح		todo = (صحيح)d->dv_hashtab.ht_استفاده_شده;
		    hashitem_T	*hi;

		    إذا (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			لكل (hi = d->dv_hashtab.ht_array;
						  HASHITEM_خالی(hi); ++hi)
			    ;
			--todo;

			// Write first key
			إذا ((options & JSON_JS)
						 && is_simple_کلید(hi->hi_کلید))
			    ga_concat(gap, hi->hi_کلید);
			وإلا
			    write_string(gap, hi->hi_کلید);
			ga_append(gap, ':');

			إذا (پشته.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_عمق_is_higher_than_maxfuncdepth));
			    اذهب theend;
			}
			إذا (ga_grow(&پشته, 1) == FAIL)
			    اذهب theend;
			قاب = ((json_enc_قاب_T *)پشته.ga_داده)
							    + پشته.ga_len;
			قاب->je_type = ENC_DICT;
			قاب->je_options = options;
			قاب->je_u.d.dict = d;
			قاب->je_u.d.hi = hi;
			قاب->je_u.d.todo = todo;
			++پشته.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			استمر;
		    }
		    وإلا
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    اكسر;

	حالة VAR_FLOAT:
#إذا defined(HAVE_MATH_H)
	    إذا (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    وإلا إذا (isinf(cur_val->vval.v_float))
	    {
		إذا (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		وإلا
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    وإلا
#پایان_اگر
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((حرف *)numbuf, حجم(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    اكسر;

	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	    داخلی_error_no_abort("json_encode_item()");
	    اذهب theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	لكل (;;)
	{
	    صحيح		advance = FALSE;

	    إذا (پشته.ga_len == 0)
	    {
		ga_clear(&پشته);
		أرجع OK;
	    }

	    قاب = ((json_enc_قاب_T *)پشته.ga_داده)
							+ پشته.ga_len - 1;
	    بدل (قاب->je_type)
	    {
		حالة ENC_LIST:
		{
		    listitem_T	*li = قاب->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    إذا ((قاب->je_options & JSON_JS)
			    && li->li_بعدی == عدم
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_عدد == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_بعدی;
		    قاب->je_u.l.li = li;
		    إذا (li != عدم && !got_int)
		    {
			ga_append(gap, ',');
			options = قاب->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			اكسر;
		    }
		    ga_append(gap, ']');
		    قاب->je_u.l.list->lv_copyID = 0;
		    --پشته.ga_len;
		    اكسر;
		}

		حالة ENC_TUPLE:
		{
		    صحيح	    idx = قاب->je_u.t.idx;
		    صحيح	    len = قاب->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    إذا ((قاب->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_قلم =
				    TUPLE_قلم(قاب->je_u.t.tuple, idx);

			إذا (t_قلم->v_type == VAR_SPECIAL
				&& t_قلم->vval.v_عدد == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    قاب->je_u.t.idx = idx;
		    إذا (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = قاب->je_options & JSON_JS;
			cur_val = TUPLE_قلم(قاب->je_u.t.tuple, idx);
			advance = TRUE;
			اكسر;
		    }
		    ga_append(gap, ']');
		    قاب->je_u.t.tuple->tv_copyID = 0;
		    --پشته.ga_len;
		    اكسر;
		}

		حالة ENC_DICT:
		{
		    hashitem_T	*hi = قاب->je_u.d.hi;
		    صحيح		todo = قاب->je_u.d.todo;

		    إذا (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			لكل (++hi; HASHITEM_خالی(hi); ++hi)
			    ;
			--todo;
			قاب->je_u.d.hi = hi;
			قاب->je_u.d.todo = todo;

			ga_append(gap, ',');
			إذا ((قاب->je_options & JSON_JS)
				&& is_simple_کلید(hi->hi_کلید))
			    ga_concat(gap, hi->hi_کلید);
			وإلا
			    write_string(gap, hi->hi_کلید);
			ga_append(gap, ':');

			options = قاب->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			اكسر;
		    }
		    ga_append(gap, '}');
		    قاب->je_u.d.dict->dv_copyID = 0;
		    --پشته.ga_len;
		    اكسر;
		}
	    }
	    إذا (advance)
		اكسر;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    لكل (i = 0; i < پشته.ga_len; i++)
    {
	قاب = ((json_enc_قاب_T *)پشته.ga_داده) + i;
	بدل (قاب->je_type)
	{
	    حالة ENC_LIST:
		قاب->je_u.l.list->lv_copyID = 0;
		اكسر;
	    حالة ENC_TUPLE:
		قاب->je_u.t.tuple->tv_copyID = 0;
		اكسر;
	    حالة ENC_DICT:
		قاب->je_u.d.dict->dv_copyID = 0;
		اكسر;
	}
    }
    ga_clear(&پشته);
    أرجع FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    ساكن فراغ
fill_numbuflen(js_read_T *reader)
{
    إذا (reader->js_fill != عدم && (صحيح)(reader->js_انتها - reader->js_buf)
						- reader->js_استفاده_شده < NUMBUFLEN)
    {
	إذا (reader->js_fill(reader))
	    reader->js_انتها = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    ساكن فراغ
json_skip_white(js_read_T *reader)
{
    صحيح c;

    لكل (;;)
    {
	c = reader->js_buf[reader->js_استفاده_شده];
	إذا (reader->js_fill != عدم && c == NUL)
	{
	    إذا (reader->js_fill(reader))
	    {
		reader->js_انتها = reader->js_buf + STRLEN(reader->js_buf);
		استمر;
	    }
	}
	إذا (c == NUL || c > ' ')
	    اكسر;
	++reader->js_استفاده_شده;
    }
    fill_numbuflen(reader);
}

    ساكن صحيح
json_decode_string(js_read_T *reader, typval_T *res, صحيح quote)
{
    garray_T    ga;
    صحيح		len;
    char_u	*p;
    صحيح		c;
    varnumber_T	nr;

    إذا (res != عدم)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_استفاده_شده + 1; // skip over " or '
    طالما (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	إذا (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    إذا (reader->js_fill == عدم)
		اكسر;
	    len = (صحيح)(reader->js_انتها - p);
	    reader->js_استفاده_شده = (صحيح)(p - reader->js_buf);
	    إذا (!reader->js_fill(reader))
		اكسر; // didn't get more
	    p = reader->js_buf + reader->js_استفاده_شده;
	    reader->js_انتها = reader->js_buf + STRLEN(reader->js_buf);
	    استمر;
	}

	إذا (*p == '\\')
	{
	    c = -1;
	    بدل (p[1])
	    {
		حالة '\\': c = '\\'; اكسر;
		حالة '"': c = '"'; اكسر;
		حالة 'b': c = BS; اكسر;
		حالة 't': c = TAB; اكسر;
		حالة 'n': c = NL; اكسر;
		حالة 'f': c = FF; اكسر;
		حالة 'r': c = CAR; اكسر;
		حالة 'u':
		    إذا (reader->js_fill != عدم
				     && (صحيح)(reader->js_انتها - p) < NUMBUFLEN)
		    {
			reader->js_استفاده_شده = (صحيح)(p - reader->js_buf);
			إذا (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_استفاده_شده;
			    reader->js_انتها = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, عدم, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, عدم, 4, TRUE, عدم);
		    إذا (len == 0)
		    {
			إذا (res != عدم)
			    ga_clear(&ga);
			أرجع FAIL;
		    }
		    p += len + 2;
		    إذا (0xd800 <= nr && nr <= 0xdbff
			    && (صحيح)(reader->js_انتها - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, عدم, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, عدم, 4, TRUE, عدم);
			إذا (len == 0)
			{
			    إذا (res != عدم)
				ga_clear(&ga);
			    أرجع FAIL;
			}
			إذا (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    إذا (0xd800 <= nr && nr <= 0xdfff)
		    {
			إذا (res != عدم)
			    ga_clear(&ga);
			أرجع FAIL;
		    }
		    إذا (res != عدم)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((صحيح)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    اكسر;
		افتراضي:
		    // not a special char, skip over backslash
		    ++p;
		    استمر;
	    }
	    إذا (c > 0)
	    {
		p += 2;
		إذا (res != عدم)
		    ga_append(&ga, c);
	    }
	}
	وإلا
	{
	    len = utf_ptr2len(p);
	    إذا (res != عدم)
	    {
		إذا (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    أرجع FAIL;
		}
		mch_memmove((حرف *)ga.ga_داده + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_استفاده_شده = (صحيح)(p - reader->js_buf);
    إذا (*p == quote)
    {
	++reader->js_استفاده_شده;
	إذا (res != عدم)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#إذا defined(USE_ICONV)
	    إذا (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_راهاندازی(&conv, (char_u*)"utf-8", p_enc);
		إذا (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_داده, عدم);
		    vim_free(ga.ga_داده);
		}
		convert_راهاندازی(&conv, عدم, عدم);
	    }
	    وإلا
#پایان_اگر
		res->vval.v_string = ga.ga_داده;
	}
	أرجع OK;
    }
    إذا (res != عدم)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_عدد = VVAL_NONE;
	ga_clear(&ga);
    }
    أرجع MAYBE;
}

تعريفنوع تعداد {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_کلید,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

تعريفنوع بنية {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_کلید_tv;
    char_u	  *jd_کلید;
} json_dec_قلم_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    ساكن صحيح
json_decode_قلم(js_read_T *reader, typval_T *res, صحيح options)
{
    char_u	*p;
    صحيح		i;
    صحيح		len;
    صحيح		retval;
    garray_T	پشته;
    typval_T	قلم;
    typval_T	*cur_قلم;
    json_dec_قلم_T *بالاترین_قلم;
    char_u	کلید_buf[NUMBUFLEN];

    ga_init2(&پشته, حجم(json_dec_قلم_T), 100);
    cur_قلم = res;
    init_tv(&قلم);
    إذا (res != عدم)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_استفاده_شده;
    لكل (;;)
    {
	بالاترین_قلم = عدم;
	إذا (پشته.ga_len > 0)
	{
	    بالاترین_قلم = ((json_dec_قلم_T *)پشته.ga_داده) + پشته.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_استفاده_شده;
	    إذا (*p == NUL)
	    {
		retval = MAYBE;
		اذهب theend;
	    }
	    إذا (بالاترین_قلم->jd_type == JSON_OBJECT_کلید
					    || بالاترین_قلم->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		إذا (*p == (بالاترین_قلم->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_استفاده_شده; // consume the ']' or '}'
		    --پشته.ga_len;
		    إذا (پشته.ga_len == 0)
		    {
			retval = OK;
			اذهب theend;
		    }
		    إذا (cur_قلم != عدم)
			cur_قلم = &بالاترین_قلم->jd_tv;
		    اذهب قلم_انتها;
		}
	    }
	}

	إذا (بالاترین_قلم != عدم && بالاترین_قلم->jd_type == JSON_OBJECT_کلید
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_استفاده_شده] != '"'
		&& reader->js_buf[reader->js_استفاده_شده] != '\''
		&& reader->js_buf[reader->js_استفاده_شده] != '['
		&& reader->js_buf[reader->js_استفاده_شده] != '{')
	{
	    char_u *کلید;

	    // accept an object key that is not in quotes
	    کلید = p = reader->js_buf + reader->js_استفاده_شده;
	    طالما (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    إذا (cur_قلم != عدم)
	    {
		cur_قلم->v_type = VAR_STRING;
		cur_قلم->vval.v_string = vim_strnsave(کلید, p - کلید);
		بالاترین_قلم->jd_کلید = cur_قلم->vval.v_string;
	    }
	    reader->js_استفاده_شده += (صحيح)(p - کلید);
	}
	وإلا
	{
	    بدل (*p)
	    {
		حالة '[': // start of array
		    إذا (بالاترین_قلم && بالاترین_قلم->jd_type == JSON_OBJECT_کلید)
		    {
			retval = FAIL;
			اكسر;
		    }
		    إذا (پشته.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_عمق_is_higher_than_maxfuncdepth));
			retval = FAIL;
			اكسر;
		    }
		    إذا (ga_grow(&پشته, 1) == FAIL)
		    {
			retval = FAIL;
			اكسر;
		    }
		    إذا (cur_قلم != عدم && rettv_list_alloc(cur_قلم) == FAIL)
		    {
			cur_قلم->v_type = VAR_SPECIAL;
			cur_قلم->vval.v_عدد = VVAL_NONE;
			retval = FAIL;
			اكسر;
		    }

		    ++reader->js_استفاده_شده; // consume the '['
		    بالاترین_قلم = ((json_dec_قلم_T *)پشته.ga_داده)
								+ پشته.ga_len;
		    بالاترین_قلم->jd_type = JSON_ARRAY;
		    ++پشته.ga_len;
		    إذا (cur_قلم != عدم)
		    {
			بالاترین_قلم->jd_tv = *cur_قلم;
			cur_قلم = &قلم;
		    }
		    استمر;

		حالة '{': // start of object
		    إذا (بالاترین_قلم && بالاترین_قلم->jd_type == JSON_OBJECT_کلید)
		    {
			retval = FAIL;
			اكسر;
		    }
		    إذا (پشته.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_عمق_is_higher_than_maxfuncdepth));
			retval = FAIL;
			اكسر;
		    }
		    إذا (ga_grow(&پشته, 1) == FAIL)
		    {
			retval = FAIL;
			اكسر;
		    }
		    إذا (cur_قلم != عدم && rettv_dict_alloc(cur_قلم) == FAIL)
		    {
			cur_قلم->v_type = VAR_SPECIAL;
			cur_قلم->vval.v_عدد = VVAL_NONE;
			retval = FAIL;
			اكسر;
		    }

		    ++reader->js_استفاده_شده; // consume the '{'
		    بالاترین_قلم = ((json_dec_قلم_T *)پشته.ga_داده)
								+ پشته.ga_len;
		    بالاترین_قلم->jd_type = JSON_OBJECT_کلید;
		    ++پشته.ga_len;
		    إذا (cur_قلم != عدم)
		    {
			بالاترین_قلم->jd_tv = *cur_قلم;
			cur_قلم = &بالاترین_قلم->jd_کلید_tv;
		    }
		    استمر;

		حالة '"': // string
		    retval = json_decode_string(reader, cur_قلم, *p);
		    اكسر;

		حالة '\'':
		    إذا (options & JSON_JS)
			retval = json_decode_string(reader, cur_قلم, *p);
		    وإلا
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    اكسر;

		حالة ',': // comma: empty item
		    إذا ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			اكسر;
		    }
		    // FALLTHROUGH
		حالة NUL: //خالی
		    إذا (cur_قلم != عدم)
		    {
			cur_قلم->v_type = VAR_SPECIAL;
			cur_قلم->vval.v_عدد = VVAL_NONE;
		    }
		    retval = OK;
		    اكسر;

		افتراضي:
		    إذا (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			إذا (*sp == '-')
			{
			    ++sp;
			    إذا (*sp == NUL)
			    {
				retval = MAYBE;
				اكسر;
			    }
			    إذا (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				اكسر;
			    }
			}
			sp = skipdigits(sp);
			إذا (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    إذا (cur_قلم == عدم)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    وإلا
			    {
				cur_قلم->v_type = VAR_FLOAT;
				len = string2float(p, &cur_قلم->vval.v_float,
									FALSE);
			    }
			}
			وإلا
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_استفاده_شده,
				    عدم, &len, 0, // what
				    &nr, عدم, 0, TRUE, عدم);
			    إذا (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				اذهب theend;
			    }
			    إذا (cur_قلم != عدم)
			    {
				cur_قلم->v_type = VAR_عدد;
				cur_قلم->vval.v_عدد = nr;
			    }
			}
			reader->js_استفاده_شده += len;
			retval = OK;
			اكسر;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#تعریف_کن تطابق_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((حرف *)(p), (kw), (len)) == 0 \
     : STRNCMP((حرف *)(p), (kw), (len)) == 0)
		    إذا (تطابق_KW(p, "false", 5))
		    {
			reader->js_استفاده_شده += 5;
			إذا (cur_قلم != عدم)
			{
			    cur_قلم->v_type = VAR_BOOL;
			    cur_قلم->vval.v_عدد = VVAL_FALSE;
			}
			retval = OK;
			اكسر;
		    }
		    إذا (تطابق_KW(p, "true", 4))
		    {
			reader->js_استفاده_شده += 4;
			إذا (cur_قلم != عدم)
			{
			    cur_قلم->v_type = VAR_BOOL;
			    cur_قلم->vval.v_عدد = VVAL_TRUE;
			}
			retval = OK;
			اكسر;
		    }
		    إذا (تطابق_KW(p, "null", 4))
		    {
			reader->js_استفاده_شده += 4;
			إذا (cur_قلم != عدم)
			{
			    cur_قلم->v_type = VAR_SPECIAL;
			    cur_قلم->vval.v_عدد = VVAL_NULL;
			}
			retval = OK;
			اكسر;
		    }
		    إذا (تطابق_KW(p, "NaN", 3))
		    {
			reader->js_استفاده_شده += 3;
			إذا (cur_قلم != عدم)
			{
			    cur_قلم->v_type = VAR_FLOAT;
			    cur_قلم->vval.v_float = NAN;
			}
			retval = OK;
			اكسر;
		    }
		    إذا (تطابق_KW(p, "-Infinity", 9))
		    {
			reader->js_استفاده_شده += 9;
			إذا (cur_قلم != عدم)
			{
			    cur_قلم->v_type = VAR_FLOAT;
			    cur_قلم->vval.v_float = -INFINITY;
			}
			retval = OK;
			اكسر;
		    }
		    إذا (تطابق_KW(p, "Infinity", 8))
		    {
			reader->js_استفاده_شده += 8;
			إذا (cur_قلم != عدم)
			{
			    cur_قلم->v_type = VAR_FLOAT;
			    cur_قلم->vval.v_float = INFINITY;
			}
			retval = OK;
			اكسر;
		    }
#لغو_تعریف تطابق_KW
		    // check for truncated name
		    len = (صحيح)(reader->js_انتها
					 - (reader->js_buf + reader->js_استفاده_شده));
		    إذا (
			    (len < 5 && STRNICMP((حرف *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((حرف *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((حرف *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((حرف *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((حرف *)p, "true", len) == 0
				    || STRNICMP((حرف *)p, "null", len) == 0)))

			retval = MAYBE;
		    وإلا
			retval = FAIL;
		    اكسر;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    إذا (retval == FAIL)
		اكسر;
	    إذا (retval == MAYBE || پشته.ga_len == 0)
		اذهب theend;

	    إذا (بالاترین_قلم != عدم && بالاترین_قلم->jd_type == JSON_OBJECT_کلید
		    && cur_قلم != عدم)
	    {
		إذا (cur_قلم->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    اذهب theend;
		}
		بالاترین_قلم->jd_کلید = tv_get_string_buf_chk(cur_قلم, کلید_buf);
		إذا (بالاترین_قلم->jd_کلید == عدم)
		{
		    emsg(_(e_invalid_آرگومان));
		    retval = FAIL;
		    اذهب theend;
		}
	    }
	}

قلم_انتها:
	بالاترین_قلم = ((json_dec_قلم_T *)پشته.ga_داده) + پشته.ga_len - 1;
	بدل (بالاترین_قلم->jd_type)
	{
	    حالة JSON_ARRAY:
		إذا (res != عدم)
		{
		    listitem_T	*li = listitem_alloc();

		    إذا (li == عدم)
		    {
			clear_tv(cur_قلم);
			retval = FAIL;
			اذهب theend;
		    }
		    li->li_tv = *cur_قلم;
		    list_append(بالاترین_قلم->jd_tv.vval.v_list, li);
		}
		إذا (cur_قلم != عدم)
		    cur_قلم = &قلم;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_استفاده_شده;
		إذا (*p == ',')
		    ++reader->js_استفاده_شده;
		وإلا إذا (*p != ']')
		{
		    إذا (*p == NUL)
			retval = MAYBE;
		    وإلا
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    اذهب theend;
		}
		اكسر;

	    حالة JSON_OBJECT_کلید:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_استفاده_شده;
		إذا (*p != ':')
		{
		    إذا (cur_قلم != عدم)
			clear_tv(cur_قلم);
		    إذا (*p == NUL)
			retval = MAYBE;
		    وإلا
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    اذهب theend;
		}
		++reader->js_استفاده_شده;
		json_skip_white(reader);
		بالاترین_قلم->jd_type = JSON_OBJECT;
		إذا (cur_قلم != عدم)
		    cur_قلم = &قلم;
		اكسر;

	    حالة JSON_OBJECT:
		إذا (cur_قلم != عدم
			&& dict_has_کلید(بالاترین_قلم->jd_tv.vval.v_dict,
						(حرف *)بالاترین_قلم->jd_کلید))
		{
		    semsg(_(e_duplicate_کلید_درون_json_str), بالاترین_قلم->jd_کلید);
		    clear_tv(cur_قلم);
		    retval = FAIL;
		    اذهب theend;
		}

		إذا (cur_قلم != عدم)
		{
		    dictitem_T *di = dictitem_alloc(بالاترین_قلم->jd_کلید);

		    clear_tv(&بالاترین_قلم->jd_کلید_tv);
		    إذا (di == عدم)
		    {
			clear_tv(cur_قلم);
			retval = FAIL;
			اذهب theend;
		    }
		    di->di_tv = *cur_قلم;
		    di->di_tv.v_قفل = 0;
		    إذا (dict_جمع_کن(بالاترین_قلم->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			اذهب theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_استفاده_شده;
		إذا (*p == ',')
		    ++reader->js_استفاده_شده;
		وإلا إذا (*p != '}')
		{
		    إذا (*p == NUL)
			retval = MAYBE;
		    وإلا
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    اذهب theend;
		}
		بالاترین_قلم->jd_type = JSON_OBJECT_کلید;
		إذا (cur_قلم != عدم)
		    cur_قلم = &بالاترین_قلم->jd_کلید_tv;
		اكسر;
	}
    }

    // Get here when parsing failed.
    إذا (res != عدم)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_عدد = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_استفاده_شده);

theend:
    لكل (i = 0; i < پشته.ga_len; i++)
	clear_tv(&(((json_dec_قلم_T *)پشته.ga_داده) + i)->jd_کلید_tv);
    ga_clear(&پشته);

    أرجع retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    ساكن صحيح
json_decode_all(js_read_T *reader, typval_T *res, صحيح options)
{
    صحيح ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_انتها = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_قلم(reader, res, options);
    إذا (ret != OK)
    {
	إذا (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	أرجع FAIL;
    }
    json_skip_white(reader);
    إذا (reader->js_buf[reader->js_استفاده_شده] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_استفاده_شده);
	أرجع FAIL;
    }
    أرجع OK;
}

#إذا defined(FEAT_کار_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    صحيح
json_decode(js_read_T *reader, typval_T *res, صحيح options)
{
    صحيح ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_انتها = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_قلم(reader, res, options);
    json_skip_white(reader);

    أرجع ret;
}
#پایان_اگر

/*
 * Decode the JSON from "reader" to find the end of the message.
 * "options" can be JSON_JS or zero.
 * This is only used for testing.
 * Return FAIL if the message has a decoding error.
 * Return MAYBE if the message is truncated, need to read more.
 * This only works reliable if the message contains an object, array or
 * string.  A number might be truncated without knowing.
 * Does not advance the reader.
 */
    صحيح
json_پیدا_کن_انتها(js_read_T *reader, صحيح options)
{
    صحيح استفاده_شده_save = reader->js_استفاده_شده;
    صحيح ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_انتها = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_قلم(reader, عدم, options);
    reader->js_استفاده_شده = استفاده_شده_save;
    أرجع ret;
}

/*
 * "js_decode()" function
 */
    فراغ
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = عدم;
    reader.js_استفاده_شده = 0;
    إذا (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_آرگومان));
}

/*
 * "js_encode()" function
 */
    فراغ
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    فراغ
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = عدم;
    reader.js_استفاده_شده = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    فراغ
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#پایان_اگر
