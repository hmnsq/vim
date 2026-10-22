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
#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ json_encode_ಅಂಶ(garray_T *gap, typval_T *val, ಪೂರ್ಣಾಂಕ copyID, ಪೂರ್ಣಾಂಕ options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
json_encode_gap(garray_T *gap, typval_T *val, ಪೂರ್ಣಾಂಕ options)
{
    ಆದರೆ (json_encode_ಅಂಶ(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_ದತ್ತಾಂಶ = vim_strsave((char_u *)"");
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, ಪೂರ್ಣಾಂಕ options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    ಆದರೆ (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    ಹಿಂತಿರುಗಿ ga.ga_ದತ್ತಾಂಶ;
}

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_ಅಭಿವ್ಯಕ್ತಿ(ಪೂರ್ಣಾಂಕ nr, typval_T *val, ಪೂರ್ಣಾಂಕ options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_ಸಂಖ್ಯೆ;
    nrtv.vval.v_ಸಂಖ್ಯೆ = nr;
    ಆದರೆ (rettv_list_alloc(&listtv) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ga_init2(&ga, 1, 4000);
    ಆದರೆ (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    ಹಿಂತಿರುಗಿ ga.ga_ದತ್ತಾಂಶ;
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
    ಆದರೆ (json_encode_gap(&ga, val, 0) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((ಅಕ್ಷರ *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_ದತ್ತಾಂಶ, ga.ga_len);
    ga_clear(&ga);
    ಹಿಂತಿರುಗಿ lspga.ga_ದತ್ತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ascii_needs_escape[128] = {
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
#ಆದರೆ defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (res == ಖಾಲಿ)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	ಹಿಂತಿರುಗಿ;
    }

#ಆದರೆ defined(USE_ICONV)
    ಆದರೆ (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_ಸಿದ್ಧತೆ(&conv, p_enc, (char_u*)"utf-8");
	ಆದರೆ (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, ಖಾಲಿ);
	convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (ಪೂರ್ಣಾಂಕ)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    ಆಗ ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	ಆದರೆ (c < 0x80)
	{
	    ಆದರೆ (!ascii_needs_escape[c])
	    {
		res += 1;
		ಮುಂದುವರಿ;
	    }

	    ಆದರೆ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    ಆಯ್ಕೆ (c)
	    {
		ಸ್ಥಿತಿ 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
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
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    ಆದರೆ (l > 1)
	    {
		res += l;
		ಮುಂದುವರಿ;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    ಆದರೆ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    ಆದರೆ (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#ಆದರೆ defined(USE_ICONV)
    vim_free(converted);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_simple_ಕೀಲಿ(char_u *ಕೀಲಿ)
{
    char_u *p;

    ಆದರೆ (!ASCII_ISALPHA(*ಕೀಲಿ))
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (p = ಕೀಲಿ + 1; *p != NUL; ++p)
	ಆದರೆ (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    json_enc_type_T je_type;
    ಪೂರ್ಣಾಂಕ		    je_options;	    // options when this container was entered
    ಒಕ್ಕೂಟ {
	ರಚನೆ {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	ರಚನೆ {
	    tuple_T	*tuple;
	    ಪೂರ್ಣಾಂಕ		idx;	    // current index
	    ಪೂರ್ಣಾಂಕ		len;
	} t;
	ರಚನೆ {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    ಪೂರ್ಣಾಂಕ		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_ಚೌಕಟ್ಟು_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
json_encode_ಅಂಶ(garray_T *gap, typval_T *val, ಪೂರ್ಣಾಂಕ copyID, ಪೂರ್ಣಾಂಕ options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    ಪೂರ್ಣಾಂಕ			i;
    garray_T		ರಾಶಿ;
    typval_T		*cur_val;
    json_enc_ಚೌಕಟ್ಟು_T	*ಚೌಕಟ್ಟು;

    ga_init2(&ರಾಶಿ, ಗಾತ್ರ(json_enc_ಚೌಕಟ್ಟು_T), 100);
    cur_val = val;

    ಗಾಗಿ (;;)
    {
    ಆಯ್ಕೆ (cur_val->v_type)
    {
	ಸ್ಥಿತಿ VAR_BOOL:
	    ಆಯ್ಕೆ ((ದೀರ್ಘ)cur_val->vval.v_ಸಂಖ್ಯೆ)
	    {
		ಸ್ಥಿತಿ VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); ನಿಲ್ಲಿಸು;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_SPECIAL:
	    ಆಯ್ಕೆ ((ದೀರ್ಘ)cur_val->vval.v_ಸಂಖ್ಯೆ)
	    {
		ಸ್ಥಿತಿ VVAL_NONE: ಆದರೆ ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    ನಿಲ್ಲಿಸು;
				// FALLTHROUGH
		ಸ್ಥಿತಿ VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); ನಿಲ್ಲಿಸು;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((ಅಕ್ಷರ *)numbuf, ಗಾತ್ರ(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_ಸಂಖ್ಯೆ);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_OBJECT:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    ಹೋಗು theend;

	ಸ್ಥಿತಿ VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    ಆದರೆ (b == ಖಾಲಿ || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		ಆದರೆ (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    ಹೋಗು theend;
		src = (char_u *)b->bv_ga.ga_ದತ್ತಾಂಶ;
		dst = (char_u *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len;
		*dst++ = '[';
		ಗಾಗಿ (i = 0; i < blen; i++)
		{
		    ಪೂರ್ಣಾಂಕ	    byte = src[i];

		    ಆದರೆ (i > 0)
			*dst++ = ',';
		    ಆದರೆ (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (ಪೂರ್ಣಾಂಕ)(dst - (char_u *)gap->ga_ದತ್ತಾಂಶ);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_LIST:
	    l = cur_val->vval.v_list;
	    ಆದರೆ (l == ಖಾಲಿ)
		GA_CONCAT_LITERAL(gap, "[]");
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    ಆದರೆ (l->lv_ಮೊದಲ != ಖಾಲಿ)
		    {
			ಆದರೆ (ರಾಶಿ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_ಆಳ_is_higher_than_maxfuncdepth));
			    ಹೋಗು theend;
			}
			ಆದರೆ (ga_grow(&ರಾಶಿ, 1) == FAIL)
			    ಹೋಗು theend;
			ಚೌಕಟ್ಟು = ((json_enc_ಚೌಕಟ್ಟು_T *)ರಾಶಿ.ga_ದತ್ತಾಂಶ)
							    + ರಾಶಿ.ga_len;
			ಚೌಕಟ್ಟು->je_type = ENC_LIST;
			ಚೌಕಟ್ಟು->je_options = options;
			ಚೌಕಟ್ಟು->je_u.l.list = l;
			ಚೌಕಟ್ಟು->je_u.l.li = l->lv_ಮೊದಲ;
			++ರಾಶಿ.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_ಮೊದಲ->li_tv;
			ಮುಂದುವರಿ;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    ಆದರೆ (tuple == ಖಾಲಿ)
		GA_CONCAT_LITERAL(gap, "[]");
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ		len = TUPLE_LEN(tuple);

		    ಆದರೆ (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			ಆದರೆ (ರಾಶಿ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_ಆಳ_is_higher_than_maxfuncdepth));
			    ಹೋಗು theend;
			}
			ಆದರೆ (ga_grow(&ರಾಶಿ, 1) == FAIL)
			    ಹೋಗು theend;
			ಚೌಕಟ್ಟು = ((json_enc_ಚೌಕಟ್ಟು_T *)ರಾಶಿ.ga_ದತ್ತಾಂಶ)
							    + ರಾಶಿ.ga_len;
			ಚೌಕಟ್ಟು->je_type = ENC_TUPLE;
			ಚೌಕಟ್ಟು->je_options = options;
			ಚೌಕಟ್ಟು->je_u.t.tuple = tuple;
			ಚೌಕಟ್ಟು->je_u.t.idx = 0;
			ಚೌಕಟ್ಟು->je_u.t.len = len;
			++ರಾಶಿ.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ಅಂಶ(tuple, 0);
			ಮುಂದುವರಿ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_DICT:
	    d = cur_val->vval.v_dict;
	    ಆದರೆ (d == ಖಾಲಿ)
		GA_CONCAT_LITERAL(gap, "{}");
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ		todo = (ಪೂರ್ಣಾಂಕ)d->dv_hashtab.ht_ಬಳಸಿದ;
		    hashitem_T	*hi;

		    ಆದರೆ (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			ಗಾಗಿ (hi = d->dv_hashtab.ht_array;
						  HASHITEM_ಖಾಲಿಗಳು(hi); ++hi)
			    ;
			--todo;

			// Write first key
			ಆದರೆ ((options & JSON_JS)
						 && is_simple_ಕೀಲಿ(hi->hi_ಕೀಲಿ))
			    ga_concat(gap, hi->hi_ಕೀಲಿ);
			ಇಲ್ಲದಿದ್ದರೆ
			    write_string(gap, hi->hi_ಕೀಲಿ);
			ga_append(gap, ':');

			ಆದರೆ (ರಾಶಿ.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_ಆಳ_is_higher_than_maxfuncdepth));
			    ಹೋಗು theend;
			}
			ಆದರೆ (ga_grow(&ರಾಶಿ, 1) == FAIL)
			    ಹೋಗು theend;
			ಚೌಕಟ್ಟು = ((json_enc_ಚೌಕಟ್ಟು_T *)ರಾಶಿ.ga_ದತ್ತಾಂಶ)
							    + ರಾಶಿ.ga_len;
			ಚೌಕಟ್ಟು->je_type = ENC_DICT;
			ಚೌಕಟ್ಟು->je_options = options;
			ಚೌಕಟ್ಟು->je_u.d.dict = d;
			ಚೌಕಟ್ಟು->je_u.d.hi = hi;
			ಚೌಕಟ್ಟು->je_u.d.todo = todo;
			++ರಾಶಿ.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			ಮುಂದುವರಿ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_FLOAT:
#ಆದರೆ defined(HAVE_MATH_H)
	    ಆದರೆ (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isinf(cur_val->vval.v_float))
	    {
		ಆದರೆ (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		ಇಲ್ಲದಿದ್ದರೆ
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((ಅಕ್ಷರ *)numbuf, ಗಾತ್ರ(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	    ಆಂತರಿಕ_error_no_abort("json_encode_item()");
	    ಹೋಗು theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	ಗಾಗಿ (;;)
	{
	    ಪೂರ್ಣಾಂಕ		advance = FALSE;

	    ಆದರೆ (ರಾಶಿ.ga_len == 0)
	    {
		ga_clear(&ರಾಶಿ);
		ಹಿಂತಿರುಗಿ OK;
	    }

	    ಚೌಕಟ್ಟು = ((json_enc_ಚೌಕಟ್ಟು_T *)ರಾಶಿ.ga_ದತ್ತಾಂಶ)
							+ ರಾಶಿ.ga_len - 1;
	    ಆಯ್ಕೆ (ಚೌಕಟ್ಟು->je_type)
	    {
		ಸ್ಥಿತಿ ENC_LIST:
		{
		    listitem_T	*li = ಚೌಕಟ್ಟು->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    ಆದರೆ ((ಚೌಕಟ್ಟು->je_options & JSON_JS)
			    && li->li_ಮುಂದಿನ == ಖಾಲಿ
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_ಮುಂದಿನ;
		    ಚೌಕಟ್ಟು->je_u.l.li = li;
		    ಆದರೆ (li != ಖಾಲಿ && !got_int)
		    {
			ga_append(gap, ',');
			options = ಚೌಕಟ್ಟು->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
		    ga_append(gap, ']');
		    ಚೌಕಟ್ಟು->je_u.l.list->lv_copyID = 0;
		    --ರಾಶಿ.ga_len;
		    ನಿಲ್ಲಿಸು;
		}

		ಸ್ಥಿತಿ ENC_TUPLE:
		{
		    ಪೂರ್ಣಾಂಕ	    idx = ಚೌಕಟ್ಟು->je_u.t.idx;
		    ಪೂರ್ಣಾಂಕ	    len = ಚೌಕಟ್ಟು->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    ಆದರೆ ((ಚೌಕಟ್ಟು->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_ಅಂಶ =
				    TUPLE_ಅಂಶ(ಚೌಕಟ್ಟು->je_u.t.tuple, idx);

			ಆದರೆ (t_ಅಂಶ->v_type == VAR_SPECIAL
				&& t_ಅಂಶ->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    ಚೌಕಟ್ಟು->je_u.t.idx = idx;
		    ಆದರೆ (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = ಚೌಕಟ್ಟು->je_options & JSON_JS;
			cur_val = TUPLE_ಅಂಶ(ಚೌಕಟ್ಟು->je_u.t.tuple, idx);
			advance = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
		    ga_append(gap, ']');
		    ಚೌಕಟ್ಟು->je_u.t.tuple->tv_copyID = 0;
		    --ರಾಶಿ.ga_len;
		    ನಿಲ್ಲಿಸು;
		}

		ಸ್ಥಿತಿ ENC_DICT:
		{
		    hashitem_T	*hi = ಚೌಕಟ್ಟು->je_u.d.hi;
		    ಪೂರ್ಣಾಂಕ		todo = ಚೌಕಟ್ಟು->je_u.d.todo;

		    ಆದರೆ (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			ಗಾಗಿ (++hi; HASHITEM_ಖಾಲಿಗಳು(hi); ++hi)
			    ;
			--todo;
			ಚೌಕಟ್ಟು->je_u.d.hi = hi;
			ಚೌಕಟ್ಟು->je_u.d.todo = todo;

			ga_append(gap, ',');
			ಆದರೆ ((ಚೌಕಟ್ಟು->je_options & JSON_JS)
				&& is_simple_ಕೀಲಿ(hi->hi_ಕೀಲಿ))
			    ga_concat(gap, hi->hi_ಕೀಲಿ);
			ಇಲ್ಲದಿದ್ದರೆ
			    write_string(gap, hi->hi_ಕೀಲಿ);
			ga_append(gap, ':');

			options = ಚೌಕಟ್ಟು->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
		    ga_append(gap, '}');
		    ಚೌಕಟ್ಟು->je_u.d.dict->dv_copyID = 0;
		    --ರಾಶಿ.ga_len;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಆದರೆ (advance)
		ನಿಲ್ಲಿಸು;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    ಗಾಗಿ (i = 0; i < ರಾಶಿ.ga_len; i++)
    {
	ಚೌಕಟ್ಟು = ((json_enc_ಚೌಕಟ್ಟು_T *)ರಾಶಿ.ga_ದತ್ತಾಂಶ) + i;
	ಆಯ್ಕೆ (ಚೌಕಟ್ಟು->je_type)
	{
	    ಸ್ಥಿತಿ ENC_LIST:
		ಚೌಕಟ್ಟು->je_u.l.list->lv_copyID = 0;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ENC_TUPLE:
		ಚೌಕಟ್ಟು->je_u.t.tuple->tv_copyID = 0;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ENC_DICT:
		ಚೌಕಟ್ಟು->je_u.d.dict->dv_copyID = 0;
		ನಿಲ್ಲಿಸು;
	}
    }
    ga_clear(&ರಾಶಿ);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fill_numbuflen(js_read_T *reader)
{
    ಆದರೆ (reader->js_fill != ಖಾಲಿ && (ಪೂರ್ಣಾಂಕ)(reader->js_ಕೊನೆ - reader->js_buf)
						- reader->js_ಬಳಸಿದ < NUMBUFLEN)
    {
	ಆದರೆ (reader->js_fill(reader))
	    reader->js_ಕೊನೆ = reader->js_buf + STRLEN(reader->js_buf);
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
    ಪೂರ್ಣಾಂಕ c;

    ಗಾಗಿ (;;)
    {
	c = reader->js_buf[reader->js_ಬಳಸಿದ];
	ಆದರೆ (reader->js_fill != ಖಾಲಿ && c == NUL)
	{
	    ಆದರೆ (reader->js_fill(reader))
	    {
		reader->js_ಕೊನೆ = reader->js_buf + STRLEN(reader->js_buf);
		ಮುಂದುವರಿ;
	    }
	}
	ಆದರೆ (c == NUL || c > ' ')
	    ನಿಲ್ಲಿಸು;
	++reader->js_ಬಳಸಿದ;
    }
    fill_numbuflen(reader);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
json_decode_string(js_read_T *reader, typval_T *res, ಪೂರ್ಣಾಂಕ quote)
{
    garray_T    ga;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		c;
    varnumber_T	nr;

    ಆದರೆ (res != ಖಾಲಿ)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_ಬಳಸಿದ + 1; // skip over " or '
    ಆಗ (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	ಆದರೆ (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    ಆದರೆ (reader->js_fill == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    len = (ಪೂರ್ಣಾಂಕ)(reader->js_ಕೊನೆ - p);
	    reader->js_ಬಳಸಿದ = (ಪೂರ್ಣಾಂಕ)(p - reader->js_buf);
	    ಆದರೆ (!reader->js_fill(reader))
		ನಿಲ್ಲಿಸು; // didn't get more
	    p = reader->js_buf + reader->js_ಬಳಸಿದ;
	    reader->js_ಕೊನೆ = reader->js_buf + STRLEN(reader->js_buf);
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (*p == '\\')
	{
	    c = -1;
	    ಆಯ್ಕೆ (p[1])
	    {
		ಸ್ಥಿತಿ '\\': c = '\\'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '"': c = '"'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'b': c = BS; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 't': c = TAB; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'n': c = NL; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'f': c = FF; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'r': c = CAR; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'u':
		    ಆದರೆ (reader->js_fill != ಖಾಲಿ
				     && (ಪೂರ್ಣಾಂಕ)(reader->js_ಕೊನೆ - p) < NUMBUFLEN)
		    {
			reader->js_ಬಳಸಿದ = (ಪೂರ್ಣಾಂಕ)(p - reader->js_buf);
			ಆದರೆ (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_ಬಳಸಿದ;
			    reader->js_ಕೊನೆ = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, ಖಾಲಿ, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, ಖಾಲಿ, 4, TRUE, ಖಾಲಿ);
		    ಆದರೆ (len == 0)
		    {
			ಆದರೆ (res != ಖಾಲಿ)
			    ga_clear(&ga);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    p += len + 2;
		    ಆದರೆ (0xd800 <= nr && nr <= 0xdbff
			    && (ಪೂರ್ಣಾಂಕ)(reader->js_ಕೊನೆ - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, ಖಾಲಿ, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, ಖಾಲಿ, 4, TRUE, ಖಾಲಿ);
			ಆದರೆ (len == 0)
			{
			    ಆದರೆ (res != ಖಾಲಿ)
				ga_clear(&ga);
			    ಹಿಂತಿರುಗಿ FAIL;
			}
			ಆದರೆ (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    ಆದರೆ (0xd800 <= nr && nr <= 0xdfff)
		    {
			ಆದರೆ (res != ಖಾಲಿ)
			    ga_clear(&ga);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    ಆದರೆ (res != ಖಾಲಿ)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((ಪೂರ್ಣಾಂಕ)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    // not a special char, skip over backslash
		    ++p;
		    ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (c > 0)
	    {
		p += 2;
		ಆದರೆ (res != ಖಾಲಿ)
		    ga_append(&ga, c);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    len = utf_ptr2len(p);
	    ಆದರೆ (res != ಖಾಲಿ)
	    {
		ಆದರೆ (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		mch_memmove((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_ಬಳಸಿದ = (ಪೂರ್ಣಾಂಕ)(p - reader->js_buf);
    ಆದರೆ (*p == quote)
    {
	++reader->js_ಬಳಸಿದ;
	ಆದರೆ (res != ಖಾಲಿ)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#ಆದರೆ defined(USE_ICONV)
	    ಆದರೆ (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_ಸಿದ್ಧತೆ(&conv, (char_u*)"utf-8", p_enc);
		ಆದರೆ (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_ದತ್ತಾಂಶ, ಖಾಲಿ);
		    vim_free(ga.ga_ದತ್ತಾಂಶ);
		}
		convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		res->vval.v_string = ga.ga_ದತ್ತಾಂಶ;
	}
	ಹಿಂತಿರುಗಿ OK;
    }
    ಆದರೆ (res != ಖಾಲಿ)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
	ga_clear(&ga);
    }
    ಹಿಂತಿರುಗಿ MAYBE;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_ಕೀಲಿ,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_ಕೀಲಿ_tv;
    char_u	  *jd_ಕೀಲಿ;
} json_dec_ಅಂಶ_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
json_decode_ಅಂಶ(js_read_T *reader, typval_T *res, ಪೂರ್ಣಾಂಕ options)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		retval;
    garray_T	ರಾಶಿ;
    typval_T	ಅಂಶ;
    typval_T	*cur_ಅಂಶ;
    json_dec_ಅಂಶ_T *ತುದಿ_ಅಂಶ;
    char_u	ಕೀಲಿ_buf[NUMBUFLEN];

    ga_init2(&ರಾಶಿ, ಗಾತ್ರ(json_dec_ಅಂಶ_T), 100);
    cur_ಅಂಶ = res;
    init_tv(&ಅಂಶ);
    ಆದರೆ (res != ಖಾಲಿ)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_ಬಳಸಿದ;
    ಗಾಗಿ (;;)
    {
	ತುದಿ_ಅಂಶ = ಖಾಲಿ;
	ಆದರೆ (ರಾಶಿ.ga_len > 0)
	{
	    ತುದಿ_ಅಂಶ = ((json_dec_ಅಂಶ_T *)ರಾಶಿ.ga_ದತ್ತಾಂಶ) + ರಾಶಿ.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_ಬಳಸಿದ;
	    ಆದರೆ (*p == NUL)
	    {
		retval = MAYBE;
		ಹೋಗು theend;
	    }
	    ಆದರೆ (ತುದಿ_ಅಂಶ->jd_type == JSON_OBJECT_ಕೀಲಿ
					    || ತುದಿ_ಅಂಶ->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		ಆದರೆ (*p == (ತುದಿ_ಅಂಶ->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_ಬಳಸಿದ; // consume the ']' or '}'
		    --ರಾಶಿ.ga_len;
		    ಆದರೆ (ರಾಶಿ.ga_len == 0)
		    {
			retval = OK;
			ಹೋಗು theend;
		    }
		    ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
			cur_ಅಂಶ = &ತುದಿ_ಅಂಶ->jd_tv;
		    ಹೋಗು ಅಂಶ_ಕೊನೆ;
		}
	    }
	}

	ಆದರೆ (ತುದಿ_ಅಂಶ != ಖಾಲಿ && ತುದಿ_ಅಂಶ->jd_type == JSON_OBJECT_ಕೀಲಿ
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_ಬಳಸಿದ] != '"'
		&& reader->js_buf[reader->js_ಬಳಸಿದ] != '\''
		&& reader->js_buf[reader->js_ಬಳಸಿದ] != '['
		&& reader->js_buf[reader->js_ಬಳಸಿದ] != '{')
	{
	    char_u *ಕೀಲಿ;

	    // accept an object key that is not in quotes
	    ಕೀಲಿ = p = reader->js_buf + reader->js_ಬಳಸಿದ;
	    ಆಗ (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
	    {
		cur_ಅಂಶ->v_type = VAR_STRING;
		cur_ಅಂಶ->vval.v_string = vim_strnsave(ಕೀಲಿ, p - ಕೀಲಿ);
		ತುದಿ_ಅಂಶ->jd_ಕೀಲಿ = cur_ಅಂಶ->vval.v_string;
	    }
	    reader->js_ಬಳಸಿದ += (ಪೂರ್ಣಾಂಕ)(p - ಕೀಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆಯ್ಕೆ (*p)
	    {
		ಸ್ಥಿತಿ '[': // start of array
		    ಆದರೆ (ತುದಿ_ಅಂಶ && ತುದಿ_ಅಂಶ->jd_type == JSON_OBJECT_ಕೀಲಿ)
		    {
			retval = FAIL;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (ರಾಶಿ.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_ಆಳ_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (ga_grow(&ರಾಶಿ, 1) == FAIL)
		    {
			retval = FAIL;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ && rettv_list_alloc(cur_ಅಂಶ) == FAIL)
		    {
			cur_ಅಂಶ->v_type = VAR_SPECIAL;
			cur_ಅಂಶ->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
			retval = FAIL;
			ನಿಲ್ಲಿಸು;
		    }

		    ++reader->js_ಬಳಸಿದ; // consume the '['
		    ತುದಿ_ಅಂಶ = ((json_dec_ಅಂಶ_T *)ರಾಶಿ.ga_ದತ್ತಾಂಶ)
								+ ರಾಶಿ.ga_len;
		    ತುದಿ_ಅಂಶ->jd_type = JSON_ARRAY;
		    ++ರಾಶಿ.ga_len;
		    ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
		    {
			ತುದಿ_ಅಂಶ->jd_tv = *cur_ಅಂಶ;
			cur_ಅಂಶ = &ಅಂಶ;
		    }
		    ಮುಂದುವರಿ;

		ಸ್ಥಿತಿ '{': // start of object
		    ಆದರೆ (ತುದಿ_ಅಂಶ && ತುದಿ_ಅಂಶ->jd_type == JSON_OBJECT_ಕೀಲಿ)
		    {
			retval = FAIL;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (ರಾಶಿ.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_ಆಳ_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (ga_grow(&ರಾಶಿ, 1) == FAIL)
		    {
			retval = FAIL;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ && rettv_dict_alloc(cur_ಅಂಶ) == FAIL)
		    {
			cur_ಅಂಶ->v_type = VAR_SPECIAL;
			cur_ಅಂಶ->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
			retval = FAIL;
			ನಿಲ್ಲಿಸು;
		    }

		    ++reader->js_ಬಳಸಿದ; // consume the '{'
		    ತುದಿ_ಅಂಶ = ((json_dec_ಅಂಶ_T *)ರಾಶಿ.ga_ದತ್ತಾಂಶ)
								+ ರಾಶಿ.ga_len;
		    ತುದಿ_ಅಂಶ->jd_type = JSON_OBJECT_ಕೀಲಿ;
		    ++ರಾಶಿ.ga_len;
		    ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
		    {
			ತುದಿ_ಅಂಶ->jd_tv = *cur_ಅಂಶ;
			cur_ಅಂಶ = &ತುದಿ_ಅಂಶ->jd_ಕೀಲಿ_tv;
		    }
		    ಮುಂದುವರಿ;

		ಸ್ಥಿತಿ '"': // string
		    retval = json_decode_string(reader, cur_ಅಂಶ, *p);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '\'':
		    ಆದರೆ (options & JSON_JS)
			retval = json_decode_string(reader, cur_ಅಂಶ, *p);
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ ',': // comma: empty item
		    ಆದರೆ ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			ನಿಲ್ಲಿಸು;
		    }
		    // FALLTHROUGH
		ಸ್ಥಿತಿ NUL: //ಖಾಲಿಗಳು
		    ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
		    {
			cur_ಅಂಶ->v_type = VAR_SPECIAL;
			cur_ಅಂಶ->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
		    }
		    retval = OK;
		    ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
		    ಆದರೆ (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			ಆದರೆ (*sp == '-')
			{
			    ++sp;
			    ಆದರೆ (*sp == NUL)
			    {
				retval = MAYBE;
				ನಿಲ್ಲಿಸು;
			    }
			    ಆದರೆ (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				ನಿಲ್ಲಿಸು;
			    }
			}
			sp = skipdigits(sp);
			ಆದರೆ (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    ಆದರೆ (cur_ಅಂಶ == ಖಾಲಿ)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				cur_ಅಂಶ->v_type = VAR_FLOAT;
				len = string2float(p, &cur_ಅಂಶ->vval.v_float,
									FALSE);
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_ಬಳಸಿದ,
				    ಖಾಲಿ, &len, 0, // what
				    &nr, ಖಾಲಿ, 0, TRUE, ಖಾಲಿ);
			    ಆದರೆ (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				ಹೋಗು theend;
			    }
			    ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
			    {
				cur_ಅಂಶ->v_type = VAR_ಸಂಖ್ಯೆ;
				cur_ಅಂಶ->vval.v_ಸಂಖ್ಯೆ = nr;
			    }
			}
			reader->js_ಬಳಸಿದ += len;
			retval = OK;
			ನಿಲ್ಲಿಸು;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#ವ್ಯಾಖ್ಯಾನಿಸು ಹೊಂದಿಕೆ_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((ಅಕ್ಷರ *)(p), (kw), (len)) == 0 \
     : STRNCMP((ಅಕ್ಷರ *)(p), (kw), (len)) == 0)
		    ಆದರೆ (ಹೊಂದಿಕೆ_KW(p, "false", 5))
		    {
			reader->js_ಬಳಸಿದ += 5;
			ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
			{
			    cur_ಅಂಶ->v_type = VAR_BOOL;
			    cur_ಅಂಶ->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
			}
			retval = OK;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (ಹೊಂದಿಕೆ_KW(p, "true", 4))
		    {
			reader->js_ಬಳಸಿದ += 4;
			ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
			{
			    cur_ಅಂಶ->v_type = VAR_BOOL;
			    cur_ಅಂಶ->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
			}
			retval = OK;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (ಹೊಂದಿಕೆ_KW(p, "null", 4))
		    {
			reader->js_ಬಳಸಿದ += 4;
			ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
			{
			    cur_ಅಂಶ->v_type = VAR_SPECIAL;
			    cur_ಅಂಶ->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
			}
			retval = OK;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (ಹೊಂದಿಕೆ_KW(p, "NaN", 3))
		    {
			reader->js_ಬಳಸಿದ += 3;
			ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
			{
			    cur_ಅಂಶ->v_type = VAR_FLOAT;
			    cur_ಅಂಶ->vval.v_float = NAN;
			}
			retval = OK;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (ಹೊಂದಿಕೆ_KW(p, "-Infinity", 9))
		    {
			reader->js_ಬಳಸಿದ += 9;
			ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
			{
			    cur_ಅಂಶ->v_type = VAR_FLOAT;
			    cur_ಅಂಶ->vval.v_float = -INFINITY;
			}
			retval = OK;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (ಹೊಂದಿಕೆ_KW(p, "Infinity", 8))
		    {
			reader->js_ಬಳಸಿದ += 8;
			ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
			{
			    cur_ಅಂಶ->v_type = VAR_FLOAT;
			    cur_ಅಂಶ->vval.v_float = INFINITY;
			}
			retval = OK;
			ನಿಲ್ಲಿಸು;
		    }
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಹೊಂದಿಕೆ_KW
		    // check for truncated name
		    len = (ಪೂರ್ಣಾಂಕ)(reader->js_ಕೊನೆ
					 - (reader->js_buf + reader->js_ಬಳಸಿದ));
		    ಆದರೆ (
			    (len < 5 && STRNICMP((ಅಕ್ಷರ *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((ಅಕ್ಷರ *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((ಅಕ್ಷರ *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((ಅಕ್ಷರ *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((ಅಕ್ಷರ *)p, "true", len) == 0
				    || STRNICMP((ಅಕ್ಷರ *)p, "null", len) == 0)))

			retval = MAYBE;
		    ಇಲ್ಲದಿದ್ದರೆ
			retval = FAIL;
		    ನಿಲ್ಲಿಸು;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    ಆದರೆ (retval == FAIL)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (retval == MAYBE || ರಾಶಿ.ga_len == 0)
		ಹೋಗು theend;

	    ಆದರೆ (ತುದಿ_ಅಂಶ != ಖಾಲಿ && ತುದಿ_ಅಂಶ->jd_type == JSON_OBJECT_ಕೀಲಿ
		    && cur_ಅಂಶ != ಖಾಲಿ)
	    {
		ಆದರೆ (cur_ಅಂಶ->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    ಹೋಗು theend;
		}
		ತುದಿ_ಅಂಶ->jd_ಕೀಲಿ = tv_get_string_buf_chk(cur_ಅಂಶ, ಕೀಲಿ_buf);
		ಆದರೆ (ತುದಿ_ಅಂಶ->jd_ಕೀಲಿ == ಖಾಲಿ)
		{
		    emsg(_(e_invalid_ವಾದ));
		    retval = FAIL;
		    ಹೋಗು theend;
		}
	    }
	}

ಅಂಶ_ಕೊನೆ:
	ತುದಿ_ಅಂಶ = ((json_dec_ಅಂಶ_T *)ರಾಶಿ.ga_ದತ್ತಾಂಶ) + ರಾಶಿ.ga_len - 1;
	ಆಯ್ಕೆ (ತುದಿ_ಅಂಶ->jd_type)
	{
	    ಸ್ಥಿತಿ JSON_ARRAY:
		ಆದರೆ (res != ಖಾಲಿ)
		{
		    listitem_T	*li = listitem_alloc();

		    ಆದರೆ (li == ಖಾಲಿ)
		    {
			clear_tv(cur_ಅಂಶ);
			retval = FAIL;
			ಹೋಗು theend;
		    }
		    li->li_tv = *cur_ಅಂಶ;
		    list_append(ತುದಿ_ಅಂಶ->jd_tv.vval.v_list, li);
		}
		ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
		    cur_ಅಂಶ = &ಅಂಶ;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_ಬಳಸಿದ;
		ಆದರೆ (*p == ',')
		    ++reader->js_ಬಳಸಿದ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p != ']')
		{
		    ಆದರೆ (*p == NUL)
			retval = MAYBE;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ಹೋಗು theend;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ JSON_OBJECT_ಕೀಲಿ:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_ಬಳಸಿದ;
		ಆದರೆ (*p != ':')
		{
		    ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
			clear_tv(cur_ಅಂಶ);
		    ಆದರೆ (*p == NUL)
			retval = MAYBE;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ಹೋಗು theend;
		}
		++reader->js_ಬಳಸಿದ;
		json_skip_white(reader);
		ತುದಿ_ಅಂಶ->jd_type = JSON_OBJECT;
		ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
		    cur_ಅಂಶ = &ಅಂಶ;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ JSON_OBJECT:
		ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ
			&& dict_has_ಕೀಲಿ(ತುದಿ_ಅಂಶ->jd_tv.vval.v_dict,
						(ಅಕ್ಷರ *)ತುದಿ_ಅಂಶ->jd_ಕೀಲಿ))
		{
		    semsg(_(e_duplicate_ಕೀಲಿ_ಒಳ_json_str), ತುದಿ_ಅಂಶ->jd_ಕೀಲಿ);
		    clear_tv(cur_ಅಂಶ);
		    retval = FAIL;
		    ಹೋಗು theend;
		}

		ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
		{
		    dictitem_T *di = dictitem_alloc(ತುದಿ_ಅಂಶ->jd_ಕೀಲಿ);

		    clear_tv(&ತುದಿ_ಅಂಶ->jd_ಕೀಲಿ_tv);
		    ಆದರೆ (di == ಖಾಲಿ)
		    {
			clear_tv(cur_ಅಂಶ);
			retval = FAIL;
			ಹೋಗು theend;
		    }
		    di->di_tv = *cur_ಅಂಶ;
		    di->di_tv.v_ಬೀಗ = 0;
		    ಆದರೆ (dict_ಕೂಡಿಸು(ತುದಿ_ಅಂಶ->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			ಹೋಗು theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_ಬಳಸಿದ;
		ಆದರೆ (*p == ',')
		    ++reader->js_ಬಳಸಿದ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p != '}')
		{
		    ಆದರೆ (*p == NUL)
			retval = MAYBE;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ಹೋಗು theend;
		}
		ತುದಿ_ಅಂಶ->jd_type = JSON_OBJECT_ಕೀಲಿ;
		ಆದರೆ (cur_ಅಂಶ != ಖಾಲಿ)
		    cur_ಅಂಶ = &ತುದಿ_ಅಂಶ->jd_ಕೀಲಿ_tv;
		ನಿಲ್ಲಿಸು;
	}
    }

    // Get here when parsing failed.
    ಆದರೆ (res != ಖಾಲಿ)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_ಬಳಸಿದ);

theend:
    ಗಾಗಿ (i = 0; i < ರಾಶಿ.ga_len; i++)
	clear_tv(&(((json_dec_ಅಂಶ_T *)ರಾಶಿ.ga_ದತ್ತಾಂಶ) + i)->jd_ಕೀಲಿ_tv);
    ga_clear(&ರಾಶಿ);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
json_decode_all(js_read_T *reader, typval_T *res, ಪೂರ್ಣಾಂಕ options)
{
    ಪೂರ್ಣಾಂಕ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_ಕೊನೆ = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_ಅಂಶ(reader, res, options);
    ಆದರೆ (ret != OK)
    {
	ಆದರೆ (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    json_skip_white(reader);
    ಆದರೆ (reader->js_buf[reader->js_ಬಳಸಿದ] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_ಬಳಸಿದ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    ಪೂರ್ಣಾಂಕ
json_decode(js_read_T *reader, typval_T *res, ಪೂರ್ಣಾಂಕ options)
{
    ಪೂರ್ಣಾಂಕ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_ಕೊನೆ = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_ಅಂಶ(reader, res, options);
    json_skip_white(reader);

    ಹಿಂತಿರುಗಿ ret;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
json_ಕಂಡುಹಿಡಿ_ಕೊನೆ(js_read_T *reader, ಪೂರ್ಣಾಂಕ options)
{
    ಪೂರ್ಣಾಂಕ ಬಳಸಿದ_save = reader->js_ಬಳಸಿದ;
    ಪೂರ್ಣಾಂಕ ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_ಕೊನೆ = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_ಅಂಶ(reader, ಖಾಲಿ, options);
    reader->js_ಬಳಸಿದ = ಬಳಸಿದ_save;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * "js_decode()" function
 */
    ಶೂನ್ಯ
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ಖಾಲಿ;
    reader.js_ಬಳಸಿದ = 0;
    ಆದರೆ (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_ವಾದ));
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

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ಖಾಲಿ;
    reader.js_ಬಳಸಿದ = 0;
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
#ಆದರೆ_ಅಂತ್ಯ
