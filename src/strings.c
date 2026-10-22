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
    ꦪꦼꦤ꧀ (p != NULL)
	mch_memmove(p, string, len);
    ꦮꦭꦶ p;
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
    ꦪꦼꦤ꧀ (p == NULL)
	ꦮꦭꦶ NULL;

    STRNCPY(p, string, len);
    p[len] = NUL;
    ꦮꦭꦶ p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    ꦮꦭꦶ vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    puur_naa_nka		cc,
    puur_naa_nka		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    puur_naa_nka		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    ꦏꦁꦒꦺꦴ (p = string; *p; p++)
    {
	ꦪꦼꦤ꧀ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    continue;
	}
	ꦪꦼꦤ꧀ (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    ꦪꦼꦤ꧀ (escaped_string == NULL)
	ꦮꦭꦶ NULL;
    p2 = escaped_string;
    ꦏꦁꦒꦺꦴ (p = string; *p; p++)
    {
	ꦪꦼꦤ꧀ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    continue;
	}
	ꦪꦼꦤ꧀ (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    ꦮꦭꦶ escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    puur_naa_nka
csh_like_shell(void)
{
    ꦮꦭꦶ (strstr((var_na *)gettail(p_sh), "csh") != NULL);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    static puur_naa_nka
fish_like_shell(void)
{
    ꦮꦭꦶ (strstr((var_na *)gettail(p_sh), "fish") != NULL);
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
vim_strsave_shellescape(char_u *string, puur_naa_nka do_special, puur_naa_nka do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    puur_naa_nka		csh_like;
    puur_naa_nka		fish_like;
    char_u	*shname;
    puur_naa_nka		powershell;
#ifdef MSWIN
    puur_naa_nka		double_quotes;
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
    ꦏꦁꦒꦺꦴ (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	ꦪꦼꦤ꧀ (double_quotes)
	{
	    ꦪꦼꦤ꧀ (*p == '"')
		++length;		// " -> ""
	}
	ꦭꦶꦪ
#endif
	ꦪꦼꦤ꧀ (*p == '\'')
	{
	    ꦪꦼꦤ꧀ (powershell)
		length += 2;		// ' => ''
	    ꦭꦶꦪ
		length += 3;		// ' => '\''
	}
	ꦪꦼꦤ꧀ ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    ꦪꦼꦤ꧀ (csh_like && do_special)
		++length;		// insert backslash
	}
	ꦪꦼꦤ꧀ (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	ꦪꦼꦤ꧀ (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    ꦪꦼꦤ꧀ (escaped_string != NULL)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	ꦪꦼꦤ꧀ (double_quotes)
	    *d++ = '"';
	ꦭꦶꦪ
#endif
	    *d++ = '\'';

	ꦏꦁꦒꦺꦴ (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    ꦪꦼꦤ꧀ (double_quotes)
	    {
		ꦪꦼꦤ꧀ (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    continue;
		}
	    }
	    ꦭꦶꦪ
#endif
	    ꦪꦼꦤ꧀ (*p == '\'')
	    {
		ꦪꦼꦤ꧀ (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		ꦭꦶꦪ
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		continue;
	    }
	    ꦪꦼꦤ꧀ ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		ꦪꦼꦤ꧀ (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		continue;
	    }
	    ꦪꦼꦤ꧀ (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		continue;
	    }
	    ꦪꦼꦤ꧀ (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		continue;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	ꦪꦼꦤ꧀ (double_quotes)
	    *d++ = '"';
	ꦭꦶꦪ
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    ꦮꦭꦶ escaped_string;
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
    ꦮꦭꦶ p1;
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
    ꦮꦭꦶ p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    void
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    puur_naa_nka	    c;

    ꦪꦼꦤ꧀ (p == NULL)
	ꦮꦭꦶ;

    p2 = p;
    ꦱꦼꦢꦼꦭ ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#ꦪꦼꦤ꧀ defined(FEAT_EVAL) || defined(FEAT_SPELL)
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

    ꦪꦼꦤ꧀ (res != NULL)
	ꦱꦼꦢꦼꦭ (*p != NUL)
	{
	    puur_naa_nka		l;

	    ꦪꦼꦤ꧀ (enc_utf8)
	    {
		puur_naa_nka	c, uc;
		puur_naa_nka	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ꦪꦼꦤ꧀ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		ꦪꦼꦤ꧀ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ꦪꦼꦤ꧀ (s == NULL)
		    {
			vim_free(res);
			ꦮꦭꦶ NULL;
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
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ꦭꦶꦪ
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    ꦮꦭꦶ res;
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

    ꦪꦼꦤ꧀ (res != NULL)
	ꦱꦼꦢꦼꦭ (*p != NUL)
	{
	    puur_naa_nka		l;

	    ꦪꦼꦤ꧀ (enc_utf8)
	    {
		puur_naa_nka	c, lc;
		puur_naa_nka	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ꦪꦼꦤ꧀ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		ꦪꦼꦤ꧀ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ꦪꦼꦤ꧀ (s == NULL)
		    {
			vim_free(res);
			ꦮꦭꦶ NULL;
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
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ꦭꦶꦪ
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    ꦮꦭꦶ res;
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
    ꦱꦼꦢꦼꦭ (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
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

    ꦪꦼꦤ꧀ (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    ꦭꦶꦪ
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(var_na *s, size_t maxlen)
{
    size_t i;
    ꦏꦁꦒꦺꦴ (i = 0; i < maxlen; ++i)
	ꦪꦼꦤ꧀ (s[i] == NUL)
	    bha_nga;
    ꦮꦭꦶ i;
}

#ꦪꦼꦤ꧀ !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    puur_naa_nka
vim_stricmp(var_na *s1, var_na *s2)
{
    puur_naa_nka		i;

    ꦏꦁꦒꦺꦴ (;;)
    {
	i = (puur_naa_nka)TOLOWER_LOC(*s1) - (puur_naa_nka)TOLOWER_LOC(*s2);
	ꦪꦼꦤ꧀ (i != 0)
	    ꦮꦭꦶ i;			    // this character different
	ꦪꦼꦤ꧀ (*s1 == NUL)
	    bha_nga;			    // strings match until NUL
	++s1;
	++s2;
    }
    ꦮꦭꦶ 0;				    // strings match
}
#endif

#ꦪꦼꦤ꧀ !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    puur_naa_nka
vim_strnicmp(var_na *s1, var_na *s2, size_t len)
{
    puur_naa_nka		i;

    ꦱꦼꦢꦼꦭ (len > 0)
    {
	i = (puur_naa_nka)TOLOWER_LOC(*s1) - (puur_naa_nka)TOLOWER_LOC(*s2);
	ꦪꦼꦤ꧀ (i != 0)
	    ꦮꦭꦶ i;			    // this character different
	ꦪꦼꦤ꧀ (*s1 == NUL)
	    bha_nga;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ꦮꦭꦶ 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    puur_naa_nka
vim_strnicmp_asc(var_na *s1, var_na *s2, size_t len)
{
    puur_naa_nka                i = 0;

    ꦱꦼꦢꦼꦭ (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	ꦪꦼꦤ꧀ (i != 0)
	    bha_nga;			// this character is different
	ꦪꦼꦤ꧀ (*s1 == NUL)
	    bha_nga;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ꦮꦭꦶ i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, puur_naa_nka c)
{
    char_u	*p;
    puur_naa_nka		b;

    p = string;
    ꦪꦼꦤ꧀ (enc_utf8 && c > 0 && c < 0x80)
	ꦮꦭꦶ vim_strbyte(string, c);
    ꦪꦼꦤ꧀ (enc_utf8 && c >= 0x80)
    {
	ꦱꦼꦢꦼꦭ (*p != NUL)
	{
	    puur_naa_nka l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    ꦪꦼꦤ꧀ (utf_ptr2char(p) == c && l > 1)
		ꦮꦭꦶ p;
	    p += l;
	}
	ꦮꦭꦶ NULL;
    }
    ꦪꦼꦤ꧀ (enc_dbcs != 0 && c > 255)
    {
	puur_naa_nka	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	ꦱꦼꦢꦼꦭ ((b = *p) != NUL)
	{
	    ꦪꦼꦤ꧀ (b == c && p[1] == n2)
		ꦮꦭꦶ p;
	    p += (*mb_ptr2len)(p);
	}
	ꦮꦭꦶ NULL;
    }
    ꦪꦼꦤ꧀ (has_mbyte)
    {
	ꦱꦼꦢꦼꦭ ((b = *p) != NUL)
	{
	    ꦪꦼꦤ꧀ (b == c)
		ꦮꦭꦶ p;
	    p += (*mb_ptr2len)(p);
	}
	ꦮꦭꦶ NULL;
    }
    ꦱꦼꦢꦼꦭ ((b = *p) != NUL)
    {
	ꦪꦼꦤ꧀ (b == c)
	    ꦮꦭꦶ p;
	++p;
    }
    ꦮꦭꦶ NULL;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, puur_naa_nka c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    ꦪꦼꦤ꧀ (c <= 0 || c > 255)
	ꦮꦭꦶ NULL;
    ꦮꦭꦶ (char_u *)strchr((var_na *)string, c);
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
vim_strrchr(char_u *string, puur_naa_nka c)
{
    char_u	*retval = NULL;
    char_u	*p = string;

    ꦱꦼꦢꦼꦭ (*p)
    {
	ꦪꦼꦤ꧀ (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    ꦮꦭꦶ retval;
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
    ꦱꦼꦢꦼꦭ (*s)
    {
	ꦪꦼꦤ꧀ (vim_strchr(charset, *s) != NULL)
	    ꦮꦭꦶ s;
	MB_PTR_ADV(s);
    }
    ꦮꦭꦶ NULL;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
static puur_naa_nka sort_compare(const void *s1, const void *s2);

    static puur_naa_nka
sort_compare(const void *s1, const void *s2)
{
    ꦮꦭꦶ STRCMP(*(var_na **)s1, *(var_na **)s2);
}

    void
sort_strings(
    char_u	**files,
    puur_naa_nka		count)
{
    qsort((void *)files, (size_t)count, parimaa_na(char_u *), sort_compare);
}

#ꦪꦼꦤ꧀ defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    puur_naa_nka
has_non_ascii(char_u *s)
{
    char_u	*p;

    ꦪꦼꦤ꧀ (s != NULL)
	ꦏꦁꦒꦺꦴ (p = s; *p != NUL; ++p)
	    ꦪꦼꦤ꧀ (*p >= 128)
		ꦮꦭꦶ TRUE;
    ꦮꦭꦶ FALSE;
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
    ꦪꦼꦤ꧀ (dest == NULL)
	ꦮꦭꦶ NULL;
    ꦪꦼꦤ꧀ (str1 == NULL)
	*dest = NUL;
    ꦭꦶꦪ
	STRCPY(dest, str1);
    ꦪꦼꦤ꧀ (str2 != NULL)
	STRCPY(dest + l, str2);
    ꦮꦭꦶ dest;
}

#ꦪꦼꦤ꧀ defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    ꦪꦼꦤ꧀ (rev == NULL)
	ꦮꦭꦶ NULL;

    ꦏꦁꦒꦺꦴ (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	ꦪꦼꦤ꧀ (has_mbyte)
	{
	    puur_naa_nka mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	ꦭꦶꦪ
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    ꦮꦭꦶ rev;
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, puur_naa_nka function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    ꦪꦼꦤ꧀ (str != NULL)
    {
	len += (unsigned)STRLEN(str);
	ꦏꦁꦒꦺꦴ (p = str; *p != NUL; MB_PTR_ADV(p))
	    ꦪꦼꦤ꧀ (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    ꦪꦼꦤ꧀ (r == NULL)
	ꦮꦭꦶ NULL;

    ꦪꦼꦤ꧀ (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    ꦭꦶꦪ
	*r++ = '\'';
    ꦪꦼꦤ꧀ (str != NULL)
	ꦏꦁꦒꦺꦴ (p = str; *p != NUL; )
	{
	    ꦪꦼꦤ꧀ (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    ꦪꦼꦤ꧀ (function)
	*r++ = ')';
    *r++ = NUL;
    ꦮꦭꦶ s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_count(char_u *haystack, char_u *needle, puur_naa_nka ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    ꦪꦼꦤ꧀ (p == NULL || needle == NULL || *needle == NUL)
	ꦮꦭꦶ 0;

    size_t  needlelen = STRLEN(needle);
    ꦪꦼꦤ꧀ (ic)
    {
	ꦱꦼꦢꦼꦭ (*p != NUL)
	{
	    ꦪꦼꦤ꧀ (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    ꦭꦶꦪ
		MB_PTR_ADV(p);
	}
    }
    ꦭꦶꦪ
	ꦱꦼꦢꦼꦭ ((next = (char_u *)strstr((var_na *)p, (var_na *)needle)) != NULL)
	{
	    ++n;
	    p = next + needlelen;
	}

    ꦮꦭꦶ n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    static puur_naa_nka
copy_first_char_to_tv(char_u *input, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    puur_naa_nka		len;

    ꦪꦼꦤ꧀ (input == NULL || output == NULL)
	ꦮꦭꦶ -1;

    len = has_mbyte ? mb_ptr2len(input) : 1;
    STRNCPY(buf, input, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    ꦮꦭꦶ output->vval.v_string == NULL ? -1 : len;
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
    puur_naa_nka		len = 0;
    puur_naa_nka		idx = 0;
    puur_naa_nka		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    ga_init2(&ga, parimaa_na(var_na), 80);
    ꦏꦁꦒꦺꦴ (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	ꦪꦼꦤ꧀ (len < 0)
	    bha_nga;

	set_vim_var_nr(VV_KEY, idx);
	ꦪꦼꦤ꧀ (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    bha_nga;
	}
	ꦪꦼꦤ꧀ (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    ꦪꦼꦤ꧀ (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		bha_nga;
	    }
	    ꦭꦶꦪ
		ga_concat(&ga, newtv.vval.v_string);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    ꦪꦼꦤ꧀ (fc != NULL)
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
    puur_naa_nka		len;
    typval_T	argv[3];
    puur_naa_nka		r;
    puur_naa_nka		called_emsg_start = called_emsg;
    funccall_T	*fc;

    ꦪꦼꦤ꧀ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ꦪꦼꦤ꧀ (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "String");
	    ꦮꦭꦶ;
	}
	len = copy_first_char_to_tv(p, rettv);
	ꦪꦼꦤ꧀ (len < 0)
	    ꦮꦭꦶ;
	p += len;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (check_for_string_arg(argvars, 2) == FAIL)
	ꦮꦭꦶ;
    ꦭꦶꦪ
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    ꦏꦁꦒꦺꦴ ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	ꦪꦼꦤ꧀ (len < 0)
	{
	    *rettv = argv[0];
	    bha_nga;
	}

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	ꦪꦼꦤ꧀ (r == FAIL || called_emsg != called_emsg_start)
	    bha_nga;
    }

    ꦪꦼꦤ꧀ (fc != NULL)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    static void
byteidx_common(typval_T *argvars, typval_T *rettv, puur_naa_nka comp)
{
    rettv->vval.v_number = -1;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ꦮꦭꦶ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    ꦪꦼꦤ꧀ (str == NULL || idx < 0)
	ꦮꦭꦶ;

    varnumber_T	utf16idx = FALSE;
    ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
    {
	puur_naa_nka error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	ꦪꦼꦤ꧀ (error)
	    ꦮꦭꦶ;
	ꦪꦼꦤ꧀ (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), utf16idx);
	    ꦮꦭꦶ;
	}
    }

    puur_naa_nka (*ptr2len)(char_u *);
    ꦪꦼꦤ꧀ (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    ꦭꦶꦪ
	ptr2len = mb_ptr2len;

    char_u *t = str;
    ꦏꦁꦒꦺꦴ ( ; idx > 0; idx--)
    {
	ꦪꦼꦤ꧀ (*t == NUL)		// EOL reached
	    ꦮꦭꦶ;
	ꦪꦼꦤ꧀ (utf16idx)
	{
	    puur_naa_nka clen = ptr2len(t);
	    puur_naa_nka c = (clen > 1) ? utf_ptr2char(t) : *t;
	    ꦪꦼꦤ꧀ (c > 0xFFFF)
		idx--;
	    ꦪꦼꦤ꧀ (idx > 0)
		t += clen;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (idx > 0)
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

    ꦪꦼꦤ꧀ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	ꦮꦭꦶ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    ꦪꦼꦤ꧀ (str == NULL || idx < 0)
	ꦮꦭꦶ;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ꦪꦼꦤ꧀ (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    puur_naa_nka (*ptr2len)(char_u *);
    ꦪꦼꦤ꧀ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    ꦭꦶꦪ
	ptr2len = mb_ptr2len;

    char_u	*p;
    puur_naa_nka		len;
    ꦏꦁꦒꦺꦴ (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	ꦪꦼꦤ꧀ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    ꦪꦼꦤ꧀ (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    ꦮꦭꦶ;
	}
	ꦪꦼꦤ꧀ (utf16idx)
	{
	    idx--;
	    puur_naa_nka clen = ptr2len(p);
	    puur_naa_nka c = (clen > 1) ? utf_ptr2char(p) : *p;
	    ꦪꦼꦤ꧀ (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_number = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    static puur_naa_nka
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ꦪꦼꦤ꧀ (convert_setup(&vimconv, from, to) == FAIL)
	ꦮꦭꦶ FAIL;
    vimconv.vc_fail = TRUE;
    ꦪꦼꦤ꧀ (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	ꦪꦼꦤ꧀ (ret->string == NULL)
	    ret->length = 0;
	ꦭꦶꦪ
	    ret->length = str->length;
    }
    ꦭꦶꦪ
    {
	puur_naa_nka len = (puur_naa_nka)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, NULL, NULL);

    ꦮꦭꦶ (ret->string == NULL) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    static void
blob_from_string(char_u *str, blob_T *blob)
{
    puur_naa_nka	    len = (puur_naa_nka)STRLEN(str);
    char_u  *dest;

    ꦪꦼꦤ꧀ (len == 0)
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ (ga_grow(&blob->bv_ga, len) == FAIL)
	ꦮꦭꦶ;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < len; ++i)
	ꦪꦼꦤ꧀ (dest[i] == NL)
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
    static puur_naa_nka
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	line_len;

    ꦪꦼꦤ꧀ (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	ꦮꦭꦶ (ret->string == NULL) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    line_len = (nl == NULL) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(line_len + 1);
    ꦪꦼꦤ꧀ (ret->string == NULL)
    {
	ret->length = 0;
	ꦮꦭꦶ FAIL;
    }
    ꦪꦼꦤ꧀ (line_len > 0)
	mch_memmove(ret->string, src, (size_t)line_len);
    ret->string[line_len] = NUL;
    ret->length = (size_t)line_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    ꦏꦁꦒꦺꦴ (long i = 0; i < line_len; i++)
	ꦪꦼꦤ꧀ (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + line_len + (nl != NULL ? 1 : 0);
    ꦮꦭꦶ OK;
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
    ꦪꦼꦤ꧀ (from_encoding_raw == NULL)
	ꦮꦭꦶ NULL;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    ꦱꦼꦢꦼꦭ (*s != NUL)
    {
	ꦪꦼꦤ꧀ (*s == '_')
	    *pe++ = '-';
	ꦭꦶꦪ
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    ꦪꦼꦤ꧀ ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    ꦮꦭꦶ from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    static void
append_converted_string_to_list(
	string_T *converted,
	puur_naa_nka validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    ꦪꦼꦤ꧀ (converted->string != NULL)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	ꦱꦼꦢꦼꦭ (p < end)
	{
	    string_T	line;
	    char_u	*line_start = p;

	    ꦱꦼꦢꦼꦭ (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    line.length = (size_t)(p - line_start);
	    line.string = vim_strnsave(line_start, line.length);
	    ꦪꦼꦤ꧀ (line.string != NULL)
	    {
		ꦪꦼꦤ꧀ (validate_utf8 && !utf_valid_string(line.string, NULL))
		{
		    vim_free(line.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    ꦮꦭꦶ; // Stop processing
		}
		ꦪꦼꦤ꧀ (list_append_string(list, line.string, (puur_naa_nka)line.length) == FAIL)
		{
		    vim_free(line.string);
		    vim_free(converted->string);
		    ꦮꦭꦶ; // Stop processing on append failure
		}
		vim_free(line.string);
	    }
	    ꦭꦶꦪ
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		ꦮꦭꦶ;
	    }

	    ꦪꦼꦤ꧀ (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    ꦭꦶꦪ
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    static puur_naa_nka
append_validated_line_to_list(string_T *line, puur_naa_nka validate_utf8, list_T *list)
{
    ꦪꦼꦤ꧀ (validate_utf8 && !utf_valid_string(line->string, NULL))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(line->string);
	ꦮꦭꦶ FAIL;
    }

    puur_naa_nka ret = list_append_string(list, line->string, (puur_naa_nka)line->length);
    vim_free(line->string);
    ꦮꦭꦶ ret;
}

    void
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    puur_naa_nka		blen;
    long	idx;
    puur_naa_nka		validate_utf8 = FALSE;

    ꦪꦼꦤ꧀ (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (rettv_list_alloc(rettv) == FAIL)
	ꦮꦭꦶ;

    blob = argvars->vval.v_blob;
    ꦪꦼꦤ꧀ (blob == NULL)
	ꦮꦭꦶ;
    blen = blob_len(blob);

    char_u	*from_encoding = NULL;
    char_u	*from_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ꦪꦼꦤ꧀ (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ꦪꦼꦤ꧀ (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		ꦪꦼꦤ꧀ (from_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    ꦮꦭꦶ;
		}
	    }
	}
    }

    ꦪꦼꦤ꧀ (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    ꦪꦼꦤ꧀ (from_encoding != NULL && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    puur_naa_nka from_prop = 0;
    ꦪꦼꦤ꧀ (from_encoding != NULL)
	from_prop = enc_canon_props(from_encoding);
    ꦪꦼꦤ꧀ (from_encoding != NULL && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	puur_naa_nka nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	ꦪꦼꦤ꧀ (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    ꦪꦼꦤ꧀ (blen > 0)
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
	ꦪꦼꦤ꧀ (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    goto done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	puur_naa_nka len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, NULL);
	converted.length = len;
	convert_setup(&vimconv, NULL, NULL);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    ꦭꦶꦪ
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	ꦱꦼꦢꦼꦭ (idx < blen)
	{
	    string_T	str;

	    ꦪꦼꦤ꧀ (string_from_blob(blob, &idx, &str) != OK)
		bha_nga;

	    ꦪꦼꦤ꧀ (from_encoding != NULL)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		puur_naa_nka	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		ꦪꦼꦤ꧀ (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    ꦪꦼꦤ꧀ (append_validated_line_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		goto done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    ꦪꦼꦤ꧀ (blen > 0 && blob_get(blob, blen - 1) == NL)
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

    ꦪꦼꦤ꧀ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (rettv_blob_alloc(rettv) == FAIL)
	ꦮꦭꦶ;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    ꦪꦼꦤ꧀ (list == NULL)
	ꦮꦭꦶ;

    char_u	*to_encoding = NULL;
    char_u	*to_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ꦪꦼꦤ꧀ (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ꦪꦼꦤ꧀ (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		ꦪꦼꦤ꧀ (to_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    ꦮꦭꦶ;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    puur_naa_nka to_prop = 0;
    ꦪꦼꦤ꧀ (to_encoding != NULL)
	to_prop = enc_canon_props(to_encoding);
    ꦪꦼꦤ꧀ (to_encoding != NULL && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    char_u *s;

	    ꦪꦼꦤ꧀ (li->li_tv.v_type != VAR_STRING)
		continue;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    ꦪꦼꦤ꧀ (li != list->lv_first)
		ga_append(&str_ga, NL);
	    ꦪꦼꦤ꧀ (s != NULL && *s != NUL)
	    {
		puur_naa_nka slen = (puur_naa_nka)STRLEN(s);

		ꦪꦼꦤ꧀ (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    goto done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	ꦪꦼꦤ꧀ (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    ꦪꦼꦤ꧀ (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    vimconv.vc_fail = TRUE;

	    puur_naa_nka		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, NULL);
	    convert_setup(&vimconv, NULL, NULL);
	    ga_clear(&str_ga);

	    ꦪꦼꦤ꧀ (converted == NULL)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    ꦪꦼꦤ꧀ (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	ꦭꦶꦪ
	    ga_clear(&str_ga);
    }
    ꦭꦶꦪ
    {
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    ꦪꦼꦤ꧀ (li->li_tv.v_type != VAR_STRING)
		continue;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    ꦪꦼꦤ꧀ (str.string == NULL)
		STR_LITERAL_SET(str, "");
	    ꦭꦶꦪ
		str.length = STRLEN(str.string);

	    ꦪꦼꦤ꧀ (to_encoding != NULL)
	    {
		puur_naa_nka	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		ꦪꦼꦤ꧀ (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    ꦪꦼꦤ꧀ (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    ꦪꦼꦤ꧀ (to_encoding != NULL)
		vim_free(str.string);
	}
    }

done:
    ꦪꦼꦤ꧀ (to_encoding != NULL)
	vim_free(to_encoding);
    ꦪꦼꦤ꧀ (to_encoding_raw != NULL)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    void
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    puur_naa_nka		utf8 = FALSE;

    ꦪꦼꦤ꧀ (rettv_list_alloc(rettv) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (puur_naa_nka)tv_get_bool_chk(&argvars[1], NULL);

    p = tv_get_string(&argvars[0]);

    ꦪꦼꦤ꧀ (has_mbyte || utf8)
    {
	puur_naa_nka (*ptr2len)(char_u *);
	puur_naa_nka (*ptr2char)(char_u *);

	ꦪꦼꦤ꧀ (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	ꦭꦶꦪ
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	ꦏꦁꦒꦺꦴ ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_number(rettv->vval.v_list, (*ptr2char)(p));
    }
    ꦭꦶꦪ
	ꦏꦁꦒꦺꦴ ( ; *p != NUL; ++p)
	    list_append_number(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    void
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    puur_naa_nka		base = 10;
    char_u	*p;
    varnumber_T	n;
    puur_naa_nka		what = 0;
    puur_naa_nka		isneg;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (puur_naa_nka)tv_get_number(&argvars[1]);
	ꦪꦼꦤ꧀ (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    ꦮꦭꦶ;
	}
	ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    ꦪꦼꦤ꧀ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    switch (base)
    {
	pak_sa 2: what |= STR2NR_BIN + STR2NR_FORCE; bha_nga;
	pak_sa 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; bha_nga;
	pak_sa 16: what |= STR2NR_HEX + STR2NR_FORCE; bha_nga;
    }
    vim_str2nr(p, NULL, NULL, what, &n, NULL, 0, FALSE, NULL);
    // Text after the number is silently ignored.
    ꦪꦼꦤ꧀ (isneg)
	rettv->vval.v_number = -n;
    ꦭꦶꦪ
	rettv->vval.v_number = n;

}

/*
 * "strgetchar()" function
 */
    void
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    puur_naa_nka		len;
    puur_naa_nka		error = FALSE;
    puur_naa_nka		charidx;
    puur_naa_nka		byteidx = 0;

    rettv->vval.v_number = -1;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    str = tv_get_string_chk(&argvars[0]);
    ꦪꦼꦤ꧀ (str == NULL)
	ꦮꦭꦶ;
    len = (puur_naa_nka)STRLEN(str);
    charidx = (puur_naa_nka)tv_get_number_chk(&argvars[1], &error);
    ꦪꦼꦤ꧀ (error)
	ꦮꦭꦶ;

    ꦱꦼꦢꦼꦭ (charidx >= 0 && byteidx < len)
    {
	ꦪꦼꦤ꧀ (charidx == 0)
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
    puur_naa_nka		start_idx;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	ꦮꦭꦶ;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_number = -1;
    ꦪꦼꦤ꧀ (needle == NULL || haystack == NULL)
	ꦮꦭꦶ;		// type error; errmsg already given

    ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
    {
	puur_naa_nka	    error = FALSE;

	start_idx = (puur_naa_nka)tv_get_number_chk(&argvars[2], &error);
	ꦪꦼꦤ꧀ (error || start_idx >= (puur_naa_nka)STRLEN(haystack))
	    ꦮꦭꦶ;
	ꦪꦼꦤ꧀ (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((var_na *)haystack, (var_na *)needle);
    ꦪꦼꦤ꧀ (pos != NULL)
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
    ꦪꦼꦤ꧀ (rettv->vval.v_string != NULL && tofree == NULL)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    void
f_strlen(typval_T *argvars, typval_T *rettv)
{
    ꦪꦼꦤ꧀ (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    rettv->vval.v_number = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    static void
strchar_common(typval_T *argvars, typval_T *rettv, puur_naa_nka skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    puur_naa_nka			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    ꦱꦼꦢꦼꦭ (*s != NUL)
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
    ꦪꦼꦤ꧀ (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    void
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN)
    {
	puur_naa_nka error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	ꦪꦼꦤ꧀ (error)
	    ꦮꦭꦶ;
	ꦪꦼꦤ꧀ (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), skipcc);
	    ꦮꦭꦶ;
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

    ꦪꦼꦤ꧀ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	ꦮꦭꦶ;

    varnumber_T countcc = FALSE;
    ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    puur_naa_nka			(*func_mb_ptr2char_adv)(char_u **pp);
    puur_naa_nka			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    ꦱꦼꦢꦼꦭ (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	ꦪꦼꦤ꧀ (ch > 0xFFFF)
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
    puur_naa_nka		col = 0;

    rettv->vval.v_number = -1;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    s = tv_get_string(&argvars[0]);
    ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN)
	col = (puur_naa_nka)tv_get_number(&argvars[1]);

    rettv->vval.v_number = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    void
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    ꦪꦼꦤ꧀ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

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
    puur_naa_nka		nchar;
    puur_naa_nka		nbyte = 0;
    puur_naa_nka		charlen;
    puur_naa_nka		skipcc = FALSE;
    puur_naa_nka		len = 0;
    puur_naa_nka		slen;
    puur_naa_nka		error = FALSE;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ꦮꦭꦶ;

    p = tv_get_string(&argvars[0]);
    slen = (puur_naa_nka)STRLEN(p);

    nchar = (puur_naa_nka)tv_get_number_chk(&argvars[1], &error);
    ꦪꦼꦤ꧀ (!error)
    {
	ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    ꦪꦼꦤ꧀ (error)
		ꦮꦭꦶ;
	    ꦪꦼꦤ꧀ (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_number_as_bool_nr), skipcc);
		ꦮꦭꦶ;
	    }
	}

	ꦪꦼꦤ꧀ (nchar > 0)
	    ꦱꦼꦢꦼꦭ (nchar > 0 && nbyte < slen)
	    {
		ꦪꦼꦤ꧀ (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		ꦭꦶꦪ
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	ꦭꦶꦪ
	    nbyte = nchar;
	ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (puur_naa_nka)tv_get_number(&argvars[2]);
	    ꦱꦼꦢꦼꦭ (charlen > 0 && nbyte + len < slen)
	    {
		puur_naa_nka off = nbyte + len;

		ꦪꦼꦤ꧀ (off < 0)
		    len += 1;
		ꦭꦶꦪ
		{
		    ꦪꦼꦤ꧀ (skipcc)
			len += mb_ptr2len(p + off);
		    ꦭꦶꦪ
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	ꦭꦶꦪ
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    ꦪꦼꦤ꧀ (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (nbyte > slen)
	nbyte = slen;
    ꦪꦼꦤ꧀ (len < 0)
	len = 0;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (nbyte + len > slen)
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
    puur_naa_nka		n;
    puur_naa_nka		len;
    puur_naa_nka		slen;
    puur_naa_nka		error = FALSE;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ꦮꦭꦶ;

    p = tv_get_string(&argvars[0]);
    slen = (puur_naa_nka)STRLEN(p);

    n = (puur_naa_nka)tv_get_number_chk(&argvars[1], &error);
    ꦪꦼꦤ꧀ (error)
	len = 0;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
	len = (puur_naa_nka)tv_get_number(&argvars[2]);
    ꦭꦶꦪ
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    ꦪꦼꦤ꧀ (n < 0)
    {
	len += n;
	n = 0;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (n > slen)
	n = slen;
    ꦪꦼꦤ꧀ (len < 0)
	len = 0;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (n + len > slen)
	len = slen - n;

    ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	puur_naa_nka off;

	// length in characters
	ꦏꦁꦒꦺꦴ (off = n; off < slen && len > 0; --len)
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
    puur_naa_nka		haystack_len, end_idx;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	ꦮꦭꦶ;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_number = -1;
    ꦪꦼꦤ꧀ (needle == NULL || haystack == NULL)
	ꦮꦭꦶ;		// type error; errmsg already given

    haystack_len = (puur_naa_nka)STRLEN(haystack);
    ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (puur_naa_nka)tv_get_number_chk(&argvars[2], NULL);
	ꦪꦼꦤ꧀ (end_idx < 0)
	    ꦮꦭꦶ;	// can never find a match
    }
    ꦭꦶꦪ
	end_idx = haystack_len;

    ꦪꦼꦤ꧀ (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    ꦭꦶꦪ
    {
	ꦏꦁꦒꦺꦴ (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((var_na *)rest, (var_na *)needle);
	    ꦪꦼꦤ꧀ (rest == NULL || rest > haystack + end_idx)
		bha_nga;
	    lastmatch = rest;
	}
    }

    ꦪꦼꦤ꧀ (lastmatch == NULL)
	rettv->vval.v_number = -1;
    ꦭꦶꦪ
	rettv->vval.v_number = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    void
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    ꦪꦼꦤ꧀ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

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

    ꦪꦼꦤ꧀ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_number_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    ꦮꦭꦶ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    ꦪꦼꦤ꧀ (str == NULL || idx < 0)
	ꦮꦭꦶ;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ꦪꦼꦤ꧀ (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    puur_naa_nka (*ptr2len)(char_u *);
    ꦪꦼꦤ꧀ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    ꦭꦶꦪ
	ptr2len = mb_ptr2len;

    char_u	*p;
    puur_naa_nka		len;
    puur_naa_nka		utf16idx = 0;
    ꦏꦁꦒꦺꦴ (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	ꦪꦼꦤ꧀ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    ꦪꦼꦤ꧀ (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    ꦮꦭꦶ;
	}
	utf16idx = len;
	puur_naa_nka clen = ptr2len(p);
	puur_naa_nka c = (clen > 1) ? utf_ptr2char(p) : *p;
	ꦪꦼꦤ꧀ (c > 0xFFFF)
	    len++;
	p += clen;
	ꦪꦼꦤ꧀ (charidx)
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
    ꦪꦼꦤ꧀ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    void
f_toupper(typval_T *argvars, typval_T *rettv)
{
    ꦪꦼꦤ꧀ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

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
    puur_naa_nka		inlen;
    puur_naa_nka		fromlen;
    puur_naa_nka		tolen;
    puur_naa_nka		idx;
    char_u	*cpstr;
    puur_naa_nka		cplen;
    puur_naa_nka		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ꦮꦭꦶ;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    ꦪꦼꦤ꧀ (fromstr == NULL || tostr == NULL)
	    ꦮꦭꦶ;		// type error; errmsg already given
    ga_init2(&ga, parimaa_na(var_na), 80);

    ꦪꦼꦤ꧀ (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	ꦪꦼꦤ꧀ (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    ꦮꦭꦶ;
	}

    // fromstr and tostr have to contain the same number of chars
    ꦱꦼꦢꦼꦭ (*in_str != NUL)
    {
	ꦪꦼꦤ꧀ (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    ꦏꦁꦒꦺꦴ (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		ꦪꦼꦤ꧀ (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    ꦏꦁꦒꦺꦴ (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			ꦪꦼꦤ꧀ (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    bha_nga;
			}
		    }
		    ꦪꦼꦤ꧀ (*p == NUL)	// tostr is shorter than fromstr
			goto error;
		    bha_nga;
		}
		++idx;
	    }

	    ꦪꦼꦤ꧀ (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		ꦏꦁꦒꦺꦴ (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		ꦪꦼꦤ꧀ (idx != 0)
		    goto error;
	    }

	    (void)ga_grow(&ga, cplen);
	    mch_memmove((var_na *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	ꦭꦶꦪ
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    ꦪꦼꦤ꧀ (p != NULL)
		ga_append(&ga, tostr[p - fromstr]);
	    ꦭꦶꦪ
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
    puur_naa_nka		c1;
    puur_naa_nka		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ꦮꦭꦶ;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    ꦪꦼꦤ꧀ (head == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (check_for_opt_string_arg(argvars, 1) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	ꦪꦼꦤ꧀ (*mask == NUL)
	    mask = NULL;

	ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    puur_naa_nka	error = 0;

	    // leading or trailing characters to trim
	    dir = (puur_naa_nka)tv_get_number_chk(&argvars[2], &error);
	    ꦪꦼꦤ꧀ (error)
		ꦮꦭꦶ;
	    ꦪꦼꦤ꧀ (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		ꦮꦭꦶ;
	    }
	}
    }

    ꦪꦼꦤ꧀ (dir == 0 || dir == 1)
    {
	// Trim leading characters
	ꦱꦼꦢꦼꦭ (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    ꦪꦼꦤ꧀ (mask == NULL)
	    {
		ꦪꦼꦤ꧀ (c1 > ' ' && c1 != 0xa0)
		    bha_nga;
	    }
	    ꦭꦶꦪ
	    {
		ꦏꦁꦒꦺꦴ (p = mask; *p != NUL; MB_PTR_ADV(p))
		    ꦪꦼꦤ꧀ (c1 == PTR2CHAR(p))
			bha_nga;
		ꦪꦼꦤ꧀ (*p == NUL)
		    bha_nga;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    ꦪꦼꦤ꧀ (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	ꦏꦁꦒꦺꦴ (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    ꦪꦼꦤ꧀ (mask == NULL)
	    {
		ꦪꦼꦤ꧀ (c1 > ' ' && c1 != 0xa0)
		    bha_nga;
	    }
	    ꦭꦶꦪ
	    {
		ꦏꦁꦒꦺꦴ (p = mask; *p != NUL; MB_PTR_ADV(p))
		    ꦪꦼꦤ꧀ (c1 == PTR2CHAR(p))
			bha_nga;
		ꦪꦼꦤ꧀ (*p == NUL)
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
    ꦪꦼꦤ꧀ (str == NULL)
	ꦮꦭꦶ NULL;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    ꦪꦼꦤ꧀ (!decoded)
	ꦮꦭꦶ NULL;

    char_u	*p = decoded;
    size_t	i = 0;

    ꦱꦼꦢꦼꦭ (i < len)
    {
	ꦪꦼꦤ꧀ (str[i] == '%')
	{
	    ꦪꦼꦤ꧀ (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		ꦪꦼꦤ꧀ (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    ꦭꦶꦪ
	    {
		puur_naa_nka val = hexhex2nr(&str[i + 1]);
		ꦪꦼꦤ꧀ (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		ꦭꦶꦪ
		{
		    // invalid hex digits following "%"
		    ꦏꦁꦒꦺꦴ (puur_naa_nka j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	ꦭꦶꦪ
	    *p++ = str[i++];
    }

    *p = NUL;

    ꦮꦭꦶ decoded;
}

/*
 * "uri_decode({str})" function
 */
    void
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ꦪꦼꦤ꧀ (check_for_string_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

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
    ꦪꦼꦤ꧀ (str == NULL)
	ꦮꦭꦶ NULL;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    ꦪꦼꦤ꧀ (encoded == NULL)
	ꦮꦭꦶ NULL;

    char_u *p = encoded;

    ꦏꦁꦒꦺꦴ (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	ꦪꦼꦤ꧀ (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	ꦭꦶꦪ
	{
	    sprintf((var_na *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    ꦮꦭꦶ encoded;
}

/*
 * "uri_encode({str})" function
 */
    void
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ꦪꦼꦤ꧀ (check_for_string_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

static var_na *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static varnumber_T
tv_nr(typval_T *tvs, puur_naa_nka *idxp)
{
    puur_naa_nka		idx = *idxp - 1;
    varnumber_T	n = 0;
    puur_naa_nka		err = FALSE;

    ꦪꦼꦤ꧀ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ꦭꦶꦪ
    {
	++*idxp;
	n = tv_get_number_chk(&tvs[idx], &err);
	ꦪꦼꦤ꧀ (err)
	    n = 0;
    }
    ꦮꦭꦶ n;
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
tv_str(typval_T *tvs, puur_naa_nka *idxp, char_u **tofree)
{
    puur_naa_nka		    idx = *idxp - 1;
    var_na	    *s = NULL;
    static char_u   numbuf[NUMBUFLEN];

    ꦪꦼꦤ꧀ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ꦭꦶꦪ
    {
	++*idxp;
	ꦪꦼꦤ꧀ (tofree != NULL)
	    s = (var_na *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	ꦭꦶꦪ
	    s = (var_na *)tv_get_string_chk(&tvs[idx]);
    }
    ꦮꦭꦶ s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static dviguu_na
tv_float(typval_T *tvs, puur_naa_nka *idxp)
{
    puur_naa_nka		idx = *idxp - 1;
    dviguu_na	f = 0;

    ꦪꦼꦤ꧀ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ꦭꦶꦪ
    {
	++*idxp;
	ꦪꦼꦤ꧀ (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (tvs[idx].v_type == VAR_NUMBER)
	    f = (dviguu_na)tvs[idx].vval.v_number;
	ꦭꦶꦪ
	    emsg(_(e_expected_float_argument_for_printf));
    }
    ꦮꦭꦶ f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    static const var_na *
infinity_str(puur_naa_nka positive,
	     var_na fmt_spec,
	     puur_naa_nka force_sign,
	     puur_naa_nka space_for_positive)
{
    static const var_na *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    puur_naa_nka idx = positive * (1 + force_sign + force_sign * space_for_positive);

    ꦪꦼꦤ꧀ (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    ꦮꦭꦶ table[idx];
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
    puur_naa_nka
vim_snprintf_add(var_na *str, size_t str_m, const var_na *fmt, ...)
{
    va_list	ap;
    puur_naa_nka		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    ꦪꦼꦤ꧀ (str_m <= len)
	space = 0;
    ꦭꦶꦪ
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    ꦮꦭꦶ str_l;
}

    puur_naa_nka
vim_snprintf(var_na *str, size_t str_m, const var_na *fmt, ...)
{
    va_list	ap;
    puur_naa_nka		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    ꦮꦭꦶ str_l;
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
    puur_naa_nka	    str_l;

    ꦪꦼꦤ꧀ (str_m == 0)
	ꦮꦭꦶ 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
    va_end(ap);

    ꦪꦼꦤ꧀ (str_l < 0)
    {
	*str = NUL;
	ꦮꦭꦶ 0;
    }
    ꦮꦭꦶ ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    puur_naa_nka
vim_vsnprintf(
    var_na	*str,
    size_t	str_m,
    const var_na	*fmt,
    va_list	ap)
{
    ꦮꦭꦶ vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
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
    static puur_naa_nka
format_typeof(
    const var_na	*type)
{
    // allowed values: \0, h, l, L
    var_na    length_modifier = '\0';

    // current conversion specifier character
    var_na    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    ꦪꦼꦤ꧀ (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	ꦪꦼꦤ꧀ (length_modifier == 'l' && *type == 'l')
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
	ꦮꦭꦶ TYPE_PERCENT;

    pak_sa 'c':
	ꦮꦭꦶ TYPE_CHAR;

    pak_sa 's':
    pak_sa 'S':
	ꦮꦭꦶ TYPE_STRING;

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

	    ꦪꦼꦤ꧀ (fmt_spec == 'p')
		ꦮꦭꦶ TYPE_POINTER;
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (fmt_spec == 'b' || fmt_spec == 'B')
		ꦮꦭꦶ TYPE_UNSIGNEDLONGLONGINT;
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (fmt_spec == 'd')
	    {
		// signed
		switch (length_modifier)
		{
		pak_sa '\0':
		pak_sa 'h':
		    // char and short arguments are passed as int.
		    ꦮꦭꦶ TYPE_INT;
		pak_sa 'l':
		    ꦮꦭꦶ TYPE_LONGINT;
		pak_sa 'L':
		    ꦮꦭꦶ TYPE_LONGLONGINT;
		}
	    }
	    ꦭꦶꦪ
	    {
		// unsigned
		switch (length_modifier)
		{
		    pak_sa '\0':
		    pak_sa 'h':
			ꦮꦭꦶ TYPE_UNSIGNEDINT;
		    pak_sa 'l':
			ꦮꦭꦶ TYPE_UNSIGNEDLONGINT;
		    pak_sa 'L':
			ꦮꦭꦶ TYPE_UNSIGNEDLONGLONGINT;
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
	ꦮꦭꦶ TYPE_FLOAT;
    }

    ꦮꦭꦶ TYPE_UNKNOWN;
}

    static var_na *
format_typename(
    const var_na  *type)
{
    switch (format_typeof(type))
    {
	pak_sa TYPE_INT:
	    ꦮꦭꦶ typename_int;

	pak_sa TYPE_LONGINT:
	    ꦮꦭꦶ typename_longint;

	pak_sa TYPE_LONGLONGINT:
	    ꦮꦭꦶ typename_longlongint;

	pak_sa TYPE_UNSIGNEDINT:
	    ꦮꦭꦶ typename_unsignedint;

	pak_sa TYPE_UNSIGNEDLONGINT:
	    ꦮꦭꦶ typename_unsignedlongint;

	pak_sa TYPE_UNSIGNEDLONGLONGINT:
	    ꦮꦭꦶ typename_unsignedlonglongint;

	pak_sa TYPE_POINTER:
	    ꦮꦭꦶ _(typename_pointer);

	pak_sa TYPE_PERCENT:
	    ꦮꦭꦶ _(typename_percent);

	pak_sa TYPE_CHAR:
	    ꦮꦭꦶ typename_char;

	pak_sa TYPE_STRING:
	    ꦮꦭꦶ _(typename_string);

	pak_sa TYPE_FLOAT:
	    ꦮꦭꦶ typename_float;
    }

    ꦮꦭꦶ _(typename_unknown);
}

    static puur_naa_nka
adjust_types(
    const var_na ***ap_types,
    puur_naa_nka arg,
    puur_naa_nka *num_posarg,
    const var_na *type)
{
    ꦪꦼꦤ꧀ (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	ꦮꦭꦶ FAIL;
    }

    ꦪꦼꦤ꧀ (*ap_types == NULL || *num_posarg < arg)
    {
	puur_naa_nka	    idx;
	const var_na  **new_types;

	ꦪꦼꦤ꧀ (*ap_types == NULL)
	    new_types = ALLOC_CLEAR_MULT(const var_na *, arg);
	ꦭꦶꦪ
	    new_types = vim_realloc((var_na **)*ap_types,
						arg * parimaa_na(const var_na *));

	ꦪꦼꦤ꧀ (new_types == NULL)
	    ꦮꦭꦶ FAIL;

	ꦏꦁꦒꦺꦴ (idx = *num_posarg; idx < arg; ++idx)
	    new_types[idx] = NULL;

	*ap_types = new_types;
	*num_posarg = arg;
    }

    ꦪꦼꦤ꧀ ((*ap_types)[arg - 1] != NULL)
    {
	ꦪꦼꦤ꧀ ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    const var_na *pt = type;
	    ꦪꦼꦤ꧀ (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    ꦪꦼꦤ꧀ (pt[0] != '*')
	    {
		switch (pt[0])
		{
		    pak_sa 'd':
		    pak_sa 'i': bha_nga;
		    default:
			semsg(_(e_positional_num_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			ꦮꦭꦶ FAIL;
		}
	    }
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_num_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		ꦮꦭꦶ FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    ꦮꦭꦶ OK;
}

    static void
format_overflow_error(const var_na *pstart)
{
    size_t	arglen = 0;
    var_na	*argcopy = NULL;
    const var_na	*p = pstart;

    ꦱꦼꦢꦼꦭ (VIM_ISDIGIT((puur_naa_nka)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(var_na, arglen + 1);
    ꦪꦼꦤ꧀ (argcopy != NULL)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	free(argcopy);
    }
    ꦭꦶꦪ
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    static puur_naa_nka
get_unsigned_int(
    const var_na *pstart,
    const var_na **p,
    unsigned puur_naa_nka *uj,
    puur_naa_nka overflow_err)
{
    *uj = **p - '0';
    ++*p;

    ꦱꦼꦢꦼꦭ (VIM_ISDIGIT((puur_naa_nka)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned puur_naa_nka)(**p - '0');
	++*p;
    }

    ꦪꦼꦤ꧀ (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	ꦪꦼꦤ꧀ (overflow_err)
	{
	    format_overflow_error(pstart);
	    ꦮꦭꦶ FAIL;
	}
	ꦭꦶꦪ
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    ꦮꦭꦶ OK;
}


    static puur_naa_nka
parse_fmt_types(
    const var_na  ***ap_types,
    puur_naa_nka		*num_posarg,
    const var_na  *fmt,
    typval_T	*tvs UNUSED
    )
{
    const var_na	*p = fmt;
    const var_na	*arg = NULL;

    puur_naa_nka		any_pos = 0;
    puur_naa_nka		any_arg = 0;
    puur_naa_nka		arg_idx;

# define CHECK_POS_ARG do { \
    ꦪꦼꦤ꧀ (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	goto error; \
    } \
} ꦱꦼꦢꦼꦭ (0);

    ꦪꦼꦤ꧀ (p == NULL)
	ꦮꦭꦶ OK;

    ꦱꦼꦢꦼꦭ (*p != NUL)
    {
	ꦪꦼꦤ꧀ (*p != '%')
	{
	    const var_na    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	ꦭꦶꦪ
	{
	    // allowed values: \0, h, l, L
	    var_na	length_modifier = '\0';

	    // variable for positional arg
	    puur_naa_nka		pos_arg = -1;
	    const var_na	*ptype = NULL;
	    const var_na	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    ꦱꦼꦢꦼꦭ (VIM_ISDIGIT(*ptype))
		++ptype;

	    ꦪꦼꦤ꧀ (*ptype == '$')
	    {
		ꦪꦼꦤ꧀ (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}

		// Positional argument
		unsigned puur_naa_nka uj;

		ꦪꦼꦤ꧀ (get_unsigned_int(pstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    ꦱꦼꦢꦼꦭ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
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
	    ꦪꦼꦤ꧀ (*(arg = p) == '*')
	    {
		p++;

		ꦪꦼꦤ꧀ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		{
		    // Positional argument field width
		    unsigned puur_naa_nka uj;

		    ꦪꦼꦤ꧀ (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    ꦪꦼꦤ꧀ (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		    ꦭꦶꦪ
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			ꦪꦼꦤ꧀ (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
			    goto error;
		    }
		}
		ꦭꦶꦪ
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (VIM_ISDIGIT((puur_naa_nka)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const var_na *digstart = p;
		unsigned puur_naa_nka uj;

		ꦪꦼꦤ꧀ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		ꦪꦼꦤ꧀ (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}
	    }

	    // parse precision
	    ꦪꦼꦤ꧀ (*p == '.')
	    {
		p++;

		ꦪꦼꦤ꧀ (*(arg = p) == '*')
		{
		    p++;

		    ꦪꦼꦤ꧀ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		    {
			// Parse precision
			unsigned puur_naa_nka uj;

			ꦪꦼꦤ꧀ (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			ꦪꦼꦤ꧀ (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    ꦪꦼꦤ꧀ (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
				goto error;
			}
			ꦭꦶꦪ
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    goto error;
			}
		    }
		    ꦭꦶꦪ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		ꦭꦶꦪ ꦪꦼꦤ꧀ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const var_na *digstart = p;
		    unsigned puur_naa_nka uj;

		    ꦪꦼꦤ꧀ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    ꦪꦼꦤ꧀ (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		}
	    }

	    ꦪꦼꦤ꧀ (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ꦪꦼꦤ꧀ (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		ꦪꦼꦤ꧀ (length_modifier == 'l' && *p == 'l')
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
		    ꦪꦼꦤ꧀ (pos_arg != -1)
		    {
			ꦪꦼꦤ꧀ (adjust_types(ap_types, pos_arg, num_posarg, ptype) == FAIL)
			    goto error;
		    }
		    ꦭꦶꦪ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    bha_nga;

		default:
		    ꦪꦼꦤ꧀ (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			goto error;
		    }
	    }

	    ꦪꦼꦤ꧀ (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    ꦏꦁꦒꦺꦴ (arg_idx = 0; arg_idx < *num_posarg; ++arg_idx)
    {
	ꦪꦼꦤ꧀ ((*ap_types)[arg_idx] == NULL)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    goto error;
	}

# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
	ꦪꦼꦤ꧀ (tvs != NULL && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    goto error;
	}
# endif
    }

    ꦮꦭꦶ OK;

error:
    vim_free((var_na**)*ap_types);
    *ap_types = NULL;
    *num_posarg = 0;
    ꦮꦭꦶ FAIL;
}

    static void
skip_to_arg(
    const var_na	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    puur_naa_nka		*arg_idx,
    puur_naa_nka		*arg_cur,
    const var_na	*fmt)
{
    puur_naa_nka		arg_min = 0;

    ꦪꦼꦤ꧀ (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	ꦮꦭꦶ;
    }

    ꦪꦼꦤ꧀ (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    ꦭꦶꦪ
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    ꦏꦁꦒꦺꦴ (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	const var_na *p;

	ꦪꦼꦤ꧀ (ap_types == NULL || ap_types[*arg_cur] == NULL)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    ꦮꦭꦶ;
	}

	p = ap_types[*arg_cur];

	puur_naa_nka fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	switch (fmt_type)
	{
	pak_sa TYPE_PERCENT:
	pak_sa TYPE_UNKNOWN:
	    bha_nga;

	pak_sa TYPE_CHAR:
	    va_arg(*ap, puur_naa_nka);
	    bha_nga;

	pak_sa TYPE_STRING:
	    va_arg(*ap, var_na *);
	    bha_nga;

	pak_sa TYPE_POINTER:
	    va_arg(*ap, void *);
	    bha_nga;

	pak_sa TYPE_INT:
	    va_arg(*ap, puur_naa_nka);
	    bha_nga;

	pak_sa TYPE_LONGINT:
	    va_arg(*ap, long puur_naa_nka);
	    bha_nga;

	pak_sa TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    bha_nga;

	pak_sa TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned puur_naa_nka);
	    bha_nga;

	pak_sa TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long puur_naa_nka);
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

    ꦮꦭꦶ;
}

    puur_naa_nka
vim_vsnprintf_typval(
    var_na	*str,
    size_t	str_m,
    const var_na	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    const var_na	*p = fmt;
    puur_naa_nka		arg_cur = 0;
    puur_naa_nka		num_posarg = 0;
    puur_naa_nka		arg_idx = 1;
    va_list	ap;
    const var_na	**ap_types = NULL;

    ꦪꦼꦤ꧀ (parse_fmt_types(&ap_types, &num_posarg, fmt, tvs) == FAIL)
	ꦮꦭꦶ 0;

    va_copy(ap, ap_start);

    ꦪꦼꦤ꧀ (p == NULL)
	p = "";
    ꦱꦼꦢꦼꦭ (*p != NUL)
    {
	ꦪꦼꦤ꧀ (*p != '%')
	{
	    const var_na    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    ꦪꦼꦤ꧀ (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	ꦭꦶꦪ
	{
	    size_t  min_field_width = 0, precision = 0;
	    puur_naa_nka	    zero_padding = 0, precision_specified = 0, justify_left = 0;
	    puur_naa_nka	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    puur_naa_nka	    space_for_positive = 1;

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
	    puur_naa_nka	    pos_arg = -1;
	    const var_na	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    ꦱꦼꦢꦼꦭ (VIM_ISDIGIT(*ptype))
		++ptype;

	    ꦪꦼꦤ꧀ (*ptype == '$')
	    {
		// Positional argument
		const var_na *digstart = p;
		unsigned puur_naa_nka uj;

		ꦪꦼꦤ꧀ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    ꦱꦼꦢꦼꦭ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
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
	    ꦪꦼꦤ꧀ (*p == '*')
	    {
		puur_naa_nka j;
		const var_na *digstart = p + 1;

		p++;

		ꦪꦼꦤ꧀ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		{
		    // Positional argument field width
		    unsigned puur_naa_nka uj;

		    ꦪꦼꦤ꧀ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
		    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, puur_naa_nka));

		ꦪꦼꦤ꧀ (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    ꦪꦼꦤ꧀ (tvs != NULL)
		    {
			format_overflow_error(digstart);
			goto error;
		    }
		    ꦭꦶꦪ
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		ꦪꦼꦤ꧀ (j >= 0)
		    min_field_width = j;
		ꦭꦶꦪ
		{
		    min_field_width = -j;
		    justify_left = 1;
		}
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (VIM_ISDIGIT((puur_naa_nka)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const var_na *digstart = p;
		unsigned puur_naa_nka uj;

		ꦪꦼꦤ꧀ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		min_field_width = uj;
	    }

	    // parse precision
	    ꦪꦼꦤ꧀ (*p == '.')
	    {
		p++;
		precision_specified = 1;

		ꦪꦼꦤ꧀ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const var_na *digstart = p;
		    unsigned puur_naa_nka uj;

		    ꦪꦼꦤ꧀ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    precision = uj;
		}
		ꦭꦶꦪ ꦪꦼꦤ꧀ (*p == '*')
		{
		    puur_naa_nka j;
		    const var_na *digstart = p;

		    p++;

		    ꦪꦼꦤ꧀ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		    {
			// positional argument
			unsigned puur_naa_nka uj;

			ꦪꦼꦤ꧀ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			arg_idx = uj;

			++p;
		    }

		    j =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
			tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, puur_naa_nka));

		    ꦪꦼꦤ꧀ (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			ꦪꦼꦤ꧀ (tvs != NULL)
			{
			    format_overflow_error(digstart);
			    goto error;
			}
			ꦭꦶꦪ
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    ꦪꦼꦤ꧀ (j >= 0)
			precision = j;
		    ꦭꦶꦪ
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ꦪꦼꦤ꧀ (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		ꦪꦼꦤ꧀ (length_modifier == 'l' && *p == 'l')
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

# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
	    switch (fmt_spec)
	    {
		pak_sa 'd': pak_sa 'u': pak_sa 'o': pak_sa 'x': pak_sa 'X':
		    ꦪꦼꦤ꧀ (tvs != NULL && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    ꦪꦼꦤ꧀ (pos_arg != -1)
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
			puur_naa_nka j;

			j =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
			    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, puur_naa_nka));

			// standard demands unsigned char
			uchar_arg = (unsigned var_na)j;
			str_arg = (var_na *)&uchar_arg;
			bha_nga;
		    }

		pak_sa 's':
		pak_sa 'S':
		    str_arg =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
				tvs != NULL ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, var_na *));

		    ꦪꦼꦤ꧀ (str_arg == NULL)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (precision == 0)
			str_arg_l = 0;
		    ꦭꦶꦪ
		    {
			// memchr on HP does not like n > 2^31  !!!
			const var_na *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == NULL) ? precision
						      : (size_t)(q - str_arg);
		    }
		    ꦪꦼꦤ꧀ (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			puur_naa_nka	cell;

			ꦏꦁꦒꦺꦴ (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    ꦪꦼꦤ꧀ (precision_specified && i + cell > precision)
				bha_nga;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			ꦪꦼꦤ꧀ (min_field_width != 0)
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
		    puur_naa_nka arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    puur_naa_nka int_arg = 0;
		    unsigned puur_naa_nka uint_arg = 0;

		    // only set for length modifier l
		    long puur_naa_nka long_arg = 0;
		    unsigned long puur_naa_nka ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    void *ptr_arg = NULL;

		    ꦪꦼꦤ꧀ (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
				 tvs != NULL ? (void *)tv_str(tvs, &arg_idx,
									NULL) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, void *));

			ꦪꦼꦤ꧀ (ptr_arg != NULL)
			    arg_sign = 1;
		    }
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
				    tvs != NULL ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			ꦪꦼꦤ꧀ (bin_arg != 0)
			    arg_sign = 1;
		    }
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (fmt_spec == 'd')
		    {
			// signed
			switch (length_modifier)
			{
			pak_sa '\0':
			pak_sa 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, puur_naa_nka));

			    ꦪꦼꦤ꧀ (int_arg > 0)
				arg_sign =  1;
			    ꦭꦶꦪ ꦪꦼꦤ꧀ (int_arg < 0)
				arg_sign = -1;
			    bha_nga;
			pak_sa 'l':
			    long_arg =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long puur_naa_nka));

			    ꦪꦼꦤ꧀ (long_arg > 0)
				arg_sign =  1;
			    ꦭꦶꦪ ꦪꦼꦤ꧀ (long_arg < 0)
				arg_sign = -1;
			    bha_nga;
			pak_sa 'L':
			    llong_arg =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    ꦪꦼꦤ꧀ (llong_arg > 0)
				arg_sign =  1;
			    ꦭꦶꦪ ꦪꦼꦤ꧀ (llong_arg < 0)
				arg_sign = -1;
			    bha_nga;
			}
		    }
		    ꦭꦶꦪ
		    {
			// unsigned
			switch (length_modifier)
			{
			    pak_sa '\0':
			    pak_sa 'h':
				uint_arg =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned puur_naa_nka));

				ꦪꦼꦤ꧀ (uint_arg != 0)
				    arg_sign = 1;
				bha_nga;
			    pak_sa 'l':
				ulong_arg =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long puur_naa_nka));

				ꦪꦼꦤ꧀ (ulong_arg != 0)
				    arg_sign = 1;
				bha_nga;
			    pak_sa 'L':
				ullong_arg =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
					    tvs != NULL ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				ꦪꦼꦤ꧀ (ullong_arg != 0)
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
		    ꦪꦼꦤ꧀ (precision_specified)
			zero_padding = 0;
		    ꦪꦼꦤ꧀ (fmt_spec == 'd')
		    {
			ꦪꦼꦤ꧀ (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (alternate_form)
		    {
			ꦪꦼꦤ꧀ (arg_sign != 0
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
		    ꦪꦼꦤ꧀ (!precision_specified)
			precision = 1;   // default precision is 1
		    ꦪꦼꦤ꧀ (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    ꦭꦶꦪ
		    {
			var_na	f[6];
			puur_naa_nka	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			ꦪꦼꦤ꧀ (!length_modifier)
			    ;
			ꦭꦶꦪ ꦪꦼꦤ꧀ (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# ꦭꦶꦪ
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			ꦭꦶꦪ
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			ꦪꦼꦤ꧀ (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			ꦭꦶꦪ ꦪꦼꦤ꧀ (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    var_na	    b[8 * parimaa_na(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    do
			    {
				b[parimaa_na(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    ꦱꦼꦢꦼꦭ (bn != 0);

			    memcpy(tmp + str_arg_l, b + parimaa_na(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			ꦭꦶꦪ ꦪꦼꦤ꧀ (fmt_spec == 'd')
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
			ꦭꦶꦪ
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
			ꦪꦼꦤ꧀ (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			ꦪꦼꦤ꧀ (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t num_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			ꦪꦼꦤ꧀ (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    ꦪꦼꦤ꧀ (!precision_specified
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
			ꦪꦼꦤ꧀ (num_of_digits < precision)
			    number_of_zeros_to_pad = precision - num_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    ꦪꦼꦤ꧀ (!justify_left && zero_padding)
		    {
			puur_naa_nka n = (puur_naa_nka)(min_field_width - (str_arg_l
						    + number_of_zeros_to_pad));
			ꦪꦼꦤ꧀ (n > 0)
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
		    puur_naa_nka		l;
		    puur_naa_nka		remove_trailing_zeroes = FALSE;

		    f =
# ꦪꦼꦤ꧀ defined(FEAT_EVAL)
			tvs != NULL ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, dviguu_na));

		    abs_f = f < 0 ? -f : f;

		    ꦪꦼꦤ꧀ (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			ꦪꦼꦤ꧀ ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			ꦭꦶꦪ
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    ꦪꦼꦤ꧀ ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# ꦭꦶꦪ
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
		    ꦭꦶꦪ
		    {
			ꦪꦼꦤ꧀ (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			ꦭꦶꦪ ꦪꦼꦤ꧀ (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			ꦭꦶꦪ
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    ꦪꦼꦤ꧀ (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    ꦪꦼꦤ꧀ (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				ꦪꦼꦤ꧀ ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				ꦪꦼꦤ꧀ (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (puur_naa_nka)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			ꦪꦼꦤ꧀ (remove_trailing_zeroes)
			{
			    puur_naa_nka i;
			    var_na *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    ꦪꦼꦤ꧀ (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    ꦭꦶꦪ
			    {
				tp = (var_na *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				ꦪꦼꦤ꧀ (tp != NULL)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    ꦪꦼꦤ꧀ (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    ꦱꦼꦢꦼꦭ (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    ꦪꦼꦤ꧀ (tp != NULL && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				ꦱꦼꦢꦼꦭ (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			ꦭꦶꦪ
			{
			    var_na *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (var_na *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    ꦪꦼꦤ꧀ (tp != NULL && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    ꦪꦼꦤ꧀ (zero_padding && min_field_width > str_arg_l
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
		ꦪꦼꦤ꧀ (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		bha_nga;
	    }

	    ꦪꦼꦤ꧀ (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    ꦪꦼꦤ꧀ (!justify_left)
	    {
		// left padding with blank or zero
		puur_naa_nka pn = (puur_naa_nka)(min_field_width - (str_arg_l + number_of_zeros_to_pad));

		ꦪꦼꦤ꧀ (pn > 0)
		{
		    ꦪꦼꦤ꧀ (str_l < str_m)
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
	    ꦪꦼꦤ꧀ (number_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    ꦭꦶꦪ
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		puur_naa_nka zn = (puur_naa_nka)zero_padding_insertion_ind;

		ꦪꦼꦤ꧀ (zn > 0)
		{
		    ꦪꦼꦤ꧀ (str_l < str_m)
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
		zn = (puur_naa_nka)number_of_zeros_to_pad;
		ꦪꦼꦤ꧀ (zn > 0)
		{
		    ꦪꦼꦤ꧀ (str_l < str_m)
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
		puur_naa_nka sn = (puur_naa_nka)(str_arg_l - zero_padding_insertion_ind);

		ꦪꦼꦤ꧀ (sn > 0)
		{
		    ꦪꦼꦤ꧀ (str_l < str_m)
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
	    ꦪꦼꦤ꧀ (justify_left)
	    {
		// right blank padding to the field width
		puur_naa_nka pn = (puur_naa_nka)(min_field_width
				      - (str_arg_l + number_of_zeros_to_pad));

		ꦪꦼꦤ꧀ (pn > 0)
		{
		    ꦪꦼꦤ꧀ (str_l < str_m)
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

    ꦪꦼꦤ꧀ (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    ꦪꦼꦤ꧀ (tvs != NULL && tvs[num_posarg != 0 ? num_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((var_na*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    ꦮꦭꦶ (puur_naa_nka)str_l;
}

#endif // PROTO
