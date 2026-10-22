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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

நிலையான முழு json_encode_item(garray_T *gap, typval_T *val, முழு copyID, முழு options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    நிலையான முழு
json_encode_gap(garray_T *gap, typval_T *val, முழு options)
{
    எனில் (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, முழு options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    எனில் (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    திரும்பு ga.ga_data;
}

#எனில் defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_கோவை(முழு nr, typval_T *val, முழு options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_எண்;
    nrtv.vval.v_எண் = nr;
    எனில் (rettv_list_alloc(&listtv) == FAIL)
	திரும்பு வெற்று;
    எனில் (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	திரும்பு வெற்று;
    }

    ga_init2(&ga, 1, 4000);
    எனில் (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    திரும்பு ga.ga_data;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_செய்தி(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    எனில் (json_encode_gap(&ga, val, 0) == FAIL)
	திரும்பு வெற்று;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((எழுத்து *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    திரும்பு lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
நிலையான மாறிலி எழுத்து ascii_needs_escape[128] = {
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
    நிலையான வெறுமை
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#எனில் defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = வெற்று;
#endif
    முழு		c;

    எனில் (res == வெற்று)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	திரும்பு;
    }

#எனில் defined(USE_ICONV)
    எனில் (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	எனில் (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, வெற்று);
	convert_setup(&conv, வெற்று, வெற்று);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (முழு)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    போது ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	எனில் (c < 0x80)
	{
	    எனில் (!ascii_needs_escape[c])
	    {
		res += 1;
		தொடர்;
	    }

	    எனில் (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    தேர்வு (c)
	    {
		நிலை 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); நிறுத்து;
		நிலை 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); நிறுத்து;
		நிலை 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); நிறுத்து;
		நிலை 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); நிறுத்து;
		நிலை 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); நிறுத்து;
		நிலை 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); நிறுத்து;
		நிலை 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); நிறுத்து;
		இயல்பு:
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
	இல்லையெனில்
	{
	    முழு	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    எனில் (l > 1)
	    {
		res += l;
		தொடர்;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    எனில் (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    எனில் (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#எனில் defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    நிலையான முழு
is_simple_சாவி(char_u *சாவி)
{
    char_u *p;

    எனில் (!ASCII_ISALPHA(*சாவி))
	திரும்பு FALSE;
    க்கு (p = சாவி + 1; *p != NUL; ++p)
	எனில் (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    திரும்பு FALSE;
    திரும்பு TRUE;
}

வகைவரை எண்ணிக்கை {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

வகைவரை கட்டமைப்பு {
    json_enc_type_T je_type;
    முழு		    je_options;	    // options when this container was entered
    ஒன்றிப்பு {
	கட்டமைப்பு {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	கட்டமைப்பு {
	    tuple_T	*tuple;
	    முழு		idx;	    // current index
	    முழு		len;
	} t;
	கட்டமைப்பு {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    முழு		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    நிலையான முழு
json_encode_item(garray_T *gap, typval_T *val, முழு copyID, முழு options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    முழு			i;
    garray_T		அடுக்கு;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&அடுக்கு, அளவு(json_enc_frame_T), 100);
    cur_val = val;

    க்கு (;;)
    {
    தேர்வு (cur_val->v_type)
    {
	நிலை VAR_BOOL:
	    தேர்வு ((long)cur_val->vval.v_எண்)
	    {
		நிலை VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); நிறுத்து;
		நிலை VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); நிறுத்து;
	    }
	    நிறுத்து;

	நிலை VAR_SPECIAL:
	    தேர்வு ((long)cur_val->vval.v_எண்)
	    {
		நிலை VVAL_NONE: எனில் ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    நிறுத்து;
				// FALLTHROUGH
		நிலை VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); நிறுத்து;
	    }
	    நிறுத்து;

	நிலை VAR_எண்:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((எழுத்து *)numbuf, அளவு(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_எண்);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    நிறுத்து;

	நிலை VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    நிறுத்து;

	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	நிலை VAR_INSTR:
	நிலை VAR_CLASS:
	நிலை VAR_OBJECT:
	நிலை VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    செல் theend;

	நிலை VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    எனில் (b == வெற்று || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    இல்லையெனில்
	    {
		முழு	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		எனில் (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    செல் theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		க்கு (i = 0; i < blen; i++)
		{
		    முழு	    byte = src[i];

		    எனில் (i > 0)
			*dst++ = ',';
		    எனில் (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    இல்லையெனில் எனில் (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    இல்லையெனில்
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (முழு)(dst - (char_u *)gap->ga_data);
	    }
	    நிறுத்து;

	நிலை VAR_LIST:
	    l = cur_val->vval.v_list;
	    எனில் (l == வெற்று)
		GA_CONCAT_LITERAL(gap, "[]");
	    இல்லையெனில்
	    {
		எனில் (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		இல்லையெனில்
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    எனில் (l->lv_first != வெற்று)
		    {
			எனில் (அடுக்கு.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    செல் theend;
			}
			எனில் (ga_grow(&அடுக்கு, 1) == FAIL)
			    செல் theend;
			frame = ((json_enc_frame_T *)அடுக்கு.ga_data)
							    + அடுக்கு.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++அடுக்கு.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			தொடர்;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    நிறுத்து;

	நிலை VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    எனில் (tuple == வெற்று)
		GA_CONCAT_LITERAL(gap, "[]");
	    இல்லையெனில்
	    {
		எனில் (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		இல்லையெனில்
		{
		    முழு		len = TUPLE_LEN(tuple);

		    எனில் (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			எனில் (அடுக்கு.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    செல் theend;
			}
			எனில் (ga_grow(&அடுக்கு, 1) == FAIL)
			    செல் theend;
			frame = ((json_enc_frame_T *)அடுக்கு.ga_data)
							    + அடுக்கு.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++அடுக்கு.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			தொடர்;
		    }
		    இல்லையெனில்
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    நிறுத்து;

	நிலை VAR_DICT:
	    d = cur_val->vval.v_dict;
	    எனில் (d == வெற்று)
		GA_CONCAT_LITERAL(gap, "{}");
	    இல்லையெனில்
	    {
		எனில் (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		இல்லையெனில்
		{
		    முழு		todo = (முழு)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    எனில் (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			க்கு (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			எனில் ((options & JSON_JS)
						 && is_simple_சாவி(hi->hi_சாவி))
			    ga_concat(gap, hi->hi_சாவி);
			இல்லையெனில்
			    write_string(gap, hi->hi_சாவி);
			ga_append(gap, ':');

			எனில் (அடுக்கு.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    செல் theend;
			}
			எனில் (ga_grow(&அடுக்கு, 1) == FAIL)
			    செல் theend;
			frame = ((json_enc_frame_T *)அடுக்கு.ga_data)
							    + அடுக்கு.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++அடுக்கு.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			தொடர்;
		    }
		    இல்லையெனில்
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    நிறுத்து;

	நிலை VAR_FLOAT:
#எனில் defined(HAVE_MATH_H)
	    எனில் (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    இல்லையெனில் எனில் (isinf(cur_val->vval.v_float))
	    {
		எனில் (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		இல்லையெனில்
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    இல்லையெனில்
#endif
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((எழுத்து *)numbuf, அளவு(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    நிறுத்து;

	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    செல் theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	க்கு (;;)
	{
	    முழு		advance = FALSE;

	    எனில் (அடுக்கு.ga_len == 0)
	    {
		ga_clear(&அடுக்கு);
		திரும்பு OK;
	    }

	    frame = ((json_enc_frame_T *)அடுக்கு.ga_data)
							+ அடுக்கு.ga_len - 1;
	    தேர்வு (frame->je_type)
	    {
		நிலை ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    எனில் ((frame->je_options & JSON_JS)
			    && li->li_next == வெற்று
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_எண் == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    எனில் (li != வெற்று && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			நிறுத்து;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --அடுக்கு.ga_len;
		    நிறுத்து;
		}

		நிலை ENC_TUPLE:
		{
		    முழு	    idx = frame->je_u.t.idx;
		    முழு	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    எனில் ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			எனில் (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_எண் == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    எனில் (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			நிறுத்து;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --அடுக்கு.ga_len;
		    நிறுத்து;
		}

		நிலை ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    முழு		todo = frame->je_u.d.todo;

		    எனில் (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			க்கு (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			எனில் ((frame->je_options & JSON_JS)
				&& is_simple_சாவி(hi->hi_சாவி))
			    ga_concat(gap, hi->hi_சாவி);
			இல்லையெனில்
			    write_string(gap, hi->hi_சாவி);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			நிறுத்து;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --அடுக்கு.ga_len;
		    நிறுத்து;
		}
	    }
	    எனில் (advance)
		நிறுத்து;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    க்கு (i = 0; i < அடுக்கு.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)அடுக்கு.ga_data) + i;
	தேர்வு (frame->je_type)
	{
	    நிலை ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		நிறுத்து;
	    நிலை ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		நிறுத்து;
	    நிலை ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		நிறுத்து;
	}
    }
    ga_clear(&அடுக்கு);
    திரும்பு FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    நிலையான வெறுமை
fill_numbuflen(js_read_T *reader)
{
    எனில் (reader->js_fill != வெற்று && (முழு)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	எனில் (reader->js_fill(reader))
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    நிலையான வெறுமை
json_skip_white(js_read_T *reader)
{
    முழு c;

    க்கு (;;)
    {
	c = reader->js_buf[reader->js_used];
	எனில் (reader->js_fill != வெற்று && c == NUL)
	{
	    எனில் (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		தொடர்;
	    }
	}
	எனில் (c == NUL || c > ' ')
	    நிறுத்து;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    நிலையான முழு
json_decode_string(js_read_T *reader, typval_T *res, முழு quote)
{
    garray_T    ga;
    முழு		len;
    char_u	*p;
    முழு		c;
    varnumber_T	nr;

    எனில் (res != வெற்று)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    போது (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	எனில் (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    எனில் (reader->js_fill == வெற்று)
		நிறுத்து;
	    len = (முழு)(reader->js_end - p);
	    reader->js_used = (முழு)(p - reader->js_buf);
	    எனில் (!reader->js_fill(reader))
		நிறுத்து; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    தொடர்;
	}

	எனில் (*p == '\\')
	{
	    c = -1;
	    தேர்வு (p[1])
	    {
		நிலை '\\': c = '\\'; நிறுத்து;
		நிலை '"': c = '"'; நிறுத்து;
		நிலை 'b': c = BS; நிறுத்து;
		நிலை 't': c = TAB; நிறுத்து;
		நிலை 'n': c = NL; நிறுத்து;
		நிலை 'f': c = FF; நிறுத்து;
		நிலை 'r': c = CAR; நிறுத்து;
		நிலை 'u':
		    எனில் (reader->js_fill != வெற்று
				     && (முழு)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (முழு)(p - reader->js_buf);
			எனில் (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, வெற்று, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, வெற்று, 4, TRUE, வெற்று);
		    எனில் (len == 0)
		    {
			எனில் (res != வெற்று)
			    ga_clear(&ga);
			திரும்பு FAIL;
		    }
		    p += len + 2;
		    எனில் (0xd800 <= nr && nr <= 0xdbff
			    && (முழு)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, வெற்று, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, வெற்று, 4, TRUE, வெற்று);
			எனில் (len == 0)
			{
			    எனில் (res != வெற்று)
				ga_clear(&ga);
			    திரும்பு FAIL;
			}
			எனில் (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    எனில் (0xd800 <= nr && nr <= 0xdfff)
		    {
			எனில் (res != வெற்று)
			    ga_clear(&ga);
			திரும்பு FAIL;
		    }
		    எனில் (res != வெற்று)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((முழு)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    நிறுத்து;
		இயல்பு:
		    // not a special char, skip over backslash
		    ++p;
		    தொடர்;
	    }
	    எனில் (c > 0)
	    {
		p += 2;
		எனில் (res != வெற்று)
		    ga_append(&ga, c);
	    }
	}
	இல்லையெனில்
	{
	    len = utf_ptr2len(p);
	    எனில் (res != வெற்று)
	    {
		எனில் (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    திரும்பு FAIL;
		}
		mch_memmove((எழுத்து *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (முழு)(p - reader->js_buf);
    எனில் (*p == quote)
    {
	++reader->js_used;
	எனில் (res != வெற்று)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#எனில் defined(USE_ICONV)
	    எனில் (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		எனில் (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, வெற்று);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, வெற்று, வெற்று);
	    }
	    இல்லையெனில்
#endif
		res->vval.v_string = ga.ga_data;
	}
	திரும்பு OK;
    }
    எனில் (res != வெற்று)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_எண் = VVAL_NONE;
	ga_clear(&ga);
    }
    திரும்பு MAYBE;
}

வகைவரை எண்ணிக்கை {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_சாவி,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

வகைவரை கட்டமைப்பு {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_சாவி_tv;
    char_u	  *jd_சாவி;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    நிலையான முழு
json_decode_item(js_read_T *reader, typval_T *res, முழு options)
{
    char_u	*p;
    முழு		i;
    முழு		len;
    முழு		retval;
    garray_T	அடுக்கு;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *உச்சி_item;
    char_u	சாவி_buf[NUMBUFLEN];

    ga_init2(&அடுக்கு, அளவு(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    எனில் (res != வெற்று)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    க்கு (;;)
    {
	உச்சி_item = வெற்று;
	எனில் (அடுக்கு.ga_len > 0)
	{
	    உச்சி_item = ((json_dec_item_T *)அடுக்கு.ga_data) + அடுக்கு.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    எனில் (*p == NUL)
	    {
		retval = MAYBE;
		செல் theend;
	    }
	    எனில் (உச்சி_item->jd_type == JSON_OBJECT_சாவி
					    || உச்சி_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		எனில் (*p == (உச்சி_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --அடுக்கு.ga_len;
		    எனில் (அடுக்கு.ga_len == 0)
		    {
			retval = OK;
			செல் theend;
		    }
		    எனில் (cur_item != வெற்று)
			cur_item = &உச்சி_item->jd_tv;
		    செல் item_end;
		}
	    }
	}

	எனில் (உச்சி_item != வெற்று && உச்சி_item->jd_type == JSON_OBJECT_சாவி
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *சாவி;

	    // accept an object key that is not in quotes
	    சாவி = p = reader->js_buf + reader->js_used;
	    போது (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    எனில் (cur_item != வெற்று)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(சாவி, p - சாவி);
		உச்சி_item->jd_சாவி = cur_item->vval.v_string;
	    }
	    reader->js_used += (முழு)(p - சாவி);
	}
	இல்லையெனில்
	{
	    தேர்வு (*p)
	    {
		நிலை '[': // start of array
		    எனில் (உச்சி_item && உச்சி_item->jd_type == JSON_OBJECT_சாவி)
		    {
			retval = FAIL;
			நிறுத்து;
		    }
		    எனில் (அடுக்கு.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			நிறுத்து;
		    }
		    எனில் (ga_grow(&அடுக்கு, 1) == FAIL)
		    {
			retval = FAIL;
			நிறுத்து;
		    }
		    எனில் (cur_item != வெற்று && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_எண் = VVAL_NONE;
			retval = FAIL;
			நிறுத்து;
		    }

		    ++reader->js_used; // consume the '['
		    உச்சி_item = ((json_dec_item_T *)அடுக்கு.ga_data)
								+ அடுக்கு.ga_len;
		    உச்சி_item->jd_type = JSON_ARRAY;
		    ++அடுக்கு.ga_len;
		    எனில் (cur_item != வெற்று)
		    {
			உச்சி_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    தொடர்;

		நிலை '{': // start of object
		    எனில் (உச்சி_item && உச்சி_item->jd_type == JSON_OBJECT_சாவி)
		    {
			retval = FAIL;
			நிறுத்து;
		    }
		    எனில் (அடுக்கு.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			நிறுத்து;
		    }
		    எனில் (ga_grow(&அடுக்கு, 1) == FAIL)
		    {
			retval = FAIL;
			நிறுத்து;
		    }
		    எனில் (cur_item != வெற்று && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_எண் = VVAL_NONE;
			retval = FAIL;
			நிறுத்து;
		    }

		    ++reader->js_used; // consume the '{'
		    உச்சி_item = ((json_dec_item_T *)அடுக்கு.ga_data)
								+ அடுக்கு.ga_len;
		    உச்சி_item->jd_type = JSON_OBJECT_சாவி;
		    ++அடுக்கு.ga_len;
		    எனில் (cur_item != வெற்று)
		    {
			உச்சி_item->jd_tv = *cur_item;
			cur_item = &உச்சி_item->jd_சாவி_tv;
		    }
		    தொடர்;

		நிலை '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    நிறுத்து;

		நிலை '\'':
		    எனில் (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    இல்லையெனில்
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    நிறுத்து;

		நிலை ',': // comma: empty item
		    எனில் ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			நிறுத்து;
		    }
		    // FALLTHROUGH
		நிலை NUL: // empty
		    எனில் (cur_item != வெற்று)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_எண் = VVAL_NONE;
		    }
		    retval = OK;
		    நிறுத்து;

		இயல்பு:
		    எனில் (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			எனில் (*sp == '-')
			{
			    ++sp;
			    எனில் (*sp == NUL)
			    {
				retval = MAYBE;
				நிறுத்து;
			    }
			    எனில் (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				நிறுத்து;
			    }
			}
			sp = skipdigits(sp);
			எனில் (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    எனில் (cur_item == வெற்று)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    இல்லையெனில்
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			இல்லையெனில்
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    வெற்று, &len, 0, // what
				    &nr, வெற்று, 0, TRUE, வெற்று);
			    எனில் (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				செல் theend;
			    }
			    எனில் (cur_item != வெற்று)
			    {
				cur_item->v_type = VAR_எண்;
				cur_item->vval.v_எண் = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			நிறுத்து;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((எழுத்து *)(p), (kw), (len)) == 0 \
     : STRNCMP((எழுத்து *)(p), (kw), (len)) == 0)
		    எனில் (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			எனில் (cur_item != வெற்று)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_எண் = VVAL_FALSE;
			}
			retval = OK;
			நிறுத்து;
		    }
		    எனில் (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			எனில் (cur_item != வெற்று)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_எண் = VVAL_TRUE;
			}
			retval = OK;
			நிறுத்து;
		    }
		    எனில் (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			எனில் (cur_item != வெற்று)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_எண் = VVAL_NULL;
			}
			retval = OK;
			நிறுத்து;
		    }
		    எனில் (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			எனில் (cur_item != வெற்று)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			நிறுத்து;
		    }
		    எனில் (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			எனில் (cur_item != வெற்று)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			நிறுத்து;
		    }
		    எனில் (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			எனில் (cur_item != வெற்று)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			நிறுத்து;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (முழு)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    எனில் (
			    (len < 5 && STRNICMP((எழுத்து *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((எழுத்து *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((எழுத்து *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((எழுத்து *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((எழுத்து *)p, "true", len) == 0
				    || STRNICMP((எழுத்து *)p, "null", len) == 0)))

			retval = MAYBE;
		    இல்லையெனில்
			retval = FAIL;
		    நிறுத்து;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    எனில் (retval == FAIL)
		நிறுத்து;
	    எனில் (retval == MAYBE || அடுக்கு.ga_len == 0)
		செல் theend;

	    எனில் (உச்சி_item != வெற்று && உச்சி_item->jd_type == JSON_OBJECT_சாவி
		    && cur_item != வெற்று)
	    {
		எனில் (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    செல் theend;
		}
		உச்சி_item->jd_சாவி = tv_get_string_buf_chk(cur_item, சாவி_buf);
		எனில் (உச்சி_item->jd_சாவி == வெற்று)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    செல் theend;
		}
	    }
	}

item_end:
	உச்சி_item = ((json_dec_item_T *)அடுக்கு.ga_data) + அடுக்கு.ga_len - 1;
	தேர்வு (உச்சி_item->jd_type)
	{
	    நிலை JSON_ARRAY:
		எனில் (res != வெற்று)
		{
		    listitem_T	*li = listitem_alloc();

		    எனில் (li == வெற்று)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			செல் theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(உச்சி_item->jd_tv.vval.v_list, li);
		}
		எனில் (cur_item != வெற்று)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		எனில் (*p == ',')
		    ++reader->js_used;
		இல்லையெனில் எனில் (*p != ']')
		{
		    எனில் (*p == NUL)
			retval = MAYBE;
		    இல்லையெனில்
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    செல் theend;
		}
		நிறுத்து;

	    நிலை JSON_OBJECT_சாவி:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		எனில் (*p != ':')
		{
		    எனில் (cur_item != வெற்று)
			clear_tv(cur_item);
		    எனில் (*p == NUL)
			retval = MAYBE;
		    இல்லையெனில்
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    செல் theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		உச்சி_item->jd_type = JSON_OBJECT;
		எனில் (cur_item != வெற்று)
		    cur_item = &item;
		நிறுத்து;

	    நிலை JSON_OBJECT:
		எனில் (cur_item != வெற்று
			&& dict_has_சாவி(உச்சி_item->jd_tv.vval.v_dict,
						(எழுத்து *)உச்சி_item->jd_சாவி))
		{
		    semsg(_(e_duplicate_சாவி_in_json_str), உச்சி_item->jd_சாவி);
		    clear_tv(cur_item);
		    retval = FAIL;
		    செல் theend;
		}

		எனில் (cur_item != வெற்று)
		{
		    dictitem_T *di = dictitem_alloc(உச்சி_item->jd_சாவி);

		    clear_tv(&உச்சி_item->jd_சாவி_tv);
		    எனில் (di == வெற்று)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			செல் theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    எனில் (dict_add(உச்சி_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			செல் theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		எனில் (*p == ',')
		    ++reader->js_used;
		இல்லையெனில் எனில் (*p != '}')
		{
		    எனில் (*p == NUL)
			retval = MAYBE;
		    இல்லையெனில்
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    செல் theend;
		}
		உச்சி_item->jd_type = JSON_OBJECT_சாவி;
		எனில் (cur_item != வெற்று)
		    cur_item = &உச்சி_item->jd_சாவி_tv;
		நிறுத்து;
	}
    }

    // Get here when parsing failed.
    எனில் (res != வெற்று)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_எண் = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    க்கு (i = 0; i < அடுக்கு.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)அடுக்கு.ga_data) + i)->jd_சாவி_tv);
    ga_clear(&அடுக்கு);

    திரும்பு retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    நிலையான முழு
json_decode_all(js_read_T *reader, typval_T *res, முழு options)
{
    முழு ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    எனில் (ret != OK)
    {
	எனில் (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	திரும்பு FAIL;
    }
    json_skip_white(reader);
    எனில் (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

#எனில் defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    முழு
json_decode(js_read_T *reader, typval_T *res, முழு options)
{
    முழு ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    திரும்பு ret;
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
    முழு
json_find_end(js_read_T *reader, முழு options)
{
    முழு used_save = reader->js_used;
    முழு ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, வெற்று, options);
    reader->js_used = used_save;
    திரும்பு ret;
}

/*
 * "js_decode()" function
 */
    வெறுமை
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = வெற்று;
    reader.js_used = 0;
    எனில் (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_argument));
}

/*
 * "js_encode()" function
 */
    வெறுமை
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    வெறுமை
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = வெற்று;
    reader.js_used = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    வெறுமை
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#endif
