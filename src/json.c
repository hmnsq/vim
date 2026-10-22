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

#အကယ်၍ defined(FEAT_EVAL)

static ကိန်းပြည့် json_encode_item(garray_T *gap, typval_T *val, ကိန်းပြည့် copyID, ကိန်းပြည့် options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    static ကိန်းပြည့်
json_encode_gap(garray_T *gap, typval_T *val, ကိန်းပြည့် options)
{
    အကယ်၍ (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	ပြန်ပေး FAIL;
    }
    ပြန်ပေး OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, ကိန်းပြည့် options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    အကယ်၍ (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    ပြန်ပေး ga.ga_data;
}

#အကယ်၍ defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_expr(ကိန်းပြည့် nr, typval_T *val, ကိန်းပြည့် options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_NUMBER;
    nrtv.vval.v_number = nr;
    အကယ်၍ (rettv_list_alloc(&listtv) == FAIL)
	ပြန်ပေး NULL;
    အကယ်၍ (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	ပြန်ပေး NULL;
    }

    ga_init2(&ga, 1, 4000);
    အကယ်၍ (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    ပြန်ပေး ga.ga_data;
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
    အကယ်၍ (json_encode_gap(&ga, val, 0) == FAIL)
	ပြန်ပေး NULL;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((var_na *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    ပြန်ပေး lspga.ga_data;
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
#အကယ်၍ defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = NULL;
#endif
    ကိန်းပြည့်		c;

    အကယ်၍ (res == NULL)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	ပြန်ပေး;
    }

#အကယ်၍ defined(USE_ICONV)
    အကယ်၍ (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	အကယ်၍ (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, NULL);
	convert_setup(&conv, NULL, NULL);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (ကိန်းပြည့်)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    နေစဉ် ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	အကယ်၍ (c < 0x80)
	{
	    အကယ်၍ (!ascii_needs_escape[c])
	    {
		res += 1;
		continue;
	    }

	    အကယ်၍ (res != from)
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
	မဟုတ်ပါက
	{
	    ကိန်းပြည့်	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    အကယ်၍ (l > 1)
	    {
		res += l;
		continue;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    အကယ်၍ (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    အကယ်၍ (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#အကယ်၍ defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    static ကိန်းပြည့်
is_simple_key(char_u *key)
{
    char_u *p;

    အကယ်၍ (!ASCII_ISALPHA(*key))
	ပြန်ပေး FALSE;
    အတွက် (p = key + 1; *p != NUL; ++p)
	အကယ်၍ (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    ပြန်ပေး FALSE;
    ပြန်ပေး TRUE;
}

typedef ga_nanaa {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

typedef sa_nracanaa {
    json_enc_type_T je_type;
    ကိန်းပြည့်		    je_options;	    // options when this container was entered
    sa_ngha {
	sa_nracanaa {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	sa_nracanaa {
	    tuple_T	*tuple;
	    ကိန်းပြည့်		idx;	    // current index
	    ကိန်းပြည့်		len;
	} t;
	sa_nracanaa {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    ကိန်းပြည့်		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    static ကိန်းပြည့်
json_encode_item(garray_T *gap, typval_T *val, ကိန်းပြည့် copyID, ကိန်းပြည့် options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    ကိန်းပြည့်			i;
    garray_T		stack;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&stack, parimaa_na(json_enc_frame_T), 100);
    cur_val = val;

    အတွက် (;;)
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
		pak_sa VVAL_NONE: အကယ်၍ ((options & JSON_JS) != 0
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
	    အကယ်၍ (b == NULL || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    မဟုတ်ပါက
	    {
		ကိန်းပြည့်	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		အကယ်၍ (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    goto theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		အတွက် (i = 0; i < blen; i++)
		{
		    ကိန်းပြည့်	    byte = src[i];

		    အကယ်၍ (i > 0)
			*dst++ = ',';
		    အကယ်၍ (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    မဟုတ်ပါက အကယ်၍ (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    မဟုတ်ပါက
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (ကိန်းပြည့်)(dst - (char_u *)gap->ga_data);
	    }
	    bha_nga;

	pak_sa VAR_LIST:
	    l = cur_val->vval.v_list;
	    အကယ်၍ (l == NULL)
		GA_CONCAT_LITERAL(gap, "[]");
	    မဟုတ်ပါက
	    {
		အကယ်၍ (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		မဟုတ်ပါက
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    အကယ်၍ (l->lv_first != NULL)
		    {
			အကယ်၍ (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			အကယ်၍ (ga_grow(&stack, 1) == FAIL)
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
	    အကယ်၍ (tuple == NULL)
		GA_CONCAT_LITERAL(gap, "[]");
	    မဟုတ်ပါက
	    {
		အကယ်၍ (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		မဟုတ်ပါက
		{
		    ကိန်းပြည့်		len = TUPLE_LEN(tuple);

		    အကယ်၍ (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			အကယ်၍ (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			အကယ်၍ (ga_grow(&stack, 1) == FAIL)
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
		    မဟုတ်ပါက
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    bha_nga;

	pak_sa VAR_DICT:
	    d = cur_val->vval.v_dict;
	    အကယ်၍ (d == NULL)
		GA_CONCAT_LITERAL(gap, "{}");
	    မဟုတ်ပါက
	    {
		အကယ်၍ (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		မဟုတ်ပါက
		{
		    ကိန်းပြည့်		todo = (ကိန်းပြည့်)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    အကယ်၍ (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			အတွက် (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			အကယ်၍ ((options & JSON_JS)
						 && is_simple_key(hi->hi_key))
			    ga_concat(gap, hi->hi_key);
			မဟုတ်ပါက
			    write_string(gap, hi->hi_key);
			ga_append(gap, ':');

			အကယ်၍ (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			အကယ်၍ (ga_grow(&stack, 1) == FAIL)
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
		    မဟုတ်ပါက
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    bha_nga;

	pak_sa VAR_FLOAT:
#အကယ်၍ defined(HAVE_MATH_H)
	    အကယ်၍ (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    မဟုတ်ပါက အကယ်၍ (isinf(cur_val->vval.v_float))
	    {
		အကယ်၍ (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		မဟုတ်ပါက
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    မဟုတ်ပါက
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
	အတွက် (;;)
	{
	    ကိန်းပြည့်		advance = FALSE;

	    အကယ်၍ (stack.ga_len == 0)
	    {
		ga_clear(&stack);
		ပြန်ပေး OK;
	    }

	    frame = ((json_enc_frame_T *)stack.ga_data)
							+ stack.ga_len - 1;
	    switch (frame->je_type)
	    {
		pak_sa ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    အကယ်၍ ((frame->je_options & JSON_JS)
			    && li->li_next == NULL
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_number == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    အကယ်၍ (li != NULL && !got_int)
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
		    ကိန်းပြည့်	    idx = frame->je_u.t.idx;
		    ကိန်းပြည့်	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    အကယ်၍ ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			အကယ်၍ (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_number == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    အကယ်၍ (idx < len && !got_int)
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
		    ကိန်းပြည့်		todo = frame->je_u.d.todo;

		    အကယ်၍ (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			အတွက် (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			အကယ်၍ ((frame->je_options & JSON_JS)
				&& is_simple_key(hi->hi_key))
			    ga_concat(gap, hi->hi_key);
			မဟုတ်ပါက
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
	    အကယ်၍ (advance)
		bha_nga;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    အတွက် (i = 0; i < stack.ga_len; i++)
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
    ပြန်ပေး FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    static void
fill_numbuflen(js_read_T *reader)
{
    အကယ်၍ (reader->js_fill != NULL && (ကိန်းပြည့်)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	အကယ်၍ (reader->js_fill(reader))
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
    ကိန်းပြည့် c;

    အတွက် (;;)
    {
	c = reader->js_buf[reader->js_used];
	အကယ်၍ (reader->js_fill != NULL && c == NUL)
	{
	    အကယ်၍ (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		continue;
	    }
	}
	အကယ်၍ (c == NUL || c > ' ')
	    bha_nga;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    static ကိန်းပြည့်
json_decode_string(js_read_T *reader, typval_T *res, ကိန်းပြည့် quote)
{
    garray_T    ga;
    ကိန်းပြည့်		len;
    char_u	*p;
    ကိန်းပြည့်		c;
    varnumber_T	nr;

    အကယ်၍ (res != NULL)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    နေစဉ် (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	အကယ်၍ (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    အကယ်၍ (reader->js_fill == NULL)
		bha_nga;
	    len = (ကိန်းပြည့်)(reader->js_end - p);
	    reader->js_used = (ကိန်းပြည့်)(p - reader->js_buf);
	    အကယ်၍ (!reader->js_fill(reader))
		bha_nga; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    continue;
	}

	အကယ်၍ (*p == '\\')
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
		    အကယ်၍ (reader->js_fill != NULL
				     && (ကိန်းပြည့်)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (ကိန်းပြည့်)(p - reader->js_buf);
			အကယ်၍ (reader->js_fill(reader))
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
		    အကယ်၍ (len == 0)
		    {
			အကယ်၍ (res != NULL)
			    ga_clear(&ga);
			ပြန်ပေး FAIL;
		    }
		    p += len + 2;
		    အကယ်၍ (0xd800 <= nr && nr <= 0xdbff
			    && (ကိန်းပြည့်)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, NULL, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, NULL, 4, TRUE, NULL);
			အကယ်၍ (len == 0)
			{
			    အကယ်၍ (res != NULL)
				ga_clear(&ga);
			    ပြန်ပေး FAIL;
			}
			အကယ်၍ (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    အကယ်၍ (0xd800 <= nr && nr <= 0xdfff)
		    {
			အကယ်၍ (res != NULL)
			    ga_clear(&ga);
			ပြန်ပေး FAIL;
		    }
		    အကယ်၍ (res != NULL)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((ကိန်းပြည့်)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    bha_nga;
		default:
		    // not a special char, skip over backslash
		    ++p;
		    continue;
	    }
	    အကယ်၍ (c > 0)
	    {
		p += 2;
		အကယ်၍ (res != NULL)
		    ga_append(&ga, c);
	    }
	}
	မဟုတ်ပါက
	{
	    len = utf_ptr2len(p);
	    အကယ်၍ (res != NULL)
	    {
		အကယ်၍ (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    ပြန်ပေး FAIL;
		}
		mch_memmove((var_na *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (ကိန်းပြည့်)(p - reader->js_buf);
    အကယ်၍ (*p == quote)
    {
	++reader->js_used;
	အကယ်၍ (res != NULL)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#အကယ်၍ defined(USE_ICONV)
	    အကယ်၍ (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		အကယ်၍ (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, NULL);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, NULL, NULL);
	    }
	    မဟုတ်ပါက
#endif
		res->vval.v_string = ga.ga_data;
	}
	ပြန်ပေး OK;
    }
    အကယ်၍ (res != NULL)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_number = VVAL_NONE;
	ga_clear(&ga);
    }
    ပြန်ပေး MAYBE;
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
    static ကိန်းပြည့်
json_decode_item(js_read_T *reader, typval_T *res, ကိန်းပြည့် options)
{
    char_u	*p;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		len;
    ကိန်းပြည့်		retval;
    garray_T	stack;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *top_item;
    char_u	key_buf[NUMBUFLEN];

    ga_init2(&stack, parimaa_na(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    အကယ်၍ (res != NULL)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    အတွက် (;;)
    {
	top_item = NULL;
	အကယ်၍ (stack.ga_len > 0)
	{
	    top_item = ((json_dec_item_T *)stack.ga_data) + stack.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    အကယ်၍ (*p == NUL)
	    {
		retval = MAYBE;
		goto theend;
	    }
	    အကယ်၍ (top_item->jd_type == JSON_OBJECT_KEY
					    || top_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		အကယ်၍ (*p == (top_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --stack.ga_len;
		    အကယ်၍ (stack.ga_len == 0)
		    {
			retval = OK;
			goto theend;
		    }
		    အကယ်၍ (cur_item != NULL)
			cur_item = &top_item->jd_tv;
		    goto item_end;
		}
	    }
	}

	အကယ်၍ (top_item != NULL && top_item->jd_type == JSON_OBJECT_KEY
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *key;

	    // accept an object key that is not in quotes
	    key = p = reader->js_buf + reader->js_used;
	    နေစဉ် (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    အကယ်၍ (cur_item != NULL)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(key, p - key);
		top_item->jd_key = cur_item->vval.v_string;
	    }
	    reader->js_used += (ကိန်းပြည့်)(p - key);
	}
	မဟုတ်ပါက
	{
	    switch (*p)
	    {
		pak_sa '[': // start of array
		    အကယ်၍ (top_item && top_item->jd_type == JSON_OBJECT_KEY)
		    {
			retval = FAIL;
			bha_nga;
		    }
		    အကယ်၍ (stack.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			bha_nga;
		    }
		    အကယ်၍ (ga_grow(&stack, 1) == FAIL)
		    {
			retval = FAIL;
			bha_nga;
		    }
		    အကယ်၍ (cur_item != NULL && rettv_list_alloc(cur_item) == FAIL)
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
		    အကယ်၍ (cur_item != NULL)
		    {
			top_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    continue;

		pak_sa '{': // start of object
		    အကယ်၍ (top_item && top_item->jd_type == JSON_OBJECT_KEY)
		    {
			retval = FAIL;
			bha_nga;
		    }
		    အကယ်၍ (stack.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			bha_nga;
		    }
		    အကယ်၍ (ga_grow(&stack, 1) == FAIL)
		    {
			retval = FAIL;
			bha_nga;
		    }
		    အကယ်၍ (cur_item != NULL && rettv_dict_alloc(cur_item) == FAIL)
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
		    အကယ်၍ (cur_item != NULL)
		    {
			top_item->jd_tv = *cur_item;
			cur_item = &top_item->jd_key_tv;
		    }
		    continue;

		pak_sa '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    bha_nga;

		pak_sa '\'':
		    အကယ်၍ (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    မဟုတ်ပါက
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    bha_nga;

		pak_sa ',': // comma: empty item
		    အကယ်၍ ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			bha_nga;
		    }
		    // FALLTHROUGH
		pak_sa NUL: // empty
		    အကယ်၍ (cur_item != NULL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
		    }
		    retval = OK;
		    bha_nga;

		default:
		    အကယ်၍ (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			အကယ်၍ (*sp == '-')
			{
			    ++sp;
			    အကယ်၍ (*sp == NUL)
			    {
				retval = MAYBE;
				bha_nga;
			    }
			    အကယ်၍ (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				bha_nga;
			    }
			}
			sp = skipdigits(sp);
			အကယ်၍ (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    အကယ်၍ (cur_item == NULL)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    မဟုတ်ပါက
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			မဟုတ်ပါက
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    NULL, &len, 0, // what
				    &nr, NULL, 0, TRUE, NULL);
			    အကယ်၍ (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				goto theend;
			    }
			    အကယ်၍ (cur_item != NULL)
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
		    အကယ်၍ (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			အကယ်၍ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_number = VVAL_FALSE;
			}
			retval = OK;
			bha_nga;
		    }
		    အကယ်၍ (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			အကယ်၍ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_number = VVAL_TRUE;
			}
			retval = OK;
			bha_nga;
		    }
		    အကယ်၍ (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			အကယ်၍ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_number = VVAL_NULL;
			}
			retval = OK;
			bha_nga;
		    }
		    အကယ်၍ (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			အကယ်၍ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			bha_nga;
		    }
		    အကယ်၍ (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			အကယ်၍ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			bha_nga;
		    }
		    အကယ်၍ (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			အကယ်၍ (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			bha_nga;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (ကိန်းပြည့်)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    အကယ်၍ (
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
		    မဟုတ်ပါက
			retval = FAIL;
		    bha_nga;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    အကယ်၍ (retval == FAIL)
		bha_nga;
	    အကယ်၍ (retval == MAYBE || stack.ga_len == 0)
		goto theend;

	    အကယ်၍ (top_item != NULL && top_item->jd_type == JSON_OBJECT_KEY
		    && cur_item != NULL)
	    {
		အကယ်၍ (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    goto theend;
		}
		top_item->jd_key = tv_get_string_buf_chk(cur_item, key_buf);
		အကယ်၍ (top_item->jd_key == NULL)
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
		အကယ်၍ (res != NULL)
		{
		    listitem_T	*li = listitem_alloc();

		    အကယ်၍ (li == NULL)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			goto theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(top_item->jd_tv.vval.v_list, li);
		}
		အကယ်၍ (cur_item != NULL)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		အကယ်၍ (*p == ',')
		    ++reader->js_used;
		မဟုတ်ပါက အကယ်၍ (*p != ']')
		{
		    အကယ်၍ (*p == NUL)
			retval = MAYBE;
		    မဟုတ်ပါက
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
		အကယ်၍ (*p != ':')
		{
		    အကယ်၍ (cur_item != NULL)
			clear_tv(cur_item);
		    အကယ်၍ (*p == NUL)
			retval = MAYBE;
		    မဟုတ်ပါက
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    goto theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		top_item->jd_type = JSON_OBJECT;
		အကယ်၍ (cur_item != NULL)
		    cur_item = &item;
		bha_nga;

	    pak_sa JSON_OBJECT:
		အကယ်၍ (cur_item != NULL
			&& dict_has_key(top_item->jd_tv.vval.v_dict,
						(var_na *)top_item->jd_key))
		{
		    semsg(_(e_duplicate_key_in_json_str), top_item->jd_key);
		    clear_tv(cur_item);
		    retval = FAIL;
		    goto theend;
		}

		အကယ်၍ (cur_item != NULL)
		{
		    dictitem_T *di = dictitem_alloc(top_item->jd_key);

		    clear_tv(&top_item->jd_key_tv);
		    အကယ်၍ (di == NULL)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			goto theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    အကယ်၍ (dict_add(top_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			goto theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		အကယ်၍ (*p == ',')
		    ++reader->js_used;
		မဟုတ်ပါက အကယ်၍ (*p != '}')
		{
		    အကယ်၍ (*p == NUL)
			retval = MAYBE;
		    မဟုတ်ပါက
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    goto theend;
		}
		top_item->jd_type = JSON_OBJECT_KEY;
		အကယ်၍ (cur_item != NULL)
		    cur_item = &top_item->jd_key_tv;
		bha_nga;
	}
    }

    // Get here when parsing failed.
    အကယ်၍ (res != NULL)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_number = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    အတွက် (i = 0; i < stack.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)stack.ga_data) + i)->jd_key_tv);
    ga_clear(&stack);

    ပြန်ပေး retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    static ကိန်းပြည့်
json_decode_all(js_read_T *reader, typval_T *res, ကိန်းပြည့် options)
{
    ကိန်းပြည့် ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    အကယ်၍ (ret != OK)
    {
	အကယ်၍ (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	ပြန်ပေး FAIL;
    }
    json_skip_white(reader);
    အကယ်၍ (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	ပြန်ပေး FAIL;
    }
    ပြန်ပေး OK;
}

#အကယ်၍ defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    ကိန်းပြည့်
json_decode(js_read_T *reader, typval_T *res, ကိန်းပြည့် options)
{
    ကိန်းပြည့် ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    ပြန်ပေး ret;
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
    ကိန်းပြည့်
json_find_end(js_read_T *reader, ကိန်းပြည့် options)
{
    ကိန်းပြည့် used_save = reader->js_used;
    ကိန်းပြည့် ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, NULL, options);
    reader->js_used = used_save;
    ပြန်ပေး ret;
}

/*
 * "js_decode()" function
 */
    void
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = NULL;
    reader.js_used = 0;
    အကယ်၍ (json_decode_all(&reader, rettv, JSON_JS) != OK)
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

    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

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
