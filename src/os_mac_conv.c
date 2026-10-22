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

#నిర్వచించు NO_X11_INCLUDES

#అన్తర్భావ "vim.h"

#అన్తర్భావ <CoreServices/CoreServices.h>


#ఒకవేళ defined(MACOS_CONVERT)

స్థిర char_u	    *mac_utf16_గమ్యం_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
స్థిర UniChar	    *mac_utf8_గమ్యం_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
స్థిర TECObjectRef g_మార్గం_Converter;
// Converter used by mac_utf16_to_utf8
స్థిర TECObjectRef g_UTF16_గమ్యం_UTF8_Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    పూర్ణాంకం			len,
    పూర్ణాంకం			*lenp,
    పూర్ణాంకం			fail_on_error,
    పూర్ణాంకం			from_enc,
    పూర్ణాంకం			గమ్యం_enc,
    పూర్ణాంకం			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    పూర్ణాంకం			buflen, లోపల, బహిర్గతం, l, i;
    CFStringEncoding	from;
    CFStringEncoding	గమ్యం;

    ఎంపిక (from_enc)
    {
	స్థితి 'l':   from = kCFStringEncodingISOLatin1; ఆపు;
	స్థితి 'm':   from = kCFStringEncodingMacRoman; ఆపు;
	స్థితి 'u':   from = kCFStringEncodingUTF8; ఆపు;
	అప్రమేయ:    తిరిగి ఖాళీ;
    }
    ఎంపిక (గమ్యం_enc)
    {
	స్థితి 'l':   గమ్యం = kCFStringEncodingISOLatin1; ఆపు;
	స్థితి 'm':   గమ్యం = kCFStringEncodingMacRoman; ఆపు;
	స్థితి 'u':   గమ్యం = kCFStringEncodingUTF8; ఆపు;
	అప్రమేయ:    తిరిగి ఖాళీ;
    }

    ఒకవేళ (unconvlenp != ఖాళీ)
	*unconvlenp = 0;
    cfstr = CFString_Create_With_బైట్లు(ఖాళీ, ptr, len, from, 0);

    ఒకవేళ (cfstr == ఖాళీ)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    అయితే (cfstr == ఖాళీ && unconvlenp != ఖాళీ && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFString_Create_With_బైట్లు(ఖాళీ, ptr, len, from, 0);
    }
    ఒకవేళ (cfstr == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (గమ్యం == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    లేకపోతే
	buflen = len + 1;
    retval = alloc(buflen);
    ఒకవేళ (retval == ఖాళీ)
    {
	CFRelease(cfstr);
	తిరిగి ఖాళీ;
    }

#ఒకవేళ 0
    CFRange convertRange = CFRangeMake(0, CFString_Get_నిడివి(cfstr));
    //  Determine output buffer size
    CFString_Get_బైట్లు(cfstr, convertRange, గమ్యం, ఖాళీ, FALSE, ఖాళీ, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : ఖాళీ;
    ఒకవేళ (retval == ఖాళీ)
    {
	CFRelease(cfstr);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (lenp)
	*lenp = buflen / పరిమాణం(char_u);

    ఒకవేళ (!CFString_Get_బైట్లు(cfstr, convertRange, గమ్యం, ఖాళీ, FALSE, retval, buflen, ఖాళీ))
#అయితే_ముగింపు
    ఒకవేళ (!CFStringGetCString(cfstr, (అక్షరం *)retval, buflen, గమ్యం))
    {
	CFRelease(cfstr);
	ఒకవేళ (fail_on_error)
	{
	    vim_free(retval);
	    తిరిగి ఖాళీ;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	కొరకు (d = retval, లోపల = 0, బహిర్గతం = 0; లోపల < len && బహిర్గతం < buflen - 1;)
	{
	    ఒకవేళ (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + లోపల);
	    లేకపోతే
		l = 1;
	    cfstr = CFString_Create_With_బైట్లు(ఖాళీ, ptr + లోపల, l, from, 0);
	    ఒకవేళ (cfstr == ఖాళీ)
	    {
		*d++ = '?';
		బహిర్గతం++;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (!CFStringGetCString(cfstr, (అక్షరం *)d, buflen - బహిర్గతం, గమ్యం))
		{
		    *d++ = '?';
		    బహిర్గతం++;
		}
		లేకపోతే
		{
		    i = STRLEN(d);
		    d += i;
		    బహిర్గతం += i;
		}
		CFRelease(cfstr);
	    }
	    లోపల += l;
	}
	*d = NUL;
	ఒకవేళ (lenp != ఖాళీ)
	    *lenp = బహిర్గతం;
	తిరిగి retval;
    }
    CFRelease(cfstr);
    ఒకవేళ (lenp != ఖాళీ)
	*lenp = STRLEN(retval);

    తిరిగి retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
macroman2enc(
    char_u	*ptr,
    దీర్ఘ	*sizep,
    దీర్ఘ	real_కొలత)
{
    CFStringRef		cfstr;
    CFRange		r;
    CFIndex		len = *sizep;

    // MacRoman is an 8-bit encoding, no need to move bytes to
    // conv_rest[].
    cfstr = CFString_Create_With_బైట్లు(ఖాళీ, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    ఒకవేళ (cfstr == ఖాళీ)
	తిరిగి FAIL;

    r.location = 0;
    r.నిడివి = CFString_Get_నిడివి(cfstr);
    ఒకవేళ (r.నిడివి != CFString_Get_బైట్లు(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_కొలత - *sizep, &len))
    {
	CFRelease(cfstr);
	తిరిగి FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    తిరిగి OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*గమ్యం,
    పూర్ణాంకం		*tolenp,
    పూర్ణాంకం		maxtolen,
    char_u	*rest,
    పూర్ణాంకం		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFString_Create_With_బైట్లు(ఖాళీ, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    అయితే (cfstr == ఖాళీ && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFString_Create_With_బైట్లు(ఖాళీ, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    ఒకవేళ (cfstr == ఖాళీ)
	తిరిగి FAIL;

    r.location = 0;
    r.నిడివి = CFString_Get_నిడివి(cfstr);
    ఒకవేళ (r.నిడివి != CFString_Get_బైట్లు(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		గమ్యం, maxtolen, &l))
    {
	CFRelease(cfstr);
	తిరిగి FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    తిరిగి OK;
}

/*
 * Initializes text converters
 */
    శూన్య
mac_conv_init(శూన్య)
{
    పాఠ్యం_Encoding    utf8_encoding;
    పాఠ్యం_Encoding    utf8_hfsplus_encoding;
    పాఠ్యం_Encoding    utf8_canon_encoding;
    పాఠ్యం_Encoding    utf16_encoding;

    utf8_encoding = Create_పాఠ్యం_Encoding(k_పాఠ్యం_Encoding_Unicode_Default,
	    k_పాఠ్యం_Encoding_Default_Variant, kUnicodeUTF8Format);
    utf8_hfsplus_encoding = Create_పాఠ్యం_Encoding(k_పాఠ్యం_Encoding_Unicode_Default,
	    kUnicodeHFSPlusCompVariant, kUnicodeUTF8Format);
    utf8_canon_encoding = Create_పాఠ్యం_Encoding(k_పాఠ్యం_Encoding_Unicode_Default,
	    kUnicodeCanonicalCompVariant, kUnicodeUTF8Format);
    utf16_encoding = Create_పాఠ్యం_Encoding(k_పాఠ్యం_Encoding_Unicode_Default,
	    k_పాఠ్యం_Encoding_Default_Variant, k_Unicode16_బిట్టు_Format);

    ఒకవేళ (TECCreateConverter(&g_మార్గం_Converter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	g_మార్గం_Converter = ఖాళీ;

    ఒకవేళ (TECCreateConverter(&g_UTF16_గమ్యం_UTF8_Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	ఒకవేళ (TECCreateConverter(&g_UTF16_గమ్యం_UTF8_Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    g_UTF16_గమ్యం_UTF8_Converter = ఖాళీ;
    }
}

/*
 * Destroys text converters
 */
    శూన్య
mac_conv_cleanup(శూన్య)
{
    ఒకవేళ (g_UTF16_గమ్యం_UTF8_Converter)
    {
	TECDisposeConverter(g_UTF16_గమ్యం_UTF8_Converter);
	g_UTF16_గమ్యం_UTF8_Converter = ఖాళీ;
    }

    ఒకవేళ (g_మార్గం_Converter)
    {
	TECDisposeConverter(g_మార్గం_Converter);
	g_మార్గం_Converter = ఖాళీ;
    }
}

/*
 * Conversion from UTF-16 UniChars to 'encoding'
 * The function signature uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    char_u *
mac_utf16_గమ్యం_enc(
    గుర్తులేని పొట్టి *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    char_u      *ఫలితం = ఖాళీ;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_గమ్యం_utf8(from, fromLen, &utf8_len);

    ఒకవేళ (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	ఒకవేళ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_అమరిక(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	ఒకవేళ (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    ఫలితం = utf8_str;
	}
	లేకపోతే
	{
	    పూర్ణాంకం len = utf8_len;
	    ఫలితం = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_అమరిక(&conv, ఖాళీ, ఖాళీ);

	ఒకవేళ (actualLen)
	    *actualLen = utf8_len;
    }
    లేకపోతే ఒకవేళ (actualLen)
	*actualLen = 0;

    తిరిగి ఫలితం;
}

/*
 * Conversion from 'encoding' to UTF-16 UniChars
 * The function return uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    గుర్తులేని పొట్టి *
mac_enc_గమ్యం_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    UniChar     *ఫలితం = ఖాళీ;
    Boolean     should_free_utf8 = FALSE;

    చేయి
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	ఒకవేళ ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_అమరిక(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    ఆపు;

	ఒకవేళ (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (పూర్ణాంకం *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	లేకపోతే
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	ఒకవేళ (utf8_str == ఖాళీ)
	    ఆపు;

	convert_అమరిక(&conv, ఖాళీ, ఖాళీ);

	ఫలితం = mac_utf8_గమ్యం_utf16(utf8_str, utf8_len, actualLen);

	ఒకవేళ (should_free_utf8)
	    vim_free(utf8_str);
	తిరిగి ఫలితం;
    }
    అయితే (0);

    ఒకవేళ (actualLen)
	*actualLen = 0;

    తిరిగి ఫలితం;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    శూన్య *
mac_enc_గమ్యం_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	ఫలితం = ఖాళీ;

    utf16_str = mac_enc_గమ్యం_utf16(from, fromLen, &utf16_len);
    ఒకవేళ (utf16_str)
    {
	ఫలితం = CFStringCreateWithCharacters(ఖాళీ, utf16_str, utf16_len/పరిమాణం(UniChar));
	vim_free(utf16_str);
    }

    తిరిగి (శూన్య *)ఫలితం;
}

/*
 * Converts a decomposed HFS+ UTF-8 path to precomposed UTF-8
 */
    char_u *
mac_precompose_మార్గం(
    char_u  *decomp_మార్గం,
    size_t  decompLen,
    size_t  *precompLen)
{
    char_u  *ఫలితం = ఖాళీ;
    size_t  actualLen = 0;

    ఒకవేళ (g_మార్గం_Converter)
    {
	ఫలితం = alloc(decompLen);
	ఒకవేళ (ఫలితం)
	{
	    ఒకవేళ (TECConvert_పాఠ్యం(g_మార్గం_Converter, decomp_మార్గం,
			decompLen, &decompLen, ఫలితం,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(ఫలితం);
	}
    }

    ఒకవేళ (precompLen)
	*precompLen = actualLen;

    తిరిగి ఫలితం;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    స్థిర char_u *
mac_utf16_గమ్యం_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_లెక్క		utf8_len;
    Byte_లెక్క		ఆదానం_Read;
    char_u		*ఫలితం;

    ఒకవేళ (g_UTF16_గమ్యం_UTF8_Converter)
    {
	ఫలితం = alloc(fromLen * 6 + 1);
	ఒకవేళ (ఫలితం && TECConvert_పాఠ్యం(g_UTF16_గమ్యం_UTF8_Converter, (Const_పాఠ్యం_Ptr)from,
		    fromLen, &ఆదానం_Read, ఫలితం,
		    (fromLen*6+1)*పరిమాణం(char_u), &utf8_len) == noErr)
	{
	    TECFlush_పాఠ్యం(g_UTF16_గమ్యం_UTF8_Converter, ఫలితం, (fromLen*6+1)*పరిమాణం(char_u), &ఆదానం_Read);
	    utf8_len += ఆదానం_Read;
	}
	లేకపోతే
	    VIM_CLEAR(ఫలితం);
    }
    లేకపోతే
    {
	ఫలితం = ఖాళీ;
    }

    ఒకవేళ (actualLen)
	*actualLen = ఫలితం ? utf8_len : 0;

    తిరిగి ఫలితం;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    స్థిర UniChar *
mac_utf8_గమ్యం_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *ఫలితం = ఖాళీ;

    utf8_str = CFString_Create_With_బైట్లు(ఖాళీ, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    ఒకవేళ (utf8_str == ఖాళీ)
    {
	ఒకవేళ (actualLen)
	    *actualLen = 0;
	తిరిగి ఖాళీ;
    }

    convertRange = CFRangeMake(0, CFString_Get_నిడివి(utf8_str));
    ఫలితం = ALLOC_MULT(UniChar, convertRange.నిడివి);

    CFStringGetCharacters(utf8_str, convertRange, ఫలితం);

    CFRelease(utf8_str);

    ఒకవేళ (actualLen)
	*actualLen = convertRange.నిడివి * పరిమాణం(UniChar);

    తిరిగి ఫలితం;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    శూన్య
mac_lang_init(శూన్య)
{
    ఒకవేళ (mch_getenv((char_u *)"LANG") != ఖాళీ)
	తిరిగి;

    అక్షరం	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    ఒకవేళ (LocaleRefGetPartString(ఖాళీ,
		k_Locale_Language_ముసుగు | k_Locale_Language_Variant_ముసుగు |
		k_Locale_Region_ముసుగు | k_Locale_Region_Variant_ముసుగు,
		పరిమాణం(buf) - 10, buf) == noErr && *buf)
    {
	ఒకవేళ (strcasestr(buf, "utf-8") == ఖాళీ)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#నిర్వచితమైతే HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#అయితే_ముగింపు
#ఒకవేళ defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#అయితే_ముగింపు
    }
}
#అయితే_ముగింపు // MACOS_CONVERT
