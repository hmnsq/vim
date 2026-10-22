/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"

#නම් defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#endif

static පූර්ණ_සංඛ්‍යා parse_isopt(char_u *var, buf_T *buf, පූර්ණ_සංඛ්‍යා only_check);
static පූර්ණ_සංඛ්‍යා win_nolbr_chartabsize(chartabsize_T *cts, පූර්ණ_සංඛ්‍යා *headp);
static unsigned nr2hex(unsigned c);

static පූර්ණ_සංඛ්‍යා    chartab_initialized = FALSE;

// b_chartab[] is an array of 32 bytes, each bit representing one of the
// characters 0-255.
#define SET_CHARTAB(buf, c) (buf)->b_chartab[(unsigned)(c) >> 3] |= (1 << ((c) & 0x7))
#define RESET_CHARTAB(buf, c) (buf)->b_chartab[(unsigned)(c) >> 3] &= ~(1 << ((c) & 0x7))
#define GET_CHARTAB(buf, c) ((buf)->b_chartab[(unsigned)(c) >> 3] & (1 << ((c) & 0x7)))

// table used below, see init_chartab() for an explanation
static char_u	g_chartab[256];

/*
 * Flags for g_chartab[].
 */
#define CT_CELL_MASK	0x07	// mask: nr of display cells (1, 2 or 4)
#define CT_PRINT_CHAR	0x10	// flag: set for printable chars
#define CT_ID_CHAR	0x20	// flag: set for ID chars
#define CT_FNAME_CHAR	0x40	// flag: set for file name chars

static පූර්ණ_සංඛ්‍යා in_win_border(win_T *wp, colnr_T vcol);

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
    පූර්ණ_සංඛ්‍යා
init_chartab(void)
{
    ආපසු buf_init_chartab(curbuf, TRUE);
}

    පූර්ණ_සංඛ්‍යා
buf_init_chartab(
    buf_T	*buf,
    පූර්ණ_සංඛ්‍යා		global)		// FALSE: only set buf->b_chartab[]
{
    පූර්ණ_සංඛ්‍යා		c;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		i;

    නම් (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 */
	c = 0;
	අතරතුර (c < ' ')
	    g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	අතරතුර (c <= '~')
	    g_chartab[c++] = 1 + CT_PRINT_CHAR;
	අතරතුර (c < 256)
	{
	    // UTF-8: bytes 0xa0 - 0xff are printable (latin1)
	    නම් (enc_utf8 && c >= 0xa0)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // euc-jp characters starting with 0x8e are single width
	    නැතහොත් නම් (enc_dbcs == DBCS_JPNU && c == 0x8e)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // other double-byte chars can be printable AND double-width
	    නැතහොත් නම් (enc_dbcs != 0 && MB_BYTE2LEN(c) == 2)
		g_chartab[c++] = CT_PRINT_CHAR + 2;
	    නැතහොත්
		// the rest is unprintable by default
		g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	}

	// Assume that every multi-byte char is a filename character.
	සඳහා (c = 1; c < 256; ++c)
	    නම් ((enc_dbcs != 0 && MB_BYTE2LEN(c) > 1)
		    || (enc_dbcs == DBCS_JPNU && c == 0x8e)
		    || (enc_utf8 && c >= 0xa0))
		g_chartab[c] |= CT_FNAME_CHAR;
    }

    /*
     * Init word char flags all to FALSE
     */
    CLEAR_FIELD(buf->b_chartab);
    නම් (enc_dbcs != 0)
	සඳහා (c = 0; c < 256; ++c)
	{
	    // double-byte characters are probably word characters
	    නම් (MB_BYTE2LEN(c) == 2)
		SET_CHARTAB(buf, c);
	}

    /*
     * In lisp mode the '-' character is included in keywords.
     */
    නම් (buf->b_p_lisp)
	SET_CHARTAB(buf, '-');

    // Walk through the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    සඳහා (i = global ? 0 : 3; i <= 3; ++i)
    {
	නම් (i == 0)
	    p = p_isi;		// first round: 'isident'
	නැතහොත් නම් (i == 1)
	    p = p_isp;		// second round: 'isprint'
	නැතහොත් නම් (i == 2)
	    p = p_isf;		// third round: 'isfname'
	නැතහොත්	// i == 3
	    p = buf->b_p_isk;	// fourth round: 'iskeyword'

	නම් (parse_isopt(p, buf, FALSE) == FAIL)
	    ආපසු FAIL;
    }

    chartab_initialized = TRUE;
    ආපසු OK;
}

/**
 * Checks the format for the option settings 'iskeyword', 'isident', 'isfname'
 * or 'isprint'.
 * Returns FAIL if has an error, OK otherwise.
 */
	පූර්ණ_සංඛ්‍යා
check_isopt(char_u *var)
{
    ආපසු parse_isopt(var, NULL, TRUE);
}

    static පූර්ණ_සංඛ්‍යා
parse_isopt(
	char_u	*var,
	buf_T	*buf,
	පූර්ණ_සංඛ්‍යා	only_check)	// FALSE: refill g_chartab[]
{
    char_u	*p = var;
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		c2;
    පූර්ණ_සංඛ්‍යා		tilde;
    පූර්ණ_සංඛ්‍යා		do_isalpha;
    පූර්ණ_සංඛ්‍යා		trail_comma;

    // Parses the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    // Each option is a list of characters, character numbers or ranges,
    // separated by commas, e.g.: "200-210,x,#-178,-"
    අතරතුර (*p)
    {
	tilde = FALSE;
	do_isalpha = FALSE;
	නම් (*p == '^' && p[1] != NUL)
	{
	    tilde = TRUE;
	    ++p;
	}
	නම් (VIM_ISDIGIT(*p))
	    c = getdigits(&p);
	නැතහොත් නම් (has_mbyte)
	    c = mb_ptr2char_adv(&p);
	නැතහොත්
	    c = *p++;
	c2 = -1;
	නම් (*p == '-' && p[1] != NUL)
	{
	    ++p;
	    නම් (VIM_ISDIGIT(*p))
		c2 = getdigits(&p);
	    නැතහොත් නම් (has_mbyte)
		c2 = mb_ptr2char_adv(&p);
	    නැතහොත්
		c2 = *p++;
	}
	නම් (c <= 0 || c >= 256 || (c2 < c && c2 != -1) || c2 >= 256
						  || !(*p == NUL || *p == ','))
	    ආපසු FAIL;

	trail_comma = *p == ',';
	p = skip_to_option_part(p);
	නම් (trail_comma && *p == NUL)
	    // Trailing comma is not allowed.
	    ආපසු FAIL;

	නම් (only_check)
	    continue;

	නම් (c2 == -1)	// not a range
	{
	    /*
	     * A single '@' (not "@-@"):
	     * Decide on letters being ID/printable/keyword chars with
	     * standard function isalpha(). This takes care of locale for
	     * single-byte characters).
	     */
	    නම් (c == '@')
	    {
		do_isalpha = TRUE;
		c = 1;
		c2 = 255;
	    }
	    නැතහොත්
		c2 = c;
	}

	අතරතුර (c <= c2)
	{
	    // Use the MB_ functions here, because isalpha() doesn't
	    // work properly when 'encoding' is "latin1" and the locale is
	    // "C".
	    නම් (!do_isalpha || MB_ISLOWER(c) || MB_ISUPPER(c))
	    {
		නම් (var == p_isi)			// (re)set ID flag
		{
		    නම් (tilde)
			g_chartab[c] &= ~CT_ID_CHAR;
		    නැතහොත්
			g_chartab[c] |= CT_ID_CHAR;
		}
		නැතහොත් නම් (var == p_isp)			// (re)set printable
		{
		    නම් ((c < ' ' || c > '~'
			    // For double-byte we keep the cell width, so
			    // that we can detect it from the first byte.
			) && !(enc_dbcs && MB_BYTE2LEN(c) == 2))
		    {
			නම් (tilde)
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_MASK)
					 + ((dy_flags & DY_UHEX) ? 4 : 2);
			    g_chartab[c] &= ~CT_PRINT_CHAR;
			}
			නැතහොත්
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_MASK) + 1;
			    g_chartab[c] |= CT_PRINT_CHAR;
			}
		    }
		}
		නැතහොත් නම් (var == p_isf)			// (re)set fname flag
		{
		    නම් (tilde)
			g_chartab[c] &= ~CT_FNAME_CHAR;
		    නැතහොත්
			g_chartab[c] |= CT_FNAME_CHAR;
		}
		නැතහොත් // var == p_isk || var == buf->b_p_isk
							// (re)set keyword flag
		{
		    නම් (tilde)
			RESET_CHARTAB(buf, c);
		    නැතහොත්
			SET_CHARTAB(buf, c);
		}
	    }
	    ++c;
	}
    }

    ආපසු OK;
}

/*
 * Translate any special characters in buf[bufsize] in-place.
 * The result is a string with only printable characters, but if there is not
 * enough room, not all characters will be translated.
 */
    void
trans_characters(
    char_u	*buf,
    පූර්ණ_සංඛ්‍යා		bufsize)
{
    පූර්ණ_සංඛ්‍යා		len;		// length of string needing translation
    පූර්ණ_සංඛ්‍යා		room;		// room in buffer after string
    char_u	*trs;		// translated character
    පූර්ණ_සංඛ්‍යා		trs_len;	// length of trs[]

    len = (පූර්ණ_සංඛ්‍යා)STRLEN(buf);
    room = bufsize - len;
    අතරතුර (*buf != 0)
    {
	// Assume a multi-byte character doesn't need translation.
	නම් (has_mbyte && (trs_len = (*mb_ptr2len)(buf)) > 1)
	    len -= trs_len;
	නැතහොත්
	{
	    trs = transchar_byte(*buf);
	    trs_len = (පූර්ණ_සංඛ්‍යා)STRLEN(trs);
	    නම් (trs_len > 1)
	    {
		room -= trs_len - 1;
		නම් (room <= 0)
		    ආපසු;
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
    පූර්ණ_සංඛ්‍යා		l, len, c;
    char_u	hexbuf[11];

    නම් (has_mbyte)
    {
	// Compute the length of the result, taking account of unprintable
	// multi-byte characters.
	len = 0;
	p = s;
	අතරතුර (*p != NUL)
	{
	    නම් ((l = (*mb_ptr2len)(p)) > 1)
	    {
		c = (*mb_ptr2char)(p);
		p += l;
		නම් (vim_isprintc(c))
		    len += l;
		නැතහොත්
		{
		    transchar_hex(hexbuf, c);
		    len += (පූර්ණ_සංඛ්‍යා)STRLEN(hexbuf);
		}
	    }
	    නැතහොත්
	    {
		l = byte2cells(*p++);
		නම් (l > 0)
		    len += l;
		නැතහොත්
		    len += 4;	// illegal byte sequence
	    }
	}
	res = alloc(len + 1);
    }
    නැතහොත්
	res = alloc(vim_strsize(s) + 1);

    නම් (res == NULL)
	ආපසු NULL;

    char_u *d = res;

    p = s;
    අතරතුර (*p != NUL)
    {
	නම් (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    c = (*mb_ptr2char)(p);
	    නම් (vim_isprintc(c))
	    {
		mch_memmove(d, p, (size_t)l);
		d += l;
	    }
	    නැතහොත්
	    {
		transchar_hex(d, c);
		d += STRLEN(d);
	    }
	    p += l;
	}
	නැතහොත්
	{
	    char_u	*trs = transchar_byte(*p++);
	    size_t	trs_len = STRLEN(trs);

	    mch_memmove(d, trs, trs_len);
	    d += trs_len;
	}
    }
    *d = NUL;
    ආපසු res;
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
    පූර්ණ_සංඛ්‍යා		orglen,
    char_u	*buf,
    පූර්ණ_සංඛ්‍යා		buflen)
{
    garray_T	ga;
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		len = orglen;

#define GA_CHAR(i)  ((char_u *)ga.ga_data)[i]
#define GA_PTR(i)   ((char_u *)ga.ga_data + (i))
#define STR_CHAR(i)  (buf == NULL ? GA_CHAR(i) : buf[i])
#define STR_PTR(i)   (buf == NULL ? GA_PTR(i) : buf + (i))

    // Copy "str" into "buf" or allocated memory, unmodified.
    නම් (buf == NULL)
    {
	ga_init2(&ga, 1, 10);
	නම් (ga_grow(&ga, len + 1) == FAIL)
	    ආපසු NULL;
	mch_memmove(ga.ga_data, str, (size_t)len);
	ga.ga_len = len;
    }
    නැතහොත්
    {
	නම් (len >= buflen)	    // Ugly!
	    len = buflen - 1;
	mch_memmove(buf, str, (size_t)len);
    }
    නම් (buf == NULL)
	GA_CHAR(len) = NUL;
    නැතහොත්
	buf[len] = NUL;

    // Make each character lower case.
    i = 0;
    අතරතුර (STR_CHAR(i) != NUL)
    {
	නම් (enc_utf8 || (has_mbyte && MB_BYTE2LEN(STR_CHAR(i)) > 1))
	{
	    නම් (enc_utf8)
	    {
		පූර්ණ_සංඛ්‍යා	c = utf_ptr2char(STR_PTR(i));
		පූර්ණ_සංඛ්‍යා	olen = utf_ptr2len(STR_PTR(i));
		පූර්ණ_සංඛ්‍යා	lc = utf_tolower(c);

		// Only replace the character when it is not an invalid
		// sequence (ASCII character or more than one byte) and
		// utf_tolower() doesn't return the original character.
		නම් ((c < 0x80 || olen > 1) && c != lc)
		{
		    පූර්ණ_සංඛ්‍යා	    nlen = utf_char2len(lc);

		    // If the byte length changes need to shift the following
		    // characters forward or backward.
		    නම් (olen != nlen)
		    {
			නම් (nlen > olen)
			{
			    නම් (buf == NULL
				    ? ga_grow(&ga, nlen - olen + 1) == FAIL
				    : len + nlen - olen >= buflen)
			    {
				// out of memory, keep old char
				lc = c;
				nlen = olen;
			    }
			}
			නම් (olen != nlen)
			{
			    නම් (buf == NULL)
			    {
				STRMOVE(GA_PTR(i) + nlen, GA_PTR(i) + olen);
				ga.ga_len += nlen - olen;
			    }
			    නැතහොත්
			    {
				STRMOVE(buf + i + nlen, buf + i + olen);
				len += nlen - olen;
			    }
			}
		    }
		    (void)utf_char2bytes(lc, STR_PTR(i));
		}
	    }
	    // skip to next multi-byte char
	    i += (*mb_ptr2len)(STR_PTR(i));
	}
	නැතහොත්
	{
	    නම් (buf == NULL)
		GA_CHAR(i) = TOLOWER_LOC(GA_CHAR(i));
	    නැතහොත්
		buf[i] = TOLOWER_LOC(buf[i]);
	    ++i;
	}
    }

    නම් (buf == NULL)
	ආපසු (char_u *)ga.ga_data;
    ආපසු buf;
}

/*
 * Catch 22: g_chartab[] can't be initialized before the options are
 * initialized, and initializing options may cause transchar() to be called!
 * When chartab_initialized == FALSE don't use g_chartab[].
 * Does NOT work for multi-byte characters, c must be <= 255.
 * Also doesn't work for the first byte of a multi-byte, "c" must be a
 * character!
 */
static char_u	transchar_charbuf[7];

    char_u *
transchar(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු transchar_buf(curbuf, c);
}

    char_u *
transchar_buf(buf_T *buf, පූර්ණ_සංඛ්‍යා c)
{
    පූර්ණ_සංඛ්‍යා			i;

    i = 0;
    නම් (IS_SPECIAL(c))	    // special key code, display as ~@ char
    {
	transchar_charbuf[0] = '~';
	transchar_charbuf[1] = '@';
	i = 2;
	c = K_SECOND(c);
    }

    නම් ((!chartab_initialized && ((c >= ' ' && c <= '~')))
					|| (c < 256 && vim_isprintc_strict(c)))
    {
	// printable character
	transchar_charbuf[i] = c;
	transchar_charbuf[i + 1] = NUL;
    }
    නැතහොත්
	transchar_nonprint(buf, transchar_charbuf + i, c);
    ආපසු transchar_charbuf;
}

/*
 * Like transchar(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of the current buffer.
 */
    char_u *
transchar_byte(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු transchar_byte_buf(curbuf, c);
}

/*
 * Like transchar_buf(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of "buf", unless it is NULL.
 */
    char_u *
transchar_byte_buf(buf_T *buf, පූර්ණ_සංඛ්‍යා c)
{
    නම් (enc_utf8 && c >= 0x80)
    {
	transchar_nonprint(buf, transchar_charbuf, c);
	ආපසු transchar_charbuf;
    }
    ආපසු transchar_buf(buf, c);
}
/*
 * Convert non-printable character to two or more printable characters in
 * "charbuf[]".  "charbuf" needs to be able to hold five bytes.
 * Does NOT work for multi-byte characters, c must be <= 255.
 */
    void
transchar_nonprint(buf_T *buf, char_u *charbuf, පූර්ණ_සංඛ්‍යා c)
{
    නම් (c == NL)
	c = NUL;		// we use newline in place of a NUL
    නැතහොත් නම් (buf != NULL && c == CAR && get_fileformat(buf) == EOL_MAC)
	c = NL;			// we use CR in place of  NL in this case

    නම් (dy_flags & DY_UHEX)		// 'display' has "uhex"
	transchar_hex(charbuf, c);

    නැතහොත් නම් (c <= 0x7f)			// 0x00 - 0x1f and 0x7f
    {
	charbuf[0] = '^';
	charbuf[1] = c ^ 0x40;		// DEL displayed as ^?
	charbuf[2] = NUL;
    }
    නැතහොත් නම් (enc_utf8)
    {
	transchar_hex(charbuf, c);
    }
    නැතහොත් නම් (c >= ' ' + 0x80 && c <= '~' + 0x80)    // 0xa0 - 0xfe
    {
	charbuf[0] = '|';
	charbuf[1] = c - 0x80;
	charbuf[2] = NUL;
    }
    නැතහොත්					    // 0x80 - 0x9f and 0xff
    {
	charbuf[0] = '~';
	charbuf[1] = (c - 0x80) ^ 0x40;	// 0xff displayed as ~?
	charbuf[2] = NUL;
    }
}

    void
transchar_hex(char_u *buf, පූර්ණ_සංඛ්‍යා c)
{
    පූර්ණ_සංඛ්‍යා		i = 0;

    buf[0] = '<';
    නම් (c > 255)
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
    static unsigned
nr2hex(unsigned c)
{
    නම් ((c & 0xf) <= 9)
	ආපසු (c & 0xf) + '0';
    ආපසු (c & 0xf) - 10 + 'a';
}

/*
 * Return number of display cells occupied by byte "b".
 * Caller must make sure 0 <= b <= 255.
 * For multi-byte mode "b" must be the first byte of a character.
 * A TAB is counted as two cells: "^I".
 * For UTF-8 mode this will return 0 for bytes >= 0x80, because the number of
 * cells depends on further bytes.
 */
    පූර්ණ_සංඛ්‍යා
byte2cells(පූර්ණ_සංඛ්‍යා b)
{
    නම් (enc_utf8 && b >= 0x80)
	ආපසු 0;
    ආපසු (g_chartab[b] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character "c".
 * "c" can be a special key (negative number) in which case 3 or 4 is returned.
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    පූර්ණ_සංඛ්‍යා
char2cells(පූර්ණ_සංඛ්‍යා c)
{
    නම් (IS_SPECIAL(c))
	ආපසු char2cells(K_SECOND(c)) + 2;
    නම් (c >= 0x80)
    {
	// UTF-8: above 0x80 need to check the value
	නම් (enc_utf8)
	    ආපසු utf_char2cells(c);
	// DBCS: double-byte means double-width, except for euc-jp with first
	// byte 0x8e
	නම් (enc_dbcs != 0 && c >= 0x100)
	{
	    නම් (enc_dbcs == DBCS_JPNU && ((unsigned)c >> 8) == 0x8e)
		ආපසු 1;
	    ආපසු 2;
	}
    }
    ආපසු (g_chartab[c & 0xff] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character at "*p".
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    පූර්ණ_සංඛ්‍යා
ptr2cells(char_u *p)
{
    නම් (!has_mbyte)
	ආපසු byte2cells(*p);

    // For UTF-8 we need to look at more bytes if the first byte is >= 0x80.
    නම් (enc_utf8 && *p >= 0x80)
	ආපසු utf_ptr2cells(p);
    // For DBCS we can tell the cell count from the first byte.
    ආපසු (g_chartab[*p] & CT_CELL_MASK);
}

/*
 * Return the number of character cells string "s" will take on the screen,
 * counting TABs as two characters: "^I".
 */
    පූර්ණ_සංඛ්‍යා
vim_strsize(char_u *s)
{
    ආපසු vim_strnsize(s, (පූර්ණ_සංඛ්‍යා)MAXCOL);
}

/*
 * Return the number of character cells string "s[len]" will take on the
 * screen, counting TABs as two characters: "^I".
 */
    පූර්ණ_සංඛ්‍යා
vim_strnsize(char_u *s, පූර්ණ_සංඛ්‍යා len)
{
    පූර්ණ_සංඛ්‍යා		size = 0;

    අතරතුර (*s != NUL && --len >= 0)
    {
	පූර්ණ_සංඛ්‍යා	    l = (*mb_ptr2len)(s);

	size += ptr2cells(s);
	s += l;
	len -= l - 1;
    }

    ආපසු size;
}

/*
 * Return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab.
 * Use a define to make it fast, this is used very often!!!
 * Also see getvcol() below.
 */

#ifdef FEAT_VARTABS
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, col) \
    නම් (*(p) == TAB && (!(wp)->w_p_list || (wp)->w_lcs_chars.tab1)) \
    { \
	ආපසු tabstop_padding(col, (buf)->b_p_ts, (buf)->b_p_vts_array); \
    } \
    නැතහොත් \
	ආපසු ptr2cells(p);
#නැතහොත්
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, col) \
    නම් (*(p) == TAB && (!(wp)->w_p_list || wp->w_lcs_chars.tab1)) \
    { \
	පූර්ණ_සංඛ්‍යා ts; \
	ts = (buf)->b_p_ts; \
	ආපසු (පූර්ණ_සංඛ්‍යා)(ts - (col % ts)); \
    } \
    නැතහොත් \
	ආපසු ptr2cells(p);
#endif

    පූර්ණ_සංඛ්‍යා
chartabsize(char_u *p, colnr_T col)
{
    RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, p, col)
}

#නම් defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    පූර්ණ_සංඛ්‍යා
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
    පූර්ණ_සංඛ්‍යා
linetabsize_str(char_u *s)
{
    ආපසු linetabsize_col(0, s);
}

/*
 * Like linetabsize_str(), but "s" starts at column "startcol".
 */
    පූර්ණ_සංඛ්‍යා
linetabsize_col(පූර්ණ_සංඛ්‍යා startcol, char_u *s)
{
    chartabsize_T cts;
    vimlong_T vcol;

    init_chartabsize_arg(&cts, curwin, 0, startcol, s, s);
    vcol = cts.cts_vcol;

    අතරතුර (*cts.cts_ptr != NUL)
    {
	vcol += lbr_chartabsize_adv(&cts);
	නම් (vcol > MAXCOL)
	{
	    cts.cts_vcol = MAXCOL;
	    bha_nga;
	}
	නැතහොත්
	    cts.cts_vcol = (පූර්ණ_සංඛ්‍යා)vcol;
    }
    clear_chartabsize_arg(&cts);
    ආපසු (පූර්ණ_සංඛ්‍යා)cts.cts_vcol;
}

/*
 * Like linetabsize_str(), but for a given window instead of the current one.
 * Doesn't count the size of 'listchars' "eol".
 */
    පූර්ණ_සංඛ්‍යා
win_linetabsize(win_T *wp, linenr_T lnum, char_u *line, colnr_T len)
{
    chartabsize_T cts;

    init_chartabsize_arg(&cts, wp, lnum, 0, line, line);
    win_linetabsize_cts(&cts, len);
    clear_chartabsize_arg(&cts);
    ආපසු (පූර්ණ_සංඛ්‍යා)cts.cts_vcol;
}

/*
 * Return the number of cells line "lnum" of window "wp" will take on the
 * screen, taking into account the size of a tab and text properties.
 * Doesn't count the size of 'listchars' "eol".
 */
    පූර්ණ_සංඛ්‍යා
linetabsize(win_T *wp, linenr_T lnum)
{
    ආපසු win_linetabsize(wp, lnum,
		       ml_get_buf(wp->w_buffer, lnum, FALSE), (colnr_T)MAXCOL);
}

/*
 * Like linetabsize(), but counts the size of 'listchars' "eol".
 */
    පූර්ණ_සංඛ්‍යා
linetabsize_eol(win_T *wp, linenr_T lnum)
{
    ආපසු linetabsize(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Like linetabsize(), but excludes 'above'/'after'/'right'/'below' aligned
 * virtual text, while keeping inline virtual text.
 */
    පූර්ණ_සංඛ්‍යා
linetabsize_no_outer(win_T *wp, linenr_T lnum)
{
#ifndef FEAT_PROP_POPUP
    ආපසු linetabsize(wp, lnum);
#නැතහොත්
    chartabsize_T cts;
    char_u *line = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, line, line);

    නම් (cts.cts_text_prop_count)
    {
	පූර්ණ_සංඛ්‍යා write_idx = 0;
	සඳහා (පූර්ණ_සංඛ්‍යා read_idx = 0; read_idx < cts.cts_text_prop_count; read_idx++)
	{
	    textprop_T *tp = &cts.cts_text_props[read_idx];
	    නම් (tp->tp_col != MAXCOL)
	    {
		නම් (read_idx != write_idx)
		    cts.cts_text_props[write_idx] = *tp;
		write_idx++;
	    }
	}
	cts.cts_text_prop_count = write_idx;
	නම් (cts.cts_text_prop_count == 0)
	    VIM_CLEAR(cts.cts_text_props);
    }

    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    ආපසු (පූර්ණ_සංඛ්‍යා)cts.cts_vcol;
#endif
}

/*
 * Like linetabsize_no_outer(), but counts the size of 'listchars' "eol".
 */
    පූර්ණ_සංඛ්‍යා
linetabsize_no_outer_eol(win_T *wp, linenr_T lnum)
{
    ආපසු linetabsize_no_outer(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Return TRUE when win_lbr_chartabsize() does nothing more than
 * win_nolbr_chartabsize(): no 'linebreak', 'breakindent', 'showbreak' and no
 * text properties that insert text.
 */
#නම් defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    static පූර්ණ_සංඛ්‍යා
win_lbr_is_simple_width(chartabsize_T *cts)
{
    ආපසු TRUE
# ifdef FEAT_LINEBREAK
	&& !cts->cts_win->w_p_lbr && !cts->cts_win->w_p_bri
	&& *get_showbreak_value(cts->cts_win) == NUL
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_text
# endif
	;
}
#endif

    void
win_linetabsize_cts(chartabsize_T *cts, colnr_T len)
{
    vimlong_T vcol = cts->cts_vcol;
#නම් defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    // Fast path: when win_lbr_chartabsize() would just return
    // win_nolbr_chartabsize() (wrap on; no 'linebreak'/'breakindent'/
    // 'showbreak'/inserted text properties), call it directly.
    පූර්ණ_සංඛ්‍යා simple_width = cts->cts_win->w_p_wrap && win_lbr_is_simple_width(cts);
#endif
#ifdef FEAT_PROP_POPUP
    cts->cts_with_trailing = len == MAXCOL;
#endif
    සඳහා ( ; *cts->cts_ptr != NUL && (len == MAXCOL || cts->cts_ptr < cts->cts_line + len);
						      MB_PTR_ADV(cts->cts_ptr))
    {
#නම් defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
	නම් (simple_width)
	{
	    පූර්ණ_සංඛ්‍යා	c = *cts->cts_ptr;

	    නම් (c < 0x80 && c != TAB)
		vcol += g_chartab[c] & CT_CELL_MASK;
	    නැතහොත්
		vcol += win_nolbr_chartabsize(cts, NULL);
	}
	නැතහොත්
#endif
	    vcol += win_lbr_chartabsize(cts, NULL, NULL);
	නම් (vcol > MAXCOL)
	{
	    cts->cts_vcol = MAXCOL;
	    bha_nga;
	}
	නැතහොත්
	    cts->cts_vcol = (පූර්ණ_සංඛ්‍යා)vcol;
    }
#ifdef FEAT_PROP_POPUP
    // check for a virtual text at the end of a line or on an empty line
    නම් (len == MAXCOL && cts->cts_has_prop_with_text && *cts->cts_ptr == NUL)
    {
	පූර්ණ_සංඛ්‍යා head = 0;
	(void)win_lbr_chartabsize(cts, &head, NULL);
	vcol += cts->cts_cur_text_width + head;
	// When properties are above the empty line must also be counted.  For
	// a property below the width already includes filling up the line.
	නම් (cts->cts_ptr == cts->cts_line && cts->cts_prop_lines > 0
							 && !cts->cts_has_below)
	    ++vcol;
	cts->cts_vcol = vcol > MAXCOL ? MAXCOL : (පූර්ණ_සංඛ්‍යා)vcol;
    }
#endif
}

/*
 * Return TRUE if 'c' is a normal identifier character:
 * Letters and characters from the 'isident' option.
 */
    පූර්ණ_සංඛ්‍යා
vim_isIDc(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු (c > 0 && c < 0x100 && (g_chartab[c] & CT_ID_CHAR));
}

/*
 * Like vim_isIDc() but not using the 'isident' option: letters, numbers and
 * underscore.
 */
    පූර්ණ_සංඛ්‍යා
vim_isNormalIDc(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු ASCII_ISALNUM(c) || c == '_';
}

/*
 * return TRUE if 'c' is a keyword character: Letters and characters from
 * 'iskeyword' option for the current buffer.
 * For multi-byte characters mb_get_class() is used (builtin rules).
 */
    පූර්ණ_සංඛ්‍යා
vim_iswordc(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු vim_iswordc_buf(c, curbuf);
}

    පූර්ණ_සංඛ්‍යා
vim_iswordc_buf(පූර්ණ_සංඛ්‍යා c, buf_T *buf)
{
    නම් (c >= 0x100)
    {
	නම් (enc_dbcs != 0)
	    ආපසු dbcs_class((unsigned)c >> 8, (unsigned)(c & 0xff)) >= 2;
	නම් (enc_utf8)
	    ආපසු utf_class_buf(c, buf) >= 2;
	ආපසු FALSE;
    }
    ආපසු (c > 0 && GET_CHARTAB(buf, c) != 0);
}

/*
 * Just like vim_iswordc() but uses a pointer to the (multi-byte) character.
 */
    පූර්ණ_සංඛ්‍යා
vim_iswordp(char_u *p)
{
    ආපසු vim_iswordp_buf(p, curbuf);
}

    පූර්ණ_සංඛ්‍යා
vim_iswordp_buf(char_u *p, buf_T *buf)
{
    පූර්ණ_සංඛ්‍යා	c = *p;

    නම් (has_mbyte && MB_BYTE2LEN(c) > 1)
	c = (*mb_ptr2char)(p);
    ආපසු vim_iswordc_buf(c, buf);
}

/*
 * Return TRUE if 'c' is a valid file-name character as specified with the
 * 'isfname' option.
 * Assume characters above 0x100 are valid (multi-byte).
 * To be used for commands like "gf".
 */
    පූර්ණ_සංඛ්‍යා
vim_isfilec(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_FNAME_CHAR)));
}

#නම් defined(FEAT_SPELL)
/*
 * Return TRUE if 'c' is a valid file-name character, including characters left
 * out of 'isfname' to make "gf" work, such as comma, space, '@', etc.
 */
    පූර්ණ_සංඛ්‍යා
vim_is_fname_char(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු vim_isfilec(c) || c == ',' || c == ' ' || c == '@';
}
#endif

/*
 * return TRUE if 'c' is a valid file-name character or a wildcard character
 * Assume characters above 0x100 are valid (multi-byte).
 * Explicitly interpret ']' as a wildcard character as mch_has_wildcard("]")
 * returns false.
 */
    පූර්ණ_සංඛ්‍යා
vim_isfilec_or_wc(පූර්ණ_සංඛ්‍යා c)
{
    char_u buf[2];

    buf[0] = (char_u)c;
    buf[1] = NUL;
    ආපසු vim_isfilec(c) || c == ']' || mch_has_wildcard(buf);
}

/*
 * Return TRUE if 'c' is a printable character.
 * Assume characters above 0x100 are printable (multi-byte), except for
 * Unicode.
 */
    පූර්ණ_සංඛ්‍යා
vim_isprintc(පූර්ණ_සංඛ්‍යා c)
{
    නම් (enc_utf8 && c >= 0x100)
	ආපසු utf_printable(c);
    ආපසු (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Strict version of vim_isprintc(c), don't return TRUE if "c" is the head
 * byte of a double-byte character.
 */
    පූර්ණ_සංඛ්‍යා
vim_isprintc_strict(පූර්ණ_සංඛ්‍යා c)
{
    නම් (enc_dbcs != 0 && c < 0x100 && MB_BYTE2LEN(c) > 1)
	ආපසු FALSE;
    නම් (enc_utf8 && c >= 0x100)
	ආපසු utf_printable(c);
    ආපසු (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Prepare the structure passed to chartabsize functions.
 * "line" is the start of the line, "ptr" is the first relevant character.
 * When "lnum" is zero do not use text properties that insert text.
 */
    void
init_chartabsize_arg(
	chartabsize_T	*cts,
	win_T		*wp,
	linenr_T	lnum UNUSED,
	colnr_T		col,
	char_u		*line,
	char_u		*ptr)
{
    CLEAR_POINTER(cts);
    cts->cts_win = wp;
    cts->cts_vcol = col;
    cts->cts_line = line;
    cts->cts_ptr = ptr;
#ifdef FEAT_LINEBREAK
    cts->cts_bri_size = -1;
#endif
#ifdef FEAT_PROP_POPUP
    නම් (lnum > 0 && !ignore_text_props)
    {
	char_u	*prop_start;
	පූර්ණ_සංඛ්‍යා	count;

	count = get_text_props(wp->w_buffer, lnum, &prop_start, FALSE);
	cts->cts_text_prop_count = count;
	නම් (count > 0)
	{
	    // Make a copy of the properties, so that they are properly
	    // aligned.  Make it twice as long for the sorting below.
	    cts->cts_text_props = ALLOC_MULT(textprop_T, count * 2);
	    නම් (cts->cts_text_props == NULL)
		cts->cts_text_prop_count = 0;
	    නැතහොත්
	    {
		පූර්ණ_සංඛ්‍යා	i;

		mch_memmove(cts->cts_text_props + count, prop_start,
						   count * parimaa_na(textprop_T));
		සඳහා (i = 0; i < count; ++i)
		{
		    textprop_T *tp = cts->cts_text_props + i + count;
		    නම් (tp->tp_id < 0
				     && text_prop_type_valid(wp->w_buffer, tp))
		    {
			cts->cts_has_prop_with_text = TRUE;
			bha_nga;
		    }
		}
		නම් (!cts->cts_has_prop_with_text)
		{
		    // won't use the text properties, free them
		    VIM_CLEAR(cts->cts_text_props);
		    cts->cts_text_prop_count = 0;
		}
		නැතහොත්
		{
		    පූර්ණ_සංඛ්‍යා	    *text_prop_idxs;

		    // Need to sort the array to get any truncation right.
		    // Do the sorting in the second part of the array, then
		    // move the sorted props to the first part of the array.
		    text_prop_idxs = ALLOC_MULT(පූර්ණ_සංඛ්‍යා, count);
		    නම් (text_prop_idxs != NULL)
		    {
			සඳහා (i = 0; i < count; ++i)
			    text_prop_idxs[i] = i + count;
			sort_text_props(curbuf, cts->cts_text_props,
							text_prop_idxs, count);
			// Here we want the reverse order.
			සඳහා (i = 0; i < count; ++i)
			    cts->cts_text_props[count - i - 1] =
					cts->cts_text_props[text_prop_idxs[i]];
			vim_free(text_prop_idxs);
		    }

		    // Convert tp_text_offset to tp_text pointer.
		    char_u *count_ptr = prop_start - PROP_COUNT_SIZE;

		    සඳහා (i = 0; i < count; ++i)
		    {
			textprop_T *tp = &cts->cts_text_props[i];

			නම් (tp->tp_id < 0 && tp->u.tp_text_offset > 0)
			{
			    tp->u.tp_text = count_ptr + tp->u.tp_text_offset;
			    tp->tp_flags |= TP_FLAG_VTEXT_PTR;
			}
			නැතහොත්
			    tp->u.tp_text = NULL;
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
    void
clear_chartabsize_arg(chartabsize_T *cts UNUSED)
{
#ifdef FEAT_PROP_POPUP
    නම් (cts->cts_text_prop_count > 0)
    {
	VIM_CLEAR(cts->cts_text_props);
	cts->cts_text_prop_count = 0;
    }
#endif
}

/*
 * Like chartabsize(), but also check for line breaks on the screen and text
 * properties that insert text.
 */
    පූර්ණ_සංඛ්‍යා
lbr_chartabsize(chartabsize_T *cts)
{
#නම් defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    නම් (1
# ifdef FEAT_LINEBREAK
	&& !curwin->w_p_lbr && *get_showbreak_value(curwin) == NUL
							   && !curwin->w_p_bri
# endif
# ifdef FEAT_PROP_POPUP
	&& !cts->cts_has_prop_with_text
# endif
       )
    {
#endif
	නම් (curwin->w_p_wrap)
	    ආපසු win_nolbr_chartabsize(cts, NULL);
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, cts->cts_ptr, cts->cts_vcol)
#නම් defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    }
    ආපසු win_lbr_chartabsize(cts, NULL, NULL);
#endif
}

/*
 * Call lbr_chartabsize() and advance the pointer.
 */
    පූර්ණ_සංඛ්‍යා
lbr_chartabsize_adv(chartabsize_T *cts)
{
    පූර්ණ_සංඛ්‍යා		retval;

    retval = lbr_chartabsize(cts);
    MB_PTR_ADV(cts->cts_ptr);
    ආපසු retval;
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
    පූර්ණ_සංඛ්‍යා
win_lbr_chartabsize(
	chartabsize_T	*cts,
	පූර්ණ_සංඛ්‍යා		*headp UNUSED,
	පූර්ණ_සංඛ්‍යා		*tailp UNUSED)
{
    win_T	*wp = cts->cts_win;
#නම් defined(FEAT_PROP_POPUP) || defined(FEAT_LINEBREAK)
    char_u	*line = cts->cts_line; // start of the line
    පූර්ණ_සංඛ්‍යා		size;
#endif
    char_u	*s = cts->cts_ptr;
    colnr_T	vcol = cts->cts_vcol;
#ifdef FEAT_LINEBREAK
    පූර්ණ_සංඛ්‍යා		mb_added = 0;
    පූර්ණ_සංඛ්‍යා		n;
    char_u	*sbr;
    පූර්ණ_සංඛ්‍යා		no_sbr = FALSE;
#endif

#නම් defined(FEAT_PROP_POPUP)
    cts->cts_cur_text_width = 0;
    cts->cts_has_below = false;
    cts->cts_first_char = 0;
#endif

#නම් defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    /*
     * No 'linebreak', 'showbreak', 'breakindent' and text properties that
     * insert text: return quickly.
     */
    නම් (win_lbr_is_simple_width(cts))
#endif
    {
	නම් (wp->w_p_wrap)
	    ආපසු win_nolbr_chartabsize(cts, headp);
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_buffer, s, vcol)
    }

#නම් defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    පූර්ණ_සංඛ්‍යා has_lcs_eol = wp->w_p_list && wp->w_lcs_chars.eol != NUL;
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
    නම් (*s == NUL)
    {
	// 1 cell for EOL list char (if present), as opposed to the two cell ^@
	// for a NUL character in the text.
	size = has_lcs_eol ? 1 : 0;
    }

    පූර්ණ_සංඛ්‍යා is_doublewidth = has_mbyte && size == 2 && MB_BYTE2LEN(*s) > 1;
# endif

# ifdef FEAT_PROP_POPUP
    නම් (cts->cts_has_prop_with_text)
    {
#  ifdef FEAT_LINEBREAK
	පූර්ණ_සංඛ්‍යා	    tab_size = size;
#  endif
	පූර්ණ_සංඛ්‍යා	    charlen = *s == NUL ? 1 : mb_ptr2len(s);
	පූර්ණ_සංඛ්‍යා	    i;
	පූර්ණ_සංඛ්‍යා	    col = (පූර්ණ_සංඛ්‍යා)(s - line);


	// The "$" for 'list' mode will go between the EOL and
	// the text prop, account for that.
	නම් (has_lcs_eol)
	{
	    ++vcol;
	    --size;
	}

	සඳහා (i = 0; i < cts->cts_text_prop_count; ++i)
	{
	    textprop_T	*tp = cts->cts_text_props + i;
	    පූර්ණ_සංඛ්‍යා		col_off = win_col_off(wp);

	    // Watch out for the text being deleted.  "cts_text_props" is a
	    // copy, the text prop may actually have been removed from the line.
	    නම් (tp->tp_id < 0
		    && ((tp->tp_col - 1 >= col
					     && tp->tp_col - 1 < col + charlen)
		       || (tp->tp_col == MAXCOL
			   && ((tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
				? col == 0
				: s[0] == NUL && cts->cts_with_trailing)))
		    && tp->u.tp_text != NULL)
	    {
		char_u *p = tp->u.tp_text;

		නම් (p != NULL)
		{
		    පූර්ණ_සංඛ්‍යා	cells;

		    නම් (tp->tp_col == MAXCOL)
		    {
			පූර්ණ_සංඛ්‍යා n_extra = (පූර්ණ_සංඛ්‍යා)STRLEN(p);

			cells = text_prop_position(wp, tp, vcol,
			     (vcol + size) % (wp->w_width - col_off) + col_off,
					      &n_extra, &p, NULL, NULL, FALSE);
#  ifdef FEAT_LINEBREAK
			නම් (text_prop_no_showbreak(tp))
			    no_sbr = TRUE;  // don't use 'showbreak' now
#  endif
		    }
		    නැතහොත්
			cells = vim_strsize(p);
		    නම් (tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
		    {
			cts->cts_first_char += cells;
			නම් (!cts->cts_no_above)
			    cts->cts_cur_text_width += cells;
		    }
		    නැතහොත්
		    {
			cts->cts_cur_text_width += cells;
			size += cells;
		    }
		    cts->cts_start_incl = tp->tp_flags & TP_FLAG_START_INCL;
#  ifdef FEAT_LINEBREAK
		    නම් (*s == TAB)
		    {
			// tab size changes because of the inserted text
			size -= tab_size;
			tab_size = win_chartabsize(wp, s,
					vcol + size - cts->cts_above_width);
			size += tab_size;
		    }
#  endif
		    නම් (tp->tp_col == MAXCOL && (tp->tp_flags
				& (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW)))
		    {
			// count extra line for property above/below
			++cts->cts_prop_lines;
			නම් (tp->tp_flags & TP_FLAG_ALIGN_BELOW)
			    cts->cts_has_below = true;
		    }
		}
	    }
	    නම් (tp->tp_col != MAXCOL && tp->tp_col - 1 > col)
		bha_nga;
	}
	නම් (has_lcs_eol)
	{
	    --vcol;
	    ++size;
	}
    }
# endif

# ifdef FEAT_LINEBREAK
    නම් (is_doublewidth && wp->w_p_wrap && in_win_border(wp, vcol + size - 2))
    {
	++size;		// Count the ">" in the last column.
	mb_added = 1;
    }

    /*
     * May have to add something for 'breakindent' and/or 'showbreak'
     * string at the start of a screen line.
     */
    පූර්ණ_සංඛ්‍යා head = mb_added;
    sbr = no_sbr ? empty_option : get_showbreak_value(wp);
    // When "size" is 0, no new screen line is started.
    නම් (size > 0 && wp->w_p_wrap && (*sbr != NUL || wp->w_p_bri))
    {
	පූර්ණ_සංඛ්‍යා	col_off_prev = win_col_off(wp);
	පූර්ණ_සංඛ්‍යා	width2 = wp->w_width - col_off_prev + win_col_off2(wp);
	colnr_T	wcol = vcol + col_off_prev;
#  ifdef FEAT_PROP_POPUP
	wcol -= wp->w_virtcol_first_char;
#  endif
	colnr_T	max_head_vcol = cts->cts_max_head_vcol;
	පූර්ණ_සංඛ්‍යා	added = 0;

	// cells taken by 'showbreak'/'breakindent' before current char
	පූර්ණ_සංඛ්‍යා	head_prev = 0;
	නම් (wcol >= wp->w_width)
	{
	    wcol -= wp->w_width;
	    col_off_prev = wp->w_width - width2;
	    නම් (wcol >= width2 && width2 > 0)
		wcol %= width2;
	    නම් (*sbr != NUL)
		head_prev += vim_strsize(sbr);
	    නම් (wp->w_p_bri)
	    {
		නම් (cts->cts_bri_size < 0)
		    cts->cts_bri_size = get_breakindent_win(wp, line);
		head_prev += cts->cts_bri_size;
	    }
	    නම් (wcol < head_prev)
	    {
		head_prev -= wcol;
		wcol += head_prev;
		added += head_prev;
		නම් (max_head_vcol <= 0 || vcol < max_head_vcol)
		    head += head_prev;
	    }
	    නැතහොත්
		head_prev = 0;
	    wcol += col_off_prev;
	}

	නම් (wcol + size > wp->w_width)
	{
	    // cells taken by 'showbreak'/'breakindent' halfway current char
	    පූර්ණ_සංඛ්‍යා	head_mid = 0;
	    නම් (*sbr != NUL)
		head_mid += vim_strsize(sbr);
	    නම් (wp->w_p_bri)
	    {
		නම් (cts->cts_bri_size < 0)
		    cts->cts_bri_size = get_breakindent_win(wp, line);
		head_mid += cts->cts_bri_size;
	    }
	    නම් (head_mid > 0)
	    {
		// Calculate effective window width.
		පූර්ණ_සංඛ්‍යා prev_rem = wp->w_width - wcol;
		පූර්ණ_සංඛ්‍යා width = width2 - head_mid;

		නම් (width <= 0)
		    width = 1;
		// Divide "size - prev_rem" by "width", rounding up.
		පූර්ණ_සංඛ්‍යා cnt = (size - prev_rem + width - 1) / width;
		added += cnt * head_mid;

		නම් (max_head_vcol == 0 || vcol + size + added < max_head_vcol)
		    head += cnt * head_mid;
		නැතහොත් නම් (width2 > 0
			   && max_head_vcol > vcol + head_prev + prev_rem)
		    head += (max_head_vcol - (vcol + head_prev + prev_rem)
					     + width2 - 1) / width2 * head_mid;
		නැතහොත් නම් (max_head_vcol < 0)
		{
		    පූර්ණ_සංඛ්‍යා off = mb_added;
#  ifdef FEAT_PROP_POPUP
		    නම් (*s != NUL
			     && ((State & MODE_NORMAL) || cts->cts_start_incl))
			off += cts->cts_cur_text_width;
#  endif
		    නම් (off >= prev_rem)
			head += (1 + (off - prev_rem) / width) * head_mid;
		}
	    }
	}

	size += added;
    }

    නම් (headp != NULL)
	*headp = head;

    පූර්ණ_සංඛ්‍යා size_before_lbr = size;
    පූර්ණ_සංඛ්‍යා need_lbr = FALSE;
    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here.
     */
    නම් (wp->w_p_lbr && wp->w_p_wrap && wp->w_width != 0
	    && VIM_ISBREAK((පූර්ණ_සංඛ්‍යා)s[0]) && !VIM_ISBREAK((පූර්ණ_සංඛ්‍යා)s[1]))
    {
	char_u	*t = cts->cts_line;
	අතරතුර (VIM_ISBREAK((පූර්ණ_සංඛ්‍යා)t[0]))
	    t++;
	// 'linebreak' is only needed when not in leading whitespace.
	need_lbr = s >= t;
    }
    නම් (need_lbr)
    {
	/*
	 * Count all characters from first non-blank after a blank up to next
	 * non-blank after a blank.
	 */
	පූර්ණ_සංඛ්‍යා numberextra = win_col_off(wp);
	colnr_T col_adj = size - 1;
	colnr_T colmax = (colnr_T)(wp->w_width - numberextra - col_adj);
	නම් (vcol >= colmax)
	{
	    colmax += col_adj;
	    n = colmax +  win_col_off2(wp);
	    නම් (n > 0)
		colmax += (((vcol + col_adj - colmax) / n) + 1) * n - col_adj;
	}

	colnr_T vcol2 = vcol;
	සඳහා (;;)
	{
	    char_u *ps = s;
	    MB_PTR_ADV(s);
	    පූර්ණ_සංඛ්‍යා c = *s;
	    නම් (!(c != NUL
		    && (VIM_ISBREAK(c)
			|| (!VIM_ISBREAK(c)
			       && (vcol2 == vcol || !VIM_ISBREAK((පූර්ණ_සංඛ්‍යා)*ps))))))
		bha_nga;

	    vcol2 += win_chartabsize(wp, s, vcol2);
	    නම් (vcol2 >= colmax)		// doesn't fit
	    {
		size = colmax - vcol + col_adj;
		bha_nga;
	    }
	}
    }

    නම් (tailp != NULL)
	*tailp = size - size_before_lbr;

#  ifdef FEAT_PROP_POPUP
    නම් (cts->cts_first_char > 0 && !cts->cts_no_above)
    {
	// Remember the width for the size of a Tab later in the line.  Use
	// assignment, this may be called more than once for a character.
	cts->cts_above_width = cts->cts_first_char;
	size += cts->cts_first_char;
    }
#  endif
# endif
    ආපසු size;
#endif
}

/*
 * Like win_lbr_chartabsize(), except that we know 'linebreak' is off, 'wrap'
 * is on and there are no properties that insert text.  This means we need to
 * check for a double-byte character that doesn't fit at the end of the screen
 * line.
 * Only uses "cts_win", "cts_ptr" and "cts_vcol" from "cts".
 */
    static පූර්ණ_සංඛ්‍යා
win_nolbr_chartabsize(
	chartabsize_T	*cts,
	පූර්ණ_සංඛ්‍යා		*headp)
{
    win_T	*wp = cts->cts_win;
    char_u	*s = cts->cts_ptr;
    colnr_T	col = cts->cts_vcol;
    පූර්ණ_සංඛ්‍යා		n;

    නම් (*s == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
    {
#ifdef FEAT_VARTABS
	ආපසු tabstop_padding(col, wp->w_buffer->b_p_ts,
				    wp->w_buffer->b_p_vts_array);
#නැතහොත්
	n = wp->w_buffer->b_p_ts;
	ආපසු (පූර්ණ_සංඛ්‍යා)(n - (col % n));
#endif
    }
    n = ptr2cells(s);
    // Add one cell for a double-width character in the last column of the
    // window, displayed with a ">".
    නම් (n == 2 && MB_BYTE2LEN(*s) > 1 && in_win_border(wp, col))
    {
	නම් (headp != NULL)
	    *headp = 1;
	ආපසු 3;
    }
    ආපසු n;
}

/*
 * Return TRUE if virtual column "vcol" is in the rightmost column of window
 * "wp".
 */
    static පූර්ණ_සංඛ්‍යා
in_win_border(win_T *wp, colnr_T vcol)
{
    පූර්ණ_සංඛ්‍යා		width1;		// width of first line (after line number)
    පූර්ණ_සංඛ්‍යා		width2;		// width of further lines

    නම් (wp->w_width == 0)	// there is no border
	ආපසු FALSE;
    width1 = wp->w_width - win_col_off(wp);
    නම් ((පූර්ණ_සංඛ්‍යා)vcol < width1 - 1)
	ආපසු FALSE;
    නම් ((පූර්ණ_සංඛ්‍යා)vcol == width1 - 1)
	ආපසු TRUE;
    width2 = width1 + win_col_off2(wp);
    නම් (width2 <= 0)
	ආපසු FALSE;
    ආපසු ((vcol - width1) % width2 == width2 - 1);
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
    void
getvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*start,
    colnr_T	*cursor,
    colnr_T	*end,
    පූර්ණ_සංඛ්‍යා		flags)
{
    colnr_T	vcol;
    char_u	*ptr;		// points to current char
    char_u	*line;		// start of the line
    පූර්ණ_සංඛ්‍යා		incr;
    පූර්ණ_සංඛ්‍යා		head;
    පූර්ණ_සංඛ්‍යා		tail;
#ifdef FEAT_VARTABS
    පූර්ණ_සංඛ්‍යා		*vts = wp->w_buffer->b_p_vts_array;
#endif
    පූර්ණ_සංඛ්‍යා		ts = wp->w_buffer->b_p_ts;
    පූර්ණ_සංඛ්‍යා		c;
    chartabsize_T cts;
#ifdef FEAT_PROP_POPUP
    පූර්ණ_සංඛ්‍යා		on_NUL = FALSE;
#endif

    vcol = 0;
    line = ptr = ml_get_buf(wp->w_buffer, pos->lnum, FALSE);

    init_chartabsize_arg(&cts, wp, pos->lnum, 0, line, line);
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
    නම් ((!wp->w_p_list || wp->w_lcs_chars.tab1 != NUL)
#ifdef FEAT_LINEBREAK
	    && !wp->w_p_lbr && *get_showbreak_value(wp) == NUL && !wp->w_p_bri
#endif
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
       )
    {
	සඳහා (;;)
	{
	    head = 0;
	    c = *ptr;
	    // make sure we don't go past the end of the line
	    නම් (c == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
		bha_nga;
	    }
	    // A tab gets expanded, depending on the current column
	    නම් (c == TAB)
#ifdef FEAT_VARTABS
		incr = tabstop_padding(vcol, ts, vts);
#නැතහොත්
		incr = ts - (vcol % ts);
#endif
	    නැතහොත්
	    {
		නම් (has_mbyte)
		{
		    // For utf-8, if the byte is >= 0x80, need to look at
		    // further bytes to find the cell width.
		    නම් (enc_utf8 && c >= 0x80)
			incr = utf_ptr2cells(ptr);
		    නැතහොත්
			incr = g_chartab[c] & CT_CELL_MASK;

		    // If a double-cell char doesn't fit at the end of a line
		    // it wraps to the next line, it's like this char is three
		    // cells wide.
		    නම් (incr == 2 && wp->w_p_wrap && MB_BYTE2LEN(*ptr) > 1
			    && in_win_border(wp, vcol))
		    {
			++incr;
			head = 1;
		    }
		}
		නැතහොත්
		    incr = g_chartab[c] & CT_CELL_MASK;
	    }

	    char_u *next_ptr = ptr + (*mb_ptr2len)(ptr);
	    නම් (next_ptr - line > pos->col) // character at pos->col
		bha_nga;

	    vcol += incr;
	    ptr = next_ptr;
	}

	tail = 0;
    }
    නැතහොත්
    {
	සඳහා (;;)
	{
	    // A tab gets expanded, depending on the current column.
	    // Other things also take up space.
	    head = 0;
	    tail = 0;
	    incr = win_lbr_chartabsize(&cts, &head, &tail);
#ifdef FEAT_PROP_POPUP
	    නම් (cts.cts_ptr == cts.cts_line)
	    {
		නම් (flags & GETVCOL_FOR_VIRTCOL)
		    // do not count the virtual text above for w_curswant
		    wp->w_virtcol_first_char = cts.cts_first_char;
		නම් ((flags & GETVCOL_NO_ABOVE) == 0)
		{
		    cts.cts_vcol += cts.cts_first_char;
		    cts.cts_above_width = cts.cts_first_char;
		}
	    }
#endif
	    // make sure we don't go past the end of the line
	    නම් (*cts.cts_ptr == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
#ifdef FEAT_PROP_POPUP
		incr += cts.cts_cur_text_width;
		on_NUL = TRUE;
#endif
		bha_nga;
	    }

	    char_u *next_ptr = cts.cts_ptr + (*mb_ptr2len)(cts.cts_ptr);
	    නම් (next_ptr - line > pos->col) // character at pos->col
		bha_nga;

	    cts.cts_vcol += incr;
	    cts.cts_ptr = next_ptr;
	}
	vcol = cts.cts_vcol;
	ptr = cts.cts_ptr;
    }
    clear_chartabsize_arg(&cts);

    නම් (*ptr == NUL && pos->col < MAXCOL && pos->col > ptr - line)
	pos->col = ptr - line;

    නම් (start != NULL)
	*start = vcol + head;
    නම් (end != NULL)
	*end = vcol + incr - (flags & GETVCOL_END_EXCL_LBR ? tail : 0) - 1;
    නම් (cursor != NULL)
    {
	නම් (*ptr == TAB
		&& (State & MODE_NORMAL)
		&& !wp->w_p_list
		&& !virtual_active()
		&& !(VIsual_active
				&& (*p_sel == 'e' || LTOREQ_POS(*pos, VIsual)))
		)
	    // TODO: subtracting "tail" may lead to better cursor position
	    *cursor = vcol + incr - 1;	    // cursor at end
	නැතහොත්
	{
#ifdef FEAT_PROP_POPUP
	    // in Insert mode, if "start_incl" is true the text gets inserted
	    // after the virtual text, thus add its width
	    නම් (((State & MODE_INSERT) == 0 || cts.cts_start_incl) && !on_NUL)
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
    පූර්ණ_සංඛ්‍යා		list_save = curwin->w_p_list;
    colnr_T	vcol;

    curwin->w_p_list = FALSE;
    නම් (posp->coladd)
	getvvcol(curwin, posp, NULL, &vcol, NULL, 0);
    නැතහොත්
	getvcol(curwin, posp, NULL, &vcol, NULL, 0);
    curwin->w_p_list = list_save;
    ආපසු vcol;
}

/*
 * Get virtual column in virtual mode.
 */
    void
getvvcol(
    win_T	*wp,
    pos_T	*pos,
    colnr_T	*start,
    colnr_T	*cursor,
    colnr_T	*end,
    පූර්ණ_සංඛ්‍යා		flags)
{
    colnr_T	col;
    colnr_T	coladd;
    colnr_T	endadd;
    char_u	*ptr;

    නම් (virtual_active())
    {
	// For virtual mode, only want one value
	getvcol(wp, pos, &col, NULL, NULL, flags);

	coladd = pos->coladd;
	endadd = 0;
	// Cannot put the cursor on part of a wide character.
	ptr = ml_get_buf(wp->w_buffer, pos->lnum, FALSE);
	නම් (pos->col < ml_get_buf_len(wp->w_buffer, pos->lnum))
	{
	    පූර්ණ_සංඛ්‍යා c = (*mb_ptr2char)(ptr + pos->col);

	    නම් (c != TAB && vim_isprintc(c))
	    {
		endadd = (colnr_T)(char2cells(c) - 1);
		නම් (coladd > endadd)	// past end of line
		    endadd = 0;
		නැතහොත්
		    coladd = 0;
	    }
	}
	col += coladd;
	නම් (start != NULL)
	    *start = col;
	නම් (cursor != NULL)
	    *cursor = col;
	නම් (end != NULL)
	    *end = col + endadd;
    }
    නැතහොත්
	getvcol(wp, pos, start, cursor, end, flags);
}

/*
 * Get the leftmost and rightmost virtual column of pos1 and pos2.
 * Used for Visual block mode.
 */
    void
getvcols(
    win_T	*wp,
    pos_T	*pos1,
    pos_T	*pos2,
    colnr_T	*left,
    colnr_T	*right,
    පූර්ණ_සංඛ්‍යා		flags)
{
    colnr_T	from1, from2, to1, to2;

    නම් (LT_POSP(pos1, pos2))
    {
	getvvcol(wp, pos1, &from1, NULL, &to1, flags);
	getvvcol(wp, pos2, &from2, NULL, &to2, flags);
    }
    නැතහොත්
    {
	getvvcol(wp, pos2, &from1, NULL, &to1, flags);
	getvvcol(wp, pos1, &from2, NULL, &to2, flags);
    }
    නම් (from2 < from1)
	*left = from2;
    නැතහොත්
	*left = from1;
    නම් (to2 > to1)
    {
	නම් (*p_sel == 'e' && from2 - 1 >= to1)
	    *right = from2 - 1;
	නැතහොත්
	    *right = to2;
    }
    නැතහොත්
	*right = to1;
}

/*
 * Skip over ' ' and '\t'.
 */
    char_u *
skipwhite(char_u *q)
{
    char_u	*p = q;

    අතරතුර (VIM_ISWHITE(*p))
	++p;
    ආපසු p;
}

#නම් defined(FEAT_EVAL)
/*
 * skip over ' ', '\t' and '\n'.
 */
    char_u *
skipwhite_and_nl(char_u *q)
{
    char_u	*p = q;

    අතරතුර (VIM_ISWHITE(*p) || *p == NL)
	++p;
    ආපසු p;
}
#endif

/*
 * getwhitecols: return the number of whitespace
 * columns (bytes) at the start of a given line
 */
    පූර්ණ_සංඛ්‍යා
getwhitecols_curline(void)
{
    ආපසු getwhitecols(ml_get_curline());
}

    පූර්ණ_සංඛ්‍යා
getwhitecols(char_u *p)
{
    ආපසු skipwhite(p) - p;
}

/*
 * skip over digits
 */
    char_u *
skipdigits(char_u *q)
{
    char_u	*p = q;

    අතරතුර (VIM_ISDIGIT(*p))	// skip to next non-digit
	++p;
    ආපසු p;
}

#නම් defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * skip over binary digits
 */
    char_u *
skipbin(char_u *q)
{
    char_u	*p = q;

    අතරතුර (vim_isbdigit(*p))	// skip to next non-digit
	++p;
    ආපසු p;
}

/*
 * skip over digits and hex characters
 */
    char_u *
skiphex(char_u *q)
{
    char_u	*p = q;

    අතරතුර (vim_isxdigit(*p))	// skip to next non-digit
	++p;
    ආපසු p;
}
#endif

/*
 * skip to bin digit (or NUL after the string)
 */
    char_u *
skiptobin(char_u *q)
{
    char_u	*p = q;

    අතරතුර (*p != NUL && !vim_isbdigit(*p))	// skip to next digit
	++p;
    ආපසු p;
}

/*
 * skip to digit (or NUL after the string)
 */
    char_u *
skiptodigit(char_u *q)
{
    char_u	*p = q;

    අතරතුර (*p != NUL && !VIM_ISDIGIT(*p))	// skip to next digit
	++p;
    ආපසු p;
}

/*
 * skip to hex character (or NUL after the string)
 */
    char_u *
skiptohex(char_u *q)
{
    char_u	*p = q;

    අතරතුර (*p != NUL && !vim_isxdigit(*p))	// skip to next digit
	++p;
    ආපසු p;
}

/*
 * Variant of isdigit() that can handle characters > 0x100.
 * We don't use isdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 * Use the VIM_ISDIGIT() macro for simple arguments.
 */
    පූර්ණ_සංඛ්‍යා
vim_isdigit(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු (c >= '0' && c <= '9');
}

/*
 * Variant of isxdigit() that can handle characters > 0x100.
 * We don't use isxdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 */
    පූර්ණ_සංඛ්‍යා
vim_isxdigit(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'f')
	|| (c >= 'A' && c <= 'F');
}

/*
 * Corollary of vim_isdigit and vim_isxdigit() that can handle
 * characters > 0x100.
 */
    පූර්ණ_සංඛ්‍යා
vim_isbdigit(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු (c == '0' || c == '1');
}

    static පූර්ණ_සංඛ්‍යා
vim_isodigit(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු (c >= '0' && c <= '7');
}

/*
 * Vim's own character class functions.  These exist because many library
 * islower()/toupper() etc. do not work properly: they crash when used with
 * invalid values or can't handle latin1 when the locale is C.
 * Speed is most important here.
 */
#define LATIN1LOWER 'l'
#define LATIN1UPPER 'U'

static char_u latin1flags[257] = "                                                                 UUUUUUUUUUUUUUUUUUUUUUUUUU      llllllllllllllllllllllllll                                                                     UUUUUUUUUUUUUUUUUUUUUUU UUUUUUUllllllllllllllllllllllll llllllll";
static char_u latin1upper[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xf7\xd8\xd9\xda\xdb\xdc\xdd\xde\xff";
static char_u latin1lower[257] = "                                 !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xd7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    පූර්ණ_සංඛ්‍යා
vim_islower(පූර්ණ_සංඛ්‍යා c)
{
    නම් (c <= '@')
	ආපසු FALSE;
    නම් (c >= 0x80)
    {
	නම් (enc_utf8)
	    ආපසු utf_islower(c);
	නම් (c >= 0x100)
	{
#ifdef HAVE_ISWLOWER
	    නම් (has_mbyte)
		ආපසු iswlower(c);
#endif
	    // islower() can't handle these chars and may crash
	    ආපසු FALSE;
	}
	නම් (enc_latin1like)
	    ආපසු (latin1flags[c] & LATIN1LOWER) == LATIN1LOWER;
    }
    ආපසු SAFE_islower(c);
}

    පූර්ණ_සංඛ්‍යා
vim_isupper(පූර්ණ_සංඛ්‍යා c)
{
    නම් (c <= '@')
	ආපසු FALSE;
    නම් (c >= 0x80)
    {
	නම් (enc_utf8)
	    ආපසු utf_isupper(c);
	නම් (c >= 0x100)
	{
#ifdef HAVE_ISWUPPER
	    නම් (has_mbyte)
		ආපසු iswupper(c);
#endif
	    // islower() can't handle these chars and may crash
	    ආපසු FALSE;
	}
	නම් (enc_latin1like)
	    ආපසු (latin1flags[c] & LATIN1UPPER) == LATIN1UPPER;
    }
    ආපසු SAFE_isupper(c);
}

    පූර්ණ_සංඛ්‍යා
vim_isalpha(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු vim_islower(c) || vim_isupper(c);
}

    පූර්ණ_සංඛ්‍යා
vim_toupper(පූර්ණ_සංඛ්‍යා c)
{
    නම් (c <= '@')
	ආපසු c;
    නම් (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	නම් (enc_utf8)
	    ආපසු utf_toupper(c);
	නම් (c >= 0x100)
	{
#ifdef HAVE_TOWUPPER
	    නම් (has_mbyte)
		ආපසු towupper(c);
#endif
	    // toupper() can't handle these chars and may crash
	    ආපසු c;
	}
	නම් (enc_latin1like)
	    ආපසු latin1upper[c];
    }
    නම් (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ආපසු TOUPPER_ASC(c);
    ආපසු TOUPPER_LOC(c);
}

    පූර්ණ_සංඛ්‍යා
vim_tolower(පූර්ණ_සංඛ්‍යා c)
{
    නම් (c <= '@')
	ආපසු c;
    නම් (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	නම් (enc_utf8)
	    ආපසු utf_tolower(c);
	නම් (c >= 0x100)
	{
#ifdef HAVE_TOWLOWER
	    නම් (has_mbyte)
		ආපසු towlower(c);
#endif
	    // tolower() can't handle these chars and may crash
	    ආපසු c;
	}
	නම් (enc_latin1like)
	    ආපසු latin1lower[c];
    }
    නම් (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	ආපසු TOLOWER_ASC(c);
    ආපසු TOLOWER_LOC(c);
}

/*
 * skiptowhite: skip over text until ' ' or '\t' or NUL.
 */
    char_u *
skiptowhite(char_u *p)
{
    අතරතුර (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    ආපසු p;
}

/*
 * skiptowhite_esc: Like skiptowhite(), but also skip escaped chars
 */
    char_u *
skiptowhite_esc(char_u *p)
{
    අතරතුර (*p != ' ' && *p != '\t' && *p != NUL)
    {
	නම් ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    ආපසු p;
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
    retval = strtol((var_na *)p, NULL, 10);
    නම් (*p == '-')		// skip negative sign
	++p;
    p = skipdigits(p);		// skip to next non-digit
    *pp = p;
    ආපසු retval;
}

/*
 * Like getdigits() but allow for embedded single quotes.
 */
    long
getdigits_quoted(char_u **pp)
{
    char_u	*p = *pp;
    long	retval = 0;

    නම් (*p == '-')
	++p;
    අතරතුර (VIM_ISDIGIT(*p))
    {
	නම් (retval >= LONG_MAX / 10 - 10)
	    retval = LONG_MAX;
	නැතහොත්
	    retval = retval * 10 - '0' + *p;
	++p;
	නම් (in_vim9script() && *p == '\'' && VIM_ISDIGIT(p[1]))
	    ++p;
    }
    නම් (**pp == '-')
    {
	නම් (retval == LONG_MAX)
	    retval = LONG_MIN;
	නැතහොත්
	    retval = -retval;
    }
    *pp = p;
    ආපසු retval;
}

/*
 * Return TRUE if "lbuf" is empty or only contains blanks.
 */
    පූර්ණ_සංඛ්‍යා
vim_isblankline(char_u *lbuf)
{
    char_u	*p;

    p = skipwhite(lbuf);
    ආපසු (*p == NUL || *p == '\r' || *p == '\n');
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
    void
vim_str2nr(
    char_u		*start,
    පූර්ණ_සංඛ්‍යා			*prep,	    // return: type of number 0 = decimal, 'x'
				    // or 'X' is hex, '0', 'o' or 'O' is octal,
				    // 'b' or 'B' is bin
    පූර්ණ_සංඛ්‍යා			*len,	    // return: detected length of number
    පූර්ණ_සංඛ්‍යා			what,	    // what numbers to recognize
    varnumber_T		*nptr,	    // return: signed result
    uvarnumber_T	*unptr,	    // return: unsigned result
    පූර්ණ_සංඛ්‍යා			maxlen,     // max length of string to check
    පූර්ණ_සංඛ්‍යා			strict,     // check strictly
    පූර්ණ_සංඛ්‍යා			*overflow)  // when not NULL set to TRUE for overflow
{
    char_u	    *ptr = start;
    පූර්ණ_සංඛ්‍යා		    pre = 0;		// default is decimal
    පූර්ණ_සංඛ්‍යා		    negative = FALSE;
    uvarnumber_T    un = 0;
    පූර්ණ_සංඛ්‍යා		    n;

    නම් (len != NULL)
	*len = 0;

    නම් (ptr[0] == '-')
    {
	negative = TRUE;
	++ptr;
    }

    // Recognize hex, octal, and bin.
    නම් (ptr[0] == '0' && ptr[1] != '8' && ptr[1] != '9'
					       && (maxlen == 0 || maxlen > 1))
    {
	pre = ptr[1];
	නම් ((what & STR2NR_HEX)
		&& (pre == 'X' || pre == 'x') && vim_isxdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // hexadecimal
	    ptr += 2;
	නැතහොත් නම් ((what & STR2NR_BIN)
		&& (pre == 'B' || pre == 'b') && vim_isbdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // binary
	    ptr += 2;
	නැතහොත් නම් ((what & STR2NR_OOCT)
		&& (pre == 'O' || pre == 'o') && vim_isodigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // octal with prefix "0o"
	    ptr += 2;
	නැතහොත්
	{
	    // decimal or octal, default is decimal
	    pre = 0;
	    නම් (what & STR2NR_OCT)
	    {
		// Don't interpret "0", "08" or "0129" as octal.
		සඳහා (n = 1; n != maxlen && VIM_ISDIGIT(ptr[n]); ++n)
		{
		    නම් (ptr[n] > '7')
		    {
			pre = 0;	// can't be octal
			bha_nga;
		    }
		    pre = '0';	// assume octal
		}
	    }
	}
    }

    // Do the conversion manually to avoid sscanf() quirks.
    n = 1;
    නම් (pre == 'B' || pre == 'b'
			     || ((what & STR2NR_BIN) && (what & STR2NR_FORCE)))
    {
	// bin
	නම් (pre != 0)
	    n += 2;	    // skip over "0b"
	අතරතුර ('0' <= *ptr && *ptr <= '1')
	{
	    // avoid ubsan error for overflow
	    නම් (un <= UVARNUM_MAX / 2)
		un = 2 * un + (uvarnumber_T)(*ptr - '0');
	    නැතහොත්
	    {
		un = UVARNUM_MAX;
		නම් (overflow != NULL)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    නම් (n++ == maxlen)
		bha_nga;
	    නම් ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '1')
	    {
		++ptr;
		නම් (n++ == maxlen)
		    bha_nga;
	    }
	}
    }
    නැතහොත් නම් (pre == 'O' || pre == 'o' ||
		pre == '0' || ((what & STR2NR_OCT) && (what & STR2NR_FORCE)))
    {
	// octal
	නම් (pre != 0 && pre != '0')
	    n += 2;	    // skip over "0o"
	අතරතුර ('0' <= *ptr && *ptr <= '7')
	{
	    // avoid ubsan error for overflow
	    නම් (un <= UVARNUM_MAX / 8)
		un = 8 * un + (uvarnumber_T)(*ptr - '0');
	    නැතහොත්
	    {
		un = UVARNUM_MAX;
		නම් (overflow != NULL)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    නම් (n++ == maxlen)
		bha_nga;
	    නම් ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '7')
	    {
		++ptr;
		නම් (n++ == maxlen)
		    bha_nga;
	    }
	}
    }
    නැතහොත් නම් (pre != 0 || ((what & STR2NR_HEX) && (what & STR2NR_FORCE)))
    {
	// hex
	නම් (pre != 0)
	    n += 2;	    // skip over "0x"
	අතරතුර (vim_isxdigit(*ptr))
	{
	    // avoid ubsan error for overflow
	    නම් (un <= UVARNUM_MAX / 16)
		un = 16 * un + (uvarnumber_T)hex2nr(*ptr);
	    නැතහොත්
	    {
		un = UVARNUM_MAX;
		නම් (overflow != NULL)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    නම් (n++ == maxlen)
		bha_nga;
	    නම් ((what & STR2NR_QUOTE) && *ptr == '\'' && vim_isxdigit(ptr[1]))
	    {
		++ptr;
		නම් (n++ == maxlen)
		    bha_nga;
	    }
	}
    }
    නැතහොත්
    {
	// decimal
	අතරතුර (VIM_ISDIGIT(*ptr))
	{
	    uvarnumber_T    digit = (uvarnumber_T)(*ptr - '0');

	    // avoid ubsan error for overflow
	    නම් (un < UVARNUM_MAX / 10
		    || (un == UVARNUM_MAX / 10 && digit <= UVARNUM_MAX % 10))
		un = 10 * un + digit;
	    නැතහොත්
	    {
		un = UVARNUM_MAX;
		නම් (overflow != NULL)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    නම් (n++ == maxlen)
		bha_nga;
	    නම් ((what & STR2NR_QUOTE) && *ptr == '\'' && VIM_ISDIGIT(ptr[1]))
	    {
		++ptr;
		නම් (n++ == maxlen)
		    bha_nga;
	    }
	}
    }

    // Check for an alphanumeric character immediately following, that is
    // most likely a typo.
    නම් (strict && n - 1 != maxlen && ASCII_ISALNUM(*ptr))
	ආපසු;

    නම් (prep != NULL)
	*prep = pre;
    නම් (len != NULL)
	*len = (පූර්ණ_සංඛ්‍යා)(ptr - start);
    නම් (nptr != NULL)
    {
	නම් (negative)   // account for leading '-' for decimal numbers
	{
	    // avoid ubsan error for overflow
	    නම් (un > VARNUM_MAX)
	    {
		*nptr = VARNUM_MIN;
		නම් (overflow != NULL)
		    *overflow = TRUE;
	    }
	    නැතහොත්
		*nptr = -(varnumber_T)un;
	}
	නැතහොත්
	{
	    // prevent a large unsigned number to become negative
	    නම් (un > VARNUM_MAX)
	    {
		un = VARNUM_MAX;
		නම් (overflow != NULL)
		    *overflow = TRUE;
	    }
	    *nptr = (varnumber_T)un;
	}
    }
    නම් (unptr != NULL)
	*unptr = un;
}

/*
 * Return the value of a single hex character.
 * Only valid when the argument is '0' - '9', 'A' - 'F' or 'a' - 'f'.
 */
    පූර්ණ_සංඛ්‍යා
hex2nr(පූර්ණ_සංඛ්‍යා c)
{
    නම් (c >= 'a' && c <= 'f')
	ආපසු c - 'a' + 10;
    නම් (c >= 'A' && c <= 'F')
	ආපසු c - 'A' + 10;
    ආපසු c - '0';
}

/*
 * Convert two hex characters to a byte.
 * Return -1 if one of the characters is not hex.
 */
    පූර්ණ_සංඛ්‍යා
hexhex2nr(char_u *p)
{
    නම් (!vim_isxdigit(p[0]) || !vim_isxdigit(p[1]))
	ආපසු -1;
    ආපසු (hex2nr(p[0]) << 4) + hex2nr(p[1]);
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
    පූර්ණ_සංඛ්‍යා
rem_backslash(char_u *str)
{
#ifdef BACKSLASH_IN_FILENAME
    ආපසු (str[0] == '\\'
	    && str[1] < 0x80
	    && (str[1] == ' '
		|| (str[1] != NUL
		    && str[1] != '*'
		    && str[1] != '?'
		    && !vim_isfilec(str[1]))));
#නැතහොත්
    ආපසු (str[0] == '\\' && str[1] != NUL);
#endif
}

/*
 * Halve the number of backslashes in a file name argument.
 * For MS-DOS we only do this if the character after the backslash
 * is not a normal file character.
 */
    void
backslash_halve(char_u *p)
{
    සඳහා ( ; *p; ++p)
	නම් (rem_backslash(p))
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
    නම් (res == NULL)
	ආපසු p;
    backslash_halve(res);
    ආපසු res;
}
