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

#ಅನ್ತರ್ಭಾವ "vim.h"

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
    ಪೂರ್ಣಾಙ್ಕ
utf8_to_utf16(char_u *instr, ಪೂರ್ಣಾಙ್ಕ inlen, short_u *outstr, ಪೂರ್ಣಾಙ್ಕ *unconvlenp)
{
    ಪೂರ್ಣಾಙ್ಕ		outlen = 0;
    char_u	*p = instr;
    ಪೂರ್ಣಾಙ್ಕ		todo = inlen;
    ಪೂರ್ಣಾಙ್ಕ		l;
    ಪೂರ್ಣಾಙ್ಕ		ch;

    ಯಾವತ್ (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	ಯದಿ (l > todo)
	{
	    // Return length of incomplete sequence.
	    ಯದಿ (unconvlenp != ರಿಕ್ತ)
		*unconvlenp = todo;
	    ಭಙ್ಗ;
	}

	ch = utf_ptr2char(p);
	ಯದಿ (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    ಯದಿ (outstr != ರಿಕ್ತ)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (outstr != ರಿಕ್ತ)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    ಪ್ರತ್ಯಾಗಮ outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf16_to_utf8(short_u *instr, ಪೂರ್ಣಾಙ್ಕ inlen, char_u *outstr)
{
    ಪೂರ್ಣಾಙ್ಕ		outlen = 0;
    ಪೂರ್ಣಾಙ್ಕ		todo = inlen;
    short_u	*p = instr;
    ಪೂರ್ಣಾಙ್ಕ		l;
    ಪೂರ್ಣಾಙ್ಕ		ch, ch2;

    ಯಾವತ್ (todo > 0)
    {
	ch = *p;
	ಯದಿ (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    ಯದಿ (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	ಯದಿ (outstr != ರಿಕ್ತ)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	ಅನ್ಯಥಾ
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    ಪ್ರತ್ಯಾಗಮ outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    ಶೂನ್ಯ
MultiByteToWideChar_alloc(UINT cp, DWORD flags,
	LPCSTR in, ಪೂರ್ಣಾಙ್ಕ inlen,
	LPWSTR *out, ಪೂರ್ಣಾಙ್ಕ *outlen)
{
    *outlen = MultiByteToWideChar(cp, flags, in, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *out = ALLOC_MULT(WCHAR, *outlen + 1);
    ಯದಿ (*out == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    MultiByteToWideChar(cp, flags, in, inlen, *out, *outlen);
    (*out)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    ಶೂನ್ಯ
WideCharToMultiByte_alloc(UINT cp, DWORD flags,
	LPCWSTR in, ಪೂರ್ಣಾಙ್ಕ inlen,
	LPSTR *out, ಪೂರ್ಣಾಙ್ಕ *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = WideCharToMultiByte(cp, flags, in, inlen, ರಿಕ್ತ, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *out = alloc(*outlen + 1);
    ಯದಿ (*out == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    WideCharToMultiByte(cp, flags, in, inlen, *out, *outlen, def, useddef);
    (*out)[*outlen] = 0;
}


#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

    ಶೂನ್ಯ
win_clip_init(ಶೂನ್ಯ)
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ type;		// MCHAR, MBLOCK or MLINE
    ಪೂರ್ಣಾಙ್ಕ txtlen;		// length of CF_TEXT in bytes
    ಪೂರ್ಣಾಙ್ಕ ucslen;		// length of CF_UNICODETEXT in words
    ಪೂರ್ಣಾಙ್ಕ rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    ಪೂರ್ಣಾಙ್ಕ
clip_mch_own_selection(Clipboard_T *cbd UNUSED)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    ಶೂನ್ಯ
clip_mch_lose_selection(Clipboard_T *cbd UNUSED)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ char_u *
crnl_to_nl(ಧ್ರುವ char_u *str, ಪೂರ್ಣಾಙ್ಕ *size)
{
    ಪೂರ್ಣಾಙ್ಕ		pos = 0;
    ಪೂರ್ಣಾಙ್ಕ		str_len = *size;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    ಯದಿ (ret != ರಿಕ್ತ)
    {
	retp = ret;
	ಪ್ರತಿ (pos = 0; pos < str_len; ++pos)
	{
	    ಯದಿ (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*size);
	    }
	    *retp++ = str[pos];
	}
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vim_open_clipboard(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ delay = 10;

    ಯಾವತ್ (!OpenClipboard(ರಿಕ್ತ))
    {
	ಯದಿ (delay > 500)
	    ಪ್ರತ್ಯಾಗಮ FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಶೂನ್ಯ
clip_mch_request_selection(Clipboard_T *cbd)
{
    VimClipType_t	metadata = { -1, -1, -1, -1 };
    HGLOBAL		hMem = ರಿಕ್ತ;
    char_u		*str = ರಿಕ್ತ;
    char_u		*to_free = ರಿಕ್ತ;
    HGLOBAL		rawh = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			str_size = 0;
    ಪೂರ್ಣಾಙ್ಕ			maxlen;
    size_t		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    ಯದಿ (!vim_open_clipboard())
	ಪ್ರತ್ಯಾಗಮ;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    ಯದಿ (IsClipboardFormatAvailable(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	ಯದಿ ((meta_h = GetClipboardData(cbd->format)) != ರಿಕ್ತ
		&& (meta_p = (VimClipType_t *)GlobalLock(meta_h)) != ರಿಕ್ತ)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = ಪರಿಮಾಣ(VimClipType_t);
	    ಯದಿ (GlobalSize(meta_h) < n)
		n = GlobalSize(meta_h);
	    memcpy(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    ಯದಿ (IsClipboardFormatAvailable(cbd->format_raw)
				      && metadata.rawlen > (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	ಯದಿ ((rawh = GetClipboardData(cbd->format_raw)) != ರಿಕ್ತ)
	{
	    char_u	*rawp;

	    rawp = (char_u *)GlobalLock(rawh);
	    ಯದಿ (rawp != ರಿಕ್ತ && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_size = (ಪೂರ್ಣಾಙ್ಕ)(metadata.rawlen - n);
	    }
	    ಅನ್ಯಥಾ
	    {
		GlobalUnlock(rawh);
		rawh = ರಿಕ್ತ;
	    }
	}
    }
    ಯದಿ (str == ರಿಕ್ತ)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	ಯದಿ (IsClipboardFormatAvailable(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    ಯದಿ ((hMemW = GetClipboardData(CF_UNICODETEXT)) != ರಿಕ್ತ)
	    {
		WCHAR *hMemWstr = (WCHAR *)GlobalLock(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (ಪೂರ್ಣಾಙ್ಕ)(GlobalSize(hMemW) / ಪರಿಮಾಣ(WCHAR));
		ಯದಿ (metadata.ucslen >= 0)
		{
		    ಯದಿ (metadata.ucslen > maxlen)
			str_size = maxlen;
		    ಅನ್ಯಥಾ
			str_size = metadata.ucslen;
		}
		ಅನ್ಯಥಾ
		{
		    ಪ್ರತಿ (str_size = 0; str_size < maxlen; ++str_size)
			ಯದಿ (hMemWstr[str_size] == NUL)
			    ಭಙ್ಗ;
		}
		to_free = str = utf16_to_enc((short_u *)hMemWstr, &str_size);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	ಅನ್ಯಥಾ ಯದಿ (IsClipboardFormatAvailable(CF_TEXT))
	{
	    ಯದಿ ((hMem = GetClipboardData(CF_TEXT)) != ರಿಕ್ತ)
	    {
		str = (char_u *)GlobalLock(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (ಪೂರ್ಣಾಙ್ಕ)GlobalSize(hMem);
		ಯದಿ (metadata.txtlen >= 0)
		{
		    ಯದಿ (metadata.txtlen > maxlen)
			str_size = maxlen;
		    ಅನ್ಯಥಾ
			str_size = metadata.txtlen;
		}
		ಅನ್ಯಥಾ
		{
		    ಪ್ರತಿ (str_size = 0; str_size < maxlen; ++str_size)
			ಯದಿ (str[str_size] == NUL)
			    ಭಙ್ಗ;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_to_enc(str, str_size, &to_free, &maxlen);
		ಯದಿ (to_free != ರಿಕ್ತ)
		{
		    str_size = maxlen;
		    str = to_free;
		}
	    }
	}
    }

    ಯದಿ (str != ರಿಕ್ತ && metadata.txtlen != 0)
    {
	char_u *temp_clipboard;

	// If the type is not known detect it.
	ಯದಿ (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	temp_clipboard = crnl_to_nl(str, &str_size);
	ಯದಿ (temp_clipboard != ರಿಕ್ತ)
	{
	    clip_yank_selection(metadata.type, temp_clipboard, str_size, cbd);
	    vim_free(temp_clipboard);
	}
    }

    // unlock the global object
    ಯದಿ (hMem != ರಿಕ್ತ)
	GlobalUnlock(hMem);
    ಯದಿ (rawh != ರಿಕ್ತ)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(to_free);
}

/*
 * Send the current selection to the clipboard.
 */
    ಶೂನ್ಯ
clip_mch_set_selection(Clipboard_T *cbd)
{
    char_u		*str = ರಿಕ್ತ;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = ರಿಕ್ತ;
    HGLOBAL		hMem = ರಿಕ್ತ;
    HGLOBAL		hMemVim = ರಿಕ್ತ;
    HGLOBAL		hMemW = ರಿಕ್ತ;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    ಯದಿ (metadata.type < 0)
	ಪ್ರತ್ಯಾಗಮ;
    metadata.txtlen = (ಪೂರ್ಣಾಙ್ಕ)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (ಪೂರ್ಣಾಙ್ಕ)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)GlobalLock(hMemRaw);
	ಯದಿ (lpszMemRaw != ರಿಕ್ತ)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    memcpy(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	ಅನ್ಯಥಾ
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*out;
	ಪೂರ್ಣಾಙ್ಕ		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	out = (WCHAR *)enc_to_utf16(str, &len);
	ಯದಿ (out != ರಿಕ್ತ)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = WideCharToMultiByte(GetACP(), 0, out, len,
							       ರಿಕ್ತ, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    ಯದಿ (str == ರಿಕ್ತ)
	    {
		vim_free(out);
		ಪ್ರತ್ಯಾಗಮ;		// out of memory
	    }
	    WideCharToMultiByte(GetACP(), 0, out, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * ಪರಿಮಾಣ(WCHAR));
	    lpszMemW = (WCHAR *)GlobalLock(hMemW);
	    ಯದಿ (lpszMemW != ರಿಕ್ತ)
	    {
		memcpy(lpszMemW, out, len * ಪರಿಮಾಣ(WCHAR));
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

	ಯದಿ (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = ರಿಕ್ತ;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       ಪರಿಮಾಣ(VimClipType_t));
	lpszMemVim = (VimClipType_t *)GlobalLock(hMemVim);
	memcpy(lpszMemVim, &metadata, ಪರಿಮಾಣ(metadata));
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
    ಯದಿ (vim_open_clipboard())
    {
	ಯದಿ (EmptyClipboard())
	{
	    SetClipboardData(cbd->format, hMemVim);
	    hMemVim = 0;
	    ಯದಿ (hMemW != ರಿಕ್ತ)
	    {
		ಯದಿ (SetClipboardData(CF_UNICODETEXT, hMemW) != ರಿಕ್ತ)
		    hMemW = ರಿಕ್ತ;
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
    ಯದಿ (hMemRaw)
	GlobalFree(hMemRaw);
    ಯದಿ (hMem)
	GlobalFree(hMem);
    ಯದಿ (hMemW)
	GlobalFree(hMemW);
    ಯದಿ (hMemVim)
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
enc_to_utf16(char_u *str, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len_loc;
    ಪೂರ್ಣಾಙ್ಕ		length;

    ಯದಿ (lenp == ರಿಕ್ತ)
    {
	len_loc = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    ಯದಿ (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	MultiByteToWideChar_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &length);
    }
    ಅನ್ಯಥಾ
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	ಯದಿ (convert_setup(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    ಯದಿ (str == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	convert_setup(&conv, ರಿಕ್ತ, ರಿಕ್ತ);

	length = utf8_to_utf16(str, *lenp, ರಿಕ್ತ, ರಿಕ್ತ);
	ret = ALLOC_MULT(WCHAR, length + 1);
	ಯದಿ (ret != ರಿಕ್ತ)
	{
	    utf8_to_utf16(str, *lenp, (short_u *)ret, ರಿಕ್ತ);
	    ret[length] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = length;
    ಪ್ರತ್ಯಾಗಮ (short_u *)ret;
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
utf16_to_enc(short_u *str, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = ರಿಕ್ತ, *enc_str = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len_loc;

    ಯದಿ (lenp == ರಿಕ್ತ)
    {
	len_loc = (ಪೂರ್ಣಾಙ್ಕ)wcslen(str) + 1;
	lenp = &len_loc;
    }

    ಯದಿ (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	ಪೂರ್ಣಾಙ್ಕ length;

	WideCharToMultiByte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &length, 0, 0);
	*lenp = length;
	ಪ್ರತ್ಯಾಗಮ enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_to_utf8(str, *lenp == 0 ? 1 : *lenp, ರಿಕ್ತ));
    ಯದಿ (utf8_str != ರಿಕ್ತ)
    {
	*lenp = utf16_to_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	ಯದಿ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	ಅನ್ಯಥಾ
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_setup(&conv, ರಿಕ್ತ, ರಿಕ್ತ);
    }

    ಪ್ರತ್ಯಾಗಮ enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    ಶೂನ್ಯ
acp_to_enc(
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		str_size,
    char_u	**out,
    ಪೂರ್ಣಾಙ್ಕ		*outlen)

{
    LPWSTR	widestr;

    MultiByteToWideChar_alloc(GetACP(), 0, (LPCSTR)str, str_size,
							    &widestr, outlen);
    ಯದಿ (widestr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ++*outlen;	// Include the 0 after the string
    *out = utf16_to_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    ಶೂನ್ಯ
enc_to_acp(
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		str_size,
    char_u	**out,
    ಪೂರ್ಣಾಙ್ಕ		*outlen)

{
    LPWSTR	widestr;
    ಪೂರ್ಣಾಙ್ಕ		len = str_size;

    widestr = (WCHAR *)enc_to_utf16(str, &len);
    ಯದಿ (widestr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    WideCharToMultiByte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)out, outlen, 0, 0);
    vim_free(widestr);
}
