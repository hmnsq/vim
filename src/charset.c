/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

#اگر defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#اگر_ختم

ساکن صحیح parse_isopt(char_u *var, buf_T *buf, صحیح only_check);
ساکن صحیح win_nolbr_chartabsize(chartabsize_T *cts, صحیح *headp);
ساکن غیرعلامتی nr2hex(غیرعلامتی c);

ساکن صحیح    chartab_initialized = FALSE;

// b_chartab[] is an array of 32 bytes, each bit representing one of the
// characters 0-255.
#تعریف_کرو سیٹ_CHARTAB(buf, c) (buf)->b_chartab[(غیرعلامتی)(c) >> 3] |= (1 << ((c) & 0x7))
#تعریف_کرو RESET_CHARTAB(buf, c) (buf)->b_chartab[(غیرعلامتی)(c) >> 3] &= ~(1 << ((c) & 0x7))
#تعریف_کرو GET_CHARTAB(buf, c) ((buf)->b_chartab[(غیرعلامتی)(c) >> 3] & (1 << ((c) & 0x7)))

// table used below, see init_chartab() for an explanation
ساکن char_u	g_chartab[256];

/*
 * Flags for g_chartab[].
 */
#تعریف_کرو CT_CELL_نقاب	0x07	// mask: nr of display cells (1, 2 or 4)
#تعریف_کرو CT_PRINT_CHAR	0x10	// flag: set for printable chars
#تعریف_کرو CT_ID_CHAR	0x20	// flag: set for ID chars
#تعریف_کرو CT_FNAME_CHAR	0x40	// flag: set for file name chars

ساکن صحیح اندر_win_border(win_T *wp, colnr_T vcol);

/*
 * Fill g_chartab[].  Also fills curbuf->b_chartab[] with flags for keyword
 * characters for current buffer.
 *
 * Depends on the option settings 'iskeyword', 'isident', 'isfname',
 * 'isprint' and 'encoding'.
 *
 * The index in g_chartab[] depends on 'encoding':
 * - For non-multi-byte index with the byte (same as the character).
 * - For DBCS index with the first byte.
 * - For UTF-8 index with the character (when first byte is up to 0x80 it is
 *   the same as the character, if the first byte is 0x80 and above it depends
 *   on further bytes).
 *
 * The contents of g_chartab[]:
 * - The lower two bits, masked by CT_CELL_MASK, give the number of display
 *   cells the character occupies (1 or 2).  Not valid for UTF-8 above 0x80.
 * - CT_PRINT_CHAR bit is set when the character is printable (no need to
 *   translate the character before displaying it).  Note that only DBCS
 *   characters can have 2 display cells and still be printable.
 * - CT_FNAME_CHAR bit is set when the character can be in a file name.
 * - CT_ID_CHAR bit is set when the character can be in an identifier.
 *
 * Return FAIL if 'iskeyword', 'isident', 'isfname' or 'isprint' option has an
 * error, OK otherwise.
 */
    صحیح
init_chartab(خالی)
{
    واپس buf_init_chartab(curbuf, TRUE);
}

    صحیح
buf_init_chartab(
    buf_T	*buf,
    صحیح		global)		// FALSE: only set buf->b_chartab[]
{
    صحیح		c;
    char_u	*p;
    صحیح		i;

    اگر (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 */
	c = 0;
	جب (c < ' ')
	    g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	جب (c <= '~')
	    g_chartab[c++] = 1 + CT_PRINT_CHAR;
	جب (c < 256)
	{
	    // UTF-8: bytes 0xa0 - 0xff are printable (latin1)
	    اگر (enc_utf8 && c >= 0xa0)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // euc-jp characters starting with 0x8e are single width
	    ورنہ اگر (enc_dbcs == DBCS_JPNU && c == 0x8e)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // other double-byte chars can be printable AND double-width
	    ورنہ اگر (enc_dbcs != 0 && MB_BYTE2LEN(c) == 2)
		g_chartab[c++] = CT_PRINT_CHAR + 2;
	    ورنہ
		// the rest is unprintable by default
		g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	}

	// Assume that every multi-byte char is a filename character.
	برائے (c = 1; c < 256; ++c)
	    اگر ((enc_dbcs != 0 && MB_BYTE2LEN(c) > 1)
		    || (enc_dbcs == DBCS_JPNU && c == 0x8e)
		    || (enc_utf8 && c >= 0xa0))
		g_chartab[c] |= CT_FNAME_CHAR;
    }

    /*
     * Init word char flags all to FALSE
     */
    CLEAR_میدان(buf->b_chartab);
    اگر (enc_dbcs != 0)
	برائے (c = 0; c < 256; ++c)
	{
	    // double-byte characters are probably word characters
	    اگر (MB_BYTE2LEN(c) == 2)
		سیٹ_CHARTAB(buf, c);
	}

    /*
     * In lisp mode the '-' character is included in keywords.
     */
    اگر (buf->b_p_lisp)
	سیٹ_CHARTAB(buf, '-');

    // Walk through the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    برائے (i = global ? 0 : 3; i <= 3; ++i)
    {
	اگر (i == 0)
	    p = p_isi;		// first round: 'isident'
	ورنہ اگر (i == 1)
	    p = p_isp;		// second round: 'isprint'
	ورنہ اگر (i == 2)
	    p = p_isf;		// third round: 'isfname'
	ورنہ	// i == 3
	    p = buf->b_p_isk;	// fourth round: 'iskeyword'

	اگر (parse_isopt(p, buf, FALSE) == FAIL)
	    واپس FAIL;
    }

    chartab_initialized = TRUE;
    واپس OK;
}

/**
 * Checks the format for the option settings 'iskeyword', 'isident', 'isfname'
 * or 'isprint'.
 * Returns FAIL if has an error, OK otherwise.
 */
	صحیح
check_isopt(char_u *var)
{
    واپس parse_isopt(var, تہی, TRUE);
}

    ساکن صحیح
parse_isopt(
	char_u	*var,
	buf_T	*buf,
	صحیح	only_check)	// FALSE: refill g_chartab[]
{
    char_u	*p = var;
    صحیح		c;
    صحیح		c2;
    صحیح		tilde;
    صحیح		do_isalpha;
    صحیح		trail_comma;

    // Parses the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    // Each option is a list of characters, character numbers or ranges,
    // separated by commas, e.g.: "200-210,x,#-178,-"
    جب (*p)
    {
	tilde = FALSE;
	do_isalpha = FALSE;
	اگر (*p == '^' && p[1] != NUL)
	{
	    tilde = TRUE;
	    ++p;
	}
	اگر (VIM_ISDIGIT(*p))
	    c = getdigits(&p);
	ورنہ اگر (has_mbyte)
	    c = mb_ptr2char_adv(&p);
	ورنہ
	    c = *p++;
	c2 = -1;
	اگر (*p == '-' && p[1] != NUL)
	{
	    ++p;
	    اگر (VIM_ISDIGIT(*p))
		c2 = getdigits(&p);
	    ورنہ اگر (has_mbyte)
		c2 = mb_ptr2char_adv(&p);
	    ورنہ
		c2 = *p++;
	}
	اگر (c <= 0 || c >= 256 || (c2 < c && c2 != -1) || c2 >= 256
						  || !(*p == NUL || *p == ','))
	    واپس FAIL;

	trail_comma = *p == ',';
	p = skip_مقصد_option_part(p);
	اگر (trail_comma && *p == NUL)
	    // Trailing comma is not allowed.
	    واپس FAIL;

	اگر (only_check)
	    جاری;

	اگر (c2 == -1)	// not a range
	{
	    /*
	     * A single '@' (not "@-@"):
	     * Decide on letters being ID/printable/keyword chars with
	     * standard function isalpha(). This takes care of locale for
	     * single-byte characters).
	     */
	    اگر (c == '@')
	    {
		do_isalpha = TRUE;
		c = 1;
		c2 = 255;
	    }
	    ورنہ
		c2 = c;
	}

	جب (c <= c2)
	{
	    // Use the MB_ functions here, because isalpha() doesn't
	    // work properly when 'encoding' is "latin1" and the locale is
	    // "C".
	    اگر (!do_isalpha || MB_ISLOWER(c) || MB_ISUPPER(c))
	    {
		اگر (var == p_isi)			// (re)set ID flag
		{
		    اگر (tilde)
			g_chartab[c] &= ~CT_ID_CHAR;
		    ورنہ
			g_chartab[c] |= CT_ID_CHAR;
		}
		ورنہ اگر (var == p_isp)			// (re)set printable
		{
		    اگر ((c < ' ' || c > '~'
			    // For double-byte we keep the cell width, so
			    // that we can detect it from the first byte.
			) && !(enc_dbcs && MB_BYTE2LEN(c) == 2))
		    {
			اگر (tilde)
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_نقاب)
					 + ((dy_flags & DY_UHEX) ? 4 : 2);
			    g_chartab[c] &= ~CT_PRINT_CHAR;
			}
			ورنہ
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_نقاب) + 1;
			    g_chartab[c] |= CT_PRINT_CHAR;
			}
		    }
		}
		ورنہ اگر (var == p_isf)			// (re)set fname flag
		{
		    اگر (tilde)
			g_chartab[c] &= ~CT_FNAME_CHAR;
		    ورنہ
			g_chartab[c] |= CT_FNAME_CHAR;
		}
		ورنہ // var == p_isk || var == buf->b_p_isk
							// (re)set keyword flag
		{
		    اگر (tilde)
			RESET_CHARTAB(buf, c);
		    ورنہ
			سیٹ_CHARTAB(buf, c);
		}
	    }
	    ++c;
	}
    }

    واپس OK;
}

/*
 * Translate any special characters in buf[bufsize] in-place.
 * The result is a string with only printable characters, but if there is not
 * enough room, not all characters will be translated.
 */
    خالی
trans_characters(
    char_u	*buf,
    صحیح		bufsize)
{
    صحیح		len;		// length of string needing translation
    صحیح		گنجائش;		// room in buffer after string
    char_u	*trs;		// translated character
    صحیح		trs_len;	// length of trs[]

    len = (صحیح)STRLEN(buf);
    گنجائش = bufsize - len;
    جب (*buf != 0)
    {
	// Assume a multi-byte character doesn't need translation.
	اگر (has_mbyte && (trs_len = (*mb_ptr2len)(buf)) > 1)
	    len -= trs_len;
	ورنہ
	{
	    trs = transchar_byte(*buf);
	    trs_len = (صحیح)STRLEN(trs);
	    اگر (trs_len > 1)
	    {
		گنجائش -= trs_len - 1;
		اگر (گنجائش <= 0)
		    واپس;
		mch_memmove(buf + trs_len, buf + 1, (size_t)len);
	    }
	    mch_memmove(buf, trs, (size_t)trs_len);
	    --len;
	}
	buf += trs_len;
    }
}

/*
 * Translate a string into allocated memory, replacing special chars with
 * printable chars.  Returns NULL when out of memory.
 */
    char_u *
transstr(char_u *s)
{
    char_u	*res;
    char_u	*p;
    صحیح		l, len, c;
    char_u	hexbuf[11];

    اگر (has_mbyte)
    {
	// Compute the length of the result, taking account of unprintable
	// multi-byte characters.
	len = 0;
	p = s;
	جب (*p != NUL)
	{
	    اگر ((l = (*mb_ptr2len)(p)) > 1)
	    {
		c = (*mb_ptr2char)(p);
		p += l;
		اگر (vim_isprintc(c))
		    len += l;
		ورنہ
		{
		    transchar_hex(hexbuf, c);
		    len += (صحیح)STRLEN(hexbuf);
		}
	    }
	    ورنہ
	    {
		l = byte2cells(*p++);
		اگر (l > 0)
		    len += l;
		ورنہ
		    len += 4;	// illegal byte sequence
	    }
	}
	res = alloc(len + 1);
    }
    ورنہ
	res = alloc(vim_strsize(s) + 1);

    اگر (res == تہی)
	واپس تہی;

    char_u *d = res;

    p = s;
    جب (*p != NUL)
    {
	اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    c = (*mb_ptr2char)(p);
	    اگر (vim_isprintc(c))
	    {
		mch_memmove(d, p, (size_t)l);
		d += l;
	    }
	    ورنہ
	    {
		transchar_hex(d, c);
		d += STRLEN(d);
	    }
	    p += l;
	}
	ورنہ
	{
	    char_u	*trs = transchar_byte(*p++);
	    size_t	trs_len = STRLEN(trs);

	    mch_memmove(d, trs, trs_len);
	    d += trs_len;
	}
    }
    *d = NUL;
    واپس res;
}

/*
 * Convert the string "str[orglen]" to do ignore-case comparing.  Uses the
 * current locale.
 * When "buf" is NULL returns an allocated string (NULL for out-of-memory).
 * Otherwise puts the result in "buf[buflen]".
 */
    char_u *
str_foldcase(
    char_u	*str,
    صحیح		orglen,
    char_u	*buf,
    صحیح		buflen)
{
    garray_T	ga;
    صحیح		i;
    صحیح		len = orglen;

#تعریف_کرو GA_CHAR(i)  ((char_u *)ga.ga_ڈیٹا)[i]
#تعریف_کرو GA_PTR(i)   ((char_u *)ga.ga_ڈیٹا + (i))
#تعریف_کرو STR_CHAR(i)  (buf == تہی ? GA_CHAR(i) : buf[i])
#تعریف_کرو STR_PTR(i)   (buf == تہی ? GA_PTR(i) : buf + (i))

    // Copy "str" into "buf" or allocated memory, unmodified.
    اگر (buf == تہی)
    {
	ga_init2(&ga, 1, 10);
	اگر (ga_grow(&ga, len + 1) == FAIL)
	    واپس تہی;
	mch_memmove(ga.ga_ڈیٹا, str, (size_t)len);
	ga.ga_len = len;
    }
    ورنہ
    {
	اگر (len >= buflen)	    // Ugly!
	    len = buflen - 1;
	mch_memmove(buf, str, (size_t)len);
    }
    اگر (buf == تہی)
	GA_CHAR(len) = NUL;
    ورنہ
	buf[len] = NUL;

    // Make each character lower case.
    i = 0;
    جب (STR_CHAR(i) != NUL)
    {
	اگر (enc_utf8 || (has_mbyte && MB_BYTE2LEN(STR_CHAR(i)) > 1))
	{
	    اگر (enc_utf8)
	    {
		صحیح	c = utf_ptr2char(STR_PTR(i));
		صحیح	olen = utf_ptr2len(STR_PTR(i));
		صحیح	lc = utf_tolower(c);

		// Only replace the character when it is not an invalid
		// sequence (ASCII character or more than one byte) and
		// utf_tolower() doesn't return the original character.
		اگر ((c < 0x80 || olen > 1) && c != lc)
		{
		    صحیح	    nlen = utf_char2len(lc);

		    // If the byte length changes need to shift the following
		    // characters forward or backward.
		    اگر (olen != nlen)
		    {
			اگر (nlen > olen)
			{
			    اگر (buf == تہی
				    ? ga_grow(&ga, nlen - olen + 1) == FAIL
				    : len + nlen - olen >= buflen)
			    {
				// out of memory, keep old char
				lc = c;
				nlen = olen;
			    }
			}
			اگر (olen != nlen)
			{
			    اگر (buf == تہی)
			    {
				STRMOVE(GA_PTR(i) + nlen, GA_PTR(i) + olen);
				ga.ga_len += nlen - olen;
			    }
			    ورنہ
			    {
				STRMOVE(buf + i + nlen, buf + i + olen);
				len += nlen - olen;
			    }
			}
		    }
		    (خالی)utf_char2bytes(lc, STR_PTR(i));
		}
	    }
	    // skip to next multi-byte char
	    i += (*mb_ptr2len)(STR_PTR(i));
	}
	ورنہ
	{
	    اگر (buf == تہی)
		GA_CHAR(i) = TOLOWER_LOC(GA_CHAR(i));
	    ورنہ
		buf[i] = TOLOWER_LOC(buf[i]);
	    ++i;
	}
    }

    اگر (buf == تہی)
	واپس (char_u *)ga.ga_ڈیٹا;
    واپس buf;
}

/*
 * Catch 22: g_chartab[] can't be initialized before the options are
 * initialized, and initializing options may cause transchar() to be called!
 * When chartab_initialized == FALSE don't use g_chartab[].
 * Does NOT work for multi-byte characters, c must be <= 255.
 * Also doesn't work for the first byte of a multi-byte, "c" must be a
 * character!
 */
ساکن char_u	transchar_charbuf[7];

    char_u *
transchar(صحیح c)
{
    واپس transchar_buf(curbuf, c);
}

    char_u *
transchar_buf(buf_T *buf, صحیح c)
{
    صحیح			i;

    i = 0;
    اگر (IS_SPECIAL(c))	    // special key code, display as ~@ char
    {
	transchar_charbuf[0] = '~';
	transchar_charbuf[1] = '@';
	i = 2;
	c = K_دوسرا(c);
    }

    اگر ((!chartab_initialized && ((c >= ' ' && c <= '~')))
					|| (c < 256 && vim_isprintc_strict(c)))
    {
	// printable character
	transchar_charbuf[i] = c;
	transchar_charbuf[i + 1] = NUL;
    }
    ورنہ
	transchar_nonprint(buf, transchar_charbuf + i, c);
    واپس transchar_charbuf;
}

/*
 * Like transchar(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of the current buffer.
 */
    char_u *
transchar_byte(صحیح c)
{
    واپس transchar_byte_buf(curbuf, c);
}

/*
 * Like transchar_buf(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of "buf", unless it is NULL.
 */
    char_u *
transchar_byte_buf(buf_T *buf, صحیح c)
{
    اگر (enc_utf8 && c >= 0x80)
    {
	transchar_nonprint(buf, transchar_charbuf, c);
	واپس transchar_charbuf;
    }
    واپس transchar_buf(buf, c);
}
/*
 * Convert non-printable character to two or more printable characters in
 * "charbuf[]".  "charbuf" needs to be able to hold five bytes.
 * Does NOT work for multi-byte characters, c must be <= 255.
 */
    خالی
transchar_nonprint(buf_T *buf, char_u *charbuf, صحیح c)
{
    اگر (c == NL)
	c = NUL;		// we use newline in place of a NUL
    ورنہ اگر (buf != تہی && c == CAR && get_fileformat(buf) == EOL_MAC)
	c = NL;			// we use CR in place of  NL in this case

    اگر (dy_flags & DY_UHEX)		// 'display' has "uhex"
	transchar_hex(charbuf, c);

    ورنہ اگر (c <= 0x7f)			// 0x00 - 0x1f and 0x7f
    {
	charbuf[0] = '^';
	charbuf[1] = c ^ 0x40;		// DEL displayed as ^?
	charbuf[2] = NUL;
    }
    ورنہ اگر (enc_utf8)
    {
	transchar_hex(charbuf, c);
    }
    ورنہ اگر (c >= ' ' + 0x80 && c <= '~' + 0x80)    // 0xa0 - 0xfe
    {
	charbuf[0] = '|';
	charbuf[1] = c - 0x80;
	charbuf[2] = NUL;
    }
    ورنہ					    // 0x80 - 0x9f and 0xff
    {
	charbuf[0] = '~';
	charbuf[1] = (c - 0x80) ^ 0x40;	// 0xff displayed as ~?
	charbuf[2] = NUL;
    }
}

    خالی
transchar_hex(char_u *buf, صحیح c)
{
    صحیح		i = 0;

    buf[0] = '<';
    اگر (c > 255)
    {
	buf[++i] = nr2hex((غیرعلامتی)c >> 12);
	buf[++i] = nr2hex((غیرعلامتی)c >> 8);
    }
    buf[++i] = nr2hex((غیرعلامتی)c >> 4);
    buf[++i] = nr2hex((غیرعلامتی)c);
    buf[++i] = '>';
    buf[++i] = NUL;
}

/*
 * Convert the lower 4 bits of byte "c" to its hex character.
 * Lower case letters are used to avoid the confusion of <F1> being 0xf1 or
 * function key 1.
 */
    ساکن غیرعلامتی
nr2hex(غیرعلامتی c)
{
    اگر ((c & 0xf) <= 9)
	واپس (c & 0xf) + '0';
    واپس (c & 0xf) - 10 + 'a';
}

/*
 * Return number of display cells occupied by byte "b".
 * Caller must make sure 0 <= b <= 255.
 * For multi-byte mode "b" must be the first byte of a character.
 * A TAB is counted as two cells: "^I".
 * For UTF-8 mode this will return 0 for bytes >= 0x80, because the number of
 * cells depends on further bytes.
 */
    صحیح
byte2cells(صحیح b)
{
    اگر (enc_utf8 && b >= 0x80)
	واپس 0;
    واپس (g_chartab[b] & CT_CELL_نقاب);
}

/*
 * Return number of display cells occupied by character "c".
 * "c" can be a special key (negative number) in which case 3 or 4 is returned.
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    صحیح
char2cells(صحیح c)
{
    اگر (IS_SPECIAL(c))
	واپس char2cells(K_دوسرا(c)) + 2;
    اگر (c >= 0x80)
    {
	// UTF-8: above 0x80 need to check the value
	اگر (enc_utf8)
	    واپس utf_char2cells(c);
	// DBCS: double-byte means double-width, except for euc-jp with first
	// byte 0x8e
	اگر (enc_dbcs != 0 && c >= 0x100)
	{
	    اگر (enc_dbcs == DBCS_JPNU && ((غیرعلامتی)c >> 8) == 0x8e)
		واپس 1;
	    واپس 2;
	}
    }
    واپس (g_chartab[c & 0xff] & CT_CELL_نقاب);
}

/*
 * Return number of display cells occupied by character at "*p".
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    صحیح
ptr2cells(char_u *p)
{
    اگر (!has_mbyte)
	واپس byte2cells(*p);

    // For UTF-8 we need to look at more bytes if the first byte is >= 0x80.
    اگر (enc_utf8 && *p >= 0x80)
	واپس utf_ptr2cells(p);
    // For DBCS we can tell the cell count from the first byte.
    واپس (g_chartab[*p] & CT_CELL_نقاب);
}

/*
 * Return the number of character cells string "s" will take on the screen,
 * counting TABs as two characters: "^I".
 */
    صحیح
vim_strsize(char_u *s)
{
    واپس vim_strnsize(s, (صحیح)MAXCOL);
}

/*
 * Return the number of character cells string "s[len]" will take on the
 * screen, counting TABs as two characters: "^I".
 */
    صحیح
vim_strnsize(char_u *s, صحیح len)
{
    صحیح		سائز = 0;

    جب (*s != NUL && --len >= 0)
    {
	صحیح	    l = (*mb_ptr2len)(s);

	سائز += ptr2cells(s);
	s += l;
	len -= l - 1;
    }

    واپس سائز;
}

/*
 * Return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab.
 * Use a define to make it fast, this is used very often!!!
 * Also see getvcol() below.
 */

#اگر_تعریف_شدہ FEAT_VARTABS
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, ستون) \
    اگر (*(p) == TAB && (!(wp)->w_p_list || (wp)->w_lcs_chars.tab1)) \
    { \
	واپس tabstop_padding(ستون, (buf)->b_p_ts, (buf)->b_p_vts_array); \
    } \
    ورنہ \
	واپس ptr2cells(p);
#ورنہ
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, ستون) \
    اگر (*(p) == TAB && (!(wp)->w_p_list || wp->w_lcs_chars.tab1)) \
    { \
	صحیح ts; \
	ts = (buf)->b_p_ts; \
	واپس (صحیح)(ts - (ستون % ts)); \
    } \
    ورنہ \
	واپس ptr2cells(p);
#اگر_ختم

    صحیح
chartabsize(char_u *p, colnr_T ستون)
{
    RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, p, ستون)
}

#اگر defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    صحیح
win_chartabsize(win_T *wp, char_u *p, colnr_T ستون)
{
    RET_WIN_BUF_CHARTABSIZE(wp, wp->w_بفر, p, ستون)
}
#اگر_ختم

/*
 * Return the number of characters the string "s" will take on the screen,
 * taking into account the size of a tab.
 * Does not handle text properties, since "s" is not a buffer line.
 */
    صحیح
linetabsize_str(char_u *s)
{
    واپس linetabsize_ستون(0, s);
}

/*
 * Like linetabsize_str(), but "s" starts at column "startcol".
 */
    صحیح
linetabsize_ستون(صحیح startcol, char_u *s)
{
    chartabsize_T cts;
    vimlong_T vcol;

    init_chartabsize_arg(&cts, curwin, 0, startcol, s, s);
    vcol = cts.cts_vcol;

    جب (*cts.cts_ptr != NUL)
    {
	vcol += lbr_chartabsize_adv(&cts);
	اگر (vcol > MAXCOL)
	{
	    cts.cts_vcol = MAXCOL;
	    توڑو;
	}
	ورنہ
	    cts.cts_vcol = (صحیح)vcol;
    }
    clear_chartabsize_arg(&cts);
    واپس (صحیح)cts.cts_vcol;
}

/*
 * Like linetabsize_str(), but for a given window instead of the current one.
 * Doesn't count the size of 'listchars' "eol".
 */
    صحیح
win_linetabsize(win_T *wp, linenr_T lnum, char_u *سطر, colnr_T len)
{
    chartabsize_T cts;

    init_chartabsize_arg(&cts, wp, lnum, 0, سطر, سطر);
    win_linetabsize_cts(&cts, len);
    clear_chartabsize_arg(&cts);
    واپس (صحیح)cts.cts_vcol;
}

/*
 * Return the number of cells line "lnum" of window "wp" will take on the
 * screen, taking into account the size of a tab and text properties.
 * Doesn't count the size of 'listchars' "eol".
 */
    صحیح
linetabsize(win_T *wp, linenr_T lnum)
{
    واپس win_linetabsize(wp, lnum,
		       ml_get_buf(wp->w_بفر, lnum, FALSE), (colnr_T)MAXCOL);
}

/*
 * Like linetabsize(), but counts the size of 'listchars' "eol".
 */
    صحیح
linetabsize_eol(win_T *wp, linenr_T lnum)
{
    واپس linetabsize(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Like linetabsize(), but excludes 'above'/'after'/'right'/'below' aligned
 * virtual text, while keeping inline virtual text.
 */
    صحیح
linetabsize_no_outer(win_T *wp, linenr_T lnum)
{
#اگر_غیر_تعریف_شدہ FEAT_PROP_POPUP
    واپس linetabsize(wp, lnum);
#ورنہ
    chartabsize_T cts;
    char_u *سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, سطر, سطر);

    اگر (cts.cts_متن_prop_گنتی)
    {
	صحیح write_idx = 0;
	برائے (صحیح read_idx = 0; read_idx < cts.cts_متن_prop_گنتی; read_idx++)
	{
	    textprop_T *tp = &cts.cts_متن_props[read_idx];
	    اگر (tp->tp_ستون != MAXCOL)
	    {
		اگر (read_idx != write_idx)
		    cts.cts_متن_props[write_idx] = *tp;
		write_idx++;
	    }
	}
	cts.cts_متن_prop_گنتی = write_idx;
	اگر (cts.cts_متن_prop_گنتی == 0)
	    VIM_CLEAR(cts.cts_متن_props);
    }

    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    واپس (صحیح)cts.cts_vcol;
#اگر_ختم
}

/*
 * Like linetabsize_no_outer(), but counts the size of 'listchars' "eol".
 */
    صحیح
linetabsize_no_outer_eol(win_T *wp, linenr_T lnum)
{
    واپس linetabsize_no_outer(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Return TRUE when win_lbr_chartabsize() does nothing more than
 * win_nolbr_chartabsize(): no 'linebreak', 'breakindent', 'showbreak' and no
 * text properties that insert text.
 */
#اگر defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ساکن صحیح
win_lbr_is_simple_چوڑائی(chartabsize_T *cts)
{
    واپس TRUE
# ifdef FEAT_LINEBREAK
	&& !cts->cts_win->w_p_lbr && !cts->cts_win->w_p_bri
	&& *get_showbreak_قدر(cts->cts_win) == NUL
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_متن
# endif
	;
}
#اگر_ختم

    خالی
win_linetabsize_cts(chartabsize_T *cts, colnr_T len)
{
    vimlong_T vcol = cts->cts_vcol;
#اگر defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    // Fast path: when win_lbr_chartabsize() would just return
    // win_nolbr_chartabsize() (wrap on; no 'linebreak'/'breakindent'/
    // 'showbreak'/inserted text properties), call it directly.
    صحیح simple_چوڑائی = cts->cts_win->w_p_wrap && win_lbr_is_simple_چوڑائی(cts);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    cts->cts_with_trailing = len == MAXCOL;
#اگر_ختم
    برائے ( ; *cts->cts_ptr != NUL && (len == MAXCOL || cts->cts_ptr < cts->cts_سطر + len);
						      MB_PTR_ADV(cts->cts_ptr))
    {
#اگر defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
	اگر (simple_چوڑائی)
	{
	    صحیح	c = *cts->cts_ptr;

	    اگر (c < 0x80 && c != TAB)
		vcol += g_chartab[c] & CT_CELL_نقاب;
	    ورنہ
		vcol += win_nolbr_chartabsize(cts, تہی);
	}
	ورنہ
#اگر_ختم
	    vcol += win_lbr_chartabsize(cts, تہی, تہی);
	اگر (vcol > MAXCOL)
	{
	    cts->cts_vcol = MAXCOL;
	    توڑو;
	}
	ورنہ
	    cts->cts_vcol = (صحیح)vcol;
    }
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // check for a virtual text at the end of a line or on an empty line
    اگر (len == MAXCOL && cts->cts_has_prop_with_متن && *cts->cts_ptr == NUL)
    {
	صحیح سر = 0;
	(خالی)win_lbr_chartabsize(cts, &سر, تہی);
	vcol += cts->cts_cur_متن_چوڑائی + سر;
	// When properties are above the empty line must also be counted.  For
	// a property below the width already includes filling up the line.
	اگر (cts->cts_ptr == cts->cts_سطر && cts->cts_prop_سطریں > 0
							 && !cts->cts_has_نیچے)
	    ++vcol;
	cts->cts_vcol = vcol > MAXCOL ? MAXCOL : (صحیح)vcol;
    }
#اگر_ختم
}

/*
 * Return TRUE if 'c' is a normal identifier character:
 * Letters and characters from the 'isident' option.
 */
    صحیح
vim_isIDc(صحیح c)
{
    واپس (c > 0 && c < 0x100 && (g_chartab[c] & CT_ID_CHAR));
}

/*
 * Like vim_isIDc() but not using the 'isident' option: letters, numbers and
 * underscore.
 */
    صحیح
vim_is_معمول_IDc(صحیح c)
{
    واپس ASCII_ISALNUM(c) || c == '_';
}

/*
 * return TRUE if 'c' is a keyword character: Letters and characters from
 * 'iskeyword' option for the current buffer.
 * For multi-byte characters mb_get_class() is used (builtin rules).
 */
    صحیح
vim_iswordc(صحیح c)
{
    واپس vim_iswordc_buf(c, curbuf);
}

    صحیح
vim_iswordc_buf(صحیح c, buf_T *buf)
{
    اگر (c >= 0x100)
    {
	اگر (enc_dbcs != 0)
	    واپس dbcs_class((غیرعلامتی)c >> 8, (غیرعلامتی)(c & 0xff)) >= 2;
	اگر (enc_utf8)
	    واپس utf_class_buf(c, buf) >= 2;
	واپس FALSE;
    }
    واپس (c > 0 && GET_CHARTAB(buf, c) != 0);
}

/*
 * Just like vim_iswordc() but uses a pointer to the (multi-byte) character.
 */
    صحیح
vim_iswordp(char_u *p)
{
    واپس vim_iswordp_buf(p, curbuf);
}

    صحیح
vim_iswordp_buf(char_u *p, buf_T *buf)
{
    صحیح	c = *p;

    اگر (has_mbyte && MB_BYTE2LEN(c) > 1)
	c = (*mb_ptr2char)(p);
    واپس vim_iswordc_buf(c, buf);
}

/*
 * Return TRUE if 'c' is a valid file-name character as specified with the
 * 'isfname' option.
 * Assume characters above 0x100 are valid (multi-byte).
 * To be used for commands like "gf".
 */
    صحیح
vim_isfilec(صحیح c)
{
    واپس (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_FNAME_CHAR)));
}

#اگر defined(FEAT_SPELL)
/*
 * Return TRUE if 'c' is a valid file-name character, including characters left
 * out of 'isfname' to make "gf" work, such as comma, space, '@', etc.
 */
    صحیح
vim_is_fname_char(صحیح c)
{
    واپس vim_isfilec(c) || c == ',' || c == ' ' || c == '@';
}
#اگر_ختم

/*
 * return TRUE if 'c' is a valid file-name character or a wildcard character
 * Assume characters above 0x100 are valid (multi-byte).
 * Explicitly interpret ']' as a wildcard character as mch_has_wildcard("]")
 * returns false.
 */
    صحیح
vim_isfilec_or_wc(صحیح c)
{
    char_u buf[2];

    buf[0] = (char_u)c;
    buf[1] = NUL;
    واپس vim_isfilec(c) || c == ']' || mch_has_wildcard(buf);
}

/*
 * Return TRUE if 'c' is a printable character.
 * Assume characters above 0x100 are printable (multi-byte), except for
 * Unicode.
 */
    صحیح
vim_isprintc(صحیح c)
{
    اگر (enc_utf8 && c >= 0x100)
	واپس utf_printable(c);
    واپس (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Strict version of vim_isprintc(c), don't return TRUE if "c" is the head
 * byte of a double-byte character.
 */
    صحیح
vim_isprintc_strict(صحیح c)
{
    اگر (enc_dbcs != 0 && c < 0x100 && MB_BYTE2LEN(c) > 1)
	واپس FALSE;
    اگر (enc_utf8 && c >= 0x100)
	واپس utf_printable(c);
    واپس (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Prepare the structure passed to chartabsize functions.
 * "line" is the start of the line, "ptr" is the first relevant character.
 * When "lnum" is zero do not use text properties that insert text.
 */
    خالی
init_chartabsize_arg(
	chartabsize_T	*cts,
	win_T		*wp,
	linenr_T	lnum غیر_مستعمل,
	colnr_T		ستون,
	char_u		*سطر,
	char_u		*ptr)
{
    CLEAR_اشارہ_گر(cts);
    cts->cts_win = wp;
    cts->cts_vcol = ستون;
    cts->cts_سطر = سطر;
    cts->cts_ptr = ptr;
#اگر_تعریف_شدہ FEAT_LINEBREAK
    cts->cts_bri_سائز = -1;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (lnum > 0 && !ignore_متن_props)
    {
	char_u	*prop_آغاز;
	صحیح	گنتی;

	گنتی = get_متن_props(wp->w_بفر, lnum, &prop_آغاز, FALSE);
	cts->cts_متن_prop_گنتی = گنتی;
	اگر (گنتی > 0)
	{
	    // Make a copy of the properties, so that they are properly
	    // aligned.  Make it twice as long for the sorting below.
	    cts->cts_متن_props = ALLOC_MULT(textprop_T, گنتی * 2);
	    اگر (cts->cts_متن_props == تہی)
		cts->cts_متن_prop_گنتی = 0;
	    ورنہ
	    {
		صحیح	i;

		mch_memmove(cts->cts_متن_props + گنتی, prop_آغاز,
						   گنتی * حجم(textprop_T));
		برائے (i = 0; i < گنتی; ++i)
		{
		    textprop_T *tp = cts->cts_متن_props + i + گنتی;
		    اگر (tp->tp_id < 0
				     && متن_prop_type_valid(wp->w_بفر, tp))
		    {
			cts->cts_has_prop_with_متن = TRUE;
			توڑو;
		    }
		}
		اگر (!cts->cts_has_prop_with_متن)
		{
		    // won't use the text properties, free them
		    VIM_CLEAR(cts->cts_متن_props);
		    cts->cts_متن_prop_گنتی = 0;
		}
		ورنہ
		{
		    صحیح	    *متن_prop_idxs;

		    // Need to sort the array to get any truncation right.
		    // Do the sorting in the second part of the array, then
		    // move the sorted props to the first part of the array.
		    متن_prop_idxs = ALLOC_MULT(صحیح, گنتی);
		    اگر (متن_prop_idxs != تہی)
		    {
			برائے (i = 0; i < گنتی; ++i)
			    متن_prop_idxs[i] = i + گنتی;
			ترتیب_دو_متن_props(curbuf, cts->cts_متن_props,
							متن_prop_idxs, گنتی);
			// Here we want the reverse order.
			برائے (i = 0; i < گنتی; ++i)
			    cts->cts_متن_props[گنتی - i - 1] =
					cts->cts_متن_props[متن_prop_idxs[i]];
			vim_free(متن_prop_idxs);
		    }

		    // Convert tp_text_offset to tp_text pointer.
		    char_u *گنتی_ptr = prop_آغاز - PROP_گنتی_سائز;

		    برائے (i = 0; i < گنتی; ++i)
		    {
			textprop_T *tp = &cts->cts_متن_props[i];

			اگر (tp->tp_id < 0 && tp->u.tp_متن_انحراف > 0)
			{
			    tp->u.tp_متن = گنتی_ptr + tp->u.tp_متن_انحراف;
			    tp->tp_flags |= TP_FLAG_VTEXT_PTR;
			}
			ورنہ
			    tp->u.tp_متن = تہی;
		    }
		}
	    }
	}
    }
#اگر_ختم
}

/*
 * Free any allocated item in "cts".
 */
    خالی
clear_chartabsize_arg(chartabsize_T *cts غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (cts->cts_متن_prop_گنتی > 0)
    {
	VIM_CLEAR(cts->cts_متن_props);
	cts->cts_متن_prop_گنتی = 0;
    }
#اگر_ختم
}

/*
 * Like chartabsize(), but also check for line breaks on the screen and text
 * properties that insert text.
 */
    صحیح
lbr_chartabsize(chartabsize_T *cts)
{
#اگر defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    اگر (1
# ifdef FEAT_LINEBREAK
	&& !curwin->w_p_lbr && *get_showbreak_قدر(curwin) == NUL
							   && !curwin->w_p_bri
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_متن
# endif
       )
    {
#اگر_ختم
	اگر (curwin->w_p_wrap)
	    واپس win_nolbr_chartabsize(cts, تہی);
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, cts->cts_ptr, cts->cts_vcol)
#اگر defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    }
    واپس win_lbr_chartabsize(cts, تہی, تہی);
#اگر_ختم
}

/*
 * Call lbr_chartabsize() and advance the pointer.
 */
    صحیح
lbr_chartabsize_adv(chartabsize_T *cts)
{
    صحیح		retval;

    retval = lbr_chartabsize(cts);
    MB_PTR_ADV(cts->cts_ptr);
    واپس retval;
}

/*
 * Return the screen size of the character indicated by "cts".
 * "cts->cts_cur_text_width" is set to the extra size for a text property that
 * inserts text.
 * This function is used very often, keep it fast!!!!
 *
 * If "headp" isn't NULL, set "*headp" to the size of 'showbreak'/'breakindent'
 * included in the return value.
 * When "cts->cts_max_head_vcol" is positive, only count in "*headp" the size
 * of 'showbreak'/'breakindent' before "cts->cts_max_head_vcol".
 * When "cts->cts_max_head_vcol" is negative, only count in "*headp" the size
 * of 'showbreak'/'breakindent' before where cursor should be placed.
 *
 * If "tailp" isn't NULL, set "*tailp" to the size of 'linebreak' included in
 * the return value.
 *
 * Warning: "*headp" and "*tailp" may not be set if the value is 0, init to 0
 * before calling.
 */
    صحیح
win_lbr_chartabsize(
	chartabsize_T	*cts,
	صحیح		*headp غیر_مستعمل,
	صحیح		*tailp غیر_مستعمل)
{
    win_T	*wp = cts->cts_win;
#اگر defined(FEAT_PROP_POPUP) || defined(FEAT_LINEBREAK)
    char_u	*سطر = cts->cts_سطر; // start of the line
    صحیح		سائز;
#اگر_ختم
    char_u	*s = cts->cts_ptr;
    colnr_T	vcol = cts->cts_vcol;
#اگر_تعریف_شدہ FEAT_LINEBREAK
    صحیح		mb_added = 0;
    صحیح		n;
    char_u	*sbr;
    صحیح		no_sbr = FALSE;
#اگر_ختم

#اگر defined(FEAT_PROP_POPUP)
    cts->cts_cur_متن_چوڑائی = 0;
    cts->cts_has_نیچے = false;
    cts->cts_پہلا_char = 0;
#اگر_ختم

#اگر defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    /*
     * No 'linebreak', 'showbreak', 'breakindent' and text properties that
     * insert text: return quickly.
     */
    اگر (win_lbr_is_simple_چوڑائی(cts))
#اگر_ختم
    {
	اگر (wp->w_p_wrap)
	    واپس win_nolbr_chartabsize(cts, headp);
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_بفر, s, vcol)
    }

#اگر defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    صحیح has_lcs_eol = wp->w_p_list && wp->w_lcs_chars.eol != NUL;
    // Virtual text above the line is on its own screen line, it does not count
    // for the size of a Tab.
    colnr_T tab_vcol = vcol;

# ifdef FEAT_PROP_POPUP
    tab_vcol -= cts->cts_above_چوڑائی;
# endif

    /*
     * First get the normal size, without 'linebreak' or text properties
     */
    سائز = win_chartabsize(wp, s, tab_vcol);
# ifdef FEAT_LINEBREAK
    اگر (*s == NUL)
    {
	// 1 cell for EOL list char (if present), as opposed to the two cell ^@
	// for a NUL character in the text.
	سائز = has_lcs_eol ? 1 : 0;
    }

    صحیح is_doublewidth = has_mbyte && سائز == 2 && MB_BYTE2LEN(*s) > 1;
# endif

# ifdef FEAT_PROP_POPUP
    اگر (cts->cts_has_prop_with_متن)
    {
#  ifdef FEAT_LINEBREAK
	صحیح	    tab_سائز = سائز;
#  endif
	صحیح	    charlen = *s == NUL ? 1 : mb_ptr2len(s);
	صحیح	    i;
	صحیح	    ستون = (صحیح)(s - سطر);


	// The "$" for 'list' mode will go between the EOL and
	// the text prop, account for that.
	اگر (has_lcs_eol)
	{
	    ++vcol;
	    --سائز;
	}

	برائے (i = 0; i < cts->cts_متن_prop_گنتی; ++i)
	{
	    textprop_T	*tp = cts->cts_متن_props + i;
	    صحیح		ستون_off = win_ستون_off(wp);

	    // Watch out for the text being deleted.  "cts_text_props" is a
	    // copy, the text prop may actually have been removed from the line.
	    اگر (tp->tp_id < 0
		    && ((tp->tp_ستون - 1 >= ستون
					     && tp->tp_ستون - 1 < ستون + charlen)
		       || (tp->tp_ستون == MAXCOL
			   && ((tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
				? ستون == 0
				: s[0] == NUL && cts->cts_with_trailing)))
		    && tp->u.tp_متن != تہی)
	    {
		char_u *p = tp->u.tp_متن;

		اگر (p != تہی)
		{
		    صحیح	cells;

		    اگر (tp->tp_ستون == MAXCOL)
		    {
			صحیح n_extra = (صحیح)STRLEN(p);

			cells = متن_prop_position(wp, tp, vcol,
			     (vcol + سائز) % (wp->w_چوڑائی - ستون_off) + ستون_off,
					      &n_extra, &p, تہی, تہی, FALSE);
#  ifdef FEAT_LINEBREAK
			اگر (متن_prop_no_showbreak(tp))
			    no_sbr = TRUE;  // don't use 'showbreak' now
#  endif
		    }
		    ورنہ
			cells = vim_strsize(p);
		    اگر (tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
		    {
			cts->cts_پہلا_char += cells;
			اگر (!cts->cts_no_above)
			    cts->cts_cur_متن_چوڑائی += cells;
		    }
		    ورنہ
		    {
			cts->cts_cur_متن_چوڑائی += cells;
			سائز += cells;
		    }
		    cts->cts_آغاز_incl = tp->tp_flags & TP_FLAG_آغاز_INCL;
#  ifdef FEAT_LINEBREAK
		    اگر (*s == TAB)
		    {
			// tab size changes because of the inserted text
			سائز -= tab_سائز;
			tab_سائز = win_chartabsize(wp, s,
					vcol + سائز - cts->cts_above_چوڑائی);
			سائز += tab_سائز;
		    }
#  endif
		    اگر (tp->tp_ستون == MAXCOL && (tp->tp_flags
				& (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_نیچے)))
		    {
			// count extra line for property above/below
			++cts->cts_prop_سطریں;
			اگر (tp->tp_flags & TP_FLAG_ALIGN_نیچے)
			    cts->cts_has_نیچے = true;
		    }
		}
	    }
	    اگر (tp->tp_ستون != MAXCOL && tp->tp_ستون - 1 > ستون)
		توڑو;
	}
	اگر (has_lcs_eol)
	{
	    --vcol;
	    ++سائز;
	}
    }
# endif

# ifdef FEAT_LINEBREAK
    اگر (is_doublewidth && wp->w_p_wrap && اندر_win_border(wp, vcol + سائز - 2))
    {
	++سائز;		// Count the ">" in the last column.
	mb_added = 1;
    }

    /*
     * May have to add something for 'breakindent' and/or 'showbreak'
     * string at the start of a screen line.
     */
    صحیح سر = mb_added;
    sbr = no_sbr ? خالی_خانے_option : get_showbreak_قدر(wp);
    // When "size" is 0, no new screen line is started.
    اگر (سائز > 0 && wp->w_p_wrap && (*sbr != NUL || wp->w_p_bri))
    {
	صحیح	ستون_off_پچھلا = win_ستون_off(wp);
	صحیح	width2 = wp->w_چوڑائی - ستون_off_پچھلا + win_ستون_off2(wp);
	colnr_T	wcol = vcol + ستون_off_پچھلا;
#  ifdef FEAT_PROP_POPUP
	wcol -= wp->w_virtcol_پہلا_char;
#  endif
	colnr_T	max_سر_vcol = cts->cts_max_سر_vcol;
	صحیح	added = 0;

	// cells taken by 'showbreak'/'breakindent' before current char
	صحیح	سر_پچھلا = 0;
	اگر (wcol >= wp->w_چوڑائی)
	{
	    wcol -= wp->w_چوڑائی;
	    ستون_off_پچھلا = wp->w_چوڑائی - width2;
	    اگر (wcol >= width2 && width2 > 0)
		wcol %= width2;
	    اگر (*sbr != NUL)
		سر_پچھلا += vim_strsize(sbr);
	    اگر (wp->w_p_bri)
	    {
		اگر (cts->cts_bri_سائز < 0)
		    cts->cts_bri_سائز = get_breakindent_win(wp, سطر);
		سر_پچھلا += cts->cts_bri_سائز;
	    }
	    اگر (wcol < سر_پچھلا)
	    {
		سر_پچھلا -= wcol;
		wcol += سر_پچھلا;
		added += سر_پچھلا;
		اگر (max_سر_vcol <= 0 || vcol < max_سر_vcol)
		    سر += سر_پچھلا;
	    }
	    ورنہ
		سر_پچھلا = 0;
	    wcol += ستون_off_پچھلا;
	}

	اگر (wcol + سائز > wp->w_چوڑائی)
	{
	    // cells taken by 'showbreak'/'breakindent' halfway current char
	    صحیح	سر_mid = 0;
	    اگر (*sbr != NUL)
		سر_mid += vim_strsize(sbr);
	    اگر (wp->w_p_bri)
	    {
		اگر (cts->cts_bri_سائز < 0)
		    cts->cts_bri_سائز = get_breakindent_win(wp, سطر);
		سر_mid += cts->cts_bri_سائز;
	    }
	    اگر (سر_mid > 0)
	    {
		// Calculate effective window width.
		صحیح پچھلا_rem = wp->w_چوڑائی - wcol;
		صحیح چوڑائی = width2 - سر_mid;

		اگر (چوڑائی <= 0)
		    چوڑائی = 1;
		// Divide "size - prev_rem" by "width", rounding up.
		صحیح cnt = (سائز - پچھلا_rem + چوڑائی - 1) / چوڑائی;
		added += cnt * سر_mid;

		اگر (max_سر_vcol == 0 || vcol + سائز + added < max_سر_vcol)
		    سر += cnt * سر_mid;
		ورنہ اگر (width2 > 0
			   && max_سر_vcol > vcol + سر_پچھلا + پچھلا_rem)
		    سر += (max_سر_vcol - (vcol + سر_پچھلا + پچھلا_rem)
					     + width2 - 1) / width2 * سر_mid;
		ورنہ اگر (max_سر_vcol < 0)
		{
		    صحیح off = mb_added;
#  ifdef FEAT_PROP_POPUP
		    اگر (*s != NUL
			     && ((State & منوال_معمول) || cts->cts_آغاز_incl))
			off += cts->cts_cur_متن_چوڑائی;
#  endif
		    اگر (off >= پچھلا_rem)
			سر += (1 + (off - پچھلا_rem) / چوڑائی) * سر_mid;
		}
	    }
	}

	سائز += added;
    }

    اگر (headp != تہی)
	*headp = سر;

    صحیح سائز_before_lbr = سائز;
    صحیح need_lbr = FALSE;
    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here.
     */
    اگر (wp->w_p_lbr && wp->w_p_wrap && wp->w_چوڑائی != 0
	    && VIM_ISBREAK((صحیح)s[0]) && !VIM_ISBREAK((صحیح)s[1]))
    {
	char_u	*t = cts->cts_سطر;
	جب (VIM_ISBREAK((صحیح)t[0]))
	    t++;
	// 'linebreak' is only needed when not in leading whitespace.
	need_lbr = s >= t;
    }
    اگر (need_lbr)
    {
	/*
	 * Count all characters from first non-blank after a blank up to next
	 * non-blank after a blank.
	 */
	صحیح numberextra = win_ستون_off(wp);
	colnr_T ستون_adj = سائز - 1;
	colnr_T colmax = (colnr_T)(wp->w_چوڑائی - numberextra - ستون_adj);
	اگر (vcol >= colmax)
	{
	    colmax += ستون_adj;
	    n = colmax +  win_ستون_off2(wp);
	    اگر (n > 0)
		colmax += (((vcol + ستون_adj - colmax) / n) + 1) * n - ستون_adj;
	}

	colnr_T vcol2 = vcol;
	برائے (;;)
	{
	    char_u *ps = s;
	    MB_PTR_ADV(s);
	    صحیح c = *s;
	    اگر (!(c != NUL
		    && (VIM_ISBREAK(c)
			|| (!VIM_ISBREAK(c)
			       && (vcol2 == vcol || !VIM_ISBREAK((صحیح)*ps))))))
		توڑو;

	    vcol2 += win_chartabsize(wp, s, vcol2);
	    اگر (vcol2 >= colmax)		// doesn't fit
	    {
		سائز = colmax - vcol + ستون_adj;
		توڑو;
	    }
	}
    }

    اگر (tailp != تہی)
	*tailp = سائز - سائز_before_lbr;

#  ifdef FEAT_PROP_POPUP
    اگر (cts->cts_پہلا_char > 0 && !cts->cts_no_above)
    {
	// Remember the width for the size of a Tab later in the line.  Use
	// assignment, this may be called more than once for a character.
	cts->cts_above_چوڑائی = cts->cts_پہلا_char;
	سائز += cts->cts_پہلا_char;
    }
#  endif
# endif
    واپس سائز;
#اگر_ختم
}

/*
 * Like win_lbr_chartabsize(), except that we know 'linebreak' is off, 'wrap'
 * is on and there are no properties that insert text.  This means we need to
 * check for a double-byte character that doesn't fit at the end of the screen
 * line.
 * Only uses "cts_win", "cts_ptr" and "cts_vcol" from "cts".
 */
    ساکن صحیح
win_nolbr_chartabsize(
	chartabsize_T	*cts,
	صحیح		*headp)
{
    win_T	*wp = cts->cts_win;
    char_u	*s = cts->cts_ptr;
    colnr_T	ستون = cts->cts_vcol;
    صحیح		n;

    اگر (*s == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
    {
#اگر_تعریف_شدہ FEAT_VARTABS
	واپس tabstop_padding(ستون, wp->w_بفر->b_p_ts,
				    wp->w_بفر->b_p_vts_array);
#ورنہ
	n = wp->w_بفر->b_p_ts;
	واپس (صحیح)(n - (ستون % n));
#اگر_ختم
    }
    n = ptr2cells(s);
    // Add one cell for a double-width character in the last column of the
    // window, displayed with a ">".
    اگر (n == 2 && MB_BYTE2LEN(*s) > 1 && اندر_win_border(wp, ستون))
    {
	اگر (headp != تہی)
	    *headp = 1;
	واپس 3;
    }
    واپس n;
}

/*
 * Return TRUE if virtual column "vcol" is in the rightmost column of window
 * "wp".
 */
    ساکن صحیح
اندر_win_border(win_T *wp, colnr_T vcol)
{
    صحیح		width1;		// width of first line (after line number)
    صحیح		width2;		// width of further lines

    اگر (wp->w_چوڑائی == 0)	// there is no border
	واپس FALSE;
    width1 = wp->w_چوڑائی - win_ستون_off(wp);
    اگر ((صحیح)vcol < width1 - 1)
	واپس FALSE;
    اگر ((صحیح)vcol == width1 - 1)
	واپس TRUE;
    width2 = width1 + win_ستون_off2(wp);
    اگر (width2 <= 0)
	واپس FALSE;
    واپس ((vcol - width1) % width2 == width2 - 1);
}

/*
 * Get virtual column number of pos.
 *  start: on the first position of this character (TAB, ctrl)
 * cursor: where the cursor is on this character (first char, except for TAB)
 *    end: on the last position of this character (TAB, ctrl)
 *
 * When 'linebreak' follows this character, "end" is set to the position before
 * 'linebreak' if "flags" contains GETVCOL_END_EXCL_LBR, otherwise it's set to
 * the end of 'linebreak'.
 *
 * This is used very often, keep it fast!
 */
    خالی
getvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*آغاز,
    colnr_T	*cursor,
    colnr_T	*انتہا,
    صحیح		flags)
{
    colnr_T	vcol;
    char_u	*ptr;		// points to current char
    char_u	*سطر;		// start of the line
    صحیح		incr;
    صحیح		سر;
    صحیح		دم;
#اگر_تعریف_شدہ FEAT_VARTABS
    صحیح		*vts = wp->w_بفر->b_p_vts_array;
#اگر_ختم
    صحیح		ts = wp->w_بفر->b_p_ts;
    صحیح		c;
    chartabsize_T cts;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    صحیح		on_NUL = FALSE;
#اگر_ختم

    vcol = 0;
    سطر = ptr = ml_get_buf(wp->w_بفر, pos->lnum, FALSE);

    init_chartabsize_arg(&cts, wp, pos->lnum, 0, سطر, سطر);
    cts.cts_max_سر_vcol = -1;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    cts.cts_no_above = true;
#اگر_ختم

    /*
     * This function is used very often, do some speed optimizations.
     * When 'list', 'linebreak', 'showbreak' and 'breakindent' are not set
     * and there are no text properties with "text" use a simple loop.
     * Also use this when 'list' is set but tabs take their normal size.
     */
    اگر ((!wp->w_p_list || wp->w_lcs_chars.tab1 != NUL)
#اگر_تعریف_شدہ FEAT_LINEBREAK
	    && !wp->w_p_lbr && *get_showbreak_قدر(wp) == NUL && !wp->w_p_bri
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_متن
#اگر_ختم
       )
    {
	برائے (;;)
	{
	    سر = 0;
	    c = *ptr;
	    // make sure we don't go past the end of the line
	    اگر (c == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
		توڑو;
	    }
	    // A tab gets expanded, depending on the current column
	    اگر (c == TAB)
#اگر_تعریف_شدہ FEAT_VARTABS
		incr = tabstop_padding(vcol, ts, vts);
#ورنہ
		incr = ts - (vcol % ts);
#اگر_ختم
	    ورنہ
	    {
		اگر (has_mbyte)
		{
		    // For utf-8, if the byte is >= 0x80, need to look at
		    // further bytes to find the cell width.
		    اگر (enc_utf8 && c >= 0x80)
			incr = utf_ptr2cells(ptr);
		    ورنہ
			incr = g_chartab[c] & CT_CELL_نقاب;

		    // If a double-cell char doesn't fit at the end of a line
		    // it wraps to the next line, it's like this char is three
		    // cells wide.
		    اگر (incr == 2 && wp->w_p_wrap && MB_BYTE2LEN(*ptr) > 1
			    && اندر_win_border(wp, vcol))
		    {
			++incr;
			سر = 1;
		    }
		}
		ورنہ
		    incr = g_chartab[c] & CT_CELL_نقاب;
	    }

	    char_u *اگلا_ptr = ptr + (*mb_ptr2len)(ptr);
	    اگر (اگلا_ptr - سطر > pos->ستون) // character at pos->col
		توڑو;

	    vcol += incr;
	    ptr = اگلا_ptr;
	}

	دم = 0;
    }
    ورنہ
    {
	برائے (;;)
	{
	    // A tab gets expanded, depending on the current column.
	    // Other things also take up space.
	    سر = 0;
	    دم = 0;
	    incr = win_lbr_chartabsize(&cts, &سر, &دم);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    اگر (cts.cts_ptr == cts.cts_سطر)
	    {
		اگر (flags & GETVCOL_FOR_VIRTCOL)
		    // do not count the virtual text above for w_curswant
		    wp->w_virtcol_پہلا_char = cts.cts_پہلا_char;
		اگر ((flags & GETVCOL_NO_ABOVE) == 0)
		{
		    cts.cts_vcol += cts.cts_پہلا_char;
		    cts.cts_above_چوڑائی = cts.cts_پہلا_char;
		}
	    }
#اگر_ختم
	    // make sure we don't go past the end of the line
	    اگر (*cts.cts_ptr == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		incr += cts.cts_cur_متن_چوڑائی;
		on_NUL = TRUE;
#اگر_ختم
		توڑو;
	    }

	    char_u *اگلا_ptr = cts.cts_ptr + (*mb_ptr2len)(cts.cts_ptr);
	    اگر (اگلا_ptr - سطر > pos->ستون) // character at pos->col
		توڑو;

	    cts.cts_vcol += incr;
	    cts.cts_ptr = اگلا_ptr;
	}
	vcol = cts.cts_vcol;
	ptr = cts.cts_ptr;
    }
    clear_chartabsize_arg(&cts);

    اگر (*ptr == NUL && pos->ستون < MAXCOL && pos->ستون > ptr - سطر)
	pos->ستون = ptr - سطر;

    اگر (آغاز != تہی)
	*آغاز = vcol + سر;
    اگر (انتہا != تہی)
	*انتہا = vcol + incr - (flags & GETVCOL_انتہا_EXCL_LBR ? دم : 0) - 1;
    اگر (cursor != تہی)
    {
	اگر (*ptr == TAB
		&& (State & منوال_معمول)
		&& !wp->w_p_list
		&& !virtual_فعال()
		&& !(VIsual_فعال
				&& (*p_sel == 'e' || LTOREQ_POS(*pos, VIsual)))
		)
	    // TODO: subtracting "tail" may lead to better cursor position
	    *cursor = vcol + incr - 1;	    // cursor at end
	ورنہ
	{
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    // in Insert mode, if "start_incl" is true the text gets inserted
	    // after the virtual text, thus add its width
	    اگر (((State & منوال_داخل_کرو) == 0 || cts.cts_آغاز_incl) && !on_NUL)
		// cursor is after inserted text, unless on the NUL
		vcol += cts.cts_cur_متن_چوڑائی;
#اگر_ختم
	    *cursor = vcol + سر;	    // cursor at start
	}
    }
}

/*
 * Get virtual cursor column in the current window, pretending 'list' is off.
 */
    colnr_T
getvcol_nolist(pos_T *posp)
{
    صحیح		list_save = curwin->w_p_list;
    colnr_T	vcol;

    curwin->w_p_list = FALSE;
    اگر (posp->coladd)
	getvvcol(curwin, posp, تہی, &vcol, تہی, 0);
    ورنہ
	getvcol(curwin, posp, تہی, &vcol, تہی, 0);
    curwin->w_p_list = list_save;
    واپس vcol;
}

/*
 * Get virtual column in virtual mode.
 */
    خالی
getvvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*آغاز,
    colnr_T	*cursor,
    colnr_T	*انتہا,
    صحیح		flags)
{
    colnr_T	ستون;
    colnr_T	coladd;
    colnr_T	endadd;
    char_u	*ptr;

    اگر (virtual_فعال())
    {
	// For virtual mode, only want one value
	getvcol(wp, pos, &ستون, تہی, تہی, flags);

	coladd = pos->coladd;
	endadd = 0;
	// Cannot put the cursor on part of a wide character.
	ptr = ml_get_buf(wp->w_بفر, pos->lnum, FALSE);
	اگر (pos->ستون < ml_get_buf_len(wp->w_بفر, pos->lnum))
	{
	    صحیح c = (*mb_ptr2char)(ptr + pos->ستون);

	    اگر (c != TAB && vim_isprintc(c))
	    {
		endadd = (colnr_T)(char2cells(c) - 1);
		اگر (coladd > endadd)	// past end of line
		    endadd = 0;
		ورنہ
		    coladd = 0;
	    }
	}
	ستون += coladd;
	اگر (آغاز != تہی)
	    *آغاز = ستون;
	اگر (cursor != تہی)
	    *cursor = ستون;
	اگر (انتہا != تہی)
	    *انتہا = ستون + endadd;
    }
    ورنہ
	getvcol(wp, pos, آغاز, cursor, انتہا, flags);
}

/*
 * Get the leftmost and rightmost virtual column of pos1 and pos2.
 * Used for Visual block mode.
 */
    خالی
getvcols(
    win_T	*wp,
    pos_T	*pos1,
    pos_T	*pos2,
    colnr_T	*بایاں,
    colnr_T	*دایاں,
    صحیح		flags)
{
    colnr_T	from1, from2, to1, to2;

    اگر (LT_POSP(pos1, pos2))
    {
	getvvcol(wp, pos1, &from1, تہی, &to1, flags);
	getvvcol(wp, pos2, &from2, تہی, &to2, flags);
    }
    ورنہ
    {
	getvvcol(wp, pos2, &from1, تہی, &to1, flags);
	getvvcol(wp, pos1, &from2, تہی, &to2, flags);
    }
    اگر (from2 < from1)
	*بایاں = from2;
    ورنہ
	*بایاں = from1;
    اگر (to2 > to1)
    {
	اگر (*p_sel == 'e' && from2 - 1 >= to1)
	    *دایاں = from2 - 1;
	ورنہ
	    *دایاں = to2;
    }
    ورنہ
	*دایاں = to1;
}

/*
 * Skip over ' ' and '\t'.
 */
    char_u *
skipwhite(char_u *q)
{
    char_u	*p = q;

    جب (VIM_ISWHITE(*p))
	++p;
    واپس p;
}

#اگر defined(FEAT_EVAL)
/*
 * skip over ' ', '\t' and '\n'.
 */
    char_u *
skipwhite_and_nl(char_u *q)
{
    char_u	*p = q;

    جب (VIM_ISWHITE(*p) || *p == NL)
	++p;
    واپس p;
}
#اگر_ختم

/*
 * getwhitecols: return the number of whitespace
 * columns (bytes) at the start of a given line
 */
    صحیح
getwhitecols_curline(خالی)
{
    واپس getwhitecols(ml_get_curline());
}

    صحیح
getwhitecols(char_u *p)
{
    واپس skipwhite(p) - p;
}

/*
 * skip over digits
 */
    char_u *
skipdigits(char_u *q)
{
    char_u	*p = q;

    جب (VIM_ISDIGIT(*p))	// skip to next non-digit
	++p;
    واپس p;
}

#اگر defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * skip over binary digits
 */
    char_u *
skipbin(char_u *q)
{
    char_u	*p = q;

    جب (vim_isbdigit(*p))	// skip to next non-digit
	++p;
    واپس p;
}

/*
 * skip over digits and hex characters
 */
    char_u *
skiphex(char_u *q)
{
    char_u	*p = q;

    جب (vim_isxdigit(*p))	// skip to next non-digit
	++p;
    واپس p;
}
#اگر_ختم

/*
 * skip to bin digit (or NUL after the string)
 */
    char_u *
skiptobin(char_u *q)
{
    char_u	*p = q;

    جب (*p != NUL && !vim_isbdigit(*p))	// skip to next digit
	++p;
    واپس p;
}

/*
 * skip to digit (or NUL after the string)
 */
    char_u *
skiptodigit(char_u *q)
{
    char_u	*p = q;

    جب (*p != NUL && !VIM_ISDIGIT(*p))	// skip to next digit
	++p;
    واپس p;
}

/*
 * skip to hex character (or NUL after the string)
 */
    char_u *
skiptohex(char_u *q)
{
    char_u	*p = q;

    جب (*p != NUL && !vim_isxdigit(*p))	// skip to next digit
	++p;
    واپس p;
}

/*
 * Variant of isdigit() that can handle characters > 0x100.
 * We don't use isdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 * Use the VIM_ISDIGIT() macro for simple arguments.
 */
    صحیح
vim_isdigit(صحیح c)
{
    واپس (c >= '0' && c <= '9');
}

/*
 * Variant of isxdigit() that can handle characters > 0x100.
 * We don't use isxdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 */
    صحیح
vim_isxdigit(صحیح c)
{
    واپس (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'f')
	|| (c >= 'A' && c <= 'F');
}

/*
 * Corollary of vim_isdigit and vim_isxdigit() that can handle
 * characters > 0x100.
 */
    صحیح
vim_isbdigit(صحیح c)
{
    واپس (c == '0' || c == '1');
}

    ساکن صحیح
vim_isodigit(صحیح c)
{
    واپس (c >= '0' && c <= '7');
}

/*
 * Vim's own character class functions.  These exist because many library
 * islower()/toupper() etc. do not work properly: they crash when used with
 * invalid values or can't handle latin1 when the locale is C.
 * Speed is most important here.
 */
#تعریف_کرو LATIN1_زیریں 'l'
#تعریف_کرو LATIN1_بالائی 'U'

ساکن char_u latin1flags[257] = "                                                                 UUUUUUUUUUUUUUUUUUUUUUUUUU      llllllllllllllllllllllllll                                                                     UUUUUUUUUUUUUUUUUUUUUUU UUUUUUUllllllllllllllllllllllll llllllll";
ساکن char_u latin1upper[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xf7\xd8\xd9\xda\xdb\xdc\xdd\xde\xff";
ساکن char_u latin1lower[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xd7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    صحیح
vim_islower(صحیح c)
{
    اگر (c <= '@')
	واپس FALSE;
    اگر (c >= 0x80)
    {
	اگر (enc_utf8)
	    واپس utf_islower(c);
	اگر (c >= 0x100)
	{
#اگر_تعریف_شدہ HAVE_ISWLOWER
	    اگر (has_mbyte)
		واپس iswlower(c);
#اگر_ختم
	    // islower() can't handle these chars and may crash
	    واپس FALSE;
	}
	اگر (enc_latin1like)
	    واپس (latin1flags[c] & LATIN1_زیریں) == LATIN1_زیریں;
    }
    واپس محفوظ_islower(c);
}

    صحیح
vim_isupper(صحیح c)
{
    اگر (c <= '@')
	واپس FALSE;
    اگر (c >= 0x80)
    {
	اگر (enc_utf8)
	    واپس utf_isupper(c);
	اگر (c >= 0x100)
	{
#اگر_تعریف_شدہ HAVE_ISWUPPER
	    اگر (has_mbyte)
		واپس iswupper(c);
#اگر_ختم
	    // islower() can't handle these chars and may crash
	    واپس FALSE;
	}
	اگر (enc_latin1like)
	    واپس (latin1flags[c] & LATIN1_بالائی) == LATIN1_بالائی;
    }
    واپس محفوظ_isupper(c);
}

    صحیح
vim_isalpha(صحیح c)
{
    واپس vim_islower(c) || vim_isupper(c);
}

    صحیح
vim_toupper(صحیح c)
{
    اگر (c <= '@')
	واپس c;
    اگر (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	اگر (enc_utf8)
	    واپس utf_toupper(c);
	اگر (c >= 0x100)
	{
#اگر_تعریف_شدہ HAVE_TOWUPPER
	    اگر (has_mbyte)
		واپس towupper(c);
#اگر_ختم
	    // toupper() can't handle these chars and may crash
	    واپس c;
	}
	اگر (enc_latin1like)
	    واپس latin1upper[c];
    }
    اگر (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	واپس TOUPPER_ASC(c);
    واپس TOUPPER_LOC(c);
}

    صحیح
vim_tolower(صحیح c)
{
    اگر (c <= '@')
	واپس c;
    اگر (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	اگر (enc_utf8)
	    واپس utf_tolower(c);
	اگر (c >= 0x100)
	{
#اگر_تعریف_شدہ HAVE_TOWLOWER
	    اگر (has_mbyte)
		واپس towlower(c);
#اگر_ختم
	    // tolower() can't handle these chars and may crash
	    واپس c;
	}
	اگر (enc_latin1like)
	    واپس latin1lower[c];
    }
    اگر (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	واپس TOLOWER_ASC(c);
    واپس TOLOWER_LOC(c);
}

/*
 * skiptowhite: skip over text until ' ' or '\t' or NUL.
 */
    char_u *
skiptowhite(char_u *p)
{
    جب (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    واپس p;
}

/*
 * skiptowhite_esc: Like skiptowhite(), but also skip escaped chars
 */
    char_u *
skiptowhite_esc(char_u *p)
{
    جب (*p != ' ' && *p != '\t' && *p != NUL)
    {
	اگر ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    واپس p;
}

/*
 * Get a number from a string and skip over it.
 * Note: the argument is a pointer to a char_u pointer!
 */
    طویل
getdigits(char_u **pp)
{
    char_u	*p;
    طویل	retval;

    p = *pp;
    retval = strtol((حرف *)p, تہی, 10);
    اگر (*p == '-')		// skip negative sign
	++p;
    p = skipdigits(p);		// skip to next non-digit
    *pp = p;
    واپس retval;
}

/*
 * Like getdigits() but allow for embedded single quotes.
 */
    طویل
getdigits_quoted(char_u **pp)
{
    char_u	*p = *pp;
    طویل	retval = 0;

    اگر (*p == '-')
	++p;
    جب (VIM_ISDIGIT(*p))
    {
	اگر (retval >= LONG_MAX / 10 - 10)
	    retval = LONG_MAX;
	ورنہ
	    retval = retval * 10 - '0' + *p;
	++p;
	اگر (اندر_vim9script() && *p == '\'' && VIM_ISDIGIT(p[1]))
	    ++p;
    }
    اگر (**pp == '-')
    {
	اگر (retval == LONG_MAX)
	    retval = LONG_MIN;
	ورنہ
	    retval = -retval;
    }
    *pp = p;
    واپس retval;
}

/*
 * Return TRUE if "lbuf" is empty or only contains blanks.
 */
    صحیح
vim_isblankline(char_u *lbuf)
{
    char_u	*p;

    p = skipwhite(lbuf);
    واپس (*p == NUL || *p == '\r' || *p == '\n');
}

/*
 * Convert a string into a long and/or unsigned long, taking care of
 * hexadecimal, octal, and binary numbers.  Accepts a '-' sign.
 * If "prep" is not NULL, returns a flag to indicate the type of the number:
 *  0	    decimal
 *  '0'	    octal
 *  'O'	    octal
 *  'o'	    octal
 *  'B'	    bin
 *  'b'	    bin
 *  'X'	    hex
 *  'x'	    hex
 * If "len" is not NULL, the length of the number in characters is returned.
 * If "nptr" is not NULL, the signed result is returned in it.
 * If "unptr" is not NULL, the unsigned result is returned in it.
 * If "what" contains STR2NR_BIN recognize binary numbers
 * If "what" contains STR2NR_OCT recognize octal numbers
 * If "what" contains STR2NR_HEX recognize hex numbers
 * If "what" contains STR2NR_FORCE always assume bin/oct/hex.
 * If "what" contains STR2NR_QUOTE ignore embedded single quotes
 * If maxlen > 0, check at a maximum maxlen chars.
 * If strict is TRUE, check the number strictly. return *len = 0 if fail.
 */
    خالی
vim_str2nr(
    char_u		*آغاز,
    صحیح			*prep,	    // return: type of number 0 = decimal, 'x'
				    // or 'X' is hex, '0', 'o' or 'O' is octal,
				    // 'b' or 'B' is bin
    صحیح			*len,	    // return: detected length of number
    صحیح			what,	    // what numbers to recognize
    varnumber_T		*nptr,	    // return: signed result
    uvarnumber_T	*unptr,	    // return: unsigned result
    صحیح			maxlen,     // max length of string to check
    صحیح			strict,     // check strictly
    صحیح			*حد_سے_تجاوز)  // when not NULL set to TRUE for overflow
{
    char_u	    *ptr = آغاز;
    صحیح		    pre = 0;		// default is decimal
    صحیح		    negative = FALSE;
    uvarnumber_T    un = 0;
    صحیح		    n;

    اگر (len != تہی)
	*len = 0;

    اگر (ptr[0] == '-')
    {
	negative = TRUE;
	++ptr;
    }

    // Recognize hex, octal, and bin.
    اگر (ptr[0] == '0' && ptr[1] != '8' && ptr[1] != '9'
					       && (maxlen == 0 || maxlen > 1))
    {
	pre = ptr[1];
	اگر ((what & STR2NR_HEX)
		&& (pre == 'X' || pre == 'x') && vim_isxdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // hexadecimal
	    ptr += 2;
	ورنہ اگر ((what & STR2NR_BIN)
		&& (pre == 'B' || pre == 'b') && vim_isbdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    //ثنائی
	    ptr += 2;
	ورنہ اگر ((what & STR2NR_OOCT)
		&& (pre == 'O' || pre == 'o') && vim_isodigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // octal with prefix "0o"
	    ptr += 2;
	ورنہ
	{
	    // decimal or octal, default is decimal
	    pre = 0;
	    اگر (what & STR2NR_OCT)
	    {
		// Don't interpret "0", "08" or "0129" as octal.
		برائے (n = 1; n != maxlen && VIM_ISDIGIT(ptr[n]); ++n)
		{
		    اگر (ptr[n] > '7')
		    {
			pre = 0;	// can't be octal
			توڑو;
		    }
		    pre = '0';	// assume octal
		}
	    }
	}
    }

    // Do the conversion manually to avoid sscanf() quirks.
    n = 1;
    اگر (pre == 'B' || pre == 'b'
			     || ((what & STR2NR_BIN) && (what & STR2NR_FORCE)))
    {
	// bin
	اگر (pre != 0)
	    n += 2;	    // skip over "0b"
	جب ('0' <= *ptr && *ptr <= '1')
	{
	    // avoid ubsan error for overflow
	    اگر (un <= UVARNUM_MAX / 2)
		un = 2 * un + (uvarnumber_T)(*ptr - '0');
	    ورنہ
	    {
		un = UVARNUM_MAX;
		اگر (حد_سے_تجاوز != تہی)
		    *حد_سے_تجاوز = TRUE;
	    }
	    ++ptr;
	    اگر (n++ == maxlen)
		توڑو;
	    اگر ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '1')
	    {
		++ptr;
		اگر (n++ == maxlen)
		    توڑو;
	    }
	}
    }
    ورنہ اگر (pre == 'O' || pre == 'o' ||
		pre == '0' || ((what & STR2NR_OCT) && (what & STR2NR_FORCE)))
    {
	// octal
	اگر (pre != 0 && pre != '0')
	    n += 2;	    // skip over "0o"
	جب ('0' <= *ptr && *ptr <= '7')
	{
	    // avoid ubsan error for overflow
	    اگر (un <= UVARNUM_MAX / 8)
		un = 8 * un + (uvarnumber_T)(*ptr - '0');
	    ورنہ
	    {
		un = UVARNUM_MAX;
		اگر (حد_سے_تجاوز != تہی)
		    *حد_سے_تجاوز = TRUE;
	    }
	    ++ptr;
	    اگر (n++ == maxlen)
		توڑو;
	    اگر ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '7')
	    {
		++ptr;
		اگر (n++ == maxlen)
		    توڑو;
	    }
	}
    }
    ورنہ اگر (pre != 0 || ((what & STR2NR_HEX) && (what & STR2NR_FORCE)))
    {
	// hex
	اگر (pre != 0)
	    n += 2;	    // skip over "0x"
	جب (vim_isxdigit(*ptr))
	{
	    // avoid ubsan error for overflow
	    اگر (un <= UVARNUM_MAX / 16)
		un = 16 * un + (uvarnumber_T)hex2nr(*ptr);
	    ورنہ
	    {
		un = UVARNUM_MAX;
		اگر (حد_سے_تجاوز != تہی)
		    *حد_سے_تجاوز = TRUE;
	    }
	    ++ptr;
	    اگر (n++ == maxlen)
		توڑو;
	    اگر ((what & STR2NR_QUOTE) && *ptr == '\'' && vim_isxdigit(ptr[1]))
	    {
		++ptr;
		اگر (n++ == maxlen)
		    توڑو;
	    }
	}
    }
    ورنہ
    {
	// decimal
	جب (VIM_ISDIGIT(*ptr))
	{
	    uvarnumber_T    digit = (uvarnumber_T)(*ptr - '0');

	    // avoid ubsan error for overflow
	    اگر (un < UVARNUM_MAX / 10
		    || (un == UVARNUM_MAX / 10 && digit <= UVARNUM_MAX % 10))
		un = 10 * un + digit;
	    ورنہ
	    {
		un = UVARNUM_MAX;
		اگر (حد_سے_تجاوز != تہی)
		    *حد_سے_تجاوز = TRUE;
	    }
	    ++ptr;
	    اگر (n++ == maxlen)
		توڑو;
	    اگر ((what & STR2NR_QUOTE) && *ptr == '\'' && VIM_ISDIGIT(ptr[1]))
	    {
		++ptr;
		اگر (n++ == maxlen)
		    توڑو;
	    }
	}
    }

    // Check for an alphanumeric character immediately following, that is
    // most likely a typo.
    اگر (strict && n - 1 != maxlen && ASCII_ISALNUM(*ptr))
	واپس;

    اگر (prep != تہی)
	*prep = pre;
    اگر (len != تہی)
	*len = (صحیح)(ptr - آغاز);
    اگر (nptr != تہی)
    {
	اگر (negative)   // account for leading '-' for decimal numbers
	{
	    // avoid ubsan error for overflow
	    اگر (un > VARNUM_MAX)
	    {
		*nptr = VARNUM_MIN;
		اگر (حد_سے_تجاوز != تہی)
		    *حد_سے_تجاوز = TRUE;
	    }
	    ورنہ
		*nptr = -(varnumber_T)un;
	}
	ورنہ
	{
	    // prevent a large unsigned number to become negative
	    اگر (un > VARNUM_MAX)
	    {
		un = VARNUM_MAX;
		اگر (حد_سے_تجاوز != تہی)
		    *حد_سے_تجاوز = TRUE;
	    }
	    *nptr = (varnumber_T)un;
	}
    }
    اگر (unptr != تہی)
	*unptr = un;
}

/*
 * Return the value of a single hex character.
 * Only valid when the argument is '0' - '9', 'A' - 'F' or 'a' - 'f'.
 */
    صحیح
hex2nr(صحیح c)
{
    اگر (c >= 'a' && c <= 'f')
	واپس c - 'a' + 10;
    اگر (c >= 'A' && c <= 'F')
	واپس c - 'A' + 10;
    واپس c - '0';
}

/*
 * Convert two hex characters to a byte.
 * Return -1 if one of the characters is not hex.
 */
    صحیح
hexhex2nr(char_u *p)
{
    اگر (!vim_isxdigit(p[0]) || !vim_isxdigit(p[1]))
	واپس -1;
    واپس (hex2nr(p[0]) << 4) + hex2nr(p[1]);
}

/*
 * Return TRUE if "str" starts with a backslash that should be removed.
 * For MS-DOS, MSWIN and OS/2 this is only done when the character after the
 * backslash is not a normal file name character.
 * '$' is a valid file name character, we don't remove the backslash before
 * it.  This means it is not possible to use an environment variable after a
 * backslash.  "C:\$VIM\doc" is taken literally, only "$VIM\doc" works.
 * Although "\ name" is valid, the backslash in "Program\ files" must be
 * removed.  Assume a file name doesn't start with a space.
 * For multi-byte names, never remove a backslash before a non-ascii
 * character, assume that all multi-byte characters are valid file name
 * characters.
 */
    صحیح
rem_backslash(char_u *str)
{
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    واپس (str[0] == '\\'
	    && str[1] < 0x80
	    && (str[1] == ' '
		|| (str[1] != NUL
		    && str[1] != '*'
		    && str[1] != '?'
		    && !vim_isfilec(str[1]))));
#ورنہ
    واپس (str[0] == '\\' && str[1] != NUL);
#اگر_ختم
}

/*
 * Halve the number of backslashes in a file name argument.
 * For MS-DOS we only do this if the character after the backslash
 * is not a normal file character.
 */
    خالی
backslash_halve(char_u *p)
{
    برائے ( ; *p; ++p)
	اگر (rem_backslash(p))
	    STRMOVE(p, p + 1);
}

/*
 * backslash_halve() plus save the result in allocated memory.
 * However, returns "p" when out of memory.
 */
    char_u *
backslash_halve_save(char_u *p)
{
    char_u	*res;

    res = vim_strsave(p);
    اگر (res == تہی)
	واپس p;
    backslash_halve(res);
    واپس res;
}
