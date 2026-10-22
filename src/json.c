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

#यदि defined(FEAT_EVAL)

स्थिर पूर्णाङ्क json_encode_item(garray_T *gap, typval_T *val, पूर्णाङ्क copyID, पूर्णाङ्क options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    स्थिर पूर्णाङ्क
json_encode_gap(garray_T *gap, typval_T *val, पूर्णाङ्क options)
{
    यदि (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
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
    यदि (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    फर्काऊ ga.ga_data;
}

#यदि defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_अभिव्यक्ति(पूर्णाङ्क nr, typval_T *val, पूर्णाङ्क options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_सङ्ख्या;
    nrtv.vval.v_सङ्ख्या = nr;
    यदि (rettv_list_alloc(&listtv) == FAIL)
	फर्काऊ रिक्त;
    यदि (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	फर्काऊ रिक्त;
    }

    ga_init2(&ga, 1, 4000);
    यदि (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    फर्काऊ ga.ga_data;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_सन्देश(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    माप_प्रकार	IObufflen;

    ga_init2(&ga, 1, 4000);
    यदि (json_encode_gap(&ga, val, 0) == FAIL)
	फर्काऊ रिक्त;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    फर्काऊ lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
स्थिर अचर वर्ण ascii_needs_escape[128] = {
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
#यदि defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = रिक्त;
#endif
    पूर्णाङ्क		c;

    यदि (res == रिक्त)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	फर्काऊ;
    }

#यदि defined(USE_ICONV)
    यदि (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	यदि (conv.vc_type != CONV_NONE)
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
    जबसम्म ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	यदि (c < 0x80)
	{
	    यदि (!ascii_needs_escape[c])
	    {
		res += 1;
		जारी;
	    }

	    यदि (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    विकल्प (c)
	    {
		पक्ष 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); रोक;
		पक्ष 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); रोक;
		पक्ष 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); रोक;
		पक्ष 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); रोक;
		पक्ष 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); रोक;
		पक्ष 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); रोक;
		पक्ष 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); रोक;
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
	अन्यथा
	{
	    पूर्णाङ्क	    l = utf_ptr2len(res);
	    माप_प्रकार  numbuflen;

	    यदि (l > 1)
	    {
		res += l;
		जारी;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    यदि (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    यदि (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#यदि defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    स्थिर पूर्णाङ्क
is_simple_साँचो(char_u *साँचो)
{
    char_u *p;

    यदि (!ASCII_ISALPHA(*साँचो))
	फर्काऊ FALSE;
    लागि (p = साँचो + 1; *p != NUL; ++p)
	यदि (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    फर्काऊ FALSE;
    फर्काऊ TRUE;
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
    garray_T		थाक;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&थाक, परिमाण(json_enc_frame_T), 100);
    cur_val = val;

    लागि (;;)
    {
    विकल्प (cur_val->v_type)
    {
	पक्ष VAR_BOOL:
	    विकल्प ((दीर्घ)cur_val->vval.v_सङ्ख्या)
	    {
		पक्ष VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); रोक;
		पक्ष VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); रोक;
	    }
	    रोक;

	पक्ष VAR_SPECIAL:
	    विकल्प ((दीर्घ)cur_val->vval.v_सङ्ख्या)
	    {
		पक्ष VVAL_NONE: यदि ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    रोक;
				// FALLTHROUGH
		पक्ष VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); रोक;
	    }
	    रोक;

	पक्ष VAR_सङ्ख्या:
	    {
		माप_प्रकार  numbuflen;

		numbuflen = vim_snprintf_safelen((वर्ण *)numbuf, परिमाण(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_सङ्ख्या);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    रोक;

	पक्ष VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    रोक;

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
	    यदि (b == रिक्त || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    अन्यथा
	    {
		पूर्णाङ्क	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		यदि (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    गच्छ theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		लागि (i = 0; i < blen; i++)
		{
		    पूर्णाङ्क	    byte = src[i];

		    यदि (i > 0)
			*dst++ = ',';
		    यदि (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    अन्यथा यदि (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    अन्यथा
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (पूर्णाङ्क)(dst - (char_u *)gap->ga_data);
	    }
	    रोक;

	पक्ष VAR_LIST:
	    l = cur_val->vval.v_list;
	    यदि (l == रिक्त)
		GA_CONCAT_LITERAL(gap, "[]");
	    अन्यथा
	    {
		यदि (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		अन्यथा
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    यदि (l->lv_first != रिक्त)
		    {
			यदि (थाक.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    गच्छ theend;
			}
			यदि (ga_grow(&थाक, 1) == FAIL)
			    गच्छ theend;
			frame = ((json_enc_frame_T *)थाक.ga_data)
							    + थाक.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++थाक.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			जारी;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    रोक;

	पक्ष VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    यदि (tuple == रिक्त)
		GA_CONCAT_LITERAL(gap, "[]");
	    अन्यथा
	    {
		यदि (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		अन्यथा
		{
		    पूर्णाङ्क		len = TUPLE_LEN(tuple);

		    यदि (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			यदि (थाक.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    गच्छ theend;
			}
			यदि (ga_grow(&थाक, 1) == FAIL)
			    गच्छ theend;
			frame = ((json_enc_frame_T *)थाक.ga_data)
							    + थाक.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++थाक.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			जारी;
		    }
		    अन्यथा
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    रोक;

	पक्ष VAR_DICT:
	    d = cur_val->vval.v_dict;
	    यदि (d == रिक्त)
		GA_CONCAT_LITERAL(gap, "{}");
	    अन्यथा
	    {
		यदि (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		अन्यथा
		{
		    पूर्णाङ्क		todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    यदि (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			लागि (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			यदि ((options & JSON_JS)
						 && is_simple_साँचो(hi->hi_साँचो))
			    ga_concat(gap, hi->hi_साँचो);
			अन्यथा
			    write_string(gap, hi->hi_साँचो);
			ga_append(gap, ':');

			यदि (थाक.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    गच्छ theend;
			}
			यदि (ga_grow(&थाक, 1) == FAIL)
			    गच्छ theend;
			frame = ((json_enc_frame_T *)थाक.ga_data)
							    + थाक.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++थाक.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			जारी;
		    }
		    अन्यथा
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    रोक;

	पक्ष VAR_FLOAT:
#यदि defined(HAVE_MATH_H)
	    यदि (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    अन्यथा यदि (isinf(cur_val->vval.v_float))
	    {
		यदि (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		अन्यथा
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    अन्यथा
#endif
	    {
		माप_प्रकार	numbuflen;

		numbuflen = vim_snprintf_safelen((वर्ण *)numbuf, परिमाण(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    रोक;

	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    गच्छ theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	लागि (;;)
	{
	    पूर्णाङ्क		advance = FALSE;

	    यदि (थाक.ga_len == 0)
	    {
		ga_clear(&थाक);
		फर्काऊ OK;
	    }

	    frame = ((json_enc_frame_T *)थाक.ga_data)
							+ थाक.ga_len - 1;
	    विकल्प (frame->je_type)
	    {
		पक्ष ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    यदि ((frame->je_options & JSON_JS)
			    && li->li_next == रिक्त
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_सङ्ख्या == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    यदि (li != रिक्त && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			रोक;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --थाक.ga_len;
		    रोक;
		}

		पक्ष ENC_TUPLE:
		{
		    पूर्णाङ्क	    idx = frame->je_u.t.idx;
		    पूर्णाङ्क	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    यदि ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			यदि (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_सङ्ख्या == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    यदि (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			रोक;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --थाक.ga_len;
		    रोक;
		}

		पक्ष ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    पूर्णाङ्क		todo = frame->je_u.d.todo;

		    यदि (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			लागि (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			यदि ((frame->je_options & JSON_JS)
				&& is_simple_साँचो(hi->hi_साँचो))
			    ga_concat(gap, hi->hi_साँचो);
			अन्यथा
			    write_string(gap, hi->hi_साँचो);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			रोक;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --थाक.ga_len;
		    रोक;
		}
	    }
	    यदि (advance)
		रोक;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    लागि (i = 0; i < थाक.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)थाक.ga_data) + i;
	विकल्प (frame->je_type)
	{
	    पक्ष ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		रोक;
	    पक्ष ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		रोक;
	    पक्ष ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		रोक;
	}
    }
    ga_clear(&थाक);
    फर्काऊ FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    स्थिर शून्य
fill_numbuflen(js_read_T *reader)
{
    यदि (reader->js_fill != रिक्त && (पूर्णाङ्क)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	यदि (reader->js_fill(reader))
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

    लागि (;;)
    {
	c = reader->js_buf[reader->js_used];
	यदि (reader->js_fill != रिक्त && c == NUL)
	{
	    यदि (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		जारी;
	    }
	}
	यदि (c == NUL || c > ' ')
	    रोक;
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

    यदि (res != रिक्त)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    जबसम्म (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	यदि (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    यदि (reader->js_fill == रिक्त)
		रोक;
	    len = (पूर्णाङ्क)(reader->js_end - p);
	    reader->js_used = (पूर्णाङ्क)(p - reader->js_buf);
	    यदि (!reader->js_fill(reader))
		रोक; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    जारी;
	}

	यदि (*p == '\\')
	{
	    c = -1;
	    विकल्प (p[1])
	    {
		पक्ष '\\': c = '\\'; रोक;
		पक्ष '"': c = '"'; रोक;
		पक्ष 'b': c = BS; रोक;
		पक्ष 't': c = TAB; रोक;
		पक्ष 'n': c = NL; रोक;
		पक्ष 'f': c = FF; रोक;
		पक्ष 'r': c = CAR; रोक;
		पक्ष 'u':
		    यदि (reader->js_fill != रिक्त
				     && (पूर्णाङ्क)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (पूर्णाङ्क)(p - reader->js_buf);
			यदि (reader->js_fill(reader))
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
		    यदि (len == 0)
		    {
			यदि (res != रिक्त)
			    ga_clear(&ga);
			फर्काऊ FAIL;
		    }
		    p += len + 2;
		    यदि (0xd800 <= nr && nr <= 0xdbff
			    && (पूर्णाङ्क)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, रिक्त, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, रिक्त, 4, TRUE, रिक्त);
			यदि (len == 0)
			{
			    यदि (res != रिक्त)
				ga_clear(&ga);
			    फर्काऊ FAIL;
			}
			यदि (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    यदि (0xd800 <= nr && nr <= 0xdfff)
		    {
			यदि (res != रिक्त)
			    ga_clear(&ga);
			फर्काऊ FAIL;
		    }
		    यदि (res != रिक्त)
		    {
			char_u	buf[NUMBUFLEN];
			माप_प्रकार	buflen;

			buflen = utf_char2bytes((पूर्णाङ्क)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    रोक;
		स्वभाव:
		    // not a special char, skip over backslash
		    ++p;
		    जारी;
	    }
	    यदि (c > 0)
	    {
		p += 2;
		यदि (res != रिक्त)
		    ga_append(&ga, c);
	    }
	}
	अन्यथा
	{
	    len = utf_ptr2len(p);
	    यदि (res != रिक्त)
	    {
		यदि (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    फर्काऊ FAIL;
		}
		mch_memmove((वर्ण *)ga.ga_data + ga.ga_len, p, (माप_प्रकार)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (पूर्णाङ्क)(p - reader->js_buf);
    यदि (*p == quote)
    {
	++reader->js_used;
	यदि (res != रिक्त)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#यदि defined(USE_ICONV)
	    यदि (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		यदि (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, रिक्त);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, रिक्त, रिक्त);
	    }
	    अन्यथा
#endif
		res->vval.v_string = ga.ga_data;
	}
	फर्काऊ OK;
    }
    यदि (res != रिक्त)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_सङ्ख्या = VVAL_NONE;
	ga_clear(&ga);
    }
    फर्काऊ MAYBE;
}

प्रकारनिर्देश गणना {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_साँचो,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

प्रकारनिर्देश संरचना {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_साँचो_tv;
    char_u	  *jd_साँचो;
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
    garray_T	थाक;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *शीर्ष_item;
    char_u	साँचो_buf[NUMBUFLEN];

    ga_init2(&थाक, परिमाण(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    यदि (res != रिक्त)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    लागि (;;)
    {
	शीर्ष_item = रिक्त;
	यदि (थाक.ga_len > 0)
	{
	    शीर्ष_item = ((json_dec_item_T *)थाक.ga_data) + थाक.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    यदि (*p == NUL)
	    {
		retval = MAYBE;
		गच्छ theend;
	    }
	    यदि (शीर्ष_item->jd_type == JSON_OBJECT_साँचो
					    || शीर्ष_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		यदि (*p == (शीर्ष_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --थाक.ga_len;
		    यदि (थाक.ga_len == 0)
		    {
			retval = OK;
			गच्छ theend;
		    }
		    यदि (cur_item != रिक्त)
			cur_item = &शीर्ष_item->jd_tv;
		    गच्छ item_end;
		}
	    }
	}

	यदि (शीर्ष_item != रिक्त && शीर्ष_item->jd_type == JSON_OBJECT_साँचो
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *साँचो;

	    // accept an object key that is not in quotes
	    साँचो = p = reader->js_buf + reader->js_used;
	    जबसम्म (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    यदि (cur_item != रिक्त)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(साँचो, p - साँचो);
		शीर्ष_item->jd_साँचो = cur_item->vval.v_string;
	    }
	    reader->js_used += (पूर्णाङ्क)(p - साँचो);
	}
	अन्यथा
	{
	    विकल्प (*p)
	    {
		पक्ष '[': // start of array
		    यदि (शीर्ष_item && शीर्ष_item->jd_type == JSON_OBJECT_साँचो)
		    {
			retval = FAIL;
			रोक;
		    }
		    यदि (थाक.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			रोक;
		    }
		    यदि (ga_grow(&थाक, 1) == FAIL)
		    {
			retval = FAIL;
			रोक;
		    }
		    यदि (cur_item != रिक्त && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_सङ्ख्या = VVAL_NONE;
			retval = FAIL;
			रोक;
		    }

		    ++reader->js_used; // consume the '['
		    शीर्ष_item = ((json_dec_item_T *)थाक.ga_data)
								+ थाक.ga_len;
		    शीर्ष_item->jd_type = JSON_ARRAY;
		    ++थाक.ga_len;
		    यदि (cur_item != रिक्त)
		    {
			शीर्ष_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    जारी;

		पक्ष '{': // start of object
		    यदि (शीर्ष_item && शीर्ष_item->jd_type == JSON_OBJECT_साँचो)
		    {
			retval = FAIL;
			रोक;
		    }
		    यदि (थाक.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			रोक;
		    }
		    यदि (ga_grow(&थाक, 1) == FAIL)
		    {
			retval = FAIL;
			रोक;
		    }
		    यदि (cur_item != रिक्त && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_सङ्ख्या = VVAL_NONE;
			retval = FAIL;
			रोक;
		    }

		    ++reader->js_used; // consume the '{'
		    शीर्ष_item = ((json_dec_item_T *)थाक.ga_data)
								+ थाक.ga_len;
		    शीर्ष_item->jd_type = JSON_OBJECT_साँचो;
		    ++थाक.ga_len;
		    यदि (cur_item != रिक्त)
		    {
			शीर्ष_item->jd_tv = *cur_item;
			cur_item = &शीर्ष_item->jd_साँचो_tv;
		    }
		    जारी;

		पक्ष '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    रोक;

		पक्ष '\'':
		    यदि (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    अन्यथा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    रोक;

		पक्ष ',': // comma: empty item
		    यदि ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			रोक;
		    }
		    // FALLTHROUGH
		पक्ष NUL: // empty
		    यदि (cur_item != रिक्त)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_सङ्ख्या = VVAL_NONE;
		    }
		    retval = OK;
		    रोक;

		स्वभाव:
		    यदि (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			यदि (*sp == '-')
			{
			    ++sp;
			    यदि (*sp == NUL)
			    {
				retval = MAYBE;
				रोक;
			    }
			    यदि (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				रोक;
			    }
			}
			sp = skipdigits(sp);
			यदि (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    यदि (cur_item == रिक्त)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    अन्यथा
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			अन्यथा
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    रिक्त, &len, 0, // what
				    &nr, रिक्त, 0, TRUE, रिक्त);
			    यदि (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				गच्छ theend;
			    }
			    यदि (cur_item != रिक्त)
			    {
				cur_item->v_type = VAR_सङ्ख्या;
				cur_item->vval.v_सङ्ख्या = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			रोक;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((वर्ण *)(p), (kw), (len)) == 0 \
     : STRNCMP((वर्ण *)(p), (kw), (len)) == 0)
		    यदि (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			यदि (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_सङ्ख्या = VVAL_FALSE;
			}
			retval = OK;
			रोक;
		    }
		    यदि (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			यदि (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_सङ्ख्या = VVAL_TRUE;
			}
			retval = OK;
			रोक;
		    }
		    यदि (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			यदि (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_सङ्ख्या = VVAL_NULL;
			}
			retval = OK;
			रोक;
		    }
		    यदि (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			यदि (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = न_अंक;
			}
			retval = OK;
			रोक;
		    }
		    यदि (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			यदि (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -अनन्त;
			}
			retval = OK;
			रोक;
		    }
		    यदि (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			यदि (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = अनन्त;
			}
			retval = OK;
			रोक;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (पूर्णाङ्क)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    यदि (
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
		    अन्यथा
			retval = FAIL;
		    रोक;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    यदि (retval == FAIL)
		रोक;
	    यदि (retval == MAYBE || थाक.ga_len == 0)
		गच्छ theend;

	    यदि (शीर्ष_item != रिक्त && शीर्ष_item->jd_type == JSON_OBJECT_साँचो
		    && cur_item != रिक्त)
	    {
		यदि (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    गच्छ theend;
		}
		शीर्ष_item->jd_साँचो = tv_get_string_buf_chk(cur_item, साँचो_buf);
		यदि (शीर्ष_item->jd_साँचो == रिक्त)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    गच्छ theend;
		}
	    }
	}

item_end:
	शीर्ष_item = ((json_dec_item_T *)थाक.ga_data) + थाक.ga_len - 1;
	विकल्प (शीर्ष_item->jd_type)
	{
	    पक्ष JSON_ARRAY:
		यदि (res != रिक्त)
		{
		    listitem_T	*li = listitem_alloc();

		    यदि (li == रिक्त)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			गच्छ theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(शीर्ष_item->jd_tv.vval.v_list, li);
		}
		यदि (cur_item != रिक्त)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		यदि (*p == ',')
		    ++reader->js_used;
		अन्यथा यदि (*p != ']')
		{
		    यदि (*p == NUL)
			retval = MAYBE;
		    अन्यथा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    गच्छ theend;
		}
		रोक;

	    पक्ष JSON_OBJECT_साँचो:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		यदि (*p != ':')
		{
		    यदि (cur_item != रिक्त)
			clear_tv(cur_item);
		    यदि (*p == NUL)
			retval = MAYBE;
		    अन्यथा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    गच्छ theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		शीर्ष_item->jd_type = JSON_OBJECT;
		यदि (cur_item != रिक्त)
		    cur_item = &item;
		रोक;

	    पक्ष JSON_OBJECT:
		यदि (cur_item != रिक्त
			&& dict_has_साँचो(शीर्ष_item->jd_tv.vval.v_dict,
						(वर्ण *)शीर्ष_item->jd_साँचो))
		{
		    semsg(_(e_duplicate_साँचो_in_json_str), शीर्ष_item->jd_साँचो);
		    clear_tv(cur_item);
		    retval = FAIL;
		    गच्छ theend;
		}

		यदि (cur_item != रिक्त)
		{
		    dictitem_T *di = dictitem_alloc(शीर्ष_item->jd_साँचो);

		    clear_tv(&शीर्ष_item->jd_साँचो_tv);
		    यदि (di == रिक्त)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			गच्छ theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    यदि (dict_add(शीर्ष_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			गच्छ theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		यदि (*p == ',')
		    ++reader->js_used;
		अन्यथा यदि (*p != '}')
		{
		    यदि (*p == NUL)
			retval = MAYBE;
		    अन्यथा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    गच्छ theend;
		}
		शीर्ष_item->jd_type = JSON_OBJECT_साँचो;
		यदि (cur_item != रिक्त)
		    cur_item = &शीर्ष_item->jd_साँचो_tv;
		रोक;
	}
    }

    // Get here when parsing failed.
    यदि (res != रिक्त)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_सङ्ख्या = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    लागि (i = 0; i < थाक.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)थाक.ga_data) + i)->jd_साँचो_tv);
    ga_clear(&थाक);

    फर्काऊ retval;
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
    यदि (ret != OK)
    {
	यदि (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	फर्काऊ FAIL;
    }
    json_skip_white(reader);
    यदि (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

#यदि defined(FEAT_JOB_CHANNEL)
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

    फर्काऊ ret;
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
    फर्काऊ ret;
}

/*
 * "js_decode()" function
 */
    शून्य
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = रिक्त;
    reader.js_used = 0;
    यदि (json_decode_all(&reader, rettv, JSON_JS) != OK)
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

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

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
