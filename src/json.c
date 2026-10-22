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

স্থির পূর্ণাঙ্ক json_encode_item(garray_T *gap, typval_T *val, পূর্ণাঙ্ক copyID, পূর্ণাঙ্ক options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    স্থির পূর্ণাঙ্ক
json_encode_gap(garray_T *gap, typval_T *val, পূর্ণাঙ্ক options)
{
    যদি (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, পূর্ণাঙ্ক options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    যদি (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    প্রত্যাগম ga.ga_data;
}

#যদি defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_রাশিমালা(পূর্ণাঙ্ক nr, typval_T *val, পূর্ণাঙ্ক options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_সংখ্যা;
    nrtv.vval.v_সংখ্যা = nr;
    যদি (rettv_list_alloc(&listtv) == FAIL)
	প্রত্যাগম রিক্ত;
    যদি (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	প্রত্যাগম রিক্ত;
    }

    ga_init2(&ga, 1, 4000);
    যদি (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    প্রত্যাগম ga.ga_data;
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
	প্রত্যাগম রিক্ত;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((বর্ণ *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    প্রত্যাগম lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
স্থির ধ্রুব বর্ণ ascii_needs_escape[128] = {
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
    char_u	*converted = রিক্ত;
#endif
    পূর্ণাঙ্ক		c;

    যদি (res == রিক্ত)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	প্রত্যাগম;
    }

#যদি defined(USE_ICONV)
    যদি (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	যদি (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, রিক্ত);
	convert_setup(&conv, রিক্ত, রিক্ত);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (পূর্ণাঙ্ক)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    যাবত্ ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	যদি (c < 0x80)
	{
	    যদি (!ascii_needs_escape[c])
	    {
		res += 1;
		অনুবর্ত;
	    }

	    যদি (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    বিকল্প (c)
	    {
		পক্ষ 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); ভঙ্গ;
		পক্ষ 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); ভঙ্গ;
		পক্ষ 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); ভঙ্গ;
		পক্ষ 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); ভঙ্গ;
		পক্ষ 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); ভঙ্গ;
		পক্ষ 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); ভঙ্গ;
		পক্ষ 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); ভঙ্গ;
		স্বভাব:
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
	    পূর্ণাঙ্ক	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    যদি (l > 1)
	    {
		res += l;
		অনুবর্ত;
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
    স্থির পূর্ণাঙ্ক
is_simple_চাবি(char_u *চাবি)
{
    char_u *p;

    যদি (!ASCII_ISALPHA(*চাবি))
	প্রত্যাগম FALSE;
    প্রতি (p = চাবি + 1; *p != NUL; ++p)
	যদি (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    প্রত্যাগম FALSE;
    প্রত্যাগম TRUE;
}

প্রকারনির্দেশ গণনা {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

প্রকারনির্দেশ সংরচনা {
    json_enc_type_T je_type;
    পূর্ণাঙ্ক		    je_options;	    // options when this container was entered
    সঙ্ঘ {
	সংরচনা {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	সংরচনা {
	    tuple_T	*tuple;
	    পূর্ণাঙ্ক		idx;	    // current index
	    পূর্ণাঙ্ক		len;
	} t;
	সংরচনা {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    পূর্ণাঙ্ক		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    স্থির পূর্ণাঙ্ক
json_encode_item(garray_T *gap, typval_T *val, পূর্ণাঙ্ক copyID, পূর্ণাঙ্ক options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    পূর্ণাঙ্ক			i;
    garray_T		স্তূপ;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&স্তূপ, পরিমাণ(json_enc_frame_T), 100);
    cur_val = val;

    প্রতি (;;)
    {
    বিকল্প (cur_val->v_type)
    {
	পক্ষ VAR_BOOL:
	    বিকল্প ((long)cur_val->vval.v_সংখ্যা)
	    {
		পক্ষ VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); ভঙ্গ;
		পক্ষ VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); ভঙ্গ;
	    }
	    ভঙ্গ;

	পক্ষ VAR_SPECIAL:
	    বিকল্প ((long)cur_val->vval.v_সংখ্যা)
	    {
		পক্ষ VVAL_NONE: যদি ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    ভঙ্গ;
				// FALLTHROUGH
		পক্ষ VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); ভঙ্গ;
	    }
	    ভঙ্গ;

	পক্ষ VAR_সংখ্যা:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((বর্ণ *)numbuf, পরিমাণ(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_সংখ্যা);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ভঙ্গ;

	পক্ষ VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    ভঙ্গ;

	পক্ষ VAR_FUNC:
	পক্ষ VAR_PARTIAL:
	পক্ষ VAR_JOB:
	পক্ষ VAR_CHANNEL:
	পক্ষ VAR_INSTR:
	পক্ষ VAR_CLASS:
	পক্ষ VAR_OBJECT:
	পক্ষ VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    গচ্ছ theend;

	পক্ষ VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    যদি (b == রিক্ত || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    অন্যথা
	    {
		পূর্ণাঙ্ক	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		যদি (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    গচ্ছ theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		প্রতি (i = 0; i < blen; i++)
		{
		    পূর্ণাঙ্ক	    byte = src[i];

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
		gap->ga_len = (পূর্ণাঙ্ক)(dst - (char_u *)gap->ga_data);
	    }
	    ভঙ্গ;

	পক্ষ VAR_LIST:
	    l = cur_val->vval.v_list;
	    যদি (l == রিক্ত)
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
		    যদি (l->lv_first != রিক্ত)
		    {
			যদি (স্তূপ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    গচ্ছ theend;
			}
			যদি (ga_grow(&স্তূপ, 1) == FAIL)
			    গচ্ছ theend;
			frame = ((json_enc_frame_T *)স্তূপ.ga_data)
							    + স্তূপ.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++স্তূপ.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			অনুবর্ত;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    ভঙ্গ;

	পক্ষ VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    যদি (tuple == রিক্ত)
		GA_CONCAT_LITERAL(gap, "[]");
	    অন্যথা
	    {
		যদি (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		অন্যথা
		{
		    পূর্ণাঙ্ক		len = TUPLE_LEN(tuple);

		    যদি (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			যদি (স্তূপ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    গচ্ছ theend;
			}
			যদি (ga_grow(&স্তূপ, 1) == FAIL)
			    গচ্ছ theend;
			frame = ((json_enc_frame_T *)স্তূপ.ga_data)
							    + স্তূপ.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++স্তূপ.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			অনুবর্ত;
		    }
		    অন্যথা
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    ভঙ্গ;

	পক্ষ VAR_DICT:
	    d = cur_val->vval.v_dict;
	    যদি (d == রিক্ত)
		GA_CONCAT_LITERAL(gap, "{}");
	    অন্যথা
	    {
		যদি (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		অন্যথা
		{
		    পূর্ণাঙ্ক		todo = (পূর্ণাঙ্ক)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    যদি (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			প্রতি (hi = d->dv_hashtab.ht_array;
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
			    গচ্ছ theend;
			}
			যদি (ga_grow(&স্তূপ, 1) == FAIL)
			    গচ্ছ theend;
			frame = ((json_enc_frame_T *)স্তূপ.ga_data)
							    + স্তূপ.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++স্তূপ.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			অনুবর্ত;
		    }
		    অন্যথা
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    ভঙ্গ;

	পক্ষ VAR_FLOAT:
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

		numbuflen = vim_snprintf_safelen((বর্ণ *)numbuf, পরিমাণ(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ভঙ্গ;

	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    গচ্ছ theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	প্রতি (;;)
	{
	    পূর্ণাঙ্ক		advance = FALSE;

	    যদি (স্তূপ.ga_len == 0)
	    {
		ga_clear(&স্তূপ);
		প্রত্যাগম OK;
	    }

	    frame = ((json_enc_frame_T *)স্তূপ.ga_data)
							+ স্তূপ.ga_len - 1;
	    বিকল্প (frame->je_type)
	    {
		পক্ষ ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    যদি ((frame->je_options & JSON_JS)
			    && li->li_next == রিক্ত
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_সংখ্যা == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    যদি (li != রিক্ত && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			ভঙ্গ;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --স্তূপ.ga_len;
		    ভঙ্গ;
		}

		পক্ষ ENC_TUPLE:
		{
		    পূর্ণাঙ্ক	    idx = frame->je_u.t.idx;
		    পূর্ণাঙ্ক	    len = frame->je_u.t.len;

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
			ভঙ্গ;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --স্তূপ.ga_len;
		    ভঙ্গ;
		}

		পক্ষ ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    পূর্ণাঙ্ক		todo = frame->je_u.d.todo;

		    যদি (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			প্রতি (++hi; HASHITEM_EMPTY(hi); ++hi)
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
			ভঙ্গ;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --স্তূপ.ga_len;
		    ভঙ্গ;
		}
	    }
	    যদি (advance)
		ভঙ্গ;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    প্রতি (i = 0; i < স্তূপ.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)স্তূপ.ga_data) + i;
	বিকল্প (frame->je_type)
	{
	    পক্ষ ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		ভঙ্গ;
	    পক্ষ ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		ভঙ্গ;
	    পক্ষ ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		ভঙ্গ;
	}
    }
    ga_clear(&স্তূপ);
    প্রত্যাগম FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    স্থির শূন্য
fill_numbuflen(js_read_T *reader)
{
    যদি (reader->js_fill != রিক্ত && (পূর্ণাঙ্ক)(reader->js_end - reader->js_buf)
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
    পূর্ণাঙ্ক c;

    প্রতি (;;)
    {
	c = reader->js_buf[reader->js_used];
	যদি (reader->js_fill != রিক্ত && c == NUL)
	{
	    যদি (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		অনুবর্ত;
	    }
	}
	যদি (c == NUL || c > ' ')
	    ভঙ্গ;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    স্থির পূর্ণাঙ্ক
json_decode_string(js_read_T *reader, typval_T *res, পূর্ণাঙ্ক quote)
{
    garray_T    ga;
    পূর্ণাঙ্ক		len;
    char_u	*p;
    পূর্ণাঙ্ক		c;
    varnumber_T	nr;

    যদি (res != রিক্ত)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    যাবত্ (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	যদি (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    যদি (reader->js_fill == রিক্ত)
		ভঙ্গ;
	    len = (পূর্ণাঙ্ক)(reader->js_end - p);
	    reader->js_used = (পূর্ণাঙ্ক)(p - reader->js_buf);
	    যদি (!reader->js_fill(reader))
		ভঙ্গ; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    অনুবর্ত;
	}

	যদি (*p == '\\')
	{
	    c = -1;
	    বিকল্প (p[1])
	    {
		পক্ষ '\\': c = '\\'; ভঙ্গ;
		পক্ষ '"': c = '"'; ভঙ্গ;
		পক্ষ 'b': c = BS; ভঙ্গ;
		পক্ষ 't': c = TAB; ভঙ্গ;
		পক্ষ 'n': c = NL; ভঙ্গ;
		পক্ষ 'f': c = FF; ভঙ্গ;
		পক্ষ 'r': c = CAR; ভঙ্গ;
		পক্ষ 'u':
		    যদি (reader->js_fill != রিক্ত
				     && (পূর্ণাঙ্ক)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (পূর্ণাঙ্ক)(p - reader->js_buf);
			যদি (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, রিক্ত, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, রিক্ত, 4, TRUE, রিক্ত);
		    যদি (len == 0)
		    {
			যদি (res != রিক্ত)
			    ga_clear(&ga);
			প্রত্যাগম FAIL;
		    }
		    p += len + 2;
		    যদি (0xd800 <= nr && nr <= 0xdbff
			    && (পূর্ণাঙ্ক)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, রিক্ত, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, রিক্ত, 4, TRUE, রিক্ত);
			যদি (len == 0)
			{
			    যদি (res != রিক্ত)
				ga_clear(&ga);
			    প্রত্যাগম FAIL;
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
			যদি (res != রিক্ত)
			    ga_clear(&ga);
			প্রত্যাগম FAIL;
		    }
		    যদি (res != রিক্ত)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((পূর্ণাঙ্ক)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    ভঙ্গ;
		স্বভাব:
		    // not a special char, skip over backslash
		    ++p;
		    অনুবর্ত;
	    }
	    যদি (c > 0)
	    {
		p += 2;
		যদি (res != রিক্ত)
		    ga_append(&ga, c);
	    }
	}
	অন্যথা
	{
	    len = utf_ptr2len(p);
	    যদি (res != রিক্ত)
	    {
		যদি (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    প্রত্যাগম FAIL;
		}
		mch_memmove((বর্ণ *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (পূর্ণাঙ্ক)(p - reader->js_buf);
    যদি (*p == quote)
    {
	++reader->js_used;
	যদি (res != রিক্ত)
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
				      string_convert(&conv, ga.ga_data, রিক্ত);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, রিক্ত, রিক্ত);
	    }
	    অন্যথা
#endif
		res->vval.v_string = ga.ga_data;
	}
	প্রত্যাগম OK;
    }
    যদি (res != রিক্ত)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_সংখ্যা = VVAL_NONE;
	ga_clear(&ga);
    }
    প্রত্যাগম MAYBE;
}

প্রকারনির্দেশ গণনা {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_চাবি,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

প্রকারনির্দেশ সংরচনা {
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
    স্থির পূর্ণাঙ্ক
json_decode_item(js_read_T *reader, typval_T *res, পূর্ণাঙ্ক options)
{
    char_u	*p;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		retval;
    garray_T	স্তূপ;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *শীর্ষ_item;
    char_u	চাবি_buf[NUMBUFLEN];

    ga_init2(&স্তূপ, পরিমাণ(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    যদি (res != রিক্ত)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    প্রতি (;;)
    {
	শীর্ষ_item = রিক্ত;
	যদি (স্তূপ.ga_len > 0)
	{
	    শীর্ষ_item = ((json_dec_item_T *)স্তূপ.ga_data) + স্তূপ.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    যদি (*p == NUL)
	    {
		retval = MAYBE;
		গচ্ছ theend;
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
			গচ্ছ theend;
		    }
		    যদি (cur_item != রিক্ত)
			cur_item = &শীর্ষ_item->jd_tv;
		    গচ্ছ item_end;
		}
	    }
	}

	যদি (শীর্ষ_item != রিক্ত && শীর্ষ_item->jd_type == JSON_OBJECT_চাবি
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *চাবি;

	    // accept an object key that is not in quotes
	    চাবি = p = reader->js_buf + reader->js_used;
	    যাবত্ (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    যদি (cur_item != রিক্ত)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(চাবি, p - চাবি);
		শীর্ষ_item->jd_চাবি = cur_item->vval.v_string;
	    }
	    reader->js_used += (পূর্ণাঙ্ক)(p - চাবি);
	}
	অন্যথা
	{
	    বিকল্প (*p)
	    {
		পক্ষ '[': // start of array
		    যদি (শীর্ষ_item && শীর্ষ_item->jd_type == JSON_OBJECT_চাবি)
		    {
			retval = FAIL;
			ভঙ্গ;
		    }
		    যদি (স্তূপ.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ভঙ্গ;
		    }
		    যদি (ga_grow(&স্তূপ, 1) == FAIL)
		    {
			retval = FAIL;
			ভঙ্গ;
		    }
		    যদি (cur_item != রিক্ত && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_সংখ্যা = VVAL_NONE;
			retval = FAIL;
			ভঙ্গ;
		    }

		    ++reader->js_used; // consume the '['
		    শীর্ষ_item = ((json_dec_item_T *)স্তূপ.ga_data)
								+ স্তূপ.ga_len;
		    শীর্ষ_item->jd_type = JSON_ARRAY;
		    ++স্তূপ.ga_len;
		    যদি (cur_item != রিক্ত)
		    {
			শীর্ষ_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    অনুবর্ত;

		পক্ষ '{': // start of object
		    যদি (শীর্ষ_item && শীর্ষ_item->jd_type == JSON_OBJECT_চাবি)
		    {
			retval = FAIL;
			ভঙ্গ;
		    }
		    যদি (স্তূপ.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ভঙ্গ;
		    }
		    যদি (ga_grow(&স্তূপ, 1) == FAIL)
		    {
			retval = FAIL;
			ভঙ্গ;
		    }
		    যদি (cur_item != রিক্ত && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_সংখ্যা = VVAL_NONE;
			retval = FAIL;
			ভঙ্গ;
		    }

		    ++reader->js_used; // consume the '{'
		    শীর্ষ_item = ((json_dec_item_T *)স্তূপ.ga_data)
								+ স্তূপ.ga_len;
		    শীর্ষ_item->jd_type = JSON_OBJECT_চাবি;
		    ++স্তূপ.ga_len;
		    যদি (cur_item != রিক্ত)
		    {
			শীর্ষ_item->jd_tv = *cur_item;
			cur_item = &শীর্ষ_item->jd_চাবি_tv;
		    }
		    অনুবর্ত;

		পক্ষ '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    ভঙ্গ;

		পক্ষ '\'':
		    যদি (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    অন্যথা
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ভঙ্গ;

		পক্ষ ',': // comma: empty item
		    যদি ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			ভঙ্গ;
		    }
		    // FALLTHROUGH
		পক্ষ NUL: // empty
		    যদি (cur_item != রিক্ত)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_সংখ্যা = VVAL_NONE;
		    }
		    retval = OK;
		    ভঙ্গ;

		স্বভাব:
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
				ভঙ্গ;
			    }
			    যদি (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				ভঙ্গ;
			    }
			}
			sp = skipdigits(sp);
			যদি (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    যদি (cur_item == রিক্ত)
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
				    রিক্ত, &len, 0, // what
				    &nr, রিক্ত, 0, TRUE, রিক্ত);
			    যদি (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				গচ্ছ theend;
			    }
			    যদি (cur_item != রিক্ত)
			    {
				cur_item->v_type = VAR_সংখ্যা;
				cur_item->vval.v_সংখ্যা = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			ভঙ্গ;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((বর্ণ *)(p), (kw), (len)) == 0 \
     : STRNCMP((বর্ণ *)(p), (kw), (len)) == 0)
		    যদি (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			যদি (cur_item != রিক্ত)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_সংখ্যা = VVAL_FALSE;
			}
			retval = OK;
			ভঙ্গ;
		    }
		    যদি (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			যদি (cur_item != রিক্ত)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_সংখ্যা = VVAL_TRUE;
			}
			retval = OK;
			ভঙ্গ;
		    }
		    যদি (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			যদি (cur_item != রিক্ত)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_সংখ্যা = VVAL_NULL;
			}
			retval = OK;
			ভঙ্গ;
		    }
		    যদি (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			যদি (cur_item != রিক্ত)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			ভঙ্গ;
		    }
		    যদি (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			যদি (cur_item != রিক্ত)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			ভঙ্গ;
		    }
		    যদি (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			যদি (cur_item != রিক্ত)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			ভঙ্গ;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (পূর্ণাঙ্ক)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    যদি (
			    (len < 5 && STRNICMP((বর্ণ *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((বর্ণ *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((বর্ণ *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((বর্ণ *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((বর্ণ *)p, "true", len) == 0
				    || STRNICMP((বর্ণ *)p, "null", len) == 0)))

			retval = MAYBE;
		    অন্যথা
			retval = FAIL;
		    ভঙ্গ;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    যদি (retval == FAIL)
		ভঙ্গ;
	    যদি (retval == MAYBE || স্তূপ.ga_len == 0)
		গচ্ছ theend;

	    যদি (শীর্ষ_item != রিক্ত && শীর্ষ_item->jd_type == JSON_OBJECT_চাবি
		    && cur_item != রিক্ত)
	    {
		যদি (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    গচ্ছ theend;
		}
		শীর্ষ_item->jd_চাবি = tv_get_string_buf_chk(cur_item, চাবি_buf);
		যদি (শীর্ষ_item->jd_চাবি == রিক্ত)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    গচ্ছ theend;
		}
	    }
	}

item_end:
	শীর্ষ_item = ((json_dec_item_T *)স্তূপ.ga_data) + স্তূপ.ga_len - 1;
	বিকল্প (শীর্ষ_item->jd_type)
	{
	    পক্ষ JSON_ARRAY:
		যদি (res != রিক্ত)
		{
		    listitem_T	*li = listitem_alloc();

		    যদি (li == রিক্ত)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			গচ্ছ theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(শীর্ষ_item->jd_tv.vval.v_list, li);
		}
		যদি (cur_item != রিক্ত)
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
		    গচ্ছ theend;
		}
		ভঙ্গ;

	    পক্ষ JSON_OBJECT_চাবি:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		যদি (*p != ':')
		{
		    যদি (cur_item != রিক্ত)
			clear_tv(cur_item);
		    যদি (*p == NUL)
			retval = MAYBE;
		    অন্যথা
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    গচ্ছ theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		শীর্ষ_item->jd_type = JSON_OBJECT;
		যদি (cur_item != রিক্ত)
		    cur_item = &item;
		ভঙ্গ;

	    পক্ষ JSON_OBJECT:
		যদি (cur_item != রিক্ত
			&& dict_has_চাবি(শীর্ষ_item->jd_tv.vval.v_dict,
						(বর্ণ *)শীর্ষ_item->jd_চাবি))
		{
		    semsg(_(e_duplicate_চাবি_in_json_str), শীর্ষ_item->jd_চাবি);
		    clear_tv(cur_item);
		    retval = FAIL;
		    গচ্ছ theend;
		}

		যদি (cur_item != রিক্ত)
		{
		    dictitem_T *di = dictitem_alloc(শীর্ষ_item->jd_চাবি);

		    clear_tv(&শীর্ষ_item->jd_চাবি_tv);
		    যদি (di == রিক্ত)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			গচ্ছ theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    যদি (dict_add(শীর্ষ_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			গচ্ছ theend;
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
		    গচ্ছ theend;
		}
		শীর্ষ_item->jd_type = JSON_OBJECT_চাবি;
		যদি (cur_item != রিক্ত)
		    cur_item = &শীর্ষ_item->jd_চাবি_tv;
		ভঙ্গ;
	}
    }

    // Get here when parsing failed.
    যদি (res != রিক্ত)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_সংখ্যা = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    প্রতি (i = 0; i < স্তূপ.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)স্তূপ.ga_data) + i)->jd_চাবি_tv);
    ga_clear(&স্তূপ);

    প্রত্যাগম retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    স্থির পূর্ণাঙ্ক
json_decode_all(js_read_T *reader, typval_T *res, পূর্ণাঙ্ক options)
{
    পূর্ণাঙ্ক ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    যদি (ret != OK)
    {
	যদি (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	প্রত্যাগম FAIL;
    }
    json_skip_white(reader);
    যদি (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

#যদি defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    পূর্ণাঙ্ক
json_decode(js_read_T *reader, typval_T *res, পূর্ণাঙ্ক options)
{
    পূর্ণাঙ্ক ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    প্রত্যাগম ret;
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
    পূর্ণাঙ্ক
json_find_end(js_read_T *reader, পূর্ণাঙ্ক options)
{
    পূর্ণাঙ্ক used_save = reader->js_used;
    পূর্ণাঙ্ক ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, রিক্ত, options);
    reader->js_used = used_save;
    প্রত্যাগম ret;
}

/*
 * "js_decode()" function
 */
    শূন্য
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = রিক্ত;
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
	প্রত্যাগম;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = রিক্ত;
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
