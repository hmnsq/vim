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
#شامل "vim.h"

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
    اگر (p != پوچ)
	mch_memmove(p, string, len);
    بازگشت p;
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
    اگر (p == پوچ)
	بازگشت پوچ;

    STRNCPY(p, string, len);
    p[len] = NUL;
    بازگشت p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    بازگشت vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    صحیح		cc,
    صحیح		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    صحیح		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    برای (p = string; *p; p++)
    {
	اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    ادامه;
	}
	اگر (vim_strchr(esc_chars, *p) != پوچ || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    اگر (escaped_string == پوچ)
	بازگشت پوچ;
    p2 = escaped_string;
    برای (p = string; *p; p++)
    {
	اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    ادامه;
	}
	اگر (vim_strchr(esc_chars, *p) != پوچ || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    بازگشت escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    صحیح
csh_like_shell(تهی)
{
    بازگشت (strstr((نویسه *)gettail(p_sh), "csh") != پوچ);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    ایستا صحیح
fish_like_shell(تهی)
{
    بازگشت (strstr((نویسه *)gettail(p_sh), "fish") != پوچ);
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
vim_strsave_shellescape(char_u *string, صحیح do_special, صحیح do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    صحیح		csh_like;
    صحیح		fish_like;
    char_u	*shname;
    صحیح		powershell;
#ifdef MSWIN
    صحیح		double_quotes;
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
    powershell = strstr((نویسه *)shname, "pwsh") != پوچ;
#ifdef MSWIN
    powershell = powershell || strstr((نویسه *)shname, "powershell") != پوچ;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (unsigned)STRLEN(string) + 3;  // two quotes and a trailing NUL
    برای (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	اگر (double_quotes)
	{
	    اگر (*p == '"')
		++length;		// " -> ""
	}
	وگرنه
#endif
	اگر (*p == '\'')
	{
	    اگر (powershell)
		length += 2;		// ' => ''
	    وگرنه
		length += 3;		// ' => '\''
	}
	اگر ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    اگر (csh_like && do_special)
		++length;		// insert backslash
	}
	اگر (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	اگر (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    اگر (escaped_string != پوچ)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	اگر (double_quotes)
	    *d++ = '"';
	وگرنه
#endif
	    *d++ = '\'';

	برای (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    اگر (double_quotes)
	    {
		اگر (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    ادامه;
		}
	    }
	    وگرنه
#endif
	    اگر (*p == '\'')
	    {
		اگر (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		وگرنه
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		ادامه;
	    }
	    اگر ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		اگر (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		ادامه;
	    }
	    اگر (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		ادامه;
	    }
	    اگر (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		ادامه;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	اگر (double_quotes)
	    *d++ = '"';
	وگرنه
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    بازگشت escaped_string;
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
    بازگشت p1;
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
    بازگشت p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    تهی
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    صحیح	    c;

    اگر (p == پوچ)
	بازگشت;

    p2 = p;
    وقتی ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#اگر defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    ایستا char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    اگر (res != پوچ)
	وقتی (*p != NUL)
	{
	    صحیح		l;

	    اگر (enc_utf8)
	    {
		صحیح	c, uc;
		صحیح	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		اگر (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		اگر (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    اگر (s == پوچ)
		    {
			vim_free(res);
			بازگشت پوچ;
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
	    وگرنه اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    وگرنه
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    بازگشت res;
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

    اگر (res != پوچ)
	وقتی (*p != NUL)
	{
	    صحیح		l;

	    اگر (enc_utf8)
	    {
		صحیح	c, lc;
		صحیح	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		اگر (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		اگر (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    اگر (s == پوچ)
		    {
			vim_free(res);
			بازگشت پوچ;
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
	    وگرنه اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    وگرنه
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    بازگشت res;
}
#endif

/*
 * delete spaces at the end of a string
 */
    تهی
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    وقتی (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    تهی
vim_strncpy(char_u *to, char_u *from, size_t len)
{
    STRNCPY(to, from, len);
    to[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    تهی
vim_strcat(char_u *to, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(to);
    size_t fromlen = STRLEN(from);

    اگر (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    وگرنه
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(نویسه *s, size_t maxlen)
{
    size_t i;
    برای (i = 0; i < maxlen; ++i)
	اگر (s[i] == NUL)
	    بشکن;
    بازگشت i;
}

#اگر !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    صحیح
vim_stricmp(نویسه *s1, نویسه *s2)
{
    صحیح		i;

    برای (;;)
    {
	i = (صحیح)TOLOWER_LOC(*s1) - (صحیح)TOLOWER_LOC(*s2);
	اگر (i != 0)
	    بازگشت i;			    // this character different
	اگر (*s1 == NUL)
	    بشکن;			    // strings match until NUL
	++s1;
	++s2;
    }
    بازگشت 0;				    // strings match
}
#endif

#اگر !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    صحیح
vim_strnicmp(نویسه *s1, نویسه *s2, size_t len)
{
    صحیح		i;

    وقتی (len > 0)
    {
	i = (صحیح)TOLOWER_LOC(*s1) - (صحیح)TOLOWER_LOC(*s2);
	اگر (i != 0)
	    بازگشت i;			    // this character different
	اگر (*s1 == NUL)
	    بشکن;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    بازگشت 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    صحیح
vim_strnicmp_asc(نویسه *s1, نویسه *s2, size_t len)
{
    صحیح                i = 0;

    وقتی (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	اگر (i != 0)
	    بشکن;			// this character is different
	اگر (*s1 == NUL)
	    بشکن;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    بازگشت i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, صحیح c)
{
    char_u	*p;
    صحیح		b;

    p = string;
    اگر (enc_utf8 && c > 0 && c < 0x80)
	بازگشت vim_strbyte(string, c);
    اگر (enc_utf8 && c >= 0x80)
    {
	وقتی (*p != NUL)
	{
	    صحیح l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    اگر (utf_ptr2char(p) == c && l > 1)
		بازگشت p;
	    p += l;
	}
	بازگشت پوچ;
    }
    اگر (enc_dbcs != 0 && c > 255)
    {
	صحیح	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	وقتی ((b = *p) != NUL)
	{
	    اگر (b == c && p[1] == n2)
		بازگشت p;
	    p += (*mb_ptr2len)(p);
	}
	بازگشت پوچ;
    }
    اگر (has_mbyte)
    {
	وقتی ((b = *p) != NUL)
	{
	    اگر (b == c)
		بازگشت p;
	    p += (*mb_ptr2len)(p);
	}
	بازگشت پوچ;
    }
    وقتی ((b = *p) != NUL)
    {
	اگر (b == c)
	    بازگشت p;
	++p;
    }
    بازگشت پوچ;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, صحیح c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    اگر (c <= 0 || c > 255)
	بازگشت پوچ;
    بازگشت (char_u *)strchr((نویسه *)string, c);
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
vim_strrchr(char_u *string, صحیح c)
{
    char_u	*retval = پوچ;
    char_u	*p = string;

    وقتی (*p)
    {
	اگر (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    بازگشت retval;
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
    وقتی (*s)
    {
	اگر (vim_strchr(charset, *s) != پوچ)
	    بازگشت s;
	MB_PTR_ADV(s);
    }
    بازگشت پوچ;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
ایستا صحیح sort_compare(ثابت تهی *s1, ثابت تهی *s2);

    ایستا صحیح
sort_compare(ثابت تهی *s1, ثابت تهی *s2)
{
    بازگشت STRCMP(*(نویسه **)s1, *(نویسه **)s2);
}

    تهی
sort_strings(
    char_u	**files,
    صحیح		شمار)
{
    qsort((تهی *)files, (size_t)شمار, اندازه(char_u *), sort_compare);
}

#اگر defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    صحیح
has_non_ascii(char_u *s)
{
    char_u	*p;

    اگر (s != پوچ)
	برای (p = s; *p != NUL; ++p)
	    اگر (*p >= 128)
		بازگشت TRUE;
    بازگشت FALSE;
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
    size_t  l = str1 == پوچ ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == پوچ ? 0 : STRLEN(str2)) + 1L);
    اگر (dest == پوچ)
	بازگشت پوچ;
    اگر (str1 == پوچ)
	*dest = NUL;
    وگرنه
	STRCPY(dest, str1);
    اگر (str2 != پوچ)
	STRCPY(dest + l, str2);
    بازگشت dest;
}

#اگر defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    اگر (rev == پوچ)
	بازگشت پوچ;

    برای (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	اگر (has_mbyte)
	{
	    صحیح mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	وگرنه
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    بازگشت rev;
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, صحیح function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    اگر (str != پوچ)
    {
	len += (unsigned)STRLEN(str);
	برای (p = str; *p != NUL; MB_PTR_ADV(p))
	    اگر (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    اگر (r == پوچ)
	بازگشت پوچ;

    اگر (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    وگرنه
	*r++ = '\'';
    اگر (str != پوچ)
	برای (p = str; *p != NUL; )
	{
	    اگر (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    اگر (function)
	*r++ = ')';
    *r++ = NUL;
    بازگشت s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_شمار(char_u *haystack, char_u *needle, صحیح ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    اگر (p == پوچ || needle == پوچ || *needle == NUL)
	بازگشت 0;

    size_t  needlelen = STRLEN(needle);
    اگر (ic)
    {
	وقتی (*p != NUL)
	{
	    اگر (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    وگرنه
		MB_PTR_ADV(p);
	}
    }
    وگرنه
	وقتی ((next = (char_u *)strstr((نویسه *)p, (نویسه *)needle)) != پوچ)
	{
	    ++n;
	    p = next + needlelen;
	}

    بازگشت n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    ایستا صحیح
copy_first_char_to_tv(char_u *درون_داد, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    صحیح		len;

    اگر (درون_داد == پوچ || output == پوچ)
	بازگشت -1;

    len = has_mbyte ? mb_ptr2len(درون_داد) : 1;
    STRNCPY(buf, درون_داد, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    بازگشت output->vval.v_string == پوچ ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    تهی
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*عبارت,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    صحیح		len = 0;
    صحیح		idx = 0;
    صحیح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_کلید, VAR_عدد);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_عبارت_get_funccal(عبارت, &newtv);

    ga_init2(&ga, اندازه(نویسه), 80);
    برای (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	اگر (len < 0)
	    بشکن;

	set_vim_var_nr(VV_کلید, idx);
	اگر (filter_map_one(&tv, عبارت, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    بشکن;
	}
	اگر (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    اگر (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		بشکن;
	    }
	    وگرنه
		ga_concat(&ga, newtv.vval.v_string);
	}
	وگرنه اگر (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    اگر (fc != پوچ)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    تهی
string_reduce(
	typval_T	*argvars,
	typval_T	*عبارت,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    صحیح		len;
    typval_T	argv[3];
    صحیح		r;
    صحیح		called_emsg_start = called_emsg;
    funccall_T	*fc;

    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	اگر (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_مقدار), "String");
	    بازگشت;
	}
	len = copy_first_char_to_tv(p, rettv);
	اگر (len < 0)
	    بازگشت;
	p += len;
    }
    وگرنه اگر (check_for_string_arg(argvars, 2) == FAIL)
	بازگشت;
    وگرنه
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_عبارت_get_funccal(عبارت, rettv);

    برای ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	اگر (len < 0)
	{
	    *rettv = argv[0];
	    بشکن;
	}

	r = eval_عبارت_typval(عبارت, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	اگر (r == FAIL || called_emsg != called_emsg_start)
	    بشکن;
    }

    اگر (fc != پوچ)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    ایستا تهی
byteidx_common(typval_T *argvars, typval_T *rettv, صحیح comp)
{
    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	بازگشت;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_عدد_chk(&argvars[1], پوچ);
    اگر (str == پوچ || idx < 0)
	بازگشت;

    varnumber_T	utf16idx = FALSE;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	صحیح error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	اگر (error)
	    بازگشت;
	اگر (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_عدد_as_bool_nr), utf16idx);
	    بازگشت;
	}
    }

    صحیح (*ptr2len)(char_u *);
    اگر (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    وگرنه
	ptr2len = mb_ptr2len;

    char_u *t = str;
    برای ( ; idx > 0; idx--)
    {
	اگر (*t == NUL)		// EOL reached
	    بازگشت;
	اگر (utf16idx)
	{
	    صحیح clen = ptr2len(t);
	    صحیح c = (clen > 1) ? utf_ptr2char(t) : *t;
	    اگر (c > 0xFFFF)
		idx--;
	    اگر (idx > 0)
		t += clen;
	}
	وگرنه اگر (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_عدد = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    تهی
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    تهی
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    تهی
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;

    اگر (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	بازگشت;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_عدد_chk(&argvars[1], پوچ);
    اگر (str == پوچ || idx < 0)
	بازگشت;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    صحیح (*ptr2len)(char_u *);
    اگر (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    وگرنه
	ptr2len = mb_ptr2len;

    char_u	*p;
    صحیح		len;
    برای (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	اگر (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    اگر (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_عدد = len;
	    بازگشت;
	}
	اگر (utf16idx)
	{
	    idx--;
	    صحیح clen = ptr2len(p);
	    صحیح c = (clen > 1) ? utf_ptr2char(p) : *p;
	    اگر (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_عدد = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    ایستا صحیح
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    اگر (convert_setup(&vimconv, from, to) == FAIL)
	بازگشت FAIL;
    vimconv.vc_fail = TRUE;
    اگر (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	اگر (ret->string == پوچ)
	    ret->length = 0;
	وگرنه
	    ret->length = str->length;
    }
    وگرنه
    {
	صحیح len = (صحیح)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, پوچ, پوچ);

    بازگشت (ret->string == پوچ) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    ایستا تهی
blob_from_string(char_u *str, blob_T *blob)
{
    صحیح	    len = (صحیح)STRLEN(str);
    char_u  *dest;

    اگر (len == 0)
	بازگشت;
    اگر (ga_grow(&blob->bv_ga, len) == FAIL)
	بازگشت;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    برای (صحیح i = 0; i < len; ++i)
	اگر (dest[i] == NL)
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
    ایستا صحیح
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	سطر_len;

    اگر (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	بازگشت (ret->string == پوچ) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    سطر_len = (nl == پوچ) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(سطر_len + 1);
    اگر (ret->string == پوچ)
    {
	ret->length = 0;
	بازگشت FAIL;
    }
    اگر (سطر_len > 0)
	mch_memmove(ret->string, src, (size_t)سطر_len);
    ret->string[سطر_len] = NUL;
    ret->length = (size_t)سطر_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    برای (long i = 0; i < سطر_len; i++)
	اگر (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + سطر_len + (nl != پوچ ? 1 : 0);
    بازگشت OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    ایستا char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    اگر (from_encoding_raw == پوچ)
	بازگشت پوچ;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    وقتی (*s != NUL)
    {
	اگر (*s == '_')
	    *pe++ = '-';
	وگرنه
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    اگر ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    بازگشت from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    ایستا تهی
append_converted_string_to_list(
	string_T *converted,
	صحیح validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    اگر (converted->string != پوچ)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	وقتی (p < end)
	{
	    string_T	سطر;
	    char_u	*سطر_start = p;

	    وقتی (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    سطر.length = (size_t)(p - سطر_start);
	    سطر.string = vim_strnsave(سطر_start, سطر.length);
	    اگر (سطر.string != پوچ)
	    {
		اگر (validate_utf8 && !utf_valid_string(سطر.string, پوچ))
		{
		    vim_free(سطر.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    بازگشت; // Stop processing
		}
		اگر (list_append_string(list, سطر.string, (صحیح)سطر.length) == FAIL)
		{
		    vim_free(سطر.string);
		    vim_free(converted->string);
		    بازگشت; // Stop processing on append failure
		}
		vim_free(سطر.string);
	    }
	    وگرنه
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		بازگشت;
	    }

	    اگر (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    وگرنه
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    ایستا صحیح
append_validated_سطر_to_list(string_T *سطر, صحیح validate_utf8, list_T *list)
{
    اگر (validate_utf8 && !utf_valid_string(سطر->string, پوچ))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(سطر->string);
	بازگشت FAIL;
    }

    صحیح ret = list_append_string(list, سطر->string, (صحیح)سطر->length);
    vim_free(سطر->string);
    بازگشت ret;
}

    تهی
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    صحیح		blen;
    long	idx;
    صحیح		validate_utf8 = FALSE;

    اگر (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	بازگشت;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    blob = argvars->vval.v_blob;
    اگر (blob == پوچ)
	بازگشت;
    blen = blob_len(blob);

    char_u	*from_encoding = پوچ;
    char_u	*from_encoding_raw = پوچ;  // Encoding name with endianness preserved for iconv
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	اگر (d != پوچ)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    اگر (enc != پوچ)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		اگر (from_encoding_raw == پوچ)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    بازگشت;
		}
	    }
	}
    }

    اگر (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    اگر (from_encoding != پوچ && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    صحیح from_prop = 0;
    اگر (from_encoding != پوچ)
	from_prop = enc_canon_props(from_encoding);
    اگر (from_encoding != پوچ && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	صحیح nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	اگر (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    اگر (blen > 0)
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
	اگر (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    برو done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	صحیح len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, پوچ);
	converted.length = len;
	convert_setup(&vimconv, پوچ, پوچ);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    وگرنه
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	وقتی (idx < blen)
	{
	    string_T	str;

	    اگر (string_from_blob(blob, &idx, &str) != OK)
		بشکن;

	    اگر (from_encoding != پوچ)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		صحیح	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		اگر (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    برو done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    اگر (append_validated_سطر_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		برو done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    اگر (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

done:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    تهی
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    اگر (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	بازگشت;

    اگر (rettv_blob_alloc(rettv) == FAIL)
	بازگشت;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    اگر (list == پوچ)
	بازگشت;

    char_u	*to_encoding = پوچ;
    char_u	*to_encoding_raw = پوچ;  // Encoding name with endianness preserved for iconv
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	اگر (d != پوچ)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    اگر (enc != پوچ)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		اگر (to_encoding_raw == پوچ)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    بازگشت;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    صحیح to_prop = 0;
    اگر (to_encoding != پوچ)
	to_prop = enc_canon_props(to_encoding);
    اگر (to_encoding != پوچ && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_اقلام(list, li)
	{
	    char_u *s;

	    اگر (li->li_tv.v_type != VAR_STRING)
		ادامه;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    اگر (li != list->lv_first)
		ga_append(&str_ga, NL);
	    اگر (s != پوچ && *s != NUL)
	    {
		صحیح slen = (صحیح)STRLEN(s);

		اگر (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    برو done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	اگر (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    اگر (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		برو done;
	    }
	    vimconv.vc_fail = TRUE;

	    صحیح		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, پوچ);
	    convert_setup(&vimconv, پوچ, پوچ);
	    ga_clear(&str_ga);

	    اگر (converted == پوچ)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		برو done;
	    }
	    اگر (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	وگرنه
	    ga_clear(&str_ga);
    }
    وگرنه
    {
	FOR_ALL_LIST_اقلام(list, li)
	{
	    اگر (li->li_tv.v_type != VAR_STRING)
		ادامه;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    اگر (str.string == پوچ)
		STR_LITERAL_SET(str, "");
	    وگرنه
		str.length = STRLEN(str.string);

	    اگر (to_encoding != پوچ)
	    {
		صحیح	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		اگر (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    برو done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    اگر (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    اگر (to_encoding != پوچ)
		vim_free(str.string);
	}
    }

done:
    اگر (to_encoding != پوچ)
	vim_free(to_encoding);
    اگر (to_encoding_raw != پوچ)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    تهی
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		utf8 = FALSE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (صحیح)tv_get_bool_chk(&argvars[1], پوچ);

    p = tv_get_string(&argvars[0]);

    اگر (has_mbyte || utf8)
    {
	صحیح (*ptr2len)(char_u *);
	صحیح (*ptr2char)(char_u *);

	اگر (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	وگرنه
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	برای ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_عدد(rettv->vval.v_list, (*ptr2char)(p));
    }
    وگرنه
	برای ( ; *p != NUL; ++p)
	    list_append_عدد(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    تهی
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    صحیح		base = 10;
    char_u	*p;
    varnumber_T	n;
    صحیح		what = 0;
    صحیح		isneg;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (صحیح)tv_get_عدد(&argvars[1]);
	اگر (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    بازگشت;
	}
	اگر (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    اگر (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    انتخاب (base)
    {
	حالت 2: what |= STR2NR_BIN + STR2NR_FORCE; بشکن;
	حالت 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; بشکن;
	حالت 16: what |= STR2NR_HEX + STR2NR_FORCE; بشکن;
    }
    vim_str2nr(p, پوچ, پوچ, what, &n, پوچ, 0, FALSE, پوچ);
    // Text after the number is silently ignored.
    اگر (isneg)
	rettv->vval.v_عدد = -n;
    وگرنه
	rettv->vval.v_عدد = n;

}

/*
 * "strgetchar()" function
 */
    تهی
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    صحیح		len;
    صحیح		error = FALSE;
    صحیح		charidx;
    صحیح		byteidx = 0;

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    str = tv_get_string_chk(&argvars[0]);
    اگر (str == پوچ)
	بازگشت;
    len = (صحیح)STRLEN(str);
    charidx = (صحیح)tv_get_عدد_chk(&argvars[1], &error);
    اگر (error)
	بازگشت;

    وقتی (charidx >= 0 && byteidx < len)
    {
	اگر (charidx == 0)
	{
	    rettv->vval.v_عدد = mb_ptr2char(str + byteidx);
	    بشکن;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    تهی
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    صحیح		start_idx;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL))
	بازگشت;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_عدد = -1;
    اگر (needle == پوچ || haystack == پوچ)
	بازگشت;		// type error; errmsg already given

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	صحیح	    error = FALSE;

	start_idx = (صحیح)tv_get_عدد_chk(&argvars[2], &error);
	اگر (error || start_idx >= (صحیح)STRLEN(haystack))
	    بازگشت;
	اگر (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((نویسه *)haystack, (نویسه *)needle);
    اگر (pos != پوچ)
	rettv->vval.v_عدد = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    تهی
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    اگر (rettv->vval.v_string != پوچ && tofree == پوچ)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    تهی
f_strlen(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && check_for_string_or_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    ایستا تهی
strchar_common(typval_T *argvars, typval_T *rettv, صحیح skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    صحیح			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    وقتی (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_عدد = len;
}

/*
 * "strcharlen()" function
 */
    تهی
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && check_for_string_or_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    تهی
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	صحیح error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	اگر (error)
	    بازگشت;
	اگر (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_عدد_as_bool_nr), skipcc);
	    بازگشت;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    تهی
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;

    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	بازگشت;

    varnumber_T countcc = FALSE;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    صحیح			(*func_mb_ptr2char_adv)(char_u **pp);
    صحیح			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    وقتی (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	اگر (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_عدد = len;
}

/*
 * "strdisplaywidth()" function
 */
    تهی
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    صحیح		col = 0;

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    s = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	col = (صحیح)tv_get_عدد(&argvars[1]);

    rettv->vval.v_عدد = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    تهی
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_عدد = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    تهی
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		nchar;
    صحیح		nbyte = 0;
    صحیح		charlen;
    صحیح		skipcc = FALSE;
    صحیح		len = 0;
    صحیح		slen;
    صحیح		error = FALSE;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	بازگشت;

    p = tv_get_string(&argvars[0]);
    slen = (صحیح)STRLEN(p);

    nchar = (صحیح)tv_get_عدد_chk(&argvars[1], &error);
    اگر (!error)
    {
	اگر (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    اگر (error)
		بازگشت;
	    اگر (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_عدد_as_bool_nr), skipcc);
		بازگشت;
	    }
	}

	اگر (nchar > 0)
	    وقتی (nchar > 0 && nbyte < slen)
	    {
		اگر (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		وگرنه
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	وگرنه
	    nbyte = nchar;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (صحیح)tv_get_عدد(&argvars[2]);
	    وقتی (charlen > 0 && nbyte + len < slen)
	    {
		صحیح off = nbyte + len;

		اگر (off < 0)
		    len += 1;
		وگرنه
		{
		    اگر (skipcc)
			len += mb_ptr2len(p + off);
		    وگرنه
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	وگرنه
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    اگر (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    وگرنه اگر (nbyte > slen)
	nbyte = slen;
    اگر (len < 0)
	len = 0;
    وگرنه اگر (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    تهی
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		n;
    صحیح		len;
    صحیح		slen;
    صحیح		error = FALSE;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	بازگشت;

    p = tv_get_string(&argvars[0]);
    slen = (صحیح)STRLEN(p);

    n = (صحیح)tv_get_عدد_chk(&argvars[1], &error);
    اگر (error)
	len = 0;
    وگرنه اگر (argvars[2].v_type != VAR_UNKNOWN)
	len = (صحیح)tv_get_عدد(&argvars[2]);
    وگرنه
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    اگر (n < 0)
    {
	len += n;
	n = 0;
    }
    وگرنه اگر (n > slen)
	n = slen;
    اگر (len < 0)
	len = 0;
    وگرنه اگر (n + len > slen)
	len = slen - n;

    اگر (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	صحیح off;

	// length in characters
	برای (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    تهی
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = پوچ;
    صحیح		haystack_len, end_idx;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL))
	بازگشت;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_عدد = -1;
    اگر (needle == پوچ || haystack == پوچ)
	بازگشت;		// type error; errmsg already given

    haystack_len = (صحیح)STRLEN(haystack);
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (صحیح)tv_get_عدد_chk(&argvars[2], پوچ);
	اگر (end_idx < 0)
	    بازگشت;	// can never find a match
    }
    وگرنه
	end_idx = haystack_len;

    اگر (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    وگرنه
    {
	برای (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((نویسه *)rest, (نویسه *)needle);
	    اگر (rest == پوچ || rest > haystack + end_idx)
		بشکن;
	    lastmatch = rest;
	}
    }

    اگر (lastmatch == پوچ)
	rettv->vval.v_عدد = -1;
    وگرنه
	rettv->vval.v_عدد = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    تهی
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    تهی
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;

    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_عدد_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    بازگشت;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_عدد_chk(&argvars[1], پوچ);
    اگر (str == پوچ || idx < 0)
	بازگشت;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    صحیح (*ptr2len)(char_u *);
    اگر (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    وگرنه
	ptr2len = mb_ptr2len;

    char_u	*p;
    صحیح		len;
    صحیح		utf16idx = 0;
    برای (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	اگر (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    اگر (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_عدد = len;
	    بازگشت;
	}
	utf16idx = len;
	صحیح clen = ptr2len(p);
	صحیح c = (clen > 1) ? utf_ptr2char(p) : *p;
	اگر (c > 0xFFFF)
	    len++;
	p += clen;
	اگر (charidx)
	    idx--;
    }

    rettv->vval.v_عدد = utf16idx;
}

/*
 * "tolower(string)" function
 */
    تهی
f_tolower(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    تهی
f_toupper(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    تهی
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*in_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    صحیح		inlen;
    صحیح		fromlen;
    صحیح		tolen;
    صحیح		idx;
    char_u	*cpstr;
    صحیح		cplen;
    صحیح		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	بازگشت;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;
    اگر (fromstr == پوچ || tostr == پوچ)
	    بازگشت;		// type error; errmsg already given
    ga_init2(&ga, اندازه(نویسه), 80);

    اگر (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	اگر (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    بازگشت;
	}

    // fromstr and tostr have to contain the same number of chars
    وقتی (*in_str != NUL)
    {
	اگر (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    برای (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		اگر (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    برای (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			اگر (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    بشکن;
			}
		    }
		    اگر (*p == NUL)	// tostr is shorter than fromstr
			برو error;
		    بشکن;
		}
		++idx;
	    }

	    اگر (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		برای (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		اگر (idx != 0)
		    برو error;
	    }

	    (تهی)ga_grow(&ga, cplen);
	    mch_memmove((نویسه *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	وگرنه
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    اگر (p != پوچ)
		ga_append(&ga, tostr[p - fromstr]);
	    وگرنه
		ga_append(&ga, *in_str);
	    ++in_str;
	}
    }

    // add a terminating NUL
    (تهی)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_data;
}

/*
 * "trim({expr})" function
 */
    تهی
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*head;
    char_u	*mask = پوچ;
    char_u	*tail;
    char_u	*prev;
    char_u	*p;
    صحیح		c1;
    صحیح		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 2) == FAIL)))
	بازگشت;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    اگر (head == پوچ)
	بازگشت;

    اگر (check_for_opt_string_arg(argvars, 1) == FAIL)
	بازگشت;

    اگر (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	اگر (*mask == NUL)
	    mask = پوچ;

	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    صحیح	error = 0;

	    // leading or trailing characters to trim
	    dir = (صحیح)tv_get_عدد_chk(&argvars[2], &error);
	    اگر (error)
		بازگشت;
	    اگر (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		بازگشت;
	    }
	}
    }

    اگر (dir == 0 || dir == 1)
    {
	// Trim leading characters
	وقتی (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    اگر (mask == پوچ)
	    {
		اگر (c1 > ' ' && c1 != 0xa0)
		    بشکن;
	    }
	    وگرنه
	    {
		برای (p = mask; *p != NUL; MB_PTR_ADV(p))
		    اگر (c1 == PTR2CHAR(p))
			بشکن;
		اگر (*p == NUL)
		    بشکن;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    اگر (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	برای (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    اگر (mask == پوچ)
	    {
		اگر (c1 > ' ' && c1 != 0xa0)
		    بشکن;
	    }
	    وگرنه
	    {
		برای (p = mask; *p != NUL; MB_PTR_ADV(p))
		    اگر (c1 == PTR2CHAR(p))
			بشکن;
		اگر (*p == NUL)
		    بشکن;
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
    ایستا char_u *
uri_decode(char_u *str)
{
    اگر (str == پوچ)
	بازگشت پوچ;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    اگر (!decoded)
	بازگشت پوچ;

    char_u	*p = decoded;
    size_t	i = 0;

    وقتی (i < len)
    {
	اگر (str[i] == '%')
	{
	    اگر (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		اگر (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    وگرنه
	    {
		صحیح val = hexhex2nr(&str[i + 1]);
		اگر (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		وگرنه
		{
		    // invalid hex digits following "%"
		    برای (صحیح j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	وگرنه
	    *p++ = str[i++];
    }

    *p = NUL;

    بازگشت decoded;
}

/*
 * "uri_decode({str})" function
 */
    تهی
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

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
    ایستا char_u *
uri_encode(char_u *str)
{
    اگر (str == پوچ)
	بازگشت پوچ;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    اگر (encoded == پوچ)
	بازگشت پوچ;

    char_u *p = encoded;

    برای (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	اگر (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	وگرنه
	{
	    sprintf((نویسه *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    بازگشت encoded;
}

/*
 * "uri_encode({str})" function
 */
    تهی
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

ایستا نویسه *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ایستا varnumber_T
tv_nr(typval_T *tvs, صحیح *idxp)
{
    صحیح		idx = *idxp - 1;
    varnumber_T	n = 0;
    صحیح		err = FALSE;

    اگر (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    وگرنه
    {
	++*idxp;
	n = tv_get_عدد_chk(&tvs[idx], &err);
	اگر (err)
	    n = 0;
    }
    بازگشت n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    ایستا نویسه *
tv_str(typval_T *tvs, صحیح *idxp, char_u **tofree)
{
    صحیح		    idx = *idxp - 1;
    نویسه	    *s = پوچ;
    ایستا char_u   numbuf[NUMBUFLEN];

    اگر (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    وگرنه
    {
	++*idxp;
	اگر (tofree != پوچ)
	    s = (نویسه *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	وگرنه
	    s = (نویسه *)tv_get_string_chk(&tvs[idx]);
    }
    بازگشت s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ایستا دوگانه
tv_float(typval_T *tvs, صحیح *idxp)
{
    صحیح		idx = *idxp - 1;
    دوگانه	f = 0;

    اگر (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    وگرنه
    {
	++*idxp;
	اگر (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	وگرنه اگر (tvs[idx].v_type == VAR_عدد)
	    f = (دوگانه)tvs[idx].vval.v_عدد;
	وگرنه
	    emsg(_(e_expected_float_argument_for_printf));
    }
    بازگشت f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    ایستا ثابت نویسه *
infinity_str(صحیح positive,
	     نویسه fmt_spec,
	     صحیح force_sign,
	     صحیح space_for_positive)
{
    ایستا ثابت نویسه *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    صحیح idx = positive * (1 + force_sign + force_sign * space_for_positive);

    اگر (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    بازگشت table[idx];
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
    صحیح
vim_snprintf_add(نویسه *str, size_t str_m, ثابت نویسه *fmt, ...)
{
    va_list	ap;
    صحیح		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    اگر (str_m <= len)
	space = 0;
    وگرنه
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    بازگشت str_l;
}

    صحیح
vim_snprintf(نویسه *str, size_t str_m, ثابت نویسه *fmt, ...)
{
    va_list	ap;
    صحیح		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    بازگشت str_l;
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
vim_snprintf_safelen(نویسه *str, size_t str_m, ثابت نویسه *fmt, ...)
{
    va_list ap;
    صحیح	    str_l;

    اگر (str_m == 0)
	بازگشت 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, پوچ);
    va_end(ap);

    اگر (str_l < 0)
    {
	*str = NUL;
	بازگشت 0;
    }
    بازگشت ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    صحیح
vim_vsnprintf(
    نویسه	*str,
    size_t	str_m,
    ثابت نویسه	*fmt,
    va_list	ap)
{
    بازگشت vim_vsnprintf_typval(str, str_m, fmt, ap, پوچ);
}

شمارش
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
    ایستا صحیح
format_typeof(
    ثابت نویسه	*type)
{
    // allowed values: \0, h, l, L
    نویسه    length_modifier = '\0';

    // current conversion specifier character
    نویسه    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    اگر (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	اگر (length_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    length_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    انتخاب (fmt_spec)
    {
	حالت 'i': fmt_spec = 'd'; بشکن;
	حالت '*': fmt_spec = 'd'; length_modifier = 'h'; بشکن;
	حالت 'D': fmt_spec = 'd'; length_modifier = 'l'; بشکن;
	حالت 'U': fmt_spec = 'u'; length_modifier = 'l'; بشکن;
	حالت 'O': fmt_spec = 'o'; length_modifier = 'l'; بشکن;
	پیشفرض: بشکن;
    }

    // get parameter value, do initial processing
    انتخاب (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    حالت '%':
	بازگشت TYPE_PERCENT;

    حالت 'c':
	بازگشت TYPE_CHAR;

    حالت 's':
    حالت 'S':
	بازگشت TYPE_STRING;

    حالت 'd': حالت 'u':
    حالت 'b': حالت 'B':
    حالت 'o':
    حالت 'x': حالت 'X':
    حالت 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //مقدار

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    اگر (fmt_spec == 'p')
		بازگشت TYPE_POINTER;
	    وگرنه اگر (fmt_spec == 'b' || fmt_spec == 'B')
		بازگشت TYPE_UNSIGNEDLONGLONGINT;
	    وگرنه اگر (fmt_spec == 'd')
	    {
		// signed
		انتخاب (length_modifier)
		{
		حالت '\0':
		حالت 'h':
		    // char and short arguments are passed as int.
		    بازگشت TYPE_INT;
		حالت 'l':
		    بازگشت TYPE_LONGINT;
		حالت 'L':
		    بازگشت TYPE_LONGLONGINT;
		}
	    }
	    وگرنه
	    {
		// unsigned
		انتخاب (length_modifier)
		{
		    حالت '\0':
		    حالت 'h':
			بازگشت TYPE_UNSIGNEDINT;
		    حالت 'l':
			بازگشت TYPE_UNSIGNEDLONGINT;
		    حالت 'L':
			بازگشت TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	بشکن;

    حالت 'f':
    حالت 'F':
    حالت 'e':
    حالت 'E':
    حالت 'g':
    حالت 'G':
	بازگشت TYPE_FLOAT;
    }

    بازگشت TYPE_UNKNOWN;
}

    ایستا نویسه *
format_typename(
    ثابت نویسه  *type)
{
    انتخاب (format_typeof(type))
    {
	حالت TYPE_INT:
	    بازگشت typename_int;

	حالت TYPE_LONGINT:
	    بازگشت typename_longint;

	حالت TYPE_LONGLONGINT:
	    بازگشت typename_longlongint;

	حالت TYPE_UNSIGNEDINT:
	    بازگشت typename_unsignedint;

	حالت TYPE_UNSIGNEDLONGINT:
	    بازگشت typename_unsignedlongint;

	حالت TYPE_UNSIGNEDLONGLONGINT:
	    بازگشت typename_unsignedlonglongint;

	حالت TYPE_POINTER:
	    بازگشت _(typename_pointer);

	حالت TYPE_PERCENT:
	    بازگشت _(typename_percent);

	حالت TYPE_CHAR:
	    بازگشت typename_char;

	حالت TYPE_STRING:
	    بازگشت _(typename_string);

	حالت TYPE_FLOAT:
	    بازگشت typename_float;
    }

    بازگشت _(typename_unknown);
}

    ایستا صحیح
adjust_types(
    ثابت نویسه ***ap_types,
    صحیح arg,
    صحیح *رقم_posarg,
    ثابت نویسه *type)
{
    اگر (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	بازگشت FAIL;
    }

    اگر (*ap_types == پوچ || *رقم_posarg < arg)
    {
	صحیح	    idx;
	ثابت نویسه  **new_types;

	اگر (*ap_types == پوچ)
	    new_types = ALLOC_CLEAR_MULT(ثابت نویسه *, arg);
	وگرنه
	    new_types = vim_realloc((نویسه **)*ap_types,
						arg * اندازه(ثابت نویسه *));

	اگر (new_types == پوچ)
	    بازگشت FAIL;

	برای (idx = *رقم_posarg; idx < arg; ++idx)
	    new_types[idx] = پوچ;

	*ap_types = new_types;
	*رقم_posarg = arg;
    }

    اگر ((*ap_types)[arg - 1] != پوچ)
    {
	اگر ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    ثابت نویسه *pt = type;
	    اگر (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    اگر (pt[0] != '*')
	    {
		انتخاب (pt[0])
		{
		    حالت 'd':
		    حالت 'i': بشکن;
		    پیشفرض:
			semsg(_(e_positional_رقم_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			بازگشت FAIL;
		}
	    }
	}
	وگرنه
	{
	    اگر (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_رقم_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		بازگشت FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    بازگشت OK;
}

    ایستا تهی
format_overflow_error(ثابت نویسه *pstart)
{
    size_t	arglen = 0;
    نویسه	*argcopy = پوچ;
    ثابت نویسه	*p = pstart;

    وقتی (VIM_ISDIGIT((صحیح)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(نویسه, arglen + 1);
    اگر (argcopy != پوچ)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	آزاد(argcopy);
    }
    وگرنه
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    ایستا صحیح
get_unsigned_int(
    ثابت نویسه *pstart,
    ثابت نویسه **p,
    unsigned صحیح *uj,
    صحیح overflow_err)
{
    *uj = **p - '0';
    ++*p;

    وقتی (VIM_ISDIGIT((صحیح)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned صحیح)(**p - '0');
	++*p;
    }

    اگر (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	اگر (overflow_err)
	{
	    format_overflow_error(pstart);
	    بازگشت FAIL;
	}
	وگرنه
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    بازگشت OK;
}


    ایستا صحیح
parse_fmt_types(
    ثابت نویسه  ***ap_types,
    صحیح		*رقم_posarg,
    ثابت نویسه  *fmt,
    typval_T	*tvs UNUSED
    )
{
    ثابت نویسه	*p = fmt;
    ثابت نویسه	*arg = پوچ;

    صحیح		any_pos = 0;
    صحیح		any_arg = 0;
    صحیح		arg_idx;

# define CHECK_POS_ARG انجامبده { \
    اگر (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	برو error; \
    } \
} وقتی (0);

    اگر (p == پوچ)
	بازگشت OK;

    وقتی (*p != NUL)
    {
	اگر (*p != '%')
	{
	    ثابت نویسه    *q = strchr(p + 1, '%');
	    size_t  n = (q == پوچ) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	وگرنه
	{
	    // allowed values: \0, h, l, L
	    نویسه	length_modifier = '\0';

	    // variable for positional arg
	    صحیح		pos_arg = -1;
	    ثابت نویسه	*ptype = پوچ;
	    ثابت نویسه	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    وقتی (VIM_ISDIGIT(*ptype))
		++ptype;

	    اگر (*ptype == '$')
	    {
		اگر (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    برو error;
		}

		// Positional argument
		unsigned صحیح uj;

		اگر (get_unsigned_int(pstart, &p, &uj, tvs != پوچ) == FAIL)
		    برو error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    وقتی (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		انتخاب (*p)
		{
		    حالت '0': بشکن;
		    حالت '-': بشکن;
		    حالت '+': بشکن;
		    حالت ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      بشکن;
		    حالت '#': بشکن;
		    حالت '\'': بشکن;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    اگر (*(arg = p) == '*')
	    {
		p++;

		اگر (VIM_ISDIGIT((صحیح)(*p)))
		{
		    // Positional argument field width
		    unsigned صحیح uj;

		    اگر (get_unsigned_int(arg + 1, &p, &uj, tvs != پوچ) == FAIL)
			برو error;

		    اگر (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			برو error;
		    }
		    وگرنه
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			اگر (adjust_types(ap_types, uj, رقم_posarg, arg) == FAIL)
			    برو error;
		    }
		}
		وگرنه
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    وگرنه اگر (VIM_ISDIGIT((صحیح)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ثابت نویسه *digstart = p;
		unsigned صحیح uj;

		اگر (get_unsigned_int(digstart, &p, &uj, tvs != پوچ) == FAIL)
		    برو error;

		اگر (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    برو error;
		}
	    }

	    // parse precision
	    اگر (*p == '.')
	    {
		p++;

		اگر (*(arg = p) == '*')
		{
		    p++;

		    اگر (VIM_ISDIGIT((صحیح)(*p)))
		    {
			// Parse precision
			unsigned صحیح uj;

			اگر (get_unsigned_int(arg + 1, &p, &uj, tvs != پوچ) == FAIL)
			    برو error;

			اگر (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    اگر (adjust_types(ap_types, uj, رقم_posarg, arg) == FAIL)
				برو error;
			}
			وگرنه
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    برو error;
			}
		    }
		    وگرنه
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		وگرنه اگر (VIM_ISDIGIT((صحیح)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ثابت نویسه *digstart = p;
		    unsigned صحیح uj;

		    اگر (get_unsigned_int(digstart, &p, &uj, tvs != پوچ) == FAIL)
			برو error;

		    اگر (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			برو error;
		    }
		}
	    }

	    اگر (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    اگر (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		اگر (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    انتخاب (*p)
	    {
		// Check for known format specifiers. % is special!
		حالت 'i':
		حالت '*':
		حالت 'd':
		حالت 'u':
		حالت 'o':
		حالت 'D':
		حالت 'U':
		حالت 'O':
		حالت 'x':
		حالت 'X':
		حالت 'b':
		حالت 'B':
		حالت 'c':
		حالت 's':
		حالت 'S':
		حالت 'p':
		حالت 'f':
		حالت 'F':
		حالت 'e':
		حالت 'E':
		حالت 'g':
		حالت 'G':
		    اگر (pos_arg != -1)
		    {
			اگر (adjust_types(ap_types, pos_arg, رقم_posarg, ptype) == FAIL)
			    برو error;
		    }
		    وگرنه
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    بشکن;

		پیشفرض:
		    اگر (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			برو error;
		    }
	    }

	    اگر (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    برای (arg_idx = 0; arg_idx < *رقم_posarg; ++arg_idx)
    {
	اگر ((*ap_types)[arg_idx] == پوچ)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    برو error;
	}

# اگر defined(FEAT_EVAL)
	اگر (tvs != پوچ && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    برو error;
	}
# endif
    }

    بازگشت OK;

error:
    vim_free((نویسه**)*ap_types);
    *ap_types = پوچ;
    *رقم_posarg = 0;
    بازگشت FAIL;
}

    ایستا تهی
skip_to_arg(
    ثابت نویسه	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    صحیح		*arg_idx,
    صحیح		*arg_cur,
    ثابت نویسه	*fmt)
{
    صحیح		arg_min = 0;

    اگر (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	بازگشت;
    }

    اگر (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    وگرنه
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    برای (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	ثابت نویسه *p;

	اگر (ap_types == پوچ || ap_types[*arg_cur] == پوچ)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    بازگشت;
	}

	p = ap_types[*arg_cur];

	صحیح fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	انتخاب (fmt_type)
	{
	حالت TYPE_PERCENT:
	حالت TYPE_UNKNOWN:
	    بشکن;

	حالت TYPE_CHAR:
	    va_arg(*ap, صحیح);
	    بشکن;

	حالت TYPE_STRING:
	    va_arg(*ap, نویسه *);
	    بشکن;

	حالت TYPE_POINTER:
	    va_arg(*ap, تهی *);
	    بشکن;

	حالت TYPE_INT:
	    va_arg(*ap, صحیح);
	    بشکن;

	حالت TYPE_LONGINT:
	    va_arg(*ap, long صحیح);
	    بشکن;

	حالت TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    بشکن;

	حالت TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned صحیح);
	    بشکن;

	حالت TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long صحیح);
	    بشکن;

	حالت TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    بشکن;

	حالت TYPE_FLOAT:
	    va_arg(*ap, دوگانه);
	    بشکن;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    بازگشت;
}

    صحیح
vim_vsnprintf_typval(
    نویسه	*str,
    size_t	str_m,
    ثابت نویسه	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    ثابت نویسه	*p = fmt;
    صحیح		arg_cur = 0;
    صحیح		رقم_posarg = 0;
    صحیح		arg_idx = 1;
    va_list	ap;
    ثابت نویسه	**ap_types = پوچ;

    اگر (parse_fmt_types(&ap_types, &رقم_posarg, fmt, tvs) == FAIL)
	بازگشت 0;

    va_copy(ap, ap_start);

    اگر (p == پوچ)
	p = "";
    وقتی (*p != NUL)
    {
	اگر (*p != '%')
	{
	    ثابت نویسه    *q = strchr(p + 1, '%');
	    size_t  n = (q == پوچ) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    اگر (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	وگرنه
	{
	    size_t  min_field_width = 0, precision = 0;
	    صحیح	    zero_padding = 0, precision_specified = 0, justify_چپ = 0;
	    صحیح	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    صحیح	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    نویسه    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    نویسه    tmp[TMP_LEN];

	    // string address in case of string argument
	    ثابت نویسه  *str_arg = پوچ;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    unsigned نویسه uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  عدد_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    نویسه    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = پوچ;

	    // variables for positional arg
	    صحیح	    pos_arg = -1;
	    ثابت نویسه	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    وقتی (VIM_ISDIGIT(*ptype))
		++ptype;

	    اگر (*ptype == '$')
	    {
		// Positional argument
		ثابت نویسه *digstart = p;
		unsigned صحیح uj;

		اگر (get_unsigned_int(digstart, &p, &uj, tvs != پوچ) == FAIL)
		    برو error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    وقتی (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		انتخاب (*p)
		{
		    حالت '0': zero_padding = 1; بشکن;
		    حالت '-': justify_چپ = 1; بشکن;
		    حالت '+': force_sign = 1; space_for_positive = 0; بشکن;
		    حالت ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      بشکن;
		    حالت '#': alternate_form = 1; بشکن;
		    حالت '\'': بشکن;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    اگر (*p == '*')
	    {
		صحیح j;
		ثابت نویسه *digstart = p + 1;

		p++;

		اگر (VIM_ISDIGIT((صحیح)(*p)))
		{
		    // Positional argument field width
		    unsigned صحیح uj;

		    اگر (get_unsigned_int(digstart, &p, &uj, tvs != پوچ) == FAIL)
			برو error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# اگر defined(FEAT_EVAL)
		    tvs != پوچ ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, صحیح));

		اگر (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    اگر (tvs != پوچ)
		    {
			format_overflow_error(digstart);
			برو error;
		    }
		    وگرنه
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		اگر (j >= 0)
		    min_field_width = j;
		وگرنه
		{
		    min_field_width = -j;
		    justify_چپ = 1;
		}
	    }
	    وگرنه اگر (VIM_ISDIGIT((صحیح)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ثابت نویسه *digstart = p;
		unsigned صحیح uj;

		اگر (get_unsigned_int(digstart, &p, &uj, tvs != پوچ) == FAIL)
		    برو error;

		min_field_width = uj;
	    }

	    // parse precision
	    اگر (*p == '.')
	    {
		p++;
		precision_specified = 1;

		اگر (VIM_ISDIGIT((صحیح)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ثابت نویسه *digstart = p;
		    unsigned صحیح uj;

		    اگر (get_unsigned_int(digstart, &p, &uj, tvs != پوچ) == FAIL)
			برو error;

		    precision = uj;
		}
		وگرنه اگر (*p == '*')
		{
		    صحیح j;
		    ثابت نویسه *digstart = p;

		    p++;

		    اگر (VIM_ISDIGIT((صحیح)(*p)))
		    {
			// positional argument
			unsigned صحیح uj;

			اگر (get_unsigned_int(digstart, &p, &uj, tvs != پوچ) == FAIL)
			    برو error;

			arg_idx = uj;

			++p;
		    }

		    j =
# اگر defined(FEAT_EVAL)
			tvs != پوچ ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, صحیح));

		    اگر (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			اگر (tvs != پوچ)
			{
			    format_overflow_error(digstart);
			    برو error;
			}
			وگرنه
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    اگر (j >= 0)
			precision = j;
		    وگرنه
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    اگر (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		اگر (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    length_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    انتخاب (fmt_spec)
	    {
		حالت 'i': fmt_spec = 'd'; بشکن;
		حالت 'D': fmt_spec = 'd'; length_modifier = 'l'; بشکن;
		حالت 'U': fmt_spec = 'u'; length_modifier = 'l'; بشکن;
		حالت 'O': fmt_spec = 'o'; length_modifier = 'l'; بشکن;
		پیشفرض: بشکن;
	    }

# اگر defined(FEAT_EVAL)
	    انتخاب (fmt_spec)
	    {
		حالت 'd': حالت 'u': حالت 'o': حالت 'x': حالت 'X':
		    اگر (tvs != پوچ && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    اگر (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    انتخاب (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    حالت '%':
	    حالت 'c':
	    حالت 's':
	    حالت 'S':
		str_arg_l = 1;
		انتخاب (fmt_spec)
		{
		حالت '%':
		    str_arg = p;
		    بشکن;

		حالت 'c':
		    {
			صحیح j;

			j =
# اگر defined(FEAT_EVAL)
			    tvs != پوچ ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, صحیح));

			// standard demands unsigned char
			uchar_arg = (unsigned نویسه)j;
			str_arg = (نویسه *)&uchar_arg;
			بشکن;
		    }

		حالت 's':
		حالت 'S':
		    str_arg =
# اگر defined(FEAT_EVAL)
				tvs != پوچ ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, نویسه *));

		    اگر (str_arg == پوچ)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    وگرنه اگر (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    وگرنه اگر (precision == 0)
			str_arg_l = 0;
		    وگرنه
		    {
			// memchr on HP does not like n > 2^31  !!!
			ثابت نویسه *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == پوچ) ? precision
						      : (size_t)(q - str_arg);
		    }
		    اگر (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			صحیح	cell;

			برای (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    اگر (precision_specified && i + cell > precision)
				بشکن;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			اگر (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    بشکن;

		پیشفرض:
		    بشکن;
		}
		بشکن;

	    حالت 'd': حالت 'u':
	    حالت 'b': حالت 'B':
	    حالت 'o':
	    حالت 'x': حالت 'X':
	    حالت 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //مقدار

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    صحیح arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    صحیح int_arg = 0;
		    unsigned صحیح uint_arg = 0;

		    // only set for length modifier l
		    long صحیح long_arg = 0;
		    unsigned long صحیح ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    تهی *ptr_arg = پوچ;

		    اگر (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# اگر defined(FEAT_EVAL)
				 tvs != پوچ ? (تهی *)tv_str(tvs, &arg_idx,
									پوچ) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, تهی *));

			اگر (ptr_arg != پوچ)
			    arg_sign = 1;
		    }
		    وگرنه اگر (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# اگر defined(FEAT_EVAL)
				    tvs != پوچ ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			اگر (bin_arg != 0)
			    arg_sign = 1;
		    }
		    وگرنه اگر (fmt_spec == 'd')
		    {
			// signed
			انتخاب (length_modifier)
			{
			حالت '\0':
			حالت 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# اگر defined(FEAT_EVAL)
					tvs != پوچ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, صحیح));

			    اگر (int_arg > 0)
				arg_sign =  1;
			    وگرنه اگر (int_arg < 0)
				arg_sign = -1;
			    بشکن;
			حالت 'l':
			    long_arg =
# اگر defined(FEAT_EVAL)
					tvs != پوچ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long صحیح));

			    اگر (long_arg > 0)
				arg_sign =  1;
			    وگرنه اگر (long_arg < 0)
				arg_sign = -1;
			    بشکن;
			حالت 'L':
			    llong_arg =
# اگر defined(FEAT_EVAL)
					tvs != پوچ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    اگر (llong_arg > 0)
				arg_sign =  1;
			    وگرنه اگر (llong_arg < 0)
				arg_sign = -1;
			    بشکن;
			}
		    }
		    وگرنه
		    {
			// unsigned
			انتخاب (length_modifier)
			{
			    حالت '\0':
			    حالت 'h':
				uint_arg =
# اگر defined(FEAT_EVAL)
					    tvs != پوچ ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned صحیح));

				اگر (uint_arg != 0)
				    arg_sign = 1;
				بشکن;
			    حالت 'l':
				ulong_arg =
# اگر defined(FEAT_EVAL)
					    tvs != پوچ ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long صحیح));

				اگر (ulong_arg != 0)
				    arg_sign = 1;
				بشکن;
			    حالت 'L':
				ullong_arg =
# اگر defined(FEAT_EVAL)
					    tvs != پوچ ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				اگر (ullong_arg != 0)
				    arg_sign = 1;
				بشکن;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    اگر (precision_specified)
			zero_padding = 0;
		    اگر (fmt_spec == 'd')
		    {
			اگر (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    وگرنه اگر (alternate_form)
		    {
			اگر (arg_sign != 0
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
		    اگر (!precision_specified)
			precision = 1;   // default precision is 1
		    اگر (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    وگرنه
		    {
			نویسه	f[6];
			صحیح	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			اگر (!length_modifier)
			    ;
			وگرنه اگر (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# وگرنه
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			وگرنه
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			اگر (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			وگرنه اگر (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    نویسه	    b[8 * اندازه(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    انجامبده
			    {
				b[اندازه(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    وقتی (bn != 0);

			    memcpy(tmp + str_arg_l, b + اندازه(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			وگرنه اگر (fmt_spec == 'd')
			{
			    // signed
			    انتخاب (length_modifier)
			    {
			    حالت '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       بشکن;
			    حالت 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (short)int_arg);
				      بشکن;
			    حالت 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      بشکن;
			    حالت 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      بشکن;
			    }
			}
			وگرنه
			{
			    // unsigned
			    انتخاب (length_modifier)
			    {
			    حالت '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       بشکن;
			    حالت 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(unsigned short)uint_arg);
				      بشکن;
			    حالت 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      بشکن;
			    حالت 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      بشکن;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			اگر (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			اگر (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t رقم_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			اگر (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    اگر (!precision_specified
					     || precision < رقم_of_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = رقم_of_digits + 1;
			    }
			}
			// zero padding to specified precision?
			اگر (رقم_of_digits < precision)
			    عدد_of_zeros_to_pad = precision - رقم_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    اگر (!justify_چپ && zero_padding)
		    {
			صحیح n = (صحیح)(min_field_width - (str_arg_l
						    + عدد_of_zeros_to_pad));
			اگر (n > 0)
			    عدد_of_zeros_to_pad += n;
		    }
		    بشکن;
		}

	    حالت 'f':
	    حالت 'F':
	    حالت 'e':
	    حالت 'E':
	    حالت 'g':
	    حالت 'G':
		{
		    // Floating point.
		    دوگانه	f;
		    دوگانه	abs_f;
		    نویسه	format[40];
		    صحیح		l;
		    صحیح		remove_trailing_zeroes = FALSE;

		    f =
# اگر defined(FEAT_EVAL)
			tvs != پوچ ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, دوگانه));

		    abs_f = f < 0 ? -f : f;

		    اگر (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			اگر ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			وگرنه
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    اگر ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# وگرنه
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
		    وگرنه
		    {
			اگر (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			وگرنه اگر (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			وگرنه
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    اگر (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    اگر (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				اگر ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				اگر (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (صحیح)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			اگر (remove_trailing_zeroes)
			{
			    صحیح i;
			    نویسه *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    اگر (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    وگرنه
			    {
				tp = (نویسه *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				اگر (tp != پوچ)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    اگر (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    وقتی (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    اگر (tp != پوچ && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				وقتی (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			وگرنه
			{
			    نویسه *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (نویسه *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    اگر (tp != پوچ && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    اگر (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			عدد_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    بشکن;
		}

	    پیشفرض:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_چپ = 1;
		min_field_width = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		اگر (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		بشکن;
	    }

	    اگر (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    اگر (!justify_چپ)
	    {
		// left padding with blank or zero
		صحیح pn = (صحیح)(min_field_width - (str_arg_l + عدد_of_zeros_to_pad));

		اگر (pn > 0)
		{
		    اگر (str_l < str_m)
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
	    اگر (عدد_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    وگرنه
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		صحیح zn = (صحیح)zero_padding_insertion_ind;

		اگر (zn > 0)
		{
		    اگر (str_l < str_m)
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
		zn = (صحیح)عدد_of_zeros_to_pad;
		اگر (zn > 0)
		{
		    اگر (str_l < str_m)
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
		صحیح sn = (صحیح)(str_arg_l - zero_padding_insertion_ind);

		اگر (sn > 0)
		{
		    اگر (str_l < str_m)
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
	    اگر (justify_چپ)
	    {
		// right blank padding to the field width
		صحیح pn = (صحیح)(min_field_width
				      - (str_arg_l + عدد_of_zeros_to_pad));

		اگر (pn > 0)
		{
		    اگر (str_l < str_m)
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

    اگر (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    اگر (tvs != پوچ && tvs[رقم_posarg != 0 ? رقم_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((نویسه*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    بازگشت (صحیح)str_l;
}

#endif // PROTO
