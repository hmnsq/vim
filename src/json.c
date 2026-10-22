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

#ضمن "vim.h"

#ܐܢ defined(FEAT_EVAL)

ساكن صحيح json_encode_item(garray_T *gap, typval_T *val, صحيح copyID, صحيح options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    ساكن صحيح
json_encode_gap(garray_T *gap, typval_T *val, صحيح options)
{
    ܐܢ (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	ܦܢܐ FAIL;
    }
    ܦܢܐ OK;
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
    ܐܢ (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    ܦܢܐ ga.ga_data;
}

#ܐܢ defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_expr(صحيح nr, typval_T *val, صحيح options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_NUMBER;
    nrtv.vval.v_number = nr;
    ܐܢ (rettv_list_alloc(&listtv) == FAIL)
	ܦܢܐ عدم;
    ܐܢ (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	ܦܢܐ عدم;
    }

    ga_init2(&ga, 1, 4000);
    ܐܢ (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    ܦܢܐ ga.ga_data;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_msg(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    ܐܢ (json_encode_gap(&ga, val, 0) == FAIL)
	ܦܢܐ عدم;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    ܦܢܐ lspga.ga_data;
}
#endif

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
#ܐܢ defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = عدم;
#endif
    صحيح		c;

    ܐܢ (res == عدم)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	ܦܢܐ;
    }

#ܐܢ defined(USE_ICONV)
    ܐܢ (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	ܐܢ (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, عدم);
	convert_setup(&conv, عدم, عدم);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (صحيح)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    ܟܡܐ ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	ܐܢ (c < 0x80)
	{
	    ܐܢ (!ascii_needs_escape[c])
	    {
		res += 1;
		استمر;
	    }

	    ܐܢ (res != from)
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
	ܐܚܪܢܐ
	{
	    صحيح	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    ܐܢ (l > 1)
	    {
		res += l;
		استمر;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    ܐܢ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    ܐܢ (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#ܐܢ defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    ساكن صحيح
is_simple_key(char_u *key)
{
    char_u *p;

    ܐܢ (!ASCII_ISALPHA(*key))
	ܦܢܐ FALSE;
    لكل (p = key + 1; *p != NUL; ++p)
	ܐܢ (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    ܦܢܐ FALSE;
    ܦܢܐ TRUE;
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
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    ساكن صحيح
json_encode_item(garray_T *gap, typval_T *val, صحيح copyID, صحيح options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    صحيح			i;
    garray_T		stack;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&stack, حجم(json_enc_frame_T), 100);
    cur_val = val;

    لكل (;;)
    {
    بدل (cur_val->v_type)
    {
	حالة VAR_BOOL:
	    بدل ((long)cur_val->vval.v_number)
	    {
		حالة VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); اكسر;
		حالة VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); اكسر;
	    }
	    اكسر;

	حالة VAR_SPECIAL:
	    بدل ((long)cur_val->vval.v_number)
	    {
		حالة VVAL_NONE: ܐܢ ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    اكسر;
				// FALLTHROUGH
		حالة VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); اكسر;
	    }
	    اكسر;

	حالة VAR_NUMBER:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((حرف *)numbuf, حجم(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_number);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    اكسر;

	حالة VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    اكسر;

	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	حالة VAR_JOB:
	حالة VAR_CHANNEL:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	حالة VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    اذهب theend;

	حالة VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    ܐܢ (b == عدم || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    ܐܚܪܢܐ
	    {
		صحيح	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		ܐܢ (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    اذهب theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		لكل (i = 0; i < blen; i++)
		{
		    صحيح	    byte = src[i];

		    ܐܢ (i > 0)
			*dst++ = ',';
		    ܐܢ (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    ܐܚܪܢܐ ܐܢ (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    ܐܚܪܢܐ
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (صحيح)(dst - (char_u *)gap->ga_data);
	    }
	    اكسر;

	حالة VAR_LIST:
	    l = cur_val->vval.v_list;
	    ܐܢ (l == عدم)
		GA_CONCAT_LITERAL(gap, "[]");
	    ܐܚܪܢܐ
	    {
		ܐܢ (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ܐܚܪܢܐ
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    ܐܢ (l->lv_first != عدم)
		    {
			ܐܢ (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    اذهب theend;
			}
			ܐܢ (ga_grow(&stack, 1) == FAIL)
			    اذهب theend;
			frame = ((json_enc_frame_T *)stack.ga_data)
							    + stack.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++stack.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			استمر;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    اكسر;

	حالة VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    ܐܢ (tuple == عدم)
		GA_CONCAT_LITERAL(gap, "[]");
	    ܐܚܪܢܐ
	    {
		ܐܢ (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ܐܚܪܢܐ
		{
		    صحيح		len = TUPLE_LEN(tuple);

		    ܐܢ (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			ܐܢ (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    اذهب theend;
			}
			ܐܢ (ga_grow(&stack, 1) == FAIL)
			    اذهب theend;
			frame = ((json_enc_frame_T *)stack.ga_data)
							    + stack.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++stack.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			استمر;
		    }
		    ܐܚܪܢܐ
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    اكسر;

	حالة VAR_DICT:
	    d = cur_val->vval.v_dict;
	    ܐܢ (d == عدم)
		GA_CONCAT_LITERAL(gap, "{}");
	    ܐܚܪܢܐ
	    {
		ܐܢ (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		ܐܚܪܢܐ
		{
		    صحيح		todo = (صحيح)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    ܐܢ (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			لكل (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			ܐܢ ((options & JSON_JS)
						 && is_simple_key(hi->hi_key))
			    ga_concat(gap, hi->hi_key);
			ܐܚܪܢܐ
			    write_string(gap, hi->hi_key);
			ga_append(gap, ':');

			ܐܢ (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    اذهب theend;
			}
			ܐܢ (ga_grow(&stack, 1) == FAIL)
			    اذهب theend;
			frame = ((json_enc_frame_T *)stack.ga_data)
							    + stack.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++stack.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			استمر;
		    }
		    ܐܚܪܢܐ
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    اكسر;

	حالة VAR_FLOAT:
#ܐܢ defined(HAVE_MATH_H)
	    ܐܢ (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    ܐܚܪܢܐ ܐܢ (isinf(cur_val->vval.v_float))
	    {
		ܐܢ (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		ܐܚܪܢܐ
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    ܐܚܪܢܐ
#endif
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
	    internal_error_no_abort("json_encode_item()");
	    اذهب theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	لكل (;;)
	{
	    صحيح		advance = FALSE;

	    ܐܢ (stack.ga_len == 0)
	    {
		ga_clear(&stack);
		ܦܢܐ OK;
	    }

	    frame = ((json_enc_frame_T *)stack.ga_data)
							+ stack.ga_len - 1;
	    بدل (frame->je_type)
	    {
		حالة ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    ܐܢ ((frame->je_options & JSON_JS)
			    && li->li_next == عدم
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_number == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    ܐܢ (li != عدم && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			اكسر;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --stack.ga_len;
		    اكسر;
		}

		حالة ENC_TUPLE:
		{
		    صحيح	    idx = frame->je_u.t.idx;
		    صحيح	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    ܐܢ ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			ܐܢ (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_number == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    ܐܢ (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			اكسر;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --stack.ga_len;
		    اكسر;
		}

		حالة ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    صحيح		todo = frame->je_u.d.todo;

		    ܐܢ (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			لكل (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			ܐܢ ((frame->je_options & JSON_JS)
				&& is_simple_key(hi->hi_key))
			    ga_concat(gap, hi->hi_key);
			ܐܚܪܢܐ
			    write_string(gap, hi->hi_key);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			اكسر;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --stack.ga_len;
		    اكسر;
		}
	    }
	    ܐܢ (advance)
		اكسر;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    لكل (i = 0; i < stack.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)stack.ga_data) + i;
	بدل (frame->je_type)
	{
	    حالة ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		اكسر;
	    حالة ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		اكسر;
	    حالة ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		اكسر;
	}
    }
    ga_clear(&stack);
    ܦܢܐ FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    ساكن فراغ
fill_numbuflen(js_read_T *reader)
{
    ܐܢ (reader->js_fill != عدم && (صحيح)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	ܐܢ (reader->js_fill(reader))
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
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
	c = reader->js_buf[reader->js_used];
	ܐܢ (reader->js_fill != عدم && c == NUL)
	{
	    ܐܢ (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		استمر;
	    }
	}
	ܐܢ (c == NUL || c > ' ')
	    اكسر;
	++reader->js_used;
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

    ܐܢ (res != عدم)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    ܟܡܐ (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	ܐܢ (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    ܐܢ (reader->js_fill == عدم)
		اكسر;
	    len = (صحيح)(reader->js_end - p);
	    reader->js_used = (صحيح)(p - reader->js_buf);
	    ܐܢ (!reader->js_fill(reader))
		اكسر; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    استمر;
	}

	ܐܢ (*p == '\\')
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
		    ܐܢ (reader->js_fill != عدم
				     && (صحيح)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (صحيح)(p - reader->js_buf);
			ܐܢ (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, عدم, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, عدم, 4, TRUE, عدم);
		    ܐܢ (len == 0)
		    {
			ܐܢ (res != عدم)
			    ga_clear(&ga);
			ܦܢܐ FAIL;
		    }
		    p += len + 2;
		    ܐܢ (0xd800 <= nr && nr <= 0xdbff
			    && (صحيح)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, عدم, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, عدم, 4, TRUE, عدم);
			ܐܢ (len == 0)
			{
			    ܐܢ (res != عدم)
				ga_clear(&ga);
			    ܦܢܐ FAIL;
			}
			ܐܢ (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    ܐܢ (0xd800 <= nr && nr <= 0xdfff)
		    {
			ܐܢ (res != عدم)
			    ga_clear(&ga);
			ܦܢܐ FAIL;
		    }
		    ܐܢ (res != عدم)
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
	    ܐܢ (c > 0)
	    {
		p += 2;
		ܐܢ (res != عدم)
		    ga_append(&ga, c);
	    }
	}
	ܐܚܪܢܐ
	{
	    len = utf_ptr2len(p);
	    ܐܢ (res != عدم)
	    {
		ܐܢ (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    ܦܢܐ FAIL;
		}
		mch_memmove((حرف *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (صحيح)(p - reader->js_buf);
    ܐܢ (*p == quote)
    {
	++reader->js_used;
	ܐܢ (res != عدم)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#ܐܢ defined(USE_ICONV)
	    ܐܢ (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		ܐܢ (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, عدم);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, عدم, عدم);
	    }
	    ܐܚܪܢܐ
#endif
		res->vval.v_string = ga.ga_data;
	}
	ܦܢܐ OK;
    }
    ܐܢ (res != عدم)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_number = VVAL_NONE;
	ga_clear(&ga);
    }
    ܦܢܐ MAYBE;
}

تعريفنوع تعداد {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_KEY,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

تعريفنوع بنية {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_key_tv;
    char_u	  *jd_key;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    ساكن صحيح
json_decode_item(js_read_T *reader, typval_T *res, صحيح options)
{
    char_u	*p;
    صحيح		i;
    صحيح		len;
    صحيح		retval;
    garray_T	stack;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *top_item;
    char_u	key_buf[NUMBUFLEN];

    ga_init2(&stack, حجم(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    ܐܢ (res != عدم)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    لكل (;;)
    {
	top_item = عدم;
	ܐܢ (stack.ga_len > 0)
	{
	    top_item = ((json_dec_item_T *)stack.ga_data) + stack.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    ܐܢ (*p == NUL)
	    {
		retval = MAYBE;
		اذهب theend;
	    }
	    ܐܢ (top_item->jd_type == JSON_OBJECT_KEY
					    || top_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		ܐܢ (*p == (top_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --stack.ga_len;
		    ܐܢ (stack.ga_len == 0)
		    {
			retval = OK;
			اذهب theend;
		    }
		    ܐܢ (cur_item != عدم)
			cur_item = &top_item->jd_tv;
		    اذهب item_end;
		}
	    }
	}

	ܐܢ (top_item != عدم && top_item->jd_type == JSON_OBJECT_KEY
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *key;

	    // accept an object key that is not in quotes
	    key = p = reader->js_buf + reader->js_used;
	    ܟܡܐ (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    ܐܢ (cur_item != عدم)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(key, p - key);
		top_item->jd_key = cur_item->vval.v_string;
	    }
	    reader->js_used += (صحيح)(p - key);
	}
	ܐܚܪܢܐ
	{
	    بدل (*p)
	    {
		حالة '[': // start of array
		    ܐܢ (top_item && top_item->jd_type == JSON_OBJECT_KEY)
		    {
			retval = FAIL;
			اكسر;
		    }
		    ܐܢ (stack.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			اكسر;
		    }
		    ܐܢ (ga_grow(&stack, 1) == FAIL)
		    {
			retval = FAIL;
			اكسر;
		    }
		    ܐܢ (cur_item != عدم && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
			retval = FAIL;
			اكسر;
		    }

		    ++reader->js_used; // consume the '['
		    top_item = ((json_dec_item_T *)stack.ga_data)
								+ stack.ga_len;
		    top_item->jd_type = JSON_ARRAY;
		    ++stack.ga_len;
		    ܐܢ (cur_item != عدم)
		    {
			top_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    استمر;

		حالة '{': // start of object
		    ܐܢ (top_item && top_item->jd_type == JSON_OBJECT_KEY)
		    {
			retval = FAIL;
			اكسر;
		    }
		    ܐܢ (stack.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			اكسر;
		    }
		    ܐܢ (ga_grow(&stack, 1) == FAIL)
		    {
			retval = FAIL;
			اكسر;
		    }
		    ܐܢ (cur_item != عدم && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
			retval = FAIL;
			اكسر;
		    }

		    ++reader->js_used; // consume the '{'
		    top_item = ((json_dec_item_T *)stack.ga_data)
								+ stack.ga_len;
		    top_item->jd_type = JSON_OBJECT_KEY;
		    ++stack.ga_len;
		    ܐܢ (cur_item != عدم)
		    {
			top_item->jd_tv = *cur_item;
			cur_item = &top_item->jd_key_tv;
		    }
		    استمر;

		حالة '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    اكسر;

		حالة '\'':
		    ܐܢ (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    ܐܚܪܢܐ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    اكسر;

		حالة ',': // comma: empty item
		    ܐܢ ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			اكسر;
		    }
		    // FALLTHROUGH
		حالة NUL: // empty
		    ܐܢ (cur_item != عدم)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
		    }
		    retval = OK;
		    اكسر;

		افتراضي:
		    ܐܢ (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			ܐܢ (*sp == '-')
			{
			    ++sp;
			    ܐܢ (*sp == NUL)
			    {
				retval = MAYBE;
				اكسر;
			    }
			    ܐܢ (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				اكسر;
			    }
			}
			sp = skipdigits(sp);
			ܐܢ (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    ܐܢ (cur_item == عدم)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    ܐܚܪܢܐ
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			ܐܚܪܢܐ
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    عدم, &len, 0, // what
				    &nr, عدم, 0, TRUE, عدم);
			    ܐܢ (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				اذهب theend;
			    }
			    ܐܢ (cur_item != عدم)
			    {
				cur_item->v_type = VAR_NUMBER;
				cur_item->vval.v_number = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			اكسر;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((حرف *)(p), (kw), (len)) == 0 \
     : STRNCMP((حرف *)(p), (kw), (len)) == 0)
		    ܐܢ (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			ܐܢ (cur_item != عدم)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_number = VVAL_FALSE;
			}
			retval = OK;
			اكسر;
		    }
		    ܐܢ (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			ܐܢ (cur_item != عدم)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_number = VVAL_TRUE;
			}
			retval = OK;
			اكسر;
		    }
		    ܐܢ (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			ܐܢ (cur_item != عدم)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_number = VVAL_NULL;
			}
			retval = OK;
			اكسر;
		    }
		    ܐܢ (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			ܐܢ (cur_item != عدم)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			اكسر;
		    }
		    ܐܢ (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			ܐܢ (cur_item != عدم)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			اكسر;
		    }
		    ܐܢ (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			ܐܢ (cur_item != عدم)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			اكسر;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (صحيح)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    ܐܢ (
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
		    ܐܚܪܢܐ
			retval = FAIL;
		    اكسر;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    ܐܢ (retval == FAIL)
		اكسر;
	    ܐܢ (retval == MAYBE || stack.ga_len == 0)
		اذهب theend;

	    ܐܢ (top_item != عدم && top_item->jd_type == JSON_OBJECT_KEY
		    && cur_item != عدم)
	    {
		ܐܢ (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    اذهب theend;
		}
		top_item->jd_key = tv_get_string_buf_chk(cur_item, key_buf);
		ܐܢ (top_item->jd_key == عدم)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    اذهب theend;
		}
	    }
	}

item_end:
	top_item = ((json_dec_item_T *)stack.ga_data) + stack.ga_len - 1;
	بدل (top_item->jd_type)
	{
	    حالة JSON_ARRAY:
		ܐܢ (res != عدم)
		{
		    listitem_T	*li = listitem_alloc();

		    ܐܢ (li == عدم)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			اذهب theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(top_item->jd_tv.vval.v_list, li);
		}
		ܐܢ (cur_item != عدم)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ܐܢ (*p == ',')
		    ++reader->js_used;
		ܐܚܪܢܐ ܐܢ (*p != ']')
		{
		    ܐܢ (*p == NUL)
			retval = MAYBE;
		    ܐܚܪܢܐ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    اذهب theend;
		}
		اكسر;

	    حالة JSON_OBJECT_KEY:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ܐܢ (*p != ':')
		{
		    ܐܢ (cur_item != عدم)
			clear_tv(cur_item);
		    ܐܢ (*p == NUL)
			retval = MAYBE;
		    ܐܚܪܢܐ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    اذهب theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		top_item->jd_type = JSON_OBJECT;
		ܐܢ (cur_item != عدم)
		    cur_item = &item;
		اكسر;

	    حالة JSON_OBJECT:
		ܐܢ (cur_item != عدم
			&& dict_has_key(top_item->jd_tv.vval.v_dict,
						(حرف *)top_item->jd_key))
		{
		    semsg(_(e_duplicate_key_in_json_str), top_item->jd_key);
		    clear_tv(cur_item);
		    retval = FAIL;
		    اذهب theend;
		}

		ܐܢ (cur_item != عدم)
		{
		    dictitem_T *di = dictitem_alloc(top_item->jd_key);

		    clear_tv(&top_item->jd_key_tv);
		    ܐܢ (di == عدم)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			اذهب theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    ܐܢ (dict_add(top_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			اذهب theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ܐܢ (*p == ',')
		    ++reader->js_used;
		ܐܚܪܢܐ ܐܢ (*p != '}')
		{
		    ܐܢ (*p == NUL)
			retval = MAYBE;
		    ܐܚܪܢܐ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    اذهب theend;
		}
		top_item->jd_type = JSON_OBJECT_KEY;
		ܐܢ (cur_item != عدم)
		    cur_item = &top_item->jd_key_tv;
		اكسر;
	}
    }

    // Get here when parsing failed.
    ܐܢ (res != عدم)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_number = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    لكل (i = 0; i < stack.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)stack.ga_data) + i)->jd_key_tv);
    ga_clear(&stack);

    ܦܢܐ retval;
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
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    ܐܢ (ret != OK)
    {
	ܐܢ (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	ܦܢܐ FAIL;
    }
    json_skip_white(reader);
    ܐܢ (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	ܦܢܐ FAIL;
    }
    ܦܢܐ OK;
}

#ܐܢ defined(FEAT_JOB_CHANNEL)
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
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    ܦܢܐ ret;
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
    صحيح
json_find_end(js_read_T *reader, صحيح options)
{
    صحيح used_save = reader->js_used;
    صحيح ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, عدم, options);
    reader->js_used = used_save;
    ܦܢܐ ret;
}

/*
 * "js_decode()" function
 */
    فراغ
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ܐܢ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = عدم;
    reader.js_used = 0;
    ܐܢ (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_argument));
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

    ܐܢ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = عدم;
    reader.js_used = 0;
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
#endif
