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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଅନ୍ତର୍ଭାଵ <CoreServices/CoreServices.h>


#ଯଦି defined(MACOS_CONVERT)

ସ୍ଥିର char_u	    *mac_utf16_to_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
ସ୍ଥିର UniChar	    *mac_utf8_to_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
ସ୍ଥିର TECObjectRef gPathConverter;
// Converter used by mac_utf16_to_utf8
ସ୍ଥିର TECObjectRef gUTF16ToUTF8Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*lenp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			fail_on_error,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			from_enc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			to_enc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    ବାଛ (from_enc)
    {
	ସ୍ଥିତି 'l':   from = kCFStringEncodingISOLatin1; ଭାଙ୍ଗ;
	ସ୍ଥିତି 'm':   from = kCFStringEncodingMacRoman; ଭାଙ୍ଗ;
	ସ୍ଥିତି 'u':   from = kCFStringEncodingUTF8; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:    ଫେରନ୍ତୁ ଖାଲି;
    }
    ବାଛ (to_enc)
    {
	ସ୍ଥିତି 'l':   to = kCFStringEncodingISOLatin1; ଭାଙ୍ଗ;
	ସ୍ଥିତି 'm':   to = kCFStringEncodingMacRoman; ଭାଙ୍ଗ;
	ସ୍ଥିତି 'u':   to = kCFStringEncodingUTF8; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:    ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (unconvlenp != ଖାଲି)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(ଖାଲି, ptr, len, from, 0);

    ଯଦି (cfstr == ଖାଲି)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    ଯେତେବେଳେ (cfstr == ଖାଲି && unconvlenp != ଖାଲି && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(ଖାଲି, ptr, len, from, 0);
    }
    ଯଦି (cfstr == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (to == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    ଅନ୍ୟଥା
	buflen = len + 1;
    retval = alloc(buflen);
    ଯଦି (retval == ଖାଲି)
    {
	CFRelease(cfstr);
	ଫେରନ୍ତୁ ଖାଲି;
    }

#ଯଦି 0
    CFRange convertRange = CFRangeMake(0, CFStringGetLength(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, ଖାଲି, FALSE, ଖାଲି, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : ଖାଲି;
    ଯଦି (retval == ଖାଲି)
    {
	CFRelease(cfstr);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (lenp)
	*lenp = buflen / ଆକାର(char_u);

    ଯଦି (!CFStringGetBytes(cfstr, convertRange, to, ଖାଲି, FALSE, retval, buflen, ଖାଲି))
#endif
    ଯଦି (!CFStringGetCString(cfstr, (ଅକ୍ଷର *)retval, buflen, to))
    {
	CFRelease(cfstr);
	ଯଦି (fail_on_error)
	{
	    vim_free(retval);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	ପାଇଁ (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    ଯଦି (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    ଅନ୍ୟଥା
		l = 1;
	    cfstr = CFStringCreateWithBytes(ଖାଲି, ptr + in, l, from, 0);
	    ଯଦି (cfstr == ଖାଲି)
	    {
		*d++ = '?';
		out++;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (!CFStringGetCString(cfstr, (ଅକ୍ଷର *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		ଅନ୍ୟଥା
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
	ଯଦି (lenp != ଖାଲି)
	    *lenp = out;
	ଫେରନ୍ତୁ retval;
    }
    CFRelease(cfstr);
    ଯଦି (lenp != ଖାଲି)
	*lenp = STRLEN(retval);

    ଫେରନ୍ତୁ retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    cfstr = CFStringCreateWithBytes(ଖାଲି, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    ଯଦି (cfstr == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    ଯଦି (r.length != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_size - *sizep, &len))
    {
	CFRelease(cfstr);
	ଫେରନ୍ତୁ FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    ଫେରନ୍ତୁ OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*to,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*tolenp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxtolen,
    char_u	*rest,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(ଖାଲି, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    ଯେତେବେଳେ (cfstr == ଖାଲି && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(ଖାଲି, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    ଯଦି (cfstr == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    ଯଦି (r.length != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
    {
	CFRelease(cfstr);
	ଫେରନ୍ତୁ FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    ଫେରନ୍ତୁ OK;
}

/*
 * Initializes text converters
 */
    ଶୂନ୍ୟ
mac_conv_init(ଶୂନ୍ୟ)
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

    ଯଦି (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = ଖାଲି;

    ଯଦି (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	ଯଦି (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    gUTF16ToUTF8Converter = ଖାଲି;
    }
}

/*
 * Destroys text converters
 */
    ଶୂନ୍ୟ
mac_conv_cleanup(ଶୂନ୍ୟ)
{
    ଯଦି (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = ଖାଲି;
    }

    ଯଦି (gPathConverter)
    {
	TECDisposeConverter(gPathConverter);
	gPathConverter = ଖାଲି;
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
    char_u      *result = ଖାଲି;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_to_utf8(from, fromLen, &utf8_len);

    ଯଦି (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	ଯଦି ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	ଯଦି (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    result = utf8_str;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = utf8_len;
	    result = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, ଖାଲି, ଖାଲି);

	ଯଦି (actualLen)
	    *actualLen = utf8_len;
    }
    ଅନ୍ୟଥା ଯଦି (actualLen)
	*actualLen = 0;

    ଫେରନ୍ତୁ result;
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
    UniChar     *result = ଖାଲି;
    Boolean     should_free_utf8 = FALSE;

    କର
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	ଯଦି ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    ଭାଙ୍ଗ;

	ଯଦି (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	ଯଦି (utf8_str == ଖାଲି)
	    ଭାଙ୍ଗ;

	convert_setup(&conv, ଖାଲି, ଖାଲି);

	result = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	ଯଦି (should_free_utf8)
	    vim_free(utf8_str);
	ଫେରନ୍ତୁ result;
    }
    ଯେତେବେଳେ (0);

    ଯଦି (actualLen)
	*actualLen = 0;

    ଫେରନ୍ତୁ result;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    ଶୂନ୍ୟ *
mac_enc_to_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	result = ଖାଲି;

    utf16_str = mac_enc_to_utf16(from, fromLen, &utf16_len);
    ଯଦି (utf16_str)
    {
	result = CFStringCreateWithCharacters(ଖାଲି, utf16_str, utf16_len/ଆକାର(UniChar));
	vim_free(utf16_str);
    }

    ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)result;
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
    char_u  *result = ଖାଲି;
    size_t  actualLen = 0;

    ଯଦି (gPathConverter)
    {
	result = alloc(decompLen);
	ଯଦି (result)
	{
	    ଯଦି (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, result,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(result);
	}
    }

    ଯଦି (precompLen)
	*precompLen = actualLen;

    ଫେରନ୍ତୁ result;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    ସ୍ଥିର char_u *
mac_utf16_to_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_ଗଣନା		utf8_len;
    Byte_ଗଣନା		ନିବେଶ_Read;
    char_u		*result;

    ଯଦି (gUTF16ToUTF8Converter)
    {
	result = alloc(fromLen * 6 + 1);
	ଯଦି (result && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &ନିବେଶ_Read, result,
		    (fromLen*6+1)*ଆକାର(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, result, (fromLen*6+1)*ଆକାର(char_u), &ନିବେଶ_Read);
	    utf8_len += ନିବେଶ_Read;
	}
	ଅନ୍ୟଥା
	    VIM_CLEAR(result);
    }
    ଅନ୍ୟଥା
    {
	result = ଖାଲି;
    }

    ଯଦି (actualLen)
	*actualLen = result ? utf8_len : 0;

    ଫେରନ୍ତୁ result;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    ସ୍ଥିର UniChar *
mac_utf8_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *result = ଖାଲି;

    utf8_str = CFStringCreateWithBytes(ଖାଲି, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    ଯଦି (utf8_str == ଖାଲି)
    {
	ଯଦି (actualLen)
	    *actualLen = 0;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    convertRange = CFRangeMake(0, CFStringGetLength(utf8_str));
    result = ALLOC_MULT(UniChar, convertRange.length);

    CFStringGetCharacters(utf8_str, convertRange, result);

    CFRelease(utf8_str);

    ଯଦି (actualLen)
	*actualLen = convertRange.length * ଆକାର(UniChar);

    ଫେରନ୍ତୁ result;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    ଶୂନ୍ୟ
mac_lang_init(ଶୂନ୍ୟ)
{
    ଯଦି (mch_getenv((char_u *)"LANG") != ଖାଲି)
	ଫେରନ୍ତୁ;

    ଅକ୍ଷର	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    ଯଦି (LocaleRefGetPartString(ଖାଲି,
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		ଆକାର(buf) - 10, buf) == noErr && *buf)
    {
	ଯଦି (strcasestr(buf, "utf-8") == ଖାଲି)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#ଯଦି defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
