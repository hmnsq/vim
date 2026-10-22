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

#include "vim.h"

#include <CoreServices/CoreServices.h>


#만약 defined(MACOS_CONVERT)

static char_u	    *mac_utf16_to_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
static UniChar	    *mac_utf8_to_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
static TECObjectRef gPathConverter;
// Converter used by mac_utf16_to_utf8
static TECObjectRef gUTF16ToUTF8Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    정수			len,
    정수			*lenp,
    정수			fail_on_error,
    정수			from_enc,
    정수			to_enc,
    정수			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    정수			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    switch (from_enc)
    {
	case 'l':   from = kCFStringEncodingISOLatin1; break;
	case 'm':   from = kCFStringEncodingMacRoman; break;
	case 'u':   from = kCFStringEncodingUTF8; break;
	default:    반환 NULL;
    }
    switch (to_enc)
    {
	case 'l':   to = kCFStringEncodingISOLatin1; break;
	case 'm':   to = kCFStringEncodingMacRoman; break;
	case 'u':   to = kCFStringEncodingUTF8; break;
	default:    반환 NULL;
    }

    만약 (unconvlenp != NULL)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(NULL, ptr, len, from, 0);

    만약 (cfstr == NULL)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    동안 (cfstr == NULL && unconvlenp != NULL && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(NULL, ptr, len, from, 0);
    }
    만약 (cfstr == NULL)
	반환 NULL;

    만약 (to == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    아니면
	buflen = len + 1;
    retval = alloc(buflen);
    만약 (retval == NULL)
    {
	CFRelease(cfstr);
	반환 NULL;
    }

#만약 0
    CFRange convertRange = CFRangeMake(0, CFStringGetLength(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, NULL, FALSE, NULL, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : NULL;
    만약 (retval == NULL)
    {
	CFRelease(cfstr);
	반환 NULL;
    }

    만약 (lenp)
	*lenp = buflen / sizeof(char_u);

    만약 (!CFStringGetBytes(cfstr, convertRange, to, NULL, FALSE, retval, buflen, NULL))
#endif
    만약 (!CFStringGetCString(cfstr, (char *)retval, buflen, to))
    {
	CFRelease(cfstr);
	만약 (fail_on_error)
	{
	    vim_free(retval);
	    반환 NULL;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	위해 (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    만약 (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    아니면
		l = 1;
	    cfstr = CFStringCreateWithBytes(NULL, ptr + in, l, from, 0);
	    만약 (cfstr == NULL)
	    {
		*d++ = '?';
		out++;
	    }
	    아니면
	    {
		만약 (!CFStringGetCString(cfstr, (char *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		아니면
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
	만약 (lenp != NULL)
	    *lenp = out;
	반환 retval;
    }
    CFRelease(cfstr);
    만약 (lenp != NULL)
	*lenp = STRLEN(retval);

    반환 retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    정수
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
    cfstr = CFStringCreateWithBytes(NULL, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    만약 (cfstr == NULL)
	반환 FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    만약 (r.length != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_size - *sizep, &len))
    {
	CFRelease(cfstr);
	반환 FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    반환 OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    정수
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*to,
    정수		*tolenp,
    정수		maxtolen,
    char_u	*rest,
    정수		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(NULL, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    동안 (cfstr == NULL && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(NULL, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    만약 (cfstr == NULL)
	반환 FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    만약 (r.length != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
    {
	CFRelease(cfstr);
	반환 FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    반환 OK;
}

/*
 * Initializes text converters
 */
    void
mac_conv_init(void)
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

    만약 (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = NULL;

    만약 (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	만약 (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    gUTF16ToUTF8Converter = NULL;
    }
}

/*
 * Destroys text converters
 */
    void
mac_conv_cleanup(void)
{
    만약 (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = NULL;
    }

    만약 (gPathConverter)
    {
	TECDisposeConverter(gPathConverter);
	gPathConverter = NULL;
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
    char_u      *result = NULL;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_to_utf8(from, fromLen, &utf8_len);

    만약 (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	만약 ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	만약 (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    result = utf8_str;
	}
	아니면
	{
	    정수 len = utf8_len;
	    result = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, NULL, NULL);

	만약 (actualLen)
	    *actualLen = utf8_len;
    }
    아니면 만약 (actualLen)
	*actualLen = 0;

    반환 result;
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
    UniChar     *result = NULL;
    Boolean     should_free_utf8 = FALSE;

    do
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	만약 ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    break;

	만약 (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (정수 *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	아니면
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	만약 (utf8_str == NULL)
	    break;

	convert_setup(&conv, NULL, NULL);

	result = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	만약 (should_free_utf8)
	    vim_free(utf8_str);
	반환 result;
    }
    동안 (0);

    만약 (actualLen)
	*actualLen = 0;

    반환 result;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    void *
mac_enc_to_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	result = NULL;

    utf16_str = mac_enc_to_utf16(from, fromLen, &utf16_len);
    만약 (utf16_str)
    {
	result = CFStringCreateWithCharacters(NULL, utf16_str, utf16_len/sizeof(UniChar));
	vim_free(utf16_str);
    }

    반환 (void *)result;
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
    char_u  *result = NULL;
    size_t  actualLen = 0;

    만약 (gPathConverter)
    {
	result = alloc(decompLen);
	만약 (result)
	{
	    만약 (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, result,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(result);
	}
    }

    만약 (precompLen)
	*precompLen = actualLen;

    반환 result;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    static char_u *
mac_utf16_to_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    ByteCount		utf8_len;
    ByteCount		inputRead;
    char_u		*result;

    만약 (gUTF16ToUTF8Converter)
    {
	result = alloc(fromLen * 6 + 1);
	만약 (result && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &inputRead, result,
		    (fromLen*6+1)*sizeof(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, result, (fromLen*6+1)*sizeof(char_u), &inputRead);
	    utf8_len += inputRead;
	}
	아니면
	    VIM_CLEAR(result);
    }
    아니면
    {
	result = NULL;
    }

    만약 (actualLen)
	*actualLen = result ? utf8_len : 0;

    반환 result;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    static UniChar *
mac_utf8_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *result = NULL;

    utf8_str = CFStringCreateWithBytes(NULL, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    만약 (utf8_str == NULL)
    {
	만약 (actualLen)
	    *actualLen = 0;
	반환 NULL;
    }

    convertRange = CFRangeMake(0, CFStringGetLength(utf8_str));
    result = ALLOC_MULT(UniChar, convertRange.length);

    CFStringGetCharacters(utf8_str, convertRange, result);

    CFRelease(utf8_str);

    만약 (actualLen)
	*actualLen = convertRange.length * sizeof(UniChar);

    반환 result;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    void
mac_lang_init(void)
{
    만약 (mch_getenv((char_u *)"LANG") != NULL)
	반환;

    char	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    만약 (LocaleRefGetPartString(NULL,
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		sizeof(buf) - 10, buf) == noErr && *buf)
    {
	만약 (strcasestr(buf, "utf-8") == NULL)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#만약 defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
