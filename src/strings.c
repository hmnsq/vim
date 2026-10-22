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
#અન્તર્ભાવ "vim.h"

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
    જો (p != ખાલી)
	mch_memmove(p, string, len);
    પાછા p;
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
    જો (p == ખાલી)
	પાછા ખાલી;

    STRNCPY(p, string, len);
    p[len] = NUL;
    પાછા p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    પાછા vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    પૂર્ણાંક		cc,
    પૂર્ણાંક		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    પૂર્ણાંક		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    માટે (p = string; *p; p++)
    {
	જો (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    ચાલુ;
	}
	જો (vim_strchr(esc_chars, *p) != ખાલી || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    જો (escaped_string == ખાલી)
	પાછા ખાલી;
    p2 = escaped_string;
    માટે (p = string; *p; p++)
    {
	જો (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    ચાલુ;
	}
	જો (vim_strchr(esc_chars, *p) != ખાલી || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    પાછા escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    પૂર્ણાંક
csh_like_shell(શૂન્ય)
{
    પાછા (strstr((અક્ષર *)gettail(p_sh), "csh") != ખાલી);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    સ્થિર પૂર્ણાંક
fish_like_shell(શૂન્ય)
{
    પાછા (strstr((અક્ષર *)gettail(p_sh), "fish") != ખાલી);
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
vim_strsave_shellescape(char_u *string, પૂર્ણાંક do_special, પૂર્ણાંક do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    પૂર્ણાંક		csh_like;
    પૂર્ણાંક		fish_like;
    char_u	*shname;
    પૂર્ણાંક		powershell;
#ifdef MSWIN
    પૂર્ણાંક		double_quotes;
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
    powershell = strstr((અક્ષર *)shname, "pwsh") != ખાલી;
#ifdef MSWIN
    powershell = powershell || strstr((અક્ષર *)shname, "powershell") != ખાલી;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (unsigned)STRLEN(string) + 3;  // two quotes and a trailing NUL
    માટે (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	જો (double_quotes)
	{
	    જો (*p == '"')
		++length;		// " -> ""
	}
	નહીંતર
#endif
	જો (*p == '\'')
	{
	    જો (powershell)
		length += 2;		// ' => ''
	    નહીંતર
		length += 3;		// ' => '\''
	}
	જો ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    જો (csh_like && do_special)
		++length;		// insert backslash
	}
	જો (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	જો (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    જો (escaped_string != ખાલી)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	જો (double_quotes)
	    *d++ = '"';
	નહીંતર
#endif
	    *d++ = '\'';

	માટે (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    જો (double_quotes)
	    {
		જો (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    ચાલુ;
		}
	    }
	    નહીંતર
#endif
	    જો (*p == '\'')
	    {
		જો (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		નહીંતર
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		ચાલુ;
	    }
	    જો ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		જો (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		ચાલુ;
	    }
	    જો (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		ચાલુ;
	    }
	    જો (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		ચાલુ;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	જો (double_quotes)
	    *d++ = '"';
	નહીંતર
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    પાછા escaped_string;
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
    પાછા p1;
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
    પાછા p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    શૂન્ય
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    પૂર્ણાંક	    c;

    જો (p == ખાલી)
	પાછા;

    p2 = p;
    જ્યારે ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#જો defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    સ્થિર char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    જો (res != ખાલી)
	જ્યારે (*p != NUL)
	{
	    પૂર્ણાંક		l;

	    જો (enc_utf8)
	    {
		પૂર્ણાંક	c, uc;
		પૂર્ણાંક	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		જો (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		જો (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    જો (s == ખાલી)
		    {
			vim_free(res);
			પાછા ખાલી;
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
	    નહીંતર જો (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    નહીંતર
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    પાછા res;
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

    જો (res != ખાલી)
	જ્યારે (*p != NUL)
	{
	    પૂર્ણાંક		l;

	    જો (enc_utf8)
	    {
		પૂર્ણાંક	c, lc;
		પૂર્ણાંક	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		જો (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		જો (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    જો (s == ખાલી)
		    {
			vim_free(res);
			પાછા ખાલી;
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
	    નહીંતર જો (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    નહીંતર
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    પાછા res;
}
#endif

/*
 * delete spaces at the end of a string
 */
    શૂન્ય
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    જ્યારે (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    શૂન્ય
vim_strncpy(char_u *to, char_u *from, size_t len)
{
    STRNCPY(to, from, len);
    to[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    શૂન્ય
vim_strcat(char_u *to, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(to);
    size_t fromlen = STRLEN(from);

    જો (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    નહીંતર
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(અક્ષર *s, size_t maxlen)
{
    size_t i;
    માટે (i = 0; i < maxlen; ++i)
	જો (s[i] == NUL)
	    તોડો;
    પાછા i;
}

#જો !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    પૂર્ણાંક
vim_stricmp(અક્ષર *s1, અક્ષર *s2)
{
    પૂર્ણાંક		i;

    માટે (;;)
    {
	i = (પૂર્ણાંક)TOLOWER_LOC(*s1) - (પૂર્ણાંક)TOLOWER_LOC(*s2);
	જો (i != 0)
	    પાછા i;			    // this character different
	જો (*s1 == NUL)
	    તોડો;			    // strings match until NUL
	++s1;
	++s2;
    }
    પાછા 0;				    // strings match
}
#endif

#જો !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    પૂર્ણાંક
vim_strnicmp(અક્ષર *s1, અક્ષર *s2, size_t len)
{
    પૂર્ણાંક		i;

    જ્યારે (len > 0)
    {
	i = (પૂર્ણાંક)TOLOWER_LOC(*s1) - (પૂર્ણાંક)TOLOWER_LOC(*s2);
	જો (i != 0)
	    પાછા i;			    // this character different
	જો (*s1 == NUL)
	    તોડો;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    પાછા 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    પૂર્ણાંક
vim_strnicmp_asc(અક્ષર *s1, અક્ષર *s2, size_t len)
{
    પૂર્ણાંક                i = 0;

    જ્યારે (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	જો (i != 0)
	    તોડો;			// this character is different
	જો (*s1 == NUL)
	    તોડો;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    પાછા i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, પૂર્ણાંક c)
{
    char_u	*p;
    પૂર્ણાંક		b;

    p = string;
    જો (enc_utf8 && c > 0 && c < 0x80)
	પાછા vim_strbyte(string, c);
    જો (enc_utf8 && c >= 0x80)
    {
	જ્યારે (*p != NUL)
	{
	    પૂર્ણાંક l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    જો (utf_ptr2char(p) == c && l > 1)
		પાછા p;
	    p += l;
	}
	પાછા ખાલી;
    }
    જો (enc_dbcs != 0 && c > 255)
    {
	પૂર્ણાંક	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	જ્યારે ((b = *p) != NUL)
	{
	    જો (b == c && p[1] == n2)
		પાછા p;
	    p += (*mb_ptr2len)(p);
	}
	પાછા ખાલી;
    }
    જો (has_mbyte)
    {
	જ્યારે ((b = *p) != NUL)
	{
	    જો (b == c)
		પાછા p;
	    p += (*mb_ptr2len)(p);
	}
	પાછા ખાલી;
    }
    જ્યારે ((b = *p) != NUL)
    {
	જો (b == c)
	    પાછા p;
	++p;
    }
    પાછા ખાલી;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, પૂર્ણાંક c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    જો (c <= 0 || c > 255)
	પાછા ખાલી;
    પાછા (char_u *)strchr((અક્ષર *)string, c);
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
vim_strrchr(char_u *string, પૂર્ણાંક c)
{
    char_u	*retval = ખાલી;
    char_u	*p = string;

    જ્યારે (*p)
    {
	જો (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    પાછા retval;
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
    જ્યારે (*s)
    {
	જો (vim_strchr(charset, *s) != ખાલી)
	    પાછા s;
	MB_PTR_ADV(s);
    }
    પાછા ખાલી;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
સ્થિર પૂર્ણાંક sort_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2);

    સ્થિર પૂર્ણાંક
sort_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    પાછા STRCMP(*(અક્ષર **)s1, *(અક્ષર **)s2);
}

    શૂન્ય
sort_strings(
    char_u	**files,
    પૂર્ણાંક		ગણતરી)
{
    qsort((શૂન્ય *)files, (size_t)ગણતરી, કદ(char_u *), sort_compare);
}

#જો defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    પૂર્ણાંક
has_non_ascii(char_u *s)
{
    char_u	*p;

    જો (s != ખાલી)
	માટે (p = s; *p != NUL; ++p)
	    જો (*p >= 128)
		પાછા TRUE;
    પાછા FALSE;
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
    size_t  l = str1 == ખાલી ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == ખાલી ? 0 : STRLEN(str2)) + 1L);
    જો (dest == ખાલી)
	પાછા ખાલી;
    જો (str1 == ખાલી)
	*dest = NUL;
    નહીંતર
	STRCPY(dest, str1);
    જો (str2 != ખાલી)
	STRCPY(dest + l, str2);
    પાછા dest;
}

#જો defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    જો (rev == ખાલી)
	પાછા ખાલી;

    માટે (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	જો (has_mbyte)
	{
	    પૂર્ણાંક mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	નહીંતર
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    પાછા rev;
}
#endif

#જો defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, પૂર્ણાંક function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    જો (str != ખાલી)
    {
	len += (unsigned)STRLEN(str);
	માટે (p = str; *p != NUL; MB_PTR_ADV(p))
	    જો (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    જો (r == ખાલી)
	પાછા ખાલી;

    જો (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    નહીંતર
	*r++ = '\'';
    જો (str != ખાલી)
	માટે (p = str; *p != NUL; )
	{
	    જો (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    જો (function)
	*r++ = ')';
    *r++ = NUL;
    પાછા s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_ગણતરી(char_u *haystack, char_u *needle, પૂર્ણાંક ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    જો (p == ખાલી || needle == ખાલી || *needle == NUL)
	પાછા 0;

    size_t  needlelen = STRLEN(needle);
    જો (ic)
    {
	જ્યારે (*p != NUL)
	{
	    જો (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    નહીંતર
		MB_PTR_ADV(p);
	}
    }
    નહીંતર
	જ્યારે ((next = (char_u *)strstr((અક્ષર *)p, (અક્ષર *)needle)) != ખાલી)
	{
	    ++n;
	    p = next + needlelen;
	}

    પાછા n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    સ્થિર પૂર્ણાંક
copy_first_char_to_tv(char_u *આગત, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    પૂર્ણાંક		len;

    જો (આગત == ખાલી || output == ખાલી)
	પાછા -1;

    len = has_mbyte ? mb_ptr2len(આગત) : 1;
    STRNCPY(buf, આગત, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    પાછા output->vval.v_string == ખાલી ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    શૂન્ય
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*પદાવલિ,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		idx = 0;
    પૂર્ણાંક		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ચાવી, VAR_સંખ્યા);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_પદાવલિ_get_funccal(પદાવલિ, &newtv);

    ga_init2(&ga, કદ(અક્ષર), 80);
    માટે (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	જો (len < 0)
	    તોડો;

	set_vim_var_nr(VV_ચાવી, idx);
	જો (filter_map_one(&tv, પદાવલિ, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    તોડો;
	}
	જો (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    જો (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		તોડો;
	    }
	    નહીંતર
		ga_concat(&ga, newtv.vval.v_string);
	}
	નહીંતર જો (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    જો (fc != ખાલી)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    શૂન્ય
string_reduce(
	typval_T	*argvars,
	typval_T	*પદાવલિ,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    પૂર્ણાંક		len;
    typval_T	argv[3];
    પૂર્ણાંક		r;
    પૂર્ણાંક		called_emsg_start = called_emsg;
    funccall_T	*fc;

    જો (argvars[2].v_type == VAR_UNKNOWN)
    {
	જો (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_કિંમત), "String");
	    પાછા;
	}
	len = copy_first_char_to_tv(p, rettv);
	જો (len < 0)
	    પાછા;
	p += len;
    }
    નહીંતર જો (check_for_string_arg(argvars, 2) == FAIL)
	પાછા;
    નહીંતર
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_પદાવલિ_get_funccal(પદાવલિ, rettv);

    માટે ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	જો (len < 0)
	{
	    *rettv = argv[0];
	    તોડો;
	}

	r = eval_પદાવલિ_typval(પદાવલિ, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	જો (r == FAIL || called_emsg != called_emsg_start)
	    તોડો;
    }

    જો (fc != ખાલી)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    સ્થિર શૂન્ય
byteidx_common(typval_T *argvars, typval_T *rettv, પૂર્ણાંક comp)
{
    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	પાછા;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_સંખ્યા_chk(&argvars[1], ખાલી);
    જો (str == ખાલી || idx < 0)
	પાછા;

    varnumber_T	utf16idx = FALSE;
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	પૂર્ણાંક error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	જો (error)
	    પાછા;
	જો (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_સંખ્યા_as_bool_nr), utf16idx);
	    પાછા;
	}
    }

    પૂર્ણાંક (*ptr2len)(char_u *);
    જો (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    નહીંતર
	ptr2len = mb_ptr2len;

    char_u *t = str;
    માટે ( ; idx > 0; idx--)
    {
	જો (*t == NUL)		// EOL reached
	    પાછા;
	જો (utf16idx)
	{
	    પૂર્ણાંક clen = ptr2len(t);
	    પૂર્ણાંક c = (clen > 1) ? utf_ptr2char(t) : *t;
	    જો (c > 0xFFFF)
		idx--;
	    જો (idx > 0)
		t += clen;
	}
	નહીંતર જો (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_સંખ્યા = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    શૂન્ય
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    શૂન્ય
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    શૂન્ય
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = -1;

    જો (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	પાછા;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_સંખ્યા_chk(&argvars[1], ખાલી);
    જો (str == ખાલી || idx < 0)
	પાછા;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	જો (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    પૂર્ણાંક (*ptr2len)(char_u *);
    જો (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    નહીંતર
	ptr2len = mb_ptr2len;

    char_u	*p;
    પૂર્ણાંક		len;
    માટે (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	જો (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    જો (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_સંખ્યા = len;
	    પાછા;
	}
	જો (utf16idx)
	{
	    idx--;
	    પૂર્ણાંક clen = ptr2len(p);
	    પૂર્ણાંક c = (clen > 1) ? utf_ptr2char(p) : *p;
	    જો (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_સંખ્યા = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    સ્થિર પૂર્ણાંક
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    જો (convert_setup(&vimconv, from, to) == FAIL)
	પાછા FAIL;
    vimconv.vc_fail = TRUE;
    જો (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	જો (ret->string == ખાલી)
	    ret->length = 0;
	નહીંતર
	    ret->length = str->length;
    }
    નહીંતર
    {
	પૂર્ણાંક len = (પૂર્ણાંક)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, ખાલી, ખાલી);

    પાછા (ret->string == ખાલી) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    સ્થિર શૂન્ય
blob_from_string(char_u *str, blob_T *blob)
{
    પૂર્ણાંક	    len = (પૂર્ણાંક)STRLEN(str);
    char_u  *dest;

    જો (len == 0)
	પાછા;
    જો (ga_grow(&blob->bv_ga, len) == FAIL)
	પાછા;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    માટે (પૂર્ણાંક i = 0; i < len; ++i)
	જો (dest[i] == NL)
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
    સ્થિર પૂર્ણાંક
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	લીટી_len;

    જો (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	પાછા (ret->string == ખાલી) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    લીટી_len = (nl == ખાલી) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(લીટી_len + 1);
    જો (ret->string == ખાલી)
    {
	ret->length = 0;
	પાછા FAIL;
    }
    જો (લીટી_len > 0)
	mch_memmove(ret->string, src, (size_t)લીટી_len);
    ret->string[લીટી_len] = NUL;
    ret->length = (size_t)લીટી_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    માટે (long i = 0; i < લીટી_len; i++)
	જો (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + લીટી_len + (nl != ખાલી ? 1 : 0);
    પાછા OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    સ્થિર char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    જો (from_encoding_raw == ખાલી)
	પાછા ખાલી;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    જ્યારે (*s != NUL)
    {
	જો (*s == '_')
	    *pe++ = '-';
	નહીંતર
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    જો ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    પાછા from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    સ્થિર શૂન્ય
append_converted_string_to_list(
	string_T *converted,
	પૂર્ણાંક validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    જો (converted->string != ખાલી)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	જ્યારે (p < end)
	{
	    string_T	લીટી;
	    char_u	*લીટી_start = p;

	    જ્યારે (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    લીટી.length = (size_t)(p - લીટી_start);
	    લીટી.string = vim_strnsave(લીટી_start, લીટી.length);
	    જો (લીટી.string != ખાલી)
	    {
		જો (validate_utf8 && !utf_valid_string(લીટી.string, ખાલી))
		{
		    vim_free(લીટી.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    પાછા; // Stop processing
		}
		જો (list_append_string(list, લીટી.string, (પૂર્ણાંક)લીટી.length) == FAIL)
		{
		    vim_free(લીટી.string);
		    vim_free(converted->string);
		    પાછા; // Stop processing on append failure
		}
		vim_free(લીટી.string);
	    }
	    નહીંતર
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		પાછા;
	    }

	    જો (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    નહીંતર
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    સ્થિર પૂર્ણાંક
append_validated_લીટી_to_list(string_T *લીટી, પૂર્ણાંક validate_utf8, list_T *list)
{
    જો (validate_utf8 && !utf_valid_string(લીટી->string, ખાલી))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(લીટી->string);
	પાછા FAIL;
    }

    પૂર્ણાંક ret = list_append_string(list, લીટી->string, (પૂર્ણાંક)લીટી->length);
    vim_free(લીટી->string);
    પાછા ret;
}

    શૂન્ય
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    પૂર્ણાંક		blen;
    long	idx;
    પૂર્ણાંક		validate_utf8 = FALSE;

    જો (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	પાછા;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    blob = argvars->vval.v_blob;
    જો (blob == ખાલી)
	પાછા;
    blen = blob_len(blob);

    char_u	*from_encoding = ખાલી;
    char_u	*from_encoding_raw = ખાલી;  // Encoding name with endianness preserved for iconv
    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	જો (d != ખાલી)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    જો (enc != ખાલી)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		જો (from_encoding_raw == ખાલી)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    પાછા;
		}
	    }
	}
    }

    જો (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    જો (from_encoding != ખાલી && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    પૂર્ણાંક from_prop = 0;
    જો (from_encoding != ખાલી)
	from_prop = enc_canon_props(from_encoding);
    જો (from_encoding != ખાલી && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	પૂર્ણાંક nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	જો (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    જો (blen > 0)
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
	જો (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    જાઓ done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	પૂર્ણાંક len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, ખાલી);
	converted.length = len;
	convert_setup(&vimconv, ખાલી, ખાલી);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    નહીંતર
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	જ્યારે (idx < blen)
	{
	    string_T	str;

	    જો (string_from_blob(blob, &idx, &str) != OK)
		તોડો;

	    જો (from_encoding != ખાલી)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		પૂર્ણાંક	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		જો (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    જાઓ done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    જો (append_validated_લીટી_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		જાઓ done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    જો (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

done:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    શૂન્ય
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    જો (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	પાછા;

    જો (rettv_blob_alloc(rettv) == FAIL)
	પાછા;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    જો (list == ખાલી)
	પાછા;

    char_u	*to_encoding = ખાલી;
    char_u	*to_encoding_raw = ખાલી;  // Encoding name with endianness preserved for iconv
    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	જો (d != ખાલી)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    જો (enc != ખાલી)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		જો (to_encoding_raw == ખાલી)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    પાછા;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    પૂર્ણાંક to_prop = 0;
    જો (to_encoding != ખાલી)
	to_prop = enc_canon_props(to_encoding);
    જો (to_encoding != ખાલી && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_વસ્તુઓ(list, li)
	{
	    char_u *s;

	    જો (li->li_tv.v_type != VAR_STRING)
		ચાલુ;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    જો (li != list->lv_first)
		ga_append(&str_ga, NL);
	    જો (s != ખાલી && *s != NUL)
	    {
		પૂર્ણાંક slen = (પૂર્ણાંક)STRLEN(s);

		જો (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    જાઓ done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	જો (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    જો (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		જાઓ done;
	    }
	    vimconv.vc_fail = TRUE;

	    પૂર્ણાંક		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, ખાલી);
	    convert_setup(&vimconv, ખાલી, ખાલી);
	    ga_clear(&str_ga);

	    જો (converted == ખાલી)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		જાઓ done;
	    }
	    જો (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	નહીંતર
	    ga_clear(&str_ga);
    }
    નહીંતર
    {
	FOR_ALL_LIST_વસ્તુઓ(list, li)
	{
	    જો (li->li_tv.v_type != VAR_STRING)
		ચાલુ;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    જો (str.string == ખાલી)
		STR_LITERAL_SET(str, "");
	    નહીંતર
		str.length = STRLEN(str.string);

	    જો (to_encoding != ખાલી)
	    {
		પૂર્ણાંક	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		જો (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    જાઓ done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    જો (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    જો (to_encoding != ખાલી)
		vim_free(str.string);
	}
    }

done:
    જો (to_encoding != ખાલી)
	vim_free(to_encoding);
    જો (to_encoding_raw != ખાલી)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    શૂન્ય
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    પૂર્ણાંક		utf8 = FALSE;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (પૂર્ણાંક)tv_get_bool_chk(&argvars[1], ખાલી);

    p = tv_get_string(&argvars[0]);

    જો (has_mbyte || utf8)
    {
	પૂર્ણાંક (*ptr2len)(char_u *);
	પૂર્ણાંક (*ptr2char)(char_u *);

	જો (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	નહીંતર
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	માટે ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_સંખ્યા(rettv->vval.v_list, (*ptr2char)(p));
    }
    નહીંતર
	માટે ( ; *p != NUL; ++p)
	    list_append_સંખ્યા(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    શૂન્ય
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		base = 10;
    char_u	*p;
    varnumber_T	n;
    પૂર્ણાંક		what = 0;
    પૂર્ણાંક		isneg;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[1]);
	જો (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    પાછા;
	}
	જો (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    જો (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    પસંદગી (base)
    {
	સ્થિતિ 2: what |= STR2NR_BIN + STR2NR_FORCE; તોડો;
	સ્થિતિ 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; તોડો;
	સ્થિતિ 16: what |= STR2NR_HEX + STR2NR_FORCE; તોડો;
    }
    vim_str2nr(p, ખાલી, ખાલી, what, &n, ખાલી, 0, FALSE, ખાલી);
    // Text after the number is silently ignored.
    જો (isneg)
	rettv->vval.v_સંખ્યા = -n;
    નહીંતર
	rettv->vval.v_સંખ્યા = n;

}

/*
 * "strgetchar()" function
 */
    શૂન્ય
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    પૂર્ણાંક		len;
    પૂર્ણાંક		error = FALSE;
    પૂર્ણાંક		charidx;
    પૂર્ણાંક		byteidx = 0;

    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    str = tv_get_string_chk(&argvars[0]);
    જો (str == ખાલી)
	પાછા;
    len = (પૂર્ણાંક)STRLEN(str);
    charidx = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[1], &error);
    જો (error)
	પાછા;

    જ્યારે (charidx >= 0 && byteidx < len)
    {
	જો (charidx == 0)
	{
	    rettv->vval.v_સંખ્યા = mb_ptr2char(str + byteidx);
	    તોડો;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    શૂન્ય
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    પૂર્ણાંક		start_idx;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL))
	પાછા;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_સંખ્યા = -1;
    જો (needle == ખાલી || haystack == ખાલી)
	પાછા;		// type error; errmsg already given

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	પૂર્ણાંક	    error = FALSE;

	start_idx = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[2], &error);
	જો (error || start_idx >= (પૂર્ણાંક)STRLEN(haystack))
	    પાછા;
	જો (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((અક્ષર *)haystack, (અક્ષર *)needle);
    જો (pos != ખાલી)
	rettv->vval.v_સંખ્યા = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    શૂન્ય
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    જો (rettv->vval.v_string != ખાલી && tofree == ખાલી)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    શૂન્ય
f_strlen(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && check_for_string_or_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    સ્થિર શૂન્ય
strchar_common(typval_T *argvars, typval_T *rettv, પૂર્ણાંક skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    પૂર્ણાંક			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    જ્યારે (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_સંખ્યા = len;
}

/*
 * "strcharlen()" function
 */
    શૂન્ય
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && check_for_string_or_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    શૂન્ય
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	પૂર્ણાંક error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	જો (error)
	    પાછા;
	જો (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_સંખ્યા_as_bool_nr), skipcc);
	    પાછા;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    શૂન્ય
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = -1;

    જો (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	પાછા;

    varnumber_T countcc = FALSE;
    જો (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    પૂર્ણાંક			(*func_mb_ptr2char_adv)(char_u **pp);
    પૂર્ણાંક			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    જ્યારે (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	જો (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_સંખ્યા = len;
}

/*
 * "strdisplaywidth()" function
 */
    શૂન્ય
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    પૂર્ણાંક		col = 0;

    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    s = tv_get_string(&argvars[0]);
    જો (argvars[1].v_type != VAR_UNKNOWN)
	col = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[1]);

    rettv->vval.v_સંખ્યા = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    શૂન્ય
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_સંખ્યા = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    શૂન્ય
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    પૂર્ણાંક		nchar;
    પૂર્ણાંક		nbyte = 0;
    પૂર્ણાંક		charlen;
    પૂર્ણાંક		skipcc = FALSE;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		slen;
    પૂર્ણાંક		error = FALSE;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	પાછા;

    p = tv_get_string(&argvars[0]);
    slen = (પૂર્ણાંક)STRLEN(p);

    nchar = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[1], &error);
    જો (!error)
    {
	જો (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    જો (error)
		પાછા;
	    જો (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_સંખ્યા_as_bool_nr), skipcc);
		પાછા;
	    }
	}

	જો (nchar > 0)
	    જ્યારે (nchar > 0 && nbyte < slen)
	    {
		જો (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		નહીંતર
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	નહીંતર
	    nbyte = nchar;
	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[2]);
	    જ્યારે (charlen > 0 && nbyte + len < slen)
	    {
		પૂર્ણાંક off = nbyte + len;

		જો (off < 0)
		    len += 1;
		નહીંતર
		{
		    જો (skipcc)
			len += mb_ptr2len(p + off);
		    નહીંતર
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	નહીંતર
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    જો (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    નહીંતર જો (nbyte > slen)
	nbyte = slen;
    જો (len < 0)
	len = 0;
    નહીંતર જો (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    શૂન્ય
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    પૂર્ણાંક		n;
    પૂર્ણાંક		len;
    પૂર્ણાંક		slen;
    પૂર્ણાંક		error = FALSE;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	પાછા;

    p = tv_get_string(&argvars[0]);
    slen = (પૂર્ણાંક)STRLEN(p);

    n = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[1], &error);
    જો (error)
	len = 0;
    નહીંતર જો (argvars[2].v_type != VAR_UNKNOWN)
	len = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[2]);
    નહીંતર
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    જો (n < 0)
    {
	len += n;
	n = 0;
    }
    નહીંતર જો (n > slen)
	n = slen;
    જો (len < 0)
	len = 0;
    નહીંતર જો (n + len > slen)
	len = slen - n;

    જો (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	પૂર્ણાંક off;

	// length in characters
	માટે (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    શૂન્ય
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = ખાલી;
    પૂર્ણાંક		haystack_len, end_idx;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL))
	પાછા;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_સંખ્યા = -1;
    જો (needle == ખાલી || haystack == ખાલી)
	પાછા;		// type error; errmsg already given

    haystack_len = (પૂર્ણાંક)STRLEN(haystack);
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[2], ખાલી);
	જો (end_idx < 0)
	    પાછા;	// can never find a match
    }
    નહીંતર
	end_idx = haystack_len;

    જો (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    નહીંતર
    {
	માટે (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((અક્ષર *)rest, (અક્ષર *)needle);
	    જો (rest == ખાલી || rest > haystack + end_idx)
		તોડો;
	    lastmatch = rest;
	}
    }

    જો (lastmatch == ખાલી)
	rettv->vval.v_સંખ્યા = -1;
    નહીંતર
	rettv->vval.v_સંખ્યા = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    શૂન્ય
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    શૂન્ય
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = -1;

    જો (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    પાછા;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_સંખ્યા_chk(&argvars[1], ખાલી);
    જો (str == ખાલી || idx < 0)
	પાછા;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	જો (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    પૂર્ણાંક (*ptr2len)(char_u *);
    જો (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    નહીંતર
	ptr2len = mb_ptr2len;

    char_u	*p;
    પૂર્ણાંક		len;
    પૂર્ણાંક		utf16idx = 0;
    માટે (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	જો (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    જો (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_સંખ્યા = len;
	    પાછા;
	}
	utf16idx = len;
	પૂર્ણાંક clen = ptr2len(p);
	પૂર્ણાંક c = (clen > 1) ? utf_ptr2char(p) : *p;
	જો (c > 0xFFFF)
	    len++;
	p += clen;
	જો (charidx)
	    idx--;
    }

    rettv->vval.v_સંખ્યા = utf16idx;
}

/*
 * "tolower(string)" function
 */
    શૂન્ય
f_tolower(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    શૂન્ય
f_toupper(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    શૂન્ય
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*in_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    પૂર્ણાંક		inlen;
    પૂર્ણાંક		fromlen;
    પૂર્ણાંક		tolen;
    પૂર્ણાંક		idx;
    char_u	*cpstr;
    પૂર્ણાંક		cplen;
    પૂર્ણાંક		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	પાછા;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;
    જો (fromstr == ખાલી || tostr == ખાલી)
	    પાછા;		// type error; errmsg already given
    ga_init2(&ga, કદ(અક્ષર), 80);

    જો (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	જો (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    પાછા;
	}

    // fromstr and tostr have to contain the same number of chars
    જ્યારે (*in_str != NUL)
    {
	જો (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    માટે (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		જો (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    માટે (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			જો (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    તોડો;
			}
		    }
		    જો (*p == NUL)	// tostr is shorter than fromstr
			જાઓ error;
		    તોડો;
		}
		++idx;
	    }

	    જો (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		માટે (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		જો (idx != 0)
		    જાઓ error;
	    }

	    (શૂન્ય)ga_grow(&ga, cplen);
	    mch_memmove((અક્ષર *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	નહીંતર
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    જો (p != ખાલી)
		ga_append(&ga, tostr[p - fromstr]);
	    નહીંતર
		ga_append(&ga, *in_str);
	    ++in_str;
	}
    }

    // add a terminating NUL
    (શૂન્ય)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_data;
}

/*
 * "trim({expr})" function
 */
    શૂન્ય
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*head;
    char_u	*mask = ખાલી;
    char_u	*tail;
    char_u	*prev;
    char_u	*p;
    પૂર્ણાંક		c1;
    પૂર્ણાંક		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL)))
	પાછા;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    જો (head == ખાલી)
	પાછા;

    જો (check_for_opt_string_arg(argvars, 1) == FAIL)
	પાછા;

    જો (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	જો (*mask == NUL)
	    mask = ખાલી;

	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    પૂર્ણાંક	error = 0;

	    // leading or trailing characters to trim
	    dir = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[2], &error);
	    જો (error)
		પાછા;
	    જો (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		પાછા;
	    }
	}
    }

    જો (dir == 0 || dir == 1)
    {
	// Trim leading characters
	જ્યારે (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    જો (mask == ખાલી)
	    {
		જો (c1 > ' ' && c1 != 0xa0)
		    તોડો;
	    }
	    નહીંતર
	    {
		માટે (p = mask; *p != NUL; MB_PTR_ADV(p))
		    જો (c1 == PTR2CHAR(p))
			તોડો;
		જો (*p == NUL)
		    તોડો;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    જો (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	માટે (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    જો (mask == ખાલી)
	    {
		જો (c1 > ' ' && c1 != 0xa0)
		    તોડો;
	    }
	    નહીંતર
	    {
		માટે (p = mask; *p != NUL; MB_PTR_ADV(p))
		    જો (c1 == PTR2CHAR(p))
			તોડો;
		જો (*p == NUL)
		    તોડો;
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
    સ્થિર char_u *
uri_decode(char_u *str)
{
    જો (str == ખાલી)
	પાછા ખાલી;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    જો (!decoded)
	પાછા ખાલી;

    char_u	*p = decoded;
    size_t	i = 0;

    જ્યારે (i < len)
    {
	જો (str[i] == '%')
	{
	    જો (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		જો (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    નહીંતર
	    {
		પૂર્ણાંક val = hexhex2nr(&str[i + 1]);
		જો (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		નહીંતર
		{
		    // invalid hex digits following "%"
		    માટે (પૂર્ણાંક j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	નહીંતર
	    *p++ = str[i++];
    }

    *p = NUL;

    પાછા decoded;
}

/*
 * "uri_decode({str})" function
 */
    શૂન્ય
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

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
    સ્થિર char_u *
uri_encode(char_u *str)
{
    જો (str == ખાલી)
	પાછા ખાલી;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    જો (encoded == ખાલી)
	પાછા ખાલી;

    char_u *p = encoded;

    માટે (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	જો (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	નહીંતર
	{
	    sprintf((અક્ષર *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    પાછા encoded;
}

/*
 * "uri_encode({str})" function
 */
    શૂન્ય
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

સ્થિર અક્ષર *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    સ્થિર varnumber_T
tv_nr(typval_T *tvs, પૂર્ણાંક *idxp)
{
    પૂર્ણાંક		idx = *idxp - 1;
    varnumber_T	n = 0;
    પૂર્ણાંક		err = FALSE;

    જો (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    નહીંતર
    {
	++*idxp;
	n = tv_get_સંખ્યા_chk(&tvs[idx], &err);
	જો (err)
	    n = 0;
    }
    પાછા n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    સ્થિર અક્ષર *
tv_str(typval_T *tvs, પૂર્ણાંક *idxp, char_u **tofree)
{
    પૂર્ણાંક		    idx = *idxp - 1;
    અક્ષર	    *s = ખાલી;
    સ્થિર char_u   numbuf[NUMBUFLEN];

    જો (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    નહીંતર
    {
	++*idxp;
	જો (tofree != ખાલી)
	    s = (અક્ષર *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	નહીંતર
	    s = (અક્ષર *)tv_get_string_chk(&tvs[idx]);
    }
    પાછા s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    સ્થિર દ્વિગુણ
tv_float(typval_T *tvs, પૂર્ણાંક *idxp)
{
    પૂર્ણાંક		idx = *idxp - 1;
    દ્વિગુણ	f = 0;

    જો (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    નહીંતર
    {
	++*idxp;
	જો (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	નહીંતર જો (tvs[idx].v_type == VAR_સંખ્યા)
	    f = (દ્વિગુણ)tvs[idx].vval.v_સંખ્યા;
	નહીંતર
	    emsg(_(e_expected_float_argument_for_printf));
    }
    પાછા f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    સ્થિર અચલ અક્ષર *
infinity_str(પૂર્ણાંક positive,
	     અક્ષર fmt_spec,
	     પૂર્ણાંક force_sign,
	     પૂર્ણાંક space_for_positive)
{
    સ્થિર અચલ અક્ષર *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    પૂર્ણાંક idx = positive * (1 + force_sign + force_sign * space_for_positive);

    જો (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    પાછા table[idx];
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
    પૂર્ણાંક
vim_snprintf_add(અક્ષર *str, size_t str_m, અચલ અક્ષર *fmt, ...)
{
    va_list	ap;
    પૂર્ણાંક		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    જો (str_m <= len)
	space = 0;
    નહીંતર
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    પાછા str_l;
}

    પૂર્ણાંક
vim_snprintf(અક્ષર *str, size_t str_m, અચલ અક્ષર *fmt, ...)
{
    va_list	ap;
    પૂર્ણાંક		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    પાછા str_l;
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
vim_snprintf_safelen(અક્ષર *str, size_t str_m, અચલ અક્ષર *fmt, ...)
{
    va_list ap;
    પૂર્ણાંક	    str_l;

    જો (str_m == 0)
	પાછા 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, ખાલી);
    va_end(ap);

    જો (str_l < 0)
    {
	*str = NUL;
	પાછા 0;
    }
    પાછા ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    પૂર્ણાંક
vim_vsnprintf(
    અક્ષર	*str,
    size_t	str_m,
    અચલ અક્ષર	*fmt,
    va_list	ap)
{
    પાછા vim_vsnprintf_typval(str, str_m, fmt, ap, ખાલી);
}

ગણના
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
    સ્થિર પૂર્ણાંક
format_typeof(
    અચલ અક્ષર	*type)
{
    // allowed values: \0, h, l, L
    અક્ષર    length_modifier = '\0';

    // current conversion specifier character
    અક્ષર    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    જો (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	જો (length_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    length_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    પસંદગી (fmt_spec)
    {
	સ્થિતિ 'i': fmt_spec = 'd'; તોડો;
	સ્થિતિ '*': fmt_spec = 'd'; length_modifier = 'h'; તોડો;
	સ્થિતિ 'D': fmt_spec = 'd'; length_modifier = 'l'; તોડો;
	સ્થિતિ 'U': fmt_spec = 'u'; length_modifier = 'l'; તોડો;
	સ્થિતિ 'O': fmt_spec = 'o'; length_modifier = 'l'; તોડો;
	મૂળભૂત: તોડો;
    }

    // get parameter value, do initial processing
    પસંદગી (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    સ્થિતિ '%':
	પાછા TYPE_PERCENT;

    સ્થિતિ 'c':
	પાછા TYPE_CHAR;

    સ્થિતિ 's':
    સ્થિતિ 'S':
	પાછા TYPE_STRING;

    સ્થિતિ 'd': સ્થિતિ 'u':
    સ્થિતિ 'b': સ્થિતિ 'B':
    સ્થિતિ 'o':
    સ્થિતિ 'x': સ્થિતિ 'X':
    સ્થિતિ 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //કિંમત

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    જો (fmt_spec == 'p')
		પાછા TYPE_POINTER;
	    નહીંતર જો (fmt_spec == 'b' || fmt_spec == 'B')
		પાછા TYPE_UNSIGNEDLONGLONGINT;
	    નહીંતર જો (fmt_spec == 'd')
	    {
		// signed
		પસંદગી (length_modifier)
		{
		સ્થિતિ '\0':
		સ્થિતિ 'h':
		    // char and short arguments are passed as int.
		    પાછા TYPE_INT;
		સ્થિતિ 'l':
		    પાછા TYPE_LONGINT;
		સ્થિતિ 'L':
		    પાછા TYPE_LONGLONGINT;
		}
	    }
	    નહીંતર
	    {
		// unsigned
		પસંદગી (length_modifier)
		{
		    સ્થિતિ '\0':
		    સ્થિતિ 'h':
			પાછા TYPE_UNSIGNEDINT;
		    સ્થિતિ 'l':
			પાછા TYPE_UNSIGNEDLONGINT;
		    સ્થિતિ 'L':
			પાછા TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	તોડો;

    સ્થિતિ 'f':
    સ્થિતિ 'F':
    સ્થિતિ 'e':
    સ્થિતિ 'E':
    સ્થિતિ 'g':
    સ્થિતિ 'G':
	પાછા TYPE_FLOAT;
    }

    પાછા TYPE_UNKNOWN;
}

    સ્થિર અક્ષર *
format_typename(
    અચલ અક્ષર  *type)
{
    પસંદગી (format_typeof(type))
    {
	સ્થિતિ TYPE_INT:
	    પાછા typename_int;

	સ્થિતિ TYPE_LONGINT:
	    પાછા typename_longint;

	સ્થિતિ TYPE_LONGLONGINT:
	    પાછા typename_longlongint;

	સ્થિતિ TYPE_UNSIGNEDINT:
	    પાછા typename_unsignedint;

	સ્થિતિ TYPE_UNSIGNEDLONGINT:
	    પાછા typename_unsignedlongint;

	સ્થિતિ TYPE_UNSIGNEDLONGLONGINT:
	    પાછા typename_unsignedlonglongint;

	સ્થિતિ TYPE_POINTER:
	    પાછા _(typename_pointer);

	સ્થિતિ TYPE_PERCENT:
	    પાછા _(typename_percent);

	સ્થિતિ TYPE_CHAR:
	    પાછા typename_char;

	સ્થિતિ TYPE_STRING:
	    પાછા _(typename_string);

	સ્થિતિ TYPE_FLOAT:
	    પાછા typename_float;
    }

    પાછા _(typename_unknown);
}

    સ્થિર પૂર્ણાંક
adjust_types(
    અચલ અક્ષર ***ap_types,
    પૂર્ણાંક arg,
    પૂર્ણાંક *અંક_posarg,
    અચલ અક્ષર *type)
{
    જો (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	પાછા FAIL;
    }

    જો (*ap_types == ખાલી || *અંક_posarg < arg)
    {
	પૂર્ણાંક	    idx;
	અચલ અક્ષર  **new_types;

	જો (*ap_types == ખાલી)
	    new_types = ALLOC_CLEAR_MULT(અચલ અક્ષર *, arg);
	નહીંતર
	    new_types = vim_realloc((અક્ષર **)*ap_types,
						arg * કદ(અચલ અક્ષર *));

	જો (new_types == ખાલી)
	    પાછા FAIL;

	માટે (idx = *અંક_posarg; idx < arg; ++idx)
	    new_types[idx] = ખાલી;

	*ap_types = new_types;
	*અંક_posarg = arg;
    }

    જો ((*ap_types)[arg - 1] != ખાલી)
    {
	જો ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    અચલ અક્ષર *pt = type;
	    જો (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    જો (pt[0] != '*')
	    {
		પસંદગી (pt[0])
		{
		    સ્થિતિ 'd':
		    સ્થિતિ 'i': તોડો;
		    મૂળભૂત:
			semsg(_(e_positional_અંક_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			પાછા FAIL;
		}
	    }
	}
	નહીંતર
	{
	    જો (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_અંક_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		પાછા FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    પાછા OK;
}

    સ્થિર શૂન્ય
format_overflow_error(અચલ અક્ષર *pstart)
{
    size_t	arglen = 0;
    અક્ષર	*argcopy = ખાલી;
    અચલ અક્ષર	*p = pstart;

    જ્યારે (VIM_ISDIGIT((પૂર્ણાંક)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(અક્ષર, arglen + 1);
    જો (argcopy != ખાલી)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	મુક્ત(argcopy);
    }
    નહીંતર
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    સ્થિર પૂર્ણાંક
get_unsigned_int(
    અચલ અક્ષર *pstart,
    અચલ અક્ષર **p,
    unsigned પૂર્ણાંક *uj,
    પૂર્ણાંક overflow_err)
{
    *uj = **p - '0';
    ++*p;

    જ્યારે (VIM_ISDIGIT((પૂર્ણાંક)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned પૂર્ણાંક)(**p - '0');
	++*p;
    }

    જો (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	જો (overflow_err)
	{
	    format_overflow_error(pstart);
	    પાછા FAIL;
	}
	નહીંતર
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    પાછા OK;
}


    સ્થિર પૂર્ણાંક
parse_fmt_types(
    અચલ અક્ષર  ***ap_types,
    પૂર્ણાંક		*અંક_posarg,
    અચલ અક્ષર  *fmt,
    typval_T	*tvs UNUSED
    )
{
    અચલ અક્ષર	*p = fmt;
    અચલ અક્ષર	*arg = ખાલી;

    પૂર્ણાંક		any_pos = 0;
    પૂર્ણાંક		any_arg = 0;
    પૂર્ણાંક		arg_idx;

# define CHECK_POS_ARG કરો { \
    જો (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	જાઓ error; \
    } \
} જ્યારે (0);

    જો (p == ખાલી)
	પાછા OK;

    જ્યારે (*p != NUL)
    {
	જો (*p != '%')
	{
	    અચલ અક્ષર    *q = strchr(p + 1, '%');
	    size_t  n = (q == ખાલી) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	નહીંતર
	{
	    // allowed values: \0, h, l, L
	    અક્ષર	length_modifier = '\0';

	    // variable for positional arg
	    પૂર્ણાંક		pos_arg = -1;
	    અચલ અક્ષર	*ptype = ખાલી;
	    અચલ અક્ષર	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    જ્યારે (VIM_ISDIGIT(*ptype))
		++ptype;

	    જો (*ptype == '$')
	    {
		જો (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    જાઓ error;
		}

		// Positional argument
		unsigned પૂર્ણાંક uj;

		જો (get_unsigned_int(pstart, &p, &uj, tvs != ખાલી) == FAIL)
		    જાઓ error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    જ્યારે (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		પસંદગી (*p)
		{
		    સ્થિતિ '0': તોડો;
		    સ્થિતિ '-': તોડો;
		    સ્થિતિ '+': તોડો;
		    સ્થિતિ ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      તોડો;
		    સ્થિતિ '#': તોડો;
		    સ્થિતિ '\'': તોડો;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    જો (*(arg = p) == '*')
	    {
		p++;

		જો (VIM_ISDIGIT((પૂર્ણાંક)(*p)))
		{
		    // Positional argument field width
		    unsigned પૂર્ણાંક uj;

		    જો (get_unsigned_int(arg + 1, &p, &uj, tvs != ખાલી) == FAIL)
			જાઓ error;

		    જો (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			જાઓ error;
		    }
		    નહીંતર
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			જો (adjust_types(ap_types, uj, અંક_posarg, arg) == FAIL)
			    જાઓ error;
		    }
		}
		નહીંતર
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    નહીંતર જો (VIM_ISDIGIT((પૂર્ણાંક)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		અચલ અક્ષર *digstart = p;
		unsigned પૂર્ણાંક uj;

		જો (get_unsigned_int(digstart, &p, &uj, tvs != ખાલી) == FAIL)
		    જાઓ error;

		જો (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    જાઓ error;
		}
	    }

	    // parse precision
	    જો (*p == '.')
	    {
		p++;

		જો (*(arg = p) == '*')
		{
		    p++;

		    જો (VIM_ISDIGIT((પૂર્ણાંક)(*p)))
		    {
			// Parse precision
			unsigned પૂર્ણાંક uj;

			જો (get_unsigned_int(arg + 1, &p, &uj, tvs != ખાલી) == FAIL)
			    જાઓ error;

			જો (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    જો (adjust_types(ap_types, uj, અંક_posarg, arg) == FAIL)
				જાઓ error;
			}
			નહીંતર
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    જાઓ error;
			}
		    }
		    નહીંતર
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		નહીંતર જો (VIM_ISDIGIT((પૂર્ણાંક)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    અચલ અક્ષર *digstart = p;
		    unsigned પૂર્ણાંક uj;

		    જો (get_unsigned_int(digstart, &p, &uj, tvs != ખાલી) == FAIL)
			જાઓ error;

		    જો (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			જાઓ error;
		    }
		}
	    }

	    જો (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    જો (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		જો (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    પસંદગી (*p)
	    {
		// Check for known format specifiers. % is special!
		સ્થિતિ 'i':
		સ્થિતિ '*':
		સ્થિતિ 'd':
		સ્થિતિ 'u':
		સ્થિતિ 'o':
		સ્થિતિ 'D':
		સ્થિતિ 'U':
		સ્થિતિ 'O':
		સ્થિતિ 'x':
		સ્થિતિ 'X':
		સ્થિતિ 'b':
		સ્થિતિ 'B':
		સ્થિતિ 'c':
		સ્થિતિ 's':
		સ્થિતિ 'S':
		સ્થિતિ 'p':
		સ્થિતિ 'f':
		સ્થિતિ 'F':
		સ્થિતિ 'e':
		સ્થિતિ 'E':
		સ્થિતિ 'g':
		સ્થિતિ 'G':
		    જો (pos_arg != -1)
		    {
			જો (adjust_types(ap_types, pos_arg, અંક_posarg, ptype) == FAIL)
			    જાઓ error;
		    }
		    નહીંતર
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    તોડો;

		મૂળભૂત:
		    જો (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			જાઓ error;
		    }
	    }

	    જો (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    માટે (arg_idx = 0; arg_idx < *અંક_posarg; ++arg_idx)
    {
	જો ((*ap_types)[arg_idx] == ખાલી)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    જાઓ error;
	}

# જો defined(FEAT_EVAL)
	જો (tvs != ખાલી && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    જાઓ error;
	}
# endif
    }

    પાછા OK;

error:
    vim_free((અક્ષર**)*ap_types);
    *ap_types = ખાલી;
    *અંક_posarg = 0;
    પાછા FAIL;
}

    સ્થિર શૂન્ય
skip_to_arg(
    અચલ અક્ષર	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    પૂર્ણાંક		*arg_idx,
    પૂર્ણાંક		*arg_cur,
    અચલ અક્ષર	*fmt)
{
    પૂર્ણાંક		arg_min = 0;

    જો (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	પાછા;
    }

    જો (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    નહીંતર
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    માટે (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	અચલ અક્ષર *p;

	જો (ap_types == ખાલી || ap_types[*arg_cur] == ખાલી)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    પાછા;
	}

	p = ap_types[*arg_cur];

	પૂર્ણાંક fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	પસંદગી (fmt_type)
	{
	સ્થિતિ TYPE_PERCENT:
	સ્થિતિ TYPE_UNKNOWN:
	    તોડો;

	સ્થિતિ TYPE_CHAR:
	    va_arg(*ap, પૂર્ણાંક);
	    તોડો;

	સ્થિતિ TYPE_STRING:
	    va_arg(*ap, અક્ષર *);
	    તોડો;

	સ્થિતિ TYPE_POINTER:
	    va_arg(*ap, શૂન્ય *);
	    તોડો;

	સ્થિતિ TYPE_INT:
	    va_arg(*ap, પૂર્ણાંક);
	    તોડો;

	સ્થિતિ TYPE_LONGINT:
	    va_arg(*ap, long પૂર્ણાંક);
	    તોડો;

	સ્થિતિ TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    તોડો;

	સ્થિતિ TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned પૂર્ણાંક);
	    તોડો;

	સ્થિતિ TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long પૂર્ણાંક);
	    તોડો;

	સ્થિતિ TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    તોડો;

	સ્થિતિ TYPE_FLOAT:
	    va_arg(*ap, દ્વિગુણ);
	    તોડો;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    પાછા;
}

    પૂર્ણાંક
vim_vsnprintf_typval(
    અક્ષર	*str,
    size_t	str_m,
    અચલ અક્ષર	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    અચલ અક્ષર	*p = fmt;
    પૂર્ણાંક		arg_cur = 0;
    પૂર્ણાંક		અંક_posarg = 0;
    પૂર્ણાંક		arg_idx = 1;
    va_list	ap;
    અચલ અક્ષર	**ap_types = ખાલી;

    જો (parse_fmt_types(&ap_types, &અંક_posarg, fmt, tvs) == FAIL)
	પાછા 0;

    va_copy(ap, ap_start);

    જો (p == ખાલી)
	p = "";
    જ્યારે (*p != NUL)
    {
	જો (*p != '%')
	{
	    અચલ અક્ષર    *q = strchr(p + 1, '%');
	    size_t  n = (q == ખાલી) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    જો (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	નહીંતર
	{
	    size_t  min_field_width = 0, precision = 0;
	    પૂર્ણાંક	    zero_padding = 0, precision_specified = 0, justify_ડાબું = 0;
	    પૂર્ણાંક	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    પૂર્ણાંક	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    અક્ષર    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    અક્ષર    tmp[TMP_LEN];

	    // string address in case of string argument
	    અચલ અક્ષર  *str_arg = ખાલી;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    unsigned અક્ષર uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  સંખ્યા_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    અક્ષર    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = ખાલી;

	    // variables for positional arg
	    પૂર્ણાંક	    pos_arg = -1;
	    અચલ અક્ષર	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    જ્યારે (VIM_ISDIGIT(*ptype))
		++ptype;

	    જો (*ptype == '$')
	    {
		// Positional argument
		અચલ અક્ષર *digstart = p;
		unsigned પૂર્ણાંક uj;

		જો (get_unsigned_int(digstart, &p, &uj, tvs != ખાલી) == FAIL)
		    જાઓ error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    જ્યારે (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		પસંદગી (*p)
		{
		    સ્થિતિ '0': zero_padding = 1; તોડો;
		    સ્થિતિ '-': justify_ડાબું = 1; તોડો;
		    સ્થિતિ '+': force_sign = 1; space_for_positive = 0; તોડો;
		    સ્થિતિ ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      તોડો;
		    સ્થિતિ '#': alternate_form = 1; તોડો;
		    સ્થિતિ '\'': તોડો;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    જો (*p == '*')
	    {
		પૂર્ણાંક j;
		અચલ અક્ષર *digstart = p + 1;

		p++;

		જો (VIM_ISDIGIT((પૂર્ણાંક)(*p)))
		{
		    // Positional argument field width
		    unsigned પૂર્ણાંક uj;

		    જો (get_unsigned_int(digstart, &p, &uj, tvs != ખાલી) == FAIL)
			જાઓ error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# જો defined(FEAT_EVAL)
		    tvs != ખાલી ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, પૂર્ણાંક));

		જો (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    જો (tvs != ખાલી)
		    {
			format_overflow_error(digstart);
			જાઓ error;
		    }
		    નહીંતર
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		જો (j >= 0)
		    min_field_width = j;
		નહીંતર
		{
		    min_field_width = -j;
		    justify_ડાબું = 1;
		}
	    }
	    નહીંતર જો (VIM_ISDIGIT((પૂર્ણાંક)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		અચલ અક્ષર *digstart = p;
		unsigned પૂર્ણાંક uj;

		જો (get_unsigned_int(digstart, &p, &uj, tvs != ખાલી) == FAIL)
		    જાઓ error;

		min_field_width = uj;
	    }

	    // parse precision
	    જો (*p == '.')
	    {
		p++;
		precision_specified = 1;

		જો (VIM_ISDIGIT((પૂર્ણાંક)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    અચલ અક્ષર *digstart = p;
		    unsigned પૂર્ણાંક uj;

		    જો (get_unsigned_int(digstart, &p, &uj, tvs != ખાલી) == FAIL)
			જાઓ error;

		    precision = uj;
		}
		નહીંતર જો (*p == '*')
		{
		    પૂર્ણાંક j;
		    અચલ અક્ષર *digstart = p;

		    p++;

		    જો (VIM_ISDIGIT((પૂર્ણાંક)(*p)))
		    {
			// positional argument
			unsigned પૂર્ણાંક uj;

			જો (get_unsigned_int(digstart, &p, &uj, tvs != ખાલી) == FAIL)
			    જાઓ error;

			arg_idx = uj;

			++p;
		    }

		    j =
# જો defined(FEAT_EVAL)
			tvs != ખાલી ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, પૂર્ણાંક));

		    જો (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			જો (tvs != ખાલી)
			{
			    format_overflow_error(digstart);
			    જાઓ error;
			}
			નહીંતર
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    જો (j >= 0)
			precision = j;
		    નહીંતર
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    જો (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		જો (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    length_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    પસંદગી (fmt_spec)
	    {
		સ્થિતિ 'i': fmt_spec = 'd'; તોડો;
		સ્થિતિ 'D': fmt_spec = 'd'; length_modifier = 'l'; તોડો;
		સ્થિતિ 'U': fmt_spec = 'u'; length_modifier = 'l'; તોડો;
		સ્થિતિ 'O': fmt_spec = 'o'; length_modifier = 'l'; તોડો;
		મૂળભૂત: તોડો;
	    }

# જો defined(FEAT_EVAL)
	    પસંદગી (fmt_spec)
	    {
		સ્થિતિ 'd': સ્થિતિ 'u': સ્થિતિ 'o': સ્થિતિ 'x': સ્થિતિ 'X':
		    જો (tvs != ખાલી && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    જો (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    પસંદગી (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    સ્થિતિ '%':
	    સ્થિતિ 'c':
	    સ્થિતિ 's':
	    સ્થિતિ 'S':
		str_arg_l = 1;
		પસંદગી (fmt_spec)
		{
		સ્થિતિ '%':
		    str_arg = p;
		    તોડો;

		સ્થિતિ 'c':
		    {
			પૂર્ણાંક j;

			j =
# જો defined(FEAT_EVAL)
			    tvs != ખાલી ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, પૂર્ણાંક));

			// standard demands unsigned char
			uchar_arg = (unsigned અક્ષર)j;
			str_arg = (અક્ષર *)&uchar_arg;
			તોડો;
		    }

		સ્થિતિ 's':
		સ્થિતિ 'S':
		    str_arg =
# જો defined(FEAT_EVAL)
				tvs != ખાલી ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, અક્ષર *));

		    જો (str_arg == ખાલી)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    નહીંતર જો (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    નહીંતર જો (precision == 0)
			str_arg_l = 0;
		    નહીંતર
		    {
			// memchr on HP does not like n > 2^31  !!!
			અચલ અક્ષર *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == ખાલી) ? precision
						      : (size_t)(q - str_arg);
		    }
		    જો (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			પૂર્ણાંક	cell;

			માટે (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    જો (precision_specified && i + cell > precision)
				તોડો;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			જો (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    તોડો;

		મૂળભૂત:
		    તોડો;
		}
		તોડો;

	    સ્થિતિ 'd': સ્થિતિ 'u':
	    સ્થિતિ 'b': સ્થિતિ 'B':
	    સ્થિતિ 'o':
	    સ્થિતિ 'x': સ્થિતિ 'X':
	    સ્થિતિ 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //કિંમત

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    પૂર્ણાંક arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    પૂર્ણાંક int_arg = 0;
		    unsigned પૂર્ણાંક uint_arg = 0;

		    // only set for length modifier l
		    long પૂર્ણાંક long_arg = 0;
		    unsigned long પૂર્ણાંક ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    શૂન્ય *ptr_arg = ખાલી;

		    જો (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# જો defined(FEAT_EVAL)
				 tvs != ખાલી ? (શૂન્ય *)tv_str(tvs, &arg_idx,
									ખાલી) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, શૂન્ય *));

			જો (ptr_arg != ખાલી)
			    arg_sign = 1;
		    }
		    નહીંતર જો (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# જો defined(FEAT_EVAL)
				    tvs != ખાલી ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			જો (bin_arg != 0)
			    arg_sign = 1;
		    }
		    નહીંતર જો (fmt_spec == 'd')
		    {
			// signed
			પસંદગી (length_modifier)
			{
			સ્થિતિ '\0':
			સ્થિતિ 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# જો defined(FEAT_EVAL)
					tvs != ખાલી ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, પૂર્ણાંક));

			    જો (int_arg > 0)
				arg_sign =  1;
			    નહીંતર જો (int_arg < 0)
				arg_sign = -1;
			    તોડો;
			સ્થિતિ 'l':
			    long_arg =
# જો defined(FEAT_EVAL)
					tvs != ખાલી ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long પૂર્ણાંક));

			    જો (long_arg > 0)
				arg_sign =  1;
			    નહીંતર જો (long_arg < 0)
				arg_sign = -1;
			    તોડો;
			સ્થિતિ 'L':
			    llong_arg =
# જો defined(FEAT_EVAL)
					tvs != ખાલી ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    જો (llong_arg > 0)
				arg_sign =  1;
			    નહીંતર જો (llong_arg < 0)
				arg_sign = -1;
			    તોડો;
			}
		    }
		    નહીંતર
		    {
			// unsigned
			પસંદગી (length_modifier)
			{
			    સ્થિતિ '\0':
			    સ્થિતિ 'h':
				uint_arg =
# જો defined(FEAT_EVAL)
					    tvs != ખાલી ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned પૂર્ણાંક));

				જો (uint_arg != 0)
				    arg_sign = 1;
				તોડો;
			    સ્થિતિ 'l':
				ulong_arg =
# જો defined(FEAT_EVAL)
					    tvs != ખાલી ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long પૂર્ણાંક));

				જો (ulong_arg != 0)
				    arg_sign = 1;
				તોડો;
			    સ્થિતિ 'L':
				ullong_arg =
# જો defined(FEAT_EVAL)
					    tvs != ખાલી ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				જો (ullong_arg != 0)
				    arg_sign = 1;
				તોડો;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    જો (precision_specified)
			zero_padding = 0;
		    જો (fmt_spec == 'd')
		    {
			જો (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    નહીંતર જો (alternate_form)
		    {
			જો (arg_sign != 0
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
		    જો (!precision_specified)
			precision = 1;   // default precision is 1
		    જો (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    નહીંતર
		    {
			અક્ષર	f[6];
			પૂર્ણાંક	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			જો (!length_modifier)
			    ;
			નહીંતર જો (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# નહીંતર
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			નહીંતર
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			જો (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			નહીંતર જો (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    અક્ષર	    b[8 * કદ(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    કરો
			    {
				b[કદ(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    જ્યારે (bn != 0);

			    memcpy(tmp + str_arg_l, b + કદ(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			નહીંતર જો (fmt_spec == 'd')
			{
			    // signed
			    પસંદગી (length_modifier)
			    {
			    સ્થિતિ '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       તોડો;
			    સ્થિતિ 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (short)int_arg);
				      તોડો;
			    સ્થિતિ 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      તોડો;
			    સ્થિતિ 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      તોડો;
			    }
			}
			નહીંતર
			{
			    // unsigned
			    પસંદગી (length_modifier)
			    {
			    સ્થિતિ '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       તોડો;
			    સ્થિતિ 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(unsigned short)uint_arg);
				      તોડો;
			    સ્થિતિ 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      તોડો;
			    સ્થિતિ 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      તોડો;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			જો (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			જો (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t અંક_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			જો (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    જો (!precision_specified
					     || precision < અંક_of_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = અંક_of_digits + 1;
			    }
			}
			// zero padding to specified precision?
			જો (અંક_of_digits < precision)
			    સંખ્યા_of_zeros_to_pad = precision - અંક_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    જો (!justify_ડાબું && zero_padding)
		    {
			પૂર્ણાંક n = (પૂર્ણાંક)(min_field_width - (str_arg_l
						    + સંખ્યા_of_zeros_to_pad));
			જો (n > 0)
			    સંખ્યા_of_zeros_to_pad += n;
		    }
		    તોડો;
		}

	    સ્થિતિ 'f':
	    સ્થિતિ 'F':
	    સ્થિતિ 'e':
	    સ્થિતિ 'E':
	    સ્થિતિ 'g':
	    સ્થિતિ 'G':
		{
		    // Floating point.
		    દ્વિગુણ	f;
		    દ્વિગુણ	abs_f;
		    અક્ષર	format[40];
		    પૂર્ણાંક		l;
		    પૂર્ણાંક		remove_trailing_zeroes = FALSE;

		    f =
# જો defined(FEAT_EVAL)
			tvs != ખાલી ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, દ્વિગુણ));

		    abs_f = f < 0 ? -f : f;

		    જો (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			જો ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			નહીંતર
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    જો ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# નહીંતર
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
		    નહીંતર
		    {
			જો (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			નહીંતર જો (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			નહીંતર
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    જો (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    જો (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				જો ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				જો (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (પૂર્ણાંક)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			જો (remove_trailing_zeroes)
			{
			    પૂર્ણાંક i;
			    અક્ષર *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    જો (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    નહીંતર
			    {
				tp = (અક્ષર *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				જો (tp != ખાલી)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    જો (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    જ્યારે (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    જો (tp != ખાલી && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				જ્યારે (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			નહીંતર
			{
			    અક્ષર *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (અક્ષર *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    જો (tp != ખાલી && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    જો (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			સંખ્યા_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    તોડો;
		}

	    મૂળભૂત:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_ડાબું = 1;
		min_field_width = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		જો (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		તોડો;
	    }

	    જો (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    જો (!justify_ડાબું)
	    {
		// left padding with blank or zero
		પૂર્ણાંક pn = (પૂર્ણાંક)(min_field_width - (str_arg_l + સંખ્યા_of_zeros_to_pad));

		જો (pn > 0)
		{
		    જો (str_l < str_m)
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
	    જો (સંખ્યા_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    નહીંતર
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		પૂર્ણાંક zn = (પૂર્ણાંક)zero_padding_insertion_ind;

		જો (zn > 0)
		{
		    જો (str_l < str_m)
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
		zn = (પૂર્ણાંક)સંખ્યા_of_zeros_to_pad;
		જો (zn > 0)
		{
		    જો (str_l < str_m)
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
		પૂર્ણાંક sn = (પૂર્ણાંક)(str_arg_l - zero_padding_insertion_ind);

		જો (sn > 0)
		{
		    જો (str_l < str_m)
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
	    જો (justify_ડાબું)
	    {
		// right blank padding to the field width
		પૂર્ણાંક pn = (પૂર્ણાંક)(min_field_width
				      - (str_arg_l + સંખ્યા_of_zeros_to_pad));

		જો (pn > 0)
		{
		    જો (str_l < str_m)
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

    જો (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    જો (tvs != ખાલી && tvs[અંક_posarg != 0 ? અંક_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((અક્ષર*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    પાછા (પૂર્ણાંક)str_l;
}

#endif // PROTO
