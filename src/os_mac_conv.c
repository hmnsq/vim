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

#define NO_X11_INCLUDES

#ضمن "vim.h"

#ضمن <CoreServices/CoreServices.h>


#إذا defined(MACOS_CONVERT)

ساكن char_u	    *mac_utf16_to_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
ساكن UniChar	    *mac_utf8_to_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
ساكن TECObjectRef gPathConverter;
// Converter used by mac_utf16_to_utf8
ساكن TECObjectRef gUTF16ToUTF8Converter;

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
    صحيح			to_enc,
    صحيح			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    صحيح			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    بدل (from_enc)
    {
	حالة 'l':   from = kCFStringEncodingISOLatin1; اكسر;
	حالة 'm':   from = kCFStringEncodingMacRoman; اكسر;
	حالة 'u':   from = kCFStringEncodingUTF8; اكسر;
	افتراضي:    أرجع عدم;
    }
    بدل (to_enc)
    {
	حالة 'l':   to = kCFStringEncodingISOLatin1; اكسر;
	حالة 'm':   to = kCFStringEncodingMacRoman; اكسر;
	حالة 'u':   to = kCFStringEncodingUTF8; اكسر;
	افتراضي:    أرجع عدم;
    }

    إذا (unconvlenp != عدم)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(عدم, ptr, len, from, 0);

    إذا (cfstr == عدم)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    طالما (cfstr == عدم && unconvlenp != عدم && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(عدم, ptr, len, from, 0);
    }
    إذا (cfstr == عدم)
	أرجع عدم;

    إذا (to == kCFStringEncodingUTF8)
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
    CFRange convertRange = CFRangeMake(0, CFStringGetLength(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, عدم, FALSE, عدم, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : عدم;
    إذا (retval == عدم)
    {
	CFRelease(cfstr);
	أرجع عدم;
    }

    إذا (lenp)
	*lenp = buflen / حجم(char_u);

    إذا (!CFStringGetBytes(cfstr, convertRange, to, عدم, FALSE, retval, buflen, عدم))
#endif
    إذا (!CFStringGetCString(cfstr, (حرف *)retval, buflen, to))
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
	لكل (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    إذا (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    وإلا
		l = 1;
	    cfstr = CFStringCreateWithBytes(عدم, ptr + in, l, from, 0);
	    إذا (cfstr == عدم)
	    {
		*d++ = '?';
		out++;
	    }
	    وإلا
	    {
		إذا (!CFStringGetCString(cfstr, (حرف *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		وإلا
		{
		    i = STRLEN(d);
		    d += i;
		    out += i;
		}
		CFRelease(cfstr);
	    }
	    in += l;
	}
	*d = NUL;
	إذا (lenp != عدم)
	    *lenp = out;
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
    long	*sizep,
    long	real_size)
{
    CFStringRef		cfstr;
    CFRange		r;
    CFIndex		len = *sizep;

    // MacRoman is an 8-bit encoding, no need to move bytes to
    // conv_rest[].
    cfstr = CFStringCreateWithBytes(عدم, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    إذا (cfstr == عدم)
	أرجع FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    إذا (r.length != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_size - *sizep, &len))
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
    char_u	*to,
    صحيح		*tolenp,
    صحيح		maxtolen,
    char_u	*rest,
    صحيح		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(عدم, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    طالما (cfstr == عدم && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(عدم, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    إذا (cfstr == عدم)
	أرجع FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    إذا (r.length != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
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
    TextEncoding    utf8_encoding;
    TextEncoding    utf8_hfsplus_encoding;
    TextEncoding    utf8_canon_encoding;
    TextEncoding    utf16_encoding;

    utf8_encoding = CreateTextEncoding(kTextEncodingUnicodeDefault,
	    kTextEncodingDefaultVariant, kUnicodeUTF8Format);
    utf8_hfsplus_encoding = CreateTextEncoding(kTextEncodingUnicodeDefault,
	    kUnicodeHFSPlusCompVariant, kUnicodeUTF8Format);
    utf8_canon_encoding = CreateTextEncoding(kTextEncodingUnicodeDefault,
	    kUnicodeCanonicalCompVariant, kUnicodeUTF8Format);
    utf16_encoding = CreateTextEncoding(kTextEncodingUnicodeDefault,
	    kTextEncodingDefaultVariant, kUnicode16BitFormat);

    إذا (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = عدم;

    إذا (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	إذا (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    gUTF16ToUTF8Converter = عدم;
    }
}

/*
 * Destroys text converters
 */
    فراغ
mac_conv_cleanup(فراغ)
{
    إذا (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = عدم;
    }

    إذا (gPathConverter)
    {
	TECDisposeConverter(gPathConverter);
	gPathConverter = عدم;
    }
}

/*
 * Conversion from UTF-16 UniChars to 'encoding'
 * The function signature uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    char_u *
mac_utf16_to_enc(
    unsigned short *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    char_u      *result = عدم;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_to_utf8(from, fromLen, &utf8_len);

    إذا (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	إذا ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	إذا (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    result = utf8_str;
	}
	وإلا
	{
	    صحيح len = utf8_len;
	    result = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, عدم, عدم);

	إذا (actualLen)
	    *actualLen = utf8_len;
    }
    وإلا إذا (actualLen)
	*actualLen = 0;

    أرجع result;
}

/*
 * Conversion from 'encoding' to UTF-16 UniChars
 * The function return uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    unsigned short *
mac_enc_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    UniChar     *result = عدم;
    Boolean     should_free_utf8 = FALSE;

    افعل
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	إذا ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
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

	convert_setup(&conv, عدم, عدم);

	result = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	إذا (should_free_utf8)
	    vim_free(utf8_str);
	أرجع result;
    }
    طالما (0);

    إذا (actualLen)
	*actualLen = 0;

    أرجع result;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    فراغ *
mac_enc_to_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	result = عدم;

    utf16_str = mac_enc_to_utf16(from, fromLen, &utf16_len);
    إذا (utf16_str)
    {
	result = CFStringCreateWithCharacters(عدم, utf16_str, utf16_len/حجم(UniChar));
	vim_free(utf16_str);
    }

    أرجع (فراغ *)result;
}

/*
 * Converts a decomposed HFS+ UTF-8 path to precomposed UTF-8
 */
    char_u *
mac_precompose_path(
    char_u  *decompPath,
    size_t  decompLen,
    size_t  *precompLen)
{
    char_u  *result = عدم;
    size_t  actualLen = 0;

    إذا (gPathConverter)
    {
	result = alloc(decompLen);
	إذا (result)
	{
	    إذا (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, result,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(result);
	}
    }

    إذا (precompLen)
	*precompLen = actualLen;

    أرجع result;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    ساكن char_u *
mac_utf16_to_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_ژماردن		utf8_len;
    Byte_ژماردن		تێخستن_Read;
    char_u		*result;

    إذا (gUTF16ToUTF8Converter)
    {
	result = alloc(fromLen * 6 + 1);
	إذا (result && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &تێخستن_Read, result,
		    (fromLen*6+1)*حجم(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, result, (fromLen*6+1)*حجم(char_u), &تێخستن_Read);
	    utf8_len += تێخستن_Read;
	}
	وإلا
	    VIM_CLEAR(result);
    }
    وإلا
    {
	result = عدم;
    }

    إذا (actualLen)
	*actualLen = result ? utf8_len : 0;

    أرجع result;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    ساكن UniChar *
mac_utf8_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *result = عدم;

    utf8_str = CFStringCreateWithBytes(عدم, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    إذا (utf8_str == عدم)
    {
	إذا (actualLen)
	    *actualLen = 0;
	أرجع عدم;
    }

    convertRange = CFRangeMake(0, CFStringGetLength(utf8_str));
    result = ALLOC_MULT(UniChar, convertRange.length);

    CFStringGetCharacters(utf8_str, convertRange, result);

    CFRelease(utf8_str);

    إذا (actualLen)
	*actualLen = convertRange.length * حجم(UniChar);

    أرجع result;
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
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		حجم(buf) - 10, buf) == noErr && *buf)
    {
	إذا (strcasestr(buf, "utf-8") == عدم)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#إذا defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
