/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#অন্তর্ভাব "vim.h"

#যদি defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#endif

স্থির পূর্ণসংখ্যা parse_isopt(char_u *var, buf_T *buf, পূর্ণসংখ্যা only_check);
স্থির পূর্ণসংখ্যা win_nolbr_chartabsize(chartabsize_T *cts, পূর্ণসংখ্যা *headp);
স্থির unsigned nr2hex(unsigned c);

স্থির পূর্ণসংখ্যা    chartab_initialized = FALSE;

// b_chartab[] is an array of 32 bytes, each bit representing one of the
// characters 0-255.
#define SET_CHARTAB(buf, c) (buf)->b_chartab[(unsigned)(c) >> 3] |= (1 << ((c) & 0x7))
#define RESET_CHARTAB(buf, c) (buf)->b_chartab[(unsigned)(c) >> 3] &= ~(1 << ((c) & 0x7))
#define GET_CHARTAB(buf, c) ((buf)->b_chartab[(unsigned)(c) >> 3] & (1 << ((c) & 0x7)))

// table used below, see init_chartab() for an explanation
স্থির char_u	g_chartab[256];

/*
 * Flags for g_chartab[].
 */
#define CT_CELL_MASK	0x07	// mask: nr of display cells (1, 2 or 4)
#define CT_PRINT_CHAR	0x10	// flag: set for printable chars
#define CT_ID_CHAR	0x20	// flag: set for ID chars
#define CT_FNAME_CHAR	0x40	// flag: set for file name chars

স্থির পূর্ণসংখ্যা in_win_border(win_T *wp, colnr_T vcol);

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
    পূর্ণসংখ্যা
init_chartab(শূন্য)
{
    ফেরত buf_init_chartab(curbuf, TRUE);
}

    পূর্ণসংখ্যা
buf_init_chartab(
    buf_T	*buf,
    পূর্ণসংখ্যা		global)		// FALSE: only set buf->b_chartab[]
{
    পূর্ণসংখ্যা		c;
    char_u	*p;
    পূর্ণসংখ্যা		i;

    যদি (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 */
	c = 0;
	যখন (c < ' ')
	    g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	যখন (c <= '~')
	    g_chartab[c++] = 1 + CT_PRINT_CHAR;
	যখন (c < 256)
	{
	    // UTF-8: bytes 0xa0 - 0xff are printable (latin1)
	    যদি (enc_utf8 && c >= 0xa0)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // euc-jp characters starting with 0x8e are single width
	    অন্যথা যদি (enc_dbcs == DBCS_JPNU && c == 0x8e)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // other double-byte chars can be printable AND double-width
	    অন্যথা যদি (enc_dbcs != 0 && MB_BYTE2LEN(c) == 2)
		g_chartab[c++] = CT_PRINT_CHAR + 2;
	    অন্যথা
		// the rest is unprintable by default
		g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	}

	// Assume that every multi-byte char is a filename character.
	জন্য (c = 1; c < 256; ++c)
	    যদি ((enc_dbcs != 0 && MB_BYTE2LEN(c) > 1)
		    || (enc_dbcs == DBCS_JPNU && c == 0x8e)
		    || (enc_utf8 && c >= 0xa0))
		g_chartab[c] |= CT_FNAME_CHAR;
    }

    /*
     * Init word char flags all to FALSE
     */
    CLEAR_FIELD(buf->b_chartab);
    যদি (enc_dbcs != 0)
	জন্য (c = 0; c < 256; ++c)
	{
	    // double-byte characters are probably word characters
	    যদি (MB_BYTE2LEN(c) == 2)
		SET_CHARTAB(buf, c);
	}

    /*
     * In lisp mode the '-' character is included in keywords.
     */
    যদি (buf->b_p_lisp)
	SET_CHARTAB(buf, '-');

    // Walk through the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    জন্য (i = global ? 0 : 3; i <= 3; ++i)
    {
	যদি (i == 0)
	    p = p_isi;		// first round: 'isident'
	অন্যথা যদি (i == 1)
	    p = p_isp;		// second round: 'isprint'
	অন্যথা যদি (i == 2)
	    p = p_isf;		// third round: 'isfname'
	অন্যথা	// i == 3
	    p = buf->b_p_isk;	// fourth round: 'iskeyword'

	যদি (parse_isopt(p, buf, FALSE) == FAIL)
	    ফেরত FAIL;
    }

    chartab_initialized = TRUE;
    ফেরত OK;
}

/**
 * Checks the format for the option settings 'iskeyword', 'isident', 'isfname'
 * or 'isprint'.
 * Returns FAIL if has an error, OK otherwise.
 */
	পূর্ণসংখ্যা
check_isopt(char_u *var)
{
    ফেরত parse_isopt(var, ফাঁকা, TRUE);
}

    স্থির পূর্ণসংখ্যা
parse_isopt(
	char_u	*var,
	buf_T	*buf,
	পূর্ণসংখ্যা	only_check)	// FALSE: refill g_chartab[]
{
    char_u	*p = var;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		c2;
    পূর্ণসংখ্যা		tilde;
    পূর্ণসংখ্যা		do_isalpha;
    পূর্ণসংখ্যা		trail_comma;

    // Parses the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    // Each option is a list of characters, character numbers or ranges,
    // separated by commas, e.g.: "200-210,x,#-178,-"
    যখন (*p)
    {
	tilde = FALSE;
	do_isalpha = FALSE;
	যদি (*p == '^' && p[1] != NUL)
	{
	    tilde = TRUE;
	    ++p;
	}
	যদি (VIM_ISDIGIT(*p))
	    c = getdigits(&p);
	অন্যথা যদি (has_mbyte)
	    c = mb_ptr2char_adv(&p);
	অন্যথা
	    c = *p++;
	c2 = -1;
	যদি (*p == '-' && p[1] != NUL)
	{
	    ++p;
	    যদি (VIM_ISDIGIT(*p))
		c2 = getdigits(&p);
	    অন্যথা যদি (has_mbyte)
		c2 = mb_ptr2char_adv(&p);
	    অন্যথা
		c2 = *p++;
	}
	যদি (c <= 0 || c >= 256 || (c2 < c && c2 != -1) || c2 >= 256
						  || !(*p == NUL || *p == ','))
	    ফেরত FAIL;

	trail_comma = *p == ',';
	p = skip_to_option_part(p);
	যদি (trail_comma && *p == NUL)
	    // Trailing comma is not allowed.
	    ফেরত FAIL;

	যদি (only_check)
	    চালু;

	যদি (c2 == -1)	// not a range
	{
	    /*
	     * A single '@' (not "@-@"):
	     * Decide on letters being ID/printable/keyword chars with
	     * standard function isalpha(). This takes care of locale for
	     * single-byte characters).
	     */
	    যদি (c == '@')
	    {
		do_isalpha = TRUE;
		c = 1;
		c2 = 255;
	    }
	    অন্যথা
		c2 = c;
	}

	যখন (c <= c2)
	{
	    // Use the MB_ functions here, because isalpha() doesn't
	    // work properly when 'encoding' is "latin1" and the locale is
	    // "C".
	    যদি (!do_isalpha || MB_ISLOWER(c) || MB_ISUPPER(c))
	    {
		যদি (var == p_isi)			// (re)set ID flag
		{
		    যদি (tilde)
			g_chartab[c] &= ~CT_ID_CHAR;
		    অন্যথা
			g_chartab[c] |= CT_ID_CHAR;
		}
		অন্যথা যদি (var == p_isp)			// (re)set printable
		{
		    যদি ((c < ' ' || c > '~'
			    // For double-byte we keep the cell width, so
			    // that we can detect it from the first byte.
			) && !(enc_dbcs && MB_BYTE2LEN(c) == 2))
		    {
			যদি (tilde)
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_MASK)
					 + ((dy_flags & DY_UHEX) ? 4 : 2);
			    g_chartab[c] &= ~CT_PRINT_CHAR;
			}
			অন্যথা
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_MASK) + 1;
			    g_chartab[c] |= CT_PRINT_CHAR;
			}
		    }
		}
		অন্যথা যদি (var == p_isf)			// (re)set fname flag
		{
		    যদি (tilde)
			g_chartab[c] &= ~CT_FNAME_CHAR;
		    অন্যথা
			g_chartab[c] |= CT_FNAME_CHAR;
		}
		অন্যথা // var == p_isk || var == buf->b_p_isk
							// (re)set keyword flag
		{
		    যদি (tilde)
			RESET_CHARTAB(buf, c);
		    অন্যথা
			SET_CHARTAB(buf, c);
		}
	    }
	    ++c;
	}
    }

    ফেরত OK;
}

/*
 * Translate any special characters in buf[bufsize] in-place.
 * The result is a string with only printable characters, but if there is not
 * enough room, not all characters will be translated.
 */
    শূন্য
trans_characters(
    char_u	*buf,
    পূর্ণসংখ্যা		bufsize)
{
    পূর্ণসংখ্যা		len;		// length of string needing translation
    পূর্ণসংখ্যা		room;		// room in buffer after string
    char_u	*trs;		// translated character
    পূর্ণসংখ্যা		trs_len;	// length of trs[]

    len = (পূর্ণসংখ্যা)STRLEN(buf);
    room = bufsize - len;
    যখন (*buf != 0)
    {
	// Assume a multi-byte character doesn't need translation.
	যদি (has_mbyte && (trs_len = (*mb_ptr2len)(buf)) > 1)
	    len -= trs_len;
	অন্যথা
	{
	    trs = transchar_byte(*buf);
	    trs_len = (পূর্ণসংখ্যা)STRLEN(trs);
	    যদি (trs_len > 1)
	    {
		room -= trs_len - 1;
		যদি (room <= 0)
		    ফেরত;
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
    পূর্ণসংখ্যা		l, len, c;
    char_u	hexbuf[11];

    যদি (has_mbyte)
    {
	// Compute the length of the result, taking account of unprintable
	// multi-byte characters.
	len = 0;
	p = s;
	যখন (*p != NUL)
	{
	    যদি ((l = (*mb_ptr2len)(p)) > 1)
	    {
		c = (*mb_ptr2char)(p);
		p += l;
		যদি (vim_isprintc(c))
		    len += l;
		অন্যথা
		{
		    transchar_hex(hexbuf, c);
		    len += (পূর্ণসংখ্যা)STRLEN(hexbuf);
		}
	    }
	    অন্যথা
	    {
		l = byte2cells(*p++);
		যদি (l > 0)
		    len += l;
		অন্যথা
		    len += 4;	// illegal byte sequence
	    }
	}
	res = alloc(len + 1);
    }
    অন্যথা
	res = alloc(vim_strsize(s) + 1);

    যদি (res == ফাঁকা)
	ফেরত ফাঁকা;

    char_u *d = res;

    p = s;
    যখন (*p != NUL)
    {
	যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    c = (*mb_ptr2char)(p);
	    যদি (vim_isprintc(c))
	    {
		mch_memmove(d, p, (size_t)l);
		d += l;
	    }
	    অন্যথা
	    {
		transchar_hex(d, c);
		d += STRLEN(d);
	    }
	    p += l;
	}
	অন্যথা
	{
	    char_u	*trs = transchar_byte(*p++);
	    size_t	trs_len = STRLEN(trs);

	    mch_memmove(d, trs, trs_len);
	    d += trs_len;
	}
    }
    *d = NUL;
    ফেরত res;
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
    পূর্ণসংখ্যা		orglen,
    char_u	*buf,
    পূর্ণসংখ্যা		buflen)
{
    garray_T	ga;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		len = orglen;

#define GA_CHAR(i)  ((char_u *)ga.ga_তথ্য)[i]
#define GA_PTR(i)   ((char_u *)ga.ga_তথ্য + (i))
#define STR_CHAR(i)  (buf == ফাঁকা ? GA_CHAR(i) : buf[i])
#define STR_PTR(i)   (buf == ফাঁকা ? GA_PTR(i) : buf + (i))

    // Copy "str" into "buf" or allocated memory, unmodified.
    যদি (buf == ফাঁকা)
    {
	ga_init2(&ga, 1, 10);
	যদি (ga_grow(&ga, len + 1) == FAIL)
	    ফেরত ফাঁকা;
	mch_memmove(ga.ga_তথ্য, str, (size_t)len);
	ga.ga_len = len;
    }
    অন্যথা
    {
	যদি (len >= buflen)	    // Ugly!
	    len = buflen - 1;
	mch_memmove(buf, str, (size_t)len);
    }
    যদি (buf == ফাঁকা)
	GA_CHAR(len) = NUL;
    অন্যথা
	buf[len] = NUL;

    // Make each character lower case.
    i = 0;
    যখন (STR_CHAR(i) != NUL)
    {
	যদি (enc_utf8 || (has_mbyte && MB_BYTE2LEN(STR_CHAR(i)) > 1))
	{
	    যদি (enc_utf8)
	    {
		পূর্ণসংখ্যা	c = utf_ptr2char(STR_PTR(i));
		পূর্ণসংখ্যা	olen = utf_ptr2len(STR_PTR(i));
		পূর্ণসংখ্যা	lc = utf_tolower(c);

		// Only replace the character when it is not an invalid
		// sequence (ASCII character or more than one byte) and
		// utf_tolower() doesn't return the original character.
		যদি ((c < 0x80 || olen > 1) && c != lc)
		{
		    পূর্ণসংখ্যা	    nlen = utf_char2len(lc);

		    // If the byte length changes need to shift the following
		    // characters forward or backward.
		    যদি (olen != nlen)
		    {
			যদি (nlen > olen)
			{
			    যদি (buf == ফাঁকা
				    ? ga_grow(&ga, nlen - olen + 1) == FAIL
				    : len + nlen - olen >= buflen)
			    {
				// out of memory, keep old char
				lc = c;
				nlen = olen;
			    }
			}
			যদি (olen != nlen)
			{
			    যদি (buf == ফাঁকা)
			    {
				STRMOVE(GA_PTR(i) + nlen, GA_PTR(i) + olen);
				ga.ga_len += nlen - olen;
			    }
			    অন্যথা
			    {
				STRMOVE(buf + i + nlen, buf + i + olen);
				len += nlen - olen;
			    }
			}
		    }
		    (শূন্য)utf_char2bytes(lc, STR_PTR(i));
		}
	    }
	    // skip to next multi-byte char
	    i += (*mb_ptr2len)(STR_PTR(i));
	}
	অন্যথা
	{
	    যদি (buf == ফাঁকা)
		GA_CHAR(i) = TOLOWER_LOC(GA_CHAR(i));
	    অন্যথা
		buf[i] = TOLOWER_LOC(buf[i]);
	    ++i;
	}
    }

    যদি (buf == ফাঁকা)
	ফেরত (char_u *)ga.ga_তথ্য;
    ফেরত buf;
}

/*
 * Catch 22: g_chartab[] can't be initialized before the options are
 * initialized, and initializing options may cause transchar() to be called!
 * When chartab_initialized == FALSE don't use g_chartab[].
 * Does NOT work for multi-byte characters, c must be <= 255.
 * Also doesn't work for the first byte of a multi-byte, "c" must be a
 * character!
 */
স্থির char_u	transchar_charbuf[7];

    char_u *
transchar(পূর্ণসংখ্যা c)
{
    ফেরত transchar_buf(curbuf, c);
}

    char_u *
transchar_buf(buf_T *buf, পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা			i;

    i = 0;
    যদি (IS_SPECIAL(c))	    // special key code, display as ~@ char
    {
	transchar_charbuf[0] = '~';
	transchar_charbuf[1] = '@';
	i = 2;
	c = K_SECOND(c);
    }

    যদি ((!chartab_initialized && ((c >= ' ' && c <= '~')))
					|| (c < 256 && vim_isprintc_strict(c)))
    {
	// printable character
	transchar_charbuf[i] = c;
	transchar_charbuf[i + 1] = NUL;
    }
    অন্যথা
	transchar_nonprint(buf, transchar_charbuf + i, c);
    ফেরত transchar_charbuf;
}

/*
 * Like transchar(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of the current buffer.
 */
    char_u *
transchar_byte(পূর্ণসংখ্যা c)
{
    ফেরত transchar_byte_buf(curbuf, c);
}

/*
 * Like transchar_buf(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of "buf", unless it is NULL.
 */
    char_u *
transchar_byte_buf(buf_T *buf, পূর্ণসংখ্যা c)
{
    যদি (enc_utf8 && c >= 0x80)
    {
	transchar_nonprint(buf, transchar_charbuf, c);
	ফেরত transchar_charbuf;
    }
    ফেরত transchar_buf(buf, c);
}
/*
 * Convert non-printable character to two or more printable characters in
 * "charbuf[]".  "charbuf" needs to be able to hold five bytes.
 * Does NOT work for multi-byte characters, c must be <= 255.
 */
    শূন্য
transchar_nonprint(buf_T *buf, char_u *charbuf, পূর্ণসংখ্যা c)
{
    যদি (c == NL)
	c = NUL;		// we use newline in place of a NUL
    অন্যথা যদি (buf != ফাঁকা && c == CAR && get_fileformat(buf) == EOL_MAC)
	c = NL;			// we use CR in place of  NL in this case

    যদি (dy_flags & DY_UHEX)		// 'display' has "uhex"
	transchar_hex(charbuf, c);

    অন্যথা যদি (c <= 0x7f)			// 0x00 - 0x1f and 0x7f
    {
	charbuf[0] = '^';
	charbuf[1] = c ^ 0x40;		// DEL displayed as ^?
	charbuf[2] = NUL;
    }
    অন্যথা যদি (enc_utf8)
    {
	transchar_hex(charbuf, c);
    }
    অন্যথা যদি (c >= ' ' + 0x80 && c <= '~' + 0x80)    // 0xa0 - 0xfe
    {
	charbuf[0] = '|';
	charbuf[1] = c - 0x80;
	charbuf[2] = NUL;
    }
    অন্যথা					    // 0x80 - 0x9f and 0xff
    {
	charbuf[0] = '~';
	charbuf[1] = (c - 0x80) ^ 0x40;	// 0xff displayed as ~?
	charbuf[2] = NUL;
    }
}

    শূন্য
transchar_hex(char_u *buf, পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা		i = 0;

    buf[0] = '<';
    যদি (c > 255)
    {
	buf[++i] = nr2hex((unsigned)c >> 12);
	buf[++i] = nr2hex((unsigned)c >> 8);
    }
    buf[++i] = nr2hex((unsigned)c >> 4);
    buf[++i] = nr2hex((unsigned)c);
    buf[++i] = '>';
    buf[++i] = NUL;
}

/*
 * Convert the lower 4 bits of byte "c" to its hex character.
 * Lower case letters are used to avoid the confusion of <F1> being 0xf1 or
 * function key 1.
 */
    স্থির unsigned
nr2hex(unsigned c)
{
    যদি ((c & 0xf) <= 9)
	ফেরত (c & 0xf) + '0';
    ফেরত (c & 0xf) - 10 + 'a';
}

/*
 * Return number of display cells occupied by byte "b".
 * Caller must make sure 0 <= b <= 255.
 * For multi-byte mode "b" must be the first byte of a character.
 * A TAB is counted as two cells: "^I".
 * For UTF-8 mode this will return 0 for bytes >= 0x80, because the number of
 * cells depends on further bytes.
 */
    পূর্ণসংখ্যা
byte2cells(পূর্ণসংখ্যা b)
{
    যদি (enc_utf8 && b >= 0x80)
	ফেরত 0;
    ফেরত (g_chartab[b] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character "c".
 * "c" can be a special key (negative number) in which case 3 or 4 is returned.
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    পূর্ণসংখ্যা
char2cells(পূর্ণসংখ্যা c)
{
    যদি (IS_SPECIAL(c))
	ফেরত char2cells(K_SECOND(c)) + 2;
    যদি (c >= 0x80)
    {
	// UTF-8: above 0x80 need to check the value
	যদি (enc_utf8)
	    ফেরত utf_char2cells(c);
	// DBCS: double-byte means double-width, except for euc-jp with first
	// byte 0x8e
	যদি (enc_dbcs != 0 && c >= 0x100)
	{
	    যদি (enc_dbcs == DBCS_JPNU && ((unsigned)c >> 8) == 0x8e)
		ফেরত 1;
	    ফেরত 2;
	}
    }
    ফেরত (g_chartab[c & 0xff] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character at "*p".
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    পূর্ণসংখ্যা
ptr2cells(char_u *p)
{
    যদি (!has_mbyte)
	ফেরত byte2cells(*p);

    // For UTF-8 we need to look at more bytes if the first byte is >= 0x80.
    যদি (enc_utf8 && *p >= 0x80)
	ফেরত utf_ptr2cells(p);
    // For DBCS we can tell the cell count from the first byte.
    ফেরত (g_chartab[*p] & CT_CELL_MASK);
}

/*
 * Return the number of character cells string "s" will take on the screen,
 * counting TABs as two characters: "^I".
 */
    পূর্ণসংখ্যা
vim_strsize(char_u *s)
{
    ফেরত vim_strnsize(s, (পূর্ণসংখ্যা)MAXCOL);
}

/*
 * Return the number of character cells string "s[len]" will take on the
 * screen, counting TABs as two characters: "^I".
 */
    পূর্ণসংখ্যা
vim_strnsize(char_u *s, পূর্ণসংখ্যা len)
{
    পূর্ণসংখ্যা		আকার_2 = 0;

    যখন (*s != NUL && --len >= 0)
    {
	পূর্ণসংখ্যা	    l = (*mb_ptr2len)(s);

	আকার_2 += ptr2cells(s);
	s += l;
	len -= l - 1;
    }

    ফেরত আকার_2;
}

/*
 * Return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab.
 * Use a define to make it fast, this is used very often!!!
 * Also see getvcol() below.
 */

#ifdef FEAT_VARTABS
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, col) \
    যদি (*(p) == TAB && (!(wp)->w_p_list || (wp)->w_lcs_chars.tab1)) \
    { \
	ফেরত tabstop_padding(col, (buf)->b_p_ts, (buf)->b_p_vts_array); \
    } \
    অন্যথা \
	ফেরত ptr2cells(p);
#অন্যথা
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, col) \
    যদি (*(p) == TAB && (!(wp)->w_p_list || wp->w_lcs_chars.tab1)) \
    { \
	পূর্ণসংখ্যা ts; \
	ts = (buf)->b_p_ts; \
	ফেরত (পূর্ণসংখ্যা)(ts - (col % ts)); \
    } \
    অন্যথা \
	ফেরত ptr2cells(p);
#endif

    পূর্ণসংখ্যা
chartabsize(char_u *p, colnr_T col)
{
    RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, p, col)
}

#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    পূর্ণসংখ্যা
win_chartabsize(win_T *wp, char_u *p, colnr_T col)
{
    RET_WIN_BUF_CHARTABSIZE(wp, wp->w_বাফার, p, col)
}
#endif

/*
 * Return the number of characters the string "s" will take on the screen,
 * taking into account the size of a tab.
 * Does not handle text properties, since "s" is not a buffer line.
 */
    পূর্ণসংখ্যা
linetabsize_str(char_u *s)
{
    ফেরত linetabsize_col(0, s);
}

/*
 * Like linetabsize_str(), but "s" starts at column "startcol".
 */
    পূর্ণসংখ্যা
linetabsize_col(পূর্ণসংখ্যা startcol, char_u *s)
{
    chartabsize_T cts;
    vimlong_T vcol;

    init_chartabsize_arg(&cts, curwin, 0, startcol, s, s);
    vcol = cts.cts_vcol;

    যখন (*cts.cts_ptr != NUL)
    {
	vcol += lbr_chartabsize_adv(&cts);
	যদি (vcol > MAXCOL)
	{
	    cts.cts_vcol = MAXCOL;
	    বিরতি;
	}
	অন্যথা
	    cts.cts_vcol = (পূর্ণসংখ্যা)vcol;
    }
    clear_chartabsize_arg(&cts);
    ফেরত (পূর্ণসংখ্যা)cts.cts_vcol;
}

/*
 * Like linetabsize_str(), but for a given window instead of the current one.
 * Doesn't count the size of 'listchars' "eol".
 */
    পূর্ণসংখ্যা
win_linetabsize(win_T *wp, linenr_T lnum, char_u *সারি, colnr_T len)
{
    chartabsize_T cts;

    init_chartabsize_arg(&cts, wp, lnum, 0, সারি, সারি);
    win_linetabsize_cts(&cts, len);
    clear_chartabsize_arg(&cts);
    ফেরত (পূর্ণসংখ্যা)cts.cts_vcol;
}

/*
 * Return the number of cells line "lnum" of window "wp" will take on the
 * screen, taking into account the size of a tab and text properties.
 * Doesn't count the size of 'listchars' "eol".
 */
    পূর্ণসংখ্যা
linetabsize(win_T *wp, linenr_T lnum)
{
    ফেরত win_linetabsize(wp, lnum,
		       ml_get_buf(wp->w_বাফার, lnum, FALSE), (colnr_T)MAXCOL);
}

/*
 * Like linetabsize(), but counts the size of 'listchars' "eol".
 */
    পূর্ণসংখ্যা
linetabsize_eol(win_T *wp, linenr_T lnum)
{
    ফেরত linetabsize(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Like linetabsize(), but excludes 'above'/'after'/'right'/'below' aligned
 * virtual text, while keeping inline virtual text.
 */
    পূর্ণসংখ্যা
linetabsize_no_outer(win_T *wp, linenr_T lnum)
{
#ifndef FEAT_PROP_POPUP
    ফেরত linetabsize(wp, lnum);
#অন্যথা
    chartabsize_T cts;
    char_u *সারি = ml_get_buf(wp->w_বাফার, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, সারি, সারি);

    যদি (cts.cts_text_prop_গুনতি)
    {
	পূর্ণসংখ্যা write_idx = 0;
	জন্য (পূর্ণসংখ্যা read_idx = 0; read_idx < cts.cts_text_prop_গুনতি; read_idx++)
	{
	    textprop_T *tp = &cts.cts_text_props[read_idx];
	    যদি (tp->tp_col != MAXCOL)
	    {
		যদি (read_idx != write_idx)
		    cts.cts_text_props[write_idx] = *tp;
		write_idx++;
	    }
	}
	cts.cts_text_prop_গুনতি = write_idx;
	যদি (cts.cts_text_prop_গুনতি == 0)
	    VIM_CLEAR(cts.cts_text_props);
    }

    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    ফেরত (পূর্ণসংখ্যা)cts.cts_vcol;
#endif
}

/*
 * Like linetabsize_no_outer(), but counts the size of 'listchars' "eol".
 */
    পূর্ণসংখ্যা
linetabsize_no_outer_eol(win_T *wp, linenr_T lnum)
{
    ফেরত linetabsize_no_outer(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Return TRUE when win_lbr_chartabsize() does nothing more than
 * win_nolbr_chartabsize(): no 'linebreak', 'breakindent', 'showbreak' and no
 * text properties that insert text.
 */
#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    স্থির পূর্ণসংখ্যা
win_lbr_is_simple_width(chartabsize_T *cts)
{
    ফেরত TRUE
# ifdef FEAT_LINEBREAK
	&& !cts->cts_win->w_p_lbr && !cts->cts_win->w_p_bri
	&& *get_showbreak_মান(cts->cts_win) == NUL
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_text
# endif
	;
}
#endif

    শূন্য
win_linetabsize_cts(chartabsize_T *cts, colnr_T len)
{
    vimlong_T vcol = cts->cts_vcol;
#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    // Fast path: when win_lbr_chartabsize() would just return
    // win_nolbr_chartabsize() (wrap on; no 'linebreak'/'breakindent'/
    // 'showbreak'/inserted text properties), call it directly.
    পূর্ণসংখ্যা simple_width = cts->cts_win->w_p_wrap && win_lbr_is_simple_width(cts);
#endif
#ifdef FEAT_PROP_POPUP
    cts->cts_with_trailing = len == MAXCOL;
#endif
    জন্য ( ; *cts->cts_ptr != NUL && (len == MAXCOL || cts->cts_ptr < cts->cts_সারি + len);
						      MB_PTR_ADV(cts->cts_ptr))
    {
#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
	যদি (simple_width)
	{
	    পূর্ণসংখ্যা	c = *cts->cts_ptr;

	    যদি (c < 0x80 && c != TAB)
		vcol += g_chartab[c] & CT_CELL_MASK;
	    অন্যথা
		vcol += win_nolbr_chartabsize(cts, ফাঁকা);
	}
	অন্যথা
#endif
	    vcol += win_lbr_chartabsize(cts, ফাঁকা, ফাঁকা);
	যদি (vcol > MAXCOL)
	{
	    cts->cts_vcol = MAXCOL;
	    বিরতি;
	}
	অন্যথা
	    cts->cts_vcol = (পূর্ণসংখ্যা)vcol;
    }
#ifdef FEAT_PROP_POPUP
    // check for a virtual text at the end of a line or on an empty line
    যদি (len == MAXCOL && cts->cts_has_prop_with_text && *cts->cts_ptr == NUL)
    {
	পূর্ণসংখ্যা শীর্ষ = 0;
	(শূন্য)win_lbr_chartabsize(cts, &শীর্ষ, ফাঁকা);
	vcol += cts->cts_cur_text_width + শীর্ষ;
	// When properties are above the empty line must also be counted.  For
	// a property below the width already includes filling up the line.
	যদি (cts->cts_ptr == cts->cts_সারি && cts->cts_prop_সারিগুলি > 0
							 && !cts->cts_has_below)
	    ++vcol;
	cts->cts_vcol = vcol > MAXCOL ? MAXCOL : (পূর্ণসংখ্যা)vcol;
    }
#endif
}

/*
 * Return TRUE if 'c' is a normal identifier character:
 * Letters and characters from the 'isident' option.
 */
    পূর্ণসংখ্যা
vim_isIDc(পূর্ণসংখ্যা c)
{
    ফেরত (c > 0 && c < 0x100 && (g_chartab[c] & CT_ID_CHAR));
}

/*
 * Like vim_isIDc() but not using the 'isident' option: letters, numbers and
 * underscore.
 */
    পূর্ণসংখ্যা
vim_isNormalIDc(পূর্ণসংখ্যা c)
{
    ফেরত ASCII_ISALNUM(c) || c == '_';
}

/*
 * return TRUE if 'c' is a keyword character: Letters and characters from
 * 'iskeyword' option for the current buffer.
 * For multi-byte characters mb_get_class() is used (builtin rules).
 */
    পূর্ণসংখ্যা
vim_iswordc(পূর্ণসংখ্যা c)
{
    ফেরত vim_iswordc_buf(c, curbuf);
}

    পূর্ণসংখ্যা
vim_iswordc_buf(পূর্ণসংখ্যা c, buf_T *buf)
{
    যদি (c >= 0x100)
    {
	যদি (enc_dbcs != 0)
	    ফেরত dbcs_class((unsigned)c >> 8, (unsigned)(c & 0xff)) >= 2;
	যদি (enc_utf8)
	    ফেরত utf_class_buf(c, buf) >= 2;
	ফেরত FALSE;
    }
    ফেরত (c > 0 && GET_CHARTAB(buf, c) != 0);
}

/*
 * Just like vim_iswordc() but uses a pointer to the (multi-byte) character.
 */
    পূর্ণসংখ্যা
vim_iswordp(char_u *p)
{
    ফেরত vim_iswordp_buf(p, curbuf);
}

    পূর্ণসংখ্যা
vim_iswordp_buf(char_u *p, buf_T *buf)
{
    পূর্ণসংখ্যা	c = *p;

    যদি (has_mbyte && MB_BYTE2LEN(c) > 1)
	c = (*mb_ptr2char)(p);
    ফেরত vim_iswordc_buf(c, buf);
}

/*
 * Return TRUE if 'c' is a valid file-name character as specified with the
 * 'isfname' option.
 * Assume characters above 0x100 are valid (multi-byte).
 * To be used for commands like "gf".
 */
    পূর্ণসংখ্যা
vim_isfilec(পূর্ণসংখ্যা c)
{
    ফেরত (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_FNAME_CHAR)));
}

#যদি defined(FEAT_SPELL)
/*
 * Return TRUE if 'c' is a valid file-name character, including characters left
 * out of 'isfname' to make "gf" work, such as comma, space, '@', etc.
 */
    পূর্ণসংখ্যা
vim_is_fname_char(পূর্ণসংখ্যা c)
{
    ফেরত vim_isfilec(c) || c == ',' || c == ' ' || c == '@';
}
#endif

/*
 * return TRUE if 'c' is a valid file-name character or a wildcard character
 * Assume characters above 0x100 are valid (multi-byte).
 * Explicitly interpret ']' as a wildcard character as mch_has_wildcard("]")
 * returns false.
 */
    পূর্ণসংখ্যা
vim_isfilec_or_wc(পূর্ণসংখ্যা c)
{
    char_u buf[2];

    buf[0] = (char_u)c;
    buf[1] = NUL;
    ফেরত vim_isfilec(c) || c == ']' || mch_has_wildcard(buf);
}

/*
 * Return TRUE if 'c' is a printable character.
 * Assume characters above 0x100 are printable (multi-byte), except for
 * Unicode.
 */
    পূর্ণসংখ্যা
vim_isprintc(পূর্ণসংখ্যা c)
{
    যদি (enc_utf8 && c >= 0x100)
	ফেরত utf_printable(c);
    ফেরত (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Strict version of vim_isprintc(c), don't return TRUE if "c" is the head
 * byte of a double-byte character.
 */
    পূর্ণসংখ্যা
vim_isprintc_strict(পূর্ণসংখ্যা c)
{
    যদি (enc_dbcs != 0 && c < 0x100 && MB_BYTE2LEN(c) > 1)
	ফেরত FALSE;
    যদি (enc_utf8 && c >= 0x100)
	ফেরত utf_printable(c);
    ফেরত (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Prepare the structure passed to chartabsize functions.
 * "line" is the start of the line, "ptr" is the first relevant character.
 * When "lnum" is zero do not use text properties that insert text.
 */
    শূন্য
init_chartabsize_arg(
	chartabsize_T	*cts,
	win_T		*wp,
	linenr_T	lnum UNUSED,
	colnr_T		col,
	char_u		*সারি,
	char_u		*ptr)
{
    CLEAR_POINTER(cts);
    cts->cts_win = wp;
    cts->cts_vcol = col;
    cts->cts_সারি = সারি;
    cts->cts_ptr = ptr;
#ifdef FEAT_LINEBREAK
    cts->cts_bri_আকার = -1;
#endif
#ifdef FEAT_PROP_POPUP
    যদি (lnum > 0 && !ignore_text_props)
    {
	char_u	*prop_start;
	পূর্ণসংখ্যা	গুনতি;

	গুনতি = get_text_props(wp->w_বাফার, lnum, &prop_start, FALSE);
	cts->cts_text_prop_গুনতি = গুনতি;
	যদি (গুনতি > 0)
	{
	    // Make a copy of the properties, so that they are properly
	    // aligned.  Make it twice as long for the sorting below.
	    cts->cts_text_props = ALLOC_MULT(textprop_T, গুনতি * 2);
	    যদি (cts->cts_text_props == ফাঁকা)
		cts->cts_text_prop_গুনতি = 0;
	    অন্যথা
	    {
		পূর্ণসংখ্যা	i;

		mch_memmove(cts->cts_text_props + গুনতি, prop_start,
						   গুনতি * আকার(textprop_T));
		জন্য (i = 0; i < গুনতি; ++i)
		{
		    textprop_T *tp = cts->cts_text_props + i + গুনতি;
		    যদি (tp->tp_id < 0
				     && text_prop_type_valid(wp->w_বাফার, tp))
		    {
			cts->cts_has_prop_with_text = TRUE;
			বিরতি;
		    }
		}
		যদি (!cts->cts_has_prop_with_text)
		{
		    // won't use the text properties, free them
		    VIM_CLEAR(cts->cts_text_props);
		    cts->cts_text_prop_গুনতি = 0;
		}
		অন্যথা
		{
		    পূর্ণসংখ্যা	    *text_prop_idxs;

		    // Need to sort the array to get any truncation right.
		    // Do the sorting in the second part of the array, then
		    // move the sorted props to the first part of the array.
		    text_prop_idxs = ALLOC_MULT(পূর্ণসংখ্যা, গুনতি);
		    যদি (text_prop_idxs != ফাঁকা)
		    {
			জন্য (i = 0; i < গুনতি; ++i)
			    text_prop_idxs[i] = i + গুনতি;
			sort_text_props(curbuf, cts->cts_text_props,
							text_prop_idxs, গুনতি);
			// Here we want the reverse order.
			জন্য (i = 0; i < গুনতি; ++i)
			    cts->cts_text_props[গুনতি - i - 1] =
					cts->cts_text_props[text_prop_idxs[i]];
			vim_free(text_prop_idxs);
		    }

		    // Convert tp_text_offset to tp_text pointer.
		    char_u *গুনতি_ptr = prop_start - PROP_গুনতি_আকার;

		    জন্য (i = 0; i < গুনতি; ++i)
		    {
			textprop_T *tp = &cts->cts_text_props[i];

			যদি (tp->tp_id < 0 && tp->u.tp_text_offset > 0)
			{
			    tp->u.tp_text = গুনতি_ptr + tp->u.tp_text_offset;
			    tp->tp_flags |= TP_FLAG_VTEXT_PTR;
			}
			অন্যথা
			    tp->u.tp_text = ফাঁকা;
		    }
		}
	    }
	}
    }
#endif
}

/*
 * Free any allocated item in "cts".
 */
    শূন্য
clear_chartabsize_arg(chartabsize_T *cts UNUSED)
{
#ifdef FEAT_PROP_POPUP
    যদি (cts->cts_text_prop_গুনতি > 0)
    {
	VIM_CLEAR(cts->cts_text_props);
	cts->cts_text_prop_গুনতি = 0;
    }
#endif
}

/*
 * Like chartabsize(), but also check for line breaks on the screen and text
 * properties that insert text.
 */
    পূর্ণসংখ্যা
lbr_chartabsize(chartabsize_T *cts)
{
#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    যদি (1
# ifdef FEAT_LINEBREAK
	&& !curwin->w_p_lbr && *get_showbreak_মান(curwin) == NUL
							   && !curwin->w_p_bri
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_text
# endif
       )
    {
#endif
	যদি (curwin->w_p_wrap)
	    ফেরত win_nolbr_chartabsize(cts, ফাঁকা);
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, cts->cts_ptr, cts->cts_vcol)
#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    }
    ফেরত win_lbr_chartabsize(cts, ফাঁকা, ফাঁকা);
#endif
}

/*
 * Call lbr_chartabsize() and advance the pointer.
 */
    পূর্ণসংখ্যা
lbr_chartabsize_adv(chartabsize_T *cts)
{
    পূর্ণসংখ্যা		retval;

    retval = lbr_chartabsize(cts);
    MB_PTR_ADV(cts->cts_ptr);
    ফেরত retval;
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
    পূর্ণসংখ্যা
win_lbr_chartabsize(
	chartabsize_T	*cts,
	পূর্ণসংখ্যা		*headp UNUSED,
	পূর্ণসংখ্যা		*tailp UNUSED)
{
    win_T	*wp = cts->cts_win;
#যদি defined(FEAT_PROP_POPUP) || defined(FEAT_LINEBREAK)
    char_u	*সারি = cts->cts_সারি; // start of the line
    পূর্ণসংখ্যা		আকার_2;
#endif
    char_u	*s = cts->cts_ptr;
    colnr_T	vcol = cts->cts_vcol;
#ifdef FEAT_LINEBREAK
    পূর্ণসংখ্যা		mb_added = 0;
    পূর্ণসংখ্যা		n;
    char_u	*sbr;
    পূর্ণসংখ্যা		no_sbr = FALSE;
#endif

#যদি defined(FEAT_PROP_POPUP)
    cts->cts_cur_text_width = 0;
    cts->cts_has_below = false;
    cts->cts_first_char = 0;
#endif

#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    /*
     * No 'linebreak', 'showbreak', 'breakindent' and text properties that
     * insert text: return quickly.
     */
    যদি (win_lbr_is_simple_width(cts))
#endif
    {
	যদি (wp->w_p_wrap)
	    ফেরত win_nolbr_chartabsize(cts, headp);
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_বাফার, s, vcol)
    }

#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    পূর্ণসংখ্যা has_lcs_eol = wp->w_p_list && wp->w_lcs_chars.eol != NUL;
    // Virtual text above the line is on its own screen line, it does not count
    // for the size of a Tab.
    colnr_T tab_vcol = vcol;

# ifdef FEAT_PROP_POPUP
    tab_vcol -= cts->cts_above_width;
# endif

    /*
     * First get the normal size, without 'linebreak' or text properties
     */
    আকার_2 = win_chartabsize(wp, s, tab_vcol);
# ifdef FEAT_LINEBREAK
    যদি (*s == NUL)
    {
	// 1 cell for EOL list char (if present), as opposed to the two cell ^@
	// for a NUL character in the text.
	আকার_2 = has_lcs_eol ? 1 : 0;
    }

    পূর্ণসংখ্যা is_doublewidth = has_mbyte && আকার_2 == 2 && MB_BYTE2LEN(*s) > 1;
# endif

# ifdef FEAT_PROP_POPUP
    যদি (cts->cts_has_prop_with_text)
    {
#  ifdef FEAT_LINEBREAK
	পূর্ণসংখ্যা	    tab_আকার = আকার_2;
#  endif
	পূর্ণসংখ্যা	    charlen = *s == NUL ? 1 : mb_ptr2len(s);
	পূর্ণসংখ্যা	    i;
	পূর্ণসংখ্যা	    col = (পূর্ণসংখ্যা)(s - সারি);


	// The "$" for 'list' mode will go between the EOL and
	// the text prop, account for that.
	যদি (has_lcs_eol)
	{
	    ++vcol;
	    --আকার_2;
	}

	জন্য (i = 0; i < cts->cts_text_prop_গুনতি; ++i)
	{
	    textprop_T	*tp = cts->cts_text_props + i;
	    পূর্ণসংখ্যা		col_off = win_col_off(wp);

	    // Watch out for the text being deleted.  "cts_text_props" is a
	    // copy, the text prop may actually have been removed from the line.
	    যদি (tp->tp_id < 0
		    && ((tp->tp_col - 1 >= col
					     && tp->tp_col - 1 < col + charlen)
		       || (tp->tp_col == MAXCOL
			   && ((tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
				? col == 0
				: s[0] == NUL && cts->cts_with_trailing)))
		    && tp->u.tp_text != ফাঁকা)
	    {
		char_u *p = tp->u.tp_text;

		যদি (p != ফাঁকা)
		{
		    পূর্ণসংখ্যা	cells;

		    যদি (tp->tp_col == MAXCOL)
		    {
			পূর্ণসংখ্যা n_extra = (পূর্ণসংখ্যা)STRLEN(p);

			cells = text_prop_position(wp, tp, vcol,
			     (vcol + আকার_2) % (wp->w_width - col_off) + col_off,
					      &n_extra, &p, ফাঁকা, ফাঁকা, FALSE);
#  ifdef FEAT_LINEBREAK
			যদি (text_prop_no_showbreak(tp))
			    no_sbr = TRUE;  // don't use 'showbreak' now
#  endif
		    }
		    অন্যথা
			cells = vim_strsize(p);
		    যদি (tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
		    {
			cts->cts_first_char += cells;
			যদি (!cts->cts_no_above)
			    cts->cts_cur_text_width += cells;
		    }
		    অন্যথা
		    {
			cts->cts_cur_text_width += cells;
			আকার_2 += cells;
		    }
		    cts->cts_start_incl = tp->tp_flags & TP_FLAG_START_INCL;
#  ifdef FEAT_LINEBREAK
		    যদি (*s == TAB)
		    {
			// tab size changes because of the inserted text
			আকার_2 -= tab_আকার;
			tab_আকার = win_chartabsize(wp, s,
					vcol + আকার_2 - cts->cts_above_width);
			আকার_2 += tab_আকার;
		    }
#  endif
		    যদি (tp->tp_col == MAXCOL && (tp->tp_flags
				& (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW)))
		    {
			// count extra line for property above/below
			++cts->cts_prop_সারিগুলি;
			যদি (tp->tp_flags & TP_FLAG_ALIGN_BELOW)
			    cts->cts_has_below = true;
		    }
		}
	    }
	    যদি (tp->tp_col != MAXCOL && tp->tp_col - 1 > col)
		বিরতি;
	}
	যদি (has_lcs_eol)
	{
	    --vcol;
	    ++আকার_2;
	}
    }
# endif

# ifdef FEAT_LINEBREAK
    যদি (is_doublewidth && wp->w_p_wrap && in_win_border(wp, vcol + আকার_2 - 2))
    {
	++আকার_2;		// Count the ">" in the last column.
	mb_added = 1;
    }

    /*
     * May have to add something for 'breakindent' and/or 'showbreak'
     * string at the start of a screen line.
     */
    পূর্ণসংখ্যা শীর্ষ = mb_added;
    sbr = no_sbr ? empty_option : get_showbreak_মান(wp);
    // When "size" is 0, no new screen line is started.
    যদি (আকার_2 > 0 && wp->w_p_wrap && (*sbr != NUL || wp->w_p_bri))
    {
	পূর্ণসংখ্যা	col_off_পূর্ব = win_col_off(wp);
	পূর্ণসংখ্যা	width2 = wp->w_width - col_off_পূর্ব + win_col_off2(wp);
	colnr_T	wcol = vcol + col_off_পূর্ব;
#  ifdef FEAT_PROP_POPUP
	wcol -= wp->w_virtcol_first_char;
#  endif
	colnr_T	max_শীর্ষ_vcol = cts->cts_max_শীর্ষ_vcol;
	পূর্ণসংখ্যা	added = 0;

	// cells taken by 'showbreak'/'breakindent' before current char
	পূর্ণসংখ্যা	শীর্ষ_পূর্ব = 0;
	যদি (wcol >= wp->w_width)
	{
	    wcol -= wp->w_width;
	    col_off_পূর্ব = wp->w_width - width2;
	    যদি (wcol >= width2 && width2 > 0)
		wcol %= width2;
	    যদি (*sbr != NUL)
		শীর্ষ_পূর্ব += vim_strsize(sbr);
	    যদি (wp->w_p_bri)
	    {
		যদি (cts->cts_bri_আকার < 0)
		    cts->cts_bri_আকার = get_breakindent_win(wp, সারি);
		শীর্ষ_পূর্ব += cts->cts_bri_আকার;
	    }
	    যদি (wcol < শীর্ষ_পূর্ব)
	    {
		শীর্ষ_পূর্ব -= wcol;
		wcol += শীর্ষ_পূর্ব;
		added += শীর্ষ_পূর্ব;
		যদি (max_শীর্ষ_vcol <= 0 || vcol < max_শীর্ষ_vcol)
		    শীর্ষ += শীর্ষ_পূর্ব;
	    }
	    অন্যথা
		শীর্ষ_পূর্ব = 0;
	    wcol += col_off_পূর্ব;
	}

	যদি (wcol + আকার_2 > wp->w_width)
	{
	    // cells taken by 'showbreak'/'breakindent' halfway current char
	    পূর্ণসংখ্যা	শীর্ষ_mid = 0;
	    যদি (*sbr != NUL)
		শীর্ষ_mid += vim_strsize(sbr);
	    যদি (wp->w_p_bri)
	    {
		যদি (cts->cts_bri_আকার < 0)
		    cts->cts_bri_আকার = get_breakindent_win(wp, সারি);
		শীর্ষ_mid += cts->cts_bri_আকার;
	    }
	    যদি (শীর্ষ_mid > 0)
	    {
		// Calculate effective window width.
		পূর্ণসংখ্যা পূর্ব_rem = wp->w_width - wcol;
		পূর্ণসংখ্যা width = width2 - শীর্ষ_mid;

		যদি (width <= 0)
		    width = 1;
		// Divide "size - prev_rem" by "width", rounding up.
		পূর্ণসংখ্যা cnt = (আকার_2 - পূর্ব_rem + width - 1) / width;
		added += cnt * শীর্ষ_mid;

		যদি (max_শীর্ষ_vcol == 0 || vcol + আকার_2 + added < max_শীর্ষ_vcol)
		    শীর্ষ += cnt * শীর্ষ_mid;
		অন্যথা যদি (width2 > 0
			   && max_শীর্ষ_vcol > vcol + শীর্ষ_পূর্ব + পূর্ব_rem)
		    শীর্ষ += (max_শীর্ষ_vcol - (vcol + শীর্ষ_পূর্ব + পূর্ব_rem)
					     + width2 - 1) / width2 * শীর্ষ_mid;
		অন্যথা যদি (max_শীর্ষ_vcol < 0)
		{
		    পূর্ণসংখ্যা off = mb_added;
#  ifdef FEAT_PROP_POPUP
		    যদি (*s != NUL
			     && ((State & MODE_NORMAL) || cts->cts_start_incl))
			off += cts->cts_cur_text_width;
#  endif
		    যদি (off >= পূর্ব_rem)
			শীর্ষ += (1 + (off - পূর্ব_rem) / width) * শীর্ষ_mid;
		}
	    }
	}

	আকার_2 += added;
    }

    যদি (headp != ফাঁকা)
	*headp = শীর্ষ;

    পূর্ণসংখ্যা আকার_before_lbr = আকার_2;
    পূর্ণসংখ্যা need_lbr = FALSE;
    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here.
     */
    যদি (wp->w_p_lbr && wp->w_p_wrap && wp->w_width != 0
	    && VIM_ISBREAK((পূর্ণসংখ্যা)s[0]) && !VIM_ISBREAK((পূর্ণসংখ্যা)s[1]))
    {
	char_u	*t = cts->cts_সারি;
	যখন (VIM_ISBREAK((পূর্ণসংখ্যা)t[0]))
	    t++;
	// 'linebreak' is only needed when not in leading whitespace.
	need_lbr = s >= t;
    }
    যদি (need_lbr)
    {
	/*
	 * Count all characters from first non-blank after a blank up to next
	 * non-blank after a blank.
	 */
	পূর্ণসংখ্যা numberextra = win_col_off(wp);
	colnr_T col_adj = আকার_2 - 1;
	colnr_T colmax = (colnr_T)(wp->w_width - numberextra - col_adj);
	যদি (vcol >= colmax)
	{
	    colmax += col_adj;
	    n = colmax +  win_col_off2(wp);
	    যদি (n > 0)
		colmax += (((vcol + col_adj - colmax) / n) + 1) * n - col_adj;
	}

	colnr_T vcol2 = vcol;
	জন্য (;;)
	{
	    char_u *ps = s;
	    MB_PTR_ADV(s);
	    পূর্ণসংখ্যা c = *s;
	    যদি (!(c != NUL
		    && (VIM_ISBREAK(c)
			|| (!VIM_ISBREAK(c)
			       && (vcol2 == vcol || !VIM_ISBREAK((পূর্ণসংখ্যা)*ps))))))
		বিরতি;

	    vcol2 += win_chartabsize(wp, s, vcol2);
	    যদি (vcol2 >= colmax)		// doesn't fit
	    {
		আকার_2 = colmax - vcol + col_adj;
		বিরতি;
	    }
	}
    }

    যদি (tailp != ফাঁকা)
	*tailp = আকার_2 - আকার_before_lbr;

#  ifdef FEAT_PROP_POPUP
    যদি (cts->cts_first_char > 0 && !cts->cts_no_above)
    {
	// Remember the width for the size of a Tab later in the line.  Use
	// assignment, this may be called more than once for a character.
	cts->cts_above_width = cts->cts_first_char;
	আকার_2 += cts->cts_first_char;
    }
#  endif
# endif
    ফেরত আকার_2;
#endif
}

/*
 * Like win_lbr_chartabsize(), except that we know 'linebreak' is off, 'wrap'
 * is on and there are no properties that insert text.  This means we need to
 * check for a double-byte character that doesn't fit at the end of the screen
 * line.
 * Only uses "cts_win", "cts_ptr" and "cts_vcol" from "cts".
 */
    স্থির পূর্ণসংখ্যা
win_nolbr_chartabsize(
	chartabsize_T	*cts,
	পূর্ণসংখ্যা		*headp)
{
    win_T	*wp = cts->cts_win;
    char_u	*s = cts->cts_ptr;
    colnr_T	col = cts->cts_vcol;
    পূর্ণসংখ্যা		n;

    যদি (*s == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
    {
#ifdef FEAT_VARTABS
	ফেরত tabstop_padding(col, wp->w_বাফার->b_p_ts,
				    wp->w_বাফার->b_p_vts_array);
#অন্যথা
	n = wp->w_বাফার->b_p_ts;
	ফেরত (পূর্ণসংখ্যা)(n - (col % n));
#endif
    }
    n = ptr2cells(s);
    // Add one cell for a double-width character in the last column of the
    // window, displayed with a ">".
    যদি (n == 2 && MB_BYTE2LEN(*s) > 1 && in_win_border(wp, col))
    {
	যদি (headp != ফাঁকা)
	    *headp = 1;
	ফেরত 3;
    }
    ফেরত n;
}

/*
 * Return TRUE if virtual column "vcol" is in the rightmost column of window
 * "wp".
 */
    স্থির পূর্ণসংখ্যা
in_win_border(win_T *wp, colnr_T vcol)
{
    পূর্ণসংখ্যা		width1;		// width of first line (after line number)
    পূর্ণসংখ্যা		width2;		// width of further lines

    যদি (wp->w_width == 0)	// there is no border
	ফেরত FALSE;
    width1 = wp->w_width - win_col_off(wp);
    যদি ((পূর্ণসংখ্যা)vcol < width1 - 1)
	ফেরত FALSE;
    যদি ((পূর্ণসংখ্যা)vcol == width1 - 1)
	ফেরত TRUE;
    width2 = width1 + win_col_off2(wp);
    যদি (width2 <= 0)
	ফেরত FALSE;
    ফেরত ((vcol - width1) % width2 == width2 - 1);
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
    শূন্য
getvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*start,
    colnr_T	*cursor,
    colnr_T	*end,
    পূর্ণসংখ্যা		flags)
{
    colnr_T	vcol;
    char_u	*ptr;		// points to current char
    char_u	*সারি;		// start of the line
    পূর্ণসংখ্যা		incr;
    পূর্ণসংখ্যা		শীর্ষ;
    পূর্ণসংখ্যা		লেজ;
#ifdef FEAT_VARTABS
    পূর্ণসংখ্যা		*vts = wp->w_বাফার->b_p_vts_array;
#endif
    পূর্ণসংখ্যা		ts = wp->w_বাফার->b_p_ts;
    পূর্ণসংখ্যা		c;
    chartabsize_T cts;
#ifdef FEAT_PROP_POPUP
    পূর্ণসংখ্যা		on_NUL = FALSE;
#endif

    vcol = 0;
    সারি = ptr = ml_get_buf(wp->w_বাফার, pos->lnum, FALSE);

    init_chartabsize_arg(&cts, wp, pos->lnum, 0, সারি, সারি);
    cts.cts_max_শীর্ষ_vcol = -1;
#ifdef FEAT_PROP_POPUP
    cts.cts_no_above = true;
#endif

    /*
     * This function is used very often, do some speed optimizations.
     * When 'list', 'linebreak', 'showbreak' and 'breakindent' are not set
     * and there are no text properties with "text" use a simple loop.
     * Also use this when 'list' is set but tabs take their normal size.
     */
    যদি ((!wp->w_p_list || wp->w_lcs_chars.tab1 != NUL)
#ifdef FEAT_LINEBREAK
	    && !wp->w_p_lbr && *get_showbreak_মান(wp) == NUL && !wp->w_p_bri
#endif
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
       )
    {
	জন্য (;;)
	{
	    শীর্ষ = 0;
	    c = *ptr;
	    // make sure we don't go past the end of the line
	    যদি (c == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
		বিরতি;
	    }
	    // A tab gets expanded, depending on the current column
	    যদি (c == TAB)
#ifdef FEAT_VARTABS
		incr = tabstop_padding(vcol, ts, vts);
#অন্যথা
		incr = ts - (vcol % ts);
#endif
	    অন্যথা
	    {
		যদি (has_mbyte)
		{
		    // For utf-8, if the byte is >= 0x80, need to look at
		    // further bytes to find the cell width.
		    যদি (enc_utf8 && c >= 0x80)
			incr = utf_ptr2cells(ptr);
		    অন্যথা
			incr = g_chartab[c] & CT_CELL_MASK;

		    // If a double-cell char doesn't fit at the end of a line
		    // it wraps to the next line, it's like this char is three
		    // cells wide.
		    যদি (incr == 2 && wp->w_p_wrap && MB_BYTE2LEN(*ptr) > 1
			    && in_win_border(wp, vcol))
		    {
			++incr;
			শীর্ষ = 1;
		    }
		}
		অন্যথা
		    incr = g_chartab[c] & CT_CELL_MASK;
	    }

	    char_u *পরবর্তী_ptr = ptr + (*mb_ptr2len)(ptr);
	    যদি (পরবর্তী_ptr - সারি > pos->col) // character at pos->col
		বিরতি;

	    vcol += incr;
	    ptr = পরবর্তী_ptr;
	}

	লেজ = 0;
    }
    অন্যথা
    {
	জন্য (;;)
	{
	    // A tab gets expanded, depending on the current column.
	    // Other things also take up space.
	    শীর্ষ = 0;
	    লেজ = 0;
	    incr = win_lbr_chartabsize(&cts, &শীর্ষ, &লেজ);
#ifdef FEAT_PROP_POPUP
	    যদি (cts.cts_ptr == cts.cts_সারি)
	    {
		যদি (flags & GETVCOL_FOR_VIRTCOL)
		    // do not count the virtual text above for w_curswant
		    wp->w_virtcol_first_char = cts.cts_first_char;
		যদি ((flags & GETVCOL_NO_ABOVE) == 0)
		{
		    cts.cts_vcol += cts.cts_first_char;
		    cts.cts_above_width = cts.cts_first_char;
		}
	    }
#endif
	    // make sure we don't go past the end of the line
	    যদি (*cts.cts_ptr == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
#ifdef FEAT_PROP_POPUP
		incr += cts.cts_cur_text_width;
		on_NUL = TRUE;
#endif
		বিরতি;
	    }

	    char_u *পরবর্তী_ptr = cts.cts_ptr + (*mb_ptr2len)(cts.cts_ptr);
	    যদি (পরবর্তী_ptr - সারি > pos->col) // character at pos->col
		বিরতি;

	    cts.cts_vcol += incr;
	    cts.cts_ptr = পরবর্তী_ptr;
	}
	vcol = cts.cts_vcol;
	ptr = cts.cts_ptr;
    }
    clear_chartabsize_arg(&cts);

    যদি (*ptr == NUL && pos->col < MAXCOL && pos->col > ptr - সারি)
	pos->col = ptr - সারি;

    যদি (start != ফাঁকা)
	*start = vcol + শীর্ষ;
    যদি (end != ফাঁকা)
	*end = vcol + incr - (flags & GETVCOL_END_EXCL_LBR ? লেজ : 0) - 1;
    যদি (cursor != ফাঁকা)
    {
	যদি (*ptr == TAB
		&& (State & MODE_NORMAL)
		&& !wp->w_p_list
		&& !virtual_active()
		&& !(VIsual_active
				&& (*p_sel == 'e' || LTOREQ_POS(*pos, VIsual)))
		)
	    // TODO: subtracting "tail" may lead to better cursor position
	    *cursor = vcol + incr - 1;	    // cursor at end
	অন্যথা
	{
#ifdef FEAT_PROP_POPUP
	    // in Insert mode, if "start_incl" is true the text gets inserted
	    // after the virtual text, thus add its width
	    যদি (((State & MODE_ঢোকাও) == 0 || cts.cts_start_incl) && !on_NUL)
		// cursor is after inserted text, unless on the NUL
		vcol += cts.cts_cur_text_width;
#endif
	    *cursor = vcol + শীর্ষ;	    // cursor at start
	}
    }
}

/*
 * Get virtual cursor column in the current window, pretending 'list' is off.
 */
    colnr_T
getvcol_nolist(pos_T *posp)
{
    পূর্ণসংখ্যা		list_save = curwin->w_p_list;
    colnr_T	vcol;

    curwin->w_p_list = FALSE;
    যদি (posp->coladd)
	getvvcol(curwin, posp, ফাঁকা, &vcol, ফাঁকা, 0);
    অন্যথা
	getvcol(curwin, posp, ফাঁকা, &vcol, ফাঁকা, 0);
    curwin->w_p_list = list_save;
    ফেরত vcol;
}

/*
 * Get virtual column in virtual mode.
 */
    শূন্য
getvvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*start,
    colnr_T	*cursor,
    colnr_T	*end,
    পূর্ণসংখ্যা		flags)
{
    colnr_T	col;
    colnr_T	coladd;
    colnr_T	endadd;
    char_u	*ptr;

    যদি (virtual_active())
    {
	// For virtual mode, only want one value
	getvcol(wp, pos, &col, ফাঁকা, ফাঁকা, flags);

	coladd = pos->coladd;
	endadd = 0;
	// Cannot put the cursor on part of a wide character.
	ptr = ml_get_buf(wp->w_বাফার, pos->lnum, FALSE);
	যদি (pos->col < ml_get_buf_len(wp->w_বাফার, pos->lnum))
	{
	    পূর্ণসংখ্যা c = (*mb_ptr2char)(ptr + pos->col);

	    যদি (c != TAB && vim_isprintc(c))
	    {
		endadd = (colnr_T)(char2cells(c) - 1);
		যদি (coladd > endadd)	// past end of line
		    endadd = 0;
		অন্যথা
		    coladd = 0;
	    }
	}
	col += coladd;
	যদি (start != ফাঁকা)
	    *start = col;
	যদি (cursor != ফাঁকা)
	    *cursor = col;
	যদি (end != ফাঁকা)
	    *end = col + endadd;
    }
    অন্যথা
	getvcol(wp, pos, start, cursor, end, flags);
}

/*
 * Get the leftmost and rightmost virtual column of pos1 and pos2.
 * Used for Visual block mode.
 */
    শূন্য
getvcols(
    win_T	*wp,
    pos_T	*pos1,
    pos_T	*pos2,
    colnr_T	*বাম,
    colnr_T	*ডান,
    পূর্ণসংখ্যা		flags)
{
    colnr_T	from1, from2, to1, to2;

    যদি (LT_POSP(pos1, pos2))
    {
	getvvcol(wp, pos1, &from1, ফাঁকা, &to1, flags);
	getvvcol(wp, pos2, &from2, ফাঁকা, &to2, flags);
    }
    অন্যথা
    {
	getvvcol(wp, pos2, &from1, ফাঁকা, &to1, flags);
	getvvcol(wp, pos1, &from2, ফাঁকা, &to2, flags);
    }
    যদি (from2 < from1)
	*বাম = from2;
    অন্যথা
	*বাম = from1;
    যদি (to2 > to1)
    {
	যদি (*p_sel == 'e' && from2 - 1 >= to1)
	    *ডান = from2 - 1;
	অন্যথা
	    *ডান = to2;
    }
    অন্যথা
	*ডান = to1;
}

/*
 * Skip over ' ' and '\t'.
 */
    char_u *
skipwhite(char_u *q)
{
    char_u	*p = q;

    যখন (VIM_ISWHITE(*p))
	++p;
    ফেরত p;
}

#যদি defined(FEAT_EVAL)
/*
 * skip over ' ', '\t' and '\n'.
 */
    char_u *
skipwhite_and_nl(char_u *q)
{
    char_u	*p = q;

    যখন (VIM_ISWHITE(*p) || *p == NL)
	++p;
    ফেরত p;
}
#endif

/*
 * getwhitecols: return the number of whitespace
 * columns (bytes) at the start of a given line
 */
    পূর্ণসংখ্যা
getwhitecols_curline(শূন্য)
{
    ফেরত getwhitecols(ml_get_curline());
}

    পূর্ণসংখ্যা
getwhitecols(char_u *p)
{
    ফেরত skipwhite(p) - p;
}

/*
 * skip over digits
 */
    char_u *
skipdigits(char_u *q)
{
    char_u	*p = q;

    যখন (VIM_ISDIGIT(*p))	// skip to next non-digit
	++p;
    ফেরত p;
}

#যদি defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * skip over binary digits
 */
    char_u *
skipbin(char_u *q)
{
    char_u	*p = q;

    যখন (vim_isbdigit(*p))	// skip to next non-digit
	++p;
    ফেরত p;
}

/*
 * skip over digits and hex characters
 */
    char_u *
skiphex(char_u *q)
{
    char_u	*p = q;

    যখন (vim_isxdigit(*p))	// skip to next non-digit
	++p;
    ফেরত p;
}
#endif

/*
 * skip to bin digit (or NUL after the string)
 */
    char_u *
skiptobin(char_u *q)
{
    char_u	*p = q;

    যখন (*p != NUL && !vim_isbdigit(*p))	// skip to next digit
	++p;
    ফেরত p;
}

/*
 * skip to digit (or NUL after the string)
 */
    char_u *
skiptodigit(char_u *q)
{
    char_u	*p = q;

    যখন (*p != NUL && !VIM_ISDIGIT(*p))	// skip to next digit
	++p;
    ফেরত p;
}

/*
 * skip to hex character (or NUL after the string)
 */
    char_u *
skiptohex(char_u *q)
{
    char_u	*p = q;

    যখন (*p != NUL && !vim_isxdigit(*p))	// skip to next digit
	++p;
    ফেরত p;
}

/*
 * Variant of isdigit() that can handle characters > 0x100.
 * We don't use isdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 * Use the VIM_ISDIGIT() macro for simple arguments.
 */
    পূর্ণসংখ্যা
vim_isdigit(পূর্ণসংখ্যা c)
{
    ফেরত (c >= '0' && c <= '9');
}

/*
 * Variant of isxdigit() that can handle characters > 0x100.
 * We don't use isxdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 */
    পূর্ণসংখ্যা
vim_isxdigit(পূর্ণসংখ্যা c)
{
    ফেরত (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'f')
	|| (c >= 'A' && c <= 'F');
}

/*
 * Corollary of vim_isdigit and vim_isxdigit() that can handle
 * characters > 0x100.
 */
    পূর্ণসংখ্যা
vim_isbdigit(পূর্ণসংখ্যা c)
{
    ফেরত (c == '0' || c == '1');
}

    স্থির পূর্ণসংখ্যা
vim_isodigit(পূর্ণসংখ্যা c)
{
    ফেরত (c >= '0' && c <= '7');
}

/*
 * Vim's own character class functions.  These exist because many library
 * islower()/toupper() etc. do not work properly: they crash when used with
 * invalid values or can't handle latin1 when the locale is C.
 * Speed is most important here.
 */
#define LATIN1LOWER 'l'
#define LATIN1UPPER 'U'

স্থির char_u latin1flags[257] = "                                                                 UUUUUUUUUUUUUUUUUUUUUUUUUU      llllllllllllllllllllllllll                                                                     UUUUUUUUUUUUUUUUUUUUUUU UUUUUUUllllllllllllllllllllllll llllllll";
স্থির char_u latin1upper[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xf7\xd8\xd9\xda\xdb\xdc\xdd\xde\xff";
স্থির char_u latin1lower[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xd7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    পূর্ণসংখ্যা
vim_islower(পূর্ণসংখ্যা c)
{
    যদি (c <= '@')
	ফেরত FALSE;
    যদি (c >= 0x80)
    {
	যদি (enc_utf8)
	    ফেরত utf_islower(c);
	যদি (c >= 0x100)
	{
#ifdef HAVE_ISWLOWER
	    যদি (has_mbyte)
		ফেরত iswlower(c);
#endif
	    // islower() can't handle these chars and may crash
	    ফেরত FALSE;
	}
	যদি (enc_latin1like)
	    ফেরত (latin1flags[c] & LATIN1LOWER) == LATIN1LOWER;
    }
    ফেরত SAFE_islower(c);
}

    পূর্ণসংখ্যা
vim_isupper(পূর্ণসংখ্যা c)
{
    যদি (c <= '@')
	ফেরত FALSE;
    যদি (c >= 0x80)
    {
	যদি (enc_utf8)
	    ফেরত utf_isupper(c);
	যদি (c >= 0x100)
	{
#ifdef HAVE_ISWUPPER
	    যদি (has_mbyte)
		ফেরত iswupper(c);
#endif
	    // islower() can't handle these chars and may crash
	    ফেরত FALSE;
	}
	যদি (enc_latin1like)
	    ফেরত (latin1flags[c] & LATIN1UPPER) == LATIN1UPPER;
    }
    ফেরত SAFE_isupper(c);
}

    পূর্ণসংখ্যা
vim_isalpha(পূর্ণসংখ্যা c)
{
    ফেরত vim_islower(c) || vim_isupper(c);
}

    পূর্ণসংখ্যা
vim_toupper(পূর্ণসংখ্যা c)
{
    যদি (c <= '@')
	ফেরত c;
    যদি (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	যদি (enc_utf8)
	    ফেরত utf_toupper(c);
	যদি (c >= 0x100)
	{
#ifdef HAVE_TOWUPPER
	    যদি (has_mbyte)
		ফেরত towupper(c);
#endif
	    // toupper() can't handle these chars and may crash
	    ফেরত c;
	}
	যদি (enc_latin1like)
	    ফেরত latin1upper[c];
    }
    যদি (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ফেরত TOUPPER_ASC(c);
    ফেরত TOUPPER_LOC(c);
}

    পূর্ণসংখ্যা
vim_tolower(পূর্ণসংখ্যা c)
{
    যদি (c <= '@')
	ফেরত c;
    যদি (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	যদি (enc_utf8)
	    ফেরত utf_tolower(c);
	যদি (c >= 0x100)
	{
#ifdef HAVE_TOWLOWER
	    যদি (has_mbyte)
		ফেরত towlower(c);
#endif
	    // tolower() can't handle these chars and may crash
	    ফেরত c;
	}
	যদি (enc_latin1like)
	    ফেরত latin1lower[c];
    }
    যদি (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ফেরত TOLOWER_ASC(c);
    ফেরত TOLOWER_LOC(c);
}

/*
 * skiptowhite: skip over text until ' ' or '\t' or NUL.
 */
    char_u *
skiptowhite(char_u *p)
{
    যখন (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    ফেরত p;
}

/*
 * skiptowhite_esc: Like skiptowhite(), but also skip escaped chars
 */
    char_u *
skiptowhite_esc(char_u *p)
{
    যখন (*p != ' ' && *p != '\t' && *p != NUL)
    {
	যদি ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    ফেরত p;
}

/*
 * Get a number from a string and skip over it.
 * Note: the argument is a pointer to a char_u pointer!
 */
    long
getdigits(char_u **pp)
{
    char_u	*p;
    long	retval;

    p = *pp;
    retval = strtol((অক্ষর *)p, ফাঁকা, 10);
    যদি (*p == '-')		// skip negative sign
	++p;
    p = skipdigits(p);		// skip to next non-digit
    *pp = p;
    ফেরত retval;
}

/*
 * Like getdigits() but allow for embedded single quotes.
 */
    long
getdigits_quoted(char_u **pp)
{
    char_u	*p = *pp;
    long	retval = 0;

    যদি (*p == '-')
	++p;
    যখন (VIM_ISDIGIT(*p))
    {
	যদি (retval >= LONG_MAX / 10 - 10)
	    retval = LONG_MAX;
	অন্যথা
	    retval = retval * 10 - '0' + *p;
	++p;
	যদি (in_vim9script() && *p == '\'' && VIM_ISDIGIT(p[1]))
	    ++p;
    }
    যদি (**pp == '-')
    {
	যদি (retval == LONG_MAX)
	    retval = LONG_MIN;
	অন্যথা
	    retval = -retval;
    }
    *pp = p;
    ফেরত retval;
}

/*
 * Return TRUE if "lbuf" is empty or only contains blanks.
 */
    পূর্ণসংখ্যা
vim_isblankline(char_u *lbuf)
{
    char_u	*p;

    p = skipwhite(lbuf);
    ফেরত (*p == NUL || *p == '\r' || *p == '\n');
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
    শূন্য
vim_str2nr(
    char_u		*start,
    পূর্ণসংখ্যা			*prep,	    // return: type of number 0 = decimal, 'x'
				    // or 'X' is hex, '0', 'o' or 'O' is octal,
				    // 'b' or 'B' is bin
    পূর্ণসংখ্যা			*len,	    // return: detected length of number
    পূর্ণসংখ্যা			what,	    // what numbers to recognize
    varnumber_T		*nptr,	    // return: signed result
    uvarnumber_T	*unptr,	    // return: unsigned result
    পূর্ণসংখ্যা			maxlen,     // max length of string to check
    পূর্ণসংখ্যা			strict,     // check strictly
    পূর্ণসংখ্যা			*overflow)  // when not NULL set to TRUE for overflow
{
    char_u	    *ptr = start;
    পূর্ণসংখ্যা		    pre = 0;		// default is decimal
    পূর্ণসংখ্যা		    negative = FALSE;
    uvarnumber_T    un = 0;
    পূর্ণসংখ্যা		    n;

    যদি (len != ফাঁকা)
	*len = 0;

    যদি (ptr[0] == '-')
    {
	negative = TRUE;
	++ptr;
    }

    // Recognize hex, octal, and bin.
    যদি (ptr[0] == '0' && ptr[1] != '8' && ptr[1] != '9'
					       && (maxlen == 0 || maxlen > 1))
    {
	pre = ptr[1];
	যদি ((what & STR2NR_HEX)
		&& (pre == 'X' || pre == 'x') && vim_isxdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // hexadecimal
	    ptr += 2;
	অন্যথা যদি ((what & STR2NR_BIN)
		&& (pre == 'B' || pre == 'b') && vim_isbdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    //দ্বিমিক
	    ptr += 2;
	অন্যথা যদি ((what & STR2NR_OOCT)
		&& (pre == 'O' || pre == 'o') && vim_isodigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // octal with prefix "0o"
	    ptr += 2;
	অন্যথা
	{
	    // decimal or octal, default is decimal
	    pre = 0;
	    যদি (what & STR2NR_OCT)
	    {
		// Don't interpret "0", "08" or "0129" as octal.
		জন্য (n = 1; n != maxlen && VIM_ISDIGIT(ptr[n]); ++n)
		{
		    যদি (ptr[n] > '7')
		    {
			pre = 0;	// can't be octal
			বিরতি;
		    }
		    pre = '0';	// assume octal
		}
	    }
	}
    }

    // Do the conversion manually to avoid sscanf() quirks.
    n = 1;
    যদি (pre == 'B' || pre == 'b'
			     || ((what & STR2NR_BIN) && (what & STR2NR_FORCE)))
    {
	// bin
	যদি (pre != 0)
	    n += 2;	    // skip over "0b"
	যখন ('0' <= *ptr && *ptr <= '1')
	{
	    // avoid ubsan error for overflow
	    যদি (un <= UVARNUM_MAX / 2)
		un = 2 * un + (uvarnumber_T)(*ptr - '0');
	    অন্যথা
	    {
		un = UVARNUM_MAX;
		যদি (overflow != ফাঁকা)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    যদি (n++ == maxlen)
		বিরতি;
	    যদি ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '1')
	    {
		++ptr;
		যদি (n++ == maxlen)
		    বিরতি;
	    }
	}
    }
    অন্যথা যদি (pre == 'O' || pre == 'o' ||
		pre == '0' || ((what & STR2NR_OCT) && (what & STR2NR_FORCE)))
    {
	// octal
	যদি (pre != 0 && pre != '0')
	    n += 2;	    // skip over "0o"
	যখন ('0' <= *ptr && *ptr <= '7')
	{
	    // avoid ubsan error for overflow
	    যদি (un <= UVARNUM_MAX / 8)
		un = 8 * un + (uvarnumber_T)(*ptr - '0');
	    অন্যথা
	    {
		un = UVARNUM_MAX;
		যদি (overflow != ফাঁকা)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    যদি (n++ == maxlen)
		বিরতি;
	    যদি ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '7')
	    {
		++ptr;
		যদি (n++ == maxlen)
		    বিরতি;
	    }
	}
    }
    অন্যথা যদি (pre != 0 || ((what & STR2NR_HEX) && (what & STR2NR_FORCE)))
    {
	// hex
	যদি (pre != 0)
	    n += 2;	    // skip over "0x"
	যখন (vim_isxdigit(*ptr))
	{
	    // avoid ubsan error for overflow
	    যদি (un <= UVARNUM_MAX / 16)
		un = 16 * un + (uvarnumber_T)hex2nr(*ptr);
	    অন্যথা
	    {
		un = UVARNUM_MAX;
		যদি (overflow != ফাঁকা)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    যদি (n++ == maxlen)
		বিরতি;
	    যদি ((what & STR2NR_QUOTE) && *ptr == '\'' && vim_isxdigit(ptr[1]))
	    {
		++ptr;
		যদি (n++ == maxlen)
		    বিরতি;
	    }
	}
    }
    অন্যথা
    {
	// decimal
	যখন (VIM_ISDIGIT(*ptr))
	{
	    uvarnumber_T    digit = (uvarnumber_T)(*ptr - '0');

	    // avoid ubsan error for overflow
	    যদি (un < UVARNUM_MAX / 10
		    || (un == UVARNUM_MAX / 10 && digit <= UVARNUM_MAX % 10))
		un = 10 * un + digit;
	    অন্যথা
	    {
		un = UVARNUM_MAX;
		যদি (overflow != ফাঁকা)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    যদি (n++ == maxlen)
		বিরতি;
	    যদি ((what & STR2NR_QUOTE) && *ptr == '\'' && VIM_ISDIGIT(ptr[1]))
	    {
		++ptr;
		যদি (n++ == maxlen)
		    বিরতি;
	    }
	}
    }

    // Check for an alphanumeric character immediately following, that is
    // most likely a typo.
    যদি (strict && n - 1 != maxlen && ASCII_ISALNUM(*ptr))
	ফেরত;

    যদি (prep != ফাঁকা)
	*prep = pre;
    যদি (len != ফাঁকা)
	*len = (পূর্ণসংখ্যা)(ptr - start);
    যদি (nptr != ফাঁকা)
    {
	যদি (negative)   // account for leading '-' for decimal numbers
	{
	    // avoid ubsan error for overflow
	    যদি (un > VARNUM_MAX)
	    {
		*nptr = VARNUM_MIN;
		যদি (overflow != ফাঁকা)
		    *overflow = TRUE;
	    }
	    অন্যথা
		*nptr = -(varnumber_T)un;
	}
	অন্যথা
	{
	    // prevent a large unsigned number to become negative
	    যদি (un > VARNUM_MAX)
	    {
		un = VARNUM_MAX;
		যদি (overflow != ফাঁকা)
		    *overflow = TRUE;
	    }
	    *nptr = (varnumber_T)un;
	}
    }
    যদি (unptr != ফাঁকা)
	*unptr = un;
}

/*
 * Return the value of a single hex character.
 * Only valid when the argument is '0' - '9', 'A' - 'F' or 'a' - 'f'.
 */
    পূর্ণসংখ্যা
hex2nr(পূর্ণসংখ্যা c)
{
    যদি (c >= 'a' && c <= 'f')
	ফেরত c - 'a' + 10;
    যদি (c >= 'A' && c <= 'F')
	ফেরত c - 'A' + 10;
    ফেরত c - '0';
}

/*
 * Convert two hex characters to a byte.
 * Return -1 if one of the characters is not hex.
 */
    পূর্ণসংখ্যা
hexhex2nr(char_u *p)
{
    যদি (!vim_isxdigit(p[0]) || !vim_isxdigit(p[1]))
	ফেরত -1;
    ফেরত (hex2nr(p[0]) << 4) + hex2nr(p[1]);
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
    পূর্ণসংখ্যা
rem_backslash(char_u *str)
{
#ifdef BACKSLASH_IN_FILENAME
    ফেরত (str[0] == '\\'
	    && str[1] < 0x80
	    && (str[1] == ' '
		|| (str[1] != NUL
		    && str[1] != '*'
		    && str[1] != '?'
		    && !vim_isfilec(str[1]))));
#অন্যথা
    ফেরত (str[0] == '\\' && str[1] != NUL);
#endif
}

/*
 * Halve the number of backslashes in a file name argument.
 * For MS-DOS we only do this if the character after the backslash
 * is not a normal file character.
 */
    শূন্য
backslash_halve(char_u *p)
{
    জন্য ( ; *p; ++p)
	যদি (rem_backslash(p))
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
    যদি (res == ফাঁকা)
	ফেরত p;
    backslash_halve(res);
    ফেরত res;
}
