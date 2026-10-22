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
    जर (p != रिक्त)
	mch_memmove(p, string, len);
    परत p;
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
    जर (p == रिक्त)
	परत रिक्त;

    STRNCPY(p, string, len);
    p[len] = NUL;
    परत p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    परत vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    पूर्णांक		cc,
    पूर्णांक		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    अचिन्हित	length;
    पूर्णांक		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    साठी (p = string; *p; p++)
    {
	जर (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    सुरूठेवा;
	}
	जर (vim_strchr(esc_chars, *p) != रिक्त || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    जर (escaped_string == रिक्त)
	परत रिक्त;
    p2 = escaped_string;
    साठी (p = string; *p; p++)
    {
	जर (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (माप_प्रकार)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    सुरूठेवा;
	}
	जर (vim_strchr(esc_chars, *p) != रिक्त || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    परत escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    पूर्णांक
csh_like_shell(शून्य)
{
    परत (म_माला((अक्षर *)gettail(p_sh), "csh") != रिक्त);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    स्थिर पूर्णांक
fish_like_shell(शून्य)
{
    परत (म_माला((अक्षर *)gettail(p_sh), "fish") != रिक्त);
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
vim_strsave_shellescape(char_u *string, पूर्णांक do_special, पूर्णांक do_newline)
{
    अचिन्हित	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    माप_प्रकार	l;
    पूर्णांक		csh_like;
    पूर्णांक		fish_like;
    char_u	*shname;
    पूर्णांक		powershell;
#ifdef MSWIN
    पूर्णांक		double_quotes;
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
    powershell = म_माला((अक्षर *)shname, "pwsh") != रिक्त;
#ifdef MSWIN
    powershell = powershell || म_माला((अक्षर *)shname, "powershell") != रिक्त;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (अचिन्हित)STRLEN(string) + 3;  // two quotes and a trailing NUL
    साठी (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	जर (double_quotes)
	{
	    जर (*p == '"')
		++length;		// " -> ""
	}
	अन्यथा
#endif
	जर (*p == '\'')
	{
	    जर (powershell)
		length += 2;		// ' => ''
	    अन्यथा
		length += 3;		// ' => '\''
	}
	जर ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    जर (csh_like && do_special)
		++length;		// insert backslash
	}
	जर (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	जर (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    जर (escaped_string != रिक्त)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	जर (double_quotes)
	    *d++ = '"';
	अन्यथा
#endif
	    *d++ = '\'';

	साठी (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    जर (double_quotes)
	    {
		जर (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    सुरूठेवा;
		}
	    }
	    अन्यथा
#endif
	    जर (*p == '\'')
	    {
		जर (powershell)
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
		सुरूठेवा;
	    }
	    जर ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		जर (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		सुरूठेवा;
	    }
	    जर (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		स_नकल(d, p, l);	// copy the var
		d += l;
		p += l;
		सुरूठेवा;
	    }
	    जर (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		सुरूठेवा;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	जर (double_quotes)
	    *d++ = '"';
	अन्यथा
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    परत escaped_string;
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
    परत p1;
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
    परत p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    शून्य
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    पूर्णांक	    c;

    जर (p == रिक्त)
	परत;

    p2 = p;
    पर्यंत ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#जर defined(FEAT_EVAL) || defined(FEAT_SPELL)
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

    जर (res != रिक्त)
	पर्यंत (*p != NUL)
	{
	    पूर्णांक		l;

	    जर (enc_utf8)
	    {
		पूर्णांक	c, uc;
		पूर्णांक	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		जर (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		जर (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    जर (s == रिक्त)
		    {
			vim_free(res);
			परत रिक्त;
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
	    अन्यथा जर (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    अन्यथा
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    परत res;
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

    जर (res != रिक्त)
	पर्यंत (*p != NUL)
	{
	    पूर्णांक		l;

	    जर (enc_utf8)
	    {
		पूर्णांक	c, lc;
		पूर्णांक	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		जर (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		जर (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    जर (s == रिक्त)
		    {
			vim_free(res);
			परत रिक्त;
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
	    अन्यथा जर (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    अन्यथा
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    परत res;
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
    पर्यंत (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
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

    जर (tolen + fromlen + 1 > tosize)
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
vim_strlen_maxlen(अक्षर *s, माप_प्रकार maxlen)
{
    माप_प्रकार i;
    साठी (i = 0; i < maxlen; ++i)
	जर (s[i] == NUL)
	    थांबा;
    परत i;
}

#जर !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    पूर्णांक
vim_stricmp(अक्षर *s1, अक्षर *s2)
{
    पूर्णांक		i;

    साठी (;;)
    {
	i = (पूर्णांक)TOLOWER_LOC(*s1) - (पूर्णांक)TOLOWER_LOC(*s2);
	जर (i != 0)
	    परत i;			    // this character different
	जर (*s1 == NUL)
	    थांबा;			    // strings match until NUL
	++s1;
	++s2;
    }
    परत 0;				    // strings match
}
#endif

#जर !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    पूर्णांक
vim_strnicmp(अक्षर *s1, अक्षर *s2, माप_प्रकार len)
{
    पूर्णांक		i;

    पर्यंत (len > 0)
    {
	i = (पूर्णांक)TOLOWER_LOC(*s1) - (पूर्णांक)TOLOWER_LOC(*s2);
	जर (i != 0)
	    परत i;			    // this character different
	जर (*s1 == NUL)
	    थांबा;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    परत 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    पूर्णांक
vim_strnicmp_asc(अक्षर *s1, अक्षर *s2, माप_प्रकार len)
{
    पूर्णांक                i = 0;

    पर्यंत (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	जर (i != 0)
	    थांबा;			// this character is different
	जर (*s1 == NUL)
	    थांबा;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    परत i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, पूर्णांक c)
{
    char_u	*p;
    पूर्णांक		b;

    p = string;
    जर (enc_utf8 && c > 0 && c < 0x80)
	परत vim_strbyte(string, c);
    जर (enc_utf8 && c >= 0x80)
    {
	पर्यंत (*p != NUL)
	{
	    पूर्णांक l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    जर (utf_ptr2char(p) == c && l > 1)
		परत p;
	    p += l;
	}
	परत रिक्त;
    }
    जर (enc_dbcs != 0 && c > 255)
    {
	पूर्णांक	n2 = c & 0xff;

	c = ((अचिन्हित)c >> 8) & 0xff;
	पर्यंत ((b = *p) != NUL)
	{
	    जर (b == c && p[1] == n2)
		परत p;
	    p += (*mb_ptr2len)(p);
	}
	परत रिक्त;
    }
    जर (has_mbyte)
    {
	पर्यंत ((b = *p) != NUL)
	{
	    जर (b == c)
		परत p;
	    p += (*mb_ptr2len)(p);
	}
	परत रिक्त;
    }
    पर्यंत ((b = *p) != NUL)
    {
	जर (b == c)
	    परत p;
	++p;
    }
    परत रिक्त;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, पूर्णांक c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    जर (c <= 0 || c > 255)
	परत रिक्त;
    परत (char_u *)म_अक्षर((अक्षर *)string, c);
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
vim_strrchr(char_u *string, पूर्णांक c)
{
    char_u	*retval = रिक्त;
    char_u	*p = string;

    पर्यंत (*p)
    {
	जर (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    परत retval;
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
    पर्यंत (*s)
    {
	जर (vim_strchr(charset, *s) != रिक्त)
	    परत s;
	MB_PTR_ADV(s);
    }
    परत रिक्त;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
स्थिर पूर्णांक sort_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2);

    स्थिर पूर्णांक
sort_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2)
{
    परत STRCMP(*(अक्षर **)s1, *(अक्षर **)s2);
}

    शून्य
sort_strings(
    char_u	**files,
    पूर्णांक		मोजणी)
{
    क्विक((शून्य *)files, (माप_प्रकार)मोजणी, आकार(char_u *), sort_compare);
}

#जर defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    पूर्णांक
has_non_ascii(char_u *s)
{
    char_u	*p;

    जर (s != रिक्त)
	साठी (p = s; *p != NUL; ++p)
	    जर (*p >= 128)
		परत TRUE;
    परत FALSE;
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
    जर (dest == रिक्त)
	परत रिक्त;
    जर (str1 == रिक्त)
	*dest = NUL;
    अन्यथा
	STRCPY(dest, str1);
    जर (str2 != रिक्त)
	STRCPY(dest + l, str2);
    परत dest;
}

#जर defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    माप_प्रकार len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    जर (rev == रिक्त)
	परत रिक्त;

    साठी (माप_प्रकार s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	जर (has_mbyte)
	{
	    पूर्णांक mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	अन्यथा
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    परत rev;
}
#endif

#जर defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, पूर्णांक function)
{
    अचिन्हित	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    जर (str != रिक्त)
    {
	len += (अचिन्हित)STRLEN(str);
	साठी (p = str; *p != NUL; MB_PTR_ADV(p))
	    जर (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    जर (r == रिक्त)
	परत रिक्त;

    जर (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    अन्यथा
	*r++ = '\'';
    जर (str != रिक्त)
	साठी (p = str; *p != NUL; )
	{
	    जर (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    जर (function)
	*r++ = ')';
    *r++ = NUL;
    परत s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    दीर्घ
string_मोजणी(char_u *haystack, char_u *needle, पूर्णांक ic)
{
    दीर्घ	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    जर (p == रिक्त || needle == रिक्त || *needle == NUL)
	परत 0;

    माप_प्रकार  needlelen = STRLEN(needle);
    जर (ic)
    {
	पर्यंत (*p != NUL)
	{
	    जर (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    अन्यथा
		MB_PTR_ADV(p);
	}
    }
    अन्यथा
	पर्यंत ((next = (char_u *)म_माला((अक्षर *)p, (अक्षर *)needle)) != रिक्त)
	{
	    ++n;
	    p = next + needlelen;
	}

    परत n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    स्थिर पूर्णांक
copy_first_char_to_tv(char_u *आदान, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    पूर्णांक		len;

    जर (आदान == रिक्त || output == रिक्त)
	परत -1;

    len = has_mbyte ? mb_ptr2len(आदान) : 1;
    STRNCPY(buf, आदान, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    परत output->vval.v_string == रिक्त ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    शून्य
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*अभिव्यक्ती,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    पूर्णांक		len = 0;
    पूर्णांक		idx = 0;
    पूर्णांक		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_किल्ली, VAR_संख्या);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_अभिव्यक्ती_get_funccal(अभिव्यक्ती, &newtv);

    ga_init2(&ga, आकार(अक्षर), 80);
    साठी (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	जर (len < 0)
	    थांबा;

	set_vim_var_nr(VV_किल्ली, idx);
	जर (filter_map_one(&tv, अभिव्यक्ती, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    थांबा;
	}
	जर (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    जर (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		थांबा;
	    }
	    अन्यथा
		ga_concat(&ga, newtv.vval.v_string);
	}
	अन्यथा जर (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    जर (fc != रिक्त)
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
	typval_T	*अभिव्यक्ती,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    पूर्णांक		len;
    typval_T	argv[3];
    पूर्णांक		r;
    पूर्णांक		called_emsg_start = called_emsg;
    funccall_T	*fc;

    जर (argvars[2].v_type == VAR_UNKNOWN)
    {
	जर (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_मूल्य), "String");
	    परत;
	}
	len = copy_first_char_to_tv(p, rettv);
	जर (len < 0)
	    परत;
	p += len;
    }
    अन्यथा जर (check_for_string_arg(argvars, 2) == FAIL)
	परत;
    अन्यथा
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_अभिव्यक्ती_get_funccal(अभिव्यक्ती, rettv);

    साठी ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	जर (len < 0)
	{
	    *rettv = argv[0];
	    थांबा;
	}

	r = eval_अभिव्यक्ती_typval(अभिव्यक्ती, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	जर (r == FAIL || called_emsg != called_emsg_start)
	    थांबा;
    }

    जर (fc != रिक्त)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    स्थिर शून्य
byteidx_common(typval_T *argvars, typval_T *rettv, पूर्णांक comp)
{
    rettv->vval.v_संख्या = -1;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	परत;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_संख्या_chk(&argvars[1], रिक्त);
    जर (str == रिक्त || idx < 0)
	परत;

    varnumber_T	utf16idx = FALSE;
    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	पूर्णांक error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	जर (error)
	    परत;
	जर (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_संख्या_as_bool_nr), utf16idx);
	    परत;
	}
    }

    पूर्णांक (*ptr2len)(char_u *);
    जर (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    अन्यथा
	ptr2len = mb_ptr2len;

    char_u *t = str;
    साठी ( ; idx > 0; idx--)
    {
	जर (*t == NUL)		// EOL reached
	    परत;
	जर (utf16idx)
	{
	    पूर्णांक clen = ptr2len(t);
	    पूर्णांक c = (clen > 1) ? utf_ptr2char(t) : *t;
	    जर (c > 0xFFFF)
		idx--;
	    जर (idx > 0)
		t += clen;
	}
	अन्यथा जर (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_संख्या = (varnumber_T)(t - str);
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
    rettv->vval.v_संख्या = -1;

    जर (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	परत;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_संख्या_chk(&argvars[1], रिक्त);
    जर (str == रिक्त || idx < 0)
	परत;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	जर (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    पूर्णांक (*ptr2len)(char_u *);
    जर (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    अन्यथा
	ptr2len = mb_ptr2len;

    char_u	*p;
    पूर्णांक		len;
    साठी (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	जर (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    जर (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_संख्या = len;
	    परत;
	}
	जर (utf16idx)
	{
	    idx--;
	    पूर्णांक clen = ptr2len(p);
	    पूर्णांक c = (clen > 1) ? utf_ptr2char(p) : *p;
	    जर (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_संख्या = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    स्थिर पूर्णांक
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    जर (convert_setup(&vimconv, from, to) == FAIL)
	परत FAIL;
    vimconv.vc_fail = TRUE;
    जर (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	जर (ret->string == रिक्त)
	    ret->length = 0;
	अन्यथा
	    ret->length = str->length;
    }
    अन्यथा
    {
	पूर्णांक len = (पूर्णांक)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, रिक्त, रिक्त);

    परत (ret->string == रिक्त) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    स्थिर शून्य
blob_from_string(char_u *str, blob_T *blob)
{
    पूर्णांक	    len = (पूर्णांक)STRLEN(str);
    char_u  *dest;

    जर (len == 0)
	परत;
    जर (ga_grow(&blob->bv_ga, len) == FAIL)
	परत;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (माप_प्रकार)len);
    // Translate newlines in the string to NUL characters
    साठी (पूर्णांक i = 0; i < len; ++i)
	जर (dest[i] == NL)
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
    स्थिर पूर्णांक
string_from_blob(blob_T *blob, दीर्घ *start_idx, string_T *ret)
{
    दीर्घ	blen = blob_len(blob);
    दीर्घ	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    दीर्घ	ओळ_len;

    जर (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	परत (ret->string == रिक्त) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)स_प्रथम(src, NL, (माप_प्रकार)(blen - start));
    ओळ_len = (nl == रिक्त) ? (blen - start) : (दीर्घ)(nl - src);

    ret->string = alloc(ओळ_len + 1);
    जर (ret->string == रिक्त)
    {
	ret->length = 0;
	परत FAIL;
    }
    जर (ओळ_len > 0)
	mch_memmove(ret->string, src, (माप_प्रकार)ओळ_len);
    ret->string[ओळ_len] = NUL;
    ret->length = (माप_प्रकार)ओळ_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    साठी (दीर्घ i = 0; i < ओळ_len; i++)
	जर (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + ओळ_len + (nl != रिक्त ? 1 : 0);
    परत OK;
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
    जर (from_encoding_raw == रिक्त)
	परत रिक्त;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    पर्यंत (*s != NUL)
    {
	जर (*s == '_')
	    *pe++ = '-';
	अन्यथा
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    जर ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    परत from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    स्थिर शून्य
append_converted_string_to_list(
	string_T *converted,
	पूर्णांक validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    जर (converted->string != रिक्त)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	पर्यंत (p < end)
	{
	    string_T	ओळ;
	    char_u	*ओळ_start = p;

	    पर्यंत (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    ओळ.length = (माप_प्रकार)(p - ओळ_start);
	    ओळ.string = vim_strnsave(ओळ_start, ओळ.length);
	    जर (ओळ.string != रिक्त)
	    {
		जर (validate_utf8 && !utf_valid_string(ओळ.string, रिक्त))
		{
		    vim_free(ओळ.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    परत; // Stop processing
		}
		जर (list_append_string(list, ओळ.string, (पूर्णांक)ओळ.length) == FAIL)
		{
		    vim_free(ओळ.string);
		    vim_free(converted->string);
		    परत; // Stop processing on append failure
		}
		vim_free(ओळ.string);
	    }
	    अन्यथा
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		परत;
	    }

	    जर (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    अन्यथा
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    स्थिर पूर्णांक
append_validated_ओळ_to_list(string_T *ओळ, पूर्णांक validate_utf8, list_T *list)
{
    जर (validate_utf8 && !utf_valid_string(ओळ->string, रिक्त))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(ओळ->string);
	परत FAIL;
    }

    पूर्णांक ret = list_append_string(list, ओळ->string, (पूर्णांक)ओळ->length);
    vim_free(ओळ->string);
    परत ret;
}

    शून्य
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    पूर्णांक		blen;
    दीर्घ	idx;
    पूर्णांक		validate_utf8 = FALSE;

    जर (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	परत;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    blob = argvars->vval.v_blob;
    जर (blob == रिक्त)
	परत;
    blen = blob_len(blob);

    char_u	*from_encoding = रिक्त;
    char_u	*from_encoding_raw = रिक्त;  // Encoding name with endianness preserved for iconv
    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	जर (d != रिक्त)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    जर (enc != रिक्त)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		जर (from_encoding_raw == रिक्त)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    परत;
		}
	    }
	}
    }

    जर (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    जर (from_encoding != रिक्त && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    पूर्णांक from_prop = 0;
    जर (from_encoding != रिक्त)
	from_prop = enc_canon_props(from_encoding);
    जर (from_encoding != रिक्त && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	पूर्णांक nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	जर (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    जर (blen > 0)
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
	जर (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    जा done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	पूर्णांक len = blen;
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
	पर्यंत (idx < blen)
	{
	    string_T	str;

	    जर (string_from_blob(blob, &idx, &str) != OK)
		थांबा;

	    जर (from_encoding != रिक्त)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		पूर्णांक	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		जर (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    जा done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    जर (append_validated_ओळ_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		जा done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    जर (blen > 0 && blob_get(blob, blen - 1) == NL)
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

    जर (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	परत;

    जर (rettv_blob_alloc(rettv) == FAIL)
	परत;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    जर (list == रिक्त)
	परत;

    char_u	*to_encoding = रिक्त;
    char_u	*to_encoding_raw = रिक्त;  // Encoding name with endianness preserved for iconv
    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	जर (d != रिक्त)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    जर (enc != रिक्त)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		जर (to_encoding_raw == रिक्त)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    परत;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    पूर्णांक to_prop = 0;
    जर (to_encoding != रिक्त)
	to_prop = enc_canon_props(to_encoding);
    जर (to_encoding != रिक्त && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_वस्तुसंच(list, li)
	{
	    char_u *s;

	    जर (li->li_tv.v_type != VAR_STRING)
		सुरूठेवा;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    जर (li != list->lv_first)
		ga_append(&str_ga, NL);
	    जर (s != रिक्त && *s != NUL)
	    {
		पूर्णांक slen = (पूर्णांक)STRLEN(s);

		जर (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    जा done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(माप_प्रकार)slen);
		str_ga.ga_len += slen;
	    }
	}

	जर (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    जर (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		जा done;
	    }
	    vimconv.vc_fail = TRUE;

	    पूर्णांक		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, रिक्त);
	    convert_setup(&vimconv, रिक्त, रिक्त);
	    ga_clear(&str_ga);

	    जर (converted == रिक्त)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		जा done;
	    }
	    जर (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
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
	FOR_ALL_LIST_वस्तुसंच(list, li)
	{
	    जर (li->li_tv.v_type != VAR_STRING)
		सुरूठेवा;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    जर (str.string == रिक्त)
		STR_LITERAL_SET(str, "");
	    अन्यथा
		str.length = STRLEN(str.string);

	    जर (to_encoding != रिक्त)
	    {
		पूर्णांक	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		जर (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    जा done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    जर (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    जर (to_encoding != रिक्त)
		vim_free(str.string);
	}
    }

done:
    जर (to_encoding != रिक्त)
	vim_free(to_encoding);
    जर (to_encoding_raw != रिक्त)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    शून्य
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    पूर्णांक		utf8 = FALSE;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (पूर्णांक)tv_get_bool_chk(&argvars[1], रिक्त);

    p = tv_get_string(&argvars[0]);

    जर (has_mbyte || utf8)
    {
	पूर्णांक (*ptr2len)(char_u *);
	पूर्णांक (*ptr2char)(char_u *);

	जर (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	अन्यथा
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	साठी ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_संख्या(rettv->vval.v_list, (*ptr2char)(p));
    }
    अन्यथा
	साठी ( ; *p != NUL; ++p)
	    list_append_संख्या(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    शून्य
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    पूर्णांक		base = 10;
    char_u	*p;
    varnumber_T	n;
    पूर्णांक		what = 0;
    पूर्णांक		isneg;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (पूर्णांक)tv_get_संख्या(&argvars[1]);
	जर (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    परत;
	}
	जर (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    जर (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    निवड (base)
    {
	स्थिती 2: what |= STR2NR_BIN + STR2NR_FORCE; थांबा;
	स्थिती 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; थांबा;
	स्थिती 16: what |= STR2NR_HEX + STR2NR_FORCE; थांबा;
    }
    vim_str2nr(p, रिक्त, रिक्त, what, &n, रिक्त, 0, FALSE, रिक्त);
    // Text after the number is silently ignored.
    जर (isneg)
	rettv->vval.v_संख्या = -n;
    अन्यथा
	rettv->vval.v_संख्या = n;

}

/*
 * "strgetchar()" function
 */
    शून्य
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    पूर्णांक		len;
    पूर्णांक		error = FALSE;
    पूर्णांक		charidx;
    पूर्णांक		byteidx = 0;

    rettv->vval.v_संख्या = -1;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	परत;

    str = tv_get_string_chk(&argvars[0]);
    जर (str == रिक्त)
	परत;
    len = (पूर्णांक)STRLEN(str);
    charidx = (पूर्णांक)tv_get_संख्या_chk(&argvars[1], &error);
    जर (error)
	परत;

    पर्यंत (charidx >= 0 && byteidx < len)
    {
	जर (charidx == 0)
	{
	    rettv->vval.v_संख्या = mb_ptr2char(str + byteidx);
	    थांबा;
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
    पूर्णांक		start_idx;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 2) == FAIL))
	परत;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_संख्या = -1;
    जर (needle == रिक्त || haystack == रिक्त)
	परत;		// type error; errmsg already given

    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	पूर्णांक	    error = FALSE;

	start_idx = (पूर्णांक)tv_get_संख्या_chk(&argvars[2], &error);
	जर (error || start_idx >= (पूर्णांक)STRLEN(haystack))
	    परत;
	जर (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)म_माला((अक्षर *)haystack, (अक्षर *)needle);
    जर (pos != रिक्त)
	rettv->vval.v_संख्या = (varnumber_T)(pos - save_haystack);
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
    जर (rettv->vval.v_string != रिक्त && tofree == रिक्त)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    शून्य
f_strlen(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script()
	    && check_for_string_or_संख्या_arg(argvars, 0) == FAIL)
	परत;

    rettv->vval.v_संख्या = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    स्थिर शून्य
strchar_common(typval_T *argvars, typval_T *rettv, पूर्णांक skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    पूर्णांक			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    पर्यंत (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_संख्या = len;
}

/*
 * "strcharlen()" function
 */
    शून्य
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script()
	    && check_for_string_or_संख्या_arg(argvars, 0) == FAIL)
	परत;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    शून्य
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	पूर्णांक error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	जर (error)
	    परत;
	जर (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_संख्या_as_bool_nr), skipcc);
	    परत;
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
    rettv->vval.v_संख्या = -1;

    जर (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	परत;

    varnumber_T countcc = FALSE;
    जर (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    पूर्णांक			(*func_mb_ptr2char_adv)(char_u **pp);
    पूर्णांक			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    पर्यंत (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	जर (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_संख्या = len;
}

/*
 * "strdisplaywidth()" function
 */
    शून्य
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    पूर्णांक		col = 0;

    rettv->vval.v_संख्या = -1;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	परत;

    s = tv_get_string(&argvars[0]);
    जर (argvars[1].v_type != VAR_UNKNOWN)
	col = (पूर्णांक)tv_get_संख्या(&argvars[1]);

    rettv->vval.v_संख्या = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    शून्य
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_संख्या = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    शून्य
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    पूर्णांक		nchar;
    पूर्णांक		nbyte = 0;
    पूर्णांक		charlen;
    पूर्णांक		skipcc = FALSE;
    पूर्णांक		len = 0;
    पूर्णांक		slen;
    पूर्णांक		error = FALSE;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	परत;

    p = tv_get_string(&argvars[0]);
    slen = (पूर्णांक)STRLEN(p);

    nchar = (पूर्णांक)tv_get_संख्या_chk(&argvars[1], &error);
    जर (!error)
    {
	जर (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    जर (error)
		परत;
	    जर (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_संख्या_as_bool_nr), skipcc);
		परत;
	    }
	}

	जर (nchar > 0)
	    पर्यंत (nchar > 0 && nbyte < slen)
	    {
		जर (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		अन्यथा
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	अन्यथा
	    nbyte = nchar;
	जर (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (पूर्णांक)tv_get_संख्या(&argvars[2]);
	    पर्यंत (charlen > 0 && nbyte + len < slen)
	    {
		पूर्णांक off = nbyte + len;

		जर (off < 0)
		    len += 1;
		अन्यथा
		{
		    जर (skipcc)
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
    जर (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    अन्यथा जर (nbyte > slen)
	nbyte = slen;
    जर (len < 0)
	len = 0;
    अन्यथा जर (nbyte + len > slen)
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
    पूर्णांक		n;
    पूर्णांक		len;
    पूर्णांक		slen;
    पूर्णांक		error = FALSE;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	परत;

    p = tv_get_string(&argvars[0]);
    slen = (पूर्णांक)STRLEN(p);

    n = (पूर्णांक)tv_get_संख्या_chk(&argvars[1], &error);
    जर (error)
	len = 0;
    अन्यथा जर (argvars[2].v_type != VAR_UNKNOWN)
	len = (पूर्णांक)tv_get_संख्या(&argvars[2]);
    अन्यथा
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    जर (n < 0)
    {
	len += n;
	n = 0;
    }
    अन्यथा जर (n > slen)
	n = slen;
    जर (len < 0)
	len = 0;
    अन्यथा जर (n + len > slen)
	len = slen - n;

    जर (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	पूर्णांक off;

	// length in characters
	साठी (off = n; off < slen && len > 0; --len)
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
    पूर्णांक		haystack_len, end_idx;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 2) == FAIL))
	परत;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_संख्या = -1;
    जर (needle == रिक्त || haystack == रिक्त)
	परत;		// type error; errmsg already given

    haystack_len = (पूर्णांक)STRLEN(haystack);
    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (पूर्णांक)tv_get_संख्या_chk(&argvars[2], रिक्त);
	जर (end_idx < 0)
	    परत;	// can never find a match
    }
    अन्यथा
	end_idx = haystack_len;

    जर (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    अन्यथा
    {
	साठी (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)म_माला((अक्षर *)rest, (अक्षर *)needle);
	    जर (rest == रिक्त || rest > haystack + end_idx)
		थांबा;
	    lastmatch = rest;
	}
    }

    जर (lastmatch == रिक्त)
	rettv->vval.v_संख्या = -1;
    अन्यथा
	rettv->vval.v_संख्या = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    शून्य
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

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
    rettv->vval.v_संख्या = -1;

    जर (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_संख्या_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    परत;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_संख्या_chk(&argvars[1], रिक्त);
    जर (str == रिक्त || idx < 0)
	परत;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	जर (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    पूर्णांक (*ptr2len)(char_u *);
    जर (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    अन्यथा
	ptr2len = mb_ptr2len;

    char_u	*p;
    पूर्णांक		len;
    पूर्णांक		utf16idx = 0;
    साठी (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	जर (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    जर (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_संख्या = len;
	    परत;
	}
	utf16idx = len;
	पूर्णांक clen = ptr2len(p);
	पूर्णांक c = (clen > 1) ? utf_ptr2char(p) : *p;
	जर (c > 0xFFFF)
	    len++;
	p += clen;
	जर (charidx)
	    idx--;
    }

    rettv->vval.v_संख्या = utf16idx;
}

/*
 * "tolower(string)" function
 */
    शून्य
f_tolower(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    शून्य
f_toupper(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

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
    पूर्णांक		inlen;
    पूर्णांक		fromlen;
    पूर्णांक		tolen;
    पूर्णांक		idx;
    char_u	*cpstr;
    पूर्णांक		cplen;
    पूर्णांक		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	परत;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
    जर (fromstr == रिक्त || tostr == रिक्त)
	    परत;		// type error; errmsg already given
    ga_init2(&ga, आकार(अक्षर), 80);

    जर (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	जर (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    परत;
	}

    // fromstr and tostr have to contain the same number of chars
    पर्यंत (*in_str != NUL)
    {
	जर (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    साठी (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		जर (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    साठी (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			जर (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    थांबा;
			}
		    }
		    जर (*p == NUL)	// tostr is shorter than fromstr
			जा error;
		    थांबा;
		}
		++idx;
	    }

	    जर (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		साठी (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		जर (idx != 0)
		    जा error;
	    }

	    (शून्य)ga_grow(&ga, cplen);
	    mch_memmove((अक्षर *)ga.ga_data + ga.ga_len, cpstr, (माप_प्रकार)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	अन्यथा
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    जर (p != रिक्त)
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
    पूर्णांक		c1;
    पूर्णांक		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 2) == FAIL)))
	परत;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    जर (head == रिक्त)
	परत;

    जर (check_for_opt_string_arg(argvars, 1) == FAIL)
	परत;

    जर (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	जर (*mask == NUL)
	    mask = रिक्त;

	जर (argvars[2].v_type != VAR_UNKNOWN)
	{
	    पूर्णांक	error = 0;

	    // leading or trailing characters to trim
	    dir = (पूर्णांक)tv_get_संख्या_chk(&argvars[2], &error);
	    जर (error)
		परत;
	    जर (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		परत;
	    }
	}
    }

    जर (dir == 0 || dir == 1)
    {
	// Trim leading characters
	पर्यंत (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    जर (mask == रिक्त)
	    {
		जर (c1 > ' ' && c1 != 0xa0)
		    थांबा;
	    }
	    अन्यथा
	    {
		साठी (p = mask; *p != NUL; MB_PTR_ADV(p))
		    जर (c1 == PTR2CHAR(p))
			थांबा;
		जर (*p == NUL)
		    थांबा;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    जर (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	साठी (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    जर (mask == रिक्त)
	    {
		जर (c1 > ' ' && c1 != 0xa0)
		    थांबा;
	    }
	    अन्यथा
	    {
		साठी (p = mask; *p != NUL; MB_PTR_ADV(p))
		    जर (c1 == PTR2CHAR(p))
			थांबा;
		जर (*p == NUL)
		    थांबा;
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
    जर (str == रिक्त)
	परत रिक्त;

    माप_प्रकार len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    जर (!decoded)
	परत रिक्त;

    char_u	*p = decoded;
    माप_प्रकार	i = 0;

    पर्यंत (i < len)
    {
	जर (str[i] == '%')
	{
	    जर (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		जर (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    अन्यथा
	    {
		पूर्णांक val = hexhex2nr(&str[i + 1]);
		जर (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		अन्यथा
		{
		    // invalid hex digits following "%"
		    साठी (पूर्णांक j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	अन्यथा
	    *p++ = str[i++];
    }

    *p = NUL;

    परत decoded;
}

/*
 * "uri_decode({str})" function
 */
    शून्य
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (check_for_string_arg(argvars, 0) == FAIL)
	परत;

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
    जर (str == रिक्त)
	परत रिक्त;

    माप_प्रकार len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    जर (encoded == रिक्त)
	परत रिक्त;

    char_u *p = encoded;

    साठी (माप_प्रकार i = 0; i < len; ++i)
    {
	char_u c = str[i];
	जर (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	अन्यथा
	{
	    प्र_लिखो((अक्षर *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    परत encoded;
}

/*
 * "uri_encode({str})" function
 */
    शून्य
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (check_for_string_arg(argvars, 0) == FAIL)
	परत;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

स्थिर अक्षर *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    स्थिर varnumber_T
tv_nr(typval_T *tvs, पूर्णांक *idxp)
{
    पूर्णांक		idx = *idxp - 1;
    varnumber_T	n = 0;
    पूर्णांक		err = FALSE;

    जर (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    अन्यथा
    {
	++*idxp;
	n = tv_get_संख्या_chk(&tvs[idx], &err);
	जर (err)
	    n = 0;
    }
    परत n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    स्थिर अक्षर *
tv_str(typval_T *tvs, पूर्णांक *idxp, char_u **tofree)
{
    पूर्णांक		    idx = *idxp - 1;
    अक्षर	    *s = रिक्त;
    स्थिर char_u   numbuf[NUMBUFLEN];

    जर (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    अन्यथा
    {
	++*idxp;
	जर (tofree != रिक्त)
	    s = (अक्षर *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	अन्यथा
	    s = (अक्षर *)tv_get_string_chk(&tvs[idx]);
    }
    परत s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    स्थिर द्विगुण
tv_float(typval_T *tvs, पूर्णांक *idxp)
{
    पूर्णांक		idx = *idxp - 1;
    द्विगुण	f = 0;

    जर (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    अन्यथा
    {
	++*idxp;
	जर (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	अन्यथा जर (tvs[idx].v_type == VAR_संख्या)
	    f = (द्विगुण)tvs[idx].vval.v_संख्या;
	अन्यथा
	    emsg(_(e_expected_float_argument_for_printf));
    }
    परत f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    स्थिर स्थिरांक अक्षर *
infinity_str(पूर्णांक positive,
	     अक्षर fmt_spec,
	     पूर्णांक force_sign,
	     पूर्णांक space_for_positive)
{
    स्थिर स्थिरांक अक्षर *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    पूर्णांक idx = positive * (1 + force_sign + force_sign * space_for_positive);

    जर (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    परत table[idx];
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
    पूर्णांक
vim_snprintf_add(अक्षर *str, माप_प्रकार str_m, स्थिरांक अक्षर *fmt, ...)
{
    बहु_सूची	ap;
    पूर्णांक		str_l;
    माप_प्रकार	len = STRLEN(str);
    माप_प्रकार	space;

    जर (str_m <= len)
	space = 0;
    अन्यथा
	space = str_m - len;
    बहु_शुरू(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    बहु_पूर्ण(ap);
    परत str_l;
}

    पूर्णांक
vim_snprintf(अक्षर *str, माप_प्रकार str_m, स्थिरांक अक्षर *fmt, ...)
{
    बहु_सूची	ap;
    पूर्णांक		str_l;

    बहु_शुरू(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    बहु_पूर्ण(ap);
    परत str_l;
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
vim_snprintf_safelen(अक्षर *str, माप_प्रकार str_m, स्थिरांक अक्षर *fmt, ...)
{
    बहु_सूची ap;
    पूर्णांक	    str_l;

    जर (str_m == 0)
	परत 0;

    बहु_शुरू(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, रिक्त);
    बहु_पूर्ण(ap);

    जर (str_l < 0)
    {
	*str = NUL;
	परत 0;
    }
    परत ((माप_प्रकार)str_l >= str_m) ? str_m - 1 : (माप_प्रकार)str_l;
}

    पूर्णांक
vim_vsnprintf(
    अक्षर	*str,
    माप_प्रकार	str_m,
    स्थिरांक अक्षर	*fmt,
    बहु_सूची	ap)
{
    परत vim_vsnprintf_typval(str, str_m, fmt, ap, रिक्त);
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
    स्थिर पूर्णांक
format_typeof(
    स्थिरांक अक्षर	*type)
{
    // allowed values: \0, h, l, L
    अक्षर    length_modifier = '\0';

    // current conversion specifier character
    अक्षर    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    जर (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	जर (length_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    length_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    निवड (fmt_spec)
    {
	स्थिती 'i': fmt_spec = 'd'; थांबा;
	स्थिती '*': fmt_spec = 'd'; length_modifier = 'h'; थांबा;
	स्थिती 'D': fmt_spec = 'd'; length_modifier = 'l'; थांबा;
	स्थिती 'U': fmt_spec = 'u'; length_modifier = 'l'; थांबा;
	स्थिती 'O': fmt_spec = 'o'; length_modifier = 'l'; थांबा;
	पूर्वनिर्धारित: थांबा;
    }

    // get parameter value, do initial processing
    निवड (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    स्थिती '%':
	परत TYPE_PERCENT;

    स्थिती 'c':
	परत TYPE_CHAR;

    स्थिती 's':
    स्थिती 'S':
	परत TYPE_STRING;

    स्थिती 'd': स्थिती 'u':
    स्थिती 'b': स्थिती 'B':
    स्थिती 'o':
    स्थिती 'x': स्थिती 'X':
    स्थिती 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //मूल्य

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    जर (fmt_spec == 'p')
		परत TYPE_POINTER;
	    अन्यथा जर (fmt_spec == 'b' || fmt_spec == 'B')
		परत TYPE_UNSIGNEDLONGLONGINT;
	    अन्यथा जर (fmt_spec == 'd')
	    {
		// signed
		निवड (length_modifier)
		{
		स्थिती '\0':
		स्थिती 'h':
		    // char and short arguments are passed as int.
		    परत TYPE_INT;
		स्थिती 'l':
		    परत TYPE_LONGINT;
		स्थिती 'L':
		    परत TYPE_LONGLONGINT;
		}
	    }
	    अन्यथा
	    {
		// unsigned
		निवड (length_modifier)
		{
		    स्थिती '\0':
		    स्थिती 'h':
			परत TYPE_UNSIGNEDINT;
		    स्थिती 'l':
			परत TYPE_UNSIGNEDLONGINT;
		    स्थिती 'L':
			परत TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	थांबा;

    स्थिती 'f':
    स्थिती 'F':
    स्थिती 'e':
    स्थिती 'E':
    स्थिती 'g':
    स्थिती 'G':
	परत TYPE_FLOAT;
    }

    परत TYPE_UNKNOWN;
}

    स्थिर अक्षर *
format_typename(
    स्थिरांक अक्षर  *type)
{
    निवड (format_typeof(type))
    {
	स्थिती TYPE_INT:
	    परत typename_int;

	स्थिती TYPE_LONGINT:
	    परत typename_longint;

	स्थिती TYPE_LONGLONGINT:
	    परत typename_longlongint;

	स्थिती TYPE_UNSIGNEDINT:
	    परत typename_unsignedint;

	स्थिती TYPE_UNSIGNEDLONGINT:
	    परत typename_unsignedlongint;

	स्थिती TYPE_UNSIGNEDLONGLONGINT:
	    परत typename_unsignedlonglongint;

	स्थिती TYPE_POINTER:
	    परत _(typename_pointer);

	स्थिती TYPE_PERCENT:
	    परत _(typename_percent);

	स्थिती TYPE_CHAR:
	    परत typename_char;

	स्थिती TYPE_STRING:
	    परत _(typename_string);

	स्थिती TYPE_FLOAT:
	    परत typename_float;
    }

    परत _(typename_unknown);
}

    स्थिर पूर्णांक
adjust_types(
    स्थिरांक अक्षर ***ap_types,
    पूर्णांक arg,
    पूर्णांक *अंक_posarg,
    स्थिरांक अक्षर *type)
{
    जर (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	परत FAIL;
    }

    जर (*ap_types == रिक्त || *अंक_posarg < arg)
    {
	पूर्णांक	    idx;
	स्थिरांक अक्षर  **new_types;

	जर (*ap_types == रिक्त)
	    new_types = ALLOC_CLEAR_MULT(स्थिरांक अक्षर *, arg);
	अन्यथा
	    new_types = vim_realloc((अक्षर **)*ap_types,
						arg * आकार(स्थिरांक अक्षर *));

	जर (new_types == रिक्त)
	    परत FAIL;

	साठी (idx = *अंक_posarg; idx < arg; ++idx)
	    new_types[idx] = रिक्त;

	*ap_types = new_types;
	*अंक_posarg = arg;
    }

    जर ((*ap_types)[arg - 1] != रिक्त)
    {
	जर ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    स्थिरांक अक्षर *pt = type;
	    जर (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    जर (pt[0] != '*')
	    {
		निवड (pt[0])
		{
		    स्थिती 'd':
		    स्थिती 'i': थांबा;
		    पूर्वनिर्धारित:
			semsg(_(e_positional_अंक_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			परत FAIL;
		}
	    }
	}
	अन्यथा
	{
	    जर (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_अंक_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		परत FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    परत OK;
}

    स्थिर शून्य
format_overflow_error(स्थिरांक अक्षर *pstart)
{
    माप_प्रकार	arglen = 0;
    अक्षर	*argcopy = रिक्त;
    स्थिरांक अक्षर	*p = pstart;

    पर्यंत (VIM_ISDIGIT((पूर्णांक)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(अक्षर, arglen + 1);
    जर (argcopy != रिक्त)
    {
	म_नकलन(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	मुक्त(argcopy);
    }
    अन्यथा
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    स्थिर पूर्णांक
get_unsigned_int(
    स्थिरांक अक्षर *pstart,
    स्थिरांक अक्षर **p,
    अचिन्हित पूर्णांक *uj,
    पूर्णांक overflow_err)
{
    *uj = **p - '0';
    ++*p;

    पर्यंत (VIM_ISDIGIT((पूर्णांक)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (अचिन्हित पूर्णांक)(**p - '0');
	++*p;
    }

    जर (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	जर (overflow_err)
	{
	    format_overflow_error(pstart);
	    परत FAIL;
	}
	अन्यथा
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    परत OK;
}


    स्थिर पूर्णांक
parse_fmt_types(
    स्थिरांक अक्षर  ***ap_types,
    पूर्णांक		*अंक_posarg,
    स्थिरांक अक्षर  *fmt,
    typval_T	*tvs UNUSED
    )
{
    स्थिरांक अक्षर	*p = fmt;
    स्थिरांक अक्षर	*arg = रिक्त;

    पूर्णांक		any_pos = 0;
    पूर्णांक		any_arg = 0;
    पूर्णांक		arg_idx;

# define CHECK_POS_ARG करा { \
    जर (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	जा error; \
    } \
} पर्यंत (0);

    जर (p == रिक्त)
	परत OK;

    पर्यंत (*p != NUL)
    {
	जर (*p != '%')
	{
	    स्थिरांक अक्षर    *q = म_अक्षर(p + 1, '%');
	    माप_प्रकार  n = (q == रिक्त) ? STRLEN(p) : (माप_प्रकार)(q - p);

	    p += n;
	}
	अन्यथा
	{
	    // allowed values: \0, h, l, L
	    अक्षर	length_modifier = '\0';

	    // variable for positional arg
	    पूर्णांक		pos_arg = -1;
	    स्थिरांक अक्षर	*ptype = रिक्त;
	    स्थिरांक अक्षर	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    पर्यंत (VIM_ISDIGIT(*ptype))
		++ptype;

	    जर (*ptype == '$')
	    {
		जर (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    जा error;
		}

		// Positional argument
		अचिन्हित पूर्णांक uj;

		जर (get_unsigned_int(pstart, &p, &uj, tvs != रिक्त) == FAIL)
		    जा error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    पर्यंत (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		निवड (*p)
		{
		    स्थिती '0': थांबा;
		    स्थिती '-': थांबा;
		    स्थिती '+': थांबा;
		    स्थिती ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      थांबा;
		    स्थिती '#': थांबा;
		    स्थिती '\'': थांबा;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    जर (*(arg = p) == '*')
	    {
		p++;

		जर (VIM_ISDIGIT((पूर्णांक)(*p)))
		{
		    // Positional argument field width
		    अचिन्हित पूर्णांक uj;

		    जर (get_unsigned_int(arg + 1, &p, &uj, tvs != रिक्त) == FAIL)
			जा error;

		    जर (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			जा error;
		    }
		    अन्यथा
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			जर (adjust_types(ap_types, uj, अंक_posarg, arg) == FAIL)
			    जा error;
		    }
		}
		अन्यथा
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    अन्यथा जर (VIM_ISDIGIT((पूर्णांक)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		स्थिरांक अक्षर *digstart = p;
		अचिन्हित पूर्णांक uj;

		जर (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
		    जा error;

		जर (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    जा error;
		}
	    }

	    // parse precision
	    जर (*p == '.')
	    {
		p++;

		जर (*(arg = p) == '*')
		{
		    p++;

		    जर (VIM_ISDIGIT((पूर्णांक)(*p)))
		    {
			// Parse precision
			अचिन्हित पूर्णांक uj;

			जर (get_unsigned_int(arg + 1, &p, &uj, tvs != रिक्त) == FAIL)
			    जा error;

			जर (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    जर (adjust_types(ap_types, uj, अंक_posarg, arg) == FAIL)
				जा error;
			}
			अन्यथा
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    जा error;
			}
		    }
		    अन्यथा
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		अन्यथा जर (VIM_ISDIGIT((पूर्णांक)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    स्थिरांक अक्षर *digstart = p;
		    अचिन्हित पूर्णांक uj;

		    जर (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
			जा error;

		    जर (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			जा error;
		    }
		}
	    }

	    जर (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    जर (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		जर (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    निवड (*p)
	    {
		// Check for known format specifiers. % is special!
		स्थिती 'i':
		स्थिती '*':
		स्थिती 'd':
		स्थिती 'u':
		स्थिती 'o':
		स्थिती 'D':
		स्थिती 'U':
		स्थिती 'O':
		स्थिती 'x':
		स्थिती 'X':
		स्थिती 'b':
		स्थिती 'B':
		स्थिती 'c':
		स्थिती 's':
		स्थिती 'S':
		स्थिती 'p':
		स्थिती 'f':
		स्थिती 'F':
		स्थिती 'e':
		स्थिती 'E':
		स्थिती 'g':
		स्थिती 'G':
		    जर (pos_arg != -1)
		    {
			जर (adjust_types(ap_types, pos_arg, अंक_posarg, ptype) == FAIL)
			    जा error;
		    }
		    अन्यथा
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    थांबा;

		पूर्वनिर्धारित:
		    जर (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			जा error;
		    }
	    }

	    जर (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    साठी (arg_idx = 0; arg_idx < *अंक_posarg; ++arg_idx)
    {
	जर ((*ap_types)[arg_idx] == रिक्त)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    जा error;
	}

# जर defined(FEAT_EVAL)
	जर (tvs != रिक्त && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    जा error;
	}
# endif
    }

    परत OK;

error:
    vim_free((अक्षर**)*ap_types);
    *ap_types = रिक्त;
    *अंक_posarg = 0;
    परत FAIL;
}

    स्थिर शून्य
skip_to_arg(
    स्थिरांक अक्षर	**ap_types,
    बहु_सूची	ap_start,
    बहु_सूची	*ap,
    पूर्णांक		*arg_idx,
    पूर्णांक		*arg_cur,
    स्थिरांक अक्षर	*fmt)
{
    पूर्णांक		arg_min = 0;

    जर (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	परत;
    }

    जर (*arg_cur >= *arg_idx)
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

    साठी (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	स्थिरांक अक्षर *p;

	जर (ap_types == रिक्त || ap_types[*arg_cur] == रिक्त)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    परत;
	}

	p = ap_types[*arg_cur];

	पूर्णांक fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	निवड (fmt_type)
	{
	स्थिती TYPE_PERCENT:
	स्थिती TYPE_UNKNOWN:
	    थांबा;

	स्थिती TYPE_CHAR:
	    बहु_तर्क(*ap, पूर्णांक);
	    थांबा;

	स्थिती TYPE_STRING:
	    बहु_तर्क(*ap, अक्षर *);
	    थांबा;

	स्थिती TYPE_POINTER:
	    बहु_तर्क(*ap, शून्य *);
	    थांबा;

	स्थिती TYPE_INT:
	    बहु_तर्क(*ap, पूर्णांक);
	    थांबा;

	स्थिती TYPE_LONGINT:
	    बहु_तर्क(*ap, दीर्घ पूर्णांक);
	    थांबा;

	स्थिती TYPE_LONGLONGINT:
	    बहु_तर्क(*ap, varnumber_T);
	    थांबा;

	स्थिती TYPE_UNSIGNEDINT:
	    बहु_तर्क(*ap, अचिन्हित पूर्णांक);
	    थांबा;

	स्थिती TYPE_UNSIGNEDLONGINT:
	    बहु_तर्क(*ap, अचिन्हित दीर्घ पूर्णांक);
	    थांबा;

	स्थिती TYPE_UNSIGNEDLONGLONGINT:
	    बहु_तर्क(*ap, uvarnumber_T);
	    थांबा;

	स्थिती TYPE_FLOAT:
	    बहु_तर्क(*ap, द्विगुण);
	    थांबा;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    परत;
}

    पूर्णांक
vim_vsnprintf_typval(
    अक्षर	*str,
    माप_प्रकार	str_m,
    स्थिरांक अक्षर	*fmt,
    बहु_सूची	ap_start,
    typval_T	*tvs)
{
    माप_प्रकार	str_l = 0;
    स्थिरांक अक्षर	*p = fmt;
    पूर्णांक		arg_cur = 0;
    पूर्णांक		अंक_posarg = 0;
    पूर्णांक		arg_idx = 1;
    बहु_सूची	ap;
    स्थिरांक अक्षर	**ap_types = रिक्त;

    जर (parse_fmt_types(&ap_types, &अंक_posarg, fmt, tvs) == FAIL)
	परत 0;

    va_copy(ap, ap_start);

    जर (p == रिक्त)
	p = "";
    पर्यंत (*p != NUL)
    {
	जर (*p != '%')
	{
	    स्थिरांक अक्षर    *q = म_अक्षर(p + 1, '%');
	    माप_प्रकार  n = (q == रिक्त) ? STRLEN(p) : (माप_प्रकार)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    जर (str_l < str_m)
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
	    पूर्णांक	    zero_padding = 0, precision_specified = 0, justify_डावा = 0;
	    पूर्णांक	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    पूर्णांक	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    अक्षर    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    अक्षर    tmp[TMP_LEN];

	    // string address in case of string argument
	    स्थिरांक अक्षर  *str_arg = रिक्त;

	    // natural field width of arg without padding and sign
	    माप_प्रकार  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    अचिन्हित अक्षर uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    माप_प्रकार  संख्या_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    माप_प्रकार  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    अक्षर    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = रिक्त;

	    // variables for positional arg
	    पूर्णांक	    pos_arg = -1;
	    स्थिरांक अक्षर	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    पर्यंत (VIM_ISDIGIT(*ptype))
		++ptype;

	    जर (*ptype == '$')
	    {
		// Positional argument
		स्थिरांक अक्षर *digstart = p;
		अचिन्हित पूर्णांक uj;

		जर (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
		    जा error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    पर्यंत (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		निवड (*p)
		{
		    स्थिती '0': zero_padding = 1; थांबा;
		    स्थिती '-': justify_डावा = 1; थांबा;
		    स्थिती '+': force_sign = 1; space_for_positive = 0; थांबा;
		    स्थिती ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      थांबा;
		    स्थिती '#': alternate_form = 1; थांबा;
		    स्थिती '\'': थांबा;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    जर (*p == '*')
	    {
		पूर्णांक j;
		स्थिरांक अक्षर *digstart = p + 1;

		p++;

		जर (VIM_ISDIGIT((पूर्णांक)(*p)))
		{
		    // Positional argument field width
		    अचिन्हित पूर्णांक uj;

		    जर (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
			जा error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# जर defined(FEAT_EVAL)
		    tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			बहु_तर्क(ap, पूर्णांक));

		जर (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    जर (tvs != रिक्त)
		    {
			format_overflow_error(digstart);
			जा error;
		    }
		    अन्यथा
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		जर (j >= 0)
		    min_field_width = j;
		अन्यथा
		{
		    min_field_width = -j;
		    justify_डावा = 1;
		}
	    }
	    अन्यथा जर (VIM_ISDIGIT((पूर्णांक)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		स्थिरांक अक्षर *digstart = p;
		अचिन्हित पूर्णांक uj;

		जर (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
		    जा error;

		min_field_width = uj;
	    }

	    // parse precision
	    जर (*p == '.')
	    {
		p++;
		precision_specified = 1;

		जर (VIM_ISDIGIT((पूर्णांक)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    स्थिरांक अक्षर *digstart = p;
		    अचिन्हित पूर्णांक uj;

		    जर (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
			जा error;

		    precision = uj;
		}
		अन्यथा जर (*p == '*')
		{
		    पूर्णांक j;
		    स्थिरांक अक्षर *digstart = p;

		    p++;

		    जर (VIM_ISDIGIT((पूर्णांक)(*p)))
		    {
			// positional argument
			अचिन्हित पूर्णांक uj;

			जर (get_unsigned_int(digstart, &p, &uj, tvs != रिक्त) == FAIL)
			    जा error;

			arg_idx = uj;

			++p;
		    }

		    j =
# जर defined(FEAT_EVAL)
			tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    बहु_तर्क(ap, पूर्णांक));

		    जर (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			जर (tvs != रिक्त)
			{
			    format_overflow_error(digstart);
			    जा error;
			}
			अन्यथा
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    जर (j >= 0)
			precision = j;
		    अन्यथा
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    जर (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		जर (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    length_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    निवड (fmt_spec)
	    {
		स्थिती 'i': fmt_spec = 'd'; थांबा;
		स्थिती 'D': fmt_spec = 'd'; length_modifier = 'l'; थांबा;
		स्थिती 'U': fmt_spec = 'u'; length_modifier = 'l'; थांबा;
		स्थिती 'O': fmt_spec = 'o'; length_modifier = 'l'; थांबा;
		पूर्वनिर्धारित: थांबा;
	    }

# जर defined(FEAT_EVAL)
	    निवड (fmt_spec)
	    {
		स्थिती 'd': स्थिती 'u': स्थिती 'o': स्थिती 'x': स्थिती 'X':
		    जर (tvs != रिक्त && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    जर (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    निवड (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    स्थिती '%':
	    स्थिती 'c':
	    स्थिती 's':
	    स्थिती 'S':
		str_arg_l = 1;
		निवड (fmt_spec)
		{
		स्थिती '%':
		    str_arg = p;
		    थांबा;

		स्थिती 'c':
		    {
			पूर्णांक j;

			j =
# जर defined(FEAT_EVAL)
			    tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				बहु_तर्क(ap, पूर्णांक));

			// standard demands unsigned char
			uchar_arg = (अचिन्हित अक्षर)j;
			str_arg = (अक्षर *)&uchar_arg;
			थांबा;
		    }

		स्थिती 's':
		स्थिती 'S':
		    str_arg =
# जर defined(FEAT_EVAL)
				tvs != रिक्त ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    बहु_तर्क(ap, अक्षर *));

		    जर (str_arg == रिक्त)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    अन्यथा जर (!precision_specified)
			str_arg_l = म_माप(str_arg);
		    // truncate string if necessary as requested by precision
		    अन्यथा जर (precision == 0)
			str_arg_l = 0;
		    अन्यथा
		    {
			// memchr on HP does not like n > 2^31  !!!
			स्थिरांक अक्षर *q = स_प्रथम(str_arg, '\0',
				  precision <= (माप_प्रकार)0x7fffffffL ? precision
						       : (माप_प्रकार)0x7fffffffL);

			str_arg_l = (q == रिक्त) ? precision
						      : (माप_प्रकार)(q - str_arg);
		    }
		    जर (fmt_spec == 'S')
		    {
			char_u	*p1;
			माप_प्रकार	i;
			पूर्णांक	cell;

			साठी (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    जर (precision_specified && i + cell > precision)
				थांबा;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			जर (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    थांबा;

		पूर्वनिर्धारित:
		    थांबा;
		}
		थांबा;

	    स्थिती 'd': स्थिती 'u':
	    स्थिती 'b': स्थिती 'B':
	    स्थिती 'o':
	    स्थिती 'x': स्थिती 'X':
	    स्थिती 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //मूल्य

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    पूर्णांक arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    पूर्णांक int_arg = 0;
		    अचिन्हित पूर्णांक uint_arg = 0;

		    // only set for length modifier l
		    दीर्घ पूर्णांक long_arg = 0;
		    अचिन्हित दीर्घ पूर्णांक ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    शून्य *ptr_arg = रिक्त;

		    जर (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# जर defined(FEAT_EVAL)
				 tvs != रिक्त ? (शून्य *)tv_str(tvs, &arg_idx,
									रिक्त) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					बहु_तर्क(ap, शून्य *));

			जर (ptr_arg != रिक्त)
			    arg_sign = 1;
		    }
		    अन्यथा जर (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# जर defined(FEAT_EVAL)
				    tvs != रिक्त ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					बहु_तर्क(ap, uvarnumber_T));

			जर (bin_arg != 0)
			    arg_sign = 1;
		    }
		    अन्यथा जर (fmt_spec == 'd')
		    {
			// signed
			निवड (length_modifier)
			{
			स्थिती '\0':
			स्थिती 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# जर defined(FEAT_EVAL)
					tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    बहु_तर्क(ap, पूर्णांक));

			    जर (int_arg > 0)
				arg_sign =  1;
			    अन्यथा जर (int_arg < 0)
				arg_sign = -1;
			    थांबा;
			स्थिती 'l':
			    long_arg =
# जर defined(FEAT_EVAL)
					tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    बहु_तर्क(ap, दीर्घ पूर्णांक));

			    जर (long_arg > 0)
				arg_sign =  1;
			    अन्यथा जर (long_arg < 0)
				arg_sign = -1;
			    थांबा;
			स्थिती 'L':
			    llong_arg =
# जर defined(FEAT_EVAL)
					tvs != रिक्त ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    बहु_तर्क(ap, varnumber_T));

			    जर (llong_arg > 0)
				arg_sign =  1;
			    अन्यथा जर (llong_arg < 0)
				arg_sign = -1;
			    थांबा;
			}
		    }
		    अन्यथा
		    {
			// unsigned
			निवड (length_modifier)
			{
			    स्थिती '\0':
			    स्थिती 'h':
				uint_arg =
# जर defined(FEAT_EVAL)
					    tvs != रिक्त ? (अचिन्हित)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						बहु_तर्क(ap, अचिन्हित पूर्णांक));

				जर (uint_arg != 0)
				    arg_sign = 1;
				थांबा;
			    स्थिती 'l':
				ulong_arg =
# जर defined(FEAT_EVAL)
					    tvs != रिक्त ? (अचिन्हित दीर्घ)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						बहु_तर्क(ap, अचिन्हित दीर्घ पूर्णांक));

				जर (ulong_arg != 0)
				    arg_sign = 1;
				थांबा;
			    स्थिती 'L':
				ullong_arg =
# जर defined(FEAT_EVAL)
					    tvs != रिक्त ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						बहु_तर्क(ap, uvarnumber_T));

				जर (ullong_arg != 0)
				    arg_sign = 1;
				थांबा;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    जर (precision_specified)
			zero_padding = 0;
		    जर (fmt_spec == 'd')
		    {
			जर (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    अन्यथा जर (alternate_form)
		    {
			जर (arg_sign != 0
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
		    जर (!precision_specified)
			precision = 1;   // default precision is 1
		    जर (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    अन्यथा
		    {
			अक्षर	f[6];
			पूर्णांक	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			जर (!length_modifier)
			    ;
			अन्यथा जर (length_modifier == 'L')
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

			जर (fmt_spec == 'p')
			    str_arg_l += प्र_लिखो(tmp + str_arg_l, f, ptr_arg);
			अन्यथा जर (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    अक्षर	    b[8 * आकार(uvarnumber_T)];
			    माप_प्रकार	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    करा
			    {
				b[आकार(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    पर्यंत (bn != 0);

			    स_नकल(tmp + str_arg_l, b + आकार(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			अन्यथा जर (fmt_spec == 'd')
			{
			    // signed
			    निवड (length_modifier)
			    {
			    स्थिती '\0': str_arg_l += प्र_लिखो(
						 tmp + str_arg_l, f,
						 int_arg);
				       थांबा;
			    स्थिती 'h': str_arg_l += प्र_लिखो(
						 tmp + str_arg_l, f,
						 (लघु)int_arg);
				      थांबा;
			    स्थिती 'l': str_arg_l += प्र_लिखो(
						tmp + str_arg_l, f, long_arg);
				      थांबा;
			    स्थिती 'L': str_arg_l += प्र_लिखो(
					       tmp + str_arg_l, f, llong_arg);
				      थांबा;
			    }
			}
			अन्यथा
			{
			    // unsigned
			    निवड (length_modifier)
			    {
			    स्थिती '\0': str_arg_l += प्र_लिखो(
						tmp + str_arg_l, f,
						uint_arg);
				       थांबा;
			    स्थिती 'h': str_arg_l += प्र_लिखो(
						tmp + str_arg_l, f,
						(अचिन्हित लघु)uint_arg);
				      थांबा;
			    स्थिती 'l': str_arg_l += प्र_लिखो(
					       tmp + str_arg_l, f, ulong_arg);
				      थांबा;
			    स्थिती 'L': str_arg_l += प्र_लिखो(
					      tmp + str_arg_l, f, ullong_arg);
				      थांबा;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			जर (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			जर (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			माप_प्रकार अंक_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			जर (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    जर (!precision_specified
					     || precision < अंक_of_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = अंक_of_digits + 1;
			    }
			}
			// zero padding to specified precision?
			जर (अंक_of_digits < precision)
			    संख्या_of_zeros_to_pad = precision - अंक_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    जर (!justify_डावा && zero_padding)
		    {
			पूर्णांक n = (पूर्णांक)(min_field_width - (str_arg_l
						    + संख्या_of_zeros_to_pad));
			जर (n > 0)
			    संख्या_of_zeros_to_pad += n;
		    }
		    थांबा;
		}

	    स्थिती 'f':
	    स्थिती 'F':
	    स्थिती 'e':
	    स्थिती 'E':
	    स्थिती 'g':
	    स्थिती 'G':
		{
		    // Floating point.
		    द्विगुण	f;
		    द्विगुण	abs_f;
		    अक्षर	format[40];
		    पूर्णांक		l;
		    पूर्णांक		remove_trailing_zeroes = FALSE;

		    f =
# जर defined(FEAT_EVAL)
			tvs != रिक्त ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    बहु_तर्क(ap, द्विगुण));

		    abs_f = f < 0 ? -f : f;

		    जर (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			जर ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			अन्यथा
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    जर ((fmt_spec == 'f' || fmt_spec == 'F') &&
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
			जर (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			अन्यथा जर (isinf(f))
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
			    जर (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    जर (precision_specified)
			    {
				माप_प्रकार max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				जर ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (माप_प्रकार)log10(abs_f);
				जर (precision > max_prec)
				    precision = max_prec;
				l += प्र_लिखो(format + l, ".%d", (पूर्णांक)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = प्र_लिखो(tmp, format, f);
			}

			जर (remove_trailing_zeroes)
			{
			    पूर्णांक i;
			    अक्षर *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    जर (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    अन्यथा
			    {
				tp = (अक्षर *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				जर (tp != रिक्त)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    जर (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    पर्यंत (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    जर (tp != रिक्त && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				पर्यंत (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			अन्यथा
			{
			    अक्षर *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (अक्षर *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    जर (tp != रिक्त && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    जर (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			संख्या_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    थांबा;
		}

	    पूर्वनिर्धारित:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_डावा = 1;
		min_field_width = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		जर (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		थांबा;
	    }

	    जर (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    जर (!justify_डावा)
	    {
		// left padding with blank or zero
		पूर्णांक pn = (पूर्णांक)(min_field_width - (str_arg_l + संख्या_of_zeros_to_pad));

		जर (pn > 0)
		{
		    जर (str_l < str_m)
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
	    जर (संख्या_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    अन्यथा
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		पूर्णांक zn = (पूर्णांक)zero_padding_insertion_ind;

		जर (zn > 0)
		{
		    जर (str_l < str_m)
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
		zn = (पूर्णांक)संख्या_of_zeros_to_pad;
		जर (zn > 0)
		{
		    जर (str_l < str_m)
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
		पूर्णांक sn = (पूर्णांक)(str_arg_l - zero_padding_insertion_ind);

		जर (sn > 0)
		{
		    जर (str_l < str_m)
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
	    जर (justify_डावा)
	    {
		// right blank padding to the field width
		पूर्णांक pn = (पूर्णांक)(min_field_width
				      - (str_arg_l + संख्या_of_zeros_to_pad));

		जर (pn > 0)
		{
		    जर (str_l < str_m)
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

    जर (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    जर (tvs != रिक्त && tvs[अंक_posarg != 0 ? अंक_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((अक्षर*)ap_types);
    बहु_पूर्ण(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    परत (पूर्णांक)str_l;
}

#endif // PROTO
