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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

സ്ഥിതം പൂർണ്ണസംഖ്യ json_encode_item(garray_T *gap, typval_T *val, പൂർണ്ണസംഖ്യ copyID, പൂർണ്ണസംഖ്യ options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
json_encode_gap(garray_T *gap, typval_T *val, പൂർണ്ണസംഖ്യ options)
{
    എങ്കിൽ (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, പൂർണ്ണസംഖ്യ options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    എങ്കിൽ (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    തിരികെ ga.ga_data;
}

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_വ്യഞ്ജകം(പൂർണ്ണസംഖ്യ nr, typval_T *val, പൂർണ്ണസംഖ്യ options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_സംഖ്യ;
    nrtv.vval.v_സംഖ്യ = nr;
    എങ്കിൽ (rettv_list_alloc(&listtv) == FAIL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	തിരികെ ശൂന്യ;
    }

    ga_init2(&ga, 1, 4000);
    എങ്കിൽ (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    തിരികെ ga.ga_data;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_സന്ദേശം(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    എങ്കിൽ (json_encode_gap(&ga, val, 0) == FAIL)
	തിരികെ ശൂന്യ;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((അക്ഷരം *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    തിരികെ lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
സ്ഥിതം സ്ഥിരം അക്ഷരം ascii_needs_escape[128] = {
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
    സ്ഥിതം ശൂന്യം
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#എങ്കിൽ defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = ശൂന്യ;
#endif
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (res == ശൂന്യ)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	തിരികെ;
    }

#എങ്കിൽ defined(USE_ICONV)
    എങ്കിൽ (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	എങ്കിൽ (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, ശൂന്യ);
	convert_setup(&conv, ശൂന്യ, ശൂന്യ);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (പൂർണ്ണസംഖ്യ)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    ആകുമ്പോൾ ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	എങ്കിൽ (c < 0x80)
	{
	    എങ്കിൽ (!ascii_needs_escape[c])
	    {
		res += 1;
		തുടരുക;
	    }

	    എങ്കിൽ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    തിരഞ്ഞെടുപ്പ് (c)
	    {
		അവസ്ഥ 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); നിർത്തുക;
		അവസ്ഥ 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); നിർത്തുക;
		അവസ്ഥ 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); നിർത്തുക;
		അവസ്ഥ 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); നിർത്തുക;
		അവസ്ഥ 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); നിർത്തുക;
		അവസ്ഥ 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); നിർത്തുക;
		അവസ്ഥ 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
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
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    എങ്കിൽ (l > 1)
	    {
		res += l;
		തുടരുക;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    എങ്കിൽ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    എങ്കിൽ (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#എങ്കിൽ defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_simple_താക്കോൽ(char_u *താക്കോൽ)
{
    char_u *p;

    എങ്കിൽ (!ASCII_ISALPHA(*താക്കോൽ))
	തിരികെ FALSE;
    വേണ്ടി (p = താക്കോൽ + 1; *p != NUL; ++p)
	എങ്കിൽ (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    തിരികെ FALSE;
    തിരികെ TRUE;
}

തരനിർവചനം എണ്ണൽ {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

തരനിർവചനം ഘടന {
    json_enc_type_T je_type;
    പൂർണ്ണസംഖ്യ		    je_options;	    // options when this container was entered
    സംഘം {
	ഘടന {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	ഘടന {
	    tuple_T	*tuple;
	    പൂർണ്ണസംഖ്യ		idx;	    // current index
	    പൂർണ്ണസംഖ്യ		len;
	} t;
	ഘടന {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    പൂർണ്ണസംഖ്യ		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
json_encode_item(garray_T *gap, typval_T *val, പൂർണ്ണസംഖ്യ copyID, പൂർണ്ണസംഖ്യ options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    പൂർണ്ണസംഖ്യ			i;
    garray_T		അട്ടി;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&അട്ടി, വലുപ്പം(json_enc_frame_T), 100);
    cur_val = val;

    വേണ്ടി (;;)
    {
    തിരഞ്ഞെടുപ്പ് (cur_val->v_type)
    {
	അവസ്ഥ VAR_BOOL:
	    തിരഞ്ഞെടുപ്പ് ((long)cur_val->vval.v_സംഖ്യ)
	    {
		അവസ്ഥ VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); നിർത്തുക;
		അവസ്ഥ VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); നിർത്തുക;
	    }
	    നിർത്തുക;

	അവസ്ഥ VAR_SPECIAL:
	    തിരഞ്ഞെടുപ്പ് ((long)cur_val->vval.v_സംഖ്യ)
	    {
		അവസ്ഥ VVAL_NONE: എങ്കിൽ ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    നിർത്തുക;
				// FALLTHROUGH
		അവസ്ഥ VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); നിർത്തുക;
	    }
	    നിർത്തുക;

	അവസ്ഥ VAR_സംഖ്യ:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((അക്ഷരം *)numbuf, വലുപ്പം(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_സംഖ്യ);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    നിർത്തുക;

	അവസ്ഥ VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    നിർത്തുക;

	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_OBJECT:
	അവസ്ഥ VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    പോകുക theend;

	അവസ്ഥ VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    എങ്കിൽ (b == ശൂന്യ || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    അല്ലെങ്കിൽ
	    {
		പൂർണ്ണസംഖ്യ	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		എങ്കിൽ (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    പോകുക theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		വേണ്ടി (i = 0; i < blen; i++)
		{
		    പൂർണ്ണസംഖ്യ	    byte = src[i];

		    എങ്കിൽ (i > 0)
			*dst++ = ',';
		    എങ്കിൽ (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    അല്ലെങ്കിൽ
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (പൂർണ്ണസംഖ്യ)(dst - (char_u *)gap->ga_data);
	    }
	    നിർത്തുക;

	അവസ്ഥ VAR_LIST:
	    l = cur_val->vval.v_list;
	    എങ്കിൽ (l == ശൂന്യ)
		GA_CONCAT_LITERAL(gap, "[]");
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		അല്ലെങ്കിൽ
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    എങ്കിൽ (l->lv_first != ശൂന്യ)
		    {
			എങ്കിൽ (അട്ടി.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    പോകുക theend;
			}
			എങ്കിൽ (ga_grow(&അട്ടി, 1) == FAIL)
			    പോകുക theend;
			frame = ((json_enc_frame_T *)അട്ടി.ga_data)
							    + അട്ടി.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++അട്ടി.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			തുടരുക;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    നിർത്തുക;

	അവസ്ഥ VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    എങ്കിൽ (tuple == ശൂന്യ)
		GA_CONCAT_LITERAL(gap, "[]");
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		അല്ലെങ്കിൽ
		{
		    പൂർണ്ണസംഖ്യ		len = TUPLE_LEN(tuple);

		    എങ്കിൽ (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			എങ്കിൽ (അട്ടി.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    പോകുക theend;
			}
			എങ്കിൽ (ga_grow(&അട്ടി, 1) == FAIL)
			    പോകുക theend;
			frame = ((json_enc_frame_T *)അട്ടി.ga_data)
							    + അട്ടി.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++അട്ടി.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			തുടരുക;
		    }
		    അല്ലെങ്കിൽ
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    നിർത്തുക;

	അവസ്ഥ VAR_DICT:
	    d = cur_val->vval.v_dict;
	    എങ്കിൽ (d == ശൂന്യ)
		GA_CONCAT_LITERAL(gap, "{}");
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		അല്ലെങ്കിൽ
		{
		    പൂർണ്ണസംഖ്യ		todo = (പൂർണ്ണസംഖ്യ)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    എങ്കിൽ (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			വേണ്ടി (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			എങ്കിൽ ((options & JSON_JS)
						 && is_simple_താക്കോൽ(hi->hi_താക്കോൽ))
			    ga_concat(gap, hi->hi_താക്കോൽ);
			അല്ലെങ്കിൽ
			    write_string(gap, hi->hi_താക്കോൽ);
			ga_append(gap, ':');

			എങ്കിൽ (അട്ടി.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    പോകുക theend;
			}
			എങ്കിൽ (ga_grow(&അട്ടി, 1) == FAIL)
			    പോകുക theend;
			frame = ((json_enc_frame_T *)അട്ടി.ga_data)
							    + അട്ടി.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++അട്ടി.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			തുടരുക;
		    }
		    അല്ലെങ്കിൽ
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    നിർത്തുക;

	അവസ്ഥ VAR_FLOAT:
#എങ്കിൽ defined(HAVE_MATH_H)
	    എങ്കിൽ (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    അല്ലെങ്കിൽ എങ്കിൽ (isinf(cur_val->vval.v_float))
	    {
		എങ്കിൽ (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		അല്ലെങ്കിൽ
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((അക്ഷരം *)numbuf, വലുപ്പം(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    നിർത്തുക;

	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    പോകുക theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	വേണ്ടി (;;)
	{
	    പൂർണ്ണസംഖ്യ		advance = FALSE;

	    എങ്കിൽ (അട്ടി.ga_len == 0)
	    {
		ga_clear(&അട്ടി);
		തിരികെ OK;
	    }

	    frame = ((json_enc_frame_T *)അട്ടി.ga_data)
							+ അട്ടി.ga_len - 1;
	    തിരഞ്ഞെടുപ്പ് (frame->je_type)
	    {
		അവസ്ഥ ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    എങ്കിൽ ((frame->je_options & JSON_JS)
			    && li->li_next == ശൂന്യ
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_സംഖ്യ == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    എങ്കിൽ (li != ശൂന്യ && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			നിർത്തുക;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --അട്ടി.ga_len;
		    നിർത്തുക;
		}

		അവസ്ഥ ENC_TUPLE:
		{
		    പൂർണ്ണസംഖ്യ	    idx = frame->je_u.t.idx;
		    പൂർണ്ണസംഖ്യ	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    എങ്കിൽ ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			എങ്കിൽ (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_സംഖ്യ == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    എങ്കിൽ (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			നിർത്തുക;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --അട്ടി.ga_len;
		    നിർത്തുക;
		}

		അവസ്ഥ ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    പൂർണ്ണസംഖ്യ		todo = frame->je_u.d.todo;

		    എങ്കിൽ (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			വേണ്ടി (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			എങ്കിൽ ((frame->je_options & JSON_JS)
				&& is_simple_താക്കോൽ(hi->hi_താക്കോൽ))
			    ga_concat(gap, hi->hi_താക്കോൽ);
			അല്ലെങ്കിൽ
			    write_string(gap, hi->hi_താക്കോൽ);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			നിർത്തുക;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --അട്ടി.ga_len;
		    നിർത്തുക;
		}
	    }
	    എങ്കിൽ (advance)
		നിർത്തുക;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    വേണ്ടി (i = 0; i < അട്ടി.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)അട്ടി.ga_data) + i;
	തിരഞ്ഞെടുപ്പ് (frame->je_type)
	{
	    അവസ്ഥ ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		നിർത്തുക;
	    അവസ്ഥ ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		നിർത്തുക;
	    അവസ്ഥ ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		നിർത്തുക;
	}
    }
    ga_clear(&അട്ടി);
    തിരികെ FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    സ്ഥിതം ശൂന്യം
fill_numbuflen(js_read_T *reader)
{
    എങ്കിൽ (reader->js_fill != ശൂന്യ && (പൂർണ്ണസംഖ്യ)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	എങ്കിൽ (reader->js_fill(reader))
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    സ്ഥിതം ശൂന്യം
json_skip_white(js_read_T *reader)
{
    പൂർണ്ണസംഖ്യ c;

    വേണ്ടി (;;)
    {
	c = reader->js_buf[reader->js_used];
	എങ്കിൽ (reader->js_fill != ശൂന്യ && c == NUL)
	{
	    എങ്കിൽ (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		തുടരുക;
	    }
	}
	എങ്കിൽ (c == NUL || c > ' ')
	    നിർത്തുക;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
json_decode_string(js_read_T *reader, typval_T *res, പൂർണ്ണസംഖ്യ quote)
{
    garray_T    ga;
    പൂർണ്ണസംഖ്യ		len;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		c;
    varnumber_T	nr;

    എങ്കിൽ (res != ശൂന്യ)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    ആകുമ്പോൾ (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	എങ്കിൽ (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    എങ്കിൽ (reader->js_fill == ശൂന്യ)
		നിർത്തുക;
	    len = (പൂർണ്ണസംഖ്യ)(reader->js_end - p);
	    reader->js_used = (പൂർണ്ണസംഖ്യ)(p - reader->js_buf);
	    എങ്കിൽ (!reader->js_fill(reader))
		നിർത്തുക; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    തുടരുക;
	}

	എങ്കിൽ (*p == '\\')
	{
	    c = -1;
	    തിരഞ്ഞെടുപ്പ് (p[1])
	    {
		അവസ്ഥ '\\': c = '\\'; നിർത്തുക;
		അവസ്ഥ '"': c = '"'; നിർത്തുക;
		അവസ്ഥ 'b': c = BS; നിർത്തുക;
		അവസ്ഥ 't': c = TAB; നിർത്തുക;
		അവസ്ഥ 'n': c = NL; നിർത്തുക;
		അവസ്ഥ 'f': c = FF; നിർത്തുക;
		അവസ്ഥ 'r': c = CAR; നിർത്തുക;
		അവസ്ഥ 'u':
		    എങ്കിൽ (reader->js_fill != ശൂന്യ
				     && (പൂർണ്ണസംഖ്യ)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (പൂർണ്ണസംഖ്യ)(p - reader->js_buf);
			എങ്കിൽ (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, ശൂന്യ, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, ശൂന്യ, 4, TRUE, ശൂന്യ);
		    എങ്കിൽ (len == 0)
		    {
			എങ്കിൽ (res != ശൂന്യ)
			    ga_clear(&ga);
			തിരികെ FAIL;
		    }
		    p += len + 2;
		    എങ്കിൽ (0xd800 <= nr && nr <= 0xdbff
			    && (പൂർണ്ണസംഖ്യ)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, ശൂന്യ, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, ശൂന്യ, 4, TRUE, ശൂന്യ);
			എങ്കിൽ (len == 0)
			{
			    എങ്കിൽ (res != ശൂന്യ)
				ga_clear(&ga);
			    തിരികെ FAIL;
			}
			എങ്കിൽ (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    എങ്കിൽ (0xd800 <= nr && nr <= 0xdfff)
		    {
			എങ്കിൽ (res != ശൂന്യ)
			    ga_clear(&ga);
			തിരികെ FAIL;
		    }
		    എങ്കിൽ (res != ശൂന്യ)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((പൂർണ്ണസംഖ്യ)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    // not a special char, skip over backslash
		    ++p;
		    തുടരുക;
	    }
	    എങ്കിൽ (c > 0)
	    {
		p += 2;
		എങ്കിൽ (res != ശൂന്യ)
		    ga_append(&ga, c);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    len = utf_ptr2len(p);
	    എങ്കിൽ (res != ശൂന്യ)
	    {
		എങ്കിൽ (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    തിരികെ FAIL;
		}
		mch_memmove((അക്ഷരം *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (പൂർണ്ണസംഖ്യ)(p - reader->js_buf);
    എങ്കിൽ (*p == quote)
    {
	++reader->js_used;
	എങ്കിൽ (res != ശൂന്യ)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#എങ്കിൽ defined(USE_ICONV)
	    എങ്കിൽ (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		എങ്കിൽ (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, ശൂന്യ);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, ശൂന്യ, ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
#endif
		res->vval.v_string = ga.ga_data;
	}
	തിരികെ OK;
    }
    എങ്കിൽ (res != ശൂന്യ)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_സംഖ്യ = VVAL_NONE;
	ga_clear(&ga);
    }
    തിരികെ MAYBE;
}

തരനിർവചനം എണ്ണൽ {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_താക്കോൽ,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

തരനിർവചനം ഘടന {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_താക്കോൽ_tv;
    char_u	  *jd_താക്കോൽ;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
json_decode_item(js_read_T *reader, typval_T *res, പൂർണ്ണസംഖ്യ options)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		retval;
    garray_T	അട്ടി;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *മുകൾ_item;
    char_u	താക്കോൽ_buf[NUMBUFLEN];

    ga_init2(&അട്ടി, വലുപ്പം(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    എങ്കിൽ (res != ശൂന്യ)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    വേണ്ടി (;;)
    {
	മുകൾ_item = ശൂന്യ;
	എങ്കിൽ (അട്ടി.ga_len > 0)
	{
	    മുകൾ_item = ((json_dec_item_T *)അട്ടി.ga_data) + അട്ടി.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    എങ്കിൽ (*p == NUL)
	    {
		retval = MAYBE;
		പോകുക theend;
	    }
	    എങ്കിൽ (മുകൾ_item->jd_type == JSON_OBJECT_താക്കോൽ
					    || മുകൾ_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		എങ്കിൽ (*p == (മുകൾ_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --അട്ടി.ga_len;
		    എങ്കിൽ (അട്ടി.ga_len == 0)
		    {
			retval = OK;
			പോകുക theend;
		    }
		    എങ്കിൽ (cur_item != ശൂന്യ)
			cur_item = &മുകൾ_item->jd_tv;
		    പോകുക item_end;
		}
	    }
	}

	എങ്കിൽ (മുകൾ_item != ശൂന്യ && മുകൾ_item->jd_type == JSON_OBJECT_താക്കോൽ
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *താക്കോൽ;

	    // accept an object key that is not in quotes
	    താക്കോൽ = p = reader->js_buf + reader->js_used;
	    ആകുമ്പോൾ (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    എങ്കിൽ (cur_item != ശൂന്യ)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(താക്കോൽ, p - താക്കോൽ);
		മുകൾ_item->jd_താക്കോൽ = cur_item->vval.v_string;
	    }
	    reader->js_used += (പൂർണ്ണസംഖ്യ)(p - താക്കോൽ);
	}
	അല്ലെങ്കിൽ
	{
	    തിരഞ്ഞെടുപ്പ് (*p)
	    {
		അവസ്ഥ '[': // start of array
		    എങ്കിൽ (മുകൾ_item && മുകൾ_item->jd_type == JSON_OBJECT_താക്കോൽ)
		    {
			retval = FAIL;
			നിർത്തുക;
		    }
		    എങ്കിൽ (അട്ടി.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			നിർത്തുക;
		    }
		    എങ്കിൽ (ga_grow(&അട്ടി, 1) == FAIL)
		    {
			retval = FAIL;
			നിർത്തുക;
		    }
		    എങ്കിൽ (cur_item != ശൂന്യ && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_സംഖ്യ = VVAL_NONE;
			retval = FAIL;
			നിർത്തുക;
		    }

		    ++reader->js_used; // consume the '['
		    മുകൾ_item = ((json_dec_item_T *)അട്ടി.ga_data)
								+ അട്ടി.ga_len;
		    മുകൾ_item->jd_type = JSON_ARRAY;
		    ++അട്ടി.ga_len;
		    എങ്കിൽ (cur_item != ശൂന്യ)
		    {
			മുകൾ_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    തുടരുക;

		അവസ്ഥ '{': // start of object
		    എങ്കിൽ (മുകൾ_item && മുകൾ_item->jd_type == JSON_OBJECT_താക്കോൽ)
		    {
			retval = FAIL;
			നിർത്തുക;
		    }
		    എങ്കിൽ (അട്ടി.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			നിർത്തുക;
		    }
		    എങ്കിൽ (ga_grow(&അട്ടി, 1) == FAIL)
		    {
			retval = FAIL;
			നിർത്തുക;
		    }
		    എങ്കിൽ (cur_item != ശൂന്യ && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_സംഖ്യ = VVAL_NONE;
			retval = FAIL;
			നിർത്തുക;
		    }

		    ++reader->js_used; // consume the '{'
		    മുകൾ_item = ((json_dec_item_T *)അട്ടി.ga_data)
								+ അട്ടി.ga_len;
		    മുകൾ_item->jd_type = JSON_OBJECT_താക്കോൽ;
		    ++അട്ടി.ga_len;
		    എങ്കിൽ (cur_item != ശൂന്യ)
		    {
			മുകൾ_item->jd_tv = *cur_item;
			cur_item = &മുകൾ_item->jd_താക്കോൽ_tv;
		    }
		    തുടരുക;

		അവസ്ഥ '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    നിർത്തുക;

		അവസ്ഥ '\'':
		    എങ്കിൽ (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    അല്ലെങ്കിൽ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    നിർത്തുക;

		അവസ്ഥ ',': // comma: empty item
		    എങ്കിൽ ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			നിർത്തുക;
		    }
		    // FALLTHROUGH
		അവസ്ഥ NUL: // empty
		    എങ്കിൽ (cur_item != ശൂന്യ)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_സംഖ്യ = VVAL_NONE;
		    }
		    retval = OK;
		    നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
		    എങ്കിൽ (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			എങ്കിൽ (*sp == '-')
			{
			    ++sp;
			    എങ്കിൽ (*sp == NUL)
			    {
				retval = MAYBE;
				നിർത്തുക;
			    }
			    എങ്കിൽ (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				നിർത്തുക;
			    }
			}
			sp = skipdigits(sp);
			എങ്കിൽ (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    എങ്കിൽ (cur_item == ശൂന്യ)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    അല്ലെങ്കിൽ
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			അല്ലെങ്കിൽ
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    ശൂന്യ, &len, 0, // what
				    &nr, ശൂന്യ, 0, TRUE, ശൂന്യ);
			    എങ്കിൽ (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				പോകുക theend;
			    }
			    എങ്കിൽ (cur_item != ശൂന്യ)
			    {
				cur_item->v_type = VAR_സംഖ്യ;
				cur_item->vval.v_സംഖ്യ = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			നിർത്തുക;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((അക്ഷരം *)(p), (kw), (len)) == 0 \
     : STRNCMP((അക്ഷരം *)(p), (kw), (len)) == 0)
		    എങ്കിൽ (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			എങ്കിൽ (cur_item != ശൂന്യ)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_സംഖ്യ = VVAL_FALSE;
			}
			retval = OK;
			നിർത്തുക;
		    }
		    എങ്കിൽ (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			എങ്കിൽ (cur_item != ശൂന്യ)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_സംഖ്യ = VVAL_TRUE;
			}
			retval = OK;
			നിർത്തുക;
		    }
		    എങ്കിൽ (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			എങ്കിൽ (cur_item != ശൂന്യ)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_സംഖ്യ = VVAL_NULL;
			}
			retval = OK;
			നിർത്തുക;
		    }
		    എങ്കിൽ (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			എങ്കിൽ (cur_item != ശൂന്യ)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			നിർത്തുക;
		    }
		    എങ്കിൽ (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			എങ്കിൽ (cur_item != ശൂന്യ)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			നിർത്തുക;
		    }
		    എങ്കിൽ (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			എങ്കിൽ (cur_item != ശൂന്യ)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			നിർത്തുക;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (പൂർണ്ണസംഖ്യ)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    എങ്കിൽ (
			    (len < 5 && STRNICMP((അക്ഷരം *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((അക്ഷരം *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((അക്ഷരം *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((അക്ഷരം *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((അക്ഷരം *)p, "true", len) == 0
				    || STRNICMP((അക്ഷരം *)p, "null", len) == 0)))

			retval = MAYBE;
		    അല്ലെങ്കിൽ
			retval = FAIL;
		    നിർത്തുക;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    എങ്കിൽ (retval == FAIL)
		നിർത്തുക;
	    എങ്കിൽ (retval == MAYBE || അട്ടി.ga_len == 0)
		പോകുക theend;

	    എങ്കിൽ (മുകൾ_item != ശൂന്യ && മുകൾ_item->jd_type == JSON_OBJECT_താക്കോൽ
		    && cur_item != ശൂന്യ)
	    {
		എങ്കിൽ (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    പോകുക theend;
		}
		മുകൾ_item->jd_താക്കോൽ = tv_get_string_buf_chk(cur_item, താക്കോൽ_buf);
		എങ്കിൽ (മുകൾ_item->jd_താക്കോൽ == ശൂന്യ)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    പോകുക theend;
		}
	    }
	}

item_end:
	മുകൾ_item = ((json_dec_item_T *)അട്ടി.ga_data) + അട്ടി.ga_len - 1;
	തിരഞ്ഞെടുപ്പ് (മുകൾ_item->jd_type)
	{
	    അവസ്ഥ JSON_ARRAY:
		എങ്കിൽ (res != ശൂന്യ)
		{
		    listitem_T	*li = listitem_alloc();

		    എങ്കിൽ (li == ശൂന്യ)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			പോകുക theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(മുകൾ_item->jd_tv.vval.v_list, li);
		}
		എങ്കിൽ (cur_item != ശൂന്യ)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		എങ്കിൽ (*p == ',')
		    ++reader->js_used;
		അല്ലെങ്കിൽ എങ്കിൽ (*p != ']')
		{
		    എങ്കിൽ (*p == NUL)
			retval = MAYBE;
		    അല്ലെങ്കിൽ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    പോകുക theend;
		}
		നിർത്തുക;

	    അവസ്ഥ JSON_OBJECT_താക്കോൽ:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		എങ്കിൽ (*p != ':')
		{
		    എങ്കിൽ (cur_item != ശൂന്യ)
			clear_tv(cur_item);
		    എങ്കിൽ (*p == NUL)
			retval = MAYBE;
		    അല്ലെങ്കിൽ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    പോകുക theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		മുകൾ_item->jd_type = JSON_OBJECT;
		എങ്കിൽ (cur_item != ശൂന്യ)
		    cur_item = &item;
		നിർത്തുക;

	    അവസ്ഥ JSON_OBJECT:
		എങ്കിൽ (cur_item != ശൂന്യ
			&& dict_has_താക്കോൽ(മുകൾ_item->jd_tv.vval.v_dict,
						(അക്ഷരം *)മുകൾ_item->jd_താക്കോൽ))
		{
		    semsg(_(e_duplicate_താക്കോൽ_in_json_str), മുകൾ_item->jd_താക്കോൽ);
		    clear_tv(cur_item);
		    retval = FAIL;
		    പോകുക theend;
		}

		എങ്കിൽ (cur_item != ശൂന്യ)
		{
		    dictitem_T *di = dictitem_alloc(മുകൾ_item->jd_താക്കോൽ);

		    clear_tv(&മുകൾ_item->jd_താക്കോൽ_tv);
		    എങ്കിൽ (di == ശൂന്യ)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			പോകുക theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    എങ്കിൽ (dict_add(മുകൾ_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			പോകുക theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		എങ്കിൽ (*p == ',')
		    ++reader->js_used;
		അല്ലെങ്കിൽ എങ്കിൽ (*p != '}')
		{
		    എങ്കിൽ (*p == NUL)
			retval = MAYBE;
		    അല്ലെങ്കിൽ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    പോകുക theend;
		}
		മുകൾ_item->jd_type = JSON_OBJECT_താക്കോൽ;
		എങ്കിൽ (cur_item != ശൂന്യ)
		    cur_item = &മുകൾ_item->jd_താക്കോൽ_tv;
		നിർത്തുക;
	}
    }

    // Get here when parsing failed.
    എങ്കിൽ (res != ശൂന്യ)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_സംഖ്യ = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    വേണ്ടി (i = 0; i < അട്ടി.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)അട്ടി.ga_data) + i)->jd_താക്കോൽ_tv);
    ga_clear(&അട്ടി);

    തിരികെ retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
json_decode_all(js_read_T *reader, typval_T *res, പൂർണ്ണസംഖ്യ options)
{
    പൂർണ്ണസംഖ്യ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    എങ്കിൽ (ret != OK)
    {
	എങ്കിൽ (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	തിരികെ FAIL;
    }
    json_skip_white(reader);
    എങ്കിൽ (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    പൂർണ്ണസംഖ്യ
json_decode(js_read_T *reader, typval_T *res, പൂർണ്ണസംഖ്യ options)
{
    പൂർണ്ണസംഖ്യ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    തിരികെ ret;
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
    പൂർണ്ണസംഖ്യ
json_find_end(js_read_T *reader, പൂർണ്ണസംഖ്യ options)
{
    പൂർണ്ണസംഖ്യ used_save = reader->js_used;
    പൂർണ്ണസംഖ്യ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, ശൂന്യ, options);
    reader->js_used = used_save;
    തിരികെ ret;
}

/*
 * "js_decode()" function
 */
    ശൂന്യം
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ശൂന്യ;
    reader.js_used = 0;
    എങ്കിൽ (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_argument));
}

/*
 * "js_encode()" function
 */
    ശൂന്യം
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    ശൂന്യം
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ശൂന്യ;
    reader.js_used = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    ശൂന്യം
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#endif
