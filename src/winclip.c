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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * Compile only the clipboard handling features when compiling for cygwin
 * posix environment.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_ਔਸਤ
# include <windows.h>
# include "winclip.pro"
#ਜੇ_ਅੰਤ

/*
 * Convert an UTF-8 string to UTF-16.
 * "instr[inlen]" is the input.  "inlen" is in bytes.
 * When "outstr" is NULL only return the number of UTF-16 words produced.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Returns the number of UTF-16 words produced.
 */
    ਪੂਰਨ
utf8_ਮੰਜ਼ਿਲ_utf16(char_u *instr, ਪੂਰਨ inlen, short_u *outstr, ਪੂਰਨ *unconvlenp)
{
    ਪੂਰਨ		outlen = 0;
    char_u	*p = instr;
    ਪੂਰਨ		todo = inlen;
    ਪੂਰਨ		l;
    ਪੂਰਨ		ch;

    ਜਦੋਂ (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	ਜੇ (l > todo)
	{
	    // Return length of incomplete sequence.
	    ਜੇ (unconvlenp != ਨਿਰਾ)
		*unconvlenp = todo;
	    ਰੋਕੋ;
	}

	ch = utf_ptr2char(p);
	ਜੇ (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    ਜੇ (outstr != ਨਿਰਾ)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (outstr != ਨਿਰਾ)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    ਵਾਪਸ outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    ਪੂਰਨ
utf16_ਮੰਜ਼ਿਲ_utf8(short_u *instr, ਪੂਰਨ inlen, char_u *outstr)
{
    ਪੂਰਨ		outlen = 0;
    ਪੂਰਨ		todo = inlen;
    short_u	*p = instr;
    ਪੂਰਨ		l;
    ਪੂਰਨ		ch, ch2;

    ਜਦੋਂ (todo > 0)
    {
	ch = *p;
	ਜੇ (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    ਜੇ (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	ਜੇ (outstr != ਨਿਰਾ)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	ਨਹੀਂਤਾਂ
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    ਵਾਪਸ outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    ਖਾਲੀ
Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char_alloc(UINT cp, DWORD flags,
	LPCSTR ਵਿੱਚ, ਪੂਰਨ inlen,
	LPWSTR *ਬਾਹਰਲਾ, ਪੂਰਨ *outlen)
{
    *outlen = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(cp, flags, ਵਿੱਚ, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *ਬਾਹਰਲਾ = ALLOC_MULT(WCHAR, *outlen + 1);
    ਜੇ (*ਬਾਹਰਲਾ == ਨਿਰਾ)
	ਵਾਪਸ;

    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(cp, flags, ਵਿੱਚ, inlen, *ਬਾਹਰਲਾ, *outlen);
    (*ਬਾਹਰਲਾ)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    ਖਾਲੀ
Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte_alloc(UINT cp, DWORD flags,
	LPCWSTR ਵਿੱਚ, ਪੂਰਨ inlen,
	LPSTR *ਬਾਹਰਲਾ, ਪੂਰਨ *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(cp, flags, ਵਿੱਚ, inlen, ਨਿਰਾ, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *ਬਾਹਰਲਾ = alloc(*outlen + 1);
    ਜੇ (*ਬਾਹਰਲਾ == ਨਿਰਾ)
	ਵਾਪਸ;
    Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(cp, flags, ਵਿੱਚ, inlen, *ਬਾਹਰਲਾ, *outlen, def, useddef);
    (*ਬਾਹਰਲਾ)[*outlen] = 0;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

    ਖਾਲੀ
win_clip_init(ਖਾਲੀ)
{
    clip_init(TRUE);

    /*
     * Vim's own clipboard format recognises whether the text is char, line,
     * or rectangular block.  Only useful for copying between two Vims.
     * "Clipboard_T" was used for previous versions, using the first
     * character to specify MCHAR, MLINE or MBLOCK.
     */
    clip_ਤਾਰਾ.format = ਰਜਿਸਟਰ_Clipboard_Format("VimClipboard2");
    clip_ਤਾਰਾ.format_raw = ਰਜਿਸਟਰ_Clipboard_Format("VimRawBytes");
}

// Type used for the clipboard type of Vim's data.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ type;		// MCHAR, MBLOCK or MLINE
    ਪੂਰਨ txtlen;		// length of CF_TEXT in bytes
    ਪੂਰਨ ucslen;		// length of CF_UNICODETEXT in words
    ਪੂਰਨ rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    ਪੂਰਨ
clip_mch_own_selection(Clipboard_T *cbd ਅਣਵਰਤਿਆ)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ਵਾਪਸ FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    ਖਾਲੀ
clip_mch_lose_selection(Clipboard_T *cbd ਅਣਵਰਤਿਆ)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    ਸਥਿਰ char_u *
crnl_ਮੰਜ਼ਿਲ_nl(ਅਚਲ char_u *str, ਪੂਰਨ *ਮਾਪ)
{
    ਪੂਰਨ		pos = 0;
    ਪੂਰਨ		str_len = *ਮਾਪ;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    ਜੇ (ret != ਨਿਰਾ)
    {
	retp = ret;
	ਲਈ (pos = 0; pos < str_len; ++pos)
	{
	    ਜੇ (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*ਮਾਪ);
	    }
	    *retp++ = str[pos];
	}
    }

    ਵਾਪਸ ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    ਸਥਿਰ ਪੂਰਨ
vim_open_clipboard(ਖਾਲੀ)
{
    ਪੂਰਨ delay = 10;

    ਜਦੋਂ (!OpenClipboard(ਨਿਰਾ))
    {
	ਜੇ (delay > 500)
	    ਵਾਪਸ FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    ਵਾਪਸ TRUE;
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
    ਖਾਲੀ
clip_mch_request_selection(Clipboard_T *cbd)
{
    VimClipType_t	metadata = { -1, -1, -1, -1 };
    HGLOBAL		hMem = ਨਿਰਾ;
    char_u		*str = ਨਿਰਾ;
    char_u		*ਮੰਜ਼ਿਲ_free = ਨਿਰਾ;
    HGLOBAL		rawh = ਨਿਰਾ;
    ਪੂਰਨ			str_ਮਾਪ = 0;
    ਪੂਰਨ			maxlen;
    size_t		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    ਜੇ (!vim_open_clipboard())
	ਵਾਪਸ;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    ਜੇ (Is_Clipboard_Format_ਉਪਲਬਧ(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	ਜੇ ((meta_h = Get_Clipboard_ਅੰਕੜੇ(cbd->format)) != ਨਿਰਾ
		&& (meta_p = (VimClipType_t *)Global_ਤਾਲਾ(meta_h)) != ਨਿਰਾ)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = ਆਕਾਰ(VimClipType_t);
	    ਜੇ (Global_ਮਾਪ(meta_h) < n)
		n = Global_ਮਾਪ(meta_h);
	    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    ਜੇ (Is_Clipboard_Format_ਉਪਲਬਧ(cbd->format_raw)
				      && metadata.rawlen > (ਪੂਰਨ)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	ਜੇ ((rawh = Get_Clipboard_ਅੰਕੜੇ(cbd->format_raw)) != ਨਿਰਾ)
	{
	    char_u	*rawp;

	    rawp = (char_u *)Global_ਤਾਲਾ(rawh);
	    ਜੇ (rawp != ਨਿਰਾ && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_ਮਾਪ = (ਪੂਰਨ)(metadata.rawlen - n);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		GlobalUnlock(rawh);
		rawh = ਨਿਰਾ;
	    }
	}
    }
    ਜੇ (str == ਨਿਰਾ)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	ਜੇ (Is_Clipboard_Format_ਉਪਲਬਧ(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    ਜੇ ((hMemW = Get_Clipboard_ਅੰਕੜੇ(CF_UNICODETEXT)) != ਨਿਰਾ)
	    {
		WCHAR *hMemWstr = (WCHAR *)Global_ਤਾਲਾ(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (ਪੂਰਨ)(Global_ਮਾਪ(hMemW) / ਆਕਾਰ(WCHAR));
		ਜੇ (metadata.ucslen >= 0)
		{
		    ਜੇ (metadata.ucslen > maxlen)
			str_ਮਾਪ = maxlen;
		    ਨਹੀਂਤਾਂ
			str_ਮਾਪ = metadata.ucslen;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਲਈ (str_ਮਾਪ = 0; str_ਮਾਪ < maxlen; ++str_ਮਾਪ)
			ਜੇ (hMemWstr[str_ਮਾਪ] == NUL)
			    ਰੋਕੋ;
		}
		ਮੰਜ਼ਿਲ_free = str = utf16_ਮੰਜ਼ਿਲ_enc((short_u *)hMemWstr, &str_ਮਾਪ);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	ਨਹੀਂਤਾਂ ਜੇ (Is_Clipboard_Format_ਉਪਲਬਧ(CF_ਪਾਠ))
	{
	    ਜੇ ((hMem = Get_Clipboard_ਅੰਕੜੇ(CF_ਪਾਠ)) != ਨਿਰਾ)
	    {
		str = (char_u *)Global_ਤਾਲਾ(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (ਪੂਰਨ)Global_ਮਾਪ(hMem);
		ਜੇ (metadata.txtlen >= 0)
		{
		    ਜੇ (metadata.txtlen > maxlen)
			str_ਮਾਪ = maxlen;
		    ਨਹੀਂਤਾਂ
			str_ਮਾਪ = metadata.txtlen;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਲਈ (str_ਮਾਪ = 0; str_ਮਾਪ < maxlen; ++str_ਮਾਪ)
			ਜੇ (str[str_ਮਾਪ] == NUL)
			    ਰੋਕੋ;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_ਮੰਜ਼ਿਲ_enc(str, str_ਮਾਪ, &ਮੰਜ਼ਿਲ_free, &maxlen);
		ਜੇ (ਮੰਜ਼ਿਲ_free != ਨਿਰਾ)
		{
		    str_ਮਾਪ = maxlen;
		    str = ਮੰਜ਼ਿਲ_free;
		}
	    }
	}
    }

    ਜੇ (str != ਨਿਰਾ && metadata.txtlen != 0)
    {
	char_u *ਆਰਜ਼ੀ_clipboard;

	// If the type is not known detect it.
	ਜੇ (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	ਆਰਜ਼ੀ_clipboard = crnl_ਮੰਜ਼ਿਲ_nl(str, &str_ਮਾਪ);
	ਜੇ (ਆਰਜ਼ੀ_clipboard != ਨਿਰਾ)
	{
	    clip_yank_selection(metadata.type, ਆਰਜ਼ੀ_clipboard, str_ਮਾਪ, cbd);
	    vim_free(ਆਰਜ਼ੀ_clipboard);
	}
    }

    // unlock the global object
    ਜੇ (hMem != ਨਿਰਾ)
	GlobalUnlock(hMem);
    ਜੇ (rawh != ਨਿਰਾ)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(ਮੰਜ਼ਿਲ_free);
}

/*
 * Send the current selection to the clipboard.
 */
    ਖਾਲੀ
clip_mch_ਸੈੱਟ_selection(Clipboard_T *cbd)
{
    char_u		*str = ਨਿਰਾ;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = ਨਿਰਾ;
    HGLOBAL		hMem = ਨਿਰਾ;
    HGLOBAL		hMemVim = ਨਿਰਾ;
    HGLOBAL		hMemW = ਨਿਰਾ;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    ਜੇ (metadata.type < 0)
	ਵਾਪਸ;
    metadata.txtlen = (ਪੂਰਨ)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (ਪੂਰਨ)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)Global_ਤਾਲਾ(hMemRaw);
	ਜੇ (lpszMemRaw != ਨਿਰਾ)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    ਸਮ੍ਰਿਤੀ_ਨਕਲ(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	ਨਹੀਂਤਾਂ
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*ਬਾਹਰਲਾ;
	ਪੂਰਨ		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	ਬਾਹਰਲਾ = (WCHAR *)enc_ਮੰਜ਼ਿਲ_utf16(str, &len);
	ਜੇ (ਬਾਹਰਲਾ != ਨਿਰਾ)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(GetACP(), 0, ਬਾਹਰਲਾ, len,
							       ਨਿਰਾ, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    ਜੇ (str == ਨਿਰਾ)
	    {
		vim_free(ਬਾਹਰਲਾ);
		ਵਾਪਸ;		// out of memory
	    }
	    Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(GetACP(), 0, ਬਾਹਰਲਾ, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * ਆਕਾਰ(WCHAR));
	    lpszMemW = (WCHAR *)Global_ਤਾਲਾ(hMemW);
	    ਜੇ (lpszMemW != ਨਿਰਾ)
	    {
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(lpszMemW, ਬਾਹਰਲਾ, len * ਆਕਾਰ(WCHAR));
		lpszMemW[len] = NUL;
		GlobalUnlock(hMemW);
	    }
	    vim_free(ਬਾਹਰਲਾ);
	    metadata.ucslen = len;
	}
    }

    // Allocate memory for the text, add one NUL byte to terminate the string.
    hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, metadata.txtlen + 1);
    {
	LPSTR lpszMem = (LPSTR)Global_ਤਾਲਾ(hMem);

	ਜੇ (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = ਨਿਰਾ;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       ਆਕਾਰ(VimClipType_t));
	lpszMemVim = (VimClipType_t *)Global_ਤਾਲਾ(hMemVim);
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(lpszMemVim, &metadata, ਆਕਾਰ(metadata));
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
    ਜੇ (vim_open_clipboard())
    {
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_Clipboard())
	{
	    ਸੈੱਟ_Clipboard_ਅੰਕੜੇ(cbd->format, hMemVim);
	    hMemVim = 0;
	    ਜੇ (hMemW != ਨਿਰਾ)
	    {
		ਜੇ (ਸੈੱਟ_Clipboard_ਅੰਕੜੇ(CF_UNICODETEXT, hMemW) != ਨਿਰਾ)
		    hMemW = ਨਿਰਾ;
	    }
	    // Always use CF_TEXT.  On Win98 Notepad won't obtain the
	    // CF_UNICODETEXT text, only CF_TEXT.
	    ਸੈੱਟ_Clipboard_ਅੰਕੜੇ(CF_ਪਾਠ, hMem);
	    hMem = 0;
	}
	CloseClipboard();
    }

    vim_free(str);
    // Free any allocations we didn't give to the clipboard:
    ਜੇ (hMemRaw)
	GlobalFree(hMemRaw);
    ਜੇ (hMem)
	GlobalFree(hMem);
    ਜੇ (hMemW)
	GlobalFree(hMemW);
    ਜੇ (hMemVim)
	GlobalFree(hMemVim);
}

#ਜੇ_ਅੰਤ // FEAT_CLIPBOARD

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
enc_ਮੰਜ਼ਿਲ_utf16(char_u *str, ਪੂਰਨ *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = ਨਿਰਾ;
    ਪੂਰਨ		len_loc;
    ਪੂਰਨ		ਲੰਮਾਈ;

    ਜੇ (lenp == ਨਿਰਾ)
    {
	len_loc = (ਪੂਰਨ)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    ਜੇ (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &ਲੰਮਾਈ);
    }
    ਨਹੀਂਤਾਂ
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	ਜੇ (convert_ਤਿਆਰੀ(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    ਜੇ (str == ਨਿਰਾ)
		ਵਾਪਸ ਨਿਰਾ;
	}
	convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);

	ਲੰਮਾਈ = utf8_ਮੰਜ਼ਿਲ_utf16(str, *lenp, ਨਿਰਾ, ਨਿਰਾ);
	ret = ALLOC_MULT(WCHAR, ਲੰਮਾਈ + 1);
	ਜੇ (ret != ਨਿਰਾ)
	{
	    utf8_ਮੰਜ਼ਿਲ_utf16(str, *lenp, (short_u *)ret, ਨਿਰਾ);
	    ret[ਲੰਮਾਈ] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = ਲੰਮਾਈ;
    ਵਾਪਸ (short_u *)ret;
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
utf16_ਮੰਜ਼ਿਲ_enc(short_u *str, ਪੂਰਨ *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = ਨਿਰਾ, *enc_str = ਨਿਰਾ;
    ਪੂਰਨ		len_loc;

    ਜੇ (lenp == ਨਿਰਾ)
    {
	len_loc = (ਪੂਰਨ)wcslen(str) + 1;
	lenp = &len_loc;
    }

    ਜੇ (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	ਪੂਰਨ ਲੰਮਾਈ;

	Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &ਲੰਮਾਈ, 0, 0);
	*lenp = ਲੰਮਾਈ;
	ਵਾਪਸ enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_ਮੰਜ਼ਿਲ_utf8(str, *lenp == 0 ? 1 : *lenp, ਨਿਰਾ));
    ਜੇ (utf8_str != ਨਿਰਾ)
    {
	*lenp = utf16_ਮੰਜ਼ਿਲ_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_ਤਿਆਰੀ(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	ਜੇ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	ਨਹੀਂਤਾਂ
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);
    }

    ਵਾਪਸ enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    ਖਾਲੀ
acp_ਮੰਜ਼ਿਲ_enc(
    char_u	*str,
    ਪੂਰਨ		str_ਮਾਪ,
    char_u	**ਬਾਹਰਲਾ,
    ਪੂਰਨ		*outlen)

{
    LPWSTR	widestr;

    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char_alloc(GetACP(), 0, (LPCSTR)str, str_ਮਾਪ,
							    &widestr, outlen);
    ਜੇ (widestr == ਨਿਰਾ)
	ਵਾਪਸ;
    ++*outlen;	// Include the 0 after the string
    *ਬਾਹਰਲਾ = utf16_ਮੰਜ਼ਿਲ_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    ਖਾਲੀ
enc_ਮੰਜ਼ਿਲ_acp(
    char_u	*str,
    ਪੂਰਨ		str_ਮਾਪ,
    char_u	**ਬਾਹਰਲਾ,
    ਪੂਰਨ		*outlen)

{
    LPWSTR	widestr;
    ਪੂਰਨ		len = str_ਮਾਪ;

    widestr = (WCHAR *)enc_ਮੰਜ਼ਿਲ_utf16(str, &len);
    ਜੇ (widestr == ਨਿਰਾ)
	ਵਾਪਸ;
    Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)ਬਾਹਰਲਾ, outlen, 0, 0);
    vim_free(widestr);
}
