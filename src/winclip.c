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

#അന്തര്ഭാവ "vim.h"

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
    പൂർണ്ണസംഖ്യ
utf8_to_utf16(char_u *instr, പൂർണ്ണസംഖ്യ inlen, short_u *outstr, പൂർണ്ണസംഖ്യ *unconvlenp)
{
    പൂർണ്ണസംഖ്യ		outlen = 0;
    char_u	*p = instr;
    പൂർണ്ണസംഖ്യ		todo = inlen;
    പൂർണ്ണസംഖ്യ		l;
    പൂർണ്ണസംഖ്യ		ch;

    ആകുമ്പോൾ (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	എങ്കിൽ (l > todo)
	{
	    // Return length of incomplete sequence.
	    എങ്കിൽ (unconvlenp != ശൂന്യ)
		*unconvlenp = todo;
	    നിർത്തുക;
	}

	ch = utf_ptr2char(p);
	എങ്കിൽ (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    എങ്കിൽ (outstr != ശൂന്യ)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (outstr != ശൂന്യ)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    തിരികെ outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    പൂർണ്ണസംഖ്യ
utf16_to_utf8(short_u *instr, പൂർണ്ണസംഖ്യ inlen, char_u *outstr)
{
    പൂർണ്ണസംഖ്യ		outlen = 0;
    പൂർണ്ണസംഖ്യ		todo = inlen;
    short_u	*p = instr;
    പൂർണ്ണസംഖ്യ		l;
    പൂർണ്ണസംഖ്യ		ch, ch2;

    ആകുമ്പോൾ (todo > 0)
    {
	ch = *p;
	എങ്കിൽ (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    എങ്കിൽ (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	എങ്കിൽ (outstr != ശൂന്യ)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	അല്ലെങ്കിൽ
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    തിരികെ outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    ശൂന്യം
MultiByteToWideChar_alloc(UINT cp, DWORD flags,
	LPCSTR in, പൂർണ്ണസംഖ്യ inlen,
	LPWSTR *out, പൂർണ്ണസംഖ്യ *outlen)
{
    *outlen = MultiByteToWideChar(cp, flags, in, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *out = ALLOC_MULT(WCHAR, *outlen + 1);
    എങ്കിൽ (*out == ശൂന്യ)
	തിരികെ;

    MultiByteToWideChar(cp, flags, in, inlen, *out, *outlen);
    (*out)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    ശൂന്യം
WideCharToMultiByte_alloc(UINT cp, DWORD flags,
	LPCWSTR in, പൂർണ്ണസംഖ്യ inlen,
	LPSTR *out, പൂർണ്ണസംഖ്യ *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = WideCharToMultiByte(cp, flags, in, inlen, ശൂന്യ, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *out = alloc(*outlen + 1);
    എങ്കിൽ (*out == ശൂന്യ)
	തിരികെ;
    WideCharToMultiByte(cp, flags, in, inlen, *out, *outlen, def, useddef);
    (*out)[*outlen] = 0;
}


#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

    ശൂന്യം
win_clip_init(ശൂന്യം)
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
തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ type;		// MCHAR, MBLOCK or MLINE
    പൂർണ്ണസംഖ്യ txtlen;		// length of CF_TEXT in bytes
    പൂർണ്ണസംഖ്യ ucslen;		// length of CF_UNICODETEXT in words
    പൂർണ്ണസംഖ്യ rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    പൂർണ്ണസംഖ്യ
clip_mch_own_selection(Clipboard_T *cbd UNUSED)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    തിരികെ FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    ശൂന്യം
clip_mch_lose_selection(Clipboard_T *cbd UNUSED)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    സ്ഥിതം char_u *
crnl_to_nl(സ്ഥിരം char_u *str, പൂർണ്ണസംഖ്യ *size)
{
    പൂർണ്ണസംഖ്യ		pos = 0;
    പൂർണ്ണസംഖ്യ		str_len = *size;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    എങ്കിൽ (ret != ശൂന്യ)
    {
	retp = ret;
	വേണ്ടി (pos = 0; pos < str_len; ++pos)
	{
	    എങ്കിൽ (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*size);
	    }
	    *retp++ = str[pos];
	}
    }

    തിരികെ ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vim_open_clipboard(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ delay = 10;

    ആകുമ്പോൾ (!OpenClipboard(ശൂന്യ))
    {
	എങ്കിൽ (delay > 500)
	    തിരികെ FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    തിരികെ TRUE;
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
    ശൂന്യം
clip_mch_request_selection(Clipboard_T *cbd)
{
    VimClipType_t	metadata = { -1, -1, -1, -1 };
    HGLOBAL		hMem = ശൂന്യ;
    char_u		*str = ശൂന്യ;
    char_u		*to_free = ശൂന്യ;
    HGLOBAL		rawh = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			str_size = 0;
    പൂർണ്ണസംഖ്യ			maxlen;
    size_t		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    എങ്കിൽ (!vim_open_clipboard())
	തിരികെ;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    എങ്കിൽ (IsClipboardFormatAvailable(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	എങ്കിൽ ((meta_h = GetClipboardData(cbd->format)) != ശൂന്യ
		&& (meta_p = (VimClipType_t *)GlobalLock(meta_h)) != ശൂന്യ)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = വലുപ്പം(VimClipType_t);
	    എങ്കിൽ (GlobalSize(meta_h) < n)
		n = GlobalSize(meta_h);
	    memcpy(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    എങ്കിൽ (IsClipboardFormatAvailable(cbd->format_raw)
				      && metadata.rawlen > (പൂർണ്ണസംഖ്യ)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	എങ്കിൽ ((rawh = GetClipboardData(cbd->format_raw)) != ശൂന്യ)
	{
	    char_u	*rawp;

	    rawp = (char_u *)GlobalLock(rawh);
	    എങ്കിൽ (rawp != ശൂന്യ && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_size = (പൂർണ്ണസംഖ്യ)(metadata.rawlen - n);
	    }
	    അല്ലെങ്കിൽ
	    {
		GlobalUnlock(rawh);
		rawh = ശൂന്യ;
	    }
	}
    }
    എങ്കിൽ (str == ശൂന്യ)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	എങ്കിൽ (IsClipboardFormatAvailable(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    എങ്കിൽ ((hMemW = GetClipboardData(CF_UNICODETEXT)) != ശൂന്യ)
	    {
		WCHAR *hMemWstr = (WCHAR *)GlobalLock(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (പൂർണ്ണസംഖ്യ)(GlobalSize(hMemW) / വലുപ്പം(WCHAR));
		എങ്കിൽ (metadata.ucslen >= 0)
		{
		    എങ്കിൽ (metadata.ucslen > maxlen)
			str_size = maxlen;
		    അല്ലെങ്കിൽ
			str_size = metadata.ucslen;
		}
		അല്ലെങ്കിൽ
		{
		    വേണ്ടി (str_size = 0; str_size < maxlen; ++str_size)
			എങ്കിൽ (hMemWstr[str_size] == NUL)
			    നിർത്തുക;
		}
		to_free = str = utf16_to_enc((short_u *)hMemWstr, &str_size);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	അല്ലെങ്കിൽ എങ്കിൽ (IsClipboardFormatAvailable(CF_TEXT))
	{
	    എങ്കിൽ ((hMem = GetClipboardData(CF_TEXT)) != ശൂന്യ)
	    {
		str = (char_u *)GlobalLock(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (പൂർണ്ണസംഖ്യ)GlobalSize(hMem);
		എങ്കിൽ (metadata.txtlen >= 0)
		{
		    എങ്കിൽ (metadata.txtlen > maxlen)
			str_size = maxlen;
		    അല്ലെങ്കിൽ
			str_size = metadata.txtlen;
		}
		അല്ലെങ്കിൽ
		{
		    വേണ്ടി (str_size = 0; str_size < maxlen; ++str_size)
			എങ്കിൽ (str[str_size] == NUL)
			    നിർത്തുക;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_to_enc(str, str_size, &to_free, &maxlen);
		എങ്കിൽ (to_free != ശൂന്യ)
		{
		    str_size = maxlen;
		    str = to_free;
		}
	    }
	}
    }

    എങ്കിൽ (str != ശൂന്യ && metadata.txtlen != 0)
    {
	char_u *temp_clipboard;

	// If the type is not known detect it.
	എങ്കിൽ (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	temp_clipboard = crnl_to_nl(str, &str_size);
	എങ്കിൽ (temp_clipboard != ശൂന്യ)
	{
	    clip_yank_selection(metadata.type, temp_clipboard, str_size, cbd);
	    vim_free(temp_clipboard);
	}
    }

    // unlock the global object
    എങ്കിൽ (hMem != ശൂന്യ)
	GlobalUnlock(hMem);
    എങ്കിൽ (rawh != ശൂന്യ)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(to_free);
}

/*
 * Send the current selection to the clipboard.
 */
    ശൂന്യം
clip_mch_set_selection(Clipboard_T *cbd)
{
    char_u		*str = ശൂന്യ;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = ശൂന്യ;
    HGLOBAL		hMem = ശൂന്യ;
    HGLOBAL		hMemVim = ശൂന്യ;
    HGLOBAL		hMemW = ശൂന്യ;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    എങ്കിൽ (metadata.type < 0)
	തിരികെ;
    metadata.txtlen = (പൂർണ്ണസംഖ്യ)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (പൂർണ്ണസംഖ്യ)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)GlobalLock(hMemRaw);
	എങ്കിൽ (lpszMemRaw != ശൂന്യ)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    memcpy(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	അല്ലെങ്കിൽ
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*out;
	പൂർണ്ണസംഖ്യ		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	out = (WCHAR *)enc_to_utf16(str, &len);
	എങ്കിൽ (out != ശൂന്യ)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = WideCharToMultiByte(GetACP(), 0, out, len,
							       ശൂന്യ, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    എങ്കിൽ (str == ശൂന്യ)
	    {
		vim_free(out);
		തിരികെ;		// out of memory
	    }
	    WideCharToMultiByte(GetACP(), 0, out, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * വലുപ്പം(WCHAR));
	    lpszMemW = (WCHAR *)GlobalLock(hMemW);
	    എങ്കിൽ (lpszMemW != ശൂന്യ)
	    {
		memcpy(lpszMemW, out, len * വലുപ്പം(WCHAR));
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

	എങ്കിൽ (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = ശൂന്യ;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       വലുപ്പം(VimClipType_t));
	lpszMemVim = (VimClipType_t *)GlobalLock(hMemVim);
	memcpy(lpszMemVim, &metadata, വലുപ്പം(metadata));
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
    എങ്കിൽ (vim_open_clipboard())
    {
	എങ്കിൽ (EmptyClipboard())
	{
	    SetClipboardData(cbd->format, hMemVim);
	    hMemVim = 0;
	    എങ്കിൽ (hMemW != ശൂന്യ)
	    {
		എങ്കിൽ (SetClipboardData(CF_UNICODETEXT, hMemW) != ശൂന്യ)
		    hMemW = ശൂന്യ;
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
    എങ്കിൽ (hMemRaw)
	GlobalFree(hMemRaw);
    എങ്കിൽ (hMem)
	GlobalFree(hMem);
    എങ്കിൽ (hMemW)
	GlobalFree(hMemW);
    എങ്കിൽ (hMemVim)
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
enc_to_utf16(char_u *str, പൂർണ്ണസംഖ്യ *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len_loc;
    പൂർണ്ണസംഖ്യ		length;

    എങ്കിൽ (lenp == ശൂന്യ)
    {
	len_loc = (പൂർണ്ണസംഖ്യ)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    എങ്കിൽ (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	MultiByteToWideChar_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &length);
    }
    അല്ലെങ്കിൽ
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	എങ്കിൽ (convert_setup(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    എങ്കിൽ (str == ശൂന്യ)
		തിരികെ ശൂന്യ;
	}
	convert_setup(&conv, ശൂന്യ, ശൂന്യ);

	length = utf8_to_utf16(str, *lenp, ശൂന്യ, ശൂന്യ);
	ret = ALLOC_MULT(WCHAR, length + 1);
	എങ്കിൽ (ret != ശൂന്യ)
	{
	    utf8_to_utf16(str, *lenp, (short_u *)ret, ശൂന്യ);
	    ret[length] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = length;
    തിരികെ (short_u *)ret;
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
utf16_to_enc(short_u *str, പൂർണ്ണസംഖ്യ *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = ശൂന്യ, *enc_str = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len_loc;

    എങ്കിൽ (lenp == ശൂന്യ)
    {
	len_loc = (പൂർണ്ണസംഖ്യ)wcslen(str) + 1;
	lenp = &len_loc;
    }

    എങ്കിൽ (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	പൂർണ്ണസംഖ്യ length;

	WideCharToMultiByte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &length, 0, 0);
	*lenp = length;
	തിരികെ enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_to_utf8(str, *lenp == 0 ? 1 : *lenp, ശൂന്യ));
    എങ്കിൽ (utf8_str != ശൂന്യ)
    {
	*lenp = utf16_to_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	എങ്കിൽ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	അല്ലെങ്കിൽ
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_setup(&conv, ശൂന്യ, ശൂന്യ);
    }

    തിരികെ enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    ശൂന്യം
acp_to_enc(
    char_u	*str,
    പൂർണ്ണസംഖ്യ		str_size,
    char_u	**out,
    പൂർണ്ണസംഖ്യ		*outlen)

{
    LPWSTR	widestr;

    MultiByteToWideChar_alloc(GetACP(), 0, (LPCSTR)str, str_size,
							    &widestr, outlen);
    എങ്കിൽ (widestr == ശൂന്യ)
	തിരികെ;
    ++*outlen;	// Include the 0 after the string
    *out = utf16_to_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    ശൂന്യം
enc_to_acp(
    char_u	*str,
    പൂർണ്ണസംഖ്യ		str_size,
    char_u	**out,
    പൂർണ്ണസംഖ്യ		*outlen)

{
    LPWSTR	widestr;
    പൂർണ്ണസംഖ്യ		len = str_size;

    widestr = (WCHAR *)enc_to_utf16(str, &len);
    എങ്കിൽ (widestr == ശൂന്യ)
	തിരികെ;
    WideCharToMultiByte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)out, outlen, 0, 0);
    vim_free(widestr);
}
