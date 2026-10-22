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
#অন্তর্ভাৱ "vim.h"

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
    যদি (p != রিক্ত)
	mch_memmove(p, string, len);
    ঘূৰাই p;
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
    যদি (p == রিক্ত)
	ঘূৰাই রিক্ত;

    STRNCPY(p, string, len);
    p[len] = NUL;
    ঘূৰাই p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    ঘূৰাই vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    পূর্ণাঙ্ক		cc,
    পূর্ণাঙ্ক		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    পূর্ণাঙ্ক		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    বাবে (p = string; *p; p++)
    {
	যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    অনুৱর্ত;
	}
	যদি (vim_strchr(esc_chars, *p) != রিক্ত || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    যদি (escaped_string == রিক্ত)
	ঘূৰাই রিক্ত;
    p2 = escaped_string;
    বাবে (p = string; *p; p++)
    {
	যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    অনুৱর্ত;
	}
	যদি (vim_strchr(esc_chars, *p) != রিক্ত || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    ঘূৰাই escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    পূর্ণাঙ্ক
csh_like_shell(শূন্য)
{
    ঘূৰাই (strstr((ৱর্ণ *)gettail(p_sh), "csh") != রিক্ত);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    স্থির পূর্ণাঙ্ক
fish_like_shell(শূন্য)
{
    ঘূৰাই (strstr((ৱর্ণ *)gettail(p_sh), "fish") != রিক্ত);
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
vim_strsave_shellescape(char_u *string, পূর্ণাঙ্ক do_special, পূর্ণাঙ্ক do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    পূর্ণাঙ্ক		csh_like;
    পূর্ণাঙ্ক		fish_like;
    char_u	*shname;
    পূর্ণাঙ্ক		powershell;
#ifdef MSWIN
    পূর্ণাঙ্ক		double_quotes;
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
    powershell = strstr((ৱর্ণ *)shname, "pwsh") != রিক্ত;
#ifdef MSWIN
    powershell = powershell || strstr((ৱর্ণ *)shname, "powershell") != রিক্ত;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (unsigned)STRLEN(string) + 3;  // two quotes and a trailing NUL
    বাবে (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	যদি (double_quotes)
	{
	    যদি (*p == '"')
		++length;		// " -> ""
	}
	অন্যথা
#endif
	যদি (*p == '\'')
	{
	    যদি (powershell)
		length += 2;		// ' => ''
	    অন্যথা
		length += 3;		// ' => '\''
	}
	যদি ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    যদি (csh_like && do_special)
		++length;		// insert backslash
	}
	যদি (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	যদি (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    যদি (escaped_string != রিক্ত)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	যদি (double_quotes)
	    *d++ = '"';
	অন্যথা
#endif
	    *d++ = '\'';

	বাবে (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    যদি (double_quotes)
	    {
		যদি (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    অনুৱর্ত;
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
		অনুৱর্ত;
	    }
	    যদি ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		যদি (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		অনুৱর্ত;
	    }
	    যদি (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		অনুৱর্ত;
	    }
	    যদি (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		অনুৱর্ত;
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

    ঘূৰাই escaped_string;
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
    ঘূৰাই p1;
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
    ঘূৰাই p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    শূন্য
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    পূর্ণাঙ্ক	    c;

    যদি (p == রিক্ত)
	ঘূৰাই;

    p2 = p;
    যেতিয়ালৈ ((c = *p2) != NUL)
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

    যদি (res != রিক্ত)
	যেতিয়ালৈ (*p != NUL)
	{
	    পূর্ণাঙ্ক		l;

	    যদি (enc_utf8)
	    {
		পূর্ণাঙ্ক	c, uc;
		পূর্ণাঙ্ক	newl;
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
		    যদি (s == রিক্ত)
		    {
			vim_free(res);
			ঘূৰাই রিক্ত;
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

    ঘূৰাই res;
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

    যদি (res != রিক্ত)
	যেতিয়ালৈ (*p != NUL)
	{
	    পূর্ণাঙ্ক		l;

	    যদি (enc_utf8)
	    {
		পূর্ণাঙ্ক	c, lc;
		পূর্ণাঙ্ক	newl;
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
		    যদি (s == রিক্ত)
		    {
			vim_free(res);
			ঘূৰাই রিক্ত;
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

    ঘূৰাই res;
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
    যেতিয়ালৈ (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
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
vim_strlen_maxlen(ৱর্ণ *s, size_t maxlen)
{
    size_t i;
    বাবে (i = 0; i < maxlen; ++i)
	যদি (s[i] == NUL)
	    ভঙ্গ;
    ঘূৰাই i;
}

#যদি !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    পূর্ণাঙ্ক
vim_stricmp(ৱর্ণ *s1, ৱর্ণ *s2)
{
    পূর্ণাঙ্ক		i;

    বাবে (;;)
    {
	i = (পূর্ণাঙ্ক)TOLOWER_LOC(*s1) - (পূর্ণাঙ্ক)TOLOWER_LOC(*s2);
	যদি (i != 0)
	    ঘূৰাই i;			    // this character different
	যদি (*s1 == NUL)
	    ভঙ্গ;			    // strings match until NUL
	++s1;
	++s2;
    }
    ঘূৰাই 0;				    // strings match
}
#endif

#যদি !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    পূর্ণাঙ্ক
vim_strnicmp(ৱর্ণ *s1, ৱর্ণ *s2, size_t len)
{
    পূর্ণাঙ্ক		i;

    যেতিয়ালৈ (len > 0)
    {
	i = (পূর্ণাঙ্ক)TOLOWER_LOC(*s1) - (পূর্ণাঙ্ক)TOLOWER_LOC(*s2);
	যদি (i != 0)
	    ঘূৰাই i;			    // this character different
	যদি (*s1 == NUL)
	    ভঙ্গ;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ঘূৰাই 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    পূর্ণাঙ্ক
vim_strnicmp_asc(ৱর্ণ *s1, ৱর্ণ *s2, size_t len)
{
    পূর্ণাঙ্ক                i = 0;

    যেতিয়ালৈ (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	যদি (i != 0)
	    ভঙ্গ;			// this character is different
	যদি (*s1 == NUL)
	    ভঙ্গ;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ঘূৰাই i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, পূর্ণাঙ্ক c)
{
    char_u	*p;
    পূর্ণাঙ্ক		b;

    p = string;
    যদি (enc_utf8 && c > 0 && c < 0x80)
	ঘূৰাই vim_strbyte(string, c);
    যদি (enc_utf8 && c >= 0x80)
    {
	যেতিয়ালৈ (*p != NUL)
	{
	    পূর্ণাঙ্ক l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    যদি (utf_ptr2char(p) == c && l > 1)
		ঘূৰাই p;
	    p += l;
	}
	ঘূৰাই রিক্ত;
    }
    যদি (enc_dbcs != 0 && c > 255)
    {
	পূর্ণাঙ্ক	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	যেতিয়ালৈ ((b = *p) != NUL)
	{
	    যদি (b == c && p[1] == n2)
		ঘূৰাই p;
	    p += (*mb_ptr2len)(p);
	}
	ঘূৰাই রিক্ত;
    }
    যদি (has_mbyte)
    {
	যেতিয়ালৈ ((b = *p) != NUL)
	{
	    যদি (b == c)
		ঘূৰাই p;
	    p += (*mb_ptr2len)(p);
	}
	ঘূৰাই রিক্ত;
    }
    যেতিয়ালৈ ((b = *p) != NUL)
    {
	যদি (b == c)
	    ঘূৰাই p;
	++p;
    }
    ঘূৰাই রিক্ত;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, পূর্ণাঙ্ক c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    যদি (c <= 0 || c > 255)
	ঘূৰাই রিক্ত;
    ঘূৰাই (char_u *)strchr((ৱর্ণ *)string, c);
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
vim_strrchr(char_u *string, পূর্ণাঙ্ক c)
{
    char_u	*retval = রিক্ত;
    char_u	*p = string;

    যেতিয়ালৈ (*p)
    {
	যদি (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    ঘূৰাই retval;
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
    যেতিয়ালৈ (*s)
    {
	যদি (vim_strchr(charset, *s) != রিক্ত)
	    ঘূৰাই s;
	MB_PTR_ADV(s);
    }
    ঘূৰাই রিক্ত;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
স্থির পূর্ণাঙ্ক sort_compare(ধ্রুৱ শূন্য *s1, ধ্রুৱ শূন্য *s2);

    স্থির পূর্ণাঙ্ক
sort_compare(ধ্রুৱ শূন্য *s1, ধ্রুৱ শূন্য *s2)
{
    ঘূৰাই STRCMP(*(ৱর্ণ **)s1, *(ৱর্ণ **)s2);
}

    শূন্য
sort_strings(
    char_u	**files,
    পূর্ণাঙ্ক		গুনতি)
{
    qsort((শূন্য *)files, (size_t)গুনতি, পরিমাণ(char_u *), sort_compare);
}

#যদি defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    পূর্ণাঙ্ক
has_non_ascii(char_u *s)
{
    char_u	*p;

    যদি (s != রিক্ত)
	বাবে (p = s; *p != NUL; ++p)
	    যদি (*p >= 128)
		ঘূৰাই TRUE;
    ঘূৰাই FALSE;
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
    size_t  l = str1 == রিক্ত ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == রিক্ত ? 0 : STRLEN(str2)) + 1L);
    যদি (dest == রিক্ত)
	ঘূৰাই রিক্ত;
    যদি (str1 == রিক্ত)
	*dest = NUL;
    অন্যথা
	STRCPY(dest, str1);
    যদি (str2 != রিক্ত)
	STRCPY(dest + l, str2);
    ঘূৰাই dest;
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
    যদি (rev == রিক্ত)
	ঘূৰাই রিক্ত;

    বাবে (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	যদি (has_mbyte)
	{
	    পূর্ণাঙ্ক mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	অন্যথা
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    ঘূৰাই rev;
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, পূর্ণাঙ্ক function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    যদি (str != রিক্ত)
    {
	len += (unsigned)STRLEN(str);
	বাবে (p = str; *p != NUL; MB_PTR_ADV(p))
	    যদি (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    যদি (r == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    অন্যথা
	*r++ = '\'';
    যদি (str != রিক্ত)
	বাবে (p = str; *p != NUL; )
	{
	    যদি (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    যদি (function)
	*r++ = ')';
    *r++ = NUL;
    ঘূৰাই s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_গুনতি(char_u *haystack, char_u *needle, পূর্ণাঙ্ক ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    যদি (p == রিক্ত || needle == রিক্ত || *needle == NUL)
	ঘূৰাই 0;

    size_t  needlelen = STRLEN(needle);
    যদি (ic)
    {
	যেতিয়ালৈ (*p != NUL)
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
	যেতিয়ালৈ ((next = (char_u *)strstr((ৱর্ণ *)p, (ৱর্ণ *)needle)) != রিক্ত)
	{
	    ++n;
	    p = next + needlelen;
	}

    ঘূৰাই n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    স্থির পূর্ণাঙ্ক
copy_first_char_to_tv(char_u *প্ৰবেশ, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    পূর্ণাঙ্ক		len;

    যদি (প্ৰবেশ == রিক্ত || output == রিক্ত)
	ঘূৰাই -1;

    len = has_mbyte ? mb_ptr2len(প্ৰবেশ) : 1;
    STRNCPY(buf, প্ৰবেশ, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    ঘূৰাই output->vval.v_string == রিক্ত ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    শূন্য
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*ৰাশিমালা,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    পূর্ণাঙ্ক		len = 0;
    পূর্ণাঙ্ক		idx = 0;
    পূর্ণাঙ্ক		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_চাবি, VAR_সংখ্যা);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ৰাশিমালা_get_funccal(ৰাশিমালা, &newtv);

    ga_init2(&ga, পরিমাণ(ৱর্ণ), 80);
    বাবে (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	যদি (len < 0)
	    ভঙ্গ;

	set_vim_var_nr(VV_চাবি, idx);
	যদি (filter_map_one(&tv, ৰাশিমালা, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    ভঙ্গ;
	}
	যদি (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    যদি (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		ভঙ্গ;
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
    rettv->vval.v_string = ga.ga_data;
    যদি (fc != রিক্ত)
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
	typval_T	*ৰাশিমালা,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    পূর্ণাঙ্ক		len;
    typval_T	argv[3];
    পূর্ণাঙ্ক		r;
    পূর্ণাঙ্ক		called_emsg_start = called_emsg;
    funccall_T	*fc;

    যদি (argvars[2].v_type == VAR_UNKNOWN)
    {
	যদি (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_মান), "String");
	    ঘূৰাই;
	}
	len = copy_first_char_to_tv(p, rettv);
	যদি (len < 0)
	    ঘূৰাই;
	p += len;
    }
    অন্যথা যদি (check_for_string_arg(argvars, 2) == FAIL)
	ঘূৰাই;
    অন্যথা
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ৰাশিমালা_get_funccal(ৰাশিমালা, rettv);

    বাবে ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	যদি (len < 0)
	{
	    *rettv = argv[0];
	    ভঙ্গ;
	}

	r = eval_ৰাশিমালা_typval(ৰাশিমালা, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	যদি (r == FAIL || called_emsg != called_emsg_start)
	    ভঙ্গ;
    }

    যদি (fc != রিক্ত)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    স্থির শূন্য
byteidx_common(typval_T *argvars, typval_T *rettv, পূর্ণাঙ্ক comp)
{
    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_সংখ্যা_chk(&argvars[1], রিক্ত);
    যদি (str == রিক্ত || idx < 0)
	ঘূৰাই;

    varnumber_T	utf16idx = FALSE;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	পূর্ণাঙ্ক error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	যদি (error)
	    ঘূৰাই;
	যদি (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_সংখ্যা_as_bool_nr), utf16idx);
	    ঘূৰাই;
	}
    }

    পূর্ণাঙ্ক (*ptr2len)(char_u *);
    যদি (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    অন্যথা
	ptr2len = mb_ptr2len;

    char_u *t = str;
    বাবে ( ; idx > 0; idx--)
    {
	যদি (*t == NUL)		// EOL reached
	    ঘূৰাই;
	যদি (utf16idx)
	{
	    পূর্ণাঙ্ক clen = ptr2len(t);
	    পূর্ণাঙ্ক c = (clen > 1) ? utf_ptr2char(t) : *t;
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
	ঘূৰাই;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_সংখ্যা_chk(&argvars[1], রিক্ত);
    যদি (str == রিক্ত || idx < 0)
	ঘূৰাই;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	যদি (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    পূর্ণাঙ্ক (*ptr2len)(char_u *);
    যদি (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    অন্যথা
	ptr2len = mb_ptr2len;

    char_u	*p;
    পূর্ণাঙ্ক		len;
    বাবে (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	যদি (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    যদি (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_সংখ্যা = len;
	    ঘূৰাই;
	}
	যদি (utf16idx)
	{
	    idx--;
	    পূর্ণাঙ্ক clen = ptr2len(p);
	    পূর্ণাঙ্ক c = (clen > 1) ? utf_ptr2char(p) : *p;
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
    স্থির পূর্ণাঙ্ক
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    যদি (convert_setup(&vimconv, from, to) == FAIL)
	ঘূৰাই FAIL;
    vimconv.vc_fail = TRUE;
    যদি (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	যদি (ret->string == রিক্ত)
	    ret->length = 0;
	অন্যথা
	    ret->length = str->length;
    }
    অন্যথা
    {
	পূর্ণাঙ্ক len = (পূর্ণাঙ্ক)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, রিক্ত, রিক্ত);

    ঘূৰাই (ret->string == রিক্ত) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    স্থির শূন্য
blob_from_string(char_u *str, blob_T *blob)
{
    পূর্ণাঙ্ক	    len = (পূর্ণাঙ্ক)STRLEN(str);
    char_u  *dest;

    যদি (len == 0)
	ঘূৰাই;
    যদি (ga_grow(&blob->bv_ga, len) == FAIL)
	ঘূৰাই;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    বাবে (পূর্ণাঙ্ক i = 0; i < len; ++i)
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
    স্থির পূর্ণাঙ্ক
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	সাৰি_len;

    যদি (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	ঘূৰাই (ret->string == রিক্ত) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    সাৰি_len = (nl == রিক্ত) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(সাৰি_len + 1);
    যদি (ret->string == রিক্ত)
    {
	ret->length = 0;
	ঘূৰাই FAIL;
    }
    যদি (সাৰি_len > 0)
	mch_memmove(ret->string, src, (size_t)সাৰি_len);
    ret->string[সাৰি_len] = NUL;
    ret->length = (size_t)সাৰি_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    বাবে (long i = 0; i < সাৰি_len; i++)
	যদি (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + সাৰি_len + (nl != রিক্ত ? 1 : 0);
    ঘূৰাই OK;
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
    যদি (from_encoding_raw == রিক্ত)
	ঘূৰাই রিক্ত;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    যেতিয়ালৈ (*s != NUL)
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

    ঘূৰাই from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    স্থির শূন্য
append_converted_string_to_list(
	string_T *converted,
	পূর্ণাঙ্ক validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    যদি (converted->string != রিক্ত)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	যেতিয়ালৈ (p < end)
	{
	    string_T	সাৰি;
	    char_u	*সাৰি_start = p;

	    যেতিয়ালৈ (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    সাৰি.length = (size_t)(p - সাৰি_start);
	    সাৰি.string = vim_strnsave(সাৰি_start, সাৰি.length);
	    যদি (সাৰি.string != রিক্ত)
	    {
		যদি (validate_utf8 && !utf_valid_string(সাৰি.string, রিক্ত))
		{
		    vim_free(সাৰি.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    ঘূৰাই; // Stop processing
		}
		যদি (list_append_string(list, সাৰি.string, (পূর্ণাঙ্ক)সাৰি.length) == FAIL)
		{
		    vim_free(সাৰি.string);
		    vim_free(converted->string);
		    ঘূৰাই; // Stop processing on append failure
		}
		vim_free(সাৰি.string);
	    }
	    অন্যথা
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		ঘূৰাই;
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

    স্থির পূর্ণাঙ্ক
append_validated_সাৰি_to_list(string_T *সাৰি, পূর্ণাঙ্ক validate_utf8, list_T *list)
{
    যদি (validate_utf8 && !utf_valid_string(সাৰি->string, রিক্ত))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(সাৰি->string);
	ঘূৰাই FAIL;
    }

    পূর্ণাঙ্ক ret = list_append_string(list, সাৰি->string, (পূর্ণাঙ্ক)সাৰি->length);
    vim_free(সাৰি->string);
    ঘূৰাই ret;
}

    শূন্য
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    পূর্ণাঙ্ক		blen;
    long	idx;
    পূর্ণাঙ্ক		validate_utf8 = FALSE;

    যদি (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ঘূৰাই;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ঘূৰাই;

    blob = argvars->vval.v_blob;
    যদি (blob == রিক্ত)
	ঘূৰাই;
    blen = blob_len(blob);

    char_u	*from_encoding = রিক্ত;
    char_u	*from_encoding_raw = রিক্ত;  // Encoding name with endianness preserved for iconv
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	যদি (d != রিক্ত)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    যদি (enc != রিক্ত)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		যদি (from_encoding_raw == রিক্ত)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    ঘূৰাই;
		}
	    }
	}
    }

    যদি (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    যদি (from_encoding != রিক্ত && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    পূর্ণাঙ্ক from_prop = 0;
    যদি (from_encoding != রিক্ত)
	from_prop = enc_canon_props(from_encoding);
    যদি (from_encoding != রিক্ত && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	পূর্ণাঙ্ক nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	যদি (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    যদি (blen > 0)
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
	যদি (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    গচ্ছ done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	পূর্ণাঙ্ক len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, রিক্ত);
	converted.length = len;
	convert_setup(&vimconv, রিক্ত, রিক্ত);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    অন্যথা
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	যেতিয়ালৈ (idx < blen)
	{
	    string_T	str;

	    যদি (string_from_blob(blob, &idx, &str) != OK)
		ভঙ্গ;

	    যদি (from_encoding != রিক্ত)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		পূর্ণাঙ্ক	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		যদি (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    গচ্ছ done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    যদি (append_validated_সাৰি_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		গচ্ছ done;
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
	ঘূৰাই;

    যদি (rettv_blob_alloc(rettv) == FAIL)
	ঘূৰাই;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    যদি (list == রিক্ত)
	ঘূৰাই;

    char_u	*to_encoding = রিক্ত;
    char_u	*to_encoding_raw = রিক্ত;  // Encoding name with endianness preserved for iconv
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	যদি (d != রিক্ত)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    যদি (enc != রিক্ত)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		যদি (to_encoding_raw == রিক্ত)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    ঘূৰাই;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    পূর্ণাঙ্ক to_prop = 0;
    যদি (to_encoding != রিক্ত)
	to_prop = enc_canon_props(to_encoding);
    যদি (to_encoding != রিক্ত && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_বস্তুসমূহ(list, li)
	{
	    char_u *s;

	    যদি (li->li_tv.v_type != VAR_STRING)
		অনুৱর্ত;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    যদি (li != list->lv_first)
		ga_append(&str_ga, NL);
	    যদি (s != রিক্ত && *s != NUL)
	    {
		পূর্ণাঙ্ক slen = (পূর্ণাঙ্ক)STRLEN(s);

		যদি (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    গচ্ছ done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
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
		গচ্ছ done;
	    }
	    vimconv.vc_fail = TRUE;

	    পূর্ণাঙ্ক		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, রিক্ত);
	    convert_setup(&vimconv, রিক্ত, রিক্ত);
	    ga_clear(&str_ga);

	    যদি (converted == রিক্ত)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		গচ্ছ done;
	    }
	    যদি (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
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
		অনুৱর্ত;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    যদি (str.string == রিক্ত)
		STR_LITERAL_SET(str, "");
	    অন্যথা
		str.length = STRLEN(str.string);

	    যদি (to_encoding != রিক্ত)
	    {
		পূর্ণাঙ্ক	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		যদি (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    গচ্ছ done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    যদি (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    যদি (to_encoding != রিক্ত)
		vim_free(str.string);
	}
    }

done:
    যদি (to_encoding != রিক্ত)
	vim_free(to_encoding);
    যদি (to_encoding_raw != রিক্ত)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    শূন্য
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    পূর্ণাঙ্ক		utf8 = FALSE;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ঘূৰাই;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (পূর্ণাঙ্ক)tv_get_bool_chk(&argvars[1], রিক্ত);

    p = tv_get_string(&argvars[0]);

    যদি (has_mbyte || utf8)
    {
	পূর্ণাঙ্ক (*ptr2len)(char_u *);
	পূর্ণাঙ্ক (*ptr2char)(char_u *);

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

	বাবে ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_সংখ্যা(rettv->vval.v_list, (*ptr2char)(p));
    }
    অন্যথা
	বাবে ( ; *p != NUL; ++p)
	    list_append_সংখ্যা(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    শূন্য
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    পূর্ণাঙ্ক		base = 10;
    char_u	*p;
    varnumber_T	n;
    পূর্ণাঙ্ক		what = 0;
    পূর্ণাঙ্ক		isneg;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ঘূৰাই;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[1]);
	যদি (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    ঘূৰাই;
	}
	যদি (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    যদি (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    ৱিকল্প (base)
    {
	পক্ষ 2: what |= STR2NR_BIN + STR2NR_FORCE; ভঙ্গ;
	পক্ষ 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; ভঙ্গ;
	পক্ষ 16: what |= STR2NR_HEX + STR2NR_FORCE; ভঙ্গ;
    }
    vim_str2nr(p, রিক্ত, রিক্ত, what, &n, রিক্ত, 0, FALSE, রিক্ত);
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
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		error = FALSE;
    পূর্ণাঙ্ক		charidx;
    পূর্ণাঙ্ক		byteidx = 0;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    str = tv_get_string_chk(&argvars[0]);
    যদি (str == রিক্ত)
	ঘূৰাই;
    len = (পূর্ণাঙ্ক)STRLEN(str);
    charidx = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (error)
	ঘূৰাই;

    যেতিয়ালৈ (charidx >= 0 && byteidx < len)
    {
	যদি (charidx == 0)
	{
	    rettv->vval.v_সংখ্যা = mb_ptr2char(str + byteidx);
	    ভঙ্গ;
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
    পূর্ণাঙ্ক		start_idx;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_সংখ্যা = -1;
    যদি (needle == রিক্ত || haystack == রিক্ত)
	ঘূৰাই;		// type error; errmsg already given

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	পূর্ণাঙ্ক	    error = FALSE;

	start_idx = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[2], &error);
	যদি (error || start_idx >= (পূর্ণাঙ্ক)STRLEN(haystack))
	    ঘূৰাই;
	যদি (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((ৱর্ণ *)haystack, (ৱর্ণ *)needle);
    যদি (pos != রিক্ত)
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
    যদি (rettv->vval.v_string != রিক্ত && tofree == রিক্ত)
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
	ঘূৰাই;

    rettv->vval.v_সংখ্যা = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    স্থির শূন্য
strchar_common(typval_T *argvars, typval_T *rettv, পূর্ণাঙ্ক skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    পূর্ণাঙ্ক			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    যেতিয়ালৈ (*s != NUL)
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
	ঘূৰাই;

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
	ঘূৰাই;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	পূর্ণাঙ্ক error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	যদি (error)
	    ঘূৰাই;
	যদি (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_সংখ্যা_as_bool_nr), skipcc);
	    ঘূৰাই;
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
	ঘূৰাই;

    varnumber_T countcc = FALSE;
    যদি (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    পূর্ণাঙ্ক			(*func_mb_ptr2char_adv)(char_u **pp);
    পূর্ণাঙ্ক			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    যেতিয়ালৈ (*s != NUL)
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
    পূর্ণাঙ্ক		col = 0;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    s = tv_get_string(&argvars[0]);
    যদি (argvars[1].v_type != VAR_UNKNOWN)
	col = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[1]);

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
	ঘূৰাই;

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
    পূর্ণাঙ্ক		nchar;
    পূর্ণাঙ্ক		nbyte = 0;
    পূর্ণাঙ্ক		charlen;
    পূর্ণাঙ্ক		skipcc = FALSE;
    পূর্ণাঙ্ক		len = 0;
    পূর্ণাঙ্ক		slen;
    পূর্ণাঙ্ক		error = FALSE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ঘূৰাই;

    p = tv_get_string(&argvars[0]);
    slen = (পূর্ণাঙ্ক)STRLEN(p);

    nchar = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (!error)
    {
	যদি (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    যদি (error)
		ঘূৰাই;
	    যদি (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_সংখ্যা_as_bool_nr), skipcc);
		ঘূৰাই;
	    }
	}

	যদি (nchar > 0)
	    যেতিয়ালৈ (nchar > 0 && nbyte < slen)
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
	    charlen = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[2]);
	    যেতিয়ালৈ (charlen > 0 && nbyte + len < slen)
	    {
		পূর্ণাঙ্ক off = nbyte + len;

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
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		slen;
    পূর্ণাঙ্ক		error = FALSE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ঘূৰাই;

    p = tv_get_string(&argvars[0]);
    slen = (পূর্ণাঙ্ক)STRLEN(p);

    n = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (error)
	len = 0;
    অন্যথা যদি (argvars[2].v_type != VAR_UNKNOWN)
	len = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[2]);
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
	পূর্ণাঙ্ক off;

	// length in characters
	বাবে (off = n; off < slen && len > 0; --len)
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
    char_u	*lastmatch = রিক্ত;
    পূর্ণাঙ্ক		haystack_len, end_idx;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_সংখ্যা = -1;
    যদি (needle == রিক্ত || haystack == রিক্ত)
	ঘূৰাই;		// type error; errmsg already given

    haystack_len = (পূর্ণাঙ্ক)STRLEN(haystack);
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[2], রিক্ত);
	যদি (end_idx < 0)
	    ঘূৰাই;	// can never find a match
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
	বাবে (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((ৱর্ণ *)rest, (ৱর্ণ *)needle);
	    যদি (rest == রিক্ত || rest > haystack + end_idx)
		ভঙ্গ;
	    lastmatch = rest;
	}
    }

    যদি (lastmatch == রিক্ত)
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
	ঘূৰাই;

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
	    ঘূৰাই;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_সংখ্যা_chk(&argvars[1], রিক্ত);
    যদি (str == রিক্ত || idx < 0)
	ঘূৰাই;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	যদি (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    পূর্ণাঙ্ক (*ptr2len)(char_u *);
    যদি (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    অন্যথা
	ptr2len = mb_ptr2len;

    char_u	*p;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		utf16idx = 0;
    বাবে (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	যদি (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    যদি (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_সংখ্যা = len;
	    ঘূৰাই;
	}
	utf16idx = len;
	পূর্ণাঙ্ক clen = ptr2len(p);
	পূর্ণাঙ্ক c = (clen > 1) ? utf_ptr2char(p) : *p;
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
	ঘূৰাই;

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
	ঘূৰাই;

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
    পূর্ণাঙ্ক		inlen;
    পূর্ণাঙ্ক		fromlen;
    পূর্ণাঙ্ক		tolen;
    পূর্ণাঙ্ক		idx;
    char_u	*cpstr;
    পূর্ণাঙ্ক		cplen;
    পূর্ণাঙ্ক		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;
    যদি (fromstr == রিক্ত || tostr == রিক্ত)
	    ঘূৰাই;		// type error; errmsg already given
    ga_init2(&ga, পরিমাণ(ৱর্ণ), 80);

    যদি (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	যদি (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    ঘূৰাই;
	}

    // fromstr and tostr have to contain the same number of chars
    যেতিয়ালৈ (*in_str != NUL)
    {
	যদি (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    বাবে (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		যদি (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    বাবে (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			যদি (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    ভঙ্গ;
			}
		    }
		    যদি (*p == NUL)	// tostr is shorter than fromstr
			গচ্ছ error;
		    ভঙ্গ;
		}
		++idx;
	    }

	    যদি (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		বাবে (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		যদি (idx != 0)
		    গচ্ছ error;
	    }

	    (শূন্য)ga_grow(&ga, cplen);
	    mch_memmove((ৱর্ণ *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	অন্যথা
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    যদি (p != রিক্ত)
		ga_append(&ga, tostr[p - fromstr]);
	    অন্যথা
		ga_append(&ga, *in_str);
	    ++in_str;
	}
    }

    // add a terminating NUL
    (শূন্য)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_data;
}

/*
 * "trim({expr})" function
 */
    শূন্য
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*head;
    char_u	*mask = রিক্ত;
    char_u	*tail;
    char_u	*prev;
    char_u	*p;
    পূর্ণাঙ্ক		c1;
    পূর্ণাঙ্ক		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL)))
	ঘূৰাই;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    যদি (head == রিক্ত)
	ঘূৰাই;

    যদি (check_for_opt_string_arg(argvars, 1) == FAIL)
	ঘূৰাই;

    যদি (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	যদি (*mask == NUL)
	    mask = রিক্ত;

	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    পূর্ণাঙ্ক	error = 0;

	    // leading or trailing characters to trim
	    dir = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[2], &error);
	    যদি (error)
		ঘূৰাই;
	    যদি (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		ঘূৰাই;
	    }
	}
    }

    যদি (dir == 0 || dir == 1)
    {
	// Trim leading characters
	যেতিয়ালৈ (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    যদি (mask == রিক্ত)
	    {
		যদি (c1 > ' ' && c1 != 0xa0)
		    ভঙ্গ;
	    }
	    অন্যথা
	    {
		বাবে (p = mask; *p != NUL; MB_PTR_ADV(p))
		    যদি (c1 == PTR2CHAR(p))
			ভঙ্গ;
		যদি (*p == NUL)
		    ভঙ্গ;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    যদি (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	বাবে (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    যদি (mask == রিক্ত)
	    {
		যদি (c1 > ' ' && c1 != 0xa0)
		    ভঙ্গ;
	    }
	    অন্যথা
	    {
		বাবে (p = mask; *p != NUL; MB_PTR_ADV(p))
		    যদি (c1 == PTR2CHAR(p))
			ভঙ্গ;
		যদি (*p == NUL)
		    ভঙ্গ;
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
    স্থির char_u *
uri_decode(char_u *str)
{
    যদি (str == রিক্ত)
	ঘূৰাই রিক্ত;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    যদি (!decoded)
	ঘূৰাই রিক্ত;

    char_u	*p = decoded;
    size_t	i = 0;

    যেতিয়ালৈ (i < len)
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
		পূর্ণাঙ্ক val = hexhex2nr(&str[i + 1]);
		যদি (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		অন্যথা
		{
		    // invalid hex digits following "%"
		    বাবে (পূর্ণাঙ্ক j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	অন্যথা
	    *p++ = str[i++];
    }

    *p = NUL;

    ঘূৰাই decoded;
}

/*
 * "uri_decode({str})" function
 */
    শূন্য
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    যদি (check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

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
    যদি (str == রিক্ত)
	ঘূৰাই রিক্ত;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    যদি (encoded == রিক্ত)
	ঘূৰাই রিক্ত;

    char_u *p = encoded;

    বাবে (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	যদি (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	অন্যথা
	{
	    sprintf((ৱর্ণ *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    ঘূৰাই encoded;
}

/*
 * "uri_encode({str})" function
 */
    শূন্য
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    যদি (check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

স্থির ৱর্ণ *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    স্থির varnumber_T
tv_nr(typval_T *tvs, পূর্ণাঙ্ক *idxp)
{
    পূর্ণাঙ্ক		idx = *idxp - 1;
    varnumber_T	n = 0;
    পূর্ণাঙ্ক		err = FALSE;

    যদি (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    অন্যথা
    {
	++*idxp;
	n = tv_get_সংখ্যা_chk(&tvs[idx], &err);
	যদি (err)
	    n = 0;
    }
    ঘূৰাই n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    স্থির ৱর্ণ *
tv_str(typval_T *tvs, পূর্ণাঙ্ক *idxp, char_u **tofree)
{
    পূর্ণাঙ্ক		    idx = *idxp - 1;
    ৱর্ণ	    *s = রিক্ত;
    স্থির char_u   numbuf[NUMBUFLEN];

    যদি (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    অন্যথা
    {
	++*idxp;
	যদি (tofree != রিক্ত)
	    s = (ৱর্ণ *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	অন্যথা
	    s = (ৱর্ণ *)tv_get_string_chk(&tvs[idx]);
    }
    ঘূৰাই s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    স্থির দ্ৱিগুণ
tv_float(typval_T *tvs, পূর্ণাঙ্ক *idxp)
{
    পূর্ণাঙ্ক		idx = *idxp - 1;
    দ্ৱিগুণ	f = 0;

    যদি (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    অন্যথা
    {
	++*idxp;
	যদি (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	অন্যথা যদি (tvs[idx].v_type == VAR_সংখ্যা)
	    f = (দ্ৱিগুণ)tvs[idx].vval.v_সংখ্যা;
	অন্যথা
	    emsg(_(e_expected_float_argument_for_printf));
    }
    ঘূৰাই f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    স্থির ধ্রুৱ ৱর্ণ *
infinity_str(পূর্ণাঙ্ক positive,
	     ৱর্ণ fmt_spec,
	     পূর্ণাঙ্ক force_sign,
	     পূর্ণাঙ্ক space_for_positive)
{
    স্থির ধ্রুৱ ৱর্ণ *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    পূর্ণাঙ্ক idx = positive * (1 + force_sign + force_sign * space_for_positive);

    যদি (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    ঘূৰাই table[idx];
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
    পূর্ণাঙ্ক
vim_snprintf_add(ৱর্ণ *str, size_t str_m, ধ্রুৱ ৱর্ণ *fmt, ...)
{
    va_list	ap;
    পূর্ণাঙ্ক		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    যদি (str_m <= len)
	space = 0;
    অন্যথা
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    ঘূৰাই str_l;
}

    পূর্ণাঙ্ক
vim_snprintf(ৱর্ণ *str, size_t str_m, ধ্রুৱ ৱর্ণ *fmt, ...)
{
    va_list	ap;
    পূর্ণাঙ্ক		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    ঘূৰাই str_l;
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
vim_snprintf_safelen(ৱর্ণ *str, size_t str_m, ধ্রুৱ ৱর্ণ *fmt, ...)
{
    va_list ap;
    পূর্ণাঙ্ক	    str_l;

    যদি (str_m == 0)
	ঘূৰাই 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, রিক্ত);
    va_end(ap);

    যদি (str_l < 0)
    {
	*str = NUL;
	ঘূৰাই 0;
    }
    ঘূৰাই ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    পূর্ণাঙ্ক
vim_vsnprintf(
    ৱর্ণ	*str,
    size_t	str_m,
    ধ্রুৱ ৱর্ণ	*fmt,
    va_list	ap)
{
    ঘূৰাই vim_vsnprintf_typval(str, str_m, fmt, ap, রিক্ত);
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
    স্থির পূর্ণাঙ্ক
format_typeof(
    ধ্রুৱ ৱর্ণ	*type)
{
    // allowed values: \0, h, l, L
    ৱর্ণ    length_modifier = '\0';

    // current conversion specifier character
    ৱর্ণ    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    যদি (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	যদি (length_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    length_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    ৱিকল্প (fmt_spec)
    {
	পক্ষ 'i': fmt_spec = 'd'; ভঙ্গ;
	পক্ষ '*': fmt_spec = 'd'; length_modifier = 'h'; ভঙ্গ;
	পক্ষ 'D': fmt_spec = 'd'; length_modifier = 'l'; ভঙ্গ;
	পক্ষ 'U': fmt_spec = 'u'; length_modifier = 'l'; ভঙ্গ;
	পক্ষ 'O': fmt_spec = 'o'; length_modifier = 'l'; ভঙ্গ;
	স্ৱভাৱ: ভঙ্গ;
    }

    // get parameter value, do initial processing
    ৱিকল্প (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    পক্ষ '%':
	ঘূৰাই TYPE_PERCENT;

    পক্ষ 'c':
	ঘূৰাই TYPE_CHAR;

    পক্ষ 's':
    পক্ষ 'S':
	ঘূৰাই TYPE_STRING;

    পক্ষ 'd': পক্ষ 'u':
    পক্ষ 'b': পক্ষ 'B':
    পক্ষ 'o':
    পক্ষ 'x': পক্ষ 'X':
    পক্ষ 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //মান

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    যদি (fmt_spec == 'p')
		ঘূৰাই TYPE_POINTER;
	    অন্যথা যদি (fmt_spec == 'b' || fmt_spec == 'B')
		ঘূৰাই TYPE_UNSIGNEDLONGLONGINT;
	    অন্যথা যদি (fmt_spec == 'd')
	    {
		// signed
		ৱিকল্প (length_modifier)
		{
		পক্ষ '\0':
		পক্ষ 'h':
		    // char and short arguments are passed as int.
		    ঘূৰাই TYPE_INT;
		পক্ষ 'l':
		    ঘূৰাই TYPE_LONGINT;
		পক্ষ 'L':
		    ঘূৰাই TYPE_LONGLONGINT;
		}
	    }
	    অন্যথা
	    {
		// unsigned
		ৱিকল্প (length_modifier)
		{
		    পক্ষ '\0':
		    পক্ষ 'h':
			ঘূৰাই TYPE_UNSIGNEDINT;
		    পক্ষ 'l':
			ঘূৰাই TYPE_UNSIGNEDLONGINT;
		    পক্ষ 'L':
			ঘূৰাই TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	ভঙ্গ;

    পক্ষ 'f':
    পক্ষ 'F':
    পক্ষ 'e':
    পক্ষ 'E':
    পক্ষ 'g':
    পক্ষ 'G':
	ঘূৰাই TYPE_FLOAT;
    }

    ঘূৰাই TYPE_UNKNOWN;
}

    স্থির ৱর্ণ *
format_typename(
    ধ্রুৱ ৱর্ণ  *type)
{
    ৱিকল্প (format_typeof(type))
    {
	পক্ষ TYPE_INT:
	    ঘূৰাই typename_int;

	পক্ষ TYPE_LONGINT:
	    ঘূৰাই typename_longint;

	পক্ষ TYPE_LONGLONGINT:
	    ঘূৰাই typename_longlongint;

	পক্ষ TYPE_UNSIGNEDINT:
	    ঘূৰাই typename_unsignedint;

	পক্ষ TYPE_UNSIGNEDLONGINT:
	    ঘূৰাই typename_unsignedlongint;

	পক্ষ TYPE_UNSIGNEDLONGLONGINT:
	    ঘূৰাই typename_unsignedlonglongint;

	পক্ষ TYPE_POINTER:
	    ঘূৰাই _(typename_pointer);

	পক্ষ TYPE_PERCENT:
	    ঘূৰাই _(typename_percent);

	পক্ষ TYPE_CHAR:
	    ঘূৰাই typename_char;

	পক্ষ TYPE_STRING:
	    ঘূৰাই _(typename_string);

	পক্ষ TYPE_FLOAT:
	    ঘূৰাই typename_float;
    }

    ঘূৰাই _(typename_unknown);
}

    স্থির পূর্ণাঙ্ক
adjust_types(
    ধ্রুৱ ৱর্ণ ***ap_types,
    পূর্ণাঙ্ক arg,
    পূর্ণাঙ্ক *অঙ্ক_posarg,
    ধ্রুৱ ৱর্ণ *type)
{
    যদি (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	ঘূৰাই FAIL;
    }

    যদি (*ap_types == রিক্ত || *অঙ্ক_posarg < arg)
    {
	পূর্ণাঙ্ক	    idx;
	ধ্রুৱ ৱর্ণ  **new_types;

	যদি (*ap_types == রিক্ত)
	    new_types = ALLOC_CLEAR_MULT(ধ্রুৱ ৱর্ণ *, arg);
	অন্যথা
	    new_types = vim_realloc((ৱর্ণ **)*ap_types,
						arg * পরিমাণ(ধ্রুৱ ৱর্ণ *));

	যদি (new_types == রিক্ত)
	    ঘূৰাই FAIL;

	বাবে (idx = *অঙ্ক_posarg; idx < arg; ++idx)
	    new_types[idx] = রিক্ত;

	*ap_types = new_types;
	*অঙ্ক_posarg = arg;
    }

    যদি ((*ap_types)[arg - 1] != রিক্ত)
    {
	যদি ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    ধ্রুৱ ৱর্ণ *pt = type;
	    যদি (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    যদি (pt[0] != '*')
	    {
		ৱিকল্প (pt[0])
		{
		    পক্ষ 'd':
		    পক্ষ 'i': ভঙ্গ;
		    স্ৱভাৱ:
			semsg(_(e_positional_অঙ্ক_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			ঘূৰাই FAIL;
		}
	    }
	}
	অন্যথা
	{
	    যদি (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_অঙ্ক_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		ঘূৰাই FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    ঘূৰাই OK;
}

    স্থির শূন্য
format_overflow_error(ধ্রুৱ ৱর্ণ *pstart)
{
    size_t	arglen = 0;
    ৱর্ণ	*argcopy = রিক্ত;
    ধ্রুৱ ৱর্ণ	*p = pstart;

    যেতিয়ালৈ (VIM_ISDIGIT((পূর্ণাঙ্ক)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(ৱর্ণ, arglen + 1);
    যদি (argcopy != রিক্ত)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	মুক্ত(argcopy);
    }
    অন্যথা
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    স্থির পূর্ণাঙ্ক
get_unsigned_int(
    ধ্রুৱ ৱর্ণ *pstart,
    ধ্রুৱ ৱর্ণ **p,
    unsigned পূর্ণাঙ্ক *uj,
    পূর্ণাঙ্ক overflow_err)
{
    *uj = **p - '0';
    ++*p;

    যেতিয়ালৈ (VIM_ISDIGIT((পূর্ণাঙ্ক)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned পূর্ণাঙ্ক)(**p - '0');
	++*p;
    }

    যদি (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	যদি (overflow_err)
	{
	    format_overflow_error(pstart);
	    ঘূৰাই FAIL;
	}
	অন্যথা
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    ঘূৰাই OK;
}


    স্থির পূর্ণাঙ্ক
parse_fmt_types(
    ধ্রুৱ ৱর্ণ  ***ap_types,
    পূর্ণাঙ্ক		*অঙ্ক_posarg,
    ধ্রুৱ ৱর্ণ  *fmt,
    typval_T	*tvs UNUSED
    )
{
    ধ্রুৱ ৱর্ণ	*p = fmt;
    ধ্রুৱ ৱর্ণ	*arg = রিক্ত;

    পূর্ণাঙ্ক		any_pos = 0;
    পূর্ণাঙ্ক		any_arg = 0;
    পূর্ণাঙ্ক		arg_idx;

# define CHECK_POS_ARG কৰা { \
    যদি (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	গচ্ছ error; \
    } \
} যেতিয়ালৈ (0);

    যদি (p == রিক্ত)
	ঘূৰাই OK;

    যেতিয়ালৈ (*p != NUL)
    {
	যদি (*p != '%')
	{
	    ধ্রুৱ ৱর্ণ    *q = strchr(p + 1, '%');
	    size_t  n = (q == রিক্ত) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	অন্যথা
	{
	    // allowed values: \0, h, l, L
	    ৱর্ণ	length_modifier = '\0';

	    // variable for positional arg
	    পূর্ণাঙ্ক		pos_arg = -1;
	    ধ্রুৱ ৱর্ণ	*ptype = রিক্ত;
	    ধ্রুৱ ৱর্ণ	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    যেতিয়ালৈ (VIM_ISDIGIT(*ptype))
		++ptype;

	    যদি (*ptype == '$')
	    {
		যদি (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    গচ্ছ error;
		}

		// Positional argument
		unsigned পূর্ণাঙ্ক uj;

		যদি (get_unsigned_int(pstart, &p, &uj, tvs != রিক্ত) == FAIL)
		    গচ্ছ error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    যেতিয়ালৈ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		ৱিকল্প (*p)
		{
		    পক্ষ '0': ভঙ্গ;
		    পক্ষ '-': ভঙ্গ;
		    পক্ষ '+': ভঙ্গ;
		    পক্ষ ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      ভঙ্গ;
		    পক্ষ '#': ভঙ্গ;
		    পক্ষ '\'': ভঙ্গ;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    যদি (*(arg = p) == '*')
	    {
		p++;

		যদি (VIM_ISDIGIT((পূর্ণাঙ্ক)(*p)))
		{
		    // Positional argument field width
		    unsigned পূর্ণাঙ্ক uj;

		    যদি (get_unsigned_int(arg + 1, &p, &uj, tvs != রিক্ত) == FAIL)
			গচ্ছ error;

		    যদি (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			গচ্ছ error;
		    }
		    অন্যথা
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			যদি (adjust_types(ap_types, uj, অঙ্ক_posarg, arg) == FAIL)
			    গচ্ছ error;
		    }
		}
		অন্যথা
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    অন্যথা যদি (VIM_ISDIGIT((পূর্ণাঙ্ক)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ধ্রুৱ ৱর্ণ *digstart = p;
		unsigned পূর্ণাঙ্ক uj;

		যদি (get_unsigned_int(digstart, &p, &uj, tvs != রিক্ত) == FAIL)
		    গচ্ছ error;

		যদি (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    গচ্ছ error;
		}
	    }

	    // parse precision
	    যদি (*p == '.')
	    {
		p++;

		যদি (*(arg = p) == '*')
		{
		    p++;

		    যদি (VIM_ISDIGIT((পূর্ণাঙ্ক)(*p)))
		    {
			// Parse precision
			unsigned পূর্ণাঙ্ক uj;

			যদি (get_unsigned_int(arg + 1, &p, &uj, tvs != রিক্ত) == FAIL)
			    গচ্ছ error;

			যদি (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    যদি (adjust_types(ap_types, uj, অঙ্ক_posarg, arg) == FAIL)
				গচ্ছ error;
			}
			অন্যথা
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    গচ্ছ error;
			}
		    }
		    অন্যথা
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		অন্যথা যদি (VIM_ISDIGIT((পূর্ণাঙ্ক)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ধ্রুৱ ৱর্ণ *digstart = p;
		    unsigned পূর্ণাঙ্ক uj;

		    যদি (get_unsigned_int(digstart, &p, &uj, tvs != রিক্ত) == FAIL)
			গচ্ছ error;

		    যদি (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			গচ্ছ error;
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
		length_modifier = *p;
		p++;
		যদি (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    ৱিকল্প (*p)
	    {
		// Check for known format specifiers. % is special!
		পক্ষ 'i':
		পক্ষ '*':
		পক্ষ 'd':
		পক্ষ 'u':
		পক্ষ 'o':
		পক্ষ 'D':
		পক্ষ 'U':
		পক্ষ 'O':
		পক্ষ 'x':
		পক্ষ 'X':
		পক্ষ 'b':
		পক্ষ 'B':
		পক্ষ 'c':
		পক্ষ 's':
		পক্ষ 'S':
		পক্ষ 'p':
		পক্ষ 'f':
		পক্ষ 'F':
		পক্ষ 'e':
		পক্ষ 'E':
		পক্ষ 'g':
		পক্ষ 'G':
		    যদি (pos_arg != -1)
		    {
			যদি (adjust_types(ap_types, pos_arg, অঙ্ক_posarg, ptype) == FAIL)
			    গচ্ছ error;
		    }
		    অন্যথা
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    ভঙ্গ;

		স্ৱভাৱ:
		    যদি (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			গচ্ছ error;
		    }
	    }

	    যদি (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    বাবে (arg_idx = 0; arg_idx < *অঙ্ক_posarg; ++arg_idx)
    {
	যদি ((*ap_types)[arg_idx] == রিক্ত)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    গচ্ছ error;
	}

# যদি defined(FEAT_EVAL)
	যদি (tvs != রিক্ত && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    গচ্ছ error;
	}
# endif
    }

    ঘূৰাই OK;

error:
    vim_free((ৱর্ণ**)*ap_types);
    *ap_types = রিক্ত;
    *অঙ্ক_posarg = 0;
    ঘূৰাই FAIL;
}

    স্থির শূন্য
skip_to_arg(
    ধ্রুৱ ৱর্ণ	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    পূর্ণাঙ্ক		*arg_idx,
    পূর্ণাঙ্ক		*arg_cur,
    ধ্রুৱ ৱর্ণ	*fmt)
{
    পূর্ণাঙ্ক		arg_min = 0;

    যদি (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	ঘূৰাই;
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

    বাবে (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	ধ্রুৱ ৱর্ণ *p;

	যদি (ap_types == রিক্ত || ap_types[*arg_cur] == রিক্ত)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    ঘূৰাই;
	}

	p = ap_types[*arg_cur];

	পূর্ণাঙ্ক fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	ৱিকল্প (fmt_type)
	{
	পক্ষ TYPE_PERCENT:
	পক্ষ TYPE_UNKNOWN:
	    ভঙ্গ;

	পক্ষ TYPE_CHAR:
	    va_arg(*ap, পূর্ণাঙ্ক);
	    ভঙ্গ;

	পক্ষ TYPE_STRING:
	    va_arg(*ap, ৱর্ণ *);
	    ভঙ্গ;

	পক্ষ TYPE_POINTER:
	    va_arg(*ap, শূন্য *);
	    ভঙ্গ;

	পক্ষ TYPE_INT:
	    va_arg(*ap, পূর্ণাঙ্ক);
	    ভঙ্গ;

	পক্ষ TYPE_LONGINT:
	    va_arg(*ap, long পূর্ণাঙ্ক);
	    ভঙ্গ;

	পক্ষ TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    ভঙ্গ;

	পক্ষ TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned পূর্ণাঙ্ক);
	    ভঙ্গ;

	পক্ষ TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long পূর্ণাঙ্ক);
	    ভঙ্গ;

	পক্ষ TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    ভঙ্গ;

	পক্ষ TYPE_FLOAT:
	    va_arg(*ap, দ্ৱিগুণ);
	    ভঙ্গ;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    ঘূৰাই;
}

    পূর্ণাঙ্ক
vim_vsnprintf_typval(
    ৱর্ণ	*str,
    size_t	str_m,
    ধ্রুৱ ৱর্ণ	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    ধ্রুৱ ৱর্ণ	*p = fmt;
    পূর্ণাঙ্ক		arg_cur = 0;
    পূর্ণাঙ্ক		অঙ্ক_posarg = 0;
    পূর্ণাঙ্ক		arg_idx = 1;
    va_list	ap;
    ধ্রুৱ ৱর্ণ	**ap_types = রিক্ত;

    যদি (parse_fmt_types(&ap_types, &অঙ্ক_posarg, fmt, tvs) == FAIL)
	ঘূৰাই 0;

    va_copy(ap, ap_start);

    যদি (p == রিক্ত)
	p = "";
    যেতিয়ালৈ (*p != NUL)
    {
	যদি (*p != '%')
	{
	    ধ্রুৱ ৱর্ণ    *q = strchr(p + 1, '%');
	    size_t  n = (q == রিক্ত) ? STRLEN(p) : (size_t)(q - p);

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
	    পূর্ণাঙ্ক	    zero_padding = 0, precision_specified = 0, justify_বাম = 0;
	    পূর্ণাঙ্ক	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    পূর্ণাঙ্ক	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    ৱর্ণ    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    ৱর্ণ    tmp[TMP_LEN];

	    // string address in case of string argument
	    ধ্রুৱ ৱর্ণ  *str_arg = রিক্ত;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    unsigned ৱর্ণ uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  সংখ্যা_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    ৱর্ণ    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = রিক্ত;

	    // variables for positional arg
	    পূর্ণাঙ্ক	    pos_arg = -1;
	    ধ্রুৱ ৱর্ণ	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    যেতিয়ালৈ (VIM_ISDIGIT(*ptype))
		++ptype;

	    যদি (*ptype == '$')
	    {
		// Positional argument
		ধ্রুৱ ৱর্ণ *digstart = p;
		unsigned পূর্ণাঙ্ক uj;

		যদি (get_unsigned_int(digstart, &p, &uj, tvs != রিক্ত) == FAIL)
		    গচ্ছ error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    যেতিয়ালৈ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		ৱিকল্প (*p)
		{
		    পক্ষ '0': zero_padding = 1; ভঙ্গ;
		    পক্ষ '-': justify_বাম = 1; ভঙ্গ;
		    পক্ষ '+': force_sign = 1; space_for_positive = 0; ভঙ্গ;
		    পক্ষ ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      ভঙ্গ;
		    পক্ষ '#': alternate_form = 1; ভঙ্গ;
		    পক্ষ '\'': ভঙ্গ;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    যদি (*p == '*')
	    {
		পূর্ণাঙ্ক j;
		ধ্রুৱ ৱর্ণ *digstart = p + 1;

		p++;

		যদি (VIM_ISDIGIT((পূর্ণাঙ্ক)(*p)))
		{
		    // Positional argument field width
		    unsigned পূর্ণাঙ্ক uj;

		    যদি (get_unsigned_int(digstart, &p, &uj, tvs != রিক্ত) == FAIL)
			গচ্ছ error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# যদি defined(FEAT_EVAL)
		    tvs != রিক্ত ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, পূর্ণাঙ্ক));

		যদি (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    যদি (tvs != রিক্ত)
		    {
			format_overflow_error(digstart);
			গচ্ছ error;
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
	    অন্যথা যদি (VIM_ISDIGIT((পূর্ণাঙ্ক)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ধ্রুৱ ৱর্ণ *digstart = p;
		unsigned পূর্ণাঙ্ক uj;

		যদি (get_unsigned_int(digstart, &p, &uj, tvs != রিক্ত) == FAIL)
		    গচ্ছ error;

		min_field_width = uj;
	    }

	    // parse precision
	    যদি (*p == '.')
	    {
		p++;
		precision_specified = 1;

		যদি (VIM_ISDIGIT((পূর্ণাঙ্ক)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ধ্রুৱ ৱর্ণ *digstart = p;
		    unsigned পূর্ণাঙ্ক uj;

		    যদি (get_unsigned_int(digstart, &p, &uj, tvs != রিক্ত) == FAIL)
			গচ্ছ error;

		    precision = uj;
		}
		অন্যথা যদি (*p == '*')
		{
		    পূর্ণাঙ্ক j;
		    ধ্রুৱ ৱর্ণ *digstart = p;

		    p++;

		    যদি (VIM_ISDIGIT((পূর্ণাঙ্ক)(*p)))
		    {
			// positional argument
			unsigned পূর্ণাঙ্ক uj;

			যদি (get_unsigned_int(digstart, &p, &uj, tvs != রিক্ত) == FAIL)
			    গচ্ছ error;

			arg_idx = uj;

			++p;
		    }

		    j =
# যদি defined(FEAT_EVAL)
			tvs != রিক্ত ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, পূর্ণাঙ্ক));

		    যদি (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			যদি (tvs != রিক্ত)
			{
			    format_overflow_error(digstart);
			    গচ্ছ error;
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
		length_modifier = *p;
		p++;
		যদি (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    length_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    ৱিকল্প (fmt_spec)
	    {
		পক্ষ 'i': fmt_spec = 'd'; ভঙ্গ;
		পক্ষ 'D': fmt_spec = 'd'; length_modifier = 'l'; ভঙ্গ;
		পক্ষ 'U': fmt_spec = 'u'; length_modifier = 'l'; ভঙ্গ;
		পক্ষ 'O': fmt_spec = 'o'; length_modifier = 'l'; ভঙ্গ;
		স্ৱভাৱ: ভঙ্গ;
	    }

# যদি defined(FEAT_EVAL)
	    ৱিকল্প (fmt_spec)
	    {
		পক্ষ 'd': পক্ষ 'u': পক্ষ 'o': পক্ষ 'x': পক্ষ 'X':
		    যদি (tvs != রিক্ত && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    যদি (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    ৱিকল্প (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    পক্ষ '%':
	    পক্ষ 'c':
	    পক্ষ 's':
	    পক্ষ 'S':
		str_arg_l = 1;
		ৱিকল্প (fmt_spec)
		{
		পক্ষ '%':
		    str_arg = p;
		    ভঙ্গ;

		পক্ষ 'c':
		    {
			পূর্ণাঙ্ক j;

			j =
# যদি defined(FEAT_EVAL)
			    tvs != রিক্ত ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, পূর্ণাঙ্ক));

			// standard demands unsigned char
			uchar_arg = (unsigned ৱর্ণ)j;
			str_arg = (ৱর্ণ *)&uchar_arg;
			ভঙ্গ;
		    }

		পক্ষ 's':
		পক্ষ 'S':
		    str_arg =
# যদি defined(FEAT_EVAL)
				tvs != রিক্ত ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, ৱর্ণ *));

		    যদি (str_arg == রিক্ত)
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
			ধ্রুৱ ৱর্ণ *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == রিক্ত) ? precision
						      : (size_t)(q - str_arg);
		    }
		    যদি (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			পূর্ণাঙ্ক	cell;

			বাবে (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    যদি (precision_specified && i + cell > precision)
				ভঙ্গ;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			যদি (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    ভঙ্গ;

		স্ৱভাৱ:
		    ভঙ্গ;
		}
		ভঙ্গ;

	    পক্ষ 'd': পক্ষ 'u':
	    পক্ষ 'b': পক্ষ 'B':
	    পক্ষ 'o':
	    পক্ষ 'x': পক্ষ 'X':
	    পক্ষ 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //মান

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    পূর্ণাঙ্ক arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    পূর্ণাঙ্ক int_arg = 0;
		    unsigned পূর্ণাঙ্ক uint_arg = 0;

		    // only set for length modifier l
		    long পূর্ণাঙ্ক long_arg = 0;
		    unsigned long পূর্ণাঙ্ক ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    শূন্য *ptr_arg = রিক্ত;

		    যদি (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# যদি defined(FEAT_EVAL)
				 tvs != রিক্ত ? (শূন্য *)tv_str(tvs, &arg_idx,
									রিক্ত) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, শূন্য *));

			যদি (ptr_arg != রিক্ত)
			    arg_sign = 1;
		    }
		    অন্যথা যদি (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# যদি defined(FEAT_EVAL)
				    tvs != রিক্ত ?
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
			ৱিকল্প (length_modifier)
			{
			পক্ষ '\0':
			পক্ষ 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# যদি defined(FEAT_EVAL)
					tvs != রিক্ত ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, পূর্ণাঙ্ক));

			    যদি (int_arg > 0)
				arg_sign =  1;
			    অন্যথা যদি (int_arg < 0)
				arg_sign = -1;
			    ভঙ্গ;
			পক্ষ 'l':
			    long_arg =
# যদি defined(FEAT_EVAL)
					tvs != রিক্ত ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long পূর্ণাঙ্ক));

			    যদি (long_arg > 0)
				arg_sign =  1;
			    অন্যথা যদি (long_arg < 0)
				arg_sign = -1;
			    ভঙ্গ;
			পক্ষ 'L':
			    llong_arg =
# যদি defined(FEAT_EVAL)
					tvs != রিক্ত ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    যদি (llong_arg > 0)
				arg_sign =  1;
			    অন্যথা যদি (llong_arg < 0)
				arg_sign = -1;
			    ভঙ্গ;
			}
		    }
		    অন্যথা
		    {
			// unsigned
			ৱিকল্প (length_modifier)
			{
			    পক্ষ '\0':
			    পক্ষ 'h':
				uint_arg =
# যদি defined(FEAT_EVAL)
					    tvs != রিক্ত ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned পূর্ণাঙ্ক));

				যদি (uint_arg != 0)
				    arg_sign = 1;
				ভঙ্গ;
			    পক্ষ 'l':
				ulong_arg =
# যদি defined(FEAT_EVAL)
					    tvs != রিক্ত ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long পূর্ণাঙ্ক));

				যদি (ulong_arg != 0)
				    arg_sign = 1;
				ভঙ্গ;
			    পক্ষ 'L':
				ullong_arg =
# যদি defined(FEAT_EVAL)
					    tvs != রিক্ত ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				যদি (ullong_arg != 0)
				    arg_sign = 1;
				ভঙ্গ;
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
			ৱর্ণ	f[6];
			পূর্ণাঙ্ক	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			যদি (!length_modifier)
			    ;
			অন্যথা যদি (length_modifier == 'L')
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
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			যদি (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			অন্যথা যদি (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    ৱর্ণ	    b[8 * পরিমাণ(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    কৰা
			    {
				b[পরিমাণ(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    যেতিয়ালৈ (bn != 0);

			    memcpy(tmp + str_arg_l, b + পরিমাণ(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			অন্যথা যদি (fmt_spec == 'd')
			{
			    // signed
			    ৱিকল্প (length_modifier)
			    {
			    পক্ষ '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       ভঙ্গ;
			    পক্ষ 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (short)int_arg);
				      ভঙ্গ;
			    পক্ষ 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      ভঙ্গ;
			    পক্ষ 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      ভঙ্গ;
			    }
			}
			অন্যথা
			{
			    // unsigned
			    ৱিকল্প (length_modifier)
			    {
			    পক্ষ '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       ভঙ্গ;
			    পক্ষ 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(unsigned short)uint_arg);
				      ভঙ্গ;
			    পক্ষ 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      ভঙ্গ;
			    পক্ষ 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      ভঙ্গ;
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
			পূর্ণাঙ্ক n = (পূর্ণাঙ্ক)(min_field_width - (str_arg_l
						    + সংখ্যা_of_zeros_to_pad));
			যদি (n > 0)
			    সংখ্যা_of_zeros_to_pad += n;
		    }
		    ভঙ্গ;
		}

	    পক্ষ 'f':
	    পক্ষ 'F':
	    পক্ষ 'e':
	    পক্ষ 'E':
	    পক্ষ 'g':
	    পক্ষ 'G':
		{
		    // Floating point.
		    দ্ৱিগুণ	f;
		    দ্ৱিগুণ	abs_f;
		    ৱর্ণ	format[40];
		    পূর্ণাঙ্ক		l;
		    পূর্ণাঙ্ক		remove_trailing_zeroes = FALSE;

		    f =
# যদি defined(FEAT_EVAL)
			tvs != রিক্ত ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, দ্ৱিগুণ));

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
				l += sprintf(format + l, ".%d", (পূর্ণাঙ্ক)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			যদি (remove_trailing_zeroes)
			{
			    পূর্ণাঙ্ক i;
			    ৱর্ণ *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    যদি (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    অন্যথা
			    {
				tp = (ৱর্ণ *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				যদি (tp != রিক্ত)
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
				    যেতিয়ালৈ (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    যদি (tp != রিক্ত && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				যেতিয়ালৈ (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			অন্যথা
			{
			    ৱর্ণ *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (ৱর্ণ *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    যদি (tp != রিক্ত && (tp[1] == '+' || tp[1] == '-')
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
		    ভঙ্গ;
		}

	    স্ৱভাৱ:
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
		ভঙ্গ;
	    }

	    যদি (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    যদি (!justify_বাম)
	    {
		// left padding with blank or zero
		পূর্ণাঙ্ক pn = (পূর্ণাঙ্ক)(min_field_width - (str_arg_l + সংখ্যা_of_zeros_to_pad));

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
		পূর্ণাঙ্ক zn = (পূর্ণাঙ্ক)zero_padding_insertion_ind;

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
		zn = (পূর্ণাঙ্ক)সংখ্যা_of_zeros_to_pad;
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
		পূর্ণাঙ্ক sn = (পূর্ণাঙ্ক)(str_arg_l - zero_padding_insertion_ind);

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
		পূর্ণাঙ্ক pn = (পূর্ণাঙ্ক)(min_field_width
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

    যদি (tvs != রিক্ত && tvs[অঙ্ক_posarg != 0 ? অঙ্ক_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((ৱর্ণ*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    ঘূৰাই (পূর্ণাঙ্ক)str_l;
}

#endif // PROTO
