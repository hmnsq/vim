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

#شامل "vim.h"

#شامل <CoreServices/CoreServices.h>


#اگر defined(MACOS_CONVERT)

ایستا char_u	    *mac_utf16_to_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
ایستا UniChar	    *mac_utf8_to_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
ایستا TECObjectRef gPathConverter;
// Converter used by mac_utf16_to_utf8
ایستا TECObjectRef gUTF16ToUTF8Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    صحیح			len,
    صحیح			*lenp,
    صحیح			fail_on_error,
    صحیح			from_enc,
    صحیح			to_enc,
    صحیح			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    صحیح			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    انتخاب (from_enc)
    {
	حالت 'l':   from = kCFStringEncodingISOLatin1; بشکن;
	حالت 'm':   from = kCFStringEncodingMacRoman; بشکن;
	حالت 'u':   from = kCFStringEncodingUTF8; بشکن;
	پیشفرض:    بازگشت پوچ;
    }
    انتخاب (to_enc)
    {
	حالت 'l':   to = kCFStringEncodingISOLatin1; بشکن;
	حالت 'm':   to = kCFStringEncodingMacRoman; بشکن;
	حالت 'u':   to = kCFStringEncodingUTF8; بشکن;
	پیشفرض:    بازگشت پوچ;
    }

    اگر (unconvlenp != پوچ)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(پوچ, ptr, len, from, 0);

    اگر (cfstr == پوچ)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    وقتی (cfstr == پوچ && unconvlenp != پوچ && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(پوچ, ptr, len, from, 0);
    }
    اگر (cfstr == پوچ)
	بازگشت پوچ;

    اگر (to == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    وگرنه
	buflen = len + 1;
    retval = alloc(buflen);
    اگر (retval == پوچ)
    {
	CFRelease(cfstr);
	بازگشت پوچ;
    }

#اگر 0
    CFRange convertRange = CFRangeMake(0, CFStringGetLength(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, پوچ, FALSE, پوچ, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : پوچ;
    اگر (retval == پوچ)
    {
	CFRelease(cfstr);
	بازگشت پوچ;
    }

    اگر (lenp)
	*lenp = buflen / اندازه(char_u);

    اگر (!CFStringGetBytes(cfstr, convertRange, to, پوچ, FALSE, retval, buflen, پوچ))
#endif
    اگر (!CFStringGetCString(cfstr, (نویسه *)retval, buflen, to))
    {
	CFRelease(cfstr);
	اگر (fail_on_error)
	{
	    vim_free(retval);
	    بازگشت پوچ;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	برای (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    اگر (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    وگرنه
		l = 1;
	    cfstr = CFStringCreateWithBytes(پوچ, ptr + in, l, from, 0);
	    اگر (cfstr == پوچ)
	    {
		*d++ = '?';
		out++;
	    }
	    وگرنه
	    {
		اگر (!CFStringGetCString(cfstr, (نویسه *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		وگرنه
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
	اگر (lenp != پوچ)
	    *lenp = out;
	بازگشت retval;
    }
    CFRelease(cfstr);
    اگر (lenp != پوچ)
	*lenp = STRLEN(retval);

    بازگشت retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    صحیح
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
    cfstr = CFStringCreateWithBytes(پوچ, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    اگر (cfstr == پوچ)
	بازگشت FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    اگر (r.length != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_size - *sizep, &len))
    {
	CFRelease(cfstr);
	بازگشت FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    بازگشت OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    صحیح
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*to,
    صحیح		*tolenp,
    صحیح		maxtolen,
    char_u	*rest,
    صحیح		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(پوچ, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    وقتی (cfstr == پوچ && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(پوچ, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    اگر (cfstr == پوچ)
	بازگشت FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    اگر (r.length != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
    {
	CFRelease(cfstr);
	بازگشت FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    بازگشت OK;
}

/*
 * Initializes text converters
 */
    تهی
mac_conv_init(تهی)
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

    اگر (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = پوچ;

    اگر (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	اگر (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    gUTF16ToUTF8Converter = پوچ;
    }
}

/*
 * Destroys text converters
 */
    تهی
mac_conv_cleanup(تهی)
{
    اگر (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = پوچ;
    }

    اگر (gPathConverter)
    {
	TECDisposeConverter(gPathConverter);
	gPathConverter = پوچ;
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
    char_u      *result = پوچ;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_to_utf8(from, fromLen, &utf8_len);

    اگر (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	اگر ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	اگر (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    result = utf8_str;
	}
	وگرنه
	{
	    صحیح len = utf8_len;
	    result = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, پوچ, پوچ);

	اگر (actualLen)
	    *actualLen = utf8_len;
    }
    وگرنه اگر (actualLen)
	*actualLen = 0;

    بازگشت result;
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
    UniChar     *result = پوچ;
    Boolean     should_free_utf8 = FALSE;

    انجامبده
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	اگر ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    بشکن;

	اگر (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (صحیح *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	وگرنه
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	اگر (utf8_str == پوچ)
	    بشکن;

	convert_setup(&conv, پوچ, پوچ);

	result = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	اگر (should_free_utf8)
	    vim_free(utf8_str);
	بازگشت result;
    }
    وقتی (0);

    اگر (actualLen)
	*actualLen = 0;

    بازگشت result;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    تهی *
mac_enc_to_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	result = پوچ;

    utf16_str = mac_enc_to_utf16(from, fromLen, &utf16_len);
    اگر (utf16_str)
    {
	result = CFStringCreateWithCharacters(پوچ, utf16_str, utf16_len/اندازه(UniChar));
	vim_free(utf16_str);
    }

    بازگشت (تهی *)result;
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
    char_u  *result = پوچ;
    size_t  actualLen = 0;

    اگر (gPathConverter)
    {
	result = alloc(decompLen);
	اگر (result)
	{
	    اگر (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, result,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(result);
	}
    }

    اگر (precompLen)
	*precompLen = actualLen;

    بازگشت result;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    ایستا char_u *
mac_utf16_to_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_شمار		utf8_len;
    Byte_شمار		درون_داد_Read;
    char_u		*result;

    اگر (gUTF16ToUTF8Converter)
    {
	result = alloc(fromLen * 6 + 1);
	اگر (result && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &درون_داد_Read, result,
		    (fromLen*6+1)*اندازه(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, result, (fromLen*6+1)*اندازه(char_u), &درون_داد_Read);
	    utf8_len += درون_داد_Read;
	}
	وگرنه
	    VIM_CLEAR(result);
    }
    وگرنه
    {
	result = پوچ;
    }

    اگر (actualLen)
	*actualLen = result ? utf8_len : 0;

    بازگشت result;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    ایستا UniChar *
mac_utf8_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *result = پوچ;

    utf8_str = CFStringCreateWithBytes(پوچ, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    اگر (utf8_str == پوچ)
    {
	اگر (actualLen)
	    *actualLen = 0;
	بازگشت پوچ;
    }

    convertRange = CFRangeMake(0, CFStringGetLength(utf8_str));
    result = ALLOC_MULT(UniChar, convertRange.length);

    CFStringGetCharacters(utf8_str, convertRange, result);

    CFRelease(utf8_str);

    اگر (actualLen)
	*actualLen = convertRange.length * اندازه(UniChar);

    بازگشت result;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    تهی
mac_lang_init(تهی)
{
    اگر (mch_getenv((char_u *)"LANG") != پوچ)
	بازگشت;

    نویسه	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    اگر (LocaleRefGetPartString(پوچ,
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		اندازه(buf) - 10, buf) == noErr && *buf)
    {
	اگر (strcasestr(buf, "utf-8") == پوچ)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#اگر defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
