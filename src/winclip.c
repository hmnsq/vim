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

#شامل "vim.h"

/*
 * Compile only the clipboard handling features when compiling for cygwin
 * posix environment.
 */
#اگر_تعریف_شدہ FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_اوسط
# include <windows.h>
# include "winclip.pro"
#اگر_ختم

/*
 * Convert an UTF-8 string to UTF-16.
 * "instr[inlen]" is the input.  "inlen" is in bytes.
 * When "outstr" is NULL only return the number of UTF-16 words produced.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Returns the number of UTF-16 words produced.
 */
    صحیح
utf8_مقصد_utf16(char_u *instr, صحیح inlen, short_u *outstr, صحیح *unconvlenp)
{
    صحیح		outlen = 0;
    char_u	*p = instr;
    صحیح		todo = inlen;
    صحیح		l;
    صحیح		ch;

    جب (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	اگر (l > todo)
	{
	    // Return length of incomplete sequence.
	    اگر (unconvlenp != تہی)
		*unconvlenp = todo;
	    توڑو;
	}

	ch = utf_ptr2char(p);
	اگر (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    اگر (outstr != تہی)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	ورنہ اگر (outstr != تہی)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    واپس outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    صحیح
utf16_مقصد_utf8(short_u *instr, صحیح inlen, char_u *outstr)
{
    صحیح		outlen = 0;
    صحیح		todo = inlen;
    short_u	*p = instr;
    صحیح		l;
    صحیح		ch, ch2;

    جب (todo > 0)
    {
	ch = *p;
	اگر (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    اگر (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	اگر (outstr != تہی)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	ورنہ
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    واپس outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    خالی
Multi_Byte_مقصد_Wide_Char_alloc(UINT cp, DWORD flags,
	LPCSTR اندر, صحیح inlen,
	LPWSTR *برآمد, صحیح *outlen)
{
    *outlen = Multi_Byte_مقصد_Wide_Char(cp, flags, اندر, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *برآمد = ALLOC_MULT(WCHAR, *outlen + 1);
    اگر (*برآمد == تہی)
	واپس;

    Multi_Byte_مقصد_Wide_Char(cp, flags, اندر, inlen, *برآمد, *outlen);
    (*برآمد)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    خالی
Wide_Char_مقصد_Multi_Byte_alloc(UINT cp, DWORD flags,
	LPCWSTR اندر, صحیح inlen,
	LPSTR *برآمد, صحیح *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = Wide_Char_مقصد_Multi_Byte(cp, flags, اندر, inlen, تہی, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *برآمد = alloc(*outlen + 1);
    اگر (*برآمد == تہی)
	واپس;
    Wide_Char_مقصد_Multi_Byte(cp, flags, اندر, inlen, *برآمد, *outlen, def, useddef);
    (*برآمد)[*outlen] = 0;
}


#اگر_تعریف_شدہ FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

    خالی
win_clip_init(خالی)
{
    clip_init(TRUE);

    /*
     * Vim's own clipboard format recognises whether the text is char, line,
     * or rectangular block.  Only useful for copying between two Vims.
     * "Clipboard_T" was used for previous versions, using the first
     * character to specify MCHAR, MLINE or MBLOCK.
     */
    clip_ستارہ.format = رجسٹر_Clipboard_Format("VimClipboard2");
    clip_ستارہ.format_raw = رجسٹر_Clipboard_Format("VimRawBytes");
}

// Type used for the clipboard type of Vim's data.
قسمتعریف ساخت
{
    صحیح type;		// MCHAR, MBLOCK or MLINE
    صحیح txtlen;		// length of CF_TEXT in bytes
    صحیح ucslen;		// length of CF_UNICODETEXT in words
    صحیح rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    صحیح
clip_mch_own_selection(Clipboard_T *cbd غیر_مستعمل)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    واپس FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    خالی
clip_mch_lose_selection(Clipboard_T *cbd غیر_مستعمل)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    ساکن char_u *
crnl_مقصد_nl(ثابت char_u *str, صحیح *سائز)
{
    صحیح		pos = 0;
    صحیح		str_len = *سائز;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    اگر (ret != تہی)
    {
	retp = ret;
	برائے (pos = 0; pos < str_len; ++pos)
	{
	    اگر (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*سائز);
	    }
	    *retp++ = str[pos];
	}
    }

    واپس ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    ساکن صحیح
vim_open_clipboard(خالی)
{
    صحیح delay = 10;

    جب (!OpenClipboard(تہی))
    {
	اگر (delay > 500)
	    واپس FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    واپس TRUE;
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
    خالی
clip_mch_request_selection(Clipboard_T *cbd)
{
    VimClipType_t	metadata = { -1, -1, -1, -1 };
    HGLOBAL		hMem = تہی;
    char_u		*str = تہی;
    char_u		*مقصد_free = تہی;
    HGLOBAL		rawh = تہی;
    صحیح			str_سائز = 0;
    صحیح			maxlen;
    size_t		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    اگر (!vim_open_clipboard())
	واپس;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    اگر (Is_Clipboard_Format_دستیاب(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	اگر ((meta_h = Get_Clipboard_ڈیٹا(cbd->format)) != تہی
		&& (meta_p = (VimClipType_t *)Global_تالا(meta_h)) != تہی)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = حجم(VimClipType_t);
	    اگر (Global_سائز(meta_h) < n)
		n = Global_سائز(meta_h);
	    حافظہ_نقل(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    اگر (Is_Clipboard_Format_دستیاب(cbd->format_raw)
				      && metadata.rawlen > (صحیح)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	اگر ((rawh = Get_Clipboard_ڈیٹا(cbd->format_raw)) != تہی)
	{
	    char_u	*rawp;

	    rawp = (char_u *)Global_تالا(rawh);
	    اگر (rawp != تہی && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_سائز = (صحیح)(metadata.rawlen - n);
	    }
	    ورنہ
	    {
		GlobalUnlock(rawh);
		rawh = تہی;
	    }
	}
    }
    اگر (str == تہی)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	اگر (Is_Clipboard_Format_دستیاب(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    اگر ((hMemW = Get_Clipboard_ڈیٹا(CF_UNICODETEXT)) != تہی)
	    {
		WCHAR *hMemWstr = (WCHAR *)Global_تالا(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (صحیح)(Global_سائز(hMemW) / حجم(WCHAR));
		اگر (metadata.ucslen >= 0)
		{
		    اگر (metadata.ucslen > maxlen)
			str_سائز = maxlen;
		    ورنہ
			str_سائز = metadata.ucslen;
		}
		ورنہ
		{
		    برائے (str_سائز = 0; str_سائز < maxlen; ++str_سائز)
			اگر (hMemWstr[str_سائز] == NUL)
			    توڑو;
		}
		مقصد_free = str = utf16_مقصد_enc((short_u *)hMemWstr, &str_سائز);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	ورنہ اگر (Is_Clipboard_Format_دستیاب(CF_متن))
	{
	    اگر ((hMem = Get_Clipboard_ڈیٹا(CF_متن)) != تہی)
	    {
		str = (char_u *)Global_تالا(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (صحیح)Global_سائز(hMem);
		اگر (metadata.txtlen >= 0)
		{
		    اگر (metadata.txtlen > maxlen)
			str_سائز = maxlen;
		    ورنہ
			str_سائز = metadata.txtlen;
		}
		ورنہ
		{
		    برائے (str_سائز = 0; str_سائز < maxlen; ++str_سائز)
			اگر (str[str_سائز] == NUL)
			    توڑو;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_مقصد_enc(str, str_سائز, &مقصد_free, &maxlen);
		اگر (مقصد_free != تہی)
		{
		    str_سائز = maxlen;
		    str = مقصد_free;
		}
	    }
	}
    }

    اگر (str != تہی && metadata.txtlen != 0)
    {
	char_u *عارضی_clipboard;

	// If the type is not known detect it.
	اگر (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	عارضی_clipboard = crnl_مقصد_nl(str, &str_سائز);
	اگر (عارضی_clipboard != تہی)
	{
	    clip_yank_selection(metadata.type, عارضی_clipboard, str_سائز, cbd);
	    vim_free(عارضی_clipboard);
	}
    }

    // unlock the global object
    اگر (hMem != تہی)
	GlobalUnlock(hMem);
    اگر (rawh != تہی)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(مقصد_free);
}

/*
 * Send the current selection to the clipboard.
 */
    خالی
clip_mch_سیٹ_selection(Clipboard_T *cbd)
{
    char_u		*str = تہی;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = تہی;
    HGLOBAL		hMem = تہی;
    HGLOBAL		hMemVim = تہی;
    HGLOBAL		hMemW = تہی;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    اگر (metadata.type < 0)
	واپس;
    metadata.txtlen = (صحیح)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (صحیح)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)Global_تالا(hMemRaw);
	اگر (lpszMemRaw != تہی)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    حافظہ_نقل(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	ورنہ
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*برآمد;
	صحیح		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	برآمد = (WCHAR *)enc_مقصد_utf16(str, &len);
	اگر (برآمد != تہی)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = Wide_Char_مقصد_Multi_Byte(GetACP(), 0, برآمد, len,
							       تہی, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    اگر (str == تہی)
	    {
		vim_free(برآمد);
		واپس;		// out of memory
	    }
	    Wide_Char_مقصد_Multi_Byte(GetACP(), 0, برآمد, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * حجم(WCHAR));
	    lpszMemW = (WCHAR *)Global_تالا(hMemW);
	    اگر (lpszMemW != تہی)
	    {
		حافظہ_نقل(lpszMemW, برآمد, len * حجم(WCHAR));
		lpszMemW[len] = NUL;
		GlobalUnlock(hMemW);
	    }
	    vim_free(برآمد);
	    metadata.ucslen = len;
	}
    }

    // Allocate memory for the text, add one NUL byte to terminate the string.
    hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, metadata.txtlen + 1);
    {
	LPSTR lpszMem = (LPSTR)Global_تالا(hMem);

	اگر (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = تہی;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       حجم(VimClipType_t));
	lpszMemVim = (VimClipType_t *)Global_تالا(hMemVim);
	حافظہ_نقل(lpszMemVim, &metadata, حجم(metadata));
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
    اگر (vim_open_clipboard())
    {
	اگر (خالی_خانے_Clipboard())
	{
	    سیٹ_Clipboard_ڈیٹا(cbd->format, hMemVim);
	    hMemVim = 0;
	    اگر (hMemW != تہی)
	    {
		اگر (سیٹ_Clipboard_ڈیٹا(CF_UNICODETEXT, hMemW) != تہی)
		    hMemW = تہی;
	    }
	    // Always use CF_TEXT.  On Win98 Notepad won't obtain the
	    // CF_UNICODETEXT text, only CF_TEXT.
	    سیٹ_Clipboard_ڈیٹا(CF_متن, hMem);
	    hMem = 0;
	}
	CloseClipboard();
    }

    vim_free(str);
    // Free any allocations we didn't give to the clipboard:
    اگر (hMemRaw)
	GlobalFree(hMemRaw);
    اگر (hMem)
	GlobalFree(hMem);
    اگر (hMemW)
	GlobalFree(hMemW);
    اگر (hMemVim)
	GlobalFree(hMemVim);
}

#اگر_ختم // FEAT_CLIPBOARD

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
enc_مقصد_utf16(char_u *str, صحیح *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = تہی;
    صحیح		len_loc;
    صحیح		لمبائی;

    اگر (lenp == تہی)
    {
	len_loc = (صحیح)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    اگر (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	Multi_Byte_مقصد_Wide_Char_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &لمبائی);
    }
    ورنہ
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	اگر (convert_تیاری(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    واپس تہی;
	اگر (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    اگر (str == تہی)
		واپس تہی;
	}
	convert_تیاری(&conv, تہی, تہی);

	لمبائی = utf8_مقصد_utf16(str, *lenp, تہی, تہی);
	ret = ALLOC_MULT(WCHAR, لمبائی + 1);
	اگر (ret != تہی)
	{
	    utf8_مقصد_utf16(str, *lenp, (short_u *)ret, تہی);
	    ret[لمبائی] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = لمبائی;
    واپس (short_u *)ret;
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
utf16_مقصد_enc(short_u *str, صحیح *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = تہی, *enc_str = تہی;
    صحیح		len_loc;

    اگر (lenp == تہی)
    {
	len_loc = (صحیح)wcslen(str) + 1;
	lenp = &len_loc;
    }

    اگر (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	صحیح لمبائی;

	Wide_Char_مقصد_Multi_Byte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &لمبائی, 0, 0);
	*lenp = لمبائی;
	واپس enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_مقصد_utf8(str, *lenp == 0 ? 1 : *lenp, تہی));
    اگر (utf8_str != تہی)
    {
	*lenp = utf16_مقصد_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_تیاری(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	اگر (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	ورنہ
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_تیاری(&conv, تہی, تہی);
    }

    واپس enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    خالی
acp_مقصد_enc(
    char_u	*str,
    صحیح		str_سائز,
    char_u	**برآمد,
    صحیح		*outlen)

{
    LPWSTR	widestr;

    Multi_Byte_مقصد_Wide_Char_alloc(GetACP(), 0, (LPCSTR)str, str_سائز,
							    &widestr, outlen);
    اگر (widestr == تہی)
	واپس;
    ++*outlen;	// Include the 0 after the string
    *برآمد = utf16_مقصد_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    خالی
enc_مقصد_acp(
    char_u	*str,
    صحیح		str_سائز,
    char_u	**برآمد,
    صحیح		*outlen)

{
    LPWSTR	widestr;
    صحیح		len = str_سائز;

    widestr = (WCHAR *)enc_مقصد_utf16(str, &len);
    اگر (widestr == تہی)
	واپس;
    Wide_Char_مقصد_Multi_Byte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)برآمد, outlen, 0, 0);
    vim_free(widestr);
}
