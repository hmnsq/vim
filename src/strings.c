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
#அந்தர்பாவ "vim.h"

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
    எனில் (p != வெற்று)
	mch_memmove(p, string, len);
    திரும்பு p;
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
    எனில் (p == வெற்று)
	திரும்பு வெற்று;

    STRNCPY(p, string, len);
    p[len] = NUL;
    திரும்பு p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    திரும்பு vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    முழு		cc,
    முழு		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    முழு		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    க்கு (p = string; *p; p++)
    {
	எனில் (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    தொடர்;
	}
	எனில் (vim_strchr(esc_chars, *p) != வெற்று || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    எனில் (escaped_string == வெற்று)
	திரும்பு வெற்று;
    p2 = escaped_string;
    க்கு (p = string; *p; p++)
    {
	எனில் (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    தொடர்;
	}
	எனில் (vim_strchr(esc_chars, *p) != வெற்று || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    திரும்பு escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    முழு
csh_like_shell(வெறுமை)
{
    திரும்பு (strstr((எழுத்து *)gettail(p_sh), "csh") != வெற்று);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    நிலையான முழு
fish_like_shell(வெறுமை)
{
    திரும்பு (strstr((எழுத்து *)gettail(p_sh), "fish") != வெற்று);
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
vim_strsave_shellescape(char_u *string, முழு do_special, முழு do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    முழு		csh_like;
    முழு		fish_like;
    char_u	*shname;
    முழு		powershell;
#ifdef MSWIN
    முழு		double_quotes;
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
    powershell = strstr((எழுத்து *)shname, "pwsh") != வெற்று;
#ifdef MSWIN
    powershell = powershell || strstr((எழுத்து *)shname, "powershell") != வெற்று;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (unsigned)STRLEN(string) + 3;  // two quotes and a trailing NUL
    க்கு (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	எனில் (double_quotes)
	{
	    எனில் (*p == '"')
		++length;		// " -> ""
	}
	இல்லையெனில்
#endif
	எனில் (*p == '\'')
	{
	    எனில் (powershell)
		length += 2;		// ' => ''
	    இல்லையெனில்
		length += 3;		// ' => '\''
	}
	எனில் ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    எனில் (csh_like && do_special)
		++length;		// insert backslash
	}
	எனில் (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	எனில் (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    எனில் (escaped_string != வெற்று)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	எனில் (double_quotes)
	    *d++ = '"';
	இல்லையெனில்
#endif
	    *d++ = '\'';

	க்கு (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    எனில் (double_quotes)
	    {
		எனில் (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    தொடர்;
		}
	    }
	    இல்லையெனில்
#endif
	    எனில் (*p == '\'')
	    {
		எனில் (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		இல்லையெனில்
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		தொடர்;
	    }
	    எனில் ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		எனில் (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		தொடர்;
	    }
	    எனில் (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		தொடர்;
	    }
	    எனில் (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		தொடர்;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	எனில் (double_quotes)
	    *d++ = '"';
	இல்லையெனில்
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    திரும்பு escaped_string;
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
    திரும்பு p1;
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
    திரும்பு p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    வெறுமை
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    முழு	    c;

    எனில் (p == வெற்று)
	திரும்பு;

    p2 = p;
    போது ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#எனில் defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    நிலையான char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    எனில் (res != வெற்று)
	போது (*p != NUL)
	{
	    முழு		l;

	    எனில் (enc_utf8)
	    {
		முழு	c, uc;
		முழு	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		எனில் (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		எனில் (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    எனில் (s == வெற்று)
		    {
			vim_free(res);
			திரும்பு வெற்று;
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
	    இல்லையெனில் எனில் (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    இல்லையெனில்
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    திரும்பு res;
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

    எனில் (res != வெற்று)
	போது (*p != NUL)
	{
	    முழு		l;

	    எனில் (enc_utf8)
	    {
		முழு	c, lc;
		முழு	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		எனில் (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		எனில் (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    எனில் (s == வெற்று)
		    {
			vim_free(res);
			திரும்பு வெற்று;
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
	    இல்லையெனில் எனில் (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    இல்லையெனில்
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    திரும்பு res;
}
#endif

/*
 * delete spaces at the end of a string
 */
    வெறுமை
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    போது (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    வெறுமை
vim_strncpy(char_u *to, char_u *from, size_t len)
{
    STRNCPY(to, from, len);
    to[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    வெறுமை
vim_strcat(char_u *to, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(to);
    size_t fromlen = STRLEN(from);

    எனில் (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    இல்லையெனில்
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(எழுத்து *s, size_t maxlen)
{
    size_t i;
    க்கு (i = 0; i < maxlen; ++i)
	எனில் (s[i] == NUL)
	    நிறுத்து;
    திரும்பு i;
}

#எனில் !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    முழு
vim_stricmp(எழுத்து *s1, எழுத்து *s2)
{
    முழு		i;

    க்கு (;;)
    {
	i = (முழு)TOLOWER_LOC(*s1) - (முழு)TOLOWER_LOC(*s2);
	எனில் (i != 0)
	    திரும்பு i;			    // this character different
	எனில் (*s1 == NUL)
	    நிறுத்து;			    // strings match until NUL
	++s1;
	++s2;
    }
    திரும்பு 0;				    // strings match
}
#endif

#எனில் !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    முழு
vim_strnicmp(எழுத்து *s1, எழுத்து *s2, size_t len)
{
    முழு		i;

    போது (len > 0)
    {
	i = (முழு)TOLOWER_LOC(*s1) - (முழு)TOLOWER_LOC(*s2);
	எனில் (i != 0)
	    திரும்பு i;			    // this character different
	எனில் (*s1 == NUL)
	    நிறுத்து;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    திரும்பு 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    முழு
vim_strnicmp_asc(எழுத்து *s1, எழுத்து *s2, size_t len)
{
    முழு                i = 0;

    போது (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	எனில் (i != 0)
	    நிறுத்து;			// this character is different
	எனில் (*s1 == NUL)
	    நிறுத்து;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    திரும்பு i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, முழு c)
{
    char_u	*p;
    முழு		b;

    p = string;
    எனில் (enc_utf8 && c > 0 && c < 0x80)
	திரும்பு vim_strbyte(string, c);
    எனில் (enc_utf8 && c >= 0x80)
    {
	போது (*p != NUL)
	{
	    முழு l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    எனில் (utf_ptr2char(p) == c && l > 1)
		திரும்பு p;
	    p += l;
	}
	திரும்பு வெற்று;
    }
    எனில் (enc_dbcs != 0 && c > 255)
    {
	முழு	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	போது ((b = *p) != NUL)
	{
	    எனில் (b == c && p[1] == n2)
		திரும்பு p;
	    p += (*mb_ptr2len)(p);
	}
	திரும்பு வெற்று;
    }
    எனில் (has_mbyte)
    {
	போது ((b = *p) != NUL)
	{
	    எனில் (b == c)
		திரும்பு p;
	    p += (*mb_ptr2len)(p);
	}
	திரும்பு வெற்று;
    }
    போது ((b = *p) != NUL)
    {
	எனில் (b == c)
	    திரும்பு p;
	++p;
    }
    திரும்பு வெற்று;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, முழு c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    எனில் (c <= 0 || c > 255)
	திரும்பு வெற்று;
    திரும்பு (char_u *)strchr((எழுத்து *)string, c);
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
vim_strrchr(char_u *string, முழு c)
{
    char_u	*retval = வெற்று;
    char_u	*p = string;

    போது (*p)
    {
	எனில் (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    திரும்பு retval;
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
    போது (*s)
    {
	எனில் (vim_strchr(charset, *s) != வெற்று)
	    திரும்பு s;
	MB_PTR_ADV(s);
    }
    திரும்பு வெற்று;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
நிலையான முழு sort_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2);

    நிலையான முழு
sort_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    திரும்பு STRCMP(*(எழுத்து **)s1, *(எழுத்து **)s2);
}

    வெறுமை
sort_strings(
    char_u	**files,
    முழு		மொத்தம்)
{
    qsort((வெறுமை *)files, (size_t)மொத்தம், அளவு(char_u *), sort_compare);
}

#எனில் defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    முழு
has_non_ascii(char_u *s)
{
    char_u	*p;

    எனில் (s != வெற்று)
	க்கு (p = s; *p != NUL; ++p)
	    எனில் (*p >= 128)
		திரும்பு TRUE;
    திரும்பு FALSE;
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
    size_t  l = str1 == வெற்று ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == வெற்று ? 0 : STRLEN(str2)) + 1L);
    எனில் (dest == வெற்று)
	திரும்பு வெற்று;
    எனில் (str1 == வெற்று)
	*dest = NUL;
    இல்லையெனில்
	STRCPY(dest, str1);
    எனில் (str2 != வெற்று)
	STRCPY(dest + l, str2);
    திரும்பு dest;
}

#எனில் defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    எனில் (rev == வெற்று)
	திரும்பு வெற்று;

    க்கு (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	எனில் (has_mbyte)
	{
	    முழு mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	இல்லையெனில்
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    திரும்பு rev;
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, முழு function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    எனில் (str != வெற்று)
    {
	len += (unsigned)STRLEN(str);
	க்கு (p = str; *p != NUL; MB_PTR_ADV(p))
	    எனில் (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    எனில் (r == வெற்று)
	திரும்பு வெற்று;

    எனில் (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    இல்லையெனில்
	*r++ = '\'';
    எனில் (str != வெற்று)
	க்கு (p = str; *p != NUL; )
	{
	    எனில் (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    எனில் (function)
	*r++ = ')';
    *r++ = NUL;
    திரும்பு s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_மொத்தம்(char_u *haystack, char_u *needle, முழு ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    எனில் (p == வெற்று || needle == வெற்று || *needle == NUL)
	திரும்பு 0;

    size_t  needlelen = STRLEN(needle);
    எனில் (ic)
    {
	போது (*p != NUL)
	{
	    எனில் (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    இல்லையெனில்
		MB_PTR_ADV(p);
	}
    }
    இல்லையெனில்
	போது ((next = (char_u *)strstr((எழுத்து *)p, (எழுத்து *)needle)) != வெற்று)
	{
	    ++n;
	    p = next + needlelen;
	}

    திரும்பு n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    நிலையான முழு
copy_first_char_to_tv(char_u *உள்ளீடு, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    முழு		len;

    எனில் (உள்ளீடு == வெற்று || output == வெற்று)
	திரும்பு -1;

    len = has_mbyte ? mb_ptr2len(உள்ளீடு) : 1;
    STRNCPY(buf, உள்ளீடு, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    திரும்பு output->vval.v_string == வெற்று ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    வெறுமை
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*கோவை,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    முழு		len = 0;
    முழு		idx = 0;
    முழு		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_சாவி, VAR_எண்);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_கோவை_get_funccal(கோவை, &newtv);

    ga_init2(&ga, அளவு(எழுத்து), 80);
    க்கு (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	எனில் (len < 0)
	    நிறுத்து;

	set_vim_var_nr(VV_சாவி, idx);
	எனில் (filter_map_one(&tv, கோவை, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    நிறுத்து;
	}
	எனில் (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    எனில் (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		நிறுத்து;
	    }
	    இல்லையெனில்
		ga_concat(&ga, newtv.vval.v_string);
	}
	இல்லையெனில் எனில் (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    எனில் (fc != வெற்று)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    வெறுமை
string_reduce(
	typval_T	*argvars,
	typval_T	*கோவை,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    முழு		len;
    typval_T	argv[3];
    முழு		r;
    முழு		called_emsg_start = called_emsg;
    funccall_T	*fc;

    எனில் (argvars[2].v_type == VAR_UNKNOWN)
    {
	எனில் (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_மதிப்பு), "String");
	    திரும்பு;
	}
	len = copy_first_char_to_tv(p, rettv);
	எனில் (len < 0)
	    திரும்பு;
	p += len;
    }
    இல்லையெனில் எனில் (check_for_string_arg(argvars, 2) == FAIL)
	திரும்பு;
    இல்லையெனில்
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_கோவை_get_funccal(கோவை, rettv);

    க்கு ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	எனில் (len < 0)
	{
	    *rettv = argv[0];
	    நிறுத்து;
	}

	r = eval_கோவை_typval(கோவை, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	எனில் (r == FAIL || called_emsg != called_emsg_start)
	    நிறுத்து;
    }

    எனில் (fc != வெற்று)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    நிலையான வெறுமை
byteidx_common(typval_T *argvars, typval_T *rettv, முழு comp)
{
    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	திரும்பு;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_எண்_chk(&argvars[1], வெற்று);
    எனில் (str == வெற்று || idx < 0)
	திரும்பு;

    varnumber_T	utf16idx = FALSE;
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	முழு error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	எனில் (error)
	    திரும்பு;
	எனில் (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_எண்_as_bool_nr), utf16idx);
	    திரும்பு;
	}
    }

    முழு (*ptr2len)(char_u *);
    எனில் (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    இல்லையெனில்
	ptr2len = mb_ptr2len;

    char_u *t = str;
    க்கு ( ; idx > 0; idx--)
    {
	எனில் (*t == NUL)		// EOL reached
	    திரும்பு;
	எனில் (utf16idx)
	{
	    முழு clen = ptr2len(t);
	    முழு c = (clen > 1) ? utf_ptr2char(t) : *t;
	    எனில் (c > 0xFFFF)
		idx--;
	    எனில் (idx > 0)
		t += clen;
	}
	இல்லையெனில் எனில் (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_எண் = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    வெறுமை
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    வெறுமை
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    வெறுமை
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = -1;

    எனில் (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	திரும்பு;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_எண்_chk(&argvars[1], வெற்று);
    எனில் (str == வெற்று || idx < 0)
	திரும்பு;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	எனில் (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    முழு (*ptr2len)(char_u *);
    எனில் (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    இல்லையெனில்
	ptr2len = mb_ptr2len;

    char_u	*p;
    முழு		len;
    க்கு (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	எனில் (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    எனில் (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_எண் = len;
	    திரும்பு;
	}
	எனில் (utf16idx)
	{
	    idx--;
	    முழு clen = ptr2len(p);
	    முழு c = (clen > 1) ? utf_ptr2char(p) : *p;
	    எனில் (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_எண் = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    நிலையான முழு
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    எனில் (convert_setup(&vimconv, from, to) == FAIL)
	திரும்பு FAIL;
    vimconv.vc_fail = TRUE;
    எனில் (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	எனில் (ret->string == வெற்று)
	    ret->length = 0;
	இல்லையெனில்
	    ret->length = str->length;
    }
    இல்லையெனில்
    {
	முழு len = (முழு)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, வெற்று, வெற்று);

    திரும்பு (ret->string == வெற்று) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    நிலையான வெறுமை
blob_from_string(char_u *str, blob_T *blob)
{
    முழு	    len = (முழு)STRLEN(str);
    char_u  *dest;

    எனில் (len == 0)
	திரும்பு;
    எனில் (ga_grow(&blob->bv_ga, len) == FAIL)
	திரும்பு;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    க்கு (முழு i = 0; i < len; ++i)
	எனில் (dest[i] == NL)
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
    நிலையான முழு
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	வரி_len;

    எனில் (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	திரும்பு (ret->string == வெற்று) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    வரி_len = (nl == வெற்று) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(வரி_len + 1);
    எனில் (ret->string == வெற்று)
    {
	ret->length = 0;
	திரும்பு FAIL;
    }
    எனில் (வரி_len > 0)
	mch_memmove(ret->string, src, (size_t)வரி_len);
    ret->string[வரி_len] = NUL;
    ret->length = (size_t)வரி_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    க்கு (long i = 0; i < வரி_len; i++)
	எனில் (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + வரி_len + (nl != வெற்று ? 1 : 0);
    திரும்பு OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    நிலையான char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    எனில் (from_encoding_raw == வெற்று)
	திரும்பு வெற்று;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    போது (*s != NUL)
    {
	எனில் (*s == '_')
	    *pe++ = '-';
	இல்லையெனில்
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    எனில் ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    திரும்பு from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    நிலையான வெறுமை
append_converted_string_to_list(
	string_T *converted,
	முழு validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    எனில் (converted->string != வெற்று)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	போது (p < end)
	{
	    string_T	வரி;
	    char_u	*வரி_start = p;

	    போது (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    வரி.length = (size_t)(p - வரி_start);
	    வரி.string = vim_strnsave(வரி_start, வரி.length);
	    எனில் (வரி.string != வெற்று)
	    {
		எனில் (validate_utf8 && !utf_valid_string(வரி.string, வெற்று))
		{
		    vim_free(வரி.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    திரும்பு; // Stop processing
		}
		எனில் (list_append_string(list, வரி.string, (முழு)வரி.length) == FAIL)
		{
		    vim_free(வரி.string);
		    vim_free(converted->string);
		    திரும்பு; // Stop processing on append failure
		}
		vim_free(வரி.string);
	    }
	    இல்லையெனில்
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		திரும்பு;
	    }

	    எனில் (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    இல்லையெனில்
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    நிலையான முழு
append_validated_வரி_to_list(string_T *வரி, முழு validate_utf8, list_T *list)
{
    எனில் (validate_utf8 && !utf_valid_string(வரி->string, வெற்று))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(வரி->string);
	திரும்பு FAIL;
    }

    முழு ret = list_append_string(list, வரி->string, (முழு)வரி->length);
    vim_free(வரி->string);
    திரும்பு ret;
}

    வெறுமை
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    முழு		blen;
    long	idx;
    முழு		validate_utf8 = FALSE;

    எனில் (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	திரும்பு;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    blob = argvars->vval.v_blob;
    எனில் (blob == வெற்று)
	திரும்பு;
    blen = blob_len(blob);

    char_u	*from_encoding = வெற்று;
    char_u	*from_encoding_raw = வெற்று;  // Encoding name with endianness preserved for iconv
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	எனில் (d != வெற்று)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    எனில் (enc != வெற்று)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		எனில் (from_encoding_raw == வெற்று)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    திரும்பு;
		}
	    }
	}
    }

    எனில் (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    எனில் (from_encoding != வெற்று && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    முழு from_prop = 0;
    எனில் (from_encoding != வெற்று)
	from_prop = enc_canon_props(from_encoding);
    எனில் (from_encoding != வெற்று && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	முழு nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	எனில் (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    எனில் (blen > 0)
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
	எனில் (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    செல் done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	முழு len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, வெற்று);
	converted.length = len;
	convert_setup(&vimconv, வெற்று, வெற்று);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    இல்லையெனில்
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	போது (idx < blen)
	{
	    string_T	str;

	    எனில் (string_from_blob(blob, &idx, &str) != OK)
		நிறுத்து;

	    எனில் (from_encoding != வெற்று)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		முழு	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		எனில் (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    செல் done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    எனில் (append_validated_வரி_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		செல் done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    எனில் (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

done:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    வெறுமை
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    எனில் (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	திரும்பு;

    எனில் (rettv_blob_alloc(rettv) == FAIL)
	திரும்பு;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    எனில் (list == வெற்று)
	திரும்பு;

    char_u	*to_encoding = வெற்று;
    char_u	*to_encoding_raw = வெற்று;  // Encoding name with endianness preserved for iconv
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	எனில் (d != வெற்று)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    எனில் (enc != வெற்று)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		எனில் (to_encoding_raw == வெற்று)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    திரும்பு;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    முழு to_prop = 0;
    எனில் (to_encoding != வெற்று)
	to_prop = enc_canon_props(to_encoding);
    எனில் (to_encoding != வெற்று && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_பொருட்கள்(list, li)
	{
	    char_u *s;

	    எனில் (li->li_tv.v_type != VAR_STRING)
		தொடர்;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    எனில் (li != list->lv_first)
		ga_append(&str_ga, NL);
	    எனில் (s != வெற்று && *s != NUL)
	    {
		முழு slen = (முழு)STRLEN(s);

		எனில் (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    செல் done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	எனில் (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    எனில் (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		செல் done;
	    }
	    vimconv.vc_fail = TRUE;

	    முழு		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, வெற்று);
	    convert_setup(&vimconv, வெற்று, வெற்று);
	    ga_clear(&str_ga);

	    எனில் (converted == வெற்று)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		செல் done;
	    }
	    எனில் (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	இல்லையெனில்
	    ga_clear(&str_ga);
    }
    இல்லையெனில்
    {
	FOR_ALL_LIST_பொருட்கள்(list, li)
	{
	    எனில் (li->li_tv.v_type != VAR_STRING)
		தொடர்;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    எனில் (str.string == வெற்று)
		STR_LITERAL_SET(str, "");
	    இல்லையெனில்
		str.length = STRLEN(str.string);

	    எனில் (to_encoding != வெற்று)
	    {
		முழு	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		எனில் (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    செல் done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    எனில் (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    எனில் (to_encoding != வெற்று)
		vim_free(str.string);
	}
    }

done:
    எனில் (to_encoding != வெற்று)
	vim_free(to_encoding);
    எனில் (to_encoding_raw != வெற்று)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    வெறுமை
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    முழு		utf8 = FALSE;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (முழு)tv_get_bool_chk(&argvars[1], வெற்று);

    p = tv_get_string(&argvars[0]);

    எனில் (has_mbyte || utf8)
    {
	முழு (*ptr2len)(char_u *);
	முழு (*ptr2char)(char_u *);

	எனில் (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	இல்லையெனில்
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	க்கு ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_எண்(rettv->vval.v_list, (*ptr2char)(p));
    }
    இல்லையெனில்
	க்கு ( ; *p != NUL; ++p)
	    list_append_எண்(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    வெறுமை
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    முழு		base = 10;
    char_u	*p;
    varnumber_T	n;
    முழு		what = 0;
    முழு		isneg;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (முழு)tv_get_எண்(&argvars[1]);
	எனில் (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    திரும்பு;
	}
	எனில் (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    எனில் (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    தேர்வு (base)
    {
	நிலை 2: what |= STR2NR_BIN + STR2NR_FORCE; நிறுத்து;
	நிலை 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; நிறுத்து;
	நிலை 16: what |= STR2NR_HEX + STR2NR_FORCE; நிறுத்து;
    }
    vim_str2nr(p, வெற்று, வெற்று, what, &n, வெற்று, 0, FALSE, வெற்று);
    // Text after the number is silently ignored.
    எனில் (isneg)
	rettv->vval.v_எண் = -n;
    இல்லையெனில்
	rettv->vval.v_எண் = n;

}

/*
 * "strgetchar()" function
 */
    வெறுமை
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    முழு		len;
    முழு		error = FALSE;
    முழு		charidx;
    முழு		byteidx = 0;

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    str = tv_get_string_chk(&argvars[0]);
    எனில் (str == வெற்று)
	திரும்பு;
    len = (முழு)STRLEN(str);
    charidx = (முழு)tv_get_எண்_chk(&argvars[1], &error);
    எனில் (error)
	திரும்பு;

    போது (charidx >= 0 && byteidx < len)
    {
	எனில் (charidx == 0)
	{
	    rettv->vval.v_எண் = mb_ptr2char(str + byteidx);
	    நிறுத்து;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    வெறுமை
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    முழு		start_idx;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_எண்_arg(argvars, 2) == FAIL))
	திரும்பு;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_எண் = -1;
    எனில் (needle == வெற்று || haystack == வெற்று)
	திரும்பு;		// type error; errmsg already given

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	முழு	    error = FALSE;

	start_idx = (முழு)tv_get_எண்_chk(&argvars[2], &error);
	எனில் (error || start_idx >= (முழு)STRLEN(haystack))
	    திரும்பு;
	எனில் (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((எழுத்து *)haystack, (எழுத்து *)needle);
    எனில் (pos != வெற்று)
	rettv->vval.v_எண் = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    வெறுமை
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    எனில் (rettv->vval.v_string != வெற்று && tofree == வெற்று)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    வெறுமை
f_strlen(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && check_for_string_or_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    நிலையான வெறுமை
strchar_common(typval_T *argvars, typval_T *rettv, முழு skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    முழு			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    போது (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_எண் = len;
}

/*
 * "strcharlen()" function
 */
    வெறுமை
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && check_for_string_or_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    வெறுமை
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	முழு error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	எனில் (error)
	    திரும்பு;
	எனில் (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_எண்_as_bool_nr), skipcc);
	    திரும்பு;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    வெறுமை
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = -1;

    எனில் (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	திரும்பு;

    varnumber_T countcc = FALSE;
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    முழு			(*func_mb_ptr2char_adv)(char_u **pp);
    முழு			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    போது (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	எனில் (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_எண் = len;
}

/*
 * "strdisplaywidth()" function
 */
    வெறுமை
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    முழு		col = 0;

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    s = tv_get_string(&argvars[0]);
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	col = (முழு)tv_get_எண்(&argvars[1]);

    rettv->vval.v_எண் = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    வெறுமை
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_எண் = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    வெறுமை
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    முழு		nchar;
    முழு		nbyte = 0;
    முழு		charlen;
    முழு		skipcc = FALSE;
    முழு		len = 0;
    முழு		slen;
    முழு		error = FALSE;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_opt_எண்_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	திரும்பு;

    p = tv_get_string(&argvars[0]);
    slen = (முழு)STRLEN(p);

    nchar = (முழு)tv_get_எண்_chk(&argvars[1], &error);
    எனில் (!error)
    {
	எனில் (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    எனில் (error)
		திரும்பு;
	    எனில் (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_எண்_as_bool_nr), skipcc);
		திரும்பு;
	    }
	}

	எனில் (nchar > 0)
	    போது (nchar > 0 && nbyte < slen)
	    {
		எனில் (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		இல்லையெனில்
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	இல்லையெனில்
	    nbyte = nchar;
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (முழு)tv_get_எண்(&argvars[2]);
	    போது (charlen > 0 && nbyte + len < slen)
	    {
		முழு off = nbyte + len;

		எனில் (off < 0)
		    len += 1;
		இல்லையெனில்
		{
		    எனில் (skipcc)
			len += mb_ptr2len(p + off);
		    இல்லையெனில்
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	இல்லையெனில்
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    எனில் (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    இல்லையெனில் எனில் (nbyte > slen)
	nbyte = slen;
    எனில் (len < 0)
	len = 0;
    இல்லையெனில் எனில் (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    வெறுமை
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    முழு		n;
    முழு		len;
    முழு		slen;
    முழு		error = FALSE;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_opt_எண்_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	திரும்பு;

    p = tv_get_string(&argvars[0]);
    slen = (முழு)STRLEN(p);

    n = (முழு)tv_get_எண்_chk(&argvars[1], &error);
    எனில் (error)
	len = 0;
    இல்லையெனில் எனில் (argvars[2].v_type != VAR_UNKNOWN)
	len = (முழு)tv_get_எண்(&argvars[2]);
    இல்லையெனில்
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    எனில் (n < 0)
    {
	len += n;
	n = 0;
    }
    இல்லையெனில் எனில் (n > slen)
	n = slen;
    எனில் (len < 0)
	len = 0;
    இல்லையெனில் எனில் (n + len > slen)
	len = slen - n;

    எனில் (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	முழு off;

	// length in characters
	க்கு (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    வெறுமை
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = வெற்று;
    முழு		haystack_len, end_idx;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_எண்_arg(argvars, 2) == FAIL))
	திரும்பு;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_எண் = -1;
    எனில் (needle == வெற்று || haystack == வெற்று)
	திரும்பு;		// type error; errmsg already given

    haystack_len = (முழு)STRLEN(haystack);
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (முழு)tv_get_எண்_chk(&argvars[2], வெற்று);
	எனில் (end_idx < 0)
	    திரும்பு;	// can never find a match
    }
    இல்லையெனில்
	end_idx = haystack_len;

    எனில் (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    இல்லையெனில்
    {
	க்கு (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((எழுத்து *)rest, (எழுத்து *)needle);
	    எனில் (rest == வெற்று || rest > haystack + end_idx)
		நிறுத்து;
	    lastmatch = rest;
	}
    }

    எனில் (lastmatch == வெற்று)
	rettv->vval.v_எண் = -1;
    இல்லையெனில்
	rettv->vval.v_எண் = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    வெறுமை
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    வெறுமை
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = -1;

    எனில் (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_எண்_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    திரும்பு;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_எண்_chk(&argvars[1], வெற்று);
    எனில் (str == வெற்று || idx < 0)
	திரும்பு;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	எனில் (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    முழு (*ptr2len)(char_u *);
    எனில் (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    இல்லையெனில்
	ptr2len = mb_ptr2len;

    char_u	*p;
    முழு		len;
    முழு		utf16idx = 0;
    க்கு (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	எனில் (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    எனில் (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_எண் = len;
	    திரும்பு;
	}
	utf16idx = len;
	முழு clen = ptr2len(p);
	முழு c = (clen > 1) ? utf_ptr2char(p) : *p;
	எனில் (c > 0xFFFF)
	    len++;
	p += clen;
	எனில் (charidx)
	    idx--;
    }

    rettv->vval.v_எண் = utf16idx;
}

/*
 * "tolower(string)" function
 */
    வெறுமை
f_tolower(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    வெறுமை
f_toupper(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    வெறுமை
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*in_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    முழு		inlen;
    முழு		fromlen;
    முழு		tolen;
    முழு		idx;
    char_u	*cpstr;
    முழு		cplen;
    முழு		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	திரும்பு;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;
    எனில் (fromstr == வெற்று || tostr == வெற்று)
	    திரும்பு;		// type error; errmsg already given
    ga_init2(&ga, அளவு(எழுத்து), 80);

    எனில் (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	எனில் (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    திரும்பு;
	}

    // fromstr and tostr have to contain the same number of chars
    போது (*in_str != NUL)
    {
	எனில் (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    க்கு (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		எனில் (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    க்கு (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			எனில் (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    நிறுத்து;
			}
		    }
		    எனில் (*p == NUL)	// tostr is shorter than fromstr
			செல் error;
		    நிறுத்து;
		}
		++idx;
	    }

	    எனில் (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		க்கு (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		எனில் (idx != 0)
		    செல் error;
	    }

	    (வெறுமை)ga_grow(&ga, cplen);
	    mch_memmove((எழுத்து *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	இல்லையெனில்
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    எனில் (p != வெற்று)
		ga_append(&ga, tostr[p - fromstr]);
	    இல்லையெனில்
		ga_append(&ga, *in_str);
	    ++in_str;
	}
    }

    // add a terminating NUL
    (வெறுமை)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_data;
}

/*
 * "trim({expr})" function
 */
    வெறுமை
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*head;
    char_u	*mask = வெற்று;
    char_u	*tail;
    char_u	*prev;
    char_u	*p;
    முழு		c1;
    முழு		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 2) == FAIL)))
	திரும்பு;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    எனில் (head == வெற்று)
	திரும்பு;

    எனில் (check_for_opt_string_arg(argvars, 1) == FAIL)
	திரும்பு;

    எனில் (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	எனில் (*mask == NUL)
	    mask = வெற்று;

	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    முழு	error = 0;

	    // leading or trailing characters to trim
	    dir = (முழு)tv_get_எண்_chk(&argvars[2], &error);
	    எனில் (error)
		திரும்பு;
	    எனில் (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		திரும்பு;
	    }
	}
    }

    எனில் (dir == 0 || dir == 1)
    {
	// Trim leading characters
	போது (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    எனில் (mask == வெற்று)
	    {
		எனில் (c1 > ' ' && c1 != 0xa0)
		    நிறுத்து;
	    }
	    இல்லையெனில்
	    {
		க்கு (p = mask; *p != NUL; MB_PTR_ADV(p))
		    எனில் (c1 == PTR2CHAR(p))
			நிறுத்து;
		எனில் (*p == NUL)
		    நிறுத்து;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    எனில் (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	க்கு (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    எனில் (mask == வெற்று)
	    {
		எனில் (c1 > ' ' && c1 != 0xa0)
		    நிறுத்து;
	    }
	    இல்லையெனில்
	    {
		க்கு (p = mask; *p != NUL; MB_PTR_ADV(p))
		    எனில் (c1 == PTR2CHAR(p))
			நிறுத்து;
		எனில் (*p == NUL)
		    நிறுத்து;
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
    நிலையான char_u *
uri_decode(char_u *str)
{
    எனில் (str == வெற்று)
	திரும்பு வெற்று;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    எனில் (!decoded)
	திரும்பு வெற்று;

    char_u	*p = decoded;
    size_t	i = 0;

    போது (i < len)
    {
	எனில் (str[i] == '%')
	{
	    எனில் (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		எனில் (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    இல்லையெனில்
	    {
		முழு val = hexhex2nr(&str[i + 1]);
		எனில் (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		இல்லையெனில்
		{
		    // invalid hex digits following "%"
		    க்கு (முழு j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	இல்லையெனில்
	    *p++ = str[i++];
    }

    *p = NUL;

    திரும்பு decoded;
}

/*
 * "uri_decode({str})" function
 */
    வெறுமை
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

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
    நிலையான char_u *
uri_encode(char_u *str)
{
    எனில் (str == வெற்று)
	திரும்பு வெற்று;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    எனில் (encoded == வெற்று)
	திரும்பு வெற்று;

    char_u *p = encoded;

    க்கு (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	எனில் (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	இல்லையெனில்
	{
	    sprintf((எழுத்து *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    திரும்பு encoded;
}

/*
 * "uri_encode({str})" function
 */
    வெறுமை
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

நிலையான எழுத்து *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    நிலையான varnumber_T
tv_nr(typval_T *tvs, முழு *idxp)
{
    முழு		idx = *idxp - 1;
    varnumber_T	n = 0;
    முழு		err = FALSE;

    எனில் (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    இல்லையெனில்
    {
	++*idxp;
	n = tv_get_எண்_chk(&tvs[idx], &err);
	எனில் (err)
	    n = 0;
    }
    திரும்பு n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    நிலையான எழுத்து *
tv_str(typval_T *tvs, முழு *idxp, char_u **tofree)
{
    முழு		    idx = *idxp - 1;
    எழுத்து	    *s = வெற்று;
    நிலையான char_u   numbuf[NUMBUFLEN];

    எனில் (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    இல்லையெனில்
    {
	++*idxp;
	எனில் (tofree != வெற்று)
	    s = (எழுத்து *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	இல்லையெனில்
	    s = (எழுத்து *)tv_get_string_chk(&tvs[idx]);
    }
    திரும்பு s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    நிலையான இரட்டை
tv_float(typval_T *tvs, முழு *idxp)
{
    முழு		idx = *idxp - 1;
    இரட்டை	f = 0;

    எனில் (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    இல்லையெனில்
    {
	++*idxp;
	எனில் (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	இல்லையெனில் எனில் (tvs[idx].v_type == VAR_எண்)
	    f = (இரட்டை)tvs[idx].vval.v_எண்;
	இல்லையெனில்
	    emsg(_(e_expected_float_argument_for_printf));
    }
    திரும்பு f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    நிலையான மாறிலி எழுத்து *
infinity_str(முழு positive,
	     எழுத்து fmt_spec,
	     முழு force_sign,
	     முழு space_for_positive)
{
    நிலையான மாறிலி எழுத்து *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    முழு idx = positive * (1 + force_sign + force_sign * space_for_positive);

    எனில் (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    திரும்பு table[idx];
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
    முழு
vim_snprintf_add(எழுத்து *str, size_t str_m, மாறிலி எழுத்து *fmt, ...)
{
    va_list	ap;
    முழு		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    எனில் (str_m <= len)
	space = 0;
    இல்லையெனில்
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    திரும்பு str_l;
}

    முழு
vim_snprintf(எழுத்து *str, size_t str_m, மாறிலி எழுத்து *fmt, ...)
{
    va_list	ap;
    முழு		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    திரும்பு str_l;
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
vim_snprintf_safelen(எழுத்து *str, size_t str_m, மாறிலி எழுத்து *fmt, ...)
{
    va_list ap;
    முழு	    str_l;

    எனில் (str_m == 0)
	திரும்பு 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, வெற்று);
    va_end(ap);

    எனில் (str_l < 0)
    {
	*str = NUL;
	திரும்பு 0;
    }
    திரும்பு ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    முழு
vim_vsnprintf(
    எழுத்து	*str,
    size_t	str_m,
    மாறிலி எழுத்து	*fmt,
    va_list	ap)
{
    திரும்பு vim_vsnprintf_typval(str, str_m, fmt, ap, வெற்று);
}

எண்ணிக்கை
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
    நிலையான முழு
format_typeof(
    மாறிலி எழுத்து	*type)
{
    // allowed values: \0, h, l, L
    எழுத்து    length_modifier = '\0';

    // current conversion specifier character
    எழுத்து    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    எனில் (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	எனில் (length_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    length_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    தேர்வு (fmt_spec)
    {
	நிலை 'i': fmt_spec = 'd'; நிறுத்து;
	நிலை '*': fmt_spec = 'd'; length_modifier = 'h'; நிறுத்து;
	நிலை 'D': fmt_spec = 'd'; length_modifier = 'l'; நிறுத்து;
	நிலை 'U': fmt_spec = 'u'; length_modifier = 'l'; நிறுத்து;
	நிலை 'O': fmt_spec = 'o'; length_modifier = 'l'; நிறுத்து;
	இயல்பு: நிறுத்து;
    }

    // get parameter value, do initial processing
    தேர்வு (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    நிலை '%':
	திரும்பு TYPE_PERCENT;

    நிலை 'c':
	திரும்பு TYPE_CHAR;

    நிலை 's':
    நிலை 'S':
	திரும்பு TYPE_STRING;

    நிலை 'd': நிலை 'u':
    நிலை 'b': நிலை 'B':
    நிலை 'o':
    நிலை 'x': நிலை 'X':
    நிலை 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //மதிப்பு

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    எனில் (fmt_spec == 'p')
		திரும்பு TYPE_POINTER;
	    இல்லையெனில் எனில் (fmt_spec == 'b' || fmt_spec == 'B')
		திரும்பு TYPE_UNSIGNEDLONGLONGINT;
	    இல்லையெனில் எனில் (fmt_spec == 'd')
	    {
		// signed
		தேர்வு (length_modifier)
		{
		நிலை '\0':
		நிலை 'h':
		    // char and short arguments are passed as int.
		    திரும்பு TYPE_INT;
		நிலை 'l':
		    திரும்பு TYPE_LONGINT;
		நிலை 'L':
		    திரும்பு TYPE_LONGLONGINT;
		}
	    }
	    இல்லையெனில்
	    {
		// unsigned
		தேர்வு (length_modifier)
		{
		    நிலை '\0':
		    நிலை 'h':
			திரும்பு TYPE_UNSIGNEDINT;
		    நிலை 'l':
			திரும்பு TYPE_UNSIGNEDLONGINT;
		    நிலை 'L':
			திரும்பு TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	நிறுத்து;

    நிலை 'f':
    நிலை 'F':
    நிலை 'e':
    நிலை 'E':
    நிலை 'g':
    நிலை 'G':
	திரும்பு TYPE_FLOAT;
    }

    திரும்பு TYPE_UNKNOWN;
}

    நிலையான எழுத்து *
format_typename(
    மாறிலி எழுத்து  *type)
{
    தேர்வு (format_typeof(type))
    {
	நிலை TYPE_INT:
	    திரும்பு typename_int;

	நிலை TYPE_LONGINT:
	    திரும்பு typename_longint;

	நிலை TYPE_LONGLONGINT:
	    திரும்பு typename_longlongint;

	நிலை TYPE_UNSIGNEDINT:
	    திரும்பு typename_unsignedint;

	நிலை TYPE_UNSIGNEDLONGINT:
	    திரும்பு typename_unsignedlongint;

	நிலை TYPE_UNSIGNEDLONGLONGINT:
	    திரும்பு typename_unsignedlonglongint;

	நிலை TYPE_POINTER:
	    திரும்பு _(typename_pointer);

	நிலை TYPE_PERCENT:
	    திரும்பு _(typename_percent);

	நிலை TYPE_CHAR:
	    திரும்பு typename_char;

	நிலை TYPE_STRING:
	    திரும்பு _(typename_string);

	நிலை TYPE_FLOAT:
	    திரும்பு typename_float;
    }

    திரும்பு _(typename_unknown);
}

    நிலையான முழு
adjust_types(
    மாறிலி எழுத்து ***ap_types,
    முழு arg,
    முழு *இலக்கம்_posarg,
    மாறிலி எழுத்து *type)
{
    எனில் (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	திரும்பு FAIL;
    }

    எனில் (*ap_types == வெற்று || *இலக்கம்_posarg < arg)
    {
	முழு	    idx;
	மாறிலி எழுத்து  **new_types;

	எனில் (*ap_types == வெற்று)
	    new_types = ALLOC_CLEAR_MULT(மாறிலி எழுத்து *, arg);
	இல்லையெனில்
	    new_types = vim_realloc((எழுத்து **)*ap_types,
						arg * அளவு(மாறிலி எழுத்து *));

	எனில் (new_types == வெற்று)
	    திரும்பு FAIL;

	க்கு (idx = *இலக்கம்_posarg; idx < arg; ++idx)
	    new_types[idx] = வெற்று;

	*ap_types = new_types;
	*இலக்கம்_posarg = arg;
    }

    எனில் ((*ap_types)[arg - 1] != வெற்று)
    {
	எனில் ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    மாறிலி எழுத்து *pt = type;
	    எனில் (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    எனில் (pt[0] != '*')
	    {
		தேர்வு (pt[0])
		{
		    நிலை 'd':
		    நிலை 'i': நிறுத்து;
		    இயல்பு:
			semsg(_(e_positional_இலக்கம்_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			திரும்பு FAIL;
		}
	    }
	}
	இல்லையெனில்
	{
	    எனில் (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_இலக்கம்_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		திரும்பு FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    திரும்பு OK;
}

    நிலையான வெறுமை
format_overflow_error(மாறிலி எழுத்து *pstart)
{
    size_t	arglen = 0;
    எழுத்து	*argcopy = வெற்று;
    மாறிலி எழுத்து	*p = pstart;

    போது (VIM_ISDIGIT((முழு)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(எழுத்து, arglen + 1);
    எனில் (argcopy != வெற்று)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	விடுவி(argcopy);
    }
    இல்லையெனில்
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    நிலையான முழு
get_unsigned_int(
    மாறிலி எழுத்து *pstart,
    மாறிலி எழுத்து **p,
    unsigned முழு *uj,
    முழு overflow_err)
{
    *uj = **p - '0';
    ++*p;

    போது (VIM_ISDIGIT((முழு)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned முழு)(**p - '0');
	++*p;
    }

    எனில் (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	எனில் (overflow_err)
	{
	    format_overflow_error(pstart);
	    திரும்பு FAIL;
	}
	இல்லையெனில்
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    திரும்பு OK;
}


    நிலையான முழு
parse_fmt_types(
    மாறிலி எழுத்து  ***ap_types,
    முழு		*இலக்கம்_posarg,
    மாறிலி எழுத்து  *fmt,
    typval_T	*tvs UNUSED
    )
{
    மாறிலி எழுத்து	*p = fmt;
    மாறிலி எழுத்து	*arg = வெற்று;

    முழு		any_pos = 0;
    முழு		any_arg = 0;
    முழு		arg_idx;

# define CHECK_POS_ARG செய் { \
    எனில் (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	செல் error; \
    } \
} போது (0);

    எனில் (p == வெற்று)
	திரும்பு OK;

    போது (*p != NUL)
    {
	எனில் (*p != '%')
	{
	    மாறிலி எழுத்து    *q = strchr(p + 1, '%');
	    size_t  n = (q == வெற்று) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	இல்லையெனில்
	{
	    // allowed values: \0, h, l, L
	    எழுத்து	length_modifier = '\0';

	    // variable for positional arg
	    முழு		pos_arg = -1;
	    மாறிலி எழுத்து	*ptype = வெற்று;
	    மாறிலி எழுத்து	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    போது (VIM_ISDIGIT(*ptype))
		++ptype;

	    எனில் (*ptype == '$')
	    {
		எனில் (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    செல் error;
		}

		// Positional argument
		unsigned முழு uj;

		எனில் (get_unsigned_int(pstart, &p, &uj, tvs != வெற்று) == FAIL)
		    செல் error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    போது (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		தேர்வு (*p)
		{
		    நிலை '0': நிறுத்து;
		    நிலை '-': நிறுத்து;
		    நிலை '+': நிறுத்து;
		    நிலை ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      நிறுத்து;
		    நிலை '#': நிறுத்து;
		    நிலை '\'': நிறுத்து;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    எனில் (*(arg = p) == '*')
	    {
		p++;

		எனில் (VIM_ISDIGIT((முழு)(*p)))
		{
		    // Positional argument field width
		    unsigned முழு uj;

		    எனில் (get_unsigned_int(arg + 1, &p, &uj, tvs != வெற்று) == FAIL)
			செல் error;

		    எனில் (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			செல் error;
		    }
		    இல்லையெனில்
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			எனில் (adjust_types(ap_types, uj, இலக்கம்_posarg, arg) == FAIL)
			    செல் error;
		    }
		}
		இல்லையெனில்
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    இல்லையெனில் எனில் (VIM_ISDIGIT((முழு)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		மாறிலி எழுத்து *digstart = p;
		unsigned முழு uj;

		எனில் (get_unsigned_int(digstart, &p, &uj, tvs != வெற்று) == FAIL)
		    செல் error;

		எனில் (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    செல் error;
		}
	    }

	    // parse precision
	    எனில் (*p == '.')
	    {
		p++;

		எனில் (*(arg = p) == '*')
		{
		    p++;

		    எனில் (VIM_ISDIGIT((முழு)(*p)))
		    {
			// Parse precision
			unsigned முழு uj;

			எனில் (get_unsigned_int(arg + 1, &p, &uj, tvs != வெற்று) == FAIL)
			    செல் error;

			எனில் (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    எனில் (adjust_types(ap_types, uj, இலக்கம்_posarg, arg) == FAIL)
				செல் error;
			}
			இல்லையெனில்
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    செல் error;
			}
		    }
		    இல்லையெனில்
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		இல்லையெனில் எனில் (VIM_ISDIGIT((முழு)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    மாறிலி எழுத்து *digstart = p;
		    unsigned முழு uj;

		    எனில் (get_unsigned_int(digstart, &p, &uj, tvs != வெற்று) == FAIL)
			செல் error;

		    எனில் (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			செல் error;
		    }
		}
	    }

	    எனில் (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    எனில் (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		எனில் (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    தேர்வு (*p)
	    {
		// Check for known format specifiers. % is special!
		நிலை 'i':
		நிலை '*':
		நிலை 'd':
		நிலை 'u':
		நிலை 'o':
		நிலை 'D':
		நிலை 'U':
		நிலை 'O':
		நிலை 'x':
		நிலை 'X':
		நிலை 'b':
		நிலை 'B':
		நிலை 'c':
		நிலை 's':
		நிலை 'S':
		நிலை 'p':
		நிலை 'f':
		நிலை 'F':
		நிலை 'e':
		நிலை 'E':
		நிலை 'g':
		நிலை 'G':
		    எனில் (pos_arg != -1)
		    {
			எனில் (adjust_types(ap_types, pos_arg, இலக்கம்_posarg, ptype) == FAIL)
			    செல் error;
		    }
		    இல்லையெனில்
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    நிறுத்து;

		இயல்பு:
		    எனில் (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			செல் error;
		    }
	    }

	    எனில் (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    க்கு (arg_idx = 0; arg_idx < *இலக்கம்_posarg; ++arg_idx)
    {
	எனில் ((*ap_types)[arg_idx] == வெற்று)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    செல் error;
	}

# எனில் defined(FEAT_EVAL)
	எனில் (tvs != வெற்று && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    செல் error;
	}
# endif
    }

    திரும்பு OK;

error:
    vim_free((எழுத்து**)*ap_types);
    *ap_types = வெற்று;
    *இலக்கம்_posarg = 0;
    திரும்பு FAIL;
}

    நிலையான வெறுமை
skip_to_arg(
    மாறிலி எழுத்து	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    முழு		*arg_idx,
    முழு		*arg_cur,
    மாறிலி எழுத்து	*fmt)
{
    முழு		arg_min = 0;

    எனில் (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	திரும்பு;
    }

    எனில் (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    இல்லையெனில்
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    க்கு (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	மாறிலி எழுத்து *p;

	எனில் (ap_types == வெற்று || ap_types[*arg_cur] == வெற்று)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    திரும்பு;
	}

	p = ap_types[*arg_cur];

	முழு fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	தேர்வு (fmt_type)
	{
	நிலை TYPE_PERCENT:
	நிலை TYPE_UNKNOWN:
	    நிறுத்து;

	நிலை TYPE_CHAR:
	    va_arg(*ap, முழு);
	    நிறுத்து;

	நிலை TYPE_STRING:
	    va_arg(*ap, எழுத்து *);
	    நிறுத்து;

	நிலை TYPE_POINTER:
	    va_arg(*ap, வெறுமை *);
	    நிறுத்து;

	நிலை TYPE_INT:
	    va_arg(*ap, முழு);
	    நிறுத்து;

	நிலை TYPE_LONGINT:
	    va_arg(*ap, long முழு);
	    நிறுத்து;

	நிலை TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    நிறுத்து;

	நிலை TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned முழு);
	    நிறுத்து;

	நிலை TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long முழு);
	    நிறுத்து;

	நிலை TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    நிறுத்து;

	நிலை TYPE_FLOAT:
	    va_arg(*ap, இரட்டை);
	    நிறுத்து;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    திரும்பு;
}

    முழு
vim_vsnprintf_typval(
    எழுத்து	*str,
    size_t	str_m,
    மாறிலி எழுத்து	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    மாறிலி எழுத்து	*p = fmt;
    முழு		arg_cur = 0;
    முழு		இலக்கம்_posarg = 0;
    முழு		arg_idx = 1;
    va_list	ap;
    மாறிலி எழுத்து	**ap_types = வெற்று;

    எனில் (parse_fmt_types(&ap_types, &இலக்கம்_posarg, fmt, tvs) == FAIL)
	திரும்பு 0;

    va_copy(ap, ap_start);

    எனில் (p == வெற்று)
	p = "";
    போது (*p != NUL)
    {
	எனில் (*p != '%')
	{
	    மாறிலி எழுத்து    *q = strchr(p + 1, '%');
	    size_t  n = (q == வெற்று) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    எனில் (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	இல்லையெனில்
	{
	    size_t  min_field_width = 0, precision = 0;
	    முழு	    zero_padding = 0, precision_specified = 0, justify_இடது = 0;
	    முழு	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    முழு	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    எழுத்து    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    எழுத்து    tmp[TMP_LEN];

	    // string address in case of string argument
	    மாறிலி எழுத்து  *str_arg = வெற்று;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    unsigned எழுத்து uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  எண்_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    எழுத்து    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = வெற்று;

	    // variables for positional arg
	    முழு	    pos_arg = -1;
	    மாறிலி எழுத்து	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    போது (VIM_ISDIGIT(*ptype))
		++ptype;

	    எனில் (*ptype == '$')
	    {
		// Positional argument
		மாறிலி எழுத்து *digstart = p;
		unsigned முழு uj;

		எனில் (get_unsigned_int(digstart, &p, &uj, tvs != வெற்று) == FAIL)
		    செல் error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    போது (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		தேர்வு (*p)
		{
		    நிலை '0': zero_padding = 1; நிறுத்து;
		    நிலை '-': justify_இடது = 1; நிறுத்து;
		    நிலை '+': force_sign = 1; space_for_positive = 0; நிறுத்து;
		    நிலை ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      நிறுத்து;
		    நிலை '#': alternate_form = 1; நிறுத்து;
		    நிலை '\'': நிறுத்து;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    எனில் (*p == '*')
	    {
		முழு j;
		மாறிலி எழுத்து *digstart = p + 1;

		p++;

		எனில் (VIM_ISDIGIT((முழு)(*p)))
		{
		    // Positional argument field width
		    unsigned முழு uj;

		    எனில் (get_unsigned_int(digstart, &p, &uj, tvs != வெற்று) == FAIL)
			செல் error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# எனில் defined(FEAT_EVAL)
		    tvs != வெற்று ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, முழு));

		எனில் (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    எனில் (tvs != வெற்று)
		    {
			format_overflow_error(digstart);
			செல் error;
		    }
		    இல்லையெனில்
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		எனில் (j >= 0)
		    min_field_width = j;
		இல்லையெனில்
		{
		    min_field_width = -j;
		    justify_இடது = 1;
		}
	    }
	    இல்லையெனில் எனில் (VIM_ISDIGIT((முழு)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		மாறிலி எழுத்து *digstart = p;
		unsigned முழு uj;

		எனில் (get_unsigned_int(digstart, &p, &uj, tvs != வெற்று) == FAIL)
		    செல் error;

		min_field_width = uj;
	    }

	    // parse precision
	    எனில் (*p == '.')
	    {
		p++;
		precision_specified = 1;

		எனில் (VIM_ISDIGIT((முழு)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    மாறிலி எழுத்து *digstart = p;
		    unsigned முழு uj;

		    எனில் (get_unsigned_int(digstart, &p, &uj, tvs != வெற்று) == FAIL)
			செல் error;

		    precision = uj;
		}
		இல்லையெனில் எனில் (*p == '*')
		{
		    முழு j;
		    மாறிலி எழுத்து *digstart = p;

		    p++;

		    எனில் (VIM_ISDIGIT((முழு)(*p)))
		    {
			// positional argument
			unsigned முழு uj;

			எனில் (get_unsigned_int(digstart, &p, &uj, tvs != வெற்று) == FAIL)
			    செல் error;

			arg_idx = uj;

			++p;
		    }

		    j =
# எனில் defined(FEAT_EVAL)
			tvs != வெற்று ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, முழு));

		    எனில் (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			எனில் (tvs != வெற்று)
			{
			    format_overflow_error(digstart);
			    செல் error;
			}
			இல்லையெனில்
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    எனில் (j >= 0)
			precision = j;
		    இல்லையெனில்
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    எனில் (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		எனில் (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    length_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    தேர்வு (fmt_spec)
	    {
		நிலை 'i': fmt_spec = 'd'; நிறுத்து;
		நிலை 'D': fmt_spec = 'd'; length_modifier = 'l'; நிறுத்து;
		நிலை 'U': fmt_spec = 'u'; length_modifier = 'l'; நிறுத்து;
		நிலை 'O': fmt_spec = 'o'; length_modifier = 'l'; நிறுத்து;
		இயல்பு: நிறுத்து;
	    }

# எனில் defined(FEAT_EVAL)
	    தேர்வு (fmt_spec)
	    {
		நிலை 'd': நிலை 'u': நிலை 'o': நிலை 'x': நிலை 'X':
		    எனில் (tvs != வெற்று && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    எனில் (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    தேர்வு (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    நிலை '%':
	    நிலை 'c':
	    நிலை 's':
	    நிலை 'S':
		str_arg_l = 1;
		தேர்வு (fmt_spec)
		{
		நிலை '%':
		    str_arg = p;
		    நிறுத்து;

		நிலை 'c':
		    {
			முழு j;

			j =
# எனில் defined(FEAT_EVAL)
			    tvs != வெற்று ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, முழு));

			// standard demands unsigned char
			uchar_arg = (unsigned எழுத்து)j;
			str_arg = (எழுத்து *)&uchar_arg;
			நிறுத்து;
		    }

		நிலை 's':
		நிலை 'S':
		    str_arg =
# எனில் defined(FEAT_EVAL)
				tvs != வெற்று ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, எழுத்து *));

		    எனில் (str_arg == வெற்று)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    இல்லையெனில் எனில் (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    இல்லையெனில் எனில் (precision == 0)
			str_arg_l = 0;
		    இல்லையெனில்
		    {
			// memchr on HP does not like n > 2^31  !!!
			மாறிலி எழுத்து *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == வெற்று) ? precision
						      : (size_t)(q - str_arg);
		    }
		    எனில் (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			முழு	cell;

			க்கு (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    எனில் (precision_specified && i + cell > precision)
				நிறுத்து;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			எனில் (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    நிறுத்து;

		இயல்பு:
		    நிறுத்து;
		}
		நிறுத்து;

	    நிலை 'd': நிலை 'u':
	    நிலை 'b': நிலை 'B':
	    நிலை 'o':
	    நிலை 'x': நிலை 'X':
	    நிலை 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //மதிப்பு

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    முழு arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    முழு int_arg = 0;
		    unsigned முழு uint_arg = 0;

		    // only set for length modifier l
		    long முழு long_arg = 0;
		    unsigned long முழு ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    வெறுமை *ptr_arg = வெற்று;

		    எனில் (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# எனில் defined(FEAT_EVAL)
				 tvs != வெற்று ? (வெறுமை *)tv_str(tvs, &arg_idx,
									வெற்று) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, வெறுமை *));

			எனில் (ptr_arg != வெற்று)
			    arg_sign = 1;
		    }
		    இல்லையெனில் எனில் (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# எனில் defined(FEAT_EVAL)
				    tvs != வெற்று ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			எனில் (bin_arg != 0)
			    arg_sign = 1;
		    }
		    இல்லையெனில் எனில் (fmt_spec == 'd')
		    {
			// signed
			தேர்வு (length_modifier)
			{
			நிலை '\0':
			நிலை 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# எனில் defined(FEAT_EVAL)
					tvs != வெற்று ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, முழு));

			    எனில் (int_arg > 0)
				arg_sign =  1;
			    இல்லையெனில் எனில் (int_arg < 0)
				arg_sign = -1;
			    நிறுத்து;
			நிலை 'l':
			    long_arg =
# எனில் defined(FEAT_EVAL)
					tvs != வெற்று ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long முழு));

			    எனில் (long_arg > 0)
				arg_sign =  1;
			    இல்லையெனில் எனில் (long_arg < 0)
				arg_sign = -1;
			    நிறுத்து;
			நிலை 'L':
			    llong_arg =
# எனில் defined(FEAT_EVAL)
					tvs != வெற்று ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    எனில் (llong_arg > 0)
				arg_sign =  1;
			    இல்லையெனில் எனில் (llong_arg < 0)
				arg_sign = -1;
			    நிறுத்து;
			}
		    }
		    இல்லையெனில்
		    {
			// unsigned
			தேர்வு (length_modifier)
			{
			    நிலை '\0':
			    நிலை 'h':
				uint_arg =
# எனில் defined(FEAT_EVAL)
					    tvs != வெற்று ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned முழு));

				எனில் (uint_arg != 0)
				    arg_sign = 1;
				நிறுத்து;
			    நிலை 'l':
				ulong_arg =
# எனில் defined(FEAT_EVAL)
					    tvs != வெற்று ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long முழு));

				எனில் (ulong_arg != 0)
				    arg_sign = 1;
				நிறுத்து;
			    நிலை 'L':
				ullong_arg =
# எனில் defined(FEAT_EVAL)
					    tvs != வெற்று ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				எனில் (ullong_arg != 0)
				    arg_sign = 1;
				நிறுத்து;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    எனில் (precision_specified)
			zero_padding = 0;
		    எனில் (fmt_spec == 'd')
		    {
			எனில் (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    இல்லையெனில் எனில் (alternate_form)
		    {
			எனில் (arg_sign != 0
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
		    எனில் (!precision_specified)
			precision = 1;   // default precision is 1
		    எனில் (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    இல்லையெனில்
		    {
			எழுத்து	f[6];
			முழு	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			எனில் (!length_modifier)
			    ;
			இல்லையெனில் எனில் (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# இல்லையெனில்
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			இல்லையெனில்
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			எனில் (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			இல்லையெனில் எனில் (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    எழுத்து	    b[8 * அளவு(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    செய்
			    {
				b[அளவு(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    போது (bn != 0);

			    memcpy(tmp + str_arg_l, b + அளவு(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			இல்லையெனில் எனில் (fmt_spec == 'd')
			{
			    // signed
			    தேர்வு (length_modifier)
			    {
			    நிலை '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       நிறுத்து;
			    நிலை 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (short)int_arg);
				      நிறுத்து;
			    நிலை 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      நிறுத்து;
			    நிலை 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      நிறுத்து;
			    }
			}
			இல்லையெனில்
			{
			    // unsigned
			    தேர்வு (length_modifier)
			    {
			    நிலை '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       நிறுத்து;
			    நிலை 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(unsigned short)uint_arg);
				      நிறுத்து;
			    நிலை 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      நிறுத்து;
			    நிலை 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      நிறுத்து;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			எனில் (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			எனில் (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t இலக்கம்_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			எனில் (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    எனில் (!precision_specified
					     || precision < இலக்கம்_of_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = இலக்கம்_of_digits + 1;
			    }
			}
			// zero padding to specified precision?
			எனில் (இலக்கம்_of_digits < precision)
			    எண்_of_zeros_to_pad = precision - இலக்கம்_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    எனில் (!justify_இடது && zero_padding)
		    {
			முழு n = (முழு)(min_field_width - (str_arg_l
						    + எண்_of_zeros_to_pad));
			எனில் (n > 0)
			    எண்_of_zeros_to_pad += n;
		    }
		    நிறுத்து;
		}

	    நிலை 'f':
	    நிலை 'F':
	    நிலை 'e':
	    நிலை 'E':
	    நிலை 'g':
	    நிலை 'G':
		{
		    // Floating point.
		    இரட்டை	f;
		    இரட்டை	abs_f;
		    எழுத்து	format[40];
		    முழு		l;
		    முழு		remove_trailing_zeroes = FALSE;

		    f =
# எனில் defined(FEAT_EVAL)
			tvs != வெற்று ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, இரட்டை));

		    abs_f = f < 0 ? -f : f;

		    எனில் (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			எனில் ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			இல்லையெனில்
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    எனில் ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# இல்லையெனில்
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
		    இல்லையெனில்
		    {
			எனில் (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			இல்லையெனில் எனில் (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			இல்லையெனில்
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    எனில் (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    எனில் (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				எனில் ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				எனில் (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (முழு)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			எனில் (remove_trailing_zeroes)
			{
			    முழு i;
			    எழுத்து *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    எனில் (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    இல்லையெனில்
			    {
				tp = (எழுத்து *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				எனில் (tp != வெற்று)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    எனில் (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    போது (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    எனில் (tp != வெற்று && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				போது (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			இல்லையெனில்
			{
			    எழுத்து *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (எழுத்து *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    எனில் (tp != வெற்று && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    எனில் (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			எண்_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    நிறுத்து;
		}

	    இயல்பு:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_இடது = 1;
		min_field_width = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		எனில் (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		நிறுத்து;
	    }

	    எனில் (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    எனில் (!justify_இடது)
	    {
		// left padding with blank or zero
		முழு pn = (முழு)(min_field_width - (str_arg_l + எண்_of_zeros_to_pad));

		எனில் (pn > 0)
		{
		    எனில் (str_l < str_m)
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
	    எனில் (எண்_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    இல்லையெனில்
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		முழு zn = (முழு)zero_padding_insertion_ind;

		எனில் (zn > 0)
		{
		    எனில் (str_l < str_m)
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
		zn = (முழு)எண்_of_zeros_to_pad;
		எனில் (zn > 0)
		{
		    எனில் (str_l < str_m)
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
		முழு sn = (முழு)(str_arg_l - zero_padding_insertion_ind);

		எனில் (sn > 0)
		{
		    எனில் (str_l < str_m)
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
	    எனில் (justify_இடது)
	    {
		// right blank padding to the field width
		முழு pn = (முழு)(min_field_width
				      - (str_arg_l + எண்_of_zeros_to_pad));

		எனில் (pn > 0)
		{
		    எனில் (str_l < str_m)
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

    எனில் (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    எனில் (tvs != வெற்று && tvs[இலக்கம்_posarg != 0 ? இலக்கம்_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((எழுத்து*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    திரும்பு (முழு)str_l;
}

#endif // PROTO
