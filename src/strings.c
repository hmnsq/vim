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
    ᱡᱩᱫᱤ (p != NULL)
	mch_memmove(p, string, len);
    ᱨᱩᱣᱟᱹᱲ p;
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
    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    STRNCPY(p, string, len);
    p[len] = NUL;
    ᱨᱩᱣᱟᱹᱲ p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    ᱨᱩᱣᱟᱹᱲ vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    for (p = string; *p; p++)
    {
	ᱡᱩᱫᱤ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    continue;
	}
	ᱡᱩᱫᱤ (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    ᱡᱩᱫᱤ (escaped_string == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;
    p2 = escaped_string;
    for (p = string; *p; p++)
    {
	ᱡᱩᱫᱤ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    continue;
	}
	ᱡᱩᱫᱤ (vim_strchr(esc_chars, *p) != NULL || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    ᱨᱩᱣᱟᱹᱲ escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    puur_naa_nka
csh_like_shell(void)
{
    ᱨᱩᱣᱟᱹᱲ (strstr((var_na *)gettail(p_sh), "csh") != NULL);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    static puur_naa_nka
fish_like_shell(void)
{
    ᱨᱩᱣᱟᱹᱲ (strstr((var_na *)gettail(p_sh), "fish") != NULL);
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
    for (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	ᱡᱩᱫᱤ (double_quotes)
	{
	    ᱡᱩᱫᱤ (*p == '"')
		++length;		// " -> ""
	}
	ᱵᱟᱝ
#endif
	ᱡᱩᱫᱤ (*p == '\'')
	{
	    ᱡᱩᱫᱤ (powershell)
		length += 2;		// ' => ''
	    ᱵᱟᱝ
		length += 3;		// ' => '\''
	}
	ᱡᱩᱫᱤ ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    ᱡᱩᱫᱤ (csh_like && do_special)
		++length;		// insert backslash
	}
	ᱡᱩᱫᱤ (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	ᱡᱩᱫᱤ (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    ᱡᱩᱫᱤ (escaped_string != NULL)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	ᱡᱩᱫᱤ (double_quotes)
	    *d++ = '"';
	ᱵᱟᱝ
#endif
	    *d++ = '\'';

	for (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    ᱡᱩᱫᱤ (double_quotes)
	    {
		ᱡᱩᱫᱤ (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    continue;
		}
	    }
	    ᱵᱟᱝ
#endif
	    ᱡᱩᱫᱤ (*p == '\'')
	    {
		ᱡᱩᱫᱤ (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		ᱵᱟᱝ
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		continue;
	    }
	    ᱡᱩᱫᱤ ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		ᱡᱩᱫᱤ (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		continue;
	    }
	    ᱡᱩᱫᱤ (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		continue;
	    }
	    ᱡᱩᱫᱤ (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		continue;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	ᱡᱩᱫᱤ (double_quotes)
	    *d++ = '"';
	ᱵᱟᱝ
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    ᱨᱩᱣᱟᱹᱲ escaped_string;
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
    ᱨᱩᱣᱟᱹᱲ p1;
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
    ᱨᱩᱣᱟᱹᱲ p1;
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

    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    p2 = p;
    ᱡᱚᱠᱷᱚᱱ ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#ᱡᱩᱫᱤ defined(FEAT_EVAL) || defined(FEAT_SPELL)
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

    ᱡᱩᱫᱤ (res != NULL)
	ᱡᱚᱠᱷᱚᱱ (*p != NUL)
	{
	    puur_naa_nka		l;

	    ᱡᱩᱫᱤ (enc_utf8)
	    {
		puur_naa_nka	c, uc;
		puur_naa_nka	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ᱡᱩᱫᱤ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		ᱡᱩᱫᱤ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ᱡᱩᱫᱤ (s == NULL)
		    {
			vim_free(res);
			ᱨᱩᱣᱟᱹᱲ NULL;
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
	    ᱵᱟᱝ ᱡᱩᱫᱤ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ᱵᱟᱝ
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    ᱨᱩᱣᱟᱹᱲ res;
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

    ᱡᱩᱫᱤ (res != NULL)
	ᱡᱚᱠᱷᱚᱱ (*p != NUL)
	{
	    puur_naa_nka		l;

	    ᱡᱩᱫᱤ (enc_utf8)
	    {
		puur_naa_nka	c, lc;
		puur_naa_nka	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ᱡᱩᱫᱤ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		ᱡᱩᱫᱤ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ᱡᱩᱫᱤ (s == NULL)
		    {
			vim_free(res);
			ᱨᱩᱣᱟᱹᱲ NULL;
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
	    ᱵᱟᱝ ᱡᱩᱫᱤ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ᱵᱟᱝ
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    ᱨᱩᱣᱟᱹᱲ res;
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
    ᱡᱚᱠᱷᱚᱱ (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
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

    ᱡᱩᱫᱤ (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    ᱵᱟᱝ
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(var_na *s, size_t maxlen)
{
    size_t i;
    for (i = 0; i < maxlen; ++i)
	ᱡᱩᱫᱤ (s[i] == NUL)
	    bha_nga;
    ᱨᱩᱣᱟᱹᱲ i;
}

#ᱡᱩᱫᱤ !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    puur_naa_nka
vim_stricmp(var_na *s1, var_na *s2)
{
    puur_naa_nka		i;

    for (;;)
    {
	i = (puur_naa_nka)TOLOWER_LOC(*s1) - (puur_naa_nka)TOLOWER_LOC(*s2);
	ᱡᱩᱫᱤ (i != 0)
	    ᱨᱩᱣᱟᱹᱲ i;			    // this character different
	ᱡᱩᱫᱤ (*s1 == NUL)
	    bha_nga;			    // strings match until NUL
	++s1;
	++s2;
    }
    ᱨᱩᱣᱟᱹᱲ 0;				    // strings match
}
#endif

#ᱡᱩᱫᱤ !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    puur_naa_nka
vim_strnicmp(var_na *s1, var_na *s2, size_t len)
{
    puur_naa_nka		i;

    ᱡᱚᱠᱷᱚᱱ (len > 0)
    {
	i = (puur_naa_nka)TOLOWER_LOC(*s1) - (puur_naa_nka)TOLOWER_LOC(*s2);
	ᱡᱩᱫᱤ (i != 0)
	    ᱨᱩᱣᱟᱹᱲ i;			    // this character different
	ᱡᱩᱫᱤ (*s1 == NUL)
	    bha_nga;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ᱨᱩᱣᱟᱹᱲ 0;				    // strings match
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

    ᱡᱚᱠᱷᱚᱱ (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	ᱡᱩᱫᱤ (i != 0)
	    bha_nga;			// this character is different
	ᱡᱩᱫᱤ (*s1 == NUL)
	    bha_nga;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ᱨᱩᱣᱟᱹᱲ i;
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
    ᱡᱩᱫᱤ (enc_utf8 && c > 0 && c < 0x80)
	ᱨᱩᱣᱟᱹᱲ vim_strbyte(string, c);
    ᱡᱩᱫᱤ (enc_utf8 && c >= 0x80)
    {
	ᱡᱚᱠᱷᱚᱱ (*p != NUL)
	{
	    puur_naa_nka l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    ᱡᱩᱫᱤ (utf_ptr2char(p) == c && l > 1)
		ᱨᱩᱣᱟᱹᱲ p;
	    p += l;
	}
	ᱨᱩᱣᱟᱹᱲ NULL;
    }
    ᱡᱩᱫᱤ (enc_dbcs != 0 && c > 255)
    {
	puur_naa_nka	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	ᱡᱚᱠᱷᱚᱱ ((b = *p) != NUL)
	{
	    ᱡᱩᱫᱤ (b == c && p[1] == n2)
		ᱨᱩᱣᱟᱹᱲ p;
	    p += (*mb_ptr2len)(p);
	}
	ᱨᱩᱣᱟᱹᱲ NULL;
    }
    ᱡᱩᱫᱤ (has_mbyte)
    {
	ᱡᱚᱠᱷᱚᱱ ((b = *p) != NUL)
	{
	    ᱡᱩᱫᱤ (b == c)
		ᱨᱩᱣᱟᱹᱲ p;
	    p += (*mb_ptr2len)(p);
	}
	ᱨᱩᱣᱟᱹᱲ NULL;
    }
    ᱡᱚᱠᱷᱚᱱ ((b = *p) != NUL)
    {
	ᱡᱩᱫᱤ (b == c)
	    ᱨᱩᱣᱟᱹᱲ p;
	++p;
    }
    ᱨᱩᱣᱟᱹᱲ NULL;
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
    ᱡᱩᱫᱤ (c <= 0 || c > 255)
	ᱨᱩᱣᱟᱹᱲ NULL;
    ᱨᱩᱣᱟᱹᱲ (char_u *)strchr((var_na *)string, c);
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

    ᱡᱚᱠᱷᱚᱱ (*p)
    {
	ᱡᱩᱫᱤ (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    ᱨᱩᱣᱟᱹᱲ retval;
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
    ᱡᱚᱠᱷᱚᱱ (*s)
    {
	ᱡᱩᱫᱤ (vim_strchr(charset, *s) != NULL)
	    ᱨᱩᱣᱟᱹᱲ s;
	MB_PTR_ADV(s);
    }
    ᱨᱩᱣᱟᱹᱲ NULL;
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
    ᱨᱩᱣᱟᱹᱲ STRCMP(*(var_na **)s1, *(var_na **)s2);
}

    void
sort_strings(
    char_u	**files,
    puur_naa_nka		count)
{
    qsort((void *)files, (size_t)count, parimaa_na(char_u *), sort_compare);
}

#ᱡᱩᱫᱤ defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    puur_naa_nka
has_non_ascii(char_u *s)
{
    char_u	*p;

    ᱡᱩᱫᱤ (s != NULL)
	for (p = s; *p != NUL; ++p)
	    ᱡᱩᱫᱤ (*p >= 128)
		ᱨᱩᱣᱟᱹᱲ TRUE;
    ᱨᱩᱣᱟᱹᱲ FALSE;
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
    ᱡᱩᱫᱤ (dest == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;
    ᱡᱩᱫᱤ (str1 == NULL)
	*dest = NUL;
    ᱵᱟᱝ
	STRCPY(dest, str1);
    ᱡᱩᱫᱤ (str2 != NULL)
	STRCPY(dest + l, str2);
    ᱨᱩᱣᱟᱹᱲ dest;
}

#ᱡᱩᱫᱤ defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    ᱡᱩᱫᱤ (rev == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    for (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	ᱡᱩᱫᱤ (has_mbyte)
	{
	    puur_naa_nka mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	ᱵᱟᱝ
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    ᱨᱩᱣᱟᱹᱲ rev;
}
#endif

#ᱡᱩᱫᱤ defined(FEAT_EVAL)
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
    ᱡᱩᱫᱤ (str != NULL)
    {
	len += (unsigned)STRLEN(str);
	for (p = str; *p != NUL; MB_PTR_ADV(p))
	    ᱡᱩᱫᱤ (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    ᱡᱩᱫᱤ (r == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    ᱡᱩᱫᱤ (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    ᱵᱟᱝ
	*r++ = '\'';
    ᱡᱩᱫᱤ (str != NULL)
	for (p = str; *p != NUL; )
	{
	    ᱡᱩᱫᱤ (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    ᱡᱩᱫᱤ (function)
	*r++ = ')';
    *r++ = NUL;
    ᱨᱩᱣᱟᱹᱲ s;
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

    ᱡᱩᱫᱤ (p == NULL || needle == NULL || *needle == NUL)
	ᱨᱩᱣᱟᱹᱲ 0;

    size_t  needlelen = STRLEN(needle);
    ᱡᱩᱫᱤ (ic)
    {
	ᱡᱚᱠᱷᱚᱱ (*p != NUL)
	{
	    ᱡᱩᱫᱤ (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    ᱵᱟᱝ
		MB_PTR_ADV(p);
	}
    }
    ᱵᱟᱝ
	ᱡᱚᱠᱷᱚᱱ ((next = (char_u *)strstr((var_na *)p, (var_na *)needle)) != NULL)
	{
	    ++n;
	    p = next + needlelen;
	}

    ᱨᱩᱣᱟᱹᱲ n;
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

    ᱡᱩᱫᱤ (input == NULL || output == NULL)
	ᱨᱩᱣᱟᱹᱲ -1;

    len = has_mbyte ? mb_ptr2len(input) : 1;
    STRNCPY(buf, input, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    ᱨᱩᱣᱟᱹᱲ output->vval.v_string == NULL ? -1 : len;
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
    for (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	ᱡᱩᱫᱤ (len < 0)
	    bha_nga;

	set_vim_var_nr(VV_KEY, idx);
	ᱡᱩᱫᱤ (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    bha_nga;
	}
	ᱡᱩᱫᱤ (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    ᱡᱩᱫᱤ (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		bha_nga;
	    }
	    ᱵᱟᱝ
		ga_concat(&ga, newtv.vval.v_string);
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    ᱡᱩᱫᱤ (fc != NULL)
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

    ᱡᱩᱫᱤ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ᱡᱩᱫᱤ (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "String");
	    ᱨᱩᱣᱟᱹᱲ;
	}
	len = copy_first_char_to_tv(p, rettv);
	ᱡᱩᱫᱤ (len < 0)
	    ᱨᱩᱣᱟᱹᱲ;
	p += len;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (check_for_string_arg(argvars, 2) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;
    ᱵᱟᱝ
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    for ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	ᱡᱩᱫᱤ (len < 0)
	{
	    *rettv = argv[0];
	    bha_nga;
	}

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	ᱡᱩᱫᱤ (r == FAIL || called_emsg != called_emsg_start)
	    bha_nga;
    }

    ᱡᱩᱫᱤ (fc != NULL)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    static void
byteidx_common(typval_T *argvars, typval_T *rettv, puur_naa_nka comp)
{
    rettv->vval.v_number = -1;

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ᱨᱩᱣᱟᱹᱲ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    ᱡᱩᱫᱤ (str == NULL || idx < 0)
	ᱨᱩᱣᱟᱹᱲ;

    varnumber_T	utf16idx = FALSE;
    ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN)
    {
	puur_naa_nka error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	ᱡᱩᱫᱤ (error)
	    ᱨᱩᱣᱟᱹᱲ;
	ᱡᱩᱫᱤ (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), utf16idx);
	    ᱨᱩᱣᱟᱹᱲ;
	}
    }

    puur_naa_nka (*ptr2len)(char_u *);
    ᱡᱩᱫᱤ (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    ᱵᱟᱝ
	ptr2len = mb_ptr2len;

    char_u *t = str;
    for ( ; idx > 0; idx--)
    {
	ᱡᱩᱫᱤ (*t == NUL)		// EOL reached
	    ᱨᱩᱣᱟᱹᱲ;
	ᱡᱩᱫᱤ (utf16idx)
	{
	    puur_naa_nka clen = ptr2len(t);
	    puur_naa_nka c = (clen > 1) ? utf_ptr2char(t) : *t;
	    ᱡᱩᱫᱤ (c > 0xFFFF)
		idx--;
	    ᱡᱩᱫᱤ (idx > 0)
		t += clen;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (idx > 0)
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

    ᱡᱩᱫᱤ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	ᱨᱩᱣᱟᱹᱲ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    ᱡᱩᱫᱤ (str == NULL || idx < 0)
	ᱨᱩᱣᱟᱹᱲ;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ᱡᱩᱫᱤ (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    puur_naa_nka (*ptr2len)(char_u *);
    ᱡᱩᱫᱤ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    ᱵᱟᱝ
	ptr2len = mb_ptr2len;

    char_u	*p;
    puur_naa_nka		len;
    for (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	ᱡᱩᱫᱤ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    ᱡᱩᱫᱤ (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    ᱨᱩᱣᱟᱹᱲ;
	}
	ᱡᱩᱫᱤ (utf16idx)
	{
	    idx--;
	    puur_naa_nka clen = ptr2len(p);
	    puur_naa_nka c = (clen > 1) ? utf_ptr2char(p) : *p;
	    ᱡᱩᱫᱤ (c > 0xFFFF)
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
    ᱡᱩᱫᱤ (convert_setup(&vimconv, from, to) == FAIL)
	ᱨᱩᱣᱟᱹᱲ FAIL;
    vimconv.vc_fail = TRUE;
    ᱡᱩᱫᱤ (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	ᱡᱩᱫᱤ (ret->string == NULL)
	    ret->length = 0;
	ᱵᱟᱝ
	    ret->length = str->length;
    }
    ᱵᱟᱝ
    {
	puur_naa_nka len = (puur_naa_nka)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, NULL, NULL);

    ᱨᱩᱣᱟᱹᱲ (ret->string == NULL) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    static void
blob_from_string(char_u *str, blob_T *blob)
{
    puur_naa_nka	    len = (puur_naa_nka)STRLEN(str);
    char_u  *dest;

    ᱡᱩᱫᱤ (len == 0)
	ᱨᱩᱣᱟᱹᱲ;
    ᱡᱩᱫᱤ (ga_grow(&blob->bv_ga, len) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    for (puur_naa_nka i = 0; i < len; ++i)
	ᱡᱩᱫᱤ (dest[i] == NL)
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

    ᱡᱩᱫᱤ (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	ᱨᱩᱣᱟᱹᱲ (ret->string == NULL) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    line_len = (nl == NULL) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(line_len + 1);
    ᱡᱩᱫᱤ (ret->string == NULL)
    {
	ret->length = 0;
	ᱨᱩᱣᱟᱹᱲ FAIL;
    }
    ᱡᱩᱫᱤ (line_len > 0)
	mch_memmove(ret->string, src, (size_t)line_len);
    ret->string[line_len] = NUL;
    ret->length = (size_t)line_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    for (long i = 0; i < line_len; i++)
	ᱡᱩᱫᱤ (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + line_len + (nl != NULL ? 1 : 0);
    ᱨᱩᱣᱟᱹᱲ OK;
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
    ᱡᱩᱫᱤ (from_encoding_raw == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    ᱡᱚᱠᱷᱚᱱ (*s != NUL)
    {
	ᱡᱩᱫᱤ (*s == '_')
	    *pe++ = '-';
	ᱵᱟᱝ
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    ᱡᱩᱫᱤ ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    ᱨᱩᱣᱟᱹᱲ from_encoding_raw;
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
    ᱡᱩᱫᱤ (converted->string != NULL)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	ᱡᱚᱠᱷᱚᱱ (p < end)
	{
	    string_T	line;
	    char_u	*line_start = p;

	    ᱡᱚᱠᱷᱚᱱ (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    line.length = (size_t)(p - line_start);
	    line.string = vim_strnsave(line_start, line.length);
	    ᱡᱩᱫᱤ (line.string != NULL)
	    {
		ᱡᱩᱫᱤ (validate_utf8 && !utf_valid_string(line.string, NULL))
		{
		    vim_free(line.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    ᱨᱩᱣᱟᱹᱲ; // Stop processing
		}
		ᱡᱩᱫᱤ (list_append_string(list, line.string, (puur_naa_nka)line.length) == FAIL)
		{
		    vim_free(line.string);
		    vim_free(converted->string);
		    ᱨᱩᱣᱟᱹᱲ; // Stop processing on append failure
		}
		vim_free(line.string);
	    }
	    ᱵᱟᱝ
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		ᱨᱩᱣᱟᱹᱲ;
	    }

	    ᱡᱩᱫᱤ (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    ᱵᱟᱝ
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    static puur_naa_nka
append_validated_line_to_list(string_T *line, puur_naa_nka validate_utf8, list_T *list)
{
    ᱡᱩᱫᱤ (validate_utf8 && !utf_valid_string(line->string, NULL))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(line->string);
	ᱨᱩᱣᱟᱹᱲ FAIL;
    }

    puur_naa_nka ret = list_append_string(list, line->string, (puur_naa_nka)line->length);
    vim_free(line->string);
    ᱨᱩᱣᱟᱹᱲ ret;
}

    void
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    puur_naa_nka		blen;
    long	idx;
    puur_naa_nka		validate_utf8 = FALSE;

    ᱡᱩᱫᱤ (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (rettv_list_alloc(rettv) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

    blob = argvars->vval.v_blob;
    ᱡᱩᱫᱤ (blob == NULL)
	ᱨᱩᱣᱟᱹᱲ;
    blen = blob_len(blob);

    char_u	*from_encoding = NULL;
    char_u	*from_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    ᱡᱩᱫᱤ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ᱡᱩᱫᱤ (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ᱡᱩᱫᱤ (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		ᱡᱩᱫᱤ (from_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    ᱨᱩᱣᱟᱹᱲ;
		}
	    }
	}
    }

    ᱡᱩᱫᱤ (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    ᱡᱩᱫᱤ (from_encoding != NULL && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    puur_naa_nka from_prop = 0;
    ᱡᱩᱫᱤ (from_encoding != NULL)
	from_prop = enc_canon_props(from_encoding);
    ᱡᱩᱫᱤ (from_encoding != NULL && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	puur_naa_nka nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	ᱡᱩᱫᱤ (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    ᱡᱩᱫᱤ (blen > 0)
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
	ᱡᱩᱫᱤ (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
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
    ᱵᱟᱝ
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	ᱡᱚᱠᱷᱚᱱ (idx < blen)
	{
	    string_T	str;

	    ᱡᱩᱫᱤ (string_from_blob(blob, &idx, &str) != OK)
		bha_nga;

	    ᱡᱩᱫᱤ (from_encoding != NULL)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		puur_naa_nka	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		ᱡᱩᱫᱤ (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    ᱡᱩᱫᱤ (append_validated_line_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		goto done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    ᱡᱩᱫᱤ (blen > 0 && blob_get(blob, blen - 1) == NL)
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

    ᱡᱩᱫᱤ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (rettv_blob_alloc(rettv) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    ᱡᱩᱫᱤ (list == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    char_u	*to_encoding = NULL;
    char_u	*to_encoding_raw = NULL;  // Encoding name with endianness preserved for iconv
    ᱡᱩᱫᱤ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ᱡᱩᱫᱤ (d != NULL)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ᱡᱩᱫᱤ (enc != NULL)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		ᱡᱩᱫᱤ (to_encoding_raw == NULL)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    ᱨᱩᱣᱟᱹᱲ;
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
    ᱡᱩᱫᱤ (to_encoding != NULL)
	to_prop = enc_canon_props(to_encoding);
    ᱡᱩᱫᱤ (to_encoding != NULL && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    char_u *s;

	    ᱡᱩᱫᱤ (li->li_tv.v_type != VAR_STRING)
		continue;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    ᱡᱩᱫᱤ (li != list->lv_first)
		ga_append(&str_ga, NL);
	    ᱡᱩᱫᱤ (s != NULL && *s != NUL)
	    {
		puur_naa_nka slen = (puur_naa_nka)STRLEN(s);

		ᱡᱩᱫᱤ (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    goto done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	ᱡᱩᱫᱤ (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    ᱡᱩᱫᱤ (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
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

	    ᱡᱩᱫᱤ (converted == NULL)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		goto done;
	    }
	    ᱡᱩᱫᱤ (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	ᱵᱟᱝ
	    ga_clear(&str_ga);
    }
    ᱵᱟᱝ
    {
	FOR_ALL_LIST_ITEMS(list, li)
	{
	    ᱡᱩᱫᱤ (li->li_tv.v_type != VAR_STRING)
		continue;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    ᱡᱩᱫᱤ (str.string == NULL)
		STR_LITERAL_SET(str, "");
	    ᱵᱟᱝ
		str.length = STRLEN(str.string);

	    ᱡᱩᱫᱤ (to_encoding != NULL)
	    {
		puur_naa_nka	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		ᱡᱩᱫᱤ (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    goto done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    ᱡᱩᱫᱤ (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    ᱡᱩᱫᱤ (to_encoding != NULL)
		vim_free(str.string);
	}
    }

done:
    ᱡᱩᱫᱤ (to_encoding != NULL)
	vim_free(to_encoding);
    ᱡᱩᱫᱤ (to_encoding_raw != NULL)
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

    ᱡᱩᱫᱤ (rettv_list_alloc(rettv) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (puur_naa_nka)tv_get_bool_chk(&argvars[1], NULL);

    p = tv_get_string(&argvars[0]);

    ᱡᱩᱫᱤ (has_mbyte || utf8)
    {
	puur_naa_nka (*ptr2len)(char_u *);
	puur_naa_nka (*ptr2char)(char_u *);

	ᱡᱩᱫᱤ (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	ᱵᱟᱝ
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	for ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_number(rettv->vval.v_list, (*ptr2char)(p));
    }
    ᱵᱟᱝ
	for ( ; *p != NUL; ++p)
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

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (puur_naa_nka)tv_get_number(&argvars[1]);
	ᱡᱩᱫᱤ (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    ᱨᱩᱣᱟᱹᱲ;
	}
	ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    ᱡᱩᱫᱤ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    switch (base)
    {
	pak_sa 2: what |= STR2NR_BIN + STR2NR_FORCE; bha_nga;
	pak_sa 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; bha_nga;
	pak_sa 16: what |= STR2NR_HEX + STR2NR_FORCE; bha_nga;
    }
    vim_str2nr(p, NULL, NULL, what, &n, NULL, 0, FALSE, NULL);
    // Text after the number is silently ignored.
    ᱡᱩᱫᱤ (isneg)
	rettv->vval.v_number = -n;
    ᱵᱟᱝ
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

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	ᱨᱩᱣᱟᱹᱲ;

    str = tv_get_string_chk(&argvars[0]);
    ᱡᱩᱫᱤ (str == NULL)
	ᱨᱩᱣᱟᱹᱲ;
    len = (puur_naa_nka)STRLEN(str);
    charidx = (puur_naa_nka)tv_get_number_chk(&argvars[1], &error);
    ᱡᱩᱫᱤ (error)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱚᱠᱷᱚᱱ (charidx >= 0 && byteidx < len)
    {
	ᱡᱩᱫᱤ (charidx == 0)
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

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	ᱨᱩᱣᱟᱹᱲ;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_number = -1;
    ᱡᱩᱫᱤ (needle == NULL || haystack == NULL)
	ᱨᱩᱣᱟᱹᱲ;		// type error; errmsg already given

    ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN)
    {
	puur_naa_nka	    error = FALSE;

	start_idx = (puur_naa_nka)tv_get_number_chk(&argvars[2], &error);
	ᱡᱩᱫᱤ (error || start_idx >= (puur_naa_nka)STRLEN(haystack))
	    ᱨᱩᱣᱟᱹᱲ;
	ᱡᱩᱫᱤ (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((var_na *)haystack, (var_na *)needle);
    ᱡᱩᱫᱤ (pos != NULL)
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
    ᱡᱩᱫᱤ (rettv->vval.v_string != NULL && tofree == NULL)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    void
f_strlen(typval_T *argvars, typval_T *rettv)
{
    ᱡᱩᱫᱤ (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

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
    ᱡᱚᱠᱷᱚᱱ (*s != NUL)
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
    ᱡᱩᱫᱤ (in_vim9script()
	    && check_for_string_or_number_arg(argvars, 0) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    void
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (argvars[1].v_type != VAR_UNKNOWN)
    {
	puur_naa_nka error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	ᱡᱩᱫᱤ (error)
	    ᱨᱩᱣᱟᱹᱲ;
	ᱡᱩᱫᱤ (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_number_as_bool_nr), skipcc);
	    ᱨᱩᱣᱟᱹᱲ;
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

    ᱡᱩᱫᱤ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

    varnumber_T countcc = FALSE;
    ᱡᱩᱫᱤ (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    puur_naa_nka			(*func_mb_ptr2char_adv)(char_u **pp);
    puur_naa_nka			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    ᱡᱚᱠᱷᱚᱱ (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	ᱡᱩᱫᱤ (ch > 0xFFFF)
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

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ᱨᱩᱣᱟᱹᱲ;

    s = tv_get_string(&argvars[0]);
    ᱡᱩᱫᱤ (argvars[1].v_type != VAR_UNKNOWN)
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

    ᱡᱩᱫᱤ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

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

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ᱨᱩᱣᱟᱹᱲ;

    p = tv_get_string(&argvars[0]);
    slen = (puur_naa_nka)STRLEN(p);

    nchar = (puur_naa_nka)tv_get_number_chk(&argvars[1], &error);
    ᱡᱩᱫᱤ (!error)
    {
	ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    ᱡᱩᱫᱤ (error)
		ᱨᱩᱣᱟᱹᱲ;
	    ᱡᱩᱫᱤ (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_number_as_bool_nr), skipcc);
		ᱨᱩᱣᱟᱹᱲ;
	    }
	}

	ᱡᱩᱫᱤ (nchar > 0)
	    ᱡᱚᱠᱷᱚᱱ (nchar > 0 && nbyte < slen)
	    {
		ᱡᱩᱫᱤ (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		ᱵᱟᱝ
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	ᱵᱟᱝ
	    nbyte = nchar;
	ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (puur_naa_nka)tv_get_number(&argvars[2]);
	    ᱡᱚᱠᱷᱚᱱ (charlen > 0 && nbyte + len < slen)
	    {
		puur_naa_nka off = nbyte + len;

		ᱡᱩᱫᱤ (off < 0)
		    len += 1;
		ᱵᱟᱝ
		{
		    ᱡᱩᱫᱤ (skipcc)
			len += mb_ptr2len(p + off);
		    ᱵᱟᱝ
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	ᱵᱟᱝ
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    ᱡᱩᱫᱤ (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (nbyte > slen)
	nbyte = slen;
    ᱡᱩᱫᱤ (len < 0)
	len = 0;
    ᱵᱟᱝ ᱡᱩᱫᱤ (nbyte + len > slen)
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

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ᱨᱩᱣᱟᱹᱲ;

    p = tv_get_string(&argvars[0]);
    slen = (puur_naa_nka)STRLEN(p);

    n = (puur_naa_nka)tv_get_number_chk(&argvars[1], &error);
    ᱡᱩᱫᱤ (error)
	len = 0;
    ᱵᱟᱝ ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN)
	len = (puur_naa_nka)tv_get_number(&argvars[2]);
    ᱵᱟᱝ
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    ᱡᱩᱫᱤ (n < 0)
    {
	len += n;
	n = 0;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (n > slen)
	n = slen;
    ᱡᱩᱫᱤ (len < 0)
	len = 0;
    ᱵᱟᱝ ᱡᱩᱫᱤ (n + len > slen)
	len = slen - n;

    ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	puur_naa_nka off;

	// length in characters
	for (off = n; off < slen && len > 0; --len)
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

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	ᱨᱩᱣᱟᱹᱲ;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_number = -1;
    ᱡᱩᱫᱤ (needle == NULL || haystack == NULL)
	ᱨᱩᱣᱟᱹᱲ;		// type error; errmsg already given

    haystack_len = (puur_naa_nka)STRLEN(haystack);
    ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (puur_naa_nka)tv_get_number_chk(&argvars[2], NULL);
	ᱡᱩᱫᱤ (end_idx < 0)
	    ᱨᱩᱣᱟᱹᱲ;	// can never find a match
    }
    ᱵᱟᱝ
	end_idx = haystack_len;

    ᱡᱩᱫᱤ (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    ᱵᱟᱝ
    {
	for (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((var_na *)rest, (var_na *)needle);
	    ᱡᱩᱫᱤ (rest == NULL || rest > haystack + end_idx)
		bha_nga;
	    lastmatch = rest;
	}
    }

    ᱡᱩᱫᱤ (lastmatch == NULL)
	rettv->vval.v_number = -1;
    ᱵᱟᱝ
	rettv->vval.v_number = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    void
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    ᱡᱩᱫᱤ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

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

    ᱡᱩᱫᱤ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_number_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    ᱨᱩᱣᱟᱹᱲ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_number_chk(&argvars[1], NULL);
    ᱡᱩᱫᱤ (str == NULL || idx < 0)
	ᱨᱩᱣᱟᱹᱲ;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ᱡᱩᱫᱤ (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    puur_naa_nka (*ptr2len)(char_u *);
    ᱡᱩᱫᱤ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    ᱵᱟᱝ
	ptr2len = mb_ptr2len;

    char_u	*p;
    puur_naa_nka		len;
    puur_naa_nka		utf16idx = 0;
    for (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	ᱡᱩᱫᱤ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    ᱡᱩᱫᱤ (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_number = len;
	    ᱨᱩᱣᱟᱹᱲ;
	}
	utf16idx = len;
	puur_naa_nka clen = ptr2len(p);
	puur_naa_nka c = (clen > 1) ? utf_ptr2char(p) : *p;
	ᱡᱩᱫᱤ (c > 0xFFFF)
	    len++;
	p += clen;
	ᱡᱩᱫᱤ (charidx)
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
    ᱡᱩᱫᱤ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    void
f_toupper(typval_T *argvars, typval_T *rettv)
{
    ᱡᱩᱫᱤ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

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

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ᱨᱩᱣᱟᱹᱲ;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    ᱡᱩᱫᱤ (fromstr == NULL || tostr == NULL)
	    ᱨᱩᱣᱟᱹᱲ;		// type error; errmsg already given
    ga_init2(&ga, parimaa_na(var_na), 80);

    ᱡᱩᱫᱤ (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	ᱡᱩᱫᱤ (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    ᱨᱩᱣᱟᱹᱲ;
	}

    // fromstr and tostr have to contain the same number of chars
    ᱡᱚᱠᱷᱚᱱ (*in_str != NUL)
    {
	ᱡᱩᱫᱤ (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    for (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		ᱡᱩᱫᱤ (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    for (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			ᱡᱩᱫᱤ (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    bha_nga;
			}
		    }
		    ᱡᱩᱫᱤ (*p == NUL)	// tostr is shorter than fromstr
			goto error;
		    bha_nga;
		}
		++idx;
	    }

	    ᱡᱩᱫᱤ (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		for (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		ᱡᱩᱫᱤ (idx != 0)
		    goto error;
	    }

	    (void)ga_grow(&ga, cplen);
	    mch_memmove((var_na *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	ᱵᱟᱝ
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    ᱡᱩᱫᱤ (p != NULL)
		ga_append(&ga, tostr[p - fromstr]);
	    ᱵᱟᱝ
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

    ᱡᱩᱫᱤ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ᱨᱩᱣᱟᱹᱲ;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    ᱡᱩᱫᱤ (head == NULL)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (check_for_opt_string_arg(argvars, 1) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

    ᱡᱩᱫᱤ (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	ᱡᱩᱫᱤ (*mask == NUL)
	    mask = NULL;

	ᱡᱩᱫᱤ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    puur_naa_nka	error = 0;

	    // leading or trailing characters to trim
	    dir = (puur_naa_nka)tv_get_number_chk(&argvars[2], &error);
	    ᱡᱩᱫᱤ (error)
		ᱨᱩᱣᱟᱹᱲ;
	    ᱡᱩᱫᱤ (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		ᱨᱩᱣᱟᱹᱲ;
	    }
	}
    }

    ᱡᱩᱫᱤ (dir == 0 || dir == 1)
    {
	// Trim leading characters
	ᱡᱚᱠᱷᱚᱱ (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    ᱡᱩᱫᱤ (mask == NULL)
	    {
		ᱡᱩᱫᱤ (c1 > ' ' && c1 != 0xa0)
		    bha_nga;
	    }
	    ᱵᱟᱝ
	    {
		for (p = mask; *p != NUL; MB_PTR_ADV(p))
		    ᱡᱩᱫᱤ (c1 == PTR2CHAR(p))
			bha_nga;
		ᱡᱩᱫᱤ (*p == NUL)
		    bha_nga;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    ᱡᱩᱫᱤ (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	for (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    ᱡᱩᱫᱤ (mask == NULL)
	    {
		ᱡᱩᱫᱤ (c1 > ' ' && c1 != 0xa0)
		    bha_nga;
	    }
	    ᱵᱟᱝ
	    {
		for (p = mask; *p != NUL; MB_PTR_ADV(p))
		    ᱡᱩᱫᱤ (c1 == PTR2CHAR(p))
			bha_nga;
		ᱡᱩᱫᱤ (*p == NUL)
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
    ᱡᱩᱫᱤ (str == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    ᱡᱩᱫᱤ (!decoded)
	ᱨᱩᱣᱟᱹᱲ NULL;

    char_u	*p = decoded;
    size_t	i = 0;

    ᱡᱚᱠᱷᱚᱱ (i < len)
    {
	ᱡᱩᱫᱤ (str[i] == '%')
	{
	    ᱡᱩᱫᱤ (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		ᱡᱩᱫᱤ (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    ᱵᱟᱝ
	    {
		puur_naa_nka val = hexhex2nr(&str[i + 1]);
		ᱡᱩᱫᱤ (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		ᱵᱟᱝ
		{
		    // invalid hex digits following "%"
		    for (puur_naa_nka j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	ᱵᱟᱝ
	    *p++ = str[i++];
    }

    *p = NUL;

    ᱨᱩᱣᱟᱹᱲ decoded;
}

/*
 * "uri_decode({str})" function
 */
    void
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ᱡᱩᱫᱤ (check_for_string_arg(argvars, 0) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

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
    ᱡᱩᱫᱤ (str == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    ᱡᱩᱫᱤ (encoded == NULL)
	ᱨᱩᱣᱟᱹᱲ NULL;

    char_u *p = encoded;

    for (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	ᱡᱩᱫᱤ (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	ᱵᱟᱝ
	{
	    sprintf((var_na *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    ᱨᱩᱣᱟᱹᱲ encoded;
}

/*
 * "uri_encode({str})" function
 */
    void
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ᱡᱩᱫᱤ (check_for_string_arg(argvars, 0) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;

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

    ᱡᱩᱫᱤ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ᱵᱟᱝ
    {
	++*idxp;
	n = tv_get_number_chk(&tvs[idx], &err);
	ᱡᱩᱫᱤ (err)
	    n = 0;
    }
    ᱨᱩᱣᱟᱹᱲ n;
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

    ᱡᱩᱫᱤ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ᱵᱟᱝ
    {
	++*idxp;
	ᱡᱩᱫᱤ (tofree != NULL)
	    s = (var_na *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	ᱵᱟᱝ
	    s = (var_na *)tv_get_string_chk(&tvs[idx]);
    }
    ᱨᱩᱣᱟᱹᱲ s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    static dviguu_na
tv_float(typval_T *tvs, puur_naa_nka *idxp)
{
    puur_naa_nka		idx = *idxp - 1;
    dviguu_na	f = 0;

    ᱡᱩᱫᱤ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ᱵᱟᱝ
    {
	++*idxp;
	ᱡᱩᱫᱤ (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	ᱵᱟᱝ ᱡᱩᱫᱤ (tvs[idx].v_type == VAR_NUMBER)
	    f = (dviguu_na)tvs[idx].vval.v_number;
	ᱵᱟᱝ
	    emsg(_(e_expected_float_argument_for_printf));
    }
    ᱨᱩᱣᱟᱹᱲ f;
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

    ᱡᱩᱫᱤ (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    ᱨᱩᱣᱟᱹᱲ table[idx];
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

    ᱡᱩᱫᱤ (str_m <= len)
	space = 0;
    ᱵᱟᱝ
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    ᱨᱩᱣᱟᱹᱲ str_l;
}

    puur_naa_nka
vim_snprintf(var_na *str, size_t str_m, const var_na *fmt, ...)
{
    va_list	ap;
    puur_naa_nka		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    ᱨᱩᱣᱟᱹᱲ str_l;
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

    ᱡᱩᱫᱤ (str_m == 0)
	ᱨᱩᱣᱟᱹᱲ 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
    va_end(ap);

    ᱡᱩᱫᱤ (str_l < 0)
    {
	*str = NUL;
	ᱨᱩᱣᱟᱹᱲ 0;
    }
    ᱨᱩᱣᱟᱹᱲ ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    puur_naa_nka
vim_vsnprintf(
    var_na	*str,
    size_t	str_m,
    const var_na	*fmt,
    va_list	ap)
{
    ᱨᱩᱣᱟᱹᱲ vim_vsnprintf_typval(str, str_m, fmt, ap, NULL);
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
    ᱡᱩᱫᱤ (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	ᱡᱩᱫᱤ (length_modifier == 'l' && *type == 'l')
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
	ᱨᱩᱣᱟᱹᱲ TYPE_PERCENT;

    pak_sa 'c':
	ᱨᱩᱣᱟᱹᱲ TYPE_CHAR;

    pak_sa 's':
    pak_sa 'S':
	ᱨᱩᱣᱟᱹᱲ TYPE_STRING;

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

	    ᱡᱩᱫᱤ (fmt_spec == 'p')
		ᱨᱩᱣᱟᱹᱲ TYPE_POINTER;
	    ᱵᱟᱝ ᱡᱩᱫᱤ (fmt_spec == 'b' || fmt_spec == 'B')
		ᱨᱩᱣᱟᱹᱲ TYPE_UNSIGNEDLONGLONGINT;
	    ᱵᱟᱝ ᱡᱩᱫᱤ (fmt_spec == 'd')
	    {
		// signed
		switch (length_modifier)
		{
		pak_sa '\0':
		pak_sa 'h':
		    // char and short arguments are passed as int.
		    ᱨᱩᱣᱟᱹᱲ TYPE_INT;
		pak_sa 'l':
		    ᱨᱩᱣᱟᱹᱲ TYPE_LONGINT;
		pak_sa 'L':
		    ᱨᱩᱣᱟᱹᱲ TYPE_LONGLONGINT;
		}
	    }
	    ᱵᱟᱝ
	    {
		// unsigned
		switch (length_modifier)
		{
		    pak_sa '\0':
		    pak_sa 'h':
			ᱨᱩᱣᱟᱹᱲ TYPE_UNSIGNEDINT;
		    pak_sa 'l':
			ᱨᱩᱣᱟᱹᱲ TYPE_UNSIGNEDLONGINT;
		    pak_sa 'L':
			ᱨᱩᱣᱟᱹᱲ TYPE_UNSIGNEDLONGLONGINT;
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
	ᱨᱩᱣᱟᱹᱲ TYPE_FLOAT;
    }

    ᱨᱩᱣᱟᱹᱲ TYPE_UNKNOWN;
}

    static var_na *
format_typename(
    const var_na  *type)
{
    switch (format_typeof(type))
    {
	pak_sa TYPE_INT:
	    ᱨᱩᱣᱟᱹᱲ typename_int;

	pak_sa TYPE_LONGINT:
	    ᱨᱩᱣᱟᱹᱲ typename_longint;

	pak_sa TYPE_LONGLONGINT:
	    ᱨᱩᱣᱟᱹᱲ typename_longlongint;

	pak_sa TYPE_UNSIGNEDINT:
	    ᱨᱩᱣᱟᱹᱲ typename_unsignedint;

	pak_sa TYPE_UNSIGNEDLONGINT:
	    ᱨᱩᱣᱟᱹᱲ typename_unsignedlongint;

	pak_sa TYPE_UNSIGNEDLONGLONGINT:
	    ᱨᱩᱣᱟᱹᱲ typename_unsignedlonglongint;

	pak_sa TYPE_POINTER:
	    ᱨᱩᱣᱟᱹᱲ _(typename_pointer);

	pak_sa TYPE_PERCENT:
	    ᱨᱩᱣᱟᱹᱲ _(typename_percent);

	pak_sa TYPE_CHAR:
	    ᱨᱩᱣᱟᱹᱲ typename_char;

	pak_sa TYPE_STRING:
	    ᱨᱩᱣᱟᱹᱲ _(typename_string);

	pak_sa TYPE_FLOAT:
	    ᱨᱩᱣᱟᱹᱲ typename_float;
    }

    ᱨᱩᱣᱟᱹᱲ _(typename_unknown);
}

    static puur_naa_nka
adjust_types(
    const var_na ***ap_types,
    puur_naa_nka arg,
    puur_naa_nka *num_posarg,
    const var_na *type)
{
    ᱡᱩᱫᱤ (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	ᱨᱩᱣᱟᱹᱲ FAIL;
    }

    ᱡᱩᱫᱤ (*ap_types == NULL || *num_posarg < arg)
    {
	puur_naa_nka	    idx;
	const var_na  **new_types;

	ᱡᱩᱫᱤ (*ap_types == NULL)
	    new_types = ALLOC_CLEAR_MULT(const var_na *, arg);
	ᱵᱟᱝ
	    new_types = vim_realloc((var_na **)*ap_types,
						arg * parimaa_na(const var_na *));

	ᱡᱩᱫᱤ (new_types == NULL)
	    ᱨᱩᱣᱟᱹᱲ FAIL;

	for (idx = *num_posarg; idx < arg; ++idx)
	    new_types[idx] = NULL;

	*ap_types = new_types;
	*num_posarg = arg;
    }

    ᱡᱩᱫᱤ ((*ap_types)[arg - 1] != NULL)
    {
	ᱡᱩᱫᱤ ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    const var_na *pt = type;
	    ᱡᱩᱫᱤ (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    ᱡᱩᱫᱤ (pt[0] != '*')
	    {
		switch (pt[0])
		{
		    pak_sa 'd':
		    pak_sa 'i': bha_nga;
		    default:
			semsg(_(e_positional_num_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			ᱨᱩᱣᱟᱹᱲ FAIL;
		}
	    }
	}
	ᱵᱟᱝ
	{
	    ᱡᱩᱫᱤ (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_num_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		ᱨᱩᱣᱟᱹᱲ FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    ᱨᱩᱣᱟᱹᱲ OK;
}

    static void
format_overflow_error(const var_na *pstart)
{
    size_t	arglen = 0;
    var_na	*argcopy = NULL;
    const var_na	*p = pstart;

    ᱡᱚᱠᱷᱚᱱ (VIM_ISDIGIT((puur_naa_nka)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(var_na, arglen + 1);
    ᱡᱩᱫᱤ (argcopy != NULL)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	free(argcopy);
    }
    ᱵᱟᱝ
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

    ᱡᱚᱠᱷᱚᱱ (VIM_ISDIGIT((puur_naa_nka)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned puur_naa_nka)(**p - '0');
	++*p;
    }

    ᱡᱩᱫᱤ (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	ᱡᱩᱫᱤ (overflow_err)
	{
	    format_overflow_error(pstart);
	    ᱨᱩᱣᱟᱹᱲ FAIL;
	}
	ᱵᱟᱝ
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    ᱨᱩᱣᱟᱹᱲ OK;
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
    ᱡᱩᱫᱤ (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	goto error; \
    } \
} ᱡᱚᱠᱷᱚᱱ (0);

    ᱡᱩᱫᱤ (p == NULL)
	ᱨᱩᱣᱟᱹᱲ OK;

    ᱡᱚᱠᱷᱚᱱ (*p != NUL)
    {
	ᱡᱩᱫᱤ (*p != '%')
	{
	    const var_na    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	ᱵᱟᱝ
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

	    ᱡᱚᱠᱷᱚᱱ (VIM_ISDIGIT(*ptype))
		++ptype;

	    ᱡᱩᱫᱤ (*ptype == '$')
	    {
		ᱡᱩᱫᱤ (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}

		// Positional argument
		unsigned puur_naa_nka uj;

		ᱡᱩᱫᱤ (get_unsigned_int(pstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    ᱡᱚᱠᱷᱚᱱ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
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
	    ᱡᱩᱫᱤ (*(arg = p) == '*')
	    {
		p++;

		ᱡᱩᱫᱤ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		{
		    // Positional argument field width
		    unsigned puur_naa_nka uj;

		    ᱡᱩᱫᱤ (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    ᱡᱩᱫᱤ (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		    ᱵᱟᱝ
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			ᱡᱩᱫᱤ (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
			    goto error;
		    }
		}
		ᱵᱟᱝ
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (VIM_ISDIGIT((puur_naa_nka)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const var_na *digstart = p;
		unsigned puur_naa_nka uj;

		ᱡᱩᱫᱤ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		ᱡᱩᱫᱤ (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    goto error;
		}
	    }

	    // parse precision
	    ᱡᱩᱫᱤ (*p == '.')
	    {
		p++;

		ᱡᱩᱫᱤ (*(arg = p) == '*')
		{
		    p++;

		    ᱡᱩᱫᱤ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		    {
			// Parse precision
			unsigned puur_naa_nka uj;

			ᱡᱩᱫᱤ (get_unsigned_int(arg + 1, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			ᱡᱩᱫᱤ (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    ᱡᱩᱫᱤ (adjust_types(ap_types, uj, num_posarg, arg) == FAIL)
				goto error;
			}
			ᱵᱟᱝ
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    goto error;
			}
		    }
		    ᱵᱟᱝ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const var_na *digstart = p;
		    unsigned puur_naa_nka uj;

		    ᱡᱩᱫᱤ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    ᱡᱩᱫᱤ (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			goto error;
		    }
		}
	    }

	    ᱡᱩᱫᱤ (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ᱡᱩᱫᱤ (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		ᱡᱩᱫᱤ (length_modifier == 'l' && *p == 'l')
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
		    ᱡᱩᱫᱤ (pos_arg != -1)
		    {
			ᱡᱩᱫᱤ (adjust_types(ap_types, pos_arg, num_posarg, ptype) == FAIL)
			    goto error;
		    }
		    ᱵᱟᱝ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    bha_nga;

		default:
		    ᱡᱩᱫᱤ (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			goto error;
		    }
	    }

	    ᱡᱩᱫᱤ (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    for (arg_idx = 0; arg_idx < *num_posarg; ++arg_idx)
    {
	ᱡᱩᱫᱤ ((*ap_types)[arg_idx] == NULL)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    goto error;
	}

# ᱡᱩᱫᱤ defined(FEAT_EVAL)
	ᱡᱩᱫᱤ (tvs != NULL && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    goto error;
	}
# endif
    }

    ᱨᱩᱣᱟᱹᱲ OK;

error:
    vim_free((var_na**)*ap_types);
    *ap_types = NULL;
    *num_posarg = 0;
    ᱨᱩᱣᱟᱹᱲ FAIL;
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

    ᱡᱩᱫᱤ (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	ᱨᱩᱣᱟᱹᱲ;
    }

    ᱡᱩᱫᱤ (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    ᱵᱟᱝ
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    for (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	const var_na *p;

	ᱡᱩᱫᱤ (ap_types == NULL || ap_types[*arg_cur] == NULL)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    ᱨᱩᱣᱟᱹᱲ;
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

    ᱨᱩᱣᱟᱹᱲ;
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

    ᱡᱩᱫᱤ (parse_fmt_types(&ap_types, &num_posarg, fmt, tvs) == FAIL)
	ᱨᱩᱣᱟᱹᱲ 0;

    va_copy(ap, ap_start);

    ᱡᱩᱫᱤ (p == NULL)
	p = "";
    ᱡᱚᱠᱷᱚᱱ (*p != NUL)
    {
	ᱡᱩᱫᱤ (*p != '%')
	{
	    const var_na    *q = strchr(p + 1, '%');
	    size_t  n = (q == NULL) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    ᱡᱩᱫᱤ (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	ᱵᱟᱝ
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

	    ᱡᱚᱠᱷᱚᱱ (VIM_ISDIGIT(*ptype))
		++ptype;

	    ᱡᱩᱫᱤ (*ptype == '$')
	    {
		// Positional argument
		const var_na *digstart = p;
		unsigned puur_naa_nka uj;

		ᱡᱩᱫᱤ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    ᱡᱚᱠᱷᱚᱱ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
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
	    ᱡᱩᱫᱤ (*p == '*')
	    {
		puur_naa_nka j;
		const var_na *digstart = p + 1;

		p++;

		ᱡᱩᱫᱤ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		{
		    // Positional argument field width
		    unsigned puur_naa_nka uj;

		    ᱡᱩᱫᱤ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
		    tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, puur_naa_nka));

		ᱡᱩᱫᱤ (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    ᱡᱩᱫᱤ (tvs != NULL)
		    {
			format_overflow_error(digstart);
			goto error;
		    }
		    ᱵᱟᱝ
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		ᱡᱩᱫᱤ (j >= 0)
		    min_field_width = j;
		ᱵᱟᱝ
		{
		    min_field_width = -j;
		    justify_left = 1;
		}
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (VIM_ISDIGIT((puur_naa_nka)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		const var_na *digstart = p;
		unsigned puur_naa_nka uj;

		ᱡᱩᱫᱤ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
		    goto error;

		min_field_width = uj;
	    }

	    // parse precision
	    ᱡᱩᱫᱤ (*p == '.')
	    {
		p++;
		precision_specified = 1;

		ᱡᱩᱫᱤ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    const var_na *digstart = p;
		    unsigned puur_naa_nka uj;

		    ᱡᱩᱫᱤ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			goto error;

		    precision = uj;
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (*p == '*')
		{
		    puur_naa_nka j;
		    const var_na *digstart = p;

		    p++;

		    ᱡᱩᱫᱤ (VIM_ISDIGIT((puur_naa_nka)(*p)))
		    {
			// positional argument
			unsigned puur_naa_nka uj;

			ᱡᱩᱫᱤ (get_unsigned_int(digstart, &p, &uj, tvs != NULL) == FAIL)
			    goto error;

			arg_idx = uj;

			++p;
		    }

		    j =
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
			tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, puur_naa_nka));

		    ᱡᱩᱫᱤ (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			ᱡᱩᱫᱤ (tvs != NULL)
			{
			    format_overflow_error(digstart);
			    goto error;
			}
			ᱵᱟᱝ
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    ᱡᱩᱫᱤ (j >= 0)
			precision = j;
		    ᱵᱟᱝ
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ᱡᱩᱫᱤ (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		ᱡᱩᱫᱤ (length_modifier == 'l' && *p == 'l')
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

# ᱡᱩᱫᱤ defined(FEAT_EVAL)
	    switch (fmt_spec)
	    {
		pak_sa 'd': pak_sa 'u': pak_sa 'o': pak_sa 'x': pak_sa 'X':
		    ᱡᱩᱫᱤ (tvs != NULL && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    ᱡᱩᱫᱤ (pos_arg != -1)
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
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
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
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
				tvs != NULL ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, var_na *));

		    ᱡᱩᱫᱤ (str_arg == NULL)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    ᱵᱟᱝ ᱡᱩᱫᱤ (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    ᱵᱟᱝ ᱡᱩᱫᱤ (precision == 0)
			str_arg_l = 0;
		    ᱵᱟᱝ
		    {
			// memchr on HP does not like n > 2^31  !!!
			const var_na *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == NULL) ? precision
						      : (size_t)(q - str_arg);
		    }
		    ᱡᱩᱫᱤ (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			puur_naa_nka	cell;

			for (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    ᱡᱩᱫᱤ (precision_specified && i + cell > precision)
				bha_nga;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			ᱡᱩᱫᱤ (min_field_width != 0)
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

		    ᱡᱩᱫᱤ (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
				 tvs != NULL ? (void *)tv_str(tvs, &arg_idx,
									NULL) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, void *));

			ᱡᱩᱫᱤ (ptr_arg != NULL)
			    arg_sign = 1;
		    }
		    ᱵᱟᱝ ᱡᱩᱫᱤ (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
				    tvs != NULL ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			ᱡᱩᱫᱤ (bin_arg != 0)
			    arg_sign = 1;
		    }
		    ᱵᱟᱝ ᱡᱩᱫᱤ (fmt_spec == 'd')
		    {
			// signed
			switch (length_modifier)
			{
			pak_sa '\0':
			pak_sa 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, puur_naa_nka));

			    ᱡᱩᱫᱤ (int_arg > 0)
				arg_sign =  1;
			    ᱵᱟᱝ ᱡᱩᱫᱤ (int_arg < 0)
				arg_sign = -1;
			    bha_nga;
			pak_sa 'l':
			    long_arg =
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long puur_naa_nka));

			    ᱡᱩᱫᱤ (long_arg > 0)
				arg_sign =  1;
			    ᱵᱟᱝ ᱡᱩᱫᱤ (long_arg < 0)
				arg_sign = -1;
			    bha_nga;
			pak_sa 'L':
			    llong_arg =
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
					tvs != NULL ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    ᱡᱩᱫᱤ (llong_arg > 0)
				arg_sign =  1;
			    ᱵᱟᱝ ᱡᱩᱫᱤ (llong_arg < 0)
				arg_sign = -1;
			    bha_nga;
			}
		    }
		    ᱵᱟᱝ
		    {
			// unsigned
			switch (length_modifier)
			{
			    pak_sa '\0':
			    pak_sa 'h':
				uint_arg =
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned puur_naa_nka));

				ᱡᱩᱫᱤ (uint_arg != 0)
				    arg_sign = 1;
				bha_nga;
			    pak_sa 'l':
				ulong_arg =
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
					    tvs != NULL ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long puur_naa_nka));

				ᱡᱩᱫᱤ (ulong_arg != 0)
				    arg_sign = 1;
				bha_nga;
			    pak_sa 'L':
				ullong_arg =
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
					    tvs != NULL ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				ᱡᱩᱫᱤ (ullong_arg != 0)
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
		    ᱡᱩᱫᱤ (precision_specified)
			zero_padding = 0;
		    ᱡᱩᱫᱤ (fmt_spec == 'd')
		    {
			ᱡᱩᱫᱤ (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    ᱵᱟᱝ ᱡᱩᱫᱤ (alternate_form)
		    {
			ᱡᱩᱫᱤ (arg_sign != 0
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
		    ᱡᱩᱫᱤ (!precision_specified)
			precision = 1;   // default precision is 1
		    ᱡᱩᱫᱤ (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    ᱵᱟᱝ
		    {
			var_na	f[6];
			puur_naa_nka	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			ᱡᱩᱫᱤ (!length_modifier)
			    ;
			ᱵᱟᱝ ᱡᱩᱫᱤ (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# ᱵᱟᱝ
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			ᱵᱟᱝ
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			ᱡᱩᱫᱤ (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			ᱵᱟᱝ ᱡᱩᱫᱤ (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    var_na	    b[8 * parimaa_na(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    do
			    {
				b[parimaa_na(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    ᱡᱚᱠᱷᱚᱱ (bn != 0);

			    memcpy(tmp + str_arg_l, b + parimaa_na(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			ᱵᱟᱝ ᱡᱩᱫᱤ (fmt_spec == 'd')
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
			ᱵᱟᱝ
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
			ᱡᱩᱫᱤ (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			ᱡᱩᱫᱤ (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t num_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			ᱡᱩᱫᱤ (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    ᱡᱩᱫᱤ (!precision_specified
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
			ᱡᱩᱫᱤ (num_of_digits < precision)
			    number_of_zeros_to_pad = precision - num_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    ᱡᱩᱫᱤ (!justify_left && zero_padding)
		    {
			puur_naa_nka n = (puur_naa_nka)(min_field_width - (str_arg_l
						    + number_of_zeros_to_pad));
			ᱡᱩᱫᱤ (n > 0)
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
# ᱡᱩᱫᱤ defined(FEAT_EVAL)
			tvs != NULL ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, dviguu_na));

		    abs_f = f < 0 ? -f : f;

		    ᱡᱩᱫᱤ (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			ᱡᱩᱫᱤ ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			ᱵᱟᱝ
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    ᱡᱩᱫᱤ ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# ᱵᱟᱝ
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
		    ᱵᱟᱝ
		    {
			ᱡᱩᱫᱤ (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			ᱵᱟᱝ ᱡᱩᱫᱤ (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			ᱵᱟᱝ
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    ᱡᱩᱫᱤ (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    ᱡᱩᱫᱤ (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				ᱡᱩᱫᱤ ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				ᱡᱩᱫᱤ (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (puur_naa_nka)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			ᱡᱩᱫᱤ (remove_trailing_zeroes)
			{
			    puur_naa_nka i;
			    var_na *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    ᱡᱩᱫᱤ (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    ᱵᱟᱝ
			    {
				tp = (var_na *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				ᱡᱩᱫᱤ (tp != NULL)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    ᱡᱩᱫᱤ (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    ᱡᱚᱠᱷᱚᱱ (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    ᱡᱩᱫᱤ (tp != NULL && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				ᱡᱚᱠᱷᱚᱱ (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			ᱵᱟᱝ
			{
			    var_na *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (var_na *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    ᱡᱩᱫᱤ (tp != NULL && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    ᱡᱩᱫᱤ (zero_padding && min_field_width > str_arg_l
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
		ᱡᱩᱫᱤ (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		bha_nga;
	    }

	    ᱡᱩᱫᱤ (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    ᱡᱩᱫᱤ (!justify_left)
	    {
		// left padding with blank or zero
		puur_naa_nka pn = (puur_naa_nka)(min_field_width - (str_arg_l + number_of_zeros_to_pad));

		ᱡᱩᱫᱤ (pn > 0)
		{
		    ᱡᱩᱫᱤ (str_l < str_m)
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
	    ᱡᱩᱫᱤ (number_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    ᱵᱟᱝ
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		puur_naa_nka zn = (puur_naa_nka)zero_padding_insertion_ind;

		ᱡᱩᱫᱤ (zn > 0)
		{
		    ᱡᱩᱫᱤ (str_l < str_m)
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
		ᱡᱩᱫᱤ (zn > 0)
		{
		    ᱡᱩᱫᱤ (str_l < str_m)
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

		ᱡᱩᱫᱤ (sn > 0)
		{
		    ᱡᱩᱫᱤ (str_l < str_m)
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
	    ᱡᱩᱫᱤ (justify_left)
	    {
		// right blank padding to the field width
		puur_naa_nka pn = (puur_naa_nka)(min_field_width
				      - (str_arg_l + number_of_zeros_to_pad));

		ᱡᱩᱫᱤ (pn > 0)
		{
		    ᱡᱩᱫᱤ (str_l < str_m)
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

    ᱡᱩᱫᱤ (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    ᱡᱩᱫᱤ (tvs != NULL && tvs[num_posarg != 0 ? num_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((var_na*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    ᱨᱩᱣᱟᱹᱲ (puur_naa_nka)str_l;
}

#endif // PROTO
