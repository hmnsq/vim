/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ parse_isopt(char_u *var, buf_T *buf, ਪੂਰਨ only_check);
ਸਥਿਰ ਪੂਰਨ win_nolbr_chartabsize(chartabsize_T *cts, ਪੂਰਨ *headp);
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ nr2hex(ਬਿਨਾਨਿਸ਼ਾਨੀ c);

ਸਥਿਰ ਪੂਰਨ    chartab_initialized = FALSE;

// b_chartab[] is an array of 32 bytes, each bit representing one of the
// characters 0-255.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_CHARTAB(buf, c) (buf)->b_chartab[(ਬਿਨਾਨਿਸ਼ਾਨੀ)(c) >> 3] |= (1 << ((c) & 0x7))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESET_CHARTAB(buf, c) (buf)->b_chartab[(ਬਿਨਾਨਿਸ਼ਾਨੀ)(c) >> 3] &= ~(1 << ((c) & 0x7))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_CHARTAB(buf, c) ((buf)->b_chartab[(ਬਿਨਾਨਿਸ਼ਾਨੀ)(c) >> 3] & (1 << ((c) & 0x7)))

// table used below, see init_chartab() for an explanation
ਸਥਿਰ char_u	g_chartab[256];

/*
 * Flags for g_chartab[].
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CT_CELL_ਨਕਾਬ	0x07	// mask: nr of display cells (1, 2 or 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CT_PRINT_CHAR	0x10	// flag: set for printable chars
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CT_ID_CHAR	0x20	// flag: set for ID chars
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CT_FNAME_CHAR	0x40	// flag: set for file name chars

ਸਥਿਰ ਪੂਰਨ ਵਿੱਚ_win_border(win_T *wp, colnr_T vcol);

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
    ਪੂਰਨ
init_chartab(ਖਾਲੀ)
{
    ਵਾਪਸ buf_init_chartab(curbuf, TRUE);
}

    ਪੂਰਨ
buf_init_chartab(
    buf_T	*buf,
    ਪੂਰਨ		global)		// FALSE: only set buf->b_chartab[]
{
    ਪੂਰਨ		c;
    char_u	*p;
    ਪੂਰਨ		i;

    ਜੇ (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 */
	c = 0;
	ਜਦੋਂ (c < ' ')
	    g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	ਜਦੋਂ (c <= '~')
	    g_chartab[c++] = 1 + CT_PRINT_CHAR;
	ਜਦੋਂ (c < 256)
	{
	    // UTF-8: bytes 0xa0 - 0xff are printable (latin1)
	    ਜੇ (enc_utf8 && c >= 0xa0)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // euc-jp characters starting with 0x8e are single width
	    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs == DBCS_JPNU && c == 0x8e)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // other double-byte chars can be printable AND double-width
	    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs != 0 && MB_BYTE2LEN(c) == 2)
		g_chartab[c++] = CT_PRINT_CHAR + 2;
	    ਨਹੀਂਤਾਂ
		// the rest is unprintable by default
		g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	}

	// Assume that every multi-byte char is a filename character.
	ਲਈ (c = 1; c < 256; ++c)
	    ਜੇ ((enc_dbcs != 0 && MB_BYTE2LEN(c) > 1)
		    || (enc_dbcs == DBCS_JPNU && c == 0x8e)
		    || (enc_utf8 && c >= 0xa0))
		g_chartab[c] |= CT_FNAME_CHAR;
    }

    /*
     * Init word char flags all to FALSE
     */
    CLEAR_ਖੇਤਰ(buf->b_chartab);
    ਜੇ (enc_dbcs != 0)
	ਲਈ (c = 0; c < 256; ++c)
	{
	    // double-byte characters are probably word characters
	    ਜੇ (MB_BYTE2LEN(c) == 2)
		ਸੈੱਟ_CHARTAB(buf, c);
	}

    /*
     * In lisp mode the '-' character is included in keywords.
     */
    ਜੇ (buf->b_p_lisp)
	ਸੈੱਟ_CHARTAB(buf, '-');

    // Walk through the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    ਲਈ (i = global ? 0 : 3; i <= 3; ++i)
    {
	ਜੇ (i == 0)
	    p = p_isi;		// first round: 'isident'
	ਨਹੀਂਤਾਂ ਜੇ (i == 1)
	    p = p_isp;		// second round: 'isprint'
	ਨਹੀਂਤਾਂ ਜੇ (i == 2)
	    p = p_isf;		// third round: 'isfname'
	ਨਹੀਂਤਾਂ	// i == 3
	    p = buf->b_p_isk;	// fourth round: 'iskeyword'

	ਜੇ (parse_isopt(p, buf, FALSE) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    chartab_initialized = TRUE;
    ਵਾਪਸ OK;
}

/**
 * Checks the format for the option settings 'iskeyword', 'isident', 'isfname'
 * or 'isprint'.
 * Returns FAIL if has an error, OK otherwise.
 */
	ਪੂਰਨ
check_isopt(char_u *var)
{
    ਵਾਪਸ parse_isopt(var, ਨਿਰਾ, TRUE);
}

    ਸਥਿਰ ਪੂਰਨ
parse_isopt(
	char_u	*var,
	buf_T	*buf,
	ਪੂਰਨ	only_check)	// FALSE: refill g_chartab[]
{
    char_u	*p = var;
    ਪੂਰਨ		c;
    ਪੂਰਨ		c2;
    ਪੂਰਨ		tilde;
    ਪੂਰਨ		do_isalpha;
    ਪੂਰਨ		trail_comma;

    // Parses the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    // Each option is a list of characters, character numbers or ranges,
    // separated by commas, e.g.: "200-210,x,#-178,-"
    ਜਦੋਂ (*p)
    {
	tilde = FALSE;
	do_isalpha = FALSE;
	ਜੇ (*p == '^' && p[1] != NUL)
	{
	    tilde = TRUE;
	    ++p;
	}
	ਜੇ (VIM_ISDIGIT(*p))
	    c = getdigits(&p);
	ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	    c = mb_ptr2char_adv(&p);
	ਨਹੀਂਤਾਂ
	    c = *p++;
	c2 = -1;
	ਜੇ (*p == '-' && p[1] != NUL)
	{
	    ++p;
	    ਜੇ (VIM_ISDIGIT(*p))
		c2 = getdigits(&p);
	    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
		c2 = mb_ptr2char_adv(&p);
	    ਨਹੀਂਤਾਂ
		c2 = *p++;
	}
	ਜੇ (c <= 0 || c >= 256 || (c2 < c && c2 != -1) || c2 >= 256
						  || !(*p == NUL || *p == ','))
	    ਵਾਪਸ FAIL;

	trail_comma = *p == ',';
	p = skip_ਮੰਜ਼ਿਲ_option_part(p);
	ਜੇ (trail_comma && *p == NUL)
	    // Trailing comma is not allowed.
	    ਵਾਪਸ FAIL;

	ਜੇ (only_check)
	    ਜਾਰੀ;

	ਜੇ (c2 == -1)	// not a range
	{
	    /*
	     * A single '@' (not "@-@"):
	     * Decide on letters being ID/printable/keyword chars with
	     * standard function isalpha(). This takes care of locale for
	     * single-byte characters).
	     */
	    ਜੇ (c == '@')
	    {
		do_isalpha = TRUE;
		c = 1;
		c2 = 255;
	    }
	    ਨਹੀਂਤਾਂ
		c2 = c;
	}

	ਜਦੋਂ (c <= c2)
	{
	    // Use the MB_ functions here, because isalpha() doesn't
	    // work properly when 'encoding' is "latin1" and the locale is
	    // "C".
	    ਜੇ (!do_isalpha || MB_ISLOWER(c) || MB_ISUPPER(c))
	    {
		ਜੇ (var == p_isi)			// (re)set ID flag
		{
		    ਜੇ (tilde)
			g_chartab[c] &= ~CT_ID_CHAR;
		    ਨਹੀਂਤਾਂ
			g_chartab[c] |= CT_ID_CHAR;
		}
		ਨਹੀਂਤਾਂ ਜੇ (var == p_isp)			// (re)set printable
		{
		    ਜੇ ((c < ' ' || c > '~'
			    // For double-byte we keep the cell width, so
			    // that we can detect it from the first byte.
			) && !(enc_dbcs && MB_BYTE2LEN(c) == 2))
		    {
			ਜੇ (tilde)
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_ਨਕਾਬ)
					 + ((dy_flags & DY_UHEX) ? 4 : 2);
			    g_chartab[c] &= ~CT_PRINT_CHAR;
			}
			ਨਹੀਂਤਾਂ
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_ਨਕਾਬ) + 1;
			    g_chartab[c] |= CT_PRINT_CHAR;
			}
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (var == p_isf)			// (re)set fname flag
		{
		    ਜੇ (tilde)
			g_chartab[c] &= ~CT_FNAME_CHAR;
		    ਨਹੀਂਤਾਂ
			g_chartab[c] |= CT_FNAME_CHAR;
		}
		ਨਹੀਂਤਾਂ // var == p_isk || var == buf->b_p_isk
							// (re)set keyword flag
		{
		    ਜੇ (tilde)
			RESET_CHARTAB(buf, c);
		    ਨਹੀਂਤਾਂ
			ਸੈੱਟ_CHARTAB(buf, c);
		}
	    }
	    ++c;
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Translate any special characters in buf[bufsize] in-place.
 * The result is a string with only printable characters, but if there is not
 * enough room, not all characters will be translated.
 */
    ਖਾਲੀ
trans_characters(
    char_u	*buf,
    ਪੂਰਨ		bufsize)
{
    ਪੂਰਨ		len;		// length of string needing translation
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;		// room in buffer after string
    char_u	*trs;		// translated character
    ਪੂਰਨ		trs_len;	// length of trs[]

    len = (ਪੂਰਨ)STRLEN(buf);
    ਗੁੰਜਾਇਸ਼ = bufsize - len;
    ਜਦੋਂ (*buf != 0)
    {
	// Assume a multi-byte character doesn't need translation.
	ਜੇ (has_mbyte && (trs_len = (*mb_ptr2len)(buf)) > 1)
	    len -= trs_len;
	ਨਹੀਂਤਾਂ
	{
	    trs = transchar_byte(*buf);
	    trs_len = (ਪੂਰਨ)STRLEN(trs);
	    ਜੇ (trs_len > 1)
	    {
		ਗੁੰਜਾਇਸ਼ -= trs_len - 1;
		ਜੇ (ਗੁੰਜਾਇਸ਼ <= 0)
		    ਵਾਪਸ;
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
    ਪੂਰਨ		l, len, c;
    char_u	hexbuf[11];

    ਜੇ (has_mbyte)
    {
	// Compute the length of the result, taking account of unprintable
	// multi-byte characters.
	len = 0;
	p = s;
	ਜਦੋਂ (*p != NUL)
	{
	    ਜੇ ((l = (*mb_ptr2len)(p)) > 1)
	    {
		c = (*mb_ptr2char)(p);
		p += l;
		ਜੇ (vim_isprintc(c))
		    len += l;
		ਨਹੀਂਤਾਂ
		{
		    transchar_hex(hexbuf, c);
		    len += (ਪੂਰਨ)STRLEN(hexbuf);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		l = byte2cells(*p++);
		ਜੇ (l > 0)
		    len += l;
		ਨਹੀਂਤਾਂ
		    len += 4;	// illegal byte sequence
	    }
	}
	res = alloc(len + 1);
    }
    ਨਹੀਂਤਾਂ
	res = alloc(vim_strsize(s) + 1);

    ਜੇ (res == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    char_u *d = res;

    p = s;
    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    c = (*mb_ptr2char)(p);
	    ਜੇ (vim_isprintc(c))
	    {
		mch_memmove(d, p, (size_t)l);
		d += l;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		transchar_hex(d, c);
		d += STRLEN(d);
	    }
	    p += l;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u	*trs = transchar_byte(*p++);
	    size_t	trs_len = STRLEN(trs);

	    mch_memmove(d, trs, trs_len);
	    d += trs_len;
	}
    }
    *d = NUL;
    ਵਾਪਸ res;
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
    ਪੂਰਨ		orglen,
    char_u	*buf,
    ਪੂਰਨ		buflen)
{
    garray_T	ga;
    ਪੂਰਨ		i;
    ਪੂਰਨ		len = orglen;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GA_CHAR(i)  ((char_u *)ga.ga_ਅੰਕੜੇ)[i]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GA_PTR(i)   ((char_u *)ga.ga_ਅੰਕੜੇ + (i))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR_CHAR(i)  (buf == ਨਿਰਾ ? GA_CHAR(i) : buf[i])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR_PTR(i)   (buf == ਨਿਰਾ ? GA_PTR(i) : buf + (i))

    // Copy "str" into "buf" or allocated memory, unmodified.
    ਜੇ (buf == ਨਿਰਾ)
    {
	ga_init2(&ga, 1, 10);
	ਜੇ (ga_grow(&ga, len + 1) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	mch_memmove(ga.ga_ਅੰਕੜੇ, str, (size_t)len);
	ga.ga_len = len;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (len >= buflen)	    // Ugly!
	    len = buflen - 1;
	mch_memmove(buf, str, (size_t)len);
    }
    ਜੇ (buf == ਨਿਰਾ)
	GA_CHAR(len) = NUL;
    ਨਹੀਂਤਾਂ
	buf[len] = NUL;

    // Make each character lower case.
    i = 0;
    ਜਦੋਂ (STR_CHAR(i) != NUL)
    {
	ਜੇ (enc_utf8 || (has_mbyte && MB_BYTE2LEN(STR_CHAR(i)) > 1))
	{
	    ਜੇ (enc_utf8)
	    {
		ਪੂਰਨ	c = utf_ptr2char(STR_PTR(i));
		ਪੂਰਨ	olen = utf_ptr2len(STR_PTR(i));
		ਪੂਰਨ	lc = utf_tolower(c);

		// Only replace the character when it is not an invalid
		// sequence (ASCII character or more than one byte) and
		// utf_tolower() doesn't return the original character.
		ਜੇ ((c < 0x80 || olen > 1) && c != lc)
		{
		    ਪੂਰਨ	    nlen = utf_char2len(lc);

		    // If the byte length changes need to shift the following
		    // characters forward or backward.
		    ਜੇ (olen != nlen)
		    {
			ਜੇ (nlen > olen)
			{
			    ਜੇ (buf == ਨਿਰਾ
				    ? ga_grow(&ga, nlen - olen + 1) == FAIL
				    : len + nlen - olen >= buflen)
			    {
				// out of memory, keep old char
				lc = c;
				nlen = olen;
			    }
			}
			ਜੇ (olen != nlen)
			{
			    ਜੇ (buf == ਨਿਰਾ)
			    {
				STRMOVE(GA_PTR(i) + nlen, GA_PTR(i) + olen);
				ga.ga_len += nlen - olen;
			    }
			    ਨਹੀਂਤਾਂ
			    {
				STRMOVE(buf + i + nlen, buf + i + olen);
				len += nlen - olen;
			    }
			}
		    }
		    (ਖਾਲੀ)utf_char2bytes(lc, STR_PTR(i));
		}
	    }
	    // skip to next multi-byte char
	    i += (*mb_ptr2len)(STR_PTR(i));
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (buf == ਨਿਰਾ)
		GA_CHAR(i) = TOLOWER_LOC(GA_CHAR(i));
	    ਨਹੀਂਤਾਂ
		buf[i] = TOLOWER_LOC(buf[i]);
	    ++i;
	}
    }

    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ (char_u *)ga.ga_ਅੰਕੜੇ;
    ਵਾਪਸ buf;
}

/*
 * Catch 22: g_chartab[] can't be initialized before the options are
 * initialized, and initializing options may cause transchar() to be called!
 * When chartab_initialized == FALSE don't use g_chartab[].
 * Does NOT work for multi-byte characters, c must be <= 255.
 * Also doesn't work for the first byte of a multi-byte, "c" must be a
 * character!
 */
ਸਥਿਰ char_u	transchar_charbuf[7];

    char_u *
transchar(ਪੂਰਨ c)
{
    ਵਾਪਸ transchar_buf(curbuf, c);
}

    char_u *
transchar_buf(buf_T *buf, ਪੂਰਨ c)
{
    ਪੂਰਨ			i;

    i = 0;
    ਜੇ (IS_SPECIAL(c))	    // special key code, display as ~@ char
    {
	transchar_charbuf[0] = '~';
	transchar_charbuf[1] = '@';
	i = 2;
	c = K_ਦੂਜਾ(c);
    }

    ਜੇ ((!chartab_initialized && ((c >= ' ' && c <= '~')))
					|| (c < 256 && vim_isprintc_strict(c)))
    {
	// printable character
	transchar_charbuf[i] = c;
	transchar_charbuf[i + 1] = NUL;
    }
    ਨਹੀਂਤਾਂ
	transchar_nonprint(buf, transchar_charbuf + i, c);
    ਵਾਪਸ transchar_charbuf;
}

/*
 * Like transchar(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of the current buffer.
 */
    char_u *
transchar_byte(ਪੂਰਨ c)
{
    ਵਾਪਸ transchar_byte_buf(curbuf, c);
}

/*
 * Like transchar_buf(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of "buf", unless it is NULL.
 */
    char_u *
transchar_byte_buf(buf_T *buf, ਪੂਰਨ c)
{
    ਜੇ (enc_utf8 && c >= 0x80)
    {
	transchar_nonprint(buf, transchar_charbuf, c);
	ਵਾਪਸ transchar_charbuf;
    }
    ਵਾਪਸ transchar_buf(buf, c);
}
/*
 * Convert non-printable character to two or more printable characters in
 * "charbuf[]".  "charbuf" needs to be able to hold five bytes.
 * Does NOT work for multi-byte characters, c must be <= 255.
 */
    ਖਾਲੀ
transchar_nonprint(buf_T *buf, char_u *charbuf, ਪੂਰਨ c)
{
    ਜੇ (c == NL)
	c = NUL;		// we use newline in place of a NUL
    ਨਹੀਂਤਾਂ ਜੇ (buf != ਨਿਰਾ && c == CAR && get_fileformat(buf) == EOL_MAC)
	c = NL;			// we use CR in place of  NL in this case

    ਜੇ (dy_flags & DY_UHEX)		// 'display' has "uhex"
	transchar_hex(charbuf, c);

    ਨਹੀਂਤਾਂ ਜੇ (c <= 0x7f)			// 0x00 - 0x1f and 0x7f
    {
	charbuf[0] = '^';
	charbuf[1] = c ^ 0x40;		// DEL displayed as ^?
	charbuf[2] = NUL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
    {
	transchar_hex(charbuf, c);
    }
    ਨਹੀਂਤਾਂ ਜੇ (c >= ' ' + 0x80 && c <= '~' + 0x80)    // 0xa0 - 0xfe
    {
	charbuf[0] = '|';
	charbuf[1] = c - 0x80;
	charbuf[2] = NUL;
    }
    ਨਹੀਂਤਾਂ					    // 0x80 - 0x9f and 0xff
    {
	charbuf[0] = '~';
	charbuf[1] = (c - 0x80) ^ 0x40;	// 0xff displayed as ~?
	charbuf[2] = NUL;
    }
}

    ਖਾਲੀ
transchar_hex(char_u *buf, ਪੂਰਨ c)
{
    ਪੂਰਨ		i = 0;

    buf[0] = '<';
    ਜੇ (c > 255)
    {
	buf[++i] = nr2hex((ਬਿਨਾਨਿਸ਼ਾਨੀ)c >> 12);
	buf[++i] = nr2hex((ਬਿਨਾਨਿਸ਼ਾਨੀ)c >> 8);
    }
    buf[++i] = nr2hex((ਬਿਨਾਨਿਸ਼ਾਨੀ)c >> 4);
    buf[++i] = nr2hex((ਬਿਨਾਨਿਸ਼ਾਨੀ)c);
    buf[++i] = '>';
    buf[++i] = NUL;
}

/*
 * Convert the lower 4 bits of byte "c" to its hex character.
 * Lower case letters are used to avoid the confusion of <F1> being 0xf1 or
 * function key 1.
 */
    ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ
nr2hex(ਬਿਨਾਨਿਸ਼ਾਨੀ c)
{
    ਜੇ ((c & 0xf) <= 9)
	ਵਾਪਸ (c & 0xf) + '0';
    ਵਾਪਸ (c & 0xf) - 10 + 'a';
}

/*
 * Return number of display cells occupied by byte "b".
 * Caller must make sure 0 <= b <= 255.
 * For multi-byte mode "b" must be the first byte of a character.
 * A TAB is counted as two cells: "^I".
 * For UTF-8 mode this will return 0 for bytes >= 0x80, because the number of
 * cells depends on further bytes.
 */
    ਪੂਰਨ
byte2cells(ਪੂਰਨ b)
{
    ਜੇ (enc_utf8 && b >= 0x80)
	ਵਾਪਸ 0;
    ਵਾਪਸ (g_chartab[b] & CT_CELL_ਨਕਾਬ);
}

/*
 * Return number of display cells occupied by character "c".
 * "c" can be a special key (negative number) in which case 3 or 4 is returned.
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    ਪੂਰਨ
char2cells(ਪੂਰਨ c)
{
    ਜੇ (IS_SPECIAL(c))
	ਵਾਪਸ char2cells(K_ਦੂਜਾ(c)) + 2;
    ਜੇ (c >= 0x80)
    {
	// UTF-8: above 0x80 need to check the value
	ਜੇ (enc_utf8)
	    ਵਾਪਸ utf_char2cells(c);
	// DBCS: double-byte means double-width, except for euc-jp with first
	// byte 0x8e
	ਜੇ (enc_dbcs != 0 && c >= 0x100)
	{
	    ਜੇ (enc_dbcs == DBCS_JPNU && ((ਬਿਨਾਨਿਸ਼ਾਨੀ)c >> 8) == 0x8e)
		ਵਾਪਸ 1;
	    ਵਾਪਸ 2;
	}
    }
    ਵਾਪਸ (g_chartab[c & 0xff] & CT_CELL_ਨਕਾਬ);
}

/*
 * Return number of display cells occupied by character at "*p".
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    ਪੂਰਨ
ptr2cells(char_u *p)
{
    ਜੇ (!has_mbyte)
	ਵਾਪਸ byte2cells(*p);

    // For UTF-8 we need to look at more bytes if the first byte is >= 0x80.
    ਜੇ (enc_utf8 && *p >= 0x80)
	ਵਾਪਸ utf_ptr2cells(p);
    // For DBCS we can tell the cell count from the first byte.
    ਵਾਪਸ (g_chartab[*p] & CT_CELL_ਨਕਾਬ);
}

/*
 * Return the number of character cells string "s" will take on the screen,
 * counting TABs as two characters: "^I".
 */
    ਪੂਰਨ
vim_strsize(char_u *s)
{
    ਵਾਪਸ vim_strnsize(s, (ਪੂਰਨ)MAXCOL);
}

/*
 * Return the number of character cells string "s[len]" will take on the
 * screen, counting TABs as two characters: "^I".
 */
    ਪੂਰਨ
vim_strnsize(char_u *s, ਪੂਰਨ len)
{
    ਪੂਰਨ		ਮਾਪ = 0;

    ਜਦੋਂ (*s != NUL && --len >= 0)
    {
	ਪੂਰਨ	    l = (*mb_ptr2len)(s);

	ਮਾਪ += ptr2cells(s);
	s += l;
	len -= l - 1;
    }

    ਵਾਪਸ ਮਾਪ;
}

/*
 * Return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab.
 * Use a define to make it fast, this is used very often!!!
 * Also see getvcol() below.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, ਖਾਨਾ) \
    ਜੇ (*(p) == TAB && (!(wp)->w_p_list || (wp)->w_lcs_chars.tab1)) \
    { \
	ਵਾਪਸ tabstop_padding(ਖਾਨਾ, (buf)->b_p_ts, (buf)->b_p_vts_array); \
    } \
    ਨਹੀਂਤਾਂ \
	ਵਾਪਸ ptr2cells(p);
#ਨਹੀਂਤਾਂ
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, ਖਾਨਾ) \
    ਜੇ (*(p) == TAB && (!(wp)->w_p_list || wp->w_lcs_chars.tab1)) \
    { \
	ਪੂਰਨ ts; \
	ts = (buf)->b_p_ts; \
	ਵਾਪਸ (ਪੂਰਨ)(ts - (ਖਾਨਾ % ts)); \
    } \
    ਨਹੀਂਤਾਂ \
	ਵਾਪਸ ptr2cells(p);
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
chartabsize(char_u *p, colnr_T ਖਾਨਾ)
{
    RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, p, ਖਾਨਾ)
}

#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ਪੂਰਨ
win_chartabsize(win_T *wp, char_u *p, colnr_T ਖਾਨਾ)
{
    RET_WIN_BUF_CHARTABSIZE(wp, wp->w_ਬਫਰ, p, ਖਾਨਾ)
}
#ਜੇ_ਅੰਤ

/*
 * Return the number of characters the string "s" will take on the screen,
 * taking into account the size of a tab.
 * Does not handle text properties, since "s" is not a buffer line.
 */
    ਪੂਰਨ
linetabsize_str(char_u *s)
{
    ਵਾਪਸ linetabsize_ਖਾਨਾ(0, s);
}

/*
 * Like linetabsize_str(), but "s" starts at column "startcol".
 */
    ਪੂਰਨ
linetabsize_ਖਾਨਾ(ਪੂਰਨ startcol, char_u *s)
{
    chartabsize_T cts;
    vimlong_T vcol;

    init_chartabsize_arg(&cts, curwin, 0, startcol, s, s);
    vcol = cts.cts_vcol;

    ਜਦੋਂ (*cts.cts_ptr != NUL)
    {
	vcol += lbr_chartabsize_adv(&cts);
	ਜੇ (vcol > MAXCOL)
	{
	    cts.cts_vcol = MAXCOL;
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	    cts.cts_vcol = (ਪੂਰਨ)vcol;
    }
    clear_chartabsize_arg(&cts);
    ਵਾਪਸ (ਪੂਰਨ)cts.cts_vcol;
}

/*
 * Like linetabsize_str(), but for a given window instead of the current one.
 * Doesn't count the size of 'listchars' "eol".
 */
    ਪੂਰਨ
win_linetabsize(win_T *wp, linenr_T lnum, char_u *ਸਤਰ, colnr_T len)
{
    chartabsize_T cts;

    init_chartabsize_arg(&cts, wp, lnum, 0, ਸਤਰ, ਸਤਰ);
    win_linetabsize_cts(&cts, len);
    clear_chartabsize_arg(&cts);
    ਵਾਪਸ (ਪੂਰਨ)cts.cts_vcol;
}

/*
 * Return the number of cells line "lnum" of window "wp" will take on the
 * screen, taking into account the size of a tab and text properties.
 * Doesn't count the size of 'listchars' "eol".
 */
    ਪੂਰਨ
linetabsize(win_T *wp, linenr_T lnum)
{
    ਵਾਪਸ win_linetabsize(wp, lnum,
		       ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE), (colnr_T)MAXCOL);
}

/*
 * Like linetabsize(), but counts the size of 'listchars' "eol".
 */
    ਪੂਰਨ
linetabsize_eol(win_T *wp, linenr_T lnum)
{
    ਵਾਪਸ linetabsize(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Like linetabsize(), but excludes 'above'/'after'/'right'/'below' aligned
 * virtual text, while keeping inline virtual text.
 */
    ਪੂਰਨ
linetabsize_no_outer(win_T *wp, linenr_T lnum)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਵਾਪਸ linetabsize(wp, lnum);
#ਨਹੀਂਤਾਂ
    chartabsize_T cts;
    char_u *ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, ਸਤਰ, ਸਤਰ);

    ਜੇ (cts.cts_ਪਾਠ_prop_ਗਿਣਤੀ)
    {
	ਪੂਰਨ write_idx = 0;
	ਲਈ (ਪੂਰਨ read_idx = 0; read_idx < cts.cts_ਪਾਠ_prop_ਗਿਣਤੀ; read_idx++)
	{
	    textprop_T *tp = &cts.cts_ਪਾਠ_props[read_idx];
	    ਜੇ (tp->tp_ਖਾਨਾ != MAXCOL)
	    {
		ਜੇ (read_idx != write_idx)
		    cts.cts_ਪਾਠ_props[write_idx] = *tp;
		write_idx++;
	    }
	}
	cts.cts_ਪਾਠ_prop_ਗਿਣਤੀ = write_idx;
	ਜੇ (cts.cts_ਪਾਠ_prop_ਗਿਣਤੀ == 0)
	    VIM_CLEAR(cts.cts_ਪਾਠ_props);
    }

    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    ਵਾਪਸ (ਪੂਰਨ)cts.cts_vcol;
#ਜੇ_ਅੰਤ
}

/*
 * Like linetabsize_no_outer(), but counts the size of 'listchars' "eol".
 */
    ਪੂਰਨ
linetabsize_no_outer_eol(win_T *wp, linenr_T lnum)
{
    ਵਾਪਸ linetabsize_no_outer(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Return TRUE when win_lbr_chartabsize() does nothing more than
 * win_nolbr_chartabsize(): no 'linebreak', 'breakindent', 'showbreak' and no
 * text properties that insert text.
 */
#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ਸਥਿਰ ਪੂਰਨ
win_lbr_is_simple_ਚੌੜਾਈ(chartabsize_T *cts)
{
    ਵਾਪਸ TRUE
# ifdef FEAT_LINEBREAK
	&& !cts->cts_win->w_p_lbr && !cts->cts_win->w_p_bri
	&& *get_showbreak_ਮੁੱਲ(cts->cts_win) == NUL
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_ਪਾਠ
# endif
	;
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
win_linetabsize_cts(chartabsize_T *cts, colnr_T len)
{
    vimlong_T vcol = cts->cts_vcol;
#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    // Fast path: when win_lbr_chartabsize() would just return
    // win_nolbr_chartabsize() (wrap on; no 'linebreak'/'breakindent'/
    // 'showbreak'/inserted text properties), call it directly.
    ਪੂਰਨ simple_ਚੌੜਾਈ = cts->cts_win->w_p_wrap && win_lbr_is_simple_ਚੌੜਾਈ(cts);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    cts->cts_with_trailing = len == MAXCOL;
#ਜੇ_ਅੰਤ
    ਲਈ ( ; *cts->cts_ptr != NUL && (len == MAXCOL || cts->cts_ptr < cts->cts_ਸਤਰ + len);
						      MB_PTR_ADV(cts->cts_ptr))
    {
#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
	ਜੇ (simple_ਚੌੜਾਈ)
	{
	    ਪੂਰਨ	c = *cts->cts_ptr;

	    ਜੇ (c < 0x80 && c != TAB)
		vcol += g_chartab[c] & CT_CELL_ਨਕਾਬ;
	    ਨਹੀਂਤਾਂ
		vcol += win_nolbr_chartabsize(cts, ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    vcol += win_lbr_chartabsize(cts, ਨਿਰਾ, ਨਿਰਾ);
	ਜੇ (vcol > MAXCOL)
	{
	    cts->cts_vcol = MAXCOL;
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	    cts->cts_vcol = (ਪੂਰਨ)vcol;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // check for a virtual text at the end of a line or on an empty line
    ਜੇ (len == MAXCOL && cts->cts_has_prop_with_ਪਾਠ && *cts->cts_ptr == NUL)
    {
	ਪੂਰਨ ਸਿਰ = 0;
	(ਖਾਲੀ)win_lbr_chartabsize(cts, &ਸਿਰ, ਨਿਰਾ);
	vcol += cts->cts_cur_ਪਾਠ_ਚੌੜਾਈ + ਸਿਰ;
	// When properties are above the empty line must also be counted.  For
	// a property below the width already includes filling up the line.
	ਜੇ (cts->cts_ptr == cts->cts_ਸਤਰ && cts->cts_prop_ਸਤਰਾਂ > 0
							 && !cts->cts_has_ਹੇਠਾਂ)
	    ++vcol;
	cts->cts_vcol = vcol > MAXCOL ? MAXCOL : (ਪੂਰਨ)vcol;
    }
#ਜੇ_ਅੰਤ
}

/*
 * Return TRUE if 'c' is a normal identifier character:
 * Letters and characters from the 'isident' option.
 */
    ਪੂਰਨ
vim_isIDc(ਪੂਰਨ c)
{
    ਵਾਪਸ (c > 0 && c < 0x100 && (g_chartab[c] & CT_ID_CHAR));
}

/*
 * Like vim_isIDc() but not using the 'isident' option: letters, numbers and
 * underscore.
 */
    ਪੂਰਨ
vim_is_ਸਾਧਾਰਨ_IDc(ਪੂਰਨ c)
{
    ਵਾਪਸ ASCII_ISALNUM(c) || c == '_';
}

/*
 * return TRUE if 'c' is a keyword character: Letters and characters from
 * 'iskeyword' option for the current buffer.
 * For multi-byte characters mb_get_class() is used (builtin rules).
 */
    ਪੂਰਨ
vim_iswordc(ਪੂਰਨ c)
{
    ਵਾਪਸ vim_iswordc_buf(c, curbuf);
}

    ਪੂਰਨ
vim_iswordc_buf(ਪੂਰਨ c, buf_T *buf)
{
    ਜੇ (c >= 0x100)
    {
	ਜੇ (enc_dbcs != 0)
	    ਵਾਪਸ dbcs_class((ਬਿਨਾਨਿਸ਼ਾਨੀ)c >> 8, (ਬਿਨਾਨਿਸ਼ਾਨੀ)(c & 0xff)) >= 2;
	ਜੇ (enc_utf8)
	    ਵਾਪਸ utf_class_buf(c, buf) >= 2;
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ (c > 0 && GET_CHARTAB(buf, c) != 0);
}

/*
 * Just like vim_iswordc() but uses a pointer to the (multi-byte) character.
 */
    ਪੂਰਨ
vim_iswordp(char_u *p)
{
    ਵਾਪਸ vim_iswordp_buf(p, curbuf);
}

    ਪੂਰਨ
vim_iswordp_buf(char_u *p, buf_T *buf)
{
    ਪੂਰਨ	c = *p;

    ਜੇ (has_mbyte && MB_BYTE2LEN(c) > 1)
	c = (*mb_ptr2char)(p);
    ਵਾਪਸ vim_iswordc_buf(c, buf);
}

/*
 * Return TRUE if 'c' is a valid file-name character as specified with the
 * 'isfname' option.
 * Assume characters above 0x100 are valid (multi-byte).
 * To be used for commands like "gf".
 */
    ਪੂਰਨ
vim_isfilec(ਪੂਰਨ c)
{
    ਵਾਪਸ (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_FNAME_CHAR)));
}

#ਜੇ defined(FEAT_SPELL)
/*
 * Return TRUE if 'c' is a valid file-name character, including characters left
 * out of 'isfname' to make "gf" work, such as comma, space, '@', etc.
 */
    ਪੂਰਨ
vim_is_fname_char(ਪੂਰਨ c)
{
    ਵਾਪਸ vim_isfilec(c) || c == ',' || c == ' ' || c == '@';
}
#ਜੇ_ਅੰਤ

/*
 * return TRUE if 'c' is a valid file-name character or a wildcard character
 * Assume characters above 0x100 are valid (multi-byte).
 * Explicitly interpret ']' as a wildcard character as mch_has_wildcard("]")
 * returns false.
 */
    ਪੂਰਨ
vim_isfilec_or_wc(ਪੂਰਨ c)
{
    char_u buf[2];

    buf[0] = (char_u)c;
    buf[1] = NUL;
    ਵਾਪਸ vim_isfilec(c) || c == ']' || mch_has_wildcard(buf);
}

/*
 * Return TRUE if 'c' is a printable character.
 * Assume characters above 0x100 are printable (multi-byte), except for
 * Unicode.
 */
    ਪੂਰਨ
vim_isprintc(ਪੂਰਨ c)
{
    ਜੇ (enc_utf8 && c >= 0x100)
	ਵਾਪਸ utf_printable(c);
    ਵਾਪਸ (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Strict version of vim_isprintc(c), don't return TRUE if "c" is the head
 * byte of a double-byte character.
 */
    ਪੂਰਨ
vim_isprintc_strict(ਪੂਰਨ c)
{
    ਜੇ (enc_dbcs != 0 && c < 0x100 && MB_BYTE2LEN(c) > 1)
	ਵਾਪਸ FALSE;
    ਜੇ (enc_utf8 && c >= 0x100)
	ਵਾਪਸ utf_printable(c);
    ਵਾਪਸ (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Prepare the structure passed to chartabsize functions.
 * "line" is the start of the line, "ptr" is the first relevant character.
 * When "lnum" is zero do not use text properties that insert text.
 */
    ਖਾਲੀ
init_chartabsize_arg(
	chartabsize_T	*cts,
	win_T		*wp,
	linenr_T	lnum ਅਣਵਰਤਿਆ,
	colnr_T		ਖਾਨਾ,
	char_u		*ਸਤਰ,
	char_u		*ptr)
{
    CLEAR_ਸੂਚਕ(cts);
    cts->cts_win = wp;
    cts->cts_vcol = ਖਾਨਾ;
    cts->cts_ਸਤਰ = ਸਤਰ;
    cts->cts_ptr = ptr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    cts->cts_bri_ਮਾਪ = -1;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (lnum > 0 && !ignore_ਪਾਠ_props)
    {
	char_u	*prop_ਸ਼ੁਰੂ;
	ਪੂਰਨ	ਗਿਣਤੀ_2;

	ਗਿਣਤੀ_2 = get_ਪਾਠ_props(wp->w_ਬਫਰ, lnum, &prop_ਸ਼ੁਰੂ, FALSE);
	cts->cts_ਪਾਠ_prop_ਗਿਣਤੀ = ਗਿਣਤੀ_2;
	ਜੇ (ਗਿਣਤੀ_2 > 0)
	{
	    // Make a copy of the properties, so that they are properly
	    // aligned.  Make it twice as long for the sorting below.
	    cts->cts_ਪਾਠ_props = ALLOC_MULT(textprop_T, ਗਿਣਤੀ_2 * 2);
	    ਜੇ (cts->cts_ਪਾਠ_props == ਨਿਰਾ)
		cts->cts_ਪਾਠ_prop_ਗਿਣਤੀ = 0;
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ	i;

		mch_memmove(cts->cts_ਪਾਠ_props + ਗਿਣਤੀ_2, prop_ਸ਼ੁਰੂ,
						   ਗਿਣਤੀ_2 * ਆਕਾਰ(textprop_T));
		ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
		{
		    textprop_T *tp = cts->cts_ਪਾਠ_props + i + ਗਿਣਤੀ_2;
		    ਜੇ (tp->tp_id < 0
				     && ਪਾਠ_prop_type_valid(wp->w_ਬਫਰ, tp))
		    {
			cts->cts_has_prop_with_ਪਾਠ = TRUE;
			ਰੋਕੋ;
		    }
		}
		ਜੇ (!cts->cts_has_prop_with_ਪਾਠ)
		{
		    // won't use the text properties, free them
		    VIM_CLEAR(cts->cts_ਪਾਠ_props);
		    cts->cts_ਪਾਠ_prop_ਗਿਣਤੀ = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ	    *ਪਾਠ_prop_idxs;

		    // Need to sort the array to get any truncation right.
		    // Do the sorting in the second part of the array, then
		    // move the sorted props to the first part of the array.
		    ਪਾਠ_prop_idxs = ALLOC_MULT(ਪੂਰਨ, ਗਿਣਤੀ_2);
		    ਜੇ (ਪਾਠ_prop_idxs != ਨਿਰਾ)
		    {
			ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
			    ਪਾਠ_prop_idxs[i] = i + ਗਿਣਤੀ_2;
			ਛਾਂਟੋ_ਪਾਠ_props(curbuf, cts->cts_ਪਾਠ_props,
							ਪਾਠ_prop_idxs, ਗਿਣਤੀ_2);
			// Here we want the reverse order.
			ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
			    cts->cts_ਪਾਠ_props[ਗਿਣਤੀ_2 - i - 1] =
					cts->cts_ਪਾਠ_props[ਪਾਠ_prop_idxs[i]];
			vim_free(ਪਾਠ_prop_idxs);
		    }

		    // Convert tp_text_offset to tp_text pointer.
		    char_u *ਗਿਣਤੀ_ptr = prop_ਸ਼ੁਰੂ - PROP_ਗਿਣਤੀ_ਮਾਪ;

		    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
		    {
			textprop_T *tp = &cts->cts_ਪਾਠ_props[i];

			ਜੇ (tp->tp_id < 0 && tp->u.tp_ਪਾਠ_ਵਿੱਥ > 0)
			{
			    tp->u.tp_ਪਾਠ = ਗਿਣਤੀ_ptr + tp->u.tp_ਪਾਠ_ਵਿੱਥ;
			    tp->tp_flags |= TP_FLAG_VTEXT_PTR;
			}
			ਨਹੀਂਤਾਂ
			    tp->u.tp_ਪਾਠ = ਨਿਰਾ;
		    }
		}
	    }
	}
    }
#ਜੇ_ਅੰਤ
}

/*
 * Free any allocated item in "cts".
 */
    ਖਾਲੀ
clear_chartabsize_arg(chartabsize_T *cts ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (cts->cts_ਪਾਠ_prop_ਗਿਣਤੀ > 0)
    {
	VIM_CLEAR(cts->cts_ਪਾਠ_props);
	cts->cts_ਪਾਠ_prop_ਗਿਣਤੀ = 0;
    }
#ਜੇ_ਅੰਤ
}

/*
 * Like chartabsize(), but also check for line breaks on the screen and text
 * properties that insert text.
 */
    ਪੂਰਨ
lbr_chartabsize(chartabsize_T *cts)
{
#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ਜੇ (1
# ifdef FEAT_LINEBREAK
	&& !curwin->w_p_lbr && *get_showbreak_ਮੁੱਲ(curwin) == NUL
							   && !curwin->w_p_bri
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_ਪਾਠ
# endif
       )
    {
#ਜੇ_ਅੰਤ
	ਜੇ (curwin->w_p_wrap)
	    ਵਾਪਸ win_nolbr_chartabsize(cts, ਨਿਰਾ);
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, cts->cts_ptr, cts->cts_vcol)
#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    }
    ਵਾਪਸ win_lbr_chartabsize(cts, ਨਿਰਾ, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
}

/*
 * Call lbr_chartabsize() and advance the pointer.
 */
    ਪੂਰਨ
lbr_chartabsize_adv(chartabsize_T *cts)
{
    ਪੂਰਨ		retval;

    retval = lbr_chartabsize(cts);
    MB_PTR_ADV(cts->cts_ptr);
    ਵਾਪਸ retval;
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
    ਪੂਰਨ
win_lbr_chartabsize(
	chartabsize_T	*cts,
	ਪੂਰਨ		*headp ਅਣਵਰਤਿਆ,
	ਪੂਰਨ		*tailp ਅਣਵਰਤਿਆ)
{
    win_T	*wp = cts->cts_win;
#ਜੇ defined(FEAT_PROP_POPUP) || defined(FEAT_LINEBREAK)
    char_u	*ਸਤਰ = cts->cts_ਸਤਰ; // start of the line
    ਪੂਰਨ		ਮਾਪ;
#ਜੇ_ਅੰਤ
    char_u	*s = cts->cts_ptr;
    colnr_T	vcol = cts->cts_vcol;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਪੂਰਨ		mb_added = 0;
    ਪੂਰਨ		n;
    char_u	*sbr;
    ਪੂਰਨ		no_sbr = FALSE;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PROP_POPUP)
    cts->cts_cur_ਪਾਠ_ਚੌੜਾਈ = 0;
    cts->cts_has_ਹੇਠਾਂ = false;
    cts->cts_ਪਹਿਲਾ_char = 0;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    /*
     * No 'linebreak', 'showbreak', 'breakindent' and text properties that
     * insert text: return quickly.
     */
    ਜੇ (win_lbr_is_simple_ਚੌੜਾਈ(cts))
#ਜੇ_ਅੰਤ
    {
	ਜੇ (wp->w_p_wrap)
	    ਵਾਪਸ win_nolbr_chartabsize(cts, headp);
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_ਬਫਰ, s, vcol)
    }

#ਜੇ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ਪੂਰਨ has_lcs_eol = wp->w_p_list && wp->w_lcs_chars.eol != NUL;
    // Virtual text above the line is on its own screen line, it does not count
    // for the size of a Tab.
    colnr_T tab_vcol = vcol;

# ifdef FEAT_PROP_POPUP
    tab_vcol -= cts->cts_above_ਚੌੜਾਈ;
# endif

    /*
     * First get the normal size, without 'linebreak' or text properties
     */
    ਮਾਪ = win_chartabsize(wp, s, tab_vcol);
# ifdef FEAT_LINEBREAK
    ਜੇ (*s == NUL)
    {
	// 1 cell for EOL list char (if present), as opposed to the two cell ^@
	// for a NUL character in the text.
	ਮਾਪ = has_lcs_eol ? 1 : 0;
    }

    ਪੂਰਨ is_doublewidth = has_mbyte && ਮਾਪ == 2 && MB_BYTE2LEN(*s) > 1;
# endif

# ifdef FEAT_PROP_POPUP
    ਜੇ (cts->cts_has_prop_with_ਪਾਠ)
    {
#  ifdef FEAT_LINEBREAK
	ਪੂਰਨ	    tab_ਮਾਪ = ਮਾਪ;
#  endif
	ਪੂਰਨ	    charlen = *s == NUL ? 1 : mb_ptr2len(s);
	ਪੂਰਨ	    i;
	ਪੂਰਨ	    ਖਾਨਾ = (ਪੂਰਨ)(s - ਸਤਰ);


	// The "$" for 'list' mode will go between the EOL and
	// the text prop, account for that.
	ਜੇ (has_lcs_eol)
	{
	    ++vcol;
	    --ਮਾਪ;
	}

	ਲਈ (i = 0; i < cts->cts_ਪਾਠ_prop_ਗਿਣਤੀ; ++i)
	{
	    textprop_T	*tp = cts->cts_ਪਾਠ_props + i;
	    ਪੂਰਨ		ਖਾਨਾ_off = win_ਖਾਨਾ_off(wp);

	    // Watch out for the text being deleted.  "cts_text_props" is a
	    // copy, the text prop may actually have been removed from the line.
	    ਜੇ (tp->tp_id < 0
		    && ((tp->tp_ਖਾਨਾ - 1 >= ਖਾਨਾ
					     && tp->tp_ਖਾਨਾ - 1 < ਖਾਨਾ + charlen)
		       || (tp->tp_ਖਾਨਾ == MAXCOL
			   && ((tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
				? ਖਾਨਾ == 0
				: s[0] == NUL && cts->cts_with_trailing)))
		    && tp->u.tp_ਪਾਠ != ਨਿਰਾ)
	    {
		char_u *p = tp->u.tp_ਪਾਠ;

		ਜੇ (p != ਨਿਰਾ)
		{
		    ਪੂਰਨ	cells;

		    ਜੇ (tp->tp_ਖਾਨਾ == MAXCOL)
		    {
			ਪੂਰਨ n_extra = (ਪੂਰਨ)STRLEN(p);

			cells = ਪਾਠ_prop_position(wp, tp, vcol,
			     (vcol + ਮਾਪ) % (wp->w_ਚੌੜਾਈ - ਖਾਨਾ_off) + ਖਾਨਾ_off,
					      &n_extra, &p, ਨਿਰਾ, ਨਿਰਾ, FALSE);
#  ifdef FEAT_LINEBREAK
			ਜੇ (ਪਾਠ_prop_no_showbreak(tp))
			    no_sbr = TRUE;  // don't use 'showbreak' now
#  endif
		    }
		    ਨਹੀਂਤਾਂ
			cells = vim_strsize(p);
		    ਜੇ (tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
		    {
			cts->cts_ਪਹਿਲਾ_char += cells;
			ਜੇ (!cts->cts_no_above)
			    cts->cts_cur_ਪਾਠ_ਚੌੜਾਈ += cells;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			cts->cts_cur_ਪਾਠ_ਚੌੜਾਈ += cells;
			ਮਾਪ += cells;
		    }
		    cts->cts_ਸ਼ੁਰੂ_incl = tp->tp_flags & TP_FLAG_ਸ਼ੁਰੂ_INCL;
#  ifdef FEAT_LINEBREAK
		    ਜੇ (*s == TAB)
		    {
			// tab size changes because of the inserted text
			ਮਾਪ -= tab_ਮਾਪ;
			tab_ਮਾਪ = win_chartabsize(wp, s,
					vcol + ਮਾਪ - cts->cts_above_ਚੌੜਾਈ);
			ਮਾਪ += tab_ਮਾਪ;
		    }
#  endif
		    ਜੇ (tp->tp_ਖਾਨਾ == MAXCOL && (tp->tp_flags
				& (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_ਹੇਠਾਂ)))
		    {
			// count extra line for property above/below
			++cts->cts_prop_ਸਤਰਾਂ;
			ਜੇ (tp->tp_flags & TP_FLAG_ALIGN_ਹੇਠਾਂ)
			    cts->cts_has_ਹੇਠਾਂ = true;
		    }
		}
	    }
	    ਜੇ (tp->tp_ਖਾਨਾ != MAXCOL && tp->tp_ਖਾਨਾ - 1 > ਖਾਨਾ)
		ਰੋਕੋ;
	}
	ਜੇ (has_lcs_eol)
	{
	    --vcol;
	    ++ਮਾਪ;
	}
    }
# endif

# ifdef FEAT_LINEBREAK
    ਜੇ (is_doublewidth && wp->w_p_wrap && ਵਿੱਚ_win_border(wp, vcol + ਮਾਪ - 2))
    {
	++ਮਾਪ;		// Count the ">" in the last column.
	mb_added = 1;
    }

    /*
     * May have to add something for 'breakindent' and/or 'showbreak'
     * string at the start of a screen line.
     */
    ਪੂਰਨ ਸਿਰ = mb_added;
    sbr = no_sbr ? ਖਾਲੀ_ਥਾਵਾਂ_option : get_showbreak_ਮੁੱਲ(wp);
    // When "size" is 0, no new screen line is started.
    ਜੇ (ਮਾਪ > 0 && wp->w_p_wrap && (*sbr != NUL || wp->w_p_bri))
    {
	ਪੂਰਨ	ਖਾਨਾ_off_prev = win_ਖਾਨਾ_off(wp);
	ਪੂਰਨ	width2 = wp->w_ਚੌੜਾਈ - ਖਾਨਾ_off_prev + win_ਖਾਨਾ_off2(wp);
	colnr_T	wcol = vcol + ਖਾਨਾ_off_prev;
#  ifdef FEAT_PROP_POPUP
	wcol -= wp->w_virtcol_ਪਹਿਲਾ_char;
#  endif
	colnr_T	max_ਸਿਰ_vcol = cts->cts_max_ਸਿਰ_vcol;
	ਪੂਰਨ	added = 0;

	// cells taken by 'showbreak'/'breakindent' before current char
	ਪੂਰਨ	ਸਿਰ_prev = 0;
	ਜੇ (wcol >= wp->w_ਚੌੜਾਈ)
	{
	    wcol -= wp->w_ਚੌੜਾਈ;
	    ਖਾਨਾ_off_prev = wp->w_ਚੌੜਾਈ - width2;
	    ਜੇ (wcol >= width2 && width2 > 0)
		wcol %= width2;
	    ਜੇ (*sbr != NUL)
		ਸਿਰ_prev += vim_strsize(sbr);
	    ਜੇ (wp->w_p_bri)
	    {
		ਜੇ (cts->cts_bri_ਮਾਪ < 0)
		    cts->cts_bri_ਮਾਪ = get_breakindent_win(wp, ਸਤਰ);
		ਸਿਰ_prev += cts->cts_bri_ਮਾਪ;
	    }
	    ਜੇ (wcol < ਸਿਰ_prev)
	    {
		ਸਿਰ_prev -= wcol;
		wcol += ਸਿਰ_prev;
		added += ਸਿਰ_prev;
		ਜੇ (max_ਸਿਰ_vcol <= 0 || vcol < max_ਸਿਰ_vcol)
		    ਸਿਰ += ਸਿਰ_prev;
	    }
	    ਨਹੀਂਤਾਂ
		ਸਿਰ_prev = 0;
	    wcol += ਖਾਨਾ_off_prev;
	}

	ਜੇ (wcol + ਮਾਪ > wp->w_ਚੌੜਾਈ)
	{
	    // cells taken by 'showbreak'/'breakindent' halfway current char
	    ਪੂਰਨ	ਸਿਰ_mid = 0;
	    ਜੇ (*sbr != NUL)
		ਸਿਰ_mid += vim_strsize(sbr);
	    ਜੇ (wp->w_p_bri)
	    {
		ਜੇ (cts->cts_bri_ਮਾਪ < 0)
		    cts->cts_bri_ਮਾਪ = get_breakindent_win(wp, ਸਤਰ);
		ਸਿਰ_mid += cts->cts_bri_ਮਾਪ;
	    }
	    ਜੇ (ਸਿਰ_mid > 0)
	    {
		// Calculate effective window width.
		ਪੂਰਨ prev_rem = wp->w_ਚੌੜਾਈ - wcol;
		ਪੂਰਨ ਚੌੜਾਈ = width2 - ਸਿਰ_mid;

		ਜੇ (ਚੌੜਾਈ <= 0)
		    ਚੌੜਾਈ = 1;
		// Divide "size - prev_rem" by "width", rounding up.
		ਪੂਰਨ cnt = (ਮਾਪ - prev_rem + ਚੌੜਾਈ - 1) / ਚੌੜਾਈ;
		added += cnt * ਸਿਰ_mid;

		ਜੇ (max_ਸਿਰ_vcol == 0 || vcol + ਮਾਪ + added < max_ਸਿਰ_vcol)
		    ਸਿਰ += cnt * ਸਿਰ_mid;
		ਨਹੀਂਤਾਂ ਜੇ (width2 > 0
			   && max_ਸਿਰ_vcol > vcol + ਸਿਰ_prev + prev_rem)
		    ਸਿਰ += (max_ਸਿਰ_vcol - (vcol + ਸਿਰ_prev + prev_rem)
					     + width2 - 1) / width2 * ਸਿਰ_mid;
		ਨਹੀਂਤਾਂ ਜੇ (max_ਸਿਰ_vcol < 0)
		{
		    ਪੂਰਨ off = mb_added;
#  ifdef FEAT_PROP_POPUP
		    ਜੇ (*s != NUL
			     && ((State & ਬਹੁਲਕ_ਸਾਧਾਰਨ) || cts->cts_ਸ਼ੁਰੂ_incl))
			off += cts->cts_cur_ਪਾਠ_ਚੌੜਾਈ;
#  endif
		    ਜੇ (off >= prev_rem)
			ਸਿਰ += (1 + (off - prev_rem) / ਚੌੜਾਈ) * ਸਿਰ_mid;
		}
	    }
	}

	ਮਾਪ += added;
    }

    ਜੇ (headp != ਨਿਰਾ)
	*headp = ਸਿਰ;

    ਪੂਰਨ ਮਾਪ_before_lbr = ਮਾਪ;
    ਪੂਰਨ need_lbr = FALSE;
    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here.
     */
    ਜੇ (wp->w_p_lbr && wp->w_p_wrap && wp->w_ਚੌੜਾਈ != 0
	    && VIM_ISBREAK((ਪੂਰਨ)s[0]) && !VIM_ISBREAK((ਪੂਰਨ)s[1]))
    {
	char_u	*t = cts->cts_ਸਤਰ;
	ਜਦੋਂ (VIM_ISBREAK((ਪੂਰਨ)t[0]))
	    t++;
	// 'linebreak' is only needed when not in leading whitespace.
	need_lbr = s >= t;
    }
    ਜੇ (need_lbr)
    {
	/*
	 * Count all characters from first non-blank after a blank up to next
	 * non-blank after a blank.
	 */
	ਪੂਰਨ numberextra = win_ਖਾਨਾ_off(wp);
	colnr_T ਖਾਨਾ_adj = ਮਾਪ - 1;
	colnr_T colmax = (colnr_T)(wp->w_ਚੌੜਾਈ - numberextra - ਖਾਨਾ_adj);
	ਜੇ (vcol >= colmax)
	{
	    colmax += ਖਾਨਾ_adj;
	    n = colmax +  win_ਖਾਨਾ_off2(wp);
	    ਜੇ (n > 0)
		colmax += (((vcol + ਖਾਨਾ_adj - colmax) / n) + 1) * n - ਖਾਨਾ_adj;
	}

	colnr_T vcol2 = vcol;
	ਲਈ (;;)
	{
	    char_u *ps = s;
	    MB_PTR_ADV(s);
	    ਪੂਰਨ c = *s;
	    ਜੇ (!(c != NUL
		    && (VIM_ISBREAK(c)
			|| (!VIM_ISBREAK(c)
			       && (vcol2 == vcol || !VIM_ISBREAK((ਪੂਰਨ)*ps))))))
		ਰੋਕੋ;

	    vcol2 += win_chartabsize(wp, s, vcol2);
	    ਜੇ (vcol2 >= colmax)		// doesn't fit
	    {
		ਮਾਪ = colmax - vcol + ਖਾਨਾ_adj;
		ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (tailp != ਨਿਰਾ)
	*tailp = ਮਾਪ - ਮਾਪ_before_lbr;

#  ifdef FEAT_PROP_POPUP
    ਜੇ (cts->cts_ਪਹਿਲਾ_char > 0 && !cts->cts_no_above)
    {
	// Remember the width for the size of a Tab later in the line.  Use
	// assignment, this may be called more than once for a character.
	cts->cts_above_ਚੌੜਾਈ = cts->cts_ਪਹਿਲਾ_char;
	ਮਾਪ += cts->cts_ਪਹਿਲਾ_char;
    }
#  endif
# endif
    ਵਾਪਸ ਮਾਪ;
#ਜੇ_ਅੰਤ
}

/*
 * Like win_lbr_chartabsize(), except that we know 'linebreak' is off, 'wrap'
 * is on and there are no properties that insert text.  This means we need to
 * check for a double-byte character that doesn't fit at the end of the screen
 * line.
 * Only uses "cts_win", "cts_ptr" and "cts_vcol" from "cts".
 */
    ਸਥਿਰ ਪੂਰਨ
win_nolbr_chartabsize(
	chartabsize_T	*cts,
	ਪੂਰਨ		*headp)
{
    win_T	*wp = cts->cts_win;
    char_u	*s = cts->cts_ptr;
    colnr_T	ਖਾਨਾ = cts->cts_vcol;
    ਪੂਰਨ		n;

    ਜੇ (*s == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	ਵਾਪਸ tabstop_padding(ਖਾਨਾ, wp->w_ਬਫਰ->b_p_ts,
				    wp->w_ਬਫਰ->b_p_vts_array);
#ਨਹੀਂਤਾਂ
	n = wp->w_ਬਫਰ->b_p_ts;
	ਵਾਪਸ (ਪੂਰਨ)(n - (ਖਾਨਾ % n));
#ਜੇ_ਅੰਤ
    }
    n = ptr2cells(s);
    // Add one cell for a double-width character in the last column of the
    // window, displayed with a ">".
    ਜੇ (n == 2 && MB_BYTE2LEN(*s) > 1 && ਵਿੱਚ_win_border(wp, ਖਾਨਾ))
    {
	ਜੇ (headp != ਨਿਰਾ)
	    *headp = 1;
	ਵਾਪਸ 3;
    }
    ਵਾਪਸ n;
}

/*
 * Return TRUE if virtual column "vcol" is in the rightmost column of window
 * "wp".
 */
    ਸਥਿਰ ਪੂਰਨ
ਵਿੱਚ_win_border(win_T *wp, colnr_T vcol)
{
    ਪੂਰਨ		width1;		// width of first line (after line number)
    ਪੂਰਨ		width2;		// width of further lines

    ਜੇ (wp->w_ਚੌੜਾਈ == 0)	// there is no border
	ਵਾਪਸ FALSE;
    width1 = wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp);
    ਜੇ ((ਪੂਰਨ)vcol < width1 - 1)
	ਵਾਪਸ FALSE;
    ਜੇ ((ਪੂਰਨ)vcol == width1 - 1)
	ਵਾਪਸ TRUE;
    width2 = width1 + win_ਖਾਨਾ_off2(wp);
    ਜੇ (width2 <= 0)
	ਵਾਪਸ FALSE;
    ਵਾਪਸ ((vcol - width1) % width2 == width2 - 1);
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
    ਖਾਲੀ
getvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*ਸ਼ੁਰੂ,
    colnr_T	*cursor,
    colnr_T	*ਸਿਰਾ,
    ਪੂਰਨ		flags)
{
    colnr_T	vcol;
    char_u	*ptr;		// points to current char
    char_u	*ਸਤਰ;		// start of the line
    ਪੂਰਨ		incr;
    ਪੂਰਨ		ਸਿਰ;
    ਪੂਰਨ		ਪੂਛ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਪੂਰਨ		*vts = wp->w_ਬਫਰ->b_p_vts_array;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ts = wp->w_ਬਫਰ->b_p_ts;
    ਪੂਰਨ		c;
    chartabsize_T cts;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਪੂਰਨ		on_NUL = FALSE;
#ਜੇ_ਅੰਤ

    vcol = 0;
    ਸਤਰ = ptr = ml_get_buf(wp->w_ਬਫਰ, pos->lnum, FALSE);

    init_chartabsize_arg(&cts, wp, pos->lnum, 0, ਸਤਰ, ਸਤਰ);
    cts.cts_max_ਸਿਰ_vcol = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    cts.cts_no_above = true;
#ਜੇ_ਅੰਤ

    /*
     * This function is used very often, do some speed optimizations.
     * When 'list', 'linebreak', 'showbreak' and 'breakindent' are not set
     * and there are no text properties with "text" use a simple loop.
     * Also use this when 'list' is set but tabs take their normal size.
     */
    ਜੇ ((!wp->w_p_list || wp->w_lcs_chars.tab1 != NUL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	    && !wp->w_p_lbr && *get_showbreak_ਮੁੱਲ(wp) == NUL && !wp->w_p_bri
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_ਪਾਠ
#ਜੇ_ਅੰਤ
       )
    {
	ਲਈ (;;)
	{
	    ਸਿਰ = 0;
	    c = *ptr;
	    // make sure we don't go past the end of the line
	    ਜੇ (c == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
		ਰੋਕੋ;
	    }
	    // A tab gets expanded, depending on the current column
	    ਜੇ (c == TAB)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		incr = tabstop_padding(vcol, ts, vts);
#ਨਹੀਂਤਾਂ
		incr = ts - (vcol % ts);
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (has_mbyte)
		{
		    // For utf-8, if the byte is >= 0x80, need to look at
		    // further bytes to find the cell width.
		    ਜੇ (enc_utf8 && c >= 0x80)
			incr = utf_ptr2cells(ptr);
		    ਨਹੀਂਤਾਂ
			incr = g_chartab[c] & CT_CELL_ਨਕਾਬ;

		    // If a double-cell char doesn't fit at the end of a line
		    // it wraps to the next line, it's like this char is three
		    // cells wide.
		    ਜੇ (incr == 2 && wp->w_p_wrap && MB_BYTE2LEN(*ptr) > 1
			    && ਵਿੱਚ_win_border(wp, vcol))
		    {
			++incr;
			ਸਿਰ = 1;
		    }
		}
		ਨਹੀਂਤਾਂ
		    incr = g_chartab[c] & CT_CELL_ਨਕਾਬ;
	    }

	    char_u *ਅਗਲਾ_ptr = ptr + (*mb_ptr2len)(ptr);
	    ਜੇ (ਅਗਲਾ_ptr - ਸਤਰ > pos->ਖਾਨਾ) // character at pos->col
		ਰੋਕੋ;

	    vcol += incr;
	    ptr = ਅਗਲਾ_ptr;
	}

	ਪੂਛ = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (;;)
	{
	    // A tab gets expanded, depending on the current column.
	    // Other things also take up space.
	    ਸਿਰ = 0;
	    ਪੂਛ = 0;
	    incr = win_lbr_chartabsize(&cts, &ਸਿਰ, &ਪੂਛ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    ਜੇ (cts.cts_ptr == cts.cts_ਸਤਰ)
	    {
		ਜੇ (flags & GETVCOL_FOR_VIRTCOL)
		    // do not count the virtual text above for w_curswant
		    wp->w_virtcol_ਪਹਿਲਾ_char = cts.cts_ਪਹਿਲਾ_char;
		ਜੇ ((flags & GETVCOL_NO_ABOVE) == 0)
		{
		    cts.cts_vcol += cts.cts_ਪਹਿਲਾ_char;
		    cts.cts_above_ਚੌੜਾਈ = cts.cts_ਪਹਿਲਾ_char;
		}
	    }
#ਜੇ_ਅੰਤ
	    // make sure we don't go past the end of the line
	    ਜੇ (*cts.cts_ptr == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		incr += cts.cts_cur_ਪਾਠ_ਚੌੜਾਈ;
		on_NUL = TRUE;
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    }

	    char_u *ਅਗਲਾ_ptr = cts.cts_ptr + (*mb_ptr2len)(cts.cts_ptr);
	    ਜੇ (ਅਗਲਾ_ptr - ਸਤਰ > pos->ਖਾਨਾ) // character at pos->col
		ਰੋਕੋ;

	    cts.cts_vcol += incr;
	    cts.cts_ptr = ਅਗਲਾ_ptr;
	}
	vcol = cts.cts_vcol;
	ptr = cts.cts_ptr;
    }
    clear_chartabsize_arg(&cts);

    ਜੇ (*ptr == NUL && pos->ਖਾਨਾ < MAXCOL && pos->ਖਾਨਾ > ptr - ਸਤਰ)
	pos->ਖਾਨਾ = ptr - ਸਤਰ;

    ਜੇ (ਸ਼ੁਰੂ != ਨਿਰਾ)
	*ਸ਼ੁਰੂ = vcol + ਸਿਰ;
    ਜੇ (ਸਿਰਾ != ਨਿਰਾ)
	*ਸਿਰਾ = vcol + incr - (flags & GETVCOL_ਸਿਰਾ_EXCL_LBR ? ਪੂਛ : 0) - 1;
    ਜੇ (cursor != ਨਿਰਾ)
    {
	ਜੇ (*ptr == TAB
		&& (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ)
		&& !wp->w_p_list
		&& !virtual_ਸਰਗਰਮ()
		&& !(VIsual_ਸਰਗਰਮ
				&& (*p_sel == 'e' || LTOREQ_POS(*pos, VIsual)))
		)
	    // TODO: subtracting "tail" may lead to better cursor position
	    *cursor = vcol + incr - 1;	    // cursor at end
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    // in Insert mode, if "start_incl" is true the text gets inserted
	    // after the virtual text, thus add its width
	    ਜੇ (((State & ਬਹੁਲਕ_ਪਾਓ) == 0 || cts.cts_ਸ਼ੁਰੂ_incl) && !on_NUL)
		// cursor is after inserted text, unless on the NUL
		vcol += cts.cts_cur_ਪਾਠ_ਚੌੜਾਈ;
#ਜੇ_ਅੰਤ
	    *cursor = vcol + ਸਿਰ;	    // cursor at start
	}
    }
}

/*
 * Get virtual cursor column in the current window, pretending 'list' is off.
 */
    colnr_T
getvcol_nolist(pos_T *posp)
{
    ਪੂਰਨ		list_save = curwin->w_p_list;
    colnr_T	vcol;

    curwin->w_p_list = FALSE;
    ਜੇ (posp->coladd)
	getvvcol(curwin, posp, ਨਿਰਾ, &vcol, ਨਿਰਾ, 0);
    ਨਹੀਂਤਾਂ
	getvcol(curwin, posp, ਨਿਰਾ, &vcol, ਨਿਰਾ, 0);
    curwin->w_p_list = list_save;
    ਵਾਪਸ vcol;
}

/*
 * Get virtual column in virtual mode.
 */
    ਖਾਲੀ
getvvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*ਸ਼ੁਰੂ,
    colnr_T	*cursor,
    colnr_T	*ਸਿਰਾ,
    ਪੂਰਨ		flags)
{
    colnr_T	ਖਾਨਾ;
    colnr_T	coladd;
    colnr_T	endadd;
    char_u	*ptr;

    ਜੇ (virtual_ਸਰਗਰਮ())
    {
	// For virtual mode, only want one value
	getvcol(wp, pos, &ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, flags);

	coladd = pos->coladd;
	endadd = 0;
	// Cannot put the cursor on part of a wide character.
	ptr = ml_get_buf(wp->w_ਬਫਰ, pos->lnum, FALSE);
	ਜੇ (pos->ਖਾਨਾ < ml_get_buf_len(wp->w_ਬਫਰ, pos->lnum))
	{
	    ਪੂਰਨ c = (*mb_ptr2char)(ptr + pos->ਖਾਨਾ);

	    ਜੇ (c != TAB && vim_isprintc(c))
	    {
		endadd = (colnr_T)(char2cells(c) - 1);
		ਜੇ (coladd > endadd)	// past end of line
		    endadd = 0;
		ਨਹੀਂਤਾਂ
		    coladd = 0;
	    }
	}
	ਖਾਨਾ += coladd;
	ਜੇ (ਸ਼ੁਰੂ != ਨਿਰਾ)
	    *ਸ਼ੁਰੂ = ਖਾਨਾ;
	ਜੇ (cursor != ਨਿਰਾ)
	    *cursor = ਖਾਨਾ;
	ਜੇ (ਸਿਰਾ != ਨਿਰਾ)
	    *ਸਿਰਾ = ਖਾਨਾ + endadd;
    }
    ਨਹੀਂਤਾਂ
	getvcol(wp, pos, ਸ਼ੁਰੂ, cursor, ਸਿਰਾ, flags);
}

/*
 * Get the leftmost and rightmost virtual column of pos1 and pos2.
 * Used for Visual block mode.
 */
    ਖਾਲੀ
getvcols(
    win_T	*wp,
    pos_T	*pos1,
    pos_T	*pos2,
    colnr_T	*ਖੱਬਾ,
    colnr_T	*ਸੱਜਾ,
    ਪੂਰਨ		flags)
{
    colnr_T	from1, from2, to1, to2;

    ਜੇ (LT_POSP(pos1, pos2))
    {
	getvvcol(wp, pos1, &from1, ਨਿਰਾ, &to1, flags);
	getvvcol(wp, pos2, &from2, ਨਿਰਾ, &to2, flags);
    }
    ਨਹੀਂਤਾਂ
    {
	getvvcol(wp, pos2, &from1, ਨਿਰਾ, &to1, flags);
	getvvcol(wp, pos1, &from2, ਨਿਰਾ, &to2, flags);
    }
    ਜੇ (from2 < from1)
	*ਖੱਬਾ = from2;
    ਨਹੀਂਤਾਂ
	*ਖੱਬਾ = from1;
    ਜੇ (to2 > to1)
    {
	ਜੇ (*p_sel == 'e' && from2 - 1 >= to1)
	    *ਸੱਜਾ = from2 - 1;
	ਨਹੀਂਤਾਂ
	    *ਸੱਜਾ = to2;
    }
    ਨਹੀਂਤਾਂ
	*ਸੱਜਾ = to1;
}

/*
 * Skip over ' ' and '\t'.
 */
    char_u *
skipwhite(char_u *q)
{
    char_u	*p = q;

    ਜਦੋਂ (VIM_ISWHITE(*p))
	++p;
    ਵਾਪਸ p;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * skip over ' ', '\t' and '\n'.
 */
    char_u *
skipwhite_and_nl(char_u *q)
{
    char_u	*p = q;

    ਜਦੋਂ (VIM_ISWHITE(*p) || *p == NL)
	++p;
    ਵਾਪਸ p;
}
#ਜੇ_ਅੰਤ

/*
 * getwhitecols: return the number of whitespace
 * columns (bytes) at the start of a given line
 */
    ਪੂਰਨ
getwhitecols_curline(ਖਾਲੀ)
{
    ਵਾਪਸ getwhitecols(ml_get_curline());
}

    ਪੂਰਨ
getwhitecols(char_u *p)
{
    ਵਾਪਸ skipwhite(p) - p;
}

/*
 * skip over digits
 */
    char_u *
skipdigits(char_u *q)
{
    char_u	*p = q;

    ਜਦੋਂ (VIM_ISDIGIT(*p))	// skip to next non-digit
	++p;
    ਵਾਪਸ p;
}

#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * skip over binary digits
 */
    char_u *
skipbin(char_u *q)
{
    char_u	*p = q;

    ਜਦੋਂ (vim_isbdigit(*p))	// skip to next non-digit
	++p;
    ਵਾਪਸ p;
}

/*
 * skip over digits and hex characters
 */
    char_u *
skiphex(char_u *q)
{
    char_u	*p = q;

    ਜਦੋਂ (vim_isxdigit(*p))	// skip to next non-digit
	++p;
    ਵਾਪਸ p;
}
#ਜੇ_ਅੰਤ

/*
 * skip to bin digit (or NUL after the string)
 */
    char_u *
skiptobin(char_u *q)
{
    char_u	*p = q;

    ਜਦੋਂ (*p != NUL && !vim_isbdigit(*p))	// skip to next digit
	++p;
    ਵਾਪਸ p;
}

/*
 * skip to digit (or NUL after the string)
 */
    char_u *
skiptodigit(char_u *q)
{
    char_u	*p = q;

    ਜਦੋਂ (*p != NUL && !VIM_ISDIGIT(*p))	// skip to next digit
	++p;
    ਵਾਪਸ p;
}

/*
 * skip to hex character (or NUL after the string)
 */
    char_u *
skiptohex(char_u *q)
{
    char_u	*p = q;

    ਜਦੋਂ (*p != NUL && !vim_isxdigit(*p))	// skip to next digit
	++p;
    ਵਾਪਸ p;
}

/*
 * Variant of isdigit() that can handle characters > 0x100.
 * We don't use isdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 * Use the VIM_ISDIGIT() macro for simple arguments.
 */
    ਪੂਰਨ
vim_isdigit(ਪੂਰਨ c)
{
    ਵਾਪਸ (c >= '0' && c <= '9');
}

/*
 * Variant of isxdigit() that can handle characters > 0x100.
 * We don't use isxdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 */
    ਪੂਰਨ
vim_isxdigit(ਪੂਰਨ c)
{
    ਵਾਪਸ (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'f')
	|| (c >= 'A' && c <= 'F');
}

/*
 * Corollary of vim_isdigit and vim_isxdigit() that can handle
 * characters > 0x100.
 */
    ਪੂਰਨ
vim_isbdigit(ਪੂਰਨ c)
{
    ਵਾਪਸ (c == '0' || c == '1');
}

    ਸਥਿਰ ਪੂਰਨ
vim_isodigit(ਪੂਰਨ c)
{
    ਵਾਪਸ (c >= '0' && c <= '7');
}

/*
 * Vim's own character class functions.  These exist because many library
 * islower()/toupper() etc. do not work properly: they crash when used with
 * invalid values or can't handle latin1 when the locale is C.
 * Speed is most important here.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LATIN1_ਹੇਠਲਾ 'l'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LATIN1_ਉੱਤਲਾ 'U'

ਸਥਿਰ char_u latin1flags[257] = "                                                                 UUUUUUUUUUUUUUUUUUUUUUUUUU      llllllllllllllllllllllllll                                                                     UUUUUUUUUUUUUUUUUUUUUUU UUUUUUUllllllllllllllllllllllll llllllll";
ਸਥਿਰ char_u latin1upper[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xf7\xd8\xd9\xda\xdb\xdc\xdd\xde\xff";
ਸਥਿਰ char_u latin1lower[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xd7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    ਪੂਰਨ
vim_islower(ਪੂਰਨ c)
{
    ਜੇ (c <= '@')
	ਵਾਪਸ FALSE;
    ਜੇ (c >= 0x80)
    {
	ਜੇ (enc_utf8)
	    ਵਾਪਸ utf_islower(c);
	ਜੇ (c >= 0x100)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ISWLOWER
	    ਜੇ (has_mbyte)
		ਵਾਪਸ iswlower(c);
#ਜੇ_ਅੰਤ
	    // islower() can't handle these chars and may crash
	    ਵਾਪਸ FALSE;
	}
	ਜੇ (enc_latin1like)
	    ਵਾਪਸ (latin1flags[c] & LATIN1_ਹੇਠਲਾ) == LATIN1_ਹੇਠਲਾ;
    }
    ਵਾਪਸ ਸੁਰੱਖਿਅਤ_islower(c);
}

    ਪੂਰਨ
vim_isupper(ਪੂਰਨ c)
{
    ਜੇ (c <= '@')
	ਵਾਪਸ FALSE;
    ਜੇ (c >= 0x80)
    {
	ਜੇ (enc_utf8)
	    ਵਾਪਸ utf_isupper(c);
	ਜੇ (c >= 0x100)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ISWUPPER
	    ਜੇ (has_mbyte)
		ਵਾਪਸ iswupper(c);
#ਜੇ_ਅੰਤ
	    // islower() can't handle these chars and may crash
	    ਵਾਪਸ FALSE;
	}
	ਜੇ (enc_latin1like)
	    ਵਾਪਸ (latin1flags[c] & LATIN1_ਉੱਤਲਾ) == LATIN1_ਉੱਤਲਾ;
    }
    ਵਾਪਸ ਸੁਰੱਖਿਅਤ_isupper(c);
}

    ਪੂਰਨ
vim_isalpha(ਪੂਰਨ c)
{
    ਵਾਪਸ vim_islower(c) || vim_isupper(c);
}

    ਪੂਰਨ
vim_toupper(ਪੂਰਨ c)
{
    ਜੇ (c <= '@')
	ਵਾਪਸ c;
    ਜੇ (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	ਜੇ (enc_utf8)
	    ਵਾਪਸ utf_toupper(c);
	ਜੇ (c >= 0x100)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TOWUPPER
	    ਜੇ (has_mbyte)
		ਵਾਪਸ towupper(c);
#ਜੇ_ਅੰਤ
	    // toupper() can't handle these chars and may crash
	    ਵਾਪਸ c;
	}
	ਜੇ (enc_latin1like)
	    ਵਾਪਸ latin1upper[c];
    }
    ਜੇ (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ਵਾਪਸ TOUPPER_ASC(c);
    ਵਾਪਸ TOUPPER_LOC(c);
}

    ਪੂਰਨ
vim_tolower(ਪੂਰਨ c)
{
    ਜੇ (c <= '@')
	ਵਾਪਸ c;
    ਜੇ (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	ਜੇ (enc_utf8)
	    ਵਾਪਸ utf_tolower(c);
	ਜੇ (c >= 0x100)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TOWLOWER
	    ਜੇ (has_mbyte)
		ਵਾਪਸ towlower(c);
#ਜੇ_ਅੰਤ
	    // tolower() can't handle these chars and may crash
	    ਵਾਪਸ c;
	}
	ਜੇ (enc_latin1like)
	    ਵਾਪਸ latin1lower[c];
    }
    ਜੇ (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ਵਾਪਸ TOLOWER_ASC(c);
    ਵਾਪਸ TOLOWER_LOC(c);
}

/*
 * skiptowhite: skip over text until ' ' or '\t' or NUL.
 */
    char_u *
skiptowhite(char_u *p)
{
    ਜਦੋਂ (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    ਵਾਪਸ p;
}

/*
 * skiptowhite_esc: Like skiptowhite(), but also skip escaped chars
 */
    char_u *
skiptowhite_esc(char_u *p)
{
    ਜਦੋਂ (*p != ' ' && *p != '\t' && *p != NUL)
    {
	ਜੇ ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    ਵਾਪਸ p;
}

/*
 * Get a number from a string and skip over it.
 * Note: the argument is a pointer to a char_u pointer!
 */
    ਲੰਮਾ
getdigits(char_u **pp)
{
    char_u	*p;
    ਲੰਮਾ	retval;

    p = *pp;
    retval = strtol((ਅੱਖਰ *)p, ਨਿਰਾ, 10);
    ਜੇ (*p == '-')		// skip negative sign
	++p;
    p = skipdigits(p);		// skip to next non-digit
    *pp = p;
    ਵਾਪਸ retval;
}

/*
 * Like getdigits() but allow for embedded single quotes.
 */
    ਲੰਮਾ
getdigits_quoted(char_u **pp)
{
    char_u	*p = *pp;
    ਲੰਮਾ	retval = 0;

    ਜੇ (*p == '-')
	++p;
    ਜਦੋਂ (VIM_ISDIGIT(*p))
    {
	ਜੇ (retval >= LONG_MAX / 10 - 10)
	    retval = LONG_MAX;
	ਨਹੀਂਤਾਂ
	    retval = retval * 10 - '0' + *p;
	++p;
	ਜੇ (ਵਿੱਚ_vim9script() && *p == '\'' && VIM_ISDIGIT(p[1]))
	    ++p;
    }
    ਜੇ (**pp == '-')
    {
	ਜੇ (retval == LONG_MAX)
	    retval = LONG_MIN;
	ਨਹੀਂਤਾਂ
	    retval = -retval;
    }
    *pp = p;
    ਵਾਪਸ retval;
}

/*
 * Return TRUE if "lbuf" is empty or only contains blanks.
 */
    ਪੂਰਨ
vim_isblankline(char_u *lbuf)
{
    char_u	*p;

    p = skipwhite(lbuf);
    ਵਾਪਸ (*p == NUL || *p == '\r' || *p == '\n');
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
    ਖਾਲੀ
vim_str2nr(
    char_u		*ਸ਼ੁਰੂ,
    ਪੂਰਨ			*prep,	    // return: type of number 0 = decimal, 'x'
				    // or 'X' is hex, '0', 'o' or 'O' is octal,
				    // 'b' or 'B' is bin
    ਪੂਰਨ			*len,	    // return: detected length of number
    ਪੂਰਨ			what,	    // what numbers to recognize
    varnumber_T		*nptr,	    // return: signed result
    uvarnumber_T	*unptr,	    // return: unsigned result
    ਪੂਰਨ			maxlen,     // max length of string to check
    ਪੂਰਨ			strict,     // check strictly
    ਪੂਰਨ			*ਉਛਾਲ)  // when not NULL set to TRUE for overflow
{
    char_u	    *ptr = ਸ਼ੁਰੂ;
    ਪੂਰਨ		    pre = 0;		// default is decimal
    ਪੂਰਨ		    negative = FALSE;
    uvarnumber_T    un = 0;
    ਪੂਰਨ		    n;

    ਜੇ (len != ਨਿਰਾ)
	*len = 0;

    ਜੇ (ptr[0] == '-')
    {
	negative = TRUE;
	++ptr;
    }

    // Recognize hex, octal, and bin.
    ਜੇ (ptr[0] == '0' && ptr[1] != '8' && ptr[1] != '9'
					       && (maxlen == 0 || maxlen > 1))
    {
	pre = ptr[1];
	ਜੇ ((what & STR2NR_HEX)
		&& (pre == 'X' || pre == 'x') && vim_isxdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // hexadecimal
	    ptr += 2;
	ਨਹੀਂਤਾਂ ਜੇ ((what & STR2NR_BIN)
		&& (pre == 'B' || pre == 'b') && vim_isbdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    //ਦੁਆਧਾਰੀ
	    ptr += 2;
	ਨਹੀਂਤਾਂ ਜੇ ((what & STR2NR_OOCT)
		&& (pre == 'O' || pre == 'o') && vim_isodigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // octal with prefix "0o"
	    ptr += 2;
	ਨਹੀਂਤਾਂ
	{
	    // decimal or octal, default is decimal
	    pre = 0;
	    ਜੇ (what & STR2NR_OCT)
	    {
		// Don't interpret "0", "08" or "0129" as octal.
		ਲਈ (n = 1; n != maxlen && VIM_ISDIGIT(ptr[n]); ++n)
		{
		    ਜੇ (ptr[n] > '7')
		    {
			pre = 0;	// can't be octal
			ਰੋਕੋ;
		    }
		    pre = '0';	// assume octal
		}
	    }
	}
    }

    // Do the conversion manually to avoid sscanf() quirks.
    n = 1;
    ਜੇ (pre == 'B' || pre == 'b'
			     || ((what & STR2NR_BIN) && (what & STR2NR_FORCE)))
    {
	// bin
	ਜੇ (pre != 0)
	    n += 2;	    // skip over "0b"
	ਜਦੋਂ ('0' <= *ptr && *ptr <= '1')
	{
	    // avoid ubsan error for overflow
	    ਜੇ (un <= UVARNUM_MAX / 2)
		un = 2 * un + (uvarnumber_T)(*ptr - '0');
	    ਨਹੀਂਤਾਂ
	    {
		un = UVARNUM_MAX;
		ਜੇ (ਉਛਾਲ != ਨਿਰਾ)
		    *ਉਛਾਲ = TRUE;
	    }
	    ++ptr;
	    ਜੇ (n++ == maxlen)
		ਰੋਕੋ;
	    ਜੇ ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '1')
	    {
		++ptr;
		ਜੇ (n++ == maxlen)
		    ਰੋਕੋ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (pre == 'O' || pre == 'o' ||
		pre == '0' || ((what & STR2NR_OCT) && (what & STR2NR_FORCE)))
    {
	// octal
	ਜੇ (pre != 0 && pre != '0')
	    n += 2;	    // skip over "0o"
	ਜਦੋਂ ('0' <= *ptr && *ptr <= '7')
	{
	    // avoid ubsan error for overflow
	    ਜੇ (un <= UVARNUM_MAX / 8)
		un = 8 * un + (uvarnumber_T)(*ptr - '0');
	    ਨਹੀਂਤਾਂ
	    {
		un = UVARNUM_MAX;
		ਜੇ (ਉਛਾਲ != ਨਿਰਾ)
		    *ਉਛਾਲ = TRUE;
	    }
	    ++ptr;
	    ਜੇ (n++ == maxlen)
		ਰੋਕੋ;
	    ਜੇ ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '7')
	    {
		++ptr;
		ਜੇ (n++ == maxlen)
		    ਰੋਕੋ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (pre != 0 || ((what & STR2NR_HEX) && (what & STR2NR_FORCE)))
    {
	// hex
	ਜੇ (pre != 0)
	    n += 2;	    // skip over "0x"
	ਜਦੋਂ (vim_isxdigit(*ptr))
	{
	    // avoid ubsan error for overflow
	    ਜੇ (un <= UVARNUM_MAX / 16)
		un = 16 * un + (uvarnumber_T)hex2nr(*ptr);
	    ਨਹੀਂਤਾਂ
	    {
		un = UVARNUM_MAX;
		ਜੇ (ਉਛਾਲ != ਨਿਰਾ)
		    *ਉਛਾਲ = TRUE;
	    }
	    ++ptr;
	    ਜੇ (n++ == maxlen)
		ਰੋਕੋ;
	    ਜੇ ((what & STR2NR_QUOTE) && *ptr == '\'' && vim_isxdigit(ptr[1]))
	    {
		++ptr;
		ਜੇ (n++ == maxlen)
		    ਰੋਕੋ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// decimal
	ਜਦੋਂ (VIM_ISDIGIT(*ptr))
	{
	    uvarnumber_T    digit = (uvarnumber_T)(*ptr - '0');

	    // avoid ubsan error for overflow
	    ਜੇ (un < UVARNUM_MAX / 10
		    || (un == UVARNUM_MAX / 10 && digit <= UVARNUM_MAX % 10))
		un = 10 * un + digit;
	    ਨਹੀਂਤਾਂ
	    {
		un = UVARNUM_MAX;
		ਜੇ (ਉਛਾਲ != ਨਿਰਾ)
		    *ਉਛਾਲ = TRUE;
	    }
	    ++ptr;
	    ਜੇ (n++ == maxlen)
		ਰੋਕੋ;
	    ਜੇ ((what & STR2NR_QUOTE) && *ptr == '\'' && VIM_ISDIGIT(ptr[1]))
	    {
		++ptr;
		ਜੇ (n++ == maxlen)
		    ਰੋਕੋ;
	    }
	}
    }

    // Check for an alphanumeric character immediately following, that is
    // most likely a typo.
    ਜੇ (strict && n - 1 != maxlen && ASCII_ISALNUM(*ptr))
	ਵਾਪਸ;

    ਜੇ (prep != ਨਿਰਾ)
	*prep = pre;
    ਜੇ (len != ਨਿਰਾ)
	*len = (ਪੂਰਨ)(ptr - ਸ਼ੁਰੂ);
    ਜੇ (nptr != ਨਿਰਾ)
    {
	ਜੇ (negative)   // account for leading '-' for decimal numbers
	{
	    // avoid ubsan error for overflow
	    ਜੇ (un > VARNUM_MAX)
	    {
		*nptr = VARNUM_MIN;
		ਜੇ (ਉਛਾਲ != ਨਿਰਾ)
		    *ਉਛਾਲ = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
		*nptr = -(varnumber_T)un;
	}
	ਨਹੀਂਤਾਂ
	{
	    // prevent a large unsigned number to become negative
	    ਜੇ (un > VARNUM_MAX)
	    {
		un = VARNUM_MAX;
		ਜੇ (ਉਛਾਲ != ਨਿਰਾ)
		    *ਉਛਾਲ = TRUE;
	    }
	    *nptr = (varnumber_T)un;
	}
    }
    ਜੇ (unptr != ਨਿਰਾ)
	*unptr = un;
}

/*
 * Return the value of a single hex character.
 * Only valid when the argument is '0' - '9', 'A' - 'F' or 'a' - 'f'.
 */
    ਪੂਰਨ
hex2nr(ਪੂਰਨ c)
{
    ਜੇ (c >= 'a' && c <= 'f')
	ਵਾਪਸ c - 'a' + 10;
    ਜੇ (c >= 'A' && c <= 'F')
	ਵਾਪਸ c - 'A' + 10;
    ਵਾਪਸ c - '0';
}

/*
 * Convert two hex characters to a byte.
 * Return -1 if one of the characters is not hex.
 */
    ਪੂਰਨ
hexhex2nr(char_u *p)
{
    ਜੇ (!vim_isxdigit(p[0]) || !vim_isxdigit(p[1]))
	ਵਾਪਸ -1;
    ਵਾਪਸ (hex2nr(p[0]) << 4) + hex2nr(p[1]);
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
    ਪੂਰਨ
rem_backslash(char_u *str)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    ਵਾਪਸ (str[0] == '\\'
	    && str[1] < 0x80
	    && (str[1] == ' '
		|| (str[1] != NUL
		    && str[1] != '*'
		    && str[1] != '?'
		    && !vim_isfilec(str[1]))));
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ (str[0] == '\\' && str[1] != NUL);
#ਜੇ_ਅੰਤ
}

/*
 * Halve the number of backslashes in a file name argument.
 * For MS-DOS we only do this if the character after the backslash
 * is not a normal file character.
 */
    ਖਾਲੀ
backslash_halve(char_u *p)
{
    ਲਈ ( ; *p; ++p)
	ਜੇ (rem_backslash(p))
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
    ਜੇ (res == ਨਿਰਾ)
	ਵਾਪਸ p;
    backslash_halve(res);
    ਵਾਪਸ res;
}
