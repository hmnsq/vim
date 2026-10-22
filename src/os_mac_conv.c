/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * os_mac_conv.c: Code specifically for Mac string conversions.
 *
 * This code has been put in a separate file to avoid the conflicts that are
 * caused by including both the X11 and Carbon header files.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NO_X11_INCLUDES

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <CoreServices/CoreServices.h>


#ਜੇ defined(MACOS_CONVERT)

ਸਥਿਰ char_u	    *mac_utf16_ਮੰਜ਼ਿਲ_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
ਸਥਿਰ UniChar	    *mac_utf8_ਮੰਜ਼ਿਲ_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
ਸਥਿਰ TECObjectRef g_ਰਸਤਾ_Converter;
// Converter used by mac_utf16_to_utf8
ਸਥਿਰ TECObjectRef g_UTF16_ਮੰਜ਼ਿਲ_UTF8_Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    ਪੂਰਨ			len,
    ਪੂਰਨ			*lenp,
    ਪੂਰਨ			fail_on_error,
    ਪੂਰਨ			from_enc,
    ਪੂਰਨ			ਮੰਜ਼ਿਲ_enc,
    ਪੂਰਨ			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    ਪੂਰਨ			buflen, ਵਿੱਚ, ਬਾਹਰਲਾ, l, i;
    CFStringEncoding	from;
    CFStringEncoding	ਮੰਜ਼ਿਲ;

    ਚੋਣ (from_enc)
    {
	ਹਾਲਤ 'l':   from = kCFStringEncodingISOLatin1; ਰੋਕੋ;
	ਹਾਲਤ 'm':   from = kCFStringEncodingMacRoman; ਰੋਕੋ;
	ਹਾਲਤ 'u':   from = kCFStringEncodingUTF8; ਰੋਕੋ;
	ਮੂਲ:    ਵਾਪਸ ਨਿਰਾ;
    }
    ਚੋਣ (ਮੰਜ਼ਿਲ_enc)
    {
	ਹਾਲਤ 'l':   ਮੰਜ਼ਿਲ = kCFStringEncodingISOLatin1; ਰੋਕੋ;
	ਹਾਲਤ 'm':   ਮੰਜ਼ਿਲ = kCFStringEncodingMacRoman; ਰੋਕੋ;
	ਹਾਲਤ 'u':   ਮੰਜ਼ਿਲ = kCFStringEncodingUTF8; ਰੋਕੋ;
	ਮੂਲ:    ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (unconvlenp != ਨਿਰਾ)
	*unconvlenp = 0;
    cfstr = CFString_Create_With_ਬਾਈਟਾਂ(ਨਿਰਾ, ptr, len, from, 0);

    ਜੇ (cfstr == ਨਿਰਾ)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    ਜਦੋਂ (cfstr == ਨਿਰਾ && unconvlenp != ਨਿਰਾ && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFString_Create_With_ਬਾਈਟਾਂ(ਨਿਰਾ, ptr, len, from, 0);
    }
    ਜੇ (cfstr == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ਮੰਜ਼ਿਲ == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    ਨਹੀਂਤਾਂ
	buflen = len + 1;
    retval = alloc(buflen);
    ਜੇ (retval == ਨਿਰਾ)
    {
	CFRelease(cfstr);
	ਵਾਪਸ ਨਿਰਾ;
    }

#ਜੇ 0
    CFRange convertRange = CFRangeMake(0, CFString_Get_ਲੰਮਾਈ(cfstr));
    //  Determine output buffer size
    CFString_Get_ਬਾਈਟਾਂ(cfstr, convertRange, ਮੰਜ਼ਿਲ, ਨਿਰਾ, FALSE, ਨਿਰਾ, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : ਨਿਰਾ;
    ਜੇ (retval == ਨਿਰਾ)
    {
	CFRelease(cfstr);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (lenp)
	*lenp = buflen / ਆਕਾਰ(char_u);

    ਜੇ (!CFString_Get_ਬਾਈਟਾਂ(cfstr, convertRange, ਮੰਜ਼ਿਲ, ਨਿਰਾ, FALSE, retval, buflen, ਨਿਰਾ))
#ਜੇ_ਅੰਤ
    ਜੇ (!CFStringGetCString(cfstr, (ਅੱਖਰ *)retval, buflen, ਮੰਜ਼ਿਲ))
    {
	CFRelease(cfstr);
	ਜੇ (fail_on_error)
	{
	    vim_free(retval);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	ਲਈ (d = retval, ਵਿੱਚ = 0, ਬਾਹਰਲਾ = 0; ਵਿੱਚ < len && ਬਾਹਰਲਾ < buflen - 1;)
	{
	    ਜੇ (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + ਵਿੱਚ);
	    ਨਹੀਂਤਾਂ
		l = 1;
	    cfstr = CFString_Create_With_ਬਾਈਟਾਂ(ਨਿਰਾ, ptr + ਵਿੱਚ, l, from, 0);
	    ਜੇ (cfstr == ਨਿਰਾ)
	    {
		*d++ = '?';
		ਬਾਹਰਲਾ++;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (!CFStringGetCString(cfstr, (ਅੱਖਰ *)d, buflen - ਬਾਹਰਲਾ, ਮੰਜ਼ਿਲ))
		{
		    *d++ = '?';
		    ਬਾਹਰਲਾ++;
		}
		ਨਹੀਂਤਾਂ
		{
		    i = STRLEN(d);
		    d += i;
		    ਬਾਹਰਲਾ += i;
		}
		CFRelease(cfstr);
	    }
	    ਵਿੱਚ += l;
	}
	*d = NUL;
	ਜੇ (lenp != ਨਿਰਾ)
	    *lenp = ਬਾਹਰਲਾ;
	ਵਾਪਸ retval;
    }
    CFRelease(cfstr);
    ਜੇ (lenp != ਨਿਰਾ)
	*lenp = STRLEN(retval);

    ਵਾਪਸ retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
macroman2enc(
    char_u	*ptr,
    ਲੰਮਾ	*sizep,
    ਲੰਮਾ	real_ਮਾਪ)
{
    CFStringRef		cfstr;
    CFRange		r;
    CFIndex		len = *sizep;

    // MacRoman is an 8-bit encoding, no need to move bytes to
    // conv_rest[].
    cfstr = CFString_Create_With_ਬਾਈਟਾਂ(ਨਿਰਾ, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    ਜੇ (cfstr == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    r.location = 0;
    r.ਲੰਮਾਈ = CFString_Get_ਲੰਮਾਈ(cfstr);
    ਜੇ (r.ਲੰਮਾਈ != CFString_Get_ਬਾਈਟਾਂ(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_ਮਾਪ - *sizep, &len))
    {
	CFRelease(cfstr);
	ਵਾਪਸ FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    ਵਾਪਸ OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*ਮੰਜ਼ਿਲ,
    ਪੂਰਨ		*tolenp,
    ਪੂਰਨ		maxtolen,
    char_u	*rest,
    ਪੂਰਨ		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFString_Create_With_ਬਾਈਟਾਂ(ਨਿਰਾ, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    ਜਦੋਂ (cfstr == ਨਿਰਾ && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFString_Create_With_ਬਾਈਟਾਂ(ਨਿਰਾ, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    ਜੇ (cfstr == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    r.location = 0;
    r.ਲੰਮਾਈ = CFString_Get_ਲੰਮਾਈ(cfstr);
    ਜੇ (r.ਲੰਮਾਈ != CFString_Get_ਬਾਈਟਾਂ(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		ਮੰਜ਼ਿਲ, maxtolen, &l))
    {
	CFRelease(cfstr);
	ਵਾਪਸ FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    ਵਾਪਸ OK;
}

/*
 * Initializes text converters
 */
    ਖਾਲੀ
mac_conv_init(ਖਾਲੀ)
{
    ਪਾਠ_Encoding    utf8_encoding;
    ਪਾਠ_Encoding    utf8_hfsplus_encoding;
    ਪਾਠ_Encoding    utf8_canon_encoding;
    ਪਾਠ_Encoding    utf16_encoding;

    utf8_encoding = Create_ਪਾਠ_Encoding(k_ਪਾਠ_Encoding_Unicode_Default,
	    k_ਪਾਠ_Encoding_Default_Variant, kUnicodeUTF8Format);
    utf8_hfsplus_encoding = Create_ਪਾਠ_Encoding(k_ਪਾਠ_Encoding_Unicode_Default,
	    kUnicodeHFSPlusCompVariant, kUnicodeUTF8Format);
    utf8_canon_encoding = Create_ਪਾਠ_Encoding(k_ਪਾਠ_Encoding_Unicode_Default,
	    kUnicodeCanonicalCompVariant, kUnicodeUTF8Format);
    utf16_encoding = Create_ਪਾਠ_Encoding(k_ਪਾਠ_Encoding_Unicode_Default,
	    k_ਪਾਠ_Encoding_Default_Variant, k_Unicode16_ਦੋਆਧਾਰੀ_ਅੰਕ_Format);

    ਜੇ (TECCreateConverter(&g_ਰਸਤਾ_Converter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	g_ਰਸਤਾ_Converter = ਨਿਰਾ;

    ਜੇ (TECCreateConverter(&g_UTF16_ਮੰਜ਼ਿਲ_UTF8_Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	ਜੇ (TECCreateConverter(&g_UTF16_ਮੰਜ਼ਿਲ_UTF8_Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    g_UTF16_ਮੰਜ਼ਿਲ_UTF8_Converter = ਨਿਰਾ;
    }
}

/*
 * Destroys text converters
 */
    ਖਾਲੀ
mac_conv_cleanup(ਖਾਲੀ)
{
    ਜੇ (g_UTF16_ਮੰਜ਼ਿਲ_UTF8_Converter)
    {
	TECDisposeConverter(g_UTF16_ਮੰਜ਼ਿਲ_UTF8_Converter);
	g_UTF16_ਮੰਜ਼ਿਲ_UTF8_Converter = ਨਿਰਾ;
    }

    ਜੇ (g_ਰਸਤਾ_Converter)
    {
	TECDisposeConverter(g_ਰਸਤਾ_Converter);
	g_ਰਸਤਾ_Converter = ਨਿਰਾ;
    }
}

/*
 * Conversion from UTF-16 UniChars to 'encoding'
 * The function signature uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    char_u *
mac_utf16_ਮੰਜ਼ਿਲ_enc(
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    char_u      *ਨਤੀਜਾ = ਨਿਰਾ;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_ਮੰਜ਼ਿਲ_utf8(from, fromLen, &utf8_len);

    ਜੇ (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	ਜੇ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_ਤਿਆਰੀ(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	ਜੇ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    ਨਤੀਜਾ = utf8_str;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ len = utf8_len;
	    ਨਤੀਜਾ = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);

	ਜੇ (actualLen)
	    *actualLen = utf8_len;
    }
    ਨਹੀਂਤਾਂ ਜੇ (actualLen)
	*actualLen = 0;

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Conversion from 'encoding' to UTF-16 UniChars
 * The function return uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *
mac_enc_ਮੰਜ਼ਿਲ_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    UniChar     *ਨਤੀਜਾ = ਨਿਰਾ;
    Boolean     should_free_utf8 = FALSE;

    ਕਰੋ
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	ਜੇ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_ਤਿਆਰੀ(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    ਰੋਕੋ;

	ਜੇ (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (ਪੂਰਨ *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	ਜੇ (utf8_str == ਨਿਰਾ)
	    ਰੋਕੋ;

	convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);

	ਨਤੀਜਾ = mac_utf8_ਮੰਜ਼ਿਲ_utf16(utf8_str, utf8_len, actualLen);

	ਜੇ (should_free_utf8)
	    vim_free(utf8_str);
	ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਜਦੋਂ (0);

    ਜੇ (actualLen)
	*actualLen = 0;

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    ਖਾਲੀ *
mac_enc_ਮੰਜ਼ਿਲ_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	ਨਤੀਜਾ = ਨਿਰਾ;

    utf16_str = mac_enc_ਮੰਜ਼ਿਲ_utf16(from, fromLen, &utf16_len);
    ਜੇ (utf16_str)
    {
	ਨਤੀਜਾ = CFStringCreateWithCharacters(ਨਿਰਾ, utf16_str, utf16_len/ਆਕਾਰ(UniChar));
	vim_free(utf16_str);
    }

    ਵਾਪਸ (ਖਾਲੀ *)ਨਤੀਜਾ;
}

/*
 * Converts a decomposed HFS+ UTF-8 path to precomposed UTF-8
 */
    char_u *
mac_precompose_ਰਸਤਾ(
    char_u  *decomp_ਰਸਤਾ,
    size_t  decompLen,
    size_t  *precompLen)
{
    char_u  *ਨਤੀਜਾ = ਨਿਰਾ;
    size_t  actualLen = 0;

    ਜੇ (g_ਰਸਤਾ_Converter)
    {
	ਨਤੀਜਾ = alloc(decompLen);
	ਜੇ (ਨਤੀਜਾ)
	{
	    ਜੇ (TECConvert_ਪਾਠ(g_ਰਸਤਾ_Converter, decomp_ਰਸਤਾ,
			decompLen, &decompLen, ਨਤੀਜਾ,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(ਨਤੀਜਾ);
	}
    }

    ਜੇ (precompLen)
	*precompLen = actualLen;

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    ਸਥਿਰ char_u *
mac_utf16_ਮੰਜ਼ਿਲ_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_ਗਿਣਤੀ		utf8_len;
    Byte_ਗਿਣਤੀ		ਨਿਵੇਸ਼_Read;
    char_u		*ਨਤੀਜਾ;

    ਜੇ (g_UTF16_ਮੰਜ਼ਿਲ_UTF8_Converter)
    {
	ਨਤੀਜਾ = alloc(fromLen * 6 + 1);
	ਜੇ (ਨਤੀਜਾ && TECConvert_ਪਾਠ(g_UTF16_ਮੰਜ਼ਿਲ_UTF8_Converter, (Const_ਪਾਠ_Ptr)from,
		    fromLen, &ਨਿਵੇਸ਼_Read, ਨਤੀਜਾ,
		    (fromLen*6+1)*ਆਕਾਰ(char_u), &utf8_len) == noErr)
	{
	    TECFlush_ਪਾਠ(g_UTF16_ਮੰਜ਼ਿਲ_UTF8_Converter, ਨਤੀਜਾ, (fromLen*6+1)*ਆਕਾਰ(char_u), &ਨਿਵੇਸ਼_Read);
	    utf8_len += ਨਿਵੇਸ਼_Read;
	}
	ਨਹੀਂਤਾਂ
	    VIM_CLEAR(ਨਤੀਜਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਨਤੀਜਾ = ਨਿਰਾ;
    }

    ਜੇ (actualLen)
	*actualLen = ਨਤੀਜਾ ? utf8_len : 0;

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    ਸਥਿਰ UniChar *
mac_utf8_ਮੰਜ਼ਿਲ_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *ਨਤੀਜਾ = ਨਿਰਾ;

    utf8_str = CFString_Create_With_ਬਾਈਟਾਂ(ਨਿਰਾ, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    ਜੇ (utf8_str == ਨਿਰਾ)
    {
	ਜੇ (actualLen)
	    *actualLen = 0;
	ਵਾਪਸ ਨਿਰਾ;
    }

    convertRange = CFRangeMake(0, CFString_Get_ਲੰਮਾਈ(utf8_str));
    ਨਤੀਜਾ = ALLOC_MULT(UniChar, convertRange.ਲੰਮਾਈ);

    CFStringGetCharacters(utf8_str, convertRange, ਨਤੀਜਾ);

    CFRelease(utf8_str);

    ਜੇ (actualLen)
	*actualLen = convertRange.ਲੰਮਾਈ * ਆਕਾਰ(UniChar);

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    ਖਾਲੀ
mac_lang_init(ਖਾਲੀ)
{
    ਜੇ (mch_getenv((char_u *)"LANG") != ਨਿਰਾ)
	ਵਾਪਸ;

    ਅੱਖਰ	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    ਜੇ (LocaleRefGetPartString(ਨਿਰਾ,
		k_Locale_Language_ਨਕਾਬ | k_Locale_Language_Variant_ਨਕਾਬ |
		k_Locale_Region_ਨਕਾਬ | k_Locale_Region_Variant_ਨਕਾਬ,
		ਆਕਾਰ(buf) - 10, buf) == noErr && *buf)
    {
	ਜੇ (strcasestr(buf, "utf-8") == ਨਿਰਾ)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#ਜੇ_ਅੰਤ
#ਜੇ defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#ਜੇ_ਅੰਤ
    }
}
#ਜੇ_ਅੰਤ // MACOS_CONVERT
