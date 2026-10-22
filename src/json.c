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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_EVAL)

स्थिर पूर्णाङ्क json_encode_item(garray_T *gap, typval_T *val, पूर्णाङ्क copyID, पूर्णाङ्क options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    स्थिर पूर्णाङ्क
json_encode_gap(garray_T *gap, typval_T *val, पूर्णाङ्क options)
{
    जइ (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, पूर्णाङ्क options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    जइ (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    प्रत्यागम ga.ga_data;
}

#जइ defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_व्यञ्जकम्(पूर्णाङ्क nr, typval_T *val, पूर्णाङ्क options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_सङ्ख्या;
    nrtv.vval.v_सङ्ख्या = nr;
    जइ (rettv_list_alloc(&listtv) == FAIL)
	प्रत्यागम रिक्त;
    जइ (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	प्रत्यागम रिक्त;
    }

    ga_init2(&ga, 1, 4000);
    जइ (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    प्रत्यागम ga.ga_data;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_सन्देशः(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    जइ (json_encode_gap(&ga, val, 0) == FAIL)
	प्रत्यागम रिक्त;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    प्रत्यागम lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
स्थिर ध्रुव वर्ण ascii_needs_escape[128] = {
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
    स्थिर शून्य
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#जइ defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = रिक्त;
#endif
    पूर्णाङ्क		c;

    जइ (res == रिक्त)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	प्रत्यागम;
    }

#जइ defined(USE_ICONV)
    जइ (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	जइ (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, रिक्त);
	convert_setup(&conv, रिक्त, रिक्त);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (पूर्णाङ्क)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    जाव ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	जइ (c < 0x80)
	{
	    जइ (!ascii_needs_escape[c])
	    {
		res += 1;
		अनुवर्त;
	    }

	    जइ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    विकल्प (c)
	    {
		पक्ष 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); भङ्ग;
		पक्ष 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); भङ्ग;
		पक्ष 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); भङ्ग;
		पक्ष 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); भङ्ग;
		पक्ष 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); भङ्ग;
		पक्ष 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); भङ्ग;
		पक्ष 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); भङ्ग;
		स्वभाव:
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
	अण्णहा
	{
	    पूर्णाङ्क	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    जइ (l > 1)
	    {
		res += l;
		अनुवर्त;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    जइ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    जइ (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#जइ defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    स्थिर पूर्णाङ्क
is_simple_कुञ्चिका(char_u *कुञ्चिका)
{
    char_u *p;

    जइ (!ASCII_ISALPHA(*कुञ्चिका))
	प्रत्यागम FALSE;
    प्रति (p = कुञ्चिका + 1; *p != NUL; ++p)
	जइ (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    प्रत्यागम FALSE;
    प्रत्यागम TRUE;
}

प्रकारनिर्देश गणना {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

प्रकारनिर्देश संरचना {
    json_enc_type_T je_type;
    पूर्णाङ्क		    je_options;	    // options when this container was entered
    सङ्घ {
	संरचना {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	संरचना {
	    tuple_T	*tuple;
	    पूर्णाङ्क		idx;	    // current index
	    पूर्णाङ्क		len;
	} t;
	संरचना {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    पूर्णाङ्क		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    स्थिर पूर्णाङ्क
json_encode_item(garray_T *gap, typval_T *val, पूर्णाङ्क copyID, पूर्णाङ्क options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    पूर्णाङ्क			i;
    garray_T		राशिः;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&राशिः, परिमाण(json_enc_frame_T), 100);
    cur_val = val;

    प्रति (;;)
    {
    विकल्प (cur_val->v_type)
    {
	पक्ष VAR_BOOL:
	    विकल्प ((long)cur_val->vval.v_सङ्ख्या)
	    {
		पक्ष VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); भङ्ग;
		पक्ष VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); भङ्ग;
	    }
	    भङ्ग;

	पक्ष VAR_SPECIAL:
	    विकल्प ((long)cur_val->vval.v_सङ्ख्या)
	    {
		पक्ष VVAL_NONE: जइ ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    भङ्ग;
				// FALLTHROUGH
		पक्ष VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); भङ्ग;
	    }
	    भङ्ग;

	पक्ष VAR_सङ्ख्या:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((वर्ण *)numbuf, परिमाण(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_सङ्ख्या);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    भङ्ग;

	पक्ष VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    भङ्ग;

	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_INSTR:
	पक्ष VAR_CLASS:
	पक्ष VAR_OBJECT:
	पक्ष VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    गच्छ theend;

	पक्ष VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    जइ (b == रिक्त || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    अण्णहा
	    {
		पूर्णाङ्क	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		जइ (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    गच्छ theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		प्रति (i = 0; i < blen; i++)
		{
		    पूर्णाङ्क	    byte = src[i];

		    जइ (i > 0)
			*dst++ = ',';
		    जइ (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    अण्णहा जइ (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    अण्णहा
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (पूर्णाङ्क)(dst - (char_u *)gap->ga_data);
	    }
	    भङ्ग;

	पक्ष VAR_LIST:
	    l = cur_val->vval.v_list;
	    जइ (l == रिक्त)
		GA_CONCAT_LITERAL(gap, "[]");
	    अण्णहा
	    {
		जइ (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		अण्णहा
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    जइ (l->lv_first != रिक्त)
		    {
			जइ (राशिः.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    गच्छ theend;
			}
			जइ (ga_grow(&राशिः, 1) == FAIL)
			    गच्छ theend;
			frame = ((json_enc_frame_T *)राशिः.ga_data)
							    + राशिः.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++राशिः.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			अनुवर्त;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    भङ्ग;

	पक्ष VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    जइ (tuple == रिक्त)
		GA_CONCAT_LITERAL(gap, "[]");
	    अण्णहा
	    {
		जइ (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		अण्णहा
		{
		    पूर्णाङ्क		len = TUPLE_LEN(tuple);

		    जइ (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			जइ (राशिः.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    गच्छ theend;
			}
			जइ (ga_grow(&राशिः, 1) == FAIL)
			    गच्छ theend;
			frame = ((json_enc_frame_T *)राशिः.ga_data)
							    + राशिः.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++राशिः.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			अनुवर्त;
		    }
		    अण्णहा
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    भङ्ग;

	पक्ष VAR_DICT:
	    d = cur_val->vval.v_dict;
	    जइ (d == रिक्त)
		GA_CONCAT_LITERAL(gap, "{}");
	    अण्णहा
	    {
		जइ (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		अण्णहा
		{
		    पूर्णाङ्क		todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    जइ (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			प्रति (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			जइ ((options & JSON_JS)
						 && is_simple_कुञ्चिका(hi->hi_कुञ्चिका))
			    ga_concat(gap, hi->hi_कुञ्चिका);
			अण्णहा
			    write_string(gap, hi->hi_कुञ्चिका);
			ga_append(gap, ':');

			जइ (राशिः.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    गच्छ theend;
			}
			जइ (ga_grow(&राशिः, 1) == FAIL)
			    गच्छ theend;
			frame = ((json_enc_frame_T *)राशिः.ga_data)
							    + राशिः.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++राशिः.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			अनुवर्त;
		    }
		    अण्णहा
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    भङ्ग;

	पक्ष VAR_FLOAT:
#जइ defined(HAVE_MATH_H)
	    जइ (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    अण्णहा जइ (isinf(cur_val->vval.v_float))
	    {
		जइ (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		अण्णहा
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    अण्णहा
#endif
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((वर्ण *)numbuf, परिमाण(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    भङ्ग;

	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    गच्छ theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	प्रति (;;)
	{
	    पूर्णाङ्क		advance = FALSE;

	    जइ (राशिः.ga_len == 0)
	    {
		ga_clear(&राशिः);
		प्रत्यागम OK;
	    }

	    frame = ((json_enc_frame_T *)राशिः.ga_data)
							+ राशिः.ga_len - 1;
	    विकल्प (frame->je_type)
	    {
		पक्ष ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    जइ ((frame->je_options & JSON_JS)
			    && li->li_next == रिक्त
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_सङ्ख्या == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    जइ (li != रिक्त && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			भङ्ग;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --राशिः.ga_len;
		    भङ्ग;
		}

		पक्ष ENC_TUPLE:
		{
		    पूर्णाङ्क	    idx = frame->je_u.t.idx;
		    पूर्णाङ्क	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    जइ ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			जइ (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_सङ्ख्या == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    जइ (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			भङ्ग;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --राशिः.ga_len;
		    भङ्ग;
		}

		पक्ष ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    पूर्णाङ्क		todo = frame->je_u.d.todo;

		    जइ (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			प्रति (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			जइ ((frame->je_options & JSON_JS)
				&& is_simple_कुञ्चिका(hi->hi_कुञ्चिका))
			    ga_concat(gap, hi->hi_कुञ्चिका);
			अण्णहा
			    write_string(gap, hi->hi_कुञ्चिका);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			भङ्ग;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --राशिः.ga_len;
		    भङ्ग;
		}
	    }
	    जइ (advance)
		भङ्ग;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    प्रति (i = 0; i < राशिः.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)राशिः.ga_data) + i;
	विकल्प (frame->je_type)
	{
	    पक्ष ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		भङ्ग;
	    पक्ष ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		भङ्ग;
	    पक्ष ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		भङ्ग;
	}
    }
    ga_clear(&राशिः);
    प्रत्यागम FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    स्थिर शून्य
fill_numbuflen(js_read_T *reader)
{
    जइ (reader->js_fill != रिक्त && (पूर्णाङ्क)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	जइ (reader->js_fill(reader))
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    स्थिर शून्य
json_skip_white(js_read_T *reader)
{
    पूर्णाङ्क c;

    प्रति (;;)
    {
	c = reader->js_buf[reader->js_used];
	जइ (reader->js_fill != रिक्त && c == NUL)
	{
	    जइ (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		अनुवर्त;
	    }
	}
	जइ (c == NUL || c > ' ')
	    भङ्ग;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    स्थिर पूर्णाङ्क
json_decode_string(js_read_T *reader, typval_T *res, पूर्णाङ्क quote)
{
    garray_T    ga;
    पूर्णाङ्क		len;
    char_u	*p;
    पूर्णाङ्क		c;
    varnumber_T	nr;

    जइ (res != रिक्त)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    जाव (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	जइ (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    जइ (reader->js_fill == रिक्त)
		भङ्ग;
	    len = (पूर्णाङ्क)(reader->js_end - p);
	    reader->js_used = (पूर्णाङ्क)(p - reader->js_buf);
	    जइ (!reader->js_fill(reader))
		भङ्ग; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    अनुवर्त;
	}

	जइ (*p == '\\')
	{
	    c = -1;
	    विकल्प (p[1])
	    {
		पक्ष '\\': c = '\\'; भङ्ग;
		पक्ष '"': c = '"'; भङ्ग;
		पक्ष 'b': c = BS; भङ्ग;
		पक्ष 't': c = TAB; भङ्ग;
		पक्ष 'n': c = NL; भङ्ग;
		पक्ष 'f': c = FF; भङ्ग;
		पक्ष 'r': c = CAR; भङ्ग;
		पक्ष 'u':
		    जइ (reader->js_fill != रिक्त
				     && (पूर्णाङ्क)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (पूर्णाङ्क)(p - reader->js_buf);
			जइ (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, रिक्त, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, रिक्त, 4, TRUE, रिक्त);
		    जइ (len == 0)
		    {
			जइ (res != रिक्त)
			    ga_clear(&ga);
			प्रत्यागम FAIL;
		    }
		    p += len + 2;
		    जइ (0xd800 <= nr && nr <= 0xdbff
			    && (पूर्णाङ्क)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, रिक्त, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, रिक्त, 4, TRUE, रिक्त);
			जइ (len == 0)
			{
			    जइ (res != रिक्त)
				ga_clear(&ga);
			    प्रत्यागम FAIL;
			}
			जइ (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    जइ (0xd800 <= nr && nr <= 0xdfff)
		    {
			जइ (res != रिक्त)
			    ga_clear(&ga);
			प्रत्यागम FAIL;
		    }
		    जइ (res != रिक्त)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((पूर्णाङ्क)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    भङ्ग;
		स्वभाव:
		    // not a special char, skip over backslash
		    ++p;
		    अनुवर्त;
	    }
	    जइ (c > 0)
	    {
		p += 2;
		जइ (res != रिक्त)
		    ga_append(&ga, c);
	    }
	}
	अण्णहा
	{
	    len = utf_ptr2len(p);
	    जइ (res != रिक्त)
	    {
		जइ (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    प्रत्यागम FAIL;
		}
		mch_memmove((वर्ण *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (पूर्णाङ्क)(p - reader->js_buf);
    जइ (*p == quote)
    {
	++reader->js_used;
	जइ (res != रिक्त)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#जइ defined(USE_ICONV)
	    जइ (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		जइ (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, रिक्त);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, रिक्त, रिक्त);
	    }
	    अण्णहा
#endif
		res->vval.v_string = ga.ga_data;
	}
	प्रत्यागम OK;
    }
    जइ (res != रिक्त)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_सङ्ख्या = VVAL_NONE;
	ga_clear(&ga);
    }
    प्रत्यागम MAYBE;
}

प्रकारनिर्देश गणना {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_कुञ्चिका,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

प्रकारनिर्देश संरचना {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_कुञ्चिका_tv;
    char_u	  *jd_कुञ्चिका;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    स्थिर पूर्णाङ्क
json_decode_item(js_read_T *reader, typval_T *res, पूर्णाङ्क options)
{
    char_u	*p;
    पूर्णाङ्क		i;
    पूर्णाङ्क		len;
    पूर्णाङ्क		retval;
    garray_T	राशिः;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *शिखरम्_item;
    char_u	कुञ्चिका_buf[NUMBUFLEN];

    ga_init2(&राशिः, परिमाण(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    जइ (res != रिक्त)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    प्रति (;;)
    {
	शिखरम्_item = रिक्त;
	जइ (राशिः.ga_len > 0)
	{
	    शिखरम्_item = ((json_dec_item_T *)राशिः.ga_data) + राशिः.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    जइ (*p == NUL)
	    {
		retval = MAYBE;
		गच्छ theend;
	    }
	    जइ (शिखरम्_item->jd_type == JSON_OBJECT_कुञ्चिका
					    || शिखरम्_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		जइ (*p == (शिखरम्_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --राशिः.ga_len;
		    जइ (राशिः.ga_len == 0)
		    {
			retval = OK;
			गच्छ theend;
		    }
		    जइ (cur_item != रिक्त)
			cur_item = &शिखरम्_item->jd_tv;
		    गच्छ item_end;
		}
	    }
	}

	जइ (शिखरम्_item != रिक्त && शिखरम्_item->jd_type == JSON_OBJECT_कुञ्चिका
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *कुञ्चिका;

	    // accept an object key that is not in quotes
	    कुञ्चिका = p = reader->js_buf + reader->js_used;
	    जाव (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    जइ (cur_item != रिक्त)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(कुञ्चिका, p - कुञ्चिका);
		शिखरम्_item->jd_कुञ्चिका = cur_item->vval.v_string;
	    }
	    reader->js_used += (पूर्णाङ्क)(p - कुञ्चिका);
	}
	अण्णहा
	{
	    विकल्प (*p)
	    {
		पक्ष '[': // start of array
		    जइ (शिखरम्_item && शिखरम्_item->jd_type == JSON_OBJECT_कुञ्चिका)
		    {
			retval = FAIL;
			भङ्ग;
		    }
		    जइ (राशिः.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			भङ्ग;
		    }
		    जइ (ga_grow(&राशिः, 1) == FAIL)
		    {
			retval = FAIL;
			भङ्ग;
		    }
		    जइ (cur_item != रिक्त && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_सङ्ख्या = VVAL_NONE;
			retval = FAIL;
			भङ्ग;
		    }

		    ++reader->js_used; // consume the '['
		    शिखरम्_item = ((json_dec_item_T *)राशिः.ga_data)
								+ राशिः.ga_len;
		    शिखरम्_item->jd_type = JSON_ARRAY;
		    ++राशिः.ga_len;
		    जइ (cur_item != रिक्त)
		    {
			शिखरम्_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    अनुवर्त;

		पक्ष '{': // start of object
		    जइ (शिखरम्_item && शिखरम्_item->jd_type == JSON_OBJECT_कुञ्चिका)
		    {
			retval = FAIL;
			भङ्ग;
		    }
		    जइ (राशिः.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			भङ्ग;
		    }
		    जइ (ga_grow(&राशिः, 1) == FAIL)
		    {
			retval = FAIL;
			भङ्ग;
		    }
		    जइ (cur_item != रिक्त && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_सङ्ख्या = VVAL_NONE;
			retval = FAIL;
			भङ्ग;
		    }

		    ++reader->js_used; // consume the '{'
		    शिखरम्_item = ((json_dec_item_T *)राशिः.ga_data)
								+ राशिः.ga_len;
		    शिखरम्_item->jd_type = JSON_OBJECT_कुञ्चिका;
		    ++राशिः.ga_len;
		    जइ (cur_item != रिक्त)
		    {
			शिखरम्_item->jd_tv = *cur_item;
			cur_item = &शिखरम्_item->jd_कुञ्चिका_tv;
		    }
		    अनुवर्त;

		पक्ष '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    भङ्ग;

		पक्ष '\'':
		    जइ (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    अण्णहा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    भङ्ग;

		पक्ष ',': // comma: empty item
		    जइ ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			भङ्ग;
		    }
		    // FALLTHROUGH
		पक्ष NUL: // empty
		    जइ (cur_item != रिक्त)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_सङ्ख्या = VVAL_NONE;
		    }
		    retval = OK;
		    भङ्ग;

		स्वभाव:
		    जइ (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			जइ (*sp == '-')
			{
			    ++sp;
			    जइ (*sp == NUL)
			    {
				retval = MAYBE;
				भङ्ग;
			    }
			    जइ (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				भङ्ग;
			    }
			}
			sp = skipdigits(sp);
			जइ (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    जइ (cur_item == रिक्त)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    अण्णहा
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			अण्णहा
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    रिक्त, &len, 0, // what
				    &nr, रिक्त, 0, TRUE, रिक्त);
			    जइ (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				गच्छ theend;
			    }
			    जइ (cur_item != रिक्त)
			    {
				cur_item->v_type = VAR_सङ्ख्या;
				cur_item->vval.v_सङ्ख्या = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			भङ्ग;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((वर्ण *)(p), (kw), (len)) == 0 \
     : STRNCMP((वर्ण *)(p), (kw), (len)) == 0)
		    जइ (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			जइ (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_सङ्ख्या = VVAL_FALSE;
			}
			retval = OK;
			भङ्ग;
		    }
		    जइ (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			जइ (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_सङ्ख्या = VVAL_TRUE;
			}
			retval = OK;
			भङ्ग;
		    }
		    जइ (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			जइ (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_सङ्ख्या = VVAL_NULL;
			}
			retval = OK;
			भङ्ग;
		    }
		    जइ (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			जइ (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			भङ्ग;
		    }
		    जइ (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			जइ (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			भङ्ग;
		    }
		    जइ (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			जइ (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			भङ्ग;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (पूर्णाङ्क)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    जइ (
			    (len < 5 && STRNICMP((वर्ण *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((वर्ण *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((वर्ण *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((वर्ण *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((वर्ण *)p, "true", len) == 0
				    || STRNICMP((वर्ण *)p, "null", len) == 0)))

			retval = MAYBE;
		    अण्णहा
			retval = FAIL;
		    भङ्ग;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    जइ (retval == FAIL)
		भङ्ग;
	    जइ (retval == MAYBE || राशिः.ga_len == 0)
		गच्छ theend;

	    जइ (शिखरम्_item != रिक्त && शिखरम्_item->jd_type == JSON_OBJECT_कुञ्चिका
		    && cur_item != रिक्त)
	    {
		जइ (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    गच्छ theend;
		}
		शिखरम्_item->jd_कुञ्चिका = tv_get_string_buf_chk(cur_item, कुञ्चिका_buf);
		जइ (शिखरम्_item->jd_कुञ्चिका == रिक्त)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    गच्छ theend;
		}
	    }
	}

item_end:
	शिखरम्_item = ((json_dec_item_T *)राशिः.ga_data) + राशिः.ga_len - 1;
	विकल्प (शिखरम्_item->jd_type)
	{
	    पक्ष JSON_ARRAY:
		जइ (res != रिक्त)
		{
		    listitem_T	*li = listitem_alloc();

		    जइ (li == रिक्त)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			गच्छ theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(शिखरम्_item->jd_tv.vval.v_list, li);
		}
		जइ (cur_item != रिक्त)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		जइ (*p == ',')
		    ++reader->js_used;
		अण्णहा जइ (*p != ']')
		{
		    जइ (*p == NUL)
			retval = MAYBE;
		    अण्णहा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    गच्छ theend;
		}
		भङ्ग;

	    पक्ष JSON_OBJECT_कुञ्चिका:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		जइ (*p != ':')
		{
		    जइ (cur_item != रिक्त)
			clear_tv(cur_item);
		    जइ (*p == NUL)
			retval = MAYBE;
		    अण्णहा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    गच्छ theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		शिखरम्_item->jd_type = JSON_OBJECT;
		जइ (cur_item != रिक्त)
		    cur_item = &item;
		भङ्ग;

	    पक्ष JSON_OBJECT:
		जइ (cur_item != रिक्त
			&& dict_has_कुञ्चिका(शिखरम्_item->jd_tv.vval.v_dict,
						(वर्ण *)शिखरम्_item->jd_कुञ्चिका))
		{
		    semsg(_(e_duplicate_कुञ्चिका_in_json_str), शिखरम्_item->jd_कुञ्चिका);
		    clear_tv(cur_item);
		    retval = FAIL;
		    गच्छ theend;
		}

		जइ (cur_item != रिक्त)
		{
		    dictitem_T *di = dictitem_alloc(शिखरम्_item->jd_कुञ्चिका);

		    clear_tv(&शिखरम्_item->jd_कुञ्चिका_tv);
		    जइ (di == रिक्त)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			गच्छ theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    जइ (dict_add(शिखरम्_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			गच्छ theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		जइ (*p == ',')
		    ++reader->js_used;
		अण्णहा जइ (*p != '}')
		{
		    जइ (*p == NUL)
			retval = MAYBE;
		    अण्णहा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    गच्छ theend;
		}
		शिखरम्_item->jd_type = JSON_OBJECT_कुञ्चिका;
		जइ (cur_item != रिक्त)
		    cur_item = &शिखरम्_item->jd_कुञ्चिका_tv;
		भङ्ग;
	}
    }

    // Get here when parsing failed.
    जइ (res != रिक्त)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_सङ्ख्या = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    प्रति (i = 0; i < राशिः.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)राशिः.ga_data) + i)->jd_कुञ्चिका_tv);
    ga_clear(&राशिः);

    प्रत्यागम retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    स्थिर पूर्णाङ्क
json_decode_all(js_read_T *reader, typval_T *res, पूर्णाङ्क options)
{
    पूर्णाङ्क ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    जइ (ret != OK)
    {
	जइ (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	प्रत्यागम FAIL;
    }
    json_skip_white(reader);
    जइ (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

#जइ defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    पूर्णाङ्क
json_decode(js_read_T *reader, typval_T *res, पूर्णाङ्क options)
{
    पूर्णाङ्क ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    प्रत्यागम ret;
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
    पूर्णाङ्क
json_find_end(js_read_T *reader, पूर्णाङ्क options)
{
    पूर्णाङ्क used_save = reader->js_used;
    पूर्णाङ्क ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, रिक्त, options);
    reader->js_used = used_save;
    प्रत्यागम ret;
}

/*
 * "js_decode()" function
 */
    शून्य
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    जइ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = रिक्त;
    reader.js_used = 0;
    जइ (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_argument));
}

/*
 * "js_encode()" function
 */
    शून्य
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    शून्य
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    जइ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = रिक्त;
    reader.js_used = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    शून्य
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#endif
