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

#অন্তর্ভাব "vim.h"

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
    পূর্ণসংখ্যা
utf8_to_utf16(char_u *instr, পূর্ণসংখ্যা inlen, short_u *outstr, পূর্ণসংখ্যা *unconvlenp)
{
    পূর্ণসংখ্যা		outlen = 0;
    char_u	*p = instr;
    পূর্ণসংখ্যা		todo = inlen;
    পূর্ণসংখ্যা		l;
    পূর্ণসংখ্যা		ch;

    যখন (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	যদি (l > todo)
	{
	    // Return length of incomplete sequence.
	    যদি (unconvlenp != ফাঁকা)
		*unconvlenp = todo;
	    বিরতি;
	}

	ch = utf_ptr2char(p);
	যদি (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    যদি (outstr != ফাঁকা)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	অন্যথা যদি (outstr != ফাঁকা)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    ফেরত outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    পূর্ণসংখ্যা
utf16_to_utf8(short_u *instr, পূর্ণসংখ্যা inlen, char_u *outstr)
{
    পূর্ণসংখ্যা		outlen = 0;
    পূর্ণসংখ্যা		todo = inlen;
    short_u	*p = instr;
    পূর্ণসংখ্যা		l;
    পূর্ণসংখ্যা		ch, ch2;

    যখন (todo > 0)
    {
	ch = *p;
	যদি (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    যদি (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	যদি (outstr != ফাঁকা)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	অন্যথা
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    ফেরত outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    শূন্য
MultiByteToWideChar_alloc(UINT cp, DWORD flags,
	LPCSTR in, পূর্ণসংখ্যা inlen,
	LPWSTR *out, পূর্ণসংখ্যা *outlen)
{
    *outlen = MultiByteToWideChar(cp, flags, in, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *out = ALLOC_MULT(WCHAR, *outlen + 1);
    যদি (*out == ফাঁকা)
	ফেরত;

    MultiByteToWideChar(cp, flags, in, inlen, *out, *outlen);
    (*out)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    শূন্য
WideCharToMultiByte_alloc(UINT cp, DWORD flags,
	LPCWSTR in, পূর্ণসংখ্যা inlen,
	LPSTR *out, পূর্ণসংখ্যা *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = WideCharToMultiByte(cp, flags, in, inlen, ফাঁকা, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *out = alloc(*outlen + 1);
    যদি (*out == ফাঁকা)
	ফেরত;
    WideCharToMultiByte(cp, flags, in, inlen, *out, *outlen, def, useddef);
    (*out)[*outlen] = 0;
}


#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

    শূন্য
win_clip_init(শূন্য)
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
প্রকারলেখ কাঠামো
{
    পূর্ণসংখ্যা type;		// MCHAR, MBLOCK or MLINE
    পূর্ণসংখ্যা txtlen;		// length of CF_TEXT in bytes
    পূর্ণসংখ্যা ucslen;		// length of CF_UNICODETEXT in words
    পূর্ণসংখ্যা rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    পূর্ণসংখ্যা
clip_mch_own_selection(Clipboard_T *cbd UNUSED)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ফেরত FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    শূন্য
clip_mch_lose_selection(Clipboard_T *cbd UNUSED)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    স্থির char_u *
crnl_to_nl(ধ্রুবক char_u *str, পূর্ণসংখ্যা *আকার_2)
{
    পূর্ণসংখ্যা		pos = 0;
    পূর্ণসংখ্যা		str_len = *আকার_2;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    যদি (ret != ফাঁকা)
    {
	retp = ret;
	জন্য (pos = 0; pos < str_len; ++pos)
	{
	    যদি (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*আকার_2);
	    }
	    *retp++ = str[pos];
	}
    }

    ফেরত ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    স্থির পূর্ণসংখ্যা
vim_open_clipboard(শূন্য)
{
    পূর্ণসংখ্যা delay = 10;

    যখন (!OpenClipboard(ফাঁকা))
    {
	যদি (delay > 500)
	    ফেরত FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    ফেরত TRUE;
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
    শূন্য
clip_mch_request_selection(Clipboard_T *cbd)
{
    VimClipType_t	metadata = { -1, -1, -1, -1 };
    HGLOBAL		hMem = ফাঁকা;
    char_u		*str = ফাঁকা;
    char_u		*to_free = ফাঁকা;
    HGLOBAL		rawh = ফাঁকা;
    পূর্ণসংখ্যা			str_আকার = 0;
    পূর্ণসংখ্যা			maxlen;
    size_t		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    যদি (!vim_open_clipboard())
	ফেরত;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    যদি (IsClipboardFormatAvailable(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	যদি ((meta_h = Get_Clipboard_তথ্য(cbd->format)) != ফাঁকা
		&& (meta_p = (VimClipType_t *)GlobalLock(meta_h)) != ফাঁকা)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = আকার(VimClipType_t);
	    যদি (Global_আকার(meta_h) < n)
		n = Global_আকার(meta_h);
	    memcpy(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    যদি (IsClipboardFormatAvailable(cbd->format_raw)
				      && metadata.rawlen > (পূর্ণসংখ্যা)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	যদি ((rawh = Get_Clipboard_তথ্য(cbd->format_raw)) != ফাঁকা)
	{
	    char_u	*rawp;

	    rawp = (char_u *)GlobalLock(rawh);
	    যদি (rawp != ফাঁকা && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_আকার = (পূর্ণসংখ্যা)(metadata.rawlen - n);
	    }
	    অন্যথা
	    {
		GlobalUnlock(rawh);
		rawh = ফাঁকা;
	    }
	}
    }
    যদি (str == ফাঁকা)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	যদি (IsClipboardFormatAvailable(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    যদি ((hMemW = Get_Clipboard_তথ্য(CF_UNICODETEXT)) != ফাঁকা)
	    {
		WCHAR *hMemWstr = (WCHAR *)GlobalLock(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (পূর্ণসংখ্যা)(Global_আকার(hMemW) / আকার(WCHAR));
		যদি (metadata.ucslen >= 0)
		{
		    যদি (metadata.ucslen > maxlen)
			str_আকার = maxlen;
		    অন্যথা
			str_আকার = metadata.ucslen;
		}
		অন্যথা
		{
		    জন্য (str_আকার = 0; str_আকার < maxlen; ++str_আকার)
			যদি (hMemWstr[str_আকার] == NUL)
			    বিরতি;
		}
		to_free = str = utf16_to_enc((short_u *)hMemWstr, &str_আকার);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	অন্যথা যদি (IsClipboardFormatAvailable(CF_TEXT))
	{
	    যদি ((hMem = Get_Clipboard_তথ্য(CF_TEXT)) != ফাঁকা)
	    {
		str = (char_u *)GlobalLock(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (পূর্ণসংখ্যা)Global_আকার(hMem);
		যদি (metadata.txtlen >= 0)
		{
		    যদি (metadata.txtlen > maxlen)
			str_আকার = maxlen;
		    অন্যথা
			str_আকার = metadata.txtlen;
		}
		অন্যথা
		{
		    জন্য (str_আকার = 0; str_আকার < maxlen; ++str_আকার)
			যদি (str[str_আকার] == NUL)
			    বিরতি;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_to_enc(str, str_আকার, &to_free, &maxlen);
		যদি (to_free != ফাঁকা)
		{
		    str_আকার = maxlen;
		    str = to_free;
		}
	    }
	}
    }

    যদি (str != ফাঁকা && metadata.txtlen != 0)
    {
	char_u *অস্থায়ী_clipboard;

	// If the type is not known detect it.
	যদি (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	অস্থায়ী_clipboard = crnl_to_nl(str, &str_আকার);
	যদি (অস্থায়ী_clipboard != ফাঁকা)
	{
	    clip_yank_selection(metadata.type, অস্থায়ী_clipboard, str_আকার, cbd);
	    vim_free(অস্থায়ী_clipboard);
	}
    }

    // unlock the global object
    যদি (hMem != ফাঁকা)
	GlobalUnlock(hMem);
    যদি (rawh != ফাঁকা)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(to_free);
}

/*
 * Send the current selection to the clipboard.
 */
    শূন্য
clip_mch_set_selection(Clipboard_T *cbd)
{
    char_u		*str = ফাঁকা;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = ফাঁকা;
    HGLOBAL		hMem = ফাঁকা;
    HGLOBAL		hMemVim = ফাঁকা;
    HGLOBAL		hMemW = ফাঁকা;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    যদি (metadata.type < 0)
	ফেরত;
    metadata.txtlen = (পূর্ণসংখ্যা)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (পূর্ণসংখ্যা)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)GlobalLock(hMemRaw);
	যদি (lpszMemRaw != ফাঁকা)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    memcpy(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	অন্যথা
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*out;
	পূর্ণসংখ্যা		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	out = (WCHAR *)enc_to_utf16(str, &len);
	যদি (out != ফাঁকা)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = WideCharToMultiByte(GetACP(), 0, out, len,
							       ফাঁকা, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    যদি (str == ফাঁকা)
	    {
		vim_free(out);
		ফেরত;		// out of memory
	    }
	    WideCharToMultiByte(GetACP(), 0, out, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * আকার(WCHAR));
	    lpszMemW = (WCHAR *)GlobalLock(hMemW);
	    যদি (lpszMemW != ফাঁকা)
	    {
		memcpy(lpszMemW, out, len * আকার(WCHAR));
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

	যদি (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = ফাঁকা;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       আকার(VimClipType_t));
	lpszMemVim = (VimClipType_t *)GlobalLock(hMemVim);
	memcpy(lpszMemVim, &metadata, আকার(metadata));
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
    যদি (vim_open_clipboard())
    {
	যদি (EmptyClipboard())
	{
	    Set_Clipboard_তথ্য(cbd->format, hMemVim);
	    hMemVim = 0;
	    যদি (hMemW != ফাঁকা)
	    {
		যদি (Set_Clipboard_তথ্য(CF_UNICODETEXT, hMemW) != ফাঁকা)
		    hMemW = ফাঁকা;
	    }
	    // Always use CF_TEXT.  On Win98 Notepad won't obtain the
	    // CF_UNICODETEXT text, only CF_TEXT.
	    Set_Clipboard_তথ্য(CF_TEXT, hMem);
	    hMem = 0;
	}
	CloseClipboard();
    }

    vim_free(str);
    // Free any allocations we didn't give to the clipboard:
    যদি (hMemRaw)
	GlobalFree(hMemRaw);
    যদি (hMem)
	GlobalFree(hMem);
    যদি (hMemW)
	GlobalFree(hMemW);
    যদি (hMemVim)
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
enc_to_utf16(char_u *str, পূর্ণসংখ্যা *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = ফাঁকা;
    পূর্ণসংখ্যা		len_loc;
    পূর্ণসংখ্যা		দৈর্ঘ্য;

    যদি (lenp == ফাঁকা)
    {
	len_loc = (পূর্ণসংখ্যা)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    যদি (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	MultiByteToWideChar_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &দৈর্ঘ্য);
    }
    অন্যথা
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	যদি (convert_setup(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    ফেরত ফাঁকা;
	যদি (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    যদি (str == ফাঁকা)
		ফেরত ফাঁকা;
	}
	convert_setup(&conv, ফাঁকা, ফাঁকা);

	দৈর্ঘ্য = utf8_to_utf16(str, *lenp, ফাঁকা, ফাঁকা);
	ret = ALLOC_MULT(WCHAR, দৈর্ঘ্য + 1);
	যদি (ret != ফাঁকা)
	{
	    utf8_to_utf16(str, *lenp, (short_u *)ret, ফাঁকা);
	    ret[দৈর্ঘ্য] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = দৈর্ঘ্য;
    ফেরত (short_u *)ret;
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
utf16_to_enc(short_u *str, পূর্ণসংখ্যা *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = ফাঁকা, *enc_str = ফাঁকা;
    পূর্ণসংখ্যা		len_loc;

    যদি (lenp == ফাঁকা)
    {
	len_loc = (পূর্ণসংখ্যা)wcslen(str) + 1;
	lenp = &len_loc;
    }

    যদি (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	পূর্ণসংখ্যা দৈর্ঘ্য;

	WideCharToMultiByte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &দৈর্ঘ্য, 0, 0);
	*lenp = দৈর্ঘ্য;
	ফেরত enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_to_utf8(str, *lenp == 0 ? 1 : *lenp, ফাঁকা));
    যদি (utf8_str != ফাঁকা)
    {
	*lenp = utf16_to_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	যদি (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	অন্যথা
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_setup(&conv, ফাঁকা, ফাঁকা);
    }

    ফেরত enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    শূন্য
acp_to_enc(
    char_u	*str,
    পূর্ণসংখ্যা		str_আকার,
    char_u	**out,
    পূর্ণসংখ্যা		*outlen)

{
    LPWSTR	widestr;

    MultiByteToWideChar_alloc(GetACP(), 0, (LPCSTR)str, str_আকার,
							    &widestr, outlen);
    যদি (widestr == ফাঁকা)
	ফেরত;
    ++*outlen;	// Include the 0 after the string
    *out = utf16_to_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    শূন্য
enc_to_acp(
    char_u	*str,
    পূর্ণসংখ্যা		str_আকার,
    char_u	**out,
    পূর্ণসংখ্যা		*outlen)

{
    LPWSTR	widestr;
    পূর্ণসংখ্যা		len = str_আকার;

    widestr = (WCHAR *)enc_to_utf16(str, &len);
    যদি (widestr == ফাঁকা)
	ফেরত;
    WideCharToMultiByte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)out, outlen, 0, 0);
    vim_free(widestr);
}
