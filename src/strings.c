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
#അന്തര്ഭാവ "vim.h"

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
    എങ്കിൽ (p != ശൂന്യ)
	mch_memmove(p, string, len);
    തിരികെ p;
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
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;

    STRNCPY(p, string, len);
    p[len] = NUL;
    തിരികെ p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    തിരികെ vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    പൂർണ്ണസംഖ്യ		cc,
    പൂർണ്ണസംഖ്യ		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    പൂർണ്ണസംഖ്യ		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    വേണ്ടി (p = string; *p; p++)
    {
	എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    തുടരുക;
	}
	എങ്കിൽ (vim_strchr(esc_chars, *p) != ശൂന്യ || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    എങ്കിൽ (escaped_string == ശൂന്യ)
	തിരികെ ശൂന്യ;
    p2 = escaped_string;
    വേണ്ടി (p = string; *p; p++)
    {
	എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    തുടരുക;
	}
	എങ്കിൽ (vim_strchr(esc_chars, *p) != ശൂന്യ || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    തിരികെ escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    പൂർണ്ണസംഖ്യ
csh_like_shell(ശൂന്യം)
{
    തിരികെ (strstr((അക്ഷരം *)gettail(p_sh), "csh") != ശൂന്യ);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fish_like_shell(ശൂന്യം)
{
    തിരികെ (strstr((അക്ഷരം *)gettail(p_sh), "fish") != ശൂന്യ);
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
vim_strsave_shellescape(char_u *string, പൂർണ്ണസംഖ്യ do_special, പൂർണ്ണസംഖ്യ do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    പൂർണ്ണസംഖ്യ		csh_like;
    പൂർണ്ണസംഖ്യ		fish_like;
    char_u	*shname;
    പൂർണ്ണസംഖ്യ		powershell;
#ifdef MSWIN
    പൂർണ്ണസംഖ്യ		double_quotes;
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
    powershell = strstr((അക്ഷരം *)shname, "pwsh") != ശൂന്യ;
#ifdef MSWIN
    powershell = powershell || strstr((അക്ഷരം *)shname, "powershell") != ശൂന്യ;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (unsigned)STRLEN(string) + 3;  // two quotes and a trailing NUL
    വേണ്ടി (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	എങ്കിൽ (double_quotes)
	{
	    എങ്കിൽ (*p == '"')
		++length;		// " -> ""
	}
	അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (*p == '\'')
	{
	    എങ്കിൽ (powershell)
		length += 2;		// ' => ''
	    അല്ലെങ്കിൽ
		length += 3;		// ' => '\''
	}
	എങ്കിൽ ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    എങ്കിൽ (csh_like && do_special)
		++length;		// insert backslash
	}
	എങ്കിൽ (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	എങ്കിൽ (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    എങ്കിൽ (escaped_string != ശൂന്യ)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	എങ്കിൽ (double_quotes)
	    *d++ = '"';
	അല്ലെങ്കിൽ
#endif
	    *d++ = '\'';

	വേണ്ടി (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    എങ്കിൽ (double_quotes)
	    {
		എങ്കിൽ (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    തുടരുക;
		}
	    }
	    അല്ലെങ്കിൽ
#endif
	    എങ്കിൽ (*p == '\'')
	    {
		എങ്കിൽ (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		അല്ലെങ്കിൽ
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		തുടരുക;
	    }
	    എങ്കിൽ ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		എങ്കിൽ (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		തുടരുക;
	    }
	    എങ്കിൽ (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		തുടരുക;
	    }
	    എങ്കിൽ (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		തുടരുക;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	എങ്കിൽ (double_quotes)
	    *d++ = '"';
	അല്ലെങ്കിൽ
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    തിരികെ escaped_string;
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
    തിരികെ p1;
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
    തിരികെ p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    ശൂന്യം
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    പൂർണ്ണസംഖ്യ	    c;

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    p2 = p;
    ആകുമ്പോൾ ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    സ്ഥിതം char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    എങ്കിൽ (res != ശൂന്യ)
	ആകുമ്പോൾ (*p != NUL)
	{
	    പൂർണ്ണസംഖ്യ		l;

	    എങ്കിൽ (enc_utf8)
	    {
		പൂർണ്ണസംഖ്യ	c, uc;
		പൂർണ്ണസംഖ്യ	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		എങ്കിൽ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		എങ്കിൽ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    എങ്കിൽ (s == ശൂന്യ)
		    {
			vim_free(res);
			തിരികെ ശൂന്യ;
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
	    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    അല്ലെങ്കിൽ
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    തിരികെ res;
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

    എങ്കിൽ (res != ശൂന്യ)
	ആകുമ്പോൾ (*p != NUL)
	{
	    പൂർണ്ണസംഖ്യ		l;

	    എങ്കിൽ (enc_utf8)
	    {
		പൂർണ്ണസംഖ്യ	c, lc;
		പൂർണ്ണസംഖ്യ	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		എങ്കിൽ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		എങ്കിൽ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    എങ്കിൽ (s == ശൂന്യ)
		    {
			vim_free(res);
			തിരികെ ശൂന്യ;
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
	    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    അല്ലെങ്കിൽ
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    തിരികെ res;
}
#endif

/*
 * delete spaces at the end of a string
 */
    ശൂന്യം
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    ആകുമ്പോൾ (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    ശൂന്യം
vim_strncpy(char_u *to, char_u *from, size_t len)
{
    STRNCPY(to, from, len);
    to[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    ശൂന്യം
vim_strcat(char_u *to, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(to);
    size_t fromlen = STRLEN(from);

    എങ്കിൽ (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    അല്ലെങ്കിൽ
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(അക്ഷരം *s, size_t maxlen)
{
    size_t i;
    വേണ്ടി (i = 0; i < maxlen; ++i)
	എങ്കിൽ (s[i] == NUL)
	    നിർത്തുക;
    തിരികെ i;
}

#എങ്കിൽ !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    പൂർണ്ണസംഖ്യ
vim_stricmp(അക്ഷരം *s1, അക്ഷരം *s2)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (;;)
    {
	i = (പൂർണ്ണസംഖ്യ)TOLOWER_LOC(*s1) - (പൂർണ്ണസംഖ്യ)TOLOWER_LOC(*s2);
	എങ്കിൽ (i != 0)
	    തിരികെ i;			    // this character different
	എങ്കിൽ (*s1 == NUL)
	    നിർത്തുക;			    // strings match until NUL
	++s1;
	++s2;
    }
    തിരികെ 0;				    // strings match
}
#endif

#എങ്കിൽ !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    പൂർണ്ണസംഖ്യ
vim_strnicmp(അക്ഷരം *s1, അക്ഷരം *s2, size_t len)
{
    പൂർണ്ണസംഖ്യ		i;

    ആകുമ്പോൾ (len > 0)
    {
	i = (പൂർണ്ണസംഖ്യ)TOLOWER_LOC(*s1) - (പൂർണ്ണസംഖ്യ)TOLOWER_LOC(*s2);
	എങ്കിൽ (i != 0)
	    തിരികെ i;			    // this character different
	എങ്കിൽ (*s1 == NUL)
	    നിർത്തുക;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    തിരികെ 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    പൂർണ്ണസംഖ്യ
vim_strnicmp_asc(അക്ഷരം *s1, അക്ഷരം *s2, size_t len)
{
    പൂർണ്ണസംഖ്യ                i = 0;

    ആകുമ്പോൾ (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	എങ്കിൽ (i != 0)
	    നിർത്തുക;			// this character is different
	എങ്കിൽ (*s1 == NUL)
	    നിർത്തുക;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    തിരികെ i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, പൂർണ്ണസംഖ്യ c)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		b;

    p = string;
    എങ്കിൽ (enc_utf8 && c > 0 && c < 0x80)
	തിരികെ vim_strbyte(string, c);
    എങ്കിൽ (enc_utf8 && c >= 0x80)
    {
	ആകുമ്പോൾ (*p != NUL)
	{
	    പൂർണ്ണസംഖ്യ l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    എങ്കിൽ (utf_ptr2char(p) == c && l > 1)
		തിരികെ p;
	    p += l;
	}
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (enc_dbcs != 0 && c > 255)
    {
	പൂർണ്ണസംഖ്യ	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	ആകുമ്പോൾ ((b = *p) != NUL)
	{
	    എങ്കിൽ (b == c && p[1] == n2)
		തിരികെ p;
	    p += (*mb_ptr2len)(p);
	}
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (has_mbyte)
    {
	ആകുമ്പോൾ ((b = *p) != NUL)
	{
	    എങ്കിൽ (b == c)
		തിരികെ p;
	    p += (*mb_ptr2len)(p);
	}
	തിരികെ ശൂന്യ;
    }
    ആകുമ്പോൾ ((b = *p) != NUL)
    {
	എങ്കിൽ (b == c)
	    തിരികെ p;
	++p;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, പൂർണ്ണസംഖ്യ c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    എങ്കിൽ (c <= 0 || c > 255)
	തിരികെ ശൂന്യ;
    തിരികെ (char_u *)strchr((അക്ഷരം *)string, c);
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
vim_strrchr(char_u *string, പൂർണ്ണസംഖ്യ c)
{
    char_u	*retval = ശൂന്യ;
    char_u	*p = string;

    ആകുമ്പോൾ (*p)
    {
	എങ്കിൽ (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    തിരികെ retval;
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
    ആകുമ്പോൾ (*s)
    {
	എങ്കിൽ (vim_strchr(charset, *s) != ശൂന്യ)
	    തിരികെ s;
	MB_PTR_ADV(s);
    }
    തിരികെ ശൂന്യ;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ sort_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2);

    സ്ഥിതം പൂർണ്ണസംഖ്യ
sort_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    തിരികെ STRCMP(*(അക്ഷരം **)s1, *(അക്ഷരം **)s2);
}

    ശൂന്യം
sort_strings(
    char_u	**files,
    പൂർണ്ണസംഖ്യ		എണ്ണം)
{
    qsort((ശൂന്യം *)files, (size_t)എണ്ണം, വലുപ്പം(char_u *), sort_compare);
}

#എങ്കിൽ defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    പൂർണ്ണസംഖ്യ
has_non_ascii(char_u *s)
{
    char_u	*p;

    എങ്കിൽ (s != ശൂന്യ)
	വേണ്ടി (p = s; *p != NUL; ++p)
	    എങ്കിൽ (*p >= 128)
		തിരികെ TRUE;
    തിരികെ FALSE;
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
    size_t  l = str1 == ശൂന്യ ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == ശൂന്യ ? 0 : STRLEN(str2)) + 1L);
    എങ്കിൽ (dest == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (str1 == ശൂന്യ)
	*dest = NUL;
    അല്ലെങ്കിൽ
	STRCPY(dest, str1);
    എങ്കിൽ (str2 != ശൂന്യ)
	STRCPY(dest + l, str2);
    തിരികെ dest;
}

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    എങ്കിൽ (rev == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	എങ്കിൽ (has_mbyte)
	{
	    പൂർണ്ണസംഖ്യ mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	അല്ലെങ്കിൽ
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    തിരികെ rev;
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, പൂർണ്ണസംഖ്യ function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    എങ്കിൽ (str != ശൂന്യ)
    {
	len += (unsigned)STRLEN(str);
	വേണ്ടി (p = str; *p != NUL; MB_PTR_ADV(p))
	    എങ്കിൽ (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    എങ്കിൽ (r == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    അല്ലെങ്കിൽ
	*r++ = '\'';
    എങ്കിൽ (str != ശൂന്യ)
	വേണ്ടി (p = str; *p != NUL; )
	{
	    എങ്കിൽ (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    എങ്കിൽ (function)
	*r++ = ')';
    *r++ = NUL;
    തിരികെ s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_എണ്ണം(char_u *haystack, char_u *needle, പൂർണ്ണസംഖ്യ ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    എങ്കിൽ (p == ശൂന്യ || needle == ശൂന്യ || *needle == NUL)
	തിരികെ 0;

    size_t  needlelen = STRLEN(needle);
    എങ്കിൽ (ic)
    {
	ആകുമ്പോൾ (*p != NUL)
	{
	    എങ്കിൽ (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    അല്ലെങ്കിൽ
		MB_PTR_ADV(p);
	}
    }
    അല്ലെങ്കിൽ
	ആകുമ്പോൾ ((next = (char_u *)strstr((അക്ഷരം *)p, (അക്ഷരം *)needle)) != ശൂന്യ)
	{
	    ++n;
	    p = next + needlelen;
	}

    തിരികെ n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
copy_first_char_to_tv(char_u *നിവേശം, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (നിവേശം == ശൂന്യ || output == ശൂന്യ)
	തിരികെ -1;

    len = has_mbyte ? mb_ptr2len(നിവേശം) : 1;
    STRNCPY(buf, നിവേശം, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    തിരികെ output->vval.v_string == ശൂന്യ ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    ശൂന്യം
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*വ്യഞ്ജകം,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		idx = 0;
    പൂർണ്ണസംഖ്യ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_താക്കോൽ, VAR_സംഖ്യ);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_വ്യഞ്ജകം_get_funccal(വ്യഞ്ജകം, &newtv);

    ga_init2(&ga, വലുപ്പം(അക്ഷരം), 80);
    വേണ്ടി (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	എങ്കിൽ (len < 0)
	    നിർത്തുക;

	set_vim_var_nr(VV_താക്കോൽ, idx);
	എങ്കിൽ (filter_map_one(&tv, വ്യഞ്ജകം, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    നിർത്തുക;
	}
	എങ്കിൽ (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    എങ്കിൽ (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ
		ga_concat(&ga, newtv.vval.v_string);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    എങ്കിൽ (fc != ശൂന്യ)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    ശൂന്യം
string_reduce(
	typval_T	*argvars,
	typval_T	*വ്യഞ്ജകം,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    പൂർണ്ണസംഖ്യ		len;
    typval_T	argv[3];
    പൂർണ്ണസംഖ്യ		r;
    പൂർണ്ണസംഖ്യ		called_emsg_start = called_emsg;
    funccall_T	*fc;

    എങ്കിൽ (argvars[2].v_type == VAR_UNKNOWN)
    {
	എങ്കിൽ (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_മൂല്യം), "String");
	    തിരികെ;
	}
	len = copy_first_char_to_tv(p, rettv);
	എങ്കിൽ (len < 0)
	    തിരികെ;
	p += len;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (check_for_string_arg(argvars, 2) == FAIL)
	തിരികെ;
    അല്ലെങ്കിൽ
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_വ്യഞ്ജകം_get_funccal(വ്യഞ്ജകം, rettv);

    വേണ്ടി ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	എങ്കിൽ (len < 0)
	{
	    *rettv = argv[0];
	    നിർത്തുക;
	}

	r = eval_വ്യഞ്ജകം_typval(വ്യഞ്ജകം, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	എങ്കിൽ (r == FAIL || called_emsg != called_emsg_start)
	    നിർത്തുക;
    }

    എങ്കിൽ (fc != ശൂന്യ)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    സ്ഥിതം ശൂന്യം
byteidx_common(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ comp)
{
    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	തിരികെ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ);
    എങ്കിൽ (str == ശൂന്യ || idx < 0)
	തിരികെ;

    varnumber_T	utf16idx = FALSE;
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	പൂർണ്ണസംഖ്യ error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	എങ്കിൽ (error)
	    തിരികെ;
	എങ്കിൽ (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_സംഖ്യ_as_bool_nr), utf16idx);
	    തിരികെ;
	}
    }

    പൂർണ്ണസംഖ്യ (*ptr2len)(char_u *);
    എങ്കിൽ (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    അല്ലെങ്കിൽ
	ptr2len = mb_ptr2len;

    char_u *t = str;
    വേണ്ടി ( ; idx > 0; idx--)
    {
	എങ്കിൽ (*t == NUL)		// EOL reached
	    തിരികെ;
	എങ്കിൽ (utf16idx)
	{
	    പൂർണ്ണസംഖ്യ clen = ptr2len(t);
	    പൂർണ്ണസംഖ്യ c = (clen > 1) ? utf_ptr2char(t) : *t;
	    എങ്കിൽ (c > 0xFFFF)
		idx--;
	    എങ്കിൽ (idx > 0)
		t += clen;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_സംഖ്യ = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    ശൂന്യം
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    ശൂന്യം
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    ശൂന്യം
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	തിരികെ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ);
    എങ്കിൽ (str == ശൂന്യ || idx < 0)
	തിരികെ;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    പൂർണ്ണസംഖ്യ (*ptr2len)(char_u *);
    എങ്കിൽ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    അല്ലെങ്കിൽ
	ptr2len = mb_ptr2len;

    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;
    വേണ്ടി (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	എങ്കിൽ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    എങ്കിൽ (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_സംഖ്യ = len;
	    തിരികെ;
	}
	എങ്കിൽ (utf16idx)
	{
	    idx--;
	    പൂർണ്ണസംഖ്യ clen = ptr2len(p);
	    പൂർണ്ണസംഖ്യ c = (clen > 1) ? utf_ptr2char(p) : *p;
	    എങ്കിൽ (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_സംഖ്യ = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    എങ്കിൽ (convert_setup(&vimconv, from, to) == FAIL)
	തിരികെ FAIL;
    vimconv.vc_fail = TRUE;
    എങ്കിൽ (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	എങ്കിൽ (ret->string == ശൂന്യ)
	    ret->length = 0;
	അല്ലെങ്കിൽ
	    ret->length = str->length;
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ len = (പൂർണ്ണസംഖ്യ)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, ശൂന്യ, ശൂന്യ);

    തിരികെ (ret->string == ശൂന്യ) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    സ്ഥിതം ശൂന്യം
blob_from_string(char_u *str, blob_T *blob)
{
    പൂർണ്ണസംഖ്യ	    len = (പൂർണ്ണസംഖ്യ)STRLEN(str);
    char_u  *dest;

    എങ്കിൽ (len == 0)
	തിരികെ;
    എങ്കിൽ (ga_grow(&blob->bv_ga, len) == FAIL)
	തിരികെ;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < len; ++i)
	എങ്കിൽ (dest[i] == NL)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	വരി_len;

    എങ്കിൽ (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	തിരികെ (ret->string == ശൂന്യ) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    വരി_len = (nl == ശൂന്യ) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(വരി_len + 1);
    എങ്കിൽ (ret->string == ശൂന്യ)
    {
	ret->length = 0;
	തിരികെ FAIL;
    }
    എങ്കിൽ (വരി_len > 0)
	mch_memmove(ret->string, src, (size_t)വരി_len);
    ret->string[വരി_len] = NUL;
    ret->length = (size_t)വരി_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    വേണ്ടി (long i = 0; i < വരി_len; i++)
	എങ്കിൽ (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + വരി_len + (nl != ശൂന്യ ? 1 : 0);
    തിരികെ OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    സ്ഥിതം char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    എങ്കിൽ (from_encoding_raw == ശൂന്യ)
	തിരികെ ശൂന്യ;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    ആകുമ്പോൾ (*s != NUL)
    {
	എങ്കിൽ (*s == '_')
	    *pe++ = '-';
	അല്ലെങ്കിൽ
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    എങ്കിൽ ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    തിരികെ from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    സ്ഥിതം ശൂന്യം
append_converted_string_to_list(
	string_T *converted,
	പൂർണ്ണസംഖ്യ validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    എങ്കിൽ (converted->string != ശൂന്യ)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	ആകുമ്പോൾ (p < end)
	{
	    string_T	വരി;
	    char_u	*വരി_start = p;

	    ആകുമ്പോൾ (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    വരി.length = (size_t)(p - വരി_start);
	    വരി.string = vim_strnsave(വരി_start, വരി.length);
	    എങ്കിൽ (വരി.string != ശൂന്യ)
	    {
		എങ്കിൽ (validate_utf8 && !utf_valid_string(വരി.string, ശൂന്യ))
		{
		    vim_free(വരി.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    തിരികെ; // Stop processing
		}
		എങ്കിൽ (list_append_string(list, വരി.string, (പൂർണ്ണസംഖ്യ)വരി.length) == FAIL)
		{
		    vim_free(വരി.string);
		    vim_free(converted->string);
		    തിരികെ; // Stop processing on append failure
		}
		vim_free(വരി.string);
	    }
	    അല്ലെങ്കിൽ
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		തിരികെ;
	    }

	    എങ്കിൽ (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    അല്ലെങ്കിൽ
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
append_validated_വരി_to_list(string_T *വരി, പൂർണ്ണസംഖ്യ validate_utf8, list_T *list)
{
    എങ്കിൽ (validate_utf8 && !utf_valid_string(വരി->string, ശൂന്യ))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(വരി->string);
	തിരികെ FAIL;
    }

    പൂർണ്ണസംഖ്യ ret = list_append_string(list, വരി->string, (പൂർണ്ണസംഖ്യ)വരി->length);
    vim_free(വരി->string);
    തിരികെ ret;
}

    ശൂന്യം
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    പൂർണ്ണസംഖ്യ		blen;
    long	idx;
    പൂർണ്ണസംഖ്യ		validate_utf8 = FALSE;

    എങ്കിൽ (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    blob = argvars->vval.v_blob;
    എങ്കിൽ (blob == ശൂന്യ)
	തിരികെ;
    blen = blob_len(blob);

    char_u	*from_encoding = ശൂന്യ;
    char_u	*from_encoding_raw = ശൂന്യ;  // Encoding name with endianness preserved for iconv
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	എങ്കിൽ (d != ശൂന്യ)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    എങ്കിൽ (enc != ശൂന്യ)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		എങ്കിൽ (from_encoding_raw == ശൂന്യ)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    തിരികെ;
		}
	    }
	}
    }

    എങ്കിൽ (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    എങ്കിൽ (from_encoding != ശൂന്യ && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    പൂർണ്ണസംഖ്യ from_prop = 0;
    എങ്കിൽ (from_encoding != ശൂന്യ)
	from_prop = enc_canon_props(from_encoding);
    എങ്കിൽ (from_encoding != ശൂന്യ && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	പൂർണ്ണസംഖ്യ nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	എങ്കിൽ (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    എങ്കിൽ (blen > 0)
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
	എങ്കിൽ (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    പോകുക done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	പൂർണ്ണസംഖ്യ len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, ശൂന്യ);
	converted.length = len;
	convert_setup(&vimconv, ശൂന്യ, ശൂന്യ);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    അല്ലെങ്കിൽ
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	ആകുമ്പോൾ (idx < blen)
	{
	    string_T	str;

	    എങ്കിൽ (string_from_blob(blob, &idx, &str) != OK)
		നിർത്തുക;

	    എങ്കിൽ (from_encoding != ശൂന്യ)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		പൂർണ്ണസംഖ്യ	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		എങ്കിൽ (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    പോകുക done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    എങ്കിൽ (append_validated_വരി_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		പോകുക done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    എങ്കിൽ (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

done:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    ശൂന്യം
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (rettv_blob_alloc(rettv) == FAIL)
	തിരികെ;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ;

    char_u	*to_encoding = ശൂന്യ;
    char_u	*to_encoding_raw = ശൂന്യ;  // Encoding name with endianness preserved for iconv
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	എങ്കിൽ (d != ശൂന്യ)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    എങ്കിൽ (enc != ശൂന്യ)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		എങ്കിൽ (to_encoding_raw == ശൂന്യ)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    തിരികെ;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    പൂർണ്ണസംഖ്യ to_prop = 0;
    എങ്കിൽ (to_encoding != ശൂന്യ)
	to_prop = enc_canon_props(to_encoding);
    എങ്കിൽ (to_encoding != ശൂന്യ && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_ഇനങ്ങൾ(list, li)
	{
	    char_u *s;

	    എങ്കിൽ (li->li_tv.v_type != VAR_STRING)
		തുടരുക;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    എങ്കിൽ (li != list->lv_first)
		ga_append(&str_ga, NL);
	    എങ്കിൽ (s != ശൂന്യ && *s != NUL)
	    {
		പൂർണ്ണസംഖ്യ slen = (പൂർണ്ണസംഖ്യ)STRLEN(s);

		എങ്കിൽ (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    പോകുക done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	എങ്കിൽ (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    എങ്കിൽ (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		പോകുക done;
	    }
	    vimconv.vc_fail = TRUE;

	    പൂർണ്ണസംഖ്യ		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, ശൂന്യ);
	    convert_setup(&vimconv, ശൂന്യ, ശൂന്യ);
	    ga_clear(&str_ga);

	    എങ്കിൽ (converted == ശൂന്യ)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		പോകുക done;
	    }
	    എങ്കിൽ (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	അല്ലെങ്കിൽ
	    ga_clear(&str_ga);
    }
    അല്ലെങ്കിൽ
    {
	FOR_ALL_LIST_ഇനങ്ങൾ(list, li)
	{
	    എങ്കിൽ (li->li_tv.v_type != VAR_STRING)
		തുടരുക;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    എങ്കിൽ (str.string == ശൂന്യ)
		STR_LITERAL_SET(str, "");
	    അല്ലെങ്കിൽ
		str.length = STRLEN(str.string);

	    എങ്കിൽ (to_encoding != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		എങ്കിൽ (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    പോകുക done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    എങ്കിൽ (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    എങ്കിൽ (to_encoding != ശൂന്യ)
		vim_free(str.string);
	}
    }

done:
    എങ്കിൽ (to_encoding != ശൂന്യ)
	vim_free(to_encoding);
    എങ്കിൽ (to_encoding_raw != ശൂന്യ)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    ശൂന്യം
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		utf8 = FALSE;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[1], ശൂന്യ);

    p = tv_get_string(&argvars[0]);

    എങ്കിൽ (has_mbyte || utf8)
    {
	പൂർണ്ണസംഖ്യ (*ptr2len)(char_u *);
	പൂർണ്ണസംഖ്യ (*ptr2char)(char_u *);

	എങ്കിൽ (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	അല്ലെങ്കിൽ
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	വേണ്ടി ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_സംഖ്യ(rettv->vval.v_list, (*ptr2char)(p));
    }
    അല്ലെങ്കിൽ
	വേണ്ടി ( ; *p != NUL; ++p)
	    list_append_സംഖ്യ(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    ശൂന്യം
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		base = 10;
    char_u	*p;
    varnumber_T	n;
    പൂർണ്ണസംഖ്യ		what = 0;
    പൂർണ്ണസംഖ്യ		isneg;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[1]);
	എങ്കിൽ (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    തിരികെ;
	}
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    എങ്കിൽ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    തിരഞ്ഞെടുപ്പ് (base)
    {
	അവസ്ഥ 2: what |= STR2NR_BIN + STR2NR_FORCE; നിർത്തുക;
	അവസ്ഥ 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; നിർത്തുക;
	അവസ്ഥ 16: what |= STR2NR_HEX + STR2NR_FORCE; നിർത്തുക;
    }
    vim_str2nr(p, ശൂന്യ, ശൂന്യ, what, &n, ശൂന്യ, 0, FALSE, ശൂന്യ);
    // Text after the number is silently ignored.
    എങ്കിൽ (isneg)
	rettv->vval.v_സംഖ്യ = -n;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = n;

}

/*
 * "strgetchar()" function
 */
    ശൂന്യം
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    പൂർണ്ണസംഖ്യ		charidx;
    പൂർണ്ണസംഖ്യ		byteidx = 0;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    str = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ;
    len = (പൂർണ്ണസംഖ്യ)STRLEN(str);
    charidx = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[1], &error);
    എങ്കിൽ (error)
	തിരികെ;

    ആകുമ്പോൾ (charidx >= 0 && byteidx < len)
    {
	എങ്കിൽ (charidx == 0)
	{
	    rettv->vval.v_സംഖ്യ = mb_ptr2char(str + byteidx);
	    നിർത്തുക;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    ശൂന്യം
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    പൂർണ്ണസംഖ്യ		start_idx;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL))
	തിരികെ;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (needle == ശൂന്യ || haystack == ശൂന്യ)
	തിരികെ;		// type error; errmsg already given

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	പൂർണ്ണസംഖ്യ	    error = FALSE;

	start_idx = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[2], &error);
	എങ്കിൽ (error || start_idx >= (പൂർണ്ണസംഖ്യ)STRLEN(haystack))
	    തിരികെ;
	എങ്കിൽ (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((അക്ഷരം *)haystack, (അക്ഷരം *)needle);
    എങ്കിൽ (pos != ശൂന്യ)
	rettv->vval.v_സംഖ്യ = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    ശൂന്യം
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    എങ്കിൽ (rettv->vval.v_string != ശൂന്യ && tofree == ശൂന്യ)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    ശൂന്യം
f_strlen(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && check_for_string_or_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    സ്ഥിതം ശൂന്യം
strchar_common(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    പൂർണ്ണസംഖ്യ			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    ആകുമ്പോൾ (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_സംഖ്യ = len;
}

/*
 * "strcharlen()" function
 */
    ശൂന്യം
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && check_for_string_or_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    ശൂന്യം
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	പൂർണ്ണസംഖ്യ error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	എങ്കിൽ (error)
	    തിരികെ;
	എങ്കിൽ (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_സംഖ്യ_as_bool_nr), skipcc);
	    തിരികെ;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    ശൂന്യം
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	തിരികെ;

    varnumber_T countcc = FALSE;
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    പൂർണ്ണസംഖ്യ			(*func_mb_ptr2char_adv)(char_u **pp);
    പൂർണ്ണസംഖ്യ			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    ആകുമ്പോൾ (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	എങ്കിൽ (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_സംഖ്യ = len;
}

/*
 * "strdisplaywidth()" function
 */
    ശൂന്യം
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    പൂർണ്ണസംഖ്യ		col = 0;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    s = tv_get_string(&argvars[0]);
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	col = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[1]);

    rettv->vval.v_സംഖ്യ = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    ശൂന്യം
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_സംഖ്യ = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    ശൂന്യം
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		nchar;
    പൂർണ്ണസംഖ്യ		nbyte = 0;
    പൂർണ്ണസംഖ്യ		charlen;
    പൂർണ്ണസംഖ്യ		skipcc = FALSE;
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		slen;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	തിരികെ;

    p = tv_get_string(&argvars[0]);
    slen = (പൂർണ്ണസംഖ്യ)STRLEN(p);

    nchar = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[1], &error);
    എങ്കിൽ (!error)
    {
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    എങ്കിൽ (error)
		തിരികെ;
	    എങ്കിൽ (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_സംഖ്യ_as_bool_nr), skipcc);
		തിരികെ;
	    }
	}

	എങ്കിൽ (nchar > 0)
	    ആകുമ്പോൾ (nchar > 0 && nbyte < slen)
	    {
		എങ്കിൽ (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		അല്ലെങ്കിൽ
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	അല്ലെങ്കിൽ
	    nbyte = nchar;
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[2]);
	    ആകുമ്പോൾ (charlen > 0 && nbyte + len < slen)
	    {
		പൂർണ്ണസംഖ്യ off = nbyte + len;

		എങ്കിൽ (off < 0)
		    len += 1;
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (skipcc)
			len += mb_ptr2len(p + off);
		    അല്ലെങ്കിൽ
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	അല്ലെങ്കിൽ
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    എങ്കിൽ (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (nbyte > slen)
	nbyte = slen;
    എങ്കിൽ (len < 0)
	len = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    ശൂന്യം
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		slen;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	തിരികെ;

    p = tv_get_string(&argvars[0]);
    slen = (പൂർണ്ണസംഖ്യ)STRLEN(p);

    n = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[1], &error);
    എങ്കിൽ (error)
	len = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	len = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[2]);
    അല്ലെങ്കിൽ
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    എങ്കിൽ (n < 0)
    {
	len += n;
	n = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n > slen)
	n = slen;
    എങ്കിൽ (len < 0)
	len = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (n + len > slen)
	len = slen - n;

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	പൂർണ്ണസംഖ്യ off;

	// length in characters
	വേണ്ടി (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    ശൂന്യം
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		haystack_len, end_idx;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL))
	തിരികെ;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (needle == ശൂന്യ || haystack == ശൂന്യ)
	തിരികെ;		// type error; errmsg already given

    haystack_len = (പൂർണ്ണസംഖ്യ)STRLEN(haystack);
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[2], ശൂന്യ);
	എങ്കിൽ (end_idx < 0)
	    തിരികെ;	// can never find a match
    }
    അല്ലെങ്കിൽ
	end_idx = haystack_len;

    എങ്കിൽ (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((അക്ഷരം *)rest, (അക്ഷരം *)needle);
	    എങ്കിൽ (rest == ശൂന്യ || rest > haystack + end_idx)
		നിർത്തുക;
	    lastmatch = rest;
	}
    }

    എങ്കിൽ (lastmatch == ശൂന്യ)
	rettv->vval.v_സംഖ്യ = -1;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    ശൂന്യം
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    ശൂന്യം
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    തിരികെ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ);
    എങ്കിൽ (str == ശൂന്യ || idx < 0)
	തിരികെ;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    പൂർണ്ണസംഖ്യ (*ptr2len)(char_u *);
    എങ്കിൽ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    അല്ലെങ്കിൽ
	ptr2len = mb_ptr2len;

    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		utf16idx = 0;
    വേണ്ടി (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	എങ്കിൽ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    എങ്കിൽ (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_സംഖ്യ = len;
	    തിരികെ;
	}
	utf16idx = len;
	പൂർണ്ണസംഖ്യ clen = ptr2len(p);
	പൂർണ്ണസംഖ്യ c = (clen > 1) ? utf_ptr2char(p) : *p;
	എങ്കിൽ (c > 0xFFFF)
	    len++;
	p += clen;
	എങ്കിൽ (charidx)
	    idx--;
    }

    rettv->vval.v_സംഖ്യ = utf16idx;
}

/*
 * "tolower(string)" function
 */
    ശൂന്യം
f_tolower(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    ശൂന്യം
f_toupper(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    ശൂന്യം
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*in_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		inlen;
    പൂർണ്ണസംഖ്യ		fromlen;
    പൂർണ്ണസംഖ്യ		tolen;
    പൂർണ്ണസംഖ്യ		idx;
    char_u	*cpstr;
    പൂർണ്ണസംഖ്യ		cplen;
    പൂർണ്ണസംഖ്യ		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	തിരികെ;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;
    എങ്കിൽ (fromstr == ശൂന്യ || tostr == ശൂന്യ)
	    തിരികെ;		// type error; errmsg already given
    ga_init2(&ga, വലുപ്പം(അക്ഷരം), 80);

    എങ്കിൽ (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	എങ്കിൽ (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    തിരികെ;
	}

    // fromstr and tostr have to contain the same number of chars
    ആകുമ്പോൾ (*in_str != NUL)
    {
	എങ്കിൽ (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    വേണ്ടി (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		എങ്കിൽ (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    വേണ്ടി (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			എങ്കിൽ (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    നിർത്തുക;
			}
		    }
		    എങ്കിൽ (*p == NUL)	// tostr is shorter than fromstr
			പോകുക error;
		    നിർത്തുക;
		}
		++idx;
	    }

	    എങ്കിൽ (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		വേണ്ടി (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		എങ്കിൽ (idx != 0)
		    പോകുക error;
	    }

	    (ശൂന്യം)ga_grow(&ga, cplen);
	    mch_memmove((അക്ഷരം *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	അല്ലെങ്കിൽ
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    എങ്കിൽ (p != ശൂന്യ)
		ga_append(&ga, tostr[p - fromstr]);
	    അല്ലെങ്കിൽ
		ga_append(&ga, *in_str);
	    ++in_str;
	}
    }

    // add a terminating NUL
    (ശൂന്യം)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_data;
}

/*
 * "trim({expr})" function
 */
    ശൂന്യം
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*head;
    char_u	*mask = ശൂന്യ;
    char_u	*tail;
    char_u	*prev;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		c1;
    പൂർണ്ണസംഖ്യ		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL)))
	തിരികെ;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    എങ്കിൽ (head == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (check_for_opt_string_arg(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	എങ്കിൽ (*mask == NUL)
	    mask = ശൂന്യ;

	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    പൂർണ്ണസംഖ്യ	error = 0;

	    // leading or trailing characters to trim
	    dir = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[2], &error);
	    എങ്കിൽ (error)
		തിരികെ;
	    എങ്കിൽ (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		തിരികെ;
	    }
	}
    }

    എങ്കിൽ (dir == 0 || dir == 1)
    {
	// Trim leading characters
	ആകുമ്പോൾ (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    എങ്കിൽ (mask == ശൂന്യ)
	    {
		എങ്കിൽ (c1 > ' ' && c1 != 0xa0)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ
	    {
		വേണ്ടി (p = mask; *p != NUL; MB_PTR_ADV(p))
		    എങ്കിൽ (c1 == PTR2CHAR(p))
			നിർത്തുക;
		എങ്കിൽ (*p == NUL)
		    നിർത്തുക;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    എങ്കിൽ (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	വേണ്ടി (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    എങ്കിൽ (mask == ശൂന്യ)
	    {
		എങ്കിൽ (c1 > ' ' && c1 != 0xa0)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ
	    {
		വേണ്ടി (p = mask; *p != NUL; MB_PTR_ADV(p))
		    എങ്കിൽ (c1 == PTR2CHAR(p))
			നിർത്തുക;
		എങ്കിൽ (*p == NUL)
		    നിർത്തുക;
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
    സ്ഥിതം char_u *
uri_decode(char_u *str)
{
    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ ശൂന്യ;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    എങ്കിൽ (!decoded)
	തിരികെ ശൂന്യ;

    char_u	*p = decoded;
    size_t	i = 0;

    ആകുമ്പോൾ (i < len)
    {
	എങ്കിൽ (str[i] == '%')
	{
	    എങ്കിൽ (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		എങ്കിൽ (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    അല്ലെങ്കിൽ
	    {
		പൂർണ്ണസംഖ്യ val = hexhex2nr(&str[i + 1]);
		എങ്കിൽ (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		അല്ലെങ്കിൽ
		{
		    // invalid hex digits following "%"
		    വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	അല്ലെങ്കിൽ
	    *p++ = str[i++];
    }

    *p = NUL;

    തിരികെ decoded;
}

/*
 * "uri_decode({str})" function
 */
    ശൂന്യം
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

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
    സ്ഥിതം char_u *
uri_encode(char_u *str)
{
    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ ശൂന്യ;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    എങ്കിൽ (encoded == ശൂന്യ)
	തിരികെ ശൂന്യ;

    char_u *p = encoded;

    വേണ്ടി (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	എങ്കിൽ (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	അല്ലെങ്കിൽ
	{
	    sprintf((അക്ഷരം *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    തിരികെ encoded;
}

/*
 * "uri_encode({str})" function
 */
    ശൂന്യം
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

സ്ഥിതം അക്ഷരം *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    സ്ഥിതം varnumber_T
tv_nr(typval_T *tvs, പൂർണ്ണസംഖ്യ *idxp)
{
    പൂർണ്ണസംഖ്യ		idx = *idxp - 1;
    varnumber_T	n = 0;
    പൂർണ്ണസംഖ്യ		err = FALSE;

    എങ്കിൽ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    അല്ലെങ്കിൽ
    {
	++*idxp;
	n = tv_get_സംഖ്യ_chk(&tvs[idx], &err);
	എങ്കിൽ (err)
	    n = 0;
    }
    തിരികെ n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    സ്ഥിതം അക്ഷരം *
tv_str(typval_T *tvs, പൂർണ്ണസംഖ്യ *idxp, char_u **tofree)
{
    പൂർണ്ണസംഖ്യ		    idx = *idxp - 1;
    അക്ഷരം	    *s = ശൂന്യ;
    സ്ഥിതം char_u   numbuf[NUMBUFLEN];

    എങ്കിൽ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    അല്ലെങ്കിൽ
    {
	++*idxp;
	എങ്കിൽ (tofree != ശൂന്യ)
	    s = (അക്ഷരം *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	അല്ലെങ്കിൽ
	    s = (അക്ഷരം *)tv_get_string_chk(&tvs[idx]);
    }
    തിരികെ s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    സ്ഥിതം ഇരട്ട
tv_float(typval_T *tvs, പൂർണ്ണസംഖ്യ *idxp)
{
    പൂർണ്ണസംഖ്യ		idx = *idxp - 1;
    ഇരട്ട	f = 0;

    എങ്കിൽ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    അല്ലെങ്കിൽ
    {
	++*idxp;
	എങ്കിൽ (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	അല്ലെങ്കിൽ എങ്കിൽ (tvs[idx].v_type == VAR_സംഖ്യ)
	    f = (ഇരട്ട)tvs[idx].vval.v_സംഖ്യ;
	അല്ലെങ്കിൽ
	    emsg(_(e_expected_float_argument_for_printf));
    }
    തിരികെ f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    സ്ഥിതം സ്ഥിരം അക്ഷരം *
infinity_str(പൂർണ്ണസംഖ്യ positive,
	     അക്ഷരം fmt_spec,
	     പൂർണ്ണസംഖ്യ force_sign,
	     പൂർണ്ണസംഖ്യ space_for_positive)
{
    സ്ഥിതം സ്ഥിരം അക്ഷരം *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    പൂർണ്ണസംഖ്യ idx = positive * (1 + force_sign + force_sign * space_for_positive);

    എങ്കിൽ (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    തിരികെ table[idx];
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
    പൂർണ്ണസംഖ്യ
vim_snprintf_add(അക്ഷരം *str, size_t str_m, സ്ഥിരം അക്ഷരം *fmt, ...)
{
    va_list	ap;
    പൂർണ്ണസംഖ്യ		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    എങ്കിൽ (str_m <= len)
	space = 0;
    അല്ലെങ്കിൽ
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    തിരികെ str_l;
}

    പൂർണ്ണസംഖ്യ
vim_snprintf(അക്ഷരം *str, size_t str_m, സ്ഥിരം അക്ഷരം *fmt, ...)
{
    va_list	ap;
    പൂർണ്ണസംഖ്യ		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    തിരികെ str_l;
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
vim_snprintf_safelen(അക്ഷരം *str, size_t str_m, സ്ഥിരം അക്ഷരം *fmt, ...)
{
    va_list ap;
    പൂർണ്ണസംഖ്യ	    str_l;

    എങ്കിൽ (str_m == 0)
	തിരികെ 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, ശൂന്യ);
    va_end(ap);

    എങ്കിൽ (str_l < 0)
    {
	*str = NUL;
	തിരികെ 0;
    }
    തിരികെ ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    പൂർണ്ണസംഖ്യ
vim_vsnprintf(
    അക്ഷരം	*str,
    size_t	str_m,
    സ്ഥിരം അക്ഷരം	*fmt,
    va_list	ap)
{
    തിരികെ vim_vsnprintf_typval(str, str_m, fmt, ap, ശൂന്യ);
}

എണ്ണൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
format_typeof(
    സ്ഥിരം അക്ഷരം	*type)
{
    // allowed values: \0, h, l, L
    അക്ഷരം    length_modifier = '\0';

    // current conversion specifier character
    അക്ഷരം    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    എങ്കിൽ (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	എങ്കിൽ (length_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    length_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    തിരഞ്ഞെടുപ്പ് (fmt_spec)
    {
	അവസ്ഥ 'i': fmt_spec = 'd'; നിർത്തുക;
	അവസ്ഥ '*': fmt_spec = 'd'; length_modifier = 'h'; നിർത്തുക;
	അവസ്ഥ 'D': fmt_spec = 'd'; length_modifier = 'l'; നിർത്തുക;
	അവസ്ഥ 'U': fmt_spec = 'u'; length_modifier = 'l'; നിർത്തുക;
	അവസ്ഥ 'O': fmt_spec = 'o'; length_modifier = 'l'; നിർത്തുക;
	സ്ഥിരസ്ഥിതി: നിർത്തുക;
    }

    // get parameter value, do initial processing
    തിരഞ്ഞെടുപ്പ് (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    അവസ്ഥ '%':
	തിരികെ TYPE_PERCENT;

    അവസ്ഥ 'c':
	തിരികെ TYPE_CHAR;

    അവസ്ഥ 's':
    അവസ്ഥ 'S':
	തിരികെ TYPE_STRING;

    അവസ്ഥ 'd': അവസ്ഥ 'u':
    അവസ്ഥ 'b': അവസ്ഥ 'B':
    അവസ്ഥ 'o':
    അവസ്ഥ 'x': അവസ്ഥ 'X':
    അവസ്ഥ 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //മൂല്യം

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    എങ്കിൽ (fmt_spec == 'p')
		തിരികെ TYPE_POINTER;
	    അല്ലെങ്കിൽ എങ്കിൽ (fmt_spec == 'b' || fmt_spec == 'B')
		തിരികെ TYPE_UNSIGNEDLONGLONGINT;
	    അല്ലെങ്കിൽ എങ്കിൽ (fmt_spec == 'd')
	    {
		// signed
		തിരഞ്ഞെടുപ്പ് (length_modifier)
		{
		അവസ്ഥ '\0':
		അവസ്ഥ 'h':
		    // char and short arguments are passed as int.
		    തിരികെ TYPE_INT;
		അവസ്ഥ 'l':
		    തിരികെ TYPE_LONGINT;
		അവസ്ഥ 'L':
		    തിരികെ TYPE_LONGLONGINT;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// unsigned
		തിരഞ്ഞെടുപ്പ് (length_modifier)
		{
		    അവസ്ഥ '\0':
		    അവസ്ഥ 'h':
			തിരികെ TYPE_UNSIGNEDINT;
		    അവസ്ഥ 'l':
			തിരികെ TYPE_UNSIGNEDLONGINT;
		    അവസ്ഥ 'L':
			തിരികെ TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	നിർത്തുക;

    അവസ്ഥ 'f':
    അവസ്ഥ 'F':
    അവസ്ഥ 'e':
    അവസ്ഥ 'E':
    അവസ്ഥ 'g':
    അവസ്ഥ 'G':
	തിരികെ TYPE_FLOAT;
    }

    തിരികെ TYPE_UNKNOWN;
}

    സ്ഥിതം അക്ഷരം *
format_typename(
    സ്ഥിരം അക്ഷരം  *type)
{
    തിരഞ്ഞെടുപ്പ് (format_typeof(type))
    {
	അവസ്ഥ TYPE_INT:
	    തിരികെ typename_int;

	അവസ്ഥ TYPE_LONGINT:
	    തിരികെ typename_longint;

	അവസ്ഥ TYPE_LONGLONGINT:
	    തിരികെ typename_longlongint;

	അവസ്ഥ TYPE_UNSIGNEDINT:
	    തിരികെ typename_unsignedint;

	അവസ്ഥ TYPE_UNSIGNEDLONGINT:
	    തിരികെ typename_unsignedlongint;

	അവസ്ഥ TYPE_UNSIGNEDLONGLONGINT:
	    തിരികെ typename_unsignedlonglongint;

	അവസ്ഥ TYPE_POINTER:
	    തിരികെ _(typename_pointer);

	അവസ്ഥ TYPE_PERCENT:
	    തിരികെ _(typename_percent);

	അവസ്ഥ TYPE_CHAR:
	    തിരികെ typename_char;

	അവസ്ഥ TYPE_STRING:
	    തിരികെ _(typename_string);

	അവസ്ഥ TYPE_FLOAT:
	    തിരികെ typename_float;
    }

    തിരികെ _(typename_unknown);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
adjust_types(
    സ്ഥിരം അക്ഷരം ***ap_types,
    പൂർണ്ണസംഖ്യ arg,
    പൂർണ്ണസംഖ്യ *അക്കം_posarg,
    സ്ഥിരം അക്ഷരം *type)
{
    എങ്കിൽ (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	തിരികെ FAIL;
    }

    എങ്കിൽ (*ap_types == ശൂന്യ || *അക്കം_posarg < arg)
    {
	പൂർണ്ണസംഖ്യ	    idx;
	സ്ഥിരം അക്ഷരം  **new_types;

	എങ്കിൽ (*ap_types == ശൂന്യ)
	    new_types = ALLOC_CLEAR_MULT(സ്ഥിരം അക്ഷരം *, arg);
	അല്ലെങ്കിൽ
	    new_types = vim_realloc((അക്ഷരം **)*ap_types,
						arg * വലുപ്പം(സ്ഥിരം അക്ഷരം *));

	എങ്കിൽ (new_types == ശൂന്യ)
	    തിരികെ FAIL;

	വേണ്ടി (idx = *അക്കം_posarg; idx < arg; ++idx)
	    new_types[idx] = ശൂന്യ;

	*ap_types = new_types;
	*അക്കം_posarg = arg;
    }

    എങ്കിൽ ((*ap_types)[arg - 1] != ശൂന്യ)
    {
	എങ്കിൽ ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    സ്ഥിരം അക്ഷരം *pt = type;
	    എങ്കിൽ (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    എങ്കിൽ (pt[0] != '*')
	    {
		തിരഞ്ഞെടുപ്പ് (pt[0])
		{
		    അവസ്ഥ 'd':
		    അവസ്ഥ 'i': നിർത്തുക;
		    സ്ഥിരസ്ഥിതി:
			semsg(_(e_positional_അക്കം_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			തിരികെ FAIL;
		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_അക്കം_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		തിരികെ FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    തിരികെ OK;
}

    സ്ഥിതം ശൂന്യം
format_overflow_error(സ്ഥിരം അക്ഷരം *pstart)
{
    size_t	arglen = 0;
    അക്ഷരം	*argcopy = ശൂന്യ;
    സ്ഥിരം അക്ഷരം	*p = pstart;

    ആകുമ്പോൾ (VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(അക്ഷരം, arglen + 1);
    എങ്കിൽ (argcopy != ശൂന്യ)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	സ്വതന്ത്രം(argcopy);
    }
    അല്ലെങ്കിൽ
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_unsigned_int(
    സ്ഥിരം അക്ഷരം *pstart,
    സ്ഥിരം അക്ഷരം **p,
    unsigned പൂർണ്ണസംഖ്യ *uj,
    പൂർണ്ണസംഖ്യ overflow_err)
{
    *uj = **p - '0';
    ++*p;

    ആകുമ്പോൾ (VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned പൂർണ്ണസംഖ്യ)(**p - '0');
	++*p;
    }

    എങ്കിൽ (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	എങ്കിൽ (overflow_err)
	{
	    format_overflow_error(pstart);
	    തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    തിരികെ OK;
}


    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_fmt_types(
    സ്ഥിരം അക്ഷരം  ***ap_types,
    പൂർണ്ണസംഖ്യ		*അക്കം_posarg,
    സ്ഥിരം അക്ഷരം  *fmt,
    typval_T	*tvs UNUSED
    )
{
    സ്ഥിരം അക്ഷരം	*p = fmt;
    സ്ഥിരം അക്ഷരം	*arg = ശൂന്യ;

    പൂർണ്ണസംഖ്യ		any_pos = 0;
    പൂർണ്ണസംഖ്യ		any_arg = 0;
    പൂർണ്ണസംഖ്യ		arg_idx;

# define CHECK_POS_ARG ചെയ്യുക { \
    എങ്കിൽ (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	പോകുക error; \
    } \
} ആകുമ്പോൾ (0);

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ OK;

    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (*p != '%')
	{
	    സ്ഥിരം അക്ഷരം    *q = strchr(p + 1, '%');
	    size_t  n = (q == ശൂന്യ) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	അല്ലെങ്കിൽ
	{
	    // allowed values: \0, h, l, L
	    അക്ഷരം	length_modifier = '\0';

	    // variable for positional arg
	    പൂർണ്ണസംഖ്യ		pos_arg = -1;
	    സ്ഥിരം അക്ഷരം	*ptype = ശൂന്യ;
	    സ്ഥിരം അക്ഷരം	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    ആകുമ്പോൾ (VIM_ISDIGIT(*ptype))
		++ptype;

	    എങ്കിൽ (*ptype == '$')
	    {
		എങ്കിൽ (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    പോകുക error;
		}

		// Positional argument
		unsigned പൂർണ്ണസംഖ്യ uj;

		എങ്കിൽ (get_unsigned_int(pstart, &p, &uj, tvs != ശൂന്യ) == FAIL)
		    പോകുക error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    ആകുമ്പോൾ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		തിരഞ്ഞെടുപ്പ് (*p)
		{
		    അവസ്ഥ '0': നിർത്തുക;
		    അവസ്ഥ '-': നിർത്തുക;
		    അവസ്ഥ '+': നിർത്തുക;
		    അവസ്ഥ ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      നിർത്തുക;
		    അവസ്ഥ '#': നിർത്തുക;
		    അവസ്ഥ '\'': നിർത്തുക;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    എങ്കിൽ (*(arg = p) == '*')
	    {
		p++;

		എങ്കിൽ (VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(*p)))
		{
		    // Positional argument field width
		    unsigned പൂർണ്ണസംഖ്യ uj;

		    എങ്കിൽ (get_unsigned_int(arg + 1, &p, &uj, tvs != ശൂന്യ) == FAIL)
			പോകുക error;

		    എങ്കിൽ (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			പോകുക error;
		    }
		    അല്ലെങ്കിൽ
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			എങ്കിൽ (adjust_types(ap_types, uj, അക്കം_posarg, arg) == FAIL)
			    പോകുക error;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		സ്ഥിരം അക്ഷരം *digstart = p;
		unsigned പൂർണ്ണസംഖ്യ uj;

		എങ്കിൽ (get_unsigned_int(digstart, &p, &uj, tvs != ശൂന്യ) == FAIL)
		    പോകുക error;

		എങ്കിൽ (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    പോകുക error;
		}
	    }

	    // parse precision
	    എങ്കിൽ (*p == '.')
	    {
		p++;

		എങ്കിൽ (*(arg = p) == '*')
		{
		    p++;

		    എങ്കിൽ (VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(*p)))
		    {
			// Parse precision
			unsigned പൂർണ്ണസംഖ്യ uj;

			എങ്കിൽ (get_unsigned_int(arg + 1, &p, &uj, tvs != ശൂന്യ) == FAIL)
			    പോകുക error;

			എങ്കിൽ (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    എങ്കിൽ (adjust_types(ap_types, uj, അക്കം_posarg, arg) == FAIL)
				പോകുക error;
			}
			അല്ലെങ്കിൽ
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    പോകുക error;
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    സ്ഥിരം അക്ഷരം *digstart = p;
		    unsigned പൂർണ്ണസംഖ്യ uj;

		    എങ്കിൽ (get_unsigned_int(digstart, &p, &uj, tvs != ശൂന്യ) == FAIL)
			പോകുക error;

		    എങ്കിൽ (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			പോകുക error;
		    }
		}
	    }

	    എങ്കിൽ (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    എങ്കിൽ (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		എങ്കിൽ (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    തിരഞ്ഞെടുപ്പ് (*p)
	    {
		// Check for known format specifiers. % is special!
		അവസ്ഥ 'i':
		അവസ്ഥ '*':
		അവസ്ഥ 'd':
		അവസ്ഥ 'u':
		അവസ്ഥ 'o':
		അവസ്ഥ 'D':
		അവസ്ഥ 'U':
		അവസ്ഥ 'O':
		അവസ്ഥ 'x':
		അവസ്ഥ 'X':
		അവസ്ഥ 'b':
		അവസ്ഥ 'B':
		അവസ്ഥ 'c':
		അവസ്ഥ 's':
		അവസ്ഥ 'S':
		അവസ്ഥ 'p':
		അവസ്ഥ 'f':
		അവസ്ഥ 'F':
		അവസ്ഥ 'e':
		അവസ്ഥ 'E':
		അവസ്ഥ 'g':
		അവസ്ഥ 'G':
		    എങ്കിൽ (pos_arg != -1)
		    {
			എങ്കിൽ (adjust_types(ap_types, pos_arg, അക്കം_posarg, ptype) == FAIL)
			    പോകുക error;
		    }
		    അല്ലെങ്കിൽ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
		    എങ്കിൽ (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			പോകുക error;
		    }
	    }

	    എങ്കിൽ (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    വേണ്ടി (arg_idx = 0; arg_idx < *അക്കം_posarg; ++arg_idx)
    {
	എങ്കിൽ ((*ap_types)[arg_idx] == ശൂന്യ)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    പോകുക error;
	}

# എങ്കിൽ defined(FEAT_EVAL)
	എങ്കിൽ (tvs != ശൂന്യ && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    പോകുക error;
	}
# endif
    }

    തിരികെ OK;

error:
    vim_free((അക്ഷരം**)*ap_types);
    *ap_types = ശൂന്യ;
    *അക്കം_posarg = 0;
    തിരികെ FAIL;
}

    സ്ഥിതം ശൂന്യം
skip_to_arg(
    സ്ഥിരം അക്ഷരം	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    പൂർണ്ണസംഖ്യ		*arg_idx,
    പൂർണ്ണസംഖ്യ		*arg_cur,
    സ്ഥിരം അക്ഷരം	*fmt)
{
    പൂർണ്ണസംഖ്യ		arg_min = 0;

    എങ്കിൽ (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	തിരികെ;
    }

    എങ്കിൽ (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    അല്ലെങ്കിൽ
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    വേണ്ടി (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	സ്ഥിരം അക്ഷരം *p;

	എങ്കിൽ (ap_types == ശൂന്യ || ap_types[*arg_cur] == ശൂന്യ)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    തിരികെ;
	}

	p = ap_types[*arg_cur];

	പൂർണ്ണസംഖ്യ fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	തിരഞ്ഞെടുപ്പ് (fmt_type)
	{
	അവസ്ഥ TYPE_PERCENT:
	അവസ്ഥ TYPE_UNKNOWN:
	    നിർത്തുക;

	അവസ്ഥ TYPE_CHAR:
	    va_arg(*ap, പൂർണ്ണസംഖ്യ);
	    നിർത്തുക;

	അവസ്ഥ TYPE_STRING:
	    va_arg(*ap, അക്ഷരം *);
	    നിർത്തുക;

	അവസ്ഥ TYPE_POINTER:
	    va_arg(*ap, ശൂന്യം *);
	    നിർത്തുക;

	അവസ്ഥ TYPE_INT:
	    va_arg(*ap, പൂർണ്ണസംഖ്യ);
	    നിർത്തുക;

	അവസ്ഥ TYPE_LONGINT:
	    va_arg(*ap, long പൂർണ്ണസംഖ്യ);
	    നിർത്തുക;

	അവസ്ഥ TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    നിർത്തുക;

	അവസ്ഥ TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned പൂർണ്ണസംഖ്യ);
	    നിർത്തുക;

	അവസ്ഥ TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long പൂർണ്ണസംഖ്യ);
	    നിർത്തുക;

	അവസ്ഥ TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    നിർത്തുക;

	അവസ്ഥ TYPE_FLOAT:
	    va_arg(*ap, ഇരട്ട);
	    നിർത്തുക;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    തിരികെ;
}

    പൂർണ്ണസംഖ്യ
vim_vsnprintf_typval(
    അക്ഷരം	*str,
    size_t	str_m,
    സ്ഥിരം അക്ഷരം	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    സ്ഥിരം അക്ഷരം	*p = fmt;
    പൂർണ്ണസംഖ്യ		arg_cur = 0;
    പൂർണ്ണസംഖ്യ		അക്കം_posarg = 0;
    പൂർണ്ണസംഖ്യ		arg_idx = 1;
    va_list	ap;
    സ്ഥിരം അക്ഷരം	**ap_types = ശൂന്യ;

    എങ്കിൽ (parse_fmt_types(&ap_types, &അക്കം_posarg, fmt, tvs) == FAIL)
	തിരികെ 0;

    va_copy(ap, ap_start);

    എങ്കിൽ (p == ശൂന്യ)
	p = "";
    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (*p != '%')
	{
	    സ്ഥിരം അക്ഷരം    *q = strchr(p + 1, '%');
	    size_t  n = (q == ശൂന്യ) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    എങ്കിൽ (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	അല്ലെങ്കിൽ
	{
	    size_t  min_field_width = 0, precision = 0;
	    പൂർണ്ണസംഖ്യ	    zero_padding = 0, precision_specified = 0, justify_ഇടത് = 0;
	    പൂർണ്ണസംഖ്യ	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    പൂർണ്ണസംഖ്യ	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    അക്ഷരം    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    അക്ഷരം    tmp[TMP_LEN];

	    // string address in case of string argument
	    സ്ഥിരം അക്ഷരം  *str_arg = ശൂന്യ;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    unsigned അക്ഷരം uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  സംഖ്യ_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    അക്ഷരം    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = ശൂന്യ;

	    // variables for positional arg
	    പൂർണ്ണസംഖ്യ	    pos_arg = -1;
	    സ്ഥിരം അക്ഷരം	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    ആകുമ്പോൾ (VIM_ISDIGIT(*ptype))
		++ptype;

	    എങ്കിൽ (*ptype == '$')
	    {
		// Positional argument
		സ്ഥിരം അക്ഷരം *digstart = p;
		unsigned പൂർണ്ണസംഖ്യ uj;

		എങ്കിൽ (get_unsigned_int(digstart, &p, &uj, tvs != ശൂന്യ) == FAIL)
		    പോകുക error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    ആകുമ്പോൾ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		തിരഞ്ഞെടുപ്പ് (*p)
		{
		    അവസ്ഥ '0': zero_padding = 1; നിർത്തുക;
		    അവസ്ഥ '-': justify_ഇടത് = 1; നിർത്തുക;
		    അവസ്ഥ '+': force_sign = 1; space_for_positive = 0; നിർത്തുക;
		    അവസ്ഥ ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      നിർത്തുക;
		    അവസ്ഥ '#': alternate_form = 1; നിർത്തുക;
		    അവസ്ഥ '\'': നിർത്തുക;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    എങ്കിൽ (*p == '*')
	    {
		പൂർണ്ണസംഖ്യ j;
		സ്ഥിരം അക്ഷരം *digstart = p + 1;

		p++;

		എങ്കിൽ (VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(*p)))
		{
		    // Positional argument field width
		    unsigned പൂർണ്ണസംഖ്യ uj;

		    എങ്കിൽ (get_unsigned_int(digstart, &p, &uj, tvs != ശൂന്യ) == FAIL)
			പോകുക error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# എങ്കിൽ defined(FEAT_EVAL)
		    tvs != ശൂന്യ ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, പൂർണ്ണസംഖ്യ));

		എങ്കിൽ (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    എങ്കിൽ (tvs != ശൂന്യ)
		    {
			format_overflow_error(digstart);
			പോകുക error;
		    }
		    അല്ലെങ്കിൽ
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		എങ്കിൽ (j >= 0)
		    min_field_width = j;
		അല്ലെങ്കിൽ
		{
		    min_field_width = -j;
		    justify_ഇടത് = 1;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		സ്ഥിരം അക്ഷരം *digstart = p;
		unsigned പൂർണ്ണസംഖ്യ uj;

		എങ്കിൽ (get_unsigned_int(digstart, &p, &uj, tvs != ശൂന്യ) == FAIL)
		    പോകുക error;

		min_field_width = uj;
	    }

	    // parse precision
	    എങ്കിൽ (*p == '.')
	    {
		p++;
		precision_specified = 1;

		എങ്കിൽ (VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    സ്ഥിരം അക്ഷരം *digstart = p;
		    unsigned പൂർണ്ണസംഖ്യ uj;

		    എങ്കിൽ (get_unsigned_int(digstart, &p, &uj, tvs != ശൂന്യ) == FAIL)
			പോകുക error;

		    precision = uj;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (*p == '*')
		{
		    പൂർണ്ണസംഖ്യ j;
		    സ്ഥിരം അക്ഷരം *digstart = p;

		    p++;

		    എങ്കിൽ (VIM_ISDIGIT((പൂർണ്ണസംഖ്യ)(*p)))
		    {
			// positional argument
			unsigned പൂർണ്ണസംഖ്യ uj;

			എങ്കിൽ (get_unsigned_int(digstart, &p, &uj, tvs != ശൂന്യ) == FAIL)
			    പോകുക error;

			arg_idx = uj;

			++p;
		    }

		    j =
# എങ്കിൽ defined(FEAT_EVAL)
			tvs != ശൂന്യ ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, പൂർണ്ണസംഖ്യ));

		    എങ്കിൽ (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			എങ്കിൽ (tvs != ശൂന്യ)
			{
			    format_overflow_error(digstart);
			    പോകുക error;
			}
			അല്ലെങ്കിൽ
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    എങ്കിൽ (j >= 0)
			precision = j;
		    അല്ലെങ്കിൽ
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    എങ്കിൽ (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		എങ്കിൽ (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    length_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    തിരഞ്ഞെടുപ്പ് (fmt_spec)
	    {
		അവസ്ഥ 'i': fmt_spec = 'd'; നിർത്തുക;
		അവസ്ഥ 'D': fmt_spec = 'd'; length_modifier = 'l'; നിർത്തുക;
		അവസ്ഥ 'U': fmt_spec = 'u'; length_modifier = 'l'; നിർത്തുക;
		അവസ്ഥ 'O': fmt_spec = 'o'; length_modifier = 'l'; നിർത്തുക;
		സ്ഥിരസ്ഥിതി: നിർത്തുക;
	    }

# എങ്കിൽ defined(FEAT_EVAL)
	    തിരഞ്ഞെടുപ്പ് (fmt_spec)
	    {
		അവസ്ഥ 'd': അവസ്ഥ 'u': അവസ്ഥ 'o': അവസ്ഥ 'x': അവസ്ഥ 'X':
		    എങ്കിൽ (tvs != ശൂന്യ && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    എങ്കിൽ (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    തിരഞ്ഞെടുപ്പ് (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    അവസ്ഥ '%':
	    അവസ്ഥ 'c':
	    അവസ്ഥ 's':
	    അവസ്ഥ 'S':
		str_arg_l = 1;
		തിരഞ്ഞെടുപ്പ് (fmt_spec)
		{
		അവസ്ഥ '%':
		    str_arg = p;
		    നിർത്തുക;

		അവസ്ഥ 'c':
		    {
			പൂർണ്ണസംഖ്യ j;

			j =
# എങ്കിൽ defined(FEAT_EVAL)
			    tvs != ശൂന്യ ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, പൂർണ്ണസംഖ്യ));

			// standard demands unsigned char
			uchar_arg = (unsigned അക്ഷരം)j;
			str_arg = (അക്ഷരം *)&uchar_arg;
			നിർത്തുക;
		    }

		അവസ്ഥ 's':
		അവസ്ഥ 'S':
		    str_arg =
# എങ്കിൽ defined(FEAT_EVAL)
				tvs != ശൂന്യ ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, അക്ഷരം *));

		    എങ്കിൽ (str_arg == ശൂന്യ)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    അല്ലെങ്കിൽ എങ്കിൽ (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    അല്ലെങ്കിൽ എങ്കിൽ (precision == 0)
			str_arg_l = 0;
		    അല്ലെങ്കിൽ
		    {
			// memchr on HP does not like n > 2^31  !!!
			സ്ഥിരം അക്ഷരം *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == ശൂന്യ) ? precision
						      : (size_t)(q - str_arg);
		    }
		    എങ്കിൽ (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			പൂർണ്ണസംഖ്യ	cell;

			വേണ്ടി (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    എങ്കിൽ (precision_specified && i + cell > precision)
				നിർത്തുക;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			എങ്കിൽ (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
		    നിർത്തുക;
		}
		നിർത്തുക;

	    അവസ്ഥ 'd': അവസ്ഥ 'u':
	    അവസ്ഥ 'b': അവസ്ഥ 'B':
	    അവസ്ഥ 'o':
	    അവസ്ഥ 'x': അവസ്ഥ 'X':
	    അവസ്ഥ 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //മൂല്യം

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    പൂർണ്ണസംഖ്യ arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    പൂർണ്ണസംഖ്യ int_arg = 0;
		    unsigned പൂർണ്ണസംഖ്യ uint_arg = 0;

		    // only set for length modifier l
		    long പൂർണ്ണസംഖ്യ long_arg = 0;
		    unsigned long പൂർണ്ണസംഖ്യ ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    ശൂന്യം *ptr_arg = ശൂന്യ;

		    എങ്കിൽ (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# എങ്കിൽ defined(FEAT_EVAL)
				 tvs != ശൂന്യ ? (ശൂന്യം *)tv_str(tvs, &arg_idx,
									ശൂന്യ) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, ശൂന്യം *));

			എങ്കിൽ (ptr_arg != ശൂന്യ)
			    arg_sign = 1;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# എങ്കിൽ defined(FEAT_EVAL)
				    tvs != ശൂന്യ ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			എങ്കിൽ (bin_arg != 0)
			    arg_sign = 1;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (fmt_spec == 'd')
		    {
			// signed
			തിരഞ്ഞെടുപ്പ് (length_modifier)
			{
			അവസ്ഥ '\0':
			അവസ്ഥ 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# എങ്കിൽ defined(FEAT_EVAL)
					tvs != ശൂന്യ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, പൂർണ്ണസംഖ്യ));

			    എങ്കിൽ (int_arg > 0)
				arg_sign =  1;
			    അല്ലെങ്കിൽ എങ്കിൽ (int_arg < 0)
				arg_sign = -1;
			    നിർത്തുക;
			അവസ്ഥ 'l':
			    long_arg =
# എങ്കിൽ defined(FEAT_EVAL)
					tvs != ശൂന്യ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long പൂർണ്ണസംഖ്യ));

			    എങ്കിൽ (long_arg > 0)
				arg_sign =  1;
			    അല്ലെങ്കിൽ എങ്കിൽ (long_arg < 0)
				arg_sign = -1;
			    നിർത്തുക;
			അവസ്ഥ 'L':
			    llong_arg =
# എങ്കിൽ defined(FEAT_EVAL)
					tvs != ശൂന്യ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    എങ്കിൽ (llong_arg > 0)
				arg_sign =  1;
			    അല്ലെങ്കിൽ എങ്കിൽ (llong_arg < 0)
				arg_sign = -1;
			    നിർത്തുക;
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			// unsigned
			തിരഞ്ഞെടുപ്പ് (length_modifier)
			{
			    അവസ്ഥ '\0':
			    അവസ്ഥ 'h':
				uint_arg =
# എങ്കിൽ defined(FEAT_EVAL)
					    tvs != ശൂന്യ ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned പൂർണ്ണസംഖ്യ));

				എങ്കിൽ (uint_arg != 0)
				    arg_sign = 1;
				നിർത്തുക;
			    അവസ്ഥ 'l':
				ulong_arg =
# എങ്കിൽ defined(FEAT_EVAL)
					    tvs != ശൂന്യ ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long പൂർണ്ണസംഖ്യ));

				എങ്കിൽ (ulong_arg != 0)
				    arg_sign = 1;
				നിർത്തുക;
			    അവസ്ഥ 'L':
				ullong_arg =
# എങ്കിൽ defined(FEAT_EVAL)
					    tvs != ശൂന്യ ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				എങ്കിൽ (ullong_arg != 0)
				    arg_sign = 1;
				നിർത്തുക;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    എങ്കിൽ (precision_specified)
			zero_padding = 0;
		    എങ്കിൽ (fmt_spec == 'd')
		    {
			എങ്കിൽ (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (alternate_form)
		    {
			എങ്കിൽ (arg_sign != 0
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
		    എങ്കിൽ (!precision_specified)
			precision = 1;   // default precision is 1
		    എങ്കിൽ (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    അല്ലെങ്കിൽ
		    {
			അക്ഷരം	f[6];
			പൂർണ്ണസംഖ്യ	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			എങ്കിൽ (!length_modifier)
			    ;
			അല്ലെങ്കിൽ എങ്കിൽ (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# അല്ലെങ്കിൽ
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			അല്ലെങ്കിൽ
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			എങ്കിൽ (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			അല്ലെങ്കിൽ എങ്കിൽ (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    അക്ഷരം	    b[8 * വലുപ്പം(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    ചെയ്യുക
			    {
				b[വലുപ്പം(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    ആകുമ്പോൾ (bn != 0);

			    memcpy(tmp + str_arg_l, b + വലുപ്പം(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (fmt_spec == 'd')
			{
			    // signed
			    തിരഞ്ഞെടുപ്പ് (length_modifier)
			    {
			    അവസ്ഥ '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       നിർത്തുക;
			    അവസ്ഥ 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (short)int_arg);
				      നിർത്തുക;
			    അവസ്ഥ 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      നിർത്തുക;
			    അവസ്ഥ 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      നിർത്തുക;
			    }
			}
			അല്ലെങ്കിൽ
			{
			    // unsigned
			    തിരഞ്ഞെടുപ്പ് (length_modifier)
			    {
			    അവസ്ഥ '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       നിർത്തുക;
			    അവസ്ഥ 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(unsigned short)uint_arg);
				      നിർത്തുക;
			    അവസ്ഥ 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      നിർത്തുക;
			    അവസ്ഥ 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      നിർത്തുക;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			എങ്കിൽ (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			എങ്കിൽ (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t അക്കം_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			എങ്കിൽ (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    എങ്കിൽ (!precision_specified
					     || precision < അക്കം_of_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = അക്കം_of_digits + 1;
			    }
			}
			// zero padding to specified precision?
			എങ്കിൽ (അക്കം_of_digits < precision)
			    സംഖ്യ_of_zeros_to_pad = precision - അക്കം_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    എങ്കിൽ (!justify_ഇടത് && zero_padding)
		    {
			പൂർണ്ണസംഖ്യ n = (പൂർണ്ണസംഖ്യ)(min_field_width - (str_arg_l
						    + സംഖ്യ_of_zeros_to_pad));
			എങ്കിൽ (n > 0)
			    സംഖ്യ_of_zeros_to_pad += n;
		    }
		    നിർത്തുക;
		}

	    അവസ്ഥ 'f':
	    അവസ്ഥ 'F':
	    അവസ്ഥ 'e':
	    അവസ്ഥ 'E':
	    അവസ്ഥ 'g':
	    അവസ്ഥ 'G':
		{
		    // Floating point.
		    ഇരട്ട	f;
		    ഇരട്ട	abs_f;
		    അക്ഷരം	format[40];
		    പൂർണ്ണസംഖ്യ		l;
		    പൂർണ്ണസംഖ്യ		remove_trailing_zeroes = FALSE;

		    f =
# എങ്കിൽ defined(FEAT_EVAL)
			tvs != ശൂന്യ ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, ഇരട്ട));

		    abs_f = f < 0 ? -f : f;

		    എങ്കിൽ (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			എങ്കിൽ ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			അല്ലെങ്കിൽ
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    എങ്കിൽ ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# അല്ലെങ്കിൽ
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
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			അല്ലെങ്കിൽ
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    എങ്കിൽ (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    എങ്കിൽ (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				എങ്കിൽ ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				എങ്കിൽ (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (പൂർണ്ണസംഖ്യ)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			എങ്കിൽ (remove_trailing_zeroes)
			{
			    പൂർണ്ണസംഖ്യ i;
			    അക്ഷരം *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    എങ്കിൽ (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    അല്ലെങ്കിൽ
			    {
				tp = (അക്ഷരം *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				എങ്കിൽ (tp != ശൂന്യ)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    എങ്കിൽ (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    ആകുമ്പോൾ (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    എങ്കിൽ (tp != ശൂന്യ && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				ആകുമ്പോൾ (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			അല്ലെങ്കിൽ
			{
			    അക്ഷരം *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (അക്ഷരം *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    എങ്കിൽ (tp != ശൂന്യ && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    എങ്കിൽ (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			സംഖ്യ_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    നിർത്തുക;
		}

	    സ്ഥിരസ്ഥിതി:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_ഇടത് = 1;
		min_field_width = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		എങ്കിൽ (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		നിർത്തുക;
	    }

	    എങ്കിൽ (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    എങ്കിൽ (!justify_ഇടത്)
	    {
		// left padding with blank or zero
		പൂർണ്ണസംഖ്യ pn = (പൂർണ്ണസംഖ്യ)(min_field_width - (str_arg_l + സംഖ്യ_of_zeros_to_pad));

		എങ്കിൽ (pn > 0)
		{
		    എങ്കിൽ (str_l < str_m)
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
	    എങ്കിൽ (സംഖ്യ_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    അല്ലെങ്കിൽ
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		പൂർണ്ണസംഖ്യ zn = (പൂർണ്ണസംഖ്യ)zero_padding_insertion_ind;

		എങ്കിൽ (zn > 0)
		{
		    എങ്കിൽ (str_l < str_m)
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
		zn = (പൂർണ്ണസംഖ്യ)സംഖ്യ_of_zeros_to_pad;
		എങ്കിൽ (zn > 0)
		{
		    എങ്കിൽ (str_l < str_m)
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
		പൂർണ്ണസംഖ്യ sn = (പൂർണ്ണസംഖ്യ)(str_arg_l - zero_padding_insertion_ind);

		എങ്കിൽ (sn > 0)
		{
		    എങ്കിൽ (str_l < str_m)
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
	    എങ്കിൽ (justify_ഇടത്)
	    {
		// right blank padding to the field width
		പൂർണ്ണസംഖ്യ pn = (പൂർണ്ണസംഖ്യ)(min_field_width
				      - (str_arg_l + സംഖ്യ_of_zeros_to_pad));

		എങ്കിൽ (pn > 0)
		{
		    എങ്കിൽ (str_l < str_m)
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

    എങ്കിൽ (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    എങ്കിൽ (tvs != ശൂന്യ && tvs[അക്കം_posarg != 0 ? അക്കം_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((അക്ഷരം*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    തിരികെ (പൂർണ്ണസംഖ്യ)str_l;
}

#endif // PROTO
