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


#ᮚᮦᮔ᮪ defined(MACOS_CONVERT)

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
    puur_naa_nka			len,
    puur_naa_nka			*lenp,
    puur_naa_nka			fail_on_error,
    puur_naa_nka			from_enc,
    puur_naa_nka			to_enc,
    puur_naa_nka			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    puur_naa_nka			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    switch (from_enc)
    {
	pak_sa 'l':   from = kCFStringEncodingISOLatin1; bha_nga;
	pak_sa 'm':   from = kCFStringEncodingMacRoman; bha_nga;
	pak_sa 'u':   from = kCFStringEncodingUTF8; bha_nga;
	default:    ᮝᮜᮤ NULL;
    }
    switch (to_enc)
    {
	pak_sa 'l':   to = kCFStringEncodingISOLatin1; bha_nga;
	pak_sa 'm':   to = kCFStringEncodingMacRoman; bha_nga;
	pak_sa 'u':   to = kCFStringEncodingUTF8; bha_nga;
	default:    ᮝᮜᮤ NULL;
    }

    ᮚᮦᮔ᮪ (unconvlenp != NULL)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(NULL, ptr, len, from, 0);

    ᮚᮦᮔ᮪ (cfstr == NULL)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    ᮞᮨᮓᮨᮜ (cfstr == NULL && unconvlenp != NULL && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(NULL, ptr, len, from, 0);
    }
    ᮚᮦᮔ᮪ (cfstr == NULL)
	ᮝᮜᮤ NULL;

    ᮚᮦᮔ᮪ (to == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    ᮜᮤᮚ
	buflen = len + 1;
    retval = alloc(buflen);
    ᮚᮦᮔ᮪ (retval == NULL)
    {
	CFRelease(cfstr);
	ᮝᮜᮤ NULL;
    }

#ᮚᮦᮔ᮪ 0
    CFRange convertRange = CFRangeMake(0, CFStringGetLength(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, NULL, FALSE, NULL, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : NULL;
    ᮚᮦᮔ᮪ (retval == NULL)
    {
	CFRelease(cfstr);
	ᮝᮜᮤ NULL;
    }

    ᮚᮦᮔ᮪ (lenp)
	*lenp = buflen / parimaa_na(char_u);

    ᮚᮦᮔ᮪ (!CFStringGetBytes(cfstr, convertRange, to, NULL, FALSE, retval, buflen, NULL))
#endif
    ᮚᮦᮔ᮪ (!CFStringGetCString(cfstr, (var_na *)retval, buflen, to))
    {
	CFRelease(cfstr);
	ᮚᮦᮔ᮪ (fail_on_error)
	{
	    vim_free(retval);
	    ᮝᮜᮤ NULL;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	for (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    ᮚᮦᮔ᮪ (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    ᮜᮤᮚ
		l = 1;
	    cfstr = CFStringCreateWithBytes(NULL, ptr + in, l, from, 0);
	    ᮚᮦᮔ᮪ (cfstr == NULL)
	    {
		*d++ = '?';
		out++;
	    }
	    ᮜᮤᮚ
	    {
		ᮚᮦᮔ᮪ (!CFStringGetCString(cfstr, (var_na *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		ᮜᮤᮚ
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
	ᮚᮦᮔ᮪ (lenp != NULL)
	    *lenp = out;
	ᮝᮜᮤ retval;
    }
    CFRelease(cfstr);
    ᮚᮦᮔ᮪ (lenp != NULL)
	*lenp = STRLEN(retval);

    ᮝᮜᮤ retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    puur_naa_nka
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
    ᮚᮦᮔ᮪ (cfstr == NULL)
	ᮝᮜᮤ FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    ᮚᮦᮔ᮪ (r.length != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_size - *sizep, &len))
    {
	CFRelease(cfstr);
	ᮝᮜᮤ FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    ᮝᮜᮤ OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    puur_naa_nka
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*to,
    puur_naa_nka		*tolenp,
    puur_naa_nka		maxtolen,
    char_u	*rest,
    puur_naa_nka		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(NULL, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    ᮞᮨᮓᮨᮜ (cfstr == NULL && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(NULL, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    ᮚᮦᮔ᮪ (cfstr == NULL)
	ᮝᮜᮤ FAIL;

    r.location = 0;
    r.length = CFStringGetLength(cfstr);
    ᮚᮦᮔ᮪ (r.length != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
    {
	CFRelease(cfstr);
	ᮝᮜᮤ FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    ᮝᮜᮤ OK;
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

    ᮚᮦᮔ᮪ (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = NULL;

    ᮚᮦᮔ᮪ (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	ᮚᮦᮔ᮪ (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
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
    ᮚᮦᮔ᮪ (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = NULL;
    }

    ᮚᮦᮔ᮪ (gPathConverter)
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

    ᮚᮦᮔ᮪ (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	ᮚᮦᮔ᮪ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	ᮚᮦᮔ᮪ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    result = utf8_str;
	}
	ᮜᮤᮚ
	{
	    puur_naa_nka len = utf8_len;
	    result = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, NULL, NULL);

	ᮚᮦᮔ᮪ (actualLen)
	    *actualLen = utf8_len;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (actualLen)
	*actualLen = 0;

    ᮝᮜᮤ result;
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
	ᮚᮦᮔ᮪ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    bha_nga;

	ᮚᮦᮔ᮪ (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (puur_naa_nka *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	ᮜᮤᮚ
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	ᮚᮦᮔ᮪ (utf8_str == NULL)
	    bha_nga;

	convert_setup(&conv, NULL, NULL);

	result = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	ᮚᮦᮔ᮪ (should_free_utf8)
	    vim_free(utf8_str);
	ᮝᮜᮤ result;
    }
    ᮞᮨᮓᮨᮜ (0);

    ᮚᮦᮔ᮪ (actualLen)
	*actualLen = 0;

    ᮝᮜᮤ result;
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
    ᮚᮦᮔ᮪ (utf16_str)
    {
	result = CFStringCreateWithCharacters(NULL, utf16_str, utf16_len/parimaa_na(UniChar));
	vim_free(utf16_str);
    }

    ᮝᮜᮤ (void *)result;
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

    ᮚᮦᮔ᮪ (gPathConverter)
    {
	result = alloc(decompLen);
	ᮚᮦᮔ᮪ (result)
	{
	    ᮚᮦᮔ᮪ (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, result,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(result);
	}
    }

    ᮚᮦᮔ᮪ (precompLen)
	*precompLen = actualLen;

    ᮝᮜᮤ result;
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

    ᮚᮦᮔ᮪ (gUTF16ToUTF8Converter)
    {
	result = alloc(fromLen * 6 + 1);
	ᮚᮦᮔ᮪ (result && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &inputRead, result,
		    (fromLen*6+1)*parimaa_na(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, result, (fromLen*6+1)*parimaa_na(char_u), &inputRead);
	    utf8_len += inputRead;
	}
	ᮜᮤᮚ
	    VIM_CLEAR(result);
    }
    ᮜᮤᮚ
    {
	result = NULL;
    }

    ᮚᮦᮔ᮪ (actualLen)
	*actualLen = result ? utf8_len : 0;

    ᮝᮜᮤ result;
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

    ᮚᮦᮔ᮪ (utf8_str == NULL)
    {
	ᮚᮦᮔ᮪ (actualLen)
	    *actualLen = 0;
	ᮝᮜᮤ NULL;
    }

    convertRange = CFRangeMake(0, CFStringGetLength(utf8_str));
    result = ALLOC_MULT(UniChar, convertRange.length);

    CFStringGetCharacters(utf8_str, convertRange, result);

    CFRelease(utf8_str);

    ᮚᮦᮔ᮪ (actualLen)
	*actualLen = convertRange.length * parimaa_na(UniChar);

    ᮝᮜᮤ result;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    void
mac_lang_init(void)
{
    ᮚᮦᮔ᮪ (mch_getenv((char_u *)"LANG") != NULL)
	ᮝᮜᮤ;

    var_na	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    ᮚᮦᮔ᮪ (LocaleRefGetPartString(NULL,
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		parimaa_na(buf) - 10, buf) == noErr && *buf)
    {
	ᮚᮦᮔ᮪ (strcasestr(buf, "utf-8") == NULL)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#ᮚᮦᮔ᮪ defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
