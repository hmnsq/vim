/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#অন্তর্ভাৱ "vim.h"

#যদি defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#endif

স্থির পূর্ণাঙ্ক parse_isopt(char_u *var, buf_T *buf, পূর্ণাঙ্ক only_check);
স্থির পূর্ণাঙ্ক win_nolbr_chartabsize(chartabsize_T *cts, পূর্ণাঙ্ক *headp);
স্থির unsigned nr2hex(unsigned c);

স্থির পূর্ণাঙ্ক    chartab_initialized = FALSE;

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

স্থির পূর্ণাঙ্ক in_win_border(win_T *wp, colnr_T vcol);

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
    পূর্ণাঙ্ক
init_chartab(শূন্য)
{
    ঘূৰাই buf_init_chartab(curbuf, TRUE);
}

    পূর্ণাঙ্ক
buf_init_chartab(
    buf_T	*buf,
    পূর্ণাঙ্ক		global)		// FALSE: only set buf->b_chartab[]
{
    পূর্ণাঙ্ক		c;
    char_u	*p;
    পূর্ণাঙ্ক		i;

    যদি (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 */
	c = 0;
	যেতিয়ালৈ (c < ' ')
	    g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	যেতিয়ালৈ (c <= '~')
	    g_chartab[c++] = 1 + CT_PRINT_CHAR;
	যেতিয়ালৈ (c < 256)
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
	বাবে (c = 1; c < 256; ++c)
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
	বাবে (c = 0; c < 256; ++c)
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
    বাবে (i = global ? 0 : 3; i <= 3; ++i)
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
	    ঘূৰাই FAIL;
    }

    chartab_initialized = TRUE;
    ঘূৰাই OK;
}

/**
 * Checks the format for the option settings 'iskeyword', 'isident', 'isfname'
 * or 'isprint'.
 * Returns FAIL if has an error, OK otherwise.
 */
	পূর্ণাঙ্ক
check_isopt(char_u *var)
{
    ঘূৰাই parse_isopt(var, রিক্ত, TRUE);
}

    স্থির পূর্ণাঙ্ক
parse_isopt(
	char_u	*var,
	buf_T	*buf,
	পূর্ণাঙ্ক	only_check)	// FALSE: refill g_chartab[]
{
    char_u	*p = var;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		c2;
    পূর্ণাঙ্ক		tilde;
    পূর্ণাঙ্ক		do_isalpha;
    পূর্ণাঙ্ক		trail_comma;

    // Parses the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    // Each option is a list of characters, character numbers or ranges,
    // separated by commas, e.g.: "200-210,x,#-178,-"
    যেতিয়ালৈ (*p)
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
	    ঘূৰাই FAIL;

	trail_comma = *p == ',';
	p = skip_to_option_part(p);
	যদি (trail_comma && *p == NUL)
	    // Trailing comma is not allowed.
	    ঘূৰাই FAIL;

	যদি (only_check)
	    অনুৱর্ত;

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

	যেতিয়ালৈ (c <= c2)
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

    ঘূৰাই OK;
}

/*
 * Translate any special characters in buf[bufsize] in-place.
 * The result is a string with only printable characters, but if there is not
 * enough room, not all characters will be translated.
 */
    শূন্য
trans_characters(
    char_u	*buf,
    পূর্ণাঙ্ক		bufsize)
{
    পূর্ণাঙ্ক		len;		// length of string needing translation
    পূর্ণাঙ্ক		room;		// room in buffer after string
    char_u	*trs;		// translated character
    পূর্ণাঙ্ক		trs_len;	// length of trs[]

    len = (পূর্ণাঙ্ক)STRLEN(buf);
    room = bufsize - len;
    যেতিয়ালৈ (*buf != 0)
    {
	// Assume a multi-byte character doesn't need translation.
	যদি (has_mbyte && (trs_len = (*mb_ptr2len)(buf)) > 1)
	    len -= trs_len;
	অন্যথা
	{
	    trs = transchar_byte(*buf);
	    trs_len = (পূর্ণাঙ্ক)STRLEN(trs);
	    যদি (trs_len > 1)
	    {
		room -= trs_len - 1;
		যদি (room <= 0)
		    ঘূৰাই;
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
    পূর্ণাঙ্ক		l, len, c;
    char_u	hexbuf[11];

    যদি (has_mbyte)
    {
	// Compute the length of the result, taking account of unprintable
	// multi-byte characters.
	len = 0;
	p = s;
	যেতিয়ালৈ (*p != NUL)
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
		    len += (পূর্ণাঙ্ক)STRLEN(hexbuf);
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

    যদি (res == রিক্ত)
	ঘূৰাই রিক্ত;

    char_u *d = res;

    p = s;
    যেতিয়ালৈ (*p != NUL)
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
    ঘূৰাই res;
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
    পূর্ণাঙ্ক		orglen,
    char_u	*buf,
    পূর্ণাঙ্ক		buflen)
{
    garray_T	ga;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		len = orglen;

#define GA_CHAR(i)  ((char_u *)ga.ga_data)[i]
#define GA_PTR(i)   ((char_u *)ga.ga_data + (i))
#define STR_CHAR(i)  (buf == রিক্ত ? GA_CHAR(i) : buf[i])
#define STR_PTR(i)   (buf == রিক্ত ? GA_PTR(i) : buf + (i))

    // Copy "str" into "buf" or allocated memory, unmodified.
    যদি (buf == রিক্ত)
    {
	ga_init2(&ga, 1, 10);
	যদি (ga_grow(&ga, len + 1) == FAIL)
	    ঘূৰাই রিক্ত;
	mch_memmove(ga.ga_data, str, (size_t)len);
	ga.ga_len = len;
    }
    অন্যথা
    {
	যদি (len >= buflen)	    // Ugly!
	    len = buflen - 1;
	mch_memmove(buf, str, (size_t)len);
    }
    যদি (buf == রিক্ত)
	GA_CHAR(len) = NUL;
    অন্যথা
	buf[len] = NUL;

    // Make each character lower case.
    i = 0;
    যেতিয়ালৈ (STR_CHAR(i) != NUL)
    {
	যদি (enc_utf8 || (has_mbyte && MB_BYTE2LEN(STR_CHAR(i)) > 1))
	{
	    যদি (enc_utf8)
	    {
		পূর্ণাঙ্ক	c = utf_ptr2char(STR_PTR(i));
		পূর্ণাঙ্ক	olen = utf_ptr2len(STR_PTR(i));
		পূর্ণাঙ্ক	lc = utf_tolower(c);

		// Only replace the character when it is not an invalid
		// sequence (ASCII character or more than one byte) and
		// utf_tolower() doesn't return the original character.
		যদি ((c < 0x80 || olen > 1) && c != lc)
		{
		    পূর্ণাঙ্ক	    nlen = utf_char2len(lc);

		    // If the byte length changes need to shift the following
		    // characters forward or backward.
		    যদি (olen != nlen)
		    {
			যদি (nlen > olen)
			{
			    যদি (buf == রিক্ত
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
			    যদি (buf == রিক্ত)
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
	    যদি (buf == রিক্ত)
		GA_CHAR(i) = TOLOWER_LOC(GA_CHAR(i));
	    অন্যথা
		buf[i] = TOLOWER_LOC(buf[i]);
	    ++i;
	}
    }

    যদি (buf == রিক্ত)
	ঘূৰাই (char_u *)ga.ga_data;
    ঘূৰাই buf;
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
transchar(পূর্ণাঙ্ক c)
{
    ঘূৰাই transchar_buf(curbuf, c);
}

    char_u *
transchar_buf(buf_T *buf, পূর্ণাঙ্ক c)
{
    পূর্ণাঙ্ক			i;

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
    ঘূৰাই transchar_charbuf;
}

/*
 * Like transchar(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of the current buffer.
 */
    char_u *
transchar_byte(পূর্ণাঙ্ক c)
{
    ঘূৰাই transchar_byte_buf(curbuf, c);
}

/*
 * Like transchar_buf(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of "buf", unless it is NULL.
 */
    char_u *
transchar_byte_buf(buf_T *buf, পূর্ণাঙ্ক c)
{
    যদি (enc_utf8 && c >= 0x80)
    {
	transchar_nonprint(buf, transchar_charbuf, c);
	ঘূৰাই transchar_charbuf;
    }
    ঘূৰাই transchar_buf(buf, c);
}
/*
 * Convert non-printable character to two or more printable characters in
 * "charbuf[]".  "charbuf" needs to be able to hold five bytes.
 * Does NOT work for multi-byte characters, c must be <= 255.
 */
    শূন্য
transchar_nonprint(buf_T *buf, char_u *charbuf, পূর্ণাঙ্ক c)
{
    যদি (c == NL)
	c = NUL;		// we use newline in place of a NUL
    অন্যথা যদি (buf != রিক্ত && c == CAR && get_fileformat(buf) == EOL_MAC)
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
transchar_hex(char_u *buf, পূর্ণাঙ্ক c)
{
    পূর্ণাঙ্ক		i = 0;

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
	ঘূৰাই (c & 0xf) + '0';
    ঘূৰাই (c & 0xf) - 10 + 'a';
}

/*
 * Return number of display cells occupied by byte "b".
 * Caller must make sure 0 <= b <= 255.
 * For multi-byte mode "b" must be the first byte of a character.
 * A TAB is counted as two cells: "^I".
 * For UTF-8 mode this will return 0 for bytes >= 0x80, because the number of
 * cells depends on further bytes.
 */
    পূর্ণাঙ্ক
byte2cells(পূর্ণাঙ্ক b)
{
    যদি (enc_utf8 && b >= 0x80)
	ঘূৰাই 0;
    ঘূৰাই (g_chartab[b] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character "c".
 * "c" can be a special key (negative number) in which case 3 or 4 is returned.
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    পূর্ণাঙ্ক
char2cells(পূর্ণাঙ্ক c)
{
    যদি (IS_SPECIAL(c))
	ঘূৰাই char2cells(K_SECOND(c)) + 2;
    যদি (c >= 0x80)
    {
	// UTF-8: above 0x80 need to check the value
	যদি (enc_utf8)
	    ঘূৰাই utf_char2cells(c);
	// DBCS: double-byte means double-width, except for euc-jp with first
	// byte 0x8e
	যদি (enc_dbcs != 0 && c >= 0x100)
	{
	    যদি (enc_dbcs == DBCS_JPNU && ((unsigned)c >> 8) == 0x8e)
		ঘূৰাই 1;
	    ঘূৰাই 2;
	}
    }
    ঘূৰাই (g_chartab[c & 0xff] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character at "*p".
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    পূর্ণাঙ্ক
ptr2cells(char_u *p)
{
    যদি (!has_mbyte)
	ঘূৰাই byte2cells(*p);

    // For UTF-8 we need to look at more bytes if the first byte is >= 0x80.
    যদি (enc_utf8 && *p >= 0x80)
	ঘূৰাই utf_ptr2cells(p);
    // For DBCS we can tell the cell count from the first byte.
    ঘূৰাই (g_chartab[*p] & CT_CELL_MASK);
}

/*
 * Return the number of character cells string "s" will take on the screen,
 * counting TABs as two characters: "^I".
 */
    পূর্ণাঙ্ক
vim_strsize(char_u *s)
{
    ঘূৰাই vim_strnsize(s, (পূর্ণাঙ্ক)MAXCOL);
}

/*
 * Return the number of character cells string "s[len]" will take on the
 * screen, counting TABs as two characters: "^I".
 */
    পূর্ণাঙ্ক
vim_strnsize(char_u *s, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক		size = 0;

    যেতিয়ালৈ (*s != NUL && --len >= 0)
    {
	পূর্ণাঙ্ক	    l = (*mb_ptr2len)(s);

	size += ptr2cells(s);
	s += l;
	len -= l - 1;
    }

    ঘূৰাই size;
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
	ঘূৰাই tabstop_padding(col, (buf)->b_p_ts, (buf)->b_p_vts_array); \
    } \
    অন্যথা \
	ঘূৰাই ptr2cells(p);
#অন্যথা
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, col) \
    যদি (*(p) == TAB && (!(wp)->w_p_list || wp->w_lcs_chars.tab1)) \
    { \
	পূর্ণাঙ্ক ts; \
	ts = (buf)->b_p_ts; \
	ঘূৰাই (পূর্ণাঙ্ক)(ts - (col % ts)); \
    } \
    অন্যথা \
	ঘূৰাই ptr2cells(p);
#endif

    পূর্ণাঙ্ক
chartabsize(char_u *p, colnr_T col)
{
    RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, p, col)
}

#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    পূর্ণাঙ্ক
win_chartabsize(win_T *wp, char_u *p, colnr_T col)
{
    RET_WIN_BUF_CHARTABSIZE(wp, wp->w_buffer, p, col)
}
#endif

/*
 * Return the number of characters the string "s" will take on the screen,
 * taking into account the size of a tab.
 * Does not handle text properties, since "s" is not a buffer line.
 */
    পূর্ণাঙ্ক
linetabsize_str(char_u *s)
{
    ঘূৰাই linetabsize_col(0, s);
}

/*
 * Like linetabsize_str(), but "s" starts at column "startcol".
 */
    পূর্ণাঙ্ক
linetabsize_col(পূর্ণাঙ্ক startcol, char_u *s)
{
    chartabsize_T cts;
    vimlong_T vcol;

    init_chartabsize_arg(&cts, curwin, 0, startcol, s, s);
    vcol = cts.cts_vcol;

    যেতিয়ালৈ (*cts.cts_ptr != NUL)
    {
	vcol += lbr_chartabsize_adv(&cts);
	যদি (vcol > MAXCOL)
	{
	    cts.cts_vcol = MAXCOL;
	    ভঙ্গ;
	}
	অন্যথা
	    cts.cts_vcol = (পূর্ণাঙ্ক)vcol;
    }
    clear_chartabsize_arg(&cts);
    ঘূৰাই (পূর্ণাঙ্ক)cts.cts_vcol;
}

/*
 * Like linetabsize_str(), but for a given window instead of the current one.
 * Doesn't count the size of 'listchars' "eol".
 */
    পূর্ণাঙ্ক
win_linetabsize(win_T *wp, linenr_T lnum, char_u *সাৰি, colnr_T len)
{
    chartabsize_T cts;

    init_chartabsize_arg(&cts, wp, lnum, 0, সাৰি, সাৰি);
    win_linetabsize_cts(&cts, len);
    clear_chartabsize_arg(&cts);
    ঘূৰাই (পূর্ণাঙ্ক)cts.cts_vcol;
}

/*
 * Return the number of cells line "lnum" of window "wp" will take on the
 * screen, taking into account the size of a tab and text properties.
 * Doesn't count the size of 'listchars' "eol".
 */
    পূর্ণাঙ্ক
linetabsize(win_T *wp, linenr_T lnum)
{
    ঘূৰাই win_linetabsize(wp, lnum,
		       ml_get_buf(wp->w_buffer, lnum, FALSE), (colnr_T)MAXCOL);
}

/*
 * Like linetabsize(), but counts the size of 'listchars' "eol".
 */
    পূর্ণাঙ্ক
linetabsize_eol(win_T *wp, linenr_T lnum)
{
    ঘূৰাই linetabsize(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Like linetabsize(), but excludes 'above'/'after'/'right'/'below' aligned
 * virtual text, while keeping inline virtual text.
 */
    পূর্ণাঙ্ক
linetabsize_no_outer(win_T *wp, linenr_T lnum)
{
#ifndef FEAT_PROP_POPUP
    ঘূৰাই linetabsize(wp, lnum);
#অন্যথা
    chartabsize_T cts;
    char_u *সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, সাৰি, সাৰি);

    যদি (cts.cts_text_prop_গুনতি)
    {
	পূর্ণাঙ্ক write_idx = 0;
	বাবে (পূর্ণাঙ্ক read_idx = 0; read_idx < cts.cts_text_prop_গুনতি; read_idx++)
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
    ঘূৰাই (পূর্ণাঙ্ক)cts.cts_vcol;
#endif
}

/*
 * Like linetabsize_no_outer(), but counts the size of 'listchars' "eol".
 */
    পূর্ণাঙ্ক
linetabsize_no_outer_eol(win_T *wp, linenr_T lnum)
{
    ঘূৰাই linetabsize_no_outer(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Return TRUE when win_lbr_chartabsize() does nothing more than
 * win_nolbr_chartabsize(): no 'linebreak', 'breakindent', 'showbreak' and no
 * text properties that insert text.
 */
#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    স্থির পূর্ণাঙ্ক
win_lbr_is_simple_width(chartabsize_T *cts)
{
    ঘূৰাই TRUE
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
    পূর্ণাঙ্ক simple_width = cts->cts_win->w_p_wrap && win_lbr_is_simple_width(cts);
#endif
#ifdef FEAT_PROP_POPUP
    cts->cts_with_trailing = len == MAXCOL;
#endif
    বাবে ( ; *cts->cts_ptr != NUL && (len == MAXCOL || cts->cts_ptr < cts->cts_সাৰি + len);
						      MB_PTR_ADV(cts->cts_ptr))
    {
#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
	যদি (simple_width)
	{
	    পূর্ণাঙ্ক	c = *cts->cts_ptr;

	    যদি (c < 0x80 && c != TAB)
		vcol += g_chartab[c] & CT_CELL_MASK;
	    অন্যথা
		vcol += win_nolbr_chartabsize(cts, রিক্ত);
	}
	অন্যথা
#endif
	    vcol += win_lbr_chartabsize(cts, রিক্ত, রিক্ত);
	যদি (vcol > MAXCOL)
	{
	    cts->cts_vcol = MAXCOL;
	    ভঙ্গ;
	}
	অন্যথা
	    cts->cts_vcol = (পূর্ণাঙ্ক)vcol;
    }
#ifdef FEAT_PROP_POPUP
    // check for a virtual text at the end of a line or on an empty line
    যদি (len == MAXCOL && cts->cts_has_prop_with_text && *cts->cts_ptr == NUL)
    {
	পূর্ণাঙ্ক head = 0;
	(শূন্য)win_lbr_chartabsize(cts, &head, রিক্ত);
	vcol += cts->cts_cur_text_width + head;
	// When properties are above the empty line must also be counted.  For
	// a property below the width already includes filling up the line.
	যদি (cts->cts_ptr == cts->cts_সাৰি && cts->cts_prop_সাৰিগুলি > 0
							 && !cts->cts_has_below)
	    ++vcol;
	cts->cts_vcol = vcol > MAXCOL ? MAXCOL : (পূর্ণাঙ্ক)vcol;
    }
#endif
}

/*
 * Return TRUE if 'c' is a normal identifier character:
 * Letters and characters from the 'isident' option.
 */
    পূর্ণাঙ্ক
vim_isIDc(পূর্ণাঙ্ক c)
{
    ঘূৰাই (c > 0 && c < 0x100 && (g_chartab[c] & CT_ID_CHAR));
}

/*
 * Like vim_isIDc() but not using the 'isident' option: letters, numbers and
 * underscore.
 */
    পূর্ণাঙ্ক
vim_isNormalIDc(পূর্ণাঙ্ক c)
{
    ঘূৰাই ASCII_ISALNUM(c) || c == '_';
}

/*
 * return TRUE if 'c' is a keyword character: Letters and characters from
 * 'iskeyword' option for the current buffer.
 * For multi-byte characters mb_get_class() is used (builtin rules).
 */
    পূর্ণাঙ্ক
vim_iswordc(পূর্ণাঙ্ক c)
{
    ঘূৰাই vim_iswordc_buf(c, curbuf);
}

    পূর্ণাঙ্ক
vim_iswordc_buf(পূর্ণাঙ্ক c, buf_T *buf)
{
    যদি (c >= 0x100)
    {
	যদি (enc_dbcs != 0)
	    ঘূৰাই dbcs_class((unsigned)c >> 8, (unsigned)(c & 0xff)) >= 2;
	যদি (enc_utf8)
	    ঘূৰাই utf_class_buf(c, buf) >= 2;
	ঘূৰাই FALSE;
    }
    ঘূৰাই (c > 0 && GET_CHARTAB(buf, c) != 0);
}

/*
 * Just like vim_iswordc() but uses a pointer to the (multi-byte) character.
 */
    পূর্ণাঙ্ক
vim_iswordp(char_u *p)
{
    ঘূৰাই vim_iswordp_buf(p, curbuf);
}

    পূর্ণাঙ্ক
vim_iswordp_buf(char_u *p, buf_T *buf)
{
    পূর্ণাঙ্ক	c = *p;

    যদি (has_mbyte && MB_BYTE2LEN(c) > 1)
	c = (*mb_ptr2char)(p);
    ঘূৰাই vim_iswordc_buf(c, buf);
}

/*
 * Return TRUE if 'c' is a valid file-name character as specified with the
 * 'isfname' option.
 * Assume characters above 0x100 are valid (multi-byte).
 * To be used for commands like "gf".
 */
    পূর্ণাঙ্ক
vim_isfilec(পূর্ণাঙ্ক c)
{
    ঘূৰাই (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_FNAME_CHAR)));
}

#যদি defined(FEAT_SPELL)
/*
 * Return TRUE if 'c' is a valid file-name character, including characters left
 * out of 'isfname' to make "gf" work, such as comma, space, '@', etc.
 */
    পূর্ণাঙ্ক
vim_is_fname_char(পূর্ণাঙ্ক c)
{
    ঘূৰাই vim_isfilec(c) || c == ',' || c == ' ' || c == '@';
}
#endif

/*
 * return TRUE if 'c' is a valid file-name character or a wildcard character
 * Assume characters above 0x100 are valid (multi-byte).
 * Explicitly interpret ']' as a wildcard character as mch_has_wildcard("]")
 * returns false.
 */
    পূর্ণাঙ্ক
vim_isfilec_or_wc(পূর্ণাঙ্ক c)
{
    char_u buf[2];

    buf[0] = (char_u)c;
    buf[1] = NUL;
    ঘূৰাই vim_isfilec(c) || c == ']' || mch_has_wildcard(buf);
}

/*
 * Return TRUE if 'c' is a printable character.
 * Assume characters above 0x100 are printable (multi-byte), except for
 * Unicode.
 */
    পূর্ণাঙ্ক
vim_isprintc(পূর্ণাঙ্ক c)
{
    যদি (enc_utf8 && c >= 0x100)
	ঘূৰাই utf_printable(c);
    ঘূৰাই (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Strict version of vim_isprintc(c), don't return TRUE if "c" is the head
 * byte of a double-byte character.
 */
    পূর্ণাঙ্ক
vim_isprintc_strict(পূর্ণাঙ্ক c)
{
    যদি (enc_dbcs != 0 && c < 0x100 && MB_BYTE2LEN(c) > 1)
	ঘূৰাই FALSE;
    যদি (enc_utf8 && c >= 0x100)
	ঘূৰাই utf_printable(c);
    ঘূৰাই (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
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
	char_u		*সাৰি,
	char_u		*ptr)
{
    CLEAR_POINTER(cts);
    cts->cts_win = wp;
    cts->cts_vcol = col;
    cts->cts_সাৰি = সাৰি;
    cts->cts_ptr = ptr;
#ifdef FEAT_LINEBREAK
    cts->cts_bri_size = -1;
#endif
#ifdef FEAT_PROP_POPUP
    যদি (lnum > 0 && !ignore_text_props)
    {
	char_u	*prop_start;
	পূর্ণাঙ্ক	গুনতি;

	গুনতি = get_text_props(wp->w_buffer, lnum, &prop_start, FALSE);
	cts->cts_text_prop_গুনতি = গুনতি;
	যদি (গুনতি > 0)
	{
	    // Make a copy of the properties, so that they are properly
	    // aligned.  Make it twice as long for the sorting below.
	    cts->cts_text_props = ALLOC_MULT(textprop_T, গুনতি * 2);
	    যদি (cts->cts_text_props == রিক্ত)
		cts->cts_text_prop_গুনতি = 0;
	    অন্যথা
	    {
		পূর্ণাঙ্ক	i;

		mch_memmove(cts->cts_text_props + গুনতি, prop_start,
						   গুনতি * পরিমাণ(textprop_T));
		বাবে (i = 0; i < গুনতি; ++i)
		{
		    textprop_T *tp = cts->cts_text_props + i + গুনতি;
		    যদি (tp->tp_id < 0
				     && text_prop_type_valid(wp->w_buffer, tp))
		    {
			cts->cts_has_prop_with_text = TRUE;
			ভঙ্গ;
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
		    পূর্ণাঙ্ক	    *text_prop_idxs;

		    // Need to sort the array to get any truncation right.
		    // Do the sorting in the second part of the array, then
		    // move the sorted props to the first part of the array.
		    text_prop_idxs = ALLOC_MULT(পূর্ণাঙ্ক, গুনতি);
		    যদি (text_prop_idxs != রিক্ত)
		    {
			বাবে (i = 0; i < গুনতি; ++i)
			    text_prop_idxs[i] = i + গুনতি;
			sort_text_props(curbuf, cts->cts_text_props,
							text_prop_idxs, গুনতি);
			// Here we want the reverse order.
			বাবে (i = 0; i < গুনতি; ++i)
			    cts->cts_text_props[গুনতি - i - 1] =
					cts->cts_text_props[text_prop_idxs[i]];
			vim_free(text_prop_idxs);
		    }

		    // Convert tp_text_offset to tp_text pointer.
		    char_u *গুনতি_ptr = prop_start - PROP_গুনতি_SIZE;

		    বাবে (i = 0; i < গুনতি; ++i)
		    {
			textprop_T *tp = &cts->cts_text_props[i];

			যদি (tp->tp_id < 0 && tp->u.tp_text_offset > 0)
			{
			    tp->u.tp_text = গুনতি_ptr + tp->u.tp_text_offset;
			    tp->tp_flags |= TP_FLAG_VTEXT_PTR;
			}
			অন্যথা
			    tp->u.tp_text = রিক্ত;
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
    পূর্ণাঙ্ক
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
	    ঘূৰাই win_nolbr_chartabsize(cts, রিক্ত);
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, cts->cts_ptr, cts->cts_vcol)
#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    }
    ঘূৰাই win_lbr_chartabsize(cts, রিক্ত, রিক্ত);
#endif
}

/*
 * Call lbr_chartabsize() and advance the pointer.
 */
    পূর্ণাঙ্ক
lbr_chartabsize_adv(chartabsize_T *cts)
{
    পূর্ণাঙ্ক		retval;

    retval = lbr_chartabsize(cts);
    MB_PTR_ADV(cts->cts_ptr);
    ঘূৰাই retval;
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
    পূর্ণাঙ্ক
win_lbr_chartabsize(
	chartabsize_T	*cts,
	পূর্ণাঙ্ক		*headp UNUSED,
	পূর্ণাঙ্ক		*tailp UNUSED)
{
    win_T	*wp = cts->cts_win;
#যদি defined(FEAT_PROP_POPUP) || defined(FEAT_LINEBREAK)
    char_u	*সাৰি = cts->cts_সাৰি; // start of the line
    পূর্ণাঙ্ক		size;
#endif
    char_u	*s = cts->cts_ptr;
    colnr_T	vcol = cts->cts_vcol;
#ifdef FEAT_LINEBREAK
    পূর্ণাঙ্ক		mb_added = 0;
    পূর্ণাঙ্ক		n;
    char_u	*sbr;
    পূর্ণাঙ্ক		no_sbr = FALSE;
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
	    ঘূৰাই win_nolbr_chartabsize(cts, headp);
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_buffer, s, vcol)
    }

#যদি defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    পূর্ণাঙ্ক has_lcs_eol = wp->w_p_list && wp->w_lcs_chars.eol != NUL;
    // Virtual text above the line is on its own screen line, it does not count
    // for the size of a Tab.
    colnr_T tab_vcol = vcol;

# ifdef FEAT_PROP_POPUP
    tab_vcol -= cts->cts_above_width;
# endif

    /*
     * First get the normal size, without 'linebreak' or text properties
     */
    size = win_chartabsize(wp, s, tab_vcol);
# ifdef FEAT_LINEBREAK
    যদি (*s == NUL)
    {
	// 1 cell for EOL list char (if present), as opposed to the two cell ^@
	// for a NUL character in the text.
	size = has_lcs_eol ? 1 : 0;
    }

    পূর্ণাঙ্ক is_doublewidth = has_mbyte && size == 2 && MB_BYTE2LEN(*s) > 1;
# endif

# ifdef FEAT_PROP_POPUP
    যদি (cts->cts_has_prop_with_text)
    {
#  ifdef FEAT_LINEBREAK
	পূর্ণাঙ্ক	    tab_size = size;
#  endif
	পূর্ণাঙ্ক	    charlen = *s == NUL ? 1 : mb_ptr2len(s);
	পূর্ণাঙ্ক	    i;
	পূর্ণাঙ্ক	    col = (পূর্ণাঙ্ক)(s - সাৰি);


	// The "$" for 'list' mode will go between the EOL and
	// the text prop, account for that.
	যদি (has_lcs_eol)
	{
	    ++vcol;
	    --size;
	}

	বাবে (i = 0; i < cts->cts_text_prop_গুনতি; ++i)
	{
	    textprop_T	*tp = cts->cts_text_props + i;
	    পূর্ণাঙ্ক		col_off = win_col_off(wp);

	    // Watch out for the text being deleted.  "cts_text_props" is a
	    // copy, the text prop may actually have been removed from the line.
	    যদি (tp->tp_id < 0
		    && ((tp->tp_col - 1 >= col
					     && tp->tp_col - 1 < col + charlen)
		       || (tp->tp_col == MAXCOL
			   && ((tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
				? col == 0
				: s[0] == NUL && cts->cts_with_trailing)))
		    && tp->u.tp_text != রিক্ত)
	    {
		char_u *p = tp->u.tp_text;

		যদি (p != রিক্ত)
		{
		    পূর্ণাঙ্ক	cells;

		    যদি (tp->tp_col == MAXCOL)
		    {
			পূর্ণাঙ্ক n_extra = (পূর্ণাঙ্ক)STRLEN(p);

			cells = text_prop_position(wp, tp, vcol,
			     (vcol + size) % (wp->w_width - col_off) + col_off,
					      &n_extra, &p, রিক্ত, রিক্ত, FALSE);
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
			size += cells;
		    }
		    cts->cts_start_incl = tp->tp_flags & TP_FLAG_START_INCL;
#  ifdef FEAT_LINEBREAK
		    যদি (*s == TAB)
		    {
			// tab size changes because of the inserted text
			size -= tab_size;
			tab_size = win_chartabsize(wp, s,
					vcol + size - cts->cts_above_width);
			size += tab_size;
		    }
#  endif
		    যদি (tp->tp_col == MAXCOL && (tp->tp_flags
				& (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW)))
		    {
			// count extra line for property above/below
			++cts->cts_prop_সাৰিগুলি;
			যদি (tp->tp_flags & TP_FLAG_ALIGN_BELOW)
			    cts->cts_has_below = true;
		    }
		}
	    }
	    যদি (tp->tp_col != MAXCOL && tp->tp_col - 1 > col)
		ভঙ্গ;
	}
	যদি (has_lcs_eol)
	{
	    --vcol;
	    ++size;
	}
    }
# endif

# ifdef FEAT_LINEBREAK
    যদি (is_doublewidth && wp->w_p_wrap && in_win_border(wp, vcol + size - 2))
    {
	++size;		// Count the ">" in the last column.
	mb_added = 1;
    }

    /*
     * May have to add something for 'breakindent' and/or 'showbreak'
     * string at the start of a screen line.
     */
    পূর্ণাঙ্ক head = mb_added;
    sbr = no_sbr ? empty_option : get_showbreak_মান(wp);
    // When "size" is 0, no new screen line is started.
    যদি (size > 0 && wp->w_p_wrap && (*sbr != NUL || wp->w_p_bri))
    {
	পূর্ণাঙ্ক	col_off_prev = win_col_off(wp);
	পূর্ণাঙ্ক	width2 = wp->w_width - col_off_prev + win_col_off2(wp);
	colnr_T	wcol = vcol + col_off_prev;
#  ifdef FEAT_PROP_POPUP
	wcol -= wp->w_virtcol_first_char;
#  endif
	colnr_T	max_head_vcol = cts->cts_max_head_vcol;
	পূর্ণাঙ্ক	added = 0;

	// cells taken by 'showbreak'/'breakindent' before current char
	পূর্ণাঙ্ক	head_prev = 0;
	যদি (wcol >= wp->w_width)
	{
	    wcol -= wp->w_width;
	    col_off_prev = wp->w_width - width2;
	    যদি (wcol >= width2 && width2 > 0)
		wcol %= width2;
	    যদি (*sbr != NUL)
		head_prev += vim_strsize(sbr);
	    যদি (wp->w_p_bri)
	    {
		যদি (cts->cts_bri_size < 0)
		    cts->cts_bri_size = get_breakindent_win(wp, সাৰি);
		head_prev += cts->cts_bri_size;
	    }
	    যদি (wcol < head_prev)
	    {
		head_prev -= wcol;
		wcol += head_prev;
		added += head_prev;
		যদি (max_head_vcol <= 0 || vcol < max_head_vcol)
		    head += head_prev;
	    }
	    অন্যথা
		head_prev = 0;
	    wcol += col_off_prev;
	}

	যদি (wcol + size > wp->w_width)
	{
	    // cells taken by 'showbreak'/'breakindent' halfway current char
	    পূর্ণাঙ্ক	head_mid = 0;
	    যদি (*sbr != NUL)
		head_mid += vim_strsize(sbr);
	    যদি (wp->w_p_bri)
	    {
		যদি (cts->cts_bri_size < 0)
		    cts->cts_bri_size = get_breakindent_win(wp, সাৰি);
		head_mid += cts->cts_bri_size;
	    }
	    যদি (head_mid > 0)
	    {
		// Calculate effective window width.
		পূর্ণাঙ্ক prev_rem = wp->w_width - wcol;
		পূর্ণাঙ্ক width = width2 - head_mid;

		যদি (width <= 0)
		    width = 1;
		// Divide "size - prev_rem" by "width", rounding up.
		পূর্ণাঙ্ক cnt = (size - prev_rem + width - 1) / width;
		added += cnt * head_mid;

		যদি (max_head_vcol == 0 || vcol + size + added < max_head_vcol)
		    head += cnt * head_mid;
		অন্যথা যদি (width2 > 0
			   && max_head_vcol > vcol + head_prev + prev_rem)
		    head += (max_head_vcol - (vcol + head_prev + prev_rem)
					     + width2 - 1) / width2 * head_mid;
		অন্যথা যদি (max_head_vcol < 0)
		{
		    পূর্ণাঙ্ক off = mb_added;
#  ifdef FEAT_PROP_POPUP
		    যদি (*s != NUL
			     && ((State & MODE_NORMAL) || cts->cts_start_incl))
			off += cts->cts_cur_text_width;
#  endif
		    যদি (off >= prev_rem)
			head += (1 + (off - prev_rem) / width) * head_mid;
		}
	    }
	}

	size += added;
    }

    যদি (headp != রিক্ত)
	*headp = head;

    পূর্ণাঙ্ক size_before_lbr = size;
    পূর্ণাঙ্ক need_lbr = FALSE;
    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here.
     */
    যদি (wp->w_p_lbr && wp->w_p_wrap && wp->w_width != 0
	    && VIM_ISBREAK((পূর্ণাঙ্ক)s[0]) && !VIM_ISBREAK((পূর্ণাঙ্ক)s[1]))
    {
	char_u	*t = cts->cts_সাৰি;
	যেতিয়ালৈ (VIM_ISBREAK((পূর্ণাঙ্ক)t[0]))
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
	পূর্ণাঙ্ক numberextra = win_col_off(wp);
	colnr_T col_adj = size - 1;
	colnr_T colmax = (colnr_T)(wp->w_width - numberextra - col_adj);
	যদি (vcol >= colmax)
	{
	    colmax += col_adj;
	    n = colmax +  win_col_off2(wp);
	    যদি (n > 0)
		colmax += (((vcol + col_adj - colmax) / n) + 1) * n - col_adj;
	}

	colnr_T vcol2 = vcol;
	বাবে (;;)
	{
	    char_u *ps = s;
	    MB_PTR_ADV(s);
	    পূর্ণাঙ্ক c = *s;
	    যদি (!(c != NUL
		    && (VIM_ISBREAK(c)
			|| (!VIM_ISBREAK(c)
			       && (vcol2 == vcol || !VIM_ISBREAK((পূর্ণাঙ্ক)*ps))))))
		ভঙ্গ;

	    vcol2 += win_chartabsize(wp, s, vcol2);
	    যদি (vcol2 >= colmax)		// doesn't fit
	    {
		size = colmax - vcol + col_adj;
		ভঙ্গ;
	    }
	}
    }

    যদি (tailp != রিক্ত)
	*tailp = size - size_before_lbr;

#  ifdef FEAT_PROP_POPUP
    যদি (cts->cts_first_char > 0 && !cts->cts_no_above)
    {
	// Remember the width for the size of a Tab later in the line.  Use
	// assignment, this may be called more than once for a character.
	cts->cts_above_width = cts->cts_first_char;
	size += cts->cts_first_char;
    }
#  endif
# endif
    ঘূৰাই size;
#endif
}

/*
 * Like win_lbr_chartabsize(), except that we know 'linebreak' is off, 'wrap'
 * is on and there are no properties that insert text.  This means we need to
 * check for a double-byte character that doesn't fit at the end of the screen
 * line.
 * Only uses "cts_win", "cts_ptr" and "cts_vcol" from "cts".
 */
    স্থির পূর্ণাঙ্ক
win_nolbr_chartabsize(
	chartabsize_T	*cts,
	পূর্ণাঙ্ক		*headp)
{
    win_T	*wp = cts->cts_win;
    char_u	*s = cts->cts_ptr;
    colnr_T	col = cts->cts_vcol;
    পূর্ণাঙ্ক		n;

    যদি (*s == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
    {
#ifdef FEAT_VARTABS
	ঘূৰাই tabstop_padding(col, wp->w_buffer->b_p_ts,
				    wp->w_buffer->b_p_vts_array);
#অন্যথা
	n = wp->w_buffer->b_p_ts;
	ঘূৰাই (পূর্ণাঙ্ক)(n - (col % n));
#endif
    }
    n = ptr2cells(s);
    // Add one cell for a double-width character in the last column of the
    // window, displayed with a ">".
    যদি (n == 2 && MB_BYTE2LEN(*s) > 1 && in_win_border(wp, col))
    {
	যদি (headp != রিক্ত)
	    *headp = 1;
	ঘূৰাই 3;
    }
    ঘূৰাই n;
}

/*
 * Return TRUE if virtual column "vcol" is in the rightmost column of window
 * "wp".
 */
    স্থির পূর্ণাঙ্ক
in_win_border(win_T *wp, colnr_T vcol)
{
    পূর্ণাঙ্ক		width1;		// width of first line (after line number)
    পূর্ণাঙ্ক		width2;		// width of further lines

    যদি (wp->w_width == 0)	// there is no border
	ঘূৰাই FALSE;
    width1 = wp->w_width - win_col_off(wp);
    যদি ((পূর্ণাঙ্ক)vcol < width1 - 1)
	ঘূৰাই FALSE;
    যদি ((পূর্ণাঙ্ক)vcol == width1 - 1)
	ঘূৰাই TRUE;
    width2 = width1 + win_col_off2(wp);
    যদি (width2 <= 0)
	ঘূৰাই FALSE;
    ঘূৰাই ((vcol - width1) % width2 == width2 - 1);
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
    পূর্ণাঙ্ক		flags)
{
    colnr_T	vcol;
    char_u	*ptr;		// points to current char
    char_u	*সাৰি;		// start of the line
    পূর্ণাঙ্ক		incr;
    পূর্ণাঙ্ক		head;
    পূর্ণাঙ্ক		tail;
#ifdef FEAT_VARTABS
    পূর্ণাঙ্ক		*vts = wp->w_buffer->b_p_vts_array;
#endif
    পূর্ণাঙ্ক		ts = wp->w_buffer->b_p_ts;
    পূর্ণাঙ্ক		c;
    chartabsize_T cts;
#ifdef FEAT_PROP_POPUP
    পূর্ণাঙ্ক		on_NUL = FALSE;
#endif

    vcol = 0;
    সাৰি = ptr = ml_get_buf(wp->w_buffer, pos->lnum, FALSE);

    init_chartabsize_arg(&cts, wp, pos->lnum, 0, সাৰি, সাৰি);
    cts.cts_max_head_vcol = -1;
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
	বাবে (;;)
	{
	    head = 0;
	    c = *ptr;
	    // make sure we don't go past the end of the line
	    যদি (c == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
		ভঙ্গ;
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
			head = 1;
		    }
		}
		অন্যথা
		    incr = g_chartab[c] & CT_CELL_MASK;
	    }

	    char_u *next_ptr = ptr + (*mb_ptr2len)(ptr);
	    যদি (next_ptr - সাৰি > pos->col) // character at pos->col
		ভঙ্গ;

	    vcol += incr;
	    ptr = next_ptr;
	}

	tail = 0;
    }
    অন্যথা
    {
	বাবে (;;)
	{
	    // A tab gets expanded, depending on the current column.
	    // Other things also take up space.
	    head = 0;
	    tail = 0;
	    incr = win_lbr_chartabsize(&cts, &head, &tail);
#ifdef FEAT_PROP_POPUP
	    যদি (cts.cts_ptr == cts.cts_সাৰি)
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
		ভঙ্গ;
	    }

	    char_u *next_ptr = cts.cts_ptr + (*mb_ptr2len)(cts.cts_ptr);
	    যদি (next_ptr - সাৰি > pos->col) // character at pos->col
		ভঙ্গ;

	    cts.cts_vcol += incr;
	    cts.cts_ptr = next_ptr;
	}
	vcol = cts.cts_vcol;
	ptr = cts.cts_ptr;
    }
    clear_chartabsize_arg(&cts);

    যদি (*ptr == NUL && pos->col < MAXCOL && pos->col > ptr - সাৰি)
	pos->col = ptr - সাৰি;

    যদি (start != রিক্ত)
	*start = vcol + head;
    যদি (end != রিক্ত)
	*end = vcol + incr - (flags & GETVCOL_END_EXCL_LBR ? tail : 0) - 1;
    যদি (cursor != রিক্ত)
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
	    *cursor = vcol + head;	    // cursor at start
	}
    }
}

/*
 * Get virtual cursor column in the current window, pretending 'list' is off.
 */
    colnr_T
getvcol_nolist(pos_T *posp)
{
    পূর্ণাঙ্ক		list_save = curwin->w_p_list;
    colnr_T	vcol;

    curwin->w_p_list = FALSE;
    যদি (posp->coladd)
	getvvcol(curwin, posp, রিক্ত, &vcol, রিক্ত, 0);
    অন্যথা
	getvcol(curwin, posp, রিক্ত, &vcol, রিক্ত, 0);
    curwin->w_p_list = list_save;
    ঘূৰাই vcol;
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
    পূর্ণাঙ্ক		flags)
{
    colnr_T	col;
    colnr_T	coladd;
    colnr_T	endadd;
    char_u	*ptr;

    যদি (virtual_active())
    {
	// For virtual mode, only want one value
	getvcol(wp, pos, &col, রিক্ত, রিক্ত, flags);

	coladd = pos->coladd;
	endadd = 0;
	// Cannot put the cursor on part of a wide character.
	ptr = ml_get_buf(wp->w_buffer, pos->lnum, FALSE);
	যদি (pos->col < ml_get_buf_len(wp->w_buffer, pos->lnum))
	{
	    পূর্ণাঙ্ক c = (*mb_ptr2char)(ptr + pos->col);

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
	যদি (start != রিক্ত)
	    *start = col;
	যদি (cursor != রিক্ত)
	    *cursor = col;
	যদি (end != রিক্ত)
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
    পূর্ণাঙ্ক		flags)
{
    colnr_T	from1, from2, to1, to2;

    যদি (LT_POSP(pos1, pos2))
    {
	getvvcol(wp, pos1, &from1, রিক্ত, &to1, flags);
	getvvcol(wp, pos2, &from2, রিক্ত, &to2, flags);
    }
    অন্যথা
    {
	getvvcol(wp, pos2, &from1, রিক্ত, &to1, flags);
	getvvcol(wp, pos1, &from2, রিক্ত, &to2, flags);
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

    যেতিয়ালৈ (VIM_ISWHITE(*p))
	++p;
    ঘূৰাই p;
}

#যদি defined(FEAT_EVAL)
/*
 * skip over ' ', '\t' and '\n'.
 */
    char_u *
skipwhite_and_nl(char_u *q)
{
    char_u	*p = q;

    যেতিয়ালৈ (VIM_ISWHITE(*p) || *p == NL)
	++p;
    ঘূৰাই p;
}
#endif

/*
 * getwhitecols: return the number of whitespace
 * columns (bytes) at the start of a given line
 */
    পূর্ণাঙ্ক
getwhitecols_curline(শূন্য)
{
    ঘূৰাই getwhitecols(ml_get_curline());
}

    পূর্ণাঙ্ক
getwhitecols(char_u *p)
{
    ঘূৰাই skipwhite(p) - p;
}

/*
 * skip over digits
 */
    char_u *
skipdigits(char_u *q)
{
    char_u	*p = q;

    যেতিয়ালৈ (VIM_ISDIGIT(*p))	// skip to next non-digit
	++p;
    ঘূৰাই p;
}

#যদি defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * skip over binary digits
 */
    char_u *
skipbin(char_u *q)
{
    char_u	*p = q;

    যেতিয়ালৈ (vim_isbdigit(*p))	// skip to next non-digit
	++p;
    ঘূৰাই p;
}

/*
 * skip over digits and hex characters
 */
    char_u *
skiphex(char_u *q)
{
    char_u	*p = q;

    যেতিয়ালৈ (vim_isxdigit(*p))	// skip to next non-digit
	++p;
    ঘূৰাই p;
}
#endif

/*
 * skip to bin digit (or NUL after the string)
 */
    char_u *
skiptobin(char_u *q)
{
    char_u	*p = q;

    যেতিয়ালৈ (*p != NUL && !vim_isbdigit(*p))	// skip to next digit
	++p;
    ঘূৰাই p;
}

/*
 * skip to digit (or NUL after the string)
 */
    char_u *
skiptodigit(char_u *q)
{
    char_u	*p = q;

    যেতিয়ালৈ (*p != NUL && !VIM_ISDIGIT(*p))	// skip to next digit
	++p;
    ঘূৰাই p;
}

/*
 * skip to hex character (or NUL after the string)
 */
    char_u *
skiptohex(char_u *q)
{
    char_u	*p = q;

    যেতিয়ালৈ (*p != NUL && !vim_isxdigit(*p))	// skip to next digit
	++p;
    ঘূৰাই p;
}

/*
 * Variant of isdigit() that can handle characters > 0x100.
 * We don't use isdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 * Use the VIM_ISDIGIT() macro for simple arguments.
 */
    পূর্ণাঙ্ক
vim_isdigit(পূর্ণাঙ্ক c)
{
    ঘূৰাই (c >= '0' && c <= '9');
}

/*
 * Variant of isxdigit() that can handle characters > 0x100.
 * We don't use isxdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 */
    পূর্ণাঙ্ক
vim_isxdigit(পূর্ণাঙ্ক c)
{
    ঘূৰাই (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'f')
	|| (c >= 'A' && c <= 'F');
}

/*
 * Corollary of vim_isdigit and vim_isxdigit() that can handle
 * characters > 0x100.
 */
    পূর্ণাঙ্ক
vim_isbdigit(পূর্ণাঙ্ক c)
{
    ঘূৰাই (c == '0' || c == '1');
}

    স্থির পূর্ণাঙ্ক
vim_isodigit(পূর্ণাঙ্ক c)
{
    ঘূৰাই (c >= '0' && c <= '7');
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

    পূর্ণাঙ্ক
vim_islower(পূর্ণাঙ্ক c)
{
    যদি (c <= '@')
	ঘূৰাই FALSE;
    যদি (c >= 0x80)
    {
	যদি (enc_utf8)
	    ঘূৰাই utf_islower(c);
	যদি (c >= 0x100)
	{
#ifdef HAVE_ISWLOWER
	    যদি (has_mbyte)
		ঘূৰাই iswlower(c);
#endif
	    // islower() can't handle these chars and may crash
	    ঘূৰাই FALSE;
	}
	যদি (enc_latin1like)
	    ঘূৰাই (latin1flags[c] & LATIN1LOWER) == LATIN1LOWER;
    }
    ঘূৰাই SAFE_islower(c);
}

    পূর্ণাঙ্ক
vim_isupper(পূর্ণাঙ্ক c)
{
    যদি (c <= '@')
	ঘূৰাই FALSE;
    যদি (c >= 0x80)
    {
	যদি (enc_utf8)
	    ঘূৰাই utf_isupper(c);
	যদি (c >= 0x100)
	{
#ifdef HAVE_ISWUPPER
	    যদি (has_mbyte)
		ঘূৰাই iswupper(c);
#endif
	    // islower() can't handle these chars and may crash
	    ঘূৰাই FALSE;
	}
	যদি (enc_latin1like)
	    ঘূৰাই (latin1flags[c] & LATIN1UPPER) == LATIN1UPPER;
    }
    ঘূৰাই SAFE_isupper(c);
}

    পূর্ণাঙ্ক
vim_isalpha(পূর্ণাঙ্ক c)
{
    ঘূৰাই vim_islower(c) || vim_isupper(c);
}

    পূর্ণাঙ্ক
vim_toupper(পূর্ণাঙ্ক c)
{
    যদি (c <= '@')
	ঘূৰাই c;
    যদি (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	যদি (enc_utf8)
	    ঘূৰাই utf_toupper(c);
	যদি (c >= 0x100)
	{
#ifdef HAVE_TOWUPPER
	    যদি (has_mbyte)
		ঘূৰাই towupper(c);
#endif
	    // toupper() can't handle these chars and may crash
	    ঘূৰাই c;
	}
	যদি (enc_latin1like)
	    ঘূৰাই latin1upper[c];
    }
    যদি (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ঘূৰাই TOUPPER_ASC(c);
    ঘূৰাই TOUPPER_LOC(c);
}

    পূর্ণাঙ্ক
vim_tolower(পূর্ণাঙ্ক c)
{
    যদি (c <= '@')
	ঘূৰাই c;
    যদি (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	যদি (enc_utf8)
	    ঘূৰাই utf_tolower(c);
	যদি (c >= 0x100)
	{
#ifdef HAVE_TOWLOWER
	    যদি (has_mbyte)
		ঘূৰাই towlower(c);
#endif
	    // tolower() can't handle these chars and may crash
	    ঘূৰাই c;
	}
	যদি (enc_latin1like)
	    ঘূৰাই latin1lower[c];
    }
    যদি (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ঘূৰাই TOLOWER_ASC(c);
    ঘূৰাই TOLOWER_LOC(c);
}

/*
 * skiptowhite: skip over text until ' ' or '\t' or NUL.
 */
    char_u *
skiptowhite(char_u *p)
{
    যেতিয়ালৈ (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    ঘূৰাই p;
}

/*
 * skiptowhite_esc: Like skiptowhite(), but also skip escaped chars
 */
    char_u *
skiptowhite_esc(char_u *p)
{
    যেতিয়ালৈ (*p != ' ' && *p != '\t' && *p != NUL)
    {
	যদি ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    ঘূৰাই p;
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
    retval = strtol((ৱর্ণ *)p, রিক্ত, 10);
    যদি (*p == '-')		// skip negative sign
	++p;
    p = skipdigits(p);		// skip to next non-digit
    *pp = p;
    ঘূৰাই retval;
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
    যেতিয়ালৈ (VIM_ISDIGIT(*p))
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
    ঘূৰাই retval;
}

/*
 * Return TRUE if "lbuf" is empty or only contains blanks.
 */
    পূর্ণাঙ্ক
vim_isblankline(char_u *lbuf)
{
    char_u	*p;

    p = skipwhite(lbuf);
    ঘূৰাই (*p == NUL || *p == '\r' || *p == '\n');
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
    পূর্ণাঙ্ক			*prep,	    // return: type of number 0 = decimal, 'x'
				    // or 'X' is hex, '0', 'o' or 'O' is octal,
				    // 'b' or 'B' is bin
    পূর্ণাঙ্ক			*len,	    // return: detected length of number
    পূর্ণাঙ্ক			what,	    // what numbers to recognize
    varnumber_T		*nptr,	    // return: signed result
    uvarnumber_T	*unptr,	    // return: unsigned result
    পূর্ণাঙ্ক			maxlen,     // max length of string to check
    পূর্ণাঙ্ক			strict,     // check strictly
    পূর্ণাঙ্ক			*overflow)  // when not NULL set to TRUE for overflow
{
    char_u	    *ptr = start;
    পূর্ণাঙ্ক		    pre = 0;		// default is decimal
    পূর্ণাঙ্ক		    negative = FALSE;
    uvarnumber_T    un = 0;
    পূর্ণাঙ্ক		    n;

    যদি (len != রিক্ত)
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
		বাবে (n = 1; n != maxlen && VIM_ISDIGIT(ptr[n]); ++n)
		{
		    যদি (ptr[n] > '7')
		    {
			pre = 0;	// can't be octal
			ভঙ্গ;
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
	যেতিয়ালৈ ('0' <= *ptr && *ptr <= '1')
	{
	    // avoid ubsan error for overflow
	    যদি (un <= UVARNUM_MAX / 2)
		un = 2 * un + (uvarnumber_T)(*ptr - '0');
	    অন্যথা
	    {
		un = UVARNUM_MAX;
		যদি (overflow != রিক্ত)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    যদি (n++ == maxlen)
		ভঙ্গ;
	    যদি ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '1')
	    {
		++ptr;
		যদি (n++ == maxlen)
		    ভঙ্গ;
	    }
	}
    }
    অন্যথা যদি (pre == 'O' || pre == 'o' ||
		pre == '0' || ((what & STR2NR_OCT) && (what & STR2NR_FORCE)))
    {
	// octal
	যদি (pre != 0 && pre != '0')
	    n += 2;	    // skip over "0o"
	যেতিয়ালৈ ('0' <= *ptr && *ptr <= '7')
	{
	    // avoid ubsan error for overflow
	    যদি (un <= UVARNUM_MAX / 8)
		un = 8 * un + (uvarnumber_T)(*ptr - '0');
	    অন্যথা
	    {
		un = UVARNUM_MAX;
		যদি (overflow != রিক্ত)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    যদি (n++ == maxlen)
		ভঙ্গ;
	    যদি ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '7')
	    {
		++ptr;
		যদি (n++ == maxlen)
		    ভঙ্গ;
	    }
	}
    }
    অন্যথা যদি (pre != 0 || ((what & STR2NR_HEX) && (what & STR2NR_FORCE)))
    {
	// hex
	যদি (pre != 0)
	    n += 2;	    // skip over "0x"
	যেতিয়ালৈ (vim_isxdigit(*ptr))
	{
	    // avoid ubsan error for overflow
	    যদি (un <= UVARNUM_MAX / 16)
		un = 16 * un + (uvarnumber_T)hex2nr(*ptr);
	    অন্যথা
	    {
		un = UVARNUM_MAX;
		যদি (overflow != রিক্ত)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    যদি (n++ == maxlen)
		ভঙ্গ;
	    যদি ((what & STR2NR_QUOTE) && *ptr == '\'' && vim_isxdigit(ptr[1]))
	    {
		++ptr;
		যদি (n++ == maxlen)
		    ভঙ্গ;
	    }
	}
    }
    অন্যথা
    {
	// decimal
	যেতিয়ালৈ (VIM_ISDIGIT(*ptr))
	{
	    uvarnumber_T    digit = (uvarnumber_T)(*ptr - '0');

	    // avoid ubsan error for overflow
	    যদি (un < UVARNUM_MAX / 10
		    || (un == UVARNUM_MAX / 10 && digit <= UVARNUM_MAX % 10))
		un = 10 * un + digit;
	    অন্যথা
	    {
		un = UVARNUM_MAX;
		যদি (overflow != রিক্ত)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    যদি (n++ == maxlen)
		ভঙ্গ;
	    যদি ((what & STR2NR_QUOTE) && *ptr == '\'' && VIM_ISDIGIT(ptr[1]))
	    {
		++ptr;
		যদি (n++ == maxlen)
		    ভঙ্গ;
	    }
	}
    }

    // Check for an alphanumeric character immediately following, that is
    // most likely a typo.
    যদি (strict && n - 1 != maxlen && ASCII_ISALNUM(*ptr))
	ঘূৰাই;

    যদি (prep != রিক্ত)
	*prep = pre;
    যদি (len != রিক্ত)
	*len = (পূর্ণাঙ্ক)(ptr - start);
    যদি (nptr != রিক্ত)
    {
	যদি (negative)   // account for leading '-' for decimal numbers
	{
	    // avoid ubsan error for overflow
	    যদি (un > VARNUM_MAX)
	    {
		*nptr = VARNUM_MIN;
		যদি (overflow != রিক্ত)
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
		যদি (overflow != রিক্ত)
		    *overflow = TRUE;
	    }
	    *nptr = (varnumber_T)un;
	}
    }
    যদি (unptr != রিক্ত)
	*unptr = un;
}

/*
 * Return the value of a single hex character.
 * Only valid when the argument is '0' - '9', 'A' - 'F' or 'a' - 'f'.
 */
    পূর্ণাঙ্ক
hex2nr(পূর্ণাঙ্ক c)
{
    যদি (c >= 'a' && c <= 'f')
	ঘূৰাই c - 'a' + 10;
    যদি (c >= 'A' && c <= 'F')
	ঘূৰাই c - 'A' + 10;
    ঘূৰাই c - '0';
}

/*
 * Convert two hex characters to a byte.
 * Return -1 if one of the characters is not hex.
 */
    পূর্ণাঙ্ক
hexhex2nr(char_u *p)
{
    যদি (!vim_isxdigit(p[0]) || !vim_isxdigit(p[1]))
	ঘূৰাই -1;
    ঘূৰাই (hex2nr(p[0]) << 4) + hex2nr(p[1]);
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
    পূর্ণাঙ্ক
rem_backslash(char_u *str)
{
#ifdef BACKSLASH_IN_FILENAME
    ঘূৰাই (str[0] == '\\'
	    && str[1] < 0x80
	    && (str[1] == ' '
		|| (str[1] != NUL
		    && str[1] != '*'
		    && str[1] != '?'
		    && !vim_isfilec(str[1]))));
#অন্যথা
    ঘূৰাই (str[0] == '\\' && str[1] != NUL);
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
    বাবে ( ; *p; ++p)
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
    যদি (res == রিক্ত)
	ঘূৰাই p;
    backslash_halve(res);
    ঘূৰাই res;
}
