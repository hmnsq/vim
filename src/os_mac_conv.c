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

#അന്തര്ഭാവ "vim.h"

#അന്തര്ഭാവ <CoreServices/CoreServices.h>


#എങ്കിൽ defined(MACOS_CONVERT)

സ്ഥിതം char_u	    *mac_utf16_to_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
സ്ഥിതം UniChar	    *mac_utf8_to_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
സ്ഥിതം TECObjectRef gPathConverter;
// Converter used by mac_utf16_to_utf8
സ്ഥിതം TECObjectRef gUTF16ToUTF8Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    പൂർണ്ണസംഖ്യ			len,
    പൂർണ്ണസംഖ്യ			*lenp,
    പൂർണ്ണസംഖ്യ			fail_on_error,
    പൂർണ്ണസംഖ്യ			from_enc,
    പൂർണ്ണസംഖ്യ			to_enc,
    പൂർണ്ണസംഖ്യ			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    പൂർണ്ണസംഖ്യ			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    തിരഞ്ഞെടുപ്പ് (from_enc)
    {
	അവസ്ഥ 'l':   from = kCFStringEncodingISOLatin1; നിർത്തുക;
	അവസ്ഥ 'm':   from = kCFStringEncodingMacRoman; നിർത്തുക;
	അവസ്ഥ 'u':   from = kCFStringEncodingUTF8; നിർത്തുക;
	സ്ഥിരസ്ഥിതി:    തിരികെ ശൂന്യ;
    }
    തിരഞ്ഞെടുപ്പ് (to_enc)
    {
	അവസ്ഥ 'l':   to = kCFStringEncodingISOLatin1; നിർത്തുക;
	അവസ്ഥ 'm':   to = kCFStringEncodingMacRoman; നിർത്തുക;
	അവസ്ഥ 'u':   to = kCFStringEncodingUTF8; നിർത്തുക;
	സ്ഥിരസ്ഥിതി:    തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (unconvlenp != ശൂന്യ)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(ശൂന്യ, ptr, len, from, 0);

    എങ്കിൽ (cfstr == ശൂന്യ)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    ആകുമ്പോൾ (cfstr == ശൂന്യ && unconvlenp != ശൂന്യ && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(ശൂന്യ, ptr, len, from, 0);
    }
    എങ്കിൽ (cfstr == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (to == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    അല്ലെങ്കിൽ
	buflen = len + 1;
    retval = alloc(buflen);
    എങ്കിൽ (retval == ശൂന്യ)
    {
	CFRelease(cfstr);
	തിരികെ ശൂന്യ;
    }

#എങ്കിൽ 0
    CFRange convertRange = CFRangeMake(0, CFStringGetLength(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, ശൂന്യ, FALSE, ശൂന്യ, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : ശൂന്യ;
    എങ്കിൽ (retval == ശൂന്യ)
    {
	CFRelease(cfstr);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (lenp)
	*lenp = buflen / വലുപ്പം(char_u);

    എങ്കിൽ (!CFStringGetBytes(cfstr, convertRange, to, ശൂന്യ, FALSE, retval, buflen, ശൂന്യ))
#endif
    എങ്കിൽ (!CFStringGetCString(cfstr, (അക്ഷരം *)retval, buflen, to))
    {
	CFRelease(cfstr);
	എങ്കിൽ (fail_on_error)
	{
	    vim_free(retval);
	    തിരികെ ശൂന്യ;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	വേണ്ടി (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    എങ്കിൽ (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    അല്ലെങ്കിൽ
		l = 1;
	    cfstr = CFStringCreateWithBytes(ശൂന്യ, ptr + in, l, from, 0);
	    എങ്കിൽ (cfstr == ശൂന്യ)
	    {
		*d++ = '?';
		out++;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (!CFStringGetCString(cfstr, (അക്ഷരം *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		അല്ലെങ്കിൽ
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
	എങ്കിൽ (lenp != ശൂന്യ)
	    *lenp = out;
	തിരികെ retval;
    }
    CFRelease(cfstr);
    എങ്കിൽ (lenp != ശൂന്യ)
	*lenp = STRLEN(retval);

    തിരികെ retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
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
    cfstr = CFStringCreateWithBytes(ശൂന്യ, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    എങ്കിൽ (cfstr == ശൂന്യ)
	തിരികെ FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    എങ്കിൽ (r.length != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_size - *sizep, &len))
    {
	CFRelease(cfstr);
	തിരികെ FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    തിരികെ OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*to,
    പൂർണ്ണസംഖ്യ		*tolenp,
    പൂർണ്ണസംഖ്യ		maxtolen,
    char_u	*rest,
    പൂർണ്ണസംഖ്യ		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(ശൂന്യ, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    ആകുമ്പോൾ (cfstr == ശൂന്യ && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(ശൂന്യ, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    എങ്കിൽ (cfstr == ശൂന്യ)
	തിരികെ FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    എങ്കിൽ (r.length != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
    {
	CFRelease(cfstr);
	തിരികെ FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    തിരികെ OK;
}

/*
 * Initializes text converters
 */
    ശൂന്യം
mac_conv_init(ശൂന്യം)
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

    എങ്കിൽ (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = ശൂന്യ;

    എങ്കിൽ (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	എങ്കിൽ (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    gUTF16ToUTF8Converter = ശൂന്യ;
    }
}

/*
 * Destroys text converters
 */
    ശൂന്യം
mac_conv_cleanup(ശൂന്യം)
{
    എങ്കിൽ (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = ശൂന്യ;
    }

    എങ്കിൽ (gPathConverter)
    {
	TECDisposeConverter(gPathConverter);
	gPathConverter = ശൂന്യ;
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
    char_u      *result = ശൂന്യ;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_to_utf8(from, fromLen, &utf8_len);

    എങ്കിൽ (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	എങ്കിൽ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	എങ്കിൽ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    result = utf8_str;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ len = utf8_len;
	    result = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, ശൂന്യ, ശൂന്യ);

	എങ്കിൽ (actualLen)
	    *actualLen = utf8_len;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (actualLen)
	*actualLen = 0;

    തിരികെ result;
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
    UniChar     *result = ശൂന്യ;
    Boolean     should_free_utf8 = FALSE;

    ചെയ്യുക
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	എങ്കിൽ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    നിർത്തുക;

	എങ്കിൽ (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (പൂർണ്ണസംഖ്യ *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	എങ്കിൽ (utf8_str == ശൂന്യ)
	    നിർത്തുക;

	convert_setup(&conv, ശൂന്യ, ശൂന്യ);

	result = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	എങ്കിൽ (should_free_utf8)
	    vim_free(utf8_str);
	തിരികെ result;
    }
    ആകുമ്പോൾ (0);

    എങ്കിൽ (actualLen)
	*actualLen = 0;

    തിരികെ result;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    ശൂന്യം *
mac_enc_to_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	result = ശൂന്യ;

    utf16_str = mac_enc_to_utf16(from, fromLen, &utf16_len);
    എങ്കിൽ (utf16_str)
    {
	result = CFStringCreateWithCharacters(ശൂന്യ, utf16_str, utf16_len/വലുപ്പം(UniChar));
	vim_free(utf16_str);
    }

    തിരികെ (ശൂന്യം *)result;
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
    char_u  *result = ശൂന്യ;
    size_t  actualLen = 0;

    എങ്കിൽ (gPathConverter)
    {
	result = alloc(decompLen);
	എങ്കിൽ (result)
	{
	    എങ്കിൽ (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, result,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(result);
	}
    }

    എങ്കിൽ (precompLen)
	*precompLen = actualLen;

    തിരികെ result;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    സ്ഥിതം char_u *
mac_utf16_to_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_എണ്ണം		utf8_len;
    Byte_എണ്ണം		നിവേശം_Read;
    char_u		*result;

    എങ്കിൽ (gUTF16ToUTF8Converter)
    {
	result = alloc(fromLen * 6 + 1);
	എങ്കിൽ (result && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &നിവേശം_Read, result,
		    (fromLen*6+1)*വലുപ്പം(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, result, (fromLen*6+1)*വലുപ്പം(char_u), &നിവേശം_Read);
	    utf8_len += നിവേശം_Read;
	}
	അല്ലെങ്കിൽ
	    VIM_CLEAR(result);
    }
    അല്ലെങ്കിൽ
    {
	result = ശൂന്യ;
    }

    എങ്കിൽ (actualLen)
	*actualLen = result ? utf8_len : 0;

    തിരികെ result;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    സ്ഥിതം UniChar *
mac_utf8_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *result = ശൂന്യ;

    utf8_str = CFStringCreateWithBytes(ശൂന്യ, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    എങ്കിൽ (utf8_str == ശൂന്യ)
    {
	എങ്കിൽ (actualLen)
	    *actualLen = 0;
	തിരികെ ശൂന്യ;
    }

    convertRange = CFRangeMake(0, CFStringGetLength(utf8_str));
    result = ALLOC_MULT(UniChar, convertRange.length);

    CFStringGetCharacters(utf8_str, convertRange, result);

    CFRelease(utf8_str);

    എങ്കിൽ (actualLen)
	*actualLen = convertRange.length * വലുപ്പം(UniChar);

    തിരികെ result;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    ശൂന്യം
mac_lang_init(ശൂന്യം)
{
    എങ്കിൽ (mch_getenv((char_u *)"LANG") != ശൂന്യ)
	തിരികെ;

    അക്ഷരം	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    എങ്കിൽ (LocaleRefGetPartString(ശൂന്യ,
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		വലുപ്പം(buf) - 10, buf) == noErr && *buf)
    {
	എങ്കിൽ (strcasestr(buf, "utf-8") == ശൂന്യ)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#എങ്കിൽ defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
