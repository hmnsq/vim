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

#تعریف_کرو USING_FLOAT_STUFF
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
    اگر (p != تہی)
	mch_memmove(p, string, len);
    واپس p;
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
    اگر (p == تہی)
	واپس تہی;

    STRNCPY(p, string, len);
    p[len] = NUL;
    واپس p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    واپس vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    غیرعلامتی	لمبائی;
    صحیح		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    لمبائی = 1;				// count the trailing NUL
    برائے (p = string; *p; p++)
    {
	اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    لمبائی += l;		// count a multibyte char
	    p += l - 1;
	    جاری;
	}
	اگر (vim_strchr(esc_chars, *p) != تہی || (bsl && rem_backslash(p)))
	    ++لمبائی;			// count a backslash
	++لمبائی;			// count an ordinary char
    }
    escaped_string = alloc(لمبائی);
    اگر (escaped_string == تہی)
	واپس تہی;
    p2 = escaped_string;
    برائے (p = string; *p; p++)
    {
	اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    جاری;
	}
	اگر (vim_strchr(esc_chars, *p) != تہی || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    واپس escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    صحیح
csh_like_shell(خالی)
{
    واپس (لڑی_تلاش((حرف *)gettail(p_sh), "csh") != تہی);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    ساکن صحیح
fish_like_shell(خالی)
{
    واپس (لڑی_تلاش((حرف *)gettail(p_sh), "fish") != تہی);
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
    غیرعلامتی	لمبائی;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    صحیح		csh_like;
    صحیح		fish_like;
    char_u	*shname;
    صحیح		powershell;
#اگر_تعریف_شدہ MSWIN
    صحیح		double_quotes;
#اگر_ختم

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
    powershell = لڑی_تلاش((حرف *)shname, "pwsh") != تہی;
#اگر_تعریف_شدہ MSWIN
    powershell = powershell || لڑی_تلاش((حرف *)shname, "powershell") != تہی;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#اگر_ختم

    // First count the number of extra bytes required.
    لمبائی = (غیرعلامتی)STRLEN(string) + 3;  // two quotes and a trailing NUL
    برائے (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#اگر_تعریف_شدہ MSWIN
	اگر (double_quotes)
	{
	    اگر (*p == '"')
		++لمبائی;		// " -> ""
	}
	ورنہ
#اگر_ختم
	اگر (*p == '\'')
	{
	    اگر (powershell)
		لمبائی += 2;		// ' => ''
	    ورنہ
		لمبائی += 3;		// ' => '\''
	}
	اگر ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++لمبائی;			// insert backslash
	    اگر (csh_like && do_special)
		++لمبائی;		// insert backslash
	}
	اگر (do_special && ڈھونڈو_cmdline_var(p, &l) >= 0)
	{
	    ++لمبائی;			// insert backslash
	    p += l - 1;
	}
	اگر (*p == '\\' && fish_like)
	    ++لمبائی;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(لمبائی);
    اگر (escaped_string != تہی)
    {
	d = escaped_string;

	// add opening quote
#اگر_تعریف_شدہ MSWIN
	اگر (double_quotes)
	    *d++ = '"';
	ورنہ
#اگر_ختم
	    *d++ = '\'';

	برائے (p = string; *p != NUL; )
	{
#اگر_تعریف_شدہ MSWIN
	    اگر (double_quotes)
	    {
		اگر (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    جاری;
		}
	    }
	    ورنہ
#اگر_ختم
	    اگر (*p == '\'')
	    {
		اگر (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		ورنہ
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		جاری;
	    }
	    اگر ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		اگر (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		جاری;
	    }
	    اگر (do_special && ڈھونڈو_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		حافظہ_نقل(d, p, l);	// copy the var
		d += l;
		p += l;
		جاری;
	    }
	    اگر (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		جاری;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#اگر_تعریف_شدہ MSWIN
	اگر (double_quotes)
	    *d++ = '"';
	ورنہ
#اگر_ختم
	    *d++ = '\'';
	*d = NUL;
    }

    واپس escaped_string;
}

/*
 * Like vim_strsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strsave_اوپر(char_u *string)
{
    char_u *p1;

    p1 = vim_strsave(string);
    vim_strup(p1);
    واپس p1;
}

/*
 * Like vim_strnsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strnsave_اوپر(char_u *string, size_t len)
{
    char_u *p1;

    p1 = vim_strnsave(string, len);
    vim_strup(p1);
    واپس p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    خالی
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    صحیح	    c;

    اگر (p == تہی)
	واپس;

    p2 = p;
    جب ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#اگر defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    ساکن char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    اگر (res != تہی)
	جب (*p != NUL)
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
		    اگر (s == تہی)
		    {
			vim_free(res);
			واپس تہی;
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
	    ورنہ اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ورنہ
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    واپس res;
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

    اگر (res != تہی)
	جب (*p != NUL)
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
		    اگر (s == تہی)
		    {
			vim_free(res);
			واپس تہی;
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
	    ورنہ اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ورنہ
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    واپس res;
}
#اگر_ختم

/*
 * delete spaces at the end of a string
 */
    خالی
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    جب (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    خالی
vim_strncpy(char_u *مقصد, char_u *from, size_t len)
{
    STRNCPY(مقصد, from, len);
    مقصد[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    خالی
vim_strcat(char_u *مقصد, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(مقصد);
    size_t fromlen = STRLEN(from);

    اگر (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(مقصد + tolen, from, tosize - tolen - 1);
	مقصد[tosize - 1] = NUL;
    }
    ورنہ
	mch_memmove(مقصد + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(حرف *s, size_t maxlen)
{
    size_t i;
    برائے (i = 0; i < maxlen; ++i)
	اگر (s[i] == NUL)
	    توڑو;
    واپس i;
}

#اگر !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    صحیح
vim_stricmp(حرف *s1, حرف *s2)
{
    صحیح		i;

    برائے (;;)
    {
	i = (صحیح)TOLOWER_LOC(*s1) - (صحیح)TOLOWER_LOC(*s2);
	اگر (i != 0)
	    واپس i;			    // this character different
	اگر (*s1 == NUL)
	    توڑو;			    // strings match until NUL
	++s1;
	++s2;
    }
    واپس 0;				    // strings match
}
#اگر_ختم

#اگر !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    صحیح
vim_strnicmp(حرف *s1, حرف *s2, size_t len)
{
    صحیح		i;

    جب (len > 0)
    {
	i = (صحیح)TOLOWER_LOC(*s1) - (صحیح)TOLOWER_LOC(*s2);
	اگر (i != 0)
	    واپس i;			    // this character different
	اگر (*s1 == NUL)
	    توڑو;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    واپس 0;				    // strings match
}
#اگر_ختم

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    صحیح
vim_strnicmp_asc(حرف *s1, حرف *s2, size_t len)
{
    صحیح                i = 0;

    جب (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	اگر (i != 0)
	    توڑو;			// this character is different
	اگر (*s1 == NUL)
	    توڑو;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    واپس i;
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
	واپس vim_strbyte(string, c);
    اگر (enc_utf8 && c >= 0x80)
    {
	جب (*p != NUL)
	{
	    صحیح l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    اگر (utf_ptr2char(p) == c && l > 1)
		واپس p;
	    p += l;
	}
	واپس تہی;
    }
    اگر (enc_dbcs != 0 && c > 255)
    {
	صحیح	n2 = c & 0xff;

	c = ((غیرعلامتی)c >> 8) & 0xff;
	جب ((b = *p) != NUL)
	{
	    اگر (b == c && p[1] == n2)
		واپس p;
	    p += (*mb_ptr2len)(p);
	}
	واپس تہی;
    }
    اگر (has_mbyte)
    {
	جب ((b = *p) != NUL)
	{
	    اگر (b == c)
		واپس p;
	    p += (*mb_ptr2len)(p);
	}
	واپس تہی;
    }
    جب ((b = *p) != NUL)
    {
	اگر (b == c)
	    واپس p;
	++p;
    }
    واپس تہی;
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
	واپس تہی;
    واپس (char_u *)strchr((حرف *)string, c);
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
    char_u	*retval = تہی;
    char_u	*p = string;

    جب (*p)
    {
	اگر (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    واپس retval;
}

/*
 * Vim's version of strpbrk(), in case it's missing.
 * Don't generate a prototype for this, causes problems when it's not used.
 */
#اگر_غیر_تعریف_شدہ PROTO
# ifndef HAVE_STRPBRK
#  ifdef vim_strpbrk
#   undef vim_strpbrk
#  endif
    char_u *
vim_strpbrk(char_u *s, char_u *charset)
{
    جب (*s)
    {
	اگر (vim_strchr(charset, *s) != تہی)
	    واپس s;
	MB_PTR_ADV(s);
    }
    واپس تہی;
}
# endif
#اگر_ختم

/*
 * Sort an array of strings.
 */
ساکن صحیح ترتیب_دو_compare(ثابت خالی *s1, ثابت خالی *s2);

    ساکن صحیح
ترتیب_دو_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    واپس STRCMP(*(حرف **)s1, *(حرف **)s2);
}

    خالی
ترتیب_دو_لڑیاں(
    char_u	**files,
    صحیح		گنتی)
{
    qsort((خالی *)files, (size_t)گنتی, حجم(char_u *), ترتیب_دو_compare);
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

    اگر (s != تہی)
	برائے (p = s; *p != NUL; ++p)
	    اگر (*p >= 128)
		واپس TRUE;
    واپس FALSE;
}
#اگر_ختم

/*
 * Concatenate two strings and return the result in allocated memory.
 * Returns NULL when out of memory.
 */
    char_u  *
concat_str(char_u *str1, char_u *str2)
{
    char_u  *dest;
    size_t  l = str1 == تہی ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == تہی ? 0 : STRLEN(str2)) + 1L);
    اگر (dest == تہی)
	واپس تہی;
    اگر (str1 == تہی)
	*dest = NUL;
    ورنہ
	STRCPY(dest, str1);
    اگر (str2 != تہی)
	STRCPY(dest + l, str2);
    واپس dest;
}

#اگر defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
الٹ_متن(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    اگر (rev == تہی)
	واپس تہی;

    برائے (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	اگر (has_mbyte)
	{
	    صحیح mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	ورنہ
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    واپس rev;
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, صحیح function)
{
    غیرعلامتی	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    اگر (str != تہی)
    {
	len += (غیرعلامتی)STRLEN(str);
	برائے (p = str; *p != NUL; MB_PTR_ADV(p))
	    اگر (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    اگر (r == تہی)
	واپس تہی;

    اگر (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    ورنہ
	*r++ = '\'';
    اگر (str != تہی)
	برائے (p = str; *p != NUL; )
	{
	    اگر (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    اگر (function)
	*r++ = ')';
    *r++ = NUL;
    واپس s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    طویل
string_گنتی(char_u *haystack, char_u *needle, صحیح ic)
{
    طویل	n = 0;
    char_u	*p = haystack;
    char_u	*اگلا;

    اگر (p == تہی || needle == تہی || *needle == NUL)
	واپس 0;

    size_t  needlelen = STRLEN(needle);
    اگر (ic)
    {
	جب (*p != NUL)
	{
	    اگر (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    ورنہ
		MB_PTR_ADV(p);
	}
    }
    ورنہ
	جب ((اگلا = (char_u *)لڑی_تلاش((حرف *)p, (حرف *)needle)) != تہی)
	{
	    ++n;
	    p = اگلا + needlelen;
	}

    واپس n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    ساکن صحیح
copy_پہلا_char_مقصد_tv(char_u *داخلہ, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    صحیح		len;

    اگر (داخلہ == تہی || output == تہی)
	واپس -1;

    len = has_mbyte ? mb_ptr2len(داخلہ) : 1;
    STRNCPY(buf, داخلہ, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    واپس output->vval.v_string == تہی ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    خالی
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*اظہاریہ,
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
    rettv->vval.v_string = تہی;

    // set_vim_var_nr() doesn't set the type
    سیٹ_vim_var_type(VV_کلید, VAR_نمبر);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اظہاریہ_get_funccal(اظہاریہ, &newtv);

    ga_init2(&ga, حجم(حرف), 80);
    برائے (p = str; *p != NUL; p += len)
    {
	len = copy_پہلا_char_مقصد_tv(p, &tv);
	اگر (len < 0)
	    توڑو;

	سیٹ_vim_var_nr(VV_کلید, idx);
	اگر (filter_map_one(&tv, اظہاریہ, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    توڑو;
	}
	اگر (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    اگر (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		توڑو;
	    }
	    ورنہ
		ga_concat(&ga, newtv.vval.v_string);
	}
	ورنہ اگر (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_ڈیٹا;
    اگر (fc != تہی)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    خالی
string_reduce(
	typval_T	*argvars,
	typval_T	*اظہاریہ,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    صحیح		len;
    typval_T	argv[3];
    صحیح		r;
    صحیح		called_emsg_آغاز = called_emsg;
    funccall_T	*fc;

    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	اگر (*p == NUL)
	{
	    semsg(_(e_reduce_کا_an_خالی_خانے_str_with_no_initial_قدر), "String");
	    واپس;
	}
	len = copy_پہلا_char_مقصد_tv(p, rettv);
	اگر (len < 0)
	    واپس;
	p += len;
    }
    ورنہ اگر (check_for_string_arg(argvars, 2) == FAIL)
	واپس;
    ورنہ
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اظہاریہ_get_funccal(اظہاریہ, rettv);

    برائے ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_پہلا_char_مقصد_tv(p, &argv[1]);
	اگر (len < 0)
	{
	    *rettv = argv[0];
	    توڑو;
	}

	r = eval_اظہاریہ_typval(اظہاریہ, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	اگر (r == FAIL || called_emsg != called_emsg_آغاز)
	    توڑو;
    }

    اگر (fc != تہی)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    ساکن خالی
byteidx_common(typval_T *argvars, typval_T *rettv, صحیح comp)
{
    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	واپس;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_نمبر_chk(&argvars[1], تہی);
    اگر (str == تہی || idx < 0)
	واپس;

    varnumber_T	utf16idx = FALSE;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	صحیح error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	اگر (error)
	    واپس;
	اگر (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_نمبر_as_bool_nr), utf16idx);
	    واپس;
	}
    }

    صحیح (*ptr2len)(char_u *);
    اگر (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    ورنہ
	ptr2len = mb_ptr2len;

    char_u *t = str;
    برائے ( ; idx > 0; idx--)
    {
	اگر (*t == NUL)		// EOL reached
	    واپس;
	اگر (utf16idx)
	{
	    صحیح clen = ptr2len(t);
	    صحیح c = (clen > 1) ? utf_ptr2char(t) : *t;
	    اگر (c > 0xFFFF)
		idx--;
	    اگر (idx > 0)
		t += clen;
	}
	ورنہ اگر (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_نمبر = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    خالی
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    خالی
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    خالی
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = -1;

    اگر (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	واپس;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_نمبر_chk(&argvars[1], تہی);
    اگر (str == تہی || idx < 0)
	واپس;

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
    ورنہ
	ptr2len = mb_ptr2len;

    char_u	*p;
    صحیح		len;
    برائے (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	اگر (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    اگر (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_نمبر = len;
	    واپس;
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

    rettv->vval.v_نمبر = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    ساکن صحیح
convert_string(string_T *str, char_u *from, char_u *مقصد, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    اگر (convert_تیاری(&vimconv, from, مقصد) == FAIL)
	واپس FAIL;
    vimconv.vc_fail = TRUE;
    اگر (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->لمبائی);
	اگر (ret->string == تہی)
	    ret->لمبائی = 0;
	ورنہ
	    ret->لمبائی = str->لمبائی;
    }
    ورنہ
    {
	صحیح len = (صحیح)str->لمبائی;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->لمبائی = len;
    }
    convert_تیاری(&vimconv, تہی, تہی);

    واپس (ret->string == تہی) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    ساکن خالی
blob_from_string(char_u *str, blob_T *blob)
{
    صحیح	    len = (صحیح)STRLEN(str);
    char_u  *dest;

    اگر (len == 0)
	واپس;
    اگر (ga_grow(&blob->bv_ga, len) == FAIL)
	واپس;
    dest = (char_u *)blob->bv_ga.ga_ڈیٹا + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    برائے (صحیح i = 0; i < len; ++i)
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
    ساکن صحیح
string_from_blob(blob_T *blob, طویل *آغاز_idx, string_T *ret)
{
    طویل	blen = blob_len(blob);
    طویل	آغاز = *آغاز_idx;
    char_u	*src;
    char_u	*nl;
    طویل	سطر_len;

    اگر (آغاز >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->لمبائی = 0;
	*آغاز_idx = blen;
	واپس (ret->string == تہی) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_ڈیٹا + آغاز;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - آغاز));
    سطر_len = (nl == تہی) ? (blen - آغاز) : (طویل)(nl - src);

    ret->string = alloc(سطر_len + 1);
    اگر (ret->string == تہی)
    {
	ret->لمبائی = 0;
	واپس FAIL;
    }
    اگر (سطر_len > 0)
	mch_memmove(ret->string, src, (size_t)سطر_len);
    ret->string[سطر_len] = NUL;
    ret->لمبائی = (size_t)سطر_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    برائے (طویل i = 0; i < سطر_len; i++)
	اگر (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *آغاز_idx = آغاز + سطر_len + (nl != تہی ? 1 : 0);
    واپس OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    ساکن char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    اگر (from_encoding_raw == تہی)
	واپس تہی;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    جب (*s != NUL)
    {
	اگر (*s == '_')
	    *pe++ = '-';
	ورنہ
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

    واپس from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    ساکن خالی
append_converted_string_مقصد_list(
	string_T *converted,
	صحیح validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    اگر (converted->string != تہی)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *انتہا = converted->string + converted->لمبائی;
	جب (p < انتہا)
	{
	    string_T	سطر;
	    char_u	*سطر_آغاز = p;

	    جب (p < انتہا && *p != NL)
		p++;

	    // Add this line to the result list
	    سطر.لمبائی = (size_t)(p - سطر_آغاز);
	    سطر.string = vim_strnsave(سطر_آغاز, سطر.لمبائی);
	    اگر (سطر.string != تہی)
	    {
		اگر (validate_utf8 && !utf_valid_string(سطر.string, تہی))
		{
		    vim_free(سطر.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    واپس; // Stop processing
		}
		اگر (list_append_string(list, سطر.string, (صحیح)سطر.لمبائی) == FAIL)
		{
		    vim_free(سطر.string);
		    vim_free(converted->string);
		    واپس; // Stop processing on append failure
		}
		vim_free(سطر.string);
	    }
	    ورنہ
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_برآمد_کا_حافظہ));
		vim_free(converted->string);
		واپس;
	    }

	    اگر (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    ورنہ
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    ساکن صحیح
append_validated_سطر_مقصد_list(string_T *سطر, صحیح validate_utf8, list_T *list)
{
    اگر (validate_utf8 && !utf_valid_string(سطر->string, تہی))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(سطر->string);
	واپس FAIL;
    }

    صحیح ret = list_append_string(list, سطر->string, (صحیح)سطر->لمبائی);
    vim_free(سطر->string);
    واپس ret;
}

    خالی
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    صحیح		blen;
    طویل	idx;
    صحیح		validate_utf8 = FALSE;

    اگر (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	واپس;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    blob = argvars->vval.v_blob;
    اگر (blob == تہی)
	واپس;
    blen = blob_len(blob);

    char_u	*from_encoding = تہی;
    char_u	*from_encoding_raw = تہی;  // Encoding name with endianness preserved for iconv
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	اگر (d != تہی)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    اگر (enc != تہی)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		اگر (from_encoding_raw == تہی)
		{
		    emsg(_(e_برآمد_کا_حافظہ));
		    VIM_CLEAR(from_encoding);
		    واپس;
		}
	    }
	}
    }

    اگر (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    اگر (from_encoding != تہی && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    صحیح from_prop = 0;
    اگر (from_encoding != تہی)
	from_prop = enc_canon_props(from_encoding);
    اگر (from_encoding != تہی && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2_لفظ)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	صحیح nul_سائز = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_سائز);
	اگر (ga_grow(&blob_ga, blen + nul_سائز) == OK)
	{
	    اگر (blen > 0)
		mch_memmove(blob_ga.ga_ڈیٹا, blob->bv_ga.ga_ڈیٹا, (size_t)blen);
	    // NUL terminator (2 bytes for UTF-16/UCS-2, 4 bytes for UTF-32/UCS-4)
	    vim_memset((char_u *)blob_ga.ga_ڈیٹا + blen, 0, (size_t)nul_سائز);
	    blob_ga.ga_len = blen + nul_سائز;
	}

	// Convert the entire blob at once
	vimconv_T vimconv;
	vimconv.vc_type = CONV_NONE;
	// Use raw encoding name for iconv to preserve endianness (utf-16be vs utf-16)
	// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
	اگر (convert_تیاری_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    جاؤ مکمل;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	صحیح len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_ڈیٹا, &len, تہی);
	converted.لمبائی = len;
	convert_تیاری(&vimconv, تہی, تہی);
	ga_clear(&blob_ga);
	append_converted_string_مقصد_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    ورنہ
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	جب (idx < blen)
	{
	    string_T	str;

	    اگر (string_from_blob(blob, &idx, &str) != OK)
		توڑو;

	    اگر (from_encoding != تہی)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		صحیح	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		اگر (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    جاؤ مکمل;
		}
		str.string = converted.string;
		str.لمبائی = converted.لمبائی;
	    }

	    اگر (append_validated_سطر_مقصد_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		جاؤ مکمل;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    اگر (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

مکمل:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    خالی
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    اگر (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	واپس;

    اگر (rettv_blob_alloc(rettv) == FAIL)
	واپس;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    اگر (list == تہی)
	واپس;

    char_u	*مقصد_encoding = تہی;
    char_u	*مقصد_encoding_raw = تہی;  // Encoding name with endianness preserved for iconv
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	اگر (d != تہی)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    اگر (enc != تہی)
	    {
		char_u *enc_skipped = enc_skip(enc);
		مقصد_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		مقصد_encoding_raw = normalize_encoding_name(enc_skipped);
		اگر (مقصد_encoding_raw == تہی)
		{
		    emsg(_(e_برآمد_کا_حافظہ));
		    VIM_CLEAR(مقصد_encoding);
		    واپس;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    صحیح مقصد_prop = 0;
    اگر (مقصد_encoding != تہی)
	مقصد_prop = enc_canon_props(مقصد_encoding);
    اگر (مقصد_encoding != تہی && (مقصد_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2_لفظ)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_اشیاء(list, li)
	{
	    char_u *s;

	    اگر (li->li_tv.v_type != VAR_STRING)
		جاری;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    اگر (li != list->lv_پہلا)
		ga_append(&str_ga, NL);
	    اگر (s != تہی && *s != NUL)
	    {
		صحیح slen = (صحیح)STRLEN(s);

		اگر (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    جاؤ مکمل;
		}
		mch_memmove((char_u *)str_ga.ga_ڈیٹا + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	اگر (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    اگر (convert_تیاری_ext(&vimconv, p_enc, FALSE, مقصد_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_مقصد_failed), مقصد_encoding);
		جاؤ مکمل;
	    }
	    vimconv.vc_fail = TRUE;

	    صحیح		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_ڈیٹا, &len, تہی);
	    convert_تیاری(&vimconv, تہی, تہی);
	    ga_clear(&str_ga);

	    اگر (converted == تہی)
	    {
		semsg(_(e_str_encoding_مقصد_failed), مقصد_encoding);
		جاؤ مکمل;
	    }
	    اگر (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_ڈیٹا + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	ورنہ
	    ga_clear(&str_ga);
    }
    ورنہ
    {
	FOR_ALL_LIST_اشیاء(list, li)
	{
	    اگر (li->li_tv.v_type != VAR_STRING)
		جاری;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    اگر (str.string == تہی)
		STR_LITERAL_سیٹ(str, "");
	    ورنہ
		str.لمبائی = STRLEN(str.string);

	    اگر (مقصد_encoding != تہی)
	    {
		صحیح	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, مقصد_encoding, &converted);
		اگر (res != OK)
		{
		    semsg(_(e_str_encoding_مقصد_failed), مقصد_encoding);
		    جاؤ مکمل;
		}
		str.string = converted.string;
		str.لمبائی = converted.لمبائی;
	    }

	    اگر (li != list->lv_پہلا)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    اگر (مقصد_encoding != تہی)
		vim_free(str.string);
	}
    }

مکمل:
    اگر (مقصد_encoding != تہی)
	vim_free(مقصد_encoding);
    اگر (مقصد_encoding_raw != تہی)
	vim_free(مقصد_encoding_raw);
}

/*
 * "str2list()" function
 */
    خالی
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		utf8 = FALSE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (صحیح)tv_get_bool_chk(&argvars[1], تہی);

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
	ورنہ
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	برائے ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_نمبر(rettv->vval.v_list, (*ptr2char)(p));
    }
    ورنہ
	برائے ( ; *p != NUL; ++p)
	    list_append_نمبر(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    خالی
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    صحیح		بنیاد = 10;
    char_u	*p;
    varnumber_T	n;
    صحیح		what = 0;
    صحیح		isneg;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	بنیاد = (صحیح)tv_get_نمبر(&argvars[1]);
	اگر (بنیاد != 2 && بنیاد != 8 && بنیاد != 10 && بنیاد != 16)
	{
	    emsg(_(e_invalid_دلیل));
	    واپس;
	}
	اگر (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    اگر (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    انتخاب (بنیاد)
    {
	حالت 2: what |= STR2NR_BIN + STR2NR_FORCE; توڑو;
	حالت 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; توڑو;
	حالت 16: what |= STR2NR_HEX + STR2NR_FORCE; توڑو;
    }
    vim_str2nr(p, تہی, تہی, what, &n, تہی, 0, FALSE, تہی);
    // Text after the number is silently ignored.
    اگر (isneg)
	rettv->vval.v_نمبر = -n;
    ورنہ
	rettv->vval.v_نمبر = n;

}

/*
 * "strgetchar()" function
 */
    خالی
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    صحیح		len;
    صحیح		error = FALSE;
    صحیح		charidx;
    صحیح		byteidx = 0;

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    str = tv_get_string_chk(&argvars[0]);
    اگر (str == تہی)
	واپس;
    len = (صحیح)STRLEN(str);
    charidx = (صحیح)tv_get_نمبر_chk(&argvars[1], &error);
    اگر (error)
	واپس;

    جب (charidx >= 0 && byteidx < len)
    {
	اگر (charidx == 0)
	{
	    rettv->vval.v_نمبر = mb_ptr2char(str + byteidx);
	    توڑو;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    خالی
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    صحیح		آغاز_idx;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 2) == FAIL))
	واپس;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_نمبر = -1;
    اگر (needle == تہی || haystack == تہی)
	واپس;		// type error; errmsg already given

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	صحیح	    error = FALSE;

	آغاز_idx = (صحیح)tv_get_نمبر_chk(&argvars[2], &error);
	اگر (error || آغاز_idx >= (صحیح)STRLEN(haystack))
	    واپس;
	اگر (آغاز_idx >= 0)
	    haystack += آغاز_idx;
    }

    pos	= (char_u *)لڑی_تلاش((حرف *)haystack, (حرف *)needle);
    اگر (pos != تہی)
	rettv->vval.v_نمبر = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    خالی
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    اگر (rettv->vval.v_string != تہی && tofree == تہی)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    خالی
f_strlen(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && check_for_string_or_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    ساکن خالی
strchar_common(typval_T *argvars, typval_T *rettv, صحیح skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    صحیح			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    جب (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_نمبر = len;
}

/*
 * "strcharlen()" function
 */
    خالی
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && check_for_string_or_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    خالی
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	صحیح error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	اگر (error)
	    واپس;
	اگر (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_نمبر_as_bool_nr), skipcc);
	    واپس;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    خالی
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = -1;

    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	واپس;

    varnumber_T countcc = FALSE;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    صحیح			(*func_mb_ptr2char_adv)(char_u **pp);
    صحیح			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    جب (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	اگر (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_نمبر = len;
}

/*
 * "strdisplaywidth()" function
 */
    خالی
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    صحیح		ستون = 0;

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    s = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	ستون = (صحیح)tv_get_نمبر(&argvars[1]);

    rettv->vval.v_نمبر = (varnumber_T)(linetabsize_ستون(ستون, s) - ستون);
}

/*
 * "strwidth()" function
 */
    خالی
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_نمبر = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    خالی
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

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	واپس;

    p = tv_get_string(&argvars[0]);
    slen = (صحیح)STRLEN(p);

    nchar = (صحیح)tv_get_نمبر_chk(&argvars[1], &error);
    اگر (!error)
    {
	اگر (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    اگر (error)
		واپس;
	    اگر (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_نمبر_as_bool_nr), skipcc);
		واپس;
	    }
	}

	اگر (nchar > 0)
	    جب (nchar > 0 && nbyte < slen)
	    {
		اگر (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		ورنہ
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	ورنہ
	    nbyte = nchar;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (صحیح)tv_get_نمبر(&argvars[2]);
	    جب (charlen > 0 && nbyte + len < slen)
	    {
		صحیح off = nbyte + len;

		اگر (off < 0)
		    len += 1;
		ورنہ
		{
		    اگر (skipcc)
			len += mb_ptr2len(p + off);
		    ورنہ
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	ورنہ
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    اگر (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    ورنہ اگر (nbyte > slen)
	nbyte = slen;
    اگر (len < 0)
	len = 0;
    ورنہ اگر (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    خالی
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		n;
    صحیح		len;
    صحیح		slen;
    صحیح		error = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	واپس;

    p = tv_get_string(&argvars[0]);
    slen = (صحیح)STRLEN(p);

    n = (صحیح)tv_get_نمبر_chk(&argvars[1], &error);
    اگر (error)
	len = 0;
    ورنہ اگر (argvars[2].v_type != VAR_UNKNOWN)
	len = (صحیح)tv_get_نمبر(&argvars[2]);
    ورنہ
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    اگر (n < 0)
    {
	len += n;
	n = 0;
    }
    ورنہ اگر (n > slen)
	n = slen;
    اگر (len < 0)
	len = 0;
    ورنہ اگر (n + len > slen)
	len = slen - n;

    اگر (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	صحیح off;

	// length in characters
	برائے (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    خالی
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = تہی;
    صحیح		haystack_len, انتہا_idx;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 2) == FAIL))
	واپس;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_نمبر = -1;
    اگر (needle == تہی || haystack == تہی)
	واپس;		// type error; errmsg already given

    haystack_len = (صحیح)STRLEN(haystack);
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	انتہا_idx = (صحیح)tv_get_نمبر_chk(&argvars[2], تہی);
	اگر (انتہا_idx < 0)
	    واپس;	// can never find a match
    }
    ورنہ
	انتہا_idx = haystack_len;

    اگر (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + انتہا_idx;
    }
    ورنہ
    {
	برائے (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)لڑی_تلاش((حرف *)rest, (حرف *)needle);
	    اگر (rest == تہی || rest > haystack + انتہا_idx)
		توڑو;
	    lastmatch = rest;
	}
    }

    اگر (lastmatch == تہی)
	rettv->vval.v_نمبر = -1;
    ورنہ
	rettv->vval.v_نمبر = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    خالی
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    خالی
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = -1;

    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_نمبر_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    واپس;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_نمبر_chk(&argvars[1], تہی);
    اگر (str == تہی || idx < 0)
	واپس;

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
    ورنہ
	ptr2len = mb_ptr2len;

    char_u	*p;
    صحیح		len;
    صحیح		utf16idx = 0;
    برائے (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	اگر (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    اگر (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_نمبر = len;
	    واپس;
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

    rettv->vval.v_نمبر = utf16idx;
}

/*
 * "tolower(string)" function
 */
    خالی
f_tolower(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    خالی
f_toupper(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    خالی
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*اندر_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    صحیح		inlen;
    صحیح		fromlen;
    صحیح		tolen;
    صحیح		idx;
    char_u	*cpstr;
    صحیح		cplen;
    صحیح		پہلا = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	واپس;

    اندر_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;
    اگر (fromstr == تہی || tostr == تہی)
	    واپس;		// type error; errmsg already given
    ga_init2(&ga, حجم(حرف), 80);

    اگر (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	اگر (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_دلیل_str), fromstr);
	    ga_clear(&ga);
	    واپس;
	}

    // fromstr and tostr have to contain the same number of chars
    جب (*اندر_str != NUL)
    {
	اگر (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(اندر_str);
	    cpstr = اندر_str;
	    cplen = inlen;
	    idx = 0;
	    برائے (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		اگر (fromlen == inlen && STRNCMP(اندر_str, p, inlen) == 0)
		{
		    برائے (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			اگر (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    توڑو;
			}
		    }
		    اگر (*p == NUL)	// tostr is shorter than fromstr
			جاؤ error;
		    توڑو;
		}
		++idx;
	    }

	    اگر (پہلا && cpstr == اندر_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		پہلا = FALSE;
		برائے (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		اگر (idx != 0)
		    جاؤ error;
	    }

	    (خالی)ga_grow(&ga, cplen);
	    mch_memmove((حرف *)ga.ga_ڈیٹا + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    اندر_str += inlen;
	}
	ورنہ
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *اندر_str);
	    اگر (p != تہی)
		ga_append(&ga, tostr[p - fromstr]);
	    ورنہ
		ga_append(&ga, *اندر_str);
	    ++اندر_str;
	}
    }

    // add a terminating NUL
    (خالی)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_ڈیٹا;
}

/*
 * "trim({expr})" function
 */
    خالی
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*سر;
    char_u	*نقاب = تہی;
    char_u	*دم;
    char_u	*پچھلا;
    char_u	*p;
    صحیح		c1;
    صحیح		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 2) == FAIL)))
	واپس;

    سر = tv_get_string_buf_chk(&argvars[0], buf1);
    اگر (سر == تہی)
	واپس;

    اگر (check_for_opt_string_arg(argvars, 1) == FAIL)
	واپس;

    اگر (argvars[1].v_type == VAR_STRING)
    {
	نقاب = tv_get_string_buf_chk(&argvars[1], buf2);
	اگر (*نقاب == NUL)
	    نقاب = تہی;

	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    صحیح	error = 0;

	    // leading or trailing characters to trim
	    dir = (صحیح)tv_get_نمبر_chk(&argvars[2], &error);
	    اگر (error)
		واپس;
	    اگر (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[2]));
		واپس;
	    }
	}
    }

    اگر (dir == 0 || dir == 1)
    {
	// Trim leading characters
	جب (*سر != NUL)
	{
	    c1 = PTR2CHAR(سر);
	    اگر (نقاب == تہی)
	    {
		اگر (c1 > ' ' && c1 != 0xa0)
		    توڑو;
	    }
	    ورنہ
	    {
		برائے (p = نقاب; *p != NUL; MB_PTR_ADV(p))
		    اگر (c1 == PTR2CHAR(p))
			توڑو;
		اگر (*p == NUL)
		    توڑو;
	    }
	    MB_PTR_ADV(سر);
	}
    }

    دم = سر + STRLEN(سر);
    اگر (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	برائے (; دم > سر; دم = پچھلا)
	{
	    پچھلا = دم;
	    MB_PTR_عقب(سر, پچھلا);
	    c1 = PTR2CHAR(پچھلا);
	    اگر (نقاب == تہی)
	    {
		اگر (c1 > ' ' && c1 != 0xa0)
		    توڑو;
	    }
	    ورنہ
	    {
		برائے (p = نقاب; *p != NUL; MB_PTR_ADV(p))
		    اگر (c1 == PTR2CHAR(p))
			توڑو;
		اگر (*p == NUL)
		    توڑو;
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
    ساکن char_u *
uri_decode(char_u *str)
{
    اگر (str == تہی)
	واپس تہی;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    اگر (!decoded)
	واپس تہی;

    char_u	*p = decoded;
    size_t	i = 0;

    جب (i < len)
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
	    ورنہ
	    {
		صحیح val = hexhex2nr(&str[i + 1]);
		اگر (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		ورنہ
		{
		    // invalid hex digits following "%"
		    برائے (صحیح j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	ورنہ
	    *p++ = str[i++];
    }

    *p = NUL;

    واپس decoded;
}

/*
 * "uri_decode({str})" function
 */
    خالی
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (check_for_string_arg(argvars, 0) == FAIL)
	واپس;

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
    ساکن char_u *
uri_encode(char_u *str)
{
    اگر (str == تہی)
	واپس تہی;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    اگر (encoded == تہی)
	واپس تہی;

    char_u *p = encoded;

    برائے (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	اگر (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	ورنہ
	{
	    sprintf((حرف *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    واپس encoded;
}

/*
 * "uri_encode({str})" function
 */
    خالی
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

ساکن حرف *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ساکن varnumber_T
tv_nr(typval_T *tvs, صحیح *idxp)
{
    صحیح		idx = *idxp - 1;
    varnumber_T	n = 0;
    صحیح		err = FALSE;

    اگر (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ورنہ
    {
	++*idxp;
	n = tv_get_نمبر_chk(&tvs[idx], &err);
	اگر (err)
	    n = 0;
    }
    واپس n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    ساکن حرف *
tv_str(typval_T *tvs, صحیح *idxp, char_u **tofree)
{
    صحیح		    idx = *idxp - 1;
    حرف	    *s = تہی;
    ساکن char_u   numbuf[NUMBUFLEN];

    اگر (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ورنہ
    {
	++*idxp;
	اگر (tofree != تہی)
	    s = (حرف *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	ورنہ
	    s = (حرف *)tv_get_string_chk(&tvs[idx]);
    }
    واپس s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ساکن دوہرا
tv_float(typval_T *tvs, صحیح *idxp)
{
    صحیح		idx = *idxp - 1;
    دوہرا	f = 0;

    اگر (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ورنہ
    {
	++*idxp;
	اگر (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	ورنہ اگر (tvs[idx].v_type == VAR_نمبر)
	    f = (دوہرا)tvs[idx].vval.v_نمبر;
	ورنہ
	    emsg(_(e_expected_float_دلیل_for_printf));
    }
    واپس f;
}

#اگر_ختم

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    ساکن ثابت حرف *
infinity_str(صحیح مثبت,
	     حرف fmt_spec,
	     صحیح force_sign,
	     صحیح space_for_مثبت)
{
    ساکن ثابت حرف *جدول[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    صحیح idx = مثبت * (1 + force_sign + force_sign * space_for_مثبت);

    اگر (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    واپس جدول[idx];
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
#اگر_غیر_تعریف_شدہ PROTO

// Like vim_vsnprintf() but append to the string.
    صحیح
vim_snprintf_جوڑو(حرف *str, size_t str_m, ثابت حرف *fmt, ...)
{
    va_list	ap;
    صحیح		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    اگر (str_m <= len)
	space = 0;
    ورنہ
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    واپس str_l;
}

    صحیح
vim_snprintf(حرف *str, size_t str_m, ثابت حرف *fmt, ...)
{
    va_list	ap;
    صحیح		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    واپس str_l;
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
    صحیح	    str_l;

    اگر (str_m == 0)
	واپس 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, تہی);
    va_end(ap);

    اگر (str_l < 0)
    {
	*str = NUL;
	واپس 0;
    }
    واپس ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    صحیح
vim_vsnprintf(
    حرف	*str,
    size_t	str_m,
    ثابت حرف	*fmt,
    va_list	ap)
{
    واپس vim_vsnprintf_typval(str, str_m, fmt, ap, تہی);
}

شمار
{
    TYPE_UNKNOWN = -1,
    TYPE_INT,
    TYPE_LONGINT,
    TYPE_LONGLONGINT,
    TYPE_UNSIGNEDINT,
    TYPE_UNSIGNEDLONGINT,
    TYPE_UNSIGNEDLONGLONGINT,
    TYPE_اشارہ_گر,
    TYPE_PERCENT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_FLOAT
};

/* Types that can be used in a format string
 */
    ساکن صحیح
format_typeof(
    ثابت حرف	*type)
{
    // allowed values: \0, h, l, L
    حرف    لمبائی_modifier = '\0';

    // current conversion specifier character
    حرف    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    اگر (*type == 'h' || *type == 'l')
    {
	لمبائی_modifier = *type;
	type++;
	اگر (لمبائی_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    لمبائی_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    انتخاب (fmt_spec)
    {
	حالت 'i': fmt_spec = 'd'; توڑو;
	حالت '*': fmt_spec = 'd'; لمبائی_modifier = 'h'; توڑو;
	حالت 'D': fmt_spec = 'd'; لمبائی_modifier = 'l'; توڑو;
	حالت 'U': fmt_spec = 'u'; لمبائی_modifier = 'l'; توڑو;
	حالت 'O': fmt_spec = 'o'; لمبائی_modifier = 'l'; توڑو;
	طےشدہ: توڑو;
    }

    // get parameter value, do initial processing
    انتخاب (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    حالت '%':
	واپس TYPE_PERCENT;

    حالت 'c':
	واپس TYPE_CHAR;

    حالت 's':
    حالت 'S':
	واپس TYPE_STRING;

    حالت 'd': حالت 'u':
    حالت 'b': حالت 'B':
    حالت 'o':
    حالت 'x': حالت 'X':
    حالت 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //قدر

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    اگر (fmt_spec == 'p')
		واپس TYPE_اشارہ_گر;
	    ورنہ اگر (fmt_spec == 'b' || fmt_spec == 'B')
		واپس TYPE_UNSIGNEDLONGLONGINT;
	    ورنہ اگر (fmt_spec == 'd')
	    {
		// signed
		انتخاب (لمبائی_modifier)
		{
		حالت '\0':
		حالت 'h':
		    // char and short arguments are passed as int.
		    واپس TYPE_INT;
		حالت 'l':
		    واپس TYPE_LONGINT;
		حالت 'L':
		    واپس TYPE_LONGLONGINT;
		}
	    }
	    ورنہ
	    {
		// unsigned
		انتخاب (لمبائی_modifier)
		{
		    حالت '\0':
		    حالت 'h':
			واپس TYPE_UNSIGNEDINT;
		    حالت 'l':
			واپس TYPE_UNSIGNEDLONGINT;
		    حالت 'L':
			واپس TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	توڑو;

    حالت 'f':
    حالت 'F':
    حالت 'e':
    حالت 'E':
    حالت 'g':
    حالت 'G':
	واپس TYPE_FLOAT;
    }

    واپس TYPE_UNKNOWN;
}

    ساکن حرف *
format_typename(
    ثابت حرف  *type)
{
    انتخاب (format_typeof(type))
    {
	حالت TYPE_INT:
	    واپس typename_int;

	حالت TYPE_LONGINT:
	    واپس typename_longint;

	حالت TYPE_LONGLONGINT:
	    واپس typename_longlongint;

	حالت TYPE_UNSIGNEDINT:
	    واپس typename_unsignedint;

	حالت TYPE_UNSIGNEDLONGINT:
	    واپس typename_unsignedlongint;

	حالت TYPE_UNSIGNEDLONGLONGINT:
	    واپس typename_unsignedlonglongint;

	حالت TYPE_اشارہ_گر:
	    واپس _(typename_اشارہ_گر);

	حالت TYPE_PERCENT:
	    واپس _(typename_percent);

	حالت TYPE_CHAR:
	    واپس typename_char;

	حالت TYPE_STRING:
	    واپس _(typename_string);

	حالت TYPE_FLOAT:
	    واپس typename_float;
    }

    واپس _(typename_unknown);
}

    ساکن صحیح
adjust_types(
    ثابت حرف ***ap_types,
    صحیح arg,
    صحیح *عدد_posarg,
    ثابت حرف *type)
{
    اگر (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	واپس FAIL;
    }

    اگر (*ap_types == تہی || *عدد_posarg < arg)
    {
	صحیح	    idx;
	ثابت حرف  **new_types;

	اگر (*ap_types == تہی)
	    new_types = ALLOC_CLEAR_MULT(ثابت حرف *, arg);
	ورنہ
	    new_types = vim_realloc((حرف **)*ap_types,
						arg * حجم(ثابت حرف *));

	اگر (new_types == تہی)
	    واپس FAIL;

	برائے (idx = *عدد_posarg; idx < arg; ++idx)
	    new_types[idx] = تہی;

	*ap_types = new_types;
	*عدد_posarg = arg;
    }

    اگر ((*ap_types)[arg - 1] != تہی)
    {
	اگر ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    ثابت حرف *pt = type;
	    اگر (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    اگر (pt[0] != '*')
	    {
		انتخاب (pt[0])
		{
		    حالت 'd':
		    حالت 'i': توڑو;
		    طےشدہ:
			semsg(_(e_positional_عدد_میدان_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			واپس FAIL;
		}
	    }
	}
	ورنہ
	{
	    اگر (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_عدد_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		واپس FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    واپس OK;
}

    ساکن خالی
format_حد_سے_تجاوز_error(ثابت حرف *pstart)
{
    size_t	arglen = 0;
    حرف	*argcopy = تہی;
    ثابت حرف	*p = pstart;

    جب (VIM_ISDIGIT((صحیح)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(حرف, arglen + 1);
    اگر (argcopy != تہی)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	آزاد(argcopy);
    }
    ورنہ
	semsg(_(e_برآمد_کا_حافظہ_allocating_nr_بائٹیں), arglen);
}

# define MAX_ALLOWED_STRING_چوڑائی 1048576    // 1 MiB

    ساکن صحیح
get_unsigned_int(
    ثابت حرف *pstart,
    ثابت حرف **p,
    غیرعلامتی صحیح *uj,
    صحیح حد_سے_تجاوز_err)
{
    *uj = **p - '0';
    ++*p;

    جب (VIM_ISDIGIT((صحیح)(**p)) && *uj < MAX_ALLOWED_STRING_چوڑائی)
    {
	*uj = 10 * *uj + (غیرعلامتی صحیح)(**p - '0');
	++*p;
    }

    اگر (*uj > MAX_ALLOWED_STRING_چوڑائی)
    {
	اگر (حد_سے_تجاوز_err)
	{
	    format_حد_سے_تجاوز_error(pstart);
	    واپس FAIL;
	}
	ورنہ
	    *uj = MAX_ALLOWED_STRING_چوڑائی;
    }

    واپس OK;
}


    ساکن صحیح
parse_fmt_types(
    ثابت حرف  ***ap_types,
    صحیح		*عدد_posarg,
    ثابت حرف  *fmt,
    typval_T	*tvs غیر_مستعمل
    )
{
    ثابت حرف	*p = fmt;
    ثابت حرف	*arg = تہی;

    صحیح		any_pos = 0;
    صحیح		any_arg = 0;
    صحیح		arg_idx;

# define CHECK_POS_ARG کرو { \
    اگر (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	جاؤ error; \
    } \
} جب (0);

    اگر (p == تہی)
	واپس OK;

    جب (*p != NUL)
    {
	اگر (*p != '%')
	{
	    ثابت حرف    *q = strchr(p + 1, '%');
	    size_t  n = (q == تہی) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	ورنہ
	{
	    // allowed values: \0, h, l, L
	    حرف	لمبائی_modifier = '\0';

	    // variable for positional arg
	    صحیح		pos_arg = -1;
	    ثابت حرف	*ptype = تہی;
	    ثابت حرف	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    جب (VIM_ISDIGIT(*ptype))
		++ptype;

	    اگر (*ptype == '$')
	    {
		اگر (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    جاؤ error;
		}

		// Positional argument
		غیرعلامتی صحیح uj;

		اگر (get_unsigned_int(pstart, &p, &uj, tvs != تہی) == FAIL)
		    جاؤ error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    جب (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		انتخاب (*p)
		{
		    حالت '0': توڑو;
		    حالت '-': توڑو;
		    حالت '+': توڑو;
		    حالت ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      توڑو;
		    حالت '#': توڑو;
		    حالت '\'': توڑو;
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
		    غیرعلامتی صحیح uj;

		    اگر (get_unsigned_int(arg + 1, &p, &uj, tvs != تہی) == FAIL)
			جاؤ error;

		    اگر (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			جاؤ error;
		    }
		    ورنہ
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			اگر (adjust_types(ap_types, uj, عدد_posarg, arg) == FAIL)
			    جاؤ error;
		    }
		}
		ورنہ
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    ورنہ اگر (VIM_ISDIGIT((صحیح)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ثابت حرف *digstart = p;
		غیرعلامتی صحیح uj;

		اگر (get_unsigned_int(digstart, &p, &uj, tvs != تہی) == FAIL)
		    جاؤ error;

		اگر (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    جاؤ error;
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
			غیرعلامتی صحیح uj;

			اگر (get_unsigned_int(arg + 1, &p, &uj, tvs != تہی) == FAIL)
			    جاؤ error;

			اگر (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    اگر (adjust_types(ap_types, uj, عدد_posarg, arg) == FAIL)
				جاؤ error;
			}
			ورنہ
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    جاؤ error;
			}
		    }
		    ورنہ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		ورنہ اگر (VIM_ISDIGIT((صحیح)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ثابت حرف *digstart = p;
		    غیرعلامتی صحیح uj;

		    اگر (get_unsigned_int(digstart, &p, &uj, tvs != تہی) == FAIL)
			جاؤ error;

		    اگر (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			جاؤ error;
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
		لمبائی_modifier = *p;
		p++;
		اگر (لمبائی_modifier == 'l' && *p == 'l')
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
			اگر (adjust_types(ap_types, pos_arg, عدد_posarg, ptype) == FAIL)
			    جاؤ error;
		    }
		    ورنہ
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    توڑو;

		طےشدہ:
		    اگر (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			جاؤ error;
		    }
	    }

	    اگر (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    برائے (arg_idx = 0; arg_idx < *عدد_posarg; ++arg_idx)
    {
	اگر ((*ap_types)[arg_idx] == تہی)
	{
	    semsg(_(e_fmt_arg_nr_غیر_مستعمل_str), arg_idx + 1, fmt);
	    جاؤ error;
	}

# اگر defined(FEAT_EVAL)
	اگر (tvs != تہی && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_برآمد_کا_bounds_str), arg_idx + 1, fmt);
	    جاؤ error;
	}
# endif
    }

    واپس OK;

error:
    vim_free((حرف**)*ap_types);
    *ap_types = تہی;
    *عدد_posarg = 0;
    واپس FAIL;
}

    ساکن خالی
skip_مقصد_arg(
    ثابت حرف	**ap_types,
    va_list	ap_آغاز,
    va_list	*ap,
    صحیح		*arg_idx,
    صحیح		*arg_cur,
    ثابت حرف	*fmt)
{
    صحیح		arg_min = 0;

    اگر (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	واپس;
    }

    اگر (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_آغاز);
    }
    ورنہ
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    برائے (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	ثابت حرف *p;

	اگر (ap_types == تہی || ap_types[*arg_cur] == تہی)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    واپس;
	}

	p = ap_types[*arg_cur];

	صحیح fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	انتخاب (fmt_type)
	{
	حالت TYPE_PERCENT:
	حالت TYPE_UNKNOWN:
	    توڑو;

	حالت TYPE_CHAR:
	    va_arg(*ap, صحیح);
	    توڑو;

	حالت TYPE_STRING:
	    va_arg(*ap, حرف *);
	    توڑو;

	حالت TYPE_اشارہ_گر:
	    va_arg(*ap, خالی *);
	    توڑو;

	حالت TYPE_INT:
	    va_arg(*ap, صحیح);
	    توڑو;

	حالت TYPE_LONGINT:
	    va_arg(*ap, طویل صحیح);
	    توڑو;

	حالت TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    توڑو;

	حالت TYPE_UNSIGNEDINT:
	    va_arg(*ap, غیرعلامتی صحیح);
	    توڑو;

	حالت TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, غیرعلامتی طویل صحیح);
	    توڑو;

	حالت TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    توڑو;

	حالت TYPE_FLOAT:
	    va_arg(*ap, دوہرا);
	    توڑو;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    واپس;
}

    صحیح
vim_vsnprintf_typval(
    حرف	*str,
    size_t	str_m,
    ثابت حرف	*fmt,
    va_list	ap_آغاز,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    ثابت حرف	*p = fmt;
    صحیح		arg_cur = 0;
    صحیح		عدد_posarg = 0;
    صحیح		arg_idx = 1;
    va_list	ap;
    ثابت حرف	**ap_types = تہی;

    اگر (parse_fmt_types(&ap_types, &عدد_posarg, fmt, tvs) == FAIL)
	واپس 0;

    va_copy(ap, ap_آغاز);

    اگر (p == تہی)
	p = "";
    جب (*p != NUL)
    {
	اگر (*p != '%')
	{
	    ثابت حرف    *q = strchr(p + 1, '%');
	    size_t  n = (q == تہی) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    اگر (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	ورنہ
	{
	    size_t  min_میدان_چوڑائی = 0, precision = 0;
	    صحیح	    zero_padding = 0, precision_specified = 0, justify_بایاں = 0;
	    صحیح	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    صحیح	    space_for_مثبت = 1;

	    // allowed values: \0, h, l, L
	    حرف    لمبائی_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    حرف    tmp[TMP_LEN];

	    // string address in case of string argument
	    ثابت حرف  *str_arg = تہی;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    غیرعلامتی حرف uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  نمبر_کا_zeros_مقصد_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    حرف    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = تہی;

	    // variables for positional arg
	    صحیح	    pos_arg = -1;
	    ثابت حرف	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    جب (VIM_ISDIGIT(*ptype))
		++ptype;

	    اگر (*ptype == '$')
	    {
		// Positional argument
		ثابت حرف *digstart = p;
		غیرعلامتی صحیح uj;

		اگر (get_unsigned_int(digstart, &p, &uj, tvs != تہی) == FAIL)
		    جاؤ error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    جب (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		انتخاب (*p)
		{
		    حالت '0': zero_padding = 1; توڑو;
		    حالت '-': justify_بایاں = 1; توڑو;
		    حالت '+': force_sign = 1; space_for_مثبت = 0; توڑو;
		    حالت ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      توڑو;
		    حالت '#': alternate_form = 1; توڑو;
		    حالت '\'': توڑو;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    اگر (*p == '*')
	    {
		صحیح j;
		ثابت حرف *digstart = p + 1;

		p++;

		اگر (VIM_ISDIGIT((صحیح)(*p)))
		{
		    // Positional argument field width
		    غیرعلامتی صحیح uj;

		    اگر (get_unsigned_int(digstart, &p, &uj, tvs != تہی) == FAIL)
			جاؤ error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# اگر defined(FEAT_EVAL)
		    tvs != تہی ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, صحیح));

		اگر (j > MAX_ALLOWED_STRING_چوڑائی)
		{
		    اگر (tvs != تہی)
		    {
			format_حد_سے_تجاوز_error(digstart);
			جاؤ error;
		    }
		    ورنہ
			j = MAX_ALLOWED_STRING_چوڑائی;
		}

		اگر (j >= 0)
		    min_میدان_چوڑائی = j;
		ورنہ
		{
		    min_میدان_چوڑائی = -j;
		    justify_بایاں = 1;
		}
	    }
	    ورنہ اگر (VIM_ISDIGIT((صحیح)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ثابت حرف *digstart = p;
		غیرعلامتی صحیح uj;

		اگر (get_unsigned_int(digstart, &p, &uj, tvs != تہی) == FAIL)
		    جاؤ error;

		min_میدان_چوڑائی = uj;
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
		    ثابت حرف *digstart = p;
		    غیرعلامتی صحیح uj;

		    اگر (get_unsigned_int(digstart, &p, &uj, tvs != تہی) == FAIL)
			جاؤ error;

		    precision = uj;
		}
		ورنہ اگر (*p == '*')
		{
		    صحیح j;
		    ثابت حرف *digstart = p;

		    p++;

		    اگر (VIM_ISDIGIT((صحیح)(*p)))
		    {
			// positional argument
			غیرعلامتی صحیح uj;

			اگر (get_unsigned_int(digstart, &p, &uj, tvs != تہی) == FAIL)
			    جاؤ error;

			arg_idx = uj;

			++p;
		    }

		    j =
# اگر defined(FEAT_EVAL)
			tvs != تہی ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, صحیح));

		    اگر (j > MAX_ALLOWED_STRING_چوڑائی)
		    {
			اگر (tvs != تہی)
			{
			    format_حد_سے_تجاوز_error(digstart);
			    جاؤ error;
			}
			ورنہ
			    j = MAX_ALLOWED_STRING_چوڑائی;
		    }

		    اگر (j >= 0)
			precision = j;
		    ورنہ
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    اگر (*p == 'h' || *p == 'l')
	    {
		لمبائی_modifier = *p;
		p++;
		اگر (لمبائی_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    لمبائی_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    انتخاب (fmt_spec)
	    {
		حالت 'i': fmt_spec = 'd'; توڑو;
		حالت 'D': fmt_spec = 'd'; لمبائی_modifier = 'l'; توڑو;
		حالت 'U': fmt_spec = 'u'; لمبائی_modifier = 'l'; توڑو;
		حالت 'O': fmt_spec = 'o'; لمبائی_modifier = 'l'; توڑو;
		طےشدہ: توڑو;
	    }

# اگر defined(FEAT_EVAL)
	    انتخاب (fmt_spec)
	    {
		حالت 'd': حالت 'u': حالت 'o': حالت 'x': حالت 'X':
		    اگر (tvs != تہی && لمبائی_modifier == '\0')
			لمبائی_modifier = 'L';
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
		    توڑو;

		حالت 'c':
		    {
			صحیح j;

			j =
# اگر defined(FEAT_EVAL)
			    tvs != تہی ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, صحیح));

			// standard demands unsigned char
			uchar_arg = (غیرعلامتی حرف)j;
			str_arg = (حرف *)&uchar_arg;
			توڑو;
		    }

		حالت 's':
		حالت 'S':
		    str_arg =
# اگر defined(FEAT_EVAL)
				tvs != تہی ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, حرف *));

		    اگر (str_arg == تہی)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    ورنہ اگر (!precision_specified)
			str_arg_l = لڑی_لمبائی(str_arg);
		    // truncate string if necessary as requested by precision
		    ورنہ اگر (precision == 0)
			str_arg_l = 0;
		    ورنہ
		    {
			// memchr on HP does not like n > 2^31  !!!
			ثابت حرف *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == تہی) ? precision
						      : (size_t)(q - str_arg);
		    }
		    اگر (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			صحیح	cell;

			برائے (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    اگر (precision_specified && i + cell > precision)
				توڑو;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			اگر (min_میدان_چوڑائی != 0)
			    min_میدان_چوڑائی += str_arg_l - i;
		    }
		    توڑو;

		طےشدہ:
		    توڑو;
		}
		توڑو;

	    حالت 'd': حالت 'u':
	    حالت 'b': حالت 'B':
	    حالت 'o':
	    حالت 'x': حالت 'X':
	    حالت 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //قدر

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    صحیح arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    صحیح int_arg = 0;
		    غیرعلامتی صحیح uint_arg = 0;

		    // only set for length modifier l
		    طویل صحیح long_arg = 0;
		    غیرعلامتی طویل صحیح ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    خالی *ptr_arg = تہی;

		    اگر (fmt_spec == 'p')
		    {
			لمبائی_modifier = '\0';
			ptr_arg =
# اگر defined(FEAT_EVAL)
				 tvs != تہی ? (خالی *)tv_str(tvs, &arg_idx,
									تہی) :
# endif
					(skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, خالی *));

			اگر (ptr_arg != تہی)
			    arg_sign = 1;
		    }
		    ورنہ اگر (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# اگر defined(FEAT_EVAL)
				    tvs != تہی ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			اگر (bin_arg != 0)
			    arg_sign = 1;
		    }
		    ورنہ اگر (fmt_spec == 'd')
		    {
			// signed
			انتخاب (لمبائی_modifier)
			{
			حالت '\0':
			حالت 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# اگر defined(FEAT_EVAL)
					tvs != تہی ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, صحیح));

			    اگر (int_arg > 0)
				arg_sign =  1;
			    ورنہ اگر (int_arg < 0)
				arg_sign = -1;
			    توڑو;
			حالت 'l':
			    long_arg =
# اگر defined(FEAT_EVAL)
					tvs != تہی ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, طویل صحیح));

			    اگر (long_arg > 0)
				arg_sign =  1;
			    ورنہ اگر (long_arg < 0)
				arg_sign = -1;
			    توڑو;
			حالت 'L':
			    llong_arg =
# اگر defined(FEAT_EVAL)
					tvs != تہی ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    اگر (llong_arg > 0)
				arg_sign =  1;
			    ورنہ اگر (llong_arg < 0)
				arg_sign = -1;
			    توڑو;
			}
		    }
		    ورنہ
		    {
			// unsigned
			انتخاب (لمبائی_modifier)
			{
			    حالت '\0':
			    حالت 'h':
				uint_arg =
# اگر defined(FEAT_EVAL)
					    tvs != تہی ? (غیرعلامتی)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, غیرعلامتی صحیح));

				اگر (uint_arg != 0)
				    arg_sign = 1;
				توڑو;
			    حالت 'l':
				ulong_arg =
# اگر defined(FEAT_EVAL)
					    tvs != تہی ? (غیرعلامتی طویل)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, غیرعلامتی طویل صحیح));

				اگر (ulong_arg != 0)
				    arg_sign = 1;
				توڑو;
			    حالت 'L':
				ullong_arg =
# اگر defined(FEAT_EVAL)
					    tvs != تہی ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				اگر (ullong_arg != 0)
				    arg_sign = 1;
				توڑو;
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
			    tmp[str_arg_l++] = space_for_مثبت ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    ورنہ اگر (alternate_form)
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
		    ورنہ
		    {
			حرف	f[6];
			صحیح	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			اگر (!لمبائی_modifier)
			    ;
			ورنہ اگر (لمبائی_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# ورنہ
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			ورنہ
			    f[f_l++] = لمبائی_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			اگر (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			ورنہ اگر (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    حرف	    b[8 * حجم(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    کرو
			    {
				b[حجم(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    جب (bn != 0);

			    حافظہ_نقل(tmp + str_arg_l, b + حجم(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			ورنہ اگر (fmt_spec == 'd')
			{
			    // signed
			    انتخاب (لمبائی_modifier)
			    {
			    حالت '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       توڑو;
			    حالت 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (مختصر)int_arg);
				      توڑو;
			    حالت 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      توڑو;
			    حالت 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      توڑو;
			    }
			}
			ورنہ
			{
			    // unsigned
			    انتخاب (لمبائی_modifier)
			    {
			    حالت '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       توڑو;
			    حالت 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(غیرعلامتی مختصر)uint_arg);
				      توڑو;
			    حالت 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      توڑو;
			    حالت 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      توڑو;
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
			size_t عدد_کا_digits = str_arg_l
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
					     || precision < عدد_کا_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = عدد_کا_digits + 1;
			    }
			}
			// zero padding to specified precision?
			اگر (عدد_کا_digits < precision)
			    نمبر_کا_zeros_مقصد_pad = precision - عدد_کا_digits;
		    }
		    // zero padding to specified minimal field width?
		    اگر (!justify_بایاں && zero_padding)
		    {
			صحیح n = (صحیح)(min_میدان_چوڑائی - (str_arg_l
						    + نمبر_کا_zeros_مقصد_pad));
			اگر (n > 0)
			    نمبر_کا_zeros_مقصد_pad += n;
		    }
		    توڑو;
		}

	    حالت 'f':
	    حالت 'F':
	    حالت 'e':
	    حالت 'E':
	    حالت 'g':
	    حالت 'G':
		{
		    // Floating point.
		    دوہرا	f;
		    دوہرا	abs_f;
		    حرف	format[40];
		    صحیح		l;
		    صحیح		remove_trailing_zeroes = FALSE;

		    f =
# اگر defined(FEAT_EVAL)
			tvs != تہی ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_مقصد_arg(ap_types, ap_آغاز, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, دوہرا));

		    abs_f = f < 0 ? -f : f;

		    اگر (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			اگر ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			ورنہ
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    اگر ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# ورنہ
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
		    ورنہ
		    {
			اگر (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			ورنہ اگر (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_مثبت));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			ورنہ
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    اگر (force_sign)
				format[l++] = space_for_مثبت ? ' ' : '+';
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
			    حرف *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    اگر (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    ورنہ
			    {
				tp = (حرف *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				اگر (tp != تہی)
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
				    جب (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    اگر (tp != تہی && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				جب (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			ورنہ
			{
			    حرف *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (حرف *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    اگر (tp != تہی && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    اگر (zero_padding && min_میدان_چوڑائی > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			نمبر_کا_zeros_مقصد_pad = min_میدان_چوڑائی - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    توڑو;
		}

	    طےشدہ:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_بایاں = 1;
		min_میدان_چوڑائی = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		اگر (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		توڑو;
	    }

	    اگر (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    اگر (!justify_بایاں)
	    {
		// left padding with blank or zero
		صحیح pn = (صحیح)(min_میدان_چوڑائی - (str_arg_l + نمبر_کا_zeros_مقصد_pad));

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
	    اگر (نمبر_کا_zeros_مقصد_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    ورنہ
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
		zn = (صحیح)نمبر_کا_zeros_مقصد_pad;
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
	    اگر (justify_بایاں)
	    {
		// right blank padding to the field width
		صحیح pn = (صحیح)(min_میدان_چوڑائی
				      - (str_arg_l + نمبر_کا_zeros_مقصد_pad));

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

    اگر (tvs != تہی && tvs[عدد_posarg != 0 ? عدد_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_مقصد_printf));

error:
    vim_free((حرف*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    واپس (صحیح)str_l;
}

#اگر_ختم // PROTO
