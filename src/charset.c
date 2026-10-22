/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#అయితే_ముగింపు

స్థిర పూర్ణాంకం parse_isopt(char_u *var, buf_T *buf, పూర్ణాంకం only_check);
స్థిర పూర్ణాంకం win_nolbr_chartabsize(chartabsize_T *cts, పూర్ణాంకం *headp);
స్థిర గుర్తులేని nr2hex(గుర్తులేని c);

స్థిర పూర్ణాంకం    chartab_initialized = FALSE;

// b_chartab[] is an array of 32 bytes, each bit representing one of the
// characters 0-255.
#నిర్వచించు సమితి_CHARTAB(buf, c) (buf)->b_chartab[(గుర్తులేని)(c) >> 3] |= (1 << ((c) & 0x7))
#నిర్వచించు RESET_CHARTAB(buf, c) (buf)->b_chartab[(గుర్తులేని)(c) >> 3] &= ~(1 << ((c) & 0x7))
#నిర్వచించు GET_CHARTAB(buf, c) ((buf)->b_chartab[(గుర్తులేని)(c) >> 3] & (1 << ((c) & 0x7)))

// table used below, see init_chartab() for an explanation
స్థిర char_u	g_chartab[256];

/*
 * Flags for g_chartab[].
 */
#నిర్వచించు CT_CELL_ముసుగు	0x07	// mask: nr of display cells (1, 2 or 4)
#నిర్వచించు CT_PRINT_CHAR	0x10	// flag: set for printable chars
#నిర్వచించు CT_ID_CHAR	0x20	// flag: set for ID chars
#నిర్వచించు CT_FNAME_CHAR	0x40	// flag: set for file name chars

స్థిర పూర్ణాంకం లోపల_win_border(win_T *wp, colnr_T vcol);

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
    పూర్ణాంకం
init_chartab(శూన్య)
{
    తిరిగి buf_init_chartab(curbuf, TRUE);
}

    పూర్ణాంకం
buf_init_chartab(
    buf_T	*buf,
    పూర్ణాంకం		global)		// FALSE: only set buf->b_chartab[]
{
    పూర్ణాంకం		c;
    char_u	*p;
    పూర్ణాంకం		i;

    ఒకవేళ (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 */
	c = 0;
	అయితే (c < ' ')
	    g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	అయితే (c <= '~')
	    g_chartab[c++] = 1 + CT_PRINT_CHAR;
	అయితే (c < 256)
	{
	    // UTF-8: bytes 0xa0 - 0xff are printable (latin1)
	    ఒకవేళ (enc_utf8 && c >= 0xa0)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // euc-jp characters starting with 0x8e are single width
	    లేకపోతే ఒకవేళ (enc_dbcs == DBCS_JPNU && c == 0x8e)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // other double-byte chars can be printable AND double-width
	    లేకపోతే ఒకవేళ (enc_dbcs != 0 && MB_BYTE2LEN(c) == 2)
		g_chartab[c++] = CT_PRINT_CHAR + 2;
	    లేకపోతే
		// the rest is unprintable by default
		g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	}

	// Assume that every multi-byte char is a filename character.
	కొరకు (c = 1; c < 256; ++c)
	    ఒకవేళ ((enc_dbcs != 0 && MB_BYTE2LEN(c) > 1)
		    || (enc_dbcs == DBCS_JPNU && c == 0x8e)
		    || (enc_utf8 && c >= 0xa0))
		g_chartab[c] |= CT_FNAME_CHAR;
    }

    /*
     * Init word char flags all to FALSE
     */
    CLEAR_క్షేత్రం(buf->b_chartab);
    ఒకవేళ (enc_dbcs != 0)
	కొరకు (c = 0; c < 256; ++c)
	{
	    // double-byte characters are probably word characters
	    ఒకవేళ (MB_BYTE2LEN(c) == 2)
		సమితి_CHARTAB(buf, c);
	}

    /*
     * In lisp mode the '-' character is included in keywords.
     */
    ఒకవేళ (buf->b_p_lisp)
	సమితి_CHARTAB(buf, '-');

    // Walk through the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    కొరకు (i = global ? 0 : 3; i <= 3; ++i)
    {
	ఒకవేళ (i == 0)
	    p = p_isi;		// first round: 'isident'
	లేకపోతే ఒకవేళ (i == 1)
	    p = p_isp;		// second round: 'isprint'
	లేకపోతే ఒకవేళ (i == 2)
	    p = p_isf;		// third round: 'isfname'
	లేకపోతే	// i == 3
	    p = buf->b_p_isk;	// fourth round: 'iskeyword'

	ఒకవేళ (parse_isopt(p, buf, FALSE) == FAIL)
	    తిరిగి FAIL;
    }

    chartab_initialized = TRUE;
    తిరిగి OK;
}

/**
 * Checks the format for the option settings 'iskeyword', 'isident', 'isfname'
 * or 'isprint'.
 * Returns FAIL if has an error, OK otherwise.
 */
	పూర్ణాంకం
check_isopt(char_u *var)
{
    తిరిగి parse_isopt(var, ఖాళీ, TRUE);
}

    స్థిర పూర్ణాంకం
parse_isopt(
	char_u	*var,
	buf_T	*buf,
	పూర్ణాంకం	only_check)	// FALSE: refill g_chartab[]
{
    char_u	*p = var;
    పూర్ణాంకం		c;
    పూర్ణాంకం		c2;
    పూర్ణాంకం		tilde;
    పూర్ణాంకం		do_isalpha;
    పూర్ణాంకం		trail_comma;

    // Parses the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    // Each option is a list of characters, character numbers or ranges,
    // separated by commas, e.g.: "200-210,x,#-178,-"
    అయితే (*p)
    {
	tilde = FALSE;
	do_isalpha = FALSE;
	ఒకవేళ (*p == '^' && p[1] != NUL)
	{
	    tilde = TRUE;
	    ++p;
	}
	ఒకవేళ (VIM_ISDIGIT(*p))
	    c = getdigits(&p);
	లేకపోతే ఒకవేళ (has_mbyte)
	    c = mb_ptr2char_adv(&p);
	లేకపోతే
	    c = *p++;
	c2 = -1;
	ఒకవేళ (*p == '-' && p[1] != NUL)
	{
	    ++p;
	    ఒకవేళ (VIM_ISDIGIT(*p))
		c2 = getdigits(&p);
	    లేకపోతే ఒకవేళ (has_mbyte)
		c2 = mb_ptr2char_adv(&p);
	    లేకపోతే
		c2 = *p++;
	}
	ఒకవేళ (c <= 0 || c >= 256 || (c2 < c && c2 != -1) || c2 >= 256
						  || !(*p == NUL || *p == ','))
	    తిరిగి FAIL;

	trail_comma = *p == ',';
	p = skip_గమ్యం_option_part(p);
	ఒకవేళ (trail_comma && *p == NUL)
	    // Trailing comma is not allowed.
	    తిరిగి FAIL;

	ఒకవేళ (only_check)
	    కొనసాగు;

	ఒకవేళ (c2 == -1)	// not a range
	{
	    /*
	     * A single '@' (not "@-@"):
	     * Decide on letters being ID/printable/keyword chars with
	     * standard function isalpha(). This takes care of locale for
	     * single-byte characters).
	     */
	    ఒకవేళ (c == '@')
	    {
		do_isalpha = TRUE;
		c = 1;
		c2 = 255;
	    }
	    లేకపోతే
		c2 = c;
	}

	అయితే (c <= c2)
	{
	    // Use the MB_ functions here, because isalpha() doesn't
	    // work properly when 'encoding' is "latin1" and the locale is
	    // "C".
	    ఒకవేళ (!do_isalpha || MB_ISLOWER(c) || MB_ISUPPER(c))
	    {
		ఒకవేళ (var == p_isi)			// (re)set ID flag
		{
		    ఒకవేళ (tilde)
			g_chartab[c] &= ~CT_ID_CHAR;
		    లేకపోతే
			g_chartab[c] |= CT_ID_CHAR;
		}
		లేకపోతే ఒకవేళ (var == p_isp)			// (re)set printable
		{
		    ఒకవేళ ((c < ' ' || c > '~'
			    // For double-byte we keep the cell width, so
			    // that we can detect it from the first byte.
			) && !(enc_dbcs && MB_BYTE2LEN(c) == 2))
		    {
			ఒకవేళ (tilde)
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_ముసుగు)
					 + ((dy_flags & DY_UHEX) ? 4 : 2);
			    g_chartab[c] &= ~CT_PRINT_CHAR;
			}
			లేకపోతే
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_ముసుగు) + 1;
			    g_chartab[c] |= CT_PRINT_CHAR;
			}
		    }
		}
		లేకపోతే ఒకవేళ (var == p_isf)			// (re)set fname flag
		{
		    ఒకవేళ (tilde)
			g_chartab[c] &= ~CT_FNAME_CHAR;
		    లేకపోతే
			g_chartab[c] |= CT_FNAME_CHAR;
		}
		లేకపోతే // var == p_isk || var == buf->b_p_isk
							// (re)set keyword flag
		{
		    ఒకవేళ (tilde)
			RESET_CHARTAB(buf, c);
		    లేకపోతే
			సమితి_CHARTAB(buf, c);
		}
	    }
	    ++c;
	}
    }

    తిరిగి OK;
}

/*
 * Translate any special characters in buf[bufsize] in-place.
 * The result is a string with only printable characters, but if there is not
 * enough room, not all characters will be translated.
 */
    శూన్య
trans_characters(
    char_u	*buf,
    పూర్ణాంకం		bufsize)
{
    పూర్ణాంకం		len;		// length of string needing translation
    పూర్ణాంకం		స్థలం;		// room in buffer after string
    char_u	*trs;		// translated character
    పూర్ణాంకం		trs_len;	// length of trs[]

    len = (పూర్ణాంకం)STRLEN(buf);
    స్థలం = bufsize - len;
    అయితే (*buf != 0)
    {
	// Assume a multi-byte character doesn't need translation.
	ఒకవేళ (has_mbyte && (trs_len = (*mb_ptr2len)(buf)) > 1)
	    len -= trs_len;
	లేకపోతే
	{
	    trs = transchar_byte(*buf);
	    trs_len = (పూర్ణాంకం)STRLEN(trs);
	    ఒకవేళ (trs_len > 1)
	    {
		స్థలం -= trs_len - 1;
		ఒకవేళ (స్థలం <= 0)
		    తిరిగి;
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
    పూర్ణాంకం		l, len, c;
    char_u	hexbuf[11];

    ఒకవేళ (has_mbyte)
    {
	// Compute the length of the result, taking account of unprintable
	// multi-byte characters.
	len = 0;
	p = s;
	అయితే (*p != NUL)
	{
	    ఒకవేళ ((l = (*mb_ptr2len)(p)) > 1)
	    {
		c = (*mb_ptr2char)(p);
		p += l;
		ఒకవేళ (vim_isprintc(c))
		    len += l;
		లేకపోతే
		{
		    transchar_hex(hexbuf, c);
		    len += (పూర్ణాంకం)STRLEN(hexbuf);
		}
	    }
	    లేకపోతే
	    {
		l = byte2cells(*p++);
		ఒకవేళ (l > 0)
		    len += l;
		లేకపోతే
		    len += 4;	// illegal byte sequence
	    }
	}
	res = alloc(len + 1);
    }
    లేకపోతే
	res = alloc(vim_strsize(s) + 1);

    ఒకవేళ (res == ఖాళీ)
	తిరిగి ఖాళీ;

    char_u *d = res;

    p = s;
    అయితే (*p != NUL)
    {
	ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    c = (*mb_ptr2char)(p);
	    ఒకవేళ (vim_isprintc(c))
	    {
		mch_memmove(d, p, (size_t)l);
		d += l;
	    }
	    లేకపోతే
	    {
		transchar_hex(d, c);
		d += STRLEN(d);
	    }
	    p += l;
	}
	లేకపోతే
	{
	    char_u	*trs = transchar_byte(*p++);
	    size_t	trs_len = STRLEN(trs);

	    mch_memmove(d, trs, trs_len);
	    d += trs_len;
	}
    }
    *d = NUL;
    తిరిగి res;
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
    పూర్ణాంకం		orglen,
    char_u	*buf,
    పూర్ణాంకం		buflen)
{
    garray_T	ga;
    పూర్ణాంకం		i;
    పూర్ణాంకం		len = orglen;

#నిర్వచించు GA_CHAR(i)  ((char_u *)ga.ga_దత్తాంశం)[i]
#నిర్వచించు GA_PTR(i)   ((char_u *)ga.ga_దత్తాంశం + (i))
#నిర్వచించు STR_CHAR(i)  (buf == ఖాళీ ? GA_CHAR(i) : buf[i])
#నిర్వచించు STR_PTR(i)   (buf == ఖాళీ ? GA_PTR(i) : buf + (i))

    // Copy "str" into "buf" or allocated memory, unmodified.
    ఒకవేళ (buf == ఖాళీ)
    {
	ga_init2(&ga, 1, 10);
	ఒకవేళ (ga_grow(&ga, len + 1) == FAIL)
	    తిరిగి ఖాళీ;
	mch_memmove(ga.ga_దత్తాంశం, str, (size_t)len);
	ga.ga_len = len;
    }
    లేకపోతే
    {
	ఒకవేళ (len >= buflen)	    // Ugly!
	    len = buflen - 1;
	mch_memmove(buf, str, (size_t)len);
    }
    ఒకవేళ (buf == ఖాళీ)
	GA_CHAR(len) = NUL;
    లేకపోతే
	buf[len] = NUL;

    // Make each character lower case.
    i = 0;
    అయితే (STR_CHAR(i) != NUL)
    {
	ఒకవేళ (enc_utf8 || (has_mbyte && MB_BYTE2LEN(STR_CHAR(i)) > 1))
	{
	    ఒకవేళ (enc_utf8)
	    {
		పూర్ణాంకం	c = utf_ptr2char(STR_PTR(i));
		పూర్ణాంకం	olen = utf_ptr2len(STR_PTR(i));
		పూర్ణాంకం	lc = utf_tolower(c);

		// Only replace the character when it is not an invalid
		// sequence (ASCII character or more than one byte) and
		// utf_tolower() doesn't return the original character.
		ఒకవేళ ((c < 0x80 || olen > 1) && c != lc)
		{
		    పూర్ణాంకం	    nlen = utf_char2len(lc);

		    // If the byte length changes need to shift the following
		    // characters forward or backward.
		    ఒకవేళ (olen != nlen)
		    {
			ఒకవేళ (nlen > olen)
			{
			    ఒకవేళ (buf == ఖాళీ
				    ? ga_grow(&ga, nlen - olen + 1) == FAIL
				    : len + nlen - olen >= buflen)
			    {
				// out of memory, keep old char
				lc = c;
				nlen = olen;
			    }
			}
			ఒకవేళ (olen != nlen)
			{
			    ఒకవేళ (buf == ఖాళీ)
			    {
				STRMOVE(GA_PTR(i) + nlen, GA_PTR(i) + olen);
				ga.ga_len += nlen - olen;
			    }
			    లేకపోతే
			    {
				STRMOVE(buf + i + nlen, buf + i + olen);
				len += nlen - olen;
			    }
			}
		    }
		    (శూన్య)utf_char2bytes(lc, STR_PTR(i));
		}
	    }
	    // skip to next multi-byte char
	    i += (*mb_ptr2len)(STR_PTR(i));
	}
	లేకపోతే
	{
	    ఒకవేళ (buf == ఖాళీ)
		GA_CHAR(i) = TOLOWER_LOC(GA_CHAR(i));
	    లేకపోతే
		buf[i] = TOLOWER_LOC(buf[i]);
	    ++i;
	}
    }

    ఒకవేళ (buf == ఖాళీ)
	తిరిగి (char_u *)ga.ga_దత్తాంశం;
    తిరిగి buf;
}

/*
 * Catch 22: g_chartab[] can't be initialized before the options are
 * initialized, and initializing options may cause transchar() to be called!
 * When chartab_initialized == FALSE don't use g_chartab[].
 * Does NOT work for multi-byte characters, c must be <= 255.
 * Also doesn't work for the first byte of a multi-byte, "c" must be a
 * character!
 */
స్థిర char_u	transchar_charbuf[7];

    char_u *
transchar(పూర్ణాంకం c)
{
    తిరిగి transchar_buf(curbuf, c);
}

    char_u *
transchar_buf(buf_T *buf, పూర్ణాంకం c)
{
    పూర్ణాంకం			i;

    i = 0;
    ఒకవేళ (IS_SPECIAL(c))	    // special key code, display as ~@ char
    {
	transchar_charbuf[0] = '~';
	transchar_charbuf[1] = '@';
	i = 2;
	c = K_రెండవ(c);
    }

    ఒకవేళ ((!chartab_initialized && ((c >= ' ' && c <= '~')))
					|| (c < 256 && vim_isprintc_strict(c)))
    {
	// printable character
	transchar_charbuf[i] = c;
	transchar_charbuf[i + 1] = NUL;
    }
    లేకపోతే
	transchar_nonprint(buf, transchar_charbuf + i, c);
    తిరిగి transchar_charbuf;
}

/*
 * Like transchar(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of the current buffer.
 */
    char_u *
transchar_byte(పూర్ణాంకం c)
{
    తిరిగి transchar_byte_buf(curbuf, c);
}

/*
 * Like transchar_buf(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of "buf", unless it is NULL.
 */
    char_u *
transchar_byte_buf(buf_T *buf, పూర్ణాంకం c)
{
    ఒకవేళ (enc_utf8 && c >= 0x80)
    {
	transchar_nonprint(buf, transchar_charbuf, c);
	తిరిగి transchar_charbuf;
    }
    తిరిగి transchar_buf(buf, c);
}
/*
 * Convert non-printable character to two or more printable characters in
 * "charbuf[]".  "charbuf" needs to be able to hold five bytes.
 * Does NOT work for multi-byte characters, c must be <= 255.
 */
    శూన్య
transchar_nonprint(buf_T *buf, char_u *charbuf, పూర్ణాంకం c)
{
    ఒకవేళ (c == NL)
	c = NUL;		// we use newline in place of a NUL
    లేకపోతే ఒకవేళ (buf != ఖాళీ && c == CAR && get_fileformat(buf) == EOL_MAC)
	c = NL;			// we use CR in place of  NL in this case

    ఒకవేళ (dy_flags & DY_UHEX)		// 'display' has "uhex"
	transchar_hex(charbuf, c);

    లేకపోతే ఒకవేళ (c <= 0x7f)			// 0x00 - 0x1f and 0x7f
    {
	charbuf[0] = '^';
	charbuf[1] = c ^ 0x40;		// DEL displayed as ^?
	charbuf[2] = NUL;
    }
    లేకపోతే ఒకవేళ (enc_utf8)
    {
	transchar_hex(charbuf, c);
    }
    లేకపోతే ఒకవేళ (c >= ' ' + 0x80 && c <= '~' + 0x80)    // 0xa0 - 0xfe
    {
	charbuf[0] = '|';
	charbuf[1] = c - 0x80;
	charbuf[2] = NUL;
    }
    లేకపోతే					    // 0x80 - 0x9f and 0xff
    {
	charbuf[0] = '~';
	charbuf[1] = (c - 0x80) ^ 0x40;	// 0xff displayed as ~?
	charbuf[2] = NUL;
    }
}

    శూన్య
transchar_hex(char_u *buf, పూర్ణాంకం c)
{
    పూర్ణాంకం		i = 0;

    buf[0] = '<';
    ఒకవేళ (c > 255)
    {
	buf[++i] = nr2hex((గుర్తులేని)c >> 12);
	buf[++i] = nr2hex((గుర్తులేని)c >> 8);
    }
    buf[++i] = nr2hex((గుర్తులేని)c >> 4);
    buf[++i] = nr2hex((గుర్తులేని)c);
    buf[++i] = '>';
    buf[++i] = NUL;
}

/*
 * Convert the lower 4 bits of byte "c" to its hex character.
 * Lower case letters are used to avoid the confusion of <F1> being 0xf1 or
 * function key 1.
 */
    స్థిర గుర్తులేని
nr2hex(గుర్తులేని c)
{
    ఒకవేళ ((c & 0xf) <= 9)
	తిరిగి (c & 0xf) + '0';
    తిరిగి (c & 0xf) - 10 + 'a';
}

/*
 * Return number of display cells occupied by byte "b".
 * Caller must make sure 0 <= b <= 255.
 * For multi-byte mode "b" must be the first byte of a character.
 * A TAB is counted as two cells: "^I".
 * For UTF-8 mode this will return 0 for bytes >= 0x80, because the number of
 * cells depends on further bytes.
 */
    పూర్ణాంకం
byte2cells(పూర్ణాంకం b)
{
    ఒకవేళ (enc_utf8 && b >= 0x80)
	తిరిగి 0;
    తిరిగి (g_chartab[b] & CT_CELL_ముసుగు);
}

/*
 * Return number of display cells occupied by character "c".
 * "c" can be a special key (negative number) in which case 3 or 4 is returned.
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    పూర్ణాంకం
char2cells(పూర్ణాంకం c)
{
    ఒకవేళ (IS_SPECIAL(c))
	తిరిగి char2cells(K_రెండవ(c)) + 2;
    ఒకవేళ (c >= 0x80)
    {
	// UTF-8: above 0x80 need to check the value
	ఒకవేళ (enc_utf8)
	    తిరిగి utf_char2cells(c);
	// DBCS: double-byte means double-width, except for euc-jp with first
	// byte 0x8e
	ఒకవేళ (enc_dbcs != 0 && c >= 0x100)
	{
	    ఒకవేళ (enc_dbcs == DBCS_JPNU && ((గుర్తులేని)c >> 8) == 0x8e)
		తిరిగి 1;
	    తిరిగి 2;
	}
    }
    తిరిగి (g_chartab[c & 0xff] & CT_CELL_ముసుగు);
}

/*
 * Return number of display cells occupied by character at "*p".
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    పూర్ణాంకం
ptr2cells(char_u *p)
{
    ఒకవేళ (!has_mbyte)
	తిరిగి byte2cells(*p);

    // For UTF-8 we need to look at more bytes if the first byte is >= 0x80.
    ఒకవేళ (enc_utf8 && *p >= 0x80)
	తిరిగి utf_ptr2cells(p);
    // For DBCS we can tell the cell count from the first byte.
    తిరిగి (g_chartab[*p] & CT_CELL_ముసుగు);
}

/*
 * Return the number of character cells string "s" will take on the screen,
 * counting TABs as two characters: "^I".
 */
    పూర్ణాంకం
vim_strsize(char_u *s)
{
    తిరిగి vim_strnsize(s, (పూర్ణాంకం)MAXCOL);
}

/*
 * Return the number of character cells string "s[len]" will take on the
 * screen, counting TABs as two characters: "^I".
 */
    పూర్ణాంకం
vim_strnsize(char_u *s, పూర్ణాంకం len)
{
    పూర్ణాంకం		కొలత = 0;

    అయితే (*s != NUL && --len >= 0)
    {
	పూర్ణాంకం	    l = (*mb_ptr2len)(s);

	కొలత += ptr2cells(s);
	s += l;
	len -= l - 1;
    }

    తిరిగి కొలత;
}

/*
 * Return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab.
 * Use a define to make it fast, this is used very often!!!
 * Also see getvcol() below.
 */

#నిర్వచితమైతే FEAT_VARTABS
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, నిలువు) \
    ఒకవేళ (*(p) == TAB && (!(wp)->w_p_list || (wp)->w_lcs_chars.tab1)) \
    { \
	తిరిగి tabstop_padding(నిలువు, (buf)->b_p_ts, (buf)->b_p_vts_array); \
    } \
    లేకపోతే \
	తిరిగి ptr2cells(p);
#లేకపోతే
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, నిలువు) \
    ఒకవేళ (*(p) == TAB && (!(wp)->w_p_list || wp->w_lcs_chars.tab1)) \
    { \
	పూర్ణాంకం ts; \
	ts = (buf)->b_p_ts; \
	తిరిగి (పూర్ణాంకం)(ts - (నిలువు % ts)); \
    } \
    లేకపోతే \
	తిరిగి ptr2cells(p);
#అయితే_ముగింపు

    పూర్ణాంకం
chartabsize(char_u *p, colnr_T నిలువు)
{
    RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, p, నిలువు)
}

#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    పూర్ణాంకం
win_chartabsize(win_T *wp, char_u *p, colnr_T నిలువు)
{
    RET_WIN_BUF_CHARTABSIZE(wp, wp->w_బఫరు, p, నిలువు)
}
#అయితే_ముగింపు

/*
 * Return the number of characters the string "s" will take on the screen,
 * taking into account the size of a tab.
 * Does not handle text properties, since "s" is not a buffer line.
 */
    పూర్ణాంకం
linetabsize_str(char_u *s)
{
    తిరిగి linetabsize_నిలువు(0, s);
}

/*
 * Like linetabsize_str(), but "s" starts at column "startcol".
 */
    పూర్ణాంకం
linetabsize_నిలువు(పూర్ణాంకం startcol, char_u *s)
{
    chartabsize_T cts;
    vimlong_T vcol;

    init_chartabsize_arg(&cts, curwin, 0, startcol, s, s);
    vcol = cts.cts_vcol;

    అయితే (*cts.cts_ptr != NUL)
    {
	vcol += lbr_chartabsize_adv(&cts);
	ఒకవేళ (vcol > MAXCOL)
	{
	    cts.cts_vcol = MAXCOL;
	    ఆపు;
	}
	లేకపోతే
	    cts.cts_vcol = (పూర్ణాంకం)vcol;
    }
    clear_chartabsize_arg(&cts);
    తిరిగి (పూర్ణాంకం)cts.cts_vcol;
}

/*
 * Like linetabsize_str(), but for a given window instead of the current one.
 * Doesn't count the size of 'listchars' "eol".
 */
    పూర్ణాంకం
win_linetabsize(win_T *wp, linenr_T lnum, char_u *పంక్తి, colnr_T len)
{
    chartabsize_T cts;

    init_chartabsize_arg(&cts, wp, lnum, 0, పంక్తి, పంక్తి);
    win_linetabsize_cts(&cts, len);
    clear_chartabsize_arg(&cts);
    తిరిగి (పూర్ణాంకం)cts.cts_vcol;
}

/*
 * Return the number of cells line "lnum" of window "wp" will take on the
 * screen, taking into account the size of a tab and text properties.
 * Doesn't count the size of 'listchars' "eol".
 */
    పూర్ణాంకం
linetabsize(win_T *wp, linenr_T lnum)
{
    తిరిగి win_linetabsize(wp, lnum,
		       ml_get_buf(wp->w_బఫరు, lnum, FALSE), (colnr_T)MAXCOL);
}

/*
 * Like linetabsize(), but counts the size of 'listchars' "eol".
 */
    పూర్ణాంకం
linetabsize_eol(win_T *wp, linenr_T lnum)
{
    తిరిగి linetabsize(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Like linetabsize(), but excludes 'above'/'after'/'right'/'below' aligned
 * virtual text, while keeping inline virtual text.
 */
    పూర్ణాంకం
linetabsize_no_outer(win_T *wp, linenr_T lnum)
{
#నిర్వచితం_కాకపోతే FEAT_PROP_POPUP
    తిరిగి linetabsize(wp, lnum);
#లేకపోతే
    chartabsize_T cts;
    char_u *పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, పంక్తి, పంక్తి);

    ఒకవేళ (cts.cts_పాఠ్యం_prop_లెక్క)
    {
	పూర్ణాంకం write_idx = 0;
	కొరకు (పూర్ణాంకం read_idx = 0; read_idx < cts.cts_పాఠ్యం_prop_లెక్క; read_idx++)
	{
	    textprop_T *tp = &cts.cts_పాఠ్యం_props[read_idx];
	    ఒకవేళ (tp->tp_నిలువు != MAXCOL)
	    {
		ఒకవేళ (read_idx != write_idx)
		    cts.cts_పాఠ్యం_props[write_idx] = *tp;
		write_idx++;
	    }
	}
	cts.cts_పాఠ్యం_prop_లెక్క = write_idx;
	ఒకవేళ (cts.cts_పాఠ్యం_prop_లెక్క == 0)
	    VIM_CLEAR(cts.cts_పాఠ్యం_props);
    }

    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    తిరిగి (పూర్ణాంకం)cts.cts_vcol;
#అయితే_ముగింపు
}

/*
 * Like linetabsize_no_outer(), but counts the size of 'listchars' "eol".
 */
    పూర్ణాంకం
linetabsize_no_outer_eol(win_T *wp, linenr_T lnum)
{
    తిరిగి linetabsize_no_outer(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Return TRUE when win_lbr_chartabsize() does nothing more than
 * win_nolbr_chartabsize(): no 'linebreak', 'breakindent', 'showbreak' and no
 * text properties that insert text.
 */
#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    స్థిర పూర్ణాంకం
win_lbr_is_simple_వెడల్పు(chartabsize_T *cts)
{
    తిరిగి TRUE
# ifdef FEAT_LINEBREAK
	&& !cts->cts_win->w_p_lbr && !cts->cts_win->w_p_bri
	&& *get_showbreak_విలువ(cts->cts_win) == NUL
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_పాఠ్యం
# endif
	;
}
#అయితే_ముగింపు

    శూన్య
win_linetabsize_cts(chartabsize_T *cts, colnr_T len)
{
    vimlong_T vcol = cts->cts_vcol;
#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    // Fast path: when win_lbr_chartabsize() would just return
    // win_nolbr_chartabsize() (wrap on; no 'linebreak'/'breakindent'/
    // 'showbreak'/inserted text properties), call it directly.
    పూర్ణాంకం simple_వెడల్పు = cts->cts_win->w_p_wrap && win_lbr_is_simple_వెడల్పు(cts);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    cts->cts_with_trailing = len == MAXCOL;
#అయితే_ముగింపు
    కొరకు ( ; *cts->cts_ptr != NUL && (len == MAXCOL || cts->cts_ptr < cts->cts_పంక్తి + len);
						      MB_PTR_ADV(cts->cts_ptr))
    {
#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
	ఒకవేళ (simple_వెడల్పు)
	{
	    పూర్ణాంకం	c = *cts->cts_ptr;

	    ఒకవేళ (c < 0x80 && c != TAB)
		vcol += g_chartab[c] & CT_CELL_ముసుగు;
	    లేకపోతే
		vcol += win_nolbr_chartabsize(cts, ఖాళీ);
	}
	లేకపోతే
#అయితే_ముగింపు
	    vcol += win_lbr_chartabsize(cts, ఖాళీ, ఖాళీ);
	ఒకవేళ (vcol > MAXCOL)
	{
	    cts->cts_vcol = MAXCOL;
	    ఆపు;
	}
	లేకపోతే
	    cts->cts_vcol = (పూర్ణాంకం)vcol;
    }
#నిర్వచితమైతే FEAT_PROP_POPUP
    // check for a virtual text at the end of a line or on an empty line
    ఒకవేళ (len == MAXCOL && cts->cts_has_prop_with_పాఠ్యం && *cts->cts_ptr == NUL)
    {
	పూర్ణాంకం తల = 0;
	(శూన్య)win_lbr_chartabsize(cts, &తల, ఖాళీ);
	vcol += cts->cts_cur_పాఠ్యం_వెడల్పు + తల;
	// When properties are above the empty line must also be counted.  For
	// a property below the width already includes filling up the line.
	ఒకవేళ (cts->cts_ptr == cts->cts_పంక్తి && cts->cts_prop_పంక్తులు > 0
							 && !cts->cts_has_క్రింద)
	    ++vcol;
	cts->cts_vcol = vcol > MAXCOL ? MAXCOL : (పూర్ణాంకం)vcol;
    }
#అయితే_ముగింపు
}

/*
 * Return TRUE if 'c' is a normal identifier character:
 * Letters and characters from the 'isident' option.
 */
    పూర్ణాంకం
vim_isIDc(పూర్ణాంకం c)
{
    తిరిగి (c > 0 && c < 0x100 && (g_chartab[c] & CT_ID_CHAR));
}

/*
 * Like vim_isIDc() but not using the 'isident' option: letters, numbers and
 * underscore.
 */
    పూర్ణాంకం
vim_is_సాధారణ_IDc(పూర్ణాంకం c)
{
    తిరిగి ASCII_ISALNUM(c) || c == '_';
}

/*
 * return TRUE if 'c' is a keyword character: Letters and characters from
 * 'iskeyword' option for the current buffer.
 * For multi-byte characters mb_get_class() is used (builtin rules).
 */
    పూర్ణాంకం
vim_iswordc(పూర్ణాంకం c)
{
    తిరిగి vim_iswordc_buf(c, curbuf);
}

    పూర్ణాంకం
vim_iswordc_buf(పూర్ణాంకం c, buf_T *buf)
{
    ఒకవేళ (c >= 0x100)
    {
	ఒకవేళ (enc_dbcs != 0)
	    తిరిగి dbcs_class((గుర్తులేని)c >> 8, (గుర్తులేని)(c & 0xff)) >= 2;
	ఒకవేళ (enc_utf8)
	    తిరిగి utf_class_buf(c, buf) >= 2;
	తిరిగి FALSE;
    }
    తిరిగి (c > 0 && GET_CHARTAB(buf, c) != 0);
}

/*
 * Just like vim_iswordc() but uses a pointer to the (multi-byte) character.
 */
    పూర్ణాంకం
vim_iswordp(char_u *p)
{
    తిరిగి vim_iswordp_buf(p, curbuf);
}

    పూర్ణాంకం
vim_iswordp_buf(char_u *p, buf_T *buf)
{
    పూర్ణాంకం	c = *p;

    ఒకవేళ (has_mbyte && MB_BYTE2LEN(c) > 1)
	c = (*mb_ptr2char)(p);
    తిరిగి vim_iswordc_buf(c, buf);
}

/*
 * Return TRUE if 'c' is a valid file-name character as specified with the
 * 'isfname' option.
 * Assume characters above 0x100 are valid (multi-byte).
 * To be used for commands like "gf".
 */
    పూర్ణాంకం
vim_isfilec(పూర్ణాంకం c)
{
    తిరిగి (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_FNAME_CHAR)));
}

#ఒకవేళ defined(FEAT_SPELL)
/*
 * Return TRUE if 'c' is a valid file-name character, including characters left
 * out of 'isfname' to make "gf" work, such as comma, space, '@', etc.
 */
    పూర్ణాంకం
vim_is_fname_char(పూర్ణాంకం c)
{
    తిరిగి vim_isfilec(c) || c == ',' || c == ' ' || c == '@';
}
#అయితే_ముగింపు

/*
 * return TRUE if 'c' is a valid file-name character or a wildcard character
 * Assume characters above 0x100 are valid (multi-byte).
 * Explicitly interpret ']' as a wildcard character as mch_has_wildcard("]")
 * returns false.
 */
    పూర్ణాంకం
vim_isfilec_or_wc(పూర్ణాంకం c)
{
    char_u buf[2];

    buf[0] = (char_u)c;
    buf[1] = NUL;
    తిరిగి vim_isfilec(c) || c == ']' || mch_has_wildcard(buf);
}

/*
 * Return TRUE if 'c' is a printable character.
 * Assume characters above 0x100 are printable (multi-byte), except for
 * Unicode.
 */
    పూర్ణాంకం
vim_isprintc(పూర్ణాంకం c)
{
    ఒకవేళ (enc_utf8 && c >= 0x100)
	తిరిగి utf_printable(c);
    తిరిగి (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Strict version of vim_isprintc(c), don't return TRUE if "c" is the head
 * byte of a double-byte character.
 */
    పూర్ణాంకం
vim_isprintc_strict(పూర్ణాంకం c)
{
    ఒకవేళ (enc_dbcs != 0 && c < 0x100 && MB_BYTE2LEN(c) > 1)
	తిరిగి FALSE;
    ఒకవేళ (enc_utf8 && c >= 0x100)
	తిరిగి utf_printable(c);
    తిరిగి (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Prepare the structure passed to chartabsize functions.
 * "line" is the start of the line, "ptr" is the first relevant character.
 * When "lnum" is zero do not use text properties that insert text.
 */
    శూన్య
init_chartabsize_arg(
	chartabsize_T	*cts,
	win_T		*wp,
	linenr_T	lnum వాడని,
	colnr_T		నిలువు,
	char_u		*పంక్తి,
	char_u		*ptr)
{
    CLEAR_సూచిక(cts);
    cts->cts_win = wp;
    cts->cts_vcol = నిలువు;
    cts->cts_పంక్తి = పంక్తి;
    cts->cts_ptr = ptr;
#నిర్వచితమైతే FEAT_LINEBREAK
    cts->cts_bri_కొలత = -1;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (lnum > 0 && !ignore_పాఠ్యం_props)
    {
	char_u	*prop_ప్రారంభం;
	పూర్ణాంకం	లెక్క;

	లెక్క = get_పాఠ్యం_props(wp->w_బఫరు, lnum, &prop_ప్రారంభం, FALSE);
	cts->cts_పాఠ్యం_prop_లెక్క = లెక్క;
	ఒకవేళ (లెక్క > 0)
	{
	    // Make a copy of the properties, so that they are properly
	    // aligned.  Make it twice as long for the sorting below.
	    cts->cts_పాఠ్యం_props = ALLOC_MULT(textprop_T, లెక్క * 2);
	    ఒకవేళ (cts->cts_పాఠ్యం_props == ఖాళీ)
		cts->cts_పాఠ్యం_prop_లెక్క = 0;
	    లేకపోతే
	    {
		పూర్ణాంకం	i;

		mch_memmove(cts->cts_పాఠ్యం_props + లెక్క, prop_ప్రారంభం,
						   లెక్క * పరిమాణం(textprop_T));
		కొరకు (i = 0; i < లెక్క; ++i)
		{
		    textprop_T *tp = cts->cts_పాఠ్యం_props + i + లెక్క;
		    ఒకవేళ (tp->tp_id < 0
				     && పాఠ్యం_prop_type_valid(wp->w_బఫరు, tp))
		    {
			cts->cts_has_prop_with_పాఠ్యం = TRUE;
			ఆపు;
		    }
		}
		ఒకవేళ (!cts->cts_has_prop_with_పాఠ్యం)
		{
		    // won't use the text properties, free them
		    VIM_CLEAR(cts->cts_పాఠ్యం_props);
		    cts->cts_పాఠ్యం_prop_లెక్క = 0;
		}
		లేకపోతే
		{
		    పూర్ణాంకం	    *పాఠ్యం_prop_idxs;

		    // Need to sort the array to get any truncation right.
		    // Do the sorting in the second part of the array, then
		    // move the sorted props to the first part of the array.
		    పాఠ్యం_prop_idxs = ALLOC_MULT(పూర్ణాంకం, లెక్క);
		    ఒకవేళ (పాఠ్యం_prop_idxs != ఖాళీ)
		    {
			కొరకు (i = 0; i < లెక్క; ++i)
			    పాఠ్యం_prop_idxs[i] = i + లెక్క;
			క్రమీకరించు_పాఠ్యం_props(curbuf, cts->cts_పాఠ్యం_props,
							పాఠ్యం_prop_idxs, లెక్క);
			// Here we want the reverse order.
			కొరకు (i = 0; i < లెక్క; ++i)
			    cts->cts_పాఠ్యం_props[లెక్క - i - 1] =
					cts->cts_పాఠ్యం_props[పాఠ్యం_prop_idxs[i]];
			vim_free(పాఠ్యం_prop_idxs);
		    }

		    // Convert tp_text_offset to tp_text pointer.
		    char_u *లెక్క_ptr = prop_ప్రారంభం - PROP_లెక్క_కొలత;

		    కొరకు (i = 0; i < లెక్క; ++i)
		    {
			textprop_T *tp = &cts->cts_పాఠ్యం_props[i];

			ఒకవేళ (tp->tp_id < 0 && tp->u.tp_పాఠ్యం_స్థానభ్రంశం > 0)
			{
			    tp->u.tp_పాఠ్యం = లెక్క_ptr + tp->u.tp_పాఠ్యం_స్థానభ్రంశం;
			    tp->tp_flags |= TP_FLAG_VTEXT_PTR;
			}
			లేకపోతే
			    tp->u.tp_పాఠ్యం = ఖాళీ;
		    }
		}
	    }
	}
    }
#అయితే_ముగింపు
}

/*
 * Free any allocated item in "cts".
 */
    శూన్య
clear_chartabsize_arg(chartabsize_T *cts వాడని)
{
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (cts->cts_పాఠ్యం_prop_లెక్క > 0)
    {
	VIM_CLEAR(cts->cts_పాఠ్యం_props);
	cts->cts_పాఠ్యం_prop_లెక్క = 0;
    }
#అయితే_ముగింపు
}

/*
 * Like chartabsize(), but also check for line breaks on the screen and text
 * properties that insert text.
 */
    పూర్ణాంకం
lbr_chartabsize(chartabsize_T *cts)
{
#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ఒకవేళ (1
# ifdef FEAT_LINEBREAK
	&& !curwin->w_p_lbr && *get_showbreak_విలువ(curwin) == NUL
							   && !curwin->w_p_bri
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_పాఠ్యం
# endif
       )
    {
#అయితే_ముగింపు
	ఒకవేళ (curwin->w_p_wrap)
	    తిరిగి win_nolbr_chartabsize(cts, ఖాళీ);
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, cts->cts_ptr, cts->cts_vcol)
#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    }
    తిరిగి win_lbr_chartabsize(cts, ఖాళీ, ఖాళీ);
#అయితే_ముగింపు
}

/*
 * Call lbr_chartabsize() and advance the pointer.
 */
    పూర్ణాంకం
lbr_chartabsize_adv(chartabsize_T *cts)
{
    పూర్ణాంకం		retval;

    retval = lbr_chartabsize(cts);
    MB_PTR_ADV(cts->cts_ptr);
    తిరిగి retval;
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
    పూర్ణాంకం
win_lbr_chartabsize(
	chartabsize_T	*cts,
	పూర్ణాంకం		*headp వాడని,
	పూర్ణాంకం		*tailp వాడని)
{
    win_T	*wp = cts->cts_win;
#ఒకవేళ defined(FEAT_PROP_POPUP) || defined(FEAT_LINEBREAK)
    char_u	*పంక్తి = cts->cts_పంక్తి; // start of the line
    పూర్ణాంకం		కొలత;
#అయితే_ముగింపు
    char_u	*s = cts->cts_ptr;
    colnr_T	vcol = cts->cts_vcol;
#నిర్వచితమైతే FEAT_LINEBREAK
    పూర్ణాంకం		mb_added = 0;
    పూర్ణాంకం		n;
    char_u	*sbr;
    పూర్ణాంకం		no_sbr = FALSE;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PROP_POPUP)
    cts->cts_cur_పాఠ్యం_వెడల్పు = 0;
    cts->cts_has_క్రింద = false;
    cts->cts_మొదటి_char = 0;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    /*
     * No 'linebreak', 'showbreak', 'breakindent' and text properties that
     * insert text: return quickly.
     */
    ఒకవేళ (win_lbr_is_simple_వెడల్పు(cts))
#అయితే_ముగింపు
    {
	ఒకవేళ (wp->w_p_wrap)
	    తిరిగి win_nolbr_chartabsize(cts, headp);
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_బఫరు, s, vcol)
    }

#ఒకవేళ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    పూర్ణాంకం has_lcs_eol = wp->w_p_list && wp->w_lcs_chars.eol != NUL;
    // Virtual text above the line is on its own screen line, it does not count
    // for the size of a Tab.
    colnr_T tab_vcol = vcol;

# ifdef FEAT_PROP_POPUP
    tab_vcol -= cts->cts_above_వెడల్పు;
# endif

    /*
     * First get the normal size, without 'linebreak' or text properties
     */
    కొలత = win_chartabsize(wp, s, tab_vcol);
# ifdef FEAT_LINEBREAK
    ఒకవేళ (*s == NUL)
    {
	// 1 cell for EOL list char (if present), as opposed to the two cell ^@
	// for a NUL character in the text.
	కొలత = has_lcs_eol ? 1 : 0;
    }

    పూర్ణాంకం is_doublewidth = has_mbyte && కొలత == 2 && MB_BYTE2LEN(*s) > 1;
# endif

# ifdef FEAT_PROP_POPUP
    ఒకవేళ (cts->cts_has_prop_with_పాఠ్యం)
    {
#  ifdef FEAT_LINEBREAK
	పూర్ణాంకం	    tab_కొలత = కొలత;
#  endif
	పూర్ణాంకం	    charlen = *s == NUL ? 1 : mb_ptr2len(s);
	పూర్ణాంకం	    i;
	పూర్ణాంకం	    నిలువు = (పూర్ణాంకం)(s - పంక్తి);


	// The "$" for 'list' mode will go between the EOL and
	// the text prop, account for that.
	ఒకవేళ (has_lcs_eol)
	{
	    ++vcol;
	    --కొలత;
	}

	కొరకు (i = 0; i < cts->cts_పాఠ్యం_prop_లెక్క; ++i)
	{
	    textprop_T	*tp = cts->cts_పాఠ్యం_props + i;
	    పూర్ణాంకం		నిలువు_off = win_నిలువు_off(wp);

	    // Watch out for the text being deleted.  "cts_text_props" is a
	    // copy, the text prop may actually have been removed from the line.
	    ఒకవేళ (tp->tp_id < 0
		    && ((tp->tp_నిలువు - 1 >= నిలువు
					     && tp->tp_నిలువు - 1 < నిలువు + charlen)
		       || (tp->tp_నిలువు == MAXCOL
			   && ((tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
				? నిలువు == 0
				: s[0] == NUL && cts->cts_with_trailing)))
		    && tp->u.tp_పాఠ్యం != ఖాళీ)
	    {
		char_u *p = tp->u.tp_పాఠ్యం;

		ఒకవేళ (p != ఖాళీ)
		{
		    పూర్ణాంకం	cells;

		    ఒకవేళ (tp->tp_నిలువు == MAXCOL)
		    {
			పూర్ణాంకం n_extra = (పూర్ణాంకం)STRLEN(p);

			cells = పాఠ్యం_prop_position(wp, tp, vcol,
			     (vcol + కొలత) % (wp->w_వెడల్పు - నిలువు_off) + నిలువు_off,
					      &n_extra, &p, ఖాళీ, ఖాళీ, FALSE);
#  ifdef FEAT_LINEBREAK
			ఒకవేళ (పాఠ్యం_prop_no_showbreak(tp))
			    no_sbr = TRUE;  // don't use 'showbreak' now
#  endif
		    }
		    లేకపోతే
			cells = vim_strsize(p);
		    ఒకవేళ (tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
		    {
			cts->cts_మొదటి_char += cells;
			ఒకవేళ (!cts->cts_no_above)
			    cts->cts_cur_పాఠ్యం_వెడల్పు += cells;
		    }
		    లేకపోతే
		    {
			cts->cts_cur_పాఠ్యం_వెడల్పు += cells;
			కొలత += cells;
		    }
		    cts->cts_ప్రారంభం_incl = tp->tp_flags & TP_FLAG_ప్రారంభం_INCL;
#  ifdef FEAT_LINEBREAK
		    ఒకవేళ (*s == TAB)
		    {
			// tab size changes because of the inserted text
			కొలత -= tab_కొలత;
			tab_కొలత = win_chartabsize(wp, s,
					vcol + కొలత - cts->cts_above_వెడల్పు);
			కొలత += tab_కొలత;
		    }
#  endif
		    ఒకవేళ (tp->tp_నిలువు == MAXCOL && (tp->tp_flags
				& (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_క్రింద)))
		    {
			// count extra line for property above/below
			++cts->cts_prop_పంక్తులు;
			ఒకవేళ (tp->tp_flags & TP_FLAG_ALIGN_క్రింద)
			    cts->cts_has_క్రింద = true;
		    }
		}
	    }
	    ఒకవేళ (tp->tp_నిలువు != MAXCOL && tp->tp_నిలువు - 1 > నిలువు)
		ఆపు;
	}
	ఒకవేళ (has_lcs_eol)
	{
	    --vcol;
	    ++కొలత;
	}
    }
# endif

# ifdef FEAT_LINEBREAK
    ఒకవేళ (is_doublewidth && wp->w_p_wrap && లోపల_win_border(wp, vcol + కొలత - 2))
    {
	++కొలత;		// Count the ">" in the last column.
	mb_added = 1;
    }

    /*
     * May have to add something for 'breakindent' and/or 'showbreak'
     * string at the start of a screen line.
     */
    పూర్ణాంకం తల = mb_added;
    sbr = no_sbr ? ఖాళీలు_option : get_showbreak_విలువ(wp);
    // When "size" is 0, no new screen line is started.
    ఒకవేళ (కొలత > 0 && wp->w_p_wrap && (*sbr != NUL || wp->w_p_bri))
    {
	పూర్ణాంకం	నిలువు_off_prev = win_నిలువు_off(wp);
	పూర్ణాంకం	width2 = wp->w_వెడల్పు - నిలువు_off_prev + win_నిలువు_off2(wp);
	colnr_T	wcol = vcol + నిలువు_off_prev;
#  ifdef FEAT_PROP_POPUP
	wcol -= wp->w_virtcol_మొదటి_char;
#  endif
	colnr_T	max_తల_vcol = cts->cts_max_తల_vcol;
	పూర్ణాంకం	added = 0;

	// cells taken by 'showbreak'/'breakindent' before current char
	పూర్ణాంకం	తల_prev = 0;
	ఒకవేళ (wcol >= wp->w_వెడల్పు)
	{
	    wcol -= wp->w_వెడల్పు;
	    నిలువు_off_prev = wp->w_వెడల్పు - width2;
	    ఒకవేళ (wcol >= width2 && width2 > 0)
		wcol %= width2;
	    ఒకవేళ (*sbr != NUL)
		తల_prev += vim_strsize(sbr);
	    ఒకవేళ (wp->w_p_bri)
	    {
		ఒకవేళ (cts->cts_bri_కొలత < 0)
		    cts->cts_bri_కొలత = get_breakindent_win(wp, పంక్తి);
		తల_prev += cts->cts_bri_కొలత;
	    }
	    ఒకవేళ (wcol < తల_prev)
	    {
		తల_prev -= wcol;
		wcol += తల_prev;
		added += తల_prev;
		ఒకవేళ (max_తల_vcol <= 0 || vcol < max_తల_vcol)
		    తల += తల_prev;
	    }
	    లేకపోతే
		తల_prev = 0;
	    wcol += నిలువు_off_prev;
	}

	ఒకవేళ (wcol + కొలత > wp->w_వెడల్పు)
	{
	    // cells taken by 'showbreak'/'breakindent' halfway current char
	    పూర్ణాంకం	తల_mid = 0;
	    ఒకవేళ (*sbr != NUL)
		తల_mid += vim_strsize(sbr);
	    ఒకవేళ (wp->w_p_bri)
	    {
		ఒకవేళ (cts->cts_bri_కొలత < 0)
		    cts->cts_bri_కొలత = get_breakindent_win(wp, పంక్తి);
		తల_mid += cts->cts_bri_కొలత;
	    }
	    ఒకవేళ (తల_mid > 0)
	    {
		// Calculate effective window width.
		పూర్ణాంకం prev_rem = wp->w_వెడల్పు - wcol;
		పూర్ణాంకం వెడల్పు = width2 - తల_mid;

		ఒకవేళ (వెడల్పు <= 0)
		    వెడల్పు = 1;
		// Divide "size - prev_rem" by "width", rounding up.
		పూర్ణాంకం cnt = (కొలత - prev_rem + వెడల్పు - 1) / వెడల్పు;
		added += cnt * తల_mid;

		ఒకవేళ (max_తల_vcol == 0 || vcol + కొలత + added < max_తల_vcol)
		    తల += cnt * తల_mid;
		లేకపోతే ఒకవేళ (width2 > 0
			   && max_తల_vcol > vcol + తల_prev + prev_rem)
		    తల += (max_తల_vcol - (vcol + తల_prev + prev_rem)
					     + width2 - 1) / width2 * తల_mid;
		లేకపోతే ఒకవేళ (max_తల_vcol < 0)
		{
		    పూర్ణాంకం off = mb_added;
#  ifdef FEAT_PROP_POPUP
		    ఒకవేళ (*s != NUL
			     && ((State & బహుళకం_సాధారణ) || cts->cts_ప్రారంభం_incl))
			off += cts->cts_cur_పాఠ్యం_వెడల్పు;
#  endif
		    ఒకవేళ (off >= prev_rem)
			తల += (1 + (off - prev_rem) / వెడల్పు) * తల_mid;
		}
	    }
	}

	కొలత += added;
    }

    ఒకవేళ (headp != ఖాళీ)
	*headp = తల;

    పూర్ణాంకం కొలత_before_lbr = కొలత;
    పూర్ణాంకం need_lbr = FALSE;
    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here.
     */
    ఒకవేళ (wp->w_p_lbr && wp->w_p_wrap && wp->w_వెడల్పు != 0
	    && VIM_ISBREAK((పూర్ణాంకం)s[0]) && !VIM_ISBREAK((పూర్ణాంకం)s[1]))
    {
	char_u	*t = cts->cts_పంక్తి;
	అయితే (VIM_ISBREAK((పూర్ణాంకం)t[0]))
	    t++;
	// 'linebreak' is only needed when not in leading whitespace.
	need_lbr = s >= t;
    }
    ఒకవేళ (need_lbr)
    {
	/*
	 * Count all characters from first non-blank after a blank up to next
	 * non-blank after a blank.
	 */
	పూర్ణాంకం numberextra = win_నిలువు_off(wp);
	colnr_T నిలువు_adj = కొలత - 1;
	colnr_T colmax = (colnr_T)(wp->w_వెడల్పు - numberextra - నిలువు_adj);
	ఒకవేళ (vcol >= colmax)
	{
	    colmax += నిలువు_adj;
	    n = colmax +  win_నిలువు_off2(wp);
	    ఒకవేళ (n > 0)
		colmax += (((vcol + నిలువు_adj - colmax) / n) + 1) * n - నిలువు_adj;
	}

	colnr_T vcol2 = vcol;
	కొరకు (;;)
	{
	    char_u *ps = s;
	    MB_PTR_ADV(s);
	    పూర్ణాంకం c = *s;
	    ఒకవేళ (!(c != NUL
		    && (VIM_ISBREAK(c)
			|| (!VIM_ISBREAK(c)
			       && (vcol2 == vcol || !VIM_ISBREAK((పూర్ణాంకం)*ps))))))
		ఆపు;

	    vcol2 += win_chartabsize(wp, s, vcol2);
	    ఒకవేళ (vcol2 >= colmax)		// doesn't fit
	    {
		కొలత = colmax - vcol + నిలువు_adj;
		ఆపు;
	    }
	}
    }

    ఒకవేళ (tailp != ఖాళీ)
	*tailp = కొలత - కొలత_before_lbr;

#  ifdef FEAT_PROP_POPUP
    ఒకవేళ (cts->cts_మొదటి_char > 0 && !cts->cts_no_above)
    {
	// Remember the width for the size of a Tab later in the line.  Use
	// assignment, this may be called more than once for a character.
	cts->cts_above_వెడల్పు = cts->cts_మొదటి_char;
	కొలత += cts->cts_మొదటి_char;
    }
#  endif
# endif
    తిరిగి కొలత;
#అయితే_ముగింపు
}

/*
 * Like win_lbr_chartabsize(), except that we know 'linebreak' is off, 'wrap'
 * is on and there are no properties that insert text.  This means we need to
 * check for a double-byte character that doesn't fit at the end of the screen
 * line.
 * Only uses "cts_win", "cts_ptr" and "cts_vcol" from "cts".
 */
    స్థిర పూర్ణాంకం
win_nolbr_chartabsize(
	chartabsize_T	*cts,
	పూర్ణాంకం		*headp)
{
    win_T	*wp = cts->cts_win;
    char_u	*s = cts->cts_ptr;
    colnr_T	నిలువు = cts->cts_vcol;
    పూర్ణాంకం		n;

    ఒకవేళ (*s == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
    {
#నిర్వచితమైతే FEAT_VARTABS
	తిరిగి tabstop_padding(నిలువు, wp->w_బఫరు->b_p_ts,
				    wp->w_బఫరు->b_p_vts_array);
#లేకపోతే
	n = wp->w_బఫరు->b_p_ts;
	తిరిగి (పూర్ణాంకం)(n - (నిలువు % n));
#అయితే_ముగింపు
    }
    n = ptr2cells(s);
    // Add one cell for a double-width character in the last column of the
    // window, displayed with a ">".
    ఒకవేళ (n == 2 && MB_BYTE2LEN(*s) > 1 && లోపల_win_border(wp, నిలువు))
    {
	ఒకవేళ (headp != ఖాళీ)
	    *headp = 1;
	తిరిగి 3;
    }
    తిరిగి n;
}

/*
 * Return TRUE if virtual column "vcol" is in the rightmost column of window
 * "wp".
 */
    స్థిర పూర్ణాంకం
లోపల_win_border(win_T *wp, colnr_T vcol)
{
    పూర్ణాంకం		width1;		// width of first line (after line number)
    పూర్ణాంకం		width2;		// width of further lines

    ఒకవేళ (wp->w_వెడల్పు == 0)	// there is no border
	తిరిగి FALSE;
    width1 = wp->w_వెడల్పు - win_నిలువు_off(wp);
    ఒకవేళ ((పూర్ణాంకం)vcol < width1 - 1)
	తిరిగి FALSE;
    ఒకవేళ ((పూర్ణాంకం)vcol == width1 - 1)
	తిరిగి TRUE;
    width2 = width1 + win_నిలువు_off2(wp);
    ఒకవేళ (width2 <= 0)
	తిరిగి FALSE;
    తిరిగి ((vcol - width1) % width2 == width2 - 1);
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
    శూన్య
getvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*ప్రారంభం,
    colnr_T	*cursor,
    colnr_T	*చివర,
    పూర్ణాంకం		flags)
{
    colnr_T	vcol;
    char_u	*ptr;		// points to current char
    char_u	*పంక్తి;		// start of the line
    పూర్ణాంకం		incr;
    పూర్ణాంకం		తల;
    పూర్ణాంకం		తోక;
#నిర్వచితమైతే FEAT_VARTABS
    పూర్ణాంకం		*vts = wp->w_బఫరు->b_p_vts_array;
#అయితే_ముగింపు
    పూర్ణాంకం		ts = wp->w_బఫరు->b_p_ts;
    పూర్ణాంకం		c;
    chartabsize_T cts;
#నిర్వచితమైతే FEAT_PROP_POPUP
    పూర్ణాంకం		on_NUL = FALSE;
#అయితే_ముగింపు

    vcol = 0;
    పంక్తి = ptr = ml_get_buf(wp->w_బఫరు, pos->lnum, FALSE);

    init_chartabsize_arg(&cts, wp, pos->lnum, 0, పంక్తి, పంక్తి);
    cts.cts_max_తల_vcol = -1;
#నిర్వచితమైతే FEAT_PROP_POPUP
    cts.cts_no_above = true;
#అయితే_ముగింపు

    /*
     * This function is used very often, do some speed optimizations.
     * When 'list', 'linebreak', 'showbreak' and 'breakindent' are not set
     * and there are no text properties with "text" use a simple loop.
     * Also use this when 'list' is set but tabs take their normal size.
     */
    ఒకవేళ ((!wp->w_p_list || wp->w_lcs_chars.tab1 != NUL)
#నిర్వచితమైతే FEAT_LINEBREAK
	    && !wp->w_p_lbr && *get_showbreak_విలువ(wp) == NUL && !wp->w_p_bri
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_పాఠ్యం
#అయితే_ముగింపు
       )
    {
	కొరకు (;;)
	{
	    తల = 0;
	    c = *ptr;
	    // make sure we don't go past the end of the line
	    ఒకవేళ (c == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
		ఆపు;
	    }
	    // A tab gets expanded, depending on the current column
	    ఒకవేళ (c == TAB)
#నిర్వచితమైతే FEAT_VARTABS
		incr = tabstop_padding(vcol, ts, vts);
#లేకపోతే
		incr = ts - (vcol % ts);
#అయితే_ముగింపు
	    లేకపోతే
	    {
		ఒకవేళ (has_mbyte)
		{
		    // For utf-8, if the byte is >= 0x80, need to look at
		    // further bytes to find the cell width.
		    ఒకవేళ (enc_utf8 && c >= 0x80)
			incr = utf_ptr2cells(ptr);
		    లేకపోతే
			incr = g_chartab[c] & CT_CELL_ముసుగు;

		    // If a double-cell char doesn't fit at the end of a line
		    // it wraps to the next line, it's like this char is three
		    // cells wide.
		    ఒకవేళ (incr == 2 && wp->w_p_wrap && MB_BYTE2LEN(*ptr) > 1
			    && లోపల_win_border(wp, vcol))
		    {
			++incr;
			తల = 1;
		    }
		}
		లేకపోతే
		    incr = g_chartab[c] & CT_CELL_ముసుగు;
	    }

	    char_u *తదుపరి_ptr = ptr + (*mb_ptr2len)(ptr);
	    ఒకవేళ (తదుపరి_ptr - పంక్తి > pos->నిలువు) // character at pos->col
		ఆపు;

	    vcol += incr;
	    ptr = తదుపరి_ptr;
	}

	తోక = 0;
    }
    లేకపోతే
    {
	కొరకు (;;)
	{
	    // A tab gets expanded, depending on the current column.
	    // Other things also take up space.
	    తల = 0;
	    తోక = 0;
	    incr = win_lbr_chartabsize(&cts, &తల, &తోక);
#నిర్వచితమైతే FEAT_PROP_POPUP
	    ఒకవేళ (cts.cts_ptr == cts.cts_పంక్తి)
	    {
		ఒకవేళ (flags & GETVCOL_FOR_VIRTCOL)
		    // do not count the virtual text above for w_curswant
		    wp->w_virtcol_మొదటి_char = cts.cts_మొదటి_char;
		ఒకవేళ ((flags & GETVCOL_NO_ABOVE) == 0)
		{
		    cts.cts_vcol += cts.cts_మొదటి_char;
		    cts.cts_above_వెడల్పు = cts.cts_మొదటి_char;
		}
	    }
#అయితే_ముగింపు
	    // make sure we don't go past the end of the line
	    ఒకవేళ (*cts.cts_ptr == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
#నిర్వచితమైతే FEAT_PROP_POPUP
		incr += cts.cts_cur_పాఠ్యం_వెడల్పు;
		on_NUL = TRUE;
#అయితే_ముగింపు
		ఆపు;
	    }

	    char_u *తదుపరి_ptr = cts.cts_ptr + (*mb_ptr2len)(cts.cts_ptr);
	    ఒకవేళ (తదుపరి_ptr - పంక్తి > pos->నిలువు) // character at pos->col
		ఆపు;

	    cts.cts_vcol += incr;
	    cts.cts_ptr = తదుపరి_ptr;
	}
	vcol = cts.cts_vcol;
	ptr = cts.cts_ptr;
    }
    clear_chartabsize_arg(&cts);

    ఒకవేళ (*ptr == NUL && pos->నిలువు < MAXCOL && pos->నిలువు > ptr - పంక్తి)
	pos->నిలువు = ptr - పంక్తి;

    ఒకవేళ (ప్రారంభం != ఖాళీ)
	*ప్రారంభం = vcol + తల;
    ఒకవేళ (చివర != ఖాళీ)
	*చివర = vcol + incr - (flags & GETVCOL_చివర_EXCL_LBR ? తోక : 0) - 1;
    ఒకవేళ (cursor != ఖాళీ)
    {
	ఒకవేళ (*ptr == TAB
		&& (State & బహుళకం_సాధారణ)
		&& !wp->w_p_list
		&& !virtual_చురుకైన()
		&& !(VIsual_చురుకైన
				&& (*p_sel == 'e' || LTOREQ_POS(*pos, VIsual)))
		)
	    // TODO: subtracting "tail" may lead to better cursor position
	    *cursor = vcol + incr - 1;	    // cursor at end
	లేకపోతే
	{
#నిర్వచితమైతే FEAT_PROP_POPUP
	    // in Insert mode, if "start_incl" is true the text gets inserted
	    // after the virtual text, thus add its width
	    ఒకవేళ (((State & బహుళకం_చొప్పించు) == 0 || cts.cts_ప్రారంభం_incl) && !on_NUL)
		// cursor is after inserted text, unless on the NUL
		vcol += cts.cts_cur_పాఠ్యం_వెడల్పు;
#అయితే_ముగింపు
	    *cursor = vcol + తల;	    // cursor at start
	}
    }
}

/*
 * Get virtual cursor column in the current window, pretending 'list' is off.
 */
    colnr_T
getvcol_nolist(pos_T *posp)
{
    పూర్ణాంకం		list_save = curwin->w_p_list;
    colnr_T	vcol;

    curwin->w_p_list = FALSE;
    ఒకవేళ (posp->coladd)
	getvvcol(curwin, posp, ఖాళీ, &vcol, ఖాళీ, 0);
    లేకపోతే
	getvcol(curwin, posp, ఖాళీ, &vcol, ఖాళీ, 0);
    curwin->w_p_list = list_save;
    తిరిగి vcol;
}

/*
 * Get virtual column in virtual mode.
 */
    శూన్య
getvvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*ప్రారంభం,
    colnr_T	*cursor,
    colnr_T	*చివర,
    పూర్ణాంకం		flags)
{
    colnr_T	నిలువు;
    colnr_T	coladd;
    colnr_T	endadd;
    char_u	*ptr;

    ఒకవేళ (virtual_చురుకైన())
    {
	// For virtual mode, only want one value
	getvcol(wp, pos, &నిలువు, ఖాళీ, ఖాళీ, flags);

	coladd = pos->coladd;
	endadd = 0;
	// Cannot put the cursor on part of a wide character.
	ptr = ml_get_buf(wp->w_బఫరు, pos->lnum, FALSE);
	ఒకవేళ (pos->నిలువు < ml_get_buf_len(wp->w_బఫరు, pos->lnum))
	{
	    పూర్ణాంకం c = (*mb_ptr2char)(ptr + pos->నిలువు);

	    ఒకవేళ (c != TAB && vim_isprintc(c))
	    {
		endadd = (colnr_T)(char2cells(c) - 1);
		ఒకవేళ (coladd > endadd)	// past end of line
		    endadd = 0;
		లేకపోతే
		    coladd = 0;
	    }
	}
	నిలువు += coladd;
	ఒకవేళ (ప్రారంభం != ఖాళీ)
	    *ప్రారంభం = నిలువు;
	ఒకవేళ (cursor != ఖాళీ)
	    *cursor = నిలువు;
	ఒకవేళ (చివర != ఖాళీ)
	    *చివర = నిలువు + endadd;
    }
    లేకపోతే
	getvcol(wp, pos, ప్రారంభం, cursor, చివర, flags);
}

/*
 * Get the leftmost and rightmost virtual column of pos1 and pos2.
 * Used for Visual block mode.
 */
    శూన్య
getvcols(
    win_T	*wp,
    pos_T	*pos1,
    pos_T	*pos2,
    colnr_T	*ఎడమ_2,
    colnr_T	*కుడి_2,
    పూర్ణాంకం		flags)
{
    colnr_T	from1, from2, to1, to2;

    ఒకవేళ (LT_POSP(pos1, pos2))
    {
	getvvcol(wp, pos1, &from1, ఖాళీ, &to1, flags);
	getvvcol(wp, pos2, &from2, ఖాళీ, &to2, flags);
    }
    లేకపోతే
    {
	getvvcol(wp, pos2, &from1, ఖాళీ, &to1, flags);
	getvvcol(wp, pos1, &from2, ఖాళీ, &to2, flags);
    }
    ఒకవేళ (from2 < from1)
	*ఎడమ_2 = from2;
    లేకపోతే
	*ఎడమ_2 = from1;
    ఒకవేళ (to2 > to1)
    {
	ఒకవేళ (*p_sel == 'e' && from2 - 1 >= to1)
	    *కుడి_2 = from2 - 1;
	లేకపోతే
	    *కుడి_2 = to2;
    }
    లేకపోతే
	*కుడి_2 = to1;
}

/*
 * Skip over ' ' and '\t'.
 */
    char_u *
skipwhite(char_u *q)
{
    char_u	*p = q;

    అయితే (VIM_ISWHITE(*p))
	++p;
    తిరిగి p;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * skip over ' ', '\t' and '\n'.
 */
    char_u *
skipwhite_and_nl(char_u *q)
{
    char_u	*p = q;

    అయితే (VIM_ISWHITE(*p) || *p == NL)
	++p;
    తిరిగి p;
}
#అయితే_ముగింపు

/*
 * getwhitecols: return the number of whitespace
 * columns (bytes) at the start of a given line
 */
    పూర్ణాంకం
getwhitecols_curline(శూన్య)
{
    తిరిగి getwhitecols(ml_get_curline());
}

    పూర్ణాంకం
getwhitecols(char_u *p)
{
    తిరిగి skipwhite(p) - p;
}

/*
 * skip over digits
 */
    char_u *
skipdigits(char_u *q)
{
    char_u	*p = q;

    అయితే (VIM_ISDIGIT(*p))	// skip to next non-digit
	++p;
    తిరిగి p;
}

#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * skip over binary digits
 */
    char_u *
skipbin(char_u *q)
{
    char_u	*p = q;

    అయితే (vim_isbdigit(*p))	// skip to next non-digit
	++p;
    తిరిగి p;
}

/*
 * skip over digits and hex characters
 */
    char_u *
skiphex(char_u *q)
{
    char_u	*p = q;

    అయితే (vim_isxdigit(*p))	// skip to next non-digit
	++p;
    తిరిగి p;
}
#అయితే_ముగింపు

/*
 * skip to bin digit (or NUL after the string)
 */
    char_u *
skiptobin(char_u *q)
{
    char_u	*p = q;

    అయితే (*p != NUL && !vim_isbdigit(*p))	// skip to next digit
	++p;
    తిరిగి p;
}

/*
 * skip to digit (or NUL after the string)
 */
    char_u *
skiptodigit(char_u *q)
{
    char_u	*p = q;

    అయితే (*p != NUL && !VIM_ISDIGIT(*p))	// skip to next digit
	++p;
    తిరిగి p;
}

/*
 * skip to hex character (or NUL after the string)
 */
    char_u *
skiptohex(char_u *q)
{
    char_u	*p = q;

    అయితే (*p != NUL && !vim_isxdigit(*p))	// skip to next digit
	++p;
    తిరిగి p;
}

/*
 * Variant of isdigit() that can handle characters > 0x100.
 * We don't use isdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 * Use the VIM_ISDIGIT() macro for simple arguments.
 */
    పూర్ణాంకం
vim_isdigit(పూర్ణాంకం c)
{
    తిరిగి (c >= '0' && c <= '9');
}

/*
 * Variant of isxdigit() that can handle characters > 0x100.
 * We don't use isxdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 */
    పూర్ణాంకం
vim_isxdigit(పూర్ణాంకం c)
{
    తిరిగి (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'f')
	|| (c >= 'A' && c <= 'F');
}

/*
 * Corollary of vim_isdigit and vim_isxdigit() that can handle
 * characters > 0x100.
 */
    పూర్ణాంకం
vim_isbdigit(పూర్ణాంకం c)
{
    తిరిగి (c == '0' || c == '1');
}

    స్థిర పూర్ణాంకం
vim_isodigit(పూర్ణాంకం c)
{
    తిరిగి (c >= '0' && c <= '7');
}

/*
 * Vim's own character class functions.  These exist because many library
 * islower()/toupper() etc. do not work properly: they crash when used with
 * invalid values or can't handle latin1 when the locale is C.
 * Speed is most important here.
 */
#నిర్వచించు LATIN1_క్రింది 'l'
#నిర్వచించు LATIN1_పై 'U'

స్థిర char_u latin1flags[257] = "                                                                 UUUUUUUUUUUUUUUUUUUUUUUUUU      llllllllllllllllllllllllll                                                                     UUUUUUUUUUUUUUUUUUUUUUU UUUUUUUllllllllllllllllllllllll llllllll";
స్థిర char_u latin1upper[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xf7\xd8\xd9\xda\xdb\xdc\xdd\xde\xff";
స్థిర char_u latin1lower[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xd7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    పూర్ణాంకం
vim_islower(పూర్ణాంకం c)
{
    ఒకవేళ (c <= '@')
	తిరిగి FALSE;
    ఒకవేళ (c >= 0x80)
    {
	ఒకవేళ (enc_utf8)
	    తిరిగి utf_islower(c);
	ఒకవేళ (c >= 0x100)
	{
#నిర్వచితమైతే HAVE_ISWLOWER
	    ఒకవేళ (has_mbyte)
		తిరిగి iswlower(c);
#అయితే_ముగింపు
	    // islower() can't handle these chars and may crash
	    తిరిగి FALSE;
	}
	ఒకవేళ (enc_latin1like)
	    తిరిగి (latin1flags[c] & LATIN1_క్రింది) == LATIN1_క్రింది;
    }
    తిరిగి సురక్షిత_islower(c);
}

    పూర్ణాంకం
vim_isupper(పూర్ణాంకం c)
{
    ఒకవేళ (c <= '@')
	తిరిగి FALSE;
    ఒకవేళ (c >= 0x80)
    {
	ఒకవేళ (enc_utf8)
	    తిరిగి utf_isupper(c);
	ఒకవేళ (c >= 0x100)
	{
#నిర్వచితమైతే HAVE_ISWUPPER
	    ఒకవేళ (has_mbyte)
		తిరిగి iswupper(c);
#అయితే_ముగింపు
	    // islower() can't handle these chars and may crash
	    తిరిగి FALSE;
	}
	ఒకవేళ (enc_latin1like)
	    తిరిగి (latin1flags[c] & LATIN1_పై) == LATIN1_పై;
    }
    తిరిగి సురక్షిత_isupper(c);
}

    పూర్ణాంకం
vim_isalpha(పూర్ణాంకం c)
{
    తిరిగి vim_islower(c) || vim_isupper(c);
}

    పూర్ణాంకం
vim_toupper(పూర్ణాంకం c)
{
    ఒకవేళ (c <= '@')
	తిరిగి c;
    ఒకవేళ (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	ఒకవేళ (enc_utf8)
	    తిరిగి utf_toupper(c);
	ఒకవేళ (c >= 0x100)
	{
#నిర్వచితమైతే HAVE_TOWUPPER
	    ఒకవేళ (has_mbyte)
		తిరిగి towupper(c);
#అయితే_ముగింపు
	    // toupper() can't handle these chars and may crash
	    తిరిగి c;
	}
	ఒకవేళ (enc_latin1like)
	    తిరిగి latin1upper[c];
    }
    ఒకవేళ (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	తిరిగి TOUPPER_ASC(c);
    తిరిగి TOUPPER_LOC(c);
}

    పూర్ణాంకం
vim_tolower(పూర్ణాంకం c)
{
    ఒకవేళ (c <= '@')
	తిరిగి c;
    ఒకవేళ (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	ఒకవేళ (enc_utf8)
	    తిరిగి utf_tolower(c);
	ఒకవేళ (c >= 0x100)
	{
#నిర్వచితమైతే HAVE_TOWLOWER
	    ఒకవేళ (has_mbyte)
		తిరిగి towlower(c);
#అయితే_ముగింపు
	    // tolower() can't handle these chars and may crash
	    తిరిగి c;
	}
	ఒకవేళ (enc_latin1like)
	    తిరిగి latin1lower[c];
    }
    ఒకవేళ (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	తిరిగి TOLOWER_ASC(c);
    తిరిగి TOLOWER_LOC(c);
}

/*
 * skiptowhite: skip over text until ' ' or '\t' or NUL.
 */
    char_u *
skiptowhite(char_u *p)
{
    అయితే (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    తిరిగి p;
}

/*
 * skiptowhite_esc: Like skiptowhite(), but also skip escaped chars
 */
    char_u *
skiptowhite_esc(char_u *p)
{
    అయితే (*p != ' ' && *p != '\t' && *p != NUL)
    {
	ఒకవేళ ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    తిరిగి p;
}

/*
 * Get a number from a string and skip over it.
 * Note: the argument is a pointer to a char_u pointer!
 */
    దీర్ఘ
getdigits(char_u **pp)
{
    char_u	*p;
    దీర్ఘ	retval;

    p = *pp;
    retval = strtol((అక్షరం *)p, ఖాళీ, 10);
    ఒకవేళ (*p == '-')		// skip negative sign
	++p;
    p = skipdigits(p);		// skip to next non-digit
    *pp = p;
    తిరిగి retval;
}

/*
 * Like getdigits() but allow for embedded single quotes.
 */
    దీర్ఘ
getdigits_quoted(char_u **pp)
{
    char_u	*p = *pp;
    దీర్ఘ	retval = 0;

    ఒకవేళ (*p == '-')
	++p;
    అయితే (VIM_ISDIGIT(*p))
    {
	ఒకవేళ (retval >= LONG_MAX / 10 - 10)
	    retval = LONG_MAX;
	లేకపోతే
	    retval = retval * 10 - '0' + *p;
	++p;
	ఒకవేళ (లోపల_vim9script() && *p == '\'' && VIM_ISDIGIT(p[1]))
	    ++p;
    }
    ఒకవేళ (**pp == '-')
    {
	ఒకవేళ (retval == LONG_MAX)
	    retval = LONG_MIN;
	లేకపోతే
	    retval = -retval;
    }
    *pp = p;
    తిరిగి retval;
}

/*
 * Return TRUE if "lbuf" is empty or only contains blanks.
 */
    పూర్ణాంకం
vim_isblankline(char_u *lbuf)
{
    char_u	*p;

    p = skipwhite(lbuf);
    తిరిగి (*p == NUL || *p == '\r' || *p == '\n');
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
    శూన్య
vim_str2nr(
    char_u		*ప్రారంభం,
    పూర్ణాంకం			*prep,	    // return: type of number 0 = decimal, 'x'
				    // or 'X' is hex, '0', 'o' or 'O' is octal,
				    // 'b' or 'B' is bin
    పూర్ణాంకం			*len,	    // return: detected length of number
    పూర్ణాంకం			what,	    // what numbers to recognize
    varnumber_T		*nptr,	    // return: signed result
    uvarnumber_T	*unptr,	    // return: unsigned result
    పూర్ణాంకం			maxlen,     // max length of string to check
    పూర్ణాంకం			strict,     // check strictly
    పూర్ణాంకం			*అధికప్రవాహం)  // when not NULL set to TRUE for overflow
{
    char_u	    *ptr = ప్రారంభం;
    పూర్ణాంకం		    pre = 0;		// default is decimal
    పూర్ణాంకం		    negative = FALSE;
    uvarnumber_T    un = 0;
    పూర్ణాంకం		    n;

    ఒకవేళ (len != ఖాళీ)
	*len = 0;

    ఒకవేళ (ptr[0] == '-')
    {
	negative = TRUE;
	++ptr;
    }

    // Recognize hex, octal, and bin.
    ఒకవేళ (ptr[0] == '0' && ptr[1] != '8' && ptr[1] != '9'
					       && (maxlen == 0 || maxlen > 1))
    {
	pre = ptr[1];
	ఒకవేళ ((what & STR2NR_HEX)
		&& (pre == 'X' || pre == 'x') && vim_isxdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // hexadecimal
	    ptr += 2;
	లేకపోతే ఒకవేళ ((what & STR2NR_BIN)
		&& (pre == 'B' || pre == 'b') && vim_isbdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    //ద్విమాన
	    ptr += 2;
	లేకపోతే ఒకవేళ ((what & STR2NR_OOCT)
		&& (pre == 'O' || pre == 'o') && vim_isodigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // octal with prefix "0o"
	    ptr += 2;
	లేకపోతే
	{
	    // decimal or octal, default is decimal
	    pre = 0;
	    ఒకవేళ (what & STR2NR_OCT)
	    {
		// Don't interpret "0", "08" or "0129" as octal.
		కొరకు (n = 1; n != maxlen && VIM_ISDIGIT(ptr[n]); ++n)
		{
		    ఒకవేళ (ptr[n] > '7')
		    {
			pre = 0;	// can't be octal
			ఆపు;
		    }
		    pre = '0';	// assume octal
		}
	    }
	}
    }

    // Do the conversion manually to avoid sscanf() quirks.
    n = 1;
    ఒకవేళ (pre == 'B' || pre == 'b'
			     || ((what & STR2NR_BIN) && (what & STR2NR_FORCE)))
    {
	// bin
	ఒకవేళ (pre != 0)
	    n += 2;	    // skip over "0b"
	అయితే ('0' <= *ptr && *ptr <= '1')
	{
	    // avoid ubsan error for overflow
	    ఒకవేళ (un <= UVARNUM_MAX / 2)
		un = 2 * un + (uvarnumber_T)(*ptr - '0');
	    లేకపోతే
	    {
		un = UVARNUM_MAX;
		ఒకవేళ (అధికప్రవాహం != ఖాళీ)
		    *అధికప్రవాహం = TRUE;
	    }
	    ++ptr;
	    ఒకవేళ (n++ == maxlen)
		ఆపు;
	    ఒకవేళ ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '1')
	    {
		++ptr;
		ఒకవేళ (n++ == maxlen)
		    ఆపు;
	    }
	}
    }
    లేకపోతే ఒకవేళ (pre == 'O' || pre == 'o' ||
		pre == '0' || ((what & STR2NR_OCT) && (what & STR2NR_FORCE)))
    {
	// octal
	ఒకవేళ (pre != 0 && pre != '0')
	    n += 2;	    // skip over "0o"
	అయితే ('0' <= *ptr && *ptr <= '7')
	{
	    // avoid ubsan error for overflow
	    ఒకవేళ (un <= UVARNUM_MAX / 8)
		un = 8 * un + (uvarnumber_T)(*ptr - '0');
	    లేకపోతే
	    {
		un = UVARNUM_MAX;
		ఒకవేళ (అధికప్రవాహం != ఖాళీ)
		    *అధికప్రవాహం = TRUE;
	    }
	    ++ptr;
	    ఒకవేళ (n++ == maxlen)
		ఆపు;
	    ఒకవేళ ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '7')
	    {
		++ptr;
		ఒకవేళ (n++ == maxlen)
		    ఆపు;
	    }
	}
    }
    లేకపోతే ఒకవేళ (pre != 0 || ((what & STR2NR_HEX) && (what & STR2NR_FORCE)))
    {
	// hex
	ఒకవేళ (pre != 0)
	    n += 2;	    // skip over "0x"
	అయితే (vim_isxdigit(*ptr))
	{
	    // avoid ubsan error for overflow
	    ఒకవేళ (un <= UVARNUM_MAX / 16)
		un = 16 * un + (uvarnumber_T)hex2nr(*ptr);
	    లేకపోతే
	    {
		un = UVARNUM_MAX;
		ఒకవేళ (అధికప్రవాహం != ఖాళీ)
		    *అధికప్రవాహం = TRUE;
	    }
	    ++ptr;
	    ఒకవేళ (n++ == maxlen)
		ఆపు;
	    ఒకవేళ ((what & STR2NR_QUOTE) && *ptr == '\'' && vim_isxdigit(ptr[1]))
	    {
		++ptr;
		ఒకవేళ (n++ == maxlen)
		    ఆపు;
	    }
	}
    }
    లేకపోతే
    {
	// decimal
	అయితే (VIM_ISDIGIT(*ptr))
	{
	    uvarnumber_T    digit = (uvarnumber_T)(*ptr - '0');

	    // avoid ubsan error for overflow
	    ఒకవేళ (un < UVARNUM_MAX / 10
		    || (un == UVARNUM_MAX / 10 && digit <= UVARNUM_MAX % 10))
		un = 10 * un + digit;
	    లేకపోతే
	    {
		un = UVARNUM_MAX;
		ఒకవేళ (అధికప్రవాహం != ఖాళీ)
		    *అధికప్రవాహం = TRUE;
	    }
	    ++ptr;
	    ఒకవేళ (n++ == maxlen)
		ఆపు;
	    ఒకవేళ ((what & STR2NR_QUOTE) && *ptr == '\'' && VIM_ISDIGIT(ptr[1]))
	    {
		++ptr;
		ఒకవేళ (n++ == maxlen)
		    ఆపు;
	    }
	}
    }

    // Check for an alphanumeric character immediately following, that is
    // most likely a typo.
    ఒకవేళ (strict && n - 1 != maxlen && ASCII_ISALNUM(*ptr))
	తిరిగి;

    ఒకవేళ (prep != ఖాళీ)
	*prep = pre;
    ఒకవేళ (len != ఖాళీ)
	*len = (పూర్ణాంకం)(ptr - ప్రారంభం);
    ఒకవేళ (nptr != ఖాళీ)
    {
	ఒకవేళ (negative)   // account for leading '-' for decimal numbers
	{
	    // avoid ubsan error for overflow
	    ఒకవేళ (un > VARNUM_MAX)
	    {
		*nptr = VARNUM_MIN;
		ఒకవేళ (అధికప్రవాహం != ఖాళీ)
		    *అధికప్రవాహం = TRUE;
	    }
	    లేకపోతే
		*nptr = -(varnumber_T)un;
	}
	లేకపోతే
	{
	    // prevent a large unsigned number to become negative
	    ఒకవేళ (un > VARNUM_MAX)
	    {
		un = VARNUM_MAX;
		ఒకవేళ (అధికప్రవాహం != ఖాళీ)
		    *అధికప్రవాహం = TRUE;
	    }
	    *nptr = (varnumber_T)un;
	}
    }
    ఒకవేళ (unptr != ఖాళీ)
	*unptr = un;
}

/*
 * Return the value of a single hex character.
 * Only valid when the argument is '0' - '9', 'A' - 'F' or 'a' - 'f'.
 */
    పూర్ణాంకం
hex2nr(పూర్ణాంకం c)
{
    ఒకవేళ (c >= 'a' && c <= 'f')
	తిరిగి c - 'a' + 10;
    ఒకవేళ (c >= 'A' && c <= 'F')
	తిరిగి c - 'A' + 10;
    తిరిగి c - '0';
}

/*
 * Convert two hex characters to a byte.
 * Return -1 if one of the characters is not hex.
 */
    పూర్ణాంకం
hexhex2nr(char_u *p)
{
    ఒకవేళ (!vim_isxdigit(p[0]) || !vim_isxdigit(p[1]))
	తిరిగి -1;
    తిరిగి (hex2nr(p[0]) << 4) + hex2nr(p[1]);
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
    పూర్ణాంకం
rem_backslash(char_u *str)
{
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    తిరిగి (str[0] == '\\'
	    && str[1] < 0x80
	    && (str[1] == ' '
		|| (str[1] != NUL
		    && str[1] != '*'
		    && str[1] != '?'
		    && !vim_isfilec(str[1]))));
#లేకపోతే
    తిరిగి (str[0] == '\\' && str[1] != NUL);
#అయితే_ముగింపు
}

/*
 * Halve the number of backslashes in a file name argument.
 * For MS-DOS we only do this if the character after the backslash
 * is not a normal file character.
 */
    శూన్య
backslash_halve(char_u *p)
{
    కొరకు ( ; *p; ++p)
	ఒకవేళ (rem_backslash(p))
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
    ఒకవేళ (res == ఖాళీ)
	తిరిగి p;
    backslash_halve(res);
    తిరిగి res;
}
