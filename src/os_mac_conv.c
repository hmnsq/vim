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

#تعریف_کړه NO_X11_INCLUDES

#ضمن "vim.h"

#ضمن <CoreServices/CoreServices.h>


#إذا defined(MACOS_CONVERT)

ساكن char_u	    *mac_utf16_منزل_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
ساكن UniChar	    *mac_utf8_منزل_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
ساكن TECObjectRef g_لار_Converter;
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
    صحيح			buflen, دننه, بهر, l, i;
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
    cfstr = CFString_Create_With_بایټونه(عدم, ptr, len, from, 0);

    إذا (cfstr == عدم)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    طالما (cfstr == عدم && unconvlenp != عدم && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFString_Create_With_بایټونه(عدم, ptr, len, from, 0);
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
    CFRange convertRange = CFRangeMake(0, CFString_Get_اوږدتیا(cfstr));
    //  Determine output buffer size
    CFString_Get_بایټونه(cfstr, convertRange, منزل, عدم, FALSE, عدم, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : عدم;
    إذا (retval == عدم)
    {
	CFRelease(cfstr);
	أرجع عدم;
    }

    إذا (lenp)
	*lenp = buflen / حجم(char_u);

    إذا (!CFString_Get_بایټونه(cfstr, convertRange, منزل, عدم, FALSE, retval, buflen, عدم))
#د_که_پای
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
	لكل (d = retval, دننه = 0, بهر = 0; دننه < len && بهر < buflen - 1;)
	{
	    إذا (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + دننه);
	    وإلا
		l = 1;
	    cfstr = CFString_Create_With_بایټونه(عدم, ptr + دننه, l, from, 0);
	    إذا (cfstr == عدم)
	    {
		*d++ = '?';
		بهر++;
	    }
	    وإلا
	    {
		إذا (!CFStringGetCString(cfstr, (حرف *)d, buflen - بهر, منزل))
		{
		    *d++ = '?';
		    بهر++;
		}
		وإلا
		{
		    i = STRLEN(d);
		    d += i;
		    بهر += i;
		}
		CFRelease(cfstr);
	    }
	    دننه += l;
	}
	*d = NUL;
	إذا (lenp != عدم)
	    *lenp = بهر;
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
    اوږد	*sizep,
    اوږد	real_اندازه)
{
    CFStringRef		cfstr;
    CFRange		r;
    CFIndex		len = *sizep;

    // MacRoman is an 8-bit encoding, no need to move bytes to
    // conv_rest[].
    cfstr = CFString_Create_With_بایټونه(عدم, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    إذا (cfstr == عدم)
	أرجع FAIL;

    r.location = 0;
    r.اوږدتیا = CFString_Get_اوږدتیا(cfstr);
    إذا (r.اوږدتیا != CFString_Get_بایټونه(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_اندازه - *sizep, &len))
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
    cfstr = CFString_Create_With_بایټونه(عدم, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    طالما (cfstr == عدم && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFString_Create_With_بایټونه(عدم, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    إذا (cfstr == عدم)
	أرجع FAIL;

    r.location = 0;
    r.اوږدتیا = CFString_Get_اوږدتیا(cfstr);
    إذا (r.اوږدتیا != CFString_Get_بایټونه(cfstr, r,
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
	    k_متن_Encoding_Default_Variant, k_Unicode16_بټ_Format);

    إذا (TECCreateConverter(&g_لار_Converter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	g_لار_Converter = عدم;

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

    إذا (g_لار_Converter)
    {
	TECDisposeConverter(g_لار_Converter);
	g_لار_Converter = عدم;
    }
}

/*
 * Conversion from UTF-16 UniChars to 'encoding'
 * The function signature uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    char_u *
mac_utf16_منزل_enc(
    بینښه لنډ *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    char_u      *پایله = عدم;

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
	    convert_تیاری(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	إذا (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    پایله = utf8_str;
	}
	وإلا
	{
	    صحيح len = utf8_len;
	    پایله = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_تیاری(&conv, عدم, عدم);

	إذا (actualLen)
	    *actualLen = utf8_len;
    }
    وإلا إذا (actualLen)
	*actualLen = 0;

    أرجع پایله;
}

/*
 * Conversion from 'encoding' to UTF-16 UniChars
 * The function return uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    بینښه لنډ *
mac_enc_منزل_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    UniChar     *پایله = عدم;
    Boolean     should_free_utf8 = FALSE;

    افعل
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	إذا ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_تیاری(&conv, p_enc ? p_enc : (char_u *)"macroman",
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

	convert_تیاری(&conv, عدم, عدم);

	پایله = mac_utf8_منزل_utf16(utf8_str, utf8_len, actualLen);

	إذا (should_free_utf8)
	    vim_free(utf8_str);
	أرجع پایله;
    }
    طالما (0);

    إذا (actualLen)
	*actualLen = 0;

    أرجع پایله;
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
    CFStringRef	پایله = عدم;

    utf16_str = mac_enc_منزل_utf16(from, fromLen, &utf16_len);
    إذا (utf16_str)
    {
	پایله = CFStringCreateWithCharacters(عدم, utf16_str, utf16_len/حجم(UniChar));
	vim_free(utf16_str);
    }

    أرجع (فراغ *)پایله;
}

/*
 * Converts a decomposed HFS+ UTF-8 path to precomposed UTF-8
 */
    char_u *
mac_precompose_لار(
    char_u  *decomp_لار,
    size_t  decompLen,
    size_t  *precompLen)
{
    char_u  *پایله = عدم;
    size_t  actualLen = 0;

    إذا (g_لار_Converter)
    {
	پایله = alloc(decompLen);
	إذا (پایله)
	{
	    إذا (TECConvert_متن(g_لار_Converter, decomp_لار,
			decompLen, &decompLen, پایله,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(پایله);
	}
    }

    إذا (precompLen)
	*precompLen = actualLen;

    أرجع پایله;
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
    Byte_شمېر		utf8_len;
    Byte_شمېر		ننوت_Read;
    char_u		*پایله;

    إذا (g_UTF16_منزل_UTF8_Converter)
    {
	پایله = alloc(fromLen * 6 + 1);
	إذا (پایله && TECConvert_متن(g_UTF16_منزل_UTF8_Converter, (Const_متن_Ptr)from,
		    fromLen, &ننوت_Read, پایله,
		    (fromLen*6+1)*حجم(char_u), &utf8_len) == noErr)
	{
	    TECFlush_متن(g_UTF16_منزل_UTF8_Converter, پایله, (fromLen*6+1)*حجم(char_u), &ننوت_Read);
	    utf8_len += ننوت_Read;
	}
	وإلا
	    VIM_CLEAR(پایله);
    }
    وإلا
    {
	پایله = عدم;
    }

    إذا (actualLen)
	*actualLen = پایله ? utf8_len : 0;

    أرجع پایله;
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
    UniChar      *پایله = عدم;

    utf8_str = CFString_Create_With_بایټونه(عدم, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    إذا (utf8_str == عدم)
    {
	إذا (actualLen)
	    *actualLen = 0;
	أرجع عدم;
    }

    convertRange = CFRangeMake(0, CFString_Get_اوږدتیا(utf8_str));
    پایله = ALLOC_MULT(UniChar, convertRange.اوږدتیا);

    CFStringGetCharacters(utf8_str, convertRange, پایله);

    CFRelease(utf8_str);

    إذا (actualLen)
	*actualLen = convertRange.اوږدتیا * حجم(UniChar);

    أرجع پایله;
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
		k_Locale_Language_ماسک | k_Locale_Language_Variant_ماسک |
		k_Locale_Region_ماسک | k_Locale_Region_Variant_ماسک,
		حجم(buf) - 10, buf) == noErr && *buf)
    {
	إذا (strcasestr(buf, "utf-8") == عدم)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#که_تعریف_شوی HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#د_که_پای
#إذا defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#د_که_پای
    }
}
#د_که_پای // MACOS_CONVERT
