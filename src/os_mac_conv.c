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

#وصف_ڪريو NO_X11_INCLUDES

#ضمن "vim.h"

#ضمن <CoreServices/CoreServices.h>


#إذا defined(MACOS_CONVERT)

ساكن char_u	    *mac_utf16_منزل_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
ساكن UniChar	    *mac_utf8_منزل_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
ساكن TECObjectRef g_رستو_Converter;
// Converter used by mac_utf16_to_utf8
ساكن TECObjectRef g_UTF16_منزل_UTF8_Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    صحيح			len,
    صحيح			*lenp,
    صحيح			fail_on_error,
    صحيح			from_enc,
    صحيح			منزل_enc,
    صحيح			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    صحيح			buflen, اندريون, ٻاهريون, l, i;
    CFStringEncoding	from;
    CFStringEncoding	منزل;

    بدل (from_enc)
    {
	حالة 'l':   from = kCFStringEncodingISOLatin1; اكسر;
	حالة 'm':   from = kCFStringEncodingMacRoman; اكسر;
	حالة 'u':   from = kCFStringEncodingUTF8; اكسر;
	افتراضي:    أرجع عدم;
    }
    بدل (منزل_enc)
    {
	حالة 'l':   منزل = kCFStringEncodingISOLatin1; اكسر;
	حالة 'm':   منزل = kCFStringEncodingMacRoman; اكسر;
	حالة 'u':   منزل = kCFStringEncodingUTF8; اكسر;
	افتراضي:    أرجع عدم;
    }

    إذا (unconvlenp != عدم)
	*unconvlenp = 0;
    cfstr = CFString_Create_With_بائيٽون(عدم, ptr, len, from, 0);

    إذا (cfstr == عدم)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    طالما (cfstr == عدم && unconvlenp != عدم && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFString_Create_With_بائيٽون(عدم, ptr, len, from, 0);
    }
    إذا (cfstr == عدم)
	أرجع عدم;

    إذا (منزل == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    وإلا
	buflen = len + 1;
    retval = alloc(buflen);
    إذا (retval == عدم)
    {
	CFRelease(cfstr);
	أرجع عدم;
    }

#إذا 0
    CFRange convertRange = CFRangeMake(0, CFString_Get_ڊيگهائي(cfstr));
    //  Determine output buffer size
    CFString_Get_بائيٽون(cfstr, convertRange, منزل, عدم, FALSE, عدم, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : عدم;
    إذا (retval == عدم)
    {
	CFRelease(cfstr);
	أرجع عدم;
    }

    إذا (lenp)
	*lenp = buflen / حجم(char_u);

    إذا (!CFString_Get_بائيٽون(cfstr, convertRange, منزل, عدم, FALSE, retval, buflen, عدم))
#جيڪڏهن_پورو
    إذا (!CFStringGetCString(cfstr, (حرف *)retval, buflen, منزل))
    {
	CFRelease(cfstr);
	إذا (fail_on_error)
	{
	    vim_free(retval);
	    أرجع عدم;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	لكل (d = retval, اندريون = 0, ٻاهريون = 0; اندريون < len && ٻاهريون < buflen - 1;)
	{
	    إذا (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + اندريون);
	    وإلا
		l = 1;
	    cfstr = CFString_Create_With_بائيٽون(عدم, ptr + اندريون, l, from, 0);
	    إذا (cfstr == عدم)
	    {
		*d++ = '?';
		ٻاهريون++;
	    }
	    وإلا
	    {
		إذا (!CFStringGetCString(cfstr, (حرف *)d, buflen - ٻاهريون, منزل))
		{
		    *d++ = '?';
		    ٻاهريون++;
		}
		وإلا
		{
		    i = STRLEN(d);
		    d += i;
		    ٻاهريون += i;
		}
		CFRelease(cfstr);
	    }
	    اندريون += l;
	}
	*d = NUL;
	إذا (lenp != عدم)
	    *lenp = ٻاهريون;
	أرجع retval;
    }
    CFRelease(cfstr);
    إذا (lenp != عدم)
	*lenp = STRLEN(retval);

    أرجع retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    صحيح
macroman2enc(
    char_u	*ptr,
    ڊگهو	*sizep,
    ڊگهو	real_سائيز)
{
    CFStringRef		cfstr;
    CFRange		r;
    CFIndex		len = *sizep;

    // MacRoman is an 8-bit encoding, no need to move bytes to
    // conv_rest[].
    cfstr = CFString_Create_With_بائيٽون(عدم, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    إذا (cfstr == عدم)
	أرجع FAIL;

    r.location = 0;
    r.ڊيگهائي = CFString_Get_ڊيگهائي(cfstr);
    إذا (r.ڊيگهائي != CFString_Get_بائيٽون(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_سائيز - *sizep, &len))
    {
	CFRelease(cfstr);
	أرجع FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    أرجع OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    صحيح
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*منزل,
    صحيح		*tolenp,
    صحيح		maxtolen,
    char_u	*rest,
    صحيح		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFString_Create_With_بائيٽون(عدم, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    طالما (cfstr == عدم && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFString_Create_With_بائيٽون(عدم, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    إذا (cfstr == عدم)
	أرجع FAIL;

    r.location = 0;
    r.ڊيگهائي = CFString_Get_ڊيگهائي(cfstr);
    إذا (r.ڊيگهائي != CFString_Get_بائيٽون(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		منزل, maxtolen, &l))
    {
	CFRelease(cfstr);
	أرجع FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    أرجع OK;
}

/*
 * Initializes text converters
 */
    فراغ
mac_conv_init(فراغ)
{
    متن_Encoding    utf8_encoding;
    متن_Encoding    utf8_hfsplus_encoding;
    متن_Encoding    utf8_canon_encoding;
    متن_Encoding    utf16_encoding;

    utf8_encoding = Create_متن_Encoding(k_متن_Encoding_Unicode_Default,
	    k_متن_Encoding_Default_Variant, kUnicodeUTF8Format);
    utf8_hfsplus_encoding = Create_متن_Encoding(k_متن_Encoding_Unicode_Default,
	    kUnicodeHFSPlusCompVariant, kUnicodeUTF8Format);
    utf8_canon_encoding = Create_متن_Encoding(k_متن_Encoding_Unicode_Default,
	    kUnicodeCanonicalCompVariant, kUnicodeUTF8Format);
    utf16_encoding = Create_متن_Encoding(k_متن_Encoding_Unicode_Default,
	    k_متن_Encoding_Default_Variant, k_Unicode16_ٻٽو_انگ_Format);

    إذا (TECCreateConverter(&g_رستو_Converter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	g_رستو_Converter = عدم;

    إذا (TECCreateConverter(&g_UTF16_منزل_UTF8_Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	إذا (TECCreateConverter(&g_UTF16_منزل_UTF8_Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    g_UTF16_منزل_UTF8_Converter = عدم;
    }
}

/*
 * Destroys text converters
 */
    فراغ
mac_conv_cleanup(فراغ)
{
    إذا (g_UTF16_منزل_UTF8_Converter)
    {
	TECDisposeConverter(g_UTF16_منزل_UTF8_Converter);
	g_UTF16_منزل_UTF8_Converter = عدم;
    }

    إذا (g_رستو_Converter)
    {
	TECDisposeConverter(g_رستو_Converter);
	g_رستو_Converter = عدم;
    }
}

/*
 * Conversion from UTF-16 UniChars to 'encoding'
 * The function signature uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    char_u *
mac_utf16_منزل_enc(
    بنانشان ننڍو *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    char_u      *نتيجو = عدم;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_منزل_utf8(from, fromLen, &utf8_len);

    إذا (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	إذا ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_تياري(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	إذا (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    نتيجو = utf8_str;
	}
	وإلا
	{
	    صحيح len = utf8_len;
	    نتيجو = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_تياري(&conv, عدم, عدم);

	إذا (actualLen)
	    *actualLen = utf8_len;
    }
    وإلا إذا (actualLen)
	*actualLen = 0;

    أرجع نتيجو;
}

/*
 * Conversion from 'encoding' to UTF-16 UniChars
 * The function return uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    بنانشان ننڍو *
mac_enc_منزل_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    UniChar     *نتيجو = عدم;
    Boolean     should_free_utf8 = FALSE;

    افعل
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	إذا ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_تياري(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    اكسر;

	إذا (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (صحيح *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	وإلا
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	إذا (utf8_str == عدم)
	    اكسر;

	convert_تياري(&conv, عدم, عدم);

	نتيجو = mac_utf8_منزل_utf16(utf8_str, utf8_len, actualLen);

	إذا (should_free_utf8)
	    vim_free(utf8_str);
	أرجع نتيجو;
    }
    طالما (0);

    إذا (actualLen)
	*actualLen = 0;

    أرجع نتيجو;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    فراغ *
mac_enc_منزل_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	نتيجو = عدم;

    utf16_str = mac_enc_منزل_utf16(from, fromLen, &utf16_len);
    إذا (utf16_str)
    {
	نتيجو = CFStringCreateWithCharacters(عدم, utf16_str, utf16_len/حجم(UniChar));
	vim_free(utf16_str);
    }

    أرجع (فراغ *)نتيجو;
}

/*
 * Converts a decomposed HFS+ UTF-8 path to precomposed UTF-8
 */
    char_u *
mac_precompose_رستو(
    char_u  *decomp_رستو,
    size_t  decompLen,
    size_t  *precompLen)
{
    char_u  *نتيجو = عدم;
    size_t  actualLen = 0;

    إذا (g_رستو_Converter)
    {
	نتيجو = alloc(decompLen);
	إذا (نتيجو)
	{
	    إذا (TECConvert_متن(g_رستو_Converter, decomp_رستو,
			decompLen, &decompLen, نتيجو,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(نتيجو);
	}
    }

    إذا (precompLen)
	*precompLen = actualLen;

    أرجع نتيجو;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    ساكن char_u *
mac_utf16_منزل_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_ڳڻپ		utf8_len;
    Byte_ڳڻپ		داخلا_Read;
    char_u		*نتيجو;

    إذا (g_UTF16_منزل_UTF8_Converter)
    {
	نتيجو = alloc(fromLen * 6 + 1);
	إذا (نتيجو && TECConvert_متن(g_UTF16_منزل_UTF8_Converter, (Const_متن_Ptr)from,
		    fromLen, &داخلا_Read, نتيجو,
		    (fromLen*6+1)*حجم(char_u), &utf8_len) == noErr)
	{
	    TECFlush_متن(g_UTF16_منزل_UTF8_Converter, نتيجو, (fromLen*6+1)*حجم(char_u), &داخلا_Read);
	    utf8_len += داخلا_Read;
	}
	وإلا
	    VIM_CLEAR(نتيجو);
    }
    وإلا
    {
	نتيجو = عدم;
    }

    إذا (actualLen)
	*actualLen = نتيجو ? utf8_len : 0;

    أرجع نتيجو;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    ساكن UniChar *
mac_utf8_منزل_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *نتيجو = عدم;

    utf8_str = CFString_Create_With_بائيٽون(عدم, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    إذا (utf8_str == عدم)
    {
	إذا (actualLen)
	    *actualLen = 0;
	أرجع عدم;
    }

    convertRange = CFRangeMake(0, CFString_Get_ڊيگهائي(utf8_str));
    نتيجو = ALLOC_MULT(UniChar, convertRange.ڊيگهائي);

    CFStringGetCharacters(utf8_str, convertRange, نتيجو);

    CFRelease(utf8_str);

    إذا (actualLen)
	*actualLen = convertRange.ڊيگهائي * حجم(UniChar);

    أرجع نتيجو;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    فراغ
mac_lang_init(فراغ)
{
    إذا (mch_getenv((char_u *)"LANG") != عدم)
	أرجع;

    حرف	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    إذا (LocaleRefGetPartString(عدم,
		k_Locale_Language_نقاب | k_Locale_Language_Variant_نقاب |
		k_Locale_Region_نقاب | k_Locale_Region_Variant_نقاب,
		حجم(buf) - 10, buf) == noErr && *buf)
    {
	إذا (strcasestr(buf, "utf-8") == عدم)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#جيڪڏهن_وصف_ٿيل HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#جيڪڏهن_پورو
#إذا defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#جيڪڏهن_پورو
    }
}
#جيڪڏهن_پورو // MACOS_CONVERT
