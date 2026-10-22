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

#ضمن "vim.h"

/*
 * Compile only the clipboard handling features when compiling for cygwin
 * posix environment.
 */
#اگر_تعریف_شده FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_میانگین
# include <windows.h>
# include "winclip.pro"
#پایان_اگر

/*
 * Convert an UTF-8 string to UTF-16.
 * "instr[inlen]" is the input.  "inlen" is in bytes.
 * When "outstr" is NULL only return the number of UTF-16 words produced.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Returns the number of UTF-16 words produced.
 */
    صحيح
utf8_مقصد_utf16(char_u *instr, صحيح inlen, short_u *outstr, صحيح *unconvlenp)
{
    صحيح		outlen = 0;
    char_u	*p = instr;
    صحيح		todo = inlen;
    صحيح		l;
    صحيح		ch;

    طالما (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	إذا (l > todo)
	{
	    // Return length of incomplete sequence.
	    إذا (unconvlenp != عدم)
		*unconvlenp = todo;
	    اكسر;
	}

	ch = utf_ptr2char(p);
	إذا (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    إذا (outstr != عدم)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	وإلا إذا (outstr != عدم)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    أرجع outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    صحيح
utf16_مقصد_utf8(short_u *instr, صحيح inlen, char_u *outstr)
{
    صحيح		outlen = 0;
    صحيح		todo = inlen;
    short_u	*p = instr;
    صحيح		l;
    صحيح		ch, ch2;

    طالما (todo > 0)
    {
	ch = *p;
	إذا (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    إذا (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	إذا (outstr != عدم)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	وإلا
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    أرجع outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    فراغ
Multi_Byte_مقصد_Wide_Char_alloc(UINT cp, DWORD flags,
	LPCSTR درون, صحيح inlen,
	LPWSTR *بیرون, صحيح *outlen)
{
    *outlen = Multi_Byte_مقصد_Wide_Char(cp, flags, درون, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *بیرون = ALLOC_MULT(WCHAR, *outlen + 1);
    إذا (*بیرون == عدم)
	أرجع;

    Multi_Byte_مقصد_Wide_Char(cp, flags, درون, inlen, *بیرون, *outlen);
    (*بیرون)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    فراغ
Wide_Char_مقصد_Multi_Byte_alloc(UINT cp, DWORD flags,
	LPCWSTR درون, صحيح inlen,
	LPSTR *بیرون, صحيح *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = Wide_Char_مقصد_Multi_Byte(cp, flags, درون, inlen, عدم, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *بیرون = alloc(*outlen + 1);
    إذا (*بیرون == عدم)
	أرجع;
    Wide_Char_مقصد_Multi_Byte(cp, flags, درون, inlen, *بیرون, *outlen, def, useddef);
    (*بیرون)[*outlen] = 0;
}


#اگر_تعریف_شده FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

    فراغ
win_clip_init(فراغ)
{
    clip_init(TRUE);

    /*
     * Vim's own clipboard format recognises whether the text is char, line,
     * or rectangular block.  Only useful for copying between two Vims.
     * "Clipboard_T" was used for previous versions, using the first
     * character to specify MCHAR, MLINE or MBLOCK.
     */
    clip_ستاره.format = ثبات_Clipboard_Format("VimClipboard2");
    clip_ستاره.format_raw = ثبات_Clipboard_Format("VimRawBytes");
}

// Type used for the clipboard type of Vim's data.
تعريفنوع بنية
{
    صحيح type;		// MCHAR, MBLOCK or MLINE
    صحيح txtlen;		// length of CF_TEXT in bytes
    صحيح ucslen;		// length of CF_UNICODETEXT in words
    صحيح rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    صحيح
clip_mch_own_selection(Clipboard_T *cbd استفادهنشده)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    أرجع FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    فراغ
clip_mch_lose_selection(Clipboard_T *cbd استفادهنشده)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    ساكن char_u *
crnl_مقصد_nl(ثابت char_u *str, صحيح *اندازه)
{
    صحيح		pos = 0;
    صحيح		str_len = *اندازه;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    إذا (ret != عدم)
    {
	retp = ret;
	لكل (pos = 0; pos < str_len; ++pos)
	{
	    إذا (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*اندازه);
	    }
	    *retp++ = str[pos];
	}
    }

    أرجع ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    ساكن صحيح
vim_open_clipboard(فراغ)
{
    صحيح delay = 10;

    طالما (!OpenClipboard(عدم))
    {
	إذا (delay > 500)
	    أرجع FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    أرجع TRUE;
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
    فراغ
clip_mch_request_selection(Clipboard_T *cbd)
{
    VimClipType_t	metadata = { -1, -1, -1, -1 };
    HGLOBAL		hMem = عدم;
    char_u		*str = عدم;
    char_u		*مقصد_free = عدم;
    HGLOBAL		rawh = عدم;
    صحيح			str_اندازه = 0;
    صحيح			maxlen;
    size_t		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    إذا (!vim_open_clipboard())
	أرجع;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    إذا (Is_Clipboard_Format_موجود(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	إذا ((meta_h = Get_Clipboard_داده(cbd->format)) != عدم
		&& (meta_p = (VimClipType_t *)Global_قفل(meta_h)) != عدم)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = حجم(VimClipType_t);
	    إذا (Global_اندازه(meta_h) < n)
		n = Global_اندازه(meta_h);
	    کپی_حافظه(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    إذا (Is_Clipboard_Format_موجود(cbd->format_raw)
				      && metadata.rawlen > (صحيح)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	إذا ((rawh = Get_Clipboard_داده(cbd->format_raw)) != عدم)
	{
	    char_u	*rawp;

	    rawp = (char_u *)Global_قفل(rawh);
	    إذا (rawp != عدم && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_اندازه = (صحيح)(metadata.rawlen - n);
	    }
	    وإلا
	    {
		GlobalUnlock(rawh);
		rawh = عدم;
	    }
	}
    }
    إذا (str == عدم)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	إذا (Is_Clipboard_Format_موجود(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    إذا ((hMemW = Get_Clipboard_داده(CF_UNICODETEXT)) != عدم)
	    {
		WCHAR *hMemWstr = (WCHAR *)Global_قفل(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (صحيح)(Global_اندازه(hMemW) / حجم(WCHAR));
		إذا (metadata.ucslen >= 0)
		{
		    إذا (metadata.ucslen > maxlen)
			str_اندازه = maxlen;
		    وإلا
			str_اندازه = metadata.ucslen;
		}
		وإلا
		{
		    لكل (str_اندازه = 0; str_اندازه < maxlen; ++str_اندازه)
			إذا (hMemWstr[str_اندازه] == NUL)
			    اكسر;
		}
		مقصد_free = str = utf16_مقصد_enc((short_u *)hMemWstr, &str_اندازه);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	وإلا إذا (Is_Clipboard_Format_موجود(CF_متن))
	{
	    إذا ((hMem = Get_Clipboard_داده(CF_متن)) != عدم)
	    {
		str = (char_u *)Global_قفل(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (صحيح)Global_اندازه(hMem);
		إذا (metadata.txtlen >= 0)
		{
		    إذا (metadata.txtlen > maxlen)
			str_اندازه = maxlen;
		    وإلا
			str_اندازه = metadata.txtlen;
		}
		وإلا
		{
		    لكل (str_اندازه = 0; str_اندازه < maxlen; ++str_اندازه)
			إذا (str[str_اندازه] == NUL)
			    اكسر;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_مقصد_enc(str, str_اندازه, &مقصد_free, &maxlen);
		إذا (مقصد_free != عدم)
		{
		    str_اندازه = maxlen;
		    str = مقصد_free;
		}
	    }
	}
    }

    إذا (str != عدم && metadata.txtlen != 0)
    {
	char_u *موقت_clipboard;

	// If the type is not known detect it.
	إذا (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	موقت_clipboard = crnl_مقصد_nl(str, &str_اندازه);
	إذا (موقت_clipboard != عدم)
	{
	    clip_yank_selection(metadata.type, موقت_clipboard, str_اندازه, cbd);
	    vim_free(موقت_clipboard);
	}
    }

    // unlock the global object
    إذا (hMem != عدم)
	GlobalUnlock(hMem);
    إذا (rawh != عدم)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(مقصد_free);
}

/*
 * Send the current selection to the clipboard.
 */
    فراغ
clip_mch_مجموعه_selection(Clipboard_T *cbd)
{
    char_u		*str = عدم;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = عدم;
    HGLOBAL		hMem = عدم;
    HGLOBAL		hMemVim = عدم;
    HGLOBAL		hMemW = عدم;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    إذا (metadata.type < 0)
	أرجع;
    metadata.txtlen = (صحيح)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (صحيح)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)Global_قفل(hMemRaw);
	إذا (lpszMemRaw != عدم)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    کپی_حافظه(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	وإلا
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*بیرون;
	صحيح		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	بیرون = (WCHAR *)enc_مقصد_utf16(str, &len);
	إذا (بیرون != عدم)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = Wide_Char_مقصد_Multi_Byte(GetACP(), 0, بیرون, len,
							       عدم, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    إذا (str == عدم)
	    {
		vim_free(بیرون);
		أرجع;		// out of memory
	    }
	    Wide_Char_مقصد_Multi_Byte(GetACP(), 0, بیرون, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * حجم(WCHAR));
	    lpszMemW = (WCHAR *)Global_قفل(hMemW);
	    إذا (lpszMemW != عدم)
	    {
		کپی_حافظه(lpszMemW, بیرون, len * حجم(WCHAR));
		lpszMemW[len] = NUL;
		GlobalUnlock(hMemW);
	    }
	    vim_free(بیرون);
	    metadata.ucslen = len;
	}
    }

    // Allocate memory for the text, add one NUL byte to terminate the string.
    hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, metadata.txtlen + 1);
    {
	LPSTR lpszMem = (LPSTR)Global_قفل(hMem);

	إذا (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = عدم;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       حجم(VimClipType_t));
	lpszMemVim = (VimClipType_t *)Global_قفل(hMemVim);
	کپی_حافظه(lpszMemVim, &metadata, حجم(metadata));
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
    إذا (vim_open_clipboard())
    {
	إذا (خالی_Clipboard())
	{
	    مجموعه_Clipboard_داده(cbd->format, hMemVim);
	    hMemVim = 0;
	    إذا (hMemW != عدم)
	    {
		إذا (مجموعه_Clipboard_داده(CF_UNICODETEXT, hMemW) != عدم)
		    hMemW = عدم;
	    }
	    // Always use CF_TEXT.  On Win98 Notepad won't obtain the
	    // CF_UNICODETEXT text, only CF_TEXT.
	    مجموعه_Clipboard_داده(CF_متن, hMem);
	    hMem = 0;
	}
	CloseClipboard();
    }

    vim_free(str);
    // Free any allocations we didn't give to the clipboard:
    إذا (hMemRaw)
	GlobalFree(hMemRaw);
    إذا (hMem)
	GlobalFree(hMem);
    إذا (hMemW)
	GlobalFree(hMemW);
    إذا (hMemVim)
	GlobalFree(hMemVim);
}

#پایان_اگر // FEAT_CLIPBOARD

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
enc_مقصد_utf16(char_u *str, صحيح *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = عدم;
    صحيح		len_loc;
    صحيح		درازی;

    إذا (lenp == عدم)
    {
	len_loc = (صحيح)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    إذا (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	Multi_Byte_مقصد_Wide_Char_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &درازی);
    }
    وإلا
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	إذا (convert_راهاندازی(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    أرجع عدم;
	إذا (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    إذا (str == عدم)
		أرجع عدم;
	}
	convert_راهاندازی(&conv, عدم, عدم);

	درازی = utf8_مقصد_utf16(str, *lenp, عدم, عدم);
	ret = ALLOC_MULT(WCHAR, درازی + 1);
	إذا (ret != عدم)
	{
	    utf8_مقصد_utf16(str, *lenp, (short_u *)ret, عدم);
	    ret[درازی] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = درازی;
    أرجع (short_u *)ret;
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
utf16_مقصد_enc(short_u *str, صحيح *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = عدم, *enc_str = عدم;
    صحيح		len_loc;

    إذا (lenp == عدم)
    {
	len_loc = (صحيح)wcslen(str) + 1;
	lenp = &len_loc;
    }

    إذا (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	صحيح درازی;

	Wide_Char_مقصد_Multi_Byte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &درازی, 0, 0);
	*lenp = درازی;
	أرجع enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_مقصد_utf8(str, *lenp == 0 ? 1 : *lenp, عدم));
    إذا (utf8_str != عدم)
    {
	*lenp = utf16_مقصد_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_راهاندازی(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	إذا (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	وإلا
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_راهاندازی(&conv, عدم, عدم);
    }

    أرجع enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    فراغ
acp_مقصد_enc(
    char_u	*str,
    صحيح		str_اندازه,
    char_u	**بیرون,
    صحيح		*outlen)

{
    LPWSTR	widestr;

    Multi_Byte_مقصد_Wide_Char_alloc(GetACP(), 0, (LPCSTR)str, str_اندازه,
							    &widestr, outlen);
    إذا (widestr == عدم)
	أرجع;
    ++*outlen;	// Include the 0 after the string
    *بیرون = utf16_مقصد_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    فراغ
enc_مقصد_acp(
    char_u	*str,
    صحيح		str_اندازه,
    char_u	**بیرون,
    صحيح		*outlen)

{
    LPWSTR	widestr;
    صحيح		len = str_اندازه;

    widestr = (WCHAR *)enc_مقصد_utf16(str, &len);
    إذا (widestr == عدم)
	أرجع;
    Wide_Char_مقصد_Multi_Byte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)بیرون, outlen, 0, 0);
    vim_free(widestr);
}
