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

#nếu defined(FEAT_EVAL)

static số_nguyên json_encode_item(garray_T *gap, typval_T *val, số_nguyên copyID, số_nguyên options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    static số_nguyên
json_encode_gap(garray_T *gap, typval_T *val, số_nguyên options)
{
    nếu (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, số_nguyên options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    nếu (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    trả_về ga.ga_data;
}

#nếu defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_expr(số_nguyên nr, typval_T *val, số_nguyên options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_NUMBER;
    nrtv.vval.v_number = nr;
    nếu (rettv_list_alloc(&listtv) == FAIL)
	trả_về NULL;
    nếu (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	trả_về NULL;
    }

    ga_init2(&ga, 1, 4000);
    nếu (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    trả_về ga.ga_data;
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
    nếu (json_encode_gap(&ga, val, 0) == FAIL)
	trả_về NULL;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((char *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    trả_về lspga.ga_data;
}
#endif

/*
 * Lookup table to quickly know if the given ASCII character must be escaped.
 */
static const char ascii_needs_escape[128] = {
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
#nếu defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = NULL;
#endif
    số_nguyên		c;

    nếu (res == NULL)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	trả_về;
    }

#nếu defined(USE_ICONV)
    nếu (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	nếu (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, NULL);
	convert_setup(&conv, NULL, NULL);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (số_nguyên)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    trong_khi ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	nếu (c < 0x80)
	{
	    nếu (!ascii_needs_escape[c])
	    {
		res += 1;
		continue;
	    }

	    nếu (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    switch (c)
	    {
		case 0x08:
		    GA_CONCAT_LITERAL(gap, "\\b"); break;
		case 0x09:
		    GA_CONCAT_LITERAL(gap, "\\t"); break;
		case 0x0a:
		    GA_CONCAT_LITERAL(gap, "\\n"); break;
		case 0x0c:
		    GA_CONCAT_LITERAL(gap, "\\f"); break;
		case 0x0d:
		    GA_CONCAT_LITERAL(gap, "\\r"); break;
		case 0x22: // "
		    GA_CONCAT_LITERAL(gap, "\\\""); break;
		case 0x5c: // backslash
		    GA_CONCAT_LITERAL(gap, "\\\\"); break;
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
	không_thì
	{
	    số_nguyên	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    nếu (l > 1)
	    {
		res += l;
		continue;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    nếu (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    nếu (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#nếu defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    static số_nguyên
is_simple_key(char_u *key)
{
    char_u *p;

    nếu (!ASCII_ISALPHA(*key))
	trả_về FALSE;
    cho (p = key + 1; *p != NUL; ++p)
	nếu (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    trả_về FALSE;
    trả_về TRUE;
}

typedef enum {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

typedef struct {
    json_enc_type_T je_type;
    số_nguyên		    je_options;	    // options when this container was entered
    union {
	struct {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	struct {
	    tuple_T	*tuple;
	    số_nguyên		idx;	    // current index
	    số_nguyên		len;
	} t;
	struct {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    số_nguyên		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    static số_nguyên
json_encode_item(garray_T *gap, typval_T *val, số_nguyên copyID, số_nguyên options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    số_nguyên			i;
    garray_T		stack;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&stack, sizeof(json_enc_frame_T), 100);
    cur_val = val;

    cho (;;)
    {
    switch (cur_val->v_type)
    {
	case VAR_BOOL:
	    switch ((long)cur_val->vval.v_number)
	    {
		case VVAL_FALSE: GA_CONCAT_LITERAL(gap, "false"); break;
		case VVAL_TRUE: GA_CONCAT_LITERAL(gap, "true"); break;
	    }
	    break;

	case VAR_SPECIAL:
	    switch ((long)cur_val->vval.v_number)
	    {
		case VVAL_NONE: nếu ((options & JSON_JS) != 0
					     && (options & JSON_NO_NONE) == 0)
				    // empty item
				    break;
				// FALLTHROUGH
		case VVAL_NULL: GA_CONCAT_LITERAL(gap, "null"); break;
	    }
	    break;

	case VAR_NUMBER:
	    {
		size_t  numbuflen;

		numbuflen = vim_snprintf_safelen((char *)numbuf, sizeof(numbuf),
		    "%lld", (varnumber_T)cur_val->vval.v_number);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    break;

	case VAR_STRING:
	    res = cur_val->vval.v_string;
	    write_string(gap, res);
	    break;

	case VAR_FUNC:
	case VAR_PARTIAL:
	case VAR_JOB:
	case VAR_CHANNEL:
	case VAR_INSTR:
	case VAR_CLASS:
	case VAR_OBJECT:
	case VAR_TYPEALIAS:
	    semsg(_(e_cannot_json_encode_str), vartype_name(cur_val->v_type));
	    goto theend;

	case VAR_BLOB:
	    b = cur_val->vval.v_blob;
	    nếu (b == NULL || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    không_thì
	    {
		số_nguyên	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		nếu (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    goto theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		cho (i = 0; i < blen; i++)
		{
		    số_nguyên	    byte = src[i];

		    nếu (i > 0)
			*dst++ = ',';
		    nếu (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    không_thì nếu (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    không_thì
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (số_nguyên)(dst - (char_u *)gap->ga_data);
	    }
	    break;

	case VAR_LIST:
	    l = cur_val->vval.v_list;
	    nếu (l == NULL)
		GA_CONCAT_LITERAL(gap, "[]");
	    không_thì
	    {
		nếu (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		không_thì
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    nếu (l->lv_first != NULL)
		    {
			nếu (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			nếu (ga_grow(&stack, 1) == FAIL)
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
	    break;

	case VAR_TUPLE:
	    tuple = cur_val->vval.v_tuple;
	    nếu (tuple == NULL)
		GA_CONCAT_LITERAL(gap, "[]");
	    không_thì
	    {
		nếu (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		không_thì
		{
		    số_nguyên		len = TUPLE_LEN(tuple);

		    nếu (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			nếu (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			nếu (ga_grow(&stack, 1) == FAIL)
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
		    không_thì
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    break;

	case VAR_DICT:
	    d = cur_val->vval.v_dict;
	    nếu (d == NULL)
		GA_CONCAT_LITERAL(gap, "{}");
	    không_thì
	    {
		nếu (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		không_thì
		{
		    số_nguyên		todo = (số_nguyên)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    nếu (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			cho (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			nếu ((options & JSON_JS)
						 && is_simple_key(hi->hi_key))
			    ga_concat(gap, hi->hi_key);
			không_thì
			    write_string(gap, hi->hi_key);
			ga_append(gap, ':');

			nếu (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			nếu (ga_grow(&stack, 1) == FAIL)
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
		    không_thì
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    break;

	case VAR_FLOAT:
#nếu defined(HAVE_MATH_H)
	    nếu (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    không_thì nếu (isinf(cur_val->vval.v_float))
	    {
		nếu (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		không_thì
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    không_thì
#endif
	    {
		size_t	numbuflen;

		numbuflen = vim_snprintf_safelen((char *)numbuf, sizeof(numbuf),
		    "%g", cur_val->vval.v_float);
		ga_concat_len(gap, numbuf, numbuflen);
	    }
	    break;

	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	    internal_error_no_abort("json_encode_item()");
	    goto theend;
    }

	// Process the completed item by advancing containers or
	// returning when the stack is empty.
	cho (;;)
	{
	    số_nguyên		advance = FALSE;

	    nếu (stack.ga_len == 0)
	    {
		ga_clear(&stack);
		trả_về OK;
	    }

	    frame = ((json_enc_frame_T *)stack.ga_data)
							+ stack.ga_len - 1;
	    switch (frame->je_type)
	    {
		case ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    nếu ((frame->je_options & JSON_JS)
			    && li->li_next == NULL
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_number == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    nếu (li != NULL && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = &li->li_tv;
			advance = TRUE;
			break;
		    }
		    ga_append(gap, ']');
		    frame->je_u.l.list->lv_copyID = 0;
		    --stack.ga_len;
		    break;
		}

		case ENC_TUPLE:
		{
		    số_nguyên	    idx = frame->je_u.t.idx;
		    số_nguyên	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    nếu ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			nếu (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_number == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    nếu (idx < len && !got_int)
		    {
			ga_append(gap, ',');
			options = frame->je_options & JSON_JS;
			cur_val = TUPLE_ITEM(frame->je_u.t.tuple, idx);
			advance = TRUE;
			break;
		    }
		    ga_append(gap, ']');
		    frame->je_u.t.tuple->tv_copyID = 0;
		    --stack.ga_len;
		    break;
		}

		case ENC_DICT:
		{
		    hashitem_T	*hi = frame->je_u.d.hi;
		    số_nguyên		todo = frame->je_u.d.todo;

		    nếu (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			cho (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			nếu ((frame->je_options & JSON_JS)
				&& is_simple_key(hi->hi_key))
			    ga_concat(gap, hi->hi_key);
			không_thì
			    write_string(gap, hi->hi_key);
			ga_append(gap, ':');

			options = frame->je_options | JSON_NO_NONE;
			cur_val = &dict_lookup(hi)->di_tv;
			advance = TRUE;
			break;
		    }
		    ga_append(gap, '}');
		    frame->je_u.d.dict->dv_copyID = 0;
		    --stack.ga_len;
		    break;
		}
	    }
	    nếu (advance)
		break;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    cho (i = 0; i < stack.ga_len; i++)
    {
	frame = ((json_enc_frame_T *)stack.ga_data) + i;
	switch (frame->je_type)
	{
	    case ENC_LIST:
		frame->je_u.l.list->lv_copyID = 0;
		break;
	    case ENC_TUPLE:
		frame->je_u.t.tuple->tv_copyID = 0;
		break;
	    case ENC_DICT:
		frame->je_u.d.dict->dv_copyID = 0;
		break;
	}
    }
    ga_clear(&stack);
    trả_về FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    static void
fill_numbuflen(js_read_T *reader)
{
    nếu (reader->js_fill != NULL && (số_nguyên)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	nếu (reader->js_fill(reader))
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
    số_nguyên c;

    cho (;;)
    {
	c = reader->js_buf[reader->js_used];
	nếu (reader->js_fill != NULL && c == NUL)
	{
	    nếu (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		continue;
	    }
	}
	nếu (c == NUL || c > ' ')
	    break;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    static số_nguyên
json_decode_string(js_read_T *reader, typval_T *res, số_nguyên quote)
{
    garray_T    ga;
    số_nguyên		len;
    char_u	*p;
    số_nguyên		c;
    varnumber_T	nr;

    nếu (res != NULL)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    trong_khi (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	nếu (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    nếu (reader->js_fill == NULL)
		break;
	    len = (số_nguyên)(reader->js_end - p);
	    reader->js_used = (số_nguyên)(p - reader->js_buf);
	    nếu (!reader->js_fill(reader))
		break; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    continue;
	}

	nếu (*p == '\\')
	{
	    c = -1;
	    switch (p[1])
	    {
		case '\\': c = '\\'; break;
		case '"': c = '"'; break;
		case 'b': c = BS; break;
		case 't': c = TAB; break;
		case 'n': c = NL; break;
		case 'f': c = FF; break;
		case 'r': c = CAR; break;
		case 'u':
		    nếu (reader->js_fill != NULL
				     && (số_nguyên)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (số_nguyên)(p - reader->js_buf);
			nếu (reader->js_fill(reader))
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
		    nếu (len == 0)
		    {
			nếu (res != NULL)
			    ga_clear(&ga);
			trả_về FAIL;
		    }
		    p += len + 2;
		    nếu (0xd800 <= nr && nr <= 0xdbff
			    && (số_nguyên)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, NULL, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, NULL, 4, TRUE, NULL);
			nếu (len == 0)
			{
			    nếu (res != NULL)
				ga_clear(&ga);
			    trả_về FAIL;
			}
			nếu (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    nếu (0xd800 <= nr && nr <= 0xdfff)
		    {
			nếu (res != NULL)
			    ga_clear(&ga);
			trả_về FAIL;
		    }
		    nếu (res != NULL)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((số_nguyên)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    break;
		default:
		    // not a special char, skip over backslash
		    ++p;
		    continue;
	    }
	    nếu (c > 0)
	    {
		p += 2;
		nếu (res != NULL)
		    ga_append(&ga, c);
	    }
	}
	không_thì
	{
	    len = utf_ptr2len(p);
	    nếu (res != NULL)
	    {
		nếu (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    trả_về FAIL;
		}
		mch_memmove((char *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (số_nguyên)(p - reader->js_buf);
    nếu (*p == quote)
    {
	++reader->js_used;
	nếu (res != NULL)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#nếu defined(USE_ICONV)
	    nếu (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		nếu (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, NULL);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, NULL, NULL);
	    }
	    không_thì
#endif
		res->vval.v_string = ga.ga_data;
	}
	trả_về OK;
    }
    nếu (res != NULL)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_number = VVAL_NONE;
	ga_clear(&ga);
    }
    trả_về MAYBE;
}

typedef enum {
    JSON_ARRAY,		// parsing items in an array
    JSON_OBJECT_KEY,	// parsing key of an object
    JSON_OBJECT		// parsing item in an object, after the key
} json_decode_T;

typedef struct {
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
    static số_nguyên
json_decode_item(js_read_T *reader, typval_T *res, số_nguyên options)
{
    char_u	*p;
    số_nguyên		i;
    số_nguyên		len;
    số_nguyên		retval;
    garray_T	stack;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *top_item;
    char_u	key_buf[NUMBUFLEN];

    ga_init2(&stack, sizeof(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    nếu (res != NULL)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    cho (;;)
    {
	top_item = NULL;
	nếu (stack.ga_len > 0)
	{
	    top_item = ((json_dec_item_T *)stack.ga_data) + stack.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    nếu (*p == NUL)
	    {
		retval = MAYBE;
		goto theend;
	    }
	    nếu (top_item->jd_type == JSON_OBJECT_KEY
					    || top_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		nếu (*p == (top_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --stack.ga_len;
		    nếu (stack.ga_len == 0)
		    {
			retval = OK;
			goto theend;
		    }
		    nếu (cur_item != NULL)
			cur_item = &top_item->jd_tv;
		    goto item_end;
		}
	    }
	}

	nếu (top_item != NULL && top_item->jd_type == JSON_OBJECT_KEY
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *key;

	    // accept an object key that is not in quotes
	    key = p = reader->js_buf + reader->js_used;
	    trong_khi (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    nếu (cur_item != NULL)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(key, p - key);
		top_item->jd_key = cur_item->vval.v_string;
	    }
	    reader->js_used += (số_nguyên)(p - key);
	}
	không_thì
	{
	    switch (*p)
	    {
		case '[': // start of array
		    nếu (top_item && top_item->jd_type == JSON_OBJECT_KEY)
		    {
			retval = FAIL;
			break;
		    }
		    nếu (stack.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			break;
		    }
		    nếu (ga_grow(&stack, 1) == FAIL)
		    {
			retval = FAIL;
			break;
		    }
		    nếu (cur_item != NULL && rettv_list_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
			retval = FAIL;
			break;
		    }

		    ++reader->js_used; // consume the '['
		    top_item = ((json_dec_item_T *)stack.ga_data)
								+ stack.ga_len;
		    top_item->jd_type = JSON_ARRAY;
		    ++stack.ga_len;
		    nếu (cur_item != NULL)
		    {
			top_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    continue;

		case '{': // start of object
		    nếu (top_item && top_item->jd_type == JSON_OBJECT_KEY)
		    {
			retval = FAIL;
			break;
		    }
		    nếu (stack.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			break;
		    }
		    nếu (ga_grow(&stack, 1) == FAIL)
		    {
			retval = FAIL;
			break;
		    }
		    nếu (cur_item != NULL && rettv_dict_alloc(cur_item) == FAIL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
			retval = FAIL;
			break;
		    }

		    ++reader->js_used; // consume the '{'
		    top_item = ((json_dec_item_T *)stack.ga_data)
								+ stack.ga_len;
		    top_item->jd_type = JSON_OBJECT_KEY;
		    ++stack.ga_len;
		    nếu (cur_item != NULL)
		    {
			top_item->jd_tv = *cur_item;
			cur_item = &top_item->jd_key_tv;
		    }
		    continue;

		case '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    break;

		case '\'':
		    nếu (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    không_thì
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    break;

		case ',': // comma: empty item
		    nếu ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			break;
		    }
		    // FALLTHROUGH
		case NUL: // empty
		    nếu (cur_item != NULL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
		    }
		    retval = OK;
		    break;

		default:
		    nếu (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			nếu (*sp == '-')
			{
			    ++sp;
			    nếu (*sp == NUL)
			    {
				retval = MAYBE;
				break;
			    }
			    nếu (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				break;
			    }
			}
			sp = skipdigits(sp);
			nếu (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    nếu (cur_item == NULL)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    không_thì
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			không_thì
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    NULL, &len, 0, // what
				    &nr, NULL, 0, TRUE, NULL);
			    nếu (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				goto theend;
			    }
			    nếu (cur_item != NULL)
			    {
				cur_item->v_type = VAR_NUMBER;
				cur_item->vval.v_number = nr;
			    }
			}
			reader->js_used += len;
			retval = OK;
			break;
		    }
		    // In strinct JSON mode, keywords must be lowercase.
		    // In JS mode, keywords are case-insensitive.
#define MATCH_KW(p, kw, len) \
    ((options & JSON_JS) \
     ? STRNICMP((char *)(p), (kw), (len)) == 0 \
     : STRNCMP((char *)(p), (kw), (len)) == 0)
		    nếu (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			nếu (cur_item != NULL)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_number = VVAL_FALSE;
			}
			retval = OK;
			break;
		    }
		    nếu (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			nếu (cur_item != NULL)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_number = VVAL_TRUE;
			}
			retval = OK;
			break;
		    }
		    nếu (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			nếu (cur_item != NULL)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_number = VVAL_NULL;
			}
			retval = OK;
			break;
		    }
		    nếu (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			nếu (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			break;
		    }
		    nếu (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			nếu (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			break;
		    }
		    nếu (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			nếu (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			break;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (số_nguyên)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    nếu (
			    (len < 5 && STRNICMP((char *)p, "false", len) == 0)
			    || (len < 9
				 && STRNICMP((char *)p, "-Infinity", len) == 0)
			    || (len < 8
				  && STRNICMP((char *)p, "Infinity", len) == 0)
			    || (len < 3 && STRNICMP((char *)p, "NaN", len) == 0)
			    || (len < 4
				  && (STRNICMP((char *)p, "true", len) == 0
				    || STRNICMP((char *)p, "null", len) == 0)))

			retval = MAYBE;
		    không_thì
			retval = FAIL;
		    break;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    nếu (retval == FAIL)
		break;
	    nếu (retval == MAYBE || stack.ga_len == 0)
		goto theend;

	    nếu (top_item != NULL && top_item->jd_type == JSON_OBJECT_KEY
		    && cur_item != NULL)
	    {
		nếu (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    goto theend;
		}
		top_item->jd_key = tv_get_string_buf_chk(cur_item, key_buf);
		nếu (top_item->jd_key == NULL)
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
	    case JSON_ARRAY:
		nếu (res != NULL)
		{
		    listitem_T	*li = listitem_alloc();

		    nếu (li == NULL)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			goto theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(top_item->jd_tv.vval.v_list, li);
		}
		nếu (cur_item != NULL)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		nếu (*p == ',')
		    ++reader->js_used;
		không_thì nếu (*p != ']')
		{
		    nếu (*p == NUL)
			retval = MAYBE;
		    không_thì
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    goto theend;
		}
		break;

	    case JSON_OBJECT_KEY:
		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		nếu (*p != ':')
		{
		    nếu (cur_item != NULL)
			clear_tv(cur_item);
		    nếu (*p == NUL)
			retval = MAYBE;
		    không_thì
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    goto theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		top_item->jd_type = JSON_OBJECT;
		nếu (cur_item != NULL)
		    cur_item = &item;
		break;

	    case JSON_OBJECT:
		nếu (cur_item != NULL
			&& dict_has_key(top_item->jd_tv.vval.v_dict,
						(char *)top_item->jd_key))
		{
		    semsg(_(e_duplicate_key_in_json_str), top_item->jd_key);
		    clear_tv(cur_item);
		    retval = FAIL;
		    goto theend;
		}

		nếu (cur_item != NULL)
		{
		    dictitem_T *di = dictitem_alloc(top_item->jd_key);

		    clear_tv(&top_item->jd_key_tv);
		    nếu (di == NULL)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			goto theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    nếu (dict_add(top_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			goto theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		nếu (*p == ',')
		    ++reader->js_used;
		không_thì nếu (*p != '}')
		{
		    nếu (*p == NUL)
			retval = MAYBE;
		    không_thì
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    goto theend;
		}
		top_item->jd_type = JSON_OBJECT_KEY;
		nếu (cur_item != NULL)
		    cur_item = &top_item->jd_key_tv;
		break;
	}
    }

    // Get here when parsing failed.
    nếu (res != NULL)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_number = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    cho (i = 0; i < stack.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)stack.ga_data) + i)->jd_key_tv);
    ga_clear(&stack);

    trả_về retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    static số_nguyên
json_decode_all(js_read_T *reader, typval_T *res, số_nguyên options)
{
    số_nguyên ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    nếu (ret != OK)
    {
	nếu (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	trả_về FAIL;
    }
    json_skip_white(reader);
    nếu (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	trả_về FAIL;
    }
    trả_về OK;
}

#nếu defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    số_nguyên
json_decode(js_read_T *reader, typval_T *res, số_nguyên options)
{
    số_nguyên ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    trả_về ret;
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
    số_nguyên
json_find_end(js_read_T *reader, số_nguyên options)
{
    số_nguyên used_save = reader->js_used;
    số_nguyên ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, NULL, options);
    reader->js_used = used_save;
    trả_về ret;
}

/*
 * "js_decode()" function
 */
    void
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = NULL;
    reader.js_used = 0;
    nếu (json_decode_all(&reader, rettv, JSON_JS) != OK)
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

    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

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
