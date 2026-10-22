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

#ವ್ಯಾಖ್ಯಾನಿಸು NO_X11_INCLUDES

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <CoreServices/CoreServices.h>


#ಆದರೆ defined(MACOS_CONVERT)

ಸ್ಥಿರ char_u	    *mac_utf16_ಗಮ್ಯ_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
ಸ್ಥಿರ UniChar	    *mac_utf8_ಗಮ್ಯ_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
ಸ್ಥಿರ TECObjectRef g_ಮಾರ್ಗ_Converter;
// Converter used by mac_utf16_to_utf8
ಸ್ಥಿರ TECObjectRef g_UTF16_ಗಮ್ಯ_UTF8_Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    ಪೂರ್ಣಾಂಕ			len,
    ಪೂರ್ಣಾಂಕ			*lenp,
    ಪೂರ್ಣಾಂಕ			fail_on_error,
    ಪೂರ್ಣಾಂಕ			from_enc,
    ಪೂರ್ಣಾಂಕ			ಗಮ್ಯ_enc,
    ಪೂರ್ಣಾಂಕ			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    ಪೂರ್ಣಾಂಕ			buflen, ಒಳ, ಹೊರಹೋಗು, l, i;
    CFStringEncoding	from;
    CFStringEncoding	ಗಮ್ಯ;

    ಆಯ್ಕೆ (from_enc)
    {
	ಸ್ಥಿತಿ 'l':   from = kCFStringEncodingISOLatin1; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'm':   from = kCFStringEncodingMacRoman; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'u':   from = kCFStringEncodingUTF8; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆಯ್ಕೆ (ಗಮ್ಯ_enc)
    {
	ಸ್ಥಿತಿ 'l':   ಗಮ್ಯ = kCFStringEncodingISOLatin1; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'm':   ಗಮ್ಯ = kCFStringEncodingMacRoman; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'u':   ಗಮ್ಯ = kCFStringEncodingUTF8; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (unconvlenp != ಖಾಲಿ)
	*unconvlenp = 0;
    cfstr = CFString_Create_With_ಬೈಟುಗಳು(ಖಾಲಿ, ptr, len, from, 0);

    ಆದರೆ (cfstr == ಖಾಲಿ)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    ಆಗ (cfstr == ಖಾಲಿ && unconvlenp != ಖಾಲಿ && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFString_Create_With_ಬೈಟುಗಳು(ಖಾಲಿ, ptr, len, from, 0);
    }
    ಆದರೆ (cfstr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ಗಮ್ಯ == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    ಇಲ್ಲದಿದ್ದರೆ
	buflen = len + 1;
    retval = alloc(buflen);
    ಆದರೆ (retval == ಖಾಲಿ)
    {
	CFRelease(cfstr);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

#ಆದರೆ 0
    CFRange convertRange = CFRangeMake(0, CFString_Get_ಉದ್ದಳತೆ(cfstr));
    //  Determine output buffer size
    CFString_Get_ಬೈಟುಗಳು(cfstr, convertRange, ಗಮ್ಯ, ಖಾಲಿ, FALSE, ಖಾಲಿ, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : ಖಾಲಿ;
    ಆದರೆ (retval == ಖಾಲಿ)
    {
	CFRelease(cfstr);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (lenp)
	*lenp = buflen / ಗಾತ್ರ(char_u);

    ಆದರೆ (!CFString_Get_ಬೈಟುಗಳು(cfstr, convertRange, ಗಮ್ಯ, ಖಾಲಿ, FALSE, retval, buflen, ಖಾಲಿ))
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!CFStringGetCString(cfstr, (ಅಕ್ಷರ *)retval, buflen, ಗಮ್ಯ))
    {
	CFRelease(cfstr);
	ಆದರೆ (fail_on_error)
	{
	    vim_free(retval);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	ಗಾಗಿ (d = retval, ಒಳ = 0, ಹೊರಹೋಗು = 0; ಒಳ < len && ಹೊರಹೋಗು < buflen - 1;)
	{
	    ಆದರೆ (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + ಒಳ);
	    ಇಲ್ಲದಿದ್ದರೆ
		l = 1;
	    cfstr = CFString_Create_With_ಬೈಟುಗಳು(ಖಾಲಿ, ptr + ಒಳ, l, from, 0);
	    ಆದರೆ (cfstr == ಖಾಲಿ)
	    {
		*d++ = '?';
		ಹೊರಹೋಗು++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (!CFStringGetCString(cfstr, (ಅಕ್ಷರ *)d, buflen - ಹೊರಹೋಗು, ಗಮ್ಯ))
		{
		    *d++ = '?';
		    ಹೊರಹೋಗು++;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    i = STRLEN(d);
		    d += i;
		    ಹೊರಹೋಗು += i;
		}
		CFRelease(cfstr);
	    }
	    ಒಳ += l;
	}
	*d = NUL;
	ಆದರೆ (lenp != ಖಾಲಿ)
	    *lenp = ಹೊರಹೋಗು;
	ಹಿಂತಿರುಗಿ retval;
    }
    CFRelease(cfstr);
    ಆದರೆ (lenp != ಖಾಲಿ)
	*lenp = STRLEN(retval);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
macroman2enc(
    char_u	*ptr,
    ದೀರ್ಘ	*sizep,
    ದೀರ್ಘ	real_ಗಾತ್ರ)
{
    CFStringRef		cfstr;
    CFRange		r;
    CFIndex		len = *sizep;

    // MacRoman is an 8-bit encoding, no need to move bytes to
    // conv_rest[].
    cfstr = CFString_Create_With_ಬೈಟುಗಳು(ಖಾಲಿ, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    ಆದರೆ (cfstr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    r.location = 0;
    r.ಉದ್ದಳತೆ = CFString_Get_ಉದ್ದಳತೆ(cfstr);
    ಆದರೆ (r.ಉದ್ದಳತೆ != CFString_Get_ಬೈಟುಗಳು(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_ಗಾತ್ರ - *sizep, &len))
    {
	CFRelease(cfstr);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*ಗಮ್ಯ,
    ಪೂರ್ಣಾಂಕ		*tolenp,
    ಪೂರ್ಣಾಂಕ		maxtolen,
    char_u	*rest,
    ಪೂರ್ಣಾಂಕ		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFString_Create_With_ಬೈಟುಗಳು(ಖಾಲಿ, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    ಆಗ (cfstr == ಖಾಲಿ && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFString_Create_With_ಬೈಟುಗಳು(ಖಾಲಿ, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    ಆದರೆ (cfstr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    r.location = 0;
    r.ಉದ್ದಳತೆ = CFString_Get_ಉದ್ದಳತೆ(cfstr);
    ಆದರೆ (r.ಉದ್ದಳತೆ != CFString_Get_ಬೈಟುಗಳು(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		ಗಮ್ಯ, maxtolen, &l))
    {
	CFRelease(cfstr);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Initializes text converters
 */
    ಶೂನ್ಯ
mac_conv_init(ಶೂನ್ಯ)
{
    ಪಠ್ಯ_Encoding    utf8_encoding;
    ಪಠ್ಯ_Encoding    utf8_hfsplus_encoding;
    ಪಠ್ಯ_Encoding    utf8_canon_encoding;
    ಪಠ್ಯ_Encoding    utf16_encoding;

    utf8_encoding = Create_ಪಠ್ಯ_Encoding(k_ಪಠ್ಯ_Encoding_Unicode_Default,
	    k_ಪಠ್ಯ_Encoding_Default_Variant, kUnicodeUTF8Format);
    utf8_hfsplus_encoding = Create_ಪಠ್ಯ_Encoding(k_ಪಠ್ಯ_Encoding_Unicode_Default,
	    kUnicodeHFSPlusCompVariant, kUnicodeUTF8Format);
    utf8_canon_encoding = Create_ಪಠ್ಯ_Encoding(k_ಪಠ್ಯ_Encoding_Unicode_Default,
	    kUnicodeCanonicalCompVariant, kUnicodeUTF8Format);
    utf16_encoding = Create_ಪಠ್ಯ_Encoding(k_ಪಠ್ಯ_Encoding_Unicode_Default,
	    k_ಪಠ್ಯ_Encoding_Default_Variant, k_Unicode16_ಬಿಟ್_Format);

    ಆದರೆ (TECCreateConverter(&g_ಮಾರ್ಗ_Converter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	g_ಮಾರ್ಗ_Converter = ಖಾಲಿ;

    ಆದರೆ (TECCreateConverter(&g_UTF16_ಗಮ್ಯ_UTF8_Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	ಆದರೆ (TECCreateConverter(&g_UTF16_ಗಮ್ಯ_UTF8_Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    g_UTF16_ಗಮ್ಯ_UTF8_Converter = ಖಾಲಿ;
    }
}

/*
 * Destroys text converters
 */
    ಶೂನ್ಯ
mac_conv_cleanup(ಶೂನ್ಯ)
{
    ಆದರೆ (g_UTF16_ಗಮ್ಯ_UTF8_Converter)
    {
	TECDisposeConverter(g_UTF16_ಗಮ್ಯ_UTF8_Converter);
	g_UTF16_ಗಮ್ಯ_UTF8_Converter = ಖಾಲಿ;
    }

    ಆದರೆ (g_ಮಾರ್ಗ_Converter)
    {
	TECDisposeConverter(g_ಮಾರ್ಗ_Converter);
	g_ಮಾರ್ಗ_Converter = ಖಾಲಿ;
    }
}

/*
 * Conversion from UTF-16 UniChars to 'encoding'
 * The function signature uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    char_u *
mac_utf16_ಗಮ್ಯ_enc(
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    char_u      *ಫಲಿತಾಂಶ = ಖಾಲಿ;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_ಗಮ್ಯ_utf8(from, fromLen, &utf8_len);

    ಆದರೆ (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	ಆದರೆ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_ಸಿದ್ಧತೆ(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	ಆದರೆ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    ಫಲಿತಾಂಶ = utf8_str;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ len = utf8_len;
	    ಫಲಿತಾಂಶ = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);

	ಆದರೆ (actualLen)
	    *actualLen = utf8_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (actualLen)
	*actualLen = 0;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Conversion from 'encoding' to UTF-16 UniChars
 * The function return uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *
mac_enc_ಗಮ್ಯ_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    UniChar     *ಫಲಿತಾಂಶ = ಖಾಲಿ;
    Boolean     should_free_utf8 = FALSE;

    ಮಾಡು
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	ಆದರೆ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_ಸಿದ್ಧತೆ(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (ಪೂರ್ಣಾಂಕ *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	ಆದರೆ (utf8_str == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);

	ಫಲಿತಾಂಶ = mac_utf8_ಗಮ್ಯ_utf16(utf8_str, utf8_len, actualLen);

	ಆದರೆ (should_free_utf8)
	    vim_free(utf8_str);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಆಗ (0);

    ಆದರೆ (actualLen)
	*actualLen = 0;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    ಶೂನ್ಯ *
mac_enc_ಗಮ್ಯ_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	ಫಲಿತಾಂಶ = ಖಾಲಿ;

    utf16_str = mac_enc_ಗಮ್ಯ_utf16(from, fromLen, &utf16_len);
    ಆದರೆ (utf16_str)
    {
	ಫಲಿತಾಂಶ = CFStringCreateWithCharacters(ಖಾಲಿ, utf16_str, utf16_len/ಗಾತ್ರ(UniChar));
	vim_free(utf16_str);
    }

    ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)ಫಲಿತಾಂಶ;
}

/*
 * Converts a decomposed HFS+ UTF-8 path to precomposed UTF-8
 */
    char_u *
mac_precompose_ಮಾರ್ಗ(
    char_u  *decomp_ಮಾರ್ಗ,
    size_t  decompLen,
    size_t  *precompLen)
{
    char_u  *ಫಲಿತಾಂಶ = ಖಾಲಿ;
    size_t  actualLen = 0;

    ಆದರೆ (g_ಮಾರ್ಗ_Converter)
    {
	ಫಲಿತಾಂಶ = alloc(decompLen);
	ಆದರೆ (ಫಲಿತಾಂಶ)
	{
	    ಆದರೆ (TECConvert_ಪಠ್ಯ(g_ಮಾರ್ಗ_Converter, decomp_ಮಾರ್ಗ,
			decompLen, &decompLen, ಫಲಿತಾಂಶ,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(ಫಲಿತಾಂಶ);
	}
    }

    ಆದರೆ (precompLen)
	*precompLen = actualLen;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    ಸ್ಥಿರ char_u *
mac_utf16_ಗಮ್ಯ_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_ಎಣಿಕೆ		utf8_len;
    Byte_ಎಣಿಕೆ		ಒಳಹರಿವು_Read;
    char_u		*ಫಲಿತಾಂಶ;

    ಆದರೆ (g_UTF16_ಗಮ್ಯ_UTF8_Converter)
    {
	ಫಲಿತಾಂಶ = alloc(fromLen * 6 + 1);
	ಆದರೆ (ಫಲಿತಾಂಶ && TECConvert_ಪಠ್ಯ(g_UTF16_ಗಮ್ಯ_UTF8_Converter, (Const_ಪಠ್ಯ_Ptr)from,
		    fromLen, &ಒಳಹರಿವು_Read, ಫಲಿತಾಂಶ,
		    (fromLen*6+1)*ಗಾತ್ರ(char_u), &utf8_len) == noErr)
	{
	    TECFlush_ಪಠ್ಯ(g_UTF16_ಗಮ್ಯ_UTF8_Converter, ಫಲಿತಾಂಶ, (fromLen*6+1)*ಗಾತ್ರ(char_u), &ಒಳಹರಿವು_Read);
	    utf8_len += ಒಳಹರಿವು_Read;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    VIM_CLEAR(ಫಲಿತಾಂಶ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಫಲಿತಾಂಶ = ಖಾಲಿ;
    }

    ಆದರೆ (actualLen)
	*actualLen = ಫಲಿತಾಂಶ ? utf8_len : 0;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    ಸ್ಥಿರ UniChar *
mac_utf8_ಗಮ್ಯ_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *ಫಲಿತಾಂಶ = ಖಾಲಿ;

    utf8_str = CFString_Create_With_ಬೈಟುಗಳು(ಖಾಲಿ, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    ಆದರೆ (utf8_str == ಖಾಲಿ)
    {
	ಆದರೆ (actualLen)
	    *actualLen = 0;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    convertRange = CFRangeMake(0, CFString_Get_ಉದ್ದಳತೆ(utf8_str));
    ಫಲಿತಾಂಶ = ALLOC_MULT(UniChar, convertRange.ಉದ್ದಳತೆ);

    CFStringGetCharacters(utf8_str, convertRange, ಫಲಿತಾಂಶ);

    CFRelease(utf8_str);

    ಆದರೆ (actualLen)
	*actualLen = convertRange.ಉದ್ದಳತೆ * ಗಾತ್ರ(UniChar);

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    ಶೂನ್ಯ
mac_lang_init(ಶೂನ್ಯ)
{
    ಆದರೆ (mch_getenv((char_u *)"LANG") != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಅಕ್ಷರ	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    ಆದರೆ (LocaleRefGetPartString(ಖಾಲಿ,
		k_Locale_Language_ಮುಖವಾಡ | k_Locale_Language_Variant_ಮುಖವಾಡ |
		k_Locale_Region_ಮುಖವಾಡ | k_Locale_Region_Variant_ಮುಖವಾಡ,
		ಗಾತ್ರ(buf) - 10, buf) == noErr && *buf)
    {
	ಆದರೆ (strcasestr(buf, "utf-8") == ಖಾಲಿ)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#ಆದರೆ_ಅಂತ್ಯ
    }
}
#ಆದರೆ_ಅಂತ್ಯ // MACOS_CONVERT
