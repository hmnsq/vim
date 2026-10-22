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
#ଅନ୍ତର୍ଭାଵ "vim.h"

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
    ଯଦି (p != ଖାଲି)
	mch_memmove(p, string, len);
    ଫେରନ୍ତୁ p;
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
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    STRNCPY(p, string, len);
    p[len] = NUL;
    ଫେରନ୍ତୁ p;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(char_u *string, char_u *esc_chars)
{
    ଫେରନ୍ତୁ vim_strsave_escaped_ext(string, esc_chars, '\\', FALSE);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bsl)
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    // First count the number of backslashes required.
    // Then allocate the memory and insert them.
    length = 1;				// count the trailing NUL
    ପାଇଁ (p = string; *p; p++)
    {
	ଯଦି (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    length += l;		// count a multibyte char
	    p += l - 1;
	    ଜାରି;
	}
	ଯଦି (vim_strchr(esc_chars, *p) != ଖାଲି || (bsl && rem_backslash(p)))
	    ++length;			// count a backslash
	++length;			// count an ordinary char
    }
    escaped_string = alloc(length);
    ଯଦି (escaped_string == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    p2 = escaped_string;
    ପାଇଁ (p = string; *p; p++)
    {
	ଯଦି (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    mch_memmove(p2, p, (size_t)l);
	    p2 += l;
	    p += l - 1;		// skip multibyte char
	    ଜାରି;
	}
	ଯଦି (vim_strchr(esc_chars, *p) != ଖାଲି || (bsl && rem_backslash(p)))
	    *p2++ = cc;
	*p2++ = *p;
    }
    *p2 = NUL;
    ଫେରନ୍ତୁ escaped_string;
}

/*
 * Return TRUE when 'shell' has "csh" in the tail.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
csh_like_shell(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (strstr((ଅକ୍ଷର *)gettail(p_sh), "csh") != ଖାଲି);
}

/*
 * Return TRUE when 'shell' has "fish" in the tail.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fish_like_shell(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (strstr((ଅକ୍ଷର *)gettail(p_sh), "fish") != ଖାଲି);
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
vim_strsave_shellescape(char_u *string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_special, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_newline)
{
    unsigned	length;
    char_u	*p;
    char_u	*d;
    char_u	*escaped_string;
    size_t	l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		csh_like;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fish_like;
    char_u	*shname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		powershell;
#ifdef MSWIN
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		double_quotes;
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
    powershell = strstr((ଅକ୍ଷର *)shname, "pwsh") != ଖାଲି;
#ifdef MSWIN
    powershell = powershell || strstr((ଅକ୍ଷର *)shname, "powershell") != ଖାଲି;
    // PowerShell only accepts single quotes so override shellslash.
    double_quotes = !powershell && !p_ssl;
#endif

    // First count the number of extra bytes required.
    length = (unsigned)STRLEN(string) + 3;  // two quotes and a trailing NUL
    ପାଇଁ (p = string; *p != NUL; MB_PTR_ADV(p))
    {
#ifdef MSWIN
	ଯଦି (double_quotes)
	{
	    ଯଦି (*p == '"')
		++length;		// " -> ""
	}
	ଅନ୍ୟଥା
#endif
	ଯଦି (*p == '\'')
	{
	    ଯଦି (powershell)
		length += 2;		// ' => ''
	    ଅନ୍ୟଥା
		length += 3;		// ' => '\''
	}
	ଯଦି ((*p == '\n' && (csh_like || do_newline))
		|| (*p == '!' && (csh_like || do_special)))
	{
	    ++length;			// insert backslash
	    ଯଦି (csh_like && do_special)
		++length;		// insert backslash
	}
	ଯଦି (do_special && find_cmdline_var(p, &l) >= 0)
	{
	    ++length;			// insert backslash
	    p += l - 1;
	}
	ଯଦି (*p == '\\' && fish_like)
	    ++length;			// insert backslash
    }

    // Allocate memory for the result and fill it.
    escaped_string = alloc(length);
    ଯଦି (escaped_string != ଖାଲି)
    {
	d = escaped_string;

	// add opening quote
#ifdef MSWIN
	ଯଦି (double_quotes)
	    *d++ = '"';
	ଅନ୍ୟଥା
#endif
	    *d++ = '\'';

	ପାଇଁ (p = string; *p != NUL; )
	{
#ifdef MSWIN
	    ଯଦି (double_quotes)
	    {
		ଯଦି (*p == '"')
		{
		    *d++ = '"';
		    *d++ = '"';
		    ++p;
		    ଜାରି;
		}
	    }
	    ଅନ୍ୟଥା
#endif
	    ଯଦି (*p == '\'')
	    {
		ଯଦି (powershell)
		{
		    *d++ = '\'';
		    *d++ = '\'';
		}
		ଅନ୍ୟଥା
		{
		    *d++ = '\'';
		    *d++ = '\\';
		    *d++ = '\'';
		    *d++ = '\'';
		}
		++p;
		ଜାରି;
	    }
	    ଯଦି ((*p == '\n' && (csh_like || do_newline))
		    || (*p == '!' && (csh_like || do_special)))
	    {
		*d++ = '\\';
		ଯଦି (csh_like && do_special)
		    *d++ = '\\';
		*d++ = *p++;
		ଜାରି;
	    }
	    ଯଦି (do_special && find_cmdline_var(p, &l) >= 0)
	    {
		*d++ = '\\';		// insert backslash
		memcpy(d, p, l);	// copy the var
		d += l;
		p += l;
		ଜାରି;
	    }
	    ଯଦି (*p == '\\' && fish_like)
	    {
		*d++ = '\\';
		*d++ = *p++;
		ଜାରି;
	    }

	    MB_COPY_CHAR(p, d);
	}

	// add terminating quote and finish with a NUL
#ifdef MSWIN
	ଯଦି (double_quotes)
	    *d++ = '"';
	ଅନ୍ୟଥା
#endif
	    *d++ = '\'';
	*d = NUL;
    }

    ଫେରନ୍ତୁ escaped_string;
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
    ଫେରନ୍ତୁ p1;
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
    ଫେରନ୍ତୁ p1;
}

/*
 * ASCII lower-to-upper case translation, language independent.
 */
    ଶୂନ୍ୟ
vim_strup(
    char_u	*p)
{
    char_u  *p2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    p2 = p;
    ଯେତେବେଳେ ((c = *p2) != NUL)
	*p2++ = (c < 'a' || c > 'z') ? c : (c - 0x20);
}

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Make string "s" all upper-case and return it in allocated memory.
 * Handles multi-byte characters as well as possible.
 * Returns NULL when out of memory.
 */
    ସ୍ଥିର char_u *
strup_save(char_u *orig)
{
    char_u	*p;
    char_u	*res;

    res = p = vim_strsave(orig);

    ଯଦି (res != ଖାଲି)
	ଯେତେବେଳେ (*p != NUL)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

	    ଯଦି (enc_utf8)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c, uc;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ଯଦି (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		uc = utf_toupper(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(uc);
		ଯଦି (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ଯଦି (s == ଖାଲି)
		    {
			vim_free(res);
			ଫେରନ୍ତୁ ଖାଲି;
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
	    ଅନ୍ୟଥା ଯଦି (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ଅନ୍ୟଥା
	    {
		*p = TOUPPER_LOC(*p); // note that toupper() can be a macro
		p++;
	    }
	}

    ଫେରନ୍ତୁ res;
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

    ଯଦି (res != ଖାଲି)
	ଯେତେବେଳେ (*p != NUL)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

	    ଯଦି (enc_utf8)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c, lc;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	newl;
		char_u	*s;

		c = utf_ptr2char(p);
		l = utf_ptr2len(p);
		ଯଦି (c == 0)
		{
		    // overlong sequence, use only the first byte
		    c = *p;
		    l = 1;
		}
		lc = utf_tolower(c);

		// Reallocate string when byte count changes.  This is rare,
		// thus it's OK to do another malloc()/free().
		newl = utf_char2len(lc);
		ଯଦି (newl != l)
		{
		    s = alloc(STRLEN(res) + 1 + newl - l);
		    ଯଦି (s == ଖାଲି)
		    {
			vim_free(res);
			ଫେରନ୍ତୁ ଖାଲି;
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
	    ଅନ୍ୟଥା ଯଦି (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
		p += l;		// skip multi-byte character
	    ଅନ୍ୟଥା
	    {
		*p = TOLOWER_LOC(*p); // note that tolower() can be a macro
		p++;
	    }
	}

    ଫେରନ୍ତୁ res;
}
#endif

/*
 * delete spaces at the end of a string
 */
    ଶୂନ୍ୟ
del_trailing_spaces(char_u *ptr)
{
    char_u	*q;

    q = ptr + STRLEN(ptr);
    ଯେତେବେଳେ (--q > ptr && VIM_ISWHITE(q[0]) && q[-1] != '\\' && q[-1] != Ctrl_V)
	*q = NUL;
}

/*
 * Like strncpy(), but always terminate the result with one NUL.
 * "to" must be "len + 1" long!
 */
    ଶୂନ୍ୟ
vim_strncpy(char_u *to, char_u *from, size_t len)
{
    STRNCPY(to, from, len);
    to[len] = NUL;
}

/*
 * Like strcat(), but make sure the result fits in "tosize" bytes and is
 * always NUL terminated. "from" and "to" may overlap.
 */
    ଶୂନ୍ୟ
vim_strcat(char_u *to, char_u *from, size_t tosize)
{
    size_t tolen = STRLEN(to);
    size_t fromlen = STRLEN(from);

    ଯଦି (tolen + fromlen + 1 > tosize)
    {
	mch_memmove(to + tolen, from, tosize - tolen - 1);
	to[tosize - 1] = NUL;
    }
    ଅନ୍ୟଥା
	mch_memmove(to + tolen, from, fromlen + 1);
}

/*
 * A version of strlen() that has a maximum length.
 */
    size_t
vim_strlen_maxlen(ଅକ୍ଷର *s, size_t maxlen)
{
    size_t i;
    ପାଇଁ (i = 0; i < maxlen; ++i)
	ଯଦି (s[i] == NUL)
	    ଭାଙ୍ଗ;
    ଫେରନ୍ତୁ i;
}

#ଯଦି !defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)
/*
 * Compare two strings, ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_stricmp(ଅକ୍ଷର *s1, ଅକ୍ଷର *s2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (;;)
    {
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)TOLOWER_LOC(*s1) - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)TOLOWER_LOC(*s2);
	ଯଦି (i != 0)
	    ଫେରନ୍ତୁ i;			    // this character different
	ଯଦି (*s1 == NUL)
	    ଭାଙ୍ଗ;			    // strings match until NUL
	++s1;
	++s2;
    }
    ଫେରନ୍ତୁ 0;				    // strings match
}
#endif

#ଯଦି !defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)
/*
 * Compare two strings, for length "len", ignoring case, using current locale.
 * Doesn't work for multi-byte characters.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_strnicmp(ଅକ୍ଷର *s1, ଅକ୍ଷର *s2, size_t len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯେତେବେଳେ (len > 0)
    {
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)TOLOWER_LOC(*s1) - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)TOLOWER_LOC(*s2);
	ଯଦି (i != 0)
	    ଫେରନ୍ତୁ i;			    // this character different
	ଯଦି (*s1 == NUL)
	    ଭାଙ୍ଗ;			    // strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ଫେରନ୍ତୁ 0;				    // strings match
}
#endif

/*
 * Compare two ASCII strings, for length "len", ignoring case, ignoring locale
 * (mostly matters for turkish locale where i I might be different).
 * return 0 for match, < 0 for smaller, > 0 for bigger
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_strnicmp_asc(ଅକ୍ଷର *s1, ଅକ୍ଷର *s2, size_t len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା                i = 0;

    ଯେତେବେଳେ (len > 0)
    {
	i = TOLOWER_ASC(*s1) - TOLOWER_ASC(*s2);
	ଯଦି (i != 0)
	    ଭାଙ୍ଗ;			// this character is different
	ଯଦି (*s1 == NUL)
	    ଭାଙ୍ଗ;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ଫେରନ୍ତୁ i;
}

/*
 * Search for first occurrence of "c" in "string".
 * Version of strchr() that handles unsigned char strings with characters from
 * 128 to 255 correctly.  It also doesn't return a pointer to the NUL at the
 * end of the string.
 */
    char_u *
vim_strchr(char_u *string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		b;

    p = string;
    ଯଦି (enc_utf8 && c > 0 && c < 0x80)
	ଫେରନ୍ତୁ vim_strbyte(string, c);
    ଯଦି (enc_utf8 && c >= 0x80)
    {
	ଯେତେବେଳେ (*p != NUL)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = utfc_ptr2len(p);

	    // Avoid matching an illegal byte here.
	    ଯଦି (utf_ptr2char(p) == c && l > 1)
		ଫେରନ୍ତୁ p;
	    p += l;
	}
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (enc_dbcs != 0 && c > 255)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n2 = c & 0xff;

	c = ((unsigned)c >> 8) & 0xff;
	ଯେତେବେଳେ ((b = *p) != NUL)
	{
	    ଯଦି (b == c && p[1] == n2)
		ଫେରନ୍ତୁ p;
	    p += (*mb_ptr2len)(p);
	}
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (has_mbyte)
    {
	ଯେତେବେଳେ ((b = *p) != NUL)
	{
	    ଯଦି (b == c)
		ଫେରନ୍ତୁ p;
	    p += (*mb_ptr2len)(p);
	}
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯେତେବେଳେ ((b = *p) != NUL)
    {
	ଯଦି (b == c)
	    ଫେରନ୍ତୁ p;
	++p;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Version of strchr() that only works for bytes and handles unsigned char
 * strings with characters above 128 correctly. It also doesn't return a
 * pointer to the NUL at the end of the string.
 */
    char_u  *
vim_strbyte(char_u *string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    // strchr() converts c to char and also matches the terminating NUL.
    // Keep the byte range and NUL behavior of this function.
    ଯଦି (c <= 0 || c > 255)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ (char_u *)strchr((ଅକ୍ଷର *)string, c);
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
vim_strrchr(char_u *string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u	*retval = ଖାଲି;
    char_u	*p = string;

    ଯେତେବେଳେ (*p)
    {
	ଯଦି (*p == c)
	    retval = p;
	MB_PTR_ADV(p);
    }
    ଫେରନ୍ତୁ retval;
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
    ଯେତେବେଳେ (*s)
    {
	ଯଦି (vim_strchr(charset, *s) != ଖାଲି)
	    ଫେରନ୍ତୁ s;
	MB_PTR_ADV(s);
    }
    ଫେରନ୍ତୁ ଖାଲି;
}
# endif
#endif

/*
 * Sort an array of strings.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sort_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2);

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sort_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    ଫେରନ୍ତୁ STRCMP(*(ଅକ୍ଷର **)s1, *(ଅକ୍ଷର **)s2);
}

    ଶୂନ୍ୟ
sort_strings(
    char_u	**files,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2)
{
    qsort((ଶୂନ୍ୟ *)files, (size_t)ଗଣନା_2, ଆକାର(char_u *), sort_compare);
}

#ଯଦି defined(FEAT_QUICKFIX) || defined(FEAT_SPELL)
/*
 * Return TRUE if string "s" contains a non-ASCII character (128 or higher).
 * When "s" is NULL FALSE is returned.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_non_ascii(char_u *s)
{
    char_u	*p;

    ଯଦି (s != ଖାଲି)
	ପାଇଁ (p = s; *p != NUL; ++p)
	    ଯଦି (*p >= 128)
		ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
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
    size_t  l = str1 == ଖାଲି ? 0 : STRLEN(str1);

    dest = alloc(l + (str2 == ଖାଲି ? 0 : STRLEN(str2)) + 1L);
    ଯଦି (dest == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (str1 == ଖାଲି)
	*dest = NUL;
    ଅନ୍ୟଥା
	STRCPY(dest, str1);
    ଯଦି (str2 != ଖାଲି)
	STRCPY(dest + l, str2);
    ଫେରନ୍ତୁ dest;
}

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_RIGHTLEFT)
/*
 * Reverse text into allocated memory.
 * Returns the allocated string, NULL when out of memory.
 */
    char_u *
reverse_text(char_u *s)
{
    size_t len = STRLEN(s);
    char_u *rev = alloc(len + 1);
    ଯଦି (rev == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (size_t s_i = 0, rev_i = len; s_i < len; ++s_i)
    {
	ଯଦି (has_mbyte)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mb_len = (*mb_ptr2len)(s + s_i);
	    rev_i -= mb_len;
	    mch_memmove(rev + rev_i, s + s_i, mb_len);
	    s_i += mb_len - 1;
	}
	ଅନ୍ୟଥା
	    rev[--rev_i] = s[s_i];
    }
    rev[len] = NUL;
    ଫେରନ୍ତୁ rev;
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * Return string "str" in ' quotes, doubling ' characters.
 * If "str" is NULL an empty string is assumed.
 * If "function" is TRUE make it function('string').
 */
    char_u *
string_quote(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା function)
{
    unsigned	len;
    char_u	*p, *r, *s;

    len = (function ? 13 : 3);
    ଯଦି (str != ଖାଲି)
    {
	len += (unsigned)STRLEN(str);
	ପାଇଁ (p = str; *p != NUL; MB_PTR_ADV(p))
	    ଯଦି (*p == '\'')
		++len;
    }
    s = r = alloc(len);
    ଯଦି (r == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (function)
    {
	STRCPY(r, "function('");
	r += 10;
    }
    ଅନ୍ୟଥା
	*r++ = '\'';
    ଯଦି (str != ଖାଲି)
	ପାଇଁ (p = str; *p != NUL; )
	{
	    ଯଦି (*p == '\'')
		*r++ = '\'';
	    MB_COPY_CHAR(p, r);
	}
    *r++ = '\'';
    ଯଦି (function)
	*r++ = ')';
    *r++ = NUL;
    ଫେରନ୍ତୁ s;
}

/*
 * Count the number of times "needle" occurs in string "haystack". Case is
 * ignored if "ic" is TRUE.
 */
    long
string_ଗଣନା(char_u *haystack, char_u *needle, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ic)
{
    long	n = 0;
    char_u	*p = haystack;
    char_u	*next;

    ଯଦି (p == ଖାଲି || needle == ଖାଲି || *needle == NUL)
	ଫେରନ୍ତୁ 0;

    size_t  needlelen = STRLEN(needle);
    ଯଦି (ic)
    {
	ଯେତେବେଳେ (*p != NUL)
	{
	    ଯଦି (MB_STRNICMP(p, needle, needlelen) == 0)
	    {
		++n;
		p += needlelen;
	    }
	    ଅନ୍ୟଥା
		MB_PTR_ADV(p);
	}
    }
    ଅନ୍ୟଥା
	ଯେତେବେଳେ ((next = (char_u *)strstr((ଅକ୍ଷର *)p, (ଅକ୍ଷର *)needle)) != ଖାଲି)
	{
	    ++n;
	    p = next + needlelen;
	}

    ଫେରନ୍ତୁ n;
}

/*
 * Make a typval_T of the first character of "input" and store it in "output".
 * Return -1 on failure or v_string's length on success.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copy_first_char_to_tv(char_u *ନିବେଶ, typval_T *output)
{
    char_u	buf[MB_MAXBYTES + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (ନିବେଶ == ଖାଲି || output == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    len = has_mbyte ? mb_ptr2len(ନିବେଶ) : 1;
    STRNCPY(buf, ନିବେଶ, len);
    buf[len] = NUL;
    output->v_type = VAR_STRING;
    output->vval.v_string = vim_strnsave(buf, len);

    ଫେରନ୍ତୁ output->vval.v_string == ଖାଲି ? -1 : len;
}

/*
 * Implementation of map() and filter() for a String. Apply "expr" to every
 * character in string "str" and return the result in "rettv".
 */
    ଶୂନ୍ୟ
string_filter_map(
	char_u		*str,
	filtermap_T	filtermap,
	typval_T	*ପଦସମଷ୍ଟି,
	typval_T	*rettv)
{
    char_u	*p;
    typval_T	tv;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rem;
    typval_T	newtv;
    funccall_T	*fc;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ଚାବି, VAR_ସଂଖ୍ୟା);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ପଦସମଷ୍ଟି_get_funccal(ପଦସମଷ୍ଟି, &newtv);

    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 80);
    ପାଇଁ (p = str; *p != NUL; p += len)
    {
	len = copy_first_char_to_tv(p, &tv);
	ଯଦି (len < 0)
	    ଭାଙ୍ଗ;

	set_vim_var_nr(VV_ଚାବି, idx);
	ଯଦି (filter_map_one(&tv, ପଦସମଷ୍ଟି, filtermap, fc, &newtv, &rem) == FAIL
		|| did_emsg)
	{
	    clear_tv(&newtv);
	    clear_tv(&tv);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (filtermap == FILTERMAP_MAP || filtermap == FILTERMAP_MAPNEW)
	{
	    ଯଦି (newtv.v_type != VAR_STRING)
	    {
		clear_tv(&newtv);
		clear_tv(&tv);
		emsg(_(e_string_required));
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା
		ga_concat(&ga, newtv.vval.v_string);
	}
	ଅନ୍ୟଥା ଯଦି (filtermap == FILTERMAP_FOREACH || !rem)
	    ga_concat(&ga, tv.vval.v_string);

	clear_tv(&newtv);
	clear_tv(&tv);

	++idx;
    }
    ga_append(&ga, NUL);
    rettv->vval.v_string = ga.ga_data;
    ଯଦି (fc != ଖାଲି)
	remove_funccal();
}

/*
 * Implementation of reduce() for String "argvars[0]" using the function "expr"
 * starting with the optional initial value "argvars[2]" and return the result
 * in "rettv".
 */
    ଶୂନ୍ୟ
string_reduce(
	typval_T	*argvars,
	typval_T	*ପଦସମଷ୍ଟି,
	typval_T	*rettv)
{
    char_u	*p = tv_get_string(&argvars[0]);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    typval_T	argv[3];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_start = called_emsg;
    funccall_T	*fc;

    ଯଦି (argvars[2].v_type == VAR_UNKNOWN)
    {
	ଯଦି (*p == NUL)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_ମୂଲ୍ୟ), "String");
	    ଫେରନ୍ତୁ;
	}
	len = copy_first_char_to_tv(p, rettv);
	ଯଦି (len < 0)
	    ଫେରନ୍ତୁ;
	p += len;
    }
    ଅନ୍ୟଥା ଯଦି (check_for_string_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ;
    ଅନ୍ୟଥା
	copy_tv(&argvars[2], rettv);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ପଦସମଷ୍ଟି_get_funccal(ପଦସମଷ୍ଟି, rettv);

    ପାଇଁ ( ; *p != NUL; p += len)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	len = copy_first_char_to_tv(p, &argv[1]);
	ଯଦି (len < 0)
	{
	    *rettv = argv[0];
	    ଭାଙ୍ଗ;
	}

	r = eval_ପଦସମଷ୍ଟି_typval(ପଦସମଷ୍ଟି, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);
	clear_tv(&argv[1]);
	ଯଦି (r == FAIL || called_emsg != called_emsg_start)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (fc != ଖାଲି)
	remove_funccal();
}

/*
 * Implementation of "byteidx()" and "byteidxcomp()" functions
 */
    ସ୍ଥିର ଶୂନ୍ୟ
byteidx_common(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା comp)
{
    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି);
    ଯଦି (str == ଖାଲି || idx < 0)
	ଫେରନ୍ତୁ;

    varnumber_T	utf16idx = FALSE;
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;
	utf16idx = tv_get_bool_chk(&argvars[2], &error);
	ଯଦି (error)
	    ଫେରନ୍ତୁ;
	ଯଦି (utf16idx < 0 || utf16idx > 1)
	{
	    semsg(_(e_using_ସଂଖ୍ୟା_as_bool_nr), utf16idx);
	    ଫେରନ୍ତୁ;
	}
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*ptr2len)(char_u *);
    ଯଦି (enc_utf8 && comp)
	ptr2len = utf_ptr2len;
    ଅନ୍ୟଥା
	ptr2len = mb_ptr2len;

    char_u *t = str;
    ପାଇଁ ( ; idx > 0; idx--)
    {
	ଯଦି (*t == NUL)		// EOL reached
	    ଫେରନ୍ତୁ;
	ଯଦି (utf16idx)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clen = ptr2len(t);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = (clen > 1) ? utf_ptr2char(t) : *t;
	    ଯଦି (c > 0xFFFF)
		idx--;
	    ଯଦି (idx > 0)
		t += clen;
	}
	ଅନ୍ୟଥା ଯଦି (idx > 0)
	    t += ptr2len(t);
    }
    rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)(t - str);
}

/*
 * "byteidx()" function
 */
    ଶୂନ୍ୟ
f_byteidx(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, FALSE);
}

/*
 * "byteidxcomp()" function
 */
    ଶୂନ୍ୟ
f_byteidxcomp(typval_T *argvars, typval_T *rettv)
{
    byteidx_common(argvars, rettv, TRUE);
}

/*
 * "charidx()" function
 */
    ଶୂନ୍ୟ
f_charidx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	ଫେରନ୍ତୁ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି);
    ଯଦି (str == ଖାଲି || idx < 0)
	ଫେରନ୍ତୁ;

    varnumber_T	countcc = FALSE;
    varnumber_T	utf16idx = FALSE;
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
	    utf16idx = tv_get_bool(&argvars[3]);
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*ptr2len)(char_u *);
    ଯଦି (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    ଅନ୍ୟଥା
	ptr2len = mb_ptr2len;

    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପାଇଁ (p = str, len = 0; utf16idx ? idx >= 0 : p <= str + idx; len++)
    {
	ଯଦି (*p == NUL)
	{
	    // If the index is exactly the number of bytes or utf-16 code units
	    // in the string then return the length of the string in
	    // characters.
	    ଯଦି (utf16idx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_ସଂଖ୍ୟା = len;
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (utf16idx)
	{
	    idx--;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clen = ptr2len(p);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = (clen > 1) ? utf_ptr2char(p) : *p;
	    ଯଦି (c > 0xFFFF)
		idx--;
	}
	p += ptr2len(p);
    }

    rettv->vval.v_ସଂଖ୍ୟା = len > 0 ? len - 1 : 0;
}

/*
 * Convert the string "str", from encoding "from" to encoding "to".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
convert_string(string_T *str, char_u *from, char_u *to, string_T *ret)
{
    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ଯଦି (convert_setup(&vimconv, from, to) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    vimconv.vc_fail = TRUE;
    ଯଦି (vimconv.vc_type == CONV_NONE)
    {
	ret->string = vim_strnsave(str->string, str->length);
	ଯଦି (ret->string == ଖାଲି)
	    ret->length = 0;
	ଅନ୍ୟଥା
	    ret->length = str->length;
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)str->length;
	ret->string = string_convert(&vimconv, str->string, &len);
	ret->length = len;
    }
    convert_setup(&vimconv, ଖାଲି, ଖାଲି);

    ଫେରନ୍ତୁ (ret->string == ଖାଲି) ? FAIL : OK;
}

/*
 * Add the bytes from "str" to "blob".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
blob_from_string(char_u *str, blob_T *blob)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);
    char_u  *dest;

    ଯଦି (len == 0)
	ଫେରନ୍ତୁ;
    ଯଦି (ga_grow(&blob->bv_ga, len) == FAIL)
	ଫେରନ୍ତୁ;
    dest = (char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len;
    mch_memmove(dest, str, (size_t)len);
    // Translate newlines in the string to NUL characters
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; ++i)
	ଯଦି (dest[i] == NL)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
string_from_blob(blob_T *blob, long *start_idx, string_T *ret)
{
    long	blen = blob_len(blob);
    long	start = *start_idx;
    char_u	*src;
    char_u	*nl;
    long	ଧାଡ଼ି_len;

    ଯଦି (start >= blen)
    {
	ret->string = vim_strsave((char_u *)"");
	ret->length = 0;
	*start_idx = blen;
	ଫେରନ୍ତୁ (ret->string == ଖାଲି) ? FAIL : OK;
    }

    src = (char_u *)blob->bv_ga.ga_data + start;
    nl = (char_u *)memchr(src, NL, (size_t)(blen - start));
    ଧାଡ଼ି_len = (nl == ଖାଲି) ? (blen - start) : (long)(nl - src);

    ret->string = alloc(ଧାଡ଼ି_len + 1);
    ଯଦି (ret->string == ଖାଲି)
    {
	ret->length = 0;
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (ଧାଡ଼ି_len > 0)
	mch_memmove(ret->string, src, (size_t)ଧାଡ଼ି_len);
    ret->string[ଧାଡ଼ି_len] = NUL;
    ret->length = (size_t)ଧାଡ଼ି_len;

    // A NUL byte in the blob represents a NL in the resulting string.
    ପାଇଁ (long i = 0; i < ଧାଡ଼ି_len; i++)
	ଯଦି (ret->string[i] == NUL)
	    ret->string[i] = NL;

    *start_idx = start + ଧାଡ଼ି_len + (nl != ଖାଲି ? 1 : 0);
    ଫେରନ୍ତୁ OK;
}

/*
 * Normalize encoding name for iconv by adding hyphens.
 * For example: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
 * Returns allocated string or NULL on allocation failure.
 */
    ସ୍ଥିର char_u *
normalize_encoding_name(char_u *enc_skipped)
{
    char_u *from_encoding_raw = alloc(STRLEN(enc_skipped) + 3);
    ଯଦି (from_encoding_raw == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    char_u *s = enc_skipped;
    char_u *pe = from_encoding_raw;

    // Convert to lowercase and replace '_' with '-'
    ଯେତେବେଳେ (*s != NUL)
    {
	ଯଦି (*s == '_')
	    *pe++ = '-';
	ଅନ୍ୟଥା
	    *pe++ = TOLOWER_ASC(*s);
	++s;
    }
    *pe = NUL;

    // Add hyphen before digit: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
    char_u *p = from_encoding_raw;
    ଯଦି ((STRNCMP(p, "ucs", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-') ||
	(STRNCMP(p, "utf", 3) == 0 && VIM_ISDIGIT(p[3]) && p[3] != NUL && p[4] != '-'))
    {
	// Insert hyphen after "ucs" or "utf": "ucs2" -> "ucs-2"
	mch_memmove(p + 4, p + 3, STRLEN(p + 3) + 1);
	p[3] = '-';
    }

    ଫେରନ୍ତୁ from_encoding_raw;
}

/*
 * "blob2str()" function
 * Converts a blob to a string, ensuring valid UTF-8 encoding.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
append_converted_string_to_list(
	string_T *converted,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା validate_utf8,
	list_T *list,
	char_u *from_encoding)
{
    ଯଦି (converted->string != ଖାଲି)
    {
	// After conversion, the output is a valid UTF-8 string (NUL-terminated)
	// Split by newlines and add to list
	char_u *p = converted->string;
	char_u *end = converted->string + converted->length;
	ଯେତେବେଳେ (p < end)
	{
	    string_T	ଧାଡ଼ି;
	    char_u	*ଧାଡ଼ି_start = p;

	    ଯେତେବେଳେ (p < end && *p != NL)
		p++;

	    // Add this line to the result list
	    ଧାଡ଼ି.length = (size_t)(p - ଧାଡ଼ି_start);
	    ଧାଡ଼ି.string = vim_strnsave(ଧାଡ଼ି_start, ଧାଡ଼ି.length);
	    ଯଦି (ଧାଡ଼ି.string != ଖାଲି)
	    {
		ଯଦି (validate_utf8 && !utf_valid_string(ଧାଡ଼ି.string, ଖାଲି))
		{
		    vim_free(ଧାଡ଼ି.string);
		    semsg(_(e_str_encoding_from_failed), p_enc);
		    vim_free(converted->string);
		    ଫେରନ୍ତୁ; // Stop processing
		}
		ଯଦି (list_append_string(list, ଧାଡ଼ି.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଧାଡ଼ି.length) == FAIL)
		{
		    vim_free(ଧାଡ଼ି.string);
		    vim_free(converted->string);
		    ଫେରନ୍ତୁ; // Stop processing on append failure
		}
		vim_free(ଧାଡ଼ି.string);
	    }
	    ଅନ୍ୟଥା
	    {
		// Allocation failure: report error and stop processing
		emsg(_(e_out_of_memory));
		vim_free(converted->string);
		ଫେରନ୍ତୁ;
	    }

	    ଯଦି (*p == NL)
		p++;
	}
	vim_free(converted->string);
    }
    ଅନ୍ୟଥା
    {
	semsg(_(e_str_encoding_from_failed), from_encoding);
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
append_validated_ଧାଡ଼ି_to_list(string_T *ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା validate_utf8, list_T *list)
{
    ଯଦି (validate_utf8 && !utf_valid_string(ଧାଡ଼ି->string, ଖାଲି))
    {
	semsg(_(e_str_encoding_from_failed), p_enc);
	vim_free(ଧାଡ଼ି->string);
	ଫେରନ୍ତୁ FAIL;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = list_append_string(list, ଧାଡ଼ି->string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଧାଡ଼ି->length);
    vim_free(ଧାଡ଼ି->string);
    ଫେରନ୍ତୁ ret;
}

    ଶୂନ୍ୟ
f_blob2str(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		blen;
    long	idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		validate_utf8 = FALSE;

    ଯଦି (check_for_blob_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    blob = argvars->vval.v_blob;
    ଯଦି (blob == ଖାଲି)
	ଫେରନ୍ତୁ;
    blen = blob_len(blob);

    char_u	*from_encoding = ଖାଲି;
    char_u	*from_encoding_raw = ଖାଲି;  // Encoding name with endianness preserved for iconv
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ଯଦି (d != ଖାଲି)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ଯଦି (enc != ଖାଲି)
	    {
		char_u *enc_skipped = enc_skip(enc);
		from_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a normalized
		// version with hyphens: "ucs2be" -> "ucs-2be", "utf16le" -> "utf-16le"
		from_encoding_raw = normalize_encoding_name(enc_skipped);
		ଯଦି (from_encoding_raw == ଖାଲି)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(from_encoding);
		    ଫେରନ୍ତୁ;
		}
	    }
	}
    }

    ଯଦି (STRCMP(p_enc, "utf-8") == 0 || STRCMP(p_enc, "utf8") == 0)
	validate_utf8 = TRUE;

    ଯଦି (from_encoding != ଖାଲି && STRCMP(from_encoding, "none") == 0)
    {
	validate_utf8 = FALSE;
	VIM_CLEAR(from_encoding);
	VIM_CLEAR(from_encoding_raw);
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 encodings: convert entire blob before splitting by newlines
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା from_prop = 0;
    ଯଦି (from_encoding != ଖାଲି)
	from_prop = enc_canon_props(from_encoding);
    ଯଦି (from_encoding != ଖାଲି && (from_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	// Build a temporary buffer from the blob as a whole
	// Don't use string_from_blob() because it treats NUL as line separator
	garray_T blob_ga;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nul_size = (from_prop & ENC_4BYTE) ? 4 : 2;
	ga_init2(&blob_ga, 1, blen + nul_size);
	ଯଦି (ga_grow(&blob_ga, blen + nul_size) == OK)
	{
	    ଯଦି (blen > 0)
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
	ଯଦି (convert_setup_ext(&vimconv, from_encoding_raw, FALSE, p_enc, FALSE) == FAIL)
	{
	    ga_clear(&blob_ga);
	    semsg(_(e_str_encoding_from_failed), from_encoding);
	    ଯାଅ done;
	}
	vimconv.vc_fail = TRUE;

	// Use string_convert_ext with explicit input length
	string_T    converted;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = blen;
	converted.string =
	    string_convert_ext(&vimconv, (char_u *)blob_ga.ga_data, &len, ଖାଲି);
	converted.length = len;
	convert_setup(&vimconv, ଖାଲି, ଖାଲି);
	ga_clear(&blob_ga);
	append_converted_string_to_list(&converted, validate_utf8, rettv->vval.v_list, from_encoding);
    }
    ଅନ୍ୟଥା
    {
	// Original logic for non-UTF-16 encodings
	idx = 0;
	ଯେତେବେଳେ (idx < blen)
	{
	    string_T	str;

	    ଯଦି (string_from_blob(blob, &idx, &str) != OK)
		ଭାଙ୍ଗ;

	    ଯଦି (from_encoding != ଖାଲି)
	    {
		// from_encoding_raw is guaranteed non-NULL whenever from_encoding != NULL
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    res;
		string_T    converted;

		res = convert_string(&str, from_encoding_raw, p_enc, &converted);
		vim_free(str.string);
		ଯଦି (res != OK)
		{
		    semsg(_(e_str_encoding_from_failed), from_encoding);
		    ଯାଅ done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    ଯଦି (append_validated_ଧାଡ଼ି_to_list(&str, validate_utf8, rettv->vval.v_list) == FAIL)
		ଯାଅ done;
	}
    }

    // If the blob ends with a newline, we need to add another empty string.
    ଯଦି (blen > 0 && blob_get(blob, blen - 1) == NL)
	list_append_string(rettv->vval.v_list, (char_u *)"", 0);

done:
    vim_free(from_encoding);
    vim_free(from_encoding_raw);
}

/*
 * "str2blob()" function
 */
    ଶୂନ୍ୟ
f_str2blob(typval_T *argvars, typval_T *rettv)
{
    blob_T	*blob;
    list_T	*list;
    listitem_T	*li;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL
	    || check_for_opt_dict_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (rettv_blob_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    blob = rettv->vval.v_blob;

    list = argvars[0].vval.v_list;
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ;

    char_u	*to_encoding = ଖାଲି;
    char_u	*to_encoding_raw = ଖାଲି;  // Encoding name with endianness preserved for iconv
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[1].vval.v_dict;
	ଯଦି (d != ଖାଲି)
	{
	    char_u *enc = dict_get_string(d, "encoding", FALSE);
	    ଯଦି (enc != ଖାଲି)
	    {
		char_u *enc_skipped = enc_skip(enc);
		to_encoding = enc_canonize(enc_skipped);

		// For iconv, preserve the endianness suffix by creating a
		// normalized version with hyphens: "utf16le" -> "utf-16le"
		to_encoding_raw = normalize_encoding_name(enc_skipped);
		ଯଦି (to_encoding_raw == ଖାଲି)
		{
		    emsg(_(e_out_of_memory));
		    VIM_CLEAR(to_encoding);
		    ଫେରନ୍ତୁ;
		}
	    }
	}
    }

    // Special handling for UTF-16/UCS-2/UTF-32/UCS-4 target encodings: join the
    // list items with a newline and convert the whole string at once, so that
    // the wide-encoded newline separators and embedded NUL bytes are preserved
    // (mirrors blob2str()).  convert_string() cannot be used here because it
    // treats every Unicode encoding as utf-8, leaving the bytes unconverted.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା to_prop = 0;
    ଯଦି (to_encoding != ଖାଲି)
	to_prop = enc_canon_props(to_encoding);
    ଯଦି (to_encoding != ଖାଲି && (to_prop & (ENC_2BYTE | ENC_4BYTE | ENC_2WORD)))
    {
	garray_T	str_ga;

	ga_init2(&str_ga, 1, 256);
	FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(list, li)
	{
	    char_u *s;

	    ଯଦି (li->li_tv.v_type != VAR_STRING)
		ଜାରି;

	    s = li->li_tv.vval.v_string;

	    // Each list string item is separated by a newline in the blob
	    ଯଦି (li != list->lv_first)
		ga_append(&str_ga, NL);
	    ଯଦି (s != ଖାଲି && *s != NUL)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା slen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);

		ଯଦି (ga_grow(&str_ga, slen) == FAIL)
		{
		    ga_clear(&str_ga);
		    ଯାଅ done;
		}
		mch_memmove((char_u *)str_ga.ga_data + str_ga.ga_len, s,
								(size_t)slen);
		str_ga.ga_len += slen;
	    }
	}

	ଯଦି (str_ga.ga_len > 0)
	{
	    vimconv_T	vimconv;

	    vimconv.vc_type = CONV_NONE;
	    ଯଦି (convert_setup_ext(&vimconv, p_enc, FALSE, to_encoding_raw, FALSE)
								    == FAIL)
	    {
		ga_clear(&str_ga);
		semsg(_(e_str_encoding_to_failed), to_encoding);
		ଯାଅ done;
	    }
	    vimconv.vc_fail = TRUE;

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = str_ga.ga_len;
	    char_u	*converted = string_convert_ext(&vimconv,
				    (char_u *)str_ga.ga_data, &len, ଖାଲି);
	    convert_setup(&vimconv, ଖାଲି, ଖାଲି);
	    ga_clear(&str_ga);

	    ଯଦି (converted == ଖାଲି)
	    {
		semsg(_(e_str_encoding_to_failed), to_encoding);
		ଯାଅ done;
	    }
	    ଯଦି (len > 0 && ga_grow(&blob->bv_ga, len) == OK)
	    {
		mch_memmove((char_u *)blob->bv_ga.ga_data + blob->bv_ga.ga_len,
						    converted, (size_t)len);
		blob->bv_ga.ga_len += len;
	    }
	    vim_free(converted);
	}
	ଅନ୍ୟଥା
	    ga_clear(&str_ga);
    }
    ଅନ୍ୟଥା
    {
	FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(list, li)
	{
	    ଯଦି (li->li_tv.v_type != VAR_STRING)
		ଜାରି;

	    string_T	str = {li->li_tv.vval.v_string, 0};

	    ଯଦି (str.string == ଖାଲି)
		STR_LITERAL_SET(str, "");
	    ଅନ୍ୟଥା
		str.length = STRLEN(str.string);

	    ଯଦି (to_encoding != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    res;
		string_T    converted;

		res = convert_string(&str, p_enc, to_encoding, &converted);
		ଯଦି (res != OK)
		{
		    semsg(_(e_str_encoding_to_failed), to_encoding);
		    ଯାଅ done;
		}
		str.string = converted.string;
		str.length = converted.length;
	    }

	    ଯଦି (li != list->lv_first)
		// Each list string item is separated by a newline in the blob
		ga_append(&blob->bv_ga, NL);

	    blob_from_string(str.string, blob);

	    ଯଦି (to_encoding != ଖାଲି)
		vim_free(str.string);
	}
    }

done:
    ଯଦି (to_encoding != ଖାଲି)
	vim_free(to_encoding);
    ଯଦି (to_encoding_raw != ଖାଲି)
	vim_free(to_encoding_raw);
}

/*
 * "str2list()" function
 */
    ଶୂନ୍ୟ
f_str2list(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		utf8 = FALSE;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[1], ଖାଲି);

    p = tv_get_string(&argvars[0]);

    ଯଦି (has_mbyte || utf8)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*ptr2len)(char_u *);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*ptr2char)(char_u *);

	ଯଦି (utf8 || enc_utf8)
	{
	    ptr2len = utf_ptr2len;
	    ptr2char = utf_ptr2char;
	}
	ଅନ୍ୟଥା
	{
	    ptr2len = mb_ptr2len;
	    ptr2char = mb_ptr2char;
	}

	ପାଇଁ ( ; *p != NUL; p += (*ptr2len)(p))
	    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (*ptr2char)(p));
    }
    ଅନ୍ୟଥା
	ପାଇଁ ( ; *p != NUL; ++p)
	    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, *p);
}

/*
 * "str2nr()" function
 */
    ଶୂନ୍ୟ
f_str2nr(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		base = 10;
    char_u	*p;
    varnumber_T	n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		what = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		isneg;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	base = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[1]);
	ଯଦି (base != 2 && base != 8 && base != 10 && base != 16)
	{
	    emsg(_(e_invalid_argument));
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[2]))
	    what |= STR2NR_QUOTE;
    }

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');
    ଯଦି (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    ବାଛ (base)
    {
	ସ୍ଥିତି 2: what |= STR2NR_BIN + STR2NR_FORCE; ଭାଙ୍ଗ;
	ସ୍ଥିତି 8: what |= STR2NR_OCT + STR2NR_OOCT + STR2NR_FORCE; ଭାଙ୍ଗ;
	ସ୍ଥିତି 16: what |= STR2NR_HEX + STR2NR_FORCE; ଭାଙ୍ଗ;
    }
    vim_str2nr(p, ଖାଲି, ଖାଲି, what, &n, ଖାଲି, 0, FALSE, ଖାଲି);
    // Text after the number is silently ignored.
    ଯଦି (isneg)
	rettv->vval.v_ସଂଖ୍ୟା = -n;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = n;

}

/*
 * "strgetchar()" function
 */
    ଶୂନ୍ୟ
f_strgetchar(typval_T *argvars, typval_T *rettv)
{
    char_u	*str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		charidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		byteidx = 0;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    str = tv_get_string_chk(&argvars[0]);
    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);
    charidx = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ;

    ଯେତେବେଳେ (charidx >= 0 && byteidx < len)
    {
	ଯଦି (charidx == 0)
	{
	    rettv->vval.v_ସଂଖ୍ୟା = mb_ptr2char(str + byteidx);
	    ଭାଙ୍ଗ;
	}
	--charidx;
	byteidx += MB_CPTR2LEN(str + byteidx);
    }
}

/*
 * "stridx()" function
 */
    ଶୂନ୍ୟ
f_stridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*save_haystack;
    char_u	*pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_idx;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    needle = tv_get_string_chk(&argvars[1]);
    save_haystack = haystack = tv_get_string_buf_chk(&argvars[0], buf);
    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (needle == ଖାଲି || haystack == ଖାଲି)
	ଫେରନ୍ତୁ;		// type error; errmsg already given

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    error = FALSE;

	start_idx = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	ଯଦି (error || start_idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(haystack))
	    ଫେରନ୍ତୁ;
	ଯଦି (start_idx >= 0)
	    haystack += start_idx;
    }

    pos	= (char_u *)strstr((ଅକ୍ଷର *)haystack, (ଅକ୍ଷର *)needle);
    ଯଦି (pos != ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)(pos - save_haystack);
}

/*
 * "string()" function
 */
    ଶୂନ୍ୟ
f_string(typval_T *argvars, typval_T *rettv)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = tv2string(&argvars[0], &tofree, numbuf,
								get_copyID());
    // Make a copy if we have a value but it's not in allocated memory.
    ଯଦି (rettv->vval.v_string != ଖାଲି && tofree == ଖାଲି)
	rettv->vval.v_string = vim_strsave(rettv->vval.v_string);
}

/*
 * "strlen()" function
 */
    ଶୂନ୍ୟ
f_strlen(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && check_for_string_or_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)(STRLEN(
					      tv_get_string(&argvars[0])));
}

    ସ୍ଥିର ଶୂନ୍ୟ
strchar_common(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା skipcc)
{
    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			(*func_mb_ptr2char_adv)(char_u **pp);

    func_mb_ptr2char_adv = skipcc ? mb_ptr2char_adv : mb_cptr2char_adv;
    ଯେତେବେଳେ (*s != NUL)
    {
	func_mb_ptr2char_adv(&s);
	++len;
    }
    rettv->vval.v_ସଂଖ୍ୟା = len;
}

/*
 * "strcharlen()" function
 */
    ଶୂନ୍ୟ
f_strcharlen(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && check_for_string_or_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    strchar_common(argvars, rettv, TRUE);
}

/*
 * "strchars()" function
 */
    ଶୂନ୍ୟ
f_strchars(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		skipcc = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;
	skipcc = tv_get_bool_chk(&argvars[1], &error);
	ଯଦି (error)
	    ଫେରନ୍ତୁ;
	ଯଦି (skipcc < 0 || skipcc > 1)
	{
	    semsg(_(e_using_ସଂଖ୍ୟା_as_bool_nr), skipcc);
	    ଫେରନ୍ତୁ;
	}
    }

    strchar_common(argvars, rettv, skipcc);
}

/*
 * "strutf16len()" function
 */
    ଶୂନ୍ୟ
f_strutf16len(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_bool_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    varnumber_T countcc = FALSE;
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	countcc = tv_get_bool(&argvars[1]);

    char_u		*s = tv_get_string(&argvars[0]);
    varnumber_T		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			(*func_mb_ptr2char_adv)(char_u **pp);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ch;

    func_mb_ptr2char_adv = countcc ? mb_cptr2char_adv : mb_ptr2char_adv;
    ଯେତେବେଳେ (*s != NUL)
    {
	ch = func_mb_ptr2char_adv(&s);
	ଯଦି (ch > 0xFFFF)
	    ++len;
	++len;
    }
    rettv->vval.v_ସଂଖ୍ୟା = len;
}

/*
 * "strdisplaywidth()" function
 */
    ଶୂନ୍ୟ
f_strdisplaywidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = 0;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    s = tv_get_string(&argvars[0]);
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[1]);

    rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)(linetabsize_col(col, s) - col);
}

/*
 * "strwidth()" function
 */
    ଶୂନ୍ୟ
f_strwidth(typval_T *argvars, typval_T *rettv)
{
    char_u	*s;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    s = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)(mb_string2cells(s, -1));
}

/*
 * "strcharpart()" function
 */
    ଶୂନ୍ୟ
f_strcharpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nchar;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nbyte = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		charlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skipcc = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		slen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ଫେରନ୍ତୁ;

    p = tv_get_string(&argvars[0]);
    slen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);

    nchar = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
    ଯଦି (!error)
    {
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN
					   && argvars[3].v_type != VAR_UNKNOWN)
	{
	    skipcc = tv_get_bool_chk(&argvars[3], &error);
	    ଯଦି (error)
		ଫେରନ୍ତୁ;
	    ଯଦି (skipcc < 0 || skipcc > 1)
	    {
		semsg(_(e_using_ସଂଖ୍ୟା_as_bool_nr), skipcc);
		ଫେରନ୍ତୁ;
	    }
	}

	ଯଦି (nchar > 0)
	    ଯେତେବେଳେ (nchar > 0 && nbyte < slen)
	    {
		ଯଦି (skipcc)
		    nbyte += mb_ptr2len(p + nbyte);
		ଅନ୍ୟଥା
		    nbyte += MB_CPTR2LEN(p + nbyte);
		--nchar;
	    }
	ଅନ୍ୟଥା
	    nbyte = nchar;
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    charlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[2]);
	    ଯେତେବେଳେ (charlen > 0 && nbyte + len < slen)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = nbyte + len;

		ଯଦି (off < 0)
		    len += 1;
		ଅନ୍ୟଥା
		{
		    ଯଦି (skipcc)
			len += mb_ptr2len(p + off);
		    ଅନ୍ୟଥା
			len += MB_CPTR2LEN(p + off);
		}
		--charlen;
	    }
	}
	ଅନ୍ୟଥା
	    len = slen - nbyte;    // default: all bytes that are available.
    }

    // Only return the overlap between the specified part and the actual
    // string.
    ଯଦି (nbyte < 0)
    {
	len += nbyte;
	nbyte = 0;
    }
    ଅନ୍ୟଥା ଯଦି (nbyte > slen)
	nbyte = slen;
    ଯଦି (len < 0)
	len = 0;
    ଅନ୍ୟଥା ଯଦି (nbyte + len > slen)
	len = slen - nbyte;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + nbyte, len);
}

/*
 * "strpart()" function
 */
    ଶୂନ୍ୟ
f_strpart(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		slen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL)))
	ଫେରନ୍ତୁ;

    p = tv_get_string(&argvars[0]);
    slen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);

    n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
    ଯଦି (error)
	len = 0;
    ଅନ୍ୟଥା ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[2]);
    ଅନ୍ୟଥା
	len = slen - n;	    // default len: all bytes that are available.

    // Only return the overlap between the specified part and the actual
    // string.
    ଯଦି (n < 0)
    {
	len += n;
	n = 0;
    }
    ଅନ୍ୟଥା ଯଦି (n > slen)
	n = slen;
    ଯଦି (len < 0)
	len = 0;
    ଅନ୍ୟଥା ଯଦି (n + len > slen)
	len = slen - n;

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN && argvars[3].v_type != VAR_UNKNOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା off;

	// length in characters
	ପାଇଁ (off = n; off < slen && len > 0; --len)
	    off += mb_ptr2len(p + off);
	len = off - n;
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_strnsave(p + n, len);
}

/*
 * "strridx()" function
 */
    ଶୂନ୍ୟ
f_strridx(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*needle;
    char_u	*haystack;
    char_u	*rest;
    char_u	*lastmatch = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		haystack_len, end_idx;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    needle = tv_get_string_chk(&argvars[1]);
    haystack = tv_get_string_buf_chk(&argvars[0], buf);

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (needle == ଖାଲି || haystack == ଖାଲି)
	ଫେରନ୍ତୁ;		// type error; errmsg already given

    haystack_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(haystack);
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	// Third argument: upper limit for index
	end_idx = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], ଖାଲି);
	ଯଦି (end_idx < 0)
	    ଫେରନ୍ତୁ;	// can never find a match
    }
    ଅନ୍ୟଥା
	end_idx = haystack_len;

    ଯଦି (*needle == NUL)
    {
	// Empty string matches past the end.
	lastmatch = haystack + end_idx;
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (rest = haystack; *rest != '\0'; ++rest)
	{
	    rest = (char_u *)strstr((ଅକ୍ଷର *)rest, (ଅକ୍ଷର *)needle);
	    ଯଦି (rest == ଖାଲି || rest > haystack + end_idx)
		ଭାଙ୍ଗ;
	    lastmatch = rest;
	}
    }

    ଯଦି (lastmatch == ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)(lastmatch - haystack);
}

/*
 * "strtrans()" function
 */
    ଶୂନ୍ୟ
f_strtrans(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = transstr(tv_get_string(&argvars[0]));
}


/*
 * "utf16idx()" function
 *
 * Converts a byte or character offset in a string to the corresponding UTF-16
 * code unit offset.
 */
    ଶୂନ୍ୟ
f_utf16idx(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
	    || check_for_opt_bool_arg(argvars, 2) == FAIL
	    || (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 3) == FAIL))
	    ଫେରନ୍ତୁ;

    char_u *str = tv_get_string_chk(&argvars[0]);
    varnumber_T	idx = tv_get_ସଂଖ୍ୟା_chk(&argvars[1], ଖାଲି);
    ଯଦି (str == ଖାଲି || idx < 0)
	ଫେରନ୍ତୁ;

    varnumber_T	countcc = FALSE;
    varnumber_T	charidx = FALSE;
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	countcc = tv_get_bool(&argvars[2]);
	ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
	    charidx = tv_get_bool(&argvars[3]);
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*ptr2len)(char_u *);
    ଯଦି (enc_utf8 && countcc)
	ptr2len = utf_ptr2len;
    ଅନ୍ୟଥା
	ptr2len = mb_ptr2len;

    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		utf16idx = 0;
    ପାଇଁ (p = str, len = 0; charidx ? idx >= 0 : p <= str + idx; len++)
    {
	ଯଦି (*p == NUL)
	{
	    // If the index is exactly the number of bytes or characters in the
	    // string then return the length of the string in utf-16 code
	    // units.
	    ଯଦି (charidx ? (idx == 0) : (p == (str + idx)))
		rettv->vval.v_ସଂଖ୍ୟା = len;
	    ଫେରନ୍ତୁ;
	}
	utf16idx = len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା clen = ptr2len(p);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = (clen > 1) ? utf_ptr2char(p) : *p;
	ଯଦି (c > 0xFFFF)
	    len++;
	p += clen;
	ଯଦି (charidx)
	    idx--;
    }

    rettv->vval.v_ସଂଖ୍ୟା = utf16idx;
}

/*
 * "tolower(string)" function
 */
    ଶୂନ୍ୟ
f_tolower(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strlow_save(tv_get_string(&argvars[0]));
}

/*
 * "toupper(string)" function
 */
    ଶୂନ୍ୟ
f_toupper(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = strup_save(tv_get_string(&argvars[0]));
}

/*
 * "tr(string, fromstr, tostr)" function
 */
    ଶୂନ୍ୟ
f_tr(typval_T *argvars, typval_T *rettv)
{
    char_u	*in_str;
    char_u	*fromstr;
    char_u	*tostr;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fromlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tolen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    char_u	*cpstr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cplen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = TRUE;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    garray_T	ga;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    in_str = tv_get_string(&argvars[0]);
    fromstr = tv_get_string_buf_chk(&argvars[1], buf);
    tostr = tv_get_string_buf_chk(&argvars[2], buf2);

    // Default return value: empty string.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;
    ଯଦି (fromstr == ଖାଲି || tostr == ଖାଲି)
	    ଫେରନ୍ତୁ;		// type error; errmsg already given
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 80);

    ଯଦି (!has_mbyte)
	// not multi-byte: fromstr and tostr must be the same length
	ଯଦି (STRLEN(fromstr) != STRLEN(tostr))
	{
error:
	    semsg(_(e_invalid_argument_str), fromstr);
	    ga_clear(&ga);
	    ଫେରନ୍ତୁ;
	}

    // fromstr and tostr have to contain the same number of chars
    ଯେତେବେଳେ (*in_str != NUL)
    {
	ଯଦି (has_mbyte)
	{
	    inlen = (*mb_ptr2len)(in_str);
	    cpstr = in_str;
	    cplen = inlen;
	    idx = 0;
	    ପାଇଁ (p = fromstr; *p != NUL; p += fromlen)
	    {
		fromlen = (*mb_ptr2len)(p);
		ଯଦି (fromlen == inlen && STRNCMP(in_str, p, inlen) == 0)
		{
		    ପାଇଁ (p = tostr; *p != NUL; p += tolen)
		    {
			tolen = (*mb_ptr2len)(p);
			ଯଦି (idx-- == 0)
			{
			    cplen = tolen;
			    cpstr = p;
			    ଭାଙ୍ଗ;
			}
		    }
		    ଯଦି (*p == NUL)	// tostr is shorter than fromstr
			ଯାଅ error;
		    ଭାଙ୍ଗ;
		}
		++idx;
	    }

	    ଯଦି (first && cpstr == in_str)
	    {
		// Check that fromstr and tostr have the same number of
		// (multi-byte) characters.  Done only once when a character
		// of in_str doesn't appear in fromstr.
		first = FALSE;
		ପାଇଁ (p = tostr; *p != NUL; p += tolen)
		{
		    tolen = (*mb_ptr2len)(p);
		    --idx;
		}
		ଯଦି (idx != 0)
		    ଯାଅ error;
	    }

	    (ଶୂନ୍ୟ)ga_grow(&ga, cplen);
	    mch_memmove((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, cpstr, (size_t)cplen);
	    ga.ga_len += cplen;

	    in_str += inlen;
	}
	ଅନ୍ୟଥା
	{
	    // When not using multi-byte chars we can do it faster.
	    p = vim_strchr(fromstr, *in_str);
	    ଯଦି (p != ଖାଲି)
		ga_append(&ga, tostr[p - fromstr]);
	    ଅନ୍ୟଥା
		ga_append(&ga, *in_str);
	    ++in_str;
	}
    }

    // add a terminating NUL
    (ଶୂନ୍ୟ)ga_grow(&ga, 1);
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_data;
}

/*
 * "trim({expr})" function
 */
    ଶୂନ୍ୟ
f_trim(typval_T *argvars, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*head;
    char_u	*mask = ଖାଲି;
    char_u	*tail;
    char_u	*prev;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    head = tv_get_string_buf_chk(&argvars[0], buf1);
    ଯଦି (head == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_opt_string_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type == VAR_STRING)
    {
	mask = tv_get_string_buf_chk(&argvars[1], buf2);
	ଯଦି (*mask == NUL)
	    mask = ଖାଲି;

	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	error = 0;

	    // leading or trailing characters to trim
	    dir = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	    ଯଦି (error)
		ଫେରନ୍ତୁ;
	    ଯଦି (dir < 0 || dir > 2)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&argvars[2]));
		ଫେରନ୍ତୁ;
	    }
	}
    }

    ଯଦି (dir == 0 || dir == 1)
    {
	// Trim leading characters
	ଯେତେବେଳେ (*head != NUL)
	{
	    c1 = PTR2CHAR(head);
	    ଯଦି (mask == ଖାଲି)
	    {
		ଯଦି (c1 > ' ' && c1 != 0xa0)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା
	    {
		ପାଇଁ (p = mask; *p != NUL; MB_PTR_ADV(p))
		    ଯଦି (c1 == PTR2CHAR(p))
			ଭାଙ୍ଗ;
		ଯଦି (*p == NUL)
		    ଭାଙ୍ଗ;
	    }
	    MB_PTR_ADV(head);
	}
    }

    tail = head + STRLEN(head);
    ଯଦି (dir == 0 || dir == 2)
    {
	// Trim trailing characters
	ପାଇଁ (; tail > head; tail = prev)
	{
	    prev = tail;
	    MB_PTR_BACK(head, prev);
	    c1 = PTR2CHAR(prev);
	    ଯଦି (mask == ଖାଲି)
	    {
		ଯଦି (c1 > ' ' && c1 != 0xa0)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା
	    {
		ପାଇଁ (p = mask; *p != NUL; MB_PTR_ADV(p))
		    ଯଦି (c1 == PTR2CHAR(p))
			ଭାଙ୍ଗ;
		ଯଦି (*p == NUL)
		    ଭାଙ୍ଗ;
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
    ସ୍ଥିର char_u *
uri_decode(char_u *str)
{
    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    size_t len = STRLEN(str);

    char_u *decoded = alloc(len + 1);
    ଯଦି (!decoded)
	ଫେରନ୍ତୁ ଖାଲି;

    char_u	*p = decoded;
    size_t	i = 0;

    ଯେତେବେଳେ (i < len)
    {
	ଯଦି (str[i] == '%')
	{
	    ଯଦି (i + 2 >= len)
	    {
		// Malformed encoding
		*p++ = str[i++];
		ଯଦି (str[i] != NUL)
		    *p++ = str[i++];
	    }
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା val = hexhex2nr(&str[i + 1]);
		ଯଦି (val != -1)
		{
		    *p++ = (char_u)val;
		    i += 3;
		}
		ଅନ୍ୟଥା
		{
		    // invalid hex digits following "%"
		    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < 3; j++)
			*p++ = str[i++];
		}
	    }

	}
	ଅନ୍ୟଥା
	    *p++ = str[i++];
    }

    *p = NUL;

    ଫେରନ୍ତୁ decoded;
}

/*
 * "uri_decode({str})" function
 */
    ଶୂନ୍ୟ
f_uridecode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

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
    ସ୍ଥିର char_u *
uri_encode(char_u *str)
{
    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    size_t len = STRLEN(str);

    // Worst case: every character needs encoding => 3x size + 1 for null
    // terminator
    char_u *encoded = alloc(len * 3 + 1);
    ଯଦି (encoded == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    char_u *p = encoded;

    ପାଇଁ (size_t i = 0; i < len; ++i)
    {
	char_u c = str[i];
	ଯଦି (ASCII_ISALNUM(c) || c == '-' || c == '_' || c == '.' || c == '~')
	    *p++ = c;
	ଅନ୍ୟଥା
	{
	    sprintf((ଅକ୍ଷର *)p, "%%%02X", c);
	    p += 3;
	}
    }

    *p = NUL;

    ଫେରନ୍ତୁ encoded;
}

/*
 * "uri_encode({str})" function
 */
    ଶୂନ୍ୟ
f_uriencode(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_string = uri_encode(tv_get_string(&argvars[0]));
}

ସ୍ଥିର ଅକ୍ଷର *e_printf = N_(e_insufficient_arguments_for_printf);

/*
 * Get number argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ସ୍ଥିର varnumber_T
tv_nr(typval_T *tvs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *idxp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = *idxp - 1;
    varnumber_T	n = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err = FALSE;

    ଯଦି (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ଅନ୍ୟଥା
    {
	++*idxp;
	n = tv_get_ସଂଖ୍ୟା_chk(&tvs[idx], &err);
	ଯଦି (err)
	    n = 0;
    }
    ଫେରନ୍ତୁ n;
}

/*
 * Get string argument from "idxp" entry in "tvs".  First entry is 1.
 * If "tofree" is NULL tv_get_string_chk() is used.  Some types (e.g. List)
 * are not converted to a string.
 * If "tofree" is not NULL echo_string() is used.  All types are converted to
 * a string with the same format as ":echo".  The caller must free "*tofree".
 * Returns NULL for an error.
 */
    ସ୍ଥିର ଅକ୍ଷର *
tv_str(typval_T *tvs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *idxp, char_u **tofree)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    idx = *idxp - 1;
    ଅକ୍ଷର	    *s = ଖାଲି;
    ସ୍ଥିର char_u   numbuf[NUMBUFLEN];

    ଯଦି (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ଅନ୍ୟଥା
    {
	++*idxp;
	ଯଦି (tofree != ଖାଲି)
	    s = (ଅକ୍ଷର *)echo_string(&tvs[idx], tofree, numbuf, get_copyID());
	ଅନ୍ୟଥା
	    s = (ଅକ୍ଷର *)tv_get_string_chk(&tvs[idx]);
    }
    ଫେରନ୍ତୁ s;
}

/*
 * Get float argument from "idxp" entry in "tvs".  First entry is 1.
 */
    ସ୍ଥିର ଦ୍ୱିଗୁଣ
tv_float(typval_T *tvs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *idxp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = *idxp - 1;
    ଦ୍ୱିଗୁଣ	f = 0;

    ଯଦି (tvs[idx].v_type == VAR_UNKNOWN)
	emsg(_(e_printf));
    ଅନ୍ୟଥା
    {
	++*idxp;
	ଯଦି (tvs[idx].v_type == VAR_FLOAT)
	    f = tvs[idx].vval.v_float;
	ଅନ୍ୟଥା ଯଦି (tvs[idx].v_type == VAR_ସଂଖ୍ୟା)
	    f = (ଦ୍ୱିଗୁଣ)tvs[idx].vval.v_ସଂଖ୍ୟା;
	ଅନ୍ୟଥା
	    emsg(_(e_expected_float_argument_for_printf));
    }
    ଫେରନ୍ତୁ f;
}

#endif

/*
 * Return the representation of infinity for printf() function:
 * "-inf", "inf", "+inf", " inf", "-INF", "INF", "+INF" or " INF".
 */
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *
infinity_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା positive,
	     ଅକ୍ଷର fmt_spec,
	     ପୂର୍ଣ୍ଣସଂଖ୍ୟା force_sign,
	     ପୂର୍ଣ୍ଣସଂଖ୍ୟା space_for_positive)
{
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *table[] =
    {
	"-inf", "inf", "+inf", " inf",
	"-INF", "INF", "+INF", " INF"
    };
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = positive * (1 + force_sign + force_sign * space_for_positive);

    ଯଦି (ASCII_ISUPPER(fmt_spec))
	idx += 4;
    ଫେରନ୍ତୁ table[idx];
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_snprintf_add(ଅକ୍ଷର *str, size_t str_m, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
    va_list	ap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		str_l;
    size_t	len = STRLEN(str);
    size_t	space;

    ଯଦି (str_m <= len)
	space = 0;
    ଅନ୍ୟଥା
	space = str_m - len;
    va_start(ap, fmt);
    str_l = vim_vsnprintf(str + len, space, fmt, ap);
    va_end(ap);
    ଫେରନ୍ତୁ str_l;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_snprintf(ଅକ୍ଷର *str, size_t str_m, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
    va_list	ap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		str_l;

    va_start(ap, fmt);
    str_l = vim_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    ଫେରନ୍ତୁ str_l;
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
vim_snprintf_safelen(ଅକ୍ଷର *str, size_t str_m, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
    va_list ap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    str_l;

    ଯଦି (str_m == 0)
	ଫେରନ୍ତୁ 0;

    va_start(ap, fmt);
    str_l = vim_vsnprintf_typval(str, str_m, fmt, ap, ଖାଲି);
    va_end(ap);

    ଯଦି (str_l < 0)
    {
	*str = NUL;
	ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ ((size_t)str_l >= str_m) ? str_m - 1 : (size_t)str_l;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_vsnprintf(
    ଅକ୍ଷର	*str,
    size_t	str_m,
    ଅଚଳ ଅକ୍ଷର	*fmt,
    va_list	ap)
{
    ଫେରନ୍ତୁ vim_vsnprintf_typval(str, str_m, fmt, ap, ଖାଲି);
}

ଗଣନା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
format_typeof(
    ଅଚଳ ଅକ୍ଷର	*type)
{
    // allowed values: \0, h, l, L
    ଅକ୍ଷର    length_modifier = '\0';

    // current conversion specifier character
    ଅକ୍ଷର    fmt_spec = '\0';

    // parse 'h', 'l' and 'll' length modifiers
    ଯଦି (*type == 'h' || *type == 'l')
    {
	length_modifier = *type;
	type++;
	ଯଦି (length_modifier == 'l' && *type == 'l')
	{
	    // double l = __int64 / varnumber_T
	    length_modifier = 'L';
	    type++;
	}
    }
    fmt_spec = *type;

    // common synonyms:
    ବାଛ (fmt_spec)
    {
	ସ୍ଥିତି 'i': fmt_spec = 'd'; ଭାଙ୍ଗ;
	ସ୍ଥିତି '*': fmt_spec = 'd'; length_modifier = 'h'; ଭାଙ୍ଗ;
	ସ୍ଥିତି 'D': fmt_spec = 'd'; length_modifier = 'l'; ଭାଙ୍ଗ;
	ସ୍ଥିତି 'U': fmt_spec = 'u'; length_modifier = 'l'; ଭାଙ୍ଗ;
	ସ୍ଥିତି 'O': fmt_spec = 'o'; length_modifier = 'l'; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଭାଙ୍ଗ;
    }

    // get parameter value, do initial processing
    ବାଛ (fmt_spec)
    {
	// '%' and 'c' behave similar to 's' regarding flags and field
	// widths
    ସ୍ଥିତି '%':
	ଫେରନ୍ତୁ TYPE_PERCENT;

    ସ୍ଥିତି 'c':
	ଫେରନ୍ତୁ TYPE_CHAR;

    ସ୍ଥିତି 's':
    ସ୍ଥିତି 'S':
	ଫେରନ୍ତୁ TYPE_STRING;

    ସ୍ଥିତି 'd': ସ୍ଥିତି 'u':
    ସ୍ଥିତି 'b': ସ୍ଥିତି 'B':
    ସ୍ଥିତି 'o':
    ସ୍ଥିତି 'x': ସ୍ଥିତି 'X':
    ସ୍ଥିତି 'p':
	{
	    // NOTE: the u, b, o, x, X and p conversion specifiers
	    // imply the value is unsigned;  d implies a signed
	    //ମୂଲ୍ୟ

	    // 0 if numeric argument is zero (or if pointer is
	    // NULL for 'p'), +1 if greater than zero (or nonzero
	    // for unsigned arguments), -1 if negative (unsigned
	    // argument is never negative)

	    ଯଦି (fmt_spec == 'p')
		ଫେରନ୍ତୁ TYPE_POINTER;
	    ଅନ୍ୟଥା ଯଦି (fmt_spec == 'b' || fmt_spec == 'B')
		ଫେରନ୍ତୁ TYPE_UNSIGNEDLONGLONGINT;
	    ଅନ୍ୟଥା ଯଦି (fmt_spec == 'd')
	    {
		// signed
		ବାଛ (length_modifier)
		{
		ସ୍ଥିତି '\0':
		ସ୍ଥିତି 'h':
		    // char and short arguments are passed as int.
		    ଫେରନ୍ତୁ TYPE_INT;
		ସ୍ଥିତି 'l':
		    ଫେରନ୍ତୁ TYPE_LONGINT;
		ସ୍ଥିତି 'L':
		    ଫେରନ୍ତୁ TYPE_LONGLONGINT;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		// unsigned
		ବାଛ (length_modifier)
		{
		    ସ୍ଥିତି '\0':
		    ସ୍ଥିତି 'h':
			ଫେରନ୍ତୁ TYPE_UNSIGNEDINT;
		    ସ୍ଥିତି 'l':
			ଫେରନ୍ତୁ TYPE_UNSIGNEDLONGINT;
		    ସ୍ଥିତି 'L':
			ଫେରନ୍ତୁ TYPE_UNSIGNEDLONGLONGINT;
		}
	    }
	}
	ଭାଙ୍ଗ;

    ସ୍ଥିତି 'f':
    ସ୍ଥିତି 'F':
    ସ୍ଥିତି 'e':
    ସ୍ଥିତି 'E':
    ସ୍ଥିତି 'g':
    ସ୍ଥିତି 'G':
	ଫେରନ୍ତୁ TYPE_FLOAT;
    }

    ଫେରନ୍ତୁ TYPE_UNKNOWN;
}

    ସ୍ଥିର ଅକ୍ଷର *
format_typename(
    ଅଚଳ ଅକ୍ଷର  *type)
{
    ବାଛ (format_typeof(type))
    {
	ସ୍ଥିତି TYPE_INT:
	    ଫେରନ୍ତୁ typename_int;

	ସ୍ଥିତି TYPE_LONGINT:
	    ଫେରନ୍ତୁ typename_longint;

	ସ୍ଥିତି TYPE_LONGLONGINT:
	    ଫେରନ୍ତୁ typename_longlongint;

	ସ୍ଥିତି TYPE_UNSIGNEDINT:
	    ଫେରନ୍ତୁ typename_unsignedint;

	ସ୍ଥିତି TYPE_UNSIGNEDLONGINT:
	    ଫେରନ୍ତୁ typename_unsignedlongint;

	ସ୍ଥିତି TYPE_UNSIGNEDLONGLONGINT:
	    ଫେରନ୍ତୁ typename_unsignedlonglongint;

	ସ୍ଥିତି TYPE_POINTER:
	    ଫେରନ୍ତୁ _(typename_pointer);

	ସ୍ଥିତି TYPE_PERCENT:
	    ଫେରନ୍ତୁ _(typename_percent);

	ସ୍ଥିତି TYPE_CHAR:
	    ଫେରନ୍ତୁ typename_char;

	ସ୍ଥିତି TYPE_STRING:
	    ଫେରନ୍ତୁ _(typename_string);

	ସ୍ଥିତି TYPE_FLOAT:
	    ଫେରନ୍ତୁ typename_float;
    }

    ଫେରନ୍ତୁ _(typename_unknown);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
adjust_types(
    ଅଚଳ ଅକ୍ଷର ***ap_types,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_posarg,
    ଅଚଳ ଅକ୍ଷର *type)
{
    ଯଦି (arg <= 0)
    {
	semsg(_( e_invalid_format_specifier_str), type);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (*ap_types == ଖାଲି || *ଅଙ୍କ_posarg < arg)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;
	ଅଚଳ ଅକ୍ଷର  **new_types;

	ଯଦି (*ap_types == ଖାଲି)
	    new_types = ALLOC_CLEAR_MULT(ଅଚଳ ଅକ୍ଷର *, arg);
	ଅନ୍ୟଥା
	    new_types = vim_realloc((ଅକ୍ଷର **)*ap_types,
						arg * ଆକାର(ଅଚଳ ଅକ୍ଷର *));

	ଯଦି (new_types == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ପାଇଁ (idx = *ଅଙ୍କ_posarg; idx < arg; ++idx)
	    new_types[idx] = ଖାଲି;

	*ap_types = new_types;
	*ଅଙ୍କ_posarg = arg;
    }

    ଯଦି ((*ap_types)[arg - 1] != ଖାଲି)
    {
	ଯଦି ((*ap_types)[arg - 1][0] == '*' || type[0] == '*')
	{
	    ଅଚଳ ଅକ୍ଷର *pt = type;
	    ଯଦି (pt[0] == '*')
		pt = (*ap_types)[arg - 1];

	    ଯଦି (pt[0] != '*')
	    {
		ବାଛ (pt[0])
		{
		    ସ୍ଥିତି 'd':
		    ସ୍ଥିତି 'i': ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			semsg(_(e_positional_ଅଙ୍କ_field_spec_reused_str_str), arg, format_typename((*ap_types)[arg - 1]), format_typename(type));
			ଫେରନ୍ତୁ FAIL;
		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (format_typeof(type) != format_typeof((*ap_types)[arg - 1]))
	    {
		semsg(_( e_positional_arg_ଅଙ୍କ_type_inconsistent_str_str), arg, format_typename(type), format_typename((*ap_types)[arg - 1]));
		ଫେରନ୍ତୁ FAIL;
	    }
	}
    }

    (*ap_types)[arg - 1] = type;

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ଶୂନ୍ୟ
format_overflow_error(ଅଚଳ ଅକ୍ଷର *pstart)
{
    size_t	arglen = 0;
    ଅକ୍ଷର	*argcopy = ଖାଲି;
    ଅଚଳ ଅକ୍ଷର	*p = pstart;

    ଯେତେବେଳେ (VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(*p)))
	++p;

    arglen = p - pstart;
    argcopy = ALLOC_CLEAR_MULT(ଅକ୍ଷର, arglen + 1);
    ଯଦି (argcopy != ଖାଲି)
    {
	strncpy(argcopy, pstart, arglen);
	semsg(_( e_val_too_large), argcopy);
	ମୁକ୍ତ(argcopy);
    }
    ଅନ୍ୟଥା
	semsg(_(e_out_of_memory_allocating_nr_bytes), arglen);
}

# define MAX_ALLOWED_STRING_WIDTH 1048576    // 1 MiB

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_unsigned_int(
    ଅଚଳ ଅକ୍ଷର *pstart,
    ଅଚଳ ଅକ୍ଷର **p,
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *uj,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା overflow_err)
{
    *uj = **p - '0';
    ++*p;

    ଯେତେବେଳେ (VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(**p)) && *uj < MAX_ALLOWED_STRING_WIDTH)
    {
	*uj = 10 * *uj + (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(**p - '0');
	++*p;
    }

    ଯଦି (*uj > MAX_ALLOWED_STRING_WIDTH)
    {
	ଯଦି (overflow_err)
	{
	    format_overflow_error(pstart);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	    *uj = MAX_ALLOWED_STRING_WIDTH;
    }

    ଫେରନ୍ତୁ OK;
}


    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_fmt_types(
    ଅଚଳ ଅକ୍ଷର  ***ap_types,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_posarg,
    ଅଚଳ ଅକ୍ଷର  *fmt,
    typval_T	*tvs UNUSED
    )
{
    ଅଚଳ ଅକ୍ଷର	*p = fmt;
    ଅଚଳ ଅକ୍ଷର	*arg = ଖାଲି;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		any_pos = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		any_arg = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg_idx;

# define CHECK_POS_ARG କର { \
    ଯଦି (any_pos && any_arg) \
    { \
	semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt); \
	ଯାଅ error; \
    } \
} ଯେତେବେଳେ (0);

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (*p != '%')
	{
	    ଅଚଳ ଅକ୍ଷର    *q = strchr(p + 1, '%');
	    size_t  n = (q == ଖାଲି) ? STRLEN(p) : (size_t)(q - p);

	    p += n;
	}
	ଅନ୍ୟଥା
	{
	    // allowed values: \0, h, l, L
	    ଅକ୍ଷର	length_modifier = '\0';

	    // variable for positional arg
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pos_arg = -1;
	    ଅଚଳ ଅକ୍ଷର	*ptype = ଖାଲି;
	    ଅଚଳ ଅକ୍ଷର	*pstart = p+1;

	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    ଯେତେବେଳେ (VIM_ISDIGIT(*ptype))
		++ptype;

	    ଯଦି (*ptype == '$')
	    {
		ଯଦି (*p == '0')
		{
		    // 0 flag at the wrong place
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    ଯାଅ error;
		}

		// Positional argument
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uj;

		ଯଦି (get_unsigned_int(pstart, &p, &uj, tvs != ଖାଲି) == FAIL)
		    ଯାଅ error;

		pos_arg = uj;

		any_pos = 1;
		CHECK_POS_ARG;

		++p;
	    }

	    // parse flags
	    ଯେତେବେଳେ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		ବାଛ (*p)
		{
		    ସ୍ଥିତି '0': ଭାଙ୍ଗ;
		    ସ୍ଥିତି '-': ଭାଙ୍ଗ;
		    ସ୍ଥିତି '+': ଭାଙ୍ଗ;
		    ସ୍ଥିତି ' ': // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି '#': ଭାଙ୍ଗ;
		    ସ୍ଥିତି '\'': ଭାଙ୍ଗ;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    ଯଦି (*(arg = p) == '*')
	    {
		p++;

		ଯଦି (VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(*p)))
		{
		    // Positional argument field width
		    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uj;

		    ଯଦି (get_unsigned_int(arg + 1, &p, &uj, tvs != ଖାଲି) == FAIL)
			ଯାଅ error;

		    ଯଦି (*p != '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			ଯାଅ error;
		    }
		    ଅନ୍ୟଥା
		    {
			++p;
			any_pos = 1;
			CHECK_POS_ARG;

			ଯଦି (adjust_types(ap_types, uj, ଅଙ୍କ_posarg, arg) == FAIL)
			    ଯାଅ error;
		    }
		}
		ଅନ୍ୟଥା
		{
		    any_arg = 1;
		    CHECK_POS_ARG;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ଅଚଳ ଅକ୍ଷର *digstart = p;
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uj;

		ଯଦି (get_unsigned_int(digstart, &p, &uj, tvs != ଖାଲି) == FAIL)
		    ଯାଅ error;

		ଯଦି (*p == '$')
		{
		    semsg(_( e_invalid_format_specifier_str), fmt);
		    ଯାଅ error;
		}
	    }

	    // parse precision
	    ଯଦି (*p == '.')
	    {
		p++;

		ଯଦି (*(arg = p) == '*')
		{
		    p++;

		    ଯଦି (VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(*p)))
		    {
			// Parse precision
			unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uj;

			ଯଦି (get_unsigned_int(arg + 1, &p, &uj, tvs != ଖାଲି) == FAIL)
			    ଯାଅ error;

			ଯଦି (*p == '$')
			{
			    any_pos = 1;
			    CHECK_POS_ARG;

			    ++p;

			    ଯଦି (adjust_types(ap_types, uj, ଅଙ୍କ_posarg, arg) == FAIL)
				ଯାଅ error;
			}
			ଅନ୍ୟଥା
			{
			    semsg(_( e_invalid_format_specifier_str), fmt);
			    ଯାଅ error;
			}
		    }
		    ଅନ୍ୟଥା
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ଅଚଳ ଅକ୍ଷର *digstart = p;
		    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uj;

		    ଯଦି (get_unsigned_int(digstart, &p, &uj, tvs != ଖାଲି) == FAIL)
			ଯାଅ error;

		    ଯଦି (*p == '$')
		    {
			semsg(_( e_invalid_format_specifier_str), fmt);
			ଯାଅ error;
		    }
		}
	    }

	    ଯଦି (pos_arg != -1)
	    {
		any_pos = 1;
		CHECK_POS_ARG;

		ptype = p;
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ଯଦି (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		ଯଦି (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    // length_modifier = 'L';
		    p++;
		}
	    }

	    ବାଛ (*p)
	    {
		// Check for known format specifiers. % is special!
		ସ୍ଥିତି 'i':
		ସ୍ଥିତି '*':
		ସ୍ଥିତି 'd':
		ସ୍ଥିତି 'u':
		ସ୍ଥିତି 'o':
		ସ୍ଥିତି 'D':
		ସ୍ଥିତି 'U':
		ସ୍ଥିତି 'O':
		ସ୍ଥିତି 'x':
		ସ୍ଥିତି 'X':
		ସ୍ଥିତି 'b':
		ସ୍ଥିତି 'B':
		ସ୍ଥିତି 'c':
		ସ୍ଥିତି 's':
		ସ୍ଥିତି 'S':
		ସ୍ଥିତି 'p':
		ସ୍ଥିତି 'f':
		ସ୍ଥିତି 'F':
		ସ୍ଥିତି 'e':
		ସ୍ଥିତି 'E':
		ସ୍ଥିତି 'g':
		ସ୍ଥିତି 'G':
		    ଯଦି (pos_arg != -1)
		    {
			ଯଦି (adjust_types(ap_types, pos_arg, ଅଙ୍କ_posarg, ptype) == FAIL)
			    ଯାଅ error;
		    }
		    ଅନ୍ୟଥା
		    {
			any_arg = 1;
			CHECK_POS_ARG;
		    }
		    ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    ଯଦି (pos_arg != -1)
		    {
			semsg(_( e_cannot_mix_positional_and_non_positional_str), fmt);
			ଯାଅ error;
		    }
	    }

	    ଯଦି (*p != NUL)
		p++;     // step over the just processed conversion specifier
	}
    }

    ପାଇଁ (arg_idx = 0; arg_idx < *ଅଙ୍କ_posarg; ++arg_idx)
    {
	ଯଦି ((*ap_types)[arg_idx] == ଖାଲି)
	{
	    semsg(_(e_fmt_arg_nr_unused_str), arg_idx + 1, fmt);
	    ଯାଅ error;
	}

# ଯଦି defined(FEAT_EVAL)
	ଯଦି (tvs != ଖାଲି && tvs[arg_idx].v_type == VAR_UNKNOWN)
	{
	    semsg(_(e_positional_nr_out_of_bounds_str), arg_idx + 1, fmt);
	    ଯାଅ error;
	}
# endif
    }

    ଫେରନ୍ତୁ OK;

error:
    vim_free((ଅକ୍ଷର**)*ap_types);
    *ap_types = ଖାଲି;
    *ଅଙ୍କ_posarg = 0;
    ଫେରନ୍ତୁ FAIL;
}

    ସ୍ଥିର ଶୂନ୍ୟ
skip_to_arg(
    ଅଚଳ ଅକ୍ଷର	**ap_types,
    va_list	ap_start,
    va_list	*ap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*arg_idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*arg_cur,
    ଅଚଳ ଅକ୍ଷର	*fmt)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg_min = 0;

    ଯଦି (*arg_cur + 1 == *arg_idx)
    {
	++*arg_cur;
	++*arg_idx;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (*arg_cur >= *arg_idx)
    {
	// Reset ap to ap_start and skip arg_idx - 1 types
	va_end(*ap);
	va_copy(*ap, ap_start);
    }
    ଅନ୍ୟଥା
    {
	// Skip over any we should skip
	arg_min = *arg_cur;
    }

    ପାଇଁ (*arg_cur = arg_min; *arg_cur < *arg_idx - 1; ++*arg_cur)
    {
	ଅଚଳ ଅକ୍ଷର *p;

	ଯଦି (ap_types == ଖାଲି || ap_types[*arg_cur] == ଖାଲି)
	{
	    siemsg(e_aptypes_is_null_nr_str, *arg_cur, fmt);
	    ଫେରନ୍ତୁ;
	}

	p = ap_types[*arg_cur];

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fmt_type = format_typeof(p);

	// get parameter value, do initial processing
	ବାଛ (fmt_type)
	{
	ସ୍ଥିତି TYPE_PERCENT:
	ସ୍ଥିତି TYPE_UNKNOWN:
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି TYPE_CHAR:
	    va_arg(*ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି TYPE_STRING:
	    va_arg(*ap, ଅକ୍ଷର *);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି TYPE_POINTER:
	    va_arg(*ap, ଶୂନ୍ୟ *);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି TYPE_INT:
	    va_arg(*ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି TYPE_LONGINT:
	    va_arg(*ap, long ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି TYPE_LONGLONGINT:
	    va_arg(*ap, varnumber_T);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି TYPE_UNSIGNEDINT:
	    va_arg(*ap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି TYPE_UNSIGNEDLONGINT:
	    va_arg(*ap, unsigned long ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି TYPE_UNSIGNEDLONGLONGINT:
	    va_arg(*ap, uvarnumber_T);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି TYPE_FLOAT:
	    va_arg(*ap, ଦ୍ୱିଗୁଣ);
	    ଭାଙ୍ଗ;
	}
    }

    // Because we know that after we return from this call,
    // a va_arg() call is made, we can pre-emptively
    // increment the current argument index.
    ++*arg_cur;
    ++*arg_idx;

    ଫେରନ୍ତୁ;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_vsnprintf_typval(
    ଅକ୍ଷର	*str,
    size_t	str_m,
    ଅଚଳ ଅକ୍ଷର	*fmt,
    va_list	ap_start,
    typval_T	*tvs)
{
    size_t	str_l = 0;
    ଅଚଳ ଅକ୍ଷର	*p = fmt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg_cur = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_posarg = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg_idx = 1;
    va_list	ap;
    ଅଚଳ ଅକ୍ଷର	**ap_types = ଖାଲି;

    ଯଦି (parse_fmt_types(&ap_types, &ଅଙ୍କ_posarg, fmt, tvs) == FAIL)
	ଫେରନ୍ତୁ 0;

    va_copy(ap, ap_start);

    ଯଦି (p == ଖାଲି)
	p = "";
    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (*p != '%')
	{
	    ଅଚଳ ଅକ୍ଷର    *q = strchr(p + 1, '%');
	    size_t  n = (q == ଖାଲି) ? STRLEN(p) : (size_t)(q - p);

	    // Copy up to the next '%' or NUL without any changes.
	    ଯଦି (str_l < str_m)
	    {
		size_t avail = str_m - str_l;

		mch_memmove(str + str_l, p, n > avail ? avail : n);
	    }
	    p += n;
	    str_l += n;
	}
	ଅନ୍ୟଥା
	{
	    size_t  min_field_width = 0, precision = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    zero_padding = 0, precision_specified = 0, justify_ବାମ = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    alternate_form = 0, force_sign = 0;

	    // If both the ' ' and '+' flags appear, the ' ' flag should be
	    // ignored.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    space_for_positive = 1;

	    // allowed values: \0, h, l, L
	    ଅକ୍ଷର    length_modifier = '\0';

	    // temporary buffer for simple numeric->string conversion
# define TMP_LEN 350	// On my system 1e308 is the biggest number possible.
			// That sounds reasonable to use as the maximum
			// printable.
	    ଅକ୍ଷର    tmp[TMP_LEN];

	    // string address in case of string argument
	    ଅଚଳ ଅକ୍ଷର  *str_arg = ଖାଲି;

	    // natural field width of arg without padding and sign
	    size_t  str_arg_l;

	    // unsigned char argument value - only defined for c conversion.
	    // N.B. standard explicitly states the char argument for the c
	    // conversion is unsigned
	    unsigned ଅକ୍ଷର uchar_arg;

	    // number of zeros to be inserted for numeric conversions as
	    // required by the precision or minimal field width
	    size_t  ସଂଖ୍ୟା_of_zeros_to_pad = 0;

	    // index into tmp where zero padding is to be inserted
	    size_t  zero_padding_insertion_ind = 0;

	    // current conversion specifier character
	    ଅକ୍ଷର    fmt_spec = '\0';

	    // buffer for 's' and 'S' specs
	    char_u  *tofree = ଖାଲି;

	    // variables for positional arg
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    pos_arg = -1;
	    ଅଚଳ ଅକ୍ଷର	*ptype;


	    p++;  // skip '%'

	    // First check to see if we find a positional
	    // argument specifier
	    ptype = p;

	    ଯେତେବେଳେ (VIM_ISDIGIT(*ptype))
		++ptype;

	    ଯଦି (*ptype == '$')
	    {
		// Positional argument
		ଅଚଳ ଅକ୍ଷର *digstart = p;
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uj;

		ଯଦି (get_unsigned_int(digstart, &p, &uj, tvs != ଖାଲି) == FAIL)
		    ଯାଅ error;

		pos_arg = uj;

		++p;
	    }

	    // parse flags
	    ଯେତେବେଳେ (*p == '0' || *p == '-' || *p == '+' || *p == ' '
						   || *p == '#' || *p == '\'')
	    {
		ବାଛ (*p)
		{
		    ସ୍ଥିତି '0': zero_padding = 1; ଭାଙ୍ଗ;
		    ସ୍ଥିତି '-': justify_ବାମ = 1; ଭାଙ୍ଗ;
		    ସ୍ଥିତି '+': force_sign = 1; space_for_positive = 0; ଭାଙ୍ଗ;
		    ସ୍ଥିତି ' ': force_sign = 1;
			      // If both the ' ' and '+' flags appear, the ' '
			      // flag should be ignored
			      ଭାଙ୍ଗ;
		    ସ୍ଥିତି '#': alternate_form = 1; ଭାଙ୍ଗ;
		    ସ୍ଥିତି '\'': ଭାଙ୍ଗ;
		}
		p++;
	    }
	    // If the '0' and '-' flags both appear, the '0' flag should be
	    // ignored.

	    // parse field width
	    ଯଦି (*p == '*')
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;
		ଅଚଳ ଅକ୍ଷର *digstart = p + 1;

		p++;

		ଯଦି (VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(*p)))
		{
		    // Positional argument field width
		    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uj;

		    ଯଦି (get_unsigned_int(digstart, &p, &uj, tvs != ଖାଲି) == FAIL)
			ଯାଅ error;

		    arg_idx = uj;

		    ++p;
		}

		j =
# ଯଦି defined(FEAT_EVAL)
		    tvs != ଖାଲି ? tv_nr(tvs, &arg_idx) :
# endif
			(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
				     &arg_cur, fmt),
			va_arg(ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

		ଯଦି (j > MAX_ALLOWED_STRING_WIDTH)
		{
		    ଯଦି (tvs != ଖାଲି)
		    {
			format_overflow_error(digstart);
			ଯାଅ error;
		    }
		    ଅନ୍ୟଥା
			j = MAX_ALLOWED_STRING_WIDTH;
		}

		ଯଦି (j >= 0)
		    min_field_width = j;
		ଅନ୍ୟଥା
		{
		    min_field_width = -j;
		    justify_ବାମ = 1;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(*p)))
	    {
		// size_t could be wider than unsigned int; make sure we treat
		// argument like common implementations do
		ଅଚଳ ଅକ୍ଷର *digstart = p;
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uj;

		ଯଦି (get_unsigned_int(digstart, &p, &uj, tvs != ଖାଲି) == FAIL)
		    ଯାଅ error;

		min_field_width = uj;
	    }

	    // parse precision
	    ଯଦି (*p == '.')
	    {
		p++;
		precision_specified = 1;

		ଯଦି (VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(*p)))
		{
		    // size_t could be wider than unsigned int; make sure we
		    // treat argument like common implementations do
		    ଅଚଳ ଅକ୍ଷର *digstart = p;
		    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uj;

		    ଯଦି (get_unsigned_int(digstart, &p, &uj, tvs != ଖାଲି) == FAIL)
			ଯାଅ error;

		    precision = uj;
		}
		ଅନ୍ୟଥା ଯଦି (*p == '*')
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;
		    ଅଚଳ ଅକ୍ଷର *digstart = p;

		    p++;

		    ଯଦି (VIM_ISDIGIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(*p)))
		    {
			// positional argument
			unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uj;

			ଯଦି (get_unsigned_int(digstart, &p, &uj, tvs != ଖାଲି) == FAIL)
			    ଯାଅ error;

			arg_idx = uj;

			++p;
		    }

		    j =
# ଯଦି defined(FEAT_EVAL)
			tvs != ଖାଲି ? tv_nr(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

		    ଯଦି (j > MAX_ALLOWED_STRING_WIDTH)
		    {
			ଯଦି (tvs != ଖାଲି)
			{
			    format_overflow_error(digstart);
			    ଯାଅ error;
			}
			ଅନ୍ୟଥା
			    j = MAX_ALLOWED_STRING_WIDTH;
		    }

		    ଯଦି (j >= 0)
			precision = j;
		    ଅନ୍ୟଥା
		    {
			precision_specified = 0;
			precision = 0;
		    }
		}
	    }

	    // parse 'h', 'l' and 'll' length modifiers
	    ଯଦି (*p == 'h' || *p == 'l')
	    {
		length_modifier = *p;
		p++;
		ଯଦି (length_modifier == 'l' && *p == 'l')
		{
		    // double l = __int64 / varnumber_T
		    length_modifier = 'L';
		    p++;
		}
	    }
	    fmt_spec = *p;

	    // common synonyms:
	    ବାଛ (fmt_spec)
	    {
		ସ୍ଥିତି 'i': fmt_spec = 'd'; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'D': fmt_spec = 'd'; length_modifier = 'l'; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'U': fmt_spec = 'u'; length_modifier = 'l'; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'O': fmt_spec = 'o'; length_modifier = 'l'; ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଭାଙ୍ଗ;
	    }

# ଯଦି defined(FEAT_EVAL)
	    ବାଛ (fmt_spec)
	    {
		ସ୍ଥିତି 'd': ସ୍ଥିତି 'u': ସ୍ଥିତି 'o': ସ୍ଥିତି 'x': ସ୍ଥିତି 'X':
		    ଯଦି (tvs != ଖାଲି && length_modifier == '\0')
			length_modifier = 'L';
	    }
# endif

	    ଯଦି (pos_arg != -1)
		arg_idx = pos_arg;

	    // get parameter value, do initial processing
	    ବାଛ (fmt_spec)
	    {
		// '%' and 'c' behave similar to 's' regarding flags and field
		// widths
	    ସ୍ଥିତି '%':
	    ସ୍ଥିତି 'c':
	    ସ୍ଥିତି 's':
	    ସ୍ଥିତି 'S':
		str_arg_l = 1;
		ବାଛ (fmt_spec)
		{
		ସ୍ଥିତି '%':
		    str_arg = p;
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି 'c':
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;

			j =
# ଯଦି defined(FEAT_EVAL)
			    tvs != ଖାଲି ? tv_nr(tvs, &arg_idx) :
# endif
				(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					     &arg_cur, fmt),
				va_arg(ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

			// standard demands unsigned char
			uchar_arg = (unsigned ଅକ୍ଷର)j;
			str_arg = (ଅକ୍ଷର *)&uchar_arg;
			ଭାଙ୍ଗ;
		    }

		ସ୍ଥିତି 's':
		ସ୍ଥିତି 'S':
		    str_arg =
# ଯଦି defined(FEAT_EVAL)
				tvs != ଖାଲି ? tv_str(tvs, &arg_idx, &tofree) :
# endif
				    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						 &arg_cur, fmt),
				    va_arg(ap, ଅକ୍ଷର *));

		    ଯଦି (str_arg == ଖାଲି)
		    {
			str_arg = "[NULL]";
			str_arg_l = 6;
		    }
		    // make sure not to address string beyond the specified
		    // precision !!!
		    ଅନ୍ୟଥା ଯଦି (!precision_specified)
			str_arg_l = strlen(str_arg);
		    // truncate string if necessary as requested by precision
		    ଅନ୍ୟଥା ଯଦି (precision == 0)
			str_arg_l = 0;
		    ଅନ୍ୟଥା
		    {
			// memchr on HP does not like n > 2^31  !!!
			ଅଚଳ ଅକ୍ଷର *q = memchr(str_arg, '\0',
				  precision <= (size_t)0x7fffffffL ? precision
						       : (size_t)0x7fffffffL);

			str_arg_l = (q == ଖାଲି) ? precision
						      : (size_t)(q - str_arg);
		    }
		    ଯଦି (fmt_spec == 'S')
		    {
			char_u	*p1;
			size_t	i;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cell;

			ପାଇଁ (i = 0, p1 = (char_u *)str_arg; *p1;
							  p1 += mb_ptr2len(p1))
			{
			    cell = mb_ptr2cells(p1);
			    ଯଦି (precision_specified && i + cell > precision)
				ଭାଙ୍ଗ;
			    i += cell;
			}

			str_arg_l = p1 - (char_u *)str_arg;
			ଯଦି (min_field_width != 0)
			    min_field_width += str_arg_l - i;
		    }
		    ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    ଭାଙ୍ଗ;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି 'd': ସ୍ଥିତି 'u':
	    ସ୍ଥିତି 'b': ସ୍ଥିତି 'B':
	    ସ୍ଥିତି 'o':
	    ସ୍ଥିତି 'x': ସ୍ଥିତି 'X':
	    ସ୍ଥିତି 'p':
		{
		    // NOTE: the u, b, o, x, X and p conversion specifiers
		    // imply the value is unsigned;  d implies a signed
		    //ମୂଲ୍ୟ

		    // 0 if numeric argument is zero (or if pointer is
		    // NULL for 'p'), +1 if greater than zero (or nonzero
		    // for unsigned arguments), -1 if negative (unsigned
		    // argument is never negative)
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_sign = 0;

		    // only set for length modifier h, or for no length
		    // modifiers
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା int_arg = 0;
		    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uint_arg = 0;

		    // only set for length modifier l
		    long ପୂର୍ଣ୍ଣସଂଖ୍ୟା long_arg = 0;
		    unsigned long ପୂର୍ଣ୍ଣସଂଖ୍ୟା ulong_arg = 0;

		    // only set for length modifier ll
		    varnumber_T llong_arg = 0;
		    uvarnumber_T ullong_arg = 0;

		    // only set for b conversion
		    uvarnumber_T bin_arg = 0;

		    // pointer argument value -only defined for p
		    // conversion
		    ଶୂନ୍ୟ *ptr_arg = ଖାଲି;

		    ଯଦି (fmt_spec == 'p')
		    {
			length_modifier = '\0';
			ptr_arg =
# ଯଦି defined(FEAT_EVAL)
				 tvs != ଖାଲି ? (ଶୂନ୍ୟ *)tv_str(tvs, &arg_idx,
									ଖାଲି) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, ଶୂନ୍ୟ *));

			ଯଦି (ptr_arg != ଖାଲି)
			    arg_sign = 1;
		    }
		    ଅନ୍ୟଥା ଯଦି (fmt_spec == 'b' || fmt_spec == 'B')
		    {
			bin_arg =
# ଯଦି defined(FEAT_EVAL)
				    tvs != ଖାଲି ?
					   (uvarnumber_T)tv_nr(tvs, &arg_idx) :
# endif
					(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
						     &arg_cur, fmt),
					va_arg(ap, uvarnumber_T));

			ଯଦି (bin_arg != 0)
			    arg_sign = 1;
		    }
		    ଅନ୍ୟଥା ଯଦି (fmt_spec == 'd')
		    {
			// signed
			ବାଛ (length_modifier)
			{
			ସ୍ଥିତି '\0':
			ସ୍ଥିତି 'h':
			    // char and short arguments are passed as int.
			    int_arg =
# ଯଦି defined(FEAT_EVAL)
					tvs != ଖାଲି ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

			    ଯଦି (int_arg > 0)
				arg_sign =  1;
			    ଅନ୍ୟଥା ଯଦି (int_arg < 0)
				arg_sign = -1;
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି 'l':
			    long_arg =
# ଯଦି defined(FEAT_EVAL)
					tvs != ଖାଲି ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, long ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

			    ଯଦି (long_arg > 0)
				arg_sign =  1;
			    ଅନ୍ୟଥା ଯଦି (long_arg < 0)
				arg_sign = -1;
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି 'L':
			    llong_arg =
# ଯଦି defined(FEAT_EVAL)
					tvs != ଖାଲି ? tv_nr(tvs, &arg_idx) :
# endif
					    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							 &arg_cur, fmt),
					    va_arg(ap, varnumber_T));

			    ଯଦି (llong_arg > 0)
				arg_sign =  1;
			    ଅନ୍ୟଥା ଯଦି (llong_arg < 0)
				arg_sign = -1;
			    ଭାଙ୍ଗ;
			}
		    }
		    ଅନ୍ୟଥା
		    {
			// unsigned
			ବାଛ (length_modifier)
			{
			    ସ୍ଥିତି '\0':
			    ସ୍ଥିତି 'h':
				uint_arg =
# ଯଦି defined(FEAT_EVAL)
					    tvs != ଖାଲି ? (unsigned)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

				ଯଦି (uint_arg != 0)
				    arg_sign = 1;
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'l':
				ulong_arg =
# ଯଦି defined(FEAT_EVAL)
					    tvs != ଖାଲି ? (unsigned long)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, unsigned long ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

				ଯଦି (ulong_arg != 0)
				    arg_sign = 1;
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'L':
				ullong_arg =
# ଯଦି defined(FEAT_EVAL)
					    tvs != ଖାଲି ? (uvarnumber_T)
							tv_nr(tvs, &arg_idx) :
# endif
						(skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
							     &arg_cur, fmt),
						va_arg(ap, uvarnumber_T));

				ଯଦି (ullong_arg != 0)
				    arg_sign = 1;
				ଭାଙ୍ଗ;
			}
		    }

		    str_arg = tmp;
		    str_arg_l = 0;

		    // NOTE:
		    //   For d, i, u, o, x, and X conversions, if precision is
		    //   specified, the '0' flag should be ignored. This is so
		    //   with Solaris 2.6, Digital UNIX 4.0, HPUX 10, Linux,
		    //   FreeBSD, NetBSD; but not with Perl.
		    ଯଦି (precision_specified)
			zero_padding = 0;
		    ଯଦି (fmt_spec == 'd')
		    {
			ଯଦି (force_sign && arg_sign >= 0)
			    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
			// leave negative numbers for sprintf to handle, to
			// avoid handling tricky cases like (short int)-32768
		    }
		    ଅନ୍ୟଥା ଯଦି (alternate_form)
		    {
			ଯଦି (arg_sign != 0
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
		    ଯଦି (!precision_specified)
			precision = 1;   // default precision is 1
		    ଯଦି (precision == 0 && arg_sign == 0)
		    {
			// When zero value is formatted with an explicit
			// precision 0, the resulting formatted string is
			// empty (d, i, u, b, B, o, x, X, p).
		    }
		    ଅନ୍ୟଥା
		    {
			ଅକ୍ଷର	f[6];
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	f_l = 0;

			// construct a simple format string for sprintf
			f[f_l++] = '%';
			ଯଦି (!length_modifier)
			    ;
			ଅନ୍ୟଥା ଯଦି (length_modifier == 'L')
			{
# ifdef MSWIN
			    f[f_l++] = 'I';
			    f[f_l++] = '6';
			    f[f_l++] = '4';
# ଅନ୍ୟଥା
			    f[f_l++] = 'l';
			    f[f_l++] = 'l';
# endif
			}
			ଅନ୍ୟଥା
			    f[f_l++] = length_modifier;
			f[f_l++] = fmt_spec;
			f[f_l++] = '\0';

			ଯଦି (fmt_spec == 'p')
			    str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
			ଅନ୍ୟଥା ଯଦି (fmt_spec == 'b' || fmt_spec == 'B')
			{
			    ଅକ୍ଷର	    b[8 * ଆକାର(uvarnumber_T)];
			    size_t	    b_l = 0;
			    uvarnumber_T    bn = bin_arg;

			    କର
			    {
				b[ଆକାର(b) - ++b_l] = '0' + (bn & 0x1);
				bn >>= 1;
			    }
			    ଯେତେବେଳେ (bn != 0);

			    memcpy(tmp + str_arg_l, b + ଆକାର(b) - b_l, b_l);
			    str_arg_l += b_l;
			}
			ଅନ୍ୟଥା ଯଦି (fmt_spec == 'd')
			{
			    // signed
			    ବାଛ (length_modifier)
			    {
			    ସ୍ଥିତି '\0': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 int_arg);
				       ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'h': str_arg_l += sprintf(
						 tmp + str_arg_l, f,
						 (short)int_arg);
				      ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'l': str_arg_l += sprintf(
						tmp + str_arg_l, f, long_arg);
				      ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'L': str_arg_l += sprintf(
					       tmp + str_arg_l, f, llong_arg);
				      ଭାଙ୍ଗ;
			    }
			}
			ଅନ୍ୟଥା
			{
			    // unsigned
			    ବାଛ (length_modifier)
			    {
			    ସ୍ଥିତି '\0': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						uint_arg);
				       ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'h': str_arg_l += sprintf(
						tmp + str_arg_l, f,
						(unsigned short)uint_arg);
				      ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'l': str_arg_l += sprintf(
					       tmp + str_arg_l, f, ulong_arg);
				      ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'L': str_arg_l += sprintf(
					      tmp + str_arg_l, f, ullong_arg);
				      ଭାଙ୍ଗ;
			    }
			}

			// include the optional minus sign and possible
			// "0x" in the region before the zero padding
			// insertion point
			ଯଦି (zero_padding_insertion_ind < str_arg_l
				&& tmp[zero_padding_insertion_ind] == '-')
			    zero_padding_insertion_ind++;
			ଯଦି (zero_padding_insertion_ind + 1 < str_arg_l
				&& tmp[zero_padding_insertion_ind]   == '0'
				&& (tmp[zero_padding_insertion_ind + 1] == 'x'
				 || tmp[zero_padding_insertion_ind + 1] == 'X'))
			    zero_padding_insertion_ind += 2;
		    }

		    {
			size_t ଅଙ୍କ_of_digits = str_arg_l
						 - zero_padding_insertion_ind;

			ଯଦି (alternate_form && fmt_spec == 'o'
				// unless zero is already the first
				// character
				&& !(zero_padding_insertion_ind < str_arg_l
				    && tmp[zero_padding_insertion_ind] == '0'))
			{
			    // assure leading zero for alternate-form
			    // octal numbers
			    ଯଦି (!precision_specified
					     || precision < ଅଙ୍କ_of_digits + 1)
			    {
				// precision is increased to force the
				// first character to be zero, except if a
				// zero value is formatted with an
				// explicit precision of zero
				precision = ଅଙ୍କ_of_digits + 1;
			    }
			}
			// zero padding to specified precision?
			ଯଦି (ଅଙ୍କ_of_digits < precision)
			    ସଂଖ୍ୟା_of_zeros_to_pad = precision - ଅଙ୍କ_of_digits;
		    }
		    // zero padding to specified minimal field width?
		    ଯଦି (!justify_ବାମ && zero_padding)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(min_field_width - (str_arg_l
						    + ସଂଖ୍ୟା_of_zeros_to_pad));
			ଯଦି (n > 0)
			    ସଂଖ୍ୟା_of_zeros_to_pad += n;
		    }
		    ଭାଙ୍ଗ;
		}

	    ସ୍ଥିତି 'f':
	    ସ୍ଥିତି 'F':
	    ସ୍ଥିତି 'e':
	    ସ୍ଥିତି 'E':
	    ସ୍ଥିତି 'g':
	    ସ୍ଥିତି 'G':
		{
		    // Floating point.
		    ଦ୍ୱିଗୁଣ	f;
		    ଦ୍ୱିଗୁଣ	abs_f;
		    ଅକ୍ଷର	format[40];
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		remove_trailing_zeroes = FALSE;

		    f =
# ଯଦି defined(FEAT_EVAL)
			tvs != ଖାଲି ? tv_float(tvs, &arg_idx) :
# endif
			    (skip_to_arg(ap_types, ap_start, &ap, &arg_idx,
					 &arg_cur, fmt),
			    va_arg(ap, ଦ୍ୱିଗୁଣ));

		    abs_f = f < 0 ? -f : f;

		    ଯଦି (fmt_spec == 'g' || fmt_spec == 'G')
		    {
			// Would be nice to use %g directly, but it prints
			// "1.0" as "1", we don't want that.
			ଯଦି ((abs_f >= 0.001 && abs_f < 10000000.0)
							      || abs_f == 0.0)
			    fmt_spec = ASCII_ISUPPER(fmt_spec) ? 'F' : 'f';
			ଅନ୍ୟଥା
			    fmt_spec = fmt_spec == 'g' ? 'e' : 'E';
			remove_trailing_zeroes = TRUE;
		    }

		    ଯଦି ((fmt_spec == 'f' || fmt_spec == 'F') &&
# ifdef VAX
			    abs_f > 1.0e38
# ଅନ୍ୟଥା
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
		    ଅନ୍ୟଥା
		    {
			ଯଦି (isnan(f))
			{
			    // Not a number: nan or NAN
			    STRCPY(tmp, ASCII_ISUPPER(fmt_spec) ? "NAN"
								      : "nan");
			    str_arg_l = 3;
			    zero_padding = 0;
			}
			ଅନ୍ୟଥା ଯଦି (isinf(f))
			{
			    STRCPY(tmp, infinity_str(f > 0.0, fmt_spec,
					      force_sign, space_for_positive));
			    str_arg_l = STRLEN(tmp);
			    zero_padding = 0;
			}
			ଅନ୍ୟଥା
			{
			    // Regular float number
			    format[0] = '%';
			    l = 1;
			    ଯଦି (force_sign)
				format[l++] = space_for_positive ? ' ' : '+';
			    ଯଦି (precision_specified)
			    {
				size_t max_prec = TMP_LEN - 10;

				// Make sure we don't get more digits than we
				// have room for.
				ଯଦି ((fmt_spec == 'f' || fmt_spec == 'F')
								&& abs_f > 1.0)
				    max_prec -= (size_t)log10(abs_f);
				ଯଦି (precision > max_prec)
				    precision = max_prec;
				l += sprintf(format + l, ".%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)precision);
			    }
			    format[l] = fmt_spec == 'F' ? 'f' : fmt_spec;
			    format[l + 1] = NUL;

			    str_arg_l = sprintf(tmp, format, f);
			}

			ଯଦି (remove_trailing_zeroes)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
			    ଅକ୍ଷର *tp;

			    // Using %g or %G: remove superfluous zeroes.
			    ଯଦି (fmt_spec == 'f' || fmt_spec == 'F')
				tp = tmp + str_arg_l - 1;
			    ଅନ୍ୟଥା
			    {
				tp = (ଅକ୍ଷର *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
				ଯଦି (tp != ଖାଲି)
				{
				    // Remove superfluous '+' and leading
				    // zeroes from the exponent.
				    ଯଦି (tp[1] == '+')
				    {
					// Change "1.0e+07" to "1.0e07"
					STRMOVE(tp + 1, tp + 2);
					--str_arg_l;
				    }
				    i = (tp[1] == '-') ? 2 : 1;
				    ଯେତେବେଳେ (tp[i] == '0')
				    {
					// Change "1.0e07" to "1.0e7"
					STRMOVE(tp + i, tp + i + 1);
					--str_arg_l;
				    }
				    --tp;
				}
			    }

			    ଯଦି (tp != ଖାଲି && !precision_specified)
				// Remove trailing zeroes, but keep the one
				// just after a dot.
				ଯେତେବେଳେ (tp > tmp + 2 && *tp == '0'
							     && tp[-1] != '.')
				{
				    STRMOVE(tp, tp + 1);
				    --tp;
				    --str_arg_l;
				}
			}
			ଅନ୍ୟଥା
			{
			    ଅକ୍ଷର *tp;

			    // Be consistent: some printf("%e") use 1.0e+12
			    // and some 1.0e+012.  Remove one zero in the last
			    // case.
			    tp = (ଅକ୍ଷର *)vim_strchr((char_u *)tmp,
						 fmt_spec == 'e' ? 'e' : 'E');
			    ଯଦି (tp != ଖାଲି && (tp[1] == '+' || tp[1] == '-')
					  && tp[2] == '0'
					  && vim_isdigit(tp[3])
					  && vim_isdigit(tp[4]))
			    {
				STRMOVE(tp + 2, tp + 3);
				--str_arg_l;
			    }
			}
		    }
		    ଯଦି (zero_padding && min_field_width > str_arg_l
					      && (tmp[0] == '-' || force_sign))
		    {
			// padding 0's should be inserted after the sign
			ସଂଖ୍ୟା_of_zeros_to_pad = min_field_width - str_arg_l;
			zero_padding_insertion_ind = 1;
		    }
		    str_arg = tmp;
		    ଭାଙ୍ଗ;
		}

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		// unrecognized conversion specifier, keep format string
		// as-is
		zero_padding = 0;  // turn zero padding off for non-numeric
				   // conversion
		justify_ବାମ = 1;
		min_field_width = 0;		    // reset flags

		// discard the unrecognized conversion, just keep *
		// the unrecognized conversion character
		str_arg = p;
		str_arg_l = 0;
		ଯଦି (*p != NUL)
		    str_arg_l++;  // include invalid conversion specifier
				  // unchanged if not at end-of-string
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (*p != NUL)
		p++;     // step over the just processed conversion specifier

	    // insert padding to the left as requested by min_field_width;
	    // this does not include the zero padding in case of numerical
	    // conversions
	    ଯଦି (!justify_ବାମ)
	    {
		// left padding with blank or zero
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା pn = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(min_field_width - (str_arg_l + ସଂଖ୍ୟା_of_zeros_to_pad));

		ଯଦି (pn > 0)
		{
		    ଯଦି (str_l < str_m)
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
	    ଯଦି (ସଂଖ୍ୟା_of_zeros_to_pad == 0)
	    {
		// will not copy first part of numeric right now, *
		// force it to be copied later in its entirety
		zero_padding_insertion_ind = 0;
	    }
	    ଅନ୍ୟଥା
	    {
		// insert first part of numerics (sign or '0x') before zero
		// padding
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା zn = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)zero_padding_insertion_ind;

		ଯଦି (zn > 0)
		{
		    ଯଦି (str_l < str_m)
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
		zn = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ସଂଖ୍ୟା_of_zeros_to_pad;
		ଯଦି (zn > 0)
		{
		    ଯଦି (str_l < str_m)
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
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା sn = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(str_arg_l - zero_padding_insertion_ind);

		ଯଦି (sn > 0)
		{
		    ଯଦି (str_l < str_m)
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
	    ଯଦି (justify_ବାମ)
	    {
		// right blank padding to the field width
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା pn = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(min_field_width
				      - (str_arg_l + ସଂଖ୍ୟା_of_zeros_to_pad));

		ଯଦି (pn > 0)
		{
		    ଯଦି (str_l < str_m)
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

    ଯଦି (str_m > 0)
    {
	// make sure the string is nul-terminated even at the expense of
	// overwriting the last character (shouldn't happen, but just in case)
	//
	str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }

    ଯଦି (tvs != ଖାଲି && tvs[ଅଙ୍କ_posarg != 0 ? ଅଙ୍କ_posarg : arg_idx - 1].v_type != VAR_UNKNOWN)
	emsg(_(e_too_many_arguments_to_printf));

error:
    vim_free((ଅକ୍ଷର*)ap_types);
    va_end(ap);

    // Return the number of characters formatted (excluding trailing nul
    // character), that is, the number of characters that would have been
    // written to the buffer if it were large enough.
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)str_l;
}

#endif // PROTO
