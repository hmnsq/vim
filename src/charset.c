/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"

#如果 defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#endif

static 整数 parse_isopt(char_u *var, buf_T *buf, 整数 only_check);
static 整数 win_nolbr_chartabsize(chartabsize_T *cts, 整数 *headp);
static unsigned nr2hex(unsigned c);

static 整数    chartab_initialized = FALSE;

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

static 整数 in_win_border(win_T *wp, colnr_T vcol);

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
    整数
init_chartab(void)
{
    返回 buf_init_chartab(curbuf, TRUE);
}

    整数
buf_init_chartab(
    buf_T	*buf,
    整数		global)		// FALSE: only set buf->b_chartab[]
{
    整数		c;
    char_u	*p;
    整数		i;

    如果 (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 */
	c = 0;
	当 (c < ' ')
	    g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	当 (c <= '~')
	    g_chartab[c++] = 1 + CT_PRINT_CHAR;
	当 (c < 256)
	{
	    // UTF-8: bytes 0xa0 - 0xff are printable (latin1)
	    如果 (enc_utf8 && c >= 0xa0)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // euc-jp characters starting with 0x8e are single width
	    否则 如果 (enc_dbcs == DBCS_JPNU && c == 0x8e)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // other double-byte chars can be printable AND double-width
	    否则 如果 (enc_dbcs != 0 && MB_BYTE2LEN(c) == 2)
		g_chartab[c++] = CT_PRINT_CHAR + 2;
	    否则
		// the rest is unprintable by default
		g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	}

	// Assume that every multi-byte char is a filename character.
	为 (c = 1; c < 256; ++c)
	    如果 ((enc_dbcs != 0 && MB_BYTE2LEN(c) > 1)
		    || (enc_dbcs == DBCS_JPNU && c == 0x8e)
		    || (enc_utf8 && c >= 0xa0))
		g_chartab[c] |= CT_FNAME_CHAR;
    }

    /*
     * Init word char flags all to FALSE
     */
    CLEAR_FIELD(buf->b_chartab);
    如果 (enc_dbcs != 0)
	为 (c = 0; c < 256; ++c)
	{
	    // double-byte characters are probably word characters
	    如果 (MB_BYTE2LEN(c) == 2)
		SET_CHARTAB(buf, c);
	}

    /*
     * In lisp mode the '-' character is included in keywords.
     */
    如果 (buf->b_p_lisp)
	SET_CHARTAB(buf, '-');

    // Walk through the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    为 (i = global ? 0 : 3; i <= 3; ++i)
    {
	如果 (i == 0)
	    p = p_isi;		// first round: 'isident'
	否则 如果 (i == 1)
	    p = p_isp;		// second round: 'isprint'
	否则 如果 (i == 2)
	    p = p_isf;		// third round: 'isfname'
	否则	// i == 3
	    p = buf->b_p_isk;	// fourth round: 'iskeyword'

	如果 (parse_isopt(p, buf, FALSE) == FAIL)
	    返回 FAIL;
    }

    chartab_initialized = TRUE;
    返回 OK;
}

/**
 * Checks the format for the option settings 'iskeyword', 'isident', 'isfname'
 * or 'isprint'.
 * Returns FAIL if has an error, OK otherwise.
 */
	整数
check_isopt(char_u *var)
{
    返回 parse_isopt(var, NULL, TRUE);
}

    static 整数
parse_isopt(
	char_u	*var,
	buf_T	*buf,
	整数	only_check)	// FALSE: refill g_chartab[]
{
    char_u	*p = var;
    整数		c;
    整数		c2;
    整数		tilde;
    整数		do_isalpha;
    整数		trail_comma;

    // Parses the 'isident', 'iskeyword', 'isfname' and 'isprint' options.
    // Each option is a list of characters, character numbers or ranges,
    // separated by commas, e.g.: "200-210,x,#-178,-"
    当 (*p)
    {
	tilde = FALSE;
	do_isalpha = FALSE;
	如果 (*p == '^' && p[1] != NUL)
	{
	    tilde = TRUE;
	    ++p;
	}
	如果 (VIM_ISDIGIT(*p))
	    c = getdigits(&p);
	否则 如果 (has_mbyte)
	    c = mb_ptr2char_adv(&p);
	否则
	    c = *p++;
	c2 = -1;
	如果 (*p == '-' && p[1] != NUL)
	{
	    ++p;
	    如果 (VIM_ISDIGIT(*p))
		c2 = getdigits(&p);
	    否则 如果 (has_mbyte)
		c2 = mb_ptr2char_adv(&p);
	    否则
		c2 = *p++;
	}
	如果 (c <= 0 || c >= 256 || (c2 < c && c2 != -1) || c2 >= 256
						  || !(*p == NUL || *p == ','))
	    返回 FAIL;

	trail_comma = *p == ',';
	p = skip_to_option_part(p);
	如果 (trail_comma && *p == NUL)
	    // Trailing comma is not allowed.
	    返回 FAIL;

	如果 (only_check)
	    continue;

	如果 (c2 == -1)	// not a range
	{
	    /*
	     * A single '@' (not "@-@"):
	     * Decide on letters being ID/printable/keyword chars with
	     * standard function isalpha(). This takes care of locale for
	     * single-byte characters).
	     */
	    如果 (c == '@')
	    {
		do_isalpha = TRUE;
		c = 1;
		c2 = 255;
	    }
	    否则
		c2 = c;
	}

	当 (c <= c2)
	{
	    // Use the MB_ functions here, because isalpha() doesn't
	    // work properly when 'encoding' is "latin1" and the locale is
	    // "C".
	    如果 (!do_isalpha || MB_ISLOWER(c) || MB_ISUPPER(c))
	    {
		如果 (var == p_isi)			// (re)set ID flag
		{
		    如果 (tilde)
			g_chartab[c] &= ~CT_ID_CHAR;
		    否则
			g_chartab[c] |= CT_ID_CHAR;
		}
		否则 如果 (var == p_isp)			// (re)set printable
		{
		    如果 ((c < ' ' || c > '~'
			    // For double-byte we keep the cell width, so
			    // that we can detect it from the first byte.
			) && !(enc_dbcs && MB_BYTE2LEN(c) == 2))
		    {
			如果 (tilde)
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_MASK)
					 + ((dy_flags & DY_UHEX) ? 4 : 2);
			    g_chartab[c] &= ~CT_PRINT_CHAR;
			}
			否则
			{
			    g_chartab[c] = (g_chartab[c] & ~CT_CELL_MASK) + 1;
			    g_chartab[c] |= CT_PRINT_CHAR;
			}
		    }
		}
		否则 如果 (var == p_isf)			// (re)set fname flag
		{
		    如果 (tilde)
			g_chartab[c] &= ~CT_FNAME_CHAR;
		    否则
			g_chartab[c] |= CT_FNAME_CHAR;
		}
		否则 // var == p_isk || var == buf->b_p_isk
							// (re)set keyword flag
		{
		    如果 (tilde)
			RESET_CHARTAB(buf, c);
		    否则
			SET_CHARTAB(buf, c);
		}
	    }
	    ++c;
	}
    }

    返回 OK;
}

/*
 * Translate any special characters in buf[bufsize] in-place.
 * The result is a string with only printable characters, but if there is not
 * enough room, not all characters will be translated.
 */
    void
trans_characters(
    char_u	*buf,
    整数		bufsize)
{
    整数		len;		// length of string needing translation
    整数		room;		// room in buffer after string
    char_u	*trs;		// translated character
    整数		trs_len;	// length of trs[]

    len = (整数)STRLEN(buf);
    room = bufsize - len;
    当 (*buf != 0)
    {
	// Assume a multi-byte character doesn't need translation.
	如果 (has_mbyte && (trs_len = (*mb_ptr2len)(buf)) > 1)
	    len -= trs_len;
	否则
	{
	    trs = transchar_byte(*buf);
	    trs_len = (整数)STRLEN(trs);
	    如果 (trs_len > 1)
	    {
		room -= trs_len - 1;
		如果 (room <= 0)
		    返回;
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
    整数		l, len, c;
    char_u	hexbuf[11];

    如果 (has_mbyte)
    {
	// Compute the length of the result, taking account of unprintable
	// multi-byte characters.
	len = 0;
	p = s;
	当 (*p != NUL)
	{
	    如果 ((l = (*mb_ptr2len)(p)) > 1)
	    {
		c = (*mb_ptr2char)(p);
		p += l;
		如果 (vim_isprintc(c))
		    len += l;
		否则
		{
		    transchar_hex(hexbuf, c);
		    len += (整数)STRLEN(hexbuf);
		}
	    }
	    否则
	    {
		l = byte2cells(*p++);
		如果 (l > 0)
		    len += l;
		否则
		    len += 4;	// illegal byte sequence
	    }
	}
	res = alloc(len + 1);
    }
    否则
	res = alloc(vim_strsize(s) + 1);

    如果 (res == NULL)
	返回 NULL;

    char_u *d = res;

    p = s;
    当 (*p != NUL)
    {
	如果 (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    c = (*mb_ptr2char)(p);
	    如果 (vim_isprintc(c))
	    {
		mch_memmove(d, p, (size_t)l);
		d += l;
	    }
	    否则
	    {
		transchar_hex(d, c);
		d += STRLEN(d);
	    }
	    p += l;
	}
	否则
	{
	    char_u	*trs = transchar_byte(*p++);
	    size_t	trs_len = STRLEN(trs);

	    mch_memmove(d, trs, trs_len);
	    d += trs_len;
	}
    }
    *d = NUL;
    返回 res;
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
    整数		orglen,
    char_u	*buf,
    整数		buflen)
{
    garray_T	ga;
    整数		i;
    整数		len = orglen;

#define GA_CHAR(i)  ((char_u *)ga.ga_data)[i]
#define GA_PTR(i)   ((char_u *)ga.ga_data + (i))
#define STR_CHAR(i)  (buf == NULL ? GA_CHAR(i) : buf[i])
#define STR_PTR(i)   (buf == NULL ? GA_PTR(i) : buf + (i))

    // Copy "str" into "buf" or allocated memory, unmodified.
    如果 (buf == NULL)
    {
	ga_init2(&ga, 1, 10);
	如果 (ga_grow(&ga, len + 1) == FAIL)
	    返回 NULL;
	mch_memmove(ga.ga_data, str, (size_t)len);
	ga.ga_len = len;
    }
    否则
    {
	如果 (len >= buflen)	    // Ugly!
	    len = buflen - 1;
	mch_memmove(buf, str, (size_t)len);
    }
    如果 (buf == NULL)
	GA_CHAR(len) = NUL;
    否则
	buf[len] = NUL;

    // Make each character lower case.
    i = 0;
    当 (STR_CHAR(i) != NUL)
    {
	如果 (enc_utf8 || (has_mbyte && MB_BYTE2LEN(STR_CHAR(i)) > 1))
	{
	    如果 (enc_utf8)
	    {
		整数	c = utf_ptr2char(STR_PTR(i));
		整数	olen = utf_ptr2len(STR_PTR(i));
		整数	lc = utf_tolower(c);

		// Only replace the character when it is not an invalid
		// sequence (ASCII character or more than one byte) and
		// utf_tolower() doesn't return the original character.
		如果 ((c < 0x80 || olen > 1) && c != lc)
		{
		    整数	    nlen = utf_char2len(lc);

		    // If the byte length changes need to shift the following
		    // characters forward or backward.
		    如果 (olen != nlen)
		    {
			如果 (nlen > olen)
			{
			    如果 (buf == NULL
				    ? ga_grow(&ga, nlen - olen + 1) == FAIL
				    : len + nlen - olen >= buflen)
			    {
				// out of memory, keep old char
				lc = c;
				nlen = olen;
			    }
			}
			如果 (olen != nlen)
			{
			    如果 (buf == NULL)
			    {
				STRMOVE(GA_PTR(i) + nlen, GA_PTR(i) + olen);
				ga.ga_len += nlen - olen;
			    }
			    否则
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
	否则
	{
	    如果 (buf == NULL)
		GA_CHAR(i) = TOLOWER_LOC(GA_CHAR(i));
	    否则
		buf[i] = TOLOWER_LOC(buf[i]);
	    ++i;
	}
    }

    如果 (buf == NULL)
	返回 (char_u *)ga.ga_data;
    返回 buf;
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
transchar(整数 c)
{
    返回 transchar_buf(curbuf, c);
}

    char_u *
transchar_buf(buf_T *buf, 整数 c)
{
    整数			i;

    i = 0;
    如果 (IS_SPECIAL(c))	    // special key code, display as ~@ char
    {
	transchar_charbuf[0] = '~';
	transchar_charbuf[1] = '@';
	i = 2;
	c = K_SECOND(c);
    }

    如果 ((!chartab_initialized && ((c >= ' ' && c <= '~')))
					|| (c < 256 && vim_isprintc_strict(c)))
    {
	// printable character
	transchar_charbuf[i] = c;
	transchar_charbuf[i + 1] = NUL;
    }
    否则
	transchar_nonprint(buf, transchar_charbuf + i, c);
    返回 transchar_charbuf;
}

/*
 * Like transchar(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of the current buffer.
 */
    char_u *
transchar_byte(整数 c)
{
    返回 transchar_byte_buf(curbuf, c);
}

/*
 * Like transchar_buf(), but called with a byte instead of a character.  Checks
 * for an illegal UTF-8 byte.  Uses 'fileformat' of "buf", unless it is NULL.
 */
    char_u *
transchar_byte_buf(buf_T *buf, 整数 c)
{
    如果 (enc_utf8 && c >= 0x80)
    {
	transchar_nonprint(buf, transchar_charbuf, c);
	返回 transchar_charbuf;
    }
    返回 transchar_buf(buf, c);
}
/*
 * Convert non-printable character to two or more printable characters in
 * "charbuf[]".  "charbuf" needs to be able to hold five bytes.
 * Does NOT work for multi-byte characters, c must be <= 255.
 */
    void
transchar_nonprint(buf_T *buf, char_u *charbuf, 整数 c)
{
    如果 (c == NL)
	c = NUL;		// we use newline in place of a NUL
    否则 如果 (buf != NULL && c == CAR && get_fileformat(buf) == EOL_MAC)
	c = NL;			// we use CR in place of  NL in this case

    如果 (dy_flags & DY_UHEX)		// 'display' has "uhex"
	transchar_hex(charbuf, c);

    否则 如果 (c <= 0x7f)			// 0x00 - 0x1f and 0x7f
    {
	charbuf[0] = '^';
	charbuf[1] = c ^ 0x40;		// DEL displayed as ^?
	charbuf[2] = NUL;
    }
    否则 如果 (enc_utf8)
    {
	transchar_hex(charbuf, c);
    }
    否则 如果 (c >= ' ' + 0x80 && c <= '~' + 0x80)    // 0xa0 - 0xfe
    {
	charbuf[0] = '|';
	charbuf[1] = c - 0x80;
	charbuf[2] = NUL;
    }
    否则					    // 0x80 - 0x9f and 0xff
    {
	charbuf[0] = '~';
	charbuf[1] = (c - 0x80) ^ 0x40;	// 0xff displayed as ~?
	charbuf[2] = NUL;
    }
}

    void
transchar_hex(char_u *buf, 整数 c)
{
    整数		i = 0;

    buf[0] = '<';
    如果 (c > 255)
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
    如果 ((c & 0xf) <= 9)
	返回 (c & 0xf) + '0';
    返回 (c & 0xf) - 10 + 'a';
}

/*
 * Return number of display cells occupied by byte "b".
 * Caller must make sure 0 <= b <= 255.
 * For multi-byte mode "b" must be the first byte of a character.
 * A TAB is counted as two cells: "^I".
 * For UTF-8 mode this will return 0 for bytes >= 0x80, because the number of
 * cells depends on further bytes.
 */
    整数
byte2cells(整数 b)
{
    如果 (enc_utf8 && b >= 0x80)
	返回 0;
    返回 (g_chartab[b] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character "c".
 * "c" can be a special key (negative number) in which case 3 or 4 is returned.
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    整数
char2cells(整数 c)
{
    如果 (IS_SPECIAL(c))
	返回 char2cells(K_SECOND(c)) + 2;
    如果 (c >= 0x80)
    {
	// UTF-8: above 0x80 need to check the value
	如果 (enc_utf8)
	    返回 utf_char2cells(c);
	// DBCS: double-byte means double-width, except for euc-jp with first
	// byte 0x8e
	如果 (enc_dbcs != 0 && c >= 0x100)
	{
	    如果 (enc_dbcs == DBCS_JPNU && ((unsigned)c >> 8) == 0x8e)
		返回 1;
	    返回 2;
	}
    }
    返回 (g_chartab[c & 0xff] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character at "*p".
 * A TAB is counted as two cells: "^I" or four: "<09>".
 */
    整数
ptr2cells(char_u *p)
{
    如果 (!has_mbyte)
	返回 byte2cells(*p);

    // For UTF-8 we need to look at more bytes if the first byte is >= 0x80.
    如果 (enc_utf8 && *p >= 0x80)
	返回 utf_ptr2cells(p);
    // For DBCS we can tell the cell count from the first byte.
    返回 (g_chartab[*p] & CT_CELL_MASK);
}

/*
 * Return the number of character cells string "s" will take on the screen,
 * counting TABs as two characters: "^I".
 */
    整数
vim_strsize(char_u *s)
{
    返回 vim_strnsize(s, (整数)MAXCOL);
}

/*
 * Return the number of character cells string "s[len]" will take on the
 * screen, counting TABs as two characters: "^I".
 */
    整数
vim_strnsize(char_u *s, 整数 len)
{
    整数		size = 0;

    当 (*s != NUL && --len >= 0)
    {
	整数	    l = (*mb_ptr2len)(s);

	size += ptr2cells(s);
	s += l;
	len -= l - 1;
    }

    返回 size;
}

/*
 * Return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab.
 * Use a define to make it fast, this is used very often!!!
 * Also see getvcol() below.
 */

#ifdef FEAT_VARTABS
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, col) \
    如果 (*(p) == TAB && (!(wp)->w_p_list || (wp)->w_lcs_chars.tab1)) \
    { \
	返回 tabstop_padding(col, (buf)->b_p_ts, (buf)->b_p_vts_array); \
    } \
    否则 \
	返回 ptr2cells(p);
#否则
# define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, col) \
    如果 (*(p) == TAB && (!(wp)->w_p_list || wp->w_lcs_chars.tab1)) \
    { \
	整数 ts; \
	ts = (buf)->b_p_ts; \
	返回 (整数)(ts - (col % ts)); \
    } \
    否则 \
	返回 ptr2cells(p);
#endif

    整数
chartabsize(char_u *p, colnr_T col)
{
    RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, p, col)
}

#如果 defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    整数
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
    整数
linetabsize_str(char_u *s)
{
    返回 linetabsize_col(0, s);
}

/*
 * Like linetabsize_str(), but "s" starts at column "startcol".
 */
    整数
linetabsize_col(整数 startcol, char_u *s)
{
    chartabsize_T cts;
    vimlong_T vcol;

    init_chartabsize_arg(&cts, curwin, 0, startcol, s, s);
    vcol = cts.cts_vcol;

    当 (*cts.cts_ptr != NUL)
    {
	vcol += lbr_chartabsize_adv(&cts);
	如果 (vcol > MAXCOL)
	{
	    cts.cts_vcol = MAXCOL;
	    break;
	}
	否则
	    cts.cts_vcol = (整数)vcol;
    }
    clear_chartabsize_arg(&cts);
    返回 (整数)cts.cts_vcol;
}

/*
 * Like linetabsize_str(), but for a given window instead of the current one.
 * Doesn't count the size of 'listchars' "eol".
 */
    整数
win_linetabsize(win_T *wp, linenr_T lnum, char_u *line, colnr_T len)
{
    chartabsize_T cts;

    init_chartabsize_arg(&cts, wp, lnum, 0, line, line);
    win_linetabsize_cts(&cts, len);
    clear_chartabsize_arg(&cts);
    返回 (整数)cts.cts_vcol;
}

/*
 * Return the number of cells line "lnum" of window "wp" will take on the
 * screen, taking into account the size of a tab and text properties.
 * Doesn't count the size of 'listchars' "eol".
 */
    整数
linetabsize(win_T *wp, linenr_T lnum)
{
    返回 win_linetabsize(wp, lnum,
		       ml_get_buf(wp->w_buffer, lnum, FALSE), (colnr_T)MAXCOL);
}

/*
 * Like linetabsize(), but counts the size of 'listchars' "eol".
 */
    整数
linetabsize_eol(win_T *wp, linenr_T lnum)
{
    返回 linetabsize(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Like linetabsize(), but excludes 'above'/'after'/'right'/'below' aligned
 * virtual text, while keeping inline virtual text.
 */
    整数
linetabsize_no_outer(win_T *wp, linenr_T lnum)
{
#ifndef FEAT_PROP_POPUP
    返回 linetabsize(wp, lnum);
#否则
    chartabsize_T cts;
    char_u *line = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, line, line);

    如果 (cts.cts_text_prop_count)
    {
	整数 write_idx = 0;
	为 (整数 read_idx = 0; read_idx < cts.cts_text_prop_count; read_idx++)
	{
	    textprop_T *tp = &cts.cts_text_props[read_idx];
	    如果 (tp->tp_col != MAXCOL)
	    {
		如果 (read_idx != write_idx)
		    cts.cts_text_props[write_idx] = *tp;
		write_idx++;
	    }
	}
	cts.cts_text_prop_count = write_idx;
	如果 (cts.cts_text_prop_count == 0)
	    VIM_CLEAR(cts.cts_text_props);
    }

    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    返回 (整数)cts.cts_vcol;
#endif
}

/*
 * Like linetabsize_no_outer(), but counts the size of 'listchars' "eol".
 */
    整数
linetabsize_no_outer_eol(win_T *wp, linenr_T lnum)
{
    返回 linetabsize_no_outer(wp, lnum)
	+ ((wp->w_p_list && wp->w_lcs_chars.eol != NUL) ? 1 : 0);
}

/*
 * Return TRUE when win_lbr_chartabsize() does nothing more than
 * win_nolbr_chartabsize(): no 'linebreak', 'breakindent', 'showbreak' and no
 * text properties that insert text.
 */
#如果 defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    static 整数
win_lbr_is_simple_width(chartabsize_T *cts)
{
    返回 TRUE
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
#如果 defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    // Fast path: when win_lbr_chartabsize() would just return
    // win_nolbr_chartabsize() (wrap on; no 'linebreak'/'breakindent'/
    // 'showbreak'/inserted text properties), call it directly.
    整数 simple_width = cts->cts_win->w_p_wrap && win_lbr_is_simple_width(cts);
#endif
#ifdef FEAT_PROP_POPUP
    cts->cts_with_trailing = len == MAXCOL;
#endif
    为 ( ; *cts->cts_ptr != NUL && (len == MAXCOL || cts->cts_ptr < cts->cts_line + len);
						      MB_PTR_ADV(cts->cts_ptr))
    {
#如果 defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
	如果 (simple_width)
	{
	    整数	c = *cts->cts_ptr;

	    如果 (c < 0x80 && c != TAB)
		vcol += g_chartab[c] & CT_CELL_MASK;
	    否则
		vcol += win_nolbr_chartabsize(cts, NULL);
	}
	否则
#endif
	    vcol += win_lbr_chartabsize(cts, NULL, NULL);
	如果 (vcol > MAXCOL)
	{
	    cts->cts_vcol = MAXCOL;
	    break;
	}
	否则
	    cts->cts_vcol = (整数)vcol;
    }
#ifdef FEAT_PROP_POPUP
    // check for a virtual text at the end of a line or on an empty line
    如果 (len == MAXCOL && cts->cts_has_prop_with_text && *cts->cts_ptr == NUL)
    {
	整数 head = 0;
	(void)win_lbr_chartabsize(cts, &head, NULL);
	vcol += cts->cts_cur_text_width + head;
	// When properties are above the empty line must also be counted.  For
	// a property below the width already includes filling up the line.
	如果 (cts->cts_ptr == cts->cts_line && cts->cts_prop_lines > 0
							 && !cts->cts_has_below)
	    ++vcol;
	cts->cts_vcol = vcol > MAXCOL ? MAXCOL : (整数)vcol;
    }
#endif
}

/*
 * Return TRUE if 'c' is a normal identifier character:
 * Letters and characters from the 'isident' option.
 */
    整数
vim_isIDc(整数 c)
{
    返回 (c > 0 && c < 0x100 && (g_chartab[c] & CT_ID_CHAR));
}

/*
 * Like vim_isIDc() but not using the 'isident' option: letters, numbers and
 * underscore.
 */
    整数
vim_isNormalIDc(整数 c)
{
    返回 ASCII_ISALNUM(c) || c == '_';
}

/*
 * return TRUE if 'c' is a keyword character: Letters and characters from
 * 'iskeyword' option for the current buffer.
 * For multi-byte characters mb_get_class() is used (builtin rules).
 */
    整数
vim_iswordc(整数 c)
{
    返回 vim_iswordc_buf(c, curbuf);
}

    整数
vim_iswordc_buf(整数 c, buf_T *buf)
{
    如果 (c >= 0x100)
    {
	如果 (enc_dbcs != 0)
	    返回 dbcs_class((unsigned)c >> 8, (unsigned)(c & 0xff)) >= 2;
	如果 (enc_utf8)
	    返回 utf_class_buf(c, buf) >= 2;
	返回 FALSE;
    }
    返回 (c > 0 && GET_CHARTAB(buf, c) != 0);
}

/*
 * Just like vim_iswordc() but uses a pointer to the (multi-byte) character.
 */
    整数
vim_iswordp(char_u *p)
{
    返回 vim_iswordp_buf(p, curbuf);
}

    整数
vim_iswordp_buf(char_u *p, buf_T *buf)
{
    整数	c = *p;

    如果 (has_mbyte && MB_BYTE2LEN(c) > 1)
	c = (*mb_ptr2char)(p);
    返回 vim_iswordc_buf(c, buf);
}

/*
 * Return TRUE if 'c' is a valid file-name character as specified with the
 * 'isfname' option.
 * Assume characters above 0x100 are valid (multi-byte).
 * To be used for commands like "gf".
 */
    整数
vim_isfilec(整数 c)
{
    返回 (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_FNAME_CHAR)));
}

#如果 defined(FEAT_SPELL)
/*
 * Return TRUE if 'c' is a valid file-name character, including characters left
 * out of 'isfname' to make "gf" work, such as comma, space, '@', etc.
 */
    整数
vim_is_fname_char(整数 c)
{
    返回 vim_isfilec(c) || c == ',' || c == ' ' || c == '@';
}
#endif

/*
 * return TRUE if 'c' is a valid file-name character or a wildcard character
 * Assume characters above 0x100 are valid (multi-byte).
 * Explicitly interpret ']' as a wildcard character as mch_has_wildcard("]")
 * returns false.
 */
    整数
vim_isfilec_or_wc(整数 c)
{
    char_u buf[2];

    buf[0] = (char_u)c;
    buf[1] = NUL;
    返回 vim_isfilec(c) || c == ']' || mch_has_wildcard(buf);
}

/*
 * Return TRUE if 'c' is a printable character.
 * Assume characters above 0x100 are printable (multi-byte), except for
 * Unicode.
 */
    整数
vim_isprintc(整数 c)
{
    如果 (enc_utf8 && c >= 0x100)
	返回 utf_printable(c);
    返回 (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
}

/*
 * Strict version of vim_isprintc(c), don't return TRUE if "c" is the head
 * byte of a double-byte character.
 */
    整数
vim_isprintc_strict(整数 c)
{
    如果 (enc_dbcs != 0 && c < 0x100 && MB_BYTE2LEN(c) > 1)
	返回 FALSE;
    如果 (enc_utf8 && c >= 0x100)
	返回 utf_printable(c);
    返回 (c >= 0x100 || (c > 0 && (g_chartab[c] & CT_PRINT_CHAR)));
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
    如果 (lnum > 0 && !ignore_text_props)
    {
	char_u	*prop_start;
	整数	count;

	count = get_text_props(wp->w_buffer, lnum, &prop_start, FALSE);
	cts->cts_text_prop_count = count;
	如果 (count > 0)
	{
	    // Make a copy of the properties, so that they are properly
	    // aligned.  Make it twice as long for the sorting below.
	    cts->cts_text_props = ALLOC_MULT(textprop_T, count * 2);
	    如果 (cts->cts_text_props == NULL)
		cts->cts_text_prop_count = 0;
	    否则
	    {
		整数	i;

		mch_memmove(cts->cts_text_props + count, prop_start,
						   count * sizeof(textprop_T));
		为 (i = 0; i < count; ++i)
		{
		    textprop_T *tp = cts->cts_text_props + i + count;
		    如果 (tp->tp_id < 0
				     && text_prop_type_valid(wp->w_buffer, tp))
		    {
			cts->cts_has_prop_with_text = TRUE;
			break;
		    }
		}
		如果 (!cts->cts_has_prop_with_text)
		{
		    // won't use the text properties, free them
		    VIM_CLEAR(cts->cts_text_props);
		    cts->cts_text_prop_count = 0;
		}
		否则
		{
		    整数	    *text_prop_idxs;

		    // Need to sort the array to get any truncation right.
		    // Do the sorting in the second part of the array, then
		    // move the sorted props to the first part of the array.
		    text_prop_idxs = ALLOC_MULT(整数, count);
		    如果 (text_prop_idxs != NULL)
		    {
			为 (i = 0; i < count; ++i)
			    text_prop_idxs[i] = i + count;
			sort_text_props(curbuf, cts->cts_text_props,
							text_prop_idxs, count);
			// Here we want the reverse order.
			为 (i = 0; i < count; ++i)
			    cts->cts_text_props[count - i - 1] =
					cts->cts_text_props[text_prop_idxs[i]];
			vim_free(text_prop_idxs);
		    }

		    // Convert tp_text_offset to tp_text pointer.
		    char_u *count_ptr = prop_start - PROP_COUNT_SIZE;

		    为 (i = 0; i < count; ++i)
		    {
			textprop_T *tp = &cts->cts_text_props[i];

			如果 (tp->tp_id < 0 && tp->u.tp_text_offset > 0)
			{
			    tp->u.tp_text = count_ptr + tp->u.tp_text_offset;
			    tp->tp_flags |= TP_FLAG_VTEXT_PTR;
			}
			否则
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
    如果 (cts->cts_text_prop_count > 0)
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
    整数
lbr_chartabsize(chartabsize_T *cts)
{
#如果 defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    如果 (1
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
	如果 (curwin->w_p_wrap)
	    返回 win_nolbr_chartabsize(cts, NULL);
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, cts->cts_ptr, cts->cts_vcol)
#如果 defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    }
    返回 win_lbr_chartabsize(cts, NULL, NULL);
#endif
}

/*
 * Call lbr_chartabsize() and advance the pointer.
 */
    整数
lbr_chartabsize_adv(chartabsize_T *cts)
{
    整数		retval;

    retval = lbr_chartabsize(cts);
    MB_PTR_ADV(cts->cts_ptr);
    返回 retval;
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
    整数
win_lbr_chartabsize(
	chartabsize_T	*cts,
	整数		*headp UNUSED,
	整数		*tailp UNUSED)
{
    win_T	*wp = cts->cts_win;
#如果 defined(FEAT_PROP_POPUP) || defined(FEAT_LINEBREAK)
    char_u	*line = cts->cts_line; // start of the line
    整数		size;
#endif
    char_u	*s = cts->cts_ptr;
    colnr_T	vcol = cts->cts_vcol;
#ifdef FEAT_LINEBREAK
    整数		mb_added = 0;
    整数		n;
    char_u	*sbr;
    整数		no_sbr = FALSE;
#endif

#如果 defined(FEAT_PROP_POPUP)
    cts->cts_cur_text_width = 0;
    cts->cts_has_below = false;
    cts->cts_first_char = 0;
#endif

#如果 defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    /*
     * No 'linebreak', 'showbreak', 'breakindent' and text properties that
     * insert text: return quickly.
     */
    如果 (win_lbr_is_simple_width(cts))
#endif
    {
	如果 (wp->w_p_wrap)
	    返回 win_nolbr_chartabsize(cts, headp);
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_buffer, s, vcol)
    }

#如果 defined(FEAT_LINEBREAK) || defined(FEAT_PROP_POPUP)
    整数 has_lcs_eol = wp->w_p_list && wp->w_lcs_chars.eol != NUL;
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
    如果 (*s == NUL)
    {
	// 1 cell for EOL list char (if present), as opposed to the two cell ^@
	// for a NUL character in the text.
	size = has_lcs_eol ? 1 : 0;
    }

    整数 is_doublewidth = has_mbyte && size == 2 && MB_BYTE2LEN(*s) > 1;
# endif

# ifdef FEAT_PROP_POPUP
    如果 (cts->cts_has_prop_with_text)
    {
#  ifdef FEAT_LINEBREAK
	整数	    tab_size = size;
#  endif
	整数	    charlen = *s == NUL ? 1 : mb_ptr2len(s);
	整数	    i;
	整数	    col = (整数)(s - line);


	// The "$" for 'list' mode will go between the EOL and
	// the text prop, account for that.
	如果 (has_lcs_eol)
	{
	    ++vcol;
	    --size;
	}

	为 (i = 0; i < cts->cts_text_prop_count; ++i)
	{
	    textprop_T	*tp = cts->cts_text_props + i;
	    整数		col_off = win_col_off(wp);

	    // Watch out for the text being deleted.  "cts_text_props" is a
	    // copy, the text prop may actually have been removed from the line.
	    如果 (tp->tp_id < 0
		    && ((tp->tp_col - 1 >= col
					     && tp->tp_col - 1 < col + charlen)
		       || (tp->tp_col == MAXCOL
			   && ((tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
				? col == 0
				: s[0] == NUL && cts->cts_with_trailing)))
		    && tp->u.tp_text != NULL)
	    {
		char_u *p = tp->u.tp_text;

		如果 (p != NULL)
		{
		    整数	cells;

		    如果 (tp->tp_col == MAXCOL)
		    {
			整数 n_extra = (整数)STRLEN(p);

			cells = text_prop_position(wp, tp, vcol,
			     (vcol + size) % (wp->w_width - col_off) + col_off,
					      &n_extra, &p, NULL, NULL, FALSE);
#  ifdef FEAT_LINEBREAK
			如果 (text_prop_no_showbreak(tp))
			    no_sbr = TRUE;  // don't use 'showbreak' now
#  endif
		    }
		    否则
			cells = vim_strsize(p);
		    如果 (tp->tp_flags & TP_FLAG_ALIGN_ABOVE)
		    {
			cts->cts_first_char += cells;
			如果 (!cts->cts_no_above)
			    cts->cts_cur_text_width += cells;
		    }
		    否则
		    {
			cts->cts_cur_text_width += cells;
			size += cells;
		    }
		    cts->cts_start_incl = tp->tp_flags & TP_FLAG_START_INCL;
#  ifdef FEAT_LINEBREAK
		    如果 (*s == TAB)
		    {
			// tab size changes because of the inserted text
			size -= tab_size;
			tab_size = win_chartabsize(wp, s,
					vcol + size - cts->cts_above_width);
			size += tab_size;
		    }
#  endif
		    如果 (tp->tp_col == MAXCOL && (tp->tp_flags
				& (TP_FLAG_ALIGN_ABOVE | TP_FLAG_ALIGN_BELOW)))
		    {
			// count extra line for property above/below
			++cts->cts_prop_lines;
			如果 (tp->tp_flags & TP_FLAG_ALIGN_BELOW)
			    cts->cts_has_below = true;
		    }
		}
	    }
	    如果 (tp->tp_col != MAXCOL && tp->tp_col - 1 > col)
		break;
	}
	如果 (has_lcs_eol)
	{
	    --vcol;
	    ++size;
	}
    }
# endif

# ifdef FEAT_LINEBREAK
    如果 (is_doublewidth && wp->w_p_wrap && in_win_border(wp, vcol + size - 2))
    {
	++size;		// Count the ">" in the last column.
	mb_added = 1;
    }

    /*
     * May have to add something for 'breakindent' and/or 'showbreak'
     * string at the start of a screen line.
     */
    整数 head = mb_added;
    sbr = no_sbr ? empty_option : get_showbreak_value(wp);
    // When "size" is 0, no new screen line is started.
    如果 (size > 0 && wp->w_p_wrap && (*sbr != NUL || wp->w_p_bri))
    {
	整数	col_off_prev = win_col_off(wp);
	整数	width2 = wp->w_width - col_off_prev + win_col_off2(wp);
	colnr_T	wcol = vcol + col_off_prev;
#  ifdef FEAT_PROP_POPUP
	wcol -= wp->w_virtcol_first_char;
#  endif
	colnr_T	max_head_vcol = cts->cts_max_head_vcol;
	整数	added = 0;

	// cells taken by 'showbreak'/'breakindent' before current char
	整数	head_prev = 0;
	如果 (wcol >= wp->w_width)
	{
	    wcol -= wp->w_width;
	    col_off_prev = wp->w_width - width2;
	    如果 (wcol >= width2 && width2 > 0)
		wcol %= width2;
	    如果 (*sbr != NUL)
		head_prev += vim_strsize(sbr);
	    如果 (wp->w_p_bri)
	    {
		如果 (cts->cts_bri_size < 0)
		    cts->cts_bri_size = get_breakindent_win(wp, line);
		head_prev += cts->cts_bri_size;
	    }
	    如果 (wcol < head_prev)
	    {
		head_prev -= wcol;
		wcol += head_prev;
		added += head_prev;
		如果 (max_head_vcol <= 0 || vcol < max_head_vcol)
		    head += head_prev;
	    }
	    否则
		head_prev = 0;
	    wcol += col_off_prev;
	}

	如果 (wcol + size > wp->w_width)
	{
	    // cells taken by 'showbreak'/'breakindent' halfway current char
	    整数	head_mid = 0;
	    如果 (*sbr != NUL)
		head_mid += vim_strsize(sbr);
	    如果 (wp->w_p_bri)
	    {
		如果 (cts->cts_bri_size < 0)
		    cts->cts_bri_size = get_breakindent_win(wp, line);
		head_mid += cts->cts_bri_size;
	    }
	    如果 (head_mid > 0)
	    {
		// Calculate effective window width.
		整数 prev_rem = wp->w_width - wcol;
		整数 width = width2 - head_mid;

		如果 (width <= 0)
		    width = 1;
		// Divide "size - prev_rem" by "width", rounding up.
		整数 cnt = (size - prev_rem + width - 1) / width;
		added += cnt * head_mid;

		如果 (max_head_vcol == 0 || vcol + size + added < max_head_vcol)
		    head += cnt * head_mid;
		否则 如果 (width2 > 0
			   && max_head_vcol > vcol + head_prev + prev_rem)
		    head += (max_head_vcol - (vcol + head_prev + prev_rem)
					     + width2 - 1) / width2 * head_mid;
		否则 如果 (max_head_vcol < 0)
		{
		    整数 off = mb_added;
#  ifdef FEAT_PROP_POPUP
		    如果 (*s != NUL
			     && ((State & MODE_NORMAL) || cts->cts_start_incl))
			off += cts->cts_cur_text_width;
#  endif
		    如果 (off >= prev_rem)
			head += (1 + (off - prev_rem) / width) * head_mid;
		}
	    }
	}

	size += added;
    }

    如果 (headp != NULL)
	*headp = head;

    整数 size_before_lbr = size;
    整数 need_lbr = FALSE;
    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here.
     */
    如果 (wp->w_p_lbr && wp->w_p_wrap && wp->w_width != 0
	    && VIM_ISBREAK((整数)s[0]) && !VIM_ISBREAK((整数)s[1]))
    {
	char_u	*t = cts->cts_line;
	当 (VIM_ISBREAK((整数)t[0]))
	    t++;
	// 'linebreak' is only needed when not in leading whitespace.
	need_lbr = s >= t;
    }
    如果 (need_lbr)
    {
	/*
	 * Count all characters from first non-blank after a blank up to next
	 * non-blank after a blank.
	 */
	整数 numberextra = win_col_off(wp);
	colnr_T col_adj = size - 1;
	colnr_T colmax = (colnr_T)(wp->w_width - numberextra - col_adj);
	如果 (vcol >= colmax)
	{
	    colmax += col_adj;
	    n = colmax +  win_col_off2(wp);
	    如果 (n > 0)
		colmax += (((vcol + col_adj - colmax) / n) + 1) * n - col_adj;
	}

	colnr_T vcol2 = vcol;
	为 (;;)
	{
	    char_u *ps = s;
	    MB_PTR_ADV(s);
	    整数 c = *s;
	    如果 (!(c != NUL
		    && (VIM_ISBREAK(c)
			|| (!VIM_ISBREAK(c)
			       && (vcol2 == vcol || !VIM_ISBREAK((整数)*ps))))))
		break;

	    vcol2 += win_chartabsize(wp, s, vcol2);
	    如果 (vcol2 >= colmax)		// doesn't fit
	    {
		size = colmax - vcol + col_adj;
		break;
	    }
	}
    }

    如果 (tailp != NULL)
	*tailp = size - size_before_lbr;

#  ifdef FEAT_PROP_POPUP
    如果 (cts->cts_first_char > 0 && !cts->cts_no_above)
    {
	// Remember the width for the size of a Tab later in the line.  Use
	// assignment, this may be called more than once for a character.
	cts->cts_above_width = cts->cts_first_char;
	size += cts->cts_first_char;
    }
#  endif
# endif
    返回 size;
#endif
}

/*
 * Like win_lbr_chartabsize(), except that we know 'linebreak' is off, 'wrap'
 * is on and there are no properties that insert text.  This means we need to
 * check for a double-byte character that doesn't fit at the end of the screen
 * line.
 * Only uses "cts_win", "cts_ptr" and "cts_vcol" from "cts".
 */
    static 整数
win_nolbr_chartabsize(
	chartabsize_T	*cts,
	整数		*headp)
{
    win_T	*wp = cts->cts_win;
    char_u	*s = cts->cts_ptr;
    colnr_T	col = cts->cts_vcol;
    整数		n;

    如果 (*s == TAB && (!wp->w_p_list || wp->w_lcs_chars.tab1))
    {
#ifdef FEAT_VARTABS
	返回 tabstop_padding(col, wp->w_buffer->b_p_ts,
				    wp->w_buffer->b_p_vts_array);
#否则
	n = wp->w_buffer->b_p_ts;
	返回 (整数)(n - (col % n));
#endif
    }
    n = ptr2cells(s);
    // Add one cell for a double-width character in the last column of the
    // window, displayed with a ">".
    如果 (n == 2 && MB_BYTE2LEN(*s) > 1 && in_win_border(wp, col))
    {
	如果 (headp != NULL)
	    *headp = 1;
	返回 3;
    }
    返回 n;
}

/*
 * Return TRUE if virtual column "vcol" is in the rightmost column of window
 * "wp".
 */
    static 整数
in_win_border(win_T *wp, colnr_T vcol)
{
    整数		width1;		// width of first line (after line number)
    整数		width2;		// width of further lines

    如果 (wp->w_width == 0)	// there is no border
	返回 FALSE;
    width1 = wp->w_width - win_col_off(wp);
    如果 ((整数)vcol < width1 - 1)
	返回 FALSE;
    如果 ((整数)vcol == width1 - 1)
	返回 TRUE;
    width2 = width1 + win_col_off2(wp);
    如果 (width2 <= 0)
	返回 FALSE;
    返回 ((vcol - width1) % width2 == width2 - 1);
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
    整数		flags)
{
    colnr_T	vcol;
    char_u	*ptr;		// points to current char
    char_u	*line;		// start of the line
    整数		incr;
    整数		head;
    整数		tail;
#ifdef FEAT_VARTABS
    整数		*vts = wp->w_buffer->b_p_vts_array;
#endif
    整数		ts = wp->w_buffer->b_p_ts;
    整数		c;
    chartabsize_T cts;
#ifdef FEAT_PROP_POPUP
    整数		on_NUL = FALSE;
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
    如果 ((!wp->w_p_list || wp->w_lcs_chars.tab1 != NUL)
#ifdef FEAT_LINEBREAK
	    && !wp->w_p_lbr && *get_showbreak_value(wp) == NUL && !wp->w_p_bri
#endif
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
       )
    {
	为 (;;)
	{
	    head = 0;
	    c = *ptr;
	    // make sure we don't go past the end of the line
	    如果 (c == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
		break;
	    }
	    // A tab gets expanded, depending on the current column
	    如果 (c == TAB)
#ifdef FEAT_VARTABS
		incr = tabstop_padding(vcol, ts, vts);
#否则
		incr = ts - (vcol % ts);
#endif
	    否则
	    {
		如果 (has_mbyte)
		{
		    // For utf-8, if the byte is >= 0x80, need to look at
		    // further bytes to find the cell width.
		    如果 (enc_utf8 && c >= 0x80)
			incr = utf_ptr2cells(ptr);
		    否则
			incr = g_chartab[c] & CT_CELL_MASK;

		    // If a double-cell char doesn't fit at the end of a line
		    // it wraps to the next line, it's like this char is three
		    // cells wide.
		    如果 (incr == 2 && wp->w_p_wrap && MB_BYTE2LEN(*ptr) > 1
			    && in_win_border(wp, vcol))
		    {
			++incr;
			head = 1;
		    }
		}
		否则
		    incr = g_chartab[c] & CT_CELL_MASK;
	    }

	    char_u *next_ptr = ptr + (*mb_ptr2len)(ptr);
	    如果 (next_ptr - line > pos->col) // character at pos->col
		break;

	    vcol += incr;
	    ptr = next_ptr;
	}

	tail = 0;
    }
    否则
    {
	为 (;;)
	{
	    // A tab gets expanded, depending on the current column.
	    // Other things also take up space.
	    head = 0;
	    tail = 0;
	    incr = win_lbr_chartabsize(&cts, &head, &tail);
#ifdef FEAT_PROP_POPUP
	    如果 (cts.cts_ptr == cts.cts_line)
	    {
		如果 (flags & GETVCOL_FOR_VIRTCOL)
		    // do not count the virtual text above for w_curswant
		    wp->w_virtcol_first_char = cts.cts_first_char;
		如果 ((flags & GETVCOL_NO_ABOVE) == 0)
		{
		    cts.cts_vcol += cts.cts_first_char;
		    cts.cts_above_width = cts.cts_first_char;
		}
	    }
#endif
	    // make sure we don't go past the end of the line
	    如果 (*cts.cts_ptr == NUL)
	    {
		incr = 1;	// NUL at end of line only takes one column
#ifdef FEAT_PROP_POPUP
		incr += cts.cts_cur_text_width;
		on_NUL = TRUE;
#endif
		break;
	    }

	    char_u *next_ptr = cts.cts_ptr + (*mb_ptr2len)(cts.cts_ptr);
	    如果 (next_ptr - line > pos->col) // character at pos->col
		break;

	    cts.cts_vcol += incr;
	    cts.cts_ptr = next_ptr;
	}
	vcol = cts.cts_vcol;
	ptr = cts.cts_ptr;
    }
    clear_chartabsize_arg(&cts);

    如果 (*ptr == NUL && pos->col < MAXCOL && pos->col > ptr - line)
	pos->col = ptr - line;

    如果 (start != NULL)
	*start = vcol + head;
    如果 (end != NULL)
	*end = vcol + incr - (flags & GETVCOL_END_EXCL_LBR ? tail : 0) - 1;
    如果 (cursor != NULL)
    {
	如果 (*ptr == TAB
		&& (State & MODE_NORMAL)
		&& !wp->w_p_list
		&& !virtual_active()
		&& !(VIsual_active
				&& (*p_sel == 'e' || LTOREQ_POS(*pos, VIsual)))
		)
	    // TODO: subtracting "tail" may lead to better cursor position
	    *cursor = vcol + incr - 1;	    // cursor at end
	否则
	{
#ifdef FEAT_PROP_POPUP
	    // in Insert mode, if "start_incl" is true the text gets inserted
	    // after the virtual text, thus add its width
	    如果 (((State & MODE_INSERT) == 0 || cts.cts_start_incl) && !on_NUL)
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
    整数		list_save = curwin->w_p_list;
    colnr_T	vcol;

    curwin->w_p_list = FALSE;
    如果 (posp->coladd)
	getvvcol(curwin, posp, NULL, &vcol, NULL, 0);
    否则
	getvcol(curwin, posp, NULL, &vcol, NULL, 0);
    curwin->w_p_list = list_save;
    返回 vcol;
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
    整数		flags)
{
    colnr_T	col;
    colnr_T	coladd;
    colnr_T	endadd;
    char_u	*ptr;

    如果 (virtual_active())
    {
	// For virtual mode, only want one value
	getvcol(wp, pos, &col, NULL, NULL, flags);

	coladd = pos->coladd;
	endadd = 0;
	// Cannot put the cursor on part of a wide character.
	ptr = ml_get_buf(wp->w_buffer, pos->lnum, FALSE);
	如果 (pos->col < ml_get_buf_len(wp->w_buffer, pos->lnum))
	{
	    整数 c = (*mb_ptr2char)(ptr + pos->col);

	    如果 (c != TAB && vim_isprintc(c))
	    {
		endadd = (colnr_T)(char2cells(c) - 1);
		如果 (coladd > endadd)	// past end of line
		    endadd = 0;
		否则
		    coladd = 0;
	    }
	}
	col += coladd;
	如果 (start != NULL)
	    *start = col;
	如果 (cursor != NULL)
	    *cursor = col;
	如果 (end != NULL)
	    *end = col + endadd;
    }
    否则
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
    整数		flags)
{
    colnr_T	from1, from2, to1, to2;

    如果 (LT_POSP(pos1, pos2))
    {
	getvvcol(wp, pos1, &from1, NULL, &to1, flags);
	getvvcol(wp, pos2, &from2, NULL, &to2, flags);
    }
    否则
    {
	getvvcol(wp, pos2, &from1, NULL, &to1, flags);
	getvvcol(wp, pos1, &from2, NULL, &to2, flags);
    }
    如果 (from2 < from1)
	*left = from2;
    否则
	*left = from1;
    如果 (to2 > to1)
    {
	如果 (*p_sel == 'e' && from2 - 1 >= to1)
	    *right = from2 - 1;
	否则
	    *right = to2;
    }
    否则
	*right = to1;
}

/*
 * Skip over ' ' and '\t'.
 */
    char_u *
skipwhite(char_u *q)
{
    char_u	*p = q;

    当 (VIM_ISWHITE(*p))
	++p;
    返回 p;
}

#如果 defined(FEAT_EVAL)
/*
 * skip over ' ', '\t' and '\n'.
 */
    char_u *
skipwhite_and_nl(char_u *q)
{
    char_u	*p = q;

    当 (VIM_ISWHITE(*p) || *p == NL)
	++p;
    返回 p;
}
#endif

/*
 * getwhitecols: return the number of whitespace
 * columns (bytes) at the start of a given line
 */
    整数
getwhitecols_curline(void)
{
    返回 getwhitecols(ml_get_curline());
}

    整数
getwhitecols(char_u *p)
{
    返回 skipwhite(p) - p;
}

/*
 * skip over digits
 */
    char_u *
skipdigits(char_u *q)
{
    char_u	*p = q;

    当 (VIM_ISDIGIT(*p))	// skip to next non-digit
	++p;
    返回 p;
}

#如果 defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * skip over binary digits
 */
    char_u *
skipbin(char_u *q)
{
    char_u	*p = q;

    当 (vim_isbdigit(*p))	// skip to next non-digit
	++p;
    返回 p;
}

/*
 * skip over digits and hex characters
 */
    char_u *
skiphex(char_u *q)
{
    char_u	*p = q;

    当 (vim_isxdigit(*p))	// skip to next non-digit
	++p;
    返回 p;
}
#endif

/*
 * skip to bin digit (or NUL after the string)
 */
    char_u *
skiptobin(char_u *q)
{
    char_u	*p = q;

    当 (*p != NUL && !vim_isbdigit(*p))	// skip to next digit
	++p;
    返回 p;
}

/*
 * skip to digit (or NUL after the string)
 */
    char_u *
skiptodigit(char_u *q)
{
    char_u	*p = q;

    当 (*p != NUL && !VIM_ISDIGIT(*p))	// skip to next digit
	++p;
    返回 p;
}

/*
 * skip to hex character (or NUL after the string)
 */
    char_u *
skiptohex(char_u *q)
{
    char_u	*p = q;

    当 (*p != NUL && !vim_isxdigit(*p))	// skip to next digit
	++p;
    返回 p;
}

/*
 * Variant of isdigit() that can handle characters > 0x100.
 * We don't use isdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 * Use the VIM_ISDIGIT() macro for simple arguments.
 */
    整数
vim_isdigit(整数 c)
{
    返回 (c >= '0' && c <= '9');
}

/*
 * Variant of isxdigit() that can handle characters > 0x100.
 * We don't use isxdigit() here, because on some systems it also considers
 * superscript 1 to be a digit.
 */
    整数
vim_isxdigit(整数 c)
{
    返回 (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'f')
	|| (c >= 'A' && c <= 'F');
}

/*
 * Corollary of vim_isdigit and vim_isxdigit() that can handle
 * characters > 0x100.
 */
    整数
vim_isbdigit(整数 c)
{
    返回 (c == '0' || c == '1');
}

    static 整数
vim_isodigit(整数 c)
{
    返回 (c >= '0' && c <= '7');
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

    整数
vim_islower(整数 c)
{
    如果 (c <= '@')
	返回 FALSE;
    如果 (c >= 0x80)
    {
	如果 (enc_utf8)
	    返回 utf_islower(c);
	如果 (c >= 0x100)
	{
#ifdef HAVE_ISWLOWER
	    如果 (has_mbyte)
		返回 iswlower(c);
#endif
	    // islower() can't handle these chars and may crash
	    返回 FALSE;
	}
	如果 (enc_latin1like)
	    返回 (latin1flags[c] & LATIN1LOWER) == LATIN1LOWER;
    }
    返回 SAFE_islower(c);
}

    整数
vim_isupper(整数 c)
{
    如果 (c <= '@')
	返回 FALSE;
    如果 (c >= 0x80)
    {
	如果 (enc_utf8)
	    返回 utf_isupper(c);
	如果 (c >= 0x100)
	{
#ifdef HAVE_ISWUPPER
	    如果 (has_mbyte)
		返回 iswupper(c);
#endif
	    // islower() can't handle these chars and may crash
	    返回 FALSE;
	}
	如果 (enc_latin1like)
	    返回 (latin1flags[c] & LATIN1UPPER) == LATIN1UPPER;
    }
    返回 SAFE_isupper(c);
}

    整数
vim_isalpha(整数 c)
{
    返回 vim_islower(c) || vim_isupper(c);
}

    整数
vim_toupper(整数 c)
{
    如果 (c <= '@')
	返回 c;
    如果 (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	如果 (enc_utf8)
	    返回 utf_toupper(c);
	如果 (c >= 0x100)
	{
#ifdef HAVE_TOWUPPER
	    如果 (has_mbyte)
		返回 towupper(c);
#endif
	    // toupper() can't handle these chars and may crash
	    返回 c;
	}
	如果 (enc_latin1like)
	    返回 latin1upper[c];
    }
    如果 (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	返回 TOUPPER_ASC(c);
    返回 TOUPPER_LOC(c);
}

    整数
vim_tolower(整数 c)
{
    如果 (c <= '@')
	返回 c;
    如果 (c >= 0x80 || !(cmp_flags & CMP_KEEPASCII))
    {
	如果 (enc_utf8)
	    返回 utf_tolower(c);
	如果 (c >= 0x100)
	{
#ifdef HAVE_TOWLOWER
	    如果 (has_mbyte)
		返回 towlower(c);
#endif
	    // tolower() can't handle these chars and may crash
	    返回 c;
	}
	如果 (enc_latin1like)
	    返回 latin1lower[c];
    }
    如果 (c < 0x80 && (cmp_flags & CMP_KEEPASCII))
	返回 TOLOWER_ASC(c);
    返回 TOLOWER_LOC(c);
}

/*
 * skiptowhite: skip over text until ' ' or '\t' or NUL.
 */
    char_u *
skiptowhite(char_u *p)
{
    当 (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    返回 p;
}

/*
 * skiptowhite_esc: Like skiptowhite(), but also skip escaped chars
 */
    char_u *
skiptowhite_esc(char_u *p)
{
    当 (*p != ' ' && *p != '\t' && *p != NUL)
    {
	如果 ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    返回 p;
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
    retval = strtol((char *)p, NULL, 10);
    如果 (*p == '-')		// skip negative sign
	++p;
    p = skipdigits(p);		// skip to next non-digit
    *pp = p;
    返回 retval;
}

/*
 * Like getdigits() but allow for embedded single quotes.
 */
    long
getdigits_quoted(char_u **pp)
{
    char_u	*p = *pp;
    long	retval = 0;

    如果 (*p == '-')
	++p;
    当 (VIM_ISDIGIT(*p))
    {
	如果 (retval >= LONG_MAX / 10 - 10)
	    retval = LONG_MAX;
	否则
	    retval = retval * 10 - '0' + *p;
	++p;
	如果 (in_vim9script() && *p == '\'' && VIM_ISDIGIT(p[1]))
	    ++p;
    }
    如果 (**pp == '-')
    {
	如果 (retval == LONG_MAX)
	    retval = LONG_MIN;
	否则
	    retval = -retval;
    }
    *pp = p;
    返回 retval;
}

/*
 * Return TRUE if "lbuf" is empty or only contains blanks.
 */
    整数
vim_isblankline(char_u *lbuf)
{
    char_u	*p;

    p = skipwhite(lbuf);
    返回 (*p == NUL || *p == '\r' || *p == '\n');
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
    整数			*prep,	    // return: type of number 0 = decimal, 'x'
				    // or 'X' is hex, '0', 'o' or 'O' is octal,
				    // 'b' or 'B' is bin
    整数			*len,	    // return: detected length of number
    整数			what,	    // what numbers to recognize
    varnumber_T		*nptr,	    // return: signed result
    uvarnumber_T	*unptr,	    // return: unsigned result
    整数			maxlen,     // max length of string to check
    整数			strict,     // check strictly
    整数			*overflow)  // when not NULL set to TRUE for overflow
{
    char_u	    *ptr = start;
    整数		    pre = 0;		// default is decimal
    整数		    negative = FALSE;
    uvarnumber_T    un = 0;
    整数		    n;

    如果 (len != NULL)
	*len = 0;

    如果 (ptr[0] == '-')
    {
	negative = TRUE;
	++ptr;
    }

    // Recognize hex, octal, and bin.
    如果 (ptr[0] == '0' && ptr[1] != '8' && ptr[1] != '9'
					       && (maxlen == 0 || maxlen > 1))
    {
	pre = ptr[1];
	如果 ((what & STR2NR_HEX)
		&& (pre == 'X' || pre == 'x') && vim_isxdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // hexadecimal
	    ptr += 2;
	否则 如果 ((what & STR2NR_BIN)
		&& (pre == 'B' || pre == 'b') && vim_isbdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // binary
	    ptr += 2;
	否则 如果 ((what & STR2NR_OOCT)
		&& (pre == 'O' || pre == 'o') && vim_isodigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // octal with prefix "0o"
	    ptr += 2;
	否则
	{
	    // decimal or octal, default is decimal
	    pre = 0;
	    如果 (what & STR2NR_OCT)
	    {
		// Don't interpret "0", "08" or "0129" as octal.
		为 (n = 1; n != maxlen && VIM_ISDIGIT(ptr[n]); ++n)
		{
		    如果 (ptr[n] > '7')
		    {
			pre = 0;	// can't be octal
			break;
		    }
		    pre = '0';	// assume octal
		}
	    }
	}
    }

    // Do the conversion manually to avoid sscanf() quirks.
    n = 1;
    如果 (pre == 'B' || pre == 'b'
			     || ((what & STR2NR_BIN) && (what & STR2NR_FORCE)))
    {
	// bin
	如果 (pre != 0)
	    n += 2;	    // skip over "0b"
	当 ('0' <= *ptr && *ptr <= '1')
	{
	    // avoid ubsan error for overflow
	    如果 (un <= UVARNUM_MAX / 2)
		un = 2 * un + (uvarnumber_T)(*ptr - '0');
	    否则
	    {
		un = UVARNUM_MAX;
		如果 (overflow != NULL)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    如果 (n++ == maxlen)
		break;
	    如果 ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '1')
	    {
		++ptr;
		如果 (n++ == maxlen)
		    break;
	    }
	}
    }
    否则 如果 (pre == 'O' || pre == 'o' ||
		pre == '0' || ((what & STR2NR_OCT) && (what & STR2NR_FORCE)))
    {
	// octal
	如果 (pre != 0 && pre != '0')
	    n += 2;	    // skip over "0o"
	当 ('0' <= *ptr && *ptr <= '7')
	{
	    // avoid ubsan error for overflow
	    如果 (un <= UVARNUM_MAX / 8)
		un = 8 * un + (uvarnumber_T)(*ptr - '0');
	    否则
	    {
		un = UVARNUM_MAX;
		如果 (overflow != NULL)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    如果 (n++ == maxlen)
		break;
	    如果 ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '7')
	    {
		++ptr;
		如果 (n++ == maxlen)
		    break;
	    }
	}
    }
    否则 如果 (pre != 0 || ((what & STR2NR_HEX) && (what & STR2NR_FORCE)))
    {
	// hex
	如果 (pre != 0)
	    n += 2;	    // skip over "0x"
	当 (vim_isxdigit(*ptr))
	{
	    // avoid ubsan error for overflow
	    如果 (un <= UVARNUM_MAX / 16)
		un = 16 * un + (uvarnumber_T)hex2nr(*ptr);
	    否则
	    {
		un = UVARNUM_MAX;
		如果 (overflow != NULL)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    如果 (n++ == maxlen)
		break;
	    如果 ((what & STR2NR_QUOTE) && *ptr == '\'' && vim_isxdigit(ptr[1]))
	    {
		++ptr;
		如果 (n++ == maxlen)
		    break;
	    }
	}
    }
    否则
    {
	// decimal
	当 (VIM_ISDIGIT(*ptr))
	{
	    uvarnumber_T    digit = (uvarnumber_T)(*ptr - '0');

	    // avoid ubsan error for overflow
	    如果 (un < UVARNUM_MAX / 10
		    || (un == UVARNUM_MAX / 10 && digit <= UVARNUM_MAX % 10))
		un = 10 * un + digit;
	    否则
	    {
		un = UVARNUM_MAX;
		如果 (overflow != NULL)
		    *overflow = TRUE;
	    }
	    ++ptr;
	    如果 (n++ == maxlen)
		break;
	    如果 ((what & STR2NR_QUOTE) && *ptr == '\'' && VIM_ISDIGIT(ptr[1]))
	    {
		++ptr;
		如果 (n++ == maxlen)
		    break;
	    }
	}
    }

    // Check for an alphanumeric character immediately following, that is
    // most likely a typo.
    如果 (strict && n - 1 != maxlen && ASCII_ISALNUM(*ptr))
	返回;

    如果 (prep != NULL)
	*prep = pre;
    如果 (len != NULL)
	*len = (整数)(ptr - start);
    如果 (nptr != NULL)
    {
	如果 (negative)   // account for leading '-' for decimal numbers
	{
	    // avoid ubsan error for overflow
	    如果 (un > VARNUM_MAX)
	    {
		*nptr = VARNUM_MIN;
		如果 (overflow != NULL)
		    *overflow = TRUE;
	    }
	    否则
		*nptr = -(varnumber_T)un;
	}
	否则
	{
	    // prevent a large unsigned number to become negative
	    如果 (un > VARNUM_MAX)
	    {
		un = VARNUM_MAX;
		如果 (overflow != NULL)
		    *overflow = TRUE;
	    }
	    *nptr = (varnumber_T)un;
	}
    }
    如果 (unptr != NULL)
	*unptr = un;
}

/*
 * Return the value of a single hex character.
 * Only valid when the argument is '0' - '9', 'A' - 'F' or 'a' - 'f'.
 */
    整数
hex2nr(整数 c)
{
    如果 (c >= 'a' && c <= 'f')
	返回 c - 'a' + 10;
    如果 (c >= 'A' && c <= 'F')
	返回 c - 'A' + 10;
    返回 c - '0';
}

/*
 * Convert two hex characters to a byte.
 * Return -1 if one of the characters is not hex.
 */
    整数
hexhex2nr(char_u *p)
{
    如果 (!vim_isxdigit(p[0]) || !vim_isxdigit(p[1]))
	返回 -1;
    返回 (hex2nr(p[0]) << 4) + hex2nr(p[1]);
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
    整数
rem_backslash(char_u *str)
{
#ifdef BACKSLASH_IN_FILENAME
    返回 (str[0] == '\\'
	    && str[1] < 0x80
	    && (str[1] == ' '
		|| (str[1] != NUL
		    && str[1] != '*'
		    && str[1] != '?'
		    && !vim_isfilec(str[1]))));
#否则
    返回 (str[0] == '\\' && str[1] != NUL);
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
    为 ( ; *p; ++p)
	如果 (rem_backslash(p))
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
    如果 (res == NULL)
	返回 p;
    backslash_halve(res);
    返回 res;
}
