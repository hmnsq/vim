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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

ایستا صحیح json_encode_item(garray_T *gap, typval_T *val, صحیح copyID, صحیح options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    ایستا صحیح
json_encode_gap(garray_T *gap, typval_T *val, صحیح options)
{
    اگر (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	بازگشت FAIL;
    }
    بازگشت OK;
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
    بازگشت ga.ga_data;
}

#اگر defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_عبارت(صحیح nr, typval_T *val, صحیح options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_عدد;
    nrtv.vval.v_عدد = nr;
    اگر (rettv_list_alloc(&listtv) == FAIL)
	بازگشت پوچ;
    اگر (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	بازگشت پوچ;
    }

    ga_init2(&ga, 1, 4000);
    اگر (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    بازگشت ga.ga_data;
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
    اگر (json_encode_gap(&ga, val, 0) == FAIL)
	بازگشت پوچ;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((نویسه *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    بازگشت lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
ایستا ثابت نویسه ascii_needs_escape[128] = {
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
    ایستا تهی
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#اگر defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = پوچ;
#endif
    صحیح		c;

    اگر (res == پوچ)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	بازگشت;
    }

#اگر defined(USE_ICONV)
    اگر (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	اگر (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, پوچ);
	convert_setup(&conv, پوچ, پوچ);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (صحیح)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    وقتی ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	اگر (c < 0x80)
	{
	    اگر (!ascii_needs_escape[c])
	    {
		res += 1;
		ادامه;
	    }

	    اگر (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    انتخاب (c)
	    {
		حالت 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); بشکن;
		حالت 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); بشکن;
		حالت 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); بشکن;
		حالت 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); بشکن;
		حالت 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); بشکن;
		حالت 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); بشکن;
		حالت 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); بشکن;
		پیشفرض:
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
	وگرنه
	{
	    صحیح	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    اگر (l > 1)
	    {
		res += l;
		ادامه;
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
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    ایستا صحیح
is_simple_کلید(char_u *کلید)
{
    char_u *p;

    اگر (!ASCII_ISALPHA(*کلید))
	بازگشت FALSE;
    برای (p = کلید + 1; *p != NUL; ++p)
	اگر (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    بازگشت FALSE;
    بازگشت TRUE;
}

تعریفنوع شمارش {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

تعریفنوع ساختار {
    json_enc_type_T je_type;
    صحیح		    je_options;	    // options when this container was entered
    اجتماع {
	ساختار {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	ساختار {
	    tuple_T	*tuple;
	    صحیح		idx;	    // current index
	    صحیح		len;
	} t;
	ساختار {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    صحیح		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    ایستا صحیح
json_encode_item(garray_T *gap, typval_T *val, صحیح copyID, صحیح options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    صحیح			i;
    garray_T		پشته;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&پشته, اندازه(json_enc_frame_T), 100);
    cur_val = val;

    برای (;;)
    {
    انتخاب (cur_val->v_type)
    {
	حالت VAR_BOOL:
	    انتخاب ((long)cur_val->vval.v_عدد)
	    {
		حالت VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); بشکن;
		حالت VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); بشکن;
	    }
	    بشکن;

	حالت VAR_SPECIAL:
	    انتخاب ((long)cur_val->vval.v_عدد)
	    {
		حالت VVAL_NONE: اگر ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    بشکن;
				// FALLTHROUGH
		حالت VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); بشکن;
	    }
	    بشکن;

	حالت VAR_عدد:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((نویسه *)numbuf, اندازه(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_عدد);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    بشکن;

	حالت VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    بشکن;

	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	حالت VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    برو theend;

	حالت VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    اگر (b == پوچ || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    وگرنه
	    {
		صحیح	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		اگر (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    برو theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		برای (i = 0; i < blen; i++)
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
		    وگرنه اگر (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    وگرنه
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (صحیح)(dst - (char_u *)gap->ga_data);
	    }
	    بشکن;

	حالت VAR_LIST:
	    l = cur_val->vval.v_list;
	    اگر (l == پوچ)
		GA_CONCAT_LITERAL(gap, "[]");
	    وگرنه
	    {
		اگر (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		وگرنه
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    اگر (l->lv_first != پوچ)
		    {
			اگر (پشته.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    برو theend;
			}
			اگر (ga_grow(&پشته, 1) == FAIL)
			    برو theend;
			frame = ((json_enc_frame_T *)پشته.ga_data)
							    + پشته.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++پشته.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			ادامه;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    بشکن;

	حالت VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    اگر (tuple == پوچ)
		GA_CONCAT_LITERAL(gap, "[]");
	    وگرنه
	    {
		اگر (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		وگرنه
		{
		    صحیح		len = TUPLE_LEN(tuple);

		    اگر (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			اگر (پشته.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    برو theend;
			}
			اگر (ga_grow(&پشته, 1) == FAIL)
			    برو theend;
			frame = ((json_enc_frame_T *)پشته.ga_data)
							    + پشته.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++پشته.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			ادامه;
		    }
		    وگرنه
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    بشکن;

	حالت VAR_DICT:
	    d = cur_val->vval.v_dict;
	    اگر (d == پوچ)
		GA_CONCAT_LITERAL(gap, "{}");
	    وگرنه
	    {
		اگر (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		وگرنه
		{
		    صحیح		todo = (صحیح)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    اگر (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			برای (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			اگر ((options & JSON_JS)
						 && is_simple_کلید(hi->hi_کلید))
			    ga_concat(gap, hi->hi_کلید);
			وگرنه
			    write_string(gap, hi->hi_کلید);
			ga_append(gap, ':');

			اگر (پشته.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    برو theend;
			}
			اگر (ga_grow(&پشته, 1) == FAIL)
			    برو theend;
			frame = ((json_enc_frame_T *)پشته.ga_data)
							    + پشته.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++پشته.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			ادامه;
		    }
		    وگرنه
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    بشکن;

	حالت VAR_FLOAT:
#اگر defined(HAVE_MATH_H)
	    اگر (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    وگرنه اگر (isinf(cur_val->vval.v_float))
	    {
		اگر (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		وگرنه
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    وگرنه
#endif
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((نویسه *)numbuf, اندازه(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    بشکن;

	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    برو theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	برای (;;)
	{
	    صحیح		advance = FALSE;

	    اگر (پشته.ga_len == 0)
	    {
		ga_clear(&پشته);
		بازگشت OK;
	    }

	    frame = ((json_enc_frame_T *)پشته.ga_data)
							+ پشته.ga_len - 1;
	    انتخاب (frame->je_type)
	    {
		حالت ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    اگر ((frame->je_options & JSON_JS)
			    && li->li_next == پوچ
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_عدد == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    اگر (li != پوچ && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			بشکن;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --پشته.ga_len;
		    بشکن;
		}

		حالت ENC_TUPLE:
		{
		    صحیح	    idx = frame->je_u.t.idx;
		    صحیح	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    اگر ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			اگر (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_عدد == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    اگر (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			بشکن;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --پشته.ga_len;
		    بشکن;
		}

		حالت ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    صحیح		todo = frame->je_u.d.todo;

		    اگر (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			برای (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			اگر ((frame->je_options & JSON_JS)
				&& is_simple_کلید(hi->hi_کلید))
			    ga_concat(gap, hi->hi_کلید);
			وگرنه
			    write_string(gap, hi->hi_کلید);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			بشکن;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --پشته.ga_len;
		    بشکن;
		}
	    }
	    اگر (advance)
		بشکن;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    برای (i = 0; i < پشته.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)پشته.ga_data) + i;
	انتخاب (frame->je_type)
	{
	    حالت ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		بشکن;
	    حالت ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		بشکن;
	    حالت ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		بشکن;
	}
    }
    ga_clear(&پشته);
    بازگشت FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    ایستا تهی
fill_numbuflen(js_read_T *reader)
{
    اگر (reader->js_fill != پوچ && (صحیح)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	اگر (reader->js_fill(reader))
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    ایستا تهی
json_skip_white(js_read_T *reader)
{
    صحیح c;

    برای (;;)
    {
	c = reader->js_buf[reader->js_used];
	اگر (reader->js_fill != پوچ && c == NUL)
	{
	    اگر (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		ادامه;
	    }
	}
	اگر (c == NUL || c > ' ')
	    بشکن;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    ایستا صحیح
json_decode_string(js_read_T *reader, typval_T *res, صحیح quote)
{
    garray_T    ga;
    صحیح		len;
    char_u	*p;
    صحیح		c;
    varnumber_T	nr;

    اگر (res != پوچ)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    وقتی (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	اگر (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    اگر (reader->js_fill == پوچ)
		بشکن;
	    len = (صحیح)(reader->js_end - p);
	    reader->js_used = (صحیح)(p - reader->js_buf);
	    اگر (!reader->js_fill(reader))
		بشکن; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    ادامه;
	}

	اگر (*p == '\\')
	{
	    c = -1;
	    انتخاب (p[1])
	    {
		حالت '\\': c = '\\'; بشکن;
		حالت '"': c = '"'; بشکن;
		حالت 'b': c = BS; بشکن;
		حالت 't': c = TAB; بشکن;
		حالت 'n': c = NL; بشکن;
		حالت 'f': c = FF; بشکن;
		حالت 'r': c = CAR; بشکن;
		حالت 'u':
		    اگر (reader->js_fill != پوچ
				     && (صحیح)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (صحیح)(p - reader->js_buf);
			اگر (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, پوچ, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, پوچ, 4, TRUE, پوچ);
		    اگر (len == 0)
		    {
			اگر (res != پوچ)
			    ga_clear(&ga);
			بازگشت FAIL;
		    }
		    p += len + 2;
		    اگر (0xd800 <= nr && nr <= 0xdbff
			    && (صحیح)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, پوچ, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, پوچ, 4, TRUE, پوچ);
			اگر (len == 0)
			{
			    اگر (res != پوچ)
				ga_clear(&ga);
			    بازگشت FAIL;
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
			اگر (res != پوچ)
			    ga_clear(&ga);
			بازگشت FAIL;
		    }
		    اگر (res != پوچ)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((صحیح)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    بشکن;
		پیشفرض:
		    // not a special char, skip over backslash
		    ++p;
		    ادامه;
	    }
	    اگر (c > 0)
	    {
		p += 2;
		اگر (res != پوچ)
		    ga_append(&ga, c);
	    }
	}
	وگرنه
	{
	    len = utf_ptr2len(p);
	    اگر (res != پوچ)
	    {
		اگر (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    بازگشت FAIL;
		}
		mch_memmove((نویسه *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (صحیح)(p - reader->js_buf);
    اگر (*p == quote)
    {
	++reader->js_used;
	اگر (res != پوچ)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#اگر defined(USE_ICONV)
	    اگر (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		اگر (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, پوچ);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, پوچ, پوچ);
	    }
	    وگرنه
#endif
		res->vval.v_string = ga.ga_data;
	}
	بازگشت OK;
    }
    اگر (res != پوچ)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_عدد = VVAL_NONE;
	ga_clear(&ga);
    }
    بازگشت MAYBE;
}

تعریفنوع شمارش {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_کلید,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

تعریفنوع ساختار {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_کلید_tv;
    char_u	  *jd_کلید;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    ایستا صحیح
json_decode_item(js_read_T *reader, typval_T *res, صحیح options)
{
    char_u	*p;
    صحیح		i;
    صحیح		len;
    صحیح		retval;
    garray_T	پشته;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *بالاترین_item;
    char_u	کلید_buf[NUMBUFLEN];

    ga_init2(&پشته, اندازه(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    اگر (res != پوچ)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    برای (;;)
    {
	بالاترین_item = پوچ;
	اگر (پشته.ga_len > 0)
	{
	    بالاترین_item = ((json_dec_item_T *)پشته.ga_data) + پشته.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    اگر (*p == NUL)
	    {
		retval = MAYBE;
		برو theend;
	    }
	    اگر (بالاترین_item->jd_type == JSON_OBJECT_کلید
					    || بالاترین_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		اگر (*p == (بالاترین_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --پشته.ga_len;
		    اگر (پشته.ga_len == 0)
		    {
			retval = OK;
			برو theend;
		    }
		    اگر (cur_item != پوچ)
			cur_item = &بالاترین_item->jd_tv;
		    برو item_end;
		}
	    }
	}

	اگر (بالاترین_item != پوچ && بالاترین_item->jd_type == JSON_OBJECT_کلید
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *کلید;

	    // accept an object key that is not in quotes
	    کلید = p = reader->js_buf + reader->js_used;
	    وقتی (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    اگر (cur_item != پوچ)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(کلید, p - کلید);
		بالاترین_item->jd_کلید = cur_item->vval.v_string;
	    }
	    reader->js_used += (صحیح)(p - کلید);
	}
	وگرنه
	{
	    انتخاب (*p)
	    {
		حالت '[': // start of array
		    اگر (بالاترین_item && بالاترین_item->jd_type == JSON_OBJECT_کلید)
		    {
			retval = FAIL;
			بشکن;
		    }
		    اگر (پشته.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			بشکن;
		    }
		    اگر (ga_grow(&پشته, 1) == FAIL)
		    {
			retval = FAIL;
			بشکن;
		    }
		    اگر (cur_item != پوچ && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_عدد = VVAL_NONE;
			retval = FAIL;
			بشکن;
		    }

		    ++reader->js_used; // consume the '['
		    بالاترین_item = ((json_dec_item_T *)پشته.ga_data)
								+ پشته.ga_len;
		    بالاترین_item->jd_type = JSON_ARRAY;
		    ++پشته.ga_len;
		    اگر (cur_item != پوچ)
		    {
			بالاترین_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    ادامه;

		حالت '{': // start of object
		    اگر (بالاترین_item && بالاترین_item->jd_type == JSON_OBJECT_کلید)
		    {
			retval = FAIL;
			بشکن;
		    }
		    اگر (پشته.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			بشکن;
		    }
		    اگر (ga_grow(&پشته, 1) == FAIL)
		    {
			retval = FAIL;
			بشکن;
		    }
		    اگر (cur_item != پوچ && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_عدد = VVAL_NONE;
			retval = FAIL;
			بشکن;
		    }

		    ++reader->js_used; // consume the '{'
		    بالاترین_item = ((json_dec_item_T *)پشته.ga_data)
								+ پشته.ga_len;
		    بالاترین_item->jd_type = JSON_OBJECT_کلید;
		    ++پشته.ga_len;
		    اگر (cur_item != پوچ)
		    {
			بالاترین_item->jd_tv = *cur_item;
			cur_item = &بالاترین_item->jd_کلید_tv;
		    }
		    ادامه;

		حالت '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    بشکن;

		حالت '\'':
		    اگر (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    وگرنه
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    بشکن;

		حالت ',': // comma: empty item
		    اگر ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			بشکن;
		    }
		    // FALLTHROUGH
		حالت NUL: // empty
		    اگر (cur_item != پوچ)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_عدد = VVAL_NONE;
		    }
		    retval = OK;
		    بشکن;

		پیشفرض:
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
				بشکن;
			    }
			    اگر (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				بشکن;
			    }
			}
			sp = skipdigits(sp);
			اگر (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    اگر (cur_item == پوچ)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    وگرنه
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			وگرنه
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    پوچ, &len, 0, // what
				    &nr, پوچ, 0, TRUE, پوچ);
			    اگر (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				برو theend;
			    }
			    اگر (cur_item != پوچ)
			    {
				cur_item->v_type = VAR_عدد;
				cur_item->vval.v_عدد = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			بشکن;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((نویسه *)(p), (kw), (len)) == 0 \
     : STRNCMP((نویسه *)(p), (kw), (len)) == 0)
		    اگر (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			اگر (cur_item != پوچ)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_عدد = VVAL_FALSE;
			}
			retval = OK;
			بشکن;
		    }
		    اگر (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			اگر (cur_item != پوچ)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_عدد = VVAL_TRUE;
			}
			retval = OK;
			بشکن;
		    }
		    اگر (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			اگر (cur_item != پوچ)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_عدد = VVAL_NULL;
			}
			retval = OK;
			بشکن;
		    }
		    اگر (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			اگر (cur_item != پوچ)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			بشکن;
		    }
		    اگر (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			اگر (cur_item != پوچ)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			بشکن;
		    }
		    اگر (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			اگر (cur_item != پوچ)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			بشکن;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (صحیح)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    اگر (
			    (len < 5 && STRNICMP((نویسه *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((نویسه *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((نویسه *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((نویسه *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((نویسه *)p, "true", len) == 0
				    || STRNICMP((نویسه *)p, "null", len) == 0)))

			retval = MAYBE;
		    وگرنه
			retval = FAIL;
		    بشکن;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    اگر (retval == FAIL)
		بشکن;
	    اگر (retval == MAYBE || پشته.ga_len == 0)
		برو theend;

	    اگر (بالاترین_item != پوچ && بالاترین_item->jd_type == JSON_OBJECT_کلید
		    && cur_item != پوچ)
	    {
		اگر (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    برو theend;
		}
		بالاترین_item->jd_کلید = tv_get_string_buf_chk(cur_item, کلید_buf);
		اگر (بالاترین_item->jd_کلید == پوچ)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    برو theend;
		}
	    }
	}

item_end:
	بالاترین_item = ((json_dec_item_T *)پشته.ga_data) + پشته.ga_len - 1;
	انتخاب (بالاترین_item->jd_type)
	{
	    حالت JSON_ARRAY:
		اگر (res != پوچ)
		{
		    listitem_T	*li = listitem_alloc();

		    اگر (li == پوچ)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			برو theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(بالاترین_item->jd_tv.vval.v_list, li);
		}
		اگر (cur_item != پوچ)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		اگر (*p == ',')
		    ++reader->js_used;
		وگرنه اگر (*p != ']')
		{
		    اگر (*p == NUL)
			retval = MAYBE;
		    وگرنه
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    برو theend;
		}
		بشکن;

	    حالت JSON_OBJECT_کلید:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		اگر (*p != ':')
		{
		    اگر (cur_item != پوچ)
			clear_tv(cur_item);
		    اگر (*p == NUL)
			retval = MAYBE;
		    وگرنه
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    برو theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		بالاترین_item->jd_type = JSON_OBJECT;
		اگر (cur_item != پوچ)
		    cur_item = &item;
		بشکن;

	    حالت JSON_OBJECT:
		اگر (cur_item != پوچ
			&& dict_has_کلید(بالاترین_item->jd_tv.vval.v_dict,
						(نویسه *)بالاترین_item->jd_کلید))
		{
		    semsg(_(e_duplicate_کلید_in_json_str), بالاترین_item->jd_کلید);
		    clear_tv(cur_item);
		    retval = FAIL;
		    برو theend;
		}

		اگر (cur_item != پوچ)
		{
		    dictitem_T *di = dictitem_alloc(بالاترین_item->jd_کلید);

		    clear_tv(&بالاترین_item->jd_کلید_tv);
		    اگر (di == پوچ)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			برو theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    اگر (dict_add(بالاترین_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			برو theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		اگر (*p == ',')
		    ++reader->js_used;
		وگرنه اگر (*p != '}')
		{
		    اگر (*p == NUL)
			retval = MAYBE;
		    وگرنه
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    برو theend;
		}
		بالاترین_item->jd_type = JSON_OBJECT_کلید;
		اگر (cur_item != پوچ)
		    cur_item = &بالاترین_item->jd_کلید_tv;
		بشکن;
	}
    }

    // Get here when parsing failed.
    اگر (res != پوچ)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_عدد = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    برای (i = 0; i < پشته.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)پشته.ga_data) + i)->jd_کلید_tv);
    ga_clear(&پشته);

    بازگشت retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    ایستا صحیح
json_decode_all(js_read_T *reader, typval_T *res, صحیح options)
{
    صحیح ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    اگر (ret != OK)
    {
	اگر (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	بازگشت FAIL;
    }
    json_skip_white(reader);
    اگر (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	بازگشت FAIL;
    }
    بازگشت OK;
}

#اگر defined(FEAT_JOB_CHANNEL)
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
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    بازگشت ret;
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
    صحیح
json_find_end(js_read_T *reader, صحیح options)
{
    صحیح used_save = reader->js_used;
    صحیح ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, پوچ, options);
    reader->js_used = used_save;
    بازگشت ret;
}

/*
 * "js_decode()" function
 */
    تهی
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = پوچ;
    reader.js_used = 0;
    اگر (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_argument));
}

/*
 * "js_encode()" function
 */
    تهی
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    تهی
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = پوچ;
    reader.js_used = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    تهی
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#endif
