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
#تعریف_کړه USING_FLOAT_STUFF

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

ساكن صحيح json_encode_توکی(garray_T *gap, typval_T *val, صحيح copyID, صحيح options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    ساكن صحيح
json_encode_gap(garray_T *gap, typval_T *val, صحيح options)
{
    إذا (json_encode_توکی(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_معلومات = vim_strsave((char_u *)"");
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
    أرجع ga.ga_معلومات;
}

#إذا defined(FEAT_دنده_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_بیان(صحيح nr, typval_T *val, صحيح options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_شمېره;
    nrtv.vval.v_شمېره = nr;
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
    أرجع ga.ga_معلومات;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_پیغام(typval_T *val)
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
    ga_concat_len(&lspga, ga.ga_معلومات, ga.ga_len);
    ga_clear(&ga);
    أرجع lspga.ga_معلومات;
}
#د_که_پای

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
#د_که_پای
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
	convert_تیاری(&conv, p_enc, (char_u*)"utf-8");
	إذا (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, عدم);
	convert_تیاری(&conv, عدم, عدم);
    }
#د_که_پای
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
#د_که_پای
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    ساكن صحيح
is_simple_کلی(char_u *کلی)
{
    char_u *p;

    إذا (!ASCII_ISALPHA(*کلی))
	أرجع FALSE;
    لكل (p = کلی + 1; *p != NUL; ++p)
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
} json_enc_چوکاټ_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    ساكن صحيح
json_encode_توکی(garray_T *gap, typval_T *val, صحيح copyID, صحيح options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    صحيح			i;
    garray_T		ډېری;
    typval_T		*cur_val;
    json_enc_چوکاټ_T	*چوکاټ;

    ga_init2(&ډېری, حجم(json_enc_چوکاټ_T), 100);
    cur_val = val;

    لكل (;;)
    {
    بدل (cur_val->v_type)
    {
	حالة VAR_BOOL:
	    بدل ((اوږد)cur_val->vval.v_شمېره)
	    {
		حالة VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); اكسر;
		حالة VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); اكسر;
	    }
	    اكسر;

	حالة VAR_SPECIAL:
	    بدل ((اوږد)cur_val->vval.v_شمېره)
	    {
		حالة VVAL_NONE: إذا ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    اكسر;
				// FALLTHROUGH
		حالة VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); اكسر;
	    }
	    اكسر;

	حالة VAR_شمېره:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((حرف *)numbuf, حجم(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_شمېره);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    اكسر;

	حالة VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    اكسر;

	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	حالة VAR_دنده:
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
		src = (char_u *)b->bv_ga.ga_معلومات;
		dst = (char_u *)gap->ga_معلومات + gap->ga_len;
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
		gap->ga_len = (صحيح)(dst - (char_u *)gap->ga_معلومات);
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
		    إذا (l->lv_لومړی != عدم)
		    {
			إذا (ډېری.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_ژوروالی_is_higher_than_maxfuncdepth));
			    اذهب theend;
			}
			إذا (ga_grow(&ډېری, 1) == FAIL)
			    اذهب theend;
			چوکاټ = ((json_enc_چوکاټ_T *)ډېری.ga_معلومات)
							    + ډېری.ga_len;
			چوکاټ->je_type = ENC_LIST;
			چوکاټ->je_options = options;
			چوکاټ->je_u.l.list = l;
			چوکاټ->je_u.l.li = l->lv_لومړی;
			++ډېری.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_لومړی->li_tv;
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
			إذا (ډېری.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_ژوروالی_is_higher_than_maxfuncdepth));
			    اذهب theend;
			}
			إذا (ga_grow(&ډېری, 1) == FAIL)
			    اذهب theend;
			چوکاټ = ((json_enc_چوکاټ_T *)ډېری.ga_معلومات)
							    + ډېری.ga_len;
			چوکاټ->je_type = ENC_TUPLE;
			چوکاټ->je_options = options;
			چوکاټ->je_u.t.tuple = tuple;
			چوکاټ->je_u.t.idx = 0;
			چوکاټ->je_u.t.len = len;
			++ډېری.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_توکی(tuple, 0);
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
		    صحيح		todo = (صحيح)d->dv_hashtab.ht_کارول_شوی;
		    hashitem_T	*hi;

		    إذا (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			لكل (hi = d->dv_hashtab.ht_array;
						  HASHITEM_تشه(hi); ++hi)
			    ;
			--todo;

			// Write first key
			إذا ((options & JSON_JS)
						 && is_simple_کلی(hi->hi_کلی))
			    ga_concat(gap, hi->hi_کلی);
			وإلا
			    write_string(gap, hi->hi_کلی);
			ga_append(gap, ':');

			إذا (ډېری.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_ژوروالی_is_higher_than_maxfuncdepth));
			    اذهب theend;
			}
			إذا (ga_grow(&ډېری, 1) == FAIL)
			    اذهب theend;
			چوکاټ = ((json_enc_چوکاټ_T *)ډېری.ga_معلومات)
							    + ډېری.ga_len;
			چوکاټ->je_type = ENC_DICT;
			چوکاټ->je_options = options;
			چوکاټ->je_u.d.dict = d;
			چوکاټ->je_u.d.hi = hi;
			چوکاټ->je_u.d.todo = todo;
			++ډېری.ga_len;
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
#د_که_پای
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
	    دننی_error_no_abort("json_encode_item()");
	    اذهب theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	لكل (;;)
	{
	    صحيح		advance = FALSE;

	    إذا (ډېری.ga_len == 0)
	    {
		ga_clear(&ډېری);
		أرجع OK;
	    }

	    چوکاټ = ((json_enc_چوکاټ_T *)ډېری.ga_معلومات)
							+ ډېری.ga_len - 1;
	    بدل (چوکاټ->je_type)
	    {
		حالة ENC_LIST:
		{
		    listitem_T	*li = چوکاټ->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    إذا ((چوکاټ->je_options & JSON_JS)
			    && li->li_بل == عدم
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_شمېره == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_بل;
		    چوکاټ->je_u.l.li = li;
		    إذا (li != عدم && !got_int)
		    {
			ga_append(gap, ',');
			options = چوکاټ->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			اكسر;
		    }
		    ga_append(gap, ']');
		    چوکاټ->je_u.l.list->lv_copyID = 0;
		    --ډېری.ga_len;
		    اكسر;
		}

		حالة ENC_TUPLE:
		{
		    صحيح	    idx = چوکاټ->je_u.t.idx;
		    صحيح	    len = چوکاټ->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    إذا ((چوکاټ->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_توکی =
				    TUPLE_توکی(چوکاټ->je_u.t.tuple, idx);

			إذا (t_توکی->v_type == VAR_SPECIAL
				&& t_توکی->vval.v_شمېره == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    چوکاټ->je_u.t.idx = idx;
		    إذا (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = چوکاټ->je_options & JSON_JS;
			cur_val = TUPLE_توکی(چوکاټ->je_u.t.tuple, idx);
			advance = TRUE;
			اكسر;
		    }
		    ga_append(gap, ']');
		    چوکاټ->je_u.t.tuple->tv_copyID = 0;
		    --ډېری.ga_len;
		    اكسر;
		}

		حالة ENC_DICT:
		{
		    hashitem_T	*hi = چوکاټ->je_u.d.hi;
		    صحيح		todo = چوکاټ->je_u.d.todo;

		    إذا (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			لكل (++hi; HASHITEM_تشه(hi); ++hi)
			    ;
			--todo;
			چوکاټ->je_u.d.hi = hi;
			چوکاټ->je_u.d.todo = todo;

			ga_append(gap, ',');
			إذا ((چوکاټ->je_options & JSON_JS)
				&& is_simple_کلی(hi->hi_کلی))
			    ga_concat(gap, hi->hi_کلی);
			وإلا
			    write_string(gap, hi->hi_کلی);
			ga_append(gap, ':');

			options = چوکاټ->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			اكسر;
		    }
		    ga_append(gap, '}');
		    چوکاټ->je_u.d.dict->dv_copyID = 0;
		    --ډېری.ga_len;
		    اكسر;
		}
	    }
	    إذا (advance)
		اكسر;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    لكل (i = 0; i < ډېری.ga_len; i++)
    {
	چوکاټ = ((json_enc_چوکاټ_T *)ډېری.ga_معلومات) + i;
	بدل (چوکاټ->je_type)
	{
	    حالة ENC_LIST:
		چوکاټ->je_u.l.list->lv_copyID = 0;
		اكسر;
	    حالة ENC_TUPLE:
		چوکاټ->je_u.t.tuple->tv_copyID = 0;
		اكسر;
	    حالة ENC_DICT:
		چوکاټ->je_u.d.dict->dv_copyID = 0;
		اكسر;
	}
    }
    ga_clear(&ډېری);
    أرجع FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    ساكن فراغ
fill_numbuflen(js_read_T *reader)
{
    إذا (reader->js_fill != عدم && (صحيح)(reader->js_پای_ټکی - reader->js_buf)
						- reader->js_کارول_شوی < NUMBUFLEN)
    {
	إذا (reader->js_fill(reader))
	    reader->js_پای_ټکی = reader->js_buf + STRLEN(reader->js_buf);
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
	c = reader->js_buf[reader->js_کارول_شوی];
	إذا (reader->js_fill != عدم && c == NUL)
	{
	    إذا (reader->js_fill(reader))
	    {
		reader->js_پای_ټکی = reader->js_buf + STRLEN(reader->js_buf);
		استمر;
	    }
	}
	إذا (c == NUL || c > ' ')
	    اكسر;
	++reader->js_کارول_شوی;
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

    p = reader->js_buf + reader->js_کارول_شوی + 1; // skip over " or '
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
	    len = (صحيح)(reader->js_پای_ټکی - p);
	    reader->js_کارول_شوی = (صحيح)(p - reader->js_buf);
	    إذا (!reader->js_fill(reader))
		اكسر; // didn't get more
	    p = reader->js_buf + reader->js_کارول_شوی;
	    reader->js_پای_ټکی = reader->js_buf + STRLEN(reader->js_buf);
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
				     && (صحيح)(reader->js_پای_ټکی - p) < NUMBUFLEN)
		    {
			reader->js_کارول_شوی = (صحيح)(p - reader->js_buf);
			إذا (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_کارول_شوی;
			    reader->js_پای_ټکی = reader->js_buf
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
			    && (صحيح)(reader->js_پای_ټکی - p) >= 6
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
		mch_memmove((حرف *)ga.ga_معلومات + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_کارول_شوی = (صحيح)(p - reader->js_buf);
    إذا (*p == quote)
    {
	++reader->js_کارول_شوی;
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
		convert_تیاری(&conv, (char_u*)"utf-8", p_enc);
		إذا (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_معلومات, عدم);
		    vim_free(ga.ga_معلومات);
		}
		convert_تیاری(&conv, عدم, عدم);
	    }
	    وإلا
#د_که_پای
		res->vval.v_string = ga.ga_معلومات;
	}
	أرجع OK;
    }
    إذا (res != عدم)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_شمېره = VVAL_NONE;
	ga_clear(&ga);
    }
    أرجع MAYBE;
}

تعريفنوع تعداد {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_کلی,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

تعريفنوع بنية {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_کلی_tv;
    char_u	  *jd_کلی;
} json_dec_توکی_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    ساكن صحيح
json_decode_توکی(js_read_T *reader, typval_T *res, صحيح options)
{
    char_u	*p;
    صحيح		i;
    صحيح		len;
    صحيح		retval;
    garray_T	ډېری;
    typval_T	توکی;
    typval_T	*cur_توکی;
    json_dec_توکی_T *سر_توکی;
    char_u	کلی_buf[NUMBUFLEN];

    ga_init2(&ډېری, حجم(json_dec_توکی_T), 100);
    cur_توکی = res;
    init_tv(&توکی);
    إذا (res != عدم)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_کارول_شوی;
    لكل (;;)
    {
	سر_توکی = عدم;
	إذا (ډېری.ga_len > 0)
	{
	    سر_توکی = ((json_dec_توکی_T *)ډېری.ga_معلومات) + ډېری.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_کارول_شوی;
	    إذا (*p == NUL)
	    {
		retval = MAYBE;
		اذهب theend;
	    }
	    إذا (سر_توکی->jd_type == JSON_OBJECT_کلی
					    || سر_توکی->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		إذا (*p == (سر_توکی->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_کارول_شوی; // consume the ']' or '}'
		    --ډېری.ga_len;
		    إذا (ډېری.ga_len == 0)
		    {
			retval = OK;
			اذهب theend;
		    }
		    إذا (cur_توکی != عدم)
			cur_توکی = &سر_توکی->jd_tv;
		    اذهب توکی_پای_ټکی;
		}
	    }
	}

	إذا (سر_توکی != عدم && سر_توکی->jd_type == JSON_OBJECT_کلی
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_کارول_شوی] != '"'
		&& reader->js_buf[reader->js_کارول_شوی] != '\''
		&& reader->js_buf[reader->js_کارول_شوی] != '['
		&& reader->js_buf[reader->js_کارول_شوی] != '{')
	{
	    char_u *کلی;

	    // accept an object key that is not in quotes
	    کلی = p = reader->js_buf + reader->js_کارول_شوی;
	    طالما (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    إذا (cur_توکی != عدم)
	    {
		cur_توکی->v_type = VAR_STRING;
		cur_توکی->vval.v_string = vim_strnsave(کلی, p - کلی);
		سر_توکی->jd_کلی = cur_توکی->vval.v_string;
	    }
	    reader->js_کارول_شوی += (صحيح)(p - کلی);
	}
	وإلا
	{
	    بدل (*p)
	    {
		حالة '[': // start of array
		    إذا (سر_توکی && سر_توکی->jd_type == JSON_OBJECT_کلی)
		    {
			retval = FAIL;
			اكسر;
		    }
		    إذا (ډېری.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_ژوروالی_is_higher_than_maxfuncdepth));
			retval = FAIL;
			اكسر;
		    }
		    إذا (ga_grow(&ډېری, 1) == FAIL)
		    {
			retval = FAIL;
			اكسر;
		    }
		    إذا (cur_توکی != عدم && rettv_list_alloc(cur_توکی) == FAIL)
		    {
			cur_توکی->v_type = VAR_SPECIAL;
			cur_توکی->vval.v_شمېره = VVAL_NONE;
			retval = FAIL;
			اكسر;
		    }

		    ++reader->js_کارول_شوی; // consume the '['
		    سر_توکی = ((json_dec_توکی_T *)ډېری.ga_معلومات)
								+ ډېری.ga_len;
		    سر_توکی->jd_type = JSON_ARRAY;
		    ++ډېری.ga_len;
		    إذا (cur_توکی != عدم)
		    {
			سر_توکی->jd_tv = *cur_توکی;
			cur_توکی = &توکی;
		    }
		    استمر;

		حالة '{': // start of object
		    إذا (سر_توکی && سر_توکی->jd_type == JSON_OBJECT_کلی)
		    {
			retval = FAIL;
			اكسر;
		    }
		    إذا (ډېری.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_ژوروالی_is_higher_than_maxfuncdepth));
			retval = FAIL;
			اكسر;
		    }
		    إذا (ga_grow(&ډېری, 1) == FAIL)
		    {
			retval = FAIL;
			اكسر;
		    }
		    إذا (cur_توکی != عدم && rettv_dict_alloc(cur_توکی) == FAIL)
		    {
			cur_توکی->v_type = VAR_SPECIAL;
			cur_توکی->vval.v_شمېره = VVAL_NONE;
			retval = FAIL;
			اكسر;
		    }

		    ++reader->js_کارول_شوی; // consume the '{'
		    سر_توکی = ((json_dec_توکی_T *)ډېری.ga_معلومات)
								+ ډېری.ga_len;
		    سر_توکی->jd_type = JSON_OBJECT_کلی;
		    ++ډېری.ga_len;
		    إذا (cur_توکی != عدم)
		    {
			سر_توکی->jd_tv = *cur_توکی;
			cur_توکی = &سر_توکی->jd_کلی_tv;
		    }
		    استمر;

		حالة '"': // string
		    retval = json_decode_string(reader, cur_توکی, *p);
		    اكسر;

		حالة '\'':
		    إذا (options & JSON_JS)
			retval = json_decode_string(reader, cur_توکی, *p);
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
		حالة NUL: //تشه
		    إذا (cur_توکی != عدم)
		    {
			cur_توکی->v_type = VAR_SPECIAL;
			cur_توکی->vval.v_شمېره = VVAL_NONE;
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
			    إذا (cur_توکی == عدم)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    وإلا
			    {
				cur_توکی->v_type = VAR_FLOAT;
				len = string2float(p, &cur_توکی->vval.v_float,
									FALSE);
			    }
			}
			وإلا
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_کارول_شوی,
				    عدم, &len, 0, // what
				    &nr, عدم, 0, TRUE, عدم);
			    إذا (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				اذهب theend;
			    }
			    إذا (cur_توکی != عدم)
			    {
				cur_توکی->v_type = VAR_شمېره;
				cur_توکی->vval.v_شمېره = nr;
			    }
			}
			reader->js_کارول_شوی += len;
			retval = OK;
			اكسر;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#تعریف_کړه سمون_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((حرف *)(p), (kw), (len)) == 0 \
     : STRNCMP((حرف *)(p), (kw), (len)) == 0)
		    إذا (سمون_KW(p, "false", 5))
		    {
			reader->js_کارول_شوی += 5;
			إذا (cur_توکی != عدم)
			{
			    cur_توکی->v_type = VAR_BOOL;
			    cur_توکی->vval.v_شمېره = VVAL_FALSE;
			}
			retval = OK;
			اكسر;
		    }
		    إذا (سمون_KW(p, "true", 4))
		    {
			reader->js_کارول_شوی += 4;
			إذا (cur_توکی != عدم)
			{
			    cur_توکی->v_type = VAR_BOOL;
			    cur_توکی->vval.v_شمېره = VVAL_TRUE;
			}
			retval = OK;
			اكسر;
		    }
		    إذا (سمون_KW(p, "null", 4))
		    {
			reader->js_کارول_شوی += 4;
			إذا (cur_توکی != عدم)
			{
			    cur_توکی->v_type = VAR_SPECIAL;
			    cur_توکی->vval.v_شمېره = VVAL_NULL;
			}
			retval = OK;
			اكسر;
		    }
		    إذا (سمون_KW(p, "NaN", 3))
		    {
			reader->js_کارول_شوی += 3;
			إذا (cur_توکی != عدم)
			{
			    cur_توکی->v_type = VAR_FLOAT;
			    cur_توکی->vval.v_float = NAN;
			}
			retval = OK;
			اكسر;
		    }
		    إذا (سمون_KW(p, "-Infinity", 9))
		    {
			reader->js_کارول_شوی += 9;
			إذا (cur_توکی != عدم)
			{
			    cur_توکی->v_type = VAR_FLOAT;
			    cur_توکی->vval.v_float = -INFINITY;
			}
			retval = OK;
			اكسر;
		    }
		    إذا (سمون_KW(p, "Infinity", 8))
		    {
			reader->js_کارول_شوی += 8;
			إذا (cur_توکی != عدم)
			{
			    cur_توکی->v_type = VAR_FLOAT;
			    cur_توکی->vval.v_float = INFINITY;
			}
			retval = OK;
			اكسر;
		    }
#تعریف_لغوه سمون_KW
		    // check for truncated name
		    len = (صحيح)(reader->js_پای_ټکی
					 - (reader->js_buf + reader->js_کارول_شوی));
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
	    إذا (retval == MAYBE || ډېری.ga_len == 0)
		اذهب theend;

	    إذا (سر_توکی != عدم && سر_توکی->jd_type == JSON_OBJECT_کلی
		    && cur_توکی != عدم)
	    {
		إذا (cur_توکی->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    اذهب theend;
		}
		سر_توکی->jd_کلی = tv_get_string_buf_chk(cur_توکی, کلی_buf);
		إذا (سر_توکی->jd_کلی == عدم)
		{
		    emsg(_(e_invalid_دلیل));
		    retval = FAIL;
		    اذهب theend;
		}
	    }
	}

توکی_پای_ټکی:
	سر_توکی = ((json_dec_توکی_T *)ډېری.ga_معلومات) + ډېری.ga_len - 1;
	بدل (سر_توکی->jd_type)
	{
	    حالة JSON_ARRAY:
		إذا (res != عدم)
		{
		    listitem_T	*li = listitem_alloc();

		    إذا (li == عدم)
		    {
			clear_tv(cur_توکی);
			retval = FAIL;
			اذهب theend;
		    }
		    li->li_tv = *cur_توکی;
		    list_append(سر_توکی->jd_tv.vval.v_list, li);
		}
		إذا (cur_توکی != عدم)
		    cur_توکی = &توکی;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_کارول_شوی;
		إذا (*p == ',')
		    ++reader->js_کارول_شوی;
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

	    حالة JSON_OBJECT_کلی:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_کارول_شوی;
		إذا (*p != ':')
		{
		    إذا (cur_توکی != عدم)
			clear_tv(cur_توکی);
		    إذا (*p == NUL)
			retval = MAYBE;
		    وإلا
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    اذهب theend;
		}
		++reader->js_کارول_شوی;
		json_skip_white(reader);
		سر_توکی->jd_type = JSON_OBJECT;
		إذا (cur_توکی != عدم)
		    cur_توکی = &توکی;
		اكسر;

	    حالة JSON_OBJECT:
		إذا (cur_توکی != عدم
			&& dict_has_کلی(سر_توکی->jd_tv.vval.v_dict,
						(حرف *)سر_توکی->jd_کلی))
		{
		    semsg(_(e_duplicate_کلی_دننه_json_str), سر_توکی->jd_کلی);
		    clear_tv(cur_توکی);
		    retval = FAIL;
		    اذهب theend;
		}

		إذا (cur_توکی != عدم)
		{
		    dictitem_T *di = dictitem_alloc(سر_توکی->jd_کلی);

		    clear_tv(&سر_توکی->jd_کلی_tv);
		    إذا (di == عدم)
		    {
			clear_tv(cur_توکی);
			retval = FAIL;
			اذهب theend;
		    }
		    di->di_tv = *cur_توکی;
		    di->di_tv.v_قلف = 0;
		    إذا (dict_جمع_کړه(سر_توکی->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			اذهب theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_کارول_شوی;
		إذا (*p == ',')
		    ++reader->js_کارول_شوی;
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
		سر_توکی->jd_type = JSON_OBJECT_کلی;
		إذا (cur_توکی != عدم)
		    cur_توکی = &سر_توکی->jd_کلی_tv;
		اكسر;
	}
    }

    // Get here when parsing failed.
    إذا (res != عدم)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_شمېره = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_کارول_شوی);

theend:
    لكل (i = 0; i < ډېری.ga_len; i++)
	clear_tv(&(((json_dec_توکی_T *)ډېری.ga_معلومات) + i)->jd_کلی_tv);
    ga_clear(&ډېری);

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
    reader->js_پای_ټکی = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_توکی(reader, res, options);
    إذا (ret != OK)
    {
	إذا (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	أرجع FAIL;
    }
    json_skip_white(reader);
    إذا (reader->js_buf[reader->js_کارول_شوی] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_کارول_شوی);
	أرجع FAIL;
    }
    أرجع OK;
}

#إذا defined(FEAT_دنده_CHANNEL)
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
    reader->js_پای_ټکی = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_توکی(reader, res, options);
    json_skip_white(reader);

    أرجع ret;
}
#د_که_پای

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
json_پیدا_کړه_پای_ټکی(js_read_T *reader, صحيح options)
{
    صحيح کارول_شوی_save = reader->js_کارول_شوی;
    صحيح ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_پای_ټکی = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_توکی(reader, عدم, options);
    reader->js_کارول_شوی = کارول_شوی_save;
    أرجع ret;
}

/*
 * "js_decode()" function
 */
    فراغ
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = عدم;
    reader.js_کارول_شوی = 0;
    إذا (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_دلیل));
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

    إذا (دننه_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = عدم;
    reader.js_کارول_شوی = 0;
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
#د_که_پای
