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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

ਸਥਿਰ ਪੂਰਨ json_encode_ਵਸਤੂ(garray_T *gap, typval_T *val, ਪੂਰਨ copyID, ਪੂਰਨ options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    ਸਥਿਰ ਪੂਰਨ
json_encode_gap(garray_T *gap, typval_T *val, ਪੂਰਨ options)
{
    ਜੇ (json_encode_ਵਸਤੂ(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_ਅੰਕੜੇ = vim_strsave((char_u *)"");
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, ਪੂਰਨ options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    ਜੇ (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    ਵਾਪਸ ga.ga_ਅੰਕੜੇ;
}

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_ਵਿਅੰਜਕ(ਪੂਰਨ nr, typval_T *val, ਪੂਰਨ options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_ਸੰਖਿਆ;
    nrtv.vval.v_ਸੰਖਿਆ = nr;
    ਜੇ (rettv_list_alloc(&listtv) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ga_init2(&ga, 1, 4000);
    ਜੇ (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    ਵਾਪਸ ga.ga_ਅੰਕੜੇ;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_ਸੁਨੇਹਾ(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    ਜੇ (json_encode_gap(&ga, val, 0) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_ਅੰਕੜੇ, ga.ga_len);
    ga_clear(&ga);
    ਵਾਪਸ lspga.ga_ਅੰਕੜੇ;
}
#ਜੇ_ਅੰਤ

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ ascii_needs_escape[128] = {
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
    ਸਥਿਰ ਖਾਲੀ
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#ਜੇ defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		c;

    ਜੇ (res == ਨਿਰਾ)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	ਵਾਪਸ;
    }

#ਜੇ defined(USE_ICONV)
    ਜੇ (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_ਤਿਆਰੀ(&conv, p_enc, (char_u*)"utf-8");
	ਜੇ (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, ਨਿਰਾ);
	convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);
    }
#ਜੇ_ਅੰਤ
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (ਪੂਰਨ)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    ਜਦੋਂ ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	ਜੇ (c < 0x80)
	{
	    ਜੇ (!ascii_needs_escape[c])
	    {
		res += 1;
		ਜਾਰੀ;
	    }

	    ਜੇ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    ਚੋਣ (c)
	    {
		ਹਾਲਤ 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); ਰੋਕੋ;
		ਹਾਲਤ 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); ਰੋਕੋ;
		ਹਾਲਤ 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); ਰੋਕੋ;
		ਹਾਲਤ 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); ਰੋਕੋ;
		ਹਾਲਤ 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); ਰੋਕੋ;
		ਹਾਲਤ 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); ਰੋਕੋ;
		ਹਾਲਤ 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); ਰੋਕੋ;
		ਮੂਲ:
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
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    ਜੇ (l > 1)
	    {
		res += l;
		ਜਾਰੀ;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    ਜੇ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    ਜੇ (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#ਜੇ defined(USE_ICONV)
    vim_free(converted);
#ਜੇ_ਅੰਤ
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    ਸਥਿਰ ਪੂਰਨ
is_simple_ਕੁੰਜੀ(char_u *ਕੁੰਜੀ)
{
    char_u *p;

    ਜੇ (!ASCII_ISALPHA(*ਕੁੰਜੀ))
	ਵਾਪਸ FALSE;
    ਲਈ (p = ਕੁੰਜੀ + 1; *p != NUL; ++p)
	ਜੇ (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    json_enc_type_T je_type;
    ਪੂਰਨ		    je_options;	    // options when this container was entered
    ਸੰਘ {
	ਬਣਤਰ {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	ਬਣਤਰ {
	    tuple_T	*tuple;
	    ਪੂਰਨ		idx;	    // current index
	    ਪੂਰਨ		len;
	} t;
	ਬਣਤਰ {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    ਪੂਰਨ		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_ਚੌਖਟਾ_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    ਸਥਿਰ ਪੂਰਨ
json_encode_ਵਸਤੂ(garray_T *gap, typval_T *val, ਪੂਰਨ copyID, ਪੂਰਨ options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    ਪੂਰਨ			i;
    garray_T		ਢੇਰ;
    typval_T		*cur_val;
    json_enc_ਚੌਖਟਾ_T	*ਚੌਖਟਾ;

    ga_init2(&ਢੇਰ, ਆਕਾਰ(json_enc_ਚੌਖਟਾ_T), 100);
    cur_val = val;

    ਲਈ (;;)
    {
    ਚੋਣ (cur_val->v_type)
    {
	ਹਾਲਤ VAR_BOOL:
	    ਚੋਣ ((ਲੰਮਾ)cur_val->vval.v_ਸੰਖਿਆ)
	    {
		ਹਾਲਤ VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); ਰੋਕੋ;
		ਹਾਲਤ VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); ਰੋਕੋ;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_SPECIAL:
	    ਚੋਣ ((ਲੰਮਾ)cur_val->vval.v_ਸੰਖਿਆ)
	    {
		ਹਾਲਤ VVAL_NONE: ਜੇ ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    ਰੋਕੋ;
				// FALLTHROUGH
		ਹਾਲਤ VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); ਰੋਕੋ;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((ਅੱਖਰ *)numbuf, ਆਕਾਰ(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_ਸੰਖਿਆ);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_OBJECT:
	ਹਾਲਤ VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    ਜਾਓ theend;

	ਹਾਲਤ VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    ਜੇ (b == ਨਿਰਾ || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		ਜੇ (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    ਜਾਓ theend;
		src = (char_u *)b->bv_ga.ga_ਅੰਕੜੇ;
		dst = (char_u *)gap->ga_ਅੰਕੜੇ + gap->ga_len;
		*dst++ = '[';
		ਲਈ (i = 0; i < blen; i++)
		{
		    ਪੂਰਨ	    byte = src[i];

		    ਜੇ (i > 0)
			*dst++ = ',';
		    ਜੇ (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    ਨਹੀਂਤਾਂ
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (ਪੂਰਨ)(dst - (char_u *)gap->ga_ਅੰਕੜੇ);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_LIST:
	    l = cur_val->vval.v_list;
	    ਜੇ (l == ਨਿਰਾ)
		GA_CONCAT_LITERAL(gap, "[]");
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ਨਹੀਂਤਾਂ
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    ਜੇ (l->lv_ਪਹਿਲਾ != ਨਿਰਾ)
		    {
			ਜੇ (ਢੇਰ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_ਡੂੰਘਾਈ_is_higher_than_maxfuncdepth));
			    ਜਾਓ theend;
			}
			ਜੇ (ga_grow(&ਢੇਰ, 1) == FAIL)
			    ਜਾਓ theend;
			ਚੌਖਟਾ = ((json_enc_ਚੌਖਟਾ_T *)ਢੇਰ.ga_ਅੰਕੜੇ)
							    + ਢੇਰ.ga_len;
			ਚੌਖਟਾ->je_type = ENC_LIST;
			ਚੌਖਟਾ->je_options = options;
			ਚੌਖਟਾ->je_u.l.list = l;
			ਚੌਖਟਾ->je_u.l.li = l->lv_ਪਹਿਲਾ;
			++ਢੇਰ.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_ਪਹਿਲਾ->li_tv;
			ਜਾਰੀ;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    ਜੇ (tuple == ਨਿਰਾ)
		GA_CONCAT_LITERAL(gap, "[]");
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ		len = TUPLE_LEN(tuple);

		    ਜੇ (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			ਜੇ (ਢੇਰ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_ਡੂੰਘਾਈ_is_higher_than_maxfuncdepth));
			    ਜਾਓ theend;
			}
			ਜੇ (ga_grow(&ਢੇਰ, 1) == FAIL)
			    ਜਾਓ theend;
			ਚੌਖਟਾ = ((json_enc_ਚੌਖਟਾ_T *)ਢੇਰ.ga_ਅੰਕੜੇ)
							    + ਢੇਰ.ga_len;
			ਚੌਖਟਾ->je_type = ENC_TUPLE;
			ਚੌਖਟਾ->je_options = options;
			ਚੌਖਟਾ->je_u.t.tuple = tuple;
			ਚੌਖਟਾ->je_u.t.idx = 0;
			ਚੌਖਟਾ->je_u.t.len = len;
			++ਢੇਰ.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ਵਸਤੂ(tuple, 0);
			ਜਾਰੀ;
		    }
		    ਨਹੀਂਤਾਂ
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_DICT:
	    d = cur_val->vval.v_dict;
	    ਜੇ (d == ਨਿਰਾ)
		GA_CONCAT_LITERAL(gap, "{}");
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ		todo = (ਪੂਰਨ)d->dv_hashtab.ht_ਵਰਤਿਆ;
		    hashitem_T	*hi;

		    ਜੇ (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			ਲਈ (hi = d->dv_hashtab.ht_array;
						  HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi); ++hi)
			    ;
			--todo;

			// Write first key
			ਜੇ ((options & JSON_JS)
						 && is_simple_ਕੁੰਜੀ(hi->hi_ਕੁੰਜੀ))
			    ga_concat(gap, hi->hi_ਕੁੰਜੀ);
			ਨਹੀਂਤਾਂ
			    write_string(gap, hi->hi_ਕੁੰਜੀ);
			ga_append(gap, ':');

			ਜੇ (ਢੇਰ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_ਡੂੰਘਾਈ_is_higher_than_maxfuncdepth));
			    ਜਾਓ theend;
			}
			ਜੇ (ga_grow(&ਢੇਰ, 1) == FAIL)
			    ਜਾਓ theend;
			ਚੌਖਟਾ = ((json_enc_ਚੌਖਟਾ_T *)ਢੇਰ.ga_ਅੰਕੜੇ)
							    + ਢੇਰ.ga_len;
			ਚੌਖਟਾ->je_type = ENC_DICT;
			ਚੌਖਟਾ->je_options = options;
			ਚੌਖਟਾ->je_u.d.dict = d;
			ਚੌਖਟਾ->je_u.d.hi = hi;
			ਚੌਖਟਾ->je_u.d.todo = todo;
			++ਢੇਰ.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			ਜਾਰੀ;
		    }
		    ਨਹੀਂਤਾਂ
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_FLOAT:
#ਜੇ defined(HAVE_MATH_H)
	    ਜੇ (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    ਨਹੀਂਤਾਂ ਜੇ (isinf(cur_val->vval.v_float))
	    {
		ਜੇ (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		ਨਹੀਂਤਾਂ
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((ਅੱਖਰ *)numbuf, ਆਕਾਰ(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	    ਅੰਦਰੂਨੀ_error_no_abort("json_encode_item()");
	    ਜਾਓ theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	ਲਈ (;;)
	{
	    ਪੂਰਨ		advance = FALSE;

	    ਜੇ (ਢੇਰ.ga_len == 0)
	    {
		ga_clear(&ਢੇਰ);
		ਵਾਪਸ OK;
	    }

	    ਚੌਖਟਾ = ((json_enc_ਚੌਖਟਾ_T *)ਢੇਰ.ga_ਅੰਕੜੇ)
							+ ਢੇਰ.ga_len - 1;
	    ਚੋਣ (ਚੌਖਟਾ->je_type)
	    {
		ਹਾਲਤ ENC_LIST:
		{
		    listitem_T	*li = ਚੌਖਟਾ->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    ਜੇ ((ਚੌਖਟਾ->je_options & JSON_JS)
			    && li->li_ਅਗਲਾ == ਨਿਰਾ
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_ਸੰਖਿਆ == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_ਅਗਲਾ;
		    ਚੌਖਟਾ->je_u.l.li = li;
		    ਜੇ (li != ਨਿਰਾ && !got_int)
		    {
			ga_append(gap, ',');
			options = ਚੌਖਟਾ->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			ਰੋਕੋ;
		    }
		    ga_append(gap, ']');
		    ਚੌਖਟਾ->je_u.l.list->lv_copyID = 0;
		    --ਢੇਰ.ga_len;
		    ਰੋਕੋ;
		}

		ਹਾਲਤ ENC_TUPLE:
		{
		    ਪੂਰਨ	    idx = ਚੌਖਟਾ->je_u.t.idx;
		    ਪੂਰਨ	    len = ਚੌਖਟਾ->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    ਜੇ ((ਚੌਖਟਾ->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_ਵਸਤੂ =
				    TUPLE_ਵਸਤੂ(ਚੌਖਟਾ->je_u.t.tuple, idx);

			ਜੇ (t_ਵਸਤੂ->v_type == VAR_SPECIAL
				&& t_ਵਸਤੂ->vval.v_ਸੰਖਿਆ == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    ਚੌਖਟਾ->je_u.t.idx = idx;
		    ਜੇ (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = ਚੌਖਟਾ->je_options & JSON_JS;
			cur_val = TUPLE_ਵਸਤੂ(ਚੌਖਟਾ->je_u.t.tuple, idx);
			advance = TRUE;
			ਰੋਕੋ;
		    }
		    ga_append(gap, ']');
		    ਚੌਖਟਾ->je_u.t.tuple->tv_copyID = 0;
		    --ਢੇਰ.ga_len;
		    ਰੋਕੋ;
		}

		ਹਾਲਤ ENC_DICT:
		{
		    hashitem_T	*hi = ਚੌਖਟਾ->je_u.d.hi;
		    ਪੂਰਨ		todo = ਚੌਖਟਾ->je_u.d.todo;

		    ਜੇ (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			ਲਈ (++hi; HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi); ++hi)
			    ;
			--todo;
			ਚੌਖਟਾ->je_u.d.hi = hi;
			ਚੌਖਟਾ->je_u.d.todo = todo;

			ga_append(gap, ',');
			ਜੇ ((ਚੌਖਟਾ->je_options & JSON_JS)
				&& is_simple_ਕੁੰਜੀ(hi->hi_ਕੁੰਜੀ))
			    ga_concat(gap, hi->hi_ਕੁੰਜੀ);
			ਨਹੀਂਤਾਂ
			    write_string(gap, hi->hi_ਕੁੰਜੀ);
			ga_append(gap, ':');

			options = ਚੌਖਟਾ->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			ਰੋਕੋ;
		    }
		    ga_append(gap, '}');
		    ਚੌਖਟਾ->je_u.d.dict->dv_copyID = 0;
		    --ਢੇਰ.ga_len;
		    ਰੋਕੋ;
		}
	    }
	    ਜੇ (advance)
		ਰੋਕੋ;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    ਲਈ (i = 0; i < ਢੇਰ.ga_len; i++)
    {
	ਚੌਖਟਾ = ((json_enc_ਚੌਖਟਾ_T *)ਢੇਰ.ga_ਅੰਕੜੇ) + i;
	ਚੋਣ (ਚੌਖਟਾ->je_type)
	{
	    ਹਾਲਤ ENC_LIST:
		ਚੌਖਟਾ->je_u.l.list->lv_copyID = 0;
		ਰੋਕੋ;
	    ਹਾਲਤ ENC_TUPLE:
		ਚੌਖਟਾ->je_u.t.tuple->tv_copyID = 0;
		ਰੋਕੋ;
	    ਹਾਲਤ ENC_DICT:
		ਚੌਖਟਾ->je_u.d.dict->dv_copyID = 0;
		ਰੋਕੋ;
	}
    }
    ga_clear(&ਢੇਰ);
    ਵਾਪਸ FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    ਸਥਿਰ ਖਾਲੀ
fill_numbuflen(js_read_T *reader)
{
    ਜੇ (reader->js_fill != ਨਿਰਾ && (ਪੂਰਨ)(reader->js_ਸਿਰਾ - reader->js_buf)
						- reader->js_ਵਰਤਿਆ < NUMBUFLEN)
    {
	ਜੇ (reader->js_fill(reader))
	    reader->js_ਸਿਰਾ = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    ਸਥਿਰ ਖਾਲੀ
json_skip_white(js_read_T *reader)
{
    ਪੂਰਨ c;

    ਲਈ (;;)
    {
	c = reader->js_buf[reader->js_ਵਰਤਿਆ];
	ਜੇ (reader->js_fill != ਨਿਰਾ && c == NUL)
	{
	    ਜੇ (reader->js_fill(reader))
	    {
		reader->js_ਸਿਰਾ = reader->js_buf + STRLEN(reader->js_buf);
		ਜਾਰੀ;
	    }
	}
	ਜੇ (c == NUL || c > ' ')
	    ਰੋਕੋ;
	++reader->js_ਵਰਤਿਆ;
    }
    fill_numbuflen(reader);
}

    ਸਥਿਰ ਪੂਰਨ
json_decode_string(js_read_T *reader, typval_T *res, ਪੂਰਨ quote)
{
    garray_T    ga;
    ਪੂਰਨ		len;
    char_u	*p;
    ਪੂਰਨ		c;
    varnumber_T	nr;

    ਜੇ (res != ਨਿਰਾ)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_ਵਰਤਿਆ + 1; // skip over " or '
    ਜਦੋਂ (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	ਜੇ (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    ਜੇ (reader->js_fill == ਨਿਰਾ)
		ਰੋਕੋ;
	    len = (ਪੂਰਨ)(reader->js_ਸਿਰਾ - p);
	    reader->js_ਵਰਤਿਆ = (ਪੂਰਨ)(p - reader->js_buf);
	    ਜੇ (!reader->js_fill(reader))
		ਰੋਕੋ; // didn't get more
	    p = reader->js_buf + reader->js_ਵਰਤਿਆ;
	    reader->js_ਸਿਰਾ = reader->js_buf + STRLEN(reader->js_buf);
	    ਜਾਰੀ;
	}

	ਜੇ (*p == '\\')
	{
	    c = -1;
	    ਚੋਣ (p[1])
	    {
		ਹਾਲਤ '\\': c = '\\'; ਰੋਕੋ;
		ਹਾਲਤ '"': c = '"'; ਰੋਕੋ;
		ਹਾਲਤ 'b': c = BS; ਰੋਕੋ;
		ਹਾਲਤ 't': c = TAB; ਰੋਕੋ;
		ਹਾਲਤ 'n': c = NL; ਰੋਕੋ;
		ਹਾਲਤ 'f': c = FF; ਰੋਕੋ;
		ਹਾਲਤ 'r': c = CAR; ਰੋਕੋ;
		ਹਾਲਤ 'u':
		    ਜੇ (reader->js_fill != ਨਿਰਾ
				     && (ਪੂਰਨ)(reader->js_ਸਿਰਾ - p) < NUMBUFLEN)
		    {
			reader->js_ਵਰਤਿਆ = (ਪੂਰਨ)(p - reader->js_buf);
			ਜੇ (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_ਵਰਤਿਆ;
			    reader->js_ਸਿਰਾ = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, ਨਿਰਾ, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, ਨਿਰਾ, 4, TRUE, ਨਿਰਾ);
		    ਜੇ (len == 0)
		    {
			ਜੇ (res != ਨਿਰਾ)
			    ga_clear(&ga);
			ਵਾਪਸ FAIL;
		    }
		    p += len + 2;
		    ਜੇ (0xd800 <= nr && nr <= 0xdbff
			    && (ਪੂਰਨ)(reader->js_ਸਿਰਾ - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, ਨਿਰਾ, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, ਨਿਰਾ, 4, TRUE, ਨਿਰਾ);
			ਜੇ (len == 0)
			{
			    ਜੇ (res != ਨਿਰਾ)
				ga_clear(&ga);
			    ਵਾਪਸ FAIL;
			}
			ਜੇ (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    ਜੇ (0xd800 <= nr && nr <= 0xdfff)
		    {
			ਜੇ (res != ਨਿਰਾ)
			    ga_clear(&ga);
			ਵਾਪਸ FAIL;
		    }
		    ਜੇ (res != ਨਿਰਾ)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((ਪੂਰਨ)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    ਰੋਕੋ;
		ਮੂਲ:
		    // not a special char, skip over backslash
		    ++p;
		    ਜਾਰੀ;
	    }
	    ਜੇ (c > 0)
	    {
		p += 2;
		ਜੇ (res != ਨਿਰਾ)
		    ga_append(&ga, c);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    len = utf_ptr2len(p);
	    ਜੇ (res != ਨਿਰਾ)
	    {
		ਜੇ (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    ਵਾਪਸ FAIL;
		}
		mch_memmove((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_ਵਰਤਿਆ = (ਪੂਰਨ)(p - reader->js_buf);
    ਜੇ (*p == quote)
    {
	++reader->js_ਵਰਤਿਆ;
	ਜੇ (res != ਨਿਰਾ)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#ਜੇ defined(USE_ICONV)
	    ਜੇ (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_ਤਿਆਰੀ(&conv, (char_u*)"utf-8", p_enc);
		ਜੇ (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_ਅੰਕੜੇ, ਨਿਰਾ);
		    vim_free(ga.ga_ਅੰਕੜੇ);
		}
		convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		res->vval.v_string = ga.ga_ਅੰਕੜੇ;
	}
	ਵਾਪਸ OK;
    }
    ਜੇ (res != ਨਿਰਾ)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_ਸੰਖਿਆ = VVAL_NONE;
	ga_clear(&ga);
    }
    ਵਾਪਸ MAYBE;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_ਕੁੰਜੀ,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_ਕੁੰਜੀ_tv;
    char_u	  *jd_ਕੁੰਜੀ;
} json_dec_ਵਸਤੂ_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    ਸਥਿਰ ਪੂਰਨ
json_decode_ਵਸਤੂ(js_read_T *reader, typval_T *res, ਪੂਰਨ options)
{
    char_u	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		len;
    ਪੂਰਨ		retval;
    garray_T	ਢੇਰ;
    typval_T	ਵਸਤੂ;
    typval_T	*cur_ਵਸਤੂ;
    json_dec_ਵਸਤੂ_T *ਸਿਖਰ_ਵਸਤੂ;
    char_u	ਕੁੰਜੀ_buf[NUMBUFLEN];

    ga_init2(&ਢੇਰ, ਆਕਾਰ(json_dec_ਵਸਤੂ_T), 100);
    cur_ਵਸਤੂ = res;
    init_tv(&ਵਸਤੂ);
    ਜੇ (res != ਨਿਰਾ)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_ਵਰਤਿਆ;
    ਲਈ (;;)
    {
	ਸਿਖਰ_ਵਸਤੂ = ਨਿਰਾ;
	ਜੇ (ਢੇਰ.ga_len > 0)
	{
	    ਸਿਖਰ_ਵਸਤੂ = ((json_dec_ਵਸਤੂ_T *)ਢੇਰ.ga_ਅੰਕੜੇ) + ਢੇਰ.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_ਵਰਤਿਆ;
	    ਜੇ (*p == NUL)
	    {
		retval = MAYBE;
		ਜਾਓ theend;
	    }
	    ਜੇ (ਸਿਖਰ_ਵਸਤੂ->jd_type == JSON_OBJECT_ਕੁੰਜੀ
					    || ਸਿਖਰ_ਵਸਤੂ->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		ਜੇ (*p == (ਸਿਖਰ_ਵਸਤੂ->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_ਵਰਤਿਆ; // consume the ']' or '}'
		    --ਢੇਰ.ga_len;
		    ਜੇ (ਢੇਰ.ga_len == 0)
		    {
			retval = OK;
			ਜਾਓ theend;
		    }
		    ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
			cur_ਵਸਤੂ = &ਸਿਖਰ_ਵਸਤੂ->jd_tv;
		    ਜਾਓ ਵਸਤੂ_ਸਿਰਾ;
		}
	    }
	}

	ਜੇ (ਸਿਖਰ_ਵਸਤੂ != ਨਿਰਾ && ਸਿਖਰ_ਵਸਤੂ->jd_type == JSON_OBJECT_ਕੁੰਜੀ
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_ਵਰਤਿਆ] != '"'
		&& reader->js_buf[reader->js_ਵਰਤਿਆ] != '\''
		&& reader->js_buf[reader->js_ਵਰਤਿਆ] != '['
		&& reader->js_buf[reader->js_ਵਰਤਿਆ] != '{')
	{
	    char_u *ਕੁੰਜੀ;

	    // accept an object key that is not in quotes
	    ਕੁੰਜੀ = p = reader->js_buf + reader->js_ਵਰਤਿਆ;
	    ਜਦੋਂ (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
	    {
		cur_ਵਸਤੂ->v_type = VAR_STRING;
		cur_ਵਸਤੂ->vval.v_string = vim_strnsave(ਕੁੰਜੀ, p - ਕੁੰਜੀ);
		ਸਿਖਰ_ਵਸਤੂ->jd_ਕੁੰਜੀ = cur_ਵਸਤੂ->vval.v_string;
	    }
	    reader->js_ਵਰਤਿਆ += (ਪੂਰਨ)(p - ਕੁੰਜੀ);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਚੋਣ (*p)
	    {
		ਹਾਲਤ '[': // start of array
		    ਜੇ (ਸਿਖਰ_ਵਸਤੂ && ਸਿਖਰ_ਵਸਤੂ->jd_type == JSON_OBJECT_ਕੁੰਜੀ)
		    {
			retval = FAIL;
			ਰੋਕੋ;
		    }
		    ਜੇ (ਢੇਰ.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_ਡੂੰਘਾਈ_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ਰੋਕੋ;
		    }
		    ਜੇ (ga_grow(&ਢੇਰ, 1) == FAIL)
		    {
			retval = FAIL;
			ਰੋਕੋ;
		    }
		    ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ && rettv_list_alloc(cur_ਵਸਤੂ) == FAIL)
		    {
			cur_ਵਸਤੂ->v_type = VAR_SPECIAL;
			cur_ਵਸਤੂ->vval.v_ਸੰਖਿਆ = VVAL_NONE;
			retval = FAIL;
			ਰੋਕੋ;
		    }

		    ++reader->js_ਵਰਤਿਆ; // consume the '['
		    ਸਿਖਰ_ਵਸਤੂ = ((json_dec_ਵਸਤੂ_T *)ਢੇਰ.ga_ਅੰਕੜੇ)
								+ ਢੇਰ.ga_len;
		    ਸਿਖਰ_ਵਸਤੂ->jd_type = JSON_ARRAY;
		    ++ਢੇਰ.ga_len;
		    ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
		    {
			ਸਿਖਰ_ਵਸਤੂ->jd_tv = *cur_ਵਸਤੂ;
			cur_ਵਸਤੂ = &ਵਸਤੂ;
		    }
		    ਜਾਰੀ;

		ਹਾਲਤ '{': // start of object
		    ਜੇ (ਸਿਖਰ_ਵਸਤੂ && ਸਿਖਰ_ਵਸਤੂ->jd_type == JSON_OBJECT_ਕੁੰਜੀ)
		    {
			retval = FAIL;
			ਰੋਕੋ;
		    }
		    ਜੇ (ਢੇਰ.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_ਡੂੰਘਾਈ_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ਰੋਕੋ;
		    }
		    ਜੇ (ga_grow(&ਢੇਰ, 1) == FAIL)
		    {
			retval = FAIL;
			ਰੋਕੋ;
		    }
		    ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ && rettv_dict_alloc(cur_ਵਸਤੂ) == FAIL)
		    {
			cur_ਵਸਤੂ->v_type = VAR_SPECIAL;
			cur_ਵਸਤੂ->vval.v_ਸੰਖਿਆ = VVAL_NONE;
			retval = FAIL;
			ਰੋਕੋ;
		    }

		    ++reader->js_ਵਰਤਿਆ; // consume the '{'
		    ਸਿਖਰ_ਵਸਤੂ = ((json_dec_ਵਸਤੂ_T *)ਢੇਰ.ga_ਅੰਕੜੇ)
								+ ਢੇਰ.ga_len;
		    ਸਿਖਰ_ਵਸਤੂ->jd_type = JSON_OBJECT_ਕੁੰਜੀ;
		    ++ਢੇਰ.ga_len;
		    ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
		    {
			ਸਿਖਰ_ਵਸਤੂ->jd_tv = *cur_ਵਸਤੂ;
			cur_ਵਸਤੂ = &ਸਿਖਰ_ਵਸਤੂ->jd_ਕੁੰਜੀ_tv;
		    }
		    ਜਾਰੀ;

		ਹਾਲਤ '"': // string
		    retval = json_decode_string(reader, cur_ਵਸਤੂ, *p);
		    ਰੋਕੋ;

		ਹਾਲਤ '\'':
		    ਜੇ (options & JSON_JS)
			retval = json_decode_string(reader, cur_ਵਸਤੂ, *p);
		    ਨਹੀਂਤਾਂ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ਰੋਕੋ;

		ਹਾਲਤ ',': // comma: empty item
		    ਜੇ ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			ਰੋਕੋ;
		    }
		    // FALLTHROUGH
		ਹਾਲਤ NUL: //ਖਾਲੀ_ਥਾਵਾਂ
		    ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
		    {
			cur_ਵਸਤੂ->v_type = VAR_SPECIAL;
			cur_ਵਸਤੂ->vval.v_ਸੰਖਿਆ = VVAL_NONE;
		    }
		    retval = OK;
		    ਰੋਕੋ;

		ਮੂਲ:
		    ਜੇ (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			ਜੇ (*sp == '-')
			{
			    ++sp;
			    ਜੇ (*sp == NUL)
			    {
				retval = MAYBE;
				ਰੋਕੋ;
			    }
			    ਜੇ (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				ਰੋਕੋ;
			    }
			}
			sp = skipdigits(sp);
			ਜੇ (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    ਜੇ (cur_ਵਸਤੂ == ਨਿਰਾ)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    ਨਹੀਂਤਾਂ
			    {
				cur_ਵਸਤੂ->v_type = VAR_FLOAT;
				len = string2float(p, &cur_ਵਸਤੂ->vval.v_float,
									FALSE);
			    }
			}
			ਨਹੀਂਤਾਂ
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_ਵਰਤਿਆ,
				    ਨਿਰਾ, &len, 0, // what
				    &nr, ਨਿਰਾ, 0, TRUE, ਨਿਰਾ);
			    ਜੇ (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				ਜਾਓ theend;
			    }
			    ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
			    {
				cur_ਵਸਤੂ->v_type = VAR_ਸੰਖਿਆ;
				cur_ਵਸਤੂ->vval.v_ਸੰਖਿਆ = nr;
			    }
			}
			reader->js_ਵਰਤਿਆ += len;
			retval = OK;
			ਰੋਕੋ;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੇਲ_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((ਅੱਖਰ *)(p), (kw), (len)) == 0 \
     : STRNCMP((ਅੱਖਰ *)(p), (kw), (len)) == 0)
		    ਜੇ (ਮੇਲ_KW(p, "false", 5))
		    {
			reader->js_ਵਰਤਿਆ += 5;
			ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
			{
			    cur_ਵਸਤੂ->v_type = VAR_BOOL;
			    cur_ਵਸਤੂ->vval.v_ਸੰਖਿਆ = VVAL_FALSE;
			}
			retval = OK;
			ਰੋਕੋ;
		    }
		    ਜੇ (ਮੇਲ_KW(p, "true", 4))
		    {
			reader->js_ਵਰਤਿਆ += 4;
			ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
			{
			    cur_ਵਸਤੂ->v_type = VAR_BOOL;
			    cur_ਵਸਤੂ->vval.v_ਸੰਖਿਆ = VVAL_TRUE;
			}
			retval = OK;
			ਰੋਕੋ;
		    }
		    ਜੇ (ਮੇਲ_KW(p, "null", 4))
		    {
			reader->js_ਵਰਤਿਆ += 4;
			ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
			{
			    cur_ਵਸਤੂ->v_type = VAR_SPECIAL;
			    cur_ਵਸਤੂ->vval.v_ਸੰਖਿਆ = VVAL_NULL;
			}
			retval = OK;
			ਰੋਕੋ;
		    }
		    ਜੇ (ਮੇਲ_KW(p, "NaN", 3))
		    {
			reader->js_ਵਰਤਿਆ += 3;
			ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
			{
			    cur_ਵਸਤੂ->v_type = VAR_FLOAT;
			    cur_ਵਸਤੂ->vval.v_float = NAN;
			}
			retval = OK;
			ਰੋਕੋ;
		    }
		    ਜੇ (ਮੇਲ_KW(p, "-Infinity", 9))
		    {
			reader->js_ਵਰਤਿਆ += 9;
			ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
			{
			    cur_ਵਸਤੂ->v_type = VAR_FLOAT;
			    cur_ਵਸਤੂ->vval.v_float = -INFINITY;
			}
			retval = OK;
			ਰੋਕੋ;
		    }
		    ਜੇ (ਮੇਲ_KW(p, "Infinity", 8))
		    {
			reader->js_ਵਰਤਿਆ += 8;
			ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
			{
			    cur_ਵਸਤੂ->v_type = VAR_FLOAT;
			    cur_ਵਸਤੂ->vval.v_float = INFINITY;
			}
			retval = OK;
			ਰੋਕੋ;
		    }
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਮੇਲ_KW
		    // check for truncated name
		    len = (ਪੂਰਨ)(reader->js_ਸਿਰਾ
					 - (reader->js_buf + reader->js_ਵਰਤਿਆ));
		    ਜੇ (
			    (len < 5 && STRNICMP((ਅੱਖਰ *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((ਅੱਖਰ *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((ਅੱਖਰ *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((ਅੱਖਰ *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((ਅੱਖਰ *)p, "true", len) == 0
				    || STRNICMP((ਅੱਖਰ *)p, "null", len) == 0)))

			retval = MAYBE;
		    ਨਹੀਂਤਾਂ
			retval = FAIL;
		    ਰੋਕੋ;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    ਜੇ (retval == FAIL)
		ਰੋਕੋ;
	    ਜੇ (retval == MAYBE || ਢੇਰ.ga_len == 0)
		ਜਾਓ theend;

	    ਜੇ (ਸਿਖਰ_ਵਸਤੂ != ਨਿਰਾ && ਸਿਖਰ_ਵਸਤੂ->jd_type == JSON_OBJECT_ਕੁੰਜੀ
		    && cur_ਵਸਤੂ != ਨਿਰਾ)
	    {
		ਜੇ (cur_ਵਸਤੂ->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    ਜਾਓ theend;
		}
		ਸਿਖਰ_ਵਸਤੂ->jd_ਕੁੰਜੀ = tv_get_string_buf_chk(cur_ਵਸਤੂ, ਕੁੰਜੀ_buf);
		ਜੇ (ਸਿਖਰ_ਵਸਤੂ->jd_ਕੁੰਜੀ == ਨਿਰਾ)
		{
		    emsg(_(e_invalid_ਦਲੀਲ));
		    retval = FAIL;
		    ਜਾਓ theend;
		}
	    }
	}

ਵਸਤੂ_ਸਿਰਾ:
	ਸਿਖਰ_ਵਸਤੂ = ((json_dec_ਵਸਤੂ_T *)ਢੇਰ.ga_ਅੰਕੜੇ) + ਢੇਰ.ga_len - 1;
	ਚੋਣ (ਸਿਖਰ_ਵਸਤੂ->jd_type)
	{
	    ਹਾਲਤ JSON_ARRAY:
		ਜੇ (res != ਨਿਰਾ)
		{
		    listitem_T	*li = listitem_alloc();

		    ਜੇ (li == ਨਿਰਾ)
		    {
			clear_tv(cur_ਵਸਤੂ);
			retval = FAIL;
			ਜਾਓ theend;
		    }
		    li->li_tv = *cur_ਵਸਤੂ;
		    list_append(ਸਿਖਰ_ਵਸਤੂ->jd_tv.vval.v_list, li);
		}
		ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
		    cur_ਵਸਤੂ = &ਵਸਤੂ;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_ਵਰਤਿਆ;
		ਜੇ (*p == ',')
		    ++reader->js_ਵਰਤਿਆ;
		ਨਹੀਂਤਾਂ ਜੇ (*p != ']')
		{
		    ਜੇ (*p == NUL)
			retval = MAYBE;
		    ਨਹੀਂਤਾਂ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ਜਾਓ theend;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ JSON_OBJECT_ਕੁੰਜੀ:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_ਵਰਤਿਆ;
		ਜੇ (*p != ':')
		{
		    ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
			clear_tv(cur_ਵਸਤੂ);
		    ਜੇ (*p == NUL)
			retval = MAYBE;
		    ਨਹੀਂਤਾਂ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ਜਾਓ theend;
		}
		++reader->js_ਵਰਤਿਆ;
		json_skip_white(reader);
		ਸਿਖਰ_ਵਸਤੂ->jd_type = JSON_OBJECT;
		ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
		    cur_ਵਸਤੂ = &ਵਸਤੂ;
		ਰੋਕੋ;

	    ਹਾਲਤ JSON_OBJECT:
		ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ
			&& dict_has_ਕੁੰਜੀ(ਸਿਖਰ_ਵਸਤੂ->jd_tv.vval.v_dict,
						(ਅੱਖਰ *)ਸਿਖਰ_ਵਸਤੂ->jd_ਕੁੰਜੀ))
		{
		    semsg(_(e_duplicate_ਕੁੰਜੀ_ਵਿੱਚ_json_str), ਸਿਖਰ_ਵਸਤੂ->jd_ਕੁੰਜੀ);
		    clear_tv(cur_ਵਸਤੂ);
		    retval = FAIL;
		    ਜਾਓ theend;
		}

		ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
		{
		    dictitem_T *di = dictitem_alloc(ਸਿਖਰ_ਵਸਤੂ->jd_ਕੁੰਜੀ);

		    clear_tv(&ਸਿਖਰ_ਵਸਤੂ->jd_ਕੁੰਜੀ_tv);
		    ਜੇ (di == ਨਿਰਾ)
		    {
			clear_tv(cur_ਵਸਤੂ);
			retval = FAIL;
			ਜਾਓ theend;
		    }
		    di->di_tv = *cur_ਵਸਤੂ;
		    di->di_tv.v_ਤਾਲਾ = 0;
		    ਜੇ (dict_ਜੋੜੋ(ਸਿਖਰ_ਵਸਤੂ->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			ਜਾਓ theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_ਵਰਤਿਆ;
		ਜੇ (*p == ',')
		    ++reader->js_ਵਰਤਿਆ;
		ਨਹੀਂਤਾਂ ਜੇ (*p != '}')
		{
		    ਜੇ (*p == NUL)
			retval = MAYBE;
		    ਨਹੀਂਤਾਂ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ਜਾਓ theend;
		}
		ਸਿਖਰ_ਵਸਤੂ->jd_type = JSON_OBJECT_ਕੁੰਜੀ;
		ਜੇ (cur_ਵਸਤੂ != ਨਿਰਾ)
		    cur_ਵਸਤੂ = &ਸਿਖਰ_ਵਸਤੂ->jd_ਕੁੰਜੀ_tv;
		ਰੋਕੋ;
	}
    }

    // Get here when parsing failed.
    ਜੇ (res != ਨਿਰਾ)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_ਸੰਖਿਆ = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_ਵਰਤਿਆ);

theend:
    ਲਈ (i = 0; i < ਢੇਰ.ga_len; i++)
	clear_tv(&(((json_dec_ਵਸਤੂ_T *)ਢੇਰ.ga_ਅੰਕੜੇ) + i)->jd_ਕੁੰਜੀ_tv);
    ga_clear(&ਢੇਰ);

    ਵਾਪਸ retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    ਸਥਿਰ ਪੂਰਨ
json_decode_all(js_read_T *reader, typval_T *res, ਪੂਰਨ options)
{
    ਪੂਰਨ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_ਸਿਰਾ = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_ਵਸਤੂ(reader, res, options);
    ਜੇ (ret != OK)
    {
	ਜੇ (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	ਵਾਪਸ FAIL;
    }
    json_skip_white(reader);
    ਜੇ (reader->js_buf[reader->js_ਵਰਤਿਆ] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_ਵਰਤਿਆ);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    ਪੂਰਨ
json_decode(js_read_T *reader, typval_T *res, ਪੂਰਨ options)
{
    ਪੂਰਨ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_ਸਿਰਾ = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_ਵਸਤੂ(reader, res, options);
    json_skip_white(reader);

    ਵਾਪਸ ret;
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
json_ਲੱਭੋ_ਸਿਰਾ(js_read_T *reader, ਪੂਰਨ options)
{
    ਪੂਰਨ ਵਰਤਿਆ_save = reader->js_ਵਰਤਿਆ;
    ਪੂਰਨ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_ਸਿਰਾ = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_ਵਸਤੂ(reader, ਨਿਰਾ, options);
    reader->js_ਵਰਤਿਆ = ਵਰਤਿਆ_save;
    ਵਾਪਸ ret;
}

/*
 * "js_decode()" function
 */
    ਖਾਲੀ
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ਨਿਰਾ;
    reader.js_ਵਰਤਿਆ = 0;
    ਜੇ (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_ਦਲੀਲ));
}

/*
 * "js_encode()" function
 */
    ਖਾਲੀ
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    ਖਾਲੀ
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ਨਿਰਾ;
    reader.js_ਵਰਤਿਆ = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    ਖਾਲੀ
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#ਜੇ_ਅੰਤ
