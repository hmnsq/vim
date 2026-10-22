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
#تعریف_کرو USING_FLOAT_STUFF

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

ساکن صحیح json_encode_شے(garray_T *gap, typval_T *val, صحیح copyID, صحیح options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    ساکن صحیح
json_encode_gap(garray_T *gap, typval_T *val, صحیح options)
{
    اگر (json_encode_شے(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_ڈیٹا = vim_strsave((char_u *)"");
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, صحیح options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    اگر (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    واپس ga.ga_ڈیٹا;
}

#اگر defined(FEAT_کام_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_اظہاریہ(صحیح nr, typval_T *val, صحیح options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_نمبر;
    nrtv.vval.v_نمبر = nr;
    اگر (rettv_list_alloc(&listtv) == FAIL)
	واپس تہی;
    اگر (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	واپس تہی;
    }

    ga_init2(&ga, 1, 4000);
    اگر (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    واپس ga.ga_ڈیٹا;
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
    اگر (json_encode_gap(&ga, val, 0) == FAIL)
	واپس تہی;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_ڈیٹا, ga.ga_len);
    ga_clear(&ga);
    واپس lspga.ga_ڈیٹا;
}
#اگر_ختم

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
ساکن ثابت حرف ascii_needs_escape[128] = {
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
    ساکن خالی
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#اگر defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = تہی;
#اگر_ختم
    صحیح		c;

    اگر (res == تہی)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	واپس;
    }

#اگر defined(USE_ICONV)
    اگر (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_تیاری(&conv, p_enc, (char_u*)"utf-8");
	اگر (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, تہی);
	convert_تیاری(&conv, تہی, تہی);
    }
#اگر_ختم
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (صحیح)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    جب ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	اگر (c < 0x80)
	{
	    اگر (!ascii_needs_escape[c])
	    {
		res += 1;
		جاری;
	    }

	    اگر (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    انتخاب (c)
	    {
		حالت 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); توڑو;
		حالت 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); توڑو;
		حالت 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); توڑو;
		حالت 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); توڑو;
		حالت 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); توڑو;
		حالت 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); توڑو;
		حالت 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); توڑو;
		طےشدہ:
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
	ورنہ
	{
	    صحیح	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    اگر (l > 1)
	    {
		res += l;
		جاری;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    اگر (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    اگر (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#اگر defined(USE_ICONV)
    vim_free(converted);
#اگر_ختم
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    ساکن صحیح
is_simple_کلید(char_u *کلید)
{
    char_u *p;

    اگر (!ASCII_ISALPHA(*کلید))
	واپس FALSE;
    برائے (p = کلید + 1; *p != NUL; ++p)
	اگر (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    واپس FALSE;
    واپس TRUE;
}

قسمتعریف شمار {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

قسمتعریف ساخت {
    json_enc_type_T je_type;
    صحیح		    je_options;	    // options when this container was entered
    اتحاد {
	ساخت {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	ساخت {
	    tuple_T	*tuple;
	    صحیح		idx;	    // current index
	    صحیح		len;
	} t;
	ساخت {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    صحیح		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_چوکھٹا_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    ساکن صحیح
json_encode_شے(garray_T *gap, typval_T *val, صحیح copyID, صحیح options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    صحیح			i;
    garray_T		ڈھیر;
    typval_T		*cur_val;
    json_enc_چوکھٹا_T	*چوکھٹا;

    ga_init2(&ڈھیر, حجم(json_enc_چوکھٹا_T), 100);
    cur_val = val;

    برائے (;;)
    {
    انتخاب (cur_val->v_type)
    {
	حالت VAR_BOOL:
	    انتخاب ((طویل)cur_val->vval.v_نمبر)
	    {
		حالت VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); توڑو;
		حالت VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); توڑو;
	    }
	    توڑو;

	حالت VAR_SPECIAL:
	    انتخاب ((طویل)cur_val->vval.v_نمبر)
	    {
		حالت VVAL_NONE: اگر ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    توڑو;
				// FALLTHROUGH
		حالت VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); توڑو;
	    }
	    توڑو;

	حالت VAR_نمبر:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((حرف *)numbuf, حجم(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_نمبر);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    توڑو;

	حالت VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    توڑو;

	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_کام:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	حالت VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    جاؤ theend;

	حالت VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    اگر (b == تہی || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    ورنہ
	    {
		صحیح	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		اگر (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    جاؤ theend;
		src = (char_u *)b->bv_ga.ga_ڈیٹا;
		dst = (char_u *)gap->ga_ڈیٹا + gap->ga_len;
		*dst++ = '[';
		برائے (i = 0; i < blen; i++)
		{
		    صحیح	    byte = src[i];

		    اگر (i > 0)
			*dst++ = ',';
		    اگر (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    ورنہ اگر (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    ورنہ
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (صحیح)(dst - (char_u *)gap->ga_ڈیٹا);
	    }
	    توڑو;

	حالت VAR_LIST:
	    l = cur_val->vval.v_list;
	    اگر (l == تہی)
		GA_CONCAT_LITERAL(gap, "[]");
	    ورنہ
	    {
		اگر (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ورنہ
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    اگر (l->lv_پہلا != تہی)
		    {
			اگر (ڈھیر.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_گہرائی_is_higher_than_maxfuncdepth));
			    جاؤ theend;
			}
			اگر (ga_grow(&ڈھیر, 1) == FAIL)
			    جاؤ theend;
			چوکھٹا = ((json_enc_چوکھٹا_T *)ڈھیر.ga_ڈیٹا)
							    + ڈھیر.ga_len;
			چوکھٹا->je_type = ENC_LIST;
			چوکھٹا->je_options = options;
			چوکھٹا->je_u.l.list = l;
			چوکھٹا->je_u.l.li = l->lv_پہلا;
			++ڈھیر.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_پہلا->li_tv;
			جاری;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    توڑو;

	حالت VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    اگر (tuple == تہی)
		GA_CONCAT_LITERAL(gap, "[]");
	    ورنہ
	    {
		اگر (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ورنہ
		{
		    صحیح		len = TUPLE_LEN(tuple);

		    اگر (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			اگر (ڈھیر.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_گہرائی_is_higher_than_maxfuncdepth));
			    جاؤ theend;
			}
			اگر (ga_grow(&ڈھیر, 1) == FAIL)
			    جاؤ theend;
			چوکھٹا = ((json_enc_چوکھٹا_T *)ڈھیر.ga_ڈیٹا)
							    + ڈھیر.ga_len;
			چوکھٹا->je_type = ENC_TUPLE;
			چوکھٹا->je_options = options;
			چوکھٹا->je_u.t.tuple = tuple;
			چوکھٹا->je_u.t.idx = 0;
			چوکھٹا->je_u.t.len = len;
			++ڈھیر.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_شے(tuple, 0);
			جاری;
		    }
		    ورنہ
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    توڑو;

	حالت VAR_DICT:
	    d = cur_val->vval.v_dict;
	    اگر (d == تہی)
		GA_CONCAT_LITERAL(gap, "{}");
	    ورنہ
	    {
		اگر (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		ورنہ
		{
		    صحیح		todo = (صحیح)d->dv_hashtab.ht_مستعمل;
		    hashitem_T	*hi;

		    اگر (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			برائے (hi = d->dv_hashtab.ht_array;
						  HASHITEM_خالی_خانے(hi); ++hi)
			    ;
			--todo;

			// Write first key
			اگر ((options & JSON_JS)
						 && is_simple_کلید(hi->hi_کلید))
			    ga_concat(gap, hi->hi_کلید);
			ورنہ
			    write_string(gap, hi->hi_کلید);
			ga_append(gap, ':');

			اگر (ڈھیر.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_گہرائی_is_higher_than_maxfuncdepth));
			    جاؤ theend;
			}
			اگر (ga_grow(&ڈھیر, 1) == FAIL)
			    جاؤ theend;
			چوکھٹا = ((json_enc_چوکھٹا_T *)ڈھیر.ga_ڈیٹا)
							    + ڈھیر.ga_len;
			چوکھٹا->je_type = ENC_DICT;
			چوکھٹا->je_options = options;
			چوکھٹا->je_u.d.dict = d;
			چوکھٹا->je_u.d.hi = hi;
			چوکھٹا->je_u.d.todo = todo;
			++ڈھیر.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			جاری;
		    }
		    ورنہ
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    توڑو;

	حالت VAR_FLOAT:
#اگر defined(HAVE_MATH_H)
	    اگر (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    ورنہ اگر (isinf(cur_val->vval.v_float))
	    {
		اگر (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		ورنہ
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    ورنہ
#اگر_ختم
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((حرف *)numbuf, حجم(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    توڑو;

	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	    اندرونی_error_no_abort("json_encode_item()");
	    جاؤ theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	برائے (;;)
	{
	    صحیح		advance = FALSE;

	    اگر (ڈھیر.ga_len == 0)
	    {
		ga_clear(&ڈھیر);
		واپس OK;
	    }

	    چوکھٹا = ((json_enc_چوکھٹا_T *)ڈھیر.ga_ڈیٹا)
							+ ڈھیر.ga_len - 1;
	    انتخاب (چوکھٹا->je_type)
	    {
		حالت ENC_LIST:
		{
		    listitem_T	*li = چوکھٹا->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    اگر ((چوکھٹا->je_options & JSON_JS)
			    && li->li_اگلا == تہی
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_نمبر == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_اگلا;
		    چوکھٹا->je_u.l.li = li;
		    اگر (li != تہی && !got_int)
		    {
			ga_append(gap, ',');
			options = چوکھٹا->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			توڑو;
		    }
		    ga_append(gap, ']');
		    چوکھٹا->je_u.l.list->lv_copyID = 0;
		    --ڈھیر.ga_len;
		    توڑو;
		}

		حالت ENC_TUPLE:
		{
		    صحیح	    idx = چوکھٹا->je_u.t.idx;
		    صحیح	    len = چوکھٹا->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    اگر ((چوکھٹا->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_شے =
				    TUPLE_شے(چوکھٹا->je_u.t.tuple, idx);

			اگر (t_شے->v_type == VAR_SPECIAL
				&& t_شے->vval.v_نمبر == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    چوکھٹا->je_u.t.idx = idx;
		    اگر (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = چوکھٹا->je_options & JSON_JS;
			cur_val = TUPLE_شے(چوکھٹا->je_u.t.tuple, idx);
			advance = TRUE;
			توڑو;
		    }
		    ga_append(gap, ']');
		    چوکھٹا->je_u.t.tuple->tv_copyID = 0;
		    --ڈھیر.ga_len;
		    توڑو;
		}

		حالت ENC_DICT:
		{
		    hashitem_T	*hi = چوکھٹا->je_u.d.hi;
		    صحیح		todo = چوکھٹا->je_u.d.todo;

		    اگر (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			برائے (++hi; HASHITEM_خالی_خانے(hi); ++hi)
			    ;
			--todo;
			چوکھٹا->je_u.d.hi = hi;
			چوکھٹا->je_u.d.todo = todo;

			ga_append(gap, ',');
			اگر ((چوکھٹا->je_options & JSON_JS)
				&& is_simple_کلید(hi->hi_کلید))
			    ga_concat(gap, hi->hi_کلید);
			ورنہ
			    write_string(gap, hi->hi_کلید);
			ga_append(gap, ':');

			options = چوکھٹا->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			توڑو;
		    }
		    ga_append(gap, '}');
		    چوکھٹا->je_u.d.dict->dv_copyID = 0;
		    --ڈھیر.ga_len;
		    توڑو;
		}
	    }
	    اگر (advance)
		توڑو;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    برائے (i = 0; i < ڈھیر.ga_len; i++)
    {
	چوکھٹا = ((json_enc_چوکھٹا_T *)ڈھیر.ga_ڈیٹا) + i;
	انتخاب (چوکھٹا->je_type)
	{
	    حالت ENC_LIST:
		چوکھٹا->je_u.l.list->lv_copyID = 0;
		توڑو;
	    حالت ENC_TUPLE:
		چوکھٹا->je_u.t.tuple->tv_copyID = 0;
		توڑو;
	    حالت ENC_DICT:
		چوکھٹا->je_u.d.dict->dv_copyID = 0;
		توڑو;
	}
    }
    ga_clear(&ڈھیر);
    واپس FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    ساکن خالی
fill_numbuflen(js_read_T *reader)
{
    اگر (reader->js_fill != تہی && (صحیح)(reader->js_انتہا - reader->js_buf)
						- reader->js_مستعمل < NUMBUFLEN)
    {
	اگر (reader->js_fill(reader))
	    reader->js_انتہا = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    ساکن خالی
json_skip_white(js_read_T *reader)
{
    صحیح c;

    برائے (;;)
    {
	c = reader->js_buf[reader->js_مستعمل];
	اگر (reader->js_fill != تہی && c == NUL)
	{
	    اگر (reader->js_fill(reader))
	    {
		reader->js_انتہا = reader->js_buf + STRLEN(reader->js_buf);
		جاری;
	    }
	}
	اگر (c == NUL || c > ' ')
	    توڑو;
	++reader->js_مستعمل;
    }
    fill_numbuflen(reader);
}

    ساکن صحیح
json_decode_string(js_read_T *reader, typval_T *res, صحیح quote)
{
    garray_T    ga;
    صحیح		len;
    char_u	*p;
    صحیح		c;
    varnumber_T	nr;

    اگر (res != تہی)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_مستعمل + 1; // skip over " or '
    جب (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	اگر (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    اگر (reader->js_fill == تہی)
		توڑو;
	    len = (صحیح)(reader->js_انتہا - p);
	    reader->js_مستعمل = (صحیح)(p - reader->js_buf);
	    اگر (!reader->js_fill(reader))
		توڑو; // didn't get more
	    p = reader->js_buf + reader->js_مستعمل;
	    reader->js_انتہا = reader->js_buf + STRLEN(reader->js_buf);
	    جاری;
	}

	اگر (*p == '\\')
	{
	    c = -1;
	    انتخاب (p[1])
	    {
		حالت '\\': c = '\\'; توڑو;
		حالت '"': c = '"'; توڑو;
		حالت 'b': c = BS; توڑو;
		حالت 't': c = TAB; توڑو;
		حالت 'n': c = NL; توڑو;
		حالت 'f': c = FF; توڑو;
		حالت 'r': c = CAR; توڑو;
		حالت 'u':
		    اگر (reader->js_fill != تہی
				     && (صحیح)(reader->js_انتہا - p) < NUMBUFLEN)
		    {
			reader->js_مستعمل = (صحیح)(p - reader->js_buf);
			اگر (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_مستعمل;
			    reader->js_انتہا = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, تہی, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, تہی, 4, TRUE, تہی);
		    اگر (len == 0)
		    {
			اگر (res != تہی)
			    ga_clear(&ga);
			واپس FAIL;
		    }
		    p += len + 2;
		    اگر (0xd800 <= nr && nr <= 0xdbff
			    && (صحیح)(reader->js_انتہا - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, تہی, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, تہی, 4, TRUE, تہی);
			اگر (len == 0)
			{
			    اگر (res != تہی)
				ga_clear(&ga);
			    واپس FAIL;
			}
			اگر (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    اگر (0xd800 <= nr && nr <= 0xdfff)
		    {
			اگر (res != تہی)
			    ga_clear(&ga);
			واپس FAIL;
		    }
		    اگر (res != تہی)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((صحیح)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    توڑو;
		طےشدہ:
		    // not a special char, skip over backslash
		    ++p;
		    جاری;
	    }
	    اگر (c > 0)
	    {
		p += 2;
		اگر (res != تہی)
		    ga_append(&ga, c);
	    }
	}
	ورنہ
	{
	    len = utf_ptr2len(p);
	    اگر (res != تہی)
	    {
		اگر (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    واپس FAIL;
		}
		mch_memmove((حرف *)ga.ga_ڈیٹا + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_مستعمل = (صحیح)(p - reader->js_buf);
    اگر (*p == quote)
    {
	++reader->js_مستعمل;
	اگر (res != تہی)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#اگر defined(USE_ICONV)
	    اگر (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_تیاری(&conv, (char_u*)"utf-8", p_enc);
		اگر (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_ڈیٹا, تہی);
		    vim_free(ga.ga_ڈیٹا);
		}
		convert_تیاری(&conv, تہی, تہی);
	    }
	    ورنہ
#اگر_ختم
		res->vval.v_string = ga.ga_ڈیٹا;
	}
	واپس OK;
    }
    اگر (res != تہی)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_نمبر = VVAL_NONE;
	ga_clear(&ga);
    }
    واپس MAYBE;
}

قسمتعریف شمار {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_کلید,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

قسمتعریف ساخت {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_کلید_tv;
    char_u	  *jd_کلید;
} json_dec_شے_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    ساکن صحیح
json_decode_شے(js_read_T *reader, typval_T *res, صحیح options)
{
    char_u	*p;
    صحیح		i;
    صحیح		len;
    صحیح		retval;
    garray_T	ڈھیر;
    typval_T	شے;
    typval_T	*cur_شے;
    json_dec_شے_T *چوٹی_شے;
    char_u	کلید_buf[NUMBUFLEN];

    ga_init2(&ڈھیر, حجم(json_dec_شے_T), 100);
    cur_شے = res;
    init_tv(&شے);
    اگر (res != تہی)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_مستعمل;
    برائے (;;)
    {
	چوٹی_شے = تہی;
	اگر (ڈھیر.ga_len > 0)
	{
	    چوٹی_شے = ((json_dec_شے_T *)ڈھیر.ga_ڈیٹا) + ڈھیر.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_مستعمل;
	    اگر (*p == NUL)
	    {
		retval = MAYBE;
		جاؤ theend;
	    }
	    اگر (چوٹی_شے->jd_type == JSON_OBJECT_کلید
					    || چوٹی_شے->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		اگر (*p == (چوٹی_شے->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_مستعمل; // consume the ']' or '}'
		    --ڈھیر.ga_len;
		    اگر (ڈھیر.ga_len == 0)
		    {
			retval = OK;
			جاؤ theend;
		    }
		    اگر (cur_شے != تہی)
			cur_شے = &چوٹی_شے->jd_tv;
		    جاؤ شے_انتہا;
		}
	    }
	}

	اگر (چوٹی_شے != تہی && چوٹی_شے->jd_type == JSON_OBJECT_کلید
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_مستعمل] != '"'
		&& reader->js_buf[reader->js_مستعمل] != '\''
		&& reader->js_buf[reader->js_مستعمل] != '['
		&& reader->js_buf[reader->js_مستعمل] != '{')
	{
	    char_u *کلید;

	    // accept an object key that is not in quotes
	    کلید = p = reader->js_buf + reader->js_مستعمل;
	    جب (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    اگر (cur_شے != تہی)
	    {
		cur_شے->v_type = VAR_STRING;
		cur_شے->vval.v_string = vim_strnsave(کلید, p - کلید);
		چوٹی_شے->jd_کلید = cur_شے->vval.v_string;
	    }
	    reader->js_مستعمل += (صحیح)(p - کلید);
	}
	ورنہ
	{
	    انتخاب (*p)
	    {
		حالت '[': // start of array
		    اگر (چوٹی_شے && چوٹی_شے->jd_type == JSON_OBJECT_کلید)
		    {
			retval = FAIL;
			توڑو;
		    }
		    اگر (ڈھیر.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_گہرائی_is_higher_than_maxfuncdepth));
			retval = FAIL;
			توڑو;
		    }
		    اگر (ga_grow(&ڈھیر, 1) == FAIL)
		    {
			retval = FAIL;
			توڑو;
		    }
		    اگر (cur_شے != تہی && rettv_list_alloc(cur_شے) == FAIL)
		    {
			cur_شے->v_type = VAR_SPECIAL;
			cur_شے->vval.v_نمبر = VVAL_NONE;
			retval = FAIL;
			توڑو;
		    }

		    ++reader->js_مستعمل; // consume the '['
		    چوٹی_شے = ((json_dec_شے_T *)ڈھیر.ga_ڈیٹا)
								+ ڈھیر.ga_len;
		    چوٹی_شے->jd_type = JSON_ARRAY;
		    ++ڈھیر.ga_len;
		    اگر (cur_شے != تہی)
		    {
			چوٹی_شے->jd_tv = *cur_شے;
			cur_شے = &شے;
		    }
		    جاری;

		حالت '{': // start of object
		    اگر (چوٹی_شے && چوٹی_شے->jd_type == JSON_OBJECT_کلید)
		    {
			retval = FAIL;
			توڑو;
		    }
		    اگر (ڈھیر.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_گہرائی_is_higher_than_maxfuncdepth));
			retval = FAIL;
			توڑو;
		    }
		    اگر (ga_grow(&ڈھیر, 1) == FAIL)
		    {
			retval = FAIL;
			توڑو;
		    }
		    اگر (cur_شے != تہی && rettv_dict_alloc(cur_شے) == FAIL)
		    {
			cur_شے->v_type = VAR_SPECIAL;
			cur_شے->vval.v_نمبر = VVAL_NONE;
			retval = FAIL;
			توڑو;
		    }

		    ++reader->js_مستعمل; // consume the '{'
		    چوٹی_شے = ((json_dec_شے_T *)ڈھیر.ga_ڈیٹا)
								+ ڈھیر.ga_len;
		    چوٹی_شے->jd_type = JSON_OBJECT_کلید;
		    ++ڈھیر.ga_len;
		    اگر (cur_شے != تہی)
		    {
			چوٹی_شے->jd_tv = *cur_شے;
			cur_شے = &چوٹی_شے->jd_کلید_tv;
		    }
		    جاری;

		حالت '"': // string
		    retval = json_decode_string(reader, cur_شے, *p);
		    توڑو;

		حالت '\'':
		    اگر (options & JSON_JS)
			retval = json_decode_string(reader, cur_شے, *p);
		    ورنہ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    توڑو;

		حالت ',': // comma: empty item
		    اگر ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			توڑو;
		    }
		    // FALLTHROUGH
		حالت NUL: //خالی_خانے
		    اگر (cur_شے != تہی)
		    {
			cur_شے->v_type = VAR_SPECIAL;
			cur_شے->vval.v_نمبر = VVAL_NONE;
		    }
		    retval = OK;
		    توڑو;

		طےشدہ:
		    اگر (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			اگر (*sp == '-')
			{
			    ++sp;
			    اگر (*sp == NUL)
			    {
				retval = MAYBE;
				توڑو;
			    }
			    اگر (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				توڑو;
			    }
			}
			sp = skipdigits(sp);
			اگر (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    اگر (cur_شے == تہی)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    ورنہ
			    {
				cur_شے->v_type = VAR_FLOAT;
				len = string2float(p, &cur_شے->vval.v_float,
									FALSE);
			    }
			}
			ورنہ
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_مستعمل,
				    تہی, &len, 0, // what
				    &nr, تہی, 0, TRUE, تہی);
			    اگر (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				جاؤ theend;
			    }
			    اگر (cur_شے != تہی)
			    {
				cur_شے->v_type = VAR_نمبر;
				cur_شے->vval.v_نمبر = nr;
			    }
			}
			reader->js_مستعمل += len;
			retval = OK;
			توڑو;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#تعریف_کرو مطابقت_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((حرف *)(p), (kw), (len)) == 0 \
     : STRNCMP((حرف *)(p), (kw), (len)) == 0)
		    اگر (مطابقت_KW(p, "false", 5))
		    {
			reader->js_مستعمل += 5;
			اگر (cur_شے != تہی)
			{
			    cur_شے->v_type = VAR_BOOL;
			    cur_شے->vval.v_نمبر = VVAL_FALSE;
			}
			retval = OK;
			توڑو;
		    }
		    اگر (مطابقت_KW(p, "true", 4))
		    {
			reader->js_مستعمل += 4;
			اگر (cur_شے != تہی)
			{
			    cur_شے->v_type = VAR_BOOL;
			    cur_شے->vval.v_نمبر = VVAL_TRUE;
			}
			retval = OK;
			توڑو;
		    }
		    اگر (مطابقت_KW(p, "null", 4))
		    {
			reader->js_مستعمل += 4;
			اگر (cur_شے != تہی)
			{
			    cur_شے->v_type = VAR_SPECIAL;
			    cur_شے->vval.v_نمبر = VVAL_NULL;
			}
			retval = OK;
			توڑو;
		    }
		    اگر (مطابقت_KW(p, "NaN", 3))
		    {
			reader->js_مستعمل += 3;
			اگر (cur_شے != تہی)
			{
			    cur_شے->v_type = VAR_FLOAT;
			    cur_شے->vval.v_float = NAN;
			}
			retval = OK;
			توڑو;
		    }
		    اگر (مطابقت_KW(p, "-Infinity", 9))
		    {
			reader->js_مستعمل += 9;
			اگر (cur_شے != تہی)
			{
			    cur_شے->v_type = VAR_FLOAT;
			    cur_شے->vval.v_float = -INFINITY;
			}
			retval = OK;
			توڑو;
		    }
		    اگر (مطابقت_KW(p, "Infinity", 8))
		    {
			reader->js_مستعمل += 8;
			اگر (cur_شے != تہی)
			{
			    cur_شے->v_type = VAR_FLOAT;
			    cur_شے->vval.v_float = INFINITY;
			}
			retval = OK;
			توڑو;
		    }
#تعریف_منسوخ مطابقت_KW
		    // check for truncated name
		    len = (صحیح)(reader->js_انتہا
					 - (reader->js_buf + reader->js_مستعمل));
		    اگر (
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
		    ورنہ
			retval = FAIL;
		    توڑو;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    اگر (retval == FAIL)
		توڑو;
	    اگر (retval == MAYBE || ڈھیر.ga_len == 0)
		جاؤ theend;

	    اگر (چوٹی_شے != تہی && چوٹی_شے->jd_type == JSON_OBJECT_کلید
		    && cur_شے != تہی)
	    {
		اگر (cur_شے->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    جاؤ theend;
		}
		چوٹی_شے->jd_کلید = tv_get_string_buf_chk(cur_شے, کلید_buf);
		اگر (چوٹی_شے->jd_کلید == تہی)
		{
		    emsg(_(e_invalid_دلیل));
		    retval = FAIL;
		    جاؤ theend;
		}
	    }
	}

شے_انتہا:
	چوٹی_شے = ((json_dec_شے_T *)ڈھیر.ga_ڈیٹا) + ڈھیر.ga_len - 1;
	انتخاب (چوٹی_شے->jd_type)
	{
	    حالت JSON_ARRAY:
		اگر (res != تہی)
		{
		    listitem_T	*li = listitem_alloc();

		    اگر (li == تہی)
		    {
			clear_tv(cur_شے);
			retval = FAIL;
			جاؤ theend;
		    }
		    li->li_tv = *cur_شے;
		    list_append(چوٹی_شے->jd_tv.vval.v_list, li);
		}
		اگر (cur_شے != تہی)
		    cur_شے = &شے;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_مستعمل;
		اگر (*p == ',')
		    ++reader->js_مستعمل;
		ورنہ اگر (*p != ']')
		{
		    اگر (*p == NUL)
			retval = MAYBE;
		    ورنہ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    جاؤ theend;
		}
		توڑو;

	    حالت JSON_OBJECT_کلید:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_مستعمل;
		اگر (*p != ':')
		{
		    اگر (cur_شے != تہی)
			clear_tv(cur_شے);
		    اگر (*p == NUL)
			retval = MAYBE;
		    ورنہ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    جاؤ theend;
		}
		++reader->js_مستعمل;
		json_skip_white(reader);
		چوٹی_شے->jd_type = JSON_OBJECT;
		اگر (cur_شے != تہی)
		    cur_شے = &شے;
		توڑو;

	    حالت JSON_OBJECT:
		اگر (cur_شے != تہی
			&& dict_has_کلید(چوٹی_شے->jd_tv.vval.v_dict,
						(حرف *)چوٹی_شے->jd_کلید))
		{
		    semsg(_(e_duplicate_کلید_اندر_json_str), چوٹی_شے->jd_کلید);
		    clear_tv(cur_شے);
		    retval = FAIL;
		    جاؤ theend;
		}

		اگر (cur_شے != تہی)
		{
		    dictitem_T *di = dictitem_alloc(چوٹی_شے->jd_کلید);

		    clear_tv(&چوٹی_شے->jd_کلید_tv);
		    اگر (di == تہی)
		    {
			clear_tv(cur_شے);
			retval = FAIL;
			جاؤ theend;
		    }
		    di->di_tv = *cur_شے;
		    di->di_tv.v_تالا = 0;
		    اگر (dict_جوڑو(چوٹی_شے->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			جاؤ theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_مستعمل;
		اگر (*p == ',')
		    ++reader->js_مستعمل;
		ورنہ اگر (*p != '}')
		{
		    اگر (*p == NUL)
			retval = MAYBE;
		    ورنہ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    جاؤ theend;
		}
		چوٹی_شے->jd_type = JSON_OBJECT_کلید;
		اگر (cur_شے != تہی)
		    cur_شے = &چوٹی_شے->jd_کلید_tv;
		توڑو;
	}
    }

    // Get here when parsing failed.
    اگر (res != تہی)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_نمبر = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_مستعمل);

theend:
    برائے (i = 0; i < ڈھیر.ga_len; i++)
	clear_tv(&(((json_dec_شے_T *)ڈھیر.ga_ڈیٹا) + i)->jd_کلید_tv);
    ga_clear(&ڈھیر);

    واپس retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    ساکن صحیح
json_decode_all(js_read_T *reader, typval_T *res, صحیح options)
{
    صحیح ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_انتہا = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_شے(reader, res, options);
    اگر (ret != OK)
    {
	اگر (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	واپس FAIL;
    }
    json_skip_white(reader);
    اگر (reader->js_buf[reader->js_مستعمل] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_مستعمل);
	واپس FAIL;
    }
    واپس OK;
}

#اگر defined(FEAT_کام_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    صحیح
json_decode(js_read_T *reader, typval_T *res, صحیح options)
{
    صحیح ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_انتہا = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_شے(reader, res, options);
    json_skip_white(reader);

    واپس ret;
}
#اگر_ختم

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
    صحیح
json_ڈھونڈو_انتہا(js_read_T *reader, صحیح options)
{
    صحیح مستعمل_save = reader->js_مستعمل;
    صحیح ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_انتہا = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_شے(reader, تہی, options);
    reader->js_مستعمل = مستعمل_save;
    واپس ret;
}

/*
 * "js_decode()" function
 */
    خالی
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = تہی;
    reader.js_مستعمل = 0;
    اگر (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_دلیل));
}

/*
 * "js_encode()" function
 */
    خالی
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    خالی
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = تہی;
    reader.js_مستعمل = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    خالی
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#اگر_ختم
