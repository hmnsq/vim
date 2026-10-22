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

#अन्तर्भाव "vim.h"

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
    पूर्णाङ्क
utf8_to_utf16(char_u *instr, पूर्णाङ्क inlen, short_u *outstr, पूर्णाङ्क *unconvlenp)
{
    पूर्णाङ्क		outlen = 0;
    char_u	*p = instr;
    पूर्णाङ्क		todo = inlen;
    पूर्णाङ्क		l;
    पूर्णाङ्क		ch;

    यावत् (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	यदि (l > todo)
	{
	    // Return length of incomplete sequence.
	    यदि (unconvlenp != रिक्त)
		*unconvlenp = todo;
	    भङ्ग;
	}

	ch = utf_ptr2char(p);
	यदि (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    यदि (outstr != रिक्त)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	अन्यथा यदि (outstr != रिक्त)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    प्रत्यागम outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    पूर्णाङ्क
utf16_to_utf8(short_u *instr, पूर्णाङ्क inlen, char_u *outstr)
{
    पूर्णाङ्क		outlen = 0;
    पूर्णाङ्क		todo = inlen;
    short_u	*p = instr;
    पूर्णाङ्क		l;
    पूर्णाङ्क		ch, ch2;

    यावत् (todo > 0)
    {
	ch = *p;
	यदि (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    यदि (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	यदि (outstr != रिक्त)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	अन्यथा
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    प्रत्यागम outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    शून्य
MultiByteToWideChar_alloc(UINT cp, DWORD flags,
	LPCSTR in, पूर्णाङ्क inlen,
	LPWSTR *out, पूर्णाङ्क *outlen)
{
    *outlen = MultiByteToWideChar(cp, flags, in, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *out = ALLOC_MULT(WCHAR, *outlen + 1);
    यदि (*out == रिक्त)
	प्रत्यागम;

    MultiByteToWideChar(cp, flags, in, inlen, *out, *outlen);
    (*out)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    शून्य
WideCharToMultiByte_alloc(UINT cp, DWORD flags,
	LPCWSTR in, पूर्णाङ्क inlen,
	LPSTR *out, पूर्णाङ्क *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = WideCharToMultiByte(cp, flags, in, inlen, रिक्त, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *out = alloc(*outlen + 1);
    यदि (*out == रिक्त)
	प्रत्यागम;
    WideCharToMultiByte(cp, flags, in, inlen, *out, *outlen, def, useddef);
    (*out)[*outlen] = 0;
}


#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

    शून्य
win_clip_init(शून्य)
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
प्रकारनिर्देश संरचना
{
    पूर्णाङ्क type;		// MCHAR, MBLOCK or MLINE
    पूर्णाङ्क txtlen;		// length of CF_TEXT in bytes
    पूर्णाङ्क ucslen;		// length of CF_UNICODETEXT in words
    पूर्णाङ्क rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    पूर्णाङ्क
clip_mch_own_selection(Clipboard_T *cbd UNUSED)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    प्रत्यागम FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    शून्य
clip_mch_lose_selection(Clipboard_T *cbd UNUSED)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    स्थिर char_u *
crnl_to_nl(ध्रुव char_u *str, पूर्णाङ्क *size)
{
    पूर्णाङ्क		pos = 0;
    पूर्णाङ्क		str_len = *size;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    यदि (ret != रिक्त)
    {
	retp = ret;
	प्रति (pos = 0; pos < str_len; ++pos)
	{
	    यदि (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*size);
	    }
	    *retp++ = str[pos];
	}
    }

    प्रत्यागम ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    स्थिर पूर्णाङ्क
vim_open_clipboard(शून्य)
{
    पूर्णाङ्क delay = 10;

    यावत् (!OpenClipboard(रिक्त))
    {
	यदि (delay > 500)
	    प्रत्यागम FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    प्रत्यागम TRUE;
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
    शून्य
clip_mch_request_selection(Clipboard_T *cbd)
{
    VimClipType_t	metadata = { -1, -1, -1, -1 };
    HGLOBAL		hMem = रिक्त;
    char_u		*str = रिक्त;
    char_u		*to_free = रिक्त;
    HGLOBAL		rawh = रिक्त;
    पूर्णाङ्क			str_size = 0;
    पूर्णाङ्क			maxlen;
    माप_प्रकार		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    यदि (!vim_open_clipboard())
	प्रत्यागम;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    यदि (IsClipboardFormatAvailable(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	यदि ((meta_h = GetClipboardData(cbd->format)) != रिक्त
		&& (meta_p = (VimClipType_t *)GlobalLock(meta_h)) != रिक्त)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = परिमाण(VimClipType_t);
	    यदि (GlobalSize(meta_h) < n)
		n = GlobalSize(meta_h);
	    स_नकल(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    यदि (IsClipboardFormatAvailable(cbd->format_raw)
				      && metadata.rawlen > (पूर्णाङ्क)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	यदि ((rawh = GetClipboardData(cbd->format_raw)) != रिक्त)
	{
	    char_u	*rawp;

	    rawp = (char_u *)GlobalLock(rawh);
	    यदि (rawp != रिक्त && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_size = (पूर्णाङ्क)(metadata.rawlen - n);
	    }
	    अन्यथा
	    {
		GlobalUnlock(rawh);
		rawh = रिक्त;
	    }
	}
    }
    यदि (str == रिक्त)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	यदि (IsClipboardFormatAvailable(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    यदि ((hMemW = GetClipboardData(CF_UNICODETEXT)) != रिक्त)
	    {
		WCHAR *hMemWstr = (WCHAR *)GlobalLock(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (पूर्णाङ्क)(GlobalSize(hMemW) / परिमाण(WCHAR));
		यदि (metadata.ucslen >= 0)
		{
		    यदि (metadata.ucslen > maxlen)
			str_size = maxlen;
		    अन्यथा
			str_size = metadata.ucslen;
		}
		अन्यथा
		{
		    प्रति (str_size = 0; str_size < maxlen; ++str_size)
			यदि (hMemWstr[str_size] == NUL)
			    भङ्ग;
		}
		to_free = str = utf16_to_enc((short_u *)hMemWstr, &str_size);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	अन्यथा यदि (IsClipboardFormatAvailable(CF_TEXT))
	{
	    यदि ((hMem = GetClipboardData(CF_TEXT)) != रिक्त)
	    {
		str = (char_u *)GlobalLock(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (पूर्णाङ्क)GlobalSize(hMem);
		यदि (metadata.txtlen >= 0)
		{
		    यदि (metadata.txtlen > maxlen)
			str_size = maxlen;
		    अन्यथा
			str_size = metadata.txtlen;
		}
		अन्यथा
		{
		    प्रति (str_size = 0; str_size < maxlen; ++str_size)
			यदि (str[str_size] == NUL)
			    भङ्ग;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_to_enc(str, str_size, &to_free, &maxlen);
		यदि (to_free != रिक्त)
		{
		    str_size = maxlen;
		    str = to_free;
		}
	    }
	}
    }

    यदि (str != रिक्त && metadata.txtlen != 0)
    {
	char_u *temp_clipboard;

	// If the type is not known detect it.
	यदि (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	temp_clipboard = crnl_to_nl(str, &str_size);
	यदि (temp_clipboard != रिक्त)
	{
	    clip_yank_selection(metadata.type, temp_clipboard, str_size, cbd);
	    vim_free(temp_clipboard);
	}
    }

    // unlock the global object
    यदि (hMem != रिक्त)
	GlobalUnlock(hMem);
    यदि (rawh != रिक्त)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(to_free);
}

/*
 * Send the current selection to the clipboard.
 */
    शून्य
clip_mch_set_selection(Clipboard_T *cbd)
{
    char_u		*str = रिक्त;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = रिक्त;
    HGLOBAL		hMem = रिक्त;
    HGLOBAL		hMemVim = रिक्त;
    HGLOBAL		hMemW = रिक्त;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    यदि (metadata.type < 0)
	प्रत्यागम;
    metadata.txtlen = (पूर्णाङ्क)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (पूर्णाङ्क)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)GlobalLock(hMemRaw);
	यदि (lpszMemRaw != रिक्त)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    स_नकल(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	अन्यथा
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*out;
	पूर्णाङ्क		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	out = (WCHAR *)enc_to_utf16(str, &len);
	यदि (out != रिक्त)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = WideCharToMultiByte(GetACP(), 0, out, len,
							       रिक्त, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    यदि (str == रिक्त)
	    {
		vim_free(out);
		प्रत्यागम;		// out of memory
	    }
	    WideCharToMultiByte(GetACP(), 0, out, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * परिमाण(WCHAR));
	    lpszMemW = (WCHAR *)GlobalLock(hMemW);
	    यदि (lpszMemW != रिक्त)
	    {
		स_नकल(lpszMemW, out, len * परिमाण(WCHAR));
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

	यदि (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = रिक्त;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       परिमाण(VimClipType_t));
	lpszMemVim = (VimClipType_t *)GlobalLock(hMemVim);
	स_नकल(lpszMemVim, &metadata, परिमाण(metadata));
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
    यदि (vim_open_clipboard())
    {
	यदि (EmptyClipboard())
	{
	    SetClipboardData(cbd->format, hMemVim);
	    hMemVim = 0;
	    यदि (hMemW != रिक्त)
	    {
		यदि (SetClipboardData(CF_UNICODETEXT, hMemW) != रिक्त)
		    hMemW = रिक्त;
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
    यदि (hMemRaw)
	GlobalFree(hMemRaw);
    यदि (hMem)
	GlobalFree(hMem);
    यदि (hMemW)
	GlobalFree(hMemW);
    यदि (hMemVim)
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
enc_to_utf16(char_u *str, पूर्णाङ्क *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = रिक्त;
    पूर्णाङ्क		len_loc;
    पूर्णाङ्क		length;

    यदि (lenp == रिक्त)
    {
	len_loc = (पूर्णाङ्क)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    यदि (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	MultiByteToWideChar_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &length);
    }
    अन्यथा
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	यदि (convert_setup(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    प्रत्यागम रिक्त;
	यदि (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    यदि (str == रिक्त)
		प्रत्यागम रिक्त;
	}
	convert_setup(&conv, रिक्त, रिक्त);

	length = utf8_to_utf16(str, *lenp, रिक्त, रिक्त);
	ret = ALLOC_MULT(WCHAR, length + 1);
	यदि (ret != रिक्त)
	{
	    utf8_to_utf16(str, *lenp, (short_u *)ret, रिक्त);
	    ret[length] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = length;
    प्रत्यागम (short_u *)ret;
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
utf16_to_enc(short_u *str, पूर्णाङ्क *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = रिक्त, *enc_str = रिक्त;
    पूर्णाङ्क		len_loc;

    यदि (lenp == रिक्त)
    {
	len_loc = (पूर्णाङ्क)wcslen(str) + 1;
	lenp = &len_loc;
    }

    यदि (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	पूर्णाङ्क length;

	WideCharToMultiByte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &length, 0, 0);
	*lenp = length;
	प्रत्यागम enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_to_utf8(str, *lenp == 0 ? 1 : *lenp, रिक्त));
    यदि (utf8_str != रिक्त)
    {
	*lenp = utf16_to_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	यदि (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	अन्यथा
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_setup(&conv, रिक्त, रिक्त);
    }

    प्रत्यागम enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    शून्य
acp_to_enc(
    char_u	*str,
    पूर्णाङ्क		str_size,
    char_u	**out,
    पूर्णाङ्क		*outlen)

{
    LPWSTR	widestr;

    MultiByteToWideChar_alloc(GetACP(), 0, (LPCSTR)str, str_size,
							    &widestr, outlen);
    यदि (widestr == रिक्त)
	प्रत्यागम;
    ++*outlen;	// Include the 0 after the string
    *out = utf16_to_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    शून्य
enc_to_acp(
    char_u	*str,
    पूर्णाङ्क		str_size,
    char_u	**out,
    पूर्णाङ्क		*outlen)

{
    LPWSTR	widestr;
    पूर्णाङ्क		len = str_size;

    widestr = (WCHAR *)enc_to_utf16(str, &len);
    यदि (widestr == रिक्त)
	प्रत्यागम;
    WideCharToMultiByte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)out, outlen, 0, 0);
    vim_free(widestr);
}
