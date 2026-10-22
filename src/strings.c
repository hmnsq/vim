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
#অন্তর্ভাব "vim.h"

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
    যদি (p != ফাঁকা)
	mch_memmove(p, string, len);
    ফেরত p;
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
    যদি (p == ফাঁকা)
	ফেরত ফাঁকা;

    STRNCPY(p, string, len);
    p[len] = NUL;
    ফেরত p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    ফেরত vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    পূর্ণসংখ্যা		cc,
    পূর্ণসংখ্যা		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	দৈর্ঘ্য;
    পূর্ণসংখ্যা		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    দৈর্ঘ্য = 1;				// count the trailing NUL
    জন্য (p = string; *p; p++)
    {
	যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    দৈর্ঘ্য += l;		// count a multibyte char
	    p += l - 1;
	    চালু;
	}
	যদি (vim_strchr(esc_chars, *p) != ফাঁকা || (bsl && rem_backslash(p)))
	    ++দৈর্ঘ্য;			// count a backslash
	++দৈর্ঘ্য;			// count an ordinary char
    }
    escaped_string = alloc(দৈর্ঘ্য);
    যদি (escaped_string == ফাঁকা)
	ফেরত ফাঁকা;
    p2 = escaped_string;
    জন্য (p = string; *p; p++)
    {
	যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    চালু;
	}
	যদি (vim_strchr(esc_chars, *p) != ফাঁকা || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    ফেরত escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    পূর্ণসংখ্যা
csh_like_shell(শূন্য)
{
    ফেরত (strstr((অক্ষর *)gettail(p_sh), "csh") != ফাঁকা);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    স্থির পূর্ণসংখ্যা
fish_like_shell(শূন্য)
{
    ফেরত (strstr((অক্ষর *)gettail(p_sh), "fish") != ফাঁকা);
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
vim_strsave_shellescape(char_u *string, পূর্ণসংখ্যা do_special, পূর্ণসংখ্যা do_newline)
{
    unsigned	দৈর্ঘ্য;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    পূর্ণসংখ্যা		csh_like;
    পূর্ণসংখ্যা		fish_like;
    char_u	*shname;
    পূর্ণসংখ্যা		powershell;
#ifdef MSWIN
    পূর্ণসংখ্যা		double_quotes;
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
    powershell = strstr((অক্ষর *)shname, "pwsh") != ফাঁকা;
#ifdef MSWIN
    powershell = powershell || strstr((অক্ষর *)shname, "powershell") != ফাঁকা;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    দৈর্ঘ্য = (unsigned)STRLEN(string) + 3;  // two quotes and a trailing NUL
    জন্য (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	যদি (double_quotes)
	{
	    যদি (*p == '"')
		++দৈর্ঘ্য;		// " -> ""
	}
	অন্যথা
#endif
	যদি (*p == '\'')
	{
	    যদি (powershell)
		দৈর্ঘ্য += 2;		// ' => ''
	    অন্যথা
		দৈর্ঘ্য += 3;		// ' => '\''
	}
	যদি ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++দৈর্ঘ্য;			// insert backslash
	    যদি (csh_like && do_special)
		++দৈর্ঘ্য;		// insert backslash
	}
	যদি (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++দৈর্ঘ্য;			// insert backslash
	    p += l - 1;
	}
	যদি (*p == '\\' && fish_like)
	    ++দৈর্ঘ্য;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(দৈর্ঘ্য);
    যদি (escaped_string != ফাঁকা)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	যদি (double_quotes)
	    *d++ = '"';
	অন্যথা
#endif
	    *d++ = '\'';

	জন্য (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    যদি (double_quotes)
	    {
		যদি (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    চালু;
		}
	    }
	    অন্যথা
#endif
	    যদি (*p == '\'')
	    {
		যদি (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		অন্যথা
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		চালু;
	    }
	    যদি ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		যদি (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		চালু;
	    }
	    যদি (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		চালু;
	    }
	    যদি (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		চালু;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	যদি (double_quotes)
	    *d++ = '"';
	অন্যথা
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    ফেরত escaped_string;
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
    ফেরত p1;
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
    ফেরত p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    শূন্য
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    পূর্ণসংখ্যা	    c;

    যদি (p == ফাঁকা)
	ফেরত;

    p2 = p;
    যখন ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#যদি defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    স্থির char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    যদি (res != ফাঁকা)
	যখন (*p != NUL)
	{
	    পূর্ণসংখ্যা		l;

	    যদি (enc_utf8)
	    {
		পূর্ণসংখ্যা	c, uc;
		পূর্ণসংখ্যা	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		যদি (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		যদি (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    যদি (s == ফাঁকা)
		    {
			vim_free(res);
			ফেরত ফাঁকা;
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
	    অন্যথা যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    অন্যথা
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    ফেরত res;
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

    যদি (res != ফাঁকা)
	যখন (*p != NUL)
	{
	    পূর্ণসংখ্যা		l;

	    যদি (enc_utf8)
	    {
		পূর্ণসংখ্যা	c, lc;
		পূর্ণসংখ্যা	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		যদি (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		যদি (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    যদি (s == ফাঁকা)
		    {
			vim_free(res);
			ফেরত ফাঁকা;
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
	    অন্যথা যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    অন্যথা
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    ফেরত res;
}
#endif

/*
 * delete spaces at the end of a string
 */
    শূন্য
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    যখন (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    শূন্য
vim_strncpy(char_u *to, char_u *from, size_t len)
{
    STRNCPY(to, from, len);
    to[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    শূন্য
vim_strcat(char_u *to, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(to);
    size_t fromlen = STRLEN(from);

    যদি (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    অন্যথা
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(অক্ষর *s, size_t maxlen)
{
    size_t i;
    জন্য (i = 0; i < maxlen; ++i)
	যদি (s[i] == NUL)
	    বিরতি;
    ফেরত i;
}

#যদি !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    পূর্ণসংখ্যা
vim_stricmp(অক্ষর *s1, অক্ষর *s2)
{
    পূর্ণসংখ্যা		i;

    জন্য (;;)
    {
	i = (পূর্ণসংখ্যা)TOLOWER_LOC(*s1) - (পূর্ণসংখ্যা)TOLOWER_LOC(*s2);
	যদি (i != 0)
	    ফেরত i;			    // this character different
	যদি (*s1 == NUL)
	    বিরতি;			    // strings match until NUL
	++s1;
	++s2;
    }
    ফেরত 0;				    // strings match
}
#endif

#যদি !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    পূর্ণসংখ্যা
vim_strnicmp(অক্ষর *s1, অক্ষর *s2, size_t len)
{
    পূর্ণসংখ্যা		i;

    যখন (len > 0)
    {
	i = (পূর্ণসংখ্যা)TOLOWER_LOC(*s1) - (পূর্ণসংখ্যা)TOLOWER_LOC(*s2);
	যদি (i != 0)
	    ফেরত i;			    // this character different
	যদি (*s1 == NUL)
	    বিরতি;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ফেরত 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    পূর্ণসংখ্যা
vim_strnicmp_asc(অক্ষর *s1, অক্ষর *s2, size_t len)
{
    পূর্ণসংখ্যা                i = 0;

    যখন (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	যদি (i != 0)
	    বিরতি;			// this character is different
	যদি (*s1 == NUL)
	    বিরতি;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ফেরত i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, পূর্ণসংখ্যা c)
{
    char_u	*p;
    পূর্ণসংখ্যা		b;

    p = string;
    যদি (enc_utf8 && c > 0 && c < 0x80)
	ফেরত vim_strbyte(string, c);
    যদি (enc_utf8 && c >= 0x80)
    {
	যখন (*p != NUL)
	{
	    পূর্ণসংখ্যা l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    যদি (utf_ptr2char(p) == c && l > 1)
		ফেরত p;
	    p += l;
	}
	ফেরত ফাঁকা;
    }
    যদি (enc_dbcs != 0 && c > 255)
    {
	পূর্ণসংখ্যা	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	যখন ((b = *p) != NUL)
	{
	    যদি (b == c && p[1] == n2)
		ফেরত p;
	    p += (*mb_ptr2len)(p);
	}
	ফেরত ফাঁকা;
    }
    যদি (has_mbyte)
    {
	যখন ((b = *p) != NUL)
	{
	    যদি (b == c)
		ফেরত p;
	    p += (*mb_ptr2len)(p);
	}
	ফেরত ফাঁকা;
    }
    যখন ((b = *p) != NUL)
    {
	যদি (b == c)
	    ফেরত p;
	++p;
    }
    ফেরত ফাঁকা;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, পূর্ণসংখ্যা c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    যদি (c <= 0 || c > 255)
	ফেরত ফাঁকা;
    ফেরত (char_u *)strchr((অক্ষর *)string, c);
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
vim_strrchr(char_u *string, পূর্ণসংখ্যা c)
{
    char_u	*retval = ফাঁকা;
    char_u	*p = string;

    যখন (*p)
    {
	যদি (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    ফেরত retval;
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
    যখন (*s)
    {
	যদি (vim_strchr(charset, *s) != ফাঁকা)
	    ফেরত s;
	MB_PTR_ADV(s);
    }
    ফেরত ফাঁকা;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
স্থির পূর্ণসংখ্যা sort_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2);

    স্থির পূর্ণসংখ্যা
sort_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    ফেরত STRCMP(*(অক্ষর **)s1, *(অক্ষর **)s2);
}

    শূন্য
sort_strings(
    char_u	**files,
    পূর্ণসংখ্যা		গুনতি)
{
    qsort((শূন্য *)files, (size_t)গুনতি, আকার(char_u *), sort_compare);
}

#যদি defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    পূর্ণসংখ্যা
has_non_ascii(char_u *s)
{
    char_u	*p;

    যদি (s != ফাঁকা)
	জন্য (p = s; *p != NUL; ++p)
	    যদি (*p >= 128)
		ফেরত TRUE;
    ফেরত FALSE;
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
    size_t  l = str1 == ফাঁকা ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == ফাঁকা ? 0 : STRLEN(str2)) + 1L);
    যদি (dest == ফাঁকা)
	ফেরত ফাঁকা;
    যদি (str1 == ফাঁকা)
	*dest = NUL;
    অন্যথা
	STRCPY(dest, str1);
    যদি (str2 != ফাঁকা)
	STRCPY(dest + l, str2);
    ফেরত dest;
}

#যদি defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    যদি (rev == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	যদি (has_mbyte)
	{
	    পূর্ণসংখ্যা mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	অন্যথা
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    ফেরত rev;
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, পূর্ণসংখ্যা function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    যদি (str != ফাঁকা)
    {
	len += (unsigned)STRLEN(str);
	জন্য (p = str; *p != NUL; MB_PTR_ADV(p))
	    যদি (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    যদি (r == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    অন্যথা
	*r++ = '\'';
    যদি (str != ফাঁকা)
	জন্য (p = str; *p != NUL; )
	{
	    যদি (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    যদি (function)
	*r++ = ')';
    *r++ = NUL;
    ফেরত s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_গুনতি(char_u *haystack, char_u *needle, পূর্ণসংখ্যা ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*পরবর্তী;

    যদি (p == ফাঁকা || needle == ফাঁকা || *needle == NUL)
	ফেরত 0;

    size_t  needlelen = STRLEN(needle);
    যদি (ic)
    {
	যখন (*p != NUL)
	{
	    যদি (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    অন্যথা
		MB_PTR_ADV(p);
	}
    }
    অন্যথা
	যখন ((পরবর্তী = (char_u *)strstr((অক্ষর *)p, (অক্ষর *)needle)) != ফাঁকা)
	{
	    ++n;
	    p = পরবর্তী + needlelen;
	}

    ফেরত n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    স্থির পূর্ণসংখ্যা
copy_first_char_to_tv(char_u *প্রবেশ, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    পূর্ণসংখ্যা		len;

    যদি (প্রবেশ == ফাঁকা || output == ফাঁকা)
	ফেরত -1;

    len = has_mbyte ? mb_ptr2len(প্রবেশ) : 1;
    STRNCPY(buf, প্রবেশ, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    ফেরত output->vval.v_string == ফাঁকা ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    শূন্য
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*রাশিমালা,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    পূর্ণসংখ্যা		len = 0;
    পূর্ণসংখ্যা		idx = 0;
    পূর্ণসংখ্যা		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_চাবি, VAR_সংখ্যা);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_রাশিমালা_get_funccal(রাশিমালা, &newtv);

    ga_init2(&ga, আকার(অক্ষর), 80);
    জন্য (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	যদি (len < 0)
	    বিরতি;

	set_vim_var_nr(VV_চাবি, idx);
	যদি (filter_map_one(&tv, রাশিমালা, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    বিরতি;
	}
	যদি (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    যদি (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		বিরতি;
	    }
	    অন্যথা
		ga_concat(&ga, newtv.vval.v_string);
	}
	অন্যথা যদি (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_তথ্য;
    যদি (fc != ফাঁকা)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    শূন্য
string_reduce(
	typval_T	*argvars,
	typval_T	*রাশিমালা,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    পূর্ণসংখ্যা		len;
    typval_T	argv[3];
    পূর্ণসংখ্যা		r;
    পূর্ণসংখ্যা		called_emsg_start = called_emsg;
    funccall_T	*fc;

    যদি (argvars[2].v_type == VAR_UNKNOWN)
    {
	যদি (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_মান), "String");
	    ফেরত;
	}
	len = copy_first_char_to_tv(p, rettv);
	যদি (len < 0)
	    ফেরত;
	p += len;
    }
    অন্যথা যদি (check_for_string_arg(argvars, 2) == FAIL)
	ফেরত;
    অন্যথা
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_রাশিমালা_get_funccal(রাশিমালা, rettv);

    জন্য ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	যদি (len < 0)
	{
	    *rettv = argv[0];
	    বিরতি;
	}

	r = eval_রাশিমালা_typval(রাশিমালা, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	যদি (r == FAIL || called_emsg != called_emsg_start)
	    বিরতি;
    }

    যদি (fc != ফাঁকা)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    স্থির শূন্য
byteidx_common(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা comp)
{
    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ফেরত;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা);
    যদি (str == ফাঁকা || idx < 0)
	ফেরত;

    varnumber_T	utf16idx = FALSE;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	পূর্ণসংখ্যা error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	যদি (error)
	    ফেরত;
	যদি (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_সংখ্যা_as_bool_nr), utf16idx);
	    ফেরত;
	}
    }

    পূর্ণসংখ্যা (*ptr2len)(char_u *);
    যদি (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    অন্যথা
	ptr2len = mb_ptr2len;

    char_u *t = str;
    জন্য ( ; idx > 0; idx--)
    {
	যদি (*t == NUL)		// EOL reached
	    ফেরত;
	যদি (utf16idx)
	{
	    পূর্ণসংখ্যা clen = ptr2len(t);
	    পূর্ণসংখ্যা c = (clen > 1) ? utf_ptr2char(t) : *t;
	    যদি (c > 0xFFFF)
		idx--;
	    যদি (idx > 0)
		t += clen;
	}
	অন্যথা যদি (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_সংখ্যা = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    শূন্য
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    শূন্য
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    শূন্য
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = -1;

    যদি (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	ফেরত;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা);
    যদি (str == ফাঁকা || idx < 0)
	ফেরত;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	যদি (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    পূর্ণসংখ্যা (*ptr2len)(char_u *);
    যদি (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    অন্যথা
	ptr2len = mb_ptr2len;

    char_u	*p;
    পূর্ণসংখ্যা		len;
    জন্য (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	যদি (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    যদি (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_সংখ্যা = len;
	    ফেরত;
	}
	যদি (utf16idx)
	{
	    idx--;
	    পূর্ণসংখ্যা clen = ptr2len(p);
	    পূর্ণসংখ্যা c = (clen > 1) ? utf_ptr2char(p) : *p;
	    যদি (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_সংখ্যা = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    স্থির পূর্ণসংখ্যা
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    যদি (convert_setup(&vimconv, from, to) == FAIL)
	ফেরত FAIL;
    vimconv.vc_fail = TRUE;
    যদি (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->দৈর্ঘ্য);
	যদি (ret->string == ফাঁকা)
	    ret->দৈর্ঘ্য = 0;
	অন্যথা
	    ret->দৈর্ঘ্য = str->দৈর্ঘ্য;
    }
    অন্যথা
    {
	পূর্ণসংখ্যা len = (পূর্ণসংখ্যা)str->দৈর্ঘ্য;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->দৈর্ঘ্য = len;
    }
    convert_setup(&vimconv, ফাঁকা, ফাঁকা);

    ফেরত (ret->string == ফাঁকা) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    স্থির শূন্য
blob_from_string(char_u *str, blob_T *blob)
{
    পূর্ণসংখ্যা	    len = (পূর্ণসংখ্যা)STRLEN(str);
    char_u  *dest;

    যদি (len == 0)
	ফেরত;
    যদি (ga_grow(&blob->bv_ga, len) == FAIL)
	ফেরত;
    dest = (char_u *)blob->bv_ga.ga_তথ্য + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    জন্য (পূর্ণসংখ্যা i = 0; i < len; ++i)
	যদি (dest[i] == NL)
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
    স্থির পূর্ণসংখ্যা
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	সারি_len;

    যদি (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->দৈর্ঘ্য = 0;
	*start_idx = blen;
	ফেরত (ret->string == ফাঁকা) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_তথ্য + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    সারি_len = (nl == ফাঁকা) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(সারি_len + 1);
    যদি (ret->string == ফাঁকা)
    {
	ret->দৈর্ঘ্য = 0;
	ফেরত FAIL;
    }
    যদি (সারি_len > 0)
	mch_memmove(ret->string, src, (size_t)সারি_len);
    ret->string[সারি_len] = NUL;
    ret->দৈর্ঘ্য = (size_t)সারি_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    জন্য (long i = 0; i < সারি_len; i++)
	যদি (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + সারি_len + (nl != ফাঁকা ? 1 : 0);
    ফেরত OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    স্থির char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    যদি (from_encoding_raw == ফাঁকা)
	ফেরত ফাঁকা;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    যখন (*s != NUL)
    {
	যদি (*s == '_')
	    *pe++ = '-';
	অন্যথা
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    যদি ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    ফেরত from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    স্থির শূন্য
append_converted_string_to_list(
	string_T *converted,
	পূর্ণসংখ্যা validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    যদি (converted->string != ফাঁকা)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->দৈর্ঘ্য;
	যখন (p < end)
	{
	    string_T	সারি;
	    char_u	*সারি_start = p;

	    যখন (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    সারি.দৈর্ঘ্য = (size_t)(p - সারি_start);
	    সারি.string = vim_strnsave(সারি_start, সারি.দৈর্ঘ্য);
	    যদি (সারি.string != ফাঁকা)
	    {
		যদি (validate_utf8 && !utf_valid_string(সারি.string, ফাঁকা))
		{
		    vim_free(সারি.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    ফেরত; // Stop processing
		}
		যদি (list_append_string(list, সারি.string, (পূর্ণসংখ্যা)সারি.দৈর্ঘ্য) == FAIL)
		{
		    vim_free(সারি.string);
		    vim_free(converted->string);
		    ফেরত; // Stop processing on append failure
		}
		vim_free(সারি.string);
	    }
	    অন্যথা
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		ফেরত;
	    }

	    যদি (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    অন্যথা
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    স্থির পূর্ণসংখ্যা
append_validated_সারি_to_list(string_T *সারি, পূর্ণসংখ্যা validate_utf8, list_T *list)
{
    যদি (validate_utf8 && !utf_valid_string(সারি->string, ফাঁকা))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(সারি->string);
	ফেরত FAIL;
    }

    পূর্ণসংখ্যা ret = list_append_string(list, সারি->string, (পূর্ণসংখ্যা)সারি->দৈর্ঘ্য);
    vim_free(সারি->string);
    ফেরত ret;
}

    শূন্য
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    পূর্ণসংখ্যা		blen;
    long	idx;
    পূর্ণসংখ্যা		validate_utf8 = FALSE;

    যদি (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ফেরত;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    blob = argvars->vval.v_blob;
    যদি (blob == ফাঁকা)
	ফেরত;
    blen = blob_len(blob);

    char_u	*from_encoding = ফাঁকা;
    char_u	*from_encoding_raw = ফাঁকা;  // Encoding name with endianness preserved for iconv
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	যদি (d != ফাঁকা)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    যদি (enc != ফাঁকা)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		যদি (from_encoding_raw == ফাঁকা)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    ফেরত;
		}
	    }
	}
    }

    যদি (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    যদি (from_encoding != ফাঁকা && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    পূর্ণসংখ্যা from_prop = 0;
    যদি (from_encoding != ফাঁকা)
	from_prop = enc_canon_props(from_encoding);
    যদি (from_encoding != ফাঁকা && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	পূর্ণসংখ্যা nul_আকার = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_আকার);
	যদি (ga_grow(&blob_ga, blen + nul_আকার) == OK)
	{
	    যদি (blen > 0)
		mch_memmove(blob_ga.ga_তথ্য, blob->bv_ga.ga_তথ্য, (size_t)blen);
	    // NUL terminator (2 bytes for UTF-16/UCS-2, 4 bytes for UTF-32/UCS-4)
	    vim_memset((char_u *)blob_ga.ga_তথ্য + blen, 0, (size_t)nul_আকার);
	    blob_ga.ga_len = blen + nul_আকার;
	}

	// Convert the entire blob at once
	vimconv_T vimconv;
	vimconv.vc_type = CONV_NONE;
	// Use raw encoding name for iconv to preserve endianness (utf-16be vs utf-16)
	// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
	যদি (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    যাও done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	পূর্ণসংখ্যা len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_তথ্য, &len, ফাঁকা);
	converted.দৈর্ঘ্য = len;
	convert_setup(&vimconv, ফাঁকা, ফাঁকা);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    অন্যথা
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	যখন (idx < blen)
	{
	    string_T	str;

	    যদি (string_from_blob(blob, &idx, &str) != OK)
		বিরতি;

	    যদি (from_encoding != ফাঁকা)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		পূর্ণসংখ্যা	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		যদি (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    যাও done;
		}
		str.string = converted.string;
		str.দৈর্ঘ্য = converted.দৈর্ঘ্য;
	    }

	    যদি (append_validated_সারি_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		যাও done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    যদি (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

done:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    শূন্য
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    যদি (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ফেরত;

    যদি (rettv_blob_alloc(rettv) == FAIL)
	ফেরত;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    যদি (list == ফাঁকা)
	ফেরত;

    char_u	*to_encoding = ফাঁকা;
    char_u	*to_encoding_raw = ফাঁকা;  // Encoding name with endianness preserved for iconv
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	যদি (d != ফাঁকা)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    যদি (enc != ফাঁকা)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		যদি (to_encoding_raw == ফাঁকা)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    ফেরত;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    পূর্ণসংখ্যা to_prop = 0;
    যদি (to_encoding != ফাঁকা)
	to_prop = enc_canon_props(to_encoding);
    যদি (to_encoding != ফাঁকা && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_বস্তুসমূহ(list, li)
	{
	    char_u *s;

	    যদি (li->li_tv.v_type != VAR_STRING)
		চালু;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    যদি (li != list->lv_first)
		ga_append(&str_ga, NL);
	    যদি (s != ফাঁকা && *s != NUL)
	    {
		পূর্ণসংখ্যা slen = (পূর্ণসংখ্যা)STRLEN(s);

		যদি (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    যাও done;
		}
		mch_memmove((char_u *)str_ga.ga_তথ্য + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	যদি (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    যদি (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		যাও done;
	    }
	    vimconv.vc_fail = TRUE;

	    পূর্ণসংখ্যা		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_তথ্য, &len, ফাঁকা);
	    convert_setup(&vimconv, ফাঁকা, ফাঁকা);
	    ga_clear(&str_ga);

	    যদি (converted == ফাঁকা)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		যাও done;
	    }
	    যদি (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_তথ্য + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	অন্যথা
	    ga_clear(&str_ga);
    }
    অন্যথা
    {
	FOR_ALL_LIST_বস্তুসমূহ(list, li)
	{
	    যদি (li->li_tv.v_type != VAR_STRING)
		চালু;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    যদি (str.string == ফাঁকা)
		STR_LITERAL_SET(str, "");
	    অন্যথা
		str.দৈর্ঘ্য = STRLEN(str.string);

	    যদি (to_encoding != ফাঁকা)
	    {
		পূর্ণসংখ্যা	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		যদি (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    যাও done;
		}
		str.string = converted.string;
		str.দৈর্ঘ্য = converted.দৈর্ঘ্য;
	    }

	    যদি (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    যদি (to_encoding != ফাঁকা)
		vim_free(str.string);
	}
    }

done:
    যদি (to_encoding != ফাঁকা)
	vim_free(to_encoding);
    যদি (to_encoding_raw != ফাঁকা)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    শূন্য
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    পূর্ণসংখ্যা		utf8 = FALSE;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[1], ফাঁকা);

    p = tv_get_string(&argvars[0]);

    যদি (has_mbyte || utf8)
    {
	পূর্ণসংখ্যা (*ptr2len)(char_u *);
	পূর্ণসংখ্যা (*ptr2char)(char_u *);

	যদি (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	অন্যথা
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	জন্য ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_সংখ্যা(rettv->vval.v_list, (*ptr2char)(p));
    }
    অন্যথা
	জন্য ( ; *p != NUL; ++p)
	    list_append_সংখ্যা(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    শূন্য
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		base = 10;
    char_u	*p;
    varnumber_T	n;
    পূর্ণসংখ্যা		what = 0;
    পূর্ণসংখ্যা		isneg;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[1]);
	যদি (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    ফেরত;
	}
	যদি (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    যদি (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    নির্বাচন (base)
    {
	ক্ষেত্র 2: what |= STR2NR_BIN + STR2NR_FORCE; বিরতি;
	ক্ষেত্র 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; বিরতি;
	ক্ষেত্র 16: what |= STR2NR_HEX + STR2NR_FORCE; বিরতি;
    }
    vim_str2nr(p, ফাঁকা, ফাঁকা, what, &n, ফাঁকা, 0, FALSE, ফাঁকা);
    // Text after the number is silently ignored.
    যদি (isneg)
	rettv->vval.v_সংখ্যা = -n;
    অন্যথা
	rettv->vval.v_সংখ্যা = n;

}

/*
 * "strgetchar()" function
 */
    শূন্য
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		error = FALSE;
    পূর্ণসংখ্যা		charidx;
    পূর্ণসংখ্যা		byteidx = 0;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    str = tv_get_string_chk(&argvars[0]);
    যদি (str == ফাঁকা)
	ফেরত;
    len = (পূর্ণসংখ্যা)STRLEN(str);
    charidx = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (error)
	ফেরত;

    যখন (charidx >= 0 && byteidx < len)
    {
	যদি (charidx == 0)
	{
	    rettv->vval.v_সংখ্যা = mb_ptr2char(str + byteidx);
	    বিরতি;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    শূন্য
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    পূর্ণসংখ্যা		start_idx;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL))
	ফেরত;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_সংখ্যা = -1;
    যদি (needle == ফাঁকা || haystack == ফাঁকা)
	ফেরত;		// type error; errmsg already given

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	পূর্ণসংখ্যা	    error = FALSE;

	start_idx = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[2], &error);
	যদি (error || start_idx >= (পূর্ণসংখ্যা)STRLEN(haystack))
	    ফেরত;
	যদি (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((অক্ষর *)haystack, (অক্ষর *)needle);
    যদি (pos != ফাঁকা)
	rettv->vval.v_সংখ্যা = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    শূন্য
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    যদি (rettv->vval.v_string != ফাঁকা && tofree == ফাঁকা)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    শূন্য
f_strlen(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && check_for_string_or_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_সংখ্যা = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    স্থির শূন্য
strchar_common(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    পূর্ণসংখ্যা			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    যখন (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_সংখ্যা = len;
}

/*
 * "strcharlen()" function
 */
    শূন্য
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && check_for_string_or_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    শূন্য
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	পূর্ণসংখ্যা error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	যদি (error)
	    ফেরত;
	যদি (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_সংখ্যা_as_bool_nr), skipcc);
	    ফেরত;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    শূন্য
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = -1;

    যদি (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	ফেরত;

    varnumber_T countcc = FALSE;
    যদি (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    পূর্ণসংখ্যা			(*func_mb_ptr2char_adv)(char_u **pp);
    পূর্ণসংখ্যা			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    যখন (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	যদি (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_সংখ্যা = len;
}

/*
 * "strdisplaywidth()" function
 */
    শূন্য
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    পূর্ণসংখ্যা		col = 0;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    s = tv_get_string(&argvars[0]);
    যদি (argvars[1].v_type != VAR_UNKNOWN)
	col = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[1]);

    rettv->vval.v_সংখ্যা = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    শূন্য
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_সংখ্যা = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    শূন্য
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    পূর্ণসংখ্যা		nchar;
    পূর্ণসংখ্যা		nbyte = 0;
    পূর্ণসংখ্যা		charlen;
    পূর্ণসংখ্যা		skipcc = FALSE;
    পূর্ণসংখ্যা		len = 0;
    পূর্ণসংখ্যা		slen;
    পূর্ণসংখ্যা		error = FALSE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ফেরত;

    p = tv_get_string(&argvars[0]);
    slen = (পূর্ণসংখ্যা)STRLEN(p);

    nchar = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (!error)
    {
	যদি (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    যদি (error)
		ফেরত;
	    যদি (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_সংখ্যা_as_bool_nr), skipcc);
		ফেরত;
	    }
	}

	যদি (nchar > 0)
	    যখন (nchar > 0 && nbyte < slen)
	    {
		যদি (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		অন্যথা
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	অন্যথা
	    nbyte = nchar;
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[2]);
	    যখন (charlen > 0 && nbyte + len < slen)
	    {
		পূর্ণসংখ্যা off = nbyte + len;

		যদি (off < 0)
		    len += 1;
		অন্যথা
		{
		    যদি (skipcc)
			len += mb_ptr2len(p + off);
		    অন্যথা
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	অন্যথা
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    যদি (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    অন্যথা যদি (nbyte > slen)
	nbyte = slen;
    যদি (len < 0)
	len = 0;
    অন্যথা যদি (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    শূন্য
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		slen;
    পূর্ণসংখ্যা		error = FALSE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ফেরত;

    p = tv_get_string(&argvars[0]);
    slen = (পূর্ণসংখ্যা)STRLEN(p);

    n = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (error)
	len = 0;
    অন্যথা যদি (argvars[2].v_type != VAR_UNKNOWN)
	len = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[2]);
    অন্যথা
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    যদি (n < 0)
    {
	len += n;
	n = 0;
    }
    অন্যথা যদি (n > slen)
	n = slen;
    যদি (len < 0)
	len = 0;
    অন্যথা যদি (n + len > slen)
	len = slen - n;

    যদি (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	পূর্ণসংখ্যা off;

	// length in characters
	জন্য (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    শূন্য
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = ফাঁকা;
    পূর্ণসংখ্যা		haystack_len, end_idx;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL))
	ফেরত;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_সংখ্যা = -1;
    যদি (needle == ফাঁকা || haystack == ফাঁকা)
	ফেরত;		// type error; errmsg already given

    haystack_len = (পূর্ণসংখ্যা)STRLEN(haystack);
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[2], ফাঁকা);
	যদি (end_idx < 0)
	    ফেরত;	// can never find a match
    }
    অন্যথা
	end_idx = haystack_len;

    যদি (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    অন্যথা
    {
	জন্য (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((অক্ষর *)rest, (অক্ষর *)needle);
	    যদি (rest == ফাঁকা || rest > haystack + end_idx)
		বিরতি;
	    lastmatch = rest;
	}
    }

    যদি (lastmatch == ফাঁকা)
	rettv->vval.v_সংখ্যা = -1;
    অন্যথা
	rettv->vval.v_সংখ্যা = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    শূন্য
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    শূন্য
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = -1;

    যদি (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    ফেরত;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা);
    যদি (str == ফাঁকা || idx < 0)
	ফেরত;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	যদি (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    পূর্ণসংখ্যা (*ptr2len)(char_u *);
    যদি (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    অন্যথা
	ptr2len = mb_ptr2len;

    char_u	*p;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		utf16idx = 0;
    জন্য (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	যদি (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    যদি (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_সংখ্যা = len;
	    ফেরত;
	}
	utf16idx = len;
	পূর্ণসংখ্যা clen = ptr2len(p);
	পূর্ণসংখ্যা c = (clen > 1) ? utf_ptr2char(p) : *p;
	যদি (c > 0xFFFF)
	    len++;
	p += clen;
	যদি (charidx)
	    idx--;
    }

    rettv->vval.v_সংখ্যা = utf16idx;
}

/*
 * "tolower(string)" function
 */
    শূন্য
f_tolower(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    শূন্য
f_toupper(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    শূন্য
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*in_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    পূর্ণসংখ্যা		inlen;
    পূর্ণসংখ্যা		fromlen;
    পূর্ণসংখ্যা		tolen;
    পূর্ণসংখ্যা		idx;
    char_u	*cpstr;
    পূর্ণসংখ্যা		cplen;
    পূর্ণসংখ্যা		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ফেরত;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;
    যদি (fromstr == ফাঁকা || tostr == ফাঁকা)
	    ফেরত;		// type error; errmsg already given
    ga_init2(&ga, আকার(অক্ষর), 80);

    যদি (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	যদি (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    ফেরত;
	}

    // fromstr and tostr have to contain the same number of chars
    যখন (*in_str != NUL)
    {
	যদি (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    জন্য (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		যদি (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    জন্য (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			যদি (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    বিরতি;
			}
		    }
		    যদি (*p == NUL)	// tostr is shorter than fromstr
			যাও error;
		    বিরতি;
		}
		++idx;
	    }

	    যদি (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		জন্য (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		যদি (idx != 0)
		    যাও error;
	    }

	    (শূন্য)ga_grow(&ga, cplen);
	    mch_memmove((অক্ষর *)ga.ga_তথ্য + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	অন্যথা
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    যদি (p != ফাঁকা)
		ga_append(&ga, tostr[p - fromstr]);
	    অন্যথা
		ga_append(&ga, *in_str);
	    ++in_str;
	}
    }

    // add a terminating NUL
    (শূন্য)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_তথ্য;
}

/*
 * "trim({expr})" function
 */
    শূন্য
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*শীর্ষ;
    char_u	*mask = ফাঁকা;
    char_u	*লেজ;
    char_u	*পূর্ব;
    char_u	*p;
    পূর্ণসংখ্যা		c1;
    পূর্ণসংখ্যা		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL)))
	ফেরত;

    শীর্ষ = tv_get_string_buf_chk(&argvars[0], buf1);
    যদি (শীর্ষ == ফাঁকা)
	ফেরত;

    যদি (check_for_opt_string_arg(argvars, 1) == FAIL)
	ফেরত;

    যদি (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	যদি (*mask == NUL)
	    mask = ফাঁকা;

	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    পূর্ণসংখ্যা	error = 0;

	    // leading or trailing characters to trim
	    dir = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[2], &error);
	    যদি (error)
		ফেরত;
	    যদি (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		ফেরত;
	    }
	}
    }

    যদি (dir == 0 || dir == 1)
    {
	// Trim leading characters
	যখন (*শীর্ষ != NUL)
	{
	    c1 = PTR2CHAR(শীর্ষ);
	    যদি (mask == ফাঁকা)
	    {
		যদি (c1 > ' ' && c1 != 0xa0)
		    বিরতি;
	    }
	    অন্যথা
	    {
		জন্য (p = mask; *p != NUL; MB_PTR_ADV(p))
		    যদি (c1 == PTR2CHAR(p))
			বিরতি;
		যদি (*p == NUL)
		    বিরতি;
	    }
	    MB_PTR_ADV(শীর্ষ);
	}
    }

    লেজ = শীর্ষ + STRLEN(শীর্ষ);
    যদি (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	জন্য (; লেজ > শীর্ষ; লেজ = পূর্ব)
	{
	    পূর্ব = লেজ;
	    MB_PTR_BACK(শীর্ষ, পূর্ব);
	    c1 = PTR2CHAR(পূর্ব);
	    যদি (mask == ফাঁকা)
	    {
		যদি (c1 > ' ' && c1 != 0xa0)
		    বিরতি;
	    }
	    অন্যথা
	    {
		জন্য (p = mask; *p != NUL; MB_PTR_ADV(p))
		    যদি (c1 == PTR2CHAR(p))
			বিরতি;
		যদি (*p == NUL)
		    বিরতি;
	    }
	}
    }
    rettv->vval.v_string = vim_strnsave(শীর্ষ, লেজ - শীর্ষ);
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
    স্থির char_u *
uri_decode(char_u *str)
{
    যদি (str == ফাঁকা)
	ফেরত ফাঁকা;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    যদি (!decoded)
	ফেরত ফাঁকা;

    char_u	*p = decoded;
    size_t	i = 0;

    যখন (i < len)
    {
	যদি (str[i] == '%')
	{
	    যদি (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		যদি (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    অন্যথা
	    {
		পূর্ণসংখ্যা val = hexhex2nr(&str[i + 1]);
		যদি (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		অন্যথা
		{
		    // invalid hex digits following "%"
		    জন্য (পূর্ণসংখ্যা j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	অন্যথা
	    *p++ = str[i++];
    }

    *p = NUL;

    ফেরত decoded;
}

/*
 * "uri_decode({str})" function
 */
    শূন্য
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

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
    স্থির char_u *
uri_encode(char_u *str)
{
    যদি (str == ফাঁকা)
	ফেরত ফাঁকা;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    যদি (encoded == ফাঁকা)
	ফেরত ফাঁকা;

    char_u *p = encoded;

    জন্য (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	যদি (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	অন্যথা
	{
	    sprintf((অক্ষর *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    ফেরত encoded;
}

/*
 * "uri_encode({str})" function
 */
    শূন্য
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

স্থির অক্ষর *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    স্থির varnumber_T
tv_nr(typval_T *tvs, পূর্ণসংখ্যা *idxp)
{
    পূর্ণসংখ্যা		idx = *idxp - 1;
    varnumber_T	n = 0;
    পূর্ণসংখ্যা		err = FALSE;

    যদি (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    অন্যথা
    {
	++*idxp;
	n = tv_get_সংখ্যা_chk(&tvs[idx], &err);
	যদি (err)
	    n = 0;
    }
    ফেরত n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    স্থির অক্ষর *
tv_str(typval_T *tvs, পূর্ণসংখ্যা *idxp, char_u **tofree)
{
    পূর্ণসংখ্যা		    idx = *idxp - 1;
    অক্ষর	    *s = ফাঁকা;
    স্থির char_u   numbuf[NUMBUFLEN];

    যদি (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    অন্যথা
    {
	++*idxp;
	যদি (tofree != ফাঁকা)
	    s = (অক্ষর *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	অন্যথা
	    s = (অক্ষর *)tv_get_string_chk(&tvs[idx]);
    }
    ফেরত s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    স্থির দ্বিগুণ
tv_float(typval_T *tvs, পূর্ণসংখ্যা *idxp)
{
    পূর্ণসংখ্যা		idx = *idxp - 1;
    দ্বিগুণ	f = 0;

    যদি (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    অন্যথা
    {
	++*idxp;
	যদি (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	অন্যথা যদি (tvs[idx].v_type == VAR_সংখ্যা)
	    f = (দ্বিগুণ)tvs[idx].vval.v_সংখ্যা;
	অন্যথা
	    emsg(_(e_expected_float_argument_for_printf));
    }
    ফেরত f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    স্থির ধ্রুবক অক্ষর *
infinity_str(পূর্ণসংখ্যা positive,
	     অক্ষর fmt_spec,
	     পূর্ণসংখ্যা force_sign,
	     পূর্ণসংখ্যা space_for_positive)
{
    স্থির ধ্রুবক অক্ষর *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    পূর্ণসংখ্যা idx = positive * (1 + force_sign + force_sign * space_for_positive);

    যদি (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    ফেরত table[idx];
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
    পূর্ণসংখ্যা
vim_snprintf_add(অক্ষর *str, size_t str_m, ধ্রুবক অক্ষর *fmt, ...)
{
    va_list	ap;
    পূর্ণসংখ্যা		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    যদি (str_m <= len)
	space = 0;
    অন্যথা
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    ফেরত str_l;
}

    পূর্ণসংখ্যা
vim_snprintf(অক্ষর *str, size_t str_m, ধ্রুবক অক্ষর *fmt, ...)
{
    va_list	ap;
    পূর্ণসংখ্যা		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    ফেরত str_l;
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
vim_snprintf_safelen(অক্ষর *str, size_t str_m, ধ্রুবক অক্ষর *fmt, ...)
{
    va_list ap;
    পূর্ণসংখ্যা	    str_l;

    যদি (str_m == 0)
	ফেরত 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, ফাঁকা);
    va_end(ap);

    যদি (str_l < 0)
    {
	*str = NUL;
	ফেরত 0;
    }
    ফেরত ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    পূর্ণসংখ্যা
vim_vsnprintf(
    অক্ষর	*str,
    size_t	str_m,
    ধ্রুবক অক্ষর	*fmt,
    va_list	ap)
{
    ফেরত vim_vsnprintf_typval(str, str_m, fmt, ap, ফাঁকা);
}

গণনা
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
    স্থির পূর্ণসংখ্যা
format_typeof(
    ধ্রুবক অক্ষর	*type)
{
    // allowed values: \0, h, l, L
    অক্ষর    দৈর্ঘ্য_modifier = '\0';

    // current conversion specifier character
    অক্ষর    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    যদি (*type == 'h' || *type == 'l')
    {
	দৈর্ঘ্য_modifier = *type;
	type++;
	যদি (দৈর্ঘ্য_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    দৈর্ঘ্য_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    নির্বাচন (fmt_spec)
    {
	ক্ষেত্র 'i': fmt_spec = 'd'; বিরতি;
	ক্ষেত্র '*': fmt_spec = 'd'; দৈর্ঘ্য_modifier = 'h'; বিরতি;
	ক্ষেত্র 'D': fmt_spec = 'd'; দৈর্ঘ্য_modifier = 'l'; বিরতি;
	ক্ষেত্র 'U': fmt_spec = 'u'; দৈর্ঘ্য_modifier = 'l'; বিরতি;
	ক্ষেত্র 'O': fmt_spec = 'o'; দৈর্ঘ্য_modifier = 'l'; বিরতি;
	পূর্বনির্ধারিত: বিরতি;
    }

    // get parameter value, do initial processing
    নির্বাচন (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    ক্ষেত্র '%':
	ফেরত TYPE_PERCENT;

    ক্ষেত্র 'c':
	ফেরত TYPE_CHAR;

    ক্ষেত্র 's':
    ক্ষেত্র 'S':
	ফেরত TYPE_STRING;

    ক্ষেত্র 'd': ক্ষেত্র 'u':
    ক্ষেত্র 'b': ক্ষেত্র 'B':
    ক্ষেত্র 'o':
    ক্ষেত্র 'x': ক্ষেত্র 'X':
    ক্ষেত্র 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //মান

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    যদি (fmt_spec == 'p')
		ফেরত TYPE_POINTER;
	    অন্যথা যদি (fmt_spec == 'b' || fmt_spec == 'B')
		ফেরত TYPE_UNSIGNEDLONGLONGINT;
	    অন্যথা যদি (fmt_spec == 'd')
	    {
		// signed
		নির্বাচন (দৈর্ঘ্য_modifier)
		{
		ক্ষেত্র '\0':
		ক্ষেত্র 'h':
		    // char and short arguments are passed as int.
		    ফেরত TYPE_INT;
		ক্ষেত্র 'l':
		    ফেরত TYPE_LONGINT;
		ক্ষেত্র 'L':
		    ফেরত TYPE_LONGLONGINT;
		}
	    }
	    অন্যথা
	    {
		// unsigned
		নির্বাচন (দৈর্ঘ্য_modifier)
		{
		    ক্ষেত্র '\0':
		    ক্ষেত্র 'h':
			ফেরত TYPE_UNSIGNEDINT;
		    ক্ষেত্র 'l':
			ফেরত TYPE_UNSIGNEDLONGINT;
		    ক্ষেত্র 'L':
			ফেরত TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	বিরতি;

    ক্ষেত্র 'f':
    ক্ষেত্র 'F':
    ক্ষেত্র 'e':
    ক্ষেত্র 'E':
    ক্ষেত্র 'g':
    ক্ষেত্র 'G':
	ফেরত TYPE_FLOAT;
    }

    ফেরত TYPE_UNKNOWN;
}

    স্থির অক্ষর *
format_typename(
    ধ্রুবক অক্ষর  *type)
{
    নির্বাচন (format_typeof(type))
    {
	ক্ষেত্র TYPE_INT:
	    ফেরত typename_int;

	ক্ষেত্র TYPE_LONGINT:
	    ফেরত typename_longint;

	ক্ষেত্র TYPE_LONGLONGINT:
	    ফেরত typename_longlongint;

	ক্ষেত্র TYPE_UNSIGNEDINT:
	    ফেরত typename_unsignedint;

	ক্ষেত্র TYPE_UNSIGNEDLONGINT:
	    ফেরত typename_unsignedlongint;

	ক্ষেত্র TYPE_UNSIGNEDLONGLONGINT:
	    ফেরত typename_unsignedlonglongint;

	ক্ষেত্র TYPE_POINTER:
	    ফেরত _(typename_pointer);

	ক্ষেত্র TYPE_PERCENT:
	    ফেরত _(typename_percent);

	ক্ষেত্র TYPE_CHAR:
	    ফেরত typename_char;

	ক্ষেত্র TYPE_STRING:
	    ফেরত _(typename_string);

	ক্ষেত্র TYPE_FLOAT:
	    ফেরত typename_float;
    }

    ফেরত _(typename_unknown);
}

    স্থির পূর্ণসংখ্যা
adjust_types(
    ধ্রুবক অক্ষর ***ap_types,
    পূর্ণসংখ্যা arg,
    পূর্ণসংখ্যা *অঙ্ক_posarg,
    ধ্রুবক অক্ষর *type)
{
    যদি (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	ফেরত FAIL;
    }

    যদি (*ap_types == ফাঁকা || *অঙ্ক_posarg < arg)
    {
	পূর্ণসংখ্যা	    idx;
	ধ্রুবক অক্ষর  **new_types;

	যদি (*ap_types == ফাঁকা)
	    new_types = ALLOC_CLEAR_MULT(ধ্রুবক অক্ষর *, arg);
	অন্যথা
	    new_types = vim_realloc((অক্ষর **)*ap_types,
						arg * আকার(ধ্রুবক অক্ষর *));

	যদি (new_types == ফাঁকা)
	    ফেরত FAIL;

	জন্য (idx = *অঙ্ক_posarg; idx < arg; ++idx)
	    new_types[idx] = ফাঁকা;

	*ap_types = new_types;
	*অঙ্ক_posarg = arg;
    }

    যদি ((*ap_types)[arg - 1] != ফাঁকা)
    {
	যদি ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    ধ্রুবক অক্ষর *pt = type;
	    যদি (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    যদি (pt[0] != '*')
	    {
		নির্বাচন (pt[0])
		{
		    ক্ষেত্র 'd':
		    ক্ষেত্র 'i': বিরতি;
		    পূর্বনির্ধারিত:
			semsg(_(e_positional_অঙ্ক_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			ফেরত FAIL;
		}
	    }
	}
	অন্যথা
	{
	    যদি (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_অঙ্ক_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		ফেরত FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    ফেরত OK;
}

    স্থির শূন্য
format_overflow_error(ধ্রুবক অক্ষর *pstart)
{
    size_t	arglen = 0;
    অক্ষর	*argcopy = ফাঁকা;
    ধ্রুবক অক্ষর	*p = pstart;

    যখন (VIM_ISDIGIT((পূর্ণসংখ্যা)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(অক্ষর, arglen + 1);
    যদি (argcopy != ফাঁকা)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	মুক্ত(argcopy);
    }
    অন্যথা
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    স্থির পূর্ণসংখ্যা
get_unsigned_int(
    ধ্রুবক অক্ষর *pstart,
    ধ্রুবক অক্ষর **p,
    unsigned পূর্ণসংখ্যা *uj,
    পূর্ণসংখ্যা overflow_err)
{
    *uj = **p - '0';
    ++*p;

    যখন (VIM_ISDIGIT((পূর্ণসংখ্যা)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned পূর্ণসংখ্যা)(**p - '0');
	++*p;
    }

    যদি (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	যদি (overflow_err)
	{
	    format_overflow_error(pstart);
	    ফেরত FAIL;
	}
	অন্যথা
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    ফেরত OK;
}


    স্থির পূর্ণসংখ্যা
parse_fmt_types(
    ধ্রুবক অক্ষর  ***ap_types,
    পূর্ণসংখ্যা		*অঙ্ক_posarg,
    ধ্রুবক অক্ষর  *fmt,
    typval_T	*tvs UNUSED
    )
{
    ধ্রুবক অক্ষর	*p = fmt;
    ধ্রুবক অক্ষর	*arg = ফাঁকা;

    পূর্ণসংখ্যা		any_pos = 0;
    পূর্ণসংখ্যা		any_arg = 0;
    পূর্ণসংখ্যা		arg_idx;

# define CHECK_POS_ARG করো { \
    যদি (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	যাও error; \
    } \
} যখন (0);

    যদি (p == ফাঁকা)
	ফেরত OK;

    যখন (*p != NUL)
    {
	যদি (*p != '%')
	{
	    ধ্রুবক অক্ষর    *q = strchr(p + 1, '%');
	    size_t  n = (q == ফাঁকা) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	অন্যথা
	{
	    // allowed values: \0, h, l, L
	    অক্ষর	দৈর্ঘ্য_modifier = '\0';

	    // variable for positional arg
	    পূর্ণসংখ্যা		pos_arg = -1;
	    ধ্রুবক অক্ষর	*ptype = ফাঁকা;
	    ধ্রুবক অক্ষর	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    যখন (VIM_ISDIGIT(*ptype))
		++ptype;

	    যদি (*ptype == '$')
	    {
		যদি (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    যাও error;
		}

		// Positional argument
		unsigned পূর্ণসংখ্যা uj;

		যদি (get_unsigned_int(pstart, &p, &uj, tvs != ফাঁকা) == FAIL)
		    যাও error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    যখন (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		নির্বাচন (*p)
		{
		    ক্ষেত্র '0': বিরতি;
		    ক্ষেত্র '-': বিরতি;
		    ক্ষেত্র '+': বিরতি;
		    ক্ষেত্র ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      বিরতি;
		    ক্ষেত্র '#': বিরতি;
		    ক্ষেত্র '\'': বিরতি;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    যদি (*(arg = p) == '*')
	    {
		p++;

		যদি (VIM_ISDIGIT((পূর্ণসংখ্যা)(*p)))
		{
		    // Positional argument field width
		    unsigned পূর্ণসংখ্যা uj;

		    যদি (get_unsigned_int(arg + 1, &p, &uj, tvs != ফাঁকা) == FAIL)
			যাও error;

		    যদি (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			যাও error;
		    }
		    অন্যথা
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			যদি (adjust_types(ap_types, uj, অঙ্ক_posarg, arg) == FAIL)
			    যাও error;
		    }
		}
		অন্যথা
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    অন্যথা যদি (VIM_ISDIGIT((পূর্ণসংখ্যা)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ধ্রুবক অক্ষর *digstart = p;
		unsigned পূর্ণসংখ্যা uj;

		যদি (get_unsigned_int(digstart, &p, &uj, tvs != ফাঁকা) == FAIL)
		    যাও error;

		যদি (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    যাও error;
		}
	    }

	    // parse precision
	    যদি (*p == '.')
	    {
		p++;

		যদি (*(arg = p) == '*')
		{
		    p++;

		    যদি (VIM_ISDIGIT((পূর্ণসংখ্যা)(*p)))
		    {
			// Parse precision
			unsigned পূর্ণসংখ্যা uj;

			যদি (get_unsigned_int(arg + 1, &p, &uj, tvs != ফাঁকা) == FAIL)
			    যাও error;

			যদি (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    যদি (adjust_types(ap_types, uj, অঙ্ক_posarg, arg) == FAIL)
				যাও error;
			}
			অন্যথা
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    যাও error;
			}
		    }
		    অন্যথা
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		অন্যথা যদি (VIM_ISDIGIT((পূর্ণসংখ্যা)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ধ্রুবক অক্ষর *digstart = p;
		    unsigned পূর্ণসংখ্যা uj;

		    যদি (get_unsigned_int(digstart, &p, &uj, tvs != ফাঁকা) == FAIL)
			যাও error;

		    যদি (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			যাও error;
		    }
		}
	    }

	    যদি (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    যদি (*p == 'h' || *p == 'l')
	    {
		দৈর্ঘ্য_modifier = *p;
		p++;
		যদি (দৈর্ঘ্য_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    নির্বাচন (*p)
	    {
		// Check for known format specifiers. % is special!
		ক্ষেত্র 'i':
		ক্ষেত্র '*':
		ক্ষেত্র 'd':
		ক্ষেত্র 'u':
		ক্ষেত্র 'o':
		ক্ষেত্র 'D':
		ক্ষেত্র 'U':
		ক্ষেত্র 'O':
		ক্ষেত্র 'x':
		ক্ষেত্র 'X':
		ক্ষেত্র 'b':
		ক্ষেত্র 'B':
		ক্ষেত্র 'c':
		ক্ষেত্র 's':
		ক্ষেত্র 'S':
		ক্ষেত্র 'p':
		ক্ষেত্র 'f':
		ক্ষেত্র 'F':
		ক্ষেত্র 'e':
		ক্ষেত্র 'E':
		ক্ষেত্র 'g':
		ক্ষেত্র 'G':
		    যদি (pos_arg != -1)
		    {
			যদি (adjust_types(ap_types, pos_arg, অঙ্ক_posarg, ptype) == FAIL)
			    যাও error;
		    }
		    অন্যথা
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    বিরতি;

		পূর্বনির্ধারিত:
		    যদি (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			যাও error;
		    }
	    }

	    যদি (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    জন্য (arg_idx = 0; arg_idx < *অঙ্ক_posarg; ++arg_idx)
    {
	যদি ((*ap_types)[arg_idx] == ফাঁকা)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    যাও error;
	}

# যদি defined(FEAT_EVAL)
	যদি (tvs != ফাঁকা && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    যাও error;
	}
# endif
    }

    ফেরত OK;

error:
    vim_free((অক্ষর**)*ap_types);
    *ap_types = ফাঁকা;
    *অঙ্ক_posarg = 0;
    ফেরত FAIL;
}

    স্থির শূন্য
skip_to_arg(
    ধ্রুবক অক্ষর	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    পূর্ণসংখ্যা		*arg_idx,
    পূর্ণসংখ্যা		*arg_cur,
    ধ্রুবক অক্ষর	*fmt)
{
    পূর্ণসংখ্যা		arg_min = 0;

    যদি (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	ফেরত;
    }

    যদি (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    অন্যথা
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    জন্য (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	ধ্রুবক অক্ষর *p;

	যদি (ap_types == ফাঁকা || ap_types[*arg_cur] == ফাঁকা)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    ফেরত;
	}

	p = ap_types[*arg_cur];

	পূর্ণসংখ্যা fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	নির্বাচন (fmt_type)
	{
	ক্ষেত্র TYPE_PERCENT:
	ক্ষেত্র TYPE_UNKNOWN:
	    বিরতি;

	ক্ষেত্র TYPE_CHAR:
	    va_arg(*ap, পূর্ণসংখ্যা);
	    বিরতি;

	ক্ষেত্র TYPE_STRING:
	    va_arg(*ap, অক্ষর *);
	    বিরতি;

	ক্ষেত্র TYPE_POINTER:
	    va_arg(*ap, শূন্য *);
	    বিরতি;

	ক্ষেত্র TYPE_INT:
	    va_arg(*ap, পূর্ণসংখ্যা);
	    বিরতি;

	ক্ষেত্র TYPE_LONGINT:
	    va_arg(*ap, long পূর্ণসংখ্যা);
	    বিরতি;

	ক্ষেত্র TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    বিরতি;

	ক্ষেত্র TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned পূর্ণসংখ্যা);
	    বিরতি;

	ক্ষেত্র TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long পূর্ণসংখ্যা);
	    বিরতি;

	ক্ষেত্র TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    বিরতি;

	ক্ষেত্র TYPE_FLOAT:
	    va_arg(*ap, দ্বিগুণ);
	    বিরতি;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    ফেরত;
}

    পূর্ণসংখ্যা
vim_vsnprintf_typval(
    অক্ষর	*str,
    size_t	str_m,
    ধ্রুবক অক্ষর	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    ধ্রুবক অক্ষর	*p = fmt;
    পূর্ণসংখ্যা		arg_cur = 0;
    পূর্ণসংখ্যা		অঙ্ক_posarg = 0;
    পূর্ণসংখ্যা		arg_idx = 1;
    va_list	ap;
    ধ্রুবক অক্ষর	**ap_types = ফাঁকা;

    যদি (parse_fmt_types(&ap_types, &অঙ্ক_posarg, fmt, tvs) == FAIL)
	ফেরত 0;

    va_copy(ap, ap_start);

    যদি (p == ফাঁকা)
	p = "";
    যখন (*p != NUL)
    {
	যদি (*p != '%')
	{
	    ধ্রুবক অক্ষর    *q = strchr(p + 1, '%');
	    size_t  n = (q == ফাঁকা) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    যদি (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	অন্যথা
	{
	    size_t  min_field_width = 0, precision = 0;
	    পূর্ণসংখ্যা	    zero_padding = 0, precision_specified = 0, justify_বাম = 0;
	    পূর্ণসংখ্যা	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    পূর্ণসংখ্যা	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    অক্ষর    দৈর্ঘ্য_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    অক্ষর    tmp[TMP_LEN];

	    // string address in case of string argument
	    ধ্রুবক অক্ষর  *str_arg = ফাঁকা;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    unsigned অক্ষর uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  সংখ্যা_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    অক্ষর    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = ফাঁকা;

	    // variables for positional arg
	    পূর্ণসংখ্যা	    pos_arg = -1;
	    ধ্রুবক অক্ষর	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    যখন (VIM_ISDIGIT(*ptype))
		++ptype;

	    যদি (*ptype == '$')
	    {
		// Positional argument
		ধ্রুবক অক্ষর *digstart = p;
		unsigned পূর্ণসংখ্যা uj;

		যদি (get_unsigned_int(digstart, &p, &uj, tvs != ফাঁকা) == FAIL)
		    যাও error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    যখন (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		নির্বাচন (*p)
		{
		    ক্ষেত্র '0': zero_padding = 1; বিরতি;
		    ক্ষেত্র '-': justify_বাম = 1; বিরতি;
		    ক্ষেত্র '+': force_sign = 1; space_for_positive = 0; বিরতি;
		    ক্ষেত্র ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      বিরতি;
		    ক্ষেত্র '#': alternate_form = 1; বিরতি;
		    ক্ষেত্র '\'': বিরতি;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    যদি (*p == '*')
	    {
		পূর্ণসংখ্যা j;
		ধ্রুবক অক্ষর *digstart = p + 1;

		p++;

		যদি (VIM_ISDIGIT((পূর্ণসংখ্যা)(*p)))
		{
		    // Positional argument field width
		    unsigned পূর্ণসংখ্যা uj;

		    যদি (get_unsigned_int(digstart, &p, &uj, tvs != ফাঁকা) == FAIL)
			যাও error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# যদি defined(FEAT_EVAL)
		    tvs != ফাঁকা ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, পূর্ণসংখ্যা));

		যদি (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    যদি (tvs != ফাঁকা)
		    {
			format_overflow_error(digstart);
			যাও error;
		    }
		    অন্যথা
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		যদি (j >= 0)
		    min_field_width = j;
		অন্যথা
		{
		    min_field_width = -j;
		    justify_বাম = 1;
		}
	    }
	    অন্যথা যদি (VIM_ISDIGIT((পূর্ণসংখ্যা)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ধ্রুবক অক্ষর *digstart = p;
		unsigned পূর্ণসংখ্যা uj;

		যদি (get_unsigned_int(digstart, &p, &uj, tvs != ফাঁকা) == FAIL)
		    যাও error;

		min_field_width = uj;
	    }

	    // parse precision
	    যদি (*p == '.')
	    {
		p++;
		precision_specified = 1;

		যদি (VIM_ISDIGIT((পূর্ণসংখ্যা)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ধ্রুবক অক্ষর *digstart = p;
		    unsigned পূর্ণসংখ্যা uj;

		    যদি (get_unsigned_int(digstart, &p, &uj, tvs != ফাঁকা) == FAIL)
			যাও error;

		    precision = uj;
		}
		অন্যথা যদি (*p == '*')
		{
		    পূর্ণসংখ্যা j;
		    ধ্রুবক অক্ষর *digstart = p;

		    p++;

		    যদি (VIM_ISDIGIT((পূর্ণসংখ্যা)(*p)))
		    {
			// positional argument
			unsigned পূর্ণসংখ্যা uj;

			যদি (get_unsigned_int(digstart, &p, &uj, tvs != ফাঁকা) == FAIL)
			    যাও error;

			arg_idx = uj;

			++p;
		    }

		    j =
# যদি defined(FEAT_EVAL)
			tvs != ফাঁকা ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, পূর্ণসংখ্যা));

		    যদি (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			যদি (tvs != ফাঁকা)
			{
			    format_overflow_error(digstart);
			    যাও error;
			}
			অন্যথা
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    যদি (j >= 0)
			precision = j;
		    অন্যথা
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    যদি (*p == 'h' || *p == 'l')
	    {
		দৈর্ঘ্য_modifier = *p;
		p++;
		যদি (দৈর্ঘ্য_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    দৈর্ঘ্য_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    নির্বাচন (fmt_spec)
	    {
		ক্ষেত্র 'i': fmt_spec = 'd'; বিরতি;
		ক্ষেত্র 'D': fmt_spec = 'd'; দৈর্ঘ্য_modifier = 'l'; বিরতি;
		ক্ষেত্র 'U': fmt_spec = 'u'; দৈর্ঘ্য_modifier = 'l'; বিরতি;
		ক্ষেত্র 'O': fmt_spec = 'o'; দৈর্ঘ্য_modifier = 'l'; বিরতি;
		পূর্বনির্ধারিত: বিরতি;
	    }

# যদি defined(FEAT_EVAL)
	    নির্বাচন (fmt_spec)
	    {
		ক্ষেত্র 'd': ক্ষেত্র 'u': ক্ষেত্র 'o': ক্ষেত্র 'x': ক্ষেত্র 'X':
		    যদি (tvs != ফাঁকা && দৈর্ঘ্য_modifier == '\0')
			দৈর্ঘ্য_modifier = 'L';
	    }
# endif

	    যদি (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    নির্বাচন (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    ক্ষেত্র '%':
	    ক্ষেত্র 'c':
	    ক্ষেত্র 's':
	    ক্ষেত্র 'S':
		str_arg_l = 1;
		নির্বাচন (fmt_spec)
		{
		ক্ষেত্র '%':
		    str_arg = p;
		    বিরতি;

		ক্ষেত্র 'c':
		    {
			পূর্ণসংখ্যা j;

			j =
# যদি defined(FEAT_EVAL)
			    tvs != ফাঁকা ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, পূর্ণসংখ্যা));

			// standard demands unsigned char
			uchar_arg = (unsigned অক্ষর)j;
			str_arg = (অক্ষর *)&uchar_arg;
			বিরতি;
		    }

		ক্ষেত্র 's':
		ক্ষেত্র 'S':
		    str_arg =
# যদি defined(FEAT_EVAL)
				tvs != ফাঁকা ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, অক্ষর *));

		    যদি (str_arg == ফাঁকা)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    অন্যথা যদি (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    অন্যথা যদি (precision == 0)
			str_arg_l = 0;
		    অন্যথা
		    {
			// memchr on HP does not like n > 2^31  !!!
			ধ্রুবক অক্ষর *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == ফাঁকা) ? precision
						      : (size_t)(q - str_arg);
		    }
		    যদি (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			পূর্ণসংখ্যা	cell;

			জন্য (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    যদি (precision_specified && i + cell > precision)
				বিরতি;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			যদি (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    বিরতি;

		পূর্বনির্ধারিত:
		    বিরতি;
		}
		বিরতি;

	    ক্ষেত্র 'd': ক্ষেত্র 'u':
	    ক্ষেত্র 'b': ক্ষেত্র 'B':
	    ক্ষেত্র 'o':
	    ক্ষেত্র 'x': ক্ষেত্র 'X':
	    ক্ষেত্র 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //মান

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    পূর্ণসংখ্যা arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    পূর্ণসংখ্যা int_arg = 0;
		    unsigned পূর্ণসংখ্যা uint_arg = 0;

		    // only set for length modifier l
		    long পূর্ণসংখ্যা long_arg = 0;
		    unsigned long পূর্ণসংখ্যা ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    শূন্য *ptr_arg = ফাঁকা;

		    যদি (fmt_spec == 'p')
		    {
			দৈর্ঘ্য_modifier = '\0';
			ptr_arg =
# যদি defined(FEAT_EVAL)
				 tvs != ফাঁকা ? (শূন্য *)tv_str(tvs, &arg_idx,
									ফাঁকা) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, শূন্য *));

			যদি (ptr_arg != ফাঁকা)
			    arg_sign = 1;
		    }
		    অন্যথা যদি (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# যদি defined(FEAT_EVAL)
				    tvs != ফাঁকা ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			যদি (bin_arg != 0)
			    arg_sign = 1;
		    }
		    অন্যথা যদি (fmt_spec == 'd')
		    {
			// signed
			নির্বাচন (দৈর্ঘ্য_modifier)
			{
			ক্ষেত্র '\0':
			ক্ষেত্র 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# যদি defined(FEAT_EVAL)
					tvs != ফাঁকা ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, পূর্ণসংখ্যা));

			    যদি (int_arg > 0)
				arg_sign =  1;
			    অন্যথা যদি (int_arg < 0)
				arg_sign = -1;
			    বিরতি;
			ক্ষেত্র 'l':
			    long_arg =
# যদি defined(FEAT_EVAL)
					tvs != ফাঁকা ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long পূর্ণসংখ্যা));

			    যদি (long_arg > 0)
				arg_sign =  1;
			    অন্যথা যদি (long_arg < 0)
				arg_sign = -1;
			    বিরতি;
			ক্ষেত্র 'L':
			    llong_arg =
# যদি defined(FEAT_EVAL)
					tvs != ফাঁকা ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    যদি (llong_arg > 0)
				arg_sign =  1;
			    অন্যথা যদি (llong_arg < 0)
				arg_sign = -1;
			    বিরতি;
			}
		    }
		    অন্যথা
		    {
			// unsigned
			নির্বাচন (দৈর্ঘ্য_modifier)
			{
			    ক্ষেত্র '\0':
			    ক্ষেত্র 'h':
				uint_arg =
# যদি defined(FEAT_EVAL)
					    tvs != ফাঁকা ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned পূর্ণসংখ্যা));

				যদি (uint_arg != 0)
				    arg_sign = 1;
				বিরতি;
			    ক্ষেত্র 'l':
				ulong_arg =
# যদি defined(FEAT_EVAL)
					    tvs != ফাঁকা ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long পূর্ণসংখ্যা));

				যদি (ulong_arg != 0)
				    arg_sign = 1;
				বিরতি;
			    ক্ষেত্র 'L':
				ullong_arg =
# যদি defined(FEAT_EVAL)
					    tvs != ফাঁকা ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				যদি (ullong_arg != 0)
				    arg_sign = 1;
				বিরতি;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    যদি (precision_specified)
			zero_padding = 0;
		    যদি (fmt_spec == 'd')
		    {
			যদি (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    অন্যথা যদি (alternate_form)
		    {
			যদি (arg_sign != 0
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
		    যদি (!precision_specified)
			precision = 1;   // default precision is 1
		    যদি (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    অন্যথা
		    {
			অক্ষর	f[6];
			পূর্ণসংখ্যা	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			যদি (!দৈর্ঘ্য_modifier)
			    ;
			অন্যথা যদি (দৈর্ঘ্য_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# অন্যথা
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			অন্যথা
			    f[f_l++] = দৈর্ঘ্য_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			যদি (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			অন্যথা যদি (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    অক্ষর	    b[8 * আকার(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    করো
			    {
				b[আকার(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    যখন (bn != 0);

			    memcpy(tmp + str_arg_l, b + আকার(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			অন্যথা যদি (fmt_spec == 'd')
			{
			    // signed
			    নির্বাচন (দৈর্ঘ্য_modifier)
			    {
			    ক্ষেত্র '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       বিরতি;
			    ক্ষেত্র 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (short)int_arg);
				      বিরতি;
			    ক্ষেত্র 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      বিরতি;
			    ক্ষেত্র 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      বিরতি;
			    }
			}
			অন্যথা
			{
			    // unsigned
			    নির্বাচন (দৈর্ঘ্য_modifier)
			    {
			    ক্ষেত্র '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       বিরতি;
			    ক্ষেত্র 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(unsigned short)uint_arg);
				      বিরতি;
			    ক্ষেত্র 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      বিরতি;
			    ক্ষেত্র 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      বিরতি;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			যদি (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			যদি (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t অঙ্ক_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			যদি (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    যদি (!precision_specified
					     || precision < অঙ্ক_of_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = অঙ্ক_of_digits + 1;
			    }
			}
			// zero padding to specified precision?
			যদি (অঙ্ক_of_digits < precision)
			    সংখ্যা_of_zeros_to_pad = precision - অঙ্ক_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    যদি (!justify_বাম && zero_padding)
		    {
			পূর্ণসংখ্যা n = (পূর্ণসংখ্যা)(min_field_width - (str_arg_l
						    + সংখ্যা_of_zeros_to_pad));
			যদি (n > 0)
			    সংখ্যা_of_zeros_to_pad += n;
		    }
		    বিরতি;
		}

	    ক্ষেত্র 'f':
	    ক্ষেত্র 'F':
	    ক্ষেত্র 'e':
	    ক্ষেত্র 'E':
	    ক্ষেত্র 'g':
	    ক্ষেত্র 'G':
		{
		    // Floating point.
		    দ্বিগুণ	f;
		    দ্বিগুণ	abs_f;
		    অক্ষর	format[40];
		    পূর্ণসংখ্যা		l;
		    পূর্ণসংখ্যা		remove_trailing_zeroes = FALSE;

		    f =
# যদি defined(FEAT_EVAL)
			tvs != ফাঁকা ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, দ্বিগুণ));

		    abs_f = f < 0 ? -f : f;

		    যদি (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			যদি ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			অন্যথা
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    যদি ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# অন্যথা
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
		    অন্যথা
		    {
			যদি (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			অন্যথা যদি (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			অন্যথা
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    যদি (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    যদি (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				যদি ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				যদি (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (পূর্ণসংখ্যা)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			যদি (remove_trailing_zeroes)
			{
			    পূর্ণসংখ্যা i;
			    অক্ষর *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    যদি (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    অন্যথা
			    {
				tp = (অক্ষর *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				যদি (tp != ফাঁকা)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    যদি (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    যখন (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    যদি (tp != ফাঁকা && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				যখন (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			অন্যথা
			{
			    অক্ষর *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (অক্ষর *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    যদি (tp != ফাঁকা && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    যদি (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			সংখ্যা_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    বিরতি;
		}

	    পূর্বনির্ধারিত:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_বাম = 1;
		min_field_width = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		যদি (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		বিরতি;
	    }

	    যদি (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    যদি (!justify_বাম)
	    {
		// left padding with blank or zero
		পূর্ণসংখ্যা pn = (পূর্ণসংখ্যা)(min_field_width - (str_arg_l + সংখ্যা_of_zeros_to_pad));

		যদি (pn > 0)
		{
		    যদি (str_l < str_m)
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
	    যদি (সংখ্যা_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    অন্যথা
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		পূর্ণসংখ্যা zn = (পূর্ণসংখ্যা)zero_padding_insertion_ind;

		যদি (zn > 0)
		{
		    যদি (str_l < str_m)
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
		zn = (পূর্ণসংখ্যা)সংখ্যা_of_zeros_to_pad;
		যদি (zn > 0)
		{
		    যদি (str_l < str_m)
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
		পূর্ণসংখ্যা sn = (পূর্ণসংখ্যা)(str_arg_l - zero_padding_insertion_ind);

		যদি (sn > 0)
		{
		    যদি (str_l < str_m)
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
	    যদি (justify_বাম)
	    {
		// right blank padding to the field width
		পূর্ণসংখ্যা pn = (পূর্ণসংখ্যা)(min_field_width
				      - (str_arg_l + সংখ্যা_of_zeros_to_pad));

		যদি (pn > 0)
		{
		    যদি (str_l < str_m)
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

    যদি (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    যদি (tvs != ফাঁকা && tvs[অঙ্ক_posarg != 0 ? অঙ্ক_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((অক্ষর*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    ফেরত (পূর্ণসংখ্যা)str_l;
}

#endif // PROTO
