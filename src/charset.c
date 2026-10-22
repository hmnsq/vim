/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_isopt(char_u *var, buf_T *buf, ಪೂರ್ಣಾಂಕ only_check);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ win_nolbr_chartabsize(chartabsize_T *cts, ಪೂರ್ಣಾಂಕ *headp);
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ nr2hex(ಚಿಹ್ನೆರಹಿತ c);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ    chartab_initialized = FALSE;

// b_chartab[] is an array of 32 bytes, each bit representing one of the
// characters 0-255.
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_CHARTAB(buf, c) (buf)->b_chartab[(ಚಿಹ್ನೆರಹಿತ)(c) >> 3] |= (1 << ((c) & 0x7))
#ವ್ಯಾಖ್ಯಾನಿಸು RESET_CHARTAB(buf, c) (buf)->b_chartab[(ಚಿಹ್ನೆರಹಿತ)(c) >> 3] &= ~(1 << ((c) & 0x7))
#ವ್ಯಾಖ್ಯಾನಿಸು GET_CHARTAB(buf, c) ((buf)->b_chartab[(ಚಿಹ್ನೆರಹಿತ)(c) >> 3] & (1 << ((c) & 0x7)))

// table used below, see init_chartab() for an explanation
ಸ್ಥಿರ char_u	g_chartab[256];

/*
 * Flags for g_chartab[].
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CT_CELL_ಮುಖವಾಡ	0x07	// mask: nr of display cells (1, 2 or 4)
#ವ್ಯಾಖ್ಯಾನಿಸು CT_PRINT_CHAR	0x10	// flag: set for printable chars
#ವ್ಯಾಖ್ಯಾನಿಸು CT_ID_CHAR	0x20	// flag: set for ID chars
#ವ್ಯಾಖ್ಯಾನಿಸು CT_FNAME_CHAR	0x40	// flag: set for file name chars

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಒಳ_win_border(win_T *wp, colnr_T vcol);

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
    ಪೂರ್ಣಾಂಕ
init_chartab(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ buf_init_chartab(curbuf, TRUE);
}

    ಪೂರ್ಣಾಂಕ
buf_init_chartab(
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		global)		// FALSE: only set buf->b_chartab[]
{
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 */
	c = 0;
	ಆಗ (c < ' ')
	    g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	ಆಗ (c <= '~')
	    g_chartab[c++] = 1 + CT_PRINT_CHAR;
	ಆಗ (c < 256)
	{
	    // UTF-8: bytes 0xa0 - 0xff are printable (latin1)
	    ಆದರೆ (enc_utf8 && c >= 0xa0)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // euc-jp characters starting with 0x8e are single width
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs == DBCS_JPNU && c == 0x8e)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // other double-byte chars can be printable AND double-width
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs != 0 && MB_BYTE2LEN(c) == 2)
		g_chartab[c++] = CT_PRINT_CHAR + 2;
	    ಇಲ್ಲದಿದ್ದರೆ
		// the rest is unprintable by default
		g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	}

	// Assume that every multi-byte char is a filename character.
	ಗಾಗಿ (c = 1; c < 256; ++c)
	    ಆದರೆ ((enc_dbcs != 0 && MB_BYTE2LEN(c) > 1)
		    || (enc_dbcs == DBCS_JPNU && c == 0x8e)
		    || (enc_utf8 && c >= 0xa0))
		g_chartab[c] |= CT_FNAME_CHAR;
    }

    /*
     * Init word char flags all to FALSE
     */
    CLEAR_ಕ್ಷೇತ್ರ(buf->b_chartab);
    ಆದರೆ (enc_dbcs != 0)
	ಗಾಗಿ (c = 0; c < 256; ++c)
	{
	    // double-byte characters are probably word characters
	    ಆದರೆ (MB_BYTE2LEN(c) == 2)
		ಗಣ_CHARTAB(buf, c);
	}

    /*
     * In lisp mode the '-' character is included in keywords.
     */
    ಆದರೆ (buf->b_p_lisp)
	ಗಣ_CHARTAB(buf, '-');

    // Walk through the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    ಗಾಗಿ (i = global ? 0 : 3; i <= 3; ++i)
    {
	ಆದರೆ (i == 0)
	    p = p_isi;		// first round: 'isident'
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 1)
	    p = p_isp;		// second round: 'isprint'
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 2)
	    p = p_isf;		// third round: 'isfname'
	ಇಲ್ಲದಿದ್ದರೆ	// i == 3
	    p = buf->b_p_isk;	// fourth round: 'iskeyword'

	ಆದರೆ (parse_isopt(p, buf, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    chartab_initialized = TRUE;
    ಹಿಂತಿರುಗಿ OK;
}

/**
 * Checks the format for the option settings 'iskeyword', 'isident', 'isfname'
 * or 'isprint'.
 * Returns FAIL if has an error, OK otherwise.
 */
	ಪೂರ್ಣಾಂಕ
check_isopt(char_u *var)
{
    ಹಿಂತಿರುಗಿ parse_isopt(var, ಖಾಲಿ, TRUE);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_isopt(
	char_u	*var,
	buf_T	*buf,
	ಪೂರ್ಣಾಂಕ	only_check)	// FALSE: refill g_chartab[]
{
    char_u	*p = var;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		c2;
    ಪೂರ್ಣಾಂಕ		tilde;
    ಪೂರ್ಣಾಂಕ		do_isalpha;
    ಪೂರ್ಣಾಂಕ		trail_comma;

    // Parses the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    // Each option is a list of characters, character numbers or ranges,
    // separated by commas, e.g.: "200-210,x,#-178,-"
    ಆಗ (*p)
    {
	tilde = FALSE;
	do_isalpha = FALSE;
	ಆದರೆ (*p == '^' && p[1] != NUL)
	{
	    tilde = TRUE;
	    ++p;
	}
	ಆದರೆ (VIM_ISDIGIT(*p))
	    c = getdigits(&p);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	    c = mb_ptr2char_adv(&p);
	ಇಲ್ಲದಿದ್ದರೆ
	    c = *p++;
	c2 = -1;
	ಆದರೆ (*p == '-' && p[1] != NUL)
	{
	    ++p;
	    ಆದರೆ (VIM_ISDIGIT(*p))
		c2 = getdigits(&p);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
		c2 = mb_ptr2char_adv(&p);
	    ಇಲ್ಲದಿದ್ದರೆ
		c2 = *p++;
	}
	ಆದರೆ (c <= 0 || c >= 256 || (c2 < c && c2 != -1) || c2 >= 256
						  || !(*p == NUL || *p == ','))
	    ಹಿಂತಿರುಗಿ FAIL;

	trail_comma = *p == ',';
	p = skip_ಗಮ್ಯ_option_part(p);
	ಆದರೆ (trail_comma && *p == NUL)
	    // Trailing comma is not allowed.
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (only_check)
	    ಮುಂದುವರಿ;

	ಆದರೆ (c2 == -1)	// not a range
	{
	    /*
	     * A single '@' (not "@-@"):
	     * Decide on letters being ID/printable/keyword chars with
	     * standard function isalpha(). This takes care of locale for
	     * single-byte characters).
	     */
	    ಆದರೆ (c == '@')
	    {
		do_isalpha = TRUE;
		c = 1;
		c2 = 255;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		c2 = c;
	}

	ಆಗ (c <= c2)
	{
	    // Use the MB_ functions here, because isalpha() doesn't
	    // work properly when 'encoding' is "latin1" and the locale is
	    // "C".
	    ಆದರೆ (!do_isalpha || MB_ISLOWER(c) || MB_ISUPPER(c))
	    {
		ಆದರೆ (var == p_isi)			// (re)set ID flag
		{
		    ಆದರೆ (tilde)
			g_chartab[c] &= ~CT_ID_CHAR;
		    ಇಲ್ಲದಿದ್ದರೆ
			g_chartab[c] |= CT_ID_CHAR;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (var == p_isp)			// (re)set printable
		{
		    ಆದರೆ ((c < ' ' || c > '~'
			    // For double-byte we keep the cell width, so
			    // that we can detect it from the first byte.
			) && !(enc_dbcs && MB_BYTE2LEN(c) == 2))
		    {
			ಆದರೆ (tilde)
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_ಮುಖವಾಡ)
					 + ((dy_flags & DY_UHEX) ? 4 : 2);
			    g_chartab[c] &= ~CT_PRINT_CHAR;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_ಮುಖವಾಡ) + 1;
			    g_chartab[c] |= CT_PRINT_CHAR;
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (var == p_isf)			// (re)set fname flag
		{
		    ಆದರೆ (tilde)
			g_chartab[c] &= ~CT_FNAME_CHAR;
		    ಇಲ್ಲದಿದ್ದರೆ
			g_chartab[c] |= CT_FNAME_CHAR;
		}
		ಇಲ್ಲದಿದ್ದರೆ // var == p_isk || var == buf->b_p_isk
							// (re)set keyword flag
		{
		    ಆದರೆ (tilde)
			RESET_CHARTAB(buf, c);
		    ಇಲ್ಲದಿದ್ದರೆ
			ಗಣ_CHARTAB(buf, c);
		}
	    }
	    ++c;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Translate any special characters in buf[bufsize] in-place.
 * The result is a string with only printable characters, but if there is not
 * enough room, not all characters will be translated.
 */
    ಶೂನ್ಯ
trans_characters(
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		bufsize)
{
    ಪೂರ್ಣಾಂಕ		len;		// length of string needing translation
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;		// room in buffer after string
    char_u	*trs;		// translated character
    ಪೂರ್ಣಾಂಕ		trs_len;	// length of trs[]

    len = (ಪೂರ್ಣಾಂಕ)STRLEN(buf);
    ಸ್ಥಳ = bufsize - len;
    ಆಗ (*buf != 0)
    {
	// Assume a multi-byte character doesn't need translation.
	ಆದರೆ (has_mbyte && (trs_len = (*mb_ptr2len)(buf)) > 1)
	    len -= trs_len;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    trs = transchar_byte(*buf);
	    trs_len = (ಪೂರ್ಣಾಂಕ)STRLEN(trs);
	    ಆದರೆ (trs_len > 1)
	    {
		ಸ್ಥಳ -= trs_len - 1;
		ಆದರೆ (ಸ್ಥಳ <= 0)
		    ಹಿಂತಿರುಗಿ;
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
    ಪೂರ್ಣಾಂಕ		l, len, c;
    char_u	hexbuf[11];

    ಆದರೆ (has_mbyte)
    {
	// Compute the length of the result, taking account of unprintable
	// multi-byte characters.
	len = 0;
	p = s;
	ಆಗ (*p != NUL)
	{
	    ಆದರೆ ((l = (*mb_ptr2len)(p)) > 1)
	    {
		c = (*mb_ptr2char)(p);
		p += l;
		ಆದರೆ (vim_isprintc(c))
		    len += l;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    transchar_hex(hexbuf, c);
		    len += (ಪೂರ್ಣಾಂಕ)STRLEN(hexbuf);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		l = byte2cells(*p++);
		ಆದರೆ (l > 0)
		    len += l;
		ಇಲ್ಲದಿದ್ದರೆ
		    len += 4;	// illegal byte sequence
	    }
	}
	res = alloc(len + 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	res = alloc(vim_strsize(s) + 1);

    ಆದರೆ (res == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    char_u *d = res;

    p = s;
    ಆಗ (*p != NUL)
    {
	ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    c = (*mb_ptr2char)(p);
	    ಆದರೆ (vim_isprintc(c))
	    {
		mch_memmove(d, p, (size_t)l);
		d += l;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		transchar_hex(d, c);
		d += STRLEN(d);
	    }
	    p += l;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u	*trs = transchar_byte(*p++);
	    size_t	trs_len = STRLEN(trs);

	    mch_memmove(d, trs, trs_len);
	    d += trs_len;
	}
    }
    *d = NUL;
    ಹಿಂತಿರುಗಿ res;
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
    ಪೂರ್ಣಾಂಕ		orglen,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		buflen)
{
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		len = orglen;

#ವ್ಯಾಖ್ಯಾನಿಸು GA_CHAR(i)  ((char_u *)ga.ga_ದತ್ತಾಂಶ)[i]
#ವ್ಯಾಖ್ಯಾನಿಸು GA_PTR(i)   ((char_u *)ga.ga_ದತ್ತಾಂಶ + (i))
#ವ್ಯಾಖ್ಯಾನಿಸು STR_CHAR(i)  (buf == ಖಾಲಿ ? GA_CHAR(i) : buf[i])
#ವ್ಯಾಖ್ಯಾನಿಸು STR_PTR(i)   (buf == ಖಾಲಿ ? GA_PTR(i) : buf + (i))

    // Copy "str" into "buf" or allocated memory, unmodified.
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	ga_init2(&ga, 1, 10);
	ಆದರೆ (ga_grow(&ga, len + 1) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	mch_memmove(ga.ga_ದತ್ತಾಂಶ, str, (size_t)len);
	ga.ga_len = len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (len >= buflen)	    // Ugly!
	    len = buflen - 1;
	mch_memmove(buf, str, (size_t)len);
    }
    ಆದರೆ (buf == ಖಾಲಿ)
	GA_CHAR(len) = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
	buf[len] = NUL;

    // Make each character lower case.
    i = 0;
    ಆಗ (STR_CHAR(i) != NUL)
    {
	ಆದರೆ (enc_utf8 || (has_mbyte && MB_BYTE2LEN(STR_CHAR(i)) > 1))
	{
	    ಆದರೆ (enc_utf8)
	    {
		ಪೂರ್ಣಾಂಕ	c = utf_ptr2char(STR_PTR(i));
		ಪೂರ್ಣಾಂಕ	olen = utf_ptr2len(STR_PTR(i));
		ಪೂರ್ಣಾಂಕ	lc = utf_tolower(c);

		// Only replace the character when it is not an invalid
		// sequence (ASCII character or more than one byte) and
		// utf_tolower() doesn't return the original character.
		ಆದರೆ ((c < 0x80 || olen > 1) && c != lc)
		{
		    ಪೂರ್ಣಾಂಕ	    nlen = utf_char2len(lc);

		    // If the byte length changes need to shift the following
		    // characters forward or backward.
		    ಆದರೆ (olen != nlen)
		    {
			ಆದರೆ (nlen > olen)
			{
			    ಆದರೆ (buf == ಖಾಲಿ
				    ? ga_grow(&ga, nlen - olen + 1) == FAIL
				    : len + nlen - olen >= buflen)
			    {
				// out of memory, keep old char
				lc = c;
				nlen = olen;
			    }
			}
			ಆದರೆ (olen != nlen)
			{
			    ಆದರೆ (buf == ಖಾಲಿ)
			    {
				STRMOVE(GA_PTR(i) + nlen, GA_PTR(i) + olen);
				ga.ga_len += nlen - olen;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				STRMOVE(buf + i + nlen, buf + i + olen);
				len += nlen - olen;
			    }
			}
		    }
		    (ಶೂನ್ಯ)utf_char2bytes(lc, STR_PTR(i));
		}
	    }
	    // skip to next multi-byte char
	    i += (*mb_ptr2len)(STR_PTR(i));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (buf == ಖಾಲಿ)
		GA_CHAR(i) = TOLOWER_LOC(GA_CHAR(i));
	    ಇಲ್ಲದಿದ್ದರೆ
		buf[i] = TOLOWER_LOC(buf[i]);
	    ++i;
	}
    }

    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (char_u *)ga.ga_ದತ್ತಾಂಶ;
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Catch 22: g_chartab[] can't be initialized before the options are
 * initialized, and initializing options may cause transchar() to be called!
 * When chartab_initialized == FALSE don't use g_chartab[].
 * Does NOT work for multi-byte characters, c must be <= 255.
 * Also doesn't work for the first byte of a multi-byte, "c" must be a
 * character!
 */
ಸ್ಥಿರ char_u	transchar_charbuf[7];

    char_u *
transchar(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ transchar_buf(curbuf, c);
}

    char_u *
transchar_buf(buf_T *buf, ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ			i;

    i = 0;
    ಆದರೆ (IS_SPECIAL(c))	    // special key code, display as ~@ char
    {
	transchar_charbuf[0] = '~';
	transchar_charbuf[1] = '@';
	i = 2;
	c = K_ಎರಡನೇ(c);
    }

    ಆದರೆ ((!chartab_initialized && ((c >= ' ' && c <= '~')))
					|| (c < 256 && vim_isprintc_strict(c)))
    {
	// printable character
	transchar_charbuf[i] = c;
	transchar_charbuf[i + 1] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	transchar_nonprint(buf, transchar_charbuf + i, c);
    ಹಿಂತಿರುಗಿ transchar_charbuf;
}

/*
 * Like transchar(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of the current buffer.
 */
    char_u *
transchar_byte(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ transchar_byte_buf(curbuf, c);
}

/*
 * Like transchar_buf(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of "buf", unless it is NULL.
 */
    char_u *
transchar_byte_buf(buf_T *buf, ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (enc_utf8 && c >= 0x80)
    {
	transchar_nonprint(buf, transchar_charbuf, c);
	ಹಿಂತಿರುಗಿ transchar_charbuf;
    }
    ಹಿಂತಿರುಗಿ transchar_buf(buf, c);
}
/*
 * Convert non-printable character to two or more printable characters in
 * "charbuf[]".  "charbuf" needs to be able to hold five bytes.
 * Does NOT work for multi-byte characters, c must be <= 255.
 */
    ಶೂನ್ಯ
transchar_nonprint(buf_T *buf, char_u *charbuf, ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c == NL)
	c = NUL;		// we use newline in place of a NUL
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf != ಖಾಲಿ && c == CAR && get_fileformat(buf) == EOL_MAC)
	c = NL;			// we use CR in place of  NL in this case

    ಆದರೆ (dy_flags & DY_UHEX)		// 'display' has "uhex"
	transchar_hex(charbuf, c);

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c <= 0x7f)			// 0x00 - 0x1f and 0x7f
    {
	charbuf[0] = '^';
	charbuf[1] = c ^ 0x40;		// DEL displayed as ^?
	charbuf[2] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
    {
	transchar_hex(charbuf, c);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c >= ' ' + 0x80 && c <= '~' + 0x80)    // 0xa0 - 0xfe
    {
	charbuf[0] = '|';
	charbuf[1] = c - 0x80;
	charbuf[2] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ					    // 0x80 - 0x9f and 0xff
    {
	charbuf[0] = '~';
	charbuf[1] = (c - 0x80) ^ 0x40;	// 0xff displayed as ~?
	charbuf[2] = NUL;
    }
}

    ಶೂನ್ಯ
transchar_hex(char_u *buf, ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ		i = 0;

    buf[0] = '<';
    ಆದರೆ (c > 255)
    {
	buf[++i] = nr2hex((ಚಿಹ್ನೆರಹಿತ)c >> 12);
	buf[++i] = nr2hex((ಚಿಹ್ನೆರಹಿತ)c >> 8);
    }
    buf[++i] = nr2hex((ಚಿಹ್ನೆರಹಿತ)c >> 4);
    buf[++i] = nr2hex((ಚಿಹ್ನೆರಹಿತ)c);
    buf[++i] = '>';
    buf[++i] = NUL;
}

/*
 * Convert the lower 4 bits of byte "c" to its hex character.
 * Lower case letters are used to avoid the confusion of <F1> being 0xf1 or
 * function key 1.
 */
    ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ
nr2hex(ಚಿಹ್ನೆರಹಿತ c)
{
    ಆದರೆ ((c & 0xf) <= 9)
	ಹಿಂತಿರುಗಿ (c & 0xf) + '0';
    ಹಿಂತಿರುಗಿ (c & 0xf) - 10 + 'a';
}

/*
 * Return number of display cells occupied by byte "b".
 * Caller must make sure 0 <= b <= 255.
 * For multi-byte mode "b" must be the first byte of a character.
 * A TAB is counted as two cells: "^I".
 * For UTF-8 mode this will return 0 for bytes >= 0x80, because the number of
 * cells depends on further bytes.
 */
    ಪೂರ್ಣಾಂಕ
byte2cells(ಪೂರ್ಣಾಂಕ b)
{
    ಆದರೆ (enc_utf8 && b >= 0x80)
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ (g_chartab[b] & CT_CELL_ಮುಖವಾಡ);
}

/*
 * Return number of display cells occupied by character "c".
 * "c" can be a special key (negative number) in which case 3 or 4 is returned.
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    ಪೂರ್ಣಾಂಕ
char2cells(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (IS_SPECIAL(c))
	ಹಿಂತಿರುಗಿ char2cells(K_ಎರಡನೇ(c)) + 2;
    ಆದರೆ (c >= 0x80)
    {
	// UTF-8: above 0x80 need to check the value
	ಆದರೆ (enc_utf8)
	    ಹಿಂತಿರುಗಿ utf_char2cells(c);
	// DBCS: double-byte means double-width, except for euc-jp with first
	// byte 0x8e
	ಆದರೆ (enc_dbcs != 0 && c >= 0x100)
	{
	    ಆದರೆ (enc_dbcs == DBCS_JPNU && ((ಚಿಹ್ನೆರಹಿತ)c >> 8) == 0x8e)
		ಹಿಂತಿರುಗಿ 1;
	    ಹಿಂತಿರುಗಿ 2;
	}
    }
    ಹಿಂತಿರುಗಿ (g_chartab[c & 0xff] & CT_CELL_ಮುಖವಾಡ);
}

/*
 * Return number of display cells occupied by character at "*p".
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    ಪೂರ್ಣಾಂಕ
ptr2cells(char_u *p)
{
    ಆದರೆ (!has_mbyte)
	ಹಿಂತಿರುಗಿ byte2cells(*p);

    // For UTF-8 we need to look at more bytes if the first byte is >= 0x80.
    ಆದರೆ (enc_utf8 && *p >= 0x80)
	ಹಿಂತಿರುಗಿ utf_ptr2cells(p);
    // For DBCS we can tell the cell count from the first byte.
    ಹಿಂತಿರುಗಿ (g_chartab[*p] & CT_CELL_ಮುಖವಾಡ);
}

/*
 * Return the number of character cells string "s" will take on the screen,
 * counting TABs as two characters: "^I".
 */
    ಪೂರ್ಣಾಂಕ
vim_strsize(char_u *s)
{
    ಹಿಂತಿರುಗಿ vim_strnsize(s, (ಪೂರ್ಣಾಂಕ)MAXCOL);
}

/*
 * Return the number of character cells string "s[len]" will take on the
 * screen, counting TABs as two characters: "^I".
 */
    ಪೂರ್ಣಾಂಕ
vim_strnsize(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ		ಗಾತ್ರ_2 = 0;

    ಆಗ (*s != NUL && --len >= 0)
    {
	ಪೂರ್ಣಾಂಕ	    l = (*mb_ptr2len)(s);

	ಗಾತ್ರ_2 += ptr2cells(s);
	s += l;
	len -= l - 1;
    }

    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

/*
 * Return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab.
 * Use a define to make it fast, this is used very often!!!
 * Also see getvcol() below.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, ಲಂಬಸಾಲು) \
    ಆದರೆ (*(p) == TAB && (!(wp)->w_p_list || (wp)->w_lcs_chars.tab1)) \
    { \
	ಹಿಂತಿರುಗಿ tabstop_padding(ಲಂಬಸಾಲು, (buf)->b_p_ts, (buf)->b_p_vts_array); \
    } \
    ಇಲ್ಲದಿದ್ದರೆ \
	ಹಿಂತಿರುಗಿ ptr2cells(p);
#ಇಲ್ಲದಿದ್ದರೆ
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, ಲಂಬಸಾಲು) \
    ಆದರೆ (*(p) == TAB && (!(wp)->w_p_list || wp->w_lcs_chars.tab1)) \
    { \
	ಪೂರ್ಣಾಂಕ ts; \
	ts = (buf)->b_p_ts; \
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(ts - (ಲಂಬಸಾಲು % ts)); \
    } \
    ಇಲ್ಲದಿದ್ದರೆ \
	ಹಿಂತಿರುಗಿ ptr2cells(p);
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
chartabsize(char_u *p, colnr_T ಲಂಬಸಾಲು)
{
    RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, p, ಲಂಬಸಾಲು)
}

#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ಪೂರ್ಣಾಂಕ
win_chartabsize(win_T *wp, char_u *p, colnr_T ಲಂಬಸಾಲು)
{
    RET_WIN_BUF_CHARTABSIZE(wp, wp->w_ಬಫರ್, p, ಲಂಬಸಾಲು)
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return the number of characters the string "s" will take on the screen,
 * taking into account the size of a tab.
 * Does not handle text properties, since "s" is not a buffer line.
 */
    ಪೂರ್ಣಾಂಕ
linetabsize_str(char_u *s)
{
    ಹಿಂತಿರುಗಿ linetabsize_ಲಂಬಸಾಲು(0, s);
}

/*
 * Like linetabsize_str(), but "s" starts at column "startcol".
 */
    ಪೂರ್ಣಾಂಕ
linetabsize_ಲಂಬಸಾಲು(ಪೂರ್ಣಾಂಕ startcol, char_u *s)
{
    chartabsize_T cts;
    vimlong_T vcol;

    init_chartabsize_arg(&cts, curwin, 0, startcol, s, s);
    vcol = cts.cts_vcol;

    ಆಗ (*cts.cts_ptr != NUL)
    {
	vcol += lbr_chartabsize_adv(&cts);
	ಆದರೆ (vcol > MAXCOL)
	{
	    cts.cts_vcol = MAXCOL;
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    cts.cts_vcol = (ಪೂರ್ಣಾಂಕ)vcol;
    }
    clear_chartabsize_arg(&cts);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)cts.cts_vcol;
}

/*
 * Like linetabsize_str(), but for a given window instead of the current one.
 * Doesn't count the size of 'listchars' "eol".
 */
    ಪೂರ್ಣಾಂಕ
win_linetabsize(win_T *wp, linenr_T lnum, char_u *ಸಾಲು, colnr_T len)
{
    chartabsize_T cts;

    init_chartabsize_arg(&cts, wp, lnum, 0, ಸಾಲು, ಸಾಲು);
    win_linetabsize_cts(&cts, len);
    clear_chartabsize_arg(&cts);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)cts.cts_vcol;
}

/*
 * Return the number of cells line "lnum" of window "wp" will take on the
 * screen, taking into account the size of a tab and text properties.
 * Doesn't count the size of 'listchars' "eol".
 */
    ಪೂರ್ಣಾಂಕ
linetabsize(win_T *wp, linenr_T lnum)
{
    ಹಿಂತಿರುಗಿ win_linetabsize(wp, lnum,
		       ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE), (colnr_T)MAXCOL);
}

/*
 * Like linetabsize(), but counts the size of 'listchars' "eol".
 */
    ಪೂರ್ಣಾಂಕ
linetabsize_eol(win_T *wp, linenr_T lnum)
{
    ಹಿಂತಿರುಗಿ linetabsize(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Like linetabsize(), but excludes 'above'/'after'/'right'/'below' aligned
 * virtual text, while keeping inline virtual text.
 */
    ಪೂರ್ಣಾಂಕ
linetabsize_no_outer(win_T *wp, linenr_T lnum)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_PROP_POPUP
    ಹಿಂತಿರುಗಿ linetabsize(wp, lnum);
#ಇಲ್ಲದಿದ್ದರೆ
    chartabsize_T cts;
    char_u *ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, ಸಾಲು, ಸಾಲು);

    ಆದರೆ (cts.cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ)
    {
	ಪೂರ್ಣಾಂಕ write_idx = 0;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ read_idx = 0; read_idx < cts.cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ; read_idx++)
	{
	    textprop_T *tp = &cts.cts_ಪಠ್ಯ_props[read_idx];
	    ಆದರೆ (tp->tp_ಲಂಬಸಾಲು != MAXCOL)
	    {
		ಆದರೆ (read_idx != write_idx)
		    cts.cts_ಪಠ್ಯ_props[write_idx] = *tp;
		write_idx++;
	    }
	}
	cts.cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ = write_idx;
	ಆದರೆ (cts.cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ == 0)
	    VIM_CLEAR(cts.cts_ಪಠ್ಯ_props);
    }

    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)cts.cts_vcol;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Like linetabsize_no_outer(), but counts the size of 'listchars' "eol".
 */
    ಪೂರ್ಣಾಂಕ
linetabsize_no_outer_eol(win_T *wp, linenr_T lnum)
{
    ಹಿಂತಿರುಗಿ linetabsize_no_outer(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Return TRUE when win_lbr_chartabsize() does nothing more than
 * win_nolbr_chartabsize(): no 'linebreak', 'breakindent', 'showbreak' and no
 * text properties that insert text.
 */
#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win_lbr_is_simple_ಅಗಲ(chartabsize_T *cts)
{
    ಹಿಂತಿರುಗಿ TRUE
# ifdef FEAT_LINEBREAK
	&& !cts->cts_win->w_p_lbr && !cts->cts_win->w_p_bri
	&& *get_showbreak_ಮೌಲ್ಯ(cts->cts_win) == NUL
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_ಪಠ್ಯ
# endif
	;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
win_linetabsize_cts(chartabsize_T *cts, colnr_T len)
{
    vimlong_T vcol = cts->cts_vcol;
#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    // Fast path: when win_lbr_chartabsize() would just return
    // win_nolbr_chartabsize() (wrap on; no 'linebreak'/'breakindent'/
    // 'showbreak'/inserted text properties), call it directly.
    ಪೂರ್ಣಾಂಕ simple_ಅಗಲ = cts->cts_win->w_p_wrap && win_lbr_is_simple_ಅಗಲ(cts);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    cts->cts_with_trailing = len == MAXCOL;
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ ( ; *cts->cts_ptr != NUL && (len == MAXCOL || cts->cts_ptr < cts->cts_ಸಾಲು + len);
						      MB_PTR_ADV(cts->cts_ptr))
    {
#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
	ಆದರೆ (simple_ಅಗಲ)
	{
	    ಪೂರ್ಣಾಂಕ	c = *cts->cts_ptr;

	    ಆದರೆ (c < 0x80 && c != TAB)
		vcol += g_chartab[c] & CT_CELL_ಮುಖವಾಡ;
	    ಇಲ್ಲದಿದ್ದರೆ
		vcol += win_nolbr_chartabsize(cts, ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    vcol += win_lbr_chartabsize(cts, ಖಾಲಿ, ಖಾಲಿ);
	ಆದರೆ (vcol > MAXCOL)
	{
	    cts->cts_vcol = MAXCOL;
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    cts->cts_vcol = (ಪೂರ್ಣಾಂಕ)vcol;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // check for a virtual text at the end of a line or on an empty line
    ಆದರೆ (len == MAXCOL && cts->cts_has_prop_with_ಪಠ್ಯ && *cts->cts_ptr == NUL)
    {
	ಪೂರ್ಣಾಂಕ ತಲೆ = 0;
	(ಶೂನ್ಯ)win_lbr_chartabsize(cts, &ತಲೆ, ಖಾಲಿ);
	vcol += cts->cts_cur_ಪಠ್ಯ_ಅಗಲ + ತಲೆ;
	// When properties are above the empty line must also be counted.  For
	// a property below the width already includes filling up the line.
	ಆದರೆ (cts->cts_ptr == cts->cts_ಸಾಲು && cts->cts_prop_ಸಾಲುಗಳು > 0
							 && !cts->cts_has_ಕೆಳಗೆ)
	    ++vcol;
	cts->cts_vcol = vcol > MAXCOL ? MAXCOL : (ಪೂರ್ಣಾಂಕ)vcol;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return TRUE if 'c' is a normal identifier character:
 * Letters and characters from the 'isident' option.
 */
    ಪೂರ್ಣಾಂಕ
vim_isIDc(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ (c > 0 && c < 0x100 && (g_chartab[c] & CT_ID_CHAR));
}

/*
 * Like vim_isIDc() but not using the 'isident' option: letters, numbers and
 * underscore.
 */
    ಪೂರ್ಣಾಂಕ
vim_is_ಸಾಮಾನ್ಯ_IDc(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ ASCII_ISALNUM(c) || c == '_';
}

/*
 * return TRUE if 'c' is a keyword character: Letters and characters from
 * 'iskeyword' option for the current buffer.
 * For multi-byte characters mb_get_class() is used (builtin rules).
 */
    ಪೂರ್ಣಾಂಕ
vim_iswordc(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ vim_iswordc_buf(c, curbuf);
}

    ಪೂರ್ಣಾಂಕ
vim_iswordc_buf(ಪೂರ್ಣಾಂಕ c, buf_T *buf)
{
    ಆದರೆ (c >= 0x100)
    {
	ಆದರೆ (enc_dbcs != 0)
	    ಹಿಂತಿರುಗಿ dbcs_class((ಚಿಹ್ನೆರಹಿತ)c >> 8, (ಚಿಹ್ನೆರಹಿತ)(c & 0xff)) >= 2;
	ಆದರೆ (enc_utf8)
	    ಹಿಂತಿರುಗಿ utf_class_buf(c, buf) >= 2;
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ (c > 0 && GET_CHARTAB(buf, c) != 0);
}

/*
 * Just like vim_iswordc() but uses a pointer to the (multi-byte) character.
 */
    ಪೂರ್ಣಾಂಕ
vim_iswordp(char_u *p)
{
    ಹಿಂತಿರುಗಿ vim_iswordp_buf(p, curbuf);
}

    ಪೂರ್ಣಾಂಕ
vim_iswordp_buf(char_u *p, buf_T *buf)
{
    ಪೂರ್ಣಾಂಕ	c = *p;

    ಆದರೆ (has_mbyte && MB_BYTE2LEN(c) > 1)
	c = (*mb_ptr2char)(p);
    ಹಿಂತಿರುಗಿ vim_iswordc_buf(c, buf);
}

/*
 * Return TRUE if 'c' is a valid file-name character as specified with the
 * 'isfname' option.
 * Assume characters above 0x100 are valid (multi-byte).
 * To be used for commands like "gf".
 */
    ಪೂರ್ಣಾಂಕ
vim_isfilec(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_FNAME_CHAR)));
}

#ಆದರೆ defined(FEAT_SPELL)
/*
 * Return TRUE if 'c' is a valid file-name character, including characters left
 * out of 'isfname' to make "gf" work, such as comma, space, '@', etc.
 */
    ಪೂರ್ಣಾಂಕ
vim_is_fname_char(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ vim_isfilec(c) || c == ',' || c == ' ' || c == '@';
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * return TRUE if 'c' is a valid file-name character or a wildcard character
 * Assume characters above 0x100 are valid (multi-byte).
 * Explicitly interpret ']' as a wildcard character as mch_has_wildcard("]")
 * returns false.
 */
    ಪೂರ್ಣಾಂಕ
vim_isfilec_or_wc(ಪೂರ್ಣಾಂಕ c)
{
    char_u buf[2];

    buf[0] = (char_u)c;
    buf[1] = NUL;
    ಹಿಂತಿರುಗಿ vim_isfilec(c) || c == ']' || mch_has_wildcard(buf);
}

/*
 * Return TRUE if 'c' is a printable character.
 * Assume characters above 0x100 are printable (multi-byte), except for
 * Unicode.
 */
    ಪೂರ್ಣಾಂಕ
vim_isprintc(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (enc_utf8 && c >= 0x100)
	ಹಿಂತಿರುಗಿ utf_printable(c);
    ಹಿಂತಿರುಗಿ (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Strict version of vim_isprintc(c), don't return TRUE if "c" is the head
 * byte of a double-byte character.
 */
    ಪೂರ್ಣಾಂಕ
vim_isprintc_strict(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (enc_dbcs != 0 && c < 0x100 && MB_BYTE2LEN(c) > 1)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (enc_utf8 && c >= 0x100)
	ಹಿಂತಿರುಗಿ utf_printable(c);
    ಹಿಂತಿರುಗಿ (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Prepare the structure passed to chartabsize functions.
 * "line" is the start of the line, "ptr" is the first relevant character.
 * When "lnum" is zero do not use text properties that insert text.
 */
    ಶೂನ್ಯ
init_chartabsize_arg(
	chartabsize_T	*cts,
	win_T		*wp,
	linenr_T	lnum ಬಳಸದ,
	colnr_T		ಲಂಬಸಾಲು,
	char_u		*ಸಾಲು,
	char_u		*ptr)
{
    CLEAR_ಸೂಚಕ(cts);
    cts->cts_win = wp;
    cts->cts_vcol = ಲಂಬಸಾಲು;
    cts->cts_ಸಾಲು = ಸಾಲು;
    cts->cts_ptr = ptr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    cts->cts_bri_ಗಾತ್ರ = -1;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (lnum > 0 && !ignore_ಪಠ್ಯ_props)
    {
	char_u	*prop_ಆರಂಭ;
	ಪೂರ್ಣಾಂಕ	ಎಣಿಕೆ_2;

	ಎಣಿಕೆ_2 = get_ಪಠ್ಯ_props(wp->w_ಬಫರ್, lnum, &prop_ಆರಂಭ, FALSE);
	cts->cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ = ಎಣಿಕೆ_2;
	ಆದರೆ (ಎಣಿಕೆ_2 > 0)
	{
	    // Make a copy of the properties, so that they are properly
	    // aligned.  Make it twice as long for the sorting below.
	    cts->cts_ಪಠ್ಯ_props = ALLOC_MULT(textprop_T, ಎಣಿಕೆ_2 * 2);
	    ಆದರೆ (cts->cts_ಪಠ್ಯ_props == ಖಾಲಿ)
		cts->cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ	i;

		mch_memmove(cts->cts_ಪಠ್ಯ_props + ಎಣಿಕೆ_2, prop_ಆರಂಭ,
						   ಎಣಿಕೆ_2 * ಗಾತ್ರ(textprop_T));
		ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
		{
		    textprop_T *tp = cts->cts_ಪಠ್ಯ_props + i + ಎಣಿಕೆ_2;
		    ಆದರೆ (tp->tp_id < 0
				     && ಪಠ್ಯ_prop_type_valid(wp->w_ಬಫರ್, tp))
		    {
			cts->cts_has_prop_with_ಪಠ್ಯ = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
		}
		ಆದರೆ (!cts->cts_has_prop_with_ಪಠ್ಯ)
		{
		    // won't use the text properties, free them
		    VIM_CLEAR(cts->cts_ಪಠ್ಯ_props);
		    cts->cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ	    *ಪಠ್ಯ_prop_idxs;

		    // Need to sort the array to get any truncation right.
		    // Do the sorting in the second part of the array, then
		    // move the sorted props to the first part of the array.
		    ಪಠ್ಯ_prop_idxs = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, ಎಣಿಕೆ_2);
		    ಆದರೆ (ಪಠ್ಯ_prop_idxs != ಖಾಲಿ)
		    {
			ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
			    ಪಠ್ಯ_prop_idxs[i] = i + ಎಣಿಕೆ_2;
			ವಿಂಗಡಿಸು_ಪಠ್ಯ_props(curbuf, cts->cts_ಪಠ್ಯ_props,
							ಪಠ್ಯ_prop_idxs, ಎಣಿಕೆ_2);
			// Here we want the reverse order.
			ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
			    cts->cts_ಪಠ್ಯ_props[ಎಣಿಕೆ_2 - i - 1] =
					cts->cts_ಪಠ್ಯ_props[ಪಠ್ಯ_prop_idxs[i]];
			vim_free(ಪಠ್ಯ_prop_idxs);
		    }

		    // Convert tp_text_offset to tp_text pointer.
		    char_u *ಎಣಿಕೆ_ptr = prop_ಆರಂಭ - PROP_ಎಣಿಕೆ_ಗಾತ್ರ;

		    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
		    {
			textprop_T *tp = &cts->cts_ಪಠ್ಯ_props[i];

			ಆದರೆ (tp->tp_id < 0 && tp->u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ > 0)
			{
			    tp->u.tp_ಪಠ್ಯ = ಎಣಿಕೆ_ptr + tp->u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ;
			    tp->tp_flags |= TP_FLAG_VTEXT_PTR;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    tp->u.tp_ಪಠ್ಯ = ಖಾಲಿ;
		    }
		}
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Free any allocated item in "cts".
 */
    ಶೂನ್ಯ
clear_chartabsize_arg(chartabsize_T *cts ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (cts->cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ > 0)
    {
	VIM_CLEAR(cts->cts_ಪಠ್ಯ_props);
	cts->cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ = 0;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Like chartabsize(), but also check for line breaks on the screen and text
 * properties that insert text.
 */
    ಪೂರ್ಣಾಂಕ
lbr_chartabsize(chartabsize_T *cts)
{
#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ಆದರೆ (1
# ifdef FEAT_LINEBREAK
	&& !curwin->w_p_lbr && *get_showbreak_ಮೌಲ್ಯ(curwin) == NUL
							   && !curwin->w_p_bri
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_ಪಠ್ಯ
# endif
       )
    {
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (curwin->w_p_wrap)
	    ಹಿಂತಿರುಗಿ win_nolbr_chartabsize(cts, ಖಾಲಿ);
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, cts->cts_ptr, cts->cts_vcol)
#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    }
    ಹಿಂತಿರುಗಿ win_lbr_chartabsize(cts, ಖಾಲಿ, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Call lbr_chartabsize() and advance the pointer.
 */
    ಪೂರ್ಣಾಂಕ
lbr_chartabsize_adv(chartabsize_T *cts)
{
    ಪೂರ್ಣಾಂಕ		retval;

    retval = lbr_chartabsize(cts);
    MB_PTR_ADV(cts->cts_ptr);
    ಹಿಂತಿರುಗಿ retval;
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
    ಪೂರ್ಣಾಂಕ
win_lbr_chartabsize(
	chartabsize_T	*cts,
	ಪೂರ್ಣಾಂಕ		*headp ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ		*tailp ಬಳಸದ)
{
    win_T	*wp = cts->cts_win;
#ಆದರೆ defined(FEAT_PROP_POPUP) || defined(FEAT_LINEBREAK)
    char_u	*ಸಾಲು = cts->cts_ಸಾಲು; // start of the line
    ಪೂರ್ಣಾಂಕ		ಗಾತ್ರ_2;
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*s = cts->cts_ptr;
    colnr_T	vcol = cts->cts_vcol;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಪೂರ್ಣಾಂಕ		mb_added = 0;
    ಪೂರ್ಣಾಂಕ		n;
    char_u	*sbr;
    ಪೂರ್ಣಾಂಕ		no_sbr = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PROP_POPUP)
    cts->cts_cur_ಪಠ್ಯ_ಅಗಲ = 0;
    cts->cts_has_ಕೆಳಗೆ = false;
    cts->cts_ಮೊದಲ_char = 0;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    /*
     * No 'linebreak', 'showbreak', 'breakindent' and text properties that
     * insert text: return quickly.
     */
    ಆದರೆ (win_lbr_is_simple_ಅಗಲ(cts))
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಆದರೆ (wp->w_p_wrap)
	    ಹಿಂತಿರುಗಿ win_nolbr_chartabsize(cts, headp);
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_ಬಫರ್, s, vcol)
    }

#ಆದರೆ defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ಪೂರ್ಣಾಂಕ has_lcs_eol = wp->w_p_list && wp->w_lcs_chars.eol != NUL;
    // Virtual text above the line is on its own screen line, it does not count
    // for the size of a Tab.
    colnr_T tab_vcol = vcol;

# ifdef FEAT_PROP_POPUP
    tab_vcol -= cts->cts_above_ಅಗಲ;
# endif

    /*
     * First get the normal size, without 'linebreak' or text properties
     */
    ಗಾತ್ರ_2 = win_chartabsize(wp, s, tab_vcol);
# ifdef FEAT_LINEBREAK
    ಆದರೆ (*s == NUL)
    {
	// 1 cell for EOL list char (if present), as opposed to the two cell ^@
	// for a NUL character in the text.
	ಗಾತ್ರ_2 = has_lcs_eol ? 1 : 0;
    }

    ಪೂರ್ಣಾಂಕ is_doublewidth = has_mbyte && ಗಾತ್ರ_2 == 2 && MB_BYTE2LEN(*s) > 1;
# endif

# ifdef FEAT_PROP_POPUP
    ಆದರೆ (cts->cts_has_prop_with_ಪಠ್ಯ)
    {
#  ifdef FEAT_LINEBREAK
	ಪೂರ್ಣಾಂಕ	    tab_ಗಾತ್ರ = ಗಾತ್ರ_2;
#  endif
	ಪೂರ್ಣಾಂಕ	    charlen = *s == NUL ? 1 : mb_ptr2len(s);
	ಪೂರ್ಣಾಂಕ	    i;
	ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(s - ಸಾಲು);


	// The "$" for 'list' mode will go between the EOL and
	// the text prop, account for that.
	ಆದರೆ (has_lcs_eol)
	{
	    ++vcol;
	    --ಗಾತ್ರ_2;
	}

	ಗಾಗಿ (i = 0; i < cts->cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ; ++i)
	{
	    textprop_T	*tp = cts->cts_ಪಠ್ಯ_props + i;
	    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_off = win_ಲಂಬಸಾಲು_off(wp);

	    // Watch out for the text being deleted.  "cts_text_props" is a
	    // copy, the text prop may actually have been removed from the line.
	    ಆದರೆ (tp->tp_id < 0
		    && ((tp->tp_ಲಂಬಸಾಲು - 1 >= ಲಂಬಸಾಲು
					     && tp->tp_ಲಂಬಸಾಲು - 1 < ಲಂಬಸಾಲು + charlen)
		       || (tp->tp_ಲಂಬಸಾಲು == MAXCOL
			   && ((tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
				? ಲಂಬಸಾಲು == 0
				: s[0] == NUL && cts->cts_with_trailing)))
		    && tp->u.tp_ಪಠ್ಯ != ಖಾಲಿ)
	    {
		char_u *p = tp->u.tp_ಪಠ್ಯ;

		ಆದರೆ (p != ಖಾಲಿ)
		{
		    ಪೂರ್ಣಾಂಕ	cells;

		    ಆದರೆ (tp->tp_ಲಂಬಸಾಲು == MAXCOL)
		    {
			ಪೂರ್ಣಾಂಕ n_extra = (ಪೂರ್ಣಾಂಕ)STRLEN(p);

			cells = ಪಠ್ಯ_prop_position(wp, tp, vcol,
			     (vcol + ಗಾತ್ರ_2) % (wp->w_ಅಗಲ - ಲಂಬಸಾಲು_off) + ಲಂಬಸಾಲು_off,
					      &n_extra, &p, ಖಾಲಿ, ಖಾಲಿ, FALSE);
#  ifdef FEAT_LINEBREAK
			ಆದರೆ (ಪಠ್ಯ_prop_no_showbreak(tp))
			    no_sbr = TRUE;  // don't use 'showbreak' now
#  endif
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			cells = vim_strsize(p);
		    ಆದರೆ (tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
		    {
			cts->cts_ಮೊದಲ_char += cells;
			ಆದರೆ (!cts->cts_no_above)
			    cts->cts_cur_ಪಠ್ಯ_ಅಗಲ += cells;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			cts->cts_cur_ಪಠ್ಯ_ಅಗಲ += cells;
			ಗಾತ್ರ_2 += cells;
		    }
		    cts->cts_ಆರಂಭ_incl = tp->tp_flags & TP_FLAG_ಆರಂಭ_INCL;
#  ifdef FEAT_LINEBREAK
		    ಆದರೆ (*s == TAB)
		    {
			// tab size changes because of the inserted text
			ಗಾತ್ರ_2 -= tab_ಗಾತ್ರ;
			tab_ಗಾತ್ರ = win_chartabsize(wp, s,
					vcol + ಗಾತ್ರ_2 - cts->cts_above_ಅಗಲ);
			ಗಾತ್ರ_2 += tab_ಗಾತ್ರ;
		    }
#  endif
		    ಆದರೆ (tp->tp_ಲಂಬಸಾಲು == MAXCOL && (tp->tp_flags
				& (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_ಕೆಳಗೆ)))
		    {
			// count extra line for property above/below
			++cts->cts_prop_ಸಾಲುಗಳು;
			ಆದರೆ (tp->tp_flags & TP_FLAG_ALIGN_ಕೆಳಗೆ)
			    cts->cts_has_ಕೆಳಗೆ = true;
		    }
		}
	    }
	    ಆದರೆ (tp->tp_ಲಂಬಸಾಲು != MAXCOL && tp->tp_ಲಂಬಸಾಲು - 1 > ಲಂಬಸಾಲು)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (has_lcs_eol)
	{
	    --vcol;
	    ++ಗಾತ್ರ_2;
	}
    }
# endif

# ifdef FEAT_LINEBREAK
    ಆದರೆ (is_doublewidth && wp->w_p_wrap && ಒಳ_win_border(wp, vcol + ಗಾತ್ರ_2 - 2))
    {
	++ಗಾತ್ರ_2;		// Count the ">" in the last column.
	mb_added = 1;
    }

    /*
     * May have to add something for 'breakindent' and/or 'showbreak'
     * string at the start of a screen line.
     */
    ಪೂರ್ಣಾಂಕ ತಲೆ = mb_added;
    sbr = no_sbr ? ಖಾಲಿಗಳು_option : get_showbreak_ಮೌಲ್ಯ(wp);
    // When "size" is 0, no new screen line is started.
    ಆದರೆ (ಗಾತ್ರ_2 > 0 && wp->w_p_wrap && (*sbr != NUL || wp->w_p_bri))
    {
	ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು_off_prev = win_ಲಂಬಸಾಲು_off(wp);
	ಪೂರ್ಣಾಂಕ	width2 = wp->w_ಅಗಲ - ಲಂಬಸಾಲು_off_prev + win_ಲಂಬಸಾಲು_off2(wp);
	colnr_T	wcol = vcol + ಲಂಬಸಾಲು_off_prev;
#  ifdef FEAT_PROP_POPUP
	wcol -= wp->w_virtcol_ಮೊದಲ_char;
#  endif
	colnr_T	max_ತಲೆ_vcol = cts->cts_max_ತಲೆ_vcol;
	ಪೂರ್ಣಾಂಕ	added = 0;

	// cells taken by 'showbreak'/'breakindent' before current char
	ಪೂರ್ಣಾಂಕ	ತಲೆ_prev = 0;
	ಆದರೆ (wcol >= wp->w_ಅಗಲ)
	{
	    wcol -= wp->w_ಅಗಲ;
	    ಲಂಬಸಾಲು_off_prev = wp->w_ಅಗಲ - width2;
	    ಆದರೆ (wcol >= width2 && width2 > 0)
		wcol %= width2;
	    ಆದರೆ (*sbr != NUL)
		ತಲೆ_prev += vim_strsize(sbr);
	    ಆದರೆ (wp->w_p_bri)
	    {
		ಆದರೆ (cts->cts_bri_ಗಾತ್ರ < 0)
		    cts->cts_bri_ಗಾತ್ರ = get_breakindent_win(wp, ಸಾಲು);
		ತಲೆ_prev += cts->cts_bri_ಗಾತ್ರ;
	    }
	    ಆದರೆ (wcol < ತಲೆ_prev)
	    {
		ತಲೆ_prev -= wcol;
		wcol += ತಲೆ_prev;
		added += ತಲೆ_prev;
		ಆದರೆ (max_ತಲೆ_vcol <= 0 || vcol < max_ತಲೆ_vcol)
		    ತಲೆ += ತಲೆ_prev;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ತಲೆ_prev = 0;
	    wcol += ಲಂಬಸಾಲು_off_prev;
	}

	ಆದರೆ (wcol + ಗಾತ್ರ_2 > wp->w_ಅಗಲ)
	{
	    // cells taken by 'showbreak'/'breakindent' halfway current char
	    ಪೂರ್ಣಾಂಕ	ತಲೆ_mid = 0;
	    ಆದರೆ (*sbr != NUL)
		ತಲೆ_mid += vim_strsize(sbr);
	    ಆದರೆ (wp->w_p_bri)
	    {
		ಆದರೆ (cts->cts_bri_ಗಾತ್ರ < 0)
		    cts->cts_bri_ಗಾತ್ರ = get_breakindent_win(wp, ಸಾಲು);
		ತಲೆ_mid += cts->cts_bri_ಗಾತ್ರ;
	    }
	    ಆದರೆ (ತಲೆ_mid > 0)
	    {
		// Calculate effective window width.
		ಪೂರ್ಣಾಂಕ prev_rem = wp->w_ಅಗಲ - wcol;
		ಪೂರ್ಣಾಂಕ ಅಗಲ = width2 - ತಲೆ_mid;

		ಆದರೆ (ಅಗಲ <= 0)
		    ಅಗಲ = 1;
		// Divide "size - prev_rem" by "width", rounding up.
		ಪೂರ್ಣಾಂಕ cnt = (ಗಾತ್ರ_2 - prev_rem + ಅಗಲ - 1) / ಅಗಲ;
		added += cnt * ತಲೆ_mid;

		ಆದರೆ (max_ತಲೆ_vcol == 0 || vcol + ಗಾತ್ರ_2 + added < max_ತಲೆ_vcol)
		    ತಲೆ += cnt * ತಲೆ_mid;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (width2 > 0
			   && max_ತಲೆ_vcol > vcol + ತಲೆ_prev + prev_rem)
		    ತಲೆ += (max_ತಲೆ_vcol - (vcol + ತಲೆ_prev + prev_rem)
					     + width2 - 1) / width2 * ತಲೆ_mid;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (max_ತಲೆ_vcol < 0)
		{
		    ಪೂರ್ಣಾಂಕ off = mb_added;
#  ifdef FEAT_PROP_POPUP
		    ಆದರೆ (*s != NUL
			     && ((State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ) || cts->cts_ಆರಂಭ_incl))
			off += cts->cts_cur_ಪಠ್ಯ_ಅಗಲ;
#  endif
		    ಆದರೆ (off >= prev_rem)
			ತಲೆ += (1 + (off - prev_rem) / ಅಗಲ) * ತಲೆ_mid;
		}
	    }
	}

	ಗಾತ್ರ_2 += added;
    }

    ಆದರೆ (headp != ಖಾಲಿ)
	*headp = ತಲೆ;

    ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_before_lbr = ಗಾತ್ರ_2;
    ಪೂರ್ಣಾಂಕ need_lbr = FALSE;
    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here.
     */
    ಆದರೆ (wp->w_p_lbr && wp->w_p_wrap && wp->w_ಅಗಲ != 0
	    && VIM_ISBREAK((ಪೂರ್ಣಾಂಕ)s[0]) && !VIM_ISBREAK((ಪೂರ್ಣಾಂಕ)s[1]))
    {
	char_u	*t = cts->cts_ಸಾಲು;
	ಆಗ (VIM_ISBREAK((ಪೂರ್ಣಾಂಕ)t[0]))
	    t++;
	// 'linebreak' is only needed when not in leading whitespace.
	need_lbr = s >= t;
    }
    ಆದರೆ (need_lbr)
    {
	/*
	 * Count all characters from first non-blank after a blank up to next
	 * non-blank after a blank.
	 */
	ಪೂರ್ಣಾಂಕ numberextra = win_ಲಂಬಸಾಲು_off(wp);
	colnr_T ಲಂಬಸಾಲು_adj = ಗಾತ್ರ_2 - 1;
	colnr_T colmax = (colnr_T)(wp->w_ಅಗಲ - numberextra - ಲಂಬಸಾಲು_adj);
	ಆದರೆ (vcol >= colmax)
	{
	    colmax += ಲಂಬಸಾಲು_adj;
	    n = colmax +  win_ಲಂಬಸಾಲು_off2(wp);
	    ಆದರೆ (n > 0)
		colmax += (((vcol + ಲಂಬಸಾಲು_adj - colmax) / n) + 1) * n - ಲಂಬಸಾಲು_adj;
	}

	colnr_T vcol2 = vcol;
	ಗಾಗಿ (;;)
	{
	    char_u *ps = s;
	    MB_PTR_ADV(s);
	    ಪೂರ್ಣಾಂಕ c = *s;
	    ಆದರೆ (!(c != NUL
		    && (VIM_ISBREAK(c)
			|| (!VIM_ISBREAK(c)
			       && (vcol2 == vcol || !VIM_ISBREAK((ಪೂರ್ಣಾಂಕ)*ps))))))
		ನಿಲ್ಲಿಸು;

	    vcol2 += win_chartabsize(wp, s, vcol2);
	    ಆದರೆ (vcol2 >= colmax)		// doesn't fit
	    {
		ಗಾತ್ರ_2 = colmax - vcol + ಲಂಬಸಾಲು_adj;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (tailp != ಖಾಲಿ)
	*tailp = ಗಾತ್ರ_2 - ಗಾತ್ರ_before_lbr;

#  ifdef FEAT_PROP_POPUP
    ಆದರೆ (cts->cts_ಮೊದಲ_char > 0 && !cts->cts_no_above)
    {
	// Remember the width for the size of a Tab later in the line.  Use
	// assignment, this may be called more than once for a character.
	cts->cts_above_ಅಗಲ = cts->cts_ಮೊದಲ_char;
	ಗಾತ್ರ_2 += cts->cts_ಮೊದಲ_char;
    }
#  endif
# endif
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Like win_lbr_chartabsize(), except that we know 'linebreak' is off, 'wrap'
 * is on and there are no properties that insert text.  This means we need to
 * check for a double-byte character that doesn't fit at the end of the screen
 * line.
 * Only uses "cts_win", "cts_ptr" and "cts_vcol" from "cts".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win_nolbr_chartabsize(
	chartabsize_T	*cts,
	ಪೂರ್ಣಾಂಕ		*headp)
{
    win_T	*wp = cts->cts_win;
    char_u	*s = cts->cts_ptr;
    colnr_T	ಲಂಬಸಾಲು = cts->cts_vcol;
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (*s == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	ಹಿಂತಿರುಗಿ tabstop_padding(ಲಂಬಸಾಲು, wp->w_ಬಫರ್->b_p_ts,
				    wp->w_ಬಫರ್->b_p_vts_array);
#ಇಲ್ಲದಿದ್ದರೆ
	n = wp->w_ಬಫರ್->b_p_ts;
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(n - (ಲಂಬಸಾಲು % n));
#ಆದರೆ_ಅಂತ್ಯ
    }
    n = ptr2cells(s);
    // Add one cell for a double-width character in the last column of the
    // window, displayed with a ">".
    ಆದರೆ (n == 2 && MB_BYTE2LEN(*s) > 1 && ಒಳ_win_border(wp, ಲಂಬಸಾಲು))
    {
	ಆದರೆ (headp != ಖಾಲಿ)
	    *headp = 1;
	ಹಿಂತಿರುಗಿ 3;
    }
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Return TRUE if virtual column "vcol" is in the rightmost column of window
 * "wp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಒಳ_win_border(win_T *wp, colnr_T vcol)
{
    ಪೂರ್ಣಾಂಕ		width1;		// width of first line (after line number)
    ಪೂರ್ಣಾಂಕ		width2;		// width of further lines

    ಆದರೆ (wp->w_ಅಗಲ == 0)	// there is no border
	ಹಿಂತಿರುಗಿ FALSE;
    width1 = wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp);
    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)vcol < width1 - 1)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)vcol == width1 - 1)
	ಹಿಂತಿರುಗಿ TRUE;
    width2 = width1 + win_ಲಂಬಸಾಲು_off2(wp);
    ಆದರೆ (width2 <= 0)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ ((vcol - width1) % width2 == width2 - 1);
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
    ಶೂನ್ಯ
getvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*ಆರಂಭ,
    colnr_T	*cursor,
    colnr_T	*ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		flags)
{
    colnr_T	vcol;
    char_u	*ptr;		// points to current char
    char_u	*ಸಾಲು;		// start of the line
    ಪೂರ್ಣಾಂಕ		incr;
    ಪೂರ್ಣಾಂಕ		ತಲೆ;
    ಪೂರ್ಣಾಂಕ		ಬಾಲ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಪೂರ್ಣಾಂಕ		*vts = wp->w_ಬಫರ್->b_p_vts_array;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ts = wp->w_ಬಫರ್->b_p_ts;
    ಪೂರ್ಣಾಂಕ		c;
    chartabsize_T cts;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಪೂರ್ಣಾಂಕ		on_NUL = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    vcol = 0;
    ಸಾಲು = ptr = ml_get_buf(wp->w_ಬಫರ್, pos->lnum, FALSE);

    init_chartabsize_arg(&cts, wp, pos->lnum, 0, ಸಾಲು, ಸಾಲು);
    cts.cts_max_ತಲೆ_vcol = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    cts.cts_no_above = true;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * This function is used very often, do some speed optimizations.
     * When 'list', 'linebreak', 'showbreak' and 'breakindent' are not set
     * and there are no text properties with "text" use a simple loop.
     * Also use this when 'list' is set but tabs take their normal size.
     */
    ಆದರೆ ((!wp->w_p_list || wp->w_lcs_chars.tab1 != NUL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	    && !wp->w_p_lbr && *get_showbreak_ಮೌಲ್ಯ(wp) == NUL && !wp->w_p_bri
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_ಪಠ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	ಗಾಗಿ (;;)
	{
	    ತಲೆ = 0;
	    c = *ptr;
	    // make sure we don't go past the end of the line
	    ಆದರೆ (c == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
		ನಿಲ್ಲಿಸು;
	    }
	    // A tab gets expanded, depending on the current column
	    ಆದರೆ (c == TAB)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		incr = tabstop_padding(vcol, ts, vts);
#ಇಲ್ಲದಿದ್ದರೆ
		incr = ts - (vcol % ts);
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (has_mbyte)
		{
		    // For utf-8, if the byte is >= 0x80, need to look at
		    // further bytes to find the cell width.
		    ಆದರೆ (enc_utf8 && c >= 0x80)
			incr = utf_ptr2cells(ptr);
		    ಇಲ್ಲದಿದ್ದರೆ
			incr = g_chartab[c] & CT_CELL_ಮುಖವಾಡ;

		    // If a double-cell char doesn't fit at the end of a line
		    // it wraps to the next line, it's like this char is three
		    // cells wide.
		    ಆದರೆ (incr == 2 && wp->w_p_wrap && MB_BYTE2LEN(*ptr) > 1
			    && ಒಳ_win_border(wp, vcol))
		    {
			++incr;
			ತಲೆ = 1;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    incr = g_chartab[c] & CT_CELL_ಮುಖವಾಡ;
	    }

	    char_u *ಮುಂದಿನ_ptr = ptr + (*mb_ptr2len)(ptr);
	    ಆದರೆ (ಮುಂದಿನ_ptr - ಸಾಲು > pos->ಲಂಬಸಾಲು) // character at pos->col
		ನಿಲ್ಲಿಸು;

	    vcol += incr;
	    ptr = ಮುಂದಿನ_ptr;
	}

	ಬಾಲ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (;;)
	{
	    // A tab gets expanded, depending on the current column.
	    // Other things also take up space.
	    ತಲೆ = 0;
	    ಬಾಲ = 0;
	    incr = win_lbr_chartabsize(&cts, &ತಲೆ, &ಬಾಲ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    ಆದರೆ (cts.cts_ptr == cts.cts_ಸಾಲು)
	    {
		ಆದರೆ (flags & GETVCOL_FOR_VIRTCOL)
		    // do not count the virtual text above for w_curswant
		    wp->w_virtcol_ಮೊದಲ_char = cts.cts_ಮೊದಲ_char;
		ಆದರೆ ((flags & GETVCOL_NO_ABOVE) == 0)
		{
		    cts.cts_vcol += cts.cts_ಮೊದಲ_char;
		    cts.cts_above_ಅಗಲ = cts.cts_ಮೊದಲ_char;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    // make sure we don't go past the end of the line
	    ಆದರೆ (*cts.cts_ptr == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		incr += cts.cts_cur_ಪಠ್ಯ_ಅಗಲ;
		on_NUL = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    }

	    char_u *ಮುಂದಿನ_ptr = cts.cts_ptr + (*mb_ptr2len)(cts.cts_ptr);
	    ಆದರೆ (ಮುಂದಿನ_ptr - ಸಾಲು > pos->ಲಂಬಸಾಲು) // character at pos->col
		ನಿಲ್ಲಿಸು;

	    cts.cts_vcol += incr;
	    cts.cts_ptr = ಮುಂದಿನ_ptr;
	}
	vcol = cts.cts_vcol;
	ptr = cts.cts_ptr;
    }
    clear_chartabsize_arg(&cts);

    ಆದರೆ (*ptr == NUL && pos->ಲಂಬಸಾಲು < MAXCOL && pos->ಲಂಬಸಾಲು > ptr - ಸಾಲು)
	pos->ಲಂಬಸಾಲು = ptr - ಸಾಲು;

    ಆದರೆ (ಆರಂಭ != ಖಾಲಿ)
	*ಆರಂಭ = vcol + ತಲೆ;
    ಆದರೆ (ಕೊನೆ != ಖಾಲಿ)
	*ಕೊನೆ = vcol + incr - (flags & GETVCOL_ಕೊನೆ_EXCL_LBR ? ಬಾಲ : 0) - 1;
    ಆದರೆ (cursor != ಖಾಲಿ)
    {
	ಆದರೆ (*ptr == TAB
		&& (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
		&& !wp->w_p_list
		&& !virtual_ಸಕ್ರಿಯ()
		&& !(VIsual_ಸಕ್ರಿಯ
				&& (*p_sel == 'e' || LTOREQ_POS(*pos, VIsual)))
		)
	    // TODO: subtracting "tail" may lead to better cursor position
	    *cursor = vcol + incr - 1;	    // cursor at end
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    // in Insert mode, if "start_incl" is true the text gets inserted
	    // after the virtual text, thus add its width
	    ಆದರೆ (((State & ಬಹುಲಕ_ಸೇರಿಸು) == 0 || cts.cts_ಆರಂಭ_incl) && !on_NUL)
		// cursor is after inserted text, unless on the NUL
		vcol += cts.cts_cur_ಪಠ್ಯ_ಅಗಲ;
#ಆದರೆ_ಅಂತ್ಯ
	    *cursor = vcol + ತಲೆ;	    // cursor at start
	}
    }
}

/*
 * Get virtual cursor column in the current window, pretending 'list' is off.
 */
    colnr_T
getvcol_nolist(pos_T *posp)
{
    ಪೂರ್ಣಾಂಕ		list_save = curwin->w_p_list;
    colnr_T	vcol;

    curwin->w_p_list = FALSE;
    ಆದರೆ (posp->coladd)
	getvvcol(curwin, posp, ಖಾಲಿ, &vcol, ಖಾಲಿ, 0);
    ಇಲ್ಲದಿದ್ದರೆ
	getvcol(curwin, posp, ಖಾಲಿ, &vcol, ಖಾಲಿ, 0);
    curwin->w_p_list = list_save;
    ಹಿಂತಿರುಗಿ vcol;
}

/*
 * Get virtual column in virtual mode.
 */
    ಶೂನ್ಯ
getvvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*ಆರಂಭ,
    colnr_T	*cursor,
    colnr_T	*ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		flags)
{
    colnr_T	ಲಂಬಸಾಲು;
    colnr_T	coladd;
    colnr_T	endadd;
    char_u	*ptr;

    ಆದರೆ (virtual_ಸಕ್ರಿಯ())
    {
	// For virtual mode, only want one value
	getvcol(wp, pos, &ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, flags);

	coladd = pos->coladd;
	endadd = 0;
	// Cannot put the cursor on part of a wide character.
	ptr = ml_get_buf(wp->w_ಬಫರ್, pos->lnum, FALSE);
	ಆದರೆ (pos->ಲಂಬಸಾಲು < ml_get_buf_len(wp->w_ಬಫರ್, pos->lnum))
	{
	    ಪೂರ್ಣಾಂಕ c = (*mb_ptr2char)(ptr + pos->ಲಂಬಸಾಲು);

	    ಆದರೆ (c != TAB && vim_isprintc(c))
	    {
		endadd = (colnr_T)(char2cells(c) - 1);
		ಆದರೆ (coladd > endadd)	// past end of line
		    endadd = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    coladd = 0;
	    }
	}
	ಲಂಬಸಾಲು += coladd;
	ಆದರೆ (ಆರಂಭ != ಖಾಲಿ)
	    *ಆರಂಭ = ಲಂಬಸಾಲು;
	ಆದರೆ (cursor != ಖಾಲಿ)
	    *cursor = ಲಂಬಸಾಲು;
	ಆದರೆ (ಕೊನೆ != ಖಾಲಿ)
	    *ಕೊನೆ = ಲಂಬಸಾಲು + endadd;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	getvcol(wp, pos, ಆರಂಭ, cursor, ಕೊನೆ, flags);
}

/*
 * Get the leftmost and rightmost virtual column of pos1 and pos2.
 * Used for Visual block mode.
 */
    ಶೂನ್ಯ
getvcols(
    win_T	*wp,
    pos_T	*pos1,
    pos_T	*pos2,
    colnr_T	*ಎಡ_2,
    colnr_T	*ಬಲ_2,
    ಪೂರ್ಣಾಂಕ		flags)
{
    colnr_T	from1, from2, to1, to2;

    ಆದರೆ (LT_POSP(pos1, pos2))
    {
	getvvcol(wp, pos1, &from1, ಖಾಲಿ, &to1, flags);
	getvvcol(wp, pos2, &from2, ಖಾಲಿ, &to2, flags);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	getvvcol(wp, pos2, &from1, ಖಾಲಿ, &to1, flags);
	getvvcol(wp, pos1, &from2, ಖಾಲಿ, &to2, flags);
    }
    ಆದರೆ (from2 < from1)
	*ಎಡ_2 = from2;
    ಇಲ್ಲದಿದ್ದರೆ
	*ಎಡ_2 = from1;
    ಆದರೆ (to2 > to1)
    {
	ಆದರೆ (*p_sel == 'e' && from2 - 1 >= to1)
	    *ಬಲ_2 = from2 - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    *ಬಲ_2 = to2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	*ಬಲ_2 = to1;
}

/*
 * Skip over ' ' and '\t'.
 */
    char_u *
skipwhite(char_u *q)
{
    char_u	*p = q;

    ಆಗ (VIM_ISWHITE(*p))
	++p;
    ಹಿಂತಿರುಗಿ p;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * skip over ' ', '\t' and '\n'.
 */
    char_u *
skipwhite_and_nl(char_u *q)
{
    char_u	*p = q;

    ಆಗ (VIM_ISWHITE(*p) || *p == NL)
	++p;
    ಹಿಂತಿರುಗಿ p;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * getwhitecols: return the number of whitespace
 * columns (bytes) at the start of a given line
 */
    ಪೂರ್ಣಾಂಕ
getwhitecols_curline(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ getwhitecols(ml_get_curline());
}

    ಪೂರ್ಣಾಂಕ
getwhitecols(char_u *p)
{
    ಹಿಂತಿರುಗಿ skipwhite(p) - p;
}

/*
 * skip over digits
 */
    char_u *
skipdigits(char_u *q)
{
    char_u	*p = q;

    ಆಗ (VIM_ISDIGIT(*p))	// skip to next non-digit
	++p;
    ಹಿಂತಿರುಗಿ p;
}

#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * skip over binary digits
 */
    char_u *
skipbin(char_u *q)
{
    char_u	*p = q;

    ಆಗ (vim_isbdigit(*p))	// skip to next non-digit
	++p;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * skip over digits and hex characters
 */
    char_u *
skiphex(char_u *q)
{
    char_u	*p = q;

    ಆಗ (vim_isxdigit(*p))	// skip to next non-digit
	++p;
    ಹಿಂತಿರುಗಿ p;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * skip to bin digit (or NUL after the string)
 */
    char_u *
skiptobin(char_u *q)
{
    char_u	*p = q;

    ಆಗ (*p != NUL && !vim_isbdigit(*p))	// skip to next digit
	++p;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * skip to digit (or NUL after the string)
 */
    char_u *
skiptodigit(char_u *q)
{
    char_u	*p = q;

    ಆಗ (*p != NUL && !VIM_ISDIGIT(*p))	// skip to next digit
	++p;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * skip to hex character (or NUL after the string)
 */
    char_u *
skiptohex(char_u *q)
{
    char_u	*p = q;

    ಆಗ (*p != NUL && !vim_isxdigit(*p))	// skip to next digit
	++p;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Variant of isdigit() that can handle characters > 0x100.
 * We don't use isdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 * Use the VIM_ISDIGIT() macro for simple arguments.
 */
    ಪೂರ್ಣಾಂಕ
vim_isdigit(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ (c >= '0' && c <= '9');
}

/*
 * Variant of isxdigit() that can handle characters > 0x100.
 * We don't use isxdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 */
    ಪೂರ್ಣಾಂಕ
vim_isxdigit(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'f')
	|| (c >= 'A' && c <= 'F');
}

/*
 * Corollary of vim_isdigit and vim_isxdigit() that can handle
 * characters > 0x100.
 */
    ಪೂರ್ಣಾಂಕ
vim_isbdigit(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ (c == '0' || c == '1');
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vim_isodigit(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ (c >= '0' && c <= '7');
}

/*
 * Vim's own character class functions.  These exist because many library
 * islower()/toupper() etc. do not work properly: they crash when used with
 * invalid values or can't handle latin1 when the locale is C.
 * Speed is most important here.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು LATIN1_ಅಧೋ 'l'
#ವ್ಯಾಖ್ಯಾನಿಸು LATIN1_ಊರ್ಧ್ವ 'U'

ಸ್ಥಿರ char_u latin1flags[257] = "                                                                 UUUUUUUUUUUUUUUUUUUUUUUUUU      llllllllllllllllllllllllll                                                                     UUUUUUUUUUUUUUUUUUUUUUU UUUUUUUllllllllllllllllllllllll llllllll";
ಸ್ಥಿರ char_u latin1upper[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xf7\xd8\xd9\xda\xdb\xdc\xdd\xde\xff";
ಸ್ಥಿರ char_u latin1lower[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xd7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    ಪೂರ್ಣಾಂಕ
vim_islower(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c <= '@')
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (c >= 0x80)
    {
	ಆದರೆ (enc_utf8)
	    ಹಿಂತಿರುಗಿ utf_islower(c);
	ಆದರೆ (c >= 0x100)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ISWLOWER
	    ಆದರೆ (has_mbyte)
		ಹಿಂತಿರುಗಿ iswlower(c);
#ಆದರೆ_ಅಂತ್ಯ
	    // islower() can't handle these chars and may crash
	    ಹಿಂತಿರುಗಿ FALSE;
	}
	ಆದರೆ (enc_latin1like)
	    ಹಿಂತಿರುಗಿ (latin1flags[c] & LATIN1_ಅಧೋ) == LATIN1_ಅಧೋ;
    }
    ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_islower(c);
}

    ಪೂರ್ಣಾಂಕ
vim_isupper(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c <= '@')
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (c >= 0x80)
    {
	ಆದರೆ (enc_utf8)
	    ಹಿಂತಿರುಗಿ utf_isupper(c);
	ಆದರೆ (c >= 0x100)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ISWUPPER
	    ಆದರೆ (has_mbyte)
		ಹಿಂತಿರುಗಿ iswupper(c);
#ಆದರೆ_ಅಂತ್ಯ
	    // islower() can't handle these chars and may crash
	    ಹಿಂತಿರುಗಿ FALSE;
	}
	ಆದರೆ (enc_latin1like)
	    ಹಿಂತಿರುಗಿ (latin1flags[c] & LATIN1_ಊರ್ಧ್ವ) == LATIN1_ಊರ್ಧ್ವ;
    }
    ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_isupper(c);
}

    ಪೂರ್ಣಾಂಕ
vim_isalpha(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ vim_islower(c) || vim_isupper(c);
}

    ಪೂರ್ಣಾಂಕ
vim_toupper(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c <= '@')
	ಹಿಂತಿರುಗಿ c;
    ಆದರೆ (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	ಆದರೆ (enc_utf8)
	    ಹಿಂತಿರುಗಿ utf_toupper(c);
	ಆದರೆ (c >= 0x100)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TOWUPPER
	    ಆದರೆ (has_mbyte)
		ಹಿಂತಿರುಗಿ towupper(c);
#ಆದರೆ_ಅಂತ್ಯ
	    // toupper() can't handle these chars and may crash
	    ಹಿಂತಿರುಗಿ c;
	}
	ಆದರೆ (enc_latin1like)
	    ಹಿಂತಿರುಗಿ latin1upper[c];
    }
    ಆದರೆ (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ಹಿಂತಿರುಗಿ TOUPPER_ASC(c);
    ಹಿಂತಿರುಗಿ TOUPPER_LOC(c);
}

    ಪೂರ್ಣಾಂಕ
vim_tolower(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c <= '@')
	ಹಿಂತಿರುಗಿ c;
    ಆದರೆ (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	ಆದರೆ (enc_utf8)
	    ಹಿಂತಿರುಗಿ utf_tolower(c);
	ಆದರೆ (c >= 0x100)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TOWLOWER
	    ಆದರೆ (has_mbyte)
		ಹಿಂತಿರುಗಿ towlower(c);
#ಆದರೆ_ಅಂತ್ಯ
	    // tolower() can't handle these chars and may crash
	    ಹಿಂತಿರುಗಿ c;
	}
	ಆದರೆ (enc_latin1like)
	    ಹಿಂತಿರುಗಿ latin1lower[c];
    }
    ಆದರೆ (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ಹಿಂತಿರುಗಿ TOLOWER_ASC(c);
    ಹಿಂತಿರುಗಿ TOLOWER_LOC(c);
}

/*
 * skiptowhite: skip over text until ' ' or '\t' or NUL.
 */
    char_u *
skiptowhite(char_u *p)
{
    ಆಗ (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * skiptowhite_esc: Like skiptowhite(), but also skip escaped chars
 */
    char_u *
skiptowhite_esc(char_u *p)
{
    ಆಗ (*p != ' ' && *p != '\t' && *p != NUL)
    {
	ಆದರೆ ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Get a number from a string and skip over it.
 * Note: the argument is a pointer to a char_u pointer!
 */
    ದೀರ್ಘ
getdigits(char_u **pp)
{
    char_u	*p;
    ದೀರ್ಘ	retval;

    p = *pp;
    retval = strtol((ಅಕ್ಷರ *)p, ಖಾಲಿ, 10);
    ಆದರೆ (*p == '-')		// skip negative sign
	++p;
    p = skipdigits(p);		// skip to next non-digit
    *pp = p;
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Like getdigits() but allow for embedded single quotes.
 */
    ದೀರ್ಘ
getdigits_quoted(char_u **pp)
{
    char_u	*p = *pp;
    ದೀರ್ಘ	retval = 0;

    ಆದರೆ (*p == '-')
	++p;
    ಆಗ (VIM_ISDIGIT(*p))
    {
	ಆದರೆ (retval >= LONG_MAX / 10 - 10)
	    retval = LONG_MAX;
	ಇಲ್ಲದಿದ್ದರೆ
	    retval = retval * 10 - '0' + *p;
	++p;
	ಆದರೆ (ಒಳ_vim9script() && *p == '\'' && VIM_ISDIGIT(p[1]))
	    ++p;
    }
    ಆದರೆ (**pp == '-')
    {
	ಆದರೆ (retval == LONG_MAX)
	    retval = LONG_MIN;
	ಇಲ್ಲದಿದ್ದರೆ
	    retval = -retval;
    }
    *pp = p;
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return TRUE if "lbuf" is empty or only contains blanks.
 */
    ಪೂರ್ಣಾಂಕ
vim_isblankline(char_u *lbuf)
{
    char_u	*p;

    p = skipwhite(lbuf);
    ಹಿಂತಿರುಗಿ (*p == NUL || *p == '\r' || *p == '\n');
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
    ಶೂನ್ಯ
vim_str2nr(
    char_u		*ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ			*prep,	    // return: type of number 0 = decimal, 'x'
				    // or 'X' is hex, '0', 'o' or 'O' is octal,
				    // 'b' or 'B' is bin
    ಪೂರ್ಣಾಂಕ			*len,	    // return: detected length of number
    ಪೂರ್ಣಾಂಕ			what,	    // what numbers to recognize
    varnumber_T		*nptr,	    // return: signed result
    uvarnumber_T	*unptr,	    // return: unsigned result
    ಪೂರ್ಣಾಂಕ			maxlen,     // max length of string to check
    ಪೂರ್ಣಾಂಕ			strict,     // check strictly
    ಪೂರ್ಣಾಂಕ			*ಅತಿಪ್ರವಾಹ)  // when not NULL set to TRUE for overflow
{
    char_u	    *ptr = ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		    pre = 0;		// default is decimal
    ಪೂರ್ಣಾಂಕ		    negative = FALSE;
    uvarnumber_T    un = 0;
    ಪೂರ್ಣಾಂಕ		    n;

    ಆದರೆ (len != ಖಾಲಿ)
	*len = 0;

    ಆದರೆ (ptr[0] == '-')
    {
	negative = TRUE;
	++ptr;
    }

    // Recognize hex, octal, and bin.
    ಆದರೆ (ptr[0] == '0' && ptr[1] != '8' && ptr[1] != '9'
					       && (maxlen == 0 || maxlen > 1))
    {
	pre = ptr[1];
	ಆದರೆ ((what & STR2NR_HEX)
		&& (pre == 'X' || pre == 'x') && vim_isxdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // hexadecimal
	    ptr += 2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((what & STR2NR_BIN)
		&& (pre == 'B' || pre == 'b') && vim_isbdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    //ದ್ವಿಮಾನ
	    ptr += 2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((what & STR2NR_OOCT)
		&& (pre == 'O' || pre == 'o') && vim_isodigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // octal with prefix "0o"
	    ptr += 2;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // decimal or octal, default is decimal
	    pre = 0;
	    ಆದರೆ (what & STR2NR_OCT)
	    {
		// Don't interpret "0", "08" or "0129" as octal.
		ಗಾಗಿ (n = 1; n != maxlen && VIM_ISDIGIT(ptr[n]); ++n)
		{
		    ಆದರೆ (ptr[n] > '7')
		    {
			pre = 0;	// can't be octal
			ನಿಲ್ಲಿಸು;
		    }
		    pre = '0';	// assume octal
		}
	    }
	}
    }

    // Do the conversion manually to avoid sscanf() quirks.
    n = 1;
    ಆದರೆ (pre == 'B' || pre == 'b'
			     || ((what & STR2NR_BIN) && (what & STR2NR_FORCE)))
    {
	// bin
	ಆದರೆ (pre != 0)
	    n += 2;	    // skip over "0b"
	ಆಗ ('0' <= *ptr && *ptr <= '1')
	{
	    // avoid ubsan error for overflow
	    ಆದರೆ (un <= UVARNUM_MAX / 2)
		un = 2 * un + (uvarnumber_T)(*ptr - '0');
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		un = UVARNUM_MAX;
		ಆದರೆ (ಅತಿಪ್ರವಾಹ != ಖಾಲಿ)
		    *ಅತಿಪ್ರವಾಹ = TRUE;
	    }
	    ++ptr;
	    ಆದರೆ (n++ == maxlen)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '1')
	    {
		++ptr;
		ಆದರೆ (n++ == maxlen)
		    ನಿಲ್ಲಿಸು;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pre == 'O' || pre == 'o' ||
		pre == '0' || ((what & STR2NR_OCT) && (what & STR2NR_FORCE)))
    {
	// octal
	ಆದರೆ (pre != 0 && pre != '0')
	    n += 2;	    // skip over "0o"
	ಆಗ ('0' <= *ptr && *ptr <= '7')
	{
	    // avoid ubsan error for overflow
	    ಆದರೆ (un <= UVARNUM_MAX / 8)
		un = 8 * un + (uvarnumber_T)(*ptr - '0');
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		un = UVARNUM_MAX;
		ಆದರೆ (ಅತಿಪ್ರವಾಹ != ಖಾಲಿ)
		    *ಅತಿಪ್ರವಾಹ = TRUE;
	    }
	    ++ptr;
	    ಆದರೆ (n++ == maxlen)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '7')
	    {
		++ptr;
		ಆದರೆ (n++ == maxlen)
		    ನಿಲ್ಲಿಸು;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pre != 0 || ((what & STR2NR_HEX) && (what & STR2NR_FORCE)))
    {
	// hex
	ಆದರೆ (pre != 0)
	    n += 2;	    // skip over "0x"
	ಆಗ (vim_isxdigit(*ptr))
	{
	    // avoid ubsan error for overflow
	    ಆದರೆ (un <= UVARNUM_MAX / 16)
		un = 16 * un + (uvarnumber_T)hex2nr(*ptr);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		un = UVARNUM_MAX;
		ಆದರೆ (ಅತಿಪ್ರವಾಹ != ಖಾಲಿ)
		    *ಅತಿಪ್ರವಾಹ = TRUE;
	    }
	    ++ptr;
	    ಆದರೆ (n++ == maxlen)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((what & STR2NR_QUOTE) && *ptr == '\'' && vim_isxdigit(ptr[1]))
	    {
		++ptr;
		ಆದರೆ (n++ == maxlen)
		    ನಿಲ್ಲಿಸು;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// decimal
	ಆಗ (VIM_ISDIGIT(*ptr))
	{
	    uvarnumber_T    digit = (uvarnumber_T)(*ptr - '0');

	    // avoid ubsan error for overflow
	    ಆದರೆ (un < UVARNUM_MAX / 10
		    || (un == UVARNUM_MAX / 10 && digit <= UVARNUM_MAX % 10))
		un = 10 * un + digit;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		un = UVARNUM_MAX;
		ಆದರೆ (ಅತಿಪ್ರವಾಹ != ಖಾಲಿ)
		    *ಅತಿಪ್ರವಾಹ = TRUE;
	    }
	    ++ptr;
	    ಆದರೆ (n++ == maxlen)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((what & STR2NR_QUOTE) && *ptr == '\'' && VIM_ISDIGIT(ptr[1]))
	    {
		++ptr;
		ಆದರೆ (n++ == maxlen)
		    ನಿಲ್ಲಿಸು;
	    }
	}
    }

    // Check for an alphanumeric character immediately following, that is
    // most likely a typo.
    ಆದರೆ (strict && n - 1 != maxlen && ASCII_ISALNUM(*ptr))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (prep != ಖಾಲಿ)
	*prep = pre;
    ಆದರೆ (len != ಖಾಲಿ)
	*len = (ಪೂರ್ಣಾಂಕ)(ptr - ಆರಂಭ);
    ಆದರೆ (nptr != ಖಾಲಿ)
    {
	ಆದರೆ (negative)   // account for leading '-' for decimal numbers
	{
	    // avoid ubsan error for overflow
	    ಆದರೆ (un > VARNUM_MAX)
	    {
		*nptr = VARNUM_MIN;
		ಆದರೆ (ಅತಿಪ್ರವಾಹ != ಖಾಲಿ)
		    *ಅತಿಪ್ರವಾಹ = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		*nptr = -(varnumber_T)un;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // prevent a large unsigned number to become negative
	    ಆದರೆ (un > VARNUM_MAX)
	    {
		un = VARNUM_MAX;
		ಆದರೆ (ಅತಿಪ್ರವಾಹ != ಖಾಲಿ)
		    *ಅತಿಪ್ರವಾಹ = TRUE;
	    }
	    *nptr = (varnumber_T)un;
	}
    }
    ಆದರೆ (unptr != ಖಾಲಿ)
	*unptr = un;
}

/*
 * Return the value of a single hex character.
 * Only valid when the argument is '0' - '9', 'A' - 'F' or 'a' - 'f'.
 */
    ಪೂರ್ಣಾಂಕ
hex2nr(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c >= 'a' && c <= 'f')
	ಹಿಂತಿರುಗಿ c - 'a' + 10;
    ಆದರೆ (c >= 'A' && c <= 'F')
	ಹಿಂತಿರುಗಿ c - 'A' + 10;
    ಹಿಂತಿರುಗಿ c - '0';
}

/*
 * Convert two hex characters to a byte.
 * Return -1 if one of the characters is not hex.
 */
    ಪೂರ್ಣಾಂಕ
hexhex2nr(char_u *p)
{
    ಆದರೆ (!vim_isxdigit(p[0]) || !vim_isxdigit(p[1]))
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ (hex2nr(p[0]) << 4) + hex2nr(p[1]);
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
    ಪೂರ್ಣಾಂಕ
rem_backslash(char_u *str)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    ಹಿಂತಿರುಗಿ (str[0] == '\\'
	    && str[1] < 0x80
	    && (str[1] == ' '
		|| (str[1] != NUL
		    && str[1] != '*'
		    && str[1] != '?'
		    && !vim_isfilec(str[1]))));
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (str[0] == '\\' && str[1] != NUL);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Halve the number of backslashes in a file name argument.
 * For MS-DOS we only do this if the character after the backslash
 * is not a normal file character.
 */
    ಶೂನ್ಯ
backslash_halve(char_u *p)
{
    ಗಾಗಿ ( ; *p; ++p)
	ಆದರೆ (rem_backslash(p))
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
    ಆದರೆ (res == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ p;
    backslash_halve(res);
    ಹಿಂತಿರುಗಿ res;
}
