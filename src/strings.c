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
    ако (p != NULL)
	mch_memmove(p, string, len);
    върни p;
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
    ако (p == NULL)
	върни NULL;

    STRNCPY(p, string, len);
    p[len] = NUL;
    върни p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    върни vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    цяло		cc,
    цяло		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    цяло		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    за (p = string; *p; p++)
    {
	ако (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    continue;
	}
	ако (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    ако (escaped_string == NULL)
	върни NULL;
    p2 = escaped_string;
    за (p = string; *p; p++)
    {
	ако (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    continue;
	}
	ако (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    върни escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    цяло
csh_like_shell(void)
{
    върни (strstr((char *)gettail(p_sh), "csh") != NULL);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    static цяло
fish_like_shell(void)
{
    върни (strstr((char *)gettail(p_sh), "fish") != NULL);
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
vim_strsave_shellescape(char_u *string, цяло do_special, цяло do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    цяло		csh_like;
    цяло		fish_like;
    char_u	*shname;
    цяло		powershell;
#ifdef MSWIN
    цяло		double_quotes;
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
    за (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	ако (double_quotes)
	{
	    ако (*p == '"')
		++length;		// " -> ""
	}
	иначе
#endif
	ако (*p == '\'')
	{
	    ако (powershell)
		length += 2;		// ' => ''
	    иначе
		length += 3;		// ' => '\''
	}
	ако ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    ако (csh_like && do_special)
		++length;		// insert backslash
	}
	ако (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	ако (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    ако (escaped_string != NULL)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	ако (double_quotes)
	    *d++ = '"';
	иначе
#endif
	    *d++ = '\'';

	за (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    ако (double_quotes)
	    {
		ако (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    continue;
		}
	    }
	    иначе
#endif
	    ако (*p == '\'')
	    {
		ако (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		иначе
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		continue;
	    }
	    ако ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		ако (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		continue;
	    }
	    ако (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		continue;
	    }
	    ако (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		continue;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	ако (double_quotes)
	    *d++ = '"';
	иначе
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    върни escaped_string;
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
    върни p1;
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
    върни p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    void
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    цяло	    c;

    ако (p == NULL)
	върни;

    p2 = p;
    докато ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#ако defined(FEAT_EVAL) || defined(FEAT_SPELL)
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

    ако (res != NULL)
	докато (*p != NUL)
	{
	    цяло		l;

	    ако (enc_utf8)
	    {
		цяло	c, uc;
		цяло	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ако (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		ако (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ако (s == NULL)
		    {
			vim_free(res);
			върни NULL;
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
	    иначе ако (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    иначе
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    върни res;
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

    ако (res != NULL)
	докато (*p != NUL)
	{
	    цяло		l;

	    ако (enc_utf8)
	    {
		цяло	c, lc;
		цяло	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ако (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		ако (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ако (s == NULL)
		    {
			vim_free(res);
			върни NULL;
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
	    иначе ако (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    иначе
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    върни res;
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
    докато (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
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

    ако (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    иначе
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(char *s, size_t maxlen)
{
    size_t i;
    за (i = 0; i < maxlen; ++i)
	ако (s[i] == NUL)
	    break;
    върни i;
}

#ако !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    цяло
vim_stricmp(char *s1, char *s2)
{
    цяло		i;

    за (;;)
    {
	i = (цяло)TOLOWER_LOC(*s1) - (цяло)TOLOWER_LOC(*s2);
	ако (i != 0)
	    върни i;			    // this character different
	ако (*s1 == NUL)
	    break;			    // strings match until NUL
	++s1;
	++s2;
    }
    върни 0;				    // strings match
}
#endif

#ако !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    цяло
vim_strnicmp(char *s1, char *s2, size_t len)
{
    цяло		i;

    докато (len > 0)
    {
	i = (цяло)TOLOWER_LOC(*s1) - (цяло)TOLOWER_LOC(*s2);
	ако (i != 0)
	    върни i;			    // this character different
	ако (*s1 == NUL)
	    break;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    върни 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    цяло
vim_strnicmp_asc(char *s1, char *s2, size_t len)
{
    цяло                i = 0;

    докато (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	ако (i != 0)
	    break;			// this character is different
	ако (*s1 == NUL)
	    break;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    върни i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, цяло c)
{
    char_u	*p;
    цяло		b;

    p = string;
    ако (enc_utf8 && c > 0 && c < 0x80)
	върни vim_strbyte(string, c);
    ако (enc_utf8 && c >= 0x80)
    {
	докато (*p != NUL)
	{
	    цяло l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    ако (utf_ptr2char(p) == c && l > 1)
		върни p;
	    p += l;
	}
	върни NULL;
    }
    ако (enc_dbcs != 0 && c > 255)
    {
	цяло	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	докато ((b = *p) != NUL)
	{
	    ако (b == c && p[1] == n2)
		върни p;
	    p += (*mb_ptr2len)(p);
	}
	върни NULL;
    }
    ако (has_mbyte)
    {
	докато ((b = *p) != NUL)
	{
	    ако (b == c)
		върни p;
	    p += (*mb_ptr2len)(p);
	}
	върни NULL;
    }
    докато ((b = *p) != NUL)
    {
	ако (b == c)
	    върни p;
	++p;
    }
    върни NULL;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, цяло c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    ако (c <= 0 || c > 255)
	върни NULL;
    върни (char_u *)strchr((char *)string, c);
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
vim_strrchr(char_u *string, цяло c)
{
    char_u	*retval = NULL;
    char_u	*p = string;

    докато (*p)
    {
	ако (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    върни retval;
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
    докато (*s)
    {
	ако (vim_strchr(charset, *s) != NULL)
	    върни s;
	MB_PTR_ADV(s);
    }
    върни NULL;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
static цяло sort_compare(const void *s1, const void *s2);

    static цяло
sort_compare(const void *s1, const void *s2)
{
    върни STRCMP(*(char **)s1, *(char **)s2);
}

    void
sort_strings(
    char_u	**files,
    цяло		count)
{
    qsort((void *)files, (size_t)count, sizeof(char_u *), sort_compare);
}

#ако defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    цяло
has_non_ascii(char_u *s)
{
    char_u	*p;

    ако (s != NULL)
	за (p = s; *p != NUL; ++p)
	    ако (*p >= 128)
		върни TRUE;
    върни FALSE;
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
    ако (dest == NULL)
	върни NULL;
    ако (str1 == NULL)
	*dest = NUL;
    иначе
	STRCPY(dest, str1);
    ако (str2 != NULL)
	STRCPY(dest + l, str2);
    върни dest;
}

#ако defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    ако (rev == NULL)
	върни NULL;

    за (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	ако (has_mbyte)
	{
	    цяло mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	иначе
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    върни rev;
}
#endif

#ако defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, цяло function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    ако (str != NULL)
    {
	len += (unsigned)STRLEN(str);
	за (p = str; *p != NUL; MB_PTR_ADV(p))
	    ако (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    ако (r == NULL)
	върни NULL;

    ако (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    иначе
	*r++ = '\'';
    ако (str != NULL)
	за (p = str; *p != NUL; )
	{
	    ако (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    ако (function)
	*r++ = ')';
    *r++ = NUL;
    върни s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_count(char_u *haystack, char_u *needle, цяло ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    ако (p == NULL || needle == NULL || *needle == NUL)
	върни 0;

    size_t  needlelen = STRLEN(needle);
    ако (ic)
    {
	докато (*p != NUL)
	{
	    ако (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    иначе
		MB_PTR_ADV(p);
	}
    }
    иначе
	докато ((next = (char_u *)strstr((char *)p, (char *)needle)) != NULL)
	{
	    ++n;
	    p = next + needlelen;
	}

    върни n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    static цяло
copy_first_char_to_tv(char_u *input, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    цяло		len;

    ако (input == NULL || output == NULL)
	върни -1;

    len = has_mbyte ? mb_ptr2len(input) : 1;
    STRNCPY(buf, input, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    върни output->vval.v_string == NULL ? -1 : len;
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
    цяло		len = 0;
    цяло		idx = 0;
    цяло		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    ga_init2(&ga, sizeof(char), 80);
    за (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	ако (len < 0)
	    break;

	set_vim_var_nr(VV_KEY, idx);
	ако (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    break;
	}
	ако (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    ако (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		break;
	    }
	    иначе
		ga_concat(&ga, newtv.vval.v_string);
	}
	иначе ако (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    ако (fc != NULL)
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
    цяло		len;
    typval_T	argv[3];
    цяло		r;
    цяло		called_emsg_start = called_emsg;
    funccall_T	*fc;

    ако (argvars[2].v_type == VAR_UNKNOWN)
    {
	ако (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "String");
	    върни;
	}
	len = copy_first_char_to_tv(p, rettv);
	ако (len < 0)
	    върни;
	p += len;
    }
    иначе ако (check_for_string_arg(argvars, 2) == FAIL)
	върни;
    иначе
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    за ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	ако (len < 0)
	{
	    *rettv = argv[0];
	    break;
	}

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	ако (r == FAIL || called_emsg != called_emsg_start)
	    break;
    }

    ако (fc != NULL)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    static void
byteidx_common(typval_T *argvars, typval_T *rettv, цяло comp)
{
    rettv->vval.v_number = -1;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	върни;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    ако (str == NULL || idx < 0)
	върни;

    varnumber_T	utf16idx = FALSE;
    ако (argvars[2].v_type != VAR_UNKNOWN)
    {
	цяло error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	ако (error)
	    върни;
	ако (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), utf16idx);
	    върни;
	}
    }

    цяло (*ptr2len)(char_u *);
    ако (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    иначе
	ptr2len = mb_ptr2len;

    char_u *t = str;
    за ( ; idx > 0; idx--)
    {
	ако (*t == NUL)		// EOL reached
	    върни;
	ако (utf16idx)
	{
	    цяло clen = ptr2len(t);
	    цяло c = (clen > 1) ? utf_ptr2char(t) : *t;
	    ако (c > 0xFFFF)
		idx--;
	    ако (idx > 0)
		t += clen;
	}
	иначе ако (idx > 0)
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

    ако (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	върни;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    ако (str == NULL || idx < 0)
	върни;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    ако (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ако (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    цяло (*ptr2len)(char_u *);
    ако (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    иначе
	ptr2len = mb_ptr2len;

    char_u	*p;
    цяло		len;
    за (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	ако (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    ако (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    върни;
	}
	ако (utf16idx)
	{
	    idx--;
	    цяло clen = ptr2len(p);
	    цяло c = (clen > 1) ? utf_ptr2char(p) : *p;
	    ако (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_number = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    static цяло
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ако (convert_setup(&vimconv, from, to) == FAIL)
	върни FAIL;
    vimconv.vc_fail = TRUE;
    ако (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	ако (ret->string == NULL)
	    ret->length = 0;
	иначе
	    ret->length = str->length;
    }
    иначе
    {
	цяло len = (цяло)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, NULL, NULL);

    върни (ret->string == NULL) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    static void
blob_from_string(char_u *str, blob_T *blob)
{
    цяло	    len = (цяло)STRLEN(str);
    char_u  *dest;

    ако (len == 0)
	върни;
    ако (ga_grow(&blob->bv_ga, len) == FAIL)
	върни;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    за (цяло i = 0; i < len; ++i)
	ако (dest[i] == NL)
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
    static цяло
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	line_len;

    ако (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	върни (ret->string == NULL) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    line_len = (nl == NULL) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(line_len + 1);
    ако (ret->string == NULL)
    {
	ret->length = 0;
	върни FAIL;
    }
    ако (line_len > 0)
	mch_memmove(ret->string, src, (size_t)line_len);
    ret->string[line_len] = NUL;
    ret->length = (size_t)line_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    за (long i = 0; i < line_len; i++)
	ако (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + line_len + (nl != NULL ? 1 : 0);
    върни OK;
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
    ако (from_encoding_raw == NULL)
	върни NULL;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    докато (*s != NUL)
    {
	ако (*s == '_')
	    *pe++ = '-';
	иначе
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    ако ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    върни from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    static void
append_converted_string_to_list(
	string_T *converted,
	цяло validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    ако (converted->string != NULL)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	докато (p < end)
	{
	    string_T	line;
	    char_u	*line_start = p;

	    докато (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    line.length = (size_t)(p - line_start);
	    line.string = vim_strnsave(line_start, line.length);
	    ако (line.string != NULL)
	    {
		ако (validate_utf8 && !utf_valid_string(line.string, NULL))
		{
		    vim_free(line.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    върни; // Stop processing
		}
		ако (list_append_string(list, line.string, (цяло)line.length) == FAIL)
		{
		    vim_free(line.string);
		    vim_free(converted->string);
		    върни; // Stop processing on append failure
		}
		vim_free(line.string);
	    }
	    иначе
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		върни;
	    }

	    ако (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    иначе
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    static цяло
append_validated_line_to_list(string_T *line, цяло validate_utf8, list_T *list)
{
    ако (validate_utf8 && !utf_valid_string(line->string, NULL))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(line->string);
	върни FAIL;
    }

    цяло ret = list_append_string(list, line->string, (цяло)line->length);
    vim_free(line->string);
    върни ret;
}

    void
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    цяло		blen;
    long	idx;
    цяло		validate_utf8 = FALSE;

    ако (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	върни;

    ако (rettv_list_alloc(rettv) == FAIL)
	върни;

    blob = argvars->vval.v_blob;
    ако (blob == NULL)
	върни;
    blen = blob_len(blob);

    char_u	*from_encoding = NULL;
    char_u	*from_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    ако (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ако (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ако (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		ако (from_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    върни;
		}
	    }
	}
    }

    ако (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    ако (from_encoding != NULL && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    цяло from_prop = 0;
    ако (from_encoding != NULL)
	from_prop = enc_canon_props(from_encoding);
    ако (from_encoding != NULL && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	цяло nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	ако (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    ако (blen > 0)
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
	ако (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    goto done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	цяло len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, NULL);
	converted.length = len;
	convert_setup(&vimconv, NULL, NULL);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    иначе
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	докато (idx < blen)
	{
	    string_T	str;

	    ако (string_from_blob(blob, &idx, &str) != OK)
		break;

	    ако (from_encoding != NULL)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		цяло	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		ако (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    ако (append_validated_line_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		goto done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    ако (blen > 0 && blob_get(blob, blen - 1) == NL)
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

    ако (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	върни;

    ако (rettv_blob_alloc(rettv) == FAIL)
	върни;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    ако (list == NULL)
	върни;

    char_u	*to_encoding = NULL;
    char_u	*to_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    ако (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ако (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ако (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		ако (to_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    върни;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    цяло to_prop = 0;
    ако (to_encoding != NULL)
	to_prop = enc_canon_props(to_encoding);
    ако (to_encoding != NULL && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    char_u *s;

	    ако (li->li_tv.v_type != VAR_STRING)
		continue;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    ако (li != list->lv_first)
		ga_append(&str_ga, NL);
	    ако (s != NULL && *s != NUL)
	    {
		цяло slen = (цяло)STRLEN(s);

		ако (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    goto done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	ако (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    ако (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    vimconv.vc_fail = TRUE;

	    цяло		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, NULL);
	    convert_setup(&vimconv, NULL, NULL);
	    ga_clear(&str_ga);

	    ако (converted == NULL)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    ако (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	иначе
	    ga_clear(&str_ga);
    }
    иначе
    {
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    ако (li->li_tv.v_type != VAR_STRING)
		continue;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    ако (str.string == NULL)
		STR_LITERAL_SET(str, "");
	    иначе
		str.length = STRLEN(str.string);

	    ако (to_encoding != NULL)
	    {
		цяло	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		ако (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    ако (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    ако (to_encoding != NULL)
		vim_free(str.string);
	}
    }

done:
    ако (to_encoding != NULL)
	vim_free(to_encoding);
    ако (to_encoding_raw != NULL)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    void
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    цяло		utf8 = FALSE;

    ако (rettv_list_alloc(rettv) == FAIL)
	върни;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	върни;

    ако (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (цяло)tv_get_bool_chk(&argvars[1], NULL);

    p = tv_get_string(&argvars[0]);

    ако (has_mbyte || utf8)
    {
	цяло (*ptr2len)(char_u *);
	цяло (*ptr2char)(char_u *);

	ако (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	иначе
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	за ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_number(rettv->vval.v_list, (*ptr2char)(p));
    }
    иначе
	за ( ; *p != NUL; ++p)
	    list_append_number(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    void
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    цяло		base = 10;
    char_u	*p;
    varnumber_T	n;
    цяло		what = 0;
    цяло		isneg;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	върни;

    ако (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (цяло)tv_get_number(&argvars[1]);
	ако (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    върни;
	}
	ако (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    ако (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    switch (base)
    {
	case 2: what |= STR2NR_BIN + STR2NR_FORCE; break;
	case 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; break;
	case 16: what |= STR2NR_HEX + STR2NR_FORCE; break;
    }
    vim_str2nr(p, NULL, NULL, what, &n, NULL, 0, FALSE, NULL);
    // Text after the number is silently ignored.
    ако (isneg)
	rettv->vval.v_number = -n;
    иначе
	rettv->vval.v_number = n;

}

/*
 * "strgetchar()" function
 */
    void
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    цяло		len;
    цяло		error = FALSE;
    цяло		charidx;
    цяло		byteidx = 0;

    rettv->vval.v_number = -1;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	върни;

    str = tv_get_string_chk(&argvars[0]);
    ако (str == NULL)
	върни;
    len = (цяло)STRLEN(str);
    charidx = (цяло)tv_get_number_chk(&argvars[1], &error);
    ако (error)
	върни;

    докато (charidx >= 0 && byteidx < len)
    {
	ако (charidx == 0)
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
    цяло		start_idx;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	върни;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_number = -1;
    ако (needle == NULL || haystack == NULL)
	върни;		// type error; errmsg already given

    ако (argvars[2].v_type != VAR_UNKNOWN)
    {
	цяло	    error = FALSE;

	start_idx = (цяло)tv_get_number_chk(&argvars[2], &error);
	ако (error || start_idx >= (цяло)STRLEN(haystack))
	    върни;
	ако (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((char *)haystack, (char *)needle);
    ако (pos != NULL)
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
    ако (rettv->vval.v_string != NULL && tofree == NULL)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    void
f_strlen(typval_T *argvars, typval_T *rettv)
{
    ако (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	върни;

    rettv->vval.v_number = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    static void
strchar_common(typval_T *argvars, typval_T *rettv, цяло skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    цяло			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    докато (*s != NUL)
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
    ако (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	върни;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    void
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	върни;

    ако (argvars[1].v_type != VAR_UNKNOWN)
    {
	цяло error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	ако (error)
	    върни;
	ако (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), skipcc);
	    върни;
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

    ако (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	върни;

    varnumber_T countcc = FALSE;
    ако (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    цяло			(*func_mb_ptr2char_adv)(char_u **pp);
    цяло			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    докато (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	ако (ch > 0xFFFF)
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
    цяло		col = 0;

    rettv->vval.v_number = -1;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	върни;

    s = tv_get_string(&argvars[0]);
    ако (argvars[1].v_type != VAR_UNKNOWN)
	col = (цяло)tv_get_number(&argvars[1]);

    rettv->vval.v_number = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    void
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    ако (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	върни;

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
    цяло		nchar;
    цяло		nbyte = 0;
    цяло		charlen;
    цяло		skipcc = FALSE;
    цяло		len = 0;
    цяло		slen;
    цяло		error = FALSE;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	върни;

    p = tv_get_string(&argvars[0]);
    slen = (цяло)STRLEN(p);

    nchar = (цяло)tv_get_number_chk(&argvars[1], &error);
    ако (!error)
    {
	ако (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    ако (error)
		върни;
	    ако (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_number_as_bool_nr), skipcc);
		върни;
	    }
	}

	ако (nchar > 0)
	    докато (nchar > 0 && nbyte < slen)
	    {
		ако (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		иначе
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	иначе
	    nbyte = nchar;
	ако (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (цяло)tv_get_number(&argvars[2]);
	    докато (charlen > 0 && nbyte + len < slen)
	    {
		цяло off = nbyte + len;

		ако (off < 0)
		    len += 1;
		иначе
		{
		    ако (skipcc)
			len += mb_ptr2len(p + off);
		    иначе
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	иначе
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    ако (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    иначе ако (nbyte > slen)
	nbyte = slen;
    ако (len < 0)
	len = 0;
    иначе ако (nbyte + len > slen)
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
    цяло		n;
    цяло		len;
    цяло		slen;
    цяло		error = FALSE;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	върни;

    p = tv_get_string(&argvars[0]);
    slen = (цяло)STRLEN(p);

    n = (цяло)tv_get_number_chk(&argvars[1], &error);
    ако (error)
	len = 0;
    иначе ако (argvars[2].v_type != VAR_UNKNOWN)
	len = (цяло)tv_get_number(&argvars[2]);
    иначе
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    ако (n < 0)
    {
	len += n;
	n = 0;
    }
    иначе ако (n > slen)
	n = slen;
    ако (len < 0)
	len = 0;
    иначе ако (n + len > slen)
	len = slen - n;

    ако (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	цяло off;

	// length in characters
	за (off = n; off < slen && len > 0; --len)
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
    цяло		haystack_len, end_idx;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	върни;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_number = -1;
    ако (needle == NULL || haystack == NULL)
	върни;		// type error; errmsg already given

    haystack_len = (цяло)STRLEN(haystack);
    ако (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (цяло)tv_get_number_chk(&argvars[2], NULL);
	ако (end_idx < 0)
	    върни;	// can never find a match
    }
    иначе
	end_idx = haystack_len;

    ако (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    иначе
    {
	за (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((char *)rest, (char *)needle);
	    ако (rest == NULL || rest > haystack + end_idx)
		break;
	    lastmatch = rest;
	}
    }

    ако (lastmatch == NULL)
	rettv->vval.v_number = -1;
    иначе
	rettv->vval.v_number = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    void
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    ако (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	върни;

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

    ако (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_number_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    върни;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    ако (str == NULL || idx < 0)
	върни;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    ако (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ако (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    цяло (*ptr2len)(char_u *);
    ако (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    иначе
	ptr2len = mb_ptr2len;

    char_u	*p;
    цяло		len;
    цяло		utf16idx = 0;
    за (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	ако (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    ако (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    върни;
	}
	utf16idx = len;
	цяло clen = ptr2len(p);
	цяло c = (clen > 1) ? utf_ptr2char(p) : *p;
	ако (c > 0xFFFF)
	    len++;
	p += clen;
	ако (charidx)
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
    ако (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	върни;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    void
f_toupper(typval_T *argvars, typval_T *rettv)
{
    ако (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	върни;

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
    цяло		inlen;
    цяло		fromlen;
    цяло		tolen;
    цяло		idx;
    char_u	*cpstr;
    цяло		cplen;
    цяло		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	върни;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    ако (fromstr == NULL || tostr == NULL)
	    върни;		// type error; errmsg already given
    ga_init2(&ga, sizeof(char), 80);

    ако (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	ако (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    върни;
	}

    // fromstr and tostr have to contain the same number of chars
    докато (*in_str != NUL)
    {
	ако (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    за (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		ако (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    за (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			ако (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    break;
			}
		    }
		    ако (*p == NUL)	// tostr is shorter than fromstr
			goto error;
		    break;
		}
		++idx;
	    }

	    ако (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		за (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		ако (idx != 0)
		    goto error;
	    }

	    (void)ga_grow(&ga, cplen);
	    mch_memmove((char *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	иначе
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    ако (p != NULL)
		ga_append(&ga, tostr[p - fromstr]);
	    иначе
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
    цяло		c1;
    цяло		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	върни;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    ако (head == NULL)
	върни;

    ако (check_for_opt_string_arg(argvars, 1) == FAIL)
	върни;

    ако (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	ако (*mask == NUL)
	    mask = NULL;

	ако (argvars[2].v_type != VAR_UNKNOWN)
	{
	    цяло	error = 0;

	    // leading or trailing characters to trim
	    dir = (цяло)tv_get_number_chk(&argvars[2], &error);
	    ако (error)
		върни;
	    ако (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		върни;
	    }
	}
    }

    ако (dir == 0 || dir == 1)
    {
	// Trim leading characters
	докато (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    ако (mask == NULL)
	    {
		ако (c1 > ' ' && c1 != 0xa0)
		    break;
	    }
	    иначе
	    {
		за (p = mask; *p != NUL; MB_PTR_ADV(p))
		    ако (c1 == PTR2CHAR(p))
			break;
		ако (*p == NUL)
		    break;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    ако (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	за (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    ако (mask == NULL)
	    {
		ако (c1 > ' ' && c1 != 0xa0)
		    break;
	    }
	    иначе
	    {
		за (p = mask; *p != NUL; MB_PTR_ADV(p))
		    ако (c1 == PTR2CHAR(p))
			break;
		ако (*p == NUL)
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
    ако (str == NULL)
	върни NULL;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    ако (!decoded)
	върни NULL;

    char_u	*p = decoded;
    size_t	i = 0;

    докато (i < len)
    {
	ако (str[i] == '%')
	{
	    ако (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		ако (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    иначе
	    {
		цяло val = hexhex2nr(&str[i + 1]);
		ако (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		иначе
		{
		    // invalid hex digits following "%"
		    за (цяло j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	иначе
	    *p++ = str[i++];
    }

    *p = NUL;

    върни decoded;
}

/*
 * "uri_decode({str})" function
 */
    void
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ако (check_for_string_arg(argvars, 0) == FAIL)
	върни;

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
    ако (str == NULL)
	върни NULL;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    ако (encoded == NULL)
	върни NULL;

    char_u *p = encoded;

    за (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	ако (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	иначе
	{
	    sprintf((char *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    върни encoded;
}

/*
 * "uri_encode({str})" function
 */
    void
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ако (check_for_string_arg(argvars, 0) == FAIL)
	върни;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

static char *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static varnumber_T
tv_nr(typval_T *tvs, цяло *idxp)
{
    цяло		idx = *idxp - 1;
    varnumber_T	n = 0;
    цяло		err = FALSE;

    ако (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    иначе
    {
	++*idxp;
	n = tv_get_number_chk(&tvs[idx], &err);
	ако (err)
	    n = 0;
    }
    върни n;
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
tv_str(typval_T *tvs, цяло *idxp, char_u **tofree)
{
    цяло		    idx = *idxp - 1;
    char	    *s = NULL;
    static char_u   numbuf[NUMBUFLEN];

    ако (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    иначе
    {
	++*idxp;
	ако (tofree != NULL)
	    s = (char *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	иначе
	    s = (char *)tv_get_string_chk(&tvs[idx]);
    }
    върни s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static double
tv_float(typval_T *tvs, цяло *idxp)
{
    цяло		idx = *idxp - 1;
    double	f = 0;

    ако (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    иначе
    {
	++*idxp;
	ако (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	иначе ако (tvs[idx].v_type == VAR_NUMBER)
	    f = (double)tvs[idx].vval.v_number;
	иначе
	    emsg(_(e_expected_float_argument_for_printf));
    }
    върни f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    static const char *
infinity_str(цяло positive,
	     char fmt_spec,
	     цяло force_sign,
	     цяло space_for_positive)
{
    static const char *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    цяло idx = positive * (1 + force_sign + force_sign * space_for_positive);

    ако (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    върни table[idx];
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
    цяло
vim_snprintf_add(char *str, size_t str_m, const char *fmt, ...)
{
    va_list	ap;
    цяло		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    ако (str_m <= len)
	space = 0;
    иначе
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    върни str_l;
}

    цяло
vim_snprintf(char *str, size_t str_m, const char *fmt, ...)
{
    va_list	ap;
    цяло		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    върни str_l;
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
    цяло	    str_l;

    ако (str_m == 0)
	върни 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
    va_end(ap);

    ако (str_l < 0)
    {
	*str = NUL;
	върни 0;
    }
    върни ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    цяло
vim_vsnprintf(
    char	*str,
    size_t	str_m,
    const char	*fmt,
    va_list	ap)
{
    върни vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
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
    static цяло
format_typeof(
    const char	*type)
{
    // allowed values: \0, h, l, L
    char    length_modifier = '\0';

    // current conversion specifier character
    char    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    ако (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	ако (length_modifier == 'l' && *type == 'l')
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
	върни TYPE_PERCENT;

    case 'c':
	върни TYPE_CHAR;

    case 's':
    case 'S':
	върни TYPE_STRING;

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

	    ако (fmt_spec == 'p')
		върни TYPE_POINTER;
	    иначе ако (fmt_spec == 'b' || fmt_spec == 'B')
		върни TYPE_UNSIGNEDLONGLONGINT;
	    иначе ако (fmt_spec == 'd')
	    {
		// signed
		switch (length_modifier)
		{
		case '\0':
		case 'h':
		    // char and short arguments are passed as int.
		    върни TYPE_INT;
		case 'l':
		    върни TYPE_LONGINT;
		case 'L':
		    върни TYPE_LONGLONGINT;
		}
	    }
	    иначе
	    {
		// unsigned
		switch (length_modifier)
		{
		    case '\0':
		    case 'h':
			върни TYPE_UNSIGNEDINT;
		    case 'l':
			върни TYPE_UNSIGNEDLONGINT;
		    case 'L':
			върни TYPE_UNSIGNEDLONGLONGINT;
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
	върни TYPE_FLOAT;
    }

    върни TYPE_UNKNOWN;
}

    static char *
format_typename(
    const char  *type)
{
    switch (format_typeof(type))
    {
	case TYPE_INT:
	    върни typename_int;

	case TYPE_LONGINT:
	    върни typename_longint;

	case TYPE_LONGLONGINT:
	    върни typename_longlongint;

	case TYPE_UNSIGNEDINT:
	    върни typename_unsignedint;

	case TYPE_UNSIGNEDLONGINT:
	    върни typename_unsignedlongint;

	case TYPE_UNSIGNEDLONGLONGINT:
	    върни typename_unsignedlonglongint;

	case TYPE_POINTER:
	    върни _(typename_pointer);

	case TYPE_PERCENT:
	    върни _(typename_percent);

	case TYPE_CHAR:
	    върни typename_char;

	case TYPE_STRING:
	    върни _(typename_string);

	case TYPE_FLOAT:
	    върни typename_float;
    }

    върни _(typename_unknown);
}

    static цяло
adjust_types(
    const char ***ap_types,
    цяло arg,
    цяло *num_posarg,
    const char *type)
{
    ако (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	върни FAIL;
    }

    ако (*ap_types == NULL || *num_posarg < arg)
    {
	цяло	    idx;
	const char  **new_types;

	ако (*ap_types == NULL)
	    new_types = ALLOC_CLEAR_MULT(const char *, arg);
	иначе
	    new_types = vim_realloc((char **)*ap_types,
						arg * sizeof(const char *));

	ако (new_types == NULL)
	    върни FAIL;

	за (idx = *num_posarg; idx < arg; ++idx)
	    new_types[idx] = NULL;

	*ap_types = new_types;
	*num_posarg = arg;
    }

    ако ((*ap_types)[arg - 1] != NULL)
    {
	ако ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    const char *pt = type;
	    ако (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    ако (pt[0] != '*')
	    {
		switch (pt[0])
		{
		    case 'd':
		    case 'i': break;
		    default:
			semsg(_(e_positional_num_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			върни FAIL;
		}
	    }
	}
	иначе
	{
	    ако (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_num_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		върни FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    върни OK;
}

    static void
format_overflow_error(const char *pstart)
{
    size_t	arglen = 0;
    char	*argcopy = NULL;
    const char	*p = pstart;

    докато (VIM_ISDIGIT((цяло)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(char, arglen + 1);
    ако (argcopy != NULL)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	free(argcopy);
    }
    иначе
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    static цяло
get_unsigned_int(
    const char *pstart,
    const char **p,
    unsigned цяло *uj,
    цяло overflow_err)
{
    *uj = **p - '0';
    ++*p;

    докато (VIM_ISDIGIT((цяло)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned цяло)(**p - '0');
	++*p;
    }

    ако (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	ако (overflow_err)
	{
	    format_overflow_error(pstart);
	    върни FAIL;
	}
	иначе
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    върни OK;
}


    static цяло
parse_fmt_types(
    const char  ***ap_types,
    цяло		*num_posarg,
    const char  *fmt,
    typval_T	*tvs UNUSED
    )
{
    const char	*p = fmt;
    const char	*arg = NULL;

    цяло		any_pos = 0;
    цяло		any_arg = 0;
    цяло		arg_idx;

# define CHECK_POS_ARG do { \
    ако (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	goto error; \
    } \
} докато (0);

    ако (p == NULL)
	върни OK;

    докато (*p != NUL)
    {
	ако (*p != '%')
	{
	    const char    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	иначе
	{
	    // allowed values: \0, h, l, L
	    char	length_modifier = '\0';

	    // variable for positional arg
	    цяло		pos_arg = -1;
	    const char	*ptype = NULL;
	    const char	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    докато (VIM_ISDIGIT(*ptype))
		++ptype;

	    ако (*ptype == '$')
	    {
		ако (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}

		// Positional argument
		unsigned цяло uj;

		ако (get_unsigned_int(pstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    докато (*p == '0' || *p == '-' || *p == '+' || *p == ' '
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
	    ако (*(arg = p) == '*')
	    {
		p++;

		ако (VIM_ISDIGIT((цяло)(*p)))
		{
		    // Positional argument field width
		    unsigned цяло uj;

		    ако (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    ако (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		    иначе
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			ако (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
			    goto error;
		    }
		}
		иначе
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    иначе ако (VIM_ISDIGIT((цяло)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const char *digstart = p;
		unsigned цяло uj;

		ако (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		ако (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}
	    }

	    // parse precision
	    ако (*p == '.')
	    {
		p++;

		ако (*(arg = p) == '*')
		{
		    p++;

		    ако (VIM_ISDIGIT((цяло)(*p)))
		    {
			// Parse precision
			unsigned цяло uj;

			ако (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			ако (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    ако (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
				goto error;
			}
			иначе
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    goto error;
			}
		    }
		    иначе
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		иначе ако (VIM_ISDIGIT((цяло)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const char *digstart = p;
		    unsigned цяло uj;

		    ако (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    ако (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		}
	    }

	    ако (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ако (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		ако (length_modifier == 'l' && *p == 'l')
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
		    ако (pos_arg != -1)
		    {
			ако (adjust_types(ap_types, pos_arg, num_posarg, ptype) == FAIL)
			    goto error;
		    }
		    иначе
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    break;

		default:
		    ако (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			goto error;
		    }
	    }

	    ако (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    за (arg_idx = 0; arg_idx < *num_posarg; ++arg_idx)
    {
	ако ((*ap_types)[arg_idx] == NULL)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    goto error;
	}

# ако defined(FEAT_EVAL)
	ако (tvs != NULL && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    goto error;
	}
# endif
    }

    върни OK;

error:
    vim_free((char**)*ap_types);
    *ap_types = NULL;
    *num_posarg = 0;
    върни FAIL;
}

    static void
skip_to_arg(
    const char	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    цяло		*arg_idx,
    цяло		*arg_cur,
    const char	*fmt)
{
    цяло		arg_min = 0;

    ако (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	върни;
    }

    ако (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    иначе
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    за (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	const char *p;

	ако (ap_types == NULL || ap_types[*arg_cur] == NULL)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    върни;
	}

	p = ap_types[*arg_cur];

	цяло fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	switch (fmt_type)
	{
	case TYPE_PERCENT:
	case TYPE_UNKNOWN:
	    break;

	case TYPE_CHAR:
	    va_arg(*ap, цяло);
	    break;

	case TYPE_STRING:
	    va_arg(*ap, char *);
	    break;

	case TYPE_POINTER:
	    va_arg(*ap, void *);
	    break;

	case TYPE_INT:
	    va_arg(*ap, цяло);
	    break;

	case TYPE_LONGINT:
	    va_arg(*ap, long цяло);
	    break;

	case TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    break;

	case TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned цяло);
	    break;

	case TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long цяло);
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

    върни;
}

    цяло
vim_vsnprintf_typval(
    char	*str,
    size_t	str_m,
    const char	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    const char	*p = fmt;
    цяло		arg_cur = 0;
    цяло		num_posarg = 0;
    цяло		arg_idx = 1;
    va_list	ap;
    const char	**ap_types = NULL;

    ако (parse_fmt_types(&ap_types, &num_posarg, fmt, tvs) == FAIL)
	върни 0;

    va_copy(ap, ap_start);

    ако (p == NULL)
	p = "";
    докато (*p != NUL)
    {
	ако (*p != '%')
	{
	    const char    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    ако (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	иначе
	{
	    size_t  min_field_width = 0, precision = 0;
	    цяло	    zero_padding = 0, precision_specified = 0, justify_left = 0;
	    цяло	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    цяло	    space_for_positive = 1;

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
	    цяло	    pos_arg = -1;
	    const char	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    докато (VIM_ISDIGIT(*ptype))
		++ptype;

	    ако (*ptype == '$')
	    {
		// Positional argument
		const char *digstart = p;
		unsigned цяло uj;

		ако (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    докато (*p == '0' || *p == '-' || *p == '+' || *p == ' '
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
	    ако (*p == '*')
	    {
		цяло j;
		const char *digstart = p + 1;

		p++;

		ако (VIM_ISDIGIT((цяло)(*p)))
		{
		    // Positional argument field width
		    unsigned цяло uj;

		    ако (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# ако defined(FEAT_EVAL)
		    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, цяло));

		ако (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    ако (tvs != NULL)
		    {
			format_overflow_error(digstart);
			goto error;
		    }
		    иначе
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		ако (j >= 0)
		    min_field_width = j;
		иначе
		{
		    min_field_width = -j;
		    justify_left = 1;
		}
	    }
	    иначе ако (VIM_ISDIGIT((цяло)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const char *digstart = p;
		unsigned цяло uj;

		ако (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		min_field_width = uj;
	    }

	    // parse precision
	    ако (*p == '.')
	    {
		p++;
		precision_specified = 1;

		ако (VIM_ISDIGIT((цяло)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const char *digstart = p;
		    unsigned цяло uj;

		    ако (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    precision = uj;
		}
		иначе ако (*p == '*')
		{
		    цяло j;
		    const char *digstart = p;

		    p++;

		    ако (VIM_ISDIGIT((цяло)(*p)))
		    {
			// positional argument
			unsigned цяло uj;

			ако (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			arg_idx = uj;

			++p;
		    }

		    j =
# ако defined(FEAT_EVAL)
			tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, цяло));

		    ако (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			ако (tvs != NULL)
			{
			    format_overflow_error(digstart);
			    goto error;
			}
			иначе
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    ако (j >= 0)
			precision = j;
		    иначе
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ако (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		ако (length_modifier == 'l' && *p == 'l')
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

# ако defined(FEAT_EVAL)
	    switch (fmt_spec)
	    {
		case 'd': case 'u': case 'o': case 'x': case 'X':
		    ако (tvs != NULL && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    ако (pos_arg != -1)
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
			цяло j;

			j =
# ако defined(FEAT_EVAL)
			    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, цяло));

			// standard demands unsigned char
			uchar_arg = (unsigned char)j;
			str_arg = (char *)&uchar_arg;
			break;
		    }

		case 's':
		case 'S':
		    str_arg =
# ако defined(FEAT_EVAL)
				tvs != NULL ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, char *));

		    ако (str_arg == NULL)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    иначе ако (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    иначе ако (precision == 0)
			str_arg_l = 0;
		    иначе
		    {
			// memchr on HP does not like n > 2^31  !!!
			const char *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == NULL) ? precision
						      : (size_t)(q - str_arg);
		    }
		    ако (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			цяло	cell;

			за (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    ако (precision_specified && i + cell > precision)
				break;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			ако (min_field_width != 0)
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
		    цяло arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    цяло int_arg = 0;
		    unsigned цяло uint_arg = 0;

		    // only set for length modifier l
		    long цяло long_arg = 0;
		    unsigned long цяло ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    void *ptr_arg = NULL;

		    ако (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# ако defined(FEAT_EVAL)
				 tvs != NULL ? (void *)tv_str(tvs, &arg_idx,
									NULL) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, void *));

			ако (ptr_arg != NULL)
			    arg_sign = 1;
		    }
		    иначе ако (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# ако defined(FEAT_EVAL)
				    tvs != NULL ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			ако (bin_arg != 0)
			    arg_sign = 1;
		    }
		    иначе ако (fmt_spec == 'd')
		    {
			// signed
			switch (length_modifier)
			{
			case '\0':
			case 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# ако defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, цяло));

			    ако (int_arg > 0)
				arg_sign =  1;
			    иначе ако (int_arg < 0)
				arg_sign = -1;
			    break;
			case 'l':
			    long_arg =
# ако defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long цяло));

			    ако (long_arg > 0)
				arg_sign =  1;
			    иначе ако (long_arg < 0)
				arg_sign = -1;
			    break;
			case 'L':
			    llong_arg =
# ако defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    ако (llong_arg > 0)
				arg_sign =  1;
			    иначе ако (llong_arg < 0)
				arg_sign = -1;
			    break;
			}
		    }
		    иначе
		    {
			// unsigned
			switch (length_modifier)
			{
			    case '\0':
			    case 'h':
				uint_arg =
# ако defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned цяло));

				ако (uint_arg != 0)
				    arg_sign = 1;
				break;
			    case 'l':
				ulong_arg =
# ако defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long цяло));

				ако (ulong_arg != 0)
				    arg_sign = 1;
				break;
			    case 'L':
				ullong_arg =
# ако defined(FEAT_EVAL)
					    tvs != NULL ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				ако (ullong_arg != 0)
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
		    ако (precision_specified)
			zero_padding = 0;
		    ако (fmt_spec == 'd')
		    {
			ако (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    иначе ако (alternate_form)
		    {
			ако (arg_sign != 0
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
		    ако (!precision_specified)
			precision = 1;   // default precision is 1
		    ако (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    иначе
		    {
			char	f[6];
			цяло	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			ако (!length_modifier)
			    ;
			иначе ако (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# иначе
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			иначе
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			ако (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			иначе ако (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    char	    b[8 * sizeof(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    do
			    {
				b[sizeof(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    докато (bn != 0);

			    memcpy(tmp + str_arg_l, b + sizeof(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			иначе ако (fmt_spec == 'd')
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
			иначе
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
			ако (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			ако (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t num_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			ако (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    ако (!precision_specified
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
			ако (num_of_digits < precision)
			    number_of_zeros_to_pad = precision - num_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    ако (!justify_left && zero_padding)
		    {
			цяло n = (цяло)(min_field_width - (str_arg_l
						    + number_of_zeros_to_pad));
			ако (n > 0)
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
		    цяло		l;
		    цяло		remove_trailing_zeroes = FALSE;

		    f =
# ако defined(FEAT_EVAL)
			tvs != NULL ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, double));

		    abs_f = f < 0 ? -f : f;

		    ако (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			ако ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			иначе
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    ако ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# иначе
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
		    иначе
		    {
			ако (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			иначе ако (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			иначе
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    ако (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    ако (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				ако ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				ако (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (цяло)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			ако (remove_trailing_zeroes)
			{
			    цяло i;
			    char *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    ако (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    иначе
			    {
				tp = (char *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				ако (tp != NULL)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    ако (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    докато (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    ако (tp != NULL && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				докато (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			иначе
			{
			    char *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (char *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    ако (tp != NULL && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    ако (zero_padding && min_field_width > str_arg_l
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
		ако (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		break;
	    }

	    ако (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    ако (!justify_left)
	    {
		// left padding with blank or zero
		цяло pn = (цяло)(min_field_width - (str_arg_l + number_of_zeros_to_pad));

		ако (pn > 0)
		{
		    ако (str_l < str_m)
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
	    ако (number_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    иначе
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		цяло zn = (цяло)zero_padding_insertion_ind;

		ако (zn > 0)
		{
		    ако (str_l < str_m)
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
		zn = (цяло)number_of_zeros_to_pad;
		ако (zn > 0)
		{
		    ако (str_l < str_m)
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
		цяло sn = (цяло)(str_arg_l - zero_padding_insertion_ind);

		ако (sn > 0)
		{
		    ако (str_l < str_m)
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
	    ако (justify_left)
	    {
		// right blank padding to the field width
		цяло pn = (цяло)(min_field_width
				      - (str_arg_l + number_of_zeros_to_pad));

		ако (pn > 0)
		{
		    ако (str_l < str_m)
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

    ако (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    ако (tvs != NULL && tvs[num_posarg != 0 ? num_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((char*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    върни (цяло)str_l;
}

#endif // PROTO
