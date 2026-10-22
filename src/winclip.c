/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * winclip.c
 *
 * Routines for Win32 clipboard handling.
 * Also used by Cygwin, using os_unix.c.
 */

#include "vim.h"

/*
 * Compile only the clipboard handling features when compiling for cygwin
 * posix environment.
 */
#ifdef FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include "winclip.pro"
#endif

/*
 * Convert an UTF-8 string to UTF-16.
 * "instr[inlen]" is the input.  "inlen" is in bytes.
 * When "outstr" is NULL only return the number of UTF-16 words produced.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Returns the number of UTF-16 words produced.
 */
    ཧྲིལ
utf8_to_utf16(char_u *instr, ཧྲིལ inlen, short_u *outstr, ཧྲིལ *unconvlenp)
{
    ཧྲིལ		outlen = 0;
    char_u	*p = instr;
    ཧྲིལ		todo = inlen;
    ཧྲིལ		l;
    ཧྲིལ		ch;

    སྐབས (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	གལ་ཏེ (l > todo)
	{
	    // Return length of incomplete sequence.
	    གལ་ཏེ (unconvlenp != NULL)
		*unconvlenp = todo;
	    bha_nga;
	}

	ch = utf_ptr2char(p);
	གལ་ཏེ (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    གལ་ཏེ (outstr != NULL)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	དེ་མིན གལ་ཏེ (outstr != NULL)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    ཕྱིར outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    ཧྲིལ
utf16_to_utf8(short_u *instr, ཧྲིལ inlen, char_u *outstr)
{
    ཧྲིལ		outlen = 0;
    ཧྲིལ		todo = inlen;
    short_u	*p = instr;
    ཧྲིལ		l;
    ཧྲིལ		ch, ch2;

    སྐབས (todo > 0)
    {
	ch = *p;
	གལ་ཏེ (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    གལ་ཏེ (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	གལ་ཏེ (outstr != NULL)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	དེ་མིན
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    ཕྱིར outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    void
MultiByteToWideChar_alloc(UINT cp, DWORD flags,
	LPCSTR in, ཧྲིལ inlen,
	LPWSTR *out, ཧྲིལ *outlen)
{
    *outlen = MultiByteToWideChar(cp, flags, in, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *out = ALLOC_MULT(WCHAR, *outlen + 1);
    གལ་ཏེ (*out == NULL)
	ཕྱིར;

    MultiByteToWideChar(cp, flags, in, inlen, *out, *outlen);
    (*out)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    void
WideCharToMultiByte_alloc(UINT cp, DWORD flags,
	LPCWSTR in, ཧྲིལ inlen,
	LPSTR *out, ཧྲིལ *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = WideCharToMultiByte(cp, flags, in, inlen, NULL, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *out = alloc(*outlen + 1);
    གལ་ཏེ (*out == NULL)
	ཕྱིར;
    WideCharToMultiByte(cp, flags, in, inlen, *out, *outlen, def, useddef);
    (*out)[*outlen] = 0;
}


#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

    void
win_clip_init(void)
{
    clip_init(TRUE);

    /*
     * Vim's own clipboard format recognises whether the text is char, line,
     * or rectangular block.  Only useful for copying between two Vims.
     * "Clipboard_T" was used for previous versions, using the first
     * character to specify MCHAR, MLINE or MBLOCK.
     */
    clip_star.format = RegisterClipboardFormat("VimClipboard2");
    clip_star.format_raw = RegisterClipboardFormat("VimRawBytes");
}

// Type used for the clipboard type of Vim's data.
typedef sa_nracanaa
{
    ཧྲིལ type;		// MCHAR, MBLOCK or MLINE
    ཧྲིལ txtlen;		// length of CF_TEXT in bytes
    ཧྲིལ ucslen;		// length of CF_UNICODETEXT in words
    ཧྲིལ rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    ཧྲིལ
clip_mch_own_selection(Clipboard_T *cbd UNUSED)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ཕྱིར FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    void
clip_mch_lose_selection(Clipboard_T *cbd UNUSED)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    static char_u *
crnl_to_nl(const char_u *str, ཧྲིལ *size)
{
    ཧྲིལ		pos = 0;
    ཧྲིལ		str_len = *size;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    གལ་ཏེ (ret != NULL)
    {
	retp = ret;
	ཆེད་དུ (pos = 0; pos < str_len; ++pos)
	{
	    གལ་ཏེ (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*size);
	    }
	    *retp++ = str[pos];
	}
    }

    ཕྱིར ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    static ཧྲིལ
vim_open_clipboard(void)
{
    ཧྲིལ delay = 10;

    སྐབས (!OpenClipboard(NULL))
    {
	གལ་ཏེ (delay > 500)
	    ཕྱིར FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    ཕྱིར TRUE;
}

/*
 * Get the current selection and put it in the clipboard register.
 *
 * NOTE: Must use GlobalLock/Unlock here to ensure Win32s compatibility.
 * On NT/W95 the clipboard data is a fixed global memory object and
 * so its handle = its pointer.
 * On Win32s, however, co-operation with the Win16 system means that
 * the clipboard data is moveable and its handle is not a pointer at all,
 * so we can't just cast the return value of GetClipboardData to (char_u*).
 * <VN>
 */
    void
clip_mch_request_selection(Clipboard_T *cbd)
{
    VimClipType_t	metadata = { -1, -1, -1, -1 };
    HGLOBAL		hMem = NULL;
    char_u		*str = NULL;
    char_u		*to_free = NULL;
    HGLOBAL		rawh = NULL;
    ཧྲིལ			str_size = 0;
    ཧྲིལ			maxlen;
    size_t		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    གལ་ཏེ (!vim_open_clipboard())
	ཕྱིར;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    གལ་ཏེ (IsClipboardFormatAvailable(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	གལ་ཏེ ((meta_h = GetClipboardData(cbd->format)) != NULL
		&& (meta_p = (VimClipType_t *)GlobalLock(meta_h)) != NULL)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = parimaa_na(VimClipType_t);
	    གལ་ཏེ (GlobalSize(meta_h) < n)
		n = GlobalSize(meta_h);
	    memcpy(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    གལ་ཏེ (IsClipboardFormatAvailable(cbd->format_raw)
				      && metadata.rawlen > (ཧྲིལ)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	གལ་ཏེ ((rawh = GetClipboardData(cbd->format_raw)) != NULL)
	{
	    char_u	*rawp;

	    rawp = (char_u *)GlobalLock(rawh);
	    གལ་ཏེ (rawp != NULL && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_size = (ཧྲིལ)(metadata.rawlen - n);
	    }
	    དེ་མིན
	    {
		GlobalUnlock(rawh);
		rawh = NULL;
	    }
	}
    }
    གལ་ཏེ (str == NULL)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	གལ་ཏེ (IsClipboardFormatAvailable(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    གལ་ཏེ ((hMemW = GetClipboardData(CF_UNICODETEXT)) != NULL)
	    {
		WCHAR *hMemWstr = (WCHAR *)GlobalLock(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (ཧྲིལ)(GlobalSize(hMemW) / parimaa_na(WCHAR));
		གལ་ཏེ (metadata.ucslen >= 0)
		{
		    གལ་ཏེ (metadata.ucslen > maxlen)
			str_size = maxlen;
		    དེ་མིན
			str_size = metadata.ucslen;
		}
		དེ་མིན
		{
		    ཆེད་དུ (str_size = 0; str_size < maxlen; ++str_size)
			གལ་ཏེ (hMemWstr[str_size] == NUL)
			    bha_nga;
		}
		to_free = str = utf16_to_enc((short_u *)hMemWstr, &str_size);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	དེ་མིན གལ་ཏེ (IsClipboardFormatAvailable(CF_TEXT))
	{
	    གལ་ཏེ ((hMem = GetClipboardData(CF_TEXT)) != NULL)
	    {
		str = (char_u *)GlobalLock(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (ཧྲིལ)GlobalSize(hMem);
		གལ་ཏེ (metadata.txtlen >= 0)
		{
		    གལ་ཏེ (metadata.txtlen > maxlen)
			str_size = maxlen;
		    དེ་མིན
			str_size = metadata.txtlen;
		}
		དེ་མིན
		{
		    ཆེད་དུ (str_size = 0; str_size < maxlen; ++str_size)
			གལ་ཏེ (str[str_size] == NUL)
			    bha_nga;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_to_enc(str, str_size, &to_free, &maxlen);
		གལ་ཏེ (to_free != NULL)
		{
		    str_size = maxlen;
		    str = to_free;
		}
	    }
	}
    }

    གལ་ཏེ (str != NULL && metadata.txtlen != 0)
    {
	char_u *temp_clipboard;

	// If the type is not known detect it.
	གལ་ཏེ (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	temp_clipboard = crnl_to_nl(str, &str_size);
	གལ་ཏེ (temp_clipboard != NULL)
	{
	    clip_yank_selection(metadata.type, temp_clipboard, str_size, cbd);
	    vim_free(temp_clipboard);
	}
    }

    // unlock the global object
    གལ་ཏེ (hMem != NULL)
	GlobalUnlock(hMem);
    གལ་ཏེ (rawh != NULL)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(to_free);
}

/*
 * Send the current selection to the clipboard.
 */
    void
clip_mch_set_selection(Clipboard_T *cbd)
{
    char_u		*str = NULL;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = NULL;
    HGLOBAL		hMem = NULL;
    HGLOBAL		hMemVim = NULL;
    HGLOBAL		hMemW = NULL;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    གལ་ཏེ (metadata.type < 0)
	ཕྱིར;
    metadata.txtlen = (ཧྲིལ)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (ཧྲིལ)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)GlobalLock(hMemRaw);
	གལ་ཏེ (lpszMemRaw != NULL)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    memcpy(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	དེ་མིན
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*out;
	ཧྲིལ		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	out = (WCHAR *)enc_to_utf16(str, &len);
	གལ་ཏེ (out != NULL)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = WideCharToMultiByte(GetACP(), 0, out, len,
							       NULL, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    གལ་ཏེ (str == NULL)
	    {
		vim_free(out);
		ཕྱིར;		// out of memory
	    }
	    WideCharToMultiByte(GetACP(), 0, out, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * parimaa_na(WCHAR));
	    lpszMemW = (WCHAR *)GlobalLock(hMemW);
	    གལ་ཏེ (lpszMemW != NULL)
	    {
		memcpy(lpszMemW, out, len * parimaa_na(WCHAR));
		lpszMemW[len] = NUL;
		GlobalUnlock(hMemW);
	    }
	    vim_free(out);
	    metadata.ucslen = len;
	}
    }

    // Allocate memory for the text, add one NUL byte to terminate the string.
    hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, metadata.txtlen + 1);
    {
	LPSTR lpszMem = (LPSTR)GlobalLock(hMem);

	གལ་ཏེ (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = NULL;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       parimaa_na(VimClipType_t));
	lpszMemVim = (VimClipType_t *)GlobalLock(hMemVim);
	memcpy(lpszMemVim, &metadata, parimaa_na(metadata));
	GlobalUnlock(hMemVim);
    }

    /*
     * Open the clipboard, clear it and put our text on it.
     * Always set our Vim format.  Put Unicode and plain text on it.
     *
     * Don't pass GetActiveWindow() as an argument to OpenClipboard()
     * because then we can't paste back into the same window for some
     * reason - webb.
     */
    གལ་ཏེ (vim_open_clipboard())
    {
	གལ་ཏེ (EmptyClipboard())
	{
	    SetClipboardData(cbd->format, hMemVim);
	    hMemVim = 0;
	    གལ་ཏེ (hMemW != NULL)
	    {
		གལ་ཏེ (SetClipboardData(CF_UNICODETEXT, hMemW) != NULL)
		    hMemW = NULL;
	    }
	    // Always use CF_TEXT.  On Win98 Notepad won't obtain the
	    // CF_UNICODETEXT text, only CF_TEXT.
	    SetClipboardData(CF_TEXT, hMem);
	    hMem = 0;
	}
	CloseClipboard();
    }

    vim_free(str);
    // Free any allocations we didn't give to the clipboard:
    གལ་ཏེ (hMemRaw)
	GlobalFree(hMemRaw);
    གལ་ཏེ (hMem)
	GlobalFree(hMem);
    གལ་ཏེ (hMemW)
	GlobalFree(hMemW);
    གལ་ཏེ (hMemVim)
	GlobalFree(hMemVim);
}

#endif // FEAT_CLIPBOARD

/*
 * Note: the following two functions are only guaranteed to work when using
 * valid MS-Windows codepages or when iconv() is available.
 */

/*
 * Convert "str" from 'encoding' to UTF-16.
 * Input in "str" with length "*lenp".  When "lenp" is NULL, use strlen().
 * Output is returned as an allocated string.  "*lenp" is set to the length of
 * the result.  A trailing NUL is always added.
 * Returns NULL when out of memory.
 */
    short_u *
enc_to_utf16(char_u *str, ཧྲིལ *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = NULL;
    ཧྲིལ		len_loc;
    ཧྲིལ		length;

    གལ་ཏེ (lenp == NULL)
    {
	len_loc = (ཧྲིལ)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    གལ་ཏེ (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	MultiByteToWideChar_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &length);
    }
    དེ་མིན
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	གལ་ཏེ (convert_setup(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    ཕྱིར NULL;
	གལ་ཏེ (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    གལ་ཏེ (str == NULL)
		ཕྱིར NULL;
	}
	convert_setup(&conv, NULL, NULL);

	length = utf8_to_utf16(str, *lenp, NULL, NULL);
	ret = ALLOC_MULT(WCHAR, length + 1);
	གལ་ཏེ (ret != NULL)
	{
	    utf8_to_utf16(str, *lenp, (short_u *)ret, NULL);
	    ret[length] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = length;
    ཕྱིར (short_u *)ret;
}

/*
 * Convert an UTF-16 string to 'encoding'.
 * Input in "str" with length (counted in wide characters) "*lenp".  When
 * "lenp" is NULL, use wcslen().
 * Output is returned as an allocated string.  If "*lenp" is not NULL it is
 * set to the length of the result.
 * Returns NULL when out of memory.
 */
    char_u *
utf16_to_enc(short_u *str, ཧྲིལ *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = NULL, *enc_str = NULL;
    ཧྲིལ		len_loc;

    གལ་ཏེ (lenp == NULL)
    {
	len_loc = (ཧྲིལ)wcslen(str) + 1;
	lenp = &len_loc;
    }

    གལ་ཏེ (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	ཧྲིལ length;

	WideCharToMultiByte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &length, 0, 0);
	*lenp = length;
	ཕྱིར enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_to_utf8(str, *lenp == 0 ? 1 : *lenp, NULL));
    གལ་ཏེ (utf8_str != NULL)
    {
	*lenp = utf16_to_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	གལ་ཏེ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	དེ་མིན
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_setup(&conv, NULL, NULL);
    }

    ཕྱིར enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    void
acp_to_enc(
    char_u	*str,
    ཧྲིལ		str_size,
    char_u	**out,
    ཧྲིལ		*outlen)

{
    LPWSTR	widestr;

    MultiByteToWideChar_alloc(GetACP(), 0, (LPCSTR)str, str_size,
							    &widestr, outlen);
    གལ་ཏེ (widestr == NULL)
	ཕྱིར;
    ++*outlen;	// Include the 0 after the string
    *out = utf16_to_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    void
enc_to_acp(
    char_u	*str,
    ཧྲིལ		str_size,
    char_u	**out,
    ཧྲིལ		*outlen)

{
    LPWSTR	widestr;
    ཧྲིལ		len = str_size;

    widestr = (WCHAR *)enc_to_utf16(str, &len);
    གལ་ཏེ (widestr == NULL)
	ཕྱིར;
    WideCharToMultiByte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)out, outlen, 0, 0);
    vim_free(widestr);
}
