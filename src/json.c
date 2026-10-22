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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

સ્થિર પૂર્ણાંક json_encode_item(garray_T *gap, typval_T *val, પૂર્ણાંક copyID, પૂર્ણાંક options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    સ્થિર પૂર્ણાંક
json_encode_gap(garray_T *gap, typval_T *val, પૂર્ણાંક options)
{
    જો (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, પૂર્ણાંક options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    જો (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    પાછા ga.ga_data;
}

#જો defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_પદાવલિ(પૂર્ણાંક nr, typval_T *val, પૂર્ણાંક options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_સંખ્યા;
    nrtv.vval.v_સંખ્યા = nr;
    જો (rettv_list_alloc(&listtv) == FAIL)
	પાછા ખાલી;
    જો (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	પાછા ખાલી;
    }

    ga_init2(&ga, 1, 4000);
    જો (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    પાછા ga.ga_data;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_સંદેશ(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    જો (json_encode_gap(&ga, val, 0) == FAIL)
	પાછા ખાલી;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((અક્ષર *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    પાછા lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
સ્થિર અચલ અક્ષર ascii_needs_escape[128] = {
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
    સ્થિર શૂન્ય
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#જો defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = ખાલી;
#endif
    પૂર્ણાંક		c;

    જો (res == ખાલી)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	પાછા;
    }

#જો defined(USE_ICONV)
    જો (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	જો (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, ખાલી);
	convert_setup(&conv, ખાલી, ખાલી);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (પૂર્ણાંક)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    જ્યારે ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	જો (c < 0x80)
	{
	    જો (!ascii_needs_escape[c])
	    {
		res += 1;
		ચાલુ;
	    }

	    જો (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    પસંદગી (c)
	    {
		સ્થિતિ 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); તોડો;
		સ્થિતિ 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); તોડો;
		સ્થિતિ 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); તોડો;
		સ્થિતિ 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); તોડો;
		સ્થિતિ 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); તોડો;
		સ્થિતિ 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); તોડો;
		સ્થિતિ 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); તોડો;
		મૂળભૂત:
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
	નહીંતર
	{
	    પૂર્ણાંક	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    જો (l > 1)
	    {
		res += l;
		ચાલુ;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    જો (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    જો (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#જો defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    સ્થિર પૂર્ણાંક
is_simple_ચાવી(char_u *ચાવી)
{
    char_u *p;

    જો (!ASCII_ISALPHA(*ચાવી))
	પાછા FALSE;
    માટે (p = ચાવી + 1; *p != NUL; ++p)
	જો (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    પાછા FALSE;
    પાછા TRUE;
}

પ્રકારવ્યાખ્યા ગણના {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

પ્રકારવ્યાખ્યા સંરચના {
    json_enc_type_T je_type;
    પૂર્ણાંક		    je_options;	    // options when this container was entered
    સંઘ {
	સંરચના {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	સંરચના {
	    tuple_T	*tuple;
	    પૂર્ણાંક		idx;	    // current index
	    પૂર્ણાંક		len;
	} t;
	સંરચના {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    પૂર્ણાંક		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    સ્થિર પૂર્ણાંક
json_encode_item(garray_T *gap, typval_T *val, પૂર્ણાંક copyID, પૂર્ણાંક options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    પૂર્ણાંક			i;
    garray_T		ઢગલો;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&ઢગલો, કદ(json_enc_frame_T), 100);
    cur_val = val;

    માટે (;;)
    {
    પસંદગી (cur_val->v_type)
    {
	સ્થિતિ VAR_BOOL:
	    પસંદગી ((long)cur_val->vval.v_સંખ્યા)
	    {
		સ્થિતિ VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); તોડો;
		સ્થિતિ VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); તોડો;
	    }
	    તોડો;

	સ્થિતિ VAR_SPECIAL:
	    પસંદગી ((long)cur_val->vval.v_સંખ્યા)
	    {
		સ્થિતિ VVAL_NONE: જો ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    તોડો;
				// FALLTHROUGH
		સ્થિતિ VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); તોડો;
	    }
	    તોડો;

	સ્થિતિ VAR_સંખ્યા:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((અક્ષર *)numbuf, કદ(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_સંખ્યા);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    તોડો;

	સ્થિતિ VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    તોડો;

	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_CHANNEL:
	સ્થિતિ VAR_INSTR:
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_OBJECT:
	સ્થિતિ VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    જાઓ theend;

	સ્થિતિ VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    જો (b == ખાલી || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    નહીંતર
	    {
		પૂર્ણાંક	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		જો (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    જાઓ theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		માટે (i = 0; i < blen; i++)
		{
		    પૂર્ણાંક	    byte = src[i];

		    જો (i > 0)
			*dst++ = ',';
		    જો (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    નહીંતર જો (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    નહીંતર
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (પૂર્ણાંક)(dst - (char_u *)gap->ga_data);
	    }
	    તોડો;

	સ્થિતિ VAR_LIST:
	    l = cur_val->vval.v_list;
	    જો (l == ખાલી)
		GA_CONCAT_LITERAL(gap, "[]");
	    નહીંતર
	    {
		જો (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		નહીંતર
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    જો (l->lv_first != ખાલી)
		    {
			જો (ઢગલો.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    જાઓ theend;
			}
			જો (ga_grow(&ઢગલો, 1) == FAIL)
			    જાઓ theend;
			frame = ((json_enc_frame_T *)ઢગલો.ga_data)
							    + ઢગલો.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++ઢગલો.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			ચાલુ;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    તોડો;

	સ્થિતિ VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    જો (tuple == ખાલી)
		GA_CONCAT_LITERAL(gap, "[]");
	    નહીંતર
	    {
		જો (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		નહીંતર
		{
		    પૂર્ણાંક		len = TUPLE_LEN(tuple);

		    જો (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			જો (ઢગલો.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    જાઓ theend;
			}
			જો (ga_grow(&ઢગલો, 1) == FAIL)
			    જાઓ theend;
			frame = ((json_enc_frame_T *)ઢગલો.ga_data)
							    + ઢગલો.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++ઢગલો.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			ચાલુ;
		    }
		    નહીંતર
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    તોડો;

	સ્થિતિ VAR_DICT:
	    d = cur_val->vval.v_dict;
	    જો (d == ખાલી)
		GA_CONCAT_LITERAL(gap, "{}");
	    નહીંતર
	    {
		જો (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		નહીંતર
		{
		    પૂર્ણાંક		todo = (પૂર્ણાંક)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    જો (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			માટે (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			જો ((options & JSON_JS)
						 && is_simple_ચાવી(hi->hi_ચાવી))
			    ga_concat(gap, hi->hi_ચાવી);
			નહીંતર
			    write_string(gap, hi->hi_ચાવી);
			ga_append(gap, ':');

			જો (ઢગલો.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    જાઓ theend;
			}
			જો (ga_grow(&ઢગલો, 1) == FAIL)
			    જાઓ theend;
			frame = ((json_enc_frame_T *)ઢગલો.ga_data)
							    + ઢગલો.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++ઢગલો.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			ચાલુ;
		    }
		    નહીંતર
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    તોડો;

	સ્થિતિ VAR_FLOAT:
#જો defined(HAVE_MATH_H)
	    જો (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    નહીંતર જો (isinf(cur_val->vval.v_float))
	    {
		જો (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		નહીંતર
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    નહીંતર
#endif
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((અક્ષર *)numbuf, કદ(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    તોડો;

	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    જાઓ theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	માટે (;;)
	{
	    પૂર્ણાંક		advance = FALSE;

	    જો (ઢગલો.ga_len == 0)
	    {
		ga_clear(&ઢગલો);
		પાછા OK;
	    }

	    frame = ((json_enc_frame_T *)ઢગલો.ga_data)
							+ ઢગલો.ga_len - 1;
	    પસંદગી (frame->je_type)
	    {
		સ્થિતિ ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    જો ((frame->je_options & JSON_JS)
			    && li->li_next == ખાલી
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_સંખ્યા == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    જો (li != ખાલી && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			તોડો;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --ઢગલો.ga_len;
		    તોડો;
		}

		સ્થિતિ ENC_TUPLE:
		{
		    પૂર્ણાંક	    idx = frame->je_u.t.idx;
		    પૂર્ણાંક	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    જો ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			જો (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_સંખ્યા == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    જો (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			તોડો;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --ઢગલો.ga_len;
		    તોડો;
		}

		સ્થિતિ ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    પૂર્ણાંક		todo = frame->je_u.d.todo;

		    જો (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			માટે (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			જો ((frame->je_options & JSON_JS)
				&& is_simple_ચાવી(hi->hi_ચાવી))
			    ga_concat(gap, hi->hi_ચાવી);
			નહીંતર
			    write_string(gap, hi->hi_ચાવી);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			તોડો;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --ઢગલો.ga_len;
		    તોડો;
		}
	    }
	    જો (advance)
		તોડો;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    માટે (i = 0; i < ઢગલો.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)ઢગલો.ga_data) + i;
	પસંદગી (frame->je_type)
	{
	    સ્થિતિ ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		તોડો;
	    સ્થિતિ ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		તોડો;
	    સ્થિતિ ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		તોડો;
	}
    }
    ga_clear(&ઢગલો);
    પાછા FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    સ્થિર શૂન્ય
fill_numbuflen(js_read_T *reader)
{
    જો (reader->js_fill != ખાલી && (પૂર્ણાંક)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	જો (reader->js_fill(reader))
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    સ્થિર શૂન્ય
json_skip_white(js_read_T *reader)
{
    પૂર્ણાંક c;

    માટે (;;)
    {
	c = reader->js_buf[reader->js_used];
	જો (reader->js_fill != ખાલી && c == NUL)
	{
	    જો (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		ચાલુ;
	    }
	}
	જો (c == NUL || c > ' ')
	    તોડો;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    સ્થિર પૂર્ણાંક
json_decode_string(js_read_T *reader, typval_T *res, પૂર્ણાંક quote)
{
    garray_T    ga;
    પૂર્ણાંક		len;
    char_u	*p;
    પૂર્ણાંક		c;
    varnumber_T	nr;

    જો (res != ખાલી)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    જ્યારે (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	જો (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    જો (reader->js_fill == ખાલી)
		તોડો;
	    len = (પૂર્ણાંક)(reader->js_end - p);
	    reader->js_used = (પૂર્ણાંક)(p - reader->js_buf);
	    જો (!reader->js_fill(reader))
		તોડો; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    ચાલુ;
	}

	જો (*p == '\\')
	{
	    c = -1;
	    પસંદગી (p[1])
	    {
		સ્થિતિ '\\': c = '\\'; તોડો;
		સ્થિતિ '"': c = '"'; તોડો;
		સ્થિતિ 'b': c = BS; તોડો;
		સ્થિતિ 't': c = TAB; તોડો;
		સ્થિતિ 'n': c = NL; તોડો;
		સ્થિતિ 'f': c = FF; તોડો;
		સ્થિતિ 'r': c = CAR; તોડો;
		સ્થિતિ 'u':
		    જો (reader->js_fill != ખાલી
				     && (પૂર્ણાંક)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (પૂર્ણાંક)(p - reader->js_buf);
			જો (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, ખાલી, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, ખાલી, 4, TRUE, ખાલી);
		    જો (len == 0)
		    {
			જો (res != ખાલી)
			    ga_clear(&ga);
			પાછા FAIL;
		    }
		    p += len + 2;
		    જો (0xd800 <= nr && nr <= 0xdbff
			    && (પૂર્ણાંક)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, ખાલી, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, ખાલી, 4, TRUE, ખાલી);
			જો (len == 0)
			{
			    જો (res != ખાલી)
				ga_clear(&ga);
			    પાછા FAIL;
			}
			જો (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    જો (0xd800 <= nr && nr <= 0xdfff)
		    {
			જો (res != ખાલી)
			    ga_clear(&ga);
			પાછા FAIL;
		    }
		    જો (res != ખાલી)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((પૂર્ણાંક)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    તોડો;
		મૂળભૂત:
		    // not a special char, skip over backslash
		    ++p;
		    ચાલુ;
	    }
	    જો (c > 0)
	    {
		p += 2;
		જો (res != ખાલી)
		    ga_append(&ga, c);
	    }
	}
	નહીંતર
	{
	    len = utf_ptr2len(p);
	    જો (res != ખાલી)
	    {
		જો (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    પાછા FAIL;
		}
		mch_memmove((અક્ષર *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (પૂર્ણાંક)(p - reader->js_buf);
    જો (*p == quote)
    {
	++reader->js_used;
	જો (res != ખાલી)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#જો defined(USE_ICONV)
	    જો (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		જો (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, ખાલી);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, ખાલી, ખાલી);
	    }
	    નહીંતર
#endif
		res->vval.v_string = ga.ga_data;
	}
	પાછા OK;
    }
    જો (res != ખાલી)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_સંખ્યા = VVAL_NONE;
	ga_clear(&ga);
    }
    પાછા MAYBE;
}

પ્રકારવ્યાખ્યા ગણના {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_ચાવી,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

પ્રકારવ્યાખ્યા સંરચના {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_ચાવી_tv;
    char_u	  *jd_ચાવી;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    સ્થિર પૂર્ણાંક
json_decode_item(js_read_T *reader, typval_T *res, પૂર્ણાંક options)
{
    char_u	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		len;
    પૂર્ણાંક		retval;
    garray_T	ઢગલો;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *ટોચ_item;
    char_u	ચાવી_buf[NUMBUFLEN];

    ga_init2(&ઢગલો, કદ(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    જો (res != ખાલી)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    માટે (;;)
    {
	ટોચ_item = ખાલી;
	જો (ઢગલો.ga_len > 0)
	{
	    ટોચ_item = ((json_dec_item_T *)ઢગલો.ga_data) + ઢગલો.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    જો (*p == NUL)
	    {
		retval = MAYBE;
		જાઓ theend;
	    }
	    જો (ટોચ_item->jd_type == JSON_OBJECT_ચાવી
					    || ટોચ_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		જો (*p == (ટોચ_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --ઢગલો.ga_len;
		    જો (ઢગલો.ga_len == 0)
		    {
			retval = OK;
			જાઓ theend;
		    }
		    જો (cur_item != ખાલી)
			cur_item = &ટોચ_item->jd_tv;
		    જાઓ item_end;
		}
	    }
	}

	જો (ટોચ_item != ખાલી && ટોચ_item->jd_type == JSON_OBJECT_ચાવી
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *ચાવી;

	    // accept an object key that is not in quotes
	    ચાવી = p = reader->js_buf + reader->js_used;
	    જ્યારે (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    જો (cur_item != ખાલી)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(ચાવી, p - ચાવી);
		ટોચ_item->jd_ચાવી = cur_item->vval.v_string;
	    }
	    reader->js_used += (પૂર્ણાંક)(p - ચાવી);
	}
	નહીંતર
	{
	    પસંદગી (*p)
	    {
		સ્થિતિ '[': // start of array
		    જો (ટોચ_item && ટોચ_item->jd_type == JSON_OBJECT_ચાવી)
		    {
			retval = FAIL;
			તોડો;
		    }
		    જો (ઢગલો.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			તોડો;
		    }
		    જો (ga_grow(&ઢગલો, 1) == FAIL)
		    {
			retval = FAIL;
			તોડો;
		    }
		    જો (cur_item != ખાલી && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_સંખ્યા = VVAL_NONE;
			retval = FAIL;
			તોડો;
		    }

		    ++reader->js_used; // consume the '['
		    ટોચ_item = ((json_dec_item_T *)ઢગલો.ga_data)
								+ ઢગલો.ga_len;
		    ટોચ_item->jd_type = JSON_ARRAY;
		    ++ઢગલો.ga_len;
		    જો (cur_item != ખાલી)
		    {
			ટોચ_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    ચાલુ;

		સ્થિતિ '{': // start of object
		    જો (ટોચ_item && ટોચ_item->jd_type == JSON_OBJECT_ચાવી)
		    {
			retval = FAIL;
			તોડો;
		    }
		    જો (ઢગલો.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			તોડો;
		    }
		    જો (ga_grow(&ઢગલો, 1) == FAIL)
		    {
			retval = FAIL;
			તોડો;
		    }
		    જો (cur_item != ખાલી && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_સંખ્યા = VVAL_NONE;
			retval = FAIL;
			તોડો;
		    }

		    ++reader->js_used; // consume the '{'
		    ટોચ_item = ((json_dec_item_T *)ઢગલો.ga_data)
								+ ઢગલો.ga_len;
		    ટોચ_item->jd_type = JSON_OBJECT_ચાવી;
		    ++ઢગલો.ga_len;
		    જો (cur_item != ખાલી)
		    {
			ટોચ_item->jd_tv = *cur_item;
			cur_item = &ટોચ_item->jd_ચાવી_tv;
		    }
		    ચાલુ;

		સ્થિતિ '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    તોડો;

		સ્થિતિ '\'':
		    જો (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    નહીંતર
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    તોડો;

		સ્થિતિ ',': // comma: empty item
		    જો ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			તોડો;
		    }
		    // FALLTHROUGH
		સ્થિતિ NUL: // empty
		    જો (cur_item != ખાલી)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_સંખ્યા = VVAL_NONE;
		    }
		    retval = OK;
		    તોડો;

		મૂળભૂત:
		    જો (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			જો (*sp == '-')
			{
			    ++sp;
			    જો (*sp == NUL)
			    {
				retval = MAYBE;
				તોડો;
			    }
			    જો (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				તોડો;
			    }
			}
			sp = skipdigits(sp);
			જો (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    જો (cur_item == ખાલી)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    નહીંતર
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			નહીંતર
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    ખાલી, &len, 0, // what
				    &nr, ખાલી, 0, TRUE, ખાલી);
			    જો (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				જાઓ theend;
			    }
			    જો (cur_item != ખાલી)
			    {
				cur_item->v_type = VAR_સંખ્યા;
				cur_item->vval.v_સંખ્યા = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			તોડો;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((અક્ષર *)(p), (kw), (len)) == 0 \
     : STRNCMP((અક્ષર *)(p), (kw), (len)) == 0)
		    જો (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			જો (cur_item != ખાલી)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_સંખ્યા = VVAL_FALSE;
			}
			retval = OK;
			તોડો;
		    }
		    જો (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			જો (cur_item != ખાલી)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_સંખ્યા = VVAL_TRUE;
			}
			retval = OK;
			તોડો;
		    }
		    જો (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			જો (cur_item != ખાલી)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_સંખ્યા = VVAL_NULL;
			}
			retval = OK;
			તોડો;
		    }
		    જો (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			જો (cur_item != ખાલી)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			તોડો;
		    }
		    જો (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			જો (cur_item != ખાલી)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			તોડો;
		    }
		    જો (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			જો (cur_item != ખાલી)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			તોડો;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (પૂર્ણાંક)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    જો (
			    (len < 5 && STRNICMP((અક્ષર *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((અક્ષર *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((અક્ષર *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((અક્ષર *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((અક્ષર *)p, "true", len) == 0
				    || STRNICMP((અક્ષર *)p, "null", len) == 0)))

			retval = MAYBE;
		    નહીંતર
			retval = FAIL;
		    તોડો;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    જો (retval == FAIL)
		તોડો;
	    જો (retval == MAYBE || ઢગલો.ga_len == 0)
		જાઓ theend;

	    જો (ટોચ_item != ખાલી && ટોચ_item->jd_type == JSON_OBJECT_ચાવી
		    && cur_item != ખાલી)
	    {
		જો (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    જાઓ theend;
		}
		ટોચ_item->jd_ચાવી = tv_get_string_buf_chk(cur_item, ચાવી_buf);
		જો (ટોચ_item->jd_ચાવી == ખાલી)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    જાઓ theend;
		}
	    }
	}

item_end:
	ટોચ_item = ((json_dec_item_T *)ઢગલો.ga_data) + ઢગલો.ga_len - 1;
	પસંદગી (ટોચ_item->jd_type)
	{
	    સ્થિતિ JSON_ARRAY:
		જો (res != ખાલી)
		{
		    listitem_T	*li = listitem_alloc();

		    જો (li == ખાલી)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			જાઓ theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(ટોચ_item->jd_tv.vval.v_list, li);
		}
		જો (cur_item != ખાલી)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		જો (*p == ',')
		    ++reader->js_used;
		નહીંતર જો (*p != ']')
		{
		    જો (*p == NUL)
			retval = MAYBE;
		    નહીંતર
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    જાઓ theend;
		}
		તોડો;

	    સ્થિતિ JSON_OBJECT_ચાવી:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		જો (*p != ':')
		{
		    જો (cur_item != ખાલી)
			clear_tv(cur_item);
		    જો (*p == NUL)
			retval = MAYBE;
		    નહીંતર
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    જાઓ theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		ટોચ_item->jd_type = JSON_OBJECT;
		જો (cur_item != ખાલી)
		    cur_item = &item;
		તોડો;

	    સ્થિતિ JSON_OBJECT:
		જો (cur_item != ખાલી
			&& dict_has_ચાવી(ટોચ_item->jd_tv.vval.v_dict,
						(અક્ષર *)ટોચ_item->jd_ચાવી))
		{
		    semsg(_(e_duplicate_ચાવી_in_json_str), ટોચ_item->jd_ચાવી);
		    clear_tv(cur_item);
		    retval = FAIL;
		    જાઓ theend;
		}

		જો (cur_item != ખાલી)
		{
		    dictitem_T *di = dictitem_alloc(ટોચ_item->jd_ચાવી);

		    clear_tv(&ટોચ_item->jd_ચાવી_tv);
		    જો (di == ખાલી)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			જાઓ theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    જો (dict_add(ટોચ_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			જાઓ theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		જો (*p == ',')
		    ++reader->js_used;
		નહીંતર જો (*p != '}')
		{
		    જો (*p == NUL)
			retval = MAYBE;
		    નહીંતર
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    જાઓ theend;
		}
		ટોચ_item->jd_type = JSON_OBJECT_ચાવી;
		જો (cur_item != ખાલી)
		    cur_item = &ટોચ_item->jd_ચાવી_tv;
		તોડો;
	}
    }

    // Get here when parsing failed.
    જો (res != ખાલી)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_સંખ્યા = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    માટે (i = 0; i < ઢગલો.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)ઢગલો.ga_data) + i)->jd_ચાવી_tv);
    ga_clear(&ઢગલો);

    પાછા retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    સ્થિર પૂર્ણાંક
json_decode_all(js_read_T *reader, typval_T *res, પૂર્ણાંક options)
{
    પૂર્ણાંક ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    જો (ret != OK)
    {
	જો (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	પાછા FAIL;
    }
    json_skip_white(reader);
    જો (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	પાછા FAIL;
    }
    પાછા OK;
}

#જો defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    પૂર્ણાંક
json_decode(js_read_T *reader, typval_T *res, પૂર્ણાંક options)
{
    પૂર્ણાંક ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    પાછા ret;
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
    પૂર્ણાંક
json_find_end(js_read_T *reader, પૂર્ણાંક options)
{
    પૂર્ણાંક used_save = reader->js_used;
    પૂર્ણાંક ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, ખાલી, options);
    reader->js_used = used_save;
    પાછા ret;
}

/*
 * "js_decode()" function
 */
    શૂન્ય
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ખાલી;
    reader.js_used = 0;
    જો (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_argument));
}

/*
 * "js_encode()" function
 */
    શૂન્ય
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    શૂન્ય
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = ખાલી;
    reader.js_used = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    શૂન્ય
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#endif
