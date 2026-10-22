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

#تعریف_کن USING_FLOAT_STUFF
#ضمن "vim.h"

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
    إذا (p != عدم)
	mch_memmove(p, string, len);
    أرجع p;
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
    إذا (p == عدم)
	أرجع عدم;

    STRNCPY(p, string, len);
    p[len] = NUL;
    أرجع p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    أرجع vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    صحيح		cc,
    صحيح		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    بیعلامت	درازی;
    صحيح		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    درازی = 1;				// count the trailing NUL
    لكل (p = string; *p; p++)
    {
	إذا (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    درازی += l;		// count a multibyte char
	    p += l - 1;
	    استمر;
	}
	إذا (vim_strchr(esc_chars, *p) != عدم || (bsl && rem_backslash(p)))
	    ++درازی;			// count a backslash
	++درازی;			// count an ordinary char
    }
    escaped_string = alloc(درازی);
    إذا (escaped_string == عدم)
	أرجع عدم;
    p2 = escaped_string;
    لكل (p = string; *p; p++)
    {
	إذا (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    استمر;
	}
	إذا (vim_strchr(esc_chars, *p) != عدم || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    أرجع escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    صحيح
csh_like_shell(فراغ)
{
    أرجع (جستجوی_رشته((حرف *)gettail(p_sh), "csh") != عدم);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    ساكن صحيح
fish_like_shell(فراغ)
{
    أرجع (جستجوی_رشته((حرف *)gettail(p_sh), "fish") != عدم);
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
vim_strsave_shellescape(char_u *string, صحيح do_special, صحيح do_newline)
{
    بیعلامت	درازی;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    صحيح		csh_like;
    صحيح		fish_like;
    char_u	*shname;
    صحيح		powershell;
#اگر_تعریف_شده MSWIN
    صحيح		double_quotes;
#پایان_اگر

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
    powershell = جستجوی_رشته((حرف *)shname, "pwsh") != عدم;
#اگر_تعریف_شده MSWIN
    powershell = powershell || جستجوی_رشته((حرف *)shname, "powershell") != عدم;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#پایان_اگر

    // First count the number of extra bytes required.
    درازی = (بیعلامت)STRLEN(string) + 3;  // two quotes and a trailing NUL
    لكل (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#اگر_تعریف_شده MSWIN
	إذا (double_quotes)
	{
	    إذا (*p == '"')
		++درازی;		// " -> ""
	}
	وإلا
#پایان_اگر
	إذا (*p == '\'')
	{
	    إذا (powershell)
		درازی += 2;		// ' => ''
	    وإلا
		درازی += 3;		// ' => '\''
	}
	إذا ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++درازی;			// insert backslash
	    إذا (csh_like && do_special)
		++درازی;		// insert backslash
	}
	إذا (do_special && پیدا_کن_cmdline_var(p, &l) >= 0)
	{
	    ++درازی;			// insert backslash
	    p += l - 1;
	}
	إذا (*p == '\\' && fish_like)
	    ++درازی;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(درازی);
    إذا (escaped_string != عدم)
    {
	d = escaped_string;

	// add opening quote
#اگر_تعریف_شده MSWIN
	إذا (double_quotes)
	    *d++ = '"';
	وإلا
#پایان_اگر
	    *d++ = '\'';

	لكل (p = string; *p != NUL; )
	{
#اگر_تعریف_شده MSWIN
	    إذا (double_quotes)
	    {
		إذا (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    استمر;
		}
	    }
	    وإلا
#پایان_اگر
	    إذا (*p == '\'')
	    {
		إذا (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		وإلا
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		استمر;
	    }
	    إذا ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		إذا (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		استمر;
	    }
	    إذا (do_special && پیدا_کن_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		کپی_حافظه(d, p, l);	// copy the var
		d += l;
		p += l;
		استمر;
	    }
	    إذا (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		استمر;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#اگر_تعریف_شده MSWIN
	إذا (double_quotes)
	    *d++ = '"';
	وإلا
#پایان_اگر
	    *d++ = '\'';
	*d = NUL;
    }

    أرجع escaped_string;
}

/*
 * Like vim_strsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strsave_به_بالا(char_u *string)
{
    char_u *p1;

    p1 = vim_strsave(string);
    vim_strup(p1);
    أرجع p1;
}

/*
 * Like vim_strnsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strnsave_به_بالا(char_u *string, size_t len)
{
    char_u *p1;

    p1 = vim_strnsave(string, len);
    vim_strup(p1);
    أرجع p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    فراغ
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    صحيح	    c;

    إذا (p == عدم)
	أرجع;

    p2 = p;
    طالما ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#إذا defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    ساكن char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    إذا (res != عدم)
	طالما (*p != NUL)
	{
	    صحيح		l;

	    إذا (enc_utf8)
	    {
		صحيح	c, uc;
		صحيح	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		إذا (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		إذا (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    إذا (s == عدم)
		    {
			vim_free(res);
			أرجع عدم;
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
	    وإلا إذا (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    وإلا
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    أرجع res;
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

    إذا (res != عدم)
	طالما (*p != NUL)
	{
	    صحيح		l;

	    إذا (enc_utf8)
	    {
		صحيح	c, lc;
		صحيح	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		إذا (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		إذا (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    إذا (s == عدم)
		    {
			vim_free(res);
			أرجع عدم;
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
	    وإلا إذا (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    وإلا
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    أرجع res;
}
#پایان_اگر

/*
 * delete spaces at the end of a string
 */
    فراغ
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    طالما (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    فراغ
vim_strncpy(char_u *مقصد, char_u *from, size_t len)
{
    STRNCPY(مقصد, from, len);
    مقصد[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    فراغ
vim_strcat(char_u *مقصد, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(مقصد);
    size_t fromlen = STRLEN(from);

    إذا (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(مقصد + tolen, from, tosize - tolen - 1);
	مقصد[tosize - 1] = NUL;
    }
    وإلا
	mch_memmove(مقصد + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(حرف *s, size_t maxlen)
{
    size_t i;
    لكل (i = 0; i < maxlen; ++i)
	إذا (s[i] == NUL)
	    اكسر;
    أرجع i;
}

#إذا !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    صحيح
vim_stricmp(حرف *s1, حرف *s2)
{
    صحيح		i;

    لكل (;;)
    {
	i = (صحيح)TOLOWER_LOC(*s1) - (صحيح)TOLOWER_LOC(*s2);
	إذا (i != 0)
	    أرجع i;			    // this character different
	إذا (*s1 == NUL)
	    اكسر;			    // strings match until NUL
	++s1;
	++s2;
    }
    أرجع 0;				    // strings match
}
#پایان_اگر

#إذا !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    صحيح
vim_strnicmp(حرف *s1, حرف *s2, size_t len)
{
    صحيح		i;

    طالما (len > 0)
    {
	i = (صحيح)TOLOWER_LOC(*s1) - (صحيح)TOLOWER_LOC(*s2);
	إذا (i != 0)
	    أرجع i;			    // this character different
	إذا (*s1 == NUL)
	    اكسر;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    أرجع 0;				    // strings match
}
#پایان_اگر

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    صحيح
vim_strnicmp_asc(حرف *s1, حرف *s2, size_t len)
{
    صحيح                i = 0;

    طالما (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	إذا (i != 0)
	    اكسر;			// this character is different
	إذا (*s1 == NUL)
	    اكسر;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    أرجع i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, صحيح c)
{
    char_u	*p;
    صحيح		b;

    p = string;
    إذا (enc_utf8 && c > 0 && c < 0x80)
	أرجع vim_strbyte(string, c);
    إذا (enc_utf8 && c >= 0x80)
    {
	طالما (*p != NUL)
	{
	    صحيح l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    إذا (utf_ptr2char(p) == c && l > 1)
		أرجع p;
	    p += l;
	}
	أرجع عدم;
    }
    إذا (enc_dbcs != 0 && c > 255)
    {
	صحيح	n2 = c & 0xff;

	c = ((بیعلامت)c >> 8) & 0xff;
	طالما ((b = *p) != NUL)
	{
	    إذا (b == c && p[1] == n2)
		أرجع p;
	    p += (*mb_ptr2len)(p);
	}
	أرجع عدم;
    }
    إذا (has_mbyte)
    {
	طالما ((b = *p) != NUL)
	{
	    إذا (b == c)
		أرجع p;
	    p += (*mb_ptr2len)(p);
	}
	أرجع عدم;
    }
    طالما ((b = *p) != NUL)
    {
	إذا (b == c)
	    أرجع p;
	++p;
    }
    أرجع عدم;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, صحيح c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    إذا (c <= 0 || c > 255)
	أرجع عدم;
    أرجع (char_u *)strchr((حرف *)string, c);
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
vim_strrchr(char_u *string, صحيح c)
{
    char_u	*retval = عدم;
    char_u	*p = string;

    طالما (*p)
    {
	إذا (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    أرجع retval;
}

/*
 * Vim's version of strpbrk(), in case it's missing.
 * Don't generate a prototype for this, causes problems when it's not used.
 */
#اگر_تعریف_نشده PROTO
# ifndef HAVE_STRPBRK
#  ifdef vim_strpbrk
#   undef vim_strpbrk
#  endif
    char_u *
vim_strpbrk(char_u *s, char_u *charset)
{
    طالما (*s)
    {
	إذا (vim_strchr(charset, *s) != عدم)
	    أرجع s;
	MB_PTR_ADV(s);
    }
    أرجع عدم;
}
# endif
#پایان_اگر

/*
 * Sort an array of strings.
 */
ساكن صحيح مرتب_سازی_compare(ثابت فراغ *s1, ثابت فراغ *s2);

    ساكن صحيح
مرتب_سازی_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    أرجع STRCMP(*(حرف **)s1, *(حرف **)s2);
}

    فراغ
مرتب_سازی_رشتهها(
    char_u	**files,
    صحيح		شمار)
{
    qsort((فراغ *)files, (size_t)شمار, حجم(char_u *), مرتب_سازی_compare);
}

#إذا defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    صحيح
has_non_ascii(char_u *s)
{
    char_u	*p;

    إذا (s != عدم)
	لكل (p = s; *p != NUL; ++p)
	    إذا (*p >= 128)
		أرجع TRUE;
    أرجع FALSE;
}
#پایان_اگر

/*
 * Concatenate two strings and return the result in allocated memory.
 * Returns NULL when out of memory.
 */
    char_u  *
concat_str(char_u *str1, char_u *str2)
{
    char_u  *dest;
    size_t  l = str1 == عدم ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == عدم ? 0 : STRLEN(str2)) + 1L);
    إذا (dest == عدم)
	أرجع عدم;
    إذا (str1 == عدم)
	*dest = NUL;
    وإلا
	STRCPY(dest, str1);
    إذا (str2 != عدم)
	STRCPY(dest + l, str2);
    أرجع dest;
}

#إذا defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
معکوس_متن(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    إذا (rev == عدم)
	أرجع عدم;

    لكل (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	إذا (has_mbyte)
	{
	    صحيح mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	وإلا
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    أرجع rev;
}
#پایان_اگر

#إذا defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, صحيح function)
{
    بیعلامت	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    إذا (str != عدم)
    {
	len += (بیعلامت)STRLEN(str);
	لكل (p = str; *p != NUL; MB_PTR_ADV(p))
	    إذا (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    إذا (r == عدم)
	أرجع عدم;

    إذا (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    وإلا
	*r++ = '\'';
    إذا (str != عدم)
	لكل (p = str; *p != NUL; )
	{
	    إذا (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    إذا (function)
	*r++ = ')';
    *r++ = NUL;
    أرجع s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    دراز
string_شمار(char_u *haystack, char_u *needle, صحيح ic)
{
    دراز	n = 0;
    char_u	*p = haystack;
    char_u	*بعدی;

    إذا (p == عدم || needle == عدم || *needle == NUL)
	أرجع 0;

    size_t  needlelen = STRLEN(needle);
    إذا (ic)
    {
	طالما (*p != NUL)
	{
	    إذا (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    وإلا
		MB_PTR_ADV(p);
	}
    }
    وإلا
	طالما ((بعدی = (char_u *)جستجوی_رشته((حرف *)p, (حرف *)needle)) != عدم)
	{
	    ++n;
	    p = بعدی + needlelen;
	}

    أرجع n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    ساكن صحيح
copy_اولی_char_مقصد_tv(char_u *درون_داد, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    صحيح		len;

    إذا (درون_داد == عدم || output == عدم)
	أرجع -1;

    len = has_mbyte ? mb_ptr2len(درون_داد) : 1;
    STRNCPY(buf, درون_داد, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    أرجع output->vval.v_string == عدم ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    فراغ
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*عبارت,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    صحيح		len = 0;
    صحيح		idx = 0;
    صحيح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    // set_vim_var_nr() doesn't set the type
    مجموعه_vim_var_type(VV_کلید, VAR_عدد);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_عبارت_get_funccal(عبارت, &newtv);

    ga_init2(&ga, حجم(حرف), 80);
    لكل (p = str; *p != NUL; p += len)
    {
	len = copy_اولی_char_مقصد_tv(p, &tv);
	إذا (len < 0)
	    اكسر;

	مجموعه_vim_var_nr(VV_کلید, idx);
	إذا (filter_map_one(&tv, عبارت, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    اكسر;
	}
	إذا (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    إذا (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		اكسر;
	    }
	    وإلا
		ga_concat(&ga, newtv.vval.v_string);
	}
	وإلا إذا (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_داده;
    إذا (fc != عدم)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    فراغ
string_reduce(
	typval_T	*argvars,
	typval_T	*عبارت,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    صحيح		len;
    typval_T	argv[3];
    صحيح		r;
    صحيح		called_emsg_شروع = called_emsg;
    funccall_T	*fc;

    إذا (argvars[2].v_type == VAR_UNKNOWN)
    {
	إذا (*p == NUL)
	{
	    semsg(_(e_reduce_از_an_خالی_str_with_no_initial_مقدار), "String");
	    أرجع;
	}
	len = copy_اولی_char_مقصد_tv(p, rettv);
	إذا (len < 0)
	    أرجع;
	p += len;
    }
    وإلا إذا (check_for_string_arg(argvars, 2) == FAIL)
	أرجع;
    وإلا
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_عبارت_get_funccal(عبارت, rettv);

    لكل ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_اولی_char_مقصد_tv(p, &argv[1]);
	إذا (len < 0)
	{
	    *rettv = argv[0];
	    اكسر;
	}

	r = eval_عبارت_typval(عبارت, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	إذا (r == FAIL || called_emsg != called_emsg_شروع)
	    اكسر;
    }

    إذا (fc != عدم)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    ساكن فراغ
byteidx_common(typval_T *argvars, typval_T *rettv, صحيح comp)
{
    rettv->vval.v_عدد = -1;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	أرجع;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_عدد_chk(&argvars[1], عدم);
    إذا (str == عدم || idx < 0)
	أرجع;

    varnumber_T	utf16idx = FALSE;
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	صحيح error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	إذا (error)
	    أرجع;
	إذا (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_عدد_as_bool_nr), utf16idx);
	    أرجع;
	}
    }

    صحيح (*ptr2len)(char_u *);
    إذا (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    وإلا
	ptr2len = mb_ptr2len;

    char_u *t = str;
    لكل ( ; idx > 0; idx--)
    {
	إذا (*t == NUL)		// EOL reached
	    أرجع;
	إذا (utf16idx)
	{
	    صحيح clen = ptr2len(t);
	    صحيح c = (clen > 1) ? utf_ptr2char(t) : *t;
	    إذا (c > 0xFFFF)
		idx--;
	    إذا (idx > 0)
		t += clen;
	}
	وإلا إذا (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_عدد = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    فراغ
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    فراغ
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    فراغ
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;

    إذا (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	أرجع;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_عدد_chk(&argvars[1], عدم);
    إذا (str == عدم || idx < 0)
	أرجع;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	إذا (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    صحيح (*ptr2len)(char_u *);
    إذا (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    وإلا
	ptr2len = mb_ptr2len;

    char_u	*p;
    صحيح		len;
    لكل (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	إذا (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    إذا (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_عدد = len;
	    أرجع;
	}
	إذا (utf16idx)
	{
	    idx--;
	    صحيح clen = ptr2len(p);
	    صحيح c = (clen > 1) ? utf_ptr2char(p) : *p;
	    إذا (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_عدد = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    ساكن صحيح
convert_string(string_T *str, char_u *from, char_u *مقصد, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    إذا (convert_راهاندازی(&vimconv, from, مقصد) == FAIL)
	أرجع FAIL;
    vimconv.vc_fail = TRUE;
    إذا (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->درازی);
	إذا (ret->string == عدم)
	    ret->درازی = 0;
	وإلا
	    ret->درازی = str->درازی;
    }
    وإلا
    {
	صحيح len = (صحيح)str->درازی;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->درازی = len;
    }
    convert_راهاندازی(&vimconv, عدم, عدم);

    أرجع (ret->string == عدم) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    ساكن فراغ
blob_from_string(char_u *str, blob_T *blob)
{
    صحيح	    len = (صحيح)STRLEN(str);
    char_u  *dest;

    إذا (len == 0)
	أرجع;
    إذا (ga_grow(&blob->bv_ga, len) == FAIL)
	أرجع;
    dest = (char_u *)blob->bv_ga.ga_داده + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    لكل (صحيح i = 0; i < len; ++i)
	إذا (dest[i] == NL)
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
    ساكن صحيح
string_from_blob(blob_T *blob, دراز *شروع_idx, string_T *ret)
{
    دراز	blen = blob_len(blob);
    دراز	شروع = *شروع_idx;
    char_u	*src;
    char_u	*nl;
    دراز	سطر_len;

    إذا (شروع >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->درازی = 0;
	*شروع_idx = blen;
	أرجع (ret->string == عدم) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_داده + شروع;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - شروع));
    سطر_len = (nl == عدم) ? (blen - شروع) : (دراز)(nl - src);

    ret->string = alloc(سطر_len + 1);
    إذا (ret->string == عدم)
    {
	ret->درازی = 0;
	أرجع FAIL;
    }
    إذا (سطر_len > 0)
	mch_memmove(ret->string, src, (size_t)سطر_len);
    ret->string[سطر_len] = NUL;
    ret->درازی = (size_t)سطر_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    لكل (دراز i = 0; i < سطر_len; i++)
	إذا (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *شروع_idx = شروع + سطر_len + (nl != عدم ? 1 : 0);
    أرجع OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    ساكن char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    إذا (from_encoding_raw == عدم)
	أرجع عدم;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    طالما (*s != NUL)
    {
	إذا (*s == '_')
	    *pe++ = '-';
	وإلا
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    إذا ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    أرجع from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    ساكن فراغ
append_converted_string_مقصد_list(
	string_T *converted,
	صحيح validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    إذا (converted->string != عدم)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *انتها = converted->string + converted->درازی;
	طالما (p < انتها)
	{
	    string_T	سطر;
	    char_u	*سطر_شروع = p;

	    طالما (p < انتها && *p != NL)
		p++;

	    // Add this line to the result list
	    سطر.درازی = (size_t)(p - سطر_شروع);
	    سطر.string = vim_strnsave(سطر_شروع, سطر.درازی);
	    إذا (سطر.string != عدم)
	    {
		إذا (validate_utf8 && !utf_valid_string(سطر.string, عدم))
		{
		    vim_free(سطر.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    أرجع; // Stop processing
		}
		إذا (list_append_string(list, سطر.string, (صحيح)سطر.درازی) == FAIL)
		{
		    vim_free(سطر.string);
		    vim_free(converted->string);
		    أرجع; // Stop processing on append failure
		}
		vim_free(سطر.string);
	    }
	    وإلا
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_بیرون_از_حافظه));
		vim_free(converted->string);
		أرجع;
	    }

	    إذا (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    وإلا
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    ساكن صحيح
append_validated_سطر_مقصد_list(string_T *سطر, صحيح validate_utf8, list_T *list)
{
    إذا (validate_utf8 && !utf_valid_string(سطر->string, عدم))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(سطر->string);
	أرجع FAIL;
    }

    صحيح ret = list_append_string(list, سطر->string, (صحيح)سطر->درازی);
    vim_free(سطر->string);
    أرجع ret;
}

    فراغ
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    صحيح		blen;
    دراز	idx;
    صحيح		validate_utf8 = FALSE;

    إذا (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    blob = argvars->vval.v_blob;
    إذا (blob == عدم)
	أرجع;
    blen = blob_len(blob);

    char_u	*from_encoding = عدم;
    char_u	*from_encoding_raw = عدم;  // Encoding name with endianness preserved for iconv
    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	إذا (d != عدم)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    إذا (enc != عدم)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		إذا (from_encoding_raw == عدم)
		{
		    emsg(_(e_بیرون_از_حافظه));
		    VIM_CLEAR(from_encoding);
		    أرجع;
		}
	    }
	}
    }

    إذا (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    إذا (from_encoding != عدم && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    صحيح from_prop = 0;
    إذا (from_encoding != عدم)
	from_prop = enc_canon_props(from_encoding);
    إذا (from_encoding != عدم && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2_کلمه)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	صحيح nul_اندازه = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_اندازه);
	إذا (ga_grow(&blob_ga, blen + nul_اندازه) == OK)
	{
	    إذا (blen > 0)
		mch_memmove(blob_ga.ga_داده, blob->bv_ga.ga_داده, (size_t)blen);
	    // NUL terminator (2 bytes for UTF-16/UCS-2, 4 bytes for UTF-32/UCS-4)
	    vim_memset((char_u *)blob_ga.ga_داده + blen, 0, (size_t)nul_اندازه);
	    blob_ga.ga_len = blen + nul_اندازه;
	}

	// Convert the entire blob at once
	vimconv_T vimconv;
	vimconv.vc_type = CONV_NONE;
	// Use raw encoding name for iconv to preserve endianness (utf-16be vs utf-16)
	// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
	إذا (convert_راهاندازی_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    اذهب انجامشده;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	صحيح len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_داده, &len, عدم);
	converted.درازی = len;
	convert_راهاندازی(&vimconv, عدم, عدم);
	ga_clear(&blob_ga);
	append_converted_string_مقصد_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    وإلا
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	طالما (idx < blen)
	{
	    string_T	str;

	    إذا (string_from_blob(blob, &idx, &str) != OK)
		اكسر;

	    إذا (from_encoding != عدم)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		صحيح	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		إذا (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    اذهب انجامشده;
		}
		str.string = converted.string;
		str.درازی = converted.درازی;
	    }

	    إذا (append_validated_سطر_مقصد_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		اذهب انجامشده;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    إذا (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

انجامشده:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    فراغ
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    إذا (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (rettv_blob_alloc(rettv) == FAIL)
	أرجع;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    إذا (list == عدم)
	أرجع;

    char_u	*مقصد_encoding = عدم;
    char_u	*مقصد_encoding_raw = عدم;  // Encoding name with endianness preserved for iconv
    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	إذا (d != عدم)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    إذا (enc != عدم)
	    {
		char_u *enc_skipped = enc_skip(enc);
		مقصد_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		مقصد_encoding_raw = normalize_encoding_name(enc_skipped);
		إذا (مقصد_encoding_raw == عدم)
		{
		    emsg(_(e_بیرون_از_حافظه));
		    VIM_CLEAR(مقصد_encoding);
		    أرجع;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    صحيح مقصد_prop = 0;
    إذا (مقصد_encoding != عدم)
	مقصد_prop = enc_canon_props(مقصد_encoding);
    إذا (مقصد_encoding != عدم && (مقصد_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2_کلمه)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_اقلام(list, li)
	{
	    char_u *s;

	    إذا (li->li_tv.v_type != VAR_STRING)
		استمر;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    إذا (li != list->lv_اولی)
		ga_append(&str_ga, NL);
	    إذا (s != عدم && *s != NUL)
	    {
		صحيح slen = (صحيح)STRLEN(s);

		إذا (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    اذهب انجامشده;
		}
		mch_memmove((char_u *)str_ga.ga_داده + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	إذا (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    إذا (convert_راهاندازی_ext(&vimconv, p_enc, FALSE, مقصد_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_مقصد_failed), مقصد_encoding);
		اذهب انجامشده;
	    }
	    vimconv.vc_fail = TRUE;

	    صحيح		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_داده, &len, عدم);
	    convert_راهاندازی(&vimconv, عدم, عدم);
	    ga_clear(&str_ga);

	    إذا (converted == عدم)
	    {
		semsg(_(e_str_encoding_مقصد_failed), مقصد_encoding);
		اذهب انجامشده;
	    }
	    إذا (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_داده + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	وإلا
	    ga_clear(&str_ga);
    }
    وإلا
    {
	FOR_ALL_LIST_اقلام(list, li)
	{
	    إذا (li->li_tv.v_type != VAR_STRING)
		استمر;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    إذا (str.string == عدم)
		STR_LITERAL_مجموعه(str, "");
	    وإلا
		str.درازی = STRLEN(str.string);

	    إذا (مقصد_encoding != عدم)
	    {
		صحيح	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, مقصد_encoding, &converted);
		إذا (res != OK)
		{
		    semsg(_(e_str_encoding_مقصد_failed), مقصد_encoding);
		    اذهب انجامشده;
		}
		str.string = converted.string;
		str.درازی = converted.درازی;
	    }

	    إذا (li != list->lv_اولی)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    إذا (مقصد_encoding != عدم)
		vim_free(str.string);
	}
    }

انجامشده:
    إذا (مقصد_encoding != عدم)
	vim_free(مقصد_encoding);
    إذا (مقصد_encoding_raw != عدم)
	vim_free(مقصد_encoding_raw);
}

/*
 * "str2list()" function
 */
    فراغ
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحيح		utf8 = FALSE;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (صحيح)tv_get_bool_chk(&argvars[1], عدم);

    p = tv_get_string(&argvars[0]);

    إذا (has_mbyte || utf8)
    {
	صحيح (*ptr2len)(char_u *);
	صحيح (*ptr2char)(char_u *);

	إذا (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	وإلا
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	لكل ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_عدد(rettv->vval.v_list, (*ptr2char)(p));
    }
    وإلا
	لكل ( ; *p != NUL; ++p)
	    list_append_عدد(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    فراغ
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    صحيح		پایه = 10;
    char_u	*p;
    varnumber_T	n;
    صحيح		what = 0;
    صحيح		isneg;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	پایه = (صحيح)tv_get_عدد(&argvars[1]);
	إذا (پایه != 2 && پایه != 8 && پایه != 10 && پایه != 16)
	{
	    emsg(_(e_invalid_آرگومان));
	    أرجع;
	}
	إذا (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    إذا (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    بدل (پایه)
    {
	حالة 2: what |= STR2NR_BIN + STR2NR_FORCE; اكسر;
	حالة 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; اكسر;
	حالة 16: what |= STR2NR_HEX + STR2NR_FORCE; اكسر;
    }
    vim_str2nr(p, عدم, عدم, what, &n, عدم, 0, FALSE, عدم);
    // Text after the number is silently ignored.
    إذا (isneg)
	rettv->vval.v_عدد = -n;
    وإلا
	rettv->vval.v_عدد = n;

}

/*
 * "strgetchar()" function
 */
    فراغ
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    صحيح		len;
    صحيح		error = FALSE;
    صحيح		charidx;
    صحيح		byteidx = 0;

    rettv->vval.v_عدد = -1;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    str = tv_get_string_chk(&argvars[0]);
    إذا (str == عدم)
	أرجع;
    len = (صحيح)STRLEN(str);
    charidx = (صحيح)tv_get_عدد_chk(&argvars[1], &error);
    إذا (error)
	أرجع;

    طالما (charidx >= 0 && byteidx < len)
    {
	إذا (charidx == 0)
	{
	    rettv->vval.v_عدد = mb_ptr2char(str + byteidx);
	    اكسر;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    فراغ
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    صحيح		شروع_idx;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL))
	أرجع;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_عدد = -1;
    إذا (needle == عدم || haystack == عدم)
	أرجع;		// type error; errmsg already given

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	صحيح	    error = FALSE;

	شروع_idx = (صحيح)tv_get_عدد_chk(&argvars[2], &error);
	إذا (error || شروع_idx >= (صحيح)STRLEN(haystack))
	    أرجع;
	إذا (شروع_idx >= 0)
	    haystack += شروع_idx;
    }

    pos	= (char_u *)جستجوی_رشته((حرف *)haystack, (حرف *)needle);
    إذا (pos != عدم)
	rettv->vval.v_عدد = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    فراغ
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    إذا (rettv->vval.v_string != عدم && tofree == عدم)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    فراغ
f_strlen(typval_T *argvars, typval_T *rettv)
{
    إذا (درون_vim9script()
	    && check_for_string_or_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_عدد = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    ساكن فراغ
strchar_common(typval_T *argvars, typval_T *rettv, صحيح skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    صحيح			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    طالما (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_عدد = len;
}

/*
 * "strcharlen()" function
 */
    فراغ
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    إذا (درون_vim9script()
	    && check_for_string_or_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    فراغ
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	صحيح error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	إذا (error)
	    أرجع;
	إذا (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_عدد_as_bool_nr), skipcc);
	    أرجع;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    فراغ
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;

    إذا (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	أرجع;

    varnumber_T countcc = FALSE;
    إذا (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    صحيح			(*func_mb_ptr2char_adv)(char_u **pp);
    صحيح			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    طالما (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	إذا (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_عدد = len;
}

/*
 * "strdisplaywidth()" function
 */
    فراغ
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    صحيح		ستون = 0;

    rettv->vval.v_عدد = -1;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    s = tv_get_string(&argvars[0]);
    إذا (argvars[1].v_type != VAR_UNKNOWN)
	ستون = (صحيح)tv_get_عدد(&argvars[1]);

    rettv->vval.v_عدد = (varnumber_T)(linetabsize_ستون(ستون, s) - ستون);
}

/*
 * "strwidth()" function
 */
    فراغ
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_عدد = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    فراغ
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحيح		nchar;
    صحيح		nbyte = 0;
    صحيح		charlen;
    صحيح		skipcc = FALSE;
    صحيح		len = 0;
    صحيح		slen;
    صحيح		error = FALSE;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	أرجع;

    p = tv_get_string(&argvars[0]);
    slen = (صحيح)STRLEN(p);

    nchar = (صحيح)tv_get_عدد_chk(&argvars[1], &error);
    إذا (!error)
    {
	إذا (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    إذا (error)
		أرجع;
	    إذا (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_عدد_as_bool_nr), skipcc);
		أرجع;
	    }
	}

	إذا (nchar > 0)
	    طالما (nchar > 0 && nbyte < slen)
	    {
		إذا (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		وإلا
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	وإلا
	    nbyte = nchar;
	إذا (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (صحيح)tv_get_عدد(&argvars[2]);
	    طالما (charlen > 0 && nbyte + len < slen)
	    {
		صحيح off = nbyte + len;

		إذا (off < 0)
		    len += 1;
		وإلا
		{
		    إذا (skipcc)
			len += mb_ptr2len(p + off);
		    وإلا
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	وإلا
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    إذا (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    وإلا إذا (nbyte > slen)
	nbyte = slen;
    إذا (len < 0)
	len = 0;
    وإلا إذا (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    فراغ
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحيح		n;
    صحيح		len;
    صحيح		slen;
    صحيح		error = FALSE;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	أرجع;

    p = tv_get_string(&argvars[0]);
    slen = (صحيح)STRLEN(p);

    n = (صحيح)tv_get_عدد_chk(&argvars[1], &error);
    إذا (error)
	len = 0;
    وإلا إذا (argvars[2].v_type != VAR_UNKNOWN)
	len = (صحيح)tv_get_عدد(&argvars[2]);
    وإلا
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    إذا (n < 0)
    {
	len += n;
	n = 0;
    }
    وإلا إذا (n > slen)
	n = slen;
    إذا (len < 0)
	len = 0;
    وإلا إذا (n + len > slen)
	len = slen - n;

    إذا (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	صحيح off;

	// length in characters
	لكل (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    فراغ
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = عدم;
    صحيح		haystack_len, انتها_idx;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL))
	أرجع;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_عدد = -1;
    إذا (needle == عدم || haystack == عدم)
	أرجع;		// type error; errmsg already given

    haystack_len = (صحيح)STRLEN(haystack);
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	انتها_idx = (صحيح)tv_get_عدد_chk(&argvars[2], عدم);
	إذا (انتها_idx < 0)
	    أرجع;	// can never find a match
    }
    وإلا
	انتها_idx = haystack_len;

    إذا (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + انتها_idx;
    }
    وإلا
    {
	لكل (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)جستجوی_رشته((حرف *)rest, (حرف *)needle);
	    إذا (rest == عدم || rest > haystack + انتها_idx)
		اكسر;
	    lastmatch = rest;
	}
    }

    إذا (lastmatch == عدم)
	rettv->vval.v_عدد = -1;
    وإلا
	rettv->vval.v_عدد = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    فراغ
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    فراغ
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;

    إذا (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_عدد_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    أرجع;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_عدد_chk(&argvars[1], عدم);
    إذا (str == عدم || idx < 0)
	أرجع;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	إذا (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    صحيح (*ptr2len)(char_u *);
    إذا (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    وإلا
	ptr2len = mb_ptr2len;

    char_u	*p;
    صحيح		len;
    صحيح		utf16idx = 0;
    لكل (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	إذا (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    إذا (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_عدد = len;
	    أرجع;
	}
	utf16idx = len;
	صحيح clen = ptr2len(p);
	صحيح c = (clen > 1) ? utf_ptr2char(p) : *p;
	إذا (c > 0xFFFF)
	    len++;
	p += clen;
	إذا (charidx)
	    idx--;
    }

    rettv->vval.v_عدد = utf16idx;
}

/*
 * "tolower(string)" function
 */
    فراغ
f_tolower(typval_T *argvars, typval_T *rettv)
{
    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    فراغ
f_toupper(typval_T *argvars, typval_T *rettv)
{
    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    فراغ
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*درون_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    صحيح		inlen;
    صحيح		fromlen;
    صحيح		tolen;
    صحيح		idx;
    char_u	*cpstr;
    صحيح		cplen;
    صحيح		اولی = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	أرجع;

    درون_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
    إذا (fromstr == عدم || tostr == عدم)
	    أرجع;		// type error; errmsg already given
    ga_init2(&ga, حجم(حرف), 80);

    إذا (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	إذا (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_آرگومان_str), fromstr);
	    ga_clear(&ga);
	    أرجع;
	}

    // fromstr and tostr have to contain the same number of chars
    طالما (*درون_str != NUL)
    {
	إذا (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(درون_str);
	    cpstr = درون_str;
	    cplen = inlen;
	    idx = 0;
	    لكل (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		إذا (fromlen == inlen && STRNCMP(درون_str, p, inlen) == 0)
		{
		    لكل (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			إذا (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    اكسر;
			}
		    }
		    إذا (*p == NUL)	// tostr is shorter than fromstr
			اذهب error;
		    اكسر;
		}
		++idx;
	    }

	    إذا (اولی && cpstr == درون_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		اولی = FALSE;
		لكل (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		إذا (idx != 0)
		    اذهب error;
	    }

	    (فراغ)ga_grow(&ga, cplen);
	    mch_memmove((حرف *)ga.ga_داده + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    درون_str += inlen;
	}
	وإلا
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *درون_str);
	    إذا (p != عدم)
		ga_append(&ga, tostr[p - fromstr]);
	    وإلا
		ga_append(&ga, *درون_str);
	    ++درون_str;
	}
    }

    // add a terminating NUL
    (فراغ)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_داده;
}

/*
 * "trim({expr})" function
 */
    فراغ
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*سر;
    char_u	*ماسک = عدم;
    char_u	*دم;
    char_u	*prev;
    char_u	*p;
    صحيح		c1;
    صحيح		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 2) == FAIL)))
	أرجع;

    سر = tv_get_string_buf_chk(&argvars[0], buf1);
    إذا (سر == عدم)
	أرجع;

    إذا (check_for_opt_string_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (argvars[1].v_type == VAR_STRING)
    {
	ماسک = tv_get_string_buf_chk(&argvars[1], buf2);
	إذا (*ماسک == NUL)
	    ماسک = عدم;

	إذا (argvars[2].v_type != VAR_UNKNOWN)
	{
	    صحيح	error = 0;

	    // leading or trailing characters to trim
	    dir = (صحيح)tv_get_عدد_chk(&argvars[2], &error);
	    إذا (error)
		أرجع;
	    إذا (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_آرگومان_str), tv_get_string(&argvars[2]));
		أرجع;
	    }
	}
    }

    إذا (dir == 0 || dir == 1)
    {
	// Trim leading characters
	طالما (*سر != NUL)
	{
	    c1 = PTR2CHAR(سر);
	    إذا (ماسک == عدم)
	    {
		إذا (c1 > ' ' && c1 != 0xa0)
		    اكسر;
	    }
	    وإلا
	    {
		لكل (p = ماسک; *p != NUL; MB_PTR_ADV(p))
		    إذا (c1 == PTR2CHAR(p))
			اكسر;
		إذا (*p == NUL)
		    اكسر;
	    }
	    MB_PTR_ADV(سر);
	}
    }

    دم = سر + STRLEN(سر);
    إذا (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	لكل (; دم > سر; دم = prev)
	{
	    prev = دم;
	    MB_PTR_پشت(سر, prev);
	    c1 = PTR2CHAR(prev);
	    إذا (ماسک == عدم)
	    {
		إذا (c1 > ' ' && c1 != 0xa0)
		    اكسر;
	    }
	    وإلا
	    {
		لكل (p = ماسک; *p != NUL; MB_PTR_ADV(p))
		    إذا (c1 == PTR2CHAR(p))
			اكسر;
		إذا (*p == NUL)
		    اكسر;
	    }
	}
    }
    rettv->vval.v_string = vim_strnsave(سر, دم - سر);
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
    ساكن char_u *
uri_decode(char_u *str)
{
    إذا (str == عدم)
	أرجع عدم;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    إذا (!decoded)
	أرجع عدم;

    char_u	*p = decoded;
    size_t	i = 0;

    طالما (i < len)
    {
	إذا (str[i] == '%')
	{
	    إذا (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		إذا (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    وإلا
	    {
		صحيح val = hexhex2nr(&str[i + 1]);
		إذا (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		وإلا
		{
		    // invalid hex digits following "%"
		    لكل (صحيح j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	وإلا
	    *p++ = str[i++];
    }

    *p = NUL;

    أرجع decoded;
}

/*
 * "uri_decode({str})" function
 */
    فراغ
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

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
    ساكن char_u *
uri_encode(char_u *str)
{
    إذا (str == عدم)
	أرجع عدم;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    إذا (encoded == عدم)
	أرجع عدم;

    char_u *p = encoded;

    لكل (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	إذا (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	وإلا
	{
	    sprintf((حرف *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    أرجع encoded;
}

/*
 * "uri_encode({str})" function
 */
    فراغ
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

ساكن حرف *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ساكن varnumber_T
tv_nr(typval_T *tvs, صحيح *idxp)
{
    صحيح		idx = *idxp - 1;
    varnumber_T	n = 0;
    صحيح		err = FALSE;

    إذا (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    وإلا
    {
	++*idxp;
	n = tv_get_عدد_chk(&tvs[idx], &err);
	إذا (err)
	    n = 0;
    }
    أرجع n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    ساكن حرف *
tv_str(typval_T *tvs, صحيح *idxp, char_u **tofree)
{
    صحيح		    idx = *idxp - 1;
    حرف	    *s = عدم;
    ساكن char_u   numbuf[NUMBUFLEN];

    إذا (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    وإلا
    {
	++*idxp;
	إذا (tofree != عدم)
	    s = (حرف *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	وإلا
	    s = (حرف *)tv_get_string_chk(&tvs[idx]);
    }
    أرجع s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ساكن مزدوج
tv_float(typval_T *tvs, صحيح *idxp)
{
    صحيح		idx = *idxp - 1;
    مزدوج	f = 0;

    إذا (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    وإلا
    {
	++*idxp;
	إذا (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	وإلا إذا (tvs[idx].v_type == VAR_عدد)
	    f = (مزدوج)tvs[idx].vval.v_عدد;
	وإلا
	    emsg(_(e_expected_float_آرگومان_for_printf));
    }
    أرجع f;
}

#پایان_اگر

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    ساكن ثابت حرف *
infinity_str(صحيح مثبت,
	     حرف fmt_spec,
	     صحيح force_sign,
	     صحيح space_for_مثبت)
{
    ساكن ثابت حرف *جدول[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    صحيح idx = مثبت * (1 + force_sign + force_sign * space_for_مثبت);

    إذا (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    أرجع جدول[idx];
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
#اگر_تعریف_نشده PROTO

// Like vim_vsnprintf() but append to the string.
    صحيح
vim_snprintf_جمع_کن(حرف *str, size_t str_m, ثابت حرف *fmt, ...)
{
    va_list	ap;
    صحيح		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    إذا (str_m <= len)
	space = 0;
    وإلا
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    أرجع str_l;
}

    صحيح
vim_snprintf(حرف *str, size_t str_m, ثابت حرف *fmt, ...)
{
    va_list	ap;
    صحيح		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    أرجع str_l;
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
vim_snprintf_safelen(حرف *str, size_t str_m, ثابت حرف *fmt, ...)
{
    va_list ap;
    صحيح	    str_l;

    إذا (str_m == 0)
	أرجع 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, عدم);
    va_end(ap);

    إذا (str_l < 0)
    {
	*str = NUL;
	أرجع 0;
    }
    أرجع ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    صحيح
vim_vsnprintf(
    حرف	*str,
    size_t	str_m,
    ثابت حرف	*fmt,
    va_list	ap)
{
    أرجع vim_vsnprintf_typval(str, str_m, fmt, ap, عدم);
}

تعداد
{
    TYPE_UNKNOWN = -1,
    TYPE_INT,
    TYPE_LONGINT,
    TYPE_LONGLONGINT,
    TYPE_UNSIGNEDINT,
    TYPE_UNSIGNEDLONGINT,
    TYPE_UNSIGNEDLONGLONGINT,
    TYPE_اشاره_گر,
    TYPE_PERCENT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_FLOAT
};

/* Types that can be used in a format string
 */
    ساكن صحيح
format_typeof(
    ثابت حرف	*type)
{
    // allowed values: \0, h, l, L
    حرف    درازی_modifier = '\0';

    // current conversion specifier character
    حرف    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    إذا (*type == 'h' || *type == 'l')
    {
	درازی_modifier = *type;
	type++;
	إذا (درازی_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    درازی_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    بدل (fmt_spec)
    {
	حالة 'i': fmt_spec = 'd'; اكسر;
	حالة '*': fmt_spec = 'd'; درازی_modifier = 'h'; اكسر;
	حالة 'D': fmt_spec = 'd'; درازی_modifier = 'l'; اكسر;
	حالة 'U': fmt_spec = 'u'; درازی_modifier = 'l'; اكسر;
	حالة 'O': fmt_spec = 'o'; درازی_modifier = 'l'; اكسر;
	افتراضي: اكسر;
    }

    // get parameter value, do initial processing
    بدل (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    حالة '%':
	أرجع TYPE_PERCENT;

    حالة 'c':
	أرجع TYPE_CHAR;

    حالة 's':
    حالة 'S':
	أرجع TYPE_STRING;

    حالة 'd': حالة 'u':
    حالة 'b': حالة 'B':
    حالة 'o':
    حالة 'x': حالة 'X':
    حالة 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //مقدار

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    إذا (fmt_spec == 'p')
		أرجع TYPE_اشاره_گر;
	    وإلا إذا (fmt_spec == 'b' || fmt_spec == 'B')
		أرجع TYPE_UNSIGNEDLONGLONGINT;
	    وإلا إذا (fmt_spec == 'd')
	    {
		// signed
		بدل (درازی_modifier)
		{
		حالة '\0':
		حالة 'h':
		    // char and short arguments are passed as int.
		    أرجع TYPE_INT;
		حالة 'l':
		    أرجع TYPE_LONGINT;
		حالة 'L':
		    أرجع TYPE_LONGLONGINT;
		}
	    }
	    وإلا
	    {
		// unsigned
		بدل (درازی_modifier)
		{
		    حالة '\0':
		    حالة 'h':
			أرجع TYPE_UNSIGNEDINT;
		    حالة 'l':
			أرجع TYPE_UNSIGNEDLONGINT;
		    حالة 'L':
			أرجع TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	اكسر;

    حالة 'f':
    حالة 'F':
    حالة 'e':
    حالة 'E':
    حالة 'g':
    حالة 'G':
	أرجع TYPE_FLOAT;
    }

    أرجع TYPE_UNKNOWN;
}

    ساكن حرف *
format_typename(
    ثابت حرف  *type)
{
    بدل (format_typeof(type))
    {
	حالة TYPE_INT:
	    أرجع typename_int;

	حالة TYPE_LONGINT:
	    أرجع typename_longint;

	حالة TYPE_LONGLONGINT:
	    أرجع typename_longlongint;

	حالة TYPE_UNSIGNEDINT:
	    أرجع typename_unsignedint;

	حالة TYPE_UNSIGNEDLONGINT:
	    أرجع typename_unsignedlongint;

	حالة TYPE_UNSIGNEDLONGLONGINT:
	    أرجع typename_unsignedlonglongint;

	حالة TYPE_اشاره_گر:
	    أرجع _(typename_اشاره_گر);

	حالة TYPE_PERCENT:
	    أرجع _(typename_percent);

	حالة TYPE_CHAR:
	    أرجع typename_char;

	حالة TYPE_STRING:
	    أرجع _(typename_string);

	حالة TYPE_FLOAT:
	    أرجع typename_float;
    }

    أرجع _(typename_unknown);
}

    ساكن صحيح
adjust_types(
    ثابت حرف ***ap_types,
    صحيح arg,
    صحيح *رقم_posarg,
    ثابت حرف *type)
{
    إذا (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	أرجع FAIL;
    }

    إذا (*ap_types == عدم || *رقم_posarg < arg)
    {
	صحيح	    idx;
	ثابت حرف  **new_types;

	إذا (*ap_types == عدم)
	    new_types = ALLOC_CLEAR_MULT(ثابت حرف *, arg);
	وإلا
	    new_types = vim_realloc((حرف **)*ap_types,
						arg * حجم(ثابت حرف *));

	إذا (new_types == عدم)
	    أرجع FAIL;

	لكل (idx = *رقم_posarg; idx < arg; ++idx)
	    new_types[idx] = عدم;

	*ap_types = new_types;
	*رقم_posarg = arg;
    }

    إذا ((*ap_types)[arg - 1] != عدم)
    {
	إذا ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    ثابت حرف *pt = type;
	    إذا (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    إذا (pt[0] != '*')
	    {
		بدل (pt[0])
		{
		    حالة 'd':
		    حالة 'i': اكسر;
		    افتراضي:
			semsg(_(e_positional_رقم_فیلد_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			أرجع FAIL;
		}
	    }
	}
	وإلا
	{
	    إذا (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_رقم_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		أرجع FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    أرجع OK;
}

    ساكن فراغ
format_سرریز_error(ثابت حرف *pstart)
{
    size_t	arglen = 0;
    حرف	*argcopy = عدم;
    ثابت حرف	*p = pstart;

    طالما (VIM_ISDIGIT((صحيح)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(حرف, arglen + 1);
    إذا (argcopy != عدم)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	حرر(argcopy);
    }
    وإلا
	semsg(_(e_بیرون_از_حافظه_allocating_nr_بایتها), arglen);
}

# define MAX_ALLOWED_STRING_عرض 1048576    // 1 MiB

    ساكن صحيح
get_unsigned_int(
    ثابت حرف *pstart,
    ثابت حرف **p,
    بیعلامت صحيح *uj,
    صحيح سرریز_err)
{
    *uj = **p - '0';
    ++*p;

    طالما (VIM_ISDIGIT((صحيح)(**p)) && *uj < MAX_ALLOWED_STRING_عرض)
    {
	*uj = 10 * *uj + (بیعلامت صحيح)(**p - '0');
	++*p;
    }

    إذا (*uj > MAX_ALLOWED_STRING_عرض)
    {
	إذا (سرریز_err)
	{
	    format_سرریز_error(pstart);
	    أرجع FAIL;
	}
	وإلا
	    *uj = MAX_ALLOWED_STRING_عرض;
    }

    أرجع OK;
}


    ساكن صحيح
parse_fmt_types(
    ثابت حرف  ***ap_types,
    صحيح		*رقم_posarg,
    ثابت حرف  *fmt,
    typval_T	*tvs استفادهنشده
    )
{
    ثابت حرف	*p = fmt;
    ثابت حرف	*arg = عدم;

    صحيح		any_pos = 0;
    صحيح		any_arg = 0;
    صحيح		arg_idx;

# define CHECK_POS_ARG افعل { \
    إذا (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	اذهب error; \
    } \
} طالما (0);

    إذا (p == عدم)
	أرجع OK;

    طالما (*p != NUL)
    {
	إذا (*p != '%')
	{
	    ثابت حرف    *q = strchr(p + 1, '%');
	    size_t  n = (q == عدم) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	وإلا
	{
	    // allowed values: \0, h, l, L
	    حرف	درازی_modifier = '\0';

	    // variable for positional arg
	    صحيح		pos_arg = -1;
	    ثابت حرف	*ptype = عدم;
	    ثابت حرف	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    طالما (VIM_ISDIGIT(*ptype))
		++ptype;

	    إذا (*ptype == '$')
	    {
		إذا (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    اذهب error;
		}

		// Positional argument
		بیعلامت صحيح uj;

		إذا (get_unsigned_int(pstart, &p, &uj, tvs != عدم) == FAIL)
		    اذهب error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    طالما (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		بدل (*p)
		{
		    حالة '0': اكسر;
		    حالة '-': اكسر;
		    حالة '+': اكسر;
		    حالة ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      اكسر;
		    حالة '#': اكسر;
		    حالة '\'': اكسر;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    إذا (*(arg = p) == '*')
	    {
		p++;

		إذا (VIM_ISDIGIT((صحيح)(*p)))
		{
		    // Positional argument field width
		    بیعلامت صحيح uj;

		    إذا (get_unsigned_int(arg + 1, &p, &uj, tvs != عدم) == FAIL)
			اذهب error;

		    إذا (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			اذهب error;
		    }
		    وإلا
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			إذا (adjust_types(ap_types, uj, رقم_posarg, arg) == FAIL)
			    اذهب error;
		    }
		}
		وإلا
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    وإلا إذا (VIM_ISDIGIT((صحيح)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ثابت حرف *digstart = p;
		بیعلامت صحيح uj;

		إذا (get_unsigned_int(digstart, &p, &uj, tvs != عدم) == FAIL)
		    اذهب error;

		إذا (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    اذهب error;
		}
	    }

	    // parse precision
	    إذا (*p == '.')
	    {
		p++;

		إذا (*(arg = p) == '*')
		{
		    p++;

		    إذا (VIM_ISDIGIT((صحيح)(*p)))
		    {
			// Parse precision
			بیعلامت صحيح uj;

			إذا (get_unsigned_int(arg + 1, &p, &uj, tvs != عدم) == FAIL)
			    اذهب error;

			إذا (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    إذا (adjust_types(ap_types, uj, رقم_posarg, arg) == FAIL)
				اذهب error;
			}
			وإلا
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    اذهب error;
			}
		    }
		    وإلا
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		وإلا إذا (VIM_ISDIGIT((صحيح)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ثابت حرف *digstart = p;
		    بیعلامت صحيح uj;

		    إذا (get_unsigned_int(digstart, &p, &uj, tvs != عدم) == FAIL)
			اذهب error;

		    إذا (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			اذهب error;
		    }
		}
	    }

	    إذا (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    إذا (*p == 'h' || *p == 'l')
	    {
		درازی_modifier = *p;
		p++;
		إذا (درازی_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    بدل (*p)
	    {
		// Check for known format specifiers. % is special!
		حالة 'i':
		حالة '*':
		حالة 'd':
		حالة 'u':
		حالة 'o':
		حالة 'D':
		حالة 'U':
		حالة 'O':
		حالة 'x':
		حالة 'X':
		حالة 'b':
		حالة 'B':
		حالة 'c':
		حالة 's':
		حالة 'S':
		حالة 'p':
		حالة 'f':
		حالة 'F':
		حالة 'e':
		حالة 'E':
		حالة 'g':
		حالة 'G':
		    إذا (pos_arg != -1)
		    {
			إذا (adjust_types(ap_types, pos_arg, رقم_posarg, ptype) == FAIL)
			    اذهب error;
		    }
		    وإلا
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    اكسر;

		افتراضي:
		    إذا (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			اذهب error;
		    }
	    }

	    إذا (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    لكل (arg_idx = 0; arg_idx < *رقم_posarg; ++arg_idx)
    {
	إذا ((*ap_types)[arg_idx] == عدم)
	{
	    semsg(_(e_fmt_arg_nr_استفادهنشده_str), arg_idx + 1, fmt);
	    اذهب error;
	}

# إذا defined(FEAT_EVAL)
	إذا (tvs != عدم && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_بیرون_از_bounds_str), arg_idx + 1, fmt);
	    اذهب error;
	}
# endif
    }

    أرجع OK;

error:
    vim_free((حرف**)*ap_types);
    *ap_types = عدم;
    *رقم_posarg = 0;
    أرجع FAIL;
}

    ساكن فراغ
skip_مقصد_arg(
    ثابت حرف	**ap_types,
    va_list	ap_شروع,
    va_list	*ap,
    صحيح		*arg_idx,
    صحيح		*arg_cur,
    ثابت حرف	*fmt)
{
    صحيح		arg_min = 0;

    إذا (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	أرجع;
    }

    إذا (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_شروع);
    }
    وإلا
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    لكل (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	ثابت حرف *p;

	إذا (ap_types == عدم || ap_types[*arg_cur] == عدم)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    أرجع;
	}

	p = ap_types[*arg_cur];

	صحيح fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	بدل (fmt_type)
	{
	حالة TYPE_PERCENT:
	حالة TYPE_UNKNOWN:
	    اكسر;

	حالة TYPE_CHAR:
	    va_arg(*ap, صحيح);
	    اكسر;

	حالة TYPE_STRING:
	    va_arg(*ap, حرف *);
	    اكسر;

	حالة TYPE_اشاره_گر:
	    va_arg(*ap, فراغ *);
	    اكسر;

	حالة TYPE_INT:
	    va_arg(*ap, صحيح);
	    اكسر;

	حالة TYPE_LONGINT:
	    va_arg(*ap, دراز صحيح);
	    اكسر;

	حالة TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    اكسر;

	حالة TYPE_UNSIGNEDINT:
	    va_arg(*ap, بیعلامت صحيح);
	    اكسر;

	حالة TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, بیعلامت دراز صحيح);
	    اكسر;

	حالة TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    اكسر;

	حالة TYPE_FLOAT:
	    va_arg(*ap, مزدوج);
	    اكسر;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    أرجع;
}

    صحيح
vim_vsnprintf_typval(
    حرف	*str,
    size_t	str_m,
    ثابت حرف	*fmt,
    va_list	ap_شروع,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    ثابت حرف	*p = fmt;
    صحيح		arg_cur = 0;
    صحيح		رقم_posarg = 0;
    صحيح		arg_idx = 1;
    va_list	ap;
    ثابت حرف	**ap_types = عدم;

    إذا (parse_fmt_types(&ap_types, &رقم_posarg, fmt, tvs) == FAIL)
	أرجع 0;

    va_copy(ap, ap_شروع);

    إذا (p == عدم)
	p = "";
    طالما (*p != NUL)
    {
	إذا (*p != '%')
	{
	    ثابت حرف    *q = strchr(p + 1, '%');
	    size_t  n = (q == عدم) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    إذا (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	وإلا
	{
	    size_t  min_فیلد_عرض = 0, precision = 0;
	    صحيح	    zero_padding = 0, precision_specified = 0, justify_چپ = 0;
	    صحيح	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    صحيح	    space_for_مثبت = 1;

	    // allowed values: \0, h, l, L
	    حرف    درازی_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    حرف    tmp[TMP_LEN];

	    // string address in case of string argument
	    ثابت حرف  *str_arg = عدم;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    بیعلامت حرف uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  عدد_از_zeros_مقصد_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    حرف    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = عدم;

	    // variables for positional arg
	    صحيح	    pos_arg = -1;
	    ثابت حرف	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    طالما (VIM_ISDIGIT(*ptype))
		++ptype;

	    إذا (*ptype == '$')
	    {
		// Positional argument
		ثابت حرف *digstart = p;
		بیعلامت صحيح uj;

		إذا (get_unsigned_int(digstart, &p, &uj, tvs != عدم) == FAIL)
		    اذهب error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    طالما (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		بدل (*p)
		{
		    حالة '0': zero_padding = 1; اكسر;
		    حالة '-': justify_چپ = 1; اكسر;
		    حالة '+': force_sign = 1; space_for_مثبت = 0; اكسر;
		    حالة ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      اكسر;
		    حالة '#': alternate_form = 1; اكسر;
		    حالة '\'': اكسر;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    إذا (*p == '*')
	    {
		صحيح j;
		ثابت حرف *digstart = p + 1;

		p++;

		إذا (VIM_ISDIGIT((صحيح)(*p)))
		{
		    // Positional argument field width
		    بیعلامت صحيح uj;

		    إذا (get_unsigned_int(digstart, &p, &uj, tvs != عدم) == FAIL)
			اذهب error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# إذا defined(FEAT_EVAL)
		    tvs != عدم ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, صحيح));

		إذا (j > MAX_ALLOWED_STRING_عرض)
		{
		    إذا (tvs != عدم)
		    {
			format_سرریز_error(digstart);
			اذهب error;
		    }
		    وإلا
			j = MAX_ALLOWED_STRING_عرض;
		}

		إذا (j >= 0)
		    min_فیلد_عرض = j;
		وإلا
		{
		    min_فیلد_عرض = -j;
		    justify_چپ = 1;
		}
	    }
	    وإلا إذا (VIM_ISDIGIT((صحيح)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ثابت حرف *digstart = p;
		بیعلامت صحيح uj;

		إذا (get_unsigned_int(digstart, &p, &uj, tvs != عدم) == FAIL)
		    اذهب error;

		min_فیلد_عرض = uj;
	    }

	    // parse precision
	    إذا (*p == '.')
	    {
		p++;
		precision_specified = 1;

		إذا (VIM_ISDIGIT((صحيح)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ثابت حرف *digstart = p;
		    بیعلامت صحيح uj;

		    إذا (get_unsigned_int(digstart, &p, &uj, tvs != عدم) == FAIL)
			اذهب error;

		    precision = uj;
		}
		وإلا إذا (*p == '*')
		{
		    صحيح j;
		    ثابت حرف *digstart = p;

		    p++;

		    إذا (VIM_ISDIGIT((صحيح)(*p)))
		    {
			// positional argument
			بیعلامت صحيح uj;

			إذا (get_unsigned_int(digstart, &p, &uj, tvs != عدم) == FAIL)
			    اذهب error;

			arg_idx = uj;

			++p;
		    }

		    j =
# إذا defined(FEAT_EVAL)
			tvs != عدم ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, صحيح));

		    إذا (j > MAX_ALLOWED_STRING_عرض)
		    {
			إذا (tvs != عدم)
			{
			    format_سرریز_error(digstart);
			    اذهب error;
			}
			وإلا
			    j = MAX_ALLOWED_STRING_عرض;
		    }

		    إذا (j >= 0)
			precision = j;
		    وإلا
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    إذا (*p == 'h' || *p == 'l')
	    {
		درازی_modifier = *p;
		p++;
		إذا (درازی_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    درازی_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    بدل (fmt_spec)
	    {
		حالة 'i': fmt_spec = 'd'; اكسر;
		حالة 'D': fmt_spec = 'd'; درازی_modifier = 'l'; اكسر;
		حالة 'U': fmt_spec = 'u'; درازی_modifier = 'l'; اكسر;
		حالة 'O': fmt_spec = 'o'; درازی_modifier = 'l'; اكسر;
		افتراضي: اكسر;
	    }

# إذا defined(FEAT_EVAL)
	    بدل (fmt_spec)
	    {
		حالة 'd': حالة 'u': حالة 'o': حالة 'x': حالة 'X':
		    إذا (tvs != عدم && درازی_modifier == '\0')
			درازی_modifier = 'L';
	    }
# endif

	    إذا (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    بدل (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    حالة '%':
	    حالة 'c':
	    حالة 's':
	    حالة 'S':
		str_arg_l = 1;
		بدل (fmt_spec)
		{
		حالة '%':
		    str_arg = p;
		    اكسر;

		حالة 'c':
		    {
			صحيح j;

			j =
# إذا defined(FEAT_EVAL)
			    tvs != عدم ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, صحيح));

			// standard demands unsigned char
			uchar_arg = (بیعلامت حرف)j;
			str_arg = (حرف *)&uchar_arg;
			اكسر;
		    }

		حالة 's':
		حالة 'S':
		    str_arg =
# إذا defined(FEAT_EVAL)
				tvs != عدم ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, حرف *));

		    إذا (str_arg == عدم)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    وإلا إذا (!precision_specified)
			str_arg_l = طول_رشته(str_arg);
		    // truncate string if necessary as requested by precision
		    وإلا إذا (precision == 0)
			str_arg_l = 0;
		    وإلا
		    {
			// memchr on HP does not like n > 2^31  !!!
			ثابت حرف *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == عدم) ? precision
						      : (size_t)(q - str_arg);
		    }
		    إذا (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			صحيح	cell;

			لكل (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    إذا (precision_specified && i + cell > precision)
				اكسر;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			إذا (min_فیلد_عرض != 0)
			    min_فیلد_عرض += str_arg_l - i;
		    }
		    اكسر;

		افتراضي:
		    اكسر;
		}
		اكسر;

	    حالة 'd': حالة 'u':
	    حالة 'b': حالة 'B':
	    حالة 'o':
	    حالة 'x': حالة 'X':
	    حالة 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //مقدار

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    صحيح arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    صحيح int_arg = 0;
		    بیعلامت صحيح uint_arg = 0;

		    // only set for length modifier l
		    دراز صحيح long_arg = 0;
		    بیعلامت دراز صحيح ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    فراغ *ptr_arg = عدم;

		    إذا (fmt_spec == 'p')
		    {
			درازی_modifier = '\0';
			ptr_arg =
# إذا defined(FEAT_EVAL)
				 tvs != عدم ? (فراغ *)tv_str(tvs, &arg_idx,
									عدم) :
# endif
					(skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, فراغ *));

			إذا (ptr_arg != عدم)
			    arg_sign = 1;
		    }
		    وإلا إذا (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# إذا defined(FEAT_EVAL)
				    tvs != عدم ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			إذا (bin_arg != 0)
			    arg_sign = 1;
		    }
		    وإلا إذا (fmt_spec == 'd')
		    {
			// signed
			بدل (درازی_modifier)
			{
			حالة '\0':
			حالة 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# إذا defined(FEAT_EVAL)
					tvs != عدم ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, صحيح));

			    إذا (int_arg > 0)
				arg_sign =  1;
			    وإلا إذا (int_arg < 0)
				arg_sign = -1;
			    اكسر;
			حالة 'l':
			    long_arg =
# إذا defined(FEAT_EVAL)
					tvs != عدم ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, دراز صحيح));

			    إذا (long_arg > 0)
				arg_sign =  1;
			    وإلا إذا (long_arg < 0)
				arg_sign = -1;
			    اكسر;
			حالة 'L':
			    llong_arg =
# إذا defined(FEAT_EVAL)
					tvs != عدم ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    إذا (llong_arg > 0)
				arg_sign =  1;
			    وإلا إذا (llong_arg < 0)
				arg_sign = -1;
			    اكسر;
			}
		    }
		    وإلا
		    {
			// unsigned
			بدل (درازی_modifier)
			{
			    حالة '\0':
			    حالة 'h':
				uint_arg =
# إذا defined(FEAT_EVAL)
					    tvs != عدم ? (بیعلامت)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, بیعلامت صحيح));

				إذا (uint_arg != 0)
				    arg_sign = 1;
				اكسر;
			    حالة 'l':
				ulong_arg =
# إذا defined(FEAT_EVAL)
					    tvs != عدم ? (بیعلامت دراز)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, بیعلامت دراز صحيح));

				إذا (ulong_arg != 0)
				    arg_sign = 1;
				اكسر;
			    حالة 'L':
				ullong_arg =
# إذا defined(FEAT_EVAL)
					    tvs != عدم ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				إذا (ullong_arg != 0)
				    arg_sign = 1;
				اكسر;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    إذا (precision_specified)
			zero_padding = 0;
		    إذا (fmt_spec == 'd')
		    {
			إذا (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_مثبت ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    وإلا إذا (alternate_form)
		    {
			إذا (arg_sign != 0
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
		    إذا (!precision_specified)
			precision = 1;   // default precision is 1
		    إذا (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    وإلا
		    {
			حرف	f[6];
			صحيح	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			إذا (!درازی_modifier)
			    ;
			وإلا إذا (درازی_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# وإلا
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			وإلا
			    f[f_l++] = درازی_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			إذا (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			وإلا إذا (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    حرف	    b[8 * حجم(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    افعل
			    {
				b[حجم(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    طالما (bn != 0);

			    کپی_حافظه(tmp + str_arg_l, b + حجم(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			وإلا إذا (fmt_spec == 'd')
			{
			    // signed
			    بدل (درازی_modifier)
			    {
			    حالة '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       اكسر;
			    حالة 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (کوتاه)int_arg);
				      اكسر;
			    حالة 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      اكسر;
			    حالة 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      اكسر;
			    }
			}
			وإلا
			{
			    // unsigned
			    بدل (درازی_modifier)
			    {
			    حالة '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       اكسر;
			    حالة 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(بیعلامت کوتاه)uint_arg);
				      اكسر;
			    حالة 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      اكسر;
			    حالة 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      اكسر;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			إذا (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			إذا (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t رقم_از_digits = str_arg_l
						 - zero_padding_insertion_ind;

			إذا (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    إذا (!precision_specified
					     || precision < رقم_از_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = رقم_از_digits + 1;
			    }
			}
			// zero padding to specified precision?
			إذا (رقم_از_digits < precision)
			    عدد_از_zeros_مقصد_pad = precision - رقم_از_digits;
		    }
		    // zero padding to specified minimal field width?
		    إذا (!justify_چپ && zero_padding)
		    {
			صحيح n = (صحيح)(min_فیلد_عرض - (str_arg_l
						    + عدد_از_zeros_مقصد_pad));
			إذا (n > 0)
			    عدد_از_zeros_مقصد_pad += n;
		    }
		    اكسر;
		}

	    حالة 'f':
	    حالة 'F':
	    حالة 'e':
	    حالة 'E':
	    حالة 'g':
	    حالة 'G':
		{
		    // Floating point.
		    مزدوج	f;
		    مزدوج	abs_f;
		    حرف	format[40];
		    صحيح		l;
		    صحيح		remove_trailing_zeroes = FALSE;

		    f =
# إذا defined(FEAT_EVAL)
			tvs != عدم ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_مقصد_arg(ap_types, ap_شروع, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, مزدوج));

		    abs_f = f < 0 ? -f : f;

		    إذا (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			إذا ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			وإلا
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    إذا ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# وإلا
			    abs_f > 1.0e307
# endif
			    )
		    {
			// Avoid a buffer overflow
			STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_مثبت));
			str_arg_l = STRLEN(tmp);
			zero_padding = 0;
		    }
		    وإلا
		    {
			إذا (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			وإلا إذا (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_مثبت));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			وإلا
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    إذا (force_sign)
				format[l++] = space_for_مثبت ? ' ' : '+';
			    إذا (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				إذا ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				إذا (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (صحيح)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			إذا (remove_trailing_zeroes)
			{
			    صحيح i;
			    حرف *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    إذا (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    وإلا
			    {
				tp = (حرف *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				إذا (tp != عدم)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    إذا (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    طالما (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    إذا (tp != عدم && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				طالما (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			وإلا
			{
			    حرف *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (حرف *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    إذا (tp != عدم && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    إذا (zero_padding && min_فیلد_عرض > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			عدد_از_zeros_مقصد_pad = min_فیلد_عرض - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    اكسر;
		}

	    افتراضي:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_چپ = 1;
		min_فیلد_عرض = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		إذا (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		اكسر;
	    }

	    إذا (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    إذا (!justify_چپ)
	    {
		// left padding with blank or zero
		صحيح pn = (صحيح)(min_فیلد_عرض - (str_arg_l + عدد_از_zeros_مقصد_pad));

		إذا (pn > 0)
		{
		    إذا (str_l < str_m)
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
	    إذا (عدد_از_zeros_مقصد_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    وإلا
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		صحيح zn = (صحيح)zero_padding_insertion_ind;

		إذا (zn > 0)
		{
		    إذا (str_l < str_m)
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
		zn = (صحيح)عدد_از_zeros_مقصد_pad;
		إذا (zn > 0)
		{
		    إذا (str_l < str_m)
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
		صحيح sn = (صحيح)(str_arg_l - zero_padding_insertion_ind);

		إذا (sn > 0)
		{
		    إذا (str_l < str_m)
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
	    إذا (justify_چپ)
	    {
		// right blank padding to the field width
		صحيح pn = (صحيح)(min_فیلد_عرض
				      - (str_arg_l + عدد_از_zeros_مقصد_pad));

		إذا (pn > 0)
		{
		    إذا (str_l < str_m)
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

    إذا (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    إذا (tvs != عدم && tvs[رقم_posarg != 0 ? رقم_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_مقصد_printf));

error:
    vim_free((حرف*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    أرجع (صحيح)str_l;
}

#پایان_اگر // PROTO
