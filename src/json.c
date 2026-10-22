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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ json_encode_item(garray_T *gap, typval_T *val, ಪೂರ್ಣಾಙ್ಕ copyID, ಪೂರ್ಣಾಙ್ಕ options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
json_encode_gap(garray_T *gap, typval_T *val, ಪೂರ್ಣಾಙ್ಕ options)
{
    ಯದಿ (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, ಪೂರ್ಣಾಙ್ಕ options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    ಯದಿ (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    ಪ್ರತ್ಯಾಗಮ ga.ga_data;
}

#ಯದಿ defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_ಅಭಿವ್ಯಕ್ತಿ(ಪೂರ್ಣಾಙ್ಕ nr, typval_T *val, ಪೂರ್ಣಾಙ್ಕ options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_ಸಂಖ್ಯೆ;
    nrtv.vval.v_ಸಂಖ್ಯೆ = nr;
    ಯದಿ (rettv_list_alloc(&listtv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ga_init2(&ga, 1, 4000);
    ಯದಿ (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    ಪ್ರತ್ಯಾಗಮ ga.ga_data;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_ಸಂದೇಶ(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    ಯದಿ (json_encode_gap(&ga, val, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((ವರ್ಣ *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    ಪ್ರತ್ಯಾಗಮ lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ ascii_needs_escape[128] = {
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
    ಸ್ಥಿರ ಶೂನ್ಯ
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#ಯದಿ defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = ರಿಕ್ತ;
#endif
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (res == ರಿಕ್ತ)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	ಪ್ರತ್ಯಾಗಮ;
    }

#ಯದಿ defined(USE_ICONV)
    ಯದಿ (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	ಯದಿ (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, ರಿಕ್ತ);
	convert_setup(&conv, ರಿಕ್ತ, ರಿಕ್ತ);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    ಯಾವತ್ ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	ಯದಿ (c < 0x80)
	{
	    ಯದಿ (!ascii_needs_escape[c])
	    {
		res += 1;
		ಅನುವರ್ತ;
	    }

	    ಯದಿ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    ವಿಕಲ್ಪ (c)
	    {
		ಪಕ್ಷ 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); ಭಙ್ಗ;
		ಪಕ್ಷ 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); ಭಙ್ಗ;
		ಪಕ್ಷ 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); ಭಙ್ಗ;
		ಪಕ್ಷ 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); ಭಙ್ಗ;
		ಪಕ್ಷ 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); ಭಙ್ಗ;
		ಪಕ್ಷ 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); ಭಙ್ಗ;
		ಪಕ್ಷ 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); ಭಙ್ಗ;
		ಸ್ವಭಾವ:
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
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    ಯದಿ (l > 1)
	    {
		res += l;
		ಅನುವರ್ತ;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    ಯದಿ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    ಯದಿ (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#ಯದಿ defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_simple_ಕೀಲಿ(char_u *ಕೀಲಿ)
{
    char_u *p;

    ಯದಿ (!ASCII_ISALPHA(*ಕೀಲಿ))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತಿ (p = ಕೀಲಿ + 1; *p != NUL; ++p)
	ಯದಿ (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    json_enc_type_T je_type;
    ಪೂರ್ಣಾಙ್ಕ		    je_options;	    // options when this container was entered
    ಸಙ್ಘ {
	ಸಂರಚನಾ {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	ಸಂರಚನಾ {
	    tuple_T	*tuple;
	    ಪೂರ್ಣಾಙ್ಕ		idx;	    // current index
	    ಪೂರ್ಣಾಙ್ಕ		len;
	} t;
	ಸಂರಚನಾ {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    ಪೂರ್ಣಾಙ್ಕ		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
json_encode_item(garray_T *gap, typval_T *val, ಪೂರ್ಣಾಙ್ಕ copyID, ಪೂರ್ಣಾಙ್ಕ options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    ಪೂರ್ಣಾಙ್ಕ			i;
    garray_T		ರಾಶಿ;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&ರಾಶಿ, ಪರಿಮಾಣ(json_enc_frame_T), 100);
    cur_val = val;

    ಪ್ರತಿ (;;)
    {
    ವಿಕಲ್ಪ (cur_val->v_type)
    {
	ಪಕ್ಷ VAR_BOOL:
	    ವಿಕಲ್ಪ ((long)cur_val->vval.v_ಸಂಖ್ಯೆ)
	    {
		ಪಕ್ಷ VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); ಭಙ್ಗ;
		ಪಕ್ಷ VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); ಭಙ್ಗ;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_SPECIAL:
	    ವಿಕಲ್ಪ ((long)cur_val->vval.v_ಸಂಖ್ಯೆ)
	    {
		ಪಕ್ಷ VVAL_NONE: ಯದಿ ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    ಭಙ್ಗ;
				// FALLTHROUGH
		ಪಕ್ಷ VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); ಭಙ್ಗ;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((ವರ್ಣ *)numbuf, ಪರಿಮಾಣ(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_ಸಂಖ್ಯೆ);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_INSTR:
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_OBJECT:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    ಗಚ್ಛ theend;

	ಪಕ್ಷ VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    ಯದಿ (b == ರಿಕ್ತ || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    ಅನ್ಯಥಾ
	    {
		ಪೂರ್ಣಾಙ್ಕ	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		ಯದಿ (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    ಗಚ್ಛ theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		ಪ್ರತಿ (i = 0; i < blen; i++)
		{
		    ಪೂರ್ಣಾಙ್ಕ	    byte = src[i];

		    ಯದಿ (i > 0)
			*dst++ = ',';
		    ಯದಿ (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    ಅನ್ಯಥಾ
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (ಪೂರ್ಣಾಙ್ಕ)(dst - (char_u *)gap->ga_data);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_LIST:
	    l = cur_val->vval.v_list;
	    ಯದಿ (l == ರಿಕ್ತ)
		GA_CONCAT_LITERAL(gap, "[]");
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ಅನ್ಯಥಾ
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    ಯದಿ (l->lv_first != ರಿಕ್ತ)
		    {
			ಯದಿ (ರಾಶಿ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    ಗಚ್ಛ theend;
			}
			ಯದಿ (ga_grow(&ರಾಶಿ, 1) == FAIL)
			    ಗಚ್ಛ theend;
			frame = ((json_enc_frame_T *)ರಾಶಿ.ga_data)
							    + ರಾಶಿ.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++ರಾಶಿ.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			ಅನುವರ್ತ;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    ಯದಿ (tuple == ರಿಕ್ತ)
		GA_CONCAT_LITERAL(gap, "[]");
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ		len = TUPLE_LEN(tuple);

		    ಯದಿ (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			ಯದಿ (ರಾಶಿ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    ಗಚ್ಛ theend;
			}
			ಯದಿ (ga_grow(&ರಾಶಿ, 1) == FAIL)
			    ಗಚ್ಛ theend;
			frame = ((json_enc_frame_T *)ರಾಶಿ.ga_data)
							    + ರಾಶಿ.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++ರಾಶಿ.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			ಅನುವರ್ತ;
		    }
		    ಅನ್ಯಥಾ
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_DICT:
	    d = cur_val->vval.v_dict;
	    ಯದಿ (d == ರಿಕ್ತ)
		GA_CONCAT_LITERAL(gap, "{}");
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ		todo = (ಪೂರ್ಣಾಙ್ಕ)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    ಯದಿ (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			ಪ್ರತಿ (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			ಯದಿ ((options & JSON_JS)
						 && is_simple_ಕೀಲಿ(hi->hi_ಕೀಲಿ))
			    ga_concat(gap, hi->hi_ಕೀಲಿ);
			ಅನ್ಯಥಾ
			    write_string(gap, hi->hi_ಕೀಲಿ);
			ga_append(gap, ':');

			ಯದಿ (ರಾಶಿ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    ಗಚ್ಛ theend;
			}
			ಯದಿ (ga_grow(&ರಾಶಿ, 1) == FAIL)
			    ಗಚ್ಛ theend;
			frame = ((json_enc_frame_T *)ರಾಶಿ.ga_data)
							    + ರಾಶಿ.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++ರಾಶಿ.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			ಅನುವರ್ತ;
		    }
		    ಅನ್ಯಥಾ
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_FLOAT:
#ಯದಿ defined(HAVE_MATH_H)
	    ಯದಿ (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    ಅನ್ಯಥಾ ಯದಿ (isinf(cur_val->vval.v_float))
	    {
		ಯದಿ (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		ಅನ್ಯಥಾ
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    ಅನ್ಯಥಾ
#endif
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((ವರ್ಣ *)numbuf, ಪರಿಮಾಣ(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    ಗಚ್ಛ theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	ಪ್ರತಿ (;;)
	{
	    ಪೂರ್ಣಾಙ್ಕ		advance = FALSE;

	    ಯದಿ (ರಾಶಿ.ga_len == 0)
	    {
		ga_clear(&ರಾಶಿ);
		ಪ್ರತ್ಯಾಗಮ OK;
	    }

	    frame = ((json_enc_frame_T *)ರಾಶಿ.ga_data)
							+ ರಾಶಿ.ga_len - 1;
	    ವಿಕಲ್ಪ (frame->je_type)
	    {
		ಪಕ್ಷ ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    ಯದಿ ((frame->je_options & JSON_JS)
			    && li->li_next == ರಿಕ್ತ
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    ಯದಿ (li != ರಿಕ್ತ && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			ಭಙ್ಗ;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --ರಾಶಿ.ga_len;
		    ಭಙ್ಗ;
		}

		ಪಕ್ಷ ENC_TUPLE:
		{
		    ಪೂರ್ಣಾಙ್ಕ	    idx = frame->je_u.t.idx;
		    ಪೂರ್ಣಾಙ್ಕ	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    ಯದಿ ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			ಯದಿ (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    ಯದಿ (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			ಭಙ್ಗ;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --ರಾಶಿ.ga_len;
		    ಭಙ್ಗ;
		}

		ಪಕ್ಷ ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    ಪೂರ್ಣಾಙ್ಕ		todo = frame->je_u.d.todo;

		    ಯದಿ (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			ಪ್ರತಿ (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			ಯದಿ ((frame->je_options & JSON_JS)
				&& is_simple_ಕೀಲಿ(hi->hi_ಕೀಲಿ))
			    ga_concat(gap, hi->hi_ಕೀಲಿ);
			ಅನ್ಯಥಾ
			    write_string(gap, hi->hi_ಕೀಲಿ);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			ಭಙ್ಗ;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --ರಾಶಿ.ga_len;
		    ಭಙ್ಗ;
		}
	    }
	    ಯದಿ (advance)
		ಭಙ್ಗ;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    ಪ್ರತಿ (i = 0; i < ರಾಶಿ.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)ರಾಶಿ.ga_data) + i;
	ವಿಕಲ್ಪ (frame->je_type)
	{
	    ಪಕ್ಷ ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		ಭಙ್ಗ;
	    ಪಕ್ಷ ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		ಭಙ್ಗ;
	    ಪಕ್ಷ ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		ಭಙ್ಗ;
	}
    }
    ga_clear(&ರಾಶಿ);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fill_numbuflen(js_read_T *reader)
{
    ಯದಿ (reader->js_fill != ರಿಕ್ತ && (ಪೂರ್ಣಾಙ್ಕ)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	ಯದಿ (reader->js_fill(reader))
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
json_skip_white(js_read_T *reader)
{
    ಪೂರ್ಣಾಙ್ಕ c;

    ಪ್ರತಿ (;;)
    {
	c = reader->js_buf[reader->js_used];
	ಯದಿ (reader->js_fill != ರಿಕ್ತ && c == NUL)
	{
	    ಯದಿ (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		ಅನುವರ್ತ;
	    }
	}
	ಯದಿ (c == NUL || c > ' ')
	    ಭಙ್ಗ;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
json_decode_string(js_read_T *reader, typval_T *res, ಪೂರ್ಣಾಙ್ಕ quote)
{
    garray_T    ga;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		c;
    varnumber_T	nr;

    ಯದಿ (res != ರಿಕ್ತ)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    ಯಾವತ್ (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	ಯದಿ (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    ಯದಿ (reader->js_fill == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    len = (ಪೂರ್ಣಾಙ್ಕ)(reader->js_end - p);
	    reader->js_used = (ಪೂರ್ಣಾಙ್ಕ)(p - reader->js_buf);
	    ಯದಿ (!reader->js_fill(reader))
		ಭಙ್ಗ; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    ಅನುವರ್ತ;
	}

	ಯದಿ (*p == '\\')
	{
	    c = -1;
	    ವಿಕಲ್ಪ (p[1])
	    {
		ಪಕ್ಷ '\\': c = '\\'; ಭಙ್ಗ;
		ಪಕ್ಷ '"': c = '"'; ಭಙ್ಗ;
		ಪಕ್ಷ 'b': c = BS; ಭಙ್ಗ;
		ಪಕ್ಷ 't': c = TAB; ಭಙ್ಗ;
		ಪಕ್ಷ 'n': c = NL; ಭಙ್ಗ;
		ಪಕ್ಷ 'f': c = FF; ಭಙ್ಗ;
		ಪಕ್ಷ 'r': c = CAR; ಭಙ್ಗ;
		ಪಕ್ಷ 'u':
		    ಯದಿ (reader->js_fill != ರಿಕ್ತ
				     && (ಪೂರ್ಣಾಙ್ಕ)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (ಪೂರ್ಣಾಙ್ಕ)(p - reader->js_buf);
			ಯದಿ (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, ರಿಕ್ತ, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, ರಿಕ್ತ, 4, TRUE, ರಿಕ್ತ);
		    ಯದಿ (len == 0)
		    {
			ಯದಿ (res != ರಿಕ್ತ)
			    ga_clear(&ga);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		    p += len + 2;
		    ಯದಿ (0xd800 <= nr && nr <= 0xdbff
			    && (ಪೂರ್ಣಾಙ್ಕ)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, ರಿಕ್ತ, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, ರಿಕ್ತ, 4, TRUE, ರಿಕ್ತ);
			ಯದಿ (len == 0)
			{
			    ಯದಿ (res != ರಿಕ್ತ)
				ga_clear(&ga);
			    ಪ್ರತ್ಯಾಗಮ FAIL;
			}
			ಯದಿ (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    ಯದಿ (0xd800 <= nr && nr <= 0xdfff)
		    {
			ಯದಿ (res != ರಿಕ್ತ)
			    ga_clear(&ga);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		    ಯದಿ (res != ರಿಕ್ತ)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((ಪೂರ್ಣಾಙ್ಕ)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    // not a special char, skip over backslash
		    ++p;
		    ಅನುವರ್ತ;
	    }
	    ಯದಿ (c > 0)
	    {
		p += 2;
		ಯದಿ (res != ರಿಕ್ತ)
		    ga_append(&ga, c);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    len = utf_ptr2len(p);
	    ಯದಿ (res != ರಿಕ್ತ)
	    {
		ಯದಿ (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		mch_memmove((ವರ್ಣ *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (ಪೂರ್ಣಾಙ್ಕ)(p - reader->js_buf);
    ಯದಿ (*p == quote)
    {
	++reader->js_used;
	ಯದಿ (res != ರಿಕ್ತ)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#ಯದಿ defined(USE_ICONV)
	    ಯದಿ (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		ಯದಿ (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, ರಿಕ್ತ);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, ರಿಕ್ತ, ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
#endif
		res->vval.v_string = ga.ga_data;
	}
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಯದಿ (res != ರಿಕ್ತ)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
	ga_clear(&ga);
    }
    ಪ್ರತ್ಯಾಗಮ MAYBE;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_ಕೀಲಿ,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_ಕೀಲಿ_tv;
    char_u	  *jd_ಕೀಲಿ;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
json_decode_item(js_read_T *reader, typval_T *res, ಪೂರ್ಣಾಙ್ಕ options)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		retval;
    garray_T	ರಾಶಿ;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *ತುದಿ_item;
    char_u	ಕೀಲಿ_buf[NUMBUFLEN];

    ga_init2(&ರಾಶಿ, ಪರಿಮಾಣ(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    ಯದಿ (res != ರಿಕ್ತ)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    ಪ್ರತಿ (;;)
    {
	ತುದಿ_item = ರಿಕ್ತ;
	ಯದಿ (ರಾಶಿ.ga_len > 0)
	{
	    ತುದಿ_item = ((json_dec_item_T *)ರಾಶಿ.ga_data) + ರಾಶಿ.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    ಯದಿ (*p == NUL)
	    {
		retval = MAYBE;
		ಗಚ್ಛ theend;
	    }
	    ಯದಿ (ತುದಿ_item->jd_type == JSON_OBJECT_ಕೀಲಿ
					    || ತುದಿ_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		ಯದಿ (*p == (ತುದಿ_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --ರಾಶಿ.ga_len;
		    ಯದಿ (ರಾಶಿ.ga_len == 0)
		    {
			retval = OK;
			ಗಚ್ಛ theend;
		    }
		    ಯದಿ (cur_item != ರಿಕ್ತ)
			cur_item = &ತುದಿ_item->jd_tv;
		    ಗಚ್ಛ item_end;
		}
	    }
	}

	ಯದಿ (ತುದಿ_item != ರಿಕ್ತ && ತುದಿ_item->jd_type == JSON_OBJECT_ಕೀಲಿ
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *ಕೀಲಿ;

	    // accept an object key that is not in quotes
	    ಕೀಲಿ = p = reader->js_buf + reader->js_used;
	    ಯಾವತ್ (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    ಯದಿ (cur_item != ರಿಕ್ತ)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(ಕೀಲಿ, p - ಕೀಲಿ);
		ತುದಿ_item->jd_ಕೀಲಿ = cur_item->vval.v_string;
	    }
	    reader->js_used += (ಪೂರ್ಣಾಙ್ಕ)(p - ಕೀಲಿ);
	}
	ಅನ್ಯಥಾ
	{
	    ವಿಕಲ್ಪ (*p)
	    {
		ಪಕ್ಷ '[': // start of array
		    ಯದಿ (ತುದಿ_item && ತುದಿ_item->jd_type == JSON_OBJECT_ಕೀಲಿ)
		    {
			retval = FAIL;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (ರಾಶಿ.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (ga_grow(&ರಾಶಿ, 1) == FAIL)
		    {
			retval = FAIL;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (cur_item != ರಿಕ್ತ && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
			retval = FAIL;
			ಭಙ್ಗ;
		    }

		    ++reader->js_used; // consume the '['
		    ತುದಿ_item = ((json_dec_item_T *)ರಾಶಿ.ga_data)
								+ ರಾಶಿ.ga_len;
		    ತುದಿ_item->jd_type = JSON_ARRAY;
		    ++ರಾಶಿ.ga_len;
		    ಯದಿ (cur_item != ರಿಕ್ತ)
		    {
			ತುದಿ_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    ಅನುವರ್ತ;

		ಪಕ್ಷ '{': // start of object
		    ಯದಿ (ತುದಿ_item && ತುದಿ_item->jd_type == JSON_OBJECT_ಕೀಲಿ)
		    {
			retval = FAIL;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (ರಾಶಿ.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (ga_grow(&ರಾಶಿ, 1) == FAIL)
		    {
			retval = FAIL;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (cur_item != ರಿಕ್ತ && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
			retval = FAIL;
			ಭಙ್ಗ;
		    }

		    ++reader->js_used; // consume the '{'
		    ತುದಿ_item = ((json_dec_item_T *)ರಾಶಿ.ga_data)
								+ ರಾಶಿ.ga_len;
		    ತುದಿ_item->jd_type = JSON_OBJECT_ಕೀಲಿ;
		    ++ರಾಶಿ.ga_len;
		    ಯದಿ (cur_item != ರಿಕ್ತ)
		    {
			ತುದಿ_item->jd_tv = *cur_item;
			cur_item = &ತುದಿ_item->jd_ಕೀಲಿ_tv;
		    }
		    ಅನುವರ್ತ;

		ಪಕ್ಷ '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    ಭಙ್ಗ;

		ಪಕ್ಷ '\'':
		    ಯದಿ (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    ಅನ್ಯಥಾ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ಭಙ್ಗ;

		ಪಕ್ಷ ',': // comma: empty item
		    ಯದಿ ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			ಭಙ್ಗ;
		    }
		    // FALLTHROUGH
		ಪಕ್ಷ NUL: // empty
		    ಯದಿ (cur_item != ರಿಕ್ತ)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
		    }
		    retval = OK;
		    ಭಙ್ಗ;

		ಸ್ವಭಾವ:
		    ಯದಿ (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			ಯದಿ (*sp == '-')
			{
			    ++sp;
			    ಯದಿ (*sp == NUL)
			    {
				retval = MAYBE;
				ಭಙ್ಗ;
			    }
			    ಯದಿ (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				ಭಙ್ಗ;
			    }
			}
			sp = skipdigits(sp);
			ಯದಿ (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    ಯದಿ (cur_item == ರಿಕ್ತ)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    ಅನ್ಯಥಾ
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			ಅನ್ಯಥಾ
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    ರಿಕ್ತ, &len, 0, // what
				    &nr, ರಿಕ್ತ, 0, TRUE, ರಿಕ್ತ);
			    ಯದಿ (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				ಗಚ್ಛ theend;
			    }
			    ಯದಿ (cur_item != ರಿಕ್ತ)
			    {
				cur_item->v_type = VAR_ಸಂಖ್ಯೆ;
				cur_item->vval.v_ಸಂಖ್ಯೆ = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			ಭಙ್ಗ;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((ವರ್ಣ *)(p), (kw), (len)) == 0 \
     : STRNCMP((ವರ್ಣ *)(p), (kw), (len)) == 0)
		    ಯದಿ (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			ಯದಿ (cur_item != ರಿಕ್ತ)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
			}
			retval = OK;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			ಯದಿ (cur_item != ರಿಕ್ತ)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
			}
			retval = OK;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			ಯದಿ (cur_item != ರಿಕ್ತ)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
			}
			retval = OK;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			ಯದಿ (cur_item != ರಿಕ್ತ)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			ಯದಿ (cur_item != ರಿಕ್ತ)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			ಯದಿ (cur_item != ರಿಕ್ತ)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			ಭಙ್ಗ;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (ಪೂರ್ಣಾಙ್ಕ)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    ಯದಿ (
			    (len < 5 && STRNICMP((ವರ್ಣ *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((ವರ್ಣ *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((ವರ್ಣ *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((ವರ್ಣ *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((ವರ್ಣ *)p, "true", len) == 0
				    || STRNICMP((ವರ್ಣ *)p, "null", len) == 0)))

			retval = MAYBE;
		    ಅನ್ಯಥಾ
			retval = FAIL;
		    ಭಙ್ಗ;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    ಯದಿ (retval == FAIL)
		ಭಙ್ಗ;
	    ಯದಿ (retval == MAYBE || ರಾಶಿ.ga_len == 0)
		ಗಚ್ಛ theend;

	    ಯದಿ (ತುದಿ_item != ರಿಕ್ತ && ತುದಿ_item->jd_type == JSON_OBJECT_ಕೀಲಿ
		    && cur_item != ರಿಕ್ತ)
	    {
		ಯದಿ (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    ಗಚ್ಛ theend;
		}
		ತುದಿ_item->jd_ಕೀಲಿ = tv_get_string_buf_chk(cur_item, ಕೀಲಿ_buf);
		ಯದಿ (ತುದಿ_item->jd_ಕೀಲಿ == ರಿಕ್ತ)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    ಗಚ್ಛ theend;
		}
	    }
	}

item_end:
	ತುದಿ_item = ((json_dec_item_T *)ರಾಶಿ.ga_data) + ರಾಶಿ.ga_len - 1;
	ವಿಕಲ್ಪ (ತುದಿ_item->jd_type)
	{
	    ಪಕ್ಷ JSON_ARRAY:
		ಯದಿ (res != ರಿಕ್ತ)
		{
		    listitem_T	*li = listitem_alloc();

		    ಯದಿ (li == ರಿಕ್ತ)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			ಗಚ್ಛ theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(ತುದಿ_item->jd_tv.vval.v_list, li);
		}
		ಯದಿ (cur_item != ರಿಕ್ತ)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ಯದಿ (*p == ',')
		    ++reader->js_used;
		ಅನ್ಯಥಾ ಯದಿ (*p != ']')
		{
		    ಯದಿ (*p == NUL)
			retval = MAYBE;
		    ಅನ್ಯಥಾ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ಗಚ್ಛ theend;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ JSON_OBJECT_ಕೀಲಿ:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ಯದಿ (*p != ':')
		{
		    ಯದಿ (cur_item != ರಿಕ್ತ)
			clear_tv(cur_item);
		    ಯದಿ (*p == NUL)
			retval = MAYBE;
		    ಅನ್ಯಥಾ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ಗಚ್ಛ theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		ತುದಿ_item->jd_type = JSON_OBJECT;
		ಯದಿ (cur_item != ರಿಕ್ತ)
		    cur_item = &item;
		ಭಙ್ಗ;

	    ಪಕ್ಷ JSON_OBJECT:
		ಯದಿ (cur_item != ರಿಕ್ತ
			&& dict_has_ಕೀಲಿ(ತುದಿ_item->jd_tv.vval.v_dict,
						(ವರ್ಣ *)ತುದಿ_item->jd_ಕೀಲಿ))
		{
		    semsg(_(e_duplicate_ಕೀಲಿ_in_json_str), ತುದಿ_item->jd_ಕೀಲಿ);
		    clear_tv(cur_item);
		    retval = FAIL;
		    ಗಚ್ಛ theend;
		}

		ಯದಿ (cur_item != ರಿಕ್ತ)
		{
		    dictitem_T *di = dictitem_alloc(ತುದಿ_item->jd_ಕೀಲಿ);

		    clear_tv(&ತುದಿ_item->jd_ಕೀಲಿ_tv);
		    ಯದಿ (di == ರಿಕ್ತ)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			ಗಚ್ಛ theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    ಯದಿ (dict_add(ತುದಿ_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			ಗಚ್ಛ theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ಯದಿ (*p == ',')
		    ++reader->js_used;
		ಅನ್ಯಥಾ ಯದಿ (*p != '}')
		{
		    ಯದಿ (*p == NUL)
			retval = MAYBE;
		    ಅನ್ಯಥಾ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ಗಚ್ಛ theend;
		}
		ತುದಿ_item->jd_type = JSON_OBJECT_ಕೀಲಿ;
		ಯದಿ (cur_item != ರಿಕ್ತ)
		    cur_item = &ತುದಿ_item->jd_ಕೀಲಿ_tv;
		ಭಙ್ಗ;
	}
    }

    // Get here when parsing failed.
    ಯದಿ (res != ರಿಕ್ತ)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    ಪ್ರತಿ (i = 0; i < ರಾಶಿ.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)ರಾಶಿ.ga_data) + i)->jd_ಕೀಲಿ_tv);
    ga_clear(&ರಾಶಿ);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
json_decode_all(js_read_T *reader, typval_T *res, ಪೂರ್ಣಾಙ್ಕ options)
{
    ಪೂರ್ಣಾಙ್ಕ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    ಯದಿ (ret != OK)
    {
	ಯದಿ (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    json_skip_white(reader);
    ಯದಿ (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    ಪೂರ್ಣಾಙ್ಕ
json_decode(js_read_T *reader, typval_T *res, ಪೂರ್ಣಾಙ್ಕ options)
{
    ಪೂರ್ಣಾಙ್ಕ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    ಪ್ರತ್ಯಾಗಮ ret;
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
    ಪೂರ್ಣಾಙ್ಕ
json_find_end(js_read_T *reader, ಪೂರ್ಣಾಙ್ಕ options)
{
    ಪೂರ್ಣಾಙ್ಕ used_save = reader->js_used;
    ಪೂರ್ಣಾಙ್ಕ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, ರಿಕ್ತ, options);
    reader->js_used = used_save;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * "js_decode()" function
 */
    ಶೂನ್ಯ
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ರಿಕ್ತ;
    reader.js_used = 0;
    ಯದಿ (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_argument));
}

/*
 * "js_encode()" function
 */
    ಶೂನ್ಯ
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    ಶೂನ್ಯ
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ರಿಕ್ತ;
    reader.js_used = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    ಶೂನ್ಯ
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#endif
