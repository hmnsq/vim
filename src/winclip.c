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

#అన్తర్భావ "vim.h"

/*
 * Compile only the clipboard handling features when compiling for cygwin
 * posix environment.
 */
#నిర్వచితమైతే FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_సగటు
# include <windows.h>
# include "winclip.pro"
#అయితే_ముగింపు

/*
 * Convert an UTF-8 string to UTF-16.
 * "instr[inlen]" is the input.  "inlen" is in bytes.
 * When "outstr" is NULL only return the number of UTF-16 words produced.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Returns the number of UTF-16 words produced.
 */
    పూర్ణాంకం
utf8_గమ్యం_utf16(char_u *instr, పూర్ణాంకం inlen, short_u *outstr, పూర్ణాంకం *unconvlenp)
{
    పూర్ణాంకం		outlen = 0;
    char_u	*p = instr;
    పూర్ణాంకం		todo = inlen;
    పూర్ణాంకం		l;
    పూర్ణాంకం		ch;

    అయితే (todo > 0)
    {
	// Only convert if we have a complete sequence.
	l = utf_ptr2len_len(p, todo);
	ఒకవేళ (l > todo)
	{
	    // Return length of incomplete sequence.
	    ఒకవేళ (unconvlenp != ఖాళీ)
		*unconvlenp = todo;
	    ఆపు;
	}

	ch = utf_ptr2char(p);
	ఒకవేళ (ch >= 0x10000)
	{
	    // non-BMP character, encoding with surrogate pairs
	    ++outlen;
	    ఒకవేళ (outstr != ఖాళీ)
	    {
		*outstr++ = (0xD800 - (0x10000 >> 10)) + (ch >> 10);
		*outstr++ = 0xDC00 | (ch & 0x3FF);
	    }
	}
	లేకపోతే ఒకవేళ (outstr != ఖాళీ)
	    *outstr++ = ch;
	++outlen;
	p += l;
	todo -= l;
    }

    తిరిగి outlen;
}

/*
 * Convert an UTF-16 string to UTF-8.
 * The input is "instr[inlen]" with "inlen" in number of UTF-16 words.
 * When "outstr" is NULL only return the required number of bytes.
 * Otherwise "outstr" must be a buffer of sufficient size.
 * Return the number of bytes produced.
 */
    పూర్ణాంకం
utf16_గమ్యం_utf8(short_u *instr, పూర్ణాంకం inlen, char_u *outstr)
{
    పూర్ణాంకం		outlen = 0;
    పూర్ణాంకం		todo = inlen;
    short_u	*p = instr;
    పూర్ణాంకం		l;
    పూర్ణాంకం		ch, ch2;

    అయితే (todo > 0)
    {
	ch = *p;
	ఒకవేళ (ch >= 0xD800 && ch <= 0xDBFF && todo > 1)
	{
	    // surrogate pairs handling
	    ch2 = p[1];
	    ఒకవేళ (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
	    {
		ch = ((ch - 0xD800) << 10) + (ch2 & 0x3FF) + 0x10000;
		++p;
		--todo;
	    }
	}
	ఒకవేళ (outstr != ఖాళీ)
	{
	    l = utf_char2bytes(ch, outstr);
	    outstr += l;
	}
	లేకపోతే
	    l = utf_char2len(ch);
	++p;
	outlen += l;
	--todo;
    }

    తిరిగి outlen;
}

/*
 * Call MultiByteToWideChar() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra zero appended.
 * "outlen" is in words.
 */
    శూన్య
Multi_Byte_గమ్యం_Wide_Char_alloc(UINT cp, DWORD flags,
	LPCSTR లోపల, పూర్ణాంకం inlen,
	LPWSTR *బహిర్గతం, పూర్ణాంకం *outlen)
{
    *outlen = Multi_Byte_గమ్యం_Wide_Char(cp, flags, లోపల, inlen, 0, 0);
    // Add one word to avoid a zero-length alloc().
    *బహిర్గతం = ALLOC_MULT(WCHAR, *outlen + 1);
    ఒకవేళ (*బహిర్గతం == ఖాళీ)
	తిరిగి;

    Multi_Byte_గమ్యం_Wide_Char(cp, flags, లోపల, inlen, *బహిర్గతం, *outlen);
    (*బహిర్గతం)[*outlen] = 0;
}

/*
 * Call WideCharToMultiByte() and allocate memory for the result.
 * Returns the result in "*out[*outlen]" with an extra NUL appended.
 */
    శూన్య
Wide_Char_గమ్యం_Multi_Byte_alloc(UINT cp, DWORD flags,
	LPCWSTR లోపల, పూర్ణాంకం inlen,
	LPSTR *బహిర్గతం, పూర్ణాంకం *outlen,
	LPCSTR def, LPBOOL useddef)
{
    *outlen = Wide_Char_గమ్యం_Multi_Byte(cp, flags, లోపల, inlen, ఖాళీ, 0, def, useddef);
    // Add one byte to avoid a zero-length alloc().
    *బహిర్గతం = alloc(*outlen + 1);
    ఒకవేళ (*బహిర్గతం == ఖాళీ)
	తిరిగి;
    Wide_Char_గమ్యం_Multi_Byte(cp, flags, లోపల, inlen, *బహిర్గతం, *outlen, def, useddef);
    (*బహిర్గతం)[*outlen] = 0;
}


#నిర్వచితమైతే FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

    శూన్య
win_clip_init(శూన్య)
{
    clip_init(TRUE);

    /*
     * Vim's own clipboard format recognises whether the text is char, line,
     * or rectangular block.  Only useful for copying between two Vims.
     * "Clipboard_T" was used for previous versions, using the first
     * character to specify MCHAR, MLINE or MBLOCK.
     */
    clip_నక్షత్రం.format = రిజిస్టరు_Clipboard_Format("VimClipboard2");
    clip_నక్షత్రం.format_raw = రిజిస్టరు_Clipboard_Format("VimRawBytes");
}

// Type used for the clipboard type of Vim's data.
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం type;		// MCHAR, MBLOCK or MLINE
    పూర్ణాంకం txtlen;		// length of CF_TEXT in bytes
    పూర్ణాంకం ucslen;		// length of CF_UNICODETEXT in words
    పూర్ణాంకం rawlen;		// length of clip_star.format_raw, including encoding,
			// excluding terminating NUL
} VimClipType_t;

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    పూర్ణాంకం
clip_mch_own_selection(Clipboard_T *cbd వాడని)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    తిరిగి FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    శూన్య
clip_mch_lose_selection(Clipboard_T *cbd వాడని)
{
    // Nothing needs to be done here
}

/*
 * Copy "str[*size]" into allocated memory, changing CR-NL to NL.
 * Return the allocated result and the size in "*size".
 * Returns NULL when out of memory.
 */
    స్థిర char_u *
crnl_గమ్యం_nl(స్థిరరాశి char_u *str, పూర్ణాంకం *కొలత)
{
    పూర్ణాంకం		pos = 0;
    పూర్ణాంకం		str_len = *కొలత;
    char_u	*ret;
    char_u	*retp;

    // Avoid allocating zero bytes, it generates an error message.
    ret = alloc(str_len == 0 ? 1 : str_len);
    ఒకవేళ (ret != ఖాళీ)
    {
	retp = ret;
	కొరకు (pos = 0; pos < str_len; ++pos)
	{
	    ఒకవేళ (str[pos] == '\r' && str[pos + 1] == '\n')
	    {
		++pos;
		--(*కొలత);
	    }
	    *retp++ = str[pos];
	}
    }

    తిరిగి ret;
}

/*
 * Wait for another process to Close the Clipboard.
 * Returns TRUE for success.
 */
    స్థిర పూర్ణాంకం
vim_open_clipboard(శూన్య)
{
    పూర్ణాంకం delay = 10;

    అయితే (!OpenClipboard(ఖాళీ))
    {
	ఒకవేళ (delay > 500)
	    తిరిగి FALSE;  // waited too long, give up
	Sleep(delay);
	delay *= 2;	// wait for 10, 20, 40, 80, etc. msec
    }
    తిరిగి TRUE;
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
    శూన్య
clip_mch_request_selection(Clipboard_T *cbd)
{
    VimClipType_t	metadata = { -1, -1, -1, -1 };
    HGLOBAL		hMem = ఖాళీ;
    char_u		*str = ఖాళీ;
    char_u		*గమ్యం_free = ఖాళీ;
    HGLOBAL		rawh = ఖాళీ;
    పూర్ణాంకం			str_కొలత = 0;
    పూర్ణాంకం			maxlen;
    size_t		n;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    ఒకవేళ (!vim_open_clipboard())
	తిరిగి;

    // Check for vim's own clipboard format first.  This only gets the type of
    // the data, still need to use CF_UNICODETEXT or CF_TEXT for the text.
    ఒకవేళ (Is_Clipboard_Format_అందుబాటు(cbd->format))
    {
	VimClipType_t	*meta_p;
	HGLOBAL		meta_h;

	// We have metadata on the clipboard; try to get it.
	ఒకవేళ ((meta_h = Get_Clipboard_దత్తాంశం(cbd->format)) != ఖాళీ
		&& (meta_p = (VimClipType_t *)Global_తాళం(meta_h)) != ఖాళీ)
	{
	    // The size of "VimClipType_t" changed, "rawlen" was added later.
	    // Only copy what is available for backwards compatibility.
	    n = పరిమాణం(VimClipType_t);
	    ఒకవేళ (Global_కొలత(meta_h) < n)
		n = Global_కొలత(meta_h);
	    జ్ఞాపకం_నకలు(&metadata, meta_p, n);
	    GlobalUnlock(meta_h);
	}
    }

    // Check for Vim's raw clipboard format first.  This is used without
    // conversion, but only if 'encoding' matches.
    ఒకవేళ (Is_Clipboard_Format_అందుబాటు(cbd->format_raw)
				      && metadata.rawlen > (పూర్ణాంకం)STRLEN(p_enc))
    {
	// We have raw data on the clipboard; try to get it.
	ఒకవేళ ((rawh = Get_Clipboard_దత్తాంశం(cbd->format_raw)) != ఖాళీ)
	{
	    char_u	*rawp;

	    rawp = (char_u *)Global_తాళం(rawh);
	    ఒకవేళ (rawp != ఖాళీ && STRCMP(p_enc, rawp) == 0)
	    {
		n = STRLEN(p_enc) + 1;
		str = rawp + n;
		str_కొలత = (పూర్ణాంకం)(metadata.rawlen - n);
	    }
	    లేకపోతే
	    {
		GlobalUnlock(rawh);
		rawh = ఖాళీ;
	    }
	}
    }
    ఒకవేళ (str == ఖాళీ)
    {
	// Try to get the clipboard in Unicode if it's not an empty string.
	ఒకవేళ (Is_Clipboard_Format_అందుబాటు(CF_UNICODETEXT) && metadata.ucslen != 0)
	{
	    HGLOBAL hMemW;

	    ఒకవేళ ((hMemW = Get_Clipboard_దత్తాంశం(CF_UNICODETEXT)) != ఖాళీ)
	    {
		WCHAR *hMemWstr = (WCHAR *)Global_తాళం(hMemW);

		// Use the length of our metadata if possible, but limit it to
		// the GlobalSize() for safety.
		maxlen = (పూర్ణాంకం)(Global_కొలత(hMemW) / పరిమాణం(WCHAR));
		ఒకవేళ (metadata.ucslen >= 0)
		{
		    ఒకవేళ (metadata.ucslen > maxlen)
			str_కొలత = maxlen;
		    లేకపోతే
			str_కొలత = metadata.ucslen;
		}
		లేకపోతే
		{
		    కొరకు (str_కొలత = 0; str_కొలత < maxlen; ++str_కొలత)
			ఒకవేళ (hMemWstr[str_కొలత] == NUL)
			    ఆపు;
		}
		గమ్యం_free = str = utf16_గమ్యం_enc((short_u *)hMemWstr, &str_కొలత);
		GlobalUnlock(hMemW);
	    }
	}
	// Get the clipboard in the Active codepage.
	లేకపోతే ఒకవేళ (Is_Clipboard_Format_అందుబాటు(CF_పాఠ్యం))
	{
	    ఒకవేళ ((hMem = Get_Clipboard_దత్తాంశం(CF_పాఠ్యం)) != ఖాళీ)
	    {
		str = (char_u *)Global_తాళం(hMem);

		// The length is either what our metadata says or the strlen().
		// But limit it to the GlobalSize() for safety.
		maxlen = (పూర్ణాంకం)Global_కొలత(hMem);
		ఒకవేళ (metadata.txtlen >= 0)
		{
		    ఒకవేళ (metadata.txtlen > maxlen)
			str_కొలత = maxlen;
		    లేకపోతే
			str_కొలత = metadata.txtlen;
		}
		లేకపోతే
		{
		    కొరకు (str_కొలత = 0; str_కొలత < maxlen; ++str_కొలత)
			ఒకవేళ (str[str_కొలత] == NUL)
			    ఆపు;
		}

		// The text is in the active codepage.  Convert to
		// 'encoding', going through UTF-16.
		acp_గమ్యం_enc(str, str_కొలత, &గమ్యం_free, &maxlen);
		ఒకవేళ (గమ్యం_free != ఖాళీ)
		{
		    str_కొలత = maxlen;
		    str = గమ్యం_free;
		}
	    }
	}
    }

    ఒకవేళ (str != ఖాళీ && metadata.txtlen != 0)
    {
	char_u *తాత్కాలిక_clipboard;

	// If the type is not known detect it.
	ఒకవేళ (metadata.type == -1)
	    metadata.type = MAUTO;

	// Translate <CR><NL> into <NL>.
	తాత్కాలిక_clipboard = crnl_గమ్యం_nl(str, &str_కొలత);
	ఒకవేళ (తాత్కాలిక_clipboard != ఖాళీ)
	{
	    clip_yank_selection(metadata.type, తాత్కాలిక_clipboard, str_కొలత, cbd);
	    vim_free(తాత్కాలిక_clipboard);
	}
    }

    // unlock the global object
    ఒకవేళ (hMem != ఖాళీ)
	GlobalUnlock(hMem);
    ఒకవేళ (rawh != ఖాళీ)
	GlobalUnlock(rawh);
    CloseClipboard();
    vim_free(గమ్యం_free);
}

/*
 * Send the current selection to the clipboard.
 */
    శూన్య
clip_mch_సమితి_selection(Clipboard_T *cbd)
{
    char_u		*str = ఖాళీ;
    VimClipType_t	metadata;
    long_u		txtlen;
    HGLOBAL		hMemRaw = ఖాళీ;
    HGLOBAL		hMem = ఖాళీ;
    HGLOBAL		hMemVim = ఖాళీ;
    HGLOBAL		hMemW = ఖాళీ;

    // If the '*' register isn't already filled in, fill it in now
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    // Get the text to be put on the clipboard, with CR-LF.
    metadata.type = clip_convert_selection(&str, &txtlen, cbd);
    ఒకవేళ (metadata.type < 0)
	తిరిగి;
    metadata.txtlen = (పూర్ణాంకం)txtlen;
    metadata.ucslen = 0;
    metadata.rawlen = 0;

    // Always set the raw bytes: 'encoding', NUL and the text.  This is used
    // when copy/paste from/to Vim with the same 'encoding', so that illegal
    // bytes can also be copied and no conversion is needed.
    {
	LPSTR lpszMemRaw;

	metadata.rawlen = (పూర్ణాంకం)(txtlen + STRLEN(p_enc) + 1);
	hMemRaw = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							 metadata.rawlen + 1);
	lpszMemRaw = (LPSTR)Global_తాళం(hMemRaw);
	ఒకవేళ (lpszMemRaw != ఖాళీ)
	{
	    STRCPY(lpszMemRaw, p_enc);
	    జ్ఞాపకం_నకలు(lpszMemRaw + STRLEN(p_enc) + 1, str, txtlen + 1);
	    GlobalUnlock(hMemRaw);
	}
	లేకపోతే
	    metadata.rawlen = 0;
    }

    {
	WCHAR		*బహిర్గతం;
	పూర్ణాంకం		len = metadata.txtlen;

	// Convert the text to UTF-16. This is put on the clipboard as
	// CF_UNICODETEXT.
	బహిర్గతం = (WCHAR *)enc_గమ్యం_utf16(str, &len);
	ఒకవేళ (బహిర్గతం != ఖాళీ)
	{
	    WCHAR *lpszMemW;

	    // Convert the text for CF_TEXT to Active codepage. Otherwise it's
	    // p_enc, which has no relation to the Active codepage.
	    metadata.txtlen = Wide_Char_గమ్యం_Multi_Byte(GetACP(), 0, బహిర్గతం, len,
							       ఖాళీ, 0, 0, 0);
	    vim_free(str);
	    str = alloc(metadata.txtlen == 0 ? 1 : metadata.txtlen);
	    ఒకవేళ (str == ఖాళీ)
	    {
		vim_free(బహిర్గతం);
		తిరిగి;		// out of memory
	    }
	    Wide_Char_గమ్యం_Multi_Byte(GetACP(), 0, బహిర్గతం, len,
					   (LPSTR)str, metadata.txtlen, 0, 0);

	    // Allocate memory for the UTF-16 text, add one NUL word to
	    // terminate the string.
	    hMemW = (LPSTR)GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
						   (len + 1) * పరిమాణం(WCHAR));
	    lpszMemW = (WCHAR *)Global_తాళం(hMemW);
	    ఒకవేళ (lpszMemW != ఖాళీ)
	    {
		జ్ఞాపకం_నకలు(lpszMemW, బహిర్గతం, len * పరిమాణం(WCHAR));
		lpszMemW[len] = NUL;
		GlobalUnlock(hMemW);
	    }
	    vim_free(బహిర్గతం);
	    metadata.ucslen = len;
	}
    }

    // Allocate memory for the text, add one NUL byte to terminate the string.
    hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, metadata.txtlen + 1);
    {
	LPSTR lpszMem = (LPSTR)Global_తాళం(hMem);

	ఒకవేళ (lpszMem)
	{
	    mch_memmove((char_u *)lpszMem, str, metadata.txtlen);
	    GlobalUnlock(hMem);
	}
    }

    // Set up metadata:
    {
	VimClipType_t *lpszMemVim = ఖాళీ;

	hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,
						       పరిమాణం(VimClipType_t));
	lpszMemVim = (VimClipType_t *)Global_తాళం(hMemVim);
	జ్ఞాపకం_నకలు(lpszMemVim, &metadata, పరిమాణం(metadata));
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
    ఒకవేళ (vim_open_clipboard())
    {
	ఒకవేళ (ఖాళీలు_Clipboard())
	{
	    సమితి_Clipboard_దత్తాంశం(cbd->format, hMemVim);
	    hMemVim = 0;
	    ఒకవేళ (hMemW != ఖాళీ)
	    {
		ఒకవేళ (సమితి_Clipboard_దత్తాంశం(CF_UNICODETEXT, hMemW) != ఖాళీ)
		    hMemW = ఖాళీ;
	    }
	    // Always use CF_TEXT.  On Win98 Notepad won't obtain the
	    // CF_UNICODETEXT text, only CF_TEXT.
	    సమితి_Clipboard_దత్తాంశం(CF_పాఠ్యం, hMem);
	    hMem = 0;
	}
	CloseClipboard();
    }

    vim_free(str);
    // Free any allocations we didn't give to the clipboard:
    ఒకవేళ (hMemRaw)
	GlobalFree(hMemRaw);
    ఒకవేళ (hMem)
	GlobalFree(hMem);
    ఒకవేళ (hMemW)
	GlobalFree(hMemW);
    ఒకవేళ (hMemVim)
	GlobalFree(hMemVim);
}

#అయితే_ముగింపు // FEAT_CLIPBOARD

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
enc_గమ్యం_utf16(char_u *str, పూర్ణాంకం *lenp)
{
    vimconv_T	conv;
    WCHAR	*ret;
    char_u	*allocbuf = ఖాళీ;
    పూర్ణాంకం		len_loc;
    పూర్ణాంకం		నిడివి;

    ఒకవేళ (lenp == ఖాళీ)
    {
	len_loc = (పూర్ణాంకం)STRLEN(str) + 1;
	lenp = &len_loc;
    }

    ఒకవేళ (enc_codepage > 0)
    {
	// We can do any CP### -> UTF-16 in one pass, and we can do it
	// without iconv() (convert_* may need iconv).
	Multi_Byte_గమ్యం_Wide_Char_alloc(enc_codepage, 0, (LPCSTR)str, *lenp,
							       &ret, &నిడివి);
    }
    లేకపోతే
    {
	// Use "latin1" by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	ఒకవేళ (convert_అమరిక(&conv, p_enc ? p_enc : (char_u *)"latin1",
						   (char_u *)"utf-8") == FAIL)
	    తిరిగి ఖాళీ;
	ఒకవేళ (conv.vc_type != CONV_NONE)
	{
	    str = allocbuf = string_convert(&conv, str, lenp);
	    ఒకవేళ (str == ఖాళీ)
		తిరిగి ఖాళీ;
	}
	convert_అమరిక(&conv, ఖాళీ, ఖాళీ);

	నిడివి = utf8_గమ్యం_utf16(str, *lenp, ఖాళీ, ఖాళీ);
	ret = ALLOC_MULT(WCHAR, నిడివి + 1);
	ఒకవేళ (ret != ఖాళీ)
	{
	    utf8_గమ్యం_utf16(str, *lenp, (short_u *)ret, ఖాళీ);
	    ret[నిడివి] = 0;
	}

	vim_free(allocbuf);
    }

    *lenp = నిడివి;
    తిరిగి (short_u *)ret;
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
utf16_గమ్యం_enc(short_u *str, పూర్ణాంకం *lenp)
{
    vimconv_T	conv;
    char_u	*utf8_str = ఖాళీ, *enc_str = ఖాళీ;
    పూర్ణాంకం		len_loc;

    ఒకవేళ (lenp == ఖాళీ)
    {
	len_loc = (పూర్ణాంకం)wcslen(str) + 1;
	lenp = &len_loc;
    }

    ఒకవేళ (enc_codepage > 0)
    {
	// We can do any UTF-16 -> CP### in one pass.
	పూర్ణాంకం నిడివి;

	Wide_Char_గమ్యం_Multi_Byte_alloc(enc_codepage, 0, str, *lenp,
					    (LPSTR *)&enc_str, &నిడివి, 0, 0);
	*lenp = నిడివి;
	తిరిగి enc_str;
    }

    // Avoid allocating zero bytes, it generates an error message.
    utf8_str = alloc(utf16_గమ్యం_utf8(str, *lenp == 0 ? 1 : *lenp, ఖాళీ));
    ఒకవేళ (utf8_str != ఖాళీ)
    {
	*lenp = utf16_గమ్యం_utf8(str, *lenp, utf8_str);

	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;
	convert_అమరిక(&conv, (char_u *)"utf-8",
					    p_enc? p_enc: (char_u *)"latin1");
	ఒకవేళ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    enc_str = utf8_str;
	}
	లేకపోతే
	{
	    enc_str = string_convert(&conv, utf8_str, lenp);
	    vim_free(utf8_str);
	}

	convert_అమరిక(&conv, ఖాళీ, ఖాళీ);
    }

    తిరిగి enc_str;
}

/*
 * Convert from the active codepage to 'encoding'.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  "outlen" includes the
 * terminating NUL.
 */
    శూన్య
acp_గమ్యం_enc(
    char_u	*str,
    పూర్ణాంకం		str_కొలత,
    char_u	**బహిర్గతం,
    పూర్ణాంకం		*outlen)

{
    LPWSTR	widestr;

    Multi_Byte_గమ్యం_Wide_Char_alloc(GetACP(), 0, (LPCSTR)str, str_కొలత,
							    &widestr, outlen);
    ఒకవేళ (widestr == ఖాళీ)
	తిరిగి;
    ++*outlen;	// Include the 0 after the string
    *బహిర్గతం = utf16_గమ్యం_enc((short_u *)widestr, outlen);
    vim_free(widestr);
}

/*
 * Convert from 'encoding' to the active codepage.
 * Input is "str[str_size]".
 * The result is in allocated memory: "out[outlen]".  With terminating NUL.
 */
    శూన్య
enc_గమ్యం_acp(
    char_u	*str,
    పూర్ణాంకం		str_కొలత,
    char_u	**బహిర్గతం,
    పూర్ణాంకం		*outlen)

{
    LPWSTR	widestr;
    పూర్ణాంకం		len = str_కొలత;

    widestr = (WCHAR *)enc_గమ్యం_utf16(str, &len);
    ఒకవేళ (widestr == ఖాళీ)
	తిరిగి;
    Wide_Char_గమ్యం_Multi_Byte_alloc(GetACP(), 0, widestr, len,
	    (LPSTR *)బహిర్గతం, outlen, 0, 0);
    vim_free(widestr);
}
