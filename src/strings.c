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
    nếu (p != NULL)
	mch_memmove(p, string, len);
    trả_về p;
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
    nếu (p == NULL)
	trả_về NULL;

    STRNCPY(p, string, len);
    p[len] = NUL;
    trả_về p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    trả_về vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    số_nguyên		cc,
    số_nguyên		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    số_nguyên		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    cho (p = string; *p; p++)
    {
	nếu (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    continue;
	}
	nếu (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    nếu (escaped_string == NULL)
	trả_về NULL;
    p2 = escaped_string;
    cho (p = string; *p; p++)
    {
	nếu (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    continue;
	}
	nếu (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    trả_về escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    số_nguyên
csh_like_shell(void)
{
    trả_về (strstr((char *)gettail(p_sh), "csh") != NULL);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    static số_nguyên
fish_like_shell(void)
{
    trả_về (strstr((char *)gettail(p_sh), "fish") != NULL);
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
vim_strsave_shellescape(char_u *string, số_nguyên do_special, số_nguyên do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    số_nguyên		csh_like;
    số_nguyên		fish_like;
    char_u	*shname;
    số_nguyên		powershell;
#ifdef MSWIN
    số_nguyên		double_quotes;
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
    cho (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	nếu (double_quotes)
	{
	    nếu (*p == '"')
		++length;		// " -> ""
	}
	không_thì
#endif
	nếu (*p == '\'')
	{
	    nếu (powershell)
		length += 2;		// ' => ''
	    không_thì
		length += 3;		// ' => '\''
	}
	nếu ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    nếu (csh_like && do_special)
		++length;		// insert backslash
	}
	nếu (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	nếu (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    nếu (escaped_string != NULL)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	nếu (double_quotes)
	    *d++ = '"';
	không_thì
#endif
	    *d++ = '\'';

	cho (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    nếu (double_quotes)
	    {
		nếu (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    continue;
		}
	    }
	    không_thì
#endif
	    nếu (*p == '\'')
	    {
		nếu (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		không_thì
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		continue;
	    }
	    nếu ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		nếu (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		continue;
	    }
	    nếu (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		continue;
	    }
	    nếu (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		continue;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	nếu (double_quotes)
	    *d++ = '"';
	không_thì
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    trả_về escaped_string;
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
    trả_về p1;
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
    trả_về p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    void
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    số_nguyên	    c;

    nếu (p == NULL)
	trả_về;

    p2 = p;
    trong_khi ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#nếu defined(FEAT_EVAL) || defined(FEAT_SPELL)
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

    nếu (res != NULL)
	trong_khi (*p != NUL)
	{
	    số_nguyên		l;

	    nếu (enc_utf8)
	    {
		số_nguyên	c, uc;
		số_nguyên	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		nếu (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		nếu (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    nếu (s == NULL)
		    {
			vim_free(res);
			trả_về NULL;
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
	    không_thì nếu (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    không_thì
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    trả_về res;
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

    nếu (res != NULL)
	trong_khi (*p != NUL)
	{
	    số_nguyên		l;

	    nếu (enc_utf8)
	    {
		số_nguyên	c, lc;
		số_nguyên	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		nếu (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		nếu (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    nếu (s == NULL)
		    {
			vim_free(res);
			trả_về NULL;
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
	    không_thì nếu (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    không_thì
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    trả_về res;
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
    trong_khi (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
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

    nếu (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    không_thì
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(char *s, size_t maxlen)
{
    size_t i;
    cho (i = 0; i < maxlen; ++i)
	nếu (s[i] == NUL)
	    break;
    trả_về i;
}

#nếu !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    số_nguyên
vim_stricmp(char *s1, char *s2)
{
    số_nguyên		i;

    cho (;;)
    {
	i = (số_nguyên)TOLOWER_LOC(*s1) - (số_nguyên)TOLOWER_LOC(*s2);
	nếu (i != 0)
	    trả_về i;			    // this character different
	nếu (*s1 == NUL)
	    break;			    // strings match until NUL
	++s1;
	++s2;
    }
    trả_về 0;				    // strings match
}
#endif

#nếu !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    số_nguyên
vim_strnicmp(char *s1, char *s2, size_t len)
{
    số_nguyên		i;

    trong_khi (len > 0)
    {
	i = (số_nguyên)TOLOWER_LOC(*s1) - (số_nguyên)TOLOWER_LOC(*s2);
	nếu (i != 0)
	    trả_về i;			    // this character different
	nếu (*s1 == NUL)
	    break;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    trả_về 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    số_nguyên
vim_strnicmp_asc(char *s1, char *s2, size_t len)
{
    số_nguyên                i = 0;

    trong_khi (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	nếu (i != 0)
	    break;			// this character is different
	nếu (*s1 == NUL)
	    break;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    trả_về i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, số_nguyên c)
{
    char_u	*p;
    số_nguyên		b;

    p = string;
    nếu (enc_utf8 && c > 0 && c < 0x80)
	trả_về vim_strbyte(string, c);
    nếu (enc_utf8 && c >= 0x80)
    {
	trong_khi (*p != NUL)
	{
	    số_nguyên l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    nếu (utf_ptr2char(p) == c && l > 1)
		trả_về p;
	    p += l;
	}
	trả_về NULL;
    }
    nếu (enc_dbcs != 0 && c > 255)
    {
	số_nguyên	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	trong_khi ((b = *p) != NUL)
	{
	    nếu (b == c && p[1] == n2)
		trả_về p;
	    p += (*mb_ptr2len)(p);
	}
	trả_về NULL;
    }
    nếu (has_mbyte)
    {
	trong_khi ((b = *p) != NUL)
	{
	    nếu (b == c)
		trả_về p;
	    p += (*mb_ptr2len)(p);
	}
	trả_về NULL;
    }
    trong_khi ((b = *p) != NUL)
    {
	nếu (b == c)
	    trả_về p;
	++p;
    }
    trả_về NULL;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, số_nguyên c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    nếu (c <= 0 || c > 255)
	trả_về NULL;
    trả_về (char_u *)strchr((char *)string, c);
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
vim_strrchr(char_u *string, số_nguyên c)
{
    char_u	*retval = NULL;
    char_u	*p = string;

    trong_khi (*p)
    {
	nếu (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    trả_về retval;
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
    trong_khi (*s)
    {
	nếu (vim_strchr(charset, *s) != NULL)
	    trả_về s;
	MB_PTR_ADV(s);
    }
    trả_về NULL;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
static số_nguyên sort_compare(const void *s1, const void *s2);

    static số_nguyên
sort_compare(const void *s1, const void *s2)
{
    trả_về STRCMP(*(char **)s1, *(char **)s2);
}

    void
sort_strings(
    char_u	**files,
    số_nguyên		count)
{
    qsort((void *)files, (size_t)count, sizeof(char_u *), sort_compare);
}

#nếu defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    số_nguyên
has_non_ascii(char_u *s)
{
    char_u	*p;

    nếu (s != NULL)
	cho (p = s; *p != NUL; ++p)
	    nếu (*p >= 128)
		trả_về TRUE;
    trả_về FALSE;
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
    nếu (dest == NULL)
	trả_về NULL;
    nếu (str1 == NULL)
	*dest = NUL;
    không_thì
	STRCPY(dest, str1);
    nếu (str2 != NULL)
	STRCPY(dest + l, str2);
    trả_về dest;
}

#nếu defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    nếu (rev == NULL)
	trả_về NULL;

    cho (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	nếu (has_mbyte)
	{
	    số_nguyên mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	không_thì
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    trả_về rev;
}
#endif

#nếu defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, số_nguyên function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    nếu (str != NULL)
    {
	len += (unsigned)STRLEN(str);
	cho (p = str; *p != NUL; MB_PTR_ADV(p))
	    nếu (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    nếu (r == NULL)
	trả_về NULL;

    nếu (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    không_thì
	*r++ = '\'';
    nếu (str != NULL)
	cho (p = str; *p != NUL; )
	{
	    nếu (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    nếu (function)
	*r++ = ')';
    *r++ = NUL;
    trả_về s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_count(char_u *haystack, char_u *needle, số_nguyên ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    nếu (p == NULL || needle == NULL || *needle == NUL)
	trả_về 0;

    size_t  needlelen = STRLEN(needle);
    nếu (ic)
    {
	trong_khi (*p != NUL)
	{
	    nếu (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    không_thì
		MB_PTR_ADV(p);
	}
    }
    không_thì
	trong_khi ((next = (char_u *)strstr((char *)p, (char *)needle)) != NULL)
	{
	    ++n;
	    p = next + needlelen;
	}

    trả_về n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    static số_nguyên
copy_first_char_to_tv(char_u *input, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    số_nguyên		len;

    nếu (input == NULL || output == NULL)
	trả_về -1;

    len = has_mbyte ? mb_ptr2len(input) : 1;
    STRNCPY(buf, input, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    trả_về output->vval.v_string == NULL ? -1 : len;
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
    số_nguyên		len = 0;
    số_nguyên		idx = 0;
    số_nguyên		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    ga_init2(&ga, sizeof(char), 80);
    cho (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	nếu (len < 0)
	    break;

	set_vim_var_nr(VV_KEY, idx);
	nếu (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    break;
	}
	nếu (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    nếu (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		break;
	    }
	    không_thì
		ga_concat(&ga, newtv.vval.v_string);
	}
	không_thì nếu (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    nếu (fc != NULL)
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
    số_nguyên		len;
    typval_T	argv[3];
    số_nguyên		r;
    số_nguyên		called_emsg_start = called_emsg;
    funccall_T	*fc;

    nếu (argvars[2].v_type == VAR_UNKNOWN)
    {
	nếu (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "String");
	    trả_về;
	}
	len = copy_first_char_to_tv(p, rettv);
	nếu (len < 0)
	    trả_về;
	p += len;
    }
    không_thì nếu (check_for_string_arg(argvars, 2) == FAIL)
	trả_về;
    không_thì
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    cho ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	nếu (len < 0)
	{
	    *rettv = argv[0];
	    break;
	}

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	nếu (r == FAIL || called_emsg != called_emsg_start)
	    break;
    }

    nếu (fc != NULL)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    static void
byteidx_common(typval_T *argvars, typval_T *rettv, số_nguyên comp)
{
    rettv->vval.v_number = -1;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	trả_về;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    nếu (str == NULL || idx < 0)
	trả_về;

    varnumber_T	utf16idx = FALSE;
    nếu (argvars[2].v_type != VAR_UNKNOWN)
    {
	số_nguyên error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	nếu (error)
	    trả_về;
	nếu (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), utf16idx);
	    trả_về;
	}
    }

    số_nguyên (*ptr2len)(char_u *);
    nếu (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    không_thì
	ptr2len = mb_ptr2len;

    char_u *t = str;
    cho ( ; idx > 0; idx--)
    {
	nếu (*t == NUL)		// EOL reached
	    trả_về;
	nếu (utf16idx)
	{
	    số_nguyên clen = ptr2len(t);
	    số_nguyên c = (clen > 1) ? utf_ptr2char(t) : *t;
	    nếu (c > 0xFFFF)
		idx--;
	    nếu (idx > 0)
		t += clen;
	}
	không_thì nếu (idx > 0)
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

    nếu (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	trả_về;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    nếu (str == NULL || idx < 0)
	trả_về;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    nếu (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	nếu (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    số_nguyên (*ptr2len)(char_u *);
    nếu (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    không_thì
	ptr2len = mb_ptr2len;

    char_u	*p;
    số_nguyên		len;
    cho (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	nếu (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    nếu (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    trả_về;
	}
	nếu (utf16idx)
	{
	    idx--;
	    số_nguyên clen = ptr2len(p);
	    số_nguyên c = (clen > 1) ? utf_ptr2char(p) : *p;
	    nếu (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_number = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    static số_nguyên
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    nếu (convert_setup(&vimconv, from, to) == FAIL)
	trả_về FAIL;
    vimconv.vc_fail = TRUE;
    nếu (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	nếu (ret->string == NULL)
	    ret->length = 0;
	không_thì
	    ret->length = str->length;
    }
    không_thì
    {
	số_nguyên len = (số_nguyên)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, NULL, NULL);

    trả_về (ret->string == NULL) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    static void
blob_from_string(char_u *str, blob_T *blob)
{
    số_nguyên	    len = (số_nguyên)STRLEN(str);
    char_u  *dest;

    nếu (len == 0)
	trả_về;
    nếu (ga_grow(&blob->bv_ga, len) == FAIL)
	trả_về;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    cho (số_nguyên i = 0; i < len; ++i)
	nếu (dest[i] == NL)
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
    static số_nguyên
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	line_len;

    nếu (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	trả_về (ret->string == NULL) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    line_len = (nl == NULL) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(line_len + 1);
    nếu (ret->string == NULL)
    {
	ret->length = 0;
	trả_về FAIL;
    }
    nếu (line_len > 0)
	mch_memmove(ret->string, src, (size_t)line_len);
    ret->string[line_len] = NUL;
    ret->length = (size_t)line_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    cho (long i = 0; i < line_len; i++)
	nếu (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + line_len + (nl != NULL ? 1 : 0);
    trả_về OK;
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
    nếu (from_encoding_raw == NULL)
	trả_về NULL;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    trong_khi (*s != NUL)
    {
	nếu (*s == '_')
	    *pe++ = '-';
	không_thì
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    nếu ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    trả_về from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    static void
append_converted_string_to_list(
	string_T *converted,
	số_nguyên validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    nếu (converted->string != NULL)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	trong_khi (p < end)
	{
	    string_T	line;
	    char_u	*line_start = p;

	    trong_khi (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    line.length = (size_t)(p - line_start);
	    line.string = vim_strnsave(line_start, line.length);
	    nếu (line.string != NULL)
	    {
		nếu (validate_utf8 && !utf_valid_string(line.string, NULL))
		{
		    vim_free(line.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    trả_về; // Stop processing
		}
		nếu (list_append_string(list, line.string, (số_nguyên)line.length) == FAIL)
		{
		    vim_free(line.string);
		    vim_free(converted->string);
		    trả_về; // Stop processing on append failure
		}
		vim_free(line.string);
	    }
	    không_thì
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		trả_về;
	    }

	    nếu (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    không_thì
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    static số_nguyên
append_validated_line_to_list(string_T *line, số_nguyên validate_utf8, list_T *list)
{
    nếu (validate_utf8 && !utf_valid_string(line->string, NULL))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(line->string);
	trả_về FAIL;
    }

    số_nguyên ret = list_append_string(list, line->string, (số_nguyên)line->length);
    vim_free(line->string);
    trả_về ret;
}

    void
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    số_nguyên		blen;
    long	idx;
    số_nguyên		validate_utf8 = FALSE;

    nếu (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	trả_về;

    nếu (rettv_list_alloc(rettv) == FAIL)
	trả_về;

    blob = argvars->vval.v_blob;
    nếu (blob == NULL)
	trả_về;
    blen = blob_len(blob);

    char_u	*from_encoding = NULL;
    char_u	*from_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	nếu (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    nếu (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		nếu (from_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    trả_về;
		}
	    }
	}
    }

    nếu (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    nếu (from_encoding != NULL && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    số_nguyên from_prop = 0;
    nếu (from_encoding != NULL)
	from_prop = enc_canon_props(from_encoding);
    nếu (from_encoding != NULL && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	số_nguyên nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	nếu (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    nếu (blen > 0)
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
	nếu (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    goto done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	số_nguyên len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, NULL);
	converted.length = len;
	convert_setup(&vimconv, NULL, NULL);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    không_thì
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	trong_khi (idx < blen)
	{
	    string_T	str;

	    nếu (string_from_blob(blob, &idx, &str) != OK)
		break;

	    nếu (from_encoding != NULL)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		số_nguyên	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		nếu (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    nếu (append_validated_line_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		goto done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    nếu (blen > 0 && blob_get(blob, blen - 1) == NL)
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

    nếu (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	trả_về;

    nếu (rettv_blob_alloc(rettv) == FAIL)
	trả_về;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    nếu (list == NULL)
	trả_về;

    char_u	*to_encoding = NULL;
    char_u	*to_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	nếu (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    nếu (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		nếu (to_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    trả_về;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    số_nguyên to_prop = 0;
    nếu (to_encoding != NULL)
	to_prop = enc_canon_props(to_encoding);
    nếu (to_encoding != NULL && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    char_u *s;

	    nếu (li->li_tv.v_type != VAR_STRING)
		continue;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    nếu (li != list->lv_first)
		ga_append(&str_ga, NL);
	    nếu (s != NULL && *s != NUL)
	    {
		số_nguyên slen = (số_nguyên)STRLEN(s);

		nếu (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    goto done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	nếu (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    nếu (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    vimconv.vc_fail = TRUE;

	    số_nguyên		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, NULL);
	    convert_setup(&vimconv, NULL, NULL);
	    ga_clear(&str_ga);

	    nếu (converted == NULL)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    nếu (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	không_thì
	    ga_clear(&str_ga);
    }
    không_thì
    {
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    nếu (li->li_tv.v_type != VAR_STRING)
		continue;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    nếu (str.string == NULL)
		STR_LITERAL_SET(str, "");
	    không_thì
		str.length = STRLEN(str.string);

	    nếu (to_encoding != NULL)
	    {
		số_nguyên	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		nếu (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    nếu (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    nếu (to_encoding != NULL)
		vim_free(str.string);
	}
    }

done:
    nếu (to_encoding != NULL)
	vim_free(to_encoding);
    nếu (to_encoding_raw != NULL)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    void
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    số_nguyên		utf8 = FALSE;

    nếu (rettv_list_alloc(rettv) == FAIL)
	trả_về;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	trả_về;

    nếu (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (số_nguyên)tv_get_bool_chk(&argvars[1], NULL);

    p = tv_get_string(&argvars[0]);

    nếu (has_mbyte || utf8)
    {
	số_nguyên (*ptr2len)(char_u *);
	số_nguyên (*ptr2char)(char_u *);

	nếu (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	không_thì
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	cho ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_number(rettv->vval.v_list, (*ptr2char)(p));
    }
    không_thì
	cho ( ; *p != NUL; ++p)
	    list_append_number(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    void
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    số_nguyên		base = 10;
    char_u	*p;
    varnumber_T	n;
    số_nguyên		what = 0;
    số_nguyên		isneg;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	trả_về;

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (số_nguyên)tv_get_number(&argvars[1]);
	nếu (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    trả_về;
	}
	nếu (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    nếu (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    switch (base)
    {
	case 2: what |= STR2NR_BIN + STR2NR_FORCE; break;
	case 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; break;
	case 16: what |= STR2NR_HEX + STR2NR_FORCE; break;
    }
    vim_str2nr(p, NULL, NULL, what, &n, NULL, 0, FALSE, NULL);
    // Text after the number is silently ignored.
    nếu (isneg)
	rettv->vval.v_number = -n;
    không_thì
	rettv->vval.v_number = n;

}

/*
 * "strgetchar()" function
 */
    void
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    số_nguyên		len;
    số_nguyên		error = FALSE;
    số_nguyên		charidx;
    số_nguyên		byteidx = 0;

    rettv->vval.v_number = -1;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	trả_về;

    str = tv_get_string_chk(&argvars[0]);
    nếu (str == NULL)
	trả_về;
    len = (số_nguyên)STRLEN(str);
    charidx = (số_nguyên)tv_get_number_chk(&argvars[1], &error);
    nếu (error)
	trả_về;

    trong_khi (charidx >= 0 && byteidx < len)
    {
	nếu (charidx == 0)
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
    số_nguyên		start_idx;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	trả_về;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_number = -1;
    nếu (needle == NULL || haystack == NULL)
	trả_về;		// type error; errmsg already given

    nếu (argvars[2].v_type != VAR_UNKNOWN)
    {
	số_nguyên	    error = FALSE;

	start_idx = (số_nguyên)tv_get_number_chk(&argvars[2], &error);
	nếu (error || start_idx >= (số_nguyên)STRLEN(haystack))
	    trả_về;
	nếu (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((char *)haystack, (char *)needle);
    nếu (pos != NULL)
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
    nếu (rettv->vval.v_string != NULL && tofree == NULL)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    void
f_strlen(typval_T *argvars, typval_T *rettv)
{
    nếu (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	trả_về;

    rettv->vval.v_number = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    static void
strchar_common(typval_T *argvars, typval_T *rettv, số_nguyên skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    số_nguyên			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    trong_khi (*s != NUL)
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
    nếu (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	trả_về;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    void
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	trả_về;

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	số_nguyên error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	nếu (error)
	    trả_về;
	nếu (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), skipcc);
	    trả_về;
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

    nếu (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	trả_về;

    varnumber_T countcc = FALSE;
    nếu (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    số_nguyên			(*func_mb_ptr2char_adv)(char_u **pp);
    số_nguyên			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    trong_khi (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	nếu (ch > 0xFFFF)
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
    số_nguyên		col = 0;

    rettv->vval.v_number = -1;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	trả_về;

    s = tv_get_string(&argvars[0]);
    nếu (argvars[1].v_type != VAR_UNKNOWN)
	col = (số_nguyên)tv_get_number(&argvars[1]);

    rettv->vval.v_number = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    void
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

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
    số_nguyên		nchar;
    số_nguyên		nbyte = 0;
    số_nguyên		charlen;
    số_nguyên		skipcc = FALSE;
    số_nguyên		len = 0;
    số_nguyên		slen;
    số_nguyên		error = FALSE;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	trả_về;

    p = tv_get_string(&argvars[0]);
    slen = (số_nguyên)STRLEN(p);

    nchar = (số_nguyên)tv_get_number_chk(&argvars[1], &error);
    nếu (!error)
    {
	nếu (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    nếu (error)
		trả_về;
	    nếu (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_number_as_bool_nr), skipcc);
		trả_về;
	    }
	}

	nếu (nchar > 0)
	    trong_khi (nchar > 0 && nbyte < slen)
	    {
		nếu (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		không_thì
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	không_thì
	    nbyte = nchar;
	nếu (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (số_nguyên)tv_get_number(&argvars[2]);
	    trong_khi (charlen > 0 && nbyte + len < slen)
	    {
		số_nguyên off = nbyte + len;

		nếu (off < 0)
		    len += 1;
		không_thì
		{
		    nếu (skipcc)
			len += mb_ptr2len(p + off);
		    không_thì
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	không_thì
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    nếu (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    không_thì nếu (nbyte > slen)
	nbyte = slen;
    nếu (len < 0)
	len = 0;
    không_thì nếu (nbyte + len > slen)
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
    số_nguyên		n;
    số_nguyên		len;
    số_nguyên		slen;
    số_nguyên		error = FALSE;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	trả_về;

    p = tv_get_string(&argvars[0]);
    slen = (số_nguyên)STRLEN(p);

    n = (số_nguyên)tv_get_number_chk(&argvars[1], &error);
    nếu (error)
	len = 0;
    không_thì nếu (argvars[2].v_type != VAR_UNKNOWN)
	len = (số_nguyên)tv_get_number(&argvars[2]);
    không_thì
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    nếu (n < 0)
    {
	len += n;
	n = 0;
    }
    không_thì nếu (n > slen)
	n = slen;
    nếu (len < 0)
	len = 0;
    không_thì nếu (n + len > slen)
	len = slen - n;

    nếu (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	số_nguyên off;

	// length in characters
	cho (off = n; off < slen && len > 0; --len)
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
    số_nguyên		haystack_len, end_idx;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	trả_về;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_number = -1;
    nếu (needle == NULL || haystack == NULL)
	trả_về;		// type error; errmsg already given

    haystack_len = (số_nguyên)STRLEN(haystack);
    nếu (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (số_nguyên)tv_get_number_chk(&argvars[2], NULL);
	nếu (end_idx < 0)
	    trả_về;	// can never find a match
    }
    không_thì
	end_idx = haystack_len;

    nếu (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    không_thì
    {
	cho (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((char *)rest, (char *)needle);
	    nếu (rest == NULL || rest > haystack + end_idx)
		break;
	    lastmatch = rest;
	}
    }

    nếu (lastmatch == NULL)
	rettv->vval.v_number = -1;
    không_thì
	rettv->vval.v_number = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    void
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

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

    nếu (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_number_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    trả_về;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    nếu (str == NULL || idx < 0)
	trả_về;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    nếu (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	nếu (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    số_nguyên (*ptr2len)(char_u *);
    nếu (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    không_thì
	ptr2len = mb_ptr2len;

    char_u	*p;
    số_nguyên		len;
    số_nguyên		utf16idx = 0;
    cho (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	nếu (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    nếu (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    trả_về;
	}
	utf16idx = len;
	số_nguyên clen = ptr2len(p);
	số_nguyên c = (clen > 1) ? utf_ptr2char(p) : *p;
	nếu (c > 0xFFFF)
	    len++;
	p += clen;
	nếu (charidx)
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
    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    void
f_toupper(typval_T *argvars, typval_T *rettv)
{
    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

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
    số_nguyên		inlen;
    số_nguyên		fromlen;
    số_nguyên		tolen;
    số_nguyên		idx;
    char_u	*cpstr;
    số_nguyên		cplen;
    số_nguyên		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	trả_về;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    nếu (fromstr == NULL || tostr == NULL)
	    trả_về;		// type error; errmsg already given
    ga_init2(&ga, sizeof(char), 80);

    nếu (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	nếu (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    trả_về;
	}

    // fromstr and tostr have to contain the same number of chars
    trong_khi (*in_str != NUL)
    {
	nếu (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    cho (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		nếu (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    cho (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			nếu (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    break;
			}
		    }
		    nếu (*p == NUL)	// tostr is shorter than fromstr
			goto error;
		    break;
		}
		++idx;
	    }

	    nếu (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		cho (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		nếu (idx != 0)
		    goto error;
	    }

	    (void)ga_grow(&ga, cplen);
	    mch_memmove((char *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	không_thì
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    nếu (p != NULL)
		ga_append(&ga, tostr[p - fromstr]);
	    không_thì
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
    số_nguyên		c1;
    số_nguyên		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	trả_về;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    nếu (head == NULL)
	trả_về;

    nếu (check_for_opt_string_arg(argvars, 1) == FAIL)
	trả_về;

    nếu (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	nếu (*mask == NUL)
	    mask = NULL;

	nếu (argvars[2].v_type != VAR_UNKNOWN)
	{
	    số_nguyên	error = 0;

	    // leading or trailing characters to trim
	    dir = (số_nguyên)tv_get_number_chk(&argvars[2], &error);
	    nếu (error)
		trả_về;
	    nếu (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		trả_về;
	    }
	}
    }

    nếu (dir == 0 || dir == 1)
    {
	// Trim leading characters
	trong_khi (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    nếu (mask == NULL)
	    {
		nếu (c1 > ' ' && c1 != 0xa0)
		    break;
	    }
	    không_thì
	    {
		cho (p = mask; *p != NUL; MB_PTR_ADV(p))
		    nếu (c1 == PTR2CHAR(p))
			break;
		nếu (*p == NUL)
		    break;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    nếu (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	cho (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    nếu (mask == NULL)
	    {
		nếu (c1 > ' ' && c1 != 0xa0)
		    break;
	    }
	    không_thì
	    {
		cho (p = mask; *p != NUL; MB_PTR_ADV(p))
		    nếu (c1 == PTR2CHAR(p))
			break;
		nếu (*p == NUL)
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
    nếu (str == NULL)
	trả_về NULL;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    nếu (!decoded)
	trả_về NULL;

    char_u	*p = decoded;
    size_t	i = 0;

    trong_khi (i < len)
    {
	nếu (str[i] == '%')
	{
	    nếu (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		nếu (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    không_thì
	    {
		số_nguyên val = hexhex2nr(&str[i + 1]);
		nếu (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		không_thì
		{
		    // invalid hex digits following "%"
		    cho (số_nguyên j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	không_thì
	    *p++ = str[i++];
    }

    *p = NUL;

    trả_về decoded;
}

/*
 * "uri_decode({str})" function
 */
    void
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    nếu (check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

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
    nếu (str == NULL)
	trả_về NULL;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    nếu (encoded == NULL)
	trả_về NULL;

    char_u *p = encoded;

    cho (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	nếu (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	không_thì
	{
	    sprintf((char *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    trả_về encoded;
}

/*
 * "uri_encode({str})" function
 */
    void
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    nếu (check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

static char *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static varnumber_T
tv_nr(typval_T *tvs, số_nguyên *idxp)
{
    số_nguyên		idx = *idxp - 1;
    varnumber_T	n = 0;
    số_nguyên		err = FALSE;

    nếu (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    không_thì
    {
	++*idxp;
	n = tv_get_number_chk(&tvs[idx], &err);
	nếu (err)
	    n = 0;
    }
    trả_về n;
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
tv_str(typval_T *tvs, số_nguyên *idxp, char_u **tofree)
{
    số_nguyên		    idx = *idxp - 1;
    char	    *s = NULL;
    static char_u   numbuf[NUMBUFLEN];

    nếu (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    không_thì
    {
	++*idxp;
	nếu (tofree != NULL)
	    s = (char *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	không_thì
	    s = (char *)tv_get_string_chk(&tvs[idx]);
    }
    trả_về s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static double
tv_float(typval_T *tvs, số_nguyên *idxp)
{
    số_nguyên		idx = *idxp - 1;
    double	f = 0;

    nếu (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    không_thì
    {
	++*idxp;
	nếu (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	không_thì nếu (tvs[idx].v_type == VAR_NUMBER)
	    f = (double)tvs[idx].vval.v_number;
	không_thì
	    emsg(_(e_expected_float_argument_for_printf));
    }
    trả_về f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    static const char *
infinity_str(số_nguyên positive,
	     char fmt_spec,
	     số_nguyên force_sign,
	     số_nguyên space_for_positive)
{
    static const char *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    số_nguyên idx = positive * (1 + force_sign + force_sign * space_for_positive);

    nếu (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    trả_về table[idx];
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
    số_nguyên
vim_snprintf_add(char *str, size_t str_m, const char *fmt, ...)
{
    va_list	ap;
    số_nguyên		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    nếu (str_m <= len)
	space = 0;
    không_thì
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    trả_về str_l;
}

    số_nguyên
vim_snprintf(char *str, size_t str_m, const char *fmt, ...)
{
    va_list	ap;
    số_nguyên		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    trả_về str_l;
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
    số_nguyên	    str_l;

    nếu (str_m == 0)
	trả_về 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
    va_end(ap);

    nếu (str_l < 0)
    {
	*str = NUL;
	trả_về 0;
    }
    trả_về ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    số_nguyên
vim_vsnprintf(
    char	*str,
    size_t	str_m,
    const char	*fmt,
    va_list	ap)
{
    trả_về vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
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
    static số_nguyên
format_typeof(
    const char	*type)
{
    // allowed values: \0, h, l, L
    char    length_modifier = '\0';

    // current conversion specifier character
    char    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    nếu (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	nếu (length_modifier == 'l' && *type == 'l')
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
	trả_về TYPE_PERCENT;

    case 'c':
	trả_về TYPE_CHAR;

    case 's':
    case 'S':
	trả_về TYPE_STRING;

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

	    nếu (fmt_spec == 'p')
		trả_về TYPE_POINTER;
	    không_thì nếu (fmt_spec == 'b' || fmt_spec == 'B')
		trả_về TYPE_UNSIGNEDLONGLONGINT;
	    không_thì nếu (fmt_spec == 'd')
	    {
		// signed
		switch (length_modifier)
		{
		case '\0':
		case 'h':
		    // char and short arguments are passed as int.
		    trả_về TYPE_INT;
		case 'l':
		    trả_về TYPE_LONGINT;
		case 'L':
		    trả_về TYPE_LONGLONGINT;
		}
	    }
	    không_thì
	    {
		// unsigned
		switch (length_modifier)
		{
		    case '\0':
		    case 'h':
			trả_về TYPE_UNSIGNEDINT;
		    case 'l':
			trả_về TYPE_UNSIGNEDLONGINT;
		    case 'L':
			trả_về TYPE_UNSIGNEDLONGLONGINT;
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
	trả_về TYPE_FLOAT;
    }

    trả_về TYPE_UNKNOWN;
}

    static char *
format_typename(
    const char  *type)
{
    switch (format_typeof(type))
    {
	case TYPE_INT:
	    trả_về typename_int;

	case TYPE_LONGINT:
	    trả_về typename_longint;

	case TYPE_LONGLONGINT:
	    trả_về typename_longlongint;

	case TYPE_UNSIGNEDINT:
	    trả_về typename_unsignedint;

	case TYPE_UNSIGNEDLONGINT:
	    trả_về typename_unsignedlongint;

	case TYPE_UNSIGNEDLONGLONGINT:
	    trả_về typename_unsignedlonglongint;

	case TYPE_POINTER:
	    trả_về _(typename_pointer);

	case TYPE_PERCENT:
	    trả_về _(typename_percent);

	case TYPE_CHAR:
	    trả_về typename_char;

	case TYPE_STRING:
	    trả_về _(typename_string);

	case TYPE_FLOAT:
	    trả_về typename_float;
    }

    trả_về _(typename_unknown);
}

    static số_nguyên
adjust_types(
    const char ***ap_types,
    số_nguyên arg,
    số_nguyên *num_posarg,
    const char *type)
{
    nếu (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	trả_về FAIL;
    }

    nếu (*ap_types == NULL || *num_posarg < arg)
    {
	số_nguyên	    idx;
	const char  **new_types;

	nếu (*ap_types == NULL)
	    new_types = ALLOC_CLEAR_MULT(const char *, arg);
	không_thì
	    new_types = vim_realloc((char **)*ap_types,
						arg * sizeof(const char *));

	nếu (new_types == NULL)
	    trả_về FAIL;

	cho (idx = *num_posarg; idx < arg; ++idx)
	    new_types[idx] = NULL;

	*ap_types = new_types;
	*num_posarg = arg;
    }

    nếu ((*ap_types)[arg - 1] != NULL)
    {
	nếu ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    const char *pt = type;
	    nếu (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    nếu (pt[0] != '*')
	    {
		switch (pt[0])
		{
		    case 'd':
		    case 'i': break;
		    default:
			semsg(_(e_positional_num_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			trả_về FAIL;
		}
	    }
	}
	không_thì
	{
	    nếu (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_num_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		trả_về FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    trả_về OK;
}

    static void
format_overflow_error(const char *pstart)
{
    size_t	arglen = 0;
    char	*argcopy = NULL;
    const char	*p = pstart;

    trong_khi (VIM_ISDIGIT((số_nguyên)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(char, arglen + 1);
    nếu (argcopy != NULL)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	free(argcopy);
    }
    không_thì
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    static số_nguyên
get_unsigned_int(
    const char *pstart,
    const char **p,
    unsigned số_nguyên *uj,
    số_nguyên overflow_err)
{
    *uj = **p - '0';
    ++*p;

    trong_khi (VIM_ISDIGIT((số_nguyên)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned số_nguyên)(**p - '0');
	++*p;
    }

    nếu (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	nếu (overflow_err)
	{
	    format_overflow_error(pstart);
	    trả_về FAIL;
	}
	không_thì
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    trả_về OK;
}


    static số_nguyên
parse_fmt_types(
    const char  ***ap_types,
    số_nguyên		*num_posarg,
    const char  *fmt,
    typval_T	*tvs UNUSED
    )
{
    const char	*p = fmt;
    const char	*arg = NULL;

    số_nguyên		any_pos = 0;
    số_nguyên		any_arg = 0;
    số_nguyên		arg_idx;

# define CHECK_POS_ARG do { \
    nếu (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	goto error; \
    } \
} trong_khi (0);

    nếu (p == NULL)
	trả_về OK;

    trong_khi (*p != NUL)
    {
	nếu (*p != '%')
	{
	    const char    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	không_thì
	{
	    // allowed values: \0, h, l, L
	    char	length_modifier = '\0';

	    // variable for positional arg
	    số_nguyên		pos_arg = -1;
	    const char	*ptype = NULL;
	    const char	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    trong_khi (VIM_ISDIGIT(*ptype))
		++ptype;

	    nếu (*ptype == '$')
	    {
		nếu (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}

		// Positional argument
		unsigned số_nguyên uj;

		nếu (get_unsigned_int(pstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    trong_khi (*p == '0' || *p == '-' || *p == '+' || *p == ' '
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
	    nếu (*(arg = p) == '*')
	    {
		p++;

		nếu (VIM_ISDIGIT((số_nguyên)(*p)))
		{
		    // Positional argument field width
		    unsigned số_nguyên uj;

		    nếu (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    nếu (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		    không_thì
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			nếu (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
			    goto error;
		    }
		}
		không_thì
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    không_thì nếu (VIM_ISDIGIT((số_nguyên)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const char *digstart = p;
		unsigned số_nguyên uj;

		nếu (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		nếu (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}
	    }

	    // parse precision
	    nếu (*p == '.')
	    {
		p++;

		nếu (*(arg = p) == '*')
		{
		    p++;

		    nếu (VIM_ISDIGIT((số_nguyên)(*p)))
		    {
			// Parse precision
			unsigned số_nguyên uj;

			nếu (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			nếu (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    nếu (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
				goto error;
			}
			không_thì
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    goto error;
			}
		    }
		    không_thì
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		không_thì nếu (VIM_ISDIGIT((số_nguyên)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const char *digstart = p;
		    unsigned số_nguyên uj;

		    nếu (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    nếu (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		}
	    }

	    nếu (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    nếu (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		nếu (length_modifier == 'l' && *p == 'l')
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
		    nếu (pos_arg != -1)
		    {
			nếu (adjust_types(ap_types, pos_arg, num_posarg, ptype) == FAIL)
			    goto error;
		    }
		    không_thì
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    break;

		default:
		    nếu (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			goto error;
		    }
	    }

	    nếu (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    cho (arg_idx = 0; arg_idx < *num_posarg; ++arg_idx)
    {
	nếu ((*ap_types)[arg_idx] == NULL)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    goto error;
	}

# nếu defined(FEAT_EVAL)
	nếu (tvs != NULL && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    goto error;
	}
# endif
    }

    trả_về OK;

error:
    vim_free((char**)*ap_types);
    *ap_types = NULL;
    *num_posarg = 0;
    trả_về FAIL;
}

    static void
skip_to_arg(
    const char	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    số_nguyên		*arg_idx,
    số_nguyên		*arg_cur,
    const char	*fmt)
{
    số_nguyên		arg_min = 0;

    nếu (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	trả_về;
    }

    nếu (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    không_thì
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    cho (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	const char *p;

	nếu (ap_types == NULL || ap_types[*arg_cur] == NULL)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    trả_về;
	}

	p = ap_types[*arg_cur];

	số_nguyên fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	switch (fmt_type)
	{
	case TYPE_PERCENT:
	case TYPE_UNKNOWN:
	    break;

	case TYPE_CHAR:
	    va_arg(*ap, số_nguyên);
	    break;

	case TYPE_STRING:
	    va_arg(*ap, char *);
	    break;

	case TYPE_POINTER:
	    va_arg(*ap, void *);
	    break;

	case TYPE_INT:
	    va_arg(*ap, số_nguyên);
	    break;

	case TYPE_LONGINT:
	    va_arg(*ap, long số_nguyên);
	    break;

	case TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    break;

	case TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned số_nguyên);
	    break;

	case TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long số_nguyên);
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

    trả_về;
}

    số_nguyên
vim_vsnprintf_typval(
    char	*str,
    size_t	str_m,
    const char	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    const char	*p = fmt;
    số_nguyên		arg_cur = 0;
    số_nguyên		num_posarg = 0;
    số_nguyên		arg_idx = 1;
    va_list	ap;
    const char	**ap_types = NULL;

    nếu (parse_fmt_types(&ap_types, &num_posarg, fmt, tvs) == FAIL)
	trả_về 0;

    va_copy(ap, ap_start);

    nếu (p == NULL)
	p = "";
    trong_khi (*p != NUL)
    {
	nếu (*p != '%')
	{
	    const char    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    nếu (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	không_thì
	{
	    size_t  min_field_width = 0, precision = 0;
	    số_nguyên	    zero_padding = 0, precision_specified = 0, justify_left = 0;
	    số_nguyên	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    số_nguyên	    space_for_positive = 1;

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
	    số_nguyên	    pos_arg = -1;
	    const char	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    trong_khi (VIM_ISDIGIT(*ptype))
		++ptype;

	    nếu (*ptype == '$')
	    {
		// Positional argument
		const char *digstart = p;
		unsigned số_nguyên uj;

		nếu (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    trong_khi (*p == '0' || *p == '-' || *p == '+' || *p == ' '
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
	    nếu (*p == '*')
	    {
		số_nguyên j;
		const char *digstart = p + 1;

		p++;

		nếu (VIM_ISDIGIT((số_nguyên)(*p)))
		{
		    // Positional argument field width
		    unsigned số_nguyên uj;

		    nếu (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# nếu defined(FEAT_EVAL)
		    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, số_nguyên));

		nếu (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    nếu (tvs != NULL)
		    {
			format_overflow_error(digstart);
			goto error;
		    }
		    không_thì
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		nếu (j >= 0)
		    min_field_width = j;
		không_thì
		{
		    min_field_width = -j;
		    justify_left = 1;
		}
	    }
	    không_thì nếu (VIM_ISDIGIT((số_nguyên)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const char *digstart = p;
		unsigned số_nguyên uj;

		nếu (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		min_field_width = uj;
	    }

	    // parse precision
	    nếu (*p == '.')
	    {
		p++;
		precision_specified = 1;

		nếu (VIM_ISDIGIT((số_nguyên)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const char *digstart = p;
		    unsigned số_nguyên uj;

		    nếu (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    precision = uj;
		}
		không_thì nếu (*p == '*')
		{
		    số_nguyên j;
		    const char *digstart = p;

		    p++;

		    nếu (VIM_ISDIGIT((số_nguyên)(*p)))
		    {
			// positional argument
			unsigned số_nguyên uj;

			nếu (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			arg_idx = uj;

			++p;
		    }

		    j =
# nếu defined(FEAT_EVAL)
			tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, số_nguyên));

		    nếu (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			nếu (tvs != NULL)
			{
			    format_overflow_error(digstart);
			    goto error;
			}
			không_thì
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    nếu (j >= 0)
			precision = j;
		    không_thì
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    nếu (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		nếu (length_modifier == 'l' && *p == 'l')
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

# nếu defined(FEAT_EVAL)
	    switch (fmt_spec)
	    {
		case 'd': case 'u': case 'o': case 'x': case 'X':
		    nếu (tvs != NULL && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    nếu (pos_arg != -1)
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
			số_nguyên j;

			j =
# nếu defined(FEAT_EVAL)
			    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, số_nguyên));

			// standard demands unsigned char
			uchar_arg = (unsigned char)j;
			str_arg = (char *)&uchar_arg;
			break;
		    }

		case 's':
		case 'S':
		    str_arg =
# nếu defined(FEAT_EVAL)
				tvs != NULL ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, char *));

		    nếu (str_arg == NULL)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    không_thì nếu (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    không_thì nếu (precision == 0)
			str_arg_l = 0;
		    không_thì
		    {
			// memchr on HP does not like n > 2^31  !!!
			const char *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == NULL) ? precision
						      : (size_t)(q - str_arg);
		    }
		    nếu (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			số_nguyên	cell;

			cho (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    nếu (precision_specified && i + cell > precision)
				break;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			nếu (min_field_width != 0)
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
		    số_nguyên arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    số_nguyên int_arg = 0;
		    unsigned số_nguyên uint_arg = 0;

		    // only set for length modifier l
		    long số_nguyên long_arg = 0;
		    unsigned long số_nguyên ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    void *ptr_arg = NULL;

		    nếu (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# nếu defined(FEAT_EVAL)
				 tvs != NULL ? (void *)tv_str(tvs, &arg_idx,
									NULL) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, void *));

			nếu (ptr_arg != NULL)
			    arg_sign = 1;
		    }
		    không_thì nếu (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# nếu defined(FEAT_EVAL)
				    tvs != NULL ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			nếu (bin_arg != 0)
			    arg_sign = 1;
		    }
		    không_thì nếu (fmt_spec == 'd')
		    {
			// signed
			switch (length_modifier)
			{
			case '\0':
			case 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# nếu defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, số_nguyên));

			    nếu (int_arg > 0)
				arg_sign =  1;
			    không_thì nếu (int_arg < 0)
				arg_sign = -1;
			    break;
			case 'l':
			    long_arg =
# nếu defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long số_nguyên));

			    nếu (long_arg > 0)
				arg_sign =  1;
			    không_thì nếu (long_arg < 0)
				arg_sign = -1;
			    break;
			case 'L':
			    llong_arg =
# nếu defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    nếu (llong_arg > 0)
				arg_sign =  1;
			    không_thì nếu (llong_arg < 0)
				arg_sign = -1;
			    break;
			}
		    }
		    không_thì
		    {
			// unsigned
			switch (length_modifier)
			{
			    case '\0':
			    case 'h':
				uint_arg =
# nếu defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned số_nguyên));

				nếu (uint_arg != 0)
				    arg_sign = 1;
				break;
			    case 'l':
				ulong_arg =
# nếu defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long số_nguyên));

				nếu (ulong_arg != 0)
				    arg_sign = 1;
				break;
			    case 'L':
				ullong_arg =
# nếu defined(FEAT_EVAL)
					    tvs != NULL ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				nếu (ullong_arg != 0)
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
		    nếu (precision_specified)
			zero_padding = 0;
		    nếu (fmt_spec == 'd')
		    {
			nếu (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    không_thì nếu (alternate_form)
		    {
			nếu (arg_sign != 0
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
		    nếu (!precision_specified)
			precision = 1;   // default precision is 1
		    nếu (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    không_thì
		    {
			char	f[6];
			số_nguyên	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			nếu (!length_modifier)
			    ;
			không_thì nếu (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# không_thì
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			không_thì
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			nếu (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			không_thì nếu (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    char	    b[8 * sizeof(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    do
			    {
				b[sizeof(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    trong_khi (bn != 0);

			    memcpy(tmp + str_arg_l, b + sizeof(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			không_thì nếu (fmt_spec == 'd')
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
			không_thì
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
			nếu (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			nếu (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t num_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			nếu (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    nếu (!precision_specified
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
			nếu (num_of_digits < precision)
			    number_of_zeros_to_pad = precision - num_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    nếu (!justify_left && zero_padding)
		    {
			số_nguyên n = (số_nguyên)(min_field_width - (str_arg_l
						    + number_of_zeros_to_pad));
			nếu (n > 0)
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
		    số_nguyên		l;
		    số_nguyên		remove_trailing_zeroes = FALSE;

		    f =
# nếu defined(FEAT_EVAL)
			tvs != NULL ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, double));

		    abs_f = f < 0 ? -f : f;

		    nếu (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			nếu ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			không_thì
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    nếu ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# không_thì
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
		    không_thì
		    {
			nếu (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			không_thì nếu (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			không_thì
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    nếu (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    nếu (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				nếu ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				nếu (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (số_nguyên)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			nếu (remove_trailing_zeroes)
			{
			    số_nguyên i;
			    char *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    nếu (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    không_thì
			    {
				tp = (char *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				nếu (tp != NULL)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    nếu (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    trong_khi (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    nếu (tp != NULL && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				trong_khi (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			không_thì
			{
			    char *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (char *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    nếu (tp != NULL && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    nếu (zero_padding && min_field_width > str_arg_l
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
		nếu (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		break;
	    }

	    nếu (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    nếu (!justify_left)
	    {
		// left padding with blank or zero
		số_nguyên pn = (số_nguyên)(min_field_width - (str_arg_l + number_of_zeros_to_pad));

		nếu (pn > 0)
		{
		    nếu (str_l < str_m)
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
	    nếu (number_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    không_thì
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		số_nguyên zn = (số_nguyên)zero_padding_insertion_ind;

		nếu (zn > 0)
		{
		    nếu (str_l < str_m)
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
		zn = (số_nguyên)number_of_zeros_to_pad;
		nếu (zn > 0)
		{
		    nếu (str_l < str_m)
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
		số_nguyên sn = (số_nguyên)(str_arg_l - zero_padding_insertion_ind);

		nếu (sn > 0)
		{
		    nếu (str_l < str_m)
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
	    nếu (justify_left)
	    {
		// right blank padding to the field width
		số_nguyên pn = (số_nguyên)(min_field_width
				      - (str_arg_l + number_of_zeros_to_pad));

		nếu (pn > 0)
		{
		    nếu (str_l < str_m)
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

    nếu (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    nếu (tvs != NULL && tvs[num_posarg != 0 ? num_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((char*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    trả_về (số_nguyên)str_l;
}

#endif // PROTO
