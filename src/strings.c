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

#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF
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
    ಆದರೆ (p != ಖಾಲಿ)
	mch_memmove(p, string, len);
    ಹಿಂತಿರುಗಿ p;
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
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    STRNCPY(p, string, len);
    p[len] = NUL;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    ಹಿಂತಿರುಗಿ vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    ಪೂರ್ಣಾಂಕ		cc,
    ಪೂರ್ಣಾಂಕ		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    ಚಿಹ್ನೆರಹಿತ	ಉದ್ದಳತೆ;
    ಪೂರ್ಣಾಂಕ		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    ಉದ್ದಳತೆ = 1;				// count the trailing NUL
    ಗಾಗಿ (p = string; *p; p++)
    {
	ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    ಉದ್ದಳತೆ += l;		// count a multibyte char
	    p += l - 1;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (vim_strchr(esc_chars, *p) != ಖಾಲಿ || (bsl && rem_backslash(p)))
	    ++ಉದ್ದಳತೆ;			// count a backslash
	++ಉದ್ದಳತೆ;			// count an ordinary char
    }
    escaped_string = alloc(ಉದ್ದಳತೆ);
    ಆದರೆ (escaped_string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    p2 = escaped_string;
    ಗಾಗಿ (p = string; *p; p++)
    {
	ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (vim_strchr(esc_chars, *p) != ಖಾಲಿ || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    ಹಿಂತಿರುಗಿ escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    ಪೂರ್ಣಾಂಕ
csh_like_shell(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)gettail(p_sh), "csh") != ಖಾಲಿ);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fish_like_shell(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)gettail(p_sh), "fish") != ಖಾಲಿ);
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
vim_strsave_shellescape(char_u *string, ಪೂರ್ಣಾಂಕ do_special, ಪೂರ್ಣಾಂಕ do_newline)
{
    ಚಿಹ್ನೆರಹಿತ	ಉದ್ದಳತೆ;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    ಪೂರ್ಣಾಂಕ		csh_like;
    ಪೂರ್ಣಾಂಕ		fish_like;
    char_u	*shname;
    ಪೂರ್ಣಾಂಕ		powershell;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಪೂರ್ಣಾಂಕ		double_quotes;
#ಆದರೆ_ಅಂತ್ಯ

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
    powershell = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)shname, "pwsh") != ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    powershell = powershell || ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)shname, "powershell") != ಖಾಲಿ;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#ಆದರೆ_ಅಂತ್ಯ

    // First count the number of extra bytes required.
    ಉದ್ದಳತೆ = (ಚಿಹ್ನೆರಹಿತ)STRLEN(string) + 3;  // two quotes and a trailing NUL
    ಗಾಗಿ (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಆದರೆ (double_quotes)
	{
	    ಆದರೆ (*p == '"')
		++ಉದ್ದಳತೆ;		// " -> ""
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (*p == '\'')
	{
	    ಆದರೆ (powershell)
		ಉದ್ದಳತೆ += 2;		// ' => ''
	    ಇಲ್ಲದಿದ್ದರೆ
		ಉದ್ದಳತೆ += 3;		// ' => '\''
	}
	ಆದರೆ ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++ಉದ್ದಳತೆ;			// insert backslash
	    ಆದರೆ (csh_like && do_special)
		++ಉದ್ದಳತೆ;		// insert backslash
	}
	ಆದರೆ (do_special && ಕಂಡುಹಿಡಿ_cmdline_var(p, &l) >= 0)
	{
	    ++ಉದ್ದಳತೆ;			// insert backslash
	    p += l - 1;
	}
	ಆದರೆ (*p == '\\' && fish_like)
	    ++ಉದ್ದಳತೆ;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(ಉದ್ದಳತೆ);
    ಆದರೆ (escaped_string != ಖಾಲಿ)
    {
	d = escaped_string;

	// add opening quote
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಆದರೆ (double_quotes)
	    *d++ = '"';
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    *d++ = '\'';

	ಗಾಗಿ (p = string; *p != NUL; )
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    ಆದರೆ (double_quotes)
	    {
		ಆದರೆ (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    ಮುಂದುವರಿ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (*p == '\'')
	    {
		ಆದರೆ (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		ಆದರೆ (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (do_special && ಕಂಡುಹಿಡಿ_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		ಸ್ಮರಣೆ_ನಕಲು(d, p, l);	// copy the var
		d += l;
		p += l;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		ಮುಂದುವರಿ;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಆದರೆ (double_quotes)
	    *d++ = '"';
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    *d++ = '\'';
	*d = NUL;
    }

    ಹಿಂತಿರುಗಿ escaped_string;
}

/*
 * Like vim_strsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strsave_ಮೇಲೆ(char_u *string)
{
    char_u *p1;

    p1 = vim_strsave(string);
    vim_strup(p1);
    ಹಿಂತಿರುಗಿ p1;
}

/*
 * Like vim_strnsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strnsave_ಮೇಲೆ(char_u *string, size_t len)
{
    char_u *p1;

    p1 = vim_strnsave(string, len);
    vim_strup(p1);
    ಹಿಂತಿರುಗಿ p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    ಶೂನ್ಯ
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    ಪೂರ್ಣಾಂಕ	    c;

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    p2 = p;
    ಆಗ ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_SPELL)
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

    ಆದರೆ (res != ಖಾಲಿ)
	ಆಗ (*p != NUL)
	{
	    ಪೂರ್ಣಾಂಕ		l;

	    ಆದರೆ (enc_utf8)
	    {
		ಪೂರ್ಣಾಂಕ	c, uc;
		ಪೂರ್ಣಾಂಕ	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ಆದರೆ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		ಆದರೆ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ಆದರೆ (s == ಖಾಲಿ)
		    {
			vim_free(res);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    ಹಿಂತಿರುಗಿ res;
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

    ಆದರೆ (res != ಖಾಲಿ)
	ಆಗ (*p != NUL)
	{
	    ಪೂರ್ಣಾಂಕ		l;

	    ಆದರೆ (enc_utf8)
	    {
		ಪೂರ್ಣಾಂಕ	c, lc;
		ಪೂರ್ಣಾಂಕ	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ಆದರೆ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		ಆದರೆ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ಆದರೆ (s == ಖಾಲಿ)
		    {
			vim_free(res);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    ಹಿಂತಿರುಗಿ res;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * delete spaces at the end of a string
 */
    ಶೂನ್ಯ
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    ಆಗ (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    ಶೂನ್ಯ
vim_strncpy(char_u *ಗಮ್ಯ, char_u *from, size_t len)
{
    STRNCPY(ಗಮ್ಯ, from, len);
    ಗಮ್ಯ[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    ಶೂನ್ಯ
vim_strcat(char_u *ಗಮ್ಯ, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(ಗಮ್ಯ);
    size_t fromlen = STRLEN(from);

    ಆದರೆ (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(ಗಮ್ಯ + tolen, from, tosize - tolen - 1);
	ಗಮ್ಯ[tosize - 1] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	mch_memmove(ಗಮ್ಯ + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(ಅಕ್ಷರ *s, size_t maxlen)
{
    size_t i;
    ಗಾಗಿ (i = 0; i < maxlen; ++i)
	ಆದರೆ (s[i] == NUL)
	    ನಿಲ್ಲಿಸು;
    ಹಿಂತಿರುಗಿ i;
}

#ಆದರೆ !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    ಪೂರ್ಣಾಂಕ
vim_stricmp(ಅಕ್ಷರ *s1, ಅಕ್ಷರ *s2)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (;;)
    {
	i = (ಪೂರ್ಣಾಂಕ)TOLOWER_LOC(*s1) - (ಪೂರ್ಣಾಂಕ)TOLOWER_LOC(*s2);
	ಆದರೆ (i != 0)
	    ಹಿಂತಿರುಗಿ i;			    // this character different
	ಆದರೆ (*s1 == NUL)
	    ನಿಲ್ಲಿಸು;			    // strings match until NUL
	++s1;
	++s2;
    }
    ಹಿಂತಿರುಗಿ 0;				    // strings match
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    ಪೂರ್ಣಾಂಕ
vim_strnicmp(ಅಕ್ಷರ *s1, ಅಕ್ಷರ *s2, size_t len)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆಗ (len > 0)
    {
	i = (ಪೂರ್ಣಾಂಕ)TOLOWER_LOC(*s1) - (ಪೂರ್ಣಾಂಕ)TOLOWER_LOC(*s2);
	ಆದರೆ (i != 0)
	    ಹಿಂತಿರುಗಿ i;			    // this character different
	ಆದರೆ (*s1 == NUL)
	    ನಿಲ್ಲಿಸು;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ಹಿಂತಿರುಗಿ 0;				    // strings match
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    ಪೂರ್ಣಾಂಕ
vim_strnicmp_asc(ಅಕ್ಷರ *s1, ಅಕ್ಷರ *s2, size_t len)
{
    ಪೂರ್ಣಾಂಕ                i = 0;

    ಆಗ (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	ಆದರೆ (i != 0)
	    ನಿಲ್ಲಿಸು;			// this character is different
	ಆದರೆ (*s1 == NUL)
	    ನಿಲ್ಲಿಸು;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ಹಿಂತಿರುಗಿ i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, ಪೂರ್ಣಾಂಕ c)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		b;

    p = string;
    ಆದರೆ (enc_utf8 && c > 0 && c < 0x80)
	ಹಿಂತಿರುಗಿ vim_strbyte(string, c);
    ಆದರೆ (enc_utf8 && c >= 0x80)
    {
	ಆಗ (*p != NUL)
	{
	    ಪೂರ್ಣಾಂಕ l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    ಆದರೆ (utf_ptr2char(p) == c && l > 1)
		ಹಿಂತಿರುಗಿ p;
	    p += l;
	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (enc_dbcs != 0 && c > 255)
    {
	ಪೂರ್ಣಾಂಕ	n2 = c & 0xff;

	c = ((ಚಿಹ್ನೆರಹಿತ)c >> 8) & 0xff;
	ಆಗ ((b = *p) != NUL)
	{
	    ಆದರೆ (b == c && p[1] == n2)
		ಹಿಂತಿರುಗಿ p;
	    p += (*mb_ptr2len)(p);
	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (has_mbyte)
    {
	ಆಗ ((b = *p) != NUL)
	{
	    ಆದರೆ (b == c)
		ಹಿಂತಿರುಗಿ p;
	    p += (*mb_ptr2len)(p);
	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆಗ ((b = *p) != NUL)
    {
	ಆದರೆ (b == c)
	    ಹಿಂತಿರುಗಿ p;
	++p;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, ಪೂರ್ಣಾಂಕ c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    ಆದರೆ (c <= 0 || c > 255)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (char_u *)strchr((ಅಕ್ಷರ *)string, c);
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
vim_strrchr(char_u *string, ಪೂರ್ಣಾಂಕ c)
{
    char_u	*retval = ಖಾಲಿ;
    char_u	*p = string;

    ಆಗ (*p)
    {
	ಆದರೆ (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Vim's version of strpbrk(), in case it's missing.
 * Don't generate a prototype for this, causes problems when it's not used.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO
# ifndef HAVE_STRPBRK
#  ifdef vim_strpbrk
#   undef vim_strpbrk
#  endif
    char_u *
vim_strpbrk(char_u *s, char_u *charset)
{
    ಆಗ (*s)
    {
	ಆದರೆ (vim_strchr(charset, *s) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ s;
	MB_PTR_ADV(s);
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Sort an array of strings.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ವಿಂಗಡಿಸು_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2);

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ವಿಂಗಡಿಸು_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ಹಿಂತಿರುಗಿ STRCMP(*(ಅಕ್ಷರ **)s1, *(ಅಕ್ಷರ **)s2);
}

    ಶೂನ್ಯ
ವಿಂಗಡಿಸು_ಅಕ್ಷರಸರಣಿಗಳು(
    char_u	**files,
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2)
{
    qsort((ಶೂನ್ಯ *)files, (size_t)ಎಣಿಕೆ_2, ಗಾತ್ರ(char_u *), ವಿಂಗಡಿಸು_compare);
}

#ಆದರೆ defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    ಪೂರ್ಣಾಂಕ
has_non_ascii(char_u *s)
{
    char_u	*p;

    ಆದರೆ (s != ಖಾಲಿ)
	ಗಾಗಿ (p = s; *p != NUL; ++p)
	    ಆದರೆ (*p >= 128)
		ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Concatenate two strings and return the result in allocated memory.
 * Returns NULL when out of memory.
 */
    char_u  *
concat_str(char_u *str1, char_u *str2)
{
    char_u  *dest;
    size_t  l = str1 == ಖಾಲಿ ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == ಖಾಲಿ ? 0 : STRLEN(str2)) + 1L);
    ಆದರೆ (dest == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (str1 == ಖಾಲಿ)
	*dest = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
	STRCPY(dest, str1);
    ಆದರೆ (str2 != ಖಾಲಿ)
	STRCPY(dest + l, str2);
    ಹಿಂತಿರುಗಿ dest;
}

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
ಹಿಮ್ಮುಖ_ಪಠ್ಯ(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    ಆದರೆ (rev == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	ಆದರೆ (has_mbyte)
	{
	    ಪೂರ್ಣಾಂಕ mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    ಹಿಂತಿರುಗಿ rev;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, ಪೂರ್ಣಾಂಕ function)
{
    ಚಿಹ್ನೆರಹಿತ	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    ಆದರೆ (str != ಖಾಲಿ)
    {
	len += (ಚಿಹ್ನೆರಹಿತ)STRLEN(str);
	ಗಾಗಿ (p = str; *p != NUL; MB_PTR_ADV(p))
	    ಆದರೆ (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    ಆದರೆ (r == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	*r++ = '\'';
    ಆದರೆ (str != ಖಾಲಿ)
	ಗಾಗಿ (p = str; *p != NUL; )
	{
	    ಆದರೆ (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    ಆದರೆ (function)
	*r++ = ')';
    *r++ = NUL;
    ಹಿಂತಿರುಗಿ s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    ದೀರ್ಘ
string_ಎಣಿಕೆ(char_u *haystack, char_u *needle, ಪೂರ್ಣಾಂಕ ic)
{
    ದೀರ್ಘ	n = 0;
    char_u	*p = haystack;
    char_u	*ಮುಂದಿನ;

    ಆದರೆ (p == ಖಾಲಿ || needle == ಖಾಲಿ || *needle == NUL)
	ಹಿಂತಿರುಗಿ 0;

    size_t  needlelen = STRLEN(needle);
    ಆದರೆ (ic)
    {
	ಆಗ (*p != NUL)
	{
	    ಆದರೆ (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		MB_PTR_ADV(p);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಆಗ ((ಮುಂದಿನ = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)p, (ಅಕ್ಷರ *)needle)) != ಖಾಲಿ)
	{
	    ++n;
	    p = ಮುಂದಿನ + needlelen;
	}

    ಹಿಂತಿರುಗಿ n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
copy_ಮೊದಲ_char_ಗಮ್ಯ_tv(char_u *ಒಳಹರಿವು, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (ಒಳಹರಿವು == ಖಾಲಿ || output == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    len = has_mbyte ? mb_ptr2len(ಒಳಹರಿವು) : 1;
    STRNCPY(buf, ಒಳಹರಿವು, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    ಹಿಂತಿರುಗಿ output->vval.v_string == ಖಾಲಿ ? -1 : len;
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
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		idx = 0;
    ಪೂರ್ಣಾಂಕ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    // set_vim_var_nr() doesn't set the type
    ಗಣ_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, &newtv);

    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 80);
    ಗಾಗಿ (p = str; *p != NUL; p += len)
    {
	len = copy_ಮೊದಲ_char_ಗಮ್ಯ_tv(p, &tv);
	ಆದರೆ (len < 0)
	    ನಿಲ್ಲಿಸು;

	ಗಣ_vim_var_nr(VV_ಕೀಲಿ, idx);
	ಆದರೆ (filter_map_one(&tv, ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    ಆದರೆ (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ga_concat(&ga, newtv.vval.v_string);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_ದತ್ತಾಂಶ;
    ಆದರೆ (fc != ಖಾಲಿ)
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
    ಪೂರ್ಣಾಂಕ		len;
    typval_T	argv[3];
    ಪೂರ್ಣಾಂಕ		r;
    ಪೂರ್ಣಾಂಕ		called_emsg_ಆರಂಭ = called_emsg;
    funccall_T	*fc;

    ಆದರೆ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ಆದರೆ (*p == NUL)
	{
	    semsg(_(e_reduce_ದ_an_ಖಾಲಿಗಳು_str_with_no_initial_ಮೌಲ್ಯ), "String");
	    ಹಿಂತಿರುಗಿ;
	}
	len = copy_ಮೊದಲ_char_ಗಮ್ಯ_tv(p, rettv);
	ಆದರೆ (len < 0)
	    ಹಿಂತಿರುಗಿ;
	p += len;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_for_string_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, rettv);

    ಗಾಗಿ ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_ಮೊದಲ_char_ಗಮ್ಯ_tv(p, &argv[1]);
	ಆದರೆ (len < 0)
	{
	    *rettv = argv[0];
	    ನಿಲ್ಲಿಸು;
	}

	r = eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	ಆದರೆ (r == FAIL || called_emsg != called_emsg_ಆರಂಭ)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (fc != ಖಾಲಿ)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
byteidx_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ comp)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ);
    ಆದರೆ (str == ಖಾಲಿ || idx < 0)
	ಹಿಂತಿರುಗಿ;

    varnumber_T	utf16idx = FALSE;
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಂಕ error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_ಸಂಖ್ಯೆ_as_bool_nr), utf16idx);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಪೂರ್ಣಾಂಕ (*ptr2len)(char_u *);
    ಆದರೆ (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    ಇಲ್ಲದಿದ್ದರೆ
	ptr2len = mb_ptr2len;

    char_u *t = str;
    ಗಾಗಿ ( ; idx > 0; idx--)
    {
	ಆದರೆ (*t == NUL)		// EOL reached
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (utf16idx)
	{
	    ಪೂರ್ಣಾಂಕ clen = ptr2len(t);
	    ಪೂರ್ಣಾಂಕ c = (clen > 1) ? utf_ptr2char(t) : *t;
	    ಆದರೆ (c > 0xFFFF)
		idx--;
	    ಆದರೆ (idx > 0)
		t += clen;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx > 0)
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

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	ಹಿಂತಿರುಗಿ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ);
    ಆದರೆ (str == ಖಾಲಿ || idx < 0)
	ಹಿಂತಿರುಗಿ;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    ಪೂರ್ಣಾಂಕ (*ptr2len)(char_u *);
    ಆದರೆ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    ಇಲ್ಲದಿದ್ದರೆ
	ptr2len = mb_ptr2len;

    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;
    ಗಾಗಿ (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	ಆದರೆ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    ಆದರೆ (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_ಸಂಖ್ಯೆ = len;
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (utf16idx)
	{
	    idx--;
	    ಪೂರ್ಣಾಂಕ clen = ptr2len(p);
	    ಪೂರ್ಣಾಂಕ c = (clen > 1) ? utf_ptr2char(p) : *p;
	    ಆದರೆ (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
convert_string(string_T *str, char_u *from, char_u *ಗಮ್ಯ, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ಆದರೆ (convert_ಸಿದ್ಧತೆ(&vimconv, from, ಗಮ್ಯ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    vimconv.vc_fail = TRUE;
    ಆದರೆ (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->ಉದ್ದಳತೆ);
	ಆದರೆ (ret->string == ಖಾಲಿ)
	    ret->ಉದ್ದಳತೆ = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    ret->ಉದ್ದಳತೆ = str->ಉದ್ದಳತೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)str->ಉದ್ದಳತೆ;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->ಉದ್ದಳತೆ = len;
    }
    convert_ಸಿದ್ಧತೆ(&vimconv, ಖಾಲಿ, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ (ret->string == ಖಾಲಿ) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
blob_from_string(char_u *str, blob_T *blob)
{
    ಪೂರ್ಣಾಂಕ	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(str);
    char_u  *dest;

    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ga_grow(&blob->bv_ga, len) == FAIL)
	ಹಿಂತಿರುಗಿ;
    dest = (char_u *)blob->bv_ga.ga_ದತ್ತಾಂಶ + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len; ++i)
	ಆದರೆ (dest[i] == NL)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
string_from_blob(blob_T *blob, ದೀರ್ಘ *ಆರಂಭ_idx, string_T *ret)
{
    ದೀರ್ಘ	blen = blob_len(blob);
    ದೀರ್ಘ	ಆರಂಭ = *ಆರಂಭ_idx;
    char_u	*src;
    char_u	*nl;
    ದೀರ್ಘ	ಸಾಲು_len;

    ಆದರೆ (ಆರಂಭ >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->ಉದ್ದಳತೆ = 0;
	*ಆರಂಭ_idx = blen;
	ಹಿಂತಿರುಗಿ (ret->string == ಖಾಲಿ) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_ದತ್ತಾಂಶ + ಆರಂಭ;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - ಆರಂಭ));
    ಸಾಲು_len = (nl == ಖಾಲಿ) ? (blen - ಆರಂಭ) : (ದೀರ್ಘ)(nl - src);

    ret->string = alloc(ಸಾಲು_len + 1);
    ಆದರೆ (ret->string == ಖಾಲಿ)
    {
	ret->ಉದ್ದಳತೆ = 0;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (ಸಾಲು_len > 0)
	mch_memmove(ret->string, src, (size_t)ಸಾಲು_len);
    ret->string[ಸಾಲು_len] = NUL;
    ret->ಉದ್ದಳತೆ = (size_t)ಸಾಲು_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    ಗಾಗಿ (ದೀರ್ಘ i = 0; i < ಸಾಲು_len; i++)
	ಆದರೆ (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *ಆರಂಭ_idx = ಆರಂಭ + ಸಾಲು_len + (nl != ಖಾಲಿ ? 1 : 0);
    ಹಿಂತಿರುಗಿ OK;
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
    ಆದರೆ (from_encoding_raw == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    ಆಗ (*s != NUL)
    {
	ಆದರೆ (*s == '_')
	    *pe++ = '-';
	ಇಲ್ಲದಿದ್ದರೆ
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    ಆದರೆ ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    ಹಿಂತಿರುಗಿ from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
append_converted_string_ಗಮ್ಯ_list(
	string_T *converted,
	ಪೂರ್ಣಾಂಕ validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    ಆದರೆ (converted->string != ಖಾಲಿ)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *ಕೊನೆ = converted->string + converted->ಉದ್ದಳತೆ;
	ಆಗ (p < ಕೊನೆ)
	{
	    string_T	ಸಾಲು;
	    char_u	*ಸಾಲು_ಆರಂಭ = p;

	    ಆಗ (p < ಕೊನೆ && *p != NL)
		p++;

	    // Add this line to the result list
	    ಸಾಲು.ಉದ್ದಳತೆ = (size_t)(p - ಸಾಲು_ಆರಂಭ);
	    ಸಾಲು.string = vim_strnsave(ಸಾಲು_ಆರಂಭ, ಸಾಲು.ಉದ್ದಳತೆ);
	    ಆದರೆ (ಸಾಲು.string != ಖಾಲಿ)
	    {
		ಆದರೆ (validate_utf8 && !utf_valid_string(ಸಾಲು.string, ಖಾಲಿ))
		{
		    vim_free(ಸಾಲು.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    ಹಿಂತಿರುಗಿ; // Stop processing
		}
		ಆದರೆ (list_append_string(list, ಸಾಲು.string, (ಪೂರ್ಣಾಂಕ)ಸಾಲು.ಉದ್ದಳತೆ) == FAIL)
		{
		    vim_free(ಸಾಲು.string);
		    vim_free(converted->string);
		    ಹಿಂತಿರುಗಿ; // Stop processing on append failure
		}
		vim_free(ಸಾಲು.string);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ));
		vim_free(converted->string);
		ಹಿಂತಿರುಗಿ;
	    }

	    ಆದರೆ (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
append_validated_ಸಾಲು_ಗಮ್ಯ_list(string_T *ಸಾಲು, ಪೂರ್ಣಾಂಕ validate_utf8, list_T *list)
{
    ಆದರೆ (validate_utf8 && !utf_valid_string(ಸಾಲು->string, ಖಾಲಿ))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(ಸಾಲು->string);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಪೂರ್ಣಾಂಕ ret = list_append_string(list, ಸಾಲು->string, (ಪೂರ್ಣಾಂಕ)ಸಾಲು->ಉದ್ದಳತೆ);
    vim_free(ಸಾಲು->string);
    ಹಿಂತಿರುಗಿ ret;
}

    ಶೂನ್ಯ
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    ಪೂರ್ಣಾಂಕ		blen;
    ದೀರ್ಘ	idx;
    ಪೂರ್ಣಾಂಕ		validate_utf8 = FALSE;

    ಆದರೆ (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    blob = argvars->vval.v_blob;
    ಆದರೆ (blob == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    blen = blob_len(blob);

    char_u	*from_encoding = ಖಾಲಿ;
    char_u	*from_encoding_raw = ಖಾಲಿ;  // Encoding name with endianness preserved for iconv
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ಆದರೆ (d != ಖಾಲಿ)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ಆದರೆ (enc != ಖಾಲಿ)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		ಆದರೆ (from_encoding_raw == ಖಾಲಿ)
		{
		    emsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ));
		    VIM_CLEAR(from_encoding);
		    ಹಿಂತಿರುಗಿ;
		}
	    }
	}
    }

    ಆದರೆ (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    ಆದರೆ (from_encoding != ಖಾಲಿ && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    ಪೂರ್ಣಾಂಕ from_prop = 0;
    ಆದರೆ (from_encoding != ಖಾಲಿ)
	from_prop = enc_canon_props(from_encoding);
    ಆದರೆ (from_encoding != ಖಾಲಿ && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2_ಶಬ್ದ)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	ಪೂರ್ಣಾಂಕ nul_ಗಾತ್ರ = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_ಗಾತ್ರ);
	ಆದರೆ (ga_grow(&blob_ga, blen + nul_ಗಾತ್ರ) == OK)
	{
	    ಆದರೆ (blen > 0)
		mch_memmove(blob_ga.ga_ದತ್ತಾಂಶ, blob->bv_ga.ga_ದತ್ತಾಂಶ, (size_t)blen);
	    // NUL terminator (2 bytes for UTF-16/UCS-2, 4 bytes for UTF-32/UCS-4)
	    vim_memset((char_u *)blob_ga.ga_ದತ್ತಾಂಶ + blen, 0, (size_t)nul_ಗಾತ್ರ);
	    blob_ga.ga_len = blen + nul_ಗಾತ್ರ;
	}

	// Convert the entire blob at once
	vimconv_T vimconv;
	vimconv.vc_type = CONV_NONE;
	// Use raw encoding name for iconv to preserve endianness (utf-16be vs utf-16)
	// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
	ಆದರೆ (convert_ಸಿದ್ಧತೆ_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    ಹೋಗು ಮುಗಿದ;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	ಪೂರ್ಣಾಂಕ len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_ದತ್ತಾಂಶ, &len, ಖಾಲಿ);
	converted.ಉದ್ದಳತೆ = len;
	convert_ಸಿದ್ಧತೆ(&vimconv, ಖಾಲಿ, ಖಾಲಿ);
	ga_clear(&blob_ga);
	append_converted_string_ಗಮ್ಯ_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	ಆಗ (idx < blen)
	{
	    string_T	str;

	    ಆದರೆ (string_from_blob(blob, &idx, &str) != OK)
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (from_encoding != ಖಾಲಿ)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		ಪೂರ್ಣಾಂಕ	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		ಆದರೆ (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    ಹೋಗು ಮುಗಿದ;
		}
		str.string = converted.string;
		str.ಉದ್ದಳತೆ = converted.ಉದ್ದಳತೆ;
	    }

	    ಆದರೆ (append_validated_ಸಾಲು_ಗಮ್ಯ_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		ಹೋಗು ಮುಗಿದ;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    ಆದರೆ (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

ಮುಗಿದ:
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

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (rettv_blob_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    char_u	*ಗಮ್ಯ_encoding = ಖಾಲಿ;
    char_u	*ಗಮ್ಯ_encoding_raw = ಖಾಲಿ;  // Encoding name with endianness preserved for iconv
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ಆದರೆ (d != ಖಾಲಿ)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ಆದರೆ (enc != ಖಾಲಿ)
	    {
		char_u *enc_skipped = enc_skip(enc);
		ಗಮ್ಯ_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		ಗಮ್ಯ_encoding_raw = normalize_encoding_name(enc_skipped);
		ಆದರೆ (ಗಮ್ಯ_encoding_raw == ಖಾಲಿ)
		{
		    emsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ));
		    VIM_CLEAR(ಗಮ್ಯ_encoding);
		    ಹಿಂತಿರುಗಿ;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    ಪೂರ್ಣಾಂಕ ಗಮ್ಯ_prop = 0;
    ಆದರೆ (ಗಮ್ಯ_encoding != ಖಾಲಿ)
	ಗಮ್ಯ_prop = enc_canon_props(ಗಮ್ಯ_encoding);
    ಆದರೆ (ಗಮ್ಯ_encoding != ಖಾಲಿ && (ಗಮ್ಯ_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2_ಶಬ್ದ)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
	{
	    char_u *s;

	    ಆದರೆ (li->li_tv.v_type != VAR_STRING)
		ಮುಂದುವರಿ;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    ಆದರೆ (li != list->lv_ಮೊದಲ)
		ga_append(&str_ga, NL);
	    ಆದರೆ (s != ಖಾಲಿ && *s != NUL)
	    {
		ಪೂರ್ಣಾಂಕ slen = (ಪೂರ್ಣಾಂಕ)STRLEN(s);

		ಆದರೆ (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    ಹೋಗು ಮುಗಿದ;
		}
		mch_memmove((char_u *)str_ga.ga_ದತ್ತಾಂಶ + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	ಆದರೆ (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    ಆದರೆ (convert_ಸಿದ್ಧತೆ_ext(&vimconv, p_enc, FALSE, ಗಮ್ಯ_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_ಗಮ್ಯ_failed), ಗಮ್ಯ_encoding);
		ಹೋಗು ಮುಗಿದ;
	    }
	    vimconv.vc_fail = TRUE;

	    ಪೂರ್ಣಾಂಕ		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_ದತ್ತಾಂಶ, &len, ಖಾಲಿ);
	    convert_ಸಿದ್ಧತೆ(&vimconv, ಖಾಲಿ, ಖಾಲಿ);
	    ga_clear(&str_ga);

	    ಆದರೆ (converted == ಖಾಲಿ)
	    {
		semsg(_(e_str_encoding_ಗಮ್ಯ_failed), ಗಮ್ಯ_encoding);
		ಹೋಗು ಮುಗಿದ;
	    }
	    ಆದರೆ (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_ದತ್ತಾಂಶ + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ga_clear(&str_ga);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
	{
	    ಆದರೆ (li->li_tv.v_type != VAR_STRING)
		ಮುಂದುವರಿ;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    ಆದರೆ (str.string == ಖಾಲಿ)
		STR_LITERAL_ಗಣ(str, "");
	    ಇಲ್ಲದಿದ್ದರೆ
		str.ಉದ್ದಳತೆ = STRLEN(str.string);

	    ಆದರೆ (ಗಮ್ಯ_encoding != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, ಗಮ್ಯ_encoding, &converted);
		ಆದರೆ (res != OK)
		{
		    semsg(_(e_str_encoding_ಗಮ್ಯ_failed), ಗಮ್ಯ_encoding);
		    ಹೋಗು ಮುಗಿದ;
		}
		str.string = converted.string;
		str.ಉದ್ದಳತೆ = converted.ಉದ್ದಳತೆ;
	    }

	    ಆದರೆ (li != list->lv_ಮೊದಲ)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    ಆದರೆ (ಗಮ್ಯ_encoding != ಖಾಲಿ)
		vim_free(str.string);
	}
    }

ಮುಗಿದ:
    ಆದರೆ (ಗಮ್ಯ_encoding != ಖಾಲಿ)
	vim_free(ಗಮ್ಯ_encoding);
    ಆದರೆ (ಗಮ್ಯ_encoding_raw != ಖಾಲಿ)
	vim_free(ಗಮ್ಯ_encoding_raw);
}

/*
 * "str2list()" function
 */
    ಶೂನ್ಯ
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		utf8 = FALSE;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[1], ಖಾಲಿ);

    p = tv_get_string(&argvars[0]);

    ಆದರೆ (has_mbyte || utf8)
    {
	ಪೂರ್ಣಾಂಕ (*ptr2len)(char_u *);
	ಪೂರ್ಣಾಂಕ (*ptr2char)(char_u *);

	ಆದರೆ (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	ಗಾಗಿ ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (*ptr2char)(p));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ ( ; *p != NUL; ++p)
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    ಶೂನ್ಯ
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		ತಳ = 10;
    char_u	*p;
    varnumber_T	n;
    ಪೂರ್ಣಾಂಕ		what = 0;
    ಪೂರ್ಣಾಂಕ		isneg;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ತಳ = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	ಆದರೆ (ತಳ != 2 && ತಳ != 8 && ತಳ != 10 && ತಳ != 16)
	{
	    emsg(_(e_invalid_ವಾದ));
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    ಆದರೆ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    ಆಯ್ಕೆ (ತಳ)
    {
	ಸ್ಥಿತಿ 2: what |= STR2NR_BIN + STR2NR_FORCE; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 16: what |= STR2NR_HEX + STR2NR_FORCE; ನಿಲ್ಲಿಸು;
    }
    vim_str2nr(p, ಖಾಲಿ, ಖಾಲಿ, what, &n, ಖಾಲಿ, 0, FALSE, ಖಾಲಿ);
    // Text after the number is silently ignored.
    ಆದರೆ (isneg)
	rettv->vval.v_ಸಂಖ್ಯೆ = -n;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = n;

}

/*
 * "strgetchar()" function
 */
    ಶೂನ್ಯ
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    ಪೂರ್ಣಾಂಕ		charidx;
    ಪೂರ್ಣಾಂಕ		byteidx = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(str);
    charidx = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;

    ಆಗ (charidx >= 0 && byteidx < len)
    {
	ಆದರೆ (charidx == 0)
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = mb_ptr2char(str + byteidx);
	    ನಿಲ್ಲಿಸು;
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
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_idx;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (needle == ಖಾಲಿ || haystack == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;		// type error; errmsg already given

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಂಕ	    error = FALSE;

	ಆರಂಭ_idx = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	ಆದರೆ (error || ಆರಂಭ_idx >= (ಪೂರ್ಣಾಂಕ)STRLEN(haystack))
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (ಆರಂಭ_idx >= 0)
	    haystack += ಆರಂಭ_idx;
    }

    pos	= (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)haystack, (ಅಕ್ಷರ *)needle);
    ಆದರೆ (pos != ಖಾಲಿ)
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
    ಆದರೆ (rettv->vval.v_string != ಖಾಲಿ && tofree == ಖಾಲಿ)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    ಶೂನ್ಯ
f_strlen(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
strchar_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    ಪೂರ್ಣಾಂಕ			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    ಆಗ (*s != NUL)
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
    ಆದರೆ (ಒಳ_vim9script()
	    && check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    ಶೂನ್ಯ
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಂಕ error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_ಸಂಖ್ಯೆ_as_bool_nr), skipcc);
	    ಹಿಂತಿರುಗಿ;
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

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    varnumber_T countcc = FALSE;
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    ಪೂರ್ಣಾಂಕ			(*func_mb_ptr2char_adv)(char_u **pp);
    ಪೂರ್ಣಾಂಕ			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    ಆಗ (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	ಆದರೆ (ch > 0xFFFF)
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
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    s = tv_get_string(&argvars[0]);
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);

    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)(linetabsize_ಲಂಬಸಾಲು(ಲಂಬಸಾಲು, s) - ಲಂಬಸಾಲು);
}

/*
 * "strwidth()" function
 */
    ಶೂನ್ಯ
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

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
    ಪೂರ್ಣಾಂಕ		nchar;
    ಪೂರ್ಣಾಂಕ		nbyte = 0;
    ಪೂರ್ಣಾಂಕ		charlen;
    ಪೂರ್ಣಾಂಕ		skipcc = FALSE;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		slen;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    p = tv_get_string(&argvars[0]);
    slen = (ಪೂರ್ಣಾಂಕ)STRLEN(p);

    nchar = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಆದರೆ (!error)
    {
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ;
	    ಆದರೆ (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_ಸಂಖ್ಯೆ_as_bool_nr), skipcc);
		ಹಿಂತಿರುಗಿ;
	    }
	}

	ಆದರೆ (nchar > 0)
	    ಆಗ (nchar > 0 && nbyte < slen)
	    {
		ಆದರೆ (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		ಇಲ್ಲದಿದ್ದರೆ
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	ಇಲ್ಲದಿದ್ದರೆ
	    nbyte = nchar;
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
	    ಆಗ (charlen > 0 && nbyte + len < slen)
	    {
		ಪೂರ್ಣಾಂಕ off = nbyte + len;

		ಆದರೆ (off < 0)
		    len += 1;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (skipcc)
			len += mb_ptr2len(p + off);
		    ಇಲ್ಲದಿದ್ದರೆ
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    ಆದರೆ (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nbyte > slen)
	nbyte = slen;
    ಆದರೆ (len < 0)
	len = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nbyte + len > slen)
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
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		slen;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    p = tv_get_string(&argvars[0]);
    slen = (ಪೂರ್ಣಾಂಕ)STRLEN(p);

    n = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಆದರೆ (error)
	len = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	len = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
    ಇಲ್ಲದಿದ್ದರೆ
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    ಆದರೆ (n < 0)
    {
	len += n;
	n = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n > slen)
	n = slen;
    ಆದರೆ (len < 0)
	len = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n + len > slen)
	len = slen - n;

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಂಕ off;

	// length in characters
	ಗಾಗಿ (off = n; off < slen && len > 0; --len)
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
    char_u	*lastmatch = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		haystack_len, ಕೊನೆ_idx;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (needle == ಖಾಲಿ || haystack == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;		// type error; errmsg already given

    haystack_len = (ಪೂರ್ಣಾಂಕ)STRLEN(haystack);
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	ಕೊನೆ_idx = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], ಖಾಲಿ);
	ಆದರೆ (ಕೊನೆ_idx < 0)
	    ಹಿಂತಿರುಗಿ;	// can never find a match
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೊನೆ_idx = haystack_len;

    ಆದರೆ (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + ಕೊನೆ_idx;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)rest, (ಅಕ್ಷರ *)needle);
	    ಆದರೆ (rest == ಖಾಲಿ || rest > haystack + ಕೊನೆ_idx)
		ನಿಲ್ಲಿಸು;
	    lastmatch = rest;
	}
    }

    ಆದರೆ (lastmatch == ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    ಶೂನ್ಯ
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

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

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    ಹಿಂತಿರುಗಿ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ);
    ಆದರೆ (str == ಖಾಲಿ || idx < 0)
	ಹಿಂತಿರುಗಿ;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    ಪೂರ್ಣಾಂಕ (*ptr2len)(char_u *);
    ಆದರೆ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    ಇಲ್ಲದಿದ್ದರೆ
	ptr2len = mb_ptr2len;

    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		utf16idx = 0;
    ಗಾಗಿ (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	ಆದರೆ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    ಆದರೆ (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_ಸಂಖ್ಯೆ = len;
	    ಹಿಂತಿರುಗಿ;
	}
	utf16idx = len;
	ಪೂರ್ಣಾಂಕ clen = ptr2len(p);
	ಪೂರ್ಣಾಂಕ c = (clen > 1) ? utf_ptr2char(p) : *p;
	ಆದರೆ (c > 0xFFFF)
	    len++;
	p += clen;
	ಆದರೆ (charidx)
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
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    ಶೂನ್ಯ
f_toupper(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    ಶೂನ್ಯ
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*ಒಳ_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		inlen;
    ಪೂರ್ಣಾಂಕ		fromlen;
    ಪೂರ್ಣಾಂಕ		tolen;
    ಪೂರ್ಣಾಂಕ		idx;
    char_u	*cpstr;
    ಪೂರ್ಣಾಂಕ		cplen;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಒಳ_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;
    ಆದರೆ (fromstr == ಖಾಲಿ || tostr == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;		// type error; errmsg already given
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 80);

    ಆದರೆ (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	ಆದರೆ (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_ವಾದ_str), fromstr);
	    ga_clear(&ga);
	    ಹಿಂತಿರುಗಿ;
	}

    // fromstr and tostr have to contain the same number of chars
    ಆಗ (*ಒಳ_str != NUL)
    {
	ಆದರೆ (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(ಒಳ_str);
	    cpstr = ಒಳ_str;
	    cplen = inlen;
	    idx = 0;
	    ಗಾಗಿ (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		ಆದರೆ (fromlen == inlen && STRNCMP(ಒಳ_str, p, inlen) == 0)
		{
		    ಗಾಗಿ (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			ಆದರೆ (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    ಆದರೆ (*p == NUL)	// tostr is shorter than fromstr
			ಹೋಗು error;
		    ನಿಲ್ಲಿಸು;
		}
		++idx;
	    }

	    ಆದರೆ (ಮೊದಲ && cpstr == ಒಳ_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		ಮೊದಲ = FALSE;
		ಗಾಗಿ (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		ಆದರೆ (idx != 0)
		    ಹೋಗು error;
	    }

	    (ಶೂನ್ಯ)ga_grow(&ga, cplen);
	    mch_memmove((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    ಒಳ_str += inlen;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *ಒಳ_str);
	    ಆದರೆ (p != ಖಾಲಿ)
		ga_append(&ga, tostr[p - fromstr]);
	    ಇಲ್ಲದಿದ್ದರೆ
		ga_append(&ga, *ಒಳ_str);
	    ++ಒಳ_str;
	}
    }

    // add a terminating NUL
    (ಶೂನ್ಯ)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_ದತ್ತಾಂಶ;
}

/*
 * "trim({expr})" function
 */
    ಶೂನ್ಯ
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*ತಲೆ;
    char_u	*ಮುಖವಾಡ = ಖಾಲಿ;
    char_u	*ಬಾಲ;
    char_u	*prev;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		c1;
    ಪೂರ್ಣಾಂಕ		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ತಲೆ = tv_get_string_buf_chk(&argvars[0], buf1);
    ಆದರೆ (ತಲೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_opt_string_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type == VAR_STRING)
    {
	ಮುಖವಾಡ = tv_get_string_buf_chk(&argvars[1], buf2);
	ಆದರೆ (*ಮುಖವಾಡ == NUL)
	    ಮುಖವಾಡ = ಖಾಲಿ;

	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ಪೂರ್ಣಾಂಕ	error = 0;

	    // leading or trailing characters to trim
	    dir = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ;
	    ಆದರೆ (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[2]));
		ಹಿಂತಿರುಗಿ;
	    }
	}
    }

    ಆದರೆ (dir == 0 || dir == 1)
    {
	// Trim leading characters
	ಆಗ (*ತಲೆ != NUL)
	{
	    c1 = PTR2CHAR(ತಲೆ);
	    ಆದರೆ (ಮುಖವಾಡ == ಖಾಲಿ)
	    {
		ಆದರೆ (c1 > ' ' && c1 != 0xa0)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಾಗಿ (p = ಮುಖವಾಡ; *p != NUL; MB_PTR_ADV(p))
		    ಆದರೆ (c1 == PTR2CHAR(p))
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (*p == NUL)
		    ನಿಲ್ಲಿಸು;
	    }
	    MB_PTR_ADV(ತಲೆ);
	}
    }

    ಬಾಲ = ತಲೆ + STRLEN(ತಲೆ);
    ಆದರೆ (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	ಗಾಗಿ (; ಬಾಲ > ತಲೆ; ಬಾಲ = prev)
	{
	    prev = ಬಾಲ;
	    MB_PTR_ಹಿಂಭಾಗ(ತಲೆ, prev);
	    c1 = PTR2CHAR(prev);
	    ಆದರೆ (ಮುಖವಾಡ == ಖಾಲಿ)
	    {
		ಆದರೆ (c1 > ' ' && c1 != 0xa0)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಾಗಿ (p = ಮುಖವಾಡ; *p != NUL; MB_PTR_ADV(p))
		    ಆದರೆ (c1 == PTR2CHAR(p))
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (*p == NUL)
		    ನಿಲ್ಲಿಸು;
	    }
	}
    }
    rettv->vval.v_string = vim_strnsave(ತಲೆ, ಬಾಲ - ತಲೆ);
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
    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    ಆದರೆ (!decoded)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    char_u	*p = decoded;
    size_t	i = 0;

    ಆಗ (i < len)
    {
	ಆದರೆ (str[i] == '%')
	{
	    ಆದರೆ (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		ಆದರೆ (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ val = hexhex2nr(&str[i + 1]);
		ಆದರೆ (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // invalid hex digits following "%"
		    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *p++ = str[i++];
    }

    *p = NUL;

    ಹಿಂತಿರುಗಿ decoded;
}

/*
 * "uri_decode({str})" function
 */
    ಶೂನ್ಯ
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

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
    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    ಆದರೆ (encoded == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    char_u *p = encoded;

    ಗಾಗಿ (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	ಆದರೆ (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    sprintf((ಅಕ್ಷರ *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    ಹಿಂತಿರುಗಿ encoded;
}

/*
 * "uri_encode({str})" function
 */
    ಶೂನ್ಯ
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

ಸ್ಥಿರ ಅಕ್ಷರ *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ಸ್ಥಿರ varnumber_T
tv_nr(typval_T *tvs, ಪೂರ್ಣಾಂಕ *idxp)
{
    ಪೂರ್ಣಾಂಕ		idx = *idxp - 1;
    varnumber_T	n = 0;
    ಪೂರ್ಣಾಂಕ		err = FALSE;

    ಆದರೆ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	++*idxp;
	n = tv_get_ಸಂಖ್ಯೆ_chk(&tvs[idx], &err);
	ಆದರೆ (err)
	    n = 0;
    }
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
tv_str(typval_T *tvs, ಪೂರ್ಣಾಂಕ *idxp, char_u **tofree)
{
    ಪೂರ್ಣಾಂಕ		    idx = *idxp - 1;
    ಅಕ್ಷರ	    *s = ಖಾಲಿ;
    ಸ್ಥಿರ char_u   numbuf[NUMBUFLEN];

    ಆದರೆ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	++*idxp;
	ಆದರೆ (tofree != ಖಾಲಿ)
	    s = (ಅಕ್ಷರ *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	ಇಲ್ಲದಿದ್ದರೆ
	    s = (ಅಕ್ಷರ *)tv_get_string_chk(&tvs[idx]);
    }
    ಹಿಂತಿರುಗಿ s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ಸ್ಥಿರ ದ್ವಿಗುಣ
tv_float(typval_T *tvs, ಪೂರ್ಣಾಂಕ *idxp)
{
    ಪೂರ್ಣಾಂಕ		idx = *idxp - 1;
    ದ್ವಿಗುಣ	f = 0;

    ಆದರೆ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	++*idxp;
	ಆದರೆ (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tvs[idx].v_type == VAR_ಸಂಖ್ಯೆ)
	    f = (ದ್ವಿಗುಣ)tvs[idx].vval.v_ಸಂಖ್ಯೆ;
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_expected_float_ವಾದ_for_printf));
    }
    ಹಿಂತಿರುಗಿ f;
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
infinity_str(ಪೂರ್ಣಾಂಕ ಧನಾತ್ಮಕ,
	     ಅಕ್ಷರ fmt_spec,
	     ಪೂರ್ಣಾಂಕ force_sign,
	     ಪೂರ್ಣಾಂಕ space_for_ಧನಾತ್ಮಕ)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೋಷ್ಟಕ[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    ಪೂರ್ಣಾಂಕ idx = ಧನಾತ್ಮಕ * (1 + force_sign + force_sign * space_for_ಧನಾತ್ಮಕ);

    ಆದರೆ (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    ಹಿಂತಿರುಗಿ ಕೋಷ್ಟಕ[idx];
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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO

// Like vim_vsnprintf() but append to the string.
    ಪೂರ್ಣಾಂಕ
vim_snprintf_ಕೂಡಿಸು(ಅಕ್ಷರ *str, size_t str_m, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
    va_list	ap;
    ಪೂರ್ಣಾಂಕ		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    ಆದರೆ (str_m <= len)
	space = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    ಹಿಂತಿರುಗಿ str_l;
}

    ಪೂರ್ಣಾಂಕ
vim_snprintf(ಅಕ್ಷರ *str, size_t str_m, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
    va_list	ap;
    ಪೂರ್ಣಾಂಕ		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    ಹಿಂತಿರುಗಿ str_l;
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
vim_snprintf_safelen(ಅಕ್ಷರ *str, size_t str_m, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
    va_list ap;
    ಪೂರ್ಣಾಂಕ	    str_l;

    ಆದರೆ (str_m == 0)
	ಹಿಂತಿರುಗಿ 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, ಖಾಲಿ);
    va_end(ap);

    ಆದರೆ (str_l < 0)
    {
	*str = NUL;
	ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    ಪೂರ್ಣಾಂಕ
vim_vsnprintf(
    ಅಕ್ಷರ	*str,
    size_t	str_m,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*fmt,
    va_list	ap)
{
    ಹಿಂತಿರುಗಿ vim_vsnprintf_typval(str, str_m, fmt, ap, ಖಾಲಿ);
}

ಎಣಿಕೆ
{
    TYPE_UNKNOWN = -1,
    TYPE_INT,
    TYPE_LONGINT,
    TYPE_LONGLONGINT,
    TYPE_UNSIGNEDINT,
    TYPE_UNSIGNEDLONGINT,
    TYPE_UNSIGNEDLONGLONGINT,
    TYPE_ಸೂಚಕ,
    TYPE_PERCENT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_FLOAT
};

/* Types that can be used in a format string
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
format_typeof(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*type)
{
    // allowed values: \0, h, l, L
    ಅಕ್ಷರ    ಉದ್ದಳತೆ_modifier = '\0';

    // current conversion specifier character
    ಅಕ್ಷರ    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    ಆದರೆ (*type == 'h' || *type == 'l')
    {
	ಉದ್ದಳತೆ_modifier = *type;
	type++;
	ಆದರೆ (ಉದ್ದಳತೆ_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    ಉದ್ದಳತೆ_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    ಆಯ್ಕೆ (fmt_spec)
    {
	ಸ್ಥಿತಿ 'i': fmt_spec = 'd'; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '*': fmt_spec = 'd'; ಉದ್ದಳತೆ_modifier = 'h'; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'D': fmt_spec = 'd'; ಉದ್ದಳತೆ_modifier = 'l'; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'U': fmt_spec = 'u'; ಉದ್ದಳತೆ_modifier = 'l'; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'O': fmt_spec = 'o'; ಉದ್ದಳತೆ_modifier = 'l'; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;
    }

    // get parameter value, do initial processing
    ಆಯ್ಕೆ (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    ಸ್ಥಿತಿ '%':
	ಹಿಂತಿರುಗಿ TYPE_PERCENT;

    ಸ್ಥಿತಿ 'c':
	ಹಿಂತಿರುಗಿ TYPE_CHAR;

    ಸ್ಥಿತಿ 's':
    ಸ್ಥಿತಿ 'S':
	ಹಿಂತಿರುಗಿ TYPE_STRING;

    ಸ್ಥಿತಿ 'd': ಸ್ಥಿತಿ 'u':
    ಸ್ಥಿತಿ 'b': ಸ್ಥಿತಿ 'B':
    ಸ್ಥಿತಿ 'o':
    ಸ್ಥಿತಿ 'x': ಸ್ಥಿತಿ 'X':
    ಸ್ಥಿತಿ 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //ಮೌಲ್ಯ

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    ಆದರೆ (fmt_spec == 'p')
		ಹಿಂತಿರುಗಿ TYPE_ಸೂಚಕ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt_spec == 'b' || fmt_spec == 'B')
		ಹಿಂತಿರುಗಿ TYPE_UNSIGNEDLONGLONGINT;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt_spec == 'd')
	    {
		// signed
		ಆಯ್ಕೆ (ಉದ್ದಳತೆ_modifier)
		{
		ಸ್ಥಿತಿ '\0':
		ಸ್ಥಿತಿ 'h':
		    // char and short arguments are passed as int.
		    ಹಿಂತಿರುಗಿ TYPE_INT;
		ಸ್ಥಿತಿ 'l':
		    ಹಿಂತಿರುಗಿ TYPE_LONGINT;
		ಸ್ಥಿತಿ 'L':
		    ಹಿಂತಿರುಗಿ TYPE_LONGLONGINT;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// unsigned
		ಆಯ್ಕೆ (ಉದ್ದಳತೆ_modifier)
		{
		    ಸ್ಥಿತಿ '\0':
		    ಸ್ಥಿತಿ 'h':
			ಹಿಂತಿರುಗಿ TYPE_UNSIGNEDINT;
		    ಸ್ಥಿತಿ 'l':
			ಹಿಂತಿರುಗಿ TYPE_UNSIGNEDLONGINT;
		    ಸ್ಥಿತಿ 'L':
			ಹಿಂತಿರುಗಿ TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 'f':
    ಸ್ಥಿತಿ 'F':
    ಸ್ಥಿತಿ 'e':
    ಸ್ಥಿತಿ 'E':
    ಸ್ಥಿತಿ 'g':
    ಸ್ಥಿತಿ 'G':
	ಹಿಂತಿರುಗಿ TYPE_FLOAT;
    }

    ಹಿಂತಿರುಗಿ TYPE_UNKNOWN;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
format_typename(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *type)
{
    ಆಯ್ಕೆ (format_typeof(type))
    {
	ಸ್ಥಿತಿ TYPE_INT:
	    ಹಿಂತಿರುಗಿ typename_int;

	ಸ್ಥಿತಿ TYPE_LONGINT:
	    ಹಿಂತಿರುಗಿ typename_longint;

	ಸ್ಥಿತಿ TYPE_LONGLONGINT:
	    ಹಿಂತಿರುಗಿ typename_longlongint;

	ಸ್ಥಿತಿ TYPE_UNSIGNEDINT:
	    ಹಿಂತಿರುಗಿ typename_unsignedint;

	ಸ್ಥಿತಿ TYPE_UNSIGNEDLONGINT:
	    ಹಿಂತಿರುಗಿ typename_unsignedlongint;

	ಸ್ಥಿತಿ TYPE_UNSIGNEDLONGLONGINT:
	    ಹಿಂತಿರುಗಿ typename_unsignedlonglongint;

	ಸ್ಥಿತಿ TYPE_ಸೂಚಕ:
	    ಹಿಂತಿರುಗಿ _(typename_ಸೂಚಕ);

	ಸ್ಥಿತಿ TYPE_PERCENT:
	    ಹಿಂತಿರುಗಿ _(typename_percent);

	ಸ್ಥಿತಿ TYPE_CHAR:
	    ಹಿಂತಿರುಗಿ typename_char;

	ಸ್ಥಿತಿ TYPE_STRING:
	    ಹಿಂತಿರುಗಿ _(typename_string);

	ಸ್ಥಿತಿ TYPE_FLOAT:
	    ಹಿಂತಿರುಗಿ typename_float;
    }

    ಹಿಂತಿರುಗಿ _(typename_unknown);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
adjust_types(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ***ap_types,
    ಪೂರ್ಣಾಂಕ arg,
    ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_posarg,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type)
{
    ಆದರೆ (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (*ap_types == ಖಾಲಿ || *ಅಂಕೆ_posarg < arg)
    {
	ಪೂರ್ಣಾಂಕ	    idx;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  **new_types;

	ಆದರೆ (*ap_types == ಖಾಲಿ)
	    new_types = ALLOC_CLEAR_MULT(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, arg);
	ಇಲ್ಲದಿದ್ದರೆ
	    new_types = vim_realloc((ಅಕ್ಷರ **)*ap_types,
						arg * ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *));

	ಆದರೆ (new_types == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಗಾಗಿ (idx = *ಅಂಕೆ_posarg; idx < arg; ++idx)
	    new_types[idx] = ಖಾಲಿ;

	*ap_types = new_types;
	*ಅಂಕೆ_posarg = arg;
    }

    ಆದರೆ ((*ap_types)[arg - 1] != ಖಾಲಿ)
    {
	ಆದರೆ ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pt = type;
	    ಆದರೆ (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    ಆದರೆ (pt[0] != '*')
	    {
		ಆಯ್ಕೆ (pt[0])
		{
		    ಸ್ಥಿತಿ 'd':
		    ಸ್ಥಿತಿ 'i': ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ:
			semsg(_(e_positional_ಅಂಕೆ_ಕ್ಷೇತ್ರ_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_ಅಂಕೆ_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
format_ಅತಿಪ್ರವಾಹ_error(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pstart)
{
    size_t	arglen = 0;
    ಅಕ್ಷರ	*argcopy = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*p = pstart;

    ಆಗ (VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(ಅಕ್ಷರ, arglen + 1);
    ಆದರೆ (argcopy != ಖಾಲಿ)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	ಬಿಡುಗಡೆ(argcopy);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ_allocating_nr_ಬೈಟುಗಳು), arglen);
}

# define MAX_ALLOWED_STRING_ಅಗಲ 1048576    // 1 MiB

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_unsigned_int(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pstart,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p,
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *uj,
    ಪೂರ್ಣಾಂಕ ಅತಿಪ್ರವಾಹ_err)
{
    *uj = **p - '0';
    ++*p;

    ಆಗ (VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(**p)) && *uj < MAX_ALLOWED_STRING_ಅಗಲ)
    {
	*uj = 10 * *uj + (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(**p - '0');
	++*p;
    }

    ಆದರೆ (*uj > MAX_ALLOWED_STRING_ಅಗಲ)
    {
	ಆದರೆ (ಅತಿಪ್ರವಾಹ_err)
	{
	    format_ಅತಿಪ್ರವಾಹ_error(pstart);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *uj = MAX_ALLOWED_STRING_ಅಗಲ;
    }

    ಹಿಂತಿರುಗಿ OK;
}


    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_fmt_types(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  ***ap_types,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_posarg,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *fmt,
    typval_T	*tvs ಬಳಸದ
    )
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*p = fmt;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*arg = ಖಾಲಿ;

    ಪೂರ್ಣಾಂಕ		any_pos = 0;
    ಪೂರ್ಣಾಂಕ		any_arg = 0;
    ಪೂರ್ಣಾಂಕ		arg_idx;

# define CHECK_POS_ARG ಮಾಡು { \
    ಆದರೆ (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	ಹೋಗು error; \
    } \
} ಆಗ (0);

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    ಆಗ (*p != NUL)
    {
	ಆದರೆ (*p != '%')
	{
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ    *q = strchr(p + 1, '%');
	    size_t  n = (q == ಖಾಲಿ) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // allowed values: \0, h, l, L
	    ಅಕ್ಷರ	ಉದ್ದಳತೆ_modifier = '\0';

	    // variable for positional arg
	    ಪೂರ್ಣಾಂಕ		pos_arg = -1;
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*ptype = ಖಾಲಿ;
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    ಆಗ (VIM_ISDIGIT(*ptype))
		++ptype;

	    ಆದರೆ (*ptype == '$')
	    {
		ಆದರೆ (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    ಹೋಗು error;
		}

		// Positional argument
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uj;

		ಆದರೆ (get_unsigned_int(pstart, &p, &uj, tvs != ಖಾಲಿ) == FAIL)
		    ಹೋಗು error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    ಆಗ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		ಆಯ್ಕೆ (*p)
		{
		    ಸ್ಥಿತಿ '0': ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '-': ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '+': ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '#': ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '\'': ನಿಲ್ಲಿಸು;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    ಆದರೆ (*(arg = p) == '*')
	    {
		p++;

		ಆದರೆ (VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(*p)))
		{
		    // Positional argument field width
		    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uj;

		    ಆದರೆ (get_unsigned_int(arg + 1, &p, &uj, tvs != ಖಾಲಿ) == FAIL)
			ಹೋಗು error;

		    ಆದರೆ (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			ಹೋಗು error;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			ಆದರೆ (adjust_types(ap_types, uj, ಅಂಕೆ_posarg, arg) == FAIL)
			    ಹೋಗು error;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *digstart = p;
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uj;

		ಆದರೆ (get_unsigned_int(digstart, &p, &uj, tvs != ಖಾಲಿ) == FAIL)
		    ಹೋಗು error;

		ಆದರೆ (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    ಹೋಗು error;
		}
	    }

	    // parse precision
	    ಆದರೆ (*p == '.')
	    {
		p++;

		ಆದರೆ (*(arg = p) == '*')
		{
		    p++;

		    ಆದರೆ (VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(*p)))
		    {
			// Parse precision
			ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uj;

			ಆದರೆ (get_unsigned_int(arg + 1, &p, &uj, tvs != ಖಾಲಿ) == FAIL)
			    ಹೋಗು error;

			ಆದರೆ (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    ಆದರೆ (adjust_types(ap_types, uj, ಅಂಕೆ_posarg, arg) == FAIL)
				ಹೋಗು error;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    ಹೋಗು error;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *digstart = p;
		    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uj;

		    ಆದರೆ (get_unsigned_int(digstart, &p, &uj, tvs != ಖಾಲಿ) == FAIL)
			ಹೋಗು error;

		    ಆದರೆ (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			ಹೋಗು error;
		    }
		}
	    }

	    ಆದರೆ (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ಆದರೆ (*p == 'h' || *p == 'l')
	    {
		ಉದ್ದಳತೆ_modifier = *p;
		p++;
		ಆದರೆ (ಉದ್ದಳತೆ_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    ಆಯ್ಕೆ (*p)
	    {
		// Check for known format specifiers. % is special!
		ಸ್ಥಿತಿ 'i':
		ಸ್ಥಿತಿ '*':
		ಸ್ಥಿತಿ 'd':
		ಸ್ಥಿತಿ 'u':
		ಸ್ಥಿತಿ 'o':
		ಸ್ಥಿತಿ 'D':
		ಸ್ಥಿತಿ 'U':
		ಸ್ಥಿತಿ 'O':
		ಸ್ಥಿತಿ 'x':
		ಸ್ಥಿತಿ 'X':
		ಸ್ಥಿತಿ 'b':
		ಸ್ಥಿತಿ 'B':
		ಸ್ಥಿತಿ 'c':
		ಸ್ಥಿತಿ 's':
		ಸ್ಥಿತಿ 'S':
		ಸ್ಥಿತಿ 'p':
		ಸ್ಥಿತಿ 'f':
		ಸ್ಥಿತಿ 'F':
		ಸ್ಥಿತಿ 'e':
		ಸ್ಥಿತಿ 'E':
		ಸ್ಥಿತಿ 'g':
		ಸ್ಥಿತಿ 'G':
		    ಆದರೆ (pos_arg != -1)
		    {
			ಆದರೆ (adjust_types(ap_types, pos_arg, ಅಂಕೆ_posarg, ptype) == FAIL)
			    ಹೋಗು error;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
		    ಆದರೆ (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			ಹೋಗು error;
		    }
	    }

	    ಆದರೆ (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    ಗಾಗಿ (arg_idx = 0; arg_idx < *ಅಂಕೆ_posarg; ++arg_idx)
    {
	ಆದರೆ ((*ap_types)[arg_idx] == ಖಾಲಿ)
	{
	    semsg(_(e_fmt_arg_nr_ಬಳಸದ_str), arg_idx + 1, fmt);
	    ಹೋಗು error;
	}

# ಆದರೆ defined(FEAT_EVAL)
	ಆದರೆ (tvs != ಖಾಲಿ && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_ಹೊರಹೋಗು_ದ_bounds_str), arg_idx + 1, fmt);
	    ಹೋಗು error;
	}
# endif
    }

    ಹಿಂತಿರುಗಿ OK;

error:
    vim_free((ಅಕ್ಷರ**)*ap_types);
    *ap_types = ಖಾಲಿ;
    *ಅಂಕೆ_posarg = 0;
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
skip_ಗಮ್ಯ_arg(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	**ap_types,
    va_list	ap_ಆರಂಭ,
    va_list	*ap,
    ಪೂರ್ಣಾಂಕ		*arg_idx,
    ಪೂರ್ಣಾಂಕ		*arg_cur,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*fmt)
{
    ಪೂರ್ಣಾಂಕ		arg_min = 0;

    ಆದರೆ (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_ಆರಂಭ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    ಗಾಗಿ (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

	ಆದರೆ (ap_types == ಖಾಲಿ || ap_types[*arg_cur] == ಖಾಲಿ)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    ಹಿಂತಿರುಗಿ;
	}

	p = ap_types[*arg_cur];

	ಪೂರ್ಣಾಂಕ fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	ಆಯ್ಕೆ (fmt_type)
	{
	ಸ್ಥಿತಿ TYPE_PERCENT:
	ಸ್ಥಿತಿ TYPE_UNKNOWN:
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ TYPE_CHAR:
	    va_arg(*ap, ಪೂರ್ಣಾಂಕ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ TYPE_STRING:
	    va_arg(*ap, ಅಕ್ಷರ *);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ TYPE_ಸೂಚಕ:
	    va_arg(*ap, ಶೂನ್ಯ *);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ TYPE_INT:
	    va_arg(*ap, ಪೂರ್ಣಾಂಕ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ TYPE_LONGINT:
	    va_arg(*ap, ದೀರ್ಘ ಪೂರ್ಣಾಂಕ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ TYPE_UNSIGNEDINT:
	    va_arg(*ap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ TYPE_FLOAT:
	    va_arg(*ap, ದ್ವಿಗುಣ);
	    ನಿಲ್ಲಿಸು;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    ಹಿಂತಿರುಗಿ;
}

    ಪೂರ್ಣಾಂಕ
vim_vsnprintf_typval(
    ಅಕ್ಷರ	*str,
    size_t	str_m,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*fmt,
    va_list	ap_ಆರಂಭ,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*p = fmt;
    ಪೂರ್ಣಾಂಕ		arg_cur = 0;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_posarg = 0;
    ಪೂರ್ಣಾಂಕ		arg_idx = 1;
    va_list	ap;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	**ap_types = ಖಾಲಿ;

    ಆದರೆ (parse_fmt_types(&ap_types, &ಅಂಕೆ_posarg, fmt, tvs) == FAIL)
	ಹಿಂತಿರುಗಿ 0;

    va_copy(ap, ap_ಆರಂಭ);

    ಆದರೆ (p == ಖಾಲಿ)
	p = "";
    ಆಗ (*p != NUL)
    {
	ಆದರೆ (*p != '%')
	{
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ    *q = strchr(p + 1, '%');
	    size_t  n = (q == ಖಾಲಿ) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    ಆದರೆ (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    size_t  min_ಕ್ಷೇತ್ರ_ಅಗಲ = 0, precision = 0;
	    ಪೂರ್ಣಾಂಕ	    zero_padding = 0, precision_specified = 0, justify_ಎಡ = 0;
	    ಪೂರ್ಣಾಂಕ	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    ಪೂರ್ಣಾಂಕ	    space_for_ಧನಾತ್ಮಕ = 1;

	    // allowed values: \0, h, l, L
	    ಅಕ್ಷರ    ಉದ್ದಳತೆ_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    ಅಕ್ಷರ    tmp[TMP_LEN];

	    // string address in case of string argument
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *str_arg = ಖಾಲಿ;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  ಸಂಖ್ಯೆ_ದ_zeros_ಗಮ್ಯ_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    ಅಕ್ಷರ    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = ಖಾಲಿ;

	    // variables for positional arg
	    ಪೂರ್ಣಾಂಕ	    pos_arg = -1;
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    ಆಗ (VIM_ISDIGIT(*ptype))
		++ptype;

	    ಆದರೆ (*ptype == '$')
	    {
		// Positional argument
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *digstart = p;
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uj;

		ಆದರೆ (get_unsigned_int(digstart, &p, &uj, tvs != ಖಾಲಿ) == FAIL)
		    ಹೋಗು error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    ಆಗ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		ಆಯ್ಕೆ (*p)
		{
		    ಸ್ಥಿತಿ '0': zero_padding = 1; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '-': justify_ಎಡ = 1; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '+': force_sign = 1; space_for_ಧನಾತ್ಮಕ = 0; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '#': alternate_form = 1; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '\'': ನಿಲ್ಲಿಸು;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    ಆದರೆ (*p == '*')
	    {
		ಪೂರ್ಣಾಂಕ j;
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *digstart = p + 1;

		p++;

		ಆದರೆ (VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(*p)))
		{
		    // Positional argument field width
		    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uj;

		    ಆದರೆ (get_unsigned_int(digstart, &p, &uj, tvs != ಖಾಲಿ) == FAIL)
			ಹೋಗು error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# ಆದರೆ defined(FEAT_EVAL)
		    tvs != ಖಾಲಿ ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, ಪೂರ್ಣಾಂಕ));

		ಆದರೆ (j > MAX_ALLOWED_STRING_ಅಗಲ)
		{
		    ಆದರೆ (tvs != ಖಾಲಿ)
		    {
			format_ಅತಿಪ್ರವಾಹ_error(digstart);
			ಹೋಗು error;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			j = MAX_ALLOWED_STRING_ಅಗಲ;
		}

		ಆದರೆ (j >= 0)
		    min_ಕ್ಷೇತ್ರ_ಅಗಲ = j;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    min_ಕ್ಷೇತ್ರ_ಅಗಲ = -j;
		    justify_ಎಡ = 1;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *digstart = p;
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uj;

		ಆದರೆ (get_unsigned_int(digstart, &p, &uj, tvs != ಖಾಲಿ) == FAIL)
		    ಹೋಗು error;

		min_ಕ್ಷೇತ್ರ_ಅಗಲ = uj;
	    }

	    // parse precision
	    ಆದರೆ (*p == '.')
	    {
		p++;
		precision_specified = 1;

		ಆದರೆ (VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *digstart = p;
		    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uj;

		    ಆದರೆ (get_unsigned_int(digstart, &p, &uj, tvs != ಖಾಲಿ) == FAIL)
			ಹೋಗು error;

		    precision = uj;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '*')
		{
		    ಪೂರ್ಣಾಂಕ j;
		    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *digstart = p;

		    p++;

		    ಆದರೆ (VIM_ISDIGIT((ಪೂರ್ಣಾಂಕ)(*p)))
		    {
			// positional argument
			ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uj;

			ಆದರೆ (get_unsigned_int(digstart, &p, &uj, tvs != ಖಾಲಿ) == FAIL)
			    ಹೋಗು error;

			arg_idx = uj;

			++p;
		    }

		    j =
# ಆದರೆ defined(FEAT_EVAL)
			tvs != ಖಾಲಿ ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, ಪೂರ್ಣಾಂಕ));

		    ಆದರೆ (j > MAX_ALLOWED_STRING_ಅಗಲ)
		    {
			ಆದರೆ (tvs != ಖಾಲಿ)
			{
			    format_ಅತಿಪ್ರವಾಹ_error(digstart);
			    ಹೋಗು error;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    j = MAX_ALLOWED_STRING_ಅಗಲ;
		    }

		    ಆದರೆ (j >= 0)
			precision = j;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ಆದರೆ (*p == 'h' || *p == 'l')
	    {
		ಉದ್ದಳತೆ_modifier = *p;
		p++;
		ಆದರೆ (ಉದ್ದಳತೆ_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    ಉದ್ದಳತೆ_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    ಆಯ್ಕೆ (fmt_spec)
	    {
		ಸ್ಥಿತಿ 'i': fmt_spec = 'd'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'D': fmt_spec = 'd'; ಉದ್ದಳತೆ_modifier = 'l'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'U': fmt_spec = 'u'; ಉದ್ದಳತೆ_modifier = 'l'; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'O': fmt_spec = 'o'; ಉದ್ದಳತೆ_modifier = 'l'; ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;
	    }

# ಆದರೆ defined(FEAT_EVAL)
	    ಆಯ್ಕೆ (fmt_spec)
	    {
		ಸ್ಥಿತಿ 'd': ಸ್ಥಿತಿ 'u': ಸ್ಥಿತಿ 'o': ಸ್ಥಿತಿ 'x': ಸ್ಥಿತಿ 'X':
		    ಆದರೆ (tvs != ಖಾಲಿ && ಉದ್ದಳತೆ_modifier == '\0')
			ಉದ್ದಳತೆ_modifier = 'L';
	    }
# endif

	    ಆದರೆ (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    ಆಯ್ಕೆ (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    ಸ್ಥಿತಿ '%':
	    ಸ್ಥಿತಿ 'c':
	    ಸ್ಥಿತಿ 's':
	    ಸ್ಥಿತಿ 'S':
		str_arg_l = 1;
		ಆಯ್ಕೆ (fmt_spec)
		{
		ಸ್ಥಿತಿ '%':
		    str_arg = p;
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'c':
		    {
			ಪೂರ್ಣಾಂಕ j;

			j =
# ಆದರೆ defined(FEAT_EVAL)
			    tvs != ಖಾಲಿ ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, ಪೂರ್ಣಾಂಕ));

			// standard demands unsigned char
			uchar_arg = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)j;
			str_arg = (ಅಕ್ಷರ *)&uchar_arg;
			ನಿಲ್ಲಿಸು;
		    }

		ಸ್ಥಿತಿ 's':
		ಸ್ಥಿತಿ 'S':
		    str_arg =
# ಆದರೆ defined(FEAT_EVAL)
				tvs != ಖಾಲಿ ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, ಅಕ್ಷರ *));

		    ಆದರೆ (str_arg == ಖಾಲಿ)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!precision_specified)
			str_arg_l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str_arg);
		    // truncate string if necessary as requested by precision
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (precision == 0)
			str_arg_l = 0;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// memchr on HP does not like n > 2^31  !!!
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == ಖಾಲಿ) ? precision
						      : (size_t)(q - str_arg);
		    }
		    ಆದರೆ (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			ಪೂರ್ಣಾಂಕ	cell;

			ಗಾಗಿ (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    ಆದರೆ (precision_specified && i + cell > precision)
				ನಿಲ್ಲಿಸು;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			ಆದರೆ (min_ಕ್ಷೇತ್ರ_ಅಗಲ != 0)
			    min_ಕ್ಷೇತ್ರ_ಅಗಲ += str_arg_l - i;
		    }
		    ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
		    ನಿಲ್ಲಿಸು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'd': ಸ್ಥಿತಿ 'u':
	    ಸ್ಥಿತಿ 'b': ಸ್ಥಿತಿ 'B':
	    ಸ್ಥಿತಿ 'o':
	    ಸ್ಥಿತಿ 'x': ಸ್ಥಿತಿ 'X':
	    ಸ್ಥಿತಿ 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //ಮೌಲ್ಯ

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    ಪೂರ್ಣಾಂಕ arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    ಪೂರ್ಣಾಂಕ int_arg = 0;
		    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uint_arg = 0;

		    // only set for length modifier l
		    ದೀರ್ಘ ಪೂರ್ಣಾಂಕ long_arg = 0;
		    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    ಶೂನ್ಯ *ptr_arg = ಖಾಲಿ;

		    ಆದರೆ (fmt_spec == 'p')
		    {
			ಉದ್ದಳತೆ_modifier = '\0';
			ptr_arg =
# ಆದರೆ defined(FEAT_EVAL)
				 tvs != ಖಾಲಿ ? (ಶೂನ್ಯ *)tv_str(tvs, &arg_idx,
									ಖಾಲಿ) :
# endif
					(skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, ಶೂನ್ಯ *));

			ಆದರೆ (ptr_arg != ಖಾಲಿ)
			    arg_sign = 1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# ಆದರೆ defined(FEAT_EVAL)
				    tvs != ಖಾಲಿ ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			ಆದರೆ (bin_arg != 0)
			    arg_sign = 1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt_spec == 'd')
		    {
			// signed
			ಆಯ್ಕೆ (ಉದ್ದಳತೆ_modifier)
			{
			ಸ್ಥಿತಿ '\0':
			ಸ್ಥಿತಿ 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# ಆದರೆ defined(FEAT_EVAL)
					tvs != ಖಾಲಿ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, ಪೂರ್ಣಾಂಕ));

			    ಆದರೆ (int_arg > 0)
				arg_sign =  1;
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (int_arg < 0)
				arg_sign = -1;
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 'l':
			    long_arg =
# ಆದರೆ defined(FEAT_EVAL)
					tvs != ಖಾಲಿ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, ದೀರ್ಘ ಪೂರ್ಣಾಂಕ));

			    ಆದರೆ (long_arg > 0)
				arg_sign =  1;
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (long_arg < 0)
				arg_sign = -1;
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 'L':
			    llong_arg =
# ಆದರೆ defined(FEAT_EVAL)
					tvs != ಖಾಲಿ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    ಆದರೆ (llong_arg > 0)
				arg_sign =  1;
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (llong_arg < 0)
				arg_sign = -1;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// unsigned
			ಆಯ್ಕೆ (ಉದ್ದಳತೆ_modifier)
			{
			    ಸ್ಥಿತಿ '\0':
			    ಸ್ಥಿತಿ 'h':
				uint_arg =
# ಆದರೆ defined(FEAT_EVAL)
					    tvs != ಖಾಲಿ ? (ಚಿಹ್ನೆರಹಿತ)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ));

				ಆದರೆ (uint_arg != 0)
				    arg_sign = 1;
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'l':
				ulong_arg =
# ಆದರೆ defined(FEAT_EVAL)
					    tvs != ಖಾಲಿ ? (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ));

				ಆದರೆ (ulong_arg != 0)
				    arg_sign = 1;
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'L':
				ullong_arg =
# ಆದರೆ defined(FEAT_EVAL)
					    tvs != ಖಾಲಿ ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				ಆದರೆ (ullong_arg != 0)
				    arg_sign = 1;
				ನಿಲ್ಲಿಸು;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    ಆದರೆ (precision_specified)
			zero_padding = 0;
		    ಆದರೆ (fmt_spec == 'd')
		    {
			ಆದರೆ (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_ಧನಾತ್ಮಕ ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (alternate_form)
		    {
			ಆದರೆ (arg_sign != 0
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
		    ಆದರೆ (!precision_specified)
			precision = 1;   // default precision is 1
		    ಆದರೆ (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಅಕ್ಷರ	f[6];
			ಪೂರ್ಣಾಂಕ	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			ಆದರೆ (!ಉದ್ದಳತೆ_modifier)
			    ;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಉದ್ದಳತೆ_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# ಇಲ್ಲದಿದ್ದರೆ
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    f[f_l++] = ಉದ್ದಳತೆ_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			ಆದರೆ (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    ಅಕ್ಷರ	    b[8 * ಗಾತ್ರ(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    ಮಾಡು
			    {
				b[ಗಾತ್ರ(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    ಆಗ (bn != 0);

			    ಸ್ಮರಣೆ_ನಕಲು(tmp + str_arg_l, b + ಗಾತ್ರ(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt_spec == 'd')
			{
			    // signed
			    ಆಯ್ಕೆ (ಉದ್ದಳತೆ_modifier)
			    {
			    ಸ್ಥಿತಿ '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (ಗಿಡ್ಡ)int_arg);
				      ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      ನಿಲ್ಲಿಸು;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // unsigned
			    ಆಯ್ಕೆ (ಉದ್ದಳತೆ_modifier)
			    {
			    ಸ್ಥಿತಿ '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)uint_arg);
				      ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      ನಿಲ್ಲಿಸು;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			ಆದರೆ (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			ಆದರೆ (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t ಅಂಕೆ_ದ_digits = str_arg_l
						 - zero_padding_insertion_ind;

			ಆದರೆ (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    ಆದರೆ (!precision_specified
					     || precision < ಅಂಕೆ_ದ_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = ಅಂಕೆ_ದ_digits + 1;
			    }
			}
			// zero padding to specified precision?
			ಆದರೆ (ಅಂಕೆ_ದ_digits < precision)
			    ಸಂಖ್ಯೆ_ದ_zeros_ಗಮ್ಯ_pad = precision - ಅಂಕೆ_ದ_digits;
		    }
		    // zero padding to specified minimal field width?
		    ಆದರೆ (!justify_ಎಡ && zero_padding)
		    {
			ಪೂರ್ಣಾಂಕ n = (ಪೂರ್ಣಾಂಕ)(min_ಕ್ಷೇತ್ರ_ಅಗಲ - (str_arg_l
						    + ಸಂಖ್ಯೆ_ದ_zeros_ಗಮ್ಯ_pad));
			ಆದರೆ (n > 0)
			    ಸಂಖ್ಯೆ_ದ_zeros_ಗಮ್ಯ_pad += n;
		    }
		    ನಿಲ್ಲಿಸು;
		}

	    ಸ್ಥಿತಿ 'f':
	    ಸ್ಥಿತಿ 'F':
	    ಸ್ಥಿತಿ 'e':
	    ಸ್ಥಿತಿ 'E':
	    ಸ್ಥಿತಿ 'g':
	    ಸ್ಥಿತಿ 'G':
		{
		    // Floating point.
		    ದ್ವಿಗುಣ	f;
		    ದ್ವಿಗುಣ	abs_f;
		    ಅಕ್ಷರ	format[40];
		    ಪೂರ್ಣಾಂಕ		l;
		    ಪೂರ್ಣಾಂಕ		remove_trailing_zeroes = FALSE;

		    f =
# ಆದರೆ defined(FEAT_EVAL)
			tvs != ಖಾಲಿ ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_ಗಮ್ಯ_arg(ap_types, ap_ಆರಂಭ, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, ದ್ವಿಗುಣ));

		    abs_f = f < 0 ? -f : f;

		    ಆದರೆ (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			ಆದರೆ ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			ಇಲ್ಲದಿದ್ದರೆ
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    ಆದರೆ ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# ಇಲ್ಲದಿದ್ದರೆ
			    abs_f > 1.0e307
# endif
			    )
		    {
			// Avoid a buffer overflow
			STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_ಧನಾತ್ಮಕ));
			str_arg_l = STRLEN(tmp);
			zero_padding = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_ಧನಾತ್ಮಕ));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    ಆದರೆ (force_sign)
				format[l++] = space_for_ಧನಾತ್ಮಕ ? ' ' : '+';
			    ಆದರೆ (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				ಆದರೆ ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				ಆದರೆ (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (ಪೂರ್ಣಾಂಕ)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			ಆದರೆ (remove_trailing_zeroes)
			{
			    ಪೂರ್ಣಾಂಕ i;
			    ಅಕ್ಷರ *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    ಆದರೆ (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				tp = (ಅಕ್ಷರ *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				ಆದರೆ (tp != ಖಾಲಿ)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    ಆದರೆ (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    ಆಗ (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    ಆದರೆ (tp != ಖಾಲಿ && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				ಆಗ (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಅಕ್ಷರ *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (ಅಕ್ಷರ *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    ಆದರೆ (tp != ಖಾಲಿ && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    ಆದರೆ (zero_padding && min_ಕ್ಷೇತ್ರ_ಅಗಲ > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			ಸಂಖ್ಯೆ_ದ_zeros_ಗಮ್ಯ_pad = min_ಕ್ಷೇತ್ರ_ಅಗಲ - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    ನಿಲ್ಲಿಸು;
		}

	    ಪೂರ್ವನಿಯೋಜಿತ:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_ಎಡ = 1;
		min_ಕ್ಷೇತ್ರ_ಅಗಲ = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		ಆದರೆ (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    ಆದರೆ (!justify_ಎಡ)
	    {
		// left padding with blank or zero
		ಪೂರ್ಣಾಂಕ pn = (ಪೂರ್ಣಾಂಕ)(min_ಕ್ಷೇತ್ರ_ಅಗಲ - (str_arg_l + ಸಂಖ್ಯೆ_ದ_zeros_ಗಮ್ಯ_pad));

		ಆದರೆ (pn > 0)
		{
		    ಆದರೆ (str_l < str_m)
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
	    ಆದರೆ (ಸಂಖ್ಯೆ_ದ_zeros_ಗಮ್ಯ_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		ಪೂರ್ಣಾಂಕ zn = (ಪೂರ್ಣಾಂಕ)zero_padding_insertion_ind;

		ಆದರೆ (zn > 0)
		{
		    ಆದರೆ (str_l < str_m)
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
		zn = (ಪೂರ್ಣಾಂಕ)ಸಂಖ್ಯೆ_ದ_zeros_ಗಮ್ಯ_pad;
		ಆದರೆ (zn > 0)
		{
		    ಆದರೆ (str_l < str_m)
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
		ಪೂರ್ಣಾಂಕ sn = (ಪೂರ್ಣಾಂಕ)(str_arg_l - zero_padding_insertion_ind);

		ಆದರೆ (sn > 0)
		{
		    ಆದರೆ (str_l < str_m)
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
	    ಆದರೆ (justify_ಎಡ)
	    {
		// right blank padding to the field width
		ಪೂರ್ಣಾಂಕ pn = (ಪೂರ್ಣಾಂಕ)(min_ಕ್ಷೇತ್ರ_ಅಗಲ
				      - (str_arg_l + ಸಂಖ್ಯೆ_ದ_zeros_ಗಮ್ಯ_pad));

		ಆದರೆ (pn > 0)
		{
		    ಆದರೆ (str_l < str_m)
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

    ಆದರೆ (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    ಆದರೆ (tvs != ಖಾಲಿ && tvs[ಅಂಕೆ_posarg != 0 ? ಅಂಕೆ_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_ಗಮ್ಯ_printf));

error:
    vim_free((ಅಕ್ಷರ*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)str_l;
}

#ಆದರೆ_ಅಂತ್ಯ // PROTO
