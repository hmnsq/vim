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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
utf8_to_utf16(char_u *instr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା inlen, short_u *outstr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *unconvlenp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		outlen = 0;
    char_u	*p = instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo = inlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ch;

    ଯେତେବେଳେ (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	ଯଦି (l > todo)
	{
	    // Return length of incomplete sequence.
	    ଯଦି (unconvlenp != ଖାଲି)
		*unconvlenp = todo;
	    ଭାଙ୍ଗ;
	}

	ch = utf_ptr2char(p);
	ଯଦି (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    ଯଦି (outstr != ଖାଲି)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (outstr != ଖାଲି)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    ଫେରନ୍ତୁ outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
utf16_to_utf8(short_u *instr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା inlen, char_u *outstr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		outlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo = inlen;
    short_u	*p = instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ch, ch2;

    ଯେତେବେଳେ (todo > 0)
    {
	ch = *p;
	ଯଦି (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    ଯଦି (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	ଯଦି (outstr != ଖାଲି)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	ଅନ୍ୟଥା
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    ଫେରନ୍ତୁ outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    ଶୂନ୍ୟ
MultiByteToWideChar_alloc(UINT cp, DWORD flags,
	LPCSTR in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା inlen,
	LPWSTR *out, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *outlen)
{
    *outlen = MultiByteToWideChar(cp, flags, in, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *out = ALLOC_MULT(WCHAR, *outlen + 1);
    ଯଦି (*out == ଖାଲି)
	ଫେରନ୍ତୁ;

    MultiByteToWideChar(cp, flags, in, inlen, *out, *outlen);
    (*out)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    ଶୂନ୍ୟ
WideCharToMultiByte_alloc(UINT cp, DWORD flags,
	LPCWSTR in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା inlen,
	LPSTR *out, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = WideCharToMultiByte(cp, flags, in, inlen, ଖାଲି, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *out = alloc(*outlen + 1);
    ଯଦି (*out == ଖାଲି)
	ଫେରନ୍ତୁ;
    WideCharToMultiByte(cp, flags, in, inlen, *out, *outlen, def, useddef);
    (*out)[*outlen] = 0;
}


#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

    ଶୂନ୍ୟ
win_clip_init(ଶୂନ୍ୟ)
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;		// MCHAR, MBLOCK or MLINE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା txtlen;		// length of CF_TEXT in bytes
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ucslen;		// length of CF_UNICODETEXT in words
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_mch_own_selection(Clipboard_T *cbd UNUSED)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    ଶୂନ୍ୟ
clip_mch_lose_selection(Clipboard_T *cbd UNUSED)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    ସ୍ଥିର char_u *
crnl_to_nl(ଅଚଳ char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *size)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pos = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		str_len = *size;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    ଯଦି (ret != ଖାଲି)
    {
	retp = ret;
	ପାଇଁ (pos = 0; pos < str_len; ++pos)
	{
	    ଯଦି (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*size);
	    }
	    *retp++ = str[pos];
	}
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_open_clipboard(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା delay = 10;

    ଯେତେବେଳେ (!OpenClipboard(ଖାଲି))
    {
	ଯଦି (delay > 500)
	    ଫେରନ୍ତୁ FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    ଫେରନ୍ତୁ TRUE;
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
    ଶୂନ୍ୟ
clip_mch_request_selection(Clipboard_T *cbd)
{
    VimClipType_t	metadata = { -1, -1, -1, -1 };
    HGLOBAL		hMem = ଖାଲି;
    char_u		*str = ଖାଲି;
    char_u		*to_free = ଖାଲି;
    HGLOBAL		rawh = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			str_size = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			maxlen;
    size_t		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    ଯଦି (!vim_open_clipboard())
	ଫେରନ୍ତୁ;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    ଯଦି (IsClipboardFormatAvailable(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	ଯଦି ((meta_h = GetClipboardData(cbd->format)) != ଖାଲି
		&& (meta_p = (VimClipType_t *)GlobalLock(meta_h)) != ଖାଲି)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = ଆକାର(VimClipType_t);
	    ଯଦି (GlobalSize(meta_h) < n)
		n = GlobalSize(meta_h);
	    memcpy(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    ଯଦି (IsClipboardFormatAvailable(cbd->format_raw)
				      && metadata.rawlen > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	ଯଦି ((rawh = GetClipboardData(cbd->format_raw)) != ଖାଲି)
	{
	    char_u	*rawp;

	    rawp = (char_u *)GlobalLock(rawh);
	    ଯଦି (rawp != ଖାଲି && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_size = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(metadata.rawlen - n);
	    }
	    ଅନ୍ୟଥା
	    {
		GlobalUnlock(rawh);
		rawh = ଖାଲି;
	    }
	}
    }
    ଯଦି (str == ଖାଲି)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	ଯଦି (IsClipboardFormatAvailable(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    ଯଦି ((hMemW = GetClipboardData(CF_UNICODETEXT)) != ଖାଲି)
	    {
		WCHAR *hMemWstr = (WCHAR *)GlobalLock(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(GlobalSize(hMemW) / ଆକାର(WCHAR));
		ଯଦି (metadata.ucslen >= 0)
		{
		    ଯଦି (metadata.ucslen > maxlen)
			str_size = maxlen;
		    ଅନ୍ୟଥା
			str_size = metadata.ucslen;
		}
		ଅନ୍ୟଥା
		{
		    ପାଇଁ (str_size = 0; str_size < maxlen; ++str_size)
			ଯଦି (hMemWstr[str_size] == NUL)
			    ଭାଙ୍ଗ;
		}
		to_free = str = utf16_to_enc((short_u *)hMemWstr, &str_size);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	ଅନ୍ୟଥା ଯଦି (IsClipboardFormatAvailable(CF_TEXT))
	{
	    ଯଦି ((hMem = GetClipboardData(CF_TEXT)) != ଖାଲି)
	    {
		str = (char_u *)GlobalLock(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GlobalSize(hMem);
		ଯଦି (metadata.txtlen >= 0)
		{
		    ଯଦି (metadata.txtlen > maxlen)
			str_size = maxlen;
		    ଅନ୍ୟଥା
			str_size = metadata.txtlen;
		}
		ଅନ୍ୟଥା
		{
		    ପାଇଁ (str_size = 0; str_size < maxlen; ++str_size)
			ଯଦି (str[str_size] == NUL)
			    ଭାଙ୍ଗ;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_to_enc(str, str_size, &to_free, &maxlen);
		ଯଦି (to_free != ଖାଲି)
		{
		    str_size = maxlen;
		    str = to_free;
		}
	    }
	}
    }

    ଯଦି (str != ଖାଲି && metadata.txtlen != 0)
    {
	char_u *temp_clipboard;

	// If the type is not known detect it.
	ଯଦି (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	temp_clipboard = crnl_to_nl(str, &str_size);
	ଯଦି (temp_clipboard != ଖାଲି)
	{
	    clip_yank_selection(metadata.type, temp_clipboard, str_size, cbd);
	    vim_free(temp_clipboard);
	}
    }

    // unlock the global object
    ଯଦି (hMem != ଖାଲି)
	GlobalUnlock(hMem);
    ଯଦି (rawh != ଖାଲି)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(to_free);
}

/*
 * Send the current selection to the clipboard.
 */
    ଶୂନ୍ୟ
clip_mch_set_selection(Clipboard_T *cbd)
{
    char_u		*str = ଖାଲି;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = ଖାଲି;
    HGLOBAL		hMem = ଖାଲି;
    HGLOBAL		hMemVim = ଖାଲି;
    HGLOBAL		hMemW = ଖାଲି;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    ଯଦି (metadata.type < 0)
	ଫେରନ୍ତୁ;
    metadata.txtlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)GlobalLock(hMemRaw);
	ଯଦି (lpszMemRaw != ଖାଲି)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    memcpy(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	ଅନ୍ୟଥା
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*out;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	out = (WCHAR *)enc_to_utf16(str, &len);
	ଯଦି (out != ଖାଲି)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = WideCharToMultiByte(GetACP(), 0, out, len,
							       ଖାଲି, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    ଯଦି (str == ଖାଲି)
	    {
		vim_free(out);
		ଫେରନ୍ତୁ;		// out of memory
	    }
	    WideCharToMultiByte(GetACP(), 0, out, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * ଆକାର(WCHAR));
	    lpszMemW = (WCHAR *)GlobalLock(hMemW);
	    ଯଦି (lpszMemW != ଖାଲି)
	    {
		memcpy(lpszMemW, out, len * ଆକାର(WCHAR));
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

	ଯଦି (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = ଖାଲି;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       ଆକାର(VimClipType_t));
	lpszMemVim = (VimClipType_t *)GlobalLock(hMemVim);
	memcpy(lpszMemVim, &metadata, ଆକାର(metadata));
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
    ଯଦି (vim_open_clipboard())
    {
	ଯଦି (EmptyClipboard())
	{
	    SetClipboardData(cbd->format, hMemVim);
	    hMemVim = 0;
	    ଯଦି (hMemW != ଖାଲି)
	    {
		ଯଦି (SetClipboardData(CF_UNICODETEXT, hMemW) != ଖାଲି)
		    hMemW = ଖାଲି;
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
    ଯଦି (hMemRaw)
	GlobalFree(hMemRaw);
    ଯଦି (hMem)
	GlobalFree(hMem);
    ଯଦି (hMemW)
	GlobalFree(hMemW);
    ଯଦି (hMemVim)
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
enc_to_utf16(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len_loc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		length;

    ଯଦି (lenp == ଖାଲି)
    {
	len_loc = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    ଯଦି (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	MultiByteToWideChar_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &length);
    }
    ଅନ୍ୟଥା
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	ଯଦି (convert_setup(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    ଯଦି (str == ଖାଲି)
		ଫେରନ୍ତୁ ଖାଲି;
	}
	convert_setup(&conv, ଖାଲି, ଖାଲି);

	length = utf8_to_utf16(str, *lenp, ଖାଲି, ଖାଲି);
	ret = ALLOC_MULT(WCHAR, length + 1);
	ଯଦି (ret != ଖାଲି)
	{
	    utf8_to_utf16(str, *lenp, (short_u *)ret, ଖାଲି);
	    ret[length] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = length;
    ଫେରନ୍ତୁ (short_u *)ret;
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
utf16_to_enc(short_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = ଖାଲି, *enc_str = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len_loc;

    ଯଦି (lenp == ଖାଲି)
    {
	len_loc = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wcslen(str) + 1;
	lenp = &len_loc;
    }

    ଯଦି (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା length;

	WideCharToMultiByte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &length, 0, 0);
	*lenp = length;
	ଫେରନ୍ତୁ enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_to_utf8(str, *lenp == 0 ? 1 : *lenp, ଖାଲି));
    ଯଦି (utf8_str != ଖାଲି)
    {
	*lenp = utf16_to_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	ଯଦି (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	ଅନ୍ୟଥା
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_setup(&conv, ଖାଲି, ଖାଲି);
    }

    ଫେରନ୍ତୁ enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    ଶୂନ୍ୟ
acp_to_enc(
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		str_size,
    char_u	**out,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*outlen)

{
    LPWSTR	widestr;

    MultiByteToWideChar_alloc(GetACP(), 0, (LPCSTR)str, str_size,
							    &widestr, outlen);
    ଯଦି (widestr == ଖାଲି)
	ଫେରନ୍ତୁ;
    ++*outlen;	// Include the 0 after the string
    *out = utf16_to_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    ଶୂନ୍ୟ
enc_to_acp(
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		str_size,
    char_u	**out,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*outlen)

{
    LPWSTR	widestr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = str_size;

    widestr = (WCHAR *)enc_to_utf16(str, &len);
    ଯଦି (widestr == ଖାଲି)
	ଫେରନ୍ତୁ;
    WideCharToMultiByte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)out, outlen, 0, 0);
    vim_free(widestr);
}
