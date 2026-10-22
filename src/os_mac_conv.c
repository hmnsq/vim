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

#अन्तर्भाव "vim.h"

#अन्तर्भाव <CoreServices/CoreServices.h>


#यदि defined(MACOS_CONVERT)

स्थिर char_u	    *mac_utf16_to_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
स्थिर UniChar	    *mac_utf8_to_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
स्थिर TECObjectRef gPathConverter;
// Converter used by mac_utf16_to_utf8
स्थिर TECObjectRef gUTF16ToUTF8Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    पूर्णाङ्क			len,
    पूर्णाङ्क			*lenp,
    पूर्णाङ्क			fail_on_error,
    पूर्णाङ्क			from_enc,
    पूर्णाङ्क			to_enc,
    पूर्णाङ्क			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    पूर्णाङ्क			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    विकल्प (from_enc)
    {
	पक्ष 'l':   from = kCFStringEncodingISOLatin1; भङ्ग;
	पक्ष 'm':   from = kCFStringEncodingMacRoman; भङ्ग;
	पक्ष 'u':   from = kCFStringEncodingUTF8; भङ्ग;
	स्वभाव:    प्रत्यागम रिक्त;
    }
    विकल्प (to_enc)
    {
	पक्ष 'l':   to = kCFStringEncodingISOLatin1; भङ्ग;
	पक्ष 'm':   to = kCFStringEncodingMacRoman; भङ्ग;
	पक्ष 'u':   to = kCFStringEncodingUTF8; भङ्ग;
	स्वभाव:    प्रत्यागम रिक्त;
    }

    यदि (unconvlenp != रिक्त)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(रिक्त, ptr, len, from, 0);

    यदि (cfstr == रिक्त)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    यावत् (cfstr == रिक्त && unconvlenp != रिक्त && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(रिक्त, ptr, len, from, 0);
    }
    यदि (cfstr == रिक्त)
	प्रत्यागम रिक्त;

    यदि (to == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    अन्यथा
	buflen = len + 1;
    retval = alloc(buflen);
    यदि (retval == रिक्त)
    {
	CFRelease(cfstr);
	प्रत्यागम रिक्त;
    }

#यदि 0
    CFRange convertRange = CFRangeMake(0, CFStringGetLength(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, रिक्त, FALSE, रिक्त, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : रिक्त;
    यदि (retval == रिक्त)
    {
	CFRelease(cfstr);
	प्रत्यागम रिक्त;
    }

    यदि (lenp)
	*lenp = buflen / परिमाण(char_u);

    यदि (!CFStringGetBytes(cfstr, convertRange, to, रिक्त, FALSE, retval, buflen, रिक्त))
#endif
    यदि (!CFStringGetCString(cfstr, (वर्ण *)retval, buflen, to))
    {
	CFRelease(cfstr);
	यदि (fail_on_error)
	{
	    vim_free(retval);
	    प्रत्यागम रिक्त;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	प्रति (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    यदि (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    अन्यथा
		l = 1;
	    cfstr = CFStringCreateWithBytes(रिक्त, ptr + in, l, from, 0);
	    यदि (cfstr == रिक्त)
	    {
		*d++ = '?';
		out++;
	    }
	    अन्यथा
	    {
		यदि (!CFStringGetCString(cfstr, (वर्ण *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		अन्यथा
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
	यदि (lenp != रिक्त)
	    *lenp = out;
	प्रत्यागम retval;
    }
    CFRelease(cfstr);
    यदि (lenp != रिक्त)
	*lenp = STRLEN(retval);

    प्रत्यागम retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
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
    cfstr = CFStringCreateWithBytes(रिक्त, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    यदि (cfstr == रिक्त)
	प्रत्यागम FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    यदि (r.length != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_size - *sizep, &len))
    {
	CFRelease(cfstr);
	प्रत्यागम FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    प्रत्यागम OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*to,
    पूर्णाङ्क		*tolenp,
    पूर्णाङ्क		maxtolen,
    char_u	*rest,
    पूर्णाङ्क		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(रिक्त, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    यावत् (cfstr == रिक्त && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(रिक्त, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    यदि (cfstr == रिक्त)
	प्रत्यागम FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    यदि (r.length != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
    {
	CFRelease(cfstr);
	प्रत्यागम FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    प्रत्यागम OK;
}

/*
 * Initializes text converters
 */
    शून्य
mac_conv_init(शून्य)
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

    यदि (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = रिक्त;

    यदि (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	यदि (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    gUTF16ToUTF8Converter = रिक्त;
    }
}

/*
 * Destroys text converters
 */
    शून्य
mac_conv_cleanup(शून्य)
{
    यदि (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = रिक्त;
    }

    यदि (gPathConverter)
    {
	TECDisposeConverter(gPathConverter);
	gPathConverter = रिक्त;
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
    char_u      *result = रिक्त;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_to_utf8(from, fromLen, &utf8_len);

    यदि (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	यदि ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	यदि (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    result = utf8_str;
	}
	अन्यथा
	{
	    पूर्णाङ्क len = utf8_len;
	    result = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, रिक्त, रिक्त);

	यदि (actualLen)
	    *actualLen = utf8_len;
    }
    अन्यथा यदि (actualLen)
	*actualLen = 0;

    प्रत्यागम result;
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
    UniChar     *result = रिक्त;
    Boolean     should_free_utf8 = FALSE;

    कुरु
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	यदि ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    भङ्ग;

	यदि (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (पूर्णाङ्क *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	अन्यथा
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	यदि (utf8_str == रिक्त)
	    भङ्ग;

	convert_setup(&conv, रिक्त, रिक्त);

	result = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	यदि (should_free_utf8)
	    vim_free(utf8_str);
	प्रत्यागम result;
    }
    यावत् (0);

    यदि (actualLen)
	*actualLen = 0;

    प्रत्यागम result;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    शून्य *
mac_enc_to_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	result = रिक्त;

    utf16_str = mac_enc_to_utf16(from, fromLen, &utf16_len);
    यदि (utf16_str)
    {
	result = CFStringCreateWithCharacters(रिक्त, utf16_str, utf16_len/परिमाण(UniChar));
	vim_free(utf16_str);
    }

    प्रत्यागम (शून्य *)result;
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
    char_u  *result = रिक्त;
    size_t  actualLen = 0;

    यदि (gPathConverter)
    {
	result = alloc(decompLen);
	यदि (result)
	{
	    यदि (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, result,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(result);
	}
    }

    यदि (precompLen)
	*precompLen = actualLen;

    प्रत्यागम result;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    स्थिर char_u *
mac_utf16_to_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_गिनती		utf8_len;
    Byte_गिनती		निवेश_Read;
    char_u		*result;

    यदि (gUTF16ToUTF8Converter)
    {
	result = alloc(fromLen * 6 + 1);
	यदि (result && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &निवेश_Read, result,
		    (fromLen*6+1)*परिमाण(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, result, (fromLen*6+1)*परिमाण(char_u), &निवेश_Read);
	    utf8_len += निवेश_Read;
	}
	अन्यथा
	    VIM_CLEAR(result);
    }
    अन्यथा
    {
	result = रिक्त;
    }

    यदि (actualLen)
	*actualLen = result ? utf8_len : 0;

    प्रत्यागम result;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    स्थिर UniChar *
mac_utf8_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *result = रिक्त;

    utf8_str = CFStringCreateWithBytes(रिक्त, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    यदि (utf8_str == रिक्त)
    {
	यदि (actualLen)
	    *actualLen = 0;
	प्रत्यागम रिक्त;
    }

    convertRange = CFRangeMake(0, CFStringGetLength(utf8_str));
    result = ALLOC_MULT(UniChar, convertRange.length);

    CFStringGetCharacters(utf8_str, convertRange, result);

    CFRelease(utf8_str);

    यदि (actualLen)
	*actualLen = convertRange.length * परिमाण(UniChar);

    प्रत्यागम result;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    शून्य
mac_lang_init(शून्य)
{
    यदि (mch_getenv((char_u *)"LANG") != रिक्त)
	प्रत्यागम;

    वर्ण	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    यदि (LocaleRefGetPartString(रिक्त,
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		परिमाण(buf) - 10, buf) == noErr && *buf)
    {
	यदि (strcasestr(buf, "utf-8") == रिक्त)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#यदि defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
