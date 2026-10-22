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
    නම් (p != NULL)
	mch_memmove(p, string, len);
    ආපසු p;
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
    නම් (p == NULL)
	ආපසු NULL;

    STRNCPY(p, string, len);
    p[len] = NUL;
    ආපසු p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    ආපසු vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    පූර්ණ_සංඛ්‍යා		cc,
    පූර්ණ_සංඛ්‍යා		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    පූර්ණ_සංඛ්‍යා		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    සඳහා (p = string; *p; p++)
    {
	නම් (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    continue;
	}
	නම් (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    නම් (escaped_string == NULL)
	ආපසු NULL;
    p2 = escaped_string;
    සඳහා (p = string; *p; p++)
    {
	නම් (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    continue;
	}
	නම් (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    ආපසු escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    පූර්ණ_සංඛ්‍යා
csh_like_shell(void)
{
    ආපසු (strstr((var_na *)gettail(p_sh), "csh") != NULL);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    static පූර්ණ_සංඛ්‍යා
fish_like_shell(void)
{
    ආපසු (strstr((var_na *)gettail(p_sh), "fish") != NULL);
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
vim_strsave_shellescape(char_u *string, පූර්ණ_සංඛ්‍යා do_special, පූර්ණ_සංඛ්‍යා do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    පූර්ණ_සංඛ්‍යා		csh_like;
    පූර්ණ_සංඛ්‍යා		fish_like;
    char_u	*shname;
    පූර්ණ_සංඛ්‍යා		powershell;
#ifdef MSWIN
    පූර්ණ_සංඛ්‍යා		double_quotes;
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
    powershell = strstr((var_na *)shname, "pwsh") != NULL;
#ifdef MSWIN
    powershell = powershell || strstr((var_na *)shname, "powershell") != NULL;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (unsigned)STRLEN(string) + 3;  // two quotes and a trailing NUL
    සඳහා (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	නම් (double_quotes)
	{
	    නම් (*p == '"')
		++length;		// " -> ""
	}
	නැතහොත්
#endif
	නම් (*p == '\'')
	{
	    නම් (powershell)
		length += 2;		// ' => ''
	    නැතහොත්
		length += 3;		// ' => '\''
	}
	නම් ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    නම් (csh_like && do_special)
		++length;		// insert backslash
	}
	නම් (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	නම් (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    නම් (escaped_string != NULL)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	නම් (double_quotes)
	    *d++ = '"';
	නැතහොත්
#endif
	    *d++ = '\'';

	සඳහා (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    නම් (double_quotes)
	    {
		නම් (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    continue;
		}
	    }
	    නැතහොත්
#endif
	    නම් (*p == '\'')
	    {
		නම් (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		නැතහොත්
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		continue;
	    }
	    නම් ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		නම් (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		continue;
	    }
	    නම් (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		continue;
	    }
	    නම් (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		continue;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	නම් (double_quotes)
	    *d++ = '"';
	නැතහොත්
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    ආපසු escaped_string;
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
    ආපසු p1;
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
    ආපසු p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    void
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    පූර්ණ_සංඛ්‍යා	    c;

    නම් (p == NULL)
	ආපසු;

    p2 = p;
    අතරතුර ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#නම් defined(FEAT_EVAL) || defined(FEAT_SPELL)
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

    නම් (res != NULL)
	අතරතුර (*p != NUL)
	{
	    පූර්ණ_සංඛ්‍යා		l;

	    නම් (enc_utf8)
	    {
		පූර්ණ_සංඛ්‍යා	c, uc;
		පූර්ණ_සංඛ්‍යා	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		නම් (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		නම් (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    නම් (s == NULL)
		    {
			vim_free(res);
			ආපසු NULL;
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
	    නැතහොත් නම් (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    නැතහොත්
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    ආපසු res;
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

    නම් (res != NULL)
	අතරතුර (*p != NUL)
	{
	    පූර්ණ_සංඛ්‍යා		l;

	    නම් (enc_utf8)
	    {
		පූර්ණ_සංඛ්‍යා	c, lc;
		පූර්ණ_සංඛ්‍යා	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		නම් (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		නම් (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    නම් (s == NULL)
		    {
			vim_free(res);
			ආපසු NULL;
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
	    නැතහොත් නම් (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    නැතහොත්
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    ආපසු res;
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
    අතරතුර (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
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

    නම් (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    නැතහොත්
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(var_na *s, size_t maxlen)
{
    size_t i;
    සඳහා (i = 0; i < maxlen; ++i)
	නම් (s[i] == NUL)
	    bha_nga;
    ආපසු i;
}

#නම් !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    පූර්ණ_සංඛ්‍යා
vim_stricmp(var_na *s1, var_na *s2)
{
    පූර්ණ_සංඛ්‍යා		i;

    සඳහා (;;)
    {
	i = (පූර්ණ_සංඛ්‍යා)TOLOWER_LOC(*s1) - (පූර්ණ_සංඛ්‍යා)TOLOWER_LOC(*s2);
	නම් (i != 0)
	    ආපසු i;			    // this character different
	නම් (*s1 == NUL)
	    bha_nga;			    // strings match until NUL
	++s1;
	++s2;
    }
    ආපසු 0;				    // strings match
}
#endif

#නම් !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    පූර්ණ_සංඛ්‍යා
vim_strnicmp(var_na *s1, var_na *s2, size_t len)
{
    පූර්ණ_සංඛ්‍යා		i;

    අතරතුර (len > 0)
    {
	i = (පූර්ණ_සංඛ්‍යා)TOLOWER_LOC(*s1) - (පූර්ණ_සංඛ්‍යා)TOLOWER_LOC(*s2);
	නම් (i != 0)
	    ආපසු i;			    // this character different
	නම් (*s1 == NUL)
	    bha_nga;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ආපසු 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    පූර්ණ_සංඛ්‍යා
vim_strnicmp_asc(var_na *s1, var_na *s2, size_t len)
{
    පූර්ණ_සංඛ්‍යා                i = 0;

    අතරතුර (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	නම් (i != 0)
	    bha_nga;			// this character is different
	නම් (*s1 == NUL)
	    bha_nga;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ආපසු i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, පූර්ණ_සංඛ්‍යා c)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		b;

    p = string;
    නම් (enc_utf8 && c > 0 && c < 0x80)
	ආපසු vim_strbyte(string, c);
    නම් (enc_utf8 && c >= 0x80)
    {
	අතරතුර (*p != NUL)
	{
	    පූර්ණ_සංඛ්‍යා l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    නම් (utf_ptr2char(p) == c && l > 1)
		ආපසු p;
	    p += l;
	}
	ආපසු NULL;
    }
    නම් (enc_dbcs != 0 && c > 255)
    {
	පූර්ණ_සංඛ්‍යා	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	අතරතුර ((b = *p) != NUL)
	{
	    නම් (b == c && p[1] == n2)
		ආපසු p;
	    p += (*mb_ptr2len)(p);
	}
	ආපසු NULL;
    }
    නම් (has_mbyte)
    {
	අතරතුර ((b = *p) != NUL)
	{
	    නම් (b == c)
		ආපසු p;
	    p += (*mb_ptr2len)(p);
	}
	ආපසු NULL;
    }
    අතරතුර ((b = *p) != NUL)
    {
	නම් (b == c)
	    ආපසු p;
	++p;
    }
    ආපසු NULL;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, පූර්ණ_සංඛ්‍යා c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    නම් (c <= 0 || c > 255)
	ආපසු NULL;
    ආපසු (char_u *)strchr((var_na *)string, c);
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
vim_strrchr(char_u *string, පූර්ණ_සංඛ්‍යා c)
{
    char_u	*retval = NULL;
    char_u	*p = string;

    අතරතුර (*p)
    {
	නම් (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    ආපසු retval;
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
    අතරතුර (*s)
    {
	නම් (vim_strchr(charset, *s) != NULL)
	    ආපසු s;
	MB_PTR_ADV(s);
    }
    ආපසු NULL;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
static පූර්ණ_සංඛ්‍යා sort_compare(const void *s1, const void *s2);

    static පූර්ණ_සංඛ්‍යා
sort_compare(const void *s1, const void *s2)
{
    ආපසු STRCMP(*(var_na **)s1, *(var_na **)s2);
}

    void
sort_strings(
    char_u	**files,
    පූර්ණ_සංඛ්‍යා		count)
{
    qsort((void *)files, (size_t)count, parimaa_na(char_u *), sort_compare);
}

#නම් defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    පූර්ණ_සංඛ්‍යා
has_non_ascii(char_u *s)
{
    char_u	*p;

    නම් (s != NULL)
	සඳහා (p = s; *p != NUL; ++p)
	    නම් (*p >= 128)
		ආපසු TRUE;
    ආපසු FALSE;
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
    නම් (dest == NULL)
	ආපසු NULL;
    නම් (str1 == NULL)
	*dest = NUL;
    නැතහොත්
	STRCPY(dest, str1);
    නම් (str2 != NULL)
	STRCPY(dest + l, str2);
    ආපසු dest;
}

#නම් defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    නම් (rev == NULL)
	ආපසු NULL;

    සඳහා (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	නම් (has_mbyte)
	{
	    පූර්ණ_සංඛ්‍යා mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	නැතහොත්
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    ආපසු rev;
}
#endif

#නම් defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, පූර්ණ_සංඛ්‍යා function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    නම් (str != NULL)
    {
	len += (unsigned)STRLEN(str);
	සඳහා (p = str; *p != NUL; MB_PTR_ADV(p))
	    නම් (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    නම් (r == NULL)
	ආපසු NULL;

    නම් (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    නැතහොත්
	*r++ = '\'';
    නම් (str != NULL)
	සඳහා (p = str; *p != NUL; )
	{
	    නම් (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    නම් (function)
	*r++ = ')';
    *r++ = NUL;
    ආපසු s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_count(char_u *haystack, char_u *needle, පූර්ණ_සංඛ්‍යා ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    නම් (p == NULL || needle == NULL || *needle == NUL)
	ආපසු 0;

    size_t  needlelen = STRLEN(needle);
    නම් (ic)
    {
	අතරතුර (*p != NUL)
	{
	    නම් (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    නැතහොත්
		MB_PTR_ADV(p);
	}
    }
    නැතහොත්
	අතරතුර ((next = (char_u *)strstr((var_na *)p, (var_na *)needle)) != NULL)
	{
	    ++n;
	    p = next + needlelen;
	}

    ආපසු n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    static පූර්ණ_සංඛ්‍යා
copy_first_char_to_tv(char_u *input, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    පූර්ණ_සංඛ්‍යා		len;

    නම් (input == NULL || output == NULL)
	ආපසු -1;

    len = has_mbyte ? mb_ptr2len(input) : 1;
    STRNCPY(buf, input, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    ආපසු output->vval.v_string == NULL ? -1 : len;
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
    පූර්ණ_සංඛ්‍යා		len = 0;
    පූර්ණ_සංඛ්‍යා		idx = 0;
    පූර්ණ_සංඛ්‍යා		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    ga_init2(&ga, parimaa_na(var_na), 80);
    සඳහා (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	නම් (len < 0)
	    bha_nga;

	set_vim_var_nr(VV_KEY, idx);
	නම් (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    bha_nga;
	}
	නම් (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    නම් (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		bha_nga;
	    }
	    නැතහොත්
		ga_concat(&ga, newtv.vval.v_string);
	}
	නැතහොත් නම් (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    නම් (fc != NULL)
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
    පූර්ණ_සංඛ්‍යා		len;
    typval_T	argv[3];
    පූර්ණ_සංඛ්‍යා		r;
    පූර්ණ_සංඛ්‍යා		called_emsg_start = called_emsg;
    funccall_T	*fc;

    නම් (argvars[2].v_type == VAR_UNKNOWN)
    {
	නම් (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "String");
	    ආපසු;
	}
	len = copy_first_char_to_tv(p, rettv);
	නම් (len < 0)
	    ආපසු;
	p += len;
    }
    නැතහොත් නම් (check_for_string_arg(argvars, 2) == FAIL)
	ආපසු;
    නැතහොත්
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    සඳහා ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	නම් (len < 0)
	{
	    *rettv = argv[0];
	    bha_nga;
	}

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	නම් (r == FAIL || called_emsg != called_emsg_start)
	    bha_nga;
    }

    නම් (fc != NULL)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    static void
byteidx_common(typval_T *argvars, typval_T *rettv, පූර්ණ_සංඛ්‍යා comp)
{
    rettv->vval.v_number = -1;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ආපසු;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    නම් (str == NULL || idx < 0)
	ආපසු;

    varnumber_T	utf16idx = FALSE;
    නම් (argvars[2].v_type != VAR_UNKNOWN)
    {
	පූර්ණ_සංඛ්‍යා error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	නම් (error)
	    ආපසු;
	නම් (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), utf16idx);
	    ආපසු;
	}
    }

    පූර්ණ_සංඛ්‍යා (*ptr2len)(char_u *);
    නම් (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    නැතහොත්
	ptr2len = mb_ptr2len;

    char_u *t = str;
    සඳහා ( ; idx > 0; idx--)
    {
	නම් (*t == NUL)		// EOL reached
	    ආපසු;
	නම් (utf16idx)
	{
	    පූර්ණ_සංඛ්‍යා clen = ptr2len(t);
	    පූර්ණ_සංඛ්‍යා c = (clen > 1) ? utf_ptr2char(t) : *t;
	    නම් (c > 0xFFFF)
		idx--;
	    නම් (idx > 0)
		t += clen;
	}
	නැතහොත් නම් (idx > 0)
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

    නම් (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	ආපසු;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    නම් (str == NULL || idx < 0)
	ආපසු;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    නම් (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	නම් (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    පූර්ණ_සංඛ්‍යා (*ptr2len)(char_u *);
    නම් (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    නැතහොත්
	ptr2len = mb_ptr2len;

    char_u	*p;
    පූර්ණ_සංඛ්‍යා		len;
    සඳහා (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	නම් (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    නම් (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    ආපසු;
	}
	නම් (utf16idx)
	{
	    idx--;
	    පූර්ණ_සංඛ්‍යා clen = ptr2len(p);
	    පූර්ණ_සංඛ්‍යා c = (clen > 1) ? utf_ptr2char(p) : *p;
	    නම් (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_number = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    static පූර්ණ_සංඛ්‍යා
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    නම් (convert_setup(&vimconv, from, to) == FAIL)
	ආපසු FAIL;
    vimconv.vc_fail = TRUE;
    නම් (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	නම් (ret->string == NULL)
	    ret->length = 0;
	නැතහොත්
	    ret->length = str->length;
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා len = (පූර්ණ_සංඛ්‍යා)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, NULL, NULL);

    ආපසු (ret->string == NULL) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    static void
blob_from_string(char_u *str, blob_T *blob)
{
    පූර්ණ_සංඛ්‍යා	    len = (පූර්ණ_සංඛ්‍යා)STRLEN(str);
    char_u  *dest;

    නම් (len == 0)
	ආපසු;
    නම් (ga_grow(&blob->bv_ga, len) == FAIL)
	ආපසු;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < len; ++i)
	නම් (dest[i] == NL)
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
    static පූර්ණ_සංඛ්‍යා
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	line_len;

    නම් (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	ආපසු (ret->string == NULL) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    line_len = (nl == NULL) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(line_len + 1);
    නම් (ret->string == NULL)
    {
	ret->length = 0;
	ආපසු FAIL;
    }
    නම් (line_len > 0)
	mch_memmove(ret->string, src, (size_t)line_len);
    ret->string[line_len] = NUL;
    ret->length = (size_t)line_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    සඳහා (long i = 0; i < line_len; i++)
	නම් (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + line_len + (nl != NULL ? 1 : 0);
    ආපසු OK;
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
    නම් (from_encoding_raw == NULL)
	ආපසු NULL;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    අතරතුර (*s != NUL)
    {
	නම් (*s == '_')
	    *pe++ = '-';
	නැතහොත්
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    නම් ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    ආපසු from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    static void
append_converted_string_to_list(
	string_T *converted,
	පූර්ණ_සංඛ්‍යා validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    නම් (converted->string != NULL)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	අතරතුර (p < end)
	{
	    string_T	line;
	    char_u	*line_start = p;

	    අතරතුර (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    line.length = (size_t)(p - line_start);
	    line.string = vim_strnsave(line_start, line.length);
	    නම් (line.string != NULL)
	    {
		නම් (validate_utf8 && !utf_valid_string(line.string, NULL))
		{
		    vim_free(line.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    ආපසු; // Stop processing
		}
		නම් (list_append_string(list, line.string, (පූර්ණ_සංඛ්‍යා)line.length) == FAIL)
		{
		    vim_free(line.string);
		    vim_free(converted->string);
		    ආපසු; // Stop processing on append failure
		}
		vim_free(line.string);
	    }
	    නැතහොත්
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		ආපසු;
	    }

	    නම් (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    නැතහොත්
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    static පූර්ණ_සංඛ්‍යා
append_validated_line_to_list(string_T *line, පූර්ණ_සංඛ්‍යා validate_utf8, list_T *list)
{
    නම් (validate_utf8 && !utf_valid_string(line->string, NULL))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(line->string);
	ආපසු FAIL;
    }

    පූර්ණ_සංඛ්‍යා ret = list_append_string(list, line->string, (පූර්ණ_සංඛ්‍යා)line->length);
    vim_free(line->string);
    ආපසු ret;
}

    void
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    පූර්ණ_සංඛ්‍යා		blen;
    long	idx;
    පූර්ණ_සංඛ්‍යා		validate_utf8 = FALSE;

    නම් (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ආපසු;

    නම් (rettv_list_alloc(rettv) == FAIL)
	ආපසු;

    blob = argvars->vval.v_blob;
    නම් (blob == NULL)
	ආපසු;
    blen = blob_len(blob);

    char_u	*from_encoding = NULL;
    char_u	*from_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    නම් (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	නම් (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    නම් (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		නම් (from_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    ආපසු;
		}
	    }
	}
    }

    නම් (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    නම් (from_encoding != NULL && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    පූර්ණ_සංඛ්‍යා from_prop = 0;
    නම් (from_encoding != NULL)
	from_prop = enc_canon_props(from_encoding);
    නම් (from_encoding != NULL && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	පූර්ණ_සංඛ්‍යා nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	නම් (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    නම් (blen > 0)
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
	නම් (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    goto done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	පූර්ණ_සංඛ්‍යා len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, NULL);
	converted.length = len;
	convert_setup(&vimconv, NULL, NULL);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    නැතහොත්
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	අතරතුර (idx < blen)
	{
	    string_T	str;

	    නම් (string_from_blob(blob, &idx, &str) != OK)
		bha_nga;

	    නම් (from_encoding != NULL)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		පූර්ණ_සංඛ්‍යා	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		නම් (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    නම් (append_validated_line_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		goto done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    නම් (blen > 0 && blob_get(blob, blen - 1) == NL)
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

    නම් (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ආපසු;

    නම් (rettv_blob_alloc(rettv) == FAIL)
	ආපසු;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    නම් (list == NULL)
	ආපසු;

    char_u	*to_encoding = NULL;
    char_u	*to_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    නම් (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	නම් (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    නම් (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		නම් (to_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    ආපසු;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    පූර්ණ_සංඛ්‍යා to_prop = 0;
    නම් (to_encoding != NULL)
	to_prop = enc_canon_props(to_encoding);
    නම් (to_encoding != NULL && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    char_u *s;

	    නම් (li->li_tv.v_type != VAR_STRING)
		continue;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    නම් (li != list->lv_first)
		ga_append(&str_ga, NL);
	    නම් (s != NULL && *s != NUL)
	    {
		පූර්ණ_සංඛ්‍යා slen = (පූර්ණ_සංඛ්‍යා)STRLEN(s);

		නම් (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    goto done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	නම් (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    නම් (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    vimconv.vc_fail = TRUE;

	    පූර්ණ_සංඛ්‍යා		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, NULL);
	    convert_setup(&vimconv, NULL, NULL);
	    ga_clear(&str_ga);

	    නම් (converted == NULL)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    නම් (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	නැතහොත්
	    ga_clear(&str_ga);
    }
    නැතහොත්
    {
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    නම් (li->li_tv.v_type != VAR_STRING)
		continue;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    නම් (str.string == NULL)
		STR_LITERAL_SET(str, "");
	    නැතහොත්
		str.length = STRLEN(str.string);

	    නම් (to_encoding != NULL)
	    {
		පූර්ණ_සංඛ්‍යා	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		නම් (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    නම් (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    නම් (to_encoding != NULL)
		vim_free(str.string);
	}
    }

done:
    නම් (to_encoding != NULL)
	vim_free(to_encoding);
    නම් (to_encoding_raw != NULL)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    void
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		utf8 = FALSE;

    නම් (rettv_list_alloc(rettv) == FAIL)
	ආපසු;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ආපසු;

    නම් (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (පූර්ණ_සංඛ්‍යා)tv_get_bool_chk(&argvars[1], NULL);

    p = tv_get_string(&argvars[0]);

    නම් (has_mbyte || utf8)
    {
	පූර්ණ_සංඛ්‍යා (*ptr2len)(char_u *);
	පූර්ණ_සංඛ්‍යා (*ptr2char)(char_u *);

	නම් (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	නැතහොත්
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	සඳහා ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_number(rettv->vval.v_list, (*ptr2char)(p));
    }
    නැතහොත්
	සඳහා ( ; *p != NUL; ++p)
	    list_append_number(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    void
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    පූර්ණ_සංඛ්‍යා		base = 10;
    char_u	*p;
    varnumber_T	n;
    පූර්ණ_සංඛ්‍යා		what = 0;
    පූර්ණ_සංඛ්‍යා		isneg;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ආපසු;

    නම් (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (පූර්ණ_සංඛ්‍යා)tv_get_number(&argvars[1]);
	නම් (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    ආපසු;
	}
	නම් (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    නම් (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    switch (base)
    {
	pak_sa 2: what |= STR2NR_BIN + STR2NR_FORCE; bha_nga;
	pak_sa 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; bha_nga;
	pak_sa 16: what |= STR2NR_HEX + STR2NR_FORCE; bha_nga;
    }
    vim_str2nr(p, NULL, NULL, what, &n, NULL, 0, FALSE, NULL);
    // Text after the number is silently ignored.
    නම් (isneg)
	rettv->vval.v_number = -n;
    නැතහොත්
	rettv->vval.v_number = n;

}

/*
 * "strgetchar()" function
 */
    void
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		error = FALSE;
    පූර්ණ_සංඛ්‍යා		charidx;
    පූර්ණ_සංඛ්‍යා		byteidx = 0;

    rettv->vval.v_number = -1;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	ආපසු;

    str = tv_get_string_chk(&argvars[0]);
    නම් (str == NULL)
	ආපසු;
    len = (පූර්ණ_සංඛ්‍යා)STRLEN(str);
    charidx = (පූර්ණ_සංඛ්‍යා)tv_get_number_chk(&argvars[1], &error);
    නම් (error)
	ආපසු;

    අතරතුර (charidx >= 0 && byteidx < len)
    {
	නම් (charidx == 0)
	{
	    rettv->vval.v_number = mb_ptr2char(str + byteidx);
	    bha_nga;
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
    පූර්ණ_සංඛ්‍යා		start_idx;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	ආපසු;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_number = -1;
    නම් (needle == NULL || haystack == NULL)
	ආපසු;		// type error; errmsg already given

    නම් (argvars[2].v_type != VAR_UNKNOWN)
    {
	පූර්ණ_සංඛ්‍යා	    error = FALSE;

	start_idx = (පූර්ණ_සංඛ්‍යා)tv_get_number_chk(&argvars[2], &error);
	නම් (error || start_idx >= (පූර්ණ_සංඛ්‍යා)STRLEN(haystack))
	    ආපසු;
	නම් (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((var_na *)haystack, (var_na *)needle);
    නම් (pos != NULL)
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
    නම් (rettv->vval.v_string != NULL && tofree == NULL)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    void
f_strlen(typval_T *argvars, typval_T *rettv)
{
    නම් (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	ආපසු;

    rettv->vval.v_number = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    static void
strchar_common(typval_T *argvars, typval_T *rettv, පූර්ණ_සංඛ්‍යා skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    පූර්ණ_සංඛ්‍යා			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    අතරතුර (*s != NUL)
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
    නම් (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	ආපසු;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    void
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ආපසු;

    නම් (argvars[1].v_type != VAR_UNKNOWN)
    {
	පූර්ණ_සංඛ්‍යා error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	නම් (error)
	    ආපසු;
	නම් (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), skipcc);
	    ආපසු;
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

    නම් (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	ආපසු;

    varnumber_T countcc = FALSE;
    නම් (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    පූර්ණ_සංඛ්‍යා			(*func_mb_ptr2char_adv)(char_u **pp);
    පූර්ණ_සංඛ්‍යා			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    අතරතුර (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	නම් (ch > 0xFFFF)
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
    පූර්ණ_සංඛ්‍යා		col = 0;

    rettv->vval.v_number = -1;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ආපසු;

    s = tv_get_string(&argvars[0]);
    නම් (argvars[1].v_type != VAR_UNKNOWN)
	col = (පූර්ණ_සංඛ්‍යා)tv_get_number(&argvars[1]);

    rettv->vval.v_number = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    void
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    නම් (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

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
    පූර්ණ_සංඛ්‍යා		nchar;
    පූර්ණ_සංඛ්‍යා		nbyte = 0;
    පූර්ණ_සංඛ්‍යා		charlen;
    පූර්ණ_සංඛ්‍යා		skipcc = FALSE;
    පූර්ණ_සංඛ්‍යා		len = 0;
    පූර්ණ_සංඛ්‍යා		slen;
    පූර්ණ_සංඛ්‍යා		error = FALSE;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ආපසු;

    p = tv_get_string(&argvars[0]);
    slen = (පූර්ණ_සංඛ්‍යා)STRLEN(p);

    nchar = (පූර්ණ_සංඛ්‍යා)tv_get_number_chk(&argvars[1], &error);
    නම් (!error)
    {
	නම් (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    නම් (error)
		ආපසු;
	    නම් (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_number_as_bool_nr), skipcc);
		ආපසු;
	    }
	}

	නම් (nchar > 0)
	    අතරතුර (nchar > 0 && nbyte < slen)
	    {
		නම් (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		නැතහොත්
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	නැතහොත්
	    nbyte = nchar;
	නම් (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (පූර්ණ_සංඛ්‍යා)tv_get_number(&argvars[2]);
	    අතරතුර (charlen > 0 && nbyte + len < slen)
	    {
		පූර්ණ_සංඛ්‍යා off = nbyte + len;

		නම් (off < 0)
		    len += 1;
		නැතහොත්
		{
		    නම් (skipcc)
			len += mb_ptr2len(p + off);
		    නැතහොත්
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	නැතහොත්
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    නම් (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    නැතහොත් නම් (nbyte > slen)
	nbyte = slen;
    නම් (len < 0)
	len = 0;
    නැතහොත් නම් (nbyte + len > slen)
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
    පූර්ණ_සංඛ්‍යා		n;
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		slen;
    පූර්ණ_සංඛ්‍යා		error = FALSE;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ආපසු;

    p = tv_get_string(&argvars[0]);
    slen = (පූර්ණ_සංඛ්‍යා)STRLEN(p);

    n = (පූර්ණ_සංඛ්‍යා)tv_get_number_chk(&argvars[1], &error);
    නම් (error)
	len = 0;
    නැතහොත් නම් (argvars[2].v_type != VAR_UNKNOWN)
	len = (පූර්ණ_සංඛ්‍යා)tv_get_number(&argvars[2]);
    නැතහොත්
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    නම් (n < 0)
    {
	len += n;
	n = 0;
    }
    නැතහොත් නම් (n > slen)
	n = slen;
    නම් (len < 0)
	len = 0;
    නැතහොත් නම් (n + len > slen)
	len = slen - n;

    නම් (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	පූර්ණ_සංඛ්‍යා off;

	// length in characters
	සඳහා (off = n; off < slen && len > 0; --len)
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
    පූර්ණ_සංඛ්‍යා		haystack_len, end_idx;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	ආපසු;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_number = -1;
    නම් (needle == NULL || haystack == NULL)
	ආපසු;		// type error; errmsg already given

    haystack_len = (පූර්ණ_සංඛ්‍යා)STRLEN(haystack);
    නම් (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (පූර්ණ_සංඛ්‍යා)tv_get_number_chk(&argvars[2], NULL);
	නම් (end_idx < 0)
	    ආපසු;	// can never find a match
    }
    නැතහොත්
	end_idx = haystack_len;

    නම් (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    නැතහොත්
    {
	සඳහා (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((var_na *)rest, (var_na *)needle);
	    නම් (rest == NULL || rest > haystack + end_idx)
		bha_nga;
	    lastmatch = rest;
	}
    }

    නම් (lastmatch == NULL)
	rettv->vval.v_number = -1;
    නැතහොත්
	rettv->vval.v_number = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    void
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    නම් (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

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

    නම් (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_number_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    ආපසු;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    නම් (str == NULL || idx < 0)
	ආපසු;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    නම් (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	නම් (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    පූර්ණ_සංඛ්‍යා (*ptr2len)(char_u *);
    නම් (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    නැතහොත්
	ptr2len = mb_ptr2len;

    char_u	*p;
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		utf16idx = 0;
    සඳහා (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	නම් (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    නම් (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    ආපසු;
	}
	utf16idx = len;
	පූර්ණ_සංඛ්‍යා clen = ptr2len(p);
	පූර්ණ_සංඛ්‍යා c = (clen > 1) ? utf_ptr2char(p) : *p;
	නම් (c > 0xFFFF)
	    len++;
	p += clen;
	නම් (charidx)
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
    නම් (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    void
f_toupper(typval_T *argvars, typval_T *rettv)
{
    නම් (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

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
    පූර්ණ_සංඛ්‍යා		inlen;
    පූර්ණ_සංඛ්‍යා		fromlen;
    පූර්ණ_සංඛ්‍යා		tolen;
    පූර්ණ_සංඛ්‍යා		idx;
    char_u	*cpstr;
    පූර්ණ_සංඛ්‍යා		cplen;
    පූර්ණ_සංඛ්‍යා		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ආපසු;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    නම් (fromstr == NULL || tostr == NULL)
	    ආපසු;		// type error; errmsg already given
    ga_init2(&ga, parimaa_na(var_na), 80);

    නම් (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	නම් (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    ආපසු;
	}

    // fromstr and tostr have to contain the same number of chars
    අතරතුර (*in_str != NUL)
    {
	නම් (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    සඳහා (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		නම් (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    සඳහා (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			නම් (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    bha_nga;
			}
		    }
		    නම් (*p == NUL)	// tostr is shorter than fromstr
			goto error;
		    bha_nga;
		}
		++idx;
	    }

	    නම් (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		සඳහා (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		නම් (idx != 0)
		    goto error;
	    }

	    (void)ga_grow(&ga, cplen);
	    mch_memmove((var_na *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	නැතහොත්
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    නම් (p != NULL)
		ga_append(&ga, tostr[p - fromstr]);
	    නැතහොත්
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
    පූර්ණ_සංඛ්‍යා		c1;
    පූර්ණ_සංඛ්‍යා		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ආපසු;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    නම් (head == NULL)
	ආපසු;

    නම් (check_for_opt_string_arg(argvars, 1) == FAIL)
	ආපසු;

    නම් (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	නම් (*mask == NUL)
	    mask = NULL;

	නම් (argvars[2].v_type != VAR_UNKNOWN)
	{
	    පූර්ණ_සංඛ්‍යා	error = 0;

	    // leading or trailing characters to trim
	    dir = (පූර්ණ_සංඛ්‍යා)tv_get_number_chk(&argvars[2], &error);
	    නම් (error)
		ආපසු;
	    නම් (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		ආපසු;
	    }
	}
    }

    නම් (dir == 0 || dir == 1)
    {
	// Trim leading characters
	අතරතුර (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    නම් (mask == NULL)
	    {
		නම් (c1 > ' ' && c1 != 0xa0)
		    bha_nga;
	    }
	    නැතහොත්
	    {
		සඳහා (p = mask; *p != NUL; MB_PTR_ADV(p))
		    නම් (c1 == PTR2CHAR(p))
			bha_nga;
		නම් (*p == NUL)
		    bha_nga;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    නම් (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	සඳහා (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    නම් (mask == NULL)
	    {
		නම් (c1 > ' ' && c1 != 0xa0)
		    bha_nga;
	    }
	    නැතහොත්
	    {
		සඳහා (p = mask; *p != NUL; MB_PTR_ADV(p))
		    නම් (c1 == PTR2CHAR(p))
			bha_nga;
		නම් (*p == NUL)
		    bha_nga;
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
    නම් (str == NULL)
	ආපසු NULL;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    නම් (!decoded)
	ආපසු NULL;

    char_u	*p = decoded;
    size_t	i = 0;

    අතරතුර (i < len)
    {
	නම් (str[i] == '%')
	{
	    නම් (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		නම් (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    නැතහොත්
	    {
		පූර්ණ_සංඛ්‍යා val = hexhex2nr(&str[i + 1]);
		නම් (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		නැතහොත්
		{
		    // invalid hex digits following "%"
		    සඳහා (පූර්ණ_සංඛ්‍යා j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	නැතහොත්
	    *p++ = str[i++];
    }

    *p = NUL;

    ආපසු decoded;
}

/*
 * "uri_decode({str})" function
 */
    void
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    නම් (check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

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
    නම් (str == NULL)
	ආපසු NULL;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    නම් (encoded == NULL)
	ආපසු NULL;

    char_u *p = encoded;

    සඳහා (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	නම් (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	නැතහොත්
	{
	    sprintf((var_na *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    ආපසු encoded;
}

/*
 * "uri_encode({str})" function
 */
    void
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    නම් (check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

static var_na *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static varnumber_T
tv_nr(typval_T *tvs, පූර්ණ_සංඛ්‍යා *idxp)
{
    පූර්ණ_සංඛ්‍යා		idx = *idxp - 1;
    varnumber_T	n = 0;
    පූර්ණ_සංඛ්‍යා		err = FALSE;

    නම් (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    නැතහොත්
    {
	++*idxp;
	n = tv_get_number_chk(&tvs[idx], &err);
	නම් (err)
	    n = 0;
    }
    ආපසු n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    static var_na *
tv_str(typval_T *tvs, පූර්ණ_සංඛ්‍යා *idxp, char_u **tofree)
{
    පූර්ණ_සංඛ්‍යා		    idx = *idxp - 1;
    var_na	    *s = NULL;
    static char_u   numbuf[NUMBUFLEN];

    නම් (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    නැතහොත්
    {
	++*idxp;
	නම් (tofree != NULL)
	    s = (var_na *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	නැතහොත්
	    s = (var_na *)tv_get_string_chk(&tvs[idx]);
    }
    ආපසු s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static dviguu_na
tv_float(typval_T *tvs, පූර්ණ_සංඛ්‍යා *idxp)
{
    පූර්ණ_සංඛ්‍යා		idx = *idxp - 1;
    dviguu_na	f = 0;

    නම් (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    නැතහොත්
    {
	++*idxp;
	නම් (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	නැතහොත් නම් (tvs[idx].v_type == VAR_NUMBER)
	    f = (dviguu_na)tvs[idx].vval.v_number;
	නැතහොත්
	    emsg(_(e_expected_float_argument_for_printf));
    }
    ආපසු f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    static const var_na *
infinity_str(පූර්ණ_සංඛ්‍යා positive,
	     var_na fmt_spec,
	     පූර්ණ_සංඛ්‍යා force_sign,
	     පූර්ණ_සංඛ්‍යා space_for_positive)
{
    static const var_na *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    පූර්ණ_සංඛ්‍යා idx = positive * (1 + force_sign + force_sign * space_for_positive);

    නම් (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    ආපසු table[idx];
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
    පූර්ණ_සංඛ්‍යා
vim_snprintf_add(var_na *str, size_t str_m, const var_na *fmt, ...)
{
    va_list	ap;
    පූර්ණ_සංඛ්‍යා		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    නම් (str_m <= len)
	space = 0;
    නැතහොත්
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    ආපසු str_l;
}

    පූර්ණ_සංඛ්‍යා
vim_snprintf(var_na *str, size_t str_m, const var_na *fmt, ...)
{
    va_list	ap;
    පූර්ණ_සංඛ්‍යා		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    ආපසු str_l;
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
vim_snprintf_safelen(var_na *str, size_t str_m, const var_na *fmt, ...)
{
    va_list ap;
    පූර්ණ_සංඛ්‍යා	    str_l;

    නම් (str_m == 0)
	ආපසු 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
    va_end(ap);

    නම් (str_l < 0)
    {
	*str = NUL;
	ආපසු 0;
    }
    ආපසු ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    පූර්ණ_සංඛ්‍යා
vim_vsnprintf(
    var_na	*str,
    size_t	str_m,
    const var_na	*fmt,
    va_list	ap)
{
    ආපසු vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
}

ga_nanaa
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
    static පූර්ණ_සංඛ්‍යා
format_typeof(
    const var_na	*type)
{
    // allowed values: \0, h, l, L
    var_na    length_modifier = '\0';

    // current conversion specifier character
    var_na    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    නම් (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	නම් (length_modifier == 'l' && *type == 'l')
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
	pak_sa 'i': fmt_spec = 'd'; bha_nga;
	pak_sa '*': fmt_spec = 'd'; length_modifier = 'h'; bha_nga;
	pak_sa 'D': fmt_spec = 'd'; length_modifier = 'l'; bha_nga;
	pak_sa 'U': fmt_spec = 'u'; length_modifier = 'l'; bha_nga;
	pak_sa 'O': fmt_spec = 'o'; length_modifier = 'l'; bha_nga;
	default: bha_nga;
    }

    // get parameter value, do initial processing
    switch (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    pak_sa '%':
	ආපසු TYPE_PERCENT;

    pak_sa 'c':
	ආපසු TYPE_CHAR;

    pak_sa 's':
    pak_sa 'S':
	ආපසු TYPE_STRING;

    pak_sa 'd': pak_sa 'u':
    pak_sa 'b': pak_sa 'B':
    pak_sa 'o':
    pak_sa 'x': pak_sa 'X':
    pak_sa 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    // value

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    නම් (fmt_spec == 'p')
		ආපසු TYPE_POINTER;
	    නැතහොත් නම් (fmt_spec == 'b' || fmt_spec == 'B')
		ආපසු TYPE_UNSIGNEDLONGLONGINT;
	    නැතහොත් නම් (fmt_spec == 'd')
	    {
		// signed
		switch (length_modifier)
		{
		pak_sa '\0':
		pak_sa 'h':
		    // char and short arguments are passed as int.
		    ආපසු TYPE_INT;
		pak_sa 'l':
		    ආපසු TYPE_LONGINT;
		pak_sa 'L':
		    ආපසු TYPE_LONGLONGINT;
		}
	    }
	    නැතහොත්
	    {
		// unsigned
		switch (length_modifier)
		{
		    pak_sa '\0':
		    pak_sa 'h':
			ආපසු TYPE_UNSIGNEDINT;
		    pak_sa 'l':
			ආපසු TYPE_UNSIGNEDLONGINT;
		    pak_sa 'L':
			ආපසු TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	bha_nga;

    pak_sa 'f':
    pak_sa 'F':
    pak_sa 'e':
    pak_sa 'E':
    pak_sa 'g':
    pak_sa 'G':
	ආපසු TYPE_FLOAT;
    }

    ආපසු TYPE_UNKNOWN;
}

    static var_na *
format_typename(
    const var_na  *type)
{
    switch (format_typeof(type))
    {
	pak_sa TYPE_INT:
	    ආපසු typename_int;

	pak_sa TYPE_LONGINT:
	    ආපසු typename_longint;

	pak_sa TYPE_LONGLONGINT:
	    ආපසු typename_longlongint;

	pak_sa TYPE_UNSIGNEDINT:
	    ආපසු typename_unsignedint;

	pak_sa TYPE_UNSIGNEDLONGINT:
	    ආපසු typename_unsignedlongint;

	pak_sa TYPE_UNSIGNEDLONGLONGINT:
	    ආපසු typename_unsignedlonglongint;

	pak_sa TYPE_POINTER:
	    ආපසු _(typename_pointer);

	pak_sa TYPE_PERCENT:
	    ආපසු _(typename_percent);

	pak_sa TYPE_CHAR:
	    ආපසු typename_char;

	pak_sa TYPE_STRING:
	    ආපසු _(typename_string);

	pak_sa TYPE_FLOAT:
	    ආපසු typename_float;
    }

    ආපසු _(typename_unknown);
}

    static පූර්ණ_සංඛ්‍යා
adjust_types(
    const var_na ***ap_types,
    පූර්ණ_සංඛ්‍යා arg,
    පූර්ණ_සංඛ්‍යා *num_posarg,
    const var_na *type)
{
    නම් (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	ආපසු FAIL;
    }

    නම් (*ap_types == NULL || *num_posarg < arg)
    {
	පූර්ණ_සංඛ්‍යා	    idx;
	const var_na  **new_types;

	නම් (*ap_types == NULL)
	    new_types = ALLOC_CLEAR_MULT(const var_na *, arg);
	නැතහොත්
	    new_types = vim_realloc((var_na **)*ap_types,
						arg * parimaa_na(const var_na *));

	නම් (new_types == NULL)
	    ආපසු FAIL;

	සඳහා (idx = *num_posarg; idx < arg; ++idx)
	    new_types[idx] = NULL;

	*ap_types = new_types;
	*num_posarg = arg;
    }

    නම් ((*ap_types)[arg - 1] != NULL)
    {
	නම් ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    const var_na *pt = type;
	    නම් (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    නම් (pt[0] != '*')
	    {
		switch (pt[0])
		{
		    pak_sa 'd':
		    pak_sa 'i': bha_nga;
		    default:
			semsg(_(e_positional_num_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			ආපසු FAIL;
		}
	    }
	}
	නැතහොත්
	{
	    නම් (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_num_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		ආපසු FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    ආපසු OK;
}

    static void
format_overflow_error(const var_na *pstart)
{
    size_t	arglen = 0;
    var_na	*argcopy = NULL;
    const var_na	*p = pstart;

    අතරතුර (VIM_ISDIGIT((පූර්ණ_සංඛ්‍යා)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(var_na, arglen + 1);
    නම් (argcopy != NULL)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	free(argcopy);
    }
    නැතහොත්
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    static පූර්ණ_සංඛ්‍යා
get_unsigned_int(
    const var_na *pstart,
    const var_na **p,
    unsigned පූර්ණ_සංඛ්‍යා *uj,
    පූර්ණ_සංඛ්‍යා overflow_err)
{
    *uj = **p - '0';
    ++*p;

    අතරතුර (VIM_ISDIGIT((පූර්ණ_සංඛ්‍යා)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned පූර්ණ_සංඛ්‍යා)(**p - '0');
	++*p;
    }

    නම් (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	නම් (overflow_err)
	{
	    format_overflow_error(pstart);
	    ආපසු FAIL;
	}
	නැතහොත්
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    ආපසු OK;
}


    static පූර්ණ_සංඛ්‍යා
parse_fmt_types(
    const var_na  ***ap_types,
    පූර්ණ_සංඛ්‍යා		*num_posarg,
    const var_na  *fmt,
    typval_T	*tvs UNUSED
    )
{
    const var_na	*p = fmt;
    const var_na	*arg = NULL;

    පූර්ණ_සංඛ්‍යා		any_pos = 0;
    පූර්ණ_සංඛ්‍යා		any_arg = 0;
    පූර්ණ_සංඛ්‍යා		arg_idx;

# define CHECK_POS_ARG do { \
    නම් (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	goto error; \
    } \
} අතරතුර (0);

    නම් (p == NULL)
	ආපසු OK;

    අතරතුර (*p != NUL)
    {
	නම් (*p != '%')
	{
	    const var_na    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	නැතහොත්
	{
	    // allowed values: \0, h, l, L
	    var_na	length_modifier = '\0';

	    // variable for positional arg
	    පූර්ණ_සංඛ්‍යා		pos_arg = -1;
	    const var_na	*ptype = NULL;
	    const var_na	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    අතරතුර (VIM_ISDIGIT(*ptype))
		++ptype;

	    නම් (*ptype == '$')
	    {
		නම් (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}

		// Positional argument
		unsigned පූර්ණ_සංඛ්‍යා uj;

		නම් (get_unsigned_int(pstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    අතරතුර (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		switch (*p)
		{
		    pak_sa '0': bha_nga;
		    pak_sa '-': bha_nga;
		    pak_sa '+': bha_nga;
		    pak_sa ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      bha_nga;
		    pak_sa '#': bha_nga;
		    pak_sa '\'': bha_nga;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    නම් (*(arg = p) == '*')
	    {
		p++;

		නම් (VIM_ISDIGIT((පූර්ණ_සංඛ්‍යා)(*p)))
		{
		    // Positional argument field width
		    unsigned පූර්ණ_සංඛ්‍යා uj;

		    නම් (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    නම් (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		    නැතහොත්
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			නම් (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
			    goto error;
		    }
		}
		නැතහොත්
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    නැතහොත් නම් (VIM_ISDIGIT((පූර්ණ_සංඛ්‍යා)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const var_na *digstart = p;
		unsigned පූර්ණ_සංඛ්‍යා uj;

		නම් (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		නම් (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}
	    }

	    // parse precision
	    නම් (*p == '.')
	    {
		p++;

		නම් (*(arg = p) == '*')
		{
		    p++;

		    නම් (VIM_ISDIGIT((පූර්ණ_සංඛ්‍යා)(*p)))
		    {
			// Parse precision
			unsigned පූර්ණ_සංඛ්‍යා uj;

			නම් (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			නම් (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    නම් (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
				goto error;
			}
			නැතහොත්
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    goto error;
			}
		    }
		    නැතහොත්
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		නැතහොත් නම් (VIM_ISDIGIT((පූර්ණ_සංඛ්‍යා)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const var_na *digstart = p;
		    unsigned පූර්ණ_සංඛ්‍යා uj;

		    නම් (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    නම් (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		}
	    }

	    නම් (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    නම් (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		නම් (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    switch (*p)
	    {
		// Check for known format specifiers. % is special!
		pak_sa 'i':
		pak_sa '*':
		pak_sa 'd':
		pak_sa 'u':
		pak_sa 'o':
		pak_sa 'D':
		pak_sa 'U':
		pak_sa 'O':
		pak_sa 'x':
		pak_sa 'X':
		pak_sa 'b':
		pak_sa 'B':
		pak_sa 'c':
		pak_sa 's':
		pak_sa 'S':
		pak_sa 'p':
		pak_sa 'f':
		pak_sa 'F':
		pak_sa 'e':
		pak_sa 'E':
		pak_sa 'g':
		pak_sa 'G':
		    නම් (pos_arg != -1)
		    {
			නම් (adjust_types(ap_types, pos_arg, num_posarg, ptype) == FAIL)
			    goto error;
		    }
		    නැතහොත්
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    bha_nga;

		default:
		    නම් (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			goto error;
		    }
	    }

	    නම් (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    සඳහා (arg_idx = 0; arg_idx < *num_posarg; ++arg_idx)
    {
	නම් ((*ap_types)[arg_idx] == NULL)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    goto error;
	}

# නම් defined(FEAT_EVAL)
	නම් (tvs != NULL && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    goto error;
	}
# endif
    }

    ආපසු OK;

error:
    vim_free((var_na**)*ap_types);
    *ap_types = NULL;
    *num_posarg = 0;
    ආපසු FAIL;
}

    static void
skip_to_arg(
    const var_na	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    පූර්ණ_සංඛ්‍යා		*arg_idx,
    පූර්ණ_සංඛ්‍යා		*arg_cur,
    const var_na	*fmt)
{
    පූර්ණ_සංඛ්‍යා		arg_min = 0;

    නම් (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	ආපසු;
    }

    නම් (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    නැතහොත්
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    සඳහා (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	const var_na *p;

	නම් (ap_types == NULL || ap_types[*arg_cur] == NULL)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    ආපසු;
	}

	p = ap_types[*arg_cur];

	පූර්ණ_සංඛ්‍යා fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	switch (fmt_type)
	{
	pak_sa TYPE_PERCENT:
	pak_sa TYPE_UNKNOWN:
	    bha_nga;

	pak_sa TYPE_CHAR:
	    va_arg(*ap, පූර්ණ_සංඛ්‍යා);
	    bha_nga;

	pak_sa TYPE_STRING:
	    va_arg(*ap, var_na *);
	    bha_nga;

	pak_sa TYPE_POINTER:
	    va_arg(*ap, void *);
	    bha_nga;

	pak_sa TYPE_INT:
	    va_arg(*ap, පූර්ණ_සංඛ්‍යා);
	    bha_nga;

	pak_sa TYPE_LONGINT:
	    va_arg(*ap, long පූර්ණ_සංඛ්‍යා);
	    bha_nga;

	pak_sa TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    bha_nga;

	pak_sa TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned පූර්ණ_සංඛ්‍යා);
	    bha_nga;

	pak_sa TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long පූර්ණ_සංඛ්‍යා);
	    bha_nga;

	pak_sa TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    bha_nga;

	pak_sa TYPE_FLOAT:
	    va_arg(*ap, dviguu_na);
	    bha_nga;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    ආපසු;
}

    පූර්ණ_සංඛ්‍යා
vim_vsnprintf_typval(
    var_na	*str,
    size_t	str_m,
    const var_na	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    const var_na	*p = fmt;
    පූර්ණ_සංඛ්‍යා		arg_cur = 0;
    පූර්ණ_සංඛ්‍යා		num_posarg = 0;
    පූර්ණ_සංඛ්‍යා		arg_idx = 1;
    va_list	ap;
    const var_na	**ap_types = NULL;

    නම් (parse_fmt_types(&ap_types, &num_posarg, fmt, tvs) == FAIL)
	ආපසු 0;

    va_copy(ap, ap_start);

    නම් (p == NULL)
	p = "";
    අතරතුර (*p != NUL)
    {
	නම් (*p != '%')
	{
	    const var_na    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    නම් (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	නැතහොත්
	{
	    size_t  min_field_width = 0, precision = 0;
	    පූර්ණ_සංඛ්‍යා	    zero_padding = 0, precision_specified = 0, justify_left = 0;
	    පූර්ණ_සංඛ්‍යා	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    පූර්ණ_සංඛ්‍යා	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    var_na    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    var_na    tmp[TMP_LEN];

	    // string address in case of string argument
	    const var_na  *str_arg = NULL;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    unsigned var_na uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  number_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    var_na    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = NULL;

	    // variables for positional arg
	    පූර්ණ_සංඛ්‍යා	    pos_arg = -1;
	    const var_na	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    අතරතුර (VIM_ISDIGIT(*ptype))
		++ptype;

	    නම් (*ptype == '$')
	    {
		// Positional argument
		const var_na *digstart = p;
		unsigned පූර්ණ_සංඛ්‍යා uj;

		නම් (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    අතරතුර (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		switch (*p)
		{
		    pak_sa '0': zero_padding = 1; bha_nga;
		    pak_sa '-': justify_left = 1; bha_nga;
		    pak_sa '+': force_sign = 1; space_for_positive = 0; bha_nga;
		    pak_sa ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      bha_nga;
		    pak_sa '#': alternate_form = 1; bha_nga;
		    pak_sa '\'': bha_nga;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    නම් (*p == '*')
	    {
		පූර්ණ_සංඛ්‍යා j;
		const var_na *digstart = p + 1;

		p++;

		නම් (VIM_ISDIGIT((පූර්ණ_සංඛ්‍යා)(*p)))
		{
		    // Positional argument field width
		    unsigned පූර්ණ_සංඛ්‍යා uj;

		    නම් (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# නම් defined(FEAT_EVAL)
		    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, පූර්ණ_සංඛ්‍යා));

		නම් (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    නම් (tvs != NULL)
		    {
			format_overflow_error(digstart);
			goto error;
		    }
		    නැතහොත්
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		නම් (j >= 0)
		    min_field_width = j;
		නැතහොත්
		{
		    min_field_width = -j;
		    justify_left = 1;
		}
	    }
	    නැතහොත් නම් (VIM_ISDIGIT((පූර්ණ_සංඛ්‍යා)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const var_na *digstart = p;
		unsigned පූර්ණ_සංඛ්‍යා uj;

		නම් (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		min_field_width = uj;
	    }

	    // parse precision
	    නම් (*p == '.')
	    {
		p++;
		precision_specified = 1;

		නම් (VIM_ISDIGIT((පූර්ණ_සංඛ්‍යා)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const var_na *digstart = p;
		    unsigned පූර්ණ_සංඛ්‍යා uj;

		    නම් (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    precision = uj;
		}
		නැතහොත් නම් (*p == '*')
		{
		    පූර්ණ_සංඛ්‍යා j;
		    const var_na *digstart = p;

		    p++;

		    නම් (VIM_ISDIGIT((පූර්ණ_සංඛ්‍යා)(*p)))
		    {
			// positional argument
			unsigned පූර්ණ_සංඛ්‍යා uj;

			නම් (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			arg_idx = uj;

			++p;
		    }

		    j =
# නම් defined(FEAT_EVAL)
			tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, පූර්ණ_සංඛ්‍යා));

		    නම් (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			නම් (tvs != NULL)
			{
			    format_overflow_error(digstart);
			    goto error;
			}
			නැතහොත්
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    නම් (j >= 0)
			precision = j;
		    නැතහොත්
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    නම් (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		නම් (length_modifier == 'l' && *p == 'l')
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
		pak_sa 'i': fmt_spec = 'd'; bha_nga;
		pak_sa 'D': fmt_spec = 'd'; length_modifier = 'l'; bha_nga;
		pak_sa 'U': fmt_spec = 'u'; length_modifier = 'l'; bha_nga;
		pak_sa 'O': fmt_spec = 'o'; length_modifier = 'l'; bha_nga;
		default: bha_nga;
	    }

# නම් defined(FEAT_EVAL)
	    switch (fmt_spec)
	    {
		pak_sa 'd': pak_sa 'u': pak_sa 'o': pak_sa 'x': pak_sa 'X':
		    නම් (tvs != NULL && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    නම් (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    switch (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    pak_sa '%':
	    pak_sa 'c':
	    pak_sa 's':
	    pak_sa 'S':
		str_arg_l = 1;
		switch (fmt_spec)
		{
		pak_sa '%':
		    str_arg = p;
		    bha_nga;

		pak_sa 'c':
		    {
			පූර්ණ_සංඛ්‍යා j;

			j =
# නම් defined(FEAT_EVAL)
			    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, පූර්ණ_සංඛ්‍යා));

			// standard demands unsigned char
			uchar_arg = (unsigned var_na)j;
			str_arg = (var_na *)&uchar_arg;
			bha_nga;
		    }

		pak_sa 's':
		pak_sa 'S':
		    str_arg =
# නම් defined(FEAT_EVAL)
				tvs != NULL ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, var_na *));

		    නම් (str_arg == NULL)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    නැතහොත් නම් (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    නැතහොත් නම් (precision == 0)
			str_arg_l = 0;
		    නැතහොත්
		    {
			// memchr on HP does not like n > 2^31  !!!
			const var_na *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == NULL) ? precision
						      : (size_t)(q - str_arg);
		    }
		    නම් (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			පූර්ණ_සංඛ්‍යා	cell;

			සඳහා (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    නම් (precision_specified && i + cell > precision)
				bha_nga;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			නම් (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    bha_nga;

		default:
		    bha_nga;
		}
		bha_nga;

	    pak_sa 'd': pak_sa 'u':
	    pak_sa 'b': pak_sa 'B':
	    pak_sa 'o':
	    pak_sa 'x': pak_sa 'X':
	    pak_sa 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    // value

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    පූර්ණ_සංඛ්‍යා arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    පූර්ණ_සංඛ්‍යා int_arg = 0;
		    unsigned පූර්ණ_සංඛ්‍යා uint_arg = 0;

		    // only set for length modifier l
		    long පූර්ණ_සංඛ්‍යා long_arg = 0;
		    unsigned long පූර්ණ_සංඛ්‍යා ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    void *ptr_arg = NULL;

		    නම් (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# නම් defined(FEAT_EVAL)
				 tvs != NULL ? (void *)tv_str(tvs, &arg_idx,
									NULL) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, void *));

			නම් (ptr_arg != NULL)
			    arg_sign = 1;
		    }
		    නැතහොත් නම් (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# නම් defined(FEAT_EVAL)
				    tvs != NULL ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			නම් (bin_arg != 0)
			    arg_sign = 1;
		    }
		    නැතහොත් නම් (fmt_spec == 'd')
		    {
			// signed
			switch (length_modifier)
			{
			pak_sa '\0':
			pak_sa 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# නම් defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, පූර්ණ_සංඛ්‍යා));

			    නම් (int_arg > 0)
				arg_sign =  1;
			    නැතහොත් නම් (int_arg < 0)
				arg_sign = -1;
			    bha_nga;
			pak_sa 'l':
			    long_arg =
# නම් defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long පූර්ණ_සංඛ්‍යා));

			    නම් (long_arg > 0)
				arg_sign =  1;
			    නැතහොත් නම් (long_arg < 0)
				arg_sign = -1;
			    bha_nga;
			pak_sa 'L':
			    llong_arg =
# නම් defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    නම් (llong_arg > 0)
				arg_sign =  1;
			    නැතහොත් නම් (llong_arg < 0)
				arg_sign = -1;
			    bha_nga;
			}
		    }
		    නැතහොත්
		    {
			// unsigned
			switch (length_modifier)
			{
			    pak_sa '\0':
			    pak_sa 'h':
				uint_arg =
# නම් defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned පූර්ණ_සංඛ්‍යා));

				නම් (uint_arg != 0)
				    arg_sign = 1;
				bha_nga;
			    pak_sa 'l':
				ulong_arg =
# නම් defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long පූර්ණ_සංඛ්‍යා));

				නම් (ulong_arg != 0)
				    arg_sign = 1;
				bha_nga;
			    pak_sa 'L':
				ullong_arg =
# නම් defined(FEAT_EVAL)
					    tvs != NULL ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				නම් (ullong_arg != 0)
				    arg_sign = 1;
				bha_nga;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    නම් (precision_specified)
			zero_padding = 0;
		    නම් (fmt_spec == 'd')
		    {
			නම් (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    නැතහොත් නම් (alternate_form)
		    {
			නම් (arg_sign != 0
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
		    නම් (!precision_specified)
			precision = 1;   // default precision is 1
		    නම් (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    නැතහොත්
		    {
			var_na	f[6];
			පූර්ණ_සංඛ්‍යා	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			නම් (!length_modifier)
			    ;
			නැතහොත් නම් (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# නැතහොත්
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			නැතහොත්
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			නම් (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			නැතහොත් නම් (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    var_na	    b[8 * parimaa_na(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    do
			    {
				b[parimaa_na(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    අතරතුර (bn != 0);

			    memcpy(tmp + str_arg_l, b + parimaa_na(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			නැතහොත් නම් (fmt_spec == 'd')
			{
			    // signed
			    switch (length_modifier)
			    {
			    pak_sa '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       bha_nga;
			    pak_sa 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (short)int_arg);
				      bha_nga;
			    pak_sa 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      bha_nga;
			    pak_sa 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      bha_nga;
			    }
			}
			නැතහොත්
			{
			    // unsigned
			    switch (length_modifier)
			    {
			    pak_sa '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       bha_nga;
			    pak_sa 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(unsigned short)uint_arg);
				      bha_nga;
			    pak_sa 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      bha_nga;
			    pak_sa 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      bha_nga;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			නම් (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			නම් (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t num_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			නම් (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    නම් (!precision_specified
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
			නම් (num_of_digits < precision)
			    number_of_zeros_to_pad = precision - num_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    නම් (!justify_left && zero_padding)
		    {
			පූර්ණ_සංඛ්‍යා n = (පූර්ණ_සංඛ්‍යා)(min_field_width - (str_arg_l
						    + number_of_zeros_to_pad));
			නම් (n > 0)
			    number_of_zeros_to_pad += n;
		    }
		    bha_nga;
		}

	    pak_sa 'f':
	    pak_sa 'F':
	    pak_sa 'e':
	    pak_sa 'E':
	    pak_sa 'g':
	    pak_sa 'G':
		{
		    // Floating point.
		    dviguu_na	f;
		    dviguu_na	abs_f;
		    var_na	format[40];
		    පූර්ණ_සංඛ්‍යා		l;
		    පූර්ණ_සංඛ්‍යා		remove_trailing_zeroes = FALSE;

		    f =
# නම් defined(FEAT_EVAL)
			tvs != NULL ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, dviguu_na));

		    abs_f = f < 0 ? -f : f;

		    නම් (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			නම් ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			නැතහොත්
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    නම් ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# නැතහොත්
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
		    නැතහොත්
		    {
			නම් (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			නැතහොත් නම් (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			නැතහොත්
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    නම් (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    නම් (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				නම් ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				නම් (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (පූර්ණ_සංඛ්‍යා)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			නම් (remove_trailing_zeroes)
			{
			    පූර්ණ_සංඛ්‍යා i;
			    var_na *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    නම් (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    නැතහොත්
			    {
				tp = (var_na *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				නම් (tp != NULL)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    නම් (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    අතරතුර (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    නම් (tp != NULL && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				අතරතුර (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			නැතහොත්
			{
			    var_na *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (var_na *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    නම් (tp != NULL && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    නම් (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			number_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    bha_nga;
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
		නම් (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		bha_nga;
	    }

	    නම් (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    නම් (!justify_left)
	    {
		// left padding with blank or zero
		පූර්ණ_සංඛ්‍යා pn = (පූර්ණ_සංඛ්‍යා)(min_field_width - (str_arg_l + number_of_zeros_to_pad));

		නම් (pn > 0)
		{
		    නම් (str_l < str_m)
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
	    නම් (number_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    නැතහොත්
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		පූර්ණ_සංඛ්‍යා zn = (පූර්ණ_සංඛ්‍යා)zero_padding_insertion_ind;

		නම් (zn > 0)
		{
		    නම් (str_l < str_m)
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
		zn = (පූර්ණ_සංඛ්‍යා)number_of_zeros_to_pad;
		නම් (zn > 0)
		{
		    නම් (str_l < str_m)
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
		පූර්ණ_සංඛ්‍යා sn = (පූර්ණ_සංඛ්‍යා)(str_arg_l - zero_padding_insertion_ind);

		නම් (sn > 0)
		{
		    නම් (str_l < str_m)
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
	    නම් (justify_left)
	    {
		// right blank padding to the field width
		පූර්ණ_සංඛ්‍යා pn = (පූර්ණ_සංඛ්‍යා)(min_field_width
				      - (str_arg_l + number_of_zeros_to_pad));

		නම් (pn > 0)
		{
		    නම් (str_l < str_m)
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

    නම් (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    නම් (tvs != NULL && tvs[num_posarg != 0 ? num_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((var_na*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    ආපසු (පූර්ණ_සංඛ්‍යා)str_l;
}

#endif // PROTO
