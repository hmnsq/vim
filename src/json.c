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

#если defined(FEAT_EVAL)

static целое json_encode_item(garray_T *gap, typval_T *val, целое copyID, целое options);

/*
 * Encode "val" into a JSON format string.
 * The result is added to "gap"
 * Returns FAIL on failure and makes gap->ga_data empty.
 */
    static целое
json_encode_gap(garray_T *gap, typval_T *val, целое options)
{
    если (json_encode_item(gap, val, get_copyID(), options) == FAIL)
    {
	ga_clear(gap);
	gap->ga_data = vim_strsave((char_u *)"");
	вернуть FAIL;
    }
    вернуть OK;
}

/*
 * Encode "val" into a JSON format string.
 * The result is in allocated memory.
 * The result is empty when encoding fails.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 */
    char_u *
json_encode(typval_T *val, целое options)
{
    garray_T ga;

    // Store bytes in the growarray.
    ga_init2(&ga, 1, 4000);
    json_encode_gap(&ga, val, options);
    если (options & JSON_NL)
	ga_append(&ga, NL);
    ga_append(&ga, NUL);
    вернуть ga.ga_data;
}

#если defined(FEAT_JOB_CHANNEL)
/*
 * Encode ["nr", "val"] into a JSON format string in allocated memory.
 * "options" can contain JSON_JS, JSON_NO_NONE and JSON_NL.
 * Returns NULL when out of memory.
 */
    char_u *
json_encode_nr_expr(целое nr, typval_T *val, целое options)
{
    typval_T	listtv;
    typval_T	nrtv;
    garray_T	ga;

    nrtv.v_type = VAR_NUMBER;
    nrtv.vval.v_number = nr;
    если (rettv_list_alloc(&listtv) == FAIL)
	вернуть NULL;
    если (list_append_tv(listtv.vval.v_list, &nrtv) == FAIL
	    || list_append_tv(listtv.vval.v_list, val) == FAIL)
    {
	list_unref(listtv.vval.v_list);
	вернуть NULL;
    }

    ga_init2(&ga, 1, 4000);
    если (json_encode_gap(&ga, &listtv, options) == OK && (options & JSON_NL))
	ga_append(&ga, '\n');
    list_unref(listtv.vval.v_list);
    ga_append(&ga, NUL);
    вернуть ga.ga_data;
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
    если (json_encode_gap(&ga, val, 0) == FAIL)
	вернуть NULL;
    ga_append(&ga, NUL);

    ga_init2(&lspga, 1, 4000);
    // Header according to LSP specification.
    IObufflen = vim_snprintf_safelen((char *)IObuff, IOSIZE,
	    "Content-Length: %u\r\n\r\n",
	    ga.ga_len - 1);
    ga_concat_len(&lspga, IObuff, IObufflen);
    ga_concat_len(&lspga, ga.ga_data, ga.ga_len);
    ga_clear(&ga);
    вернуть lspga.ga_data;
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
#если defined(USE_ICONV)
    vimconv_T   conv;
    char_u	*converted = NULL;
#endif
    целое		c;

    если (res == NULL)
    {
	GA_CONCAT_LITERAL(gap, "\"\"");
	вернуть;
    }

#если defined(USE_ICONV)
    если (!enc_utf8)
    {
	// Convert the text from 'encoding' to utf-8, because a JSON string is
	// always utf-8.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, p_enc, (char_u*)"utf-8");
	если (conv.vc_type != CONV_NONE)
	    converted = res = string_convert(&conv, res, NULL);
	convert_setup(&conv, NULL, NULL);
    }
#endif
    ga_append(gap, '"');
    // Pre-grow for the common case: input length + quotes + some escapes.
    ga_grow(gap, (целое)STRLEN(res) + 2);
    // `from` is the beginning of a sequence of bytes we can directly copy from
    // the input string, avoiding the overhead associated to decoding/encoding
    // them.
    from = res;
    пока ((c = *res) != NUL)
    {
	// always use utf-8 encoding, ignore 'encoding'
	если (c < 0x80)
	{
	    если (!ascii_needs_escape[c])
	    {
		res += 1;
		continue;
	    }

	    если (res != from)
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
	иначе
	{
	    целое	    l = utf_ptr2len(res);
	    size_t  numbuflen;

	    если (l > 1)
	    {
		res += l;
		continue;
	    }

	    // Invalid utf-8 sequence, replace it with the Unicode replacement
	    // character U+FFFD.
	    если (res != from)
		ga_concat_len(gap, from, res - from);
	    from = res + 1;

	    numbuflen = utf_char2bytes(0xFFFD, numbuf);
	    numbuf[numbuflen] = NUL;
	    ga_concat_len(gap, numbuf, numbuflen);

	    res += l;
	}
    }

    если (res != from)
	ga_concat_len(gap, from, res - from);

    ga_append(gap, '"');
#если defined(USE_ICONV)
    vim_free(converted);
#endif
}

/*
 * Return TRUE if "key" can be used without quotes.
 * That is when it starts with a letter and only contains letters, digits and
 * underscore.
 */
    static целое
is_simple_key(char_u *key)
{
    char_u *p;

    если (!ASCII_ISALPHA(*key))
	вернуть FALSE;
    для (p = key + 1; *p != NUL; ++p)
	если (!ASCII_ISALPHA(*p) && *p != '_' && !vim_isdigit(*p))
	    вернуть FALSE;
    вернуть TRUE;
}

typedef enum {
    ENC_LIST,
    ENC_TUPLE,
    ENC_DICT,
} json_enc_type_T;

typedef struct {
    json_enc_type_T je_type;
    целое		    je_options;	    // options when this container was entered
    union {
	struct {
	    list_T	*list;
	    listitem_T	*li;	    // current item
	} l;
	struct {
	    tuple_T	*tuple;
	    целое		idx;	    // current index
	    целое		len;
	} t;
	struct {
	    dict_T	*dict;
	    hashitem_T	*hi;	    // current entry
	    целое		todo;	    // remaining entries
	} d;
    } je_u;
} json_enc_frame_T;

/*
 * Encode "val" into "gap".
 * Uses an explicit stack to avoid deep recursion on nested structures.
 * Return FAIL or OK.
 */
    static целое
json_encode_item(garray_T *gap, typval_T *val, целое copyID, целое options)
{
    char_u		numbuf[NUMBUFLEN];
    char_u		*res;
    blob_T		*b;
    list_T		*l;
    tuple_T		*tuple;
    dict_T		*d;
    целое			i;
    garray_T		stack;
    typval_T		*cur_val;
    json_enc_frame_T	*frame;

    ga_init2(&stack, sizeof(json_enc_frame_T), 100);
    cur_val = val;

    для (;;)
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
		case VVAL_NONE: если ((options & JSON_JS) != 0
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
	    если (b == NULL || b->bv_ga.ga_len == 0)
		GA_CONCAT_LITERAL(gap, "[]");
	    иначе
	    {
		целое	blen = b->bv_ga.ga_len;
		char_u	*src;
		char_u	*dst;

		// Worst case: '[' + ']' + per-byte 3 digits + comma = 2 + 4*blen
		если (ga_grow(gap, 2 + 4 * blen) == FAIL)
		    goto theend;
		src = (char_u *)b->bv_ga.ga_data;
		dst = (char_u *)gap->ga_data + gap->ga_len;
		*dst++ = '[';
		для (i = 0; i < blen; i++)
		{
		    целое	    byte = src[i];

		    если (i > 0)
			*dst++ = ',';
		    если (byte >= 100)
		    {
			*dst++ = '0' + byte / 100;
			*dst++ = '0' + (byte / 10) % 10;
			*dst++ = '0' + byte % 10;
		    }
		    иначе если (byte >= 10)
		    {
			*dst++ = '0' + byte / 10;
			*dst++ = '0' + byte % 10;
		    }
		    иначе
			*dst++ = '0' + byte;
		}
		*dst++ = ']';
		gap->ga_len = (целое)(dst - (char_u *)gap->ga_data);
	    }
	    break;

	case VAR_LIST:
	    l = cur_val->vval.v_list;
	    если (l == NULL)
		GA_CONCAT_LITERAL(gap, "[]");
	    иначе
	    {
		если (l->lv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		иначе
		{
		    l->lv_copyID = copyID;
		    ga_append(gap, '[');
		    CHECK_LIST_MATERIALIZE(l);
		    если (l->lv_first != NULL)
		    {
			если (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			если (ga_grow(&stack, 1) == FAIL)
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
	    если (tuple == NULL)
		GA_CONCAT_LITERAL(gap, "[]");
	    иначе
	    {
		если (tuple->tv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "[]");
		иначе
		{
		    целое		len = TUPLE_LEN(tuple);

		    если (len > 0)
		    {
			tuple->tv_copyID = copyID;
			ga_append(gap, '[');
			если (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			если (ga_grow(&stack, 1) == FAIL)
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
		    иначе
			GA_CONCAT_LITERAL(gap, "[]");
		}
	    }
	    break;

	case VAR_DICT:
	    d = cur_val->vval.v_dict;
	    если (d == NULL)
		GA_CONCAT_LITERAL(gap, "{}");
	    иначе
	    {
		если (d->dv_copyID == copyID)
		    GA_CONCAT_LITERAL(gap, "{}");
		иначе
		{
		    целое		todo = (целое)d->dv_hashtab.ht_used;
		    hashitem_T	*hi;

		    если (todo > 0)
		    {
			d->dv_copyID = copyID;
			ga_append(gap, '{');

			// Find first non-empty hash entry
			для (hi = d->dv_hashtab.ht_array;
						  HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;

			// Write first key
			если ((options & JSON_JS)
						 && is_simple_key(hi->hi_key))
			    ga_concat(gap, hi->hi_key);
			иначе
			    write_string(gap, hi->hi_key);
			ga_append(gap, ':');

			если (stack.ga_len >= p_mfd)
			{
			    emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			    goto theend;
			}
			если (ga_grow(&stack, 1) == FAIL)
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
		    иначе
			GA_CONCAT_LITERAL(gap, "{}");
		}
	    }
	    break;

	case VAR_FLOAT:
#если defined(HAVE_MATH_H)
	    если (isnan(cur_val->vval.v_float))
		GA_CONCAT_LITERAL(gap, "NaN");
	    иначе если (isinf(cur_val->vval.v_float))
	    {
		если (cur_val->vval.v_float < 0.0)
		    GA_CONCAT_LITERAL(gap, "-Infinity");
		иначе
		    GA_CONCAT_LITERAL(gap, "Infinity");
	    }
	    иначе
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
	для (;;)
	{
	    целое		advance = FALSE;

	    если (stack.ga_len == 0)
	    {
		ga_clear(&stack);
		вернуть OK;
	    }

	    frame = ((json_enc_frame_T *)stack.ga_data)
							+ stack.ga_len - 1;
	    switch (frame->je_type)
	    {
		case ENC_LIST:
		{
		    listitem_T	*li = frame->je_u.l.li;

		    // JSON_JS: add trailing comma if last item is v:none
		    если ((frame->je_options & JSON_JS)
			    && li->li_next == NULL
			    && li->li_tv.v_type == VAR_SPECIAL
			    && li->li_tv.vval.v_number == VVAL_NONE)
			ga_append(gap, ',');

		    li = li->li_next;
		    frame->je_u.l.li = li;
		    если (li != NULL && !got_int)
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
		    целое	    idx = frame->je_u.t.idx;
		    целое	    len = frame->je_u.t.len;

		    // JSON_JS: add trailing comma if last item is v:none
		    если ((frame->je_options & JSON_JS) && idx == len - 1)
		    {
			typval_T *t_item =
				    TUPLE_ITEM(frame->je_u.t.tuple, idx);

			если (t_item->v_type == VAR_SPECIAL
				&& t_item->vval.v_number == VVAL_NONE)
			    ga_append(gap, ',');
		    }

		    ++idx;
		    frame->je_u.t.idx = idx;
		    если (idx < len && !got_int)
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
		    целое		todo = frame->je_u.d.todo;

		    если (todo > 0 && !got_int)
		    {
			// Advance to next non-empty entry
			для (++hi; HASHITEM_EMPTY(hi); ++hi)
			    ;
			--todo;
			frame->je_u.d.hi = hi;
			frame->je_u.d.todo = todo;

			ga_append(gap, ',');
			если ((frame->je_options & JSON_JS)
				&& is_simple_key(hi->hi_key))
			    ga_concat(gap, hi->hi_key);
			иначе
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
	    если (advance)
		break;
	}
    }

theend:
    // Clean up copyIDs for remaining frames on error/failure
    для (i = 0; i < stack.ga_len; i++)
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
    вернуть FAIL;
}

/*
 * When "reader" has less than NUMBUFLEN bytes available, call the fill
 * callback to get more.
 */
    static void
fill_numbuflen(js_read_T *reader)
{
    если (reader->js_fill != NULL && (целое)(reader->js_end - reader->js_buf)
						- reader->js_used < NUMBUFLEN)
    {
	если (reader->js_fill(reader))
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
    целое c;

    для (;;)
    {
	c = reader->js_buf[reader->js_used];
	если (reader->js_fill != NULL && c == NUL)
	{
	    если (reader->js_fill(reader))
	    {
		reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
		continue;
	    }
	}
	если (c == NUL || c > ' ')
	    break;
	++reader->js_used;
    }
    fill_numbuflen(reader);
}

    static целое
json_decode_string(js_read_T *reader, typval_T *res, целое quote)
{
    garray_T    ga;
    целое		len;
    char_u	*p;
    целое		c;
    varnumber_T	nr;

    если (res != NULL)
	ga_init2(&ga, 1, 200);

    p = reader->js_buf + reader->js_used + 1; // skip over " or '
    пока (*p != quote)
    {
	// The JSON is always expected to be utf-8, thus use utf functions
	// here. The string is converted below if needed.
	если (*p == NUL || p[1] == NUL || utf_ptr2len(p) < utf_byte2len(*p))
	{
	    // Not enough bytes to make a character or end of the string. Get
	    // more if possible.
	    если (reader->js_fill == NULL)
		break;
	    len = (целое)(reader->js_end - p);
	    reader->js_used = (целое)(p - reader->js_buf);
	    если (!reader->js_fill(reader))
		break; // didn't get more
	    p = reader->js_buf + reader->js_used;
	    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
	    continue;
	}

	если (*p == '\\')
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
		    если (reader->js_fill != NULL
				     && (целое)(reader->js_end - p) < NUMBUFLEN)
		    {
			reader->js_used = (целое)(p - reader->js_buf);
			если (reader->js_fill(reader))
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
		    если (len == 0)
		    {
			если (res != NULL)
			    ga_clear(&ga);
			вернуть FAIL;
		    }
		    p += len + 2;
		    если (0xd800 <= nr && nr <= 0xdbff
			    && (целое)(reader->js_end - p) >= 6
			    && *p == '\\' && *(p+1) == 'u')
		    {
			varnumber_T	nr2 = 0;

			// decode surrogate pair: \ud812\u3456
			len = 0;
			vim_str2nr(p + 2, NULL, &len, STR2NR_HEX + STR2NR_FORCE,
						    &nr2, NULL, 4, TRUE, NULL);
			если (len == 0)
			{
			    если (res != NULL)
				ga_clear(&ga);
			    вернуть FAIL;
			}
			если (0xdc00 <= nr2 && nr2 <= 0xdfff)
			{
			    p += len + 2;
			    nr = (((nr - 0xd800) << 10) |
				((nr2 - 0xdc00) & 0x3ff)) + 0x10000;
			}
		    }
		    // Lone surrogate is invalid.
		    если (0xd800 <= nr && nr <= 0xdfff)
		    {
			если (res != NULL)
			    ga_clear(&ga);
			вернуть FAIL;
		    }
		    если (res != NULL)
		    {
			char_u	buf[NUMBUFLEN];
			size_t	buflen;

			buflen = utf_char2bytes((целое)nr, buf);
			buf[buflen] = NUL;
			ga_concat_len(&ga, buf, buflen);
		    }
		    break;
		default:
		    // not a special char, skip over backslash
		    ++p;
		    continue;
	    }
	    если (c > 0)
	    {
		p += 2;
		если (res != NULL)
		    ga_append(&ga, c);
	    }
	}
	иначе
	{
	    len = utf_ptr2len(p);
	    если (res != NULL)
	    {
		если (ga_grow(&ga, len) == FAIL)
		{
		    ga_clear(&ga);
		    вернуть FAIL;
		}
		mch_memmove((char *)ga.ga_data + ga.ga_len, p, (size_t)len);
		ga.ga_len += len;
	    }
	    p += len;
	}
    }

    reader->js_used = (целое)(p - reader->js_buf);
    если (*p == quote)
    {
	++reader->js_used;
	если (res != NULL)
	{
	    ga_append(&ga, NUL);
	    res->v_type = VAR_STRING;
#если defined(USE_ICONV)
	    если (!enc_utf8)
	    {
		vimconv_T   conv;

		// Convert the utf-8 string to 'encoding'.
		conv.vc_type = CONV_NONE;
		convert_setup(&conv, (char_u*)"utf-8", p_enc);
		если (conv.vc_type != CONV_NONE)
		{
		    res->vval.v_string =
				      string_convert(&conv, ga.ga_data, NULL);
		    vim_free(ga.ga_data);
		}
		convert_setup(&conv, NULL, NULL);
	    }
	    иначе
#endif
		res->vval.v_string = ga.ga_data;
	}
	вернуть OK;
    }
    если (res != NULL)
    {
	res->v_type = VAR_SPECIAL;
	res->vval.v_number = VVAL_NONE;
	ga_clear(&ga);
    }
    вернуть MAYBE;
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
    static целое
json_decode_item(js_read_T *reader, typval_T *res, целое options)
{
    char_u	*p;
    целое		i;
    целое		len;
    целое		retval;
    garray_T	stack;
    typval_T	item;
    typval_T	*cur_item;
    json_dec_item_T *top_item;
    char_u	key_buf[NUMBUFLEN];

    ga_init2(&stack, sizeof(json_dec_item_T), 100);
    cur_item = res;
    init_tv(&item);
    если (res != NULL)
	init_tv(res);

    fill_numbuflen(reader);
    p = reader->js_buf + reader->js_used;
    для (;;)
    {
	top_item = NULL;
	если (stack.ga_len > 0)
	{
	    top_item = ((json_dec_item_T *)stack.ga_data) + stack.ga_len - 1;
	    json_skip_white(reader);
	    p = reader->js_buf + reader->js_used;
	    если (*p == NUL)
	    {
		retval = MAYBE;
		goto theend;
	    }
	    если (top_item->jd_type == JSON_OBJECT_KEY
					    || top_item->jd_type == JSON_ARRAY)
	    {
		// Check for end of object or array.
		если (*p == (top_item->jd_type == JSON_ARRAY ? ']' : '}'))
		{
		    ++reader->js_used; // consume the ']' or '}'
		    --stack.ga_len;
		    если (stack.ga_len == 0)
		    {
			retval = OK;
			goto theend;
		    }
		    если (cur_item != NULL)
			cur_item = &top_item->jd_tv;
		    goto item_end;
		}
	    }
	}

	если (top_item != NULL && top_item->jd_type == JSON_OBJECT_KEY
		&& (options & JSON_JS)
		&& reader->js_buf[reader->js_used] != '"'
		&& reader->js_buf[reader->js_used] != '\''
		&& reader->js_buf[reader->js_used] != '['
		&& reader->js_buf[reader->js_used] != '{')
	{
	    char_u *key;

	    // accept an object key that is not in quotes
	    key = p = reader->js_buf + reader->js_used;
	    пока (*p != NUL && *p != ':' && *p > ' ')
		++p;
	    если (cur_item != NULL)
	    {
		cur_item->v_type = VAR_STRING;
		cur_item->vval.v_string = vim_strnsave(key, p - key);
		top_item->jd_key = cur_item->vval.v_string;
	    }
	    reader->js_used += (целое)(p - key);
	}
	иначе
	{
	    switch (*p)
	    {
		case '[': // start of array
		    если (top_item && top_item->jd_type == JSON_OBJECT_KEY)
		    {
			retval = FAIL;
			break;
		    }
		    если (stack.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			break;
		    }
		    если (ga_grow(&stack, 1) == FAIL)
		    {
			retval = FAIL;
			break;
		    }
		    если (cur_item != NULL && rettv_list_alloc(cur_item) == FAIL)
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
		    если (cur_item != NULL)
		    {
			top_item->jd_tv = *cur_item;
			cur_item = &item;
		    }
		    continue;

		case '{': // start of object
		    если (top_item && top_item->jd_type == JSON_OBJECT_KEY)
		    {
			retval = FAIL;
			break;
		    }
		    если (stack.ga_len >= p_mfd)
		    {
			emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
			retval = FAIL;
			break;
		    }
		    если (ga_grow(&stack, 1) == FAIL)
		    {
			retval = FAIL;
			break;
		    }
		    если (cur_item != NULL && rettv_dict_alloc(cur_item) == FAIL)
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
		    если (cur_item != NULL)
		    {
			top_item->jd_tv = *cur_item;
			cur_item = &top_item->jd_key_tv;
		    }
		    continue;

		case '"': // string
		    retval = json_decode_string(reader, cur_item, *p);
		    break;

		case '\'':
		    если (options & JSON_JS)
			retval = json_decode_string(reader, cur_item, *p);
		    иначе
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    break;

		case ',': // comma: empty item
		    если ((options & JSON_JS) == 0)
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
			break;
		    }
		    // FALLTHROUGH
		case NUL: // empty
		    если (cur_item != NULL)
		    {
			cur_item->v_type = VAR_SPECIAL;
			cur_item->vval.v_number = VVAL_NONE;
		    }
		    retval = OK;
		    break;

		default:
		    если (VIM_ISDIGIT(*p) || (*p == '-'
					&& (VIM_ISDIGIT(p[1]) || p[1] == NUL)))
		    {
			char_u  *sp = p;

			если (*sp == '-')
			{
			    ++sp;
			    если (*sp == NUL)
			    {
				retval = MAYBE;
				break;
			    }
			    если (!VIM_ISDIGIT(*sp))
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				break;
			    }
			}
			sp = skipdigits(sp);
			если (*sp == '.' || *sp == 'e' || *sp == 'E')
			{
			    если (cur_item == NULL)
			    {
				float_T f;

				len = string2float(p, &f, FALSE);
			    }
			    иначе
			    {
				cur_item->v_type = VAR_FLOAT;
				len = string2float(p, &cur_item->vval.v_float,
									FALSE);
			    }
			}
			иначе
			{
			    varnumber_T nr;

			    vim_str2nr(reader->js_buf + reader->js_used,
				    NULL, &len, 0, // what
				    &nr, NULL, 0, TRUE, NULL);
			    если (len == 0)
			    {
				semsg(_(e_json_decode_error_at_str), p);
				retval = FAIL;
				goto theend;
			    }
			    если (cur_item != NULL)
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
		    если (MATCH_KW(p, "false", 5))
		    {
			reader->js_used += 5;
			если (cur_item != NULL)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_number = VVAL_FALSE;
			}
			retval = OK;
			break;
		    }
		    если (MATCH_KW(p, "true", 4))
		    {
			reader->js_used += 4;
			если (cur_item != NULL)
			{
			    cur_item->v_type = VAR_BOOL;
			    cur_item->vval.v_number = VVAL_TRUE;
			}
			retval = OK;
			break;
		    }
		    если (MATCH_KW(p, "null", 4))
		    {
			reader->js_used += 4;
			если (cur_item != NULL)
			{
			    cur_item->v_type = VAR_SPECIAL;
			    cur_item->vval.v_number = VVAL_NULL;
			}
			retval = OK;
			break;
		    }
		    если (MATCH_KW(p, "NaN", 3))
		    {
			reader->js_used += 3;
			если (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = NAN;
			}
			retval = OK;
			break;
		    }
		    если (MATCH_KW(p, "-Infinity", 9))
		    {
			reader->js_used += 9;
			если (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = -INFINITY;
			}
			retval = OK;
			break;
		    }
		    если (MATCH_KW(p, "Infinity", 8))
		    {
			reader->js_used += 8;
			если (cur_item != NULL)
			{
			    cur_item->v_type = VAR_FLOAT;
			    cur_item->vval.v_float = INFINITY;
			}
			retval = OK;
			break;
		    }
#undef MATCH_KW
		    // check for truncated name
		    len = (целое)(reader->js_end
					 - (reader->js_buf + reader->js_used));
		    если (
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
		    иначе
			retval = FAIL;
		    break;
	    }

	    // We are finished when retval is FAIL or MAYBE and when at the
	    // toplevel.
	    если (retval == FAIL)
		break;
	    если (retval == MAYBE || stack.ga_len == 0)
		goto theend;

	    если (top_item != NULL && top_item->jd_type == JSON_OBJECT_KEY
		    && cur_item != NULL)
	    {
		если (cur_item->v_type == VAR_FLOAT)
		{
		    // cannot use a float as a key
		    emsg(_(e_using_float_as_string));
		    retval = FAIL;
		    goto theend;
		}
		top_item->jd_key = tv_get_string_buf_chk(cur_item, key_buf);
		если (top_item->jd_key == NULL)
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
		если (res != NULL)
		{
		    listitem_T	*li = listitem_alloc();

		    если (li == NULL)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			goto theend;
		    }
		    li->li_tv = *cur_item;
		    list_append(top_item->jd_tv.vval.v_list, li);
		}
		если (cur_item != NULL)
		    cur_item = &item;

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		если (*p == ',')
		    ++reader->js_used;
		иначе если (*p != ']')
		{
		    если (*p == NUL)
			retval = MAYBE;
		    иначе
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
		если (*p != ':')
		{
		    если (cur_item != NULL)
			clear_tv(cur_item);
		    если (*p == NUL)
			retval = MAYBE;
		    иначе
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    goto theend;
		}
		++reader->js_used;
		json_skip_white(reader);
		top_item->jd_type = JSON_OBJECT;
		если (cur_item != NULL)
		    cur_item = &item;
		break;

	    case JSON_OBJECT:
		если (cur_item != NULL
			&& dict_has_key(top_item->jd_tv.vval.v_dict,
						(char *)top_item->jd_key))
		{
		    semsg(_(e_duplicate_key_in_json_str), top_item->jd_key);
		    clear_tv(cur_item);
		    retval = FAIL;
		    goto theend;
		}

		если (cur_item != NULL)
		{
		    dictitem_T *di = dictitem_alloc(top_item->jd_key);

		    clear_tv(&top_item->jd_key_tv);
		    если (di == NULL)
		    {
			clear_tv(cur_item);
			retval = FAIL;
			goto theend;
		    }
		    di->di_tv = *cur_item;
		    di->di_tv.v_lock = 0;
		    если (dict_add(top_item->jd_tv.vval.v_dict, di) == FAIL)
		    {
			dictitem_free(di);
			retval = FAIL;
			goto theend;
		    }
		}

		json_skip_white(reader);
		p = reader->js_buf + reader->js_used;
		если (*p == ',')
		    ++reader->js_used;
		иначе если (*p != '}')
		{
		    если (*p == NUL)
			retval = MAYBE;
		    иначе
		    {
			semsg(_(e_json_decode_error_at_str), p);
			retval = FAIL;
		    }
		    goto theend;
		}
		top_item->jd_type = JSON_OBJECT_KEY;
		если (cur_item != NULL)
		    cur_item = &top_item->jd_key_tv;
		break;
	}
    }

    // Get here when parsing failed.
    если (res != NULL)
    {
	clear_tv(res);
	res->v_type = VAR_SPECIAL;
	res->vval.v_number = VVAL_NONE;
    }
    // "p" may dangle into a buffer freed by a js_fill() refill in
    // json_decode_string(); report the position from the current reader.
    semsg(_(e_json_decode_error_at_str), reader->js_buf + reader->js_used);

theend:
    для (i = 0; i < stack.ga_len; i++)
	clear_tv(&(((json_dec_item_T *)stack.ga_data) + i)->jd_key_tv);
    ga_clear(&stack);

    вернуть retval;
}

/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL if not the whole message was consumed.
 */
    static целое
json_decode_all(js_read_T *reader, typval_T *res, целое options)
{
    целое ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    если (ret != OK)
    {
	если (ret == MAYBE)
	    semsg(_(e_json_decode_error_at_str), reader->js_buf);
	вернуть FAIL;
    }
    json_skip_white(reader);
    если (reader->js_buf[reader->js_used] != NUL)
    {
	semsg(_(e_trailing_characters_str), reader->js_buf + reader->js_used);
	вернуть FAIL;
    }
    вернуть OK;
}

#если defined(FEAT_JOB_CHANNEL)
/*
 * Decode the JSON from "reader" and store the result in "res".
 * "options" can be JSON_JS or zero;
 * Return FAIL for a decoding error.
 * Return MAYBE for an incomplete message.
 * Consumes the message anyway.
 */
    целое
json_decode(js_read_T *reader, typval_T *res, целое options)
{
    целое ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, res, options);
    json_skip_white(reader);

    вернуть ret;
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
    целое
json_find_end(js_read_T *reader, целое options)
{
    целое used_save = reader->js_used;
    целое ret;

    // We find the end once, to avoid calling strlen() many times.
    reader->js_end = reader->js_buf + STRLEN(reader->js_buf);
    json_skip_white(reader);
    ret = json_decode_item(reader, NULL, options);
    reader->js_used = used_save;
    вернуть ret;
}

/*
 * "js_decode()" function
 */
    void
f_js_decode(typval_T *argvars, typval_T *rettv)
{
    js_read_T	reader;

    если (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	вернуть;

    reader.js_buf = tv_get_string(&argvars[0]);
    reader.js_fill = NULL;
    reader.js_used = 0;
    если (json_decode_all(&reader, rettv, JSON_JS) != OK)
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

    если (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	вернуть;

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
