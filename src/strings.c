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
#अन्तर्भाव "vim.h"

/*
 * Copy "string" into newly allocated memory.
 */
    char_u *
vim_strsave(char_u *string)
{
    char_u	*p;
    माप_प्रकार	len;

    len = STRLEN(string) + 1;
    p = alloc(len);
    यदि (p != रिक्त)
	mch_memmove(p, string, len);
    फर्काऊ p;
}

/*
 * Copy up to "len" bytes of "string" into newly allocated memory and
 * terminate with a NUL.
 * The allocated memory always has size "len + 1", also when "string" is
 * shorter.
 */
    char_u *
vim_strnsave(char_u *string, माप_प्रकार len)
{
    char_u	*p;

    p = alloc(len + 1);
    यदि (p == रिक्त)
	फर्काऊ रिक्त;

    STRNCPY(p, string, len);
    p[len] = NUL;
    फर्काऊ p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    फर्काऊ vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    पूर्णाङ्क		cc,
    पूर्णाङ्क		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    अचिन्हित	length;
    पूर्णाङ्क		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    लागि (p = string; *p; p++)
    {
	यदि (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    जारी;
	}
	यदि (vim_strchr(esc_chars, *p) != रिक्त || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    यदि (escaped_string == रिक्त)
	फर्काऊ रिक्त;
    p2 = escaped_string;
    लागि (p = string; *p; p++)
    {
	यदि (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (माप_प्रकार)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    जारी;
	}
	यदि (vim_strchr(esc_chars, *p) != रिक्त || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    फर्काऊ escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    पूर्णाङ्क
csh_like_shell(शून्य)
{
    फर्काऊ (म_माला((वर्ण *)gettail(p_sh), "csh") != रिक्त);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    स्थिर पूर्णाङ्क
fish_like_shell(शून्य)
{
    फर्काऊ (म_माला((वर्ण *)gettail(p_sh), "fish") != रिक्त);
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
vim_strsave_shellescape(char_u *string, पूर्णाङ्क do_special, पूर्णाङ्क do_newline)
{
    अचिन्हित	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    माप_प्रकार	l;
    पूर्णाङ्क		csh_like;
    पूर्णाङ्क		fish_like;
    char_u	*shname;
    पूर्णाङ्क		powershell;
#ifdef MSWIN
    पूर्णाङ्क		double_quotes;
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
    powershell = म_माला((वर्ण *)shname, "pwsh") != रिक्त;
#ifdef MSWIN
    powershell = powershell || म_माला((वर्ण *)shname, "powershell") != रिक्त;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (अचिन्हित)STRLEN(string) + 3;  // two quotes and a trailing NUL
    लागि (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	यदि (double_quotes)
	{
	    यदि (*p == '"')
		++length;		// " -> ""
	}
	अन्यथा
#endif
	यदि (*p == '\'')
	{
	    यदि (powershell)
		length += 2;		// ' => ''
	    अन्यथा
		length += 3;		// ' => '\''
	}
	यदि ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    यदि (csh_like && do_special)
		++length;		// insert backslash
	}
	यदि (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	यदि (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    यदि (escaped_string != रिक्त)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	यदि (double_quotes)
	    *d++ = '"';
	अन्यथा
#endif
	    *d++ = '\'';

	लागि (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    यदि (double_quotes)
	    {
		यदि (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    जारी;
		}
	    }
	    अन्यथा
#endif
	    यदि (*p == '\'')
	    {
		यदि (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		अन्यथा
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		जारी;
	    }
	    यदि ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		यदि (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		जारी;
	    }
	    यदि (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		स_नकल(d, p, l);	// copy the var
		d += l;
		p += l;
		जारी;
	    }
	    यदि (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		जारी;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	यदि (double_quotes)
	    *d++ = '"';
	अन्यथा
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    फर्काऊ escaped_string;
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
    फर्काऊ p1;
}

/*
 * Like vim_strnsave(), but make all characters uppercase.
 * This uses ASCII lower-to-upper case translation, language independent.
 */
    char_u *
vim_strnsave_up(char_u *string, माप_प्रकार len)
{
    char_u *p1;

    p1 = vim_strnsave(string, len);
    vim_strup(p1);
    फर्काऊ p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    शून्य
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    पूर्णाङ्क	    c;

    यदि (p == रिक्त)
	फर्काऊ;

    p2 = p;
    जबसम्म ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#यदि defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    स्थिर char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    यदि (res != रिक्त)
	जबसम्म (*p != NUL)
	{
	    पूर्णाङ्क		l;

	    यदि (enc_utf8)
	    {
		पूर्णाङ्क	c, uc;
		पूर्णाङ्क	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		यदि (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		यदि (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    यदि (s == रिक्त)
		    {
			vim_free(res);
			फर्काऊ रिक्त;
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
	    अन्यथा यदि (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    अन्यथा
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    फर्काऊ res;
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

    यदि (res != रिक्त)
	जबसम्म (*p != NUL)
	{
	    पूर्णाङ्क		l;

	    यदि (enc_utf8)
	    {
		पूर्णाङ्क	c, lc;
		पूर्णाङ्क	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		यदि (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		यदि (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    यदि (s == रिक्त)
		    {
			vim_free(res);
			फर्काऊ रिक्त;
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
	    अन्यथा यदि (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    अन्यथा
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    फर्काऊ res;
}
#endif

/*
 * delete spaces at the end of a string
 */
    शून्य
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    जबसम्म (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    शून्य
vim_strncpy(char_u *to, char_u *from, माप_प्रकार len)
{
    STRNCPY(to, from, len);
    to[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    शून्य
vim_strcat(char_u *to, char_u *from, माप_प्रकार tosize)
{
    माप_प्रकार tolen = STRLEN(to);
    माप_प्रकार fromlen = STRLEN(from);

    यदि (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    अन्यथा
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    माप_प्रकार
vim_strlen_maxlen(वर्ण *s, माप_प्रकार maxlen)
{
    माप_प्रकार i;
    लागि (i = 0; i < maxlen; ++i)
	यदि (s[i] == NUL)
	    रोक;
    फर्काऊ i;
}

#यदि !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    पूर्णाङ्क
vim_stricmp(वर्ण *s1, वर्ण *s2)
{
    पूर्णाङ्क		i;

    लागि (;;)
    {
	i = (पूर्णाङ्क)TOLOWER_LOC(*s1) - (पूर्णाङ्क)TOLOWER_LOC(*s2);
	यदि (i != 0)
	    फर्काऊ i;			    // this character different
	यदि (*s1 == NUL)
	    रोक;			    // strings match until NUL
	++s1;
	++s2;
    }
    फर्काऊ 0;				    // strings match
}
#endif

#यदि !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    पूर्णाङ्क
vim_strnicmp(वर्ण *s1, वर्ण *s2, माप_प्रकार len)
{
    पूर्णाङ्क		i;

    जबसम्म (len > 0)
    {
	i = (पूर्णाङ्क)TOLOWER_LOC(*s1) - (पूर्णाङ्क)TOLOWER_LOC(*s2);
	यदि (i != 0)
	    फर्काऊ i;			    // this character different
	यदि (*s1 == NUL)
	    रोक;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    फर्काऊ 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    पूर्णाङ्क
vim_strnicmp_asc(वर्ण *s1, वर्ण *s2, माप_प्रकार len)
{
    पूर्णाङ्क                i = 0;

    जबसम्म (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	यदि (i != 0)
	    रोक;			// this character is different
	यदि (*s1 == NUL)
	    रोक;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    फर्काऊ i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, पूर्णाङ्क c)
{
    char_u	*p;
    पूर्णाङ्क		b;

    p = string;
    यदि (enc_utf8 && c > 0 && c < 0x80)
	फर्काऊ vim_strbyte(string, c);
    यदि (enc_utf8 && c >= 0x80)
    {
	जबसम्म (*p != NUL)
	{
	    पूर्णाङ्क l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    यदि (utf_ptr2char(p) == c && l > 1)
		फर्काऊ p;
	    p += l;
	}
	फर्काऊ रिक्त;
    }
    यदि (enc_dbcs != 0 && c > 255)
    {
	पूर्णाङ्क	n2 = c & 0xff;

	c = ((अचिन्हित)c >> 8) & 0xff;
	जबसम्म ((b = *p) != NUL)
	{
	    यदि (b == c && p[1] == n2)
		फर्काऊ p;
	    p += (*mb_ptr2len)(p);
	}
	फर्काऊ रिक्त;
    }
    यदि (has_mbyte)
    {
	जबसम्म ((b = *p) != NUL)
	{
	    यदि (b == c)
		फर्काऊ p;
	    p += (*mb_ptr2len)(p);
	}
	फर्काऊ रिक्त;
    }
    जबसम्म ((b = *p) != NUL)
    {
	यदि (b == c)
	    फर्काऊ p;
	++p;
    }
    फर्काऊ रिक्त;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, पूर्णाङ्क c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    यदि (c <= 0 || c > 255)
	फर्काऊ रिक्त;
    फर्काऊ (char_u *)म_अक्षर((वर्ण *)string, c);
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
vim_strrchr(char_u *string, पूर्णाङ्क c)
{
    char_u	*retval = रिक्त;
    char_u	*p = string;

    जबसम्म (*p)
    {
	यदि (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    फर्काऊ retval;
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
    जबसम्म (*s)
    {
	यदि (vim_strchr(charset, *s) != रिक्त)
	    फर्काऊ s;
	MB_PTR_ADV(s);
    }
    फर्काऊ रिक्त;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
स्थिर पूर्णाङ्क sort_compare(अचर शून्य *s1, अचर शून्य *s2);

    स्थिर पूर्णाङ्क
sort_compare(अचर शून्य *s1, अचर शून्य *s2)
{
    फर्काऊ STRCMP(*(वर्ण **)s1, *(वर्ण **)s2);
}

    शून्य
sort_strings(
    char_u	**files,
    पूर्णाङ्क		गन्ती)
{
    क्विक((शून्य *)files, (माप_प्रकार)गन्ती, परिमाण(char_u *), sort_compare);
}

#यदि defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    पूर्णाङ्क
has_non_ascii(char_u *s)
{
    char_u	*p;

    यदि (s != रिक्त)
	लागि (p = s; *p != NUL; ++p)
	    यदि (*p >= 128)
		फर्काऊ TRUE;
    फर्काऊ FALSE;
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
    माप_प्रकार  l = str1 == रिक्त ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == रिक्त ? 0 : STRLEN(str2)) + 1L);
    यदि (dest == रिक्त)
	फर्काऊ रिक्त;
    यदि (str1 == रिक्त)
	*dest = NUL;
    अन्यथा
	STRCPY(dest, str1);
    यदि (str2 != रिक्त)
	STRCPY(dest + l, str2);
    फर्काऊ dest;
}

#यदि defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    माप_प्रकार len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    यदि (rev == रिक्त)
	फर्काऊ रिक्त;

    लागि (माप_प्रकार s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	यदि (has_mbyte)
	{
	    पूर्णाङ्क mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	अन्यथा
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    फर्काऊ rev;
}
#endif

#यदि defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, पूर्णाङ्क function)
{
    अचिन्हित	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    यदि (str != रिक्त)
    {
	len += (अचिन्हित)STRLEN(str);
	लागि (p = str; *p != NUL; MB_PTR_ADV(p))
	    यदि (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    यदि (r == रिक्त)
	फर्काऊ रिक्त;

    यदि (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    अन्यथा
	*r++ = '\'';
    यदि (str != रिक्त)
	लागि (p = str; *p != NUL; )
	{
	    यदि (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    यदि (function)
	*r++ = ')';
    *r++ = NUL;
    फर्काऊ s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    दीर्घ
string_गन्ती(char_u *haystack, char_u *needle, पूर्णाङ्क ic)
{
    दीर्घ	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    यदि (p == रिक्त || needle == रिक्त || *needle == NUL)
	फर्काऊ 0;

    माप_प्रकार  needlelen = STRLEN(needle);
    यदि (ic)
    {
	जबसम्म (*p != NUL)
	{
	    यदि (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    अन्यथा
		MB_PTR_ADV(p);
	}
    }
    अन्यथा
	जबसम्म ((next = (char_u *)म_माला((वर्ण *)p, (वर्ण *)needle)) != रिक्त)
	{
	    ++n;
	    p = next + needlelen;
	}

    फर्काऊ n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    स्थिर पूर्णाङ्क
copy_first_char_to_tv(char_u *आगत, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    पूर्णाङ्क		len;

    यदि (आगत == रिक्त || output == रिक्त)
	फर्काऊ -1;

    len = has_mbyte ? mb_ptr2len(आगत) : 1;
    STRNCPY(buf, आगत, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    फर्काऊ output->vval.v_string == रिक्त ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    शून्य
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*अभिव्यक्ति,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    पूर्णाङ्क		len = 0;
    पूर्णाङ्क		idx = 0;
    पूर्णाङ्क		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_साँचो, VAR_सङ्ख्या);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_अभिव्यक्ति_get_funccal(अभिव्यक्ति, &newtv);

    ga_init2(&ga, परिमाण(वर्ण), 80);
    लागि (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	यदि (len < 0)
	    रोक;

	set_vim_var_nr(VV_साँचो, idx);
	यदि (filter_map_one(&tv, अभिव्यक्ति, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    रोक;
	}
	यदि (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    यदि (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		रोक;
	    }
	    अन्यथा
		ga_concat(&ga, newtv.vval.v_string);
	}
	अन्यथा यदि (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    यदि (fc != रिक्त)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    शून्य
string_reduce(
	typval_T	*argvars,
	typval_T	*अभिव्यक्ति,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    पूर्णाङ्क		len;
    typval_T	argv[3];
    पूर्णाङ्क		r;
    पूर्णाङ्क		called_emsg_start = called_emsg;
    funccall_T	*fc;

    यदि (argvars[2].v_type == VAR_UNKNOWN)
    {
	यदि (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_मान), "String");
	    फर्काऊ;
	}
	len = copy_first_char_to_tv(p, rettv);
	यदि (len < 0)
	    फर्काऊ;
	p += len;
    }
    अन्यथा यदि (check_for_string_arg(argvars, 2) == FAIL)
	फर्काऊ;
    अन्यथा
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_अभिव्यक्ति_get_funccal(अभिव्यक्ति, rettv);

    लागि ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	यदि (len < 0)
	{
	    *rettv = argv[0];
	    रोक;
	}

	r = eval_अभिव्यक्ति_typval(अभिव्यक्ति, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	यदि (r == FAIL || called_emsg != called_emsg_start)
	    रोक;
    }

    यदि (fc != रिक्त)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    स्थिर शून्य
byteidx_common(typval_T *argvars, typval_T *rettv, पूर्णाङ्क comp)
{
    rettv->vval.v_सङ्ख्या = -1;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	फर्काऊ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_सङ्ख्या_chk(&argvars[1], रिक्त);
    यदि (str == रिक्त || idx < 0)
	फर्काऊ;

    varnumber_T	utf16idx = FALSE;
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	पूर्णाङ्क error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	यदि (error)
	    फर्काऊ;
	यदि (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_सङ्ख्या_as_bool_nr), utf16idx);
	    फर्काऊ;
	}
    }

    पूर्णाङ्क (*ptr2len)(char_u *);
    यदि (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    अन्यथा
	ptr2len = mb_ptr2len;

    char_u *t = str;
    लागि ( ; idx > 0; idx--)
    {
	यदि (*t == NUL)		// EOL reached
	    फर्काऊ;
	यदि (utf16idx)
	{
	    पूर्णाङ्क clen = ptr2len(t);
	    पूर्णाङ्क c = (clen > 1) ? utf_ptr2char(t) : *t;
	    यदि (c > 0xFFFF)
		idx--;
	    यदि (idx > 0)
		t += clen;
	}
	अन्यथा यदि (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_सङ्ख्या = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    शून्य
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    शून्य
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    शून्य
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = -1;

    यदि (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	फर्काऊ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_सङ्ख्या_chk(&argvars[1], रिक्त);
    यदि (str == रिक्त || idx < 0)
	फर्काऊ;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	यदि (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    पूर्णाङ्क (*ptr2len)(char_u *);
    यदि (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    अन्यथा
	ptr2len = mb_ptr2len;

    char_u	*p;
    पूर्णाङ्क		len;
    लागि (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	यदि (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    यदि (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_सङ्ख्या = len;
	    फर्काऊ;
	}
	यदि (utf16idx)
	{
	    idx--;
	    पूर्णाङ्क clen = ptr2len(p);
	    पूर्णाङ्क c = (clen > 1) ? utf_ptr2char(p) : *p;
	    यदि (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_सङ्ख्या = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    स्थिर पूर्णाङ्क
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    यदि (convert_setup(&vimconv, from, to) == FAIL)
	फर्काऊ FAIL;
    vimconv.vc_fail = TRUE;
    यदि (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	यदि (ret->string == रिक्त)
	    ret->length = 0;
	अन्यथा
	    ret->length = str->length;
    }
    अन्यथा
    {
	पूर्णाङ्क len = (पूर्णाङ्क)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, रिक्त, रिक्त);

    फर्काऊ (ret->string == रिक्त) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    स्थिर शून्य
blob_from_string(char_u *str, blob_T *blob)
{
    पूर्णाङ्क	    len = (पूर्णाङ्क)STRLEN(str);
    char_u  *dest;

    यदि (len == 0)
	फर्काऊ;
    यदि (ga_grow(&blob->bv_ga, len) == FAIL)
	फर्काऊ;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (माप_प्रकार)len);
    // Translate newlines in the string to NUL characters
    लागि (पूर्णाङ्क i = 0; i < len; ++i)
	यदि (dest[i] == NL)
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
    स्थिर पूर्णाङ्क
string_from_blob(blob_T *blob, दीर्घ *start_idx, string_T *ret)
{
    दीर्घ	blen = blob_len(blob);
    दीर्घ	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    दीर्घ	पङ्क्ति_len;

    यदि (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	फर्काऊ (ret->string == रिक्त) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)स_प्रथम(src, NL, (माप_प्रकार)(blen - start));
    पङ्क्ति_len = (nl == रिक्त) ? (blen - start) : (दीर्घ)(nl - src);

    ret->string = alloc(पङ्क्ति_len + 1);
    यदि (ret->string == रिक्त)
    {
	ret->length = 0;
	फर्काऊ FAIL;
    }
    यदि (पङ्क्ति_len > 0)
	mch_memmove(ret->string, src, (माप_प्रकार)पङ्क्ति_len);
    ret->string[पङ्क्ति_len] = NUL;
    ret->length = (माप_प्रकार)पङ्क्ति_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    लागि (दीर्घ i = 0; i < पङ्क्ति_len; i++)
	यदि (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + पङ्क्ति_len + (nl != रिक्त ? 1 : 0);
    फर्काऊ OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    स्थिर char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    यदि (from_encoding_raw == रिक्त)
	फर्काऊ रिक्त;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    जबसम्म (*s != NUL)
    {
	यदि (*s == '_')
	    *pe++ = '-';
	अन्यथा
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    यदि ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    फर्काऊ from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    स्थिर शून्य
append_converted_string_to_list(
	string_T *converted,
	पूर्णाङ्क validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    यदि (converted->string != रिक्त)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	जबसम्म (p < end)
	{
	    string_T	पङ्क्ति;
	    char_u	*पङ्क्ति_start = p;

	    जबसम्म (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    पङ्क्ति.length = (माप_प्रकार)(p - पङ्क्ति_start);
	    पङ्क्ति.string = vim_strnsave(पङ्क्ति_start, पङ्क्ति.length);
	    यदि (पङ्क्ति.string != रिक्त)
	    {
		यदि (validate_utf8 && !utf_valid_string(पङ्क्ति.string, रिक्त))
		{
		    vim_free(पङ्क्ति.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    फर्काऊ; // Stop processing
		}
		यदि (list_append_string(list, पङ्क्ति.string, (पूर्णाङ्क)पङ्क्ति.length) == FAIL)
		{
		    vim_free(पङ्क्ति.string);
		    vim_free(converted->string);
		    फर्काऊ; // Stop processing on append failure
		}
		vim_free(पङ्क्ति.string);
	    }
	    अन्यथा
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		फर्काऊ;
	    }

	    यदि (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    अन्यथा
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    स्थिर पूर्णाङ्क
append_validated_पङ्क्ति_to_list(string_T *पङ्क्ति, पूर्णाङ्क validate_utf8, list_T *list)
{
    यदि (validate_utf8 && !utf_valid_string(पङ्क्ति->string, रिक्त))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(पङ्क्ति->string);
	फर्काऊ FAIL;
    }

    पूर्णाङ्क ret = list_append_string(list, पङ्क्ति->string, (पूर्णाङ्क)पङ्क्ति->length);
    vim_free(पङ्क्ति->string);
    फर्काऊ ret;
}

    शून्य
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    पूर्णाङ्क		blen;
    दीर्घ	idx;
    पूर्णाङ्क		validate_utf8 = FALSE;

    यदि (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	फर्काऊ;

    यदि (rettv_list_alloc(rettv) == FAIL)
	फर्काऊ;

    blob = argvars->vval.v_blob;
    यदि (blob == रिक्त)
	फर्काऊ;
    blen = blob_len(blob);

    char_u	*from_encoding = रिक्त;
    char_u	*from_encoding_raw = रिक्त;  // Encoding name with endianness preserved for iconv
    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	यदि (d != रिक्त)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    यदि (enc != रिक्त)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		यदि (from_encoding_raw == रिक्त)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    फर्काऊ;
		}
	    }
	}
    }

    यदि (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    यदि (from_encoding != रिक्त && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    पूर्णाङ्क from_prop = 0;
    यदि (from_encoding != रिक्त)
	from_prop = enc_canon_props(from_encoding);
    यदि (from_encoding != रिक्त && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	पूर्णाङ्क nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	यदि (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    यदि (blen > 0)
		mch_memmove(blob_ga.ga_data, blob->bv_ga.ga_data, (माप_प्रकार)blen);
	    // NUL terminator (2 bytes for UTF-16/UCS-2, 4 bytes for UTF-32/UCS-4)
	    vim_memset((char_u *)blob_ga.ga_data + blen, 0, (माप_प्रकार)nul_size);
	    blob_ga.ga_len = blen + nul_size;
	}

	// Convert the entire blob at once
	vimconv_T vimconv;
	vimconv.vc_type = CONV_NONE;
	// Use raw encoding name for iconv to preserve endianness (utf-16be vs utf-16)
	// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
	यदि (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    गच्छ done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	पूर्णाङ्क len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, रिक्त);
	converted.length = len;
	convert_setup(&vimconv, रिक्त, रिक्त);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    अन्यथा
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	जबसम्म (idx < blen)
	{
	    string_T	str;

	    यदि (string_from_blob(blob, &idx, &str) != OK)
		रोक;

	    यदि (from_encoding != रिक्त)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		पूर्णाङ्क	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		यदि (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    गच्छ done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    यदि (append_validated_पङ्क्ति_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		गच्छ done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    यदि (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

done:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    शून्य
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    यदि (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	फर्काऊ;

    यदि (rettv_blob_alloc(rettv) == FAIL)
	फर्काऊ;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    यदि (list == रिक्त)
	फर्काऊ;

    char_u	*to_encoding = रिक्त;
    char_u	*to_encoding_raw = रिक्त;  // Encoding name with endianness preserved for iconv
    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	यदि (d != रिक्त)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    यदि (enc != रिक्त)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		यदि (to_encoding_raw == रिक्त)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    फर्काऊ;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    पूर्णाङ्क to_prop = 0;
    यदि (to_encoding != रिक्त)
	to_prop = enc_canon_props(to_encoding);
    यदि (to_encoding != रिक्त && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_वस्तुहरू(list, li)
	{
	    char_u *s;

	    यदि (li->li_tv.v_type != VAR_STRING)
		जारी;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    यदि (li != list->lv_first)
		ga_append(&str_ga, NL);
	    यदि (s != रिक्त && *s != NUL)
	    {
		पूर्णाङ्क slen = (पूर्णाङ्क)STRLEN(s);

		यदि (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    गच्छ done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(माप_प्रकार)slen);
		str_ga.ga_len += slen;
	    }
	}

	यदि (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    यदि (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		गच्छ done;
	    }
	    vimconv.vc_fail = TRUE;

	    पूर्णाङ्क		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, रिक्त);
	    convert_setup(&vimconv, रिक्त, रिक्त);
	    ga_clear(&str_ga);

	    यदि (converted == रिक्त)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		गच्छ done;
	    }
	    यदि (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (माप_प्रकार)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	अन्यथा
	    ga_clear(&str_ga);
    }
    अन्यथा
    {
	FOR_ALL_LIST_वस्तुहरू(list, li)
	{
	    यदि (li->li_tv.v_type != VAR_STRING)
		जारी;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    यदि (str.string == रिक्त)
		STR_LITERAL_SET(str, "");
	    अन्यथा
		str.length = STRLEN(str.string);

	    यदि (to_encoding != रिक्त)
	    {
		पूर्णाङ्क	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		यदि (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    गच्छ done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    यदि (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    यदि (to_encoding != रिक्त)
		vim_free(str.string);
	}
    }

done:
    यदि (to_encoding != रिक्त)
	vim_free(to_encoding);
    यदि (to_encoding_raw != रिक्त)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    शून्य
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    पूर्णाङ्क		utf8 = FALSE;

    यदि (rettv_list_alloc(rettv) == FAIL)
	फर्काऊ;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	फर्काऊ;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (पूर्णाङ्क)tv_get_bool_chk(&argvars[1], रिक्त);

    p = tv_get_string(&argvars[0]);

    यदि (has_mbyte || utf8)
    {
	पूर्णाङ्क (*ptr2len)(char_u *);
	पूर्णाङ्क (*ptr2char)(char_u *);

	यदि (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	अन्यथा
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	लागि ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_सङ्ख्या(rettv->vval.v_list, (*ptr2char)(p));
    }
    अन्यथा
	लागि ( ; *p != NUL; ++p)
	    list_append_सङ्ख्या(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    शून्य
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		base = 10;
    char_u	*p;
    varnumber_T	n;
    पूर्णाङ्क		what = 0;
    पूर्णाङ्क		isneg;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	फर्काऊ;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[1]);
	यदि (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    फर्काऊ;
	}
	यदि (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    यदि (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    विकल्प (base)
    {
	पक्ष 2: what |= STR2NR_BIN + STR2NR_FORCE; रोक;
	पक्ष 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; रोक;
	पक्ष 16: what |= STR2NR_HEX + STR2NR_FORCE; रोक;
    }
    vim_str2nr(p, रिक्त, रिक्त, what, &n, रिक्त, 0, FALSE, रिक्त);
    // Text after the number is silently ignored.
    यदि (isneg)
	rettv->vval.v_सङ्ख्या = -n;
    अन्यथा
	rettv->vval.v_सङ्ख्या = n;

}

/*
 * "strgetchar()" function
 */
    शून्य
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    पूर्णाङ्क		len;
    पूर्णाङ्क		error = FALSE;
    पूर्णाङ्क		charidx;
    पूर्णाङ्क		byteidx = 0;

    rettv->vval.v_सङ्ख्या = -1;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL))
	फर्काऊ;

    str = tv_get_string_chk(&argvars[0]);
    यदि (str == रिक्त)
	फर्काऊ;
    len = (पूर्णाङ्क)STRLEN(str);
    charidx = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[1], &error);
    यदि (error)
	फर्काऊ;

    जबसम्म (charidx >= 0 && byteidx < len)
    {
	यदि (charidx == 0)
	{
	    rettv->vval.v_सङ्ख्या = mb_ptr2char(str + byteidx);
	    रोक;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    शून्य
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    पूर्णाङ्क		start_idx;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 2) == FAIL))
	फर्काऊ;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_सङ्ख्या = -1;
    यदि (needle == रिक्त || haystack == रिक्त)
	फर्काऊ;		// type error; errmsg already given

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	पूर्णाङ्क	    error = FALSE;

	start_idx = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[2], &error);
	यदि (error || start_idx >= (पूर्णाङ्क)STRLEN(haystack))
	    फर्काऊ;
	यदि (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)म_माला((वर्ण *)haystack, (वर्ण *)needle);
    यदि (pos != रिक्त)
	rettv->vval.v_सङ्ख्या = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    शून्य
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    यदि (rettv->vval.v_string != रिक्त && tofree == रिक्त)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    शून्य
f_strlen(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && check_for_string_or_सङ्ख्या_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->vval.v_सङ्ख्या = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    स्थिर शून्य
strchar_common(typval_T *argvars, typval_T *rettv, पूर्णाङ्क skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    पूर्णाङ्क			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    जबसम्म (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_सङ्ख्या = len;
}

/*
 * "strcharlen()" function
 */
    शून्य
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && check_for_string_or_सङ्ख्या_arg(argvars, 0) == FAIL)
	फर्काऊ;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    शून्य
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	फर्काऊ;

    यदि (argvars[1].v_type != VAR_UNKNOWN)
    {
	पूर्णाङ्क error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	यदि (error)
	    फर्काऊ;
	यदि (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_सङ्ख्या_as_bool_nr), skipcc);
	    फर्काऊ;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    शून्य
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = -1;

    यदि (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	फर्काऊ;

    varnumber_T countcc = FALSE;
    यदि (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    पूर्णाङ्क			(*func_mb_ptr2char_adv)(char_u **pp);
    पूर्णाङ्क			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    जबसम्म (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	यदि (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_सङ्ख्या = len;
}

/*
 * "strdisplaywidth()" function
 */
    शून्य
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    पूर्णाङ्क		col = 0;

    rettv->vval.v_सङ्ख्या = -1;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	फर्काऊ;

    s = tv_get_string(&argvars[0]);
    यदि (argvars[1].v_type != VAR_UNKNOWN)
	col = (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[1]);

    rettv->vval.v_सङ्ख्या = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    शून्य
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_सङ्ख्या = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    शून्य
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    पूर्णाङ्क		nchar;
    पूर्णाङ्क		nbyte = 0;
    पूर्णाङ्क		charlen;
    पूर्णाङ्क		skipcc = FALSE;
    पूर्णाङ्क		len = 0;
    पूर्णाङ्क		slen;
    पूर्णाङ्क		error = FALSE;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	फर्काऊ;

    p = tv_get_string(&argvars[0]);
    slen = (पूर्णाङ्क)STRLEN(p);

    nchar = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[1], &error);
    यदि (!error)
    {
	यदि (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    यदि (error)
		फर्काऊ;
	    यदि (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_सङ्ख्या_as_bool_nr), skipcc);
		फर्काऊ;
	    }
	}

	यदि (nchar > 0)
	    जबसम्म (nchar > 0 && nbyte < slen)
	    {
		यदि (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		अन्यथा
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	अन्यथा
	    nbyte = nchar;
	यदि (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[2]);
	    जबसम्म (charlen > 0 && nbyte + len < slen)
	    {
		पूर्णाङ्क off = nbyte + len;

		यदि (off < 0)
		    len += 1;
		अन्यथा
		{
		    यदि (skipcc)
			len += mb_ptr2len(p + off);
		    अन्यथा
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	अन्यथा
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    यदि (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    अन्यथा यदि (nbyte > slen)
	nbyte = slen;
    यदि (len < 0)
	len = 0;
    अन्यथा यदि (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    शून्य
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    पूर्णाङ्क		n;
    पूर्णाङ्क		len;
    पूर्णाङ्क		slen;
    पूर्णाङ्क		error = FALSE;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	फर्काऊ;

    p = tv_get_string(&argvars[0]);
    slen = (पूर्णाङ्क)STRLEN(p);

    n = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[1], &error);
    यदि (error)
	len = 0;
    अन्यथा यदि (argvars[2].v_type != VAR_UNKNOWN)
	len = (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[2]);
    अन्यथा
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    यदि (n < 0)
    {
	len += n;
	n = 0;
    }
    अन्यथा यदि (n > slen)
	n = slen;
    यदि (len < 0)
	len = 0;
    अन्यथा यदि (n + len > slen)
	len = slen - n;

    यदि (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	पूर्णाङ्क off;

	// length in characters
	लागि (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    शून्य
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = रिक्त;
    पूर्णाङ्क		haystack_len, end_idx;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 2) == FAIL))
	फर्काऊ;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_सङ्ख्या = -1;
    यदि (needle == रिक्त || haystack == रिक्त)
	फर्काऊ;		// type error; errmsg already given

    haystack_len = (पूर्णाङ्क)STRLEN(haystack);
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[2], रिक्त);
	यदि (end_idx < 0)
	    फर्काऊ;	// can never find a match
    }
    अन्यथा
	end_idx = haystack_len;

    यदि (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    अन्यथा
    {
	लागि (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)म_माला((वर्ण *)rest, (वर्ण *)needle);
	    यदि (rest == रिक्त || rest > haystack + end_idx)
		रोक;
	    lastmatch = rest;
	}
    }

    यदि (lastmatch == रिक्त)
	rettv->vval.v_सङ्ख्या = -1;
    अन्यथा
	rettv->vval.v_सङ्ख्या = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    शून्य
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    शून्य
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = -1;

    यदि (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    फर्काऊ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_सङ्ख्या_chk(&argvars[1], रिक्त);
    यदि (str == रिक्त || idx < 0)
	फर्काऊ;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	यदि (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    पूर्णाङ्क (*ptr2len)(char_u *);
    यदि (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    अन्यथा
	ptr2len = mb_ptr2len;

    char_u	*p;
    पूर्णाङ्क		len;
    पूर्णाङ्क		utf16idx = 0;
    लागि (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	यदि (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    यदि (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_सङ्ख्या = len;
	    फर्काऊ;
	}
	utf16idx = len;
	पूर्णाङ्क clen = ptr2len(p);
	पूर्णाङ्क c = (clen > 1) ? utf_ptr2char(p) : *p;
	यदि (c > 0xFFFF)
	    len++;
	p += clen;
	यदि (charidx)
	    idx--;
    }

    rettv->vval.v_सङ्ख्या = utf16idx;
}

/*
 * "tolower(string)" function
 */
    शून्य
f_tolower(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    शून्य
f_toupper(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    शून्य
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*in_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    पूर्णाङ्क		inlen;
    पूर्णाङ्क		fromlen;
    पूर्णाङ्क		tolen;
    पूर्णाङ्क		idx;
    char_u	*cpstr;
    पूर्णाङ्क		cplen;
    पूर्णाङ्क		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	फर्काऊ;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
    यदि (fromstr == रिक्त || tostr == रिक्त)
	    फर्काऊ;		// type error; errmsg already given
    ga_init2(&ga, परिमाण(वर्ण), 80);

    यदि (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	यदि (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    फर्काऊ;
	}

    // fromstr and tostr have to contain the same number of chars
    जबसम्म (*in_str != NUL)
    {
	यदि (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    लागि (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		यदि (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    लागि (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			यदि (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    रोक;
			}
		    }
		    यदि (*p == NUL)	// tostr is shorter than fromstr
			गच्छ error;
		    रोक;
		}
		++idx;
	    }

	    यदि (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		लागि (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		यदि (idx != 0)
		    गच्छ error;
	    }

	    (शून्य)ga_grow(&ga, cplen);
	    mch_memmove((वर्ण *)ga.ga_data + ga.ga_len, cpstr, (माप_प्रकार)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	अन्यथा
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    यदि (p != रिक्त)
		ga_append(&ga, tostr[p - fromstr]);
	    अन्यथा
		ga_append(&ga, *in_str);
	    ++in_str;
	}
    }

    // add a terminating NUL
    (शून्य)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_data;
}

/*
 * "trim({expr})" function
 */
    शून्य
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*head;
    char_u	*mask = रिक्त;
    char_u	*tail;
    char_u	*prev;
    char_u	*p;
    पूर्णाङ्क		c1;
    पूर्णाङ्क		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_सङ्ख्या_arg(argvars, 2) == FAIL)))
	फर्काऊ;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    यदि (head == रिक्त)
	फर्काऊ;

    यदि (check_for_opt_string_arg(argvars, 1) == FAIL)
	फर्काऊ;

    यदि (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	यदि (*mask == NUL)
	    mask = रिक्त;

	यदि (argvars[2].v_type != VAR_UNKNOWN)
	{
	    पूर्णाङ्क	error = 0;

	    // leading or trailing characters to trim
	    dir = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[2], &error);
	    यदि (error)
		फर्काऊ;
	    यदि (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		फर्काऊ;
	    }
	}
    }

    यदि (dir == 0 || dir == 1)
    {
	// Trim leading characters
	जबसम्म (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    यदि (mask == रिक्त)
	    {
		यदि (c1 > ' ' && c1 != 0xa0)
		    रोक;
	    }
	    अन्यथा
	    {
		लागि (p = mask; *p != NUL; MB_PTR_ADV(p))
		    यदि (c1 == PTR2CHAR(p))
			रोक;
		यदि (*p == NUL)
		    रोक;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    यदि (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	लागि (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    यदि (mask == रिक्त)
	    {
		यदि (c1 > ' ' && c1 != 0xa0)
		    रोक;
	    }
	    अन्यथा
	    {
		लागि (p = mask; *p != NUL; MB_PTR_ADV(p))
		    यदि (c1 == PTR2CHAR(p))
			रोक;
		यदि (*p == NUL)
		    रोक;
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
    स्थिर char_u *
uri_decode(char_u *str)
{
    यदि (str == रिक्त)
	फर्काऊ रिक्त;

    माप_प्रकार len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    यदि (!decoded)
	फर्काऊ रिक्त;

    char_u	*p = decoded;
    माप_प्रकार	i = 0;

    जबसम्म (i < len)
    {
	यदि (str[i] == '%')
	{
	    यदि (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		यदि (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    अन्यथा
	    {
		पूर्णाङ्क val = hexhex2nr(&str[i + 1]);
		यदि (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		अन्यथा
		{
		    // invalid hex digits following "%"
		    लागि (पूर्णाङ्क j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	अन्यथा
	    *p++ = str[i++];
    }

    *p = NUL;

    फर्काऊ decoded;
}

/*
 * "uri_decode({str})" function
 */
    शून्य
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    यदि (check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

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
    स्थिर char_u *
uri_encode(char_u *str)
{
    यदि (str == रिक्त)
	फर्काऊ रिक्त;

    माप_प्रकार len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    यदि (encoded == रिक्त)
	फर्काऊ रिक्त;

    char_u *p = encoded;

    लागि (माप_प्रकार i = 0; i < len; ++i)
    {
	char_u c = str[i];
	यदि (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	अन्यथा
	{
	    प्र_लिखो((वर्ण *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    फर्काऊ encoded;
}

/*
 * "uri_encode({str})" function
 */
    शून्य
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    यदि (check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

स्थिर वर्ण *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    स्थिर varnumber_T
tv_nr(typval_T *tvs, पूर्णाङ्क *idxp)
{
    पूर्णाङ्क		idx = *idxp - 1;
    varnumber_T	n = 0;
    पूर्णाङ्क		err = FALSE;

    यदि (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    अन्यथा
    {
	++*idxp;
	n = tv_get_सङ्ख्या_chk(&tvs[idx], &err);
	यदि (err)
	    n = 0;
    }
    फर्काऊ n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    स्थिर वर्ण *
tv_str(typval_T *tvs, पूर्णाङ्क *idxp, char_u **tofree)
{
    पूर्णाङ्क		    idx = *idxp - 1;
    वर्ण	    *s = रिक्त;
    स्थिर char_u   numbuf[NUMBUFLEN];

    यदि (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    अन्यथा
    {
	++*idxp;
	यदि (tofree != रिक्त)
	    s = (वर्ण *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	अन्यथा
	    s = (वर्ण *)tv_get_string_chk(&tvs[idx]);
    }
    फर्काऊ s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    स्थिर द्विगुण
tv_float(typval_T *tvs, पूर्णाङ्क *idxp)
{
    पूर्णाङ्क		idx = *idxp - 1;
    द्विगुण	f = 0;

    यदि (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    अन्यथा
    {
	++*idxp;
	यदि (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	अन्यथा यदि (tvs[idx].v_type == VAR_सङ्ख्या)
	    f = (द्विगुण)tvs[idx].vval.v_सङ्ख्या;
	अन्यथा
	    emsg(_(e_expected_float_argument_for_printf));
    }
    फर्काऊ f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    स्थिर अचर वर्ण *
infinity_str(पूर्णाङ्क positive,
	     वर्ण fmt_spec,
	     पूर्णाङ्क force_sign,
	     पूर्णाङ्क space_for_positive)
{
    स्थिर अचर वर्ण *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    पूर्णाङ्क idx = positive * (1 + force_sign + force_sign * space_for_positive);

    यदि (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    फर्काऊ table[idx];
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
    पूर्णाङ्क
vim_snprintf_add(वर्ण *str, माप_प्रकार str_m, अचर वर्ण *fmt, ...)
{
    बहु_सूची	ap;
    पूर्णाङ्क		str_l;
    माप_प्रकार	len = STRLEN(str);
    माप_प्रकार	space;

    यदि (str_m <= len)
	space = 0;
    अन्यथा
	space = str_m - len;
    बहु_शुरू(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    बहु_पूर्ण(ap);
    फर्काऊ str_l;
}

    पूर्णाङ्क
vim_snprintf(वर्ण *str, माप_प्रकार str_m, अचर वर्ण *fmt, ...)
{
    बहु_सूची	ap;
    पूर्णाङ्क		str_l;

    बहु_शुरू(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    बहु_पूर्ण(ap);
    फर्काऊ str_l;
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
    माप_प्रकार
vim_snprintf_safelen(वर्ण *str, माप_प्रकार str_m, अचर वर्ण *fmt, ...)
{
    बहु_सूची ap;
    पूर्णाङ्क	    str_l;

    यदि (str_m == 0)
	फर्काऊ 0;

    बहु_शुरू(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, रिक्त);
    बहु_पूर्ण(ap);

    यदि (str_l < 0)
    {
	*str = NUL;
	फर्काऊ 0;
    }
    फर्काऊ ((माप_प्रकार)str_l >= str_m) ? str_m - 1 : (माप_प्रकार)str_l;
}

    पूर्णाङ्क
vim_vsnprintf(
    वर्ण	*str,
    माप_प्रकार	str_m,
    अचर वर्ण	*fmt,
    बहु_सूची	ap)
{
    फर्काऊ vim_vsnprintf_typval(str, str_m, fmt, ap, रिक्त);
}

गणना
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
    स्थिर पूर्णाङ्क
format_typeof(
    अचर वर्ण	*type)
{
    // allowed values: \0, h, l, L
    वर्ण    length_modifier = '\0';

    // current conversion specifier character
    वर्ण    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    यदि (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	यदि (length_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    length_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    विकल्प (fmt_spec)
    {
	पक्ष 'i': fmt_spec = 'd'; रोक;
	पक्ष '*': fmt_spec = 'd'; length_modifier = 'h'; रोक;
	पक्ष 'D': fmt_spec = 'd'; length_modifier = 'l'; रोक;
	पक्ष 'U': fmt_spec = 'u'; length_modifier = 'l'; रोक;
	पक्ष 'O': fmt_spec = 'o'; length_modifier = 'l'; रोक;
	स्वभाव: रोक;
    }

    // get parameter value, do initial processing
    विकल्प (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    पक्ष '%':
	फर्काऊ TYPE_PERCENT;

    पक्ष 'c':
	फर्काऊ TYPE_CHAR;

    पक्ष 's':
    पक्ष 'S':
	फर्काऊ TYPE_STRING;

    पक्ष 'd': पक्ष 'u':
    पक्ष 'b': पक्ष 'B':
    पक्ष 'o':
    पक्ष 'x': पक्ष 'X':
    पक्ष 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //मान

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    यदि (fmt_spec == 'p')
		फर्काऊ TYPE_POINTER;
	    अन्यथा यदि (fmt_spec == 'b' || fmt_spec == 'B')
		फर्काऊ TYPE_UNSIGNEDLONGLONGINT;
	    अन्यथा यदि (fmt_spec == 'd')
	    {
		// signed
		विकल्प (length_modifier)
		{
		पक्ष '\0':
		पक्ष 'h':
		    // char and short arguments are passed as int.
		    फर्काऊ TYPE_INT;
		पक्ष 'l':
		    फर्काऊ TYPE_LONGINT;
		पक्ष 'L':
		    फर्काऊ TYPE_LONGLONGINT;
		}
	    }
	    अन्यथा
	    {
		// unsigned
		विकल्प (length_modifier)
		{
		    पक्ष '\0':
		    पक्ष 'h':
			फर्काऊ TYPE_UNSIGNEDINT;
		    पक्ष 'l':
			फर्काऊ TYPE_UNSIGNEDLONGINT;
		    पक्ष 'L':
			फर्काऊ TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	रोक;

    पक्ष 'f':
    पक्ष 'F':
    पक्ष 'e':
    पक्ष 'E':
    पक्ष 'g':
    पक्ष 'G':
	फर्काऊ TYPE_FLOAT;
    }

    फर्काऊ TYPE_UNKNOWN;
}

    स्थिर वर्ण *
format_typename(
    अचर वर्ण  *type)
{
    विकल्प (format_typeof(type))
    {
	पक्ष TYPE_INT:
	    फर्काऊ typename_int;

	पक्ष TYPE_LONGINT:
	    फर्काऊ typename_longint;

	पक्ष TYPE_LONGLONGINT:
	    फर्काऊ typename_longlongint;

	पक्ष TYPE_UNSIGNEDINT:
	    फर्काऊ typename_unsignedint;

	पक्ष TYPE_UNSIGNEDLONGINT:
	    फर्काऊ typename_unsignedlongint;

	पक्ष TYPE_UNSIGNEDLONGLONGINT:
	    फर्काऊ typename_unsignedlonglongint;

	पक्ष TYPE_POINTER:
	    फर्काऊ _(typename_pointer);

	पक्ष TYPE_PERCENT:
	    फर्काऊ _(typename_percent);

	पक्ष TYPE_CHAR:
	    फर्काऊ typename_char;

	पक्ष TYPE_STRING:
	    फर्काऊ _(typename_string);

	पक्ष TYPE_FLOAT:
	    फर्काऊ typename_float;
    }

    फर्काऊ _(typename_unknown);
}

    स्थिर पूर्णाङ्क
adjust_types(
    अचर वर्ण ***ap_types,
    पूर्णाङ्क arg,
    पूर्णाङ्क *अङ्क_posarg,
    अचर वर्ण *type)
{
    यदि (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	फर्काऊ FAIL;
    }

    यदि (*ap_types == रिक्त || *अङ्क_posarg < arg)
    {
	पूर्णाङ्क	    idx;
	अचर वर्ण  **new_types;

	यदि (*ap_types == रिक्त)
	    new_types = ALLOC_CLEAR_MULT(अचर वर्ण *, arg);
	अन्यथा
	    new_types = vim_realloc((वर्ण **)*ap_types,
						arg * परिमाण(अचर वर्ण *));

	यदि (new_types == रिक्त)
	    फर्काऊ FAIL;

	लागि (idx = *अङ्क_posarg; idx < arg; ++idx)
	    new_types[idx] = रिक्त;

	*ap_types = new_types;
	*अङ्क_posarg = arg;
    }

    यदि ((*ap_types)[arg - 1] != रिक्त)
    {
	यदि ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    अचर वर्ण *pt = type;
	    यदि (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    यदि (pt[0] != '*')
	    {
		विकल्प (pt[0])
		{
		    पक्ष 'd':
		    पक्ष 'i': रोक;
		    स्वभाव:
			semsg(_(e_positional_अङ्क_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			फर्काऊ FAIL;
		}
	    }
	}
	अन्यथा
	{
	    यदि (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_अङ्क_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		फर्काऊ FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    फर्काऊ OK;
}

    स्थिर शून्य
format_overflow_error(अचर वर्ण *pstart)
{
    माप_प्रकार	arglen = 0;
    वर्ण	*argcopy = रिक्त;
    अचर वर्ण	*p = pstart;

    जबसम्म (VIM_ISDIGIT((पूर्णाङ्क)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(वर्ण, arglen + 1);
    यदि (argcopy != रिक्त)
    {
	म_नकलन(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	मुक्त(argcopy);
    }
    अन्यथा
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    स्थिर पूर्णाङ्क
get_unsigned_int(
    अचर वर्ण *pstart,
    अचर वर्ण **p,
    अचिन्हित पूर्णाङ्क *uj,
    पूर्णाङ्क overflow_err)
{
    *uj = **p - '0';
    ++*p;

    जबसम्म (VIM_ISDIGIT((पूर्णाङ्क)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (अचिन्हित पूर्णाङ्क)(**p - '0');
	++*p;
    }

    यदि (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	यदि (overflow_err)
	{
	    format_overflow_error(pstart);
	    फर्काऊ FAIL;
	}
	अन्यथा
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    फर्काऊ OK;
}


    स्थिर पूर्णाङ्क
parse_fmt_types(
    अचर वर्ण  ***ap_types,
    पूर्णाङ्क		*अङ्क_posarg,
    अचर वर्ण  *fmt,
    typval_T	*tvs UNUSED
    )
{
    अचर वर्ण	*p = fmt;
    अचर वर्ण	*arg = रिक्त;

    पूर्णाङ्क		any_pos = 0;
    पूर्णाङ्क		any_arg = 0;
    पूर्णाङ्क		arg_idx;

# define CHECK_POS_ARG गर { \
    यदि (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	गच्छ error; \
    } \
} जबसम्म (0);

    यदि (p == रिक्त)
	फर्काऊ OK;

    जबसम्म (*p != NUL)
    {
	यदि (*p != '%')
	{
	    अचर वर्ण    *q = म_अक्षर(p + 1, '%');
	    माप_प्रकार  n = (q == रिक्त) ? STRLEN(p) : (माप_प्रकार)(q - p);

	    p += n;
	}
	अन्यथा
	{
	    // allowed values: \0, h, l, L
	    वर्ण	length_modifier = '\0';

	    // variable for positional arg
	    पूर्णाङ्क		pos_arg = -1;
	    अचर वर्ण	*ptype = रिक्त;
	    अचर वर्ण	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    जबसम्म (VIM_ISDIGIT(*ptype))
		++ptype;

	    यदि (*ptype == '$')
	    {
		यदि (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    गच्छ error;
		}

		// Positional argument
		अचिन्हित पूर्णाङ्क uj;

		यदि (get_unsigned_int(pstart, &p, &uj, tvs != रिक्त) == FAIL)
		    गच्छ error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    जबसम्म (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		विकल्प (*p)
		{
		    पक्ष '0': रोक;
		    पक्ष '-': रोक;
		    पक्ष '+': रोक;
		    पक्ष ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      रोक;
		    पक्ष '#': रोक;
		    पक्ष '\'': रोक;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    यदि (*(arg = p) == '*')
	    {
		p++;

		यदि (VIM_ISDIGIT((पूर्णाङ्क)(*p)))
		{
		    // Positional argument field width
		    अचिन्हित पूर्णाङ्क uj;

		    यदि (get_unsigned_int(arg + 1, &p, &uj, tvs != रिक्त) == FAIL)
			गच्छ error;

		    यदि (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			गच्छ error;
		    }
		    अन्यथा
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			यदि (adjust_types(ap_types, uj, अङ्क_posarg, arg) == FAIL)
			    गच्छ error;
		    }
		}
		अन्यथा
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    अन्यथा यदि (VIM_ISDIGIT((पूर्णाङ्क)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		अचर वर्ण *digstart = p;
		अचिन्हित पूर्णाङ्क uj;

		यदि (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
		    गच्छ error;

		यदि (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    गच्छ error;
		}
	    }

	    // parse precision
	    यदि (*p == '.')
	    {
		p++;

		यदि (*(arg = p) == '*')
		{
		    p++;

		    यदि (VIM_ISDIGIT((पूर्णाङ्क)(*p)))
		    {
			// Parse precision
			अचिन्हित पूर्णाङ्क uj;

			यदि (get_unsigned_int(arg + 1, &p, &uj, tvs != रिक्त) == FAIL)
			    गच्छ error;

			यदि (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    यदि (adjust_types(ap_types, uj, अङ्क_posarg, arg) == FAIL)
				गच्छ error;
			}
			अन्यथा
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    गच्छ error;
			}
		    }
		    अन्यथा
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		अन्यथा यदि (VIM_ISDIGIT((पूर्णाङ्क)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    अचर वर्ण *digstart = p;
		    अचिन्हित पूर्णाङ्क uj;

		    यदि (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
			गच्छ error;

		    यदि (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			गच्छ error;
		    }
		}
	    }

	    यदि (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    यदि (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		यदि (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    विकल्प (*p)
	    {
		// Check for known format specifiers. % is special!
		पक्ष 'i':
		पक्ष '*':
		पक्ष 'd':
		पक्ष 'u':
		पक्ष 'o':
		पक्ष 'D':
		पक्ष 'U':
		पक्ष 'O':
		पक्ष 'x':
		पक्ष 'X':
		पक्ष 'b':
		पक्ष 'B':
		पक्ष 'c':
		पक्ष 's':
		पक्ष 'S':
		पक्ष 'p':
		पक्ष 'f':
		पक्ष 'F':
		पक्ष 'e':
		पक्ष 'E':
		पक्ष 'g':
		पक्ष 'G':
		    यदि (pos_arg != -1)
		    {
			यदि (adjust_types(ap_types, pos_arg, अङ्क_posarg, ptype) == FAIL)
			    गच्छ error;
		    }
		    अन्यथा
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    रोक;

		स्वभाव:
		    यदि (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			गच्छ error;
		    }
	    }

	    यदि (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    लागि (arg_idx = 0; arg_idx < *अङ्क_posarg; ++arg_idx)
    {
	यदि ((*ap_types)[arg_idx] == रिक्त)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    गच्छ error;
	}

# यदि defined(FEAT_EVAL)
	यदि (tvs != रिक्त && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    गच्छ error;
	}
# endif
    }

    फर्काऊ OK;

error:
    vim_free((वर्ण**)*ap_types);
    *ap_types = रिक्त;
    *अङ्क_posarg = 0;
    फर्काऊ FAIL;
}

    स्थिर शून्य
skip_to_arg(
    अचर वर्ण	**ap_types,
    बहु_सूची	ap_start,
    बहु_सूची	*ap,
    पूर्णाङ्क		*arg_idx,
    पूर्णाङ्क		*arg_cur,
    अचर वर्ण	*fmt)
{
    पूर्णाङ्क		arg_min = 0;

    यदि (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	फर्काऊ;
    }

    यदि (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	बहु_पूर्ण(*ap);
	va_copy(*ap, ap_start);
    }
    अन्यथा
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    लागि (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	अचर वर्ण *p;

	यदि (ap_types == रिक्त || ap_types[*arg_cur] == रिक्त)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    फर्काऊ;
	}

	p = ap_types[*arg_cur];

	पूर्णाङ्क fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	विकल्प (fmt_type)
	{
	पक्ष TYPE_PERCENT:
	पक्ष TYPE_UNKNOWN:
	    रोक;

	पक्ष TYPE_CHAR:
	    बहु_तर्क(*ap, पूर्णाङ्क);
	    रोक;

	पक्ष TYPE_STRING:
	    बहु_तर्क(*ap, वर्ण *);
	    रोक;

	पक्ष TYPE_POINTER:
	    बहु_तर्क(*ap, शून्य *);
	    रोक;

	पक्ष TYPE_INT:
	    बहु_तर्क(*ap, पूर्णाङ्क);
	    रोक;

	पक्ष TYPE_LONGINT:
	    बहु_तर्क(*ap, दीर्घ पूर्णाङ्क);
	    रोक;

	पक्ष TYPE_LONGLONGINT:
	    बहु_तर्क(*ap, varnumber_T);
	    रोक;

	पक्ष TYPE_UNSIGNEDINT:
	    बहु_तर्क(*ap, अचिन्हित पूर्णाङ्क);
	    रोक;

	पक्ष TYPE_UNSIGNEDLONGINT:
	    बहु_तर्क(*ap, अचिन्हित दीर्घ पूर्णाङ्क);
	    रोक;

	पक्ष TYPE_UNSIGNEDLONGLONGINT:
	    बहु_तर्क(*ap, uvarnumber_T);
	    रोक;

	पक्ष TYPE_FLOAT:
	    बहु_तर्क(*ap, द्विगुण);
	    रोक;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    फर्काऊ;
}

    पूर्णाङ्क
vim_vsnprintf_typval(
    वर्ण	*str,
    माप_प्रकार	str_m,
    अचर वर्ण	*fmt,
    बहु_सूची	ap_start,
    typval_T	*tvs)
{
    माप_प्रकार	str_l = 0;
    अचर वर्ण	*p = fmt;
    पूर्णाङ्क		arg_cur = 0;
    पूर्णाङ्क		अङ्क_posarg = 0;
    पूर्णाङ्क		arg_idx = 1;
    बहु_सूची	ap;
    अचर वर्ण	**ap_types = रिक्त;

    यदि (parse_fmt_types(&ap_types, &अङ्क_posarg, fmt, tvs) == FAIL)
	फर्काऊ 0;

    va_copy(ap, ap_start);

    यदि (p == रिक्त)
	p = "";
    जबसम्म (*p != NUL)
    {
	यदि (*p != '%')
	{
	    अचर वर्ण    *q = म_अक्षर(p + 1, '%');
	    माप_प्रकार  n = (q == रिक्त) ? STRLEN(p) : (माप_प्रकार)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    यदि (str_l < str_m)
	    {
		माप_प्रकार avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	अन्यथा
	{
	    माप_प्रकार  min_field_width = 0, precision = 0;
	    पूर्णाङ्क	    zero_padding = 0, precision_specified = 0, justify_बायाँ = 0;
	    पूर्णाङ्क	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    पूर्णाङ्क	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    वर्ण    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    वर्ण    tmp[TMP_LEN];

	    // string address in case of string argument
	    अचर वर्ण  *str_arg = रिक्त;

	    // natural field width of arg without padding and sign
	    माप_प्रकार  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    अचिन्हित वर्ण uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    माप_प्रकार  सङ्ख्या_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    माप_प्रकार  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    वर्ण    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = रिक्त;

	    // variables for positional arg
	    पूर्णाङ्क	    pos_arg = -1;
	    अचर वर्ण	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    जबसम्म (VIM_ISDIGIT(*ptype))
		++ptype;

	    यदि (*ptype == '$')
	    {
		// Positional argument
		अचर वर्ण *digstart = p;
		अचिन्हित पूर्णाङ्क uj;

		यदि (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
		    गच्छ error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    जबसम्म (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		विकल्प (*p)
		{
		    पक्ष '0': zero_padding = 1; रोक;
		    पक्ष '-': justify_बायाँ = 1; रोक;
		    पक्ष '+': force_sign = 1; space_for_positive = 0; रोक;
		    पक्ष ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      रोक;
		    पक्ष '#': alternate_form = 1; रोक;
		    पक्ष '\'': रोक;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    यदि (*p == '*')
	    {
		पूर्णाङ्क j;
		अचर वर्ण *digstart = p + 1;

		p++;

		यदि (VIM_ISDIGIT((पूर्णाङ्क)(*p)))
		{
		    // Positional argument field width
		    अचिन्हित पूर्णाङ्क uj;

		    यदि (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
			गच्छ error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# यदि defined(FEAT_EVAL)
		    tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			बहु_तर्क(ap, पूर्णाङ्क));

		यदि (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    यदि (tvs != रिक्त)
		    {
			format_overflow_error(digstart);
			गच्छ error;
		    }
		    अन्यथा
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		यदि (j >= 0)
		    min_field_width = j;
		अन्यथा
		{
		    min_field_width = -j;
		    justify_बायाँ = 1;
		}
	    }
	    अन्यथा यदि (VIM_ISDIGIT((पूर्णाङ्क)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		अचर वर्ण *digstart = p;
		अचिन्हित पूर्णाङ्क uj;

		यदि (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
		    गच्छ error;

		min_field_width = uj;
	    }

	    // parse precision
	    यदि (*p == '.')
	    {
		p++;
		precision_specified = 1;

		यदि (VIM_ISDIGIT((पूर्णाङ्क)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    अचर वर्ण *digstart = p;
		    अचिन्हित पूर्णाङ्क uj;

		    यदि (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
			गच्छ error;

		    precision = uj;
		}
		अन्यथा यदि (*p == '*')
		{
		    पूर्णाङ्क j;
		    अचर वर्ण *digstart = p;

		    p++;

		    यदि (VIM_ISDIGIT((पूर्णाङ्क)(*p)))
		    {
			// positional argument
			अचिन्हित पूर्णाङ्क uj;

			यदि (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
			    गच्छ error;

			arg_idx = uj;

			++p;
		    }

		    j =
# यदि defined(FEAT_EVAL)
			tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    बहु_तर्क(ap, पूर्णाङ्क));

		    यदि (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			यदि (tvs != रिक्त)
			{
			    format_overflow_error(digstart);
			    गच्छ error;
			}
			अन्यथा
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    यदि (j >= 0)
			precision = j;
		    अन्यथा
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    यदि (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		यदि (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    length_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    विकल्प (fmt_spec)
	    {
		पक्ष 'i': fmt_spec = 'd'; रोक;
		पक्ष 'D': fmt_spec = 'd'; length_modifier = 'l'; रोक;
		पक्ष 'U': fmt_spec = 'u'; length_modifier = 'l'; रोक;
		पक्ष 'O': fmt_spec = 'o'; length_modifier = 'l'; रोक;
		स्वभाव: रोक;
	    }

# यदि defined(FEAT_EVAL)
	    विकल्प (fmt_spec)
	    {
		पक्ष 'd': पक्ष 'u': पक्ष 'o': पक्ष 'x': पक्ष 'X':
		    यदि (tvs != रिक्त && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    यदि (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    विकल्प (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    पक्ष '%':
	    पक्ष 'c':
	    पक्ष 's':
	    पक्ष 'S':
		str_arg_l = 1;
		विकल्प (fmt_spec)
		{
		पक्ष '%':
		    str_arg = p;
		    रोक;

		पक्ष 'c':
		    {
			पूर्णाङ्क j;

			j =
# यदि defined(FEAT_EVAL)
			    tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				बहु_तर्क(ap, पूर्णाङ्क));

			// standard demands unsigned char
			uchar_arg = (अचिन्हित वर्ण)j;
			str_arg = (वर्ण *)&uchar_arg;
			रोक;
		    }

		पक्ष 's':
		पक्ष 'S':
		    str_arg =
# यदि defined(FEAT_EVAL)
				tvs != रिक्त ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    बहु_तर्क(ap, वर्ण *));

		    यदि (str_arg == रिक्त)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    अन्यथा यदि (!precision_specified)
			str_arg_l = म_माप(str_arg);
		    // truncate string if necessary as requested by precision
		    अन्यथा यदि (precision == 0)
			str_arg_l = 0;
		    अन्यथा
		    {
			// memchr on HP does not like n > 2^31  !!!
			अचर वर्ण *q = स_प्रथम(str_arg, '\0',
				  precision <= (माप_प्रकार)0x7fffffffL ? precision
						       : (माप_प्रकार)0x7fffffffL);

			str_arg_l = (q == रिक्त) ? precision
						      : (माप_प्रकार)(q - str_arg);
		    }
		    यदि (fmt_spec == 'S')
		    {
			char_u	*p1;
			माप_प्रकार	i;
			पूर्णाङ्क	cell;

			लागि (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    यदि (precision_specified && i + cell > precision)
				रोक;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			यदि (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    रोक;

		स्वभाव:
		    रोक;
		}
		रोक;

	    पक्ष 'd': पक्ष 'u':
	    पक्ष 'b': पक्ष 'B':
	    पक्ष 'o':
	    पक्ष 'x': पक्ष 'X':
	    पक्ष 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //मान

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    पूर्णाङ्क arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    पूर्णाङ्क int_arg = 0;
		    अचिन्हित पूर्णाङ्क uint_arg = 0;

		    // only set for length modifier l
		    दीर्घ पूर्णाङ्क long_arg = 0;
		    अचिन्हित दीर्घ पूर्णाङ्क ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    शून्य *ptr_arg = रिक्त;

		    यदि (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# यदि defined(FEAT_EVAL)
				 tvs != रिक्त ? (शून्य *)tv_str(tvs, &arg_idx,
									रिक्त) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					बहु_तर्क(ap, शून्य *));

			यदि (ptr_arg != रिक्त)
			    arg_sign = 1;
		    }
		    अन्यथा यदि (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# यदि defined(FEAT_EVAL)
				    tvs != रिक्त ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					बहु_तर्क(ap, uvarnumber_T));

			यदि (bin_arg != 0)
			    arg_sign = 1;
		    }
		    अन्यथा यदि (fmt_spec == 'd')
		    {
			// signed
			विकल्प (length_modifier)
			{
			पक्ष '\0':
			पक्ष 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# यदि defined(FEAT_EVAL)
					tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    बहु_तर्क(ap, पूर्णाङ्क));

			    यदि (int_arg > 0)
				arg_sign =  1;
			    अन्यथा यदि (int_arg < 0)
				arg_sign = -1;
			    रोक;
			पक्ष 'l':
			    long_arg =
# यदि defined(FEAT_EVAL)
					tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    बहु_तर्क(ap, दीर्घ पूर्णाङ्क));

			    यदि (long_arg > 0)
				arg_sign =  1;
			    अन्यथा यदि (long_arg < 0)
				arg_sign = -1;
			    रोक;
			पक्ष 'L':
			    llong_arg =
# यदि defined(FEAT_EVAL)
					tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    बहु_तर्क(ap, varnumber_T));

			    यदि (llong_arg > 0)
				arg_sign =  1;
			    अन्यथा यदि (llong_arg < 0)
				arg_sign = -1;
			    रोक;
			}
		    }
		    अन्यथा
		    {
			// unsigned
			विकल्प (length_modifier)
			{
			    पक्ष '\0':
			    पक्ष 'h':
				uint_arg =
# यदि defined(FEAT_EVAL)
					    tvs != रिक्त ? (अचिन्हित)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						बहु_तर्क(ap, अचिन्हित पूर्णाङ्क));

				यदि (uint_arg != 0)
				    arg_sign = 1;
				रोक;
			    पक्ष 'l':
				ulong_arg =
# यदि defined(FEAT_EVAL)
					    tvs != रिक्त ? (अचिन्हित दीर्घ)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						बहु_तर्क(ap, अचिन्हित दीर्घ पूर्णाङ्क));

				यदि (ulong_arg != 0)
				    arg_sign = 1;
				रोक;
			    पक्ष 'L':
				ullong_arg =
# यदि defined(FEAT_EVAL)
					    tvs != रिक्त ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						बहु_तर्क(ap, uvarnumber_T));

				यदि (ullong_arg != 0)
				    arg_sign = 1;
				रोक;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    यदि (precision_specified)
			zero_padding = 0;
		    यदि (fmt_spec == 'd')
		    {
			यदि (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    अन्यथा यदि (alternate_form)
		    {
			यदि (arg_sign != 0
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
		    यदि (!precision_specified)
			precision = 1;   // default precision is 1
		    यदि (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    अन्यथा
		    {
			वर्ण	f[6];
			पूर्णाङ्क	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			यदि (!length_modifier)
			    ;
			अन्यथा यदि (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# अन्यथा
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			अन्यथा
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			यदि (fmt_spec == 'p')
			    str_arg_l += प्र_लिखो(tmp + str_arg_l, f, ptr_arg);
			अन्यथा यदि (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    वर्ण	    b[8 * परिमाण(uvarnumber_T)];
			    माप_प्रकार	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    गर
			    {
				b[परिमाण(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    जबसम्म (bn != 0);

			    स_नकल(tmp + str_arg_l, b + परिमाण(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			अन्यथा यदि (fmt_spec == 'd')
			{
			    // signed
			    विकल्प (length_modifier)
			    {
			    पक्ष '\0': str_arg_l += प्र_लिखो(
						 tmp + str_arg_l, f,
						 int_arg);
				       रोक;
			    पक्ष 'h': str_arg_l += प्र_लिखो(
						 tmp + str_arg_l, f,
						 (लघु)int_arg);
				      रोक;
			    पक्ष 'l': str_arg_l += प्र_लिखो(
						tmp + str_arg_l, f, long_arg);
				      रोक;
			    पक्ष 'L': str_arg_l += प्र_लिखो(
					       tmp + str_arg_l, f, llong_arg);
				      रोक;
			    }
			}
			अन्यथा
			{
			    // unsigned
			    विकल्प (length_modifier)
			    {
			    पक्ष '\0': str_arg_l += प्र_लिखो(
						tmp + str_arg_l, f,
						uint_arg);
				       रोक;
			    पक्ष 'h': str_arg_l += प्र_लिखो(
						tmp + str_arg_l, f,
						(अचिन्हित लघु)uint_arg);
				      रोक;
			    पक्ष 'l': str_arg_l += प्र_लिखो(
					       tmp + str_arg_l, f, ulong_arg);
				      रोक;
			    पक्ष 'L': str_arg_l += प्र_लिखो(
					      tmp + str_arg_l, f, ullong_arg);
				      रोक;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			यदि (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			यदि (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			माप_प्रकार अङ्क_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			यदि (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    यदि (!precision_specified
					     || precision < अङ्क_of_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = अङ्क_of_digits + 1;
			    }
			}
			// zero padding to specified precision?
			यदि (अङ्क_of_digits < precision)
			    सङ्ख्या_of_zeros_to_pad = precision - अङ्क_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    यदि (!justify_बायाँ && zero_padding)
		    {
			पूर्णाङ्क n = (पूर्णाङ्क)(min_field_width - (str_arg_l
						    + सङ्ख्या_of_zeros_to_pad));
			यदि (n > 0)
			    सङ्ख्या_of_zeros_to_pad += n;
		    }
		    रोक;
		}

	    पक्ष 'f':
	    पक्ष 'F':
	    पक्ष 'e':
	    पक्ष 'E':
	    पक्ष 'g':
	    पक्ष 'G':
		{
		    // Floating point.
		    द्विगुण	f;
		    द्विगुण	abs_f;
		    वर्ण	format[40];
		    पूर्णाङ्क		l;
		    पूर्णाङ्क		remove_trailing_zeroes = FALSE;

		    f =
# यदि defined(FEAT_EVAL)
			tvs != रिक्त ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    बहु_तर्क(ap, द्विगुण));

		    abs_f = f < 0 ? -f : f;

		    यदि (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			यदि ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			अन्यथा
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    यदि ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# अन्यथा
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
		    अन्यथा
		    {
			यदि (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			अन्यथा यदि (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			अन्यथा
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    यदि (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    यदि (precision_specified)
			    {
				माप_प्रकार max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				यदि ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (माप_प्रकार)log10(abs_f);
				यदि (precision > max_prec)
				    precision = max_prec;
				l += प्र_लिखो(format + l, ".%d", (पूर्णाङ्क)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = प्र_लिखो(tmp, format, f);
			}

			यदि (remove_trailing_zeroes)
			{
			    पूर्णाङ्क i;
			    वर्ण *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    यदि (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    अन्यथा
			    {
				tp = (वर्ण *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				यदि (tp != रिक्त)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    यदि (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    जबसम्म (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    यदि (tp != रिक्त && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				जबसम्म (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			अन्यथा
			{
			    वर्ण *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (वर्ण *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    यदि (tp != रिक्त && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    यदि (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			सङ्ख्या_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    रोक;
		}

	    स्वभाव:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_बायाँ = 1;
		min_field_width = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		यदि (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		रोक;
	    }

	    यदि (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    यदि (!justify_बायाँ)
	    {
		// left padding with blank or zero
		पूर्णाङ्क pn = (पूर्णाङ्क)(min_field_width - (str_arg_l + सङ्ख्या_of_zeros_to_pad));

		यदि (pn > 0)
		{
		    यदि (str_l < str_m)
		    {
			माप_प्रकार avail = str_m - str_l;

			vim_memset(str + str_l, zero_padding ? '0' : ' ',
					     (माप_प्रकार)pn > avail ? avail
								: (माप_प्रकार)pn);
		    }
		    str_l += pn;
		}
	    }

	    // zero padding as requested by the precision or by the minimal
	    // field width for numeric conversions required?
	    यदि (सङ्ख्या_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    अन्यथा
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		पूर्णाङ्क zn = (पूर्णाङ्क)zero_padding_insertion_ind;

		यदि (zn > 0)
		{
		    यदि (str_l < str_m)
		    {
			माप_प्रकार avail = str_m - str_l;

			mch_memmove(str + str_l, str_arg,
					     (माप_प्रकार)zn > avail ? avail
								: (माप_प्रकार)zn);
		    }
		    str_l += zn;
		}

		// insert zero padding as requested by the precision or min
		// field width
		zn = (पूर्णाङ्क)सङ्ख्या_of_zeros_to_pad;
		यदि (zn > 0)
		{
		    यदि (str_l < str_m)
		    {
			माप_प्रकार avail = str_m - str_l;

			vim_memset(str + str_l, '0',
					     (माप_प्रकार)zn > avail ? avail
								: (माप_प्रकार)zn);
		    }
		    str_l += zn;
		}
	    }

	    // insert formatted string
	    // (or as-is conversion specifier for unknown conversions)
	    {
		पूर्णाङ्क sn = (पूर्णाङ्क)(str_arg_l - zero_padding_insertion_ind);

		यदि (sn > 0)
		{
		    यदि (str_l < str_m)
		    {
			माप_प्रकार avail = str_m - str_l;

			mch_memmove(str + str_l,
				str_arg + zero_padding_insertion_ind,
				(माप_प्रकार)sn > avail ? avail : (माप_प्रकार)sn);
		    }
		    str_l += sn;
		}
	    }

	    // insert right padding
	    यदि (justify_बायाँ)
	    {
		// right blank padding to the field width
		पूर्णाङ्क pn = (पूर्णाङ्क)(min_field_width
				      - (str_arg_l + सङ्ख्या_of_zeros_to_pad));

		यदि (pn > 0)
		{
		    यदि (str_l < str_m)
		    {
			माप_प्रकार avail = str_m - str_l;

			vim_memset(str + str_l, ' ',
					     (माप_प्रकार)pn > avail ? avail
								: (माप_प्रकार)pn);
		    }
		    str_l += pn;
		}
	    }
	    vim_free(tofree);
	}
    }

    यदि (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    यदि (tvs != रिक्त && tvs[अङ्क_posarg != 0 ? अङ्क_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((वर्ण*)ap_types);
    बहु_पूर्ण(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    फर्काऊ (पूर्णाङ्क)str_l;
}

#endif // PROTO
