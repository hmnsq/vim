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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <CoreServices/CoreServices.h>


#ಯದಿ defined(MACOS_CONVERT)

ಸ್ಥಿರ char_u	    *mac_utf16_to_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
ಸ್ಥಿರ UniChar	    *mac_utf8_to_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
ಸ್ಥಿರ TECObjectRef gPathConverter;
// Converter used by mac_utf16_to_utf8
ಸ್ಥಿರ TECObjectRef gUTF16ToUTF8Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    ಪೂರ್ಣಾಙ್ಕ			len,
    ಪೂರ್ಣಾಙ್ಕ			*lenp,
    ಪೂರ್ಣಾಙ್ಕ			fail_on_error,
    ಪೂರ್ಣಾಙ್ಕ			from_enc,
    ಪೂರ್ಣಾಙ್ಕ			to_enc,
    ಪೂರ್ಣಾಙ್ಕ			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    ಪೂರ್ಣಾಙ್ಕ			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    ವಿಕಲ್ಪ (from_enc)
    {
	ಪಕ್ಷ 'l':   from = kCFStringEncodingISOLatin1; ಭಙ್ಗ;
	ಪಕ್ಷ 'm':   from = kCFStringEncodingMacRoman; ಭಙ್ಗ;
	ಪಕ್ಷ 'u':   from = kCFStringEncodingUTF8; ಭಙ್ಗ;
	ಸ್ವಭಾವ:    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ವಿಕಲ್ಪ (to_enc)
    {
	ಪಕ್ಷ 'l':   to = kCFStringEncodingISOLatin1; ಭಙ್ಗ;
	ಪಕ್ಷ 'm':   to = kCFStringEncodingMacRoman; ಭಙ್ಗ;
	ಪಕ್ಷ 'u':   to = kCFStringEncodingUTF8; ಭಙ್ಗ;
	ಸ್ವಭಾವ:    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (unconvlenp != ರಿಕ್ತ)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(ರಿಕ್ತ, ptr, len, from, 0);

    ಯದಿ (cfstr == ರಿಕ್ತ)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    ಯಾವತ್ (cfstr == ರಿಕ್ತ && unconvlenp != ರಿಕ್ತ && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(ರಿಕ್ತ, ptr, len, from, 0);
    }
    ಯದಿ (cfstr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (to == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    ಅನ್ಯಥಾ
	buflen = len + 1;
    retval = alloc(buflen);
    ಯದಿ (retval == ರಿಕ್ತ)
    {
	CFRelease(cfstr);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

#ಯದಿ 0
    CFRange convertRange = CFRangeMake(0, CFStringGetLength(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, ರಿಕ್ತ, FALSE, ರಿಕ್ತ, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : ರಿಕ್ತ;
    ಯದಿ (retval == ರಿಕ್ತ)
    {
	CFRelease(cfstr);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (lenp)
	*lenp = buflen / ಪರಿಮಾಣ(char_u);

    ಯದಿ (!CFStringGetBytes(cfstr, convertRange, to, ರಿಕ್ತ, FALSE, retval, buflen, ರಿಕ್ತ))
#endif
    ಯದಿ (!CFStringGetCString(cfstr, (ವರ್ಣ *)retval, buflen, to))
    {
	CFRelease(cfstr);
	ಯದಿ (fail_on_error)
	{
	    vim_free(retval);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	ಪ್ರತಿ (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    ಯದಿ (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    ಅನ್ಯಥಾ
		l = 1;
	    cfstr = CFStringCreateWithBytes(ರಿಕ್ತ, ptr + in, l, from, 0);
	    ಯದಿ (cfstr == ರಿಕ್ತ)
	    {
		*d++ = '?';
		out++;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (!CFStringGetCString(cfstr, (ವರ್ಣ *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		ಅನ್ಯಥಾ
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
	ಯದಿ (lenp != ರಿಕ್ತ)
	    *lenp = out;
	ಪ್ರತ್ಯಾಗಮ retval;
    }
    CFRelease(cfstr);
    ಯದಿ (lenp != ರಿಕ್ತ)
	*lenp = STRLEN(retval);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
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
    cfstr = CFStringCreateWithBytes(ರಿಕ್ತ, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    ಯದಿ (cfstr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    ಯದಿ (r.length != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_size - *sizep, &len))
    {
	CFRelease(cfstr);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*to,
    ಪೂರ್ಣಾಙ್ಕ		*tolenp,
    ಪೂರ್ಣಾಙ್ಕ		maxtolen,
    char_u	*rest,
    ಪೂರ್ಣಾಙ್ಕ		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(ರಿಕ್ತ, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    ಯಾವತ್ (cfstr == ರಿಕ್ತ && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(ರಿಕ್ತ, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    ಯದಿ (cfstr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    ಯದಿ (r.length != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
    {
	CFRelease(cfstr);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Initializes text converters
 */
    ಶೂನ್ಯ
mac_conv_init(ಶೂನ್ಯ)
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

    ಯದಿ (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = ರಿಕ್ತ;

    ಯದಿ (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	ಯದಿ (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    gUTF16ToUTF8Converter = ರಿಕ್ತ;
    }
}

/*
 * Destroys text converters
 */
    ಶೂನ್ಯ
mac_conv_cleanup(ಶೂನ್ಯ)
{
    ಯದಿ (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = ರಿಕ್ತ;
    }

    ಯದಿ (gPathConverter)
    {
	TECDisposeConverter(gPathConverter);
	gPathConverter = ರಿಕ್ತ;
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
    char_u      *result = ರಿಕ್ತ;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_to_utf8(from, fromLen, &utf8_len);

    ಯದಿ (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	ಯದಿ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	ಯದಿ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    result = utf8_str;
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ len = utf8_len;
	    result = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, ರಿಕ್ತ, ರಿಕ್ತ);

	ಯದಿ (actualLen)
	    *actualLen = utf8_len;
    }
    ಅನ್ಯಥಾ ಯದಿ (actualLen)
	*actualLen = 0;

    ಪ್ರತ್ಯಾಗಮ result;
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
    UniChar     *result = ರಿಕ್ತ;
    Boolean     should_free_utf8 = FALSE;

    ಕುರು
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	ಯದಿ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    ಭಙ್ಗ;

	ಯದಿ (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (ಪೂರ್ಣಾಙ್ಕ *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	ಯದಿ (utf8_str == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	convert_setup(&conv, ರಿಕ್ತ, ರಿಕ್ತ);

	result = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	ಯದಿ (should_free_utf8)
	    vim_free(utf8_str);
	ಪ್ರತ್ಯಾಗಮ result;
    }
    ಯಾವತ್ (0);

    ಯದಿ (actualLen)
	*actualLen = 0;

    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    ಶೂನ್ಯ *
mac_enc_to_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	result = ರಿಕ್ತ;

    utf16_str = mac_enc_to_utf16(from, fromLen, &utf16_len);
    ಯದಿ (utf16_str)
    {
	result = CFStringCreateWithCharacters(ರಿಕ್ತ, utf16_str, utf16_len/ಪರಿಮಾಣ(UniChar));
	vim_free(utf16_str);
    }

    ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)result;
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
    char_u  *result = ರಿಕ್ತ;
    size_t  actualLen = 0;

    ಯದಿ (gPathConverter)
    {
	result = alloc(decompLen);
	ಯದಿ (result)
	{
	    ಯದಿ (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, result,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(result);
	}
    }

    ಯದಿ (precompLen)
	*precompLen = actualLen;

    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    ಸ್ಥಿರ char_u *
mac_utf16_to_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_ಎಣಿಕೆ		utf8_len;
    Byte_ಎಣಿಕೆ		ಒಳಹರಿವು_Read;
    char_u		*result;

    ಯದಿ (gUTF16ToUTF8Converter)
    {
	result = alloc(fromLen * 6 + 1);
	ಯದಿ (result && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &ಒಳಹರಿವು_Read, result,
		    (fromLen*6+1)*ಪರಿಮಾಣ(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, result, (fromLen*6+1)*ಪರಿಮಾಣ(char_u), &ಒಳಹರಿವು_Read);
	    utf8_len += ಒಳಹರಿವು_Read;
	}
	ಅನ್ಯಥಾ
	    VIM_CLEAR(result);
    }
    ಅನ್ಯಥಾ
    {
	result = ರಿಕ್ತ;
    }

    ಯದಿ (actualLen)
	*actualLen = result ? utf8_len : 0;

    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    ಸ್ಥಿರ UniChar *
mac_utf8_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *result = ರಿಕ್ತ;

    utf8_str = CFStringCreateWithBytes(ರಿಕ್ತ, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    ಯದಿ (utf8_str == ರಿಕ್ತ)
    {
	ಯದಿ (actualLen)
	    *actualLen = 0;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    convertRange = CFRangeMake(0, CFStringGetLength(utf8_str));
    result = ALLOC_MULT(UniChar, convertRange.length);

    CFStringGetCharacters(utf8_str, convertRange, result);

    CFRelease(utf8_str);

    ಯದಿ (actualLen)
	*actualLen = convertRange.length * ಪರಿಮಾಣ(UniChar);

    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    ಶೂನ್ಯ
mac_lang_init(ಶೂನ್ಯ)
{
    ಯದಿ (mch_getenv((char_u *)"LANG") != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ವರ್ಣ	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    ಯದಿ (LocaleRefGetPartString(ರಿಕ್ತ,
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		ಪರಿಮಾಣ(buf) - 10, buf) == noErr && *buf)
    {
	ಯದಿ (strcasestr(buf, "utf-8") == ರಿಕ್ತ)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#ಯದಿ defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
