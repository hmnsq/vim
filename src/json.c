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
#నిర్వచించు USING_FLOAT_STUFF

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

స్థిర పూర్ణాంకం json_encode_అంశం(garray_T *gap, typval_T *val, పూర్ణాంకం copyID, పూర్ణాంకం options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    స్థిర పూర్ణాంకం
json_encode_gap(garray_T *gap, typval_T *val, పూర్ణాంకం options)
{
    ఒకవేళ (json_encode_అంశం(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_దత్తాంశం = vim_strsave((char_u *)"");
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, పూర్ణాంకం options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    ఒకవేళ (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    తిరిగి ga.ga_దత్తాంశం;
}

#ఒకవేళ defined(FEAT_పని_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_సమాసం(పూర్ణాంకం nr, typval_T *val, పూర్ణాంకం options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_సంఖ్య;
    nrtv.vval.v_సంఖ్య = nr;
    ఒకవేళ (rettv_list_alloc(&listtv) == FAIL)
	తిరిగి ఖాళీ;
    ఒకవేళ (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	తిరిగి ఖాళీ;
    }

    ga_init2(&ga, 1, 4000);
    ఒకవేళ (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    తిరిగి ga.ga_దత్తాంశం;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_సందేశం(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    ఒకవేళ (json_encode_gap(&ga, val, 0) == FAIL)
	తిరిగి ఖాళీ;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((అక్షరం *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_దత్తాంశం, ga.ga_len);
    ga_clear(&ga);
    తిరిగి lspga.ga_దత్తాంశం;
}
#అయితే_ముగింపు

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
స్థిర స్థిరరాశి అక్షరం ascii_needs_escape[128] = {
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
    స్థిర శూన్య
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#ఒకవేళ defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = ఖాళీ;
#అయితే_ముగింపు
    పూర్ణాంకం		c;

    ఒకవేళ (res == ఖాళీ)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	తిరిగి;
    }

#ఒకవేళ defined(USE_ICONV)
    ఒకవేళ (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_అమరిక(&conv, p_enc, (char_u*)"utf-8");
	ఒకవేళ (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, ఖాళీ);
	convert_అమరిక(&conv, ఖాళీ, ఖాళీ);
    }
#అయితే_ముగింపు
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (పూర్ణాంకం)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    అయితే ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	ఒకవేళ (c < 0x80)
	{
	    ఒకవేళ (!ascii_needs_escape[c])
	    {
		res += 1;
		కొనసాగు;
	    }

	    ఒకవేళ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    ఎంపిక (c)
	    {
		స్థితి 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); ఆపు;
		స్థితి 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); ఆపు;
		స్థితి 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); ఆపు;
		స్థితి 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); ఆపు;
		స్థితి 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); ఆపు;
		స్థితి 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); ఆపు;
		స్థితి 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); ఆపు;
		అప్రమేయ:
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
	లేకపోతే
	{
	    పూర్ణాంకం	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    ఒకవేళ (l > 1)
	    {
		res += l;
		కొనసాగు;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    ఒకవేళ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    ఒకవేళ (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#ఒకవేళ defined(USE_ICONV)
    vim_free(converted);
#అయితే_ముగింపు
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    స్థిర పూర్ణాంకం
is_simple_కీలకం(char_u *కీలకం)
{
    char_u *p;

    ఒకవేళ (!ASCII_ISALPHA(*కీలకం))
	తిరిగి FALSE;
    కొరకు (p = కీలకం + 1; *p != NUL; ++p)
	ఒకవేళ (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    తిరిగి FALSE;
    తిరిగి TRUE;
}

రకనిర్వచనం గణన {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

రకనిర్వచనం నిర్మాణం {
    json_enc_type_T je_type;
    పూర్ణాంకం		    je_options;	    // options when this container was entered
    సంఘం {
	నిర్మాణం {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	నిర్మాణం {
	    tuple_T	*tuple;
	    పూర్ణాంకం		idx;	    // current index
	    పూర్ణాంకం		len;
	} t;
	నిర్మాణం {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    పూర్ణాంకం		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_చట్రం_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    స్థిర పూర్ణాంకం
json_encode_అంశం(garray_T *gap, typval_T *val, పూర్ణాంకం copyID, పూర్ణాంకం options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    పూర్ణాంకం			i;
    garray_T		దొంతర;
    typval_T		*cur_val;
    json_enc_చట్రం_T	*చట్రం;

    ga_init2(&దొంతర, పరిమాణం(json_enc_చట్రం_T), 100);
    cur_val = val;

    కొరకు (;;)
    {
    ఎంపిక (cur_val->v_type)
    {
	స్థితి VAR_BOOL:
	    ఎంపిక ((దీర్ఘ)cur_val->vval.v_సంఖ్య)
	    {
		స్థితి VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); ఆపు;
		స్థితి VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); ఆపు;
	    }
	    ఆపు;

	స్థితి VAR_SPECIAL:
	    ఎంపిక ((దీర్ఘ)cur_val->vval.v_సంఖ్య)
	    {
		స్థితి VVAL_NONE: ఒకవేళ ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    ఆపు;
				// FALLTHROUGH
		స్థితి VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); ఆపు;
	    }
	    ఆపు;

	స్థితి VAR_సంఖ్య:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((అక్షరం *)numbuf, పరిమాణం(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_సంఖ్య);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ఆపు;

	స్థితి VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    ఆపు;

	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_INSTR:
	స్థితి VAR_CLASS:
	స్థితి VAR_OBJECT:
	స్థితి VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    వెళ్ళు theend;

	స్థితి VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    ఒకవేళ (b == ఖాళీ || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    లేకపోతే
	    {
		పూర్ణాంకం	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		ఒకవేళ (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    వెళ్ళు theend;
		src = (char_u *)b->bv_ga.ga_దత్తాంశం;
		dst = (char_u *)gap->ga_దత్తాంశం + gap->ga_len;
		*dst++ = '[';
		కొరకు (i = 0; i < blen; i++)
		{
		    పూర్ణాంకం	    byte = src[i];

		    ఒకవేళ (i > 0)
			*dst++ = ',';
		    ఒకవేళ (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    లేకపోతే ఒకవేళ (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    లేకపోతే
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (పూర్ణాంకం)(dst - (char_u *)gap->ga_దత్తాంశం);
	    }
	    ఆపు;

	స్థితి VAR_LIST:
	    l = cur_val->vval.v_list;
	    ఒకవేళ (l == ఖాళీ)
		GA_CONCAT_LITERAL(gap, "[]");
	    లేకపోతే
	    {
		ఒకవేళ (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		లేకపోతే
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    ఒకవేళ (l->lv_మొదటి != ఖాళీ)
		    {
			ఒకవేళ (దొంతర.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_లోతు_is_higher_than_maxfuncdepth));
			    వెళ్ళు theend;
			}
			ఒకవేళ (ga_grow(&దొంతర, 1) == FAIL)
			    వెళ్ళు theend;
			చట్రం = ((json_enc_చట్రం_T *)దొంతర.ga_దత్తాంశం)
							    + దొంతర.ga_len;
			చట్రం->je_type = ENC_LIST;
			చట్రం->je_options = options;
			చట్రం->je_u.l.list = l;
			చట్రం->je_u.l.li = l->lv_మొదటి;
			++దొంతర.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_మొదటి->li_tv;
			కొనసాగు;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    ఆపు;

	స్థితి VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    ఒకవేళ (tuple == ఖాళీ)
		GA_CONCAT_LITERAL(gap, "[]");
	    లేకపోతే
	    {
		ఒకవేళ (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		లేకపోతే
		{
		    పూర్ణాంకం		len = TUPLE_LEN(tuple);

		    ఒకవేళ (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			ఒకవేళ (దొంతర.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_లోతు_is_higher_than_maxfuncdepth));
			    వెళ్ళు theend;
			}
			ఒకవేళ (ga_grow(&దొంతర, 1) == FAIL)
			    వెళ్ళు theend;
			చట్రం = ((json_enc_చట్రం_T *)దొంతర.ga_దత్తాంశం)
							    + దొంతర.ga_len;
			చట్రం->je_type = ENC_TUPLE;
			చట్రం->je_options = options;
			చట్రం->je_u.t.tuple = tuple;
			చట్రం->je_u.t.idx = 0;
			చట్రం->je_u.t.len = len;
			++దొంతర.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_అంశం(tuple, 0);
			కొనసాగు;
		    }
		    లేకపోతే
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    ఆపు;

	స్థితి VAR_DICT:
	    d = cur_val->vval.v_dict;
	    ఒకవేళ (d == ఖాళీ)
		GA_CONCAT_LITERAL(gap, "{}");
	    లేకపోతే
	    {
		ఒకవేళ (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		లేకపోతే
		{
		    పూర్ణాంకం		todo = (పూర్ణాంకం)d->dv_hashtab.ht_వాడిన;
		    hashitem_T	*hi;

		    ఒకవేళ (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			కొరకు (hi = d->dv_hashtab.ht_array;
						  HASHITEM_ఖాళీలు(hi); ++hi)
			    ;
			--todo;

			// Write first key
			ఒకవేళ ((options & JSON_JS)
						 && is_simple_కీలకం(hi->hi_కీలకం))
			    ga_concat(gap, hi->hi_కీలకం);
			లేకపోతే
			    write_string(gap, hi->hi_కీలకం);
			ga_append(gap, ':');

			ఒకవేళ (దొంతర.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_లోతు_is_higher_than_maxfuncdepth));
			    వెళ్ళు theend;
			}
			ఒకవేళ (ga_grow(&దొంతర, 1) == FAIL)
			    వెళ్ళు theend;
			చట్రం = ((json_enc_చట్రం_T *)దొంతర.ga_దత్తాంశం)
							    + దొంతర.ga_len;
			చట్రం->je_type = ENC_DICT;
			చట్రం->je_options = options;
			చట్రం->je_u.d.dict = d;
			చట్రం->je_u.d.hi = hi;
			చట్రం->je_u.d.todo = todo;
			++దొంతర.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			కొనసాగు;
		    }
		    లేకపోతే
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    ఆపు;

	స్థితి VAR_FLOAT:
#ఒకవేళ defined(HAVE_MATH_H)
	    ఒకవేళ (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    లేకపోతే ఒకవేళ (isinf(cur_val->vval.v_float))
	    {
		ఒకవేళ (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		లేకపోతే
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((అక్షరం *)numbuf, పరిమాణం(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    ఆపు;

	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	    అంతర్గత_error_no_abort("json_encode_item()");
	    వెళ్ళు theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	కొరకు (;;)
	{
	    పూర్ణాంకం		advance = FALSE;

	    ఒకవేళ (దొంతర.ga_len == 0)
	    {
		ga_clear(&దొంతర);
		తిరిగి OK;
	    }

	    చట్రం = ((json_enc_చట్రం_T *)దొంతర.ga_దత్తాంశం)
							+ దొంతర.ga_len - 1;
	    ఎంపిక (చట్రం->je_type)
	    {
		స్థితి ENC_LIST:
		{
		    listitem_T	*li = చట్రం->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    ఒకవేళ ((చట్రం->je_options & JSON_JS)
			    && li->li_తదుపరి == ఖాళీ
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_సంఖ్య == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_తదుపరి;
		    చట్రం->je_u.l.li = li;
		    ఒకవేళ (li != ఖాళీ && !got_int)
		    {
			ga_append(gap, ',');
			options = చట్రం->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			ఆపు;
		    }
		    ga_append(gap, ']');
		    చట్రం->je_u.l.list->lv_copyID = 0;
		    --దొంతర.ga_len;
		    ఆపు;
		}

		స్థితి ENC_TUPLE:
		{
		    పూర్ణాంకం	    idx = చట్రం->je_u.t.idx;
		    పూర్ణాంకం	    len = చట్రం->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    ఒకవేళ ((చట్రం->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_అంశం =
				    TUPLE_అంశం(చట్రం->je_u.t.tuple, idx);

			ఒకవేళ (t_అంశం->v_type == VAR_SPECIAL
				&& t_అంశం->vval.v_సంఖ్య == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    చట్రం->je_u.t.idx = idx;
		    ఒకవేళ (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = చట్రం->je_options & JSON_JS;
			cur_val = TUPLE_అంశం(చట్రం->je_u.t.tuple, idx);
			advance = TRUE;
			ఆపు;
		    }
		    ga_append(gap, ']');
		    చట్రం->je_u.t.tuple->tv_copyID = 0;
		    --దొంతర.ga_len;
		    ఆపు;
		}

		స్థితి ENC_DICT:
		{
		    hashitem_T	*hi = చట్రం->je_u.d.hi;
		    పూర్ణాంకం		todo = చట్రం->je_u.d.todo;

		    ఒకవేళ (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			కొరకు (++hi; HASHITEM_ఖాళీలు(hi); ++hi)
			    ;
			--todo;
			చట్రం->je_u.d.hi = hi;
			చట్రం->je_u.d.todo = todo;

			ga_append(gap, ',');
			ఒకవేళ ((చట్రం->je_options & JSON_JS)
				&& is_simple_కీలకం(hi->hi_కీలకం))
			    ga_concat(gap, hi->hi_కీలకం);
			లేకపోతే
			    write_string(gap, hi->hi_కీలకం);
			ga_append(gap, ':');

			options = చట్రం->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			ఆపు;
		    }
		    ga_append(gap, '}');
		    చట్రం->je_u.d.dict->dv_copyID = 0;
		    --దొంతర.ga_len;
		    ఆపు;
		}
	    }
	    ఒకవేళ (advance)
		ఆపు;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    కొరకు (i = 0; i < దొంతర.ga_len; i++)
    {
	చట్రం = ((json_enc_చట్రం_T *)దొంతర.ga_దత్తాంశం) + i;
	ఎంపిక (చట్రం->je_type)
	{
	    స్థితి ENC_LIST:
		చట్రం->je_u.l.list->lv_copyID = 0;
		ఆపు;
	    స్థితి ENC_TUPLE:
		చట్రం->je_u.t.tuple->tv_copyID = 0;
		ఆపు;
	    స్థితి ENC_DICT:
		చట్రం->je_u.d.dict->dv_copyID = 0;
		ఆపు;
	}
    }
    ga_clear(&దొంతర);
    తిరిగి FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    స్థిర శూన్య
fill_numbuflen(js_read_T *reader)
{
    ఒకవేళ (reader->js_fill != ఖాళీ && (పూర్ణాంకం)(reader->js_చివర - reader->js_buf)
						- reader->js_వాడిన < NUMBUFLEN)
    {
	ఒకవేళ (reader->js_fill(reader))
	    reader->js_చివర = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    స్థిర శూన్య
json_skip_white(js_read_T *reader)
{
    పూర్ణాంకం c;

    కొరకు (;;)
    {
	c = reader->js_buf[reader->js_వాడిన];
	ఒకవేళ (reader->js_fill != ఖాళీ && c == NUL)
	{
	    ఒకవేళ (reader->js_fill(reader))
	    {
		reader->js_చివర = reader->js_buf + STRLEN(reader->js_buf);
		కొనసాగు;
	    }
	}
	ఒకవేళ (c == NUL || c > ' ')
	    ఆపు;
	++reader->js_వాడిన;
    }
    fill_numbuflen(reader);
}

    స్థిర పూర్ణాంకం
json_decode_string(js_read_T *reader, typval_T *res, పూర్ణాంకం quote)
{
    garray_T    ga;
    పూర్ణాంకం		len;
    char_u	*p;
    పూర్ణాంకం		c;
    varnumber_T	nr;

    ఒకవేళ (res != ఖాళీ)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_వాడిన + 1; // skip over " or '
    అయితే (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	ఒకవేళ (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    ఒకవేళ (reader->js_fill == ఖాళీ)
		ఆపు;
	    len = (పూర్ణాంకం)(reader->js_చివర - p);
	    reader->js_వాడిన = (పూర్ణాంకం)(p - reader->js_buf);
	    ఒకవేళ (!reader->js_fill(reader))
		ఆపు; // didn't get more
	    p = reader->js_buf + reader->js_వాడిన;
	    reader->js_చివర = reader->js_buf + STRLEN(reader->js_buf);
	    కొనసాగు;
	}

	ఒకవేళ (*p == '\\')
	{
	    c = -1;
	    ఎంపిక (p[1])
	    {
		స్థితి '\\': c = '\\'; ఆపు;
		స్థితి '"': c = '"'; ఆపు;
		స్థితి 'b': c = BS; ఆపు;
		స్థితి 't': c = TAB; ఆపు;
		స్థితి 'n': c = NL; ఆపు;
		స్థితి 'f': c = FF; ఆపు;
		స్థితి 'r': c = CAR; ఆపు;
		స్థితి 'u':
		    ఒకవేళ (reader->js_fill != ఖాళీ
				     && (పూర్ణాంకం)(reader->js_చివర - p) < NUMBUFLEN)
		    {
			reader->js_వాడిన = (పూర్ణాంకం)(p - reader->js_buf);
			ఒకవేళ (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_వాడిన;
			    reader->js_చివర = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, ఖాళీ, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, ఖాళీ, 4, TRUE, ఖాళీ);
		    ఒకవేళ (len == 0)
		    {
			ఒకవేళ (res != ఖాళీ)
			    ga_clear(&ga);
			తిరిగి FAIL;
		    }
		    p += len + 2;
		    ఒకవేళ (0xd800 <= nr && nr <= 0xdbff
			    && (పూర్ణాంకం)(reader->js_చివర - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, ఖాళీ, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, ఖాళీ, 4, TRUE, ఖాళీ);
			ఒకవేళ (len == 0)
			{
			    ఒకవేళ (res != ఖాళీ)
				ga_clear(&ga);
			    తిరిగి FAIL;
			}
			ఒకవేళ (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    ఒకవేళ (0xd800 <= nr && nr <= 0xdfff)
		    {
			ఒకవేళ (res != ఖాళీ)
			    ga_clear(&ga);
			తిరిగి FAIL;
		    }
		    ఒకవేళ (res != ఖాళీ)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((పూర్ణాంకం)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    ఆపు;
		అప్రమేయ:
		    // not a special char, skip over backslash
		    ++p;
		    కొనసాగు;
	    }
	    ఒకవేళ (c > 0)
	    {
		p += 2;
		ఒకవేళ (res != ఖాళీ)
		    ga_append(&ga, c);
	    }
	}
	లేకపోతే
	{
	    len = utf_ptr2len(p);
	    ఒకవేళ (res != ఖాళీ)
	    {
		ఒకవేళ (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    తిరిగి FAIL;
		}
		mch_memmove((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_వాడిన = (పూర్ణాంకం)(p - reader->js_buf);
    ఒకవేళ (*p == quote)
    {
	++reader->js_వాడిన;
	ఒకవేళ (res != ఖాళీ)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#ఒకవేళ defined(USE_ICONV)
	    ఒకవేళ (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_అమరిక(&conv, (char_u*)"utf-8", p_enc);
		ఒకవేళ (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_దత్తాంశం, ఖాళీ);
		    vim_free(ga.ga_దత్తాంశం);
		}
		convert_అమరిక(&conv, ఖాళీ, ఖాళీ);
	    }
	    లేకపోతే
#అయితే_ముగింపు
		res->vval.v_string = ga.ga_దత్తాంశం;
	}
	తిరిగి OK;
    }
    ఒకవేళ (res != ఖాళీ)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_సంఖ్య = VVAL_NONE;
	ga_clear(&ga);
    }
    తిరిగి MAYBE;
}

రకనిర్వచనం గణన {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_కీలకం,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

రకనిర్వచనం నిర్మాణం {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_కీలకం_tv;
    char_u	  *jd_కీలకం;
} json_dec_అంశం_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    స్థిర పూర్ణాంకం
json_decode_అంశం(js_read_T *reader, typval_T *res, పూర్ణాంకం options)
{
    char_u	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		len;
    పూర్ణాంకం		retval;
    garray_T	దొంతర;
    typval_T	అంశం;
    typval_T	*cur_అంశం;
    json_dec_అంశం_T *శిఖరం_అంశం;
    char_u	కీలకం_buf[NUMBUFLEN];

    ga_init2(&దొంతర, పరిమాణం(json_dec_అంశం_T), 100);
    cur_అంశం = res;
    init_tv(&అంశం);
    ఒకవేళ (res != ఖాళీ)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_వాడిన;
    కొరకు (;;)
    {
	శిఖరం_అంశం = ఖాళీ;
	ఒకవేళ (దొంతర.ga_len > 0)
	{
	    శిఖరం_అంశం = ((json_dec_అంశం_T *)దొంతర.ga_దత్తాంశం) + దొంతర.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_వాడిన;
	    ఒకవేళ (*p == NUL)
	    {
		retval = MAYBE;
		వెళ్ళు theend;
	    }
	    ఒకవేళ (శిఖరం_అంశం->jd_type == JSON_OBJECT_కీలకం
					    || శిఖరం_అంశం->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		ఒకవేళ (*p == (శిఖరం_అంశం->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_వాడిన; // consume the ']' or '}'
		    --దొంతర.ga_len;
		    ఒకవేళ (దొంతర.ga_len == 0)
		    {
			retval = OK;
			వెళ్ళు theend;
		    }
		    ఒకవేళ (cur_అంశం != ఖాళీ)
			cur_అంశం = &శిఖరం_అంశం->jd_tv;
		    వెళ్ళు అంశం_చివర;
		}
	    }
	}

	ఒకవేళ (శిఖరం_అంశం != ఖాళీ && శిఖరం_అంశం->jd_type == JSON_OBJECT_కీలకం
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_వాడిన] != '"'
		&& reader->js_buf[reader->js_వాడిన] != '\''
		&& reader->js_buf[reader->js_వాడిన] != '['
		&& reader->js_buf[reader->js_వాడిన] != '{')
	{
	    char_u *కీలకం;

	    // accept an object key that is not in quotes
	    కీలకం = p = reader->js_buf + reader->js_వాడిన;
	    అయితే (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    ఒకవేళ (cur_అంశం != ఖాళీ)
	    {
		cur_అంశం->v_type = VAR_STRING;
		cur_అంశం->vval.v_string = vim_strnsave(కీలకం, p - కీలకం);
		శిఖరం_అంశం->jd_కీలకం = cur_అంశం->vval.v_string;
	    }
	    reader->js_వాడిన += (పూర్ణాంకం)(p - కీలకం);
	}
	లేకపోతే
	{
	    ఎంపిక (*p)
	    {
		స్థితి '[': // start of array
		    ఒకవేళ (శిఖరం_అంశం && శిఖరం_అంశం->jd_type == JSON_OBJECT_కీలకం)
		    {
			retval = FAIL;
			ఆపు;
		    }
		    ఒకవేళ (దొంతర.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_లోతు_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ఆపు;
		    }
		    ఒకవేళ (ga_grow(&దొంతర, 1) == FAIL)
		    {
			retval = FAIL;
			ఆపు;
		    }
		    ఒకవేళ (cur_అంశం != ఖాళీ && rettv_list_alloc(cur_అంశం) == FAIL)
		    {
			cur_అంశం->v_type = VAR_SPECIAL;
			cur_అంశం->vval.v_సంఖ్య = VVAL_NONE;
			retval = FAIL;
			ఆపు;
		    }

		    ++reader->js_వాడిన; // consume the '['
		    శిఖరం_అంశం = ((json_dec_అంశం_T *)దొంతర.ga_దత్తాంశం)
								+ దొంతర.ga_len;
		    శిఖరం_అంశం->jd_type = JSON_ARRAY;
		    ++దొంతర.ga_len;
		    ఒకవేళ (cur_అంశం != ఖాళీ)
		    {
			శిఖరం_అంశం->jd_tv = *cur_అంశం;
			cur_అంశం = &అంశం;
		    }
		    కొనసాగు;

		స్థితి '{': // start of object
		    ఒకవేళ (శిఖరం_అంశం && శిఖరం_అంశం->jd_type == JSON_OBJECT_కీలకం)
		    {
			retval = FAIL;
			ఆపు;
		    }
		    ఒకవేళ (దొంతర.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_లోతు_is_higher_than_maxfuncdepth));
			retval = FAIL;
			ఆపు;
		    }
		    ఒకవేళ (ga_grow(&దొంతర, 1) == FAIL)
		    {
			retval = FAIL;
			ఆపు;
		    }
		    ఒకవేళ (cur_అంశం != ఖాళీ && rettv_dict_alloc(cur_అంశం) == FAIL)
		    {
			cur_అంశం->v_type = VAR_SPECIAL;
			cur_అంశం->vval.v_సంఖ్య = VVAL_NONE;
			retval = FAIL;
			ఆపు;
		    }

		    ++reader->js_వాడిన; // consume the '{'
		    శిఖరం_అంశం = ((json_dec_అంశం_T *)దొంతర.ga_దత్తాంశం)
								+ దొంతర.ga_len;
		    శిఖరం_అంశం->jd_type = JSON_OBJECT_కీలకం;
		    ++దొంతర.ga_len;
		    ఒకవేళ (cur_అంశం != ఖాళీ)
		    {
			శిఖరం_అంశం->jd_tv = *cur_అంశం;
			cur_అంశం = &శిఖరం_అంశం->jd_కీలకం_tv;
		    }
		    కొనసాగు;

		స్థితి '"': // string
		    retval = json_decode_string(reader, cur_అంశం, *p);
		    ఆపు;

		స్థితి '\'':
		    ఒకవేళ (options & JSON_JS)
			retval = json_decode_string(reader, cur_అంశం, *p);
		    లేకపోతే
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    ఆపు;

		స్థితి ',': // comma: empty item
		    ఒకవేళ ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			ఆపు;
		    }
		    // FALLTHROUGH
		స్థితి NUL: //ఖాళీలు
		    ఒకవేళ (cur_అంశం != ఖాళీ)
		    {
			cur_అంశం->v_type = VAR_SPECIAL;
			cur_అంశం->vval.v_సంఖ్య = VVAL_NONE;
		    }
		    retval = OK;
		    ఆపు;

		అప్రమేయ:
		    ఒకవేళ (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			ఒకవేళ (*sp == '-')
			{
			    ++sp;
			    ఒకవేళ (*sp == NUL)
			    {
				retval = MAYBE;
				ఆపు;
			    }
			    ఒకవేళ (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				ఆపు;
			    }
			}
			sp = skipdigits(sp);
			ఒకవేళ (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    ఒకవేళ (cur_అంశం == ఖాళీ)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    లేకపోతే
			    {
				cur_అంశం->v_type = VAR_FLOAT;
				len = string2float(p, &cur_అంశం->vval.v_float,
									FALSE);
			    }
			}
			లేకపోతే
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_వాడిన,
				    ఖాళీ, &len, 0, // what
				    &nr, ఖాళీ, 0, TRUE, ఖాళీ);
			    ఒకవేళ (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				వెళ్ళు theend;
			    }
			    ఒకవేళ (cur_అంశం != ఖాళీ)
			    {
				cur_అంశం->v_type = VAR_సంఖ్య;
				cur_అంశం->vval.v_సంఖ్య = nr;
			    }
			}
			reader->js_వాడిన += len;
			retval = OK;
			ఆపు;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#నిర్వచించు సరిపోలిక_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((అక్షరం *)(p), (kw), (len)) == 0 \
     : STRNCMP((అక్షరం *)(p), (kw), (len)) == 0)
		    ఒకవేళ (సరిపోలిక_KW(p, "false", 5))
		    {
			reader->js_వాడిన += 5;
			ఒకవేళ (cur_అంశం != ఖాళీ)
			{
			    cur_అంశం->v_type = VAR_BOOL;
			    cur_అంశం->vval.v_సంఖ్య = VVAL_FALSE;
			}
			retval = OK;
			ఆపు;
		    }
		    ఒకవేళ (సరిపోలిక_KW(p, "true", 4))
		    {
			reader->js_వాడిన += 4;
			ఒకవేళ (cur_అంశం != ఖాళీ)
			{
			    cur_అంశం->v_type = VAR_BOOL;
			    cur_అంశం->vval.v_సంఖ్య = VVAL_TRUE;
			}
			retval = OK;
			ఆపు;
		    }
		    ఒకవేళ (సరిపోలిక_KW(p, "null", 4))
		    {
			reader->js_వాడిన += 4;
			ఒకవేళ (cur_అంశం != ఖాళీ)
			{
			    cur_అంశం->v_type = VAR_SPECIAL;
			    cur_అంశం->vval.v_సంఖ్య = VVAL_NULL;
			}
			retval = OK;
			ఆపు;
		    }
		    ఒకవేళ (సరిపోలిక_KW(p, "NaN", 3))
		    {
			reader->js_వాడిన += 3;
			ఒకవేళ (cur_అంశం != ఖాళీ)
			{
			    cur_అంశం->v_type = VAR_FLOAT;
			    cur_అంశం->vval.v_float = NAN;
			}
			retval = OK;
			ఆపు;
		    }
		    ఒకవేళ (సరిపోలిక_KW(p, "-Infinity", 9))
		    {
			reader->js_వాడిన += 9;
			ఒకవేళ (cur_అంశం != ఖాళీ)
			{
			    cur_అంశం->v_type = VAR_FLOAT;
			    cur_అంశం->vval.v_float = -INFINITY;
			}
			retval = OK;
			ఆపు;
		    }
		    ఒకవేళ (సరిపోలిక_KW(p, "Infinity", 8))
		    {
			reader->js_వాడిన += 8;
			ఒకవేళ (cur_అంశం != ఖాళీ)
			{
			    cur_అంశం->v_type = VAR_FLOAT;
			    cur_అంశం->vval.v_float = INFINITY;
			}
			retval = OK;
			ఆపు;
		    }
#నిర్వచనం_రద్దు సరిపోలిక_KW
		    // check for truncated name
		    len = (పూర్ణాంకం)(reader->js_చివర
					 - (reader->js_buf + reader->js_వాడిన));
		    ఒకవేళ (
			    (len < 5 && STRNICMP((అక్షరం *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((అక్షరం *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((అక్షరం *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((అక్షరం *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((అక్షరం *)p, "true", len) == 0
				    || STRNICMP((అక్షరం *)p, "null", len) == 0)))

			retval = MAYBE;
		    లేకపోతే
			retval = FAIL;
		    ఆపు;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    ఒకవేళ (retval == FAIL)
		ఆపు;
	    ఒకవేళ (retval == MAYBE || దొంతర.ga_len == 0)
		వెళ్ళు theend;

	    ఒకవేళ (శిఖరం_అంశం != ఖాళీ && శిఖరం_అంశం->jd_type == JSON_OBJECT_కీలకం
		    && cur_అంశం != ఖాళీ)
	    {
		ఒకవేళ (cur_అంశం->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    వెళ్ళు theend;
		}
		శిఖరం_అంశం->jd_కీలకం = tv_get_string_buf_chk(cur_అంశం, కీలకం_buf);
		ఒకవేళ (శిఖరం_అంశం->jd_కీలకం == ఖాళీ)
		{
		    emsg(_(e_invalid_వాదం));
		    retval = FAIL;
		    వెళ్ళు theend;
		}
	    }
	}

అంశం_చివర:
	శిఖరం_అంశం = ((json_dec_అంశం_T *)దొంతర.ga_దత్తాంశం) + దొంతర.ga_len - 1;
	ఎంపిక (శిఖరం_అంశం->jd_type)
	{
	    స్థితి JSON_ARRAY:
		ఒకవేళ (res != ఖాళీ)
		{
		    listitem_T	*li = listitem_alloc();

		    ఒకవేళ (li == ఖాళీ)
		    {
			clear_tv(cur_అంశం);
			retval = FAIL;
			వెళ్ళు theend;
		    }
		    li->li_tv = *cur_అంశం;
		    list_append(శిఖరం_అంశం->jd_tv.vval.v_list, li);
		}
		ఒకవేళ (cur_అంశం != ఖాళీ)
		    cur_అంశం = &అంశం;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_వాడిన;
		ఒకవేళ (*p == ',')
		    ++reader->js_వాడిన;
		లేకపోతే ఒకవేళ (*p != ']')
		{
		    ఒకవేళ (*p == NUL)
			retval = MAYBE;
		    లేకపోతే
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    వెళ్ళు theend;
		}
		ఆపు;

	    స్థితి JSON_OBJECT_కీలకం:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_వాడిన;
		ఒకవేళ (*p != ':')
		{
		    ఒకవేళ (cur_అంశం != ఖాళీ)
			clear_tv(cur_అంశం);
		    ఒకవేళ (*p == NUL)
			retval = MAYBE;
		    లేకపోతే
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    వెళ్ళు theend;
		}
		++reader->js_వాడిన;
		json_skip_white(reader);
		శిఖరం_అంశం->jd_type = JSON_OBJECT;
		ఒకవేళ (cur_అంశం != ఖాళీ)
		    cur_అంశం = &అంశం;
		ఆపు;

	    స్థితి JSON_OBJECT:
		ఒకవేళ (cur_అంశం != ఖాళీ
			&& dict_has_కీలకం(శిఖరం_అంశం->jd_tv.vval.v_dict,
						(అక్షరం *)శిఖరం_అంశం->jd_కీలకం))
		{
		    semsg(_(e_duplicate_కీలకం_లోపల_json_str), శిఖరం_అంశం->jd_కీలకం);
		    clear_tv(cur_అంశం);
		    retval = FAIL;
		    వెళ్ళు theend;
		}

		ఒకవేళ (cur_అంశం != ఖాళీ)
		{
		    dictitem_T *di = dictitem_alloc(శిఖరం_అంశం->jd_కీలకం);

		    clear_tv(&శిఖరం_అంశం->jd_కీలకం_tv);
		    ఒకవేళ (di == ఖాళీ)
		    {
			clear_tv(cur_అంశం);
			retval = FAIL;
			వెళ్ళు theend;
		    }
		    di->di_tv = *cur_అంశం;
		    di->di_tv.v_తాళం = 0;
		    ఒకవేళ (dict_కూడు(శిఖరం_అంశం->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			వెళ్ళు theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_వాడిన;
		ఒకవేళ (*p == ',')
		    ++reader->js_వాడిన;
		లేకపోతే ఒకవేళ (*p != '}')
		{
		    ఒకవేళ (*p == NUL)
			retval = MAYBE;
		    లేకపోతే
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    వెళ్ళు theend;
		}
		శిఖరం_అంశం->jd_type = JSON_OBJECT_కీలకం;
		ఒకవేళ (cur_అంశం != ఖాళీ)
		    cur_అంశం = &శిఖరం_అంశం->jd_కీలకం_tv;
		ఆపు;
	}
    }

    // Get here when parsing failed.
    ఒకవేళ (res != ఖాళీ)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_సంఖ్య = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_వాడిన);

theend:
    కొరకు (i = 0; i < దొంతర.ga_len; i++)
	clear_tv(&(((json_dec_అంశం_T *)దొంతర.ga_దత్తాంశం) + i)->jd_కీలకం_tv);
    ga_clear(&దొంతర);

    తిరిగి retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    స్థిర పూర్ణాంకం
json_decode_all(js_read_T *reader, typval_T *res, పూర్ణాంకం options)
{
    పూర్ణాంకం ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_చివర = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_అంశం(reader, res, options);
    ఒకవేళ (ret != OK)
    {
	ఒకవేళ (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	తిరిగి FAIL;
    }
    json_skip_white(reader);
    ఒకవేళ (reader->js_buf[reader->js_వాడిన] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_వాడిన);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_పని_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    పూర్ణాంకం
json_decode(js_read_T *reader, typval_T *res, పూర్ణాంకం options)
{
    పూర్ణాంకం ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_చివర = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_అంశం(reader, res, options);
    json_skip_white(reader);

    తిరిగి ret;
}
#అయితే_ముగింపు

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
    పూర్ణాంకం
json_కనుగొను_చివర(js_read_T *reader, పూర్ణాంకం options)
{
    పూర్ణాంకం వాడిన_save = reader->js_వాడిన;
    పూర్ణాంకం ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_చివర = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_అంశం(reader, ఖాళీ, options);
    reader->js_వాడిన = వాడిన_save;
    తిరిగి ret;
}

/*
 * "js_decode()" function
 */
    శూన్య
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ఖాళీ;
    reader.js_వాడిన = 0;
    ఒకవేళ (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_వాదం));
}

/*
 * "js_encode()" function
 */
    శూన్య
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    శూన్య
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ఖాళీ;
    reader.js_వాడిన = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    శూన్య
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#అయితే_ముగింపు
