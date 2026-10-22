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

#include "vim.h"

#ᰀᰧ defined(FEAT_EVAL)

static puur_naa_nka json_encode_item(garray_T *gap, typval_T *val, puur_naa_nka copyID, puur_naa_nka options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    static puur_naa_nka
json_encode_gap(garray_T *gap, typval_T *val, puur_naa_nka options)
{
    ᰀᰧ (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	ᰀᰧᰌ FAIL;
    }
    ᰀᰧᰌ OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, puur_naa_nka options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    ᰀᰧ (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    ᰀᰧᰌ ga.ga_data;
}

#ᰀᰧ defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_expr(puur_naa_nka nr, typval_T *val, puur_naa_nka options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_NUMBER;
    nrtv.vval.v_number = nr;
    ᰀᰧ (rettv_list_alloc(&listtv) == FAIL)
	ᰀᰧᰌ NULL;
    ᰀᰧ (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	ᰀᰧᰌ NULL;
    }

    ga_init2(&ga, 1, 4000);
    ᰀᰧ (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    ᰀᰧᰌ ga.ga_data;
}

/*
 * Encode "val" into a JSON format string prefixed by the LSP HTTP header.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_lsp_msg(typval_T *val)
{
    garray_T	ga;
    garray_T	lspga;
    size_t	IObufflen;

    ga_init2(&ga, 1, 4000);
    ᰀᰧ (json_encode_gap(&ga, val, 0) == FAIL)
	ᰀᰧᰌ NULL;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((var_na *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    ᰀᰧᰌ lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
static const var_na ascii_needs_escape[128] = {
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
    static void
write_string(garray_T *gap, char_u *str)
{
    char_u	*res = str;
    char_u	numbuf[NUMBUFLEN];
    char_u	*from;
#ᰀᰧ defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = NULL;
#endif
    puur_naa_nka		c;

    ᰀᰧ (res == NULL)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	ᰀᰧᰌ;
    }

#ᰀᰧ defined(USE_ICONV)
    ᰀᰧ (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	ᰀᰧ (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, NULL);
	convert_setup(&conv, NULL, NULL);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (puur_naa_nka)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    ᰀᰬ ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	ᰀᰧ (c < 0x80)
	{
	    ᰀᰧ (!ascii_needs_escape[c])
	    {
		res += 1;
		continue;
	    }

	    ᰀᰧ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    switch (c)
	    {
		pak_sa 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); bha_nga;
		pak_sa 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); bha_nga;
		pak_sa 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); bha_nga;
		pak_sa 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); bha_nga;
		pak_sa 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); bha_nga;
		pak_sa 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); bha_nga;
		pak_sa 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); bha_nga;
		default:
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
	ᰀᰧᰀ
	{
	    puur_naa_nka	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    ᰀᰧ (l > 1)
	    {
		res += l;
		continue;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    ᰀᰧ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    ᰀᰧ (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#ᰀᰧ defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    static puur_naa_nka
is_simple_key(char_u *key)
{
    char_u *p;

    ᰀᰧ (!ASCII_ISALPHA(*key))
	ᰀᰧᰌ FALSE;
    for (p = key + 1; *p != NUL; ++p)
	ᰀᰧ (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    ᰀᰧᰌ FALSE;
    ᰀᰧᰌ TRUE;
}

typedef ga_nanaa {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

typedef sa_nracanaa {
    json_enc_type_T je_type;
    puur_naa_nka		    je_options;	    // options when this container was entered
    sa_ngha {
	sa_nracanaa {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	sa_nracanaa {
	    tuple_T	*tuple;
	    puur_naa_nka		idx;	    // current index
	    puur_naa_nka		len;
	} t;
	sa_nracanaa {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    puur_naa_nka		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    static puur_naa_nka
json_encode_item(garray_T *gap, typval_T *val, puur_naa_nka copyID, puur_naa_nka options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    puur_naa_nka			i;
    garray_T		stack;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&stack, parimaa_na(json_enc_frame_T), 100);
    cur_val = val;

    for (;;)
    {
    switch (cur_val->v_type)
    {
	pak_sa VAR_BOOL:
	    switch ((long)cur_val->vval.v_number)
	    {
		pak_sa VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); bha_nga;
		pak_sa VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); bha_nga;
	    }
	    bha_nga;

	pak_sa VAR_SPECIAL:
	    switch ((long)cur_val->vval.v_number)
	    {
		pak_sa VVAL_NONE: ᰀᰧ ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    bha_nga;
				// FALLTHROUGH
		pak_sa VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); bha_nga;
	    }
	    bha_nga;

	pak_sa VAR_NUMBER:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((var_na *)numbuf, parimaa_na(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_number);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    bha_nga;

	pak_sa VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    bha_nga;

	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	pak_sa VAR_JOB:
	pak_sa VAR_CHANNEL:
	pak_sa VAR_INSTR:
	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	pak_sa VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    goto theend;

	pak_sa VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    ᰀᰧ (b == NULL || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    ᰀᰧᰀ
	    {
		puur_naa_nka	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		ᰀᰧ (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    goto theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		for (i = 0; i < blen; i++)
		{
		    puur_naa_nka	    byte = src[i];

		    ᰀᰧ (i > 0)
			*dst++ = ',';
		    ᰀᰧ (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    ᰀᰧᰀ ᰀᰧ (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    ᰀᰧᰀ
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (puur_naa_nka)(dst - (char_u *)gap->ga_data);
	    }
	    bha_nga;

	pak_sa VAR_LIST:
	    l = cur_val->vval.v_list;
	    ᰀᰧ (l == NULL)
		GA_CONCAT_LITERAL(gap, "[]");
	    ᰀᰧᰀ
	    {
		ᰀᰧ (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ᰀᰧᰀ
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    ᰀᰧ (l->lv_first != NULL)
		    {
			ᰀᰧ (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			ᰀᰧ (ga_grow(&stack, 1) == FAIL)
			    goto theend;
			frame = ((json_enc_frame_T *)stack.ga_data)
							    + stack.ga_len;
			frame->je_type = ENC_LIST;
			frame->je_options = options;
			frame->je_u.l.list = l;
			frame->je_u.l.li = l->lv_first;
			++stack.ga_len;
			options = options & JSON_JS;
			cur_val = &l->lv_first->li_tv;
			continue;
		    }
		    ga_append(gap, ']');
		    l->lv_copyID = 0;
		}
	    }
	    bha_nga;

	pak_sa VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    ᰀᰧ (tuple == NULL)
		GA_CONCAT_LITERAL(gap, "[]");
	    ᰀᰧᰀ
	    {
		ᰀᰧ (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		ᰀᰧᰀ
		{
		    puur_naa_nka		len = TUPLE_LEN(tuple);

		    ᰀᰧ (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			ᰀᰧ (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			ᰀᰧ (ga_grow(&stack, 1) == FAIL)
			    goto theend;
			frame = ((json_enc_frame_T *)stack.ga_data)
							    + stack.ga_len;
			frame->je_type = ENC_TUPLE;
			frame->je_options = options;
			frame->je_u.t.tuple = tuple;
			frame->je_u.t.idx = 0;
			frame->je_u.t.len = len;
			++stack.ga_len;
			options = options & JSON_JS;
			cur_val = TUPLE_ITEM(tuple, 0);
			continue;
		    }
		    ᰀᰧᰀ
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    bha_nga;

	pak_sa VAR_DICT:
	    d = cur_val->vval.v_dict;
	    ᰀᰧ (d == NULL)
		GA_CONCAT_LITERAL(gap, "{}");
	    ᰀᰧᰀ
	    {
		ᰀᰧ (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		ᰀᰧᰀ
		{
		    puur_naa_nka		todo = (puur_naa_nka)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    ᰀᰧ (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			for (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			ᰀᰧ ((options & JSON_JS)
						 && is_simple_key(hi->hi_key))
			    ga_concat(gap, hi->hi_key);
			ᰀᰧᰀ
			    write_string(gap, hi->hi_key);
			ga_append(gap, ':');

			ᰀᰧ (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			ᰀᰧ (ga_grow(&stack, 1) == FAIL)
			    goto theend;
			frame = ((json_enc_frame_T *)stack.ga_data)
							    + stack.ga_len;
			frame->je_type = ENC_DICT;
			frame->je_options = options;
			frame->je_u.d.dict = d;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;
			++stack.ga_len;
			options = options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			continue;
		    }
		    ᰀᰧᰀ
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    bha_nga;

	pak_sa VAR_FLOAT:
#ᰀᰧ defined(HAVE_MATH_H)
	    ᰀᰧ (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    ᰀᰧᰀ ᰀᰧ (isinf(cur_val->vval.v_float))
	    {
		ᰀᰧ (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		ᰀᰧᰀ
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    ᰀᰧᰀ
#endif
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((var_na *)numbuf, parimaa_na(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    bha_nga;

	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    goto theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	for (;;)
	{
	    puur_naa_nka		advance = FALSE;

	    ᰀᰧ (stack.ga_len == 0)
	    {
		ga_clear(&stack);
		ᰀᰧᰌ OK;
	    }

	    frame = ((json_enc_frame_T *)stack.ga_data)
							+ stack.ga_len - 1;
	    switch (frame->je_type)
	    {
		pak_sa ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    ᰀᰧ ((frame->je_options & JSON_JS)
			    && li->li_next == NULL
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_number == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    ᰀᰧ (li != NULL && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			bha_nga;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --stack.ga_len;
		    bha_nga;
		}

		pak_sa ENC_TUPLE:
		{
		    puur_naa_nka	    idx = frame->je_u.t.idx;
		    puur_naa_nka	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    ᰀᰧ ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			ᰀᰧ (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_number == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    ᰀᰧ (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			bha_nga;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --stack.ga_len;
		    bha_nga;
		}

		pak_sa ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    puur_naa_nka		todo = frame->je_u.d.todo;

		    ᰀᰧ (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			for (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			ᰀᰧ ((frame->je_options & JSON_JS)
				&& is_simple_key(hi->hi_key))
			    ga_concat(gap, hi->hi_key);
			ᰀᰧᰀ
			    write_string(gap, hi->hi_key);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			bha_nga;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --stack.ga_len;
		    bha_nga;
		}
	    }
	    ᰀᰧ (advance)
		bha_nga;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    for (i = 0; i < stack.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)stack.ga_data) + i;
	switch (frame->je_type)
	{
	    pak_sa ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		bha_nga;
	    pak_sa ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		bha_nga;
	    pak_sa ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		bha_nga;
	}
    }
    ga_clear(&stack);
    ᰀᰧᰌ FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    static void
fill_numbuflen(js_read_T *reader)
{
    ᰀᰧ (reader->js_fill != NULL && (puur_naa_nka)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	ᰀᰧ (reader->js_fill(reader))
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    }
}

/*
 * Skip white space in "reader".  All characters <= space are considered white
 * space.
 * Also tops up readahead when needed.
 */
    static void
json_skip_white(js_read_T *reader)
{
    puur_naa_nka c;

    for (;;)
    {
	c = reader->js_buf[reader->js_used];
	ᰀᰧ (reader->js_fill != NULL && c == NUL)
	{
	    ᰀᰧ (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		continue;
	    }
	}
	ᰀᰧ (c == NUL || c > ' ')
	    bha_nga;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    static puur_naa_nka
json_decode_string(js_read_T *reader, typval_T *res, puur_naa_nka quote)
{
    garray_T    ga;
    puur_naa_nka		len;
    char_u	*p;
    puur_naa_nka		c;
    varnumber_T	nr;

    ᰀᰧ (res != NULL)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    ᰀᰬ (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	ᰀᰧ (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    ᰀᰧ (reader->js_fill == NULL)
		bha_nga;
	    len = (puur_naa_nka)(reader->js_end - p);
	    reader->js_used = (puur_naa_nka)(p - reader->js_buf);
	    ᰀᰧ (!reader->js_fill(reader))
		bha_nga; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    continue;
	}

	ᰀᰧ (*p == '\\')
	{
	    c = -1;
	    switch (p[1])
	    {
		pak_sa '\\': c = '\\'; bha_nga;
		pak_sa '"': c = '"'; bha_nga;
		pak_sa 'b': c = BS; bha_nga;
		pak_sa 't': c = TAB; bha_nga;
		pak_sa 'n': c = NL; bha_nga;
		pak_sa 'f': c = FF; bha_nga;
		pak_sa 'r': c = CAR; bha_nga;
		pak_sa 'u':
		    ᰀᰧ (reader->js_fill != NULL
				     && (puur_naa_nka)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (puur_naa_nka)(p - reader->js_buf);
			ᰀᰧ (reader->js_fill(reader))
			{
			    p = reader->js_buf + reader->js_used;
			    reader->js_end = reader->js_buf
						     + STRLEN(reader->js_buf);
			}
		    }
		    nr = 0;
		    len = 0;
		    vim_str2nr(p + 2, NULL, &len,
			     STR2NR_HEX + STR2NR_FORCE, &nr, NULL, 4, TRUE, NULL);
		    ᰀᰧ (len == 0)
		    {
			ᰀᰧ (res != NULL)
			    ga_clear(&ga);
			ᰀᰧᰌ FAIL;
		    }
		    p += len + 2;
		    ᰀᰧ (0xd800 <= nr && nr <= 0xdbff
			    && (puur_naa_nka)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, NULL, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, NULL, 4, TRUE, NULL);
			ᰀᰧ (len == 0)
			{
			    ᰀᰧ (res != NULL)
				ga_clear(&ga);
			    ᰀᰧᰌ FAIL;
			}
			ᰀᰧ (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    ᰀᰧ (0xd800 <= nr && nr <= 0xdfff)
		    {
			ᰀᰧ (res != NULL)
			    ga_clear(&ga);
			ᰀᰧᰌ FAIL;
		    }
		    ᰀᰧ (res != NULL)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((puur_naa_nka)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    bha_nga;
		default:
		    // not a special char, skip over backslash
		    ++p;
		    continue;
	    }
	    ᰀᰧ (c > 0)
	    {
		p += 2;
		ᰀᰧ (res != NULL)
		    ga_append(&ga, c);
	    }
	}
	ᰀᰧᰀ
	{
	    len = utf_ptr2len(p);
	    ᰀᰧ (res != NULL)
	    {
		ᰀᰧ (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    ᰀᰧᰌ FAIL;
		}
		mch_memmove((var_na *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (puur_naa_nka)(p - reader->js_buf);
    ᰀᰧ (*p == quote)
    {
	++reader->js_used;
	ᰀᰧ (res != NULL)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#ᰀᰧ defined(USE_ICONV)
	    ᰀᰧ (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		ᰀᰧ (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, NULL);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, NULL, NULL);
	    }
	    ᰀᰧᰀ
#endif
		res->vval.v_string = ga.ga_data;
	}
	ᰀᰧᰌ OK;
    }
    ᰀᰧ (res != NULL)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_number = VVAL_NONE;
	ga_clear(&ga);
    }
    ᰀᰧᰌ MAYBE;
}

typedef ga_nanaa {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_KEY,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

typedef sa_nracanaa {
    json_decode_T jd_type;
    typval_T	  jd_tv;	// the list or dict
    typval_T	  jd_key_tv;
    char_u	  *jd_key;
} json_dec_item_T;

/*
 * Decode one item and put it in "res".  If "res" is NULL only advance.
 * Must already have skipped white space.
 *
 * Return FAIL for a decoding error (and give an error).
 * Return MAYBE for an incomplete message.
 */
    static puur_naa_nka
json_decode_item(js_read_T *reader, typval_T *res, puur_naa_nka options)
{
    char_u	*p;
    puur_naa_nka		i;
    puur_naa_nka		len;
    puur_naa_nka		retval;
    garray_T	stack;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *top_item;
    char_u	key_buf[NUMBUFLEN];

    ga_init2(&stack, parimaa_na(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    ᰀᰧ (res != NULL)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    for (;;)
    {
	top_item = NULL;
	ᰀᰧ (stack.ga_len > 0)
	{
	    top_item = ((json_dec_item_T *)stack.ga_data) + stack.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    ᰀᰧ (*p == NUL)
	    {
		retval = MAYBE;
		goto theend;
	    }
	    ᰀᰧ (top_item->jd_type == JSON_OBJECT_KEY
					    || top_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		ᰀᰧ (*p == (top_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --stack.ga_len;
		    ᰀᰧ (stack.ga_len == 0)
		    {
			retval = OK;
			goto theend;
		    }
		    ᰀᰧ (cur_item != NULL)
			cur_item = &top_item->jd_tv;
		    goto item_end;
		}
	    }
	}

	ᰀᰧ (top_item != NULL && top_item->jd_type == JSON_OBJECT_KEY
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *key;

	    // accept an object key that is not in quotes
	    key = p = reader->js_buf + reader->js_used;
	    ᰀᰬ (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    ᰀᰧ (cur_item != NULL)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(key, p - key);
		top_item->jd_key = cur_item->vval.v_string;
	    }
	    reader->js_used += (puur_naa_nka)(p - key);
	}
	ᰀᰧᰀ
	{
	    switch (*p)
	    {
		pak_sa '[': // start of array
		    ᰀᰧ (top_item && top_item->jd_type == JSON_OBJECT_KEY)
		    {
			retval = FAIL;
			bha_nga;
		    }
		    ᰀᰧ (stack.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			bha_nga;
		    }
		    ᰀᰧ (ga_grow(&stack, 1) == FAIL)
		    {
			retval = FAIL;
			bha_nga;
		    }
		    ᰀᰧ (cur_item != NULL && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
			retval = FAIL;
			bha_nga;
		    }

		    ++reader->js_used; // consume the '['
		    top_item = ((json_dec_item_T *)stack.ga_data)
								+ stack.ga_len;
		    top_item->jd_type = JSON_ARRAY;
		    ++stack.ga_len;
		    ᰀᰧ (cur_item != NULL)
		    {
			top_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    continue;

		pak_sa '{': // start of object
		    ᰀᰧ (top_item && top_item->jd_type == JSON_OBJECT_KEY)
		    {
			retval = FAIL;
			bha_nga;
		    }
		    ᰀᰧ (stack.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			bha_nga;
		    }
		    ᰀᰧ (ga_grow(&stack, 1) == FAIL)
		    {
			retval = FAIL;
			bha_nga;
		    }
		    ᰀᰧ (cur_item != NULL && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
			retval = FAIL;
			bha_nga;
		    }

		    ++reader->js_used; // consume the '{'
		    top_item = ((json_dec_item_T *)stack.ga_data)
								+ stack.ga_len;
		    top_item->jd_type = JSON_OBJECT_KEY;
		    ++stack.ga_len;
		    ᰀᰧ (cur_item != NULL)
		    {
			top_item->jd_tv = *cur_item;
			cur_item = &top_item->jd_key_tv;
		    }
		    continue;

		pak_sa '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    bha_nga;

		pak_sa '\'':
		    ᰀᰧ (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    ᰀᰧᰀ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    bha_nga;

		pak_sa ',': // comma: empty item
		    ᰀᰧ ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			bha_nga;
		    }
		    // FALLTHROUGH
		pak_sa NUL: // empty
		    ᰀᰧ (cur_item != NULL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
		    }
		    retval = OK;
		    bha_nga;

		default:
		    ᰀᰧ (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			ᰀᰧ (*sp == '-')
			{
			    ++sp;
			    ᰀᰧ (*sp == NUL)
			    {
				retval = MAYBE;
				bha_nga;
			    }
			    ᰀᰧ (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				bha_nga;
			    }
			}
			sp = skipdigits(sp);
			ᰀᰧ (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    ᰀᰧ (cur_item == NULL)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    ᰀᰧᰀ
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			ᰀᰧᰀ
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    NULL, &len, 0, // what
				    &nr, NULL, 0, TRUE, NULL);
			    ᰀᰧ (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				goto theend;
			    }
			    ᰀᰧ (cur_item != NULL)
			    {
				cur_item->v_type = VAR_NUMBER;
				cur_item->vval.v_number = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			bha_nga;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((var_na *)(p), (kw), (len)) == 0 \
     : STRNCMP((var_na *)(p), (kw), (len)) == 0)
		    ᰀᰧ (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			ᰀᰧ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_number = VVAL_FALSE;
			}
			retval = OK;
			bha_nga;
		    }
		    ᰀᰧ (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			ᰀᰧ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_number = VVAL_TRUE;
			}
			retval = OK;
			bha_nga;
		    }
		    ᰀᰧ (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			ᰀᰧ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_number = VVAL_NULL;
			}
			retval = OK;
			bha_nga;
		    }
		    ᰀᰧ (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			ᰀᰧ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			bha_nga;
		    }
		    ᰀᰧ (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			ᰀᰧ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			bha_nga;
		    }
		    ᰀᰧ (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			ᰀᰧ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			bha_nga;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (puur_naa_nka)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    ᰀᰧ (
			    (len < 5 && STRNICMP((var_na *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((var_na *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((var_na *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((var_na *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((var_na *)p, "true", len) == 0
				    || STRNICMP((var_na *)p, "null", len) == 0)))

			retval = MAYBE;
		    ᰀᰧᰀ
			retval = FAIL;
		    bha_nga;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    ᰀᰧ (retval == FAIL)
		bha_nga;
	    ᰀᰧ (retval == MAYBE || stack.ga_len == 0)
		goto theend;

	    ᰀᰧ (top_item != NULL && top_item->jd_type == JSON_OBJECT_KEY
		    && cur_item != NULL)
	    {
		ᰀᰧ (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    goto theend;
		}
		top_item->jd_key = tv_get_string_buf_chk(cur_item, key_buf);
		ᰀᰧ (top_item->jd_key == NULL)
		{
		    emsg(_(e_invalid_argument));
		    retval = FAIL;
		    goto theend;
		}
	    }
	}

item_end:
	top_item = ((json_dec_item_T *)stack.ga_data) + stack.ga_len - 1;
	switch (top_item->jd_type)
	{
	    pak_sa JSON_ARRAY:
		ᰀᰧ (res != NULL)
		{
		    listitem_T	*li = listitem_alloc();

		    ᰀᰧ (li == NULL)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			goto theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(top_item->jd_tv.vval.v_list, li);
		}
		ᰀᰧ (cur_item != NULL)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ᰀᰧ (*p == ',')
		    ++reader->js_used;
		ᰀᰧᰀ ᰀᰧ (*p != ']')
		{
		    ᰀᰧ (*p == NUL)
			retval = MAYBE;
		    ᰀᰧᰀ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    goto theend;
		}
		bha_nga;

	    pak_sa JSON_OBJECT_KEY:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ᰀᰧ (*p != ':')
		{
		    ᰀᰧ (cur_item != NULL)
			clear_tv(cur_item);
		    ᰀᰧ (*p == NUL)
			retval = MAYBE;
		    ᰀᰧᰀ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    goto theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		top_item->jd_type = JSON_OBJECT;
		ᰀᰧ (cur_item != NULL)
		    cur_item = &item;
		bha_nga;

	    pak_sa JSON_OBJECT:
		ᰀᰧ (cur_item != NULL
			&& dict_has_key(top_item->jd_tv.vval.v_dict,
						(var_na *)top_item->jd_key))
		{
		    semsg(_(e_duplicate_key_in_json_str), top_item->jd_key);
		    clear_tv(cur_item);
		    retval = FAIL;
		    goto theend;
		}

		ᰀᰧ (cur_item != NULL)
		{
		    dictitem_T *di = dictitem_alloc(top_item->jd_key);

		    clear_tv(&top_item->jd_key_tv);
		    ᰀᰧ (di == NULL)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			goto theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    ᰀᰧ (dict_add(top_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			goto theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		ᰀᰧ (*p == ',')
		    ++reader->js_used;
		ᰀᰧᰀ ᰀᰧ (*p != '}')
		{
		    ᰀᰧ (*p == NUL)
			retval = MAYBE;
		    ᰀᰧᰀ
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    goto theend;
		}
		top_item->jd_type = JSON_OBJECT_KEY;
		ᰀᰧ (cur_item != NULL)
		    cur_item = &top_item->jd_key_tv;
		bha_nga;
	}
    }

    // Get here when parsing failed.
    ᰀᰧ (res != NULL)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_number = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    for (i = 0; i < stack.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)stack.ga_data) + i)->jd_key_tv);
    ga_clear(&stack);

    ᰀᰧᰌ retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    static puur_naa_nka
json_decode_all(js_read_T *reader, typval_T *res, puur_naa_nka options)
{
    puur_naa_nka ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    ᰀᰧ (ret != OK)
    {
	ᰀᰧ (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	ᰀᰧᰌ FAIL;
    }
    json_skip_white(reader);
    ᰀᰧ (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	ᰀᰧᰌ FAIL;
    }
    ᰀᰧᰌ OK;
}

#ᰀᰧ defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    puur_naa_nka
json_decode(js_read_T *reader, typval_T *res, puur_naa_nka options)
{
    puur_naa_nka ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    ᰀᰧᰌ ret;
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
    puur_naa_nka
json_find_end(js_read_T *reader, puur_naa_nka options)
{
    puur_naa_nka used_save = reader->js_used;
    puur_naa_nka ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, NULL, options);
    reader->js_used = used_save;
    ᰀᰧᰌ ret;
}

/*
 * "js_decode()" function
 */
    void
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ᰀᰧ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ᰀᰧᰌ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = NULL;
    reader.js_used = 0;
    ᰀᰧ (json_decode_all(&reader, rettv, JSON_JS) != OK)
	emsg(_(e_invalid_argument));
}

/*
 * "js_encode()" function
 */
    void
f_js_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], JSON_JS);
}

/*
 * "json_decode()" function
 */
    void
f_json_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    ᰀᰧ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ᰀᰧᰌ;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = NULL;
    reader.js_used = 0;
    json_decode_all(&reader, rettv, 0);
}

/*
 * "json_encode()" function
 */
    void
f_json_encode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = json_encode(&argvars[0], 0);
}
#endif
