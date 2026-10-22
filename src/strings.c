/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * strings.c: string manipulation functions
 */

#define USING_FLOAT_STUFF
#include "vim.h"

/*
 * Copy "string" into newly allocated memory.
 */
    char_u *
vim_strsave(char_u *string)
{
    char_u	*p;
    size_t	len;

    len = STRLEN(string) + 1;
    p = alloc(len);
    如果 (p != NULL)
	mch_memmove(p, string, len);
    返回 p;
}

/*
 * Copy up to "len" bytes of "string" into newly allocated memory and
 * terminate with a NUL.
 * The allocated memory always has size "len + 1", also when "string" is
 * shorter.
 */
    char_u *
vim_strnsave(char_u *string, size_t len)
{
    char_u	*p;

    p = alloc(len + 1);
    如果 (p == NULL)
	返回 NULL;

    STRNCPY(p, string, len);
    p[len] = NUL;
    返回 p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    返回 vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
}

/*
 * Same as vim_strsave_escaped(), but when "bsl" is TRUE also escape
 * characters where rem_backslash() would remove the backslash.
 * Escape the characters with "cc".
 */
    char_u *
vim_strsave_escaped_ext(
    char_u	*string,
    char_u	*esc_chars,
    整数		cc,
    整数		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    整数		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    为 (p = string; *p; p++)
    {
	如果 (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    continue;
	}
	如果 (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    如果 (escaped_string == NULL)
	返回 NULL;
    p2 = escaped_string;
    为 (p = string; *p; p++)
    {
	如果 (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    continue;
	}
	如果 (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    返回 escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    整数
csh_like_shell(void)
{
    返回 (strstr((char *)gettail(p_sh), "csh") != NULL);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    static 整数
fish_like_shell(void)
{
    返回 (strstr((char *)gettail(p_sh), "fish") != NULL);
}

/*
 * Escape "string" for use as a shell argument with system().
 * This uses single quotes, except when we know we need to use double quotes
 * (MS-DOS and MS-Windows not using PowerShell and without 'shellslash' set).
 * PowerShell also uses a novel escaping for enclosed single quotes - double
 * them up.
 * Escape a newline, depending on the 'shell' option.
 * When "do_special" is TRUE also replace "!", "%", "#" and things starting
 * with "<" like "<cfile>".
 * When "do_newline" is FALSE do not escape newline unless it is csh shell.
 * Returns the result in allocated memory, NULL if we have run out.
 */
    char_u *
vim_strsave_shellescape(char_u *string, 整数 do_special, 整数 do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    整数		csh_like;
    整数		fish_like;
    char_u	*shname;
    整数		powershell;
#ifdef MSWIN
    整数		double_quotes;
#endif

    // Only csh and similar shells expand '!' within single quotes.  For sh and
    // the like we must not put a backslash before it, it will be taken
    // literally.  If do_special is set the '!' will be escaped twice.
    // Csh also needs to have "\n" escaped twice when do_special is set.
    csh_like = csh_like_shell();

    // Fish shell uses '\' as an escape character within single quotes, so '\'
    // itself must be escaped to get a literal '\'.
    fish_like = fish_like_shell();

    // PowerShell uses its own version for quoting single quotes
    shname = gettail(p_sh);
    powershell = strstr((char *)shname, "pwsh") != NULL;
#ifdef MSWIN
    powershell = powershell || strstr((char *)shname, "powershell") != NULL;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (unsigned)STRLEN(string) + 3;  // two quotes and a trailing NUL
    为 (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	如果 (double_quotes)
	{
	    如果 (*p == '"')
		++length;		// " -> ""
	}
	否则
#endif
	如果 (*p == '\'')
	{
	    如果 (powershell)
		length += 2;		// ' => ''
	    否则
		length += 3;		// ' => '\''
	}
	如果 ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    如果 (csh_like && do_special)
		++length;		// insert backslash
	}
	如果 (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	如果 (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    如果 (escaped_string != NULL)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	如果 (double_quotes)
	    *d++ = '"';
	否则
#endif
	    *d++ = '\'';

	为 (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    如果 (double_quotes)
	    {
		如果 (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    continue;
		}
	    }
	    否则
#endif
	    如果 (*p == '\'')
	    {
		如果 (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		否则
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		continue;
	    }
	    如果 ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		如果 (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		continue;
	    }
	    如果 (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		continue;
	    }
	    如果 (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		continue;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	如果 (double_quotes)
	    *d++ = '"';
	否则
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    返回 escaped_string;
}

/*
 * Like vim_strsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strsave_up(char_u *string)
{
    char_u *p1;

    p1 = vim_strsave(string);
    vim_strup(p1);
    返回 p1;
}

/*
 * Like vim_strnsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strnsave_up(char_u *string, size_t len)
{
    char_u *p1;

    p1 = vim_strnsave(string, len);
    vim_strup(p1);
    返回 p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    void
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    整数	    c;

    如果 (p == NULL)
	返回;

    p2 = p;
    当 ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#如果 defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    static char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    如果 (res != NULL)
	当 (*p != NUL)
	{
	    整数		l;

	    如果 (enc_utf8)
	    {
		整数	c, uc;
		整数	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		如果 (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		如果 (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    如果 (s == NULL)
		    {
			vim_free(res);
			返回 NULL;
		    }
		    mch_memmove(s, res, p - res);
		    STRCPY(s + (p - res) + newl, p + l);
		    p = s + (p - res);
		    vim_free(res);
		    res = s;
		}

		utf_char2bytes(uc, p);
		p += newl;
	    }
	    否则 如果 (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    否则
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    返回 res;
}

/*
 * Make string "s" all lower-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    char_u *
strlow_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    如果 (res != NULL)
	当 (*p != NUL)
	{
	    整数		l;

	    如果 (enc_utf8)
	    {
		整数	c, lc;
		整数	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		如果 (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		如果 (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    如果 (s == NULL)
		    {
			vim_free(res);
			返回 NULL;
		    }
		    mch_memmove(s, res, p - res);
		    STRCPY(s + (p - res) + newl, p + l);
		    p = s + (p - res);
		    vim_free(res);
		    res = s;
		}

		utf_char2bytes(lc, p);
		p += newl;
	    }
	    否则 如果 (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    否则
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    返回 res;
}
#endif

/*
 * delete spaces at the end of a string
 */
    void
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    当 (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    void
vim_strncpy(char_u *to, char_u *from, size_t len)
{
    STRNCPY(to, from, len);
    to[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    void
vim_strcat(char_u *to, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(to);
    size_t fromlen = STRLEN(from);

    如果 (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    否则
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(char *s, size_t maxlen)
{
    size_t i;
    为 (i = 0; i < maxlen; ++i)
	如果 (s[i] == NUL)
	    break;
    返回 i;
}

#如果 !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    整数
vim_stricmp(char *s1, char *s2)
{
    整数		i;

    为 (;;)
    {
	i = (整数)TOLOWER_LOC(*s1) - (整数)TOLOWER_LOC(*s2);
	如果 (i != 0)
	    返回 i;			    // this character different
	如果 (*s1 == NUL)
	    break;			    // strings match until NUL
	++s1;
	++s2;
    }
    返回 0;				    // strings match
}
#endif

#如果 !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    整数
vim_strnicmp(char *s1, char *s2, size_t len)
{
    整数		i;

    当 (len > 0)
    {
	i = (整数)TOLOWER_LOC(*s1) - (整数)TOLOWER_LOC(*s2);
	如果 (i != 0)
	    返回 i;			    // this character different
	如果 (*s1 == NUL)
	    break;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    返回 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    整数
vim_strnicmp_asc(char *s1, char *s2, size_t len)
{
    整数                i = 0;

    当 (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	如果 (i != 0)
	    break;			// this character is different
	如果 (*s1 == NUL)
	    break;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    返回 i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, 整数 c)
{
    char_u	*p;
    整数		b;

    p = string;
    如果 (enc_utf8 && c > 0 && c < 0x80)
	返回 vim_strbyte(string, c);
    如果 (enc_utf8 && c >= 0x80)
    {
	当 (*p != NUL)
	{
	    整数 l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    如果 (utf_ptr2char(p) == c && l > 1)
		返回 p;
	    p += l;
	}
	返回 NULL;
    }
    如果 (enc_dbcs != 0 && c > 255)
    {
	整数	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	当 ((b = *p) != NUL)
	{
	    如果 (b == c && p[1] == n2)
		返回 p;
	    p += (*mb_ptr2len)(p);
	}
	返回 NULL;
    }
    如果 (has_mbyte)
    {
	当 ((b = *p) != NUL)
	{
	    如果 (b == c)
		返回 p;
	    p += (*mb_ptr2len)(p);
	}
	返回 NULL;
    }
    当 ((b = *p) != NUL)
    {
	如果 (b == c)
	    返回 p;
	++p;
    }
    返回 NULL;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, 整数 c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    如果 (c <= 0 || c > 255)
	返回 NULL;
    返回 (char_u *)strchr((char *)string, c);
}

/*
 * Search for last occurrence of "c" in "string".
 * Version of strrchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 * Return NULL if not found.
 * Does not handle multi-byte char for "c"!
 */
    char_u  *
vim_strrchr(char_u *string, 整数 c)
{
    char_u	*retval = NULL;
    char_u	*p = string;

    当 (*p)
    {
	如果 (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    返回 retval;
}

/*
 * Vim's version of strpbrk(), in case it's missing.
 * Don't generate a prototype for this, causes problems when it's not used.
 */
#ifndef PROTO
# ifndef HAVE_STRPBRK
#  ifdef vim_strpbrk
#   undef vim_strpbrk
#  endif
    char_u *
vim_strpbrk(char_u *s, char_u *charset)
{
    当 (*s)
    {
	如果 (vim_strchr(charset, *s) != NULL)
	    返回 s;
	MB_PTR_ADV(s);
    }
    返回 NULL;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
static 整数 sort_compare(const void *s1, const void *s2);

    static 整数
sort_compare(const void *s1, const void *s2)
{
    返回 STRCMP(*(char **)s1, *(char **)s2);
}

    void
sort_strings(
    char_u	**files,
    整数		count)
{
    qsort((void *)files, (size_t)count, sizeof(char_u *), sort_compare);
}

#如果 defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    整数
has_non_ascii(char_u *s)
{
    char_u	*p;

    如果 (s != NULL)
	为 (p = s; *p != NUL; ++p)
	    如果 (*p >= 128)
		返回 TRUE;
    返回 FALSE;
}
#endif

/*
 * Concatenate two strings and return the result in allocated memory.
 * Returns NULL when out of memory.
 */
    char_u  *
concat_str(char_u *str1, char_u *str2)
{
    char_u  *dest;
    size_t  l = str1 == NULL ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == NULL ? 0 : STRLEN(str2)) + 1L);
    如果 (dest == NULL)
	返回 NULL;
    如果 (str1 == NULL)
	*dest = NUL;
    否则
	STRCPY(dest, str1);
    如果 (str2 != NULL)
	STRCPY(dest + l, str2);
    返回 dest;
}

#如果 defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    如果 (rev == NULL)
	返回 NULL;

    为 (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	如果 (has_mbyte)
	{
	    整数 mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	否则
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    返回 rev;
}
#endif

#如果 defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, 整数 function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    如果 (str != NULL)
    {
	len += (unsigned)STRLEN(str);
	为 (p = str; *p != NUL; MB_PTR_ADV(p))
	    如果 (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    如果 (r == NULL)
	返回 NULL;

    如果 (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    否则
	*r++ = '\'';
    如果 (str != NULL)
	为 (p = str; *p != NUL; )
	{
	    如果 (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    如果 (function)
	*r++ = ')';
    *r++ = NUL;
    返回 s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_count(char_u *haystack, char_u *needle, 整数 ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    如果 (p == NULL || needle == NULL || *needle == NUL)
	返回 0;

    size_t  needlelen = STRLEN(needle);
    如果 (ic)
    {
	当 (*p != NUL)
	{
	    如果 (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    否则
		MB_PTR_ADV(p);
	}
    }
    否则
	当 ((next = (char_u *)strstr((char *)p, (char *)needle)) != NULL)
	{
	    ++n;
	    p = next + needlelen;
	}

    返回 n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    static 整数
copy_first_char_to_tv(char_u *input, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    整数		len;

    如果 (input == NULL || output == NULL)
	返回 -1;

    len = has_mbyte ? mb_ptr2len(input) : 1;
    STRNCPY(buf, input, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    返回 output->vval.v_string == NULL ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    void
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*expr,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    整数		len = 0;
    整数		idx = 0;
    整数		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    ga_init2(&ga, sizeof(char), 80);
    为 (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	如果 (len < 0)
	    break;

	set_vim_var_nr(VV_KEY, idx);
	如果 (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    break;
	}
	如果 (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    如果 (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		break;
	    }
	    否则
		ga_concat(&ga, newtv.vval.v_string);
	}
	否则 如果 (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    如果 (fc != NULL)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    void
string_reduce(
	typval_T	*argvars,
	typval_T	*expr,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    整数		len;
    typval_T	argv[3];
    整数		r;
    整数		called_emsg_start = called_emsg;
    funccall_T	*fc;

    如果 (argvars[2].v_type == VAR_UNKNOWN)
    {
	如果 (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "String");
	    返回;
	}
	len = copy_first_char_to_tv(p, rettv);
	如果 (len < 0)
	    返回;
	p += len;
    }
    否则 如果 (check_for_string_arg(argvars, 2) == FAIL)
	返回;
    否则
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    为 ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	如果 (len < 0)
	{
	    *rettv = argv[0];
	    break;
	}

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	如果 (r == FAIL || called_emsg != called_emsg_start)
	    break;
    }

    如果 (fc != NULL)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    static void
byteidx_common(typval_T *argvars, typval_T *rettv, 整数 comp)
{
    rettv->vval.v_number = -1;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	返回;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    如果 (str == NULL || idx < 0)
	返回;

    varnumber_T	utf16idx = FALSE;
    如果 (argvars[2].v_type != VAR_UNKNOWN)
    {
	整数 error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	如果 (error)
	    返回;
	如果 (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), utf16idx);
	    返回;
	}
    }

    整数 (*ptr2len)(char_u *);
    如果 (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    否则
	ptr2len = mb_ptr2len;

    char_u *t = str;
    为 ( ; idx > 0; idx--)
    {
	如果 (*t == NUL)		// EOL reached
	    返回;
	如果 (utf16idx)
	{
	    整数 clen = ptr2len(t);
	    整数 c = (clen > 1) ? utf_ptr2char(t) : *t;
	    如果 (c > 0xFFFF)
		idx--;
	    如果 (idx > 0)
		t += clen;
	}
	否则 如果 (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_number = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    void
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    void
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    void
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = -1;

    如果 (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	返回;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    如果 (str == NULL || idx < 0)
	返回;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    如果 (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	如果 (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    整数 (*ptr2len)(char_u *);
    如果 (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    否则
	ptr2len = mb_ptr2len;

    char_u	*p;
    整数		len;
    为 (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	如果 (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    如果 (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    返回;
	}
	如果 (utf16idx)
	{
	    idx--;
	    整数 clen = ptr2len(p);
	    整数 c = (clen > 1) ? utf_ptr2char(p) : *p;
	    如果 (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_number = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    static 整数
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    如果 (convert_setup(&vimconv, from, to) == FAIL)
	返回 FAIL;
    vimconv.vc_fail = TRUE;
    如果 (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	如果 (ret->string == NULL)
	    ret->length = 0;
	否则
	    ret->length = str->length;
    }
    否则
    {
	整数 len = (整数)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, NULL, NULL);

    返回 (ret->string == NULL) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    static void
blob_from_string(char_u *str, blob_T *blob)
{
    整数	    len = (整数)STRLEN(str);
    char_u  *dest;

    如果 (len == 0)
	返回;
    如果 (ga_grow(&blob->bv_ga, len) == FAIL)
	返回;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    为 (整数 i = 0; i < len; ++i)
	如果 (dest[i] == NL)
	    dest[i] = NUL;
    blob->bv_ga.ga_len += len;
}

/*
 * Return a string created from the bytes in blob starting at "start_idx".
 * A NL character in the blob indicates end of string.
 * A NUL character in the blob is translated to a NL.
 * If a newline is followed by another newline (empty line), then an empty
 * allocated string is returned and "start_idx" is moved forward by one byte.
 * On return, "start_idx" points to next byte to process in blob.
 */
    static 整数
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	line_len;

    如果 (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	返回 (ret->string == NULL) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    line_len = (nl == NULL) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(line_len + 1);
    如果 (ret->string == NULL)
    {
	ret->length = 0;
	返回 FAIL;
    }
    如果 (line_len > 0)
	mch_memmove(ret->string, src, (size_t)line_len);
    ret->string[line_len] = NUL;
    ret->length = (size_t)line_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    为 (long i = 0; i < line_len; i++)
	如果 (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + line_len + (nl != NULL ? 1 : 0);
    返回 OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    static char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    如果 (from_encoding_raw == NULL)
	返回 NULL;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    当 (*s != NUL)
    {
	如果 (*s == '_')
	    *pe++ = '-';
	否则
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    如果 ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    返回 from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    static void
append_converted_string_to_list(
	string_T *converted,
	整数 validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    如果 (converted->string != NULL)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	当 (p < end)
	{
	    string_T	line;
	    char_u	*line_start = p;

	    当 (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    line.length = (size_t)(p - line_start);
	    line.string = vim_strnsave(line_start, line.length);
	    如果 (line.string != NULL)
	    {
		如果 (validate_utf8 && !utf_valid_string(line.string, NULL))
		{
		    vim_free(line.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    返回; // Stop processing
		}
		如果 (list_append_string(list, line.string, (整数)line.length) == FAIL)
		{
		    vim_free(line.string);
		    vim_free(converted->string);
		    返回; // Stop processing on append failure
		}
		vim_free(line.string);
	    }
	    否则
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		返回;
	    }

	    如果 (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    否则
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    static 整数
append_validated_line_to_list(string_T *line, 整数 validate_utf8, list_T *list)
{
    如果 (validate_utf8 && !utf_valid_string(line->string, NULL))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(line->string);
	返回 FAIL;
    }

    整数 ret = list_append_string(list, line->string, (整数)line->length);
    vim_free(line->string);
    返回 ret;
}

    void
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    整数		blen;
    long	idx;
    整数		validate_utf8 = FALSE;

    如果 (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	返回;

    如果 (rettv_list_alloc(rettv) == FAIL)
	返回;

    blob = argvars->vval.v_blob;
    如果 (blob == NULL)
	返回;
    blen = blob_len(blob);

    char_u	*from_encoding = NULL;
    char_u	*from_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    如果 (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	如果 (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    如果 (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		如果 (from_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    返回;
		}
	    }
	}
    }

    如果 (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    如果 (from_encoding != NULL && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    整数 from_prop = 0;
    如果 (from_encoding != NULL)
	from_prop = enc_canon_props(from_encoding);
    如果 (from_encoding != NULL && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	整数 nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	如果 (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    如果 (blen > 0)
		mch_memmove(blob_ga.ga_data, blob->bv_ga.ga_data, (size_t)blen);
	    // NUL terminator (2 bytes for UTF-16/UCS-2, 4 bytes for UTF-32/UCS-4)
	    vim_memset((char_u *)blob_ga.ga_data + blen, 0, (size_t)nul_size);
	    blob_ga.ga_len = blen + nul_size;
	}

	// Convert the entire blob at once
	vimconv_T vimconv;
	vimconv.vc_type = CONV_NONE;
	// Use raw encoding name for iconv to preserve endianness (utf-16be vs utf-16)
	// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
	如果 (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    goto done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	整数 len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, NULL);
	converted.length = len;
	convert_setup(&vimconv, NULL, NULL);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    否则
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	当 (idx < blen)
	{
	    string_T	str;

	    如果 (string_from_blob(blob, &idx, &str) != OK)
		break;

	    如果 (from_encoding != NULL)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		整数	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		如果 (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    如果 (append_validated_line_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		goto done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    如果 (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

done:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    void
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    如果 (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	返回;

    如果 (rettv_blob_alloc(rettv) == FAIL)
	返回;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    如果 (list == NULL)
	返回;

    char_u	*to_encoding = NULL;
    char_u	*to_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    如果 (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	如果 (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    如果 (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		如果 (to_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    返回;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    整数 to_prop = 0;
    如果 (to_encoding != NULL)
	to_prop = enc_canon_props(to_encoding);
    如果 (to_encoding != NULL && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    char_u *s;

	    如果 (li->li_tv.v_type != VAR_STRING)
		continue;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    如果 (li != list->lv_first)
		ga_append(&str_ga, NL);
	    如果 (s != NULL && *s != NUL)
	    {
		整数 slen = (整数)STRLEN(s);

		如果 (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    goto done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	如果 (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    如果 (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    vimconv.vc_fail = TRUE;

	    整数		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, NULL);
	    convert_setup(&vimconv, NULL, NULL);
	    ga_clear(&str_ga);

	    如果 (converted == NULL)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    如果 (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	否则
	    ga_clear(&str_ga);
    }
    否则
    {
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    如果 (li->li_tv.v_type != VAR_STRING)
		continue;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    如果 (str.string == NULL)
		STR_LITERAL_SET(str, "");
	    否则
		str.length = STRLEN(str.string);

	    如果 (to_encoding != NULL)
	    {
		整数	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		如果 (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    如果 (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    如果 (to_encoding != NULL)
		vim_free(str.string);
	}
    }

done:
    如果 (to_encoding != NULL)
	vim_free(to_encoding);
    如果 (to_encoding_raw != NULL)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    void
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    整数		utf8 = FALSE;

    如果 (rettv_list_alloc(rettv) == FAIL)
	返回;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	返回;

    如果 (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (整数)tv_get_bool_chk(&argvars[1], NULL);

    p = tv_get_string(&argvars[0]);

    如果 (has_mbyte || utf8)
    {
	整数 (*ptr2len)(char_u *);
	整数 (*ptr2char)(char_u *);

	如果 (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	否则
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	为 ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_number(rettv->vval.v_list, (*ptr2char)(p));
    }
    否则
	为 ( ; *p != NUL; ++p)
	    list_append_number(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    void
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    整数		base = 10;
    char_u	*p;
    varnumber_T	n;
    整数		what = 0;
    整数		isneg;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	返回;

    如果 (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (整数)tv_get_number(&argvars[1]);
	如果 (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    返回;
	}
	如果 (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    如果 (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    switch (base)
    {
	case 2: what |= STR2NR_BIN + STR2NR_FORCE; break;
	case 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; break;
	case 16: what |= STR2NR_HEX + STR2NR_FORCE; break;
    }
    vim_str2nr(p, NULL, NULL, what, &n, NULL, 0, FALSE, NULL);
    // Text after the number is silently ignored.
    如果 (isneg)
	rettv->vval.v_number = -n;
    否则
	rettv->vval.v_number = n;

}

/*
 * "strgetchar()" function
 */
    void
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    整数		len;
    整数		error = FALSE;
    整数		charidx;
    整数		byteidx = 0;

    rettv->vval.v_number = -1;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	返回;

    str = tv_get_string_chk(&argvars[0]);
    如果 (str == NULL)
	返回;
    len = (整数)STRLEN(str);
    charidx = (整数)tv_get_number_chk(&argvars[1], &error);
    如果 (error)
	返回;

    当 (charidx >= 0 && byteidx < len)
    {
	如果 (charidx == 0)
	{
	    rettv->vval.v_number = mb_ptr2char(str + byteidx);
	    break;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    void
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    整数		start_idx;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	返回;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_number = -1;
    如果 (needle == NULL || haystack == NULL)
	返回;		// type error; errmsg already given

    如果 (argvars[2].v_type != VAR_UNKNOWN)
    {
	整数	    error = FALSE;

	start_idx = (整数)tv_get_number_chk(&argvars[2], &error);
	如果 (error || start_idx >= (整数)STRLEN(haystack))
	    返回;
	如果 (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((char *)haystack, (char *)needle);
    如果 (pos != NULL)
	rettv->vval.v_number = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    void
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    如果 (rettv->vval.v_string != NULL && tofree == NULL)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    void
f_strlen(typval_T *argvars, typval_T *rettv)
{
    如果 (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	返回;

    rettv->vval.v_number = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    static void
strchar_common(typval_T *argvars, typval_T *rettv, 整数 skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    整数			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    当 (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_number = len;
}

/*
 * "strcharlen()" function
 */
    void
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    如果 (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	返回;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    void
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	返回;

    如果 (argvars[1].v_type != VAR_UNKNOWN)
    {
	整数 error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	如果 (error)
	    返回;
	如果 (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), skipcc);
	    返回;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    void
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = -1;

    如果 (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	返回;

    varnumber_T countcc = FALSE;
    如果 (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    整数			(*func_mb_ptr2char_adv)(char_u **pp);
    整数			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    当 (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	如果 (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_number = len;
}

/*
 * "strdisplaywidth()" function
 */
    void
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    整数		col = 0;

    rettv->vval.v_number = -1;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	返回;

    s = tv_get_string(&argvars[0]);
    如果 (argvars[1].v_type != VAR_UNKNOWN)
	col = (整数)tv_get_number(&argvars[1]);

    rettv->vval.v_number = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    void
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    如果 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	返回;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_number = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    void
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    整数		nchar;
    整数		nbyte = 0;
    整数		charlen;
    整数		skipcc = FALSE;
    整数		len = 0;
    整数		slen;
    整数		error = FALSE;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	返回;

    p = tv_get_string(&argvars[0]);
    slen = (整数)STRLEN(p);

    nchar = (整数)tv_get_number_chk(&argvars[1], &error);
    如果 (!error)
    {
	如果 (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    如果 (error)
		返回;
	    如果 (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_number_as_bool_nr), skipcc);
		返回;
	    }
	}

	如果 (nchar > 0)
	    当 (nchar > 0 && nbyte < slen)
	    {
		如果 (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		否则
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	否则
	    nbyte = nchar;
	如果 (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (整数)tv_get_number(&argvars[2]);
	    当 (charlen > 0 && nbyte + len < slen)
	    {
		整数 off = nbyte + len;

		如果 (off < 0)
		    len += 1;
		否则
		{
		    如果 (skipcc)
			len += mb_ptr2len(p + off);
		    否则
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	否则
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    如果 (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    否则 如果 (nbyte > slen)
	nbyte = slen;
    如果 (len < 0)
	len = 0;
    否则 如果 (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    void
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    整数		n;
    整数		len;
    整数		slen;
    整数		error = FALSE;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	返回;

    p = tv_get_string(&argvars[0]);
    slen = (整数)STRLEN(p);

    n = (整数)tv_get_number_chk(&argvars[1], &error);
    如果 (error)
	len = 0;
    否则 如果 (argvars[2].v_type != VAR_UNKNOWN)
	len = (整数)tv_get_number(&argvars[2]);
    否则
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    如果 (n < 0)
    {
	len += n;
	n = 0;
    }
    否则 如果 (n > slen)
	n = slen;
    如果 (len < 0)
	len = 0;
    否则 如果 (n + len > slen)
	len = slen - n;

    如果 (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	整数 off;

	// length in characters
	为 (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    void
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = NULL;
    整数		haystack_len, end_idx;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	返回;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_number = -1;
    如果 (needle == NULL || haystack == NULL)
	返回;		// type error; errmsg already given

    haystack_len = (整数)STRLEN(haystack);
    如果 (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (整数)tv_get_number_chk(&argvars[2], NULL);
	如果 (end_idx < 0)
	    返回;	// can never find a match
    }
    否则
	end_idx = haystack_len;

    如果 (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    否则
    {
	为 (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((char *)rest, (char *)needle);
	    如果 (rest == NULL || rest > haystack + end_idx)
		break;
	    lastmatch = rest;
	}
    }

    如果 (lastmatch == NULL)
	rettv->vval.v_number = -1;
    否则
	rettv->vval.v_number = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    void
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    如果 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	返回;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    void
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = -1;

    如果 (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_number_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    返回;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    如果 (str == NULL || idx < 0)
	返回;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    如果 (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	如果 (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    整数 (*ptr2len)(char_u *);
    如果 (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    否则
	ptr2len = mb_ptr2len;

    char_u	*p;
    整数		len;
    整数		utf16idx = 0;
    为 (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	如果 (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    如果 (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    返回;
	}
	utf16idx = len;
	整数 clen = ptr2len(p);
	整数 c = (clen > 1) ? utf_ptr2char(p) : *p;
	如果 (c > 0xFFFF)
	    len++;
	p += clen;
	如果 (charidx)
	    idx--;
    }

    rettv->vval.v_number = utf16idx;
}

/*
 * "tolower(string)" function
 */
    void
f_tolower(typval_T *argvars, typval_T *rettv)
{
    如果 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	返回;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    void
f_toupper(typval_T *argvars, typval_T *rettv)
{
    如果 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	返回;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    void
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*in_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    整数		inlen;
    整数		fromlen;
    整数		tolen;
    整数		idx;
    char_u	*cpstr;
    整数		cplen;
    整数		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	返回;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    如果 (fromstr == NULL || tostr == NULL)
	    返回;		// type error; errmsg already given
    ga_init2(&ga, sizeof(char), 80);

    如果 (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	如果 (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    返回;
	}

    // fromstr and tostr have to contain the same number of chars
    当 (*in_str != NUL)
    {
	如果 (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    为 (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		如果 (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    为 (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			如果 (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    break;
			}
		    }
		    如果 (*p == NUL)	// tostr is shorter than fromstr
			goto error;
		    break;
		}
		++idx;
	    }

	    如果 (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		为 (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		如果 (idx != 0)
		    goto error;
	    }

	    (void)ga_grow(&ga, cplen);
	    mch_memmove((char *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	否则
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    如果 (p != NULL)
		ga_append(&ga, tostr[p - fromstr]);
	    否则
		ga_append(&ga, *in_str);
	    ++in_str;
	}
    }

    // add a terminating NUL
    (void)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_data;
}

/*
 * "trim({expr})" function
 */
    void
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*head;
    char_u	*mask = NULL;
    char_u	*tail;
    char_u	*prev;
    char_u	*p;
    整数		c1;
    整数		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	返回;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    如果 (head == NULL)
	返回;

    如果 (check_for_opt_string_arg(argvars, 1) == FAIL)
	返回;

    如果 (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	如果 (*mask == NUL)
	    mask = NULL;

	如果 (argvars[2].v_type != VAR_UNKNOWN)
	{
	    整数	error = 0;

	    // leading or trailing characters to trim
	    dir = (整数)tv_get_number_chk(&argvars[2], &error);
	    如果 (error)
		返回;
	    如果 (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		返回;
	    }
	}
    }

    如果 (dir == 0 || dir == 1)
    {
	// Trim leading characters
	当 (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    如果 (mask == NULL)
	    {
		如果 (c1 > ' ' && c1 != 0xa0)
		    break;
	    }
	    否则
	    {
		为 (p = mask; *p != NUL; MB_PTR_ADV(p))
		    如果 (c1 == PTR2CHAR(p))
			break;
		如果 (*p == NUL)
		    break;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    如果 (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	为 (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    如果 (mask == NULL)
	    {
		如果 (c1 > ' ' && c1 != 0xa0)
		    break;
	    }
	    否则
	    {
		为 (p = mask; *p != NUL; MB_PTR_ADV(p))
		    如果 (c1 == PTR2CHAR(p))
			break;
		如果 (*p == NUL)
		    break;
	    }
	}
    }
    rettv->vval.v_string = vim_strnsave(head, tail - head);
}

/*
 * Decodes a URI-encoded string.
 *
 * Parameters:
 *   str - The URI-encoded input string (may contain %XX sequences and '+').
 *
 * Returns:
 *   A newly allocated string with URI encoding decoded:
 *     - %XX sequences are converted to the corresponding character.
 *     - If the input is malformed (e.g., incomplete % sequence), the original
 *       characters are copied.
 *   The output string will never be longer than the input string.
 *   The caller is responsible for freeing the returned string.
 *
 * Returns NULL if input is NULL or memory allocation fails.
 */
    static char_u *
uri_decode(char_u *str)
{
    如果 (str == NULL)
	返回 NULL;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    如果 (!decoded)
	返回 NULL;

    char_u	*p = decoded;
    size_t	i = 0;

    当 (i < len)
    {
	如果 (str[i] == '%')
	{
	    如果 (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		如果 (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    否则
	    {
		整数 val = hexhex2nr(&str[i + 1]);
		如果 (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		否则
		{
		    // invalid hex digits following "%"
		    为 (整数 j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	否则
	    *p++ = str[i++];
    }

    *p = NUL;

    返回 decoded;
}

/*
 * "uri_decode({str})" function
 */
    void
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    如果 (check_for_string_arg(argvars, 0) == FAIL)
	返回;

    rettv->vval.v_string = uri_decode(tv_get_string(&argvars[0]));
}

/*
 * Encodes a string for safe use in a URI.
 *
 * Parameters:
 *   str - The input string to encode.
 *
 * Returns:
 *   A newly allocated string where:
 *     - Alphanumeric characters and '-', '_', '.', '~' are left unchanged.
 *     - All other bytes are encoded as %XX (uppercase hex).
 *   The caller is responsible for freeing the returned string.
 *
 *   Returns NULL if input is NULL or memory allocation fails.
 */
    static char_u *
uri_encode(char_u *str)
{
    如果 (str == NULL)
	返回 NULL;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    如果 (encoded == NULL)
	返回 NULL;

    char_u *p = encoded;

    为 (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	如果 (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	否则
	{
	    sprintf((char *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    返回 encoded;
}

/*
 * "uri_encode({str})" function
 */
    void
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    如果 (check_for_string_arg(argvars, 0) == FAIL)
	返回;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

static char *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static varnumber_T
tv_nr(typval_T *tvs, 整数 *idxp)
{
    整数		idx = *idxp - 1;
    varnumber_T	n = 0;
    整数		err = FALSE;

    如果 (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    否则
    {
	++*idxp;
	n = tv_get_number_chk(&tvs[idx], &err);
	如果 (err)
	    n = 0;
    }
    返回 n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    static char *
tv_str(typval_T *tvs, 整数 *idxp, char_u **tofree)
{
    整数		    idx = *idxp - 1;
    char	    *s = NULL;
    static char_u   numbuf[NUMBUFLEN];

    如果 (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    否则
    {
	++*idxp;
	如果 (tofree != NULL)
	    s = (char *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	否则
	    s = (char *)tv_get_string_chk(&tvs[idx]);
    }
    返回 s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static double
tv_float(typval_T *tvs, 整数 *idxp)
{
    整数		idx = *idxp - 1;
    double	f = 0;

    如果 (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    否则
    {
	++*idxp;
	如果 (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	否则 如果 (tvs[idx].v_type == VAR_NUMBER)
	    f = (double)tvs[idx].vval.v_number;
	否则
	    emsg(_(e_expected_float_argument_for_printf));
    }
    返回 f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    static const char *
infinity_str(整数 positive,
	     char fmt_spec,
	     整数 force_sign,
	     整数 space_for_positive)
{
    static const char *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    整数 idx = positive * (1 + force_sign + force_sign * space_for_positive);

    如果 (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    返回 table[idx];
}

/*
 * This code was included to provide a portable vsnprintf() and snprintf().
 * Some systems may provide their own, but we always use this one for
 * consistency.
 *
 * This code is based on snprintf.c - a portable implementation of snprintf
 * by Mark Martinec <mark.martinec@ijs.si>, Version 2.2, 2000-10-06.
 * Included with permission.  It was heavily modified to fit in Vim.
 * The original code, including useful comments, can be found here:
 *	http://www.ijs.si/software/snprintf/
 *
 * This snprintf() only supports the following conversion specifiers:
 * s, c, d, u, o, x, X, p  (and synonyms: i, D, U, O - see below)
 * with flags: '-', '+', ' ', '0' and '#'.
 * An asterisk is supported for field width as well as precision.
 *
 * Limited support for floating point was added: 'f', 'F', 'e', 'E', 'g', 'G'.
 *
 * Length modifiers 'h' (short int) and 'l' (long int) and 'll' (long long int)
 * are supported.  NOTE: for 'll' the argument is varnumber_T or uvarnumber_T.
 *
 * The locale is not used, the string is used as a byte string.  This is only
 * relevant for double-byte encodings where the second byte may be '%'.
 *
 * It is permitted for "str_m" to be zero, and it is permitted to specify NULL
 * pointer for resulting string argument if "str_m" is zero (as per ISO C99).
 *
 * The return value is the number of characters which would be generated
 * for the given input, excluding the trailing NUL. If this value
 * is greater or equal to "str_m", not all characters from the result
 * have been stored in str, output bytes beyond the ("str_m"-1) -th character
 * are discarded. If "str_m" is greater than zero it is guaranteed
 * the resulting string will be NUL-terminated.
 */

/*
 * When va_list is not supported we only define vim_snprintf().
 *
 * vim_vsnprintf_typval() can be invoked with either "va_list" or a list of
 * "typval_T".  When the latter is not used it must be NULL.
 */

// When generating prototypes all of this is skipped, cproto doesn't
// understand this.
#ifndef PROTO

// Like vim_vsnprintf() but append to the string.
    整数
vim_snprintf_add(char *str, size_t str_m, const char *fmt, ...)
{
    va_list	ap;
    整数		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    如果 (str_m <= len)
	space = 0;
    否则
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    返回 str_l;
}

    整数
vim_snprintf(char *str, size_t str_m, const char *fmt, ...)
{
    va_list	ap;
    整数		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    返回 str_l;
}

/*
 * Like vim_snprintf() except the return value can be safely used to increment a
 * buffer length.
 * Normal `snprintf()` (and `vim_snprintf()`) returns the number of bytes that
 * would have been copied if the destination buffer was large enough.
 * This means that you cannot rely on it's return value for the destination
 * length because the destination may be shorter than the source. This function
 * guarantees the returned length will never be greater than the destination length.
 */
    size_t
vim_snprintf_safelen(char *str, size_t str_m, const char *fmt, ...)
{
    va_list ap;
    整数	    str_l;

    如果 (str_m == 0)
	返回 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
    va_end(ap);

    如果 (str_l < 0)
    {
	*str = NUL;
	返回 0;
    }
    返回 ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    整数
vim_vsnprintf(
    char	*str,
    size_t	str_m,
    const char	*fmt,
    va_list	ap)
{
    返回 vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
}

enum
{
    TYPE_UNKNOWN = -1,
    TYPE_INT,
    TYPE_LONGINT,
    TYPE_LONGLONGINT,
    TYPE_UNSIGNEDINT,
    TYPE_UNSIGNEDLONGINT,
    TYPE_UNSIGNEDLONGLONGINT,
    TYPE_POINTER,
    TYPE_PERCENT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_FLOAT
};

/* Types that can be used in a format string
 */
    static 整数
format_typeof(
    const char	*type)
{
    // allowed values: \0, h, l, L
    char    length_modifier = '\0';

    // current conversion specifier character
    char    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    如果 (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	如果 (length_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    length_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    switch (fmt_spec)
    {
	case 'i': fmt_spec = 'd'; break;
	case '*': fmt_spec = 'd'; length_modifier = 'h'; break;
	case 'D': fmt_spec = 'd'; length_modifier = 'l'; break;
	case 'U': fmt_spec = 'u'; length_modifier = 'l'; break;
	case 'O': fmt_spec = 'o'; length_modifier = 'l'; break;
	default: break;
    }

    // get parameter value, do initial processing
    switch (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    case '%':
	返回 TYPE_PERCENT;

    case 'c':
	返回 TYPE_CHAR;

    case 's':
    case 'S':
	返回 TYPE_STRING;

    case 'd': case 'u':
    case 'b': case 'B':
    case 'o':
    case 'x': case 'X':
    case 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    // value

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    如果 (fmt_spec == 'p')
		返回 TYPE_POINTER;
	    否则 如果 (fmt_spec == 'b' || fmt_spec == 'B')
		返回 TYPE_UNSIGNEDLONGLONGINT;
	    否则 如果 (fmt_spec == 'd')
	    {
		// signed
		switch (length_modifier)
		{
		case '\0':
		case 'h':
		    // char and short arguments are passed as int.
		    返回 TYPE_INT;
		case 'l':
		    返回 TYPE_LONGINT;
		case 'L':
		    返回 TYPE_LONGLONGINT;
		}
	    }
	    否则
	    {
		// unsigned
		switch (length_modifier)
		{
		    case '\0':
		    case 'h':
			返回 TYPE_UNSIGNEDINT;
		    case 'l':
			返回 TYPE_UNSIGNEDLONGINT;
		    case 'L':
			返回 TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	break;

    case 'f':
    case 'F':
    case 'e':
    case 'E':
    case 'g':
    case 'G':
	返回 TYPE_FLOAT;
    }

    返回 TYPE_UNKNOWN;
}

    static char *
format_typename(
    const char  *type)
{
    switch (format_typeof(type))
    {
	case TYPE_INT:
	    返回 typename_int;

	case TYPE_LONGINT:
	    返回 typename_longint;

	case TYPE_LONGLONGINT:
	    返回 typename_longlongint;

	case TYPE_UNSIGNEDINT:
	    返回 typename_unsignedint;

	case TYPE_UNSIGNEDLONGINT:
	    返回 typename_unsignedlongint;

	case TYPE_UNSIGNEDLONGLONGINT:
	    返回 typename_unsignedlonglongint;

	case TYPE_POINTER:
	    返回 _(typename_pointer);

	case TYPE_PERCENT:
	    返回 _(typename_percent);

	case TYPE_CHAR:
	    返回 typename_char;

	case TYPE_STRING:
	    返回 _(typename_string);

	case TYPE_FLOAT:
	    返回 typename_float;
    }

    返回 _(typename_unknown);
}

    static 整数
adjust_types(
    const char ***ap_types,
    整数 arg,
    整数 *num_posarg,
    const char *type)
{
    如果 (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	返回 FAIL;
    }

    如果 (*ap_types == NULL || *num_posarg < arg)
    {
	整数	    idx;
	const char  **new_types;

	如果 (*ap_types == NULL)
	    new_types = ALLOC_CLEAR_MULT(const char *, arg);
	否则
	    new_types = vim_realloc((char **)*ap_types,
						arg * sizeof(const char *));

	如果 (new_types == NULL)
	    返回 FAIL;

	为 (idx = *num_posarg; idx < arg; ++idx)
	    new_types[idx] = NULL;

	*ap_types = new_types;
	*num_posarg = arg;
    }

    如果 ((*ap_types)[arg - 1] != NULL)
    {
	如果 ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    const char *pt = type;
	    如果 (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    如果 (pt[0] != '*')
	    {
		switch (pt[0])
		{
		    case 'd':
		    case 'i': break;
		    default:
			semsg(_(e_positional_num_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			返回 FAIL;
		}
	    }
	}
	否则
	{
	    如果 (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_num_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		返回 FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    返回 OK;
}

    static void
format_overflow_error(const char *pstart)
{
    size_t	arglen = 0;
    char	*argcopy = NULL;
    const char	*p = pstart;

    当 (VIM_ISDIGIT((整数)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(char, arglen + 1);
    如果 (argcopy != NULL)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	free(argcopy);
    }
    否则
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    static 整数
get_unsigned_int(
    const char *pstart,
    const char **p,
    unsigned 整数 *uj,
    整数 overflow_err)
{
    *uj = **p - '0';
    ++*p;

    当 (VIM_ISDIGIT((整数)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned 整数)(**p - '0');
	++*p;
    }

    如果 (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	如果 (overflow_err)
	{
	    format_overflow_error(pstart);
	    返回 FAIL;
	}
	否则
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    返回 OK;
}


    static 整数
parse_fmt_types(
    const char  ***ap_types,
    整数		*num_posarg,
    const char  *fmt,
    typval_T	*tvs UNUSED
    )
{
    const char	*p = fmt;
    const char	*arg = NULL;

    整数		any_pos = 0;
    整数		any_arg = 0;
    整数		arg_idx;

# define CHECK_POS_ARG do { \
    如果 (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	goto error; \
    } \
} 当 (0);

    如果 (p == NULL)
	返回 OK;

    当 (*p != NUL)
    {
	如果 (*p != '%')
	{
	    const char    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	否则
	{
	    // allowed values: \0, h, l, L
	    char	length_modifier = '\0';

	    // variable for positional arg
	    整数		pos_arg = -1;
	    const char	*ptype = NULL;
	    const char	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    当 (VIM_ISDIGIT(*ptype))
		++ptype;

	    如果 (*ptype == '$')
	    {
		如果 (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}

		// Positional argument
		unsigned 整数 uj;

		如果 (get_unsigned_int(pstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    当 (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		switch (*p)
		{
		    case '0': break;
		    case '-': break;
		    case '+': break;
		    case ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      break;
		    case '#': break;
		    case '\'': break;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    如果 (*(arg = p) == '*')
	    {
		p++;

		如果 (VIM_ISDIGIT((整数)(*p)))
		{
		    // Positional argument field width
		    unsigned 整数 uj;

		    如果 (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    如果 (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		    否则
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			如果 (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
			    goto error;
		    }
		}
		否则
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    否则 如果 (VIM_ISDIGIT((整数)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const char *digstart = p;
		unsigned 整数 uj;

		如果 (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		如果 (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}
	    }

	    // parse precision
	    如果 (*p == '.')
	    {
		p++;

		如果 (*(arg = p) == '*')
		{
		    p++;

		    如果 (VIM_ISDIGIT((整数)(*p)))
		    {
			// Parse precision
			unsigned 整数 uj;

			如果 (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			如果 (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    如果 (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
				goto error;
			}
			否则
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    goto error;
			}
		    }
		    否则
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		否则 如果 (VIM_ISDIGIT((整数)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const char *digstart = p;
		    unsigned 整数 uj;

		    如果 (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    如果 (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		}
	    }

	    如果 (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    如果 (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		如果 (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    switch (*p)
	    {
		// Check for known format specifiers. % is special!
		case 'i':
		case '*':
		case 'd':
		case 'u':
		case 'o':
		case 'D':
		case 'U':
		case 'O':
		case 'x':
		case 'X':
		case 'b':
		case 'B':
		case 'c':
		case 's':
		case 'S':
		case 'p':
		case 'f':
		case 'F':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
		    如果 (pos_arg != -1)
		    {
			如果 (adjust_types(ap_types, pos_arg, num_posarg, ptype) == FAIL)
			    goto error;
		    }
		    否则
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    break;

		default:
		    如果 (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			goto error;
		    }
	    }

	    如果 (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    为 (arg_idx = 0; arg_idx < *num_posarg; ++arg_idx)
    {
	如果 ((*ap_types)[arg_idx] == NULL)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    goto error;
	}

# 如果 defined(FEAT_EVAL)
	如果 (tvs != NULL && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    goto error;
	}
# endif
    }

    返回 OK;

error:
    vim_free((char**)*ap_types);
    *ap_types = NULL;
    *num_posarg = 0;
    返回 FAIL;
}

    static void
skip_to_arg(
    const char	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    整数		*arg_idx,
    整数		*arg_cur,
    const char	*fmt)
{
    整数		arg_min = 0;

    如果 (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	返回;
    }

    如果 (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    否则
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    为 (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	const char *p;

	如果 (ap_types == NULL || ap_types[*arg_cur] == NULL)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    返回;
	}

	p = ap_types[*arg_cur];

	整数 fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	switch (fmt_type)
	{
	case TYPE_PERCENT:
	case TYPE_UNKNOWN:
	    break;

	case TYPE_CHAR:
	    va_arg(*ap, 整数);
	    break;

	case TYPE_STRING:
	    va_arg(*ap, char *);
	    break;

	case TYPE_POINTER:
	    va_arg(*ap, void *);
	    break;

	case TYPE_INT:
	    va_arg(*ap, 整数);
	    break;

	case TYPE_LONGINT:
	    va_arg(*ap, long 整数);
	    break;

	case TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    break;

	case TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned 整数);
	    break;

	case TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long 整数);
	    break;

	case TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    break;

	case TYPE_FLOAT:
	    va_arg(*ap, double);
	    break;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    返回;
}

    整数
vim_vsnprintf_typval(
    char	*str,
    size_t	str_m,
    const char	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    const char	*p = fmt;
    整数		arg_cur = 0;
    整数		num_posarg = 0;
    整数		arg_idx = 1;
    va_list	ap;
    const char	**ap_types = NULL;

    如果 (parse_fmt_types(&ap_types, &num_posarg, fmt, tvs) == FAIL)
	返回 0;

    va_copy(ap, ap_start);

    如果 (p == NULL)
	p = "";
    当 (*p != NUL)
    {
	如果 (*p != '%')
	{
	    const char    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    如果 (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	否则
	{
	    size_t  min_field_width = 0, precision = 0;
	    整数	    zero_padding = 0, precision_specified = 0, justify_left = 0;
	    整数	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    整数	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    char    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    char    tmp[TMP_LEN];

	    // string address in case of string argument
	    const char  *str_arg = NULL;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    unsigned char uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  number_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    char    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = NULL;

	    // variables for positional arg
	    整数	    pos_arg = -1;
	    const char	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    当 (VIM_ISDIGIT(*ptype))
		++ptype;

	    如果 (*ptype == '$')
	    {
		// Positional argument
		const char *digstart = p;
		unsigned 整数 uj;

		如果 (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    当 (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		switch (*p)
		{
		    case '0': zero_padding = 1; break;
		    case '-': justify_left = 1; break;
		    case '+': force_sign = 1; space_for_positive = 0; break;
		    case ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      break;
		    case '#': alternate_form = 1; break;
		    case '\'': break;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    如果 (*p == '*')
	    {
		整数 j;
		const char *digstart = p + 1;

		p++;

		如果 (VIM_ISDIGIT((整数)(*p)))
		{
		    // Positional argument field width
		    unsigned 整数 uj;

		    如果 (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# 如果 defined(FEAT_EVAL)
		    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, 整数));

		如果 (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    如果 (tvs != NULL)
		    {
			format_overflow_error(digstart);
			goto error;
		    }
		    否则
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		如果 (j >= 0)
		    min_field_width = j;
		否则
		{
		    min_field_width = -j;
		    justify_left = 1;
		}
	    }
	    否则 如果 (VIM_ISDIGIT((整数)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const char *digstart = p;
		unsigned 整数 uj;

		如果 (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		min_field_width = uj;
	    }

	    // parse precision
	    如果 (*p == '.')
	    {
		p++;
		precision_specified = 1;

		如果 (VIM_ISDIGIT((整数)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const char *digstart = p;
		    unsigned 整数 uj;

		    如果 (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    precision = uj;
		}
		否则 如果 (*p == '*')
		{
		    整数 j;
		    const char *digstart = p;

		    p++;

		    如果 (VIM_ISDIGIT((整数)(*p)))
		    {
			// positional argument
			unsigned 整数 uj;

			如果 (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			arg_idx = uj;

			++p;
		    }

		    j =
# 如果 defined(FEAT_EVAL)
			tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, 整数));

		    如果 (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			如果 (tvs != NULL)
			{
			    format_overflow_error(digstart);
			    goto error;
			}
			否则
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    如果 (j >= 0)
			precision = j;
		    否则
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    如果 (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		如果 (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    length_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    switch (fmt_spec)
	    {
		case 'i': fmt_spec = 'd'; break;
		case 'D': fmt_spec = 'd'; length_modifier = 'l'; break;
		case 'U': fmt_spec = 'u'; length_modifier = 'l'; break;
		case 'O': fmt_spec = 'o'; length_modifier = 'l'; break;
		default: break;
	    }

# 如果 defined(FEAT_EVAL)
	    switch (fmt_spec)
	    {
		case 'd': case 'u': case 'o': case 'x': case 'X':
		    如果 (tvs != NULL && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    如果 (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    switch (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    case '%':
	    case 'c':
	    case 's':
	    case 'S':
		str_arg_l = 1;
		switch (fmt_spec)
		{
		case '%':
		    str_arg = p;
		    break;

		case 'c':
		    {
			整数 j;

			j =
# 如果 defined(FEAT_EVAL)
			    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, 整数));

			// standard demands unsigned char
			uchar_arg = (unsigned char)j;
			str_arg = (char *)&uchar_arg;
			break;
		    }

		case 's':
		case 'S':
		    str_arg =
# 如果 defined(FEAT_EVAL)
				tvs != NULL ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, char *));

		    如果 (str_arg == NULL)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    否则 如果 (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    否则 如果 (precision == 0)
			str_arg_l = 0;
		    否则
		    {
			// memchr on HP does not like n > 2^31  !!!
			const char *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == NULL) ? precision
						      : (size_t)(q - str_arg);
		    }
		    如果 (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			整数	cell;

			为 (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    如果 (precision_specified && i + cell > precision)
				break;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			如果 (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    break;

		default:
		    break;
		}
		break;

	    case 'd': case 'u':
	    case 'b': case 'B':
	    case 'o':
	    case 'x': case 'X':
	    case 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    // value

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    整数 arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    整数 int_arg = 0;
		    unsigned 整数 uint_arg = 0;

		    // only set for length modifier l
		    long 整数 long_arg = 0;
		    unsigned long 整数 ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    void *ptr_arg = NULL;

		    如果 (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# 如果 defined(FEAT_EVAL)
				 tvs != NULL ? (void *)tv_str(tvs, &arg_idx,
									NULL) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, void *));

			如果 (ptr_arg != NULL)
			    arg_sign = 1;
		    }
		    否则 如果 (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# 如果 defined(FEAT_EVAL)
				    tvs != NULL ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			如果 (bin_arg != 0)
			    arg_sign = 1;
		    }
		    否则 如果 (fmt_spec == 'd')
		    {
			// signed
			switch (length_modifier)
			{
			case '\0':
			case 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# 如果 defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, 整数));

			    如果 (int_arg > 0)
				arg_sign =  1;
			    否则 如果 (int_arg < 0)
				arg_sign = -1;
			    break;
			case 'l':
			    long_arg =
# 如果 defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long 整数));

			    如果 (long_arg > 0)
				arg_sign =  1;
			    否则 如果 (long_arg < 0)
				arg_sign = -1;
			    break;
			case 'L':
			    llong_arg =
# 如果 defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    如果 (llong_arg > 0)
				arg_sign =  1;
			    否则 如果 (llong_arg < 0)
				arg_sign = -1;
			    break;
			}
		    }
		    否则
		    {
			// unsigned
			switch (length_modifier)
			{
			    case '\0':
			    case 'h':
				uint_arg =
# 如果 defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned 整数));

				如果 (uint_arg != 0)
				    arg_sign = 1;
				break;
			    case 'l':
				ulong_arg =
# 如果 defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long 整数));

				如果 (ulong_arg != 0)
				    arg_sign = 1;
				break;
			    case 'L':
				ullong_arg =
# 如果 defined(FEAT_EVAL)
					    tvs != NULL ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				如果 (ullong_arg != 0)
				    arg_sign = 1;
				break;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    如果 (precision_specified)
			zero_padding = 0;
		    如果 (fmt_spec == 'd')
		    {
			如果 (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    否则 如果 (alternate_form)
		    {
			如果 (arg_sign != 0
				     && (fmt_spec == 'b' || fmt_spec == 'B'
				      || fmt_spec == 'x' || fmt_spec == 'X') )
			{
			    tmp[str_arg_l++] = '0';
			    tmp[str_arg_l++] = fmt_spec;
			}
			// alternate form should have no effect for p
			// conversion, but ...
		    }

		    zero_padding_insertion_ind = str_arg_l;
		    如果 (!precision_specified)
			precision = 1;   // default precision is 1
		    如果 (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    否则
		    {
			char	f[6];
			整数	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			如果 (!length_modifier)
			    ;
			否则 如果 (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# 否则
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			否则
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			如果 (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			否则 如果 (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    char	    b[8 * sizeof(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    do
			    {
				b[sizeof(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    当 (bn != 0);

			    memcpy(tmp + str_arg_l, b + sizeof(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			否则 如果 (fmt_spec == 'd')
			{
			    // signed
			    switch (length_modifier)
			    {
			    case '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       break;
			    case 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (short)int_arg);
				      break;
			    case 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      break;
			    case 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      break;
			    }
			}
			否则
			{
			    // unsigned
			    switch (length_modifier)
			    {
			    case '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       break;
			    case 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(unsigned short)uint_arg);
				      break;
			    case 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      break;
			    case 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      break;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			如果 (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			如果 (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t num_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			如果 (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    如果 (!precision_specified
					     || precision < num_of_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = num_of_digits + 1;
			    }
			}
			// zero padding to specified precision?
			如果 (num_of_digits < precision)
			    number_of_zeros_to_pad = precision - num_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    如果 (!justify_left && zero_padding)
		    {
			整数 n = (整数)(min_field_width - (str_arg_l
						    + number_of_zeros_to_pad));
			如果 (n > 0)
			    number_of_zeros_to_pad += n;
		    }
		    break;
		}

	    case 'f':
	    case 'F':
	    case 'e':
	    case 'E':
	    case 'g':
	    case 'G':
		{
		    // Floating point.
		    double	f;
		    double	abs_f;
		    char	format[40];
		    整数		l;
		    整数		remove_trailing_zeroes = FALSE;

		    f =
# 如果 defined(FEAT_EVAL)
			tvs != NULL ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, double));

		    abs_f = f < 0 ? -f : f;

		    如果 (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			如果 ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			否则
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    如果 ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# 否则
			    abs_f > 1.0e307
# endif
			    )
		    {
			// Avoid a buffer overflow
			STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			str_arg_l = STRLEN(tmp);
			zero_padding = 0;
		    }
		    否则
		    {
			如果 (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			否则 如果 (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			否则
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    如果 (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    如果 (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				如果 ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				如果 (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (整数)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			如果 (remove_trailing_zeroes)
			{
			    整数 i;
			    char *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    如果 (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    否则
			    {
				tp = (char *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				如果 (tp != NULL)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    如果 (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    当 (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    如果 (tp != NULL && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				当 (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			否则
			{
			    char *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (char *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    如果 (tp != NULL && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    如果 (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			number_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    break;
		}

	    default:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_left = 1;
		min_field_width = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		如果 (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		break;
	    }

	    如果 (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    如果 (!justify_left)
	    {
		// left padding with blank or zero
		整数 pn = (整数)(min_field_width - (str_arg_l + number_of_zeros_to_pad));

		如果 (pn > 0)
		{
		    如果 (str_l < str_m)
		    {
			size_t avail = str_m - str_l;

			vim_memset(str + str_l, zero_padding ? '0' : ' ',
					     (size_t)pn > avail ? avail
								: (size_t)pn);
		    }
		    str_l += pn;
		}
	    }

	    // zero padding as requested by the precision or by the minimal
	    // field width for numeric conversions required?
	    如果 (number_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    否则
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		整数 zn = (整数)zero_padding_insertion_ind;

		如果 (zn > 0)
		{
		    如果 (str_l < str_m)
		    {
			size_t avail = str_m - str_l;

			mch_memmove(str + str_l, str_arg,
					     (size_t)zn > avail ? avail
								: (size_t)zn);
		    }
		    str_l += zn;
		}

		// insert zero padding as requested by the precision or min
		// field width
		zn = (整数)number_of_zeros_to_pad;
		如果 (zn > 0)
		{
		    如果 (str_l < str_m)
		    {
			size_t avail = str_m - str_l;

			vim_memset(str + str_l, '0',
					     (size_t)zn > avail ? avail
								: (size_t)zn);
		    }
		    str_l += zn;
		}
	    }

	    // insert formatted string
	    // (or as-is conversion specifier for unknown conversions)
	    {
		整数 sn = (整数)(str_arg_l - zero_padding_insertion_ind);

		如果 (sn > 0)
		{
		    如果 (str_l < str_m)
		    {
			size_t avail = str_m - str_l;

			mch_memmove(str + str_l,
				str_arg + zero_padding_insertion_ind,
				(size_t)sn > avail ? avail : (size_t)sn);
		    }
		    str_l += sn;
		}
	    }

	    // insert right padding
	    如果 (justify_left)
	    {
		// right blank padding to the field width
		整数 pn = (整数)(min_field_width
				      - (str_arg_l + number_of_zeros_to_pad));

		如果 (pn > 0)
		{
		    如果 (str_l < str_m)
		    {
			size_t avail = str_m - str_l;

			vim_memset(str + str_l, ' ',
					     (size_t)pn > avail ? avail
								: (size_t)pn);
		    }
		    str_l += pn;
		}
	    }
	    vim_free(tofree);
	}
    }

    如果 (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    如果 (tvs != NULL && tvs[num_posarg != 0 ? num_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((char*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    返回 (整数)str_l;
}

#endif // PROTO
