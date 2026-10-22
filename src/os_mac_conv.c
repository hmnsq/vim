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

#অন্তর্ভাব "vim.h"

#অন্তর্ভাব <CoreServices/CoreServices.h>


#যদি defined(MACOS_CONVERT)

স্থির char_u	    *mac_utf16_to_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
স্থির UniChar	    *mac_utf8_to_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
স্থির TECObjectRef gPathConverter;
// Converter used by mac_utf16_to_utf8
স্থির TECObjectRef gUTF16ToUTF8Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    পূর্ণসংখ্যা			len,
    পূর্ণসংখ্যা			*lenp,
    পূর্ণসংখ্যা			fail_on_error,
    পূর্ণসংখ্যা			from_enc,
    পূর্ণসংখ্যা			to_enc,
    পূর্ণসংখ্যা			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    পূর্ণসংখ্যা			buflen, in, out, l, i;
    CFStringEncoding	from;
    CFStringEncoding	to;

    নির্বাচন (from_enc)
    {
	ক্ষেত্র 'l':   from = kCFStringEncodingISOLatin1; বিরতি;
	ক্ষেত্র 'm':   from = kCFStringEncodingMacRoman; বিরতি;
	ক্ষেত্র 'u':   from = kCFStringEncodingUTF8; বিরতি;
	পূর্বনির্ধারিত:    ফেরত ফাঁকা;
    }
    নির্বাচন (to_enc)
    {
	ক্ষেত্র 'l':   to = kCFStringEncodingISOLatin1; বিরতি;
	ক্ষেত্র 'm':   to = kCFStringEncodingMacRoman; বিরতি;
	ক্ষেত্র 'u':   to = kCFStringEncodingUTF8; বিরতি;
	পূর্বনির্ধারিত:    ফেরত ফাঁকা;
    }

    যদি (unconvlenp != ফাঁকা)
	*unconvlenp = 0;
    cfstr = CFStringCreateWithBytes(ফাঁকা, ptr, len, from, 0);

    যদি (cfstr == ফাঁকা)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    যখন (cfstr == ফাঁকা && unconvlenp != ফাঁকা && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFStringCreateWithBytes(ফাঁকা, ptr, len, from, 0);
    }
    যদি (cfstr == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (to == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    অন্যথা
	buflen = len + 1;
    retval = alloc(buflen);
    যদি (retval == ফাঁকা)
    {
	CFRelease(cfstr);
	ফেরত ফাঁকা;
    }

#যদি 0
    CFRange convertRange = CFRangeMake(0, CFString_Get_দৈর্ঘ্য(cfstr));
    //  Determine output buffer size
    CFStringGetBytes(cfstr, convertRange, to, ফাঁকা, FALSE, ফাঁকা, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : ফাঁকা;
    যদি (retval == ফাঁকা)
    {
	CFRelease(cfstr);
	ফেরত ফাঁকা;
    }

    যদি (lenp)
	*lenp = buflen / আকার(char_u);

    যদি (!CFStringGetBytes(cfstr, convertRange, to, ফাঁকা, FALSE, retval, buflen, ফাঁকা))
#endif
    যদি (!CFStringGetCString(cfstr, (অক্ষর *)retval, buflen, to))
    {
	CFRelease(cfstr);
	যদি (fail_on_error)
	{
	    vim_free(retval);
	    ফেরত ফাঁকা;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	জন্য (d = retval, in = 0, out = 0; in < len && out < buflen - 1;)
	{
	    যদি (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + in);
	    অন্যথা
		l = 1;
	    cfstr = CFStringCreateWithBytes(ফাঁকা, ptr + in, l, from, 0);
	    যদি (cfstr == ফাঁকা)
	    {
		*d++ = '?';
		out++;
	    }
	    অন্যথা
	    {
		যদি (!CFStringGetCString(cfstr, (অক্ষর *)d, buflen - out, to))
		{
		    *d++ = '?';
		    out++;
		}
		অন্যথা
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
	যদি (lenp != ফাঁকা)
	    *lenp = out;
	ফেরত retval;
    }
    CFRelease(cfstr);
    যদি (lenp != ফাঁকা)
	*lenp = STRLEN(retval);

    ফেরত retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
macroman2enc(
    char_u	*ptr,
    long	*sizep,
    long	real_আকার)
{
    CFStringRef		cfstr;
    CFRange		r;
    CFIndex		len = *sizep;

    // MacRoman is an 8-bit encoding, no need to move bytes to
    // conv_rest[].
    cfstr = CFStringCreateWithBytes(ফাঁকা, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    যদি (cfstr == ফাঁকা)
	ফেরত FAIL;

    r.location = 0;
    r.দৈর্ঘ্য = CFString_Get_দৈর্ঘ্য(cfstr);
    যদি (r.দৈর্ঘ্য != CFStringGetBytes(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_আকার - *sizep, &len))
    {
	CFRelease(cfstr);
	ফেরত FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    ফেরত OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*to,
    পূর্ণসংখ্যা		*tolenp,
    পূর্ণসংখ্যা		maxtolen,
    char_u	*rest,
    পূর্ণসংখ্যা		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFStringCreateWithBytes(ফাঁকা, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    যখন (cfstr == ফাঁকা && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFStringCreateWithBytes(ফাঁকা, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    যদি (cfstr == ফাঁকা)
	ফেরত FAIL;

    r.location = 0;
    r.দৈর্ঘ্য = CFString_Get_দৈর্ঘ্য(cfstr);
    যদি (r.দৈর্ঘ্য != CFStringGetBytes(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		to, maxtolen, &l))
    {
	CFRelease(cfstr);
	ফেরত FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    ফেরত OK;
}

/*
 * Initializes text converters
 */
    শূন্য
mac_conv_init(শূন্য)
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

    যদি (TECCreateConverter(&gPathConverter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	gPathConverter = ফাঁকা;

    যদি (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	যদি (TECCreateConverter(&gUTF16ToUTF8Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    gUTF16ToUTF8Converter = ফাঁকা;
    }
}

/*
 * Destroys text converters
 */
    শূন্য
mac_conv_cleanup(শূন্য)
{
    যদি (gUTF16ToUTF8Converter)
    {
	TECDisposeConverter(gUTF16ToUTF8Converter);
	gUTF16ToUTF8Converter = ফাঁকা;
    }

    যদি (gPathConverter)
    {
	TECDisposeConverter(gPathConverter);
	gPathConverter = ফাঁকা;
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
    char_u      *ফলাফল = ফাঁকা;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_to_utf8(from, fromLen, &utf8_len);

    যদি (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	যদি ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_setup(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	যদি (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    ফলাফল = utf8_str;
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা len = utf8_len;
	    ফলাফল = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_setup(&conv, ফাঁকা, ফাঁকা);

	যদি (actualLen)
	    *actualLen = utf8_len;
    }
    অন্যথা যদি (actualLen)
	*actualLen = 0;

    ফেরত ফলাফল;
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
    UniChar     *ফলাফল = ফাঁকা;
    Boolean     should_free_utf8 = FALSE;

    করো
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	যদি ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_setup(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    বিরতি;

	যদি (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (পূর্ণসংখ্যা *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	অন্যথা
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	যদি (utf8_str == ফাঁকা)
	    বিরতি;

	convert_setup(&conv, ফাঁকা, ফাঁকা);

	ফলাফল = mac_utf8_to_utf16(utf8_str, utf8_len, actualLen);

	যদি (should_free_utf8)
	    vim_free(utf8_str);
	ফেরত ফলাফল;
    }
    যখন (0);

    যদি (actualLen)
	*actualLen = 0;

    ফেরত ফলাফল;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    শূন্য *
mac_enc_to_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	ফলাফল = ফাঁকা;

    utf16_str = mac_enc_to_utf16(from, fromLen, &utf16_len);
    যদি (utf16_str)
    {
	ফলাফল = CFStringCreateWithCharacters(ফাঁকা, utf16_str, utf16_len/আকার(UniChar));
	vim_free(utf16_str);
    }

    ফেরত (শূন্য *)ফলাফল;
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
    char_u  *ফলাফল = ফাঁকা;
    size_t  actualLen = 0;

    যদি (gPathConverter)
    {
	ফলাফল = alloc(decompLen);
	যদি (ফলাফল)
	{
	    যদি (TECConvertText(gPathConverter, decompPath,
			decompLen, &decompLen, ফলাফল,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(ফলাফল);
	}
    }

    যদি (precompLen)
	*precompLen = actualLen;

    ফেরত ফলাফল;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    স্থির char_u *
mac_utf16_to_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_গুনতি		utf8_len;
    Byte_গুনতি		প্রবেশ_Read;
    char_u		*ফলাফল;

    যদি (gUTF16ToUTF8Converter)
    {
	ফলাফল = alloc(fromLen * 6 + 1);
	যদি (ফলাফল && TECConvertText(gUTF16ToUTF8Converter, (ConstTextPtr)from,
		    fromLen, &প্রবেশ_Read, ফলাফল,
		    (fromLen*6+1)*আকার(char_u), &utf8_len) == noErr)
	{
	    TECFlushText(gUTF16ToUTF8Converter, ফলাফল, (fromLen*6+1)*আকার(char_u), &প্রবেশ_Read);
	    utf8_len += প্রবেশ_Read;
	}
	অন্যথা
	    VIM_CLEAR(ফলাফল);
    }
    অন্যথা
    {
	ফলাফল = ফাঁকা;
    }

    যদি (actualLen)
	*actualLen = ফলাফল ? utf8_len : 0;

    ফেরত ফলাফল;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    স্থির UniChar *
mac_utf8_to_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *ফলাফল = ফাঁকা;

    utf8_str = CFStringCreateWithBytes(ফাঁকা, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    যদি (utf8_str == ফাঁকা)
    {
	যদি (actualLen)
	    *actualLen = 0;
	ফেরত ফাঁকা;
    }

    convertRange = CFRangeMake(0, CFString_Get_দৈর্ঘ্য(utf8_str));
    ফলাফল = ALLOC_MULT(UniChar, convertRange.দৈর্ঘ্য);

    CFStringGetCharacters(utf8_str, convertRange, ফলাফল);

    CFRelease(utf8_str);

    যদি (actualLen)
	*actualLen = convertRange.দৈর্ঘ্য * আকার(UniChar);

    ফেরত ফলাফল;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    শূন্য
mac_lang_init(শূন্য)
{
    যদি (mch_getenv((char_u *)"LANG") != ফাঁকা)
	ফেরত;

    অক্ষর	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    যদি (LocaleRefGetPartString(ফাঁকা,
		kLocaleLanguageMask | kLocaleLanguageVariantMask |
		kLocaleRegionMask | kLocaleRegionVariantMask,
		আকার(buf) - 10, buf) == noErr && *buf)
    {
	যদি (strcasestr(buf, "utf-8") == ফাঁকা)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif
#যদি defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#endif
    }
}
#endif // MACOS_CONVERT
