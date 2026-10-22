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
#ಅನ್ತರ್ಭಾವ "vim.h"

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
    ಯದಿ (p != ರಿಕ್ತ)
	mch_memmove(p, string, len);
    ಪ್ರತ್ಯಾಗಮ p;
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
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    STRNCPY(p, string, len);
    p[len] = NUL;
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    ಪ್ರತ್ಯಾಗಮ vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    ಪೂರ್ಣಾಙ್ಕ		cc,
    ಪೂರ್ಣಾಙ್ಕ		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    ಪೂರ್ಣಾಙ್ಕ		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    ಪ್ರತಿ (p = string; *p; p++)
    {
	ಯದಿ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (vim_strchr(esc_chars, *p) != ರಿಕ್ತ || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    ಯದಿ (escaped_string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    p2 = escaped_string;
    ಪ್ರತಿ (p = string; *p; p++)
    {
	ಯದಿ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    ಅನುವರ್ತ;
	}
	ಯದಿ (vim_strchr(esc_chars, *p) != ರಿಕ್ತ || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    ಪ್ರತ್ಯಾಗಮ escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    ಪೂರ್ಣಾಙ್ಕ
csh_like_shell(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (strstr((ವರ್ಣ *)gettail(p_sh), "csh") != ರಿಕ್ತ);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fish_like_shell(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (strstr((ವರ್ಣ *)gettail(p_sh), "fish") != ರಿಕ್ತ);
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
vim_strsave_shellescape(char_u *string, ಪೂರ್ಣಾಙ್ಕ do_special, ಪೂರ್ಣಾಙ್ಕ do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    ಪೂರ್ಣಾಙ್ಕ		csh_like;
    ಪೂರ್ಣಾಙ್ಕ		fish_like;
    char_u	*shname;
    ಪೂರ್ಣಾಙ್ಕ		powershell;
#ifdef MSWIN
    ಪೂರ್ಣಾಙ್ಕ		double_quotes;
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
    powershell = strstr((ವರ್ಣ *)shname, "pwsh") != ರಿಕ್ತ;
#ifdef MSWIN
    powershell = powershell || strstr((ವರ್ಣ *)shname, "powershell") != ರಿಕ್ತ;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (unsigned)STRLEN(string) + 3;  // two quotes and a trailing NUL
    ಪ್ರತಿ (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	ಯದಿ (double_quotes)
	{
	    ಯದಿ (*p == '"')
		++length;		// " -> ""
	}
	ಅನ್ಯಥಾ
#endif
	ಯದಿ (*p == '\'')
	{
	    ಯದಿ (powershell)
		length += 2;		// ' => ''
	    ಅನ್ಯಥಾ
		length += 3;		// ' => '\''
	}
	ಯದಿ ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    ಯದಿ (csh_like && do_special)
		++length;		// insert backslash
	}
	ಯದಿ (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	ಯದಿ (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    ಯದಿ (escaped_string != ರಿಕ್ತ)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	ಯದಿ (double_quotes)
	    *d++ = '"';
	ಅನ್ಯಥಾ
#endif
	    *d++ = '\'';

	ಪ್ರತಿ (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    ಯದಿ (double_quotes)
	    {
		ಯದಿ (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    ಅನುವರ್ತ;
		}
	    }
	    ಅನ್ಯಥಾ
#endif
	    ಯದಿ (*p == '\'')
	    {
		ಯದಿ (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		ಅನ್ಯಥಾ
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		ಯದಿ (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		ಅನುವರ್ತ;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	ಯದಿ (double_quotes)
	    *d++ = '"';
	ಅನ್ಯಥಾ
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    ಪ್ರತ್ಯಾಗಮ escaped_string;
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
    ಪ್ರತ್ಯಾಗಮ p1;
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
    ಪ್ರತ್ಯಾಗಮ p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    ಶೂನ್ಯ
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    ಪೂರ್ಣಾಙ್ಕ	    c;

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    p2 = p;
    ಯಾವತ್ ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    ಯದಿ (res != ರಿಕ್ತ)
	ಯಾವತ್ (*p != NUL)
	{
	    ಪೂರ್ಣಾಙ್ಕ		l;

	    ಯದಿ (enc_utf8)
	    {
		ಪೂರ್ಣಾಙ್ಕ	c, uc;
		ಪೂರ್ಣಾಙ್ಕ	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ಯದಿ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		ಯದಿ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ಯದಿ (s == ರಿಕ್ತ)
		    {
			vim_free(res);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
	    ಅನ್ಯಥಾ ಯದಿ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ಅನ್ಯಥಾ
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    ಪ್ರತ್ಯಾಗಮ res;
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

    ಯದಿ (res != ರಿಕ್ತ)
	ಯಾವತ್ (*p != NUL)
	{
	    ಪೂರ್ಣಾಙ್ಕ		l;

	    ಯದಿ (enc_utf8)
	    {
		ಪೂರ್ಣಾಙ್ಕ	c, lc;
		ಪೂರ್ಣಾಙ್ಕ	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ಯದಿ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		ಯದಿ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ಯದಿ (s == ರಿಕ್ತ)
		    {
			vim_free(res);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
	    ಅನ್ಯಥಾ ಯದಿ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ಅನ್ಯಥಾ
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    ಪ್ರತ್ಯಾಗಮ res;
}
#endif

/*
 * delete spaces at the end of a string
 */
    ಶೂನ್ಯ
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    ಯಾವತ್ (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    ಶೂನ್ಯ
vim_strncpy(char_u *to, char_u *from, size_t len)
{
    STRNCPY(to, from, len);
    to[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    ಶೂನ್ಯ
vim_strcat(char_u *to, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(to);
    size_t fromlen = STRLEN(from);

    ಯದಿ (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    ಅನ್ಯಥಾ
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(ವರ್ಣ *s, size_t maxlen)
{
    size_t i;
    ಪ್ರತಿ (i = 0; i < maxlen; ++i)
	ಯದಿ (s[i] == NUL)
	    ಭಙ್ಗ;
    ಪ್ರತ್ಯಾಗಮ i;
}

#ಯದಿ !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_stricmp(ವರ್ಣ *s1, ವರ್ಣ *s2)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (;;)
    {
	i = (ಪೂರ್ಣಾಙ್ಕ)TOLOWER_LOC(*s1) - (ಪೂರ್ಣಾಙ್ಕ)TOLOWER_LOC(*s2);
	ಯದಿ (i != 0)
	    ಪ್ರತ್ಯಾಗಮ i;			    // this character different
	ಯದಿ (*s1 == NUL)
	    ಭಙ್ಗ;			    // strings match until NUL
	++s1;
	++s2;
    }
    ಪ್ರತ್ಯಾಗಮ 0;				    // strings match
}
#endif

#ಯದಿ !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_strnicmp(ವರ್ಣ *s1, ವರ್ಣ *s2, size_t len)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯಾವತ್ (len > 0)
    {
	i = (ಪೂರ್ಣಾಙ್ಕ)TOLOWER_LOC(*s1) - (ಪೂರ್ಣಾಙ್ಕ)TOLOWER_LOC(*s2);
	ಯದಿ (i != 0)
	    ಪ್ರತ್ಯಾಗಮ i;			    // this character different
	ಯದಿ (*s1 == NUL)
	    ಭಙ್ಗ;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ಪ್ರತ್ಯಾಗಮ 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_strnicmp_asc(ವರ್ಣ *s1, ವರ್ಣ *s2, size_t len)
{
    ಪೂರ್ಣಾಙ್ಕ                i = 0;

    ಯಾವತ್ (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	ಯದಿ (i != 0)
	    ಭಙ್ಗ;			// this character is different
	ಯದಿ (*s1 == NUL)
	    ಭಙ್ಗ;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ಪ್ರತ್ಯಾಗಮ i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, ಪೂರ್ಣಾಙ್ಕ c)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		b;

    p = string;
    ಯದಿ (enc_utf8 && c > 0 && c < 0x80)
	ಪ್ರತ್ಯಾಗಮ vim_strbyte(string, c);
    ಯದಿ (enc_utf8 && c >= 0x80)
    {
	ಯಾವತ್ (*p != NUL)
	{
	    ಪೂರ್ಣಾಙ್ಕ l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    ಯದಿ (utf_ptr2char(p) == c && l > 1)
		ಪ್ರತ್ಯಾಗಮ p;
	    p += l;
	}
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (enc_dbcs != 0 && c > 255)
    {
	ಪೂರ್ಣಾಙ್ಕ	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	ಯಾವತ್ ((b = *p) != NUL)
	{
	    ಯದಿ (b == c && p[1] == n2)
		ಪ್ರತ್ಯಾಗಮ p;
	    p += (*mb_ptr2len)(p);
	}
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (has_mbyte)
    {
	ಯಾವತ್ ((b = *p) != NUL)
	{
	    ಯದಿ (b == c)
		ಪ್ರತ್ಯಾಗಮ p;
	    p += (*mb_ptr2len)(p);
	}
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯಾವತ್ ((b = *p) != NUL)
    {
	ಯದಿ (b == c)
	    ಪ್ರತ್ಯಾಗಮ p;
	++p;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, ಪೂರ್ಣಾಙ್ಕ c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    ಯದಿ (c <= 0 || c > 255)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (char_u *)strchr((ವರ್ಣ *)string, c);
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
vim_strrchr(char_u *string, ಪೂರ್ಣಾಙ್ಕ c)
{
    char_u	*retval = ರಿಕ್ತ;
    char_u	*p = string;

    ಯಾವತ್ (*p)
    {
	ಯದಿ (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    ಪ್ರತ್ಯಾಗಮ retval;
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
    ಯಾವತ್ (*s)
    {
	ಯದಿ (vim_strchr(charset, *s) != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ s;
	MB_PTR_ADV(s);
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sort_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2);

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sort_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    ಪ್ರತ್ಯಾಗಮ STRCMP(*(ವರ್ಣ **)s1, *(ವರ್ಣ **)s2);
}

    ಶೂನ್ಯ
sort_strings(
    char_u	**files,
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ)
{
    qsort((ಶೂನ್ಯ *)files, (size_t)ಎಣಿಕೆ, ಪರಿಮಾಣ(char_u *), sort_compare);
}

#ಯದಿ defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_non_ascii(char_u *s)
{
    char_u	*p;

    ಯದಿ (s != ರಿಕ್ತ)
	ಪ್ರತಿ (p = s; *p != NUL; ++p)
	    ಯದಿ (*p >= 128)
		ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    size_t  l = str1 == ರಿಕ್ತ ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == ರಿಕ್ತ ? 0 : STRLEN(str2)) + 1L);
    ಯದಿ (dest == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (str1 == ರಿಕ್ತ)
	*dest = NUL;
    ಅನ್ಯಥಾ
	STRCPY(dest, str1);
    ಯದಿ (str2 != ರಿಕ್ತ)
	STRCPY(dest + l, str2);
    ಪ್ರತ್ಯಾಗಮ dest;
}

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    ಯದಿ (rev == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	ಯದಿ (has_mbyte)
	{
	    ಪೂರ್ಣಾಙ್ಕ mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	ಅನ್ಯಥಾ
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    ಪ್ರತ್ಯಾಗಮ rev;
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, ಪೂರ್ಣಾಙ್ಕ function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    ಯದಿ (str != ರಿಕ್ತ)
    {
	len += (unsigned)STRLEN(str);
	ಪ್ರತಿ (p = str; *p != NUL; MB_PTR_ADV(p))
	    ಯದಿ (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    ಯದಿ (r == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    ಅನ್ಯಥಾ
	*r++ = '\'';
    ಯದಿ (str != ರಿಕ್ತ)
	ಪ್ರತಿ (p = str; *p != NUL; )
	{
	    ಯದಿ (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    ಯದಿ (function)
	*r++ = ')';
    *r++ = NUL;
    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_ಎಣಿಕೆ(char_u *haystack, char_u *needle, ಪೂರ್ಣಾಙ್ಕ ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    ಯದಿ (p == ರಿಕ್ತ || needle == ರಿಕ್ತ || *needle == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;

    size_t  needlelen = STRLEN(needle);
    ಯದಿ (ic)
    {
	ಯಾವತ್ (*p != NUL)
	{
	    ಯದಿ (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    ಅನ್ಯಥಾ
		MB_PTR_ADV(p);
	}
    }
    ಅನ್ಯಥಾ
	ಯಾವತ್ ((next = (char_u *)strstr((ವರ್ಣ *)p, (ವರ್ಣ *)needle)) != ರಿಕ್ತ)
	{
	    ++n;
	    p = next + needlelen;
	}

    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
copy_first_char_to_tv(char_u *ಒಳಹರಿವು, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (ಒಳಹರಿವು == ರಿಕ್ತ || output == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    len = has_mbyte ? mb_ptr2len(ಒಳಹರಿವು) : 1;
    STRNCPY(buf, ಒಳಹರಿವು, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    ಪ್ರತ್ಯಾಗಮ output->vval.v_string == ರಿಕ್ತ ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    ಶೂನ್ಯ
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, &newtv);

    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 80);
    ಪ್ರತಿ (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	ಯದಿ (len < 0)
	    ಭಙ್ಗ;

	set_vim_var_nr(VV_ಕೀಲಿ, idx);
	ಯದಿ (filter_map_one(&tv, ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    ಭಙ್ಗ;
	}
	ಯದಿ (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    ಯದಿ (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ
		ga_concat(&ga, newtv.vval.v_string);
	}
	ಅನ್ಯಥಾ ಯದಿ (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    ಯದಿ (fc != ರಿಕ್ತ)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    ಶೂನ್ಯ
string_reduce(
	typval_T	*argvars,
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    ಪೂರ್ಣಾಙ್ಕ		len;
    typval_T	argv[3];
    ಪೂರ್ಣಾಙ್ಕ		r;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_start = called_emsg;
    funccall_T	*fc;

    ಯದಿ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ಯದಿ (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_ಮೌಲ್ಯ), "String");
	    ಪ್ರತ್ಯಾಗಮ;
	}
	len = copy_first_char_to_tv(p, rettv);
	ಯದಿ (len < 0)
	    ಪ್ರತ್ಯಾಗಮ;
	p += len;
    }
    ಅನ್ಯಥಾ ಯದಿ (check_for_string_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಅನ್ಯಥಾ
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, rettv);

    ಪ್ರತಿ ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	ಯದಿ (len < 0)
	{
	    *rettv = argv[0];
	    ಭಙ್ಗ;
	}

	r = eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	ಯದಿ (r == FAIL || called_emsg != called_emsg_start)
	    ಭಙ್ಗ;
    }

    ಯದಿ (fc != ರಿಕ್ತ)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
byteidx_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ comp)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ);
    ಯದಿ (str == ರಿಕ್ತ || idx < 0)
	ಪ್ರತ್ಯಾಗಮ;

    varnumber_T	utf16idx = FALSE;
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಙ್ಕ error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_ಸಂಖ್ಯೆ_as_bool_nr), utf16idx);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    ಪೂರ್ಣಾಙ್ಕ (*ptr2len)(char_u *);
    ಯದಿ (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    ಅನ್ಯಥಾ
	ptr2len = mb_ptr2len;

    char_u *t = str;
    ಪ್ರತಿ ( ; idx > 0; idx--)
    {
	ಯದಿ (*t == NUL)		// EOL reached
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (utf16idx)
	{
	    ಪೂರ್ಣಾಙ್ಕ clen = ptr2len(t);
	    ಪೂರ್ಣಾಙ್ಕ c = (clen > 1) ? utf_ptr2char(t) : *t;
	    ಯದಿ (c > 0xFFFF)
		idx--;
	    ಯದಿ (idx > 0)
		t += clen;
	}
	ಅನ್ಯಥಾ ಯದಿ (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    ಶೂನ್ಯ
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    ಶೂನ್ಯ
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    ಶೂನ್ಯ
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ);
    ಯದಿ (str == ರಿಕ್ತ || idx < 0)
	ಪ್ರತ್ಯಾಗಮ;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    ಪೂರ್ಣಾಙ್ಕ (*ptr2len)(char_u *);
    ಯದಿ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    ಅನ್ಯಥಾ
	ptr2len = mb_ptr2len;

    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪ್ರತಿ (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	ಯದಿ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    ಯದಿ (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_ಸಂಖ್ಯೆ = len;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (utf16idx)
	{
	    idx--;
	    ಪೂರ್ಣಾಙ್ಕ clen = ptr2len(p);
	    ಪೂರ್ಣಾಙ್ಕ c = (clen > 1) ? utf_ptr2char(p) : *p;
	    ಯದಿ (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ಯದಿ (convert_setup(&vimconv, from, to) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    vimconv.vc_fail = TRUE;
    ಯದಿ (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	ಯದಿ (ret->string == ರಿಕ್ತ)
	    ret->length = 0;
	ಅನ್ಯಥಾ
	    ret->length = str->length;
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ len = (ಪೂರ್ಣಾಙ್ಕ)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, ರಿಕ್ತ, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ (ret->string == ರಿಕ್ತ) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
blob_from_string(char_u *str, blob_T *blob)
{
    ಪೂರ್ಣಾಙ್ಕ	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str);
    char_u  *dest;

    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (ga_grow(&blob->bv_ga, len) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len; ++i)
	ಯದಿ (dest[i] == NL)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	ಸಾಲು_len;

    ಯದಿ (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	ಪ್ರತ್ಯಾಗಮ (ret->string == ರಿಕ್ತ) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    ಸಾಲು_len = (nl == ರಿಕ್ತ) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(ಸಾಲು_len + 1);
    ಯದಿ (ret->string == ರಿಕ್ತ)
    {
	ret->length = 0;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (ಸಾಲು_len > 0)
	mch_memmove(ret->string, src, (size_t)ಸಾಲು_len);
    ret->string[ಸಾಲು_len] = NUL;
    ret->length = (size_t)ಸಾಲು_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    ಪ್ರತಿ (long i = 0; i < ಸಾಲು_len; i++)
	ಯದಿ (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + ಸಾಲು_len + (nl != ರಿಕ್ತ ? 1 : 0);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    ಸ್ಥಿರ char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    ಯದಿ (from_encoding_raw == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    ಯಾವತ್ (*s != NUL)
    {
	ಯದಿ (*s == '_')
	    *pe++ = '-';
	ಅನ್ಯಥಾ
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    ಯದಿ ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    ಪ್ರತ್ಯಾಗಮ from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
append_converted_string_to_list(
	string_T *converted,
	ಪೂರ್ಣಾಙ್ಕ validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    ಯದಿ (converted->string != ರಿಕ್ತ)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	ಯಾವತ್ (p < end)
	{
	    string_T	ಸಾಲು;
	    char_u	*ಸಾಲು_start = p;

	    ಯಾವತ್ (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    ಸಾಲು.length = (size_t)(p - ಸಾಲು_start);
	    ಸಾಲು.string = vim_strnsave(ಸಾಲು_start, ಸಾಲು.length);
	    ಯದಿ (ಸಾಲು.string != ರಿಕ್ತ)
	    {
		ಯದಿ (validate_utf8 && !utf_valid_string(ಸಾಲು.string, ರಿಕ್ತ))
		{
		    vim_free(ಸಾಲು.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    ಪ್ರತ್ಯಾಗಮ; // Stop processing
		}
		ಯದಿ (list_append_string(list, ಸಾಲು.string, (ಪೂರ್ಣಾಙ್ಕ)ಸಾಲು.length) == FAIL)
		{
		    vim_free(ಸಾಲು.string);
		    vim_free(converted->string);
		    ಪ್ರತ್ಯಾಗಮ; // Stop processing on append failure
		}
		vim_free(ಸಾಲು.string);
	    }
	    ಅನ್ಯಥಾ
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		ಪ್ರತ್ಯಾಗಮ;
	    }

	    ಯದಿ (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    ಅನ್ಯಥಾ
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
append_validated_ಸಾಲು_to_list(string_T *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ validate_utf8, list_T *list)
{
    ಯದಿ (validate_utf8 && !utf_valid_string(ಸಾಲು->string, ರಿಕ್ತ))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(ಸಾಲು->string);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪೂರ್ಣಾಙ್ಕ ret = list_append_string(list, ಸಾಲು->string, (ಪೂರ್ಣಾಙ್ಕ)ಸಾಲು->length);
    vim_free(ಸಾಲು->string);
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಶೂನ್ಯ
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    ಪೂರ್ಣಾಙ್ಕ		blen;
    long	idx;
    ಪೂರ್ಣಾಙ್ಕ		validate_utf8 = FALSE;

    ಯದಿ (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    blob = argvars->vval.v_blob;
    ಯದಿ (blob == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    blen = blob_len(blob);

    char_u	*from_encoding = ರಿಕ್ತ;
    char_u	*from_encoding_raw = ರಿಕ್ತ;  // Encoding name with endianness preserved for iconv
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ಯದಿ (d != ರಿಕ್ತ)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ಯದಿ (enc != ರಿಕ್ತ)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		ಯದಿ (from_encoding_raw == ರಿಕ್ತ)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    ಪ್ರತ್ಯಾಗಮ;
		}
	    }
	}
    }

    ಯದಿ (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    ಯದಿ (from_encoding != ರಿಕ್ತ && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    ಪೂರ್ಣಾಙ್ಕ from_prop = 0;
    ಯದಿ (from_encoding != ರಿಕ್ತ)
	from_prop = enc_canon_props(from_encoding);
    ಯದಿ (from_encoding != ರಿಕ್ತ && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	ಪೂರ್ಣಾಙ್ಕ nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	ಯದಿ (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    ಯದಿ (blen > 0)
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
	ಯದಿ (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    ಗಚ್ಛ done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	ಪೂರ್ಣಾಙ್ಕ len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, ರಿಕ್ತ);
	converted.length = len;
	convert_setup(&vimconv, ರಿಕ್ತ, ರಿಕ್ತ);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    ಅನ್ಯಥಾ
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	ಯಾವತ್ (idx < blen)
	{
	    string_T	str;

	    ಯದಿ (string_from_blob(blob, &idx, &str) != OK)
		ಭಙ್ಗ;

	    ಯದಿ (from_encoding != ರಿಕ್ತ)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		ಪೂರ್ಣಾಙ್ಕ	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		ಯದಿ (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    ಗಚ್ಛ done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    ಯದಿ (append_validated_ಸಾಲು_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		ಗಚ್ಛ done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    ಯದಿ (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

done:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    ಶೂನ್ಯ
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (rettv_blob_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    char_u	*to_encoding = ರಿಕ್ತ;
    char_u	*to_encoding_raw = ರಿಕ್ತ;  // Encoding name with endianness preserved for iconv
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ಯದಿ (d != ರಿಕ್ತ)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ಯದಿ (enc != ರಿಕ್ತ)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		ಯದಿ (to_encoding_raw == ರಿಕ್ತ)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    ಪ್ರತ್ಯಾಗಮ;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    ಪೂರ್ಣಾಙ್ಕ to_prop = 0;
    ಯದಿ (to_encoding != ರಿಕ್ತ)
	to_prop = enc_canon_props(to_encoding);
    ಯದಿ (to_encoding != ರಿಕ್ತ && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
	{
	    char_u *s;

	    ಯದಿ (li->li_tv.v_type != VAR_STRING)
		ಅನುವರ್ತ;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    ಯದಿ (li != list->lv_first)
		ga_append(&str_ga, NL);
	    ಯದಿ (s != ರಿಕ್ತ && *s != NUL)
	    {
		ಪೂರ್ಣಾಙ್ಕ slen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);

		ಯದಿ (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    ಗಚ್ಛ done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	ಯದಿ (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    ಯದಿ (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		ಗಚ್ಛ done;
	    }
	    vimconv.vc_fail = TRUE;

	    ಪೂರ್ಣಾಙ್ಕ		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, ರಿಕ್ತ);
	    convert_setup(&vimconv, ರಿಕ್ತ, ರಿಕ್ತ);
	    ga_clear(&str_ga);

	    ಯದಿ (converted == ರಿಕ್ತ)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		ಗಚ್ಛ done;
	    }
	    ಯದಿ (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	ಅನ್ಯಥಾ
	    ga_clear(&str_ga);
    }
    ಅನ್ಯಥಾ
    {
	FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
	{
	    ಯದಿ (li->li_tv.v_type != VAR_STRING)
		ಅನುವರ್ತ;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    ಯದಿ (str.string == ರಿಕ್ತ)
		STR_LITERAL_SET(str, "");
	    ಅನ್ಯಥಾ
		str.length = STRLEN(str.string);

	    ಯದಿ (to_encoding != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		ಯದಿ (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    ಗಚ್ಛ done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    ಯದಿ (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    ಯದಿ (to_encoding != ರಿಕ್ತ)
		vim_free(str.string);
	}
    }

done:
    ಯದಿ (to_encoding != ರಿಕ್ತ)
	vim_free(to_encoding);
    ಯದಿ (to_encoding_raw != ರಿಕ್ತ)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    ಶೂನ್ಯ
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		utf8 = FALSE;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[1], ರಿಕ್ತ);

    p = tv_get_string(&argvars[0]);

    ಯದಿ (has_mbyte || utf8)
    {
	ಪೂರ್ಣಾಙ್ಕ (*ptr2len)(char_u *);
	ಪೂರ್ಣಾಙ್ಕ (*ptr2char)(char_u *);

	ಯದಿ (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	ಅನ್ಯಥಾ
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	ಪ್ರತಿ ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (*ptr2char)(p));
    }
    ಅನ್ಯಥಾ
	ಪ್ರತಿ ( ; *p != NUL; ++p)
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    ಶೂನ್ಯ
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		base = 10;
    char_u	*p;
    varnumber_T	n;
    ಪೂರ್ಣಾಙ್ಕ		what = 0;
    ಪೂರ್ಣಾಙ್ಕ		isneg;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	ಯದಿ (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    ಯದಿ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    ವಿಕಲ್ಪ (base)
    {
	ಪಕ್ಷ 2: what |= STR2NR_BIN + STR2NR_FORCE; ಭಙ್ಗ;
	ಪಕ್ಷ 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; ಭಙ್ಗ;
	ಪಕ್ಷ 16: what |= STR2NR_HEX + STR2NR_FORCE; ಭಙ್ಗ;
    }
    vim_str2nr(p, ರಿಕ್ತ, ರಿಕ್ತ, what, &n, ರಿಕ್ತ, 0, FALSE, ರಿಕ್ತ);
    // Text after the number is silently ignored.
    ಯದಿ (isneg)
	rettv->vval.v_ಸಂಖ್ಯೆ = -n;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = n;

}

/*
 * "strgetchar()" function
 */
    ಶೂನ್ಯ
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		charidx;
    ಪೂರ್ಣಾಙ್ಕ		byteidx = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_chk(&argvars[0]);
    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str);
    charidx = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;

    ಯಾವತ್ (charidx >= 0 && byteidx < len)
    {
	ಯದಿ (charidx == 0)
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = mb_ptr2char(str + byteidx);
	    ಭಙ್ಗ;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    ಶೂನ್ಯ
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    ಪೂರ್ಣಾಙ್ಕ		start_idx;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (needle == ರಿಕ್ತ || haystack == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಙ್ಕ	    error = FALSE;

	start_idx = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಯದಿ (error || start_idx >= (ಪೂರ್ಣಾಙ್ಕ)STRLEN(haystack))
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((ವರ್ಣ *)haystack, (ವರ್ಣ *)needle);
    ಯದಿ (pos != ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    ಶೂನ್ಯ
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    ಯದಿ (rettv->vval.v_string != ರಿಕ್ತ && tofree == ರಿಕ್ತ)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    ಶೂನ್ಯ
f_strlen(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
strchar_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    ಪೂರ್ಣಾಙ್ಕ			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    ಯಾವತ್ (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = len;
}

/*
 * "strcharlen()" function
 */
    ಶೂನ್ಯ
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    ಶೂನ್ಯ
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಙ್ಕ error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_ಸಂಖ್ಯೆ_as_bool_nr), skipcc);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    ಶೂನ್ಯ
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    varnumber_T countcc = FALSE;
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    ಪೂರ್ಣಾಙ್ಕ			(*func_mb_ptr2char_adv)(char_u **pp);
    ಪೂರ್ಣಾಙ್ಕ			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    ಯಾವತ್ (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	ಯದಿ (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = len;
}

/*
 * "strdisplaywidth()" function
 */
    ಶೂನ್ಯ
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		col = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    s = tv_get_string(&argvars[0]);
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	col = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);

    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    ಶೂನ್ಯ
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    ಶೂನ್ಯ
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		nchar;
    ಪೂರ್ಣಾಙ್ಕ		nbyte = 0;
    ಪೂರ್ಣಾಙ್ಕ		charlen;
    ಪೂರ್ಣಾಙ್ಕ		skipcc = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		slen;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    p = tv_get_string(&argvars[0]);
    slen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);

    nchar = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಯದಿ (!error)
    {
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ;
	    ಯದಿ (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_ಸಂಖ್ಯೆ_as_bool_nr), skipcc);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}

	ಯದಿ (nchar > 0)
	    ಯಾವತ್ (nchar > 0 && nbyte < slen)
	    {
		ಯದಿ (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		ಅನ್ಯಥಾ
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	ಅನ್ಯಥಾ
	    nbyte = nchar;
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
	    ಯಾವತ್ (charlen > 0 && nbyte + len < slen)
	    {
		ಪೂರ್ಣಾಙ್ಕ off = nbyte + len;

		ಯದಿ (off < 0)
		    len += 1;
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (skipcc)
			len += mb_ptr2len(p + off);
		    ಅನ್ಯಥಾ
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	ಅನ್ಯಥಾ
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    ಯದಿ (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (nbyte > slen)
	nbyte = slen;
    ಯದಿ (len < 0)
	len = 0;
    ಅನ್ಯಥಾ ಯದಿ (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    ಶೂನ್ಯ
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		slen;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    p = tv_get_string(&argvars[0]);
    slen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);

    n = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಯದಿ (error)
	len = 0;
    ಅನ್ಯಥಾ ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	len = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
    ಅನ್ಯಥಾ
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    ಯದಿ (n < 0)
    {
	len += n;
	n = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (n > slen)
	n = slen;
    ಯದಿ (len < 0)
	len = 0;
    ಅನ್ಯಥಾ ಯದಿ (n + len > slen)
	len = slen - n;

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಙ್ಕ off;

	// length in characters
	ಪ್ರತಿ (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    ಶೂನ್ಯ
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		haystack_len, end_idx;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (needle == ರಿಕ್ತ || haystack == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given

    haystack_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(haystack);
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], ರಿಕ್ತ);
	ಯದಿ (end_idx < 0)
	    ಪ್ರತ್ಯಾಗಮ;	// can never find a match
    }
    ಅನ್ಯಥಾ
	end_idx = haystack_len;

    ಯದಿ (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((ವರ್ಣ *)rest, (ವರ್ಣ *)needle);
	    ಯದಿ (rest == ರಿಕ್ತ || rest > haystack + end_idx)
		ಭಙ್ಗ;
	    lastmatch = rest;
	}
    }

    ಯದಿ (lastmatch == ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    ಶೂನ್ಯ
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    ಶೂನ್ಯ
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    ಪ್ರತ್ಯಾಗಮ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ);
    ಯದಿ (str == ರಿಕ್ತ || idx < 0)
	ಪ್ರತ್ಯಾಗಮ;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    ಪೂರ್ಣಾಙ್ಕ (*ptr2len)(char_u *);
    ಯದಿ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    ಅನ್ಯಥಾ
	ptr2len = mb_ptr2len;

    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		utf16idx = 0;
    ಪ್ರತಿ (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	ಯದಿ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    ಯದಿ (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_ಸಂಖ್ಯೆ = len;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	utf16idx = len;
	ಪೂರ್ಣಾಙ್ಕ clen = ptr2len(p);
	ಪೂರ್ಣಾಙ್ಕ c = (clen > 1) ? utf_ptr2char(p) : *p;
	ಯದಿ (c > 0xFFFF)
	    len++;
	p += clen;
	ಯದಿ (charidx)
	    idx--;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = utf16idx;
}

/*
 * "tolower(string)" function
 */
    ಶೂನ್ಯ
f_tolower(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    ಶೂನ್ಯ
f_toupper(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    ಶೂನ್ಯ
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*in_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		inlen;
    ಪೂರ್ಣಾಙ್ಕ		fromlen;
    ಪೂರ್ಣಾಙ್ಕ		tolen;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    char_u	*cpstr;
    ಪೂರ್ಣಾಙ್ಕ		cplen;
    ಪೂರ್ಣಾಙ್ಕ		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;
    ಯದಿ (fromstr == ರಿಕ್ತ || tostr == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given
    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 80);

    ಯದಿ (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	ಯದಿ (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    ಪ್ರತ್ಯಾಗಮ;
	}

    // fromstr and tostr have to contain the same number of chars
    ಯಾವತ್ (*in_str != NUL)
    {
	ಯದಿ (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    ಪ್ರತಿ (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		ಯದಿ (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    ಪ್ರತಿ (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			ಯದಿ (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    ಭಙ್ಗ;
			}
		    }
		    ಯದಿ (*p == NUL)	// tostr is shorter than fromstr
			ಗಚ್ಛ error;
		    ಭಙ್ಗ;
		}
		++idx;
	    }

	    ಯದಿ (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		ಪ್ರತಿ (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		ಯದಿ (idx != 0)
		    ಗಚ್ಛ error;
	    }

	    (ಶೂನ್ಯ)ga_grow(&ga, cplen);
	    mch_memmove((ವರ್ಣ *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	ಅನ್ಯಥಾ
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    ಯದಿ (p != ರಿಕ್ತ)
		ga_append(&ga, tostr[p - fromstr]);
	    ಅನ್ಯಥಾ
		ga_append(&ga, *in_str);
	    ++in_str;
	}
    }

    // add a terminating NUL
    (ಶೂನ್ಯ)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_data;
}

/*
 * "trim({expr})" function
 */
    ಶೂನ್ಯ
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*head;
    char_u	*mask = ರಿಕ್ತ;
    char_u	*tail;
    char_u	*prev;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		c1;
    ಪೂರ್ಣಾಙ್ಕ		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    ಯದಿ (head == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_opt_string_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	ಯದಿ (*mask == NUL)
	    mask = ರಿಕ್ತ;

	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ಪೂರ್ಣಾಙ್ಕ	error = 0;

	    // leading or trailing characters to trim
	    dir = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ;
	    ಯದಿ (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}
    }

    ಯದಿ (dir == 0 || dir == 1)
    {
	// Trim leading characters
	ಯಾವತ್ (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    ಯದಿ (mask == ರಿಕ್ತ)
	    {
		ಯದಿ (c1 > ' ' && c1 != 0xa0)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಪ್ರತಿ (p = mask; *p != NUL; MB_PTR_ADV(p))
		    ಯದಿ (c1 == PTR2CHAR(p))
			ಭಙ್ಗ;
		ಯದಿ (*p == NUL)
		    ಭಙ್ಗ;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    ಯದಿ (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	ಪ್ರತಿ (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    ಯದಿ (mask == ರಿಕ್ತ)
	    {
		ಯದಿ (c1 > ' ' && c1 != 0xa0)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಪ್ರತಿ (p = mask; *p != NUL; MB_PTR_ADV(p))
		    ಯದಿ (c1 == PTR2CHAR(p))
			ಭಙ್ಗ;
		ಯದಿ (*p == NUL)
		    ಭಙ್ಗ;
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
    ಸ್ಥಿರ char_u *
uri_decode(char_u *str)
{
    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    ಯದಿ (!decoded)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    char_u	*p = decoded;
    size_t	i = 0;

    ಯಾವತ್ (i < len)
    {
	ಯದಿ (str[i] == '%')
	{
	    ಯದಿ (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		ಯದಿ (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    ಅನ್ಯಥಾ
	    {
		ಪೂರ್ಣಾಙ್ಕ val = hexhex2nr(&str[i + 1]);
		ಯದಿ (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		ಅನ್ಯಥಾ
		{
		    // invalid hex digits following "%"
		    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	ಅನ್ಯಥಾ
	    *p++ = str[i++];
    }

    *p = NUL;

    ಪ್ರತ್ಯಾಗಮ decoded;
}

/*
 * "uri_decode({str})" function
 */
    ಶೂನ್ಯ
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

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
    ಸ್ಥಿರ char_u *
uri_encode(char_u *str)
{
    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    ಯದಿ (encoded == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    char_u *p = encoded;

    ಪ್ರತಿ (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	ಯದಿ (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	ಅನ್ಯಥಾ
	{
	    sprintf((ವರ್ಣ *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    ಪ್ರತ್ಯಾಗಮ encoded;
}

/*
 * "uri_encode({str})" function
 */
    ಶೂನ್ಯ
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

ಸ್ಥಿರ ವರ್ಣ *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ಸ್ಥಿರ varnumber_T
tv_nr(typval_T *tvs, ಪೂರ್ಣಾಙ್ಕ *idxp)
{
    ಪೂರ್ಣಾಙ್ಕ		idx = *idxp - 1;
    varnumber_T	n = 0;
    ಪೂರ್ಣಾಙ್ಕ		err = FALSE;

    ಯದಿ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ಅನ್ಯಥಾ
    {
	++*idxp;
	n = tv_get_ಸಂಖ್ಯೆ_chk(&tvs[idx], &err);
	ಯದಿ (err)
	    n = 0;
    }
    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    ಸ್ಥಿರ ವರ್ಣ *
tv_str(typval_T *tvs, ಪೂರ್ಣಾಙ್ಕ *idxp, char_u **tofree)
{
    ಪೂರ್ಣಾಙ್ಕ		    idx = *idxp - 1;
    ವರ್ಣ	    *s = ರಿಕ್ತ;
    ಸ್ಥಿರ char_u   numbuf[NUMBUFLEN];

    ಯದಿ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ಅನ್ಯಥಾ
    {
	++*idxp;
	ಯದಿ (tofree != ರಿಕ್ತ)
	    s = (ವರ್ಣ *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	ಅನ್ಯಥಾ
	    s = (ವರ್ಣ *)tv_get_string_chk(&tvs[idx]);
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ಸ್ಥಿರ ದ್ವಿಗುಣ
tv_float(typval_T *tvs, ಪೂರ್ಣಾಙ್ಕ *idxp)
{
    ಪೂರ್ಣಾಙ್ಕ		idx = *idxp - 1;
    ದ್ವಿಗುಣ	f = 0;

    ಯದಿ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ಅನ್ಯಥಾ
    {
	++*idxp;
	ಯದಿ (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	ಅನ್ಯಥಾ ಯದಿ (tvs[idx].v_type == VAR_ಸಂಖ್ಯೆ)
	    f = (ದ್ವಿಗುಣ)tvs[idx].vval.v_ಸಂಖ್ಯೆ;
	ಅನ್ಯಥಾ
	    emsg(_(e_expected_float_argument_for_printf));
    }
    ಪ್ರತ್ಯಾಗಮ f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *
infinity_str(ಪೂರ್ಣಾಙ್ಕ positive,
	     ವರ್ಣ fmt_spec,
	     ಪೂರ್ಣಾಙ್ಕ force_sign,
	     ಪೂರ್ಣಾಙ್ಕ space_for_positive)
{
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    ಪೂರ್ಣಾಙ್ಕ idx = positive * (1 + force_sign + force_sign * space_for_positive);

    ಯದಿ (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    ಪ್ರತ್ಯಾಗಮ table[idx];
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
    ಪೂರ್ಣಾಙ್ಕ
vim_snprintf_add(ವರ್ಣ *str, size_t str_m, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
    va_list	ap;
    ಪೂರ್ಣಾಙ್ಕ		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    ಯದಿ (str_m <= len)
	space = 0;
    ಅನ್ಯಥಾ
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    ಪ್ರತ್ಯಾಗಮ str_l;
}

    ಪೂರ್ಣಾಙ್ಕ
vim_snprintf(ವರ್ಣ *str, size_t str_m, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
    va_list	ap;
    ಪೂರ್ಣಾಙ್ಕ		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    ಪ್ರತ್ಯಾಗಮ str_l;
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
vim_snprintf_safelen(ವರ್ಣ *str, size_t str_m, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
    va_list ap;
    ಪೂರ್ಣಾಙ್ಕ	    str_l;

    ಯದಿ (str_m == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, ರಿಕ್ತ);
    va_end(ap);

    ಯದಿ (str_l < 0)
    {
	*str = NUL;
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    ಪೂರ್ಣಾಙ್ಕ
vim_vsnprintf(
    ವರ್ಣ	*str,
    size_t	str_m,
    ಧ್ರುವ ವರ್ಣ	*fmt,
    va_list	ap)
{
    ಪ್ರತ್ಯಾಗಮ vim_vsnprintf_typval(str, str_m, fmt, ap, ರಿಕ್ತ);
}

ಗಣನಾ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
format_typeof(
    ಧ್ರುವ ವರ್ಣ	*type)
{
    // allowed values: \0, h, l, L
    ವರ್ಣ    length_modifier = '\0';

    // current conversion specifier character
    ವರ್ಣ    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    ಯದಿ (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	ಯದಿ (length_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    length_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    ವಿಕಲ್ಪ (fmt_spec)
    {
	ಪಕ್ಷ 'i': fmt_spec = 'd'; ಭಙ್ಗ;
	ಪಕ್ಷ '*': fmt_spec = 'd'; length_modifier = 'h'; ಭಙ್ಗ;
	ಪಕ್ಷ 'D': fmt_spec = 'd'; length_modifier = 'l'; ಭಙ್ಗ;
	ಪಕ್ಷ 'U': fmt_spec = 'u'; length_modifier = 'l'; ಭಙ್ಗ;
	ಪಕ್ಷ 'O': fmt_spec = 'o'; length_modifier = 'l'; ಭಙ್ಗ;
	ಸ್ವಭಾವ: ಭಙ್ಗ;
    }

    // get parameter value, do initial processing
    ವಿಕಲ್ಪ (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    ಪಕ್ಷ '%':
	ಪ್ರತ್ಯಾಗಮ TYPE_PERCENT;

    ಪಕ್ಷ 'c':
	ಪ್ರತ್ಯಾಗಮ TYPE_CHAR;

    ಪಕ್ಷ 's':
    ಪಕ್ಷ 'S':
	ಪ್ರತ್ಯಾಗಮ TYPE_STRING;

    ಪಕ್ಷ 'd': ಪಕ್ಷ 'u':
    ಪಕ್ಷ 'b': ಪಕ್ಷ 'B':
    ಪಕ್ಷ 'o':
    ಪಕ್ಷ 'x': ಪಕ್ಷ 'X':
    ಪಕ್ಷ 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //ಮೌಲ್ಯ

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    ಯದಿ (fmt_spec == 'p')
		ಪ್ರತ್ಯಾಗಮ TYPE_POINTER;
	    ಅನ್ಯಥಾ ಯದಿ (fmt_spec == 'b' || fmt_spec == 'B')
		ಪ್ರತ್ಯಾಗಮ TYPE_UNSIGNEDLONGLONGINT;
	    ಅನ್ಯಥಾ ಯದಿ (fmt_spec == 'd')
	    {
		// signed
		ವಿಕಲ್ಪ (length_modifier)
		{
		ಪಕ್ಷ '\0':
		ಪಕ್ಷ 'h':
		    // char and short arguments are passed as int.
		    ಪ್ರತ್ಯಾಗಮ TYPE_INT;
		ಪಕ್ಷ 'l':
		    ಪ್ರತ್ಯಾಗಮ TYPE_LONGINT;
		ಪಕ್ಷ 'L':
		    ಪ್ರತ್ಯಾಗಮ TYPE_LONGLONGINT;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// unsigned
		ವಿಕಲ್ಪ (length_modifier)
		{
		    ಪಕ್ಷ '\0':
		    ಪಕ್ಷ 'h':
			ಪ್ರತ್ಯಾಗಮ TYPE_UNSIGNEDINT;
		    ಪಕ್ಷ 'l':
			ಪ್ರತ್ಯಾಗಮ TYPE_UNSIGNEDLONGINT;
		    ಪಕ್ಷ 'L':
			ಪ್ರತ್ಯಾಗಮ TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	ಭಙ್ಗ;

    ಪಕ್ಷ 'f':
    ಪಕ್ಷ 'F':
    ಪಕ್ಷ 'e':
    ಪಕ್ಷ 'E':
    ಪಕ್ಷ 'g':
    ಪಕ್ಷ 'G':
	ಪ್ರತ್ಯಾಗಮ TYPE_FLOAT;
    }

    ಪ್ರತ್ಯಾಗಮ TYPE_UNKNOWN;
}

    ಸ್ಥಿರ ವರ್ಣ *
format_typename(
    ಧ್ರುವ ವರ್ಣ  *type)
{
    ವಿಕಲ್ಪ (format_typeof(type))
    {
	ಪಕ್ಷ TYPE_INT:
	    ಪ್ರತ್ಯಾಗಮ typename_int;

	ಪಕ್ಷ TYPE_LONGINT:
	    ಪ್ರತ್ಯಾಗಮ typename_longint;

	ಪಕ್ಷ TYPE_LONGLONGINT:
	    ಪ್ರತ್ಯಾಗಮ typename_longlongint;

	ಪಕ್ಷ TYPE_UNSIGNEDINT:
	    ಪ್ರತ್ಯಾಗಮ typename_unsignedint;

	ಪಕ್ಷ TYPE_UNSIGNEDLONGINT:
	    ಪ್ರತ್ಯಾಗಮ typename_unsignedlongint;

	ಪಕ್ಷ TYPE_UNSIGNEDLONGLONGINT:
	    ಪ್ರತ್ಯಾಗಮ typename_unsignedlonglongint;

	ಪಕ್ಷ TYPE_POINTER:
	    ಪ್ರತ್ಯಾಗಮ _(typename_pointer);

	ಪಕ್ಷ TYPE_PERCENT:
	    ಪ್ರತ್ಯಾಗಮ _(typename_percent);

	ಪಕ್ಷ TYPE_CHAR:
	    ಪ್ರತ್ಯಾಗಮ typename_char;

	ಪಕ್ಷ TYPE_STRING:
	    ಪ್ರತ್ಯಾಗಮ _(typename_string);

	ಪಕ್ಷ TYPE_FLOAT:
	    ಪ್ರತ್ಯಾಗಮ typename_float;
    }

    ಪ್ರತ್ಯಾಗಮ _(typename_unknown);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
adjust_types(
    ಧ್ರುವ ವರ್ಣ ***ap_types,
    ಪೂರ್ಣಾಙ್ಕ arg,
    ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_posarg,
    ಧ್ರುವ ವರ್ಣ *type)
{
    ಯದಿ (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (*ap_types == ರಿಕ್ತ || *ಅಂಕೆ_posarg < arg)
    {
	ಪೂರ್ಣಾಙ್ಕ	    idx;
	ಧ್ರುವ ವರ್ಣ  **new_types;

	ಯದಿ (*ap_types == ರಿಕ್ತ)
	    new_types = ALLOC_CLEAR_MULT(ಧ್ರುವ ವರ್ಣ *, arg);
	ಅನ್ಯಥಾ
	    new_types = vim_realloc((ವರ್ಣ **)*ap_types,
						arg * ಪರಿಮಾಣ(ಧ್ರುವ ವರ್ಣ *));

	ಯದಿ (new_types == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪ್ರತಿ (idx = *ಅಂಕೆ_posarg; idx < arg; ++idx)
	    new_types[idx] = ರಿಕ್ತ;

	*ap_types = new_types;
	*ಅಂಕೆ_posarg = arg;
    }

    ಯದಿ ((*ap_types)[arg - 1] != ರಿಕ್ತ)
    {
	ಯದಿ ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    ಧ್ರುವ ವರ್ಣ *pt = type;
	    ಯದಿ (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    ಯದಿ (pt[0] != '*')
	    {
		ವಿಕಲ್ಪ (pt[0])
		{
		    ಪಕ್ಷ 'd':
		    ಪಕ್ಷ 'i': ಭಙ್ಗ;
		    ಸ್ವಭಾವ:
			semsg(_(e_positional_ಅಂಕೆ_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_ಅಂಕೆ_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
format_overflow_error(ಧ್ರುವ ವರ್ಣ *pstart)
{
    size_t	arglen = 0;
    ವರ್ಣ	*argcopy = ರಿಕ್ತ;
    ಧ್ರುವ ವರ್ಣ	*p = pstart;

    ಯಾವತ್ (VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(ವರ್ಣ, arglen + 1);
    ಯದಿ (argcopy != ರಿಕ್ತ)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	ಮುಕ್ತ(argcopy);
    }
    ಅನ್ಯಥಾ
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_unsigned_int(
    ಧ್ರುವ ವರ್ಣ *pstart,
    ಧ್ರುವ ವರ್ಣ **p,
    unsigned ಪೂರ್ಣಾಙ್ಕ *uj,
    ಪೂರ್ಣಾಙ್ಕ overflow_err)
{
    *uj = **p - '0';
    ++*p;

    ಯಾವತ್ (VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned ಪೂರ್ಣಾಙ್ಕ)(**p - '0');
	++*p;
    }

    ಯದಿ (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	ಯದಿ (overflow_err)
	{
	    format_overflow_error(pstart);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}


    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_fmt_types(
    ಧ್ರುವ ವರ್ಣ  ***ap_types,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_posarg,
    ಧ್ರುವ ವರ್ಣ  *fmt,
    typval_T	*tvs UNUSED
    )
{
    ಧ್ರುವ ವರ್ಣ	*p = fmt;
    ಧ್ರುವ ವರ್ಣ	*arg = ರಿಕ್ತ;

    ಪೂರ್ಣಾಙ್ಕ		any_pos = 0;
    ಪೂರ್ಣಾಙ್ಕ		any_arg = 0;
    ಪೂರ್ಣಾಙ್ಕ		arg_idx;

# define CHECK_POS_ARG ಕುರು { \
    ಯದಿ (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	ಗಚ್ಛ error; \
    } \
} ಯಾವತ್ (0);

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (*p != '%')
	{
	    ಧ್ರುವ ವರ್ಣ    *q = strchr(p + 1, '%');
	    size_t  n = (q == ರಿಕ್ತ) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	ಅನ್ಯಥಾ
	{
	    // allowed values: \0, h, l, L
	    ವರ್ಣ	length_modifier = '\0';

	    // variable for positional arg
	    ಪೂರ್ಣಾಙ್ಕ		pos_arg = -1;
	    ಧ್ರುವ ವರ್ಣ	*ptype = ರಿಕ್ತ;
	    ಧ್ರುವ ವರ್ಣ	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    ಯಾವತ್ (VIM_ISDIGIT(*ptype))
		++ptype;

	    ಯದಿ (*ptype == '$')
	    {
		ಯದಿ (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    ಗಚ್ಛ error;
		}

		// Positional argument
		unsigned ಪೂರ್ಣಾಙ್ಕ uj;

		ಯದಿ (get_unsigned_int(pstart, &p, &uj, tvs != ರಿಕ್ತ) == FAIL)
		    ಗಚ್ಛ error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    ಯಾವತ್ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		ವಿಕಲ್ಪ (*p)
		{
		    ಪಕ್ಷ '0': ಭಙ್ಗ;
		    ಪಕ್ಷ '-': ಭಙ್ಗ;
		    ಪಕ್ಷ '+': ಭಙ್ಗ;
		    ಪಕ್ಷ ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      ಭಙ್ಗ;
		    ಪಕ್ಷ '#': ಭಙ್ಗ;
		    ಪಕ್ಷ '\'': ಭಙ್ಗ;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    ಯದಿ (*(arg = p) == '*')
	    {
		p++;

		ಯದಿ (VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(*p)))
		{
		    // Positional argument field width
		    unsigned ಪೂರ್ಣಾಙ್ಕ uj;

		    ಯದಿ (get_unsigned_int(arg + 1, &p, &uj, tvs != ರಿಕ್ತ) == FAIL)
			ಗಚ್ಛ error;

		    ಯದಿ (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			ಗಚ್ಛ error;
		    }
		    ಅನ್ಯಥಾ
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			ಯದಿ (adjust_types(ap_types, uj, ಅಂಕೆ_posarg, arg) == FAIL)
			    ಗಚ್ಛ error;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ಧ್ರುವ ವರ್ಣ *digstart = p;
		unsigned ಪೂರ್ಣಾಙ್ಕ uj;

		ಯದಿ (get_unsigned_int(digstart, &p, &uj, tvs != ರಿಕ್ತ) == FAIL)
		    ಗಚ್ಛ error;

		ಯದಿ (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    ಗಚ್ಛ error;
		}
	    }

	    // parse precision
	    ಯದಿ (*p == '.')
	    {
		p++;

		ಯದಿ (*(arg = p) == '*')
		{
		    p++;

		    ಯದಿ (VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(*p)))
		    {
			// Parse precision
			unsigned ಪೂರ್ಣಾಙ್ಕ uj;

			ಯದಿ (get_unsigned_int(arg + 1, &p, &uj, tvs != ರಿಕ್ತ) == FAIL)
			    ಗಚ್ಛ error;

			ಯದಿ (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    ಯದಿ (adjust_types(ap_types, uj, ಅಂಕೆ_posarg, arg) == FAIL)
				ಗಚ್ಛ error;
			}
			ಅನ್ಯಥಾ
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    ಗಚ್ಛ error;
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ಧ್ರುವ ವರ್ಣ *digstart = p;
		    unsigned ಪೂರ್ಣಾಙ್ಕ uj;

		    ಯದಿ (get_unsigned_int(digstart, &p, &uj, tvs != ರಿಕ್ತ) == FAIL)
			ಗಚ್ಛ error;

		    ಯದಿ (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			ಗಚ್ಛ error;
		    }
		}
	    }

	    ಯದಿ (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ಯದಿ (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		ಯದಿ (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    ವಿಕಲ್ಪ (*p)
	    {
		// Check for known format specifiers. % is special!
		ಪಕ್ಷ 'i':
		ಪಕ್ಷ '*':
		ಪಕ್ಷ 'd':
		ಪಕ್ಷ 'u':
		ಪಕ್ಷ 'o':
		ಪಕ್ಷ 'D':
		ಪಕ್ಷ 'U':
		ಪಕ್ಷ 'O':
		ಪಕ್ಷ 'x':
		ಪಕ್ಷ 'X':
		ಪಕ್ಷ 'b':
		ಪಕ್ಷ 'B':
		ಪಕ್ಷ 'c':
		ಪಕ್ಷ 's':
		ಪಕ್ಷ 'S':
		ಪಕ್ಷ 'p':
		ಪಕ್ಷ 'f':
		ಪಕ್ಷ 'F':
		ಪಕ್ಷ 'e':
		ಪಕ್ಷ 'E':
		ಪಕ್ಷ 'g':
		ಪಕ್ಷ 'G':
		    ಯದಿ (pos_arg != -1)
		    {
			ಯದಿ (adjust_types(ap_types, pos_arg, ಅಂಕೆ_posarg, ptype) == FAIL)
			    ಗಚ್ಛ error;
		    }
		    ಅನ್ಯಥಾ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    ಭಙ್ಗ;

		ಸ್ವಭಾವ:
		    ಯದಿ (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			ಗಚ್ಛ error;
		    }
	    }

	    ಯದಿ (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    ಪ್ರತಿ (arg_idx = 0; arg_idx < *ಅಂಕೆ_posarg; ++arg_idx)
    {
	ಯದಿ ((*ap_types)[arg_idx] == ರಿಕ್ತ)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    ಗಚ್ಛ error;
	}

# ಯದಿ defined(FEAT_EVAL)
	ಯದಿ (tvs != ರಿಕ್ತ && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    ಗಚ್ಛ error;
	}
# endif
    }

    ಪ್ರತ್ಯಾಗಮ OK;

error:
    vim_free((ವರ್ಣ**)*ap_types);
    *ap_types = ರಿಕ್ತ;
    *ಅಂಕೆ_posarg = 0;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
skip_to_arg(
    ಧ್ರುವ ವರ್ಣ	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    ಪೂರ್ಣಾಙ್ಕ		*arg_idx,
    ಪೂರ್ಣಾಙ್ಕ		*arg_cur,
    ಧ್ರುವ ವರ್ಣ	*fmt)
{
    ಪೂರ್ಣಾಙ್ಕ		arg_min = 0;

    ಯದಿ (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    ಅನ್ಯಥಾ
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    ಪ್ರತಿ (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	ಧ್ರುವ ವರ್ಣ *p;

	ಯದಿ (ap_types == ರಿಕ್ತ || ap_types[*arg_cur] == ರಿಕ್ತ)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	p = ap_types[*arg_cur];

	ಪೂರ್ಣಾಙ್ಕ fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	ವಿಕಲ್ಪ (fmt_type)
	{
	ಪಕ್ಷ TYPE_PERCENT:
	ಪಕ್ಷ TYPE_UNKNOWN:
	    ಭಙ್ಗ;

	ಪಕ್ಷ TYPE_CHAR:
	    va_arg(*ap, ಪೂರ್ಣಾಙ್ಕ);
	    ಭಙ್ಗ;

	ಪಕ್ಷ TYPE_STRING:
	    va_arg(*ap, ವರ್ಣ *);
	    ಭಙ್ಗ;

	ಪಕ್ಷ TYPE_POINTER:
	    va_arg(*ap, ಶೂನ್ಯ *);
	    ಭಙ್ಗ;

	ಪಕ್ಷ TYPE_INT:
	    va_arg(*ap, ಪೂರ್ಣಾಙ್ಕ);
	    ಭಙ್ಗ;

	ಪಕ್ಷ TYPE_LONGINT:
	    va_arg(*ap, long ಪೂರ್ಣಾಙ್ಕ);
	    ಭಙ್ಗ;

	ಪಕ್ಷ TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    ಭಙ್ಗ;

	ಪಕ್ಷ TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned ಪೂರ್ಣಾಙ್ಕ);
	    ಭಙ್ಗ;

	ಪಕ್ಷ TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long ಪೂರ್ಣಾಙ್ಕ);
	    ಭಙ್ಗ;

	ಪಕ್ಷ TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    ಭಙ್ಗ;

	ಪಕ್ಷ TYPE_FLOAT:
	    va_arg(*ap, ದ್ವಿಗುಣ);
	    ಭಙ್ಗ;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    ಪ್ರತ್ಯಾಗಮ;
}

    ಪೂರ್ಣಾಙ್ಕ
vim_vsnprintf_typval(
    ವರ್ಣ	*str,
    size_t	str_m,
    ಧ್ರುವ ವರ್ಣ	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    ಧ್ರುವ ವರ್ಣ	*p = fmt;
    ಪೂರ್ಣಾಙ್ಕ		arg_cur = 0;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_posarg = 0;
    ಪೂರ್ಣಾಙ್ಕ		arg_idx = 1;
    va_list	ap;
    ಧ್ರುವ ವರ್ಣ	**ap_types = ರಿಕ್ತ;

    ಯದಿ (parse_fmt_types(&ap_types, &ಅಂಕೆ_posarg, fmt, tvs) == FAIL)
	ಪ್ರತ್ಯಾಗಮ 0;

    va_copy(ap, ap_start);

    ಯದಿ (p == ರಿಕ್ತ)
	p = "";
    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (*p != '%')
	{
	    ಧ್ರುವ ವರ್ಣ    *q = strchr(p + 1, '%');
	    size_t  n = (q == ರಿಕ್ತ) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    ಯದಿ (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	ಅನ್ಯಥಾ
	{
	    size_t  min_field_width = 0, precision = 0;
	    ಪೂರ್ಣಾಙ್ಕ	    zero_padding = 0, precision_specified = 0, justify_ಎಡ = 0;
	    ಪೂರ್ಣಾಙ್ಕ	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    ಪೂರ್ಣಾಙ್ಕ	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    ವರ್ಣ    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    ವರ್ಣ    tmp[TMP_LEN];

	    // string address in case of string argument
	    ಧ್ರುವ ವರ್ಣ  *str_arg = ರಿಕ್ತ;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    unsigned ವರ್ಣ uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  ಸಂಖ್ಯೆ_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    ವರ್ಣ    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = ರಿಕ್ತ;

	    // variables for positional arg
	    ಪೂರ್ಣಾಙ್ಕ	    pos_arg = -1;
	    ಧ್ರುವ ವರ್ಣ	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    ಯಾವತ್ (VIM_ISDIGIT(*ptype))
		++ptype;

	    ಯದಿ (*ptype == '$')
	    {
		// Positional argument
		ಧ್ರುವ ವರ್ಣ *digstart = p;
		unsigned ಪೂರ್ಣಾಙ್ಕ uj;

		ಯದಿ (get_unsigned_int(digstart, &p, &uj, tvs != ರಿಕ್ತ) == FAIL)
		    ಗಚ್ಛ error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    ಯಾವತ್ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		ವಿಕಲ್ಪ (*p)
		{
		    ಪಕ್ಷ '0': zero_padding = 1; ಭಙ್ಗ;
		    ಪಕ್ಷ '-': justify_ಎಡ = 1; ಭಙ್ಗ;
		    ಪಕ್ಷ '+': force_sign = 1; space_for_positive = 0; ಭಙ್ಗ;
		    ಪಕ್ಷ ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      ಭಙ್ಗ;
		    ಪಕ್ಷ '#': alternate_form = 1; ಭಙ್ಗ;
		    ಪಕ್ಷ '\'': ಭಙ್ಗ;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    ಯದಿ (*p == '*')
	    {
		ಪೂರ್ಣಾಙ್ಕ j;
		ಧ್ರುವ ವರ್ಣ *digstart = p + 1;

		p++;

		ಯದಿ (VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(*p)))
		{
		    // Positional argument field width
		    unsigned ಪೂರ್ಣಾಙ್ಕ uj;

		    ಯದಿ (get_unsigned_int(digstart, &p, &uj, tvs != ರಿಕ್ತ) == FAIL)
			ಗಚ್ಛ error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# ಯದಿ defined(FEAT_EVAL)
		    tvs != ರಿಕ್ತ ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, ಪೂರ್ಣಾಙ್ಕ));

		ಯದಿ (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    ಯದಿ (tvs != ರಿಕ್ತ)
		    {
			format_overflow_error(digstart);
			ಗಚ್ಛ error;
		    }
		    ಅನ್ಯಥಾ
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		ಯದಿ (j >= 0)
		    min_field_width = j;
		ಅನ್ಯಥಾ
		{
		    min_field_width = -j;
		    justify_ಎಡ = 1;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ಧ್ರುವ ವರ್ಣ *digstart = p;
		unsigned ಪೂರ್ಣಾಙ್ಕ uj;

		ಯದಿ (get_unsigned_int(digstart, &p, &uj, tvs != ರಿಕ್ತ) == FAIL)
		    ಗಚ್ಛ error;

		min_field_width = uj;
	    }

	    // parse precision
	    ಯದಿ (*p == '.')
	    {
		p++;
		precision_specified = 1;

		ಯದಿ (VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ಧ್ರುವ ವರ್ಣ *digstart = p;
		    unsigned ಪೂರ್ಣಾಙ್ಕ uj;

		    ಯದಿ (get_unsigned_int(digstart, &p, &uj, tvs != ರಿಕ್ತ) == FAIL)
			ಗಚ್ಛ error;

		    precision = uj;
		}
		ಅನ್ಯಥಾ ಯದಿ (*p == '*')
		{
		    ಪೂರ್ಣಾಙ್ಕ j;
		    ಧ್ರುವ ವರ್ಣ *digstart = p;

		    p++;

		    ಯದಿ (VIM_ISDIGIT((ಪೂರ್ಣಾಙ್ಕ)(*p)))
		    {
			// positional argument
			unsigned ಪೂರ್ಣಾಙ್ಕ uj;

			ಯದಿ (get_unsigned_int(digstart, &p, &uj, tvs != ರಿಕ್ತ) == FAIL)
			    ಗಚ್ಛ error;

			arg_idx = uj;

			++p;
		    }

		    j =
# ಯದಿ defined(FEAT_EVAL)
			tvs != ರಿಕ್ತ ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, ಪೂರ್ಣಾಙ್ಕ));

		    ಯದಿ (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			ಯದಿ (tvs != ರಿಕ್ತ)
			{
			    format_overflow_error(digstart);
			    ಗಚ್ಛ error;
			}
			ಅನ್ಯಥಾ
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    ಯದಿ (j >= 0)
			precision = j;
		    ಅನ್ಯಥಾ
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ಯದಿ (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		ಯದಿ (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    length_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    ವಿಕಲ್ಪ (fmt_spec)
	    {
		ಪಕ್ಷ 'i': fmt_spec = 'd'; ಭಙ್ಗ;
		ಪಕ್ಷ 'D': fmt_spec = 'd'; length_modifier = 'l'; ಭಙ್ಗ;
		ಪಕ್ಷ 'U': fmt_spec = 'u'; length_modifier = 'l'; ಭಙ್ಗ;
		ಪಕ್ಷ 'O': fmt_spec = 'o'; length_modifier = 'l'; ಭಙ್ಗ;
		ಸ್ವಭಾವ: ಭಙ್ಗ;
	    }

# ಯದಿ defined(FEAT_EVAL)
	    ವಿಕಲ್ಪ (fmt_spec)
	    {
		ಪಕ್ಷ 'd': ಪಕ್ಷ 'u': ಪಕ್ಷ 'o': ಪಕ್ಷ 'x': ಪಕ್ಷ 'X':
		    ಯದಿ (tvs != ರಿಕ್ತ && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    ಯದಿ (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    ವಿಕಲ್ಪ (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    ಪಕ್ಷ '%':
	    ಪಕ್ಷ 'c':
	    ಪಕ್ಷ 's':
	    ಪಕ್ಷ 'S':
		str_arg_l = 1;
		ವಿಕಲ್ಪ (fmt_spec)
		{
		ಪಕ್ಷ '%':
		    str_arg = p;
		    ಭಙ್ಗ;

		ಪಕ್ಷ 'c':
		    {
			ಪೂರ್ಣಾಙ್ಕ j;

			j =
# ಯದಿ defined(FEAT_EVAL)
			    tvs != ರಿಕ್ತ ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, ಪೂರ್ಣಾಙ್ಕ));

			// standard demands unsigned char
			uchar_arg = (unsigned ವರ್ಣ)j;
			str_arg = (ವರ್ಣ *)&uchar_arg;
			ಭಙ್ಗ;
		    }

		ಪಕ್ಷ 's':
		ಪಕ್ಷ 'S':
		    str_arg =
# ಯದಿ defined(FEAT_EVAL)
				tvs != ರಿಕ್ತ ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, ವರ್ಣ *));

		    ಯದಿ (str_arg == ರಿಕ್ತ)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    ಅನ್ಯಥಾ ಯದಿ (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    ಅನ್ಯಥಾ ಯದಿ (precision == 0)
			str_arg_l = 0;
		    ಅನ್ಯಥಾ
		    {
			// memchr on HP does not like n > 2^31  !!!
			ಧ್ರುವ ವರ್ಣ *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == ರಿಕ್ತ) ? precision
						      : (size_t)(q - str_arg);
		    }
		    ಯದಿ (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			ಪೂರ್ಣಾಙ್ಕ	cell;

			ಪ್ರತಿ (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    ಯದಿ (precision_specified && i + cell > precision)
				ಭಙ್ಗ;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			ಯದಿ (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    ಭಙ್ಗ;

		ಸ್ವಭಾವ:
		    ಭಙ್ಗ;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ 'd': ಪಕ್ಷ 'u':
	    ಪಕ್ಷ 'b': ಪಕ್ಷ 'B':
	    ಪಕ್ಷ 'o':
	    ಪಕ್ಷ 'x': ಪಕ್ಷ 'X':
	    ಪಕ್ಷ 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //ಮೌಲ್ಯ

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    ಪೂರ್ಣಾಙ್ಕ arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    ಪೂರ್ಣಾಙ್ಕ int_arg = 0;
		    unsigned ಪೂರ್ಣಾಙ್ಕ uint_arg = 0;

		    // only set for length modifier l
		    long ಪೂರ್ಣಾಙ್ಕ long_arg = 0;
		    unsigned long ಪೂರ್ಣಾಙ್ಕ ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    ಶೂನ್ಯ *ptr_arg = ರಿಕ್ತ;

		    ಯದಿ (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# ಯದಿ defined(FEAT_EVAL)
				 tvs != ರಿಕ್ತ ? (ಶೂನ್ಯ *)tv_str(tvs, &arg_idx,
									ರಿಕ್ತ) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, ಶೂನ್ಯ *));

			ಯದಿ (ptr_arg != ರಿಕ್ತ)
			    arg_sign = 1;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# ಯದಿ defined(FEAT_EVAL)
				    tvs != ರಿಕ್ತ ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			ಯದಿ (bin_arg != 0)
			    arg_sign = 1;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (fmt_spec == 'd')
		    {
			// signed
			ವಿಕಲ್ಪ (length_modifier)
			{
			ಪಕ್ಷ '\0':
			ಪಕ್ಷ 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# ಯದಿ defined(FEAT_EVAL)
					tvs != ರಿಕ್ತ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, ಪೂರ್ಣಾಙ್ಕ));

			    ಯದಿ (int_arg > 0)
				arg_sign =  1;
			    ಅನ್ಯಥಾ ಯದಿ (int_arg < 0)
				arg_sign = -1;
			    ಭಙ್ಗ;
			ಪಕ್ಷ 'l':
			    long_arg =
# ಯದಿ defined(FEAT_EVAL)
					tvs != ರಿಕ್ತ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long ಪೂರ್ಣಾಙ್ಕ));

			    ಯದಿ (long_arg > 0)
				arg_sign =  1;
			    ಅನ್ಯಥಾ ಯದಿ (long_arg < 0)
				arg_sign = -1;
			    ಭಙ್ಗ;
			ಪಕ್ಷ 'L':
			    llong_arg =
# ಯದಿ defined(FEAT_EVAL)
					tvs != ರಿಕ್ತ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    ಯದಿ (llong_arg > 0)
				arg_sign =  1;
			    ಅನ್ಯಥಾ ಯದಿ (llong_arg < 0)
				arg_sign = -1;
			    ಭಙ್ಗ;
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			// unsigned
			ವಿಕಲ್ಪ (length_modifier)
			{
			    ಪಕ್ಷ '\0':
			    ಪಕ್ಷ 'h':
				uint_arg =
# ಯದಿ defined(FEAT_EVAL)
					    tvs != ರಿಕ್ತ ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned ಪೂರ್ಣಾಙ್ಕ));

				ಯದಿ (uint_arg != 0)
				    arg_sign = 1;
				ಭಙ್ಗ;
			    ಪಕ್ಷ 'l':
				ulong_arg =
# ಯದಿ defined(FEAT_EVAL)
					    tvs != ರಿಕ್ತ ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long ಪೂರ್ಣಾಙ್ಕ));

				ಯದಿ (ulong_arg != 0)
				    arg_sign = 1;
				ಭಙ್ಗ;
			    ಪಕ್ಷ 'L':
				ullong_arg =
# ಯದಿ defined(FEAT_EVAL)
					    tvs != ರಿಕ್ತ ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				ಯದಿ (ullong_arg != 0)
				    arg_sign = 1;
				ಭಙ್ಗ;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    ಯದಿ (precision_specified)
			zero_padding = 0;
		    ಯದಿ (fmt_spec == 'd')
		    {
			ಯದಿ (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    ಅನ್ಯಥಾ ಯದಿ (alternate_form)
		    {
			ಯದಿ (arg_sign != 0
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
		    ಯದಿ (!precision_specified)
			precision = 1;   // default precision is 1
		    ಯದಿ (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    ಅನ್ಯಥಾ
		    {
			ವರ್ಣ	f[6];
			ಪೂರ್ಣಾಙ್ಕ	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			ಯದಿ (!length_modifier)
			    ;
			ಅನ್ಯಥಾ ಯದಿ (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# ಅನ್ಯಥಾ
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			ಅನ್ಯಥಾ
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			ಯದಿ (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			ಅನ್ಯಥಾ ಯದಿ (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    ವರ್ಣ	    b[8 * ಪರಿಮಾಣ(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    ಕುರು
			    {
				b[ಪರಿಮಾಣ(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    ಯಾವತ್ (bn != 0);

			    memcpy(tmp + str_arg_l, b + ಪರಿಮಾಣ(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			ಅನ್ಯಥಾ ಯದಿ (fmt_spec == 'd')
			{
			    // signed
			    ವಿಕಲ್ಪ (length_modifier)
			    {
			    ಪಕ್ಷ '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       ಭಙ್ಗ;
			    ಪಕ್ಷ 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (short)int_arg);
				      ಭಙ್ಗ;
			    ಪಕ್ಷ 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      ಭಙ್ಗ;
			    ಪಕ್ಷ 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      ಭಙ್ಗ;
			    }
			}
			ಅನ್ಯಥಾ
			{
			    // unsigned
			    ವಿಕಲ್ಪ (length_modifier)
			    {
			    ಪಕ್ಷ '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       ಭಙ್ಗ;
			    ಪಕ್ಷ 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(unsigned short)uint_arg);
				      ಭಙ್ಗ;
			    ಪಕ್ಷ 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      ಭಙ್ಗ;
			    ಪಕ್ಷ 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      ಭಙ್ಗ;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			ಯದಿ (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			ಯದಿ (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t ಅಂಕೆ_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			ಯದಿ (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    ಯದಿ (!precision_specified
					     || precision < ಅಂಕೆ_of_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = ಅಂಕೆ_of_digits + 1;
			    }
			}
			// zero padding to specified precision?
			ಯದಿ (ಅಂಕೆ_of_digits < precision)
			    ಸಂಖ್ಯೆ_of_zeros_to_pad = precision - ಅಂಕೆ_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    ಯದಿ (!justify_ಎಡ && zero_padding)
		    {
			ಪೂರ್ಣಾಙ್ಕ n = (ಪೂರ್ಣಾಙ್ಕ)(min_field_width - (str_arg_l
						    + ಸಂಖ್ಯೆ_of_zeros_to_pad));
			ಯದಿ (n > 0)
			    ಸಂಖ್ಯೆ_of_zeros_to_pad += n;
		    }
		    ಭಙ್ಗ;
		}

	    ಪಕ್ಷ 'f':
	    ಪಕ್ಷ 'F':
	    ಪಕ್ಷ 'e':
	    ಪಕ್ಷ 'E':
	    ಪಕ್ಷ 'g':
	    ಪಕ್ಷ 'G':
		{
		    // Floating point.
		    ದ್ವಿಗುಣ	f;
		    ದ್ವಿಗುಣ	abs_f;
		    ವರ್ಣ	format[40];
		    ಪೂರ್ಣಾಙ್ಕ		l;
		    ಪೂರ್ಣಾಙ್ಕ		remove_trailing_zeroes = FALSE;

		    f =
# ಯದಿ defined(FEAT_EVAL)
			tvs != ರಿಕ್ತ ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, ದ್ವಿಗುಣ));

		    abs_f = f < 0 ? -f : f;

		    ಯದಿ (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			ಯದಿ ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			ಅನ್ಯಥಾ
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    ಯದಿ ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# ಅನ್ಯಥಾ
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
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			ಅನ್ಯಥಾ ಯದಿ (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			ಅನ್ಯಥಾ
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    ಯದಿ (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    ಯದಿ (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				ಯದಿ ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				ಯದಿ (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (ಪೂರ್ಣಾಙ್ಕ)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			ಯದಿ (remove_trailing_zeroes)
			{
			    ಪೂರ್ಣಾಙ್ಕ i;
			    ವರ್ಣ *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    ಯದಿ (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    ಅನ್ಯಥಾ
			    {
				tp = (ವರ್ಣ *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				ಯದಿ (tp != ರಿಕ್ತ)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    ಯದಿ (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    ಯಾವತ್ (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    ಯದಿ (tp != ರಿಕ್ತ && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				ಯಾವತ್ (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			ಅನ್ಯಥಾ
			{
			    ವರ್ಣ *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (ವರ್ಣ *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    ಯದಿ (tp != ರಿಕ್ತ && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    ಯದಿ (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			ಸಂಖ್ಯೆ_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    ಭಙ್ಗ;
		}

	    ಸ್ವಭಾವ:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_ಎಡ = 1;
		min_field_width = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		ಯದಿ (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		ಭಙ್ಗ;
	    }

	    ಯದಿ (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    ಯದಿ (!justify_ಎಡ)
	    {
		// left padding with blank or zero
		ಪೂರ್ಣಾಙ್ಕ pn = (ಪೂರ್ಣಾಙ್ಕ)(min_field_width - (str_arg_l + ಸಂಖ್ಯೆ_of_zeros_to_pad));

		ಯದಿ (pn > 0)
		{
		    ಯದಿ (str_l < str_m)
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
	    ಯದಿ (ಸಂಖ್ಯೆ_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    ಅನ್ಯಥಾ
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		ಪೂರ್ಣಾಙ್ಕ zn = (ಪೂರ್ಣಾಙ್ಕ)zero_padding_insertion_ind;

		ಯದಿ (zn > 0)
		{
		    ಯದಿ (str_l < str_m)
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
		zn = (ಪೂರ್ಣಾಙ್ಕ)ಸಂಖ್ಯೆ_of_zeros_to_pad;
		ಯದಿ (zn > 0)
		{
		    ಯದಿ (str_l < str_m)
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
		ಪೂರ್ಣಾಙ್ಕ sn = (ಪೂರ್ಣಾಙ್ಕ)(str_arg_l - zero_padding_insertion_ind);

		ಯದಿ (sn > 0)
		{
		    ಯದಿ (str_l < str_m)
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
	    ಯದಿ (justify_ಎಡ)
	    {
		// right blank padding to the field width
		ಪೂರ್ಣಾಙ್ಕ pn = (ಪೂರ್ಣಾಙ್ಕ)(min_field_width
				      - (str_arg_l + ಸಂಖ್ಯೆ_of_zeros_to_pad));

		ಯದಿ (pn > 0)
		{
		    ಯದಿ (str_l < str_m)
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

    ಯದಿ (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    ಯದಿ (tvs != ರಿಕ್ತ && tvs[ಅಂಕೆ_posarg != 0 ? ಅಂಕೆ_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((ವರ್ಣ*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)str_l;
}

#endif // PROTO
