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

#నిర్వచించు USING_FLOAT_STUFF
#అన్తర్భావ "vim.h"

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
    ఒకవేళ (p != ఖాళీ)
	mch_memmove(p, string, len);
    తిరిగి p;
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
    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;

    STRNCPY(p, string, len);
    p[len] = NUL;
    తిరిగి p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    తిరిగి vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    పూర్ణాంకం		cc,
    పూర్ణాంకం		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    గుర్తులేని	నిడివి;
    పూర్ణాంకం		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    నిడివి = 1;				// count the trailing NUL
    కొరకు (p = string; *p; p++)
    {
	ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    నిడివి += l;		// count a multibyte char
	    p += l - 1;
	    కొనసాగు;
	}
	ఒకవేళ (vim_strchr(esc_chars, *p) != ఖాళీ || (bsl && rem_backslash(p)))
	    ++నిడివి;			// count a backslash
	++నిడివి;			// count an ordinary char
    }
    escaped_string = alloc(నిడివి);
    ఒకవేళ (escaped_string == ఖాళీ)
	తిరిగి ఖాళీ;
    p2 = escaped_string;
    కొరకు (p = string; *p; p++)
    {
	ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    కొనసాగు;
	}
	ఒకవేళ (vim_strchr(esc_chars, *p) != ఖాళీ || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    తిరిగి escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    పూర్ణాంకం
csh_like_shell(శూన్య)
{
    తిరిగి (అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(p_sh), "csh") != ఖాళీ);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    స్థిర పూర్ణాంకం
fish_like_shell(శూన్య)
{
    తిరిగి (అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(p_sh), "fish") != ఖాళీ);
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
vim_strsave_shellescape(char_u *string, పూర్ణాంకం do_special, పూర్ణాంకం do_newline)
{
    గుర్తులేని	నిడివి;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    పూర్ణాంకం		csh_like;
    పూర్ణాంకం		fish_like;
    char_u	*shname;
    పూర్ణాంకం		powershell;
#నిర్వచితమైతే MSWIN
    పూర్ణాంకం		double_quotes;
#అయితే_ముగింపు

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
    powershell = అక్షరశ్రేణి_శోధన((అక్షరం *)shname, "pwsh") != ఖాళీ;
#నిర్వచితమైతే MSWIN
    powershell = powershell || అక్షరశ్రేణి_శోధన((అక్షరం *)shname, "powershell") != ఖాళీ;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#అయితే_ముగింపు

    // First count the number of extra bytes required.
    నిడివి = (గుర్తులేని)STRLEN(string) + 3;  // two quotes and a trailing NUL
    కొరకు (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#నిర్వచితమైతే MSWIN
	ఒకవేళ (double_quotes)
	{
	    ఒకవేళ (*p == '"')
		++నిడివి;		// " -> ""
	}
	లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (*p == '\'')
	{
	    ఒకవేళ (powershell)
		నిడివి += 2;		// ' => ''
	    లేకపోతే
		నిడివి += 3;		// ' => '\''
	}
	ఒకవేళ ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++నిడివి;			// insert backslash
	    ఒకవేళ (csh_like && do_special)
		++నిడివి;		// insert backslash
	}
	ఒకవేళ (do_special && కనుగొను_cmdline_var(p, &l) >= 0)
	{
	    ++నిడివి;			// insert backslash
	    p += l - 1;
	}
	ఒకవేళ (*p == '\\' && fish_like)
	    ++నిడివి;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(నిడివి);
    ఒకవేళ (escaped_string != ఖాళీ)
    {
	d = escaped_string;

	// add opening quote
#నిర్వచితమైతే MSWIN
	ఒకవేళ (double_quotes)
	    *d++ = '"';
	లేకపోతే
#అయితే_ముగింపు
	    *d++ = '\'';

	కొరకు (p = string; *p != NUL; )
	{
#నిర్వచితమైతే MSWIN
	    ఒకవేళ (double_quotes)
	    {
		ఒకవేళ (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    కొనసాగు;
		}
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    ఒకవేళ (*p == '\'')
	    {
		ఒకవేళ (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		లేకపోతే
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		కొనసాగు;
	    }
	    ఒకవేళ ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		ఒకవేళ (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		కొనసాగు;
	    }
	    ఒకవేళ (do_special && కనుగొను_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		జ్ఞాపకం_నకలు(d, p, l);	// copy the var
		d += l;
		p += l;
		కొనసాగు;
	    }
	    ఒకవేళ (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		కొనసాగు;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#నిర్వచితమైతే MSWIN
	ఒకవేళ (double_quotes)
	    *d++ = '"';
	లేకపోతే
#అయితే_ముగింపు
	    *d++ = '\'';
	*d = NUL;
    }

    తిరిగి escaped_string;
}

/*
 * Like vim_strsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strsave_పైకి(char_u *string)
{
    char_u *p1;

    p1 = vim_strsave(string);
    vim_strup(p1);
    తిరిగి p1;
}

/*
 * Like vim_strnsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strnsave_పైకి(char_u *string, size_t len)
{
    char_u *p1;

    p1 = vim_strnsave(string, len);
    vim_strup(p1);
    తిరిగి p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    శూన్య
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    పూర్ణాంకం	    c;

    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    p2 = p;
    అయితే ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    స్థిర char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    ఒకవేళ (res != ఖాళీ)
	అయితే (*p != NUL)
	{
	    పూర్ణాంకం		l;

	    ఒకవేళ (enc_utf8)
	    {
		పూర్ణాంకం	c, uc;
		పూర్ణాంకం	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ఒకవేళ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		ఒకవేళ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ఒకవేళ (s == ఖాళీ)
		    {
			vim_free(res);
			తిరిగి ఖాళీ;
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
	    లేకపోతే ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    లేకపోతే
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    తిరిగి res;
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

    ఒకవేళ (res != ఖాళీ)
	అయితే (*p != NUL)
	{
	    పూర్ణాంకం		l;

	    ఒకవేళ (enc_utf8)
	    {
		పూర్ణాంకం	c, lc;
		పూర్ణాంకం	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ఒకవేళ (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		ఒకవేళ (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ఒకవేళ (s == ఖాళీ)
		    {
			vim_free(res);
			తిరిగి ఖాళీ;
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
	    లేకపోతే ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    లేకపోతే
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    తిరిగి res;
}
#అయితే_ముగింపు

/*
 * delete spaces at the end of a string
 */
    శూన్య
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    అయితే (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    శూన్య
vim_strncpy(char_u *గమ్యం, char_u *from, size_t len)
{
    STRNCPY(గమ్యం, from, len);
    గమ్యం[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    శూన్య
vim_strcat(char_u *గమ్యం, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(గమ్యం);
    size_t fromlen = STRLEN(from);

    ఒకవేళ (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(గమ్యం + tolen, from, tosize - tolen - 1);
	గమ్యం[tosize - 1] = NUL;
    }
    లేకపోతే
	mch_memmove(గమ్యం + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(అక్షరం *s, size_t maxlen)
{
    size_t i;
    కొరకు (i = 0; i < maxlen; ++i)
	ఒకవేళ (s[i] == NUL)
	    ఆపు;
    తిరిగి i;
}

#ఒకవేళ !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    పూర్ణాంకం
vim_stricmp(అక్షరం *s1, అక్షరం *s2)
{
    పూర్ణాంకం		i;

    కొరకు (;;)
    {
	i = (పూర్ణాంకం)TOLOWER_LOC(*s1) - (పూర్ణాంకం)TOLOWER_LOC(*s2);
	ఒకవేళ (i != 0)
	    తిరిగి i;			    // this character different
	ఒకవేళ (*s1 == NUL)
	    ఆపు;			    // strings match until NUL
	++s1;
	++s2;
    }
    తిరిగి 0;				    // strings match
}
#అయితే_ముగింపు

#ఒకవేళ !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    పూర్ణాంకం
vim_strnicmp(అక్షరం *s1, అక్షరం *s2, size_t len)
{
    పూర్ణాంకం		i;

    అయితే (len > 0)
    {
	i = (పూర్ణాంకం)TOLOWER_LOC(*s1) - (పూర్ణాంకం)TOLOWER_LOC(*s2);
	ఒకవేళ (i != 0)
	    తిరిగి i;			    // this character different
	ఒకవేళ (*s1 == NUL)
	    ఆపు;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    తిరిగి 0;				    // strings match
}
#అయితే_ముగింపు

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    పూర్ణాంకం
vim_strnicmp_asc(అక్షరం *s1, అక్షరం *s2, size_t len)
{
    పూర్ణాంకం                i = 0;

    అయితే (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	ఒకవేళ (i != 0)
	    ఆపు;			// this character is different
	ఒకవేళ (*s1 == NUL)
	    ఆపు;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    తిరిగి i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, పూర్ణాంకం c)
{
    char_u	*p;
    పూర్ణాంకం		b;

    p = string;
    ఒకవేళ (enc_utf8 && c > 0 && c < 0x80)
	తిరిగి vim_strbyte(string, c);
    ఒకవేళ (enc_utf8 && c >= 0x80)
    {
	అయితే (*p != NUL)
	{
	    పూర్ణాంకం l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    ఒకవేళ (utf_ptr2char(p) == c && l > 1)
		తిరిగి p;
	    p += l;
	}
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (enc_dbcs != 0 && c > 255)
    {
	పూర్ణాంకం	n2 = c & 0xff;

	c = ((గుర్తులేని)c >> 8) & 0xff;
	అయితే ((b = *p) != NUL)
	{
	    ఒకవేళ (b == c && p[1] == n2)
		తిరిగి p;
	    p += (*mb_ptr2len)(p);
	}
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (has_mbyte)
    {
	అయితే ((b = *p) != NUL)
	{
	    ఒకవేళ (b == c)
		తిరిగి p;
	    p += (*mb_ptr2len)(p);
	}
	తిరిగి ఖాళీ;
    }
    అయితే ((b = *p) != NUL)
    {
	ఒకవేళ (b == c)
	    తిరిగి p;
	++p;
    }
    తిరిగి ఖాళీ;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, పూర్ణాంకం c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    ఒకవేళ (c <= 0 || c > 255)
	తిరిగి ఖాళీ;
    తిరిగి (char_u *)strchr((అక్షరం *)string, c);
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
vim_strrchr(char_u *string, పూర్ణాంకం c)
{
    char_u	*retval = ఖాళీ;
    char_u	*p = string;

    అయితే (*p)
    {
	ఒకవేళ (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    తిరిగి retval;
}

/*
 * Vim's version of strpbrk(), in case it's missing.
 * Don't generate a prototype for this, causes problems when it's not used.
 */
#నిర్వచితం_కాకపోతే PROTO
# ifndef HAVE_STRPBRK
#  ifdef vim_strpbrk
#   undef vim_strpbrk
#  endif
    char_u *
vim_strpbrk(char_u *s, char_u *charset)
{
    అయితే (*s)
    {
	ఒకవేళ (vim_strchr(charset, *s) != ఖాళీ)
	    తిరిగి s;
	MB_PTR_ADV(s);
    }
    తిరిగి ఖాళీ;
}
# endif
#అయితే_ముగింపు

/*
 * Sort an array of strings.
 */
స్థిర పూర్ణాంకం క్రమీకరించు_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2);

    స్థిర పూర్ణాంకం
క్రమీకరించు_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    తిరిగి STRCMP(*(అక్షరం **)s1, *(అక్షరం **)s2);
}

    శూన్య
క్రమీకరించు_అక్షరశ్రేణులు(
    char_u	**files,
    పూర్ణాంకం		లెక్క)
{
    qsort((శూన్య *)files, (size_t)లెక్క, పరిమాణం(char_u *), క్రమీకరించు_compare);
}

#ఒకవేళ defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    పూర్ణాంకం
has_non_ascii(char_u *s)
{
    char_u	*p;

    ఒకవేళ (s != ఖాళీ)
	కొరకు (p = s; *p != NUL; ++p)
	    ఒకవేళ (*p >= 128)
		తిరిగి TRUE;
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Concatenate two strings and return the result in allocated memory.
 * Returns NULL when out of memory.
 */
    char_u  *
concat_str(char_u *str1, char_u *str2)
{
    char_u  *dest;
    size_t  l = str1 == ఖాళీ ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == ఖాళీ ? 0 : STRLEN(str2)) + 1L);
    ఒకవేళ (dest == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (str1 == ఖాళీ)
	*dest = NUL;
    లేకపోతే
	STRCPY(dest, str1);
    ఒకవేళ (str2 != ఖాళీ)
	STRCPY(dest + l, str2);
    తిరిగి dest;
}

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
తిరగేయి_పాఠ్యం(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    ఒకవేళ (rev == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	ఒకవేళ (has_mbyte)
	{
	    పూర్ణాంకం mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	లేకపోతే
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    తిరిగి rev;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, పూర్ణాంకం function)
{
    గుర్తులేని	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    ఒకవేళ (str != ఖాళీ)
    {
	len += (గుర్తులేని)STRLEN(str);
	కొరకు (p = str; *p != NUL; MB_PTR_ADV(p))
	    ఒకవేళ (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    ఒకవేళ (r == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    లేకపోతే
	*r++ = '\'';
    ఒకవేళ (str != ఖాళీ)
	కొరకు (p = str; *p != NUL; )
	{
	    ఒకవేళ (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    ఒకవేళ (function)
	*r++ = ')';
    *r++ = NUL;
    తిరిగి s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    దీర్ఘ
string_లెక్క(char_u *haystack, char_u *needle, పూర్ణాంకం ic)
{
    దీర్ఘ	n = 0;
    char_u	*p = haystack;
    char_u	*తదుపరి;

    ఒకవేళ (p == ఖాళీ || needle == ఖాళీ || *needle == NUL)
	తిరిగి 0;

    size_t  needlelen = STRLEN(needle);
    ఒకవేళ (ic)
    {
	అయితే (*p != NUL)
	{
	    ఒకవేళ (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    లేకపోతే
		MB_PTR_ADV(p);
	}
    }
    లేకపోతే
	అయితే ((తదుపరి = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)p, (అక్షరం *)needle)) != ఖాళీ)
	{
	    ++n;
	    p = తదుపరి + needlelen;
	}

    తిరిగి n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    స్థిర పూర్ణాంకం
copy_మొదటి_char_గమ్యం_tv(char_u *ఆదానం, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    పూర్ణాంకం		len;

    ఒకవేళ (ఆదానం == ఖాళీ || output == ఖాళీ)
	తిరిగి -1;

    len = has_mbyte ? mb_ptr2len(ఆదానం) : 1;
    STRNCPY(buf, ఆదానం, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    తిరిగి output->vval.v_string == ఖాళీ ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    శూన్య
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*సమాసం,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		idx = 0;
    పూర్ణాంకం		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    // set_vim_var_nr() doesn't set the type
    సమితి_vim_var_type(VV_కీలకం, VAR_సంఖ్య);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_సమాసం_get_funccal(సమాసం, &newtv);

    ga_init2(&ga, పరిమాణం(అక్షరం), 80);
    కొరకు (p = str; *p != NUL; p += len)
    {
	len = copy_మొదటి_char_గమ్యం_tv(p, &tv);
	ఒకవేళ (len < 0)
	    ఆపు;

	సమితి_vim_var_nr(VV_కీలకం, idx);
	ఒకవేళ (filter_map_one(&tv, సమాసం, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    ఆపు;
	}
	ఒకవేళ (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    ఒకవేళ (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		ఆపు;
	    }
	    లేకపోతే
		ga_concat(&ga, newtv.vval.v_string);
	}
	లేకపోతే ఒకవేళ (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_దత్తాంశం;
    ఒకవేళ (fc != ఖాళీ)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    శూన్య
string_reduce(
	typval_T	*argvars,
	typval_T	*సమాసం,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    పూర్ణాంకం		len;
    typval_T	argv[3];
    పూర్ణాంకం		r;
    పూర్ణాంకం		called_emsg_ప్రారంభం = called_emsg;
    funccall_T	*fc;

    ఒకవేళ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ఒకవేళ (*p == NUL)
	{
	    semsg(_(e_reduce_యొక్క_an_ఖాళీలు_str_with_no_initial_విలువ), "String");
	    తిరిగి;
	}
	len = copy_మొదటి_char_గమ్యం_tv(p, rettv);
	ఒకవేళ (len < 0)
	    తిరిగి;
	p += len;
    }
    లేకపోతే ఒకవేళ (check_for_string_arg(argvars, 2) == FAIL)
	తిరిగి;
    లేకపోతే
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_సమాసం_get_funccal(సమాసం, rettv);

    కొరకు ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_మొదటి_char_గమ్యం_tv(p, &argv[1]);
	ఒకవేళ (len < 0)
	{
	    *rettv = argv[0];
	    ఆపు;
	}

	r = eval_సమాసం_typval(సమాసం, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	ఒకవేళ (r == FAIL || called_emsg != called_emsg_ప్రారంభం)
	    ఆపు;
    }

    ఒకవేళ (fc != ఖాళీ)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    స్థిర శూన్య
byteidx_common(typval_T *argvars, typval_T *rettv, పూర్ణాంకం comp)
{
    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	తిరిగి;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ);
    ఒకవేళ (str == ఖాళీ || idx < 0)
	తిరిగి;

    varnumber_T	utf16idx = FALSE;
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	పూర్ణాంకం error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	ఒకవేళ (error)
	    తిరిగి;
	ఒకవేళ (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_సంఖ్య_as_bool_nr), utf16idx);
	    తిరిగి;
	}
    }

    పూర్ణాంకం (*ptr2len)(char_u *);
    ఒకవేళ (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    లేకపోతే
	ptr2len = mb_ptr2len;

    char_u *t = str;
    కొరకు ( ; idx > 0; idx--)
    {
	ఒకవేళ (*t == NUL)		// EOL reached
	    తిరిగి;
	ఒకవేళ (utf16idx)
	{
	    పూర్ణాంకం clen = ptr2len(t);
	    పూర్ణాంకం c = (clen > 1) ? utf_ptr2char(t) : *t;
	    ఒకవేళ (c > 0xFFFF)
		idx--;
	    ఒకవేళ (idx > 0)
		t += clen;
	}
	లేకపోతే ఒకవేళ (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_సంఖ్య = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    శూన్య
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    శూన్య
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    శూన్య
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	తిరిగి;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ);
    ఒకవేళ (str == ఖాళీ || idx < 0)
	తిరిగి;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    పూర్ణాంకం (*ptr2len)(char_u *);
    ఒకవేళ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    లేకపోతే
	ptr2len = mb_ptr2len;

    char_u	*p;
    పూర్ణాంకం		len;
    కొరకు (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	ఒకవేళ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    ఒకవేళ (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_సంఖ్య = len;
	    తిరిగి;
	}
	ఒకవేళ (utf16idx)
	{
	    idx--;
	    పూర్ణాంకం clen = ptr2len(p);
	    పూర్ణాంకం c = (clen > 1) ? utf_ptr2char(p) : *p;
	    ఒకవేళ (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_సంఖ్య = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    స్థిర పూర్ణాంకం
convert_string(string_T *str, char_u *from, char_u *గమ్యం, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ఒకవేళ (convert_అమరిక(&vimconv, from, గమ్యం) == FAIL)
	తిరిగి FAIL;
    vimconv.vc_fail = TRUE;
    ఒకవేళ (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->నిడివి);
	ఒకవేళ (ret->string == ఖాళీ)
	    ret->నిడివి = 0;
	లేకపోతే
	    ret->నిడివి = str->నిడివి;
    }
    లేకపోతే
    {
	పూర్ణాంకం len = (పూర్ణాంకం)str->నిడివి;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->నిడివి = len;
    }
    convert_అమరిక(&vimconv, ఖాళీ, ఖాళీ);

    తిరిగి (ret->string == ఖాళీ) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    స్థిర శూన్య
blob_from_string(char_u *str, blob_T *blob)
{
    పూర్ణాంకం	    len = (పూర్ణాంకం)STRLEN(str);
    char_u  *dest;

    ఒకవేళ (len == 0)
	తిరిగి;
    ఒకవేళ (ga_grow(&blob->bv_ga, len) == FAIL)
	తిరిగి;
    dest = (char_u *)blob->bv_ga.ga_దత్తాంశం + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    కొరకు (పూర్ణాంకం i = 0; i < len; ++i)
	ఒకవేళ (dest[i] == NL)
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
    స్థిర పూర్ణాంకం
string_from_blob(blob_T *blob, దీర్ఘ *ప్రారంభం_idx, string_T *ret)
{
    దీర్ఘ	blen = blob_len(blob);
    దీర్ఘ	ప్రారంభం = *ప్రారంభం_idx;
    char_u	*src;
    char_u	*nl;
    దీర్ఘ	పంక్తి_len;

    ఒకవేళ (ప్రారంభం >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->నిడివి = 0;
	*ప్రారంభం_idx = blen;
	తిరిగి (ret->string == ఖాళీ) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_దత్తాంశం + ప్రారంభం;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - ప్రారంభం));
    పంక్తి_len = (nl == ఖాళీ) ? (blen - ప్రారంభం) : (దీర్ఘ)(nl - src);

    ret->string = alloc(పంక్తి_len + 1);
    ఒకవేళ (ret->string == ఖాళీ)
    {
	ret->నిడివి = 0;
	తిరిగి FAIL;
    }
    ఒకవేళ (పంక్తి_len > 0)
	mch_memmove(ret->string, src, (size_t)పంక్తి_len);
    ret->string[పంక్తి_len] = NUL;
    ret->నిడివి = (size_t)పంక్తి_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    కొరకు (దీర్ఘ i = 0; i < పంక్తి_len; i++)
	ఒకవేళ (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *ప్రారంభం_idx = ప్రారంభం + పంక్తి_len + (nl != ఖాళీ ? 1 : 0);
    తిరిగి OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    స్థిర char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    ఒకవేళ (from_encoding_raw == ఖాళీ)
	తిరిగి ఖాళీ;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    అయితే (*s != NUL)
    {
	ఒకవేళ (*s == '_')
	    *pe++ = '-';
	లేకపోతే
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    ఒకవేళ ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    తిరిగి from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    స్థిర శూన్య
append_converted_string_గమ్యం_list(
	string_T *converted,
	పూర్ణాంకం validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    ఒకవేళ (converted->string != ఖాళీ)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *చివర = converted->string + converted->నిడివి;
	అయితే (p < చివర)
	{
	    string_T	పంక్తి;
	    char_u	*పంక్తి_ప్రారంభం = p;

	    అయితే (p < చివర && *p != NL)
		p++;

	    // Add this line to the result list
	    పంక్తి.నిడివి = (size_t)(p - పంక్తి_ప్రారంభం);
	    పంక్తి.string = vim_strnsave(పంక్తి_ప్రారంభం, పంక్తి.నిడివి);
	    ఒకవేళ (పంక్తి.string != ఖాళీ)
	    {
		ఒకవేళ (validate_utf8 && !utf_valid_string(పంక్తి.string, ఖాళీ))
		{
		    vim_free(పంక్తి.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    తిరిగి; // Stop processing
		}
		ఒకవేళ (list_append_string(list, పంక్తి.string, (పూర్ణాంకం)పంక్తి.నిడివి) == FAIL)
		{
		    vim_free(పంక్తి.string);
		    vim_free(converted->string);
		    తిరిగి; // Stop processing on append failure
		}
		vim_free(పంక్తి.string);
	    }
	    లేకపోతే
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం));
		vim_free(converted->string);
		తిరిగి;
	    }

	    ఒకవేళ (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    లేకపోతే
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    స్థిర పూర్ణాంకం
append_validated_పంక్తి_గమ్యం_list(string_T *పంక్తి, పూర్ణాంకం validate_utf8, list_T *list)
{
    ఒకవేళ (validate_utf8 && !utf_valid_string(పంక్తి->string, ఖాళీ))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(పంక్తి->string);
	తిరిగి FAIL;
    }

    పూర్ణాంకం ret = list_append_string(list, పంక్తి->string, (పూర్ణాంకం)పంక్తి->నిడివి);
    vim_free(పంక్తి->string);
    తిరిగి ret;
}

    శూన్య
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    పూర్ణాంకం		blen;
    దీర్ఘ	idx;
    పూర్ణాంకం		validate_utf8 = FALSE;

    ఒకవేళ (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    blob = argvars->vval.v_blob;
    ఒకవేళ (blob == ఖాళీ)
	తిరిగి;
    blen = blob_len(blob);

    char_u	*from_encoding = ఖాళీ;
    char_u	*from_encoding_raw = ఖాళీ;  // Encoding name with endianness preserved for iconv
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ఒకవేళ (d != ఖాళీ)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ఒకవేళ (enc != ఖాళీ)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		ఒకవేళ (from_encoding_raw == ఖాళీ)
		{
		    emsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం));
		    VIM_CLEAR(from_encoding);
		    తిరిగి;
		}
	    }
	}
    }

    ఒకవేళ (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    ఒకవేళ (from_encoding != ఖాళీ && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    పూర్ణాంకం from_prop = 0;
    ఒకవేళ (from_encoding != ఖాళీ)
	from_prop = enc_canon_props(from_encoding);
    ఒకవేళ (from_encoding != ఖాళీ && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2_మాట)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	పూర్ణాంకం nul_కొలత = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_కొలత);
	ఒకవేళ (ga_grow(&blob_ga, blen + nul_కొలత) == OK)
	{
	    ఒకవేళ (blen > 0)
		mch_memmove(blob_ga.ga_దత్తాంశం, blob->bv_ga.ga_దత్తాంశం, (size_t)blen);
	    // NUL terminator (2 bytes for UTF-16/UCS-2, 4 bytes for UTF-32/UCS-4)
	    vim_memset((char_u *)blob_ga.ga_దత్తాంశం + blen, 0, (size_t)nul_కొలత);
	    blob_ga.ga_len = blen + nul_కొలత;
	}

	// Convert the entire blob at once
	vimconv_T vimconv;
	vimconv.vc_type = CONV_NONE;
	// Use raw encoding name for iconv to preserve endianness (utf-16be vs utf-16)
	// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
	ఒకవేళ (convert_అమరిక_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    వెళ్ళు పూర్తి;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	పూర్ణాంకం len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_దత్తాంశం, &len, ఖాళీ);
	converted.నిడివి = len;
	convert_అమరిక(&vimconv, ఖాళీ, ఖాళీ);
	ga_clear(&blob_ga);
	append_converted_string_గమ్యం_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    లేకపోతే
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	అయితే (idx < blen)
	{
	    string_T	str;

	    ఒకవేళ (string_from_blob(blob, &idx, &str) != OK)
		ఆపు;

	    ఒకవేళ (from_encoding != ఖాళీ)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		పూర్ణాంకం	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		ఒకవేళ (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    వెళ్ళు పూర్తి;
		}
		str.string = converted.string;
		str.నిడివి = converted.నిడివి;
	    }

	    ఒకవేళ (append_validated_పంక్తి_గమ్యం_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		వెళ్ళు పూర్తి;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    ఒకవేళ (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

పూర్తి:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    శూన్య
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (rettv_blob_alloc(rettv) == FAIL)
	తిరిగి;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    ఒకవేళ (list == ఖాళీ)
	తిరిగి;

    char_u	*గమ్యం_encoding = ఖాళీ;
    char_u	*గమ్యం_encoding_raw = ఖాళీ;  // Encoding name with endianness preserved for iconv
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ఒకవేళ (d != ఖాళీ)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ఒకవేళ (enc != ఖాళీ)
	    {
		char_u *enc_skipped = enc_skip(enc);
		గమ్యం_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		గమ్యం_encoding_raw = normalize_encoding_name(enc_skipped);
		ఒకవేళ (గమ్యం_encoding_raw == ఖాళీ)
		{
		    emsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం));
		    VIM_CLEAR(గమ్యం_encoding);
		    తిరిగి;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    పూర్ణాంకం గమ్యం_prop = 0;
    ఒకవేళ (గమ్యం_encoding != ఖాళీ)
	గమ్యం_prop = enc_canon_props(గమ్యం_encoding);
    ఒకవేళ (గమ్యం_encoding != ఖాళీ && (గమ్యం_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2_మాట)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_వస్తువులు(list, li)
	{
	    char_u *s;

	    ఒకవేళ (li->li_tv.v_type != VAR_STRING)
		కొనసాగు;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    ఒకవేళ (li != list->lv_మొదటి)
		ga_append(&str_ga, NL);
	    ఒకవేళ (s != ఖాళీ && *s != NUL)
	    {
		పూర్ణాంకం slen = (పూర్ణాంకం)STRLEN(s);

		ఒకవేళ (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    వెళ్ళు పూర్తి;
		}
		mch_memmove((char_u *)str_ga.ga_దత్తాంశం + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	ఒకవేళ (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    ఒకవేళ (convert_అమరిక_ext(&vimconv, p_enc, FALSE, గమ్యం_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_గమ్యం_failed), గమ్యం_encoding);
		వెళ్ళు పూర్తి;
	    }
	    vimconv.vc_fail = TRUE;

	    పూర్ణాంకం		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_దత్తాంశం, &len, ఖాళీ);
	    convert_అమరిక(&vimconv, ఖాళీ, ఖాళీ);
	    ga_clear(&str_ga);

	    ఒకవేళ (converted == ఖాళీ)
	    {
		semsg(_(e_str_encoding_గమ్యం_failed), గమ్యం_encoding);
		వెళ్ళు పూర్తి;
	    }
	    ఒకవేళ (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_దత్తాంశం + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	లేకపోతే
	    ga_clear(&str_ga);
    }
    లేకపోతే
    {
	FOR_ALL_LIST_వస్తువులు(list, li)
	{
	    ఒకవేళ (li->li_tv.v_type != VAR_STRING)
		కొనసాగు;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    ఒకవేళ (str.string == ఖాళీ)
		STR_LITERAL_సమితి(str, "");
	    లేకపోతే
		str.నిడివి = STRLEN(str.string);

	    ఒకవేళ (గమ్యం_encoding != ఖాళీ)
	    {
		పూర్ణాంకం	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, గమ్యం_encoding, &converted);
		ఒకవేళ (res != OK)
		{
		    semsg(_(e_str_encoding_గమ్యం_failed), గమ్యం_encoding);
		    వెళ్ళు పూర్తి;
		}
		str.string = converted.string;
		str.నిడివి = converted.నిడివి;
	    }

	    ఒకవేళ (li != list->lv_మొదటి)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    ఒకవేళ (గమ్యం_encoding != ఖాళీ)
		vim_free(str.string);
	}
    }

పూర్తి:
    ఒకవేళ (గమ్యం_encoding != ఖాళీ)
	vim_free(గమ్యం_encoding);
    ఒకవేళ (గమ్యం_encoding_raw != ఖాళీ)
	vim_free(గమ్యం_encoding_raw);
}

/*
 * "str2list()" function
 */
    శూన్య
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    పూర్ణాంకం		utf8 = FALSE;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (పూర్ణాంకం)tv_get_bool_chk(&argvars[1], ఖాళీ);

    p = tv_get_string(&argvars[0]);

    ఒకవేళ (has_mbyte || utf8)
    {
	పూర్ణాంకం (*ptr2len)(char_u *);
	పూర్ణాంకం (*ptr2char)(char_u *);

	ఒకవేళ (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	లేకపోతే
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	కొరకు ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_సంఖ్య(rettv->vval.v_list, (*ptr2char)(p));
    }
    లేకపోతే
	కొరకు ( ; *p != NUL; ++p)
	    list_append_సంఖ్య(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    శూన్య
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		పునాది = 10;
    char_u	*p;
    varnumber_T	n;
    పూర్ణాంకం		what = 0;
    పూర్ణాంకం		isneg;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	పునాది = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[1]);
	ఒకవేళ (పునాది != 2 && పునాది != 8 && పునాది != 10 && పునాది != 16)
	{
	    emsg(_(e_invalid_వాదం));
	    తిరిగి;
	}
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    ఒకవేళ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    ఎంపిక (పునాది)
    {
	స్థితి 2: what |= STR2NR_BIN + STR2NR_FORCE; ఆపు;
	స్థితి 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; ఆపు;
	స్థితి 16: what |= STR2NR_HEX + STR2NR_FORCE; ఆపు;
    }
    vim_str2nr(p, ఖాళీ, ఖాళీ, what, &n, ఖాళీ, 0, FALSE, ఖాళీ);
    // Text after the number is silently ignored.
    ఒకవేళ (isneg)
	rettv->vval.v_సంఖ్య = -n;
    లేకపోతే
	rettv->vval.v_సంఖ్య = n;

}

/*
 * "strgetchar()" function
 */
    శూన్య
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    పూర్ణాంకం		len;
    పూర్ణాంకం		error = FALSE;
    పూర్ణాంకం		charidx;
    పూర్ణాంకం		byteidx = 0;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    str = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (str == ఖాళీ)
	తిరిగి;
    len = (పూర్ణాంకం)STRLEN(str);
    charidx = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[1], &error);
    ఒకవేళ (error)
	తిరిగి;

    అయితే (charidx >= 0 && byteidx < len)
    {
	ఒకవేళ (charidx == 0)
	{
	    rettv->vval.v_సంఖ్య = mb_ptr2char(str + byteidx);
	    ఆపు;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    శూన్య
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    పూర్ణాంకం		ప్రారంభం_idx;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL))
	తిరిగి;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (needle == ఖాళీ || haystack == ఖాళీ)
	తిరిగి;		// type error; errmsg already given

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	పూర్ణాంకం	    error = FALSE;

	ప్రారంభం_idx = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[2], &error);
	ఒకవేళ (error || ప్రారంభం_idx >= (పూర్ణాంకం)STRLEN(haystack))
	    తిరిగి;
	ఒకవేళ (ప్రారంభం_idx >= 0)
	    haystack += ప్రారంభం_idx;
    }

    pos	= (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)haystack, (అక్షరం *)needle);
    ఒకవేళ (pos != ఖాళీ)
	rettv->vval.v_సంఖ్య = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    శూన్య
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    ఒకవేళ (rettv->vval.v_string != ఖాళీ && tofree == ఖాళీ)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    శూన్య
f_strlen(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && check_for_string_or_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    స్థిర శూన్య
strchar_common(typval_T *argvars, typval_T *rettv, పూర్ణాంకం skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    పూర్ణాంకం			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    అయితే (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_సంఖ్య = len;
}

/*
 * "strcharlen()" function
 */
    శూన్య
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && check_for_string_or_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    శూన్య
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	పూర్ణాంకం error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	ఒకవేళ (error)
	    తిరిగి;
	ఒకవేళ (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_సంఖ్య_as_bool_nr), skipcc);
	    తిరిగి;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    శూన్య
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	తిరిగి;

    varnumber_T countcc = FALSE;
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    పూర్ణాంకం			(*func_mb_ptr2char_adv)(char_u **pp);
    పూర్ణాంకం			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    అయితే (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	ఒకవేళ (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_సంఖ్య = len;
}

/*
 * "strdisplaywidth()" function
 */
    శూన్య
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    పూర్ణాంకం		నిలువు = 0;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    s = tv_get_string(&argvars[0]);
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	నిలువు = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[1]);

    rettv->vval.v_సంఖ్య = (varnumber_T)(linetabsize_నిలువు(నిలువు, s) - నిలువు);
}

/*
 * "strwidth()" function
 */
    శూన్య
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_సంఖ్య = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    శూన్య
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    పూర్ణాంకం		nchar;
    పూర్ణాంకం		nbyte = 0;
    పూర్ణాంకం		charlen;
    పూర్ణాంకం		skipcc = FALSE;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		slen;
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	తిరిగి;

    p = tv_get_string(&argvars[0]);
    slen = (పూర్ణాంకం)STRLEN(p);

    nchar = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[1], &error);
    ఒకవేళ (!error)
    {
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    ఒకవేళ (error)
		తిరిగి;
	    ఒకవేళ (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_సంఖ్య_as_bool_nr), skipcc);
		తిరిగి;
	    }
	}

	ఒకవేళ (nchar > 0)
	    అయితే (nchar > 0 && nbyte < slen)
	    {
		ఒకవేళ (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		లేకపోతే
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	లేకపోతే
	    nbyte = nchar;
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[2]);
	    అయితే (charlen > 0 && nbyte + len < slen)
	    {
		పూర్ణాంకం off = nbyte + len;

		ఒకవేళ (off < 0)
		    len += 1;
		లేకపోతే
		{
		    ఒకవేళ (skipcc)
			len += mb_ptr2len(p + off);
		    లేకపోతే
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	లేకపోతే
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    ఒకవేళ (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    లేకపోతే ఒకవేళ (nbyte > slen)
	nbyte = slen;
    ఒకవేళ (len < 0)
	len = 0;
    లేకపోతే ఒకవేళ (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    శూన్య
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    పూర్ణాంకం		n;
    పూర్ణాంకం		len;
    పూర్ణాంకం		slen;
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	తిరిగి;

    p = tv_get_string(&argvars[0]);
    slen = (పూర్ణాంకం)STRLEN(p);

    n = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[1], &error);
    ఒకవేళ (error)
	len = 0;
    లేకపోతే ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	len = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[2]);
    లేకపోతే
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    ఒకవేళ (n < 0)
    {
	len += n;
	n = 0;
    }
    లేకపోతే ఒకవేళ (n > slen)
	n = slen;
    ఒకవేళ (len < 0)
	len = 0;
    లేకపోతే ఒకవేళ (n + len > slen)
	len = slen - n;

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	పూర్ణాంకం off;

	// length in characters
	కొరకు (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    శూన్య
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = ఖాళీ;
    పూర్ణాంకం		haystack_len, చివర_idx;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL))
	తిరిగి;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (needle == ఖాళీ || haystack == ఖాళీ)
	తిరిగి;		// type error; errmsg already given

    haystack_len = (పూర్ణాంకం)STRLEN(haystack);
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	చివర_idx = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[2], ఖాళీ);
	ఒకవేళ (చివర_idx < 0)
	    తిరిగి;	// can never find a match
    }
    లేకపోతే
	చివర_idx = haystack_len;

    ఒకవేళ (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + చివర_idx;
    }
    లేకపోతే
    {
	కొరకు (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)rest, (అక్షరం *)needle);
	    ఒకవేళ (rest == ఖాళీ || rest > haystack + చివర_idx)
		ఆపు;
	    lastmatch = rest;
	}
    }

    ఒకవేళ (lastmatch == ఖాళీ)
	rettv->vval.v_సంఖ్య = -1;
    లేకపోతే
	rettv->vval.v_సంఖ్య = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    శూన్య
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    శూన్య
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    తిరిగి;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_సంఖ్య_chk(&argvars[1], ఖాళీ);
    ఒకవేళ (str == ఖాళీ || idx < 0)
	తిరిగి;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    పూర్ణాంకం (*ptr2len)(char_u *);
    ఒకవేళ (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    లేకపోతే
	ptr2len = mb_ptr2len;

    char_u	*p;
    పూర్ణాంకం		len;
    పూర్ణాంకం		utf16idx = 0;
    కొరకు (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	ఒకవేళ (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    ఒకవేళ (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_సంఖ్య = len;
	    తిరిగి;
	}
	utf16idx = len;
	పూర్ణాంకం clen = ptr2len(p);
	పూర్ణాంకం c = (clen > 1) ? utf_ptr2char(p) : *p;
	ఒకవేళ (c > 0xFFFF)
	    len++;
	p += clen;
	ఒకవేళ (charidx)
	    idx--;
    }

    rettv->vval.v_సంఖ్య = utf16idx;
}

/*
 * "tolower(string)" function
 */
    శూన్య
f_tolower(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    శూన్య
f_toupper(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    శూన్య
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*లోపల_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    పూర్ణాంకం		inlen;
    పూర్ణాంకం		fromlen;
    పూర్ణాంకం		tolen;
    పూర్ణాంకం		idx;
    char_u	*cpstr;
    పూర్ణాంకం		cplen;
    పూర్ణాంకం		మొదటి = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	తిరిగి;

    లోపల_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;
    ఒకవేళ (fromstr == ఖాళీ || tostr == ఖాళీ)
	    తిరిగి;		// type error; errmsg already given
    ga_init2(&ga, పరిమాణం(అక్షరం), 80);

    ఒకవేళ (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	ఒకవేళ (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_వాదం_str), fromstr);
	    ga_clear(&ga);
	    తిరిగి;
	}

    // fromstr and tostr have to contain the same number of chars
    అయితే (*లోపల_str != NUL)
    {
	ఒకవేళ (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(లోపల_str);
	    cpstr = లోపల_str;
	    cplen = inlen;
	    idx = 0;
	    కొరకు (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		ఒకవేళ (fromlen == inlen && STRNCMP(లోపల_str, p, inlen) == 0)
		{
		    కొరకు (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			ఒకవేళ (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    ఆపు;
			}
		    }
		    ఒకవేళ (*p == NUL)	// tostr is shorter than fromstr
			వెళ్ళు error;
		    ఆపు;
		}
		++idx;
	    }

	    ఒకవేళ (మొదటి && cpstr == లోపల_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		మొదటి = FALSE;
		కొరకు (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		ఒకవేళ (idx != 0)
		    వెళ్ళు error;
	    }

	    (శూన్య)ga_grow(&ga, cplen);
	    mch_memmove((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    లోపల_str += inlen;
	}
	లేకపోతే
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *లోపల_str);
	    ఒకవేళ (p != ఖాళీ)
		ga_append(&ga, tostr[p - fromstr]);
	    లేకపోతే
		ga_append(&ga, *లోపల_str);
	    ++లోపల_str;
	}
    }

    // add a terminating NUL
    (శూన్య)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_దత్తాంశం;
}

/*
 * "trim({expr})" function
 */
    శూన్య
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*తల;
    char_u	*ముసుగు = ఖాళీ;
    char_u	*తోక;
    char_u	*prev;
    char_u	*p;
    పూర్ణాంకం		c1;
    పూర్ణాంకం		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL)))
	తిరిగి;

    తల = tv_get_string_buf_chk(&argvars[0], buf1);
    ఒకవేళ (తల == ఖాళీ)
	తిరిగి;

    ఒకవేళ (check_for_opt_string_arg(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type == VAR_STRING)
    {
	ముసుగు = tv_get_string_buf_chk(&argvars[1], buf2);
	ఒకవేళ (*ముసుగు == NUL)
	    ముసుగు = ఖాళీ;

	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    పూర్ణాంకం	error = 0;

	    // leading or trailing characters to trim
	    dir = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[2], &error);
	    ఒకవేళ (error)
		తిరిగి;
	    ఒకవేళ (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[2]));
		తిరిగి;
	    }
	}
    }

    ఒకవేళ (dir == 0 || dir == 1)
    {
	// Trim leading characters
	అయితే (*తల != NUL)
	{
	    c1 = PTR2CHAR(తల);
	    ఒకవేళ (ముసుగు == ఖాళీ)
	    {
		ఒకవేళ (c1 > ' ' && c1 != 0xa0)
		    ఆపు;
	    }
	    లేకపోతే
	    {
		కొరకు (p = ముసుగు; *p != NUL; MB_PTR_ADV(p))
		    ఒకవేళ (c1 == PTR2CHAR(p))
			ఆపు;
		ఒకవేళ (*p == NUL)
		    ఆపు;
	    }
	    MB_PTR_ADV(తల);
	}
    }

    తోక = తల + STRLEN(తల);
    ఒకవేళ (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	కొరకు (; తోక > తల; తోక = prev)
	{
	    prev = తోక;
	    MB_PTR_వెనుక(తల, prev);
	    c1 = PTR2CHAR(prev);
	    ఒకవేళ (ముసుగు == ఖాళీ)
	    {
		ఒకవేళ (c1 > ' ' && c1 != 0xa0)
		    ఆపు;
	    }
	    లేకపోతే
	    {
		కొరకు (p = ముసుగు; *p != NUL; MB_PTR_ADV(p))
		    ఒకవేళ (c1 == PTR2CHAR(p))
			ఆపు;
		ఒకవేళ (*p == NUL)
		    ఆపు;
	    }
	}
    }
    rettv->vval.v_string = vim_strnsave(తల, తోక - తల);
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
    స్థిర char_u *
uri_decode(char_u *str)
{
    ఒకవేళ (str == ఖాళీ)
	తిరిగి ఖాళీ;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    ఒకవేళ (!decoded)
	తిరిగి ఖాళీ;

    char_u	*p = decoded;
    size_t	i = 0;

    అయితే (i < len)
    {
	ఒకవేళ (str[i] == '%')
	{
	    ఒకవేళ (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		ఒకవేళ (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    లేకపోతే
	    {
		పూర్ణాంకం val = hexhex2nr(&str[i + 1]);
		ఒకవేళ (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		లేకపోతే
		{
		    // invalid hex digits following "%"
		    కొరకు (పూర్ణాంకం j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	లేకపోతే
	    *p++ = str[i++];
    }

    *p = NUL;

    తిరిగి decoded;
}

/*
 * "uri_decode({str})" function
 */
    శూన్య
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

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
    స్థిర char_u *
uri_encode(char_u *str)
{
    ఒకవేళ (str == ఖాళీ)
	తిరిగి ఖాళీ;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    ఒకవేళ (encoded == ఖాళీ)
	తిరిగి ఖాళీ;

    char_u *p = encoded;

    కొరకు (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	ఒకవేళ (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	లేకపోతే
	{
	    sprintf((అక్షరం *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    తిరిగి encoded;
}

/*
 * "uri_encode({str})" function
 */
    శూన్య
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

స్థిర అక్షరం *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    స్థిర varnumber_T
tv_nr(typval_T *tvs, పూర్ణాంకం *idxp)
{
    పూర్ణాంకం		idx = *idxp - 1;
    varnumber_T	n = 0;
    పూర్ణాంకం		err = FALSE;

    ఒకవేళ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    లేకపోతే
    {
	++*idxp;
	n = tv_get_సంఖ్య_chk(&tvs[idx], &err);
	ఒకవేళ (err)
	    n = 0;
    }
    తిరిగి n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    స్థిర అక్షరం *
tv_str(typval_T *tvs, పూర్ణాంకం *idxp, char_u **tofree)
{
    పూర్ణాంకం		    idx = *idxp - 1;
    అక్షరం	    *s = ఖాళీ;
    స్థిర char_u   numbuf[NUMBUFLEN];

    ఒకవేళ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    లేకపోతే
    {
	++*idxp;
	ఒకవేళ (tofree != ఖాళీ)
	    s = (అక్షరం *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	లేకపోతే
	    s = (అక్షరం *)tv_get_string_chk(&tvs[idx]);
    }
    తిరిగి s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    స్థిర ద్విగుణ
tv_float(typval_T *tvs, పూర్ణాంకం *idxp)
{
    పూర్ణాంకం		idx = *idxp - 1;
    ద్విగుణ	f = 0;

    ఒకవేళ (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    లేకపోతే
    {
	++*idxp;
	ఒకవేళ (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	లేకపోతే ఒకవేళ (tvs[idx].v_type == VAR_సంఖ్య)
	    f = (ద్విగుణ)tvs[idx].vval.v_సంఖ్య;
	లేకపోతే
	    emsg(_(e_expected_float_వాదం_for_printf));
    }
    తిరిగి f;
}

#అయితే_ముగింపు

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    స్థిర స్థిరరాశి అక్షరం *
infinity_str(పూర్ణాంకం ధనాత్మక,
	     అక్షరం fmt_spec,
	     పూర్ణాంకం force_sign,
	     పూర్ణాంకం space_for_ధనాత్మక)
{
    స్థిర స్థిరరాశి అక్షరం *పట్టిక[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    పూర్ణాంకం idx = ధనాత్మక * (1 + force_sign + force_sign * space_for_ధనాత్మక);

    ఒకవేళ (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    తిరిగి పట్టిక[idx];
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
#నిర్వచితం_కాకపోతే PROTO

// Like vim_vsnprintf() but append to the string.
    పూర్ణాంకం
vim_snprintf_కూడు(అక్షరం *str, size_t str_m, స్థిరరాశి అక్షరం *fmt, ...)
{
    va_list	ap;
    పూర్ణాంకం		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    ఒకవేళ (str_m <= len)
	space = 0;
    లేకపోతే
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    తిరిగి str_l;
}

    పూర్ణాంకం
vim_snprintf(అక్షరం *str, size_t str_m, స్థిరరాశి అక్షరం *fmt, ...)
{
    va_list	ap;
    పూర్ణాంకం		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    తిరిగి str_l;
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
vim_snprintf_safelen(అక్షరం *str, size_t str_m, స్థిరరాశి అక్షరం *fmt, ...)
{
    va_list ap;
    పూర్ణాంకం	    str_l;

    ఒకవేళ (str_m == 0)
	తిరిగి 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, ఖాళీ);
    va_end(ap);

    ఒకవేళ (str_l < 0)
    {
	*str = NUL;
	తిరిగి 0;
    }
    తిరిగి ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    పూర్ణాంకం
vim_vsnprintf(
    అక్షరం	*str,
    size_t	str_m,
    స్థిరరాశి అక్షరం	*fmt,
    va_list	ap)
{
    తిరిగి vim_vsnprintf_typval(str, str_m, fmt, ap, ఖాళీ);
}

గణన
{
    TYPE_UNKNOWN = -1,
    TYPE_INT,
    TYPE_LONGINT,
    TYPE_LONGLONGINT,
    TYPE_UNSIGNEDINT,
    TYPE_UNSIGNEDLONGINT,
    TYPE_UNSIGNEDLONGLONGINT,
    TYPE_సూచిక,
    TYPE_PERCENT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_FLOAT
};

/* Types that can be used in a format string
 */
    స్థిర పూర్ణాంకం
format_typeof(
    స్థిరరాశి అక్షరం	*type)
{
    // allowed values: \0, h, l, L
    అక్షరం    నిడివి_modifier = '\0';

    // current conversion specifier character
    అక్షరం    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    ఒకవేళ (*type == 'h' || *type == 'l')
    {
	నిడివి_modifier = *type;
	type++;
	ఒకవేళ (నిడివి_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    నిడివి_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    ఎంపిక (fmt_spec)
    {
	స్థితి 'i': fmt_spec = 'd'; ఆపు;
	స్థితి '*': fmt_spec = 'd'; నిడివి_modifier = 'h'; ఆపు;
	స్థితి 'D': fmt_spec = 'd'; నిడివి_modifier = 'l'; ఆపు;
	స్థితి 'U': fmt_spec = 'u'; నిడివి_modifier = 'l'; ఆపు;
	స్థితి 'O': fmt_spec = 'o'; నిడివి_modifier = 'l'; ఆపు;
	అప్రమేయ: ఆపు;
    }

    // get parameter value, do initial processing
    ఎంపిక (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    స్థితి '%':
	తిరిగి TYPE_PERCENT;

    స్థితి 'c':
	తిరిగి TYPE_CHAR;

    స్థితి 's':
    స్థితి 'S':
	తిరిగి TYPE_STRING;

    స్థితి 'd': స్థితి 'u':
    స్థితి 'b': స్థితి 'B':
    స్థితి 'o':
    స్థితి 'x': స్థితి 'X':
    స్థితి 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //విలువ

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    ఒకవేళ (fmt_spec == 'p')
		తిరిగి TYPE_సూచిక;
	    లేకపోతే ఒకవేళ (fmt_spec == 'b' || fmt_spec == 'B')
		తిరిగి TYPE_UNSIGNEDLONGLONGINT;
	    లేకపోతే ఒకవేళ (fmt_spec == 'd')
	    {
		// signed
		ఎంపిక (నిడివి_modifier)
		{
		స్థితి '\0':
		స్థితి 'h':
		    // char and short arguments are passed as int.
		    తిరిగి TYPE_INT;
		స్థితి 'l':
		    తిరిగి TYPE_LONGINT;
		స్థితి 'L':
		    తిరిగి TYPE_LONGLONGINT;
		}
	    }
	    లేకపోతే
	    {
		// unsigned
		ఎంపిక (నిడివి_modifier)
		{
		    స్థితి '\0':
		    స్థితి 'h':
			తిరిగి TYPE_UNSIGNEDINT;
		    స్థితి 'l':
			తిరిగి TYPE_UNSIGNEDLONGINT;
		    స్థితి 'L':
			తిరిగి TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	ఆపు;

    స్థితి 'f':
    స్థితి 'F':
    స్థితి 'e':
    స్థితి 'E':
    స్థితి 'g':
    స్థితి 'G':
	తిరిగి TYPE_FLOAT;
    }

    తిరిగి TYPE_UNKNOWN;
}

    స్థిర అక్షరం *
format_typename(
    స్థిరరాశి అక్షరం  *type)
{
    ఎంపిక (format_typeof(type))
    {
	స్థితి TYPE_INT:
	    తిరిగి typename_int;

	స్థితి TYPE_LONGINT:
	    తిరిగి typename_longint;

	స్థితి TYPE_LONGLONGINT:
	    తిరిగి typename_longlongint;

	స్థితి TYPE_UNSIGNEDINT:
	    తిరిగి typename_unsignedint;

	స్థితి TYPE_UNSIGNEDLONGINT:
	    తిరిగి typename_unsignedlongint;

	స్థితి TYPE_UNSIGNEDLONGLONGINT:
	    తిరిగి typename_unsignedlonglongint;

	స్థితి TYPE_సూచిక:
	    తిరిగి _(typename_సూచిక);

	స్థితి TYPE_PERCENT:
	    తిరిగి _(typename_percent);

	స్థితి TYPE_CHAR:
	    తిరిగి typename_char;

	స్థితి TYPE_STRING:
	    తిరిగి _(typename_string);

	స్థితి TYPE_FLOAT:
	    తిరిగి typename_float;
    }

    తిరిగి _(typename_unknown);
}

    స్థిర పూర్ణాంకం
adjust_types(
    స్థిరరాశి అక్షరం ***ap_types,
    పూర్ణాంకం arg,
    పూర్ణాంకం *అంకె_posarg,
    స్థిరరాశి అక్షరం *type)
{
    ఒకవేళ (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	తిరిగి FAIL;
    }

    ఒకవేళ (*ap_types == ఖాళీ || *అంకె_posarg < arg)
    {
	పూర్ణాంకం	    idx;
	స్థిరరాశి అక్షరం  **new_types;

	ఒకవేళ (*ap_types == ఖాళీ)
	    new_types = ALLOC_CLEAR_MULT(స్థిరరాశి అక్షరం *, arg);
	లేకపోతే
	    new_types = vim_realloc((అక్షరం **)*ap_types,
						arg * పరిమాణం(స్థిరరాశి అక్షరం *));

	ఒకవేళ (new_types == ఖాళీ)
	    తిరిగి FAIL;

	కొరకు (idx = *అంకె_posarg; idx < arg; ++idx)
	    new_types[idx] = ఖాళీ;

	*ap_types = new_types;
	*అంకె_posarg = arg;
    }

    ఒకవేళ ((*ap_types)[arg - 1] != ఖాళీ)
    {
	ఒకవేళ ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    స్థిరరాశి అక్షరం *pt = type;
	    ఒకవేళ (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    ఒకవేళ (pt[0] != '*')
	    {
		ఎంపిక (pt[0])
		{
		    స్థితి 'd':
		    స్థితి 'i': ఆపు;
		    అప్రమేయ:
			semsg(_(e_positional_అంకె_క్షేత్రం_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			తిరిగి FAIL;
		}
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_అంకె_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		తిరిగి FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    తిరిగి OK;
}

    స్థిర శూన్య
format_అధికప్రవాహం_error(స్థిరరాశి అక్షరం *pstart)
{
    size_t	arglen = 0;
    అక్షరం	*argcopy = ఖాళీ;
    స్థిరరాశి అక్షరం	*p = pstart;

    అయితే (VIM_ISDIGIT((పూర్ణాంకం)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(అక్షరం, arglen + 1);
    ఒకవేళ (argcopy != ఖాళీ)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	విడుదల(argcopy);
    }
    లేకపోతే
	semsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం_allocating_nr_బైట్లు), arglen);
}

# define MAX_ALLOWED_STRING_వెడల్పు 1048576    // 1 MiB

    స్థిర పూర్ణాంకం
get_unsigned_int(
    స్థిరరాశి అక్షరం *pstart,
    స్థిరరాశి అక్షరం **p,
    గుర్తులేని పూర్ణాంకం *uj,
    పూర్ణాంకం అధికప్రవాహం_err)
{
    *uj = **p - '0';
    ++*p;

    అయితే (VIM_ISDIGIT((పూర్ణాంకం)(**p)) && *uj < MAX_ALLOWED_STRING_వెడల్పు)
    {
	*uj = 10 * *uj + (గుర్తులేని పూర్ణాంకం)(**p - '0');
	++*p;
    }

    ఒకవేళ (*uj > MAX_ALLOWED_STRING_వెడల్పు)
    {
	ఒకవేళ (అధికప్రవాహం_err)
	{
	    format_అధికప్రవాహం_error(pstart);
	    తిరిగి FAIL;
	}
	లేకపోతే
	    *uj = MAX_ALLOWED_STRING_వెడల్పు;
    }

    తిరిగి OK;
}


    స్థిర పూర్ణాంకం
parse_fmt_types(
    స్థిరరాశి అక్షరం  ***ap_types,
    పూర్ణాంకం		*అంకె_posarg,
    స్థిరరాశి అక్షరం  *fmt,
    typval_T	*tvs వాడని
    )
{
    స్థిరరాశి అక్షరం	*p = fmt;
    స్థిరరాశి అక్షరం	*arg = ఖాళీ;

    పూర్ణాంకం		any_pos = 0;
    పూర్ణాంకం		any_arg = 0;
    పూర్ణాంకం		arg_idx;

# define CHECK_POS_ARG చేయి { \
    ఒకవేళ (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	వెళ్ళు error; \
    } \
} అయితే (0);

    ఒకవేళ (p == ఖాళీ)
	తిరిగి OK;

    అయితే (*p != NUL)
    {
	ఒకవేళ (*p != '%')
	{
	    స్థిరరాశి అక్షరం    *q = strchr(p + 1, '%');
	    size_t  n = (q == ఖాళీ) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	లేకపోతే
	{
	    // allowed values: \0, h, l, L
	    అక్షరం	నిడివి_modifier = '\0';

	    // variable for positional arg
	    పూర్ణాంకం		pos_arg = -1;
	    స్థిరరాశి అక్షరం	*ptype = ఖాళీ;
	    స్థిరరాశి అక్షరం	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    అయితే (VIM_ISDIGIT(*ptype))
		++ptype;

	    ఒకవేళ (*ptype == '$')
	    {
		ఒకవేళ (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    వెళ్ళు error;
		}

		// Positional argument
		గుర్తులేని పూర్ణాంకం uj;

		ఒకవేళ (get_unsigned_int(pstart, &p, &uj, tvs != ఖాళీ) == FAIL)
		    వెళ్ళు error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    అయితే (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		ఎంపిక (*p)
		{
		    స్థితి '0': ఆపు;
		    స్థితి '-': ఆపు;
		    స్థితి '+': ఆపు;
		    స్థితి ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      ఆపు;
		    స్థితి '#': ఆపు;
		    స్థితి '\'': ఆపు;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    ఒకవేళ (*(arg = p) == '*')
	    {
		p++;

		ఒకవేళ (VIM_ISDIGIT((పూర్ణాంకం)(*p)))
		{
		    // Positional argument field width
		    గుర్తులేని పూర్ణాంకం uj;

		    ఒకవేళ (get_unsigned_int(arg + 1, &p, &uj, tvs != ఖాళీ) == FAIL)
			వెళ్ళు error;

		    ఒకవేళ (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			వెళ్ళు error;
		    }
		    లేకపోతే
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			ఒకవేళ (adjust_types(ap_types, uj, అంకె_posarg, arg) == FAIL)
			    వెళ్ళు error;
		    }
		}
		లేకపోతే
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    లేకపోతే ఒకవేళ (VIM_ISDIGIT((పూర్ణాంకం)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		స్థిరరాశి అక్షరం *digstart = p;
		గుర్తులేని పూర్ణాంకం uj;

		ఒకవేళ (get_unsigned_int(digstart, &p, &uj, tvs != ఖాళీ) == FAIL)
		    వెళ్ళు error;

		ఒకవేళ (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    వెళ్ళు error;
		}
	    }

	    // parse precision
	    ఒకవేళ (*p == '.')
	    {
		p++;

		ఒకవేళ (*(arg = p) == '*')
		{
		    p++;

		    ఒకవేళ (VIM_ISDIGIT((పూర్ణాంకం)(*p)))
		    {
			// Parse precision
			గుర్తులేని పూర్ణాంకం uj;

			ఒకవేళ (get_unsigned_int(arg + 1, &p, &uj, tvs != ఖాళీ) == FAIL)
			    వెళ్ళు error;

			ఒకవేళ (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    ఒకవేళ (adjust_types(ap_types, uj, అంకె_posarg, arg) == FAIL)
				వెళ్ళు error;
			}
			లేకపోతే
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    వెళ్ళు error;
			}
		    }
		    లేకపోతే
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		లేకపోతే ఒకవేళ (VIM_ISDIGIT((పూర్ణాంకం)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    స్థిరరాశి అక్షరం *digstart = p;
		    గుర్తులేని పూర్ణాంకం uj;

		    ఒకవేళ (get_unsigned_int(digstart, &p, &uj, tvs != ఖాళీ) == FAIL)
			వెళ్ళు error;

		    ఒకవేళ (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			వెళ్ళు error;
		    }
		}
	    }

	    ఒకవేళ (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ఒకవేళ (*p == 'h' || *p == 'l')
	    {
		నిడివి_modifier = *p;
		p++;
		ఒకవేళ (నిడివి_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    ఎంపిక (*p)
	    {
		// Check for known format specifiers. % is special!
		స్థితి 'i':
		స్థితి '*':
		స్థితి 'd':
		స్థితి 'u':
		స్థితి 'o':
		స్థితి 'D':
		స్థితి 'U':
		స్థితి 'O':
		స్థితి 'x':
		స్థితి 'X':
		స్థితి 'b':
		స్థితి 'B':
		స్థితి 'c':
		స్థితి 's':
		స్థితి 'S':
		స్థితి 'p':
		స్థితి 'f':
		స్థితి 'F':
		స్థితి 'e':
		స్థితి 'E':
		స్థితి 'g':
		స్థితి 'G':
		    ఒకవేళ (pos_arg != -1)
		    {
			ఒకవేళ (adjust_types(ap_types, pos_arg, అంకె_posarg, ptype) == FAIL)
			    వెళ్ళు error;
		    }
		    లేకపోతే
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    ఆపు;

		అప్రమేయ:
		    ఒకవేళ (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			వెళ్ళు error;
		    }
	    }

	    ఒకవేళ (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    కొరకు (arg_idx = 0; arg_idx < *అంకె_posarg; ++arg_idx)
    {
	ఒకవేళ ((*ap_types)[arg_idx] == ఖాళీ)
	{
	    semsg(_(e_fmt_arg_nr_వాడని_str), arg_idx + 1, fmt);
	    వెళ్ళు error;
	}

# ఒకవేళ defined(FEAT_EVAL)
	ఒకవేళ (tvs != ఖాళీ && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_బహిర్గతం_యొక్క_bounds_str), arg_idx + 1, fmt);
	    వెళ్ళు error;
	}
# endif
    }

    తిరిగి OK;

error:
    vim_free((అక్షరం**)*ap_types);
    *ap_types = ఖాళీ;
    *అంకె_posarg = 0;
    తిరిగి FAIL;
}

    స్థిర శూన్య
skip_గమ్యం_arg(
    స్థిరరాశి అక్షరం	**ap_types,
    va_list	ap_ప్రారంభం,
    va_list	*ap,
    పూర్ణాంకం		*arg_idx,
    పూర్ణాంకం		*arg_cur,
    స్థిరరాశి అక్షరం	*fmt)
{
    పూర్ణాంకం		arg_min = 0;

    ఒకవేళ (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	తిరిగి;
    }

    ఒకవేళ (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_ప్రారంభం);
    }
    లేకపోతే
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    కొరకు (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	స్థిరరాశి అక్షరం *p;

	ఒకవేళ (ap_types == ఖాళీ || ap_types[*arg_cur] == ఖాళీ)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    తిరిగి;
	}

	p = ap_types[*arg_cur];

	పూర్ణాంకం fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	ఎంపిక (fmt_type)
	{
	స్థితి TYPE_PERCENT:
	స్థితి TYPE_UNKNOWN:
	    ఆపు;

	స్థితి TYPE_CHAR:
	    va_arg(*ap, పూర్ణాంకం);
	    ఆపు;

	స్థితి TYPE_STRING:
	    va_arg(*ap, అక్షరం *);
	    ఆపు;

	స్థితి TYPE_సూచిక:
	    va_arg(*ap, శూన్య *);
	    ఆపు;

	స్థితి TYPE_INT:
	    va_arg(*ap, పూర్ణాంకం);
	    ఆపు;

	స్థితి TYPE_LONGINT:
	    va_arg(*ap, దీర్ఘ పూర్ణాంకం);
	    ఆపు;

	స్థితి TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    ఆపు;

	స్థితి TYPE_UNSIGNEDINT:
	    va_arg(*ap, గుర్తులేని పూర్ణాంకం);
	    ఆపు;

	స్థితి TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, గుర్తులేని దీర్ఘ పూర్ణాంకం);
	    ఆపు;

	స్థితి TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    ఆపు;

	స్థితి TYPE_FLOAT:
	    va_arg(*ap, ద్విగుణ);
	    ఆపు;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    తిరిగి;
}

    పూర్ణాంకం
vim_vsnprintf_typval(
    అక్షరం	*str,
    size_t	str_m,
    స్థిరరాశి అక్షరం	*fmt,
    va_list	ap_ప్రారంభం,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    స్థిరరాశి అక్షరం	*p = fmt;
    పూర్ణాంకం		arg_cur = 0;
    పూర్ణాంకం		అంకె_posarg = 0;
    పూర్ణాంకం		arg_idx = 1;
    va_list	ap;
    స్థిరరాశి అక్షరం	**ap_types = ఖాళీ;

    ఒకవేళ (parse_fmt_types(&ap_types, &అంకె_posarg, fmt, tvs) == FAIL)
	తిరిగి 0;

    va_copy(ap, ap_ప్రారంభం);

    ఒకవేళ (p == ఖాళీ)
	p = "";
    అయితే (*p != NUL)
    {
	ఒకవేళ (*p != '%')
	{
	    స్థిరరాశి అక్షరం    *q = strchr(p + 1, '%');
	    size_t  n = (q == ఖాళీ) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    ఒకవేళ (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	లేకపోతే
	{
	    size_t  min_క్షేత్రం_వెడల్పు = 0, precision = 0;
	    పూర్ణాంకం	    zero_padding = 0, precision_specified = 0, justify_ఎడమ = 0;
	    పూర్ణాంకం	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    పూర్ణాంకం	    space_for_ధనాత్మక = 1;

	    // allowed values: \0, h, l, L
	    అక్షరం    నిడివి_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    అక్షరం    tmp[TMP_LEN];

	    // string address in case of string argument
	    స్థిరరాశి అక్షరం  *str_arg = ఖాళీ;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    గుర్తులేని అక్షరం uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  సంఖ్య_యొక్క_zeros_గమ్యం_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    అక్షరం    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = ఖాళీ;

	    // variables for positional arg
	    పూర్ణాంకం	    pos_arg = -1;
	    స్థిరరాశి అక్షరం	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    అయితే (VIM_ISDIGIT(*ptype))
		++ptype;

	    ఒకవేళ (*ptype == '$')
	    {
		// Positional argument
		స్థిరరాశి అక్షరం *digstart = p;
		గుర్తులేని పూర్ణాంకం uj;

		ఒకవేళ (get_unsigned_int(digstart, &p, &uj, tvs != ఖాళీ) == FAIL)
		    వెళ్ళు error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    అయితే (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		ఎంపిక (*p)
		{
		    స్థితి '0': zero_padding = 1; ఆపు;
		    స్థితి '-': justify_ఎడమ = 1; ఆపు;
		    స్థితి '+': force_sign = 1; space_for_ధనాత్మక = 0; ఆపు;
		    స్థితి ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      ఆపు;
		    స్థితి '#': alternate_form = 1; ఆపు;
		    స్థితి '\'': ఆపు;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    ఒకవేళ (*p == '*')
	    {
		పూర్ణాంకం j;
		స్థిరరాశి అక్షరం *digstart = p + 1;

		p++;

		ఒకవేళ (VIM_ISDIGIT((పూర్ణాంకం)(*p)))
		{
		    // Positional argument field width
		    గుర్తులేని పూర్ణాంకం uj;

		    ఒకవేళ (get_unsigned_int(digstart, &p, &uj, tvs != ఖాళీ) == FAIL)
			వెళ్ళు error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# ఒకవేళ defined(FEAT_EVAL)
		    tvs != ఖాళీ ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, పూర్ణాంకం));

		ఒకవేళ (j > MAX_ALLOWED_STRING_వెడల్పు)
		{
		    ఒకవేళ (tvs != ఖాళీ)
		    {
			format_అధికప్రవాహం_error(digstart);
			వెళ్ళు error;
		    }
		    లేకపోతే
			j = MAX_ALLOWED_STRING_వెడల్పు;
		}

		ఒకవేళ (j >= 0)
		    min_క్షేత్రం_వెడల్పు = j;
		లేకపోతే
		{
		    min_క్షేత్రం_వెడల్పు = -j;
		    justify_ఎడమ = 1;
		}
	    }
	    లేకపోతే ఒకవేళ (VIM_ISDIGIT((పూర్ణాంకం)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		స్థిరరాశి అక్షరం *digstart = p;
		గుర్తులేని పూర్ణాంకం uj;

		ఒకవేళ (get_unsigned_int(digstart, &p, &uj, tvs != ఖాళీ) == FAIL)
		    వెళ్ళు error;

		min_క్షేత్రం_వెడల్పు = uj;
	    }

	    // parse precision
	    ఒకవేళ (*p == '.')
	    {
		p++;
		precision_specified = 1;

		ఒకవేళ (VIM_ISDIGIT((పూర్ణాంకం)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    స్థిరరాశి అక్షరం *digstart = p;
		    గుర్తులేని పూర్ణాంకం uj;

		    ఒకవేళ (get_unsigned_int(digstart, &p, &uj, tvs != ఖాళీ) == FAIL)
			వెళ్ళు error;

		    precision = uj;
		}
		లేకపోతే ఒకవేళ (*p == '*')
		{
		    పూర్ణాంకం j;
		    స్థిరరాశి అక్షరం *digstart = p;

		    p++;

		    ఒకవేళ (VIM_ISDIGIT((పూర్ణాంకం)(*p)))
		    {
			// positional argument
			గుర్తులేని పూర్ణాంకం uj;

			ఒకవేళ (get_unsigned_int(digstart, &p, &uj, tvs != ఖాళీ) == FAIL)
			    వెళ్ళు error;

			arg_idx = uj;

			++p;
		    }

		    j =
# ఒకవేళ defined(FEAT_EVAL)
			tvs != ఖాళీ ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, పూర్ణాంకం));

		    ఒకవేళ (j > MAX_ALLOWED_STRING_వెడల్పు)
		    {
			ఒకవేళ (tvs != ఖాళీ)
			{
			    format_అధికప్రవాహం_error(digstart);
			    వెళ్ళు error;
			}
			లేకపోతే
			    j = MAX_ALLOWED_STRING_వెడల్పు;
		    }

		    ఒకవేళ (j >= 0)
			precision = j;
		    లేకపోతే
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ఒకవేళ (*p == 'h' || *p == 'l')
	    {
		నిడివి_modifier = *p;
		p++;
		ఒకవేళ (నిడివి_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    నిడివి_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    ఎంపిక (fmt_spec)
	    {
		స్థితి 'i': fmt_spec = 'd'; ఆపు;
		స్థితి 'D': fmt_spec = 'd'; నిడివి_modifier = 'l'; ఆపు;
		స్థితి 'U': fmt_spec = 'u'; నిడివి_modifier = 'l'; ఆపు;
		స్థితి 'O': fmt_spec = 'o'; నిడివి_modifier = 'l'; ఆపు;
		అప్రమేయ: ఆపు;
	    }

# ఒకవేళ defined(FEAT_EVAL)
	    ఎంపిక (fmt_spec)
	    {
		స్థితి 'd': స్థితి 'u': స్థితి 'o': స్థితి 'x': స్థితి 'X':
		    ఒకవేళ (tvs != ఖాళీ && నిడివి_modifier == '\0')
			నిడివి_modifier = 'L';
	    }
# endif

	    ఒకవేళ (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    ఎంపిక (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    స్థితి '%':
	    స్థితి 'c':
	    స్థితి 's':
	    స్థితి 'S':
		str_arg_l = 1;
		ఎంపిక (fmt_spec)
		{
		స్థితి '%':
		    str_arg = p;
		    ఆపు;

		స్థితి 'c':
		    {
			పూర్ణాంకం j;

			j =
# ఒకవేళ defined(FEAT_EVAL)
			    tvs != ఖాళీ ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, పూర్ణాంకం));

			// standard demands unsigned char
			uchar_arg = (గుర్తులేని అక్షరం)j;
			str_arg = (అక్షరం *)&uchar_arg;
			ఆపు;
		    }

		స్థితి 's':
		స్థితి 'S':
		    str_arg =
# ఒకవేళ defined(FEAT_EVAL)
				tvs != ఖాళీ ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, అక్షరం *));

		    ఒకవేళ (str_arg == ఖాళీ)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    లేకపోతే ఒకవేళ (!precision_specified)
			str_arg_l = అక్షరశ్రేణి_పొడవు(str_arg);
		    // truncate string if necessary as requested by precision
		    లేకపోతే ఒకవేళ (precision == 0)
			str_arg_l = 0;
		    లేకపోతే
		    {
			// memchr on HP does not like n > 2^31  !!!
			స్థిరరాశి అక్షరం *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == ఖాళీ) ? precision
						      : (size_t)(q - str_arg);
		    }
		    ఒకవేళ (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			పూర్ణాంకం	cell;

			కొరకు (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    ఒకవేళ (precision_specified && i + cell > precision)
				ఆపు;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			ఒకవేళ (min_క్షేత్రం_వెడల్పు != 0)
			    min_క్షేత్రం_వెడల్పు += str_arg_l - i;
		    }
		    ఆపు;

		అప్రమేయ:
		    ఆపు;
		}
		ఆపు;

	    స్థితి 'd': స్థితి 'u':
	    స్థితి 'b': స్థితి 'B':
	    స్థితి 'o':
	    స్థితి 'x': స్థితి 'X':
	    స్థితి 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //విలువ

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    పూర్ణాంకం arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    పూర్ణాంకం int_arg = 0;
		    గుర్తులేని పూర్ణాంకం uint_arg = 0;

		    // only set for length modifier l
		    దీర్ఘ పూర్ణాంకం long_arg = 0;
		    గుర్తులేని దీర్ఘ పూర్ణాంకం ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    శూన్య *ptr_arg = ఖాళీ;

		    ఒకవేళ (fmt_spec == 'p')
		    {
			నిడివి_modifier = '\0';
			ptr_arg =
# ఒకవేళ defined(FEAT_EVAL)
				 tvs != ఖాళీ ? (శూన్య *)tv_str(tvs, &arg_idx,
									ఖాళీ) :
# endif
					(skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, శూన్య *));

			ఒకవేళ (ptr_arg != ఖాళీ)
			    arg_sign = 1;
		    }
		    లేకపోతే ఒకవేళ (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# ఒకవేళ defined(FEAT_EVAL)
				    tvs != ఖాళీ ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			ఒకవేళ (bin_arg != 0)
			    arg_sign = 1;
		    }
		    లేకపోతే ఒకవేళ (fmt_spec == 'd')
		    {
			// signed
			ఎంపిక (నిడివి_modifier)
			{
			స్థితి '\0':
			స్థితి 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# ఒకవేళ defined(FEAT_EVAL)
					tvs != ఖాళీ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, పూర్ణాంకం));

			    ఒకవేళ (int_arg > 0)
				arg_sign =  1;
			    లేకపోతే ఒకవేళ (int_arg < 0)
				arg_sign = -1;
			    ఆపు;
			స్థితి 'l':
			    long_arg =
# ఒకవేళ defined(FEAT_EVAL)
					tvs != ఖాళీ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, దీర్ఘ పూర్ణాంకం));

			    ఒకవేళ (long_arg > 0)
				arg_sign =  1;
			    లేకపోతే ఒకవేళ (long_arg < 0)
				arg_sign = -1;
			    ఆపు;
			స్థితి 'L':
			    llong_arg =
# ఒకవేళ defined(FEAT_EVAL)
					tvs != ఖాళీ ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    ఒకవేళ (llong_arg > 0)
				arg_sign =  1;
			    లేకపోతే ఒకవేళ (llong_arg < 0)
				arg_sign = -1;
			    ఆపు;
			}
		    }
		    లేకపోతే
		    {
			// unsigned
			ఎంపిక (నిడివి_modifier)
			{
			    స్థితి '\0':
			    స్థితి 'h':
				uint_arg =
# ఒకవేళ defined(FEAT_EVAL)
					    tvs != ఖాళీ ? (గుర్తులేని)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, గుర్తులేని పూర్ణాంకం));

				ఒకవేళ (uint_arg != 0)
				    arg_sign = 1;
				ఆపు;
			    స్థితి 'l':
				ulong_arg =
# ఒకవేళ defined(FEAT_EVAL)
					    tvs != ఖాళీ ? (గుర్తులేని దీర్ఘ)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, గుర్తులేని దీర్ఘ పూర్ణాంకం));

				ఒకవేళ (ulong_arg != 0)
				    arg_sign = 1;
				ఆపు;
			    స్థితి 'L':
				ullong_arg =
# ఒకవేళ defined(FEAT_EVAL)
					    tvs != ఖాళీ ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				ఒకవేళ (ullong_arg != 0)
				    arg_sign = 1;
				ఆపు;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    ఒకవేళ (precision_specified)
			zero_padding = 0;
		    ఒకవేళ (fmt_spec == 'd')
		    {
			ఒకవేళ (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_ధనాత్మక ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    లేకపోతే ఒకవేళ (alternate_form)
		    {
			ఒకవేళ (arg_sign != 0
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
		    ఒకవేళ (!precision_specified)
			precision = 1;   // default precision is 1
		    ఒకవేళ (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    లేకపోతే
		    {
			అక్షరం	f[6];
			పూర్ణాంకం	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			ఒకవేళ (!నిడివి_modifier)
			    ;
			లేకపోతే ఒకవేళ (నిడివి_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# లేకపోతే
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			లేకపోతే
			    f[f_l++] = నిడివి_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			ఒకవేళ (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			లేకపోతే ఒకవేళ (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    అక్షరం	    b[8 * పరిమాణం(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    చేయి
			    {
				b[పరిమాణం(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    అయితే (bn != 0);

			    జ్ఞాపకం_నకలు(tmp + str_arg_l, b + పరిమాణం(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			లేకపోతే ఒకవేళ (fmt_spec == 'd')
			{
			    // signed
			    ఎంపిక (నిడివి_modifier)
			    {
			    స్థితి '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       ఆపు;
			    స్థితి 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (పొట్టి)int_arg);
				      ఆపు;
			    స్థితి 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      ఆపు;
			    స్థితి 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      ఆపు;
			    }
			}
			లేకపోతే
			{
			    // unsigned
			    ఎంపిక (నిడివి_modifier)
			    {
			    స్థితి '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       ఆపు;
			    స్థితి 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(గుర్తులేని పొట్టి)uint_arg);
				      ఆపు;
			    స్థితి 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      ఆపు;
			    స్థితి 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      ఆపు;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			ఒకవేళ (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			ఒకవేళ (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t అంకె_యొక్క_digits = str_arg_l
						 - zero_padding_insertion_ind;

			ఒకవేళ (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    ఒకవేళ (!precision_specified
					     || precision < అంకె_యొక్క_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = అంకె_యొక్క_digits + 1;
			    }
			}
			// zero padding to specified precision?
			ఒకవేళ (అంకె_యొక్క_digits < precision)
			    సంఖ్య_యొక్క_zeros_గమ్యం_pad = precision - అంకె_యొక్క_digits;
		    }
		    // zero padding to specified minimal field width?
		    ఒకవేళ (!justify_ఎడమ && zero_padding)
		    {
			పూర్ణాంకం n = (పూర్ణాంకం)(min_క్షేత్రం_వెడల్పు - (str_arg_l
						    + సంఖ్య_యొక్క_zeros_గమ్యం_pad));
			ఒకవేళ (n > 0)
			    సంఖ్య_యొక్క_zeros_గమ్యం_pad += n;
		    }
		    ఆపు;
		}

	    స్థితి 'f':
	    స్థితి 'F':
	    స్థితి 'e':
	    స్థితి 'E':
	    స్థితి 'g':
	    స్థితి 'G':
		{
		    // Floating point.
		    ద్విగుణ	f;
		    ద్విగుణ	abs_f;
		    అక్షరం	format[40];
		    పూర్ణాంకం		l;
		    పూర్ణాంకం		remove_trailing_zeroes = FALSE;

		    f =
# ఒకవేళ defined(FEAT_EVAL)
			tvs != ఖాళీ ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_గమ్యం_arg(ap_types, ap_ప్రారంభం, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, ద్విగుణ));

		    abs_f = f < 0 ? -f : f;

		    ఒకవేళ (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			ఒకవేళ ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			లేకపోతే
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    ఒకవేళ ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# లేకపోతే
			    abs_f > 1.0e307
# endif
			    )
		    {
			// Avoid a buffer overflow
			STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_ధనాత్మక));
			str_arg_l = STRLEN(tmp);
			zero_padding = 0;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			లేకపోతే ఒకవేళ (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_ధనాత్మక));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			లేకపోతే
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    ఒకవేళ (force_sign)
				format[l++] = space_for_ధనాత్మక ? ' ' : '+';
			    ఒకవేళ (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				ఒకవేళ ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				ఒకవేళ (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (పూర్ణాంకం)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			ఒకవేళ (remove_trailing_zeroes)
			{
			    పూర్ణాంకం i;
			    అక్షరం *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    ఒకవేళ (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    లేకపోతే
			    {
				tp = (అక్షరం *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				ఒకవేళ (tp != ఖాళీ)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    ఒకవేళ (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    అయితే (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    ఒకవేళ (tp != ఖాళీ && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				అయితే (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			లేకపోతే
			{
			    అక్షరం *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (అక్షరం *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    ఒకవేళ (tp != ఖాళీ && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    ఒకవేళ (zero_padding && min_క్షేత్రం_వెడల్పు > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			సంఖ్య_యొక్క_zeros_గమ్యం_pad = min_క్షేత్రం_వెడల్పు - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    ఆపు;
		}

	    అప్రమేయ:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_ఎడమ = 1;
		min_క్షేత్రం_వెడల్పు = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		ఒకవేళ (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		ఆపు;
	    }

	    ఒకవేళ (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    ఒకవేళ (!justify_ఎడమ)
	    {
		// left padding with blank or zero
		పూర్ణాంకం pn = (పూర్ణాంకం)(min_క్షేత్రం_వెడల్పు - (str_arg_l + సంఖ్య_యొక్క_zeros_గమ్యం_pad));

		ఒకవేళ (pn > 0)
		{
		    ఒకవేళ (str_l < str_m)
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
	    ఒకవేళ (సంఖ్య_యొక్క_zeros_గమ్యం_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    లేకపోతే
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		పూర్ణాంకం zn = (పూర్ణాంకం)zero_padding_insertion_ind;

		ఒకవేళ (zn > 0)
		{
		    ఒకవేళ (str_l < str_m)
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
		zn = (పూర్ణాంకం)సంఖ్య_యొక్క_zeros_గమ్యం_pad;
		ఒకవేళ (zn > 0)
		{
		    ఒకవేళ (str_l < str_m)
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
		పూర్ణాంకం sn = (పూర్ణాంకం)(str_arg_l - zero_padding_insertion_ind);

		ఒకవేళ (sn > 0)
		{
		    ఒకవేళ (str_l < str_m)
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
	    ఒకవేళ (justify_ఎడమ)
	    {
		// right blank padding to the field width
		పూర్ణాంకం pn = (పూర్ణాంకం)(min_క్షేత్రం_వెడల్పు
				      - (str_arg_l + సంఖ్య_యొక్క_zeros_గమ్యం_pad));

		ఒకవేళ (pn > 0)
		{
		    ఒకవేళ (str_l < str_m)
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

    ఒకవేళ (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    ఒకవేళ (tvs != ఖాళీ && tvs[అంకె_posarg != 0 ? అంకె_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_గమ్యం_printf));

error:
    vim_free((అక్షరం*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    తిరిగి (పూర్ణాంకం)str_l;
}

#అయితే_ముగింపు // PROTO
