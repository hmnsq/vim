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

#जर defined(FEAT_EVAL)

स्थिर पूर्णांक json_encode_item(garray_T *gap, typval_T *val, पूर्णांक copyID, पूर्णांक options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    स्थिर पूर्णांक
json_encode_gap(garray_T *gap, typval_T *val, पूर्णांक options)
{
    जर (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	परत FAIL;
    }
    परत OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, पूर्णांक options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    जर (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    परत ga.ga_data;
}

#जर defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_अभिव्यक्ती(पूर्णांक nr, typval_T *val, पूर्णांक options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_संख्या;
    nrtv.vval.v_संख्या = nr;
    जर (rettv_list_alloc(&listtv) == FAIL)
	परत रिक्त;
    जर (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	परत रिक्त;
    }

    ga_init2(&ga, 1, 4000);
    जर (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    परत ga.ga_data;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_संदेश(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    माप_प्रकार	IObufflen;

    ga_init2(&ga, 1, 4000);
    जर (json_encode_gap(&ga, val, 0) == FAIL)
	परत रिक्त;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((अक्षर *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    परत lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
स्थिर स्थिरांक अक्षर ascii_needs_escape[128] = {
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
#जर defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = रिक्त;
#endif
    पूर्णांक		c;

    जर (res == रिक्त)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	परत;
    }

#जर defined(USE_ICONV)
    जर (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	जर (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, रिक्त);
	convert_setup(&conv, रिक्त, रिक्त);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (पूर्णांक)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    पर्यंत ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	जर (c < 0x80)
	{
	    जर (!ascii_needs_escape[c])
	    {
		res += 1;
		सुरूठेवा;
	    }

	    जर (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    निवड (c)
	    {
		स्थिती 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); थांबा;
		स्थिती 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); थांबा;
		स्थिती 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); थांबा;
		स्थिती 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); थांबा;
		स्थिती 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); थांबा;
		स्थिती 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); थांबा;
		स्थिती 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); थांबा;
		पूर्वनिर्धारित:
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
	    पूर्णांक	    l = utf_ptr2len(res);
	    माप_प्रकार  numbuflen;

	    जर (l > 1)
	    {
		res += l;
		सुरूठेवा;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    जर (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    जर (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#जर defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    स्थिर पूर्णांक
is_simple_किल्ली(char_u *किल्ली)
{
    char_u *p;

    जर (!ASCII_ISALPHA(*किल्ली))
	परत FALSE;
    साठी (p = किल्ली + 1; *p != NUL; ++p)
	जर (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    परत FALSE;
    परत TRUE;
}

प्रकारव्याख्या गणना {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

प्रकारव्याख्या संरचना {
    json_enc_type_T je_type;
    पूर्णांक		    je_options;	    // options when this container was entered
    संघ {
	संरचना {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	संरचना {
	    tuple_T	*tuple;
	    पूर्णांक		idx;	    // current index
	    पूर्णांक		len;
	} t;
	संरचना {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    पूर्णांक		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    स्थिर पूर्णांक
json_encode_item(garray_T *gap, typval_T *val, पूर्णांक copyID, पूर्णांक options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    पूर्णांक			i;
    garray_T		चळत;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&चळत, आकार(json_enc_frame_T), 100);
    cur_val = val;

    साठी (;;)
    {
    निवड (cur_val->v_type)
    {
	स्थिती VAR_BOOL:
	    निवड ((दीर्घ)cur_val->vval.v_संख्या)
	    {
		स्थिती VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); थांबा;
		स्थिती VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); थांबा;
	    }
	    थांबा;

	स्थिती VAR_SPECIAL:
	    निवड ((दीर्घ)cur_val->vval.v_संख्या)
	    {
		स्थिती VVAL_NONE: जर ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    थांबा;
				// FALLTHROUGH
		स्थिती VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); थांबा;
	    }
	    थांबा;

	स्थिती VAR_संख्या:
	    {
		माप_प्रकार  numbuflen;

		numbuflen = vim_snprintf_safelen((अक्षर *)numbuf, आकार(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_संख्या);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    थांबा;

	स्थिती VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    थांबा;

	स्थिती VAR_FUNC:
	स्थिती VAR_PARTIAL:
	स्थिती VAR_JOB:
	स्थिती VAR_CHANNEL:
	स्थिती VAR_INSTR:
	स्थिती VAR_CLASS:
	स्थिती VAR_OBJECT:
	स्थिती VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    जा theend;

	स्थिती VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    जर (b == रिक्त || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    अन्यथा
	    {
		पूर्णांक	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		जर (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    जा theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		साठी (i = 0; i < blen; i++)
		{
		    पूर्णांक	    byte = src[i];

		    जर (i > 0)
			*dst++ = ',';
		    जर (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    अन्यथा जर (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    अन्यथा
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (पूर्णांक)(dst - (char_u *)gap->ga_data);
	    }
	    थांबा;

	स्थिती VAR_LIST:
	    l = cur_val->vval.v_list;
	    जर (l == रिक्त)
		GA_CONCAT_LITERAL(gap, "[]");
	    अन्यथा
	    {
		जर (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		अन्यथा
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    जर (l->lv_first != रिक्त)
		    {
			जर (चळत.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    जा theend;
			}
			जर (ga_grow(&चळत, 1) == FAIL)
			    जा theend;
			frame = ((json_enc_frame_T *)चळत.ga_data)
							    + चळत.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++चळत.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			सुरूठेवा;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    थांबा;

	स्थिती VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    जर (tuple == रिक्त)
		GA_CONCAT_LITERAL(gap, "[]");
	    अन्यथा
	    {
		जर (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		अन्यथा
		{
		    पूर्णांक		len = TUPLE_LEN(tuple);

		    जर (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			जर (चळत.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    जा theend;
			}
			जर (ga_grow(&चळत, 1) == FAIL)
			    जा theend;
			frame = ((json_enc_frame_T *)चळत.ga_data)
							    + चळत.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++चळत.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			सुरूठेवा;
		    }
		    अन्यथा
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    थांबा;

	स्थिती VAR_DICT:
	    d = cur_val->vval.v_dict;
	    जर (d == रिक्त)
		GA_CONCAT_LITERAL(gap, "{}");
	    अन्यथा
	    {
		जर (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		अन्यथा
		{
		    पूर्णांक		todo = (पूर्णांक)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    जर (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			साठी (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			जर ((options & JSON_JS)
						 && is_simple_किल्ली(hi->hi_किल्ली))
			    ga_concat(gap, hi->hi_किल्ली);
			अन्यथा
			    write_string(gap, hi->hi_किल्ली);
			ga_append(gap, ':');

			जर (चळत.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    जा theend;
			}
			जर (ga_grow(&चळत, 1) == FAIL)
			    जा theend;
			frame = ((json_enc_frame_T *)चळत.ga_data)
							    + चळत.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++चळत.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			सुरूठेवा;
		    }
		    अन्यथा
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    थांबा;

	स्थिती VAR_FLOAT:
#जर defined(HAVE_MATH_H)
	    जर (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    अन्यथा जर (isinf(cur_val->vval.v_float))
	    {
		जर (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		अन्यथा
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    अन्यथा
#endif
	    {
		माप_प्रकार	numbuflen;

		numbuflen = vim_snprintf_safelen((अक्षर *)numbuf, आकार(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    थांबा;

	स्थिती VAR_UNKNOWN:
	स्थिती VAR_ANY:
	स्थिती VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    जा theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	साठी (;;)
	{
	    पूर्णांक		advance = FALSE;

	    जर (चळत.ga_len == 0)
	    {
		ga_clear(&चळत);
		परत OK;
	    }

	    frame = ((json_enc_frame_T *)चळत.ga_data)
							+ चळत.ga_len - 1;
	    निवड (frame->je_type)
	    {
		स्थिती ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    जर ((frame->je_options & JSON_JS)
			    && li->li_next == रिक्त
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_संख्या == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    जर (li != रिक्त && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			थांबा;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --चळत.ga_len;
		    थांबा;
		}

		स्थिती ENC_TUPLE:
		{
		    पूर्णांक	    idx = frame->je_u.t.idx;
		    पूर्णांक	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    जर ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			जर (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_संख्या == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    जर (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			थांबा;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --चळत.ga_len;
		    थांबा;
		}

		स्थिती ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    पूर्णांक		todo = frame->je_u.d.todo;

		    जर (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			साठी (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			जर ((frame->je_options & JSON_JS)
				&& is_simple_किल्ली(hi->hi_किल्ली))
			    ga_concat(gap, hi->hi_किल्ली);
			अन्यथा
			    write_string(gap, hi->hi_किल्ली);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			थांबा;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --चळत.ga_len;
		    थांबा;
		}
	    }
	    जर (advance)
		थांबा;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    साठी (i = 0; i < चळत.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)चळत.ga_data) + i;
	निवड (frame->je_type)
	{
	    स्थिती ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		थांबा;
	    स्थिती ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		थांबा;
	    स्थिती ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		थांबा;
	}
    }
    ga_clear(&चळत);
    परत FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    स्थिर शून्य
fill_numbuflen(js_read_T *reader)
{
    जर (reader->js_fill != रिक्त && (पूर्णांक)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	जर (reader->js_fill(reader))
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
    पूर्णांक c;

    साठी (;;)
    {
	c = reader->js_buf[reader->js_used];
	जर (reader->js_fill != रिक्त && c == NUL)
	{
	    जर (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		सुरूठेवा;
	    }
	}
	जर (c == NUL || c > ' ')
	    थांबा;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    स्थिर पूर्णांक
json_decode_string(js_read_T *reader, typval_T *res, पूर्णांक quote)
{
    garray_T    ga;
    पूर्णांक		len;
    char_u	*p;
    पूर्णांक		c;
    varnumber_T	nr;

    जर (res != रिक्त)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    पर्यंत (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	जर (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    जर (reader->js_fill == रिक्त)
		थांबा;
	    len = (पूर्णांक)(reader->js_end - p);
	    reader->js_used = (पूर्णांक)(p - reader->js_buf);
	    जर (!reader->js_fill(reader))
		थांबा; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    सुरूठेवा;
	}

	जर (*p == '\\')
	{
	    c = -1;
	    निवड (p[1])
	    {
		स्थिती '\\': c = '\\'; थांबा;
		स्थिती '"': c = '"'; थांबा;
		स्थिती 'b': c = BS; थांबा;
		स्थिती 't': c = TAB; थांबा;
		स्थिती 'n': c = NL; थांबा;
		स्थिती 'f': c = FF; थांबा;
		स्थिती 'r': c = CAR; थांबा;
		स्थिती 'u':
		    जर (reader->js_fill != रिक्त
				     && (पूर्णांक)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (पूर्णांक)(p - reader->js_buf);
			जर (reader->js_fill(reader))
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
		    जर (len == 0)
		    {
			जर (res != रिक्त)
			    ga_clear(&ga);
			परत FAIL;
		    }
		    p += len + 2;
		    जर (0xd800 <= nr && nr <= 0xdbff
			    && (पूर्णांक)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, रिक्त, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, रिक्त, 4, TRUE, रिक्त);
			जर (len == 0)
			{
			    जर (res != रिक्त)
				ga_clear(&ga);
			    परत FAIL;
			}
			जर (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    जर (0xd800 <= nr && nr <= 0xdfff)
		    {
			जर (res != रिक्त)
			    ga_clear(&ga);
			परत FAIL;
		    }
		    जर (res != रिक्त)
		    {
			char_u	buf[NUMBUFLEN];
			माप_प्रकार	buflen;

			buflen = utf_char2bytes((पूर्णांक)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    थांबा;
		पूर्वनिर्धारित:
		    // not a special char, skip over backslash
		    ++p;
		    सुरूठेवा;
	    }
	    जर (c > 0)
	    {
		p += 2;
		जर (res != रिक्त)
		    ga_append(&ga, c);
	    }
	}
	अन्यथा
	{
	    len = utf_ptr2len(p);
	    जर (res != रिक्त)
	    {
		जर (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    परत FAIL;
		}
		mch_memmove((अक्षर *)ga.ga_data + ga.ga_len, p, (माप_प्रकार)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (पूर्णांक)(p - reader->js_buf);
    जर (*p == quote)
    {
	++reader->js_used;
	जर (res != रिक्त)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#जर defined(USE_ICONV)
	    जर (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		जर (conv.vc_type != CONV_NONE)
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
	परत OK;
    }
    जर (res != रिक्त)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_संख्या = VVAL_NONE;
	ga_clear(&ga);
    }
    परत MAYBE;
}

प्रकारव्याख्या गणना {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_किल्ली,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

प्रकारव्याख्या संरचना {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_किल्ली_tv;
    char_u	  *jd_किल्ली;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    स्थिर पूर्णांक
json_decode_item(js_read_T *reader, typval_T *res, पूर्णांक options)
{
    char_u	*p;
    पूर्णांक		i;
    पूर्णांक		len;
    पूर्णांक		retval;
    garray_T	चळत;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *शीर्ष_item;
    char_u	किल्ली_buf[NUMBUFLEN];

    ga_init2(&चळत, आकार(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    जर (res != रिक्त)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    साठी (;;)
    {
	शीर्ष_item = रिक्त;
	जर (चळत.ga_len > 0)
	{
	    शीर्ष_item = ((json_dec_item_T *)चळत.ga_data) + चळत.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    जर (*p == NUL)
	    {
		retval = MAYBE;
		जा theend;
	    }
	    जर (शीर्ष_item->jd_type == JSON_OBJECT_किल्ली
					    || शीर्ष_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		जर (*p == (शीर्ष_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --चळत.ga_len;
		    जर (चळत.ga_len == 0)
		    {
			retval = OK;
			जा theend;
		    }
		    जर (cur_item != रिक्त)
			cur_item = &शीर्ष_item->jd_tv;
		    जा item_end;
		}
	    }
	}

	जर (शीर्ष_item != रिक्त && शीर्ष_item->jd_type == JSON_OBJECT_किल्ली
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *किल्ली;

	    // accept an object key that is not in quotes
	    किल्ली = p = reader->js_buf + reader->js_used;
	    पर्यंत (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    जर (cur_item != रिक्त)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(किल्ली, p - किल्ली);
		शीर्ष_item->jd_किल्ली = cur_item->vval.v_string;
	    }
	    reader->js_used += (पूर्णांक)(p - किल्ली);
	}
	अन्यथा
	{
	    निवड (*p)
	    {
		स्थिती '[': // start of array
		    जर (शीर्ष_item && शीर्ष_item->jd_type == JSON_OBJECT_किल्ली)
		    {
			retval = FAIL;
			थांबा;
		    }
		    जर (चळत.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			थांबा;
		    }
		    जर (ga_grow(&चळत, 1) == FAIL)
		    {
			retval = FAIL;
			थांबा;
		    }
		    जर (cur_item != रिक्त && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_संख्या = VVAL_NONE;
			retval = FAIL;
			थांबा;
		    }

		    ++reader->js_used; // consume the '['
		    शीर्ष_item = ((json_dec_item_T *)चळत.ga_data)
								+ चळत.ga_len;
		    शीर्ष_item->jd_type = JSON_ARRAY;
		    ++चळत.ga_len;
		    जर (cur_item != रिक्त)
		    {
			शीर्ष_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    सुरूठेवा;

		स्थिती '{': // start of object
		    जर (शीर्ष_item && शीर्ष_item->jd_type == JSON_OBJECT_किल्ली)
		    {
			retval = FAIL;
			थांबा;
		    }
		    जर (चळत.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			थांबा;
		    }
		    जर (ga_grow(&चळत, 1) == FAIL)
		    {
			retval = FAIL;
			थांबा;
		    }
		    जर (cur_item != रिक्त && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_संख्या = VVAL_NONE;
			retval = FAIL;
			थांबा;
		    }

		    ++reader->js_used; // consume the '{'
		    शीर्ष_item = ((json_dec_item_T *)चळत.ga_data)
								+ चळत.ga_len;
		    शीर्ष_item->jd_type = JSON_OBJECT_किल्ली;
		    ++चळत.ga_len;
		    जर (cur_item != रिक्त)
		    {
			शीर्ष_item->jd_tv = *cur_item;
			cur_item = &शीर्ष_item->jd_किल्ली_tv;
		    }
		    सुरूठेवा;

		स्थिती '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    थांबा;

		स्थिती '\'':
		    जर (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    अन्यथा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    थांबा;

		स्थिती ',': // comma: empty item
		    जर ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			थांबा;
		    }
		    // FALLTHROUGH
		स्थिती NUL: // empty
		    जर (cur_item != रिक्त)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_संख्या = VVAL_NONE;
		    }
		    retval = OK;
		    थांबा;

		पूर्वनिर्धारित:
		    जर (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			जर (*sp == '-')
			{
			    ++sp;
			    जर (*sp == NUL)
			    {
				retval = MAYBE;
				थांबा;
			    }
			    जर (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				थांबा;
			    }
			}
			sp = skipdigits(sp);
			जर (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    जर (cur_item == रिक्त)
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
			    जर (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				जा theend;
			    }
			    जर (cur_item != रिक्त)
			    {
				cur_item->v_type = VAR_संख्या;
				cur_item->vval.v_संख्या = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			थांबा;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((अक्षर *)(p), (kw), (len)) == 0 \
     : STRNCMP((अक्षर *)(p), (kw), (len)) == 0)
		    जर (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			जर (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_संख्या = VVAL_FALSE;
			}
			retval = OK;
			थांबा;
		    }
		    जर (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			जर (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_संख्या = VVAL_TRUE;
			}
			retval = OK;
			थांबा;
		    }
		    जर (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			जर (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_संख्या = VVAL_NULL;
			}
			retval = OK;
			थांबा;
		    }
		    जर (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			जर (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = न_अंक;
			}
			retval = OK;
			थांबा;
		    }
		    जर (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			जर (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -अनन्त;
			}
			retval = OK;
			थांबा;
		    }
		    जर (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			जर (cur_item != रिक्त)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = अनन्त;
			}
			retval = OK;
			थांबा;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (पूर्णांक)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    जर (
			    (len < 5 && STRNICMP((अक्षर *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((अक्षर *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((अक्षर *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((अक्षर *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((अक्षर *)p, "true", len) == 0
				    || STRNICMP((अक्षर *)p, "null", len) == 0)))

			retval = MAYBE;
		    अन्यथा
			retval = FAIL;
		    थांबा;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    जर (retval == FAIL)
		थांबा;
	    जर (retval == MAYBE || चळत.ga_len == 0)
		जा theend;

	    जर (शीर्ष_item != रिक्त && शीर्ष_item->jd_type == JSON_OBJECT_किल्ली
		    && cur_item != रिक्त)
	    {
		जर (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    जा theend;
		}
		शीर्ष_item->jd_किल्ली = tv_get_string_buf_chk(cur_item, किल्ली_buf);
		जर (शीर्ष_item->jd_किल्ली == रिक्त)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    जा theend;
		}
	    }
	}

item_end:
	शीर्ष_item = ((json_dec_item_T *)चळत.ga_data) + चळत.ga_len - 1;
	निवड (शीर्ष_item->jd_type)
	{
	    स्थिती JSON_ARRAY:
		जर (res != रिक्त)
		{
		    listitem_T	*li = listitem_alloc();

		    जर (li == रिक्त)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			जा theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(शीर्ष_item->jd_tv.vval.v_list, li);
		}
		जर (cur_item != रिक्त)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		जर (*p == ',')
		    ++reader->js_used;
		अन्यथा जर (*p != ']')
		{
		    जर (*p == NUL)
			retval = MAYBE;
		    अन्यथा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    जा theend;
		}
		थांबा;

	    स्थिती JSON_OBJECT_किल्ली:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		जर (*p != ':')
		{
		    जर (cur_item != रिक्त)
			clear_tv(cur_item);
		    जर (*p == NUL)
			retval = MAYBE;
		    अन्यथा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    जा theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		शीर्ष_item->jd_type = JSON_OBJECT;
		जर (cur_item != रिक्त)
		    cur_item = &item;
		थांबा;

	    स्थिती JSON_OBJECT:
		जर (cur_item != रिक्त
			&& dict_has_किल्ली(शीर्ष_item->jd_tv.vval.v_dict,
						(अक्षर *)शीर्ष_item->jd_किल्ली))
		{
		    semsg(_(e_duplicate_किल्ली_in_json_str), शीर्ष_item->jd_किल्ली);
		    clear_tv(cur_item);
		    retval = FAIL;
		    जा theend;
		}

		जर (cur_item != रिक्त)
		{
		    dictitem_T *di = dictitem_alloc(शीर्ष_item->jd_किल्ली);

		    clear_tv(&शीर्ष_item->jd_किल्ली_tv);
		    जर (di == रिक्त)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			जा theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    जर (dict_add(शीर्ष_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			जा theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		जर (*p == ',')
		    ++reader->js_used;
		अन्यथा जर (*p != '}')
		{
		    जर (*p == NUL)
			retval = MAYBE;
		    अन्यथा
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    जा theend;
		}
		शीर्ष_item->jd_type = JSON_OBJECT_किल्ली;
		जर (cur_item != रिक्त)
		    cur_item = &शीर्ष_item->jd_किल्ली_tv;
		थांबा;
	}
    }

    // Get here when parsing failed.
    जर (res != रिक्त)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_संख्या = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    साठी (i = 0; i < चळत.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)चळत.ga_data) + i)->jd_किल्ली_tv);
    ga_clear(&चळत);

    परत retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    स्थिर पूर्णांक
json_decode_all(js_read_T *reader, typval_T *res, पूर्णांक options)
{
    पूर्णांक ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    जर (ret != OK)
    {
	जर (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	परत FAIL;
    }
    json_skip_white(reader);
    जर (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	परत FAIL;
    }
    परत OK;
}

#जर defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    पूर्णांक
json_decode(js_read_T *reader, typval_T *res, पूर्णांक options)
{
    पूर्णांक ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    परत ret;
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
    पूर्णांक
json_find_end(js_read_T *reader, पूर्णांक options)
{
    पूर्णांक used_save = reader->js_used;
    पूर्णांक ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, रिक्त, options);
    reader->js_used = used_save;
    परत ret;
}

/*
 * "js_decode()" function
 */
    शून्य
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = रिक्त;
    reader.js_used = 0;
    जर (json_decode_all(&reader, rettv, JSON_JS) != OK)
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

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

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
