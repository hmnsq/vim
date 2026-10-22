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

#અન્તર્ભાવ "vim.h"

#અન્તર્ભાવ <CoreServices/CoreServices.h>


#જો defined(MACOS_CONVERT)

સ્થિર char_u	    *mac_utf16_to_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
સ્થિર UniChar	    *mac_utf8_to_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
સ્થિર TECObjectRef gPathConverter;
// Converter used by mac_utf16_to_utf8
સ્થિર TECObjectRef gUTF16ToUTF8Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    પૂર્ણાંક			len,
    પૂર્ણાંક			*lenp,
    પૂર્ણાંક			fail_on_error,
    પૂર્ણાંક			from_enc,
    પૂર્ણાંક			to_enc,
    પૂર્ણાંક			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    પૂર્ણાંક			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    પસંદગી (from_enc)
    {
	સ્થિતિ 'l':   from = kCFStringEncodingISOLatin1; તોડો;
	સ્થિતિ 'm':   from = kCFStringEncodingMacRoman; તોડો;
	સ્થિતિ 'u':   from = kCFStringEncodingUTF8; તોડો;
	મૂળભૂત:    પાછા ખાલી;
    }
    પસંદગી (to_enc)
    {
	સ્થિતિ 'l':   to = kCFStringEncodingISOLatin1; તોડો;
	સ્થિતિ 'm':   to = kCFStringEncodingMacRoman; તોડો;
	સ્થિતિ 'u':   to = kCFStringEncodingUTF8; તોડો;
	મૂળભૂત:    પાછા ખાલી;
    }

    જો (unconvlenp != ખાલી)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(ખાલી, ptr, len, from, 0);

    જો (cfstr == ખાલી)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    જ્યારે (cfstr == ખાલી && unconvlenp != ખાલી && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(ખાલી, ptr, len, from, 0);
    }
    જો (cfstr == ખાલી)
	પાછા ખાલી;

    જો (to == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    નહીંતર
	buflen = len + 1;
    retval = alloc(buflen);
    જો (retval == ખાલી)
    {
	CFRelease(cfstr);
	પાછા ખાલી;
    }

#જો 0
    CFRange convertRange = CFRangeMake(0, CFStringGetLength(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, ખાલી, FALSE, ખાલી, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : ખાલી;
    જો (retval == ખાલી)
    {
	CFRelease(cfstr);
	પાછા ખાલી;
    }

    જો (lenp)
	*lenp = buflen / કદ(char_u);

    જો (!CFStringGetBytes(cfstr, convertRange, to, ખાલી, FALSE, retval, buflen, ખાલી))
#endif
    જો (!CFStringGetCString(cfstr, (અક્ષર *)retval, buflen, to))
    {
	CFRelease(cfstr);
	જો (fail_on_error)
	{
	    vim_free(retval);
	    પાછા ખાલી;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	માટે (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    જો (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    નહીંતર
		l = 1;
	    cfstr = CFStringCreateWithBytes(ખાલી, ptr + in, l, from, 0);
	    જો (cfstr == ખાલી)
	    {
		*d++ = '?';
		out++;
	    }
	    નહીંતર
	    {
		જો (!CFStringGetCString(cfstr, (અક્ષર *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		નહીંતર
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
	જો (lenp != ખાલી)
	    *lenp = out;
	પાછા retval;
    }
    CFRelease(cfstr);
    જો (lenp != ખાલી)
	*lenp = STRLEN(retval);

    પાછા retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
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
    cfstr = CFStringCreateWithBytes(ખાલી, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    જો (cfstr == ખાલી)
	પાછા FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    જો (r.length != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_size - *sizep, &len))
    {
	CFRelease(cfstr);
	પાછા FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    પાછા OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*to,
    પૂર્ણાંક		*tolenp,
    પૂર્ણાંક		maxtolen,
    char_u	*rest,
    પૂર્ણાંક		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(ખાલી, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    જ્યારે (cfstr == ખાલી && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(ખાલી, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    જો (cfstr == ખાલી)
	પાછા FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    જો (r.length != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
    {
	CFRelease(cfstr);
	પાછા FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    પાછા OK;
}

/*
 * Initializes text converters
 */
    શૂન્ય
mac_conv_init(શૂન્ય)
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

    જો (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = ખાલી;

    જો (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	જો (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    gUTF16ToUTF8Converter = ખાલી;
    }
}

/*
 * Destroys text converters
 */
    શૂન્ય
mac_conv_cleanup(શૂન્ય)
{
    જો (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = ખાલી;
    }

    જો (gPathConverter)
    {
	TECDisposeConverter(gPathConverter);
	gPathConverter = ખાલી;
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
    char_u      *result = ખાલી;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_to_utf8(from, fromLen, &utf8_len);

    જો (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	જો ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	જો (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    result = utf8_str;
	}
	નહીંતર
	{
	    પૂર્ણાંક len = utf8_len;
	    result = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, ખાલી, ખાલી);

	જો (actualLen)
	    *actualLen = utf8_len;
    }
    નહીંતર જો (actualLen)
	*actualLen = 0;

    પાછા result;
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
    UniChar     *result = ખાલી;
    Boolean     should_free_utf8 = FALSE;

    કરો
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	જો ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    તોડો;

	જો (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (પૂર્ણાંક *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	નહીંતર
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	જો (utf8_str == ખાલી)
	    તોડો;

	convert_setup(&conv, ખાલી, ખાલી);

	result = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	જો (should_free_utf8)
	    vim_free(utf8_str);
	પાછા result;
    }
    જ્યારે (0);

    જો (actualLen)
	*actualLen = 0;

    પાછા result;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    શૂન્ય *
mac_enc_to_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	result = ખાલી;

    utf16_str = mac_enc_to_utf16(from, fromLen, &utf16_len);
    જો (utf16_str)
    {
	result = CFStringCreateWithCharacters(ખાલી, utf16_str, utf16_len/કદ(UniChar));
	vim_free(utf16_str);
    }

    પાછા (શૂન્ય *)result;
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
    char_u  *result = ખાલી;
    size_t  actualLen = 0;

    જો (gPathConverter)
    {
	result = alloc(decompLen);
	જો (result)
	{
	    જો (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, result,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(result);
	}
    }

    જો (precompLen)
	*precompLen = actualLen;

    પાછા result;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    સ્થિર char_u *
mac_utf16_to_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_ગણતરી		utf8_len;
    Byte_ગણતરી		આગત_Read;
    char_u		*result;

    જો (gUTF16ToUTF8Converter)
    {
	result = alloc(fromLen * 6 + 1);
	જો (result && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &આગત_Read, result,
		    (fromLen*6+1)*કદ(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, result, (fromLen*6+1)*કદ(char_u), &આગત_Read);
	    utf8_len += આગત_Read;
	}
	નહીંતર
	    VIM_CLEAR(result);
    }
    નહીંતર
    {
	result = ખાલી;
    }

    જો (actualLen)
	*actualLen = result ? utf8_len : 0;

    પાછા result;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    સ્થિર UniChar *
mac_utf8_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *result = ખાલી;

    utf8_str = CFStringCreateWithBytes(ખાલી, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    જો (utf8_str == ખાલી)
    {
	જો (actualLen)
	    *actualLen = 0;
	પાછા ખાલી;
    }

    convertRange = CFRangeMake(0, CFStringGetLength(utf8_str));
    result = ALLOC_MULT(UniChar, convertRange.length);

    CFStringGetCharacters(utf8_str, convertRange, result);

    CFRelease(utf8_str);

    જો (actualLen)
	*actualLen = convertRange.length * કદ(UniChar);

    પાછા result;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    શૂન્ય
mac_lang_init(શૂન્ય)
{
    જો (mch_getenv((char_u *)"LANG") != ખાલી)
	પાછા;

    અક્ષર	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    જો (LocaleRefGetPartString(ખાલી,
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		કદ(buf) - 10, buf) == noErr && *buf)
    {
	જો (strcasestr(buf, "utf-8") == ખાલી)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#જો defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
