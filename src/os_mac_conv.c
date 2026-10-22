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

#அந்தர்பாவ "vim.h"

#அந்தர்பாவ <CoreServices/CoreServices.h>


#எனில் defined(MACOS_CONVERT)

நிலையான char_u	    *mac_utf16_to_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
நிலையான UniChar	    *mac_utf8_to_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
நிலையான TECObjectRef gPathConverter;
// Converter used by mac_utf16_to_utf8
நிலையான TECObjectRef gUTF16ToUTF8Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    முழு			len,
    முழு			*lenp,
    முழு			fail_on_error,
    முழு			from_enc,
    முழு			to_enc,
    முழு			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    முழு			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    தேர்வு (from_enc)
    {
	நிலை 'l':   from = kCFStringEncodingISOLatin1; நிறுத்து;
	நிலை 'm':   from = kCFStringEncodingMacRoman; நிறுத்து;
	நிலை 'u':   from = kCFStringEncodingUTF8; நிறுத்து;
	இயல்பு:    திரும்பு வெற்று;
    }
    தேர்வு (to_enc)
    {
	நிலை 'l':   to = kCFStringEncodingISOLatin1; நிறுத்து;
	நிலை 'm':   to = kCFStringEncodingMacRoman; நிறுத்து;
	நிலை 'u':   to = kCFStringEncodingUTF8; நிறுத்து;
	இயல்பு:    திரும்பு வெற்று;
    }

    எனில் (unconvlenp != வெற்று)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(வெற்று, ptr, len, from, 0);

    எனில் (cfstr == வெற்று)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    போது (cfstr == வெற்று && unconvlenp != வெற்று && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(வெற்று, ptr, len, from, 0);
    }
    எனில் (cfstr == வெற்று)
	திரும்பு வெற்று;

    எனில் (to == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    இல்லையெனில்
	buflen = len + 1;
    retval = alloc(buflen);
    எனில் (retval == வெற்று)
    {
	CFRelease(cfstr);
	திரும்பு வெற்று;
    }

#எனில் 0
    CFRange convertRange = CFRangeMake(0, CFStringGetLength(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, வெற்று, FALSE, வெற்று, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : வெற்று;
    எனில் (retval == வெற்று)
    {
	CFRelease(cfstr);
	திரும்பு வெற்று;
    }

    எனில் (lenp)
	*lenp = buflen / அளவு(char_u);

    எனில் (!CFStringGetBytes(cfstr, convertRange, to, வெற்று, FALSE, retval, buflen, வெற்று))
#endif
    எனில் (!CFStringGetCString(cfstr, (எழுத்து *)retval, buflen, to))
    {
	CFRelease(cfstr);
	எனில் (fail_on_error)
	{
	    vim_free(retval);
	    திரும்பு வெற்று;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	க்கு (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    எனில் (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    இல்லையெனில்
		l = 1;
	    cfstr = CFStringCreateWithBytes(வெற்று, ptr + in, l, from, 0);
	    எனில் (cfstr == வெற்று)
	    {
		*d++ = '?';
		out++;
	    }
	    இல்லையெனில்
	    {
		எனில் (!CFStringGetCString(cfstr, (எழுத்து *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		இல்லையெனில்
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
	எனில் (lenp != வெற்று)
	    *lenp = out;
	திரும்பு retval;
    }
    CFRelease(cfstr);
    எனில் (lenp != வெற்று)
	*lenp = STRLEN(retval);

    திரும்பு retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    முழு
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
    cfstr = CFStringCreateWithBytes(வெற்று, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    எனில் (cfstr == வெற்று)
	திரும்பு FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    எனில் (r.length != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_size - *sizep, &len))
    {
	CFRelease(cfstr);
	திரும்பு FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    திரும்பு OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    முழு
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*to,
    முழு		*tolenp,
    முழு		maxtolen,
    char_u	*rest,
    முழு		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(வெற்று, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    போது (cfstr == வெற்று && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(வெற்று, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    எனில் (cfstr == வெற்று)
	திரும்பு FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    எனில் (r.length != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
    {
	CFRelease(cfstr);
	திரும்பு FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    திரும்பு OK;
}

/*
 * Initializes text converters
 */
    வெறுமை
mac_conv_init(வெறுமை)
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

    எனில் (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = வெற்று;

    எனில் (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	எனில் (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    gUTF16ToUTF8Converter = வெற்று;
    }
}

/*
 * Destroys text converters
 */
    வெறுமை
mac_conv_cleanup(வெறுமை)
{
    எனில் (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = வெற்று;
    }

    எனில் (gPathConverter)
    {
	TECDisposeConverter(gPathConverter);
	gPathConverter = வெற்று;
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
    char_u      *result = வெற்று;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_to_utf8(from, fromLen, &utf8_len);

    எனில் (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	எனில் ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	எனில் (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    result = utf8_str;
	}
	இல்லையெனில்
	{
	    முழு len = utf8_len;
	    result = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, வெற்று, வெற்று);

	எனில் (actualLen)
	    *actualLen = utf8_len;
    }
    இல்லையெனில் எனில் (actualLen)
	*actualLen = 0;

    திரும்பு result;
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
    UniChar     *result = வெற்று;
    Boolean     should_free_utf8 = FALSE;

    செய்
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	எனில் ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    நிறுத்து;

	எனில் (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (முழு *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	இல்லையெனில்
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	எனில் (utf8_str == வெற்று)
	    நிறுத்து;

	convert_setup(&conv, வெற்று, வெற்று);

	result = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	எனில் (should_free_utf8)
	    vim_free(utf8_str);
	திரும்பு result;
    }
    போது (0);

    எனில் (actualLen)
	*actualLen = 0;

    திரும்பு result;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    வெறுமை *
mac_enc_to_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	result = வெற்று;

    utf16_str = mac_enc_to_utf16(from, fromLen, &utf16_len);
    எனில் (utf16_str)
    {
	result = CFStringCreateWithCharacters(வெற்று, utf16_str, utf16_len/அளவு(UniChar));
	vim_free(utf16_str);
    }

    திரும்பு (வெறுமை *)result;
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
    char_u  *result = வெற்று;
    size_t  actualLen = 0;

    எனில் (gPathConverter)
    {
	result = alloc(decompLen);
	எனில் (result)
	{
	    எனில் (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, result,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(result);
	}
    }

    எனில் (precompLen)
	*precompLen = actualLen;

    திரும்பு result;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    நிலையான char_u *
mac_utf16_to_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_மொத்தம்		utf8_len;
    Byte_மொத்தம்		உள்ளீடு_Read;
    char_u		*result;

    எனில் (gUTF16ToUTF8Converter)
    {
	result = alloc(fromLen * 6 + 1);
	எனில் (result && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &உள்ளீடு_Read, result,
		    (fromLen*6+1)*அளவு(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, result, (fromLen*6+1)*அளவு(char_u), &உள்ளீடு_Read);
	    utf8_len += உள்ளீடு_Read;
	}
	இல்லையெனில்
	    VIM_CLEAR(result);
    }
    இல்லையெனில்
    {
	result = வெற்று;
    }

    எனில் (actualLen)
	*actualLen = result ? utf8_len : 0;

    திரும்பு result;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    நிலையான UniChar *
mac_utf8_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *result = வெற்று;

    utf8_str = CFStringCreateWithBytes(வெற்று, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    எனில் (utf8_str == வெற்று)
    {
	எனில் (actualLen)
	    *actualLen = 0;
	திரும்பு வெற்று;
    }

    convertRange = CFRangeMake(0, CFStringGetLength(utf8_str));
    result = ALLOC_MULT(UniChar, convertRange.length);

    CFStringGetCharacters(utf8_str, convertRange, result);

    CFRelease(utf8_str);

    எனில் (actualLen)
	*actualLen = convertRange.length * அளவு(UniChar);

    திரும்பு result;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    வெறுமை
mac_lang_init(வெறுமை)
{
    எனில் (mch_getenv((char_u *)"LANG") != வெற்று)
	திரும்பு;

    எழுத்து	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    எனில் (LocaleRefGetPartString(வெற்று,
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		அளவு(buf) - 10, buf) == noErr && *buf)
    {
	எனில் (strcasestr(buf, "utf-8") == வெற்று)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#எனில் defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
