/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_isopt(char_u *var, buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା only_check);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା win_nolbr_chartabsize(chartabsize_T *cts, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *headp);
ସ୍ଥିର unsigned nr2hex(unsigned c);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା    chartab_initialized = FALSE;

// b_chartab[] is an array of 32 bytes, each bit representing one of the
// characters 0-255.
#define SET_CHARTAB(buf, c) (buf)->b_chartab[(unsigned)(c) >> 3] |= (1 << ((c) & 0x7))
#define RESET_CHARTAB(buf, c) (buf)->b_chartab[(unsigned)(c) >> 3] &= ~(1 << ((c) & 0x7))
#define GET_CHARTAB(buf, c) ((buf)->b_chartab[(unsigned)(c) >> 3] & (1 << ((c) & 0x7)))

// table used below, see init_chartab() for an explanation
ସ୍ଥିର char_u	g_chartab[256];

/*
 * Flags for g_chartab[].
 */
#define CT_CELL_MASK	0x07	// mask: nr of display cells (1, 2 or 4)
#define CT_PRINT_CHAR	0x10	// flag: set for printable chars
#define CT_ID_CHAR	0x20	// flag: set for ID chars
#define CT_FNAME_CHAR	0x40	// flag: set for file name chars

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_win_border(win_T *wp, colnr_T vcol);

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
init_chartab(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ buf_init_chartab(curbuf, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_init_chartab(
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		global)		// FALSE: only set buf->b_chartab[]
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 */
	c = 0;
	ଯେତେବେଳେ (c < ' ')
	    g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	ଯେତେବେଳେ (c <= '~')
	    g_chartab[c++] = 1 + CT_PRINT_CHAR;
	ଯେତେବେଳେ (c < 256)
	{
	    // UTF-8: bytes 0xa0 - 0xff are printable (latin1)
	    ଯଦି (enc_utf8 && c >= 0xa0)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // euc-jp characters starting with 0x8e are single width
	    ଅନ୍ୟଥା ଯଦି (enc_dbcs == DBCS_JPNU && c == 0x8e)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // other double-byte chars can be printable AND double-width
	    ଅନ୍ୟଥା ଯଦି (enc_dbcs != 0 && MB_BYTE2LEN(c) == 2)
		g_chartab[c++] = CT_PRINT_CHAR + 2;
	    ଅନ୍ୟଥା
		// the rest is unprintable by default
		g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	}

	// Assume that every multi-byte char is a filename character.
	ପାଇଁ (c = 1; c < 256; ++c)
	    ଯଦି ((enc_dbcs != 0 && MB_BYTE2LEN(c) > 1)
		    || (enc_dbcs == DBCS_JPNU && c == 0x8e)
		    || (enc_utf8 && c >= 0xa0))
		g_chartab[c] |= CT_FNAME_CHAR;
    }

    /*
     * Init word char flags all to FALSE
     */
    CLEAR_FIELD(buf->b_chartab);
    ଯଦି (enc_dbcs != 0)
	ପାଇଁ (c = 0; c < 256; ++c)
	{
	    // double-byte characters are probably word characters
	    ଯଦି (MB_BYTE2LEN(c) == 2)
		SET_CHARTAB(buf, c);
	}

    /*
     * In lisp mode the '-' character is included in keywords.
     */
    ଯଦି (buf->b_p_lisp)
	SET_CHARTAB(buf, '-');

    // Walk through the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    ପାଇଁ (i = global ? 0 : 3; i <= 3; ++i)
    {
	ଯଦି (i == 0)
	    p = p_isi;		// first round: 'isident'
	ଅନ୍ୟଥା ଯଦି (i == 1)
	    p = p_isp;		// second round: 'isprint'
	ଅନ୍ୟଥା ଯଦି (i == 2)
	    p = p_isf;		// third round: 'isfname'
	ଅନ୍ୟଥା	// i == 3
	    p = buf->b_p_isk;	// fourth round: 'iskeyword'

	ଯଦି (parse_isopt(p, buf, FALSE) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    chartab_initialized = TRUE;
    ଫେରନ୍ତୁ OK;
}

/**
 * Checks the format for the option settings 'iskeyword', 'isident', 'isfname'
 * or 'isprint'.
 * Returns FAIL if has an error, OK otherwise.
 */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_isopt(char_u *var)
{
    ଫେରନ୍ତୁ parse_isopt(var, ଖାଲି, TRUE);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_isopt(
	char_u	*var,
	buf_T	*buf,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	only_check)	// FALSE: refill g_chartab[]
{
    char_u	*p = var;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tilde;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_isalpha;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trail_comma;

    // Parses the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    // Each option is a list of characters, character numbers or ranges,
    // separated by commas, e.g.: "200-210,x,#-178,-"
    ଯେତେବେଳେ (*p)
    {
	tilde = FALSE;
	do_isalpha = FALSE;
	ଯଦି (*p == '^' && p[1] != NUL)
	{
	    tilde = TRUE;
	    ++p;
	}
	ଯଦି (VIM_ISDIGIT(*p))
	    c = getdigits(&p);
	ଅନ୍ୟଥା ଯଦି (has_mbyte)
	    c = mb_ptr2char_adv(&p);
	ଅନ୍ୟଥା
	    c = *p++;
	c2 = -1;
	ଯଦି (*p == '-' && p[1] != NUL)
	{
	    ++p;
	    ଯଦି (VIM_ISDIGIT(*p))
		c2 = getdigits(&p);
	    ଅନ୍ୟଥା ଯଦି (has_mbyte)
		c2 = mb_ptr2char_adv(&p);
	    ଅନ୍ୟଥା
		c2 = *p++;
	}
	ଯଦି (c <= 0 || c >= 256 || (c2 < c && c2 != -1) || c2 >= 256
						  || !(*p == NUL || *p == ','))
	    ଫେରନ୍ତୁ FAIL;

	trail_comma = *p == ',';
	p = skip_to_option_part(p);
	ଯଦି (trail_comma && *p == NUL)
	    // Trailing comma is not allowed.
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (only_check)
	    ଜାରି;

	ଯଦି (c2 == -1)	// not a range
	{
	    /*
	     * A single '@' (not "@-@"):
	     * Decide on letters being ID/printable/keyword chars with
	     * standard function isalpha(). This takes care of locale for
	     * single-byte characters).
	     */
	    ଯଦି (c == '@')
	    {
		do_isalpha = TRUE;
		c = 1;
		c2 = 255;
	    }
	    ଅନ୍ୟଥା
		c2 = c;
	}

	ଯେତେବେଳେ (c <= c2)
	{
	    // Use the MB_ functions here, because isalpha() doesn't
	    // work properly when 'encoding' is "latin1" and the locale is
	    // "C".
	    ଯଦି (!do_isalpha || MB_ISLOWER(c) || MB_ISUPPER(c))
	    {
		ଯଦି (var == p_isi)			// (re)set ID flag
		{
		    ଯଦି (tilde)
			g_chartab[c] &= ~CT_ID_CHAR;
		    ଅନ୍ୟଥା
			g_chartab[c] |= CT_ID_CHAR;
		}
		ଅନ୍ୟଥା ଯଦି (var == p_isp)			// (re)set printable
		{
		    ଯଦି ((c < ' ' || c > '~'
			    // For double-byte we keep the cell width, so
			    // that we can detect it from the first byte.
			) && !(enc_dbcs && MB_BYTE2LEN(c) == 2))
		    {
			ଯଦି (tilde)
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_MASK)
					 + ((dy_flags & DY_UHEX) ? 4 : 2);
			    g_chartab[c] &= ~CT_PRINT_CHAR;
			}
			ଅନ୍ୟଥା
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_MASK) + 1;
			    g_chartab[c] |= CT_PRINT_CHAR;
			}
		    }
		}
		ଅନ୍ୟଥା ଯଦି (var == p_isf)			// (re)set fname flag
		{
		    ଯଦି (tilde)
			g_chartab[c] &= ~CT_FNAME_CHAR;
		    ଅନ୍ୟଥା
			g_chartab[c] |= CT_FNAME_CHAR;
		}
		ଅନ୍ୟଥା // var == p_isk || var == buf->b_p_isk
							// (re)set keyword flag
		{
		    ଯଦି (tilde)
			RESET_CHARTAB(buf, c);
		    ଅନ୍ୟଥା
			SET_CHARTAB(buf, c);
		}
	    }
	    ++c;
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Translate any special characters in buf[bufsize] in-place.
 * The result is a string with only printable characters, but if there is not
 * enough room, not all characters will be translated.
 */
    ଶୂନ୍ୟ
trans_characters(
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufsize)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;		// length of string needing translation
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		room;		// room in buffer after string
    char_u	*trs;		// translated character
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trs_len;	// length of trs[]

    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buf);
    room = bufsize - len;
    ଯେତେବେଳେ (*buf != 0)
    {
	// Assume a multi-byte character doesn't need translation.
	ଯଦି (has_mbyte && (trs_len = (*mb_ptr2len)(buf)) > 1)
	    len -= trs_len;
	ଅନ୍ୟଥା
	{
	    trs = transchar_byte(*buf);
	    trs_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(trs);
	    ଯଦି (trs_len > 1)
	    {
		room -= trs_len - 1;
		ଯଦି (room <= 0)
		    ଫେରନ୍ତୁ;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l, len, c;
    char_u	hexbuf[11];

    ଯଦି (has_mbyte)
    {
	// Compute the length of the result, taking account of unprintable
	// multi-byte characters.
	len = 0;
	p = s;
	ଯେତେବେଳେ (*p != NUL)
	{
	    ଯଦି ((l = (*mb_ptr2len)(p)) > 1)
	    {
		c = (*mb_ptr2char)(p);
		p += l;
		ଯଦି (vim_isprintc(c))
		    len += l;
		ଅନ୍ୟଥା
		{
		    transchar_hex(hexbuf, c);
		    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(hexbuf);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		l = byte2cells(*p++);
		ଯଦି (l > 0)
		    len += l;
		ଅନ୍ୟଥା
		    len += 4;	// illegal byte sequence
	    }
	}
	res = alloc(len + 1);
    }
    ଅନ୍ୟଥା
	res = alloc(vim_strsize(s) + 1);

    ଯଦି (res == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    char_u *d = res;

    p = s;
    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    c = (*mb_ptr2char)(p);
	    ଯଦି (vim_isprintc(c))
	    {
		mch_memmove(d, p, (size_t)l);
		d += l;
	    }
	    ଅନ୍ୟଥା
	    {
		transchar_hex(d, c);
		d += STRLEN(d);
	    }
	    p += l;
	}
	ଅନ୍ୟଥା
	{
	    char_u	*trs = transchar_byte(*p++);
	    size_t	trs_len = STRLEN(trs);

	    mch_memmove(d, trs, trs_len);
	    d += trs_len;
	}
    }
    *d = NUL;
    ଫେରନ୍ତୁ res;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orglen,
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflen)
{
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = orglen;

#define GA_CHAR(i)  ((char_u *)ga.ga_data)[i]
#define GA_PTR(i)   ((char_u *)ga.ga_data + (i))
#define STR_CHAR(i)  (buf == ଖାଲି ? GA_CHAR(i) : buf[i])
#define STR_PTR(i)   (buf == ଖାଲି ? GA_PTR(i) : buf + (i))

    // Copy "str" into "buf" or allocated memory, unmodified.
    ଯଦି (buf == ଖାଲି)
    {
	ga_init2(&ga, 1, 10);
	ଯଦି (ga_grow(&ga, len + 1) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	mch_memmove(ga.ga_data, str, (size_t)len);
	ga.ga_len = len;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (len >= buflen)	    // Ugly!
	    len = buflen - 1;
	mch_memmove(buf, str, (size_t)len);
    }
    ଯଦି (buf == ଖାଲି)
	GA_CHAR(len) = NUL;
    ଅନ୍ୟଥା
	buf[len] = NUL;

    // Make each character lower case.
    i = 0;
    ଯେତେବେଳେ (STR_CHAR(i) != NUL)
    {
	ଯଦି (enc_utf8 || (has_mbyte && MB_BYTE2LEN(STR_CHAR(i)) > 1))
	{
	    ଯଦି (enc_utf8)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c = utf_ptr2char(STR_PTR(i));
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	olen = utf_ptr2len(STR_PTR(i));
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lc = utf_tolower(c);

		// Only replace the character when it is not an invalid
		// sequence (ASCII character or more than one byte) and
		// utf_tolower() doesn't return the original character.
		ଯଦି ((c < 0x80 || olen > 1) && c != lc)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nlen = utf_char2len(lc);

		    // If the byte length changes need to shift the following
		    // characters forward or backward.
		    ଯଦି (olen != nlen)
		    {
			ଯଦି (nlen > olen)
			{
			    ଯଦି (buf == ଖାଲି
				    ? ga_grow(&ga, nlen - olen + 1) == FAIL
				    : len + nlen - olen >= buflen)
			    {
				// out of memory, keep old char
				lc = c;
				nlen = olen;
			    }
			}
			ଯଦି (olen != nlen)
			{
			    ଯଦି (buf == ଖାଲି)
			    {
				STRMOVE(GA_PTR(i) + nlen, GA_PTR(i) + olen);
				ga.ga_len += nlen - olen;
			    }
			    ଅନ୍ୟଥା
			    {
				STRMOVE(buf + i + nlen, buf + i + olen);
				len += nlen - olen;
			    }
			}
		    }
		    (ଶୂନ୍ୟ)utf_char2bytes(lc, STR_PTR(i));
		}
	    }
	    // skip to next multi-byte char
	    i += (*mb_ptr2len)(STR_PTR(i));
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (buf == ଖାଲି)
		GA_CHAR(i) = TOLOWER_LOC(GA_CHAR(i));
	    ଅନ୍ୟଥା
		buf[i] = TOLOWER_LOC(buf[i]);
	    ++i;
	}
    }

    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ (char_u *)ga.ga_data;
    ଫେରନ୍ତୁ buf;
}

/*
 * Catch 22: g_chartab[] can't be initialized before the options are
 * initialized, and initializing options may cause transchar() to be called!
 * When chartab_initialized == FALSE don't use g_chartab[].
 * Does NOT work for multi-byte characters, c must be <= 255.
 * Also doesn't work for the first byte of a multi-byte, "c" must be a
 * character!
 */
ସ୍ଥିର char_u	transchar_charbuf[7];

    char_u *
transchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ transchar_buf(curbuf, c);
}

    char_u *
transchar_buf(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;

    i = 0;
    ଯଦି (IS_SPECIAL(c))	    // special key code, display as ~@ char
    {
	transchar_charbuf[0] = '~';
	transchar_charbuf[1] = '@';
	i = 2;
	c = K_SECOND(c);
    }

    ଯଦି ((!chartab_initialized && ((c >= ' ' && c <= '~')))
					|| (c < 256 && vim_isprintc_strict(c)))
    {
	// printable character
	transchar_charbuf[i] = c;
	transchar_charbuf[i + 1] = NUL;
    }
    ଅନ୍ୟଥା
	transchar_nonprint(buf, transchar_charbuf + i, c);
    ଫେରନ୍ତୁ transchar_charbuf;
}

/*
 * Like transchar(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of the current buffer.
 */
    char_u *
transchar_byte(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ transchar_byte_buf(curbuf, c);
}

/*
 * Like transchar_buf(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of "buf", unless it is NULL.
 */
    char_u *
transchar_byte_buf(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (enc_utf8 && c >= 0x80)
    {
	transchar_nonprint(buf, transchar_charbuf, c);
	ଫେରନ୍ତୁ transchar_charbuf;
    }
    ଫେରନ୍ତୁ transchar_buf(buf, c);
}
/*
 * Convert non-printable character to two or more printable characters in
 * "charbuf[]".  "charbuf" needs to be able to hold five bytes.
 * Does NOT work for multi-byte characters, c must be <= 255.
 */
    ଶୂନ୍ୟ
transchar_nonprint(buf_T *buf, char_u *charbuf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c == NL)
	c = NUL;		// we use newline in place of a NUL
    ଅନ୍ୟଥା ଯଦି (buf != ଖାଲି && c == CAR && get_fileformat(buf) == EOL_MAC)
	c = NL;			// we use CR in place of  NL in this case

    ଯଦି (dy_flags & DY_UHEX)		// 'display' has "uhex"
	transchar_hex(charbuf, c);

    ଅନ୍ୟଥା ଯଦି (c <= 0x7f)			// 0x00 - 0x1f and 0x7f
    {
	charbuf[0] = '^';
	charbuf[1] = c ^ 0x40;		// DEL displayed as ^?
	charbuf[2] = NUL;
    }
    ଅନ୍ୟଥା ଯଦି (enc_utf8)
    {
	transchar_hex(charbuf, c);
    }
    ଅନ୍ୟଥା ଯଦି (c >= ' ' + 0x80 && c <= '~' + 0x80)    // 0xa0 - 0xfe
    {
	charbuf[0] = '|';
	charbuf[1] = c - 0x80;
	charbuf[2] = NUL;
    }
    ଅନ୍ୟଥା					    // 0x80 - 0x9f and 0xff
    {
	charbuf[0] = '~';
	charbuf[1] = (c - 0x80) ^ 0x40;	// 0xff displayed as ~?
	charbuf[2] = NUL;
    }
}

    ଶୂନ୍ୟ
transchar_hex(char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;

    buf[0] = '<';
    ଯଦି (c > 255)
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
    ସ୍ଥିର unsigned
nr2hex(unsigned c)
{
    ଯଦି ((c & 0xf) <= 9)
	ଫେରନ୍ତୁ (c & 0xf) + '0';
    ଫେରନ୍ତୁ (c & 0xf) - 10 + 'a';
}

/*
 * Return number of display cells occupied by byte "b".
 * Caller must make sure 0 <= b <= 255.
 * For multi-byte mode "b" must be the first byte of a character.
 * A TAB is counted as two cells: "^I".
 * For UTF-8 mode this will return 0 for bytes >= 0x80, because the number of
 * cells depends on further bytes.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
byte2cells(ପୂର୍ଣ୍ଣସଂଖ୍ୟା b)
{
    ଯଦି (enc_utf8 && b >= 0x80)
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ (g_chartab[b] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character "c".
 * "c" can be a special key (negative number) in which case 3 or 4 is returned.
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
char2cells(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (IS_SPECIAL(c))
	ଫେରନ୍ତୁ char2cells(K_SECOND(c)) + 2;
    ଯଦି (c >= 0x80)
    {
	// UTF-8: above 0x80 need to check the value
	ଯଦି (enc_utf8)
	    ଫେରନ୍ତୁ utf_char2cells(c);
	// DBCS: double-byte means double-width, except for euc-jp with first
	// byte 0x8e
	ଯଦି (enc_dbcs != 0 && c >= 0x100)
	{
	    ଯଦି (enc_dbcs == DBCS_JPNU && ((unsigned)c >> 8) == 0x8e)
		ଫେରନ୍ତୁ 1;
	    ଫେରନ୍ତୁ 2;
	}
    }
    ଫେରନ୍ତୁ (g_chartab[c & 0xff] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character at "*p".
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ptr2cells(char_u *p)
{
    ଯଦି (!has_mbyte)
	ଫେରନ୍ତୁ byte2cells(*p);

    // For UTF-8 we need to look at more bytes if the first byte is >= 0x80.
    ଯଦି (enc_utf8 && *p >= 0x80)
	ଫେରନ୍ତୁ utf_ptr2cells(p);
    // For DBCS we can tell the cell count from the first byte.
    ଫେରନ୍ତୁ (g_chartab[*p] & CT_CELL_MASK);
}

/*
 * Return the number of character cells string "s" will take on the screen,
 * counting TABs as two characters: "^I".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_strsize(char_u *s)
{
    ଫେରନ୍ତୁ vim_strnsize(s, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)MAXCOL);
}

/*
 * Return the number of character cells string "s[len]" will take on the
 * screen, counting TABs as two characters: "^I".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_strnsize(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		size = 0;

    ଯେତେବେଳେ (*s != NUL && --len >= 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    l = (*mb_ptr2len)(s);

	size += ptr2cells(s);
	s += l;
	len -= l - 1;
    }

    ଫେରନ୍ତୁ size;
}

/*
 * Return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab.
 * Use a define to make it fast, this is used very often!!!
 * Also see getvcol() below.
 */

#ifdef FEAT_VARTABS
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, col) \
    ଯଦି (*(p) == TAB && (!(wp)->w_p_list || (wp)->w_lcs_chars.tab1)) \
    { \
	ଫେରନ୍ତୁ tabstop_padding(col, (buf)->b_p_ts, (buf)->b_p_vts_array); \
    } \
    ଅନ୍ୟଥା \
	ଫେରନ୍ତୁ ptr2cells(p);
#ଅନ୍ୟଥା
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, col) \
    ଯଦି (*(p) == TAB && (!(wp)->w_p_list || wp->w_lcs_chars.tab1)) \
    { \
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ts; \
	ts = (buf)->b_p_ts; \
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ts - (col % ts)); \
    } \
    ଅନ୍ୟଥା \
	ଫେରନ୍ତୁ ptr2cells(p);
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
chartabsize(char_u *p, colnr_T col)
{
    RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, p, col)
}

#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
linetabsize_str(char_u *s)
{
    ଫେରନ୍ତୁ linetabsize_col(0, s);
}

/*
 * Like linetabsize_str(), but "s" starts at column "startcol".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
linetabsize_col(ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol, char_u *s)
{
    chartabsize_T cts;
    vimlong_T vcol;

    init_chartabsize_arg(&cts, curwin, 0, startcol, s, s);
    vcol = cts.cts_vcol;

    ଯେତେବେଳେ (*cts.cts_ptr != NUL)
    {
	vcol += lbr_chartabsize_adv(&cts);
	ଯଦି (vcol > MAXCOL)
	{
	    cts.cts_vcol = MAXCOL;
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	    cts.cts_vcol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vcol;
    }
    clear_chartabsize_arg(&cts);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cts.cts_vcol;
}

/*
 * Like linetabsize_str(), but for a given window instead of the current one.
 * Doesn't count the size of 'listchars' "eol".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_linetabsize(win_T *wp, linenr_T lnum, char_u *ଧାଡ଼ି, colnr_T len)
{
    chartabsize_T cts;

    init_chartabsize_arg(&cts, wp, lnum, 0, ଧାଡ଼ି, ଧାଡ଼ି);
    win_linetabsize_cts(&cts, len);
    clear_chartabsize_arg(&cts);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cts.cts_vcol;
}

/*
 * Return the number of cells line "lnum" of window "wp" will take on the
 * screen, taking into account the size of a tab and text properties.
 * Doesn't count the size of 'listchars' "eol".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
linetabsize(win_T *wp, linenr_T lnum)
{
    ଫେରନ୍ତୁ win_linetabsize(wp, lnum,
		       ml_get_buf(wp->w_buffer, lnum, FALSE), (colnr_T)MAXCOL);
}

/*
 * Like linetabsize(), but counts the size of 'listchars' "eol".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
linetabsize_eol(win_T *wp, linenr_T lnum)
{
    ଫେରନ୍ତୁ linetabsize(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Like linetabsize(), but excludes 'above'/'after'/'right'/'below' aligned
 * virtual text, while keeping inline virtual text.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
linetabsize_no_outer(win_T *wp, linenr_T lnum)
{
#ifndef FEAT_PROP_POPUP
    ଫେରନ୍ତୁ linetabsize(wp, lnum);
#ଅନ୍ୟଥା
    chartabsize_T cts;
    char_u *ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, ଧାଡ଼ି, ଧାଡ଼ି);

    ଯଦି (cts.cts_text_prop_ଗଣନା)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_idx = 0;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_idx = 0; read_idx < cts.cts_text_prop_ଗଣନା; read_idx++)
	{
	    textprop_T *tp = &cts.cts_text_props[read_idx];
	    ଯଦି (tp->tp_col != MAXCOL)
	    {
		ଯଦି (read_idx != write_idx)
		    cts.cts_text_props[write_idx] = *tp;
		write_idx++;
	    }
	}
	cts.cts_text_prop_ଗଣନା = write_idx;
	ଯଦି (cts.cts_text_prop_ଗଣନା == 0)
	    VIM_CLEAR(cts.cts_text_props);
    }

    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cts.cts_vcol;
#endif
}

/*
 * Like linetabsize_no_outer(), but counts the size of 'listchars' "eol".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
linetabsize_no_outer_eol(win_T *wp, linenr_T lnum)
{
    ଫେରନ୍ତୁ linetabsize_no_outer(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Return TRUE when win_lbr_chartabsize() does nothing more than
 * win_nolbr_chartabsize(): no 'linebreak', 'breakindent', 'showbreak' and no
 * text properties that insert text.
 */
#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_lbr_is_simple_width(chartabsize_T *cts)
{
    ଫେରନ୍ତୁ TRUE
# ifdef FEAT_LINEBREAK
	&& !cts->cts_win->w_p_lbr && !cts->cts_win->w_p_bri
	&& *get_showbreak_ମୂଲ୍ୟ(cts->cts_win) == NUL
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_text
# endif
	;
}
#endif

    ଶୂନ୍ୟ
win_linetabsize_cts(chartabsize_T *cts, colnr_T len)
{
    vimlong_T vcol = cts->cts_vcol;
#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    // Fast path: when win_lbr_chartabsize() would just return
    // win_nolbr_chartabsize() (wrap on; no 'linebreak'/'breakindent'/
    // 'showbreak'/inserted text properties), call it directly.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା simple_width = cts->cts_win->w_p_wrap && win_lbr_is_simple_width(cts);
#endif
#ifdef FEAT_PROP_POPUP
    cts->cts_with_trailing = len == MAXCOL;
#endif
    ପାଇଁ ( ; *cts->cts_ptr != NUL && (len == MAXCOL || cts->cts_ptr < cts->cts_ଧାଡ଼ି + len);
						      MB_PTR_ADV(cts->cts_ptr))
    {
#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
	ଯଦି (simple_width)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c = *cts->cts_ptr;

	    ଯଦି (c < 0x80 && c != TAB)
		vcol += g_chartab[c] & CT_CELL_MASK;
	    ଅନ୍ୟଥା
		vcol += win_nolbr_chartabsize(cts, ଖାଲି);
	}
	ଅନ୍ୟଥା
#endif
	    vcol += win_lbr_chartabsize(cts, ଖାଲି, ଖାଲି);
	ଯଦି (vcol > MAXCOL)
	{
	    cts->cts_vcol = MAXCOL;
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	    cts->cts_vcol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vcol;
    }
#ifdef FEAT_PROP_POPUP
    // check for a virtual text at the end of a line or on an empty line
    ଯଦି (len == MAXCOL && cts->cts_has_prop_with_text && *cts->cts_ptr == NUL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା head = 0;
	(ଶୂନ୍ୟ)win_lbr_chartabsize(cts, &head, ଖାଲି);
	vcol += cts->cts_cur_text_width + head;
	// When properties are above the empty line must also be counted.  For
	// a property below the width already includes filling up the line.
	ଯଦି (cts->cts_ptr == cts->cts_ଧାଡ଼ି && cts->cts_prop_ଧାଡ଼ିଗୁଡ଼ିକ > 0
							 && !cts->cts_has_below)
	    ++vcol;
	cts->cts_vcol = vcol > MAXCOL ? MAXCOL : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vcol;
    }
#endif
}

/*
 * Return TRUE if 'c' is a normal identifier character:
 * Letters and characters from the 'isident' option.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isIDc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ (c > 0 && c < 0x100 && (g_chartab[c] & CT_ID_CHAR));
}

/*
 * Like vim_isIDc() but not using the 'isident' option: letters, numbers and
 * underscore.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isNormalIDc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ ASCII_ISALNUM(c) || c == '_';
}

/*
 * return TRUE if 'c' is a keyword character: Letters and characters from
 * 'iskeyword' option for the current buffer.
 * For multi-byte characters mb_get_class() is used (builtin rules).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_iswordc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ vim_iswordc_buf(c, curbuf);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_iswordc_buf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, buf_T *buf)
{
    ଯଦି (c >= 0x100)
    {
	ଯଦି (enc_dbcs != 0)
	    ଫେରନ୍ତୁ dbcs_class((unsigned)c >> 8, (unsigned)(c & 0xff)) >= 2;
	ଯଦି (enc_utf8)
	    ଫେରନ୍ତୁ utf_class_buf(c, buf) >= 2;
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ (c > 0 && GET_CHARTAB(buf, c) != 0);
}

/*
 * Just like vim_iswordc() but uses a pointer to the (multi-byte) character.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_iswordp(char_u *p)
{
    ଫେରନ୍ତୁ vim_iswordp_buf(p, curbuf);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_iswordp_buf(char_u *p, buf_T *buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c = *p;

    ଯଦି (has_mbyte && MB_BYTE2LEN(c) > 1)
	c = (*mb_ptr2char)(p);
    ଫେରନ୍ତୁ vim_iswordc_buf(c, buf);
}

/*
 * Return TRUE if 'c' is a valid file-name character as specified with the
 * 'isfname' option.
 * Assume characters above 0x100 are valid (multi-byte).
 * To be used for commands like "gf".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isfilec(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_FNAME_CHAR)));
}

#ଯଦି defined(FEAT_SPELL)
/*
 * Return TRUE if 'c' is a valid file-name character, including characters left
 * out of 'isfname' to make "gf" work, such as comma, space, '@', etc.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_is_fname_char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ vim_isfilec(c) || c == ',' || c == ' ' || c == '@';
}
#endif

/*
 * return TRUE if 'c' is a valid file-name character or a wildcard character
 * Assume characters above 0x100 are valid (multi-byte).
 * Explicitly interpret ']' as a wildcard character as mch_has_wildcard("]")
 * returns false.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isfilec_or_wc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u buf[2];

    buf[0] = (char_u)c;
    buf[1] = NUL;
    ଫେରନ୍ତୁ vim_isfilec(c) || c == ']' || mch_has_wildcard(buf);
}

/*
 * Return TRUE if 'c' is a printable character.
 * Assume characters above 0x100 are printable (multi-byte), except for
 * Unicode.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isprintc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (enc_utf8 && c >= 0x100)
	ଫେରନ୍ତୁ utf_printable(c);
    ଫେରନ୍ତୁ (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Strict version of vim_isprintc(c), don't return TRUE if "c" is the head
 * byte of a double-byte character.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isprintc_strict(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (enc_dbcs != 0 && c < 0x100 && MB_BYTE2LEN(c) > 1)
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (enc_utf8 && c >= 0x100)
	ଫେରନ୍ତୁ utf_printable(c);
    ଫେରନ୍ତୁ (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Prepare the structure passed to chartabsize functions.
 * "line" is the start of the line, "ptr" is the first relevant character.
 * When "lnum" is zero do not use text properties that insert text.
 */
    ଶୂନ୍ୟ
init_chartabsize_arg(
	chartabsize_T	*cts,
	win_T		*wp,
	linenr_T	lnum UNUSED,
	colnr_T		col,
	char_u		*ଧାଡ଼ି,
	char_u		*ptr)
{
    CLEAR_POINTER(cts);
    cts->cts_win = wp;
    cts->cts_vcol = col;
    cts->cts_ଧାଡ଼ି = ଧାଡ଼ି;
    cts->cts_ptr = ptr;
#ifdef FEAT_LINEBREAK
    cts->cts_bri_size = -1;
#endif
#ifdef FEAT_PROP_POPUP
    ଯଦି (lnum > 0 && !ignore_text_props)
    {
	char_u	*prop_start;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଗଣନା_2;

	ଗଣନା_2 = get_text_props(wp->w_buffer, lnum, &prop_start, FALSE);
	cts->cts_text_prop_ଗଣନା = ଗଣନା_2;
	ଯଦି (ଗଣନା_2 > 0)
	{
	    // Make a copy of the properties, so that they are properly
	    // aligned.  Make it twice as long for the sorting below.
	    cts->cts_text_props = ALLOC_MULT(textprop_T, ଗଣନା_2 * 2);
	    ଯଦି (cts->cts_text_props == ଖାଲି)
		cts->cts_text_prop_ଗଣନା = 0;
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

		mch_memmove(cts->cts_text_props + ଗଣନା_2, prop_start,
						   ଗଣନା_2 * ଆକାର(textprop_T));
		ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
		{
		    textprop_T *tp = cts->cts_text_props + i + ଗଣନା_2;
		    ଯଦି (tp->tp_id < 0
				     && text_prop_type_valid(wp->w_buffer, tp))
		    {
			cts->cts_has_prop_with_text = TRUE;
			ଭାଙ୍ଗ;
		    }
		}
		ଯଦି (!cts->cts_has_prop_with_text)
		{
		    // won't use the text properties, free them
		    VIM_CLEAR(cts->cts_text_props);
		    cts->cts_text_prop_ଗଣନା = 0;
		}
		ଅନ୍ୟଥା
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *text_prop_idxs;

		    // Need to sort the array to get any truncation right.
		    // Do the sorting in the second part of the array, then
		    // move the sorted props to the first part of the array.
		    text_prop_idxs = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଗଣନା_2);
		    ଯଦି (text_prop_idxs != ଖାଲି)
		    {
			ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
			    text_prop_idxs[i] = i + ଗଣନା_2;
			sort_text_props(curbuf, cts->cts_text_props,
							text_prop_idxs, ଗଣନା_2);
			// Here we want the reverse order.
			ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
			    cts->cts_text_props[ଗଣନା_2 - i - 1] =
					cts->cts_text_props[text_prop_idxs[i]];
			vim_free(text_prop_idxs);
		    }

		    // Convert tp_text_offset to tp_text pointer.
		    char_u *ଗଣନା_ptr = prop_start - PROP_ଗଣନା_SIZE;

		    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
		    {
			textprop_T *tp = &cts->cts_text_props[i];

			ଯଦି (tp->tp_id < 0 && tp->u.tp_text_offset > 0)
			{
			    tp->u.tp_text = ଗଣନା_ptr + tp->u.tp_text_offset;
			    tp->tp_flags |= TP_FLAG_VTEXT_PTR;
			}
			ଅନ୍ୟଥା
			    tp->u.tp_text = ଖାଲି;
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
    ଶୂନ୍ୟ
clear_chartabsize_arg(chartabsize_T *cts UNUSED)
{
#ifdef FEAT_PROP_POPUP
    ଯଦି (cts->cts_text_prop_ଗଣନା > 0)
    {
	VIM_CLEAR(cts->cts_text_props);
	cts->cts_text_prop_ଗଣନା = 0;
    }
#endif
}

/*
 * Like chartabsize(), but also check for line breaks on the screen and text
 * properties that insert text.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lbr_chartabsize(chartabsize_T *cts)
{
#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ଯଦି (1
# ifdef FEAT_LINEBREAK
	&& !curwin->w_p_lbr && *get_showbreak_ମୂଲ୍ୟ(curwin) == NUL
							   && !curwin->w_p_bri
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_text
# endif
       )
    {
#endif
	ଯଦି (curwin->w_p_wrap)
	    ଫେରନ୍ତୁ win_nolbr_chartabsize(cts, ଖାଲି);
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, cts->cts_ptr, cts->cts_vcol)
#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    }
    ଫେରନ୍ତୁ win_lbr_chartabsize(cts, ଖାଲି, ଖାଲି);
#endif
}

/*
 * Call lbr_chartabsize() and advance the pointer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lbr_chartabsize_adv(chartabsize_T *cts)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;

    retval = lbr_chartabsize(cts);
    MB_PTR_ADV(cts->cts_ptr);
    ଫେରନ୍ତୁ retval;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_lbr_chartabsize(
	chartabsize_T	*cts,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*headp UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*tailp UNUSED)
{
    win_T	*wp = cts->cts_win;
#ଯଦି defined(FEAT_PROP_POPUP) || defined(FEAT_LINEBREAK)
    char_u	*ଧାଡ଼ି = cts->cts_ଧାଡ଼ି; // start of the line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		size;
#endif
    char_u	*s = cts->cts_ptr;
    colnr_T	vcol = cts->cts_vcol;
#ifdef FEAT_LINEBREAK
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mb_added = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    char_u	*sbr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		no_sbr = FALSE;
#endif

#ଯଦି defined(FEAT_PROP_POPUP)
    cts->cts_cur_text_width = 0;
    cts->cts_has_below = false;
    cts->cts_first_char = 0;
#endif

#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    /*
     * No 'linebreak', 'showbreak', 'breakindent' and text properties that
     * insert text: return quickly.
     */
    ଯଦି (win_lbr_is_simple_width(cts))
#endif
    {
	ଯଦି (wp->w_p_wrap)
	    ଫେରନ୍ତୁ win_nolbr_chartabsize(cts, headp);
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_buffer, s, vcol)
    }

#ଯଦି defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_lcs_eol = wp->w_p_list && wp->w_lcs_chars.eol != NUL;
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
    ଯଦି (*s == NUL)
    {
	// 1 cell for EOL list char (if present), as opposed to the two cell ^@
	// for a NUL character in the text.
	size = has_lcs_eol ? 1 : 0;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_doublewidth = has_mbyte && size == 2 && MB_BYTE2LEN(*s) > 1;
# endif

# ifdef FEAT_PROP_POPUP
    ଯଦି (cts->cts_has_prop_with_text)
    {
#  ifdef FEAT_LINEBREAK
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tab_size = size;
#  endif
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    charlen = *s == NUL ? 1 : mb_ptr2len(s);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - ଧାଡ଼ି);


	// The "$" for 'list' mode will go between the EOL and
	// the text prop, account for that.
	ଯଦି (has_lcs_eol)
	{
	    ++vcol;
	    --size;
	}

	ପାଇଁ (i = 0; i < cts->cts_text_prop_ଗଣନା; ++i)
	{
	    textprop_T	*tp = cts->cts_text_props + i;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_off = win_col_off(wp);

	    // Watch out for the text being deleted.  "cts_text_props" is a
	    // copy, the text prop may actually have been removed from the line.
	    ଯଦି (tp->tp_id < 0
		    && ((tp->tp_col - 1 >= col
					     && tp->tp_col - 1 < col + charlen)
		       || (tp->tp_col == MAXCOL
			   && ((tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
				? col == 0
				: s[0] == NUL && cts->cts_with_trailing)))
		    && tp->u.tp_text != ଖାଲି)
	    {
		char_u *p = tp->u.tp_text;

		ଯଦି (p != ଖାଲି)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cells;

		    ଯଦି (tp->tp_col == MAXCOL)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_extra = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);

			cells = text_prop_position(wp, tp, vcol,
			     (vcol + size) % (wp->w_width - col_off) + col_off,
					      &n_extra, &p, ଖାଲି, ଖାଲି, FALSE);
#  ifdef FEAT_LINEBREAK
			ଯଦି (text_prop_no_showbreak(tp))
			    no_sbr = TRUE;  // don't use 'showbreak' now
#  endif
		    }
		    ଅନ୍ୟଥା
			cells = vim_strsize(p);
		    ଯଦି (tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
		    {
			cts->cts_first_char += cells;
			ଯଦି (!cts->cts_no_above)
			    cts->cts_cur_text_width += cells;
		    }
		    ଅନ୍ୟଥା
		    {
			cts->cts_cur_text_width += cells;
			size += cells;
		    }
		    cts->cts_start_incl = tp->tp_flags & TP_FLAG_START_INCL;
#  ifdef FEAT_LINEBREAK
		    ଯଦି (*s == TAB)
		    {
			// tab size changes because of the inserted text
			size -= tab_size;
			tab_size = win_chartabsize(wp, s,
					vcol + size - cts->cts_above_width);
			size += tab_size;
		    }
#  endif
		    ଯଦି (tp->tp_col == MAXCOL && (tp->tp_flags
				& (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW)))
		    {
			// count extra line for property above/below
			++cts->cts_prop_ଧାଡ଼ିଗୁଡ଼ିକ;
			ଯଦି (tp->tp_flags & TP_FLAG_ALIGN_BELOW)
			    cts->cts_has_below = true;
		    }
		}
	    }
	    ଯଦି (tp->tp_col != MAXCOL && tp->tp_col - 1 > col)
		ଭାଙ୍ଗ;
	}
	ଯଦି (has_lcs_eol)
	{
	    --vcol;
	    ++size;
	}
    }
# endif

# ifdef FEAT_LINEBREAK
    ଯଦି (is_doublewidth && wp->w_p_wrap && in_win_border(wp, vcol + size - 2))
    {
	++size;		// Count the ">" in the last column.
	mb_added = 1;
    }

    /*
     * May have to add something for 'breakindent' and/or 'showbreak'
     * string at the start of a screen line.
     */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା head = mb_added;
    sbr = no_sbr ? empty_option : get_showbreak_ମୂଲ୍ୟ(wp);
    // When "size" is 0, no new screen line is started.
    ଯଦି (size > 0 && wp->w_p_wrap && (*sbr != NUL || wp->w_p_bri))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col_off_prev = win_col_off(wp);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	width2 = wp->w_width - col_off_prev + win_col_off2(wp);
	colnr_T	wcol = vcol + col_off_prev;
#  ifdef FEAT_PROP_POPUP
	wcol -= wp->w_virtcol_first_char;
#  endif
	colnr_T	max_head_vcol = cts->cts_max_head_vcol;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	added = 0;

	// cells taken by 'showbreak'/'breakindent' before current char
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	head_prev = 0;
	ଯଦି (wcol >= wp->w_width)
	{
	    wcol -= wp->w_width;
	    col_off_prev = wp->w_width - width2;
	    ଯଦି (wcol >= width2 && width2 > 0)
		wcol %= width2;
	    ଯଦି (*sbr != NUL)
		head_prev += vim_strsize(sbr);
	    ଯଦି (wp->w_p_bri)
	    {
		ଯଦି (cts->cts_bri_size < 0)
		    cts->cts_bri_size = get_breakindent_win(wp, ଧାଡ଼ି);
		head_prev += cts->cts_bri_size;
	    }
	    ଯଦି (wcol < head_prev)
	    {
		head_prev -= wcol;
		wcol += head_prev;
		added += head_prev;
		ଯଦି (max_head_vcol <= 0 || vcol < max_head_vcol)
		    head += head_prev;
	    }
	    ଅନ୍ୟଥା
		head_prev = 0;
	    wcol += col_off_prev;
	}

	ଯଦି (wcol + size > wp->w_width)
	{
	    // cells taken by 'showbreak'/'breakindent' halfway current char
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	head_mid = 0;
	    ଯଦି (*sbr != NUL)
		head_mid += vim_strsize(sbr);
	    ଯଦି (wp->w_p_bri)
	    {
		ଯଦି (cts->cts_bri_size < 0)
		    cts->cts_bri_size = get_breakindent_win(wp, ଧାଡ଼ି);
		head_mid += cts->cts_bri_size;
	    }
	    ଯଦି (head_mid > 0)
	    {
		// Calculate effective window width.
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_rem = wp->w_width - wcol;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = width2 - head_mid;

		ଯଦି (width <= 0)
		    width = 1;
		// Divide "size - prev_rem" by "width", rounding up.
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt = (size - prev_rem + width - 1) / width;
		added += cnt * head_mid;

		ଯଦି (max_head_vcol == 0 || vcol + size + added < max_head_vcol)
		    head += cnt * head_mid;
		ଅନ୍ୟଥା ଯଦି (width2 > 0
			   && max_head_vcol > vcol + head_prev + prev_rem)
		    head += (max_head_vcol - (vcol + head_prev + prev_rem)
					     + width2 - 1) / width2 * head_mid;
		ଅନ୍ୟଥା ଯଦି (max_head_vcol < 0)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = mb_added;
#  ifdef FEAT_PROP_POPUP
		    ଯଦି (*s != NUL
			     && ((State & MODE_NORMAL) || cts->cts_start_incl))
			off += cts->cts_cur_text_width;
#  endif
		    ଯଦି (off >= prev_rem)
			head += (1 + (off - prev_rem) / width) * head_mid;
		}
	    }
	}

	size += added;
    }

    ଯଦି (headp != ଖାଲି)
	*headp = head;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା size_before_lbr = size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା need_lbr = FALSE;
    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here.
     */
    ଯଦି (wp->w_p_lbr && wp->w_p_wrap && wp->w_width != 0
	    && VIM_ISBREAK((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)s[0]) && !VIM_ISBREAK((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)s[1]))
    {
	char_u	*t = cts->cts_ଧାଡ଼ି;
	ଯେତେବେଳେ (VIM_ISBREAK((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)t[0]))
	    t++;
	// 'linebreak' is only needed when not in leading whitespace.
	need_lbr = s >= t;
    }
    ଯଦି (need_lbr)
    {
	/*
	 * Count all characters from first non-blank after a blank up to next
	 * non-blank after a blank.
	 */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା numberextra = win_col_off(wp);
	colnr_T col_adj = size - 1;
	colnr_T colmax = (colnr_T)(wp->w_width - numberextra - col_adj);
	ଯଦି (vcol >= colmax)
	{
	    colmax += col_adj;
	    n = colmax +  win_col_off2(wp);
	    ଯଦି (n > 0)
		colmax += (((vcol + col_adj - colmax) / n) + 1) * n - col_adj;
	}

	colnr_T vcol2 = vcol;
	ପାଇଁ (;;)
	{
	    char_u *ps = s;
	    MB_PTR_ADV(s);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = *s;
	    ଯଦି (!(c != NUL
		    && (VIM_ISBREAK(c)
			|| (!VIM_ISBREAK(c)
			       && (vcol2 == vcol || !VIM_ISBREAK((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*ps))))))
		ଭାଙ୍ଗ;

	    vcol2 += win_chartabsize(wp, s, vcol2);
	    ଯଦି (vcol2 >= colmax)		// doesn't fit
	    {
		size = colmax - vcol + col_adj;
		ଭାଙ୍ଗ;
	    }
	}
    }

    ଯଦି (tailp != ଖାଲି)
	*tailp = size - size_before_lbr;

#  ifdef FEAT_PROP_POPUP
    ଯଦି (cts->cts_first_char > 0 && !cts->cts_no_above)
    {
	// Remember the width for the size of a Tab later in the line.  Use
	// assignment, this may be called more than once for a character.
	cts->cts_above_width = cts->cts_first_char;
	size += cts->cts_first_char;
    }
#  endif
# endif
    ଫେରନ୍ତୁ size;
#endif
}

/*
 * Like win_lbr_chartabsize(), except that we know 'linebreak' is off, 'wrap'
 * is on and there are no properties that insert text.  This means we need to
 * check for a double-byte character that doesn't fit at the end of the screen
 * line.
 * Only uses "cts_win", "cts_ptr" and "cts_vcol" from "cts".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_nolbr_chartabsize(
	chartabsize_T	*cts,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*headp)
{
    win_T	*wp = cts->cts_win;
    char_u	*s = cts->cts_ptr;
    colnr_T	col = cts->cts_vcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯଦି (*s == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
    {
#ifdef FEAT_VARTABS
	ଫେରନ୍ତୁ tabstop_padding(col, wp->w_buffer->b_p_ts,
				    wp->w_buffer->b_p_vts_array);
#ଅନ୍ୟଥା
	n = wp->w_buffer->b_p_ts;
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(n - (col % n));
#endif
    }
    n = ptr2cells(s);
    // Add one cell for a double-width character in the last column of the
    // window, displayed with a ">".
    ଯଦି (n == 2 && MB_BYTE2LEN(*s) > 1 && in_win_border(wp, col))
    {
	ଯଦି (headp != ଖାଲି)
	    *headp = 1;
	ଫେରନ୍ତୁ 3;
    }
    ଫେରନ୍ତୁ n;
}

/*
 * Return TRUE if virtual column "vcol" is in the rightmost column of window
 * "wp".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
in_win_border(win_T *wp, colnr_T vcol)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width1;		// width of first line (after line number)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width2;		// width of further lines

    ଯଦି (wp->w_width == 0)	// there is no border
	ଫେରନ୍ତୁ FALSE;
    width1 = wp->w_width - win_col_off(wp);
    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vcol < width1 - 1)
	ଫେରନ୍ତୁ FALSE;
    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vcol == width1 - 1)
	ଫେରନ୍ତୁ TRUE;
    width2 = width1 + win_col_off2(wp);
    ଯଦି (width2 <= 0)
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ ((vcol - width1) % width2 == width2 - 1);
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
    ଶୂନ୍ୟ
getvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*start,
    colnr_T	*cursor,
    colnr_T	*end,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    colnr_T	vcol;
    char_u	*ptr;		// points to current char
    char_u	*ଧାଡ଼ି;		// start of the line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		incr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		head;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tail;
#ifdef FEAT_VARTABS
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*vts = wp->w_buffer->b_p_vts_array;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ts = wp->w_buffer->b_p_ts;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    chartabsize_T cts;
#ifdef FEAT_PROP_POPUP
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		on_NUL = FALSE;
#endif

    vcol = 0;
    ଧାଡ଼ି = ptr = ml_get_buf(wp->w_buffer, pos->lnum, FALSE);

    init_chartabsize_arg(&cts, wp, pos->lnum, 0, ଧାଡ଼ି, ଧାଡ଼ି);
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
    ଯଦି ((!wp->w_p_list || wp->w_lcs_chars.tab1 != NUL)
#ifdef FEAT_LINEBREAK
	    && !wp->w_p_lbr && *get_showbreak_ମୂଲ୍ୟ(wp) == NUL && !wp->w_p_bri
#endif
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
       )
    {
	ପାଇଁ (;;)
	{
	    head = 0;
	    c = *ptr;
	    // make sure we don't go past the end of the line
	    ଯଦି (c == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
		ଭାଙ୍ଗ;
	    }
	    // A tab gets expanded, depending on the current column
	    ଯଦି (c == TAB)
#ifdef FEAT_VARTABS
		incr = tabstop_padding(vcol, ts, vts);
#ଅନ୍ୟଥା
		incr = ts - (vcol % ts);
#endif
	    ଅନ୍ୟଥା
	    {
		ଯଦି (has_mbyte)
		{
		    // For utf-8, if the byte is >= 0x80, need to look at
		    // further bytes to find the cell width.
		    ଯଦି (enc_utf8 && c >= 0x80)
			incr = utf_ptr2cells(ptr);
		    ଅନ୍ୟଥା
			incr = g_chartab[c] & CT_CELL_MASK;

		    // If a double-cell char doesn't fit at the end of a line
		    // it wraps to the next line, it's like this char is three
		    // cells wide.
		    ଯଦି (incr == 2 && wp->w_p_wrap && MB_BYTE2LEN(*ptr) > 1
			    && in_win_border(wp, vcol))
		    {
			++incr;
			head = 1;
		    }
		}
		ଅନ୍ୟଥା
		    incr = g_chartab[c] & CT_CELL_MASK;
	    }

	    char_u *next_ptr = ptr + (*mb_ptr2len)(ptr);
	    ଯଦି (next_ptr - ଧାଡ଼ି > pos->col) // character at pos->col
		ଭାଙ୍ଗ;

	    vcol += incr;
	    ptr = next_ptr;
	}

	tail = 0;
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (;;)
	{
	    // A tab gets expanded, depending on the current column.
	    // Other things also take up space.
	    head = 0;
	    tail = 0;
	    incr = win_lbr_chartabsize(&cts, &head, &tail);
#ifdef FEAT_PROP_POPUP
	    ଯଦି (cts.cts_ptr == cts.cts_ଧାଡ଼ି)
	    {
		ଯଦି (flags & GETVCOL_FOR_VIRTCOL)
		    // do not count the virtual text above for w_curswant
		    wp->w_virtcol_first_char = cts.cts_first_char;
		ଯଦି ((flags & GETVCOL_NO_ABOVE) == 0)
		{
		    cts.cts_vcol += cts.cts_first_char;
		    cts.cts_above_width = cts.cts_first_char;
		}
	    }
#endif
	    // make sure we don't go past the end of the line
	    ଯଦି (*cts.cts_ptr == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
#ifdef FEAT_PROP_POPUP
		incr += cts.cts_cur_text_width;
		on_NUL = TRUE;
#endif
		ଭାଙ୍ଗ;
	    }

	    char_u *next_ptr = cts.cts_ptr + (*mb_ptr2len)(cts.cts_ptr);
	    ଯଦି (next_ptr - ଧାଡ଼ି > pos->col) // character at pos->col
		ଭାଙ୍ଗ;

	    cts.cts_vcol += incr;
	    cts.cts_ptr = next_ptr;
	}
	vcol = cts.cts_vcol;
	ptr = cts.cts_ptr;
    }
    clear_chartabsize_arg(&cts);

    ଯଦି (*ptr == NUL && pos->col < MAXCOL && pos->col > ptr - ଧାଡ଼ି)
	pos->col = ptr - ଧାଡ଼ି;

    ଯଦି (start != ଖାଲି)
	*start = vcol + head;
    ଯଦି (end != ଖାଲି)
	*end = vcol + incr - (flags & GETVCOL_END_EXCL_LBR ? tail : 0) - 1;
    ଯଦି (cursor != ଖାଲି)
    {
	ଯଦି (*ptr == TAB
		&& (State & MODE_NORMAL)
		&& !wp->w_p_list
		&& !virtual_active()
		&& !(VIsual_active
				&& (*p_sel == 'e' || LTOREQ_POS(*pos, VIsual)))
		)
	    // TODO: subtracting "tail" may lead to better cursor position
	    *cursor = vcol + incr - 1;	    // cursor at end
	ଅନ୍ୟଥା
	{
#ifdef FEAT_PROP_POPUP
	    // in Insert mode, if "start_incl" is true the text gets inserted
	    // after the virtual text, thus add its width
	    ଯଦି (((State & MODE_ଭର୍ତ୍ତି) == 0 || cts.cts_start_incl) && !on_NUL)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		list_save = curwin->w_p_list;
    colnr_T	vcol;

    curwin->w_p_list = FALSE;
    ଯଦି (posp->coladd)
	getvvcol(curwin, posp, ଖାଲି, &vcol, ଖାଲି, 0);
    ଅନ୍ୟଥା
	getvcol(curwin, posp, ଖାଲି, &vcol, ଖାଲି, 0);
    curwin->w_p_list = list_save;
    ଫେରନ୍ତୁ vcol;
}

/*
 * Get virtual column in virtual mode.
 */
    ଶୂନ୍ୟ
getvvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*start,
    colnr_T	*cursor,
    colnr_T	*end,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    colnr_T	col;
    colnr_T	coladd;
    colnr_T	endadd;
    char_u	*ptr;

    ଯଦି (virtual_active())
    {
	// For virtual mode, only want one value
	getvcol(wp, pos, &col, ଖାଲି, ଖାଲି, flags);

	coladd = pos->coladd;
	endadd = 0;
	// Cannot put the cursor on part of a wide character.
	ptr = ml_get_buf(wp->w_buffer, pos->lnum, FALSE);
	ଯଦି (pos->col < ml_get_buf_len(wp->w_buffer, pos->lnum))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = (*mb_ptr2char)(ptr + pos->col);

	    ଯଦି (c != TAB && vim_isprintc(c))
	    {
		endadd = (colnr_T)(char2cells(c) - 1);
		ଯଦି (coladd > endadd)	// past end of line
		    endadd = 0;
		ଅନ୍ୟଥା
		    coladd = 0;
	    }
	}
	col += coladd;
	ଯଦି (start != ଖାଲି)
	    *start = col;
	ଯଦି (cursor != ଖାଲି)
	    *cursor = col;
	ଯଦି (end != ଖାଲି)
	    *end = col + endadd;
    }
    ଅନ୍ୟଥା
	getvcol(wp, pos, start, cursor, end, flags);
}

/*
 * Get the leftmost and rightmost virtual column of pos1 and pos2.
 * Used for Visual block mode.
 */
    ଶୂନ୍ୟ
getvcols(
    win_T	*wp,
    pos_T	*pos1,
    pos_T	*pos2,
    colnr_T	*ବାମ,
    colnr_T	*ଡାହାଣ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    colnr_T	from1, from2, to1, to2;

    ଯଦି (LT_POSP(pos1, pos2))
    {
	getvvcol(wp, pos1, &from1, ଖାଲି, &to1, flags);
	getvvcol(wp, pos2, &from2, ଖାଲି, &to2, flags);
    }
    ଅନ୍ୟଥା
    {
	getvvcol(wp, pos2, &from1, ଖାଲି, &to1, flags);
	getvvcol(wp, pos1, &from2, ଖାଲି, &to2, flags);
    }
    ଯଦି (from2 < from1)
	*ବାମ = from2;
    ଅନ୍ୟଥା
	*ବାମ = from1;
    ଯଦି (to2 > to1)
    {
	ଯଦି (*p_sel == 'e' && from2 - 1 >= to1)
	    *ଡାହାଣ = from2 - 1;
	ଅନ୍ୟଥା
	    *ଡାହାଣ = to2;
    }
    ଅନ୍ୟଥା
	*ଡାହାଣ = to1;
}

/*
 * Skip over ' ' and '\t'.
 */
    char_u *
skipwhite(char_u *q)
{
    char_u	*p = q;

    ଯେତେବେଳେ (VIM_ISWHITE(*p))
	++p;
    ଫେରନ୍ତୁ p;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * skip over ' ', '\t' and '\n'.
 */
    char_u *
skipwhite_and_nl(char_u *q)
{
    char_u	*p = q;

    ଯେତେବେଳେ (VIM_ISWHITE(*p) || *p == NL)
	++p;
    ଫେରନ୍ତୁ p;
}
#endif

/*
 * getwhitecols: return the number of whitespace
 * columns (bytes) at the start of a given line
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getwhitecols_curline(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ getwhitecols(ml_get_curline());
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getwhitecols(char_u *p)
{
    ଫେରନ୍ତୁ skipwhite(p) - p;
}

/*
 * skip over digits
 */
    char_u *
skipdigits(char_u *q)
{
    char_u	*p = q;

    ଯେତେବେଳେ (VIM_ISDIGIT(*p))	// skip to next non-digit
	++p;
    ଫେରନ୍ତୁ p;
}

#ଯଦି defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * skip over binary digits
 */
    char_u *
skipbin(char_u *q)
{
    char_u	*p = q;

    ଯେତେବେଳେ (vim_isbdigit(*p))	// skip to next non-digit
	++p;
    ଫେରନ୍ତୁ p;
}

/*
 * skip over digits and hex characters
 */
    char_u *
skiphex(char_u *q)
{
    char_u	*p = q;

    ଯେତେବେଳେ (vim_isxdigit(*p))	// skip to next non-digit
	++p;
    ଫେରନ୍ତୁ p;
}
#endif

/*
 * skip to bin digit (or NUL after the string)
 */
    char_u *
skiptobin(char_u *q)
{
    char_u	*p = q;

    ଯେତେବେଳେ (*p != NUL && !vim_isbdigit(*p))	// skip to next digit
	++p;
    ଫେରନ୍ତୁ p;
}

/*
 * skip to digit (or NUL after the string)
 */
    char_u *
skiptodigit(char_u *q)
{
    char_u	*p = q;

    ଯେତେବେଳେ (*p != NUL && !VIM_ISDIGIT(*p))	// skip to next digit
	++p;
    ଫେରନ୍ତୁ p;
}

/*
 * skip to hex character (or NUL after the string)
 */
    char_u *
skiptohex(char_u *q)
{
    char_u	*p = q;

    ଯେତେବେଳେ (*p != NUL && !vim_isxdigit(*p))	// skip to next digit
	++p;
    ଫେରନ୍ତୁ p;
}

/*
 * Variant of isdigit() that can handle characters > 0x100.
 * We don't use isdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 * Use the VIM_ISDIGIT() macro for simple arguments.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isdigit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ (c >= '0' && c <= '9');
}

/*
 * Variant of isxdigit() that can handle characters > 0x100.
 * We don't use isxdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isxdigit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'f')
	|| (c >= 'A' && c <= 'F');
}

/*
 * Corollary of vim_isdigit and vim_isxdigit() that can handle
 * characters > 0x100.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isbdigit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ (c == '0' || c == '1');
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isodigit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ (c >= '0' && c <= '7');
}

/*
 * Vim's own character class functions.  These exist because many library
 * islower()/toupper() etc. do not work properly: they crash when used with
 * invalid values or can't handle latin1 when the locale is C.
 * Speed is most important here.
 */
#define LATIN1LOWER 'l'
#define LATIN1UPPER 'U'

ସ୍ଥିର char_u latin1flags[257] = "                                                                 UUUUUUUUUUUUUUUUUUUUUUUUUU      llllllllllllllllllllllllll                                                                     UUUUUUUUUUUUUUUUUUUUUUU UUUUUUUllllllllllllllllllllllll llllllll";
ସ୍ଥିର char_u latin1upper[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xf7\xd8\xd9\xda\xdb\xdc\xdd\xde\xff";
ସ୍ଥିର char_u latin1lower[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xd7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_islower(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c <= '@')
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (c >= 0x80)
    {
	ଯଦି (enc_utf8)
	    ଫେରନ୍ତୁ utf_islower(c);
	ଯଦି (c >= 0x100)
	{
#ifdef HAVE_ISWLOWER
	    ଯଦି (has_mbyte)
		ଫେରନ୍ତୁ iswlower(c);
#endif
	    // islower() can't handle these chars and may crash
	    ଫେରନ୍ତୁ FALSE;
	}
	ଯଦି (enc_latin1like)
	    ଫେରନ୍ତୁ (latin1flags[c] & LATIN1LOWER) == LATIN1LOWER;
    }
    ଫେରନ୍ତୁ SAFE_islower(c);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isupper(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c <= '@')
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (c >= 0x80)
    {
	ଯଦି (enc_utf8)
	    ଫେରନ୍ତୁ utf_isupper(c);
	ଯଦି (c >= 0x100)
	{
#ifdef HAVE_ISWUPPER
	    ଯଦି (has_mbyte)
		ଫେରନ୍ତୁ iswupper(c);
#endif
	    // islower() can't handle these chars and may crash
	    ଫେରନ୍ତୁ FALSE;
	}
	ଯଦି (enc_latin1like)
	    ଫେରନ୍ତୁ (latin1flags[c] & LATIN1UPPER) == LATIN1UPPER;
    }
    ଫେରନ୍ତୁ SAFE_isupper(c);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isalpha(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ vim_islower(c) || vim_isupper(c);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_toupper(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c <= '@')
	ଫେରନ୍ତୁ c;
    ଯଦି (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	ଯଦି (enc_utf8)
	    ଫେରନ୍ତୁ utf_toupper(c);
	ଯଦି (c >= 0x100)
	{
#ifdef HAVE_TOWUPPER
	    ଯଦି (has_mbyte)
		ଫେରନ୍ତୁ towupper(c);
#endif
	    // toupper() can't handle these chars and may crash
	    ଫେରନ୍ତୁ c;
	}
	ଯଦି (enc_latin1like)
	    ଫେରନ୍ତୁ latin1upper[c];
    }
    ଯଦି (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ଫେରନ୍ତୁ TOUPPER_ASC(c);
    ଫେରନ୍ତୁ TOUPPER_LOC(c);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_tolower(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c <= '@')
	ଫେରନ୍ତୁ c;
    ଯଦି (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	ଯଦି (enc_utf8)
	    ଫେରନ୍ତୁ utf_tolower(c);
	ଯଦି (c >= 0x100)
	{
#ifdef HAVE_TOWLOWER
	    ଯଦି (has_mbyte)
		ଫେରନ୍ତୁ towlower(c);
#endif
	    // tolower() can't handle these chars and may crash
	    ଫେରନ୍ତୁ c;
	}
	ଯଦି (enc_latin1like)
	    ଫେରନ୍ତୁ latin1lower[c];
    }
    ଯଦି (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ଫେରନ୍ତୁ TOLOWER_ASC(c);
    ଫେରନ୍ତୁ TOLOWER_LOC(c);
}

/*
 * skiptowhite: skip over text until ' ' or '\t' or NUL.
 */
    char_u *
skiptowhite(char_u *p)
{
    ଯେତେବେଳେ (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    ଫେରନ୍ତୁ p;
}

/*
 * skiptowhite_esc: Like skiptowhite(), but also skip escaped chars
 */
    char_u *
skiptowhite_esc(char_u *p)
{
    ଯେତେବେଳେ (*p != ' ' && *p != '\t' && *p != NUL)
    {
	ଯଦି ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    ଫେରନ୍ତୁ p;
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
    retval = strtol((ଅକ୍ଷର *)p, ଖାଲି, 10);
    ଯଦି (*p == '-')		// skip negative sign
	++p;
    p = skipdigits(p);		// skip to next non-digit
    *pp = p;
    ଫେରନ୍ତୁ retval;
}

/*
 * Like getdigits() but allow for embedded single quotes.
 */
    long
getdigits_quoted(char_u **pp)
{
    char_u	*p = *pp;
    long	retval = 0;

    ଯଦି (*p == '-')
	++p;
    ଯେତେବେଳେ (VIM_ISDIGIT(*p))
    {
	ଯଦି (retval >= LONG_MAX / 10 - 10)
	    retval = LONG_MAX;
	ଅନ୍ୟଥା
	    retval = retval * 10 - '0' + *p;
	++p;
	ଯଦି (in_vim9script() && *p == '\'' && VIM_ISDIGIT(p[1]))
	    ++p;
    }
    ଯଦି (**pp == '-')
    {
	ଯଦି (retval == LONG_MAX)
	    retval = LONG_MIN;
	ଅନ୍ୟଥା
	    retval = -retval;
    }
    *pp = p;
    ଫେରନ୍ତୁ retval;
}

/*
 * Return TRUE if "lbuf" is empty or only contains blanks.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isblankline(char_u *lbuf)
{
    char_u	*p;

    p = skipwhite(lbuf);
    ଫେରନ୍ତୁ (*p == NUL || *p == '\r' || *p == '\n');
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
    ଶୂନ୍ୟ
vim_str2nr(
    char_u		*start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*prep,	    // return: type of number 0 = decimal, 'x'
				    // or 'X' is hex, '0', 'o' or 'O' is octal,
				    // 'b' or 'B' is bin
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*len,	    // return: detected length of number
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			what,	    // what numbers to recognize
    varnumber_T		*nptr,	    // return: signed result
    uvarnumber_T	*unptr,	    // return: unsigned result
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			maxlen,     // max length of string to check
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			strict,     // check strictly
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*overflow)  // when not NULL set to TRUE for overflow
{
    char_u	    *ptr = start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    pre = 0;		// default is decimal
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    negative = FALSE;
    uvarnumber_T    un = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    n;

    ଯଦି (len != ଖାଲି)
	*len = 0;

    ଯଦି (ptr[0] == '-')
    {
	negative = TRUE;
	++ptr;
    }

    // Recognize hex, octal, and bin.
    ଯଦି (ptr[0] == '0' && ptr[1] != '8' && ptr[1] != '9'
					       && (maxlen == 0 || maxlen > 1))
    {
	pre = ptr[1];
	ଯଦି ((what & STR2NR_HEX)
		&& (pre == 'X' || pre == 'x') && vim_isxdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // hexadecimal
	    ptr += 2;
	ଅନ୍ୟଥା ଯଦି ((what & STR2NR_BIN)
		&& (pre == 'B' || pre == 'b') && vim_isbdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    //ଦ୍ୱିମିକ
	    ptr += 2;
	ଅନ୍ୟଥା ଯଦି ((what & STR2NR_OOCT)
		&& (pre == 'O' || pre == 'o') && vim_isodigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // octal with prefix "0o"
	    ptr += 2;
	ଅନ୍ୟଥା
	{
	    // decimal or octal, default is decimal
	    pre = 0;
	    ଯଦି (what & STR2NR_OCT)
	    {
		// Don't interpret "0", "08" or "0129" as octal.
		ପାଇଁ (n = 1; n != maxlen && VIM_ISDIGIT(ptr[n]); ++n)
		{
		    ଯଦି (ptr[n] > '7')
		    {
			pre = 0;	// can't be octal
			ଭାଙ୍ଗ;
		    }
		    pre = '0';	// assume octal
		}
	    }
	}
    }

    // Do the conversion manually to avoid sscanf() quirks.
    n = 1;
    ଯଦି (pre == 'B' || pre == 'b'
			     || ((what & STR2NR_BIN) && (what & STR2NR_FORCE)))
    {
	// bin
	ଯଦି (pre != 0)
	    n += 2;	    // skip over "0b"
	ଯେତେବେଳେ ('0' <= *ptr && *ptr <= '1')
	{
	    // avoid ubsan error for overflow
	    ଯଦି (un <= UVARNUM_MAX / 2)
		un = 2 * un + (uvarnumber_T)(*ptr - '0');
	    ଅନ୍ୟଥା
	    {
		un = UVARNUM_MAX;
		ଯଦି (overflow != ଖାଲି)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    ଯଦି (n++ == maxlen)
		ଭାଙ୍ଗ;
	    ଯଦି ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '1')
	    {
		++ptr;
		ଯଦି (n++ == maxlen)
		    ଭାଙ୍ଗ;
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (pre == 'O' || pre == 'o' ||
		pre == '0' || ((what & STR2NR_OCT) && (what & STR2NR_FORCE)))
    {
	// octal
	ଯଦି (pre != 0 && pre != '0')
	    n += 2;	    // skip over "0o"
	ଯେତେବେଳେ ('0' <= *ptr && *ptr <= '7')
	{
	    // avoid ubsan error for overflow
	    ଯଦି (un <= UVARNUM_MAX / 8)
		un = 8 * un + (uvarnumber_T)(*ptr - '0');
	    ଅନ୍ୟଥା
	    {
		un = UVARNUM_MAX;
		ଯଦି (overflow != ଖାଲି)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    ଯଦି (n++ == maxlen)
		ଭାଙ୍ଗ;
	    ଯଦି ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '7')
	    {
		++ptr;
		ଯଦି (n++ == maxlen)
		    ଭାଙ୍ଗ;
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (pre != 0 || ((what & STR2NR_HEX) && (what & STR2NR_FORCE)))
    {
	// hex
	ଯଦି (pre != 0)
	    n += 2;	    // skip over "0x"
	ଯେତେବେଳେ (vim_isxdigit(*ptr))
	{
	    // avoid ubsan error for overflow
	    ଯଦି (un <= UVARNUM_MAX / 16)
		un = 16 * un + (uvarnumber_T)hex2nr(*ptr);
	    ଅନ୍ୟଥା
	    {
		un = UVARNUM_MAX;
		ଯଦି (overflow != ଖାଲି)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    ଯଦି (n++ == maxlen)
		ଭାଙ୍ଗ;
	    ଯଦି ((what & STR2NR_QUOTE) && *ptr == '\'' && vim_isxdigit(ptr[1]))
	    {
		++ptr;
		ଯଦି (n++ == maxlen)
		    ଭାଙ୍ଗ;
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	// decimal
	ଯେତେବେଳେ (VIM_ISDIGIT(*ptr))
	{
	    uvarnumber_T    digit = (uvarnumber_T)(*ptr - '0');

	    // avoid ubsan error for overflow
	    ଯଦି (un < UVARNUM_MAX / 10
		    || (un == UVARNUM_MAX / 10 && digit <= UVARNUM_MAX % 10))
		un = 10 * un + digit;
	    ଅନ୍ୟଥା
	    {
		un = UVARNUM_MAX;
		ଯଦି (overflow != ଖାଲି)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    ଯଦି (n++ == maxlen)
		ଭାଙ୍ଗ;
	    ଯଦି ((what & STR2NR_QUOTE) && *ptr == '\'' && VIM_ISDIGIT(ptr[1]))
	    {
		++ptr;
		ଯଦି (n++ == maxlen)
		    ଭାଙ୍ଗ;
	    }
	}
    }

    // Check for an alphanumeric character immediately following, that is
    // most likely a typo.
    ଯଦି (strict && n - 1 != maxlen && ASCII_ISALNUM(*ptr))
	ଫେରନ୍ତୁ;

    ଯଦି (prep != ଖାଲି)
	*prep = pre;
    ଯଦି (len != ଖାଲି)
	*len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ptr - start);
    ଯଦି (nptr != ଖାଲି)
    {
	ଯଦି (negative)   // account for leading '-' for decimal numbers
	{
	    // avoid ubsan error for overflow
	    ଯଦି (un > VARNUM_MAX)
	    {
		*nptr = VARNUM_MIN;
		ଯଦି (overflow != ଖାଲି)
		    *overflow = TRUE;
	    }
	    ଅନ୍ୟଥା
		*nptr = -(varnumber_T)un;
	}
	ଅନ୍ୟଥା
	{
	    // prevent a large unsigned number to become negative
	    ଯଦି (un > VARNUM_MAX)
	    {
		un = VARNUM_MAX;
		ଯଦି (overflow != ଖାଲି)
		    *overflow = TRUE;
	    }
	    *nptr = (varnumber_T)un;
	}
    }
    ଯଦି (unptr != ଖାଲି)
	*unptr = un;
}

/*
 * Return the value of a single hex character.
 * Only valid when the argument is '0' - '9', 'A' - 'F' or 'a' - 'f'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hex2nr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c >= 'a' && c <= 'f')
	ଫେରନ୍ତୁ c - 'a' + 10;
    ଯଦି (c >= 'A' && c <= 'F')
	ଫେରନ୍ତୁ c - 'A' + 10;
    ଫେରନ୍ତୁ c - '0';
}

/*
 * Convert two hex characters to a byte.
 * Return -1 if one of the characters is not hex.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hexhex2nr(char_u *p)
{
    ଯଦି (!vim_isxdigit(p[0]) || !vim_isxdigit(p[1]))
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ (hex2nr(p[0]) << 4) + hex2nr(p[1]);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rem_backslash(char_u *str)
{
#ifdef BACKSLASH_IN_FILENAME
    ଫେରନ୍ତୁ (str[0] == '\\'
	    && str[1] < 0x80
	    && (str[1] == ' '
		|| (str[1] != NUL
		    && str[1] != '*'
		    && str[1] != '?'
		    && !vim_isfilec(str[1]))));
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ (str[0] == '\\' && str[1] != NUL);
#endif
}

/*
 * Halve the number of backslashes in a file name argument.
 * For MS-DOS we only do this if the character after the backslash
 * is not a normal file character.
 */
    ଶୂନ୍ୟ
backslash_halve(char_u *p)
{
    ପାଇଁ ( ; *p; ++p)
	ଯଦି (rem_backslash(p))
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
    ଯଦି (res == ଖାଲି)
	ଫେରନ୍ତୁ p;
    backslash_halve(res);
    ଫେରନ୍ତୁ res;
}
