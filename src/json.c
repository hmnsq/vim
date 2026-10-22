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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା json_encode_item(garray_T *gap, typval_T *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
json_encode_gap(garray_T *gap, typval_T *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    ଯଦି (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    ଯଦି (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    ଫେରନ୍ତୁ ga.ga_data;
}

#ଯଦି defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_ପଦସମଷ୍ଟି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, typval_T *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_ସଂଖ୍ୟା;
    nrtv.vval.v_ସଂଖ୍ୟା = nr;
    ଯଦି (rettv_list_alloc(&listtv) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ga_init2(&ga, 1, 4000);
    ଯଦି (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    ଫେରନ୍ତୁ ga.ga_data;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_ସନ୍ଦେଶ(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    ଯଦି (json_encode_gap(&ga, val, 0) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((ଅକ୍ଷର *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    ଫେରନ୍ତୁ lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର ascii_needs_escape[128] = {
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
    ସ୍ଥିର ଶୂନ୍ୟ
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#ଯଦି defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = ଖାଲି;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (res == ଖାଲି)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	ଫେରନ୍ତୁ;
    }

#ଯଦି defined(USE_ICONV)
    ଯଦି (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	ଯଦି (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, ଖାଲି);
	convert_setup(&conv, ଖାଲି, ଖାଲି);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    ଯେତେବେଳେ ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	ଯଦି (c < 0x80)
	{
	    ଯଦି (!ascii_needs_escape[c])
	    {
		res += 1;
		ଜାରି;
	    }

	    ଯଦି (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    ବାଛ (c)
	    {
		ସ୍ଥିତି 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); ଭାଙ୍ଗ;
		ସ୍ଥିତି 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); ଭାଙ୍ଗ;
		ସ୍ଥିତି 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); ଭାଙ୍ଗ;
		ସ୍ଥିତି 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); ଭାଙ୍ଗ;
		ସ୍ଥିତି 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); ଭାଙ୍ଗ;
		ସ୍ଥିତି 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); ଭାଙ୍ଗ;
		ସ୍ଥିତି 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
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
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    ଯଦି (l > 1)
	    {
		res += l;
		ଜାରି;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    ଯଦି (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    ଯଦି (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#ଯଦି defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_simple_ଚାବି(char_u *ଚାବି)
{
    char_u *p;

    ଯଦି (!ASCII_ISALPHA(*ଚାବି))
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (p = ଚାବି + 1; *p != NUL; ++p)
	ଯଦି (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    json_enc_type_T je_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    je_options;	    // options when this container was entered
    ସଂଘ {
	ସଂରଚନା {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	ସଂରଚନା {
	    tuple_T	*tuple;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;	    // current index
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
	} t;
	ସଂରଚନା {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
json_encode_item(garray_T *gap, typval_T *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    garray_T		ଗଦା;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&ଗଦା, ଆକାର(json_enc_frame_T), 100);
    cur_val = val;

    ପାଇଁ (;;)
    {
    ବାଛ (cur_val->v_type)
    {
	ସ୍ଥିତି VAR_BOOL:
	    ବାଛ ((long)cur_val->vval.v_ସଂଖ୍ୟା)
	    {
		ସ୍ଥିତି VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); ଭାଙ୍ଗ;
		ସ୍ଥିତି VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); ଭାଙ୍ଗ;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_SPECIAL:
	    ବାଛ ((long)cur_val->vval.v_ସଂଖ୍ୟା)
	    {
		ସ୍ଥିତି VVAL_NONE: ଯଦି ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    ଭାଙ୍ଗ;
				// FALLTHROUGH
		ସ୍ଥିତି VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); ଭାଙ୍ଗ;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((ଅକ୍ଷର *)numbuf, ଆକାର(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_ସଂଖ୍ୟା);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_OBJECT:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    ଯାଅ theend;

	ସ୍ଥିତି VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    ଯଦି (b == ଖାଲି || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		ଯଦି (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    ଯାଅ theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		ପାଇଁ (i = 0; i < blen; i++)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    byte = src[i];

		    ଯଦି (i > 0)
			*dst++ = ',';
		    ଯଦି (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    ଅନ୍ୟଥା ଯଦି (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    ଅନ୍ୟଥା
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(dst - (char_u *)gap->ga_data);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_LIST:
	    l = cur_val->vval.v_list;
	    ଯଦି (l == ଖାଲି)
		GA_CONCAT_LITERAL(gap, "[]");
	    ଅନ୍ୟଥା
	    {
		ଯଦି (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ଅନ୍ୟଥା
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    ଯଦି (l->lv_first != ଖାଲି)
		    {
			ଯଦି (ଗଦା.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    ଯାଅ theend;
			}
			ଯଦି (ga_grow(&ଗଦା, 1) == FAIL)
			    ଯାଅ theend;
			frame = ((json_enc_frame_T *)ଗଦା.ga_data)
							    + ଗଦା.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++ଗଦା.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			ଜାରି;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    ଯଦି (tuple == ଖାଲି)
		GA_CONCAT_LITERAL(gap, "[]");
	    ଅନ୍ୟଥା
	    {
		ଯଦି (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ଅନ୍ୟଥା
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = TUPLE_LEN(tuple);

		    ଯଦି (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			ଯଦି (ଗଦା.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    ଯାଅ theend;
			}
			ଯଦି (ga_grow(&ଗଦା, 1) == FAIL)
			    ଯାଅ theend;
			frame = ((json_enc_frame_T *)ଗଦା.ga_data)
							    + ଗଦା.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++ଗଦା.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			ଜାରି;
		    }
		    ଅନ୍ୟଥା
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_DICT:
	    d = cur_val->vval.v_dict;
	    ଯଦି (d == ଖାଲି)
		GA_CONCAT_LITERAL(gap, "{}");
	    ଅନ୍ୟଥା
	    {
		ଯଦି (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		ଅନ୍ୟଥା
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    ଯଦି (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			ପାଇଁ (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			ଯଦି ((options & JSON_JS)
						 && is_simple_ଚାବି(hi->hi_ଚାବି))
			    ga_concat(gap, hi->hi_ଚାବି);
			ଅନ୍ୟଥା
			    write_string(gap, hi->hi_ଚାବି);
			ga_append(gap, ':');

			ଯଦି (ଗଦା.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    ଯାଅ theend;
			}
			ଯଦି (ga_grow(&ଗଦା, 1) == FAIL)
			    ଯାଅ theend;
			frame = ((json_enc_frame_T *)ଗଦା.ga_data)
							    + ଗଦା.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++ଗଦା.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			ଜାରି;
		    }
		    ଅନ୍ୟଥା
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_FLOAT:
#ଯଦି defined(HAVE_MATH_H)
	    ଯଦି (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    ଅନ୍ୟଥା ଯଦି (isinf(cur_val->vval.v_float))
	    {
		ଯଦି (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		ଅନ୍ୟଥା
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((ଅକ୍ଷର *)numbuf, ଆକାର(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    ଯାଅ theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	ପାଇଁ (;;)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		advance = FALSE;

	    ଯଦି (ଗଦା.ga_len == 0)
	    {
		ga_clear(&ଗଦା);
		ଫେରନ୍ତୁ OK;
	    }

	    frame = ((json_enc_frame_T *)ଗଦା.ga_data)
							+ ଗଦା.ga_len - 1;
	    ବାଛ (frame->je_type)
	    {
		ସ୍ଥିତି ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    ଯଦି ((frame->je_options & JSON_JS)
			    && li->li_next == ଖାଲି
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_ସଂଖ୍ୟା == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    ଯଦି (li != ଖାଲି && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			ଭାଙ୍ଗ;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --ଗଦା.ga_len;
		    ଭାଙ୍ଗ;
		}

		ସ୍ଥିତି ENC_TUPLE:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx = frame->je_u.t.idx;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    ଯଦି ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			ଯଦି (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_ସଂଖ୍ୟା == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    ଯଦି (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			ଭାଙ୍ଗ;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --ଗଦା.ga_len;
		    ଭାଙ୍ଗ;
		}

		ସ୍ଥିତି ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo = frame->je_u.d.todo;

		    ଯଦି (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			ପାଇଁ (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			ଯଦି ((frame->je_options & JSON_JS)
				&& is_simple_ଚାବି(hi->hi_ଚାବି))
			    ga_concat(gap, hi->hi_ଚାବି);
			ଅନ୍ୟଥା
			    write_string(gap, hi->hi_ଚାବି);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			ଭାଙ୍ଗ;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --ଗଦା.ga_len;
		    ଭାଙ୍ଗ;
		}
	    }
	    ଯଦି (advance)
		ଭାଙ୍ଗ;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    ପାଇଁ (i = 0; i < ଗଦା.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)ଗଦା.ga_data) + i;
	ବାଛ (frame->je_type)
	{
	    ସ୍ଥିତି ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		ଭାଙ୍ଗ;
	}
    }
    ga_clear(&ଗଦା);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
fill_numbuflen(js_read_T *reader)
{
    ଯଦି (reader->js_fill != ଖାଲି && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	ଯଦି (reader->js_fill(reader))
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
json_skip_white(js_read_T *reader)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

    ପାଇଁ (;;)
    {
	c = reader->js_buf[reader->js_used];
	ଯଦି (reader->js_fill != ଖାଲି && c == NUL)
	{
	    ଯଦି (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		ଜାରି;
	    }
	}
	ଯଦି (c == NUL || c > ' ')
	    ଭାଙ୍ଗ;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
json_decode_string(js_read_T *reader, typval_T *res, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote)
{
    garray_T    ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    varnumber_T	nr;

    ଯଦି (res != ଖାଲି)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    ଯେତେବେଳେ (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	ଯଦି (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    ଯଦି (reader->js_fill == ଖାଲି)
		ଭାଙ୍ଗ;
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(reader->js_end - p);
	    reader->js_used = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - reader->js_buf);
	    ଯଦି (!reader->js_fill(reader))
		ଭାଙ୍ଗ; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    ଜାରି;
	}

	ଯଦି (*p == '\\')
	{
	    c = -1;
	    ବାଛ (p[1])
	    {
		ସ୍ଥିତି '\\': c = '\\'; ଭାଙ୍ଗ;
		ସ୍ଥିତି '"': c = '"'; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'b': c = BS; ଭାଙ୍ଗ;
		ସ୍ଥିତି 't': c = TAB; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'n': c = NL; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'f': c = FF; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'r': c = CAR; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'u':
		    ଯଦି (reader->js_fill != ଖାଲି
				     && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - reader->js_buf);
			ଯଦି (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, ଖାଲି, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, ଖାଲି, 4, TRUE, ଖାଲି);
		    ଯଦି (len == 0)
		    {
			ଯଦି (res != ଖାଲି)
			    ga_clear(&ga);
			ଫେରନ୍ତୁ FAIL;
		    }
		    p += len + 2;
		    ଯଦି (0xd800 <= nr && nr <= 0xdbff
			    && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, ଖାଲି, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, ଖାଲି, 4, TRUE, ଖାଲି);
			ଯଦି (len == 0)
			{
			    ଯଦି (res != ଖାଲି)
				ga_clear(&ga);
			    ଫେରନ୍ତୁ FAIL;
			}
			ଯଦି (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    ଯଦି (0xd800 <= nr && nr <= 0xdfff)
		    {
			ଯଦି (res != ଖାଲି)
			    ga_clear(&ga);
			ଫେରନ୍ତୁ FAIL;
		    }
		    ଯଦି (res != ଖାଲି)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    // not a special char, skip over backslash
		    ++p;
		    ଜାରି;
	    }
	    ଯଦି (c > 0)
	    {
		p += 2;
		ଯଦି (res != ଖାଲି)
		    ga_append(&ga, c);
	    }
	}
	ଅନ୍ୟଥା
	{
	    len = utf_ptr2len(p);
	    ଯଦି (res != ଖାଲି)
	    {
		ଯଦି (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    ଫେରନ୍ତୁ FAIL;
		}
		mch_memmove((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - reader->js_buf);
    ଯଦି (*p == quote)
    {
	++reader->js_used;
	ଯଦି (res != ଖାଲି)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#ଯଦି defined(USE_ICONV)
	    ଯଦି (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		ଯଦି (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, ଖାଲି);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, ଖାଲି, ଖାଲି);
	    }
	    ଅନ୍ୟଥା
#endif
		res->vval.v_string = ga.ga_data;
	}
	ଫେରନ୍ତୁ OK;
    }
    ଯଦି (res != ଖାଲି)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_ସଂଖ୍ୟା = VVAL_NONE;
	ga_clear(&ga);
    }
    ଫେରନ୍ତୁ MAYBE;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_ଚାବି,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_ଚାବି_tv;
    char_u	  *jd_ଚାବି;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
json_decode_item(js_read_T *reader, typval_T *res, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    garray_T	ଗଦା;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *ଶୀର୍ଷ_item;
    char_u	ଚାବି_buf[NUMBUFLEN];

    ga_init2(&ଗଦା, ଆକାର(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    ଯଦି (res != ଖାଲି)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    ପାଇଁ (;;)
    {
	ଶୀର୍ଷ_item = ଖାଲି;
	ଯଦି (ଗଦା.ga_len > 0)
	{
	    ଶୀର୍ଷ_item = ((json_dec_item_T *)ଗଦା.ga_data) + ଗଦା.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    ଯଦି (*p == NUL)
	    {
		retval = MAYBE;
		ଯାଅ theend;
	    }
	    ଯଦି (ଶୀର୍ଷ_item->jd_type == JSON_OBJECT_ଚାବି
					    || ଶୀର୍ଷ_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		ଯଦି (*p == (ଶୀର୍ଷ_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --ଗଦା.ga_len;
		    ଯଦି (ଗଦା.ga_len == 0)
		    {
			retval = OK;
			ଯାଅ theend;
		    }
		    ଯଦି (cur_item != ଖାଲି)
			cur_item = &ଶୀର୍ଷ_item->jd_tv;
		    ଯାଅ item_end;
		}
	    }
	}

	ଯଦି (ଶୀର୍ଷ_item != ଖାଲି && ଶୀର୍ଷ_item->jd_type == JSON_OBJECT_ଚାବି
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *ଚାବି;

	    // accept an object key that is not in quotes
	    ଚାବି = p = reader->js_buf + reader->js_used;
	    ଯେତେବେଳେ (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    ଯଦି (cur_item != ଖାଲି)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(ଚାବି, p - ଚାବି);
		ଶୀର୍ଷ_item->jd_ଚାବି = cur_item->vval.v_string;
	    }
	    reader->js_used += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଚାବି);
	}
	ଅନ୍ୟଥା
	{
	    ବାଛ (*p)
	    {
		ସ୍ଥିତି '[': // start of array
		    ଯଦି (ଶୀର୍ଷ_item && ଶୀର୍ଷ_item->jd_type == JSON_OBJECT_ଚାବି)
		    {
			retval = FAIL;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (ଗଦା.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (ga_grow(&ଗଦା, 1) == FAIL)
		    {
			retval = FAIL;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (cur_item != ଖାଲି && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_ସଂଖ୍ୟା = VVAL_NONE;
			retval = FAIL;
			ଭାଙ୍ଗ;
		    }

		    ++reader->js_used; // consume the '['
		    ଶୀର୍ଷ_item = ((json_dec_item_T *)ଗଦା.ga_data)
								+ ଗଦା.ga_len;
		    ଶୀର୍ଷ_item->jd_type = JSON_ARRAY;
		    ++ଗଦା.ga_len;
		    ଯଦି (cur_item != ଖାଲି)
		    {
			ଶୀର୍ଷ_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    ଜାରି;

		ସ୍ଥିତି '{': // start of object
		    ଯଦି (ଶୀର୍ଷ_item && ଶୀର୍ଷ_item->jd_type == JSON_OBJECT_ଚାବି)
		    {
			retval = FAIL;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (ଗଦା.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (ga_grow(&ଗଦା, 1) == FAIL)
		    {
			retval = FAIL;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (cur_item != ଖାଲି && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_ସଂଖ୍ୟା = VVAL_NONE;
			retval = FAIL;
			ଭାଙ୍ଗ;
		    }

		    ++reader->js_used; // consume the '{'
		    ଶୀର୍ଷ_item = ((json_dec_item_T *)ଗଦା.ga_data)
								+ ଗଦା.ga_len;
		    ଶୀର୍ଷ_item->jd_type = JSON_OBJECT_ଚାବି;
		    ++ଗଦା.ga_len;
		    ଯଦି (cur_item != ଖାଲି)
		    {
			ଶୀର୍ଷ_item->jd_tv = *cur_item;
			cur_item = &ଶୀର୍ଷ_item->jd_ଚାବି_tv;
		    }
		    ଜାରି;

		ସ୍ଥିତି '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି '\'':
		    ଯଦି (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    ଅନ୍ୟଥା
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି ',': // comma: empty item
		    ଯଦି ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			ଭାଙ୍ଗ;
		    }
		    // FALLTHROUGH
		ସ୍ଥିତି NUL: // empty
		    ଯଦି (cur_item != ଖାଲି)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_ସଂଖ୍ୟା = VVAL_NONE;
		    }
		    retval = OK;
		    ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    ଯଦି (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			ଯଦି (*sp == '-')
			{
			    ++sp;
			    ଯଦି (*sp == NUL)
			    {
				retval = MAYBE;
				ଭାଙ୍ଗ;
			    }
			    ଯଦି (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				ଭାଙ୍ଗ;
			    }
			}
			sp = skipdigits(sp);
			ଯଦି (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    ଯଦି (cur_item == ଖାଲି)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    ଅନ୍ୟଥା
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			ଅନ୍ୟଥା
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    ଖାଲି, &len, 0, // what
				    &nr, ଖାଲି, 0, TRUE, ଖାଲି);
			    ଯଦି (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				ଯାଅ theend;
			    }
			    ଯଦି (cur_item != ଖାଲି)
			    {
				cur_item->v_type = VAR_ସଂଖ୍ୟା;
				cur_item->vval.v_ସଂଖ୍ୟା = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			ଭାଙ୍ଗ;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((ଅକ୍ଷର *)(p), (kw), (len)) == 0 \
     : STRNCMP((ଅକ୍ଷର *)(p), (kw), (len)) == 0)
		    ଯଦି (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			ଯଦି (cur_item != ଖାଲି)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_ସଂଖ୍ୟା = VVAL_FALSE;
			}
			retval = OK;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			ଯଦି (cur_item != ଖାଲି)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;
			}
			retval = OK;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			ଯଦି (cur_item != ଖାଲି)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_ସଂଖ୍ୟା = VVAL_NULL;
			}
			retval = OK;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			ଯଦି (cur_item != ଖାଲି)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			ଯଦି (cur_item != ଖାଲି)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			ଯଦି (cur_item != ଖାଲି)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			ଭାଙ୍ଗ;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    ଯଦି (
			    (len < 5 && STRNICMP((ଅକ୍ଷର *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((ଅକ୍ଷର *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((ଅକ୍ଷର *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((ଅକ୍ଷର *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((ଅକ୍ଷର *)p, "true", len) == 0
				    || STRNICMP((ଅକ୍ଷର *)p, "null", len) == 0)))

			retval = MAYBE;
		    ଅନ୍ୟଥା
			retval = FAIL;
		    ଭାଙ୍ଗ;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    ଯଦି (retval == FAIL)
		ଭାଙ୍ଗ;
	    ଯଦି (retval == MAYBE || ଗଦା.ga_len == 0)
		ଯାଅ theend;

	    ଯଦି (ଶୀର୍ଷ_item != ଖାଲି && ଶୀର୍ଷ_item->jd_type == JSON_OBJECT_ଚାବି
		    && cur_item != ଖାଲି)
	    {
		ଯଦି (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    ଯାଅ theend;
		}
		ଶୀର୍ଷ_item->jd_ଚାବି = tv_get_string_buf_chk(cur_item, ଚାବି_buf);
		ଯଦି (ଶୀର୍ଷ_item->jd_ଚାବି == ଖାଲି)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    ଯାଅ theend;
		}
	    }
	}

item_end:
	ଶୀର୍ଷ_item = ((json_dec_item_T *)ଗଦା.ga_data) + ଗଦା.ga_len - 1;
	ବାଛ (ଶୀର୍ଷ_item->jd_type)
	{
	    ସ୍ଥିତି JSON_ARRAY:
		ଯଦି (res != ଖାଲି)
		{
		    listitem_T	*li = listitem_alloc();

		    ଯଦି (li == ଖାଲି)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			ଯାଅ theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(ଶୀର୍ଷ_item->jd_tv.vval.v_list, li);
		}
		ଯଦି (cur_item != ଖାଲି)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ଯଦି (*p == ',')
		    ++reader->js_used;
		ଅନ୍ୟଥା ଯଦି (*p != ']')
		{
		    ଯଦି (*p == NUL)
			retval = MAYBE;
		    ଅନ୍ୟଥା
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ଯାଅ theend;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି JSON_OBJECT_ଚାବି:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ଯଦି (*p != ':')
		{
		    ଯଦି (cur_item != ଖାଲି)
			clear_tv(cur_item);
		    ଯଦି (*p == NUL)
			retval = MAYBE;
		    ଅନ୍ୟଥା
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ଯାଅ theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		ଶୀର୍ଷ_item->jd_type = JSON_OBJECT;
		ଯଦି (cur_item != ଖାଲି)
		    cur_item = &item;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି JSON_OBJECT:
		ଯଦି (cur_item != ଖାଲି
			&& dict_has_ଚାବି(ଶୀର୍ଷ_item->jd_tv.vval.v_dict,
						(ଅକ୍ଷର *)ଶୀର୍ଷ_item->jd_ଚାବି))
		{
		    semsg(_(e_duplicate_ଚାବି_in_json_str), ଶୀର୍ଷ_item->jd_ଚାବି);
		    clear_tv(cur_item);
		    retval = FAIL;
		    ଯାଅ theend;
		}

		ଯଦି (cur_item != ଖାଲି)
		{
		    dictitem_T *di = dictitem_alloc(ଶୀର୍ଷ_item->jd_ଚାବି);

		    clear_tv(&ଶୀର୍ଷ_item->jd_ଚାବି_tv);
		    ଯଦି (di == ଖାଲି)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			ଯାଅ theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    ଯଦି (dict_add(ଶୀର୍ଷ_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			ଯାଅ theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ଯଦି (*p == ',')
		    ++reader->js_used;
		ଅନ୍ୟଥା ଯଦି (*p != '}')
		{
		    ଯଦି (*p == NUL)
			retval = MAYBE;
		    ଅନ୍ୟଥା
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ଯାଅ theend;
		}
		ଶୀର୍ଷ_item->jd_type = JSON_OBJECT_ଚାବି;
		ଯଦି (cur_item != ଖାଲି)
		    cur_item = &ଶୀର୍ଷ_item->jd_ଚାବି_tv;
		ଭାଙ୍ଗ;
	}
    }

    // Get here when parsing failed.
    ଯଦି (res != ଖାଲି)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_ସଂଖ୍ୟା = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    ପାଇଁ (i = 0; i < ଗଦା.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)ଗଦା.ga_data) + i)->jd_ଚାବି_tv);
    ga_clear(&ଗଦା);

    ଫେରନ୍ତୁ retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
json_decode_all(js_read_T *reader, typval_T *res, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    ଯଦି (ret != OK)
    {
	ଯଦି (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	ଫେରନ୍ତୁ FAIL;
    }
    json_skip_white(reader);
    ଯଦି (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

#ଯଦି defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
json_decode(js_read_T *reader, typval_T *res, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    ଫେରନ୍ତୁ ret;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
json_find_end(js_read_T *reader, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା used_save = reader->js_used;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, ଖାଲି, options);
    reader->js_used = used_save;
    ଫେରନ୍ତୁ ret;
}

/*
 * "js_decode()" function
 */
    ଶୂନ୍ୟ
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ଖାଲି;
    reader.js_used = 0;
    ଯଦି (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_argument));
}

/*
 * "js_encode()" function
 */
    ଶୂନ୍ୟ
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    ଶୂନ୍ୟ
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ଖାଲି;
    reader.js_used = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    ଶୂନ୍ୟ
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#endif
