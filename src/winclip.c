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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_ಸರಾಸರಿ
# include <windows.h>
# include "winclip.pro"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Convert an UTF-8 string to UTF-16.
 * "instr[inlen]" is the input.  "inlen" is in bytes.
 * When "outstr" is NULL only return the number of UTF-16 words produced.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Returns the number of UTF-16 words produced.
 */
    ಪೂರ್ಣಾಂಕ
utf8_ಗಮ್ಯ_utf16(char_u *instr, ಪೂರ್ಣಾಂಕ inlen, short_u *outstr, ಪೂರ್ಣಾಂಕ *unconvlenp)
{
    ಪೂರ್ಣಾಂಕ		outlen = 0;
    char_u	*p = instr;
    ಪೂರ್ಣಾಂಕ		todo = inlen;
    ಪೂರ್ಣಾಂಕ		l;
    ಪೂರ್ಣಾಂಕ		ch;

    ಆಗ (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	ಆದರೆ (l > todo)
	{
	    // Return length of incomplete sequence.
	    ಆದರೆ (unconvlenp != ಖಾಲಿ)
		*unconvlenp = todo;
	    ನಿಲ್ಲಿಸು;
	}

	ch = utf_ptr2char(p);
	ಆದರೆ (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    ಆದರೆ (outstr != ಖಾಲಿ)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (outstr != ಖಾಲಿ)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    ಹಿಂತಿರುಗಿ outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    ಪೂರ್ಣಾಂಕ
utf16_ಗಮ್ಯ_utf8(short_u *instr, ಪೂರ್ಣಾಂಕ inlen, char_u *outstr)
{
    ಪೂರ್ಣಾಂಕ		outlen = 0;
    ಪೂರ್ಣಾಂಕ		todo = inlen;
    short_u	*p = instr;
    ಪೂರ್ಣಾಂಕ		l;
    ಪೂರ್ಣಾಂಕ		ch, ch2;

    ಆಗ (todo > 0)
    {
	ch = *p;
	ಆದರೆ (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    ಆದರೆ (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	ಆದರೆ (outstr != ಖಾಲಿ)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    ಹಿಂತಿರುಗಿ outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    ಶೂನ್ಯ
Multi_Byte_ಗಮ್ಯ_Wide_Char_alloc(UINT cp, DWORD flags,
	LPCSTR ಒಳ, ಪೂರ್ಣಾಂಕ inlen,
	LPWSTR *ಹೊರಹೋಗು, ಪೂರ್ಣಾಂಕ *outlen)
{
    *outlen = Multi_Byte_ಗಮ್ಯ_Wide_Char(cp, flags, ಒಳ, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *ಹೊರಹೋಗು = ALLOC_MULT(WCHAR, *outlen + 1);
    ಆದರೆ (*ಹೊರಹೋಗು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    Multi_Byte_ಗಮ್ಯ_Wide_Char(cp, flags, ಒಳ, inlen, *ಹೊರಹೋಗು, *outlen);
    (*ಹೊರಹೋಗು)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    ಶೂನ್ಯ
Wide_Char_ಗಮ್ಯ_Multi_Byte_alloc(UINT cp, DWORD flags,
	LPCWSTR ಒಳ, ಪೂರ್ಣಾಂಕ inlen,
	LPSTR *ಹೊರಹೋಗು, ಪೂರ್ಣಾಂಕ *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = Wide_Char_ಗಮ್ಯ_Multi_Byte(cp, flags, ಒಳ, inlen, ಖಾಲಿ, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *ಹೊರಹೋಗು = alloc(*outlen + 1);
    ಆದರೆ (*ಹೊರಹೋಗು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    Wide_Char_ಗಮ್ಯ_Multi_Byte(cp, flags, ಒಳ, inlen, *ಹೊರಹೋಗು, *outlen, def, useddef);
    (*ಹೊರಹೋಗು)[*outlen] = 0;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
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
    clip_ನಕ್ಷತ್ರ.format = ರಿಜಿಸ್ಟರ್_Clipboard_Format("VimClipboard2");
    clip_ನಕ್ಷತ್ರ.format_raw = ರಿಜಿಸ್ಟರ್_Clipboard_Format("VimRawBytes");
}

// Type used for the clipboard type of Vim's data.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ type;		// MCHAR, MBLOCK or MLINE
    ಪೂರ್ಣಾಂಕ txtlen;		// length of CF_TEXT in bytes
    ಪೂರ್ಣಾಂಕ ucslen;		// length of CF_UNICODETEXT in words
    ಪೂರ್ಣಾಂಕ rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    ಪೂರ್ಣಾಂಕ
clip_mch_own_selection(Clipboard_T *cbd ಬಳಸದ)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    ಶೂನ್ಯ
clip_mch_lose_selection(Clipboard_T *cbd ಬಳಸದ)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ char_u *
crnl_ಗಮ್ಯ_nl(ಸ್ಥಿರಾಂಕ char_u *str, ಪೂರ್ಣಾಂಕ *ಗಾತ್ರ_2)
{
    ಪೂರ್ಣಾಂಕ		pos = 0;
    ಪೂರ್ಣಾಂಕ		str_len = *ಗಾತ್ರ_2;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    ಆದರೆ (ret != ಖಾಲಿ)
    {
	retp = ret;
	ಗಾಗಿ (pos = 0; pos < str_len; ++pos)
	{
	    ಆದರೆ (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*ಗಾತ್ರ_2);
	    }
	    *retp++ = str[pos];
	}
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vim_open_clipboard(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ delay = 10;

    ಆಗ (!OpenClipboard(ಖಾಲಿ))
    {
	ಆದರೆ (delay > 500)
	    ಹಿಂತಿರುಗಿ FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    ಹಿಂತಿರುಗಿ TRUE;
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
    HGLOBAL		hMem = ಖಾಲಿ;
    char_u		*str = ಖಾಲಿ;
    char_u		*ಗಮ್ಯ_free = ಖಾಲಿ;
    HGLOBAL		rawh = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			str_ಗಾತ್ರ = 0;
    ಪೂರ್ಣಾಂಕ			maxlen;
    size_t		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    ಆದರೆ (!vim_open_clipboard())
	ಹಿಂತಿರುಗಿ;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    ಆದರೆ (Is_Clipboard_Format_ಲಭ್ಯ(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	ಆದರೆ ((meta_h = Get_Clipboard_ದತ್ತಾಂಶ(cbd->format)) != ಖಾಲಿ
		&& (meta_p = (VimClipType_t *)Global_ಬೀಗ(meta_h)) != ಖಾಲಿ)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = ಗಾತ್ರ(VimClipType_t);
	    ಆದರೆ (Global_ಗಾತ್ರ(meta_h) < n)
		n = Global_ಗಾತ್ರ(meta_h);
	    ಸ್ಮರಣೆ_ನಕಲು(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    ಆದರೆ (Is_Clipboard_Format_ಲಭ್ಯ(cbd->format_raw)
				      && metadata.rawlen > (ಪೂರ್ಣಾಂಕ)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	ಆದರೆ ((rawh = Get_Clipboard_ದತ್ತಾಂಶ(cbd->format_raw)) != ಖಾಲಿ)
	{
	    char_u	*rawp;

	    rawp = (char_u *)Global_ಬೀಗ(rawh);
	    ಆದರೆ (rawp != ಖಾಲಿ && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_ಗಾತ್ರ = (ಪೂರ್ಣಾಂಕ)(metadata.rawlen - n);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		GlobalUnlock(rawh);
		rawh = ಖಾಲಿ;
	    }
	}
    }
    ಆದರೆ (str == ಖಾಲಿ)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	ಆದರೆ (Is_Clipboard_Format_ಲಭ್ಯ(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    ಆದರೆ ((hMemW = Get_Clipboard_ದತ್ತಾಂಶ(CF_UNICODETEXT)) != ಖಾಲಿ)
	    {
		WCHAR *hMemWstr = (WCHAR *)Global_ಬೀಗ(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (ಪೂರ್ಣಾಂಕ)(Global_ಗಾತ್ರ(hMemW) / ಗಾತ್ರ(WCHAR));
		ಆದರೆ (metadata.ucslen >= 0)
		{
		    ಆದರೆ (metadata.ucslen > maxlen)
			str_ಗಾತ್ರ = maxlen;
		    ಇಲ್ಲದಿದ್ದರೆ
			str_ಗಾತ್ರ = metadata.ucslen;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಗಾಗಿ (str_ಗಾತ್ರ = 0; str_ಗಾತ್ರ < maxlen; ++str_ಗಾತ್ರ)
			ಆದರೆ (hMemWstr[str_ಗಾತ್ರ] == NUL)
			    ನಿಲ್ಲಿಸು;
		}
		ಗಮ್ಯ_free = str = utf16_ಗಮ್ಯ_enc((short_u *)hMemWstr, &str_ಗಾತ್ರ);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Is_Clipboard_Format_ಲಭ್ಯ(CF_ಪಠ್ಯ))
	{
	    ಆದರೆ ((hMem = Get_Clipboard_ದತ್ತಾಂಶ(CF_ಪಠ್ಯ)) != ಖಾಲಿ)
	    {
		str = (char_u *)Global_ಬೀಗ(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (ಪೂರ್ಣಾಂಕ)Global_ಗಾತ್ರ(hMem);
		ಆದರೆ (metadata.txtlen >= 0)
		{
		    ಆದರೆ (metadata.txtlen > maxlen)
			str_ಗಾತ್ರ = maxlen;
		    ಇಲ್ಲದಿದ್ದರೆ
			str_ಗಾತ್ರ = metadata.txtlen;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಗಾಗಿ (str_ಗಾತ್ರ = 0; str_ಗಾತ್ರ < maxlen; ++str_ಗಾತ್ರ)
			ಆದರೆ (str[str_ಗಾತ್ರ] == NUL)
			    ನಿಲ್ಲಿಸು;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_ಗಮ್ಯ_enc(str, str_ಗಾತ್ರ, &ಗಮ್ಯ_free, &maxlen);
		ಆದರೆ (ಗಮ್ಯ_free != ಖಾಲಿ)
		{
		    str_ಗಾತ್ರ = maxlen;
		    str = ಗಮ್ಯ_free;
		}
	    }
	}
    }

    ಆದರೆ (str != ಖಾಲಿ && metadata.txtlen != 0)
    {
	char_u *ತಾತ್ಕಾಲಿಕ_clipboard;

	// If the type is not known detect it.
	ಆದರೆ (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	ತಾತ್ಕಾಲಿಕ_clipboard = crnl_ಗಮ್ಯ_nl(str, &str_ಗಾತ್ರ);
	ಆದರೆ (ತಾತ್ಕಾಲಿಕ_clipboard != ಖಾಲಿ)
	{
	    clip_yank_selection(metadata.type, ತಾತ್ಕಾಲಿಕ_clipboard, str_ಗಾತ್ರ, cbd);
	    vim_free(ತಾತ್ಕಾಲಿಕ_clipboard);
	}
    }

    // unlock the global object
    ಆದರೆ (hMem != ಖಾಲಿ)
	GlobalUnlock(hMem);
    ಆದರೆ (rawh != ಖಾಲಿ)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(ಗಮ್ಯ_free);
}

/*
 * Send the current selection to the clipboard.
 */
    ಶೂನ್ಯ
clip_mch_ಗಣ_selection(Clipboard_T *cbd)
{
    char_u		*str = ಖಾಲಿ;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = ಖಾಲಿ;
    HGLOBAL		hMem = ಖಾಲಿ;
    HGLOBAL		hMemVim = ಖಾಲಿ;
    HGLOBAL		hMemW = ಖಾಲಿ;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    ಆದರೆ (metadata.type < 0)
	ಹಿಂತಿರುಗಿ;
    metadata.txtlen = (ಪೂರ್ಣಾಂಕ)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (ಪೂರ್ಣಾಂಕ)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)Global_ಬೀಗ(hMemRaw);
	ಆದರೆ (lpszMemRaw != ಖಾಲಿ)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    ಸ್ಮರಣೆ_ನಕಲು(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*ಹೊರಹೋಗು;
	ಪೂರ್ಣಾಂಕ		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	ಹೊರಹೋಗು = (WCHAR *)enc_ಗಮ್ಯ_utf16(str, &len);
	ಆದರೆ (ಹೊರಹೋಗು != ಖಾಲಿ)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = Wide_Char_ಗಮ್ಯ_Multi_Byte(GetACP(), 0, ಹೊರಹೋಗು, len,
							       ಖಾಲಿ, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    ಆದರೆ (str == ಖಾಲಿ)
	    {
		vim_free(ಹೊರಹೋಗು);
		ಹಿಂತಿರುಗಿ;		// out of memory
	    }
	    Wide_Char_ಗಮ್ಯ_Multi_Byte(GetACP(), 0, ಹೊರಹೋಗು, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * ಗಾತ್ರ(WCHAR));
	    lpszMemW = (WCHAR *)Global_ಬೀಗ(hMemW);
	    ಆದರೆ (lpszMemW != ಖಾಲಿ)
	    {
		ಸ್ಮರಣೆ_ನಕಲು(lpszMemW, ಹೊರಹೋಗು, len * ಗಾತ್ರ(WCHAR));
		lpszMemW[len] = NUL;
		GlobalUnlock(hMemW);
	    }
	    vim_free(ಹೊರಹೋಗು);
	    metadata.ucslen = len;
	}
    }

    // Allocate memory for the text, add one NUL byte to terminate the string.
    hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, metadata.txtlen + 1);
    {
	LPSTR lpszMem = (LPSTR)Global_ಬೀಗ(hMem);

	ಆದರೆ (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = ಖಾಲಿ;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       ಗಾತ್ರ(VimClipType_t));
	lpszMemVim = (VimClipType_t *)Global_ಬೀಗ(hMemVim);
	ಸ್ಮರಣೆ_ನಕಲು(lpszMemVim, &metadata, ಗಾತ್ರ(metadata));
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
    ಆದರೆ (vim_open_clipboard())
    {
	ಆದರೆ (ಖಾಲಿಗಳು_Clipboard())
	{
	    ಗಣ_Clipboard_ದತ್ತಾಂಶ(cbd->format, hMemVim);
	    hMemVim = 0;
	    ಆದರೆ (hMemW != ಖಾಲಿ)
	    {
		ಆದರೆ (ಗಣ_Clipboard_ದತ್ತಾಂಶ(CF_UNICODETEXT, hMemW) != ಖಾಲಿ)
		    hMemW = ಖಾಲಿ;
	    }
	    // Always use CF_TEXT.  On Win98 Notepad won't obtain the
	    // CF_UNICODETEXT text, only CF_TEXT.
	    ಗಣ_Clipboard_ದತ್ತಾಂಶ(CF_ಪಠ್ಯ, hMem);
	    hMem = 0;
	}
	CloseClipboard();
    }

    vim_free(str);
    // Free any allocations we didn't give to the clipboard:
    ಆದರೆ (hMemRaw)
	GlobalFree(hMemRaw);
    ಆದರೆ (hMem)
	GlobalFree(hMem);
    ಆದರೆ (hMemW)
	GlobalFree(hMemW);
    ಆದರೆ (hMemVim)
	GlobalFree(hMemVim);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_CLIPBOARD

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
enc_ಗಮ್ಯ_utf16(char_u *str, ಪೂರ್ಣಾಂಕ *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len_loc;
    ಪೂರ್ಣಾಂಕ		ಉದ್ದಳತೆ;

    ಆದರೆ (lenp == ಖಾಲಿ)
    {
	len_loc = (ಪೂರ್ಣಾಂಕ)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    ಆದರೆ (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	Multi_Byte_ಗಮ್ಯ_Wide_Char_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &ಉದ್ದಳತೆ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	ಆದರೆ (convert_ಸಿದ್ಧತೆ(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    ಆದರೆ (str == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);

	ಉದ್ದಳತೆ = utf8_ಗಮ್ಯ_utf16(str, *lenp, ಖಾಲಿ, ಖಾಲಿ);
	ret = ALLOC_MULT(WCHAR, ಉದ್ದಳತೆ + 1);
	ಆದರೆ (ret != ಖಾಲಿ)
	{
	    utf8_ಗಮ್ಯ_utf16(str, *lenp, (short_u *)ret, ಖಾಲಿ);
	    ret[ಉದ್ದಳತೆ] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = ಉದ್ದಳತೆ;
    ಹಿಂತಿರುಗಿ (short_u *)ret;
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
utf16_ಗಮ್ಯ_enc(short_u *str, ಪೂರ್ಣಾಂಕ *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = ಖಾಲಿ, *enc_str = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len_loc;

    ಆದರೆ (lenp == ಖಾಲಿ)
    {
	len_loc = (ಪೂರ್ಣಾಂಕ)wcslen(str) + 1;
	lenp = &len_loc;
    }

    ಆದರೆ (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ;

	Wide_Char_ಗಮ್ಯ_Multi_Byte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &ಉದ್ದಳತೆ, 0, 0);
	*lenp = ಉದ್ದಳತೆ;
	ಹಿಂತಿರುಗಿ enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_ಗಮ್ಯ_utf8(str, *lenp == 0 ? 1 : *lenp, ಖಾಲಿ));
    ಆದರೆ (utf8_str != ಖಾಲಿ)
    {
	*lenp = utf16_ಗಮ್ಯ_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_ಸಿದ್ಧತೆ(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	ಆದರೆ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);
    }

    ಹಿಂತಿರುಗಿ enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    ಶೂನ್ಯ
acp_ಗಮ್ಯ_enc(
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		str_ಗಾತ್ರ,
    char_u	**ಹೊರಹೋಗು,
    ಪೂರ್ಣಾಂಕ		*outlen)

{
    LPWSTR	widestr;

    Multi_Byte_ಗಮ್ಯ_Wide_Char_alloc(GetACP(), 0, (LPCSTR)str, str_ಗಾತ್ರ,
							    &widestr, outlen);
    ಆದರೆ (widestr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ++*outlen;	// Include the 0 after the string
    *ಹೊರಹೋಗು = utf16_ಗಮ್ಯ_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    ಶೂನ್ಯ
enc_ಗಮ್ಯ_acp(
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		str_ಗಾತ್ರ,
    char_u	**ಹೊರಹೋಗು,
    ಪೂರ್ಣಾಂಕ		*outlen)

{
    LPWSTR	widestr;
    ಪೂರ್ಣಾಂಕ		len = str_ಗಾತ್ರ;

    widestr = (WCHAR *)enc_ಗಮ್ಯ_utf16(str, &len);
    ಆದರೆ (widestr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    Wide_Char_ಗಮ್ಯ_Multi_Byte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)ಹೊರಹೋಗು, outlen, 0, 0);
    vim_free(widestr);
}
