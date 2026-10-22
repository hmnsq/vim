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
#define USING_FLOAT_STUFF

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

স্থির পূর্ণসংখ্যা json_encode_item(garray_T *gap, typval_T *val, পূর্ণসংখ্যা copyID, পূর্ণসংখ্যা options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    স্থির পূর্ণসংখ্যা
json_encode_gap(garray_T *gap, typval_T *val, পূর্ণসংখ্যা options)
{
    যদি (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_তথ্য = vim_strsave((char_u *)"");
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, পূর্ণসংখ্যা options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    যদি (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    ফেরত ga.ga_তথ্য;
}

#যদি defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_রাশিমালা(পূর্ণসংখ্যা nr, typval_T *val, পূর্ণসংখ্যা options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_সংখ্যা;
    nrtv.vval.v_সংখ্যা = nr;
    যদি (rettv_list_alloc(&listtv) == FAIL)
	ফেরত ফাঁকা;
    যদি (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	ফেরত ফাঁকা;
    }

    ga_init2(&ga, 1, 4000);
    যদি (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    ফেরত ga.ga_তথ্য;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_বার্তা(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    যদি (json_encode_gap(&ga, val, 0) == FAIL)
	ফেরত ফাঁকা;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((অক্ষর *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_তথ্য, ga.ga_len);
    ga_clear(&ga);
    ফেরত lspga.ga_তথ্য;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
স্থির ধ্রুবক অক্ষর ascii_needs_escape[128] = {
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
    স্থির শূন্য
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#যদি defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = ফাঁকা;
#endif
    পূর্ণসংখ্যা		c;

    যদি (res == ফাঁকা)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	ফেরত;
    }

#যদি defined(USE_ICONV)
    যদি (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	যদি (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, ফাঁকা);
	convert_setup(&conv, ফাঁকা, ফাঁকা);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (পূর্ণসংখ্যা)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    যখন ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	যদি (c < 0x80)
	{
	    যদি (!ascii_needs_escape[c])
	    {
		res += 1;
		চালু;
	    }

	    যদি (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    নির্বাচন (c)
	    {
		ক্ষেত্র 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); বিরতি;
		ক্ষেত্র 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); বিরতি;
		ক্ষেত্র 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); বিরতি;
		ক্ষেত্র 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); বিরতি;
		ক্ষেত্র 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); বিরতি;
		ক্ষেত্র 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); বিরতি;
		ক্ষেত্র 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); বিরতি;
		পূর্বনির্ধারিত:
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
	অন্যথা
	{
	    পূর্ণসংখ্যা	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    যদি (l > 1)
	    {
		res += l;
		চালু;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    যদি (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    যদি (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#যদি defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    স্থির পূর্ণসংখ্যা
is_simple_চাবি(char_u *চাবি)
{
    char_u *p;

    যদি (!ASCII_ISALPHA(*চাবি))
	ফেরত FALSE;
    জন্য (p = চাবি + 1; *p != NUL; ++p)
	যদি (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    ফেরত FALSE;
    ফেরত TRUE;
}

প্রকারলেখ গণনা {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

প্রকারলেখ কাঠামো {
    json_enc_type_T je_type;
    পূর্ণসংখ্যা		    je_options;	    // options when this container was entered
    সংঘ {
	কাঠামো {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	কাঠামো {
	    tuple_T	*tuple;
	    পূর্ণসংখ্যা		idx;	    // current index
	    পূর্ণসংখ্যা		len;
	} t;
	কাঠামো {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    পূর্ণসংখ্যা		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    স্থির পূর্ণসংখ্যা
json_encode_item(garray_T *gap, typval_T *val, পূর্ণসংখ্যা copyID, পূর্ণসংখ্যা options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    পূর্ণসংখ্যা			i;
    garray_T		স্তূপ;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&স্তূপ, আকার(json_enc_frame_T), 100);
    cur_val = val;

    জন্য (;;)
    {
    নির্বাচন (cur_val->v_type)
    {
	ক্ষেত্র VAR_BOOL:
	    নির্বাচন ((long)cur_val->vval.v_সংখ্যা)
	    {
		ক্ষেত্র VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); বিরতি;
		ক্ষেত্র VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); বিরতি;
	    }
	    বিরতি;

	ক্ষেত্র VAR_SPECIAL:
	    নির্বাচন ((long)cur_val->vval.v_সংখ্যা)
	    {
		ক্ষেত্র VVAL_NONE: যদি ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    বিরতি;
				// FALLTHROUGH
		ক্ষেত্র VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); বিরতি;
	    }
	    বিরতি;

	ক্ষেত্র VAR_সংখ্যা:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((অক্ষর *)numbuf, আকার(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_সংখ্যা);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    বিরতি;

	ক্ষেত্র VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    বিরতি;

	ক্ষেত্র VAR_FUNC:
	ক্ষেত্র VAR_PARTIAL:
	ক্ষেত্র VAR_JOB:
	ক্ষেত্র VAR_CHANNEL:
	ক্ষেত্র VAR_INSTR:
	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_OBJECT:
	ক্ষেত্র VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    যাও theend;

	ক্ষেত্র VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    যদি (b == ফাঁকা || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    অন্যথা
	    {
		পূর্ণসংখ্যা	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		যদি (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    যাও theend;
		src = (char_u *)b->bv_ga.ga_তথ্য;
		dst = (char_u *)gap->ga_তথ্য + gap->ga_len;
		*dst++ = '[';
		জন্য (i = 0; i < blen; i++)
		{
		    পূর্ণসংখ্যা	    byte = src[i];

		    যদি (i > 0)
			*dst++ = ',';
		    যদি (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    অন্যথা যদি (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    অন্যথা
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (পূর্ণসংখ্যা)(dst - (char_u *)gap->ga_তথ্য);
	    }
	    বিরতি;

	ক্ষেত্র VAR_LIST:
	    l = cur_val->vval.v_list;
	    যদি (l == ফাঁকা)
		GA_CONCAT_LITERAL(gap, "[]");
	    অন্যথা
	    {
		যদি (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		অন্যথা
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    যদি (l->lv_first != ফাঁকা)
		    {
			যদি (স্তূপ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    যাও theend;
			}
			যদি (ga_grow(&স্তূপ, 1) == FAIL)
			    যাও theend;
			frame = ((json_enc_frame_T *)স্তূপ.ga_তথ্য)
							    + স্তূপ.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++স্তূপ.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			চালু;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    বিরতি;

	ক্ষেত্র VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    যদি (tuple == ফাঁকা)
		GA_CONCAT_LITERAL(gap, "[]");
	    অন্যথা
	    {
		যদি (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		অন্যথা
		{
		    পূর্ণসংখ্যা		len = TUPLE_LEN(tuple);

		    যদি (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			যদি (স্তূপ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    যাও theend;
			}
			যদি (ga_grow(&স্তূপ, 1) == FAIL)
			    যাও theend;
			frame = ((json_enc_frame_T *)স্তূপ.ga_তথ্য)
							    + স্তূপ.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++স্তূপ.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			চালু;
		    }
		    অন্যথা
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    বিরতি;

	ক্ষেত্র VAR_DICT:
	    d = cur_val->vval.v_dict;
	    যদি (d == ফাঁকা)
		GA_CONCAT_LITERAL(gap, "{}");
	    অন্যথা
	    {
		যদি (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		অন্যথা
		{
		    পূর্ণসংখ্যা		todo = (পূর্ণসংখ্যা)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    যদি (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			জন্য (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			যদি ((options & JSON_JS)
						 && is_simple_চাবি(hi->hi_চাবি))
			    ga_concat(gap, hi->hi_চাবি);
			অন্যথা
			    write_string(gap, hi->hi_চাবি);
			ga_append(gap, ':');

			যদি (স্তূপ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    যাও theend;
			}
			যদি (ga_grow(&স্তূপ, 1) == FAIL)
			    যাও theend;
			frame = ((json_enc_frame_T *)স্তূপ.ga_তথ্য)
							    + স্তূপ.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++স্তূপ.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			চালু;
		    }
		    অন্যথা
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    বিরতি;

	ক্ষেত্র VAR_FLOAT:
#যদি defined(HAVE_MATH_H)
	    যদি (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    অন্যথা যদি (isinf(cur_val->vval.v_float))
	    {
		যদি (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		অন্যথা
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    অন্যথা
#endif
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((অক্ষর *)numbuf, আকার(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    বিরতি;

	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    যাও theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	জন্য (;;)
	{
	    পূর্ণসংখ্যা		advance = FALSE;

	    যদি (স্তূপ.ga_len == 0)
	    {
		ga_clear(&স্তূপ);
		ফেরত OK;
	    }

	    frame = ((json_enc_frame_T *)স্তূপ.ga_তথ্য)
							+ স্তূপ.ga_len - 1;
	    নির্বাচন (frame->je_type)
	    {
		ক্ষেত্র ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    যদি ((frame->je_options & JSON_JS)
			    && li->li_পরবর্তী == ফাঁকা
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_সংখ্যা == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_পরবর্তী;
		    frame->je_u.l.li = li;
		    যদি (li != ফাঁকা && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			বিরতি;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --স্তূপ.ga_len;
		    বিরতি;
		}

		ক্ষেত্র ENC_TUPLE:
		{
		    পূর্ণসংখ্যা	    idx = frame->je_u.t.idx;
		    পূর্ণসংখ্যা	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    যদি ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			যদি (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_সংখ্যা == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    যদি (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			বিরতি;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --স্তূপ.ga_len;
		    বিরতি;
		}

		ক্ষেত্র ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    পূর্ণসংখ্যা		todo = frame->je_u.d.todo;

		    যদি (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			জন্য (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			যদি ((frame->je_options & JSON_JS)
				&& is_simple_চাবি(hi->hi_চাবি))
			    ga_concat(gap, hi->hi_চাবি);
			অন্যথা
			    write_string(gap, hi->hi_চাবি);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			বিরতি;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --স্তূপ.ga_len;
		    বিরতি;
		}
	    }
	    যদি (advance)
		বিরতি;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    জন্য (i = 0; i < স্তূপ.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)স্তূপ.ga_তথ্য) + i;
	নির্বাচন (frame->je_type)
	{
	    ক্ষেত্র ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		বিরতি;
	    ক্ষেত্র ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		বিরতি;
	    ক্ষেত্র ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		বিরতি;
	}
    }
    ga_clear(&স্তূপ);
    ফেরত FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    স্থির শূন্য
fill_numbuflen(js_read_T *reader)
{
    যদি (reader->js_fill != ফাঁকা && (পূর্ণসংখ্যা)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	যদি (reader->js_fill(reader))
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    স্থির শূন্য
json_skip_white(js_read_T *reader)
{
    পূর্ণসংখ্যা c;

    জন্য (;;)
    {
	c = reader->js_buf[reader->js_used];
	যদি (reader->js_fill != ফাঁকা && c == NUL)
	{
	    যদি (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		চালু;
	    }
	}
	যদি (c == NUL || c > ' ')
	    বিরতি;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    স্থির পূর্ণসংখ্যা
json_decode_string(js_read_T *reader, typval_T *res, পূর্ণসংখ্যা quote)
{
    garray_T    ga;
    পূর্ণসংখ্যা		len;
    char_u	*p;
    পূর্ণসংখ্যা		c;
    varnumber_T	nr;

    যদি (res != ফাঁকা)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    যখন (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	যদি (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    যদি (reader->js_fill == ফাঁকা)
		বিরতি;
	    len = (পূর্ণসংখ্যা)(reader->js_end - p);
	    reader->js_used = (পূর্ণসংখ্যা)(p - reader->js_buf);
	    যদি (!reader->js_fill(reader))
		বিরতি; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    চালু;
	}

	যদি (*p == '\\')
	{
	    c = -1;
	    নির্বাচন (p[1])
	    {
		ক্ষেত্র '\\': c = '\\'; বিরতি;
		ক্ষেত্র '"': c = '"'; বিরতি;
		ক্ষেত্র 'b': c = BS; বিরতি;
		ক্ষেত্র 't': c = TAB; বিরতি;
		ক্ষেত্র 'n': c = NL; বিরতি;
		ক্ষেত্র 'f': c = FF; বিরতি;
		ক্ষেত্র 'r': c = CAR; বিরতি;
		ক্ষেত্র 'u':
		    যদি (reader->js_fill != ফাঁকা
				     && (পূর্ণসংখ্যা)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (পূর্ণসংখ্যা)(p - reader->js_buf);
			যদি (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, ফাঁকা, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, ফাঁকা, 4, TRUE, ফাঁকা);
		    যদি (len == 0)
		    {
			যদি (res != ফাঁকা)
			    ga_clear(&ga);
			ফেরত FAIL;
		    }
		    p += len + 2;
		    যদি (0xd800 <= nr && nr <= 0xdbff
			    && (পূর্ণসংখ্যা)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, ফাঁকা, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, ফাঁকা, 4, TRUE, ফাঁকা);
			যদি (len == 0)
			{
			    যদি (res != ফাঁকা)
				ga_clear(&ga);
			    ফেরত FAIL;
			}
			যদি (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    যদি (0xd800 <= nr && nr <= 0xdfff)
		    {
			যদি (res != ফাঁকা)
			    ga_clear(&ga);
			ফেরত FAIL;
		    }
		    যদি (res != ফাঁকা)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((পূর্ণসংখ্যা)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    বিরতি;
		পূর্বনির্ধারিত:
		    // not a special char, skip over backslash
		    ++p;
		    চালু;
	    }
	    যদি (c > 0)
	    {
		p += 2;
		যদি (res != ফাঁকা)
		    ga_append(&ga, c);
	    }
	}
	অন্যথা
	{
	    len = utf_ptr2len(p);
	    যদি (res != ফাঁকা)
	    {
		যদি (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    ফেরত FAIL;
		}
		mch_memmove((অক্ষর *)ga.ga_তথ্য + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (পূর্ণসংখ্যা)(p - reader->js_buf);
    যদি (*p == quote)
    {
	++reader->js_used;
	যদি (res != ফাঁকা)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#যদি defined(USE_ICONV)
	    যদি (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		যদি (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_তথ্য, ফাঁকা);
		    vim_free(ga.ga_তথ্য);
		}
		convert_setup(&conv, ফাঁকা, ফাঁকা);
	    }
	    অন্যথা
#endif
		res->vval.v_string = ga.ga_তথ্য;
	}
	ফেরত OK;
    }
    যদি (res != ফাঁকা)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_সংখ্যা = VVAL_NONE;
	ga_clear(&ga);
    }
    ফেরত MAYBE;
}

প্রকারলেখ গণনা {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_চাবি,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

প্রকারলেখ কাঠামো {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_চাবি_tv;
    char_u	  *jd_চাবি;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    স্থির পূর্ণসংখ্যা
json_decode_item(js_read_T *reader, typval_T *res, পূর্ণসংখ্যা options)
{
    char_u	*p;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		retval;
    garray_T	স্তূপ;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *শীর্ষ_item;
    char_u	চাবি_buf[NUMBUFLEN];

    ga_init2(&স্তূপ, আকার(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    যদি (res != ফাঁকা)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    জন্য (;;)
    {
	শীর্ষ_item = ফাঁকা;
	যদি (স্তূপ.ga_len > 0)
	{
	    শীর্ষ_item = ((json_dec_item_T *)স্তূপ.ga_তথ্য) + স্তূপ.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    যদি (*p == NUL)
	    {
		retval = MAYBE;
		যাও theend;
	    }
	    যদি (শীর্ষ_item->jd_type == JSON_OBJECT_চাবি
					    || শীর্ষ_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		যদি (*p == (শীর্ষ_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --স্তূপ.ga_len;
		    যদি (স্তূপ.ga_len == 0)
		    {
			retval = OK;
			যাও theend;
		    }
		    যদি (cur_item != ফাঁকা)
			cur_item = &শীর্ষ_item->jd_tv;
		    যাও item_end;
		}
	    }
	}

	যদি (শীর্ষ_item != ফাঁকা && শীর্ষ_item->jd_type == JSON_OBJECT_চাবি
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *চাবি;

	    // accept an object key that is not in quotes
	    চাবি = p = reader->js_buf + reader->js_used;
	    যখন (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    যদি (cur_item != ফাঁকা)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(চাবি, p - চাবি);
		শীর্ষ_item->jd_চাবি = cur_item->vval.v_string;
	    }
	    reader->js_used += (পূর্ণসংখ্যা)(p - চাবি);
	}
	অন্যথা
	{
	    নির্বাচন (*p)
	    {
		ক্ষেত্র '[': // start of array
		    যদি (শীর্ষ_item && শীর্ষ_item->jd_type == JSON_OBJECT_চাবি)
		    {
			retval = FAIL;
			বিরতি;
		    }
		    যদি (স্তূপ.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			বিরতি;
		    }
		    যদি (ga_grow(&স্তূপ, 1) == FAIL)
		    {
			retval = FAIL;
			বিরতি;
		    }
		    যদি (cur_item != ফাঁকা && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_সংখ্যা = VVAL_NONE;
			retval = FAIL;
			বিরতি;
		    }

		    ++reader->js_used; // consume the '['
		    শীর্ষ_item = ((json_dec_item_T *)স্তূপ.ga_তথ্য)
								+ স্তূপ.ga_len;
		    শীর্ষ_item->jd_type = JSON_ARRAY;
		    ++স্তূপ.ga_len;
		    যদি (cur_item != ফাঁকা)
		    {
			শীর্ষ_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    চালু;

		ক্ষেত্র '{': // start of object
		    যদি (শীর্ষ_item && শীর্ষ_item->jd_type == JSON_OBJECT_চাবি)
		    {
			retval = FAIL;
			বিরতি;
		    }
		    যদি (স্তূপ.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			বিরতি;
		    }
		    যদি (ga_grow(&স্তূপ, 1) == FAIL)
		    {
			retval = FAIL;
			বিরতি;
		    }
		    যদি (cur_item != ফাঁকা && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_সংখ্যা = VVAL_NONE;
			retval = FAIL;
			বিরতি;
		    }

		    ++reader->js_used; // consume the '{'
		    শীর্ষ_item = ((json_dec_item_T *)স্তূপ.ga_তথ্য)
								+ স্তূপ.ga_len;
		    শীর্ষ_item->jd_type = JSON_OBJECT_চাবি;
		    ++স্তূপ.ga_len;
		    যদি (cur_item != ফাঁকা)
		    {
			শীর্ষ_item->jd_tv = *cur_item;
			cur_item = &শীর্ষ_item->jd_চাবি_tv;
		    }
		    চালু;

		ক্ষেত্র '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    বিরতি;

		ক্ষেত্র '\'':
		    যদি (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    অন্যথা
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    বিরতি;

		ক্ষেত্র ',': // comma: empty item
		    যদি ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			বিরতি;
		    }
		    // FALLTHROUGH
		ক্ষেত্র NUL: // empty
		    যদি (cur_item != ফাঁকা)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_সংখ্যা = VVAL_NONE;
		    }
		    retval = OK;
		    বিরতি;

		পূর্বনির্ধারিত:
		    যদি (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			যদি (*sp == '-')
			{
			    ++sp;
			    যদি (*sp == NUL)
			    {
				retval = MAYBE;
				বিরতি;
			    }
			    যদি (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				বিরতি;
			    }
			}
			sp = skipdigits(sp);
			যদি (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    যদি (cur_item == ফাঁকা)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    অন্যথা
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			অন্যথা
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    ফাঁকা, &len, 0, // what
				    &nr, ফাঁকা, 0, TRUE, ফাঁকা);
			    যদি (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				যাও theend;
			    }
			    যদি (cur_item != ফাঁকা)
			    {
				cur_item->v_type = VAR_সংখ্যা;
				cur_item->vval.v_সংখ্যা = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			বিরতি;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((অক্ষর *)(p), (kw), (len)) == 0 \
     : STRNCMP((অক্ষর *)(p), (kw), (len)) == 0)
		    যদি (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			যদি (cur_item != ফাঁকা)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_সংখ্যা = VVAL_FALSE;
			}
			retval = OK;
			বিরতি;
		    }
		    যদি (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			যদি (cur_item != ফাঁকা)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_সংখ্যা = VVAL_TRUE;
			}
			retval = OK;
			বিরতি;
		    }
		    যদি (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			যদি (cur_item != ফাঁকা)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_সংখ্যা = VVAL_NULL;
			}
			retval = OK;
			বিরতি;
		    }
		    যদি (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			যদি (cur_item != ফাঁকা)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			বিরতি;
		    }
		    যদি (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			যদি (cur_item != ফাঁকা)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			বিরতি;
		    }
		    যদি (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			যদি (cur_item != ফাঁকা)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			বিরতি;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (পূর্ণসংখ্যা)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    যদি (
			    (len < 5 && STRNICMP((অক্ষর *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((অক্ষর *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((অক্ষর *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((অক্ষর *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((অক্ষর *)p, "true", len) == 0
				    || STRNICMP((অক্ষর *)p, "null", len) == 0)))

			retval = MAYBE;
		    অন্যথা
			retval = FAIL;
		    বিরতি;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    যদি (retval == FAIL)
		বিরতি;
	    যদি (retval == MAYBE || স্তূপ.ga_len == 0)
		যাও theend;

	    যদি (শীর্ষ_item != ফাঁকা && শীর্ষ_item->jd_type == JSON_OBJECT_চাবি
		    && cur_item != ফাঁকা)
	    {
		যদি (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    যাও theend;
		}
		শীর্ষ_item->jd_চাবি = tv_get_string_buf_chk(cur_item, চাবি_buf);
		যদি (শীর্ষ_item->jd_চাবি == ফাঁকা)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    যাও theend;
		}
	    }
	}

item_end:
	শীর্ষ_item = ((json_dec_item_T *)স্তূপ.ga_তথ্য) + স্তূপ.ga_len - 1;
	নির্বাচন (শীর্ষ_item->jd_type)
	{
	    ক্ষেত্র JSON_ARRAY:
		যদি (res != ফাঁকা)
		{
		    listitem_T	*li = listitem_alloc();

		    যদি (li == ফাঁকা)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			যাও theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(শীর্ষ_item->jd_tv.vval.v_list, li);
		}
		যদি (cur_item != ফাঁকা)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		যদি (*p == ',')
		    ++reader->js_used;
		অন্যথা যদি (*p != ']')
		{
		    যদি (*p == NUL)
			retval = MAYBE;
		    অন্যথা
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    যাও theend;
		}
		বিরতি;

	    ক্ষেত্র JSON_OBJECT_চাবি:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		যদি (*p != ':')
		{
		    যদি (cur_item != ফাঁকা)
			clear_tv(cur_item);
		    যদি (*p == NUL)
			retval = MAYBE;
		    অন্যথা
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    যাও theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		শীর্ষ_item->jd_type = JSON_OBJECT;
		যদি (cur_item != ফাঁকা)
		    cur_item = &item;
		বিরতি;

	    ক্ষেত্র JSON_OBJECT:
		যদি (cur_item != ফাঁকা
			&& dict_has_চাবি(শীর্ষ_item->jd_tv.vval.v_dict,
						(অক্ষর *)শীর্ষ_item->jd_চাবি))
		{
		    semsg(_(e_duplicate_চাবি_in_json_str), শীর্ষ_item->jd_চাবি);
		    clear_tv(cur_item);
		    retval = FAIL;
		    যাও theend;
		}

		যদি (cur_item != ফাঁকা)
		{
		    dictitem_T *di = dictitem_alloc(শীর্ষ_item->jd_চাবি);

		    clear_tv(&শীর্ষ_item->jd_চাবি_tv);
		    যদি (di == ফাঁকা)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			যাও theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    যদি (dict_add(শীর্ষ_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			যাও theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		যদি (*p == ',')
		    ++reader->js_used;
		অন্যথা যদি (*p != '}')
		{
		    যদি (*p == NUL)
			retval = MAYBE;
		    অন্যথা
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    যাও theend;
		}
		শীর্ষ_item->jd_type = JSON_OBJECT_চাবি;
		যদি (cur_item != ফাঁকা)
		    cur_item = &শীর্ষ_item->jd_চাবি_tv;
		বিরতি;
	}
    }

    // Get here when parsing failed.
    যদি (res != ফাঁকা)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_সংখ্যা = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    জন্য (i = 0; i < স্তূপ.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)স্তূপ.ga_তথ্য) + i)->jd_চাবি_tv);
    ga_clear(&স্তূপ);

    ফেরত retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    স্থির পূর্ণসংখ্যা
json_decode_all(js_read_T *reader, typval_T *res, পূর্ণসংখ্যা options)
{
    পূর্ণসংখ্যা ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    যদি (ret != OK)
    {
	যদি (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	ফেরত FAIL;
    }
    json_skip_white(reader);
    যদি (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	ফেরত FAIL;
    }
    ফেরত OK;
}

#যদি defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    পূর্ণসংখ্যা
json_decode(js_read_T *reader, typval_T *res, পূর্ণসংখ্যা options)
{
    পূর্ণসংখ্যা ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    ফেরত ret;
}
#endif

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
    পূর্ণসংখ্যা
json_find_end(js_read_T *reader, পূর্ণসংখ্যা options)
{
    পূর্ণসংখ্যা used_save = reader->js_used;
    পূর্ণসংখ্যা ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, ফাঁকা, options);
    reader->js_used = used_save;
    ফেরত ret;
}

/*
 * "js_decode()" function
 */
    শূন্য
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ফাঁকা;
    reader.js_used = 0;
    যদি (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_argument));
}

/*
 * "js_encode()" function
 */
    শূন্য
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    শূন্য
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ফাঁকা;
    reader.js_used = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    শূন্য
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#endif
